/* Target-dependent code for GDB, the GNU debugger.

   Copyright 2000, 2001, 2002, 2003, 2004 Free Software Foundation,
   Inc.

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

#ifndef PPC_TDEP_H
#define PPC_TDEP_H

struct gdbarch;
struct frame_info;
struct value;
struct regcache;
struct type;

/* From ppc-linux-tdep.c... */
enum return_value_convention ppc_sysv_abi_return_value (struct gdbarch *gdbarch,
							struct type *valtype,
							struct regcache *regcache,
							void *readbuf,
							const void *writebuf);
enum return_value_convention ppc_sysv_abi_broken_return_value (struct gdbarch *gdbarch,
							       struct type *valtype,
							       struct regcache *regcache,
							       void *readbuf,
							       const void *writebuf);
CORE_ADDR ppc_sysv_abi_push_dummy_call (struct gdbarch *gdbarch,
					CORE_ADDR func_addr,
					struct regcache *regcache,
					CORE_ADDR bp_addr, int nargs,
					struct value **args, CORE_ADDR sp,
					int struct_return,
					CORE_ADDR struct_addr);
CORE_ADDR ppc64_sysv_abi_push_dummy_call (struct gdbarch *gdbarch,
					  CORE_ADDR func_addr,
					  struct regcache *regcache,
					  CORE_ADDR bp_addr, int nargs,
					  struct value **args, CORE_ADDR sp,
					  int struct_return,
					  CORE_ADDR struct_addr);
CORE_ADDR ppc64_sysv_abi_adjust_breakpoint_address (struct gdbarch *gdbarch,
						    CORE_ADDR bpaddr);
int ppc_linux_memory_remove_breakpoint (CORE_ADDR addr, char *contents_cache);
struct link_map_offsets *ppc_linux_svr4_fetch_link_map_offsets (void);
void ppc_linux_supply_gregset (struct regcache *regcache,
			       int regnum, const void *gregs, size_t size,
			       int wordsize);
void ppc_linux_supply_fpregset (const struct regset *regset,
				struct regcache *regcache,
				int regnum, const void *gregs, size_t size);

enum return_value_convention ppc64_sysv_abi_return_value (struct gdbarch *gdbarch,
							  struct type *valtype,
							  struct regcache *regcache,
							  void *readbuf,
							  const void *writebuf);

/* From rs6000-tdep.c... */
int altivec_register_p (int regno);
int spe_register_p (int regno);

/* Return non-zero if the architecture described by GDBARCH has
   floating-point registers (f0 --- f31 and fpscr).  */
int ppc_floating_point_unit_p (struct gdbarch *gdbarch);

/* Register set description.  */

struct ppc_reg_offsets
{
  /* General-purpose registers.  */
  int r0_offset;
  int pc_offset;
  int ps_offset;
  int cr_offset;
  int lr_offset;
  int ctr_offset;
  int xer_offset;
  int mq_offset;

  /* Floating-point registers.  */
  int f0_offset;
  int fpscr_offset;

  /* AltiVec registers.  */
  int vr0_offset;
  int vscr_offset;
  int vrsave_offset;
};

/* Supply register REGNUM in the general-purpose register set REGSET
   from the buffer specified by GREGS and LEN to register cache
   REGCACHE.  If REGNUM is -1, do this for all registers in REGSET.  */

extern void ppc_supply_gregset (const struct regset *regset,
				struct regcache *regcache,
				int regnum, const void *gregs, size_t len);

/* Supply register REGNUM in the floating-point register set REGSET
   from the buffer specified by FPREGS and LEN to register cache
   REGCACHE.  If REGNUM is -1, do this for all registers in REGSET.  */

extern void ppc_supply_fpregset (const struct regset *regset,
				 struct regcache *regcache,
				 int regnum, const void *fpregs, size_t len);

/* Collect register REGNUM in the general-purpose register set
   REGSET. from register cache REGCACHE into the buffer specified by
   GREGS and LEN.  If REGNUM is -1, do this for all registers in
   REGSET.  */

extern void ppc_collect_gregset (const struct regset *regset,
				 const struct regcache *regcache,
				 int regnum, void *gregs, size_t len);

/* Collect register REGNUM in the floating-point register set
   REGSET. from register cache REGCACHE into the buffer specified by
   FPREGS and LEN.  If REGNUM is -1, do this for all registers in
   REGSET.  */

extern void ppc_collect_fpregset (const struct regset *regset,
				  const struct regcache *regcache,
				  int regnum, void *fpregs, size_t len);

/* Private data that this module attaches to struct gdbarch. */

struct gdbarch_tdep
  {
    int wordsize;              /* size in bytes of fixed-point word */
    const struct reg *regs;    /* from current variant */
    int ppc_gp0_regnum;		/* GPR register 0 */
    int ppc_toc_regnum;		/* TOC register */
    int ppc_ps_regnum;	        /* Processor (or machine) status (%msr) */
    int ppc_cr_regnum;		/* Condition register */
    int ppc_lr_regnum;		/* Link register */
    int ppc_ctr_regnum;		/* Count register */
    int ppc_xer_regnum;		/* Integer exception register */

    /* On PPC and RS6000 variants that have no floating-point
       registers, the next two members will be -1.  */
    int ppc_fp0_regnum;         /* floating-point register 0 */
    int ppc_fpscr_regnum;	/* Floating point status and condition
    				   register */

    int ppc_mq_regnum;		/* Multiply/Divide extension register */
    int ppc_vr0_regnum;		/* First AltiVec register */
    int ppc_vrsave_regnum;	/* Last AltiVec register */
    int ppc_ev0_regnum;         /* First ev register */
    int ppc_ev31_regnum;        /* Last ev register */
    int ppc_acc_regnum;         /* SPE 'acc' register */
    int ppc_spefscr_regnum;     /* SPE 'spefscr' register */
    int lr_frame_offset;	/* Offset to ABI specific location where
                                   link register is saved.  */
};


/* Constants for register set sizes.  */
enum
  {
    ppc_num_gprs = 32,          /* 32 general-purpose registers */
    ppc_num_fprs = 32           /* 32 floating-point registers */
  };

#endif
