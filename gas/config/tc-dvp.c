/* tc-dvp.c -- Assembler for the DVP
   Copyright (C) 1997, 1998 Free Software Foundation.

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
#include "sysdep.h"
#include "as.h"
#include "subsegs.h"
/* Needed by opcode/dvp.h.  */
#include "dis-asm.h"
#include "opcode/dvp.h"
#include "elf/mips.h"

static long parse_dma_ild_autocount ();
static long parse_dma_ptr_autocount ();

static void insert_operand 
     PARAMS ((dvp_cpu, const dvp_opcode *, const dvp_operand *, int,
	      DVP_INSN *, offsetT, const char **));
static void insert_operand_final
     PARAMS ((dvp_cpu, const dvp_operand *, int,
	      DVP_INSN *, offsetT, char *, unsigned int));

static int insert_file PARAMS ((const char *));

static int cur_vif_insn_length PARAMS ((void));
static void install_vif_length PARAMS ((char *, int));

const char comment_chars[] = ";";
const char line_comment_chars[] = "#";
const char line_separator_chars[] = "!";
const char EXP_CHARS[] = "eE";
const char FLT_CHARS[] = "dD";

/* Current assembler state.
   Instructions like mpg and direct are followed by a restricted set of
   instructions.  In the case of a '*' length argument an end marker must
   be provided.  (e.g. mpg is followed by vu insns until a .EndMpg is
   seen).  */
typedef enum {
  ASM_INIT, ASM_MPG, ASM_DIRECT, ASM_UNPACK, ASM_GIF, ASM_VU
} asm_state;
static asm_state cur_asm_state = ASM_INIT;

/* For variable length instructions, pointer to the initial frag
   and pointer into that frag.  These only hold valid values if
   cur_asm_state is one of ASM_MPG, ASM_DIRECT, ASM_UNPACK.  */
static fragS *cur_varlen_frag;
static char *cur_varlen_insn;
/* The length value specified in the insn, or -1 if '*'.  */
static int cur_varlen_value;

/* Non-zero if packing vif instructions in dma tags.  */
static int dma_pack_vif_p;

/* Non-zero if dma insns are to be included in the output.
   This is the default, but writing "if (! no_dma)" is klunky.  */
static int output_dma = 1;
/* Non-zero if vif insns are to be included in the output.  */
static int output_vif = 1;

const char *md_shortopts = "";

struct option md_longopts[] =
{
#define OPTION_NO_DMA (OPTION_MD_BASE + 1)
  { "no-dma", no_argument, NULL, OPTION_NO_DMA },
#define OPTION_NO_DMA_VIF (OPTION_NO_DMA + 1)
  { "no-dma-vif", no_argument, NULL, OPTION_NO_DMA_VIF },

  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
    case OPTION_NO_DMA :
      output_dma = 0;
      break;
    case OPTION_NO_DMA_VIF :
      output_dma = 0;
      output_vif = 0;
      break;
    default :
      return 0;
    }
  return 1;
}

void
md_show_usage (stream)
  FILE *stream;
{
  fprintf (stream, "\
DVP options:\n\
-no-dma			do not include DMA instructions in the output\n\
-no-dma-vif		do not include DMA or VIF instructions in the output\n\
");
} 

/* Set by md_assemble for use by dvp_fill_insn.  */
static subsegT prev_subseg;
static segT prev_seg;

static void s_dmadata PARAMS ((int));
static void s_dmadata_implied PARAMS ((int));
static void s_enddmadata PARAMS ((int));
static void s_dmapackvif PARAMS ((int));
static void s_enddirect PARAMS ((int));
static void s_endgif PARAMS ((int));
static void s_endmpg PARAMS ((int));
static void s_endunpack PARAMS ((int));
static void s_state PARAMS ((int));

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
    { "dmadata", s_dmadata, 0 },
    { "dmapackvif", s_dmapackvif, 0 },
    { "enddirect", s_enddirect, 0 },
    { "enddmadata", s_enddmadata, 0 },
    { "endgif", s_endgif, 0 },
    { "endmpg", s_endmpg, 0 },
    { "endunpack", s_endunpack, 0 },
    /* .vu,.gif added to simplify debugging */
    { "vu", s_state, ASM_VU },
    { "gif", s_state, ASM_GIF },
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
  dvp_opcode_init_tables (0);

  cur_asm_state = ASM_INIT;
  dma_pack_vif_p = 0;
}

/* We need to keep a list of fixups.  We can't simply generate them as
   we go, because that would require us to first create the frag, and
   that would screw up references to ``.''.  */

struct dvp_fixup
{
  /* index into `dvp_operands' */
  int opindex;
  /* byte offset from beginning of instruction */
  int offset;
  expressionS exp;
};

#define MAX_FIXUPS 5

static int fixup_count;
static struct dvp_fixup fixups[MAX_FIXUPS];

/* Given a cpu type and operand number, return a temporary reloc type
   for use in generating the fixup that encodes the cpu type and operand.  */
static int encode_fixup_reloc_type PARAMS ((dvp_cpu, int));
/* Given an encoded fixup reloc type, decode it into cpu and operand.  */
static void decode_fixup_reloc_type PARAMS ((int, dvp_cpu *,
					     const dvp_operand **));

static void assemble_dma PARAMS ((char *));
static void assemble_gif PARAMS ((char *));
static void assemble_vif PARAMS ((char *));
static void assemble_vu PARAMS ((char *));
static const dvp_opcode * assemble_vu_insn PARAMS ((dvp_cpu,
						    const dvp_opcode *,
						    const dvp_operand *,
						    char **, char *));
static const dvp_opcode * assemble_one_insn PARAMS ((dvp_cpu,
						     const dvp_opcode *,
						     const dvp_operand *,
						     char **, DVP_INSN *));

/* Main entry point for assembling an instruction.  */

void
md_assemble (str)
     char *str;
{
  /* Skip leading white space.  */
  while (isspace (*str))
    str++;

  if (cur_asm_state == ASM_INIT)
    {
      if (strncasecmp (str, "dma", 3) == 0)
	assemble_dma (str);
      else
	assemble_vif (str);
    }
  else if (cur_asm_state == ASM_GIF)
    assemble_gif (str);
  else if (cur_asm_state == ASM_VU
	   || cur_asm_state == ASM_MPG)
    assemble_vu (str);
  else
    as_fatal ("unknown parse state");
}

/* Subroutine of md_assemble to assemble DMA instructions.  */

static void
assemble_dma (str)
     char *str;
{
    DVP_INSN insn_buf[4];
    int len;				/* Insn's length, in 32 bit words.  */
    char *f;                            /* Pointer to allocated frag.  */
    int i;
    const dvp_opcode *opcode;

    /*
    Fill the first two words with VIF NOPs.
    They may be over-written later if DmaPackPke is on.
    initialize the remainder with zeros.
    */
    insn_buf[ 0] = 0;
    insn_buf[ 1] = 0;
    insn_buf[ 2] = 0;
    insn_buf[ 3] = 0;

    opcode = assemble_one_insn (DVP_DMA,
        		        dma_opcode_lookup_asm (str), dma_operands,
			        &str, insn_buf);
    if( opcode == NULL) return;
    if( !output_dma) return;

    len = 4;
    f = frag_more( len * 4);

    /* Write out the VIF / DMA instructions. */
    for( i = 0; i < len; ++i)
	md_number_to_chars( f + i * 4, insn_buf[i], 4);

    /* Create any fixups.  */
    /* FIXME: It might eventually be possible to combine all the various
       copies of this bit of code.  */
    for( i = 0; i < fixup_count; ++i)
    {
	int op_type, reloc_type, offset;
	const dvp_operand *operand;

#if 0
	/*
	Create a fixup for this operand.
	At this point we do not use a bfd_reloc_code_real_type for
	operands residing in the insn, but instead just use the
	operand index.  This lets us easily handle fixups for any
	operand type, although that is admittedly not a very exciting
	feature.  We pick a BFD reloc type in md_apply_fix.
	*/

	op_type = fixups[i].opindex;
	offset = fixups[i].offset;
	reloc_type = encode_fixup_reloc_type (DVP_VIF, op_type);
	operand = &vif_operands[op_type];
	fix_new_exp (frag_now, f + offset - frag_now->fr_literal, 4,
		     &fixups[i].exp,
		     (operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0,
		     (bfd_reloc_code_real_type) reloc_type);
#endif
    }
}

/* Subroutine of md_assemble to assemble VIF instructions.  */

static void
assemble_vif (str)
     char *str;
{
  /* Space for the instruction.
     The variable length insns can require much more space than this.
     It is allocated later, when we know we have such an insn.  */
  DVP_INSN insn_buf[5];
  /* Insn's length, in 32 bit words.  */
  int len;
  /* Pointer to allocated frag.  */
  char *f;
  int i;
  const dvp_opcode *opcode;

  opcode = assemble_one_insn (DVP_VIF,
			      vif_opcode_lookup_asm (str), vif_operands,
			      &str, insn_buf);
  if (opcode == NULL)
    return;

  if (opcode->flags & VIF_OPCODE_LENVAR)
    len = 1; /* actual data follows later */
  else if (opcode->flags & VIF_OPCODE_LEN2)
    len = 2;
  else if (opcode->flags & VIF_OPCODE_LEN5)
    len = 5;
  else
    len = 1;

  /* We still have to switch modes (if mpg for example) so we can't exit
     early if -no-vif.  */
  if (output_vif)
    {
      /* Reminder: it is important to fetch enough space in one call to
	 `frag_more'.  We use (f - frag_now->fr_literal) to compute where
	 we are and we don't want frag_now to change between calls.  */
      f = frag_more (len * 4);

      /* Write out the instruction.  */
      for (i = 0; i < len; ++i)
	md_number_to_chars (f + i * 4, insn_buf[i], 4);

      /* Create any fixups.  */
      /* FIXME: It might eventually be possible to combine all the various
	 copies of this bit of code.  */
      for (i = 0; i < fixup_count; ++i)
	{
	  int op_type, reloc_type, offset;
	  const dvp_operand *operand;

	  /* Create a fixup for this operand.
	     At this point we do not use a bfd_reloc_code_real_type for
	     operands residing in the insn, but instead just use the
	     operand index.  This lets us easily handle fixups for any
	     operand type, although that is admittedly not a very exciting
	     feature.  We pick a BFD reloc type in md_apply_fix.  */

	  op_type = fixups[i].opindex;
	  offset = fixups[i].offset;
	  reloc_type = encode_fixup_reloc_type (DVP_VIF, op_type);
	  operand = &vif_operands[op_type];
	  fix_new_exp (frag_now, f + offset - frag_now->fr_literal, 4,
		       &fixups[i].exp,
		       (operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0,
		       (bfd_reloc_code_real_type) reloc_type);
	}
    }

  /* Handle variable length insns.  */

  if (opcode->flags & VIF_OPCODE_LENVAR)
    {
      /* Name of file to read data from.  */
      char *file;
      /* Length in 32 bit words.  */
      int data_len;

      file = NULL;
      data_len = 0;
      vif_get_var_data (&file, &data_len);
      if (file)
	{
	  int byte_len = insert_file (file);
	  if (output_vif)
	    install_vif_length (f, byte_len);
	  /* Update $.MpgLoc.  */
	  vif_set_mpgloc (vif_get_mpgloc () + byte_len);
	}
      else
	{
	  /* data_len == -1 means the value must be computed from
	     the data.  */
	  if (data_len == 0 || data_len < -2)
	    as_bad ("invalid data length");
	  cur_varlen_frag = frag_now;
	  cur_varlen_insn = f;
	  cur_varlen_value = data_len;
	  if (opcode->flags & VIF_OPCODE_MPG)
	    cur_asm_state = ASM_MPG;
	  else if (opcode->flags & VIF_OPCODE_DIRECT)
	    cur_asm_state = ASM_DIRECT;
	  else if (opcode->flags & VIF_OPCODE_UNPACK)
	    cur_asm_state = ASM_UNPACK;
	}
    }
}

/* Subroutine of md_assemble to assemble GIF instructions.  */

static void
assemble_gif (str)
     char *str;
{
  DVP_INSN insn_buf[4];
  const dvp_opcode *opcode;

  opcode = assemble_one_insn (DVP_GIF,
			      gif_opcode_lookup_asm (str), gif_operands,
			      &str, insn_buf);
  if (opcode == NULL)
    return;
}

/* Subroutine of md_assemble to assemble VU instructions.  */

static void
assemble_vu (str)
     char *str;
{
  /* The lower instruction has the lower address.
     Handle this by grabbing 8 bytes now, and then filling each word
     as appropriate.  */
  char *f = frag_more (8);
  const dvp_opcode *opcode;

#ifdef VERTICAL_BAR_SEPARATOR
  char *p = strchr (str, '|');

  if (p == NULL)
    {
      as_bad ("lower slot missing in `%s'", str);
      return;
    }

  *p = 0;
  opcode = assemble_vu_insn (DVP_VUUP,
			     vu_upper_opcode_lookup_asm (str), vu_operands,
			     &str, f + 4);
  *p = '|';
  if (opcode == NULL)
    return;
  str = p + 1;
  assemble_vu_insn (DVP_VULO,
		    vu_lower_opcode_lookup_asm (str), vu_operands,
		    &str, f);
#else
  opcode = assemble_vu_insn (DVP_VUUP,
			     vu_upper_opcode_lookup_asm (str), vu_operands,
			     &str, f + 4);
  /* Don't assemble next one if we couldn't assemble the first.  */
  if (opcode)
    assemble_vu_insn (DVP_VULO,
		      vu_lower_opcode_lookup_asm (str), vu_operands,
		      &str, f);
#endif
}

static const dvp_opcode *
assemble_vu_insn (cpu, opcode, operand_table, pstr, buf)
     dvp_cpu cpu;
     const dvp_opcode *opcode;
     const dvp_operand *operand_table;
     char **pstr;
     char *buf;
{
  int i;
  DVP_INSN insn;

  opcode = assemble_one_insn (cpu, opcode, operand_table, pstr, &insn);
  if (opcode == NULL)
    return NULL;

  /* Write out the instruction.
     Reminder: it is important to fetch enough space in one call to
     `frag_more'.  We use (f - frag_now->fr_literal) to compute where
     we are and we don't want frag_now to change between calls.  */
  md_number_to_chars (buf, insn, 4);

  /* Create any fixups.  */
  for (i = 0; i < fixup_count; ++i)
    {
      int op_type, reloc_type;
      const dvp_operand *operand;

      /* Create a fixup for this operand.
	 At this point we do not use a bfd_reloc_code_real_type for
	 operands residing in the insn, but instead just use the
	 operand index.  This lets us easily handle fixups for any
	 operand type, although that is admittedly not a very exciting
	 feature.  We pick a BFD reloc type in md_apply_fix.  */

      op_type = fixups[i].opindex;
      reloc_type = encode_fixup_reloc_type (cpu, op_type);
      operand = &vu_operands[op_type];
      fix_new_exp (frag_now, buf - frag_now->fr_literal, 4,
		   &fixups[i].exp,
		   (operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0,
		   (bfd_reloc_code_real_type) reloc_type);
    }

  /* All done.  */
  return opcode;
}

/* Assemble one instruction at *PSTR.
   CPU indicates what component we're assembling for.
   The assembled instruction is stored in INSN_BUF.
   OPCODE is a pointer to the head of the hash chain.

   *PSTR is updated to point passed the parsed instruction.

   If the insn is successfully parsed the result is a pointer to the opcode
   entry that successfully matched and *PSTR is updated to point passed
   the parsed insn.  If an error occurs the result is NULL and *PSTR is left
   at some random point in the string (??? may wish to leave it pointing where
   the error occured).  */

static const dvp_opcode *
assemble_one_insn (cpu, opcode, operand_table, pstr, insn_buf)
     dvp_cpu cpu;
     const dvp_opcode *opcode;
     const dvp_operand *operand_table;
     char **pstr;
     DVP_INSN *insn_buf;
{
  char *start, *str;

  /* Keep looking until we find a match.  */

  start = str = *pstr;
  for ( ; opcode != NULL; opcode = DVP_OPCODE_NEXT_ASM (opcode))
    {
      int past_opcode_p, num_suffixes, num_operands;
      const unsigned char *syn;

      /* Ensure the mnemonic part matches.  */
      for (str = start, syn = opcode->mnemonic; *syn != '\0'; ++str, ++syn)
	if (tolower (*str) != tolower (*syn))
	  break;
      if (*syn != '\0')
	continue;

      /* Scan the syntax string.  If it doesn't match, try the next one.  */

      dvp_opcode_init_parse ();
      insn_buf[ opcode->opcode_word] = opcode->value;
      fixup_count = 0;
      past_opcode_p = 0;
      num_suffixes = 0;
      num_operands = 0;

      /* We don't check for (*str != '\0') here because we want to parse
	 any trailing fake arguments in the syntax string.  */
      for (/*str = start, */ syn = opcode->syntax; *syn != '\0'; )
	{
	  int mods,index;
	  const dvp_operand *operand;
	  const char *errmsg;

	  /* Non operand chars must match exactly.
	     Operand chars that are letters are not part of symbols
	     and are case insensitive.  */
	  if (*syn < 128)
	    {
	      if (tolower (*str) == tolower (*syn))
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
	  index = DVP_OPERAND_INDEX (*syn);
	  while (DVP_MOD_P (operand_table[index].flags))
	    {
	      mods |= operand_table[index].flags & DVP_MOD_BITS;
	      ++syn;
	      index = DVP_OPERAND_INDEX (*syn);
	    }
	  operand = operand_table + index;

	  if (operand->flags & DVP_OPERAND_FAKE)
	    {
	      if (operand->insert)
		{
		  errmsg = NULL;
		  (*operand->insert) (opcode, operand, mods, insn_buf, 0,
				      &errmsg);
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

	      if (!(operand->flags & DVP_OPERAND_SUFFIX))
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
	      /* ??? Hmmm ... there may not be any need to nul-terminate the
		 string, and it may in fact complicate things.  */
	      for (t = (*s == '.' || *s == '/' || *s == '[') ? s + 1 : s;
		   *t && (isalnum (*t) || *t == ']');
		   ++t)
		continue;
	      c = *t;
	      *t = '\0';
	      errmsg = NULL;
	      suf_value = (*operand->parse) (opcode, operand, mods, &s,
					     &errmsg);
	      *t = c;
	      if (errmsg)
		{
		  /* This can happen, for example, in ARC's in "blle foo" and
		     we're currently using the template "b%q%.n %j".  The "bl"
		     insn occurs later in the table so "lle" isn't an illegal
		     suffix.  */
		  break;
		}

	      /* Insert the suffix's value into the insn.  */
	      insert_operand (cpu, opcode, operand, mods, insn_buf,
			      (offsetT) suf_value, &errmsg);

	      /* FIXME: For suffixes that have a null "" value,
		 this next line is wrong as we will skip over something
		 we're not supposed to.  */
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

	      if (operand->flags & DVP_OPERAND_SUFFIX)
		as_fatal ("bad opcode table, suffix wrong");

#if 0 /* commas are in the syntax string now */
	      /* If this is not the first, there must be a comma.  */
	      if (num_operands > 0)
		{
		  if (*str != ',')
		    break;
		  ++str;
		}
#endif

	      if( operand->flags & DVP_OPERAND_DMA_ILD)
	      {
		  s_dmadata_implied( 0);
		  ++syn;
		  break;
	      }

	      /* Is there anything left to parse?
		 We don't check for this at the top because we want to parse
		 any trailing fake arguments in the syntax string.  */
	      /* ??? This doesn't allow operands with a legal value of "".  */
	      if (*str == '\0')
		break;

	      /* Parse the operand.  */
	      if( operand->flags & DVP_OPERAND_DMA_ILD_AUTOCOUNT)
	      {
		  errmsg = 0;
		  value = parse_dma_ild_autocount( opcode, operand, mods, insn_buf, &str, &errmsg);
		  if( errmsg) break;
	      }
	      else if( operand->flags & DVP_OPERAND_DMA_PTR_AUTOCOUNT)
	      {
		  errmsg = 0;
		  value = parse_dma_ptr_autocount( opcode, operand, mods, insn_buf, &str, &errmsg);
		  if( errmsg) break;
	      }
	      else if (operand->parse)
		{
		  errmsg = NULL;
		  value = (*operand->parse) (opcode, operand, mods,
					     &str, &errmsg);
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
		      if (fixup_count >= MAX_FIXUPS)
			as_fatal ("too many fixups");
		      fixups[fixup_count].exp = exp;
		      fixups[fixup_count].opindex = index;
		      fixups[fixup_count].offset = (operand->shift / 32) * 4;
		      ++fixup_count;
		      value = 0;
		    }
		}

	      /* Insert the register or expression into the instruction.  */
	      errmsg = NULL;
	      insert_operand (cpu, opcode, operand, mods, insn_buf,
			      (offsetT) value, &errmsg);
	      if (errmsg != (const char *) NULL)
		break;

	      ++syn;
	      ++num_operands;
	    }
	}

      /* If we're at the end of the syntax string, we're done.  */
      if (*syn == '\0')
	{
	  int i;

	  /* For the moment we assume a valid `str' can only contain blanks
	     now.  IE: We needn't try again with a longer version of the
	     insn and it is assumed that longer versions of insns appear
	     before shorter ones (eg: lsr r2,r3,1 vs lsr r2,r3).  */

	  while (isspace (*str))
	    ++str;

	  if (*str != '\0'
#ifndef VERTICAL_BAR_SEPARATOR
	      && cpu != DVP_VUUP
#endif
	      )
	    as_bad ("junk at end of line: `%s'", str);

	  /* It's now up to the caller to emit the instruction and any
	     relocations.  */
	  *pstr = str;
	  return opcode;
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

/* Called after parsing the file via md_after_pass_hook.  */

void
dvp_parse_done ()
{
  /* Check for missing .EndMpg, and supply one if necessary.  */
  if (cur_asm_state == ASM_MPG)
    s_endmpg (0);
  else if (cur_asm_state == ASM_DIRECT)
    s_enddirect (0);
  else if (cur_asm_state == ASM_UNPACK)
    s_endunpack (0);
}

/* Functions concerning relocs.  */

/* Spacing between each cpu type's operand numbers.
   Should be at least as bit as any operand table.  */
#define RELOC_SPACING 256

/* Given a cpu type and operand number, return a temporary reloc type
   for use in generating the fixup that encodes the cpu type and operand
   number.  */

static int
encode_fixup_reloc_type (cpu, opnum)
     dvp_cpu cpu;
     int opnum;
{
  return (int) BFD_RELOC_UNUSED + ((int) cpu * RELOC_SPACING) + opnum;
}

/* Given a fixup reloc type, decode it into cpu type and operand.  */

static void
decode_fixup_reloc_type (fixup_reloc, cpuP, operandP)
     int fixup_reloc;
     dvp_cpu *cpuP;
     const dvp_operand **operandP;
{
  dvp_cpu cpu = (fixup_reloc - (int) BFD_RELOC_UNUSED) / RELOC_SPACING;
  int opnum = (fixup_reloc - (int) BFD_RELOC_UNUSED) % RELOC_SPACING;

  *cpuP = cpu;
  switch (cpu)
    {
    case DVP_VUUP : *operandP = &vu_operands[opnum]; break;
    case DVP_VULO : *operandP = &vu_operands[opnum]; break;
    case DVP_DMA : *operandP = &dma_operands[opnum]; break;
    case DVP_VIF : *operandP = &vif_operands[opnum]; break;
    case DVP_GIF : *operandP = &gif_operands[opnum]; break;
    default : as_fatal ("bad fixup encoding");
    }
}

/* Given a fixup reloc type, return a pointer to the operand 

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
	 Let the linker figure it out.  +8: branch offsets are relative to the
	 delay slot.  */
      return 8;
    }

  /* We assume this is a vu branch.
     Offsets are calculated based on the address of the next insn.  */
  return ((fixP->fx_frag->fr_address + fixP->fx_where) & -8L) + 8;
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
    {
      value = *valueP;
    }
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

  /* Check for dvp operand's.  These are indicated with a reloc value
     >= BFD_RELOC_UNUSED.  */

  if ((int) fixP->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      dvp_cpu cpu;
      const dvp_operand *operand;
      DVP_INSN insn;

      decode_fixup_reloc_type ((int) fixP->fx_r_type,
			       & cpu, & operand);

      /* Fetch the instruction, insert the fully resolved operand
	 value, and stuff the instruction back again.  */
      insn = bfd_getl32 ((unsigned char *) where);
      insert_operand_final (cpu, operand, DVP_MOD_THIS_WORD, &insn,
			    (offsetT) value, fixP->fx_file, fixP->fx_line);
      bfd_putl32 ((bfd_vma) insn, (unsigned char *) where);

      if (fixP->fx_done)
	{
	  /* Nothing else to do here.  */
	  return 1;
	}

      /* Determine a BFD reloc value based on the operand information.
	 We are only prepared to turn a few of the operands into relocs.  */
      /* FIXME: This test is a hack.  */
      if ((operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0)
	{
	  assert ((operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0
		  && operand->bits == 11
		  && operand->shift == 0);
	  fixP->fx_r_type = BFD_RELOC_MIPS_DVP_11_PCREL;
	}
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
	case BFD_RELOC_8:
	  md_number_to_chars (where, value, 1);
	  break;
	case BFD_RELOC_16:
	  md_number_to_chars (where, value, 2);
	  break;
	case BFD_RELOC_32:
	  md_number_to_chars (where, value, 4);
	  break;
	default:
	  abort ();
	}
    }

  fixP->fx_addnumber = value;

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

  reloc = (arelent *) xmalloc (sizeof (arelent));

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

  assert (!fixP->fx_pcrel == !reloc->howto->pc_relative);

  reloc->addend = fixP->fx_addnumber;

  return reloc;
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

/*
Compute the auto-count value for a DMA tag with inline data.
*/
static long
parse_dma_ild_autocount( opcode, operand, mods, insn_buf, pstr, errmsg)
    const dvp_opcode *opcode;
    const dvp_operand *operand;
    int mods;
    DVP_INSN *insn_buf;
    char **pstr;
    const char **errmsg;
{
    char *start = *pstr;
    char *end = start;
    long retval;

#if 0
	/* FIXME: unfinished */
	evaluate the operand as an expression
	store the value to the count field
	compute the length as _$EndDma-.
#endif

    *pstr = end;
    return 0;
}

/* Scan a symbol and return a pointer to one past the end. */
#define issymchar(ch) (isalnum(ch) || ch == '_')
static char *
scan_symbol( sym)
    char *sym;
{
    while( *sym && issymchar( *sym))
        ++sym;
    return sym;
}

/*
Compute the auto-count value for a DMA tag with out-of-line data.
*/
static long
parse_dma_ptr_autocount( opcode, operand, mods, insn_buf, pstr, errmsg)
    const dvp_opcode *opcode;
    const dvp_operand *operand;
    int mods;
    DVP_INSN *insn_buf;
    char **pstr;
    const char **errmsg;
{
    char *start = *pstr;
    char *end = start;
    long retval;

    /* Data reference must be a .DmaData label. */
    struct symbol *label, *label2;
    const char *name;
    char *name2;
    int len;
    long count;

    label = label2 = 0;
    if( is_name_beginner( *start) )
    {
	char c;

	end = scan_symbol( start);
	c = *end;
	*end = 0;
	label = symbol_find( start);
	*end = c;
    }
    if( label )
    {
	name = S_GET_NAME( label);
	len = strlen( name) + 1;
	name2 = xmalloc( len + 2);
	name2[ 0] = '_';
	name2[ 1] = '$';
	memcpy( name2+2, name, len);	/* copy original name & \0 */
	label2 = symbol_find( name2);
	free( name2);
    }
    if( label == 0 || label2 == 0 )
    {
	*errmsg = "2nd operand must be a .DmaData label";
	return 0;
    }

    /* The second operand's value is the value of "symbol". */
    retval = S_GET_VALUE( label);

    /* The computed count value is val(symbol2) - val(symbol). */
    count = S_GET_VALUE( label2) - retval;

    /* Store the count field. */
    count &= 0x0000ffff;
    insn_buf[ 4] &= 0xffff0000;
    insn_buf[ 4] |= count & 0x0000ffff;

    *pstr = end;
    return retval;
}

/* Return length in bytes of the variable length VIF insn
   currently being assembled.  */

static int
cur_vif_insn_length ()
{
  int byte_len;
  fragS *f;

  if (cur_varlen_frag == frag_now)
    byte_len = frag_more (0) - cur_varlen_insn - 4; /* -4 for mpg itself */
  else
    {
      byte_len = (cur_varlen_frag->fr_fix + cur_varlen_frag->fr_offset -
		  (cur_varlen_insn - cur_varlen_frag->fr_literal)) - 4;
      for (f = cur_varlen_frag->fr_next; f != frag_now; f = f->fr_next)
	byte_len += f->fr_fix + f->fr_offset;
      byte_len += frag_now_fix ();
    }

  return byte_len;
}

/* Install length LEN, in bytes, in the vif insn at BUF.
   The bytes in BUF are in target order.  */

static void
install_vif_length (buf, len)
     char *buf;
     int len;
{
  char cmd = buf[3];

  if ((cmd & 0x70) == 0x40)
    {
      /* mpg */
      len /= 4;
      /* ??? Worry about data /= 4 cuts off?  */
      if (len > 256)
	as_bad ("`mpg' data length must be between 1 and 256");
      buf[2] = len == 256 ? 0 : len;
    }
  else if ((cmd & 0x70) == 0x50)
    {
      /* direct/directhl */
      /* ??? Worry about data /= 16 cuts off?  */
      len /= 16;
      if (len > 65536)
	as_bad ("`direct' data length must be between 1 and 65536");
      len = len == 65536 ? 0 : len;
      buf[0] = len;
      buf[1] = len >> 8;
    }
  else if ((cmd & 0x60) == 0x60)
    {
      /* unpack */
      /* FIXME */
    }
  else
    as_fatal ("bad call to install_vif_length");
}

/* Insert a file into the output.
   -I is used to specify where to find the file.
   The result is the number of bytes inserted.
   If an error occurs an error message is printed and zero is returned.  */

static int
insert_file (file)
     const char *file;
{
  FILE *f;
  char buf[256];
  int i, n, total;
  char *path;

  path = xmalloc (strlen (file) + include_dir_maxlen + 5 /*slop*/);
  f = NULL;
  for (i = 0; i < include_dir_count; i++)
    {
      strcpy (path, include_dirs[i]);
      strcat (path, "/");
      strcat (path, file);
      if ((f = fopen (path, FOPEN_RB)) != NULL)
	break;
    }
  free (path);
  if (f == NULL)
    f = fopen (file, FOPEN_RB);
  if (f == NULL)
    {
      as_bad ("unable to read file `%s'", file);
      return 0;
    }

  total = 0;
  do {
    n = fread (buf, 1, sizeof (buf), f);
    if (n > 0)
      {
	char *fr = frag_more (n);
	memcpy (fr, buf, n);
	total += n;
      }
  } while (n > 0);

  fclose (f);
  /* We assume the file is smaller than 2^31 bytes.
     Ok, we shouldn't make any assumptions.  Later.  */
  return total;
}

/* Insert an operand value into an instruction.  */

static void
insert_operand (cpu, opcode, operand, mods, insn_buf, val, errmsg)
     dvp_cpu cpu;
     const dvp_opcode *opcode;
     const dvp_operand *operand;
     int mods;
     DVP_INSN *insn_buf;
     offsetT val;
     const char **errmsg;
{
  if (operand->insert)
    {
      (*operand->insert) (opcode, operand, mods, insn_buf, (long) val, errmsg);
    }
  else
    {
#if 0 /* FIXME: revisit */
      /* We currently assume a field does not cross a word boundary.  */
      int shift = ((mods & DVP_MOD_THIS_WORD)
		   ? (operand->shift & 31)
		   : operand->shift);
      DVP_INSN *p = insn_buf + (shift / 32);
      if (operand->bits == 32)
	*p = val;
      else
	{
	  shift = shift % 32;
	  *p |= ((long) val & ((1 << operand->bits) - 1)) << shift;
	}
#else
      /* We currently assume a field does not cross a word boundary.  */
      if (operand->bits == 32)
	insn_buf[ operand->word] = val;
      else
	{
	  long temp = (long) val & ((1 << operand->bits) - 1);
	  insn_buf[ operand->word] |= temp << operand->shift;
	}
    }
#endif
}

/* Insert an operand's final value into an instruction.
   Here we can give warning messages about operand values if we want to.  */

static void
insert_operand_final (cpu, operand, mods, insn_buf, val, file, line)
     dvp_cpu cpu;
     const dvp_operand *operand;
     int mods;
     DVP_INSN *insn_buf;
     offsetT val;
     char *file;
     unsigned int line;
{
  if (operand->bits != 32)
    {
      offsetT min, max, test;

      if ((operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0)
	{
	  if ((val & 7) != 0)
	    {
	      if (file == (char *) NULL)
		as_warn ("branch to misaligned address");
	      else
		as_warn_where (file, line, "branch to misaligned address");
	    }
	  val >>= 3;
	}

      if ((operand->flags & DVP_OPERAND_SIGNED) != 0)
	{
	  if ((operand->flags & DVP_OPERAND_SIGNOPT) != 0)
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

      if ((operand->flags & DVP_OPERAND_NEGATIVE) != 0)
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

  {
    const char *errmsg = NULL;
    insert_operand (cpu, NULL, operand, mods, insn_buf, val, &errmsg);
    if (errmsg != NULL)
      as_warn_where (file, line, errmsg);
  }
}

static short dmadata_state = 0;
static const char *dmadata_name;

/* Non-zero if .DmaData was implied by a real (non-pseudo) opcode. */
static int implied_dmadata_p = 0;

static void
s_dmadata_implied( ignore)
    int ignore;
{
    if( dmadata_state != 0 )
    {
	as_bad( "DmaData blocks cannot be nested.");
    }
    dmadata_state = 1;
    dmadata_name = 0;
}

static void
s_dmadata( ignore)
    int ignore;
{
    char *name, c;

    dmadata_name = 0;

    if( dmadata_state != 0 )
    {
	as_bad( "DmaData blocks cannot be nested.");
	ignore_rest_of_line();
	return;
    }
    dmadata_state = 1;

    SKIP_WHITESPACE();			/* Leading whitespace is part of operand. */
    name = input_line_pointer;

    if( !is_name_beginner( *name) )
    {
	as_bad( "invalid identifier for \".DmaData\"");
	obstack_1grow( &cond_obstack, 0);  /*FIXME what is this for?*/
	ignore_rest_of_line();
	return;
    }

    c = get_symbol_end();
    line_label = colon( name);	/* user-defined label */
    dmadata_name = line_label->bsym->name;
    *input_line_pointer = c;

    demand_empty_rest_of_line();
}

static void
s_enddmadata( ignore)
    int ignore;
{
    if( dmadata_state != 1)
    {
	as_warn( ".EndDmaData encountered outside a DmaData block -- ignored.");
	ignore_rest_of_line();
	dmadata_name = 0;
    }
    dmadata_state = 0;
    demand_empty_rest_of_line();

    /*
    * "label" points to beginning of block
    * Create a name for the final label like _$<name>
    */
    if( dmadata_name) {
	int temp;
	char *name;

	temp = strlen( dmadata_name) + 1;
	name = xmalloc( temp + 2);
	name[ 0] = '_';
	name[ 1] = '$';
	memcpy( name+2, dmadata_name, temp);	/* copy original name & \0 */
	colon( name);
	free( name);
    }
}

static void
s_dmapackvif( ignore)
    int ignore;
{
    /* Syntax: .dmapackvif 0|1 */
    struct symbol *label;		/* Points to symbol */
    char *name;				/* points to name of symbol */

    SKIP_WHITESPACE();			/* Leading whitespace is part of operand. */
    switch( *input_line_pointer++ )
    {
    case '0':
	dma_pack_vif_p = 0;
	break;
    case '1':
	dma_pack_vif_p = 1;
	break;
    default:
	as_bad( "illegal argument to `.DmaPackPke'");
    }
    demand_empty_rest_of_line();
}

static void
s_enddirect (ignore)
     int ignore;
{
  int byte_len;

  if (cur_asm_state != ASM_DIRECT)
    {
      as_bad ("`.enddirect' has no matching `direct' instruction");
      return;
    }

  byte_len = cur_vif_insn_length ();
  if (cur_varlen_value != -1
      && cur_varlen_value * 16 != byte_len)
    as_warn ("length in `direct' instruction does not match length of data");
  if (output_vif)
    install_vif_length (cur_varlen_insn, byte_len);

  cur_asm_state = ASM_INIT;

  /* These needn't be reset, but to catch bugs they are.  */
  cur_varlen_frag = NULL;
  cur_varlen_insn = NULL;
  cur_varlen_value = 0;
}

static void
s_endmpg (ignore)
     int ignore;
{
  int byte_len;

  if (cur_asm_state != ASM_MPG)
    {
      as_bad ("`.endmpg' has no matching `mpg' instruction");
      return;
    }

  byte_len = cur_vif_insn_length ();
  if (cur_varlen_value != -1
      && cur_varlen_value * 8 != byte_len)
    as_warn ("length in `mpg' instruction does not match length of data");
  if (output_vif)
    install_vif_length (cur_varlen_insn, byte_len);

  cur_asm_state = ASM_INIT;

  /* These needn't be reset, but to catch bugs they are.  */
  cur_varlen_frag = NULL;
  cur_varlen_insn = NULL;
  cur_varlen_value = 0;

  /* Update $.MpgLoc.  */
  vif_set_mpgloc (vif_get_mpgloc () + byte_len);
}

static void
s_endunpack (ignore)
     int ignore;
{
  int byte_len;

  if (cur_asm_state != ASM_UNPACK)
    {
      as_bad ("`.endunpack' has no matching `unpack' instruction");
      return;
    }

  byte_len = cur_vif_insn_length ();
#if 0 /* unpack doesn't support prespecifying a length */
  if (cur_varlen_value * 16 != bytelen)
    as_warn ("length in `direct' instruction does not match length of data");
#endif
  if (output_vif)
    install_vif_length (cur_varlen_insn, byte_len);

  cur_asm_state = ASM_INIT;

  /* These needn't be reset, but to catch bugs they are.  */
  cur_varlen_frag = NULL;
  cur_varlen_insn = NULL;
  cur_varlen_value = 0;

  /* Update $.UnpackLoc.  */
  vif_set_unpackloc (vif_get_unpackloc () + byte_len);
}

static void
s_state (state)
     int state;
{
  cur_asm_state = state;
}

static void
s_endgif (ignore)
     int ignore;
{
}

/* Parse a DMA data spec which can be either of '*' or a quad word count.  */

static int
parse_dma_count( pstr, errmsg)
    char **pstr;
    const char **errmsg;
{
    char *str = *pstr;
    long count, value;
    expressionS exp;

    if( *str == '*' )
    {
	++*pstr;
	/* -1 is a special marker to caller to tell it the count is to be
	computed from the data. */
	return -1;
    }

    expression( &exp);
    if( exp.X_op == O_illegal
	|| exp.X_op == O_absent )
	;
    else if( exp.X_op == O_constant )
	value = exp.X_add_number;
    else if( exp.X_op == O_register )
	as_fatal( "got O_register");
    else
    {
	/* We need to generate a fixup for this expression.  */
	if( fixup_count >= MAX_FIXUPS )
	    as_fatal( "too many fixups");
	fixups[fixup_count].exp = exp;
	fixups[fixup_count].opindex = 0 /*FIXME*/;
	fixups[fixup_count].offset = 0 /*FIXME*/;
	++fixup_count;
	value = 0;
    }

    if( isdigit( *str) ) /*      ????????needs to accept an expression*/
    {
	char *start = str;
	while( *str && *str != ',' )
	    ++str;
	if( *str != ',' )
	{
	    *errmsg = "invalid dma count";
	    return 0;
	}
	count = atoi (start);
	*pstr = str;
	return(count);
    }

    *errmsg = "invalid dma count";
    return 0;
}
