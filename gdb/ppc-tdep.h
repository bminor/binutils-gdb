/* Target-dependent code for GDB, the GNU debugger.
   Copyright 2000
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

/* From ppc-linux-tdep.c... */
CORE_ADDR ppc_linux_frame_saved_pc (struct frame_info *fi);
void ppc_linux_init_extra_frame_info (int fromleaf, struct frame_info *);
int ppc_linux_frameless_function_invocation (struct frame_info *);
void ppc_linux_frame_init_saved_regs (struct frame_info *);
CORE_ADDR ppc_linux_frame_chain (struct frame_info *);
CORE_ADDR ppc_sysv_abi_push_arguments (int, value_ptr *, CORE_ADDR, int,
				       CORE_ADDR);
int ppc_linux_memory_remove_breakpoint (CORE_ADDR addr, char *contents_cache);


/* From rs6000-tdep.c... */
CORE_ADDR rs6000_frame_saved_pc (struct frame_info *fi);
void rs6000_init_extra_frame_info (int fromleaf, struct frame_info *);
int rs6000_frameless_function_invocation (struct frame_info *);
void rs6000_frame_init_saved_regs (struct frame_info *);
CORE_ADDR rs6000_frame_chain (struct frame_info *);

/* Some important register numbers. */

#define	PPC_GP0_REGNUM 0		/* GPR register 0 */
#define	PPC_TOC_REGNUM 2		/* TOC register */
#define PPC_PS_REGNUM 65		/* Processor (or machine) status (%msr) */
#define	PPC_CR_REGNUM 66		/* Condition register */
#define	PPC_LR_REGNUM 67		/* Link register */
#define	PPC_CTR_REGNUM 68		/* Count register */
#define	PPC_XER_REGNUM 69		/* Integer exception register */
#define	PPC_MQ_REGNUM 70		/* Multiply/Divide extension register */
