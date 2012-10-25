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
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
}

%code provides {
/* Flex returns this code upon error */
#define YYERRCODE  256

/* Error handling routine */
int yyerror (const char *format, ...) __attribute__ ((format (gnu_printf, 1, 2)));

/* Compile .bng source to .bngo format */
int bng_compile (FILE *script_fp, const char *script_name, FILE *output_fp, FILE *err_fp);

}

%code requires {
/* Terminal location type */
typedef struct YYLTYPE {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  char *filename;
} YYLTYPE;
# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */
}

%code {
extern int yylex ();
extern FILE *yyin, *yyout;
FILE *yyerr;

#define XFREE(ptr) if (ptr) { free (ptr); ptr = NULL;}
#define _PAD_SPACES(num) do { int i; for (i=1; i<num; i++) putchar (' '); } while (0)

/* Current yyin script source */
static const char *__script_name=NULL;
extern int yylineno;
}

/*** Bison declarations ***/
/* Start symbol */
%start program
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

/* Grammar Rules */
%%
program: | program section
section: begincb | inputcb | rule | endcb

begincb:
TBEGIN
{
  fprintf (yyout, "def BEGIN():");
}

inputcb:
TINPUT
{
  fprintf (yyout, "def INPUT():");
}

rule: TRULE TRULE_NAME TRULE_CONDT
{
  if ($2 == NULL)
    {
      yyerror ("RULE has no name.\n");
      YYABORT;
    }

  if ($3 == NULL)
    fprintf (yyout, "RULES.APPEND('DEFAULT', '%s', lambda: True, '_RULE_%s()'))\n", $2, $3);
  else
    fprintf (yyout, "RULES.APPEND('DEFAULT', '%s', lambda: %s, '_RULE_%s()'))\n", $2, $3, $3);

  fprintf (yyout, "def _RULE_%s():", $2);

  XFREE ($2);
  XFREE ($3);
}
| TGROUP TGROUP_NAME TRULE TRULE_NAME TRULE_CONDT
{
  if ($2 == NULL)
    {
      yyerror ("GROUP has no name.\n");
      YYABORT;
    }

  if (strncmp ($2, "RULE", 4) == 0)
    {
      yyerror ("GROUP has no name.\n");
      YYABORT;
    }

  if ($4 == NULL)
    {
      yyerror ("RULE has no name.\n");
      YYABORT;
    }

  if ($5 == NULL)
    fprintf (yyout, "RULES.APPEND('%s', '%s', '''True''', '_RULE_%s()'))\n", $2, $4, $5);
  else
    fprintf (yyout, "RULES.APPEND('%s', '%s', '''%s''', '_RULE_%s()'))\n", $2, $4, $5, $5);

  fprintf (yyout, "def _RULE_%s():", $4);

  XFREE ($2);
  XFREE ($4);
  XFREE ($5);
}

endcb:
TEND
{
  fprintf (yyout, "def END():");
}
| error
{
  YYABORT;
}

%%

int
yyerror (const char *format, ...)
{
  if (!format)
    return YYERRCODE;

  if (yyerr)
    {
      extern int yylineno;
      va_list ap;
      va_start (ap, format);

      if (__script_name && __script_name[0])
	fprintf (yyerr, "ERROR[%s:%d]: ", __script_name, yylineno);
      else
	fprintf (yyerr, "ERROR[line %d]: ", yylineno);
      vfprintf (yyerr, format, ap);
      va_end (ap);
    }
  // exit (1);
  return YYERRCODE;
}

int
bng_compile (FILE *script_fp, const char *script_name, FILE *output_fp, FILE *err_fp)
{
  int status;

  extern void _bng_scanner_init (void);
  _bng_scanner_init (); /* (Re)Initializes Flex global variables. */

  if (script_fp == NULL)
    yyin = stdin;
  else
    {
      yyin = script_fp;
      // extern yyrestart (yyin); /* Just to be on the safer side. */
    }

  if (output_fp == NULL)
    yyout = stdout;
  else
    yyout = output_fp;

  yyerr = err_fp; /* yyerr is declared by parser.y and not flex. */

  __script_name = script_name; /* Used by yyerror to relate error messages to script. */

  if (yyparse () == 0)
    status = 0;
  else
    status = 1;

  yyin = yyout = yyerr = (FILE *) NULL;
  __script_name = NULL;

  return status;
}

#ifdef _DEBUG_PARSER
int
main (void)
{
  // bng_compile (NULL, NULL, NULL, NULL);
  bng_compile (stdin, NULL, stdout, stderr);
  return 0;
}

#endif
