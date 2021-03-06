/*
libbungee.h: core bungee routines

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


#ifndef _LIBBUNGEE_H
#define _LIBBUNGEE_H

#ifdef __cplusplus
extern "C" {
#endif

#define BNG_HOOK_BEGIN  "BEGIN"
#define BNG_HOOK_END    "END"
#define BNG_HOOK_INPUT  "INPUT"

/* bng_rc can be NULL or /path/to/.bngrc */
gint bng_init (bng_console_t msg, bng_console_t log, bng_log_level_t log_level);
gint bng_fini (void);
gint bng_eval (const gchar *code);
gint bng_load (const gchar *path);
gint bng_run (const gchar *script_name);

#ifdef __cplusplus
}
#endif

#endif /* _LIBBUNGEE_H */
