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

#include "local-defs.h"
#include "bungee.h"

/**************************/
/* Evaluate bungee script */
/**************************/
gint
bng_eval (const gchar *code)
{
  return (PyRun_SimpleString (code));
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
      BNG_WARNING (_("Unable to read [%s], %s"), _bng_script, strerror (errno));
      status = 1;
      goto END;
    }

  status = PyRun_SimpleFileEx (pyscript, _bng_script, TRUE);

 END:
  wordfree (&exp_bng_script);
  return (status);
}


/***************************************/
/* Execute bungee startup file	       */
/* if bng_rc is NULL, load ~/.bungeerc */
/***************************************/
static gint
bng_load_rc (const gchar *bng_rc)
{
  if (bng_rc)
    {
      if (bng_load (bng_rc) != 0)
	{
	  BNG_WARNING (_("Error loading startup file [%s]"), bng_rc);
	  return (1);
	}
    }
  else
    {
      if (bng_load ("~/"BNG_RC) != 0)
	{
	  BNG_WARNING (_("Error loading startup file [~/"BNG_RC"]"));
	  return (1);
	}
    }  return (0);
}

/*********************************/
/* Initialize bungee environment */
/*********************************/
gint
bng_init (const gchar *bng_script)
{
  if (PY_MAJOR_VERSION < 3)
    {
      BNG_ERROR (_("Requires Python version 3 or above"));
      return 1;
    }

  Py_Initialize();

  bng_load_rc (bng_script);

  return (0);
}

/***************************/
/* Free bungee environment */
/***************************/
gint
bng_fini (void)
{
  Py_Finalize();

  return (0);
}
