
/*  A Bison parser, made from expread.y  */

#define	INT	258
#define	CHAR	259
#define	FLOAT	260
#define	NAME	261
#define	TYPENAME	262
#define	STRING	263
#define	STRUCT	264
#define	UNION	265
#define	ENUM	266
#define	SIZEOF	267
#define	UNSIGNED	268
#define	COLONCOLON	269
#define	LAST	270
#define	REGNAME	271
#define	VARIABLE	272
#define	ASSIGN_MODIFY	273
#define	THIS	274
#define	ABOVE_COMMA	275
#define	OR	276
#define	AND	277
#define	EQUAL	278
#define	NOTEQUAL	279
#define	LEQ	280
#define	GEQ	281
#define	LSH	282
#define	RSH	283
#define	UNARY	284
#define	INCREMENT	285
#define	DECREMENT	286
#define	ARROW	287

#line 30 "expread.y"

#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"
#include "expression.h"

#include <stdio.h>

static struct expression *expout;
static int expout_size;
static int expout_ptr;

static int yylex ();
static yyerror ();
static void write_exp_elt ();
static void write_exp_string ();
static void start_arglist ();
static int end_arglist ();
static void free_funcalls ();
static char *copy_name ();

/* If this is nonzero, this block is used as the lexical context
   for symbol names.  */

static struct block *expression_context_block;

/* Number of arguments seen so far in innermost function call.  */
static int arglist_len;

/* Data structure for saving values of arglist_len
   for function calls whose arguments contain other function calls.  */

struct funcall
  {
    struct funcall *next;
    int arglist_len;
  };

struct funcall *funcall_chain;

/* This kind of datum is used to represent the name
   of a symbol token.  */

struct stoken
  {
    char *ptr;
    int length;
  };

#line 85 "expread.y"
typedef union
  {
    long lval;
    double dval;
    struct symbol *sym;
    struct type *tval;
    struct stoken sval;
    int voidval;
    struct block *bval;
    enum exp_opcode opcode;
    struct internalvar *ivar;

    struct type **tvec;
    int *ivec;
  } YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
 {
      int timestamp;
      int first_line;
      int first_column;
 int last_line;
      int last_column;
      char *text;
   }
 yyltype;

#define YYLTYPE yyltype
#endif

#define	YYACCEPT	return(0)
#define	YYABORT	return(1)
#define	YYERROR	return(1)
#include <stdio.h>

#ifndef __STDC__
#define const
#endif



#define	YYFINAL		152
#define	YYFLAG		-32768
#define	YYNTBASE	57

#define YYTRANSLATE(x) ((unsigned)(x) <= 287 ? yytranslate[x] : 68)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    48,     2,     2,     2,    40,    27,     2,    51,
    52,    38,    36,    20,    37,    46,    39,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    56,     2,    30,
    22,    31,    55,    41,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    47,     2,    50,    26,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    53,    25,    54,    49,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    21,    23,    24,    28,    29,    32,
    33,    34,    35,    42,    43,    44,    45
};

static const short yyrline[] = {     0,
   152,   156,   157,   162,   165,   168,   172,   176,   180,   184,
   188,   192,   196,   200,   206,   210,   216,   220,   224,   228,
   234,   237,   241,   245,   251,   257,   263,   267,   271,   275,
   279,   283,   287,   291,   295,   299,   303,   307,   311,   315,
   319,   323,   327,   331,   335,   339,   343,   347,   353,   360,
   367,   374,   377,   383,   389,   395,   402,   409,   416,   437,
   448,   461,   474,   510,   575,   576,   578,   580,   582,   584,
   586,   592,   595,   598,   601,   604,   609,   614,   621,   622
};

static const char * const yytname[] = {     0,
"error","$illegal.","INT","CHAR","FLOAT","NAME","TYPENAME","STRING","STRUCT","UNION",
"ENUM","SIZEOF","UNSIGNED","COLONCOLON","LAST","REGNAME","VARIABLE","ASSIGN_MODIFY","THIS","','",
"ABOVE_COMMA","'='","OR","AND","'|'","'^'","'&'","EQUAL","NOTEQUAL","'<'",
"'>'","LEQ","GEQ","LSH","RSH","'+'","'-'","'*'","'/'","'%'",
"'@'","UNARY","INCREMENT","DECREMENT","ARROW","'.'","'['","'!'","'~'","']'",
"'('","')'","'{'","'}'","'?'","':'","start"
};

static const short yyr1[] = {     0,
    57,    58,    58,    59,    59,    59,    59,    59,    59,    59,
    59,    59,    59,    59,    59,    59,    59,    59,    60,    59,
    61,    61,    61,    59,    59,    59,    59,    59,    59,    59,
    59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
    59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
    59,    59,    59,    59,    59,    59,    59,    59,    62,    62,
    63,    63,    63,    63,    64,    64,    64,    64,    64,    64,
    64,    65,    65,    65,    65,    65,    66,    66,    67,    67
};

static const short yyr2[] = {     0,
     1,     1,     3,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     3,     4,     3,     4,     4,     0,     5,
     0,     1,     3,     4,     4,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     5,     3,     3,     1,     1,
     1,     1,     1,     1,     1,     4,     1,     1,     1,     3,
     3,     3,     2,     1,     1,     2,     2,     3,     6,     8,
     9,     1,     2,     2,     2,     2,     1,     3,     1,     1
};

static const short yydefact[] = {     0,
    49,    50,    51,    64,    72,    57,     0,     0,     0,     0,
     0,     0,    53,    54,    55,    58,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     1,     2,     0,    52,     0,
    59,    79,    80,    73,    74,    75,     0,    13,    76,    63,
     5,     6,     4,     9,    10,     7,     8,     0,     0,    65,
    72,     0,    65,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    11,    12,     0,     0,     0,
    19,     0,     0,     0,     0,    26,    67,    66,     0,     0,
     0,     0,     0,     3,    48,    47,    45,    44,    43,    42,
    41,    35,    36,    39,    40,    37,    38,    33,    34,    31,
    32,    28,    29,    30,    27,     0,    14,     0,    16,     0,
    21,     0,    61,    62,    56,     0,    25,    68,    24,    15,
    17,    18,    22,     0,     0,     0,     0,    20,    46,     0,
    23,    69,     0,    70,    77,     0,     0,    71,    78,     0,
     0,     0
};

static const short yydefgoto[] = {   150,
    48,    27,   121,   134,    28,    29,    49,    30,   146,    31
};

static const short yypact[] = {   125,
-32768,-32768,-32768,     1,-32768,-32768,    24,    24,    24,   176,
    24,    24,-32768,-32768,-32768,-32768,   125,   125,   125,   125,
   125,   125,   125,   125,   515,     0,   243,    14,-32768,    18,
-32768,-32768,-32768,-32768,-32768,-32768,   125,   472,-32768,-32768,
   472,   472,   472,   472,   472,   472,   472,    21,    52,    19,
-32768,    58,    20,   125,   125,   125,   125,   125,   125,   125,
   125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
   125,   125,   125,   125,   125,-32768,-32768,    -1,    29,   125,
-32768,   125,    24,    24,   119,-32768,-32768,-32768,   515,   125,
    32,   125,   -14,   243,   243,   243,   275,   306,   336,   365,
   393,   419,   419,   441,   441,   441,   441,   161,   161,   459,
   459,   104,   104,   104,   472,   125,-32768,   125,-32768,    49,
   125,   208,    26,-32768,   125,    57,   472,-32768,   472,   104,
   104,-32768,   243,    22,   125,    38,   125,-32768,   243,    25,
   243,    27,    16,-32768,    54,    23,   515,-32768,    54,    84,
    86,-32768
};

static const short yypgoto[] = {-32768,
     3,   -10,-32768,-32768,-32768,-32768,   -21,   -23,-32768,    10
};


#define	YYLAST		528


static const short yytable[] = {    38,
    50,    53,    26,    52,    32,    33,    41,    42,    43,    44,
    45,    46,    47,    50,   -79,    85,    34,    35,    36,    54,
    39,    40,    51,   128,     7,     8,     9,    83,    11,    32,
    33,    84,    91,    93,    32,    33,   116,    32,    33,   -60,
    54,   137,   147,    94,    95,    96,    97,    98,    99,   100,
   101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
   111,   112,   113,   114,   115,   126,   118,   144,    54,   128,
   136,   122,    86,   138,   148,   140,   142,   143,    87,   127,
    87,   129,   120,   151,    87,   152,     0,   117,   119,    88,
     0,    88,   123,   124,     0,    88,     0,     0,   132,     0,
   124,     0,    89,    90,    89,   130,     0,   131,    89,     0,
   133,    92,     0,     0,   127,     0,     0,     0,     0,    53,
     0,   145,     0,    53,   139,   149,   141,     1,     2,     3,
     4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
    14,    15,     0,    16,    75,    87,    76,    77,    78,    79,
    80,    17,     0,     0,    81,     0,    88,     0,    82,     0,
     0,    18,    19,     0,     0,     0,     0,    20,    21,    89,
   125,     0,    22,    23,     0,    24,     0,    25,     1,     2,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,     0,    16,     0,    70,    71,    72,    73,
    74,    75,    17,    76,    77,    78,    79,    80,     0,     0,
     0,    81,    18,    19,     0,    82,     0,     0,    20,    21,
     0,     0,     0,    22,    23,    55,    37,     0,    25,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
    67,    68,    69,    70,    71,    72,    73,    74,    75,     0,
    76,    77,    78,    79,    80,     0,     0,     0,    81,     0,
    55,     0,    82,   135,    56,    57,    58,    59,    60,    61,
    62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,    76,    77,    78,    79,    80,
     0,     0,     0,    81,     0,     0,     0,    82,    58,    59,
    60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,     0,    76,    77,    78,
    79,    80,     0,     0,     0,    81,     0,     0,     0,    82,
    59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
    69,    70,    71,    72,    73,    74,    75,     0,    76,    77,
    78,    79,    80,     0,     0,     0,    81,     0,     0,     0,
    82,    60,    61,    62,    63,    64,    65,    66,    67,    68,
    69,    70,    71,    72,    73,    74,    75,     0,    76,    77,
    78,    79,    80,     0,     0,     0,    81,     0,     0,     0,
    82,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,     0,    76,    77,    78,
    79,    80,     0,     0,     0,    81,     0,     0,     0,    82,
    62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,     0,    76,    77,    78,    79,    80,
     0,     0,     0,    81,     0,     0,     0,    82,    64,    65,
    66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
     0,    76,    77,    78,    79,    80,     0,     0,     0,    81,
     0,     0,     0,    82,    68,    69,    70,    71,    72,    73,
    74,    75,     0,    76,    77,    78,    79,    80,     0,     0,
     0,    81,     0,     0,     0,    82,    72,    73,    74,    75,
     0,    76,    77,    78,    79,    80,     0,     0,     0,    81,
     0,     0,     0,    82,    76,    77,    78,    79,    80,     0,
     0,    51,    81,     7,     8,     9,    82,    11
};

static const short yycheck[] = {    10,
    24,    25,     0,    25,     6,     7,    17,    18,    19,    20,
    21,    22,    23,    37,    14,    37,     7,     8,     9,    20,
    11,    12,     7,    38,     9,    10,    11,    14,    13,     6,
     7,    14,    14,    14,     6,     7,    38,     6,     7,    14,
    20,    20,    20,    54,    55,    56,    57,    58,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    89,    38,    52,    20,    38,
    14,    82,    52,    52,    52,    38,    52,    51,    27,    90,
    27,    92,    80,     0,    27,     0,    -1,    78,    79,    38,
    -1,    38,    83,    84,    -1,    38,    -1,    -1,    50,    -1,
    91,    -1,    51,    52,    51,   116,    -1,   118,    51,    -1,
   121,    54,    -1,    -1,   125,    -1,    -1,    -1,    -1,   143,
    -1,   143,    -1,   147,   135,   147,   137,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    -1,    19,    41,    27,    43,    44,    45,    46,
    47,    27,    -1,    -1,    51,    -1,    38,    -1,    55,    -1,
    -1,    37,    38,    -1,    -1,    -1,    -1,    43,    44,    51,
    52,    -1,    48,    49,    -1,    51,    -1,    53,     3,     4,
     5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
    15,    16,    17,    -1,    19,    -1,    36,    37,    38,    39,
    40,    41,    27,    43,    44,    45,    46,    47,    -1,    -1,
    -1,    51,    37,    38,    -1,    55,    -1,    -1,    43,    44,
    -1,    -1,    -1,    48,    49,    18,    51,    -1,    53,    22,
    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
    43,    44,    45,    46,    47,    -1,    -1,    -1,    51,    -1,
    18,    -1,    55,    56,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    -1,    43,    44,    45,    46,    47,
    -1,    -1,    -1,    51,    -1,    -1,    -1,    55,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    -1,    43,    44,    45,
    46,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,    55,
    25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    -1,    43,    44,
    45,    46,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,
    55,    26,    27,    28,    29,    30,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    -1,    43,    44,
    45,    46,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,
    55,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    -1,    43,    44,    45,
    46,    47,    -1,    -1,    -1,    51,    -1,    -1,    -1,    55,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    -1,    43,    44,    45,    46,    47,
    -1,    -1,    -1,    51,    -1,    -1,    -1,    55,    30,    31,
    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    -1,    43,    44,    45,    46,    47,    -1,    -1,    -1,    51,
    -1,    -1,    -1,    55,    34,    35,    36,    37,    38,    39,
    40,    41,    -1,    43,    44,    45,    46,    47,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    38,    39,    40,    41,
    -1,    43,    44,    45,    46,    47,    -1,    -1,    -1,    51,
    -1,    -1,    -1,    55,    43,    44,    45,    46,    47,    -1,
    -1,     7,    51,     9,    10,    11,    55,    13
};
#define YYPURE 1

#line 2 "bison.simple"

/* Skeleton output parser for bison,
   copyright (C) 1984 Bob Corbett and Richard Stallman

		       NO WARRANTY

  BECAUSE THIS PROGRAM IS LICENSED FREE OF CHARGE, WE PROVIDE ABSOLUTELY
NO WARRANTY, TO THE EXTENT PERMITTED BY APPLICABLE STATE LAW.  EXCEPT
WHEN OTHERWISE STATED IN WRITING, FREE SOFTWARE FOUNDATION, INC,
RICHARD M. STALLMAN AND/OR OTHER PARTIES PROVIDE THIS PROGRAM "AS IS"
WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY
AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE
DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
CORRECTION.

 IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW WILL RICHARD M.
STALLMAN, THE FREE SOFTWARE FOUNDATION, INC., AND/OR ANY OTHER PARTY
WHO MAY MODIFY AND REDISTRIBUTE THIS PROGRAM AS PERMITTED BELOW, BE
LIABLE TO YOU FOR DAMAGES, INCLUDING ANY LOST PROFITS, LOST MONIES, OR
OTHER SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
USE OR INABILITY TO USE (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR
DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY THIRD PARTIES OR
A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS) THIS
PROGRAM, EVEN IF YOU HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY.

		GENERAL PUBLIC LICENSE TO COPY

  1. You may copy and distribute verbatim copies of this source file
as you receive it, in any medium, provided that you conspicuously and
appropriately publish on each copy a valid copyright notice "Copyright
(C) 1985 Free Software Foundation, Inc."; and include following the
copyright notice a verbatim copy of the above disclaimer of warranty
and of this License.  You may charge a distribution fee for the
physical act of transferring a copy.

  2. You may modify your copy or copies of this source file or
any portion of it, and copy and distribute such modifications under
the terms of Paragraph 1 above, provided that you also do the following:

    a) cause the modified files to carry prominent notices stating
    that you changed the files and the date of any change; and

    b) cause the whole of any work that you distribute or publish,
    that in whole or in part contains or is a derivative of this
    program or any part thereof, to be licensed at no charge to all
    third parties on terms identical to those contained in this
    License Agreement (except that you may choose to grant more
    extensive warranty protection to third parties, at your option).

    c) You may charge a distribution fee for the physical act of
    transferring a copy, and you may at your option offer warranty
    protection in exchange for a fee.

  3. You may copy and distribute this program or any portion of it in
compiled, executable or object code form under the terms of Paragraphs
1 and 2 above provided that you do the following:

    a) cause each such copy to be accompanied by the
    corresponding machine-readable source code, which must
    be distributed under the terms of Paragraphs 1 and 2 above; or,

    b) cause each such copy to be accompanied by a
    written offer, with no time limit, to give any third party
    free (except for a nominal shipping charge) a machine readable
    copy of the corresponding source code, to be distributed
    under the terms of Paragraphs 1 and 2 above; or,

    c) in the case of a recipient of this program in compiled, executable
    or object code form (without the corresponding source code) you
    shall cause copies you distribute to be accompanied by a copy
    of the written offer of source code which you received along
    with the copy you received.

  4. You may not copy, sublicense, distribute or transfer this program
except as expressly provided under this License Agreement.  Any attempt
otherwise to copy, sublicense, distribute or transfer this program is void and
your rights to use the program under this License agreement shall be
automatically terminated.  However, parties who have received computer
software programs from you with this License Agreement will not have
their licenses terminated so long as such parties remain in full compliance.

  5. If you wish to incorporate parts of this program into other free
programs whose distribution conditions are different, write to the Free
Software Foundation at 675 Mass Ave, Cambridge, MA 02139.  We have not yet
worked out a simple rule that can be stated here, but we will often permit
this.  We will be guided by the two goals of preserving the free status of
all derivatives of our free software and of promoting the sharing and reuse of
software.


In other words, you are welcome to use, share and improve this program.
You are forbidden to forbid anyone else to use, share and improve
what you give them.   Help stamp out software-hoarding!  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYFAIL		goto yyerrlab;

#define YYTERROR	1

#ifndef YYIMPURE
#define YYLEX		yylex()
#endif

#ifndef YYPURE
#define YYLEX		yylex(&yylval, &yylloc)
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYIMPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/

int yydebug = 0;		/*  nonzero means print parse trace	*/

#endif  /* YYIMPURE */


/*  YYMAXDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYMAXDEPTH
#define YYMAXDEPTH 200
#endif

/*  YYMAXLIMIT is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#ifndef YYMAXLIMIT
#define YYMAXLIMIT 10000
#endif


#line 87 "bison.simple"
int
yyparse()
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  YYLTYPE *yylsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYMAXDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYMAXDEPTH];	/*  the semantic value stack		*/
  YYLTYPE yylsa[YYMAXDEPTH];	/*  the location stack			*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */
  YYLTYPE *yyls = yylsa;

  int yymaxdepth = YYMAXDEPTH;

#ifndef YYPURE

  int yychar;
  YYSTYPE yylval;
  YYLTYPE yylloc;

  extern int yydebug;

#endif


  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

  if (yydebug)
    fprintf(stderr, "Starting parse\n");

  yystate = 0;
  yyerrstatus = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
  yylsp = yyls;

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yymaxdepth - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      YYLTYPE *yyls1 = yyls;
      short *yyss1 = yyss;

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yymaxdepth);

      yyss = yyss1; yyvs = yyvs1; yyls = yyls1;
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yymaxdepth >= YYMAXLIMIT)
	yyerror("parser stack overflow");
      yymaxdepth *= 2;
      if (yymaxdepth > YYMAXLIMIT)
	yymaxdepth = YYMAXLIMIT;
      yyss = (short *) alloca (yymaxdepth * sizeof (*yyssp));
      bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyls = (YYLTYPE *) alloca (yymaxdepth * sizeof (*yylsp));
      bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
      yyvs = (YYSTYPE *) alloca (yymaxdepth * sizeof (*yyvsp));
      bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yylsp = yyls + size - 1;
      yyvsp = yyvs + size - 1;

      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yymaxdepth);

      if (yyssp >= yyss + yymaxdepth - 1)
	YYERROR;
    }

  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
yyresume:

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

      if (yydebug)
	fprintf(stderr, "Parsing next token; it is %d (%s)\n", yychar, yytname[yychar1]);
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
  *++yylsp = yylloc;

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  yyval = yyvsp[1-yylen]; /* implement default value of the action */

  if (yydebug)
    {
      if (yylen == 1)
	fprintf (stderr, "Reducing 1 value via line %d, ",
		 yyrline[yyn]);
      else
	fprintf (stderr, "Reducing %d values via line %d, ",
		 yylen, yyrline[yyn]);
    }


  switch (yyn) {

case 3:
#line 158 "expread.y"
{ write_exp_elt (BINOP_COMMA); ;
    break;}
case 4:
#line 163 "expread.y"
{ write_exp_elt (UNOP_IND); ;
    break;}
case 5:
#line 166 "expread.y"
{ write_exp_elt (UNOP_ADDR); ;
    break;}
case 6:
#line 169 "expread.y"
{ write_exp_elt (UNOP_NEG); ;
    break;}
case 7:
#line 173 "expread.y"
{ write_exp_elt (UNOP_ZEROP); ;
    break;}
case 8:
#line 177 "expread.y"
{ write_exp_elt (UNOP_LOGNOT); ;
    break;}
case 9:
#line 181 "expread.y"
{ write_exp_elt (UNOP_PREINCREMENT); ;
    break;}
case 10:
#line 185 "expread.y"
{ write_exp_elt (UNOP_PREDECREMENT); ;
    break;}
case 11:
#line 189 "expread.y"
{ write_exp_elt (UNOP_POSTINCREMENT); ;
    break;}
case 12:
#line 193 "expread.y"
{ write_exp_elt (UNOP_POSTDECREMENT); ;
    break;}
case 13:
#line 197 "expread.y"
{ write_exp_elt (UNOP_SIZEOF); ;
    break;}
case 14:
#line 201 "expread.y"
{ write_exp_elt (STRUCTOP_PTR);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt (STRUCTOP_PTR); ;
    break;}
case 15:
#line 207 "expread.y"
{ write_exp_elt (STRUCTOP_MPTR); ;
    break;}
case 16:
#line 211 "expread.y"
{ write_exp_elt (STRUCTOP_STRUCT);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt (STRUCTOP_STRUCT); ;
    break;}
case 17:
#line 217 "expread.y"
{ write_exp_elt (STRUCTOP_MEMBER); ;
    break;}
case 18:
#line 221 "expread.y"
{ write_exp_elt (BINOP_SUBSCRIPT); ;
    break;}
case 19:
#line 227 "expread.y"
{ start_arglist (); ;
    break;}
case 20:
#line 229 "expread.y"
{ write_exp_elt (OP_FUNCALL);
			  write_exp_elt (end_arglist ());
			  write_exp_elt (OP_FUNCALL); ;
    break;}
case 22:
#line 238 "expread.y"
{ arglist_len = 1; ;
    break;}
case 23:
#line 242 "expread.y"
{ arglist_len++; ;
    break;}
case 24:
#line 246 "expread.y"
{ write_exp_elt (UNOP_MEMVAL);
			  write_exp_elt (yyvsp[-2].tval);
			  write_exp_elt (UNOP_MEMVAL); ;
    break;}
case 25:
#line 252 "expread.y"
{ write_exp_elt (UNOP_CAST);
			  write_exp_elt (yyvsp[-2].tval);
			  write_exp_elt (UNOP_CAST); ;
    break;}
case 26:
#line 258 "expread.y"
{ ;
    break;}
case 27:
#line 264 "expread.y"
{ write_exp_elt (BINOP_REPEAT); ;
    break;}
case 28:
#line 268 "expread.y"
{ write_exp_elt (BINOP_MUL); ;
    break;}
case 29:
#line 272 "expread.y"
{ write_exp_elt (BINOP_DIV); ;
    break;}
case 30:
#line 276 "expread.y"
{ write_exp_elt (BINOP_REM); ;
    break;}
case 31:
#line 280 "expread.y"
{ write_exp_elt (BINOP_ADD); ;
    break;}
case 32:
#line 284 "expread.y"
{ write_exp_elt (BINOP_SUB); ;
    break;}
case 33:
#line 288 "expread.y"
{ write_exp_elt (BINOP_LSH); ;
    break;}
case 34:
#line 292 "expread.y"
{ write_exp_elt (BINOP_RSH); ;
    break;}
case 35:
#line 296 "expread.y"
{ write_exp_elt (BINOP_EQUAL); ;
    break;}
case 36:
#line 300 "expread.y"
{ write_exp_elt (BINOP_NOTEQUAL); ;
    break;}
case 37:
#line 304 "expread.y"
{ write_exp_elt (BINOP_LEQ); ;
    break;}
case 38:
#line 308 "expread.y"
{ write_exp_elt (BINOP_GEQ); ;
    break;}
case 39:
#line 312 "expread.y"
{ write_exp_elt (BINOP_LESS); ;
    break;}
case 40:
#line 316 "expread.y"
{ write_exp_elt (BINOP_GTR); ;
    break;}
case 41:
#line 320 "expread.y"
{ write_exp_elt (BINOP_LOGAND); ;
    break;}
case 42:
#line 324 "expread.y"
{ write_exp_elt (BINOP_LOGXOR); ;
    break;}
case 43:
#line 328 "expread.y"
{ write_exp_elt (BINOP_LOGIOR); ;
    break;}
case 44:
#line 332 "expread.y"
{ write_exp_elt (BINOP_AND); ;
    break;}
case 45:
#line 336 "expread.y"
{ write_exp_elt (BINOP_OR); ;
    break;}
case 46:
#line 340 "expread.y"
{ write_exp_elt (TERNOP_COND); ;
    break;}
case 47:
#line 344 "expread.y"
{ write_exp_elt (BINOP_ASSIGN); ;
    break;}
case 48:
#line 348 "expread.y"
{ write_exp_elt (BINOP_ASSIGN_MODIFY);
			  write_exp_elt (yyvsp[-1].opcode);
			  write_exp_elt (BINOP_ASSIGN_MODIFY); ;
    break;}
case 49:
#line 354 "expread.y"
{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_long);
			  write_exp_elt (yyvsp[0].lval);
			  write_exp_elt (OP_LONG); ;
    break;}
case 50:
#line 361 "expread.y"
{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_char);
			  write_exp_elt (yyvsp[0].lval);
			  write_exp_elt (OP_LONG); ;
    break;}
case 51:
#line 368 "expread.y"
{ write_exp_elt (OP_DOUBLE);
			  write_exp_elt (builtin_type_double);
			  write_exp_elt (yyvsp[0].dval);
			  write_exp_elt (OP_DOUBLE); ;
    break;}
case 53:
#line 378 "expread.y"
{ write_exp_elt (OP_LAST);
			  write_exp_elt (yyvsp[0].lval);
			  write_exp_elt (OP_LAST); ;
    break;}
case 54:
#line 384 "expread.y"
{ write_exp_elt (OP_REGISTER);
			  write_exp_elt (yyvsp[0].lval);
			  write_exp_elt (OP_REGISTER); ;
    break;}
case 55:
#line 390 "expread.y"
{ write_exp_elt (OP_INTERNALVAR);
			  write_exp_elt (yyvsp[0].ivar);
			  write_exp_elt (OP_INTERNALVAR); ;
    break;}
case 56:
#line 396 "expread.y"
{ write_exp_elt (OP_LONG);
			  write_exp_elt (builtin_type_int);
			  write_exp_elt ((long) TYPE_LENGTH (yyvsp[-1].tval));
			  write_exp_elt (OP_LONG); ;
    break;}
case 57:
#line 403 "expread.y"
{ write_exp_elt (OP_STRING);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt (OP_STRING); ;
    break;}
case 58:
#line 410 "expread.y"
{ write_exp_elt (OP_THIS);
			  write_exp_elt (OP_THIS); ;
    break;}
case 59:
#line 417 "expread.y"
{
			  struct symtab *tem = lookup_symtab (copy_name (yyvsp[0].sval));
			  struct symbol *sym;
			  
			  if (tem)
			    yyval.bval = BLOCKVECTOR_BLOCK (BLOCKVECTOR (tem), 1);
			  else
			    {
			      sym = lookup_symbol (copy_name (yyvsp[0].sval),
						   expression_context_block,
						   VAR_NAMESPACE);
			      if (sym && SYMBOL_CLASS (sym) == LOC_BLOCK)
				yyval.bval = SYMBOL_BLOCK_VALUE (sym);
			      else
				error ("No file or function \"%s\".",
				       copy_name (yyvsp[0].sval));
			    }
			;
    break;}
case 60:
#line 438 "expread.y"
{
			  struct symbol *tem
			    = lookup_symbol (copy_name (yyvsp[0].sval), yyvsp[-2].bval, VAR_NAMESPACE);
			  if (!tem || SYMBOL_CLASS (tem) != LOC_BLOCK)
			    error ("No function \"%s\" in specified context.",
				   copy_name (yyvsp[-2].bval));
			  yyval.bval = SYMBOL_BLOCK_VALUE (tem);
			;
    break;}
case 61:
#line 449 "expread.y"
{
			  struct symbol *sym;
			  sym = lookup_symbol (copy_name (yyvsp[0].sval), yyvsp[-2].bval, VAR_NAMESPACE);
			  if (sym == 0)
			    error ("No symbol \"%s\" in specified context.",
				   copy_name (yyvsp[0].sval));
			  write_exp_elt (OP_VAR_VALUE);
			  write_exp_elt (sym);
			  write_exp_elt (OP_VAR_VALUE);
			;
    break;}
case 62:
#line 462 "expread.y"
{
			  struct type *type = yyvsp[-2].tval;
			  if (TYPE_CODE (type) != TYPE_CODE_STRUCT
			      && TYPE_CODE (type) != TYPE_CODE_UNION)
			    error ("`%s' is not defined as an aggregate type.",
				   TYPE_NAME (type));

			  write_exp_elt (OP_SCOPE);
			  write_exp_elt (type);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt (OP_SCOPE);
			;
    break;}
case 63:
#line 475 "expread.y"
{
			  char *name = copy_name (yyvsp[0].sval);
			  struct symbol *sym;
			  int i;

			  sym = lookup_symbol_2 (name, 0, VAR_NAMESPACE);
			  if (sym)
			    {
			      write_exp_elt (OP_VAR_VALUE);
			      write_exp_elt (sym);
			      write_exp_elt (OP_VAR_VALUE);
			      break;
			    }
			  for (i = 0; i < misc_function_count; i++)
			    if (!strcmp (misc_function_vector[i].name, name))
			      break;

			  if (i < misc_function_count)
			    {
			      write_exp_elt (OP_LONG);
			      write_exp_elt (builtin_type_int);
			      write_exp_elt (misc_function_vector[i].address);
			      write_exp_elt (OP_LONG);
			      write_exp_elt (UNOP_MEMVAL);
			      write_exp_elt (builtin_type_char);
			      write_exp_elt (UNOP_MEMVAL);
			    }
			  else
			    if (symtab_list == 0)
			      error ("No symbol table is loaded.  Use the \"symbol-file\" command.");
			    else
			      error ("No symbol \"%s\" in current context.", name);
			;
    break;}
case 64:
#line 511 "expread.y"
{ struct symbol *sym;
			  sym = lookup_symbol_1 (copy_name (yyvsp[0].sval),
						 expression_context_block,
						 VAR_NAMESPACE);
			  if (sym)
			    {
			      write_exp_elt (OP_VAR_VALUE);
			      write_exp_elt (sym);
			      write_exp_elt (OP_VAR_VALUE);
			    }
			  else
			    {
			      register char *arg = copy_name (yyvsp[0].sval);
			      register int i;
			      int v, val;
			      /* C++: see if it hangs off of `this'.  Must
			         not inadvertently convert from a method call
				 to data ref.  */
			      v = (int)value_of_this (0);
			      if (v)
				{
				  val = check_field (v, arg);
				  if (val)
				    {
				      write_exp_elt (OP_THIS);
				      write_exp_elt (OP_THIS);
				      write_exp_elt (STRUCTOP_PTR);
				      write_exp_string (yyvsp[0].sval);
				      write_exp_elt (STRUCTOP_PTR);
				      break;
				    }
				}
			      sym = lookup_symbol_2 (arg, 0, VAR_NAMESPACE);
			      if (sym)
				{
				  write_exp_elt (OP_VAR_VALUE);
				  write_exp_elt (sym);
				  write_exp_elt (OP_VAR_VALUE);
				  break; /* YACC-dependent */
				}
			      for (i = 0; i < misc_function_count; i++)
				if (!strcmp (misc_function_vector[i].name, arg))
				  break;

			      if (i < misc_function_count)
				{
				  write_exp_elt (OP_LONG);
				  write_exp_elt (builtin_type_int);
				  write_exp_elt (misc_function_vector[i].address);
				  write_exp_elt (OP_LONG);
				  write_exp_elt (UNOP_MEMVAL);
				  write_exp_elt (builtin_type_char);
				  write_exp_elt (UNOP_MEMVAL);
				}
			      else
				if (symtab_list == 0)
				  error ("No symbol table is loaded.  Use the \"symbol-file\" command.");
				else
				  error ("No symbol \"%s\" in current context.",
					 copy_name (yyvsp[0].sval));
			    }
			;
    break;}
case 66:
#line 577 "expread.y"
{ yyval.tval = lookup_pointer_type (yyvsp[-1].tval); ;
    break;}
case 67:
#line 579 "expread.y"
{ yyval.tval = lookup_reference_type (yyvsp[-1].tval); ;
    break;}
case 68:
#line 581 "expread.y"
{ yyval.tval = lookup_member_pointer_type (builtin_type_int, yyvsp[-2].tval); ;
    break;}
case 69:
#line 583 "expread.y"
{ yyval.tval = lookup_member_pointer_type (yyvsp[-5].tval, yyvsp[-3].tval); ;
    break;}
case 70:
#line 585 "expread.y"
{ yyval.tval = lookup_member_pointer_type (lookup_function_type (yyvsp[-7].tval, 0), yyvsp[-5].tval); ;
    break;}
case 71:
#line 587 "expread.y"
{ yyval.tval = lookup_member_pointer_type (lookup_function_type (yyvsp[-8].tval, yyvsp[-1].tvec), yyvsp[-6].tval);
			  free (yyvsp[-1].tvec); ;
    break;}
case 72:
#line 593 "expread.y"
{ yyval.tval = lookup_typename (copy_name (yyvsp[0].sval),
						expression_context_block, 0); ;
    break;}
case 73:
#line 596 "expread.y"
{ yyval.tval = lookup_struct (copy_name (yyvsp[0].sval),
					      expression_context_block); ;
    break;}
case 74:
#line 599 "expread.y"
{ yyval.tval = lookup_union (copy_name (yyvsp[0].sval),
					     expression_context_block); ;
    break;}
case 75:
#line 602 "expread.y"
{ yyval.tval = lookup_enum (copy_name (yyvsp[0].sval),
					    expression_context_block); ;
    break;}
case 76:
#line 605 "expread.y"
{ yyval.tval = lookup_unsigned_typename (copy_name (yyvsp[0].sval)); ;
    break;}
case 77:
#line 610 "expread.y"
{ yyval.tvec = (struct type **)xmalloc (sizeof (struct type *) * 2);
		  yyval.tvec[0] = (struct type *)0;
		  yyval.tvec[1] = yyvsp[0].tval;
		;
    break;}
case 78:
#line 615 "expread.y"
{ int len = sizeof (struct type *) * ++(yyvsp[-2].ivec[0]);
		  yyval.tvec = (struct type **)xrealloc (yyvsp[-2].tvec, len);
		  yyval.tvec[yyval.ivec[0]] = yyvsp[0].tval;
		;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 303 "bison.simple"

  yyvsp -= yylen;
  yylsp -= yylen;
  yyssp -= yylen;

  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now", yyssp-yyss);
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }

  *++yyvsp = yyval;

  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
#ifdef ESKIT
      db_yyerror("parse error", yyssp, yychar);
#else
      yyerror("parse error");
#endif
    }

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYERROR;

      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYERROR;
  yyvsp--;
  yylsp--;
  yystate = *--yyssp;

  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now", yyssp-yyss);
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

  if (yydebug)
    fprintf(stderr, "Shifting error token, ");

  *++yyvsp = yylval;
  *++yylsp = yylloc;

  yystate = yyn;
  goto yynewstate;
}
#line 624 "expread.y"


/* Begin counting arguments for a function call,
   saving the data about any containing call.  */

static void
start_arglist ()
{
  register struct funcall *new = (struct funcall *) xmalloc (sizeof (struct funcall));

  new->next = funcall_chain;
  new->arglist_len = arglist_len;
  arglist_len = 0;
  funcall_chain = new;
}

/* Return the number of arguments in a function call just terminated,
   and restore the data for the containing function call.  */

static int
end_arglist ()
{
  register int val = arglist_len;
  register struct funcall *call = funcall_chain;
  funcall_chain = call->next;
  arglist_len = call->arglist_len;
  free (call);
  return val;
}

/* Free everything in the funcall chain.
   Used when there is an error inside parsing.  */

static void
free_funcalls ()
{
  register struct funcall *call, *next;

  for (call = funcall_chain; call; call = next)
    {
      next = call->next;
      free (call);
    }
}

/* This page contains the functions for adding data to the  struct expression
   being constructed.  */

/* Add one element to the end of the expression.  */

static void
write_exp_elt (expelt)
     union exp_element expelt;
{
  if (expout_ptr >= expout_size)
    {
      expout_size *= 2;
      expout = (struct expression *) xrealloc (expout,
					       sizeof (struct expression)
					       + expout_size * sizeof (union exp_element));
    }
  expout->elts[expout_ptr++] = expelt;
}

/* Add a string constant to the end of the expression.
   Follow it by its length in bytes, as a separate exp_element.  */

static void
write_exp_string (str)
     struct stoken str;
{
  register int len = str.length;
  register int lenelt
    = (len + sizeof (union exp_element)) / sizeof (union exp_element);

  expout_ptr += lenelt;

  if (expout_ptr >= expout_size)
    {
      expout_size = max (expout_size * 2, expout_ptr + 10);
      expout = (struct expression *) xrealloc (expout,
					       sizeof (struct expression)
					       + expout_size * sizeof (union exp_element));
    }
  bcopy (str.ptr, (char *) &expout->elts[expout_ptr - lenelt], len);
  ((char *) &expout->elts[expout_ptr - lenelt])[len] = 0;
  write_exp_elt (len);
}

/* During parsing of a C expression, the pointer to the next character
   is in this variable.  */

static char *lexptr;

/* Tokens that refer to names do so with explicit pointer and length,
   so they can share the storage that lexptr is parsing.

   When it is necessary to pass a name to a function that expects
   a null-terminated string, the substring is copied out
   into a block of storage that namecopy points to.

   namecopy is allocated once, guaranteed big enough, for each parsing.  */

static char *namecopy;

/* Current depth in parentheses within the expression.  */

static int paren_depth;

/* Nonzero means stop parsing on first comma (if not within parentheses).  */

static int comma_terminates;

/* Take care of parsing a number (anything that starts with a digit).
   Set yylval and return the token type; update lexptr.
   LEN is the number of characters in it.  */

/*** Needs some error checking for the float case ***/

static int
parse_number (olen)
     int olen;
{
  register char *p = lexptr;
  register long n = 0;
  register int c;
  register int base = 10;
  register int len = olen;
  char *err_copy;

  extern double atof ();

  for (c = 0; c < len; c++)
    if (p[c] == '.')
      {
	/* It's a float since it contains a point.  */
	yylval.dval = atof (p);
	lexptr += len;
	return FLOAT;
      }

  if (len >= 3 && (!strncmp (p, "0x", 2) || !strncmp (p, "0X", 2)))
    {
      p += 2;
      base = 16;
      len -= 2;
    }
  else if (*p == '0')
    base = 8;

  while (len-- > 0)
    {
      c = *p++;
      n *= base;
      if (c >= '0' && c <= '9')
	n += c - '0';
      else
	{
	  if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
	  if (base == 16 && c >= 'a' && c <= 'f')
	    n += c - 'a' + 10;
	  else if (len == 0 && c == 'l')
	    ;
	  else
	    {
	      err_copy = (char *) alloca (olen + 1);
	      bcopy (lexptr, err_copy, olen);
	      err_copy[olen] = 0;
	      error ("Invalid number \"%s\".", err_copy);
	    }
	}
    }

  lexptr = p;
  yylval.lval = n;
  return INT;
}

struct token
{
  char *operator;
  int token;
  enum exp_opcode opcode;
};

static struct token tokentab3[] =
  {
    {">>=", ASSIGN_MODIFY, BINOP_RSH},
    {"<<=", ASSIGN_MODIFY, BINOP_LSH}
  };

static struct token tokentab2[] =
  {
    {"+=", ASSIGN_MODIFY, BINOP_ADD},
    {"-=", ASSIGN_MODIFY, BINOP_SUB},
    {"*=", ASSIGN_MODIFY, BINOP_MUL},
    {"/=", ASSIGN_MODIFY, BINOP_DIV},
    {"%=", ASSIGN_MODIFY, BINOP_REM},
    {"|=", ASSIGN_MODIFY, BINOP_LOGIOR},
    {"&=", ASSIGN_MODIFY, BINOP_LOGAND},
    {"^=", ASSIGN_MODIFY, BINOP_LOGXOR},
    {"++", INCREMENT, BINOP_END},
    {"--", DECREMENT, BINOP_END},
    {"->", ARROW, BINOP_END},
    {"&&", AND, BINOP_END},
    {"||", OR, BINOP_END},
    {"::", COLONCOLON, BINOP_END},
    {"<<", LSH, BINOP_END},
    {">>", RSH, BINOP_END},
    {"==", EQUAL, BINOP_END},
    {"!=", NOTEQUAL, BINOP_END},
    {"<=", LEQ, BINOP_END},
    {">=", GEQ, BINOP_END}
  };

/* Read one token, getting characters through lexptr.  */

static int
yylex ()
{
  register int c;
  register int namelen;
  register int i;
  register char *tokstart;

 retry:

  tokstart = lexptr;
  /* See if it is a special token of length 3.  */
  for (i = 0; i < sizeof tokentab3 / sizeof tokentab3[0]; i++)
    if (!strncmp (tokstart, tokentab3[i].operator, 3))
      {
	lexptr += 3;
	yylval.opcode = tokentab3[i].opcode;
	return tokentab3[i].token;
      }

  /* See if it is a special token of length 2.  */
  for (i = 0; i < sizeof tokentab2 / sizeof tokentab2[0]; i++)
    if (!strncmp (tokstart, tokentab2[i].operator, 2))
      {
	lexptr += 2;
	yylval.opcode = tokentab2[i].opcode;
	return tokentab2[i].token;
      }

  switch (c = *tokstart)
    {
    case 0:
      return 0;

    case ' ':
    case '\t':
    case '\n':
      lexptr++;
      goto retry;

    case '\'':
      lexptr++;
      c = *lexptr++;
      if (c == '\\')
	c = parse_escape (&lexptr);
      yylval.lval = c;
      c = *lexptr++;
      if (c != '\'')
	error ("Invalid character constant.");
      return CHAR;

    case '(':
      paren_depth++;
      lexptr++;
      return c;

    case ')':
      if (paren_depth == 0)
	return 0;
      paren_depth--;
      lexptr++;
      return c;

    case ',':
      if (comma_terminates && paren_depth == 0)
	return 0;
      lexptr++;
      return c;

    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '|':
    case '&':
    case '^':
    case '~':
    case '!':
    case '@':
    case '<':
    case '>':
    case '[':
    case ']':
    case '.':
    case '?':
    case ':':
    case '=':
    case '{':
    case '}':
      lexptr++;
      return c;

    case '"':
      for (namelen = 1; (c = tokstart[namelen]) != '"'; namelen++)
	if (c == '\\')
	  {
	    c = tokstart[++namelen];
	    if (c >= '0' && c <= '9')
	      {
		c = tokstart[++namelen];
		if (c >= '0' && c <= '9')
		  c = tokstart[++namelen];
	      }
	  }
      yylval.sval.ptr = tokstart + 1;
      yylval.sval.length = namelen - 1;
      lexptr += namelen + 1;
      return STRING;
    }
  if (c >= '0' && c <= '9')
    {
      /* It's a number */
      for (namelen = 0;
	   c = tokstart[namelen],
	   (c == '_' || c == '$' || c == '.' || (c >= '0' && c <= '9')
	    || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
	   namelen++)
	;
      return parse_number (namelen);
    }

  if (!(c == '_' || c == '$'
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
    error ("Invalid token in expression.");

  /* It is a name.  See how long it is.  */

  for (namelen = 0;
       c = tokstart[namelen],
       (c == '_' || c == '$' || (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
       namelen++)
    ;

  /* The token "if" terminates the expression and is NOT 
     removed from the input stream.  */
  if (namelen == 2 && tokstart[0] == 'i' && tokstart[1] == 'f')
    {
      return 0;
    }

  lexptr += namelen;

  /* Handle the tokens $digits; also $ (short for $0) and $$ (short for $$1)
     and $$digits (equivalent to $<-digits> if you could type that).
     Make token type LAST, and put the number (the digits) in yylval.  */

  if (*tokstart == '$')
    {
      register int negate = 0;
      c = 1;
      /* Double dollar means negate the number and add -1 as well.
	 Thus $$ alone means -1.  */
      if (namelen >= 2 && tokstart[1] == '$')
	{
	  negate = 1;
	  c = 2;
	}
      if (c == namelen)
	{
	  /* Just dollars (one or two) */
	  yylval.lval = - negate;
	  return LAST;
	}
      /* Is the rest of the token digits?  */
      for (; c < namelen; c++)
	if (!(tokstart[c] >= '0' && tokstart[c] <= '9'))
	  break;
      if (c == namelen)
	{
	  yylval.lval = atoi (tokstart + 1 + negate);
	  if (negate)
	    yylval.lval = - yylval.lval;
	  return LAST;
	}
    }

  /* Handle tokens that refer to machine registers:
     $ followed by a register name.  */

  if (*tokstart == '$')
    for (c = 0; c < NUM_REGS; c++)
      if (namelen - 1 == strlen (reg_names[c])
	  && !strncmp (tokstart + 1, reg_names[c], namelen - 1))
	{
	  yylval.lval = c;
	  return REGNAME;
	}

  if (namelen == 6 && !strncmp (tokstart, "struct", 6))
    {
      return STRUCT;
    }
  if (namelen == 5)
    {
      if (!strncmp (tokstart, "union", 5))
	{
	  return UNION;
	}
    }
  if (namelen == 4)
    {
      if (!strncmp (tokstart, "enum", 4))
	{
	  return ENUM;
	}
      if (!strncmp (tokstart, "this", 4))
	{
	  return THIS;
	}
    }
  if (namelen == 6 && !strncmp (tokstart, "sizeof", 6))
    {
      return SIZEOF;
    }
  if (namelen == 8 && !strncmp (tokstart, "unsigned", 6))
    {
      return UNSIGNED;
    }
  yylval.sval.ptr = tokstart;
  yylval.sval.length = namelen;

  /* Any other names starting in $ are debugger internal variables.  */

  if (*tokstart == '$')
    {
      yylval.ivar = (struct internalvar *) lookup_internalvar (copy_name (yylval.sval) + 1);
      return VARIABLE;
    }

  /* Use token-type TYPENAME for symbols that happen to be defined
     currently as names of types; NAME for other symbols.
     The caller is not constrained to care about the distinction.  */
  if (lookup_typename (copy_name (yylval.sval), expression_context_block, 1))
    return TYPENAME;
  return NAME;
}

static
yyerror ()
{
  error ("Invalid syntax in expression.");
}

/* Return a null-terminated temporary copy of the name
   of a string token.  */

static char *
copy_name (token)
     struct stoken token;
{
  bcopy (token.ptr, namecopy, token.length);
  namecopy[token.length] = 0;
  return namecopy;
}

/* Reverse an expression from suffix form (in which it is constructed)
   to prefix form (in which we can conveniently print or execute it).  */

static void prefixify_subexp ();

static void
prefixify_expression (expr)
     register struct expression *expr;
{
  register int len = sizeof (struct expression) +
				    expr->nelts * sizeof (union exp_element);
  register struct expression *temp;
  register int inpos = expr->nelts, outpos = 0;

  temp = (struct expression *) alloca (len);

  /* Copy the original expression into temp.  */
  bcopy (expr, temp, len);

  prefixify_subexp (temp, expr, inpos, outpos);
}

/* Return the number of exp_elements in the subexpression of EXPR
   whose last exp_element is at index ENDPOS - 1 in EXPR.  */

static int
length_of_subexp (expr, endpos)
     register struct expression *expr;
     register int endpos;
{
  register int oplen = 1;
  register int args = 0;
  register int i;

  if (endpos < 0)
    error ("?error in length_of_subexp");

  i = (int) expr->elts[endpos - 1].opcode;

  switch (i)
    {
      /* C++  */
    case OP_SCOPE:
      oplen = 4 + ((expr->elts[endpos - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
      break;

    case OP_LONG:
    case OP_DOUBLE:
      oplen = 4;
      break;

    case OP_VAR_VALUE:
    case OP_LAST:
    case OP_REGISTER:
    case OP_INTERNALVAR:
      oplen = 3;
      break;

    case OP_FUNCALL:
      oplen = 3;
      args = 1 + expr->elts[endpos - 2].longconst;
      break;

    case UNOP_CAST:
    case UNOP_MEMVAL:
      oplen = 3;
      args = 1;
      break;

    case STRUCTOP_STRUCT:
    case STRUCTOP_PTR:
      args = 1;
    case OP_STRING:
      oplen = 3 + ((expr->elts[endpos - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
      break;

    case TERNOP_COND:
      args = 3;
      break;

    case BINOP_ASSIGN_MODIFY:
      oplen = 3;
      args = 2;
      break;

      /* C++ */
    case OP_THIS:
      oplen = 2;
      break;

    default:
      args = 1 + (i < (int) BINOP_END);
    }

  while (args > 0)
    {
      oplen += length_of_subexp (expr, endpos - oplen);
      args--;
    }

  return oplen;
}

/* Copy the subexpression ending just before index INEND in INEXPR
   into OUTEXPR, starting at index OUTBEG.
   In the process, convert it from suffix to prefix form.  */

static void
prefixify_subexp (inexpr, outexpr, inend, outbeg)
     register struct expression *inexpr;
     struct expression *outexpr;
     register int inend;
     int outbeg;
{
  register int oplen = 1;
  register int args = 0;
  register int i;
  int *arglens;
  enum exp_opcode opcode;

  /* Compute how long the last operation is (in OPLEN),
     and also how many preceding subexpressions serve as
     arguments for it (in ARGS).  */

  opcode = inexpr->elts[inend - 1].opcode;
  switch (opcode)
    {
      /* C++  */
    case OP_SCOPE:
      oplen = 4 + ((inexpr->elts[inend - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
      break;

    case OP_LONG:
    case OP_DOUBLE:
      oplen = 4;
      break;

    case OP_VAR_VALUE:
    case OP_LAST:
    case OP_REGISTER:
    case OP_INTERNALVAR:
      oplen = 3;
      break;

    case OP_FUNCALL:
      oplen = 3;
      args = 1 + inexpr->elts[inend - 2].longconst;
      break;

    case UNOP_CAST:
    case UNOP_MEMVAL:
      oplen = 3;
      args = 1;
      break;

    case STRUCTOP_STRUCT:
    case STRUCTOP_PTR:
      args = 1;
    case OP_STRING:
      oplen = 3 + ((inexpr->elts[inend - 2].longconst
		    + sizeof (union exp_element))
		   / sizeof (union exp_element));
		   
      break;

    case TERNOP_COND:
      args = 3;
      break;

    case BINOP_ASSIGN_MODIFY:
      oplen = 3;
      args = 2;
      break;

      /* C++ */
    case OP_THIS:
      oplen = 2;
      break;

    default:
      args = 1 + ((int) opcode < (int) BINOP_END);
    }

  /* Copy the final operator itself, from the end of the input
     to the beginning of the output.  */
  inend -= oplen;
  bcopy (&inexpr->elts[inend], &outexpr->elts[outbeg],
	 oplen * sizeof (union exp_element));
  outbeg += oplen;

  /* Find the lengths of the arg subexpressions.  */
  arglens = (int *) alloca (args * sizeof (int));
  for (i = args - 1; i >= 0; i--)
    {
      oplen = length_of_subexp (inexpr, inend);
      arglens[i] = oplen;
      inend -= oplen;
    }

  /* Now copy each subexpression, preserving the order of
     the subexpressions, but prefixifying each one.
     In this loop, inend starts at the beginning of
     the expression this level is working on
     and marches forward over the arguments.
     outbeg does similarly in the output.  */
  for (i = 0; i < args; i++)
    {
      oplen = arglens[i];
      inend += oplen;
      prefixify_subexp (inexpr, outexpr, inend, outbeg);
      outbeg += oplen;
    }
}

/* This page contains the two entry points to this file.  */

/* Read a C expression from the string *STRINGPTR points to,
   parse it, and return a pointer to a  struct expression  that we malloc.
   Use block BLOCK as the lexical context for variable names;
   if BLOCK is zero, use the block of the selected stack frame.
   Meanwhile, advance *STRINGPTR to point after the expression,
   at the first nonwhite character that is not part of the expression
   (possibly a null character).

   If COMMA is nonzero, stop if a comma is reached.  */

struct expression *
parse_c_1 (stringptr, block, comma)
     char **stringptr;
     struct block *block;
{
  struct cleanup *old_chain;

  lexptr = *stringptr;

  comma_terminates = comma;

  if (lexptr == 0 || *lexptr == 0)
    error_no_arg ("expression to compute");

  old_chain = make_cleanup (free_funcalls, 0);
  funcall_chain = 0;

  expression_context_block = block ? block : get_selected_block ();

  namecopy = (char *) alloca (strlen (lexptr) + 1);
  expout_size = 10;
  expout_ptr = 0;
  expout = (struct expression *) xmalloc (sizeof (struct expression)
					  + expout_size * sizeof (union exp_element));
  make_cleanup (free_current_contents, &expout);
  if (yyparse ())
    yyerror ();
  discard_cleanups (old_chain);
  expout->nelts = expout_ptr;
  expout = (struct expression *)
    xrealloc (expout,
	      sizeof (struct expression)
	      + expout_ptr * sizeof (union exp_element));
  prefixify_expression (expout);
  *stringptr = lexptr;
  return expout;
}

/* Parse STRING as an expression, and complain if this fails
   to use up all of the contents of STRING.  */

struct expression *
parse_c_expression (string)
     char *string;
{
  register struct expression *exp;
  exp = parse_c_1 (&string, 0, 0);
  if (*string)
    error ("Junk after end of expression.");
  return exp;
}
