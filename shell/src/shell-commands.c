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

#include <bungee.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "local-defs.h"
#include "shell-commands.h"

static gint
shell_help (const gchar *path)
{
  g_printf (_("help - Show this help\n"));
  g_printf (_("quit - Quit this shell\n"));
  g_printf (_("eval EXPR - Evaluate "PACKAGE" expression\n"));
  g_printf (_("load FILE - Load and evaluate script\n"));
  g_printf (_("run FILE  - Load and evaluate "PACKAGE" script\n"));
  return (0);
}

static gint
shell_compile (const gchar *path)
{
  gint status = 0;
  if (!path || !path[0])
    {
      g_print (_("File name required.\n"));
      return 1;
    }

  status = bng_compile_file (path, stderr);
  if (status == 0)
    {
      if (g_str_has_suffix (path, ".bng"))
	g_printf ("%s compiled to %so\n", path, path);
      else
	g_printf ("%s compiled to %s.bngo\n", path, path);
    }

  g_print (_("completed with status [%d].\n"), status);

  return status;
}

static gint
shell_eval (const gchar *path)
{
  gint status = 0;
  if (path && path[0])
    {
      status = bng_eval (path);
      g_print (_("completed with status [%d].\n"), status);
    }
  else
    {
      status = 1;
      g_print (_("No code to evaluate.\n"));
    }
  return (status);
}

static gint
shell_load (const gchar *path)
{
  gint status = 0;
  if (path && path[0])
    {
      status = bng_load (path);
      g_print (_("completed with status [%d].\n"), status);
    }
  else
    {
      status = 1;
      g_print (_("File name required.\n"));
    }
  return status;
}

static gint
shell_run (const gchar *path)
{
  gint status = 0;
  if (path && path[0])
    {
      status = bng_run (path);
      g_print (_("completed with status [%d].\n"), status);
    }
  else
    {
      status = 1;
      g_print (_("File name required.\n"));
    }
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
  else if (g_ascii_strcasecmp (cmd, "help") == 0)
    {
      shell_help (args);
    }
  else if (g_ascii_strcasecmp (cmd, "compile") == 0)
    {
      shell_compile (args);
    }
  else if (g_ascii_strcasecmp (cmd, "eval") == 0)
    {
      shell_eval (args);
    }
  else if (g_ascii_strcasecmp (cmd, "load") == 0)
    {
      shell_load (args);
    }
  else if (g_ascii_strcasecmp (cmd, "run") == 0)
    {
      shell_run (args);
    }

  g_strfreev (cmd_list);
  return (0);
}
