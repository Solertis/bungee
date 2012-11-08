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
#include "python-module-bungee.h"
#include "python-module-rules.h"
#include "python-embedding.h"

/*
  Invokes a python procedure and returns its value. Caller assumes the
  responsibility of freeing the return value with Py_XDECREF or Py_DECREF.
 */
PyObject *
bng_py_hook_call (const gchar *hook_name, char *format, ...)
{
  if (hook_name == NULL)
    {
      errno = EINVAL;
      BNG_DBG (_(PACKAGE" main module is not initialized"));
      return (NULL);
    }

  PyObject *py_hook, *py_result;
  va_list args;
  PyObject *_mod_main; /* __main__ module */
  PyObject *_main_dict;

  /* Barrowed reference to main module*/
  _mod_main = PyImport_AddModule ("__main__");
  if (_mod_main == NULL)
    {
      BNG_WARN (_("Could not access __main__ module"));
      return (NULL);
    }

  /* Borrowed reference to main dict */
  _main_dict = PyModule_GetDict (_mod_main);

  /* Borrowed reference to "hook_name" from the global dictionary */
  py_hook = PyDict_GetItemString (_main_dict, hook_name);

  if (py_hook == NULL)
    {
      BNG_DBG (_("[%s] hook function is not declared"), hook_name);
      return (NULL);
    }

  if (PyCallable_Check (py_hook) == 0)
    {
      BNG_WARN (_("[%s] hook function is not callable"), hook_name);
      return (NULL);
    }

  if (format && *format)
    {
      va_start (args, format);
      py_result = PyObject_CallFunction (py_hook, format, args, NULL);
      va_end (args);
      return (py_result);
    }
  else
    {
      py_result = PyObject_CallFunction (py_hook, NULL);
      return (py_result);
    }
}

gint
bng_py_init (void)
{
  if (mod_bungee_register () != 0)
    {
      BNG_ERR (_("Module BUNGEE registration failed."));
      return (-1);
    }

  if (mod_rules_register () != 0)
    {
      BNG_ERR (_("Module RULES registration failed."));
      return (-1);
    }

  Py_Initialize ();

  if (mod_bungee_init () != 0)
    {
      BNG_ERR (_("Unable to initialize BUNGEE module."));
      return (-1);
    }

  if (mod_rules_init () != 0)
    {
      BNG_ERR (_("Unable to initialize RULES module."));
      return (-1);
    }

  return (0);
}

gint
bng_py_fini (void)
{
  if (mod_bungee_fini () != 0)
    {
      BNG_WARN (_("Unable to uninitialize BUNGEE module"));
      return (-1);
    }

  if (mod_rules_fini () != 0)
    {
      BNG_WARN (_("Unable to uninitialize RULES module"));
      return (-1);
    }

  return (0);
}
