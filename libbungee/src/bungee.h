/*
bungee.h: main bungee header file

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


#ifndef _BUNGEE_H
#define _BUNGEE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <glib.h>

#include "logger.h"
#include "python-embedding.h"

/* bng_rc can be NULL or /path/to/.bngrc */
gint bng_init (bng_console_t msg, bng_console_t log, bng_log_level_t log_level);
gint bng_fini (void);
gint bng_eval (const gchar *code);
gint bng_load (const gchar *path);
gint bng_run (const gchar *bng_script);

#ifdef __cplusplus
}
#endif

#endif /* _BUNGEE_H */
