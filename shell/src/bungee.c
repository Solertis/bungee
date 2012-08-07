/*
bungee.c: Shell interface to bungee framework.

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
#include <sys/types.h>
#include <stdlib.h>
#include <glib.h>
#include <bungee.h>

#include "local-defs.h"
#include "shell.h"

const char *program_bug_address = PACKAGE_BUGREPORT;

static gboolean show_version (const gchar *option_name, const gchar *value, gpointer data, GError **error);

/* Option flags and variables.  These are initialized in parse_opt.  */
static gchar *oname = NULL;			/* --output=FILE */
static gchar *startup_script = NULL;			/* --startup=FILE */
FILE *ofile;
static gchar *desired_directory = NULL;	/* --directory=DIR */
static gchar *bng_script = NULL;	/* --file=FILE */
static gboolean want_quiet = FALSE;		/* --quiet, --silent */
static gboolean want_verbose = FALSE;		/* --verbose */
static gboolean want_no_warn = FALSE;		/* --no-warn */

static GOptionEntry opt_entries[] = {
  { "version", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, show_version,
    N_("Print version information"), NULL },
  { "file", 'f', 0, G_OPTION_ARG_FILENAME, &bng_script,
    N_("Execute "PACKAGE" script"), "FILE"},
  { "startup", 0, 0, G_OPTION_ARG_FILENAME, &startup_script,
    N_("Use this startup FILE instead"), "FILE"},
  { "output", 'o', 0, G_OPTION_ARG_FILENAME, &oname,
    N_("Send output to FILE instead of standard output"), "FILE"},
  { "quiet", 'q', 0, G_OPTION_ARG_NONE, &want_quiet,
    N_("Inhibit usual output"), NULL },
  { "verbose", 0, 0, G_OPTION_ARG_NONE, &want_verbose,
    N_("Print more information"), NULL },
  { "no-warn", 0, 0, G_OPTION_ARG_NONE, &want_no_warn,
    N_("Disable warnings"), NULL },
  { "directory", 0, 0, G_OPTION_ARG_STRING, &desired_directory,
    N_("Use directory DIR"), "DIR" },
  { NULL }
};

/* Show the version number and copyright information.  */
static gboolean show_version (const gchar *option_name,
			      const gchar *value,
			      gpointer data, GError **error)
{
  /* Print in small parts whose localizations can hopefully be copied
     from other programs.  */
  g_print (PACKAGE" "VERSION"\n");
  g_print (_("Copyright (C) %s %s\n"), "2012", "Red Hat, Inc.");
  g_print (_("License: Apache License, Version 2.0\n"
	     "This is free software: you are free to change and redistribute it. "
	     "There is NO WARRANTY, to the extent permitted by law.\n\n"));
  g_print (_("Written by %s.\n"), "Anand Babu (AB) Periasamy");
  g_print (_("URL: %s\n"), PACKAGE_URL);

  exit (0);
}

int
main (int argc, char **argv)
{
  GError *error = NULL;
  GOptionContext *context;
  gint status = 0;

  textdomain(PACKAGE);

  /* Glib based option parsing */
  context = g_option_context_new (NULL);

  g_option_context_set_summary (context, N_("Bungee is a distributed \"awk\" like framework for analyzing big unstructured data."));
  g_option_context_set_description (context, N_("For more information, please visit http://www.bungeeproject.org/"));
  g_option_context_add_main_entries (context, opt_entries, PACKAGE);

  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_print (_("option parsing failed: %s\n"), error->message);
    g_option_context_free(context);
    exit (1);
  }

  g_option_context_free(context);

  /* Initialize Bungee environment */
  bng_init ();

  /* Load BNG_RC startup script */
  if (startup_script)
    bng_load (startup_script);
  else
    bng_load (BNG_RC);

  if (bng_script != NULL)
    {
      status = bng_run (bng_script);
      if (status != 0)
	{
	  BNG_WARNING (_("Error execuring [%s] "PACKAGE" script"), bng_script);
	  goto END;
	}
    }
  else
    {
      /* If nothing else, run the interactive shell by default */
      status = bng_shell (); /* Main interactive shell */
    }

 END:
  bng_fini ();
  exit (status);
}
