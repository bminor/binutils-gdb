/* Interface between GDB and target environments, including files and processes
   Copyright 1990, 1991 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by John Gilmore.

This file is part of GDB.

GDB is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GDB is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GDB; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This include file defines the interface between the main part
   of the debugger, and the part which is target-specific, or
   specific to the communications interface between us and the
   target.

   A TARGET is an interface between the debugger and a particular 
   kind of file or process.  Targets can be STACKED in STRATA, 
   so that more than one target can potentially respond to a request.
   In particular, memory accesses will walk down the stack of targets
   until they find a target that is interested in handling that particular
   address.  STRATA are artificial boundaries on the stack, within
   which particular kinds of targets live.  Strata exist so that
   people don't get confused by pushing e.g. a process target and then
   a file target, and wondering why they can't see the current values
   of variables any more (the file target is handling them and they
   never get to the process target).  So when you push a file target,
   it goes into the file stratum, which is always below the process
   stratum.  */

enum strata {
	dummy_stratum,		/* The lowest of the low */
	file_stratum,		/* Executable files, etc */
	core_stratum,		/* Core dump files */
	process_stratum,	/* Executing processes */
};

struct target_ops {
	char  *to_shortname;		/* Name this target type */
	char  *to_longname;		/* Name for printing */
	/* Documentation.  Does not include trailing newline, and
	   starts with a one-line description (probably similar to
	   to_longname).  */
	char  *to_doc;
#ifdef __STDC__
	void (*to_open) (char *name, int from_tty);
	void (*to_close) (int quitting);
	void (*to_attach) (char *name, int from_tty);
	void (*to_detach) (char *args, int from_tty);
	void (*to_resume) (int step, int siggnal);
	int  (*to_wait)   (int *status);
	int  (*to_fetch_registers) (int regno);
	int  (*to_store_registers) (int regno);
	void (*to_prepare_to_store) ();
	void (*to_convert_to_virtual) (int regnum, char *from, char *to);
	void (*to_convert_from_virtual) (int regnum, char *from, char *to);
	int  (*to_xfer_memory) (CORE_ADDR memaddr, char *myaddr, int len, int w);
	void (*to_files_info) ();
	int  (*to_insert_breakpoint) (CORE_ADDR addr, char *save);
	int  (*to_remove_breakpoint) (CORE_ADDR addr, char *save);
	void  (*to_terminal_init) ();
	void  (*to_terminal_inferior) ();
	void  (*to_terminal_ours_for_output) ();
	void  (*to_terminal_ours) ();
	void  (*to_terminal_info) (char *arg, int from_tty);
	void  (*to_kill) (char *arg, int from_tty);
	void  (*to_load) (char *arg, int from_tty);
	void  (*to_add_syms) (char *arg, int from_tty);
struct value *(*to_call_function) (struct value *function,
				   int nargs, struct value **args);
	int   (*to_lookup_symbol) (char *name, CORE_ADDR *addrp);
	void  (*to_create_inferior) (char *exec, char *args, char **env);
	void  (*to_mourn_inferior) ();
    enum strata to_stratum;
struct target_ops *to_next;
	int	to_has_all_memory;
	int	to_has_memory;
	int	to_has_stack;
	int	to_has_registers;
	int	to_has_execution;
	int     to_magic;
/* Need sub-structure for target machine related rather than comm related? */
#else  /* STDC */
	void (*to_open) ();
	void (*to_close) ();
	void (*to_attach) ();
	void (*to_detach) ();
	void (*to_resume) ();
	int  (*to_wait)   ();
	int  (*to_fetch_registers) ();
	int  (*to_store_registers) ();
	void (*to_prepare_to_store) ();
	void (*to_convert_to_virtual) ();
	void (*to_convert_from_virtual) ();
	int  (*to_xfer_memory) ();
	void (*to_files_info) ();
	int  (*to_insert_breakpoint) ();
	int  (*to_remove_breakpoint) ();
	void  (*to_terminal_init) ();
	void  (*to_terminal_inferior) ();
	void  (*to_terminal_ours_for_output) ();
	void  (*to_terminal_ours) ();
	void  (*to_terminal_info) ();
	void  (*to_kill) ();
	void  (*to_load) ();
	void  (*to_add_syms) ();
struct value *(*to_call_function) ();
	int   (*to_lookup_symbol) ();
	void  (*to_create_inferior) ();
	void  (*to_mourn_inferior) ();
    enum strata to_stratum;
struct target_ops *to_next;
	int	to_has_all_memory;
	int	to_has_memory;
	int	to_has_stack;
	int	to_has_registers;
	int	to_has_execution;
	int     to_magic;
/* Need sub-structure for target machine related rather than comm related? */
#endif
};

/* Magic number for checking ops size.  If a struct doesn't end with this
   number, somebody changed the declaration but didn't change all the
   places that initialize one.  */

#define	OPS_MAGIC	3840

/* The ops structure for our "current" target process.  */

extern struct target_ops	*current_target;

/* Define easy words for doing these operations on our current target.  */

#define	target_shortname	(current_target->to_shortname)
#define	target_longname		(current_target->to_longname)

/* The open routine takes the rest of the parameters from the command,
   and (if successful) pushes a new target onto the stack.
   Targets should supply this routine, if only to provide an error message.  */
#define	target_open(name, from_tty)	\
	(*current_target->to_open) (name, from_tty)

/* Does whatever cleanup is required for a target that we are no longer
   going to be calling.  Argument says whether we are quitting gdb and
   should not get hung in case of errors, or whether we want a clean
   termination even if it takes a while.  This routine is automatically
   always called just before a routine is popped off the target stack.
   Closing file descriptors and freeing memory are typical things it should
   do.  */

#define	target_close(quitting)	\
	(*current_target->to_close) (quitting)

/* Attaches to a process on the target side.  */

#define	target_attach(args, from_tty)	\
	(*current_target->to_attach) (args, from_tty)

/* Takes a program previously attached to and detaches it.
   The program may resume execution (some targets do, some don't) and will
   no longer stop on signals, etc.  We better not have left any breakpoints
   in the program or it'll die when it hits one.  ARGS is arguments
   typed by the user (e.g. a signal to send the process).  FROM_TTY
   says whether to be verbose or not.  */

#define	target_detach(args, from_tty)		\
	(*current_target->to_detach) (args, from_tty)

/* Resume execution of the target process.  STEP says whether to single-step
   or to run free; SIGGNAL is the signal value (e.g. SIGINT) to be given
   to the target, or zero for no signal.  */

#define	target_resume(step, siggnal)	\
	(*current_target->to_resume) (step, siggnal)

/* Wait for inferior process to do something.  Return pid of child,
   or -1 in case of error; store status through argument pointer STATUS.  */

#define	target_wait(status)		\
	(*current_target->to_wait) (status)

/* Fetch register REGNO, or all regs if regno == -1.  Result is 0
   for success, -1 for problems.  */

#define	target_fetch_registers(regno)	\
	(*current_target->to_fetch_registers) (regno)

/* Store at least register REGNO, or all regs if REGNO == -1.
   It can store as many registers as it wants to, so the entire registers
   array must be valid.  Result is 0 for success, -1 for problems.  */

#define	target_store_registers(regs)	\
	(*current_target->to_store_registers) (regs)

/* Get ready to modify the registers array.  On machines which store
   individual registers, this doesn't need to do anything.  On machines
   which store all the registers in one fell swoop, this makes sure
   that REGISTERS contains all the registers from the program being
   debugged.  */

#define	target_prepare_to_store()	\
	(*current_target->to_prepare_to_store) ()

/* Convert data from raw format for register REGNUM
   to virtual format for register REGNUM.  */

#define	target_convert_to_virtual(regnum, from, to)	\
	(*current_target->to_convert_to_virtual) (regnum, from, to)
	
/* Convert data from virtual format for register REGNUM
   to raw format for register REGNUM.  */

#define	target_convert_from_virtual(regnum, from, to)	\
	(*current_target->to_convert_from_virtual) (regnum, from, to)

/* Reading and writing memory actually happens through a glue
   function which iterates across the various targets.  Result is
   0 for success, or an errno value.  */

#ifdef __STDC__
/* Needs defs.h for CORE_ADDR */
extern int target_read_memory(CORE_ADDR memaddr, char *myaddr, int len);
extern int target_write_memory(CORE_ADDR memaddr, char *myaddr, int len);
extern int target_xfer_memory(CORE_ADDR memaddr, char *myaddr, int len,
			      int write);
#else
extern int target_read_memory();
extern int target_write_memory();
extern int target_xfer_memory();
#endif

/* Print a line about the current target.  */

#define	target_files_info()	\
	(*current_target->to_files_info) ()

/* Insert a breakpoint at address ADDR in the target machine.
   SAVE is a pointer to memory allocated for saving the
   target contents.  It is guaranteed by the caller to be long enough
   to save "sizeof BREAKPOINT" bytes.  Result is 0 for success, or
   an errno value.  */

#define	target_insert_breakpoint(addr, save)	\
	(*current_target->to_insert_breakpoint) (addr, save)

/* Remove a breakpoint at address ADDR in the target machine.
   SAVE is a pointer to the same save area 
   that was previously passed to target_insert_breakpoint.  
   Result is 0 for success, or an errno value.  */

#define	target_remove_breakpoint(addr, save)	\
	(*current_target->to_remove_breakpoint) (addr, save)

/* Initialize the terminal settings we record for the inferior,
   before we actually run the inferior.  */

#define target_terminal_init() \
	(*current_target->to_terminal_init) ()
	
/* Put the inferior's terminal settings into effect.
   This is preparation for starting or resuming the inferior.  */

#define target_terminal_inferior() \
	(*current_target->to_terminal_inferior) ()

/* Put some of our terminal settings into effect,
   enough to get proper results from our output,
   but do not change into or out of RAW mode
   so that no input is discarded.

   After doing this, either terminal_ours or terminal_inferior
   should be called to get back to a normal state of affairs.  */

#define target_terminal_ours_for_output() \
	(*current_target->to_terminal_ours_for_output) ()

/* Put our terminal settings into effect.
   First record the inferior's terminal settings
   so they can be restored properly later.  */

#define target_terminal_ours() \
	(*current_target->to_terminal_ours) ()

/* Print useful information about our terminal status, if such a thing
   exists.  */

#define target_terminal_info(arg, from_tty) \
	(*current_target->to_terminal_info) (arg, from_tty)

/* Kill the inferior process.   Make it go away.  */

#define target_kill(arg, from_tty) \
	(*current_target->to_kill) (arg, from_tty)

/* Load an executable file into the target process.  This is expected to
   not only bring new code into the target process, but also to update
   GDB's symbol tables to match.  */

#define target_load(arg, from_tty) \
	(*current_target->to_load) (arg, from_tty)

/* Add the symbols from an executable file into GDB's symbol table, as if
   the file had been loaded at a particular address (or set of addresses).
   This does not change any state in the target system, only in GDB.  */

#define target_add_syms(arg, from_tty) \
	(*current_target->to_add_syms) (arg, from_tty)

/* Perform a function call in the inferior.
   ARGS is a vector of values of arguments (NARGS of them).
   FUNCTION is a value, the function to be called.
   Returns a value representing what the function returned.
   May fail to return, if a breakpoint or signal is hit
   during the execution of the function.  */

#define target_call_function(function, nargs, args) 	\
  (*current_target->to_call_function) (function, nargs, args)

/* Look up a symbol in the target's symbol table.  NAME is the symbol
   name.  ADDRP is a CORE_ADDR * pointing to where the value of the symbol
   should be returned.  The result is 0 if successful, nonzero if the
   symbol does not exist in the target environment.  This function should
   not call error() if communication with the target is interrupted, since
   it is called from symbol reading, but should return nonzero, possibly
   doing a complain().  */

#define target_lookup_symbol(name, addrp) 	\
  (*current_target->to_lookup_symbol) (name, addrp)

/* Start an inferior process and set inferior_pid to its pid.
   EXEC_FILE is the file to run.
   ALLARGS is a string containing the arguments to the program.
   ENV is the environment vector to pass.  Errors reported with error().
   On VxWorks and various standalone systems, we ignore exec_file.  */
 
#define	target_create_inferior(exec_file, args, env)	\
	(*current_target->to_create_inferior) (exec_file, args, env)

/* The inferior process has died.  Do what is right.  */

#define	target_mourn_inferior()	\
	(*current_target->to_mourn_inferior) ()

/* Pointer to next target in the chain, e.g. a core file and an exec file.  */

#define	target_next \
	(current_target->to_next)

/* Does the target include all of memory, or only part of it?  This
   determines whether we look up the target chain for other parts of
   memory if this target can't satisfy a request.  */

#define	target_has_all_memory	\
	(current_target->to_has_all_memory)

/* Does the target include memory?  (Dummy targets don't.)  */

#define	target_has_memory	\
	(current_target->to_has_memory)

/* Does the target have a stack?  (Exec files don't, VxWorks doesn't, until
   we start a process.)  */
   
#define	target_has_stack	\
	(current_target->to_has_stack)

/* Does the target have registers?  (Exec files don't.)  */

#define	target_has_registers	\
	(current_target->to_has_registers)

/* Does the target have execution?  Can we make it jump (through hoops),
   or pop its stack a few times, or set breakpoints?  */

#define	target_has_execution	\
	(current_target->to_has_execution)

/* Routines for maintenance of the target structures...

   add_target:   Add a target to the list of all possible targets.

   push_target:  Make this target the top of the stack of currently used
		 targets, within its particular stratum of the stack.  Result
		 is 0 if now atop the stack, nonzero if not on top (maybe
		 should warn user).

   unpush_target: Remove this from the stack of currently used targets,
		 no matter where it is on the list.  Returns 0 if no
		 change, 1 if removed from stack.

   pop_target:	 Remove the top thing on the stack of current targets.  */

#ifdef __STDC__
void add_target (struct target_ops *);
int push_target (struct target_ops *);
int unpush_target (struct target_ops *);
void target_preopen (int);
void pop_target (void);
#else
void add_target ();
int push_target ();
int unpush_target ();
void target_preopen ();
void pop_target ();
#endif
