/* Target-dependent code for Hitachi Super-H, for GDB.
   Copyright 1993, 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003
   Free Software Foundation, Inc.

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

/*
   Contributed by Steve Chamberlain
   sac@cygnus.com
 */

#include "defs.h"
#include "frame.h"
#include "symtab.h"
#include "symfile.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "value.h"
#include "dis-asm.h"
#include "inferior.h"
#include "gdb_string.h"
#include "arch-utils.h"
#include "floatformat.h"
#include "regcache.h"
#include "doublest.h"
#include "osabi.h"

#include "sh-tdep.h"

#include "elf-bfd.h"
#include "solib-svr4.h"

/* sh flags */
#include "elf/sh.h"
/* registers numbers shared with the simulator */
#include "gdb/sim-sh.h"

static void (*sh_show_regs) (void);

#define SH_NUM_REGS 59

/* Define other aspects of the stack frame.
   we keep a copy of the worked out return pc lying around, since it
   is a useful bit of info */
  
struct frame_extra_info
{
  CORE_ADDR return_pc;
  int leaf_function;
  int f_offset;
};

static const char *
sh_generic_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    "fpul", "fpscr",
    "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
    "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
    "ssr",  "spc",
    "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
    "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1",
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const char *
sh_sh_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    "",     "",
    "",     "",     "",     "",     "",     "",     "",     "",
    "",     "",     "",     "",     "",     "",     "",     "",
    "",     "",
    "",     "",     "",     "",     "",     "",     "",     "",
    "",     "",     "",     "",     "",     "",     "",     "",
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const char *
sh_sh3_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    "",     "",
    "",     "",     "",     "",     "",     "",     "",     "",
    "",     "",     "",     "",     "",     "",     "",     "",
    "ssr",  "spc",
    "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
    "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1"
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const char *
sh_sh3e_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    "fpul", "fpscr",
    "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
    "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
    "ssr",  "spc",
    "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
    "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1",
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const char *
sh_sh2e_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    "fpul", "fpscr",
    "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
    "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
    "",  "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "",
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const char *
sh_sh_dsp_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    "",     "dsr",
    "a0g",  "a0",   "a1g",  "a1",   "m0",   "m1",   "x0",   "x1",
    "y0",   "y1",   "",     "",     "",     "",     "",     "mod",
    "",     "",
    "rs",   "re",   "",     "",     "",     "",     "",     "",
    "",     "",     "",     "",     "",     "",     "",     "",
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const char *
sh_sh3_dsp_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    "",     "dsr",
    "a0g",  "a0",   "a1g",  "a1",   "m0",   "m1",   "x0",   "x1",
    "y0",   "y1",   "",     "",     "",     "",     "",     "mod",
    "ssr",  "spc",
    "rs",   "re",   "",     "",     "",     "",     "",     "",
    "r0b",  "r1b",  "r2b",  "r3b",  "r4b",  "r5b",  "r6b",  "r7b"
    "",     "",     "",     "",     "",     "",     "",     "",
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const char *
sh_sh4_register_name (int reg_nr)
{
  static char *register_names[] =
  {
    /* general registers 0-15 */
    "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
    "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
    /* 16 - 22 */
    "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
    /* 23, 24 */
    "fpul", "fpscr",
    /* floating point registers 25 - 40 */
    "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
    "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
    /* 41, 42 */
    "ssr",  "spc",
    /* bank 0 43 - 50 */
    "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
    /* bank 1 51 - 58 */
    "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1",
    /* double precision (pseudo) 59 - 66 */
    "dr0",  "dr2",  "dr4",  "dr6",  "dr8",  "dr10", "dr12", "dr14",
    /* vectors (pseudo) 67 - 70 */
    "fv0",  "fv4",  "fv8",  "fv12",
    /* FIXME: missing XF 71 - 86 */
    /* FIXME: missing XD 87 - 94 */
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static const unsigned char *
sh_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  /* 0xc3c3 is trapa #c3, and it works in big and little endian modes */
  static unsigned char breakpoint[] =  {0xc3, 0xc3};
  
  *lenptr = sizeof (breakpoint);
  return breakpoint;
}

static CORE_ADDR
sh_push_dummy_code (struct gdbarch *gdbarch,
		    CORE_ADDR sp, CORE_ADDR funaddr, int using_gcc,
		    struct value **args, int nargs,
		    struct type *value_type,
		    CORE_ADDR *real_pc, CORE_ADDR *bp_addr)
{
  /* Allocate space sufficient for a breakpoint.  */
  sp = (sp - 2) & ~1;
  /* Store the address of that breakpoint */
  *bp_addr = sp;
  /* sh always starts the call at the callee's entry point.  */
  *real_pc = funaddr;
  return sp;
}

/* Prologue looks like
   [mov.l       <regs>,@-r15]...
   [sts.l       pr,@-r15]
   [mov.l       r14,@-r15]
   [mov         r15,r14]

   Actually it can be more complicated than this.  For instance, with
   newer gcc's:

   mov.l   r14,@-r15
   add     #-12,r15
   mov     r15,r14
   mov     r4,r1
   mov     r5,r2
   mov.l   r6,@(4,r14)
   mov.l   r7,@(8,r14)
   mov.b   r1,@r14
   mov     r14,r1
   mov     r14,r1
   add     #2,r1
   mov.w   r2,@r1

 */

/* STS.L PR,@-r15  0100111100100010
   r15-4-->r15, PR-->(r15) */
#define IS_STS(x)  		((x) == 0x4f22)

/* MOV.L Rm,@-r15  00101111mmmm0110
   r15-4-->r15, Rm-->(R15) */
#define IS_PUSH(x) 		(((x) & 0xff0f) == 0x2f06)

#define GET_PUSHED_REG(x)  	(((x) >> 4) & 0xf)

/* MOV r15,r14     0110111011110011
   r15-->r14  */
#define IS_MOV_SP_FP(x)  	((x) == 0x6ef3)

/* ADD #imm,r15    01111111iiiiiiii
   r15+imm-->r15 */
#define IS_ADD_IMM_SP(x)	(((x) & 0xff00) == 0x7f00)

#define IS_MOV_R3(x) 		(((x) & 0xff00) == 0x1a00)
#define IS_SHLL_R3(x)		((x) == 0x4300)

/* ADD r3,r15      0011111100111100
   r15+r3-->r15 */
#define IS_ADD_R3SP(x)		((x) == 0x3f3c)

/* FMOV.S FRm,@-Rn  Rn-4-->Rn, FRm-->(Rn)     1111nnnnmmmm1011
   FMOV DRm,@-Rn    Rn-8-->Rn, DRm-->(Rn)     1111nnnnmmm01011
   FMOV XDm,@-Rn    Rn-8-->Rn, XDm-->(Rn)     1111nnnnmmm11011 */
/* CV, 2003-08-28: Only suitable with Rn == SP, therefore name changed to
		   make this entirely clear. */
#define IS_FPUSH(x)		(((x) & 0xf00f) == 0xf00b)

/* MOV Rm,Rn            Rm-->Rn          0110nnnnmmmm0011 
   MOV.L Rm,@(disp,Rn)  Rm-->(dispx4+Rn) 0001nnnnmmmmdddd
   MOV.L Rm,@Rn         Rm-->(Rn)        0010nnnnmmmm0010
   where Rm is one of r4,r5,r6,r7 which are the argument registers. */
#define IS_ARG_MOV(x) \
(((((x) & 0xf00f) == 0x6003) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070)) \
 || ((((x) & 0xf000) == 0x1000) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070)) \
 || ((((x) & 0xf00f) == 0x2002) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070)))

/* MOV.L Rm,@(disp,r14)  00011110mmmmdddd
   Rm-->(dispx4+r14) where Rm is one of r4,r5,r6,r7 */
#define IS_MOV_TO_R14(x) \
     ((((x) & 0xff00) == 0x1e) && (((x) & 0x00f0) >= 0x0040 && ((x) & 0x00f0) <= 0x0070))
                        
#define FPSCR_SZ		(1 << 20)

/* Skip any prologue before the guts of a function */

/* Skip the prologue using the debug information. If this fails we'll
   fall back on the 'guess' method below. */
static CORE_ADDR
after_prologue (CORE_ADDR pc)
{
  struct symtab_and_line sal;
  CORE_ADDR func_addr, func_end;

  /* If we can not find the symbol in the partial symbol table, then
     there is no hope we can determine the function's start address
     with this code.  */
  if (!find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    return 0;

  /* Get the line associated with FUNC_ADDR.  */
  sal = find_pc_line (func_addr, 0);

  /* There are only two cases to consider.  First, the end of the source line
     is within the function bounds.  In that case we return the end of the
     source line.  Second is the end of the source line extends beyond the
     bounds of the current function.  We need to use the slow code to
     examine instructions in that case.  */
  if (sal.end < func_end)
    return sal.end;
  else
    return 0;
}

/* Here we look at each instruction in the function, and try to guess
   where the prologue ends. Unfortunately this is not always 
   accurate. */
static CORE_ADDR
sh_skip_prologue_hard_way (CORE_ADDR start_pc)
{
  CORE_ADDR here, end;
  int updated_fp = 0;

  if (!start_pc)
    return 0;

  for (here = start_pc, end = start_pc + (2 * 28); here < end;)
    {
      int w = read_memory_integer (here, 2);
      here += 2;
      if (IS_FPUSH (w) || IS_PUSH (w) || IS_STS (w) || IS_MOV_R3 (w)
	  || IS_ADD_R3SP (w) || IS_ADD_IMM_SP (w) || IS_SHLL_R3 (w) 
	  || IS_ARG_MOV (w) || IS_MOV_TO_R14 (w))
	{
	  start_pc = here;
	}
      else if (IS_MOV_SP_FP (w))
	{
	  start_pc = here;
	  updated_fp = 1;
	}
      else
	/* Don't bail out yet, if we are before the copy of sp. */
	if (updated_fp)
	  break;
    }

  return start_pc;
}

static CORE_ADDR
sh_skip_prologue (CORE_ADDR pc)
{
  CORE_ADDR post_prologue_pc;

  /* See if we can determine the end of the prologue via the symbol table.
     If so, then return either PC, or the PC after the prologue, whichever
     is greater.  */
  post_prologue_pc = after_prologue (pc);

  /* If after_prologue returned a useful address, then use it.  Else
     fall back on the instruction skipping code. */
  if (post_prologue_pc != 0)
    return max (pc, post_prologue_pc);
  else
    return sh_skip_prologue_hard_way (pc);
}

/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.

   The return address is the value saved in the PR register + 4  */
static CORE_ADDR
sh_saved_pc_after_call (struct frame_info *frame)
{
  return (ADDR_BITS_REMOVE (read_register (PR_REGNUM)));
}

/* Should call_function allocate stack space for a struct return?  */
static int
sh_use_struct_convention (int gcc_p, struct type *type)
{
#if 0
  return (TYPE_LENGTH (type) > 1);
#else
  int len = TYPE_LENGTH (type);
  int nelem = TYPE_NFIELDS (type);
  return ((len != 1 && len != 2 && len != 4 && len != 8) || nelem != 1) &&
	  (len != 8 || TYPE_LENGTH (TYPE_FIELD_TYPE (type, 0)) != 4);
#endif
}

/* Disassemble an instruction.  */
static int
gdb_print_insn_sh (bfd_vma memaddr, disassemble_info *info)
{
  info->endian = TARGET_BYTE_ORDER;
  return print_insn_sh (memaddr, info);
}

/* Given a GDB frame, determine the address of the calling function's
   frame.  This will be used to create a new GDB frame struct, and
   then DEPRECATED_INIT_EXTRA_FRAME_INFO and DEPRECATED_INIT_FRAME_PC
   will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */
static CORE_ADDR
sh_frame_chain (struct frame_info *frame)
{
  if (DEPRECATED_PC_IN_CALL_DUMMY (get_frame_pc (frame),
				   get_frame_base (frame),
				   get_frame_base (frame)))
    return get_frame_base (frame);	/* dummy frame same as caller's frame */
  if (get_frame_pc (frame)
      && !deprecated_inside_entry_file (get_frame_pc (frame)))
    return read_memory_integer (get_frame_base (frame)
				+ get_frame_extra_info (frame)->f_offset, 4);
  else
    return 0;
}

/* Find REGNUM on the stack.  Otherwise, it's in an active register.  One thing
   we might want to do here is to check REGNUM against the clobber mask, and
   somehow flag it as invalid if it isn't saved on the stack somewhere.  This
   would provide a graceful failure mode when trying to get the value of
   caller-saves registers for an inner frame.  */
static CORE_ADDR
sh_find_callers_reg (struct frame_info *fi, int regnum)
{
  for (; fi; fi = get_next_frame (fi))
    if (DEPRECATED_PC_IN_CALL_DUMMY (get_frame_pc (fi), get_frame_base (fi),
				     get_frame_base (fi)))
      /* When the caller requests PR from the dummy frame, we return PC because
         that's where the previous routine appears to have done a call from. */
      return deprecated_read_register_dummy (get_frame_pc (fi),
					     get_frame_base (fi), regnum);
    else
      {
	DEPRECATED_FRAME_INIT_SAVED_REGS (fi);
	if (!get_frame_pc (fi))
	  return 0;
	if (get_frame_saved_regs (fi)[regnum] != 0)
	  return read_memory_integer (get_frame_saved_regs (fi)[regnum],
				      register_size (current_gdbarch, regnum));
      }
  return read_register (regnum);
}

/* Put here the code to store, into a struct frame_saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special: the address we
   return for it IS the sp for the next frame. */
static void
sh_nofp_frame_init_saved_regs (struct frame_info *fi)
{
  int *where = (int *) alloca ((NUM_REGS + NUM_PSEUDO_REGS) * sizeof(int));
  int rn;
  int have_fp = 0;
  int depth;
  int pc;
  int opc;
  int inst;
  int r3_val = 0;
  char *dummy_regs = deprecated_generic_find_dummy_frame (get_frame_pc (fi),
							  get_frame_base (fi));
  
  if (get_frame_saved_regs (fi) == NULL)
    frame_saved_regs_zalloc (fi);
  else
    memset (get_frame_saved_regs (fi), 0, SIZEOF_FRAME_SAVED_REGS);
  
  if (dummy_regs)
    {
      /* DANGER!  This is ONLY going to work if the char buffer format of
         the saved registers is byte-for-byte identical to the 
         CORE_ADDR regs[NUM_REGS] format used by struct frame_saved_regs! */
      memcpy (get_frame_saved_regs (fi), dummy_regs, SIZEOF_FRAME_SAVED_REGS);
      return;
    }

  get_frame_extra_info (fi)->leaf_function = 1;
  get_frame_extra_info (fi)->f_offset = 0;

  for (rn = 0; rn < NUM_REGS + NUM_PSEUDO_REGS; rn++)
    where[rn] = -1;

  depth = 0;

  /* Loop around examining the prologue inst until we find something
     that does not appear to be part of the prologue.  But give up
     after 20 of them, since we're getting silly then. */

  pc = get_frame_func (fi);
  if (!pc)
    {
      deprecated_update_frame_pc_hack (fi, 0);
      return;
    }

  for (opc = pc + (2 * 28); pc < opc; pc += 2)
    {
      inst = read_memory_integer (pc, 2);
      /* See where the registers will be saved to */
      if (IS_PUSH (inst))
	{
	  rn = GET_PUSHED_REG (inst);
	  where[rn] = depth;
	  depth += 4;
	}
      else if (IS_STS (inst))
	{
	  where[PR_REGNUM] = depth;
	  /* If we're storing the pr then this isn't a leaf */
	  get_frame_extra_info (fi)->leaf_function = 0;
	  depth += 4;
	}
      else if (IS_MOV_R3 (inst))
	{
	  r3_val = ((inst & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_SHLL_R3 (inst))
	{
	  r3_val <<= 1;
	}
      else if (IS_ADD_R3SP (inst))
	{
	  depth += -r3_val;
	}
      else if (IS_ADD_IMM_SP (inst))
	{
	  depth -= ((inst & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_MOV_SP_FP (inst))
	break;
#if 0 /* This used to just stop when it found an instruction that
	 was not considered part of the prologue.  Now, we just
	 keep going looking for likely instructions. */
      else
	break;
#endif
    }

  /* Now we know how deep things are, we can work out their addresses */

  for (rn = 0; rn < NUM_REGS + NUM_PSEUDO_REGS; rn++)
    {
      if (where[rn] >= 0)
	{
	  if (rn == DEPRECATED_FP_REGNUM)
	    have_fp = 1;

	  get_frame_saved_regs (fi)[rn] = get_frame_base (fi) - where[rn] + depth - 4;
	}
      else
	{
	  get_frame_saved_regs (fi)[rn] = 0;
	}
    }

  if (have_fp)
    {
      get_frame_saved_regs (fi)[SP_REGNUM] = read_memory_integer (get_frame_saved_regs (fi)[DEPRECATED_FP_REGNUM], 4);
    }
  else
    {
      get_frame_saved_regs (fi)[SP_REGNUM] = get_frame_base (fi) - 4;
    }

  get_frame_extra_info (fi)->f_offset = depth - where[DEPRECATED_FP_REGNUM] - 4;
  /* Work out the return pc - either from the saved pr or the pr
     value */
}

/* For vectors of 4 floating point registers. */
static int
fv_reg_base_num (int fv_regnum)
{
  int fp_regnum;

  fp_regnum = FP0_REGNUM + 
    (fv_regnum - FV0_REGNUM) * 4;
  return fp_regnum;
}

/* For double precision floating point registers, i.e 2 fp regs.*/
static int
dr_reg_base_num (int dr_regnum)
{
  int fp_regnum;

  fp_regnum = FP0_REGNUM + 
    (dr_regnum - DR0_REGNUM) * 2;
  return fp_regnum;
}

static void
sh_fp_frame_init_saved_regs (struct frame_info *fi)
{
  int *where = (int *) alloca ((NUM_REGS + NUM_PSEUDO_REGS) * sizeof (int));
  int rn;
  int have_fp = 0;
  int depth;
  int pc;
  int opc;
  int inst;
  int r3_val = 0;
  char *dummy_regs = deprecated_generic_find_dummy_frame (get_frame_pc (fi), get_frame_base (fi));
  
  if (get_frame_saved_regs (fi) == NULL)
    frame_saved_regs_zalloc (fi);
  else
    memset (get_frame_saved_regs (fi), 0, SIZEOF_FRAME_SAVED_REGS);
  
  if (dummy_regs)
    {
      /* DANGER!  This is ONLY going to work if the char buffer format of
         the saved registers is byte-for-byte identical to the 
         CORE_ADDR regs[NUM_REGS] format used by struct frame_saved_regs! */
      memcpy (get_frame_saved_regs (fi), dummy_regs, SIZEOF_FRAME_SAVED_REGS);
      return;
    }

  get_frame_extra_info (fi)->leaf_function = 1;
  get_frame_extra_info (fi)->f_offset = 0;

  for (rn = 0; rn < NUM_REGS + NUM_PSEUDO_REGS; rn++)
    where[rn] = -1;

  depth = 0;

  /* Loop around examining the prologue inst until we find something
     that does not appear to be part of the prologue.  But give up
     after 20 of them, since we're getting silly then. */

  pc = get_frame_func (fi);
  if (!pc)
    {
      deprecated_update_frame_pc_hack (fi, 0);
      return;
    }

  for (opc = pc + (2 * 28); pc < opc; pc += 2)
    {
      inst = read_memory_integer (pc, 2);
      /* See where the registers will be saved to */
      if (IS_PUSH (inst))
	{
	  rn = GET_PUSHED_REG (inst);
	  where[rn] = depth;
	  depth += 4;
	}
      else if (IS_STS (inst))
	{
	  where[PR_REGNUM] = depth;
	  /* If we're storing the pr then this isn't a leaf */
	  get_frame_extra_info (fi)->leaf_function = 0;
	  depth += 4;
	}
      else if (IS_MOV_R3 (inst))
	{
	  r3_val = ((inst & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_SHLL_R3 (inst))
	{
	  r3_val <<= 1;
	}
      else if (IS_ADD_R3SP (inst))
	{
	  depth += -r3_val;
	}
      else if (IS_ADD_IMM_SP (inst))
	{
	  depth -= ((inst & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_FPUSH (inst))
	{
	  if (read_register (FPSCR_REGNUM) & FPSCR_SZ)
	    {
	      depth += 8;
	    }
	  else
	    {
	      depth += 4;
	    }
	}
      else if (IS_MOV_SP_FP (inst))
	break;
#if 0 /* This used to just stop when it found an instruction that
	 was not considered part of the prologue.  Now, we just
	 keep going looking for likely instructions. */
      else
	break;
#endif
    }

  /* Now we know how deep things are, we can work out their addresses */

  for (rn = 0; rn < NUM_REGS + NUM_PSEUDO_REGS; rn++)
    {
      if (where[rn] >= 0)
	{
	  if (rn == DEPRECATED_FP_REGNUM)
	    have_fp = 1;

	  get_frame_saved_regs (fi)[rn] = get_frame_base (fi) - where[rn] + depth - 4;
	}
      else
	{
	  get_frame_saved_regs (fi)[rn] = 0;
	}
    }

  if (have_fp)
    {
      get_frame_saved_regs (fi)[SP_REGNUM] =
	read_memory_integer (get_frame_saved_regs (fi)[DEPRECATED_FP_REGNUM], 4);
    }
  else
    {
      get_frame_saved_regs (fi)[SP_REGNUM] = get_frame_base (fi) - 4;
    }

  get_frame_extra_info (fi)->f_offset = depth - where[DEPRECATED_FP_REGNUM] - 4;
  /* Work out the return pc - either from the saved pr or the pr
     value */
}

/* Initialize the extra info saved in a FRAME */
static void
sh_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{

  frame_extra_info_zalloc (fi, sizeof (struct frame_extra_info));

  if (get_next_frame (fi))
    deprecated_update_frame_pc_hack (fi, DEPRECATED_FRAME_SAVED_PC (get_next_frame (fi)));

  if (DEPRECATED_PC_IN_CALL_DUMMY (get_frame_pc (fi), get_frame_base (fi),
				   get_frame_base (fi)))
    {
      /* We need to setup fi->frame here because call_function_by_hand
         gets it wrong by assuming it's always FP.  */
      deprecated_update_frame_base_hack (fi, deprecated_read_register_dummy (get_frame_pc (fi), get_frame_base (fi),
									     SP_REGNUM));
      get_frame_extra_info (fi)->return_pc = deprecated_read_register_dummy (get_frame_pc (fi),
								  get_frame_base (fi),
								  PC_REGNUM);
      get_frame_extra_info (fi)->f_offset = -(DEPRECATED_CALL_DUMMY_LENGTH + 4);
      get_frame_extra_info (fi)->leaf_function = 0;
      return;
    }
  else
    {
      DEPRECATED_FRAME_INIT_SAVED_REGS (fi);
      get_frame_extra_info (fi)->return_pc = 
	sh_find_callers_reg (fi, PR_REGNUM);
    }
}

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */
static CORE_ADDR
sh_extract_struct_value_address (struct regcache *regcache)
{
  ULONGEST addr;
  regcache_cooked_read_unsigned (regcache, STRUCT_RETURN_REGNUM, &addr);
  return addr;
}

static CORE_ADDR
sh_frame_saved_pc (struct frame_info *frame)
{
  return (get_frame_extra_info (frame)->return_pc);
}

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */
static void
sh_pop_frame (void)
{
  struct frame_info *frame = get_current_frame ();
  CORE_ADDR fp;
  int regnum;

  if (DEPRECATED_PC_IN_CALL_DUMMY (get_frame_pc (frame),
				   get_frame_base (frame),
				   get_frame_base (frame)))
    generic_pop_dummy_frame ();
  else
    {
      fp = get_frame_base (frame);
      DEPRECATED_FRAME_INIT_SAVED_REGS (frame);

      /* Copy regs from where they were saved in the frame */
      for (regnum = 0; regnum < NUM_REGS + NUM_PSEUDO_REGS; regnum++)
	if (get_frame_saved_regs (frame)[regnum])
	  write_register (regnum,
			  read_memory_integer (get_frame_saved_regs (frame)[regnum], 4));

      write_register (PC_REGNUM, get_frame_extra_info (frame)->return_pc);
      write_register (SP_REGNUM, fp + 4);
    }
  flush_cached_frames ();
}

static CORE_ADDR
sh_frame_align (struct gdbarch *ignore, CORE_ADDR sp)
{
  return sp & ~3;
}

/* Function: push_dummy_call (formerly push_arguments)
   Setup the function arguments for calling a function in the inferior.

   On the Hitachi SH architecture, there are four registers (R4 to R7)
   which are dedicated for passing function arguments.  Up to the first
   four arguments (depending on size) may go into these registers.
   The rest go on the stack.

   MVS: Except on SH variants that have floating point registers.
   In that case, float and double arguments are passed in the same
   manner, but using FP registers instead of GP registers.

   Arguments that are smaller than 4 bytes will still take up a whole
   register or a whole 32-bit word on the stack, and will be 
   right-justified in the register or the stack word.  This includes
   chars, shorts, and small aggregate types.

   Arguments that are larger than 4 bytes may be split between two or 
   more registers.  If there are not enough registers free, an argument
   may be passed partly in a register (or registers), and partly on the
   stack.  This includes doubles, long longs, and larger aggregates. 
   As far as I know, there is no upper limit to the size of aggregates 
   that will be passed in this way; in other words, the convention of 
   passing a pointer to a large aggregate instead of a copy is not used.

   MVS: The above appears to be true for the SH variants that do not
   have an FPU, however those that have an FPU appear to copy the
   aggregate argument onto the stack (and not place it in registers)
   if it is larger than 16 bytes (four GP registers).

   An exceptional case exists for struct arguments (and possibly other
   aggregates such as arrays) if the size is larger than 4 bytes but 
   not a multiple of 4 bytes.  In this case the argument is never split 
   between the registers and the stack, but instead is copied in its
   entirety onto the stack, AND also copied into as many registers as 
   there is room for.  In other words, space in registers permitting, 
   two copies of the same argument are passed in.  As far as I can tell,
   only the one on the stack is used, although that may be a function 
   of the level of compiler optimization.  I suspect this is a compiler
   bug.  Arguments of these odd sizes are left-justified within the 
   word (as opposed to arguments smaller than 4 bytes, which are 
   right-justified).

   If the function is to return an aggregate type such as a struct, it 
   is either returned in the normal return value register R0 (if its 
   size is no greater than one byte), or else the caller must allocate
   space into which the callee will copy the return value (if the size
   is greater than one byte).  In this case, a pointer to the return 
   value location is passed into the callee in register R2, which does 
   not displace any of the other arguments passed in via registers R4
   to R7.   */

static CORE_ADDR
sh_push_dummy_call_fpu (struct gdbarch *gdbarch, 
			CORE_ADDR func_addr,
			struct regcache *regcache, 
			CORE_ADDR bp_addr, int nargs,
			struct value **args, 
			CORE_ADDR sp, int struct_return,
			CORE_ADDR struct_addr)
{
  int stack_offset, stack_alloc;
  int argreg, flt_argreg;
  int argnum;
  struct type *type;
  CORE_ADDR regval;
  char *val;
  char valbuf[4];
  int len;
  int odd_sized_struct;

  /* first force sp to a 4-byte alignment */
  sp = sh_frame_align (gdbarch, sp);

  /* The "struct return pointer" pseudo-argument has its own dedicated 
     register */
  if (struct_return)
    regcache_cooked_write_unsigned (regcache, 
				    STRUCT_RETURN_REGNUM, 
				    struct_addr);

  /* Now make sure there's space on the stack */
  for (argnum = 0, stack_alloc = 0; argnum < nargs; argnum++)
    stack_alloc += ((TYPE_LENGTH (VALUE_TYPE (args[argnum])) + 3) & ~3);
  sp -= stack_alloc;		/* make room on stack for args */

  /* Now load as many as possible of the first arguments into
     registers, and push the rest onto the stack.  There are 16 bytes
     in four registers available.  Loop thru args from first to last.  */

  argreg = ARG0_REGNUM;
  flt_argreg = FLOAT_ARG0_REGNUM;
  for (argnum = 0, stack_offset = 0; argnum < nargs; argnum++)
    {
      type = VALUE_TYPE (args[argnum]);
      len = TYPE_LENGTH (type);
      memset (valbuf, 0, sizeof (valbuf));
      if (len < 4)
	{
	  /* value gets right-justified in the register or stack word */
	  if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
	    memcpy (valbuf + (4 - len),
		    (char *) VALUE_CONTENTS (args[argnum]), len);
	  else
	    memcpy (valbuf, (char *) VALUE_CONTENTS (args[argnum]), len);
	  val = valbuf;
	}
      else
	val = (char *) VALUE_CONTENTS (args[argnum]);

      if (len > 4 && (len & 3) != 0)
	odd_sized_struct = 1;	/* Such structs go entirely on stack.  */
      else if (len > 16)
	odd_sized_struct = 1;	/* So do aggregates bigger than 4 words.  */
      else
	odd_sized_struct = 0;
      while (len > 0)
	{
	  if ((TYPE_CODE (type) == TYPE_CODE_FLT 
	       && flt_argreg > FLOAT_ARGLAST_REGNUM) 
	      || argreg > ARGLAST_REGNUM
	      || odd_sized_struct)
	    {			
	      /* must go on the stack */
	      write_memory (sp + stack_offset, val, 4);
	      stack_offset += 4;
	    }
	  /* NOTE WELL!!!!!  This is not an "else if" clause!!!
	     That's because some *&^%$ things get passed on the stack
	     AND in the registers!   */
	  if (TYPE_CODE (type) == TYPE_CODE_FLT &&
	      flt_argreg > 0 && flt_argreg <= FLOAT_ARGLAST_REGNUM)
	    {
	      /* Argument goes in a single-precision fp reg.  */
	      regval = extract_unsigned_integer (val, register_size (gdbarch,
								     argreg));
	      regcache_cooked_write_unsigned (regcache, flt_argreg++, regval);
	    }
	  else if (argreg <= ARGLAST_REGNUM)
	    {			
	      /* there's room in a register */
	      regval = extract_unsigned_integer (val, register_size (gdbarch,
								     argreg));
	      regcache_cooked_write_unsigned (regcache, argreg++, regval);
	    }
	  /* Store the value 4 bytes at a time.  This means that things
	     larger than 4 bytes may go partly in registers and partly
	     on the stack.  */
	  len -= register_size (gdbarch, argreg);
	  val += register_size (gdbarch, argreg);
	}
    }

  /* Store return address. */
  regcache_cooked_write_unsigned (regcache, PR_REGNUM, bp_addr);

  /* Update stack pointer.  */
  regcache_cooked_write_unsigned (regcache, SP_REGNUM, sp);

  return sp;
}

static CORE_ADDR
sh_push_dummy_call_nofpu (struct gdbarch *gdbarch, 
			  CORE_ADDR func_addr,
			  struct regcache *regcache, 
			  CORE_ADDR bp_addr, 
			  int nargs, struct value **args, 
			  CORE_ADDR sp, int struct_return, 
			  CORE_ADDR struct_addr)
{
  int stack_offset, stack_alloc;
  int argreg;
  int argnum;
  struct type *type;
  CORE_ADDR regval;
  char *val;
  char valbuf[4];
  int len;
  int odd_sized_struct;

  /* first force sp to a 4-byte alignment */
  sp = sh_frame_align (gdbarch, sp);

  /* The "struct return pointer" pseudo-argument has its own dedicated 
     register */
  if (struct_return)
    regcache_cooked_write_unsigned (regcache,
				    STRUCT_RETURN_REGNUM,
				    struct_addr);

  /* Now make sure there's space on the stack */
  for (argnum = 0, stack_alloc = 0; argnum < nargs; argnum++)
    stack_alloc += ((TYPE_LENGTH (VALUE_TYPE (args[argnum])) + 3) & ~3);
  sp -= stack_alloc;		/* make room on stack for args */

  /* Now load as many as possible of the first arguments into
     registers, and push the rest onto the stack.  There are 16 bytes
     in four registers available.  Loop thru args from first to last.  */

  argreg = ARG0_REGNUM;
  for (argnum = 0, stack_offset = 0; argnum < nargs; argnum++)
    {
      type = VALUE_TYPE (args[argnum]);
      len = TYPE_LENGTH (type);
      memset (valbuf, 0, sizeof (valbuf));
      if (len < 4)
	{
	  /* value gets right-justified in the register or stack word */
	  if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
	    memcpy (valbuf + (4 - len),
		    (char *) VALUE_CONTENTS (args[argnum]), len);
	  else
	    memcpy (valbuf, (char *) VALUE_CONTENTS (args[argnum]), len);
	  val = valbuf;
	}
      else
	val = (char *) VALUE_CONTENTS (args[argnum]);

      if (len > 4 && (len & 3) != 0)
	odd_sized_struct = 1;	/* such structs go entirely on stack */
      else
	odd_sized_struct = 0;
      while (len > 0)
	{
	  if (argreg > ARGLAST_REGNUM
	      || odd_sized_struct)
	    {			
	      /* must go on the stack */
	      write_memory (sp + stack_offset, val, 4);
	      stack_offset += 4;
	    }
	  /* NOTE WELL!!!!!  This is not an "else if" clause!!!
	     That's because some *&^%$ things get passed on the stack
	     AND in the registers!   */
	  if (argreg <= ARGLAST_REGNUM)
	    {			
	      /* there's room in a register */
	      regval = extract_unsigned_integer (val, register_size (gdbarch,
								     argreg));
	      regcache_cooked_write_unsigned (regcache, argreg++, regval);
	    }
	  /* Store the value 4 bytes at a time.  This means that things
	     larger than 4 bytes may go partly in registers and partly
	     on the stack.  */
	  len -= register_size (gdbarch, argreg);
	  val += register_size (gdbarch, argreg);
	}
    }

  /* Store return address. */
  regcache_cooked_write_unsigned (regcache, PR_REGNUM, bp_addr);

  /* Update stack pointer.  */
  regcache_cooked_write_unsigned (regcache, SP_REGNUM, sp);

  return sp;
}

/* Find a function's return value in the appropriate registers (in
   regbuf), and copy it into valbuf.  Extract from an array REGBUF
   containing the (raw) register state a function return value of type
   TYPE, and copy that, in virtual format, into VALBUF.  */
static void
sh_default_extract_return_value (struct type *type, struct regcache *regcache,
				 void *valbuf)
{
  int len = TYPE_LENGTH (type);
  int return_register = R0_REGNUM;
  int offset;
  
  if (len <= 4)
    {
      ULONGEST c;

      regcache_cooked_read_unsigned (regcache, R0_REGNUM, &c);
      store_unsigned_integer (valbuf, len, c);
    }
  else if (len == 8)
    {
      int i, regnum = R0_REGNUM;
      for (i = 0; i < len; i += 4)
	regcache_raw_read (regcache, regnum++, (char *)valbuf + i);
    }
  else
    error ("bad size for return value");
}

static void
sh3e_sh4_extract_return_value (struct type *type, struct regcache *regcache,
			       void *valbuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT)
    {
      int len = TYPE_LENGTH (type);
      int i, regnum = FP0_REGNUM;
      for (i = 0; i < len; i += 4)
	regcache_raw_read (regcache, regnum++, (char *)valbuf + i);
    }
  else
    sh_default_extract_return_value (type, regcache, valbuf);
}

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.
   If the architecture is sh4 or sh3e, store a function's return value
   in the R0 general register or in the FP0 floating point register,
   depending on the type of the return value. In all the other cases
   the result is stored in r0, left-justified. */
static void
sh_default_store_return_value (struct type *type, struct regcache *regcache,
			       const void *valbuf)
{
  ULONGEST val;
  int len = TYPE_LENGTH (type);

  if (len <= 4)
    {
      val = extract_unsigned_integer (valbuf, len);
      regcache_cooked_write_unsigned (regcache, R0_REGNUM, val);
    }
  else
    {
      int i, regnum = R0_REGNUM;
      for (i = 0; i < len; i += 4)
        regcache_raw_write (regcache, regnum++, (char *)valbuf + i);
    }
}

static void
sh3e_sh4_store_return_value (struct type *type, struct regcache *regcache,
			     const void *valbuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT) 
    {
      int len = TYPE_LENGTH (type);
      int i, regnum = FP0_REGNUM;
      for (i = 0; i < len; i += 4)
        regcache_raw_write (regcache, regnum++, (char *)valbuf + i);
    }
  else
    sh_default_store_return_value (type, regcache, valbuf);
}

/* Print the registers in a form similar to the E7000 */

static void
sh_generic_show_regs (void)
{
  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));
}

static void
sh3_show_regs (void)
{
  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));
  printf_filtered (" SSR=%08lx SPC=%08lx",
	           (long) read_register (SSR_REGNUM),
		   (long) read_register (SPC_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));
}


static void
sh2e_show_regs (void)
{
  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));
  printf_filtered (" FPUL=%08lx FPSCR=%08lx",
	  	   (long) read_register (FPUL_REGNUM),
                   (long) read_register (FPSCR_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));

  printf_filtered (("FP0-FP7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		   (long) read_register (FP0_REGNUM + 0),
		   (long) read_register (FP0_REGNUM + 1),
		   (long) read_register (FP0_REGNUM + 2),
		   (long) read_register (FP0_REGNUM + 3),
		   (long) read_register (FP0_REGNUM + 4),
		   (long) read_register (FP0_REGNUM + 5),
		   (long) read_register (FP0_REGNUM + 6),
		   (long) read_register (FP0_REGNUM + 7));
  printf_filtered (("FP8-FP15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		   (long) read_register (FP0_REGNUM + 8),
		   (long) read_register (FP0_REGNUM + 9),
		   (long) read_register (FP0_REGNUM + 10),
		   (long) read_register (FP0_REGNUM + 11),
		   (long) read_register (FP0_REGNUM + 12),
		   (long) read_register (FP0_REGNUM + 13),
		   (long) read_register (FP0_REGNUM + 14),
		   (long) read_register (FP0_REGNUM + 15));
}

static void
sh3e_show_regs (void)
{
  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));
  printf_filtered (" SSR=%08lx SPC=%08lx",
		   (long) read_register (SSR_REGNUM),
		   (long) read_register (SPC_REGNUM));
  printf_filtered (" FPUL=%08lx FPSCR=%08lx",
		   (long) read_register (FPUL_REGNUM),
		   (long) read_register (FPSCR_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));

  printf_filtered (("FP0-FP7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		   (long) read_register (FP0_REGNUM + 0),
		   (long) read_register (FP0_REGNUM + 1),
		   (long) read_register (FP0_REGNUM + 2),
		   (long) read_register (FP0_REGNUM + 3),
		   (long) read_register (FP0_REGNUM + 4),
		   (long) read_register (FP0_REGNUM + 5),
		   (long) read_register (FP0_REGNUM + 6),
		   (long) read_register (FP0_REGNUM + 7));
  printf_filtered (("FP8-FP15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		   (long) read_register (FP0_REGNUM + 8),
		   (long) read_register (FP0_REGNUM + 9),
		   (long) read_register (FP0_REGNUM + 10),
		   (long) read_register (FP0_REGNUM + 11),
		   (long) read_register (FP0_REGNUM + 12),
		   (long) read_register (FP0_REGNUM + 13),
		   (long) read_register (FP0_REGNUM + 14),
		   (long) read_register (FP0_REGNUM + 15));
}

static void
sh3_dsp_show_regs (void)
{
  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));

  printf_filtered (" SSR=%08lx SPC=%08lx",
		   (long) read_register (SSR_REGNUM),
		   (long) read_register (SPC_REGNUM));

  printf_filtered (" DSR=%08lx", 
		   (long) read_register (DSR_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));

  printf_filtered ("A0G=%02lx A0=%08lx M0=%08lx X0=%08lx Y0=%08lx RS=%08lx MOD=%08lx\n",
		   (long) read_register (A0G_REGNUM) & 0xff,
		   (long) read_register (A0_REGNUM),
		   (long) read_register (M0_REGNUM),
		   (long) read_register (X0_REGNUM),
		   (long) read_register (Y0_REGNUM),
		   (long) read_register (RS_REGNUM),
		   (long) read_register (MOD_REGNUM));
  printf_filtered ("A1G=%02lx A1=%08lx M1=%08lx X1=%08lx Y1=%08lx RE=%08lx\n",
		   (long) read_register (A1G_REGNUM) & 0xff,
		   (long) read_register (A1_REGNUM),
		   (long) read_register (M1_REGNUM),
		   (long) read_register (X1_REGNUM),
		   (long) read_register (Y1_REGNUM),
		   (long) read_register (RE_REGNUM));
}

static void
sh4_show_regs (void)
{
  int pr = read_register (FPSCR_REGNUM) & 0x80000;
  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));
  printf_filtered (" SSR=%08lx SPC=%08lx",
		   (long) read_register (SSR_REGNUM),
		   (long) read_register (SPC_REGNUM));
  printf_filtered (" FPUL=%08lx FPSCR=%08lx",
		   (long) read_register (FPUL_REGNUM),
		   (long) read_register (FPSCR_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));

  printf_filtered ((pr
		    ? "DR0-DR6  %08lx%08lx %08lx%08lx %08lx%08lx %08lx%08lx\n"
		    : "FP0-FP7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		   (long) read_register (FP0_REGNUM + 0),
		   (long) read_register (FP0_REGNUM + 1),
		   (long) read_register (FP0_REGNUM + 2),
		   (long) read_register (FP0_REGNUM + 3),
		   (long) read_register (FP0_REGNUM + 4),
		   (long) read_register (FP0_REGNUM + 5),
		   (long) read_register (FP0_REGNUM + 6),
		   (long) read_register (FP0_REGNUM + 7));
  printf_filtered ((pr
		    ? "DR8-DR14 %08lx%08lx %08lx%08lx %08lx%08lx %08lx%08lx\n"
		    : "FP8-FP15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n"),
		   (long) read_register (FP0_REGNUM + 8),
		   (long) read_register (FP0_REGNUM + 9),
		   (long) read_register (FP0_REGNUM + 10),
		   (long) read_register (FP0_REGNUM + 11),
		   (long) read_register (FP0_REGNUM + 12),
		   (long) read_register (FP0_REGNUM + 13),
		   (long) read_register (FP0_REGNUM + 14),
		   (long) read_register (FP0_REGNUM + 15));
}

static void
sh_dsp_show_regs (void)
{
  printf_filtered ("PC=%s SR=%08lx PR=%08lx MACH=%08lx MACHL=%08lx\n",
		   paddr (read_register (PC_REGNUM)),
		   (long) read_register (SR_REGNUM),
		   (long) read_register (PR_REGNUM),
		   (long) read_register (MACH_REGNUM),
		   (long) read_register (MACL_REGNUM));

  printf_filtered ("GBR=%08lx VBR=%08lx",
		   (long) read_register (GBR_REGNUM),
		   (long) read_register (VBR_REGNUM));

  printf_filtered (" DSR=%08lx", 
		   (long) read_register (DSR_REGNUM));

  printf_filtered ("\nR0-R7  %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (0),
		   (long) read_register (1),
		   (long) read_register (2),
		   (long) read_register (3),
		   (long) read_register (4),
		   (long) read_register (5),
		   (long) read_register (6),
		   (long) read_register (7));
  printf_filtered ("R8-R15 %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
		   (long) read_register (8),
		   (long) read_register (9),
		   (long) read_register (10),
		   (long) read_register (11),
		   (long) read_register (12),
		   (long) read_register (13),
		   (long) read_register (14),
		   (long) read_register (15));

  printf_filtered ("A0G=%02lx A0=%08lx M0=%08lx X0=%08lx Y0=%08lx RS=%08lx MOD=%08lx\n",
		   (long) read_register (A0G_REGNUM) & 0xff,
		   (long) read_register (A0_REGNUM),
		   (long) read_register (M0_REGNUM),
		   (long) read_register (X0_REGNUM),
		   (long) read_register (Y0_REGNUM),
		   (long) read_register (RS_REGNUM),
		   (long) read_register (MOD_REGNUM));
  printf_filtered ("A1G=%02lx A1=%08lx M1=%08lx X1=%08lx Y1=%08lx RE=%08lx\n",
		   (long) read_register (A1G_REGNUM) & 0xff,
		   (long) read_register (A1_REGNUM),
		   (long) read_register (M1_REGNUM),
		   (long) read_register (X1_REGNUM),
		   (long) read_register (Y1_REGNUM),
		   (long) read_register (RE_REGNUM));
}

static void
sh_show_regs_command (char *args, int from_tty)
{
  if (sh_show_regs)
    (*sh_show_regs)();
}

/* Return the GDB type object for the "standard" data type
   of data in register N.  */
static struct type *
sh_sh3e_register_type (struct gdbarch *gdbarch, int reg_nr)
{
  if ((reg_nr >= FP0_REGNUM
       && (reg_nr <= FP_LAST_REGNUM)) 
      || (reg_nr == FPUL_REGNUM))
    return builtin_type_float;
  else
    return builtin_type_int;
}

static struct type *
sh_sh4_build_float_register_type (int high)
{
  struct type *temp;

  temp = create_range_type (NULL, builtin_type_int, 0, high);
  return create_array_type (NULL, builtin_type_float, temp);
}

static struct type *
sh_sh4_register_type (struct gdbarch *gdbarch, int reg_nr)
{
  if ((reg_nr >= FP0_REGNUM
       && (reg_nr <= FP_LAST_REGNUM)) 
      || (reg_nr == FPUL_REGNUM))
    return builtin_type_float;
  else if (reg_nr >= DR0_REGNUM 
	   && reg_nr <= DR_LAST_REGNUM)
    return builtin_type_double;
  else if  (reg_nr >= FV0_REGNUM 
	   && reg_nr <= FV_LAST_REGNUM)
    return sh_sh4_build_float_register_type (3);
  else
    return builtin_type_int;
}

static struct type *
sh_default_register_type (struct gdbarch *gdbarch, int reg_nr)
{
  return builtin_type_int;
}

/* On the sh4, the DRi pseudo registers are problematic if the target
   is little endian. When the user writes one of those registers, for
   instance with 'ser var $dr0=1', we want the double to be stored
   like this: 
   fr0 = 0x00 0x00 0x00 0x00 0x00 0xf0 0x3f 
   fr1 = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 

   This corresponds to little endian byte order & big endian word
   order.  However if we let gdb write the register w/o conversion, it
   will write fr0 and fr1 this way:
   fr0 = 0x00 0x00 0x00 0x00 0x00 0x00 0x00
   fr1 = 0x00 0x00 0x00 0x00 0x00 0xf0 0x3f
   because it will consider fr0 and fr1 as a single LE stretch of memory.
   
   To achieve what we want we must force gdb to store things in
   floatformat_ieee_double_littlebyte_bigword (which is defined in
   include/floatformat.h and libiberty/floatformat.c.

   In case the target is big endian, there is no problem, the
   raw bytes will look like:
   fr0 = 0x3f 0xf0 0x00 0x00 0x00 0x00 0x00
   fr1 = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 

   The other pseudo registers (the FVs) also don't pose a problem
   because they are stored as 4 individual FP elements. */

static void
sh_sh4_register_convert_to_virtual (int regnum, struct type *type,
                                  char *from, char *to)
{
  if (regnum >= DR0_REGNUM 
      && regnum <= DR_LAST_REGNUM)
    {
      DOUBLEST val;
      floatformat_to_doublest (&floatformat_ieee_double_littlebyte_bigword, from, &val);
      store_typed_floating (to, type, val);
    }
  else
    error ("sh_register_convert_to_virtual called with non DR register number");
}

static void
sh_sh4_register_convert_to_raw (struct type *type, int regnum,
				const void *from, void *to)
{
  if (regnum >= DR0_REGNUM 
      && regnum <= DR_LAST_REGNUM)
    {
      DOUBLEST val = extract_typed_floating (from, type);
      floatformat_from_doublest (&floatformat_ieee_double_littlebyte_bigword, &val, to);
    }
  else
    error("sh_register_convert_to_raw called with non DR register number");
}

static void
sh_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
			 int reg_nr, void *buffer)
{
  int base_regnum, portion;
  char temp_buffer[MAX_REGISTER_SIZE];

  if (reg_nr >= DR0_REGNUM 
      && reg_nr <= DR_LAST_REGNUM)
    {
      base_regnum = dr_reg_base_num (reg_nr);

      /* Build the value in the provided buffer. */ 
      /* Read the real regs for which this one is an alias.  */
      for (portion = 0; portion < 2; portion++)
	regcache_raw_read (regcache, base_regnum + portion, 
			   (temp_buffer
			    + register_size (gdbarch, base_regnum) * portion));
      /* We must pay attention to the endiannes. */
      sh_sh4_register_convert_to_virtual (reg_nr,
					  gdbarch_register_type (gdbarch, reg_nr),
					  temp_buffer, buffer);
    }
  else if (reg_nr >= FV0_REGNUM 
	   && reg_nr <= FV_LAST_REGNUM)
    {
      base_regnum = fv_reg_base_num (reg_nr);

      /* Read the real regs for which this one is an alias.  */
      for (portion = 0; portion < 4; portion++)
	regcache_raw_read (regcache, base_regnum + portion, 
			   ((char *) buffer
			    + register_size (gdbarch, base_regnum) * portion));
    }
}

static void
sh_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
			  int reg_nr, const void *buffer)
{
  int base_regnum, portion;
  char temp_buffer[MAX_REGISTER_SIZE];

  if (reg_nr >= DR0_REGNUM
      && reg_nr <= DR_LAST_REGNUM)
    {
      base_regnum = dr_reg_base_num (reg_nr);

      /* We must pay attention to the endiannes. */
      sh_sh4_register_convert_to_raw (gdbarch_register_type (gdbarch, reg_nr), reg_nr,
				      buffer, temp_buffer);

      /* Write the real regs for which this one is an alias.  */
      for (portion = 0; portion < 2; portion++)
	regcache_raw_write (regcache, base_regnum + portion, 
			    (temp_buffer
			     + register_size (gdbarch, base_regnum) * portion));
    }
  else if (reg_nr >= FV0_REGNUM
	   && reg_nr <= FV_LAST_REGNUM)
    {
      base_regnum = fv_reg_base_num (reg_nr);

      /* Write the real regs for which this one is an alias.  */
      for (portion = 0; portion < 4; portion++)
	regcache_raw_write (regcache, base_regnum + portion,
			    ((char *) buffer
			     + register_size (gdbarch, base_regnum) * portion));
    }
}

/* Floating point vector of 4 float registers. */
static void
do_fv_register_info (struct gdbarch *gdbarch, struct ui_file *file,
		     int fv_regnum)
{
  int first_fp_reg_num = fv_reg_base_num (fv_regnum);
  fprintf_filtered (file, "fv%d\t0x%08x\t0x%08x\t0x%08x\t0x%08x\n", 
		     fv_regnum - FV0_REGNUM, 
		     (int) read_register (first_fp_reg_num),
		     (int) read_register (first_fp_reg_num + 1),
		     (int) read_register (first_fp_reg_num + 2),
		     (int) read_register (first_fp_reg_num + 3));
}

/* Double precision registers. */
static void
do_dr_register_info (struct gdbarch *gdbarch, struct ui_file *file,
		     int dr_regnum)
{
  int first_fp_reg_num = dr_reg_base_num (dr_regnum);

  fprintf_filtered (file, "dr%d\t0x%08x%08x\n", 
		    dr_regnum - DR0_REGNUM, 
		    (int) read_register (first_fp_reg_num),
		    (int) read_register (first_fp_reg_num + 1));
}

static void
sh_print_pseudo_register (struct gdbarch *gdbarch, struct ui_file *file,
			  int regnum)
{
  if (regnum < NUM_REGS || regnum >= NUM_REGS + NUM_PSEUDO_REGS)
    internal_error (__FILE__, __LINE__,
		    "Invalid pseudo register number %d\n", regnum);
  else if (regnum >= DR0_REGNUM
	   && regnum <= DR_LAST_REGNUM)
    do_dr_register_info (gdbarch, file, regnum);
  else if (regnum >= FV0_REGNUM
	   && regnum <= FV_LAST_REGNUM)
    do_fv_register_info (gdbarch, file, regnum);
}

static void
sh_do_fp_register (struct gdbarch *gdbarch, struct ui_file *file, int regnum)
{				/* do values for FP (float) regs */
  char *raw_buffer;
  double flt;	/* double extracted from raw hex data */
  int inv;
  int j;

  /* Allocate space for the float. */
  raw_buffer = (char *) alloca (register_size (gdbarch, FP0_REGNUM));

  /* Get the data in raw format.  */
  if (!frame_register_read (get_selected_frame (), regnum, raw_buffer))
    error ("can't read register %d (%s)", regnum, REGISTER_NAME (regnum));

  /* Get the register as a number */ 
  flt = unpack_double (builtin_type_float, raw_buffer, &inv);

  /* Print the name and some spaces. */
  fputs_filtered (REGISTER_NAME (regnum), file);
  print_spaces_filtered (15 - strlen (REGISTER_NAME (regnum)), file);

  /* Print the value. */
  if (inv)
    fprintf_filtered (file, "<invalid float>");
  else
    fprintf_filtered (file, "%-10.9g", flt);

  /* Print the fp register as hex. */
  fprintf_filtered (file, "\t(raw 0x");
  for (j = 0; j < register_size (gdbarch, regnum); j++)
    {
      int idx = TARGET_BYTE_ORDER == BFD_ENDIAN_BIG ? j
	: register_size (gdbarch, regnum) - 1 - j;
      fprintf_filtered (file, "%02x", (unsigned char) raw_buffer[idx]);
    }
  fprintf_filtered (file, ")");
  fprintf_filtered (file, "\n");
}

static void
sh_do_register (struct gdbarch *gdbarch, struct ui_file *file, int regnum)
{
  char raw_buffer[MAX_REGISTER_SIZE];

  fputs_filtered (REGISTER_NAME (regnum), file);
  print_spaces_filtered (15 - strlen (REGISTER_NAME (regnum)), file);

  /* Get the data in raw format.  */
  if (!frame_register_read (get_selected_frame (), regnum, raw_buffer))
    fprintf_filtered (file, "*value not available*\n");
      
  val_print (gdbarch_register_type (gdbarch, regnum), raw_buffer, 0, 0,
	     file, 'x', 1, 0, Val_pretty_default);
  fprintf_filtered (file, "\t");
  val_print (gdbarch_register_type (gdbarch, regnum), raw_buffer, 0, 0,
	     file, 0, 1, 0, Val_pretty_default);
  fprintf_filtered (file, "\n");
}

static void
sh_print_register (struct gdbarch *gdbarch, struct ui_file *file, int regnum)
{
  if (regnum < 0 || regnum >= NUM_REGS + NUM_PSEUDO_REGS)
    internal_error (__FILE__, __LINE__,
		    "Invalid register number %d\n", regnum);

  else if (regnum >= 0 && regnum < NUM_REGS)
    {
      if (TYPE_CODE (gdbarch_register_type (gdbarch, regnum)) == TYPE_CODE_FLT)
	sh_do_fp_register (gdbarch, file, regnum);	/* FP regs */
      else
	sh_do_register (gdbarch, file, regnum);	/* All other regs */
    }

  else if (regnum < NUM_REGS + NUM_PSEUDO_REGS)
    {
      sh_print_pseudo_register (gdbarch, file, regnum);
    }
}

static void
sh_print_registers_info (struct gdbarch *gdbarch, struct ui_file *file,
			 struct frame_info *frame, int regnum, int fpregs)
{
  if (regnum != -1)		/* do one specified register */
    {
      if (*(REGISTER_NAME (regnum)) == '\0')
	error ("Not a valid register for the current processor type");

      sh_print_register (gdbarch, file, regnum);
    }
  else
    /* do all (or most) registers */
    {
      regnum = 0;
      while (regnum < NUM_REGS)
	{
	  /* If the register name is empty, it is undefined for this
	     processor, so don't display anything.  */
	  if (REGISTER_NAME (regnum) == NULL
	      || *(REGISTER_NAME (regnum)) == '\0')
	    { 
	      regnum++;
	      continue;
	    }

	  if (TYPE_CODE (gdbarch_register_type (gdbarch, regnum)) == TYPE_CODE_FLT)
	    {
	      if (fpregs)
		{
		  /* true for "INFO ALL-REGISTERS" command */
		  sh_do_fp_register (gdbarch, file, regnum);	/* FP regs */
		  regnum ++;
		}
	      else
		regnum += (FP_LAST_REGNUM - FP0_REGNUM);	/* skip FP regs */
	    }
	  else
	    {
	      sh_do_register (gdbarch, file, regnum);	/* All other regs */
	      regnum++;
	    }
	}

      if (fpregs)
	while (regnum < NUM_REGS + NUM_PSEUDO_REGS)
	  {
	    sh_print_pseudo_register (gdbarch, file, regnum);
	    regnum++;
	  }
    }
}

#ifdef SVR4_SHARED_LIBS

/* Fetch (and possibly build) an appropriate link_map_offsets structure
   for native i386 linux targets using the struct offsets defined in
   link.h (but without actual reference to that file).

   This makes it possible to access i386-linux shared libraries from
   a gdb that was not built on an i386-linux host (for cross debugging).
   */

struct link_map_offsets *
sh_linux_svr4_fetch_link_map_offsets (void)
{
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = 0;

  if (lmp == 0)
    {
      lmp = &lmo;

      lmo.r_debug_size = 8;	/* 20 not actual size but all we need */

      lmo.r_map_offset = 4;
      lmo.r_map_size   = 4;

      lmo.link_map_size = 20;	/* 552 not actual size but all we need */

      lmo.l_addr_offset = 0;
      lmo.l_addr_size   = 4;

      lmo.l_name_offset = 4;
      lmo.l_name_size   = 4;

      lmo.l_next_offset = 12;
      lmo.l_next_size   = 4;

      lmo.l_prev_offset = 16;
      lmo.l_prev_size   = 4;
    }

    return lmp;
}
#endif /* SVR4_SHARED_LIBS */

static int
sh_dsp_register_sim_regno (int nr)
{
  if (legacy_register_sim_regno (nr) < 0)
    return legacy_register_sim_regno (nr);
  if (nr >= DSR_REGNUM && nr <= Y1_REGNUM)
    return nr - DSR_REGNUM + SIM_SH_DSR_REGNUM;
  if (nr == MOD_REGNUM)
    return SIM_SH_MOD_REGNUM;
  if (nr == RS_REGNUM)
    return SIM_SH_RS_REGNUM;
  if (nr == RE_REGNUM)
    return SIM_SH_RE_REGNUM;
  if (nr >= R0_BANK_REGNUM && nr <= R7_BANK_REGNUM)
    return nr - R0_BANK_REGNUM + SIM_SH_R0_BANK_REGNUM;
  return nr;
}

static gdbarch_init_ftype sh_gdbarch_init;

static struct gdbarch *
sh_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;

  sh_show_regs = sh_generic_show_regs;
  switch (info.bfd_arch_info->mach)
    {
      case bfd_mach_sh2e:
        sh_show_regs = sh2e_show_regs;
        break;
      case bfd_mach_sh_dsp:
        sh_show_regs = sh_dsp_show_regs;
        break;

      case bfd_mach_sh3:
        sh_show_regs = sh3_show_regs;
        break;

      case bfd_mach_sh3e:
        sh_show_regs = sh3e_show_regs;
        break;

      case bfd_mach_sh3_dsp:
        sh_show_regs = sh3_dsp_show_regs;
        break;

      case bfd_mach_sh4:
        sh_show_regs = sh4_show_regs;
        break;

      case bfd_mach_sh5:
	sh_show_regs = sh64_show_regs;
        /* SH5 is handled entirely in sh64-tdep.c */
        return sh64_gdbarch_init (info, arches);
    }

  /* If there is already a candidate, use it.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  /* None found, create a new architecture from the information
     provided. */
  gdbarch = gdbarch_alloc (&info, NULL);

  /* NOTE: cagney/2002-12-06: This can be deleted when this arch is
     ready to unwind the PC first (see frame.c:get_prev_frame()).  */
  set_gdbarch_deprecated_init_frame_pc (gdbarch, init_frame_pc_default);

  set_gdbarch_short_bit (gdbarch, 2 * TARGET_CHAR_BIT);
  set_gdbarch_int_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_long_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_float_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_double_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_long_double_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_ptr_bit (gdbarch, 4 * TARGET_CHAR_BIT);

  set_gdbarch_num_regs (gdbarch, SH_NUM_REGS);
  set_gdbarch_sp_regnum (gdbarch, 15);
  set_gdbarch_deprecated_fp_regnum (gdbarch, 14);
  set_gdbarch_pc_regnum (gdbarch, 16);
  set_gdbarch_fp0_regnum (gdbarch, -1);
  set_gdbarch_num_pseudo_regs (gdbarch, 0);

  set_gdbarch_breakpoint_from_pc (gdbarch, sh_breakpoint_from_pc);
  set_gdbarch_use_struct_convention (gdbarch, sh_use_struct_convention);

  set_gdbarch_print_insn (gdbarch, gdb_print_insn_sh);
  set_gdbarch_register_sim_regno (gdbarch, legacy_register_sim_regno);

  set_gdbarch_write_pc (gdbarch, generic_target_write_pc);

  set_gdbarch_skip_prologue (gdbarch, sh_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_decr_pc_after_break (gdbarch, 0);
  set_gdbarch_function_start_offset (gdbarch, 0);

  set_gdbarch_frame_args_skip (gdbarch, 0);
  set_gdbarch_frameless_function_invocation (gdbarch, frameless_look_for_prologue);
  set_gdbarch_believe_pcc_promotion (gdbarch, 1);

  set_gdbarch_deprecated_frame_chain (gdbarch, sh_frame_chain);
  set_gdbarch_deprecated_get_saved_register (gdbarch, deprecated_generic_get_saved_register);
  set_gdbarch_deprecated_init_extra_frame_info (gdbarch, sh_init_extra_frame_info);
  set_gdbarch_deprecated_pop_frame (gdbarch, sh_pop_frame);
  set_gdbarch_deprecated_frame_saved_pc (gdbarch, sh_frame_saved_pc);
  set_gdbarch_deprecated_saved_pc_after_call (gdbarch, sh_saved_pc_after_call);
  set_gdbarch_frame_align (gdbarch, sh_frame_align);

  switch (info.bfd_arch_info->mach)
    {
    case bfd_mach_sh:
      set_gdbarch_register_name (gdbarch, sh_sh_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_default_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_store_return_value (gdbarch, sh_default_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh_default_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_nofpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      break;
    case bfd_mach_sh2:
      set_gdbarch_register_name (gdbarch, sh_sh_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_default_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_store_return_value (gdbarch, sh_default_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh_default_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_nofpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      break;      
    case bfd_mach_sh2e:
      /* doubles on sh2e and sh3e are actually 4 byte. */
      set_gdbarch_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);

      set_gdbarch_register_name (gdbarch, sh_sh2e_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_sh3e_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_fp0_regnum (gdbarch, 25);
      set_gdbarch_store_return_value (gdbarch, sh3e_sh4_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh3e_sh4_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_fpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      break;
    case bfd_mach_sh_dsp:
      set_gdbarch_register_name (gdbarch, sh_sh_dsp_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_default_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_register_sim_regno (gdbarch, sh_dsp_register_sim_regno);
      set_gdbarch_store_return_value (gdbarch, sh_default_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh_default_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_nofpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      break;
    case bfd_mach_sh3:
      set_gdbarch_register_name (gdbarch, sh_sh3_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_default_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_store_return_value (gdbarch, sh_default_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh_default_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_nofpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      break;
    case bfd_mach_sh3e:
      /* doubles on sh2e and sh3e are actually 4 byte. */
      set_gdbarch_double_bit (gdbarch, 4 * TARGET_CHAR_BIT);

      set_gdbarch_register_name (gdbarch, sh_sh3e_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_sh3e_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_fp0_regnum (gdbarch, 25);
      set_gdbarch_store_return_value (gdbarch, sh3e_sh4_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh3e_sh4_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_fpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_fp_frame_init_saved_regs);
      break;
    case bfd_mach_sh3_dsp:
      set_gdbarch_register_name (gdbarch, sh_sh3_dsp_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_default_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_register_sim_regno (gdbarch, sh_dsp_register_sim_regno);
      set_gdbarch_store_return_value (gdbarch, sh_default_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh_default_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_nofpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      break;
    case bfd_mach_sh4:
      set_gdbarch_register_name (gdbarch, sh_sh4_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_sh4_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_fp0_regnum (gdbarch, 25);
      set_gdbarch_num_pseudo_regs (gdbarch, 12);
      set_gdbarch_pseudo_register_read (gdbarch, sh_pseudo_register_read);
      set_gdbarch_pseudo_register_write (gdbarch, sh_pseudo_register_write);
      set_gdbarch_store_return_value (gdbarch, sh3e_sh4_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh3e_sh4_extract_return_value);
      set_gdbarch_push_dummy_call (gdbarch, sh_push_dummy_call_fpu);
      set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_fp_frame_init_saved_regs);
      break;
    default:
      set_gdbarch_register_name (gdbarch, sh_generic_register_name);
      set_gdbarch_print_registers_info (gdbarch, sh_print_registers_info);
      set_gdbarch_register_type (gdbarch, sh_default_register_type);
      set_gdbarch_push_dummy_code (gdbarch, sh_push_dummy_code);
      set_gdbarch_store_return_value (gdbarch, sh_default_store_return_value);
      set_gdbarch_extract_return_value (gdbarch, sh_default_extract_return_value);

      set_gdbarch_deprecated_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      break;
    }

  /* Hook in ABI-specific overrides, if they have been registered.  */
  gdbarch_init_osabi (info, gdbarch);

  return gdbarch;
}

extern initialize_file_ftype _initialize_sh_tdep; /* -Wmissing-prototypes */

void
_initialize_sh_tdep (void)
{
  struct cmd_list_element *c;
  
  gdbarch_register (bfd_arch_sh, sh_gdbarch_init, NULL);

  add_com ("regs", class_vars, sh_show_regs_command, "Print all registers");
}
