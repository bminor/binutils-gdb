/* Target dependent code for ARC arhitecture, for GDB.

   Copyright 2005-2016 Free Software Foundation, Inc.
   Contributed by Synopsys Inc.

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

/* GDB header files.  */
#include "defs.h"
#include "arch-utils.h"
#include "disasm.h"
#include "dwarf2-frame.h"
#include "frame-base.h"
#include "frame-unwind.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "objfiles.h"
#include "trad-frame.h"

/* ARC header files.  */
#include "opcode/arc.h"
#include "arc-tdep.h"

/* Standard headers.  */
#include <algorithm>

/* Default target descriptions.  */
#include "features/arc-v2.c"
#include "features/arc-arcompact.c"

/* The frame unwind cache for the ARC.  Current structure is a stub, because
   it should be filled in during the prologue analysis.  */

struct arc_frame_cache
{
  /* The stack pointer at the time this frame was created; i.e. the caller's
     stack pointer when this function was called.  It is used to identify this
     frame.  */
  CORE_ADDR prev_sp;

  /* Store addresses for registers saved in prologue.  */
  struct trad_frame_saved_reg *saved_regs;
};

/* Global debug flag.  */

int arc_debug;

/* XML target description features.  */

static const char core_v2_feature_name[] = "org.gnu.gdb.arc.core.v2";
static const char
  core_reduced_v2_feature_name[] = "org.gnu.gdb.arc.core-reduced.v2";
static const char
  core_arcompact_feature_name[] = "org.gnu.gdb.arc.core.arcompact";
static const char aux_minimal_feature_name[] = "org.gnu.gdb.arc.aux-minimal";

/* XML target description known registers.  */

static const char *const core_v2_register_names[] = {
  "r0", "r1", "r2", "r3",
  "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11",
  "r12", "r13", "r14", "r15",
  "r16", "r17", "r18", "r19",
  "r20", "r21", "r22", "r23",
  "r24", "r25", "gp", "fp",
  "sp", "ilink", "r30", "blink",
  "r32", "r33", "r34", "r35",
  "r36", "r37", "r38", "r39",
  "r40", "r41", "r42", "r43",
  "r44", "r45", "r46", "r47",
  "r48", "r49", "r50", "r51",
  "r52", "r53", "r54", "r55",
  "r56", "r57", "accl", "acch",
  "lp_count", "pcl",
};

static const char *const aux_minimal_register_names[] = {
  "pc", "status32",
};

static const char *const core_arcompact_register_names[] = {
  "r0", "r1", "r2", "r3",
  "r4", "r5", "r6", "r7",
  "r8", "r9", "r10", "r11",
  "r12", "r13", "r14", "r15",
  "r16", "r17", "r18", "r19",
  "r20", "r21", "r22", "r23",
  "r24", "r25", "gp", "fp",
  "sp", "ilink1", "ilink2", "blink",
  "r32", "r33", "r34", "r35",
  "r36", "r37", "r38", "r39",
  "r40", "r41", "r42", "r43",
  "r44", "r45", "r46", "r47",
  "r48", "r49", "r50", "r51",
  "r52", "r53", "r54", "r55",
  "r56", "r57", "r58", "r59",
  "lp_count", "pcl",
};

/* Implement the "write_pc" gdbarch method.

   In ARC PC register is a normal register so in most cases setting PC value
   is a straightforward process: debugger just writes PC value.  However it
   gets trickier in case when current instruction is an instruction in delay
   slot.  In this case CPU will execute instruction at current PC value, then
   will set PC to the current value of BTA register; also current instruction
   cannot be branch/jump and some of the other instruction types.  Thus if
   debugger would try to just change PC value in this case, this instruction
   will get executed, but then core will "jump" to the original branch target.

   Whether current instruction is a delay-slot instruction or not is indicated
   by DE bit in STATUS32 register indicates if current instruction is a delay
   slot instruction.  This bit is writable by debug host, which allows debug
   host to prevent core from jumping after the delay slot instruction.  It
   also works in another direction: setting this bit will make core to treat
   any current instructions as a delay slot instruction and to set PC to the
   current value of BTA register.

   To workaround issues with changing PC register while in delay slot
   instruction, debugger should check for the STATUS32.DE bit and reset it if
   it is set.  No other change is required in this function.  Most common
   case, where this function might be required is calling inferior functions
   from debugger.  Generic GDB logic handles this pretty well: current values
   of registers are stored, value of PC is changed (that is the job of this
   function), and after inferior function is executed, GDB restores all
   registers, include BTA and STATUS32, which also means that core is returned
   to its original state of being halted on delay slot instructions.

   This method is useless for ARC 600, because it doesn't have externally
   exposed BTA register.  In the case of ARC 600 it is impossible to restore
   core to its state in all occasions thus core should never be halted (from
   the perspective of debugger host) in the delay slot.  */

static void
arc_write_pc (struct regcache *regcache, CORE_ADDR new_pc)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);

  if (arc_debug)
    debug_printf ("arc: Writing PC, new value=%s\n",
		  paddress (gdbarch, new_pc));

  regcache_cooked_write_unsigned (regcache, gdbarch_pc_regnum (gdbarch),
				  new_pc);

  ULONGEST status32;
  regcache_cooked_read_unsigned (regcache, gdbarch_ps_regnum (gdbarch),
				 &status32);

  /* Mask for DE bit is 0x40.  */
  if (status32 & 0x40)
    {
      if (arc_debug)
	{
	  debug_printf ("arc: Changing PC while in delay slot.  Will "
			"reset STATUS32.DE bit to zero.  Value of STATUS32 "
			"register is 0x%s\n",
			phex (status32, ARC_REGISTER_SIZE));
	}

      /* Reset bit and write to the cache.  */
      status32 &= ~0x40;
      regcache_cooked_write_unsigned (regcache, gdbarch_ps_regnum (gdbarch),
				      status32);
    }
}

/* Implement the "virtual_frame_pointer" gdbarch method.

   According to ABI the FP (r27) is used to point to the middle of the current
   stack frame, just below the saved FP and before local variables, register
   spill area and outgoing args.  However for optimization levels above O2 and
   in any case in leaf functions, the frame pointer is usually not set at all.
   The exception being when handling nested functions.

   We use this function to return a "virtual" frame pointer, marking the start
   of the current stack frame as a register-offset pair.  If the FP is not
   being used, then it should return SP, with an offset of the frame size.

   The current implementation doesn't actually know the frame size, nor
   whether the FP is actually being used, so for now we just return SP and an
   offset of zero.  This is no worse than other architectures, but is needed
   to avoid assertion failures.

   TODO: Can we determine the frame size to get a correct offset?

   PC is a program counter where we need the virtual FP.  REG_PTR is the base
   register used for the virtual FP.  OFFSET_PTR is the offset used for the
   virtual FP.  */

static void
arc_virtual_frame_pointer (struct gdbarch *gdbarch, CORE_ADDR pc,
			   int *reg_ptr, LONGEST *offset_ptr)
{
  *reg_ptr = gdbarch_sp_regnum (gdbarch);
  *offset_ptr = 0;
}

/* Implement the "dummy_id" gdbarch method.

   Tear down a dummy frame created by arc_push_dummy_call ().  This data has
   to be constructed manually from the data in our hand.  The stack pointer
   and program counter can be obtained from the frame info.  */

static struct frame_id
arc_dummy_id (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  return frame_id_build (get_frame_sp (this_frame),
			 get_frame_pc (this_frame));
}

/* Implement the "push_dummy_call" gdbarch method.

   Stack Frame Layout

   This shows the layout of the stack frame for the general case of a
   function call; a given function might not have a variable number of
   arguments or local variables, or might not save any registers, so it would
   not have the corresponding frame areas.  Additionally, a leaf function
   (i.e. one which calls no other functions) does not need to save the
   contents of the BLINK register (which holds its return address), and a
   function might not have a frame pointer.

   The stack grows downward, so SP points below FP in memory; SP always
   points to the last used word on the stack, not the first one.

                      |                       |   |
                      |      arg word N       |   | caller's
                      |           :           |   | frame
                      |      arg word 10      |   |
                      |      arg word 9       |   |
          old SP ---> +-----------------------+ --+
                      |                       |   |
                      |      callee-saved     |   |
                      |       registers       |   |
                      |  including fp, blink  |   |
                      |                       |   | callee's
          new FP ---> +-----------------------+   | frame
                      |                       |   |
                      |         local         |   |
                      |       variables       |   |
                      |                       |   |
                      |       register        |   |
                      |      spill area       |   |
                      |                       |   |
                      |     outgoing args     |   |
                      |                       |   |
          new SP ---> +-----------------------+ --+
                      |                       |
                      |         unused        |
                      |                       |
                                  |
                                  |
                                  V
                              downwards

   The list of arguments to be passed to a function is considered to be a
   sequence of _N_ words (as though all the parameters were stored in order in
   memory with each parameter occupying an integral number of words).  Words
   1..8 are passed in registers 0..7; if the function has more than 8 words of
   arguments then words 9..@em N are passed on the stack in the caller's frame.

   If the function has a variable number of arguments, e.g. it has a form such
   as `function (p1, p2, ...);' and _P_ words are required to hold the values
   of the named parameters (which are passed in registers 0..@em P -1), then
   the remaining 8 - _P_ words passed in registers _P_..7 are spilled into the
   top of the frame so that the anonymous parameter words occupy a continuous
   region.

   Any arguments are already in target byte order.  We just need to store
   them!

   BP_ADDR is the return address where breakpoint must be placed.  NARGS is
   the number of arguments to the function.  ARGS is the arguments values (in
   target byte order).  SP is the Current value of SP register.  STRUCT_RETURN
   is TRUE if structures are returned by the function.  STRUCT_ADDR is the
   hidden address for returning a struct.  Returns SP of a new frame.  */

static CORE_ADDR
arc_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
		     struct regcache *regcache, CORE_ADDR bp_addr, int nargs,
		     struct value **args, CORE_ADDR sp, int struct_return,
		     CORE_ADDR struct_addr)
{
  if (arc_debug)
    debug_printf ("arc: push_dummy_call (nargs = %d)\n", nargs);

  int arg_reg = ARC_FIRST_ARG_REGNUM;

  /* Push the return address.  */
  regcache_cooked_write_unsigned (regcache, ARC_BLINK_REGNUM, bp_addr);

  /* Are we returning a value using a structure return instead of a normal
     value return?  If so, struct_addr is the address of the reserved space for
     the return structure to be written on the stack, and that address is
     passed to that function as a hidden first argument.  */
  if (struct_return)
    {
      /* Pass the return address in the first argument register.  */
      regcache_cooked_write_unsigned (regcache, arg_reg, struct_addr);

      if (arc_debug)
	debug_printf ("arc: struct return address %s passed in R%d",
		      print_core_address (gdbarch, struct_addr), arg_reg);

      arg_reg++;
    }

  if (nargs > 0)
    {
      unsigned int total_space = 0;

      /* How much space do the arguments occupy in total?  Must round each
	 argument's size up to an integral number of words.  */
      for (int i = 0; i < nargs; i++)
	{
	  unsigned int len = TYPE_LENGTH (value_type (args[i]));
	  unsigned int space = align_up (len, 4);

	  total_space += space;

	  if (arc_debug)
	    debug_printf ("arc: arg %d: %u bytes -> %u\n", i, len, space);
	}

      /* Allocate a buffer to hold a memory image of the arguments.  */
      gdb_byte *memory_image = XCNEWVEC (gdb_byte, total_space);

      /* Now copy all of the arguments into the buffer, correctly aligned.  */
      gdb_byte *data = memory_image;
      for (int i = 0; i < nargs; i++)
	{
	  unsigned int len = TYPE_LENGTH (value_type (args[i]));
	  unsigned int space = align_up (len, 4);

	  memcpy (data, value_contents (args[i]), (size_t) len);
	  if (arc_debug)
	    debug_printf ("arc: copying arg %d, val 0x%08x, len %d to mem\n",
			  i, *((int *) value_contents (args[i])), len);

	  data += space;
	}

      /* Now load as much as possible of the memory image into registers.  */
      data = memory_image;
      while (arg_reg <= ARC_LAST_ARG_REGNUM)
	{
	  if (arc_debug)
	    debug_printf ("arc: passing 0x%02x%02x%02x%02x in register R%d\n",
			  data[0], data[1], data[2], data[3], arg_reg);

	  /* Note we don't use write_unsigned here, since that would convert
	     the byte order, but we are already in the correct byte order.  */
	  regcache_cooked_write (regcache, arg_reg, data);

	  data += ARC_REGISTER_SIZE;
	  total_space -= ARC_REGISTER_SIZE;

	  /* All the data is now in registers.  */
	  if (total_space == 0)
	    break;

	  arg_reg++;
	}

      /* If there is any data left, push it onto the stack (in a single write
	 operation).  */
      if (total_space > 0)
	{
	  if (arc_debug)
	    debug_printf ("arc: passing %d bytes on stack\n", total_space);

	  sp -= total_space;
	  write_memory (sp, data, (int) total_space);
	}

      xfree (memory_image);
    }

  /* Finally, update the SP register.  */
  regcache_cooked_write_unsigned (regcache, gdbarch_sp_regnum (gdbarch), sp);

  return sp;
}

/* Implement the "push_dummy_code" gdbarch method.

   We don't actually push any code.  We just identify where a breakpoint can
   be inserted to which we are can return and the resume address where we
   should be called.

   ARC does not necessarily have an executable stack, so we can't put the
   return breakpoint there.  Instead we put it at the entry point of the
   function.  This means the SP is unchanged.

   SP is a current stack pointer FUNADDR is an address of the function to be
   called.  ARGS is arguments to pass.  NARGS is a number of args to pass.
   VALUE_TYPE is a type of value returned.  REAL_PC is a resume address when
   the function is called.  BP_ADDR is an address where breakpoint should be
   set.  Returns the updated stack pointer.  */

static CORE_ADDR
arc_push_dummy_code (struct gdbarch *gdbarch, CORE_ADDR sp, CORE_ADDR funaddr,
		     struct value **args, int nargs, struct type *value_type,
		     CORE_ADDR *real_pc, CORE_ADDR *bp_addr,
		     struct regcache *regcache)
{
  *real_pc = funaddr;
  *bp_addr = entry_point_address ();
  return sp;
}

/* Implement the "cannot_fetch_register" gdbarch method.  */

static int
arc_cannot_fetch_register (struct gdbarch *gdbarch, int regnum)
{
  /* Assume that register is readable if it is unknown.  */
  return FALSE;
}

/* Implement the "cannot_store_register" gdbarch method.  */

static int
arc_cannot_store_register (struct gdbarch *gdbarch, int regnum)
{
  /* Assume that register is writable if it is unknown.  */
  switch (regnum)
    {
    case ARC_PCL_REGNUM:
      return TRUE;
    default:
      return FALSE;
    }
}

/* Get the return value of a function from the registers/memory used to
   return it, according to the convention used by the ABI - 4-bytes values are
   in the R0, while 8-byte values are in the R0-R1.

   TODO: This implementation ignores the case of "complex double", where
   according to ABI, value is returned in the R0-R3 registers.

   TYPE is a returned value's type.  VALBUF is a buffer for the returned
   value.  */

static void
arc_extract_return_value (struct gdbarch *gdbarch, struct type *type,
			  struct regcache *regcache, gdb_byte *valbuf)
{
  unsigned int len = TYPE_LENGTH (type);

  if (arc_debug)
    debug_printf ("arc: extract_return_value\n");

  if (len <= ARC_REGISTER_SIZE)
    {
      ULONGEST val;

      /* Get the return value from one register.  */
      regcache_cooked_read_unsigned (regcache, ARC_R0_REGNUM, &val);
      store_unsigned_integer (valbuf, (int) len,
			      gdbarch_byte_order (gdbarch), val);

      if (arc_debug)
	debug_printf ("arc: returning 0x%s\n", phex (val, ARC_REGISTER_SIZE));
    }
  else if (len <= ARC_REGISTER_SIZE * 2)
    {
      ULONGEST low, high;

      /* Get the return value from two registers.  */
      regcache_cooked_read_unsigned (regcache, ARC_R0_REGNUM, &low);
      regcache_cooked_read_unsigned (regcache, ARC_R1_REGNUM, &high);

      store_unsigned_integer (valbuf, ARC_REGISTER_SIZE,
			      gdbarch_byte_order (gdbarch), low);
      store_unsigned_integer (valbuf + ARC_REGISTER_SIZE,
			      (int) len - ARC_REGISTER_SIZE,
			      gdbarch_byte_order (gdbarch), high);

      if (arc_debug)
	debug_printf ("arc: returning 0x%s%s\n",
		      phex (high, ARC_REGISTER_SIZE),
		      phex (low, ARC_REGISTER_SIZE));
    }
  else
    error (_("arc: extract_return_value: type length %u too large"), len);
}


/* Store the return value of a function into the registers/memory used to
   return it, according to the convention used by the ABI.

   TODO: This implementation ignores the case of "complex double", where
   according to ABI, value is returned in the R0-R3 registers.

   TYPE is a returned value's type.  VALBUF is a buffer with the value to
   return.  */

static void
arc_store_return_value (struct gdbarch *gdbarch, struct type *type,
			struct regcache *regcache, const gdb_byte *valbuf)
{
  unsigned int len = TYPE_LENGTH (type);

  if (arc_debug)
    debug_printf ("arc: store_return_value\n");

  if (len <= ARC_REGISTER_SIZE)
    {
      ULONGEST val;

      /* Put the return value into one register.  */
      val = extract_unsigned_integer (valbuf, (int) len,
				      gdbarch_byte_order (gdbarch));
      regcache_cooked_write_unsigned (regcache, ARC_R0_REGNUM, val);

      if (arc_debug)
	debug_printf ("arc: storing 0x%s\n", phex (val, ARC_REGISTER_SIZE));
    }
  else if (len <= ARC_REGISTER_SIZE * 2)
    {
      ULONGEST low, high;

      /* Put the return value into  two registers.  */
      low = extract_unsigned_integer (valbuf, ARC_REGISTER_SIZE,
				      gdbarch_byte_order (gdbarch));
      high = extract_unsigned_integer (valbuf + ARC_REGISTER_SIZE,
				       (int) len - ARC_REGISTER_SIZE,
				       gdbarch_byte_order (gdbarch));

      regcache_cooked_write_unsigned (regcache, ARC_R0_REGNUM, low);
      regcache_cooked_write_unsigned (regcache, ARC_R1_REGNUM, high);

      if (arc_debug)
	debug_printf ("arc: storing 0x%s%s\n",
		      phex (high, ARC_REGISTER_SIZE),
		      phex (low, ARC_REGISTER_SIZE));
    }
  else
    error (_("arc_store_return_value: type length too large."));
}

/* Implement the "get_longjmp_target" gdbarch method.  */

static int
arc_get_longjmp_target (struct frame_info *frame, CORE_ADDR *pc)
{
  if (arc_debug)
    debug_printf ("arc: get_longjmp_target\n");

  struct gdbarch *gdbarch = get_frame_arch (frame);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int pc_offset = tdep->jb_pc * ARC_REGISTER_SIZE;
  gdb_byte buf[ARC_REGISTER_SIZE];
  CORE_ADDR jb_addr = get_frame_register_unsigned (frame, ARC_FIRST_ARG_REGNUM);

  if (target_read_memory (jb_addr + pc_offset, buf, ARC_REGISTER_SIZE))
    return 0; /* Failed to read from memory.  */

  *pc = extract_unsigned_integer (buf, ARC_REGISTER_SIZE,
				  gdbarch_byte_order (gdbarch));
  return 1;
}

/* Implement the "return_value" gdbarch method.  */

static enum return_value_convention
arc_return_value (struct gdbarch *gdbarch, struct value *function,
		  struct type *valtype, struct regcache *regcache,
		  gdb_byte *readbuf, const gdb_byte *writebuf)
{
  /* If the return type is a struct, or a union, or would occupy more than two
     registers, the ABI uses the "struct return convention": the calling
     function passes a hidden first parameter to the callee (in R0).  That
     parameter is the address at which the value being returned should be
     stored.  Otherwise, the result is returned in registers.  */
  int is_struct_return = (TYPE_CODE (valtype) == TYPE_CODE_STRUCT
			  || TYPE_CODE (valtype) == TYPE_CODE_UNION
			  || TYPE_LENGTH (valtype) > 2 * ARC_REGISTER_SIZE);

  if (arc_debug)
    debug_printf ("arc: return_value (readbuf = %s, writebuf = %s)\n",
		  host_address_to_string (readbuf),
		  host_address_to_string (writebuf));

  if (writebuf != NULL)
    {
      /* Case 1.  GDB should not ask us to set a struct return value: it
	 should know the struct return location and write the value there
	 itself.  */
      gdb_assert (!is_struct_return);
      arc_store_return_value (gdbarch, valtype, regcache, writebuf);
    }
  else if (readbuf != NULL)
    {
      /* Case 2.  GDB should not ask us to get a struct return value: it
	 should know the struct return location and read the value from there
	 itself.  */
      gdb_assert (!is_struct_return);
      arc_extract_return_value (gdbarch, valtype, regcache, readbuf);
    }

  return (is_struct_return
	  ? RETURN_VALUE_STRUCT_CONVENTION
	  : RETURN_VALUE_REGISTER_CONVENTION);
}

/* Return the base address of the frame.  For ARC, the base address is the
   frame pointer.  */

static CORE_ADDR
arc_frame_base_address (struct frame_info *this_frame, void **prologue_cache)
{
  return (CORE_ADDR) get_frame_register_unsigned (this_frame, ARC_FP_REGNUM);
}

/* Implement the "skip_prologue" gdbarch method.

   Skip the prologue for the function at PC.  This is done by checking from
   the line information read from the DWARF, if possible; otherwise, we scan
   the function prologue to find its end.  */

static CORE_ADDR
arc_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  if (arc_debug)
    debug_printf ("arc: skip_prologue\n");

  CORE_ADDR func_addr;
  const char *func_name;

  /* See what the symbol table says.  */
  if (find_pc_partial_function (pc, &func_name, &func_addr, NULL))
    {
      /* Found a function.  */
      CORE_ADDR postprologue_pc
	= skip_prologue_using_sal (gdbarch, func_addr);

      if (postprologue_pc != 0)
	return std::max (pc, postprologue_pc);
    }

  /* No prologue info in symbol table, have to analyze prologue.  */

  /* Find an upper limit on the function prologue using the debug
     information.  If the debug information could not be used to provide that
     bound, then pass 0 and arc_scan_prologue will estimate value itself.  */
  CORE_ADDR limit_pc = skip_prologue_using_sal (gdbarch, pc);
  /* We don't have a proper analyze_prologue function yet, but its result
     should be returned here.  Currently GDB will just stop at the first
     instruction of function if debug information doesn't have prologue info;
     and if there is a debug info about prologue - this code path will not be
     taken at all.  */
  return (limit_pc == 0 ? pc : limit_pc);
}

/* Implement the "print_insn" gdbarch method.

   arc_get_disassembler () may return different functions depending on bfd
   type, so it is not possible to pass print_insn directly to
   set_gdbarch_print_insn ().  Instead this wrapper function is used.  It also
   may be used by other functions to get disassemble_info for address.  It is
   important to note, that those print_insn from opcodes always print
   instruction to the stream specified in the INFO.  If this is not desired,
   then either `print_insn` function in INFO should be set to some function
   that will not print, or `stream` should be different from standard
   gdb_stdlog.  */

static int
arc_delayed_print_insn (bfd_vma addr, struct disassemble_info *info)
{
  int (*print_insn) (bfd_vma, struct disassemble_info *);
  /* exec_bfd may be null, if GDB is run without a target BFD file.  Opcodes
     will handle NULL value gracefully.  */
  print_insn = arc_get_disassembler (exec_bfd);
  gdb_assert (print_insn != NULL);
  return print_insn (addr, info);
}

/* Baremetal breakpoint instructions.

   ARC supports both big- and little-endian.  However, instructions for
   little-endian processors are encoded in the middle-endian: half-words are
   in big-endian, while bytes inside the half-words are in little-endian; data
   is represented in the "normal" little-endian.  Big-endian processors treat
   data and code identically.

   Assuming the number 0x01020304, it will be presented this way:

   Address            :  N   N+1  N+2  N+3
   little-endian      : 0x04 0x03 0x02 0x01
   big-endian         : 0x01 0x02 0x03 0x04
   ARC middle-endian  : 0x02 0x01 0x04 0x03
  */

static const gdb_byte arc_brk_s_be[] = { 0x7f, 0xff };
static const gdb_byte arc_brk_s_le[] = { 0xff, 0x7f };
static const gdb_byte arc_brk_be[] = { 0x25, 0x6f, 0x00, 0x3f };
static const gdb_byte arc_brk_le[] = { 0x6f, 0x25, 0x3f, 0x00 };

/* For ARC ELF, breakpoint uses the 16-bit BRK_S instruction, which is 0x7fff
   (little endian) or 0xff7f (big endian).  We used to insert BRK_S even
   instead of 32-bit instructions, which works mostly ok, unless breakpoint is
   inserted into delay slot instruction.  In this case if branch is taken
   BLINK value will be set to address of instruction after delay slot, however
   if we replaced 32-bit instruction in delay slot with 16-bit long BRK_S,
   then BLINK value will have an invalid value - it will point to the address
   after the BRK_S (which was there at the moment of branch execution) while
   it should point to the address after the 32-bit long instruction.  To avoid
   such issues this function disassembles instruction at target location and
   evaluates it value.

   ARC 600 supports only 16-bit BRK_S.

   NB: Baremetal GDB uses BRK[_S], while user-space GDB uses TRAP_S.  BRK[_S]
   is much better because it doesn't commit unlike TRAP_S, so it can be set in
   delay slots; however it cannot be used in user-mode, hence usage of TRAP_S
   in GDB for user-space.  */

/* Implement the "breakpoint_kind_from_pc" gdbarch method.  */

static int
arc_breakpoint_kind_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pcptr)
{
  size_t length_with_limm = gdb_insn_length (gdbarch, *pcptr);

  /* Replace 16-bit instruction with BRK_S, replace 32-bit instructions with
     BRK.  LIMM is part of instruction length, so it can be either 4 or 8
     bytes for 32-bit instructions.  */
  if ((length_with_limm == 4 || length_with_limm == 8)
      && !arc_mach_is_arc600 (gdbarch))
    return sizeof (arc_brk_le);
  else
    return sizeof (arc_brk_s_le);
}

/* Implement the "sw_breakpoint_from_kind" gdbarch method.  */

static const gdb_byte *
arc_sw_breakpoint_from_kind (struct gdbarch *gdbarch, int kind, int *size)
{
  *size = kind;

  if (kind == sizeof (arc_brk_le))
    {
      return ((gdbarch_byte_order (gdbarch) == BFD_ENDIAN_BIG)
	      ? arc_brk_be
	      : arc_brk_le);
    }
  else
    {
      return ((gdbarch_byte_order (gdbarch) == BFD_ENDIAN_BIG)
	      ? arc_brk_s_be
	      : arc_brk_s_le);
    }
}

/* Implement the "unwind_pc" gdbarch method.  */

static CORE_ADDR
arc_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  int pc_regnum = gdbarch_pc_regnum (gdbarch);
  CORE_ADDR pc = frame_unwind_register_unsigned (next_frame, pc_regnum);

  if (arc_debug)
    debug_printf ("arc: unwind PC: %s\n", paddress (gdbarch, pc));

  return pc;
}

/* Implement the "unwind_sp" gdbarch method.  */

static CORE_ADDR
arc_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  int sp_regnum = gdbarch_sp_regnum (gdbarch);
  CORE_ADDR sp = frame_unwind_register_unsigned (next_frame, sp_regnum);

  if (arc_debug)
    debug_printf ("arc: unwind SP: %s\n", paddress (gdbarch, sp));

  return sp;
}

/* Implement the "frame_align" gdbarch method.  */

static CORE_ADDR
arc_frame_align (struct gdbarch *gdbarch, CORE_ADDR sp)
{
  return align_down (sp, 4);
}

/* Frame unwinder for normal frames.  */

static struct arc_frame_cache *
arc_make_frame_cache (struct frame_info *this_frame)
{
  if (arc_debug)
    debug_printf ("arc: frame_cache\n");

  struct gdbarch *gdbarch = get_frame_arch (this_frame);

  CORE_ADDR block_addr = get_frame_address_in_block (this_frame);
  CORE_ADDR prev_pc = get_frame_pc (this_frame);

  CORE_ADDR entrypoint, prologue_end;
  if (find_pc_partial_function (block_addr, NULL, &entrypoint, &prologue_end))
    {
      struct symtab_and_line sal = find_pc_line (entrypoint, 0);
      if (sal.line == 0)
	/* No line info so use current PC.  */
	prologue_end = prev_pc;
      else if (sal.end < prologue_end)
	/* The next line begins after the function end.  */
	prologue_end = sal.end;

      prologue_end = std::min (prologue_end, prev_pc);
    }
  else
    {
      entrypoint = get_frame_register_unsigned (this_frame,
						gdbarch_pc_regnum (gdbarch));
      prologue_end = 0;
    }

  /* Allocate new frame cache instance and space for saved register info.
   * FRAME_OBSTACK_ZALLOC will initialize fields to zeroes.  */
  struct arc_frame_cache *cache
    = FRAME_OBSTACK_ZALLOC (struct arc_frame_cache);
  cache->saved_regs = trad_frame_alloc_saved_regs (this_frame);

  /* Should call analyze_prologue here, when it will be implemented.  */

  return cache;
}

/* Implement the "this_id" frame_unwind method.  */

static void
arc_frame_this_id (struct frame_info *this_frame, void **this_cache,
		   struct frame_id *this_id)
{
  if (arc_debug)
    debug_printf ("arc: frame_this_id\n");

  struct gdbarch *gdbarch = get_frame_arch (this_frame);

  if (*this_cache == NULL)
    *this_cache = arc_make_frame_cache (this_frame);
  struct arc_frame_cache *cache = (struct arc_frame_cache *) (*this_cache);

  CORE_ADDR stack_addr = cache->prev_sp;

  /* There are 4 possible situation which decide how frame_id->code_addr is
     evaluated:

     1) Function is compiled with option -g.  Then frame_id will be created
     in dwarf_* function and not in this function.  NB: even if target
     binary is compiled with -g, some std functions like __start and _init
     are not, so they still will follow one of the following choices.

     2) Function is compiled without -g and binary hasn't been stripped in
     any way.  In this case GDB still has enough information to evaluate
     frame code_addr properly.  This case is covered by call to
     get_frame_func ().

     3) Binary has been striped with option -g (strip debug symbols).  In
     this case there is still enough symbols for get_frame_func () to work
     properly, so this case is also covered by it.

     4) Binary has been striped with option -s (strip all symbols).  In this
     case GDB cannot get function start address properly, so we return current
     PC value instead.
   */
  CORE_ADDR code_addr = get_frame_func (this_frame);
  if (code_addr == 0)
    code_addr = get_frame_register_unsigned (this_frame,
					     gdbarch_pc_regnum (gdbarch));

  *this_id = frame_id_build (stack_addr, code_addr);
}

/* Implement the "prev_register" frame_unwind method.  */

static struct value *
arc_frame_prev_register (struct frame_info *this_frame,
			 void **this_cache, int regnum)
{
  if (*this_cache == NULL)
    *this_cache = arc_make_frame_cache (this_frame);
  struct arc_frame_cache *cache = (struct arc_frame_cache *) (*this_cache);

  struct gdbarch *gdbarch = get_frame_arch (this_frame);

  /* If we are asked to unwind the PC, then we need to return BLINK instead:
     the saved value of PC points into this frame's function's prologue, not
     the next frame's function's resume location.  */
  if (regnum == gdbarch_pc_regnum (gdbarch))
    regnum = ARC_BLINK_REGNUM;

  /* SP is a special case - we should return prev_sp, because
     trad_frame_get_prev_register will return _current_ SP value.
     Alternatively we could have stored cache->prev_sp in the cache->saved
     regs, but here we follow the lead of AArch64, ARM and Xtensa and will
     leave that logic in this function, instead of prologue analyzers.  That I
     think is a bit more clear as `saved_regs` should contain saved regs, not
     computable.

     Because value has been computed, "got_constant" should be used, so that
     returned value will be a "not_lval" - immutable.  */

  if (regnum == gdbarch_sp_regnum (gdbarch))
    return frame_unwind_got_constant (this_frame, regnum, cache->prev_sp);

  return trad_frame_get_prev_register (this_frame, cache->saved_regs, regnum);
}

/* Implement the "init_reg" dwarf2_frame method.  */

static void
arc_dwarf2_frame_init_reg (struct gdbarch *gdbarch, int regnum,
			   struct dwarf2_frame_state_reg *reg,
			   struct frame_info *info)
{
  if (regnum == gdbarch_pc_regnum (gdbarch))
    /* The return address column.  */
    reg->how = DWARF2_FRAME_REG_RA;
  else if (regnum == gdbarch_sp_regnum (gdbarch))
    /* The call frame address.  */
    reg->how = DWARF2_FRAME_REG_CFA;
}

/* Structure defining the ARC ordinary frame unwind functions.  Since we are
   the fallback unwinder, we use the default frame sniffer, which always
   accepts the frame.  */

static const struct frame_unwind arc_frame_unwind = {
  NORMAL_FRAME,
  default_frame_unwind_stop_reason,
  arc_frame_this_id,
  arc_frame_prev_register,
  NULL,
  default_frame_sniffer,
  NULL,
  NULL
};


static const struct frame_base arc_normal_base = {
  &arc_frame_unwind,
  arc_frame_base_address,
  arc_frame_base_address,
  arc_frame_base_address
};

/* Initialize target description for the ARC.

   Returns TRUE if input tdesc was valid and in this case it will assign TDESC
   and TDESC_DATA output parameters.  */

static int
arc_tdesc_init (struct gdbarch_info info, const struct target_desc **tdesc,
		struct tdesc_arch_data **tdesc_data)
{
  if (arc_debug)
    debug_printf ("arc: Target description initialization.\n");

  const struct target_desc *tdesc_loc = info.target_desc;

  /* Depending on whether this is ARCompact or ARCv2 we will assign
     different default registers sets (which will differ in exactly two core
     registers).  GDB will also refuse to accept register feature from invalid
     ISA - v2 features can be used only with v2 ARChitecture.  We read
     bfd_arch_info, which looks like to be a safe bet here, as it looks like it
     is always initialized even when we don't pass any elf file to GDB at all
     (it uses default arch in this case).  Also GDB will call this function
     multiple times, and if XML target description file contains architecture
     specifications, then GDB will set this architecture to info.bfd_arch_info,
     overriding value from ELF file if they are different.  That means that,
     where matters, this value is always our best guess on what CPU we are
     debugging.  It has been noted that architecture specified in tdesc file
     has higher precedence over ELF and even "set architecture" - that is,
     using "set architecture" command will have no effect when tdesc has "arch"
     tag.  */
  /* Cannot use arc_mach_is_arcv2 (), because gdbarch is not created yet.  */
  const int is_arcv2 = (info.bfd_arch_info->mach == bfd_mach_arc_arcv2);
  int is_reduced_rf;
  const char *const *core_regs;
  const char *core_feature_name;

  /* If target doesn't provide a description - use default one.  */
  if (!tdesc_has_registers (tdesc_loc))
    {
      if (is_arcv2)
	{
	  tdesc_loc = tdesc_arc_v2;
	  if (arc_debug)
	    debug_printf ("arc: Using default register set for ARC v2.\n");
	}
      else
	{
	  tdesc_loc = tdesc_arc_arcompact;
	  if (arc_debug)
	    debug_printf ("arc: Using default register set for ARCompact.\n");
	}
    }
  else
    {
      if (arc_debug)
	debug_printf ("arc: Using provided register set.\n");
    }
  gdb_assert (tdesc_loc != NULL);

  /* Now we can search for base registers.  Core registers can be either full
     or reduced.  Summary:

     - core.v2 + aux-minimal
     - core-reduced.v2 + aux-minimal
     - core.arcompact + aux-minimal

     NB: It is entirely feasible to have ARCompact with reduced core regs, but
     we ignore that because GCC doesn't support that and at the same time
     ARCompact is considered obsolete, so there is not much reason to support
     that.  */
  const struct tdesc_feature *feature
    = tdesc_find_feature (tdesc_loc, core_v2_feature_name);
  if (feature != NULL)
    {
      /* Confirm that register and architecture match, to prevent accidents in
	 some situations.  This code will trigger an error if:

	 1. XML tdesc doesn't specify arch explicitly, registers are for arch
	 X, but ELF specifies arch Y.

	 2. XML tdesc specifies arch X, but contains registers for arch Y.

	 It will not protect from case where XML or ELF specify arch X,
	 registers are for the same arch X, but the real target is arch Y.  To
	 detect this case we need to check IDENTITY register.  */
      if (!is_arcv2)
	{
	  arc_print (_("Error: ARC v2 target description supplied for "
		       "non-ARCv2 target.\n"));
	  return FALSE;
	}

      is_reduced_rf = FALSE;
      core_feature_name = core_v2_feature_name;
      core_regs = core_v2_register_names;
    }
  else
    {
      feature = tdesc_find_feature (tdesc_loc, core_reduced_v2_feature_name);
      if (feature != NULL)
	{
	  if (!is_arcv2)
	    {
	      arc_print (_("Error: ARC v2 target description supplied for "
			   "non-ARCv2 target.\n"));
	      return FALSE;
	    }

	  is_reduced_rf = TRUE;
	  core_feature_name = core_reduced_v2_feature_name;
	  core_regs = core_v2_register_names;
	}
      else
	{
	  feature = tdesc_find_feature (tdesc_loc,
					core_arcompact_feature_name);
	  if (feature != NULL)
	    {
	      if (is_arcv2)
		{
		  arc_print (_("Error: ARCompact target description supplied "
			       "for non-ARCompact target.\n"));
		  return FALSE;
		}

	      is_reduced_rf = FALSE;
	      core_feature_name = core_arcompact_feature_name;
	      core_regs = core_arcompact_register_names;
	    }
	  else
	    {
	      arc_print (_("Error: Couldn't find core register feature in "
			   "supplied target description."));
	      return FALSE;
	    }
	}
    }

  struct tdesc_arch_data *tdesc_data_loc = tdesc_data_alloc ();

  gdb_assert (feature != NULL);
  int valid_p = 1;

  for (int i = 0; i <= ARC_LAST_CORE_REGNUM; i++)
    {
      /* If rf16, then skip extra registers.  */
      if (is_reduced_rf && ((i >= ARC_R4_REGNUM && i <= ARC_R9_REGNUM)
			    || (i >= ARC_R16_REGNUM && i <= ARC_R25_REGNUM)))
	continue;

      valid_p = tdesc_numbered_register (feature, tdesc_data_loc, i,
					 core_regs[i]);

      /* - Ignore errors in extension registers - they are optional.
	 - Ignore missing ILINK because it doesn't make sense for Linux.
	 - Ignore missing ILINK2 when architecture is ARCompact, because it
	 doesn't make sense for Linux targets.

	 In theory those optional registers should be in separate features, but
	 that would create numerous but tiny features, which looks like an
	 overengineering of a rather simple task.  */
      if (!valid_p && (i <= ARC_SP_REGNUM || i == ARC_BLINK_REGNUM
		       || i == ARC_LP_COUNT_REGNUM || i == ARC_PCL_REGNUM
		       || (i == ARC_R30_REGNUM && is_arcv2)))
	{
	  arc_print (_("Error: Cannot find required register `%s' in "
		       "feature `%s'.\n"), core_regs[i], core_feature_name);
	  tdesc_data_cleanup (tdesc_data_loc);
	  return FALSE;
	}
    }

  /* Mandatory AUX registeres are intentionally few and are common between
     ARCompact and ARC v2, so same code can be used for both.  */
  feature = tdesc_find_feature (tdesc_loc, aux_minimal_feature_name);
  if (feature == NULL)
    {
      arc_print (_("Error: Cannot find required feature `%s' in supplied "
		   "target description.\n"), aux_minimal_feature_name);
      tdesc_data_cleanup (tdesc_data_loc);
      return FALSE;
    }

  for (int i = ARC_FIRST_AUX_REGNUM; i <= ARC_LAST_AUX_REGNUM; i++)
    {
      const char *name = aux_minimal_register_names[i - ARC_FIRST_AUX_REGNUM];
      valid_p = tdesc_numbered_register (feature, tdesc_data_loc, i, name);
      if (!valid_p)
	{
	  arc_print (_("Error: Cannot find required register `%s' "
		       "in feature `%s'.\n"),
		     name, tdesc_feature_name (feature));
	  tdesc_data_cleanup (tdesc_data_loc);
	  return FALSE;
	}
    }

  *tdesc = tdesc_loc;
  *tdesc_data = tdesc_data_loc;

  return TRUE;
}

/* Implement the "init" gdbarch method.  */

static struct gdbarch *
arc_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  const struct target_desc *tdesc;
  struct tdesc_arch_data *tdesc_data;

  if (arc_debug)
    debug_printf ("arc: Architecture initialization.\n");

  if (!arc_tdesc_init (info, &tdesc, &tdesc_data))
    return NULL;

  /* Allocate the ARC-private target-dependent information structure, and the
     GDB target-independent information structure.  */
  struct gdbarch_tdep *tdep = XCNEW (struct gdbarch_tdep);
  tdep->jb_pc = -1; /* No longjmp support by default.  */
  struct gdbarch *gdbarch = gdbarch_alloc (&info, tdep);

  /* Data types.  */
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_long_bit (gdbarch, 32);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_long_long_align_bit (gdbarch, 32);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_float_format (gdbarch, floatformats_ieee_single);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_double_format (gdbarch, floatformats_ieee_double);
  set_gdbarch_ptr_bit (gdbarch, 32);
  set_gdbarch_addr_bit (gdbarch, 32);
  set_gdbarch_char_signed (gdbarch, 0);

  set_gdbarch_write_pc (gdbarch, arc_write_pc);

  set_gdbarch_virtual_frame_pointer (gdbarch, arc_virtual_frame_pointer);

  /* tdesc_use_registers expects gdbarch_num_regs to return number of registers
     parsed by gdbarch_init, and then it will add all of the remaining
     registers and will increase number of registers.  */
  set_gdbarch_num_regs (gdbarch, ARC_LAST_REGNUM + 1);
  set_gdbarch_num_pseudo_regs (gdbarch, 0);
  set_gdbarch_sp_regnum (gdbarch, ARC_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, ARC_PC_REGNUM);
  set_gdbarch_ps_regnum (gdbarch, ARC_STATUS32_REGNUM);
  set_gdbarch_fp0_regnum (gdbarch, -1);	/* No FPU registers.  */

  set_gdbarch_dummy_id (gdbarch, arc_dummy_id);
  set_gdbarch_push_dummy_call (gdbarch, arc_push_dummy_call);
  set_gdbarch_push_dummy_code (gdbarch, arc_push_dummy_code);

  set_gdbarch_cannot_fetch_register (gdbarch, arc_cannot_fetch_register);
  set_gdbarch_cannot_store_register (gdbarch, arc_cannot_store_register);

  set_gdbarch_believe_pcc_promotion (gdbarch, 1);

  set_gdbarch_return_value (gdbarch, arc_return_value);

  set_gdbarch_skip_prologue (gdbarch, arc_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  set_gdbarch_breakpoint_kind_from_pc (gdbarch, arc_breakpoint_kind_from_pc);
  set_gdbarch_sw_breakpoint_from_kind (gdbarch, arc_sw_breakpoint_from_kind);

  /* On ARC 600 BRK_S instruction advances PC, unlike other ARC cores.  */
  if (!arc_mach_is_arc600 (gdbarch))
    set_gdbarch_decr_pc_after_break (gdbarch, 0);
  else
    set_gdbarch_decr_pc_after_break (gdbarch, 2);

  set_gdbarch_unwind_pc (gdbarch, arc_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, arc_unwind_sp);

  set_gdbarch_frame_align (gdbarch, arc_frame_align);

  set_gdbarch_print_insn (gdbarch, arc_delayed_print_insn);

  set_gdbarch_cannot_step_breakpoint (gdbarch, 1);

  /* "nonsteppable" watchpoint means that watchpoint triggers before
     instruction is committed, therefore it is required to remove watchpoint
     to step though instruction that triggers it.  ARC watchpoints trigger
     only after instruction is committed, thus there is no need to remove
     them.  In fact on ARC watchpoint for memory writes may trigger with more
     significant delay, like one or two instructions, depending on type of
     memory where write is performed (CCM or external) and next instruction
     after the memory write.  */
  set_gdbarch_have_nonsteppable_watchpoint (gdbarch, 0);

  /* This doesn't include possible long-immediate value.  */
  set_gdbarch_max_insn_length (gdbarch, 4);

  /* Frame unwinders and sniffers.  */
  dwarf2_frame_set_init_reg (gdbarch, arc_dwarf2_frame_init_reg);
  dwarf2_append_unwinders (gdbarch);
  frame_unwind_append_unwinder (gdbarch, &arc_frame_unwind);
  frame_base_set_default (gdbarch, &arc_normal_base);

  /* Setup stuff specific to a particular environment (baremetal or Linux).
     It can override functions set earlier.  */
  gdbarch_init_osabi (info, gdbarch);

  if (tdep->jb_pc >= 0)
    set_gdbarch_get_longjmp_target (gdbarch, arc_get_longjmp_target);

  tdesc_use_registers (gdbarch, tdesc, tdesc_data);

  return gdbarch;
}

/* Implement the "dump_tdep" gdbarch method.  */

static void
arc_dump_tdep (struct gdbarch *gdbarch, struct ui_file *file)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  fprintf_unfiltered (file, "arc_dump_tdep: jb_pc = %i\n", tdep->jb_pc);
}

/* Suppress warning from -Wmissing-prototypes.  */
extern initialize_file_ftype _initialize_arc_tdep;

void
_initialize_arc_tdep (void)
{
  gdbarch_register (bfd_arch_arc, arc_gdbarch_init, arc_dump_tdep);

  initialize_tdesc_arc_v2 ();
  initialize_tdesc_arc_arcompact ();

  /* Register ARC-specific commands with gdb.  */

  /* Debug internals for ARC GDB.  */
  add_setshow_zinteger_cmd ("arc", class_maintenance,
			    &arc_debug,
			    _("Set ARC specific debugging."),
			    _("Show ARC specific debugging."),
			    _("Non-zero enables ARC specific debugging."),
			    NULL, NULL, &setdebuglist, &showdebuglist);
}
