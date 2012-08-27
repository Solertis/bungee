/*
python-module-bungee.c: BUNGEE module

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

static PyObject *mod_bungee; /* hold a reference bungee module imported by mod_bungee_init */

/************* BUNGEE PRIMITIVES ***************/
static PyObject* emb_bng_version (PyObject *self, PyObject *args);

static PyMethodDef BungeeMethods[] = {
  {"VERSION", emb_bng_version, METH_VARARGS,
   N_("Get "PACKAGE" version string.")},
  {NULL, NULL, 0, NULL}
};

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

/****************************************/
/* >>>> Insert new primitives here <<<< */
/****************************************/

/************* BUNGEE MODULE ***************/
static PyObject* PyInit_bungee(void);

static PyModuleDef BungeeModule = {
  PyModuleDef_HEAD_INIT, "BUNGEE", NULL, -1, BungeeMethods,
  NULL, NULL, NULL, NULL
};

/* Called at the time of instantiation of the object */
static PyObject*
PyInit_bungee(void)
{
  return PyModule_Create (&BungeeModule);
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
