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

#ifdef USE_STDARG
#include <stdarg.h>
#else
#include <varargs.h>
#endif

/* Value of VIF `nop' instruction.  */
#define VIFNOP 0

#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* Compute DMA operand index number of OP.  */
#define DMA_OPERAND_INDEX(op) ((op) - dma_operands)

/* Our local label prefix.  */
#define LOCAL_LABEL_PREFIX ".L"
/* Label prefix for end markers used in autocounts.  */
#define END_LABEL_PREFIX ".L.end."
/* Label to use for unique labels.  */
#define UNIQUE_LABEL_PREFIX ".L.dvptmp."
/* Prefix for mips version of labels defined in vu code.
   Note that symbols that begin with '$' are local symbols
   on mips targets, so we can't begin it with '$'.  */
#define VU_LABEL_PREFIX "._."

static long parse_float PARAMS ((char **, const char **));
static symbolS * create_label PARAMS ((const char *, const char *));
static symbolS * create_colon_label PARAMS ((int, const char *, const char *));
static char * unique_name PARAMS ((const char *));
static symbolS * compute_mpgloc PARAMS ((symbolS *, symbolS *, symbolS *));
static int compute_nloop PARAMS ((gif_type, int, int));
static void check_nloop PARAMS ((gif_type, int, int, int,
				 char *, unsigned int));
static long eval_expr PARAMS ((dvp_cpu, int, int, const char *, ...));
static long parse_dma_addr_autocount ();
static void inline_dma_data PARAMS ((int, DVP_INSN *));
static void setup_dma_autocount PARAMS ((const char *, DVP_INSN *, int));

static void insert_operand 
     PARAMS ((dvp_cpu, const dvp_opcode *, const dvp_operand *, int,
	      DVP_INSN *, offsetT, const char **));
static void insert_operand_final
     PARAMS ((dvp_cpu, const dvp_operand *, int,
	      DVP_INSN *, offsetT, char *, unsigned int));

static void insert_mpg_marker PARAMS ((unsigned long));
static void insert_unpack_marker PARAMS ((unsigned long));
static int insert_file PARAMS ((const char *,
				void (*) PARAMS ((unsigned long)),
				unsigned long, int));

static int vif_insn_type PARAMS ((char));
static int vif_length_value PARAMS ((char, int, int, int));
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
   seen).

   Allowed state transitions:
   ASM_INIT <--> ASM_MPG
                 ASM_DIRECT <--> ASM_GIF
                 ASM_UNPACK
                 ASM_VU

   FIXME: Make the ASM_INIT -> ASM_VU a one way transition.
   ".vu" must be seen at the top of the file,
   and cannot be switched out of.
*/

typedef enum {
  ASM_INIT, ASM_DIRECT, ASM_MPG, ASM_UNPACK, ASM_VU, ASM_GIF
} asm_state;

/* We need to maintain a stack of the current and previous status to handle
   such things as "direct ...; gifpacked ... ; .endgif ; .enddirect".  */
#define MAX_STATE_DEPTH 2
static asm_state asm_state_stack[MAX_STATE_DEPTH];
/* Current state's index in the stack.  */
static int cur_state_index;
/* Macro to fetch the current state.  */
#define CUR_ASM_STATE (asm_state_stack[cur_state_index])

/* Functions to push/pop the state stack.  */
static void push_asm_state PARAMS ((asm_state));
static void pop_asm_state PARAMS ((int));
static void set_asm_state PARAMS ((asm_state));

/* Set to non-zero if any non-vu insn seen.
   Used to control type of relocations emitted.  */
static int non_vu_insn_seen_p = 0;

/* Current cpu (machine variant) type state.
   We copy the mips16 way of recording what the current machine type is in
   the code.  A label is created whenever necessary and has an "other" value
   the denotes the machine type.  */
static dvp_cpu cur_cpu;
/* Record the current mach type.  */
static void record_mach PARAMS ((dvp_cpu, int));
/* Force emission of mach type label at next insn.
   This isn't static as TC_START_LABEL uses it.  */
int force_mach_label PARAMS ((void));
/* Given a dvp_cpu value, return the STO_DVP value to use.  */
static int cpu_sto PARAMS ((dvp_cpu, const char **));

/* Nonzero if inside .DmaData.  */
static int dma_data_state = 0;
/* Label of .DmaData (internally generated for inline data).  */
static const char *dma_data_name;

/* Variable length VIF insn support.  */
/* Label at start of insn's data.  */
static symbolS *vif_data_start;
/* Label at end of insn's data.  */
static symbolS *vif_data_end;

/* Special symbol $.mpgloc.  The value is in bytes.  */
static symbolS *mpgloc_sym;

/* GIF insn support.  */
/* Type of insn.  */
static int gif_insn_type;
/* Name of label of insn's data.  */
static const char *gif_data_name;
/* Pointer to frag of insn.  */
static fragS *gif_insn_frag;
/* Pointer to current gif insn in gif_insn_frag.  */
static char *gif_insn_frag_loc;
/* The length value specified in the insn, or -1 if '*'.  */
static int gif_user_value;

/* Count of vu insns seen since the last mpg.
   Set to -1 to disable automatic mpg insertion.  */
static int vu_count;

/* Non-zero if packing vif instructions in dma tags.  */
static int dma_pack_vif_p;

/* Non-zero if dma insns are to be included in the output.
   This is the default, but writing "if (! no_dma)" is klunky.  */
static int output_dma = 1;
/* Non-zero if vif insns are to be included in the output.  */
static int output_vif = 1;

/* Current opcode/operand for use by md_operand.  */
static const dvp_opcode *cur_opcode;
static const dvp_operand *cur_operand;

/* Options for the `caller' argument to s_endmpg.  */
typedef enum { ENDMPG_USER, ENDMPG_INTERNAL, ENDMPG_MIDDLE } endmpg_caller;

/* Relaxation support.  */
#define RELAX_MPG 1
#define RELAX_DIRECT 2
/* vu insns aren't relaxed, but they use machine dependent frags so we
   must handle them during relaxation */
#define RELAX_VU 3
#define RELAX_ENCODE(type, growth) (10 + (growth))
#define RELAX_GROWTH(state) ((state) - 10)
/* Return non-zero if STATE represents a relaxed state.  */
#define RELAX_DONE_P(state) ((state) >= 10)

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

static void s_dmadata PARAMS ((int));
static void s_enddmadata PARAMS ((int));
static void s_dmapackvif PARAMS ((int));
static void s_enddirect PARAMS ((int));
static void s_endmpg PARAMS ((int));
static void s_endunpack PARAMS ((int));
static void s_endgif PARAMS ((int));
static void s_state PARAMS ((int));

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  { "word", cons, 4 },
  { "quad", cons, 16 },
  { "dmadata", s_dmadata, 0 },
  { "dmapackvif", s_dmapackvif, 0 },
  { "enddirect", s_enddirect, 0 },
  { "enddmadata", s_enddmadata, 0 },
  { "endmpg", s_endmpg, ENDMPG_USER },
  { "endunpack", s_endunpack, 0 },
  { "endgif", s_endgif, 0 },
  { "vu", s_state, ASM_VU },
  { NULL, NULL, 0 }
};

void
md_begin ()
{
  /* Initialize the opcode tables.
     This involves computing the hash chains.  */
  dvp_opcode_init_tables (0);

  /* Force a mach type label for the first insn.  */
  force_mach_label ();

  /* Initialize the parsing state.  */
  cur_state_index = 0;
  set_asm_state (ASM_INIT);

  /* Pack vif insns in dma tags by default.  */
  dma_pack_vif_p = 1;

  /* Disable automatic mpg insertion.  */
  vu_count = -1;

  /* Create special symbols.  */
  mpgloc_sym = expr_build_uconstant (0);
}

/* We need to keep a list of fixups.  We can't simply generate them as
   we go, because that would require us to first create the frag, and
   that would screw up references to ``.''.  */

struct dvp_fixup
{
  /* the cpu this fixup is associated with */
  dvp_cpu cpu;
  /* index into `dvp_operands' */
  int opindex;
  /* byte offset from beginning of instruction */
  int offset;
  /* user specified value [when there is one] */
  int user_value;
  /* wl,cl values, only used with unpack insn */
  short wl,cl;
  /* the expression */
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
						     int, int,
						     char **, DVP_INSN *));

/* Main entry point for assembling an instruction.  */

void
md_assemble (str)
     char *str;
{
  /* Skip leading white space.  */
  while (isspace (*str))
    str++;

  /* After a gif tag, no insns can appear until a .endgif is seen.  */
  if (CUR_ASM_STATE == ASM_GIF)
    {
      as_bad ("missing .endgif");
      pop_asm_state (1);
    }
  /* Ditto for unpack.  */
  if (CUR_ASM_STATE == ASM_UNPACK)
    {
      as_bad ("missing .endunpack");
      pop_asm_state (1);
    }
#if 0 /* this doesn't work of course as gif insns may follow */
  /* Ditto for direct.  */
  if (CUR_ASM_STATE == ASM_DIRECT)
    {
      as_bad ("missing .enddirect");
      pop_asm_state (1);
    }
#endif

  if (CUR_ASM_STATE == ASM_INIT)
    {
      if (strncasecmp (str, "dma", 3) == 0)
	assemble_dma (str);
      else if (strncasecmp (str, "gif", 3) == 0)
	assemble_gif (str);
      else
	assemble_vif (str);
      non_vu_insn_seen_p = 1;
    }
  else if (CUR_ASM_STATE == ASM_DIRECT)
    {
      assemble_gif (str);
      non_vu_insn_seen_p = 1;
    }
  else if (CUR_ASM_STATE == ASM_VU
	   || CUR_ASM_STATE == ASM_MPG)
    assemble_vu (str);
  else
    as_fatal ("internal error: unknown parse state");
}

/* Subroutine of md_assemble to assemble DMA instructions.  */

static void
assemble_dma (str)
     char *str;
{
  DVP_INSN insn_buf[2];
  /* Insn's length, in 32 bit words.  */
  int len;
  /* Pointer to allocated frag.  */
  char *f;
  int i;
  const dvp_opcode *opcode;

  if (output_dma)
    {
      /* Do an implicit alignment to a 16 byte boundary.
	 Do it now so that inline dma data labels are at the right place.  */
      /* ??? One can certainly argue all this implicit alignment is
	 questionable.  The thing is assembler programming is all that will
	 mostly likely ever be done and not doing so forces an extra [and
	 arguably unnecessary] burden on the programmer.  */
      frag_align (4, 0, 0);
      record_alignment (now_seg, 4);
    }

  /* This is the DMA tag.  */
  insn_buf[0] = 0;
  insn_buf[1] = 0;

  opcode = assemble_one_insn (DVP_DMA,
			      dma_opcode_lookup_asm (str), dma_operands,
			      0, 0, &str, insn_buf);
  if (opcode == NULL)
    return;
  if (!output_dma)
    return;

  record_mach (DVP_DMA, 0);

  f = frag_more (8);

  /* Write out the DMA instruction. */
  for (i = 0; i < 2; ++i)
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
      reloc_type = encode_fixup_reloc_type (DVP_DMA, op_type);
      operand = &dma_operands[op_type];
      fix_new_exp (frag_now, f + offset - frag_now->fr_literal, 4,
		   &fixups[i].exp,
		   (operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0,
		   (bfd_reloc_code_real_type) reloc_type);
    }

  /* The upper two words are vif insns.  */
  record_mach (DVP_VIF, 0);

  /* If not doing dma/vif packing, fill out the insn with vif nops.
     ??? We take advantage of the fact that the default fill value of zero
     is the vifnop insn.  This occurs for example when handling mpg
     alignment.  It also occurs when one dma tag immediately follows the
     previous one.  */
  if (! dma_pack_vif_p)
    {
      f = frag_more (8);
      md_number_to_chars (f + 0, VIFNOP, 4);
      md_number_to_chars (f + 4, VIFNOP, 4);
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
  int i,wl,cl;
  const dvp_opcode *opcode;
  fragS * insn_frag;
  /* Name of file to read data from.  */
  const char *file;
  /* Length in 32 bit words.  */
  int data_len;
  /* Macro expansion, if there is one.  */
  char * macstr;

  /* First check for macros.  */
  macstr = dvp_expand_macro (vif_macros, vif_macro_count, str);
  if (macstr)
    {
      /* The macro may expand into several insns (delimited with '\n'),
	 so loop.  */
      char * next = macstr;
      do
	{
	  char *p = strchr (next, '\n');
	  if (p)
	    *p = 0;
	  assemble_vif (next);
	  next = p ? p + 1 : 0;
	}
      while (next);
      free (macstr);
      return;
    }

  opcode = assemble_one_insn (DVP_VIF,
			      vif_opcode_lookup_asm (str), vif_operands,
			      0, 0, &str, insn_buf);
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
      /* Record the mach before doing the alignment so that we properly
	 disassemble any inserted vifnop's.  For mpg and direct insns
	 force the recording of the mach type for the next insn.  The data
	 will switch the mach type and we want to ensure it's switched
	 back.  */

      if (opcode->flags & (VIF_OPCODE_MPG | VIF_OPCODE_DIRECT))
	record_mach (DVP_VIF, 1);
      else
	record_mach (DVP_VIF, 0);

      /* For variable length instructions record a fixup that is the symbol
	 marking the end of the data.  eval_expr will queue the fixup
	 which will then be emitted later.  */
      if (opcode->flags & VIF_OPCODE_LENVAR)
	{
	  char *name;

	  asprintf (&name, "%s%s", LOCAL_LABEL_PREFIX,
		    unique_name ("varlen"));
	  vif_data_end = symbol_new (name, now_seg, 0, 0);
	  symbol_table_insert (vif_data_end);
	  fixups[fixup_count].cpu = DVP_VIF;
	  fixups[fixup_count].exp.X_op = O_symbol;
	  fixups[fixup_count].exp.X_add_symbol = vif_data_end;
	  fixups[fixup_count].exp.X_add_number = 0;
	  fixups[fixup_count].opindex = vif_operand_datalen_special;
	  fixups[fixup_count].offset = 0;

	  /* See what the user specified.  */
	  vif_get_var_data (&file, &data_len);
	  if (file)
	    data_len = -1;
	  fixups[fixup_count].user_value = data_len;
	  /* Get the wl,cl values.  Only useful for the unpack insn but
	     it doesn't hurt to always record them.  */
	  vif_get_wl_cl (&wl, &cl);
	  fixups[fixup_count].wl = wl;
	  fixups[fixup_count].cl = cl;
	  ++fixup_count;
	}

      /* Obtain space in which to store the instruction.  */

      if (opcode->flags & VIF_OPCODE_MPG)
	{
	  /* The data must be aligned on a 64 bit boundary (so the mpg insn
	     comes just before that 64 bit boundary).
	     Do this by putting the mpg insn in a relaxable fragment
	     with a symbol that marks the beginning of the aligned data.  */

	  /* Ensure relaxable fragments are in their own fragment.
	     Otherwise md_apply_fix3 mishandles fixups to insns earlier
	     in the fragment (because we set fr_opcode for the `mpg' insn
	     because it can move in the fragment).  */
	  frag_wane (frag_now);
	  frag_new (0);

	  /* One could combine the previous two lines with the following.
	     They're not for clarity: keep separate the actions being
	     performed.  */

	  /* This dance with frag_grow is so we can record frag_now in
	     insn_frag.  frag_var always changes frag_now.  We must allocate
	     the maximal amount of space we need so there's room to move
	     the insn in the frag during relaxation.  */
	  frag_grow (8);
	  /* Allocate space for the fixed part.  */
	  f = frag_more (4);
	  insn_frag = frag_now;

	  frag_var (rs_machine_dependent,
		    4, /* max chars */
		    0, /* variable part is empty at present */
		    RELAX_MPG, /* subtype */
		    NULL, /* no symbol */
		    0, /* offset */
		    f); /* opcode */

	  frag_align (3, 0, 0);
	  record_alignment (now_seg, 3);

	  /* Put a symbol at the start of data.  The relaxation code uses
	     this to figure out how many bytes to insert.  $.mpgloc
	     calculations also use it.  */
	  vif_data_start = create_colon_label (STO_DVP_VU, LOCAL_LABEL_PREFIX,
					       unique_name ("mpg"));
	  insn_frag->fr_symbol = vif_data_start;

	  /* Get the value of mpgloc.  If it wasn't '*'
	     then update $.mpgloc.  */
	  {
	    int mpgloc = vif_get_mpgloc ();
	    if (mpgloc != -1)
	      {
		mpgloc_sym->sy_value.X_op = O_constant;
		/* The value is recorded in bytes.  */
		mpgloc_sym->sy_value.X_add_number = mpgloc * 8;
		mpgloc_sym->sy_value.X_unsigned = 1;
	      }
	  }
	}
      else if (opcode->flags & VIF_OPCODE_DIRECT)
	{
	  /* The data must be aligned on a 128 bit boundary (so the direct insn
	     comes just before that 128 bit boundary).
	     Do this by putting the direct insn in a relaxable fragment.
	     with a symbol that marks the beginning of the aligned data.  */

	  /* Ensure relaxable fragments are in their own fragment.
	     Otherwise md_apply_fix3 mishandles fixups to insns earlier
	     in the fragment (because we set fr_opcode for the `direct' insn
	     because it can move in the fragment).  */
	  frag_wane (frag_now);
	  frag_new (0);

	  /* One could combine the previous two lines with the following.
	     They're not for clarity: keep separate the actions being
	     performed.  */

	  /* This dance with frag_grow is so we can record frag_now in
	     insn_frag.  frag_var always changes frag_now.  We must allocate
	     the maximal amount of space we need so there's room to move
	     the insn in the frag during relaxation.  */
	  frag_grow (16);
	  /* Allocate space for the fixed part.  */
	  f = frag_more (4);
	  insn_frag = frag_now;

	  frag_var (rs_machine_dependent,
		    12, /* max chars */
		    0, /* variable part is empty at present */
		    RELAX_DIRECT, /* subtype */
		    NULL, /* no symbol */
		    0, /* offset */
		    f); /* opcode */

	  frag_align (4, 0, 0);
	  record_alignment (now_seg, 4);

	  /* Put a symbol at the start of data.  The relaxation code uses
	     this to figure out how many bytes to insert.  */
	  vif_data_start = create_colon_label (0, LOCAL_LABEL_PREFIX,
					       unique_name ("direct"));
	  insn_frag->fr_symbol = vif_data_start;
	}
      else if (opcode->flags & VIF_OPCODE_UNPACK)
	{
	  f = frag_more (len * 4);
	  insn_frag = frag_now;
	  /* Put a symbol at the start of data.  $.unpackloc calculations
	     use it.  */
	  /* ??? $.unpackloc is gone.  Is this also used for data length
	     verification?  */
	  vif_data_start = create_colon_label (STO_DVP_VIF, LOCAL_LABEL_PREFIX,
					       unique_name ("unpack"));
	}
      else
	{
	  /* Reminder: it is important to fetch enough space in one call to
	     `frag_more'.  We use (f - frag_now->fr_literal) to compute where
	     we are and we don't want frag_now to change between calls.  */
	  f = frag_more (len * 4);
	  insn_frag = frag_now;
	}

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
	  fixS *fixP;

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
	  fixP = fix_new_exp (insn_frag, f + offset - insn_frag->fr_literal, 4,
			      &fixups[i].exp,
			      (operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0,
			      (bfd_reloc_code_real_type) reloc_type);
	  fixP->tc_fix_data.user_value = fixups[i].user_value;
	  fixP->tc_fix_data.wl = fixups[i].wl;
	  fixP->tc_fix_data.cl = fixups[i].cl;

	  /* Set fx_tcbit so other parts of the code know this fixup is for
	     a vif insn.  */
	  fixP->fx_tcbit = 1;
	}
    }

  /* Handle variable length insns.  */

  if (opcode->flags & VIF_OPCODE_LENVAR)
    {
      /* See what the user specified.  */
      vif_get_var_data (&file, &data_len);

      if (file)
	{
	  int byte_len;

	  /* The handling for each of mpg,direct,unpack is basically the same:
	     - emit a label to set the mach type for the data we're inserting
	     - switch to the new assembler state
	     - insert the file
	     - call the `end' handler  */

	  if (opcode->flags & VIF_OPCODE_MPG)
	    {
	      record_mach (DVP_VUUP, 1);
	      set_asm_state (ASM_MPG);
	      byte_len = insert_file (file, insert_mpg_marker, 0, 256 * 8);
	      s_endmpg (ENDMPG_INTERNAL);
	    }
	  else if (opcode->flags & VIF_OPCODE_DIRECT)
	    {
	      record_mach (DVP_GIF, 1);
	      set_asm_state (ASM_DIRECT);
	      byte_len = insert_file (file, NULL, 0, 0);
	      s_enddirect (1);
	    }
	  else if (opcode->flags & VIF_OPCODE_UNPACK)
	    {
	      int max_len = 0; /*unpack_max_byte_len (insn_buf[0]);*/
	      set_asm_state (ASM_UNPACK);
	      byte_len = insert_file (file, NULL /*insert_unpack_marker*/,
				      insn_buf[0], max_len);
	      s_endunpack (1);
	    }
	  else
	    as_fatal ("internal error: unknown cpu type for variable length vif insn");
	}
      else /* file == NULL */
	{
	  /* data_len == -1 means the value must be computed from
	     the data.  */
	  if (data_len <= -2)
	    as_bad ("invalid data length");

	  if (output_vif && data_len != -1)
	    install_vif_length (f, data_len);

	  if (opcode->flags & VIF_OPCODE_MPG)
	    {
	      set_asm_state (ASM_MPG);
	      /* Enable automatic mpg insertion every 256 insns.  */
	      vu_count = 0;
	    }
	  else if (opcode->flags & VIF_OPCODE_DIRECT)
	    set_asm_state (ASM_DIRECT);
	  else if (opcode->flags & VIF_OPCODE_UNPACK)
	    set_asm_state (ASM_UNPACK);
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
  char *f;
  int i;

  insn_buf[0] = insn_buf[1] = insn_buf[2] = insn_buf[3] = 0;

  opcode = assemble_one_insn (DVP_GIF,
			      gif_opcode_lookup_asm (str), gif_operands,
			      0, 0, &str, insn_buf);
  if (opcode == NULL)
    return;

  /* Do an implicit alignment to a 16 byte boundary.  */
  frag_align (4, 0, 0);
  record_alignment (now_seg, 4);

  /* Insert a label so we can compute the number of quadwords when the
     .endgif is seen.  This is put before the mach type label because gif
     insns are followed by data and we don't want the disassembler to try
     to disassemble them as mips insns (since it uses the st_other field)
     of the closest label to choose the mach type and since we don't have
     a special st_other value for "data".  */
  gif_data_name = S_GET_NAME (create_colon_label (0, LOCAL_LABEL_PREFIX,
						  unique_name ("gifdata")));

  record_mach (DVP_GIF, 1);

  gif_insn_frag_loc = f = frag_more (16);
  gif_insn_frag = frag_now;
  for (i = 0; i < 4; ++i)
    md_number_to_chars (f + i * 4, insn_buf[i], 4);

  /* Record the type of the gif tag so we know how to compute nloop
     in s_endgif.  */
  if (strcmp (opcode->mnemonic, "gifpacked") == 0)
    gif_insn_type = GIF_PACKED;
  else if (strcmp (opcode->mnemonic, "gifreglist") == 0)
    gif_insn_type = GIF_REGLIST;
  else if (strcmp (opcode->mnemonic, "gifimage") == 0)
    gif_insn_type = GIF_IMAGE;
  else
    abort ();
  push_asm_state (ASM_GIF);
}

/* Subroutine of md_assemble to assemble VU instructions.  */

static void
assemble_vu (str)
     char *str;
{
  int i;
  char *f;
  const dvp_opcode *opcode;
  /* The lower instruction has the lower address so insns[0] = lower insn,
     insns[1] = upper insn.  */
  DVP_INSN insns[2];
  fragS * insn_frag;

  /* Handle automatic mpg insertion if enabled.  */
  if (CUR_ASM_STATE == ASM_MPG
      && vu_count == 256)
    insert_mpg_marker (0);

  /* Do an implicit alignment to a 8 byte boundary.  */
  frag_align (3, 0, 0);
  record_alignment (now_seg, 3);

  record_mach (DVP_VUUP, 0);

#ifdef VERTICAL_BAR_SEPARATOR
  char *p = strchr (str, '|');

  if (p == NULL)
    {
      as_bad ("lower slot missing in `%s'", str);
      return;
    }

  *p = 0;
  opcode = assemble_one_insn (DVP_VUUP,
			      vu_upper_opcode_lookup_asm (str), vu_operands,
			      0, 4, &str, &insns[1]);
  *p = '|';
  str = p + 1;
#else
  opcode = assemble_one_insn (DVP_VUUP,
			     vu_upper_opcode_lookup_asm (str), vu_operands,
			     0, 4, &str, &insns[1]);
#endif

  /* Don't assemble next one if we couldn't assemble the first.  */
  if (opcode == NULL)
    return;

  /* Assemble the lower insn.
     Pass `fixup_count' for `init_fixup_count' so that we don't clobber
     any fixups the upper insn had.  */
  opcode = assemble_one_insn (DVP_VULO,
			      vu_lower_opcode_lookup_asm (str), vu_operands,
			      fixup_count, 0, &str, &insns[0]);
  if (opcode == NULL)
    return;

  /* If there were fixups and we're inside mpg, create a machine dependent
     fragment so that we can record the current value of $.mpgloc in fr_symbol.
     Reminder: it is important to fetch enough space in one call to
     `frag_more'.  We use (f - frag_now->fr_literal) to compute where
     we are and we don't want frag_now to change between calls.  */
  if (fixup_count != 0
      && CUR_ASM_STATE == ASM_MPG)
    {
      symbolS * cur_mpgloc;

      /* Ensure we get a new frag.  */
      frag_wane (frag_now);
      frag_new (0);

      /* Compute the current $.mpgloc.  */
      cur_mpgloc = compute_mpgloc (mpgloc_sym, vif_data_start,
				   expr_build_dot ());

      /* We need to use frag_now afterwards, so we can't just call frag_var.
	 Instead we use frag_more and save the value of frag_now in
	 insn_frag.  */
      f = frag_more (8);
      insn_frag = frag_now;
      /* Turn the frag into a machine dependent frag.  */
      frag_variant (rs_machine_dependent,
		    0, /* max chars */
		    0, /* no variable part */
		    RELAX_VU, /* subtype */
		    cur_mpgloc, /* $.mpgloc */
		    0, /* offset */
		    NULL); /* opcode */
    }
  else
    {
      f = frag_more (8);
      insn_frag = frag_now;
    }

  /* Write out the instructions.  */
  md_number_to_chars (f, insns[0], 4);
  md_number_to_chars (f + 4, insns[1], 4);

  /* Create any fixups.  */
  for (i = 0; i < fixup_count; ++i)
    {
      int op_type, reloc_type;
      const dvp_operand *operand;
      dvp_cpu cpu;

      /* Create a fixup for this operand.
	 At this point we do not use a bfd_reloc_code_real_type for
	 operands residing in the insn, but instead just use the
	 operand index.  This lets us easily handle fixups for any
	 operand type, although that is admittedly not a very exciting
	 feature.  We pick a BFD reloc type in md_apply_fix.  */

      cpu = fixups[i].cpu;
      op_type = fixups[i].opindex;
      reloc_type = encode_fixup_reloc_type (cpu, op_type);
      operand = &vu_operands[op_type];

      /* Branch operands inside mpg have to be handled specially.
	 We want a pc relative relocation in a section different from our own.
	 See the br-2.s dejagnu testcase for a good example.  */
      if (CUR_ASM_STATE == ASM_MPG
	  && (operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0)
	{
	  symbolS *e1,*e2,*diff_expr;

	  /* For "br foo" we want "foo - (. + 8)".  */
	  e1 = expr_build_binary (O_add, insn_frag->fr_symbol,
				  expr_build_uconstant (8));
	  e2 = make_expr_symbol (&fixups[i].exp);
	  diff_expr = expr_build_binary (O_subtract, e2, e1);
	  fixups[i].exp.X_op = O_symbol; 
	  fixups[i].exp.X_add_symbol = diff_expr;
	  fixups[i].exp.X_add_number = 0;
	}

      fix_new_exp (insn_frag, f + fixups[i].offset - insn_frag->fr_literal, 4,
		   &fixups[i].exp,
		   CUR_ASM_STATE == ASM_MPG /* pcrel */
		   ? 0
		   : (operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0,
		   (bfd_reloc_code_real_type) reloc_type);
    }

  /* If this was the "loi" pseudo-insn, we need to set the `i' bit.  */
  if (strcmp (opcode->mnemonic, "loi") == 0)
    f[7] |= 0x80;

  /* Increment the vu insn counter.
     If get reach 256 we need to insert an `mpg'.  */
  ++vu_count;
}

/* Assemble one instruction at *PSTR.
   CPU indicates what component we're assembling for.
   The assembled instruction is stored in INSN_BUF.
   OPCODE is a pointer to the head of the hash chain.
   INIT_FIXUP_COUNT is the initial value for `fixup_count'.
   It exists to allow the fixups for multiple calls to this insn to be
   queued up before actually emitting them.
   *PSTR is updated to point passed the parsed instruction.

   If the insn is successfully parsed the result is a pointer to the opcode
   entry that successfully matched and *PSTR is updated to point passed
   the parsed insn.  If an error occurs the result is NULL and *PSTR is left
   at some random point in the string (??? may wish to leave it pointing where
   the error occured).  */

static const dvp_opcode *
assemble_one_insn (cpu, opcode, operand_table, init_fixup_count, fixup_offset,
		   pstr, insn_buf)
     dvp_cpu cpu;
     const dvp_opcode *opcode;
     const dvp_operand *operand_table;
     int init_fixup_count;
     int fixup_offset;
     char **pstr;
     DVP_INSN *insn_buf;
{
  char *start, *str;

  /* Keep looking until we find a match.  */

  start = str = *pstr;
  for ( ; opcode != NULL; opcode = DVP_OPCODE_NEXT_ASM (opcode))
    {
      int past_opcode_p, num_suffixes;
      const unsigned char *syn;

      /* Ensure the mnemonic part matches.  */
      for (str = start, syn = opcode->mnemonic; *syn != '\0'; ++str, ++syn)
	if (tolower (*str) != tolower (*syn))
	  break;
      if (*syn != '\0')
	continue;

      /* Scan the syntax string.  If it doesn't match, try the next one.  */

      dvp_opcode_init_parse ();
      insn_buf[opcode->opcode_word] = opcode->value;
      fixup_count = init_fixup_count;
      past_opcode_p = 0;
      num_suffixes = 0;

      /* We don't check for (*str != '\0') here because we want to parse
	 any trailing fake arguments in the syntax string.  */
      for (/*str = start, */ syn = opcode->syntax; *syn != '\0'; )
	{
	  int mods,index;
	  const dvp_operand *operand;
	  const char *errmsg;
	  long value;

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
	      long value = 0;

	      if (operand->flags & DVP_OPERAND_DMA_INLINE)
		{
		  inline_dma_data ((mods & DVP_OPERAND_AUTOCOUNT) != 0,
				  insn_buf);
		  ++syn;
		  continue;
		}

	      if (operand->parse)
		{
		  errmsg = NULL;
		  value = (*operand->parse) (opcode, operand, mods,
					     &str, &errmsg);
		  if (errmsg)
		    break;
		}
	      if (operand->insert)
		{
		  errmsg = NULL;
		  (*operand->insert) (opcode, operand, mods, insn_buf,
				      (offsetT) value, &errmsg);
		  /* If we get an error, go on to try the next insn.  */
		  if (errmsg)
		    break;
		}
	      ++syn;
	      continue;
	    }

	  /* Are we finished with suffixes?  */
	  if (!past_opcode_p)
	    {
	      long suf_value;

	      if (!(operand->flags & DVP_OPERAND_SUFFIX))
		as_fatal ("internal error: bad opcode table, missing suffix flag");

	      /* If we're at a space in the input string, we want to skip the
		 remaining suffixes.  There may be some fake ones though, so
		 just go on to try the next one.  */
	      if (*str == ' ')
		{
		  ++syn;
		  continue;
		}

	      /* Parse the suffix.  */
	      errmsg = NULL;
	      suf_value = (*operand->parse) (opcode, operand, mods, &str,
					     &errmsg);
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

	      ++syn;
	      continue;
	    }

	  /* This is an operand, either a register or an expression of
	     some kind.  */

	  value = 0;

	  if (operand->flags & DVP_OPERAND_SUFFIX)
	    as_fatal ("internal error: bad opcode table, suffix wrong");

	  /* Is there anything left to parse?
	     We don't check for this at the top because we want to parse
	     any trailing fake arguments in the syntax string.  */
	  /* ??? This doesn't allow operands with a legal value of "".  */
	  if (*str == '\0')
	    break;

	  /* Parse the operand.  */
	  if (operand->flags & DVP_OPERAND_FLOAT)
	    {
	      errmsg = 0;
	      value = parse_float (&str, &errmsg);
	      if (errmsg)
		break;
	    }
	  else if ((operand->flags & DVP_OPERAND_DMA_ADDR)
		   && (mods & DVP_OPERAND_AUTOCOUNT))
	    {
	      errmsg = 0;
	      value = parse_dma_addr_autocount (opcode, operand, mods,
						insn_buf, &str, &errmsg);
	      if (errmsg)
		break;
	    }
	  else
	    {
	      char *origstr,*hold;
	      expressionS exp;

	      /* First see if there is a special parser.  */
	      origstr = str;
	      if (operand->parse)
		{
		  errmsg = NULL;
		  value = (*operand->parse) (opcode, operand, mods,
					     &str, &errmsg);
		  if (errmsg)
		    break;
		}

	      /* If there wasn't a special parser, or there was and it
		 left the input stream unchanged, use the general
		 expression parser.  */
	      if (str == origstr)
		{
		  hold = input_line_pointer;
		  input_line_pointer = str;
		  /* Set cur_{opcode,operand} for md_operand.  */
		  cur_opcode = opcode;
		  cur_operand = operand;
		  expression (&exp);
		  cur_opcode = NULL;
		  str = input_line_pointer;
		  input_line_pointer = hold;

		  if (exp.X_op == O_illegal
		      || exp.X_op == O_absent)
		    break;
		  else if (exp.X_op == O_constant)
		    value = exp.X_add_number;
		  else if (exp.X_op == O_register)
		    as_fatal ("internal error: got O_register");
		  else
		    {
		      /* We need to generate a fixup for this expression.  */
		      if (fixup_count >= MAX_FIXUPS)
			as_fatal ("internal error: too many fixups");
		      fixups[fixup_count].cpu = cpu;
		      fixups[fixup_count].exp = exp;
		      fixups[fixup_count].opindex = index;
		      /* FIXME: Revisit.  Do we really need operand->word?
			 The endianness of a 128 bit DMAtag is rather
			 twisted.  How about defining word 0 as the word with
			 the lowest address and basing operand-shift off that.
			 operand->word could then be deleted.  */
		      fixups[fixup_count].offset = fixup_offset;
		      if (operand->word != 0)
			fixups[fixup_count].offset += operand->word * 4;
		      else
			fixups[fixup_count].offset += (operand->shift / 32) * 4;
		      ++fixup_count;
		      value = 0;
		    }
		}
	    }

	  /* Insert the register or expression into the instruction.  */
	  errmsg = NULL;
	  insert_operand (cpu, opcode, operand, mods, insn_buf,
			  (offsetT) value, &errmsg);
	  if (errmsg != (const char *) NULL)
	    break;

	  ++syn;
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

/* Given a dvp cpu type, return it's STO_DVP value.
   The label prefix to use is stored in *PNAME.  */

static int
cpu_sto (cpu, pname)
     dvp_cpu cpu;
     const char **pname;
{
  switch (cpu)
    {
    case DVP_DMA : *pname = ".dma."; return STO_DVP_DMA;
    case DVP_VIF : *pname = ".vif."; return STO_DVP_VIF;
    case DVP_GIF : *pname = ".gif."; return STO_DVP_GIF;
    case DVP_VUUP : *pname = ".vu."; return STO_DVP_VU;
    }
  abort ();
}

/* Record the current mach type in the object file.
   If FORCE_NEXT_P is non-zero, force a label to be emitted the next time
   we're called.  This is useful for variable length instructions that can
   have labels embedded within them.  */

static void
record_mach (cpu, force_next_p)
     dvp_cpu cpu;
     int force_next_p;
{
  symbolS *label;
  const char *name;
  int sto;

  if (cpu == cur_cpu)
    return;

  sto = cpu_sto (cpu, &name);

  label = create_colon_label (sto, "", unique_name (name));

  if (force_next_p)
    cur_cpu = DVP_UNKNOWN;
  else
    cur_cpu = cpu;
}

/* Force emission of mach type label at next insn.
   This isn't static as TC_START_LABEL uses it.
   The result is the value of TC_START_LABEL.  */

int
force_mach_label ()
{
  cur_cpu = DVP_UNKNOWN;
  return 1;
}

/* Push/pop the current parsing state.  */

static void
push_asm_state (new_state)
     asm_state new_state;
{
  ++cur_state_index;
  if (cur_state_index == MAX_STATE_DEPTH)
    as_fatal ("internal error: unexpected state push");
  asm_state_stack[cur_state_index] = new_state;
}

/* TOP_OK_P is non-zero if it's ok that we're at the top of the stack.
   This happens if there are errors in the assembler code.
   We just reset the stack to its "init" state.  */

static void
pop_asm_state (top_ok_p)
     int top_ok_p;
{
  if (cur_state_index == 0)
    {
      if (top_ok_p)
	asm_state_stack[cur_state_index] = ASM_INIT;
      else
	as_fatal ("internal error: unexpected state pop");
    }
  else
    --cur_state_index;
}

static void
set_asm_state (state)
     asm_state state;
{
  CUR_ASM_STATE = state;
}

void 
md_operand (expressionP)
     expressionS *expressionP;
{
  /* Check if this is a '*' for mpgloc.  */
  if (cur_opcode
      && (cur_opcode->flags & VIF_OPCODE_MPG) != 0
      && (cur_operand->flags & DVP_OPERAND_VU_ADDRESS) != 0
      && *input_line_pointer == '*')
    {
      expressionP->X_op = O_symbol;
      expressionP->X_add_symbol = mpgloc_sym;
      expressionP->X_add_number = 0;

      /* Advance over the '*'.  */
      ++input_line_pointer;
    }
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
dvp_after_pass_hook ()
{
  /* If doing dma packing, ensure the last dma tag is filled out.  */
  if (dma_pack_vif_p)
    {
      /* Nothing to do as vifnops are zero and frag_align at beginning
	 of dmatag is all we need.  */
    }

#if 0 /* ??? Doesn't work unless we keep track of the nested include file
	 level.  */
  /* Check for missing .EndMpg, and supply one if necessary.  */
  if (CUR_ASM_STATE == ASM_MPG)
    s_endmpg (ENDMPG_INTERNAL);
  else if (CUR_ASM_STATE == ASM_DIRECT)
    s_enddirect (0);
  else if (CUR_ASM_STATE == ASM_UNPACK)
    s_endunpack (0);
#endif
}

/* Called via tc_frob_label when a label is defined.  */

void
dvp_frob_label (sym)
     symbolS *sym;
{
  const char * name = S_GET_NAME (sym);

  /* All labels in vu code must be specially marked for the disassembler.
     The disassembler ignores all previous information at each new label
     (that has a higher address than the last one).  */
  if (CUR_ASM_STATE == ASM_MPG
      || CUR_ASM_STATE == ASM_VU)
    S_SET_OTHER (sym, STO_DVP_VU);

  /* If inside an mpg, move vu space labels to their own section and create
     the corresponding ._. version in normal space.  */

  if (CUR_ASM_STATE == ASM_MPG
      /* Only do this special processing for user specified symbols.
	 Not sure how we can distinguish them other than by some prefix.  */
      && *name != '.'
      /* Check for recursive invocation creating the ._.name.  */
      && strncmp (name, VU_LABEL_PREFIX, sizeof (VU_LABEL_PREFIX) - 1) != 0)
    {
      /* Move this symbol to vu space.  */
      symbolS * cur_mpgloc = compute_mpgloc (mpgloc_sym, vif_data_start,
					     expr_build_dot ());
      S_SET_SEGMENT (sym, expr_section);
      sym->sy_value = cur_mpgloc->sy_value;
      sym->sy_frag = &zero_address_frag;

      /* Create the ._. symbol in normal space.  */
      create_colon_label (STO_DVP_VU, VU_LABEL_PREFIX, name);
    }
}

/* mpg/direct alignment is handled via relaxation */

/* Return an initial guess of the length by which a fragment must grow to
   hold a branch to reach its destination.
   Also updates fr_type/fr_subtype as necessary.

   Called just before doing relaxation.
   Any symbol that is now undefined will not become defined.
   The guess for fr_var is ACTUALLY the growth beyond fr_fix.
   Whatever we do to grow fr_fix or fr_var contributes to our returned value.
   Although it may not be explicit in the frag, pretend fr_var starts with a
   0 value.  */

int
md_estimate_size_before_relax (fragP, segment)
     fragS * fragP;
     segT segment;
{
  /* Our initial estimate is always 0.  */
  return 0;
} 

/* Perform the relaxation.
   All we have to do is figure out how many bytes we need to insert to
   get to the recorded symbol (which is at the required alignment).
   This function is also called for machine dependent vu insn frags.
   In this case the growth is always 0.  */

long
dvp_relax_frag (fragP, stretch)
     fragS * fragP;
     long stretch;
{
  /* Address of variable part.  */
  long address = fragP->fr_address + fragP->fr_fix;
  /* Symbol marking start of data.  */
  symbolS * symbolP = fragP->fr_symbol;
  /* Address of the symbol.  */
  long target;
  long growth;

  /* subtype >= 10 means "done" */
  if (RELAX_DONE_P (fragP->fr_subtype))
    return 0;

  /* vu insn? */
  if (fragP->fr_subtype == RELAX_VU)
    {
      fragP->fr_subtype = RELAX_ENCODE (RELAX_VU, 0);
      return 0;
    }

  target = S_GET_VALUE (symbolP) + symbolP->sy_frag->fr_address;

  if (fragP->fr_subtype == RELAX_MPG)
    {
      growth = target - address;
      if (growth < 0)
	as_fatal ("internal error: bad mpg alignment handling");
      fragP->fr_subtype = RELAX_ENCODE (RELAX_MPG, growth);
      return growth;
    }

  if (fragP->fr_subtype == RELAX_DIRECT)
    {
      growth = target - address;
      if (growth < 0)
	as_fatal ("internal error: bad direct alignment handling");
      fragP->fr_subtype = RELAX_ENCODE (RELAX_DIRECT, growth);
      return growth;
    }

  as_fatal ("internal error: unknown fr_subtype");
}

/* *fragP has been relaxed to its final size, and now needs to have
   the bytes inside it modified to conform to the new size.

   Called after relaxation is finished.
   fragP->fr_type == rs_machine_dependent.
   fragP->fr_subtype is the subtype of what the address relaxed to.  */

void
md_convert_frag (abfd, sec, fragP)
  bfd * abfd;
  segT sec;
  fragS * fragP;
{
  int growth = RELAX_GROWTH (fragP->fr_subtype);

  fragP->fr_fix += growth;

  if (growth != 0)
    {
      /* We had to grow this fragment.  Shift the mpg/direct insn to the end
	 (so it abuts the following data).  */
      DVP_INSN insn = bfd_getl32 (fragP->fr_opcode);
      md_number_to_chars (fragP->fr_opcode, VIFNOP, 4);
      if (growth > 4)
	md_number_to_chars (fragP->fr_opcode + 4, VIFNOP, 4);
      if (growth > 8)
	md_number_to_chars (fragP->fr_opcode + 8, VIFNOP, 4);
      md_number_to_chars (fragP->fr_literal + fragP->fr_fix - 4, insn, 4);

      /* Adjust fr_opcode so md_apply_fix3 works with the right bytes.  */
      fragP->fr_opcode += growth;
    }
}

/* Functions concerning relocs.  */

/* Spacing between each cpu type's operand numbers.
   Should be at least as big as any operand table.  */
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
    default : as_fatal ("internal error: bad fixup encoding");
    }
}

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
      /* If fx_tcbit is set this is for a vif insn and thus should never
	 happen in correct code.  */
      /* ??? The error message could be a bit more descriptive.  */
      if (fixP->fx_tcbit)
	as_bad ("unable to compute length of vif insn");
      /* The symbol is undefined (or is defined but not in this section).
	 Let the linker figure it out.  +8: branch offsets are relative to the
	 delay slot.  */
      return 8;
    }

  /* If fx_tcbit is set, this is a vif end-of-variable-length-insn marker.
     In this case the offset is relative to the start of data.
     Otherwise we assume this is a vu branch.  In this case
     offsets are calculated based on the address of the next insn.  */
  if (fixP->fx_tcbit)
    {
      /* As a further refinement, if fr_opcode is NULL this is `unpack'
	 which doesn't involve any relaxing.  */
      if (fixP->fx_frag->fr_opcode == NULL)
	return fixP->fx_frag->fr_address + fixP->fx_where + 4;
      else
	return fixP->fx_frag->fr_address + fixP->fx_frag->fr_fix;
    }
  else
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

  /* Check for dvp operands.  These are indicated with a reloc value
     >= BFD_RELOC_UNUSED.  */

  if ((int) fixP->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      dvp_cpu cpu;
      const dvp_operand *operand;
      DVP_INSN insn;
      fragS *fragP = fixP->fx_frag;

      /* If this was a relaxable insn, the opcode may have moved.  Find it.  */
      if (fragP->fr_opcode != NULL)
	where = fragP->fr_opcode;

      decode_fixup_reloc_type ((int) fixP->fx_r_type,
			       & cpu, & operand);

      /* For variable length vif insn data lengths, validate the user specified
	 value or install the computed value in the instruction.  */
      if (cpu == DVP_VIF
	  && (operand - vif_operands) == vif_operand_datalen_special)
	{
	  int insn_type = vif_insn_type (where[3]);
	  value = vif_length_value (where[3],
				    fixP->tc_fix_data.wl, fixP->tc_fix_data.cl,
				    value);
	  if (fixP->tc_fix_data.user_value != -1)
	    {
	      /* We can't do this for unpack insns with wl > cl.  */
	      if ((insn_type != VIF_OPCODE_UNPACK
		   || (fixP->tc_fix_data.wl <= fixP->tc_fix_data.cl))
		  && fixP->tc_fix_data.user_value != value)
		as_warn_where (fixP->fx_file, fixP->fx_line,
			       "specified length value doesn't match computed value");
	      /* Don't override the user specified value.  */
	    }
	  else
	    {
	      if (output_vif)
		{
		  install_vif_length (where, value);
		}
	    }
	  fixP->fx_done = 1;
	  return 1;
	}

      /* For the gif nloop operand, if it was specified by the user ensure
	 it matches the value we computed.  */
      if (cpu == DVP_GIF
	  && (operand - gif_operands) == gif_operand_nloop)
	{
	  value = compute_nloop (fixP->tc_fix_data.type,
				 fixP->tc_fix_data.nregs,
				 value);
	  if (fixP->tc_fix_data.user_value != -1)
	    {
	      check_nloop (fixP->tc_fix_data.type,
			   fixP->tc_fix_data.nregs,
			   fixP->tc_fix_data.user_value,
			   value,
			   fixP->fx_file, fixP->fx_line);
	      /* Don't override the user specified value.  */
	      fixP->fx_done = 1;
	      return 1;
	    }
	}

      /* Fetch the instruction, insert the fully resolved operand
	 value, and stuff the instruction back again.  The fixup is recorded
	 at the appropriate word so pass DVP_MOD_THIS_WORD so any offset
	 specified in the tables is ignored.  */
      insn = bfd_getl32 ((unsigned char *) where);
      insert_operand_final (cpu, operand, DVP_MOD_THIS_WORD, &insn,
			    (offsetT) value, fixP->fx_file, fixP->fx_line);
      bfd_putl32 ((bfd_vma) insn, (unsigned char *) where);

      /* If this is mpgloc/unpackloc, we're done.  */
      if (operand->flags & (DVP_OPERAND_VU_ADDRESS | DVP_OPERAND_UNPACK_ADDRESS))
	fixP->fx_done = 1;

      if (fixP->fx_done)
	{
	  /* Nothing else to do here.  */
	  return 1;
	}

      /* Determine a BFD reloc value based on the operand information.
	 We are only prepared to turn a few of the operands into relocs.  */
      if ((operand->flags & DVP_OPERAND_RELATIVE_BRANCH) != 0)
	{
	  assert (operand->bits == 11
		  && operand->shift == 0);

	  /* The fixup isn't recorded as a pc relative branch to some label.
	     Instead a complicated expression is used to compute the desired
	     value.  Well, that didn't work and we have to emit a reloc.
	     Things are tricky because the result we want is the difference
	     of two addresses in a section potentially different from the one
	     the reloc is in.  Ugh.
	     The solution is to emit two relocs, one that adds the target
	     address and one that subtracts the source address + 8 (the
	     linker will perform the byte->dword conversion).
	     This is rather complicated and rather than risk breaking
	     existing code we fall back on the old way if the file only
	     contains vu code.  In this case the file is intended to
	     be fully linked with other vu code and thus we have a normal
	     situation where the relocation directly corresponds to the
	     branch insn.  */

	  if (non_vu_insn_seen_p)
	    {
	      as_bad_where (fixP->fx_file, fixP->fx_line,
			    "can't handle mpg loaded vu code with branch relocations");
	      fixP->fx_done = 1;
	      return 1;
	    }
	  else
	    {
	      fixP->fx_r_type = BFD_RELOC_MIPS_DVP_11_PCREL;
	    }
	}
      else if ((operand->flags & DVP_OPERAND_DMA_ADDR) != 0
	       || (operand->flags & DVP_OPERAND_DMA_NEXT) != 0)
	{
	  assert (operand->bits == 27
		  && operand->shift == 4);
	  fixP->fx_r_type = BFD_RELOC_MIPS_DVP_27_S4;
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
	  as_fatal ("internal error: unexpected fixup");
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

/* Miscellaneous utilities.  */

/* Parse a 32 bit floating point number.
   The result is those 32 bits as an integer.  */

static long
parse_float (pstr, errmsg)
     char **pstr;
     const char **errmsg;
{
  LITTLENUM_TYPE words[MAX_LITTLENUMS];
  char *p;

  p = atof_ieee (*pstr, 'f', words);
  *pstr = p;
  return (words[0] << 16) | words[1];
}

/* Scan a symbol and return a pointer to one past the end.  */

#define issymchar(ch) (isalnum(ch) || ch == '_')
static char *
scan_symbol (sym)
    char *sym;
{
  while (*sym && issymchar (*sym))
    ++sym;
  return sym;
}

/* Evaluate an expression for an operand.
   The result is the value of the expression if it can be evaluated,
   or 0 if it cannot (say because some symbols haven't been defined yet)
   in which case a fixup is queued.

   If OPINDEX is 0, don't queue any fixups, just return 0.  */

static long
#ifdef USE_STDARG
eval_expr (dvp_cpu cpu, int opindex, int offset, const char *fmt, ...)
#else
eval_expr (cpu, opindex, offset, fmt, va_alist)
     dvp_cpu cpu;
     int opindex,offset;
     const char *fmt;
     va_dcl
#endif
{
  long value;
  va_list ap;
  char *str,*save_input;
  expressionS exp;

#ifdef USE_STDARG
  va_start (ap, fmt);
#else
  va_start (ap);
#endif
  vasprintf (&str, fmt, ap);
  va_end (ap);

  save_input = input_line_pointer;
  input_line_pointer = str;
  expression (&exp);
  input_line_pointer = save_input;
  free (str);
  if (exp.X_op == O_constant)
    value = exp.X_add_number;
  else
    {
      if (opindex != 0)
	{
	  fixups[fixup_count].cpu = cpu;
	  fixups[fixup_count].exp = exp;
	  fixups[fixup_count].opindex = opindex;
	  fixups[fixup_count].offset = offset;
	  fixups[fixup_count].user_value = -1;
	  fixups[fixup_count].wl = -1;
	  fixups[fixup_count].cl = -1;
	  ++fixup_count;
	}
      value = 0;
    }
  return value;
}

/* Create a label named by concatenating PREFIX to NAME.  */

static symbolS *
create_label (prefix, name)
     const char *prefix, *name;
{
  int namelen = strlen (name);
  int prefixlen = strlen (prefix);
  char *fullname;
  symbolS *result;

  fullname = xmalloc (prefixlen + namelen + 1);
  strcpy (fullname, prefix);
  strcat (fullname, name);
  result = symbol_find_or_make (fullname);
  free (fullname);
  return result;
}

/* Create a label named by concatenating PREFIX to NAME,
   and define it as `.'.
   STO, if non-zero, is the st_other value to assign to this label.
   If STO is zero `cur_cpu', call force_mach_label to force record_mach to
   emit a cpu label.  Otherwise the disassembler gets confused.  */

static symbolS *
create_colon_label (sto, prefix, name)
     int sto;
     const char *prefix, *name;
{
  int namelen = strlen (name);
  int prefixlen = strlen (prefix);
  char *fullname;
  symbolS *result;

  fullname = xmalloc (prefixlen + namelen + 1);
  strcpy (fullname, prefix);
  strcat (fullname, name);
  result = colon (fullname);
  if (sto)
    S_SET_OTHER (result, sto);
  else
    force_mach_label ();
  free (fullname);
  return result;
}

/* Return a malloc'd string useful in creating unique labels.
   PREFIX is the prefix to use or NULL if we're to pick one.  */

static char *
unique_name (prefix)
     const char *prefix;
{
  static int counter;
  char *result;

  if (prefix == NULL)
    prefix = UNIQUE_LABEL_PREFIX;
  asprintf (&result, "%s%d", prefix, counter);
  ++counter;
  return result;
}

/* Compute a value for $.mpgloc given a symbol at the start of a chunk
   of code, the $.mpgloc value for the start, and a symbol at the end
   of the chunk of code.  */

static symbolS *
compute_mpgloc (startloc, startsym, endsym)
     symbolS * startloc;
     symbolS * startsym;
     symbolS * endsym;
{
  symbolS *s;

  s = expr_build_binary (O_subtract, endsym, startsym);
  s = expr_build_binary (O_add, startloc, s);
  return s;
}

/* Compute a value for nloop.  */

static int
compute_nloop (type, nregs, bytes)
     gif_type type;
     int nregs, bytes;
{
  int computed_nloop;

  switch (type)
    {
    case GIF_PACKED :
      /* We can't compute a value if no regs were specified and there is a
	 non-zero amount of data.  Just set to something useful, a warning
	 will be issued later.  */
      if (nregs == 0)
	nregs = 1;
      computed_nloop = (bytes >> 4) / nregs;
      break;
    case GIF_REGLIST :
      if (nregs == 0)
	nregs = 1;
      computed_nloop = (bytes >> 3) / nregs;
      break;
    case GIF_IMAGE :
      computed_nloop = bytes >> 4;
      break;
    }

  return computed_nloop;
}

/* Issue a warning if the user specified nloop value doesn't match the
   computed value.  */

static void
check_nloop (type, nregs, user_nloop, computed_nloop, file, line)
     gif_type type;
     int nregs,user_nloop,computed_nloop;
     char *file;
     unsigned int line;
{
  if (user_nloop != computed_nloop)
    as_warn_where (file, line, "nloop value does not match amount of data");
}

/* Compute the auto-count value for a DMA tag.
   INLINE_P is non-zero if the dma data is inline.  */

static void
setup_dma_autocount (name, insn_buf, inline_p)
     const char *name;
     DVP_INSN *insn_buf;
     int inline_p;
{
  long count;

  if (inline_p)
    {
      /* -1: The count is the number of following quadwords, so skip the one
	 containing the dma tag.  */
      count = eval_expr (DVP_DMA, dma_operand_count, 0,
			 "((%s%s - %s) >> 4) - 1", END_LABEL_PREFIX, name, name);
    }
  else
    {
      /* We don't want to subtract 1 here as the begin and end labels
	 properly surround the data we want to compute the length of.  */
      count = eval_expr (DVP_DMA, dma_operand_count, 0,
			 "(%s%s - %s) >> 4", END_LABEL_PREFIX, name, name);
    }

  /* Store the count field. */
  insn_buf[0] &= 0xffff0000;
  insn_buf[0] |= count & 0x0000ffff;
}

/* Record that inline data follows.  */

static void
inline_dma_data (autocount_p, insn_buf)
    int autocount_p;
    DVP_INSN *insn_buf;
{
  if (dma_data_state != 0 )
    {
      as_bad ("DmaData blocks cannot be nested.");
      return;
    }

  dma_data_state = 1;

  if (autocount_p)
    {
      dma_data_name = S_GET_NAME (create_colon_label (0, "", unique_name (NULL)));
      setup_dma_autocount (dma_data_name, insn_buf, 1);
    }
  else
    dma_data_name = 0;
}

/* Compute the auto-count value for a DMA tag with out-of-line data.  */

static long
parse_dma_addr_autocount (opcode, operand, mods, insn_buf, pstr, errmsg)
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
  /* Data reference must be a .DmaData label.  */
  symbolS *label, *label2, *endlabel;
  const char *name;
  char c;

  label = label2 = 0;
  if (! is_name_beginner (*start))
    {
      *errmsg = "invalid .DmaData label";
      return 0;
    }

  name = start;
  end = scan_symbol (name);
  c = *end;
  *end = 0;
  label = symbol_find_or_make (name);
  *end = c;

  label2 = create_label ("_$", name);
  endlabel = create_label (END_LABEL_PREFIX, name);

  retval = eval_expr (DVP_DMA, dma_operand_addr, 4, name);

  setup_dma_autocount (name, insn_buf, 0);

  *pstr = end;
  return retval;
}

/* Compute the type of vif insn of IBYTE.
   IBYTE is the msb of the insn.
   This is only used for mpg,direct,unpack insns.
   The result is one of VIF_OPCODE_{DIRECT,DIRECTHL,MPG,UNPACK}.  */

static int
vif_insn_type (ibyte)
     char ibyte;
{
  switch (ibyte & 0x70)
    {
    case 0x50 :
      return (ibyte & 1) ? VIF_OPCODE_DIRECTHL : VIF_OPCODE_DIRECT;
    case 0x40 :
      return VIF_OPCODE_MPG;
    case 0x60 :
    case 0x70 :
      return VIF_OPCODE_UNPACK;
    default :
      as_fatal ("internal error: bad call to vif_insn_type");
    }
}

/* Return the length value to insert in a VIF instruction whose upper
   byte is IBYTE and whose data length is BYTES.
   WL,CL are used for unpack insns and are the stcycl values in effect.
   This does not do the max -> 0 conversion.  */

static int
vif_length_value (ibyte, wl, cl, bytes)
     char ibyte;
     int wl,cl;
     int bytes;
{
  switch (ibyte & 0x70)
    {
    case 0x50 : /* direct */
      /* ??? Worry about data /= 16 cuts off?  */
      return bytes / 16;
    case 0x40 : /* mpg */
      /* ??? Worry about data /= 8 cuts off?  */
      return bytes / 8;
    case 0x60 : /* unpack */
    case 0x70 :
      return vif_unpack_len_value (ibyte & 15, wl, cl, bytes);
    default :
      as_fatal ("internal error: bad call to vif_length_value");
    }
}

/* Install length LEN in the vif insn at BUF.
   LEN is the actual value to store, except that the max->0 conversion
   hasn't been done (we do it).
   The bytes in BUF are in target order.  */

static void
install_vif_length (buf, len)
     char *buf;
     int len;
{
  unsigned char ibyte = buf[3];

  if ((ibyte & 0x70) == 0x40)
    {
      /* mpg */
      if (len > 256)
	as_bad ("`mpg' data length must be between 1 and 256");
      buf[2] = len == 256 ? 0 : len;
    }
  else if ((ibyte & 0x70) == 0x50)
    {
      /* direct/directhl */
      if (len > 65536)
	as_bad ("`direct' data length must be between 1 and 65536");
      len = len == 65536 ? 0 : len;
      buf[0] = len;
      buf[1] = len >> 8;
    }
  else if ((ibyte & 0x60) == 0x60)
    {
      /* unpack */
      /* len == -1 means wl,cl are unknown and thus we can't compute
	 a useful value */
      if (len == -1)
	{
	  as_bad ("missing `stcycle', can't compute length of `unpack' insn");
	  len = 1;
	}
      if (len < 0 || len > 256)
	as_bad ("`unpack' data length must be between 0 and 256");
      /* 256 is recorded as 0 in the insn */
      len = len == 256 ? 0 : len;
      buf[2] = len;
    }
  else
    as_fatal ("internal error: bad call to install_vif_length");
}

/* Finish off the current set of mpg insns, and start a new set.
   The IGNORE arg exists because insert_unpack_marker uses it and both
   of these functions are passed to insert_file.  */

static void
insert_mpg_marker (ignore)
     unsigned long ignore;
{
  s_endmpg (ENDMPG_MIDDLE);
  /* mpgloc is updated by s_endmpg.  */
  md_assemble ("mpg *,*");
  /* Record the cpu type in case we're in the middle of reading binary
     data.  */
  record_mach (DVP_VUUP, 0);
}

/* Finish off the current unpack insn and start a new one.
   INSN0 is the first word of the insn and is used to figure out what
   kind of unpack insn it is.  */

static void
insert_unpack_marker (insn0)
     unsigned long insn0;
{
}

/* Insert a file into the output.
   The -I arg passed to GAS is used to specify where to find the file.
   INSERT_MARKER if non-NULL is called every SIZE bytes with an argument of
   INSERT_MARKER_ARG.  This is used by the mpg insn to insert mpg's every 256
   insns and by the unpack insn.
   The result is the number of bytes inserted.
   If an error occurs an error message is printed and zero is returned.  */

static int
insert_file (file, insert_marker, insert_marker_arg, size)
     const char *file;
     void (*insert_marker) PARAMS ((unsigned long));
     unsigned long insert_marker_arg;
     int size;
{
  FILE *f;
  char buf[256];
  int i, n, total, left_before_marker;
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
  left_before_marker = 0;
  do {
    int bytes;
    if (insert_marker)
      bytes = MIN (size - left_before_marker, sizeof (buf));
    else
      bytes = sizeof (buf);
    n = fread (buf, 1, bytes, f);
    if (n > 0)
      {
	char *fr = frag_more (n);
	memcpy (fr, buf, n);
	total += n;
	if (insert_marker)
	  {
	    left_before_marker += n;
	    if (left_before_marker > size)
	      as_fatal ("internal error: file insertion sanity checky failed");
	    if (left_before_marker == size)
	      {
		(*insert_marker) (insert_marker_arg);
		left_before_marker = 0;
	      }
	  }
      }
  } while (n > 0);

  fclose (f);
  /* We assume the file is smaller than 2^31 bytes.
     Ok, we shouldn't make any assumptions.  */
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
      /* We currently assume a field does not cross a word boundary.  */
      int shift = ((mods & DVP_MOD_THIS_WORD)
		   ? (operand->shift & 31)
		   : operand->shift);
      /* FIXME: revisit */
      if (operand->word == 0)
	{
	  int word = (mods & DVP_MOD_THIS_WORD) ? 0 : (shift / 32);
	  if (operand->bits == 32)
	    insn_buf[word] = val;
	  else
	    {
	      shift = shift % 32;
	      insn_buf[word] |= ((long) val & ((1 << operand->bits) - 1)) << shift;
	    }
	}
      else
	{
	  int word = (mods & DVP_MOD_THIS_WORD) ? 0 : operand->word;
	  if (operand->bits == 32)
	    insn_buf[word] = val;
	  else
	    {
	      long temp = (long) val & ((1 << operand->bits) - 1);
	      insn_buf[word] |= temp << operand->shift;
	    }
	}
    }
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

      /* ??? This test belongs more properly in the insert handler.  */
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
      /* ??? This test belongs more properly in the insert handler.  */
      else if ((operand->flags & DVP_OPERAND_VU_ADDRESS) != 0)
	{
	  if ((val & 7) != 0)
	    {
	      if (file == (char *) NULL)
		as_warn ("misaligned vu address");
	      else
		as_warn_where (file, line, "misaligned vu address");
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

/* DVP pseudo ops.  */

static void
s_dmadata (ignore)
    int ignore;
{
  char *name, c;

  dma_data_name = 0;

  if (dma_data_state != 0)
    {
      as_bad ("DmaData blocks cannot be nested.");
      ignore_rest_of_line ();
      return;
    }
  dma_data_state = 1;

  SKIP_WHITESPACE ();		/* Leading whitespace is part of operand. */
  name = input_line_pointer;

  if (!is_name_beginner (*name))
    {
      as_bad ("invalid identifier for \".DmaData\"");
      ignore_rest_of_line ();
      return;
    }

  /* Do an implicit alignment to a 16 byte boundary. */
  frag_align (4, 0, 0);
  record_alignment (now_seg, 4);

  c = get_symbol_end ();
  line_label = colon (name);	/* user-defined label */
  dma_data_name = S_GET_NAME (line_label);
  *input_line_pointer = c;

  /* Force emission of a machine type label for the next insn.  */
  force_mach_label ();

  demand_empty_rest_of_line ();
}

static void
s_enddmadata (ignore)
    int ignore;
{
  if (dma_data_state != 1)
    {
      as_warn (".EndDmaData encountered outside a DmaData block -- ignored.");
      ignore_rest_of_line ();
      dma_data_name = 0;
    }
  dma_data_state = 0;
  demand_empty_rest_of_line ();

  /* If count provided, verify it is correct.  */
  /* ... */

  /* Fill the data out to a multiple of 16 bytes.  */
  /* FIXME: Are the fill contents right?  */
  frag_align (4, 0, 0);

  /* "label" points to beginning of block.
     Create a name for the final label like _$<name>.  */
  if (dma_data_name)
    create_colon_label (0, END_LABEL_PREFIX, dma_data_name);
}

static void
s_dmapackvif (ignore)
    int ignore;
{
  /* Syntax: .dmapackvif 0|1 */

  /* Leading whitespace is part of operand. */
  SKIP_WHITESPACE ();
  switch (*input_line_pointer++)
    {
    case '0':
      dma_pack_vif_p = 0;
      break;
    case '1':
      dma_pack_vif_p = 1;
      break;
    default:
      as_bad ("illegal argument to `.dmapackvif'");
    }
  demand_empty_rest_of_line ();
}

/* INTERNAL_P is non-zero if invoked internally by this file rather than
   by the user.  In this case we don't touch the input stream.  */

static void
s_enddirect (internal_p)
     int internal_p;
{
  if (CUR_ASM_STATE != ASM_DIRECT)
    {
      as_bad ("`.enddirect' has no matching `direct' instruction");
      return;
    }

  /* Record in the end data symbol the current location.  */
  if (now_seg != S_GET_SEGMENT (vif_data_end))
    as_bad (".enddirect in different section");
  vif_data_end->sy_frag = frag_now;
  S_SET_VALUE (vif_data_end, (valueT) frag_now_fix ());

  set_asm_state (ASM_INIT);

  /* Needn't be reset, but to catch bugs it is.  */
  vif_data_end = NULL;

  if (! internal_p)
    demand_empty_rest_of_line ();
}

/* CALLER denotes who's calling us.
   If ENDMPG_USER then .endmpg was found in the input stream.
   If ENDMPG_INTERNAL then we've been invoked to finish off file insertion.
   If ENDMPG_MIDDLE then we've been invoked in the middle of a long stretch
   of vu code.  */

static void
s_endmpg (caller)
     int caller;
{
  if (CUR_ASM_STATE != ASM_MPG)
    {
      as_bad ("`.endmpg' has no matching `mpg' instruction");
      return;
    }

  /* Record in the end data symbol the current location.  */
  if (now_seg != S_GET_SEGMENT (vif_data_end))
    as_bad (".endmpg in different section");
  vif_data_end->sy_frag = frag_now;
  S_SET_VALUE (vif_data_end, (valueT) frag_now_fix ());

  /* Update $.mpgloc.
     We have to leave the old value alone as it may be used in fixups
     already recorded.  Since compute_mpgloc allocates a new symbol for the
     result we're ok.  The new value is the old value plus the number of
     double words in this chunk.  */
  mpgloc_sym = compute_mpgloc (mpgloc_sym, vif_data_start, vif_data_end);

  set_asm_state (ASM_INIT);

  /* Needn't be reset, but to catch bugs it is.  */
  vif_data_end = NULL;

  /* Reset the vu insn counter.  */
  if (caller != ENDMPG_MIDDLE)
    vu_count = -1;

  if (caller == ENDMPG_USER)
    demand_empty_rest_of_line ();
}

/* INTERNAL_P is non-zero if invoked internally by this file rather than
   by the user.  In this case we don't touch the input stream.  */

static void
s_endunpack (internal_p)
     int internal_p;
{
  if (CUR_ASM_STATE != ASM_UNPACK)
    {
      as_bad ("`.endunpack' has no matching `unpack' instruction");
      return;
    }

  /* Record in the end data symbol the current location.  */
  /* ??? $.unpackloc is gone.  Is this also used for data length
     verification?  */
  if (now_seg != S_GET_SEGMENT (vif_data_end))
    as_bad (".endunpack in different section");
  vif_data_end->sy_frag = frag_now;
  S_SET_VALUE (vif_data_end, (valueT) frag_now_fix ());

  /* Round up to next word boundary.  */
  frag_align (2, 0, 0);

  set_asm_state (ASM_INIT);

  /* Needn't be reset, but to catch bugs it is.  */
  vif_data_end = NULL;

  if (! internal_p)
    demand_empty_rest_of_line ();
}

static void
s_endgif (ignore)
     int ignore;
{
  int bytes;
  int specified_nloop = gif_nloop ();
  int computed_nloop;
  int nregs = gif_nregs ();
  char *file;
  unsigned int line;

  as_where (&file, &line);

  if (CUR_ASM_STATE != ASM_GIF)
    {
      as_bad (".endgif doesn't follow a gif tag");
      return;
    }
  pop_asm_state (0);

  /* Fill out to proper boundary.
     ??? This may cause eval_expr to always queue a fixup.  So be it.  */
  switch (gif_insn_type)
    {
    case GIF_PACKED :  frag_align (4, 0, 0); break;
    case GIF_REGLIST : frag_align (3, 0, 0); break;
    case GIF_IMAGE :   frag_align (4, 0, 0); break;
    }

  /* The -16 is because the `gif_data_name' label is emitted at the
     start of the gif tag.  If we're in a different frag from the one we
     started with, this can't be computed until much later.  To cope we queue
     a fixup and deal with it then.
     ??? The other way to handle this is by having expr() compute "syma - symb"
     when they're in different fragments but the difference is constant.
     Not sure how much of a slowdown that will introduce though.  */
  fixup_count = 0;
  bytes = eval_expr (DVP_GIF, gif_operand_nloop, 0, ". - %s - 16", gif_data_name);

  /* Compute a value for nloop if we can.  */

  if (fixup_count == 0)
    {
      computed_nloop = compute_nloop (gif_insn_type, nregs, bytes);

      /* If the user specified nloop, verify it.  */
      if (specified_nloop != -1)
	check_nloop (gif_insn_type, nregs,
		     specified_nloop, computed_nloop,
		     file, line);
    }

  /* If computation of nloop can't be done yet, queue a fixup and do it later.
     Otherwise validate nloop if specified or write the computed value into
     the insn.  */

  if (fixup_count != 0)
    {
      /* FIXME: It might eventually be possible to combine all the various
	 copies of this bit of code.  */
      int op_type, reloc_type, offset;
      const dvp_operand *operand;
      fixS *fix;

      op_type = fixups[0].opindex;
      offset = fixups[0].offset;
      reloc_type = encode_fixup_reloc_type (DVP_GIF, op_type);
      operand = &gif_operands[op_type];
      fix = fix_new_exp (gif_insn_frag,
			 (gif_insn_frag_loc + offset
			  - gif_insn_frag->fr_literal),
			 4, &fixups[0].exp, 0,
			 (bfd_reloc_code_real_type) reloc_type);
      /* Record user specified value so we can test it when we compute the
	 actual value.  */
      fix->tc_fix_data.type = gif_insn_type;
      fix->tc_fix_data.nregs = nregs;
      fix->tc_fix_data.user_value = specified_nloop;
    }
  else if (specified_nloop != -1)
    ; /* nothing to do */
  else
    {
      DVP_INSN insn = bfd_getl32 (gif_insn_frag_loc);
      insert_operand_final (DVP_GIF, &gif_operands[gif_operand_nloop],
			    DVP_MOD_THIS_WORD, &insn,
			    (offsetT) computed_nloop, file, line);
      bfd_putl32 ((bfd_vma) insn, gif_insn_frag_loc);
    }

  /* These needn't be reset, but to catch bugs they are.  */
  gif_data_name = NULL;
  gif_insn_frag = NULL;
  gif_insn_frag_loc = NULL;

  demand_empty_rest_of_line ();
}

static void
s_state (state)
     int state;
{
  /* If in MPG state and the user requests to change to VU state,
     leave the state as MPG.  This happens when we see an mpg followed
     by a .include that has .vu.  Note that no attempt is made to support
     an include depth > 1 for this case.  */
  if (CUR_ASM_STATE == ASM_MPG && state == ASM_VU)
    return;

  /* If changing to the VU state, we need to set up things for $.mpgloc
     calculations.  */
  if (state == ASM_VU)
    {
      /* FIXME: May need to check that we're not clobbering currently
	 in use versions of these.  Also need to worry about which section
	 the .vu is issued in.  On the other hand, ".vu" isn't intended
	 to be supported everywhere.  */
      mpgloc_sym = expr_build_uconstant (0);
      vif_data_start = expr_build_dot ();
    }

  set_asm_state (state);

  demand_empty_rest_of_line ();
}
