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
bungee_globals_init ()
{
  _globals = PyDict_New ();
  if ((_globals == NULL) || !(PyDict_CheckExact (_globals)))
    return (-1);

  PyObject *mod_bungee; /* __main__ module */

  /* Barrowed reference to main module*/
  mod_bungee = PyImport_AddModule ("Bungee");
  if (mod_bungee == NULL)
    {
      BNG_WARN (_("Could not access Bungee module"));
      return (1);
    }

  /* Make our "_globals" dictionary available through BUNGEE module. */
  PyObject_SetAttrString (mod_bungee, "_globals", _globals);

  return (0);
}

gint
bungee_globals_fini ()
{
  Py_DECREF (_globals);
  return (0);
}
