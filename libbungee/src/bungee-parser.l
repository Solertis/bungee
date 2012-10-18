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
%x bstring bmlstring bgroupname brulename brulecondt
%option yylineno noyywrap warn
/* %option bison-bridge bison-locations */
/* %option debug */
/* %option   8bit reentrant */

%{
#include <string.h>
#include <stdlib.h>
#include "bungee-parser.h"
#include "bungee-parser.tab.h"

char *rule_condt;
static inline void _print_token (enum yytokentype val) __attribute__ ((unused));
static unsigned int _get_indent_len (char *stmt, int len) __attribute__ ((unused));
static void _eat_up_spaces (void) __attribute__ ((unused));

/* Uncomment this line instead if you want to DEBUG */
//#define BRETURN(val) do { _print_token (val); return val; } while (0)
#define BRETURN(val) return val
static unsigned char slstring_type='\0', mlstring_type='\0';
unsigned long sl_start, ml_start;

/* handle locations */
int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
  yylloc.first_column = yycolumn; \
  yylloc.last_column = yycolumn+yyleng-1; \
  yycolumn += yyleng-1;
%}

%%
\n {
  yycolumn = 1;
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

[ \t]+BEGIN[ \t]*\: { /* Error Case */
  yyerror ("BEGIN keyword should start at the beginning of line.\n");
}

^(INPUT[ \t]*\:) { /* INPUT: block */
  BRETURN (TINPUT);
}

[ \t]+INPUT[ \t]*\: { /* Error Case */
  yyerror ("INPUT keyword should start at the beginning of line.\n");
}

^(GROUP) {
  _eat_up_spaces ();
  BEGIN (bgroupname);
  BRETURN (TGROUP);
}

<bgroupname>{
  [a-zA-Z0-9_]+ { /* Group Name */
    yylval.string = strndup (yytext, yyleng);
    if (strncmp (yylval.string, "RULE", 4) == 0)
      {
        free (yylval.string);
        yylval.string = NULL;
        yyless (0);
      }
    _eat_up_spaces ();
    BEGIN (INITIAL);
    BRETURN (TGROUP_NAME);
  }
}

[ \t]+GROUP[ \t]+ { /* Error Case */
  yyerror ("GROUP keyword should start at the beginning of line.\n");
}

RULE { /* Rule Keyword */
  _eat_up_spaces ();
  BEGIN (brulename);
  BRETURN (TRULE);
}

<brulename>{
  [a-zA-Z0-9_]+ { /* Rule Name */
    yylval.string = strndup (yytext, yyleng);
    _eat_up_spaces ();
    BEGIN (brulecondt);
    BRETURN (TRULE_NAME);
  }
}

<brulecondt>{
  [^ \t\:]+ { /* Rule Condition */
    yylval.string = strndup (yytext, yyleng);
    _eat_up_spaces ();
    BEGIN (INITIAL);
    BRETURN (TRULE_CONDT);
  }
}

^([ \t]+RULE[ \t]+) { /* Error Case */
  yyerror ("RULE keyword should start at the beginning of line.\n");
}

^(END[ \t]*\:) { /* END: block */
  BRETURN (TEND);
}

[ \t]+END[ \t]*\: { /* Error Case */
  yyerror ("END keyword should start at the beginning of line.\n");
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

[ \t]+ ECHO;
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

static void
_eat_up_spaces (void)
{
  register int ch;
  for (;;)
    {
      ch = input ();
      if ((ch == ' ') || (ch == '\t'))
	{
	  yycolumn++;
	  continue;
	}
      else if (ch == EOF)
	{
	  yyerror ("Unexpected end of file while eating spaces.\n");
	}
      else
	{
	  unput (ch);
	  break;
	}
    }
}

static unsigned int
_get_indent_len (char *stmt, int len)
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

#if 0
int
main (void)
{
  /* uncomment debug BRETURN macro definition */
  yylex ();
}
#endif
