/* tc-mips.c -- assemble code for a MIPS chip.
   Copyright (C) 1993 Free Software Foundation, Inc.
   Contributed by the OSF and Ralph Campbell.
   Written by Keith Knowles and Ralph Campbell, working independently.
   Modified for ECOFF support by Ian Lance Taylor of Cygnus Support.

   This file is part of GAS.

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

#include "as.h"

#include <ctype.h>

#ifndef __STDC__
#ifndef NO_STDARG
#define NO_STDARG
#endif
#endif

#ifndef NO_STDARG
#include <stdarg.h>
#else
#ifndef NO_VARARGS
#include <varargs.h>
#endif /* NO_VARARGS */
#endif /* NO_STDARG */

#include "opcode/mips.h"

#define AT  1
#define GP  28
#define RA  31

static int mips_warn_about_macros;
static int mips_noreorder;
static int mips_nomove;
static int mips_noat;
static int mips_nobopt;

#ifdef OBJ_ECOFF
/* The size of the small data section.  */
static int g_switch_value = 8;
#endif

#define N_RMASK 0xc4
#define N_VFP   0xd4

/* handle of the OPCODE hash table */
static struct hash_control *op_hash = NULL;

/* This array holds the chars that always start a comment.  If the
    pre-processor is disabled, these aren't very useful */
const char comment_chars[] = "#";

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output. */
/* Also note that C style comments are always supported.  */
const char line_comment_chars[] = "#";

/* This array holds machine specific line separator characters. */
const char line_separator_chars[] = "";

/* Chars that can be used to separate mant from exp in floating point nums */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

/* Also be aware that MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT may have to be
   changed in read.c .  Ideally it shouldn't have to know about it at all,
   but nothing is ideal around here.
 */

static char *insn_error;

static int byte_order = BYTE_ORDER;

static int auto_align = 1;

/* Symbol labelling the current insn.  */
static symbolS *insn_label;

/* To output NOP instructions correctly, we need to keep information
   about the previous two instructions.  */

/* Whether we are optimizing.  The default value of 2 means to remove
   unneeded NOPs and swap branch instructions when possible.  A value
   of 1 means to not swap branches.  A value of 0 means to always
   insert NOPs.  */
static int mips_optimize = 2;

/* The previous instruction.  */
static struct mips_cl_insn prev_insn;

/* The instruction before prev_insn.  */
static struct mips_cl_insn prev_prev_insn;

/* If we don't want information for prev_insn or prev_prev_insn, we
   point the insn_mo field at this dummy integer.  */
static const struct mips_opcode dummy_opcode = { 0 };

/* Non-zero if prev_insn is valid.  */
static int prev_insn_valid;

/* The frag for the previous instruction.  */
static struct frag *prev_insn_frag;

/* The offset into prev_insn_frag for the previous instruction.  */
static long prev_insn_where;

/* The reloc for the previous instruction, if any.  */
static fixS *prev_insn_fixp;

/* Non-zero if the previous instruction was in a delay slot.  */
static int prev_insn_is_delay_slot;

/* Non-zero if the previous instruction was in a .set noreorder.  */
static int prev_insn_unreordered;

/* Non-zero if the previous previous instruction was in a .set
   noreorder.  */
static int prev_prev_insn_unreordered;

/* Prototypes for static functions.  */

#ifdef __STDC__
#define internalError() \
    as_fatal ("internal Error, line %d, %s", __LINE__, __FILE__)
#else
#define internalError() as_fatal ("MIPS internal Error");
#endif

static int insn_uses_reg PARAMS ((struct mips_cl_insn *ip,
				  int reg, int fpr));
static void append_insn PARAMS ((struct mips_cl_insn * ip,
				 expressionS * p,
				 bfd_reloc_code_real_type r));
static void mips_no_prev_insn PARAMS ((void));
static void mips_emit_delays PARAMS ((void));
static int gp_reference PARAMS ((expressionS * ep));
static void macro_build PARAMS ((int *counter, expressionS * ep,
				 const char *name, const char *fmt,
				 ...));
static void macro_build_lui PARAMS ((int *counter, expressionS * ep,
				     int regnum));
static void set_at PARAMS ((int *counter, int reg));
static void set_at_unsigned PARAMS ((int *counter, int reg));
static void check_absolute_expr PARAMS ((struct mips_cl_insn * ip,
					 expressionS *));
static void load_register PARAMS ((int *counter,
				   struct mips_cl_insn * ip,
				   int reg, expressionS * ep));
static void macro PARAMS ((struct mips_cl_insn * ip));
static void mips_ip PARAMS ((char *str, struct mips_cl_insn * ip));
static int my_getSmallExpression PARAMS ((expressionS * ep, char *str));
static void my_getExpression PARAMS ((expressionS * ep, char *str));
static symbolS *get_symbol PARAMS ((void));
static void mips_align PARAMS ((int to, int fill));
static void s_align PARAMS ((int));
static void s_stringer PARAMS ((int));
static void s_change_sec PARAMS ((int));
static void s_cons PARAMS ((int));
static void s_err PARAMS ((int));
static void s_extern PARAMS ((int));
static void s_float_cons PARAMS ((int));
static void s_option PARAMS ((int));
static void s_mipsset PARAMS ((int));
static void s_mips_space PARAMS ((int));
#ifndef OBJ_ECOFF
static void md_obj_begin PARAMS ((void));
static void md_obj_end PARAMS ((void));
static long get_number PARAMS ((void));
static void s_ent PARAMS ((int));
static void s_mipsend PARAMS ((int));
static void s_file PARAMS ((int));
static void s_frame PARAMS ((int));
static void s_loc PARAMS ((int));
static void s_mask PARAMS ((char));
#endif

/* Pseudo-op table.

   The following pseudo-ops from the Kane and Heinrich MIPS book
   should be defined here, but are currently unsupported: .alias,
   .galive, .gjaldef, .gjrlive, .livereg, .noalias.

   The following pseudo-ops from the Kane and Heinrich MIPS book are
   specific to the type of debugging information being generated, and
   should be defined by the object format: .aent, .begin, .bend,
   .bgnb, .end, .endb, .ent, .fmask, .frame, .loc, .mask, .verstamp,
   .vreg.

   The following pseudo-ops from the Kane and Heinrich MIPS book are
   not MIPS CPU specific, but are also not specific to the object file
   format.  This file is probably the best place to define them, but
   they are not currently supported: .asm0, .endr, .lab, .repeat,
   .struct, .weakext.  */

const pseudo_typeS md_pseudo_table[] =
{
 /* MIPS specific pseudo-ops.  */
  {"option", s_option, 0},
  {"set", s_mipsset, 0},
  {"rdata", s_change_sec, 'r',},
  {"sdata", s_change_sec, 's',},

 /* Relatively generic pseudo-ops that happen to be used on MIPS
     chips.  */
  {"asciiz", s_stringer, 1},
  {"bss", s_change_sec, 'b'},
  {"err", s_err, 0},
  {"half", s_cons, 1},

 /* These pseudo-ops are defined in read.c, but must be overridden
     here for one reason or another.  */
  {"align", s_align, 0},
  {"ascii", s_stringer, 0},
  {"asciz", s_stringer, 1},
  {"byte", s_cons, 0},
  {"data", s_change_sec, 'd'},
  {"double", s_float_cons, 'd'},
  {"extern", s_extern, 0},
  {"float", s_float_cons, 'f'},
  {"space", s_mips_space, 0},
  {"text", s_change_sec, 't'},
  {"word", s_cons, 2},

#ifndef OBJ_ECOFF
 /* These pseudo-ops should be defined by the object file format.
     However, ECOFF is the only format which currently defines them,
     so we have versions here for a.out.  */
  {"aent", s_ent, 1},
  {"end", s_mipsend, 0},
  {"ent", s_ent, 0},
  {"file", s_file, 0},
  {"fmask", s_ignore, 'F'},
  {"frame", s_ignore, 0},
  {"loc", s_ignore, 0},
  {"mask", s_ignore, 'R'},
  {"verstamp", s_ignore, 0},
#endif

 /* Sentinel.  */
  {NULL}
};

const relax_typeS md_relax_table[] =
{
  { 0 }
};


static char *expr_end;

static expressionS imm_expr;
static expressionS offset_expr;
static bfd_reloc_code_real_type imm_reloc;
static bfd_reloc_code_real_type offset_reloc;

/*
 * This function is called once, at assembler startup time.  It should
 * set up all the tables, etc. that the MD part of the assembler will need.
 */
void
md_begin ()
{
  register char *retval = NULL;
  register unsigned int i = 0;

  if ((op_hash = hash_new ()) == NULL)
    {
      as_fatal ("Virtual memory exhausted");
    }
  for (i = 0; i < NUMOPCODES;)
    {
      const char *name = mips_opcodes[i].name;

      retval = hash_insert (op_hash, name, &mips_opcodes[i]);
      if (retval != NULL && *retval != '\0')
	{
	  fprintf (stderr, "internal error: can't hash `%s': %s\n",
		   mips_opcodes[i].name, retval);
	  as_fatal ("Broken assembler.  No assembly attempted.");
	}
      do
	{
	  if ((mips_opcodes[i].match & mips_opcodes[i].mask) !=
	      mips_opcodes[i].match)
	    {
	      fprintf (stderr, "internal error: bad opcode: `%s' \"%s\"\n",
		       mips_opcodes[i].name, mips_opcodes[i].args);
	      as_fatal ("Broken assembler.  No assembly attempted.");
	    }
	  ++i;
	}
      while ((i < NUMOPCODES) && !strcmp (mips_opcodes[i].name, name));
    }

  mips_no_prev_insn ();

#ifndef OBJ_ECOFF
  md_obj_begin ();
#endif
}

void
md_end ()
{
#ifndef OBJ_ECOFF
  md_obj_end ();
#endif
}

void
md_assemble (str)
     char *str;
{
  struct mips_cl_insn insn;
  static int init;

  if (!init)
    {
      /* set the default alignment for the text section (2**2) */
      /* This should go in md_begin but text_section isn't initialized then */
      record_alignment (text_section, 2);
#ifdef OBJ_ECOFF
      bfd_set_gp_size (stdoutput, g_switch_value);
#endif
      init = 1;
    }

  imm_expr.X_op = O_absent;
  offset_expr.X_op = O_absent;

  mips_ip (str, &insn);
  if (insn_error)
    {
      as_bad ("%s `%s'", insn_error, str);
      return;
    }
  if (insn.insn_mo->pinfo == INSN_MACRO)
    {
      macro (&insn);
    }
  else
    {
      if (imm_expr.X_op != O_absent)
	append_insn (&insn, &imm_expr, imm_reloc);
      else if (offset_expr.X_op != O_absent)
	append_insn (&insn, &offset_expr, offset_reloc);
      else
	append_insn (&insn, NULL, BFD_RELOC_UNUSED);
    }
}

/* See whether instruction IP reads register REG.  If FPR is non-zero,
   REG is a floating point register.  */

static int
insn_uses_reg (ip, reg, fpr)
     struct mips_cl_insn *ip;
     int reg;
     int fpr;
{
  /* Don't report on general register 0, since it never changes.  */
  if (! fpr && reg == 0)
    return 0;

  if (fpr)
    {
      /* If we are called with either $f0 or $f1, we must check $f0.
	 This is not optimal, because it will introduce an unnecessary
	 NOP between "lwc1 $f0" and "swc1 $f1".  To fix this we would
	 need to distinguish reading both $f0 and $f1 or just one of
	 them.  Note that we don't have to check the other way,
	 because there is no instruction that sets both $f0 and $f1
	 and requires a delay.  */
      if ((ip->insn_mo->pinfo & INSN_READ_FPR_S)
	  && ((ip->insn_opcode >> OP_SH_FS) & OP_MASK_FS) == (reg &~ 1))
	return 1;
      if ((ip->insn_mo->pinfo & INSN_READ_FPR_T)
	  && ((ip->insn_opcode >> OP_SH_FT) & OP_MASK_FT) == (reg &~ 1))
	return 1;
    }
  else
    {
      if ((ip->insn_mo->pinfo & INSN_READ_GPR_S)
	  && ((ip->insn_opcode >> OP_SH_RS) & OP_MASK_RS) == reg)
	return 1;
      if ((ip->insn_mo->pinfo & INSN_READ_GPR_T)
	  && ((ip->insn_opcode >> OP_SH_RT) & OP_MASK_RT) == reg)
	return 1;
    }

  return 0;
}

#define ALIGN_ERR "Attempt to assemble instruction onto non word boundary."
#define ALIGN_ERR2 "GAS doesn't do implicit alignment; use .align directive."

/*
 *                                append insn
 * Output an instruction.
 */
static void
append_insn (ip, address_expr, reloc_type)
     struct mips_cl_insn *ip;
     expressionS *address_expr;
     bfd_reloc_code_real_type reloc_type;
{
  char *f;
  fixS *fixp;
  int nops = 0;

  if (! mips_noreorder)
    {
      /* If the previous insn required any delay slots, see if we need
	 to insert a NOP or two.  There are six kinds of possible
	 hazards, of which an instruction can have at most one type.
	 (1) a load delay
	 (2) an unconditional branch delay
	 (3) a conditional branch delay
	 (4) a generic coprocessor delay
	 (5) a coprocessor condition code delay
	 (6) a HI/LO special register delay

	 There are a lot of optimizations we could do that we don't.
	 In particular, we do not, in general, reorder instructions.
	 If you use gcc with optimization, it will reorder
	 instructions and generally do much more optimization then we
	 do here; repeating all that work in the assembler would only
	 benefit hand written assembly code, and does not seem worth
	 it.  */

      /* This is how a NOP is emitted.  */
#define emit_nop() md_number_to_chars (frag_more (4), 0, 4)

      /* The previous insn might require a delay slot, depending upon
	 the contents of the current insn.  */
      if (prev_insn.insn_mo->pinfo & INSN_LOAD_DELAY)
	{
	  /* A load delay.  All load delays delay the use of general
	     register rt for one instruction.  */
	  know (prev_insn.insn_mo->pinfo & INSN_WRITE_GPR_T);
	  if (mips_optimize == 0
	      || insn_uses_reg (ip,
				((prev_insn.insn_opcode >> OP_SH_RT)
				 & OP_MASK_RT),
				0))
	    ++nops;
	}
      else if (prev_insn.insn_mo->pinfo & INSN_COPROC_DELAY)
	{
	  /* A generic coprocessor delay.  The previous instruction
	     modified a coprocessor general or control register.  If
	     it modified a control register, we need to avoid any
	     coprocessor instruction (this is probably not always
	     required, but it sometimes is).  If it modified a general
	     register, we avoid using that register.

	     This case is not handled very well.  There is no special
	     knowledge of CP0 handling, and the coprocessors other
	     than the floating point unit are not distinguished at
	     all.  */
	  if (prev_insn.insn_mo->pinfo & INSN_WRITE_FPR_T)
	    {
	      if (mips_optimize == 0
		  || insn_uses_reg (ip,
				    ((prev_insn.insn_opcode >> OP_SH_RT)
				     & OP_MASK_RT),
				    1))
		++nops;
	    }
	  else if (prev_insn.insn_mo->pinfo & INSN_WRITE_FPR_D)
	    {
	      if (mips_optimize == 0
		  || insn_uses_reg (ip,
				    ((prev_insn.insn_opcode >> OP_SH_RD)
				     & OP_MASK_RD),
				    1))
		++nops;
	    }
	  else
	    {
	      /* We don't know exactly what the previous instruction
		 does.  If the current instruction uses a coprocessor
		 register, we must insert a NOP.  If previous
		 instruction may set the condition codes, and the
		 current instruction uses them, we must insert two
		 NOPS.  */
	      if (mips_optimize == 0
		  || ((prev_insn.insn_mo->pinfo & INSN_WRITE_COND_CODE)
		      && (ip->insn_mo->pinfo & INSN_READ_COND_CODE)))
		nops += 2;
	      else if (ip->insn_mo->pinfo & INSN_COP)
		++nops;
	    }
	}
      else if (prev_insn.insn_mo->pinfo & INSN_WRITE_COND_CODE)
	{
	  /* The previous instruction sets the coprocessor condition
	     codes, but does not require a general coprocessor delay
	     (this means it is a floating point comparison
	     instruction).  If this instruction uses the condition
	     codes, we need to insert a single NOP.  */
	  if (mips_optimize == 0
	      || ip->insn_mo->pinfo & INSN_READ_COND_CODE)
	    ++nops;
	}
      else if (prev_insn.insn_mo->pinfo & INSN_READ_LO)
	{
	  /* The previous instruction reads the LO register; if the
	     current instruction writes to the LO register, we must
	     insert two NOPS.  */
	  if (mips_optimize == 0
	      || ip->insn_mo->pinfo & INSN_WRITE_LO)
	    nops += 2;
	}
      else if (prev_insn.insn_mo->pinfo & INSN_READ_HI)
	{
	  /* The previous instruction reads the HI register; if the
	     current instruction writes to the HI register, we must
	     insert a NOP.  */
	  if (mips_optimize == 0
	      || ip->insn_mo->pinfo & INSN_WRITE_HI)
	    nops += 2;
	}

      /* There are two cases which require two intervening
	 instructions: 1) setting the condition codes using a move to
	 coprocessor instruction which requires a general coprocessor
	 delay and then reading the condition codes 2) reading the HI
	 or LO register and then writing to it.  If we are not already
	 emitting a NOP instruction, we must check for these cases
	 compared to the instruction previous to the previous
	 instruction.  */
      if (nops == 0
	  && (((prev_prev_insn.insn_mo->pinfo & INSN_COPROC_DELAY)
	       && (prev_prev_insn.insn_mo->pinfo & INSN_WRITE_COND_CODE)
	       && (ip->insn_mo->pinfo & INSN_READ_COND_CODE))
	      || ((prev_prev_insn.insn_mo->pinfo & INSN_READ_LO)
		  && (ip->insn_mo->pinfo & INSN_WRITE_LO))
	      || ((prev_prev_insn.insn_mo->pinfo & INSN_READ_HI)
		  && (ip->insn_mo->pinfo & INSN_WRITE_HI))))
	++nops;

      /* Now emit the right number of NOP instructions.  */
      if (nops > 0)
	{
	  emit_nop ();
	  if (nops > 1)
	    emit_nop ();
	  if (insn_label != NULL)
	    {
	      assert (S_GET_SEGMENT (insn_label) == now_seg);
	      insn_label->sy_frag = frag_now;
	      S_SET_VALUE (insn_label, frag_now_fix ());
	    }
	}
    }
  
  f = frag_more (4);
#if 0
  /* This is testing the address of the frag, not the alignment of
     the instruction in the current section.  */
  if ((int) f & 3)
    {
      as_bad (ALIGN_ERR);
      as_bad (ALIGN_ERR2);
    }
#endif
  fixp = NULL;
  if (address_expr != NULL)
    {
      if (address_expr->X_op == O_constant)
	{
	  switch (reloc_type)
	    {
	    case BFD_RELOC_32:
	      ip->insn_opcode |= address_expr->X_add_number;
	      break;

	    case BFD_RELOC_LO16:
	      ip->insn_opcode |= address_expr->X_add_number & 0xffff;
	      break;

	    case BFD_RELOC_MIPS_JMP:
	    case BFD_RELOC_16_PCREL_S2:
	      goto need_reloc;

	    default:
	      internalError ();
	    }
	}
      else
	{
	  assert (reloc_type != BFD_RELOC_UNUSED);
	need_reloc:
	  fixp = fix_new_exp (frag_now, f - frag_now->fr_literal, 4,
			      address_expr,
			      reloc_type == BFD_RELOC_16_PCREL_S2,
			      reloc_type);
	}
    }

  md_number_to_chars (f, ip->insn_opcode, 4);

  if (! mips_noreorder)
    {
      /* Filling the branch delay slot is more complex.  We try to
	 switch the branch with the previous instruction, which we can
	 do if the previous instruction does not set up a condition
	 that the branch tests and if the branch is not itself the
	 target of any branch.  */
      if ((ip->insn_mo->pinfo & INSN_UNCOND_BRANCH_DELAY)
	  || (ip->insn_mo->pinfo & INSN_COND_BRANCH_DELAY))
	{
	  if (mips_optimize < 2
	      /* If we have seen .set nobopt, don't optimize.  */
	      || mips_nobopt != 0
	      /* If we have seen .set volatile or .set nomove, don't
		 optimize.  */
	      || mips_nomove != 0
	      /* If we had to emit any NOP instructions, then we
		 already know we can not swap.  */
	      || nops != 0
	      /* If we don't even know the previous insn, we can not
		 swap. */
	      || ! prev_insn_valid
	      /* If the previous insn is already in a branch delay
		 slot, then we can not swap.  */
	      || prev_insn_is_delay_slot
	      /* If the previous previous insn was in a .set
		 noreorder, we can't swap.  Actually, the MIPS
		 assembler will swap in this situation.  However, gcc
		 configured -with-gnu-as will generate code like
		   .set noreorder
		   lw	$4,XXX
		   .set	reorder
		   INSN
		   bne	$4,$0,foo
		 in which we can not swap the bne and INSN.  If gcc is
		 not configured -with-gnu-as, it does not output the
		 .set pseudo-ops.  We don't have to check
		 prev_insn_unreordered, because prev_insn_valid will
		 be 0 in that case.  We don't want to use
		 prev_prev_insn_valid, because we do want to be able
		 to swap at the start of a function.  */
	      || prev_prev_insn_unreordered
	      /* If the branch is itself the target of a branch, we
		 can not swap.  We cheat on this; all we check for is
		 whether there is a label on this instruction.  If
		 there are any branches to anything other than a
		 label, users must use .set noreorder.  */
	      || insn_label != NULL
	      /* If the branch reads the condition codes, we don't
		 even try to swap, because in the sequence
		   ctc1 $X,$31
		   INSN
		   INSN
		   bc1t LABEL
		 we can not swap, and I don't feel like handling that
		 case.  */
	      || (ip->insn_mo->pinfo & INSN_READ_COND_CODE)
	      /* We can not swap with an instruction that requires a
		 delay slot, becase the target of the branch might
		 interfere with that instruction.  */
	      || (prev_insn.insn_mo->pinfo
		  & (INSN_LOAD_DELAY
		     | INSN_COPROC_DELAY
		     | INSN_WRITE_COND_CODE
		     | INSN_READ_LO
		     | INSN_READ_HI))
	      /* We can not swap with a branch instruction.  */
	      || (prev_insn.insn_mo->pinfo
		  & (INSN_UNCOND_BRANCH_DELAY | INSN_COND_BRANCH_DELAY))
	      /* If the branch reads a register that the previous
		 instruction sets, we can not swap.  */
	      || ((prev_insn.insn_mo->pinfo & INSN_WRITE_GPR_T)
		  && insn_uses_reg (ip,
				    ((prev_insn.insn_opcode >> OP_SH_RT)
				     & OP_MASK_RT),
				    0))
	      || ((prev_insn.insn_mo->pinfo & INSN_WRITE_GPR_D)
		  && insn_uses_reg (ip,
				    ((prev_insn.insn_opcode >> OP_SH_RD)
				     & OP_MASK_RD),
				    0))
	      /* If the branch writes a register that the previous
		 instruction reads, we can not swap (we know that
		 branches only write to RD or to $31).  */
	      || ((ip->insn_mo->pinfo & INSN_WRITE_GPR_D)
		  && insn_uses_reg (&prev_insn,
				    ((ip->insn_opcode >> OP_SH_RD)
				     & OP_MASK_RD),
				    0))
	      || ((ip->insn_mo->pinfo & INSN_WRITE_GPR_31)
		  && insn_uses_reg (&prev_insn, 31, 0))
	      /* If the previous previous instruction has a load
		 delay, and sets a register that the branch reads, we
		 can not swap.  */
	      || ((prev_prev_insn.insn_mo->pinfo & INSN_LOAD_DELAY)
		  && insn_uses_reg (ip,
				    ((prev_prev_insn.insn_opcode >> OP_SH_RT)
				     & OP_MASK_RT),
				    0)))
	    {
	      /* We could do even better for unconditional branches to
		 portions of this object file; we could pick up the
		 instruction at the destination, put it in the delay
		 slot, and bump the destination address.  */
	      emit_nop ();
	      /* Update the previous insn information.  */
	      prev_prev_insn = *ip;
	      prev_insn.insn_mo = &dummy_opcode;
	    }
	  else
	    {
	      char *prev_f;
	      char temp[4];

	      /* It looks like we can actually do the swap.  */
	      prev_f = prev_insn_frag->fr_literal + prev_insn_where;
	      memcpy (temp, prev_f, 4);
	      memcpy (prev_f, f, 4);
	      memcpy (f, temp, 4);
	      if (prev_insn_fixp)
		{
		  prev_insn_fixp->fx_frag = frag_now;
		  prev_insn_fixp->fx_where = f - frag_now->fr_literal;
		}
	      if (fixp)
		{
		  fixp->fx_frag = prev_insn_frag;
		  fixp->fx_where = prev_insn_where;
		}
	      /* Update the previous insn information; leave prev_insn
		 unchanged.  */
	      prev_prev_insn = *ip;
	    }
	  prev_insn_is_delay_slot = 1;

	  /* If that was an unconditional branch, forget the previous
	     insn information.  */
	  if (ip->insn_mo->pinfo & INSN_UNCOND_BRANCH_DELAY)
	    {
	      prev_prev_insn.insn_mo = &dummy_opcode;
	      prev_insn.insn_mo = &dummy_opcode;
	    }
	}
      else
	{
	  /* Update the previous insn information.  */
	  if (nops > 0)
	    prev_prev_insn.insn_mo = &dummy_opcode;
	  else
	    prev_prev_insn = prev_insn;
	  prev_insn = *ip;

	  /* Any time we see a branch, we always fill the delay slot
	     immediately; since this insn is not a branch, we know it
	     is not in a delay slot.  */
	  prev_insn_is_delay_slot = 0;
	}

      prev_prev_insn_unreordered = prev_insn_unreordered;
      prev_insn_unreordered = 0;
      prev_insn_frag = frag_now;
      prev_insn_where = f - frag_now->fr_literal;
      prev_insn_fixp = fixp;
      prev_insn_valid = 1;
    }

  /* We just output an insn, so the next one doesn't have a label.  */
  insn_label = NULL;
}

/* This function forgets that there was any previous instruction or
   label.  */

static void
mips_no_prev_insn ()
{
  prev_insn.insn_mo = &dummy_opcode;
  prev_prev_insn.insn_mo = &dummy_opcode;
  prev_insn_valid = 0;
  prev_insn_is_delay_slot = 0;
  prev_insn_unreordered = 0;
  prev_prev_insn_unreordered = 0;
  insn_label = NULL;
}

/* This function must be called whenever we turn on noreorder or emit
   something other than instructions.  It inserts any NOPS which might
   be needed by the previous instruction, and clears the information
   kept for the previous instructions.  */

static void
mips_emit_delays ()
{
  if (! mips_noreorder)
    {
      int nop;

      nop = 0;
      if (prev_insn.insn_mo->pinfo & ANY_DELAY)
	{
	  nop = 1;
	  if ((prev_insn.insn_mo->pinfo & INSN_WRITE_COND_CODE)
	      || (prev_insn.insn_mo->pinfo & INSN_READ_HI)
	      || (prev_insn.insn_mo->pinfo & INSN_READ_LO))
	    emit_nop ();
	}
      else if ((prev_prev_insn.insn_mo->pinfo & INSN_WRITE_COND_CODE)
	       || (prev_prev_insn.insn_mo->pinfo & INSN_READ_HI)
	       || (prev_prev_insn.insn_mo->pinfo & INSN_READ_LO))
	nop = 1;
      if (nop)
	{
	  emit_nop ();
	  if (insn_label != NULL)
	    {
	      assert (S_GET_SEGMENT (insn_label) == now_seg);
	      insn_label->sy_frag = frag_now;
	      S_SET_VALUE (insn_label, frag_now_fix ());
	    }
	}
      mips_no_prev_insn ();
    }
}

/* Return 1 if an expression can be accessed via the GP register.  */

static int
gp_reference (ep)
     expressionS *ep;
{
#ifdef OBJ_ECOFF
  symbolS *sym;
  const char *symname;
  const char *segname;

  sym = ep->X_add_symbol;
  if (sym == (symbolS *) NULL
      || ep->X_op_symbol != (symbolS *) NULL)
    return 0;

  /* Certain symbols can not be referenced off the GP, although it
     appears as though they can.  */
  symname = S_GET_NAME (sym);
  if (symname != (const char *) NULL
      && (strcmp (symname, "eprol") == 0
	  || strcmp (symname, "etext") == 0
	  || strcmp (symname, "_gp") == 0
	  || strcmp (symname, "edata") == 0
	  || strcmp (symname, "_fbss") == 0
	  || strcmp (symname, "_fdata") == 0
	  || strcmp (symname, "_ftext") == 0
	  || strcmp (symname, "end") == 0))
    return 0;
  if (! S_IS_DEFINED (sym)
      && S_GET_VALUE (sym) != 0
      && S_GET_VALUE (sym) <= g_switch_value)
    return 1;
  segname = segment_name (S_GET_SEGMENT (ep->X_add_symbol));
  return (strcmp (segname, ".sdata") == 0
	  || strcmp (segname, ".sbss") == 0
	  || strcmp (segname, ".lit8") == 0
	  || strcmp (segname, ".lit4") == 0);
#else /* ! defined (OBJ_ECOFF) */
  /* The GP register is only used for ECOFF.  */
  return 0;
#endif /* ! defined (OBJ_ECOFF) */  
}

/* Build an instruction created by a macro expansion.  This is passed
   a pointer to the count of instructions created so far, an
   expression, the name of the instruction to build, an operand format
   string, and corresponding arguments.  */

#ifndef NO_STDARG
static void
macro_build (int *counter,
	     expressionS * ep,
	     const char *name,
	     const char *fmt,
	     ...)
#else /* ! defined (NO_STDARG) */
static void
macro_build (counter, ep, name, fmt, va_alist)
     int *counter;
     expressionS *ep;
     const char *name;
     const char *fmt;
     va_dcl
#endif /* ! defined (NO_STDARG) */
{
  struct mips_cl_insn insn;
  bfd_reloc_code_real_type r;
  va_list args;

#ifndef NO_STDARG
  va_start (args, fmt);
#else
  va_start (args);
#endif

  /*
   * If the macro is about to expand into a second instruction,
   * print a warning if needed. We need to pass ip as a parameter
   * to generate a better warning message here...
   */
  if (mips_warn_about_macros && *counter == 1)
    as_warn ("Macro instruction expanded into multiple instructions");

  *counter += 1;		/* bump instruction counter */

  r = BFD_RELOC_UNUSED;
  insn.insn_mo = (struct mips_opcode *) hash_find (op_hash, name);
  assert (insn.insn_mo);
  assert (strcmp (name, insn.insn_mo->name) == 0);

  while (strcmp (fmt, insn.insn_mo->args) != 0)
    {
      ++insn.insn_mo;
      assert (insn.insn_mo->name);
      assert (strcmp (name, insn.insn_mo->name) == 0);
    }
  assert (insn.insn_mo->pinfo != INSN_MACRO);
  insn.insn_opcode = insn.insn_mo->match;
  for (;;)
    {
      switch (*fmt++)
	{
	case '\0':
	  break;

	case ',':
	case '(':
	case ')':
	  continue;

	case 't':
	case 'w':
	case 'E':
	  insn.insn_opcode |= va_arg (args, int) << 16;
	  continue;

	case 'c':
	case 'T':
	case 'W':
	  insn.insn_opcode |= va_arg (args, int) << 16;
	  continue;

	case 'd':
	case 'G':
	  insn.insn_opcode |= va_arg (args, int) << 11;
	  continue;

	case 'V':
	case 'S':
	  insn.insn_opcode |= va_arg (args, int) << 11;
	  continue;

	case '<':
	  insn.insn_opcode |= va_arg (args, int) << 6;
	  continue;

	case 'D':
	  insn.insn_opcode |= va_arg (args, int) << 6;
	  continue;

	case 'B':
	  insn.insn_opcode |= va_arg (args, int) << 6;
	  continue;

	case 'b':
	case 's':
	case 'r':
	case 'v':
	  insn.insn_opcode |= va_arg (args, int) << 21;
	  continue;

	case 'i':
	case 'j':
	case 'o':
	  r = BFD_RELOC_LO16;
	  continue;

	case 'p':
	  assert (ep != NULL);
	  /*
	   * This allows macro() to pass an immediate expression for
	   * creating short branches without creating a symbol.
	   * Note that the expression still might come from the assembly
	   * input, in which case the value is not checked for range nor
	   * is a relocation entry generated (yuck).
	   */
	  if (ep->X_op == O_constant)
	    {
	      insn.insn_opcode |= (ep->X_add_number >> 2) & 0xffff;
	      ep = NULL;
	    }
	  else
	    r = BFD_RELOC_16_PCREL_S2;
	  continue;

	default:
	  internalError ();
	}
      break;
    }
  va_end (args);
  assert (r == BFD_RELOC_UNUSED ? ep == NULL : ep != NULL);

  /* Use GP relative addressing if possible.  */
  if (r == BFD_RELOC_LO16
      && gp_reference (ep))
    r = BFD_RELOC_MIPS_GPREL;

  append_insn (&insn, ep, r);
}

/*
 * Generate a "lui" instruction.
 */
static void
macro_build_lui (counter, ep, regnum)
     int *counter;
     expressionS *ep;
     int regnum;
{
  expressionS high_expr;
  struct mips_cl_insn insn;
  bfd_reloc_code_real_type r;
  CONST char *name = "lui";
  CONST char *fmt = "t,u";

  high_expr = *ep;

  if (high_expr.X_op == O_constant)
    {
      /* we can compute the instruction now without a relocation entry */
      if (high_expr.X_add_number & 0x8000)
	high_expr.X_add_number += 0x10000;
      high_expr.X_add_number =
	((unsigned long) high_expr.X_add_number >> 16) & 0xffff;
      r = BFD_RELOC_UNUSED;
    }
  else
    r = BFD_RELOC_HI16_S;

  /*
   * If the macro is about to expand into a second instruction,
   * print a warning if needed. We need to pass ip as a parameter
   * to generate a better warning message here...
   */
  if (mips_warn_about_macros && *counter == 1)
    as_warn ("Macro instruction expanded into multiple instructions");

  *counter += 1;		/* bump instruction counter */

  insn.insn_mo = (struct mips_opcode *) hash_find (op_hash, name);
  assert (insn.insn_mo);
  assert (strcmp (name, insn.insn_mo->name) == 0);
  assert (strcmp (fmt, insn.insn_mo->args) == 0);

  insn.insn_opcode = insn.insn_mo->match | (regnum << 16);
  if (r == BFD_RELOC_UNUSED)
    {
      insn.insn_opcode |= high_expr.X_add_number;
      append_insn (&insn, NULL, r);
    }
  else
    append_insn (&insn, &high_expr, r);
}

/*			set_at()
 * Generates code to set the $at register to true (one)
 * if reg is less than the immediate expression.
 */
static void
set_at (counter, reg)
     int *counter;
     int reg;
{

  switch (imm_expr.X_add_number & 0xffff8000)
    {
    case 0:
    case 0xffff8000:
      macro_build (counter, &imm_expr, "slti", "t,r,j", AT, reg);
      return;

    case 0x8000:
      macro_build (counter, &imm_expr, "ori", "t,r,i", AT, 0);
      break;

    default:
      macro_build_lui (counter, &imm_expr, AT);
      if (imm_expr.X_add_number & 0xffff)
	macro_build (counter, &imm_expr, "addiu", "t,r,j", AT, AT);
    }
  macro_build (counter, NULL, "slt", "d,v,t", AT, reg, AT);
}

/*			set_at_unsigned()
 * Generates code to set the $at register to true (one)
 * if reg is less than the immediate expression.
 * Unsigned comparison is perfomed.
 */
static void
set_at_unsigned (counter, reg)
     int *counter;
     int reg;
{

  switch (imm_expr.X_add_number & 0xffff8000)
    {
    case 0:
    case 0xffff8000:
      macro_build (counter, &imm_expr, "sltiu", "t,r,j", AT, reg);
      return;

    case 0x8000:
      macro_build (counter, &imm_expr, "ori", "t,r,i", AT, 0);
      break;

    default:
      macro_build_lui (counter, &imm_expr, AT);
      if (imm_expr.X_add_number & 0xffff)
	macro_build (counter, &imm_expr, "addiu", "t,r,j", AT, AT);
    }
  macro_build (counter, NULL, "sltu", "d,v,t", AT, reg, AT);
}

static void
check_absolute_expr (ip, ex)
     struct mips_cl_insn *ip;
     expressionS *ex;
{
  if (ex->X_op != O_constant)
    as_warn ("Instruction %s requires absolute expression", ip->insn_mo->name);
}

/*			load_register()
 *  This routine generates the least number of instructions neccessary to load
 *  an absolute expression value into a register.
 */
static void
load_register (counter, ip, reg, ep)
     int *counter;
     struct mips_cl_insn *ip;
     int reg;
     expressionS *ep;
{
  switch (ep->X_add_number & 0xffff8000)
    {
    case 0:
    case 0xffff8000:
      macro_build (counter, ep, "addiu", "t,r,j", reg, 0);
      break;

    case 0x8000:
      macro_build (counter, ep, "ori", "t,r,i", reg, 0);
      break;

    default:
      macro_build_lui (counter, ep, reg);
      if (ep->X_add_number & 0xffff)
	macro_build (counter, ep, "addiu", "t,r,j", reg, reg);
    }
}


/*
 *			Build macros
 *   This routine implements the seemingly endless macro or synthesized
 * instructions and addressing modes in the mips assembly language. Many
 * of these macros are simple and are similar to each other. These could
 * probably be handled by some kind of table or grammer aproach instead of
 * this verbose method. Others are not simple macros but are more like
 * optimizing code generation.
 *   One interesting optimization is when several store macros appear
 * consecutivly that would load AT with the upper half of the same address.
 * The ensuing load upper instructions are ommited. This implies some kind
 * of global optimization. We currently only optimize within a single macro.
 *   For many of the load and store macros if the address is specified as a
 * constant expression in the first 64k of memory (ie ld $2,0x4000c) we
 * first load register 'at' with zero and use it as the base register. The
 * mips assembler simply uses register $zero. Just one tiny optimization
 * we're missing.
 */
static void
macro (ip)
     struct mips_cl_insn *ip;
{
  register int treg, sreg, dreg, breg;
  int tempreg;
  int mask;
  int icnt = 0;
  int used_at;
  expressionS expr1;
  const char *s;
  const char *fmt;

  treg = (ip->insn_opcode >> 16) & 0x1f;
  dreg = (ip->insn_opcode >> 11) & 0x1f;
  sreg = breg = (ip->insn_opcode >> 21) & 0x1f;
  mask = ip->insn_mo->mask;

  expr1.X_op = O_constant;
  expr1.X_op_symbol = NULL;
  expr1.X_add_symbol = NULL;
  expr1.X_add_number = 1;

  switch (mask)
    {
    case M_ABS:
    case M_ABSU:
      /*
	Note: mips algorithm requires the move in the delay slot.
	<main>:		bgez $a0,0x4001bc <main+12>
	<main+4>:	move v0,$a0
	<main+8>:	sub v0,$zero,$a0
	<main+12>:	nop
	*/

      mips_emit_delays ();
      ++mips_noreorder;

      expr1.X_add_number = 8;
      macro_build (&icnt, &expr1, "bgez", "s,p", sreg);
      macro_build (&icnt, NULL, "move", "d,s", dreg, sreg, 0);
      macro_build (&icnt, NULL, mask == M_ABS ? "sub" : "subu", "d,v,t",
		   dreg, 0, sreg);

      --mips_noreorder;
      return;

    case M_ADD_I:
    case M_ADDU_I:
      switch (imm_expr.X_add_number & 0xffff8000)
	{
	case 0:
	case 0xffff8000:
	  macro_build (&icnt, &imm_expr,
		   mask == M_ADD_I ? "addi" : "addiu", "t,r,j", treg, sreg);
	  return;

	case 0x8000:
	  macro_build (&icnt, &imm_expr, "ori", "t,r,i", AT, 0);
	  break;

	default:
	  macro_build_lui (&icnt, &imm_expr, AT);
	  if (imm_expr.X_add_number & 0xffff)
	    macro_build (&icnt, &imm_expr, "addiu", "t,r,j", AT, AT);
	  break;
	}
      macro_build (&icnt, NULL,
		 mask == M_ADD_I ? "add" : "addu", "d,v,t", treg, sreg, AT);
      break;

    case M_AND_I:
    case M_OR_I:
    case M_NOR_I:
    case M_XOR_I:
      switch (imm_expr.X_add_number & 0xffff8000)
	{
	case 0:
	case 0x8000:
	  switch (mask)
	    {
	    case M_AND_I:
	      macro_build (&icnt, &imm_expr, "andi", "t,r,i", treg, sreg);
	      return;
	    case M_OR_I:
	      macro_build (&icnt, &imm_expr, "ori", "t,r,i", treg, sreg);
	      return;
	    case M_NOR_I:
	      macro_build (&icnt, &imm_expr, "ori", "t,r,i", treg, sreg);
	      macro_build (&icnt, &imm_expr, "nor", "d,v,t", treg, treg, 0);
	      return;
	    case M_XOR_I:
	      macro_build (&icnt, &imm_expr, "xori", "t,r,i", treg, sreg);
	      return;
	    default:
	      internalError ();
	    }

	case 0xffff8000:
	  macro_build (&icnt, &imm_expr, "addiu", "t,r,j", AT, 0);
	  break;

	default:
	  macro_build_lui (&icnt, &imm_expr, AT);
	  if (imm_expr.X_add_number & 0xffff)
	    macro_build (&icnt, &imm_expr, "addiu", "t,r,j", AT, AT);
	}
      switch (mask)
	{
	case M_AND_I:
	  macro_build (&icnt, NULL, "and", "d,v,t", treg, sreg, AT);
	  break;
	case M_OR_I:
	  macro_build (&icnt, NULL, "or", "d,v,t", treg, sreg, AT);
	  break;
	case M_NOR_I:
	  macro_build (&icnt, NULL, "nor", "d,v,t", treg, sreg, AT);
	  break;
	case M_XOR_I:
	  macro_build (&icnt, NULL, "xor", "d,v,t", treg, sreg, AT);
	  break;
	default:
	  internalError ();
	}
      break;

    case M_BEQ_I:
    case M_BNE_I:
      if (imm_expr.X_add_number == 0)
	{
	  macro_build (&icnt, &offset_expr, mask == M_BEQ_I ? "beq" : "bne",
		       "s,t,p", sreg, 0);
	  return;
	}
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, &offset_expr, mask == M_BEQ_I ? "beq" : "bne",
		   "s,t,p", sreg, AT);
      break;

    case M_BGE:
      if (treg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bgez", "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build (&icnt, &offset_expr, "blez", "s,p", treg);
	  return;
	}
      macro_build (&icnt, NULL, "slt", "d,v,t", AT, sreg, treg);
      macro_build (&icnt, &offset_expr, "beq", "s,t,p", AT, 0);
      break;

    case M_BGT_I:
      /* check for > max integer */
      if (imm_expr.X_add_number == 0x7fffffff)
	{
	do_false:
	  /* result is always false */
	  as_warn ("Branch %s is always false (nop)", ip->insn_mo->name);
	  macro_build (&icnt, NULL, "nop", "", 0);
	  return;
	}
      imm_expr.X_add_number++;
      /* FALLTHROUGH */

    case M_BGE_I:
      if (imm_expr.X_add_number == 0)
	{
	  macro_build (&icnt, &offset_expr, "bgez", "s,p", sreg);
	  return;
	}
      if (imm_expr.X_add_number == 1)
	{
	  macro_build (&icnt, &offset_expr, "bgtz", "s,p", sreg);
	  return;
	}
      if (imm_expr.X_add_number == 0x80000000)
	{
	do_true:
	  /* result is always true */
	  as_warn ("Branch %s is always true", ip->insn_mo->name);
	  macro_build (&icnt, &offset_expr, "b", "p");
	  return;
	}
      set_at (&icnt, sreg);
      macro_build (&icnt, &offset_expr, "beq", "s,t,p", AT, 0);
      break;

    case M_BGEU:
      if (treg == 0)
	goto do_true;
      if (sreg == 0)
	{
	  macro_build (&icnt, &offset_expr, "beq", "s,t,p", 0, treg);
	  return;
	}
      macro_build (&icnt, NULL, "sltu", "d,v,t", AT, sreg, treg);
      macro_build (&icnt, &offset_expr, "beq", "s,t,p", AT, 0);
      break;

    case M_BGTU_I:
      if (sreg == 0 || imm_expr.X_add_number == 0xffffffff)
	goto do_false;
      imm_expr.X_add_number++;
      /* FALLTHROUGH */

    case M_BGEU_I:
      if (imm_expr.X_add_number == 0)
	goto do_true;
      if (imm_expr.X_add_number == 1)
	{
	  macro_build (&icnt, &offset_expr, "bne", "s,t,p", sreg, 0);
	  return;
	}
      set_at_unsigned (&icnt, sreg);
      macro_build (&icnt, &offset_expr, "beq", "s,t,p", AT, 0);
      break;

    case M_BGT:
      if (treg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bgtz", "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bltz", "s,p", treg);
	  return;
	}
      macro_build (&icnt, NULL, "slt", "d,v,t", AT, treg, sreg);
      macro_build (&icnt, &offset_expr, "bne", "s,t,p", AT, 0);
      break;

    case M_BGTU:
      if (treg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bne", "s,t,p", sreg, 0);
	  return;
	}
      if (sreg == 0)
	goto do_false;
      macro_build (&icnt, NULL, "sltu", "d,v,t", AT, treg, sreg);
      macro_build (&icnt, &offset_expr, "bne", "s,t,p", AT, 0);
      break;

    case M_BLE:
      if (treg == 0)
	{
	  macro_build (&icnt, &offset_expr, "blez", "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bgez", "s,p", treg);
	  return;
	}
      macro_build (&icnt, NULL, "slt", "d,v,t", AT, treg, sreg);
      macro_build (&icnt, &offset_expr, "beq", "s,t,p", AT, 0);
      break;

    case M_BLE_I:
      if (imm_expr.X_add_number == 0x7fffffff)
	goto do_true;
      imm_expr.X_add_number++;
      /* FALLTHROUGH */

    case M_BLT_I:
      if (imm_expr.X_add_number == 0)
	{
	  macro_build (&icnt, &offset_expr, "bltz", "s,p", sreg);
	  return;
	}
      if (imm_expr.X_add_number == 1)
	{
	  macro_build (&icnt, &offset_expr, "blez", "s,p", sreg);
	  return;
	}
      set_at (&icnt, sreg);
      macro_build (&icnt, &offset_expr, "bne", "s,t,p", AT, 0);
      break;

    case M_BLEU:
      if (treg == 0)
	{
	  macro_build (&icnt, &offset_expr, "beq", "s,t,p", sreg, 0);
	  return;
	}
      if (sreg == 0)
	goto do_true;
      macro_build (&icnt, NULL, "sltu", "d,v,t", AT, treg, sreg);
      macro_build (&icnt, &offset_expr, "beq", "s,t,p", AT, 0);
      break;

    case M_BLEU_I:
      if (sreg == 0 || imm_expr.X_add_number == 0xffffffff)
	goto do_true;
      imm_expr.X_add_number++;
      /* FALLTHROUGH */

    case M_BLTU_I:
      if (imm_expr.X_add_number == 0)
	goto do_false;
      if (imm_expr.X_add_number == 1)
	{
	  macro_build (&icnt, &offset_expr, "beq", "s,t,p", sreg, 0);
	  return;
	}
      set_at_unsigned (&icnt, sreg);
      macro_build (&icnt, &offset_expr, "bne", "s,t,p", AT, 0);
      break;

    case M_BLT:
      if (treg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bltz", "s,p", sreg);
	  return;
	}
      if (sreg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bgtz", "s,p", treg);
	  return;
	}
      macro_build (&icnt, NULL, "slt", "d,v,t", AT, sreg, treg);
      macro_build (&icnt, &offset_expr, "bne", "s,t,p", AT, 0);
      break;

    case M_BLTU:
      if (treg == 0)
	goto do_false;
      if (sreg == 0)
	{
	  macro_build (&icnt, &offset_expr, "bne", "s,t,p", 0, treg);
	  return;
	}
      macro_build (&icnt, NULL, "sltu", "d,v,t", AT, sreg, treg);
      macro_build (&icnt, &offset_expr, "bne", "s,t,p", AT, 0);
      break;

    case M_DIV_3:
    case M_REM_3:
      if (treg == 0)
	{
	  as_warn ("Divide by zero.");
	  macro_build (&icnt, NULL, "break", "c", 7);
	  return;
	}

      mips_emit_delays ();
      ++mips_noreorder;
      macro_build (&icnt, NULL, "div", "s,t", sreg, treg);
      expr1.X_add_number = 8;
      macro_build (&icnt, &expr1, "bne", "s,t,p", treg, 0);
      macro_build (&icnt, NULL, "nop", "", 0);
      macro_build (&icnt, NULL, "break", "c", 7);
      expr1.X_add_number = -1;
      macro_build (&icnt, &expr1, "addiu", "t,r,j", AT, 0);
      expr1.X_add_number = 16;
      macro_build (&icnt, &expr1, "bne", "s,t,p", treg, AT);
      expr1.X_add_number = 0x80000000;
      macro_build_lui (&icnt, &expr1, AT);
      expr1.X_add_number = 8;
      macro_build (&icnt, &expr1, "bne", "s,t,p", sreg, AT);
      macro_build (&icnt, NULL, "nop", "", 0);
      macro_build (&icnt, NULL, "break", "c", 6);
      --mips_noreorder;
      macro_build (&icnt, NULL, mask == M_DIV_3 ? "mflo" : "mfhi", "d", dreg);
      /* with reorder on there will be two implicit nop instructions here. */
      break;

    case M_DIV_3I:
    case M_DIVU_3I:
    case M_REM_3I:
    case M_REMU_3I:
      if (imm_expr.X_add_number == 0)
	{
	  as_warn ("Divide by zero.");
	  macro_build (&icnt, NULL, "break", "c", 7);
	  return;
	}
      if (imm_expr.X_add_number == 1)
	{
	  if (mask == (int) M_DIV_3I || mask == (int) M_DIVU_3I)
	    macro_build (&icnt, NULL, "move", "d,s", dreg, sreg);
	  else
	    macro_build (&icnt, NULL, "move", "d,s", dreg, 0);
	  return;
	}

      load_register (&icnt, ip, AT, &imm_expr);
      if (mask == (int) M_DIV_3I || mask == (int) M_REM_3I)
	macro_build (&icnt, NULL, "div", "s,t", sreg, AT);
      else
	macro_build (&icnt, NULL, "divu", "s,t", sreg, AT);

      if (mask == (int) M_DIV_3I || mask == (int) M_DIVU_3I)
	macro_build (&icnt, NULL, "mflo", "d", dreg);
      else
	macro_build (&icnt, NULL, "mfhi", "d", dreg);
      /* two implicit nop's required for mflo or mfhi */
      break;

    case M_DIVU_3:
    case M_REMU_3:
      mips_emit_delays ();
      ++mips_noreorder;
      macro_build (&icnt, NULL, "divu", "s,t", sreg, treg);
      expr1.X_add_number = 8;
      macro_build (&icnt, &expr1, "bne", "s,t,p", treg, 0);
      macro_build (&icnt, NULL, "nop", "", 0);
      macro_build (&icnt, NULL, "break", "c", 7);
      --mips_noreorder;
      macro_build (&icnt, NULL, mask == M_DIVU_3 ? "mflo" : "mfhi", "d", dreg);
      /* with reorder on there will be two implicit nop instructions here. */
      return;

    case M_LA:
      if (offset_expr.X_op == O_constant)
	{
	  load_register (&icnt, ip, treg, &offset_expr);
	  return;
	}
      if (gp_reference (&offset_expr))
	macro_build (&icnt, &offset_expr, "addiu", "t,r,j", treg, GP);
      else
	{
	  macro_build_lui (&icnt, &offset_expr, treg);
	  macro_build (&icnt, &offset_expr, "addiu", "t,r,j", treg, treg);
	}
      return;

    case M_LA_AB:
      tempreg = (breg == treg) ? AT : treg;
      if (offset_expr.X_op == O_constant)
	load_register (&icnt, ip, tempreg, &offset_expr);
      else if (gp_reference (&offset_expr))
	macro_build (&icnt, &offset_expr, "addiu", "t,r,j", tempreg, GP);
      else
	{
	  macro_build_lui (&icnt, &offset_expr, tempreg);
	  macro_build (&icnt, &offset_expr, "addiu", "t,r,j", tempreg, tempreg);
	}
      if (breg != 0)
	macro_build (&icnt, NULL, "addu", "d,v,t", treg, tempreg, breg);
      if (breg == treg)
	break;
      return;

    case M_LB_AB:
      s = "lb";
      goto ld;
    case M_LBU_AB:
      s = "lbu";
      goto ld;
    case M_LH_AB:
      s = "lh";
      goto ld;
    case M_LHU_AB:
      s = "lhu";
      goto ld;
    case M_LW_AB:
      s = "lw";
      goto ld;
    case M_LWC0_AB:
      s = "lwc0";
      goto ld;
    case M_LWC1_AB:
    case M_LI_SS:
      s = "lwc1";
      goto ld;
    case M_LWC2_AB:
      s = "lwc2";
      goto ld;
    case M_LWC3_AB:
      s = "lwc3";
      goto ld;
    case M_LWL_AB:
      s = "lwl";
      goto ld;
    case M_LWR_AB:
      s = "lwr";
    ld:
      if (breg == treg
	  || mask == M_LWC0_AB
	  || mask == M_LWC1_AB
	  || mask == M_LI_SS
	  || mask == M_LWC2_AB
	  || mask == M_LWC3_AB)
	{
	  tempreg = AT;
	  used_at = 1;
	}
      else
	{
	  tempreg = treg;
	  used_at = 0;
	}
      goto ld_st;
    case M_SB_AB:
      s = "sb";
      goto st;
    case M_SH_AB:
      s = "sh";
      goto st;
    case M_SW_AB:
      s = "sw";
      goto st;
    case M_SWC0_AB:
      s = "swc0";
      goto st;
    case M_SWC1_AB:
      s = "swc1";
      goto st;
    case M_SWC2_AB:
      s = "swc2";
      goto st;
    case M_SWC3_AB:
      s = "swc3";
      goto st;
    case M_SWL_AB:
      s = "swl";
      goto st;
    case M_SWR_AB:
      s = "swr";
    st:
      tempreg = AT;
      used_at = 1;
    ld_st:
      if (mask == M_LWC1_AB || mask == M_SWC1_AB || mask == M_LI_SS)
	fmt = "T,o(b)";
      else if (mask == M_LWC0_AB
	       || mask == M_LWC2_AB
	       || mask == M_LWC3_AB
	       || mask == M_SWC0_AB
	       || mask == M_SWC2_AB
	       || mask == M_SWC3_AB)
	fmt = "E,o(b)";
      else
	fmt = "t,o(b)";
      if (gp_reference (&offset_expr))
	{
	  if (breg == 0)
	    {
	      macro_build (&icnt, &offset_expr, s, fmt, treg, GP);
	      return;
	    }
	  macro_build (&icnt, (expressionS *) NULL, "addu", "d,v,t",
		       tempreg, breg, GP);
	}
      else
	{
	  macro_build_lui (&icnt, &offset_expr, tempreg);
	  if (breg != 0)
	    macro_build (&icnt, NULL, "addu", "d,v,t", tempreg, tempreg, breg);
	}
      macro_build (&icnt, &offset_expr, s, fmt, treg, tempreg);
      if (used_at)
	break;
      return;

    case M_LI:
    case M_LI_S:
      load_register (&icnt, ip, treg, &imm_expr);
      return;

    case M_LI_D:
      /*  lui $at,%hi(foo)
	  lw $v0,%lo(foo)($at)
	  lw $v1,%lo(foo+4)($at)
	  .rdata
	 foo:
	  .double 3.133435
	 */
      macro_build_lui (&icnt, &offset_expr, AT);
      macro_build (&icnt, &offset_expr, "lw", "t,o(b)", treg, AT);
      offset_expr.X_add_number = 4;
      macro_build (&icnt, &offset_expr, "lw", "t,o(b)", treg + 1, AT);
      break;

    case M_LI_DD:
      /* Load a floating point number from the .lit8 section.  */
      breg = GP;
      /* Fall through.  */
    case M_L_DOB:
      /* Even on a big endian machine $fn comes before $fn+1.  We have
	 to adjust when loading from memory.  */
      macro_build (&icnt, &offset_expr, "lwc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg : treg + 1,
		   breg);
      offset_expr.X_add_number += 4;
      macro_build (&icnt, &offset_expr, "lwc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg + 1 : treg,
		   breg);
      return;

    case M_L_DAB:
      /*
       * The MIPS assembler seems to check for X_add_number not
       * being double aligned and generating:
       *	lui	at,%hi(foo+1)
       *	addu	at,at,v1
       *	addiu	at,at,%lo(foo+1)
       *	lwc1	f2,0(at)
       *	lwc1	f3,4(at)
       * But, the resulting address is the same after relocation so why
       * generate the extra instruction?
       */
      if (gp_reference (&offset_expr))
	{
	  if (breg == 0)
	    tempreg = GP;
	  else
	    {
	      macro_build (&icnt, &offset_expr, "addu", "d,v,t", AT, breg, GP);
	      tempreg = AT;
	    }
	}
      else
	{
	  macro_build_lui (&icnt, &offset_expr, AT);
	  if (breg != 0)
	    macro_build (&icnt, NULL, "addu", "d,v,t", AT, AT, breg);
	  tempreg = AT;
	}
      /* Even on a big endian machine $fn comes before $fn+1.  We have
	 to adjust when loading from memory.  */
      macro_build (&icnt, &offset_expr, "lwc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg : treg + 1,
		   tempreg);
      offset_expr.X_add_number += 4;
      macro_build (&icnt, &offset_expr, "lwc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg + 1 : treg,
		   tempreg);
      if (tempreg == AT)
	break;
      return;

    case M_LD_OB:
      s = "lw";
      goto sd_ob;
    case M_SD_OB:
      s = "sw";
    sd_ob:
      macro_build (&icnt, &offset_expr, s, "t,o(b)", treg, breg);
      offset_expr.X_add_number = 4;
      macro_build (&icnt, &offset_expr, s, "t,o(b)", treg + 1, breg);
      return;

    case M_LD_AB:
      s = "lw";
      if (breg == treg)
	{
	  tempreg = AT;
	  used_at = 1;
	}
      else
	{
	  tempreg = treg;
	  used_at = 0;
	}
      goto sd_ab;
    case M_SD_AB:
      s = "sw";
      tempreg = AT;
      used_at = 1;
    sd_ab:
      if (gp_reference (&offset_expr))
	{
	  if (breg == 0)
	    {
	      tempreg = GP;
	      used_at = 0;
	    }
	  else
	    macro_build (&icnt, (expressionS *) NULL, "addu", "d,v,t",
			 tempreg, breg, GP);
	}
      else
	{
	  macro_build_lui (&icnt, &offset_expr, tempreg);
	  if (breg != 0)
	    macro_build (&icnt, NULL, "addu", "d,v,t", tempreg, tempreg, breg);
	}
      macro_build (&icnt, &offset_expr, s, "t,o(b)", treg, tempreg);
      offset_expr.X_add_number += 4;
      macro_build (&icnt, &offset_expr, s, "t,o(b)", treg + 1, tempreg);
      if (used_at)
	break;
      return;

    case M_MUL:
      macro_build (&icnt, NULL, "multu", "s,t", sreg, treg);
      macro_build (&icnt, NULL, "mflo", "d", dreg);
      /* two implicit nop's required for mflo */
      return;

    case M_MUL_I:
      /*
       * The mips assembler some times generates shifts and adds.
       * Im not trying to be that fancy. GCC should do this for us
       * anyway.
       */
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, NULL, "mult", "s,t", sreg, AT);
      macro_build (&icnt, NULL, "mflo", "d", dreg);
      /* two implicit nop's required for mflo */
      break;

    case M_ROL:
      macro_build (&icnt, NULL, "subu", "d,v,t", AT, 0, treg);
      macro_build (&icnt, NULL, "srlv", "d,t,s", AT, sreg, AT);
      macro_build (&icnt, NULL, "sllv", "d,t,s", dreg, sreg, treg);
      macro_build (&icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_ROL_I:
      macro_build (&icnt, NULL, "sll", "d,w,<", AT, sreg,
		   imm_expr.X_add_number & 0x1f);
      macro_build (&icnt, NULL, "srl", "d,w,<", dreg, sreg,
		   (0 - imm_expr.X_add_number) & 0x1f);
      macro_build (&icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_ROR:
      macro_build (&icnt, NULL, "subu", "d,v,t", AT, 0, treg);
      macro_build (&icnt, NULL, "sllv", "d,t,s", AT, sreg, AT);
      macro_build (&icnt, NULL, "srlv", "d,t,s", dreg, sreg, treg);
      macro_build (&icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_ROR_I:
      macro_build (&icnt, NULL, "srl", "d,w,<", AT, sreg,
		   imm_expr.X_add_number & 0x1f);
      macro_build (&icnt, NULL, "sll", "d,w,<", dreg, sreg,
		   (0 - imm_expr.X_add_number) & 0x1f);
      macro_build (&icnt, NULL, "or", "d,v,t", dreg, dreg, AT);
      break;

    case M_S_DOB:
      /* Even on a big endian machine $fn comes before $fn+1.  We have
	 to adjust when storing to memory.  */
      macro_build (&icnt, &offset_expr, "swc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg : treg + 1,
		   breg);
      offset_expr.X_add_number += 4;
      macro_build (&icnt, &offset_expr, "swc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg + 1 : treg,
		   breg);
      return;

    case M_S_DAB:
      if (gp_reference (&offset_expr))
	{
	  if (breg == 0)
	    tempreg = GP;
	  else
	    {
	      macro_build (&icnt, (expressionS *) NULL, "addu", "d,v,t",
			   AT, breg, GP);
	      tempreg = AT;
	    }
	}
      else
	{
	  macro_build_lui (&icnt, &offset_expr, AT);
	  if (breg != 0)
	    macro_build (&icnt, NULL, "addu", "d,v,t", AT, AT, breg);
	  tempreg = AT;
	}
      /* Even on a big endian machine $fn comes before $fn+1.  We have
	 to adjust when storing to memory.  */
      macro_build (&icnt, &offset_expr, "swc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg : treg + 1,
		   tempreg);
      offset_expr.X_add_number += 4;
      macro_build (&icnt, &offset_expr, "swc1", "T,o(b)",
		   byte_order == LITTLE_ENDIAN ? treg + 1 : treg,
		   tempreg);
      if (tempreg == AT)
	break;
      return;

    case M_SEQ:
      if (sreg == 0)
	macro_build (&icnt, &expr1, "sltiu", "t,r,j", dreg, treg);
      else if (treg == 0)
	macro_build (&icnt, &expr1, "sltiu", "t,r,j", dreg, sreg);
      else
	{
	  macro_build (&icnt, NULL, "xor", "d,v,t", dreg, sreg, treg);
	  macro_build (&icnt, &expr1, "sltiu", "t,r,j", dreg, dreg);
	}
      return;

    case M_SEQ_I:
      if (imm_expr.X_add_number == 0)
	{
	  macro_build (&icnt, &expr1, "sltiu", "t,r,j", dreg, sreg);
	  return;
	}
      if (sreg == 0)
	{
	  as_warn ("Instruction %s: result is always false",
	    ip->insn_mo->name);
	  macro_build (&icnt, NULL, "move", "d,s", dreg, 0);
	  return;
	}
      switch (imm_expr.X_add_number & 0xffff8000)
	{
	case 0:
	case 0x8000:
	  macro_build (&icnt, &imm_expr, "xori", "t,r,i", dreg, sreg);
	  used_at = 0;
	  break;

	case 0xffff8000:
	  if (imm_expr.X_add_number != -32768)
	    {
	      imm_expr.X_add_number = -imm_expr.X_add_number;
	      macro_build (&icnt, &imm_expr, "addiu", "t,r,j", dreg, sreg);
	      used_at = 0;
	      break;
	    }
	  /* FALLTHROUGH */

	default:
	  macro_build_lui (&icnt, &imm_expr, AT);
	  if (imm_expr.X_add_number & 0xffff)
	    macro_build (&icnt, &imm_expr, "addiu", "t,r,j", AT, AT);
	  macro_build (&icnt, NULL, "xor", "d,v,t", dreg, sreg, AT);
	  used_at = 1;
	}
      macro_build (&icnt, &expr1, "sltiu", "t,r,j", dreg, dreg);
      if (used_at)
	break;
      return;

    case M_SGE:		/* sreg >= treg <==> not (sreg < treg) */
      s = "slt";
      goto sge;
    case M_SGEU:
      s = "sltu";
    sge:
      macro_build (&icnt, NULL, s, "d,v,t", dreg, sreg, treg);
      macro_build (&icnt, &expr1, "xori", "t,r,i", dreg, dreg);
      return;

    case M_SGE_I:		/* sreg >= I <==> not (sreg < I) */
    case M_SGEU_I:
      if (imm_expr.X_add_number < 32768 && imm_expr.X_add_number > -32769)
	{
	  macro_build (&icnt, &expr1,
		   mask == M_SGE_I ? "slti" : "sltiu", "t,r,j", dreg, sreg);
	  used_at = 0;
	}
      else
	{
	  load_register (&icnt, ip, AT, &imm_expr);
	  macro_build (&icnt, NULL,
		 mask == M_SGE_I ? "slt" : "sltu", "d,v,t", dreg, sreg, AT);
	  used_at = 1;
	}
      macro_build (&icnt, &expr1, "xori", "t,r,i", dreg, dreg);
      if (used_at)
	break;
      return;

    case M_SGT:		/* sreg > treg  <==>  treg < sreg */
      s = "slt";
      goto sgt;
    case M_SGTU:
      s = "sltu";
    sgt:
      macro_build (&icnt, NULL, s, "d,v,t", dreg, treg, sreg);
      return;

    case M_SGT_I:		/* sreg > I  <==>  I < sreg */
      s = "slt";
      goto sgti;
    case M_SGTU_I:
      s = "sltu";
    sgti:
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, NULL, s, "d,v,t", dreg, AT, sreg);
      break;

    case M_SLE:		/* sreg <= treg  <==>  treg >= sreg  <==>  not (treg < sreg) */
      s = "slt";
      goto sle;
    case M_SLEU:
      s = "sltu";
    sle:
      macro_build (&icnt, NULL, s, "d,v,t", dreg, treg, sreg);
      macro_build (&icnt, &expr1, "xori", "t,r,i", dreg, dreg);
      return;

    case M_SLE_I:		/* sreg <= I <==> I >= sreg <==> not (I < sreg) */
      s = "slt";
      goto slei;
    case M_SLEU_I:
      s = "sltu";
    slei:
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, NULL, s, "d,v,t", dreg, AT, sreg);
      macro_build (&icnt, &expr1, "xori", "t,r,i", dreg, dreg);
      break;

    case M_SLT_I:
      if (imm_expr.X_add_number < 32768 && imm_expr.X_add_number > -32769)
	{
	  macro_build (&icnt, &imm_expr, "slti", "t,r,j", dreg, sreg);
	  return;
	}
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, NULL, "slt", "d,v,t", dreg, sreg, AT);
      break;

    case M_SLTU_I:
      if (imm_expr.X_add_number < 32768 && imm_expr.X_add_number > -32769)
	{
	  macro_build (&icnt, &imm_expr, "sltiu", "t,r,j", dreg, sreg);
	  return;
	}
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, NULL, "sltu", "d,v,t", dreg, sreg, AT);
      break;

    case M_SNE:
      if (sreg == 0)
	macro_build (&icnt, NULL, "sltu", "d,v,t", dreg, 0, treg);
      else if (treg == 0)
	macro_build (&icnt, NULL, "sltu", "d,v,t", dreg, 0, sreg);
      else
	{
	  macro_build (&icnt, NULL, "xor", "d,v,t", dreg, sreg, treg);
	  macro_build (&icnt, NULL, "sltu", "d,v,t", dreg, 0, dreg);
	}
      return;

    case M_SNE_I:
      if (imm_expr.X_add_number == 0)
	{
	  macro_build (&icnt, NULL, "sltu", "d,v,t", dreg, 0, sreg);
	  return;
	}
      if (sreg == 0)
	{
	  as_warn ("Instruction %s: result is always true",
	    ip->insn_mo->name);
	  macro_build (&icnt, &expr1, "addiu", "t,r,j", dreg, 0);
	  return;
	}
      switch (imm_expr.X_add_number & 0xffff8000)
	{
	case 0:
	case 0x8000:
	  macro_build (&icnt, &imm_expr, "xori", "t,r,i", dreg, sreg);
	  used_at = 0;
	  break;

	case 0xffff8000:
	  if (imm_expr.X_add_number != -32768)
	    {
	      imm_expr.X_add_number = -imm_expr.X_add_number;
	      macro_build (&icnt, &imm_expr, "addiu", "t,r,j", dreg, sreg);
	      used_at = 0;
	      break;
	    }
	  /* FALLTHROUGH */

	default:
	  macro_build_lui (&icnt, &imm_expr, AT);
	  if (imm_expr.X_add_number & 0xffff)
	    macro_build (&icnt, &imm_expr, "addiu", "t,r,j", AT, AT);
	  macro_build (&icnt, NULL, "xor", "d,v,t", dreg, sreg, AT);
	  used_at = 1;
	}
      macro_build (&icnt, NULL, "sltu", "d,v,t", dreg, 0, dreg);
      if (used_at)
	break;
      return;

    case M_SUB_I:
      if (imm_expr.X_add_number < 32768 && imm_expr.X_add_number > -32768)
	{
	  imm_expr.X_add_number = -imm_expr.X_add_number;
	  macro_build (&icnt, &imm_expr, "addi", "t,r,j", dreg, sreg);
	  return;
	}
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, NULL, "sub", "d,v,t", dreg, sreg, AT);
      break;

    case M_SUBU_I:
      if (imm_expr.X_add_number < 32768 && imm_expr.X_add_number > -32768)
	{
	  imm_expr.X_add_number = -imm_expr.X_add_number;
	  macro_build (&icnt, &imm_expr, "addiu", "t,r,j", dreg, sreg);
	  return;
	}
      load_register (&icnt, ip, AT, &imm_expr);
      macro_build (&icnt, NULL, "subu", "d,v,t", dreg, sreg, AT);
      break;

    case M_TRUNCWD:
    case M_TRUNCWS:
      sreg = (ip->insn_opcode >> 11) & 0x1f;	/* floating reg */
      dreg = (ip->insn_opcode >> 06) & 0x1f;	/* floating reg */

      /*
       * Is the double cfc1 instruction a bug in the mips assembler;
       * or is there a reason for it?
       */
      mips_emit_delays ();
      ++mips_noreorder;
      macro_build (&icnt, NULL, "cfc1", "t,G", treg, 31);
      macro_build (&icnt, NULL, "cfc1", "t,G", treg, 31);
      macro_build (&icnt, NULL, "nop", "");
      expr1.X_add_number = 3;
      macro_build (&icnt, &expr1, "ori", "t,r,i", AT, treg);
      expr1.X_add_number = 2;
      macro_build (&icnt, &expr1, "xori", "t,r,i", AT, AT);
      macro_build (&icnt, NULL, "ctc1", "t,G", AT, 31);
      macro_build (&icnt, NULL, "nop", "");
      macro_build (&icnt, NULL,
	      mask == M_TRUNCWD ? "cvt.w.d" : "cvt.w.s", "D,S", dreg, sreg);
      macro_build (&icnt, NULL, "ctc1", "t,G", treg, 31);
      macro_build (&icnt, NULL, "nop", "");
      --mips_noreorder;
      break;

    case M_ULH:
      s = "lb";
      goto ulh;
    case M_ULHU:
      s = "lbu";
    ulh:
      /* avoid load delay */
      offset_expr.X_add_number += 1;
      macro_build (&icnt, &offset_expr, s, "t,o(b)", treg, breg);
      offset_expr.X_add_number -= 1;
      macro_build (&icnt, &offset_expr, "lbu", "t,o(b)", AT, breg);
      macro_build (&icnt, NULL, "sll", "d,w,<", treg, treg, 8);
      macro_build (&icnt, NULL, "or", "d,v,t", treg, treg, AT);
      break;

    case M_ULW:
      /* does this work on a big endian machine? */
      offset_expr.X_add_number += 3;
      macro_build (&icnt, &offset_expr, "lwl", "t,o(b)", treg, breg);
      offset_expr.X_add_number -= 3;
      macro_build (&icnt, &offset_expr, "lwr", "t,o(b)", treg, breg);
      return;

    case M_ULH_A:
    case M_ULHU_A:
    case M_ULW_A:
      if (offset_expr.X_op == O_constant)
	load_register (&icnt, ip, AT, &offset_expr);
      else if (gp_reference (&offset_expr))
	macro_build (&icnt, &offset_expr, "addiu", "t,r,j", AT, GP);
      else
	{
	  macro_build_lui (&icnt, &offset_expr, AT);
	  macro_build (&icnt, &offset_expr, "addiu", "t,r,j", AT, AT);
	}
      if (mask == M_ULW_A)
	{
	  expr1.X_add_number = 3;
	  macro_build (&icnt, &expr1, "lwl", "t,o(b)", treg, AT);
	  imm_expr.X_add_number = 0;
	  macro_build (&icnt, &expr1, "lwr", "t,o(b)", treg, AT);
	}
      else
	{
	  macro_build (&icnt, &expr1,
		       mask == M_ULH_A ? "lb" : "lbu", "t,o(b)", treg, AT);
	  imm_expr.X_add_number = 0;
	  macro_build (&icnt, &expr1, "lbu", "t,o(b)", AT, AT);
	  macro_build (&icnt, NULL, "sll", "d,w,<", treg, treg, 8);
	  macro_build (&icnt, NULL, "or", "d,v,t", treg, treg, AT);
	}
      break;

    case M_USH:
      macro_build (&icnt, &offset_expr, "sb", "t,o(b)", treg, breg);
      macro_build (&icnt, NULL, "srl", "d,w,<", AT, treg, 8);
      offset_expr.X_add_number += 1;
      macro_build (&icnt, &offset_expr, "sb", "t,o(b)", AT, breg);
      break;

    case M_USW:
      offset_expr.X_add_number += 3;
      macro_build (&icnt, &offset_expr, "swl", "t,o(b)", treg, breg);
      offset_expr.X_add_number -= 3;
      macro_build (&icnt, &offset_expr, "swr", "t,o(b)", treg, breg);
      return;

    case M_USH_A:
    case M_USW_A:
      if (offset_expr.X_op == O_constant)
	load_register (&icnt, ip, AT, &offset_expr);
      else if (gp_reference (&offset_expr))
	macro_build (&icnt, &offset_expr, "addiu", "t,r,j", AT, GP);
      else
	{
	  macro_build_lui (&icnt, &offset_expr, AT);
	  macro_build (&icnt, &offset_expr, "addiu", "t,r,j", AT, AT);
	}
      if (mask == M_USW_A)
	{
	  expr1.X_add_number = 3;
	  macro_build (&icnt, &expr1, "swl", "t,o(b)", treg, AT);
	  expr1.X_add_number = 0;
	  macro_build (&icnt, &expr1, "swr", "t,o(b)", treg, AT);
	}
      else
	{
	  expr1.X_add_number = 0;
	  macro_build (&icnt, &expr1, "sb", "t,o(b)", treg, AT);
	  macro_build (&icnt, NULL, "srl", "d,w,<", treg, treg, 8);
	  expr1.X_add_number = 1;
	  macro_build (&icnt, &expr1, "sb", "t,o(b)", treg, AT);
	  expr1.X_add_number = 0;
	  macro_build (&icnt, &expr1, "lbu", "t,o(b)", AT, AT);
	  macro_build (&icnt, NULL, "sll", "d,w,<", treg, treg, 8);
	  macro_build (&icnt, NULL, "or", "d,v,t", treg, treg, AT);
	}
      break;

    default:
      as_bad ("Macro %s not implemented yet", ip->insn_mo->name);
    }
  if (mips_noat)
    as_warn ("Macro used $at after \".set noat\"");
}


/*
This routine assembles an instruction into its binary format.  As a side
effect it sets one of the global variables imm_reloc or offset_reloc to the
type of relocation to do if one of the operands is an address expression.
*/
static void
mips_ip (str, ip)
     char *str;
     struct mips_cl_insn *ip;
{
  char *s;
  const char *args;
  char c;
  struct mips_opcode *insn;
  char *argsStart;
  unsigned int regno;
  unsigned int lastregno = 0;
  char *s_reset;

  insn_error = NULL;

  for (s = str; islower (*s) || (*s >= '0' && *s <= '3') || *s == '.'; ++s)
    continue;
  switch (*s)
    {
    case '\0':
      break;

    case ' ':
      *s++ = '\0';
      break;

    default:
      as_warn ("Unknown opcode: `%s'", str);
      exit (1);
    }
  if ((insn = (struct mips_opcode *) hash_find (op_hash, str)) == NULL)
    {
      as_warn ("`%s' not in hash table.", str);
      insn_error = "ERROR: Unrecognized opcode";
      return;
    }
  argsStart = s;
  for (;;)
    {
      assert (strcmp (insn->name, str) == 0);
      ip->insn_mo = insn;
      ip->insn_opcode = insn->match;
      for (args = insn->args;; ++args)
	{
	  if (*s == ' ')
	    ++s;
	  switch (*args)
	    {
	    case '\0':		/* end of args */
	      if (*s == '\0')
		return;
	      break;

	    case ',':
	      if (*s++ == *args)
		continue;
	      s--;
	      switch (*++args)
		{
		case 'r':
		case 'v':
		  ip->insn_opcode |= lastregno << 21;
		  continue;

		case 'w':
		case 'W':
		  ip->insn_opcode |= lastregno << 16;
		  continue;

		case 'V':
		  ip->insn_opcode |= lastregno << 11;
		  continue;
		}
	      break;

	    case '(':
	      /* handle optional base register.
		 Either the base register is omitted or
		 we must have a left paren. */
	      /* this is dependent on the next operand specifier
		 is a 'b' for base register */
	      assert (args[1] == 'b');
	      if (*s == '\0')
		return;

	    case ')':		/* these must match exactly */
	      if (*s++ == *args)
		continue;
	      break;

	    case '<':		/* must be at least one digit */
	      /*
	       * According to the manual, if the shift amount is greater
	       * than 31 or less than 0 the the shift amount should be
	       * mod 32. In reality the mips assembler issues an error.
	       * We issue a warning and do the mod.
	       */
	      my_getExpression (&imm_expr, s);
	      check_absolute_expr (ip, &imm_expr);
	      if ((unsigned long) imm_expr.X_add_number > 31)
		{
		  as_warn ("Improper shift amount (%d)",
			   imm_expr.X_add_number);
		  imm_expr.X_add_number = imm_expr.X_add_number % 32;
		}
	      ip->insn_opcode |= imm_expr.X_add_number << 6;
	      imm_expr.X_op = O_absent;
	      s = expr_end;
	      continue;

	    case 'c':		/* break code */
	      my_getExpression (&imm_expr, s);
	      check_absolute_expr (ip, &imm_expr);
	      if ((unsigned) imm_expr.X_add_number > 1023)
		as_warn ("Illegal break code (%d)", imm_expr.X_add_number);
	      ip->insn_opcode |= imm_expr.X_add_number << 16;
	      imm_expr.X_op = O_absent;
	      s = expr_end;
	      continue;

	    case 'B':		/* syscall code */
	      my_getExpression (&imm_expr, s);
	      check_absolute_expr (ip, &imm_expr);
	      if ((unsigned) imm_expr.X_add_number > 0xfffff)
		as_warn ("Illegal syscall code (%d)", imm_expr.X_add_number);
	      ip->insn_opcode |= imm_expr.X_add_number << 6;
	      imm_expr.X_op = O_absent;
	      s = expr_end;
	      continue;

            case 'C':           /* Coprocessor code */
              my_getExpression (&imm_expr, s);
	      check_absolute_expr (ip, &imm_expr);
              if ((unsigned long) imm_expr.X_add_number >= (1<<25))
		{
                  as_warn ("Coproccesor code > 25 bits (%d)",
			   imm_expr.X_add_number);
                  imm_expr.X_add_number &= ((1<<25) - 1);
		}
              ip->insn_opcode |= imm_expr.X_add_number;
              imm_expr.X_op = O_absent;
              s = expr_end;
              continue;

	    case 'b':		/* base register */
	    case 'd':		/* destination register */
	    case 's':		/* source register */
	    case 't':		/* target register */
	    case 'r':		/* both target and source */
	    case 'v':		/* both dest and source */
	    case 'w':		/* both dest and target */
	    case 'E':		/* coprocessor target register */
	    case 'G':		/* coprocessor destination register */
	      s_reset = s;
	      if (s[0] == '$')
		{
		  if (isdigit (s[1]))
		    {
		      ++s;
		      regno = 0;
		      do
			{
			  regno *= 10;
			  regno += *s - '0';
			  ++s;
			}
		      while (isdigit (*s));
		      if (regno > 31)
			as_bad ("Invalid register number (%d)", regno);
		    }
		  else if (*args != 'E' && *args != 'G')
		    {
		      if (s[1] == 'f' && s[2] == 'p')
			{
			  s += 3;
			  regno = 30;
			}
		      else if (s[1] == 's' && s[2] == 'p')
			{
			  s += 3;
			  regno = 29;
			}
		      else if (s[1] == 'g' && s[2] == 'p')
			{
			  s += 3;
			  regno = 28;
			}
		      else if (s[1] == 'a' && s[2] == 't')
			{
			  s += 3;
			  regno = 1;
			}
		      else
			goto notreg;
		      if (regno == AT && ! mips_noat)
			as_warn ("Used $at without \".set noat\"");
		    }
		  c = *args;
		  if (*s == ' ')
		    s++;
		  if (args[1] != *s)
		    {
		      if (c == 'r' || c == 'v' || c == 'w')
			{
			  regno = lastregno;
			  s = s_reset;
			  args++;
			}
		    }
		  switch (c)
		    {
		    case 'r':
		    case 's':
		    case 'v':
		    case 'b':
		      ip->insn_opcode |= regno << 21;
		      break;
		    case 'd':
		    case 'G':
		      ip->insn_opcode |= regno << 11;
		      break;
		    case 'w':
		    case 't':
		    case 'E':
		      ip->insn_opcode |= regno << 16;
		    }
		  lastregno = regno;
		  continue;
		}
	    notreg:
	      switch (*args++)
		{
		case 'r':
		case 'v':
		  ip->insn_opcode |= lastregno << 21;
		  continue;
		case 'w':
		  ip->insn_opcode |= lastregno << 16;
		  continue;
		}
	      break;

	    case 'D':		/* floating point destination register */
	    case 'S':		/* floating point source register */
	    case 'T':		/* floating point target register */
	    case 'V':
	    case 'W':
	      s_reset = s;
	      if (s[0] == '$' && s[1] == 'f' && isdigit (s[2]))
		{
		  s += 2;
		  regno = 0;
		  do
		    {
		      regno *= 10;
		      regno += *s - '0';
		      ++s;
		    }
		  while (isdigit (*s));

		  if (regno > 31)
		    as_bad ("Invalid float register number (%d)", regno);

		  if ((regno & 1) &&
		      !(strcmp (str, "mtc1") == 0 ||
			strcmp (str, "mfc1") == 0 ||
			strcmp (str, "lwc1") == 0 ||
			strcmp (str, "swc1") == 0))
		    as_warn ("Float register should be even, was %d",
			     regno);

		  c = *args;
		  if (*s == ' ')
		    s++;
		  if (args[1] != *s)
		    {
		      if (c == 'V' || c == 'W')
			{
			  regno = lastregno;
			  s = s_reset;
			  args++;
			}
		    }
		  switch (c)
		    {
		    case 'D':
		      ip->insn_opcode |= regno << 6;
		      break;
		    case 'V':
		    case 'S':
		      ip->insn_opcode |= regno << 11;
		      break;
		    case 'W':
		    case 'T':
		      ip->insn_opcode |= regno << 16;
		    }
		  lastregno = regno;
		  continue;
		}
	      switch (*args++)
		{
		case 'V':
		  ip->insn_opcode |= lastregno << 11;
		  continue;
		case 'W':
		  ip->insn_opcode |= lastregno << 16;
		  continue;
		}
	      break;

	    case 'I':
	      my_getExpression (&imm_expr, s);
	      check_absolute_expr (ip, &imm_expr);
	      s = expr_end;
	      continue;

	    case 'A':
	      my_getExpression (&offset_expr, s);
	      imm_reloc = BFD_RELOC_32;
	      s = expr_end;
	      continue;

	    case 'F':
	    case 'L':
	    case 'f':
	    case 'l':
	      {
		int f64;
		char *save_in;
		char *err;
		unsigned char temp[8];
		int length;
		segT seg;
		subsegT subseg;
		char *p;

		/* These only appear as the last operand in an
		   instruction, and every instruction that accepts
		   them in any variant accepts them in all variants.
		   This means we don't have to worry about backing out
		   any changes if the instruction does not match.

		   The difference between them is the size of the
		   floating point constant and where it goes.  For 'F'
		   and 'L' the constant is 64 bits; for 'f' and 'l' it
		   is 32 bits.  Where the constant is placed is based
		   on how the MIPS assembler does things:
		    F -- .rdata
		    L -- .lit8
		    f -- immediate value
		    l -- .lit4
		   */

		f64 = *args == 'F' || *args == 'L';

		save_in = input_line_pointer;
		input_line_pointer = s;
		err = md_atof (f64 ? 'd' : 'f', (char *) temp, &length);
		s = input_line_pointer;
		input_line_pointer = save_in;
		if (err != NULL && *err != '\0')
		  {
		    as_bad ("Bad floating point constant: %s", err);
		    memset (temp, '\0', sizeof temp);
		    length = f64 ? 8 : 4;
		  }

		assert (length == (f64 ? 8 : 4));

		if (*args == 'f')
		  {
		    imm_expr.X_op = O_constant;
		    if (byte_order == LITTLE_ENDIAN)
		      imm_expr.X_add_number =
			(((((((int) temp[3] << 8)
			     | temp[2]) << 8)
			   | temp[1]) << 8)
			 | temp[0]);
		    else
		      imm_expr.X_add_number =
			(((((((int) temp[0] << 8)
			     | temp[1]) << 8)
			   | temp[2]) << 8)
			 | temp[3]);
		  }
		else
		  {
		    /* Switch to the right section.  */
		    seg = now_seg;
		    subseg = now_subseg;
		    switch (*args)
		      {
		      case 'F':
			subseg_new (".rdata", (subsegT) 0);
			break;
		      case 'L':
			subseg_new (".lit8", (subsegT) 0);
			break;
		      case 'l':
			subseg_new (".lit4", (subsegT) 0);
			break;
		      }
		    if (seg == now_seg)
		      as_bad ("Can't use floating point insn in this section");

		    /* Set the argument to the current address in the
		       .rdata section.  */
		    offset_expr.X_op = O_symbol;
		    offset_expr.X_add_symbol =
		      symbol_new ("L0\001", now_seg,
				  (valueT) frag_now_fix (), frag_now);
		    offset_expr.X_add_number = 0;

		    /* Put the floating point number into the section.  */
		    p = frag_more (length);
		    memcpy (p, temp, length);

		    /* Switch back to the original section.  */
		    subseg_set (seg, subseg);
		  }
	      }
	      continue;

	    case 'i':		/* 16 bit unsigned immediate */
	    case 'j':		/* 16 bit signed immediate */
	      imm_reloc = BFD_RELOC_LO16;
	      c = my_getSmallExpression (&imm_expr, s);
	      if (c)
		{
		  if (c != 'l')
		    {
		      if (imm_expr.X_op == O_constant)
			imm_expr.X_add_number =
			  (imm_expr.X_add_number >> 16) & 0xffff;
		      else if (c == 'h')
			imm_reloc = BFD_RELOC_HI16_S;
		      else
			imm_reloc = BFD_RELOC_HI16;
		    }
		}
	      else
		check_absolute_expr (ip, &imm_expr);
	      if (*args == 'i')
		{
		  if ((unsigned long) imm_expr.X_add_number > 65535)
		    {
		      if (insn + 1 < &mips_opcodes[NUMOPCODES] &&
			  !strcmp (insn->name, insn[1].name))
			break;
		      as_bad ("16 bit expression not in range 0..65535");
		    }
		}
	      else
		{
		  if (imm_expr.X_add_number < -32768 ||
		      imm_expr.X_add_number > 32767)
		    {
		      if (insn + 1 < &mips_opcodes[NUMOPCODES] &&
			  !strcmp (insn->name, insn[1].name))
			break;
		      as_bad ("16 bit expression not in range -32768..32767");
		    }
		}
	      s = expr_end;
	      continue;

	    case 'o':		/* 16 bit offset */
	      c = my_getSmallExpression (&offset_expr, s);
	      /*
	       * If this value won't fit into a 16 bit offset, then
	       * go find a macro that will generate the 32 bit offset
	       * code pattern.
	       */
	      if ((offset_expr.X_add_symbol
		   && offset_expr.X_op != O_constant)
		  || offset_expr.X_op_symbol
		  || offset_expr.X_add_number > 32767
		  || offset_expr.X_add_number < -32768)
		break;

	      offset_reloc = BFD_RELOC_LO16;
	      if (c == 'h' || c == 'H')
		offset_expr.X_add_number =
		  (offset_expr.X_add_number >> 16) & 0xffff;
	      s = expr_end;
	      continue;

	    case 'p':		/* pc relative offset */
	      offset_reloc = BFD_RELOC_16_PCREL_S2;
	      my_getExpression (&offset_expr, s);
	      s = expr_end;
	      continue;

	    case 'u':		/* upper 16 bits */
	      c = my_getSmallExpression (&imm_expr, s);
	      if ((unsigned long) imm_expr.X_add_number > 65535)
		as_bad ("lui expression not in range 0..65535");
	      imm_reloc = BFD_RELOC_LO16;
	      if (c)
		{
		  if (c != 'l')
		    {
		      if (imm_expr.X_op == O_constant)
			imm_expr.X_add_number =
			  (imm_expr.X_add_number >> 16) & 0xffff;
		      else if (c == 'h')
			imm_reloc = BFD_RELOC_HI16_S;
		      else
			imm_reloc = BFD_RELOC_HI16;
		    }
		}
	      s = expr_end;
	      continue;

	    case 'a':		/* 26 bit address */
	      my_getExpression (&offset_expr, s);
	      s = expr_end;
	      offset_reloc = BFD_RELOC_MIPS_JMP;
	      continue;

	    default:
	      fprintf (stderr, "bad char = '%c'\n", *args);
	      internalError ();
	    }
	  break;
	}
      /* Args don't match.  */
      if (insn + 1 < &mips_opcodes[NUMOPCODES] &&
	  !strcmp (insn->name, insn[1].name))
	{
	  ++insn;
	  s = argsStart;
	  continue;
	}
      insn_error = "ERROR: Illegal operands";
      return;
    }
}

#define LP '('
#define RP ')'

static int
my_getSmallExpression (ep, str)
     expressionS *ep;
     char *str;
{
  char *sp;
  int c = 0;

  if (*str == ' ')
    str++;
  if (*str == LP
      || (*str == '%' &&
	  ((str[1] == 'h' && str[2] == 'i')
	   || (str[1] == 'H' && str[2] == 'I')
	   || (str[1] == 'l' && str[2] == 'o'))
	  && str[3] == LP))
    {
      if (*str == LP)
	c = 0;
      else
	{
	  c = str[1];
	  str += 3;
	}

      /*
       * A small expression may be followed by a base register.
       * Scan to the end of this operand, and then back over a possible
       * base register.  Then scan the small expression up to that
       * point.  (Based on code in sparc.c...)
       */
      for (sp = str; *sp && *sp != ','; sp++)
	;
      if (sp - 4 >= str && sp[-1] == RP)
	{
	  if (isdigit (sp[-2]))
	    {
	      for (sp -= 3; sp >= str && isdigit (*sp); sp--)
		;
	      if (*sp == '$' && sp > str && sp[-1] == LP)
		{
		  sp--;
		  goto do_it;
		}
	    }
	  else if (sp - 5 >= str
		   && sp[-5] == LP
		   && sp[-4] == '$'
		   && ((sp[-3] == 'f' && sp[-2] == 'p')
		       || (sp[-3] == 's' && sp[-2] == 'p')
		       || (sp[-3] == 'g' && sp[-2] == 'p')
		       || (sp[-3] == 'a' && sp[-2] == 't')))
	    {
	      sp -= 5;
	    do_it:
	      if (sp == str)
		{
		  /* no expression means zero offset */
		  if (c)
		    {
		      /* %xx(reg) is an error */
		      ep->X_op = O_absent;
		      expr_end = str - 3;
		    }
		  else
		    {
		      ep->X_op = O_absent;
		      expr_end = sp;
		    }
		  ep->X_add_symbol = NULL;
		  ep->X_op_symbol = NULL;
		  ep->X_add_number = 0;
		}
	      else
		{
		  *sp = '\0';
		  my_getExpression (ep, str);
		  *sp = LP;
		}
	      return c;
	    }
	}
    }
  my_getExpression (ep, str);
  return c;			/* => %hi or %lo encountered */
}

static void
my_getExpression (ep, str)
     expressionS *ep;
     char *str;
{
  char *save_in;

  save_in = input_line_pointer;
  input_line_pointer = str;
  expression (ep);
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *litP.  The number
   of LITTLENUMS emitted is stored in *sizeP .  An error message is
   returned, or NULL on OK.  */

char *
md_atof (type, litP, sizeP)
     int type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizeP = 0;
      return "bad call to md_atof";
    }

  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizeP = prec * 2;

  if (byte_order == LITTLE_ENDIAN)
    {
      for (i = prec - 1; i >= 0; i--)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }
  else
    {
      for (i = 0; i < prec; i++)
	{
	  md_number_to_chars (litP, (valueT) words[i], 2);
	  litP += 2;
	}
    }
     
  return NULL;
}

void
md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
  switch (byte_order)
    {
    case LITTLE_ENDIAN:
      switch (n)
	{
	case 4:
	  *buf++ = val;
	  *buf++ = val >> 8;
	  *buf++ = val >> 16;
	  *buf = val >> 24;
	  return;

	case 2:
	  *buf++ = val;
	  *buf = val >> 8;
	  return;

	case 1:
	  *buf = val;
	  return;

	default:
	  internalError ();
	}

    case BIG_ENDIAN:
      switch (n)
	{
	case 4:
	  *buf++ = val >> 24;
	  *buf++ = val >> 16;
	case 2:
	  *buf++ = val >> 8;
	case 1:
	  *buf = val;
	  return;

	default:
	  internalError ();
	}

    default:
      internalError ();
    }
}

int
md_parse_option (argP, cntP, vecP)
     char **argP;
     int *cntP;
     char ***vecP;
{
  /* Accept -nocpp but ignore it. */
  if (!strcmp (*argP, "nocpp"))
    {
      *argP += 5;
      return 1;
    }

  if (strcmp (*argP, "EL") == 0
      || strcmp (*argP, "EB") == 0)
    {
      /* FIXME: This breaks -L -EL.  */
      flagseen['L'] = 0;
      *argP = "";
      return 1;
    }

  if (**argP == 'O')
    {
      if ((*argP)[1] == '0')
	mips_optimize = 1;
      else
	mips_optimize = 2;
      return 1;
    }

  if (**argP == 'g')
    {
      if ((*argP)[1] == '\0' || (*argP)[1] == '2')
	mips_optimize = 0;
      return 1;
    }

#ifdef OBJ_ECOFF
  if (**argP == 'G')
    {
      if ((*argP)[1] != '\0')
	g_switch_value = atoi (*argP + 1);
      else if (*cntP)
	{
	  **vecP = (char *) NULL;
	  (*cntP)--;
	  (*vecP)++;
	  g_switch_value = atoi (**vecP);
	}
      else
	as_warn ("Number expected after -G");
      *argP = "";
      return 1;
    }
#endif

  return 1;			/* pretend you parsed the character */
}

long
md_pcrel_from (fixP)
     fixS *fixP;
{
  /* return the address of the delay slot */
  return fixP->fx_size + fixP->fx_where + fixP->fx_frag->fr_address;
}

int
md_apply_fix (fixP, valueP)
     fixS *fixP;
     valueT *valueP;
{
  unsigned char *buf;
  long insn, value;

  assert (fixP->fx_size == 4);

  value = *valueP;
  fixP->fx_addnumber = value;	/* Remember value for tc_gen_reloc */

  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_32:
    case BFD_RELOC_MIPS_JMP:
    case BFD_RELOC_HI16:
    case BFD_RELOC_HI16_S:
    case BFD_RELOC_LO16:
    case BFD_RELOC_MIPS_GPREL:
      /* Nothing needed to do. The value comes from the reloc entry */
      return 1;

    case BFD_RELOC_16_PCREL_S2:
      /*
       * We need to save the bits in the instruction since fixup_segment()
       * might be deleting the relocation entry (i.e., a branch within
       * the current segment).
       */
      if (value & 0x3)
	as_warn ("Branch to odd address (%x)", value);
      value >>= 2;
      if ((value & ~0xFFFF) && (value & ~0xFFFF) != (-1 & ~0xFFFF))
	as_bad ("Relocation overflow");

      /* update old instruction data */
      buf = (unsigned char *) (fixP->fx_where + fixP->fx_frag->fr_literal);
      switch (byte_order)
	{
	case LITTLE_ENDIAN:
	  insn = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
	  break;

	case BIG_ENDIAN:
	  insn = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
	  break;

	default:
	  internalError ();
	  return 0;
	}
      insn |= value & 0xFFFF;
      md_number_to_chars ((char *) buf, insn, 4);
      break;

    default:
      internalError ();
    }
  return 1;
}

#if 0
void
printInsn (oc)
     unsigned long oc;
{
  const struct mips_opcode *p;
  int treg, sreg, dreg, shamt;
  short imm;
  const char *args;
  int i;

  for (i = 0; i < NUMOPCODES; ++i)
    {
      p = &mips_opcodes[i];
      if (((oc & p->mask) == p->match) && (p->pinfo != INSN_MACRO))
	{
	  printf ("%08lx %s\t", oc, p->name);
	  treg = (oc >> 16) & 0x1f;
	  sreg = (oc >> 21) & 0x1f;
	  dreg = (oc >> 11) & 0x1f;
	  shamt = (oc >> 6) & 0x1f;
	  imm = oc;
	  for (args = p->args;; ++args)
	    {
	      switch (*args)
		{
		case '\0':
		  printf ("\n");
		  break;

		case ',':
		case '(':
		case ')':
		  printf ("%c", *args);
		  continue;

		case 'r':
		  assert (treg == sreg);
		  printf ("$%d,$%d", treg, sreg);
		  continue;

		case 'd':
		case 'G':
		  printf ("$%d", dreg);
		  continue;

		case 't':
		case 'E':
		  printf ("$%d", treg);
		  continue;

		case 'b':
		case 's':
		  printf ("$%d", sreg);
		  continue;

		case 'a':
		  printf ("0x%08lx", oc & 0x1ffffff);
		  continue;

		case 'i':
		case 'j':
		case 'o':
		case 'u':
		  printf ("%d", imm);
		  continue;

		case '<':
		  printf ("$%d", shamt);
		  continue;

		default:
		  internalError ();
		}
	      break;
	    }
	  return;
	}
    }
  printf ("%08lx  UNDEFINED\n", oc);
}
#endif

static symbolS *
get_symbol ()
{
  int c;
  char *name;
  symbolS *p;

  name = input_line_pointer;
  c = get_symbol_end ();
  p = (symbolS *) symbol_find_or_make (name);
  *input_line_pointer = c;
  return p;
}

/* Align the current frag to a given power of two.  The MIPS assembler
   also automatically adjusts any preceding label.  */

static void
mips_align (to, fill)
     int to;
     int fill;
{
  mips_emit_delays ();
  frag_align (to, fill);
  record_alignment (now_seg, to);
  if (insn_label != NULL)
    {
      assert (S_GET_SEGMENT (insn_label) == now_seg);
      insn_label->sy_frag = frag_now;
      S_SET_VALUE (insn_label, frag_now_fix ());
    }
}

/* Align to a given power of two.  .align 0 turns off the automatic
   alignment used by the data creating pseudo-ops.  */

static void
s_align (x)
     int x;
{
  register int temp;
  register long temp_fill;
  long max_alignment = 15;

  /*

    o  Note that the assembler pulls down any immediately preceeding label
       to the aligned address.
    o  It's not documented but auto alignment is reinstated by
       a .align pseudo instruction.
    o  Note also that after auto alignment is turned off the mips assembler
       issues an error on attempt to assemble an improperly aligned data item.
       We don't.

    */

  temp = get_absolute_expression ();
  if (temp > max_alignment)
    as_bad ("Alignment too large: %d. assumed.", temp = max_alignment);
  else if (temp < 0)
    {
      as_warn ("Alignment negative: 0 assumed.");
      temp = 0;
    }
  if (*input_line_pointer == ',')
    {
      input_line_pointer++;
      temp_fill = get_absolute_expression ();
    }
  else
    temp_fill = 0;
  if (temp)
    {
      auto_align = 1;
      mips_align (temp, (int) temp_fill);
    }
  else
    {
      auto_align = 0;
    }

  demand_empty_rest_of_line ();
}

/* Handle .ascii and .asciiz.  This just calls stringer and forgets
   that there was a previous instruction.  */

static void
s_stringer (append_zero)
     int append_zero;
{
  mips_emit_delays ();
  stringer (append_zero);
}

static void
s_change_sec (sec)
     int sec;
{
  segT segment;

  mips_emit_delays ();
  segment = now_seg;
  switch (sec)
    {
    case 't':
      s_text ();
      break;
    case 'r':
#ifdef OBJ_ECOFF
      subseg_new (".rdata", (subsegT) get_absolute_expression ());
      demand_empty_rest_of_line ();
      break;
#else
      /* Fall through.  */
#endif
    case 'd':
      s_data ();
      break;
    case 'b':
#ifdef BFD_ASSEMBLER
      subseg_set (bss_section, (subsegT) get_absolute_expression ());
#else
      subseg_new (bss_section, (subsegT) get_absolute_expression ());
#endif
      demand_empty_rest_of_line ();
      break;
    case 's':
#ifdef OBJ_ECOFF
      subseg_new (".sdata", (subsegT) get_absolute_expression ());
      demand_empty_rest_of_line ();
      break;
#else
      as_bad ("Global pointers not supported; recompile -G 0");
      demand_empty_rest_of_line ();
      return;
#endif
    }
  auto_align = 1;
}

static void
s_cons (log_size)
     int log_size;
{
  mips_emit_delays ();
  if (log_size > 0 && auto_align)
    mips_align (log_size, 0);
  cons (1 << log_size);
}

static void
s_err (x)
     int x;
{
  as_fatal ("Encountered `.err', aborting assembly");
}

static void
s_extern (x)
     int x;
{
  long size;
  symbolS *symbolP;

  symbolP = get_symbol ();
  if (*input_line_pointer == ',')
    input_line_pointer++;
  size = get_absolute_expression ();
  S_SET_VALUE (symbolP, size);
  S_SET_EXTERNAL (symbolP);

#ifdef OBJ_ECOFF
  /* ECOFF needs to distinguish a .comm symbol from a .extern symbol,
     so we use an additional ECOFF specific field.  */
  symbolP->ecoff_undefined = 1;
#endif
}

static void
s_float_cons (type)
     int type;
{
  mips_emit_delays ();

  if (auto_align)
    if (type == 'd')
      mips_align (3, 0);
    else
      mips_align (2, 0);

  float_cons (type);
}

static void
s_option (x)
     int x;
{
  if (strcmp (input_line_pointer, "O1") != 0
      && strcmp (input_line_pointer, "O2") != 0)
    as_warn ("Unrecognized option");
  demand_empty_rest_of_line ();
}

static void
s_mipsset (x)
     int x;
{
  char *name = input_line_pointer, ch;

  while (!is_end_of_line[(unsigned char) *input_line_pointer])
    input_line_pointer++;
  ch = *input_line_pointer;
  *input_line_pointer = '\0';

  if (strcmp (name, "reorder") == 0)
    {
      if (mips_noreorder)
	{
	  prev_insn_unreordered = 1;
	  prev_prev_insn_unreordered = 1;
	}
      mips_noreorder = 0;
    }
  else if (strcmp (name, "noreorder") == 0)
    {
      mips_emit_delays ();
      mips_noreorder = 1;
    }
  else if (strcmp (name, "at") == 0)
    {
      mips_noat = 0;
    }
  else if (strcmp (name, "noat") == 0)
    {
      mips_noat = 1;
    }
  else if (strcmp (name, "macro") == 0)
    {
      mips_warn_about_macros = 0;
    }
  else if (strcmp (name, "nomacro") == 0)
    {
      if (mips_noreorder == 0)
	as_bad ("`noreorder' must be set before `nomacro'");
      mips_warn_about_macros = 1;
    }
  else if (strcmp (name, "move") == 0 || strcmp (name, "novolatile") == 0)
    {
      mips_nomove = 0;
    }
  else if (strcmp (name, "nomove") == 0 || strcmp (name, "volatile") == 0)
    {
      mips_nomove = 1;
    }
  else if (strcmp (name, "bopt") == 0)
    {
      mips_nobopt = 0;
    }
  else if (strcmp (name, "nobopt") == 0)
    {
      mips_nobopt = 1;
    }
  else
    {
      as_warn ("Tried to set unrecognized symbol: %s\n", name);
    }
  *input_line_pointer = ch;
  demand_empty_rest_of_line ();
}

/* The same as the usual .space directive, except that we have to
   forget about any previous instruction.  */

static void
s_mips_space (param)
     int param;
{
  mips_emit_delays ();
  s_space (param);
}

int
tc_get_register ()
{
  int reg;

  SKIP_WHITESPACE ();
  if (*input_line_pointer++ != '$')
    {
      as_warn ("expected `$'");
      return 0;
    }
  if (isdigit ((unsigned char) *input_line_pointer))
    {
      reg = get_absolute_expression ();
      if (reg < 0 || reg >= 32)
	{
	  as_warn ("Bad register number");
	  reg = 0;
	}
    }
  else
    {
      if (strncmp (input_line_pointer, "fp", 2) == 0)
	reg = 30;
      else if (strncmp (input_line_pointer, "sp", 2) == 0)
	reg = 29;
      else if (strncmp (input_line_pointer, "gp", 2) == 0)
	reg = 28;
      else if (strncmp (input_line_pointer, "at", 2) == 0)
	reg = 1;
      else
	{
	  as_warn ("Unrecognized register name");
	  return 0;
	}
      input_line_pointer += 2;
    }
  return reg;
}

/*
 * Translate internal representation of relocation info to BFD target format.
 */
arelent *
tc_gen_reloc (section, fixp)
     asection *section;
     fixS *fixp;
{
  arelent *reloc;

  reloc = (arelent *) xmalloc (sizeof (arelent));
  assert (reloc != 0);

  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  if (fixp->fx_pcrel == 0)
    reloc->addend = fixp->fx_addnumber;
  else
#ifdef OBJ_ELF
    reloc->addend = 0;
#else
    reloc->addend = -reloc->address;
#endif
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  assert (reloc->howto != 0);

  return reloc;
}

/* should never be called */
valueT
md_section_align (seg, addr)
     asection *seg;
     valueT addr;
{
  int align = bfd_get_section_alignment (stdoutput, seg);

  return ((addr + (1 << align) - 1) & (-1 << align));
}

int
md_estimate_size_before_relax (fragP, segtype)
     fragS *fragP;
     asection *segtype;
{
  as_fatal ("md_estimate_size_before_relax");
  return (1);
}				/* md_estimate_size_before_relax() */

/* This function is called whenever a label is defined.  It is used
   when handling branch delays; if a branch has a label, we assume we
   can not move it.  */

void
mips_define_label (sym)
     symbolS *sym;
{
  insn_label = sym;
}

#ifndef OBJ_ECOFF

/* These functions should really be defined by the object file format,
   since they are related to debugging information.  However, this
   code has to work for the a.out format, which does not define them,
   so we provide simple versions here.  These don't actually generate
   any debugging information, but they do simple checking and someday
   somebody may make them useful.  */

typedef struct loc
{
  struct loc *loc_next;
  unsigned long loc_fileno;
  unsigned long loc_lineno;
  unsigned long loc_offset;
  unsigned short loc_delta;
  unsigned short loc_count;
#if 0
  fragS *loc_frag;
#endif
}
locS;

typedef struct proc
  {
    struct proc *proc_next;
    struct symbol *proc_isym;
    struct symbol *proc_end;
    unsigned long proc_reg_mask;
    unsigned long proc_reg_offset;
    unsigned long proc_fpreg_mask;
    unsigned long proc_fpreg_offset;
    unsigned long proc_frameoffset;
    unsigned long proc_framereg;
    unsigned long proc_pcreg;
    locS *proc_iline;
    struct file *proc_file;
    int proc_index;
  }
procS;

typedef struct file
  {
    struct file *file_next;
    unsigned long file_fileno;
    struct symbol *file_symbol;
    struct symbol *file_end;
    struct proc *file_proc;
    int file_numprocs;
  }
fileS;

static struct obstack proc_frags;
static procS *proc_lastP;
static procS *proc_rootP;
static int numprocs;

static void
md_obj_begin ()
{
  obstack_begin (&proc_frags, 0x2000);
}

static void
md_obj_end ()
{
  /* check for premature end, nesting errors, etc */
  if (proc_lastP && proc_lastP->proc_end == NULL)
    as_warn ("missing `.end' at end of assembly");
}

extern char hex_value[];

static long
get_number ()
{
  int negative = 0;
  long val = 0;

  if (*input_line_pointer == '-')
    {
      ++input_line_pointer;
      negative = 1;
    }
  if (!isdigit (*input_line_pointer))
    as_bad ("Expected simple number.");
  if (input_line_pointer[0] == '0')
    {
      if (input_line_pointer[1] == 'x')
	{
	  input_line_pointer += 2;
	  while (isxdigit (*input_line_pointer))
	    {
	      val <<= 4;
	      val |= hex_value[(int) *input_line_pointer++];
	    }
	  return negative ? -val : val;
	}
      else
	{
	  ++input_line_pointer;
	  while (isdigit (*input_line_pointer))
	    {
	      val <<= 3;
	      val |= *input_line_pointer++ - '0';
	    }
	  return negative ? -val : val;
	}
    }
  if (!isdigit (*input_line_pointer))
    {
      printf (" *input_line_pointer == '%c' 0x%02x\n",
	      *input_line_pointer, *input_line_pointer);
      as_warn ("Invalid number");
      return -1;
    }
  while (isdigit (*input_line_pointer))
    {
      val *= 10;
      val += *input_line_pointer++ - '0';
    }
  return negative ? -val : val;
}

/* The .file directive; just like the usual .file directive, but there
   is an initial number which is the ECOFF file index.  */

static void
s_file (x)
     int x;
{
  int line;

  line = get_number ();
  s_app_file (0);
}


/* The .end directive.  */

static void
s_mipsend (x)
     int x;
{
  symbolS *p;

  if (!is_end_of_line[(unsigned char) *input_line_pointer])
    {
      p = get_symbol ();
      demand_empty_rest_of_line ();
    }
  else
    p = NULL;
  if (now_seg != text_section)
    as_warn (".end not in text section");
  if (!proc_lastP)
    {
      as_warn (".end and no .ent seen yet.");
      return;
    }

  if (p != NULL)
    {
      assert (S_GET_NAME (p));
      if (strcmp (S_GET_NAME (p), S_GET_NAME (proc_lastP->proc_isym)))
	as_warn (".end symbol does not match .ent symbol.");
    }

  proc_lastP->proc_end = (symbolS *) 1;
}

/* The .aent and .ent directives.  */

static void
s_ent (aent)
     int aent;
{
  int number = 0;
  procS *procP;
  symbolS *symbolP;

  symbolP = get_symbol ();
  if (*input_line_pointer == ',')
    input_line_pointer++;
  if (isdigit (*input_line_pointer) || *input_line_pointer == '-')
    number = get_number ();
  if (now_seg != text_section)
    as_warn (".ent or .aent not in text section.");

  if (!aent && proc_lastP && proc_lastP->proc_end == NULL)
    as_warn ("missing `.end'");

  if (!aent)
    {
      procP = (procS *) obstack_alloc (&proc_frags, sizeof (*procP));
      procP->proc_isym = symbolP;
      procP->proc_reg_mask = 0;
      procP->proc_reg_offset = 0;
      procP->proc_fpreg_mask = 0;
      procP->proc_fpreg_offset = 0;
      procP->proc_frameoffset = 0;
      procP->proc_framereg = 0;
      procP->proc_pcreg = 0;
      procP->proc_end = NULL;
      procP->proc_next = NULL;
      if (proc_lastP)
	proc_lastP->proc_next = procP;
      else
	proc_rootP = procP;
      proc_lastP = procP;
      numprocs++;
    }
  demand_empty_rest_of_line ();
}

/* The .frame directive.  */

static void
s_frame (x)
     int x;
{
#if 0
  char str[100];
  symbolS *symP;
  int frame_reg;
  int frame_off;
  int pcreg;

  frame_reg = tc_get_register ();
  if (*input_line_pointer == ',')
    input_line_pointer++;
  frame_off = get_absolute_expression ();
  if (*input_line_pointer == ',')
    input_line_pointer++;
  pcreg = tc_get_register ();

  /* bob third eye */
  assert (proc_rootP);
  proc_rootP->proc_framereg = frame_reg;
  proc_rootP->proc_frameoffset = frame_off;
  proc_rootP->proc_pcreg = pcreg;
  /* bob macho .frame */

  /* We don't have to write out a frame stab for unoptimized code. */
  if (!(frame_reg == 30 && frame_off == 0))
    {
      if (!proc_lastP)
	as_warn ("No .ent for .frame to use.");
      (void) sprintf (str, "R%d;%d", frame_reg, frame_off);
      symP = symbol_new (str, N_VFP, 0, frag_now);
      S_SET_TYPE (symP, N_RMASK);
      S_SET_OTHER (symP, 0);
      S_SET_DESC (symP, 0);
      symP->sy_forward = proc_lastP->proc_isym;
      /* bob perhaps I should have used pseudo set */
    }
  demand_empty_rest_of_line ();
#endif
}

/* The .fmask and .mask directives.  */

static void
s_mask (reg_type)
     char reg_type;
{
#if 0
  char str[100], *strP;
  symbolS *symP;
  int i;
  unsigned int mask;
  int off;

  mask = get_number ();
  if (*input_line_pointer == ',')
    input_line_pointer++;
  off = get_absolute_expression ();

  /* bob only for coff */
  assert (proc_rootP);
  if (reg_type == 'F')
    {
      proc_rootP->proc_fpreg_mask = mask;
      proc_rootP->proc_fpreg_offset = off;
    }
  else
    {
      proc_rootP->proc_reg_mask = mask;
      proc_rootP->proc_reg_offset = off;
    }

  /* bob macho .mask + .fmask */

  /* We don't have to write out a mask stab if no saved regs. */
  if (!(mask == 0))
    {
      if (!proc_lastP)
	as_warn ("No .ent for .mask to use.");
      strP = str;
      for (i = 0; i < 32; i++)
	{
	  if (mask % 2)
	    {
	      sprintf (strP, "%c%d,", reg_type, i);
	      strP += strlen (strP);
	    }
	  mask /= 2;
	}
      sprintf (strP, ";%d,", off);
      symP = symbol_new (str, N_RMASK, 0, frag_now);
      S_SET_TYPE (symP, N_RMASK);
      S_SET_OTHER (symP, 0);
      S_SET_DESC (symP, 0);
      symP->sy_forward = proc_lastP->proc_isym;
      /* bob perhaps I should have used pseudo set */
    }
#endif
}

/* The .loc directive.  */

static void
s_loc (x)
     int x;
{
#if 0
  symbolS *symbolP;
  int lineno;
  int addroff;

  assert (now_seg == text_section);

  lineno = get_number ();
  addroff = obstack_next_free (&frags) - frag_now->fr_literal;

  symbolP = symbol_new ("", N_SLINE, addroff, frag_now);
  S_SET_TYPE (symbolP, N_SLINE);
  S_SET_OTHER (symbolP, 0);
  S_SET_DESC (symbolP, lineno);
  symbolP->sy_segment = now_seg;
#endif
}

#endif /* ! defined (OBJ_ECOFF) */
