/*
python-embedding.h: Use python as extension language.

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

#ifndef _PYTHON_EMBEDDING_H
#define _PYTHON_EMBEDDING_H

#ifdef __cplusplus
extern "C" {
#endif

#define BNG_HOOK_BEGIN  "BEGIN"
#define BNG_HOOK_END    "END"
#define BNG_HOOK_INPUT  "INPUT"
#define BNG_HOOK_OUTPUT "OUTPUT"

gint bng_py_init (void);
gint bng_py_hook_call (const gchar *hook_name, char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* _PYTHON_EMBEDDING_H */
