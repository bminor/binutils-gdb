%{
/* Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 *  $Id$ 

 *
*/



/*SUPPRESS 529*/
/*SUPPRESS 26*/
/*SUPPRESS 29*/
#define LEXDEBUG 0
#include "sysdep.h"
#include "bfd.h"

#include <ctype.h>
#include "ldlex.h"

#include "ld.h"
#include "ldexp.h"
#include "ldgram.tab.h"
#include "ldmisc.h"

#undef input
#undef unput
#define input lex_input
#define unput lex_unput
int debug;

extern boolean ldgram_in_expression;
extern boolean ldgram_in_defsym;
extern boolean ldgram_in_script;
static char *command_line;

extern int fgetc();
extern int yyparse();

typedef struct {
	char *name;	
int value;
} keyword_type;
#define RTOKEN(x)  {  yylval.token = x; return x; }
keyword_type keywords[] = 
{
"MEMORY",MEMORY,
"ORIGIN",ORIGIN,
"BLOCK",BLOCK,
"LENGTH",LENGTH,
"ALIGN",ALIGN_K,
"SUBSECTION_ALIGN",SUBSECTION_ALIGN,
"ADDR",ADDR,
"ENTRY",ENTRY,
"SCRIPT", SCRIPT,
"ENDSCRIPT", ENDSCRIPT,
"NEXT",NEXT,
"MAP",MAP,
"SIZEOF",SIZEOF,
"TARGET",TARGET_K,
"SEARCH_DIR",SEARCH_DIR,
"OUTPUT",OUTPUT,
"INPUT",INPUT,
"DEFINED",DEFINED,
"CREATE_OBJECT_SYMBOLS",CREATE_OBJECT_SYMBOLS,
"FORCE_COMMON_ALLOCATION",FORCE_COMMON_ALLOCATION,
"SECTIONS",SECTIONS,
"FILL",FILL,
"STARTUP",STARTUP,
"OUTPUT_FORMAT",OUTPUT_FORMAT,
"HLL",HLL,
"SYSLIB",SYSLIB,
"FLOAT",FLOAT,
"LONG", LONG,
"SHORT", SHORT,
"BYTE", BYTE,
"NOFLOAT",NOFLOAT,
"o",ORIGIN,
"org",ORIGIN,
"l", LENGTH,
"len", LENGTH,
0,0};
unsigned int lineno;
extern boolean hex_mode;
FILE *ldlex_input_stack;
static unsigned int have_pushback;
#define NPUSHBACK 10
int pushback[NPUSHBACK];
int thischar;
extern char *ldfile_input_filename;
int donehash = 0;
int
lex_input()
{
  if (have_pushback > 0) 
      {
	have_pushback --;
	return thischar = pushback[have_pushback];
      }
  if (ldlex_input_stack) {
    thischar = fgetc(ldlex_input_stack);

    if (thischar == EOF)  {
      fclose(ldlex_input_stack);
      ldlex_input_stack = (FILE *)NULL;
      ldfile_input_filename = (char *)NULL;
      /* First char after script eof is a @ so that we can tell the grammer
	 that we've eft */
      thischar = '@';

    }
  }
  else if (command_line && *command_line)  {
    thischar = *(command_line++);
  }
  else {
 thischar = 0;
  }
  if(thischar == '\t') thischar = ' ';
  if (thischar == '\n') { thischar = ' '; lineno++; }
  return thischar ;
}

void
lex_unput(c)
int c;
{
  if (have_pushback > NPUSHBACK) {
    info("%F%P Too many pushbacks\n");
  }

  pushback[have_pushback] = c;
  have_pushback ++;
}


	int
yywrap()
	 { return 1; }
/*VARARGS*/

void
allprint(x) 
int x;
{
fprintf(yyout,"%d",x);
}

void
sprint(x) 
char *x;
{
fprintf(yyout,"%s",x);
}

int  thischar;

void parse_line(arg)
char *arg;
{
  command_line = arg;
  have_pushback = 0;
  yyparse();
}



void
parse_args(ac, av)
int ac;
char **av;
{
  char *p;
  int i;
  size_t size = 0;
  char *dst;
  debug = 1;
  for (i= 1; i < ac; i++) {
    size += strlen(av[i]) + 2; 
  }
  dst = p = (char *)ldmalloc(size + 2);
/* Put a space arount each option */


  for (i =1; i < ac; i++) {

    unsigned int s = strlen(av[i]);
  *dst++ = ' ';
    memcpy(dst, av[i], s);
    dst[s] = ' ';
    dst += s + 1;
  }
  *dst	= 0;
  parse_line(p);

  free(p);


}

long number(text, base)
char *text;
int base;
{
  unsigned  long l = 0;
  char *p;
  for (p = text; *p != 0; p++) {
    if (*p == 'K') {
      l =l * 1024;
    }
    else if(*p== 'M') {
      l =l * 1024 * 1024;
    }
    else {
      l =l * base;
      if (isdigit(*p))  {
	l += *p - '0';
      }
      else if (islower(*p)) {
	l += *p - 'a' + 10;
      }
      else {
	l += *p - 'A' + 10;
      }
    }
  }
  return l;
}
%}

%a 4000
%o 5000
FILENAMECHAR	[a-zA-Z0-9\/\.\-\_\+\=]
FILENAME	{FILENAMECHAR}+


WHITE		[ \t]+

%%

"@" { return ENDSCRIPT; }
"\ -defsym\ " { return OPTION_defsym; }
"\ -noinhibit_exec\ " { return OPTION_noinhibit_exec; }
"\ -format\ " { return OPTION_format; }
"\ -n\ "		{ return OPTION_n; }
"\ -r\ "		{ return OPTION_r; }
"\ -Ur\ "		{ return OPTION_Ur; }
"\ -o\ "		{ return OPTION_o; }
"\ -g\ "		{ return OPTION_g; }
"\ -e\ "		{ return OPTION_e; }
"\ -b\ "		{ return OPTION_b; }
"\ -dc\ "		{ return OPTION_dc; }
"\ -dp\ "		{ return OPTION_dp; }
"\ -d\ "		{ return OPTION_d; }
"\ -v\ "		{ return OPTION_v; }
"\ -M\ "		{ return OPTION_M; }
"\ -t\ "		{ return OPTION_t; }
"\ -X\ "		{ return OPTION_X; }
"\ -x\ "		{ return OPTION_x; }
"\ -c\ "		{ return OPTION_c; }
"\ -R\ "		{ return OPTION_R; }
"\ -u\ "		{ return OPTION_u; }
"\ -s\ "            { return OPTION_s; }
"\ -S\ "            { return OPTION_S; }
"\ -l"{FILENAME} {
	 	yylval.name = buystring(yytext+3);
		return OPTION_l; 
	}

"\ -L"{FILENAME} 	{ 
		yylval.name = buystring(yytext+3);
	 	return OPTION_L; 
	 }
"\ -Ttext\ "  {
		 yylval.name = ".text";
		 return OPTION_Texp;
	       }
"\ -Tdata\ "  {
		 yylval.name = ".data";
		 return OPTION_Texp;
	       }
"\ -Tbss\ "  {
		 yylval.name = ".bss";
		 return OPTION_Texp;
	       }

"\ -T"{FILENAME}  {
		 yylval.name = buystring(yytext+3);
		 return OPTION_Tfile;
	       }
"\ -T\ "          {
		 return OPTION_T;
	       }

"\ -F"{FILENAME}  {
		 return OPTION_F;
	       }
"\ -F\ "          {
		 return OPTION_F;
	       }

"\ -A"{FILENAME} {
                 yylval.name = buystring(yytext+3);
		 return OPTION_Aarch;
	       }
" " { }
"<<="		{ RTOKEN(LSHIFTEQ);}
">>="		{ RTOKEN(RSHIFTEQ);}
"||"		{ RTOKEN(OROR);}
"=="		{ RTOKEN(EQ);}
"!="		{ RTOKEN(NE);}
">="		{ RTOKEN(GE);}
"<="		{ RTOKEN(LE);}
"<<"		{ RTOKEN(LSHIFT);}
">>"		{ RTOKEN(RSHIFT);}
"+="		{ RTOKEN(PLUSEQ);}
"-="		{ RTOKEN(MINUSEQ);}
"*="		{ RTOKEN(MULTEQ);}
"/="		{ RTOKEN(DIVEQ);}
"&="		{ RTOKEN(ANDEQ);}
"|="		{ RTOKEN(OREQ);}
"&&"		{ RTOKEN(ANDAND);}
">"		{ RTOKEN('>');}
","		{ RTOKEN(',');}
"&"		{ RTOKEN('&');}
"|"		{ RTOKEN('|');}
"~"		{ RTOKEN('~');}
"!"		{ RTOKEN('!');}
"?"		{ RTOKEN('?');}
"*"		{ RTOKEN('*');}
"%"		{ RTOKEN('%');}
"<"		{ RTOKEN('<');}
">"		{ RTOKEN('>');}
"}"		{ RTOKEN('}') ; }
"{"		{ RTOKEN('{'); }
")"		{ RTOKEN(')');}
"("		{ RTOKEN('(');}
"]"		{ RTOKEN(']');}
"["		{ RTOKEN('[');}
":"		{ RTOKEN(':'); }
";"		{ RTOKEN('\;');}
"-"		{ RTOKEN('-');}



"/*"		{ 
  while (1) {
    int ch;
    ch = input();
    while (ch != '*') {
      ch = input();
    }



    if (input() == '/') {
      break;
    }
    unput(yytext[yyleng-1]);
  }
}

"\""[^\"]*"\"" {

  yylval.name = buystring(yytext+1);
  yylval.name[yyleng-2] = 0; /* Fry final quote */
  return NAME;
}

"\#"{WHITE}*{FILENAMECHAR}+ {
  char *p = yytext+1;
  while(*p ==' ' || *p == '\t') p++;
  yylval.name = buystring(p);
  return NAME;
}
{FILENAMECHAR} {

  boolean loop = false;
  /*
    Tokenize a name, this is really pain, since a name can be a
    filename or a symbol name. filenames have slashes and stuff whist
    in an expression those things are seperate tokens. We hack this by
    setting ldlang_in_script when we are expecting a symbol, so that
    [/+-] get taken to be seperate tokens. An extra gotcha is
    expressions after defsyms, we only allow +s and -s in a defsym
    expression, so -defsym foo=bar+9 /file.o is parsed ok.
    
    The more I think about this the more I hate it. I've got a problem
    now with the = sign, what should I do ? imagine:
    __start=.;
    You'd think that was pretty unambiguous wouldn't you. Well it's
    not since __start=. is (at the moment) a perfectly valid
    filename. And in some cases we don't know what's going on. I'm
    going to have to hack this. If we see a '/' before the = sign then
    we say we've got an = in a filename, otherwise it's an operator.
    (later)
    That's it, I've had enough. From now on, an =s on a command line
    will be taken to be part of a file name unless its in a defsym,
    and an = in a file will be taken to be an operator.
    */
  int ch;
  keyword_type *k;

  if ((hex_mode && isxdigit(yytext[0]))
      ||
      (isdigit(yytext[0]) && (ldgram_in_expression == true || ldgram_in_script == true))) {
    char *start = yytext;
    unsigned int base = 10;
    if (hex_mode == true) base = 16;
    if (yytext[0] == '0') {
      base = 8;
    }
    ch = input();
    while (isxdigit(ch) 
	   || ch == 'x'
	   || ch == 'X'
	   || ch == 'M'		
	   )
	{
	  if (ch == 'x' || ch == 'X') {
	    base = 16;
	    start = yytext + yyleng;
	  }
	  else {
	    yytext[yyleng++] = ch;
	  }
	  ch = input();
	}
    yytext[yyleng] = 0;
    unput(ch);
    yylval.integer = number(start, base);
    return INT;
  }

  if (ldfile_input_filename) {
    /* We're inside a file */
    if (yytext[0]== '=') {
      RTOKEN('=');
    }
  }

    

  /* Otherwise we only notice special things if were in an
     expression */

  if (ldgram_in_expression) {
    if (yytext[0] != '/' ||  ldgram_in_defsym == false)  {
      switch (yytext[0]) {
      case '/': RTOKEN('/');
      case '=': RTOKEN('=');
      case '+': RTOKEN('+');
      case '-': RTOKEN('-');
      }
    }
  }

  ch = input();
  while (true)
      {
	if (isalpha(ch) || isdigit(ch) || ch == '.'  || ch == '_' ) {
	  yytext[yyleng++] = ch;
	}
	else if (ch == '=' && ldgram_in_script) {
	  /* An = within a script is always taken to be an operator */
	  break;
	}
	else if (ch == '+' || ch == '-' || ch == '/' || ch == '=') {
	  if (ldgram_in_expression) break;
	  yytext[yyleng++] = ch;
	}
	else 
	  break;
	ch = input();
      }

  yytext[yyleng] = 0;
  unput(ch);
  /* Filenames  of just =signs are tokens */
  if (yyleng == 1 && yytext[0] == '=') {
    RTOKEN('=');
  }
  for(k = keywords; k ->name != (char *)NULL; k++) {

    if (strcmp(k->name, yytext)==0) {
      yylval.token = k->value;
      return k->value;
    }
  }
  yylval.name = buystring(yytext);
  return NAME;
}





%%
