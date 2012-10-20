/*
  bungee-parser.y: bungee script bison parser

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

%code top {
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

#include "bungee-parser.h"
}

%code provides {
/* Flex returns this code upon error */
#define YYERRCODE  256

/* Error handling routine */
int yyerror (const char *format, ...) __attribute__ ((format (gnu_printf, 1, 2)));
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
int yylex ();

#define XFREE(ptr) if (ptr) { free (ptr); ptr = NULL;}
#define _PAD_SPACES(num) do { int i; for (i=1; i<num; i++) putchar (' '); } while (0)

/* Current yyin script source */
const char *__script_name=NULL;
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
program: begincb inputcb rules endcb /* valid case */
| begincb rules endcb { yyerror ("No INPUT section found after BEGIN section.\n"); YYABORT; }
| begincb inputcb endcb { yyerror ("No RULEs found after INPUT section.\n"); YYABORT; }
| begincb endcb { yyerror ("No INPUT section or RULEs found.\n"); YYABORT; }

begincb: /* Optional */
| TBEGIN
{
  printf ("def BEGIN():");
}

inputcb:
TINPUT
{
  printf ("def INPUT():");
}

rules: rule | rules rule

rule: TRULE TRULE_NAME TRULE_CONDT
{
  if ($2 == NULL)
    {
      yyerror ("RULE has no name.\n");
      YYABORT;
    }

  if ($3 == NULL)
    printf ("RULES.APPEND('DEFAULT', '%s', lambda: True, '_RULE_%s()'))\n", $2, $3);
  else
    printf ("RULES.APPEND('DEFAULT', '%s', lambda: %s, '_RULE_%s()'))\n", $2, $3, $3);

  printf ("def _RULE_%s():", $2);

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
    printf ("RULES.APPEND('%s', '%s', '''True''', '_RULE_%s()'))\n", $2, $4, $5);
  else
    printf ("RULES.APPEND('%s', '%s', '''%s''', '_RULE_%s()'))\n", $2, $4, $5, $5);

  printf ("def _RULE_%s():", $4);

  XFREE ($2);
  XFREE ($4);
  XFREE ($5);
}

endcb: /* Optional */
| TEND
{
  printf ("def END():");
}
| error
{
  YYABORT;
}

%%

int
yyerror (const char *format, ...)
{
  extern int yylineno;
  va_list ap;
  va_start (ap, format);

  if (__script_name && __script_name[0])
    fprintf (stderr, "ERROR[%s:%d]: ", __script_name, yylineno);
  else
    fprintf (stderr, "ERROR[line %d]: ", yylineno);
  vfprintf (stderr, format, ap);
  va_end (ap);
  // exit (1);
  return YYERRCODE;
}

int
bng_compile (FILE *in_fp, FILE *out_fp, const char *script_name)
{
  extern FILE *yyin, *yyout;
  yyin = in_fp;
  yyout = out_fp;
  __script_name = script_name; /* Used by yyerror to relate error messages to script */

  if (yyparse () != 0)
    return (1);
  return (0);
}

#ifdef _DEBUG_PARSER
int
main (void)
{
  bng_compile (stdin, stdout, "test.bng");
  return 0;
}

#endif
