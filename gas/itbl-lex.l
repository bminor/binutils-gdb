%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "itbl-parse.h"

#ifdef DEBUG
#define DBG(x) printf x
#define MDBG(x) printf x
#else
#define DBG(x)
#define MDBG(x)
#endif

int insntbl_line = 1;
%}

ALNUM	[A-Za-z0-9_]
DIGIT	[0-9]
ALPHA	[A-Za-z_]
HEX	[0-9A-Fa-f]

%%

"creg"|"CREG"	{
		return CREG;
		}
"dreg"|"DREG"	{
		return DREG;
		}
"greg"|"GREG"	{
		return GREG;
		}
"immed"|"IMMED"	{
		return IMMED;
		}
"addr"|"ADDR"	{
		return ADDR;
		}
"insn"|"INSN"	{
		return INSN;
		}
"p"{DIGIT} {
		yytext[yyleng]=0;
		yylval.processor = strtoul(yytext+1,0,0);
		return PNUM;
		}
{DIGIT}+	{
		yytext[yyleng]=0;
		yylval.num = strtoul(yytext,0,0);
		return NUM;
		}
"0x"{HEX}+	{
		yytext[yyleng]=0;
		yylval.num = strtoul(yytext,0,0);
		return NUM;
		}
{ALPHA}{ALNUM}*	{
		yytext[yyleng]=0;
		yylval.str = strdup(yytext);
		return ID;
		}
";"|"#"	{
		int c;
		while ((c = input()) != EOF) {
		    if (c == '\n') 
			{
				unput(c);
				break;
			}
		}
		}
"\n"	{ 
		insntbl_line++; 
		MDBG(("in lex, NL=%d (x%x)\n",NL,NL));
		return NL; 
		}
" "|"\t"	{ }
.		{
		MDBG(("char=%x,%d\n",yytext[0],yytext[0]));
		return yytext[0];
		}
%%

int yywrap() { return 1; }
