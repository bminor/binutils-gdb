/* as.h - global header file
   Copyright (C) 1987, 1990, 1991, 1992 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef GAS
#define GAS 1
/*
 * I think this stuff is largely out of date.  xoxorich.
 *
 * CAPITALISED names are #defined.
 * "lowercaseH" is #defined if "lowercase.h" has been #include-d.
 * "lowercaseT" is a typedef of "lowercase" objects.
 * "lowercaseP" is type "pointer to object of type 'lowercase'".
 * "lowercaseS" is typedef struct ... lowercaseS.
 *
 * #define DEBUG to enable all the "know" assertion tests.
 * #define SUSPECT when debugging hash code.
 * #define COMMON as "extern" for all modules except one, where you #define
 *	COMMON as "".
 * If TEST is #defined, then we are testing a module: #define COMMON as "".
 */

/* These #defines are for parameters of entire assembler. */

/* These #includes are for type definitions etc. */

#include "config.h"

#include <stdio.h>
#ifdef DEBUG
#undef NDEBUG
#endif
#include <assert.h>

#include <ansidecl.h>
#ifdef BFD_ASSEMBLER
#include <bfd.h>
#endif
#include "host.h"
#include "flonum.h"

/* Make Saber happier on obstack.h.  */
#ifdef SABER
#undef  __PTR_TO_INT
#define __PTR_TO_INT(P) ((int)(P))
#undef  __INT_TO_PTR
#define __INT_TO_PTR(P) ((char *)(P))
#endif

#ifndef __LINE__
#define __LINE__ "unknown"
#endif /* __LINE__ */

#ifndef __FILE__
#define __FILE__ "unknown"
#endif /* __FILE__ */

#ifndef __STDC__
#ifndef const
#define const
#endif
#ifndef volatile
#define volatile
#endif
#endif /* ! __STDC__ */

#if !defined (__GNUC__) && !defined (inline)
#define inline
#endif

#ifndef FOPEN_WB
#include "fopen-same.h"
#endif

#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free xfree

#define xfree free

#define BAD_CASE(val) \
{ \
      as_fatal("Case value %ld unexpected at line %d of file \"%s\"\n", \
	       (long) val, __LINE__, __FILE__); \
	   }

/* Version 2.1 of Solaris had problems with this declaration, but I
   think that bug has since been fixed.  If it causes problems on your
   system, just delete it.  */
extern char *strstr ();


/* These are assembler-wide concepts */

#ifdef BFD_ASSEMBLER
extern bfd *stdoutput;
typedef bfd_vma addressT;
typedef bfd_signed_vma offsetT;
#else
typedef unsigned long addressT;
typedef long offsetT;
#endif

/* Type of symbol value, etc.  For use in prototypes.  */
typedef addressT valueT;

#ifndef COMMON
#ifdef TEST
#define COMMON			/* declare our COMMONs storage here. */
#else
#define COMMON extern		/* our commons live elswhere */
#endif
#endif
/* COMMON now defined */

#ifdef DEBUG
#ifndef know
#define know(p) assert(p)	/* Verify our assumptions! */
#endif /* not yet defined */
#else
#define know(p)			/* know() checks are no-op.ed */
#endif

#if defined (BROKEN_ASSERT) && !defined (NDEBUG)
/* Used on machines where the "assert" macro is buggy.  (For example, on the
   RS/6000, Reiser-cpp substitution is done to put the condition into a
   string, so if the condition contains a string, parse errors result.)  If
   the condition fails, just drop core file.  */
#undef assert
#define assert(p) ((p) ? 0 : (abort (), 0))
#endif

/* input_scrub.c */

/*
 * Supplies sanitised buffers to read.c.
 * Also understands printing line-number part of error messages.
 */


/* subsegs.c     Sub-segments. Also, segment(=expression type)s.*/

#ifndef BFD_ASSEMBLER

#ifdef MANY_SEGMENTS
#include "bfd.h"
#define N_SEGMENTS 10
#define SEG_NORMAL(x) ((x) >= SEG_E0 && (x) <= SEG_E9)
#define SEG_LIST SEG_E0,SEG_E1,SEG_E2,SEG_E3,SEG_E4,SEG_E5,SEG_E6,SEG_E7,SEG_E8,SEG_E9
#define SEG_TEXT SEG_E0
#define SEG_DATA SEG_E1
#define SEG_BSS SEG_E2
#else
#define N_SEGMENTS 3
#define SEG_NORMAL(x) ((x) == SEG_TEXT || (x) == SEG_DATA || (x) == SEG_BSS)
#define SEG_LIST SEG_TEXT,SEG_DATA,SEG_BSS
#endif

typedef enum _segT
  {
    SEG_ABSOLUTE = 0,
    SEG_LIST,
    SEG_UNKNOWN,
    SEG_GOOF,			/* Only happens if AS has a logic error. */
    /* Invented so we don't crash printing */
    /* error message involving weird segment. */
    SEG_EXPR,			/* Intermediate expression values. */
    SEG_DEBUG,			/* Debug segment */
    SEG_NTV,			/* Transfert vector preload segment */
    SEG_PTV,			/* Transfert vector postload segment */
    SEG_REGISTER		/* Mythical: a register-valued expression */
  } segT;

#define SEG_MAXIMUM_ORDINAL (SEG_REGISTER)
#else
typedef asection *segT;
#define SEG_NORMAL(SEG)		((SEG) != absolute_section	\
				 && (SEG) != undefined_section	\
				 && (SEG) != reg_section	\
				 && (SEG) != expr_section)
#endif
typedef int subsegT;

/* What subseg we are accreting now? */
COMMON subsegT now_subseg;

/* Segment our instructions emit to. */
COMMON segT now_seg;

#ifdef BFD_ASSEMBLER
#define segment_name(SEG)	bfd_get_section_name (stdoutput, SEG)
#else
extern char *const seg_name[];
#define segment_name(SEG)	seg_name[(int) (SEG)]
#endif

#ifndef BFD_ASSEMBLER
extern int section_alignment[];
#endif

#ifdef BFD_ASSEMBLER
extern segT reg_section, expr_section;
/* Shouldn't these be eliminated someday?  */
extern segT text_section, data_section, bss_section;
#define absolute_section	(&bfd_abs_section)
#define undefined_section	(&bfd_und_section)
#else
#define reg_section		SEG_REGISTER
#define expr_section		SEG_EXPR
#define text_section		SEG_TEXT
#define data_section		SEG_DATA
#define bss_section		SEG_BSS
#define absolute_section	SEG_ABSOLUTE
#define undefined_section	SEG_UNKNOWN
#endif

/* relax() */

typedef enum _relax_state
  {
    /* Variable chars to be repeated fr_offset times.
       Fr_symbol unused. Used with fr_offset == 0 for a
       constant length frag. */
    rs_fill = 1,

    /* Align: Fr_offset: power of 2.  Variable chars: fill pattern. */
    rs_align,

    /* Org: Fr_offset, fr_symbol: address. 1 variable char: fill
       character. */
    rs_org,

    rs_machine_dependent

#ifndef WORKING_DOT_WORD
    /* JF: gunpoint */
      , rs_broken_word
#endif
  } relax_stateT;

/* typedef unsigned char relax_substateT; */
/* JF this is more likely to leave the end of a struct frag on an align
   boundry.  Be very careful with this.  */
typedef unsigned long relax_substateT;

/* Enough bits for address, but still an integer type.
   Could be a problem, cross-assembling for 64-bit machines.  */
typedef addressT relax_addressT;


/* frags.c */

/*
 * A code fragment (frag) is some known number of chars, followed by some
 * unknown number of chars. Typically the unknown number of chars is an
 * instruction address whose size is yet unknown. We always know the greatest
 * possible size the unknown number of chars may become, and reserve that
 * much room at the end of the frag.
 * Once created, frags do not change address during assembly.
 * We chain the frags in (a) forward-linked list(s). The object-file address
 * of the 1st char of a frag is generally not known until after relax().
 * Many things at assembly time describe an address by {object-file-address
 * of a particular frag}+offset.

 BUG: it may be smarter to have a single pointer off to various different
 notes for different frag kinds. See how code pans
 */
struct frag
{
  /* Object file address. */
  addressT fr_address;
  /* Chain forward; ascending address order.  Rooted in frch_root. */
  struct frag *fr_next;

  /* (Fixed) number of chars we know we have.  May be 0. */
  offsetT fr_fix;
  /* (Variable) number of chars after above.  May be 0. */
  offsetT fr_var;
  /* For variable-length tail. */
  struct symbol *fr_symbol;
  /* For variable-length tail. */
  offsetT fr_offset;
  /* Points to opcode low addr byte, for relaxation.  */
  char *fr_opcode;

#ifndef NO_LISTING
  struct list_info_struct *line;
#endif

  /* What state is my tail in? */
  relax_stateT fr_type;
  relax_substateT fr_subtype;

  /* These are needed only on the NS32K machines */
  char fr_pcrel_adjust;
  char fr_bsr;

  /* Chars begin here.
     One day we will compile fr_literal[0]. */
  char fr_literal[1];
};

#define SIZEOF_STRUCT_FRAG \
((int)zero_address_frag.fr_literal-(int)&zero_address_frag)
/* We want to say fr_literal[0] above. */

typedef struct frag fragS;

/* Current frag we are building.  This frag is incomplete.  It is, however,
   included in frchain_now.  The fr_fix field is bogus; instead, use:
   obstack_next_free(&frags)-frag_now->fr_literal.  */
COMMON fragS *frag_now;
#define frag_now_fix() ((char*)obstack_next_free (&frags) - frag_now->fr_literal)

/* For foreign-segment symbol fixups. */
COMMON fragS zero_address_frag;
/* For local common (N_BSS segment) fixups. */
COMMON fragS bss_address_frag;

/* main program "as.c" (command arguments etc) */

/* ['x'] TRUE if "-x" seen. */
COMMON char flagseen[128];
COMMON unsigned char flag_readonly_data_in_text;
COMMON unsigned char flag_suppress_warnings;
COMMON unsigned char flag_always_generate_output;

/* name of emitted object file */
COMMON char *out_file_name;

/* TRUE if we need a second pass. */
COMMON int need_pass_2;

/* TRUE if we should do no relaxing, and
   leave lots of padding.  */
COMMON int linkrelax;

/* TRUE if we should produce a listing.  */
extern int listing;

struct _pseudo_type
  {
    /* assembler mnemonic, lower case, no '.' */
    char *poc_name;
    /* Do the work */
    void (*poc_handler) PARAMS ((int));
    /* Value to pass to handler */
    int poc_val;
  };

typedef struct _pseudo_type pseudo_typeS;

#ifdef BFD_ASSEMBLER_xxx
struct lineno_struct
  {
    alent line;
    fragS *frag;
    struct lineno_struct *next;
  };
typedef struct lineno_struct lineno;
#endif

#if defined (__STDC__) && !defined(NO_STDARG)

#if __GNUC__ >= 2
/* for use with -Wformat */
#define PRINTF_LIKE(FCN)	void FCN (const char *format, ...) \
					__attribute__ ((format (printf, 1, 2)))
#define PRINTF_WHERE_LIKE(FCN)	void FCN (char *file, unsigned int line, \
					  const char *format, ...) \
					__attribute__ ((format (printf, 3, 4)))
#else /* ANSI C with stdarg, but not GNU C */
#define PRINTF_LIKE(FCN)	void FCN (const char *format, ...)
#define PRINTF_WHERE_LIKE(FCN)	void FCN (char *file, unsigned int line, \
					  const char *format, ...)
#endif
#else /* not ANSI C, or not stdarg */
#define PRINTF_LIKE(FCN)	void FCN ()
#define PRINTF_WHERE_LIKE(FCN)	void FCN ()
#endif

PRINTF_LIKE (as_bad);
PRINTF_LIKE (as_fatal);
PRINTF_LIKE (as_tsktsk);
PRINTF_LIKE (as_warn);
PRINTF_WHERE_LIKE (as_bad_where);
PRINTF_WHERE_LIKE (as_warn_where);

void fprint_value PARAMS ((FILE *file, addressT value));
void sprint_value PARAMS ((char *buf, addressT value));

int had_errors PARAMS ((void));
int had_warnings PARAMS ((void));

void print_version_id PARAMS ((void));
char *app_push PARAMS ((void));
char *atof_ieee PARAMS ((char *str, int what_kind, LITTLENUM_TYPE * words));
char *input_scrub_include_file PARAMS ((char *filename, char *position));
char *input_scrub_new_file PARAMS ((char *filename));
char *input_scrub_next_buffer PARAMS ((char **bufp));
PTR xmalloc PARAMS ((unsigned long size));
PTR xrealloc PARAMS ((PTR ptr, unsigned long n));
int do_scrub_next_char PARAMS ((int (*get) (void), void (*unget) (int)));
int gen_to_words PARAMS ((LITTLENUM_TYPE * words, int precision,
			  long exponent_bits));
int had_err PARAMS ((void));
int ignore_input PARAMS ((void));
int scrub_from_file PARAMS ((void));
int scrub_from_string PARAMS ((void));
int seen_at_least_1_file PARAMS ((void));
void app_pop PARAMS ((char *arg));
void as_howmuch PARAMS ((FILE * stream));
void as_perror PARAMS ((const char *gripe, const char *filename));
void as_where PARAMS ((char **namep, unsigned int *linep));
void bump_line_counters PARAMS ((void));
void do_scrub_begin PARAMS ((void));
void input_scrub_begin PARAMS ((void));
void input_scrub_close PARAMS ((void));
void input_scrub_end PARAMS ((void));
void new_logical_line PARAMS ((char *fname, int line_number));
void scrub_to_file PARAMS ((int ch));
void scrub_to_string PARAMS ((int ch));
void subsegs_begin PARAMS ((void));
void subseg_change PARAMS ((segT seg, int subseg));
segT subseg_new PARAMS ((const char *name, subsegT subseg));
segT subseg_force_new PARAMS ((const char *name, subsegT subseg));
void subseg_set PARAMS ((segT seg, subsegT subseg));
#ifdef BFD_ASSEMBLER
segT subseg_get PARAMS ((const char *, int));
#endif

struct expressionS;
struct fix;
struct symbol;

#ifdef BFD_ASSEMBLER
/* literal.c */
valueT add_to_literal_pool PARAMS ((struct symbol *, valueT, segT, int));
#endif

/* this one starts the chain of target dependant headers */
#include "targ-env.h"

#include "expr.h"
#include "struc-symbol.h"
#include "write.h"
#include "frags.h"
#include "hash.h"
#include "read.h"
#include "symbols.h"

#include "tc.h"
#include "obj.h"

#include "listing.h"

#ifdef BFD_ASSEMBLER
/* Someday perhaps this will be selectable at run-time.  */
#if defined (OBJ_AOUT) || defined (OBJ_BOUT)
#define OUTPUT_FLAVOR bfd_target_aout_flavour
#endif
#ifdef OBJ_COFF
#define OUTPUT_FLAVOR bfd_target_coff_flavour
#endif
#ifdef OBJ_ECOFF
#define OUTPUT_FLAVOR bfd_target_ecoff_flavour
#endif
#ifdef OBJ_ELF
#define OUTPUT_FLAVOR bfd_target_elf_flavour
#endif
#endif /* BFD_ASSEMBLER */

#endif /* GAS */

/* end of as.h */
