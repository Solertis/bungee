/*
python-module-bungee.c: Bungee module

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
#include "python-bungee-globals.h"

static PyObject *mod_bungee; /* hold a reference Bungee module imported by mod_bungee_init */

/************* Bungee Primitives ***************/
static PyObject* emb_bng_version (PyObject *self, PyObject *args);

static PyMethodDef BungeeMethods[] = {
  {"version", emb_bng_version, METH_VARARGS,
   N_("Get Bungee version string.")},
  {NULL, NULL, 0, NULL}
};

/* Take no argument. Returns current PACKAGE_VERSION as unicode string */
static PyObject*
emb_bng_version (PyObject *self, PyObject *args)
{
  /* string after : is used as function name in error messages */
  if(!PyArg_ParseTuple(args, ":version"))
    {
      BNG_DBG (_("Error parsing Bungee.version() tuple"));
      Py_RETURN_NONE;
    }
  return PyUnicode_FromString (VERSION);
}

/****************************************/
/* >>>> Insert new primitives here <<<< */
/****************************************/

/************* Bungee Module ***************/
static PyObject* PyInit_bungee(void);

static PyModuleDef BungeeModule = {
  PyModuleDef_HEAD_INIT, "Bungee", NULL, -1, BungeeMethods,
  NULL, NULL, NULL, NULL
};

/* Called at the time of instantiation of the object */
static PyObject*
PyInit_bungee(void)
{
  return PyModule_Create (&BungeeModule);
}

/* Imports Bungee module to __main__ and returns a reference to Bungee module */
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
  PyObject *_module_str = PyUnicode_FromString ("Bungee");
  _mod_bungee = PyImport_Import (_module_str);
  PyObject_SetAttrString (_mod_main, "Bungee", _mod_bungee);
  Py_DECREF (_module_str);

  return (_mod_bungee);
}

/* Register Bungee module to standard modules table.
   THIS FUNCTION SHOULD BE CALLED BEFORE PyInitialize(). */
gint
mod_bungee_register (void)
{
  if (PyImport_AppendInittab ("Bungee", &PyInit_bungee) == -1)
    return (-1);
  else
    return (0); /* Python return values are not consistent. Some functions return 1 for success. */
}

gint
mod_bungee_init ()
{
  mod_bungee = import_mod_bungee ();
  if (mod_bungee == NULL)
    {
      BNG_DBG (_("Unable to import Bungee module."));
      return (-1);
    }

  if (bungee_globals_init () != 0)
    {
      BNG_DBG (_("Unable to initialize _globals dictionary."));
      return (-1);
    }

  return (0);
}

gint
mod_bungee_fini ()
{
  Py_DECREF (mod_bungee);

  if (bungee_globals_fini () != 0)
    {
      BNG_DBG (_("Unable to uninitialize _globals dictionary."));
      return (-1);
    }

  return (0);
}
