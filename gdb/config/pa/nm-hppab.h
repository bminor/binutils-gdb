// OBSOLETE /* HPPA PA-RISC machine native support for BSD, for GDB.
// OBSOLETE    Copyright 1991, 1992, 1993, 1994, 1995, 2002 Free Software Foundation, Inc.
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
// OBSOLETE #include "somsolib.h"
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE #define U_REGS_OFFSET 0
// OBSOLETE 
// OBSOLETE #define KERNEL_U_ADDR 0
// OBSOLETE 
// OBSOLETE /* What a coincidence! */
// OBSOLETE #define REGISTER_U_ADDR(addr, blockend, regno)				\
// OBSOLETE { addr = (int)(blockend) + REGISTER_BYTE (regno);}
// OBSOLETE 
// OBSOLETE /* 3rd argument to ptrace is supposed to be a caddr_t.  */
// OBSOLETE 
// OBSOLETE #define	PTRACE_ARG3_TYPE caddr_t
// OBSOLETE 
// OBSOLETE /* HPUX 8.0, in its infinite wisdom, has chosen to prototype ptrace
// OBSOLETE    with five arguments, so programs written for normal ptrace lose.  */
// OBSOLETE #define FIVE_ARG_PTRACE
// OBSOLETE 
// OBSOLETE 
// OBSOLETE /* fetch_inferior_registers is in hppab-nat.c.  */
// OBSOLETE #define FETCH_INFERIOR_REGISTERS
// OBSOLETE 
// OBSOLETE /* attach/detach works to some extent under BSD and HPUX.  So long
// OBSOLETE    as the process you're attaching to isn't blocked waiting on io,
// OBSOLETE    blocked waiting on a signal, or in a system call things work 
// OBSOLETE    fine.  (The problems in those cases are related to the fact that
// OBSOLETE    the kernel can't provide complete register information for the
// OBSOLETE    target process...  Which really pisses off GDB.)  */
// OBSOLETE 
// OBSOLETE #define ATTACH_DETACH
// OBSOLETE 
// OBSOLETE /* The PA-BSD kernel has support for using the data memory break bit
// OBSOLETE    to implement fast watchpoints.
// OBSOLETE 
// OBSOLETE    Watchpoints on the PA act much like traditional page protection
// OBSOLETE    schemes, but with some notable differences.
// OBSOLETE 
// OBSOLETE    First, a special bit in the page table entry is used to cause
// OBSOLETE    a trap when a specific page is written to.  This avoids having
// OBSOLETE    to overload watchpoints on the page protection bits.  This makes
// OBSOLETE    it possible for the kernel to easily decide if a trap was caused
// OBSOLETE    by a watchpoint or by the user writing to protected memory and can
// OBSOLETE    signal the user program differently in each case.  
// OBSOLETE 
// OBSOLETE    Second, the PA has a bit in the processor status word which causes
// OBSOLETE    data memory breakpoints (aka watchpoints) to be disabled for a single
// OBSOLETE    instruction.  This bit can be used to avoid the overhead of unprotecting
// OBSOLETE    and reprotecting pages when it becomes necessary to step over a watchpoint.
// OBSOLETE 
// OBSOLETE 
// OBSOLETE    When the kernel receives a trap indicating a write to a page which
// OBSOLETE    is being watched, the kernel performs a couple of simple actions.  First
// OBSOLETE    is sets the magic "disable memory breakpoint" bit in the processor
// OBSOLETE    status word, it then sends a SIGTRAP to the process which caused the
// OBSOLETE    trap.
// OBSOLETE 
// OBSOLETE    GDB will take control and catch the signal for the inferior.  GDB then
// OBSOLETE    examines the PSW-X bit to determine if the SIGTRAP was caused by a 
// OBSOLETE    watchpoint firing.  If so GDB single steps the inferior over the
// OBSOLETE    instruction which caused the watchpoint to trigger (note because the
// OBSOLETE    kernel disabled the data memory break bit for one instruction no trap
// OBSOLETE    will be taken!).  GDB will then determines the appropriate action to
// OBSOLETE    take.  (this may include restarting the inferior if the watchpoint
// OBSOLETE    fired because of a write to an address on the same page as a watchpoint,
// OBSOLETE    but no write to the watched address occured).  */
// OBSOLETE 
// OBSOLETE #define TARGET_HAS_HARDWARE_WATCHPOINTS		/* Enable the code in procfs.c */
// OBSOLETE 
// OBSOLETE /* The PA can watch any number of locations, there's no need for it to reject
// OBSOLETE    anything (generic routines already check that all intermediates are
// OBSOLETE    in memory).  */
// OBSOLETE #define TARGET_CAN_USE_HARDWARE_WATCHPOINT(type, cnt, ot) \
// OBSOLETE 	((type) == bp_hardware_watchpoint)
// OBSOLETE 
// OBSOLETE /* When a hardware watchpoint fires off the PC will be left at the
// OBSOLETE    instruction which caused the watchpoint.  It will be necessary for
// OBSOLETE    GDB to step over the watchpoint.
// OBSOLETE 
// OBSOLETE    On a PA running BSD, it is trivial to identify when it will be
// OBSOLETE    necessary to step over a hardware watchpoint as we can examine
// OBSOLETE    the PSW-X bit.  If the bit is on, then we trapped because of a 
// OBSOLETE    watchpoint, else we trapped for some other reason.  */
// OBSOLETE #define STOPPED_BY_WATCHPOINT(W) \
// OBSOLETE   ((W).kind == TARGET_WAITKIND_STOPPED \
// OBSOLETE    && (W).value.sig == TARGET_SIGNAL_TRAP \
// OBSOLETE    && ((int) read_register (IPSW_REGNUM) & 0x00100000))
// OBSOLETE 
// OBSOLETE /* The PA can single step over a watchpoint if the kernel has set the
// OBSOLETE    "X" bit in the processor status word (disable data memory breakpoint
// OBSOLETE    for one instruction).
// OBSOLETE 
// OBSOLETE    The kernel will always set this bit before notifying the inferior
// OBSOLETE    that it hit a watchpoint.  Thus, the inferior can single step over
// OBSOLETE    the instruction which caused the watchpoint to fire.  This avoids
// OBSOLETE    the traditional need to disable the watchpoint, step the inferior,
// OBSOLETE    then enable the watchpoint again.  */
// OBSOLETE #define HAVE_STEPPABLE_WATCHPOINT
// OBSOLETE 
// OBSOLETE /* Use these macros for watchpoint insertion/deletion.  */
// OBSOLETE /* type can be 0: write watch, 1: read watch, 2: access watch (read/write) */
// OBSOLETE #define target_insert_watchpoint(addr, len, type) hppa_set_watchpoint (addr, len, 1)
// OBSOLETE #define target_remove_watchpoint(addr, len, type) hppa_set_watchpoint (addr, len, 0)
