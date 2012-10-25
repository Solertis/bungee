/*
  parser-interface.h: Interface to bison parser

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

#ifndef _PARSER_INTERFACE_H
#define _PARSER_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Compile Bungee .bng script to .bngo format.

   script_fp, output_fp and err_fp are FILE * type.
   It means you can pass stdin, stdout and stderr here.

   script_name is only used for log messages.

   NULL err_fp disables bison error messages. */
int bng_compile (FILE *script_fp, const char *script_name, FILE *output_fp, FILE *err_fp);

/* Compile Bungee [file].bng script to [file].bngo output.
   NULL err_fp disables bison error messages. Debug logs will work how ever. */
gint bng_compile_file (const gchar *script_name, FILE *err_fp);

#ifdef __cplusplus
}
#endif

#endif /* _PARSER_INTERFACE_H */
