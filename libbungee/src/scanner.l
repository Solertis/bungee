/*
  scanner.l: bungee's flex scanner

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
%x bquote bmlquote bgroupname brulename brulecondt
%option yylineno noyywrap warn
/* %option debug */

%{
#include <string.h>
#include <stdlib.h>

#ifdef _DEBUG_PARSER /* Support hand-compiled testing for quick debugging */
#include "parser.tab.h"
#else
#include "parser.h" /* Automake auto-generates parser.h and not default style parser.tab.h */
#endif

/* Uncomment this line instead if you want to DEBUG */
//#define BRETURN(val) do { _print_token (val); return val; } while (0)
#define BRETURN(val) return val

/* handle locations */
int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
  yylloc.first_column = yycolumn; \
  yylloc.last_column = yycolumn+yyleng-1; \
  yycolumn += yyleng-1;

static inline char _input_ch (void) __attribute__ ((unused));
static unsigned int _get_indent_len (char *stmt, int len) __attribute__ ((unused));
static int _eat_up_spaces (void) __attribute__ ((unused));
static inline void _print_token (enum yytokentype val) __attribute__ ((unused));

static unsigned char slquote_type='\0', mlquote_type='\0';
static unsigned long sl_start, ml_start;
static unsigned char _begin_found, _input_found, _end_found;
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
  slquote_type='\"';
  ECHO;
  BEGIN (bquote);
}

\' {
  sl_start = yylineno+1;
  slquote_type='\'';
  ECHO;
  BEGIN (bquote);
}

<bquote>{
  [^\\\"\'\n]* ECHO;  /* Consume anything that's not a '*' or newline */

  \\\n ; /* Remove newline chars in single quote text */

  \" {    /* End of quote */
    if (slquote_type == '\"')
      {
        ECHO;
        slquote_type='\0';
        sl_start = 0;
        BEGIN (INITIAL);
      }
  }

  \' {    /* End of quote */
    if (slquote_type == '\'')
      {
        ECHO;
        slquote_type='\0';
        sl_start = 0;
        BEGIN (INITIAL);
      }
    }
  <<EOF>> { /* Unterminated quote */
    if (sl_start != 0)
      return yyerror ("ERROR: Unterminated quote in line %ld", sl_start);
  }
}

\'\'\' { /* Multi line quote. */
  ml_start=yylineno+1;
  mlquote_type='\'';
  BEGIN (bmlquote);
  ECHO;
}

\"\"\" { /* Multi line quote. */
  ml_start=yylineno+1;
  mlquote_type='\"';
  ECHO;
  BEGIN (bmlquote);
}

<bmlquote>{
  [^\\\'\"]* { /* Multi line quote. */
    ECHO;
  }

  (\'\'\') { /* End of multi line quote. */
    ECHO;
    if (mlquote_type == '\'')
      {
        mlquote_type='\0';
        ml_start = 0;
        BEGIN (INITIAL);
      }
  }

  (\"\"\") { /* End of multi line quote. */
    ECHO;
    if (mlquote_type == '\"')
      {
        mlquote_type='\0';
        ml_start = 0;
        BEGIN (INITIAL);
      }
  }
  <<EOF>> { /* Unterminated quote */
    if (ml_start != 0)
      return yyerror ("ERROR: Unterminated quote in line %ld", ml_start);
  }
}

^(BEGIN[ \t]*\:) { /* BEGIN: block */
  if (_begin_found)
    return yyerror ("Duplicate BEGIN section found.\n");
  _begin_found=1;

  BRETURN (TBEGIN);
}

[ \t]+BEGIN[ \t]*\: { /* Error Case */
  ECHO;
  return yyerror ("BEGIN keyword should start at the beginning of line.\n");
}

^(INPUT[ \t]*\:) { /* INPUT: block */
  if (_input_found)
    return yyerror ("Duplicate INPUT section found.\n");
  _input_found=1;

  BRETURN (TINPUT);
}

[ \t]+INPUT[ \t]*\: { /* Error Case */
  ECHO;
  return yyerror ("INPUT keyword should start at the beginning of line.\n");
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
  ECHO;
  return yyerror ("GROUP keyword should start at the beginning of line.\n");
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
  \: {
    yyless (0);
    yylval.string = '\0';
    BEGIN (brulecondt);
    BRETURN (TRULE_NAME);
  }
}

<brulecondt>{
  [^\:\n]*[\:] { /* Rule Condition */
    yytext[yyleng-1] = '\0'; /* Discard ':' */
    yylval.string = strndup (yytext, yyleng);
    BEGIN (INITIAL);
    BRETURN (TRULE_CONDT);
  }
}

^([ \t]+RULE[ \t]+) { /* Error Case */
  ECHO;
  return yyerror ("RULE keyword should start at the beginning of line.\n");
}

^(END[ \t]*\:) { /* END: block */
  if (_end_found)
    return yyerror ("Duplicate END section found.\n");
  _end_found=1;

  BRETURN (TEND);
}

[ \t]+END[ \t]*\: { /* Error Case */
  ECHO;
  return yyerror ("END keyword should start at the beginning of line.\n");
}

\$\$ { /* Dictionary of all BUNGEE global variables. */
  fprintf(yyout, "BUNGEE.GLOBALS");
}

\$\* { /* All field values in a list. */
  fprintf(yyout, "BUNGEE.GLOBALS.items()");
}

\$\@ { /* All field symbols in a list. */
  fprintf(yyout, "BUNGEE.GLOBALS.keys()");
}

\$\# { /* Number of fields. */
  fprintf(yyout, "len(BUNGEE.GLOBALS)");
}

\$[a-zA-Z_][a-zA-Z_0-9]* { /* Global variable */
  fprintf(yyout, "BUNGEE.GLOBALS['%s']", yytext+1);
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

/* Lex's input() wrapper to safely handle location tacking */
static inline char
_input_ch (void)
{
  yycolumn++;;
  return input ();
}

static int
_eat_up_spaces (void)
{
  register int ch;
  for (;;)
    {
      ch = _input_ch ();
      if ((ch == ' ') || (ch == '\t'))
	  continue;
      else if (ch == EOF)
	return yyerror ("Unexpected end of file while eating spaces.\n");
      else
	{
	  unput (ch);
	  break;
	}
    }
  return 0;
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

/* Initialize flex scanner. */
void
_bng_scanner_init (void)
{
  slquote_type = mlquote_type='\0';
  sl_start = ml_start = 0;
  _begin_found = _input_found = _end_found = 0;
}

#if 0
int
main (void)
{
  /* uncomment debug BRETURN macro definition */
  yylex ();
}
#endif
