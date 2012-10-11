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
%{
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

#include "bungee-parser.h"

/* Lex will pass expr or regex via this variable. You are responsible for freeing it. */
char *group_name, *rule_name, *rule_condt;
#define XFREE(ptr) if (ptr) { free (ptr); }
%}

/*** Bison declarations ***/
/* Start symbol */
%start program

/* Union is declared as C Union instead in bungee-parser.h file */
/* Terminal symbols */
%token <id> TGLOBAL TBEGIN TINPUT TEND TGROUP TRULE

/* Grammar Rules */
%%
program: begincb inputcb rules endcb | error { yyerror ("INPUT block missing"); }
  ;

begincb: /* Empty */
  | TBEGIN {
    printf ("def BEGIN():");
  }
  ;

inputcb: TINPUT {
    printf ("def INPUT():");
  }
  ;

endcb: /* Empty */
  | TEND {
    printf ("def END():");
  }
  ;

rules: rule | rules rule;
  ;

rule: TRULE {
    if (rule_name == NULL)
      yyerror ("ERROR: RULE has no name\n");

    if (rule_condt == NULL)
      yyerror ("ERROR: RULE %s has no condition\n", rule_name);

    printf ("_BNG_RULES['DEFAULT'].append(('%s', '_RULE_%s()'))\n", rule_name, rule_condt);
    printf ("def _RULE_%s():", rule_name);

    XFREE (rule_name);
    XFREE (rule_condt);
  }
  | TGROUP TRULE {
    if (group_name == NULL)
      yyerror ("ERROR: GROUP has no name.\n");

    if (rule_name == NULL)
      yyerror ("ERROR: RULE has no name.\n");

    if (rule_condt == NULL)
      yyerror ("ERROR: RULE [%s] has no condition.\n", rule_name);

    printf ("_BNG_RULES['%s'].append(('%s', '_RULE_%s()'))\n", group_name, rule_name, rule_condt);
    printf ("def _RULE_%s():", rule_name);

    XFREE (group_name);
    XFREE (rule_name);
    XFREE (rule_condt);
  }
  ;
%%

void
yyerror (const char *format, ...)
{
  extern int yylineno;
  va_list ap;
  va_start (ap, format);
  fprintf (stderr, "ERROR[line %d]: ", yylineno);
  vfprintf (stderr, format, ap);
  putchar ('\n');
  va_end (ap);
  exit (1);
}

#if 0

int
main (void)
{
  yyparse ();
  return (0);
}

#endif
