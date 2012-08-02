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

#include "bungee.h"

static gchar *bng_rc = NULL;

/*************************/
/* Load bungee extension */
/*************************/
gint
bng_load (gchar *path)
{
  struct stat stat_buf;
  wordexp_t exp_path;
  gchar *_path = NULL;
  gint status = 0;

  if (wordexp(path, &exp_path, 0) == 0)
    _path = exp_path.we_wordv[0];
  else
    _path = path;

  if (stat (_path, &stat_buf) !=0)
    {
      status = 1; /* File likely doesn't exist */
      goto END;
    }

  if (stat_buf.st_size == 0)
    {
      status = 1;
      goto END;
    }

  FILE* pyscript = fopen (_path, "r");
  if (pyscript == NULL)
    {
      BNG_WARNING ("Unable to read [%s], %s", _path, strerror (errno));
      status = 1;
      goto END;
    }

  status = PyRun_SimpleFileEx (pyscript, _path, TRUE);

 END:
  wordfree (&exp_path);
  return (status);
}

/***************************************/
/* Set a different bungee startup file */
/***************************************/
gint
bng_set_rc (gchar *path)
{
  bng_rc = path;
}

/*******************************/
/* Execute bungee startup file */
/*******************************/
gint
bng_load_rc (void)
{
  gchar *bng_rc_path = NULL;

  if (bng_rc)
    {
      if (bng_load (bng_rc) != 0)
	{
	  BNG_WARNING ("Error loading startup file [%s]", bng_rc);
	  return (1);
	}
    }
  else
    {
      bng_rc_path = g_build_filename (g_get_home_dir(), BNG_RC , NULL);
      if (bng_rc_path == NULL)
	{
	  BNG_WARNING ("Unable to expand ["BNG_RC"] startup path");
	  return 1;
	}

      if (bng_load (bng_rc_path) != 0)
	{
	  BNG_WARNING ("Error loading startup file [%s]", bng_rc_path);
	  g_free (bng_rc_path);
	  return (1);
	}
      g_free (bng_rc_path);
    }
  return (0);
}

/*********************************/
/* Initialize bungee environment */
/*********************************/
gint
bng_init (void)
{
  if (PY_MAJOR_VERSION < 3)
    {
      BNG_ERROR ("Requires Python version 3 or above");
      return 1;
    }

  Py_Initialize();

  bng_load_rc ();

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
