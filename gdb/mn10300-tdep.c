/* Target-dependent code for the Matsushita MN10300 for GDB, the GNU debugger.

   Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004 Free
   Software Foundation, Inc.

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

/* MVS Notes:

   To get from 1.1 to 1.2, add:
   use_struct_convention
   store_return_value
   extract_return_value
   extract_struct_value_address
   
   Make sure to use regcache.  */

/* MVS Notes:

   Apparently cannot run without a stub placeholder for unwind_dummy_id.  
*/

/* MVS Notes:

   To get from 1.2 to 1.3, add:
   read_pc, write_pc
   frame_unwind_init
   struct mn10300_unwind_cache
   unwind_pc
   unwind_dummy_id
   frame_this_id
   frame_prev_register
   frame_sniffer (struct mn10300_frame_unwind)
*/

#include "defs.h"
#include "arch-utils.h"
#include "dis-asm.h"
#include "gdbtypes.h"
#include "regcache.h"
#include "gdb_string.h"
#include "gdb_assert.h"
#include "gdbcore.h"	/* for write_memory_unsigned_integer */
#include "value.h"
#include "gdbtypes.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "symtab.h"
#include "dwarf2-frame.h"
#include "regcache.h"

#include "mn10300-tdep.h"


/* Compute the alignment required by a type.  */

static int
mn10300_type_align (struct type *type)
{
  int i, align = 1;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_INT:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_SET:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_BOOL:
    case TYPE_CODE_FLT:
    case TYPE_CODE_PTR:
    case TYPE_CODE_REF:
      return TYPE_LENGTH (type);

    case TYPE_CODE_COMPLEX:
      return TYPE_LENGTH (type) / 2;

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      for (i = 0; i < TYPE_NFIELDS (type); i++)
	{
	  int falign = mn10300_type_align (TYPE_FIELD_TYPE (type, i));
	  while (align < falign)
	    align <<= 1;
	}
      return align;

    case TYPE_CODE_ARRAY:
      /* HACK!  Structures containing arrays, even small ones, are not
	 elligible for returning in registers.  */
      return 256;

    case TYPE_CODE_TYPEDEF:
      return mn10300_type_align (check_typedef (type));

    default:
      internal_error (__FILE__, __LINE__, _("bad switch"));
    }
}

/* MVS note this is deprecated.  */
/* Should call_function allocate stack space for a struct return?  */
/* gcc_p unused */
static int
mn10300_use_struct_convention (int gcc_p, struct type *type)
{
  /* Structures bigger than a pair of words can't be returned in
     registers.  */
  if (TYPE_LENGTH (type) > 8)
    return 1;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      /* Structures with a single field are handled as the field
	 itself.  */
      if (TYPE_NFIELDS (type) == 1)
	return mn10300_use_struct_convention (gcc_p, 
					      TYPE_FIELD_TYPE (type, 0));

      /* Structures with word or double-word size are passed in memory, as
	 long as they require at least word alignment.  */
      if (mn10300_type_align (type) >= 4)
	return 0;

      return 1;

      /* Arrays are addressable, so they're never returned in
	 registers.  This condition can only hold when the array is
	 the only field of a struct or union.  */
    case TYPE_CODE_ARRAY:
      return 1;

    case TYPE_CODE_TYPEDEF:
      return mn10300_use_struct_convention (gcc_p, check_typedef (type));

    default:
      return 0;
    }
}

/* MVS note this is deprecated.  */
static void
mn10300_store_return_value (struct type *type,
			    struct regcache *regcache, const void *valbuf)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  int len = TYPE_LENGTH (type);
  int reg, regsz;
  
  if (TYPE_CODE (type) == TYPE_CODE_PTR)
    reg = 4;
  else
    reg = 0;

  regsz = register_size (gdbarch, reg);

  if (len <= regsz)
    regcache_raw_write_part (regcache, reg, 0, len, valbuf);
  else if (len <= 2 * regsz)
    {
      regcache_raw_write (regcache, reg, valbuf);
      gdb_assert (regsz == register_size (gdbarch, reg + 1));
      regcache_raw_write_part (regcache, reg+1, 0,
			       len - regsz, (char *) valbuf + regsz);
    }
  else
    internal_error (__FILE__, __LINE__,
		    _("Cannot store return value %d bytes long."), len);
}

/* MVS note deprecated.  */
static void
mn10300_extract_return_value (struct type *type,
			      struct regcache *regcache, void *valbuf)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  char buf[MAX_REGISTER_SIZE];
  int len = TYPE_LENGTH (type);
  int reg, regsz;

  if (TYPE_CODE (type) == TYPE_CODE_PTR)
    reg = 4;
  else
    reg = 0;

  regsz = register_size (gdbarch, reg);
  if (len <= regsz)
    {
      regcache_raw_read (regcache, reg, buf);
      memcpy (valbuf, buf, len);
    }
  else if (len <= 2 * regsz)
    {
      regcache_raw_read (regcache, reg, buf);
      memcpy (valbuf, buf, regsz);
      gdb_assert (regsz == register_size (gdbarch, reg + 1));
      regcache_raw_read (regcache, reg + 1, buf);
      memcpy ((char *) valbuf + regsz, buf, len - regsz);
    }
  else
    internal_error (__FILE__, __LINE__,
		    _("Cannot extract return value %d bytes long."), len);
}

static char *
register_name (int reg, char **regs, long sizeof_regs)
{
  if (reg < 0 || reg >= sizeof_regs / sizeof (regs[0]))
    return NULL;
  else
    return regs[reg];
}

static const char *
mn10300_generic_register_name (int reg)
{
  static char *regs[] =
  { "d0", "d1", "d2", "d3", "a0", "a1", "a2", "a3",
    "sp", "pc", "mdr", "psw", "lir", "lar", "", "",
    "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "fp"
  };
  return register_name (reg, regs, sizeof regs);
}


static const char *
am33_register_name (int reg)
{
  static char *regs[] =
  { "d0", "d1", "d2", "d3", "a0", "a1", "a2", "a3",
    "sp", "pc", "mdr", "psw", "lir", "lar", "",
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "ssp", "msp", "usp", "mcrh", "mcrl", "mcvf", "", "", ""
  };
  return register_name (reg, regs, sizeof regs);
}


static struct type *
mn10300_register_type (struct gdbarch *gdbarch, int reg)
{
  return builtin_type_int;
}

static CORE_ADDR
mn10300_read_pc (ptid_t ptid)
{
  return read_register_pid (E_PC_REGNUM, ptid);
}

static void
mn10300_write_pc (CORE_ADDR val, ptid_t ptid)
{
  return write_register_pid (E_PC_REGNUM, val, ptid);
}

/* The breakpoint instruction must be the same size as the smallest
   instruction in the instruction set.

   The Matsushita mn10x00 processors have single byte instructions
   so we need a single byte breakpoint.  Matsushita hasn't defined
   one, so we defined it ourselves.  */

const static unsigned char *
mn10300_breakpoint_from_pc (CORE_ADDR *bp_addr, int *bp_size)
{
  static char breakpoint[] = {0xff};
  *bp_size = 1;
  return breakpoint;
}

/* Function: skip_prologue
   Return the address of the first inst past the prologue of the function.  */

static CORE_ADDR
mn10300_skip_prologue (CORE_ADDR pc)
{
  /* FIXME: not implemented.  */
  /* First approximation, try simply using skip_prologue_using_sal.  */
  return skip_prologue_using_sal (pc);
}

/* Simple frame_unwind_cache.  
   This finds the "extra info" for the frame.  */
struct trad_frame_cache *
mn10300_frame_unwind_cache (struct frame_info *next_frame,
			    void **this_prologue_cache)
{
  struct trad_frame_cache *cache;
  CORE_ADDR pc;

  if (*this_prologue_cache)
    return (*this_prologue_cache);

  cache = trad_frame_cache_zalloc (next_frame);
  pc = gdbarch_unwind_pc (current_gdbarch, next_frame);
  mn10300_analyze_prologue (next_frame, (void **) &cache, pc);

  trad_frame_set_id (cache, 
		     frame_id_build (trad_frame_get_this_base (cache), pc));

  (*this_prologue_cache) = cache;
  return cache;
}

/* Here is a dummy implementation.  */
static struct frame_id
mn10300_unwind_dummy_id (struct gdbarch *gdbarch,
			 struct frame_info *next_frame)
{
  return frame_id_build (frame_sp_unwind (next_frame), 
			 frame_pc_unwind (next_frame));
}

/* Trad frame implementation.  */
static void
mn10300_frame_this_id (struct frame_info *next_frame,
		       void **this_prologue_cache,
		       struct frame_id *this_id)
{
  struct trad_frame_cache *cache = 
    mn10300_frame_unwind_cache (next_frame, this_prologue_cache);

  trad_frame_get_id (cache, this_id);
}

static void
mn10300_frame_prev_register (struct frame_info *next_frame,
			     void **this_prologue_cache,
			     int regnum, int *optimizedp,
			     enum lval_type *lvalp, CORE_ADDR *addrp,
			     int *realnump, void *bufferp)
{
  struct trad_frame_cache *cache =
    mn10300_frame_unwind_cache (next_frame, this_prologue_cache);

  trad_frame_get_register (cache, next_frame, regnum, optimizedp, 
			   lvalp, addrp, realnump, bufferp);
  /* Or...
  trad_frame_get_prev_register (next_frame, cache->prev_regs, regnum, 
			   optimizedp, lvalp, addrp, realnump, bufferp);
  */
}

static const struct frame_unwind mn10300_frame_unwind = {
  NORMAL_FRAME,
  mn10300_frame_this_id, 
  mn10300_frame_prev_register
};

static CORE_ADDR
mn10300_frame_base_address (struct frame_info *next_frame,
			    void **this_prologue_cache)
{
  struct trad_frame_cache *cache = 
    mn10300_frame_unwind_cache (next_frame, this_prologue_cache);

  return trad_frame_get_this_base (cache);
}

static const struct frame_unwind *
mn10300_frame_sniffer (struct frame_info *next_frame)
{
  return &mn10300_frame_unwind;
}

static const struct frame_base mn10300_frame_base = {
  &mn10300_frame_unwind, 
  mn10300_frame_base_address, 
  mn10300_frame_base_address,
  mn10300_frame_base_address
};

static CORE_ADDR
mn10300_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST pc;

  frame_unwind_unsigned_register (next_frame, E_PC_REGNUM, &pc);
  return pc;
}

static CORE_ADDR
mn10300_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  ULONGEST sp;

  frame_unwind_unsigned_register (next_frame, E_SP_REGNUM, &sp);
  return sp;
}

static void
mn10300_frame_unwind_init (struct gdbarch *gdbarch)
{
  frame_unwind_append_sniffer (gdbarch, dwarf2_frame_sniffer);
  frame_unwind_append_sniffer (gdbarch, mn10300_frame_sniffer);
  frame_base_set_default (gdbarch, &mn10300_frame_base);
  set_gdbarch_unwind_dummy_id (gdbarch, mn10300_unwind_dummy_id);
  set_gdbarch_unwind_pc (gdbarch, mn10300_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, mn10300_unwind_sp);
}

/* Function: push_dummy_call
 *
 * Set up machine state for a target call, including
 * function arguments, stack, return address, etc.
 *
 */

static CORE_ADDR
mn10300_push_dummy_call (struct gdbarch *gdbarch, 
			 struct value *target_func,
			 struct regcache *regcache,
			 CORE_ADDR bp_addr, 
			 int nargs, struct value **args,
			 CORE_ADDR sp, 
			 int struct_return,
			 CORE_ADDR struct_addr)
{
  const int push_size = register_size (gdbarch, E_PC_REGNUM);
  int regs_used = struct_return ? 1 : 0;
  int len, arg_len; 
  int stack_offset = 0;
  int argnum;
  char *val;

  /* FIXME temp, don't handle struct args at all.  */
  if (struct_return)
    error ("Target doesn't handle struct return");

  /* This should be a nop, but align the stack just in case something
     went wrong.  Stacks are four byte aligned on the mn10300.  */
  sp &= ~3;

  /* Now make space on the stack for the args.

     XXX This doesn't appear to handle pass-by-invisible reference
     arguments.  */
  for (len = 0, argnum = 0; argnum < nargs; argnum++)
    {
      arg_len = (TYPE_LENGTH (value_type (args[argnum])) + 3) & ~3;
      if (TYPE_CODE (value_type (args[argnum])) == TYPE_CODE_STRUCT)
	error ("Target does not handle struct args");
      while (regs_used < 2 && arg_len > 0)
	{
	  regs_used++;
	  arg_len -= push_size;
	}
      len += arg_len;
    }

  /* Allocate stack space.  */
  sp -= len;

  regs_used = struct_return ? 1 : 0;
  /* Push all arguments onto the stack. */
  for (argnum = 0; argnum < nargs; argnum++)
    {
      /* FIXME what about structs?  */
      arg_len = TYPE_LENGTH (value_type (*args));
      val = (char *) value_contents (*args);

      while (regs_used < 2 && arg_len > 0)
	{
	  write_register (regs_used, extract_unsigned_integer (val, 
							       push_size));
	  val += push_size;
	  arg_len -= push_size;
	  regs_used++;
	}

      while (arg_len > 0)
	{
	  write_memory (sp + stack_offset, val, push_size);
	  arg_len -= push_size;
	  val += push_size;
	  stack_offset += push_size;
	}

      args++;
    }

  /* Make space for the flushback area.  */
  sp -= 8;

  /* Push the return address that contains the magic breakpoint.  */
  sp -= 4;
  write_memory_unsigned_integer (sp, push_size, bp_addr);
  /* Update $sp.  */
  regcache_cooked_write_unsigned (regcache, E_SP_REGNUM, sp);
  return sp;
}


static struct gdbarch *
mn10300_gdbarch_init (struct gdbarch_info info,
		      struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;

  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  tdep = xmalloc (sizeof (struct gdbarch_tdep));
  gdbarch = gdbarch_alloc (&info, tdep);

  switch (info.bfd_arch_info->mach)
    {
    case 0:
    case bfd_mach_mn10300:
      set_gdbarch_register_name (gdbarch, mn10300_generic_register_name);
      tdep->am33_mode = 0;
      break;
    case bfd_mach_am33:
      set_gdbarch_register_name (gdbarch, am33_register_name);
      tdep->am33_mode = 1;
      break;
    default:
      internal_error (__FILE__, __LINE__,
		      _("mn10300_gdbarch_init: Unknown mn10300 variant"));
      break;
    }

  /* Registers.  */
  set_gdbarch_num_regs (gdbarch, E_NUM_REGS);
  set_gdbarch_register_type (gdbarch, mn10300_register_type);
  set_gdbarch_skip_prologue (gdbarch, mn10300_skip_prologue);
  set_gdbarch_read_pc (gdbarch, mn10300_read_pc);
  set_gdbarch_write_pc (gdbarch, mn10300_write_pc);
  set_gdbarch_pc_regnum (gdbarch, E_PC_REGNUM);
  set_gdbarch_sp_regnum (gdbarch, E_SP_REGNUM);

  /* Stack unwinding.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  /* Breakpoints.  */
  set_gdbarch_breakpoint_from_pc (gdbarch, mn10300_breakpoint_from_pc);
  /* decr_pc_after_break? */
  /* Disassembly.  */
  set_gdbarch_print_insn (gdbarch, print_insn_mn10300);

  /* Stage 2 */
  /* MVS Note: at least the first one is deprecated!  */
  set_gdbarch_deprecated_use_struct_convention (gdbarch, 
						mn10300_use_struct_convention);
  set_gdbarch_store_return_value (gdbarch, mn10300_store_return_value);
  set_gdbarch_extract_return_value (gdbarch, mn10300_extract_return_value);
  
  /* Stage 3 -- get target calls working.  */
  set_gdbarch_push_dummy_call (gdbarch, mn10300_push_dummy_call);
  /* set_gdbarch_return_value (store, extract) */


  mn10300_frame_unwind_init (gdbarch);

  return gdbarch;
}
 
/* Dump out the mn10300 specific architecture information. */

static void
mn10300_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  fprintf_unfiltered (file, "mn10300_dump_tdep: am33_mode = %d\n",
		      tdep->am33_mode);
}

void
_initialize_mn10300_tdep (void)
{
  gdbarch_register (bfd_arch_mn10300, mn10300_gdbarch_init, mn10300_dump_tdep);
}

