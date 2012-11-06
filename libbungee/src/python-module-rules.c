/*
python-module-rules.c: RULES module

This file is part of Bungee.

Copyright 2012 Red Hat, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/* Python.h should be the first header to include, even before system headers */
#include <Python.h>
#include <glib.h>

#include "local-defs.h"
#include "logger.h"


/************* RULES PRIMITIVES ***************/
static PyObject* emb_append_rule (PyObject *self, PyObject *args);


/************* GLOBAL DATA STRUCTURES ***************/
static PyObject *mod_rules; /* hold a reference RULE module imported by mod_rule_init */

/* Glib's Keyed Data List - It can work as associate array or a simple
   hash table. Strings are conerted to (and back from) quarks (a unique
   integer) using a two-way hash
   - group_table groups rules in to rule_table indexed by group_name.
   - rule_table contains rules indexed by rule_name.

    +=============+
    | GROUP_TABLE |
    +=============+
    | group_nameA |----> +============+
    +-------------+	 | RULE_TABLE |
    | group_nameB |	 +============+
    +-------------+	 | rule_nameA |----> { condition(), action() }
    |    ...      |	 +------------+
    +-------------+	 | rule_nameB |
			 +------------+
			 |    ...     |
			 +------------+

*/
static GData *group_table;

/* String key to GLOBAL rule_table in group_table. This static const string
   is used for quark id lookup optimization. */
static const gchar *global_group_name = "GLOBAL";

typedef struct
{
  // gchar *group_name; /* You can find group_name by converting quark from group_table. */
  // gchar *rule_name; /* You can find rule_name by converting quark from rule_table. */
  PyObject *condt; /*  Function pointer to condition. */
  PyObject *action; /* Function pointer to action. */
} rule_t;

static PyMethodDef RulesMethods[] =
  {
    {"APPEND", emb_append_rule, METH_VARARGS,
     N_("Append a new RULE.")},
    {NULL, NULL, 0, NULL}
  };


/************* MISC ROUTINES *************/

/* Pointer to group_table entry containing all the default global rules with
   "GLOBAL" string as key. */
static GQuark get_global_rule_table_id (void)
{
  static GQuark global_rule_table_id;

  if (!global_rule_table_id)
    global_rule_table_id = g_quark_from_static_string (global_group_name);

  return global_rule_table_id;
}

/* Pointer to group_table entry containing all the default global rules with
   "GLOBAL" string as key. */
static inline GData * get_global_rule_table (void)
{
  return g_dataset_id_get_data (group_table, get_global_rule_table_id ());
}

/* Destroy function fo rule_table stored in group_table */
void rule_table_destroy (void *rule_table)
{
  GData *_rule_table = (GData *) rule_table;
  g_dataset_destroy (_rule_table);
}

/* Destroy function fo rule stored in rule_table */
void rule_destroy (void *rule)
{
  rule_t *_rule = (rule_t *) rule;

  Py_XDECREF (_rule->condt);  /* Decrement a reference to new callback */
  Py_XDECREF (_rule->action);  /* Decrement a reference to new callback */
}


/****************************************/
/* >>>> Insert new primitives here <<<< */
/****************************************/
/*
  # RULES.APPEND('GroupName', 'RuleName', '''CONDITION()''', 'ACTION()'))

  RULES.APPEND primitive appends a new rule to the rule table. It uses event
  driven programming model, where condition determines the action.

  Arguments:
  ----------
  GROUP_NAME - Group name as string.
  RULE_NAME  - Rule name as string.
  CONDITION()- Condition should evaluate to boolean.
             - Condition() is a callback Python function. It takes no
	       argument and returns bool.
  ACTION()   - action() is callback python function. It takes dictionary
               object returned by previous RULE or INPUT as argument and
	       returns a new dictionary (may be same or modified).

  Returns:
  --------
  Returns the a FLOW tuple as dictated by ACTION or False upon failure.
 */
static PyObject*
emb_append_rule (PyObject *self, PyObject *args)
{
  gchar *group_name, *rule_name;
  rule_t rule;

  if(!PyArg_ParseTuple (args, "ssO!O!:APPEND", &group_name, &rule_name,
			&PyFunction_Type, &rule.condt,
			&PyFunction_Type, &rule.action))
    {
      BNG_DBG (_("Error parsing RULES.APPEND(...) rule."));
    }

/* Let PyArg_ParseTuple do the validation via O!. */
/*
  if (!PyFunction_Check (&rule.condt))
    {
      BNG_DBG (_("CONDT is not a callable function in RULE \"%s\".", rule_name));
      Py_RETURN_NONE;
    }

  if (!PyFunction_Check (&rule.action))
    {
      BNG_DBG (_("ACTION is not a callable function in RULE \"%s\".", rule_name));
      Py_RETURN_NONE;
    }
*/

  Py_XINCREF(rule.condt);  /* Increment a reference to new callback */
  Py_XINCREF(rule.action); /* Increment a reference to new callback */

  GData **rule_table = g_dataset_get_data (&group_table, group_name);
  if (!rule_table)  /* Initialize this rule_table. */
    {
      g_datalist_init (rule_table);
      /* Insert this rule under rule_name into rule_table. */
      g_datalist_set_data_full (rule_table, rule_name, &rule, rule_destroy);
      /* Insert this rule_table into group_table under group_name. */
      g_datalist_set_data_full (&group_table, group_name, rule_table, rule_table_destroy);
    }
  else /* rule_table already exists under this group_name. */
    {
      /* Add this rule under rule_name into rule_table. */
      g_datalist_set_data_full (rule_table, rule_name, &rule, rule_destroy);
    }

  Py_RETURN_TRUE;
}

/************* RULES MODULE ***************/
static PyObject* PyInit_rules (void);

static PyModuleDef RulesModule = {
  PyModuleDef_HEAD_INIT, "RULES", NULL, -1, RulesMethods,
  NULL, NULL, NULL, NULL
};

/* Called at the time of instantiation of the object */
static PyObject*
PyInit_rules(void)
{
  return PyModule_Create (&RulesModule);
}

/* Imports RULES module to __main__ and returns a reference to RULES module */
static PyObject *
import_mod_rules (void)
{
  PyObject *_mod_main; /* __main__ module */
  PyObject *_mod_rules;

  /* Barrowed reference to main module*/
  _mod_main = PyImport_AddModule ("__main__");
  if (_mod_main == NULL)
    {
      BNG_ERR (_("Could not access __main__ module"));
      return (NULL);
    }

  /* Make our "rules" module available to Python's main to import */
  PyObject *_module_str = PyUnicode_FromString ("RULES");
  _mod_rules = PyImport_Import (_module_str);
  PyObject_SetAttrString (_mod_main, "RULES", _mod_rules);
  Py_DECREF (_module_str);

  return (_mod_rules);
}

/* Register RULES module to standard modules table. This function should be called before PyInitialize(). */
gint
mod_rules_register (void)
{
  if (PyImport_AppendInittab ("RULES", &PyInit_rules) == -1)
    return (-1);
  else
    return (0); /* Python return values are not consistent. Some functions return 1 for success. */
}

gint
mod_rules_init ()
{
  g_datalist_init (&group_table);

  mod_rules = import_mod_rules ();
  if (mod_rules == NULL)
    return (-1);

  return (0);
}

gint
mod_rules_fini ()
{
  /* Empty our RULE table. */
  // g_datalist_destroy (&group_table);
  g_datalist_clear (&group_table);

  /* Decrement our refernce to mod_rules. */
  Py_XDECREF (mod_rules);
  return (0);
}
