
/*  A Bison parser, made from ../expread.y  */

#define	INT	258
#define	CHAR	259
#define	UINT	260
#define	FLOAT	261
#define	STRING	262
#define	NAME	263
#define	BLOCKNAME	264
#define	TYPENAME	265
#define	NAME_OR_INT	266
#define	NAME_OR_UINT	267
#define	STRUCT	268
#define	UNION	269
#define	ENUM	270
#define	SIZEOF	271
#define	UNSIGNED	272
#define	COLONCOLON	273
#define	ERROR	274
#define	SIGNED	275
#define	LONG	276
#define	SHORT	277
#define	INT_KEYWORD	278
#define	LAST	279
#define	REGNAME	280
#define	VARIABLE	281
#define	ASSIGN_MODIFY	282
#define	THIS	283
#define	ABOVE_COMMA	284
#define	OR	285
#define	AND	286
#define	EQUAL	287
#define	NOTEQUAL	288
#define	LEQ	289
#define	GEQ	290
#define	LSH	291
#define	RSH	292
#define	UNARY	293
#define	INCREMENT	294
#define	DECREMENT	295
#define	ARROW	296

#line 29 "../expread.y"

#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"
#include "expression.h"
#include "value.h"
#include "command.h"

static struct expression *expout;
static int expout_size;
static int expout_ptr;

static int yylex ();
static void yyerror ();
static void write_exp_elt ();
static void write_exp_elt_opcode ();
static void write_exp_elt_sym ();
static void write_exp_elt_longcst ();
static void write_exp_elt_dblcst ();
static void write_exp_elt_type ();
static void write_exp_elt_intern ();
static void write_exp_string ();
static void start_arglist ();
static int end_arglist ();
static void free_funcalls ();
static char *copy_name ();
static int parse_number ();

/* If this is nonzero, this block is used as the lexical context
   for symbol names.  */

static struct block *expression_context_block;

/* The innermost context required by the stack and register variables
   we've encountered so far. */
struct block *innermost_block;

/* The block in which the most recently discovered symbol was found. */
struct block *block_found;

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

struct ttype
  {
    struct stoken stoken;
    struct type *type;
  };

struct symtoken
  {
    struct stoken stoken;
    struct symbol *sym;
    int is_a_field_of_this;
  };

/* For parsing of complicated types.
   An array should be preceded in the list by the size of the array.  */
enum type_pieces
  {tp_end = -1, tp_pointer, tp_reference, tp_array, tp_function};
static enum type_pieces *type_stack;
static int type_stack_depth, type_stack_size;

static void push_type ();
static enum type_pieces pop_type ();

/* Allow debugging of parsing.  */
#define YYDEBUG 1

#line 125 "../expread.y"
typedef union
  {
    LONGEST lval;
    unsigned LONGEST ulval;
    double dval;
    struct symbol *sym;
    struct type *tval;
    struct stoken sval;
    struct ttype tsym;
    struct symtoken ssym;
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

#include <stdio.h>

#ifndef __STDC__
#define const
#endif



#define	YYFINAL		189
#define	YYFLAG		-32768
#define	YYNTBASE	66

#define YYTRANSLATE(x) ((unsigned)(x) <= 296 ? yytranslate[x] : 84)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    59,     2,     2,     2,    51,    37,     2,    58,
    62,    49,    47,    29,    48,    56,    50,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    65,     2,    40,
    31,    41,    32,    46,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    57,     2,    61,    36,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    63,    35,    64,    60,     2,     2,     2,     2,
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
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    30,    33,    34,    38,    39,    42,    43,
    44,    45,    52,    53,    54,    55
};

static const short yyrline[] = {     0,
   218,   222,   223,   228,   231,   234,   238,   242,   246,   250,
   254,   258,   262,   266,   272,   276,   282,   286,   290,   294,
   300,   303,   307,   311,   317,   323,   329,   333,   337,   341,
   345,   349,   353,   357,   361,   365,   369,   373,   377,   381,
   385,   389,   393,   397,   401,   405,   409,   413,   419,   429,
   442,   454,   467,   474,   481,   484,   490,   496,   502,   509,
   516,   523,   540,   550,   562,   575,   621,   695,   696,   731,
   733,   735,   738,   740,   745,   751,   753,   757,   759,   763,
   767,   768,   770,   772,   775,   782,   784,   786,   788,   790,
   792,   794,   796,   798,   801,   804,   807,   809,   811,   813,
   817,   818,   824,   830,   839,   844,   851,   852,   853,   854,
   855,   858,   859,   860,   861
};

static const char * const yytname[] = {     0,
"error","$illegal.","INT","CHAR","UINT","FLOAT","STRING","NAME","BLOCKNAME","TYPENAME",
"NAME_OR_INT","NAME_OR_UINT","STRUCT","UNION","ENUM","SIZEOF","UNSIGNED","COLONCOLON","ERROR","SIGNED",
"LONG","SHORT","INT_KEYWORD","LAST","REGNAME","VARIABLE","ASSIGN_MODIFY","THIS","','","ABOVE_COMMA",
"'='","'?'","OR","AND","'|'","'^'","'&'","EQUAL","NOTEQUAL","'<'",
"'>'","LEQ","GEQ","LSH","RSH","'@'","'+'","'-'","'*'","'/'",
"'%'","UNARY","INCREMENT","DECREMENT","ARROW","'.'","'['","'('","'!'","'~'",
"']'","')'","'{'","'}'","':'","start"
};

static const short yyr1[] = {     0,
    66,    67,    67,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    68,    68,    68,    68,    68,    69,    68,
    70,    70,    70,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    68,    68,    68,    68,    68,    68,    68,    68,    68,
    68,    71,    71,    72,    72,    72,    72,    73,    73,    74,
    74,    74,    75,    75,    75,    75,    75,    76,    76,    77,
    78,    78,    78,    78,    78,    79,    79,    79,    79,    79,
    79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
    80,    80,    80,    80,    81,    81,    82,    82,    82,    82,
    82,    83,    83,    83,    83
};

static const short yyr2[] = {     0,
     1,     1,     3,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     3,     4,     3,     4,     4,     0,     5,
     0,     1,     3,     4,     4,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     5,     3,     3,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     4,     1,
     1,     1,     3,     3,     3,     2,     1,     1,     2,     1,
     2,     1,     3,     2,     1,     2,     1,     2,     3,     2,
     1,     3,     6,     8,     9,     1,     1,     1,     1,     2,
     3,     2,     3,     2,     2,     2,     2,     1,     2,     1,
     1,     1,     1,     1,     1,     3,     1,     1,     1,     1,
     1,     1,     1,     1,     1
};

static const short yydefact[] = {     0,
    49,    53,    51,    54,    60,   112,   113,    86,    50,    52,
     0,     0,     0,     0,    98,     0,   100,    88,    89,    87,
    56,    57,    58,    61,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     1,     2,     0,    55,     0,    67,   107,
   108,   109,   110,   111,    94,    95,    96,     0,    13,   101,
   103,   104,   102,    97,    66,   103,   104,    99,    90,    92,
     5,     6,     4,     9,    10,     0,    81,     0,    68,     7,
     8,     0,    68,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,    11,    12,     0,     0,
     0,    19,     0,     0,     0,    91,    93,    26,     0,     0,
     0,    70,     0,     0,    69,    72,    75,    77,     0,     0,
     3,    48,    47,     0,    45,    44,    43,    42,    41,    35,
    36,    39,    40,    37,    38,    33,    34,    27,    31,    32,
    28,    29,    30,     0,    14,     0,    16,     0,    21,    64,
    65,    59,     0,    25,    82,    71,     0,    78,    80,     0,
     0,    74,    76,    24,     0,    15,    17,    18,    22,     0,
     0,    79,    73,    46,     0,    20,     0,    23,    83,     0,
    84,   105,     0,     0,    85,   106,     0,     0,     0
};

static const short yydefgoto[] = {   187,
    66,    35,   149,   170,    36,    37,    67,   115,   116,   117,
   118,    68,    38,    54,   183,   151,    39
};

static const short yypact[] = {   159,
-32768,-32768,-32768,-32768,-32768,-32768,   -13,-32768,-32768,-32768,
    21,    21,    21,   220,   121,    21,   124,   -15,     3,-32768,
-32768,-32768,-32768,-32768,   159,   159,   159,   159,   159,   159,
   159,   159,   272,    80,   306,    32,-32768,    99,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   159,   508,-32768,
   100,   101,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   508,   508,   508,   508,   508,    -4,-32768,   -52,    79,   508,
   508,   -55,    92,   159,   159,   159,   159,   159,   159,   159,
   159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
   159,   159,   159,   159,   159,   159,-32768,-32768,    90,   104,
   159,-32768,    21,    21,   -35,-32768,-32768,-32768,   272,   159,
   110,   157,    25,   209,-32768,    30,-32768,-32768,   159,    84,
   306,   306,   306,   271,   358,   382,   405,   427,   448,   467,
   467,   144,   144,   144,   144,   480,   480,   492,   502,   502,
   508,   508,   508,   159,-32768,   159,-32768,    66,   159,   111,
-32768,    31,   122,   508,-32768,-32768,    94,-32768,-32768,    95,
    98,-32768,-32768,   508,   159,   508,   508,-32768,   306,    63,
   107,-32768,-32768,   333,   159,-32768,   116,   306,   128,   239,
-32768,   145,    64,   272,-32768,   145,   204,   205,-32768
};

static const short yypgoto[] = {-32768,
     2,   -14,-32768,-32768,-32768,-32768,-32768,    -8,-32768,    93,
   105,   -32,   -26,   193,-32768,     8,-32768
};


#define	YYLAST		566


static const short yytable[] = {    49,
    72,    34,   109,    69,   -62,   109,    73,    59,   119,   110,
    61,    62,    63,    64,    65,   105,    70,    71,    45,    46,
    47,    69,   109,    55,    74,    60,   152,   157,    40,    41,
    42,    43,    44,     1,     2,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,   103,
    17,    18,    19,    20,    21,    22,    23,   108,    24,   121,
   122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
   132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
   142,   143,   153,    28,    29,   158,   113,   161,    30,    31,
    32,   175,   184,    33,    74,   154,   111,    40,    41,    42,
    43,    44,   148,   156,   164,   160,   145,   147,    74,   120,
   150,    40,    41,    42,    43,    44,   104,    40,    41,    42,
    43,    44,   106,   107,   176,   185,   168,   112,   -63,   166,
    50,   167,   155,    50,   169,   113,   114,   154,   144,   171,
   112,    51,    52,    53,    56,    57,    53,   182,   113,   114,
   174,   186,   146,    73,   172,   177,   173,    73,   155,   159,
   178,     1,     2,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,   179,    17,    18,
    19,    20,    21,    22,    23,   180,    24,    89,    90,    91,
    92,    93,    94,    95,    96,    25,    97,    98,    99,   100,
   101,   102,   109,   188,   189,   112,    26,    27,   162,    58,
     0,    28,    29,   113,   114,     0,    30,    31,    32,     0,
   163,    33,     1,     2,     3,     4,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,     0,    17,
    18,    19,    20,    21,    22,    23,     0,    24,     8,     0,
     0,    11,    12,    13,     0,    15,    25,   112,    17,    18,
    19,    20,     0,     0,     0,   113,   114,    26,    27,     0,
   159,     0,    28,    29,     0,     0,     0,    48,    31,    32,
     0,     8,    33,     0,    11,    12,    13,     0,    15,     0,
     0,    17,    18,    19,    20,     0,     0,    75,     0,     0,
   181,    76,    77,    78,    79,    80,    81,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,    96,     0,    97,    98,    99,   100,   101,   102,     0,
     0,     0,    75,     0,     0,   165,    76,    77,    78,    79,
    80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
    90,    91,    92,    93,    94,    95,    96,     0,    97,    98,
    99,   100,   101,   102,    77,    78,    79,    80,    81,    82,
    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
    93,    94,    95,    96,     0,    97,    98,    99,   100,   101,
   102,    79,    80,    81,    82,    83,    84,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
    97,    98,    99,   100,   101,   102,    80,    81,    82,    83,
    84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
    94,    95,    96,     0,    97,    98,    99,   100,   101,   102,
    81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
    91,    92,    93,    94,    95,    96,     0,    97,    98,    99,
   100,   101,   102,    82,    83,    84,    85,    86,    87,    88,
    89,    90,    91,    92,    93,    94,    95,    96,     0,    97,
    98,    99,   100,   101,   102,    83,    84,    85,    86,    87,
    88,    89,    90,    91,    92,    93,    94,    95,    96,     0,
    97,    98,    99,   100,   101,   102,    85,    86,    87,    88,
    89,    90,    91,    92,    93,    94,    95,    96,     0,    97,
    98,    99,   100,   101,   102,    91,    92,    93,    94,    95,
    96,     0,    97,    98,    99,   100,   101,   102,    92,    93,
    94,    95,    96,     0,    97,    98,    99,   100,   101,   102,
    94,    95,    96,     0,    97,    98,    99,   100,   101,   102,
    97,    98,    99,   100,   101,   102
};

static const short yycheck[] = {    14,
    33,     0,    58,    30,    18,    58,    33,    23,    64,    62,
    25,    26,    27,    28,    29,    48,    31,    32,    11,    12,
    13,    48,    58,    16,    29,    23,    62,     3,     8,     9,
    10,    11,    12,     3,     4,     5,     6,     7,     8,     9,
    10,    11,    12,    13,    14,    15,    16,    17,    18,    18,
    20,    21,    22,    23,    24,    25,    26,    62,    28,    74,
    75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
    85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
    95,    96,   109,    53,    54,    61,    57,    58,    58,    59,
    60,    29,    29,    63,    29,   110,    18,     8,     9,    10,
    11,    12,   101,   112,   119,   114,    99,   100,    29,    18,
   103,     8,     9,    10,    11,    12,    18,     8,     9,    10,
    11,    12,    23,    23,    62,    62,    61,    49,    18,   144,
    10,   146,    49,    10,   149,    57,    58,   152,    49,    18,
    49,    21,    22,    23,    21,    22,    23,   180,    57,    58,
   165,   184,    49,   180,    61,    49,    62,   184,    49,    62,
   175,     3,     4,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    62,    20,    21,
    22,    23,    24,    25,    26,    58,    28,    44,    45,    46,
    47,    48,    49,    50,    51,    37,    53,    54,    55,    56,
    57,    58,    58,     0,     0,    49,    48,    49,   116,    17,
    -1,    53,    54,    57,    58,    -1,    58,    59,    60,    -1,
   116,    63,     3,     4,     5,     6,     7,     8,     9,    10,
    11,    12,    13,    14,    15,    16,    17,    18,    -1,    20,
    21,    22,    23,    24,    25,    26,    -1,    28,    10,    -1,
    -1,    13,    14,    15,    -1,    17,    37,    49,    20,    21,
    22,    23,    -1,    -1,    -1,    57,    58,    48,    49,    -1,
    62,    -1,    53,    54,    -1,    -1,    -1,    58,    59,    60,
    -1,    10,    63,    -1,    13,    14,    15,    -1,    17,    -1,
    -1,    20,    21,    22,    23,    -1,    -1,    27,    -1,    -1,
    62,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    51,    -1,    53,    54,    55,    56,    57,    58,    -1,
    -1,    -1,    27,    -1,    -1,    65,    31,    32,    33,    34,
    35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
    45,    46,    47,    48,    49,    50,    51,    -1,    53,    54,
    55,    56,    57,    58,    32,    33,    34,    35,    36,    37,
    38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
    48,    49,    50,    51,    -1,    53,    54,    55,    56,    57,
    58,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    -1,
    53,    54,    55,    56,    57,    58,    35,    36,    37,    38,
    39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    -1,    53,    54,    55,    56,    57,    58,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51,    -1,    53,    54,    55,
    56,    57,    58,    37,    38,    39,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    -1,    53,
    54,    55,    56,    57,    58,    38,    39,    40,    41,    42,
    43,    44,    45,    46,    47,    48,    49,    50,    51,    -1,
    53,    54,    55,    56,    57,    58,    40,    41,    42,    43,
    44,    45,    46,    47,    48,    49,    50,    51,    -1,    53,
    54,    55,    56,    57,    58,    46,    47,    48,    49,    50,
    51,    -1,    53,    54,    55,    56,    57,    58,    47,    48,
    49,    50,    51,    -1,    53,    54,    55,    56,    57,    58,
    49,    50,    51,    -1,    53,    54,    55,    56,    57,    58,
    53,    54,    55,    56,    57,    58
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__)
#include <alloca.h>
#endif

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
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab

#define YYTERROR	1
#define YYERRCODE	256

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

int yynerrs;			/*  number of parse errors so far       */
#endif  /* YYIMPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYMAXDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYMAXDEPTH
#define YYMAXDEPTH 200
#endif

/*  YYMAXLIMIT is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#ifndef YYMAXLIMIT
#define YYMAXLIMIT 10000
#endif


#line 90 "bison.simple"
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
  int yynerrs;
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
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
      yyvs = (YYSTYPE *) alloca (yymaxdepth * sizeof (*yyvsp));
      bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yymaxdepth * sizeof (*yylsp));
      bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yymaxdepth);
#endif

      if (yyssp >= yyss + yymaxdepth - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

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
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Next token is %d (%s)\n", yychar, yytname[yychar1]);
#endif
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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

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

#if YYDEBUG != 0
  if (yydebug)
    {
      if (yylen == 1)
	fprintf (stderr, "Reducing 1 value via line %d, ",
		 yyrline[yyn]);
      else
	fprintf (stderr, "Reducing %d values via line %d, ",
		 yylen, yyrline[yyn]);
    }
#endif


  switch (yyn) {

case 3:
#line 224 "../expread.y"
{ write_exp_elt_opcode (BINOP_COMMA); ;
    break;}
case 4:
#line 229 "../expread.y"
{ write_exp_elt_opcode (UNOP_IND); ;
    break;}
case 5:
#line 232 "../expread.y"
{ write_exp_elt_opcode (UNOP_ADDR); ;
    break;}
case 6:
#line 235 "../expread.y"
{ write_exp_elt_opcode (UNOP_NEG); ;
    break;}
case 7:
#line 239 "../expread.y"
{ write_exp_elt_opcode (UNOP_ZEROP); ;
    break;}
case 8:
#line 243 "../expread.y"
{ write_exp_elt_opcode (UNOP_LOGNOT); ;
    break;}
case 9:
#line 247 "../expread.y"
{ write_exp_elt_opcode (UNOP_PREINCREMENT); ;
    break;}
case 10:
#line 251 "../expread.y"
{ write_exp_elt_opcode (UNOP_PREDECREMENT); ;
    break;}
case 11:
#line 255 "../expread.y"
{ write_exp_elt_opcode (UNOP_POSTINCREMENT); ;
    break;}
case 12:
#line 259 "../expread.y"
{ write_exp_elt_opcode (UNOP_POSTDECREMENT); ;
    break;}
case 13:
#line 263 "../expread.y"
{ write_exp_elt_opcode (UNOP_SIZEOF); ;
    break;}
case 14:
#line 267 "../expread.y"
{ write_exp_elt_opcode (STRUCTOP_PTR);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt_opcode (STRUCTOP_PTR); ;
    break;}
case 15:
#line 273 "../expread.y"
{ write_exp_elt_opcode (STRUCTOP_MPTR); ;
    break;}
case 16:
#line 277 "../expread.y"
{ write_exp_elt_opcode (STRUCTOP_STRUCT);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt_opcode (STRUCTOP_STRUCT); ;
    break;}
case 17:
#line 283 "../expread.y"
{ write_exp_elt_opcode (STRUCTOP_MEMBER); ;
    break;}
case 18:
#line 287 "../expread.y"
{ write_exp_elt_opcode (BINOP_SUBSCRIPT); ;
    break;}
case 19:
#line 293 "../expread.y"
{ start_arglist (); ;
    break;}
case 20:
#line 295 "../expread.y"
{ write_exp_elt_opcode (OP_FUNCALL);
			  write_exp_elt_longcst ((LONGEST) end_arglist ());
			  write_exp_elt_opcode (OP_FUNCALL); ;
    break;}
case 22:
#line 304 "../expread.y"
{ arglist_len = 1; ;
    break;}
case 23:
#line 308 "../expread.y"
{ arglist_len++; ;
    break;}
case 24:
#line 312 "../expread.y"
{ write_exp_elt_opcode (UNOP_MEMVAL);
			  write_exp_elt_type (yyvsp[-2].tval);
			  write_exp_elt_opcode (UNOP_MEMVAL); ;
    break;}
case 25:
#line 318 "../expread.y"
{ write_exp_elt_opcode (UNOP_CAST);
			  write_exp_elt_type (yyvsp[-2].tval);
			  write_exp_elt_opcode (UNOP_CAST); ;
    break;}
case 26:
#line 324 "../expread.y"
{ ;
    break;}
case 27:
#line 330 "../expread.y"
{ write_exp_elt_opcode (BINOP_REPEAT); ;
    break;}
case 28:
#line 334 "../expread.y"
{ write_exp_elt_opcode (BINOP_MUL); ;
    break;}
case 29:
#line 338 "../expread.y"
{ write_exp_elt_opcode (BINOP_DIV); ;
    break;}
case 30:
#line 342 "../expread.y"
{ write_exp_elt_opcode (BINOP_REM); ;
    break;}
case 31:
#line 346 "../expread.y"
{ write_exp_elt_opcode (BINOP_ADD); ;
    break;}
case 32:
#line 350 "../expread.y"
{ write_exp_elt_opcode (BINOP_SUB); ;
    break;}
case 33:
#line 354 "../expread.y"
{ write_exp_elt_opcode (BINOP_LSH); ;
    break;}
case 34:
#line 358 "../expread.y"
{ write_exp_elt_opcode (BINOP_RSH); ;
    break;}
case 35:
#line 362 "../expread.y"
{ write_exp_elt_opcode (BINOP_EQUAL); ;
    break;}
case 36:
#line 366 "../expread.y"
{ write_exp_elt_opcode (BINOP_NOTEQUAL); ;
    break;}
case 37:
#line 370 "../expread.y"
{ write_exp_elt_opcode (BINOP_LEQ); ;
    break;}
case 38:
#line 374 "../expread.y"
{ write_exp_elt_opcode (BINOP_GEQ); ;
    break;}
case 39:
#line 378 "../expread.y"
{ write_exp_elt_opcode (BINOP_LESS); ;
    break;}
case 40:
#line 382 "../expread.y"
{ write_exp_elt_opcode (BINOP_GTR); ;
    break;}
case 41:
#line 386 "../expread.y"
{ write_exp_elt_opcode (BINOP_LOGAND); ;
    break;}
case 42:
#line 390 "../expread.y"
{ write_exp_elt_opcode (BINOP_LOGXOR); ;
    break;}
case 43:
#line 394 "../expread.y"
{ write_exp_elt_opcode (BINOP_LOGIOR); ;
    break;}
case 44:
#line 398 "../expread.y"
{ write_exp_elt_opcode (BINOP_AND); ;
    break;}
case 45:
#line 402 "../expread.y"
{ write_exp_elt_opcode (BINOP_OR); ;
    break;}
case 46:
#line 406 "../expread.y"
{ write_exp_elt_opcode (TERNOP_COND); ;
    break;}
case 47:
#line 410 "../expread.y"
{ write_exp_elt_opcode (BINOP_ASSIGN); ;
    break;}
case 48:
#line 414 "../expread.y"
{ write_exp_elt_opcode (BINOP_ASSIGN_MODIFY);
			  write_exp_elt_opcode (yyvsp[-1].opcode);
			  write_exp_elt_opcode (BINOP_ASSIGN_MODIFY); ;
    break;}
case 49:
#line 420 "../expread.y"
{ write_exp_elt_opcode (OP_LONG);
			  if (yyvsp[0].lval == (int) yyvsp[0].lval || yyvsp[0].lval == (unsigned int) yyvsp[0].lval)
			    write_exp_elt_type (builtin_type_int);
			  else
			    write_exp_elt_type (BUILTIN_TYPE_LONGEST);
			  write_exp_elt_longcst ((LONGEST) yyvsp[0].lval);
			  write_exp_elt_opcode (OP_LONG); ;
    break;}
case 50:
#line 430 "../expread.y"
{ YYSTYPE val;
			  parse_number (yyvsp[0].ssym.stoken.ptr, yyvsp[0].ssym.stoken.length, 0, &val);
			  write_exp_elt_opcode (OP_LONG);
			  if (val.lval == (int) val.lval ||
			      val.lval == (unsigned int) val.lval)
			    write_exp_elt_type (builtin_type_int);
			  else
			    write_exp_elt_type (BUILTIN_TYPE_LONGEST);
			  write_exp_elt_longcst (val.lval);
			  write_exp_elt_opcode (OP_LONG); ;
    break;}
case 51:
#line 443 "../expread.y"
{
			  write_exp_elt_opcode (OP_LONG);
			  if (yyvsp[0].ulval == (unsigned int) yyvsp[0].ulval)
			    write_exp_elt_type (builtin_type_unsigned_int);
			  else
			    write_exp_elt_type (BUILTIN_TYPE_UNSIGNED_LONGEST);
			  write_exp_elt_longcst ((LONGEST) yyvsp[0].ulval);
			  write_exp_elt_opcode (OP_LONG);
			;
    break;}
case 52:
#line 455 "../expread.y"
{ YYSTYPE val;
			  parse_number (yyvsp[0].ssym.stoken.ptr, yyvsp[0].ssym.stoken.length, 0, &val);
			  write_exp_elt_opcode (OP_LONG);
			  if (val.ulval == (unsigned int) val.ulval)
			    write_exp_elt_type (builtin_type_unsigned_int);
			  else
			    write_exp_elt_type (BUILTIN_TYPE_UNSIGNED_LONGEST);
			  write_exp_elt_longcst ((LONGEST)val.ulval);
			  write_exp_elt_opcode (OP_LONG);
			;
    break;}
case 53:
#line 468 "../expread.y"
{ write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type (builtin_type_char);
			  write_exp_elt_longcst ((LONGEST) yyvsp[0].lval);
			  write_exp_elt_opcode (OP_LONG); ;
    break;}
case 54:
#line 475 "../expread.y"
{ write_exp_elt_opcode (OP_DOUBLE);
			  write_exp_elt_type (builtin_type_double);
			  write_exp_elt_dblcst (yyvsp[0].dval);
			  write_exp_elt_opcode (OP_DOUBLE); ;
    break;}
case 56:
#line 485 "../expread.y"
{ write_exp_elt_opcode (OP_LAST);
			  write_exp_elt_longcst ((LONGEST) yyvsp[0].lval);
			  write_exp_elt_opcode (OP_LAST); ;
    break;}
case 57:
#line 491 "../expread.y"
{ write_exp_elt_opcode (OP_REGISTER);
			  write_exp_elt_longcst ((LONGEST) yyvsp[0].lval);
			  write_exp_elt_opcode (OP_REGISTER); ;
    break;}
case 58:
#line 497 "../expread.y"
{ write_exp_elt_opcode (OP_INTERNALVAR);
			  write_exp_elt_intern (yyvsp[0].ivar);
			  write_exp_elt_opcode (OP_INTERNALVAR); ;
    break;}
case 59:
#line 503 "../expread.y"
{ write_exp_elt_opcode (OP_LONG);
			  write_exp_elt_type (builtin_type_int);
			  write_exp_elt_longcst ((LONGEST) TYPE_LENGTH (yyvsp[-1].tval));
			  write_exp_elt_opcode (OP_LONG); ;
    break;}
case 60:
#line 510 "../expread.y"
{ write_exp_elt_opcode (OP_STRING);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt_opcode (OP_STRING); ;
    break;}
case 61:
#line 517 "../expread.y"
{ write_exp_elt_opcode (OP_THIS);
			  write_exp_elt_opcode (OP_THIS); ;
    break;}
case 62:
#line 524 "../expread.y"
{
			  if (yyvsp[0].ssym.sym != 0)
			      yyval.bval = SYMBOL_BLOCK_VALUE (yyvsp[0].ssym.sym);
			  else
			    {
			      struct symtab *tem =
				  lookup_symtab (copy_name (yyvsp[0].ssym.stoken));
			      if (tem)
				yyval.bval = BLOCKVECTOR_BLOCK (BLOCKVECTOR (tem), 1);
			      else
				error ("No file or function \"%s\".",
				       copy_name (yyvsp[0].ssym.stoken));
			    }
			;
    break;}
case 63:
#line 541 "../expread.y"
{ struct symbol *tem
			    = lookup_symbol (copy_name (yyvsp[0].sval), yyvsp[-2].bval,
					     VAR_NAMESPACE, 0, NULL);
			  if (!tem || SYMBOL_CLASS (tem) != LOC_BLOCK)
			    error ("No function \"%s\" in specified context.",
				   copy_name (yyvsp[0].sval));
			  yyval.bval = SYMBOL_BLOCK_VALUE (tem); ;
    break;}
case 64:
#line 551 "../expread.y"
{ struct symbol *sym;
			  sym = lookup_symbol (copy_name (yyvsp[0].sval), yyvsp[-2].bval,
					       VAR_NAMESPACE, 0, NULL);
			  if (sym == 0)
			    error ("No symbol \"%s\" in specified context.",
				   copy_name (yyvsp[0].sval));
			  write_exp_elt_opcode (OP_VAR_VALUE);
			  write_exp_elt_sym (sym);
			  write_exp_elt_opcode (OP_VAR_VALUE); ;
    break;}
case 65:
#line 563 "../expread.y"
{
			  struct type *type = yyvsp[-2].tval;
			  if (TYPE_CODE (type) != TYPE_CODE_STRUCT
			      && TYPE_CODE (type) != TYPE_CODE_UNION)
			    error ("`%s' is not defined as an aggregate type.",
				   TYPE_NAME (type));

			  write_exp_elt_opcode (OP_SCOPE);
			  write_exp_elt_type (type);
			  write_exp_string (yyvsp[0].sval);
			  write_exp_elt_opcode (OP_SCOPE);
			;
    break;}
case 66:
#line 576 "../expread.y"
{
			  char *name = copy_name (yyvsp[0].sval);
			  struct symbol *sym;
			  int i;

			  sym =
			    lookup_symbol (name, 0, VAR_NAMESPACE, 0, NULL);
			  if (sym)
			    {
			      write_exp_elt_opcode (OP_VAR_VALUE);
			      write_exp_elt_sym (sym);
			      write_exp_elt_opcode (OP_VAR_VALUE);
			      break;
			    }
			  for (i = 0; i < misc_function_count; i++)
			    if (!strcmp (misc_function_vector[i].name, name))
			      break;

			  if (i < misc_function_count)
			    {
			      enum misc_function_type mft =
				  misc_function_vector[i].type;
			      
			      write_exp_elt_opcode (OP_LONG);
			      write_exp_elt_type (builtin_type_int);
			      write_exp_elt_longcst ((LONGEST) misc_function_vector[i].address);
			      write_exp_elt_opcode (OP_LONG);
			      write_exp_elt_opcode (UNOP_MEMVAL);
			      if (mft == mf_data || mft == mf_bss)
				write_exp_elt_type (builtin_type_int);
			      else if (mft == mf_text)
				write_exp_elt_type (lookup_function_type (builtin_type_int));
			      else
				write_exp_elt_type (builtin_type_char);
			      write_exp_elt_opcode (UNOP_MEMVAL);
			    }
			  else
			    if (symtab_list == 0
				&& partial_symtab_list == 0)
			      error ("No symbol table is loaded.  Use the \"file\" command.");
			    else
			      error ("No symbol \"%s\" in current context.", name);
			;
    break;}
case 67:
#line 622 "../expread.y"
{ struct symbol *sym = yyvsp[0].ssym.sym;

			  if (sym)
			    {
			      switch (sym->class)
				{
				case LOC_REGISTER:
				case LOC_ARG:
				case LOC_LOCAL:
				case LOC_LOCAL_ARG:
				  if (innermost_block == 0 ||
				      contained_in (block_found, 
						    innermost_block))
				    innermost_block = block_found;
				}
			      write_exp_elt_opcode (OP_VAR_VALUE);
			      write_exp_elt_sym (sym);
			      write_exp_elt_opcode (OP_VAR_VALUE);
			    }
			  else if (yyvsp[0].ssym.is_a_field_of_this)
			    {
			      /* C++: it hangs off of `this'.  Must
			         not inadvertently convert from a method call
				 to data ref.  */
			      if (innermost_block == 0 || 
				  contained_in (block_found, innermost_block))
				innermost_block = block_found;
			      write_exp_elt_opcode (OP_THIS);
			      write_exp_elt_opcode (OP_THIS);
			      write_exp_elt_opcode (STRUCTOP_PTR);
			      write_exp_string (yyvsp[0].ssym.stoken);
			      write_exp_elt_opcode (STRUCTOP_PTR);
			    }
			  else
			    {
			      register int i;
			      register char *arg = copy_name (yyvsp[0].ssym.stoken);

				/* FIXME, this search is linear!  At least
				   optimize the strcmp with a 1-char cmp... */
			      for (i = 0; i < misc_function_count; i++)
				if (!strcmp (misc_function_vector[i].name, arg))
				  break;

			      if (i < misc_function_count)
				{
				  enum misc_function_type mft =
				      misc_function_vector[i].type;
				  
				  write_exp_elt_opcode (OP_LONG);
				  write_exp_elt_type (builtin_type_int);
				  write_exp_elt_longcst ((LONGEST) misc_function_vector[i].address);
				  write_exp_elt_opcode (OP_LONG);
				  write_exp_elt_opcode (UNOP_MEMVAL);
				  if (mft == mf_data || mft == mf_bss)
				    write_exp_elt_type (builtin_type_int);
				  else if (mft == mf_text)
				    write_exp_elt_type (lookup_function_type (builtin_type_int));
				  else
				    write_exp_elt_type (builtin_type_char);
				  write_exp_elt_opcode (UNOP_MEMVAL);
				}
			      else if (symtab_list == 0
				       && partial_symtab_list == 0)
				error ("No symbol table is loaded.  Use the \"file\" command.");
			      else
				error ("No symbol \"%s\" in current context.",
				       copy_name (yyvsp[0].ssym.stoken));
			    }
			;
    break;}
case 69:
#line 697 "../expread.y"
{
		  /* This is where the interesting stuff happens.  */
		  int done = 0;
		  int array_size;
		  struct type *follow_type = yyvsp[-1].tval;
		  
		  while (!done)
		    switch (pop_type ())
		      {
		      case tp_end:
			done = 1;
			break;
		      case tp_pointer:
			follow_type = lookup_pointer_type (follow_type);
			break;
		      case tp_reference:
			follow_type = lookup_reference_type (follow_type);
			break;
		      case tp_array:
			array_size = (int) pop_type ();
			if (array_size != -1)
			  follow_type = create_array_type (follow_type,
							   array_size);
			else
			  follow_type = lookup_pointer_type (follow_type);
			break;
		      case tp_function:
			follow_type = lookup_function_type (follow_type);
			break;
		      }
		  yyval.tval = follow_type;
		;
    break;}
case 70:
#line 732 "../expread.y"
{ push_type (tp_pointer); yyval.voidval = 0; ;
    break;}
case 71:
#line 734 "../expread.y"
{ push_type (tp_pointer); yyval.voidval = yyvsp[0].voidval; ;
    break;}
case 73:
#line 739 "../expread.y"
{ yyval.voidval = yyvsp[-1].voidval; ;
    break;}
case 74:
#line 741 "../expread.y"
{
			  push_type ((enum type_pieces) yyvsp[0].lval);
			  push_type (tp_array);
			;
    break;}
case 75:
#line 746 "../expread.y"
{
			  push_type ((enum type_pieces) yyvsp[0].lval);
			  push_type (tp_array);
			  yyval.voidval = 0;
			;
    break;}
case 76:
#line 752 "../expread.y"
{ push_type (tp_function); ;
    break;}
case 77:
#line 754 "../expread.y"
{ push_type (tp_function); ;
    break;}
case 78:
#line 758 "../expread.y"
{ yyval.lval = -1; ;
    break;}
case 79:
#line 760 "../expread.y"
{ yyval.lval = yyvsp[-1].lval; ;
    break;}
case 80:
#line 764 "../expread.y"
{ yyval.voidval = 0; ;
    break;}
case 82:
#line 769 "../expread.y"
{ yyval.tval = lookup_member_type (builtin_type_int, yyvsp[-2].tval); ;
    break;}
case 83:
#line 771 "../expread.y"
{ yyval.tval = lookup_member_type (yyvsp[-5].tval, yyvsp[-3].tval); ;
    break;}
case 84:
#line 773 "../expread.y"
{ yyval.tval = lookup_member_type
			    (lookup_function_type (yyvsp[-7].tval), yyvsp[-5].tval); ;
    break;}
case 85:
#line 776 "../expread.y"
{ yyval.tval = lookup_member_type
			    (lookup_function_type (yyvsp[-8].tval), yyvsp[-6].tval);
			  free (yyvsp[-1].tvec); ;
    break;}
case 86:
#line 783 "../expread.y"
{ yyval.tval = yyvsp[0].tsym.type; ;
    break;}
case 87:
#line 785 "../expread.y"
{ yyval.tval = builtin_type_int; ;
    break;}
case 88:
#line 787 "../expread.y"
{ yyval.tval = builtin_type_long; ;
    break;}
case 89:
#line 789 "../expread.y"
{ yyval.tval = builtin_type_short; ;
    break;}
case 90:
#line 791 "../expread.y"
{ yyval.tval = builtin_type_long; ;
    break;}
case 91:
#line 793 "../expread.y"
{ yyval.tval = builtin_type_unsigned_long; ;
    break;}
case 92:
#line 795 "../expread.y"
{ yyval.tval = builtin_type_short; ;
    break;}
case 93:
#line 797 "../expread.y"
{ yyval.tval = builtin_type_unsigned_short; ;
    break;}
case 94:
#line 799 "../expread.y"
{ yyval.tval = lookup_struct (copy_name (yyvsp[0].sval),
					      expression_context_block); ;
    break;}
case 95:
#line 802 "../expread.y"
{ yyval.tval = lookup_union (copy_name (yyvsp[0].sval),
					     expression_context_block); ;
    break;}
case 96:
#line 805 "../expread.y"
{ yyval.tval = lookup_enum (copy_name (yyvsp[0].sval),
					    expression_context_block); ;
    break;}
case 97:
#line 808 "../expread.y"
{ yyval.tval = lookup_unsigned_typename (TYPE_NAME(yyvsp[0].tsym.type)); ;
    break;}
case 98:
#line 810 "../expread.y"
{ yyval.tval = builtin_type_unsigned_int; ;
    break;}
case 99:
#line 812 "../expread.y"
{ yyval.tval = yyvsp[0].tsym.type; ;
    break;}
case 100:
#line 814 "../expread.y"
{ yyval.tval = builtin_type_int; ;
    break;}
case 102:
#line 819 "../expread.y"
{
		  yyval.tsym.stoken.ptr = "int";
		  yyval.tsym.stoken.length = 3;
		  yyval.tsym.type = builtin_type_int;
		;
    break;}
case 103:
#line 825 "../expread.y"
{
		  yyval.tsym.stoken.ptr = "long";
		  yyval.tsym.stoken.length = 4;
		  yyval.tsym.type = builtin_type_long;
		;
    break;}
case 104:
#line 831 "../expread.y"
{
		  yyval.tsym.stoken.ptr = "short";
		  yyval.tsym.stoken.length = 5;
		  yyval.tsym.type = builtin_type_short;
		;
    break;}
case 105:
#line 840 "../expread.y"
{ yyval.tvec = (struct type **)xmalloc (sizeof (struct type *) * 2);
		  yyval.tvec[0] = (struct type *)0;
		  yyval.tvec[1] = yyvsp[0].tval;
		;
    break;}
case 106:
#line 845 "../expread.y"
{ int len = sizeof (struct type *) * ++(yyvsp[-2].ivec[0]);
		  yyval.tvec = (struct type **)xrealloc (yyvsp[-2].tvec, len);
		  yyval.tvec[yyval.ivec[0]] = yyvsp[0].tval;
		;
    break;}
case 107:
#line 851 "../expread.y"
{ yyval.sval = yyvsp[0].ssym.stoken; ;
    break;}
case 108:
#line 852 "../expread.y"
{ yyval.sval = yyvsp[0].ssym.stoken; ;
    break;}
case 109:
#line 853 "../expread.y"
{ yyval.sval = yyvsp[0].tsym.stoken; ;
    break;}
case 110:
#line 854 "../expread.y"
{ yyval.sval = yyvsp[0].ssym.stoken; ;
    break;}
case 111:
#line 855 "../expread.y"
{ yyval.sval = yyvsp[0].ssym.stoken; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 327 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
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
#endif

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
      ++yynerrs;
      yyerror("parse error");
    }

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

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

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

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

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 864 "../expread.y"


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

/* To avoid a bug in the Sun 4 compiler, we pass things that can fit into
   a register through here */

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

static void
write_exp_elt_opcode (expelt)
     enum exp_opcode expelt;
{
  union exp_element tmp;

  tmp.opcode = expelt;

  write_exp_elt (tmp);
}

static void
write_exp_elt_sym (expelt)
     struct symbol *expelt;
{
  union exp_element tmp;

  tmp.symbol = expelt;

  write_exp_elt (tmp);
}

static void
write_exp_elt_longcst (expelt)
     LONGEST expelt;
{
  union exp_element tmp;

  tmp.longconst = expelt;

  write_exp_elt (tmp);
}

static void
write_exp_elt_dblcst (expelt)
     double expelt;
{
  union exp_element tmp;

  tmp.doubleconst = expelt;

  write_exp_elt (tmp);
}

static void
write_exp_elt_type (expelt)
     struct type *expelt;
{
  union exp_element tmp;

  tmp.type = expelt;

  write_exp_elt (tmp);
}

static void
write_exp_elt_intern (expelt)
     struct internalvar *expelt;
{
  union exp_element tmp;

  tmp.internalvar = expelt;

  write_exp_elt (tmp);
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
      expout = (struct expression *)
	xrealloc (expout, (sizeof (struct expression)
			   + (expout_size * sizeof (union exp_element))));
    }
  bcopy (str.ptr, (char *) &expout->elts[expout_ptr - lenelt], len);
  ((char *) &expout->elts[expout_ptr - lenelt])[len] = 0;
  write_exp_elt_longcst ((LONGEST) len);
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
parse_number (p, len, parsed_float, putithere)
     register char *p;
     register int len;
     int parsed_float;
     YYSTYPE *putithere;
{
  register LONGEST n = 0;
  register int i;
  register int c;
  register int base = input_radix;
  int unsigned_p = 0;

  extern double atof ();

  if (parsed_float)
    {
      /* It's a float since it contains a point or an exponent.  */
      putithere->dval = atof (p);
      return FLOAT;
    }

  /* Handle base-switching prefixes 0x, 0t, 0d, 0 */
  if (p[0] == '0')
    switch (p[1])
      {
      case 'x':
      case 'X':
	if (len >= 3)
	  {
	    p += 2;
	    base = 16;
	    len -= 2;
	  }
	break;

      case 't':
      case 'T':
      case 'd':
      case 'D':
	if (len >= 3)
	  {
	    p += 2;
	    base = 10;
	    len -= 2;
	  }
	break;

      default:
	base = 8;
	break;
      }

  while (len-- > 0)
    {
      c = *p++;
      if (c >= 'A' && c <= 'Z')
	c += 'a' - 'A';
      if (c != 'l' && c != 'u')
	n *= base;
      if (c >= '0' && c <= '9')
	n += i = c - '0';
      else
	{
	  if (base > 10 && c >= 'a' && c <= 'f')
	    n += i = c - 'a' + 10;
	  else if (len == 0 && c == 'l')
	    ;
	  else if (len == 0 && c == 'u')
	    unsigned_p = 1;
	  else
	    return ERROR;	/* Char not a digit */
	}
      if (i >= base)
	return ERROR;		/* Invalid digit in this base */
    }

  if (unsigned_p)
    {
      putithere->ulval = n;
      return UINT;
    }
  else
    {
      putithere->lval = n;
      return INT;
    }
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

/* assign machine-independent names to certain registers 
 * (unless overridden by the REGISTER_NAMES table)
 */
struct std_regs {
	char *name;
	int regnum;
} std_regs[] = {
#ifdef PC_REGNUM
	{ "pc", PC_REGNUM },
#endif
#ifdef FP_REGNUM
	{ "fp", FP_REGNUM },
#endif
#ifdef SP_REGNUM
	{ "sp", SP_REGNUM },
#endif
#ifdef PS_REGNUM
	{ "ps", PS_REGNUM },
#endif
};

#define NUM_STD_REGS (sizeof std_regs / sizeof std_regs[0])

/* Read one token, getting characters through lexptr.  */

static int
yylex ()
{
  register int c;
  register int namelen;
  register unsigned i;
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

    case '.':
      /* Might be a floating point number.  */
      if (lexptr[1] < '0' || lexptr[1] > '9')
	goto symbol;		/* Nope, must be a symbol. */
      /* FALL THRU into number case.  */

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      {
	/* It's a number.  */
	int got_dot = 0, got_e = 0, toktype;
	register char *p = tokstart;
	int hex = input_radix > 10;

	if (c == '0' && (p[1] == 'x' || p[1] == 'X'))
	  {
	    p += 2;
	    hex = 1;
	  }
	else if (c == '0' && (p[1]=='t' || p[1]=='T' || p[1]=='d' || p[1]=='D'))
	  {
	    p += 2;
	    hex = 0;
	  }

	for (;; ++p)
	  {
	    if (!hex && !got_e && (*p == 'e' || *p == 'E'))
	      got_dot = got_e = 1;
	    else if (!hex && !got_dot && *p == '.')
	      got_dot = 1;
	    else if (got_e && (p[-1] == 'e' || p[-1] == 'E')
		     && (*p == '-' || *p == '+'))
	      /* This is the sign of the exponent, not the end of the
		 number.  */
	      continue;
	    /* We will take any letters or digits.  parse_number will
	       complain if past the radix, or if L or U are not final.  */
	    else if ((*p < '0' || *p > '9')
		     && ((*p < 'a' || *p > 'z')
				  && (*p < 'A' || *p > 'Z')))
	      break;
	  }
	toktype = parse_number (tokstart, p - tokstart, got_dot|got_e, &yylval);
        if (toktype == ERROR)
	  {
	    char *err_copy = (char *) alloca (p - tokstart + 1);

	    bcopy (tokstart, err_copy, p - tokstart);
	    err_copy[p - tokstart] = 0;
	    error ("Invalid number \"%s\".", err_copy);
	  }
	lexptr = p;
	return toktype;
      }

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
    case '?':
    case ':':
    case '=':
    case '{':
    case '}':
    symbol:
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

  if (!(c == '_' || c == '$'
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
    /* We must have come across a bad character (e.g. ';').  */
    error ("Invalid character '%c' in expression.", c);

  /* It's a name.  See how long it is.  */
  namelen = 0;
  for (c = tokstart[namelen];
       (c == '_' || c == '$' || (c >= '0' && c <= '9')
	|| (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
       c = tokstart[++namelen])
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

  if (*tokstart == '$') {
    for (c = 0; c < NUM_REGS; c++)
      if (namelen - 1 == strlen (reg_names[c])
	  && !strncmp (tokstart + 1, reg_names[c], namelen - 1))
	{
	  yylval.lval = c;
	  return REGNAME;
	}
    for (c = 0; c < NUM_STD_REGS; c++)
     if (namelen - 1 == strlen (std_regs[c].name)
	 && !strncmp (tokstart + 1, std_regs[c].name, namelen - 1))
       {
	 yylval.lval = std_regs[c].regnum;
	 return REGNAME;
       }
  }
  /* Catch specific keywords.  Should be done with a data structure.  */
  switch (namelen)
    {
    case 8:
      if (!strncmp (tokstart, "unsigned", 8))
	return UNSIGNED;
      break;
    case 6:
      if (!strncmp (tokstart, "struct", 6))
	return STRUCT;
      if (!strncmp (tokstart, "signed", 6))
	return SIGNED;
      if (!strncmp (tokstart, "sizeof", 6))      
	return SIZEOF;
      break;
    case 5:
      if (!strncmp (tokstart, "union", 5))
	return UNION;
      if (!strncmp (tokstart, "short", 5))
	return SHORT;
      break;
    case 4:
      if (!strncmp (tokstart, "enum", 4))
	return ENUM;
      if (!strncmp (tokstart, "long", 4))
	return LONG;
      if (!strncmp (tokstart, "this", 4))
	{
	  static const char this_name[] =
				 { CPLUS_MARKER, 't', 'h', 'i', 's', '\0' };

	  if (lookup_symbol (this_name, expression_context_block,
			     VAR_NAMESPACE, 0, NULL))
	    return THIS;
	}
      break;
    case 3:
      if (!strncmp (tokstart, "int", 3))
	return INT_KEYWORD;
      break;
    default:
      break;
    }

  yylval.sval.ptr = tokstart;
  yylval.sval.length = namelen;

  /* Any other names starting in $ are debugger internal variables.  */

  if (*tokstart == '$')
    {
      yylval.ivar =  lookup_internalvar (copy_name (yylval.sval) + 1);
      return VARIABLE;
    }

  /* Use token-type BLOCKNAME for symbols that happen to be defined as
     functions or symtabs.  If this is not so, then ...
     Use token-type TYPENAME for symbols that happen to be defined
     currently as names of types; NAME for other symbols.
     The caller is not constrained to care about the distinction.  */
  {
    char *tmp = copy_name (yylval.sval);
    struct symbol *sym;
    int is_a_field_of_this = 0;
    int hextype;

    sym = lookup_symbol (tmp, expression_context_block,
			 VAR_NAMESPACE, &is_a_field_of_this, NULL);
    if ((sym && SYMBOL_CLASS (sym) == LOC_BLOCK) ||
        lookup_partial_symtab (tmp))
      {
	yylval.ssym.sym = sym;
	yylval.ssym.is_a_field_of_this = is_a_field_of_this;
	return BLOCKNAME;
      }
    if (sym && SYMBOL_CLASS (sym) == LOC_TYPEDEF)
        {
	  yylval.tsym.type = SYMBOL_TYPE (sym);
	  return TYPENAME;
        }
    if ((yylval.tsym.type = lookup_primitive_typename (tmp)) != 0)
	return TYPENAME;

    /* Input names that aren't symbols but ARE valid hex numbers,
       when the input radix permits them, can be names or numbers
       depending on the parse.  Note we support radixes > 16 here.  */
    if (!sym && 
        ((tokstart[0] >= 'a' && tokstart[0] < 'a' + input_radix - 10) ||
         (tokstart[0] >= 'A' && tokstart[0] < 'A' + input_radix - 10)))
      {
 	YYSTYPE newlval;	/* Its value is ignored.  */
	hextype = parse_number (tokstart, namelen, 0, &newlval);
	if (hextype == INT)
	  {
	    yylval.ssym.sym = sym;
	    yylval.ssym.is_a_field_of_this = is_a_field_of_this;
	    return NAME_OR_INT;
	  }
	if (hextype == UINT)
	  {
	    yylval.ssym.sym = sym;
	    yylval.ssym.is_a_field_of_this = is_a_field_of_this;
	    return NAME_OR_UINT;
	  }
      }

    /* Any other kind of symbol */
    yylval.ssym.sym = sym;
    yylval.ssym.is_a_field_of_this = is_a_field_of_this;
    return NAME;
  }
}

static void
yyerror (msg)
     char *msg;
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
     int comma;
{
  struct cleanup *old_chain;

  lexptr = *stringptr;

  paren_depth = 0;
  type_stack_depth = 0;

  comma_terminates = comma;

  if (lexptr == 0 || *lexptr == 0)
    error_no_arg ("expression to compute");

  old_chain = make_cleanup (free_funcalls, 0);
  funcall_chain = 0;

  expression_context_block = block ? block : get_selected_block ();

  namecopy = (char *) alloca (strlen (lexptr) + 1);
  expout_size = 10;
  expout_ptr = 0;
  expout = (struct expression *)
    xmalloc (sizeof (struct expression)
	     + expout_size * sizeof (union exp_element));
  make_cleanup (free_current_contents, &expout);
  if (yyparse ())
    yyerror (NULL);
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

static void 
push_type (tp)
     enum type_pieces tp;
{
  if (type_stack_depth == type_stack_size)
    {
      type_stack_size *= 2;
      type_stack = (enum type_pieces *)
	xrealloc (type_stack, type_stack_size * sizeof (enum type_pieces));
    }
  type_stack[type_stack_depth++] = tp;
}

static enum type_pieces 
pop_type ()
{
  if (type_stack_depth)
    return type_stack[--type_stack_depth];
  return tp_end;
}

void
_initialize_expread ()
{
  type_stack_size = 80;
  type_stack_depth = 0;
  type_stack = (enum type_pieces *)
    xmalloc (type_stack_size * sizeof (enum type_pieces));
}
