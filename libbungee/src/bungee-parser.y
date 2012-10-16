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

/* Pass the argument to yyparse through to yylex. */
#define XFREE(ptr) if (ptr) { free (ptr); ptr = NULL;}

/* Current yyin script source */
const char *__script_name=NULL;
extern int yylineno;

/* Lex will pass expr or regex via this variable. You are responsible for freeing it. */
char *group_name=NULL, *rule_name=NULL, *rule_condt=NULL;
 %}

/*** Bison declarations ***/
/* Start symbol */
%start program
%error-verbose
/* %locations
%initial-action
{
  // Initialize the initial location.
  @$.filename = &__script_name;
};
 */

/* %pure_parser */

%union {
  struct {
    char *name;
  } group;
  struct {
    char *name;
    char *condt;
  } rule;
}

/** Terminal symbols **/
/* Terminal symbols with no value */
%token TBEGIN TINPUT TEND TGROUP
/* Terminal symbols with $1.name (yylval.group.name) value */
%token <group> TGROUP_NAME
/* Terminal symbols with $1.name (yylval.rule.name) and $1.condt (yylval.rule.condt) value */
%token <rule> TRULE

/*  Free heap based token values during error recovery */
%destructor { XFREE ($$.name); XFREE ($$.condt) } <rule>
%destructor { XFREE ($$.name); } <group>

/* Grammar Rules */
%%
program: inputcb rules /* valid case */
  | begincb inputcb rules /* valid case */
  | inputcb rules endcb /* valid case */
  | begincb inputcb rules endcb /* valid case */
  | begincb rules endcb { yyerror ("No INPUT found after BEGIN\n"); }
  | begincb rules { yyerror ("No INPUT found after BEGIN\n"); }
  | rules endcb { yyerror ("No INPUT found before RULE(s)\n"); }
  | rules { yyerror ("No INPUT found before RULE(s)\n"); }
  | no_rules { yyerror ("No RULEs found after INPUT\n"); }
  | { yyerror ("No INPUT and RULE(s) found\n");}
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

rule: TRULE {
    if ($1.name == NULL)
      yyerror ("ERROR: RULE has no name\n");

    if ($1.condt == NULL)
      printf ("RULES.APPEND('DEFAULT', '%s', '''True''', '_RULE_%s()'))\n", $1.name,$1.condt);
    else
      printf ("RULES.APPEND('DEFAULT', '%s', '''%s''', '_RULE_%s()'))\n", $1.name, $1.condt, $1.condt);

    {
      int i;
      printf ("[DEBUG %d %d %d %d]", @TRULE.first_line, @TRULE.first_column, @TRULE.last_line, @TRULE.last_column);
      for (i=0; i<@1.first_column; i++)	{ putchar (' '); }
    }
    printf ("def _RULE_%s():", $1.name);

    XFREE ($1.name);
    XFREE ($1.condt);
  }
  | TGROUP TGROUP_NAME TRULE {
    if ($2.name == NULL)
      yyerror ("ERROR: GROUP has no name.\n");

    if (strncmp ($2.name, "RULE", 4) == 0)
      yyerror ("ERROR: GROUP has no name.\n");

    if ($3.name == NULL)
      yyerror ("ERROR: RULE has no name.\n");

    if ($3.condt == NULL)
      printf ("RULES.APPEND('%s', '%s', '''True''', '_RULE_%s()'))\n", $2.name, $3.name, $3.condt);
    else
      printf ("RULES.APPEND('%s', '%s', '''%s''', '_RULE_%s()'))\n", $2.name, $3.name, $3.condt, $3.condt);

    printf ("def _RULE_%s():", $3.name);

    printf ("[DEBUG %d %d %d %d]", @2.first_line, @2.first_column, @2.last_line, @2.last_column);
    XFREE ($2.name);
    XFREE ($3.name);
    XFREE ($3.condt);
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
