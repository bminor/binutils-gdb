/* Common target dependent code for GDB on ARM systems.

   Copyright 1988, 1989, 1991, 1992, 1993, 1995, 1996, 1998, 1999,
   2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <ctype.h>		/* XXX for isupper () */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "dis-asm.h"		/* For register styles. */
#include "regcache.h"
#include "doublest.h"
#include "value.h"
#include "arch-utils.h"
#include "osabi.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "objfiles.h"
#include "dwarf2-frame.h"
#include "observer.h"

#include "arm-tdep.h"
#include "gdb/sim-arm.h"

#include "elf-bfd.h"
#include "coff/internal.h"
#include "elf/arm.h"

#include "gdb_assert.h"

static int arm_debug;

/* Extra information which ARM uses to select the appropriate GDB
   architecture.  */

struct gdbarch_tdep_info
{
  /* See the descriptions of these fields in struct gdbarch_tdep.  */

  int target_has_iwmmxt_regs;
  int target_iwmmxt_regnum;
  int target_has_vfp_regs;
  int target_vfp_regnum;
};

/* Each OS has a different mechanism for accessing the various
   registers stored in the sigcontext structure.

   SIGCONTEXT_REGISTER_ADDRESS should be defined to the name (or
   function pointer) which may be used to determine the addresses
   of the various saved registers in the sigcontext structure.

   For the ARM target, there are three parameters to this function. 
   The first is the pc value of the frame under consideration, the
   second the stack pointer of this frame, and the last is the
   register number to fetch.  

   If the tm.h file does not define this macro, then it's assumed that
   no mechanism is needed and we define SIGCONTEXT_REGISTER_ADDRESS to
   be 0. 
   
   When it comes time to multi-arching this code, see the identically
   named machinery in ia64-tdep.c for an example of how it could be
   done.  It should not be necessary to modify the code below where
   this macro is used.  */

#ifdef SIGCONTEXT_REGISTER_ADDRESS
#ifndef SIGCONTEXT_REGISTER_ADDRESS_P
#define SIGCONTEXT_REGISTER_ADDRESS_P() 1
#endif
#else
#define SIGCONTEXT_REGISTER_ADDRESS(SP,PC,REG) 0
#define SIGCONTEXT_REGISTER_ADDRESS_P() 0
#endif

/* Macros for setting and testing a bit in a minimal symbol that marks
   it as Thumb function.  The MSB of the minimal symbol's "info" field
   is used for this purpose.

   MSYMBOL_SET_SPECIAL	Actually sets the "special" bit.
   MSYMBOL_IS_SPECIAL   Tests the "special" bit in a minimal symbol.  */

#define MSYMBOL_SET_SPECIAL(msym)					\
	MSYMBOL_INFO (msym) = (char *) (((long) MSYMBOL_INFO (msym))	\
					| 0x80000000)

#define MSYMBOL_IS_SPECIAL(msym)				\
	(((long) MSYMBOL_INFO (msym) & 0x80000000) != 0)

/* The list of available "set arm ..." and "show arm ..." commands.  */
static struct cmd_list_element *setarmcmdlist = NULL;
static struct cmd_list_element *showarmcmdlist = NULL;

/* The type of floating-point to use.  Keep this in sync with enum
   arm_float_model, and the help string in _initialize_arm_tdep.  */
static const char *fp_model_strings[] =
{
  "auto",
  "softfpa",
  "fpa",
  "softvfp",
  "vfp",
  NULL
};

/* A variable that can be configured by the user.  */
static enum arm_float_model arm_fp_model = ARM_FLOAT_AUTO;
static const char *current_fp_model = "auto";

/* The ABI to use.  Keep this in sync with arm_abi_kind.  */
static const char *arm_abi_strings[] =
{
  "auto",
  "APCS",
  "AAPCS",
  NULL
};

/* A variable that can be configured by the user.  */
static enum arm_abi_kind arm_abi_global = ARM_ABI_AUTO;
static const char *arm_abi_string = "auto";

/* Number of different reg name sets (options).  */
static int num_disassembly_options;

/* We have more registers than the disassembler as gdb can print the value
   of special registers as well.
   The general register names are overwritten by whatever is being used by
   the disassembler at the moment. We also adjust the case of cpsr and fps.  */

/* Initial value: Register names used in ARM's ISA documentation.  */
static char * arm_register_name_strings[] =
{"r0",  "r1",  "r2",  "r3",	/*  0  1  2  3 */
 "r4",  "r5",  "r6",  "r7",	/*  4  5  6  7 */
 "r8",  "r9",  "r10", "r11",	/*  8  9 10 11 */
 "r12", "sp",  "lr",  "pc",	/* 12 13 14 15 */
 "f0",  "f1",  "f2",  "f3",	/* 16 17 18 19 */
 "f4",  "f5",  "f6",  "f7",	/* 20 21 22 23 */
 "fps", "cpsr" };		/* 24 25       */
static char **arm_register_names = arm_register_name_strings;

/* Valid register name styles.  */
static const char **valid_disassembly_styles;

/* Disassembly style to use. Default to "std" register names.  */
static const char *disassembly_style;
/* Index to that option in the opcodes table.  */
static int current_option;

/* This is used to keep the bfd arch_info in sync with the disassembly
   style.  */
static void set_disassembly_style_sfunc(char *, int,
					 struct cmd_list_element *);
static void set_disassembly_style (void);

static void convert_from_extended (const struct floatformat *, const void *,
				   void *);
static void convert_to_extended (const struct floatformat *, void *,
				 const void *);

struct arm_prologue_cache
{
  /* The stack pointer at the time this frame was created; i.e. the
     caller's stack pointer when this function was called.  It is used
     to identify this frame.  */
  CORE_ADDR prev_sp;

  /* The frame base for this frame is just prev_sp + frame offset -
     frame size.  FRAMESIZE is the size of this stack frame, and
     FRAMEOFFSET if the initial offset from the stack pointer (this
     frame's stack pointer, not PREV_SP) to the frame base.  */

  int framesize;
  int frameoffset;

  /* The register used to hold the frame pointer for this frame.  */
  int framereg;

  /* Saved register offsets.  */
  struct trad_frame_saved_reg *saved_regs;
};

/* Addresses for calling Thumb functions have the bit 0 set.
   Here are some macros to test, set, or clear bit 0 of addresses.  */
#define IS_THUMB_ADDR(addr)	((addr) & 1)
#define MAKE_THUMB_ADDR(addr)	((addr) | 1)
#define UNMAKE_THUMB_ADDR(addr) ((addr) & ~1)

/* Support routines for bitfield extraction.  */

#define submask(x) ((1L << ((x) + 1)) - 1)
#define bit(obj,st) (((obj) >> (st)) & 1)
#define bits(obj,st,fn) (((obj) >> (st)) & submask ((fn) - (st)))
#define sbits(obj,st,fn) \
  ((long) (bits(obj,st,fn) | ((long) bit(obj,fn) * ~ submask (fn - st))))

/* Set to true if the 32-bit mode is in use.  */

int arm_apcs_32 = 1;

/* Return number of 1-bits in VAL.  */

static int
bitcount (unsigned long val)
{
  int nbits;
  for (nbits = 0; val != 0; nbits++)
    val &= val - 1;		/* delete rightmost 1-bit in val */
  return nbits;
}

/* Determine if the program counter specified in MEMADDR is in a Thumb
   function.  */

int
arm_pc_is_thumb (CORE_ADDR memaddr)
{
  struct minimal_symbol *sym;

  /* If bit 0 of the address is set, assume this is a Thumb address.  */
  if (IS_THUMB_ADDR (memaddr))
    return 1;

  /* Thumb functions have a "special" bit set in minimal symbols.  */
  sym = lookup_minimal_symbol_by_pc (memaddr);
  if (sym)
    {
      return (MSYMBOL_IS_SPECIAL (sym));
    }
  else
    {
      return 0;
    }
}

/* Remove useless bits from addresses in a running program.  */
static CORE_ADDR
arm_addr_bits_remove (CORE_ADDR val)
{
  if (arm_apcs_32)
    return (val & (arm_pc_is_thumb (val) ? 0xfffffffe : 0xfffffffc));
  else
    return (val & 0x03fffffc);
}

/* When reading symbols, we need to zap the low bit of the address,
   which may be set to 1 for Thumb functions.  */
static CORE_ADDR
arm_smash_text_address (CORE_ADDR val)
{
  return val & ~1;
}

/* Immediately after a function call, return the saved pc.  Can't
   always go through the frames for this because on some machines the
   new frame is not set up until the new function executes some
   instructions.  */

static CORE_ADDR
arm_saved_pc_after_call (struct frame_info *frame)
{
  return ADDR_BITS_REMOVE (read_register (ARM_LR_REGNUM));
}

/* Read a thumb instruction from memory. 16-bit instructions are returned in
   the lower half of the returned word. 32-bit instructions are returned with
   the first halfword at the most-significant end. If non-zero, the int
   pointed to by length is set to the instruction's length in bytes.  */

static unsigned long
thumb_get_insn (CORE_ADDR addr, int *length)
{
  unsigned long insn = read_memory_unsigned_integer (addr, 2);
  int len = 2;

  if ((insn & 0xf800) > 0xe000)
    {
      insn = (insn << 16) | read_memory_unsigned_integer (addr + 2, 2);
      len = 4;
    }

  if (length)
    *length = len;

  return insn;
}

/* Advance the PC across any function entry prologue instructions, apart
   from those which have been scheduled later into the function. This function
   handles Thumb and Thumb-2 prologues.
   We skip over instructions of the form:
   
     - push  OR  stmdb  OR  str rn,[sp,#-4]!  OR  fstmdb
     - mov r7, sp  OR  add r7, sp, #imm
     - sub.n sp, sp, #n  OR  sub.w sp, sp, #n
  
   That is, instructions which push callee-saved registers onto the stack,
   instructions to move SP to FP (if there is one) with a possible offset, and
   instructions to decrement the stack pointer.
 
   GCC usually optimises the frame pointer away at > -O0, so we don't require
   all the above to be present.  */

static CORE_ADDR
thumb_skip_prologue (CORE_ADDR pc, CORE_ADDR func_end)
{
  CORE_ADDR current_pc;

  for (current_pc = pc; current_pc < func_end && current_pc < pc+40;)
    {
      int length;
      unsigned long insn = thumb_get_insn (current_pc, &length);

      /* FIXME: stmdb must store two or more registers!  */
      if ((insn & 0xfffffe00) == 0xb400               /* push  */
	  || (insn & 0xffffa000) == 0xe92d0000        /* stmdb.w sp!,{...}  */
	  || (insn & 0xffff0fff) == 0xf84d0d04        /* str rn, [sp, #-4]!  */
	  || (insn & 0xffff0f00) == 0xed2d0b00)       /* fstmdb[xd] sp!,{..}  */
	;
      else if (insn == 0x466f)                        /* mov r7, sp  */
	;
      else if ((insn & 0xffffff00) == 0xaf00)         /* add r7, sp, #imm  */
	;
      else if ((insn & 0xffffff00) == 0xb000          /* add/sub sp, #n  */
	       || (insn & 0xfbef8f00) == 0xf1ad0d00)  /* sub.w sp, sp, #n  */
	;
      else
	break;

      current_pc += length;
    }
  return current_pc;
}

/* Advance the PC across any function entry prologue instructions to
   reach some "real" code.

   The APCS (ARM Procedure Call Standard) defines the following
   prologue:

   mov          ip, sp
   [stmfd       sp!, {a1,a2,a3,a4}]
   stmfd        sp!, {...,fp,ip,lr,pc}
   [stfe        f7, [sp, #-12]!]
   [stfe        f6, [sp, #-12]!]
   [stfe        f5, [sp, #-12]!]
   [stfe        f4, [sp, #-12]!]
   sub fp, ip, #nn @@ nn == 20 or 4 depending on second insn */

static CORE_ADDR
arm_skip_prologue (CORE_ADDR pc)
{
  unsigned long inst;
  CORE_ADDR skip_pc;
  CORE_ADDR func_addr, func_end = 0;
  char *func_name;
  struct symtab_and_line sal;

  /* If we're in a dummy frame, don't even try to skip the prologue.  */
  if (deprecated_pc_in_call_dummy (pc))
    return pc;

  /* See what the symbol table says.  */

  if (find_pc_partial_function (pc, &func_name, &func_addr, &func_end))
    {
      struct symbol *sym;

      /* Found a function.  */
      sym = lookup_symbol (func_name, NULL, VAR_DOMAIN, NULL, NULL);
      if (sym && SYMBOL_LANGUAGE (sym) != language_asm)
        {
	  /* Don't use this trick for assembly source files.  */
	  sal = find_pc_line (func_addr, 0);
	  if ((sal.line != 0) && (sal.end < func_end))
	    return sal.end;
        }
    }

  /* Check if this is Thumb[-2] code.  */
  if (arm_pc_is_thumb (pc))
    return thumb_skip_prologue (pc, func_end);

  /* Can't find the prologue end in the symbol table, try it the hard way
     by disassembling the instructions.  */

  /* Like arm_scan_prologue, stop no later than pc + 64. */
  if (func_end == 0 || func_end > pc + 64)
    func_end = pc + 64;

  for (skip_pc = pc; skip_pc < func_end; skip_pc += 4)
    {
      inst = read_memory_integer (skip_pc, 4);

      /* "mov ip, sp" is no longer a required part of the prologue.  */
      if (inst == 0xe1a0c00d)			/* mov ip, sp */
	continue;

      if ((inst & 0xfffff000) == 0xe28dc000)    /* add ip, sp #n */
	continue;

      if ((inst & 0xfffff000) == 0xe24dc000)    /* sub ip, sp #n */
	continue;

      /* Some prologues begin with "str lr, [sp, #-4]!".  */
      if (inst == 0xe52de004)			/* str lr, [sp, #-4]! */
	continue;

      if ((inst & 0xfffffff0) == 0xe92d0000)	/* stmfd sp!,{a1,a2,a3,a4} */
	continue;

      if ((inst & 0xfffff800) == 0xe92dd800)	/* stmfd sp!,{fp,ip,lr,pc} */
	continue;

      /* Any insns after this point may float into the code, if it makes
	 for better instruction scheduling, so we skip them only if we
	 find them, but still consider the function to be frame-ful.  */

      /* We may have either one sfmfd instruction here, or several stfe
	 insns, depending on the version of floating point code we
	 support.  */
      if ((inst & 0xffbf0fff) == 0xec2d0200)	/* sfmfd fn, <cnt>, [sp]! */
	continue;

      if ((inst & 0xffff8fff) == 0xed6d0103)	/* stfe fn, [sp, #-12]! */
	continue;

      if ((inst & 0xfffff000) == 0xe24cb000)	/* sub fp, ip, #nn */
	continue;

      if ((inst & 0xfffff000) == 0xe24dd000)	/* sub sp, sp, #nn */
	continue;

      if ((inst & 0xffffc000) == 0xe54b0000 ||	/* strb r(0123),[r11,#-nn] */
	  (inst & 0xffffc0f0) == 0xe14b00b0 ||	/* strh r(0123),[r11,#-nn] */
	  (inst & 0xffffc000) == 0xe50b0000)	/* str  r(0123),[r11,#-nn] */
	continue;

      if ((inst & 0xffffc000) == 0xe5cd0000 ||	/* strb r(0123),[sp,#nn] */
	  (inst & 0xffffc0f0) == 0xe1cd00b0 ||	/* strh r(0123),[sp,#nn] */
	  (inst & 0xffffc000) == 0xe58d0000)	/* str  r(0123),[sp,#nn] */
	continue;

      /* Un-recognized instruction; stop scanning.  */
      break;
    }

  return skip_pc;		/* End of prologue */
}

/* Perform the Thumb-2 modify-constant(imm12) transformation.
   This is how the transformation works:
     1. 0...255 are represented as-is.

     2. Otherwise the 32-bit constant is normalized by rotating left until the
     most-significant bit is bit 7. The size of the rotation is then encoded
     in bits 7-11, overwriting bit 7.

     3. Constants of form 0x00XY00XY have bits[11:8] of imm12 set to 0b0001
     and bits[7:0] set to 0xXY.

     4. Constants of form 0xXY00XY00 as above, but bits[11:8] of imm12 are set
     to 0b0010.

     5. Similarly constants of form 0xXYXYXYXY have bits[11:8] set to 0b0011.
*/

static unsigned long
thumb2_modify_constant (unsigned long imm12)
{
  unsigned int rotation = bits (imm12, 7, 11);
  unsigned int replicate = rotation >> 1;  /* bits 8-11.  */

  if (rotation >= 8)
    {
      imm12 = bits (imm12, 0, 6) | 0x80;
      return (imm12 >> rotation) | (imm12 << (32 - rotation));
    }
  else
    {
      switch (replicate)
	{
	case 0:
	  return imm12;

	case 1:
	  return imm12 | (imm12 << 16);

	case 2:
	  return (imm12 << 8) | (imm12 << 24);

	case 3:
	  return imm12 | (imm12 << 8) | (imm12 << 16) | (imm12 << 24);
	}
    }
}

/* *INDENT-OFF* */
/* Function: thumb_scan_prologue (helper function for arm_scan_prologue)
   This function decodes a Thumb function prologue to determine:
     1) the size of the stack frame
     2) which registers are saved on it
     3) the offsets of saved regs
     4) the offset from the stack pointer to the frame pointer

   A typical Thumb function prologue would create this stack frame
   (offsets relative to FP)
     old SP ->	24  stack parameters
		20  LR
		16  R7
     R7 ->       0  local variables (16 bytes)
     SP ->     -12  additional stack space (12 bytes)
   The frame size would thus be 36 bytes, and the frame offset would be
   12 bytes.  The frame register is R7. 
   
   The comments for thumb_skip_prolog() describe the algorithm we use
   to detect the end of the prolog.  */
/* *INDENT-ON* */

static void
thumb_scan_prologue (CORE_ADDR prev_pc, struct arm_prologue_cache *cache)
{
  CORE_ADDR prologue_start;
  CORE_ADDR prologue_end;
  CORE_ADDR current_pc;
  /* Which register has been copied to register n?  */
  int saved_reg[16];
  /* findmask:
     bit 0 - push { rlist }
     bit 1 - mov r7, sp  OR  add r7, sp, #imm  (setting of r7)
     bit 2 - sub sp, #simm  OR  add sp, #simm  (adjusting of sp)
  */
  int findmask = 0;
  int i;

  if (find_pc_partial_function (prev_pc, NULL, &prologue_start, &prologue_end))
    {
      struct symtab_and_line sal = find_pc_line (prologue_start, 0);

      if (sal.line == 0)		/* no line info, use current PC  */
	prologue_end = prev_pc;
      else if (sal.end < prologue_end)	/* next line begins after fn end */
	prologue_end = sal.end;		/* (probably means no prologue)  */
    }
  else
    /* We're in the boondocks: allow for 
       16 pushes, an add, and "mv fp,sp".  */
    prologue_end = prologue_start + 40;

  prologue_end = min (prologue_end, prev_pc);

  /* Initialize the saved register map.  When register H is copied to
     register L, we will put H in saved_reg[L].  */
  for (i = 0; i < 16; i++)
    saved_reg[i] = i;

  /* Search the prologue looking for instructions that set up the
     frame pointer, adjust the stack pointer, and save registers.
     Do this until all basic prolog instructions are found.  */

  cache->framesize = 0;
  for (current_pc = prologue_start;
       (current_pc < prologue_end) && ((findmask & 7) != 7);)
    {
      unsigned long insn;
      int length;
      int regno;
      int offset;

      insn = thumb_get_insn (current_pc, &length);

      if ((insn & 0xfffffe00) == 0xb400)	/* push { rlist } */
	{
	  int mask;
	  findmask |= 1;		/* push found */
	  /* Bits 0-7 contain a mask for registers R0-R7.  Bit 8 says
	     whether to save LR (R14).  */
	  mask = (insn & 0xff) | ((insn & 0x100) << 6);

	  /* Calculate offsets of saved R0-R7 and LR.  */
	  for (regno = ARM_LR_REGNUM; regno >= 0; regno--)
	    if (mask & (1 << regno))
	      {
		cache->framesize += 4;
		cache->saved_regs[saved_reg[regno]].addr = -cache->framesize;
		/* Reset saved register map.  */
		saved_reg[regno] = regno;
	      }
	}
      else if (((insn & 0xffffa000) == 0xe92d0000)
	       && bitcount (insn & 0x5fff) > 1) /* stmfd.w sp!, { rlist }  */
	{
          unsigned int mask;
	  findmask |= 1;

	  /* As above, but Thumb-2 can store R0-R12 and LR.  */
          mask = insn & 0x5fff;

	  for (regno = ARM_LR_REGNUM; regno >= 0; regno--)
	    if (mask & (1 << regno))
	      {
                cache->framesize += 4;
		/* Thumb-2 doesn't need to copy high registers to low registers
		   in order to push them, so we don't need the saved_reg
		   array.  */
		cache->saved_regs[regno].addr = -cache->framesize;
	      }
	}
      else if ((insn & 0xffff0fff) == 0xf84d0d04)  /* str.w rn, [sp, #-4]!  */
	{
	  /* A single callee-saved high register (except LR) is saved with an
	     STR instruction on Thumb-2.  */
          int rn = bits (insn, 12, 15);
	  cache->framesize += 4;
	  /* Again, no need to use saved_reg[] because the high register will
	     be pushed directly when using Thumb-2.  */
	  cache->saved_regs[rn].addr = -cache->framesize;
	}
      else if ((insn & 0xffffff00) == 0xb000)	/* add sp, #simm  OR  
						   sub sp, #simm */
	{
	  if ((findmask & 1) == 0)		/* before push?  */
	    goto next_iter;
	  else
	    findmask |= 4;			/* add/sub sp found */
	  
	  offset = (insn & 0x7f) << 2;		/* get scaled offset */
	  if (insn & 0x80)		/* is it signed? (==subtracting) */
	    {
	      cache->frameoffset += offset;
	      offset = -offset;
	    }
	  cache->framesize -= offset;
	}
      else if ((insn & 0xfbef8f00) == 0xf1ad0d00)  /* sub.w sp, sp, #imm  */
	{
          unsigned int imm;

	  if ((findmask & 1) == 0)              /* before push?  */
	    goto next_iter;
	  else
	    findmask |= 4;                      /* sub sp found.  */

	  imm = bits (insn, 0, 7) | (bits (insn, 12, 14) << 8)
	        | (bit (insn, 26) << 11);

	  imm = thumb2_modify_constant (imm);

	  cache->frameoffset += imm;
	  cache->framesize += offset;
	}
      else if ((insn & 0xffffff00) == 0xaf00)	/* add r7, sp, #imm */
	{
	  findmask |= 2;			/* setting of r7 found */
	  cache->framereg = THUMB_FP_REGNUM;
	  /* get scaled offset */
	  cache->frameoffset = (insn & 0xff) << 2;
	}
      else if (insn == 0x466f)			/* mov r7, sp */
	{
	  findmask |= 2;			/* setting of r7 found */
	  cache->framereg = THUMB_FP_REGNUM;
	  cache->frameoffset = 0;
	  saved_reg[THUMB_FP_REGNUM] = ARM_SP_REGNUM;
	}
      else if ((insn & 0xffffffc0) == 0x4640)	/* mov r0-r7, r8-r15 */
	{
	  int lo_reg = insn & 7;		/* dest.  register (r0-r7) */
	  int hi_reg = ((insn >> 3) & 7) + 8;	/* source register (r8-15) */
	  saved_reg[lo_reg] = hi_reg;		/* remember hi reg was saved */
	}
      else
	/* Something in the prolog that we don't care about or some
	   instruction from outside the prolog scheduled here for
	   optimization.  */ 
	goto next_iter;

    next_iter:
      current_pc += length;
    }
}

/* This function decodes an ARM function prologue to determine:
   1) the size of the stack frame
   2) which registers are saved on it
   3) the offsets of saved regs
   4) the offset from the stack pointer to the frame pointer
   This information is stored in the "extra" fields of the frame_info.

   There are two basic forms for the ARM prologue.  The fixed argument
   function call will look like:

   mov    ip, sp
   stmfd  sp!, {fp, ip, lr, pc}
   sub    fp, ip, #4
   [sub sp, sp, #4]

   Which would create this stack frame (offsets relative to FP):
   IP ->   4    (caller's stack)
   FP ->   0    PC (points to address of stmfd instruction + 8 in callee)
   -4   LR (return address in caller)
   -8   IP (copy of caller's SP)
   -12  FP (caller's FP)
   SP -> -28    Local variables

   The frame size would thus be 32 bytes, and the frame offset would be
   28 bytes.  The stmfd call can also save any of the vN registers it
   plans to use, which increases the frame size accordingly.

   Note: The stored PC is 8 off of the STMFD instruction that stored it
   because the ARM Store instructions always store PC + 8 when you read
   the PC register.

   A variable argument function call will look like:

   mov    ip, sp
   stmfd  sp!, {a1, a2, a3, a4}
   stmfd  sp!, {fp, ip, lr, pc}
   sub    fp, ip, #20

   Which would create this stack frame (offsets relative to FP):
   IP ->  20    (caller's stack)
   16  A4
   12  A3
   8  A2
   4  A1
   FP ->   0    PC (points to address of stmfd instruction + 8 in callee)
   -4   LR (return address in caller)
   -8   IP (copy of caller's SP)
   -12  FP (caller's FP)
   SP -> -28    Local variables

   The frame size would thus be 48 bytes, and the frame offset would be
   28 bytes.

   There is another potential complication, which is that the optimizer
   will try to separate the store of fp in the "stmfd" instruction from
   the "sub fp, ip, #NN" instruction.  Almost anything can be there, so
   we just key on the stmfd, and then scan for the "sub fp, ip, #NN"...

   Also, note, the original version of the ARM toolchain claimed that there
   should be an

   instruction at the end of the prologue.  I have never seen GCC produce
   this, and the ARM docs don't mention it.  We still test for it below in
   case it happens...

 */

static void
arm_scan_prologue (struct frame_info *next_frame, struct arm_prologue_cache *cache)
{
  int regno, sp_offset, fp_offset, ip_offset;
  CORE_ADDR prologue_start, prologue_end, current_pc;
  CORE_ADDR prev_pc = frame_pc_unwind (next_frame);

  /* Assume there is no frame until proven otherwise.  */
  cache->framereg = ARM_SP_REGNUM;
  cache->framesize = 0;
  cache->frameoffset = 0;

  /* Check for Thumb prologue.  */
  if (arm_pc_is_thumb (prev_pc))
    {
      thumb_scan_prologue (prev_pc, cache);
      return;
    }

  /* Find the function prologue.  If we can't find the function in
     the symbol table, peek in the stack frame to find the PC.  */
  if (find_pc_partial_function (prev_pc, NULL, &prologue_start, &prologue_end))
    {
      /* One way to find the end of the prologue (which works well
         for unoptimized code) is to do the following:

	    struct symtab_and_line sal = find_pc_line (prologue_start, 0);

	    if (sal.line == 0)
	      prologue_end = prev_pc;
	    else if (sal.end < prologue_end)
	      prologue_end = sal.end;

	 This mechanism is very accurate so long as the optimizer
	 doesn't move any instructions from the function body into the
	 prologue.  If this happens, sal.end will be the last
	 instruction in the first hunk of prologue code just before
	 the first instruction that the scheduler has moved from
	 the body to the prologue.

	 In order to make sure that we scan all of the prologue
	 instructions, we use a slightly less accurate mechanism which
	 may scan more than necessary.  To help compensate for this
	 lack of accuracy, the prologue scanning loop below contains
	 several clauses which'll cause the loop to terminate early if
	 an implausible prologue instruction is encountered.  
	 
	 The expression
	 
	      prologue_start + 64
	    
	 is a suitable endpoint since it accounts for the largest
	 possible prologue plus up to five instructions inserted by
	 the scheduler.  */
         
      if (prologue_end > prologue_start + 64)
	{
	  prologue_end = prologue_start + 64;	/* See above.  */
	}
    }
  else
    {
      /* We have no symbol information.  Our only option is to assume this
	 function has a standard stack frame and the normal frame register.
	 Then, we can find the value of our frame pointer on entrance to
	 the callee (or at the present moment if this is the innermost frame).
	 The value stored there should be the address of the stmfd + 8.  */
      CORE_ADDR frame_loc;
      LONGEST return_value;

      frame_loc = frame_unwind_register_unsigned (next_frame, ARM_FP_REGNUM);
      if (!safe_read_memory_integer (frame_loc, 4, &return_value))
        return;
      else
        {
          prologue_start = ADDR_BITS_REMOVE (return_value) - 8;
          prologue_end = prologue_start + 64;	/* See above.  */
        }
    }

  if (prev_pc < prologue_end)
    prologue_end = prev_pc;

  /* Now search the prologue looking for instructions that set up the
     frame pointer, adjust the stack pointer, and save registers.

     Be careful, however, and if it doesn't look like a prologue,
     don't try to scan it.  If, for instance, a frameless function
     begins with stmfd sp!, then we will tell ourselves there is
     a frame, which will confuse stack traceback, as well as "finish" 
     and other operations that rely on a knowledge of the stack
     traceback.

     In the APCS, the prologue should start with  "mov ip, sp" so
     if we don't see this as the first insn, we will stop.  

     [Note: This doesn't seem to be true any longer, so it's now an
     optional part of the prologue.  - Kevin Buettner, 2001-11-20]

     [Note further: The "mov ip,sp" only seems to be missing in
     frameless functions at optimization level "-O2" or above,
     in which case it is often (but not always) replaced by
     "str lr, [sp, #-4]!".  - Michael Snyder, 2002-04-23]  */

  sp_offset = fp_offset = ip_offset = 0;

  for (current_pc = prologue_start;
       current_pc < prologue_end;
       current_pc += 4)
    {
      unsigned int insn = read_memory_unsigned_integer (current_pc, 4);

      if (insn == 0xe1a0c00d)		/* mov ip, sp */
	{
	  ip_offset = 0;
	  continue;
	}
      else if ((insn & 0xfffff000) == 0xe28dc000) /* add ip, sp #n */
	{
	  unsigned imm = insn & 0xff;                   /* immediate value */
	  unsigned rot = (insn & 0xf00) >> 7;           /* rotate amount */
	  imm = (imm >> rot) | (imm << (32 - rot));
	  ip_offset = imm;
	  continue;
	}
      else if ((insn & 0xfffff000) == 0xe24dc000) /* sub ip, sp #n */
	{
	  unsigned imm = insn & 0xff;                   /* immediate value */
	  unsigned rot = (insn & 0xf00) >> 7;           /* rotate amount */
	  imm = (imm >> rot) | (imm << (32 - rot));
	  ip_offset = -imm;
	  continue;
	}
      else if (insn == 0xe52de004)	/* str lr, [sp, #-4]! */
	{
	  sp_offset -= 4;
	  cache->saved_regs[ARM_LR_REGNUM].addr = sp_offset;
	  continue;
	}
      else if ((insn & 0xffff0000) == 0xe92d0000)
	/* stmfd sp!, {..., fp, ip, lr, pc}
	   or
	   stmfd sp!, {a1, a2, a3, a4}  */
	{
	  int mask = insn & 0xffff;

	  /* Calculate offsets of saved registers.  */
	  for (regno = ARM_PC_REGNUM; regno >= 0; regno--)
	    if (mask & (1 << regno))
	      {
		sp_offset -= 4;
		cache->saved_regs[regno].addr = sp_offset;
	      }
	}
      else if ((insn & 0xffffc000) == 0xe54b0000 ||	/* strb rx,[r11,#-n] */
	       (insn & 0xffffc0f0) == 0xe14b00b0 ||	/* strh rx,[r11,#-n] */
	       (insn & 0xffffc000) == 0xe50b0000)	/* str  rx,[r11,#-n] */
	{
	  /* No need to add this to saved_regs -- it's just an arg reg.  */
	  continue;
	}
      else if ((insn & 0xffffc000) == 0xe5cd0000 ||	/* strb rx,[sp,#n] */
	       (insn & 0xffffc0f0) == 0xe1cd00b0 ||	/* strh rx,[sp,#n] */
	       (insn & 0xffffc000) == 0xe58d0000)	/* str  rx,[sp,#n] */
	{
	  /* No need to add this to saved_regs -- it's just an arg reg.  */
	  continue;
	}
      else if ((insn & 0xfffff000) == 0xe24cb000)	/* sub fp, ip #n */
	{
	  unsigned imm = insn & 0xff;			/* immediate value */
	  unsigned rot = (insn & 0xf00) >> 7;		/* rotate amount */
	  imm = (imm >> rot) | (imm << (32 - rot));
	  fp_offset = -imm + ip_offset;
	  cache->framereg = ARM_FP_REGNUM;
	}
      else if ((insn & 0xfffff000) == 0xe24dd000)	/* sub sp, sp #n */
	{
	  unsigned imm = insn & 0xff;			/* immediate value */
	  unsigned rot = (insn & 0xf00) >> 7;		/* rotate amount */
	  imm = (imm >> rot) | (imm << (32 - rot));
	  sp_offset -= imm;
	}
      else if ((insn & 0xffff7fff) == 0xed6d0103)	/* stfe f?, [sp, -#c]! */
	{
	  sp_offset -= 12;
	  regno = ARM_F0_REGNUM + ((insn >> 12) & 0x07);
	  cache->saved_regs[regno].addr = sp_offset;
	}
      else if ((insn & 0xffbf0fff) == 0xec2d0200)	/* sfmfd f0, 4, [sp!] */
	{
	  int n_saved_fp_regs;
	  unsigned int fp_start_reg, fp_bound_reg;

	  if ((insn & 0x800) == 0x800)		/* N0 is set */
	    {
	      if ((insn & 0x40000) == 0x40000)	/* N1 is set */
		n_saved_fp_regs = 3;
	      else
		n_saved_fp_regs = 1;
	    }
	  else
	    {
	      if ((insn & 0x40000) == 0x40000)	/* N1 is set */
		n_saved_fp_regs = 2;
	      else
		n_saved_fp_regs = 4;
	    }

	  fp_start_reg = ARM_F0_REGNUM + ((insn >> 12) & 0x7);
	  fp_bound_reg = fp_start_reg + n_saved_fp_regs;
	  for (; fp_start_reg < fp_bound_reg; fp_start_reg++)
	    {
	      sp_offset -= 12;
	      cache->saved_regs[fp_start_reg++].addr = sp_offset;
	    }
	}
      else if ((insn & 0xf0000000) != 0xe0000000)
	break;			/* Condition not true, exit early */
      else if ((insn & 0xfe200000) == 0xe8200000)	/* ldm? */
	break;			/* Don't scan past a block load */
      else
	/* The optimizer might shove anything into the prologue,
	   so we just skip what we don't recognize.  */
	continue;
    }

  /* The frame size is just the negative of the offset (from the
     original SP) of the last thing thing we pushed on the stack. 
     The frame offset is [new FP] - [new SP].  */
  cache->framesize = -sp_offset;
  if (cache->framereg == ARM_FP_REGNUM)
    cache->frameoffset = fp_offset - sp_offset;
  else
    cache->frameoffset = 0;
}

static struct arm_prologue_cache *
arm_make_prologue_cache (struct frame_info *next_frame)
{
  int reg;
  struct arm_prologue_cache *cache;
  CORE_ADDR unwound_fp;

  cache = frame_obstack_zalloc (sizeof (struct arm_prologue_cache));
  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  arm_scan_prologue (next_frame, cache);

  unwound_fp = frame_unwind_register_unsigned (next_frame, cache->framereg);
  if (unwound_fp == 0)
    return cache;

  cache->prev_sp = unwound_fp + cache->framesize - cache->frameoffset;

  /* Calculate actual addresses of saved registers using offsets
     determined by arm_scan_prologue.  */
  for (reg = 0; reg < NUM_REGS; reg++)
    if (trad_frame_addr_p (cache->saved_regs, reg))
      cache->saved_regs[reg].addr += cache->prev_sp;

  return cache;
}

/* Our frame ID for a normal frame is the current function's starting PC
   and the caller's SP when we were called.  */

static void
arm_prologue_this_id (struct frame_info *next_frame,
		      void **this_cache,
		      struct frame_id *this_id)
{
  struct arm_prologue_cache *cache;
  struct frame_id id;
  CORE_ADDR func;

  if (*this_cache == NULL)
    *this_cache = arm_make_prologue_cache (next_frame);
  cache = *this_cache;

  func = frame_func_unwind (next_frame);

  /* This is meant to halt the backtrace at "_start".  Make sure we
     don't halt it at a generic dummy frame. */
  if (func <= LOWEST_PC)
    return;

  /* If we've hit a wall, stop.  */
  if (cache->prev_sp == 0)
    return;

  id = frame_id_build (cache->prev_sp, func);
  *this_id = id;
}

static void
arm_prologue_prev_register (struct frame_info *next_frame,
			    void **this_cache,
			    int prev_regnum,
			    int *optimized,
			    enum lval_type *lvalp,
			    CORE_ADDR *addrp,
			    int *realnump,
			    gdb_byte *valuep)
{
  struct arm_prologue_cache *cache;

  if (*this_cache == NULL)
    *this_cache = arm_make_prologue_cache (next_frame);
  cache = *this_cache;

  /* If we are asked to unwind the PC, then we need to return the LR
     instead.  The saved value of PC points into this frame's
     prologue, not the next frame's resume location.  */
  if (prev_regnum == ARM_PC_REGNUM)
    prev_regnum = ARM_LR_REGNUM;

  /* SP is generally not saved to the stack, but this frame is
     identified by NEXT_FRAME's stack pointer at the time of the call.
     The value was already reconstructed into PREV_SP.  */
  if (prev_regnum == ARM_SP_REGNUM)
    {
      *lvalp = not_lval;
      if (valuep)
	store_unsigned_integer (valuep, 4, cache->prev_sp);
      return;
    }

  trad_frame_get_prev_register (next_frame, cache->saved_regs, prev_regnum,
				optimized, lvalp, addrp, realnump, valuep);
}

struct frame_unwind arm_prologue_unwind = {
  NORMAL_FRAME,
  arm_prologue_this_id,
  arm_prologue_prev_register
};

static const struct frame_unwind *
arm_prologue_unwind_sniffer (struct frame_info *next_frame)
{
  return &arm_prologue_unwind;
}

static struct arm_prologue_cache *
arm_make_stub_cache (struct frame_info *next_frame)
{
  int reg;
  struct arm_prologue_cache *cache;
  CORE_ADDR unwound_fp;

  cache = frame_obstack_zalloc (sizeof (struct arm_prologue_cache));
  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  cache->prev_sp = frame_unwind_register_unsigned (next_frame, ARM_SP_REGNUM);

  return cache;
}

/* Our frame ID for a stub frame is the current SP and LR.  */

static void
arm_stub_this_id (struct frame_info *next_frame,
		  void **this_cache,
		  struct frame_id *this_id)
{
  struct arm_prologue_cache *cache;

  if (*this_cache == NULL)
    *this_cache = arm_make_stub_cache (next_frame);
  cache = *this_cache;

  *this_id = frame_id_build (cache->prev_sp,
			     frame_pc_unwind (next_frame));
}

struct frame_unwind arm_stub_unwind = {
  NORMAL_FRAME,
  arm_stub_this_id,
  arm_prologue_prev_register
};

static const struct frame_unwind *
arm_stub_unwind_sniffer (struct frame_info *next_frame)
{
  char dummy[4];

  if (in_plt_section (frame_unwind_address_in_block (next_frame), NULL)
      || target_read_memory (frame_pc_unwind (next_frame), dummy, 4) != 0)
    return &arm_stub_unwind;

  return NULL;
}

static CORE_ADDR
arm_normal_frame_base (struct frame_info *next_frame, void **this_cache)
{
  struct arm_prologue_cache *cache;

  if (*this_cache == NULL)
    *this_cache = arm_make_prologue_cache (next_frame);
  cache = *this_cache;

  return cache->prev_sp + cache->frameoffset - cache->framesize;
}

struct frame_base arm_normal_base = {
  &arm_prologue_unwind,
  arm_normal_frame_base,
  arm_normal_frame_base,
  arm_normal_frame_base
};

static struct arm_prologue_cache *
arm_make_sigtramp_cache (struct frame_info *next_frame)
{
  struct arm_prologue_cache *cache;
  int reg;

  cache = frame_obstack_zalloc (sizeof (struct arm_prologue_cache));

  cache->prev_sp = frame_unwind_register_unsigned (next_frame, ARM_SP_REGNUM);

  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  for (reg = 0; reg < NUM_REGS; reg++)
    cache->saved_regs[reg].addr
      = SIGCONTEXT_REGISTER_ADDRESS (cache->prev_sp,
				     frame_pc_unwind (next_frame), reg);

  /* FIXME: What about thumb mode?  */
  cache->framereg = ARM_SP_REGNUM;
  cache->prev_sp
    = read_memory_integer (cache->saved_regs[cache->framereg].addr,
			   register_size (current_gdbarch, cache->framereg));

  return cache;
}

static void
arm_sigtramp_this_id (struct frame_info *next_frame,
		      void **this_cache,
		      struct frame_id *this_id)
{
  struct arm_prologue_cache *cache;

  if (*this_cache == NULL)
    *this_cache = arm_make_sigtramp_cache (next_frame);
  cache = *this_cache;

  /* FIXME drow/2003-07-07: This isn't right if we single-step within
     the sigtramp frame; the PC should be the beginning of the trampoline.  */
  *this_id = frame_id_build (cache->prev_sp, frame_pc_unwind (next_frame));
}

static void
arm_sigtramp_prev_register (struct frame_info *next_frame,
			    void **this_cache,
			    int prev_regnum,
			    int *optimized,
			    enum lval_type *lvalp,
			    CORE_ADDR *addrp,
			    int *realnump,
			    gdb_byte *valuep)
{
  struct arm_prologue_cache *cache;

  if (*this_cache == NULL)
    *this_cache = arm_make_sigtramp_cache (next_frame);
  cache = *this_cache;

  trad_frame_get_prev_register (next_frame, cache->saved_regs, prev_regnum,
				optimized, lvalp, addrp, realnump, valuep);
}

struct frame_unwind arm_sigtramp_unwind = {
  SIGTRAMP_FRAME,
  arm_sigtramp_this_id,
  arm_sigtramp_prev_register
};

static const struct frame_unwind *
arm_sigtramp_unwind_sniffer (struct frame_info *next_frame)
{
  if (SIGCONTEXT_REGISTER_ADDRESS_P ()
      && legacy_pc_in_sigtramp (frame_pc_unwind (next_frame), (char *) 0))
    return &arm_sigtramp_unwind;

  return NULL;
}

/* Assuming NEXT_FRAME->prev is a dummy, return the frame ID of that
   dummy frame.  The frame ID's base needs to match the TOS value
   saved by save_dummy_frame_tos() and returned from
   arm_push_dummy_call, and the PC needs to match the dummy frame's
   breakpoint.  */

static struct frame_id
arm_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_id_build (frame_unwind_register_unsigned (next_frame, ARM_SP_REGNUM),
			 frame_pc_unwind (next_frame));
}

/* Given THIS_FRAME, find the previous frame's resume PC (which will
   be used to construct the previous frame's ID, after looking up the
   containing function).  */

static CORE_ADDR
arm_unwind_pc (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  CORE_ADDR pc;
  pc = frame_unwind_register_unsigned (this_frame, ARM_PC_REGNUM);
  return IS_THUMB_ADDR (pc) ? UNMAKE_THUMB_ADDR (pc) : pc;
}

static CORE_ADDR
arm_unwind_sp (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  return frame_unwind_register_unsigned (this_frame, ARM_SP_REGNUM);
}

/* When arguments must be pushed onto the stack, they go on in reverse
   order.  The code below implements a FILO (stack) to do this.  */

struct stack_item
{
  int len;
  struct stack_item *prev;
  void *data;
};

static struct stack_item *
push_stack_item (struct stack_item *prev, void *contents, int len)
{
  struct stack_item *si;
  si = xmalloc (sizeof (struct stack_item));
  si->data = xmalloc (len);
  si->len = len;
  si->prev = prev;
  memcpy (si->data, contents, len);
  return si;
}

static struct stack_item *
pop_stack_item (struct stack_item *si)
{
  struct stack_item *dead = si;
  si = si->prev;
  xfree (dead->data);
  xfree (dead);
  return si;
}


/* Return the alignment (in bytes) of the given type.  */

static int
arm_type_align (struct type *t)
{
  int n;
  int align;
  int falign;

  t = check_typedef (t);
  switch (TYPE_CODE (t))
    {
    default:
      /* Should never happen.  */
      internal_error (__FILE__, __LINE__, _("unknown type alignment"));
      return 4;

    case TYPE_CODE_PTR:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
    case TYPE_CODE_SET:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_BITSTRING:
    case TYPE_CODE_REF:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_BOOL:
      return TYPE_LENGTH (t);

    case TYPE_CODE_ARRAY:
    case TYPE_CODE_COMPLEX:
      /* TODO: What about vector types?  */
      return arm_type_align (TYPE_TARGET_TYPE (t));

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      align = 1;
      for (n = 0; n < TYPE_NFIELDS (t); n++)
	{
	  falign = arm_type_align (TYPE_FIELD_TYPE (t, n));
	  if (falign > align)
	    align = falign;
	}
      return align;
    }
}

/* We currently only support passing parameters in integer registers.  This
   conforms with GCC's default model.  Several other variants exist and
   we should probably support some of them based on the selected ABI.  */

static CORE_ADDR
arm_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
		     struct regcache *regcache, CORE_ADDR bp_addr, int nargs,
		     struct value **args, CORE_ADDR sp, int struct_return,
		     CORE_ADDR struct_addr)
{
  int argnum;
  int argreg;
  int nstack;
  struct stack_item *si = NULL;

  /* Set the return address.  For the ARM, the return breakpoint is
     always at BP_ADDR.  */
  /* XXX Fix for Thumb.  */
  regcache_cooked_write_unsigned (regcache, ARM_LR_REGNUM, bp_addr);

  /* Walk through the list of args and determine how large a temporary
     stack is required.  Need to take care here as structs may be
     passed on the stack, and we have to to push them.  */
  nstack = 0;

  argreg = ARM_A1_REGNUM;
  nstack = 0;

  /* Some platforms require a double-word aligned stack.  Make sure sp
     is correctly aligned before we start.  We always do this even if
     it isn't really needed -- it can never hurt things.  */
  sp &= ~(CORE_ADDR)(2 * DEPRECATED_REGISTER_SIZE - 1);

  /* The struct_return pointer occupies the first parameter
     passing register.  */
  if (struct_return)
    {
      if (arm_debug)
	fprintf_unfiltered (gdb_stdlog, "struct return in %s = 0x%s\n",
			    REGISTER_NAME (argreg), paddr (struct_addr));
      regcache_cooked_write_unsigned (regcache, argreg, struct_addr);
      argreg++;
    }

  for (argnum = 0; argnum < nargs; argnum++)
    {
      int len;
      struct type *arg_type;
      struct type *target_type;
      enum type_code typecode;
      bfd_byte *val;
      int align;

      arg_type = check_typedef (value_type (args[argnum]));
      len = TYPE_LENGTH (arg_type);
      target_type = TYPE_TARGET_TYPE (arg_type);
      typecode = TYPE_CODE (arg_type);
      val = value_contents_writeable (args[argnum]);

      align = arm_type_align (arg_type);
      /* Round alignment up to a whole number of words.  */
      align = (align + INT_REGISTER_SIZE - 1) & ~(INT_REGISTER_SIZE - 1);
      /* Different ABIs have different maximum alignments.  */
      if (gdbarch_tdep (gdbarch)->arm_abi == ARM_ABI_APCS)
	{
	  /* The APCS ABI only requires word alignment.  */
	  align = INT_REGISTER_SIZE;
	}
      else
	{
	  /* The AAPCS requires at most doubleword alignment.  */
	  if (align > INT_REGISTER_SIZE * 2)
	    align = INT_REGISTER_SIZE * 2;
	}

      /* Push stack padding for dowubleword alignment.  */
      if (nstack & (align - 1))
	{
	  si = push_stack_item (si, val, INT_REGISTER_SIZE);
	  nstack += INT_REGISTER_SIZE;
	}
      
      /* Doubleword aligned quantities must go in even register pairs.  */
      if (argreg <= ARM_LAST_ARG_REGNUM
	  && align > INT_REGISTER_SIZE
	  && argreg & 1)
	argreg++;

      /* If the argument is a pointer to a function, and it is a
	 Thumb function, create a LOCAL copy of the value and set
	 the THUMB bit in it.  */
      if (TYPE_CODE_PTR == typecode
	  && target_type != NULL
	  && TYPE_CODE_FUNC == TYPE_CODE (target_type))
	{
	  CORE_ADDR regval = extract_unsigned_integer (val, len);
	  if (arm_pc_is_thumb (regval))
	    {
	      val = alloca (len);
	      store_unsigned_integer (val, len, MAKE_THUMB_ADDR (regval));
	    }
	}

      /* Copy the argument to general registers or the stack in
	 register-sized pieces.  Large arguments are split between
	 registers and stack.  */
      while (len > 0)
	{
	  int partial_len = len < DEPRECATED_REGISTER_SIZE ? len : DEPRECATED_REGISTER_SIZE;

	  if (argreg <= ARM_LAST_ARG_REGNUM)
	    {
	      /* The argument is being passed in a general purpose
		 register.  */
	      CORE_ADDR regval = extract_unsigned_integer (val, partial_len);
	      if (arm_debug)
		fprintf_unfiltered (gdb_stdlog, "arg %d in %s = 0x%s\n",
				    argnum, REGISTER_NAME (argreg),
				    phex (regval, DEPRECATED_REGISTER_SIZE));
	      regcache_cooked_write_unsigned (regcache, argreg, regval);
	      argreg++;
	    }
	  else
	    {
	      /* Push the arguments onto the stack.  */
	      if (arm_debug)
		fprintf_unfiltered (gdb_stdlog, "arg %d @ sp + %d\n",
				    argnum, nstack);
	      si = push_stack_item (si, val, DEPRECATED_REGISTER_SIZE);
	      nstack += DEPRECATED_REGISTER_SIZE;
	    }
	      
	  len -= partial_len;
	  val += partial_len;
	}
    }
  /* If we have an odd number of words to push, then decrement the stack
     by one word now, so first stack argument will be dword aligned.  */
  if (nstack & 4)
    sp -= 4;

  while (si)
    {
      sp -= si->len;
      write_memory (sp, si->data, si->len);
      si = pop_stack_item (si);
    }

  /* Finally, update teh SP register.  */
  regcache_cooked_write_unsigned (regcache, ARM_SP_REGNUM, sp);

  return sp;
}

static void
print_fpu_flags (int flags)
{
  if (flags & (1 << 0))
    fputs ("IVO ", stdout);
  if (flags & (1 << 1))
    fputs ("DVZ ", stdout);
  if (flags & (1 << 2))
    fputs ("OFL ", stdout);
  if (flags & (1 << 3))
    fputs ("UFL ", stdout);
  if (flags & (1 << 4))
    fputs ("INX ", stdout);
  putchar ('\n');
}

/* Print interesting information about the floating point processor
   (if present) or emulator.  */
static void
arm_print_float_info (struct gdbarch *gdbarch, struct ui_file *file,
		      struct frame_info *frame, const char *args)
{
  unsigned long status = read_register (ARM_FPS_REGNUM);
  int type;

  type = (status >> 24) & 127;
  if (status & (1 << 31))
    printf (_("Hardware FPU type %d\n"), type);
  else
    printf (_("Software FPU type %d\n"), type);
  /* i18n: [floating point unit] mask */
  fputs (_("mask: "), stdout);
  print_fpu_flags (status >> 16);
  /* i18n: [floating point unit] flags */
  fputs (_("flags: "), stdout);
  print_fpu_flags (status);
}

/* Type for iWMMXt registers.  */
static struct type *arm_iwmmxt_type;

/* Construct the type for iWMMXt registers.  */
static struct type *
arm_build_iwmmxt_type (void)
{
  /* The type we're building is this: */
#if 0
  union __gdb_builtin_type_vec64i 
  {
    int64_t uint64;
    int32_t v2_int32[2];
    int16_t v4_int16[4];
    int8_t v8_int8[8];
  };
#endif

  if (!arm_iwmmxt_type)
    {
      struct type *t;

      t = init_composite_type ("__gdb_builtin_type_vec64i", TYPE_CODE_UNION);
      append_composite_type_field (t, "uint64", builtin_type_int64);
      append_composite_type_field (t, "v2_int32", builtin_type_v2_int32);
      append_composite_type_field (t, "v4_int16", builtin_type_v4_int16);
      append_composite_type_field (t, "v8_int8", builtin_type_v8_int8);

      TYPE_FLAGS (t) |= TYPE_FLAG_VECTOR;
      TYPE_NAME (t) = "builtin_type_vec64i";

      arm_iwmmxt_type = t;
    }

  return arm_iwmmxt_type;
}
/* Return the GDB type object for the "standard" data type of data in
   register N.  */

static struct type *
arm_register_type (struct gdbarch *gdbarch, int regnum)
{
  int first;

  first = gdbarch_tdep (gdbarch)->first_iwmmxt_regnum;
  if (first != -1)
    {
      if (regnum >= first && regnum < first + NUM_IWMMXT_COP0REGS)
	return arm_build_iwmmxt_type ();

      first += NUM_IWMMXT_COP0REGS;

      if (regnum >= first && regnum < first + NUM_IWMMXT_COP1REGS)
	return builtin_type_int32;
    }

  first = gdbarch_tdep (gdbarch)->first_vfp_regnum;
  if (first != -1)
    {
      if (regnum >= first && regnum < first + NUM_VFP_XREGS)
	return builtin_type_int32;

      first += NUM_VFP_XREGS;

      if (regnum >= first && regnum < first + NUM_VFP_SREGS)
	return builtin_type_float;
    }

  first = gdbarch_tdep (gdbarch)->first_vfp_pseudo;
  if (first != -1)
    {
      first += gdbarch_num_regs (gdbarch);

      if (regnum >= first && regnum < first + NUM_VFP_PSEUDOS)
	return builtin_type_double;
    }

  if (regnum >= ARM_F0_REGNUM && regnum < ARM_F0_REGNUM + NUM_FPA_REGS)
    {
      if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
	return builtin_type_arm_ext_big;
      else
	return builtin_type_arm_ext_littlebyte_bigword;
    }
  else
    return builtin_type_int32;
}

/* Index within `registers' of the first byte of the space for
   register N.  */

static int
arm_register_byte (int regnum)
{
  int offset;
  int first;

  offset = 0;
  first = 0;
  if (regnum < ARM_F0_REGNUM)
    return regnum * INT_REGISTER_SIZE;
  offset += NUM_GREGS * INT_REGISTER_SIZE;
  first += NUM_GREGS;

  if (regnum < first + NUM_FPA_REGS)
    return offset + (regnum - first) * FPA_REGISTER_SIZE;
  offset += NUM_FPA_REGS * FPA_REGISTER_SIZE;
  first += NUM_FPA_REGS;

  if (regnum < first + NUM_SREGS)
    return offset + (regnum - first) * STATUS_REGISTER_SIZE;
  offset += NUM_SREGS * STATUS_REGISTER_SIZE;
  
  first = gdbarch_tdep (current_gdbarch)->first_iwmmxt_regnum;
  if (first != -1)
    {
      if (regnum >= first && regnum < first + NUM_IWMMXT_COP0REGS)
	return offset + (regnum - first) * IWMMXT_COP0_REGSIZE;

      offset += NUM_IWMMXT_COP0REGS * IWMMXT_COP0_REGSIZE;
      first += NUM_IWMMXT_COP0REGS;

      if (regnum >= first && regnum < first + NUM_IWMMXT_COP1REGS)
	return offset + (regnum - first) * IWMMXT_COP1_REGSIZE;

      offset += NUM_IWMMXT_COP1REGS * IWMMXT_COP1_REGSIZE;
    }

  first = gdbarch_tdep (current_gdbarch)->first_vfp_regnum;
  if (first != -1)
    {
      if (regnum >= first && regnum < first + NUM_VFP_XREGS)
	return offset + (regnum - first) * VFP_XREG_SIZE;

      offset += NUM_VFP_XREGS * VFP_XREG_SIZE;
      first += NUM_VFP_XREGS;

      if (regnum >= first && regnum < first + NUM_VFP_SREGS)
	return offset + (regnum - first) * VFP_SREG_SIZE;
    }
  internal_error (__FILE__, __LINE__, _("Bad REGNUM %d"), regnum);
}

static void
arm_pseudo_register_read (struct gdbarch *gdbarch,
			  struct regcache *regcache,
			  int regnum, void *buf)
{
  int first;
  int low_regnum;
  int high_regnum;

  first = gdbarch_tdep (current_gdbarch)->first_vfp_pseudo;
  if (first != -1)
    {
      first += gdbarch_num_regs (gdbarch);

      if (regnum >= first && regnum < first + NUM_VFP_PSEUDOS)
	{
	  regnum = gdbarch_tdep (current_gdbarch)->first_vfp_regnum
	    + NUM_VFP_XREGS + 2 * (regnum - first);
	  if (gdbarch_byte_order (gdbarch) == BFD_ENDIAN_LITTLE)
	    {
	      low_regnum = regnum;
	      high_regnum = regnum + 1;
	    }
	  else
	    {
	      low_regnum = regnum + 1;
	      high_regnum = regnum;
	    }
	  regcache_raw_read (regcache, low_regnum, buf);
	  regcache_raw_read (regcache, high_regnum, ((char *)buf) + 4);
	  return;
	}
    }
  internal_error (__FILE__, __LINE__, _("Bad pseudo %d"), regnum);
}

static void
arm_pseudo_register_write (struct gdbarch *gdbarch,
			   struct regcache *regcache,
			   int regnum, const void *buf)
{
  int first;
  int low_regnum;
  int high_regnum;

  first = gdbarch_tdep (current_gdbarch)->first_vfp_pseudo;
  if (first != -1)
    {
      first += gdbarch_num_regs (gdbarch);

      if (regnum >= first && regnum < first + NUM_VFP_PSEUDOS)
	{
	  regnum = gdbarch_tdep (current_gdbarch)->first_vfp_regnum
	    + NUM_VFP_XREGS + 2 * (regnum - first);
	  if (gdbarch_byte_order (gdbarch) == BFD_ENDIAN_LITTLE)
	    {
	      low_regnum = regnum;
	      high_regnum = regnum + 1;
	    }
	  else
	    {
	      low_regnum = regnum + 1;
	      high_regnum = regnum;
	    }
	  regcache_raw_write (regcache, low_regnum, buf);
	  regcache_raw_write (regcache, high_regnum + 1, ((char *)buf) + 4);
	  return;
	}
    }
  internal_error (__FILE__, __LINE__, _("Bad pseudo %d"), regnum);
}

/* Map DWARF register numbers onto internal GDB register numbers.  */
static int
arm_dwarf_reg_to_regnum (int reg)
{
  int first;
 
  if (reg >= 0 && reg <= 16)
    return reg; /* Core integer regs.  */
  if (reg >= 16 && reg <= 23)
    return ARM_F0_REGNUM + reg - 16; /* Legacy FPA encoding.  */
  if (reg >= 96 && reg <= 103)
    return ARM_F0_REGNUM + reg - 96; /* FPA regs.  */

  /* VFP regs.  */
  first = gdbarch_tdep (current_gdbarch)->first_vfp_regnum;
  if (first != -1 && reg >= 64 && reg <= 95);
      return first + NUM_VFP_XREGS + reg - 64;;

  /* iWMMXt regs.  */
  first = gdbarch_tdep (current_gdbarch)->first_iwmmxt_regnum;
  if (first != -1)
    {
      if (reg >= 112 && reg <= 127) /* wr0-wr15 */
	return first + reg - 112;
      first += NUM_IWMMXT_COP0REGS;
      if (reg >= 192 && reg <= 199) /* wc0-wc7 */
	return first + reg - 104;
      first += 8;
      if (reg >= 104 && reg <= 111) /*wcgr0-wcgr7 */
	return first + reg - 104;
    }

  warning (_("Unmapped DWARF Register #%d encountered."), reg);

  return -1;
}

/* Map GDB internal REGNUM onto the Arm simulator register numbers.  */
static int
arm_register_sim_regno (int regnum)
{
  int reg = regnum;
  gdb_assert (reg >= 0 && reg < NUM_REGS);

  if (gdbarch_tdep (current_gdbarch)->first_iwmmxt_regnum != -1)
    {
      int first = gdbarch_tdep (current_gdbarch)->first_iwmmxt_regnum;

      if (regnum >= first && regnum < first + NUM_IWMMXT_REGS)
	return SIM_ARM_IWMMXT_COP0R0_REGNUM + (regnum - first);
    }

  if (reg < NUM_GREGS)
    return SIM_ARM_R0_REGNUM + reg;
  reg -= NUM_GREGS;

  if (reg < NUM_FPA_REGS)
    return SIM_ARM_FP0_REGNUM + reg;
  reg -= NUM_FPA_REGS;

  if (reg < NUM_SREGS)
    return SIM_ARM_FPS_REGNUM + reg;
  reg -= NUM_SREGS;

  internal_error (__FILE__, __LINE__, _("Bad REGNUM %d"), regnum);
}

/* Map GDB internal REGNUM onto the current remote protocol
   register numbers.  */

static int
arm_register_remote_regno (int regnum)
{
  gdb_assert (regnum >= 0 && regnum < NUM_REGS);

  if (gdbarch_tdep (current_gdbarch)->first_iwmmxt_regnum != -1)
    {
      int first = gdbarch_tdep (current_gdbarch)->first_iwmmxt_regnum;

      if (regnum >= first && regnum < first + NUM_IWMMXT_REGS)
	return gdbarch_tdep (current_gdbarch)->target_iwmmxt_regnum
			     + (regnum - first);
    }

  if (gdbarch_tdep (current_gdbarch)->first_vfp_regnum != -1)
    {
      int first = gdbarch_tdep (current_gdbarch)->first_vfp_regnum;

      if (regnum >= first && regnum < first + NUM_VFP_XREGS + NUM_VFP_SREGS)
	return gdbarch_tdep (current_gdbarch)->target_vfp_regnum
			     + (regnum - first);
    }

  return regnum;
}

/* NOTE: cagney/2001-08-20: Both convert_from_extended() and
   convert_to_extended() use floatformat_arm_ext_littlebyte_bigword.
   It is thought that this is is the floating-point register format on
   little-endian systems.  */

static void
convert_from_extended (const struct floatformat *fmt, const void *ptr,
		       void *dbl)
{
  DOUBLEST d;
  if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
    floatformat_to_doublest (&floatformat_arm_ext_big, ptr, &d);
  else
    floatformat_to_doublest (&floatformat_arm_ext_littlebyte_bigword,
			     ptr, &d);
  floatformat_from_doublest (fmt, &d, dbl);
}

static void
convert_to_extended (const struct floatformat *fmt, void *dbl, const void *ptr)
{
  DOUBLEST d;
  floatformat_to_doublest (fmt, ptr, &d);
  if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
    floatformat_from_doublest (&floatformat_arm_ext_big, &d, dbl);
  else
    floatformat_from_doublest (&floatformat_arm_ext_littlebyte_bigword,
			       &d, dbl);
}

static int
condition_true (unsigned long cond, unsigned long status_reg)
{
  if (cond == INST_AL || cond == INST_NV)
    return 1;

  switch (cond)
    {
    case INST_EQ:
      return ((status_reg & FLAG_Z) != 0);
    case INST_NE:
      return ((status_reg & FLAG_Z) == 0);
    case INST_CS:
      return ((status_reg & FLAG_C) != 0);
    case INST_CC:
      return ((status_reg & FLAG_C) == 0);
    case INST_MI:
      return ((status_reg & FLAG_N) != 0);
    case INST_PL:
      return ((status_reg & FLAG_N) == 0);
    case INST_VS:
      return ((status_reg & FLAG_V) != 0);
    case INST_VC:
      return ((status_reg & FLAG_V) == 0);
    case INST_HI:
      return ((status_reg & (FLAG_C | FLAG_Z)) == FLAG_C);
    case INST_LS:
      return ((status_reg & (FLAG_C | FLAG_Z)) != FLAG_C);
    case INST_GE:
      return (((status_reg & FLAG_N) == 0) == ((status_reg & FLAG_V) == 0));
    case INST_LT:
      return (((status_reg & FLAG_N) == 0) != ((status_reg & FLAG_V) == 0));
    case INST_GT:
      return (((status_reg & FLAG_Z) == 0) &&
	      (((status_reg & FLAG_N) == 0) == ((status_reg & FLAG_V) == 0)));
    case INST_LE:
      return (((status_reg & FLAG_Z) != 0) ||
	      (((status_reg & FLAG_N) == 0) != ((status_reg & FLAG_V) == 0)));
    }
  return 1;
}

/* Support routines for single stepping.  Calculate the next PC value.  */
#define BranchDest(addr,instr) \
  ((CORE_ADDR) (((long) (addr)) + 8 + (sbits (instr, 0, 23) << 2)))
#define ARM_PC_32 1

static unsigned long
shifted_reg_val (unsigned long inst, int carry, unsigned long pc_val,
		 unsigned long status_reg)
{
  unsigned long res, shift;
  int rm = bits (inst, 0, 3);
  unsigned long shifttype = bits (inst, 5, 6);

  if (bit (inst, 4))
    {
      int rs = bits (inst, 8, 11);
      shift = (rs == 15 ? pc_val + 8 : read_register (rs)) & 0xFF;
    }
  else
    shift = bits (inst, 7, 11);

  res = (rm == 15
	 ? ((pc_val | (ARM_PC_32 ? 0 : status_reg))
	    + (bit (inst, 4) ? 12 : 8))
	 : read_register (rm));

  switch (shifttype)
    {
    case 0:			/* LSL */
      res = shift >= 32 ? 0 : res << shift;
      break;

    case 1:			/* LSR */
      res = shift >= 32 ? 0 : res >> shift;
      break;

    case 2:			/* ASR */
      if (shift >= 32)
	shift = 31;
      res = ((res & 0x80000000L)
	     ? ~((~res) >> shift) : res >> shift);
      break;

    case 3:			/* ROR/RRX */
      shift &= 31;
      if (shift == 0)
	res = (res >> 1) | (carry ? 0x80000000L : 0);
      else
	res = (res >> shift) | (res << (32 - shift));
      break;
    }

  return res & 0xffffffff;
}


/* Find the next instruction to be executed in an IT block.  */

static CORE_ADDR
thumb2_it_block (CORE_ADDR nextpc, unsigned long status, unsigned int cond,
                 unsigned int mask)
{
  int cond_true = condition_true (cond, status);
  int numinsns;

  if ((mask & 0x7) == 0)
    numinsns = 1;
  else if ((mask & 0x3) == 0)
    numinsns = 2;
  else if ((mask & 0x1) == 0)
    numinsns = 3;
  else
    numinsns = 4;

  for (; numinsns > 0; numinsns--, mask <<= 1)
    {
      int length;
      thumb_get_insn (nextpc, &length);

      if ((cond_true && (mask & 0x8))
	  || (!cond_true && !(mask & 0x8)))
	break;

      nextpc += length;
    }

  return nextpc;
}

/* Get next Thumb PC. We must consider both 16-bit and 32-bit instructions
   which can modify control flow. Thankfully, this doesn't include most of the
   ALU instructions (unlike ARM).  */

CORE_ADDR
thumb_get_next_pc (CORE_ADDR pc)
{
  int length;
  unsigned long inst = thumb_get_insn (pc, &length);
  unsigned long status = read_register (ARM_PS_REGNUM);
  CORE_ADDR nextpc = pc + length;
  unsigned long offset;

  /* Check CPSR to see if we're in the middle of an IT block.  */

  if (bits (status, 13, 15) != 0)
    {
      int cond = bits (status, 12, 15);
      int mask = bits (status, 25, 26) | (bits (status, 10, 11) << 2);
      return thumb2_it_block (nextpc, status, cond, mask);
    }

  /* Thumb/Thumb-2 16-bit instructions.  */
 
  /* If-Then. We must:
     (a) Decode the IT instruction.
     (b) Evaluate the current condition.
     (c) Insert breakpoint on one of the following four instructions, depending
     on the IT mask and (2).

     Subsequent conditional instructions are handled by decoding the CPSR,
     above.
  */
  if ((inst & 0xffffff00) == 0xbf00
      && (inst & 0x000f) != 0)
    {
      unsigned int mask = inst & 0xf;
      unsigned int cond = bits (inst, 4, 7);
      return thumb2_it_block (nextpc, status, cond, mask);
    }
  /* Compare zero and branch.  */
  else if ((inst & 0xfffff500) == 0xb100)
    {
      int reg = bits (inst, 0, 2);
      unsigned int regval = read_register (reg);
      int not_equal = bit (inst, 11);
      int do_branch = (regval == 0) ^ not_equal;
      unsigned int offset = (bits (inst, 3, 7) << 1) | (bit (inst, 9) << 6);
      if (do_branch)
	nextpc = pc + 4 + offset;
    }
  else if ((inst & 0xffffff00) == 0xbd00)         /* pop {rlist, pc} */
    {
      CORE_ADDR sp;

      /* Fetch the saved PC from the stack.  It's stored above
         all of the other registers.  */
      offset = bitcount (bits (inst, 0, 7)) * DEPRECATED_REGISTER_SIZE;
      sp = read_register (ARM_SP_REGNUM);
      nextpc = (CORE_ADDR) read_memory_integer (sp + offset, 4);
      nextpc = ADDR_BITS_REMOVE (nextpc);
    }
  else if ((inst & 0xfffff000) == 0xd000)         /* conditional branch */
    {
      unsigned long status = read_register (ARM_PS_REGNUM);
      unsigned long cond = bits (inst, 8, 11);
      if (cond != 0x0f && condition_true (cond, status))    /* 0x0f = SWI */
	nextpc = pc + 4 + (sbits (inst, 0, 7) << 1);
    }
  else if ((inst & 0xfffff800) == 0xe000)         /* unconditional branch */
    {
      nextpc = pc + 4 + (sbits (inst, 0, 10) << 1);
    }
  else if ((inst & 0xffffff00) == 0x4700)         /* bx REG, blx REG */
    {
      if (bits (inst, 3, 6) == 0x0f)
	nextpc = pc + 4;
      else
	nextpc = read_register (bits (inst, 3, 6));

      nextpc = ADDR_BITS_REMOVE (nextpc);
    }

  /* 32-bit instructions.  */

  else if ((inst & 0xf800d000) == 0xf0009000      /* B (unconditional)  */
           || (inst & 0xf800d000) == 0xf000d000)  /* BL  */
    {
      unsigned int s = bit (inst, 26);
      unsigned int i1 = bit (inst, 13);
      unsigned int i2 = bit (inst, 11);
      unsigned int offset = (bits (inst, 0, 10) << 1)
	                    | (bits (inst, 16, 25) << 12)
			    | (!(i2 ^ s) << 22)
			    | (!(i1 ^ s) << 23)
			    | (s << 24);
      nextpc = pc + 4 + sbits (offset, 0, 24);
    }
  else if ((inst & 0xf800d000) == 0xf0008000)     /* B (conditional)  */
    {
      unsigned int s = bit (inst, 26);
      unsigned int j1 = bit (inst, 13);
      unsigned int j2 = bit (inst, 11);
      unsigned int offset = (bits (inst, 0, 10) << 1)
	                  | (bits (inst, 16, 21) << 12)
			  | (j1 << 18)
			  | (j2 << 19)
			  | (s << 20);
      unsigned int cond = bits (inst, 22, 25);
      int cond_true = condition_true (cond, status);
      if (cond_true)
	nextpc = pc + 4 + sbits (offset, 0, 20);
    }
  else if ((inst & 0xf800d001) == 0xf000c000)     /* BLX  */
    {
      unsigned int s = bit (inst, 26);
      unsigned int i1 = bit (inst, 13);
      unsigned int i2 = bit (inst, 11);
      unsigned int offset = (bits (inst, 1, 10) << 2)
	                    | (bits (inst, 16, 25) << 12)
			    | (!(i2 ^ s) << 22)
			    | (!(i1 ^ s) << 23)
			    | (s << 24);
      nextpc = ((pc + 4) & 0xfffffffc) + sbits (offset, 0, 24);
    }
  else if (((inst & 0xffd08000) == 0xe9108000     /* LDM rb,{..pc..}  */
            || (inst & 0xffd08000) == 0xe8908000)
           && bitcount (inst & 0x0000dfff) > 1)
    {
      int incr_after = bit (inst, 23);
      /* We know the PC is loaded because of the mask we use to detect the
         instruction. LR cannot be loaded at the same time, so just count
	 r0-r12.  */
      int numregs = bitcount (bits (inst, 0, 12));
      CORE_ADDR base = read_register (bits (inst, 16, 19));
      CORE_ADDR pcaddr = incr_after ? base + 4 * numregs : base - 4;
      nextpc = read_memory_integer (pcaddr, 4) & 0xfffffffe;
    }
  else if ((inst & 0xfff0f000) == 0xf8d0f000)     /* LDR pc, [rn, #imm]  */
    {
      CORE_ADDR base = read_register (bits (inst, 16, 19));
      int offset = bits (inst, 0, 11);
      nextpc = read_memory_integer (base + offset, 4) & 0xfffffffe;
    }
  else if ((inst & 0xfff0ff00) == 0xf850fc00)     /* LDR pc, [rn, #-imm]  */
    {
      CORE_ADDR base = read_register (bits (inst, 16, 19));
      int offset = bits (inst, 0, 7);
      nextpc = read_memory_integer (base - offset, 4) & 0xfffffffe;
    }
  else if ((inst & 0xfff0fd00) == 0xf850f900)     /* LDR pc, [rn], #+/-imm  */
    {
      CORE_ADDR addr = read_register (bits (inst, 16, 19));
      nextpc = read_memory_integer (addr, 4) & 0xfffffffe;
    }
  else if ((inst & 0xfff0fd00) == 0xf850fd00)     /* LDR pc, [rn, #+/-imm]  */
    {
      CORE_ADDR base = read_register (bits (inst, 16, 19));
      int plus = bit (inst, 9);
      int offset = bits (inst, 0, 7);
      CORE_ADDR addr = plus ? base + offset : base - offset;
      nextpc = read_memory_integer (addr, 4) & 0xfffffffe;
    }
  else if ((inst & 0xfff0ffc0) ==0xf850f000)      /* LDR pc, [rn, rm, lsl]  */
    {
      CORE_ADDR base = read_register (bits (inst, 16, 19));
      CORE_ADDR index = read_register (bits (inst, 0, 3));
      int shift = bits (inst, 4, 5);
      nextpc = read_memory_integer (base + (index << shift), 4) & 0xfffffffe;
    }
  else if ((inst & 0xff7ff000) == 0xf85ff000)     /* LDR pc, [pc, #+/-imm]  */
    {
      CORE_ADDR base = (pc + 4) & 0xfffffffc;
      int plus = bit (inst, 23);
      int offset = bits (inst, 0, 11);
      CORE_ADDR addr = plus ? base + offset : base - offset;
      nextpc = read_memory_integer (addr, 4) & 0xfffffffe;
    }
  else if (((inst & 0xffd00000) == 0xe8100000)    /* RFE  */
           || ((inst & 0xffd00000) == 0xe9900000))
    {
      unsigned int incr_after = bit (inst, 23);
      CORE_ADDR base = read_register (bits (inst, 16, 19));
      CORE_ADDR pcaddr = incr_after ? base : base - 8;
      nextpc = read_memory_integer (pcaddr, 4) & 0xfffffffe;
    }
  else if ((inst & 0xffffdf00) == 0xf3de8f00)     /* SUBS PC, LR, #imm  */
    {
      int imm = bits (inst, 0, 7);
      CORE_ADDR lr = read_register (ARM_LR_REGNUM);
      nextpc = lr - imm;
    }
  else if ((inst & 0xfff000f0) == 0xe8d00000)     /* TBB  */
    {
      unsigned int rn = bits (inst, 16, 19);
      CORE_ADDR base = (rn == ARM_PC_REGNUM) ? pc + 4 : read_register (rn);
      unsigned int index = read_register (bits (inst, 0, 3));
      unsigned int offset = read_memory_unsigned_integer (base + index, 1) << 1;
      nextpc = pc + offset;
    }
  else if ((inst & 0xfff000f0) == 0xe8d00010)     /* TBH  */
    {
      unsigned int rn = bits (inst, 16, 19);
      CORE_ADDR base = (rn == ARM_PC_REGNUM) ? pc + 4 : read_register (rn);
      unsigned int index = read_register (bits (inst, 0, 3)) << 1;
      unsigned int offset = read_memory_unsigned_integer (base + index, 2) << 1;
      nextpc = pc + offset;
    }

  if (nextpc == pc)
    error (_("Infinite loop detected"));

  return nextpc;
}

CORE_ADDR
arm_get_next_pc (CORE_ADDR pc)
{
  unsigned long pc_val;
  unsigned long this_instr;
  unsigned long status;
  CORE_ADDR nextpc;

  if (arm_pc_is_thumb (pc))
    return thumb_get_next_pc (pc);

  pc_val = (unsigned long) pc;
  this_instr = read_memory_integer (pc, 4);
  status = read_register (ARM_PS_REGNUM);
  nextpc = (CORE_ADDR) (pc_val + 4);	/* Default case */

  if (condition_true (bits (this_instr, 28, 31), status))
    {
      switch (bits (this_instr, 24, 27))
	{
	case 0x0:
	case 0x1:			/* data processing */
	case 0x2:
	case 0x3:
	  {
	    unsigned long operand1, operand2, result = 0;
	    unsigned long rn;
	    int c;

	    if (bits (this_instr, 12, 15) != 15)
	      break;

	    if (bits (this_instr, 22, 25) == 0
		&& bits (this_instr, 4, 7) == 9)	/* multiply */
	      error (_("Invalid update to pc in instruction"));

	    /* BX <reg>, BLX <reg> */
	    if (bits (this_instr, 4, 28) == 0x12fff1
		|| bits (this_instr, 4, 28) == 0x12fff3)
	      {
		rn = bits (this_instr, 0, 3);
		result = (rn == 15) ? pc_val + 8 : read_register (rn);
		nextpc = (CORE_ADDR) ADDR_BITS_REMOVE (result);

		if (nextpc == pc)
		  error (_("Infinite loop detected"));

		return nextpc;
	      }

	    /* Multiply into PC */
	    c = (status & FLAG_C) ? 1 : 0;
	    rn = bits (this_instr, 16, 19);
	    operand1 = (rn == 15) ? pc_val + 8 : read_register (rn);

	    if (bit (this_instr, 25))
	      {
		unsigned long immval = bits (this_instr, 0, 7);
		unsigned long rotate = 2 * bits (this_instr, 8, 11);
		operand2 = ((immval >> rotate) | (immval << (32 - rotate)))
		  & 0xffffffff;
	      }
	    else		/* operand 2 is a shifted register */
	      operand2 = shifted_reg_val (this_instr, c, pc_val, status);

	    switch (bits (this_instr, 21, 24))
	      {
	      case 0x0:	/*and */
		result = operand1 & operand2;
		break;

	      case 0x1:	/*eor */
		result = operand1 ^ operand2;
		break;

	      case 0x2:	/*sub */
		result = operand1 - operand2;
		break;

	      case 0x3:	/*rsb */
		result = operand2 - operand1;
		break;

	      case 0x4:	/*add */
		result = operand1 + operand2;
		break;

	      case 0x5:	/*adc */
		result = operand1 + operand2 + c;
		break;

	      case 0x6:	/*sbc */
		result = operand1 - operand2 + c;
		break;

	      case 0x7:	/*rsc */
		result = operand2 - operand1 + c;
		break;

	      case 0x8:
	      case 0x9:
	      case 0xa:
	      case 0xb:	/* tst, teq, cmp, cmn */
		result = (unsigned long) nextpc;
		break;

	      case 0xc:	/*orr */
		result = operand1 | operand2;
		break;

	      case 0xd:	/*mov */
		/* Always step into a function.  */
		result = operand2;
		break;

	      case 0xe:	/*bic */
		result = operand1 & ~operand2;
		break;

	      case 0xf:	/*mvn */
		result = ~operand2;
		break;
	      }
	    nextpc = (CORE_ADDR) ADDR_BITS_REMOVE (result);

	    if (nextpc == pc)
	      error (_("Infinite loop detected"));
	    break;
	  }

	case 0x4:
	case 0x5:		/* data transfer */
	case 0x6:
	case 0x7:
	  if (bit (this_instr, 20))
	    {
	      /* load */
	      if (bits (this_instr, 12, 15) == 15)
		{
		  /* rd == pc */
		  unsigned long rn;
		  unsigned long base;

		  if (bit (this_instr, 22))
		    error (_("Invalid update to pc in instruction"));

		  /* byte write to PC */
		  rn = bits (this_instr, 16, 19);
		  base = (rn == 15) ? pc_val + 8 : read_register (rn);
		  if (bit (this_instr, 24))
		    {
		      /* pre-indexed */
		      int c = (status & FLAG_C) ? 1 : 0;
		      unsigned long offset =
		      (bit (this_instr, 25)
		       ? shifted_reg_val (this_instr, c, pc_val, status)
		       : bits (this_instr, 0, 11));

		      if (bit (this_instr, 23))
			base += offset;
		      else
			base -= offset;
		    }
		  nextpc = (CORE_ADDR) read_memory_integer ((CORE_ADDR) base,
							    4);

		  nextpc = ADDR_BITS_REMOVE (nextpc);

		  if (nextpc == pc)
		    error (_("Infinite loop detected"));
		}
	    }
	  break;

	case 0x8:
	case 0x9:		/* block transfer */
	  if (bit (this_instr, 20))
	    {
	      /* LDM */
	      if (bit (this_instr, 15))
		{
		  /* loading pc */
		  int offset = 0;

		  if (bit (this_instr, 23))
		    {
		      /* up */
		      unsigned long reglist = bits (this_instr, 0, 14);
		      offset = bitcount (reglist) * 4;
		      if (bit (this_instr, 24))		/* pre */
			offset += 4;
		    }
		  else if (bit (this_instr, 24))
		    offset = -4;

		  {
		    unsigned long rn_val =
		    read_register (bits (this_instr, 16, 19));
		    nextpc =
		      (CORE_ADDR) read_memory_integer ((CORE_ADDR) (rn_val
								  + offset),
						       4);
		  }
		  nextpc = ADDR_BITS_REMOVE (nextpc);
		  if (nextpc == pc)
		    error (_("Infinite loop detected"));
		}
	    }
	  break;

	case 0xb:		/* branch & link */
	case 0xa:		/* branch */
	  {
	    nextpc = BranchDest (pc, this_instr);

	    /* BLX */
	    if (bits (this_instr, 28, 31) == INST_NV)
	      nextpc |= bit (this_instr, 24) << 1;

	    nextpc = ADDR_BITS_REMOVE (nextpc);
	    if (nextpc == pc)
	      error (_("Infinite loop detected"));
	    break;
	  }

	case 0xc:
	case 0xd:
	case 0xe:		/* coproc ops */
	case 0xf:		/* SWI */
	  break;

	default:
	  fprintf_filtered (gdb_stderr, _("Bad bit-field extraction\n"));
	  return (pc);
	}
    }

  return nextpc;
}

/* single_step() is called just before we want to resume the inferior,
   if we want to single-step it but there is no hardware or kernel
   single-step support.  We find the target of the coming instruction
   and breakpoint it.

   single_step() is also called just after the inferior stops.  If we
   had set up a simulated single-step, we undo our damage.  */

static void
arm_software_single_step (enum target_signal sig, int insert_bpt)
{
  static int next_pc;		 /* State between setting and unsetting.  */
  static char break_mem[BREAKPOINT_MAX]; /* Temporary storage for mem@bpt */

  if (insert_bpt)
    {
      next_pc = arm_get_next_pc (read_register (ARM_PC_REGNUM));
      target_insert_breakpoint (next_pc, break_mem);
    }
  else
    target_remove_breakpoint (next_pc, break_mem);
}

#include "bfd-in2.h"
#include "libcoff.h"

static int
gdb_print_insn_arm (bfd_vma memaddr, disassemble_info *info)
{
  if (arm_pc_is_thumb (memaddr))
    {
      static asymbol *asym;
      static combined_entry_type ce;
      static struct coff_symbol_struct csym;
      static struct bfd fake_bfd;
      static bfd_target fake_target;

      if (csym.native == NULL)
	{
	  /* Create a fake symbol vector containing a Thumb symbol.
	     This is solely so that the code in print_insn_little_arm() 
	     and print_insn_big_arm() in opcodes/arm-dis.c will detect
	     the presence of a Thumb symbol and switch to decoding
	     Thumb instructions.  */

	  fake_target.flavour = bfd_target_coff_flavour;
	  fake_bfd.xvec = &fake_target;
	  ce.u.syment.n_sclass = C_THUMBEXTFUNC;
	  csym.native = &ce;
	  csym.symbol.the_bfd = &fake_bfd;
	  csym.symbol.name = "fake";
	  asym = (asymbol *) & csym;
	}

      memaddr = UNMAKE_THUMB_ADDR (memaddr);
      info->symbols = &asym;
    }
  else
    info->symbols = NULL;

  if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
    return print_insn_big_arm (memaddr, info);
  else
    return print_insn_little_arm (memaddr, info);
}

/* The following define instruction sequences that will cause ARM
   cpu's to take an undefined instruction trap.  These are used to
   signal a breakpoint to GDB.
   
   The newer ARMv4T cpu's are capable of operating in ARM or Thumb
   modes.  A different instruction is required for each mode.  The ARM
   cpu's can also be big or little endian.  Thus four different
   instructions are needed to support all cases.
   
   Note: ARMv4 defines several new instructions that will take the
   undefined instruction trap.  ARM7TDMI is nominally ARMv4T, but does
   not in fact add the new instructions.  The new undefined
   instructions in ARMv4 are all instructions that had no defined
   behaviour in earlier chips.  There is no guarantee that they will
   raise an exception, but may be treated as NOP's.  In practice, it
   may only safe to rely on instructions matching:
   
   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 
   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
   C C C C 0 1 1 x x x x x x x x x x x x x x x x x x x x 1 x x x x
   
   Even this may only true if the condition predicate is true. The
   following use a condition predicate of ALWAYS so it is always TRUE.
   
   There are other ways of forcing a breakpoint.  GNU/Linux, RISC iX,
   and NetBSD all use a software interrupt rather than an undefined
   instruction to force a trap.  This can be handled by by the
   abi-specific code during establishment of the gdbarch vector.  */


/* NOTE rearnsha 2002-02-18: for now we allow a non-multi-arch gdb to
   override these definitions.  */
#ifndef ARM_LE_BREAKPOINT
#define ARM_LE_BREAKPOINT {0xFE,0xDE,0xFF,0xE7}
#endif
#ifndef ARM_BE_BREAKPOINT
#define ARM_BE_BREAKPOINT {0xE7,0xFF,0xDE,0xFE}
#endif
#ifndef THUMB_LE_BREAKPOINT
#define THUMB_LE_BREAKPOINT {0xfe,0xdf}
#endif
#ifndef THUMB_BE_BREAKPOINT
#define THUMB_BE_BREAKPOINT {0xdf,0xfe}
#endif

static const char arm_default_arm_le_breakpoint[] = ARM_LE_BREAKPOINT;
static const char arm_default_arm_be_breakpoint[] = ARM_BE_BREAKPOINT;
static const char arm_default_thumb_le_breakpoint[] = THUMB_LE_BREAKPOINT;
static const char arm_default_thumb_be_breakpoint[] = THUMB_BE_BREAKPOINT;

/* Determine the type and size of breakpoint to insert at PCPTR.  Uses
   the program counter value to determine whether a 16-bit or 32-bit
   breakpoint should be used.  It returns a pointer to a string of
   bytes that encode a breakpoint instruction, stores the length of
   the string to *lenptr, and adjusts the program counter (if
   necessary) to point to the actual memory location where the
   breakpoint should be inserted.  */

/* XXX ??? from old tm-arm.h: if we're using RDP, then we're inserting
   breakpoints and storing their handles instread of what was in
   memory.  It is nice that this is the same size as a handle -
   otherwise remote-rdp will have to change.  */

static const unsigned char *
arm_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  if (arm_pc_is_thumb (*pcptr))
    {
      *pcptr = UNMAKE_THUMB_ADDR (*pcptr);
      *lenptr = tdep->thumb_breakpoint_size;
      return tdep->thumb_breakpoint;
    }
  else
    {
      *lenptr = tdep->arm_breakpoint_size;
      return tdep->arm_breakpoint;
    }
}

/* Extract from an array REGBUF containing the (raw) register state a
   function return value of type TYPE, and copy that, in virtual
   format, into VALBUF.  */

static void
arm_extract_return_value (struct type *type, struct regcache *regs,
			  gdb_byte *valbuf)
{
  if (TYPE_CODE_FLT == TYPE_CODE (type))
    {
      switch (gdbarch_tdep (current_gdbarch)->fp_model)
	{
	case ARM_FLOAT_FPA:
	  {
	    /* The value is in register F0 in internal format.  We need to
	       extract the raw value and then convert it to the desired
	       internal type.  */
	    bfd_byte tmpbuf[FPA_REGISTER_SIZE];

	    regcache_cooked_read (regs, ARM_F0_REGNUM, tmpbuf);
	    convert_from_extended (floatformat_from_type (type), tmpbuf,
				   valbuf);
	  }
	  break;

	case ARM_FLOAT_SOFT_FPA:
	case ARM_FLOAT_SOFT_VFP:
	  regcache_cooked_read (regs, ARM_A1_REGNUM, valbuf);
	  if (TYPE_LENGTH (type) > 4)
	    regcache_cooked_read (regs, ARM_A1_REGNUM + 1,
				  valbuf + INT_REGISTER_SIZE);
	  break;

	default:
	  internal_error
	    (__FILE__, __LINE__,
	     _("arm_extract_return_value: Floating point model not supported"));
	  break;
	}
    }
  else if (TYPE_CODE (type) == TYPE_CODE_INT
	   || TYPE_CODE (type) == TYPE_CODE_CHAR
	   || TYPE_CODE (type) == TYPE_CODE_BOOL
	   || TYPE_CODE (type) == TYPE_CODE_PTR
	   || TYPE_CODE (type) == TYPE_CODE_REF
	   || TYPE_CODE (type) == TYPE_CODE_ENUM)
    {
      /* If the the type is a plain integer, then the access is
	 straight-forward.  Otherwise we have to play around a bit more.  */
      int len = TYPE_LENGTH (type);
      int regno = ARM_A1_REGNUM;
      ULONGEST tmp;

      while (len > 0)
	{
	  /* By using store_unsigned_integer we avoid having to do
	     anything special for small big-endian values.  */
	  regcache_cooked_read_unsigned (regs, regno++, &tmp);
	  store_unsigned_integer (valbuf, 
				  (len > INT_REGISTER_SIZE
				   ? INT_REGISTER_SIZE : len),
				  tmp);
	  len -= INT_REGISTER_SIZE;
	  valbuf += INT_REGISTER_SIZE;
	}
    }
  else
    {
      /* For a structure or union the behaviour is as if the value had
         been stored to word-aligned memory and then loaded into 
         registers with 32-bit load instruction(s).  */
      int len = TYPE_LENGTH (type);
      int regno = ARM_A1_REGNUM;
      bfd_byte tmpbuf[INT_REGISTER_SIZE];

      while (len > 0)
	{
	  regcache_cooked_read (regs, regno++, tmpbuf);
	  memcpy (valbuf, tmpbuf,
		  len > INT_REGISTER_SIZE ? INT_REGISTER_SIZE : len);
	  len -= INT_REGISTER_SIZE;
	  valbuf += INT_REGISTER_SIZE;
	}
    }
}


/* Will a function return an aggregate type in memory or in a
   register?  Return 0 if an aggregate type can be returned in a
   register, 1 if it must be returned in memory.  */

static int
arm_return_in_memory (struct gdbarch *gdbarch, struct type *type)
{
  int nRc;
  enum type_code code;

  CHECK_TYPEDEF (type);

  /* In the ARM ABI, "integer" like aggregate types are returned in
     registers.  For an aggregate type to be integer like, its size
     must be less than or equal to DEPRECATED_REGISTER_SIZE and the
     offset of each addressable subfield must be zero.  Note that bit
     fields are not addressable, and all addressable subfields of
     unions always start at offset zero.

     This function is based on the behaviour of GCC 2.95.1.
     See: gcc/arm.c: arm_return_in_memory() for details.

     Note: All versions of GCC before GCC 2.95.2 do not set up the
     parameters correctly for a function returning the following
     structure: struct { float f;}; This should be returned in memory,
     not a register.  Richard Earnshaw sent me a patch, but I do not
     know of any way to detect if a function like the above has been
     compiled with the correct calling convention.  */

  /* All aggregate types that won't fit in a register must be returned
     in memory.  */
  if (TYPE_LENGTH (type) > DEPRECATED_REGISTER_SIZE)
    {
      return 1;
    }

  /* The AAPCS says all aggregates not larger than a word are returned
     in a register.  */
  if (gdbarch_tdep (gdbarch)->arm_abi != ARM_ABI_APCS)
    return 0;

  /* The only aggregate types that can be returned in a register are
     structs and unions.  Arrays must be returned in memory.  */
  code = TYPE_CODE (type);
  if ((TYPE_CODE_STRUCT != code) && (TYPE_CODE_UNION != code))
    {
      return 1;
    }

  /* Assume all other aggregate types can be returned in a register.
     Run a check for structures, unions and arrays.  */
  nRc = 0;

  if ((TYPE_CODE_STRUCT == code) || (TYPE_CODE_UNION == code))
    {
      int i;
      /* Need to check if this struct/union is "integer" like.  For
         this to be true, its size must be less than or equal to
         DEPRECATED_REGISTER_SIZE and the offset of each addressable
         subfield must be zero.  Note that bit fields are not
         addressable, and unions always start at offset zero.  If any
         of the subfields is a floating point type, the struct/union
         cannot be an integer type.  */

      /* For each field in the object, check:
         1) Is it FP? --> yes, nRc = 1;
         2) Is it addressable (bitpos != 0) and
         not packed (bitsize == 0)?
         --> yes, nRc = 1  
       */

      for (i = 0; i < TYPE_NFIELDS (type); i++)
	{
	  enum type_code field_type_code;
	  field_type_code = TYPE_CODE (check_typedef (TYPE_FIELD_TYPE (type, i)));

	  /* Is it a floating point type field?  */
	  if (field_type_code == TYPE_CODE_FLT)
	    {
	      nRc = 1;
	      break;
	    }

	  /* If bitpos != 0, then we have to care about it.  */
	  if (TYPE_FIELD_BITPOS (type, i) != 0)
	    {
	      /* Bitfields are not addressable.  If the field bitsize is 
	         zero, then the field is not packed.  Hence it cannot be
	         a bitfield or any other packed type.  */
	      if (TYPE_FIELD_BITSIZE (type, i) == 0)
		{
		  nRc = 1;
		  break;
		}
	    }
	}
    }

  return nRc;
}

/* Write into appropriate registers a function return value of type
   TYPE, given in virtual format.  */

static void
arm_store_return_value (struct type *type, struct regcache *regs,
			const gdb_byte *valbuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    {
      char buf[MAX_REGISTER_SIZE];

      switch (gdbarch_tdep (current_gdbarch)->fp_model)
	{
	case ARM_FLOAT_FPA:

	  convert_to_extended (floatformat_from_type (type), buf, valbuf);
	  regcache_cooked_write (regs, ARM_F0_REGNUM, buf);
	  break;

	case ARM_FLOAT_SOFT_FPA:
	case ARM_FLOAT_SOFT_VFP:
	  regcache_cooked_write (regs, ARM_A1_REGNUM, valbuf);
	  if (TYPE_LENGTH (type) > 4)
	    regcache_cooked_write (regs, ARM_A1_REGNUM + 1, 
				   valbuf + INT_REGISTER_SIZE);
	  break;

	default:
	  internal_error
	    (__FILE__, __LINE__,
	     _("arm_store_return_value: Floating point model not supported"));
	  break;
	}
    }
  else if (TYPE_CODE (type) == TYPE_CODE_INT
	   || TYPE_CODE (type) == TYPE_CODE_CHAR
	   || TYPE_CODE (type) == TYPE_CODE_BOOL
	   || TYPE_CODE (type) == TYPE_CODE_PTR
	   || TYPE_CODE (type) == TYPE_CODE_REF
	   || TYPE_CODE (type) == TYPE_CODE_ENUM)
    {
      if (TYPE_LENGTH (type) <= 4)
	{
	  /* Values of one word or less are zero/sign-extended and
	     returned in r0.  */
	  bfd_byte tmpbuf[INT_REGISTER_SIZE];
	  LONGEST val = unpack_long (type, valbuf);

	  store_signed_integer (tmpbuf, INT_REGISTER_SIZE, val);
	  regcache_cooked_write (regs, ARM_A1_REGNUM, tmpbuf);
	}
      else
	{
	  /* Integral values greater than one word are stored in consecutive
	     registers starting with r0.  This will always be a multiple of
	     the regiser size.  */
	  int len = TYPE_LENGTH (type);
	  int regno = ARM_A1_REGNUM;

	  while (len > 0)
	    {
	      regcache_cooked_write (regs, regno++, valbuf);
	      len -= INT_REGISTER_SIZE;
	      valbuf += INT_REGISTER_SIZE;
	    }
	}
    }
  else
    {
      /* For a structure or union the behaviour is as if the value had
         been stored to word-aligned memory and then loaded into 
         registers with 32-bit load instruction(s).  */
      int len = TYPE_LENGTH (type);
      int regno = ARM_A1_REGNUM;
      bfd_byte tmpbuf[INT_REGISTER_SIZE];

      while (len > 0)
	{
	  memcpy (tmpbuf, valbuf,
		  len > INT_REGISTER_SIZE ? INT_REGISTER_SIZE : len);
	  regcache_cooked_write (regs, regno++, tmpbuf);
	  len -= INT_REGISTER_SIZE;
	  valbuf += INT_REGISTER_SIZE;
	}
    }
}


/* Handle function return values.  */

static enum return_value_convention
arm_return_value (struct gdbarch *gdbarch, struct type *valtype,
		  struct regcache *regcache, void *readbuf,
		  const void *writebuf)
{
  if (TYPE_CODE (valtype) == TYPE_CODE_STRUCT
      || TYPE_CODE (valtype) == TYPE_CODE_UNION
      || TYPE_CODE (valtype) == TYPE_CODE_ARRAY)
    {
      if (arm_return_in_memory (gdbarch, valtype))
	return RETURN_VALUE_STRUCT_CONVENTION;
    }

  if (writebuf)
    arm_store_return_value (valtype, regcache, writebuf);

  if (readbuf)
    arm_extract_return_value (valtype, regcache, readbuf);

  return RETURN_VALUE_REGISTER_CONVENTION;
}


static int
arm_get_longjmp_target (CORE_ADDR *pc)
{
  CORE_ADDR jb_addr;
  char buf[INT_REGISTER_SIZE];
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  
  jb_addr = read_register (ARM_A1_REGNUM);

  if (target_read_memory (jb_addr + tdep->jb_pc * tdep->jb_elt_size, buf,
			  INT_REGISTER_SIZE))
    return 0;

  *pc = extract_unsigned_integer (buf, INT_REGISTER_SIZE);
  return 1;
}

/* Return non-zero if the PC is inside a thumb call thunk.  */

int
arm_in_call_stub (CORE_ADDR pc, char *name)
{
  CORE_ADDR start_addr;

  /* Find the starting address of the function containing the PC.  If
     the caller didn't give us a name, look it up at the same time.  */
  if (0 == find_pc_partial_function (pc, name ? NULL : &name, 
				     &start_addr, NULL))
    return 0;

  return strncmp (name, "_call_via_r", 11) == 0;
}

/* If PC is in a Thumb call or return stub, return the address of the
   target PC, which is in a register.  The thunk functions are called
   _called_via_xx, where x is the register name.  The possible names
   are r0-r9, sl, fp, ip, sp, and lr.  */

CORE_ADDR
arm_skip_stub (CORE_ADDR pc)
{
  char *name;
  CORE_ADDR start_addr;

  /* Find the starting address and name of the function containing the PC.  */
  if (find_pc_partial_function (pc, &name, &start_addr, NULL) == 0)
    return 0;

  /* Call thunks always start with "_call_via_".  */
  if (strncmp (name, "_call_via_", 10) == 0)
    {
      /* Use the name suffix to determine which register contains the
         target PC.  */
      static char *table[15] =
      {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
       "r8", "r9", "sl", "fp", "ip", "sp", "lr"
      };
      int regno;

      for (regno = 0; regno <= 14; regno++)
	if (strcmp (&name[10], table[regno]) == 0)
	  return read_register (regno);
    }

  return 0;			/* not a stub */
}

static void
set_arm_command (char *args, int from_tty)
{
  printf_unfiltered (_("\
\"set arm\" must be followed by an apporpriate subcommand.\n"));
  help_list (setarmcmdlist, "set arm ", all_commands, gdb_stdout);
}

static void
show_arm_command (char *args, int from_tty)
{
  cmd_show_list (showarmcmdlist, from_tty, "");
}

static void
arm_update_current_architecture (void)
{
  struct gdbarch_info info;

  /* If the current architecture is not ARM, we have nothing to do.  */
  if (gdbarch_bfd_arch_info (current_gdbarch)->arch != bfd_arch_arm)
    return;

  /* Update the architecture.  */
  gdbarch_info_init (&info);

  if (!gdbarch_update_p (info))
    internal_error (__FILE__, __LINE__, "could not update architecture");
}

static void
set_fp_model_sfunc (char *args, int from_tty,
		    struct cmd_list_element *c)
{
  enum arm_float_model fp_model;

  for (fp_model = ARM_FLOAT_AUTO; fp_model != ARM_FLOAT_LAST; fp_model++)
    if (strcmp (current_fp_model, fp_model_strings[fp_model]) == 0)
      {
	arm_fp_model = fp_model;
	break;
      }

  if (fp_model == ARM_FLOAT_LAST)
    internal_error (__FILE__, __LINE__, _("Invalid fp model accepted: %s."),
		    current_fp_model);

  arm_update_current_architecture ();
}

static void
show_fp_model (struct ui_file *file, int from_tty,
	       struct cmd_list_element *c, const char *value)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  if (arm_fp_model == ARM_FLOAT_AUTO
      && gdbarch_bfd_arch_info (current_gdbarch)->arch == bfd_arch_arm)
    fprintf_filtered (file, _("\
The current ARM floating point model is \"auto\" (currently \"%s\").\n"),
		      fp_model_strings[tdep->fp_model]);
  else
    fprintf_filtered (file, _("\
The current ARM floating point model is \"%s\".\n"),
		      fp_model_strings[arm_fp_model]);
}

static void
arm_set_abi (char *args, int from_tty,
	     struct cmd_list_element *c)
{
  enum arm_abi_kind arm_abi;

  for (arm_abi = ARM_ABI_AUTO; arm_abi != ARM_ABI_LAST; arm_abi++)
    if (strcmp (arm_abi_string, arm_abi_strings[arm_abi]) == 0)
      {
	arm_abi_global = arm_abi;
	break;
      }

  if (arm_abi == ARM_ABI_LAST)
    internal_error (__FILE__, __LINE__, _("Invalid ABI accepted: %s."),
		    arm_abi_string);

  arm_update_current_architecture ();
}

static void
arm_show_abi (struct ui_file *file, int from_tty,
	     struct cmd_list_element *c, const char *value)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  if (arm_abi_global == ARM_ABI_AUTO
      && gdbarch_bfd_arch_info (current_gdbarch)->arch == bfd_arch_arm)
    fprintf_filtered (file, _("\
The current ARM ABI is \"auto\" (currently \"%s\").\n"),
		      arm_abi_strings[tdep->arm_abi]);
  else
    fprintf_filtered (file, _("The current ARM ABI is \"%s\".\n"),
		      arm_abi_string);
}

/* If the user changes the register disassembly style used for info
   register and other commands, we have to also switch the style used
   in opcodes for disassembly output.  This function is run in the "set
   arm disassembly" command, and does that.  */

static void
set_disassembly_style_sfunc (char *args, int from_tty,
			      struct cmd_list_element *c)
{
  set_disassembly_style ();
}

/* Return the ARM register name corresponding to register I.  */
static const char *
arm_register_name (int regnum)
{
  int first;

  first = gdbarch_tdep (current_gdbarch)->first_iwmmxt_regnum;
  if (first != -1)
    {
      static const char *const iwmmxt_register_names[] = 
	{
	  "wr0", "wr1", "wr2", "wr3", "wr4", "wr5", "wr6", "wr7",
	  "wr8", "wr9", "wr10", "wr11", "wr12", "wr13", "wr14", "wr15",
	  "wcid", "wcon", "wcssf", "wcasf", "", "", "", "",
	  "wcgr0", "wcgr1", "wcgr2", "wcgr3", "", "", "", ""
	};

      if (regnum >= first && regnum < first + NUM_IWMMXT_REGS)
	return iwmmxt_register_names[regnum - first];
    }

  first = gdbarch_tdep (current_gdbarch)->first_vfp_regnum;
  if (first != -1)
    {
      static const char *const vfp_register_names[] = 
	{
	  "fpsid", "fpscr", "", "", "", "", "", "",
	  "fpexc", "", "", "", "", "", "", "",
	  "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
	  "s8",  "s9",  "s10", "s11", "s12", "s13", "s14", "s15",
	  "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
	  "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31"
	};

      if (regnum >= first && regnum < first + NUM_VFP_XREGS + NUM_VFP_SREGS)
	return vfp_register_names[regnum - first];
    }

  first = gdbarch_tdep (current_gdbarch)->first_vfp_pseudo;
  if (first != -1)
    {
      static const char *const vfp_pseudo_names[] = 
	{
	  "d0", "d1", "d2",  "d3",  "d4",  "d5",  "d6",  "d7",
	  "d8", "d9", "d10", "d11", "d12", "d13", "d14", "d15",
	};

      first += gdbarch_num_regs (current_gdbarch);

      if (regnum >= first && regnum < first + NUM_VFP_PSEUDOS)
	return vfp_pseudo_names[regnum - first];
    }

  gdb_assert (regnum < NUM_GREGS + NUM_FPA_REGS + NUM_SREGS);
  return arm_register_names[regnum];
}

static void
set_disassembly_style (void)
{
  const char *setname, *setdesc, *const *regnames;
  int numregs, j;

  /* Find the style that the user wants in the opcodes table.  */
  int current = 0;
  numregs = get_arm_regnames (current, &setname, &setdesc, &regnames);
  while ((disassembly_style != setname)
	 && (current < num_disassembly_options))
    get_arm_regnames (++current, &setname, &setdesc, &regnames);
  current_option = current;

  /* Fill our copy.  */
  for (j = 0; j < numregs; j++)
    arm_register_names[j] = (char *) regnames[j];

  /* Adjust case.  */
  if (isupper (*regnames[ARM_PC_REGNUM]))
    {
      arm_register_names[ARM_FPS_REGNUM] = "FPS";
      arm_register_names[ARM_PS_REGNUM] = "CPSR";
    }
  else
    {
      arm_register_names[ARM_FPS_REGNUM] = "fps";
      arm_register_names[ARM_PS_REGNUM] = "cpsr";
    }

  /* Synchronize the disassembler.  */
  set_arm_regname_option (current);
}

/* Test whether the coff symbol specific value corresponds to a Thumb
   function.  */

static int
coff_sym_is_thumb (int val)
{
  return (val == C_THUMBEXT ||
	  val == C_THUMBSTAT ||
	  val == C_THUMBEXTFUNC ||
	  val == C_THUMBSTATFUNC ||
	  val == C_THUMBLABEL);
}

/* arm_coff_make_msymbol_special()
   arm_elf_make_msymbol_special()
   
   These functions test whether the COFF or ELF symbol corresponds to
   an address in thumb code, and set a "special" bit in a minimal
   symbol to indicate that it does.  */
   
static void
arm_elf_make_msymbol_special(asymbol *sym, struct minimal_symbol *msym)
{
  /* Thumb symbols are of type STT_LOPROC, (synonymous with
     STT_ARM_TFUNC).  */
  if (ELF_ST_TYPE (((elf_symbol_type *)sym)->internal_elf_sym.st_info)
      == STT_LOPROC)
    MSYMBOL_SET_SPECIAL (msym);
}

static void
arm_coff_make_msymbol_special(int val, struct minimal_symbol *msym)
{
  if (coff_sym_is_thumb (val))
    MSYMBOL_SET_SPECIAL (msym);
}

static void
arm_write_pc (CORE_ADDR pc, ptid_t ptid)
{
  write_register_pid (ARM_PC_REGNUM, pc, ptid);

  /* If necessary, set the T bit.  */
  if (arm_apcs_32)
    {
      CORE_ADDR val = read_register_pid (ARM_PS_REGNUM, ptid);
      if (arm_pc_is_thumb (pc))
	write_register_pid (ARM_PS_REGNUM, val | 0x20, ptid);
      else
	write_register_pid (ARM_PS_REGNUM, val & ~(CORE_ADDR) 0x20, ptid);
    }
}

static enum gdb_osabi
arm_elf_osabi_sniffer (bfd *abfd)
{
  unsigned int elfosabi;
  enum gdb_osabi osabi = GDB_OSABI_UNKNOWN;

  elfosabi = elf_elfheader (abfd)->e_ident[EI_OSABI];

  if (elfosabi == ELFOSABI_ARM)
    /* GNU tools use this value.  Check note sections in this case,
       as well.  */
    bfd_map_over_sections (abfd,
			   generic_elf_osabi_sniff_abi_tag_sections, 
			   &osabi);

  /* Anything else will be handled by the generic ELF sniffer.  */
  return osabi;
}

/* */

static char *
arm_sim_available_registers (struct gdbarch *gdbarch,
			     const struct target_ops *target)
{
  /* The built-in simulator supports iWMMXt.  */
  return xstrdup ("iwmmxt");
}

/* Update the current architecture based on architecture features.  */

static void
arm_update_architecture (struct target_ops *target, int from_tty)
{
  struct gdbarch_tdep_info tdep;
  struct gdbarch_info info;
  char buf[64];
  int bytes_read;

  memset (&tdep, 0, sizeof (tdep));

  /* FIXME: Define TARGET_READ_FULL which allocates the buffer large
     enough, modelled on auxv.c.  */
  bytes_read = target_read_partial (target, TARGET_OBJECT_AVAILABLE_REGISTERS,
				    "", buf, 0, 64);

  /* "Parse" the response.  */
  if (bytes_read == 6 && strncmp (buf, "iwmmxt", 6) == 0)
    {
      tdep.target_has_iwmmxt_regs = 1;
      tdep.target_iwmmxt_regnum = -1;
    }
  else if (bytes_read > 6 && strncmp (buf, "iwmmxt:", 7) == 0)
    {
      char *end;
      tdep.target_has_iwmmxt_regs = 1;
      tdep.target_iwmmxt_regnum = strtol (buf + 7, &end, 16);
    }
  else
    {
      tdep.target_has_iwmmxt_regs = 0;
      tdep.target_iwmmxt_regnum = -1;
    }

  if (bytes_read > 3 && strncmp (buf, "vfp:", 4) == 0)
    {
      char *end;
      tdep.target_has_vfp_regs = 1;
      tdep.target_vfp_regnum = strtol (buf + 4, &end, 16);
    }
  else
    {
      tdep.target_has_vfp_regs = 0;
      tdep.target_vfp_regnum = -1;
    }

  /* Update the architecture.  */
  gdbarch_info_init (&info);
  info.tdep_info = &tdep;
  if (!gdbarch_update_p (info))
    internal_error (__FILE__, __LINE__, "could not update architecture");
}


/* Initialize the current architecture based on INFO.  If possible,
   re-use an architecture from ARCHES, which is a list of
   architectures already created during this debugging session.

   Called e.g. at program startup, when reading a core file, and when
   reading a binary file.  */

static struct gdbarch *
arm_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch_tdep *tdep;
  struct gdbarch *gdbarch;
  struct gdbarch_list *best_arch;
  enum arm_abi_kind arm_abi = arm_abi_global;
  enum arm_float_model fp_model = arm_fp_model;
  int nregs;
  int pseudos;

  /* If we have an object to base this architecture on, try to determine
     its ABI.  */

  if (arm_abi == ARM_ABI_AUTO && info.abfd != NULL)
    {
      int ei_osabi;

      switch (bfd_get_flavour (info.abfd))
	{
	case bfd_target_aout_flavour:
	  /* Assume it's an old APCS-style ABI.  */
	  arm_abi = ARM_ABI_APCS;
	  break;

	case bfd_target_coff_flavour:
	  /* Assume it's an old APCS-style ABI.  */
	  /* XXX WinCE?  */
	  arm_abi = ARM_ABI_APCS;
	  break;

	case bfd_target_elf_flavour:
	  ei_osabi = elf_elfheader (info.abfd)->e_ident[EI_OSABI];
	  if (ei_osabi == ELFOSABI_ARM)
	    {
	      /* GNU tools used to use this value, but do not for EABI
		 objects.  There's nowhere to tag an EABI version anyway,
		 so assume APCS.  */
	      arm_abi = ARM_ABI_APCS;
	    }
	  else if (ei_osabi == ELFOSABI_NONE)
	    {
	      int e_flags, eabi_ver;

	      e_flags = elf_elfheader (info.abfd)->e_flags;
	      eabi_ver = EF_ARM_EABI_VERSION (e_flags);

	      switch (eabi_ver)
		{
		case EF_ARM_EABI_UNKNOWN:
		  /* Assume GNU tools.  */
		  arm_abi = ARM_ABI_APCS;
		  break;

		case EF_ARM_EABI_VER4:
		  arm_abi = ARM_ABI_AAPCS;
		  /* EABI binaries default to VFP float ordering.  */
		  if (fp_model == ARM_FLOAT_AUTO)
		    fp_model = ARM_FLOAT_SOFT_VFP;
		  break;

		default:
		  warning (_("unknown ARM EABI version 0x%x"), eabi_ver);
		  arm_abi = ARM_ABI_APCS;
		  break;
		}
	    }
	  break;

	default:
	  /* Leave it as "auto".  */
	  break;
	}
    }

  /* Now that we have inferred any architecture settings that we
     can, try to inherit from the last ARM ABI.  */
  if (arches != NULL)
    {
      if (arm_abi == ARM_ABI_AUTO)
	arm_abi = gdbarch_tdep (arches->gdbarch)->arm_abi;

      if (fp_model == ARM_FLOAT_AUTO)
	fp_model = gdbarch_tdep (arches->gdbarch)->fp_model;
    }
  else
    {
      /* There was no prior ARM architecture; fill in default values.  */

      if (arm_abi == ARM_ABI_AUTO)
	arm_abi = ARM_ABI_APCS;

      /* We used to default to FPA for generic ARM, but almost nobody
	 uses that now, and we now provide a way for the user to force
	 the model.  So default to the most useful variant.  */
      if (fp_model == ARM_FLOAT_AUTO)
	fp_model = ARM_FLOAT_SOFT_FPA;
    }

  /* If there is already a candidate, use it.  */
  for (best_arch = gdbarch_list_lookup_by_info (arches, &info);
       best_arch != NULL;
       best_arch = gdbarch_list_lookup_by_info (best_arch->next, &info))
    {
      /* If we have target-specific bits in INFO, then make sure
	 they match.  */
      if (info.tdep_info)
	{
	  if (gdbarch_tdep (best_arch->gdbarch)->target_has_iwmmxt_regs
	      != info.tdep_info->target_has_iwmmxt_regs)
	    continue;
	  if (gdbarch_tdep (best_arch->gdbarch)->target_iwmmxt_regnum
	      != info.tdep_info->target_iwmmxt_regnum)
	    continue;

	  if (gdbarch_tdep (best_arch->gdbarch)->target_has_vfp_regs
	      != info.tdep_info->target_has_vfp_regs)
	    continue;
	  if (gdbarch_tdep (best_arch->gdbarch)->target_vfp_regnum
	      != info.tdep_info->target_vfp_regnum)
	    continue;
	}

      if (arm_abi != gdbarch_tdep (best_arch->gdbarch)->arm_abi)
	continue;

      if (fp_model != gdbarch_tdep (best_arch->gdbarch)->fp_model)
	continue;

      /* Found a match.  */
      break;
    }

  if (best_arch != NULL)
    return best_arch->gdbarch;

  tdep = xcalloc (1, sizeof (struct gdbarch_tdep));
  gdbarch = gdbarch_alloc (&info, tdep);

  /* Record additional information about the architecture we are defining.
     These are gdbarch discriminators, like the OSABI.  */
  tdep->arm_abi = arm_abi;
  tdep->fp_model = fp_model;

  if (info.tdep_info)
    {
      /* If specific target capabilities were requested, use them.  */
      tdep->target_has_iwmmxt_regs = info.tdep_info->target_has_iwmmxt_regs;
      tdep->target_iwmmxt_regnum = info.tdep_info->target_iwmmxt_regnum;
      tdep->target_has_vfp_regs = info.tdep_info->target_has_vfp_regs;
      tdep->target_vfp_regnum = info.tdep_info->target_vfp_regnum;
    }
  else if (arches)
    {
      /* Otherwise, inherit from the last ARM architecture, if any.  */
      tdep->target_has_iwmmxt_regs
	= gdbarch_tdep (arches->gdbarch)->target_has_iwmmxt_regs;
      tdep->target_iwmmxt_regnum
	= gdbarch_tdep (arches->gdbarch)->target_iwmmxt_regnum;
      tdep->target_has_vfp_regs
	= gdbarch_tdep (arches->gdbarch)->target_has_vfp_regs;
      tdep->target_vfp_regnum
	= gdbarch_tdep (arches->gdbarch)->target_vfp_regnum;
    }
  else
    {
      /* Set defaults.  */
      tdep->target_has_iwmmxt_regs = 0;
      tdep->target_iwmmxt_regnum = -1;
      tdep->target_has_vfp_regs = 0;
      tdep->target_vfp_regnum = -1;
    }

  /* Breakpoints.  */
  switch (info.byte_order)
    {
    case BFD_ENDIAN_BIG:
      tdep->arm_breakpoint = arm_default_arm_be_breakpoint;
      tdep->arm_breakpoint_size = sizeof (arm_default_arm_be_breakpoint);
      tdep->thumb_breakpoint = arm_default_thumb_be_breakpoint;
      tdep->thumb_breakpoint_size = sizeof (arm_default_thumb_be_breakpoint);

      break;

    case BFD_ENDIAN_LITTLE:
      tdep->arm_breakpoint = arm_default_arm_le_breakpoint;
      tdep->arm_breakpoint_size = sizeof (arm_default_arm_le_breakpoint);
      tdep->thumb_breakpoint = arm_default_thumb_le_breakpoint;
      tdep->thumb_breakpoint_size = sizeof (arm_default_thumb_le_breakpoint);

      break;

    default:
      internal_error (__FILE__, __LINE__,
		      _("arm_gdbarch_init: bad byte order for float format"));
    }

  /* On ARM targets char defaults to unsigned.  */
  set_gdbarch_char_signed (gdbarch, 0);

  /* This should be low enough for everything.  */
  tdep->lowest_pc = 0x20;
  tdep->jb_pc = -1;	/* Longjump support not enabled by default.  */

  set_gdbarch_push_dummy_call (gdbarch, arm_push_dummy_call);

  set_gdbarch_write_pc (gdbarch, arm_write_pc);

  /* Frame handling.  */
  set_gdbarch_unwind_dummy_id (gdbarch, arm_unwind_dummy_id);
  set_gdbarch_unwind_pc (gdbarch, arm_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, arm_unwind_sp);

  frame_base_set_default (gdbarch, &arm_normal_base);

  /* Address manipulation.  */
  set_gdbarch_smash_text_address (gdbarch, arm_smash_text_address);
  set_gdbarch_addr_bits_remove (gdbarch, arm_addr_bits_remove);

  /* Advance PC across function entry code.  */
  set_gdbarch_skip_prologue (gdbarch, arm_skip_prologue);

  /* Get the PC when a frame might not be available.  */
  set_gdbarch_deprecated_saved_pc_after_call (gdbarch, arm_saved_pc_after_call);

  /* The stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  /* Breakpoint manipulation.  */
  set_gdbarch_breakpoint_from_pc (gdbarch, arm_breakpoint_from_pc);

  /* Information about registers, etc.  */
  set_gdbarch_print_float_info (gdbarch, arm_print_float_info);
  set_gdbarch_deprecated_fp_regnum (gdbarch, ARM_FP_REGNUM);	/* ??? */
  set_gdbarch_sp_regnum (gdbarch, ARM_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, ARM_PC_REGNUM);
  set_gdbarch_deprecated_register_byte (gdbarch, arm_register_byte);
  set_gdbarch_register_type (gdbarch, arm_register_type);

  nregs = NUM_GREGS + NUM_FPA_REGS + NUM_SREGS;
  pseudos = 0;
  if (tdep->target_has_iwmmxt_regs)
    {
      tdep->first_iwmmxt_regnum = nregs;
      nregs += NUM_IWMMXT_REGS;
    }
  else
    tdep->first_iwmmxt_regnum = -1;

  if (tdep->target_has_vfp_regs)
    {
      tdep->first_vfp_regnum = nregs;
      nregs += NUM_VFP_XREGS + NUM_VFP_SREGS;
      tdep->first_vfp_pseudo = pseudos;
      pseudos += NUM_VFP_PSEUDOS;
    }
  else
    {
      tdep->first_vfp_regnum = -1;
      tdep->first_vfp_pseudo = -1;
    }

  set_gdbarch_num_regs (gdbarch, nregs);
  set_gdbarch_num_pseudo_regs (gdbarch, pseudos);
  set_gdbarch_pseudo_register_read (gdbarch, arm_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, arm_pseudo_register_write);

  /* The FPA registers are included in the remote 'g' packet for
     historic reasons.  */
  set_gdbarch_remote_num_g_packet_regs (gdbarch,
					NUM_GREGS + NUM_FPA_REGS + NUM_SREGS);

  /* Internal <-> external register number maps.  */
  set_gdbarch_dwarf_reg_to_regnum (gdbarch, arm_dwarf_reg_to_regnum);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, arm_dwarf_reg_to_regnum);
  set_gdbarch_register_sim_regno (gdbarch, arm_register_sim_regno);

  set_gdbarch_sim_available_registers (gdbarch, arm_sim_available_registers);
  set_gdbarch_register_remote_regno (gdbarch, arm_register_remote_regno);

  /* Integer registers are 4 bytes.  */
  set_gdbarch_deprecated_register_size (gdbarch, 4);
  set_gdbarch_register_name (gdbarch, arm_register_name);

  /* Returning results.  */
  set_gdbarch_return_value (gdbarch, arm_return_value);

  /* Single stepping.  */
  /* XXX For an RDI target we should ask the target if it can single-step.  */
  set_gdbarch_software_single_step (gdbarch, arm_software_single_step);

  /* Disassembly.  */
  set_gdbarch_print_insn (gdbarch, gdb_print_insn_arm);

  /* Minsymbol frobbing.  */
  set_gdbarch_elf_make_msymbol_special (gdbarch, arm_elf_make_msymbol_special);
  set_gdbarch_coff_make_msymbol_special (gdbarch,
					 arm_coff_make_msymbol_special);

  /* Hook in the ABI-specific overrides, if they have been registered.  */
  gdbarch_init_osabi (info, gdbarch);

  /* Add some default predicates.  */
  frame_unwind_append_sniffer (gdbarch, arm_stub_unwind_sniffer);
  frame_unwind_append_sniffer (gdbarch, arm_sigtramp_unwind_sniffer);
  frame_unwind_append_sniffer (gdbarch, dwarf2_frame_sniffer);
  frame_unwind_append_sniffer (gdbarch, arm_prologue_unwind_sniffer);

  /* Now we have tuned the configuration, set a few final things,
     based on what the OS ABI has told us.  */

  if (tdep->jb_pc >= 0)
    set_gdbarch_get_longjmp_target (gdbarch, arm_get_longjmp_target);

  /* Floating point sizes and format.  */
  switch (info.byte_order)
    {
    case BFD_ENDIAN_BIG:
      set_gdbarch_float_format (gdbarch, &floatformat_ieee_single_big);
      set_gdbarch_double_format (gdbarch, &floatformat_ieee_double_big);
      set_gdbarch_long_double_format (gdbarch, &floatformat_ieee_double_big);
      break;

    case BFD_ENDIAN_LITTLE:
      set_gdbarch_float_format (gdbarch, &floatformat_ieee_single_little);
      if (fp_model == ARM_FLOAT_SOFT_FPA || fp_model == ARM_FLOAT_FPA)
	{
	  set_gdbarch_double_format
	    (gdbarch, &floatformat_ieee_double_littlebyte_bigword);
	  set_gdbarch_long_double_format
	    (gdbarch, &floatformat_ieee_double_littlebyte_bigword);
	}
      else
	{
	  set_gdbarch_double_format (gdbarch, &floatformat_ieee_double_little);
	  set_gdbarch_long_double_format (gdbarch,
					  &floatformat_ieee_double_little);
	}
      break;

    default:
      internal_error (__FILE__, __LINE__,
		      _("arm_gdbarch_init: bad byte order for float format"));
    }

  return gdbarch;
}

static void
arm_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  if (tdep == NULL)
    return;

  fprintf_unfiltered (file, _("arm_dump_tdep: Lowest pc = 0x%lx"),
		      (unsigned long) tdep->lowest_pc);
}

extern initialize_file_ftype _initialize_arm_tdep; /* -Wmissing-prototypes */

void
_initialize_arm_tdep (void)
{
  struct ui_file *stb;
  long length;
  struct cmd_list_element *new_set, *new_show;
  const char *setname;
  const char *setdesc;
  const char *const *regnames;
  int numregs, i, j;
  static char *helptext;
  char regdesc[1024], *rdptr = regdesc;
  size_t rest = sizeof (regdesc);

  gdbarch_register (bfd_arch_arm, arm_gdbarch_init, arm_dump_tdep);

  /* Register an ELF OS ABI sniffer for ARM binaries.  */
  gdbarch_register_osabi_sniffer (bfd_arch_arm,
				  bfd_target_elf_flavour,
				  arm_elf_osabi_sniffer);

  /* Get the number of possible sets of register names defined in opcodes.  */
  num_disassembly_options = get_arm_regname_num_options ();

  /* Add root prefix command for all "set arm"/"show arm" commands.  */
  add_prefix_cmd ("arm", no_class, set_arm_command,
		  _("Various ARM-specific commands."),
		  &setarmcmdlist, "set arm ", 0, &setlist);

  add_prefix_cmd ("arm", no_class, show_arm_command,
		  _("Various ARM-specific commands."),
		  &showarmcmdlist, "show arm ", 0, &showlist);

  /* Sync the opcode insn printer with our register viewer.  */
  parse_arm_disassembler_option ("reg-names-std");

  /* Initialize the array that will be passed to
     add_setshow_enum_cmd().  */
  valid_disassembly_styles
    = xmalloc ((num_disassembly_options + 1) * sizeof (char *));
  for (i = 0; i < num_disassembly_options; i++)
    {
      numregs = get_arm_regnames (i, &setname, &setdesc, &regnames);
      valid_disassembly_styles[i] = setname;
      length = snprintf (rdptr, rest, "%s - %s\n", setname, setdesc);
      rdptr += length;
      rest -= length;
      /* Copy the default names (if found) and synchronize disassembler.  */
      if (!strcmp (setname, "std"))
	{
          disassembly_style = setname;
          current_option = i;
	  for (j = 0; j < numregs; j++)
            arm_register_names[j] = (char *) regnames[j];
          set_arm_regname_option (i);
	}
    }
  /* Mark the end of valid options.  */
  valid_disassembly_styles[num_disassembly_options] = NULL;

  /* Create the help text.  */
  stb = mem_fileopen ();
  fprintf_unfiltered (stb, "%s%s%s",
		      _("The valid values are:\n"),
		      regdesc,
		      _("The default is \"std\"."));
  helptext = ui_file_xstrdup (stb, &length);
  ui_file_delete (stb);

  add_setshow_enum_cmd("disassembler", no_class,
		       valid_disassembly_styles, &disassembly_style,
		       _("Set the disassembly style."),
		       _("Show the disassembly style."),
		       helptext,
		       set_disassembly_style_sfunc,
		       NULL, /* FIXME: i18n: The disassembly style is \"%s\".  */
		       &setarmcmdlist, &showarmcmdlist);

  add_setshow_boolean_cmd ("apcs32", no_class, &arm_apcs_32,
			   _("Set usage of ARM 32-bit mode."),
			   _("Show usage of ARM 32-bit mode."),
			   _("When off, a 26-bit PC will be used."),
			   NULL,
			   NULL, /* FIXME: i18n: Usage of ARM 32-bit mode is %s.  */
			   &setarmcmdlist, &showarmcmdlist);

  /* Add a command to allow the user to force the FPU model.  */
  add_setshow_enum_cmd ("fpu", no_class, fp_model_strings, &current_fp_model,
			_("Set the floating point type."),
			_("Show the floating point type."),
			_("auto - Determine the FP typefrom the OS-ABI.\n\
softfpa - Software FP, mixed-endian doubles on little-endian ARMs.\n\
fpa - FPA co-processor (GCC compiled).\n\
softvfp - Software FP with pure-endian doubles.\n\
vfp - VFP co-processor."),
			set_fp_model_sfunc, show_fp_model,
			&setarmcmdlist, &showarmcmdlist);

  /* Add a command to allow the user to force the ABI.  */
  add_setshow_enum_cmd ("abi", class_support, arm_abi_strings, &arm_abi_string,
			_("Set the ABI."),
			_("Show the ABI."),
			NULL, arm_set_abi, arm_show_abi,
			&setarmcmdlist, &showarmcmdlist);

  /* Debugging flag.  */
  add_setshow_boolean_cmd ("arm", class_maintenance, &arm_debug,
			   _("Set ARM debugging."),
			   _("Show ARM debugging."),
			   _("When on, arm-specific debugging is enabled."),
			   NULL,
			   NULL, /* FIXME: i18n: "ARM debugging is %s.  */
			   &setdebuglist, &showdebuglist);

  observer_attach_inferior_created (arm_update_architecture);
}
