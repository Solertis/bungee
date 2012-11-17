/*
  parser.y: bungee's bison parser

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

/*** Bison declarations ***/
/* Start symbol */
%start program
%define api.pure
%error-verbose
%locations

/* Terminal value type */
%union {
  char *string;
}

/** Terminal symbols **/
/* Terminal symbols with no value */
%token TBEGIN TINPUT TEND TRULE TGROUP TEOF
/* Terminal symbols with string value */
%token <string> TGROUP_NAME TRULE_NAME TRULE_CONDT

/*  Free heap based token values during error recovery */
%destructor { XFREE ($$); } <string>

/* Pass the argument to yyparse through to yylex. */
%parse-param {yyscan_t yyscanner}
%lex-param   {yyscan_t yyscanner}

/* C declarations */
%initial-action {
#ifndef _DEBUG_PARSER
  bindtextdomain ("bison-runtime", BISON_LOCALEDIR);
#endif
}

%code top {
#include <libintl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
}

%code provides {
/* Compile .bng source to .bngo format */
int bng_compile (FILE *script_fp, const char *script_name, FILE *out_fp, FILE *err_fp);
}

%code requires {
  typedef struct {
    FILE *err_fp;
    const char *script_name;
    struct {
      unsigned char slquote_type;
      unsigned char mlquote_type;
      unsigned long sl_start;
      unsigned long ml_start;
    } quote;
    struct found {
      unsigned char begin;
      unsigned char input;
      unsigned char end;
    } found;
  } local_vars_t;

/* Terminal location type */
typedef struct YYLTYPE {
  int _column;
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */
}

%code {
#define XFREE(ptr) if (ptr) { free (ptr); ptr = NULL;}
#define _PAD_SPACES(num) do { int i; for (i=1; i<num; i++) putchar (' '); } while (0)

/* Flex generates all the header definitions for us. */
#include "scanner.h"

extern int yyerror (yyscan_t yyscanner, const char *format, ...) __attribute__ ((format (gnu_printf, 2, 3)));
}

/* Grammar Rules */
%%
program: | program section
section: begincb | inputcb | rule | endcb

begincb:
TBEGIN
{
  fprintf (yyget_out (yyscanner), "def BEGIN():");
}

inputcb:
TINPUT
{
  fprintf (yyget_out (yyscanner), "def INPUT():");
}

rule: TRULE TRULE_NAME TRULE_CONDT
{
  if ($2 == NULL)
    {
      yyerror (yyscanner, "RULE has no name.\n");
      YYABORT;
    }

  if ($3 == NULL)
    fprintf (yyget_out (yyscanner), "Rules.append('_global', '%s', lambda: True, '_ation_%s')\n", $2, $2);
  else
    fprintf (yyget_out (yyscanner), "Rules.append('_global', '%s', lambda: %s, '_action_%s')\n", $2, $3, $2);

  fprintf (yyget_out (yyscanner), "def _action_%s():", $2);

  XFREE ($2);
  XFREE ($3);
}
| TGROUP TGROUP_NAME TRULE TRULE_NAME TRULE_CONDT
{
  if ($2 == NULL)
    {
      yyerror (yyscanner, "GROUP has no name.\n");
      YYABORT;
    }

  if (strncmp ($2, "RULE", 4) == 0)
    {
      yyerror (yyscanner, "GROUP has no name.\n");
      YYABORT;
    }

  if ($4 == NULL)
    {
      yyerror (yyscanner, "RULE has no name.\n");
      YYABORT;
    }

  if ($5 == NULL)
    fprintf (yyget_out (yyscanner), "Rules.append('%s', '%s', '''lambda: True''', '_action_%s')\n", $2, $4, $4);
  else
    fprintf (yyget_out (yyscanner), "Rules.append('%s', '%s', '''lambda: %s''', '_action_%s')\n", $2, $4, $5, $4);

  fprintf (yyget_out (yyscanner), "def _action_%s():", $4);

  XFREE ($2);
  XFREE ($4);
  XFREE ($5);
}

endcb:
TEND
{
  fprintf (yyget_out (yyscanner), "def END():");
}
| error
{
  YYABORT;
}

%%

int
bng_compile (FILE *script_fp, const char *script_name, FILE *out_fp, FILE *err_fp)
{
  int status;
  yyscan_t yyscanner; /* Re-entrant praser stores its state here. */
  local_vars_t locals;

  locals.quote.slquote_type = locals.quote.mlquote_type='\0';
  locals.quote.sl_start = locals.quote.ml_start = 0;
  locals.found.begin = locals.found.input = locals.found.end = 0;
  locals.err_fp = stderr;
  locals.script_name = script_name; /* Used by yyerror to relate error messages to script. */

  if (yylex_init_extra (&locals, &yyscanner) != 0)
    return 1;

  if (script_fp == NULL)
    yyset_in (stdin, yyscanner);
  else
    yyset_in (script_fp, yyscanner);

  if (out_fp == NULL)
    yyset_out (stdout, yyscanner);
  else
    yyset_out (out_fp, yyscanner);

  if (yyparse (yyscanner) == 0)
    status = 0;
  else
    status = 1;

  yylex_destroy (yyscanner);

  return status;
}

#ifdef _DEBUG_PARSER
int
main (void)
{
  //return  bng_compile (NULL, NULL, NULL, NULL);
  return bng_compile (stdin, NULL, stdout, stderr);
}

#endif
