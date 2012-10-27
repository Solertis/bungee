/*
  parser-interface.c: Interface to bison parser

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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "local-defs.h"
#include "logger.h"
#include "parser.h"

/* Compile Bungee [file].bng script to [file].bngo output. */
gint
bng_compile_file (const gchar *path, FILE *err_fp)
{
  gint status = 0;
  if (!path || !path[0])
    {
      BNG_DBG (_("File name required.\n"));
      return 1;
    }

  /* Open script source. */
  FILE* script_fp = fopen (path, "r");
  if (script_fp == NULL)
    {
      BNG_DBG (_("Unable to read [%s], %s"), path, strerror (errno));
      return 1;
    }

  /* Construct filename for compiled output */
  char *out_name;
  if (g_str_has_suffix (path, ".bng"))
    out_name = g_strdup_printf ("%so", path);
  else
    out_name = g_strdup_printf ("%s.bngo", path);

  if (out_name == NULL)
    {
      BNG_DBG (_("Unable to construct output file name from [%s], %s"), path, strerror (errno));
      return 1;
    }

  FILE *out_fp = fopen (out_name, "w");
  if (out_fp == NULL)
    {
      BNG_DBG (_("Unable to open [%s] in write mode, %s"), out_name, strerror (errno));
      g_free (out_name);
      return 1;
    }

  status = bng_compile (script_fp, path, out_fp, err_fp);

  g_free (out_name);
  fclose (script_fp);
  fclose (out_fp);

  return status;
}
