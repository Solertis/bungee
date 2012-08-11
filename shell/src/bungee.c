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
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <bungee.h>

#include "local-defs.h"
#include "shell.h"

const char *program_bug_address = PACKAGE_BUGREPORT;

static gboolean show_version (const gchar *option_name, const gchar *value, gpointer data, GError **error);
/* Rest of unparsed strings are stored here. How ever we only support
   one string i.e. script filename. */
static gchar **rest_args = NULL;
/* Option flags and variables. These are initialized in parse_opt.  */
static gchar **log_devices = NULL; /* Log file */
FILE *log_fp;
static gchar **msg_devices = NULL; /* Log file */
FILE *msg_fp;
static gchar *log_level = NULL;  /* Minimum log level */

static gchar *startup_script = NULL; /* Choose a different startup file other than "~/.bungeerc" */
static gchar *bng_script = NULL;  /* Execute this bungee script  */

static GOptionEntry opt_entries[] = {
  { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &rest_args,
    NULL, NULL },
  { "version", 'v', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK, show_version,
    N_("Print version information"), NULL },
  { "startup", 0, 0, G_OPTION_ARG_FILENAME, &startup_script,
    N_("Use this startup FILE instead"), "FILE"},
  { "log", 'l', 0, G_OPTION_ARG_STRING_ARRAY, &log_devices,
    N_("Send log messages to these targets"), "[stdout|*stderr|syslog|FILE|zmq]"},
  { "log-level", 'L', 0, G_OPTION_ARG_STRING, &log_level,
    N_("Set minium log level"), "[fatal|error|*warning|info|debug]"},
  { "output", 'o', 0, G_OPTION_ARG_STRING_ARRAY, &msg_devices,
    N_("Send console messages to these targets"), "[*stdout|stderr|syslog|FILE|zmq]"},
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
  g_print ("Big data processor inspired by AWK and Spreadsheet");
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
  context = g_option_context_new (_("[FILE]"));

  g_option_context_set_summary (context, N_("Bungee is a big data processor inspired by AWK and Spreadsheet."));
  g_option_context_set_description (context, N_("For more information, please visit http://www.bungeeproject.org/"));
  g_option_context_add_main_entries (context, opt_entries, PACKAGE);

  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_print (_("option parsing failed: %s\n"), error->message);
    g_option_context_free(context);
    exit (1);
  }
  g_option_context_free(context);


  /* Initialize Bungee environment */
  bng_console_t msg, log;

  memset (&log, 0, sizeof (log));
  memset (&msg, 0, sizeof (msg));

  msg.type = BNG_CONSOLE_TYPE_FILE;
  msg.device.fp = stdout;

  /* Parse message devices */
  gchar *msg_file = NULL;
  msg.type = BNG_CONSOLE_TYPE_STDERR;
  if (msg_devices)
    {
      gint i;
      for(i=0; msg_devices[i] && msg_devices[i][0]; i++)
	{
	  if (g_strcmp0 (msg_devices[i], "stdout") == 0)
	    msg.type = msg.type | BNG_CONSOLE_TYPE_STDOUT;
	  else if (g_strcmp0 (msg_devices[i], "stderr") == 0)
	    msg.type = msg.type | BNG_CONSOLE_TYPE_STDERR;
	  else if (g_strcmp0 (msg_devices[i], "syslog") == 0)
	    msg.type = msg.type | BNG_CONSOLE_TYPE_SYSLOG;
	  else if (g_strcmp0 (msg_devices[i], "zmq") == 0)
	    msg.type = msg.type | BNG_CONSOLE_TYPE_ZMQ;
	  else
	    {
	      /* No other option matches. This means msg_devices[i] must be FILE. */
	      /* If we have not processed this option already in previous iterations */
	      if (~(msg.type & BNG_CONSOLE_TYPE_FILE))
		{
		  msg.type = msg.type | BNG_CONSOLE_TYPE_FILE;
		  msg.device.fp = g_fopen (msg_devices[i], "a");
		  if (!msg.device.fp)
		    {
		      BNG_ERR (_("Unable to open msg file [%s]."), msg_devices[i]);
		      exit (1);
		    }
		  /* We need to save the filename to compare with log filename. If they are same,
		     have to reopen then. Remember to free this msg_file later. */
		  msg_file = g_strdup (msg_devices[i]);
		}
	    }
	}
    }
  g_strfreev (msg_devices);


    /* Parse log devices */
  log.type = BNG_CONSOLE_TYPE_STDERR;
  if (log_devices)
    {
      gint i;
      for(i=0; log_devices[i] && log_devices[i][0]; i++)
	{
	  if (g_strcmp0 (log_devices[i], "stdout") == 0)
	    log.type = log.type | BNG_CONSOLE_TYPE_STDOUT;
	  else if (g_strcmp0 (log_devices[i], "stderr") == 0)
	    log.type = log.type | BNG_CONSOLE_TYPE_STDERR;
	  else if (g_strcmp0 (log_devices[i], "syslog") == 0)
	    log.type = log.type | BNG_CONSOLE_TYPE_SYSLOG;
	  else if (g_strcmp0 (log_devices[i], "zmq") == 0)
	    log.type = log.type | BNG_CONSOLE_TYPE_ZMQ;
	  else
	    {
	      /* No other option matches. This means log_devices[i] must be FILE. */
	      /* If we have not processed this option already in previous iterations */
	      if (~(log.type & BNG_CONSOLE_TYPE_FILE))
		{
		  log.type = log.type | BNG_CONSOLE_TYPE_FILE;

		  if ((msg_file && msg_file[0]) && (g_strcmp0 (msg_file, log_devices[i]) == 0))
		    {
		      /* User requested msg and log to go to same file. We don't have to reopen again. */
		      log.device.fp = msg.device.fp;
		      g_free (msg_file);
		    }
		  else
		    {
		      log.device.fp = g_fopen (log_devices[i], "a");
		      if (!log.device.fp)
			{
			  BNG_ERR (_("Unable to open log file [%s]."), log_devices[i]);
			  exit (1);
			}
		    }
		}
	    }
	}
    }
  g_strfreev (log_devices);


  /* Parse log level */
  /* default log level is WARNING */
  bng_log_level_t _log_level = BNG_LOG_LEVEL_WARNING;
  if (log_level && log_level[0])
    {
      if (g_strcmp0 (log_level, "fatal") == 0)
	_log_level = BNG_LOG_LEVEL_FATAL;
      else if (g_strcmp0 (log_level, "error") == 0)
	_log_level = BNG_LOG_LEVEL_ERROR;
      else if (g_strcmp0 (log_level, "warning") == 0)
	_log_level = BNG_LOG_LEVEL_WARNING;
      else if (g_strcmp0 (log_level, "info") == 0)
	_log_level = BNG_LOG_LEVEL_INFO;
      else if (g_strcmp0 (log_level, "debug") == 0)
	_log_level = BNG_LOG_LEVEL_DEBUG;
      else
	{
	  BNG_ERR (_("Unknown log level [%s] passed. Assuming defaults."), log_level);
	  exit (1);
	}
      g_free (log_level);
    }


  /* Parse script FILE from rest of arguments */
  if (rest_args && rest_args[0])
    bng_script = g_strdup (rest_args[0]);
  if (rest_args && rest_args[1])
    {
      BNG_ERR (_("Excess arguments after [%s]"), rest_args[0]);
      exit (1);
    }
  g_strfreev (rest_args);


  if (bng_init (msg, log, _log_level) != 0)
    {
      BNG_ERR (_("Failed to intialize "PACKAGE));
      exit (1);
    }

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
	  BNG_ERR (_("Could not execute "PACKAGE" script [%s]"), bng_script);
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
