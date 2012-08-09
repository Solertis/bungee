/*
python-embedding.c: Use python as extension language.

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
#include "python-embedding.h"

typedef struct {
  PyObject *main; /* __main__ module */
  PyObject *bungee; /* bungee module: automatucally imported into __main__ */
} bng_py_modules_t;

static bng_py_modules_t bng_py_modules;

static PyObject*
emb_bng_version (PyObject *self, PyObject *args)
{
  if(!PyArg_ParseTuple(args, ":version"))
    {
      BNG_WARN (_("Error parsing version() tuple"));
      return NULL;
    }
  return PyUnicode_FromString (VERSION);
}

static PyMethodDef BungeeMethods[] = {
  {"version", emb_bng_version, METH_VARARGS,
   N_("Get "PACKAGE" version string.")},
  {NULL, NULL, 0, NULL}
};

static PyModuleDef BungeeModule = {
  PyModuleDef_HEAD_INIT, "bungee", NULL, -1, BungeeMethods,
  NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_bungee(void)
{
  return PyModule_Create (&BungeeModule);
}

static gint
register_module_bungee (void)
{
  /* Make our module "bungee" available to Python's main to import */
  if (PyImport_AppendInittab ("bungee", &PyInit_bungee) == -1)
    return (-1);
  else
    return (0); /* Python return values are not consistent. Some functions return 1 for success. */
}

static gint
bng_register_primitives (void)
{
  PyObject *_bng_module_str = PyUnicode_FromString ("bungee");
  bng_py_modules.bungee = PyImport_Import (_bng_module_str);
  PyObject_SetAttrString (bng_py_modules.main, "bungee", bng_py_modules.bungee);
  Py_DECREF (_bng_module_str);

  return (0);
}

gint
bng_py_init (void)
{
  /* Import "bungee" module automatically. This step must be performed before PyInitialize (). */
  if (register_module_bungee () != 0)
    {
      BNG_WARN (_(PACKAGE" registration failed"));
      return (1);
    }

  Py_Initialize ();

  memset (&bng_py_modules, 0, sizeof (bng_py_modules));
  /* Barrowed reference to main module*/
  bng_py_modules.main = PyImport_AddModule ("__main__");
  if (bng_py_modules.main == NULL)
    {
      BNG_WARN (_("Could not access __main__ module"));
      return (1);
    }

  /* Register our bungee primitives */
  if (bng_register_primitives () != 0)
    {
      BNG_WARN (_("Unable to register "PACKAGE" primitives"));
      return (1);
    }

  return (0);
}

/*
gint
bng_py_hook (const gchar *hook_name)
{
  if (bng_py_modules.main == NULL)
    {
      BNG_WARN (_(PACKAGE" main module has been initialized yet"));
      return (1);
    }

  if (!PyObject_HasAttrString (bng_py_modules.main, hook_name))
    {
      BNG_DBG (_(PACKAGE" [%s] hook is missing"), hook_name);
      return (1);
    }

  PyObject *py_hook=NULL, *py_result=NULL;

  py_hook = PyObject_GetAttrString (bng_py_modules.main, hook_name);
  if (py_hook && PyCallable_Check (py_hook))
    {
      py_result = PyObject_CallFunction (py_hook, NULL);
      Py_XDECREF (py_result);
    }
  else
    {
      BNG_DBG (_("[%s] hook is not a callable function"), hook_name);
      Py_XDECREF (py_hook);
      return (1);
    }

  Py_XDECREF (py_hook);
  return (0);
}
*/

gint
bng_py_hook_call (const gchar *hook_name, char *format, ...)
{
  if (bng_py_modules.main == NULL)
    {
      BNG_WARN (_(PACKAGE" main module is not initialized"));
      return (1);
    }

  PyObject *py_hook = NULL, *py_result = NULL;
  va_list args;
  PyObject *main_dict = NULL;

  /* Borrowed reference to main dict */
  main_dict = PyModule_GetDict (bng_py_modules.main);

  /* Borrowed reference to "hook_name" from the global dictionary */
  py_hook = PyDict_GetItemString (main_dict, hook_name);

  if (py_hook == NULL)
    {
      BNG_DBG (_("[%s] hook function is not declared"), hook_name);
      return (1);
    }

  if (PyCallable_Check (py_hook) == 0)
    {
      BNG_WARN (_("[%s] hook function is not callable"), hook_name);
      return (1);
    }

  if (format && *format)
    {
      va_start (args, format);
      py_result = PyObject_CallFunction (py_hook, format, args, NULL);
      Py_XDECREF (py_result);
      va_end (args);
    }
  else
    {
      py_result = PyObject_CallFunction (py_hook, NULL);
      Py_XDECREF (py_result);
    }

  return (0);
}

gint
bng_py_fini (void)
{
  Py_XDECREF (bng_py_modules.bungee);
  return (0);
}
