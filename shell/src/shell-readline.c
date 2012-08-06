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

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <glib.h>

#include "bungee.h"

/* Read a string, and return a pointer to it.
   Returns NULL on EOF. */
gchar *
rl_gets (const gchar *prompt)
{
  gchar *line_read = NULL;

  /* Get a line from the user. */
  line_read = readline (prompt);

  /* If the line has any text in it,
     save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}


static gchar *
auto_complete (const gchar *text, gint state)
{
  const gchar *command_completion_regex [] = {
    "^ *$",
    "^ */[^ ]*$",
    "^ */help +[^ ]*$",
    "^ *help +[^ ]*$",
    "^ */history +[^ ]*$",
    "^ */eval +[^ ]*$",
    NULL
  };

  const gchar *file_completion_regex [] = {
    "^ */load +[^ ]*$",
    NULL
  };
}


gchar **rl_auto_complete (const gchar *text, gint start, int end)
{
  gchar **matches  = (char **)NULL;

  matches = rl_completion_matches (text, auto_complete);

  return matches;

}
