// OBSOLETE /* Definitions to make GDB run on a Sequent Symmetry under ptx
// OBSOLETE    with Weitek 1167 and i387 support.
// OBSOLETE    Copyright 1986, 1987, 1989, 1992, 1994, 1996, 2000
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
// OBSOLETE #include "config/nm-sysv4.h"
// OBSOLETE 
// OBSOLETE #undef USE_PROC_FS
// OBSOLETE 
// OBSOLETE #include "i386/nm-symmetry.h"
// OBSOLETE 
// OBSOLETE #define PTRACE_READ_REGS(pid,regaddr) mptrace (XPT_RREGS, (pid), (regaddr), 0)
// OBSOLETE #define PTRACE_WRITE_REGS(pid,regaddr) \
// OBSOLETE   mptrace (XPT_WREGS, (pid), (regaddr), 0)
// OBSOLETE 
// OBSOLETE /* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */
// OBSOLETE 
// OBSOLETE #define FETCH_INFERIOR_REGISTERS
// OBSOLETE 
// OBSOLETE /* We must fetch all the regs before storing, since we store all at once.  */
// OBSOLETE 
// OBSOLETE #define CHILD_PREPARE_TO_STORE() deprecated_read_register_bytes (0, NULL, REGISTER_BYTES)
// OBSOLETE 
// OBSOLETE #define CHILD_WAIT
// OBSOLETE struct target_waitstatus;
// OBSOLETE extern ptid_t child_wait (ptid_t, struct target_waitstatus *);
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * ptx does attach as of ptx version 2.1.  Prior to that, the interface
// OBSOLETE  * exists but does not work.
// OBSOLETE  *
// OBSOLETE  * FIXME: Using attach/detach requires using the ptx MPDEBUGGER
// OBSOLETE  * interface.  There are still problems with that, so for now don't
// OBSOLETE  * enable attach/detach.  If you turn it on anyway, it will mostly
// OBSOLETE  * work, but has a number of bugs. -fubar, 2/94.
// OBSOLETE  */
// OBSOLETE /*#define ATTACH_DETACH 1 */
// OBSOLETE #undef ATTACH_DETACH
// OBSOLETE #define PTRACE_ATTACH XPT_DEBUG
// OBSOLETE #define PTRACE_DETACH XPT_UNDEBUG
// OBSOLETE /*
// OBSOLETE  * The following drivel is needed because there are two ptrace-ish
// OBSOLETE  * calls on ptx: ptrace() and mptrace(), each of which does about half
// OBSOLETE  * of the ptrace functions.
// OBSOLETE  */
// OBSOLETE #define PTRACE_ATTACH_CALL(pid)  ptx_do_attach(pid)
// OBSOLETE #define PTRACE_DETACH_CALL(pid, signo) ptx_do_detach(pid, signo)
