/* Target-dependent code for GDB, the GNU debugger.
   Copyright 2001 Free Software Foundation, Inc.
   Contributed by D.J. Barrow (djbarrow@de.ibm.com,barrow_dj@yahoo.com)
   for IBM Deutschland Entwicklung GmbH, IBM Corporation.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#define S390_TDEP		/* for special macros in tm-s390.h */
#include <defs.h>
#include "arch-utils.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "target.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "symfile.h"
#include "objfiles.h"
#include "tm.h"
#include "../bfd/bfd.h"
#include "floatformat.h"
#include "regcache.h"





/* Number of bytes of storage in the actual machine representation
   for register N. 
   Note that the unsigned cast here forces the result of the
   subtraction to very high positive values if N < S390_FP0_REGNUM */
int
s390_register_raw_size (int reg_nr)
{
  return ((unsigned) reg_nr - S390_FP0_REGNUM) <
    S390_NUM_FPRS ? S390_FPR_SIZE : 4;
}

int
s390x_register_raw_size (int reg_nr)
{
  return (reg_nr == S390_FPC_REGNUM)
    || (reg_nr >= S390_FIRST_ACR && reg_nr <= S390_LAST_ACR) ? 4 : 8;
}

int
s390_cannot_fetch_register (int regno)
{
  return (regno >= S390_FIRST_CR && regno < (S390_FIRST_CR + 9)) ||
    (regno >= (S390_FIRST_CR + 12) && regno <= S390_LAST_CR);
}

int
s390_register_byte (int reg_nr)
{
  if (reg_nr <= S390_GP_LAST_REGNUM)
    return reg_nr * S390_GPR_SIZE;
  if (reg_nr <= S390_LAST_ACR)
    return S390_ACR0_OFFSET + (((reg_nr) - S390_FIRST_ACR) * S390_ACR_SIZE);
  if (reg_nr <= S390_LAST_CR)
    return S390_CR0_OFFSET + (((reg_nr) - S390_FIRST_CR) * S390_CR_SIZE);
  if (reg_nr == S390_FPC_REGNUM)
    return S390_FPC_OFFSET;
  else
    return S390_FP0_OFFSET + (((reg_nr) - S390_FP0_REGNUM) * S390_FPR_SIZE);
}

#ifndef GDBSERVER
#define S390_MAX_INSTR_SIZE (6)
#define S390_SYSCALL_OPCODE (0x0a)
#define S390_SYSCALL_SIZE   (2)
#define S390_SIGCONTEXT_SREGS_OFFSET (8)
#define S390X_SIGCONTEXT_SREGS_OFFSET (8)
#define S390_SIGREGS_FP0_OFFSET       (144)
#define S390X_SIGREGS_FP0_OFFSET      (216)
#define S390_UC_MCONTEXT_OFFSET    (256)
#define S390X_UC_MCONTEXT_OFFSET   (344)
#define S390_STACK_FRAME_OVERHEAD  (GDB_TARGET_IS_ESAME ? 160:96)
#define S390_SIGNAL_FRAMESIZE  (GDB_TARGET_IS_ESAME ? 160:96)
#define s390_NR_sigreturn          119
#define s390_NR_rt_sigreturn       173



struct frame_extra_info
{
  int initialised;
  int good_prologue;
  CORE_ADDR function_start;
  CORE_ADDR skip_prologue_function_start;
  CORE_ADDR saved_pc_valid;
  CORE_ADDR saved_pc;
  CORE_ADDR sig_fixed_saved_pc_valid;
  CORE_ADDR sig_fixed_saved_pc;
  CORE_ADDR frame_pointer_saved_pc;	/* frame pointer needed for alloca */
  CORE_ADDR stack_bought;	/* amount we decrement the stack pointer by */
  CORE_ADDR sigcontext;
};


static CORE_ADDR s390_frame_saved_pc_nofix (struct frame_info *fi);

int
s390_readinstruction (bfd_byte instr[], CORE_ADDR at,
		      struct disassemble_info *info)
{
  int instrlen;

  static int s390_instrlen[] = {
    2,
    4,
    4,
    6
  };
  if ((*info->read_memory_func) (at, &instr[0], 2, info))
    return -1;
  instrlen = s390_instrlen[instr[0] >> 6];
  if ((*info->read_memory_func) (at + 2, &instr[2], instrlen - 2, info))
    return -1;
  return instrlen;
}

static void
s390_memset_extra_info (struct frame_extra_info *fextra_info)
{
  memset (fextra_info, 0, sizeof (struct frame_extra_info));
}



char *
s390_register_name (int reg_nr)
{
  static char *register_names[] = {
    "pswm", "pswa",
    "gpr0", "gpr1", "gpr2", "gpr3", "gpr4", "gpr5", "gpr6", "gpr7",
    "gpr8", "gpr9", "gpr10", "gpr11", "gpr12", "gpr13", "gpr14", "gpr15",
    "acr0", "acr1", "acr2", "acr3", "acr4", "acr5", "acr6", "acr7",
    "acr8", "acr9", "acr10", "acr11", "acr12", "acr13", "acr14", "acr15",
    "cr0", "cr1", "cr2", "cr3", "cr4", "cr5", "cr6", "cr7",
    "cr8", "cr9", "cr10", "cr11", "cr12", "cr13", "cr14", "cr15",
    "fpc",
    "fpr0", "fpr1", "fpr2", "fpr3", "fpr4", "fpr5", "fpr6", "fpr7",
    "fpr8", "fpr9", "fpr10", "fpr11", "fpr12", "fpr13", "fpr14", "fpr15"
  };

  if (reg_nr >= S390_LAST_REGNUM)
    return NULL;
  return register_names[reg_nr];
}




int
s390_stab_reg_to_regnum (int regno)
{
  return regno >= 64 ? S390_PSWM_REGNUM - 64 :
    regno >= 48 ? S390_FIRST_ACR - 48 :
    regno >= 32 ? S390_FIRST_CR - 32 :
    regno <= 15 ? (regno + 2) :
    S390_FP0_REGNUM + ((regno - 16) & 8) + (((regno - 16) & 3) << 1) +
    (((regno - 16) & 4) >> 2);
}



/* s390_get_frame_info based on Hartmuts
   prologue definition in
   gcc-2.8.1/config/l390/linux.c 

   It reads one instruction at a time & based on whether
   it looks like prologue code or not it makes a decision on
   whether the prologue is over, there are various state machines
   in the code to determine if the prologue code is possilby valid.
   
   This is done to hopefully allow the code survive minor revs of
   calling conventions.

 */

int
s390_get_frame_info (CORE_ADDR pc, struct frame_extra_info *fextra_info,
		     struct frame_info *fi, int init_extra_info)
{
#define CONST_POOL_REGIDX 13
#define GOT_REGIDX        12
  bfd_byte instr[S390_MAX_INSTR_SIZE];
  CORE_ADDR test_pc = pc, test_pc2;
  CORE_ADDR orig_sp = 0, save_reg_addr = 0, *saved_regs = NULL;
  int valid_prologue, good_prologue = 0;
  int gprs_saved[S390_NUM_GPRS];
  int fprs_saved[S390_NUM_FPRS];
  int regidx, instrlen;
  int save_link_regidx, subtract_sp_regidx;
  int const_pool_state, save_link_state, got_state;
  int frame_pointer_found, varargs_state;
  int loop_cnt, gdb_gpr_store, gdb_fpr_store;
  int frame_pointer_regidx = 0xf;
  int offset, expected_offset;
  int err = 0;
  disassemble_info info;
  const_pool_state = save_link_state = got_state = varargs_state = 0;
  frame_pointer_found = 0;
  memset (gprs_saved, 0, sizeof (gprs_saved));
  memset (fprs_saved, 0, sizeof (fprs_saved));
  info.read_memory_func = dis_asm_read_memory;

  save_link_regidx = subtract_sp_regidx = 0;
  if (fextra_info)
    {
      if (fi && fi->frame)
	{
	  orig_sp = fi->frame + fextra_info->stack_bought;
	  saved_regs = fi->saved_regs;
	}
      if (init_extra_info || !fextra_info->initialised)
	{
	  s390_memset_extra_info (fextra_info);
	  fextra_info->function_start = pc;
	  fextra_info->initialised = 1;
	}
    }
  instrlen = 0;
  do
    {
      valid_prologue = 0;
      test_pc += instrlen;
      /* add the previous instruction len */
      instrlen = s390_readinstruction (instr, test_pc, &info);
      if (instrlen < 0)
	{
	  good_prologue = 0;
	  err = -1;
	  break;
	}
      /* We probably are in a glibc syscall */
      if (instr[0] == S390_SYSCALL_OPCODE && test_pc == pc)
	{
	  good_prologue = 1;
	  if (saved_regs && fextra_info && fi->next && fi->next->extra_info
	      && fi->next->extra_info->sigcontext)
	    {
	      /* We are backtracing from a signal handler */
	      save_reg_addr = fi->next->extra_info->sigcontext +
		REGISTER_BYTE (S390_GP0_REGNUM);
	      for (regidx = 0; regidx < S390_NUM_GPRS; regidx++)
		{
		  saved_regs[S390_GP0_REGNUM + regidx] = save_reg_addr;
		  save_reg_addr += S390_GPR_SIZE;
		}
	      save_reg_addr = fi->next->extra_info->sigcontext +
		(GDB_TARGET_IS_ESAME ? S390X_SIGREGS_FP0_OFFSET :
		 S390_SIGREGS_FP0_OFFSET);
	      for (regidx = 0; regidx < S390_NUM_FPRS; regidx++)
		{
		  saved_regs[S390_FP0_REGNUM + regidx] = save_reg_addr;
		  save_reg_addr += S390_FPR_SIZE;
		}
	    }
	  break;
	}
      if (save_link_state == 0)
	{
	  /* check for a stack relative STMG or STM */
	  if (((GDB_TARGET_IS_ESAME &&
		((instr[0] == 0xeb) && (instr[5] == 0x24))) ||
	       (instr[0] == 0x90)) && ((instr[2] >> 4) == 0xf))
	    {
	      regidx = (instr[1] >> 4);
	      if (regidx < 6)
		varargs_state = 1;
	      offset = ((instr[2] & 0xf) << 8) + instr[3];
	      expected_offset =
		S390_GPR6_STACK_OFFSET + (S390_GPR_SIZE * (regidx - 6));
	      if (offset != expected_offset)
		{
		  good_prologue = 0;
		  break;
		}
	      if (saved_regs)
		save_reg_addr = orig_sp + offset;
	      for (; regidx <= (instr[1] & 0xf); regidx++)
		{
		  if (gprs_saved[regidx])
		    {
		      good_prologue = 0;
		      break;
		    }
		  good_prologue = 1;
		  gprs_saved[regidx] = 1;
		  if (saved_regs)
		    {
		      saved_regs[S390_GP0_REGNUM + regidx] = save_reg_addr;
		      save_reg_addr += S390_GPR_SIZE;
		    }
		}
	      valid_prologue = 1;
	      continue;
	    }
	}
      /* check for a stack relative STG or ST */
      if ((save_link_state == 0 || save_link_state == 3) &&
	  ((GDB_TARGET_IS_ESAME &&
	    ((instr[0] == 0xe3) && (instr[5] == 0x24))) ||
	   (instr[0] == 0x50)) && ((instr[2] >> 4) == 0xf))
	{
	  regidx = instr[1] >> 4;
	  offset = ((instr[2] & 0xf) << 8) + instr[3];
	  if (offset == 0)
	    {
	      if (save_link_state == 3 && regidx == save_link_regidx)
		{
		  save_link_state = 4;
		  valid_prologue = 1;
		  continue;
		}
	      else
		break;
	    }
	  if (regidx < 6)
	    varargs_state = 1;
	  expected_offset =
	    S390_GPR6_STACK_OFFSET + (S390_GPR_SIZE * (regidx - 6));
	  if (offset != expected_offset)
	    {
	      good_prologue = 0;
	      break;
	    }
	  if (gprs_saved[regidx])
	    {
	      good_prologue = 0;
	      break;
	    }
	  good_prologue = 1;
	  gprs_saved[regidx] = 1;
	  if (saved_regs)
	    {
	      save_reg_addr = orig_sp + offset;
	      saved_regs[S390_GP0_REGNUM + regidx] = save_reg_addr;
	    }
	  valid_prologue = 1;
	  continue;
	}

      /* check for STD */
      if (instr[0] == 0x60 && (instr[2] >> 4) == 0xf)
	{
	  regidx = instr[1] >> 4;
	  if (regidx == 0 || regidx == 2)
	    varargs_state = 1;
	  if (fprs_saved[regidx])
	    {
	      good_prologue = 0;
	      break;
	    }
	  fprs_saved[regidx] = 1;
	  if (saved_regs)
	    {
	      save_reg_addr = orig_sp + (((instr[2] & 0xf) << 8) + instr[3]);
	      saved_regs[S390_FP0_REGNUM + regidx] = save_reg_addr;
	    }
	  valid_prologue = 1;
	  continue;
	}


      if (const_pool_state == 0)
	{

	  if (GDB_TARGET_IS_ESAME)
	    {
	      /* Check for larl CONST_POOL_REGIDX,offset on ESAME */
	      if ((instr[0] == 0xc0)
		  && (instr[1] == (CONST_POOL_REGIDX << 4)))
		{
		  const_pool_state = 2;
		  valid_prologue = 1;
		  continue;
		}
	    }
	  else
	    {
	      /* Check for BASR gpr13,gpr0 used to load constant pool pointer to r13 in old compiler */
	      if (instr[0] == 0xd && (instr[1] & 0xf) == 0
		  && ((instr[1] >> 4) == CONST_POOL_REGIDX))
		{
		  const_pool_state = 1;
		  valid_prologue = 1;
		  continue;
		}
	    }
	  /* Check for new fangled bras %r13,newpc to load new constant pool */
	  /* embedded in code, older pre abi compilers also emitted this stuff.  */
	  if ((instr[0] == 0xa7) && ((instr[1] & 0xf) == 0x5) &&
	      ((instr[1] >> 4) == CONST_POOL_REGIDX)
	      && ((instr[2] & 0x80) == 0))
	    {
	      const_pool_state = 2;
	      test_pc +=
		(((((instr[2] & 0xf) << 8) + instr[3]) << 1) - instrlen);
	      valid_prologue = 1;
	      continue;
	    }
	}
      /* Check for AGHI or AHI CONST_POOL_REGIDX,val */
      if (const_pool_state == 1 && (instr[0] == 0xa7) &&
	  ((GDB_TARGET_IS_ESAME &&
	    (instr[1] == ((CONST_POOL_REGIDX << 4) | 0xb))) ||
	   (instr[1] == ((CONST_POOL_REGIDX << 4) | 0xa))))
	{
	  const_pool_state = 2;
	  valid_prologue = 1;
	  continue;
	}
      /* Check for LGR or LR gprx,15 */
      if ((GDB_TARGET_IS_ESAME &&
	   instr[0] == 0xb9 && instr[1] == 0x04 && (instr[3] & 0xf) == 0xf) ||
	  (instr[0] == 0x18 && (instr[1] & 0xf) == 0xf))
	{
	  if (GDB_TARGET_IS_ESAME)
	    regidx = instr[3] >> 4;
	  else
	    regidx = instr[1] >> 4;
	  if (save_link_state == 0 && regidx != 0xb)
	    {
	      /* Almost defintely code for
	         decrementing the stack pointer 
	         ( i.e. a non leaf function 
	         or else leaf with locals ) */
	      save_link_regidx = regidx;
	      save_link_state = 1;
	      valid_prologue = 1;
	      continue;
	    }
	  /* We use this frame pointer for alloca
	     unfortunately we need to assume its gpr11
	     otherwise we would need a smarter prologue
	     walker. */
	  if (!frame_pointer_found && regidx == 0xb)
	    {
	      frame_pointer_regidx = 0xb;
	      frame_pointer_found = 1;
	      if (fextra_info)
		fextra_info->frame_pointer_saved_pc = test_pc;
	      valid_prologue = 1;
	      continue;
	    }
	}
      /* Check for AHI or AGHI gpr15,val */
      if (save_link_state == 1 && (instr[0] == 0xa7) &&
	  ((GDB_TARGET_IS_ESAME && (instr[1] == 0xfb)) || (instr[1] == 0xfa)))
	{
	  if (fextra_info)
	    fextra_info->stack_bought =
	      -extract_signed_integer (&instr[2], 2);
	  save_link_state = 3;
	  valid_prologue = 1;
	  continue;
	}
      /* Alternatively check for the complex construction for
         buying more than 32k of stack
         BRAS gprx,.+8
         long vals    %r15,0(%gprx)  gprx currently r1 */
      if ((save_link_state == 1) && (instr[0] == 0xa7)
	  && ((instr[1] & 0xf) == 0x5) && (instr[2] == 0)
	  && (instr[3] == 0x4) && ((instr[1] >> 4) != CONST_POOL_REGIDX))
	{
	  subtract_sp_regidx = instr[1] >> 4;
	  save_link_state = 2;
	  if (fextra_info)
	    target_read_memory (test_pc + instrlen,
				(char *) &fextra_info->stack_bought,
				sizeof (fextra_info->stack_bought));
	  test_pc += 4;
	  valid_prologue = 1;
	  continue;
	}
      if (save_link_state == 2 && instr[0] == 0x5b
	  && instr[1] == 0xf0 &&
	  instr[2] == (subtract_sp_regidx << 4) && instr[3] == 0)
	{
	  save_link_state = 3;
	  valid_prologue = 1;
	  continue;
	}
      /* check for LA gprx,offset(15) used for varargs */
      if ((instr[0] == 0x41) && ((instr[2] >> 4) == 0xf) &&
	  ((instr[1] & 0xf) == 0))
	{
	  /* some code uses gpr7 to point to outgoing args */
	  if (((instr[1] >> 4) == 7) && (save_link_state == 0) &&
	      ((instr[2] & 0xf) == 0)
	      && (instr[3] == S390_STACK_FRAME_OVERHEAD))
	    {
	      valid_prologue = 1;
	      continue;
	    }
	  if (varargs_state == 1)
	    {
	      varargs_state = 2;
	      valid_prologue = 1;
	      continue;
	    }
	}
      /* Check for a GOT load */

      if (GDB_TARGET_IS_ESAME)
	{
	  /* Check for larl  GOT_REGIDX, on ESAME */
	  if ((got_state == 0) && (instr[0] == 0xc0)
	      && (instr[1] == (GOT_REGIDX << 4)))
	    {
	      got_state = 2;
	      valid_prologue = 1;
	      continue;
	    }
	}
      else
	{
	  /* check for l GOT_REGIDX,x(CONST_POOL_REGIDX) */
	  if (got_state == 0 && const_pool_state == 2 && instr[0] == 0x58
	      && (instr[2] == (CONST_POOL_REGIDX << 4))
	      && ((instr[1] >> 4) == GOT_REGIDX))
	    {
	      got_state == 1;
	      valid_prologue = 1;
	      continue;
	    }
	  /* Check for subsequent ar got_regidx,basr_regidx */
	  if (got_state == 1 && instr[0] == 0x1a &&
	      instr[1] == ((GOT_REGIDX << 4) | CONST_POOL_REGIDX))
	    {
	      got_state = 2;
	      valid_prologue = 1;
	      continue;
	    }
	}
    }
  while (valid_prologue && good_prologue);
  if (good_prologue)
    {
      good_prologue = (((got_state == 0) || (got_state == 2)) &&
		       ((const_pool_state == 0) || (const_pool_state == 2)) &&
		       ((save_link_state == 0) || (save_link_state == 4)) &&
		       ((varargs_state == 0) || (varargs_state == 2)));
    }
  if (fextra_info)
    {
      fextra_info->good_prologue = good_prologue;
      fextra_info->skip_prologue_function_start =
	(good_prologue ? test_pc : pc);
    }
  return err;
}


int
s390_check_function_end (CORE_ADDR pc)
{
  bfd_byte instr[S390_MAX_INSTR_SIZE];
  disassemble_info info;
  int regidx, instrlen;

  info.read_memory_func = dis_asm_read_memory;
  instrlen = s390_readinstruction (instr, pc, &info);
  if (instrlen < 0)
    return -1;
  /* check for BR */
  if (instrlen != 2 || instr[0] != 07 || (instr[1] >> 4) != 0xf)
    return 0;
  regidx = instr[1] & 0xf;
  /* Check for LMG or LG */
  instrlen =
    s390_readinstruction (instr, pc - (GDB_TARGET_IS_ESAME ? 6 : 4), &info);
  if (instrlen < 0)
    return -1;
  if (GDB_TARGET_IS_ESAME)
    {

      if (instrlen != 6 || instr[0] != 0xeb || instr[5] != 0x4)
	return 0;
    }
  else if (instrlen != 4 || instr[0] != 0x98)
    {
      return 0;
    }
  if ((instr[2] >> 4) != 0xf)
    return 0;
  if (regidx == 14)
    return 1;
  instrlen = s390_readinstruction (instr, pc - (GDB_TARGET_IS_ESAME ? 12 : 8),
				   &info);
  if (instrlen < 0)
    return -1;
  if (GDB_TARGET_IS_ESAME)
    {
      /* Check for LG */
      if (instrlen != 6 || instr[0] != 0xe3 || instr[5] != 0x4)
	return 0;
    }
  else
    {
      /* Check for L */
      if (instrlen != 4 || instr[0] != 0x58)
	return 0;
    }
  if (instr[2] >> 4 != 0xf)
    return 0;
  if (instr[1] >> 4 != regidx)
    return 0;
  return 1;
}

static CORE_ADDR
s390_sniff_pc_function_start (CORE_ADDR pc, struct frame_info *fi)
{
  CORE_ADDR function_start, test_function_start;
  int loop_cnt, err, function_end;
  struct frame_extra_info fextra_info;
  function_start = get_pc_function_start (pc);

  if (function_start == 0)
    {
      test_function_start = pc;
      if (test_function_start & 1)
	return 0;		/* This has to be bogus */
      loop_cnt = 0;
      do
	{

	  err =
	    s390_get_frame_info (test_function_start, &fextra_info, fi, 1);
	  loop_cnt++;
	  test_function_start -= 2;
	  function_end = s390_check_function_end (test_function_start);
	}
      while (!(function_end == 1 || err || loop_cnt >= 4096 ||
	       (fextra_info.good_prologue)));
      if (fextra_info.good_prologue)
	function_start = fextra_info.function_start;
      else if (function_end == 1)
	function_start = test_function_start;
    }
  return function_start;
}



CORE_ADDR
s390_function_start (struct frame_info *fi)
{
  CORE_ADDR function_start = 0;

  if (fi->extra_info && fi->extra_info->initialised)
    function_start = fi->extra_info->function_start;
  else if (fi->pc)
    function_start = get_pc_function_start (fi->pc);
  return function_start;
}




int
s390_frameless_function_invocation (struct frame_info *fi)
{
  struct frame_extra_info fextra_info, *fextra_info_ptr;
  int frameless = 0;

  if (fi->next == NULL)		/* no may be frameless */
    {
      if (fi->extra_info)
	fextra_info_ptr = fi->extra_info;
      else
	{
	  fextra_info_ptr = &fextra_info;
	  s390_get_frame_info (s390_sniff_pc_function_start (fi->pc, fi),
			       fextra_info_ptr, fi, 1);
	}
      frameless = ((fextra_info_ptr->stack_bought == 0));
    }
  return frameless;

}


static int
s390_is_sigreturn (CORE_ADDR pc, struct frame_info *sighandler_fi,
		   CORE_ADDR *sregs, CORE_ADDR *sigcaller_pc)
{
  bfd_byte instr[S390_MAX_INSTR_SIZE];
  disassemble_info info;
  int instrlen;
  CORE_ADDR scontext;
  int retval = 0;
  CORE_ADDR orig_sp;
  CORE_ADDR temp_sregs;

  scontext = temp_sregs = 0;

  info.read_memory_func = dis_asm_read_memory;
  instrlen = s390_readinstruction (instr, pc, &info);
  if (sigcaller_pc)
    *sigcaller_pc = 0;
  if (((instrlen == S390_SYSCALL_SIZE) &&
       (instr[0] == S390_SYSCALL_OPCODE)) &&
      ((instr[1] == s390_NR_sigreturn) || (instr[1] == s390_NR_rt_sigreturn)))
    {
      if (sighandler_fi)
	{
	  if (s390_frameless_function_invocation (sighandler_fi))
	    orig_sp = sighandler_fi->frame;
	  else
	    orig_sp = ADDR_BITS_REMOVE ((CORE_ADDR)
					read_memory_integer (sighandler_fi->
							     frame,
							     S390_GPR_SIZE));
	  if (orig_sp && sigcaller_pc)
	    {
	      scontext = orig_sp + S390_SIGNAL_FRAMESIZE;
	      if (pc == scontext && instr[1] == s390_NR_rt_sigreturn)
		{
		  /* We got a new style rt_signal */
		  /* get address of read ucontext->uc_mcontext */
		  temp_sregs = orig_sp + (GDB_TARGET_IS_ESAME ?
					  S390X_UC_MCONTEXT_OFFSET :
					  S390_UC_MCONTEXT_OFFSET);
		}
	      else
		{
		  /* read sigcontext->sregs */
		  temp_sregs = ADDR_BITS_REMOVE ((CORE_ADDR)
						 read_memory_integer (scontext
								      +
								      (GDB_TARGET_IS_ESAME
								       ?
								       S390X_SIGCONTEXT_SREGS_OFFSET
								       :
								       S390_SIGCONTEXT_SREGS_OFFSET),
								      S390_GPR_SIZE));

		}
	      /* read sigregs->psw.addr */
	      *sigcaller_pc =
		ADDR_BITS_REMOVE ((CORE_ADDR)
				  read_memory_integer (temp_sregs +
						       REGISTER_BYTE
						       (S390_PC_REGNUM),
						       S390_PSW_ADDR_SIZE));
	    }
	}
      retval = 1;
    }
  if (sregs)
    *sregs = temp_sregs;
  return retval;
}

/*
  We need to do something better here but this will keep us out of trouble
  for the moment.
  For some reason the blockframe.c calls us with fi->next->fromleaf
  so this seems of little use to us. */
void
s390_init_frame_pc_first (int next_fromleaf, struct frame_info *fi)
{
  CORE_ADDR sigcaller_pc;

  fi->pc = 0;
  if (next_fromleaf)
    {
      fi->pc = ADDR_BITS_REMOVE (read_register (S390_RETADDR_REGNUM));
      /* fix signal handlers */
    }
  else if (fi->next && fi->next->pc)
    fi->pc = s390_frame_saved_pc_nofix (fi->next);
  if (fi->pc && fi->next && fi->next->frame &&
      s390_is_sigreturn (fi->pc, fi->next, NULL, &sigcaller_pc))
    {
      fi->pc = sigcaller_pc;
    }

}

void
s390_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{
  fi->extra_info = frame_obstack_alloc (sizeof (struct frame_extra_info));
  if (fi->pc)
    s390_get_frame_info (s390_sniff_pc_function_start (fi->pc, fi),
			 fi->extra_info, fi, 1);
  else
    s390_memset_extra_info (fi->extra_info);
}

/* If saved registers of frame FI are not known yet, read and cache them.
   &FEXTRA_INFOP contains struct frame_extra_info; TDATAP can be NULL,
   in which case the framedata are read.  */

void
s390_frame_init_saved_regs (struct frame_info *fi)
{

  int quick;

  if (fi->saved_regs == NULL)
    {
      /* zalloc memsets the saved regs */
      frame_saved_regs_zalloc (fi);
      if (fi->pc)
	{
	  quick = (fi->extra_info && fi->extra_info->initialised
		   && fi->extra_info->good_prologue);
	  s390_get_frame_info (quick ? fi->extra_info->function_start :
			       s390_sniff_pc_function_start (fi->pc, fi),
			       fi->extra_info, fi, !quick);
	}
    }
}



CORE_ADDR
s390_frame_args_address (struct frame_info *fi)
{

  /* Apparently gdb already knows gdb_args_offset itself */
  return fi->frame;
}


static CORE_ADDR
s390_frame_saved_pc_nofix (struct frame_info *fi)
{
  if (fi->extra_info && fi->extra_info->saved_pc_valid)
    return fi->extra_info->saved_pc;
  s390_frame_init_saved_regs (fi);
  if (fi->extra_info)
    {
      fi->extra_info->saved_pc_valid = 1;
      if (fi->extra_info->good_prologue)
	{
	  if (fi->saved_regs[S390_RETADDR_REGNUM])
	    {
	      return (fi->extra_info->saved_pc =
		      ADDR_BITS_REMOVE (read_memory_integer
					(fi->saved_regs[S390_RETADDR_REGNUM],
					 S390_GPR_SIZE)));
	    }
	}
    }
  return 0;
}

CORE_ADDR
s390_frame_saved_pc (struct frame_info *fi)
{
  CORE_ADDR saved_pc = 0, sig_pc;

  if (fi->extra_info && fi->extra_info->sig_fixed_saved_pc_valid)
    return fi->extra_info->sig_fixed_saved_pc;
  saved_pc = s390_frame_saved_pc_nofix (fi);

  if (fi->extra_info)
    {
      fi->extra_info->sig_fixed_saved_pc_valid = 1;
      if (saved_pc)
	{
	  if (s390_is_sigreturn (saved_pc, fi, NULL, &sig_pc))
	    saved_pc = sig_pc;
	}
      fi->extra_info->sig_fixed_saved_pc = saved_pc;
    }
  return saved_pc;
}




/* We want backtraces out of signal handlers so we don't
   set thisframe->signal_handler_caller to 1 */

CORE_ADDR
s390_frame_chain (struct frame_info *thisframe)
{
  CORE_ADDR prev_fp = 0;

  if (thisframe->prev && thisframe->prev->frame)
    prev_fp = thisframe->prev->frame;
  else
    {
      int sigreturn = 0;
      CORE_ADDR sregs = 0;
      struct frame_extra_info prev_fextra_info;

      memset (&prev_fextra_info, 0, sizeof (prev_fextra_info));
      if (thisframe->pc)
	{
	  CORE_ADDR saved_pc, sig_pc;

	  saved_pc = s390_frame_saved_pc_nofix (thisframe);
	  if (saved_pc)
	    {
	      if ((sigreturn =
		   s390_is_sigreturn (saved_pc, thisframe, &sregs, &sig_pc)))
		saved_pc = sig_pc;
	      s390_get_frame_info (s390_sniff_pc_function_start
				   (saved_pc, NULL), &prev_fextra_info, NULL,
				   1);
	    }
	}
      if (sigreturn)
	{
	  /* read sigregs,regs.gprs[11 or 15] */
	  prev_fp = read_memory_integer (sregs +
					 REGISTER_BYTE (S390_GP0_REGNUM +
							(prev_fextra_info.
							 frame_pointer_saved_pc
							 ? 11 : 15)),
					 S390_GPR_SIZE);
	  thisframe->extra_info->sigcontext = sregs;
	}
      else
	{
	  if (thisframe->saved_regs)
	    {

	      int regno;

	      regno =
		((prev_fextra_info.frame_pointer_saved_pc
		  && thisframe->
		  saved_regs[S390_FRAME_REGNUM]) ? S390_FRAME_REGNUM :
		 S390_SP_REGNUM);
	      if (thisframe->saved_regs[regno])
		prev_fp =
		  read_memory_integer (thisframe->saved_regs[regno],
				       S390_GPR_SIZE);
	    }
	}
    }
  return ADDR_BITS_REMOVE (prev_fp);
}

/*
  Whether struct frame_extra_info is actually needed I'll have to figure
  out as our frames are similar to rs6000 there is a possibility
  i386 dosen't need it. */



/* a given return value in `regbuf' with a type `valtype', extract and copy its
   value into `valbuf' */
void
s390_extract_return_value (struct type *valtype, char *regbuf, char *valbuf)
{
  /* floats and doubles are returned in fpr0. fpr's have a size of 8 bytes.
     We need to truncate the return value into float size (4 byte) if
     necessary. */
  int len = TYPE_LENGTH (valtype);

  if (TYPE_CODE (valtype) == TYPE_CODE_FLT)
    {
      if (len > (TARGET_FLOAT_BIT >> 3))
	memcpy (valbuf, &regbuf[REGISTER_BYTE (S390_FP0_REGNUM)], len);
      else
	{
	  /* float */
	  DOUBLEST val;

	  floatformat_to_doublest (&floatformat_ieee_double_big,
				   &regbuf[REGISTER_BYTE (S390_FP0_REGNUM)],
				   &val);
	  store_floating (valbuf, len, val);
	}
    }
  else
    {
      int offset = 0;
      /* return value is copied starting from r2. */
      if (TYPE_LENGTH (valtype) < S390_GPR_SIZE)
	offset = S390_GPR_SIZE - TYPE_LENGTH (valtype);
      memcpy (valbuf,
	      regbuf + REGISTER_BYTE (S390_GP0_REGNUM + 2) + offset,
	      TYPE_LENGTH (valtype));
    }
}


static char *
s390_promote_integer_argument (struct type *valtype, char *valbuf,
			       char *reg_buff, int *arglen)
{
  char *value = valbuf;
  int len = TYPE_LENGTH (valtype);

  if (len < S390_GPR_SIZE)
    {
      /* We need to upgrade this value to a register to pass it correctly */
      int idx, diff = S390_GPR_SIZE - len, negative =
	(!TYPE_UNSIGNED (valtype) && value[0] & 0x80);
      for (idx = 0; idx < S390_GPR_SIZE; idx++)
	{
	  reg_buff[idx] = (idx < diff ? (negative ? 0xff : 0x0) :
			   value[idx - diff]);
	}
      value = reg_buff;
      *arglen = S390_GPR_SIZE;
    }
  else
    {
      if (len & (S390_GPR_SIZE - 1))
	{
	  fprintf_unfiltered (gdb_stderr,
			      "s390_promote_integer_argument detected an argument not "
			      "a multiple of S390_GPR_SIZE & greater than S390_GPR_SIZE "
			      "we might not deal with this correctly.\n");
	}
      *arglen = len;
    }

  return (value);
}

void
s390_store_return_value (struct type *valtype, char *valbuf)
{
  int arglen;
  char *reg_buff = alloca (max (S390_FPR_SIZE, REGISTER_SIZE)), *value;

  if (TYPE_CODE (valtype) == TYPE_CODE_FLT)
    {
      DOUBLEST tempfloat = extract_floating (valbuf, TYPE_LENGTH (valtype));

      floatformat_from_doublest (&floatformat_ieee_double_big, &tempfloat,
				 reg_buff);
      write_register_bytes (REGISTER_BYTE (S390_FP0_REGNUM), reg_buff,
			    S390_FPR_SIZE);
    }
  else
    {
      value =
	s390_promote_integer_argument (valtype, valbuf, reg_buff, &arglen);
      /* Everything else is returned in GPR2 and up. */
      write_register_bytes (REGISTER_BYTE (S390_GP0_REGNUM + 2), value,
			    arglen);
    }
}
static int
gdb_print_insn_s390 (bfd_vma memaddr, disassemble_info * info)
{
  bfd_byte instrbuff[S390_MAX_INSTR_SIZE];
  int instrlen, cnt;

  instrlen = s390_readinstruction (instrbuff, (CORE_ADDR) memaddr, info);
  if (instrlen < 0)
    {
      (*info->memory_error_func) (instrlen, memaddr, info);
      return -1;
    }
  for (cnt = 0; cnt < instrlen; cnt++)
    info->fprintf_func (info->stream, "%02X ", instrbuff[cnt]);
  for (cnt = instrlen; cnt < S390_MAX_INSTR_SIZE; cnt++)
    info->fprintf_func (info->stream, "   ");
  instrlen = print_insn_s390 (memaddr, info);
  return instrlen;
}



/* Not the most efficent code in the world */
int
s390_fp_regnum ()
{
  int regno = S390_SP_REGNUM;
  struct frame_extra_info fextra_info;

  CORE_ADDR pc = ADDR_BITS_REMOVE (read_register (S390_PC_REGNUM));

  s390_get_frame_info (s390_sniff_pc_function_start (pc, NULL), &fextra_info,
		       NULL, 1);
  if (fextra_info.frame_pointer_saved_pc)
    regno = S390_FRAME_REGNUM;
  return regno;
}

CORE_ADDR
s390_read_fp ()
{
  return read_register (s390_fp_regnum ());
}


void
s390_write_fp (CORE_ADDR val)
{
  write_register (s390_fp_regnum (), val);
}


void
s390_push_dummy_frame ()
{
  CORE_ADDR orig_sp = read_register (S390_SP_REGNUM), new_sp;
  void *saved_regs = alloca (REGISTER_BYTES);

  new_sp = (orig_sp - (REGISTER_BYTES + S390_GPR_SIZE));
  read_register_bytes (0, (char *) saved_regs, REGISTER_BYTES);
  /* Use saved copy instead of orig_sp as this will have the correct endianness */
  write_memory (new_sp, (char *) saved_regs + REGISTER_BYTE (S390_SP_REGNUM),
		S390_GPR_SIZE);
  write_memory (new_sp + S390_GPR_SIZE, (char *) &saved_regs, REGISTER_BYTES);
  write_register (S390_SP_REGNUM, new_sp);
}

/* pop the innermost frame, go back to the caller.
    Used in `call_function_by_hand' to remove an artificial stack
     frame.  */
void
s390_pop_frame ()
{
  CORE_ADDR new_sp = read_register (S390_SP_REGNUM), orig_sp;
  void *saved_regs = alloca (REGISTER_BYTES);


  read_memory (new_sp + S390_GPR_SIZE, (char *) saved_regs, REGISTER_BYTES);
  write_register_bytes (0, (char *) &saved_regs, REGISTER_BYTES);
}

/* used by call function by hand
  struct_return indicates that this function returns a structure &
  therefore gpr2 stores a pointer to the structure to be returned as
  opposed to the first argument.
  Currently I haven't seen a TYPE_CODE_INT whose size wasn't 2^n or less
  than S390_GPR_SIZE this is good because I don't seem to have to worry
  about sign extending pushed arguments (i.e. a signed char currently
  comes into this code with a size of 4 ). */

CORE_ADDR
s390_push_arguments (int nargs, value_ptr *args, CORE_ADDR sp,
		     int struct_return, CORE_ADDR struct_addr)
{
  int num_float_args, num_gpr_args, orig_num_gpr_args, argno;
  int second_pass, len, arglen, gprs_required;
  CORE_ADDR outgoing_args_ptr, outgoing_args_space;
  value_ptr arg;
  struct type *type;
  int max_num_gpr_args = 5 - (struct_return ? 1 : 0);
  int arg0_regnum = S390_GP0_REGNUM + 2 + (struct_return ? 1 : 0);
  char *reg_buff = alloca (max (S390_FPR_SIZE, REGISTER_SIZE)), *value;

  for (second_pass = 0; second_pass <= 1; second_pass++)
    {
      if (second_pass)
	outgoing_args_ptr = sp + S390_STACK_FRAME_OVERHEAD;
      else
	outgoing_args_ptr = 0;
      num_float_args = 0;
      num_gpr_args = 0;
      for (argno = 0; argno < nargs; argno++)
	{
	  arg = args[argno];
	  type = check_typedef (VALUE_TYPE (arg));
	  len = TYPE_LENGTH (type);
	  if (TYPE_CODE (type) == TYPE_CODE_FLT)
	    {
	      int all_float_registers_used =
		num_float_args > (GDB_TARGET_IS_ESAME ? 3 : 1);

	      if (second_pass)
		{
		  DOUBLEST tempfloat =
		    extract_floating (VALUE_CONTENTS (arg), len);


		  floatformat_from_doublest (all_float_registers_used &&
					     len == (TARGET_FLOAT_BIT >> 3)
					     ? &floatformat_ieee_single_big
					     : &floatformat_ieee_double_big,
					     &tempfloat, reg_buff);
		  if (all_float_registers_used)
		    write_memory (outgoing_args_ptr, reg_buff, len);
		  else
		    write_register_bytes (REGISTER_BYTE ((S390_FP0_REGNUM)
							 +
							 (2 *
							  num_float_args)),
					  reg_buff, S390_FPR_SIZE);
		}
	      if (all_float_registers_used)
		outgoing_args_ptr += len;
	      num_float_args++;
	    }
	  else
	    {
	      gprs_required = ((len + (S390_GPR_SIZE - 1)) / S390_GPR_SIZE);

	      value =
		s390_promote_integer_argument (type, VALUE_CONTENTS (arg),
					       reg_buff, &arglen);

	      orig_num_gpr_args = num_gpr_args;
	      num_gpr_args += gprs_required;
	      if (num_gpr_args > max_num_gpr_args)
		{
		  if (second_pass)
		    write_memory (outgoing_args_ptr, value, arglen);
		  outgoing_args_ptr += arglen;
		}
	      else
		{
		  if (second_pass)
		    write_register_bytes (REGISTER_BYTE (arg0_regnum)
					  +
					  (orig_num_gpr_args * S390_GPR_SIZE),
					  value, arglen);
		}
	    }
	}
      if (!second_pass)
	{
	  outgoing_args_space = outgoing_args_ptr;
	  /* Align to 16 bytes because because I like alignment & 
	     some of the kernel code requires 8 byte stack alignment at least. */
	  sp = (sp - (S390_STACK_FRAME_OVERHEAD + outgoing_args_ptr)) & (-16);
	}

    }
  return sp;

}

void
s390_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs,
		     struct value **args, struct type *value_type,
		     int using_gcc)
{
  store_unsigned_integer (dummy + 4, REGISTER_SIZE, fun);
}


/* Return the GDB type object for the "standard" data type
   of data in register N.  */
struct type *
s390_register_virtual_type (int regno)
{
  return ((unsigned) regno - S390_FPC_REGNUM) <
    S390_NUM_FPRS ? builtin_type_double : builtin_type_int;
}


struct type *
s390x_register_virtual_type (int regno)
{
  return (regno == S390_FPC_REGNUM) ||
    (regno >= S390_FIRST_ACR && regno <= S390_LAST_ACR) ? builtin_type_int :
    (regno >= S390_FP0_REGNUM) ? builtin_type_double : builtin_type_long;
}



void
s390_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
{
  write_register (S390_GP0_REGNUM + 2, addr);
}



static unsigned char *
s390_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  static unsigned char breakpoint[] = { 0x0, 0x1 };

  *lenptr = sizeof (breakpoint);
  return breakpoint;
}

/* Advance PC across any function entry prologue instructions to reach some
   "real" code.  */
CORE_ADDR
s390_skip_prologue (CORE_ADDR pc)
{
  struct frame_extra_info fextra_info;

  s390_get_frame_info (pc, &fextra_info, NULL, 1);
  return fextra_info.skip_prologue_function_start;
}

/* pc_in_call_dummy_on stack may work for us must test this */
int
s390_pc_in_call_dummy (CORE_ADDR pc, CORE_ADDR sp, CORE_ADDR frame_address)
{
  return pc > sp && pc < (sp + 4096);
}

/* Immediately after a function call, return the saved pc.
   Can't go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.  */
CORE_ADDR
s390_saved_pc_after_call (struct frame_info *frame)
{
  return ADDR_BITS_REMOVE (read_register (S390_RETADDR_REGNUM));
}

static CORE_ADDR
s390_addr_bits_remove (CORE_ADDR addr)
{
  return (addr) & 0x7fffffff;
}


static CORE_ADDR
s390_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
{
  return sp;
}

struct gdbarch *
s390_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{

  /* instruction sequence for s390 call dummy is as follows
     bras %r1,.+8      ; 0xA7150004   
     long basraddr     ; 0x00000000
     l    %r1,0(%r1)   ; 0x58101000
     basr %r14,%r1     ; 0x0DE1
     breakpoint        ; 0x0001 */
  static LONGEST s390_call_dummy_words[] = { 0xA7150004, 0x00000000,
    0x58101000, 0x0DE10001
  };
  /* instruction sequence for esame call dummy is as follows
     bras %r1,.+12     ; 0xA7150006   
     long basraddr     ; 0x0000000000000000
     lg   %r1,0(%r1)   ; 0xE31010000004
     basr %r14,%r1     ; 0x0DE1
     breakpoint        ; 0x0001 */
  static LONGEST s390x_call_dummy_words[] = { 0xA715000600000000,
    0x00000000E3101000,
    0x00040DE100010000
  };
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  int elf_flags;

  /* First see if there is already a gdbarch that can satisfy the request.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  /* None found: is the request for a s390 architecture? */
  if (info.bfd_arch_info->arch != bfd_arch_s390)
    return NULL;		/* No; then it's not for us.  */

  /* Yes: create a new gdbarch for the specified machine type.  */
  gdbarch = gdbarch_alloc (&info, NULL);

  set_gdbarch_believe_pcc_promotion (gdbarch, 0);

  /* We don't define set_gdbarch_call_dummy_breakpoint_offset 
     as we already have a breakpoint inserted. */
  set_gdbarch_use_generic_dummy_frames (gdbarch, 0);

  set_gdbarch_call_dummy_location (gdbarch, ON_STACK);
  set_gdbarch_call_dummy_start_offset (gdbarch, 0);
  set_gdbarch_pc_in_call_dummy (gdbarch, s390_pc_in_call_dummy);
  set_gdbarch_frame_args_skip (gdbarch, 0);
  set_gdbarch_frame_args_address (gdbarch, s390_frame_args_address);
  set_gdbarch_frame_chain (gdbarch, s390_frame_chain);
  set_gdbarch_frame_init_saved_regs (gdbarch, s390_frame_init_saved_regs);
  set_gdbarch_frame_locals_address (gdbarch, s390_frame_args_address);
  /* We can't do this */
  set_gdbarch_frame_num_args (gdbarch, frame_num_args_unknown);
  set_gdbarch_store_struct_return (gdbarch, s390_store_struct_return);
  set_gdbarch_extract_return_value (gdbarch, s390_extract_return_value);
  set_gdbarch_store_return_value (gdbarch, s390_store_return_value);
  /* Amount PC must be decremented by after a breakpoint.
     This is often the number of bytes in BREAKPOINT
     but not always.  */
  set_gdbarch_decr_pc_after_break (gdbarch, 2);
  set_gdbarch_pop_frame (gdbarch, s390_pop_frame);
  set_gdbarch_push_dummy_frame (gdbarch, s390_push_dummy_frame);
  set_gdbarch_push_arguments (gdbarch, s390_push_arguments);
  set_gdbarch_ieee_float (gdbarch, 1);
  /* Stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  /* Offset from address of function to start of its code.
     Zero on most machines.  */
  set_gdbarch_function_start_offset (gdbarch, 0);
  set_gdbarch_max_register_raw_size (gdbarch, 8);
  set_gdbarch_max_register_virtual_size (gdbarch, 8);
  set_gdbarch_breakpoint_from_pc (gdbarch, s390_breakpoint_from_pc);
  set_gdbarch_skip_prologue (gdbarch, s390_skip_prologue);
  set_gdbarch_init_extra_frame_info (gdbarch, s390_init_extra_frame_info);
  set_gdbarch_init_frame_pc_first (gdbarch, s390_init_frame_pc_first);
  set_gdbarch_read_fp (gdbarch, s390_read_fp);
  set_gdbarch_write_fp (gdbarch, s390_write_fp);
  /* This function that tells us whether the function invocation represented
     by FI does not have a frame on the stack associated with it.  If it
     does not, FRAMELESS is set to 1, else 0.  */
  set_gdbarch_frameless_function_invocation (gdbarch,
					     s390_frameless_function_invocation);
  /* Return saved PC from a frame */
  set_gdbarch_frame_saved_pc (gdbarch, s390_frame_saved_pc);
  /* FRAME_CHAIN takes a frame's nominal address
     and produces the frame's chain-pointer. */
  set_gdbarch_frame_chain (gdbarch, s390_frame_chain);
  set_gdbarch_saved_pc_after_call (gdbarch, s390_saved_pc_after_call);
  set_gdbarch_register_byte (gdbarch, s390_register_byte);
  set_gdbarch_pc_regnum (gdbarch, S390_PC_REGNUM);
  set_gdbarch_sp_regnum (gdbarch, S390_SP_REGNUM);
  set_gdbarch_fp_regnum (gdbarch, S390_FP_REGNUM);
  set_gdbarch_fp0_regnum (gdbarch, S390_FP0_REGNUM);
  set_gdbarch_num_regs (gdbarch, S390_NUM_REGS);
  set_gdbarch_cannot_fetch_register (gdbarch, s390_cannot_fetch_register);
  set_gdbarch_cannot_store_register (gdbarch, s390_cannot_fetch_register);
  set_gdbarch_get_saved_register (gdbarch, generic_get_saved_register);
  set_gdbarch_use_struct_convention (gdbarch, generic_use_struct_convention);
  set_gdbarch_frame_chain_valid (gdbarch, file_frame_chain_valid);
  set_gdbarch_register_name (gdbarch, s390_register_name);
  set_gdbarch_stab_reg_to_regnum (gdbarch, s390_stab_reg_to_regnum);
  set_gdbarch_dwarf_reg_to_regnum (gdbarch, s390_stab_reg_to_regnum);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, s390_stab_reg_to_regnum);
  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

  /* Stuff below here wouldn't be required if gdbarch.sh was a little */
  /* more intelligent */
  set_gdbarch_call_dummy_breakpoint_offset_p (gdbarch, 0);
  set_gdbarch_call_dummy_p (gdbarch, 1);
  set_gdbarch_call_dummy_stack_adjust_p (gdbarch, 0);
  set_gdbarch_extract_struct_value_address (gdbarch, 0);
  set_gdbarch_fix_call_dummy (gdbarch, s390_fix_call_dummy);
#ifdef GDB_NM_FILE
  set_gdbarch_prepare_to_proceed (gdbarch, linuxthreads_prepare_to_proceed);
#endif
  set_gdbarch_push_return_address (gdbarch, s390_push_return_address);

  switch (info.bfd_arch_info->mach)
    {
    case bfd_mach_s390_esa:
      set_gdbarch_register_size (gdbarch, 4);
      set_gdbarch_call_dummy_length (gdbarch, 16);
      set_gdbarch_register_raw_size (gdbarch, s390_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, s390_register_raw_size);
      set_gdbarch_register_virtual_type (gdbarch, s390_register_virtual_type);

      set_gdbarch_addr_bits_remove (gdbarch, s390_addr_bits_remove);

      set_gdbarch_sizeof_call_dummy_words (gdbarch,
					   sizeof (s390_call_dummy_words));
      set_gdbarch_call_dummy_words (gdbarch, s390_call_dummy_words);
      set_gdbarch_register_bytes (gdbarch, S390_REGISTER_BYTES);
      break;
    case bfd_mach_s390_esame:
      set_gdbarch_register_size (gdbarch, 8);
      set_gdbarch_call_dummy_length (gdbarch, 22);
      set_gdbarch_register_raw_size (gdbarch, s390x_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, s390x_register_raw_size);
      set_gdbarch_register_virtual_type (gdbarch,
					 s390x_register_virtual_type);

      set_gdbarch_long_bit (gdbarch, 64);
      set_gdbarch_long_long_bit (gdbarch, 64);
      set_gdbarch_ptr_bit (gdbarch, 64);
      set_gdbarch_sizeof_call_dummy_words (gdbarch,
					   sizeof (s390x_call_dummy_words));
      set_gdbarch_call_dummy_words (gdbarch, s390x_call_dummy_words);
      set_gdbarch_register_bytes (gdbarch, S390X_REGISTER_BYTES);
      break;
    }

  return gdbarch;
}



void
_initialize_s390_tdep ()
{

  /* Hook us into the gdbarch mechanism.  */
  register_gdbarch_init (bfd_arch_s390, s390_gdbarch_init);
  if (!tm_print_insn)		/* Someone may have already set it */
    tm_print_insn = gdb_print_insn_s390;
}

#endif /* GDBSERVER */
