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
 * $Log$
 * Revision 1.1  1991/03/21 21:28:50  gumby
 * Initial revision
 *
 * Revision 1.3  1991/03/16  22:27:24  rich
 * fish
 *
 * Revision 1.2  1991/03/15  18:45:55  rich
 * foo
 *
 * Revision 1.1  1991/03/13  00:48:27  chrisb
 * Initial revision
 *
 * Revision 1.6  1991/03/10  09:31:32  rich
 *  Modified Files:
 *  	Makefile config.h ld-emul.c ld-emul.h ld-gld.c ld-gld960.c
 *  	ld-lnk960.c ld.h lddigest.c ldexp.c ldexp.h ldfile.c ldfile.h
 *  	ldgram.y ldinfo.h ldlang.c ldlang.h ldlex.h ldlex.l ldmain.c
 *  	ldmain.h ldmisc.c ldmisc.h ldsym.c ldsym.h ldversion.c
 *  	ldversion.h ldwarn.h ldwrite.c ldwrite.h y.tab.h
 *
 * As of this round of changes, ld now builds on all hosts of (Intel960)
 * interest and copy passes my copy test on big endian hosts again.
 *
 * Revision 1.5  1991/03/09  03:25:49  sac
 * Can now parse the -Ur flag
 *
 * Revision 1.4  1991/03/06  02:26:04  sac
 * Added support for constructor sections.
 * Remove parsing ambiguity.
 * Lint
 *
 * Revision 1.3  1991/02/22  17:15:14  sac
 * Added RCS keywords and copyrights
 *
*/



/*SUPPRESS 529*/
/*SUPPRESS 26*/
/*SUPPRESS 29*/
#define LEXDEBUG
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
extern boolean ldgram_want_filename;
extern boolean ldgram_mustbe_filename;
extern boolean ldgram_mustbe_symbolname;
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
"NEXT",NEXT,
"MAP",MAP,
"SIZEOF",SIZEOF,
"TARGET",TARGET_K,
"SEARCH_DIR",SEARCH_DIR,
"OUTPUT",OUTPUT,
"INPUT",INPUT,
"DEFINED",DEFINED,
"CREATE_OBJECT_SYMBOLS",CREATE_OBJECT_SYMBOLS,
"SECTIONS",SECTIONS,
"FILL",FILL,
"STARTUP",STARTUP,
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

int
lex_input()
{
  /*
     When we know that the next token must be a filename we force the 
     input routine to return a '#' character, which will cause the special
     filname regexp to match the following chars even if they don't look
     much like a filename to any sane person.
     */
  if (ldgram_mustbe_filename) {
    ldgram_mustbe_filename = false;
    return '#';
  }

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
      thischar = lex_input();

    }
  }
  else if (command_line && *command_line)  {
    thischar = *(command_line++);
  }
  else thischar = 0;
  if(thischar == '\t') thischar = ' ';
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
FILENAMECHAR	[a-zA-Z0-9\/\.\-\_\+]
FILENAME	{FILENAMECHAR}+


WHITE		[ \t]+

%%
"\n"		{ lineno++; }


"\ -defsym" { return OPTION_defsym; }
"\ -noinhibit_exec" { return OPTION_noinhibit_exec; }
"\ -format" { return OPTION_format; }
"\ -n"		{ return OPTION_n; }
"\ -r"		{ return OPTION_r; }
"\ -Ur"		{ return OPTION_Ur; }
"\ -o"		{ return OPTION_o; }
"\ -g"		{ return OPTION_g; }
"\ -e"		{ return OPTION_e; }
"\ -b"		{ return OPTION_b; }
"\ -dc"		{ return OPTION_dc; }
"\ -dp"		{ return OPTION_dp; }
"\ -d"		{ return OPTION_d; }
"\ -v"		{ return OPTION_v; }
"\ -M"		{ return OPTION_M; }
"\ -t"		{ return OPTION_t; }
"\ -X"		{ return OPTION_X; }
"\ -x"		{ return OPTION_x; }
"\ -c"		{ return OPTION_c; }
"\ -s"            { return OPTION_s; }
"\ -S"            { return OPTION_S; }
"\ -l"{FILENAME} {
	 	yylval.name = buystring(yytext+3);
		return OPTION_l; 
	}

"\ -L"{FILENAME} 	{ 
		yylval.name = buystring(yytext+3);
	 	return OPTION_L; 
	 }
"\ -Ttext"  {
		 yylval.name = ".text";
		 return OPTION_Texp;
	       }
"\ -Tdata"  {
		 yylval.name = ".data";
		 return OPTION_Texp;
	       }
"\ -Tbss"  {
		 yylval.name = ".bss";
		 return OPTION_Texp;
	       }

"\ -T"{FILENAME}  {
		 yylval.name = buystring(yytext+3);
		 return OPTION_Tfile;
	       }
"\ -T"          {
		 return OPTION_T;
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
"+"		{ RTOKEN('+');}
">"		{ RTOKEN('>');}
"}"		{ RTOKEN('}') ; }
"{"		{ RTOKEN('{'); }
")"		{ RTOKEN(')');}
"("		{ RTOKEN('(');}
"]"		{ RTOKEN(']');}
"["		{ RTOKEN('[');}
":"		{ RTOKEN(':'); }
";"		{ RTOKEN(';');}
"-"		{ RTOKEN('-');}
"="		{ RTOKEN('=');}


"/*"		{ 
  while (1) {
    int ch;
    ch = input();
    while (ch != '*') {
      if (ch == '\n') {lineno++; }
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
[0][0-7KM]* {

  yylval.integer = number(yytext+1, 8);
 return INT;
}

[0-9]+[KM]? {
  if (hex_mode == true) {
    yylval.integer = number(yytext, 16);
  }
  else {
    yylval.integer = number(yytext, 10);
  }
  return INT;
}

0[Xx][0-9a-fA-FKM]+ {
 
  yylval.integer = number(yytext+2,16);	
  return INT;
}

"\#"{WHITE}*{FILENAMECHAR}+ {
  char *p = yytext+1;
  while(*p ==' ' || *p == '\t') p++;
  yylval.name = buystring(p);
  return NAME;
}


{FILENAMECHAR} 	{

int ch;
  keyword_type *k;
  if (yytext[0] == '/' && ldgram_mustbe_symbolname) 
    { RTOKEN('/');}
   ch = input();
  while (true) {
    if (isalpha(ch) || isdigit(ch) || ch == '.'  || ch == '_') {
      yytext[yyleng++] = ch;
    }
    else if (ch == '-' && ldgram_want_filename == true) {
      yytext[yyleng++] = ch;
    }
    else if (ch == '+' && ldgram_want_filename == true) {
      yytext[yyleng++] = ch;
    }

    else if (ch == '/' && ldgram_want_filename == true) {
      yytext[yyleng++] = ch;
    }

    else break;
    ch = input();
  }

  yytext[yyleng] = 0;
  unput(ch);

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
