/* Target-dependent code for Hitachi Super-H, for GDB.
   Copyright 1993, 1994, 1995, 1996, 1997, 1998, 2000 Free Software
   Foundation, Inc.

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
#include "obstack.h"
#include "symtab.h"
#include "symfile.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "value.h"
#include "dis-asm.h"
#include "inferior.h"		/* for BEFORE_TEXT_END etc. */
#include "gdb_string.h"
#include "arch-utils.h"
#include "floatformat.h"

#include "solib-svr4.h"

#undef XMALLOC
#define XMALLOC(TYPE) ((TYPE*) xmalloc (sizeof (TYPE)))


/* Frame interpretation related functions. */
static gdbarch_breakpoint_from_pc_ftype sh_breakpoint_from_pc;
static gdbarch_frame_chain_ftype sh_frame_chain;
static gdbarch_frame_saved_pc_ftype sh_frame_saved_pc;
static gdbarch_skip_prologue_ftype sh_skip_prologue;

static gdbarch_frame_init_saved_regs_ftype sh_nofp_frame_init_saved_regs;
static gdbarch_frame_init_saved_regs_ftype sh_fp_frame_init_saved_regs;
static gdbarch_init_extra_frame_info_ftype sh_init_extra_frame_info;
static gdbarch_pop_frame_ftype sh_pop_frame;
static gdbarch_saved_pc_after_call_ftype sh_saved_pc_after_call;
static gdbarch_frame_args_address_ftype sh_frame_args_address;
static gdbarch_frame_locals_address_ftype sh_frame_locals_address;

/* Function call related functions. */
static gdbarch_extract_return_value_ftype sh_extract_return_value;
static gdbarch_extract_struct_value_address_ftype sh_extract_struct_value_address;
static gdbarch_use_struct_convention_ftype sh_use_struct_convention;
static gdbarch_store_struct_return_ftype sh_store_struct_return;
static gdbarch_push_arguments_ftype sh_push_arguments;
static gdbarch_push_return_address_ftype sh_push_return_address;
static gdbarch_coerce_float_to_double_ftype sh_coerce_float_to_double;
static gdbarch_store_return_value_ftype	sh_default_store_return_value;
static gdbarch_store_return_value_ftype	sh3e_sh4_store_return_value;

static gdbarch_register_name_ftype sh_generic_register_name;
static gdbarch_register_name_ftype sh_sh_register_name;
static gdbarch_register_name_ftype sh_sh3_register_name;
static gdbarch_register_name_ftype sh_sh3e_register_name;
static gdbarch_register_name_ftype sh_sh_dsp_register_name;
static gdbarch_register_name_ftype sh_sh3_dsp_register_name;

/* Registers display related functions */
static gdbarch_register_raw_size_ftype sh_default_register_raw_size;
static gdbarch_register_raw_size_ftype sh_sh4_register_raw_size;

static gdbarch_register_virtual_size_ftype sh_register_virtual_size;

static gdbarch_register_byte_ftype sh_default_register_byte;
static gdbarch_register_byte_ftype sh_sh4_register_byte;

static gdbarch_register_virtual_type_ftype sh_sh3e_register_virtual_type;
static gdbarch_register_virtual_type_ftype sh_sh4_register_virtual_type;
static gdbarch_register_virtual_type_ftype sh_default_register_virtual_type;

static void sh_generic_show_regs (void);
static void sh3_show_regs (void);
static void sh3e_show_regs (void);
static void sh3_dsp_show_regs (void);
static void sh_dsp_show_regs (void);
static void sh4_show_regs (void);
static void sh_show_regs_command (char *, int);

static struct type *sh_sh4_build_float_register_type (int high);

static gdbarch_fetch_pseudo_register_ftype sh_fetch_pseudo_register;
static gdbarch_store_pseudo_register_ftype sh_store_pseudo_register;
static int fv_reg_base_num (int);
static int dr_reg_base_num (int);
static void do_fv_register_info (int fv_regnum);
static void do_dr_register_info (int dr_regnum);
static void sh_do_pseudo_register (int regnum);
static void sh_do_fp_register (int regnum);
static void sh_do_register (int regnum);
static void sh_print_register (int regnum);

void (*sh_show_regs) (void);


/* Define other aspects of the stack frame.
   we keep a copy of the worked out return pc lying around, since it
   is a useful bit of info */
  
struct frame_extra_info
{
  CORE_ADDR return_pc;
  int leaf_function;
  int f_offset;
};

#if 0
#ifdef _WIN32_WCE
char **sh_register_names = sh3_reg_names;
#else
char **sh_register_names = sh_generic_reg_names;
#endif
#endif

static char *
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

static char *
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

static char *
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

static char *
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

static char *
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

static char *
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

static char *
sh_sh4_register_name (int reg_nr)
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
    "dr0",  "dr2",  "dr4",  "dr6",  "dr8",  "dr10", "dr12", "dr14",
    "fv0",  "fv4",  "fv8",  "fv12",
  };
  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= (sizeof (register_names) / sizeof (*register_names)))
    return NULL;
  return register_names[reg_nr];
}

static unsigned char *
sh_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  /* 0xc3c3 is trapa #c3, and it works in big and little endian modes */
  static unsigned char breakpoint[] =  {0xc3, 0xc3};
  
  *lenptr = sizeof (breakpoint);
  return breakpoint;
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
#define IS_ADD_SP(x) 		(((x) & 0xff00) == 0x7f00)

#define IS_MOV_R3(x) 		(((x) & 0xff00) == 0x1a00)
#define IS_SHLL_R3(x)		((x) == 0x4300)

/* ADD r3,r15      0011111100111100
   r15+r3-->r15 */
#define IS_ADD_R3SP(x)		((x) == 0x3f3c)

/* FMOV.S FRm,@-Rn  Rn-4-->Rn, FRm-->(Rn)     1111nnnnmmmm1011
   FMOV DRm,@-Rn    Rn-8-->Rn, DRm-->(Rn)     1111nnnnmmm01011
   FMOV XDm,@-Rn    Rn-8-->Rn, XDm-->(Rn)     1111nnnnmmm11011 */
#define IS_FMOV(x)		(((x) & 0xf00f) == 0xf00b)

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
#define IS_MOV_R14(x) \
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
skip_prologue_hard_way (CORE_ADDR start_pc)
{
  CORE_ADDR here, end;
  int updated_fp = 0;

  if (!start_pc)
    return 0;

  for (here = start_pc, end = start_pc + (2 * 28); here < end;)
    {
      int w = read_memory_integer (here, 2);
      here += 2;
      if (IS_FMOV (w) || IS_PUSH (w) || IS_STS (w) || IS_MOV_R3 (w)
	  || IS_ADD_R3SP (w) || IS_ADD_SP (w) || IS_SHLL_R3 (w) 
	  || IS_ARG_MOV (w) || IS_MOV_R14 (w))
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
    return (skip_prologue_hard_way (pc));
}

/* Immediately after a function call, return the saved pc.
   Can't always go through the frames for this because on some machines
   the new frame is not set up until the new function executes
   some instructions.

   The return address is the value saved in the PR register + 4  */
static CORE_ADDR
sh_saved_pc_after_call (struct frame_info *frame)
{
  return (ADDR_BITS_REMOVE(read_register(PR_REGNUM)));
}

/* Should call_function allocate stack space for a struct return?  */
static int
sh_use_struct_convention (int gcc_p, struct type *type)
{
  return (TYPE_LENGTH (type) > 1);
}

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function.

   We store structs through a pointer passed in R0 */
static void
sh_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
{
  write_register (STRUCT_RETURN_REGNUM, (addr));
}

/* Disassemble an instruction.  */
static int
gdb_print_insn_sh (bfd_vma memaddr, disassemble_info *info)
{
  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    return print_insn_sh (memaddr, info);
  else
    return print_insn_shl (memaddr, info);
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */
static CORE_ADDR
sh_frame_chain (struct frame_info *frame)
{
  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    return frame->frame;	/* dummy frame same as caller's frame */
  if (frame->pc && !inside_entry_file (frame->pc))
    return read_memory_integer (FRAME_FP (frame) + frame->extra_info->f_offset, 4);
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
  for (; fi; fi = fi->next)
    if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
      /* When the caller requests PR from the dummy frame, we return PC because
         that's where the previous routine appears to have done a call from. */
      return generic_read_register_dummy (fi->pc, fi->frame, regnum);
    else
      {
	FRAME_INIT_SAVED_REGS (fi);
	if (!fi->pc)
	  return 0;
	if (fi->saved_regs[regnum] != 0)
	  return read_memory_integer (fi->saved_regs[regnum],
				      REGISTER_RAW_SIZE (regnum));
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
  int where[NUM_REGS];
  int rn;
  int have_fp = 0;
  int depth;
  int pc;
  int opc;
  int insn;
  int r3_val = 0;
  char *dummy_regs = generic_find_dummy_frame (fi->pc, fi->frame);
  
  if (fi->saved_regs == NULL)
    frame_saved_regs_zalloc (fi);
  else
    memset (fi->saved_regs, 0, SIZEOF_FRAME_SAVED_REGS);
  
  if (dummy_regs)
    {
      /* DANGER!  This is ONLY going to work if the char buffer format of
         the saved registers is byte-for-byte identical to the 
         CORE_ADDR regs[NUM_REGS] format used by struct frame_saved_regs! */
      memcpy (fi->saved_regs, dummy_regs, sizeof (fi->saved_regs));
      return;
    }

  fi->extra_info->leaf_function = 1;
  fi->extra_info->f_offset = 0;

  for (rn = 0; rn < NUM_REGS; rn++)
    where[rn] = -1;

  depth = 0;

  /* Loop around examining the prologue insns until we find something
     that does not appear to be part of the prologue.  But give up
     after 20 of them, since we're getting silly then. */

  pc = get_pc_function_start (fi->pc);
  if (!pc)
    {
      fi->pc = 0;
      return;
    }

  for (opc = pc + (2 * 28); pc < opc; pc += 2)
    {
      insn = read_memory_integer (pc, 2);
      /* See where the registers will be saved to */
      if (IS_PUSH (insn))
	{
	  rn = GET_PUSHED_REG (insn);
	  where[rn] = depth;
	  depth += 4;
	}
      else if (IS_STS (insn))
	{
	  where[PR_REGNUM] = depth;
	  /* If we're storing the pr then this isn't a leaf */
	  fi->extra_info->leaf_function = 0;
	  depth += 4;
	}
      else if (IS_MOV_R3 (insn))
	{
	  r3_val = ((insn & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_SHLL_R3 (insn))
	{
	  r3_val <<= 1;
	}
      else if (IS_ADD_R3SP (insn))
	{
	  depth += -r3_val;
	}
      else if (IS_ADD_SP (insn))
	{
	  depth -= ((insn & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_MOV_SP_FP (insn))
	break;
#if 0 /* This used to just stop when it found an instruction that
	 was not considered part of the prologue.  Now, we just
	 keep going looking for likely instructions. */
      else
	break;
#endif
    }

  /* Now we know how deep things are, we can work out their addresses */

  for (rn = 0; rn < NUM_REGS; rn++)
    {
      if (where[rn] >= 0)
	{
	  if (rn == FP_REGNUM)
	    have_fp = 1;

	  fi->saved_regs[rn] = fi->frame - where[rn] + depth - 4;
	}
      else
	{
	  fi->saved_regs[rn] = 0;
	}
    }

  if (have_fp)
    {
      fi->saved_regs[SP_REGNUM] = read_memory_integer (fi->saved_regs[FP_REGNUM], 4);
    }
  else
    {
      fi->saved_regs[SP_REGNUM] = fi->frame - 4;
    }

  fi->extra_info->f_offset = depth - where[FP_REGNUM] - 4;
  /* Work out the return pc - either from the saved pr or the pr
     value */
}

static void
sh_fp_frame_init_saved_regs (struct frame_info *fi)
{
  int where[NUM_REGS];
  int rn;
  int have_fp = 0;
  int depth;
  int pc;
  int opc;
  int insn;
  int r3_val = 0;
  char *dummy_regs = generic_find_dummy_frame (fi->pc, fi->frame);
  
  if (fi->saved_regs == NULL)
    frame_saved_regs_zalloc (fi);
  else
    memset (fi->saved_regs, 0, SIZEOF_FRAME_SAVED_REGS);
  
  if (dummy_regs)
    {
      /* DANGER!  This is ONLY going to work if the char buffer format of
         the saved registers is byte-for-byte identical to the 
         CORE_ADDR regs[NUM_REGS] format used by struct frame_saved_regs! */
      memcpy (fi->saved_regs, dummy_regs, sizeof (fi->saved_regs));
      return;
    }

  fi->extra_info->leaf_function = 1;
  fi->extra_info->f_offset = 0;

  for (rn = 0; rn < NUM_REGS; rn++)
    where[rn] = -1;

  depth = 0;

  /* Loop around examining the prologue insns until we find something
     that does not appear to be part of the prologue.  But give up
     after 20 of them, since we're getting silly then. */

  pc = get_pc_function_start (fi->pc);
  if (!pc)
    {
      fi->pc = 0;
      return;
    }

  for (opc = pc + (2 * 28); pc < opc; pc += 2)
    {
      insn = read_memory_integer (pc, 2);
      /* See where the registers will be saved to */
      if (IS_PUSH (insn))
	{
	  rn = GET_PUSHED_REG (insn);
	  where[rn] = depth;
	  depth += 4;
	}
      else if (IS_STS (insn))
	{
	  where[PR_REGNUM] = depth;
	  /* If we're storing the pr then this isn't a leaf */
	  fi->extra_info->leaf_function = 0;
	  depth += 4;
	}
      else if (IS_MOV_R3 (insn))
	{
	  r3_val = ((insn & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_SHLL_R3 (insn))
	{
	  r3_val <<= 1;
	}
      else if (IS_ADD_R3SP (insn))
	{
	  depth += -r3_val;
	}
      else if (IS_ADD_SP (insn))
	{
	  depth -= ((insn & 0xff) ^ 0x80) - 0x80;
	}
      else if (IS_FMOV (insn))
	{
	  if (read_register (gdbarch_tdep (current_gdbarch)->FPSCR_REGNUM) & FPSCR_SZ)
	    {
	      depth += 8;
	    }
	  else
	    {
	      depth += 4;
	    }
	}
      else if (IS_MOV_SP_FP (insn))
	break;
#if 0 /* This used to just stop when it found an instruction that
	 was not considered part of the prologue.  Now, we just
	 keep going looking for likely instructions. */
      else
	break;
#endif
    }

  /* Now we know how deep things are, we can work out their addresses */

  for (rn = 0; rn < NUM_REGS; rn++)
    {
      if (where[rn] >= 0)
	{
	  if (rn == FP_REGNUM)
	    have_fp = 1;

	  fi->saved_regs[rn] = fi->frame - where[rn] + depth - 4;
	}
      else
	{
	  fi->saved_regs[rn] = 0;
	}
    }

  if (have_fp)
    {
      fi->saved_regs[SP_REGNUM] = read_memory_integer (fi->saved_regs[FP_REGNUM], 4);
    }
  else
    {
      fi->saved_regs[SP_REGNUM] = fi->frame - 4;
    }

  fi->extra_info->f_offset = depth - where[FP_REGNUM] - 4;
  /* Work out the return pc - either from the saved pr or the pr
     value */
}

/* Initialize the extra info saved in a FRAME */
static void
sh_init_extra_frame_info (int fromleaf, struct frame_info *fi)
{

  fi->extra_info = (struct frame_extra_info *)
    frame_obstack_alloc (sizeof (struct frame_extra_info));

  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);

  if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
    {
      /* We need to setup fi->frame here because run_stack_dummy gets it wrong
         by assuming it's always FP.  */
      fi->frame = generic_read_register_dummy (fi->pc, fi->frame,
					       SP_REGNUM);
      fi->extra_info->return_pc = generic_read_register_dummy (fi->pc, fi->frame,
							       PC_REGNUM);
      fi->extra_info->f_offset = -(CALL_DUMMY_LENGTH + 4);
      fi->extra_info->leaf_function = 0;
      return;
    }
  else
    {
      FRAME_INIT_SAVED_REGS (fi);
      fi->extra_info->return_pc = sh_find_callers_reg (fi, PR_REGNUM);
    }
}

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR (or an expression that can be used as one).  */
CORE_ADDR
static sh_extract_struct_value_address (regbuf)
     char *regbuf;
{
  return (extract_address ((regbuf), REGISTER_RAW_SIZE (0)));
}

static CORE_ADDR
sh_frame_saved_pc (struct frame_info *frame)
{
  return ((frame)->extra_info->return_pc);
}

static CORE_ADDR
sh_frame_args_address (struct frame_info *fi)
{
  return (fi)->frame;
}

static CORE_ADDR
sh_frame_locals_address (struct frame_info *fi)
{
  return (fi)->frame;
}

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */
static void
sh_pop_frame (void)
{
  register struct frame_info *frame = get_current_frame ();
  register CORE_ADDR fp;
  register int regnum;

  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    generic_pop_dummy_frame ();
  else
    {
      fp = FRAME_FP (frame);
      FRAME_INIT_SAVED_REGS (frame);

      /* Copy regs from where they were saved in the frame */
      for (regnum = 0; regnum < NUM_REGS; regnum++)
	if (frame->saved_regs[regnum])
	  write_register (regnum, read_memory_integer (frame->saved_regs[regnum], 4));

      write_register (PC_REGNUM, frame->extra_info->return_pc);
      write_register (SP_REGNUM, fp + 4);
    }
  flush_cached_frames ();
}

/* Function: push_arguments
   Setup the function arguments for calling a function in the inferior.

   On the Hitachi SH architecture, there are four registers (R4 to R7)
   which are dedicated for passing function arguments.  Up to the first
   four arguments (depending on size) may go into these registers.
   The rest go on the stack.

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
sh_push_arguments (int nargs, struct value **args, CORE_ADDR sp,
		   int struct_return, CORE_ADDR struct_addr)
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
  sp = sp & ~3;

  /* The "struct return pointer" pseudo-argument has its own dedicated 
     register */
  if (struct_return)
    write_register (STRUCT_RETURN_REGNUM, struct_addr);

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
	  memcpy (valbuf + (4 - len),
		  (char *) VALUE_CONTENTS (args[argnum]), len);
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
	  if (argreg > ARGLAST_REGNUM || odd_sized_struct)
	    {			/* must go on the stack */
	      write_memory (sp + stack_offset, val, 4);
	      stack_offset += 4;
	    }
	  /* NOTE WELL!!!!!  This is not an "else if" clause!!!
	     That's because some *&^%$ things get passed on the stack
	     AND in the registers!   */
	  if (argreg <= ARGLAST_REGNUM)
	    {			/* there's room in a register */
	      regval = extract_address (val, REGISTER_RAW_SIZE (argreg));
	      write_register (argreg++, regval);
	    }
	  /* Store the value 4 bytes at a time.  This means that things
	     larger than 4 bytes may go partly in registers and partly
	     on the stack.  */
	  len -= REGISTER_RAW_SIZE (argreg);
	  val += REGISTER_RAW_SIZE (argreg);
	}
    }
  return sp;
}

/* Function: push_return_address (pc)
   Set up the return address for the inferior function call.
   Needed for targets where we don't actually execute a JSR/BSR instruction */

static CORE_ADDR
sh_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
{
  write_register (PR_REGNUM, CALL_DUMMY_ADDRESS ());
  return sp;
}

/* Function: fix_call_dummy
   Poke the callee function's address into the destination part of 
   the CALL_DUMMY.  The address is actually stored in a data word 
   following the actualy CALL_DUMMY instructions, which will load
   it into a register using PC-relative addressing.  This function
   expects the CALL_DUMMY to look like this:

   mov.w @(2,PC), R8
   jsr   @R8
   nop
   trap
   <destination>
 */

#if 0
void
sh_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs,
		   value_ptr *args, struct type *type, int gcc_p)
{
  *(unsigned long *) (dummy + 8) = fun;
}
#endif

static int
sh_coerce_float_to_double (struct type *formal, struct type *actual)
{
  return 1;
}

/* Find a function's return value in the appropriate registers (in
   regbuf), and copy it into valbuf.  Extract from an array REGBUF
   containing the (raw) register state a function return value of type
   TYPE, and copy that, in virtual format, into VALBUF.  */
static void
sh_extract_return_value (struct type *type, char *regbuf, char *valbuf)
{
  int len = TYPE_LENGTH (type);

  if (len <= 4)
    memcpy (valbuf, ((char *) regbuf) + 4 - len, len);
  else if (len <= 8)
    memcpy (valbuf, ((char *) regbuf) + 8 - len, len);
  else
    error ("bad size for return value");
}

/* Write into appropriate registers a function return value
   of type TYPE, given in virtual format.
   If the architecture is sh4 or sh3e, store a function's return value
   in the R0 general register or in the FP0 floating point register,
   depending on the type of the return value. In all the other cases
   the result is stored in r0. */
static void
sh_default_store_return_value (struct type *type, char *valbuf)
{
  write_register_bytes (REGISTER_BYTE (0), 
			valbuf, TYPE_LENGTH (type));
}

static void
sh3e_sh4_store_return_value (struct type *type, char *valbuf)
{
  if (TYPE_CODE (type) == TYPE_CODE_FLT) 
    write_register_bytes (REGISTER_BYTE (FP0_REGNUM), 
			  valbuf, TYPE_LENGTH (type));
  else
    write_register_bytes (REGISTER_BYTE (0), 
			  valbuf, TYPE_LENGTH (type));
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
	           (long) read_register (gdbarch_tdep (current_gdbarch)->SSR_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->SPC_REGNUM));

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
		   (long) read_register (gdbarch_tdep (current_gdbarch)->SSR_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->SPC_REGNUM));
  printf_filtered (" FPUL=%08lx FPSCR=%08lx",
		   (long) read_register (gdbarch_tdep (current_gdbarch)->FPUL_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->FPSCR_REGNUM));

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
		   (long) read_register (gdbarch_tdep (current_gdbarch)->SSR_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->SPC_REGNUM));

  printf_filtered (" DSR=%08lx", 
		   (long) read_register (gdbarch_tdep (current_gdbarch)->DSR_REGNUM));

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
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A0G_REGNUM) & 0xff,
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->M0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->X0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->Y0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->RS_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->MOD_REGNUM));
  printf_filtered ("A1G=%02lx A1=%08lx M1=%08lx X1=%08lx Y1=%08lx RE=%08lx\n",
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A1G_REGNUM) & 0xff,
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->M1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->X1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->Y1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->RE_REGNUM));
}

static void
sh4_show_regs (void)
{
  int pr = read_register (gdbarch_tdep (current_gdbarch)->FPSCR_REGNUM) & 0x80000;
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
		   (long) read_register (gdbarch_tdep (current_gdbarch)->SSR_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->SPC_REGNUM));
  printf_filtered (" FPUL=%08lx FPSCR=%08lx",
		   (long) read_register (gdbarch_tdep (current_gdbarch)->FPUL_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->FPSCR_REGNUM));

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
		   (long) read_register (gdbarch_tdep (current_gdbarch)->DSR_REGNUM));

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
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A0G_REGNUM) & 0xff,
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->M0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->X0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->Y0_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->RS_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->MOD_REGNUM));
  printf_filtered ("A1G=%02lx A1=%08lx M1=%08lx X1=%08lx Y1=%08lx RE=%08lx\n",
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A1G_REGNUM) & 0xff,
		   (long) read_register (gdbarch_tdep (current_gdbarch)->A1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->M1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->X1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->Y1_REGNUM),
		   (long) read_register (gdbarch_tdep (current_gdbarch)->RE_REGNUM));
}

void sh_show_regs_command (char *args, int from_tty)
{
  if (sh_show_regs)
    (*sh_show_regs)();
}

/* Index within `registers' of the first byte of the space for
   register N.  */
static int
sh_default_register_byte (int reg_nr)
{
  return (reg_nr * 4);
}

static int
sh_sh4_register_byte (int reg_nr)
{
  if (reg_nr >= gdbarch_tdep (current_gdbarch)->DR0_REGNUM 
      && reg_nr <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM)
    return (dr_reg_base_num (reg_nr) * 4);
  else if  (reg_nr >= gdbarch_tdep (current_gdbarch)->FV0_REGNUM 
	    && reg_nr <= gdbarch_tdep (current_gdbarch)->FV12_REGNUM)
    return (fv_reg_base_num (reg_nr) * 4);
  else
    return (reg_nr * 4);
}

/* Number of bytes of storage in the actual machine representation for
   register REG_NR.  */
static int
sh_default_register_raw_size (int reg_nr)
{
  return 4;
}

static int
sh_sh4_register_raw_size (int reg_nr)
{
  if (reg_nr >= gdbarch_tdep (current_gdbarch)->DR0_REGNUM 
      && reg_nr <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM)
    return 8;
  else if  (reg_nr >= gdbarch_tdep (current_gdbarch)->FV0_REGNUM 
	    && reg_nr <= gdbarch_tdep (current_gdbarch)->FV12_REGNUM)
    return 16;
  else
    return 4;
}

/* Number of bytes of storage in the program's representation
   for register N.  */
static int
sh_register_virtual_size (int reg_nr)
{
  return 4;
}

/* Return the GDB type object for the "standard" data type
   of data in register N.  */

static struct type *
sh_sh3e_register_virtual_type (int reg_nr)
{
  if ((reg_nr >= FP0_REGNUM
       && (reg_nr <= gdbarch_tdep (current_gdbarch)->FP15_REGNUM)) 
      || (reg_nr == gdbarch_tdep (current_gdbarch)->FPUL_REGNUM))
    return builtin_type_float;
  else
    return builtin_type_int;
}

static struct type *
sh_sh4_register_virtual_type (int reg_nr)
{
  if ((reg_nr >= FP0_REGNUM
       && (reg_nr <= gdbarch_tdep (current_gdbarch)->FP15_REGNUM)) 
      || (reg_nr == gdbarch_tdep (current_gdbarch)->FPUL_REGNUM))
    return builtin_type_float;
  else if (reg_nr >= gdbarch_tdep (current_gdbarch)->DR0_REGNUM 
	   && reg_nr <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM)
    return builtin_type_double;
  else if  (reg_nr >= gdbarch_tdep (current_gdbarch)->FV0_REGNUM 
	   && reg_nr <= gdbarch_tdep (current_gdbarch)->FV12_REGNUM)
    return sh_sh4_build_float_register_type (3);
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
sh_default_register_virtual_type (int reg_nr)
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

int
sh_sh4_register_convertible (int nr)
{
  if (TARGET_BYTE_ORDER == LITTLE_ENDIAN)
    return (gdbarch_tdep (current_gdbarch)->DR0_REGNUM <= nr
	    && nr <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM);
  else 
    return 0;
}

void
sh_sh4_register_convert_to_virtual (int regnum, struct type *type,
                                  char *from, char *to)
{
  if (regnum >= gdbarch_tdep (current_gdbarch)->DR0_REGNUM 
      && regnum <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM)
    {
      DOUBLEST val;
      floatformat_to_doublest (&floatformat_ieee_double_littlebyte_bigword, from, &val);
      store_floating(to, TYPE_LENGTH(type), val);
    }
  else
    error("sh_register_convert_to_virtual called with non DR register number");
}

void
sh_sh4_register_convert_to_raw (struct type *type, int regnum,
                              char *from, char *to)
{
  if (regnum >= gdbarch_tdep (current_gdbarch)->DR0_REGNUM 
      && regnum <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM)
    {
      DOUBLEST val = extract_floating (from, TYPE_LENGTH(type));
      floatformat_from_doublest (&floatformat_ieee_double_littlebyte_bigword, &val, to);
    }
  else
    error("sh_register_convert_to_raw called with non DR register number");
}

void
sh_fetch_pseudo_register (int reg_nr)
{
  int base_regnum, portion;

  if (!register_cached (reg_nr))
    {
      if (reg_nr >= gdbarch_tdep (current_gdbarch)->DR0_REGNUM 
	  && reg_nr <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM)
        {
	  base_regnum = dr_reg_base_num (reg_nr);

          /* Read the real regs for which this one is an alias.  */
          for (portion = 0; portion < 2; portion++)
            if (!register_cached (base_regnum + portion))
              target_fetch_registers (base_regnum + portion);
        }
      else if (reg_nr >= gdbarch_tdep (current_gdbarch)->FV0_REGNUM 
	       && reg_nr <= gdbarch_tdep (current_gdbarch)->FV12_REGNUM)
        {
	  base_regnum = fv_reg_base_num (reg_nr);

          /* Read the real regs for which this one is an alias.  */
          for (portion = 0; portion < 4; portion++)
            if (!register_cached (base_regnum + portion))
              target_fetch_registers (base_regnum + portion);

        }
      register_valid [reg_nr] = 1;
    }
}

void
sh_store_pseudo_register (int reg_nr)
{
  int base_regnum, portion;

  if (reg_nr >= gdbarch_tdep (current_gdbarch)->DR0_REGNUM
      && reg_nr <= gdbarch_tdep (current_gdbarch)->DR14_REGNUM)
    {
      base_regnum = dr_reg_base_num (reg_nr);

      /* Write the real regs for which this one is an alias.  */
      for (portion = 0; portion < 2; portion++)
	{
	  register_valid[base_regnum + portion] = 1;
	  target_store_registers (base_regnum + portion);
	}
    }
  else if (reg_nr >= gdbarch_tdep (current_gdbarch)->FV0_REGNUM
	   && reg_nr <= gdbarch_tdep (current_gdbarch)->FV12_REGNUM)
    {
      base_regnum = fv_reg_base_num (reg_nr);

      /* Write the real regs for which this one is an alias.  */
      for (portion = 0; portion < 4; portion++)
	{
	  register_valid[base_regnum + portion] = 1;
	  target_store_registers (base_regnum + portion);
	}
    }
}

static int
fv_reg_base_num (int fv_regnum)
{
  int fp_regnum;

  fp_regnum = FP0_REGNUM + 
    (fv_regnum - gdbarch_tdep (current_gdbarch)->FV0_REGNUM) * 4;
  return fp_regnum;
}

static int
dr_reg_base_num (int dr_regnum)
{
  int fp_regnum;

  fp_regnum = FP0_REGNUM + 
    (dr_regnum - gdbarch_tdep (current_gdbarch)->DR0_REGNUM) * 2;
  return fp_regnum;
}

static void
do_fv_register_info (int fv_regnum)
{
  int first_fp_reg_num = fv_reg_base_num (fv_regnum);
  printf_filtered ("fv%d\t0x%08x\t0x%08x\t0x%08x\t0x%08x\n", 
		     fv_regnum - gdbarch_tdep (current_gdbarch)->FV0_REGNUM, 
		     (int) read_register (first_fp_reg_num),
		     (int) read_register (first_fp_reg_num + 1),
		     (int) read_register (first_fp_reg_num + 2),
		     (int) read_register (first_fp_reg_num + 3));
}

static void
do_dr_register_info (int dr_regnum)
{
  int first_fp_reg_num = dr_reg_base_num (dr_regnum);

  printf_filtered ("dr%d\t0x%08x%08x\n", 
		    dr_regnum - gdbarch_tdep (current_gdbarch)->DR0_REGNUM, 
		    (int) read_register (first_fp_reg_num),
		    (int) read_register (first_fp_reg_num + 1));
}

static void
sh_do_pseudo_register (int regnum)
{
  if (regnum < NUM_REGS || regnum >= NUM_REGS + NUM_PSEUDO_REGS)
    internal_error ("Invalid pseudo register number %d\n", regnum);
  else if (regnum >= NUM_REGS && 
	   regnum < gdbarch_tdep (current_gdbarch)->FV0_REGNUM)
    do_dr_register_info (regnum);
  else if (regnum >= gdbarch_tdep (current_gdbarch)->FV0_REGNUM &&
	   regnum <= gdbarch_tdep (current_gdbarch)->FV12_REGNUM)
    do_fv_register_info (regnum);
}


static void
sh_do_fp_register (int regnum)
{				/* do values for FP (float) regs */
  char *raw_buffer;
  double flt;	/* double extracted from raw hex data */
  int inv;
  int j;

  /* Allocate space for the float. */
  raw_buffer = (char *) alloca (REGISTER_RAW_SIZE (FP0_REGNUM));

  /* Get the data in raw format.  */
  if (read_relative_register_raw_bytes (regnum, raw_buffer))
    error ("can't read register %d (%s)", regnum, REGISTER_NAME (regnum));

  /* Get the register as a number */ 
  flt = unpack_double (builtin_type_float, raw_buffer, &inv);

  /* Print the name and some spaces. */
  fputs_filtered (REGISTER_NAME (regnum), gdb_stdout);
  print_spaces_filtered (15 - strlen (REGISTER_NAME (regnum)), gdb_stdout);

  /* Print the value. */
  printf_filtered (inv ? "<invalid float>" : "%-10.9g", flt);

  /* Print the fp register as hex. */
  printf_filtered ("\t(raw 0x");
  for (j = 0; j < REGISTER_RAW_SIZE (regnum); j++)
    {
      register int idx = TARGET_BYTE_ORDER == BIG_ENDIAN ? j
	: REGISTER_RAW_SIZE (regnum) - 1 - j;
      printf_filtered ("%02x", (unsigned char) raw_buffer[idx]);
    }
  printf_filtered (")");
  printf_filtered ("\n");
}

static void
sh_do_register (int regnum)
{
  char raw_buffer[MAX_REGISTER_RAW_SIZE];

  fputs_filtered (REGISTER_NAME (regnum), gdb_stdout);
  print_spaces_filtered (15 - strlen (REGISTER_NAME (regnum)), gdb_stdout);

  /* Get the data in raw format.  */
  if (read_relative_register_raw_bytes (regnum, raw_buffer))
    printf_filtered ("*value not available*\n");
      
  val_print (REGISTER_VIRTUAL_TYPE (regnum), raw_buffer, 0, 0,
	     gdb_stdout, 'x', 1, 0, Val_pretty_default);
  printf_filtered ("\t");
  val_print (REGISTER_VIRTUAL_TYPE (regnum), raw_buffer, 0, 0,
	     gdb_stdout, 0, 1, 0, Val_pretty_default);
  printf_filtered ("\n");
}

static void
sh_print_register (int regnum)
{
  if (regnum < 0 || regnum >= NUM_REGS + NUM_PSEUDO_REGS)
    internal_error ("Invalid register number %d\n", regnum);

  else if (regnum > 0 && regnum < NUM_REGS)
    {
      if (TYPE_CODE (REGISTER_VIRTUAL_TYPE (regnum)) == TYPE_CODE_FLT)
	sh_do_fp_register (regnum);	/* FP regs */
      else
	sh_do_register (regnum);	/* All other regs */
    }

  else if (regnum < NUM_REGS + NUM_PSEUDO_REGS)
    sh_do_pseudo_register (regnum);
}

void
sh_do_registers_info (int regnum, int fpregs)
{
  if (regnum != -1)		/* do one specified register */
    {
      if (*(REGISTER_NAME (regnum)) == '\0')
	error ("Not a valid register for the current processor type");

      sh_print_register (regnum);
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

	  if (TYPE_CODE (REGISTER_VIRTUAL_TYPE (regnum)) == TYPE_CODE_FLT)
	    {
	      if (fpregs)
		{
		  /* true for "INFO ALL-REGISTERS" command */
		  sh_do_fp_register (regnum);	/* FP regs */
		  regnum ++;
		}
	      else
		regnum += (gdbarch_tdep (current_gdbarch)->FP15_REGNUM - FP0_REGNUM);	/* skip FP regs */
	    }
	  else
	    {
	      sh_do_register (regnum);	/* All other regs */
	      regnum++;
	    }
	}

      if (fpregs)
	while (regnum < NUM_REGS + NUM_PSEUDO_REGS)
	  {
	    sh_do_pseudo_register (regnum);
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

static gdbarch_init_ftype sh_gdbarch_init;

static struct gdbarch *
sh_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  static LONGEST sh_call_dummy_words[] = {0};
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  gdbarch_register_name_ftype *sh_register_name;
  gdbarch_store_return_value_ftype *sh_store_return_value;
  gdbarch_register_virtual_type_ftype *sh_register_virtual_type;

  /* Find a candidate among the list of pre-declared architectures. */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  /* None found, create a new architecture from the information
     provided. */
  tdep = XMALLOC (struct gdbarch_tdep);
  gdbarch = gdbarch_alloc (&info, tdep);

  /* Initialize the register numbers that are not common to all the
     variants to -1, if necessary thse will be overwritten in the case
     statement below. */
  tdep->FPUL_REGNUM = -1;
  tdep->FPSCR_REGNUM = -1;
  tdep->DSR_REGNUM = -1;
  tdep->FP15_REGNUM = -1;
  tdep->A0G_REGNUM = -1;
  tdep->A0_REGNUM = -1;
  tdep->A1G_REGNUM = -1;
  tdep->A1_REGNUM = -1;
  tdep->M0_REGNUM = -1;
  tdep->M1_REGNUM = -1;
  tdep->X0_REGNUM = -1;
  tdep->X1_REGNUM = -1;
  tdep->Y0_REGNUM = -1;
  tdep->Y1_REGNUM = -1;
  tdep->MOD_REGNUM = -1;
  tdep->RS_REGNUM = -1;
  tdep->RE_REGNUM = -1;
  tdep->SSR_REGNUM = -1;
  tdep->SPC_REGNUM = -1;
  tdep->DR0_REGNUM = -1;
  tdep->DR2_REGNUM = -1;
  tdep->DR4_REGNUM = -1;
  tdep->DR6_REGNUM = -1;
  tdep->DR8_REGNUM = -1;
  tdep->DR10_REGNUM = -1;
  tdep->DR12_REGNUM = -1;
  tdep->DR14_REGNUM = -1;
  tdep->FV0_REGNUM = -1;
  tdep->FV4_REGNUM = -1;
  tdep->FV8_REGNUM = -1;
  tdep->FV12_REGNUM = -1;
  set_gdbarch_fp0_regnum (gdbarch, -1);
  set_gdbarch_num_pseudo_regs (gdbarch, 0);
  set_gdbarch_max_register_raw_size (gdbarch, 4);
  set_gdbarch_max_register_virtual_size (gdbarch, 4);

  switch (info.bfd_arch_info->mach)
    {
    case bfd_mach_sh:
      sh_register_name = sh_sh_register_name;
      sh_show_regs = sh_generic_show_regs;
      sh_store_return_value = sh_default_store_return_value;
      sh_register_virtual_type = sh_default_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      set_gdbarch_register_raw_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_default_register_byte);
      break;
    case bfd_mach_sh2:
      sh_register_name = sh_sh_register_name;
      sh_show_regs = sh_generic_show_regs;
      sh_store_return_value = sh_default_store_return_value;
      sh_register_virtual_type = sh_default_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      set_gdbarch_register_raw_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_default_register_byte);
      break;      
    case bfd_mach_sh_dsp:
      sh_register_name = sh_sh_dsp_register_name;
      sh_show_regs = sh_dsp_show_regs;
      sh_store_return_value = sh_default_store_return_value;
      sh_register_virtual_type = sh_default_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      set_gdbarch_register_raw_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_default_register_byte);
      tdep->DSR_REGNUM = 24;
      tdep->A0G_REGNUM = 25;
      tdep->A0_REGNUM = 26;
      tdep->A1G_REGNUM = 27;
      tdep->A1_REGNUM = 28;
      tdep->M0_REGNUM = 29;
      tdep->M1_REGNUM = 30;
      tdep->X0_REGNUM = 31;
      tdep->X1_REGNUM = 32;
      tdep->Y0_REGNUM = 33;
      tdep->Y1_REGNUM = 34;
      tdep->MOD_REGNUM = 40;
      tdep->RS_REGNUM = 43;
      tdep->RE_REGNUM = 44;
      break;
    case bfd_mach_sh3:
      sh_register_name = sh_sh3_register_name;
      sh_show_regs = sh3_show_regs;
      sh_store_return_value = sh_default_store_return_value;
      sh_register_virtual_type = sh_default_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      set_gdbarch_register_raw_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_default_register_byte);
      tdep->SSR_REGNUM = 41;
      tdep->SPC_REGNUM = 42;
      break;
    case bfd_mach_sh3e:
      sh_register_name = sh_sh3e_register_name;
      sh_show_regs = sh3e_show_regs;
      sh_store_return_value = sh3e_sh4_store_return_value;
      sh_register_virtual_type = sh_sh3e_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_fp_frame_init_saved_regs);
      set_gdbarch_register_raw_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_default_register_byte);
      set_gdbarch_fp0_regnum (gdbarch, 25);
      tdep->FPUL_REGNUM = 23;
      tdep->FPSCR_REGNUM = 24;
      tdep->FP15_REGNUM = 40;
      tdep->SSR_REGNUM = 41;
      tdep->SPC_REGNUM = 42;
      break;
    case bfd_mach_sh3_dsp:
      sh_register_name = sh_sh3_dsp_register_name;
      sh_show_regs = sh3_dsp_show_regs;
      sh_store_return_value = sh_default_store_return_value;
      sh_register_virtual_type = sh_default_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      set_gdbarch_register_raw_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_default_register_byte);
      tdep->DSR_REGNUM = 24;
      tdep->A0G_REGNUM = 25;
      tdep->A0_REGNUM = 26;
      tdep->A1G_REGNUM = 27;
      tdep->A1_REGNUM = 28;
      tdep->M0_REGNUM = 29;
      tdep->M1_REGNUM = 30;
      tdep->X0_REGNUM = 31;
      tdep->X1_REGNUM = 32;
      tdep->Y0_REGNUM = 33;
      tdep->Y1_REGNUM = 34;
      tdep->MOD_REGNUM = 40;
      tdep->RS_REGNUM = 43;
      tdep->RE_REGNUM = 44;
      tdep->SSR_REGNUM = 41;
      tdep->SPC_REGNUM = 42;
      break;
    case bfd_mach_sh4:
      sh_register_name = sh_sh4_register_name;
      sh_show_regs = sh4_show_regs;
      sh_store_return_value = sh3e_sh4_store_return_value;
      sh_register_virtual_type = sh_sh4_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_fp_frame_init_saved_regs);
      set_gdbarch_fp0_regnum (gdbarch, 25);
      set_gdbarch_register_raw_size (gdbarch, sh_sh4_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_sh4_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_sh4_register_byte);
      set_gdbarch_num_pseudo_regs (gdbarch, 12);
      set_gdbarch_max_register_raw_size (gdbarch, 4 * 4);
      set_gdbarch_max_register_virtual_size (gdbarch, 4 * 4);
      set_gdbarch_register_convert_to_raw (gdbarch, sh_sh4_register_convert_to_raw);
      set_gdbarch_register_convert_to_virtual (gdbarch, sh_sh4_register_convert_to_virtual);
      set_gdbarch_register_convertible (gdbarch, sh_sh4_register_convertible);
      tdep->FPUL_REGNUM = 23;
      tdep->FPSCR_REGNUM = 24;
      tdep->FP15_REGNUM = 40;
      tdep->SSR_REGNUM = 41;
      tdep->SPC_REGNUM = 42;
      tdep->DR0_REGNUM = 59;
      tdep->DR2_REGNUM = 60;
      tdep->DR4_REGNUM = 61;
      tdep->DR6_REGNUM = 62;
      tdep->DR8_REGNUM = 63;
      tdep->DR10_REGNUM = 64;
      tdep->DR12_REGNUM = 65;
      tdep->DR14_REGNUM = 66;
      tdep->FV0_REGNUM = 67;
      tdep->FV4_REGNUM = 68;
      tdep->FV8_REGNUM = 69;
      tdep->FV12_REGNUM = 70;
      break;
    default:
      sh_register_name = sh_generic_register_name;
      sh_show_regs = sh_generic_show_regs;
      sh_store_return_value = sh_default_store_return_value;
      sh_register_virtual_type = sh_default_register_virtual_type;
      set_gdbarch_frame_init_saved_regs (gdbarch, sh_nofp_frame_init_saved_regs);
      set_gdbarch_register_raw_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_virtual_size (gdbarch, sh_default_register_raw_size);
      set_gdbarch_register_byte (gdbarch, sh_default_register_byte);
      break;
    }

  set_gdbarch_read_pc (gdbarch, generic_target_read_pc);
  set_gdbarch_write_pc (gdbarch, generic_target_write_pc);
  set_gdbarch_read_fp (gdbarch, generic_target_read_fp);
  set_gdbarch_write_fp (gdbarch, generic_target_write_fp);
  set_gdbarch_read_sp (gdbarch, generic_target_read_sp);
  set_gdbarch_write_sp (gdbarch, generic_target_write_sp);

  set_gdbarch_num_regs (gdbarch, 59);
  set_gdbarch_sp_regnum (gdbarch, 15);
  set_gdbarch_fp_regnum (gdbarch, 14);
  set_gdbarch_pc_regnum (gdbarch, 16);
  set_gdbarch_register_name (gdbarch, sh_register_name);
  set_gdbarch_register_size (gdbarch, 4);
  set_gdbarch_register_bytes (gdbarch, NUM_REGS * 4);
  set_gdbarch_register_virtual_type (gdbarch, sh_register_virtual_type);

  set_gdbarch_ptr_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_short_bit (gdbarch, 2 * TARGET_CHAR_BIT);
  set_gdbarch_int_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_long_long_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_float_bit (gdbarch, 4 * TARGET_CHAR_BIT);
  set_gdbarch_double_bit (gdbarch, 8 * TARGET_CHAR_BIT);
  set_gdbarch_long_double_bit (gdbarch, 16 * TARGET_CHAR_BIT);

  set_gdbarch_use_generic_dummy_frames (gdbarch, 1);
  set_gdbarch_call_dummy_length (gdbarch, 0);
  set_gdbarch_call_dummy_location (gdbarch, AT_ENTRY_POINT);
  set_gdbarch_call_dummy_address (gdbarch, entry_point_address);
  set_gdbarch_call_dummy_breakpoint_offset_p (gdbarch, 1); /*???*/
  set_gdbarch_call_dummy_breakpoint_offset (gdbarch, 0);
  set_gdbarch_call_dummy_start_offset (gdbarch, 0);
  set_gdbarch_pc_in_call_dummy (gdbarch, generic_pc_in_call_dummy);
  set_gdbarch_call_dummy_words (gdbarch, sh_call_dummy_words);
  set_gdbarch_sizeof_call_dummy_words (gdbarch, sizeof (sh_call_dummy_words));
  set_gdbarch_call_dummy_p (gdbarch, 1);
  set_gdbarch_call_dummy_stack_adjust_p (gdbarch, 0);
  set_gdbarch_get_saved_register (gdbarch, generic_get_saved_register);
  set_gdbarch_fix_call_dummy (gdbarch, generic_fix_call_dummy);
  set_gdbarch_coerce_float_to_double (gdbarch, 
				      sh_coerce_float_to_double);

  set_gdbarch_extract_return_value (gdbarch, sh_extract_return_value);
  set_gdbarch_push_arguments (gdbarch, sh_push_arguments);
  set_gdbarch_push_dummy_frame (gdbarch, generic_push_dummy_frame);
  set_gdbarch_push_return_address (gdbarch, sh_push_return_address);

  set_gdbarch_store_struct_return (gdbarch, sh_store_struct_return);
  set_gdbarch_store_return_value (gdbarch, sh_store_return_value);
  set_gdbarch_extract_struct_value_address (gdbarch, sh_extract_struct_value_address);
  set_gdbarch_use_struct_convention (gdbarch, sh_use_struct_convention);
  set_gdbarch_init_extra_frame_info (gdbarch, sh_init_extra_frame_info);
  set_gdbarch_pop_frame (gdbarch, sh_pop_frame);
  set_gdbarch_skip_prologue (gdbarch, sh_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_decr_pc_after_break (gdbarch, 0);
  set_gdbarch_function_start_offset (gdbarch, 0);
  set_gdbarch_breakpoint_from_pc (gdbarch, sh_breakpoint_from_pc);

  set_gdbarch_fetch_pseudo_register (gdbarch, sh_fetch_pseudo_register);
  set_gdbarch_store_pseudo_register (gdbarch, sh_store_pseudo_register);
  set_gdbarch_frame_args_skip (gdbarch, 0);
  set_gdbarch_frameless_function_invocation (gdbarch, frameless_look_for_prologue);
  set_gdbarch_frame_chain (gdbarch, sh_frame_chain);
  set_gdbarch_frame_chain_valid (gdbarch, generic_file_frame_chain_valid);
  set_gdbarch_frame_saved_pc (gdbarch, sh_frame_saved_pc);
  set_gdbarch_frame_args_address (gdbarch, sh_frame_args_address);
  set_gdbarch_frame_locals_address (gdbarch, sh_frame_locals_address);
  set_gdbarch_saved_pc_after_call (gdbarch, sh_saved_pc_after_call);
  set_gdbarch_frame_num_args (gdbarch, frame_num_args_unknown);
  set_gdbarch_believe_pcc_promotion (gdbarch, 1);
  set_gdbarch_ieee_float (gdbarch, 1);

  return gdbarch;
}

void
_initialize_sh_tdep (void)
{
  struct cmd_list_element *c;
  
  register_gdbarch_init (bfd_arch_sh, sh_gdbarch_init);
  tm_print_insn = gdb_print_insn_sh;

  add_com ("regs", class_vars, sh_show_regs_command, "Print all registers");
}
