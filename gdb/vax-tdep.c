/* Print VAX instructions for GDB, the GNU debugger.

   Copyright 1986, 1989, 1991, 1992, 1995, 1996, 1998, 1999, 2000,
   2002, 2003, 2004 Free Software Foundation, Inc.

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

#include "defs.h"
#include "symtab.h"
#include "opcode/vax.h"
#include "gdbcore.h"
#include "inferior.h"
#include "regcache.h"
#include "frame.h"
#include "frame-base.h"
#include "frame-unwind.h"
#include "trad-frame.h"
#include "value.h"
#include "arch-utils.h"
#include "gdb_string.h"
#include "osabi.h"
#include "dis-asm.h"

#include "vax-tdep.h"

/* Return the name of register REGNUM.  */

static const char *
vax_register_name (int regnum)
{
  static char *register_names[] =
  {
    "r0", "r1", "r2",  "r3",  "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "ap", "fp", "sp", "pc",
    "ps",
  };

  if (regnum >= 0 && regnum < ARRAY_SIZE (register_names))
    return register_names[regnum];

  return NULL;
}

/* Return the GDB type object for the "standard" data type of data in
   register REGNUM. */

static struct type *
vax_register_type (struct gdbarch *gdbarch, int regnum)
{
  return builtin_type_int;
}


static void
vax_push_dummy_frame (void)
{
  CORE_ADDR sp = read_register (SP_REGNUM);
  int regnum;

  sp = push_word (sp, 0);	/* arglist */
  for (regnum = 11; regnum >= 0; regnum--)
    sp = push_word (sp, read_register (regnum));
  sp = push_word (sp, read_register (PC_REGNUM));
  sp = push_word (sp, read_register (VAX_FP_REGNUM));
  sp = push_word (sp, read_register (VAX_AP_REGNUM));
  sp = push_word (sp, (read_register (PS_REGNUM) & 0xffef) + 0x2fff0000);
  sp = push_word (sp, 0);
  write_register (SP_REGNUM, sp);
  write_register (VAX_FP_REGNUM, sp);
  write_register (VAX_AP_REGNUM, sp + (17 * 4));
}

static void
vax_pop_frame (void)
{
  CORE_ADDR fp = read_register (VAX_FP_REGNUM);
  int regnum;
  int regmask = read_memory_integer (fp + 4, 4);

  write_register (PS_REGNUM,
		  (regmask & 0xffff)
		  | (read_register (PS_REGNUM) & 0xffff0000));
  write_register (PC_REGNUM, read_memory_integer (fp + 16, 4));
  write_register (VAX_FP_REGNUM, read_memory_integer (fp + 12, 4));
  write_register (VAX_AP_REGNUM, read_memory_integer (fp + 8, 4));
  fp += 16;
  for (regnum = 0; regnum < 12; regnum++)
    if (regmask & (0x10000 << regnum))
      write_register (regnum, read_memory_integer (fp += 4, 4));
  fp = fp + 4 + ((regmask >> 30) & 3);
  if (regmask & 0x20000000)
    {
      regnum = read_memory_integer (fp, 4);
      fp += (regnum + 1) * 4;
    }
  write_register (SP_REGNUM, fp);
  flush_cached_frames ();
}

/* The VAX call dummy sequence:

	calls #69, @#32323232
	bpt

   It is 8 bytes long.  The address and argc are patched by
   vax_fix_call_dummy().  */
static LONGEST vax_call_dummy_words[] = { 0x329f69fb, 0x03323232 };
static int sizeof_vax_call_dummy_words = sizeof(vax_call_dummy_words);

static void
vax_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun, int nargs,
                    struct value **args, struct type *type, int gcc_p)
{
  dummy[1] = nargs;
  store_unsigned_integer (dummy + 3, 4, fun);
}

static void
vax_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
{
  write_register (1, addr);
}

static void
vax_extract_return_value (struct type *valtype, char *regbuf, char *valbuf)
{
  memcpy (valbuf, regbuf + DEPRECATED_REGISTER_BYTE (0), TYPE_LENGTH (valtype));
}

static void
vax_store_return_value (struct type *valtype, char *valbuf)
{
  deprecated_write_register_bytes (0, valbuf, TYPE_LENGTH (valtype));
}


/* Use the program counter to determine the contents and size of a
   breakpoint instruction.  Return a pointer to a string of bytes that
   encode a breakpoint instruction, store the length of the string in
   *LEN and optionally adjust *PC to point to the correct memory
   location for inserting the breakpoint.  */
   
static const unsigned char *
vax_breakpoint_from_pc (CORE_ADDR *pc, int *len)
{
  static unsigned char break_insn[] = { 3 };

  *len = sizeof (break_insn);
  return break_insn;
}

/* Advance PC across any function entry prologue instructions
   to reach some "real" code.  */

static CORE_ADDR
vax_skip_prologue (CORE_ADDR pc)
{
  int op = (unsigned char) read_memory_integer (pc, 1);
  if (op == 0x11)
    pc += 2;			/* skip brb */
  if (op == 0x31)
    pc += 3;			/* skip brw */
  if (op == 0xC2
      && ((unsigned char) read_memory_integer (pc + 2, 1)) == 0x5E)
    pc += 3;			/* skip subl2 */
  if (op == 0x9E
      && ((unsigned char) read_memory_integer (pc + 1, 1)) == 0xAE
      && ((unsigned char) read_memory_integer (pc + 3, 1)) == 0x5E)
    pc += 4;			/* skip movab */
  if (op == 0x9E
      && ((unsigned char) read_memory_integer (pc + 1, 1)) == 0xCE
      && ((unsigned char) read_memory_integer (pc + 4, 1)) == 0x5E)
    pc += 5;			/* skip movab */
  if (op == 0x9E
      && ((unsigned char) read_memory_integer (pc + 1, 1)) == 0xEE
      && ((unsigned char) read_memory_integer (pc + 6, 1)) == 0x5E)
    pc += 7;			/* skip movab */
  return pc;
}


/* Unwinding the stack is relatively easy since the VAX has a
   dedicated frame pointer, and frames are set up automatically as the
   result of a function call.  Most of the relevant information can be
   inferred from the documentation of the Procedure Call Instructions
   in the VAX MACRO and Instruction Set Reference Manual.  */

struct vax_frame_cache
{
  /* Base address.  */
  CORE_ADDR base;

  /* Table of saved registers.  */
  struct trad_frame_saved_reg *saved_regs;
};

struct vax_frame_cache *
vax_frame_cache (struct frame_info *next_frame, void **this_cache)
{
  struct vax_frame_cache *cache;
  CORE_ADDR addr;
  ULONGEST mask;
  int regnum;

  if (*this_cache)
    return *this_cache;

  /* Allocate a new cache.  */
  cache = FRAME_OBSTACK_ZALLOC (struct vax_frame_cache);
  cache->saved_regs = trad_frame_alloc_saved_regs (next_frame);

  /* The frame pointer is used as the base for the frame.  */
  cache->base = frame_unwind_register_unsigned (next_frame, VAX_FP_REGNUM);
  if (cache->base == 0)
    return cache;

  /* The register save mask and control bits determine the layout of
     the stack frame.  */
  mask = get_frame_memory_unsigned (next_frame, cache->base + 4, 4) >> 16;

  /* These are always saved.  */
  cache->saved_regs[VAX_PC_REGNUM].addr = cache->base + 16;
  cache->saved_regs[VAX_FP_REGNUM].addr = cache->base + 12;
  cache->saved_regs[VAX_AP_REGNUM].addr = cache->base + 8;
  cache->saved_regs[VAX_PS_REGNUM].addr = cache->base + 4;

  /* Scan the register save mask and record the location of the saved
     registers.  */
  addr = cache->base + 20;
  for (regnum = 0; regnum < VAX_AP_REGNUM; regnum++)
    {
      if (mask & (1 << regnum))
	{
	  cache->saved_regs[regnum].addr = addr;
	  addr += 4;
	}
    }

  /* The CALLS/CALLG flag determines whether this frame has a General
     Argument List or a Stack Argument List.  */
  if (mask & (1 << 13))
    {
      ULONGEST numarg;

      /* This is a procedure with Stack Argument List.  Adjust the
         stack address for the arguments thet were pushed onto the
         stack.  The return instruction will automatically pop the
         arguments from the stack.  */
      numarg = get_frame_memory_unsigned (next_frame, addr, 1);
      addr += 4 + numarg * 4;
    }

  /* Bits 1:0 of the stack pointer were saved in the control bits.  */
  trad_frame_set_value (cache->saved_regs, VAX_SP_REGNUM, addr + (mask >> 14));

  return cache;
}

static void
vax_frame_this_id (struct frame_info *next_frame, void **this_cache,
		   struct frame_id *this_id)
{
  struct vax_frame_cache *cache = vax_frame_cache (next_frame, this_cache);

  /* This marks the outermost frame.  */
  if (cache->base == 0)
    return;

  (*this_id) = frame_id_build (cache->base, frame_pc_unwind (next_frame));
}

static void
vax_frame_prev_register (struct frame_info *next_frame, void **this_cache,
			 int regnum, int *optimizedp,
			 enum lval_type *lvalp, CORE_ADDR *addrp,
			 int *realnump, void *valuep)
{
  struct vax_frame_cache *cache = vax_frame_cache (next_frame, this_cache);

  trad_frame_prev_register (next_frame, cache->saved_regs, regnum,
			    optimizedp, lvalp, addrp, realnump, valuep);
}

static const struct frame_unwind vax_frame_unwind =
{
  NORMAL_FRAME,
  vax_frame_this_id,
  vax_frame_prev_register
};

static const struct frame_unwind *
vax_frame_sniffer (struct frame_info *next_frame)
{
  return &vax_frame_unwind;
}


static CORE_ADDR
vax_frame_base_address (struct frame_info *next_frame, void **this_cache)
{
  struct vax_frame_cache *cache = vax_frame_cache (next_frame, this_cache);

  return cache->base;
}

static CORE_ADDR
vax_frame_args_address (struct frame_info *next_frame, void **this_cache)
{
  return frame_unwind_register_unsigned (next_frame, VAX_AP_REGNUM);
}

static const struct frame_base vax_frame_base =
{
  &vax_frame_unwind,
  vax_frame_base_address,
  vax_frame_base_address,
  vax_frame_args_address
};

/* Return number of arguments for FRAME.  */

static int
vax_frame_num_args (struct frame_info *frame)
{
  CORE_ADDR args;

  /* Assume that the argument pointer for the outermost frame is
     hosed, as is the case on NetBSD/vax ELF.  */
  if (get_frame_base (frame) == 0)
    return 0;

  args = get_frame_register_unsigned (frame, VAX_AP_REGNUM);
  return get_frame_memory_unsigned (frame, args, 1);
}

static CORE_ADDR
vax_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, VAX_PC_REGNUM);
}


/* Initialize the current architecture based on INFO.  If possible, re-use an
   architecture from ARCHES, which is a list of architectures already created
   during this debugging session.

   Called e.g. at program startup, when reading a core file, and when reading
   a binary file.  */

static struct gdbarch *
vax_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;

  /* If there is already a candidate, use it.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  gdbarch = gdbarch_alloc (&info, NULL);

  /* Register info */
  set_gdbarch_num_regs (gdbarch, VAX_NUM_REGS);
  set_gdbarch_register_name (gdbarch, vax_register_name);
  set_gdbarch_register_type (gdbarch, vax_register_type);
  set_gdbarch_sp_regnum (gdbarch, VAX_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, VAX_PC_REGNUM);
  set_gdbarch_ps_regnum (gdbarch, VAX_PS_REGNUM);

  /* Frame and stack info */
  set_gdbarch_skip_prologue (gdbarch, vax_skip_prologue);
  set_gdbarch_frame_num_args (gdbarch, vax_frame_num_args);

  set_gdbarch_frame_args_skip (gdbarch, 4);

  /* Stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  /* Return value info */
  set_gdbarch_deprecated_store_struct_return (gdbarch, vax_store_struct_return);
  set_gdbarch_deprecated_extract_return_value (gdbarch, vax_extract_return_value);
  set_gdbarch_deprecated_store_return_value (gdbarch, vax_store_return_value);

  /* Call dummy info */
  set_gdbarch_deprecated_push_dummy_frame (gdbarch, vax_push_dummy_frame);
  set_gdbarch_deprecated_pop_frame (gdbarch, vax_pop_frame);
  set_gdbarch_call_dummy_location (gdbarch, ON_STACK);
  set_gdbarch_deprecated_call_dummy_words (gdbarch, vax_call_dummy_words);
  set_gdbarch_deprecated_sizeof_call_dummy_words (gdbarch, sizeof_vax_call_dummy_words);
  set_gdbarch_deprecated_fix_call_dummy (gdbarch, vax_fix_call_dummy);
  set_gdbarch_deprecated_call_dummy_breakpoint_offset (gdbarch, 7);
  set_gdbarch_deprecated_use_generic_dummy_frames (gdbarch, 0);
  set_gdbarch_deprecated_pc_in_call_dummy (gdbarch, deprecated_pc_in_call_dummy_on_stack);

  /* Breakpoint info */
  set_gdbarch_breakpoint_from_pc (gdbarch, vax_breakpoint_from_pc);

  /* Misc info */
  set_gdbarch_function_start_offset (gdbarch, 2);
  set_gdbarch_believe_pcc_promotion (gdbarch, 1);

  /* Should be using push_dummy_call.  */
  set_gdbarch_deprecated_dummy_write_sp (gdbarch, deprecated_write_sp);

  set_gdbarch_unwind_pc (gdbarch, vax_unwind_pc);

  frame_base_set_default (gdbarch, &vax_frame_base);

  /* Hook in ABI-specific overrides, if they have been registered.  */
  gdbarch_init_osabi (info, gdbarch);

  frame_unwind_append_sniffer (gdbarch, vax_frame_sniffer);

  set_gdbarch_print_insn (gdbarch, print_insn_vax);

  return (gdbarch);
}

extern initialize_file_ftype _initialize_vax_tdep; /* -Wmissing-prototypes */

void
_initialize_vax_tdep (void)
{
  gdbarch_register (bfd_arch_vax, vax_gdbarch_init, NULL);
}
