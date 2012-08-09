/*
bungee.c: core bungee routines

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
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <wordexp.h>
#include <glib.h>

#include "local-defs.h"
#include "logger.h"
#include "python-embedding.h"

/* Evaluate bungee code */
gint
bng_eval (const gchar *code)
{
  if (code == NULL)
    {
      BNG_DBG (_("Invalid argument, code is NULL"));
      errno = EINVAL;
      return (1);
    }

  if (PyRun_SimpleString (code) == 0)
    return (0);
  else
    return (1);
}

/*************************/
/* Load bungee extension */
/*************************/
gint
bng_load (const gchar *bng_script)
{
  struct stat stat_buf;
  wordexp_t exp_bng_script;
  const gchar *_bng_script = NULL;
  gint status = 0;

  if (wordexp(bng_script, &exp_bng_script, 0) == 0)
    _bng_script = exp_bng_script.we_wordv[0];
  else
    _bng_script = bng_script;

  if (stat (_bng_script, &stat_buf) !=0)
    {
      status = 1; /* File likely doesn't exist */
      goto END;
    }

  if (stat_buf.st_size == 0)
    {
      status = 1;
      goto END;
    }

  FILE* pyscript = fopen (_bng_script, "r");
  if (pyscript == NULL)
    {
      BNG_DBG (_("Unable to read [%s], %s"), _bng_script, strerror (errno));
      status = 1;
      goto END;
    }

  status = PyRun_SimpleFileEx (pyscript, _bng_script, TRUE);

 END:
  wordfree (&exp_bng_script);
  return (status);
}


/*********************************/
/* Initialize bungee environment */
/*********************************/
gint
bng_init (bng_console_t msg, bng_console_t log)
{
  bng_console_init (msg, log);

  if (PY_MAJOR_VERSION < 3)
    {
      BNG_ERR (_("Requires Python version 3 or above"));
      return 1;
    }

  bng_py_init ();

  return (0);
}

/***************************/
/* Free bungee environment */
/***************************/
gint
bng_fini (void)
{
  bng_py_fini ();
  Py_Finalize ();

  return (0);
}


/*********************************/
/* Bungee core execution loop    */
/*********************************/
gint
bng_engine (void)
{
  /* BEGIN hook is optional */
  bng_py_hook_call (BNG_HOOK_BEGIN, NULL);

  /*
    Heart of Bungee!. As data flows from INPUT hook, call MATCH and TARGET appropriately.
   */

  /* END hook is optional */
  bng_py_hook_call (BNG_HOOK_END, NULL);

  return (0);
}

/*********************************/
/* Bungee core execution loop    */
/*********************************/
gint
bng_run (const gchar *bng_script)
{
  gint status = 0;
  status = bng_load (bng_script);
  if (status != 0)
  {
    BNG_DBG (_("Error loading "PACKAGE" script [%s]"), bng_script);
    return (status);
  }

  if (bng_engine () != 0)
  {
    BNG_DBG (_("Error executing "PACKAGE" script [%s]"), bng_script);
    return (1);
  }

  return (0);
}
