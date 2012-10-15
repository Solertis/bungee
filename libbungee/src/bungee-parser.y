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

#define XFREE(ptr) if (ptr) { free (ptr); }

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
%token TBEGIN TINPUT TEND
/* Terminal symbols with $1.name (yylval.group.name) value */
%token <group> TGROUP
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
      yyerror ("ERROR: RULE %s has no condition\n", $1.name);

    printf ("_BNG_RULES['DEFAULT'].append(('%s', '_RULE_%s()'))\n", $1.name, $1.condt);
    {
      int i;
      for (i=0; i<@1.first_column; i++)
	putchar (' ');
    }
    printf ("def _RULE_%s():", $1.name);

    XFREE ($1.name);
    XFREE ($1.condt);
  }
  | TGROUP TRULE {
    if ($1.name == NULL)
      yyerror ("ERROR: GROUP has no name.\n");

    if (strncmp ($1.name, "RULE", 4) == 0)
      yyerror ("ERROR: GROUP has no name.\n");

    if ($2.name == NULL)
      yyerror ("ERROR: RULE has no name.\n");

    if ($2.condt == NULL)
      yyerror ("ERROR: RULE [%s] has no condition.\n", $2.name);

    printf ("_BNG_RULES['%s'].append(('%s', '_RULE_%s()'))\n", $1.name, $2.name, $2.condt);
    printf ("def _RULE_%s():", $2.name);

    XFREE ($1.name);
    XFREE ($2.name);
    XFREE ($2.condt);
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

  return 9;
}

#ifdef _DEBUG_PARSER
int
main (void)
{
  bng_parse ("test.bng");
  return 0;
}

#endif
