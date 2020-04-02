/* Internal interfaces for the GNU/Linux specific target code for gdbserver.
   Copyright (C) 2002-2020 Free Software Foundation, Inc.

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

#ifndef GDBSERVER_LINUX_LOW_H
#define GDBSERVER_LINUX_LOW_H

#include "nat/linux-nat.h"
#include "nat/gdb_thread_db.h"
#include <signal.h>

#include "gdbthread.h"
#include "gdb_proc_service.h"

/* Included for ptrace type definitions.  */
#include "nat/linux-ptrace.h"
#include "target/waitstatus.h" /* For enum target_stop_reason.  */
#include "tracepoint.h"

#define PTRACE_XFER_TYPE long

#ifdef HAVE_LINUX_REGSETS
typedef void (*regset_fill_func) (struct regcache *, void *);
typedef void (*regset_store_func) (struct regcache *, const void *);
enum regset_type {
  GENERAL_REGS,
  FP_REGS,
  EXTENDED_REGS,
  OPTIONAL_REGS, /* Do not error if the regset cannot be accessed.  */
};

/* The arch's regsets array initializer must be terminated with a NULL
   regset.  */
#define NULL_REGSET \
  { 0, 0, 0, -1, (enum regset_type) -1, NULL, NULL }

struct regset_info
{
  int get_request, set_request;
  /* If NT_TYPE isn't 0, it will be passed to ptrace as the 3rd
     argument and the 4th argument should be "const struct iovec *".  */
  int nt_type;
  int size;
  enum regset_type type;
  regset_fill_func fill_function;
  regset_store_func store_function;
};

/* Aggregation of all the supported regsets of a given
   architecture/mode.  */

struct regsets_info
{
  /* The regsets array.  */
  struct regset_info *regsets;

  /* The number of regsets in the REGSETS array.  */
  int num_regsets;

  /* If we get EIO on a regset, do not try it again.  Note the set of
     supported regsets may depend on processor mode on biarch
     machines.  This is a (lazily allocated) array holding one boolean
     byte (0/1) per regset, with each element corresponding to the
     regset in the REGSETS array above at the same offset.  */
  char *disabled_regsets;
};

#endif

/* Mapping between the general-purpose registers in `struct user'
   format and GDB's register array layout.  */

struct usrregs_info
{
  /* The number of registers accessible.  */
  int num_regs;

  /* The registers map.  */
  int *regmap;
};

/* All info needed to access an architecture/mode's registers.  */

struct regs_info
{
  /* Regset support bitmap: 1 for registers that are transferred as a part
     of a regset, 0 for ones that need to be handled individually.  This
     can be NULL if all registers are transferred with regsets or regsets
     are not supported.  */
  unsigned char *regset_bitmap;

  /* Info used when accessing registers with PTRACE_PEEKUSER /
     PTRACE_POKEUSER.  This can be NULL if all registers are
     transferred with regsets  .*/
  struct usrregs_info *usrregs;

#ifdef HAVE_LINUX_REGSETS
  /* Info used when accessing registers with regsets.  */
  struct regsets_info *regsets_info;
#endif
};

struct process_info_private
{
  /* Arch-specific additions.  */
  struct arch_process_info *arch_private;

  /* libthread_db-specific additions.  Not NULL if this process has loaded
     thread_db, and it is active.  */
  struct thread_db *thread_db;

  /* &_r_debug.  0 if not yet determined.  -1 if no PT_DYNAMIC in Phdrs.  */
  CORE_ADDR r_debug;
};

struct lwp_info;

struct linux_target_ops
{
  /* See target.h for details.  */
  int (*breakpoint_kind_from_pc) (CORE_ADDR *pcptr);

  /* See target.h for details.  */
  const gdb_byte *(*sw_breakpoint_from_kind) (int kind, int *size);

  /* Find the next possible PCs after the current instruction executes.  */
  std::vector<CORE_ADDR> (*get_next_pcs) (struct regcache *regcache);

  int decr_pc_after_break;
  int (*breakpoint_at) (CORE_ADDR pc);

  /* Breakpoint and watchpoint related functions.  See target.h for
     comments.  */
  int (*supports_z_point_type) (char z_type);
  int (*insert_point) (enum raw_bkpt_type type, CORE_ADDR addr,
		       int size, struct raw_breakpoint *bp);
  int (*remove_point) (enum raw_bkpt_type type, CORE_ADDR addr,
		       int size, struct raw_breakpoint *bp);

  int (*stopped_by_watchpoint) (void);
  CORE_ADDR (*stopped_data_address) (void);

  /* Hooks to reformat register data for PEEKUSR/POKEUSR (in particular
     for registers smaller than an xfer unit).  */
  void (*collect_ptrace_register) (struct regcache *regcache,
				   int regno, char *buf);
  void (*supply_ptrace_register) (struct regcache *regcache,
				  int regno, const char *buf);

  /* Hook to convert from target format to ptrace format and back.
     Returns true if any conversion was done; false otherwise.
     If DIRECTION is 1, then copy from INF to NATIVE.
     If DIRECTION is 0, copy from NATIVE to INF.  */
  int (*siginfo_fixup) (siginfo_t *native, gdb_byte *inf, int direction);

  /* Hook to call when a new process is created or attached to.
     If extra per-process architecture-specific data is needed,
     allocate it here.  */
  struct arch_process_info * (*new_process) (void);

  /* Hook to call when a process is being deleted.  If extra per-process
     architecture-specific data is needed, delete it here.  */
  void (*delete_process) (struct arch_process_info *info);

  /* Hook to call when a new thread is detected.
     If extra per-thread architecture-specific data is needed,
     allocate it here.  */
  void (*new_thread) (struct lwp_info *);

  /* Hook to call when a thread is being deleted.  If extra per-thread
     architecture-specific data is needed, delete it here.  */
  void (*delete_thread) (struct arch_lwp_info *);

  /* Hook to call, if any, when a new fork is attached.  */
  void (*new_fork) (struct process_info *parent, struct process_info *child);

  /* Hook to call prior to resuming a thread.  */
  void (*prepare_to_resume) (struct lwp_info *);

  /* Hook to support target specific qSupported.  */
  void (*process_qsupported) (char **, int count);

  /* Returns true if the low target supports tracepoints.  */
  int (*supports_tracepoints) (void);

  /* Fill ADDRP with the thread area address of LWPID.  Returns 0 on
     success, -1 on failure.  */
  int (*get_thread_area) (int lwpid, CORE_ADDR *addrp);

  /* Install a fast tracepoint jump pad.  See target.h for
     comments.  */
  int (*install_fast_tracepoint_jump_pad) (CORE_ADDR tpoint, CORE_ADDR tpaddr,
					   CORE_ADDR collector,
					   CORE_ADDR lockaddr,
					   ULONGEST orig_size,
					   CORE_ADDR *jump_entry,
					   CORE_ADDR *trampoline,
					   ULONGEST *trampoline_size,
					   unsigned char *jjump_pad_insn,
					   ULONGEST *jjump_pad_insn_size,
					   CORE_ADDR *adjusted_insn_addr,
					   CORE_ADDR *adjusted_insn_addr_end,
					   char *err);

  /* Return the bytecode operations vector for the current inferior.
     Returns NULL if bytecode compilation is not supported.  */
  struct emit_ops *(*emit_ops) (void);

  /* Return the minimum length of an instruction that can be safely overwritten
     for use as a fast tracepoint.  */
  int (*get_min_fast_tracepoint_insn_len) (void);

  /* Returns true if the low target supports range stepping.  */
  int (*supports_range_stepping) (void);

  /* See target.h.  */
  int (*breakpoint_kind_from_current_state) (CORE_ADDR *pcptr);

  /* See target.h.  */
  int (*supports_hardware_single_step) (void);

  /* Fill *SYSNO with the syscall nr trapped.  Only to be called when
     inferior is stopped due to SYSCALL_SIGTRAP.  */
  void (*get_syscall_trapinfo) (struct regcache *regcache, int *sysno);

  /* See target.h.  */
  int (*get_ipa_tdesc_idx) (void);
};

extern struct linux_target_ops the_low_target;

/* Target ops definitions for a Linux target.  */

class linux_process_target : public process_stratum_target
{
public:

  int create_inferior (const char *program,
		       const std::vector<char *> &program_args) override;

  void post_create_inferior () override;

  int attach (unsigned long pid) override;

  int kill (process_info *proc) override;

  int detach (process_info *proc) override;

  void mourn (process_info *proc) override;

  void join (int pid) override;

  bool thread_alive (ptid_t pid) override;

  void resume (thread_resume *resume_info, size_t n) override;

  ptid_t wait (ptid_t ptid, target_waitstatus *status,
	       int options) override;

  void fetch_registers (regcache *regcache, int regno) override;

  void store_registers (regcache *regcache, int regno) override;

  int prepare_to_access_memory () override;

  void done_accessing_memory () override;

  int read_memory (CORE_ADDR memaddr, unsigned char *myaddr,
		   int len) override;

  int write_memory (CORE_ADDR memaddr, const unsigned char *myaddr,
		    int len) override;

  void look_up_symbols () override;

  void request_interrupt () override;

  bool supports_read_auxv () override;

  int read_auxv (CORE_ADDR offset, unsigned char *myaddr,
		 unsigned int len) override;

  bool supports_z_point_type (char z_type) override;

  int insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
		    int size, raw_breakpoint *bp) override;

  int remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
		    int size, raw_breakpoint *bp) override;

  bool stopped_by_sw_breakpoint () override;

  bool supports_stopped_by_sw_breakpoint () override;

  bool stopped_by_hw_breakpoint () override;

  bool supports_stopped_by_hw_breakpoint () override;

  bool supports_hardware_single_step () override;

  bool stopped_by_watchpoint () override;

  CORE_ADDR stopped_data_address () override;

  bool supports_read_offsets () override;

  int read_offsets (CORE_ADDR *text, CORE_ADDR *data) override;

  bool supports_get_tls_address () override;

  int get_tls_address (thread_info *thread, CORE_ADDR offset,
		       CORE_ADDR load_module, CORE_ADDR *address) override;

  bool supports_qxfer_osdata () override;

  int qxfer_osdata (const char *annex, unsigned char *readbuf,
		    unsigned const char *writebuf,
		    CORE_ADDR offset, int len) override;

  bool supports_qxfer_siginfo () override;

  int qxfer_siginfo (const char *annex, unsigned char *readbuf,
		     unsigned const char *writebuf,
		     CORE_ADDR offset, int len) override;

  bool supports_non_stop () override;

  bool async (bool enable) override;

  int start_non_stop (bool enable) override;

  bool supports_multi_process () override;

  bool supports_fork_events () override;

  bool supports_vfork_events () override;

  bool supports_exec_events () override;

  void handle_new_gdb_connection () override;

  int handle_monitor_command (char *mon) override;

  int core_of_thread (ptid_t ptid) override;

#if defined PT_GETDSBT || defined PTRACE_GETFDPIC
  bool supports_read_loadmap () override;

  int read_loadmap (const char *annex, CORE_ADDR offset,
		    unsigned char *myaddr, unsigned int len) override;
#endif

  void process_qsupported (char **features, int count) override;

  bool supports_tracepoints () override;

  CORE_ADDR read_pc (regcache *regcache) override;

  void write_pc (regcache *regcache, CORE_ADDR pc) override;

  bool supports_thread_stopped () override;

  bool thread_stopped (thread_info *thread) override;

  void pause_all (bool freeze) override;

  void unpause_all (bool unfreeze) override;

  void stabilize_threads () override;

  bool supports_fast_tracepoints () override;

  int install_fast_tracepoint_jump_pad (CORE_ADDR tpoint,
					CORE_ADDR tpaddr,
					CORE_ADDR collector,
					CORE_ADDR lockaddr,
					ULONGEST orig_size,
					CORE_ADDR *jump_entry,
					CORE_ADDR *trampoline,
					ULONGEST *trampoline_size,
					unsigned char *jjump_pad_insn,
					ULONGEST *jjump_pad_insn_size,
					CORE_ADDR *adjusted_insn_addr,
					CORE_ADDR *adjusted_insn_addr_end,
					char *err) override;

  int get_min_fast_tracepoint_insn_len () override;

  struct emit_ops *emit_ops () override;

  bool supports_disable_randomization () override;

  bool supports_qxfer_libraries_svr4 () override;

  int qxfer_libraries_svr4 (const char *annex,
			    unsigned char *readbuf,
			    unsigned const char *writebuf,
			    CORE_ADDR offset, int len) override;

  bool supports_agent () override;

#ifdef HAVE_LINUX_BTRACE
  btrace_target_info *enable_btrace (ptid_t ptid,
				     const btrace_config *conf) override;

  int disable_btrace (btrace_target_info *tinfo) override;

  int read_btrace (btrace_target_info *tinfo, buffer *buf,
		   enum btrace_read_type type) override;

  int read_btrace_conf (const btrace_target_info *tinfo,
			buffer *buf) override;
#endif

  bool supports_range_stepping () override;

  bool supports_pid_to_exec_file () override;

  char *pid_to_exec_file (int pid) override;

  bool supports_multifs () override;

  int multifs_open (int pid, const char *filename, int flags,
		    mode_t mode) override;

  int multifs_unlink (int pid, const char *filename) override;

  ssize_t multifs_readlink (int pid, const char *filename, char *buf,
			    size_t bufsiz) override;

  int breakpoint_kind_from_pc (CORE_ADDR *pcptr) override;

  const gdb_byte *sw_breakpoint_from_kind (int kind, int *size) override;

  int breakpoint_kind_from_current_state (CORE_ADDR *pcptr) override;

  const char *thread_name (ptid_t thread) override;

#if USE_THREAD_DB
  bool thread_handle (ptid_t ptid, gdb_byte **handle,
		      int *handle_len) override;
#endif

  bool supports_software_single_step () override;

  bool supports_catch_syscall () override;

  int get_ipa_tdesc_idx () override;

  /* Return the information to access registers.  This has public
     visibility because proc-service uses it.  */
  virtual const regs_info *get_regs_info () = 0;

private:

  /* Handle a GNU/Linux extended wait response.  If we see a clone,
     fork, or vfork event, we need to add the new LWP to our list
     (and return 0 so as not to report the trap to higher layers).
     If we see an exec event, we will modify ORIG_EVENT_LWP to point
     to a new LWP representing the new program.  */
  int handle_extended_wait (lwp_info **orig_event_lwp, int wstat);

  /* Do low-level handling of the event, and check if we should go on
     and pass it to caller code.  Return the affected lwp if we are, or
     NULL otherwise.  */
  lwp_info *filter_event (int lwpid, int wstat);

  /* Wait for an event from child(ren) WAIT_PTID, and return any that
     match FILTER_PTID (leaving others pending).  The PTIDs can be:
     minus_one_ptid, to specify any child; a pid PTID, specifying all
     lwps of a thread group; or a PTID representing a single lwp.  Store
     the stop status through the status pointer WSTAT.  OPTIONS is
     passed to the waitpid call.  Return 0 if no event was found and
     OPTIONS contains WNOHANG.  Return -1 if no unwaited-for children
     was found.  Return the PID of the stopped child otherwise.  */
  int wait_for_event_filtered (ptid_t wait_ptid, ptid_t filter_ptid,
			       int *wstatp, int options);

  /* Wait for an event from child(ren) PTID.  PTIDs can be:
     minus_one_ptid, to specify any child; a pid PTID, specifying all
     lwps of a thread group; or a PTID representing a single lwp.  Store
     the stop status through the status pointer WSTAT.  OPTIONS is
     passed to the waitpid call.  Return 0 if no event was found and
     OPTIONS contains WNOHANG.  Return -1 if no unwaited-for children
     was found.  Return the PID of the stopped child otherwise.  */
  int wait_for_event (ptid_t ptid, int *wstatp, int options);

  /* Wait for all children to stop for the SIGSTOPs we just queued.  */
  void wait_for_sigstop ();

  /* Wait for process, returns status.  */
  ptid_t wait_1 (ptid_t ptid, target_waitstatus *ourstatus,
		 int target_options);

  /* Stop all lwps that aren't stopped yet, except EXCEPT, if not NULL.
     If SUSPEND, then also increase the suspend count of every LWP,
     except EXCEPT.  */
  void stop_all_lwps (int suspend, lwp_info *except);

  /* Stopped LWPs that the client wanted to be running, that don't have
     pending statuses, are set to run again, except for EXCEPT, if not
     NULL.  This undoes a stop_all_lwps call.  */
  void unstop_all_lwps (int unsuspend, lwp_info *except);

  /* Start a step-over operation on LWP.  When LWP stopped at a
     breakpoint, to make progress, we need to remove the breakpoint out
     of the way.  If we let other threads run while we do that, they may
     pass by the breakpoint location and miss hitting it.  To avoid
     that, a step-over momentarily stops all threads while LWP is
     single-stepped by either hardware or software while the breakpoint
     is temporarily uninserted from the inferior.  When the single-step
     finishes, we reinsert the breakpoint, and let all threads that are
     supposed to be running, run again.  */
  void start_step_over (lwp_info *lwp);

  /* If there's a step over in progress, wait until all threads stop
     (that is, until the stepping thread finishes its step), and
     unsuspend all lwps.  The stepping thread ends with its status
     pending, which is processed later when we get back to processing
     events.  */
  void complete_ongoing_step_over ();

  /* When we finish a step-over, set threads running again.  If there's
     another thread that may need a step-over, now's the time to start
     it.  Eventually, we'll move all threads past their breakpoints.  */
  void proceed_all_lwps ();

  /* The reason we resume in the caller, is because we want to be able
     to pass lwp->status_pending as WSTAT, and we need to clear
     status_pending_p before resuming, otherwise, resume_one_lwp
     refuses to resume.  */
  bool maybe_move_out_of_jump_pad (lwp_info *lwp, int *wstat);

  /* Move THREAD out of the jump pad.  */
  void move_out_of_jump_pad (thread_info *thread);

  /* Call low_arch_setup on THREAD.  */
  void arch_setup_thread (thread_info *thread);

#ifdef HAVE_LINUX_USRREGS
  /* Fetch one register.  */
  void fetch_register (const usrregs_info *usrregs, regcache *regcache,
		       int regno);

  /* Store one register.  */
  void store_register (const usrregs_info *usrregs, regcache *regcache,
		       int regno);
#endif

  /* Fetch all registers, or just one, from the child process.
     If REGNO is -1, do this for all registers, skipping any that are
     assumed to have been retrieved by regsets_fetch_inferior_registers,
     unless ALL is non-zero.
     Otherwise, REGNO specifies which register (so we can save time).  */
  void usr_fetch_inferior_registers (const regs_info *regs_info,
				     regcache *regcache, int regno, int all);

  /* Store our register values back into the inferior.
     If REGNO is -1, do this for all registers, skipping any that are
     assumed to have been saved by regsets_store_inferior_registers,
     unless ALL is non-zero.
     Otherwise, REGNO specifies which register (so we can save time).  */
  void usr_store_inferior_registers (const regs_info *regs_info,
				     regcache *regcache, int regno, int all);

  /* Return the PC as read from the regcache of LWP, without any
     adjustment.  */
  CORE_ADDR get_pc (lwp_info *lwp);

  /* Called when the LWP stopped for a signal/trap.  If it stopped for a
     trap check what caused it (breakpoint, watchpoint, trace, etc.),
     and save the result in the LWP's stop_reason field.  If it stopped
     for a breakpoint, decrement the PC if necessary on the lwp's
     architecture.  Returns true if we now have the LWP's stop PC.  */
  bool save_stop_reason (lwp_info *lwp);

  /* Resume execution of LWP.  If STEP is nonzero, single-step it.  If
     SIGNAL is nonzero, give it that signal.  */
  void resume_one_lwp_throw (lwp_info *lwp, int step, int signal,
			     siginfo_t *info);

  /* Like resume_one_lwp_throw, but no error is thrown if the LWP
     disappears while we try to resume it.  */
  void resume_one_lwp (lwp_info *lwp, int step, int signal, siginfo_t *info);

  /* This function is called once per thread.  We check the thread's
     last resume request, which will tell us whether to resume, step, or
     leave the thread stopped.  Any signal the client requested to be
     delivered has already been enqueued at this point.

     If any thread that GDB wants running is stopped at an internal
     breakpoint that needs stepping over, we start a step-over operation
     on that particular thread, and leave all others stopped.  */
  void proceed_one_lwp (thread_info *thread, lwp_info *except);

  /* This function is called once per thread.  We check the thread's
     resume request, which will tell us whether to resume, step, or
     leave the thread stopped; and what signal, if any, it should be
     sent.

     For threads which we aren't explicitly told otherwise, we preserve
     the stepping flag; this is used for stepping over gdbserver-placed
     breakpoints.

     If pending_flags was set in any thread, we queue any needed
     signals, since we won't actually resume.  We already have a pending
     event to report, so we don't need to preserve any step requests;
     they should be re-issued if necessary.  */
  void resume_one_thread (thread_info *thread, bool leave_all_stopped);

  /* Return true if this lwp has an interesting status pending.  */
  bool status_pending_p_callback (thread_info *thread, ptid_t ptid);

  /* Resume LWPs that are currently stopped without any pending status
     to report, but are resumed from the core's perspective.  */
  void resume_stopped_resumed_lwps (thread_info *thread);

  /* Unsuspend THREAD, except EXCEPT, and proceed.  */
  void unsuspend_and_proceed_one_lwp (thread_info *thread, lwp_info *except);

  /* Return true if this lwp still has an interesting status pending.
     If not (e.g., it had stopped for a breakpoint that is gone), return
     false.  */
  bool thread_still_has_status_pending (thread_info *thread);

  /* Return true if this lwp is to-be-resumed and has an interesting
     status pending.  */
  bool resume_status_pending (thread_info *thread);

  /* Return true if this lwp that GDB wants running is stopped at an
     internal breakpoint that we need to step over.  It assumes that
     any required STOP_PC adjustment has already been propagated to
     the inferior's regcache.  */
  bool thread_needs_step_over (thread_info *thread);

  /* Single step via hardware or software single step.
     Return 1 if hardware single stepping, 0 if software single stepping
     or can't single step.  */
  int single_step (lwp_info* lwp);

  /* Install breakpoints for software single stepping.  */
  void install_software_single_step_breakpoints (lwp_info *lwp);

protected:
  /* The architecture-specific "low" methods are listed below.  */

  /* Architecture-specific setup for the current thread.  */
  virtual void low_arch_setup () = 0;

  /* Return false if we can fetch/store the register, true if we cannot
     fetch/store the register.  */
  virtual bool low_cannot_fetch_register (int regno) = 0;

  virtual bool low_cannot_store_register (int regno) = 0;

  /* Hook to fetch a register in some non-standard way.  Used for
     example by backends that have read-only registers with hardcoded
     values (e.g., IA64's gr0/fr0/fr1).  Returns true if register
     REGNO was supplied, false if not, and we should fallback to the
     standard ptrace methods.  */
  virtual bool low_fetch_register (regcache *regcache, int regno);

  /* Return true if breakpoints are supported.  Such targets must
     implement the GET_PC and SET_PC methods.  */
  virtual bool low_supports_breakpoints ();

  virtual CORE_ADDR low_get_pc (regcache *regcache);

  virtual void low_set_pc (regcache *regcache, CORE_ADDR newpc);
};

extern linux_process_target *the_linux_target;

#define get_thread_lwp(thr) ((struct lwp_info *) (thread_target_data (thr)))
#define get_lwp_thread(lwp) ((lwp)->thread)

/* This struct is recorded in the target_data field of struct thread_info.

   On linux ``all_threads'' is keyed by the LWP ID, which we use as the
   GDB protocol representation of the thread ID.  Threads also have
   a "process ID" (poorly named) which is (presently) the same as the
   LWP ID.

   There is also ``all_processes'' is keyed by the "overall process ID",
   which GNU/Linux calls tgid, "thread group ID".  */

struct lwp_info
{
  /* Backlink to the parent object.  */
  struct thread_info *thread;

  /* If this flag is set, the next SIGSTOP will be ignored (the
     process will be immediately resumed).  This means that either we
     sent the SIGSTOP to it ourselves and got some other pending event
     (so the SIGSTOP is still pending), or that we stopped the
     inferior implicitly via PTRACE_ATTACH and have not waited for it
     yet.  */
  int stop_expected;

  /* When this is true, we shall not try to resume this thread, even
     if last_resume_kind isn't resume_stop.  */
  int suspended;

  /* If this flag is set, the lwp is known to be stopped right now (stop
     event already received in a wait()).  */
  int stopped;

  /* Signal whether we are in a SYSCALL_ENTRY or
     in a SYSCALL_RETURN event.
     Values:
     - TARGET_WAITKIND_SYSCALL_ENTRY
     - TARGET_WAITKIND_SYSCALL_RETURN */
  enum target_waitkind syscall_state;

  /* When stopped is set, the last wait status recorded for this lwp.  */
  int last_status;

  /* If WAITSTATUS->KIND != TARGET_WAITKIND_IGNORE, the waitstatus for
     this LWP's last event, to pass to GDB without any further
     processing.  This is used to store extended ptrace event
     information or exit status until it can be reported to GDB.  */
  struct target_waitstatus waitstatus;

  /* A pointer to the fork child/parent relative.  Valid only while
     the parent fork event is not reported to higher layers.  Used to
     avoid wildcard vCont actions resuming a fork child before GDB is
     notified about the parent's fork event.  */
  struct lwp_info *fork_relative;

  /* When stopped is set, this is where the lwp last stopped, with
     decr_pc_after_break already accounted for.  If the LWP is
     running, this is the address at which the lwp was resumed.  */
  CORE_ADDR stop_pc;

  /* If this flag is set, STATUS_PENDING is a waitstatus that has not yet
     been reported.  */
  int status_pending_p;
  int status_pending;

  /* The reason the LWP last stopped, if we need to track it
     (breakpoint, watchpoint, etc.)  */
  enum target_stop_reason stop_reason;

  /* On architectures where it is possible to know the data address of
     a triggered watchpoint, STOPPED_DATA_ADDRESS is non-zero, and
     contains such data address.  Only valid if STOPPED_BY_WATCHPOINT
     is true.  */
  CORE_ADDR stopped_data_address;

  /* If this is non-zero, it is a breakpoint to be reinserted at our next
     stop (SIGTRAP stops only).  */
  CORE_ADDR bp_reinsert;

  /* If this flag is set, the last continue operation at the ptrace
     level on this process was a single-step.  */
  int stepping;

  /* Range to single step within.  This is a copy of the step range
     passed along the last resume request.  See 'struct
     thread_resume'.  */
  CORE_ADDR step_range_start;	/* Inclusive */
  CORE_ADDR step_range_end;	/* Exclusive */

  /* If this flag is set, we need to set the event request flags the
     next time we see this LWP stop.  */
  int must_set_ptrace_flags;

  /* If this is non-zero, it points to a chain of signals which need to
     be delivered to this process.  */
  struct pending_signals *pending_signals;

  /* A link used when resuming.  It is initialized from the resume request,
     and then processed and cleared in linux_resume_one_lwp.  */
  struct thread_resume *resume;

  /* Information bout this lwp's fast tracepoint collection status (is it
     currently stopped in the jump pad, and if so, before or at/after the
     relocated instruction).  Normally, we won't care about this, but we will
     if a signal arrives to this lwp while it is collecting.  */
  fast_tpoint_collect_result collecting_fast_tracepoint;

  /* If this is non-zero, it points to a chain of signals which need
     to be reported to GDB.  These were deferred because the thread
     was doing a fast tracepoint collect when they arrived.  */
  struct pending_signals *pending_signals_to_report;

  /* When collecting_fast_tracepoint is first found to be 1, we insert
     a exit-jump-pad-quickly breakpoint.  This is it.  */
  struct breakpoint *exit_jump_pad_bkpt;

#ifdef USE_THREAD_DB
  int thread_known;
  /* The thread handle, used for e.g. TLS access.  Only valid if
     THREAD_KNOWN is set.  */
  td_thrhandle_t th;

  /* The pthread_t handle.  */
  thread_t thread_handle;
#endif

  /* Arch-specific additions.  */
  struct arch_lwp_info *arch_private;
};

int linux_pid_exe_is_elf_64_file (int pid, unsigned int *machine);

/* Attach to PTID.  Returns 0 on success, non-zero otherwise (an
   errno).  */
int linux_attach_lwp (ptid_t ptid);

struct lwp_info *find_lwp_pid (ptid_t ptid);
/* For linux_stop_lwp see nat/linux-nat.h.  */

#ifdef HAVE_LINUX_REGSETS
void initialize_regsets_info (struct regsets_info *regsets_info);
#endif

void initialize_low_arch (void);

void linux_set_pc_32bit (struct regcache *regcache, CORE_ADDR pc);
CORE_ADDR linux_get_pc_32bit (struct regcache *regcache);

void linux_set_pc_64bit (struct regcache *regcache, CORE_ADDR pc);
CORE_ADDR linux_get_pc_64bit (struct regcache *regcache);

/* From thread-db.c  */
int thread_db_init (void);
void thread_db_detach (struct process_info *);
void thread_db_mourn (struct process_info *);
int thread_db_handle_monitor_command (char *);
int thread_db_get_tls_address (struct thread_info *thread, CORE_ADDR offset,
			       CORE_ADDR load_module, CORE_ADDR *address);
int thread_db_look_up_one_symbol (const char *name, CORE_ADDR *addrp);

/* Called from linux-low.c when a clone event is detected.  Upon entry,
   both the clone and the parent should be stopped.  This function does
   whatever is required have the clone under thread_db's control.  */

void thread_db_notice_clone (struct thread_info *parent_thr, ptid_t child_ptid);

bool thread_db_thread_handle (ptid_t ptid, gdb_byte **handle, int *handle_len);

extern int have_ptrace_getregset;

/* Search for the value with type MATCH in the auxv vector with
   entries of length WORDSIZE bytes.  If found, store the value in
   *VALP and return 1.  If not found or if there is an error, return
   0.  */

int linux_get_auxv (int wordsize, CORE_ADDR match,
		    CORE_ADDR *valp);

/* Fetch the AT_HWCAP entry from the auxv vector, where entries are length
   WORDSIZE.  If no entry was found, return zero.  */

CORE_ADDR linux_get_hwcap (int wordsize);

/* Fetch the AT_HWCAP2 entry from the auxv vector, where entries are length
   WORDSIZE.  If no entry was found, return zero.  */

CORE_ADDR linux_get_hwcap2 (int wordsize);

#endif /* GDBSERVER_LINUX_LOW_H */
