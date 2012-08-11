/*
shell.c: shell interface

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
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <bungee.h>
#include <glib.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "local-defs.h"
#include "shell.h"
#include "shell-readline.h"
#include "shell-commands.h"

struct termios old_termios, new_termios;

void
signal_caught (int signal)
{
  tcsetattr(0, TCSANOW, &old_termios);
  putchar ('\n');
  bng_fini ();
  exit (0);
}

gint
bng_shell (void)
{
  gchar *cmd_line = NULL;

  /* Install signal handler */
  setvbuf (stdout, NULL, _IONBF, 0);
  signal (SIGINT, signal_caught);
  tcgetattr (0, &old_termios);
  new_termios             = old_termios;
  new_termios.c_cc[VEOF]  = 3; // ^C
  //  new_termios.c_cc[VINTR] = 4; // ^D
  tcsetattr (0, TCSANOW, &new_termios);

  shell_readline_init ();
  rl_bind_key('\t',rl_complete);

  do
    {
      cmd_line = (gchar *) readline (BNG_PROMPT);
      cmd_line = g_strstrip (cmd_line);

      if (!cmd_line || strlen (cmd_line) == 0)
	continue;

      if (shell_interpreter (cmd_line) != 0)
	{
	  g_free (cmd_line);
	  break;
	}

      add_history (cmd_line);
      g_free (cmd_line);
    } while (1);

  /* Uninstall signal handler */
  tcsetattr(0, TCSANOW, &old_termios);
  return (0);
}
