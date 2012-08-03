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
#include "python-embedding.h"

static PyObject*
emb_bng_version (PyObject *self, PyObject *args)
{
  if(!PyArg_ParseTuple(args, ":version"))
    {
      BNG_WARNING (_("Error calling bng_version"));
      return NULL;
    }
  return PyUnicode_FromString (VERSION);
}

static PyMethodDef EmbMethods[] = {
  {"bng_version", emb_bng_version, METH_VARARGS,
   N_("Get "PACKAGE" version string.")},
  {NULL, NULL, 0, NULL}
};

static PyModuleDef EmbModule = {
  PyModuleDef_HEAD_INIT, "bungee", NULL, -1, EmbMethods,
  NULL, NULL, NULL, NULL
};

static PyObject*
PyInit_emb(void)
{
  return PyModule_Create(&EmbModule);
}
