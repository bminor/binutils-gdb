/* Target-dependent code for S390.
   Copyright (C) 2017 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"

#include "arch-utils.h"
#include "auxv.h"
#include "ax-gdb.h"
#include "dis-asm.h"
#include "dwarf2-frame.h"
#include "elf/common.h"
#include "elf/s390.h"
#include "elf-bfd.h"
#include "frame.h"
#include "frame-base.h"
#include "frame-unwind.h"
#include "gdbarch.h"
#include "gdbcore.h"
#include "infrun.h"
#include "linux-tdep.h"
#include "objfiles.h"
#include "osabi.h"
#include "prologue-value.h"
#include "record-full.h"
#include "reggroups.h"
#include "regset.h"
#include "s390-linux-tdep.h"
#include "s390-tdep.h"
#include "target-descriptions.h"
#include "trad-frame.h"
#include "value.h"

/* Decoding S/390 instructions.  */

/* Read a single instruction from address AT.  */

static int
s390_readinstruction (bfd_byte instr[], CORE_ADDR at)
{
  static int s390_instrlen[] = { 2, 4, 4, 6 };
  int instrlen;

  if (target_read_memory (at, &instr[0], 2))
    return -1;
  instrlen = s390_instrlen[instr[0] >> 6];
  if (instrlen > 2)
    {
      if (target_read_memory (at + 2, &instr[2], instrlen - 2))
	return -1;
    }
  return instrlen;
}

/* The functions below are for recognizing and decoding S/390
   instructions of various formats.  Each of them checks whether INSN
   is an instruction of the given format, with the specified opcodes.
   If it is, it sets the remaining arguments to the values of the
   instruction's fields, and returns a non-zero value; otherwise, it
   returns zero.

   These functions' arguments appear in the order they appear in the
   instruction, not in the machine-language form.  So, opcodes always
   come first, even though they're sometimes scattered around the
   instructions.  And displacements appear before base and extension
   registers, as they do in the assembly syntax, not at the end, as
   they do in the machine language.  */

static int
is_ri (bfd_byte *insn, int op1, int op2, unsigned int *r1, int *i2)
{
  if (insn[0] == op1 && (insn[1] & 0xf) == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      /* i2 is a 16-bit signed quantity.  */
      *i2 = (((insn[2] << 8) | insn[3]) ^ 0x8000) - 0x8000;
      return 1;
    }
  else
    return 0;
}


static int
is_ril (bfd_byte *insn, int op1, int op2,
	unsigned int *r1, int *i2)
{
  if (insn[0] == op1 && (insn[1] & 0xf) == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      /* i2 is a signed quantity.  If the host 'int' is 32 bits long,
	 no sign extension is necessary, but we don't want to assume
	 that.  */
      *i2 = (((insn[2] << 24)
	      | (insn[3] << 16)
	      | (insn[4] << 8)
	      | (insn[5])) ^ 0x80000000) - 0x80000000;
      return 1;
    }
  else
    return 0;
}


static int
is_rr (bfd_byte *insn, int op, unsigned int *r1, unsigned int *r2)
{
  if (insn[0] == op)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r2 = insn[1] & 0xf;
      return 1;
    }
  else
    return 0;
}


static int
is_rre (bfd_byte *insn, int op, unsigned int *r1, unsigned int *r2)
{
  if (((insn[0] << 8) | insn[1]) == op)
    {
      /* Yes, insn[3].  insn[2] is unused in RRE format.  */
      *r1 = (insn[3] >> 4) & 0xf;
      *r2 = insn[3] & 0xf;
      return 1;
    }
  else
    return 0;
}


static int
is_rs (bfd_byte *insn, int op,
       unsigned int *r1, unsigned int *r3, int *d2, unsigned int *b2)
{
  if (insn[0] == op)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r3 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      *d2 = ((insn[2] & 0xf) << 8) | insn[3];
      return 1;
    }
  else
    return 0;
}


static int
is_rsy (bfd_byte *insn, int op1, int op2,
	unsigned int *r1, unsigned int *r3, int *d2, unsigned int *b2)
{
  if (insn[0] == op1
      && insn[5] == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r3 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      /* The 'long displacement' is a 20-bit signed integer.  */
      *d2 = ((((insn[2] & 0xf) << 8) | insn[3] | (insn[4] << 12))
		^ 0x80000) - 0x80000;
      return 1;
    }
  else
    return 0;
}


static int
is_rsi (bfd_byte *insn, int op,
	unsigned int *r1, unsigned int *r3, int *i2)
{
  if (insn[0] == op)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r3 = insn[1] & 0xf;
      /* i2 is a 16-bit signed quantity.  */
      *i2 = (((insn[2] << 8) | insn[3]) ^ 0x8000) - 0x8000;
      return 1;
    }
  else
    return 0;
}


static int
is_rie (bfd_byte *insn, int op1, int op2,
	unsigned int *r1, unsigned int *r3, int *i2)
{
  if (insn[0] == op1
      && insn[5] == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *r3 = insn[1] & 0xf;
      /* i2 is a 16-bit signed quantity.  */
      *i2 = (((insn[2] << 8) | insn[3]) ^ 0x8000) - 0x8000;
      return 1;
    }
  else
    return 0;
}


static int
is_rx (bfd_byte *insn, int op,
       unsigned int *r1, int *d2, unsigned int *x2, unsigned int *b2)
{
  if (insn[0] == op)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *x2 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      *d2 = ((insn[2] & 0xf) << 8) | insn[3];
      return 1;
    }
  else
    return 0;
}


static int
is_rxy (bfd_byte *insn, int op1, int op2,
	unsigned int *r1, int *d2, unsigned int *x2, unsigned int *b2)
{
  if (insn[0] == op1
      && insn[5] == op2)
    {
      *r1 = (insn[1] >> 4) & 0xf;
      *x2 = insn[1] & 0xf;
      *b2 = (insn[2] >> 4) & 0xf;
      /* The 'long displacement' is a 20-bit signed integer.  */
      *d2 = ((((insn[2] & 0xf) << 8) | insn[3] | (insn[4] << 12))
		^ 0x80000) - 0x80000;
      return 1;
    }
  else
    return 0;
}

/* Breakpoints.  */

constexpr gdb_byte s390_break_insn[] = { 0x0, 0x1 };

typedef BP_MANIPULATION (s390_break_insn) s390_breakpoint;

/* A helper for s390_software_single_step, decides if an instruction
   is a partial-execution instruction that needs to be executed until
   completion when in record mode.  If it is, returns 1 and writes
   instruction length to a pointer.  */

static int
s390_is_partial_instruction (struct gdbarch *gdbarch, CORE_ADDR loc, int *len)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  uint16_t insn;

  insn = read_memory_integer (loc, 2, byte_order);

  switch (insn >> 8)
    {
    case 0xa8: /* MVCLE */
      *len = 4;
      return 1;

    case 0xeb:
      {
	insn = read_memory_integer (loc + 4, 2, byte_order);
	if ((insn & 0xff) == 0x8e)
	  {
	    /* MVCLU */
	    *len = 6;
	    return 1;
	  }
      }
      break;
    }

  switch (insn)
    {
    case 0xb255: /* MVST */
    case 0xb263: /* CMPSC */
    case 0xb2a5: /* TRE */
    case 0xb2a6: /* CU21 */
    case 0xb2a7: /* CU12 */
    case 0xb9b0: /* CU14 */
    case 0xb9b1: /* CU24 */
    case 0xb9b2: /* CU41 */
    case 0xb9b3: /* CU42 */
    case 0xb92a: /* KMF */
    case 0xb92b: /* KMO */
    case 0xb92f: /* KMC */
    case 0xb92d: /* KMCTR */
    case 0xb92e: /* KM */
    case 0xb93c: /* PPNO */
    case 0xb990: /* TRTT */
    case 0xb991: /* TRTO */
    case 0xb992: /* TROT */
    case 0xb993: /* TROO */
      *len = 4;
      return 1;
    }

  return 0;
}

/* Implement the "software_single_step" gdbarch method, needed to single step
   through instructions like MVCLE in record mode, to make sure they are
   executed to completion.  Without that, record will save the full length
   of destination buffer on every iteration, even though the CPU will only
   process about 4kiB of it each time, leading to O(n**2) memory and time
   complexity.  */

static VEC (CORE_ADDR) *
s390_software_single_step (struct regcache *regcache)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  CORE_ADDR loc = regcache_read_pc (regcache);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int len;
  uint16_t insn;
  VEC (CORE_ADDR) *next_pcs = NULL;

  /* Special handling only if recording.  */
  if (!record_full_is_used ())
    return NULL;

  /* First, match a partial instruction.  */
  if (!s390_is_partial_instruction (gdbarch, loc, &len))
    return NULL;

  loc += len;

  /* Second, look for a branch back to it.  */
  insn = read_memory_integer (loc, 2, byte_order);
  if (insn != 0xa714) /* BRC with mask 1 */
    return NULL;

  insn = read_memory_integer (loc + 2, 2, byte_order);
  if (insn != (uint16_t) -(len / 2))
    return NULL;

  loc += 4;

  /* Found it, step past the whole thing.  */
  VEC_safe_push (CORE_ADDR, next_pcs, loc);

  return next_pcs;
}


/* Prologue analysis.  */

struct s390_prologue_data {

  /* The stack.  */
  struct pv_area *stack;

  /* The size and byte-order of a GPR or FPR.  */
  int gpr_size;
  int fpr_size;
  enum bfd_endian byte_order;

  /* The general-purpose registers.  */
  pv_t gpr[S390_NUM_GPRS];

  /* The floating-point registers.  */
  pv_t fpr[S390_NUM_FPRS];

  /* The offset relative to the CFA where the incoming GPR N was saved
     by the function prologue.  0 if not saved or unknown.  */
  int gpr_slot[S390_NUM_GPRS];

  /* Likewise for FPRs.  */
  int fpr_slot[S390_NUM_FPRS];

  /* Nonzero if the backchain was saved.  This is assumed to be the
     case when the incoming SP is saved at the current SP location.  */
  int back_chain_saved_p;
};

/* Return the effective address for an X-style instruction, like:

	L R1, D2(X2, B2)

   Here, X2 and B2 are registers, and D2 is a signed 20-bit
   constant; the effective address is the sum of all three.  If either
   X2 or B2 are zero, then it doesn't contribute to the sum --- this
   means that r0 can't be used as either X2 or B2.  */

static pv_t
s390_addr (struct s390_prologue_data *data,
	   int d2, unsigned int x2, unsigned int b2)
{
  pv_t result;

  result = pv_constant (d2);
  if (x2)
    result = pv_add (result, data->gpr[x2]);
  if (b2)
    result = pv_add (result, data->gpr[b2]);

  return result;
}

/* Do a SIZE-byte store of VALUE to D2(X2,B2).  Helper for
   s390_analyze_prologue.  */

static void
s390_store (struct s390_prologue_data *data,
	    int d2, unsigned int x2, unsigned int b2, CORE_ADDR size,
	    pv_t value)
{
  pv_t addr = s390_addr (data, d2, x2, b2);
  pv_t offset;

  /* Check whether we are storing the backchain.  */
  offset = pv_subtract (data->gpr[S390_SP_REGNUM - S390_R0_REGNUM], addr);

  if (pv_is_constant (offset) && offset.k == 0)
    if (size == data->gpr_size
	&& pv_is_register_k (value, S390_SP_REGNUM, 0))
      {
	data->back_chain_saved_p = 1;
	return;
      }


  /* Check whether we are storing a register into the stack.  */
  if (!pv_area_store_would_trash (data->stack, addr))
    pv_area_store (data->stack, addr, size, value);


  /* Note: If this is some store we cannot identify, you might think we
     should forget our cached values, as any of those might have been hit.

     However, we make the assumption that the register save areas are only
     ever stored to once in any given function, and we do recognize these
     stores.  Thus every store we cannot recognize does not hit our data.  */
}

/* Do a SIZE-byte load from D2(X2,B2).  Helper for s390_analyze_prologue.  */

static pv_t
s390_load (struct s390_prologue_data *data,
	   int d2, unsigned int x2, unsigned int b2, CORE_ADDR size)

{
  pv_t addr = s390_addr (data, d2, x2, b2);

  /* If it's a load from an in-line constant pool, then we can
     simulate that, under the assumption that the code isn't
     going to change between the time the processor actually
     executed it creating the current frame, and the time when
     we're analyzing the code to unwind past that frame.  */
  if (pv_is_constant (addr))
    {
      struct target_section *secp;
      secp = target_section_by_addr (&current_target, addr.k);
      if (secp != NULL
	  && (bfd_get_section_flags (secp->the_bfd_section->owner,
				     secp->the_bfd_section)
	      & SEC_READONLY))
	return pv_constant (read_memory_integer (addr.k, size,
						 data->byte_order));
    }

  /* Check whether we are accessing one of our save slots.  */
  return pv_area_fetch (data->stack, addr, size);
}

/* Function for finding saved registers in a 'struct pv_area'; we pass
   this to pv_area_scan.

   If VALUE is a saved register, ADDR says it was saved at a constant
   offset from the frame base, and SIZE indicates that the whole
   register was saved, record its offset in the reg_offset table in
   PROLOGUE_UNTYPED.  */

static void
s390_check_for_saved (void *data_untyped, pv_t addr,
		      CORE_ADDR size, pv_t value)
{
  struct s390_prologue_data *data = (struct s390_prologue_data *) data_untyped;
  int i, offset;

  if (!pv_is_register (addr, S390_SP_REGNUM))
    return;

  offset = 16 * data->gpr_size + 32 - addr.k;

  /* If we are storing the original value of a register, we want to
     record the CFA offset.  If the same register is stored multiple
     times, the stack slot with the highest address counts.  */

  for (i = 0; i < S390_NUM_GPRS; i++)
    if (size == data->gpr_size
	&& pv_is_register_k (value, S390_R0_REGNUM + i, 0))
      if (data->gpr_slot[i] == 0
	  || data->gpr_slot[i] > offset)
	{
	  data->gpr_slot[i] = offset;
	  return;
	}

  for (i = 0; i < S390_NUM_FPRS; i++)
    if (size == data->fpr_size
	&& pv_is_register_k (value, S390_F0_REGNUM + i, 0))
      if (data->fpr_slot[i] == 0
	  || data->fpr_slot[i] > offset)
	{
	  data->fpr_slot[i] = offset;
	  return;
	}
}

/* Analyze the prologue of the function starting at START_PC,
   continuing at most until CURRENT_PC.  Initialize DATA to
   hold all information we find out about the state of the registers
   and stack slots.  Return the address of the instruction after
   the last one that changed the SP, FP, or back chain; or zero
   on error.  */

static CORE_ADDR
s390_analyze_prologue (struct gdbarch *gdbarch,
		       CORE_ADDR start_pc,
		       CORE_ADDR current_pc,
		       struct s390_prologue_data *data)
{
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;

  /* Our return value:
     The address of the instruction after the last one that changed
     the SP, FP, or back chain;  zero if we got an error trying to
     read memory.  */
  CORE_ADDR result = start_pc;

  /* The current PC for our abstract interpretation.  */
  CORE_ADDR pc;

  /* The address of the next instruction after that.  */
  CORE_ADDR next_pc;

  /* Set up everything's initial value.  */
  {
    int i;

    data->stack = make_pv_area (S390_SP_REGNUM, gdbarch_addr_bit (gdbarch));

    /* For the purpose of prologue tracking, we consider the GPR size to
       be equal to the ABI word size, even if it is actually larger
       (i.e. when running a 32-bit binary under a 64-bit kernel).  */
    data->gpr_size = word_size;
    data->fpr_size = 8;
    data->byte_order = gdbarch_byte_order (gdbarch);

    for (i = 0; i < S390_NUM_GPRS; i++)
      data->gpr[i] = pv_register (S390_R0_REGNUM + i, 0);

    for (i = 0; i < S390_NUM_FPRS; i++)
      data->fpr[i] = pv_register (S390_F0_REGNUM + i, 0);

    for (i = 0; i < S390_NUM_GPRS; i++)
      data->gpr_slot[i]  = 0;

    for (i = 0; i < S390_NUM_FPRS; i++)
      data->fpr_slot[i]  = 0;

    data->back_chain_saved_p = 0;
  }

  /* Start interpreting instructions, until we hit the frame's
     current PC or the first branch instruction.  */
  for (pc = start_pc; pc > 0 && pc < current_pc; pc = next_pc)
    {
      bfd_byte insn[S390_MAX_INSTR_SIZE];
      int insn_len = s390_readinstruction (insn, pc);

      bfd_byte dummy[S390_MAX_INSTR_SIZE] = { 0 };
      bfd_byte *insn32 = word_size == 4 ? insn : dummy;
      bfd_byte *insn64 = word_size == 8 ? insn : dummy;

      /* Fields for various kinds of instructions.  */
      unsigned int b2, r1, r2, x2, r3;
      int i2, d2;

      /* The values of SP and FP before this instruction,
	 for detecting instructions that change them.  */
      pv_t pre_insn_sp, pre_insn_fp;
      /* Likewise for the flag whether the back chain was saved.  */
      int pre_insn_back_chain_saved_p;

      /* If we got an error trying to read the instruction, report it.  */
      if (insn_len < 0)
	{
	  result = 0;
	  break;
	}

      next_pc = pc + insn_len;

      pre_insn_sp = data->gpr[S390_SP_REGNUM - S390_R0_REGNUM];
      pre_insn_fp = data->gpr[S390_FRAME_REGNUM - S390_R0_REGNUM];
      pre_insn_back_chain_saved_p = data->back_chain_saved_p;


      /* LHI r1, i2 --- load halfword immediate.  */
      /* LGHI r1, i2 --- load halfword immediate (64-bit version).  */
      /* LGFI r1, i2 --- load fullword immediate.  */
      if (is_ri (insn32, op1_lhi, op2_lhi, &r1, &i2)
	  || is_ri (insn64, op1_lghi, op2_lghi, &r1, &i2)
	  || is_ril (insn, op1_lgfi, op2_lgfi, &r1, &i2))
	data->gpr[r1] = pv_constant (i2);

      /* LR r1, r2 --- load from register.  */
      /* LGR r1, r2 --- load from register (64-bit version).  */
      else if (is_rr (insn32, op_lr, &r1, &r2)
	       || is_rre (insn64, op_lgr, &r1, &r2))
	data->gpr[r1] = data->gpr[r2];

      /* L r1, d2(x2, b2) --- load.  */
      /* LY r1, d2(x2, b2) --- load (long-displacement version).  */
      /* LG r1, d2(x2, b2) --- load (64-bit version).  */
      else if (is_rx (insn32, op_l, &r1, &d2, &x2, &b2)
	       || is_rxy (insn32, op1_ly, op2_ly, &r1, &d2, &x2, &b2)
	       || is_rxy (insn64, op1_lg, op2_lg, &r1, &d2, &x2, &b2))
	data->gpr[r1] = s390_load (data, d2, x2, b2, data->gpr_size);

      /* ST r1, d2(x2, b2) --- store.  */
      /* STY r1, d2(x2, b2) --- store (long-displacement version).  */
      /* STG r1, d2(x2, b2) --- store (64-bit version).  */
      else if (is_rx (insn32, op_st, &r1, &d2, &x2, &b2)
	       || is_rxy (insn32, op1_sty, op2_sty, &r1, &d2, &x2, &b2)
	       || is_rxy (insn64, op1_stg, op2_stg, &r1, &d2, &x2, &b2))
	s390_store (data, d2, x2, b2, data->gpr_size, data->gpr[r1]);

      /* STD r1, d2(x2,b2) --- store floating-point register.  */
      else if (is_rx (insn, op_std, &r1, &d2, &x2, &b2))
	s390_store (data, d2, x2, b2, data->fpr_size, data->fpr[r1]);

      /* STM r1, r3, d2(b2) --- store multiple.  */
      /* STMY r1, r3, d2(b2) --- store multiple (long-displacement
	 version).  */
      /* STMG r1, r3, d2(b2) --- store multiple (64-bit version).  */
      else if (is_rs (insn32, op_stm, &r1, &r3, &d2, &b2)
	       || is_rsy (insn32, op1_stmy, op2_stmy, &r1, &r3, &d2, &b2)
	       || is_rsy (insn64, op1_stmg, op2_stmg, &r1, &r3, &d2, &b2))
	{
	  for (; r1 <= r3; r1++, d2 += data->gpr_size)
	    s390_store (data, d2, 0, b2, data->gpr_size, data->gpr[r1]);
	}

      /* AHI r1, i2 --- add halfword immediate.  */
      /* AGHI r1, i2 --- add halfword immediate (64-bit version).  */
      /* AFI r1, i2 --- add fullword immediate.  */
      /* AGFI r1, i2 --- add fullword immediate (64-bit version).  */
      else if (is_ri (insn32, op1_ahi, op2_ahi, &r1, &i2)
	       || is_ri (insn64, op1_aghi, op2_aghi, &r1, &i2)
	       || is_ril (insn32, op1_afi, op2_afi, &r1, &i2)
	       || is_ril (insn64, op1_agfi, op2_agfi, &r1, &i2))
	data->gpr[r1] = pv_add_constant (data->gpr[r1], i2);

      /* ALFI r1, i2 --- add logical immediate.  */
      /* ALGFI r1, i2 --- add logical immediate (64-bit version).  */
      else if (is_ril (insn32, op1_alfi, op2_alfi, &r1, &i2)
	       || is_ril (insn64, op1_algfi, op2_algfi, &r1, &i2))
	data->gpr[r1] = pv_add_constant (data->gpr[r1],
					 (CORE_ADDR)i2 & 0xffffffff);

      /* AR r1, r2 -- add register.  */
      /* AGR r1, r2 -- add register (64-bit version).  */
      else if (is_rr (insn32, op_ar, &r1, &r2)
	       || is_rre (insn64, op_agr, &r1, &r2))
	data->gpr[r1] = pv_add (data->gpr[r1], data->gpr[r2]);

      /* A r1, d2(x2, b2) -- add.  */
      /* AY r1, d2(x2, b2) -- add (long-displacement version).  */
      /* AG r1, d2(x2, b2) -- add (64-bit version).  */
      else if (is_rx (insn32, op_a, &r1, &d2, &x2, &b2)
	       || is_rxy (insn32, op1_ay, op2_ay, &r1, &d2, &x2, &b2)
	       || is_rxy (insn64, op1_ag, op2_ag, &r1, &d2, &x2, &b2))
	data->gpr[r1] = pv_add (data->gpr[r1],
				s390_load (data, d2, x2, b2, data->gpr_size));

      /* SLFI r1, i2 --- subtract logical immediate.  */
      /* SLGFI r1, i2 --- subtract logical immediate (64-bit version).  */
      else if (is_ril (insn32, op1_slfi, op2_slfi, &r1, &i2)
	       || is_ril (insn64, op1_slgfi, op2_slgfi, &r1, &i2))
	data->gpr[r1] = pv_add_constant (data->gpr[r1],
					 -((CORE_ADDR)i2 & 0xffffffff));

      /* SR r1, r2 -- subtract register.  */
      /* SGR r1, r2 -- subtract register (64-bit version).  */
      else if (is_rr (insn32, op_sr, &r1, &r2)
	       || is_rre (insn64, op_sgr, &r1, &r2))
	data->gpr[r1] = pv_subtract (data->gpr[r1], data->gpr[r2]);

      /* S r1, d2(x2, b2) -- subtract.  */
      /* SY r1, d2(x2, b2) -- subtract (long-displacement version).  */
      /* SG r1, d2(x2, b2) -- subtract (64-bit version).  */
      else if (is_rx (insn32, op_s, &r1, &d2, &x2, &b2)
	       || is_rxy (insn32, op1_sy, op2_sy, &r1, &d2, &x2, &b2)
	       || is_rxy (insn64, op1_sg, op2_sg, &r1, &d2, &x2, &b2))
	data->gpr[r1] = pv_subtract (data->gpr[r1],
				s390_load (data, d2, x2, b2, data->gpr_size));

      /* LA r1, d2(x2, b2) --- load address.  */
      /* LAY r1, d2(x2, b2) --- load address (long-displacement version).  */
      else if (is_rx (insn, op_la, &r1, &d2, &x2, &b2)
	       || is_rxy (insn, op1_lay, op2_lay, &r1, &d2, &x2, &b2))
	data->gpr[r1] = s390_addr (data, d2, x2, b2);

      /* LARL r1, i2 --- load address relative long.  */
      else if (is_ril (insn, op1_larl, op2_larl, &r1, &i2))
	data->gpr[r1] = pv_constant (pc + i2 * 2);

      /* BASR r1, 0 --- branch and save.
	 Since r2 is zero, this saves the PC in r1, but doesn't branch.  */
      else if (is_rr (insn, op_basr, &r1, &r2)
	       && r2 == 0)
	data->gpr[r1] = pv_constant (next_pc);

      /* BRAS r1, i2 --- branch relative and save.  */
      else if (is_ri (insn, op1_bras, op2_bras, &r1, &i2))
	{
	  data->gpr[r1] = pv_constant (next_pc);
	  next_pc = pc + i2 * 2;

	  /* We'd better not interpret any backward branches.  We'll
	     never terminate.  */
	  if (next_pc <= pc)
	    break;
	}

      /* BRC/BRCL -- branch relative on condition.  Ignore "branch
	 never", branch to following instruction, and "conditional
	 trap" (BRC +2).  Otherwise terminate search.  */
      else if (is_ri (insn, op1_brc, op2_brc, &r1, &i2))
	{
	  if (r1 != 0 && i2 != 1 && i2 != 2)
	    break;
	}
      else if (is_ril (insn, op1_brcl, op2_brcl, &r1, &i2))
	{
	  if (r1 != 0 && i2 != 3)
	    break;
	}

      /* Terminate search when hitting any other branch instruction.  */
      else if (is_rr (insn, op_basr, &r1, &r2)
	       || is_rx (insn, op_bas, &r1, &d2, &x2, &b2)
	       || is_rr (insn, op_bcr, &r1, &r2)
	       || is_rx (insn, op_bc, &r1, &d2, &x2, &b2)
	       || is_ril (insn, op1_brasl, op2_brasl, &r2, &i2))
	break;

      else
	{
	  /* An instruction we don't know how to simulate.  The only
	     safe thing to do would be to set every value we're tracking
	     to 'unknown'.  Instead, we'll be optimistic: we assume that
	     we *can* interpret every instruction that the compiler uses
	     to manipulate any of the data we're interested in here --
	     then we can just ignore anything else.  */
	}

      /* Record the address after the last instruction that changed
	 the FP, SP, or backlink.  Ignore instructions that changed
	 them back to their original values --- those are probably
	 restore instructions.  (The back chain is never restored,
	 just popped.)  */
      {
	pv_t sp = data->gpr[S390_SP_REGNUM - S390_R0_REGNUM];
	pv_t fp = data->gpr[S390_FRAME_REGNUM - S390_R0_REGNUM];

	if ((! pv_is_identical (pre_insn_sp, sp)
	     && ! pv_is_register_k (sp, S390_SP_REGNUM, 0)
	     && sp.kind != pvk_unknown)
	    || (! pv_is_identical (pre_insn_fp, fp)
		&& ! pv_is_register_k (fp, S390_FRAME_REGNUM, 0)
		&& fp.kind != pvk_unknown)
	    || pre_insn_back_chain_saved_p != data->back_chain_saved_p)
	  result = next_pc;
      }
    }

  /* Record where all the registers were saved.  */
  pv_area_scan (data->stack, s390_check_for_saved, data);

  free_pv_area (data->stack);
  data->stack = NULL;

  return result;
}

/* Advance PC across any function entry prologue instructions to reach
   some "real" code.  */

static CORE_ADDR
s390_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  struct s390_prologue_data data;
  CORE_ADDR skip_pc, func_addr;

  if (find_pc_partial_function (pc, NULL, &func_addr, NULL))
    {
      CORE_ADDR post_prologue_pc
	= skip_prologue_using_sal (gdbarch, func_addr);
      if (post_prologue_pc != 0)
	return std::max (pc, post_prologue_pc);
    }

  skip_pc = s390_analyze_prologue (gdbarch, pc, (CORE_ADDR)-1, &data);
  return skip_pc ? skip_pc : pc;
}


/* Registers.  */

/* ABI call-saved register information.  */

static int
s390_register_call_saved (struct gdbarch *gdbarch, int regnum)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  switch (tdep->abi)
    {
    case ABI_LINUX_S390:
      if ((regnum >= S390_R6_REGNUM && regnum <= S390_R15_REGNUM)
	  || regnum == S390_F4_REGNUM || regnum == S390_F6_REGNUM
	  || regnum == S390_A0_REGNUM)
	return 1;

      break;

    case ABI_LINUX_ZSERIES:
      if ((regnum >= S390_R6_REGNUM && regnum <= S390_R15_REGNUM)
	  || (regnum >= S390_F8_REGNUM && regnum <= S390_F15_REGNUM)
	  || (regnum >= S390_A0_REGNUM && regnum <= S390_A1_REGNUM))
	return 1;

      break;
    }

  return 0;
}

/* Return the name of register REGNO.  Return the empty string for
   registers that shouldn't be visible.  */

static const char *
s390_register_name (struct gdbarch *gdbarch, int regnum)
{
  if (regnum >= S390_V0_LOWER_REGNUM
      && regnum <= S390_V15_LOWER_REGNUM)
    return "";
  return tdesc_register_name (gdbarch, regnum);
}

static int
s390_cannot_store_register (struct gdbarch *gdbarch, int regnum)
{
  /* The last-break address is read-only.  */
  return regnum == S390_LAST_BREAK_REGNUM;
}

static void
s390_write_pc (struct regcache *regcache, CORE_ADDR pc)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  regcache_cooked_write_unsigned (regcache, tdep->pc_regnum, pc);

  /* Set special SYSTEM_CALL register to 0 to prevent the kernel from
     messing with the PC we just installed, if we happen to be within
     an interrupted system call that the kernel wants to restart.

     Note that after we return from the dummy call, the SYSTEM_CALL and
     ORIG_R2 registers will be automatically restored, and the kernel
     continues to restart the system call at this point.  */
  if (register_size (gdbarch, S390_SYSTEM_CALL_REGNUM) > 0)
    regcache_cooked_write_unsigned (regcache, S390_SYSTEM_CALL_REGNUM, 0);
}

/* DWARF Register Mapping.  */

static const short s390_dwarf_regmap[] =
{
  /* 0-15: General Purpose Registers.  */
  S390_R0_REGNUM, S390_R1_REGNUM, S390_R2_REGNUM, S390_R3_REGNUM,
  S390_R4_REGNUM, S390_R5_REGNUM, S390_R6_REGNUM, S390_R7_REGNUM,
  S390_R8_REGNUM, S390_R9_REGNUM, S390_R10_REGNUM, S390_R11_REGNUM,
  S390_R12_REGNUM, S390_R13_REGNUM, S390_R14_REGNUM, S390_R15_REGNUM,

  /* 16-31: Floating Point Registers / Vector Registers 0-15. */
  S390_F0_REGNUM, S390_F2_REGNUM, S390_F4_REGNUM, S390_F6_REGNUM,
  S390_F1_REGNUM, S390_F3_REGNUM, S390_F5_REGNUM, S390_F7_REGNUM,
  S390_F8_REGNUM, S390_F10_REGNUM, S390_F12_REGNUM, S390_F14_REGNUM,
  S390_F9_REGNUM, S390_F11_REGNUM, S390_F13_REGNUM, S390_F15_REGNUM,

  /* 32-47: Control Registers.  */
  S390_CR0_REGNUM, S390_CR1_REGNUM, S390_CR2_REGNUM, S390_CR3_REGNUM,
  S390_CR4_REGNUM, S390_CR5_REGNUM, S390_CR6_REGNUM, S390_CR7_REGNUM,
  S390_CR8_REGNUM, S390_CR9_REGNUM, S390_CR10_REGNUM, S390_CR11_REGNUM,
  S390_CR12_REGNUM, S390_CR13_REGNUM, S390_CR14_REGNUM, S390_CR15_REGNUM,

  /* 48-63: Access Registers.  */
  S390_A0_REGNUM, S390_A1_REGNUM, S390_A2_REGNUM, S390_A3_REGNUM,
  S390_A4_REGNUM, S390_A5_REGNUM, S390_A6_REGNUM, S390_A7_REGNUM,
  S390_A8_REGNUM, S390_A9_REGNUM, S390_A10_REGNUM, S390_A11_REGNUM,
  S390_A12_REGNUM, S390_A13_REGNUM, S390_A14_REGNUM, S390_A15_REGNUM,

  /* 64-65: Program Status Word.  */
  S390_PSWM_REGNUM,
  S390_PSWA_REGNUM,

  /* 66-67: Reserved.  */
  -1, -1,

  /* 68-83: Vector Registers 16-31.  */
  S390_V16_REGNUM, S390_V18_REGNUM, S390_V20_REGNUM, S390_V22_REGNUM,
  S390_V17_REGNUM, S390_V19_REGNUM, S390_V21_REGNUM, S390_V23_REGNUM,
  S390_V24_REGNUM, S390_V26_REGNUM, S390_V28_REGNUM, S390_V30_REGNUM,
  S390_V25_REGNUM, S390_V27_REGNUM, S390_V29_REGNUM, S390_V31_REGNUM,

  /* End of "official" DWARF registers.  The remainder of the map is
     for GDB internal use only.  */

  /* GPR Lower Half Access.  */
  S390_R0_REGNUM, S390_R1_REGNUM, S390_R2_REGNUM, S390_R3_REGNUM,
  S390_R4_REGNUM, S390_R5_REGNUM, S390_R6_REGNUM, S390_R7_REGNUM,
  S390_R8_REGNUM, S390_R9_REGNUM, S390_R10_REGNUM, S390_R11_REGNUM,
  S390_R12_REGNUM, S390_R13_REGNUM, S390_R14_REGNUM, S390_R15_REGNUM,
};

enum { s390_dwarf_reg_r0l = ARRAY_SIZE (s390_dwarf_regmap) - 16 };

/* Convert DWARF register number REG to the appropriate register
   number used by GDB.  */

static int
s390_dwarf_reg_to_regnum (struct gdbarch *gdbarch, int reg)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int gdb_reg = -1;

  /* In a 32-on-64 debug scenario, debug info refers to the full
     64-bit GPRs.  Note that call frame information still refers to
     the 32-bit lower halves, because s390_adjust_frame_regnum uses
     special register numbers to access GPRs.  */
  if (tdep->gpr_full_regnum != -1 && reg >= 0 && reg < 16)
    return tdep->gpr_full_regnum + reg;

  if (reg >= 0 && reg < ARRAY_SIZE (s390_dwarf_regmap))
    gdb_reg = s390_dwarf_regmap[reg];

  if (tdep->v0_full_regnum == -1)
    {
      if (gdb_reg >= S390_V16_REGNUM && gdb_reg <= S390_V31_REGNUM)
	gdb_reg = -1;
    }
  else
    {
      if (gdb_reg >= S390_F0_REGNUM && gdb_reg <= S390_F15_REGNUM)
	gdb_reg = gdb_reg - S390_F0_REGNUM + tdep->v0_full_regnum;
    }

  return gdb_reg;
}

static int
regnum_is_gpr_full (struct gdbarch_tdep *tdep, int regnum)
{
  return (tdep->gpr_full_regnum != -1
	  && regnum >= tdep->gpr_full_regnum
	  && regnum <= tdep->gpr_full_regnum + 15);
}

/* Check whether REGNUM indicates a full vector register (v0-v15).
   These pseudo-registers are composed of f0-f15 and v0l-v15l.  */

static int
regnum_is_vxr_full (struct gdbarch_tdep *tdep, int regnum)
{
  return (tdep->v0_full_regnum != -1
	  && regnum >= tdep->v0_full_regnum
	  && regnum <= tdep->v0_full_regnum + 15);
}

/* 'float' values are stored in the upper half of floating-point
   registers, even though we are otherwise a big-endian platform.  The
   same applies to a 'float' value within a vector.  */

static struct value *
s390_value_from_register (struct gdbarch *gdbarch, struct type *type,
			  int regnum, struct frame_id frame_id)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  struct value *value = default_value_from_register (gdbarch, type,
						     regnum, frame_id);
  check_typedef (type);

  if ((regnum >= S390_F0_REGNUM && regnum <= S390_F15_REGNUM
       && TYPE_LENGTH (type) < 8)
      || regnum_is_vxr_full (tdep, regnum)
      || (regnum >= S390_V16_REGNUM && regnum <= S390_V31_REGNUM))
    set_value_offset (value, 0);

  return value;
}

static const struct target_desc *
s390_core_read_description (struct gdbarch *gdbarch,
			    struct target_ops *target, bfd *abfd)
{
  asection *section = bfd_get_section_by_name (abfd, ".reg");
  CORE_ADDR hwcap = 0;
  int high_gprs, v1, v2, te, vx;

  target_auxv_search (target, AT_HWCAP, &hwcap);
  if (!section)
    return NULL;

  high_gprs = (bfd_get_section_by_name (abfd, ".reg-s390-high-gprs")
	       != NULL);
  v1 = (bfd_get_section_by_name (abfd, ".reg-s390-last-break") != NULL);
  v2 = (bfd_get_section_by_name (abfd, ".reg-s390-system-call") != NULL);
  vx = (hwcap & HWCAP_S390_VX);
  te = (hwcap & HWCAP_S390_TE);

  switch (bfd_section_size (abfd, section))
    {
    case s390_sizeof_gregset:
      if (high_gprs)
	return (te && vx ? tdesc_s390_tevx_linux64 :
		vx ? tdesc_s390_vx_linux64 :
		te ? tdesc_s390_te_linux64 :
		v2 ? tdesc_s390_linux64v2 :
		v1 ? tdesc_s390_linux64v1 : tdesc_s390_linux64);
      else
	return (v2 ? tdesc_s390_linux32v2 :
		v1 ? tdesc_s390_linux32v1 : tdesc_s390_linux32);

    case s390x_sizeof_gregset:
      return (te && vx ? tdesc_s390x_tevx_linux64 :
	      vx ? tdesc_s390x_vx_linux64 :
	      te ? tdesc_s390x_te_linux64 :
	      v2 ? tdesc_s390x_linux64v2 :
	      v1 ? tdesc_s390x_linux64v1 : tdesc_s390x_linux64);

    default:
      return NULL;
    }
}

/* Iterate over supported core file register note sections. */

static void
s390_iterate_over_regset_sections (struct gdbarch *gdbarch,
				   iterate_over_regset_sections_cb *cb,
				   void *cb_data,
				   const struct regcache *regcache)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  const int gregset_size = (tdep->abi == ABI_LINUX_S390 ?
			    s390_sizeof_gregset : s390x_sizeof_gregset);

  cb (".reg", gregset_size, &s390_gregset, NULL, cb_data);
  cb (".reg2", s390_sizeof_fpregset, &s390_fpregset, NULL, cb_data);

  if (tdep->abi == ABI_LINUX_S390 && tdep->gpr_full_regnum != -1)
    cb (".reg-s390-high-gprs", 16 * 4, &s390_upper_regset,
	"s390 GPR upper halves", cb_data);

  if (tdep->have_linux_v1)
    cb (".reg-s390-last-break", 8,
	(gdbarch_ptr_bit (gdbarch) == 32
	 ? &s390_last_break_regset : &s390x_last_break_regset),
	"s390 last-break address", cb_data);

  if (tdep->have_linux_v2)
    cb (".reg-s390-system-call", 4, &s390_system_call_regset,
	"s390 system-call", cb_data);

  /* If regcache is set, we are in "write" (gcore) mode.  In this
     case, don't iterate over the TDB unless its registers are
     available.  */
  if (tdep->have_tdb
      && (regcache == NULL
	  || REG_VALID == regcache_register_status (regcache,
						    S390_TDB_DWORD0_REGNUM)))
    cb (".reg-s390-tdb", s390_sizeof_tdbregset, &s390_tdb_regset,
	"s390 TDB", cb_data);

  if (tdep->v0_full_regnum != -1)
    {
      cb (".reg-s390-vxrs-low", 16 * 8, &s390_vxrs_low_regset,
	  "s390 vector registers 0-15 lower half", cb_data);
      cb (".reg-s390-vxrs-high", 16 * 16, &s390_vxrs_high_regset,
	  "s390 vector registers 16-31", cb_data);
    }
}


/* Pseudo registers.  */

static const char *
s390_pseudo_register_name (struct gdbarch *gdbarch, int regnum)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (regnum == tdep->pc_regnum)
    return "pc";

  if (regnum == tdep->cc_regnum)
    return "cc";

  if (regnum_is_gpr_full (tdep, regnum))
    {
      static const char *full_name[] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
	"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
      };
      return full_name[regnum - tdep->gpr_full_regnum];
    }

  if (regnum_is_vxr_full (tdep, regnum))
    {
      static const char *full_name[] = {
	"v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
	"v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
      };
      return full_name[regnum - tdep->v0_full_regnum];
    }

  internal_error (__FILE__, __LINE__, _("invalid regnum"));
}

static enum register_status
s390_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
			   int regnum, gdb_byte *buf)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int regsize = register_size (gdbarch, regnum);
  ULONGEST val;

  if (regnum == tdep->pc_regnum)
    {
      enum register_status status;

      status = regcache_raw_read_unsigned (regcache, S390_PSWA_REGNUM, &val);
      if (status == REG_VALID)
	{
	  if (register_size (gdbarch, S390_PSWA_REGNUM) == 4)
	    val &= 0x7fffffff;
	  store_unsigned_integer (buf, regsize, byte_order, val);
	}
      return status;
    }

  if (regnum == tdep->cc_regnum)
    {
      enum register_status status;

      status = regcache_raw_read_unsigned (regcache, S390_PSWM_REGNUM, &val);
      if (status == REG_VALID)
	{
	  if (register_size (gdbarch, S390_PSWA_REGNUM) == 4)
	    val = (val >> 12) & 3;
	  else
	    val = (val >> 44) & 3;
	  store_unsigned_integer (buf, regsize, byte_order, val);
	}
      return status;
    }

  if (regnum_is_gpr_full (tdep, regnum))
    {
      enum register_status status;
      ULONGEST val_upper;

      regnum -= tdep->gpr_full_regnum;

      status = regcache_raw_read_unsigned (regcache,
					   S390_R0_REGNUM + regnum, &val);
      if (status == REG_VALID)
	status = regcache_raw_read_unsigned (regcache,
					     S390_R0_UPPER_REGNUM + regnum,
					     &val_upper);
      if (status == REG_VALID)
	{
	  val |= val_upper << 32;
	  store_unsigned_integer (buf, regsize, byte_order, val);
	}
      return status;
    }

  if (regnum_is_vxr_full (tdep, regnum))
    {
      enum register_status status;

      regnum -= tdep->v0_full_regnum;

      status = regcache_raw_read (regcache, S390_F0_REGNUM + regnum, buf);
      if (status == REG_VALID)
	status = regcache_raw_read (regcache,
				    S390_V0_LOWER_REGNUM + regnum, buf + 8);
      return status;
    }

  internal_error (__FILE__, __LINE__, _("invalid regnum"));
}

static void
s390_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
			    int regnum, const gdb_byte *buf)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int regsize = register_size (gdbarch, regnum);
  ULONGEST val, psw;

  if (regnum == tdep->pc_regnum)
    {
      val = extract_unsigned_integer (buf, regsize, byte_order);
      if (register_size (gdbarch, S390_PSWA_REGNUM) == 4)
	{
	  regcache_raw_read_unsigned (regcache, S390_PSWA_REGNUM, &psw);
	  val = (psw & 0x80000000) | (val & 0x7fffffff);
	}
      regcache_raw_write_unsigned (regcache, S390_PSWA_REGNUM, val);
      return;
    }

  if (regnum == tdep->cc_regnum)
    {
      val = extract_unsigned_integer (buf, regsize, byte_order);
      regcache_raw_read_unsigned (regcache, S390_PSWM_REGNUM, &psw);
      if (register_size (gdbarch, S390_PSWA_REGNUM) == 4)
	val = (psw & ~((ULONGEST)3 << 12)) | ((val & 3) << 12);
      else
	val = (psw & ~((ULONGEST)3 << 44)) | ((val & 3) << 44);
      regcache_raw_write_unsigned (regcache, S390_PSWM_REGNUM, val);
      return;
    }

  if (regnum_is_gpr_full (tdep, regnum))
    {
      regnum -= tdep->gpr_full_regnum;
      val = extract_unsigned_integer (buf, regsize, byte_order);
      regcache_raw_write_unsigned (regcache, S390_R0_REGNUM + regnum,
				   val & 0xffffffff);
      regcache_raw_write_unsigned (regcache, S390_R0_UPPER_REGNUM + regnum,
				   val >> 32);
      return;
    }

  if (regnum_is_vxr_full (tdep, regnum))
    {
      regnum -= tdep->v0_full_regnum;
      regcache_raw_write (regcache, S390_F0_REGNUM + regnum, buf);
      regcache_raw_write (regcache, S390_V0_LOWER_REGNUM + regnum, buf + 8);
      return;
    }

  internal_error (__FILE__, __LINE__, _("invalid regnum"));
}

static struct type *
s390_pseudo_register_type (struct gdbarch *gdbarch, int regnum)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (regnum == tdep->pc_regnum)
    return builtin_type (gdbarch)->builtin_func_ptr;

  if (regnum == tdep->cc_regnum)
    return builtin_type (gdbarch)->builtin_int;

  if (regnum_is_gpr_full (tdep, regnum))
    return builtin_type (gdbarch)->builtin_uint64;

  if (regnum_is_vxr_full (tdep, regnum))
    return tdesc_find_type (gdbarch, "vec128");

  internal_error (__FILE__, __LINE__, _("invalid regnum"));
}

/* Register groups.  */

static int
s390_pseudo_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
				 struct reggroup *group)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* We usually save/restore the whole PSW, which includes PC and CC.
     However, some older gdbservers may not support saving/restoring
     the whole PSW yet, and will return an XML register description
     excluding those from the save/restore register groups.  In those
     cases, we still need to explicitly save/restore PC and CC in order
     to push or pop frames.  Since this doesn't hurt anything if we
     already save/restore the whole PSW (it's just redundant), we add
     PC and CC at this point unconditionally.  */
  if (group == save_reggroup || group == restore_reggroup)
    return regnum == tdep->pc_regnum || regnum == tdep->cc_regnum;

  if (group == vector_reggroup)
    return regnum_is_vxr_full (tdep, regnum);

  if (group == general_reggroup && regnum_is_vxr_full (tdep, regnum))
    return 0;

  return default_register_reggroup_p (gdbarch, regnum, group);
}

/* The "ax_pseudo_register_collect" gdbarch method.  */

static int
s390_ax_pseudo_register_collect (struct gdbarch *gdbarch,
				 struct agent_expr *ax, int regnum)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  if (regnum == tdep->pc_regnum)
    {
      ax_reg_mask (ax, S390_PSWA_REGNUM);
    }
  else if (regnum == tdep->cc_regnum)
    {
      ax_reg_mask (ax, S390_PSWM_REGNUM);
    }
  else if (regnum_is_gpr_full (tdep, regnum))
    {
      regnum -= tdep->gpr_full_regnum;
      ax_reg_mask (ax, S390_R0_REGNUM + regnum);
      ax_reg_mask (ax, S390_R0_UPPER_REGNUM + regnum);
    }
  else if (regnum_is_vxr_full (tdep, regnum))
    {
      regnum -= tdep->v0_full_regnum;
      ax_reg_mask (ax, S390_F0_REGNUM + regnum);
      ax_reg_mask (ax, S390_V0_LOWER_REGNUM + regnum);
    }
  else
    {
      internal_error (__FILE__, __LINE__, _("invalid regnum"));
    }
  return 0;
}

/* The "ax_pseudo_register_push_stack" gdbarch method.  */

static int
s390_ax_pseudo_register_push_stack (struct gdbarch *gdbarch,
				    struct agent_expr *ax, int regnum)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  if (regnum == tdep->pc_regnum)
    {
      ax_reg (ax, S390_PSWA_REGNUM);
      if (register_size (gdbarch, S390_PSWA_REGNUM) == 4)
	{
	  ax_zero_ext (ax, 31);
	}
    }
  else if (regnum == tdep->cc_regnum)
    {
      ax_reg (ax, S390_PSWM_REGNUM);
      if (register_size (gdbarch, S390_PSWA_REGNUM) == 4)
	ax_const_l (ax, 12);
      else
	ax_const_l (ax, 44);
      ax_simple (ax, aop_rsh_unsigned);
      ax_zero_ext (ax, 2);
    }
  else if (regnum_is_gpr_full (tdep, regnum))
    {
      regnum -= tdep->gpr_full_regnum;
      ax_reg (ax, S390_R0_REGNUM + regnum);
      ax_reg (ax, S390_R0_UPPER_REGNUM + regnum);
      ax_const_l (ax, 32);
      ax_simple (ax, aop_lsh);
      ax_simple (ax, aop_bit_or);
    }
  else if (regnum_is_vxr_full (tdep, regnum))
    {
      /* Too large to stuff on the stack.  */
      return 1;
    }
  else
    {
      internal_error (__FILE__, __LINE__, _("invalid regnum"));
    }
  return 0;
}

/* The "gen_return_address" gdbarch method.  Since this is supposed to be
   just a best-effort method, and we don't really have the means to run
   the full unwinder here, just collect the link register.  */

static void
s390_gen_return_address (struct gdbarch *gdbarch,
			 struct agent_expr *ax, struct axs_value *value,
			 CORE_ADDR scope)
{
  value->type = register_type (gdbarch, S390_R14_REGNUM);
  value->kind = axs_lvalue_register;
  value->u.reg = S390_R14_REGNUM;
}

/* Helper routine to unwind pseudo registers.  */

static struct value *
s390_unwind_pseudo_register (struct frame_info *this_frame, int regnum)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  struct type *type = register_type (gdbarch, regnum);

  /* Unwind PC via PSW address.  */
  if (regnum == tdep->pc_regnum)
    {
      struct value *val;

      val = frame_unwind_register_value (this_frame, S390_PSWA_REGNUM);
      if (!value_optimized_out (val))
	{
	  LONGEST pswa = value_as_long (val);

	  if (TYPE_LENGTH (type) == 4)
	    return value_from_pointer (type, pswa & 0x7fffffff);
	  else
	    return value_from_pointer (type, pswa);
	}
    }

  /* Unwind CC via PSW mask.  */
  if (regnum == tdep->cc_regnum)
    {
      struct value *val;

      val = frame_unwind_register_value (this_frame, S390_PSWM_REGNUM);
      if (!value_optimized_out (val))
	{
	  LONGEST pswm = value_as_long (val);

	  if (TYPE_LENGTH (type) == 4)
	    return value_from_longest (type, (pswm >> 12) & 3);
	  else
	    return value_from_longest (type, (pswm >> 44) & 3);
	}
    }

  /* Unwind full GPRs to show at least the lower halves (as the
     upper halves are undefined).  */
  if (regnum_is_gpr_full (tdep, regnum))
    {
      int reg = regnum - tdep->gpr_full_regnum;
      struct value *val;

      val = frame_unwind_register_value (this_frame, S390_R0_REGNUM + reg);
      if (!value_optimized_out (val))
	return value_cast (type, val);
    }

  return allocate_optimized_out_value (type);
}


/* Inferior function calls.  */

/* Dummy function calls.  */

/* Unwrap any single-field structs in TYPE and return the effective
   "inner" type.  E.g., yield "float" for all these cases:

     float x;
     struct { float x };
     struct { struct { float x; } x; };
     struct { struct { struct { float x; } x; } x; };

   However, if an inner type is smaller than MIN_SIZE, abort the
   unwrapping.  */

static struct type *
s390_effective_inner_type (struct type *type, unsigned int min_size)
{
  while (TYPE_CODE (type) == TYPE_CODE_STRUCT
	 && TYPE_NFIELDS (type) == 1)
    {
      struct type *inner = check_typedef (TYPE_FIELD_TYPE (type, 0));

      if (TYPE_LENGTH (inner) < min_size)
	break;
      type = inner;
    }

  return type;
}

/* Return non-zero if TYPE should be passed like "float" or
   "double".  */

static int
s390_function_arg_float (struct type *type)
{
  /* Note that long double as well as complex types are intentionally
     excluded. */
  if (TYPE_LENGTH (type) > 8)
    return 0;

  /* A struct containing just a float or double is passed like a float
     or double.  */
  type = s390_effective_inner_type (type, 0);

  return (TYPE_CODE (type) == TYPE_CODE_FLT
	  || TYPE_CODE (type) == TYPE_CODE_DECFLOAT);
}

/* Return non-zero if TYPE should be passed like a vector.  */

static int
s390_function_arg_vector (struct type *type)
{
  if (TYPE_LENGTH (type) > 16)
    return 0;

  /* Structs containing just a vector are passed like a vector.  */
  type = s390_effective_inner_type (type, TYPE_LENGTH (type));

  return TYPE_CODE (type) == TYPE_CODE_ARRAY && TYPE_VECTOR (type);
}

/* Determine whether N is a power of two.  */

static int
is_power_of_two (unsigned int n)
{
  return n && ((n & (n - 1)) == 0);
}

/* For an argument whose type is TYPE and which is not passed like a
   float or vector, return non-zero if it should be passed like "int"
   or "long long".  */

static int
s390_function_arg_integer (struct type *type)
{
  enum type_code code = TYPE_CODE (type);

  if (TYPE_LENGTH (type) > 8)
    return 0;

  if (code == TYPE_CODE_INT
      || code == TYPE_CODE_ENUM
      || code == TYPE_CODE_RANGE
      || code == TYPE_CODE_CHAR
      || code == TYPE_CODE_BOOL
      || code == TYPE_CODE_PTR
      || code == TYPE_CODE_REF)
    return 1;

  return ((code == TYPE_CODE_UNION || code == TYPE_CODE_STRUCT)
	  && is_power_of_two (TYPE_LENGTH (type)));
}

/* Argument passing state: Internal data structure passed to helper
   routines of s390_push_dummy_call.  */

struct s390_arg_state
  {
    /* Register cache, or NULL, if we are in "preparation mode".  */
    struct regcache *regcache;
    /* Next available general/floating-point/vector register for
       argument passing.  */
    int gr, fr, vr;
    /* Current pointer to copy area (grows downwards).  */
    CORE_ADDR copy;
    /* Current pointer to parameter area (grows upwards).  */
    CORE_ADDR argp;
  };

/* Prepare one argument ARG for a dummy call and update the argument
   passing state AS accordingly.  If the regcache field in AS is set,
   operate in "write mode" and write ARG into the inferior.  Otherwise
   run "preparation mode" and skip all updates to the inferior.  */

static void
s390_handle_arg (struct s390_arg_state *as, struct value *arg,
		 struct gdbarch_tdep *tdep, int word_size,
		 enum bfd_endian byte_order, int is_unnamed)
{
  struct type *type = check_typedef (value_type (arg));
  unsigned int length = TYPE_LENGTH (type);
  int write_mode = as->regcache != NULL;

  if (s390_function_arg_float (type))
    {
      /* The GNU/Linux for S/390 ABI uses FPRs 0 and 2 to pass
	 arguments.  The GNU/Linux for zSeries ABI uses 0, 2, 4, and
	 6.  */
      if (as->fr <= (tdep->abi == ABI_LINUX_S390 ? 2 : 6))
	{
	  /* When we store a single-precision value in an FP register,
	     it occupies the leftmost bits.  */
	  if (write_mode)
	    regcache_cooked_write_part (as->regcache,
					S390_F0_REGNUM + as->fr,
					0, length,
					value_contents (arg));
	  as->fr += 2;
	}
      else
	{
	  /* When we store a single-precision value in a stack slot,
	     it occupies the rightmost bits.  */
	  as->argp = align_up (as->argp + length, word_size);
	  if (write_mode)
	    write_memory (as->argp - length, value_contents (arg),
			  length);
	}
    }
  else if (tdep->vector_abi == S390_VECTOR_ABI_128
	   && s390_function_arg_vector (type))
    {
      static const char use_vr[] = {24, 26, 28, 30, 25, 27, 29, 31};

      if (!is_unnamed && as->vr < ARRAY_SIZE (use_vr))
	{
	  int regnum = S390_V24_REGNUM + use_vr[as->vr] - 24;

	  if (write_mode)
	    regcache_cooked_write_part (as->regcache, regnum,
					0, length,
					value_contents (arg));
	  as->vr++;
	}
      else
	{
	  if (write_mode)
	    write_memory (as->argp, value_contents (arg), length);
	  as->argp = align_up (as->argp + length, word_size);
	}
    }
  else if (s390_function_arg_integer (type) && length <= word_size)
    {
      /* Initialize it just to avoid a GCC false warning.  */
      ULONGEST val = 0;

      if (write_mode)
	{
	  /* Place value in least significant bits of the register or
	     memory word and sign- or zero-extend to full word size.
	     This also applies to a struct or union.  */
	  val = TYPE_UNSIGNED (type)
	    ? extract_unsigned_integer (value_contents (arg),
					length, byte_order)
	    : extract_signed_integer (value_contents (arg),
				      length, byte_order);
	}

      if (as->gr <= 6)
	{
	  if (write_mode)
	    regcache_cooked_write_unsigned (as->regcache,
					    S390_R0_REGNUM + as->gr,
					    val);
	  as->gr++;
	}
      else
	{
	  if (write_mode)
	    write_memory_unsigned_integer (as->argp, word_size,
					   byte_order, val);
	  as->argp += word_size;
	}
    }
  else if (s390_function_arg_integer (type) && length == 8)
    {
      if (as->gr <= 5)
	{
	  if (write_mode)
	    {
	      regcache_cooked_write (as->regcache,
				     S390_R0_REGNUM + as->gr,
				     value_contents (arg));
	      regcache_cooked_write (as->regcache,
				     S390_R0_REGNUM + as->gr + 1,
				     value_contents (arg) + word_size);
	    }
	  as->gr += 2;
	}
      else
	{
	  /* If we skipped r6 because we couldn't fit a DOUBLE_ARG
	     in it, then don't go back and use it again later.  */
	  as->gr = 7;

	  if (write_mode)
	    write_memory (as->argp, value_contents (arg), length);
	  as->argp += length;
	}
    }
  else
    {
      /* This argument type is never passed in registers.  Place the
	 value in the copy area and pass a pointer to it.  Use 8-byte
	 alignment as a conservative assumption.  */
      as->copy = align_down (as->copy - length, 8);
      if (write_mode)
	write_memory (as->copy, value_contents (arg), length);

      if (as->gr <= 6)
	{
	  if (write_mode)
	    regcache_cooked_write_unsigned (as->regcache,
					    S390_R0_REGNUM + as->gr,
					    as->copy);
	  as->gr++;
	}
      else
	{
	  if (write_mode)
	    write_memory_unsigned_integer (as->argp, word_size,
					   byte_order, as->copy);
	  as->argp += word_size;
	}
    }
}

/* Put the actual parameter values pointed to by ARGS[0..NARGS-1] in
   place to be passed to a function, as specified by the "GNU/Linux
   for S/390 ELF Application Binary Interface Supplement".

   SP is the current stack pointer.  We must put arguments, links,
   padding, etc. whereever they belong, and return the new stack
   pointer value.

   If STRUCT_RETURN is non-zero, then the function we're calling is
   going to return a structure by value; STRUCT_ADDR is the address of
   a block we've allocated for it on the stack.

   Our caller has taken care of any type promotions needed to satisfy
   prototypes or the old K&R argument-passing rules.  */

static CORE_ADDR
s390_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
		      struct regcache *regcache, CORE_ADDR bp_addr,
		      int nargs, struct value **args, CORE_ADDR sp,
		      int struct_return, CORE_ADDR struct_addr)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int i;
  struct s390_arg_state arg_state, arg_prep;
  CORE_ADDR param_area_start, new_sp;
  struct type *ftype = check_typedef (value_type (function));

  if (TYPE_CODE (ftype) == TYPE_CODE_PTR)
    ftype = check_typedef (TYPE_TARGET_TYPE (ftype));

  arg_prep.copy = sp;
  arg_prep.gr = struct_return ? 3 : 2;
  arg_prep.fr = 0;
  arg_prep.vr = 0;
  arg_prep.argp = 0;
  arg_prep.regcache = NULL;

  /* Initialize arg_state for "preparation mode".  */
  arg_state = arg_prep;

  /* Update arg_state.copy with the start of the reference-to-copy area
     and arg_state.argp with the size of the parameter area.  */
  for (i = 0; i < nargs; i++)
    s390_handle_arg (&arg_state, args[i], tdep, word_size, byte_order,
		     TYPE_VARARGS (ftype) && i >= TYPE_NFIELDS (ftype));

  param_area_start = align_down (arg_state.copy - arg_state.argp, 8);

  /* Allocate the standard frame areas: the register save area, the
     word reserved for the compiler, and the back chain pointer.  */
  new_sp = param_area_start - (16 * word_size + 32);

  /* Now we have the final stack pointer.  Make sure we didn't
     underflow; on 31-bit, this would result in addresses with the
     high bit set, which causes confusion elsewhere.  Note that if we
     error out here, stack and registers remain untouched.  */
  if (gdbarch_addr_bits_remove (gdbarch, new_sp) != new_sp)
    error (_("Stack overflow"));

  /* Pass the structure return address in general register 2.  */
  if (struct_return)
    regcache_cooked_write_unsigned (regcache, S390_R2_REGNUM, struct_addr);

  /* Initialize arg_state for "write mode".  */
  arg_state = arg_prep;
  arg_state.argp = param_area_start;
  arg_state.regcache = regcache;

  /* Write all parameters.  */
  for (i = 0; i < nargs; i++)
    s390_handle_arg (&arg_state, args[i], tdep, word_size, byte_order,
		     TYPE_VARARGS (ftype) && i >= TYPE_NFIELDS (ftype));

  /* Store return PSWA.  In 31-bit mode, keep addressing mode bit.  */
  if (word_size == 4)
    {
      ULONGEST pswa;
      regcache_cooked_read_unsigned (regcache, S390_PSWA_REGNUM, &pswa);
      bp_addr = (bp_addr & 0x7fffffff) | (pswa & 0x80000000);
    }
  regcache_cooked_write_unsigned (regcache, S390_RETADDR_REGNUM, bp_addr);

  /* Store updated stack pointer.  */
  regcache_cooked_write_unsigned (regcache, S390_SP_REGNUM, new_sp);

  /* We need to return the 'stack part' of the frame ID,
     which is actually the top of the register save area.  */
  return param_area_start;
}

/* Assuming THIS_FRAME is a dummy, return the frame ID of that
   dummy frame.  The frame ID's base needs to match the TOS value
   returned by push_dummy_call, and the PC match the dummy frame's
   breakpoint.  */

static struct frame_id
s390_dummy_id (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  CORE_ADDR sp = get_frame_register_unsigned (this_frame, S390_SP_REGNUM);
  sp = gdbarch_addr_bits_remove (gdbarch, sp);

  return frame_id_build (sp + 16*word_size + 32,
			 get_frame_pc (this_frame));
}

static CORE_ADDR
s390_frame_align (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  /* Both the 32- and 64-bit ABI's say that the stack pointer should
     always be aligned on an eight-byte boundary.  */
  return (addr & -8);
}

/* Helper for s390_return_value: Set or retrieve a function return
   value if it resides in a register.  */

static void
s390_register_return_value (struct gdbarch *gdbarch, struct type *type,
			    struct regcache *regcache,
			    gdb_byte *out, const gdb_byte *in)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  int length = TYPE_LENGTH (type);
  int code = TYPE_CODE (type);

  if (code == TYPE_CODE_FLT || code == TYPE_CODE_DECFLOAT)
    {
      /* Float-like value: left-aligned in f0.  */
      if (in != NULL)
	regcache_cooked_write_part (regcache, S390_F0_REGNUM,
				    0, length, in);
      else
	regcache_cooked_read_part (regcache, S390_F0_REGNUM,
				   0, length, out);
    }
  else if (code == TYPE_CODE_ARRAY)
    {
      /* Vector: left-aligned in v24.  */
      if (in != NULL)
	regcache_cooked_write_part (regcache, S390_V24_REGNUM,
				    0, length, in);
      else
	regcache_cooked_read_part (regcache, S390_V24_REGNUM,
				   0, length, out);
    }
  else if (length <= word_size)
    {
      /* Integer: zero- or sign-extended in r2.  */
      if (out != NULL)
	regcache_cooked_read_part (regcache, S390_R2_REGNUM,
				   word_size - length, length, out);
      else if (TYPE_UNSIGNED (type))
	regcache_cooked_write_unsigned
	  (regcache, S390_R2_REGNUM,
	   extract_unsigned_integer (in, length, byte_order));
      else
	regcache_cooked_write_signed
	  (regcache, S390_R2_REGNUM,
	   extract_signed_integer (in, length, byte_order));
    }
  else if (length == 2 * word_size)
    {
      /* Double word: in r2 and r3.  */
      if (in != NULL)
	{
	  regcache_cooked_write (regcache, S390_R2_REGNUM, in);
	  regcache_cooked_write (regcache, S390_R3_REGNUM,
				 in + word_size);
	}
      else
	{
	  regcache_cooked_read (regcache, S390_R2_REGNUM, out);
	  regcache_cooked_read (regcache, S390_R3_REGNUM,
				out + word_size);
	}
    }
  else
    internal_error (__FILE__, __LINE__, _("invalid return type"));
}

/* Implement the 'return_value' gdbarch method.  */

static enum return_value_convention
s390_return_value (struct gdbarch *gdbarch, struct value *function,
		   struct type *type, struct regcache *regcache,
		   gdb_byte *out, const gdb_byte *in)
{
  enum return_value_convention rvc;

  type = check_typedef (type);

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
    case TYPE_CODE_COMPLEX:
      rvc = RETURN_VALUE_STRUCT_CONVENTION;
      break;
    case TYPE_CODE_ARRAY:
      rvc = (gdbarch_tdep (gdbarch)->vector_abi == S390_VECTOR_ABI_128
	     && TYPE_LENGTH (type) <= 16 && TYPE_VECTOR (type))
	? RETURN_VALUE_REGISTER_CONVENTION
	: RETURN_VALUE_STRUCT_CONVENTION;
      break;
    default:
      rvc = TYPE_LENGTH (type) <= 8
	? RETURN_VALUE_REGISTER_CONVENTION
	: RETURN_VALUE_STRUCT_CONVENTION;
    }

  if (in != NULL || out != NULL)
    {
      if (rvc == RETURN_VALUE_REGISTER_CONVENTION)
	s390_register_return_value (gdbarch, type, regcache, out, in);
      else if (in != NULL)
	error (_("Cannot set function return value."));
      else
	error (_("Function return value unknown."));
    }

  return rvc;
}


/* Frame handling.  */

/* Implmement the stack_frame_destroyed_p gdbarch method.  */

static int
s390_stack_frame_destroyed_p (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;

  /* In frameless functions, there's not frame to destroy and thus
     we don't care about the epilogue.

     In functions with frame, the epilogue sequence is a pair of
     a LM-type instruction that restores (amongst others) the
     return register %r14 and the stack pointer %r15, followed
     by a branch 'br %r14' --or equivalent-- that effects the
     actual return.

     In that situation, this function needs to return 'true' in
     exactly one case: when pc points to that branch instruction.

     Thus we try to disassemble the one instructions immediately
     preceding pc and check whether it is an LM-type instruction
     modifying the stack pointer.

     Note that disassembling backwards is not reliable, so there
     is a slight chance of false positives here ...  */

  bfd_byte insn[6];
  unsigned int r1, r3, b2;
  int d2;

  if (word_size == 4
      && !target_read_memory (pc - 4, insn, 4)
      && is_rs (insn, op_lm, &r1, &r3, &d2, &b2)
      && r3 == S390_SP_REGNUM - S390_R0_REGNUM)
    return 1;

  if (word_size == 4
      && !target_read_memory (pc - 6, insn, 6)
      && is_rsy (insn, op1_lmy, op2_lmy, &r1, &r3, &d2, &b2)
      && r3 == S390_SP_REGNUM - S390_R0_REGNUM)
    return 1;

  if (word_size == 8
      && !target_read_memory (pc - 6, insn, 6)
      && is_rsy (insn, op1_lmg, op2_lmg, &r1, &r3, &d2, &b2)
      && r3 == S390_SP_REGNUM - S390_R0_REGNUM)
    return 1;

  return 0;
}

/* DWARF-2 frame support.  */

static struct value *
s390_dwarf2_prev_register (struct frame_info *this_frame, void **this_cache,
			   int regnum)
{
  return s390_unwind_pseudo_register (this_frame, regnum);
}

static void
s390_dwarf2_frame_init_reg (struct gdbarch *gdbarch, int regnum,
			    struct dwarf2_frame_state_reg *reg,
			    struct frame_info *this_frame)
{
  /* The condition code (and thus PSW mask) is call-clobbered.  */
  if (regnum == S390_PSWM_REGNUM)
    reg->how = DWARF2_FRAME_REG_UNDEFINED;

  /* The PSW address unwinds to the return address.  */
  else if (regnum == S390_PSWA_REGNUM)
    reg->how = DWARF2_FRAME_REG_RA;

  /* Fixed registers are call-saved or call-clobbered
     depending on the ABI in use.  */
  else if (regnum < S390_NUM_REGS)
    {
      if (s390_register_call_saved (gdbarch, regnum))
	reg->how = DWARF2_FRAME_REG_SAME_VALUE;
      else
	reg->how = DWARF2_FRAME_REG_UNDEFINED;
    }

  /* We install a special function to unwind pseudos.  */
  else
    {
      reg->how = DWARF2_FRAME_REG_FN;
      reg->loc.fn = s390_dwarf2_prev_register;
    }
}

/* Translate a .eh_frame register to DWARF register, or adjust a
   .debug_frame register.  */

static int
s390_adjust_frame_regnum (struct gdbarch *gdbarch, int num, int eh_frame_p)
{
  /* See s390_dwarf_reg_to_regnum for comments.  */
  return (num >= 0 && num < 16) ? num + s390_dwarf_reg_r0l : num;
}

/* Normal stack frames.  */

struct s390_unwind_cache {

  CORE_ADDR func;
  CORE_ADDR frame_base;
  CORE_ADDR local_base;

  struct trad_frame_saved_reg *saved_regs;
};

static int
s390_prologue_frame_unwind_cache (struct frame_info *this_frame,
				  struct s390_unwind_cache *info)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  struct s390_prologue_data data;
  pv_t *fp = &data.gpr[S390_FRAME_REGNUM - S390_R0_REGNUM];
  pv_t *sp = &data.gpr[S390_SP_REGNUM - S390_R0_REGNUM];
  int i;
  CORE_ADDR cfa;
  CORE_ADDR func;
  CORE_ADDR result;
  ULONGEST reg;
  CORE_ADDR prev_sp;
  int frame_pointer;
  int size;
  struct frame_info *next_frame;

  /* Try to find the function start address.  If we can't find it, we don't
     bother searching for it -- with modern compilers this would be mostly
     pointless anyway.  Trust that we'll either have valid DWARF-2 CFI data
     or else a valid backchain ...  */
  if (!get_frame_func_if_available (this_frame, &info->func))
    {
      info->func = -1;
      return 0;
    }
  func = info->func;

  /* Try to analyze the prologue.  */
  result = s390_analyze_prologue (gdbarch, func,
				  get_frame_pc (this_frame), &data);
  if (!result)
    return 0;

  /* If this was successful, we should have found the instruction that
     sets the stack pointer register to the previous value of the stack
     pointer minus the frame size.  */
  if (!pv_is_register (*sp, S390_SP_REGNUM))
    return 0;

  /* A frame size of zero at this point can mean either a real
     frameless function, or else a failure to find the prologue.
     Perform some sanity checks to verify we really have a
     frameless function.  */
  if (sp->k == 0)
    {
      /* If the next frame is a NORMAL_FRAME, this frame *cannot* have frame
	 size zero.  This is only possible if the next frame is a sentinel
	 frame, a dummy frame, or a signal trampoline frame.  */
      /* FIXME: cagney/2004-05-01: This sanity check shouldn't be
	 needed, instead the code should simpliy rely on its
	 analysis.  */
      next_frame = get_next_frame (this_frame);
      while (next_frame && get_frame_type (next_frame) == INLINE_FRAME)
	next_frame = get_next_frame (next_frame);
      if (next_frame
	  && get_frame_type (get_next_frame (this_frame)) == NORMAL_FRAME)
	return 0;

      /* If we really have a frameless function, %r14 must be valid
	 -- in particular, it must point to a different function.  */
      reg = get_frame_register_unsigned (this_frame, S390_RETADDR_REGNUM);
      reg = gdbarch_addr_bits_remove (gdbarch, reg) - 1;
      if (get_pc_function_start (reg) == func)
	{
	  /* However, there is one case where it *is* valid for %r14
	     to point to the same function -- if this is a recursive
	     call, and we have stopped in the prologue *before* the
	     stack frame was allocated.

	     Recognize this case by looking ahead a bit ...  */

	  struct s390_prologue_data data2;
	  pv_t *sp = &data2.gpr[S390_SP_REGNUM - S390_R0_REGNUM];

	  if (!(s390_analyze_prologue (gdbarch, func, (CORE_ADDR)-1, &data2)
		&& pv_is_register (*sp, S390_SP_REGNUM)
		&& sp->k != 0))
	    return 0;
	}
    }


  /* OK, we've found valid prologue data.  */
  size = -sp->k;

  /* If the frame pointer originally also holds the same value
     as the stack pointer, we're probably using it.  If it holds
     some other value -- even a constant offset -- it is most
     likely used as temp register.  */
  if (pv_is_identical (*sp, *fp))
    frame_pointer = S390_FRAME_REGNUM;
  else
    frame_pointer = S390_SP_REGNUM;

  /* If we've detected a function with stack frame, we'll still have to
     treat it as frameless if we're currently within the function epilog
     code at a point where the frame pointer has already been restored.
     This can only happen in an innermost frame.  */
  /* FIXME: cagney/2004-05-01: This sanity check shouldn't be needed,
     instead the code should simpliy rely on its analysis.  */
  next_frame = get_next_frame (this_frame);
  while (next_frame && get_frame_type (next_frame) == INLINE_FRAME)
    next_frame = get_next_frame (next_frame);
  if (size > 0
      && (next_frame == NULL
	  || get_frame_type (get_next_frame (this_frame)) != NORMAL_FRAME))
    {
      /* See the comment in s390_stack_frame_destroyed_p on why this is
	 not completely reliable ...  */
      if (s390_stack_frame_destroyed_p (gdbarch, get_frame_pc (this_frame)))
	{
	  memset (&data, 0, sizeof (data));
	  size = 0;
	  frame_pointer = S390_SP_REGNUM;
	}
    }

  /* Once we know the frame register and the frame size, we can unwind
     the current value of the frame register from the next frame, and
     add back the frame size to arrive that the previous frame's
     stack pointer value.  */
  prev_sp = get_frame_register_unsigned (this_frame, frame_pointer) + size;
  cfa = prev_sp + 16*word_size + 32;

  /* Set up ABI call-saved/call-clobbered registers.  */
  for (i = 0; i < S390_NUM_REGS; i++)
    if (!s390_register_call_saved (gdbarch, i))
      trad_frame_set_unknown (info->saved_regs, i);

  /* CC is always call-clobbered.  */
  trad_frame_set_unknown (info->saved_regs, S390_PSWM_REGNUM);

  /* Record the addresses of all register spill slots the prologue parser
     has recognized.  Consider only registers defined as call-saved by the
     ABI; for call-clobbered registers the parser may have recognized
     spurious stores.  */

  for (i = 0; i < 16; i++)
    if (s390_register_call_saved (gdbarch, S390_R0_REGNUM + i)
	&& data.gpr_slot[i] != 0)
      info->saved_regs[S390_R0_REGNUM + i].addr = cfa - data.gpr_slot[i];

  for (i = 0; i < 16; i++)
    if (s390_register_call_saved (gdbarch, S390_F0_REGNUM + i)
	&& data.fpr_slot[i] != 0)
      info->saved_regs[S390_F0_REGNUM + i].addr = cfa - data.fpr_slot[i];

  /* Function return will set PC to %r14.  */
  info->saved_regs[S390_PSWA_REGNUM] = info->saved_regs[S390_RETADDR_REGNUM];

  /* In frameless functions, we unwind simply by moving the return
     address to the PC.  However, if we actually stored to the
     save area, use that -- we might only think the function frameless
     because we're in the middle of the prologue ...  */
  if (size == 0
      && !trad_frame_addr_p (info->saved_regs, S390_PSWA_REGNUM))
    {
      info->saved_regs[S390_PSWA_REGNUM].realreg = S390_RETADDR_REGNUM;
    }

  /* Another sanity check: unless this is a frameless function,
     we should have found spill slots for SP and PC.
     If not, we cannot unwind further -- this happens e.g. in
     libc's thread_start routine.  */
  if (size > 0)
    {
      if (!trad_frame_addr_p (info->saved_regs, S390_SP_REGNUM)
	  || !trad_frame_addr_p (info->saved_regs, S390_PSWA_REGNUM))
	prev_sp = -1;
    }

  /* We use the current value of the frame register as local_base,
     and the top of the register save area as frame_base.  */
  if (prev_sp != -1)
    {
      info->frame_base = prev_sp + 16*word_size + 32;
      info->local_base = prev_sp - size;
    }

  return 1;
}


/* Code stubs and their stack frames.  For things like PLTs and NULL
   function calls (where there is no true frame and the return address
   is in the RETADDR register).  */

struct s390_stub_unwind_cache
{
  CORE_ADDR frame_base;
  struct trad_frame_saved_reg *saved_regs;
};

static struct s390_stub_unwind_cache *
s390_stub_frame_unwind_cache (struct frame_info *this_frame,
			      void **this_prologue_cache)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  struct s390_stub_unwind_cache *info;
  ULONGEST reg;

  if (*this_prologue_cache)
    return (struct s390_stub_unwind_cache *) *this_prologue_cache;

  info = FRAME_OBSTACK_ZALLOC (struct s390_stub_unwind_cache);
  *this_prologue_cache = info;
  info->saved_regs = trad_frame_alloc_saved_regs (this_frame);

  /* The return address is in register %r14.  */
  info->saved_regs[S390_PSWA_REGNUM].realreg = S390_RETADDR_REGNUM;

  /* Retrieve stack pointer and determine our frame base.  */
  reg = get_frame_register_unsigned (this_frame, S390_SP_REGNUM);
  info->frame_base = reg + 16*word_size + 32;

  return info;
}

static void
s390_stub_frame_this_id (struct frame_info *this_frame,
			 void **this_prologue_cache,
			 struct frame_id *this_id)
{
  struct s390_stub_unwind_cache *info
    = s390_stub_frame_unwind_cache (this_frame, this_prologue_cache);
  *this_id = frame_id_build (info->frame_base, get_frame_pc (this_frame));
}

static struct value *
s390_trad_frame_prev_register (struct frame_info *this_frame,
			       struct trad_frame_saved_reg saved_regs[],
			       int regnum)
{
  if (regnum < S390_NUM_REGS)
    return trad_frame_get_prev_register (this_frame, saved_regs, regnum);
  else
    return s390_unwind_pseudo_register (this_frame, regnum);
}

static struct value *
s390_stub_frame_prev_register (struct frame_info *this_frame,
			       void **this_prologue_cache, int regnum)
{
  struct s390_stub_unwind_cache *info
    = s390_stub_frame_unwind_cache (this_frame, this_prologue_cache);
  return s390_trad_frame_prev_register (this_frame, info->saved_regs, regnum);
}

static int
s390_stub_frame_sniffer (const struct frame_unwind *self,
			 struct frame_info *this_frame,
			 void **this_prologue_cache)
{
  CORE_ADDR addr_in_block;
  bfd_byte insn[S390_MAX_INSTR_SIZE];

  /* If the current PC points to non-readable memory, we assume we
     have trapped due to an invalid function pointer call.  We handle
     the non-existing current function like a PLT stub.  */
  addr_in_block = get_frame_address_in_block (this_frame);
  if (in_plt_section (addr_in_block)
      || s390_readinstruction (insn, get_frame_pc (this_frame)) < 0)
    return 1;
  return 0;
}

static const struct frame_unwind s390_stub_frame_unwind = {
  NORMAL_FRAME,
  default_frame_unwind_stop_reason,
  s390_stub_frame_this_id,
  s390_stub_frame_prev_register,
  NULL,
  s390_stub_frame_sniffer
};

/* Signal trampoline stack frames.  */

struct s390_sigtramp_unwind_cache {
  CORE_ADDR frame_base;
  struct trad_frame_saved_reg *saved_regs;
};

static struct s390_sigtramp_unwind_cache *
s390_sigtramp_frame_unwind_cache (struct frame_info *this_frame,
				  void **this_prologue_cache)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  struct s390_sigtramp_unwind_cache *info;
  ULONGEST this_sp, prev_sp;
  CORE_ADDR next_ra, next_cfa, sigreg_ptr, sigreg_high_off;
  int i;

  if (*this_prologue_cache)
    return (struct s390_sigtramp_unwind_cache *) *this_prologue_cache;

  info = FRAME_OBSTACK_ZALLOC (struct s390_sigtramp_unwind_cache);
  *this_prologue_cache = info;
  info->saved_regs = trad_frame_alloc_saved_regs (this_frame);

  this_sp = get_frame_register_unsigned (this_frame, S390_SP_REGNUM);
  next_ra = get_frame_pc (this_frame);
  next_cfa = this_sp + 16*word_size + 32;

  /* New-style RT frame:
	retcode + alignment (8 bytes)
	siginfo (128 bytes)
	ucontext (contains sigregs at offset 5 words).  */
  if (next_ra == next_cfa)
    {
      sigreg_ptr = next_cfa + 8 + 128 + align_up (5*word_size, 8);
      /* sigregs are followed by uc_sigmask (8 bytes), then by the
	 upper GPR halves if present.  */
      sigreg_high_off = 8;
    }

  /* Old-style RT frame and all non-RT frames:
	old signal mask (8 bytes)
	pointer to sigregs.  */
  else
    {
      sigreg_ptr = read_memory_unsigned_integer (next_cfa + 8,
						 word_size, byte_order);
      /* sigregs are followed by signo (4 bytes), then by the
	 upper GPR halves if present.  */
      sigreg_high_off = 4;
    }

  /* The sigregs structure looks like this:
	    long   psw_mask;
	    long   psw_addr;
	    long   gprs[16];
	    int    acrs[16];
	    int    fpc;
	    int    __pad;
	    double fprs[16];  */

  /* PSW mask and address.  */
  info->saved_regs[S390_PSWM_REGNUM].addr = sigreg_ptr;
  sigreg_ptr += word_size;
  info->saved_regs[S390_PSWA_REGNUM].addr = sigreg_ptr;
  sigreg_ptr += word_size;

  /* Then the GPRs.  */
  for (i = 0; i < 16; i++)
    {
      info->saved_regs[S390_R0_REGNUM + i].addr = sigreg_ptr;
      sigreg_ptr += word_size;
    }

  /* Then the ACRs.  */
  for (i = 0; i < 16; i++)
    {
      info->saved_regs[S390_A0_REGNUM + i].addr = sigreg_ptr;
      sigreg_ptr += 4;
    }

  /* The floating-point control word.  */
  info->saved_regs[S390_FPC_REGNUM].addr = sigreg_ptr;
  sigreg_ptr += 8;

  /* And finally the FPRs.  */
  for (i = 0; i < 16; i++)
    {
      info->saved_regs[S390_F0_REGNUM + i].addr = sigreg_ptr;
      sigreg_ptr += 8;
    }

  /* If we have them, the GPR upper halves are appended at the end.  */
  sigreg_ptr += sigreg_high_off;
  if (tdep->gpr_full_regnum != -1)
    for (i = 0; i < 16; i++)
      {
	info->saved_regs[S390_R0_UPPER_REGNUM + i].addr = sigreg_ptr;
	sigreg_ptr += 4;
      }

  /* Restore the previous frame's SP.  */
  prev_sp = read_memory_unsigned_integer (
			info->saved_regs[S390_SP_REGNUM].addr,
			word_size, byte_order);

  /* Determine our frame base.  */
  info->frame_base = prev_sp + 16*word_size + 32;

  return info;
}

static void
s390_sigtramp_frame_this_id (struct frame_info *this_frame,
			     void **this_prologue_cache,
			     struct frame_id *this_id)
{
  struct s390_sigtramp_unwind_cache *info
    = s390_sigtramp_frame_unwind_cache (this_frame, this_prologue_cache);
  *this_id = frame_id_build (info->frame_base, get_frame_pc (this_frame));
}

static struct value *
s390_sigtramp_frame_prev_register (struct frame_info *this_frame,
				   void **this_prologue_cache, int regnum)
{
  struct s390_sigtramp_unwind_cache *info
    = s390_sigtramp_frame_unwind_cache (this_frame, this_prologue_cache);
  return s390_trad_frame_prev_register (this_frame, info->saved_regs, regnum);
}

static int
s390_sigtramp_frame_sniffer (const struct frame_unwind *self,
			     struct frame_info *this_frame,
			     void **this_prologue_cache)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  bfd_byte sigreturn[2];

  if (target_read_memory (pc, sigreturn, 2))
    return 0;

  if (sigreturn[0] != op_svc)
    return 0;

  if (sigreturn[1] != 119 /* sigreturn */
      && sigreturn[1] != 173 /* rt_sigreturn */)
    return 0;

  return 1;
}

static const struct frame_unwind s390_sigtramp_frame_unwind = {
  SIGTRAMP_FRAME,
  default_frame_unwind_stop_reason,
  s390_sigtramp_frame_this_id,
  s390_sigtramp_frame_prev_register,
  NULL,
  s390_sigtramp_frame_sniffer
};

/* Frame unwind.  */

static void
s390_backchain_frame_unwind_cache (struct frame_info *this_frame,
				   struct s390_unwind_cache *info)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  int word_size = gdbarch_ptr_bit (gdbarch) / 8;
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  CORE_ADDR backchain;
  ULONGEST reg;
  LONGEST sp, tmp;
  int i;

  /* Set up ABI call-saved/call-clobbered registers.  */
  for (i = 0; i < S390_NUM_REGS; i++)
    if (!s390_register_call_saved (gdbarch, i))
      trad_frame_set_unknown (info->saved_regs, i);

  /* CC is always call-clobbered.  */
  trad_frame_set_unknown (info->saved_regs, S390_PSWM_REGNUM);

  /* Get the backchain.  */
  reg = get_frame_register_unsigned (this_frame, S390_SP_REGNUM);
  if (!safe_read_memory_integer (reg, word_size, byte_order, &tmp))
    tmp = 0;
  backchain = (CORE_ADDR) tmp;

  /* A zero backchain terminates the frame chain.  As additional
     sanity check, let's verify that the spill slot for SP in the
     save area pointed to by the backchain in fact links back to
     the save area.  */
  if (backchain != 0
      && safe_read_memory_integer (backchain + 15*word_size,
				   word_size, byte_order, &sp)
      && (CORE_ADDR)sp == backchain)
    {
      /* We don't know which registers were saved, but it will have
	 to be at least %r14 and %r15.  This will allow us to continue
	 unwinding, but other prev-frame registers may be incorrect ...  */
      info->saved_regs[S390_SP_REGNUM].addr = backchain + 15*word_size;
      info->saved_regs[S390_RETADDR_REGNUM].addr = backchain + 14*word_size;

      /* Function return will set PC to %r14.  */
      info->saved_regs[S390_PSWA_REGNUM]
	= info->saved_regs[S390_RETADDR_REGNUM];

      /* We use the current value of the frame register as local_base,
	 and the top of the register save area as frame_base.  */
      info->frame_base = backchain + 16*word_size + 32;
      info->local_base = reg;
    }

  info->func = get_frame_pc (this_frame);
}

static struct s390_unwind_cache *
s390_frame_unwind_cache (struct frame_info *this_frame,
			 void **this_prologue_cache)
{
  struct s390_unwind_cache *info;

  if (*this_prologue_cache)
    return (struct s390_unwind_cache *) *this_prologue_cache;

  info = FRAME_OBSTACK_ZALLOC (struct s390_unwind_cache);
  *this_prologue_cache = info;
  info->saved_regs = trad_frame_alloc_saved_regs (this_frame);
  info->func = -1;
  info->frame_base = -1;
  info->local_base = -1;

  TRY
    {
      /* Try to use prologue analysis to fill the unwind cache.
	 If this fails, fall back to reading the stack backchain.  */
      if (!s390_prologue_frame_unwind_cache (this_frame, info))
	s390_backchain_frame_unwind_cache (this_frame, info);
    }
  CATCH (ex, RETURN_MASK_ERROR)
    {
      if (ex.error != NOT_AVAILABLE_ERROR)
	throw_exception (ex);
    }
  END_CATCH

  return info;
}

static void
s390_frame_this_id (struct frame_info *this_frame,
		    void **this_prologue_cache,
		    struct frame_id *this_id)
{
  struct s390_unwind_cache *info
    = s390_frame_unwind_cache (this_frame, this_prologue_cache);

  if (info->frame_base == -1)
    {
      if (info->func != -1)
	*this_id = frame_id_build_unavailable_stack (info->func);
      return;
    }

  *this_id = frame_id_build (info->frame_base, info->func);
}

static struct value *
s390_frame_prev_register (struct frame_info *this_frame,
			  void **this_prologue_cache, int regnum)
{
  struct s390_unwind_cache *info
    = s390_frame_unwind_cache (this_frame, this_prologue_cache);

  return s390_trad_frame_prev_register (this_frame, info->saved_regs, regnum);
}

static const struct frame_unwind s390_frame_unwind = {
  NORMAL_FRAME,
  default_frame_unwind_stop_reason,
  s390_frame_this_id,
  s390_frame_prev_register,
  NULL,
  default_frame_sniffer
};

/* Frame base handling.  */

static CORE_ADDR
s390_frame_base_address (struct frame_info *this_frame, void **this_cache)
{
  struct s390_unwind_cache *info
    = s390_frame_unwind_cache (this_frame, this_cache);
  return info->frame_base;
}

static CORE_ADDR
s390_local_base_address (struct frame_info *this_frame, void **this_cache)
{
  struct s390_unwind_cache *info
    = s390_frame_unwind_cache (this_frame, this_cache);
  return info->local_base;
}

static const struct frame_base s390_frame_base = {
  &s390_frame_unwind,
  s390_frame_base_address,
  s390_local_base_address,
  s390_local_base_address
};

static CORE_ADDR
s390_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  ULONGEST pc;
  pc = frame_unwind_register_unsigned (next_frame, tdep->pc_regnum);
  return gdbarch_addr_bits_remove (gdbarch, pc);
}

static CORE_ADDR
s390_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST sp;
  sp = frame_unwind_register_unsigned (next_frame, S390_SP_REGNUM);
  return gdbarch_addr_bits_remove (gdbarch, sp);
}


/* Displaced stepping.  */

/* Return true if INSN is a non-branch RIL-b or RIL-c format
   instruction.  */

static int
is_non_branch_ril (gdb_byte *insn)
{
  gdb_byte op1 = insn[0];

  if (op1 == 0xc4)
    {
      gdb_byte op2 = insn[1] & 0x0f;

      switch (op2)
	{
	case 0x02: /* llhrl */
	case 0x04: /* lghrl */
	case 0x05: /* lhrl */
	case 0x06: /* llghrl */
	case 0x07: /* sthrl */
	case 0x08: /* lgrl */
	case 0x0b: /* stgrl */
	case 0x0c: /* lgfrl */
	case 0x0d: /* lrl */
	case 0x0e: /* llgfrl */
	case 0x0f: /* strl */
	  return 1;
	}
    }
  else if (op1 == 0xc6)
    {
      gdb_byte op2 = insn[1] & 0x0f;

      switch (op2)
	{
	case 0x00: /* exrl */
	case 0x02: /* pfdrl */
	case 0x04: /* cghrl */
	case 0x05: /* chrl */
	case 0x06: /* clghrl */
	case 0x07: /* clhrl */
	case 0x08: /* cgrl */
	case 0x0a: /* clgrl */
	case 0x0c: /* cgfrl */
	case 0x0d: /* crl */
	case 0x0e: /* clgfrl */
	case 0x0f: /* clrl */
	  return 1;
	}
    }

  return 0;
}

/* Implementation of gdbarch_displaced_step_copy_insn.  */

static struct displaced_step_closure *
s390_displaced_step_copy_insn (struct gdbarch *gdbarch,
			       CORE_ADDR from, CORE_ADDR to,
			       struct regcache *regs)
{
  size_t len = gdbarch_max_insn_length (gdbarch);
  gdb_byte *buf = (gdb_byte *) xmalloc (len);
  struct cleanup *old_chain = make_cleanup (xfree, buf);

  read_memory (from, buf, len);

  /* Adjust the displacement field of PC-relative RIL instructions,
     except branches.  The latter are handled in the fixup hook.  */
  if (is_non_branch_ril (buf))
    {
      LONGEST offset;

      offset = extract_signed_integer (buf + 2, 4, BFD_ENDIAN_BIG);
      offset = (from - to + offset * 2) / 2;

      /* If the instruction is too far from the jump pad, punt.  This
	 will usually happen with instructions in shared libraries.
	 We could probably support these by rewriting them to be
	 absolute or fully emulating them.  */
      if (offset < INT32_MIN || offset > INT32_MAX)
	{
	  /* Let the core fall back to stepping over the breakpoint
	     in-line.  */
	  if (debug_displaced)
	    {
	      fprintf_unfiltered (gdb_stdlog,
				  "displaced: can't displaced step "
				  "RIL instruction: offset %s out of range\n",
				  plongest (offset));
	    }
	  do_cleanups (old_chain);
	  return NULL;
	}

      store_signed_integer (buf + 2, 4, BFD_ENDIAN_BIG, offset);
    }

  write_memory (to, buf, len);

  if (debug_displaced)
    {
      fprintf_unfiltered (gdb_stdlog, "displaced: copy %s->%s: ",
			  paddress (gdbarch, from), paddress (gdbarch, to));
      displaced_step_dump_bytes (gdb_stdlog, buf, len);
    }

  discard_cleanups (old_chain);
  return (struct displaced_step_closure *) buf;
}

/* Fix up the state of registers and memory after having single-stepped
   a displaced instruction.  */

static void
s390_displaced_step_fixup (struct gdbarch *gdbarch,
			   struct displaced_step_closure *closure,
			   CORE_ADDR from, CORE_ADDR to,
			   struct regcache *regs)
{
  /* Our closure is a copy of the instruction.  */
  gdb_byte *insn = (gdb_byte *) closure;
  static int s390_instrlen[] = { 2, 4, 4, 6 };
  int insnlen = s390_instrlen[insn[0] >> 6];

  /* Fields for various kinds of instructions.  */
  unsigned int b2, r1, r2, x2, r3;
  int i2, d2;

  /* Get current PC and addressing mode bit.  */
  CORE_ADDR pc = regcache_read_pc (regs);
  ULONGEST amode = 0;

  if (register_size (gdbarch, S390_PSWA_REGNUM) == 4)
    {
      regcache_cooked_read_unsigned (regs, S390_PSWA_REGNUM, &amode);
      amode &= 0x80000000;
    }

  if (debug_displaced)
    fprintf_unfiltered (gdb_stdlog,
			"displaced: (s390) fixup (%s, %s) pc %s len %d amode 0x%x\n",
			paddress (gdbarch, from), paddress (gdbarch, to),
			paddress (gdbarch, pc), insnlen, (int) amode);

  /* Handle absolute branch and save instructions.  */
  if (is_rr (insn, op_basr, &r1, &r2)
      || is_rx (insn, op_bas, &r1, &d2, &x2, &b2))
    {
      /* Recompute saved return address in R1.  */
      regcache_cooked_write_unsigned (regs, S390_R0_REGNUM + r1,
				      amode | (from + insnlen));
    }

  /* Handle absolute branch instructions.  */
  else if (is_rr (insn, op_bcr, &r1, &r2)
	   || is_rx (insn, op_bc, &r1, &d2, &x2, &b2)
	   || is_rr (insn, op_bctr, &r1, &r2)
	   || is_rre (insn, op_bctgr, &r1, &r2)
	   || is_rx (insn, op_bct, &r1, &d2, &x2, &b2)
	   || is_rxy (insn, op1_bctg, op2_brctg, &r1, &d2, &x2, &b2)
	   || is_rs (insn, op_bxh, &r1, &r3, &d2, &b2)
	   || is_rsy (insn, op1_bxhg, op2_bxhg, &r1, &r3, &d2, &b2)
	   || is_rs (insn, op_bxle, &r1, &r3, &d2, &b2)
	   || is_rsy (insn, op1_bxleg, op2_bxleg, &r1, &r3, &d2, &b2))
    {
      /* Update PC iff branch was *not* taken.  */
      if (pc == to + insnlen)
	regcache_write_pc (regs, from + insnlen);
    }

  /* Handle PC-relative branch and save instructions.  */
  else if (is_ri (insn, op1_bras, op2_bras, &r1, &i2)
	   || is_ril (insn, op1_brasl, op2_brasl, &r1, &i2))
    {
      /* Update PC.  */
      regcache_write_pc (regs, pc - to + from);
      /* Recompute saved return address in R1.  */
      regcache_cooked_write_unsigned (regs, S390_R0_REGNUM + r1,
				      amode | (from + insnlen));
    }

  /* Handle PC-relative branch instructions.  */
  else if (is_ri (insn, op1_brc, op2_brc, &r1, &i2)
	   || is_ril (insn, op1_brcl, op2_brcl, &r1, &i2)
	   || is_ri (insn, op1_brct, op2_brct, &r1, &i2)
	   || is_ri (insn, op1_brctg, op2_brctg, &r1, &i2)
	   || is_rsi (insn, op_brxh, &r1, &r3, &i2)
	   || is_rie (insn, op1_brxhg, op2_brxhg, &r1, &r3, &i2)
	   || is_rsi (insn, op_brxle, &r1, &r3, &i2)
	   || is_rie (insn, op1_brxlg, op2_brxlg, &r1, &r3, &i2))
    {
      /* Update PC.  */
      regcache_write_pc (regs, pc - to + from);
    }

  /* Handle LOAD ADDRESS RELATIVE LONG.  */
  else if (is_ril (insn, op1_larl, op2_larl, &r1, &i2))
    {
      /* Update PC.  */
      regcache_write_pc (regs, from + insnlen);
      /* Recompute output address in R1.  */
      regcache_cooked_write_unsigned (regs, S390_R0_REGNUM + r1,
				      amode | (from + i2 * 2));
    }

  /* If we executed a breakpoint instruction, point PC right back at it.  */
  else if (insn[0] == 0x0 && insn[1] == 0x1)
    regcache_write_pc (regs, from);

  /* For any other insn, PC points right after the original instruction.  */
  else
    regcache_write_pc (regs, from + insnlen);

  if (debug_displaced)
    fprintf_unfiltered (gdb_stdlog,
			"displaced: (s390) pc is now %s\n",
			paddress (gdbarch, regcache_read_pc (regs)));
}

static int
s390_displaced_step_hw_singlestep (struct gdbarch *gdbarch,
				   struct displaced_step_closure *closure)
{
  return 1;
}


/* Address handling.  */

static CORE_ADDR
s390_addr_bits_remove (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  return addr & 0x7fffffff;
}

static int
s390_address_class_type_flags (int byte_size, int dwarf2_addr_class)
{
  if (byte_size == 4)
    return TYPE_INSTANCE_FLAG_ADDRESS_CLASS_1;
  else
    return 0;
}

static const char *
s390_address_class_type_flags_to_name (struct gdbarch *gdbarch, int type_flags)
{
  if (type_flags & TYPE_INSTANCE_FLAG_ADDRESS_CLASS_1)
    return "mode32";
  else
    return NULL;
}

static int
s390_address_class_name_to_type_flags (struct gdbarch *gdbarch,
				       const char *name,
				       int *type_flags_ptr)
{
  if (strcmp (name, "mode32") == 0)
    {
      *type_flags_ptr = TYPE_INSTANCE_FLAG_ADDRESS_CLASS_1;
      return 1;
    }
  else
    return 0;
}


/* Set up gdbarch struct.  */

static struct gdbarch *
s390_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  const struct target_desc *tdesc = info.target_desc;
  struct tdesc_arch_data *tdesc_data = NULL;
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  enum s390_abi_kind tdep_abi;
  enum s390_vector_abi_kind vector_abi;
  int have_upper = 0;
  int have_linux_v1 = 0;
  int have_linux_v2 = 0;
  int have_tdb = 0;
  int have_vx = 0;
  int first_pseudo_reg, last_pseudo_reg;
  static const char *const stap_register_prefixes[] = { "%", NULL };
  static const char *const stap_register_indirection_prefixes[] = { "(",
								    NULL };
  static const char *const stap_register_indirection_suffixes[] = { ")",
								    NULL };

  /* Default ABI and register size.  */
  switch (info.bfd_arch_info->mach)
    {
    case bfd_mach_s390_31:
      tdep_abi = ABI_LINUX_S390;
      break;

    case bfd_mach_s390_64:
      tdep_abi = ABI_LINUX_ZSERIES;
      break;

    default:
      return NULL;
    }

  /* Use default target description if none provided by the target.  */
  if (!tdesc_has_registers (tdesc))
    {
      if (tdep_abi == ABI_LINUX_S390)
	tdesc = tdesc_s390_linux32;
      else
	tdesc = tdesc_s390x_linux64;
    }

  /* Check any target description for validity.  */
  if (tdesc_has_registers (tdesc))
    {
      static const char *const gprs[] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
	"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
      };
      static const char *const fprs[] = {
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
	"f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15"
      };
      static const char *const acrs[] = {
	"acr0", "acr1", "acr2", "acr3", "acr4", "acr5", "acr6", "acr7",
	"acr8", "acr9", "acr10", "acr11", "acr12", "acr13", "acr14", "acr15"
      };
      static const char *const gprs_lower[] = {
	"r0l", "r1l", "r2l", "r3l", "r4l", "r5l", "r6l", "r7l",
	"r8l", "r9l", "r10l", "r11l", "r12l", "r13l", "r14l", "r15l"
      };
      static const char *const gprs_upper[] = {
	"r0h", "r1h", "r2h", "r3h", "r4h", "r5h", "r6h", "r7h",
	"r8h", "r9h", "r10h", "r11h", "r12h", "r13h", "r14h", "r15h"
      };
      static const char *const tdb_regs[] = {
	"tdb0", "tac", "tct", "atia",
	"tr0", "tr1", "tr2", "tr3", "tr4", "tr5", "tr6", "tr7",
	"tr8", "tr9", "tr10", "tr11", "tr12", "tr13", "tr14", "tr15"
      };
      static const char *const vxrs_low[] = {
	"v0l", "v1l", "v2l", "v3l", "v4l", "v5l", "v6l", "v7l", "v8l",
	"v9l", "v10l", "v11l", "v12l", "v13l", "v14l", "v15l",
      };
      static const char *const vxrs_high[] = {
	"v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24",
	"v25", "v26", "v27", "v28", "v29", "v30", "v31",
      };
      static const char *const cr[] = {
	"cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7",
	"cr8", "cr9", "cr10", "cr11", "cr12", "cr13", "cr14", "cr15"
      };
      const struct tdesc_feature *feature;
      int i, valid_p = 1;

      feature = tdesc_find_feature (tdesc, "org.gnu.gdb.s390.core");
      if (feature == NULL)
	return NULL;

      tdesc_data = tdesc_data_alloc ();

      valid_p &= tdesc_numbered_register (feature, tdesc_data,
					  S390_PSWM_REGNUM, "pswm");
      valid_p &= tdesc_numbered_register (feature, tdesc_data,
					  S390_PSWA_REGNUM, "pswa");

      if (tdesc_unnumbered_register (feature, "r0"))
	{
	  for (i = 0; i < 16; i++)
	    valid_p &= tdesc_numbered_register (feature, tdesc_data,
						S390_R0_REGNUM + i, gprs[i]);
	}
      else
	{
	  have_upper = 1;

	  for (i = 0; i < 16; i++)
	    valid_p &= tdesc_numbered_register (feature, tdesc_data,
						S390_R0_REGNUM + i,
						gprs_lower[i]);
	  for (i = 0; i < 16; i++)
	    valid_p &= tdesc_numbered_register (feature, tdesc_data,
						S390_R0_UPPER_REGNUM + i,
						gprs_upper[i]);
	}

      feature = tdesc_find_feature (tdesc, "org.gnu.gdb.s390.fpr");
      if (feature == NULL)
	{
	  tdesc_data_cleanup (tdesc_data);
	  return NULL;
	}

      valid_p &= tdesc_numbered_register (feature, tdesc_data,
					  S390_FPC_REGNUM, "fpc");
      for (i = 0; i < 16; i++)
	valid_p &= tdesc_numbered_register (feature, tdesc_data,
					    S390_F0_REGNUM + i, fprs[i]);

      feature = tdesc_find_feature (tdesc, "org.gnu.gdb.s390.acr");
      if (feature == NULL)
	{
	  tdesc_data_cleanup (tdesc_data);
	  return NULL;
	}

      for (i = 0; i < 16; i++)
	valid_p &= tdesc_numbered_register (feature, tdesc_data,
					    S390_A0_REGNUM + i, acrs[i]);

      /* Optional GNU/Linux-specific "registers".  */
      feature = tdesc_find_feature (tdesc, "org.gnu.gdb.s390.linux");
      if (feature)
	{
	  tdesc_numbered_register (feature, tdesc_data,
				   S390_ORIG_R2_REGNUM, "orig_r2");

	  if (tdesc_numbered_register (feature, tdesc_data,
				       S390_LAST_BREAK_REGNUM, "last_break"))
	    have_linux_v1 = 1;

	  if (tdesc_numbered_register (feature, tdesc_data,
				       S390_SYSTEM_CALL_REGNUM, "system_call"))
	    have_linux_v2 = 1;

	  if (have_linux_v2 > have_linux_v1)
	    valid_p = 0;
	}

      /* Transaction diagnostic block.  */
      feature = tdesc_find_feature (tdesc, "org.gnu.gdb.s390.tdb");
      if (feature)
	{
	  for (i = 0; i < ARRAY_SIZE (tdb_regs); i++)
	    valid_p &= tdesc_numbered_register (feature, tdesc_data,
						S390_TDB_DWORD0_REGNUM + i,
						tdb_regs[i]);
	  have_tdb = 1;
	}

      /* Vector registers.  */
      feature = tdesc_find_feature (tdesc, "org.gnu.gdb.s390.vx");
      if (feature)
	{
	  for (i = 0; i < 16; i++)
	    valid_p &= tdesc_numbered_register (feature, tdesc_data,
						S390_V0_LOWER_REGNUM + i,
						vxrs_low[i]);
	  for (i = 0; i < 16; i++)
	    valid_p &= tdesc_numbered_register (feature, tdesc_data,
						S390_V16_REGNUM + i,
						vxrs_high[i]);
	  have_vx = 1;
	}

      if (!valid_p)
	{
	  tdesc_data_cleanup (tdesc_data);
	  return NULL;
	}
    }

  /* Determine vector ABI.  */
  vector_abi = S390_VECTOR_ABI_NONE;
#ifdef HAVE_ELF
  if (have_vx
      && info.abfd != NULL
      && info.abfd->format == bfd_object
      && bfd_get_flavour (info.abfd) == bfd_target_elf_flavour
      && bfd_elf_get_obj_attr_int (info.abfd, OBJ_ATTR_GNU,
				   Tag_GNU_S390_ABI_Vector) == 2)
    vector_abi = S390_VECTOR_ABI_128;
#endif

  /* Find a candidate among extant architectures.  */
  for (arches = gdbarch_list_lookup_by_info (arches, &info);
       arches != NULL;
       arches = gdbarch_list_lookup_by_info (arches->next, &info))
    {
      tdep = gdbarch_tdep (arches->gdbarch);
      if (!tdep)
	continue;
      if (tdep->abi != tdep_abi)
	continue;
      if (tdep->vector_abi != vector_abi)
	continue;
      if ((tdep->gpr_full_regnum != -1) != have_upper)
	continue;
      if (tdesc_data != NULL)
	tdesc_data_cleanup (tdesc_data);

      return arches->gdbarch;
    }

  /* Otherwise create a new gdbarch for the specified machine type.  */
  tdep = XCNEW (struct gdbarch_tdep);
  tdep->abi = tdep_abi;
  tdep->vector_abi = vector_abi;
  tdep->have_linux_v1 = have_linux_v1;
  tdep->have_linux_v2 = have_linux_v2;
  tdep->have_tdb = have_tdb;
  gdbarch = gdbarch_alloc (&info, tdep);

  set_gdbarch_believe_pcc_promotion (gdbarch, 0);
  set_gdbarch_char_signed (gdbarch, 0);

  /* S/390 GNU/Linux uses either 64-bit or 128-bit long doubles.
     We can safely let them default to 128-bit, since the debug info
     will give the size of type actually used in each case.  */
  set_gdbarch_long_double_bit (gdbarch, 128);
  set_gdbarch_long_double_format (gdbarch, floatformats_ia64_quad);

  /* Amount PC must be decremented by after a breakpoint.  This is
     often the number of bytes returned by gdbarch_breakpoint_from_pc but not
     always.  */
  set_gdbarch_decr_pc_after_break (gdbarch, 2);

  /* Breakpoints.  */
  set_gdbarch_breakpoint_kind_from_pc (gdbarch, s390_breakpoint::kind_from_pc);
  set_gdbarch_sw_breakpoint_from_kind (gdbarch, s390_breakpoint::bp_from_kind);
  set_gdbarch_software_single_step (gdbarch, s390_software_single_step);
  set_gdbarch_skip_prologue (gdbarch, s390_skip_prologue);

  /* Registers.  */
  set_gdbarch_num_regs (gdbarch, S390_NUM_REGS);
  set_gdbarch_sp_regnum (gdbarch, S390_SP_REGNUM);
  set_gdbarch_fp0_regnum (gdbarch, S390_F0_REGNUM);
  set_gdbarch_register_name (gdbarch, s390_register_name);
  set_gdbarch_cannot_store_register (gdbarch, s390_cannot_store_register);
  set_gdbarch_write_pc (gdbarch, s390_write_pc);
  set_gdbarch_stab_reg_to_regnum (gdbarch, s390_dwarf_reg_to_regnum);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, s390_dwarf_reg_to_regnum);
  set_gdbarch_value_from_register (gdbarch, s390_value_from_register);
  set_gdbarch_core_read_description (gdbarch, s390_core_read_description);
  set_gdbarch_iterate_over_regset_sections (gdbarch,
					    s390_iterate_over_regset_sections);

  /* Pseudo registers.  */
  set_tdesc_pseudo_register_name (gdbarch, s390_pseudo_register_name);
  set_gdbarch_pseudo_register_read (gdbarch, s390_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, s390_pseudo_register_write);
  set_tdesc_pseudo_register_type (gdbarch, s390_pseudo_register_type);
  set_tdesc_pseudo_register_reggroup_p (gdbarch,
					s390_pseudo_register_reggroup_p);
  set_gdbarch_ax_pseudo_register_collect (gdbarch,
					  s390_ax_pseudo_register_collect);
  set_gdbarch_ax_pseudo_register_push_stack (gdbarch,
					     s390_ax_pseudo_register_push_stack);
  set_gdbarch_gen_return_address (gdbarch, s390_gen_return_address);
  tdesc_use_registers (gdbarch, tdesc, tdesc_data);

  /* Assign pseudo register numbers.  */
  first_pseudo_reg = gdbarch_num_regs (gdbarch);
  last_pseudo_reg = first_pseudo_reg;
  tdep->gpr_full_regnum = -1;
  if (have_upper)
    {
      tdep->gpr_full_regnum = last_pseudo_reg;
      last_pseudo_reg += 16;
    }
  tdep->v0_full_regnum = -1;
  if (have_vx)
    {
      tdep->v0_full_regnum = last_pseudo_reg;
      last_pseudo_reg += 16;
    }
  tdep->pc_regnum = last_pseudo_reg++;
  tdep->cc_regnum = last_pseudo_reg++;
  set_gdbarch_pc_regnum (gdbarch, tdep->pc_regnum);
  set_gdbarch_num_pseudo_regs (gdbarch, last_pseudo_reg - first_pseudo_reg);

  /* Inferior function calls.  */
  set_gdbarch_push_dummy_call (gdbarch, s390_push_dummy_call);
  set_gdbarch_dummy_id (gdbarch, s390_dummy_id);
  set_gdbarch_frame_align (gdbarch, s390_frame_align);
  set_gdbarch_return_value (gdbarch, s390_return_value);

  /* Frame handling.  */
  /* Stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_stack_frame_destroyed_p (gdbarch, s390_stack_frame_destroyed_p);
  dwarf2_frame_set_init_reg (gdbarch, s390_dwarf2_frame_init_reg);
  dwarf2_frame_set_adjust_regnum (gdbarch, s390_adjust_frame_regnum);
  dwarf2_append_unwinders (gdbarch);
  frame_base_append_sniffer (gdbarch, dwarf2_frame_base_sniffer);
  frame_unwind_append_unwinder (gdbarch, &s390_stub_frame_unwind);
  frame_unwind_append_unwinder (gdbarch, &s390_sigtramp_frame_unwind);
  frame_unwind_append_unwinder (gdbarch, &s390_frame_unwind);
  frame_base_set_default (gdbarch, &s390_frame_base);
  set_gdbarch_unwind_pc (gdbarch, s390_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, s390_unwind_sp);

  /* Displaced stepping.  */
  set_gdbarch_displaced_step_copy_insn (gdbarch,
					s390_displaced_step_copy_insn);
  set_gdbarch_displaced_step_fixup (gdbarch, s390_displaced_step_fixup);
  set_gdbarch_displaced_step_free_closure (gdbarch,
					   simple_displaced_step_free_closure);
  set_gdbarch_displaced_step_location (gdbarch, linux_displaced_step_location);
  set_gdbarch_displaced_step_hw_singlestep (gdbarch,
					    s390_displaced_step_hw_singlestep);
  set_gdbarch_max_insn_length (gdbarch, S390_MAX_INSTR_SIZE);

  switch (tdep->abi)
    {
    case ABI_LINUX_S390:
      set_gdbarch_addr_bits_remove (gdbarch, s390_addr_bits_remove);
      break;

    case ABI_LINUX_ZSERIES:
      set_gdbarch_long_bit (gdbarch, 64);
      set_gdbarch_long_long_bit (gdbarch, 64);
      set_gdbarch_ptr_bit (gdbarch, 64);
      set_gdbarch_address_class_type_flags (gdbarch,
					    s390_address_class_type_flags);
      set_gdbarch_address_class_type_flags_to_name (gdbarch,
						    s390_address_class_type_flags_to_name);
      set_gdbarch_address_class_name_to_type_flags (gdbarch,
						    s390_address_class_name_to_type_flags);
      break;
    }

  set_gdbarch_print_insn (gdbarch, print_insn_s390);

  /* SystemTap functions.  */
  set_gdbarch_stap_register_prefixes (gdbarch, stap_register_prefixes);
  set_gdbarch_stap_register_indirection_prefixes (gdbarch,
						  stap_register_indirection_prefixes);
  set_gdbarch_stap_register_indirection_suffixes (gdbarch,
						  stap_register_indirection_suffixes);

  set_gdbarch_disassembler_options (gdbarch, &s390_disassembler_options);
  set_gdbarch_valid_disassembler_options (gdbarch,
					  disassembler_options_s390 ());

  /* Note that GNU/Linux is the only OS supported on this platform.  */
  s390_gdbarch_linux_init (info, gdbarch);

  return gdbarch;
}


extern initialize_file_ftype _initialize_s390_tdep; /* -Wmissing-prototypes */

void
_initialize_s390_tdep (void)
{
  /* Hook us into the gdbarch mechanism.  */
  register_gdbarch_init (bfd_arch_s390, s390_gdbarch_init);
}
