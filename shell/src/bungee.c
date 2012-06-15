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
#include <argp.h>

#include "system.h"
#include "shell.h"

#define EXIT_FAILURE 1

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define textdomain(Domain)
# define _(Text) Text
#endif
#define N_(Text) Text

char *xmalloc ();
char *xrealloc ();
char *xstrdup ();

static error_t parse_opt (int key, char *arg, struct argp_state *state);
static void show_version (FILE *stream, struct argp_state *state);

/* argp option keys */
enum {DUMMY_KEY=129
      ,DRYRUN_KEY
      ,NOWARN_KEY
      ,DIRECTORY_KEY
};

/* Option flags and variables.  These are initialized in parse_opt.  */

char *oname;			/* --output=FILE */
FILE *ofile;
char *desired_directory;	/* --directory=DIR */
int want_interactive;		/* --interactive */
int want_quiet;			/* --quiet, --silent */
int want_verbose;		/* --verbose */
int want_dry_run;		/* --dry-run */
int want_no_warn;		/* --no-warn */

static struct argp_option options[] =
{
  { "interactive", 'i',           NULL,            0,
    N_("Prompt for confirmation"), 0 },
  { "output",      'o',           N_("FILE"),      0,
    N_("Send output to FILE instead of standard output"), 0 },
  { "quiet",       'q',           NULL,            0,
    N_("Inhibit usual output"), 0 },
  { "silent",      0,             NULL,            OPTION_ALIAS,
    NULL, 0 },
  { "verbose",     'v',           NULL,            0,
    N_("Print more information"), 0 },
  { "dry-run",     DRYRUN_KEY,    NULL,            0,
    N_("Take no real actions"), 0 },
  { "no-warn",     NOWARN_KEY,    NULL,            0,
    N_("Disable warnings"), 0 },
  { "directory",   DIRECTORY_KEY, N_("DIR"),       0,
    N_("Use directory DIR"), 0 },
  { NULL, 0, NULL, 0, NULL, 0 }
};

/* The argp functions examine these global variables.  */
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
void (*argp_program_version_hook) (FILE *, struct argp_state *) = show_version;

static struct argp argp =
{
  options, parse_opt, N_("[FILE...]"),
  N_("Bungee is a simple framework for analyzing big unstructured data."),
  NULL, NULL, NULL
};

int
main (int argc, char **argv)
{
  textdomain(PACKAGE);
  argp_parse(&argp, argc, argv, 0, NULL, NULL);

  do {
    bng_shell ();
  } while (1);

  exit (0);
}

/* Parse a single option.  */
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  switch (key)
    {
    case ARGP_KEY_INIT:
      /* Set up default values.  */
      oname = "stdout";
      ofile = stdout;
      desired_directory = NULL;
      want_interactive = 0;
      want_quiet = 0;
      want_verbose = 0;
      want_dry_run = 0;
      want_no_warn = 0;
      break;

    case 'i':			/* --interactive */
      want_interactive = 1;
      break;
    case 'o':			/* --output */
      oname = xstrdup (arg);
      ofile = fopen (oname, "w");
      if (!ofile)
	argp_failure (state, EXIT_FAILURE, errno,
		      _("Cannot open %s for writing"), oname);
      break;
    case 'q':			/* --quiet, --silent */
      want_quiet = 1;
      break;
    case 'v':			/* --verbose */
      want_verbose = 1;
      break;
    case DRYRUN_KEY:		/* --dry-run */
      want_dry_run = 1;
      break;
    case NOWARN_KEY:		/* --no-warn */
      want_no_warn = 1;
      break;
    case DIRECTORY_KEY:		/* --directory */
      desired_directory = xstrdup (optarg);
      break;

    case ARGP_KEY_ARG:		/* [FILE]... */
      /* TODO: Do something with ARG, or remove this case and make
         main give argp_parse a non-NULL fifth argument.  */
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

/* Show the version number and copyright information.  */
static void
show_version (FILE *stream, struct argp_state *state)
{
  (void) state;
  /* Print in small parts whose localizations can hopefully be copied
     from other programs.  */
  fputs(PACKAGE" "VERSION"\n", stream);
  fprintf(stream, _("Written by %s.\n"), "Anand Babu (AB) Periasamy");
  fprintf(stream, _("URL: %s\n\n"), PACKAGE_URL);
  fprintf(stream, _("Copyright (C) %s %s\n"), "2012", "Red Hat, Inc.");
  fputs(_("\
This program is free software; you may redistribute it under the terms of\n\
the Apache License v2.0.  This program has absolutely no warranty.\n"),
	stream);
}
