/* Target dependent code for ARC700, for GDB, the GNU debugger.

   Copyright 2005 Free Software Foundation, Inc.

   Authors: 
      Soam Vasani <soam.vasani@codito.com>
      Ramana Radhakrishnan <ramana.radhakrishnan@codito.com> 

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include <string.h>

#include "defs.h"
#include "osabi.h"
#include "frame.h"
#include "regcache.h"
#include "gdb_assert.h"
#include "inferior.h"
#include "reggroups.h"
#include "solib-svr4.h"
#include "symtab.h"
#include "objfiles.h"
#include "block.h"

#include "arc-tdep.h"
#include "regset.h"

//#define ARC_DEBUG 1

/* Default Breakpoint instructions used for 
   ARC700 Linux 
*/
unsigned int arc700_linux_breakpoint_size = 2;
unsigned char arc700_linux_breakpoint_insn[2] = { 0x3e,0x78 } ;

static const char *
arc_linux_register_name (int regno)
{
  static char linux_names[][10] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6",
				   "r7", "r8", "r9", "r10", "r11", "r12", "r13",
				   "r14", "r15", "r16", "r17", "r18", "r19", "r20",
				   "r21", "r22", "r23", "r24", "r25", "r26",
				   
				   "bta",
				   "lp_start", "lp_end", "lp_count",
				   "status32", "blink",
				   "fp", "sp", "efa",
				   /* linux-only registers */
				   "ret", "orig_r8", "pc",

				   /* pseudo-regs */
				   "ilink1", "ilink2", "eret",
				   "status_l1", "status_l2", "erstatus" };

  gdb_assert(ARRAY_SIZE (linux_names) == NUM_REGS + NUM_PSEUDO_REGS);
  gdb_assert(regno >=0 && regno < NUM_REGS + NUM_PSEUDO_REGS);

  return linux_names[regno];
}

/*
 * The kernel stores only one of (ilink1,ilink2,eret).  This is stored in
 * the ret "register".  ilink1 is stored when the kernel has been entered
 * because of a level 1 interrupt, etc.
 *
 * Same story for (status_l1, status_l2, erstatus).
 *
 * This disambiguity has been fixed by adding orig_r8 to pt_regs.
 * It will take the following values -
 *    1. if an exception of any kind occurs then orig_r8 >= 0
 *    2. Int level 1 : -1
 *    3. Int level 2 : -2
 *
 * Registers whose value we don't know are given the value zero.
 */
static void
arc_linux_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
				int regno, void *buf)
{
  int status32, ret, orig_r8;
  regcache_cooked_read (current_regcache, ARC_ORIG_R8_REGNUM, &orig_r8);

  if(regno == ARC_ILINK1_REGNUM ||
     regno == ARC_ILINK2_REGNUM ||
     regno == ARC_ERET_REGNUM)
    {
      regcache_cooked_read (current_regcache, ARC_RET_REGNUM, &ret);

      if(regno == ARC_ILINK1_REGNUM)
	*((unsigned int *)buf) = ((orig_r8 == -1) ? ret : 0);
      else if(regno == ARC_ILINK2_REGNUM)
	*((unsigned int *)buf) = ((orig_r8 == -2) ? ret : 0);
      else if(regno == ARC_ERET_REGNUM)
	*((unsigned int *)buf) = ((orig_r8 >= 0) ? ret : 0);

    }
  else if(regno == ARC_STATUS32_L1_REGNUM ||
	  regno == ARC_STATUS32_L2_REGNUM ||
	  regno == ARC_ERSTATUS_REGNUM)
    {
      regcache_cooked_read (current_regcache, ARC_STATUS32_REGNUM, &status32);

      if(regno == ARC_STATUS32_L1_REGNUM)
	*((unsigned int *)buf) = ((orig_r8 == -1) ? status32 : 0);
      else if(regno == ARC_STATUS32_L2_REGNUM)
	*((unsigned int *)buf) = ((orig_r8 == -2) ? status32 : 0);
      else if(regno == ARC_ERSTATUS_REGNUM)
	*((unsigned int *)buf) = ((orig_r8 >= 0) ? status32 : 0);
    }
  else
    internal_error(__FILE__, __LINE__, "arc_pseudo_register_read: bad register number (%d)", regno);
}

static void
arc_linux_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
				 int regno, const void *buf)
{
  /* none of our pseudo-regs are writable */
  internal_error(__FILE__, __LINE__, "arc_pseudo_register_write: bad register number");
}

/*
 * print registers in the correct order.
 *
 * Why not have the regnums in the right order in the first place ?
 * Because some of the registers have to be pseudo-registers because of
 * the way the kernel is written, and because gdb assumes that
 * pseudo-registers have regnums greater than real register regnums.
 */
static void
arc_linux_print_registers_info (struct gdbarch *gdbarch, struct ui_file *file,
				struct frame_info *frame, int regnum, int all)
{
  int i;

  if (regnum >= 0)
    {
      default_print_registers_info (gdbarch, file, frame, regnum, all);
      return;
    }

  /* print all registers */

  /* r0..r26 */
  for (i=0; i <= 26; ++i)
    default_print_registers_info (gdbarch, file, frame, i, all);

  default_print_registers_info (gdbarch, file, frame, ARC_FP_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_SP_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_ILINK1_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_ILINK2_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_BLINK_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_LP_COUNT_REGNUM, all);
  
  /* now the aux registers */
  
  default_print_registers_info (gdbarch, file, frame, ARC_BTA_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_LP_START_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_LP_END_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_EFA_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_ERET_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_STATUS32_L1_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_STATUS32_L2_REGNUM, all);
  default_print_registers_info (gdbarch, file, frame, ARC_ERSTATUS_REGNUM, all);

  /* show the pc */
  default_print_registers_info (gdbarch, file, frame, ARC_STOP_PC_REGNUM, all);
}


/*
 * mov r8,nr_sigreturn
 * swi 
 */
static char arc_sigtramp_insn[] = { 0x8a, 0x20, 0xc1, 0x1d, 0x6f, 0x22, 0x3f, 0x00 };

/* Return whether the frame preceding NEXT_FRAME corresponds to a
   GNU/Linux sigtramp routine.  */
static int
arc_linux_sigtramp_p (struct frame_info *next_frame)
{
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  unsigned char buf[8];

  if (!safe_frame_unwind_memory (next_frame, pc, buf, 8))
    return 0;

  if (memcmp(buf, arc_sigtramp_insn, 8) == 0)
    return 1;
  else
    {
      pc -= 4;
      
      if (!safe_frame_unwind_memory (next_frame, pc, buf, 8))
	return 0;

      if (memcmp(buf, arc_sigtramp_insn, 8) == 0)
	return 1;
    }

  return 0;
}

/* Assuming NEXT_FRAME is a frame following a GNU/Linux sigtramp
   routine, return the address of the associated sigcontext structure.  */
static CORE_ADDR
arc_linux_sigcontext_addr (struct frame_info *next_frame)
{
  char buf[4];
  CORE_ADDR sp;
  
  frame_unwind_register (next_frame, ARC_SP_REGNUM, buf);
  sp = extract_unsigned_integer (buf, 4);

  return sp;
}

int
arc_linux_register_reggroup_p (int regnum, struct reggroup *group)
{
  if(regnum == ARC_ORIG_R8_REGNUM && group == system_reggroup)
    return 1;

  if(regnum == ARC_RET_REGNUM && group == general_reggroup)
    return 0;

  return -1;
}

/* Mapping between the general-purpose registers in `struct
   sigcontext' format and GDB's register cache layout.
   arc_linux_sc_reg_offset[i] is the sigcontext offset of GDB regnum `i'.  */

/* From <asm/sigcontext.h>.  */
static int arc_linux_sc_reg_offset[] =
{
  23 * 4,				     /* r0 */
  22 * 4,				     /* r1 */
  21 * 4,				     /* r2 */
  20 * 4,				     /* r3 */
  19 * 4,				     /* r4 */
  18 * 4,				     /* r5 */
  17 * 4,				     /* r6 */
  16 * 4,				     /* r7 */
  15 * 4,				     /* r8 */
  14 * 4,				     /* r9 */
  13 * 4,				     /* r10 */
  12 * 4,				     /* r11 */
  11 * 4,				     /* r12 */
  -1,					     /* r13 */
  -1,					     /* r14 */
  -1,					     /* r15 */
  -1,					     /* r16 */
  -1,					     /* r17 */
  -1,					     /* r18 */
  -1,					     /* r19 */
  -1,					     /* r20 */
  -1,					     /* r21 */
  -1,					     /* r22 */
  -1,					     /* r23 */
  -1,					     /* r24 */
  -1,					     /* r25 */
  10 * 4,				     /* r26 */
  2 * 4,				     /* bta */
  3 * 4,				     /* lp_start */
  4 * 4,				     /* lp_end */
  5 * 4,				     /* lp_count */
  6 * 4,				     /* status32 */
  8 * 4,				     /* blink */
  9 * 4,				     /* fp */
  1 * 4,				     /* sp */
  -1,					     /* efa */
  7 * 4,				     /* ret */
  -1,					     /* orig_r8 */
  -1, 					     /* stop_pc */
};


/* Set the program counter for process PTID to PC.  */

static void
arc700_linux_write_pc (CORE_ADDR pc, ptid_t ptid)
{
  ULONGEST val;
  write_register_pid (PC_REGNUM, pc, ptid);

  /* We must be careful with modifying the program counter.  If we
     just interrupted a system call, the kernel might try to restart
     it when we resume the inferior.  On restarting the system call,
     the kernel will try backing up the program counter even though it
     no longer points at the system call.  This typically results in a
     SIGSEGV or SIGILL.  We can prevent this by writing `-1' in the
     "orig_r8" pseudo-register.

     Note that "orig_r8" is saved when setting up a dummy call frame.
     This means that it is properly restored when that frame is
     popped, and that the interrupted system call will be restarted
     when we resume the inferior on return from a function call from
     within GDB.  In all other cases the system call will not be
     restarted.  */
  write_register_pid (ARC_ORIG_R8_REGNUM, -3, ptid);
}


/* See the comments for SKIP_SOLIB_RESOLVER at the top of infrun.c.

   This is called on every single step thru the PLT and runtime resolver.

   This function:
   1) decides whether a PLT has sent us into the linker to resolve
      a function reference, and 
   2) if so, tells us where to set a temporary breakpoint that will
      trigger when the dynamic linker is done.  */

CORE_ADDR
arc_linux_skip_solib_resolver (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  /* For uClibc 0.9.26.

     An unresolved PLT entry points to "__dl_linux_resolve", which calls
     "__dl_linux_resolver" to do the resolving and then eventually jumps to
     the function.

     So we look for the symbol `_dl_linux_resolver', and if we are there,
     we set a breakpoint at the return address, and continue.  */

  /* lookup_minimal_symbol didn't work, for some reason.  */
  struct symbol *resolver 
    = lookup_symbol_global ("_dl_linux_resolver", 0, VAR_DOMAIN, 0);
  
#ifdef ARC_DEBUG
  printf("--- arc_linux_skip_solib_resolver: pc = %x, resolver at %x\n", pc,
	 resolver ? BLOCK_START (SYMBOL_BLOCK_VALUE (resolver)) : 0);
#endif
  
  if (resolver && (BLOCK_START (SYMBOL_BLOCK_VALUE (resolver))) == pc)
    {
	return frame_pc_unwind (get_current_frame ());
    }

  return 0;
}      


/* arcompact_linux_core_reg_offsets[i] is the offset in the .reg section of GDB
   regnum i .
   
   From include/asm-arc/user.h in the ARC Linux sources.  */
static int arcompact_linux_core_reg_offsets[] = {
  22 * 4,    /* r0 */
  21 * 4,    /* r1 */
  20 * 4,    /* r2 */
  19 * 4,    /* r3 */
  18 * 4,    /* r4 */
  17 * 4,    /* r5 */
  16 * 4,    /* r6 */
  15 * 4,    /* r7 */
  14 * 4,    /* r8 */
  13 * 4,    /* r9 */
  12 * 4,    /* r10 */
  11 * 4,    /* r11 */
  10 * 4,    /* r12 */
  39 * 4,    /* r13 */
  38 * 4,    /* r14 */
  37 * 4,    /* r15 */
  36 * 4,    /* r16 */
  35 * 4,    /* r17 */
  34 * 4,    /* r18 */
  33 * 4,    /* r19 */
  32 * 4,    /* r20 */
  31 * 4,    /* r21 */
  30 * 4,    /* r22 */
  29 * 4,    /* r23 */
  28 * 4,    /* r24 */
  27 * 4,    /* r25 */
   9 * 4,    /* r26 */
   1 * 4,    /* bta */
   2 * 4,    /* lp_start */
   3 * 4,    /* lp_end */
   4 * 4,    /* lp_count */
   5 * 4,    /* status32 */
   7 * 4,    /* blink */
   8 * 4,    /* fp */
  25 * 4,    /* sp */
  -1,        /* efa */
   6 * 4,    /* ret */
  24 * 4,    /* orig_r8 */
  40 * 4,    /* stop_pc */
};

/* Extract the register values found in the ABI GREGSET, storing their
   values in REGCACHE.  */
static void
arcompact_linux_supply_gregset (struct regcache *regcache,
				int regnum, const void *gregs, size_t size)
{
  int regi;
  int arc_num_gprs = ARRAY_SIZE (arcompact_linux_core_reg_offsets);
  const bfd_byte *buf = gregs;

  for (regi = 0; regi < arc_num_gprs; regi++)
    {
      if (arcompact_linux_core_reg_offsets[regi] > 0)
	regcache_raw_supply (regcache, regi,
			     buf + arcompact_linux_core_reg_offsets[regi]);
    }
}

/* Call the right architecture variant's supply_gregset function.  For now,
   we only have ARCompact.  */
static void
arc_linux_supply_gregset (const struct regset *regset,
			  struct regcache *regcache,
			  int regnum, const void *gregs, size_t size)
{
  arcompact_linux_supply_gregset (regcache, regnum, gregs, size);
}

/* Functions for handling core files.
   The first element is a parameter to pass the rest of the functions.  We
   don't need it.
   supply_gregset is for reading the core file.
   collect_regset, which we haven't defined, would be for writing the core
   file.  */
static struct regset arc_linux_gregset = {
  NULL, arc_linux_supply_gregset
};

/* This is called through gdbarch.  */
static const struct regset *
arc_linux_regset_from_core_section (struct gdbarch *core_arch,
				    const char *sect_name, size_t sect_size)
{
  if (strcmp (sect_name, ".reg") == 0)
    return &arc_linux_gregset;

  return NULL;
}

/* Add the signal stuff to gdbarch->tdep.  */
static void
arc_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  
  tdep->sigtramp_p = arc_linux_sigtramp_p;
  tdep->sigcontext_addr = arc_linux_sigcontext_addr;
  tdep->sc_reg_offset = arc_linux_sc_reg_offset;
  tdep->sc_num_regs = ARRAY_SIZE (arc_linux_sc_reg_offset);

  tdep->arc_breakpoint_size = arc700_linux_breakpoint_size;
  tdep->arc_breakpoint_insn = arc700_linux_breakpoint_insn;

  set_gdbarch_num_regs (gdbarch, ARC_NR_REGS);
  set_gdbarch_num_pseudo_regs (gdbarch, ARC_NR_PSEUDO_REGS);
  
  set_gdbarch_pc_regnum (gdbarch, ARC_STOP_PC_REGNUM);
  set_gdbarch_register_name (gdbarch, arc_linux_register_name);

  set_gdbarch_software_single_step (gdbarch, arc_software_single_step);

  set_gdbarch_write_pc (gdbarch, arc700_linux_write_pc);

  tdep->pc_regnum_in_sigcontext = ARC_RET_REGNUM;


  set_gdbarch_pseudo_register_read (gdbarch, arc_linux_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, arc_linux_pseudo_register_write);

  set_gdbarch_print_registers_info (gdbarch, arc_linux_print_registers_info);

  tdep->register_reggroup_p = arc_linux_register_reggroup_p;
  
  tdep->lowest_pc = 0x74;

  tdep->arc_processor_variant_info = NULL;
  set_gdbarch_regset_from_core_section (gdbarch, 
					arc_linux_regset_from_core_section);
  /* GNU/Linux uses SVR4-style shared libraries.  */
  set_solib_svr4_fetch_link_map_offsets
    (gdbarch, svr4_ilp32_fetch_link_map_offsets);

  set_gdbarch_skip_solib_resolver (gdbarch, arc_linux_skip_solib_resolver);
}

void
_initialize_arc_linux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_arc, 0, GDB_OSABI_LINUX,
			  arc_linux_init_abi);
}
