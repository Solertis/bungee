/*
shell-commands.c: bungee shell commands

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
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <glib.h>
#include <bungee.h>

#include "local-defs.h"
#include "shell-commands.h"

static gint
shell_eval (const gchar *code)
{
  gint status = bng_eval (code);
  g_print (_("status=[%d]\n"), status);

  return (status);
}

static gint
shell_load (const gchar *path)
{
  gint status = bng_run (path);
  g_print (_("completed with status [%d]\n"), status);
  return (status);
}

gint
shell_interpreter (const gchar *cmd_line)
{
  gchar **cmd_list = NULL;
  gchar *cmd = NULL, *args = NULL;

  cmd_list =  g_strsplit (cmd_line, " ", 2);
  cmd = cmd_list[0];
  args = cmd_list[1];

  if (g_ascii_strcasecmp (cmd, "quit") == 0)
    return (1);
  else if (g_ascii_strcasecmp (cmd, "eval") == 0)
    {
      shell_eval (args);
    }
  else if (g_ascii_strcasecmp (cmd, "load") == 0)
    {
      shell_load (args);
    }

  g_strfreev (cmd_list);
  return (0);
}
