/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     NAME = 259,
     LNAME = 260,
     OREQ = 261,
     ANDEQ = 262,
     RSHIFTEQ = 263,
     LSHIFTEQ = 264,
     DIVEQ = 265,
     MULTEQ = 266,
     MINUSEQ = 267,
     PLUSEQ = 268,
     OROR = 269,
     ANDAND = 270,
     NE = 271,
     EQ = 272,
     GE = 273,
     LE = 274,
     RSHIFT = 275,
     LSHIFT = 276,
     UNARY = 277,
     END = 278,
     ALIGN_K = 279,
     BLOCK = 280,
     BIND = 281,
     QUAD = 282,
     SQUAD = 283,
     LONG = 284,
     SHORT = 285,
     BYTE = 286,
     SECTIONS = 287,
     PHDRS = 288,
     DATA_SEGMENT_ALIGN = 289,
     DATA_SEGMENT_RELRO_END = 290,
     DATA_SEGMENT_END = 291,
     SORT_BY_NAME = 292,
     SORT_BY_ALIGNMENT = 293,
     SIZEOF_HEADERS = 294,
     OUTPUT_FORMAT = 295,
     FORCE_COMMON_ALLOCATION = 296,
     OUTPUT_ARCH = 297,
     INHIBIT_COMMON_ALLOCATION = 298,
     SEGMENT_START = 299,
     INCLUDE = 300,
     MEMORY = 301,
     NOLOAD = 302,
     DSECT = 303,
     COPY = 304,
     INFO = 305,
     OVERLAY = 306,
     DEFINED = 307,
     TARGET_K = 308,
     SEARCH_DIR = 309,
     MAP = 310,
     ENTRY = 311,
     NEXT = 312,
     SIZEOF = 313,
     ALIGNOF = 314,
     ADDR = 315,
     LOADADDR = 316,
     MAX_K = 317,
     MIN_K = 318,
     STARTUP = 319,
     HLL = 320,
     SYSLIB = 321,
     FLOAT = 322,
     NOFLOAT = 323,
     NOCROSSREFS = 324,
     ORIGIN = 325,
     FILL = 326,
     LENGTH = 327,
     CREATE_OBJECT_SYMBOLS = 328,
     INPUT = 329,
     GROUP = 330,
     OUTPUT = 331,
     CONSTRUCTORS = 332,
     ALIGNMOD = 333,
     AT = 334,
     SUBALIGN = 335,
     PROVIDE = 336,
     PROVIDE_HIDDEN = 337,
     AS_NEEDED = 338,
     CHIP = 339,
     LIST = 340,
     SECT = 341,
     ABSOLUTE = 342,
     LOAD = 343,
     NEWLINE = 344,
     ENDWORD = 345,
     ORDER = 346,
     NAMEWORD = 347,
     ASSERT_K = 348,
     FORMAT = 349,
     PUBLIC = 350,
     DEFSYMEND = 351,
     BASE = 352,
     ALIAS = 353,
     TRUNCATE = 354,
     REL = 355,
     INPUT_SCRIPT = 356,
     INPUT_MRI_SCRIPT = 357,
     INPUT_DEFSYM = 358,
     CASE = 359,
     EXTERN = 360,
     START = 361,
     VERS_TAG = 362,
     VERS_IDENTIFIER = 363,
     GLOBAL = 364,
     LOCAL = 365,
     VERSIONK = 366,
     INPUT_VERSION_SCRIPT = 367,
     KEEP = 368,
     ONLY_IF_RO = 369,
     ONLY_IF_RW = 370,
     SPECIAL = 371,
     EXCLUDE_FILE = 372,
     CONSTANT = 373,
     INPUT_DYNAMIC_LIST = 374
   };
#endif
/* Tokens.  */
#define INT 258
#define NAME 259
#define LNAME 260
#define OREQ 261
#define ANDEQ 262
#define RSHIFTEQ 263
#define LSHIFTEQ 264
#define DIVEQ 265
#define MULTEQ 266
#define MINUSEQ 267
#define PLUSEQ 268
#define OROR 269
#define ANDAND 270
#define NE 271
#define EQ 272
#define GE 273
#define LE 274
#define RSHIFT 275
#define LSHIFT 276
#define UNARY 277
#define END 278
#define ALIGN_K 279
#define BLOCK 280
#define BIND 281
#define QUAD 282
#define SQUAD 283
#define LONG 284
#define SHORT 285
#define BYTE 286
#define SECTIONS 287
#define PHDRS 288
#define DATA_SEGMENT_ALIGN 289
#define DATA_SEGMENT_RELRO_END 290
#define DATA_SEGMENT_END 291
#define SORT_BY_NAME 292
#define SORT_BY_ALIGNMENT 293
#define SIZEOF_HEADERS 294
#define OUTPUT_FORMAT 295
#define FORCE_COMMON_ALLOCATION 296
#define OUTPUT_ARCH 297
#define INHIBIT_COMMON_ALLOCATION 298
#define SEGMENT_START 299
#define INCLUDE 300
#define MEMORY 301
#define NOLOAD 302
#define DSECT 303
#define COPY 304
#define INFO 305
#define OVERLAY 306
#define DEFINED 307
#define TARGET_K 308
#define SEARCH_DIR 309
#define MAP 310
#define ENTRY 311
#define NEXT 312
#define SIZEOF 313
#define ALIGNOF 314
#define ADDR 315
#define LOADADDR 316
#define MAX_K 317
#define MIN_K 318
#define STARTUP 319
#define HLL 320
#define SYSLIB 321
#define FLOAT 322
#define NOFLOAT 323
#define NOCROSSREFS 324
#define ORIGIN 325
#define FILL 326
#define LENGTH 327
#define CREATE_OBJECT_SYMBOLS 328
#define INPUT 329
#define GROUP 330
#define OUTPUT 331
#define CONSTRUCTORS 332
#define ALIGNMOD 333
#define AT 334
#define SUBALIGN 335
#define PROVIDE 336
#define PROVIDE_HIDDEN 337
#define AS_NEEDED 338
#define CHIP 339
#define LIST 340
#define SECT 341
#define ABSOLUTE 342
#define LOAD 343
#define NEWLINE 344
#define ENDWORD 345
#define ORDER 346
#define NAMEWORD 347
#define ASSERT_K 348
#define FORMAT 349
#define PUBLIC 350
#define DEFSYMEND 351
#define BASE 352
#define ALIAS 353
#define TRUNCATE 354
#define REL 355
#define INPUT_SCRIPT 356
#define INPUT_MRI_SCRIPT 357
#define INPUT_DEFSYM 358
#define CASE 359
#define EXTERN 360
#define START 361
#define VERS_TAG 362
#define VERS_IDENTIFIER 363
#define GLOBAL 364
#define LOCAL 365
#define VERSIONK 366
#define INPUT_VERSION_SCRIPT 367
#define KEEP 368
#define ONLY_IF_RO 369
#define ONLY_IF_RW 370
#define SPECIAL 371
#define EXCLUDE_FILE 372
#define CONSTANT 373
#define INPUT_DYNAMIC_LIST 374




/* Copy the first part of user declarations.  */
#line 23 "ldgram.y"

/*

 */

#define DONTDECLARE_MALLOC

#include "sysdep.h"
#include "bfd.h"
#include "bfdlink.h"
#include "ld.h"
#include "ldexp.h"
#include "ldver.h"
#include "ldlang.h"
#include "ldfile.h"
#include "ldemul.h"
#include "ldmisc.h"
#include "ldmain.h"
#include "mri.h"
#include "ldctor.h"
#include "ldlex.h"

#ifndef YYDEBUG
#define YYDEBUG 1
#endif

static enum section_type sectype;
static lang_memory_region_type *region;

FILE *saved_script_handle = NULL;
bfd_boolean force_make_executable = FALSE;

bfd_boolean ldgram_in_script = FALSE;
bfd_boolean ldgram_had_equals = FALSE;
bfd_boolean ldgram_had_keep = FALSE;
char *ldgram_vers_current_lang = NULL;

#define ERROR_NAME_MAX 20
static char *error_names[ERROR_NAME_MAX];
static int error_index;
#define PUSH_ERROR(x) if (error_index < ERROR_NAME_MAX) error_names[error_index] = x; error_index++;
#define POP_ERROR()   error_index--;


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 66 "ldgram.y"
{
  bfd_vma integer;
  struct big_int
    {
      bfd_vma integer;
      char *str;
    } bigint;
  fill_type *fill;
  char *name;
  const char *cname;
  struct wildcard_spec wildcard;
  struct wildcard_list *wildcard_list;
  struct name_list *name_list;
  int token;
  union etree_union *etree;
  struct phdr_info
    {
      bfd_boolean filehdr;
      bfd_boolean phdrs;
      union etree_union *at;
      union etree_union *flags;
    } phdr;
  struct lang_nocrossref *nocrossref;
  struct lang_output_section_phdr_list *section_phdr;
  struct bfd_elf_version_deps *deflist;
  struct bfd_elf_version_expr *versyms;
  struct bfd_elf_version_tree *versnode;
}
/* Line 187 of yacc.c.  */
#line 407 "ldgram.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 420 "ldgram.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  17
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1716

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  143
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  122
/* YYNRULES -- Number of rules.  */
#define YYNRULES  346
/* YYNRULES -- Number of states.  */
#define YYNSTATES  737

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   374

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   141,     2,     2,     2,    34,    21,     2,
      37,   138,    32,    30,   136,    31,     2,    33,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    16,   137,
      24,     6,    25,    15,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   139,     2,   140,    20,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    53,    19,    54,   142,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     7,     8,     9,    10,    11,    12,    13,    14,    17,
      18,    22,    23,    26,    27,    28,    29,    35,    36,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,     9,    12,    15,    18,    20,    21,
      26,    27,    30,    34,    35,    38,    43,    45,    47,    50,
      52,    57,    62,    66,    69,    74,    78,    83,    88,    93,
      98,   103,   106,   109,   112,   117,   122,   125,   128,   131,
     134,   135,   141,   144,   145,   149,   152,   153,   155,   159,
     161,   165,   166,   168,   172,   173,   176,   178,   181,   185,
     186,   189,   192,   193,   195,   197,   199,   201,   203,   205,
     207,   209,   211,   213,   218,   223,   228,   233,   242,   247,
     249,   251,   256,   257,   263,   268,   269,   275,   280,   285,
     287,   291,   294,   296,   300,   303,   304,   310,   311,   319,
     320,   327,   332,   335,   338,   339,   344,   347,   348,   356,
     358,   360,   362,   364,   370,   375,   380,   388,   396,   404,
     412,   421,   424,   426,   430,   432,   434,   438,   443,   445,
     446,   452,   455,   457,   459,   461,   466,   468,   473,   478,
     479,   488,   491,   493,   494,   496,   498,   500,   502,   504,
     506,   508,   511,   512,   514,   516,   518,   520,   522,   524,
     526,   528,   530,   532,   536,   540,   547,   554,   556,   557,
     563,   566,   570,   571,   572,   580,   584,   588,   589,   593,
     595,   598,   600,   603,   608,   613,   617,   621,   623,   628,
     632,   633,   635,   637,   638,   641,   645,   646,   649,   652,
     656,   661,   664,   667,   670,   674,   678,   682,   686,   690,
     694,   698,   702,   706,   710,   714,   718,   722,   726,   730,
     734,   740,   744,   748,   753,   755,   757,   762,   767,   772,
     777,   782,   787,   792,   799,   806,   813,   818,   825,   830,
     832,   839,   846,   853,   858,   863,   867,   868,   873,   874,
     879,   880,   885,   886,   888,   890,   892,   893,   894,   895,
     896,   897,   898,   918,   919,   920,   921,   922,   923,   942,
     943,   944,   952,   954,   956,   958,   960,   962,   966,   967,
     970,   974,   977,   984,   995,   998,  1000,  1001,  1003,  1006,
    1007,  1008,  1012,  1013,  1014,  1015,  1016,  1028,  1033,  1034,
    1037,  1038,  1039,  1046,  1048,  1049,  1053,  1059,  1060,  1064,
    1065,  1068,  1070,  1073,  1078,  1081,  1082,  1085,  1086,  1092,
    1094,  1097,  1102,  1108,  1115,  1117,  1120,  1121,  1124,  1129,
    1134,  1143,  1145,  1147,  1151,  1155,  1156,  1166,  1167,  1175,
    1177,  1181,  1183,  1187,  1189,  1193,  1194
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     144,     0,    -1,   117,   160,    -1,   118,   148,    -1,   128,
     253,    -1,   135,   248,    -1,   119,   146,    -1,     4,    -1,
      -1,   147,     4,     6,   210,    -1,    -1,   149,   150,    -1,
     150,   151,   105,    -1,    -1,   100,   210,    -1,   100,   210,
     136,   210,    -1,     4,    -1,   101,    -1,   107,   153,    -1,
     106,    -1,   111,     4,     6,   210,    -1,   111,     4,   136,
     210,    -1,   111,     4,   210,    -1,   110,     4,    -1,   102,
       4,   136,   210,    -1,   102,     4,   210,    -1,   102,     4,
       6,   210,    -1,    38,     4,     6,   210,    -1,    38,     4,
     136,   210,    -1,    94,     4,     6,   210,    -1,    94,     4,
     136,   210,    -1,   103,   155,    -1,   104,   154,    -1,   108,
       4,    -1,   114,     4,   136,     4,    -1,   114,     4,   136,
       3,    -1,   113,   210,    -1,   115,     3,    -1,   120,   156,
      -1,   121,   157,    -1,    -1,    61,   145,   152,   150,    36,
      -1,   122,     4,    -1,    -1,   153,   136,     4,    -1,   153,
       4,    -1,    -1,     4,    -1,   154,   136,     4,    -1,     4,
      -1,   155,   136,     4,    -1,    -1,     4,    -1,   156,   136,
       4,    -1,    -1,   158,   159,    -1,     4,    -1,   159,     4,
      -1,   159,   136,     4,    -1,    -1,   161,   162,    -1,   162,
     163,    -1,    -1,   192,    -1,   170,    -1,   240,    -1,   201,
      -1,   202,    -1,   204,    -1,   206,    -1,   172,    -1,   255,
      -1,   137,    -1,    69,    37,     4,   138,    -1,    70,    37,
     145,   138,    -1,    92,    37,   145,   138,    -1,    56,    37,
       4,   138,    -1,    56,    37,     4,   136,     4,   136,     4,
     138,    -1,    58,    37,     4,   138,    -1,    57,    -1,    59,
      -1,    90,    37,   166,   138,    -1,    -1,    91,   164,    37,
     166,   138,    -1,    71,    37,   145,   138,    -1,    -1,    61,
     145,   165,   162,    36,    -1,    85,    37,   207,   138,    -1,
     121,    37,   157,   138,    -1,     4,    -1,   166,   136,     4,
      -1,   166,     4,    -1,     5,    -1,   166,   136,     5,    -1,
     166,     5,    -1,    -1,    99,    37,   167,   166,   138,    -1,
      -1,   166,   136,    99,    37,   168,   166,   138,    -1,    -1,
     166,    99,    37,   169,   166,   138,    -1,    46,    53,   171,
      54,    -1,   171,   216,    -1,   171,   172,    -1,    -1,    72,
      37,     4,   138,    -1,   190,   189,    -1,    -1,   109,   173,
      37,   210,   136,     4,   138,    -1,     4,    -1,    32,    -1,
      15,    -1,   174,    -1,   133,    37,   176,   138,   174,    -1,
      51,    37,   174,   138,    -1,    52,    37,   174,   138,    -1,
      51,    37,    52,    37,   174,   138,   138,    -1,    51,    37,
      51,    37,   174,   138,   138,    -1,    52,    37,    51,    37,
     174,   138,   138,    -1,    52,    37,    52,    37,   174,   138,
     138,    -1,    51,    37,   133,    37,   176,   138,   174,   138,
      -1,   176,   174,    -1,   174,    -1,   177,   191,   175,    -1,
     175,    -1,     4,    -1,   139,   177,   140,    -1,   175,    37,
     177,   138,    -1,   178,    -1,    -1,   129,    37,   180,   178,
     138,    -1,   190,   189,    -1,    89,    -1,   137,    -1,    93,
      -1,    51,    37,    93,   138,    -1,   179,    -1,   185,    37,
     208,   138,    -1,    87,    37,   186,   138,    -1,    -1,   109,
     182,    37,   210,   136,     4,   138,   189,    -1,   183,   181,
      -1,   181,    -1,    -1,   183,    -1,    41,    -1,    42,    -1,
      43,    -1,    44,    -1,    45,    -1,   208,    -1,     6,   186,
      -1,    -1,    14,    -1,    13,    -1,    12,    -1,    11,    -1,
      10,    -1,     9,    -1,     8,    -1,     7,    -1,   137,    -1,
     136,    -1,     4,     6,   208,    -1,     4,   188,   208,    -1,
      97,    37,     4,     6,   208,   138,    -1,    98,    37,     4,
       6,   208,   138,    -1,   136,    -1,    -1,    62,    53,   194,
     193,    54,    -1,   193,   194,    -1,   193,   136,   194,    -1,
      -1,    -1,     4,   195,   198,    16,   196,   191,   197,    -1,
      86,     6,   208,    -1,    88,     6,   208,    -1,    -1,    37,
     199,   138,    -1,   200,    -1,   199,   200,    -1,     4,    -1,
     141,     4,    -1,    80,    37,   145,   138,    -1,    81,    37,
     203,   138,    -1,    81,    37,   138,    -1,   203,   191,   145,
      -1,   145,    -1,    82,    37,   205,   138,    -1,   205,   191,
     145,    -1,    -1,    83,    -1,    84,    -1,    -1,     4,   207,
      -1,     4,   136,   207,    -1,    -1,   209,   210,    -1,    31,
     210,    -1,    37,   210,   138,    -1,    73,    37,   210,   138,
      -1,   141,   210,    -1,    30,   210,    -1,   142,   210,    -1,
     210,    32,   210,    -1,   210,    33,   210,    -1,   210,    34,
     210,    -1,   210,    30,   210,    -1,   210,    31,   210,    -1,
     210,    29,   210,    -1,   210,    28,   210,    -1,   210,    23,
     210,    -1,   210,    22,   210,    -1,   210,    27,   210,    -1,
     210,    26,   210,    -1,   210,    24,   210,    -1,   210,    25,
     210,    -1,   210,    21,   210,    -1,   210,    20,   210,    -1,
     210,    19,   210,    -1,   210,    15,   210,    16,   210,    -1,
     210,    18,   210,    -1,   210,    17,   210,    -1,    68,    37,
       4,   138,    -1,     3,    -1,    55,    -1,    75,    37,     4,
     138,    -1,    74,    37,     4,   138,    -1,    76,    37,     4,
     138,    -1,    77,    37,     4,   138,    -1,   134,    37,     4,
     138,    -1,   103,    37,   210,   138,    -1,    38,    37,   210,
     138,    -1,    38,    37,   210,   136,   210,   138,    -1,    48,
      37,   210,   136,   210,   138,    -1,    49,    37,   210,   136,
     210,   138,    -1,    50,    37,   210,   138,    -1,    60,    37,
       4,   136,   210,   138,    -1,    39,    37,   210,   138,    -1,
       4,    -1,    78,    37,   210,   136,   210,   138,    -1,    79,
      37,   210,   136,   210,   138,    -1,   109,    37,   210,   136,
       4,   138,    -1,    86,    37,     4,   138,    -1,    88,    37,
       4,   138,    -1,    95,    25,     4,    -1,    -1,    95,    37,
     210,   138,    -1,    -1,    38,    37,   210,   138,    -1,    -1,
      96,    37,   210,   138,    -1,    -1,   130,    -1,   131,    -1,
     132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,   217,
     231,   212,   213,   214,   218,   215,    53,   219,   184,    54,
     220,   234,   211,   235,   187,   221,   191,    -1,    -1,    -1,
      -1,    -1,    -1,    67,   222,   232,   233,   212,   214,   223,
      53,   224,   236,    54,   225,   234,   211,   235,   187,   226,
     191,    -1,    -1,    -1,    91,   227,   231,   228,    53,   171,
      54,    -1,    63,    -1,    64,    -1,    65,    -1,    66,    -1,
      67,    -1,    37,   229,   138,    -1,    -1,    37,   138,    -1,
     210,   230,    16,    -1,   230,    16,    -1,    40,    37,   210,
     138,   230,    16,    -1,    40,    37,   210,   138,    39,    37,
     210,   138,   230,    16,    -1,   210,    16,    -1,    16,    -1,
      -1,    85,    -1,    25,     4,    -1,    -1,    -1,   235,    16,
       4,    -1,    -1,    -1,    -1,    -1,   236,     4,   237,    53,
     184,    54,   238,   235,   187,   239,   191,    -1,    47,    53,
     241,    54,    -1,    -1,   241,   242,    -1,    -1,    -1,     4,
     243,   245,   246,   244,   137,    -1,   210,    -1,    -1,     4,
     247,   246,    -1,    95,    37,   210,   138,   246,    -1,    -1,
      37,   210,   138,    -1,    -1,   249,   250,    -1,   251,    -1,
     250,   251,    -1,    53,   252,    54,   137,    -1,   261,   137,
      -1,    -1,   254,   257,    -1,    -1,   256,   127,    53,   257,
      54,    -1,   258,    -1,   257,   258,    -1,    53,   260,    54,
     137,    -1,   123,    53,   260,    54,   137,    -1,   123,    53,
     260,    54,   259,   137,    -1,   123,    -1,   259,   123,    -1,
      -1,   261,   137,    -1,   125,    16,   261,   137,    -1,   126,
      16,   261,   137,    -1,   125,    16,   261,   137,   126,    16,
     261,   137,    -1,   124,    -1,     4,    -1,   261,   137,   124,
      -1,   261,   137,     4,    -1,    -1,   261,   137,   121,     4,
      53,   262,   261,   264,    54,    -1,    -1,   121,     4,    53,
     263,   261,   264,    54,    -1,   125,    -1,   261,   137,   125,
      -1,   126,    -1,   261,   137,   126,    -1,   121,    -1,   261,
     137,   121,    -1,    -1,   137,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   164,   164,   165,   166,   167,   168,   172,   176,   176,
     186,   186,   199,   200,   204,   205,   206,   209,   212,   213,
     214,   216,   218,   220,   222,   224,   226,   228,   230,   232,
     234,   236,   237,   238,   240,   242,   244,   246,   248,   249,
     251,   250,   254,   256,   260,   261,   262,   266,   268,   272,
     274,   279,   280,   281,   286,   286,   291,   293,   295,   300,
     300,   306,   307,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   324,   326,   328,   331,   333,   335,
     337,   339,   341,   340,   344,   347,   346,   350,   354,   358,
     361,   364,   367,   370,   373,   377,   376,   381,   380,   385,
     384,   391,   395,   396,   397,   401,   403,   404,   404,   412,
     416,   420,   427,   433,   439,   445,   451,   457,   463,   469,
     475,   484,   493,   504,   513,   524,   532,   536,   543,   545,
     544,   551,   552,   556,   557,   562,   567,   568,   573,   577,
     577,   583,   584,   587,   589,   593,   595,   597,   599,   601,
     606,   613,   615,   619,   621,   623,   625,   627,   629,   631,
     633,   638,   638,   643,   647,   655,   659,   667,   667,   671,
     675,   676,   677,   682,   681,   689,   697,   705,   706,   710,
     711,   715,   717,   722,   727,   728,   733,   735,   741,   743,
     745,   749,   751,   757,   760,   769,   780,   780,   786,   788,
     790,   792,   794,   796,   799,   801,   803,   805,   807,   809,
     811,   813,   815,   817,   819,   821,   823,   825,   827,   829,
     831,   833,   835,   837,   839,   841,   844,   846,   848,   850,
     852,   854,   856,   858,   860,   862,   864,   866,   875,   877,
     879,   881,   883,   885,   887,   893,   894,   898,   899,   903,
     904,   908,   909,   913,   914,   915,   916,   919,   923,   926,
     932,   934,   919,   941,   943,   945,   950,   952,   940,   962,
     964,   962,   972,   973,   974,   975,   976,   980,   981,   982,
     986,   987,   992,   993,   998,   999,  1004,  1005,  1010,  1012,
    1017,  1020,  1033,  1037,  1042,  1044,  1035,  1052,  1055,  1057,
    1061,  1062,  1061,  1071,  1116,  1119,  1131,  1140,  1143,  1150,
    1150,  1162,  1163,  1167,  1171,  1180,  1180,  1194,  1194,  1204,
    1205,  1209,  1213,  1217,  1224,  1228,  1236,  1239,  1243,  1247,
    1251,  1258,  1262,  1266,  1270,  1275,  1274,  1288,  1287,  1297,
    1301,  1305,  1309,  1313,  1317,  1323,  1325
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "NAME", "LNAME", "'='", "OREQ",
  "ANDEQ", "RSHIFTEQ", "LSHIFTEQ", "DIVEQ", "MULTEQ", "MINUSEQ", "PLUSEQ",
  "'?'", "':'", "OROR", "ANDAND", "'|'", "'^'", "'&'", "NE", "EQ", "'<'",
  "'>'", "GE", "LE", "RSHIFT", "LSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'",
  "UNARY", "END", "'('", "ALIGN_K", "BLOCK", "BIND", "QUAD", "SQUAD",
  "LONG", "SHORT", "BYTE", "SECTIONS", "PHDRS", "DATA_SEGMENT_ALIGN",
  "DATA_SEGMENT_RELRO_END", "DATA_SEGMENT_END", "SORT_BY_NAME",
  "SORT_BY_ALIGNMENT", "'{'", "'}'", "SIZEOF_HEADERS", "OUTPUT_FORMAT",
  "FORCE_COMMON_ALLOCATION", "OUTPUT_ARCH", "INHIBIT_COMMON_ALLOCATION",
  "SEGMENT_START", "INCLUDE", "MEMORY", "NOLOAD", "DSECT", "COPY", "INFO",
  "OVERLAY", "DEFINED", "TARGET_K", "SEARCH_DIR", "MAP", "ENTRY", "NEXT",
  "SIZEOF", "ALIGNOF", "ADDR", "LOADADDR", "MAX_K", "MIN_K", "STARTUP",
  "HLL", "SYSLIB", "FLOAT", "NOFLOAT", "NOCROSSREFS", "ORIGIN", "FILL",
  "LENGTH", "CREATE_OBJECT_SYMBOLS", "INPUT", "GROUP", "OUTPUT",
  "CONSTRUCTORS", "ALIGNMOD", "AT", "SUBALIGN", "PROVIDE",
  "PROVIDE_HIDDEN", "AS_NEEDED", "CHIP", "LIST", "SECT", "ABSOLUTE",
  "LOAD", "NEWLINE", "ENDWORD", "ORDER", "NAMEWORD", "ASSERT_K", "FORMAT",
  "PUBLIC", "DEFSYMEND", "BASE", "ALIAS", "TRUNCATE", "REL",
  "INPUT_SCRIPT", "INPUT_MRI_SCRIPT", "INPUT_DEFSYM", "CASE", "EXTERN",
  "START", "VERS_TAG", "VERS_IDENTIFIER", "GLOBAL", "LOCAL", "VERSIONK",
  "INPUT_VERSION_SCRIPT", "KEEP", "ONLY_IF_RO", "ONLY_IF_RW", "SPECIAL",
  "EXCLUDE_FILE", "CONSTANT", "INPUT_DYNAMIC_LIST", "','", "';'", "')'",
  "'['", "']'", "'!'", "'~'", "$accept", "file", "filename", "defsym_expr",
  "@1", "mri_script_file", "@2", "mri_script_lines", "mri_script_command",
  "@3", "ordernamelist", "mri_load_name_list", "mri_abs_name_list",
  "casesymlist", "extern_name_list", "@4", "extern_name_list_body",
  "script_file", "@5", "ifile_list", "ifile_p1", "@6", "@7", "input_list",
  "@8", "@9", "@10", "sections", "sec_or_group_p1", "statement_anywhere",
  "@11", "wildcard_name", "wildcard_spec", "exclude_name_list",
  "file_NAME_list", "input_section_spec_no_keep", "input_section_spec",
  "@12", "statement", "@13", "statement_list", "statement_list_opt",
  "length", "fill_exp", "fill_opt", "assign_op", "end", "assignment",
  "opt_comma", "memory", "memory_spec_list", "memory_spec", "@14",
  "origin_spec", "length_spec", "attributes_opt", "attributes_list",
  "attributes_string", "startup", "high_level_library",
  "high_level_library_NAME_list", "low_level_library",
  "low_level_library_NAME_list", "floating_point_support",
  "nocrossref_list", "mustbe_exp", "@15", "exp", "memspec_at_opt",
  "opt_at", "opt_align", "opt_subalign", "sect_constraint", "section",
  "@16", "@17", "@18", "@19", "@20", "@21", "@22", "@23", "@24", "@25",
  "@26", "@27", "type", "atype", "opt_exp_with_type",
  "opt_exp_without_type", "opt_nocrossrefs", "memspec_opt", "phdr_opt",
  "overlay_section", "@28", "@29", "@30", "phdrs", "phdr_list", "phdr",
  "@31", "@32", "phdr_type", "phdr_qualifiers", "phdr_val",
  "dynamic_list_file", "@33", "dynamic_list_nodes", "dynamic_list_node",
  "dynamic_list_tag", "version_script_file", "@34", "version", "@35",
  "vers_nodes", "vers_node", "verdep", "vers_tag", "vers_defns", "@36",
  "@37", "opt_semicolon", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,    61,   261,   262,   263,
     264,   265,   266,   267,   268,    63,    58,   269,   270,   124,
      94,    38,   271,   272,    60,    62,   273,   274,   275,   276,
      43,    45,    42,    47,    37,   277,   278,    40,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   123,   125,   294,   295,   296,   297,   298,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   328,
     329,   330,   331,   332,   333,   334,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,   371,   372,   373,   374,    44,    59,    41,    91,
      93,    33,   126
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   143,   144,   144,   144,   144,   144,   145,   147,   146,
     149,   148,   150,   150,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
     152,   151,   151,   151,   153,   153,   153,   154,   154,   155,
     155,   156,   156,   156,   158,   157,   159,   159,   159,   161,
     160,   162,   162,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   164,   163,   163,   165,   163,   163,   163,   166,
     166,   166,   166,   166,   166,   167,   166,   168,   166,   169,
     166,   170,   171,   171,   171,   172,   172,   173,   172,   174,
     174,   174,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   176,   176,   177,   177,   178,   178,   178,   179,   180,
     179,   181,   181,   181,   181,   181,   181,   181,   181,   182,
     181,   183,   183,   184,   184,   185,   185,   185,   185,   185,
     186,   187,   187,   188,   188,   188,   188,   188,   188,   188,
     188,   189,   189,   190,   190,   190,   190,   191,   191,   192,
     193,   193,   193,   195,   194,   196,   197,   198,   198,   199,
     199,   200,   200,   201,   202,   202,   203,   203,   204,   205,
     205,   206,   206,   207,   207,   207,   209,   208,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   211,   211,   212,   212,   213,
     213,   214,   214,   215,   215,   215,   215,   217,   218,   219,
     220,   221,   216,   222,   223,   224,   225,   226,   216,   227,
     228,   216,   229,   229,   229,   229,   229,   230,   230,   230,
     231,   231,   231,   231,   232,   232,   233,   233,   234,   234,
     235,   235,   236,   237,   238,   239,   236,   240,   241,   241,
     243,   244,   242,   245,   246,   246,   246,   247,   247,   249,
     248,   250,   250,   251,   252,   254,   253,   256,   255,   257,
     257,   258,   258,   258,   259,   259,   260,   260,   260,   260,
     260,   261,   261,   261,   261,   262,   261,   263,   261,   261,
     261,   261,   261,   261,   261,   264,   264
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     2,     2,     2,     2,     1,     0,     4,
       0,     2,     3,     0,     2,     4,     1,     1,     2,     1,
       4,     4,     3,     2,     4,     3,     4,     4,     4,     4,
       4,     2,     2,     2,     4,     4,     2,     2,     2,     2,
       0,     5,     2,     0,     3,     2,     0,     1,     3,     1,
       3,     0,     1,     3,     0,     2,     1,     2,     3,     0,
       2,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     4,     4,     4,     4,     8,     4,     1,
       1,     4,     0,     5,     4,     0,     5,     4,     4,     1,
       3,     2,     1,     3,     2,     0,     5,     0,     7,     0,
       6,     4,     2,     2,     0,     4,     2,     0,     7,     1,
       1,     1,     1,     5,     4,     4,     7,     7,     7,     7,
       8,     2,     1,     3,     1,     1,     3,     4,     1,     0,
       5,     2,     1,     1,     1,     4,     1,     4,     4,     0,
       8,     2,     1,     0,     1,     1,     1,     1,     1,     1,
       1,     2,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     6,     6,     1,     0,     5,
       2,     3,     0,     0,     7,     3,     3,     0,     3,     1,
       2,     1,     2,     4,     4,     3,     3,     1,     4,     3,
       0,     1,     1,     0,     2,     3,     0,     2,     2,     3,
       4,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       5,     3,     3,     4,     1,     1,     4,     4,     4,     4,
       4,     4,     4,     6,     6,     6,     4,     6,     4,     1,
       6,     6,     6,     4,     4,     3,     0,     4,     0,     4,
       0,     4,     0,     1,     1,     1,     0,     0,     0,     0,
       0,     0,    19,     0,     0,     0,     0,     0,    18,     0,
       0,     7,     1,     1,     1,     1,     1,     3,     0,     2,
       3,     2,     6,    10,     2,     1,     0,     1,     2,     0,
       0,     3,     0,     0,     0,     0,    11,     4,     0,     2,
       0,     0,     6,     1,     0,     3,     5,     0,     3,     0,
       2,     1,     2,     4,     2,     0,     2,     0,     5,     1,
       2,     4,     5,     6,     1,     2,     0,     2,     4,     4,
       8,     1,     1,     3,     3,     0,     9,     0,     7,     1,
       3,     1,     3,     1,     3,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       0,    59,    10,     8,   315,   309,     0,     2,    62,     3,
      13,     6,     0,     4,     0,     5,     0,     1,    60,    11,
       0,   326,     0,   316,   319,     0,   310,   311,     0,     0,
       0,     0,    79,     0,    80,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   191,   192,     0,     0,    82,     0,
       0,     0,   107,     0,    72,    61,    64,    70,     0,    63,
      66,    67,    68,    69,    65,    71,     0,    16,     0,     0,
       0,     0,    17,     0,     0,     0,    19,    46,     0,     0,
       0,     0,     0,     0,    51,    54,     0,     0,     0,   332,
     343,   331,   339,   341,     0,     0,   326,   320,   339,   341,
       0,     0,   312,   196,   160,   159,   158,   157,   156,   155,
     154,   153,   196,   104,   298,     0,     0,     7,    85,     0,
       0,     0,     0,     0,     0,     0,   190,   193,     0,     0,
       0,     0,     0,     0,    54,   162,   161,   106,     0,     0,
      40,     0,   224,   239,     0,     0,     0,     0,     0,     0,
       0,     0,   225,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    14,
       0,    49,    31,    47,    32,    18,    33,    23,     0,    36,
       0,    37,    52,    38,    39,     0,    42,    12,     9,     0,
       0,     0,     0,   327,     0,     0,   314,   163,     0,   164,
       0,     0,     0,     0,    62,   173,   172,     0,     0,     0,
       0,     0,   185,   187,   168,   168,   193,     0,    89,    92,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    13,     0,     0,   202,   198,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   201,   203,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      25,     0,     0,    45,     0,     0,     0,    22,     0,     0,
      56,    55,   337,     0,     0,   321,   334,   344,   333,   340,
     342,     0,   313,   197,   257,   101,   263,   269,   103,   102,
     300,   297,   299,     0,    76,    78,   317,   177,     0,    73,
      74,    84,   105,   183,   167,   184,     0,   188,     0,   193,
     194,    87,    95,    91,    94,     0,     0,    81,     0,    75,
     196,   196,     0,    88,     0,    27,    28,    43,    29,    30,
     199,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   222,   221,   219,   218,   217,   212,   211,   215,   216,
     214,   213,   210,   209,   207,   208,   204,   205,   206,    15,
      26,    24,    50,    48,    44,    20,    21,    35,    34,    53,
      57,     0,     0,   328,   329,     0,   324,   322,     0,   278,
       0,   278,     0,     0,    86,     0,     0,   169,     0,   170,
     186,   189,   195,     0,    99,    90,    93,     0,    83,     0,
       0,     0,   318,    41,     0,   232,   238,     0,     0,   236,
       0,   223,   200,   227,   226,   228,   229,     0,     0,   243,
     244,   231,     0,   230,     0,    58,   345,   342,   335,   325,
     323,     0,     0,   278,     0,   248,   285,     0,   286,   270,
     303,   304,     0,   181,     0,     0,   179,     0,   171,     0,
       0,    97,   165,   166,     0,     0,     0,     0,     0,     0,
       0,     0,   220,   346,     0,     0,     0,   272,   273,   274,
     275,   276,   279,     0,     0,     0,     0,   281,     0,   250,
     284,   287,   248,     0,   307,     0,   301,     0,   182,   178,
     180,     0,   168,    96,     0,     0,   108,   233,   234,   235,
     237,   240,   241,   242,   338,     0,   345,   277,     0,   280,
       0,     0,   252,   252,   104,     0,   304,     0,     0,    77,
     196,     0,   100,     0,   330,     0,   278,     0,     0,     0,
     258,   264,     0,     0,   305,     0,   302,   175,     0,   174,
      98,   336,     0,     0,   247,     0,     0,   256,     0,   271,
     308,   304,   196,     0,   282,   249,     0,   253,   254,   255,
       0,   265,   306,   176,     0,   251,   259,   292,   278,   143,
       0,     0,   125,   111,   110,   145,   146,   147,   148,   149,
       0,     0,     0,   132,   134,   139,     0,     0,   133,     0,
     112,     0,   128,   136,   142,   144,     0,     0,     0,   293,
     266,   283,     0,     0,   196,     0,   129,     0,   109,     0,
     124,   168,     0,   141,   260,   196,   131,     0,   289,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   150,     0,
       0,   122,     0,     0,   126,     0,   168,   289,     0,   143,
       0,   246,     0,     0,   135,     0,   114,     0,     0,   115,
     138,     0,   109,     0,     0,   121,   123,   127,   246,   137,
       0,   288,     0,   290,     0,     0,     0,     0,     0,     0,
     130,   113,   290,   294,     0,   152,     0,     0,     0,     0,
       0,     0,   152,   290,   245,   196,     0,   267,   117,   116,
       0,   118,   119,     0,   261,   152,   151,   291,   168,   120,
     140,   168,   295,   268,   262,   168,   296
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,   118,    11,    12,     9,    10,    19,    87,   231,
     175,   174,   172,   183,   184,   185,   291,     7,     8,    18,
      55,   129,   204,   221,   423,   525,   480,    56,   200,    57,
     133,   620,   621,   662,   641,   622,   623,   660,   624,   635,
     625,   626,   627,   657,   717,   112,   137,    58,   665,    59,
     318,   206,   317,   522,   569,   416,   475,   476,    60,    61,
     214,    62,   215,    63,   217,   658,   198,   236,   693,   509,
     542,   560,   590,   309,   409,   577,   599,   667,   731,   410,
     578,   597,   648,   728,   411,   513,   503,   464,   465,   468,
     512,   671,   705,   600,   647,   713,   735,    64,   201,   312,
     412,   548,   471,   516,   546,    15,    16,    26,    27,   100,
      13,    14,    65,    66,    23,    24,   408,    94,    95,   496,
     402,   494
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -675
static const yytype_int16 yypact[] =
{
     293,  -675,  -675,  -675,  -675,  -675,    42,  -675,  -675,  -675,
    -675,  -675,    46,  -675,   -18,  -675,    14,  -675,   736,  1472,
      65,   104,    51,   -18,  -675,   206,    14,  -675,   445,    57,
      63,    40,  -675,    86,  -675,   124,   119,   139,   140,   144,
     189,   190,   196,   204,  -675,  -675,   209,   220,  -675,   223,
     231,   236,  -675,   238,  -675,  -675,  -675,  -675,    -4,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,   153,  -675,   147,   124,
     277,   613,  -675,   279,   280,   283,  -675,  -675,   285,   294,
     296,   613,   298,   301,   302,  -675,   303,   205,   613,  -675,
     305,  -675,   307,   308,   259,   180,   104,  -675,  -675,  -675,
     264,   184,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,   321,   324,  -675,  -675,   331,
     333,   124,   124,   338,   124,    16,  -675,   339,    20,   309,
     124,   346,   348,   317,  -675,  -675,  -675,  -675,   311,    28,
    -675,    35,  -675,  -675,   613,   613,   613,   320,   325,   328,
     344,   345,  -675,   347,   350,   351,   355,   356,   357,   358,
     363,   368,   369,   370,   371,   376,   395,   613,   613,  1295,
     217,  -675,   225,  -675,   295,    10,  -675,  -675,   341,  1667,
     299,  -675,  -675,   300,  -675,   381,  -675,  -675,  1667,   390,
     206,   206,   310,   249,   380,   323,   249,  -675,   613,  -675,
     394,    22,   -25,   326,  -675,  -675,  -675,   327,   329,   330,
     332,   334,  -675,  -675,    73,   100,    11,   335,  -675,  -675,
     408,    27,    20,   362,   440,   465,   613,   364,   -18,   613,
     613,  -675,   613,   613,  -675,  -675,   929,   613,   613,   613,
     613,   613,   480,   482,   613,   484,   489,   490,   491,   613,
     613,   497,   500,   613,   613,   502,  -675,  -675,   613,   613,
     613,   613,   613,   613,   613,   613,   613,   613,   613,   613,
     613,   613,   613,   613,   613,   613,   613,   613,   613,   613,
    1667,   506,   507,  -675,   511,   613,   613,  1667,   136,   512,
    -675,    24,  -675,   384,   385,  -675,  -675,   514,  -675,  -675,
    -675,   -75,  -675,  1667,   445,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,   519,  -675,  -675,   808,   488,    98,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,   124,  -675,   124,   339,
    -675,  -675,  -675,  -675,  -675,   492,    32,  -675,    81,  -675,
    -675,  -675,  1315,  -675,   -14,  1667,  1667,  1496,  1667,  1667,
    -675,   745,   949,  1335,  1355,   969,   406,   388,   989,   393,
     405,   410,   411,  1375,  1395,   412,   413,  1009,  1415,   422,
    1627,  1494,  1516,  1682,   909,   701,   894,   894,   525,   525,
     525,   525,   392,   392,   152,   152,  -675,  -675,  -675,  1667,
    1667,  1667,  -675,  -675,  -675,  1667,  1667,  -675,  -675,  -675,
    -675,   526,   206,   278,   249,   508,  -675,  -675,   -68,   459,
     536,   459,   613,   427,  -675,     4,   548,  -675,   331,  -675,
    -675,  -675,  -675,    20,  -675,  -675,  -675,   532,  -675,   432,
     438,   573,  -675,  -675,   613,  -675,  -675,   613,   613,  -675,
     613,  -675,  -675,  -675,  -675,  -675,  -675,   613,   613,  -675,
    -675,  -675,   574,  -675,   613,  -675,   442,   564,  -675,  -675,
    -675,    15,   544,  1604,   566,   503,  -675,  1647,   510,  -675,
    1667,    25,   583,  -675,   588,     3,  -675,   513,  -675,   125,
      20,  -675,  -675,  -675,   464,  1029,  1051,  1071,  1091,  1111,
    1131,   467,  1667,   249,   543,   206,   206,  -675,  -675,  -675,
    -675,  -675,  -675,   468,   613,   375,   587,  -675,   570,   580,
    -675,  -675,   503,   555,   584,   586,  -675,   487,  -675,  -675,
    -675,   620,   495,  -675,   133,    20,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,   496,   442,  -675,  1151,  -675,
     613,   590,   540,   540,  -675,   613,    25,   613,   501,  -675,
    -675,   549,  -675,   138,   249,   592,   212,  1173,   613,   591,
    -675,  -675,   474,  1193,  -675,  1213,  -675,  -675,   634,  -675,
    -675,  -675,   605,   631,  -675,  1233,   613,   113,   595,  -675,
    -675,    25,  -675,   613,  -675,  -675,  1253,  -675,  -675,  -675,
     596,  -675,  -675,  -675,  1273,  -675,  -675,  -675,   617,   652,
      68,   639,   699,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
     621,   622,   623,  -675,  -675,  -675,   627,   629,  -675,   227,
    -675,   632,  -675,  -675,  -675,   652,   603,   635,    -4,  -675,
    -675,  -675,   304,   282,  -675,   637,  -675,   284,  -675,   638,
    -675,   -93,   227,  -675,  -675,  -675,  -675,   618,   651,   642,
     643,   545,   645,   547,   661,   663,   576,   577,  -675,   613,
      83,  -675,    12,   297,  -675,   227,   114,   651,   579,   652,
     698,   624,   284,   284,  -675,   284,  -675,   284,   284,  -675,
    -675,  1435,   582,   599,   284,  -675,  -675,  -675,   624,  -675,
     664,  -675,   696,  -675,   600,   604,    80,   606,   608,   739,
    -675,  -675,  -675,  -675,   744,    43,   614,   615,   284,   619,
     646,   648,    43,  -675,  -675,  -675,   747,  -675,  -675,  -675,
     649,  -675,  -675,    -4,  -675,    43,  -675,  -675,   495,  -675,
    -675,   495,  -675,  -675,  -675,   495,  -675
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -675,  -675,   -64,  -675,  -675,  -675,  -675,   527,  -675,  -675,
    -675,  -675,  -675,  -675,   625,  -675,  -675,  -675,  -675,   552,
    -675,  -675,  -675,  -210,  -675,  -675,  -675,  -675,   244,  -194,
    -675,   -43,  -407,   105,   148,   141,  -675,  -675,   171,  -675,
    -675,   130,  -675,    85,  -674,  -675,  -606,  -569,  -211,  -675,
    -675,  -297,  -675,  -675,  -675,  -675,  -675,   336,  -675,  -675,
    -675,  -675,  -675,  -675,  -193,  -101,  -675,   -71,   115,   290,
    -675,   261,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -450,   398,  -675,
    -675,   143,  -588,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -513,  -675,  -675,  -675,  -675,   787,  -675,
    -675,  -675,  -675,  -675,   594,   -22,  -675,   718,   -16,  -675,
    -675,   287
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -318
static const yytype_int16 yytable[] =
{
     169,    97,   197,   326,   328,   140,   308,   473,   473,   101,
     179,   199,   338,   506,   283,   216,   638,   188,   142,   143,
     117,   419,   646,   330,   218,   219,   310,   603,   400,   514,
     628,   333,   334,   564,   229,    21,   425,   426,   724,    21,
     432,   232,    17,   324,   604,   144,   145,   664,   406,   715,
      20,   732,   146,   147,   148,   459,   628,   208,   209,   716,
     211,   213,   407,   149,   150,   151,   223,    25,   592,   460,
     152,    88,   629,   234,   235,   153,   311,   115,   497,   498,
     499,   500,   501,   154,   638,   333,   334,   682,   155,   156,
     157,   158,   159,   160,   161,   603,   256,   257,   603,   280,
     628,   162,   205,   163,    96,    22,   573,   287,    89,    22,
     113,   313,   604,   314,   712,   604,   114,   730,   164,   220,
     515,   478,   630,   116,   165,   725,   335,   303,   117,   333,
     334,   427,   135,   136,   639,   611,   422,   333,   334,   397,
     398,   519,   333,   334,   474,   474,   284,   329,   601,   166,
     684,   139,   417,   502,   212,   342,   167,   168,   345,   346,
     401,   348,   349,   336,   230,   337,   351,   352,   353,   354,
     355,   233,   119,   358,   293,   294,   120,   121,   363,   364,
     335,   122,   367,   368,   274,   275,   276,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,   389,   390,   391,   324,
      89,   325,   640,   479,   395,   396,   617,   336,   708,   428,
     142,   143,   619,   278,   335,    90,   123,   124,    91,    92,
      93,   638,   335,   125,   418,   640,   324,   335,   327,   429,
     430,   126,   603,   587,   588,   589,   127,   144,   145,   505,
     324,   572,   687,   296,   146,   147,   148,   128,   686,   604,
     130,   336,   420,   523,   421,   149,   150,   151,   131,   336,
     524,   552,   152,   132,   336,   134,   570,   153,   639,   611,
     138,   141,   296,   170,   171,   154,   638,   173,   638,   176,
     155,   156,   157,   158,   159,   160,   161,   603,   177,   603,
     178,   638,   180,   162,   181,   163,   182,   186,   638,   189,
     187,   551,   603,   192,   604,   553,   604,   193,   195,   603,
     164,   196,    97,   190,   191,   202,   165,    90,   203,   604,
      91,    98,    99,   654,   655,   205,   604,   207,   463,   467,
     463,   470,   210,   216,   142,   143,   222,   285,   649,   650,
     224,   166,   225,   279,   226,   649,   650,   237,   167,   168,
     617,   281,   238,   485,   228,   239,   486,   487,   308,   488,
     297,   144,   145,   298,   299,   300,   489,   490,   146,   147,
     148,   240,   241,   492,   242,   290,   456,   243,   244,   149,
     150,   151,   245,   246,   247,   248,   152,   651,   304,   297,
     249,   153,   298,   299,   457,   250,   251,   252,   253,   154,
       1,     2,     3,   254,   155,   156,   157,   158,   159,   160,
     161,     4,   272,   273,   274,   275,   276,   162,     5,   163,
     652,   282,   255,   538,   301,   288,   289,   652,   497,   498,
     499,   500,   501,   292,   164,   332,   340,   295,   305,   567,
     165,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     302,   306,   142,   143,   315,   319,    40,   320,   321,   557,
     322,   341,   323,   331,   563,   166,   565,   286,   304,   535,
     536,   593,   167,   168,   356,   307,   357,   575,   359,   144,
     145,    50,    51,   360,   361,   362,   461,   147,   148,   462,
     339,   365,   343,    52,   366,   586,   369,   149,   150,   151,
     392,   393,   594,   502,   152,   394,   399,   733,   405,   153,
     734,   403,   404,   413,   736,   415,   441,   154,   579,   424,
     455,   443,   155,   156,   157,   158,   159,   160,   161,   142,
     143,   306,   440,   444,   668,   162,    40,   163,   445,   446,
     449,   450,   466,   270,   271,   272,   273,   274,   275,   276,
     453,   458,   164,   472,   477,   307,   144,   145,   165,   481,
     482,    50,    51,   146,   147,   148,   483,   484,   491,   493,
     495,   504,   507,    52,   149,   150,   151,   517,   681,   653,
     656,   152,   518,   166,   661,   511,   153,   534,   508,   521,
     167,   168,   526,   539,   154,   533,   537,   540,   544,   155,
     156,   157,   158,   159,   160,   161,   142,   143,   541,   685,
     653,   545,   162,   547,   163,   549,   550,   558,   576,   694,
     695,   324,   661,   554,   697,   698,   559,   568,   566,   164,
     582,   701,   583,   144,   145,   165,   571,   584,   591,   596,
     146,   147,   148,   685,   505,   631,   602,   644,   632,   633,
     634,   149,   150,   151,   636,   720,   637,   603,   152,   642,
     166,   669,   645,   153,   659,   663,   670,   167,   168,   672,
     673,   154,   675,   674,   604,   676,   155,   156,   157,   158,
     159,   160,   161,   605,   606,   607,   608,   609,   677,   162,
     678,   163,   691,   610,   611,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   679,   680,   164,   689,   703,   692,
    -125,   704,   165,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,  -109,   700,   706,   612,
      28,   613,   707,   711,   709,   614,   710,   166,   714,    50,
      51,   727,   718,   719,   167,   168,   316,   721,   347,   227,
     258,   615,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     696,   616,    29,    30,   722,   617,   723,   729,   562,   618,
     666,   619,    31,    32,    33,    34,   643,    35,    36,   690,
     726,   683,   543,   702,   561,    37,    38,    39,    40,   469,
     688,   520,    28,   102,   194,     0,    41,    42,    43,    44,
      45,    46,   344,   555,     0,     0,    47,    48,    49,     0,
       0,     0,     0,    50,    51,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   414,    52,     0,     0,     0,     0,
       0,     0,     0,     0,    29,    30,     0,    53,     0,     0,
       0,     0,     0,  -317,    31,    32,    33,    34,     0,    35,
      36,     0,     0,    54,     0,     0,     0,    37,    38,    39,
      40,   434,     0,   435,     0,     0,     0,     0,    41,    42,
      43,    44,    45,    46,     0,     0,     0,     0,    47,    48,
      49,     0,     0,     0,     0,    50,    51,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    52,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,    53,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   258,    54,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   258,     0,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   258,     0,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   258,     0,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   258,     0,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   258,     0,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,     0,     0,   258,   350,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   258,   436,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   258,   439,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   258,   442,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   258,   451,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   258,   527,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,     0,     0,   258,   528,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   258,   529,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   258,   530,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   258,   531,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   258,   532,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   258,   556,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,     0,     0,
     258,   574,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     258,   580,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     258,   581,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     258,   585,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     258,   595,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     258,   598,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     258,   277,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     258,   431,   259,   260,   261,   262,   263,   264,   265,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
       0,   437,     0,     0,     0,     0,    67,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   438,     0,     0,     0,     0,     0,     0,     0,     0,
      67,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      68,   447,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,     0,
       0,   448,   433,    69,    68,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   452,     0,     0,     0,     0,     0,    69,     0,     0,
       0,     0,     0,     0,     0,     0,    70,     0,     0,     0,
       0,   699,    71,    72,    73,    74,    75,   -43,    76,    77,
      78,     0,    79,    80,     0,    81,    82,    83,     0,     0,
      70,     0,    84,    85,    86,     0,    71,    72,    73,    74,
      75,     0,    76,    77,    78,     0,    79,    80,     0,    81,
      82,    83,     0,     0,     0,     0,    84,    85,    86,   258,
       0,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   275,   276,     0,
       0,   505,   258,   454,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   258,   510,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   258,     0,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,   276
};

static const yytype_int16 yycheck[] =
{
      71,    23,   103,   214,   215,    69,   200,     4,     4,    25,
      81,   112,   222,   463,     4,     4,     4,    88,     3,     4,
       4,   318,   628,   216,     4,     5,     4,    15,     4,     4,
     599,     4,     5,   546,     6,    53,     4,     5,   712,    53,
      54,     6,     0,   136,    32,    30,    31,   140,   123,     6,
       4,   725,    37,    38,    39,   123,   625,   121,   122,    16,
     124,   125,   137,    48,    49,    50,   130,    53,   581,   137,
      55,     6,     4,   144,   145,    60,    54,    37,    63,    64,
      65,    66,    67,    68,     4,     4,     5,     4,    73,    74,
      75,    76,    77,    78,    79,    15,   167,   168,    15,   170,
     669,    86,     4,    88,    53,   123,   556,   178,     4,   123,
      53,   136,    32,   138,   702,    32,    53,   723,   103,    99,
      95,   418,    54,    37,   109,   713,    99,   198,     4,     4,
       5,    99,   136,   137,    51,    52,   329,     4,     5,     3,
       4,   138,     4,     5,   141,   141,   136,   136,   598,   134,
     138,     4,    54,   138,   138,   226,   141,   142,   229,   230,
     136,   232,   233,   136,   136,   138,   237,   238,   239,   240,
     241,   136,    53,   244,   190,   191,    37,    37,   249,   250,
      99,    37,   253,   254,    32,    33,    34,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   136,
       4,   138,   619,   423,   285,   286,   133,   136,   138,   138,
       3,     4,   139,     6,    99,   121,    37,    37,   124,   125,
     126,     4,    99,    37,   136,   642,   136,    99,   138,   340,
     341,    37,    15,   130,   131,   132,    37,    30,    31,    37,
     136,    39,   138,     4,    37,    38,    39,    37,   665,    32,
      37,   136,   326,   138,   328,    48,    49,    50,    37,   136,
     480,   138,    55,    37,   136,    37,   138,    60,    51,    52,
     127,     4,     4,     4,     4,    68,     4,     4,     4,     4,
      73,    74,    75,    76,    77,    78,    79,    15,     4,    15,
       4,     4,     4,    86,     3,    88,     4,     4,     4,     4,
     105,   522,    15,    54,    32,   525,    32,   137,    54,    15,
     103,   137,   344,    16,    16,     4,   109,   121,     4,    32,
     124,   125,   126,    51,    52,     4,    32,     4,   409,   410,
     411,   412,     4,     4,     3,     4,    37,     6,    51,    52,
       4,   134,     4,   136,    37,    51,    52,    37,   141,   142,
     133,   136,    37,   434,    53,    37,   437,   438,   562,   440,
     121,    30,    31,   124,   125,   126,   447,   448,    37,    38,
      39,    37,    37,   454,    37,     4,   402,    37,    37,    48,
      49,    50,    37,    37,    37,    37,    55,    93,     4,   121,
      37,    60,   124,   125,   126,    37,    37,    37,    37,    68,
     117,   118,   119,    37,    73,    74,    75,    76,    77,    78,
      79,   128,    30,    31,    32,    33,    34,    86,   135,    88,
     133,   136,    37,   504,    54,   136,   136,   133,    63,    64,
      65,    66,    67,    53,   103,    37,     6,   137,    54,   550,
     109,     6,     7,     8,     9,    10,    11,    12,    13,    14,
     137,    67,     3,     4,   138,   138,    72,   138,   138,   540,
     138,     6,   138,   138,   545,   134,   547,   136,     4,   495,
     496,   582,   141,   142,     4,    91,     4,   558,     4,    30,
      31,    97,    98,     4,     4,     4,    37,    38,    39,    40,
     138,     4,   138,   109,     4,   576,     4,    48,    49,    50,
       4,     4,   583,   138,    55,     4,     4,   728,     4,    60,
     731,   137,   137,     4,   735,    37,   138,    68,    54,    37,
       4,   138,    73,    74,    75,    76,    77,    78,    79,     3,
       4,    67,   136,   138,   645,    86,    72,    88,   138,   138,
     138,   138,    16,    28,    29,    30,    31,    32,    33,    34,
     138,    53,   103,   136,    16,    91,    30,    31,   109,    37,
     138,    97,    98,    37,    38,    39,   138,     4,     4,   137,
      16,    37,    16,   109,    48,    49,    50,     4,   659,   632,
     633,    55,     4,   134,   637,    85,    60,    54,    95,    86,
     141,   142,   138,    16,    68,   138,   138,    37,    53,    73,
      74,    75,    76,    77,    78,    79,     3,     4,    38,   662,
     663,    37,    86,    37,    88,   138,     6,    37,    37,   672,
     673,   136,   675,   137,   677,   678,    96,    88,   137,   103,
       6,   684,    37,    30,    31,   109,    54,    16,    53,    53,
      37,    38,    39,   696,    37,    16,     4,    54,    37,    37,
      37,    48,    49,    50,    37,   708,    37,    15,    55,    37,
     134,    53,    37,    60,    37,    37,    25,   141,   142,    37,
      37,    68,    37,   138,    32,   138,    73,    74,    75,    76,
      77,    78,    79,    41,    42,    43,    44,    45,    37,    86,
      37,    88,     4,    51,    52,     6,     7,     8,     9,    10,
      11,    12,    13,    14,   138,   138,   103,   138,    54,    95,
     138,    25,   109,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    37,   138,   138,    87,
       4,    89,   138,     4,   138,    93,   138,   134,     4,    97,
      98,     4,   138,   138,   141,   142,   204,   138,   231,   134,
      15,   109,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
     675,   129,    46,    47,   138,   133,   138,   138,   544,   137,
     642,   139,    56,    57,    58,    59,   625,    61,    62,   669,
     715,   660,   512,   688,   543,    69,    70,    71,    72,   411,
     667,   475,     4,    26,    96,    -1,    80,    81,    82,    83,
      84,    85,   228,   536,    -1,    -1,    90,    91,    92,    -1,
      -1,    -1,    -1,    97,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    36,   109,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    46,    47,    -1,   121,    -1,    -1,
      -1,    -1,    -1,   127,    56,    57,    58,    59,    -1,    61,
      62,    -1,    -1,   137,    -1,    -1,    -1,    69,    70,    71,
      72,   136,    -1,   138,    -1,    -1,    -1,    -1,    80,    81,
      82,    83,    84,    85,    -1,    -1,    -1,    -1,    90,    91,
      92,    -1,    -1,    -1,    -1,    97,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,   121,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    15,   137,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    15,    -1,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    -1,    15,   138,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    15,   138,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    15,   138,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    15,   138,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    15,   138,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    15,   138,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    -1,    -1,    15,   138,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   138,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   138,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   138,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   138,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    15,   138,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    -1,    -1,
      15,   138,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   138,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   138,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   138,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   138,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   138,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   136,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      15,   136,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      -1,   136,    -1,    -1,    -1,    -1,     4,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   136,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,   136,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      -1,   136,    36,    61,    38,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,   136,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,
      -1,   136,   100,   101,   102,   103,   104,   105,   106,   107,
     108,    -1,   110,   111,    -1,   113,   114,   115,    -1,    -1,
      94,    -1,   120,   121,   122,    -1,   100,   101,   102,   103,
     104,    -1,   106,   107,   108,    -1,   110,   111,    -1,   113,
     114,   115,    -1,    -1,    -1,    -1,   120,   121,   122,    15,
      -1,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    -1,
      -1,    37,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    15,    -1,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   117,   118,   119,   128,   135,   144,   160,   161,   148,
     149,   146,   147,   253,   254,   248,   249,     0,   162,   150,
       4,    53,   123,   257,   258,    53,   250,   251,     4,    46,
      47,    56,    57,    58,    59,    61,    62,    69,    70,    71,
      72,    80,    81,    82,    83,    84,    85,    90,    91,    92,
      97,    98,   109,   121,   137,   163,   170,   172,   190,   192,
     201,   202,   204,   206,   240,   255,   256,     4,    38,    61,
      94,   100,   101,   102,   103,   104,   106,   107,   108,   110,
     111,   113,   114,   115,   120,   121,   122,   151,     6,     4,
     121,   124,   125,   126,   260,   261,    53,   258,   125,   126,
     252,   261,   251,     6,     7,     8,     9,    10,    11,    12,
      13,    14,   188,    53,    53,    37,    37,     4,   145,    53,
      37,    37,    37,    37,    37,    37,    37,    37,    37,   164,
      37,    37,    37,   173,    37,   136,   137,   189,   127,     4,
     145,     4,     3,     4,    30,    31,    37,    38,    39,    48,
      49,    50,    55,    60,    68,    73,    74,    75,    76,    77,
      78,    79,    86,    88,   103,   109,   134,   141,   142,   210,
       4,     4,   155,     4,   154,   153,     4,     4,     4,   210,
       4,     3,     4,   156,   157,   158,     4,   105,   210,     4,
      16,    16,    54,   137,   260,    54,   137,   208,   209,   208,
     171,   241,     4,     4,   165,     4,   194,     4,   145,   145,
       4,   145,   138,   145,   203,   205,     4,   207,     4,     5,
      99,   166,    37,   145,     4,     4,    37,   157,    53,     6,
     136,   152,     6,   136,   210,   210,   210,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    37,    37,
      37,    37,    37,    37,    37,    37,   210,   210,    15,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,   136,     6,   136,
     210,   136,   136,     4,   136,     6,   136,   210,   136,   136,
       4,   159,    53,   261,   261,   137,     4,   121,   124,   125,
     126,    54,   137,   210,     4,    54,    67,    91,   172,   216,
       4,    54,   242,   136,   138,   138,   162,   195,   193,   138,
     138,   138,   138,   138,   136,   138,   191,   138,   191,   136,
     207,   138,    37,     4,     5,    99,   136,   138,   166,   138,
       6,     6,   210,   138,   257,   210,   210,   150,   210,   210,
     138,   210,   210,   210,   210,   210,     4,     4,   210,     4,
       4,     4,     4,   210,   210,     4,     4,   210,   210,     4,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,     4,     4,     4,   210,   210,     3,     4,     4,
       4,   136,   263,   137,   137,     4,   123,   137,   259,   217,
     222,   227,   243,     4,    36,    37,   198,    54,   136,   194,
     145,   145,   207,   167,    37,     4,     5,    99,   138,   208,
     208,   136,    54,    36,   136,   138,   138,   136,   136,   138,
     136,   138,   138,   138,   138,   138,   138,   136,   136,   138,
     138,   138,   136,   138,    16,     4,   261,   126,    53,   123,
     137,    37,    40,   210,   230,   231,    16,   210,   232,   231,
     210,   245,   136,     4,   141,   199,   200,    16,   194,   166,
     169,    37,   138,   138,     4,   210,   210,   210,   210,   210,
     210,     4,   210,   137,   264,    16,   262,    63,    64,    65,
      66,    67,   138,   229,    37,    37,   230,    16,    95,   212,
      16,    85,   233,   228,     4,    95,   246,     4,     4,   138,
     200,    86,   196,   138,   166,   168,   138,   138,   138,   138,
     138,   138,   138,   138,    54,   261,   261,   138,   210,    16,
      37,    38,   213,   212,    53,    37,   247,    37,   244,   138,
       6,   191,   138,   166,   137,   264,   138,   210,    37,    96,
     214,   214,   171,   210,   246,   210,   137,   208,    88,   197,
     138,    54,    39,   230,   138,   210,    37,   218,   223,    54,
     138,   138,     6,    37,    16,   138,   210,   130,   131,   132,
     215,    53,   246,   208,   210,   138,    53,   224,   138,   219,
     236,   230,     4,    15,    32,    41,    42,    43,    44,    45,
      51,    52,    87,    89,    93,   109,   129,   133,   137,   139,
     174,   175,   178,   179,   181,   183,   184,   185,   190,     4,
      54,    16,    37,    37,    37,   182,    37,    37,     4,    51,
     175,   177,    37,   181,    54,    37,   189,   237,   225,    51,
      52,    93,   133,   174,    51,    52,   174,   186,   208,    37,
     180,   174,   176,    37,   140,   191,   177,   220,   208,    53,
      25,   234,    37,    37,   138,    37,   138,    37,    37,   138,
     138,   210,     4,   178,   138,   174,   175,   138,   234,   138,
     184,     4,    95,   211,   174,   174,   176,   174,   174,   136,
     138,   174,   211,    54,    25,   235,   138,   138,   138,   138,
     138,     4,   235,   238,     4,     6,    16,   187,   138,   138,
     174,   138,   138,   138,   187,   235,   186,     4,   226,   138,
     189,   221,   187,   191,   191,   239,   191
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 8:
#line 176 "ldgram.y"
    { ldlex_defsym(); }
    break;

  case 9:
#line 178 "ldgram.y"
    {
		  ldlex_popstate();
		  lang_add_assignment(exp_assop((yyvsp[(3) - (4)].token),(yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)));
		}
    break;

  case 10:
#line 186 "ldgram.y"
    {
		  ldlex_mri_script ();
		  PUSH_ERROR (_("MRI style script"));
		}
    break;

  case 11:
#line 191 "ldgram.y"
    {
		  ldlex_popstate ();
		  mri_draw_tree ();
		  POP_ERROR ();
		}
    break;

  case 16:
#line 206 "ldgram.y"
    {
			einfo(_("%P%F: unrecognised keyword in MRI style script '%s'\n"),(yyvsp[(1) - (1)].name));
			}
    break;

  case 17:
#line 209 "ldgram.y"
    {
			config.map_filename = "-";
			}
    break;

  case 20:
#line 215 "ldgram.y"
    { mri_public((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree)); }
    break;

  case 21:
#line 217 "ldgram.y"
    { mri_public((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree)); }
    break;

  case 22:
#line 219 "ldgram.y"
    { mri_public((yyvsp[(2) - (3)].name), (yyvsp[(3) - (3)].etree)); }
    break;

  case 23:
#line 221 "ldgram.y"
    { mri_format((yyvsp[(2) - (2)].name)); }
    break;

  case 24:
#line 223 "ldgram.y"
    { mri_output_section((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree));}
    break;

  case 25:
#line 225 "ldgram.y"
    { mri_output_section((yyvsp[(2) - (3)].name), (yyvsp[(3) - (3)].etree));}
    break;

  case 26:
#line 227 "ldgram.y"
    { mri_output_section((yyvsp[(2) - (4)].name), (yyvsp[(4) - (4)].etree));}
    break;

  case 27:
#line 229 "ldgram.y"
    { mri_align((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 28:
#line 231 "ldgram.y"
    { mri_align((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 29:
#line 233 "ldgram.y"
    { mri_alignmod((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 30:
#line 235 "ldgram.y"
    { mri_alignmod((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].etree)); }
    break;

  case 33:
#line 239 "ldgram.y"
    { mri_name((yyvsp[(2) - (2)].name)); }
    break;

  case 34:
#line 241 "ldgram.y"
    { mri_alias((yyvsp[(2) - (4)].name),(yyvsp[(4) - (4)].name),0);}
    break;

  case 35:
#line 243 "ldgram.y"
    { mri_alias ((yyvsp[(2) - (4)].name), 0, (int) (yyvsp[(4) - (4)].bigint).integer); }
    break;

  case 36:
#line 245 "ldgram.y"
    { mri_base((yyvsp[(2) - (2)].etree)); }
    break;

  case 37:
#line 247 "ldgram.y"
    { mri_truncate ((unsigned int) (yyvsp[(2) - (2)].bigint).integer); }
    break;

  case 40:
#line 251 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file((yyvsp[(2) - (2)].name)); }
    break;

  case 41:
#line 253 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 42:
#line 255 "ldgram.y"
    { lang_add_entry ((yyvsp[(2) - (2)].name), FALSE); }
    break;

  case 44:
#line 260 "ldgram.y"
    { mri_order((yyvsp[(3) - (3)].name)); }
    break;

  case 45:
#line 261 "ldgram.y"
    { mri_order((yyvsp[(2) - (2)].name)); }
    break;

  case 47:
#line 267 "ldgram.y"
    { mri_load((yyvsp[(1) - (1)].name)); }
    break;

  case 48:
#line 268 "ldgram.y"
    { mri_load((yyvsp[(3) - (3)].name)); }
    break;

  case 49:
#line 273 "ldgram.y"
    { mri_only_load((yyvsp[(1) - (1)].name)); }
    break;

  case 50:
#line 275 "ldgram.y"
    { mri_only_load((yyvsp[(3) - (3)].name)); }
    break;

  case 51:
#line 279 "ldgram.y"
    { (yyval.name) = NULL; }
    break;

  case 54:
#line 286 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 55:
#line 288 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 56:
#line 292 "ldgram.y"
    { ldlang_add_undef ((yyvsp[(1) - (1)].name)); }
    break;

  case 57:
#line 294 "ldgram.y"
    { ldlang_add_undef ((yyvsp[(2) - (2)].name)); }
    break;

  case 58:
#line 296 "ldgram.y"
    { ldlang_add_undef ((yyvsp[(3) - (3)].name)); }
    break;

  case 59:
#line 300 "ldgram.y"
    { ldlex_both(); }
    break;

  case 60:
#line 302 "ldgram.y"
    { ldlex_popstate(); }
    break;

  case 73:
#line 323 "ldgram.y"
    { lang_add_target((yyvsp[(3) - (4)].name)); }
    break;

  case 74:
#line 325 "ldgram.y"
    { ldfile_add_library_path ((yyvsp[(3) - (4)].name), FALSE); }
    break;

  case 75:
#line 327 "ldgram.y"
    { lang_add_output((yyvsp[(3) - (4)].name), 1); }
    break;

  case 76:
#line 329 "ldgram.y"
    { lang_add_output_format ((yyvsp[(3) - (4)].name), (char *) NULL,
					    (char *) NULL, 1); }
    break;

  case 77:
#line 332 "ldgram.y"
    { lang_add_output_format ((yyvsp[(3) - (8)].name), (yyvsp[(5) - (8)].name), (yyvsp[(7) - (8)].name), 1); }
    break;

  case 78:
#line 334 "ldgram.y"
    { ldfile_set_output_arch ((yyvsp[(3) - (4)].name), bfd_arch_unknown); }
    break;

  case 79:
#line 336 "ldgram.y"
    { command_line.force_common_definition = TRUE ; }
    break;

  case 80:
#line 338 "ldgram.y"
    { command_line.inhibit_common_definition = TRUE ; }
    break;

  case 82:
#line 341 "ldgram.y"
    { lang_enter_group (); }
    break;

  case 83:
#line 343 "ldgram.y"
    { lang_leave_group (); }
    break;

  case 84:
#line 345 "ldgram.y"
    { lang_add_map((yyvsp[(3) - (4)].name)); }
    break;

  case 85:
#line 347 "ldgram.y"
    { ldlex_script (); ldfile_open_command_file((yyvsp[(2) - (2)].name)); }
    break;

  case 86:
#line 349 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 87:
#line 351 "ldgram.y"
    {
		  lang_add_nocrossref ((yyvsp[(3) - (4)].nocrossref));
		}
    break;

  case 89:
#line 359 "ldgram.y"
    { lang_add_input_file((yyvsp[(1) - (1)].name),lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 90:
#line 362 "ldgram.y"
    { lang_add_input_file((yyvsp[(3) - (3)].name),lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 91:
#line 365 "ldgram.y"
    { lang_add_input_file((yyvsp[(2) - (2)].name),lang_input_file_is_search_file_enum,
				 (char *)NULL); }
    break;

  case 92:
#line 368 "ldgram.y"
    { lang_add_input_file((yyvsp[(1) - (1)].name),lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 93:
#line 371 "ldgram.y"
    { lang_add_input_file((yyvsp[(3) - (3)].name),lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 94:
#line 374 "ldgram.y"
    { lang_add_input_file((yyvsp[(2) - (2)].name),lang_input_file_is_l_enum,
				 (char *)NULL); }
    break;

  case 95:
#line 377 "ldgram.y"
    { (yyval.integer) = as_needed; as_needed = TRUE; }
    break;

  case 96:
#line 379 "ldgram.y"
    { as_needed = (yyvsp[(3) - (5)].integer); }
    break;

  case 97:
#line 381 "ldgram.y"
    { (yyval.integer) = as_needed; as_needed = TRUE; }
    break;

  case 98:
#line 383 "ldgram.y"
    { as_needed = (yyvsp[(5) - (7)].integer); }
    break;

  case 99:
#line 385 "ldgram.y"
    { (yyval.integer) = as_needed; as_needed = TRUE; }
    break;

  case 100:
#line 387 "ldgram.y"
    { as_needed = (yyvsp[(4) - (6)].integer); }
    break;

  case 105:
#line 402 "ldgram.y"
    { lang_add_entry ((yyvsp[(3) - (4)].name), FALSE); }
    break;

  case 107:
#line 404 "ldgram.y"
    {ldlex_expression ();}
    break;

  case 108:
#line 405 "ldgram.y"
    { ldlex_popstate ();
		  lang_add_assignment (exp_assert ((yyvsp[(4) - (7)].etree), (yyvsp[(6) - (7)].name))); }
    break;

  case 109:
#line 413 "ldgram.y"
    {
			  (yyval.cname) = (yyvsp[(1) - (1)].name);
			}
    break;

  case 110:
#line 417 "ldgram.y"
    {
			  (yyval.cname) = "*";
			}
    break;

  case 111:
#line 421 "ldgram.y"
    {
			  (yyval.cname) = "?";
			}
    break;

  case 112:
#line 428 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(1) - (1)].cname);
			  (yyval.wildcard).sorted = none;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 113:
#line 434 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (5)].cname);
			  (yyval.wildcard).sorted = none;
			  (yyval.wildcard).exclude_name_list = (yyvsp[(3) - (5)].name_list);
			}
    break;

  case 114:
#line 440 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(3) - (4)].cname);
			  (yyval.wildcard).sorted = by_name;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 115:
#line 446 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(3) - (4)].cname);
			  (yyval.wildcard).sorted = by_alignment;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 116:
#line 452 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_name_alignment;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 117:
#line 458 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_name;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 118:
#line 464 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_alignment_name;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 119:
#line 470 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(5) - (7)].cname);
			  (yyval.wildcard).sorted = by_alignment;
			  (yyval.wildcard).exclude_name_list = NULL;
			}
    break;

  case 120:
#line 476 "ldgram.y"
    {
			  (yyval.wildcard).name = (yyvsp[(7) - (8)].cname);
			  (yyval.wildcard).sorted = by_name;
			  (yyval.wildcard).exclude_name_list = (yyvsp[(5) - (8)].name_list);
			}
    break;

  case 121:
#line 485 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = (yyvsp[(2) - (2)].cname);
			  tmp->next = (yyvsp[(1) - (2)].name_list);
			  (yyval.name_list) = tmp;
			}
    break;

  case 122:
#line 494 "ldgram.y"
    {
			  struct name_list *tmp;
			  tmp = (struct name_list *) xmalloc (sizeof *tmp);
			  tmp->name = (yyvsp[(1) - (1)].cname);
			  tmp->next = NULL;
			  (yyval.name_list) = tmp;
			}
    break;

  case 123:
#line 505 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = (yyvsp[(1) - (3)].wildcard_list);
			  tmp->spec = (yyvsp[(3) - (3)].wildcard);
			  (yyval.wildcard_list) = tmp;
			}
    break;

  case 124:
#line 514 "ldgram.y"
    {
			  struct wildcard_list *tmp;
			  tmp = (struct wildcard_list *) xmalloc (sizeof *tmp);
			  tmp->next = NULL;
			  tmp->spec = (yyvsp[(1) - (1)].wildcard);
			  (yyval.wildcard_list) = tmp;
			}
    break;

  case 125:
#line 525 "ldgram.y"
    {
			  struct wildcard_spec tmp;
			  tmp.name = (yyvsp[(1) - (1)].name);
			  tmp.exclude_name_list = NULL;
			  tmp.sorted = none;
			  lang_add_wild (&tmp, NULL, ldgram_had_keep);
			}
    break;

  case 126:
#line 533 "ldgram.y"
    {
			  lang_add_wild (NULL, (yyvsp[(2) - (3)].wildcard_list), ldgram_had_keep);
			}
    break;

  case 127:
#line 537 "ldgram.y"
    {
			  lang_add_wild (&(yyvsp[(1) - (4)].wildcard), (yyvsp[(3) - (4)].wildcard_list), ldgram_had_keep);
			}
    break;

  case 129:
#line 545 "ldgram.y"
    { ldgram_had_keep = TRUE; }
    break;

  case 130:
#line 547 "ldgram.y"
    { ldgram_had_keep = FALSE; }
    break;

  case 132:
#line 553 "ldgram.y"
    {
 		lang_add_attribute(lang_object_symbols_statement_enum);
	      	}
    break;

  case 134:
#line 558 "ldgram.y"
    {

		  lang_add_attribute(lang_constructors_statement_enum);
		}
    break;

  case 135:
#line 563 "ldgram.y"
    {
		  constructors_sorted = TRUE;
		  lang_add_attribute (lang_constructors_statement_enum);
		}
    break;

  case 137:
#line 569 "ldgram.y"
    {
			  lang_add_data ((int) (yyvsp[(1) - (4)].integer), (yyvsp[(3) - (4)].etree));
			}
    break;

  case 138:
#line 574 "ldgram.y"
    {
			  lang_add_fill ((yyvsp[(3) - (4)].fill));
			}
    break;

  case 139:
#line 577 "ldgram.y"
    {ldlex_expression ();}
    break;

  case 140:
#line 578 "ldgram.y"
    { ldlex_popstate ();
			  lang_add_assignment (exp_assert ((yyvsp[(4) - (8)].etree), (yyvsp[(6) - (8)].name))); }
    break;

  case 145:
#line 594 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 146:
#line 596 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 147:
#line 598 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 148:
#line 600 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 149:
#line 602 "ldgram.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].token); }
    break;

  case 150:
#line 607 "ldgram.y"
    {
		  (yyval.fill) = exp_get_fill ((yyvsp[(1) - (1)].etree), 0, "fill value");
		}
    break;

  case 151:
#line 614 "ldgram.y"
    { (yyval.fill) = (yyvsp[(2) - (2)].fill); }
    break;

  case 152:
#line 615 "ldgram.y"
    { (yyval.fill) = (fill_type *) 0; }
    break;

  case 153:
#line 620 "ldgram.y"
    { (yyval.token) = '+'; }
    break;

  case 154:
#line 622 "ldgram.y"
    { (yyval.token) = '-'; }
    break;

  case 155:
#line 624 "ldgram.y"
    { (yyval.token) = '*'; }
    break;

  case 156:
#line 626 "ldgram.y"
    { (yyval.token) = '/'; }
    break;

  case 157:
#line 628 "ldgram.y"
    { (yyval.token) = LSHIFT; }
    break;

  case 158:
#line 630 "ldgram.y"
    { (yyval.token) = RSHIFT; }
    break;

  case 159:
#line 632 "ldgram.y"
    { (yyval.token) = '&'; }
    break;

  case 160:
#line 634 "ldgram.y"
    { (yyval.token) = '|'; }
    break;

  case 163:
#line 644 "ldgram.y"
    {
		  lang_add_assignment (exp_assop ((yyvsp[(2) - (3)].token), (yyvsp[(1) - (3)].name), (yyvsp[(3) - (3)].etree)));
		}
    break;

  case 164:
#line 648 "ldgram.y"
    {
		  lang_add_assignment (exp_assop ('=', (yyvsp[(1) - (3)].name),
						  exp_binop ((yyvsp[(2) - (3)].token),
							     exp_nameop (NAME,
									 (yyvsp[(1) - (3)].name)),
							     (yyvsp[(3) - (3)].etree))));
		}
    break;

  case 165:
#line 656 "ldgram.y"
    {
		  lang_add_assignment (exp_provide ((yyvsp[(3) - (6)].name), (yyvsp[(5) - (6)].etree), FALSE));
		}
    break;

  case 166:
#line 660 "ldgram.y"
    {
		  lang_add_assignment (exp_provide ((yyvsp[(3) - (6)].name), (yyvsp[(5) - (6)].etree), TRUE));
		}
    break;

  case 173:
#line 682 "ldgram.y"
    { region = lang_memory_region_lookup ((yyvsp[(1) - (1)].name), TRUE); }
    break;

  case 174:
#line 685 "ldgram.y"
    {}
    break;

  case 175:
#line 690 "ldgram.y"
    {
		  region->origin = exp_get_vma ((yyvsp[(3) - (3)].etree), 0, "origin");
		  region->current = region->origin;
		}
    break;

  case 176:
#line 698 "ldgram.y"
    {
		  region->length = exp_get_vma ((yyvsp[(3) - (3)].etree), -1, "length");
		}
    break;

  case 177:
#line 705 "ldgram.y"
    { /* dummy action to avoid bison 1.25 error message */ }
    break;

  case 181:
#line 716 "ldgram.y"
    { lang_set_flags (region, (yyvsp[(1) - (1)].name), 0); }
    break;

  case 182:
#line 718 "ldgram.y"
    { lang_set_flags (region, (yyvsp[(2) - (2)].name), 1); }
    break;

  case 183:
#line 723 "ldgram.y"
    { lang_startup((yyvsp[(3) - (4)].name)); }
    break;

  case 185:
#line 729 "ldgram.y"
    { ldemul_hll((char *)NULL); }
    break;

  case 186:
#line 734 "ldgram.y"
    { ldemul_hll((yyvsp[(3) - (3)].name)); }
    break;

  case 187:
#line 736 "ldgram.y"
    { ldemul_hll((yyvsp[(1) - (1)].name)); }
    break;

  case 189:
#line 744 "ldgram.y"
    { ldemul_syslib((yyvsp[(3) - (3)].name)); }
    break;

  case 191:
#line 750 "ldgram.y"
    { lang_float(TRUE); }
    break;

  case 192:
#line 752 "ldgram.y"
    { lang_float(FALSE); }
    break;

  case 193:
#line 757 "ldgram.y"
    {
		  (yyval.nocrossref) = NULL;
		}
    break;

  case 194:
#line 761 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = (yyvsp[(1) - (2)].name);
		  n->next = (yyvsp[(2) - (2)].nocrossref);
		  (yyval.nocrossref) = n;
		}
    break;

  case 195:
#line 770 "ldgram.y"
    {
		  struct lang_nocrossref *n;

		  n = (struct lang_nocrossref *) xmalloc (sizeof *n);
		  n->name = (yyvsp[(1) - (3)].name);
		  n->next = (yyvsp[(3) - (3)].nocrossref);
		  (yyval.nocrossref) = n;
		}
    break;

  case 196:
#line 780 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 197:
#line 782 "ldgram.y"
    { ldlex_popstate (); (yyval.etree)=(yyvsp[(2) - (2)].etree);}
    break;

  case 198:
#line 787 "ldgram.y"
    { (yyval.etree) = exp_unop ('-', (yyvsp[(2) - (2)].etree)); }
    break;

  case 199:
#line 789 "ldgram.y"
    { (yyval.etree) = (yyvsp[(2) - (3)].etree); }
    break;

  case 200:
#line 791 "ldgram.y"
    { (yyval.etree) = exp_unop ((int) (yyvsp[(1) - (4)].integer),(yyvsp[(3) - (4)].etree)); }
    break;

  case 201:
#line 793 "ldgram.y"
    { (yyval.etree) = exp_unop ('!', (yyvsp[(2) - (2)].etree)); }
    break;

  case 202:
#line 795 "ldgram.y"
    { (yyval.etree) = (yyvsp[(2) - (2)].etree); }
    break;

  case 203:
#line 797 "ldgram.y"
    { (yyval.etree) = exp_unop ('~', (yyvsp[(2) - (2)].etree));}
    break;

  case 204:
#line 800 "ldgram.y"
    { (yyval.etree) = exp_binop ('*', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 205:
#line 802 "ldgram.y"
    { (yyval.etree) = exp_binop ('/', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 206:
#line 804 "ldgram.y"
    { (yyval.etree) = exp_binop ('%', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 207:
#line 806 "ldgram.y"
    { (yyval.etree) = exp_binop ('+', (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 208:
#line 808 "ldgram.y"
    { (yyval.etree) = exp_binop ('-' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 209:
#line 810 "ldgram.y"
    { (yyval.etree) = exp_binop (LSHIFT , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 210:
#line 812 "ldgram.y"
    { (yyval.etree) = exp_binop (RSHIFT , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 211:
#line 814 "ldgram.y"
    { (yyval.etree) = exp_binop (EQ , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 212:
#line 816 "ldgram.y"
    { (yyval.etree) = exp_binop (NE , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 213:
#line 818 "ldgram.y"
    { (yyval.etree) = exp_binop (LE , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 214:
#line 820 "ldgram.y"
    { (yyval.etree) = exp_binop (GE , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 215:
#line 822 "ldgram.y"
    { (yyval.etree) = exp_binop ('<' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 216:
#line 824 "ldgram.y"
    { (yyval.etree) = exp_binop ('>' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 217:
#line 826 "ldgram.y"
    { (yyval.etree) = exp_binop ('&' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 218:
#line 828 "ldgram.y"
    { (yyval.etree) = exp_binop ('^' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 219:
#line 830 "ldgram.y"
    { (yyval.etree) = exp_binop ('|' , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 220:
#line 832 "ldgram.y"
    { (yyval.etree) = exp_trinop ('?' , (yyvsp[(1) - (5)].etree), (yyvsp[(3) - (5)].etree), (yyvsp[(5) - (5)].etree)); }
    break;

  case 221:
#line 834 "ldgram.y"
    { (yyval.etree) = exp_binop (ANDAND , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 222:
#line 836 "ldgram.y"
    { (yyval.etree) = exp_binop (OROR , (yyvsp[(1) - (3)].etree), (yyvsp[(3) - (3)].etree)); }
    break;

  case 223:
#line 838 "ldgram.y"
    { (yyval.etree) = exp_nameop (DEFINED, (yyvsp[(3) - (4)].name)); }
    break;

  case 224:
#line 840 "ldgram.y"
    { (yyval.etree) = exp_bigintop ((yyvsp[(1) - (1)].bigint).integer, (yyvsp[(1) - (1)].bigint).str); }
    break;

  case 225:
#line 842 "ldgram.y"
    { (yyval.etree) = exp_nameop (SIZEOF_HEADERS,0); }
    break;

  case 226:
#line 845 "ldgram.y"
    { (yyval.etree) = exp_nameop (ALIGNOF,(yyvsp[(3) - (4)].name)); }
    break;

  case 227:
#line 847 "ldgram.y"
    { (yyval.etree) = exp_nameop (SIZEOF,(yyvsp[(3) - (4)].name)); }
    break;

  case 228:
#line 849 "ldgram.y"
    { (yyval.etree) = exp_nameop (ADDR,(yyvsp[(3) - (4)].name)); }
    break;

  case 229:
#line 851 "ldgram.y"
    { (yyval.etree) = exp_nameop (LOADADDR,(yyvsp[(3) - (4)].name)); }
    break;

  case 230:
#line 853 "ldgram.y"
    { (yyval.etree) = exp_nameop (CONSTANT,(yyvsp[(3) - (4)].name)); }
    break;

  case 231:
#line 855 "ldgram.y"
    { (yyval.etree) = exp_unop (ABSOLUTE, (yyvsp[(3) - (4)].etree)); }
    break;

  case 232:
#line 857 "ldgram.y"
    { (yyval.etree) = exp_unop (ALIGN_K,(yyvsp[(3) - (4)].etree)); }
    break;

  case 233:
#line 859 "ldgram.y"
    { (yyval.etree) = exp_binop (ALIGN_K,(yyvsp[(3) - (6)].etree),(yyvsp[(5) - (6)].etree)); }
    break;

  case 234:
#line 861 "ldgram.y"
    { (yyval.etree) = exp_binop (DATA_SEGMENT_ALIGN, (yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].etree)); }
    break;

  case 235:
#line 863 "ldgram.y"
    { (yyval.etree) = exp_binop (DATA_SEGMENT_RELRO_END, (yyvsp[(5) - (6)].etree), (yyvsp[(3) - (6)].etree)); }
    break;

  case 236:
#line 865 "ldgram.y"
    { (yyval.etree) = exp_unop (DATA_SEGMENT_END, (yyvsp[(3) - (4)].etree)); }
    break;

  case 237:
#line 867 "ldgram.y"
    { /* The operands to the expression node are
			     placed in the opposite order from the way
			     in which they appear in the script as
			     that allows us to reuse more code in
			     fold_binary.  */
			  (yyval.etree) = exp_binop (SEGMENT_START,
					  (yyvsp[(5) - (6)].etree),
					  exp_nameop (NAME, (yyvsp[(3) - (6)].name))); }
    break;

  case 238:
#line 876 "ldgram.y"
    { (yyval.etree) = exp_unop (ALIGN_K,(yyvsp[(3) - (4)].etree)); }
    break;

  case 239:
#line 878 "ldgram.y"
    { (yyval.etree) = exp_nameop (NAME,(yyvsp[(1) - (1)].name)); }
    break;

  case 240:
#line 880 "ldgram.y"
    { (yyval.etree) = exp_binop (MAX_K, (yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].etree) ); }
    break;

  case 241:
#line 882 "ldgram.y"
    { (yyval.etree) = exp_binop (MIN_K, (yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].etree) ); }
    break;

  case 242:
#line 884 "ldgram.y"
    { (yyval.etree) = exp_assert ((yyvsp[(3) - (6)].etree), (yyvsp[(5) - (6)].name)); }
    break;

  case 243:
#line 886 "ldgram.y"
    { (yyval.etree) = exp_nameop (ORIGIN, (yyvsp[(3) - (4)].name)); }
    break;

  case 244:
#line 888 "ldgram.y"
    { (yyval.etree) = exp_nameop (LENGTH, (yyvsp[(3) - (4)].name)); }
    break;

  case 245:
#line 893 "ldgram.y"
    { (yyval.name) = (yyvsp[(3) - (3)].name); }
    break;

  case 246:
#line 894 "ldgram.y"
    { (yyval.name) = 0; }
    break;

  case 247:
#line 898 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (4)].etree); }
    break;

  case 248:
#line 899 "ldgram.y"
    { (yyval.etree) = 0; }
    break;

  case 249:
#line 903 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (4)].etree); }
    break;

  case 250:
#line 904 "ldgram.y"
    { (yyval.etree) = 0; }
    break;

  case 251:
#line 908 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (4)].etree); }
    break;

  case 252:
#line 909 "ldgram.y"
    { (yyval.etree) = 0; }
    break;

  case 253:
#line 913 "ldgram.y"
    { (yyval.token) = ONLY_IF_RO; }
    break;

  case 254:
#line 914 "ldgram.y"
    { (yyval.token) = ONLY_IF_RW; }
    break;

  case 255:
#line 915 "ldgram.y"
    { (yyval.token) = SPECIAL; }
    break;

  case 256:
#line 916 "ldgram.y"
    { (yyval.token) = 0; }
    break;

  case 257:
#line 919 "ldgram.y"
    { ldlex_expression(); }
    break;

  case 258:
#line 923 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 259:
#line 926 "ldgram.y"
    {
			  lang_enter_output_section_statement((yyvsp[(1) - (9)].name), (yyvsp[(3) - (9)].etree),
							      sectype,
							      (yyvsp[(5) - (9)].etree), (yyvsp[(6) - (9)].etree), (yyvsp[(4) - (9)].etree), (yyvsp[(8) - (9)].token));
			}
    break;

  case 260:
#line 932 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 261:
#line 934 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_leave_output_section_statement ((yyvsp[(17) - (17)].fill), (yyvsp[(14) - (17)].name), (yyvsp[(16) - (17)].section_phdr), (yyvsp[(15) - (17)].name));
		}
    break;

  case 262:
#line 939 "ldgram.y"
    {}
    break;

  case 263:
#line 941 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 264:
#line 943 "ldgram.y"
    { ldlex_popstate (); ldlex_script (); }
    break;

  case 265:
#line 945 "ldgram.y"
    {
			  lang_enter_overlay ((yyvsp[(3) - (8)].etree), (yyvsp[(6) - (8)].etree));
			}
    break;

  case 266:
#line 950 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 267:
#line 952 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay ((yyvsp[(5) - (16)].etree), (int) (yyvsp[(4) - (16)].integer),
					      (yyvsp[(16) - (16)].fill), (yyvsp[(13) - (16)].name), (yyvsp[(15) - (16)].section_phdr), (yyvsp[(14) - (16)].name));
			}
    break;

  case 269:
#line 962 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 270:
#line 964 "ldgram.y"
    {
		  ldlex_popstate ();
		  lang_add_assignment (exp_assop ('=', ".", (yyvsp[(3) - (3)].etree)));
		}
    break;

  case 272:
#line 972 "ldgram.y"
    { sectype = noload_section; }
    break;

  case 273:
#line 973 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 274:
#line 974 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 275:
#line 975 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 276:
#line 976 "ldgram.y"
    { sectype = noalloc_section; }
    break;

  case 278:
#line 981 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 279:
#line 982 "ldgram.y"
    { sectype = normal_section; }
    break;

  case 280:
#line 986 "ldgram.y"
    { (yyval.etree) = (yyvsp[(1) - (3)].etree); }
    break;

  case 281:
#line 987 "ldgram.y"
    { (yyval.etree) = (etree_type *)NULL;  }
    break;

  case 282:
#line 992 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (6)].etree); }
    break;

  case 283:
#line 994 "ldgram.y"
    { (yyval.etree) = (yyvsp[(3) - (10)].etree); }
    break;

  case 284:
#line 998 "ldgram.y"
    { (yyval.etree) = (yyvsp[(1) - (2)].etree); }
    break;

  case 285:
#line 999 "ldgram.y"
    { (yyval.etree) = (etree_type *) NULL;  }
    break;

  case 286:
#line 1004 "ldgram.y"
    { (yyval.integer) = 0; }
    break;

  case 287:
#line 1006 "ldgram.y"
    { (yyval.integer) = 1; }
    break;

  case 288:
#line 1011 "ldgram.y"
    { (yyval.name) = (yyvsp[(2) - (2)].name); }
    break;

  case 289:
#line 1012 "ldgram.y"
    { (yyval.name) = DEFAULT_MEMORY_REGION; }
    break;

  case 290:
#line 1017 "ldgram.y"
    {
		  (yyval.section_phdr) = NULL;
		}
    break;

  case 291:
#line 1021 "ldgram.y"
    {
		  struct lang_output_section_phdr_list *n;

		  n = ((struct lang_output_section_phdr_list *)
		       xmalloc (sizeof *n));
		  n->name = (yyvsp[(3) - (3)].name);
		  n->used = FALSE;
		  n->next = (yyvsp[(1) - (3)].section_phdr);
		  (yyval.section_phdr) = n;
		}
    break;

  case 293:
#line 1037 "ldgram.y"
    {
			  ldlex_script ();
			  lang_enter_overlay_section ((yyvsp[(2) - (2)].name));
			}
    break;

  case 294:
#line 1042 "ldgram.y"
    { ldlex_popstate (); ldlex_expression (); }
    break;

  case 295:
#line 1044 "ldgram.y"
    {
			  ldlex_popstate ();
			  lang_leave_overlay_section ((yyvsp[(9) - (9)].fill), (yyvsp[(8) - (9)].section_phdr));
			}
    break;

  case 300:
#line 1061 "ldgram.y"
    { ldlex_expression (); }
    break;

  case 301:
#line 1062 "ldgram.y"
    { ldlex_popstate (); }
    break;

  case 302:
#line 1064 "ldgram.y"
    {
		  lang_new_phdr ((yyvsp[(1) - (6)].name), (yyvsp[(3) - (6)].etree), (yyvsp[(4) - (6)].phdr).filehdr, (yyvsp[(4) - (6)].phdr).phdrs, (yyvsp[(4) - (6)].phdr).at,
				 (yyvsp[(4) - (6)].phdr).flags);
		}
    break;

  case 303:
#line 1072 "ldgram.y"
    {
		  (yyval.etree) = (yyvsp[(1) - (1)].etree);

		  if ((yyvsp[(1) - (1)].etree)->type.node_class == etree_name
		      && (yyvsp[(1) - (1)].etree)->type.node_code == NAME)
		    {
		      const char *s;
		      unsigned int i;
		      static const char * const phdr_types[] =
			{
			  "PT_NULL", "PT_LOAD", "PT_DYNAMIC",
			  "PT_INTERP", "PT_NOTE", "PT_SHLIB",
			  "PT_PHDR", "PT_TLS"
			};

		      s = (yyvsp[(1) - (1)].etree)->name.name;
		      for (i = 0;
			   i < sizeof phdr_types / sizeof phdr_types[0];
			   i++)
			if (strcmp (s, phdr_types[i]) == 0)
			  {
			    (yyval.etree) = exp_intop (i);
			    break;
			  }
		      if (i == sizeof phdr_types / sizeof phdr_types[0])
			{
			  if (strcmp (s, "PT_GNU_EH_FRAME") == 0)
			    (yyval.etree) = exp_intop (0x6474e550);
			  else if (strcmp (s, "PT_GNU_STACK") == 0)
			    (yyval.etree) = exp_intop (0x6474e551);
			  else
			    {
			      einfo (_("\
%X%P:%S: unknown phdr type `%s' (try integer literal)\n"),
				     s);
			      (yyval.etree) = exp_intop (0);
			    }
			}
		    }
		}
    break;

  case 304:
#line 1116 "ldgram.y"
    {
		  memset (&(yyval.phdr), 0, sizeof (struct phdr_info));
		}
    break;

  case 305:
#line 1120 "ldgram.y"
    {
		  (yyval.phdr) = (yyvsp[(3) - (3)].phdr);
		  if (strcmp ((yyvsp[(1) - (3)].name), "FILEHDR") == 0 && (yyvsp[(2) - (3)].etree) == NULL)
		    (yyval.phdr).filehdr = TRUE;
		  else if (strcmp ((yyvsp[(1) - (3)].name), "PHDRS") == 0 && (yyvsp[(2) - (3)].etree) == NULL)
		    (yyval.phdr).phdrs = TRUE;
		  else if (strcmp ((yyvsp[(1) - (3)].name), "FLAGS") == 0 && (yyvsp[(2) - (3)].etree) != NULL)
		    (yyval.phdr).flags = (yyvsp[(2) - (3)].etree);
		  else
		    einfo (_("%X%P:%S: PHDRS syntax error at `%s'\n"), (yyvsp[(1) - (3)].name));
		}
    break;

  case 306:
#line 1132 "ldgram.y"
    {
		  (yyval.phdr) = (yyvsp[(5) - (5)].phdr);
		  (yyval.phdr).at = (yyvsp[(3) - (5)].etree);
		}
    break;

  case 307:
#line 1140 "ldgram.y"
    {
		  (yyval.etree) = NULL;
		}
    break;

  case 308:
#line 1144 "ldgram.y"
    {
		  (yyval.etree) = (yyvsp[(2) - (3)].etree);
		}
    break;

  case 309:
#line 1150 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("dynamic list"));
		}
    break;

  case 310:
#line 1155 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 314:
#line 1172 "ldgram.y"
    {
		  lang_append_dynamic_list ((yyvsp[(1) - (2)].versyms));
		}
    break;

  case 315:
#line 1180 "ldgram.y"
    {
		  ldlex_version_file ();
		  PUSH_ERROR (_("VERSION script"));
		}
    break;

  case 316:
#line 1185 "ldgram.y"
    {
		  ldlex_popstate ();
		  POP_ERROR ();
		}
    break;

  case 317:
#line 1194 "ldgram.y"
    {
		  ldlex_version_script ();
		}
    break;

  case 318:
#line 1198 "ldgram.y"
    {
		  ldlex_popstate ();
		}
    break;

  case 321:
#line 1210 "ldgram.y"
    {
		  lang_register_vers_node (NULL, (yyvsp[(2) - (4)].versnode), NULL);
		}
    break;

  case 322:
#line 1214 "ldgram.y"
    {
		  lang_register_vers_node ((yyvsp[(1) - (5)].name), (yyvsp[(3) - (5)].versnode), NULL);
		}
    break;

  case 323:
#line 1218 "ldgram.y"
    {
		  lang_register_vers_node ((yyvsp[(1) - (6)].name), (yyvsp[(3) - (6)].versnode), (yyvsp[(5) - (6)].deflist));
		}
    break;

  case 324:
#line 1225 "ldgram.y"
    {
		  (yyval.deflist) = lang_add_vers_depend (NULL, (yyvsp[(1) - (1)].name));
		}
    break;

  case 325:
#line 1229 "ldgram.y"
    {
		  (yyval.deflist) = lang_add_vers_depend ((yyvsp[(1) - (2)].deflist), (yyvsp[(2) - (2)].name));
		}
    break;

  case 326:
#line 1236 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node (NULL, NULL);
		}
    break;

  case 327:
#line 1240 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node ((yyvsp[(1) - (2)].versyms), NULL);
		}
    break;

  case 328:
#line 1244 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node ((yyvsp[(3) - (4)].versyms), NULL);
		}
    break;

  case 329:
#line 1248 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node (NULL, (yyvsp[(3) - (4)].versyms));
		}
    break;

  case 330:
#line 1252 "ldgram.y"
    {
		  (yyval.versnode) = lang_new_vers_node ((yyvsp[(3) - (8)].versyms), (yyvsp[(7) - (8)].versyms));
		}
    break;

  case 331:
#line 1259 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, (yyvsp[(1) - (1)].name), ldgram_vers_current_lang, FALSE);
		}
    break;

  case 332:
#line 1263 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, (yyvsp[(1) - (1)].name), ldgram_vers_current_lang, TRUE);
		}
    break;

  case 333:
#line 1267 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), (yyvsp[(3) - (3)].name), ldgram_vers_current_lang, FALSE);
		}
    break;

  case 334:
#line 1271 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), (yyvsp[(3) - (3)].name), ldgram_vers_current_lang, TRUE);
		}
    break;

  case 335:
#line 1275 "ldgram.y"
    {
			  (yyval.name) = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = (yyvsp[(4) - (5)].name);
			}
    break;

  case 336:
#line 1280 "ldgram.y"
    {
			  struct bfd_elf_version_expr *pat;
			  for (pat = (yyvsp[(7) - (9)].versyms); pat->next != NULL; pat = pat->next);
			  pat->next = (yyvsp[(1) - (9)].versyms);
			  (yyval.versyms) = (yyvsp[(7) - (9)].versyms);
			  ldgram_vers_current_lang = (yyvsp[(6) - (9)].name);
			}
    break;

  case 337:
#line 1288 "ldgram.y"
    {
			  (yyval.name) = ldgram_vers_current_lang;
			  ldgram_vers_current_lang = (yyvsp[(2) - (3)].name);
			}
    break;

  case 338:
#line 1293 "ldgram.y"
    {
			  (yyval.versyms) = (yyvsp[(5) - (7)].versyms);
			  ldgram_vers_current_lang = (yyvsp[(4) - (7)].name);
			}
    break;

  case 339:
#line 1298 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 340:
#line 1302 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), "global", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 341:
#line 1306 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 342:
#line 1310 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), "local", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 343:
#line 1314 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern (NULL, "extern", ldgram_vers_current_lang, FALSE);
		}
    break;

  case 344:
#line 1318 "ldgram.y"
    {
		  (yyval.versyms) = lang_new_vers_pattern ((yyvsp[(1) - (3)].versyms), "extern", ldgram_vers_current_lang, FALSE);
		}
    break;


/* Line 1267 of yacc.c.  */
#line 4178 "ldgram.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1328 "ldgram.y"

void
yyerror(arg)
     const char *arg;
{
  if (ldfile_assumed_script)
    einfo (_("%P:%s: file format not recognized; treating as linker script\n"),
	   ldfile_input_filename);
  if (error_index > 0 && error_index < ERROR_NAME_MAX)
     einfo ("%P%F:%S: %s in %s\n", arg, error_names[error_index-1]);
  else
     einfo ("%P%F:%S: %s\n", arg);
}

