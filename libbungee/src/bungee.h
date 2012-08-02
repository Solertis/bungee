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

#include "python-embedding.h"
#include "logger.h"

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif

#define BNG_PROMPT "=> "
#define BNG_RC ".bungeerc"

gint bng_init (void);
gint bng_set_rc (gchar *path);
gint bng_load_rc (void);
gint bng_load (gchar *path);
gint bng_fini (void);
gchar *bng_home_dir (void);

#ifdef __cplusplus
}
#endif

#endif /* _BUNGEE_H */
