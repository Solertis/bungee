/*
python-globals.c: Bungee python global variables

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

/* hold a reference GLOBALS dictionary. All $variables will be stored in this dictionary. */
static PyObject *_globals;

gint
globals_init ()
{
  _globals = PyDict_New ();
  if ((_globals == NULL) || !(PyDict_CheckExact (_globals)))
    return (-1);

  PyObject *_mod_main; /* __main__ module */

  /* Barrowed reference to main module*/
  _mod_main = PyImport_AddModule ("__main__");
  if (_mod_main == NULL)
    {
      BNG_WARN (_("Could not access __main__ module"));
      return (1);
    }

  /* Make our "_globals" dictionary available to Python's main. */
  PyObject_SetAttrString (_mod_main, "GLOBALS", _globals);

  return (0);
}

gint
globals_fini ()
{
  Py_DECREF (_globals);
  return (0);
}
