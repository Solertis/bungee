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
#include <termios.h>
#include <signal.h>
#include <bungee.h>
#include <glib.h>
#include <readline/readline.h>

#include "shell-readline.h"

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
  gchar *cmd = NULL;

  bng_init ();

  /* Install signal handler */
  setvbuf (stdout, NULL, _IONBF, 0);
  signal (SIGINT, signal_caught);
  tcgetattr (0, &old_termios);
  new_termios             = old_termios;
  new_termios.c_cc[VEOF]  = 3; // ^C
  new_termios.c_cc[VINTR] = 4; // ^D
  tcsetattr (0, TCSANOW, &new_termios);

  do
    {
      //      cmd = (gchar *) rl_gets (BNG_PROMPT);
      cmd = (gchar *) readline (BNG_PROMPT);
      cmd = g_strstrip (cmd);

      if (!cmd || strlen (cmd) == 0)
	continue;

      if (g_strcmp0 (cmd, "quit") == 0)
	{
	  g_free (cmd);
	  break;
	}

      g_free (cmd);
    } while (1);

  /* Uninstall signal handler */
  tcsetattr(0, TCSANOW, &old_termios);

  bng_fini ();

  return (0);

}
