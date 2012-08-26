/*
logger.h: bungee logging framework

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

#ifndef _LOGGER_H
#define _LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  BNG_LOG_LEVEL_FATAL,
  BNG_LOG_LEVEL_ERROR,
  BNG_LOG_LEVEL_WARNING,
  BNG_LOG_LEVEL_INFO,
  BNG_LOG_LEVEL_DEBUG
} bng_log_level_t;

typedef enum {
  BNG_CONSOLE_TYPE_STDOUT  = 1 << 0,
  BNG_CONSOLE_TYPE_STDERR  = 1 << 1,
  BNG_CONSOLE_TYPE_FILE    = 1 << 2,
  BNG_CONSOLE_TYPE_SYSLOG  = 1 << 3,
  BNG_CONSOLE_TYPE_ZMQ     = 1 << 4
} bng_console_type_t;

typedef struct
{
  bng_console_type_t type;
  union
  {
    FILE* fp;
    /* syslog */
  } device;
} bng_console_t;

#define BNG_MSG(format, ...) bng_msg (format, ##__VA_ARGS__)

#define BNG_FATAL(format, ...) bng_log ("FATAL [%s:%d]: "format, __FILE__, __LINE__, ##__VA_ARGS__)
#define BNG_ERR(format, ...) bng_log ("ERROR [%s:%d]: "format, __FILE__, __LINE__, ##__VA_ARGS__)
#define BNG_WARN(format, ...) bng_log ("WARNING [%s:%d]: "format, __FILE__, __LINE__, ##__VA_ARGS__)
#define BNG_DBG(format, ...) bng_log ("DEBUG [%s:%d]: "format, __FILE__, __LINE__, ##__VA_ARGS__)

gint bng_console_init (bng_console_t msg, bng_console_t log, bng_log_level_t log_level);
gint bng_msg (gchar *format, ...);
gint bng_log (gchar *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _LOGGER_H */
