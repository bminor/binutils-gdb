/* Target operations for the remote server for GDB.
   Copyright (C) 2002, 2003, 2004, 2005, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

   Contributed by MontaVista Software.

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

#ifndef TARGET_H
#define TARGET_H

/* Ways to "resume" a thread.  */

enum resume_kind
{
  /* Thread should continue.  */
  resume_continue,

  /* Thread should single-step.  */
  resume_step,

  /* Thread should be stopped.  */
  resume_stop
};

/* This structure describes how to resume a particular thread (or all
   threads) based on the client's request.  If thread is -1, then this
   entry applies to all threads.  These are passed around as an
   array.  */

struct thread_resume
{
  ptid_t thread;

  /* How to "resume".  */
  enum resume_kind kind;

  /* If non-zero, send this signal when we resume, or to stop the
     thread.  If stopping a thread, and this is 0, the target should
     stop the thread however it best decides to (e.g., SIGSTOP on
     linux; SuspendThread on win32).  */
  int sig;
};

/* Generally, what has the program done?  */
enum target_waitkind
  {
    /* The program has exited.  The exit status is in
       value.integer.  */
    TARGET_WAITKIND_EXITED,

    /* The program has stopped with a signal.  Which signal is in
       value.sig.  */
    TARGET_WAITKIND_STOPPED,

    /* The program has terminated with a signal.  Which signal is in
       value.sig.  */
    TARGET_WAITKIND_SIGNALLED,

    /* The program is letting us know that it dynamically loaded
       something.  */
    TARGET_WAITKIND_LOADED,

    /* The program has exec'ed a new executable file.  The new file's
       pathname is pointed to by value.execd_pathname.  */
    TARGET_WAITKIND_EXECD,

    /* Nothing of interest to GDB happened, but we stopped anyway.  */
    TARGET_WAITKIND_SPURIOUS,

    /* An event has occurred, but we should wait again.  In this case,
       we want to go back to the event loop and wait there for another
       event from the inferior.  */
    TARGET_WAITKIND_IGNORE
  };

struct target_waitstatus
  {
    enum target_waitkind kind;

    /* Forked child pid, execd pathname, exit status or signal number.  */
    union
      {
	int integer;
	enum target_signal sig;
	ptid_t related_pid;
	char *execd_pathname;
      }
    value;
  };

/* Options that can be passed to target_ops->wait.  */

#define TARGET_WNOHANG 1

struct target_ops
{
  /* Start a new process.

     PROGRAM is a path to the program to execute.
     ARGS is a standard NULL-terminated array of arguments,
     to be passed to the inferior as ``argv''.

     Returns the new PID on success, -1 on failure.  Registers the new
     process with the process list.  */

  int (*create_inferior) (char *program, char **args);

  /* Attach to a running process.

     PID is the process ID to attach to, specified by the user
     or a higher layer.

     Returns -1 if attaching is unsupported, 0 on success, and calls
     error() otherwise.  */

  int (*attach) (unsigned long pid);

  /* Kill inferior PID.  Return -1 on failure, and 0 on success.  */

  int (*kill) (int pid);

  /* Detach from inferior PID. Return -1 on failure, and 0 on
     success.  */

  int (*detach) (int pid);

  /* Wait for inferior PID to exit.  */
  void (*join) (int pid);

  /* Return 1 iff the thread with process ID PID is alive.  */

  int (*thread_alive) (ptid_t pid);

  /* Resume the inferior process.  */

  void (*resume) (struct thread_resume *resume_info, size_t n);

  /* Wait for the inferior process or thread to change state.  Store
     status through argument pointer STATUS.

     PTID = -1 to wait for any pid to do something, PTID(pid,0,0) to
     wait for any thread of process pid to do something.  Return ptid
     of child, or -1 in case of error; store status through argument
     pointer STATUS.  OPTIONS is a bit set of options defined as
     TARGET_W* above.  If options contains TARGET_WNOHANG and there's
     no child stop to report, return is
     null_ptid/TARGET_WAITKIND_IGNORE.  */

  ptid_t (*wait) (ptid_t ptid, struct target_waitstatus *status, int options);

  /* Fetch registers from the inferior process.

     If REGNO is -1, fetch all registers; otherwise, fetch at least REGNO.  */

  void (*fetch_registers) (int regno);

  /* Store registers to the inferior process.

     If REGNO is -1, store all registers; otherwise, store at least REGNO.  */

  void (*store_registers) (int regno);

  /* Read memory from the inferior process.  This should generally be
     called through read_inferior_memory, which handles breakpoint shadowing.

     Read LEN bytes at MEMADDR into a buffer at MYADDR.
  
     Returns 0 on success and errno on failure.  */

  int (*read_memory) (CORE_ADDR memaddr, unsigned char *myaddr, int len);

  /* Write memory to the inferior process.  This should generally be
     called through write_inferior_memory, which handles breakpoint shadowing.

     Write LEN bytes from the buffer at MYADDR to MEMADDR.

     Returns 0 on success and errno on failure.  */

  int (*write_memory) (CORE_ADDR memaddr, const unsigned char *myaddr,
		       int len);

  /* Query GDB for the values of any symbols we're interested in.
     This function is called whenever we receive a "qSymbols::"
     query, which corresponds to every time more symbols (might)
     become available.  NULL if we aren't interested in any
     symbols.  */

  void (*look_up_symbols) (void);

  /* Send an interrupt request to the inferior process,
     however is appropriate.  */

  void (*request_interrupt) (void);

  /* Read auxiliary vector data from the inferior process.

     Read LEN bytes at OFFSET into a buffer at MYADDR.  */

  int (*read_auxv) (CORE_ADDR offset, unsigned char *myaddr,
		    unsigned int len);

  /* Insert and remove a break or watchpoint.
     Returns 0 on success, -1 on failure and 1 on unsupported.
     The type is coded as follows:
       '0' - software-breakpoint
       '1' - hardware-breakpoint
       '2' - write watchpoint
       '3' - read watchpoint
       '4' - access watchpoint  */

  int (*insert_point) (char type, CORE_ADDR addr, int len);
  int (*remove_point) (char type, CORE_ADDR addr, int len);

  /* Returns 1 if target was stopped due to a watchpoint hit, 0 otherwise.  */

  int (*stopped_by_watchpoint) (void);

  /* Returns the address associated with the watchpoint that hit, if any;
     returns 0 otherwise.  */

  CORE_ADDR (*stopped_data_address) (void);

  /* Reports the text, data offsets of the executable.  This is
     needed for uclinux where the executable is relocated during load
     time.  */

  int (*read_offsets) (CORE_ADDR *text, CORE_ADDR *data);

  /* Fetch the address associated with a specific thread local storage
     area, determined by the specified THREAD, OFFSET, and LOAD_MODULE.
     Stores it in *ADDRESS and returns zero on success; otherwise returns
     an error code.  A return value of -1 means this system does not
     support the operation.  */

  int (*get_tls_address) (struct thread_info *thread, CORE_ADDR offset,
			  CORE_ADDR load_module, CORE_ADDR *address);

   /* Read/Write from/to spufs using qXfer packets.  */
  int (*qxfer_spu) (const char *annex, unsigned char *readbuf,
		    unsigned const char *writebuf, CORE_ADDR offset, int len);

  /* Fill BUF with an hostio error packet representing the last hostio
     error.  */
  void (*hostio_last_error) (char *buf);

  /* Read/Write OS data using qXfer packets.  */
  int (*qxfer_osdata) (const char *annex, unsigned char *readbuf,
		       unsigned const char *writebuf, CORE_ADDR offset,
		       int len);

  /* Read/Write extra signal info.  */
  int (*qxfer_siginfo) (const char *annex, unsigned char *readbuf,
			unsigned const char *writebuf,
			CORE_ADDR offset, int len);

  int (*supports_non_stop) (void);

  /* Enables async target events.  Returns the previous enable
     state.  */
  int (*async) (int enable);

  /* Switch to non-stop (1) or all-stop (0) mode.  Return 0 on
     success, -1 otherwise.  */
  int (*start_non_stop) (int);

  /* Returns true if the target supports multi-process debugging.  */
  int (*supports_multi_process) (void);

  /* If not NULL, target-specific routine to process monitor command.
     Returns 1 if handled, or 0 to perform default processing.  */
  int (*handle_monitor_command) (char *);

  /* Returns the core given a thread, or -1 if not known.  */
  int (*core_of_thread) (ptid_t);
};

extern struct target_ops *the_target;

void set_target_ops (struct target_ops *);

#define create_inferior(program, args) \
  (*the_target->create_inferior) (program, args)

#define myattach(pid) \
  (*the_target->attach) (pid)

#define kill_inferior(pid) \
  (*the_target->kill) (pid)

#define detach_inferior(pid) \
  (*the_target->detach) (pid)

#define mythread_alive(pid) \
  (*the_target->thread_alive) (pid)

#define fetch_inferior_registers(regno) \
  (*the_target->fetch_registers) (regno)

#define store_inferior_registers(regno) \
  (*the_target->store_registers) (regno)

#define join_inferior(pid) \
  (*the_target->join) (pid)

#define target_supports_non_stop() \
  (the_target->supports_non_stop ? (*the_target->supports_non_stop ) () : 0)

#define target_async(enable) \
  (the_target->async ? (*the_target->async) (enable) : 0)

#define target_supports_multi_process() \
  (the_target->supports_multi_process ? \
   (*the_target->supports_multi_process) () : 0)

/* Start non-stop mode, returns 0 on success, -1 on failure.   */

int start_non_stop (int nonstop);

ptid_t mywait (ptid_t ptid, struct target_waitstatus *ourstatus, int options,
	       int connected_wait);

int read_inferior_memory (CORE_ADDR memaddr, unsigned char *myaddr, int len);

int write_inferior_memory (CORE_ADDR memaddr, const unsigned char *myaddr,
			   int len);

void set_desired_inferior (int id);

const char *target_pid_to_str (ptid_t);

#endif /* TARGET_H */
