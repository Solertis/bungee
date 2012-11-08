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

/* General Flex options. */
%option yylineno noyywrap warn stack

/* Make our scanner and parser re-entrant. This optin tells Bison to
   pass parameters appropriately. */
%option reentrant bison-locations bison-bridge

/* Thread specific local variables are stored in this structure.
   Use "yyget_extra (yyscanner)->" to get access to this structure. */
%option extra-type=" local_vars_t *"

/* Generate header file for all the function definitions. */
/* %option header-file=scanner.h */

/* Start Conditions. */
%x bquote bmlquote bgroupname brulename brulecondt

/* %option debug */

%{
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* Disable warnings in the flex generated code that are safe to ignore. */
#if defined __GNUC__
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#ifdef _DEBUG_PARSER /* Support hand-compiled testing for quick debugging. */
#include "parser.tab.h"
#else
#include "parser.h" /* Automake auto-generates parser.h and not default style parser.tab.h */
#endif

/* Uncomment this line instead if you want to DEBUG */
//#define BRETURN(val) do { _print_token (val); return val; } while (0)
#define BRETURN(val) return val

/* Handle locations. User action macro is always executed prior to the matched rule's action. */
#define YY_USER_ACTION \
   yyget_lloc (yyscanner)->first_line = yyget_lloc (yyscanner)->last_line = yyget_lineno (yyscanner); \
   yyget_lloc (yyscanner)->first_column = yyget_lloc (yyscanner)->_column; \
   yyget_lloc (yyscanner)->last_column = yyget_lloc (yyscanner)->_column+yyget_leng (yyscanner)-1; \
   yyget_lloc (yyscanner)->_column += yyget_leng (yyscanner)-1;

/* Flex returns this code upon error */
#define YYERRCODE  256

/* Error handling routine */
int yyerror (yyscan_t yyscanner, const char *format, ...) __attribute__ ((format (gnu_printf, 2, 3)));
static inline char _input_ch (yyscan_t yyscanner);
static unsigned int _get_indent_len (char *stmt, int len);
static int _eat_up_spaces (yyscan_t yyscanner);
static inline void _print_token (enum yytokentype val);
%}

%%
\n {
  yyget_lloc (yyscanner)->_column = 1;
  ECHO;
}

\#+[^\n\\]* { /* Comment line. */
   ECHO;
}

\" {
  yyget_extra (yyscanner)->quote.sl_start = yyget_lineno (yyscanner)+1;
  yyget_extra (yyscanner)->quote.slquote_type = '\"';
  ECHO;
  BEGIN (bquote);
}

\' {
  yyget_extra (yyscanner)->quote.sl_start = yyget_lineno (yyscanner)+1;
  yyget_extra (yyscanner)->quote.slquote_type = '\'';
  ECHO;
  BEGIN (bquote);
}

<bquote>{
  [^\\\"\'\n]* ECHO;  /* Consume anything that's not a '*' or newline */

  \\\n ; /* Remove newline chars in single quote text */

  \" {    /* End of quote */
    if (yyget_extra (yyscanner)->quote.slquote_type == '\"')
      {
        ECHO;
        yyget_extra (yyscanner)->quote.slquote_type ='\0';
        yyget_extra (yyscanner)->quote.sl_start = 0;
        BEGIN (INITIAL);
      }
  }

  \' {    /* End of quote */
    if (yyget_extra (yyscanner)->quote.slquote_type == '\'')
      {
        ECHO;
        yyget_extra (yyscanner)->quote.slquote_type ='\0';
        yyget_extra (yyscanner)->quote.sl_start = 0;
        BEGIN (INITIAL);
      }
    }
  <<EOF>> { /* Unterminated quote */
    if (yyget_extra (yyscanner)->quote.sl_start != 0)
      return yyerror (yyscanner, "ERROR: Unterminated quote in line %ld", yyget_extra (yyscanner)->quote.sl_start);
  }
}

\'\'\' { /* Multi line quote. */
  yyget_extra (yyscanner)->quote.ml_start = yyget_lineno (yyscanner)+1;
  yyget_extra (yyscanner)->quote.mlquote_type = '\'';
  BEGIN (bmlquote);
  ECHO;
}

\"\"\" { /* Multi line quote. */
  yyget_extra (yyscanner)->quote.ml_start = yyget_lineno (yyscanner)+1;
  yyget_extra (yyscanner)->quote.mlquote_type = '\"';
  ECHO;
  BEGIN (bmlquote);
}

<bmlquote>{
  [^\\\'\"]* { /* Multi line quote. */
    ECHO;
  }

  (\'\'\') { /* End of multi line quote. */
    ECHO;
    if (yyget_extra (yyscanner)->quote.mlquote_type == '\'')
      {
        yyget_extra (yyscanner)->quote.mlquote_type ='\0';
        yyget_extra (yyscanner)->quote.ml_start = 0;
        BEGIN (INITIAL);
      }
  }

  (\"\"\") { /* End of multi line quote. */
    ECHO;
    if (yyget_extra (yyscanner)->quote.mlquote_type == '\"')
      {
        yyget_extra (yyscanner)->quote.mlquote_type ='\0';
        yyget_extra (yyscanner)->quote.ml_start = 0;
        BEGIN (INITIAL);
      }
  }
  <<EOF>> { /* Unterminated quote */
    if (yyget_extra (yyscanner)->quote.ml_start != 0)
      return yyerror (yyscanner, "ERROR: Unterminated quote in line %ld", yyget_extra (yyscanner)->quote.ml_start);
  }
}

^(BEGIN[ \t]*\:) { /* BEGIN: block */
  if (yyget_extra (yyscanner)->found.begin)
    return yyerror (yyscanner, "Duplicate BEGIN section found.\n");
  yyget_extra (yyscanner)->found.begin = 1;

  BRETURN (TBEGIN);
}

[ \t]+BEGIN[ \t]*\: { /* Error Case */
  ECHO;
  return yyerror (yyscanner, "BEGIN keyword should start at the beginning of line.\n");
}

^(INPUT[ \t]*\:) { /* INPUT: block */
  if (yyget_extra (yyscanner)->found.input)
    return yyerror (yyscanner, "Duplicate INPUT section found.\n");
  yyget_extra (yyscanner)->found.input = 1;

  BRETURN (TINPUT);
}

[ \t]+INPUT[ \t]*\: { /* Error Case */
  ECHO;
  return yyerror (yyscanner, "INPUT keyword should start at the beginning of line.\n");
}

^(GROUP) {
  _eat_up_spaces (yyscanner);
  BEGIN (bgroupname);
  BRETURN (TGROUP);
}

<bgroupname>{
  [a-zA-Z0-9_]+ { /* Group Name */
    yyget_lval(yyscanner)->string = strndup (yyget_text (yyscanner), yyget_leng (yyscanner));
    if (strncmp (yyget_lval(yyscanner)->string, "RULE", 4) == 0)
      {
        free (yyget_lval(yyscanner)->string);
        yyget_lval(yyscanner)->string = NULL;
        yyless (0);
      }
    _eat_up_spaces (yyscanner);
    BEGIN (INITIAL);
    BRETURN (TGROUP_NAME);
  }
}

[ \t]+GROUP[ \t]+ { /* Error Case */
  ECHO;
  return yyerror (yyscanner, "GROUP keyword should start at the beginning of line.\n");
}

RULE { /* Rule Keyword */
  _eat_up_spaces (yyscanner);
  BEGIN (brulename);
  BRETURN (TRULE);
}

<brulename>{
  [a-zA-Z0-9_]+ { /* Rule Name */
    yyget_lval(yyscanner)->string = strndup (yyget_text (yyscanner), yyget_leng (yyscanner));
    _eat_up_spaces (yyscanner);
    BEGIN (brulecondt);
    BRETURN (TRULE_NAME);
  }
  \: {
    yyless (0);
    yyget_lval(yyscanner)->string = '\0';
    BEGIN (brulecondt);
    BRETURN (TRULE_NAME);
  }
}

<brulecondt>{
  [^\:\n]*[\:] { /* Rule Condition */
    yyget_text (yyscanner)[yyget_leng (yyscanner)-1] = '\0'; /* Discard ':' */
    yyget_lval(yyscanner)->string = strndup (yyget_text (yyscanner), yyget_leng (yyscanner));
    BEGIN (INITIAL);
    BRETURN (TRULE_CONDT);
  }
}

^([ \t]+RULE[ \t]+) { /* Error Case: Global RULE starting with space. */
  ECHO;
  return yyerror (yyscanner, "RULE keyword should start at the beginning of line.\n");
}

^(END[ \t]*\:) { /* END: block */
  if (yyget_extra (yyscanner)->found.end)
    return yyerror (yyscanner, "Duplicate END section found.\n");
  yyget_extra (yyscanner)->found.end = 1;

  BRETURN (TEND);
}

[ \t]+END[ \t]*\: { /* Error Case */
  ECHO;
  return yyerror (yyscanner, "END keyword should start at the beginning of line.\n");
}

\$\$ { /* Dictionary of all BUNGEE global variables. */
  fprintf(yyget_out (yyscanner), "BUNGEE.GLOBALS");
}

\$\* { /* All field values in a list. */
  fprintf(yyget_out (yyscanner), "BUNGEE.GLOBALS.items()");
}

\$\@ { /* All field symbols in a list. */
  fprintf(yyget_out (yyscanner), "BUNGEE.GLOBALS.keys()");
}

\$\# { /* Number of fields. */
  fprintf(yyget_out (yyscanner), "len(BUNGEE.GLOBALS)");
}

\$[a-zA-Z_][a-zA-Z_0-9]* { /* Global variable */
  fprintf(yyget_out (yyscanner), "BUNGEE.GLOBALS['%s']", yyget_text (yyscanner)+1);
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
_input_ch (yyscan_t yyscanner)
{
  yyget_lloc (yyscanner)->_column++;;
  return input (yyscanner);
}

static int
_eat_up_spaces (yyscan_t yyscanner)
{
  struct yyguts_t * yyg = (struct yyguts_t*)yyscanner;
  register int ch;

  for (;;)
    {
      ch = _input_ch (yyscanner);
      if ((ch == ' ') || (ch == '\t'))
	  continue;
      else if (ch == EOF)
	return yyerror (yyscanner, "Unexpected end of file while eating spaces.\n");
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

int
yyerror (yyscan_t yyscanner, const char *format, ...)
{
  if (!format)
    return YYERRCODE;

  local_vars_t *locals = yyget_extra (yyscanner);
  if (locals->err_fp)
    {
      va_list ap;
      va_start (ap, format);

      if (locals->script_name && locals->script_name[0])
	fprintf (locals->err_fp, "ERROR[%s:%d]: ", locals->script_name, yyget_lineno (yyscanner));
      else
	fprintf (locals->err_fp, "ERROR[line %d]: ", yyget_lineno (yyscanner));

      vfprintf (locals->err_fp, format, ap);
      va_end (ap);
    }
  // exit (1);
  return YYERRCODE;
}

#if 0
int
main (void)
{
  /* uncomment debug BRETURN macro definition */
  yylex ();
}
#endif
