// OBSOLETE /* Macro definitions for GDB for a Fujitsu SPARClite.
// OBSOLETE    Copyright 1993, 1994, 1995, 1998, 1999, 2000, 2001, 2002, 2003
// OBSOLETE    Free Software Foundation, Inc.
// OBSOLETE 
// OBSOLETE    This file is part of GDB.
// OBSOLETE 
// OBSOLETE    This program is free software; you can redistribute it and/or modify
// OBSOLETE    it under the terms of the GNU General Public License as published by
// OBSOLETE    the Free Software Foundation; either version 2 of the License, or
// OBSOLETE    (at your option) any later version.
// OBSOLETE 
// OBSOLETE    This program is distributed in the hope that it will be useful,
// OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of
// OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// OBSOLETE    GNU General Public License for more details.
// OBSOLETE 
// OBSOLETE    You should have received a copy of the GNU General Public License
// OBSOLETE    along with this program; if not, write to the Free Software
// OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330,
// OBSOLETE    Boston, MA 02111-1307, USA.  */
// OBSOLETE 
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE #define TARGET_SPARCLITE 1	/* Still needed for non-multi-arch case */
// OBSOLETE 
// OBSOLETE #include "sparc/tm-sparc.h"
// OBSOLETE 
// OBSOLETE /* Note: we are not defining GDB_MULTI_ARCH for the sparclet target
// OBSOLETE    at this time, because we have not figured out how to detect the
// OBSOLETE    sparclet target from the bfd structure.  */
// OBSOLETE 
// OBSOLETE /* Sparclite regs, for debugging purposes */
// OBSOLETE 
// OBSOLETE enum {
// OBSOLETE   DIA1_REGNUM = 72,		/* debug instr address register 1 */
// OBSOLETE   DIA2_REGNUM = 73,		/* debug instr address register 2 */
// OBSOLETE   DDA1_REGNUM = 74,		/* debug data address register 1 */
// OBSOLETE   DDA2_REGNUM = 75,		/* debug data address register 2 */
// OBSOLETE   DDV1_REGNUM = 76,		/* debug data value register 1 */
// OBSOLETE   DDV2_REGNUM = 77,		/* debug data value register 2 */
// OBSOLETE   DCR_REGNUM  = 78,		/* debug control register */
// OBSOLETE   DSR_REGNUM  = 79		/* debug status regsiter */
// OBSOLETE };
// OBSOLETE 
// OBSOLETE /* overrides of tm-sparc.h */
// OBSOLETE 
// OBSOLETE #undef TARGET_BYTE_ORDER
// OBSOLETE 
// OBSOLETE /* Select the sparclite disassembler.  Slightly different instruction set from
// OBSOLETE    the V8 sparc.  */
// OBSOLETE 
// OBSOLETE #undef TM_PRINT_INSN_MACH
// OBSOLETE #define TM_PRINT_INSN_MACH bfd_mach_sparc_sparclite
// OBSOLETE 
// OBSOLETE /* Amount PC must be decremented by after a hardware instruction breakpoint.
// OBSOLETE    This is often the number of bytes in BREAKPOINT
// OBSOLETE    but not always.  */
// OBSOLETE 
// OBSOLETE #define DECR_PC_AFTER_HW_BREAK 4
// OBSOLETE 
// OBSOLETE #if !defined (GDB_MULTI_ARCH) || (GDB_MULTI_ARCH == 0)
// OBSOLETE /*
// OBSOLETE  * The following defines must go away for MULTI_ARCH.
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE #undef NUM_REGS
// OBSOLETE #define NUM_REGS 80
// OBSOLETE 
// OBSOLETE #undef REGISTER_BYTES
// OBSOLETE #define REGISTER_BYTES (32*4+32*4+8*4+8*4)
// OBSOLETE 
// OBSOLETE #undef REGISTER_NAMES
// OBSOLETE #define REGISTER_NAMES  \
// OBSOLETE { "g0", "g1", "g2", "g3", "g4", "g5", "g6", "g7",       \
// OBSOLETE   "o0", "o1", "o2", "o3", "o4", "o5", "sp", "o7",       \
// OBSOLETE   "l0", "l1", "l2", "l3", "l4", "l5", "l6", "l7",       \
// OBSOLETE   "i0", "i1", "i2", "i3", "i4", "i5", "fp", "i7",       \
// OBSOLETE                                                                 \
// OBSOLETE   "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",       \
// OBSOLETE   "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15", \
// OBSOLETE   "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",       \
// OBSOLETE   "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",       \
// OBSOLETE                                                                 \
// OBSOLETE   "y", "psr", "wim", "tbr", "pc", "npc", "fpsr", "cpsr",        \
// OBSOLETE   "dia1", "dia2", "dda1", "dda2", "ddv1", "ddv2", "dcr", "dsr" }
// OBSOLETE 
// OBSOLETE #define DIA1_REGNUM 72		/* debug instr address register 1 */
// OBSOLETE #define DIA2_REGNUM 73		/* debug instr address register 2 */
// OBSOLETE #define DDA1_REGNUM 74		/* debug data address register 1 */
// OBSOLETE #define DDA2_REGNUM 75		/* debug data address register 2 */
// OBSOLETE #define DDV1_REGNUM 76		/* debug data value register 1 */
// OBSOLETE #define DDV2_REGNUM 77		/* debug data value register 2 */
// OBSOLETE #define DCR_REGNUM 78		/* debug control register */
// OBSOLETE #define DSR_REGNUM 79		/* debug status regsiter */
// OBSOLETE 
// OBSOLETE #endif /* GDB_MULTI_ARCH */
// OBSOLETE 
// OBSOLETE #define TARGET_HW_BREAK_LIMIT 2
// OBSOLETE #define TARGET_HW_WATCH_LIMIT 2
// OBSOLETE 
// OBSOLETE /* Enable watchpoint macro's */
// OBSOLETE 
// OBSOLETE #define TARGET_HAS_HARDWARE_WATCHPOINTS
// OBSOLETE 
// OBSOLETE #define TARGET_CAN_USE_HARDWARE_WATCHPOINT(type, cnt, ot) \
// OBSOLETE 	sparclite_check_watch_resources (type, cnt, ot)
// OBSOLETE 
// OBSOLETE /* When a hardware watchpoint fires off the PC will be left at the
// OBSOLETE    instruction which caused the watchpoint.  It will be necessary for
// OBSOLETE    GDB to step over the watchpoint. ***
// OBSOLETE 
// OBSOLETE    #define STOPPED_BY_WATCHPOINT(W) \
// OBSOLETE    ((W).kind == TARGET_WAITKIND_STOPPED \
// OBSOLETE    && (W).value.sig == TARGET_SIGNAL_TRAP \
// OBSOLETE    && ((int) read_register (IPSW_REGNUM) & 0x00100000))
// OBSOLETE  */
// OBSOLETE 
// OBSOLETE /* Use these macros for watchpoint insertion/deletion.  */
// OBSOLETE #define target_insert_watchpoint(addr, len, type) sparclite_insert_watchpoint (addr, len, type)
// OBSOLETE #define target_remove_watchpoint(addr, len, type) sparclite_remove_watchpoint (addr, len, type)
// OBSOLETE #define target_insert_hw_breakpoint(addr, len) sparclite_insert_hw_breakpoint (addr, len)
// OBSOLETE #define target_remove_hw_breakpoint(addr, len) sparclite_remove_hw_breakpoint (addr, len)
// OBSOLETE #define target_stopped_data_address() sparclite_stopped_data_address()
