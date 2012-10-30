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

static PyObject *mod_rules; /* hold a reference RULE module imported by mod_rule_init */

/************* BUNGEE PRIMITIVES ***************/
static PyObject* emb_rules_append (PyObject *self, PyObject *args);

static PyMethodDef BungeeMethods[] = {
  {"ADD", emb_rules_append, METH_VARARGS,
   N_("Append a new RULE.")},
  {NULL, NULL, 0, NULL}
};

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
emb_rules_append (PyObject *self, PyObject *args)
{
  gchar *group_name, *rule_name, *condt, *action;
  if(!PyArg_ParseTuple (args, "ssss:APPEND", &group_name, &rule_name, &condt, &action))
    BNG_DBG (_("Error parsing RULES.APPEND(...) rule."));

  Py_RETURN_NONE;
}

/****************************************/
/* >>>> Insert new primitives here <<<< */
/****************************************/

/************* BUNGEE MODULE ***************/
static PyObject* PyInit_bungee (void);

static PyModuleDef RulesModule = {
  PyModuleDef_HEAD_INIT, "RULES", NULL, -1, BungeeMethods,
  NULL, NULL, NULL, NULL
};

/* Called at the time of instantiation of the object */
static PyObject*
PyInit_bungee(void)
{
  return PyModule_Create (&RulesModule);
}

/* Imports BUNGEE module to __main__ and returns a reference to BUNGEE module */
static PyObject *
import_mod_bungee (void)
{
  PyObject *_mod_main; /* __main__ module */
  PyObject *_mod_bungee;

  /* Barrowed reference to main module*/
  _mod_main = PyImport_AddModule ("__main__");
  if (_mod_main == NULL)
    {
      BNG_WARN (_("Could not access __main__ module"));
      return (NULL);
    }

  /* Make our "bungee" module available to Python's main to import */
  PyObject *_bng_module_str = PyUnicode_FromString ("BUNGEE");
  _mod_bungee = PyImport_Import (_bng_module_str);
  PyObject_SetAttrString (_mod_main, "BUNGEE", _mod_bungee);
  Py_DECREF (_bng_module_str);

  return (_mod_bungee);
}

/* Register BUNGEE module to standard modules table. This function should be called before PyInitialize(). */
gint
mod_bungee_register (void)
{
  if (PyImport_AppendInittab ("BUNGEE", &PyInit_bungee) == -1)
    return (-1);
  else
    return (0); /* Python return values are not consistent. Some functions return 1 for success. */
}

gint
mod_bungee_init ()
{
  mod_bungee = import_mod_bungee ();
  if (mod_bungee == NULL)
    return (-1);
  return (0);
}

gint
mod_bungee_fini ()
{
  Py_DECREF (mod_bungee);
  return (0);
}
