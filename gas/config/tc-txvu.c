/* tc-txvu.c -- Assembler for the TX VU.
   Copyright (C) 1997 Free Software Foundation.

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
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <ctype.h>
#include "as.h"
#include "subsegs.h"
/* Needed by opcode/txvu.h.  */
#include "dis-asm.h"
#include "opcode/txvu.h"
#include "elf/txvu.h"

const char comment_chars[] = ";";
const char line_comment_chars[] = "#";
const char line_separator_chars[] = "!";
const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "dD";

const char *md_shortopts = "";

struct option md_longopts[] =
{
  /* insert options here */

  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);       

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  return 0;
}

void
md_show_usage (stream)
  FILE *stream;
{
#if 0
  fprintf (stream, "TX VU options:\n");
#endif
} 

/* Set by md_assemble for use by txvu_fill_insn.  */
static subsegT prev_subseg;
static segT prev_seg;

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  { "word", cons, 4 },
  { NULL, NULL, 0 }
};

void
md_begin ()
{
  flagword applicable;
  segT seg;
  subsegT subseg;

  /* Save the current subseg so we can restore it [it's the default one and
     we don't want the initial section to be .sbss.  */
  seg = now_seg;
  subseg = now_subseg;

  subseg_set (seg, subseg);

  /* Initialize the opcode tables.
     This involves computing the hash chains.  */
  txvu_opcode_init_tables (0);
}

/* We need to keep a list of fixups.  We can't simply generate them as
   we go, because that would require us to first create the frag, and
   that would screw up references to ``.''.  */

struct txvu_fixup
{
  /* index into `txvu_operands' */
  int opindex;
  expressionS exp;
};

#define MAX_FIXUPS 5

static char * assemble_insn PARAMS ((char *, int));

void
md_assemble (str)
     char *str;
{
#ifdef VERTICAL_BAR_SEPARATOR
  char *p = strchr (str, '|');

  if (p == NULL)
    {
      as_bad ("lower slot missing in `%s'", str);
      return;
    }

  *p = 0;
  assemble_insn (str, 0);
  *p = '|';
  assemble_insn (p + 1, 1);
#else
  str = assemble_insn (str, 0);
  /* Don't assemble next one if we couldn't assemble the first.  */
  if (str)
    assemble_insn (str, 1);
#endif
}

/* Assemble one instruction.
   LOWER_P is non-zero if assembling in the lower insn slot.
   The result is a pointer to beyond the end of the scanned insn.
   If this is the upper insn, the caller can pass back to result to us
   parse the lower insn.  */

static char *
assemble_insn (str, lower_p)
     char *str;
     int lower_p;
{
  const struct txvu_opcode *opcode;
  char *start;
  TXVU_INSN insn_buf[2];
  TXVU_INSN insn;

  /* Skip leading white space.  */
  while (isspace (*str))
    str++;

  /* The instructions are stored in lists hashed by the first letter (though
     we needn't care how they're hashed).  Get the first in the list.  */

  if (lower_p)
    opcode = txvu_lower_opcode_lookup_asm (str);
  else
    opcode = txvu_upper_opcode_lookup_asm (str);

  /* Keep looking until we find a match.  */

  start = str;
  for ( ; opcode != NULL; opcode = TXVU_OPCODE_NEXT_ASM (opcode))
    {
      int past_opcode_p, fc, num_suffixes, num_operands;
      const unsigned char *syn;
      struct txvu_fixup fixups[MAX_FIXUPS];

      /* Ensure the mnemonic part matches.  */
      for (str = start, syn = opcode->mnemonic; *syn != '\0'; ++str, ++syn)
	if (tolower (*str) != tolower (*syn))
	  break;
      if (*syn != '\0')
	continue;
      if (isalpha (*str))
	continue;

      /* Scan the syntax string.  If it doesn't match, try the next one.  */

      txvu_opcode_init_parse ();
      insn = opcode->value;
      fc = 0;
      past_opcode_p = 0;
      num_suffixes = 0;
      num_operands = 0;

      /* We don't check for (*str != '\0') here because we want to parse
	 any trailing fake arguments in the syntax string.  */
      for (/*str = start, */ syn = opcode->syntax; *syn != '\0'; )
	{
	  int mods,index;
	  const struct txvu_operand *operand;
	  const char *errmsg;

	  /* Non operand chars must match exactly.  */
	  if (*syn < 128)
	    {
	      if (*str == *syn)
		{
		  if (*syn == ' ')
		    past_opcode_p = 1;
		  ++syn;
		  ++str;
		}
	      else
		break;
	      continue;
	    }

	  /* We have a suffix or an operand.  Pick out any modifiers.  */
	  mods = 0;
	  index = TXVU_OPERAND_INDEX (*syn);
	  while (TXVU_MOD_P (txvu_operands[index].flags))
	    {
	      mods |= txvu_operands[index].flags & TXVU_MOD_BITS;
	      ++syn;
	      index = TXVU_OPERAND_INDEX (*syn);
	    }
	  operand = txvu_operands + index;

	  if (operand->flags & TXVU_OPERAND_FAKE)
	    {
	      if (operand->insert)
		{
		  insn = (*operand->insert) (insn, operand, mods, 0, &errmsg);
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
	      long suf_value;

	      if (!(operand->flags & TXVU_OPERAND_SUFFIX))
		as_fatal ("bad opcode table, missing suffix flag");

	      /* If we're at a space in the input string, we want to skip the
		 remaining suffixes.  There may be some fake ones though, so
		 just go on to try the next one.  */
	      if (*str == ' ')
		{
		  ++syn;
		  continue;
		}

	      s = str;

	      /* Pick the suffix out and parse it.  */
	      for (t = *s == '.' ? s + 1 : s; *t && isalpha (*t); ++t)
		continue;
	      c = *t;
	      *t = '\0';
	      suf_value = (*operand->parse) (&s, &errmsg);
	      *t = c;
	      if (errmsg)
		{
		  /* This can happen in "blle foo" and we're currently using
		     the template "b%q%.n %j".  The "bl" insn occurs later in
		     the table so "lle" isn't an illegal suffix.  */
		  break;
		}
	      /* Insert the suffix's value into the insn.  */
	      if (operand->insert)
		insn = (*operand->insert) (insn, operand,
					   mods, suf_value, NULL);
	      else
		insn |= suf_value << operand->shift;

	      str = t;
	      ++syn;
	    }
	  else
	    /* This is an operand, either a register or an expression of
	       some kind.  */
	    {
	      char c;
	      char *hold;
	      long value = 0;
	      expressionS exp;

	      if (operand->flags & TXVU_OPERAND_SUFFIX)
		as_fatal ("bad opcode table, suffix wrong");

	      /* If this is not the first, there must be a comma.  */
	      if (num_operands > 0)
		{
		  if (*str != ',')
		    break;
		  ++str;
		}

	      /* Is there anything left to parse?
		 We don't check for this at the top because we want to parse
		 any trailing fake arguments in the syntax string.  */
	      if (*str == '\0')
		break;

	      /* Parse the operand.  */
	      if (operand->parse)
		{
		  value = (*operand->parse) (&str, &errmsg);
		  if (errmsg)
		    break;
		}
	      else
		{
		  hold = input_line_pointer;
		  input_line_pointer = str;
		  expression (&exp);
		  str = input_line_pointer;
		  input_line_pointer = hold;

		  if (exp.X_op == O_illegal
		      || exp.X_op == O_absent)
		    break;
		  else if (exp.X_op == O_constant)
		    value = exp.X_add_number;
		  else if (exp.X_op == O_register)
		    as_fatal ("got O_register");
		  else
		    {
		      /* We need to generate a fixup for this expression.  */
		      if (fc >= MAX_FIXUPS)
			as_fatal ("too many fixups");
		      fixups[fc].exp = exp;
		      fixups[fc].opindex = index;
		      ++fc;
		      value = 0;
		    }
		}

	      /* Insert the register or expression into the instruction.  */
	      if (operand->insert)
		{
		  const char *errmsg = NULL;
		  insn = (*operand->insert) (insn, operand, mods,
					     value, &errmsg);
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
	      ++num_operands;
	    }
	}

      /* If we're at the end of the syntax string, we're done.  */
      /* FIXME: try to move this to a separate function.  */
      if (*syn == '\0')
	{
	  int i;
	  char *f;

	  /* For the moment we assume a valid `str' can only contain blanks
	     now.  IE: We needn't try again with a longer version of the
	     insn and it is assumed that longer versions of insns appear
	     before shorter ones (eg: lsr r2,r3,1 vs lsr r2,r3).  */

	  while (isspace (*str))
	    ++str;

	  if (*str != '\0'
#ifndef VERTICAL_BAR_SEPARATOR
	      && lower_p
#endif
	      )
	    as_bad ("junk at end of line: `%s'", str);

	  /* Write out the instruction.
	     It is important to fetch enough space in one call to `frag_more'.
	     We use (f - frag_now->fr_literal) to compute where we are and we
	     don't want frag_now to change between calls.  */
	  f = frag_more (4);
	  md_number_to_chars (f, insn, 4);

	  /* Create any fixups.  */
	  for (i = 0; i < fc; ++i)
	    {
	      int op_type, reloc_type;
	      const struct txvu_operand *operand;

	      /* Create a fixup for this operand.
		 At this point we do not use a bfd_reloc_code_real_type for
		 operands residing in the insn, but instead just use the
		 operand index.  This lets us easily handle fixups for any
		 operand type, although that is admittedly not a very exciting
		 feature.  We pick a BFD reloc type in md_apply_fix.  */

	      op_type = fixups[i].opindex;
	      reloc_type = op_type + (int) BFD_RELOC_UNUSED;
	      operand = &txvu_operands[op_type];
	      fix_new_exp (frag_now,
			   ((f - frag_now->fr_literal)
			    + (operand->flags & TXVU_OPERAND_LIMM ? 4 : 0)), 4,
			   &fixups[i].exp,
			   (operand->flags & TXVU_OPERAND_RELATIVE_BRANCH) != 0,
			   (bfd_reloc_code_real_type) reloc_type);
	    }

	  /* All done.  */
	  return str;
	}

      /* Try the next entry.  */
    }

  as_bad ("bad instruction `%s'", start);
  return 0;
}

void 
md_operand (expressionP)
     expressionS *expressionP;
{
}

valueT
md_section_align (segment, size)
     segT segment;
     valueT size;
{
  int align = bfd_get_section_alignment (stdoutput, segment);
  return ((size + (1 << align) - 1) & (-1 << align));
}

symbolS *
md_undefined_symbol (name)
  char *name;
{
  return 0;
}

/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from_section (fixP, sec)
     fixS *fixP;
     segT sec;
{
  if (fixP->fx_addsy != (symbolS *) NULL
      && (! S_IS_DEFINED (fixP->fx_addsy)
	  || S_GET_SEGMENT (fixP->fx_addsy) != sec))
    {
      /* The symbol is undefined (or is defined but not in this section).
	 Let the linker figure it out.  */
      return 0;
    }

  /* FIXME: `& -16L'? */
  return (fixP->fx_frag->fr_address + fixP->fx_where) & -4L;
}

/* Apply a fixup to the object code.  This is called for all the
   fixups we generated by calls to fix_new_exp.  At this point all symbol
   values should be fully resolved, and we attempt to completely resolve the
   reloc.  If we can not do that, we determine the correct reloc code and put
   it back in the fixup.  */

int
md_apply_fix3 (fixP, valueP, seg)
     fixS *fixP;
     valueT *valueP;
     segT seg;
{
  char *where = fixP->fx_frag->fr_literal + fixP->fx_where;
  valueT value;

  as_fatal ("txvu md_apply_fix3\n");
}

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent *
tc_gen_reloc (section, fixp)
     asection *section;
     fixS *fixp;
{
  /* relocs not handled yet */
  as_fatal ("txvu tc_gen_reloc\n");
}

/* Write a value out to the object file, using the appropriate endianness.  */

void
md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
  if (target_big_endian)
    number_to_chars_bigendian (buf, val, n);
  else
    number_to_chars_littleendian (buf, val, n);
}

/* Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message is returned, or NULL on OK.
*/

/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

char *
md_atof (type, litP, sizeP)
     char type;
     char *litP;
     int *sizeP;
{
  int i,prec;
  LITTLENUM_TYPE words[MAX_LITTLENUMS];
  LITTLENUM_TYPE *wordP;
  char *t;
  char *atof_ieee ();

  switch (type)
    {
    case 'f':
    case 'F':
    case 's':
    case 'S':
      prec = 2;
      break;

    case 'd':
    case 'D':
    case 'r':
    case 'R':
      prec = 4;
      break;

   /* FIXME: Some targets allow other format chars for bigger sizes here.  */

    default:
      *sizeP = 0;
      return "Bad call to md_atof()";
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;
  *sizeP = prec * sizeof (LITTLENUM_TYPE);

  if (target_big_endian)
    {
      for (i = 0; i < prec; i++)
	{
	  md_number_to_chars (litP, (valueT) words[i], sizeof (LITTLENUM_TYPE));
	  litP += sizeof (LITTLENUM_TYPE);
	}
    }
  else
    {
      for (i = prec - 1; i >= 0; i--)
	{
	  md_number_to_chars (litP, (valueT) words[i], sizeof (LITTLENUM_TYPE));
	  litP += sizeof (LITTLENUM_TYPE);
	}
    }
     
  return 0;
}
