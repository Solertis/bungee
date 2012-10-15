/*
  bungee-parser.l: bungee script flex scanner

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
%x bstring bmlstring bgroupname
%option yylineno noyywrap noinput nounput
 // %define api.pure

%{
#include <string.h>
#include <stdlib.h>
#include "bungee-parser.h"
#include "bungee-parser.tab.h"

char *rule_condt;
static inline void _print_token (enum yytokentype val);

/* Uncomment this line instead if you want to DEBUG */
//#define BRETURN(val) do { _print_token (val); return val; } while (0)
#define BRETURN(val) return val
static unsigned char slstring_type='\0', mlstring_type='\0';
extern char *group_name, *rule_name, *rule_condt;
unsigned long sl_start, ml_start;
%}

%%
\n {
  ECHO;
}

\#+[^\n\\]* { /* Comment line. */
   ECHO;
}

\" {
  sl_start = yylineno+1;
  slstring_type='\"';
  ECHO;
  BEGIN (bstring);
}

\' {
  sl_start = yylineno+1;
  slstring_type='\'';
  ECHO;
  BEGIN (bstring);
}

<bstring>{
  [^\\\"\'\n]* ECHO;  /* Consume anything that's not a '*' or newline */

  \\\n ; /* Remove newline chars in single quote strings */

  \" {    /* End of string */
    if (slstring_type == '\"')
      {
        ECHO;
        slstring_type='\0';
        sl_start = 0;
        BEGIN (INITIAL);
      }
  }

  \' {    /* End of string */
    if (slstring_type == '\'')
      {
        ECHO;
        slstring_type='\0';
        sl_start = 0;
        BEGIN (INITIAL);
      }
    }
  <<EOF>> { /* Unterminated quote */
    if (sl_start != 0)
      yyerror ("ERROR: Unterminated quote in line %ld", sl_start);
  }
}

\'\'\' { /* Multi line string. */
  ml_start=yylineno+1;
  mlstring_type='\'';
  BEGIN (bmlstring);
  ECHO;
}

\"\"\" { /* Multi line string. */
  ml_start=yylineno+1;
  mlstring_type='\"';
  ECHO;
  BEGIN (bmlstring);
}

<bmlstring>{
  [^\\\'\"]* { /* Multi line string. */
    ECHO;
  }

  (\'\'\') { /* End of multi line string. */
    ECHO;
    if (mlstring_type == '\'')
      {
        mlstring_type='\0';
        ml_start = 0;
        BEGIN (INITIAL);
      }
  }

  (\"\"\") { /* End of multi line string. */
    ECHO;
    if (mlstring_type == '\"')
      {
        mlstring_type='\0';
        ml_start = 0;
        BEGIN (INITIAL);
      }
  }
  <<EOF>> { /* Unterminated quote */
    if (ml_start != 0)
      yyerror ("ERROR: Unterminated quote in line %ld", ml_start);
  }
}

^(BEGIN[ \t]*\:) { /* BEGIN: block */
  BRETURN (TBEGIN);
}

^(INPUT[ \t]*\:) { /* INPUT: block */
  BRETURN (TINPUT);
}

^(GROUP[ \t]+[a-zA-Z0-9_]+[ \t]+) {
  /* GROUP GroupName */
  yytext[yyleng-1]='\0'; /* Skip ":" */
  sscanf (yytext, "%*s %as", &yylval.group.name);
  BRETURN (TGROUP);
 }

RULE[ \t]+[a-zA-Z0-9_]+(([ \t]+[a-zA-Z0-9_]+)|([ \t]*))\: { /* RULE RuleName [Condtion]: */
  yytext[yyleng-1]='\0'; /* Skip ":" */
  sscanf (yytext, "%*s %as %as", &yylval.rule.name, &yylval.rule.condt);
  BRETURN (TRULE);
}

^(END[ \t]*\:) { /* END: block */
  BRETURN (TEND);
}

\$\* { /* All field values in a list. */
  fprintf(yyout, "_BNG_GLOBALS.items()");
}

\$\@ { /* All field symbols in a list. */
  fprintf(yyout, "_BNG_GLOBALS.keys()");
}

\$\# { /* Number of fields. */
  fprintf(yyout, "len(_BNG_GLOBALS.keys())");
}

\$[a-zA-Z_][a-zA-Z_0-9]+ { /* Global variable */
  fprintf(yyout, "_BNG_GLOBALS['%s']", yytext);
}
%%

static inline void _print_token (enum yytokentype val)
{
  switch (val)
    {
      case TBEGIN:
        printf ("<TBEGIN>");
        break;
      case TINPUT:
        printf ("<TINPUT>");
        break;
      case TEND:
        printf ("<TEND>");
        break;
      case TGROUP:
        printf ("<TGROUP>");
        break;
      case TRULE:
        printf ("<TRULE>");
        break;
      default:
        printf ("<UNDEFINED>");
    }
}

#if 0
static unsigned int
get_indent_len (char *stmt, int len)
{
  unsigned int indent_len, i;
  for (indent_len=0, i=0; i < len; i++)
    {
      if ((stmt[i] == ' ') || (stmt[i] == '\t'))
        indent_len++;
      else
	return indent_len;
    }
  return indent_len;
}
#endif

#if 0
int
main (void)
{
  /* uncomment debug BRETURN macro definition */
  yylex ();
}
#endif
