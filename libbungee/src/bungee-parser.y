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
void yyerror (const char *format, ...) __attribute__ ((format (gnu_printf, 1, 2)));
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
int bng_parse (const char* script_name);

int yylex ();

/* Pass the argument to yyparse through to yylex. */
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
// %pure_parser

/*
%initial-action
{
  // Initialize the initial location.
  @$.filename = &__script_name;
};
*/

/* Terminal value type */
%union {
  char *string;
}

/** Terminal symbols **/
/* Terminal symbols with no value */
%token TBEGIN TINPUT TEND TRULE TGROUP
/* Terminal symbols with string value */
%token <string> TGROUP_NAME TRULE_NAME TRULE_CONDT

/*  Free heap based token values during error recovery */
%destructor { XFREE ($$); } <string>

/* Grammar Rules */
%%
program: inputcb rules /* valid case */
  | begincb inputcb rules /* valid case */
  | inputcb rules endcb /* valid case */
  | begincb inputcb rules endcb /* valid case */
  | begincb rules endcb { yyerror ("No INPUT found after BEGIN.\n"); }
  | begincb rules { yyerror ("No INPUT found after BEGIN.\n"); }
  | rules endcb { yyerror ("No INPUT found before RULE.(s)\n"); }
  | rules { yyerror ("No INPUT found before RULE(s).\n"); }
  | no_rules { yyerror ("No RULEs found after INPUT.\n"); }
  | { yyerror ("No INPUT and RULE(s) found.\n");}
  ;

no_rules: begincb inputcb endcb
  | begincb inputcb
  | inputcb endcb
  | inputcb

begincb: /* Empty */
  TBEGIN {
    printf ("def BEGIN():");
  }
  ;

inputcb: TINPUT {
    printf ("def INPUT():");
  }
  ;

endcb: /* Empty */
  TEND {
    printf ("def END():");
  }
  ;

rules: rule | rules rule;
  ;

rule: TRULE TRULE_NAME TRULE_CONDT {
    if ($2 == NULL)
      yyerror ("RULE has no name.\n");

    if ($3 == NULL)
      printf ("RULES.APPEND('DEFAULT', '%s', lambda: True, '_RULE_%s()'))\n", $2, $3);
    else
      printf ("RULES.APPEND('DEFAULT', '%s', lambda: %s, '_RULE_%s()'))\n", $2, $3, $3);

    printf ("def _RULE_%s():", $2);

    XFREE ($2);
    XFREE ($3);
  }
  | TGROUP TGROUP_NAME TRULE TRULE_NAME TRULE_CONDT {
    if ($2 == NULL)
      yyerror ("GROUP has no name.\n");

    if (strncmp ($2, "RULE", 4) == 0)
      yyerror ("GROUP has no name.\n");

    if ($4 == NULL)
      yyerror ("RULE has no name.\n");

    if ($5 == NULL)
      printf ("RULES.APPEND('%s', '%s', '''True''', '_RULE_%s()'))\n", $2, $4, $5);
    else
      printf ("RULES.APPEND('%s', '%s', '''%s''', '_RULE_%s()'))\n", $2, $4, $5, $5);

    printf ("def _RULE_%s():", $4);

    XFREE ($2);
    XFREE ($4);
    XFREE ($5);
  }
  ;
%%

void
yyerror (const char *format, ...)
{
  extern int yylineno;
  va_list ap;
  va_start (ap, format);
  if (__script_name && !__script_name[0])
    fprintf (stderr, "ERROR[%s:%d]: ", __script_name, yylineno);
  else
    fprintf (stderr, "ERROR[line %d]: ", yylineno);
  vfprintf (stderr, format, ap);
  va_end (ap);
  exit (1);
}

int
bng_parse (const char* script_name)
{
  //  __script_name = script_name;
  yyparse ();

  return (0);
}

#ifdef _DEBUG_PARSER
int
main (void)
{
  bng_parse ("test.bng");
  return 0;
}

#endif
