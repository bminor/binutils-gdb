// OBSOLETE /* Mach 3.0 common definitions and global vars.
// OBSOLETE 
// OBSOLETE    Copyright 1992, 1993, 1994, 1996 Free Software Foundation, Inc.
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
// OBSOLETE #ifndef NM_M3_H
// OBSOLETE #define NM_M3_H
// OBSOLETE 
// OBSOLETE #include <mach.h>
// OBSOLETE #include "regcache.h"
// OBSOLETE 
// OBSOLETE /* Mach3 doesn't declare errno in <errno.h>.  */
// OBSOLETE extern int errno;
// OBSOLETE 
// OBSOLETE /* Task port of our debugged inferior. */
// OBSOLETE 
// OBSOLETE extern task_t inferior_task;
// OBSOLETE 
// OBSOLETE /* Thread port of the current thread in the inferior. */
// OBSOLETE 
// OBSOLETE extern thread_t current_thread;
// OBSOLETE 
// OBSOLETE /* If nonzero, we must suspend/abort && resume threads
// OBSOLETE  * when setting or getting the state.
// OBSOLETE  */
// OBSOLETE extern int must_suspend_thread;
// OBSOLETE 
// OBSOLETE #define PREPARE_TO_PROCEED(select_it) mach3_prepare_to_proceed(select_it)
// OBSOLETE 
// OBSOLETE /* Try to get the privileged host port for authentication to machid
// OBSOLETE 
// OBSOLETE  * If you can get this, you may debug anything on this host.
// OBSOLETE  *
// OBSOLETE  * If you can't, gdb gives it's own task port as the
// OBSOLETE  * authentication port
// OBSOLETE  */
// OBSOLETE #define  mach_privileged_host_port() task_by_pid(-1)
// OBSOLETE 
// OBSOLETE /*
// OBSOLETE  * This is the MIG ID number of the emulator/server bsd_execve() RPC call.
// OBSOLETE  *
// OBSOLETE  * It SHOULD never change, but if it does, gdb `run'
// OBSOLETE  * command won't work until you fix this define.
// OBSOLETE  * 
// OBSOLETE  */
// OBSOLETE #define MIG_EXEC_SYSCALL_ID		101000
// OBSOLETE 
// OBSOLETE /* If our_message_port gets a msg with this ID,
// OBSOLETE  * GDB suspends it's inferior and enters command level.
// OBSOLETE  * (Useful at least if ^C does not work)
// OBSOLETE  */
// OBSOLETE #define GDB_MESSAGE_ID_STOP			0x41151
// OBSOLETE 
// OBSOLETE /* wait3 WNOHANG is defined in <sys/wait.h> but
// OBSOLETE  * for some reason gdb does not want to include
// OBSOLETE  * that file.
// OBSOLETE  *
// OBSOLETE  * If your system defines WNOHANG differently, this has to be changed.
// OBSOLETE  */
// OBSOLETE #define WNOHANG 1
// OBSOLETE 
// OBSOLETE /* Before storing, we need to read all the registers.  */
// OBSOLETE 
// OBSOLETE #define CHILD_PREPARE_TO_STORE() deprecated_read_register_bytes (0, NULL, REGISTER_BYTES)
// OBSOLETE 
// OBSOLETE /* Check if the inferior exists */
// OBSOLETE #define MACH_ERROR_NO_INFERIOR \
// OBSOLETE   do if (!MACH_PORT_VALID (inferior_task)) \
// OBSOLETE   	error ("Inferior task does not exist."); while(0)
// OBSOLETE 
// OBSOLETE /* Error handler for mach calls */
// OBSOLETE #define CHK(str,ret)	\
// OBSOLETE   do if (ret != KERN_SUCCESS) \
// OBSOLETE        error ("Gdb %s [%d] %s : %s\n",__FILE__,__LINE__,str, \
// OBSOLETE 	      mach_error_string(ret)); while(0)
// OBSOLETE 
// OBSOLETE /* This is from POE9 emulator/emul_stack.h
// OBSOLETE  */
// OBSOLETE /*
// OBSOLETE  * Top of emulator stack holds link and reply port.
// OBSOLETE  */
// OBSOLETE struct emul_stack_top
// OBSOLETE   {
// OBSOLETE     struct emul_stack_top *link;
// OBSOLETE     mach_port_t reply_port;
// OBSOLETE   };
// OBSOLETE 
// OBSOLETE #define EMULATOR_STACK_SIZE (4096*4)
// OBSOLETE 
// OBSOLETE #define THREAD_ALLOWED_TO_BREAK(mid) mach_thread_for_breakpoint (mid)
// OBSOLETE 
// OBSOLETE #define THREAD_PARSE_ID(arg) mach_thread_parse_id (arg)
// OBSOLETE 
// OBSOLETE #define THREAD_OUTPUT_ID(mid) mach_thread_output_id (mid)
// OBSOLETE 
// OBSOLETE #define ATTACH_TO_THREAD attach_to_thread
// OBSOLETE 
// OBSOLETE /* Don't do wait_for_inferior on attach.  */
// OBSOLETE #define ATTACH_NO_WAIT
// OBSOLETE 
// OBSOLETE /* Do Mach 3 dependent operations when ^C or a STOP is requested */
// OBSOLETE #define DO_QUIT() mach3_quit ()
// OBSOLETE 
// OBSOLETE #if 0
// OBSOLETE /* This is bogus.  It is NOT OK to quit out of target_wait.  */
// OBSOLETE /* If in mach_msg() and ^C is typed set immediate_quit */
// OBSOLETE #define REQUEST_QUIT() mach3_request_quit ()
// OBSOLETE #endif
// OBSOLETE 
// OBSOLETE #endif /* NM_M3_H */
