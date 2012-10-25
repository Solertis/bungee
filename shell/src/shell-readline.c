/*
shell-readline.c: readline interface to shell

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

#include <bungee.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <glib.h>
#include <regex.h>

#include "local-defs.h"
#include "shell-readline.h"

/* Read a string, and return a pointer to it.
   Returns NULL on EOF. */
static gchar *
shell_completion_generator (const gchar *text, gint state)
{
  const gchar *commands [] = {"help", "compile", "quit", "eval", "load", "run", NULL};
  static gint list_index, len;
  const gchar *name;

  if (state == 0)
    {
      list_index = 0;
      len = strlen (text);
    }

  while ((name = commands[list_index++])) {
    if (g_ascii_strncasecmp (name, text, len) == 0)
      return (g_strdup (name));
  }

  /* If no names matched, then return NULL. */
  return ((gchar *)NULL);
}

static gchar **
shell_completion_matches (const gchar *text, gint start, gint end)
{
  gchar **matches  = (char **)NULL;

  if (start == 0)
    matches = rl_completion_matches (text, shell_completion_generator);

  return (matches);
}

void
shell_readline_init (void)
{
  rl_attempted_completion_function = shell_completion_matches;
}
