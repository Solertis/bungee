/*
logger.c: bungee logging framework

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
#include <stdarg.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "local-defs.h"
#include "logger.h"

bng_console_t msg_console, log_console;

gint
bng_console_init (bng_console_t msg, bng_console_t log)
{
  msg_console = msg;
  log_console = log;
  return (0);
}

gint
bng_msg (gchar *format, ...)
{
  gchar *msg_buff = NULL;
  va_list args;

  va_start (args, format);
  msg_buff = g_strdup_vprintf (format, args);

  switch (msg_console.type)
    {
    case BNG_CONSOLE_TYPE_FD:
    case BNG_CONSOLE_TYPE_FP:
    case BNG_CONSOLE_TYPE_SYSLOG:
    default:
      fputs (msg_buff, stderr);
      fputc ('\n', stderr);
    }

  va_end (args);
  g_free (msg_buff);

  return (0);
}

gint
bng_log (gchar *format, ...)
{
  gchar *log_buff = NULL;
  va_list args;

  va_start (args, format);
  log_buff = g_strdup_vprintf (format, args);

  switch (log_console.type)
    {
    case BNG_CONSOLE_TYPE_FD:
    case BNG_CONSOLE_TYPE_FP:
    case BNG_CONSOLE_TYPE_SYSLOG:
    default:
      fputs (log_buff, stderr);
      fputc ('\n', stderr);
    }

  va_end (args);
  g_free (log_buff);

  return (0);
}
