/* tc-arc.c -- Assembler for the ARC
   Copyright (C) 1994 Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).

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

#include <stdio.h>
#include <ctype.h>
#include "as.h"
#include "subsegs.h"
#include "opcode/arc.h"

extern int target_big_endian;

static arc_insn arc_insert_operand PARAMS ((arc_insn insn,
					    const struct arc_operand *operand,
					    int mods,
					    const struct arc_operand_value *reg,
					    offsetT val,
					    char *file, unsigned int line));

static void s_data1 PARAMS ((void));
static void s_seg PARAMS ((int));
static void s_proc PARAMS ((int));
static void s_reserve PARAMS ((int));
static void s_common PARAMS ((int));

const pseudo_typeS md_pseudo_table[] =
{
  {"align", s_align_bytes, 0},	/* Defaulting is invalid (0) */
  {"common", s_common, 0},
  {"global", s_globl, 0},
  {"half", cons, 2},
  {"optim", s_ignore, 0},
  {"proc", s_proc, 0},
  {"reserve", s_reserve, 0},
  {"seg", s_seg, 0},
  {"skip", s_space, 0},
  {"word", cons, 4},
  {"xword", cons, 8},
  {"pushsection", obj_elf_section, 0},
  {"popsection", obj_elf_previous, 0},
  {"uahalf", cons, 2},
  {"uaword", cons, 4},
  {"uaxword", cons, 8},
  {NULL, 0, 0},
};

const int md_short_jump_size = 4;
const int md_long_jump_size = 4;
const int md_reloc_size = 12;	/* Size of relocation record */

/* This array holds the chars that always start a comment.  If the
   pre-processor is disabled, these aren't very useful */
const char comment_chars[] = "#";

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output. */
/* Also note that comments started like this one will always
   work if '/' isn't otherwise defined. */
const char line_comment_chars[] = "#";

const char line_separator_chars[] = "";

/* Chars that can be used to separate mant from exp in floating point nums */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

static const char *arc_condition_codes[] =
{
  "al", "eq", "ne", "p", "n", "c", "nc", "v",
  "nv", "gt", "ge", "lt", "le", "hi", "ls", "pnz"
};

static struct hash_control *arc_ops_hash = NULL;
static struct hash_control *arc_suffix_hash = NULL;
static struct hash_control *arc_reg_hash = NULL;

const char *md_shortopts = "m:";
struct option md_longopts[] = {
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);

/* Non-zero if we accept the mul/mulu and variable shift insns.  */
static int have_mult = 0;

/*
 * md_parse_option
 *
 * Invocation line includes a switch not recognized by the base assembler.
 * See if it's a processor-specific option.
 */

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
    case 'm':
      if (strcmp (arg, "mult") == 0)
	have_mult = 1;
      else
	{
	  as_bad ("invalid architecture -m%s", arg);
	  return 0;
	}
      break;

    default:
      return 0;
    }

 return 1;
}

void
md_show_usage (stream)
     FILE *stream;
{
  fprintf (stream, "\
ARC options:\n\
-mmult			recognize the mul/mulu and variable shift instructions\n");
}

/* This function is called once, at assembler startup time.  It should
   set up all the tables, etc. that the MD part of the assembler will need. */
void
md_begin ()
{
  register unsigned int i = 0;
  char *last;

  target_big_endian = 1;

  if ((arc_ops_hash = hash_new ()) == NULL
      || (arc_suffix_hash = hash_new ()) == NULL
      || (arc_reg_hash = hash_new ()) == NULL)
    as_fatal ("Virtual memory exhausted");

#if 0
  for (i = 0; i < arc_opcodes_count; i++)
    hash_insert (arc_ops_hash, arc_opcodes[i].name, (PTR) (arc_opcodes + i));
#endif

  /* Only put the first entry of each equivalently named suffix in the
     table.  */
  last = "";
  for (i = 0; i < arc_suffixes_count; i++)
    {
      if (strcmp (arc_suffixes[i].name, last) != 0)
	hash_insert (arc_suffix_hash, arc_suffixes[i].name, (PTR) (arc_suffixes + i));
      last = arc_suffixes[i].name;
    }

  /* ??? This is the simple version.  See tc-arm.c for something snazzier.  */
  for (i = 0; i < arc_reg_names_count; i++)
    hash_insert (arc_reg_hash, arc_reg_names[i].name, (PTR) (arc_reg_names + i));

  /* This initializes a few things in arc-opc.c that we need.  */
  arc_opcode_init_tables (have_mult ? ARC_HAVE_MULT_SHIFT : 0);
}

/* Insert an operand value into an instruction.
   If REG is non-NULL, it is a register number and ignore VAL.  */

static arc_insn
arc_insert_operand (insn, operand, mods, reg, val, file, line)
     arc_insn insn;
     const struct arc_operand *operand;
     int mods;
     const struct arc_operand_value *reg;
     offsetT val;
     char *file;
     unsigned int line;
{
  if (operand->bits != 32)
    {
      long min, max;
      offsetT test;

      if ((operand->flags & ARC_OPERAND_SIGNED) != 0)
	{
	  if ((operand->flags & ARC_OPERAND_SIGNOPT) != 0)
	    max = (1 << operand->bits) - 1;
	  else
	    max = (1 << (operand->bits - 1)) - 1;
	  min = - (1 << (operand->bits - 1));
	}
      else
	{
	  max = (1 << operand->bits) - 1;
	  min = 0;
	}

      if ((operand->flags & ARC_OPERAND_NEGATIVE) != 0)
	test = - val;
      else
	test = val;

      if (test < (offsetT) min || test > (offsetT) max)
	{
	  const char *err =
	    "operand out of range (%s not between %ld and %ld)";
	  char buf[100];

	  sprint_value (buf, test);
	  if (file == (char *) NULL)
	    as_warn (err, buf, min, max);
	  else
	    as_warn_where (file, line, err, buf, min, max);
	}
    }

  if (operand->insert)
    {
      const char *errmsg;

      errmsg = NULL;
      insn = (*operand->insert) (insn, operand, mods, reg, (long) val, &errmsg);
      if (errmsg != (const char *) NULL)
	as_warn (errmsg);
    }
  else
    insn |= (((long) val & ((1 << operand->bits) - 1))
	     << operand->shift);

  return insn;
}

/* We need to keep a list of fixups.  We can't simply generate them as
   we go, because that would require us to first create the frag, and
   that would screw up references to ``.''.  */

struct arc_fixup
{
  expressionS exp;
  /* index into `arc_operands' */
  int opindex;
};

#define MAX_INSN_FIXUPS 5

/* This routine is called for each instruction to be assembled.  */

void
md_assemble (str)
     char *str;
{
  const struct arc_opcode *opcode,*opcode_end;
  char *start;
  arc_insn insn;
  bfd_reloc_code_real_type reloc;

  /* Skip leading white space.  */
  while (isspace (*str))
    str++;

  /* The instructions are sorted by the first letter.  Scan the opcode table
     until we find the right one.  */
  opcode_end = arc_opcodes + arc_opcodes_count;
  for (opcode = arc_opcodes; opcode < opcode_end; opcode++)
    if (*opcode->syntax == *str)
      break;
  if (opcode == opcode_end)
    {
      as_bad ("bad instruction `%s'", str);
      return;
    }

  /* Keep looking until we find a match.  If we haven't found a match, and the
     first character no longer matches, we needn't look any further.  */
  start = str;
  for ( ; opcode < opcode_end && *opcode->syntax == *start; ++opcode)
    {
      int past_opcode_p;
      char *syn;
      struct arc_fixup fixups[MAX_INSN_FIXUPS];
      int fc,limm_reloc_p;

      /* Scan the syntax string.  If it doesn't match, try the next one.  */

      arc_opcode_init_insert ();
      insn = opcode->value;
      reloc = BFD_RELOC_NONE;
      fc = 0;
      past_opcode_p = 0;

      /* Used as a sanity check.  If we need a limm reloc, make sure we ask
	 for an extra 4 bytes from frag_more.  */
      limm_reloc_p = 0;

      /* We don't check for (*str != '\0') here because we want to parse
	 any trailing fake arguments in the syntax string.  */
      for (str = start, syn = opcode->syntax; *syn != '\0'; )
	{
	  int mods;
	  const struct arc_operand *operand;

	  /* Non operand chars must match exactly.  */
	  if (*syn != '%' || *++syn == '%')
	    {
	      /* Handle '+' specially as we want to allow "ld r0,[sp-4]".  */
	      if (*syn == '+' && *str == '-')
		{
		  /* Skip over syn's +, but leave str's - alone.
		     That makes the case identical to "ld r0,[sp+-4]".  */
		  ++syn;
		}
	      else if (*str != *syn)
		break;
	      else
		{
		  if (*syn == ' ')
		    past_opcode_p = 1;
		  ++syn;
		  ++str;
		}
	      continue;
	    }

	  /* We have an operand.  Pick out any modifiers.  */
	  mods = 0;
	  while (ARC_MOD_P (arc_operands[arc_operand_map[*syn]].flags))
	    {
	      mods |= arc_operands[arc_operand_map[*syn]].flags & ARC_MOD_BITS;
	      ++syn;
	    }
	  operand = arc_operands + arc_operand_map[*syn];
	  if (operand->fmt == 0)
	    as_fatal ("unknown syntax format character `%c'", *syn);

	  if (operand->flags & ARC_OPERAND_FAKE)
	    {
	      const char *errmsg = NULL;
	      if (operand->insert)
		{
		  insn = (*operand->insert) (insn, operand, mods, NULL, 0, &errmsg);
		  /* If we get an error, go on to try the next insn.  */
		  if (errmsg)
		    break;
		}
	      ++syn;
	    }
	  /* Are we finished with suffixes?  */
	  else if (!past_opcode_p)
	    {
	      int found;
	      char c;
	      char *s,*t;
	      const struct arc_operand_value *suf,*suffix,*suffix_end;

	      if (!(operand->flags & ARC_OPERAND_SUFFIX))
		abort ();

	      /* If we're at a space in the input string, we want to skip the
		 remaining suffixes.  There may be some fake ones though, so
		 just go on to try the next one.  */
	      if (*str == ' ')
		{
		  ++syn;
		  continue;
		}

	      s = str;
	      if (mods & ARC_MOD_DOT)
		{
		  if (*s != '.')
		    break;
		  ++s;
		}
	      else
		{
		  /* This can happen in "b.nd foo" and we're currently looking
		     for "%q" (ie: a condition code suffix).  */
		  if (*s == '.')
		    {
		      ++syn;
		      continue;
		    }
		}

	      /* Pick the suffix out and look it up via the hash table.  */
	      for (t = s; *t && isalpha (*t); ++t)
		continue;
	      c = *t;
	      *t = '\0';
	      suf = hash_find (arc_suffix_hash, s);
	      *t = c;
	      if (!suf)
		{
		  /* This can happen in "blle foo" and we're currently using
		     the template "b%q%.n %j".  The "bl" insn occurs later in
		     the table so "lle" isn't an illegal suffix.  */
		  break;
		}

	      /* Is it the right type?  Note that the same character is used
	         several times, so we have to examine all of them.  This is
		 relatively efficient as equivalent entries are kept
		 together.  If it's not the right type, don't increment `str'
		 so we try the next one in the series.  */
	      found = 0;
	      suffix_end = arc_suffixes + arc_suffixes_count;
	      for (suffix = suf;
		   suffix < suffix_end && strcmp (suffix->name, suf->name) == 0;
		   suffix++)
		{
		  if (arc_operands[suffix->type].fmt == *syn)
		    {
		      /* Insert the suffix's value into the insn.  */
		      if (operand->insert)
			insn = (*operand->insert) (insn, operand,
						   mods, NULL, suffix->value,
						   NULL);
		      else
			insn |= suffix->value << operand->shift;

		      str = t;
		      found = 1;
		      break;
		    }
		}
	      ++syn;
	      if (!found)
		/* There's nothing to do except, go on to try the next one.
		   ??? This test can be deleted when we're done.  */
		;
	    }
	  else
	    /* This is either a register or an expression of some kind.  */
	    {
	      char c;
	      char *hold;
	      const struct arc_operand_value *reg;
	      long value;
	      expressionS ex;

	      if (operand->flags & ARC_OPERAND_SUFFIX)
		abort ();

	      /* Is there anything left to parse?
		 We don't check for this at the top because we want to parse
		 any trailing fake arguments in the syntax string.  */
	      if (*str == '\0')
		break;

	      /* Is this a syntax character?  Eg: is there a '[' present when
		 there shouldn't be?  */
	      if (!isalnum (*str)
		  /* '.' as in ".LLC0" */
		  && *str != '.'
		  /* '_' as in "_print" */
		  && *str != '_'
		  /* '-' as in "[fp-4]" */
		  && *str != '-')
		break;

	      /* Is it a register?  */
	      value = 0;
	      hold = str;
	      while (*str && (isalnum (*str) || *str == '_'))
		++str;
	      c = *str;
	      *str = '\0';
	      reg = hash_find (arc_reg_hash, hold);
	      *str = c;
	      if (!reg)
		{
		  /* Restore `str', it wasn't a register.  */
		  str = hold;

		  /* Gather the operand.  */
		  hold = input_line_pointer;
		  input_line_pointer = str;
		  expression (&ex);
		  str = input_line_pointer;
		  input_line_pointer = hold;

		  if (ex.X_op == O_illegal)
		    as_bad ("illegal operand");
		  else if (ex.X_op == O_absent)
		    as_bad ("missing operand");
		  else if (ex.X_op == O_constant)
		    {
		      value = ex.X_add_number;
		    }
		  else
		    {
		      /* We need to generate a fixup for this expression.
			 If this is a register constant (IE: one whose register
			 value gets stored as 61-63) then this must be a limm.
			 We don't support shimm relocs.  */
		      if (fc >= MAX_INSN_FIXUPS)
			as_fatal ("too many fixups");
		      fixups[fc].exp = ex;

		      /* ??? This bit could use some cleaning up.  Referencing
			 the format chars like this goes against style.  */
#define IS_REG_OPERAND(o) ((o) == 'a' || (o) == 'b' || (o) == 'c')
		      if (IS_REG_OPERAND (*syn))
			{
			  const char *junk;
			  fixups[fc].opindex = arc_operand_map['L'];
			  limm_reloc_p = 1;
			  /* Tell insert_reg we need a limm.  This is needed
			     because the value at this point is zero, a shimm.
			     ??? Hack city.  */
			  (*arc_operands[arc_operand_map['Q']].insert)
			    (insn, operand, mods, reg, 0L, &junk);
			}
		      else
			fixups[fc].opindex = arc_operand_map[*syn];
		      ++fc;
		      value = 0;
		    }
		}

	      /* Insert the register or expression into the instruction.  */
	      if (operand->insert)
		{
		  const char *errmsg = NULL;
		  insn = (*operand->insert) (insn, operand, mods,
					     reg, (long) value, &errmsg);
#if 0
		  if (errmsg != (const char *) NULL)
		    as_warn (errmsg);
#endif
		  /* FIXME: We want to try shimm insns for limm ones.  But if
		     the constant won't fit, we must go on to try the next
		     possibility.  Where do we issue warnings for constants
		     that are too big then?  At present, we'll flag the insn
		     as unrecognizable!  Maybe have the "bad instruction"
		     error message include our `errmsg'?  */
		  if (errmsg != (const char *) NULL)
		    break;
		}
	      else
		insn |= (value & ((1 << operand->bits) - 1)) << operand->shift;

	      ++syn;
	    }
	}

      if (*syn == '\0')
	{
	  int i;
	  char *f;
	  long limm;

	  /* We've found a matching insn.
	     ??? For the moment we assume a valid `str' can only contain blanks
	     now.  IE: We needn't try again with a longer version of the
	     insn.  */

	  while (isspace (*str))
	    ++str;

	  if (*str != '\0')
	    as_bad ("junk at end of line: `%s'", str);

	  /* Write out the instruction.  */
	  f = frag_more (4);
	  md_number_to_chars (f, insn, 4);

	  if (arc_opcode_limm_p (&limm))
	    {
	      char *f2 = frag_more (4);
	      md_number_to_chars (f2, limm, 4);
	    }
	  else if (limm_reloc_p)
	    /* Ahh! We need a limm reloc, but the tables think we don't.  */
	    abort ();

	  /* Create any fixups.  At this point we do not use a
	     bfd_reloc_code_real_type, but instead just use the operand index.
	     This lets us easily handle fixups for any operand type, although
	     that is admittedly not a very exciting feature.  We pick a BFD
	     reloc type in md_apply_fix.  */
	  for (i = 0; i < fc; i++)
	    {
	      const struct arc_operand *operand;

	      operand = &arc_operands[fixups[i].opindex];
	      fix_new_exp (frag_now,
			   ((f - frag_now->fr_literal)
			    + (operand->fmt == 'L' ? 4 : 0)), 4,
			   &fixups[i].exp,
			   (operand->flags & ARC_OPERAND_RELATIVE) != 0,
			   ((bfd_reloc_code_real_type)
			    (fixups[i].opindex + (int) BFD_RELOC_UNUSED)));
	    }

	  /* All done.  */
	  return;
	}

      /* Try the next entry.  */
    }

  as_bad ("bad instruction `%s'", start);
}

/*
 * sort of like s_lcomm
 *
 */
#ifndef OBJ_ELF
static int max_alignment = 15;
#endif

static void
s_reserve (ignore)
     int ignore;
{
  char *name;
  char *p;
  char c;
  int align;
  int size;
  int temp;
  symbolS *symbolP;

  name = input_line_pointer;
  c = get_symbol_end ();
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();

  if (*input_line_pointer != ',')
    {
      as_bad ("Expected comma after name");
      ignore_rest_of_line ();
      return;
    }

  ++input_line_pointer;

  if ((size = get_absolute_expression ()) < 0)
    {
      as_bad ("BSS length (%d.) <0! Ignored.", size);
      ignore_rest_of_line ();
      return;
    }				/* bad length */

  *p = 0;
  symbolP = symbol_find_or_make (name);
  *p = c;

  if (strncmp (input_line_pointer, ",\"bss\"", 6) != 0
      && strncmp (input_line_pointer, ",\".bss\"", 7) != 0)
    {
      as_bad ("bad .reserve segment -- expected BSS segment");
      return;
    }

  if (input_line_pointer[2] == '.')
    input_line_pointer += 7;
  else
    input_line_pointer += 6;
  SKIP_WHITESPACE ();

  if (*input_line_pointer == ',')
    {
      ++input_line_pointer;

      SKIP_WHITESPACE ();
      if (*input_line_pointer == '\n')
	{
	  as_bad ("Missing alignment");
	  return;
	}

      align = get_absolute_expression ();
#ifndef OBJ_ELF
      if (align > max_alignment)
	{
	  align = max_alignment;
	  as_warn ("Alignment too large: %d. assumed.", align);
	}
#endif
      if (align < 0)
	{
	  align = 0;
	  as_warn ("Alignment negative. 0 assumed.");
	}

      record_alignment (bss_section, align);

      /* convert to a power of 2 alignment */
      for (temp = 0; (align & 1) == 0; align >>= 1, ++temp);;

      if (align != 1)
	{
	  as_bad ("Alignment not a power of 2");
	  ignore_rest_of_line ();
	  return;
	}			/* not a power of two */

      align = temp;
    }				/* if has optional alignment */
  else
    align = 0;

  if ((S_GET_SEGMENT (symbolP) == bss_section
       || !S_IS_DEFINED (symbolP))
#ifdef OBJ_AOUT
      && S_GET_OTHER (symbolP) == 0
      && S_GET_DESC (symbolP) == 0
#endif
      )
    {
      if (! need_pass_2)
	{
	  char *pfrag;
	  segT current_seg = now_seg;
	  subsegT current_subseg = now_subseg;

	  subseg_set (bss_section, 1); /* switch to bss */

	  if (align)
	    frag_align (align, 0); /* do alignment */

	  /* detach from old frag */
	  if (S_GET_SEGMENT(symbolP) == bss_section)
	    symbolP->sy_frag->fr_symbol = NULL;

	  symbolP->sy_frag = frag_now;
	  pfrag = frag_var (rs_org, 1, 1, (relax_substateT)0, symbolP,
			    size, (char *)0);
	  *pfrag = 0;

	  S_SET_SEGMENT (symbolP, bss_section);

	  subseg_set (current_seg, current_subseg);
	}
    }
  else
    {
      as_warn("Ignoring attempt to re-define symbol %s.", name);
    }				/* if not redefining */

  demand_empty_rest_of_line ();
}

static void
s_common (ignore)
     int ignore;
{
  char *name;
  char c;
  char *p;
  int temp, size;
  symbolS *symbolP;

  name = input_line_pointer;
  c = get_symbol_end ();
  /* just after name is now '\0' */
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      as_bad ("Expected comma after symbol-name");
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;		/* skip ',' */
  if ((temp = get_absolute_expression ()) < 0)
    {
      as_bad (".COMMon length (%d.) <0! Ignored.", temp);
      ignore_rest_of_line ();
      return;
    }
  size = temp;
  *p = 0;
  symbolP = symbol_find_or_make (name);
  *p = c;
  if (S_IS_DEFINED (symbolP))
    {
      as_bad ("Ignoring attempt to re-define symbol");
      ignore_rest_of_line ();
      return;
    }
  if (S_GET_VALUE (symbolP) != 0)
    {
      if (S_GET_VALUE (symbolP) != size)
	{
	  as_warn ("Length of .comm \"%s\" is already %ld. Not changed to %d.",
		   S_GET_NAME (symbolP), (long) S_GET_VALUE (symbolP), size);
	}
    }
  else
    {
#ifndef OBJ_ELF
      S_SET_VALUE (symbolP, (valueT) size);
      S_SET_EXTERNAL (symbolP);
#endif
    }
  assert (symbolP->sy_frag == &zero_address_frag);
  if (*input_line_pointer != ',')
    {
      as_bad ("Expected comma after common length");
      ignore_rest_of_line ();
      return;
    }
  input_line_pointer++;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != '"')
    {
      temp = get_absolute_expression ();
#ifndef OBJ_ELF
      if (temp > max_alignment)
	{
	  temp = max_alignment;
	  as_warn ("Common alignment too large: %d. assumed", temp);
	}
#endif
      if (temp < 0)
	{
	  temp = 0;
	  as_warn ("Common alignment negative; 0 assumed");
	}
#ifdef OBJ_ELF
      if (symbolP->local)
	{
	  segT old_sec;
	  int old_subsec;
	  char *p;
	  int align;

	allocate_bss:
	  old_sec = now_seg;
	  old_subsec = now_subseg;
	  align = temp;
	  record_alignment (bss_section, align);
	  subseg_set (bss_section, 0);
	  if (align)
	    frag_align (align, 0);
	  if (S_GET_SEGMENT (symbolP) == bss_section)
	    symbolP->sy_frag->fr_symbol = 0;
	  symbolP->sy_frag = frag_now;
	  p = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP, size,
			(char *) 0);
	  *p = 0;
	  S_SET_SEGMENT (symbolP, bss_section);
	  S_CLEAR_EXTERNAL (symbolP);
	  subseg_set (old_sec, old_subsec);
	}
      else
#endif
	{
	allocate_common:
	  S_SET_VALUE (symbolP, (valueT) size);
#ifdef OBJ_ELF
	  S_SET_ALIGN (symbolP, temp);
#endif
	  S_SET_EXTERNAL (symbolP);
	  /* should be common, but this is how gas does it for now */
	  S_SET_SEGMENT (symbolP, bfd_und_section_ptr);
	}
    }
  else
    {
      input_line_pointer++;
      /* @@ Some use the dot, some don't.  Can we get some consistency??  */
      if (*input_line_pointer == '.')
	input_line_pointer++;
      /* @@ Some say data, some say bss.  */
      if (strncmp (input_line_pointer, "bss\"", 4)
	  && strncmp (input_line_pointer, "data\"", 5))
	{
	  while (*--input_line_pointer != '"')
	    ;
	  input_line_pointer--;
	  goto bad_common_segment;
	}
      while (*input_line_pointer++ != '"')
	;
      goto allocate_common;
    }
  demand_empty_rest_of_line ();
  return;

  {
  bad_common_segment:
    p = input_line_pointer;
    while (*p && *p != '\n')
      p++;
    c = *p;
    *p = '\0';
    as_bad ("bad .common segment %s", input_line_pointer + 1);
    *p = c;
    input_line_pointer = p;
    ignore_rest_of_line ();
    return;
  }
}

static void
s_seg (ignore)
     int ignore;
{

  if (strncmp (input_line_pointer, "\"text\"", 6) == 0)
    {
      input_line_pointer += 6;
      s_text (0);
      return;
    }
  if (strncmp (input_line_pointer, "\"data\"", 6) == 0)
    {
      input_line_pointer += 6;
      s_data (0);
      return;
    }
  if (strncmp (input_line_pointer, "\"data1\"", 7) == 0)
    {
      input_line_pointer += 7;
      s_data1 ();
      return;
    }
  if (strncmp (input_line_pointer, "\"bss\"", 5) == 0)
    {
      input_line_pointer += 5;
      /* We only support 2 segments -- text and data -- for now, so
	 things in the "bss segment" will have to go into data for now.
	 You can still allocate SEG_BSS stuff with .lcomm or .reserve. */
      subseg_set (data_section, 255);	/* FIXME-SOMEDAY */
      return;
    }
  as_bad ("Unknown segment type");
  demand_empty_rest_of_line ();
}

static void
s_data1 ()
{
  subseg_set (data_section, 1);
  demand_empty_rest_of_line ();
}

static void
s_proc (ignore)
     int ignore;
{
  while (!is_end_of_line[(unsigned char) *input_line_pointer])
    {
      ++input_line_pointer;
    }
  ++input_line_pointer;
}

/* Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP.
   An error message is returned, or NULL on OK.  */

/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

char *
md_atof (type, litP, sizeP)
     char type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[MAX_LITTLENUMS];
  LITTLENUM_TYPE *wordP;
  char *t;
  char *atof_ieee ();

  switch (type)
    {
    case 'f':
    case 'F':
      prec = 2;
      break;

    case 'd':
    case 'D':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return "bad call to md_atof";
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;
  *sizeP = prec * sizeof (LITTLENUM_TYPE);
  for (wordP = words; prec--;)
    {
      md_number_to_chars (litP, (valueT) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }

  return NULL;
}

/* Write a value out to the object file, using the appropriate
   endianness.  */

void
md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
  number_to_chars_bigendian (buf, val, n);
}

/* Round up a section size to the appropriate boundary. */

valueT
md_section_align (segment, size)
     segT segment;
     valueT size;
{
  int align = bfd_get_section_alignment (stdoutput, segment);

  return ((size + (1 << align) - 1) & (-1 << align));
}

/* We don't have any form of relaxing.  */

int
md_estimate_size_before_relax (fragp, seg)
     fragS *fragp;
     asection *seg;
{
  abort ();
}

const relax_typeS md_relax_table[] =
{
  { 0 }
};

/* Convert a machine dependent frag.  We never generate these.  */

void
md_convert_frag (abfd, sec, fragp)
     bfd *abfd;
     asection *sec;
     fragS *fragp;
{
  abort ();
}

/* Parse an operand that is machine-specific.
   We just return without modifying the expression if we have nothing to do. */

/* ARGSUSED */
void 
md_operand (expressionP)
     expressionS *expressionP;
{
}

/* We have no need to default values of symbols. */

/* ARGSUSED */
symbolS *
md_undefined_symbol (name)
     char *name;
{
  return 0;
}				/* md_undefined_symbol() */

/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long 
md_pcrel_from (fixP)
     fixS *fixP;
{
  if (fixP->fx_addsy != (symbolS *) NULL
      && ! S_IS_DEFINED (fixP->fx_addsy))
    {
      /* This makes a branch to an undefined symbol be a branch to the
	 current location.  */
      return 4;
    }

  /* Return the address of the delay slot.  */
  return fixP->fx_frag->fr_address + fixP->fx_where + fixP->fx_size;
}

/* Apply a fixup to the object code.  This is called for all the
   fixups we generated by the call to fix_new_exp, above.  In the call
   above we used a reloc code which was the largest legal reloc code
   plus the operand index.  Here we undo that to recover the operand
   index.  At this point all symbol values should be fully resolved,
   and we attempt to completely resolve the reloc.  If we can not do
   that, we determine the correct reloc code and put it back in the
   fixup.  */

int
md_apply_fix (fixP, valueP)
     fixS *fixP;
     valueT *valueP;
{
  /*char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;*/
  valueT value;

  /* FIXME FIXME FIXME: The value we are passed in *valueP includes
     the symbol values.  Since we are using BFD_ASSEMBLER, if we are
     doing this relocation the code in write.c is going to call
     bfd_perform_relocation, which is also going to use the symbol
     value.  That means that if the reloc is fully resolved we want to
     use *valueP since bfd_perform_relocation is not being used.
     However, if the reloc is not fully resolved we do not want to use
     *valueP, and must use fx_offset instead.  However, if the reloc
     is PC relative, we do want to use *valueP since it includes the
     result of md_pcrel_from.  This is confusing.  */

  if (fixP->fx_addsy == (symbolS *) NULL)
    {
      value = *valueP;
      fixP->fx_done = 1;
    }
  else if (fixP->fx_pcrel)
    value = *valueP;
  else
    {
      value = fixP->fx_offset;
      if (fixP->fx_subsy != (symbolS *) NULL)
	{
	  if (S_GET_SEGMENT (fixP->fx_subsy) == absolute_section)
	    value -= S_GET_VALUE (fixP->fx_subsy);
	  else
	    {
	      /* We can't actually support subtracting a symbol.  */
	      as_bad_where (fixP->fx_file, fixP->fx_line,
			    "expression too complex");
	    }
	}
    }

  if ((int) fixP->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      int opindex;
      const struct arc_operand *operand;
      char *where;
      arc_insn insn;

      opindex = (int) fixP->fx_r_type - (int) BFD_RELOC_UNUSED;

      operand = &arc_operands[opindex];

      /* Fetch the instruction, insert the fully resolved operand
	 value, and stuff the instruction back again.  */
      where = fixP->fx_frag->fr_literal + fixP->fx_where;
      insn = bfd_getb32 ((unsigned char *) where);
      insn = arc_insert_operand (insn, operand, -1, NULL, (offsetT) value,
				 fixP->fx_file, fixP->fx_line);
      bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);

      if (fixP->fx_done)
	{
	  /* Nothing else to do here.  */
	  return 1;
	}

      /* Determine a BFD reloc value based on the operand information.
	 We are only prepared to turn a few of the operands into relocs.
	 FIXME: Selecting the reloc type is a bit haphazard; perhaps
	 there should be a new field in the operand table.  */
      if ((operand->flags & ARC_OPERAND_RELATIVE) != 0
	  && operand->bits == 20
	  && operand->shift == 7)
	fixP->fx_r_type = BFD_RELOC_ARC_B22_PCREL;
      else if ((operand->flags & ARC_OPERAND_ABSOLUTE) != 0
	       && operand->bits == 32
	       && operand->shift == 32)
	fixP->fx_r_type = BFD_RELOC_32;
      else
	{
	  as_bad_where (fixP->fx_file, fixP->fx_line,
			"unresolved expression that must be resolved");
	  fixP->fx_done = 1;
	  return 1;
	}
    }
  else
    {
      switch (fixP->fx_r_type)
	{
	case BFD_RELOC_32:
	  md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
			      value, 4);
	  break;
	case BFD_RELOC_16:
	  md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
			      value, 2);
	  break;
	case BFD_RELOC_8:
	  md_number_to_chars (fixP->fx_frag->fr_literal + fixP->fx_where,
			      value, 1);
	  break;
	default:
	  abort ();
	}
    }

#ifdef OBJ_ELF
  fixP->fx_addnumber = value;
#else
  fixP->fx_addnumber = 0;
#endif

  return 1;
}

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent *
tc_gen_reloc (section, fixP)
     asection *section;
     fixS *fixP;
{
  arelent *reloc;

  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));

  reloc->sym_ptr_ptr = &fixP->fx_addsy->bsym;
  reloc->address = fixP->fx_frag->fr_address + fixP->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixP->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixP->fx_file, fixP->fx_line,
		    "internal error: can't export reloc type %d (`%s')",
		    fixP->fx_r_type, bfd_get_reloc_code_name (fixP->fx_r_type));
      return NULL;
    }
  reloc->addend = fixP->fx_addnumber;

  assert (!fixP->fx_pcrel == !reloc->howto->pc_relative);

  return reloc;
}
