%{

/* Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
This was written by steve chamberlain
                    sac@cygnus.com
*/


#include <ansidecl.h>
#include <stdio.h>
#include "bfd.h"
#include "ld.h"
#include "ldgram.h"
#include "ldmisc.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldfile.h"

int ldgram_in_defsym;

int hex_mode;

char *buystring();

unsigned int lineno = 1;
int old;

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) yy_input(buf, &result, max_size)
#undef YY_FATAL_ERROR
#define YY_FATAL_ERROR(s)
#define MAX_INCLUDE_DEPTH 10
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
char *file_name_stack[MAX_INCLUDE_DEPTH];
unsigned int include_stack_ptr = 0;

static YY_BUFFER_STATE yy_create_string_buffer PARAMS ((const char *string,
							int size));
static void yy_input PARAMS ((char *, int *result, int max_size));
static void comment PARAMS ((void));

/* STATES 
	COMMAND	 	on command line
	COMMENT		in a C comment
	EXPRESSION	definiatelyt in an expression
	SCRIPT		definately in a script
	SOMEWHERE	either EXPRESSION or SCRIPT
        MRI             in an MRI script
*/
#define RTOKEN(x)  {  yylval.token = x; return x; }
%}

%a 4000
%o 5000

CMDFILENAMECHAR   [_a-zA-Z0-9\/\.\\_\+\$\:\[\]\\\,\=\&\!\<\>\-\~]
CMDFILENAMECHAR1  [_a-zA-Z0-9\/\.\\_\+\$\:\[\]\\\,\=\&\!\<\>\~]
FILENAMECHAR1	[_a-zA-Z\/\.\\\$\_\~]
SYMBOLCHARN     [_a-zA-Z\/\.\\0-9]
FILENAMECHAR	[_a-zA-Z0-9\/\.\-\_\+\=\$\:\[\]\\\,\~]
FILENAME	{FILENAMECHAR}+
WHITE		[ \t\n]+ 

NOCFILENAMECHAR	[_a-zA-Z0-9\/\.\-\_\+\$\:\[\]\\\~]


%s COMMAND
%s SCRIPT
%s EXPRESSION
%s COMMENT
%s BOTH
%s DEFSYMEXP
%s MRI
%%


<BOTH,SCRIPT,EXPRESSION,COMMAND>"/*"	{ comment(); }


<DEFSYMEXP>"-"                  { RTOKEN('-');}
<DEFSYMEXP>"+"                  { RTOKEN('+');}
<DEFSYMEXP>{FILENAMECHAR1}{SYMBOLCHARN}*   { yylval.name = buystring(yytext); return NAME; }
<DEFSYMEXP>[ \t]                { RTOKEN(DEFSYMEND); }
<DEFSYMEXP>"="                  { RTOKEN('='); }
<COMMAND>"--help"		{ return OPTION_help; }
<COMMAND>"--version"		{ return OPTION_version; }
<COMMAND>"-defsym"{WHITE}*	{ return OPTION_defsym; }
<COMMAND>"-format"		{ return OPTION_format; }
<COMMAND>"-noinhibit-exec"	{ return OPTION_noinhibit_exec; }
<COMMAND>"-noinhibit_exec" 	{ return OPTION_noinhibit_exec; }
<COMMAND>"-oformat"		{ return OPTION_oformat; }
<COMMAND>"-sort-common"		{ return OPTION_sort_common;}
<COMMAND>"-sort_common"		{ return OPTION_sort_common;}
<COMMAND>"-warn-common"		{ return OPTION_warn_common;}
<COMMAND>"-n"			{ return OPTION_n; }
<COMMAND>"-N"			{ return OPTION_N; }
<COMMAND>"-r"			{ return OPTION_r; }
<COMMAND>"-relax"		{ return OPTION_relax; }
<COMMAND>"-i"			{ return OPTION_r; }
<COMMAND>"-Ur"			{ return OPTION_Ur; }
<COMMAND>"-o"			{ return OPTION_o; }
<COMMAND>"-g"			{ return OPTION_g; }
<COMMAND>"-e"			{ return OPTION_e; }
<COMMAND>"-b"			{ return OPTION_b; }
<COMMAND>"-dc"			{ return OPTION_dc; }
<COMMAND>"-dp"			{ return OPTION_dp; }
<COMMAND>"-d"			{ return OPTION_d; }
<COMMAND>"-v"			{ return OPTION_v; }
<COMMAND>"-V"			{ return OPTION_V; }
<COMMAND>"-m"			{ return OPTION_m; }
<COMMAND>"-m"{FILENAME}		{ return OPTION_memul; }
<COMMAND>"-M"			{ return OPTION_M; }
<COMMAND>"-Map"	             	{ return OPTION_Map;}
<COMMAND>"-t"			{ return OPTION_t; }
<COMMAND>"-X"			{ return OPTION_X; }
<COMMAND>"-x"			{ return OPTION_x; }
<COMMAND>"-c"			{ return OPTION_c; }
<COMMAND>"-R"			{ return OPTION_R; }
<COMMAND>"-u"			{ return OPTION_u; }
<COMMAND>"-s"	           	{ return OPTION_s; }
<COMMAND>"-S"	           	{ return OPTION_S; }
<COMMAND>"-Bstat"	 	{ return OPTION_Bstatic; }
<COMMAND>"-B"{FILENAME}	   	{ /* Ignored */ }
<COMMAND>"-l"{FILENAME} 	{
 					yylval.name = buystring(yytext+2);
				  	return OPTION_l; 
				}

<COMMAND>"-L"{FILENAME} 	{ 
					yylval.name = buystring(yytext+2);
					return OPTION_Lfile;
				}
<COMMAND>"-L"			{ return OPTION_L; }
<COMMAND>"-Ttext" 	        {
				 yylval.name = ".text";
				 return OPTION_Texp;
				}
<COMMAND>"-Tdata"  		{
				 yylval.name = ".data";
				 return OPTION_Texp;
			       }
<COMMAND>"-Tbss"		{
				 yylval.name = ".bss";
				 return OPTION_Texp;
			       }
<COMMAND>"-O"([0-9])+		{
					yylval.integer = atoi (yytext + 2);
					return OPTION_Oval;
				}
<COMMAND>"-O"{FILENAME} 	{
				yylval.name = buystring(yytext+2);
				return OPTION_Texp;
				}

<COMMAND>"-T"{FILENAME}  	{
				 yylval.name = buystring(yytext+2);
				 return OPTION_Tfile;
			       }
<COMMAND>"-T"          		{
				 return OPTION_T;
			       }

<COMMAND>"-F"{FILENAME}  	{
				 return OPTION_F;
			       }
<COMMAND>"-F"          		{
				 return OPTION_F;
			       }

<COMMAND>"-y"{FILENAME}         {
					yylval.name = buystring(yytext+2);
					return OPTION_y;
				}
				
<COMMAND>"-A"{FILENAME} 	{
		                	yylval.name = buystring(yytext+2);
				 	return OPTION_Aarch;
			       	}

<COMMAND>"-retain-symbols-file"	{ return OPTION_RETAIN_SYMBOLS_FILE; }

<COMMAND>"-EB"          	{
					return OPTION_EB;
				}
<COMMAND>"-EL"        		{
					return OPTION_EL;
				}
<COMMAND>"-G"			{
					return OPTION_G;
				}
<COMMAND>"-G"([0-9])+		{
					yylval.integer = atoi (yytext + 2);
					return OPTION_Gval;
				}

<COMMAND>"-Qy"			{	return OPTION_Qy; }
<COMMAND>"-dn"			{	return OPTION_dn; }
<COMMAND>"-Y"			{	return OPTION_Y;  }
<COMMAND>"-YP,"{FILENAME}	{
					yylval.name = buystring (yytext+4);
					return OPTION_YP;
				}
<COMMAND>"-non_shared"		{	return OPTION_non_shared; }
<COMMAND>"-call_shared"		{	return OPTION_call_shared; }

<MRI,EXPRESSION>"$"([0-9A-Fa-f])+ {
  				yylval.integer = bfd_scan_vma (yytext+1, 0,16);
				return INT;
			}

<MRI,EXPRESSION>([0-9A-Fa-f])+(H|X|B|O|D) {
				   int base ;
				   switch (yytext[yyleng-1]) {
				    case 'X': 
				    case 'H':
				     base = 16;
				     break;
				    case 'O':
				     base = 8;
				     break;
				    case 'B':
				     base = 2;
				     break;
				    default:
				     base = 10;
				   }
				   yylval.integer = bfd_scan_vma (yytext+1, 0,
								  base);
				   return INT;
				 }
<SCRIPT,DEFSYMEXP,MRI,BOTH,EXPRESSION>"$"?"0x"?([0-9A-Fa-f])+(M|K|m|k)? {
				  yylval.integer = bfd_scan_vma (yytext, 0,
								 hex_mode);
				  if (yytext[yyleng-1]=='M'
					|| yytext[yyleng-1] == 'm') {
				      yylval.integer *= 1024*1024;
				    }	
				  if (yytext[yyleng-1]=='K' 
				|| yytext[yyleng-1]=='k') {
				      yylval.integer *= 1024;
				    }		
				  return INT;
				}
<BOTH,SCRIPT,EXPRESSION>"]"		{ RTOKEN(']');}
<BOTH,SCRIPT,EXPRESSION>"["		{ RTOKEN('[');}
<BOTH,SCRIPT,EXPRESSION>"<<="	{ RTOKEN(LSHIFTEQ);}
<BOTH,SCRIPT,EXPRESSION>">>="	{ RTOKEN(RSHIFTEQ);}
<BOTH,SCRIPT,EXPRESSION>"||"	{ RTOKEN(OROR);}
<BOTH,SCRIPT,EXPRESSION>"=="	{ RTOKEN(EQ);}
<BOTH,SCRIPT,EXPRESSION>"!="	{ RTOKEN(NE);}
<BOTH,SCRIPT,EXPRESSION>">="	{ RTOKEN(GE);}
<BOTH,SCRIPT,EXPRESSION>"<="	{ RTOKEN(LE);}
<BOTH,SCRIPT,EXPRESSION>"<<"	{ RTOKEN(LSHIFT);}
<BOTH,SCRIPT,EXPRESSION>">>"	{ RTOKEN(RSHIFT);}
<BOTH,SCRIPT,EXPRESSION>"+="	{ RTOKEN(PLUSEQ);}
<BOTH,SCRIPT,EXPRESSION>"-="	{ RTOKEN(MINUSEQ);}
<BOTH,SCRIPT,EXPRESSION>"*="	{ RTOKEN(MULTEQ);}
<BOTH,SCRIPT,EXPRESSION>"/="	{ RTOKEN(DIVEQ);}
<BOTH,SCRIPT,EXPRESSION>"&="	{ RTOKEN(ANDEQ);}
<BOTH,SCRIPT,EXPRESSION>"|="	{ RTOKEN(OREQ);}
<BOTH,SCRIPT,EXPRESSION>"&&"	{ RTOKEN(ANDAND);}
<BOTH,SCRIPT,EXPRESSION>">"		{ RTOKEN('>');}
<MRI,BOTH,SCRIPT,EXPRESSION>","		{ RTOKEN(',');}
<BOTH,SCRIPT,EXPRESSION>"&"		{ RTOKEN('&');}
<BOTH,SCRIPT,EXPRESSION>"|"		{ RTOKEN('|');}
<BOTH,SCRIPT,EXPRESSION>"~"		{ RTOKEN('~');}
<BOTH,SCRIPT,EXPRESSION>"!"		{ RTOKEN('!');}
<BOTH,SCRIPT,EXPRESSION>"?"		{ RTOKEN('?');}
<BOTH,SCRIPT,EXPRESSION>"*"		{ RTOKEN('*');}
<BOTH,SCRIPT,EXPRESSION>"+"		{ RTOKEN('+');}
<BOTH,SCRIPT,EXPRESSION>"-"		{ RTOKEN('-');}
<BOTH,SCRIPT,EXPRESSION>"/"		{ RTOKEN('/');}
<BOTH,SCRIPT,EXPRESSION>"%"		{ RTOKEN('%');}
<BOTH,SCRIPT,EXPRESSION>"<"		{ RTOKEN('<');}
<BOTH,SCRIPT,EXPRESSION>">"		{ RTOKEN('>');}
<MRI,BOTH,SCRIPT,EXPRESSION>"="          { RTOKEN('=');}
<BOTH,SCRIPT,EXPRESSION>"}"			{ RTOKEN('}') ; }
<BOTH,SCRIPT,EXPRESSION>"{"			{ RTOKEN('{'); }
<BOTH,SCRIPT,EXPRESSION>")"			{ RTOKEN(')');}
<BOTH,SCRIPT,EXPRESSION>"("			{ RTOKEN('(');}
<BOTH,SCRIPT,EXPRESSION>"]"		{ RTOKEN(']');}
<BOTH,SCRIPT,EXPRESSION>"["		{ RTOKEN('[');}
<BOTH,SCRIPT,EXPRESSION>":"		{ RTOKEN(':'); }
<BOTH,SCRIPT,EXPRESSION>";"		{ RTOKEN(';');}
<BOTH,SCRIPT,EXPRESSION>"-"		{ RTOKEN('-');}
<BOTH,SCRIPT,EXPRESSION>"/"		{ RTOKEN('/');}
<BOTH,SCRIPT>"MEMORY"		{ RTOKEN(MEMORY);}
<BOTH,SCRIPT>"ORIGIN"		{ RTOKEN(ORIGIN);}
<BOTH,SCRIPT>"BLOCK"			{ RTOKEN(BLOCK);}
<BOTH,SCRIPT>"LENGTH"		{ RTOKEN(LENGTH);}
<EXPRESSION,BOTH,SCRIPT>"ALIGN"			{ RTOKEN(ALIGN_K);}
<EXPRESSION,BOTH,SCRIPT>"ADDR"			{ RTOKEN(ADDR);}
<BOTH,SCRIPT>"ENTRY"			{ RTOKEN(ENTRY);}
<EXPRESSION,BOTH,SCRIPT>"NEXT"			{ RTOKEN(NEXT);}
<EXPRESSION,BOTH,SCRIPT>"sizeof_headers"	{ RTOKEN(SIZEOF_HEADERS);}
<EXPRESSION,BOTH,SCRIPT>"SIZEOF_HEADERS"	{ RTOKEN(SIZEOF_HEADERS);}
<BOTH,SCRIPT>"MAP"			{ RTOKEN(MAP);}
<EXPRESSION,BOTH,SCRIPT>"SIZEOF"		{ RTOKEN(SIZEOF);}
<BOTH,SCRIPT>"TARGET"		{ RTOKEN(TARGET_K);}
<BOTH,SCRIPT>"SEARCH_DIR"		{ RTOKEN(SEARCH_DIR);}
<BOTH,SCRIPT>"OUTPUT"		{ RTOKEN(OUTPUT);}
<BOTH,SCRIPT>"INPUT"			{ RTOKEN(INPUT);}
<EXPRESSION,BOTH,SCRIPT>"DEFINED"		{ RTOKEN(DEFINED);}
<BOTH,SCRIPT>"CREATE_OBJECT_SYMBOLS"	{ RTOKEN(CREATE_OBJECT_SYMBOLS);}
<BOTH,SCRIPT>"CONSTRUCTORS"		{ RTOKEN( CONSTRUCTORS);}
<BOTH,SCRIPT>"FORCE_COMMON_ALLOCATION" { RTOKEN(FORCE_COMMON_ALLOCATION);}
<BOTH,SCRIPT>"SECTIONS"		{ RTOKEN(SECTIONS);}
<BOTH,SCRIPT>"FILL"			{ RTOKEN(FILL);}
<BOTH,SCRIPT>"STARTUP"		{ RTOKEN(STARTUP);}
<BOTH,SCRIPT>"OUTPUT_FORMAT"		{ RTOKEN(OUTPUT_FORMAT);}
<BOTH,SCRIPT>"OUTPUT_ARCH"		{ RTOKEN( OUTPUT_ARCH);}
<BOTH,SCRIPT>"HLL"			{ RTOKEN(HLL);}
<BOTH,SCRIPT>"SYSLIB"		{ RTOKEN(SYSLIB);}
<BOTH,SCRIPT>"FLOAT"			{ RTOKEN(FLOAT);}
<BOTH,SCRIPT>"LONG"			{ RTOKEN( LONG);}
<BOTH,SCRIPT>"SHORT"			{ RTOKEN( SHORT);}
<BOTH,SCRIPT>"BYTE"			{ RTOKEN( BYTE);}
<BOTH,SCRIPT>"NOFLOAT"		{ RTOKEN(NOFLOAT);}
<EXPRESSION,BOTH,SCRIPT>"NOLOAD"		{ RTOKEN(NOLOAD);}
<BOTH,SCRIPT>"DSECT"			{ RTOKEN(DSECT);}
<BOTH,SCRIPT>"COPY"			{ RTOKEN(COPY);}
<BOTH,SCRIPT>"INFO"			{ RTOKEN(INFO);}
<BOTH,SCRIPT>"OVERLAY"		{ RTOKEN(OVERLAY);}
<BOTH,SCRIPT>"o"			{ RTOKEN(ORIGIN);}
<BOTH,SCRIPT>"org"			{ RTOKEN(ORIGIN);}
<BOTH,SCRIPT>"l"			{ RTOKEN( LENGTH);}
<BOTH,SCRIPT>"len"			{ RTOKEN( LENGTH);}
<BOTH,SCRIPT>"INCLUDE"			{ RTOKEN(INCLUDE);}
<EXPRESSION,BOTH,SCRIPT>"AT"			{ RTOKEN(AT);}
<MRI>"\n"	                { ++ lineno;  RTOKEN(NEWLINE); }
<MRI>"*".*			{ /* Mri comment line */ }
<MRI>"END"                      { RTOKEN(ENDWORD); }
<MRI>"ALIGNMOD"		{ RTOKEN(ALIGNMOD);}
<MRI>"ALIGN"		{ RTOKEN(ALIGN_K);}

<MRI>"CHIP"                     { RTOKEN(CHIP); }
<MRI>"BASE"                     { RTOKEN(BASE); }
<MRI>"ALIAS"                     { RTOKEN(ALIAS); }
<MRI>"TRUNCATE"                     { RTOKEN(TRUNCATE); }
<MRI>"LOAD"                     { RTOKEN(LOAD); }
<MRI>"PUBLIC"                   { RTOKEN(PUBLIC); }
<MRI>"ORDER"                    { RTOKEN(ORDER); }
<MRI>"NAME"                     { RTOKEN(NAMEWORD); }
<MRI>"FORMAT"                   { RTOKEN(FORMAT); }
<MRI>"LIST".*                   { RTOKEN(LIST); /* LIST and ignore to end of line */ }
<MRI>"SECT"			{ RTOKEN(SECT); }
<EXPRESSION,BOTH,SCRIPT,MRI>"ABSOLUTE"			{ RTOKEN(ABSOLUTE); }
<MRI>"end"                      { RTOKEN(ENDWORD); }
<MRI>"chip"                     { RTOKEN(CHIP); }
<MRI>"load"                     { RTOKEN(LOAD); }
<MRI>"order"                    { RTOKEN(ORDER); }
<MRI>"name"                     { RTOKEN(NAMEWORD); }
<MRI>"format"                   { RTOKEN(FORMAT); }
<MRI>"list".*                   { RTOKEN(LIST); /* LIST and ignore to end of line */ }
<MRI>"sect"			{ RTOKEN(SECT); }
<EXPRESSION,BOTH,SCRIPT,MRI>"absolute"			{ RTOKEN(ABSOLUTE); }

<COMMAND>{CMDFILENAMECHAR1}{CMDFILENAMECHAR}*	{
				 yylval.name = buystring(yytext); 
				  return NAME;
				}


<MRI>{FILENAMECHAR1}{NOCFILENAMECHAR}*	{
/* Filename without commas, needed to parse mri stuff */
				 yylval.name = buystring(yytext); 
				  return NAME;
				}


<BOTH,EXPRESSION>{FILENAMECHAR1}{FILENAMECHAR}*	{
				 yylval.name = buystring(yytext); 
				  return NAME;
				}
<SCRIPT,COMMAND>{FILENAMECHAR}* { yylval.name = buystring(yytext); 
				  return NAME;
				}

<EXPRESSION,BOTH,COMMAND,SCRIPT>"\""[^\"]*"\"" {
					/* No matter the state, quotes
					   give what's inside */
					yylval.name = buystring(yytext+1);
					yylval.name[yyleng-2] = 0;
					return NAME;
				}
<BOTH,SCRIPT,EXPRESSION>"\n"		{ lineno++;}
<MRI,COMMAND,BOTH,SCRIPT,EXPRESSION>[ \t]

<<EOF>> {
  include_stack_ptr--;
    
  if (include_stack_ptr == 0) 
  {
    yyterminate();
  }
  else 
  {
    yy_switch_to_buffer(include_stack[include_stack_ptr]);

  }
  if (include_stack_ptr==1) 
  {
    BEGIN(COMMAND);
  }
  else 
  {
    BEGIN(SCRIPT);
    ldfile_input_filename = file_name_stack[include_stack_ptr-1];
  }

  return END;
}
    
%%

void
lex_push_file (file, name)
     FILE *file;
     char *name;
{
  if (include_stack_ptr >= MAX_INCLUDE_DEPTH) 
  {
    einfo("%F:includes nested too deeply\n");
  }
  file_name_stack[include_stack_ptr] = name;
  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;

  include_stack_ptr++;
  yyin = file;
  yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));

  BEGIN(SCRIPT);
}

static YY_BUFFER_STATE 
yy_create_string_buffer (string,  size)
     CONST char *string;
     int size;
{
  YY_BUFFER_STATE b;

  b = (YY_BUFFER_STATE) malloc( sizeof( struct yy_buffer_state ) );
  b->yy_input_file = 0;
  
  if ( ! b )
   YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );

  b->yy_buf_size = size;

  /* yy_ch_buf has to be 2 characters longer than the size given because
   * we need to put in 2 end-of-buffer characters.
   */
  b->yy_ch_buf = (YY_CHAR *) malloc( (unsigned) (b->yy_buf_size + 3) );

  if ( ! b->yy_ch_buf )
   YY_FATAL_ERROR( "out of dynamic memory in yy_create_buffer()" );


  b->yy_ch_buf[0] = '\n';
  strcpy(b->yy_ch_buf+1,  string);
  b->yy_ch_buf[size+1] = YY_END_OF_BUFFER_CHAR;
  b->yy_ch_buf[size+2] = YY_END_OF_BUFFER_CHAR;
    b->yy_n_chars = size+1;
  b->yy_buf_pos = &b->yy_ch_buf[1];

  b->yy_eof_status = EOF_NOT_SEEN;

  return ( b );
}



void
lex_redirect (string)
     CONST  char *string;
{
  YY_BUFFER_STATE tmp;

  int len = strlen(string);
  yy_init = 0  ;  
  if (include_stack_ptr >= MAX_INCLUDE_DEPTH) 
  {
    einfo("%F: macros nested too deeply\n");
  }
  file_name_stack[include_stack_ptr] = "redirect";
  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
  include_stack_ptr++;
  tmp =   yy_create_string_buffer(string, len);
  
  yy_switch_to_buffer(tmp);
  BEGIN(COMMAND);
  yyout = stdout;
}

int state_stack[20];
int *state_stack_p = state_stack;

void
ldlex_script ()
{
  *(state_stack_p)++ = yy_start;

  BEGIN(SCRIPT);
}


void
ldlex_mri_script ()
{
  *(state_stack_p)++ = yy_start;
  BEGIN(MRI);
}

void
ldlex_defsym ()
{
  *(state_stack_p)++ = yy_start;
  BEGIN(DEFSYMEXP);
}
	   
void
ldlex_expression ()
{
  *(state_stack_p)++ = yy_start;
  BEGIN(EXPRESSION);

}
void
ldlex_both ()
{
  *(state_stack_p)++ = yy_start;
  BEGIN(BOTH);
}
void
ldlex_command ()
{
  *(state_stack_p)++ = yy_start;
  BEGIN(COMMAND);
}

void
ldlex_popstate ()
{
  yy_start = *(--state_stack_p);
}

static void
yy_input(buf, result, max_size)
char *buf;
int *result;
int max_size;
{
  *result = 0; 
  if (yy_current_buffer->yy_input_file)
  {
    if (yyin)
     if ( (*result = read( fileno(yyin), (char *) buf, max_size )) < 0 ) 
      YY_FATAL_ERROR( "read() in flex scanner failed" );
  }
}

static void
comment ()
{
  int c;
  while (1)
  {
    c = input();
    while (c !='*' && c != EOF) 
    {
      if (c == '\n') lineno++;
      c = input();
    }


    if (c == '*')
    {
      c = input();
      while ( c == '*')
       c = input();
      if ( c == '/' )
       break;			/* found the end */
    }

    if ( c == EOF )
    {
      einfo( "%F%P :EOF in comment\n");
      break;
    }
  }
}
