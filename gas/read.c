/* read.c - read a source file -
   Copyright (C) 1986, 1987, 1990, 1991, 1993, 1994
   Free Software Foundation, Inc.

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
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#if 0
#define MASK_CHAR (0xFF)	/* If your chars aren't 8 bits, you will
				   change this a bit.  But then, GNU isn't
				   spozed to run on your machine anyway.
				   (RMS is so shortsighted sometimes.)
				   */
#else
#define MASK_CHAR ((int)(unsigned char)-1)
#endif


/* This is the largest known floating point format (for now). It will
   grow when we do 4361 style flonums. */

#define MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT (16)

/* Routines that read assembler source text to build spagetti in memory.
   Another group of these functions is in the expr.c module.  */

/* for isdigit() */
#include <ctype.h>

#include "as.h"
#include "subsegs.h"

#include "obstack.h"
#include "listing.h"

#ifndef TC_START_LABEL
#define TC_START_LABEL(x,y) (x==':')
#endif

/* The NOP_OPCODE is for the alignment fill value.
 * fill it a nop instruction so that the disassembler does not choke
 * on it
 */
#ifndef NOP_OPCODE
#define NOP_OPCODE 0x00
#endif

char *input_line_pointer;	/*->next char of source file to parse. */

#if BITS_PER_CHAR != 8
/*  The following table is indexed by[(char)] and will break if
    a char does not have exactly 256 states (hopefully 0:255!)!  */
die horribly;
#endif

#ifndef LEX_AT
/* The m88k unfortunately uses @ as a label beginner.  */
#define LEX_AT 0
#endif

#ifndef LEX_BR
/* The RS/6000 assembler uses {,},[,] as parts of symbol names.  */
#define LEX_BR 0
#endif

#ifndef LEX_PCT
/* The Delta 68k assembler permits % inside label names.  */
#define LEX_PCT 0
#endif

/* used by is_... macros. our ctype[] */
const char lex_type[256] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* @ABCDEFGHIJKLMNO */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ[\]^_ */
  0, 0, 0, 0, 3, LEX_PCT, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0,	/* _!"#$%&'()*+,-./ */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,	/* 0123456789:;<=>? */
  LEX_AT, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* @ABCDEFGHIJKLMNO */
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, LEX_BR, 0, LEX_BR, 0, 3, /* PQRSTUVWXYZ[\]^_ */
  0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* `abcdefghijklmno */
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, LEX_BR, 0, LEX_BR, 0, 0, /* pqrstuvwxyz{|}~. */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


/*
 * In: a character.
 * Out: 1 if this character ends a line.
 */
#define _ (0)
char is_end_of_line[256] =
{
#ifdef CR_EOL
  _, _, _, _, _, _, _, _, _, _, 99, _, _, 99, _, _,	/* @abcdefghijklmno */
#else
  _, _, _, _, _, _, _, _, _, _, 99, _, _, _, _, _,	/* @abcdefghijklmno */
#endif
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
#ifdef TC_HPPA
  _,99, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* _!"#$%&'()*+,-./ */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* 0123456789:;<=>? */
#else
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, 99, _, _, _, _,	/* 0123456789:;<=>? */
#endif
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
  _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,	/* */
};
#undef _

/* Functions private to this file. */

static char *buffer;	/* 1st char of each buffer of lines is here. */
static char *buffer_limit;	/*->1 + last char in buffer. */

int target_big_endian;

static char *old_buffer;	/* JF a hack */
static char *old_input;
static char *old_limit;

/* Variables for handling include file directory list. */

char **include_dirs;	/* List of pointers to directories to
			   search for .include's */
int include_dir_count;	/* How many are in the list */
int include_dir_maxlen = 1;/* Length of longest in list */

#ifndef WORKING_DOT_WORD
struct broken_word *broken_words;
int new_broken_words;
#endif

char *demand_copy_string PARAMS ((int *lenP));
int is_it_end_of_statement PARAMS ((void));
static segT get_segmented_expression PARAMS ((expressionS *expP));
static segT get_known_segmented_expression PARAMS ((expressionS * expP));
static void pobegin PARAMS ((void));


void
read_begin ()
{
  const char *p;

  pobegin ();
  obj_read_begin_hook ();

  /* Something close -- but not too close -- to a multiple of 1024.
     The debugging malloc I'm using has 24 bytes of overhead.  */
  obstack_begin (&notes, 5090);
  obstack_begin (&cond_obstack, 990);

  /* Use machine dependent syntax */
  for (p = line_separator_chars; *p; p++)
    is_end_of_line[(unsigned char) *p] = 1;
  /* Use more.  FIXME-SOMEDAY. */
}

/* set up pseudo-op tables */

struct hash_control *po_hash;

static const pseudo_typeS potable[] =
{
  {"abort", s_abort, 0},
  {"align", s_align_ptwo, 0},
  {"ascii", stringer, 0},
  {"asciz", stringer, 1},
/* block */
  {"byte", cons, 1},
  {"comm", s_comm, 0},
  {"data", s_data, 0},
#ifdef S_SET_DESC
  {"desc", s_desc, 0},
#endif
/* dim */
  {"double", float_cons, 'd'},
/* dsect */
  {"eject", listing_eject, 0},	/* Formfeed listing */
  {"else", s_else, 0},
  {"end", s_end, 0},
  {"endif", s_endif, 0},
/* endef */
  {"equ", s_set, 0},
/* err */
/* extend */
  {"extern", s_ignore, 0},	/* We treat all undef as ext */
  {"appfile", s_app_file, 1},
  {"appline", s_app_line, 0},
  {"file", s_app_file, 0},
  {"fill", s_fill, 0},
  {"float", float_cons, 'f'},
  {"global", s_globl, 0},
  {"globl", s_globl, 0},
  {"hword", cons, 2},
  {"if", s_if, 0},
  {"ifdef", s_ifdef, 0},
  {"ifeqs", s_ifeqs, 0},
  {"ifndef", s_ifdef, 1},
  {"ifnes", s_ifeqs, 1},
  {"ifnotdef", s_ifdef, 1},
  {"include", s_include, 0},
  {"int", cons, 4},
  {"lcomm", s_lcomm, 0},
  {"lflags", listing_flags, 0},	/* Listing flags */
  {"list", listing_list, 1},	/* Turn listing on */
  {"long", cons, 4},
  {"lsym", s_lsym, 0},
  {"nolist", listing_list, 0},	/* Turn listing off */
  {"octa", cons, 16},
  {"org", s_org, 0},
  {"psize", listing_psize, 0},	/* set paper size */
/* print */
  {"quad", cons, 8},
  {"sbttl", listing_title, 1},	/* Subtitle of listing */
/* scl */
/* sect */
  {"set", s_set, 0},
  {"short", cons, 2},
  {"single", float_cons, 'f'},
/* size */
  {"space", s_space, 0},
  {"stabd", s_stab, 'd'},
  {"stabn", s_stab, 'n'},
  {"stabs", s_stab, 's'},
  {"string", stringer, 1},
/* tag */
  {"text", s_text, 0},

  /* This is for gcc to use.  It's only just been added (2/94), so gcc
     won't be able to use it for a while -- probably a year or more.
     But once this has been released, check with gcc maintainers
     before deleting it or even changing the spelling.  */
  {"this_GCC_requires_the_GNU_assembler", s_ignore, 0},
  /* If we're folding case -- done for some targets, not necessarily
     all -- the above string in an input file will be converted to
     this one.  Match it either way...  */
  {"this_gcc_requires_the_gnu_assembler", s_ignore, 0},

  {"title", listing_title, 0},	/* Listing title */
/* type */
/* use */
/* val */
  {"xstabs", s_xstab, 's'},
  {"word", cons, 2},
  {"zero", s_space, 0},
  {NULL}			/* end sentinel */
};

static void 
pobegin ()
{
  const char *errtxt;			/* error text */
  const pseudo_typeS *pop;

  po_hash = hash_new ();

  /* Do the target-specific pseudo ops. */
  for (pop = md_pseudo_table; pop->poc_name; pop++)
    {
      errtxt = hash_insert (po_hash, pop->poc_name, (char *) pop);
      if (errtxt)
	{
	  as_fatal ("error constructing md pseudo-op table");
	}			/* on error */
    }				/* for each op */

  /* Now object specific.  Skip any that were in the target table. */
  for (pop = obj_pseudo_table; pop->poc_name; pop++)
    {
      errtxt = hash_insert (po_hash, pop->poc_name, (char *) pop);
      if (errtxt)
	{
	  if (!strcmp (errtxt, "exists"))
	    {
#ifdef DIE_ON_OVERRIDES
	      as_fatal ("pseudo op \".%s\" overridden.\n", pop->poc_name);
#endif /* DIE_ON_OVERRIDES */
	      continue;		/* OK if target table overrides. */
	    }
	  else
	    {
	      as_fatal ("error constructing obj pseudo-op table");
	    }			/* if overridden */
	}			/* on error */
    }				/* for each op */

  /* Now portable ones.  Skip any that we've seen already. */
  for (pop = potable; pop->poc_name; pop++)
    {
      errtxt = hash_insert (po_hash, pop->poc_name, (char *) pop);
      if (errtxt)
	{
	  if (!strcmp (errtxt, "exists"))
	    {
#ifdef DIE_ON_OVERRIDES
	      as_fatal ("pseudo op \".%s\" overridden.\n", pop->poc_name);
#endif /* DIE_ON_OVERRIDES */
	      continue;		/* OK if target table overrides. */
	    }
	  else
	    {
	      as_fatal ("error constructing obj pseudo-op table");
	    }			/* if overridden */
	}			/* on error */
    }				/* for each op */

  return;
}				/* pobegin() */

#define HANDLE_CONDITIONAL_ASSEMBLY()					\
  if (ignore_input ())							\
    {									\
      while (! is_end_of_line[(unsigned char) *input_line_pointer++])	\
	if (input_line_pointer == buffer_limit)				\
	  break;							\
      continue;								\
    }


/*	read_a_source_file()
 *
 * We read the file, putting things into a web that
 * represents what we have been reading.
 */
void 
read_a_source_file (name)
     char *name;
{
  register char c;
  register char *s;		/* string of symbol, '\0' appended */
  register int temp;
  pseudo_typeS *pop;

  buffer = input_scrub_new_file (name);

  listing_file (name);
  listing_newline ("");

  while ((buffer_limit = input_scrub_next_buffer (&input_line_pointer)) != 0)
    {				/* We have another line to parse. */
      know (buffer_limit[-1] == '\n');	/* Must have a sentinel. */
    contin:			/* JF this goto is my fault I admit it.
				   Someone brave please re-write the whole
				   input section here?  Pleeze???  */
      while (input_line_pointer < buffer_limit)
	{
	  /* We have more of this buffer to parse. */

	  /*
	   * We now have input_line_pointer->1st char of next line.
	   * If input_line_pointer [-1] == '\n' then we just
	   * scanned another line: so bump line counters.
	   */
	  if (is_end_of_line[(unsigned char) input_line_pointer[-1]])
	    {
	      if (input_line_pointer[-1] == '\n')
		bump_line_counters ();

#if defined (MRI) || defined (LABELS_WITHOUT_COLONS)
	      /* Text at the start of a line must be a label, we run down
		 and stick a colon in.  */
	      if (is_name_beginner (*input_line_pointer))
		{
		  char *line_start = input_line_pointer;
		  char c = get_symbol_end ();
		  colon (line_start);
		  *input_line_pointer = c;
		  if (c == ':')
		    input_line_pointer++;

		}
#endif
	    }


	  /*
	   * We are at the begining of a line, or similar place.
	   * We expect a well-formed assembler statement.
	   * A "symbol-name:" is a statement.
	   *
	   * Depending on what compiler is used, the order of these tests
	   * may vary to catch most common case 1st.
	   * Each test is independent of all other tests at the (top) level.
	   * PLEASE make a compiler that doesn't use this assembler.
	   * It is crufty to waste a compiler's time encoding things for this
	   * assembler, which then wastes more time decoding it.
	   * (And communicating via (linear) files is silly!
	   * If you must pass stuff, please pass a tree!)
	   */
	  if ((c = *input_line_pointer++) == '\t'
	      || c == ' '
	      || c == '\f'
	      || c == 0)
	    {
	      c = *input_line_pointer++;
	    }
	  know (c != ' ');	/* No further leading whitespace. */
	  LISTING_NEWLINE ();
	  /*
	   * C is the 1st significant character.
	   * Input_line_pointer points after that character.
	   */
	  if (is_name_beginner (c))
	    {
	      /* want user-defined label or pseudo/opcode */
	      HANDLE_CONDITIONAL_ASSEMBLY ();

	      s = --input_line_pointer;
	      c = get_symbol_end ();	/* name's delimiter */
	      /*
	       * C is character after symbol.
	       * That character's place in the input line is now '\0'.
	       * S points to the beginning of the symbol.
	       *   [In case of pseudo-op, s->'.'.]
	       * Input_line_pointer->'\0' where c was.
	       */
	      if (TC_START_LABEL(c, input_line_pointer))
		{
		  colon (s);	/* user-defined label */
		  *input_line_pointer++ = ':';	/* Put ':' back for error messages' sake. */
		  /* Input_line_pointer->after ':'. */
		  SKIP_WHITESPACE ();


		}
	      else if (c == '='
		       || (input_line_pointer[1] == '='
#ifdef TC_EQUAL_IN_INSN
			   && ! TC_EQUAL_IN_INSN (c, input_line_pointer)
#endif
			   ))
		{
		  equals (s);
		  demand_empty_rest_of_line ();
		}
	      else
		{		/* expect pseudo-op or machine instruction */
#ifdef MRI
		  if (!done_pseudo (s))

#else

		  pop = NULL;

#define IGNORE_OPCODE_CASE
#ifdef IGNORE_OPCODE_CASE
		  {
		    char *s2 = s;
		    while (*s2)
		      {
			if (isupper (*s2))
			  *s2 = tolower (*s2);
			s2++;
		      }
		  }
#endif

#ifdef NO_PSEUDO_DOT
		  /* The m88k uses pseudo-ops without a period.  */
		  pop = (pseudo_typeS *) hash_find (po_hash, s);
		  if (pop != NULL && pop->poc_handler == NULL)
		    pop = NULL;
#endif

		  if (pop != NULL || *s == '.')
		    {
		      /*
		       * PSEUDO - OP.
		       *
		       * WARNING: c has next char, which may be end-of-line.
		       * We lookup the pseudo-op table with s+1 because we
		       * already know that the pseudo-op begins with a '.'.
		       */

		      if (pop == NULL)
			pop = (pseudo_typeS *) hash_find (po_hash, s + 1);

		      /* Print the error msg now, while we still can */
		      if (pop == NULL)
			{
			  as_bad ("Unknown pseudo-op:  `%s'", s);
			  *input_line_pointer = c;
			  s_ignore (0);
			  continue;
			}

		      /* Put it back for error messages etc. */
		      *input_line_pointer = c;
		      /* The following skip of whitespace is compulsory.
			 A well shaped space is sometimes all that separates
			 keyword from operands. */
		      if (c == ' ' || c == '\t')
			input_line_pointer++;
		      /*
		       * Input_line is restored.
		       * Input_line_pointer->1st non-blank char
		       * after pseudo-operation.
		       */
		      (*pop->poc_handler) (pop->poc_val);
		    }
		  else
#endif
		    {		/* machine instruction */
		      /* WARNING: c has char, which may be end-of-line. */
		      /* Also: input_line_pointer->`\0` where c was. */
		      *input_line_pointer = c;
		      while (!is_end_of_line[(unsigned char) *input_line_pointer]
#ifdef TC_EOL_IN_INSN
			     || TC_EOL_IN_INSN (input_line_pointer)
#endif
			     )
			{
			  input_line_pointer++;
			}

		      c = *input_line_pointer;
		      *input_line_pointer = '\0';

		      md_assemble (s);	/* Assemble 1 instruction. */

		      *input_line_pointer++ = c;

		      /* We resume loop AFTER the end-of-line from
			 this instruction. */
		    }		/* if (*s=='.') */
		}		/* if c==':' */
	      continue;
	    }			/* if (is_name_beginner(c) */


	  /* Empty statement?  */
	  if (is_end_of_line[(unsigned char) c])
	    continue;

#if defined(LOCAL_LABELS_DOLLAR) || defined(LOCAL_LABELS_FB)
	  if (isdigit (c))
	    {
	      /* local label  ("4:") */
	      char *backup = input_line_pointer;

	      HANDLE_CONDITIONAL_ASSEMBLY ();

	      temp = c - '0';

	      while (isdigit (*input_line_pointer))
		{
		  temp = (temp * 10) + *input_line_pointer - '0';
		  ++input_line_pointer;
		}		/* read the whole number */

#ifdef LOCAL_LABELS_DOLLAR
	      if (*input_line_pointer == '$'
		  && *(input_line_pointer + 1) == ':')
		{
		  input_line_pointer += 2;

		  if (dollar_label_defined (temp))
		    {
		      as_fatal ("label \"%d$\" redefined", temp);
		    }

		  define_dollar_label (temp);
		  colon (dollar_label_name (temp, 0));
		  continue;
		}
#endif /* LOCAL_LABELS_DOLLAR */

#ifdef LOCAL_LABELS_FB
	      if (*input_line_pointer++ == ':')
		{
		  fb_label_instance_inc (temp);
		  colon (fb_label_name (temp, 0));
		  continue;
		}
#endif /* LOCAL_LABELS_FB */

	      input_line_pointer = backup;
	    }			/* local label  ("4:") */
#endif /* LOCAL_LABELS_DOLLAR or LOCAL_LABELS_FB */

	  if (c && strchr (line_comment_chars, c))
	    {			/* Its a comment.  Better say APP or NO_APP */
	      char *ends;
	      char *new_buf;
	      char *new_tmp;
	      unsigned int new_length;
	      char *tmp_buf = 0;
	      extern char *scrub_string, *scrub_last_string;

	      bump_line_counters ();
	      s = input_line_pointer;
	      if (strncmp (s, "APP\n", 4))
		continue;	/* We ignore it */
	      s += 4;

	      ends = strstr (s, "#NO_APP\n");

	      if (!ends)
		{
		  unsigned int tmp_len;
		  unsigned int num;

		  /* The end of the #APP wasn't in this buffer.  We
		     keep reading in buffers until we find the #NO_APP
		     that goes with this #APP  There is one.  The specs
		     guarentee it. . . */
		  tmp_len = buffer_limit - s;
		  tmp_buf = xmalloc (tmp_len + 1);
		  memcpy (tmp_buf, s, tmp_len);
		  do
		    {
		      new_tmp = input_scrub_next_buffer (&buffer);
		      if (!new_tmp)
			break;
		      else
			buffer_limit = new_tmp;
		      input_line_pointer = buffer;
		      ends = strstr (buffer, "#NO_APP\n");
		      if (ends)
			num = ends - buffer;
		      else
			num = buffer_limit - buffer;

		      tmp_buf = xrealloc (tmp_buf, tmp_len + num);
		      memcpy (tmp_buf + tmp_len, buffer, num);
		      tmp_len += num;
		    }
		  while (!ends);

		  input_line_pointer = ends ? ends + 8 : NULL;

		  s = tmp_buf;
		  ends = s + tmp_len;

		}
	      else
		{
		  input_line_pointer = ends + 8;
		}
	      new_buf = xmalloc (100);
	      new_length = 100;
	      new_tmp = new_buf;

	      scrub_string = s;
	      scrub_last_string = ends;
	      for (;;)
		{
		  int ch;

		  ch = do_scrub_next_char (scrub_from_string, scrub_to_string);
		  if (ch == EOF)
		    break;
		  *new_tmp++ = ch;
		  if (new_tmp == new_buf + new_length)
		    {
		      new_buf = xrealloc (new_buf, new_length + 100);
		      new_tmp = new_buf + new_length;
		      new_length += 100;
		    }
		}

	      if (tmp_buf)
		free (tmp_buf);
	      old_buffer = buffer;
	      old_input = input_line_pointer;
	      old_limit = buffer_limit;
	      buffer = new_buf;
	      input_line_pointer = new_buf;
	      buffer_limit = new_tmp;
	      continue;
	    }

	  HANDLE_CONDITIONAL_ASSEMBLY ();

	  /* as_warn("Junk character %d.",c);  Now done by ignore_rest */
	  input_line_pointer--;	/* Report unknown char as ignored. */
	  ignore_rest_of_line ();
	}			/* while (input_line_pointer<buffer_limit) */
      if (old_buffer)
	{
	  bump_line_counters ();
	  if (old_input != 0)
	    {
	      buffer = old_buffer;
	      input_line_pointer = old_input;
	      buffer_limit = old_limit;
	      old_buffer = 0;
	      goto contin;
	    }
	}
    }				/* while (more buffers to scan) */
  input_scrub_close ();		/* Close the input file */

}

void 
s_abort (ignore)
     int ignore;
{
  as_fatal (".abort detected.  Abandoning ship.");
}

/* Guts of .align directive.  */
static void 
do_align (n, fill)
     int n;
     char *fill;
{
#ifdef md_do_align
  md_do_align (n, fill, just_record_alignment);
#endif
  if (!fill)
    {
      /* @@ Fix this right for BFD!  */
      static char zero;
      static char nop_opcode = NOP_OPCODE;

      if (now_seg != data_section && now_seg != bss_section)
	{
	  fill = &nop_opcode;
	}
      else
	{
	  fill = &zero;
	}
    }
  /* Only make a frag if we HAVE to. . . */
  if (n && !need_pass_2)
    frag_align (n, *fill);

#ifdef md_do_align
 just_record_alignment:
#endif

  record_alignment (now_seg, n);
}

/* For machines where ".align 4" means align to a 4 byte boundary. */
void 
s_align_bytes (arg)
     int arg;
{
  register unsigned int temp;
  char temp_fill;
  unsigned int i = 0;
  unsigned long max_alignment = 1 << 15;

  if (is_end_of_line[(unsigned char) *input_line_pointer])
    temp = arg;			/* Default value from pseudo-op table */
  else
    temp = get_absolute_expression ();

  if (temp > max_alignment)
    {
      as_bad ("Alignment too large: %d. assumed.", temp = max_alignment);
    }

  /* For the sparc, `.align (1<<n)' actually means `.align n' so we
     have to convert it.  */
  if (temp != 0)
    {
      for (i = 0; (temp & 1) == 0; temp >>= 1, ++i)
	;
    }
  if (temp != 1)
    as_bad ("Alignment not a power of 2");

  temp = i;
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      temp_fill = get_absolute_expression ();
      do_align (temp, &temp_fill);
    }
  else
    do_align (temp, (char *) 0);

  demand_empty_rest_of_line ();
}

/* For machines where ".align 4" means align to 2**4 boundary. */
void 
s_align_ptwo (ignore)
     int ignore;
{
  register int temp;
  char temp_fill;
  long max_alignment = 15;

  temp = get_absolute_expression ();
  if (temp > max_alignment)
    as_bad ("Alignment too large: %d. assumed.", temp = max_alignment);
  else if (temp < 0)
    {
      as_bad ("Alignment negative. 0 assumed.");
      temp = 0;
    }
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      temp_fill = get_absolute_expression ();
      do_align (temp, &temp_fill);
    }
  else
    do_align (temp, (char *) 0);

  demand_empty_rest_of_line ();
}

void 
s_comm (ignore)
     int ignore;
{
  register char *name;
  register char c;
  register char *p;
  offsetT temp;
  register symbolS *symbolP;

  name = input_line_pointer;
  c = get_symbol_end ();
  /* just after name is now '\0' */
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      as_bad ("Expected comma after symbol-name: rest of line ignored.");
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;		/* skip ',' */
  if ((temp = get_absolute_expression ()) < 0)
    {
      as_warn (".COMMon length (%ld.) <0! Ignored.", (long) temp);
      ignore_rest_of_line ();
      return;
    }
  *p = 0;
  symbolP = symbol_find_or_make (name);
  *p = c;
  if (S_IS_DEFINED (symbolP))
    {
      as_bad ("Ignoring attempt to re-define symbol `%s'.",
	      S_GET_NAME (symbolP));
      ignore_rest_of_line ();
      return;
    }
  if (S_GET_VALUE (symbolP))
    {
      if (S_GET_VALUE (symbolP) != (valueT) temp)
	as_bad ("Length of .comm \"%s\" is already %ld. Not changed to %ld.",
		S_GET_NAME (symbolP),
		(long) S_GET_VALUE (symbolP),
		(long) temp);
    }
  else
    {
      S_SET_VALUE (symbolP, (valueT) temp);
      S_SET_EXTERNAL (symbolP);
    }
#ifdef OBJ_VMS
  {
    extern int flag_one;
    if ( (!temp) || !flag_one)
      S_GET_OTHER(symbolP) = const_flag;
  }
#endif /* not OBJ_VMS */
  know (symbolP->sy_frag == &zero_address_frag);
  demand_empty_rest_of_line ();
}				/* s_comm() */

void
s_data (ignore)
     int ignore;
{
  segT section;
  register int temp;

  temp = get_absolute_expression ();
  if (flag_readonly_data_in_text)
    {
      section = text_section;
      temp += 1000;
    }
  else
    section = data_section;

  subseg_set (section, (subsegT) temp);

#ifdef OBJ_VMS
  const_flag = 0;
#endif
  demand_empty_rest_of_line ();
}

/* Handle the .appfile pseudo-op.  This is automatically generated by
   do_scrub_next_char when a preprocessor # line comment is seen with
   a file name.  This default definition may be overridden by the
   object or CPU specific pseudo-ops.  This function is also the
   default definition for .file; the APPFILE argument is 1 for
   .appfile, 0 for .file.  */

void 
s_app_file (appfile)
     int appfile;
{
  register char *s;
  int length;

  /* Some assemblers tolerate immediately following '"' */
  if ((s = demand_copy_string (&length)) != 0)
    {
      /* If this is a fake .appfile, a fake newline was inserted into
	 the buffer.  Passing -2 to new_logical_line tells it to
	 account for it.  */
      new_logical_line (s, appfile ? -2 : -1);
      demand_empty_rest_of_line ();
#ifdef LISTING
      if (listing)
	listing_source_file (s);
#endif
    }
#ifdef OBJ_COFF
  c_dot_file_symbol (s);
#endif /* OBJ_COFF */
#ifdef OBJ_ELF
  elf_file_symbol (s);
#endif
}

/* Handle the .appline pseudo-op.  This is automatically generated by
   do_scrub_next_char when a preprocessor # line comment is seen.
   This default definition may be overridden by the object or CPU
   specific pseudo-ops.  */

void
s_app_line (ignore)
     int ignore;
{
  int l;

  /* The given number is that of the next line.  */
  l = get_absolute_expression () - 1;
  new_logical_line ((char *) NULL, l);
#ifdef LISTING
  if (listing)
    listing_source_line (l);
#endif
  demand_empty_rest_of_line ();
}

void 
s_fill (ignore)
     int ignore;
{
  long temp_repeat = 0;
  long temp_size = 1;
  register long temp_fill = 0;
  char *p;


  temp_repeat = get_absolute_expression ();
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      temp_size = get_absolute_expression ();
      if (*input_line_pointer == ',')
	{
	  input_line_pointer++;
	  temp_fill = get_absolute_expression ();
	}
    }
  /* This is to be compatible with BSD 4.2 AS, not for any rational reason.  */
#define BSD_FILL_SIZE_CROCK_8 (8)
  if (temp_size > BSD_FILL_SIZE_CROCK_8)
    {
      as_warn (".fill size clamped to %d.", BSD_FILL_SIZE_CROCK_8);
      temp_size = BSD_FILL_SIZE_CROCK_8;
    }
  if (temp_size < 0)
    {
      as_warn ("Size negative: .fill ignored.");
      temp_size = 0;
    }
  else if (temp_repeat <= 0)
    {
      as_warn ("Repeat < 0, .fill ignored");
      temp_size = 0;
    }

  if (temp_size && !need_pass_2)
    {
      p = frag_var (rs_fill, (int) temp_size, (int) temp_size, (relax_substateT) 0, (symbolS *) 0, temp_repeat, (char *) 0);
      memset (p, 0, (unsigned int) temp_size);
      /* The magic number BSD_FILL_SIZE_CROCK_4 is from BSD 4.2 VAX
       * flavoured AS.  The following bizzare behaviour is to be
       * compatible with above.  I guess they tried to take up to 8
       * bytes from a 4-byte expression and they forgot to sign
       * extend. Un*x Sux. */
#define BSD_FILL_SIZE_CROCK_4 (4)
      md_number_to_chars (p, (valueT) temp_fill,
			  (temp_size > BSD_FILL_SIZE_CROCK_4
			   ? BSD_FILL_SIZE_CROCK_4
			   : (int) temp_size));
      /* Note: .fill (),0 emits no frag (since we are asked to .fill 0 bytes)
       * but emits no error message because it seems a legal thing to do.
       * It is a degenerate case of .fill but could be emitted by a compiler.
       */
    }
  demand_empty_rest_of_line ();
}

void 
s_globl (ignore)
     int ignore;
{
  char *name;
  int c;
  symbolS *symbolP;

  do
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      symbolP = symbol_find_or_make (name);
      *input_line_pointer = c;
      SKIP_WHITESPACE ();
      S_SET_EXTERNAL (symbolP);
      if (c == ',')
	{
	  input_line_pointer++;
	  SKIP_WHITESPACE ();
	  if (*input_line_pointer == '\n')
	    c = '\n';
	}
    }
  while (c == ',');
  demand_empty_rest_of_line ();
}

void 
s_lcomm (needs_align)
     /* 1 if this was a ".bss" directive, which may require a 3rd argument
	(alignment); 0 if it was an ".lcomm" (2 args only)  */
     int needs_align;
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register symbolS *symbolP;
  segT current_seg = now_seg;
  subsegT current_subseg = now_subseg;
  const int max_alignment = 15;
  int align = 0;
  segT bss_seg = bss_section;

  name = input_line_pointer;
  c = get_symbol_end ();
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();

  /* Accept an optional comma after the name.  The comma used to be
     required, but Irix 5 cc does not generate it.  */
  if (*input_line_pointer == ',')
    {
      ++input_line_pointer;
      SKIP_WHITESPACE ();
    }

  if (*input_line_pointer == '\n')
    {
      as_bad ("Missing size expression");
      return;
    }

  if ((temp = get_absolute_expression ()) < 0)
    {
      as_warn ("BSS length (%d.) <0! Ignored.", temp);
      ignore_rest_of_line ();
      return;
    }

#if defined (TC_MIPS) || defined (TC_ALPHA)
#if defined (OBJ_ECOFF) || defined (OBJ_ELF)
  /* For MIPS and Alpha ECOFF or ELF, small objects are put in .sbss.  */
  if (temp <= bfd_get_gp_size (stdoutput))
    {
      bss_seg = subseg_new (".sbss", 1);
      seg_info (bss_seg)->bss = 1;
    }
#endif
#endif
   if (!needs_align)
     {
       /* FIXME. This needs to be machine independent. */
       if (temp >= 8)
	 align = 3;
       else if (temp >= 4)
	 align = 2;
       else if (temp >= 2)
	 align = 1;
       else
	 align = temp;

       record_alignment(bss_seg, align);
     }

  if (needs_align)
    {
      align = 0;
      SKIP_WHITESPACE ();
      if (*input_line_pointer != ',')
	{
	  as_bad ("Expected comma after size");
	  ignore_rest_of_line ();
	  return;
	}
      input_line_pointer++;
      SKIP_WHITESPACE ();
      if (*input_line_pointer == '\n')
	{
	  as_bad ("Missing alignment");
	  return;
	}
      align = get_absolute_expression ();
      if (align > max_alignment)
	{
	  align = max_alignment;
	  as_warn ("Alignment too large: %d. assumed.", align);
	}
      else if (align < 0)
	{
	  align = 0;
	  as_warn ("Alignment negative. 0 assumed.");
	}
      record_alignment (bss_seg, align);
    }				/* if needs align */
  else
    {
      /* Assume some objects may require alignment on some systems.  */
#ifdef TC_ALPHA
      if (temp > 1)
	{
	  align = ffs (temp) - 1;
	  if (temp % (1 << align))
	    abort ();
	}
#endif
    }

  *p = 0;
  symbolP = symbol_find_or_make (name);
  *p = c;

  if (
#if defined(OBJ_AOUT) | defined(OBJ_BOUT)
       S_GET_OTHER (symbolP) == 0 &&
       S_GET_DESC (symbolP) == 0 &&
#endif /* OBJ_AOUT or OBJ_BOUT */
       (S_GET_SEGMENT (symbolP) == bss_seg
	|| (!S_IS_DEFINED (symbolP) && S_GET_VALUE (symbolP) == 0)))
    {
      char *pfrag;

      subseg_set (bss_seg, 1);

      if (align)
	frag_align (align, 0);
					/* detach from old frag	*/
      if (S_GET_SEGMENT (symbolP) == bss_seg)
	symbolP->sy_frag->fr_symbol = NULL;

      symbolP->sy_frag = frag_now;
      pfrag = frag_var (rs_org, 1, 1, (relax_substateT)0, symbolP,
			temp, (char *)0);
      *pfrag = 0;

      S_SET_SEGMENT (symbolP, bss_seg);

#ifdef OBJ_COFF
      /* The symbol may already have been created with a preceding
         ".globl" directive -- be careful not to step on storage class
         in that case.  Otherwise, set it to static. */
      if (S_GET_STORAGE_CLASS (symbolP) != C_EXT)
	{
	  S_SET_STORAGE_CLASS (symbolP, C_STAT);
	}
#endif /* OBJ_COFF */
    }
  else
    as_bad ("Ignoring attempt to re-define symbol `%s'.",
	    S_GET_NAME (symbolP));

  subseg_set (current_seg, current_subseg);

  demand_empty_rest_of_line ();
}				/* s_lcomm() */

void 
s_lsym (ignore)
     int ignore;
{
  register char *name;
  register char c;
  register char *p;
  expressionS exp;
  register symbolS *symbolP;

  /* we permit ANY defined expression: BSD4.2 demands constants */
  name = input_line_pointer;
  c = get_symbol_end ();
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      *p = 0;
      as_bad ("Expected comma after name \"%s\"", name);
      *p = c;
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  expression (&exp);
  if (exp.X_op != O_constant
      && exp.X_op != O_register)
    {
      as_bad ("bad expression");
      ignore_rest_of_line ();
      return;
    }
  *p = 0;
  symbolP = symbol_find_or_make (name);

  /* FIXME-SOON I pulled a (&& symbolP->sy_other == 0 &&
     symbolP->sy_desc == 0) out of this test because coff doesn't have
     those fields, and I can't see when they'd ever be tripped.  I
     don't think I understand why they were here so I may have
     introduced a bug. As recently as 1.37 didn't have this test
     anyway.  xoxorich. */

  if (S_GET_SEGMENT (symbolP) == undefined_section
      && S_GET_VALUE (symbolP) == 0)
    {
      /* The name might be an undefined .global symbol; be sure to
	 keep the "external" bit. */
      S_SET_SEGMENT (symbolP,
		     (exp.X_op == O_constant
		      ? absolute_section
		      : reg_section));
      S_SET_VALUE (symbolP, (valueT) exp.X_add_number);
    }
  else
    {
      as_bad ("Symbol %s already defined", name);
    }
  *p = c;
  demand_empty_rest_of_line ();
}				/* s_lsym() */

void 
s_org (ignore)
     int ignore;
{
  register segT segment;
  expressionS exp;
  register long temp_fill;
  register char *p;
  /* Don't believe the documentation of BSD 4.2 AS.  There is no such
     thing as a sub-segment-relative origin.  Any absolute origin is
     given a warning, then assumed to be segment-relative.  Any
     segmented origin expression ("foo+42") had better be in the right
     segment or the .org is ignored.

     BSD 4.2 AS warns if you try to .org backwards. We cannot because
     we never know sub-segment sizes when we are reading code.  BSD
     will crash trying to emit negative numbers of filler bytes in
     certain .orgs. We don't crash, but see as-write for that code.

     Don't make frag if need_pass_2==1.  */
  segment = get_known_segmented_expression (&exp);
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      temp_fill = get_absolute_expression ();
    }
  else
    temp_fill = 0;
  if (!need_pass_2)
    {
      if (segment != now_seg && segment != absolute_section)
	as_bad ("Invalid segment \"%s\". Segment \"%s\" assumed.",
		segment_name (segment), segment_name (now_seg));
      p = frag_var (rs_org, 1, 1, (relax_substateT) 0, exp.X_add_symbol,
		    exp.X_add_number, (char *) 0);
      *p = temp_fill;
    }				/* if (ok to make frag) */
  demand_empty_rest_of_line ();
}				/* s_org() */

void 
s_set (ignore)
     int ignore;
{
  register char *name;
  register char delim;
  register char *end_name;
  register symbolS *symbolP;

  /*
   * Especial apologies for the random logic:
   * this just grew, and could be parsed much more simply!
   * Dean in haste.
   */
  name = input_line_pointer;
  delim = get_symbol_end ();
  end_name = input_line_pointer;
  *end_name = delim;
  SKIP_WHITESPACE ();

  if (*input_line_pointer != ',')
    {
      *end_name = 0;
      as_bad ("Expected comma after name \"%s\"", name);
      *end_name = delim;
      ignore_rest_of_line ();
      return;
    }

  input_line_pointer++;
  *end_name = 0;

  if (name[0] == '.' && name[1] == '\0')
    {
      /* Turn '. = mumble' into a .org mumble */
      register segT segment;
      expressionS exp;
      register char *ptr;

      segment = get_known_segmented_expression (&exp);

      if (!need_pass_2)
	{
	  if (segment != now_seg && segment != absolute_section)
	    as_bad ("Invalid segment \"%s\". Segment \"%s\" assumed.",
		    segment_name (segment),
		    segment_name (now_seg));
	  ptr = frag_var (rs_org, 1, 1, (relax_substateT) 0, exp.X_add_symbol,
			  exp.X_add_number, (char *) 0);
	  *ptr = 0;
	}			/* if (ok to make frag) */

      *end_name = delim;
      return;
    }

  if ((symbolP = symbol_find (name)) == NULL
      && (symbolP = md_undefined_symbol (name)) == NULL)
    {
      symbolP = symbol_new (name, undefined_section, 0, &zero_address_frag);
#ifdef OBJ_COFF
      /* "set" symbols are local unless otherwise specified. */
      SF_SET_LOCAL (symbolP);
#endif /* OBJ_COFF */

    }				/* make a new symbol */

  symbol_table_insert (symbolP);

  *end_name = delim;
  pseudo_set (symbolP);
  demand_empty_rest_of_line ();
}				/* s_set() */

void 
s_space (mult)
     int mult;
{
  long temp_repeat;
  register long temp_fill;
  register char *p;

  /* Just like .fill, but temp_size = 1 */
  if (get_absolute_expression_and_terminator (&temp_repeat) == ',')
    {
      temp_fill = get_absolute_expression ();
    }
  else
    {
      input_line_pointer--;	/* Backup over what was not a ','. */
      temp_fill = 0;
    }
  if (mult)
    {
      temp_repeat *= mult;
    }
  if (temp_repeat <= 0)
    {
      as_warn ("Repeat < 0, .space ignored");
      ignore_rest_of_line ();
      return;
    }
  if (!need_pass_2)
    {
      p = frag_var (rs_fill, 1, 1, (relax_substateT) 0, (symbolS *) 0,
		    temp_repeat, (char *) 0);
      *p = temp_fill;
    }
  demand_empty_rest_of_line ();
}				/* s_space() */

void
s_text (ignore)
     int ignore;
{
  register int temp;

  temp = get_absolute_expression ();
  subseg_set (text_section, (subsegT) temp);
  demand_empty_rest_of_line ();
}				/* s_text() */


void 
demand_empty_rest_of_line ()
{
  SKIP_WHITESPACE ();
  if (is_end_of_line[(unsigned char) *input_line_pointer])
    {
      input_line_pointer++;
    }
  else
    {
      ignore_rest_of_line ();
    }
  /* Return having already swallowed end-of-line. */
}				/* Return pointing just after end-of-line. */

void
ignore_rest_of_line ()		/* For suspect lines: gives warning. */
{
  if (!is_end_of_line[(unsigned char) *input_line_pointer])
    {
      if (isprint (*input_line_pointer))
	as_bad ("Rest of line ignored. First ignored character is `%c'.",
		*input_line_pointer);
      else
	as_bad ("Rest of line ignored. First ignored character valued 0x%x.",
		*input_line_pointer);
      while (input_line_pointer < buffer_limit
	     && !is_end_of_line[(unsigned char) *input_line_pointer])
	{
	  input_line_pointer++;
	}
    }
  input_line_pointer++;		/* Return pointing just after end-of-line. */
  know (is_end_of_line[(unsigned char) input_line_pointer[-1]]);
}

/*
 *			pseudo_set()
 *
 * In:	Pointer to a symbol.
 *	Input_line_pointer->expression.
 *
 * Out:	Input_line_pointer->just after any whitespace after expression.
 *	Tried to set symbol to value of expression.
 *	Will change symbols type, value, and frag;
 */
void
pseudo_set (symbolP)
     symbolS *symbolP;
{
  expressionS exp;
#if defined(OBJ_AOUT) | defined(OBJ_BOUT)
  int ext;
#endif /* OBJ_AOUT or OBJ_BOUT */

  know (symbolP);		/* NULL pointer is logic error. */
#if defined(OBJ_AOUT) | defined(OBJ_BOUT)
  /* @@ Fix this right for BFD.  */
  ext = S_IS_EXTERNAL (symbolP);
#endif /* OBJ_AOUT or OBJ_BOUT */

  (void) expression (&exp);

  if (exp.X_op == O_illegal)
    as_bad ("illegal expression; zero assumed");
  else if (exp.X_op == O_absent)
    as_bad ("missing expression; zero assumed");
  else if (exp.X_op == O_big)
    as_bad ("%s number invalid; zero assumed",
	    exp.X_add_number > 0 ? "bignum" : "floating point");
  else if (exp.X_op == O_subtract
	   && (S_GET_SEGMENT (exp.X_add_symbol)
	       == S_GET_SEGMENT (exp.X_op_symbol))
	   && SEG_NORMAL (S_GET_SEGMENT (exp.X_add_symbol))
	   && exp.X_add_symbol->sy_frag == exp.X_op_symbol->sy_frag)
    {
      exp.X_op = O_constant;
      exp.X_add_number = (S_GET_VALUE (exp.X_add_symbol)
			  - S_GET_VALUE (exp.X_op_symbol));
    }

  switch (exp.X_op)
    {
    case O_illegal:
    case O_absent:
    case O_big:
      exp.X_add_number = 0;
      /* Fall through.  */
    case O_constant:
      S_SET_SEGMENT (symbolP, absolute_section);
#if defined(OBJ_AOUT) | defined(OBJ_BOUT)
      /* @@ Fix this right for BFD.  */
      if (ext)
	S_SET_EXTERNAL (symbolP);
      else
	S_CLEAR_EXTERNAL (symbolP);
#endif /* OBJ_AOUT or OBJ_BOUT */
      S_SET_VALUE (symbolP, (valueT) exp.X_add_number);
      symbolP->sy_frag = &zero_address_frag;
      break;

    case O_register:
      S_SET_SEGMENT (symbolP, reg_section);
      S_SET_VALUE (symbolP, (valueT) exp.X_add_number);
      symbolP->sy_frag = &zero_address_frag;
      break;

    case O_symbol:
      if (S_GET_SEGMENT (exp.X_add_symbol) == undefined_section)
	symbolP->sy_value = exp;
      else
	{
	  S_SET_SEGMENT (symbolP, S_GET_SEGMENT (exp.X_add_symbol));
#if defined(OBJ_AOUT) | defined(OBJ_BOUT)
	  /* @@ Fix this right for BFD!  */
	  if (ext)
	    S_SET_EXTERNAL (symbolP);
	  else
	    S_CLEAR_EXTERNAL (symbolP);
#endif /* OBJ_AOUT or OBJ_BOUT */
	  S_SET_VALUE (symbolP,
		       exp.X_add_number + S_GET_VALUE (exp.X_add_symbol));
	  symbolP->sy_frag = exp.X_add_symbol->sy_frag;
	}
      break;

    default:
      /* The value is some complex expression.
	 FIXME: Should we set the segment to anything?  */
      symbolP->sy_value = exp;
      break;
    }
}

/*
 *			cons()
 *
 * CONStruct more frag of .bytes, or .words etc.
 * Should need_pass_2 be 1 then emit no frag(s).
 * This understands EXPRESSIONS.
 *
 * Bug (?)
 *
 * This has a split personality. We use expression() to read the
 * value. We can detect if the value won't fit in a byte or word.
 * But we can't detect if expression() discarded significant digits
 * in the case of a long. Not worth the crocks required to fix it.
 */

/* Select a parser for cons expressions.  */

/* Some targets need to parse the expression in various fancy ways.
   You can define TC_PARSE_CONS_EXPRESSION to do whatever you like
   (for example, the HPPA does this).  Otherwise, you can define
   BITFIELD_CONS_EXPRESSIONS to permit bitfields to be specified, or
   REPEAT_CONS_EXPRESSIONS to permit repeat counts.  If none of these
   are defined, which is the normal case, then only simple expressions
   are permitted.  */

#ifndef TC_PARSE_CONS_EXPRESSION
#ifdef BITFIELD_CONS_EXPRESSIONS
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) parse_bitfield_cons (EXP, NBYTES)
static void 
parse_bitfield_cons PARAMS ((expressionS *exp, unsigned int nbytes));
#endif
#ifdef MRI
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) parse_mri_cons (EXP)
static void
parse_mri_cons PARAMS ((expressionS *exp));
#endif
#ifdef REPEAT_CONS_EXPRESSIONS
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) parse_repeat_cons (EXP, NBYTES)
static void
parse_repeat_cons PARAMS ((expressionS *exp, unsigned int nbytes));
#endif

/* If we haven't gotten one yet, just call expression.  */
#ifndef TC_PARSE_CONS_EXPRESSION
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) expression (EXP)
#endif
#endif

/* worker to do .byte etc statements */
/* clobbers input_line_pointer, checks */
/* end-of-line. */
void 
cons (nbytes)
     register int nbytes;	/* 1=.byte, 2=.word, 4=.long */
{
  expressionS exp;

  if (is_it_end_of_statement ())
    {
      demand_empty_rest_of_line ();
      return;
    }

  do
    {
      TC_PARSE_CONS_EXPRESSION (&exp, (unsigned int) nbytes);
      emit_expr (&exp, (unsigned int) nbytes);
    }
  while (*input_line_pointer++ == ',');

  input_line_pointer--;		/* Put terminator back into stream. */
  demand_empty_rest_of_line ();
}

/* Put the contents of expression EXP into the object file using
   NBYTES bytes.  If need_pass_2 is 1, this does nothing.  */

void
emit_expr (exp, nbytes)
     expressionS *exp;
     unsigned int nbytes;
{
  operatorT op;
  register char *p;
  valueT extra_digit = 0;

  /* Don't do anything if we are going to make another pass.  */
  if (need_pass_2)
    return;

  op = exp->X_op;

  /* Handle a negative bignum.  */
  if (op == O_uminus
      && exp->X_add_number == 0
      && exp->X_add_symbol->sy_value.X_op == O_big
      && exp->X_add_symbol->sy_value.X_add_number > 0)
    {
      int i;
      unsigned long carry;

      exp = &exp->X_add_symbol->sy_value;

      /* Negate the bignum: one's complement each digit and add 1.  */
      carry = 1;
      for (i = 0; i < exp->X_add_number; i++)
	{
	  unsigned long next;

	  next = (((~ (generic_bignum[i] & LITTLENUM_MASK))
		   & LITTLENUM_MASK)
		  + carry);
	  generic_bignum[i] = next & LITTLENUM_MASK;
	  carry = next >> LITTLENUM_NUMBER_OF_BITS;
	}

      /* We can ignore any carry out, because it will be handled by
	 extra_digit if it is needed.  */

      extra_digit = (valueT) -1;
      op = O_big;
    }

  if (op == O_absent || op == O_illegal)
    {
      as_warn ("zero assumed for missing expression");
      exp->X_add_number = 0;
      op = O_constant;
    }
  else if (op == O_big && exp->X_add_number <= 0)
    {
      as_bad ("floating point number invalid; zero assumed");
      exp->X_add_number = 0;
      op = O_constant;
    }
  else if (op == O_register)
    {
      as_warn ("register value used as expression");
      op = O_constant;
    }

  p = frag_more ((int) nbytes);

#ifndef WORKING_DOT_WORD
  /* If we have the difference of two symbols in a word, save it on
     the broken_words list.  See the code in write.c.  */
  if (op == O_subtract && nbytes == 2)
    {
      struct broken_word *x;

      x = (struct broken_word *) xmalloc (sizeof (struct broken_word));
      x->next_broken_word = broken_words;
      broken_words = x;
      x->frag = frag_now;
      x->word_goes_here = p;
      x->dispfrag = 0;
      x->add = exp->X_add_symbol;
      x->sub = exp->X_op_symbol;
      x->addnum = exp->X_add_number;
      x->added = 0;
      new_broken_words++;
      return;
    }
#endif

  /* If we have an integer, but the number of bytes is too large to
     pass to md_number_to_chars, handle it as a bignum.  */
  if (op == O_constant && nbytes > sizeof (valueT))
    {
      valueT val;
      int gencnt;

      if (! exp->X_unsigned && exp->X_add_number < 0)
	extra_digit = (valueT) -1;
      val = (valueT) exp->X_add_number;
      gencnt = 0;
      do
	{
	  generic_bignum[gencnt] = val & LITTLENUM_MASK;
	  val >>= LITTLENUM_NUMBER_OF_BITS;
	  ++gencnt;
	}
      while (val != 0);
      op = exp->X_op = O_big;
      exp->X_add_number = gencnt;
    }

  if (op == O_constant)
    {
      register valueT get;
      register valueT use;
      register valueT mask;
      register valueT unmask;

      /* JF << of >= number of bits in the object is undefined.  In
	 particular SPARC (Sun 4) has problems */
      if (nbytes >= sizeof (valueT))
	mask = 0;
      else
	mask = ~(valueT) 0 << (BITS_PER_CHAR * nbytes);	/* Don't store these bits. */

      unmask = ~mask;		/* Do store these bits. */

#ifdef NEVER
      "Do this mod if you want every overflow check to assume SIGNED 2's complement data.";
      mask = ~(unmask >> 1);	/* Includes sign bit now. */
#endif

      get = exp->X_add_number;
      use = get & unmask;
      if ((get & mask) != 0 && (get & mask) != mask)
	{		/* Leading bits contain both 0s & 1s. */
	  as_warn ("Value 0x%lx truncated to 0x%lx.", get, use);
	}
      /* put bytes in right order. */
      md_number_to_chars (p, use, (int) nbytes);
    }
  else if (op == O_big)
    {
      int size;
      LITTLENUM_TYPE *nums;

      know (nbytes % CHARS_PER_LITTLENUM == 0);

      size = exp->X_add_number * CHARS_PER_LITTLENUM;
      if (nbytes < size)
	{
	  as_warn ("Bignum truncated to %d bytes", nbytes);
	  size = nbytes;
	}

      if (target_big_endian)
	{
	  while (nbytes > size)
	    {
	      md_number_to_chars (p, extra_digit, CHARS_PER_LITTLENUM);
	      nbytes -= CHARS_PER_LITTLENUM;
	      p += CHARS_PER_LITTLENUM;
	    }

	  nums = generic_bignum + size / CHARS_PER_LITTLENUM;
	  while (size > 0)
	    {
	      --nums;
	      md_number_to_chars (p, (valueT) *nums, CHARS_PER_LITTLENUM);
	      size -= CHARS_PER_LITTLENUM;
	      p += CHARS_PER_LITTLENUM;
	    }
	}
      else
	{
	  nums = generic_bignum;
	  while (size > 0)
	    {
	      md_number_to_chars (p, (valueT) *nums, CHARS_PER_LITTLENUM);
	      ++nums;
	      size -= CHARS_PER_LITTLENUM;
	      p += CHARS_PER_LITTLENUM;
	      nbytes -= CHARS_PER_LITTLENUM;
	    }

	  while (nbytes > 0)
	    {
	      md_number_to_chars (p, extra_digit, CHARS_PER_LITTLENUM);
	      nbytes -= CHARS_PER_LITTLENUM;
	      p += CHARS_PER_LITTLENUM;
	    }
	}
    }
  else
    {
      memset (p, 0, nbytes);

      /* Now we need to generate a fixS to record the symbol value.
	 This is easy for BFD.  For other targets it can be more
	 complex.  For very complex cases (currently, the HPPA and
	 NS32K), you can define TC_CONS_FIX_NEW to do whatever you
	 want.  For simpler cases, you can define TC_CONS_RELOC to be
	 the name of the reloc code that should be stored in the fixS.
	 If neither is defined, the code uses NO_RELOC if it is
	 defined, and otherwise uses 0.  */

#ifdef BFD_ASSEMBLER
#ifdef TC_CONS_FIX_NEW
      TC_CONS_FIX_NEW (frag_now, p - frag_now->fr_literal, nbytes, exp);
#else
      fix_new_exp (frag_now, p - frag_now->fr_literal, (int) nbytes, exp, 0,
		   /* @@ Should look at CPU word size.  */
		   nbytes == 2 ? BFD_RELOC_16
		   : nbytes == 8 ? BFD_RELOC_64
		   : BFD_RELOC_32);
#endif
#else
#ifdef TC_CONS_FIX_NEW
      TC_CONS_FIX_NEW (frag_now, p - frag_now->fr_literal, nbytes, exp);
#else
      /* Figure out which reloc number to use.  Use TC_CONS_RELOC if
	 it is defined, otherwise use NO_RELOC if it is defined,
	 otherwise use 0.  */
#ifndef TC_CONS_RELOC
#ifdef NO_RELOC
#define TC_CONS_RELOC NO_RELOC
#else
#define TC_CONS_RELOC 0
#endif
#endif
      fix_new_exp (frag_now, p - frag_now->fr_literal, (int) nbytes, exp, 0,
		   TC_CONS_RELOC);
#endif /* TC_CONS_FIX_NEW */
#endif /* BFD_ASSEMBLER */
    }
}

#ifdef BITFIELD_CONS_EXPRESSIONS

/* i960 assemblers, (eg, asm960), allow bitfields after ".byte" as
   w:x,y:z, where w and y are bitwidths and x and y are values.  They
   then pack them all together. We do a little better in that we allow
   them in words, longs, etc. and we'll pack them in target byte order
   for you.

   The rules are: pack least significat bit first, if a field doesn't
   entirely fit, put it in the next unit.  Overflowing the bitfield is
   explicitly *not* even a warning.  The bitwidth should be considered
   a "mask".

   To use this function the tc-XXX.h file should define
   BITFIELD_CONS_EXPRESSIONS.  */

static void 
parse_bitfield_cons (exp, nbytes)
     expressionS *exp;
     unsigned int nbytes;
{
  unsigned int bits_available = BITS_PER_CHAR * nbytes;
  char *hold = input_line_pointer;

  (void) expression (exp);

  if (*input_line_pointer == ':')
    {			/* bitfields */
      long value = 0;

      for (;;)
	{
	  unsigned long width;

	  if (*input_line_pointer != ':')
	    {
	      input_line_pointer = hold;
	      break;
	    }			/* next piece is not a bitfield */

	  /* In the general case, we can't allow
	     full expressions with symbol
	     differences and such.  The relocation
	     entries for symbols not defined in this
	     assembly would require arbitrary field
	     widths, positions, and masks which most
	     of our current object formats don't
	     support.
	     
	     In the specific case where a symbol
	     *is* defined in this assembly, we
	     *could* build fixups and track it, but
	     this could lead to confusion for the
	     backends.  I'm lazy. I'll take any
	     SEG_ABSOLUTE. I think that means that
	     you can use a previous .set or
	     .equ type symbol.  xoxorich. */

	  if (exp->X_op == O_absent)
	    {
	      as_warn ("using a bit field width of zero");
	      exp->X_add_number = 0;
	      exp->X_op = O_constant;
	    }			/* implied zero width bitfield */

	  if (exp->X_op != O_constant)
	    {
	      *input_line_pointer = '\0';
	      as_bad ("field width \"%s\" too complex for a bitfield", hold);
	      *input_line_pointer = ':';
	      demand_empty_rest_of_line ();
	      return;
	    }			/* too complex */

	  if ((width = exp->X_add_number) > (BITS_PER_CHAR * nbytes))
	    {
	      as_warn ("field width %lu too big to fit in %d bytes: truncated to %d bits",
		       width, nbytes, (BITS_PER_CHAR * nbytes));
	      width = BITS_PER_CHAR * nbytes;
	    }			/* too big */

	  if (width > bits_available)
	    {
	      /* FIXME-SOMEDAY: backing up and reparsing is wasteful.  */
	      input_line_pointer = hold;
	      exp->X_add_number = value;
	      break;
	    }			/* won't fit */

	  hold = ++input_line_pointer; /* skip ':' */

	  (void) expression (exp);
	  if (exp->X_op != O_constant)
	    {
	      char cache = *input_line_pointer;

	      *input_line_pointer = '\0';
	      as_bad ("field value \"%s\" too complex for a bitfield", hold);
	      *input_line_pointer = cache;
	      demand_empty_rest_of_line ();
	      return;
	    }			/* too complex */

	  value |= ((~(-1 << width) & exp->X_add_number)
		    << ((BITS_PER_CHAR * nbytes) - bits_available));

	  if ((bits_available -= width) == 0
	      || is_it_end_of_statement ()
	      || *input_line_pointer != ',')
	    {
	      break;
	    }			/* all the bitfields we're gonna get */

	  hold = ++input_line_pointer;
	  (void) expression (exp);
	}			/* forever loop */

      exp->X_add_number = value;
      exp->X_op = O_constant;
      exp->X_unsigned = 1;
    }				/* if looks like a bitfield */
}				/* parse_bitfield_cons() */

#endif /* BITFIELD_CONS_EXPRESSIONS */

#ifdef MRI

static void
parse_mri_cons (exp, nbytes)
     expressionS *exp;
     unsigned int nbytes;
{
  if (*input_line_pointer == '\'')
    {
      /* An MRI style string, cut into as many bytes as will fit into
	 a nbyte chunk, left justify if necessary, and separate with
	 commas so we can try again later */
      int scan = 0;
      unsigned int result = 0;
      input_line_pointer++;
      for (scan = 0; scan < nbytes; scan++)
	{
	  if (*input_line_pointer == '\'')
	    {
	      if (input_line_pointer[1] == '\'')
		{
		  input_line_pointer++;
		}
	      else
		break;
	    }
	  result = (result << 8) | (*input_line_pointer++);
	}

      /* Left justify */
      while (scan < nbytes)
	{
	  result <<= 8;
	  scan++;
	}
      /* Create correct expression */
      exp->X_op = O_constant;
      exp->X_add_number = result;
      /* Fake it so that we can read the next char too */
      if (input_line_pointer[0] != '\'' ||
	  (input_line_pointer[0] == '\'' && input_line_pointer[1] == '\''))
	{
	  input_line_pointer -= 2;
	  input_line_pointer[0] = ',';
	  input_line_pointer[1] = '\'';
	}
      else
	input_line_pointer++;
    }
  else
    expression (&exp);
}

#endif /* MRI */

#ifdef REPEAT_CONS_EXPRESSIONS

/* Parse a repeat expression for cons.  This is used by the MIPS
   assembler.  The format is NUMBER:COUNT; NUMBER appears in the
   object file COUNT times.

   To use this for a target, define REPEAT_CONS_EXPRESSIONS.  */

static void
parse_repeat_cons (exp, nbytes)
     expressionS *exp;
     unsigned int nbytes;
{
  expressionS count;
  register int i;

  expression (exp);

  if (*input_line_pointer != ':')
    {
      /* No repeat count.  */
      return;
    }

  ++input_line_pointer;
  expression (&count);
  if (count.X_op != O_constant
      || count.X_add_number <= 0)
    {
      as_warn ("Unresolvable or nonpositive repeat count; using 1");
      return;
    }

  /* The cons function is going to output this expression once.  So we
     output it count - 1 times.  */
  for (i = count.X_add_number - 1; i > 0; i--)
    emit_expr (exp, nbytes);
}

#endif /* REPEAT_CONS_EXPRESSIONS */

/*
 *			float_cons()
 *
 * CONStruct some more frag chars of .floats .ffloats etc.
 * Makes 0 or more new frags.
 * If need_pass_2 == 1, no frags are emitted.
 * This understands only floating literals, not expressions. Sorry.
 *
 * A floating constant is defined by atof_generic(), except it is preceded
 * by 0d 0f 0g or 0h. After observing the STRANGE way my BSD AS does its
 * reading, I decided to be incompatible. This always tries to give you
 * rounded bits to the precision of the pseudo-op. Former AS did premature
 * truncatation, restored noisy bits instead of trailing 0s AND gave you
 * a choice of 2 flavours of noise according to which of 2 floating-point
 * scanners you directed AS to use.
 *
 * In:	input_line_pointer->whitespace before, or '0' of flonum.
 *
 */

void
float_cons (float_type)
     /* Clobbers input_line-pointer, checks end-of-line. */
     register int float_type;	/* 'f':.ffloat ... 'F':.float ... */
{
  register char *p;
  int length;			/* Number of chars in an object. */
  register char *err;		/* Error from scanning floating literal. */
  char temp[MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT];

  if (is_it_end_of_statement ())
    {
      demand_empty_rest_of_line ();
      return;
    }

  do
    {
      /* input_line_pointer->1st char of a flonum (we hope!). */
      SKIP_WHITESPACE ();

      /* Skip any 0{letter} that may be present. Don't even check if the
       * letter is legal. Someone may invent a "z" format and this routine
       * has no use for such information. Lusers beware: you get
       * diagnostics if your input is ill-conditioned.
       */
      if (input_line_pointer[0] == '0' && isalpha (input_line_pointer[1]))
	input_line_pointer += 2;

      err = md_atof (float_type, temp, &length);
      know (length <= MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT);
      know (length > 0);
      if (err)
	{
	  as_bad ("Bad floating literal: %s", err);
	  ignore_rest_of_line ();
	  return;
	}

      if (!need_pass_2)
	{
	  int count;

	  count = 1;

#ifdef REPEAT_CONS_EXPRESSIONS
	  if (*input_line_pointer == ':')
	    {
	      expressionS count_exp;

	      ++input_line_pointer;
	      expression (&count_exp);
	      if (count_exp.X_op != O_constant
		  || count_exp.X_add_number <= 0)
		{
		  as_warn ("unresolvable or nonpositive repeat count; using 1");
		}
	      else
		count = count_exp.X_add_number;
	    }
#endif

	  while (--count >= 0)
	    {
	      p = frag_more (length);
	      memcpy (p, temp, (unsigned int) length);
	    }
	}
      SKIP_WHITESPACE ();
    }
  while (*input_line_pointer++ == ',');

  --input_line_pointer;		/* Put terminator back into stream.  */
  demand_empty_rest_of_line ();
}				/* float_cons() */

/*
 *			stringer()
 *
 * We read 0 or more ',' seperated, double-quoted strings.
 *
 * Caller should have checked need_pass_2 is FALSE because we don't check it.
 */


void 
stringer (append_zero)		/* Worker to do .ascii etc statements. */
     /* Checks end-of-line. */
     register int append_zero;	/* 0: don't append '\0', else 1 */
{
  register unsigned int c;

  /*
   * The following awkward logic is to parse ZERO or more strings,
   * comma seperated. Recall a string expression includes spaces
   * before the opening '\"' and spaces after the closing '\"'.
   * We fake a leading ',' if there is (supposed to be)
   * a 1st, expression. We keep demanding expressions for each
   * ','.
   */
  if (is_it_end_of_statement ())
    {
      c = 0;			/* Skip loop. */
      ++input_line_pointer;	/* Compensate for end of loop. */
    }
  else
    {
      c = ',';			/* Do loop. */
    }
  while (c == ',' || c == '<' || c == '"')
    {
      SKIP_WHITESPACE ();
      switch (*input_line_pointer)
	{
	case '\"':
	  ++input_line_pointer;	/*->1st char of string. */
	  while (is_a_char (c = next_char_of_string ()))
	    {
	      FRAG_APPEND_1_CHAR (c);
	    }
	  if (append_zero)
	    {
	      FRAG_APPEND_1_CHAR (0);
	    }
	  know (input_line_pointer[-1] == '\"');
	  break;
	case '<':
	  input_line_pointer++;
	  c = get_single_number ();
	  FRAG_APPEND_1_CHAR (c);
	  if (*input_line_pointer != '>')
	    {
	      as_bad ("Expected <nn>");
	    }
	  input_line_pointer++;
	  break;
	case ',':
	  input_line_pointer++;
	  break;
	}
      SKIP_WHITESPACE ();
      c = *input_line_pointer;
    }

  demand_empty_rest_of_line ();
}				/* stringer() */

/* FIXME-SOMEDAY: I had trouble here on characters with the
    high bits set.  We'll probably also have trouble with
    multibyte chars, wide chars, etc.  Also be careful about
    returning values bigger than 1 byte.  xoxorich. */

unsigned int 
next_char_of_string ()
{
  register unsigned int c;

  c = *input_line_pointer++ & CHAR_MASK;
  switch (c)
    {
    case '\"':
      c = NOT_A_CHAR;
      break;

#ifndef NO_STRING_ESCAPES
    case '\\':
      switch (c = *input_line_pointer++)
	{
	case 'b':
	  c = '\b';
	  break;

	case 'f':
	  c = '\f';
	  break;

	case 'n':
	  c = '\n';
	  break;

	case 'r':
	  c = '\r';
	  break;

	case 't':
	  c = '\t';
	  break;

#ifdef BACKSLASH_V
	case 'v':
	  c = '\013';
	  break;
#endif

	case '\\':
	case '"':
	  break;		/* As itself. */

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
	    long number;
	    int i;

	    for (i = 0, number = 0; isdigit (c) && i < 3; c = *input_line_pointer++, i++)
	      {
		number = number * 8 + c - '0';
	      }
	    c = number & 0xff;
	  }
	  --input_line_pointer;
	  break;

	case 'x':
	case 'X':
	  {
	    long number;

	    number = 0;
	    c = *input_line_pointer++;
	    while (isxdigit (c))
	      {
		if (isdigit (c))
		  number = number * 16 + c - '0';
		else if (isupper (c))
		  number = number * 16 + c - 'A' + 10;
		else
		  number = number * 16 + c - 'a' + 10;
		c = *input_line_pointer++;
	      }
	    c = number & 0xff;
	    --input_line_pointer;
	  }
	  break;

	case '\n':
	  /* To be compatible with BSD 4.2 as: give the luser a linefeed!! */
	  as_warn ("Unterminated string: Newline inserted.");
	  c = '\n';
	  break;

	default:

#ifdef ONLY_STANDARD_ESCAPES
	  as_bad ("Bad escaped character in string, '?' assumed");
	  c = '?';
#endif /* ONLY_STANDARD_ESCAPES */

	  break;
	}			/* switch on escaped char */
      break;
#endif /* ! defined (NO_STRING_ESCAPES) */

    default:
      break;
    }				/* switch on char */
  return (c);
}				/* next_char_of_string() */

static segT
get_segmented_expression (expP)
     register expressionS *expP;
{
  register segT retval;

  retval = expression (expP);
  if (expP->X_op == O_illegal
      || expP->X_op == O_absent
      || expP->X_op == O_big)
    {
      as_bad ("expected address expression; zero assumed");
      expP->X_op = O_constant;
      expP->X_add_number = 0;
      retval = absolute_section;
    }
  return retval;
}

static segT 
get_known_segmented_expression (expP)
     register expressionS *expP;
{
  register segT retval;

  if ((retval = get_segmented_expression (expP)) == undefined_section)
    {
      /* There is no easy way to extract the undefined symbol from the
	 expression.  */
      if (expP->X_add_symbol != NULL
	  && S_GET_SEGMENT (expP->X_add_symbol) != expr_section)
	as_warn ("symbol \"%s\" undefined; zero assumed",
		 S_GET_NAME (expP->X_add_symbol));
      else
	as_warn ("some symbol undefined; zero assumed");
      retval = absolute_section;
      expP->X_op = O_constant;
      expP->X_add_number = 0;
    }
  know (retval == absolute_section || SEG_NORMAL (retval));
  return (retval);
}				/* get_known_segmented_expression() */

offsetT
get_absolute_expression ()
{
  expressionS exp;

  expression (&exp);
  if (exp.X_op != O_constant)
    {
      if (exp.X_op != O_absent)
	as_bad ("bad absolute expression; zero assumed");
      exp.X_add_number = 0;
    }
  return exp.X_add_number;
}

char				/* return terminator */
get_absolute_expression_and_terminator (val_pointer)
     long *val_pointer;		/* return value of expression */
{
  /* FIXME: val_pointer should probably be offsetT *.  */
  *val_pointer = (long) get_absolute_expression ();
  return (*input_line_pointer++);
}

/*
 *			demand_copy_C_string()
 *
 * Like demand_copy_string, but return NULL if the string contains any '\0's.
 * Give a warning if that happens.
 */
char *
demand_copy_C_string (len_pointer)
     int *len_pointer;
{
  register char *s;

  if ((s = demand_copy_string (len_pointer)) != 0)
    {
      register int len;

      for (len = *len_pointer;
	   len > 0;
	   len--)
	{
	  if (*s == 0)
	    {
	      s = 0;
	      len = 1;
	      *len_pointer = 0;
	      as_bad ("This string may not contain \'\\0\'");
	    }
	}
    }
  return (s);
}

/*
 *			demand_copy_string()
 *
 * Demand string, but return a safe (=private) copy of the string.
 * Return NULL if we can't read a string here.
 */
char *
demand_copy_string (lenP)
     int *lenP;
{
  register unsigned int c;
  register int len;
  char *retval;

  len = 0;
  SKIP_WHITESPACE ();
  if (*input_line_pointer == '\"')
    {
      input_line_pointer++;	/* Skip opening quote. */

      while (is_a_char (c = next_char_of_string ()))
	{
	  obstack_1grow (&notes, c);
	  len++;
	}
      /* JF this next line is so demand_copy_C_string will return a null
		   termanated string. */
      obstack_1grow (&notes, '\0');
      retval = obstack_finish (&notes);
    }
  else
    {
      as_warn ("Missing string");
      retval = NULL;
      ignore_rest_of_line ();
    }
  *lenP = len;
  return (retval);
}				/* demand_copy_string() */

/*
 *		is_it_end_of_statement()
 *
 * In:	Input_line_pointer->next character.
 *
 * Do:	Skip input_line_pointer over all whitespace.
 *
 * Out:	1 if input_line_pointer->end-of-line.
*/
int 
is_it_end_of_statement ()
{
  SKIP_WHITESPACE ();
  return (is_end_of_line[(unsigned char) *input_line_pointer]);
}				/* is_it_end_of_statement() */

void 
equals (sym_name)
     char *sym_name;
{
  register symbolS *symbolP;	/* symbol we are working with */

  input_line_pointer++;
  if (*input_line_pointer == '=')
    input_line_pointer++;

  while (*input_line_pointer == ' ' || *input_line_pointer == '\t')
    input_line_pointer++;

  if (sym_name[0] == '.' && sym_name[1] == '\0')
    {
      /* Turn '. = mumble' into a .org mumble */
      register segT segment;
      expressionS exp;
      register char *p;

      segment = get_known_segmented_expression (&exp);
      if (!need_pass_2)
	{
	  if (segment != now_seg && segment != absolute_section)
	    as_warn ("Illegal segment \"%s\". Segment \"%s\" assumed.",
		     segment_name (segment),
		     segment_name (now_seg));
	  p = frag_var (rs_org, 1, 1, (relax_substateT) 0, exp.X_add_symbol,
			exp.X_add_number, (char *) 0);
	  *p = 0;
	}			/* if (ok to make frag) */
    }
  else
    {
      symbolP = symbol_find_or_make (sym_name);
      pseudo_set (symbolP);
    }
}				/* equals() */

/* .include -- include a file at this point. */

/* ARGSUSED */
void 
s_include (arg)
     int arg;
{
  char *newbuf;
  char *filename;
  int i;
  FILE *try;
  char *path;

  filename = demand_copy_string (&i);
  demand_empty_rest_of_line ();
  path = xmalloc ((unsigned long) i + include_dir_maxlen + 5 /* slop */ );
  for (i = 0; i < include_dir_count; i++)
    {
      strcpy (path, include_dirs[i]);
      strcat (path, "/");
      strcat (path, filename);
      if (0 != (try = fopen (path, FOPEN_RT)))
	{
	  fclose (try);
	  goto gotit;
	}
    }
  free (path);
  path = filename;
gotit:
  /* malloc Storage leak when file is found on path.  FIXME-SOMEDAY. */
  newbuf = input_scrub_include_file (path, input_line_pointer);
  buffer_limit = input_scrub_next_buffer (&input_line_pointer);
}				/* s_include() */

void 
add_include_dir (path)
     char *path;
{
  int i;

  if (include_dir_count == 0)
    {
      include_dirs = (char **) xmalloc (2 * sizeof (*include_dirs));
      include_dirs[0] = ".";	/* Current dir */
      include_dir_count = 2;
    }
  else
    {
      include_dir_count++;
      include_dirs = (char **) realloc (include_dirs,
				include_dir_count * sizeof (*include_dirs));
    }

  include_dirs[include_dir_count - 1] = path;	/* New one */

  i = strlen (path);
  if (i > include_dir_maxlen)
    include_dir_maxlen = i;
}				/* add_include_dir() */

void 
s_ignore (arg)
     int arg;
{
  while (!is_end_of_line[(unsigned char) *input_line_pointer])
    {
      ++input_line_pointer;
    }
  ++input_line_pointer;
}


/* end of read.c */
