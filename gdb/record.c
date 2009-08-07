/* Process record and replay target for GDB, the GNU debugger.

   Copyright (C) 2008, 2009 Free Software Foundation, Inc.

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

#include "defs.h"
#include "gdbcmd.h"
#include "regcache.h"
#include "gdbthread.h"
#include "event-top.h"
#include "exceptions.h"
#include "completer.h"
#include "record.h"
#include "checkpoint.h"

#include <byteswap.h>
#include <signal.h>
#include <netinet/in.h>

#define DEFAULT_RECORD_INSN_MAX_NUM	200000

#define RECORD_IS_REPLAY \
     (record_list->next || execution_direction == EXEC_REVERSE)

#define RECORD_FILE_MAGIC	htonl(0x20090726) /* Host to network order */

/* These are the core struct of record function.

   An record_entry is a record of the value change of a register
   ("record_reg") or a part of memory ("record_mem").  And each
   instruction must has a struct record_entry ("record_end") that points out this
   is the last struct record_entry of this instruction.

   Each struct record_entry is linked to "record_list" by "prev" and "next". */

struct record_reg_entry
{
  int num;
  gdb_byte *val;
};

struct record_mem_entry
{
  CORE_ADDR addr;
  int len;
  /* Set this flag if target memory for this entry
     can no longer be accessed.  */
  int mem_entry_not_accessible;
  gdb_byte *val;
};

enum record_type
{
  record_end = 0,
  record_reg,
  record_mem
};

struct record_entry
{
  struct record_entry *prev;
  struct record_entry *next;
  enum record_type type;
  union
  {
    /* reg */
    struct record_reg_entry reg;
    /* mem */
    struct record_mem_entry mem;
  } u;
};

/* This is the debug switch for process record.  */
int record_debug = 0;

/* These list is for execution log.  */
static struct record_entry record_first;
static struct record_entry *record_list = &record_first;
static struct record_entry *record_arch_list_head = NULL;
static struct record_entry *record_arch_list_tail = NULL;

/* 1 ask user. 0 auto delete the last struct record_entry.  */
static int record_stop_at_limit = 1;
static int record_insn_max_num = DEFAULT_RECORD_INSN_MAX_NUM;
static int record_insn_num = 0;

/* The target_ops of process record.  */
static struct target_ops record_ops;

/* The beneath function pointers.  */
static struct target_ops *record_beneath_to_resume_ops;
static void (*record_beneath_to_resume) (struct target_ops *, ptid_t, int,
                                         enum target_signal);
static struct target_ops *record_beneath_to_wait_ops;
static ptid_t (*record_beneath_to_wait) (struct target_ops *, ptid_t,
					 struct target_waitstatus *,
					 int);
static struct target_ops *record_beneath_to_store_registers_ops;
static void (*record_beneath_to_store_registers) (struct target_ops *,
                                                  struct regcache *,
						  int regno);
static struct target_ops *record_beneath_to_xfer_partial_ops;
static LONGEST (*record_beneath_to_xfer_partial) (struct target_ops *ops,
						  enum target_object object,
						  const char *annex,
						  gdb_byte *readbuf,
						  const gdb_byte *writebuf,
						  ULONGEST offset,
						  LONGEST len);
static int (*record_beneath_to_insert_breakpoint) (struct gdbarch *,
						   struct bp_target_info *);
static int (*record_beneath_to_remove_breakpoint) (struct gdbarch *,
						   struct bp_target_info *);

/* Checkpoint target methods.  */

static void *record_insert_checkpoint (struct checkpoint_info *, int);
static void record_delete_checkpoint (struct checkpoint_info *, int);
static void record_show_checkpoint_info (struct checkpoint_info *, int);
static void record_restore_checkpoint (struct checkpoint_info *, int);

static void
record_list_release (struct record_entry *rec)
{
  struct record_entry *tmp;

  if (!rec)
    return;

  while (rec->next)
    {
      rec = rec->next;
    }

  while (rec->prev)
    {
      tmp = rec;
      rec = rec->prev;
      if (tmp->type == record_reg)
	xfree (tmp->u.reg.val);
      else if (tmp->type == record_mem)
	xfree (tmp->u.mem.val);
      xfree (tmp);
    }

  if (rec != &record_first)
    xfree (rec);

  record_list = &record_first;
  record_arch_list_tail = NULL;
  record_arch_list_tail = NULL;
  record_insn_num = 0;
}

static void
record_list_release_next (void)
{
  struct record_entry *rec = record_list;
  struct record_entry *tmp = rec->next;
  rec->next = NULL;
  while (tmp)
    {
      rec = tmp->next;
      if (tmp->type == record_reg)
	record_insn_num--;
      else if (tmp->type == record_reg)
	xfree (tmp->u.reg.val);
      else if (tmp->type == record_mem)
	xfree (tmp->u.mem.val);
      xfree (tmp);
      tmp = rec;
    }
}

static void
record_list_release_first (void)
{
  struct record_entry *tmp = NULL;
  enum record_type type;

  if (!record_first.next)
    return;

  while (1)
    {
      type = record_first.next->type;

      if (type == record_reg)
	xfree (record_first.next->u.reg.val);
      else if (type == record_mem)
	xfree (record_first.next->u.mem.val);
      tmp = record_first.next;
      record_first.next = tmp->next;
      xfree (tmp);

      if (!record_first.next)
	{
	  gdb_assert (record_insn_num == 1);
	  break;
	}

      record_first.next->prev = &record_first;

      if (type == record_end)
	break;
    }

  record_insn_num--;
}

/* Add a struct record_entry to record_arch_list.  */

static void
record_arch_list_add (struct record_entry *rec)
{
  if (record_debug > 1)
    fprintf_unfiltered (gdb_stdlog,
			"Process record: record_arch_list_add %s.\n",
			host_address_to_string (rec));

  if (record_arch_list_tail)
    {
      record_arch_list_tail->next = rec;
      rec->prev = record_arch_list_tail;
      record_arch_list_tail = rec;
    }
  else
    {
      record_arch_list_head = rec;
      record_arch_list_tail = rec;
    }
}

/* Record the value of a register NUM to record_arch_list.  */

int
record_arch_list_add_reg (struct regcache *regcache, int num)
{
  struct record_entry *rec;

  if (record_debug > 1)
    fprintf_unfiltered (gdb_stdlog,
			"Process record: add register num = %d to "
			"record list.\n",
			num);

  rec = (struct record_entry *) xmalloc (sizeof (struct record_entry));
  rec->u.reg.val = (gdb_byte *) xcalloc (1, MAX_REGISTER_SIZE);
  rec->prev = NULL;
  rec->next = NULL;
  rec->type = record_reg;
  rec->u.reg.num = num;

  regcache_raw_read (regcache, num, rec->u.reg.val);

  record_arch_list_add (rec);

  return 0;
}

/* Record the value of a region of memory whose address is ADDR and
   length is LEN to record_arch_list.  */

int
record_arch_list_add_mem (CORE_ADDR addr, int len)
{
  struct record_entry *rec;

  if (record_debug > 1)
    fprintf_unfiltered (gdb_stdlog,
			"Process record: add mem addr = %s len = %d to "
			"record list.\n",
			paddress (target_gdbarch, addr), len);

  if (!addr)
    return 0;

  rec = (struct record_entry *) xmalloc (sizeof (struct record_entry));
  rec->u.mem.val = (gdb_byte *) xmalloc (len);
  rec->prev = NULL;
  rec->next = NULL;
  rec->type = record_mem;
  rec->u.mem.addr = addr;
  rec->u.mem.len = len;
  rec->u.mem.mem_entry_not_accessible = 0;

  if (target_read_memory (addr, rec->u.mem.val, len))
    {
      if (record_debug)
	fprintf_unfiltered (gdb_stdlog,
			    "Process record: error reading memory at "
			    "addr = %s len = %d.\n",
			    paddress (target_gdbarch, addr), len);
      xfree (rec->u.mem.val);
      xfree (rec);
      return -1;
    }

  record_arch_list_add (rec);

  return 0;
}

/* Add a record_end type struct record_entry to record_arch_list.  */

int
record_arch_list_add_end (void)
{
  struct record_entry *rec;

  if (record_debug > 1)
    fprintf_unfiltered (gdb_stdlog,
			"Process record: add end to arch list.\n");

  rec = (struct record_entry *) xmalloc (sizeof (struct record_entry));
  rec->prev = NULL;
  rec->next = NULL;
  rec->type = record_end;

  record_arch_list_add (rec);

  return 0;
}

static void
record_check_insn_num (int set_terminal)
{
  if (record_insn_max_num)
    {
      gdb_assert (record_insn_num <= record_insn_max_num);
      if (record_insn_num == record_insn_max_num)
	{
	  /* Ask user what to do.  */
	  if (record_stop_at_limit)
	    {
	      int q;
	      if (set_terminal)
		target_terminal_ours ();
	      q = yquery (_("Do you want to auto delete previous execution "
			    "log entries when record/replay buffer becomes "
			    "full (record stop-at-limit)?"));
	      if (set_terminal)
		target_terminal_inferior ();
	      if (q)
		record_stop_at_limit = 0;
	      else
		error (_("Process record: inferior program stopped."));
	    }
	}
    }
}

/* Before inferior step (when GDB record the running message, inferior
   only can step), GDB will call this function to record the values to
   record_list.  This function will call gdbarch_process_record to
   record the running message of inferior and set them to
   record_arch_list, and add it to record_list.  */

static void
record_message_cleanups (void *ignore)
{
  record_list_release (record_arch_list_tail);
}

static int
record_message (void *args)
{
  int ret;
  struct regcache *regcache = args;
  struct cleanup *old_cleanups = make_cleanup (record_message_cleanups, 0);

  record_arch_list_head = NULL;
  record_arch_list_tail = NULL;

  /* Check record_insn_num.  */
  record_check_insn_num (1);

  ret = gdbarch_process_record (get_regcache_arch (regcache),
				regcache,
				regcache_read_pc (regcache));
  if (ret > 0)
    error (_("Process record: inferior program stopped."));
  if (ret < 0)
    error (_("Process record: failed to record execution log."));

  discard_cleanups (old_cleanups);

  record_list->next = record_arch_list_head;
  record_arch_list_head->prev = record_list;
  record_list = record_arch_list_tail;

  if (record_insn_num == record_insn_max_num && record_insn_max_num)
    record_list_release_first ();
  else
    record_insn_num++;

  return 1;
}

static int
do_record_message (struct regcache *regcache)
{
  return catch_errors (record_message, regcache, NULL, RETURN_MASK_ALL);
}

/* Set to 1 if record_store_registers and record_xfer_partial
   doesn't need record.  */

static int record_gdb_operation_disable = 0;

struct cleanup *
record_gdb_operation_disable_set (void)
{
  struct cleanup *old_cleanups = NULL;

  old_cleanups =
    make_cleanup_restore_integer (&record_gdb_operation_disable);
  record_gdb_operation_disable = 1;

  return old_cleanups;
}

static inline void
record_exec_entry (struct regcache *regcache, struct gdbarch *gdbarch,
                   struct record_entry *entry)
{
  switch (entry->type)
    {
    case record_reg: /* reg */
      {
        gdb_byte reg[MAX_REGISTER_SIZE];

        if (record_debug > 1)
          fprintf_unfiltered (gdb_stdlog,
                              "Process record: record_reg %s to "
                              "inferior num = %d.\n",
                              host_address_to_string (entry),
                              entry->u.reg.num);

        memset (reg, 0, sizeof (reg));
        regcache_cooked_read (regcache, entry->u.reg.num, reg);
        regcache_cooked_write (regcache, entry->u.reg.num, entry->u.reg.val);
        memcpy (entry->u.reg.val, reg, MAX_REGISTER_SIZE);
      }
      break;

    case record_mem: /* mem */
      {
	/* Nothing to do if the entry is flagged not_accessible.  */
        if (!record_list->u.mem.mem_entry_not_accessible)
          {
            gdb_byte *mem = alloca (entry->u.mem.len);

            if (record_debug > 1)
              fprintf_unfiltered (gdb_stdlog,
                                  "Process record: record_mem %s to "
                                  "inferior addr = %s len = %d.\n",
                                  host_address_to_string (entry),
                                  paddress (gdbarch, entry->u.mem.addr),
                                  record_list->u.mem.len);

            if (target_read_memory (entry->u.mem.addr, mem, entry->u.mem.len))
              {
                if (execution_direction == EXEC_REVERSE)
                  {
		    /* Read failed -- 
		       flag entry as not_accessible.  */
                    record_list->u.mem.mem_entry_not_accessible = 1;
                    if (record_debug)
                      warning (_("Process record: error reading memory at "
                                 "addr = %s len = %d."),
                               paddress (gdbarch, entry->u.mem.addr),
                               entry->u.mem.len);
                  }
                else
                  error (_("Process record: error reading memory at "
                           "addr = %s len = %d."),
                         paddress (gdbarch, entry->u.mem.addr),
			 entry->u.mem.len);
              }
            else
              {
                if (target_write_memory (entry->u.mem.addr, entry->u.mem.val,
                                         entry->u.mem.len))
                  {
                    if (execution_direction == EXEC_REVERSE)
                      {
			/* Write failed -- 
			   flag entry as not_accessible.  */
                        record_list->u.mem.mem_entry_not_accessible = 1;
                        if (record_debug)
                          warning (_("Process record: error writing memory at "
                                     "addr = %s len = %d."),
                                   paddress (gdbarch, entry->u.mem.addr),
                                   entry->u.mem.len);
                      }
                    else
                      error (_("Process record: error writing memory at "
                               "addr = %s len = %d."),
                             paddress (gdbarch, entry->u.mem.addr),
			     entry->u.mem.len);
                  }
		else
		  memcpy (entry->u.mem.val, mem, entry->u.mem.len);
              }
          }
      }
      break;
    }
}

static void
record_open (char *name, int from_tty)
{
  struct target_ops *t;

  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, "Process record: record_open\n");

  /* check exec */
  if (!target_has_execution)
    error (_("Process record: the program is not being run."));
  if (non_stop)
    error (_("Process record target can't debug inferior in non-stop mode "
	     "(non-stop)."));
  if (target_async_permitted)
    error (_("Process record target can't debug inferior in asynchronous "
	     "mode (target-async)."));

  if (!gdbarch_process_record_p (target_gdbarch))
    error (_("Process record: the current architecture doesn't support "
	     "record function."));

  /* Check if record target is already running.  */
  if (current_target.to_stratum == record_stratum)
    {
      if (!nquery
	  (_("Process record target already running, do you want to delete "
	     "the old record log?")))
	return;
    }

  /*Reset the beneath function pointers.  */
  record_beneath_to_resume = NULL;
  record_beneath_to_wait = NULL;
  record_beneath_to_store_registers = NULL;
  record_beneath_to_xfer_partial = NULL;
  record_beneath_to_insert_breakpoint = NULL;
  record_beneath_to_remove_breakpoint = NULL;

  /* Set the beneath function pointers.  */
  for (t = current_target.beneath; t != NULL; t = t->beneath)
    {
      if (!record_beneath_to_resume)
        {
	  record_beneath_to_resume = t->to_resume;
	  record_beneath_to_resume_ops = t;
        }
      if (!record_beneath_to_wait)
        {
	  record_beneath_to_wait = t->to_wait;
	  record_beneath_to_wait_ops = t;
        }
      if (!record_beneath_to_store_registers)
        {
	  record_beneath_to_store_registers = t->to_store_registers;
	  record_beneath_to_store_registers_ops = t;
        }
      if (!record_beneath_to_xfer_partial)
        {
	  record_beneath_to_xfer_partial = t->to_xfer_partial;
	  record_beneath_to_xfer_partial_ops = t;
        }
      if (!record_beneath_to_insert_breakpoint)
	record_beneath_to_insert_breakpoint = t->to_insert_breakpoint;
      if (!record_beneath_to_remove_breakpoint)
	record_beneath_to_remove_breakpoint = t->to_remove_breakpoint;
    }
  if (!record_beneath_to_resume)
    error (_("Process record can't get to_resume."));
  if (!record_beneath_to_wait)
    error (_("Process record can't get to_wait."));
  if (!record_beneath_to_store_registers)
    error (_("Process record can't get to_store_registers."));
  if (!record_beneath_to_xfer_partial)
    error (_("Process record can't get to_xfer_partial."));
  if (!record_beneath_to_insert_breakpoint)
    error (_("Process record can't get to_insert_breakpoint."));
  if (!record_beneath_to_remove_breakpoint)
    error (_("Process record can't get to_remove_breakpoint."));

  push_target (&record_ops);

  /* Reset */
  record_insn_num = 0;
  record_list = &record_first;
  record_list->next = NULL;
}

static void
record_close (int quitting)
{
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, "Process record: record_close\n");

  record_list_release (record_list);
}

static int record_resume_step = 0;
static enum target_signal record_resume_siggnal;
static int record_resume_error;

static void
record_resume (struct target_ops *ops, ptid_t ptid, int step,
               enum target_signal siggnal)
{
  record_resume_step = step;
  record_resume_siggnal = siggnal;

  if (!RECORD_IS_REPLAY)
    {
      if (do_record_message (get_current_regcache ()))
        {
          record_resume_error = 0;
        }
      else
        {
          record_resume_error = 1;
          return;
        }
      record_beneath_to_resume (record_beneath_to_resume_ops, ptid, 1,
                                siggnal);
    }
}

static int record_get_sig = 0;

static void
record_sig_handler (int signo)
{
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, "Process record: get a signal\n");

  /* It will break the running inferior in replay mode.  */
  record_resume_step = 1;

  /* It will let record_wait set inferior status to get the signal
     SIGINT.  */
  record_get_sig = 1;
}

static void
record_wait_cleanups (void *ignore)
{
  if (execution_direction == EXEC_REVERSE)
    {
      if (record_list->next)
	record_list = record_list->next;
    }
  else
    record_list = record_list->prev;
}

/* In replay mode, this function examines the recorded log and
   determines where to stop.  */

static ptid_t
record_wait (struct target_ops *ops,
	     ptid_t ptid, struct target_waitstatus *status,
	     int options)
{
  struct cleanup *set_cleanups = record_gdb_operation_disable_set ();

  if (record_debug)
    fprintf_unfiltered (gdb_stdlog,
			"Process record: record_wait "
			"record_resume_step = %d\n",
			record_resume_step);

  if (!RECORD_IS_REPLAY)
    {
      if (record_resume_error)
	{
	  /* If record_resume get error, return directly.  */
	  status->kind = TARGET_WAITKIND_STOPPED;
	  status->value.sig = TARGET_SIGNAL_ABRT;
	  return inferior_ptid;
	}

      if (record_resume_step)
	{
	  /* This is a single step.  */
	  return record_beneath_to_wait (record_beneath_to_wait_ops,
					 ptid, status, 0);
	}
      else
	{
	  /* This is not a single step.  */
	  ptid_t ret;
	  CORE_ADDR tmp_pc;

	  while (1)
	    {
	      ret = record_beneath_to_wait (record_beneath_to_wait_ops,
					    ptid, status, 0);

	      if (status->kind == TARGET_WAITKIND_STOPPED
		  && status->value.sig == TARGET_SIGNAL_TRAP)
		{
		  /* Check if there is a breakpoint.  */
		  registers_changed ();
		  tmp_pc = regcache_read_pc (get_current_regcache ());
		  if (breakpoint_inserted_here_p (tmp_pc))
		    {
		      /* There is a breakpoint.  */
		      CORE_ADDR decr_pc_after_break =
			gdbarch_decr_pc_after_break
			(get_regcache_arch (get_current_regcache ()));
		      if (decr_pc_after_break)
			{
			  regcache_write_pc (get_thread_regcache (ret),
					     tmp_pc + decr_pc_after_break);
			}
		    }
		  else
		    {
		      /* There is not a breakpoint.  */
		      if (!do_record_message (get_current_regcache ()))
			{
                          break;
			}
		      record_beneath_to_resume (record_beneath_to_resume_ops,
						ptid, 1,
						record_resume_siggnal);
		      continue;
		    }
		}

	      /* The inferior is broken by a breakpoint or a signal.  */
	      break;
	    }

	  return ret;
	}
    }
  else
    {
      struct regcache *regcache = get_current_regcache ();
      struct gdbarch *gdbarch = get_regcache_arch (regcache);
      int continue_flag = 1;
      int first_record_end = 1;
      struct cleanup *old_cleanups = make_cleanup (record_wait_cleanups, 0);
      CORE_ADDR tmp_pc;

      status->kind = TARGET_WAITKIND_STOPPED;

      /* Check breakpoint when forward execute.  */
      if (execution_direction == EXEC_FORWARD)
	{
	  tmp_pc = regcache_read_pc (regcache);
	  if (breakpoint_inserted_here_p (tmp_pc))
	    {
	      if (record_debug)
		fprintf_unfiltered (gdb_stdlog,
				    "Process record: break at %s.\n",
				    paddress (gdbarch, tmp_pc));
	      if (gdbarch_decr_pc_after_break (gdbarch)
		  && !record_resume_step)
		regcache_write_pc (regcache,
				   tmp_pc +
				   gdbarch_decr_pc_after_break (gdbarch));
	      goto replay_out;
	    }
	}

      record_get_sig = 0;
      signal (SIGINT, record_sig_handler);
      /* If GDB is in terminal_inferior mode, it will not get the signal.
         And in GDB replay mode, GDB doesn't need to be in terminal_inferior
         mode, because inferior will not executed.
         Then set it to terminal_ours to make GDB get the signal.  */
      target_terminal_ours ();

      /* In EXEC_FORWARD mode, record_list points to the tail of prev
         instruction.  */
      if (execution_direction == EXEC_FORWARD && record_list->next)
	record_list = record_list->next;

      /* Loop over the record_list, looking for the next place to
	 stop.  */
      do
	{
	  /* Check for beginning and end of log.  */
	  if (execution_direction == EXEC_REVERSE
	      && record_list == &record_first)
	    {
	      /* Hit beginning of record log in reverse.  */
	      status->kind = TARGET_WAITKIND_NO_HISTORY;
	      break;
	    }
	  if (execution_direction != EXEC_REVERSE && !record_list->next)
	    {
	      /* Hit end of record log going forward.  */
	      status->kind = TARGET_WAITKIND_NO_HISTORY;
	      break;
	    }

          record_exec_entry (regcache, gdbarch, record_list);

	  if (record_list->type == record_end)
	    {
	      if (record_debug > 1)
		fprintf_unfiltered (gdb_stdlog,
				    "Process record: record_end %s to "
				    "inferior.\n",
				    host_address_to_string (record_list));

	      if (first_record_end && execution_direction == EXEC_REVERSE)
		{
		  /* When reverse excute, the first record_end is the part of
		     current instruction.  */
		  first_record_end = 0;
		}
	      else
		{
		  /* In EXEC_REVERSE mode, this is the record_end of prev
		     instruction.
		     In EXEC_FORWARD mode, this is the record_end of current
		     instruction.  */
		  /* step */
		  if (record_resume_step)
		    {
		      if (record_debug > 1)
			fprintf_unfiltered (gdb_stdlog,
					    "Process record: step.\n");
		      continue_flag = 0;
		    }

		  /* check breakpoint */
		  tmp_pc = regcache_read_pc (regcache);
		  if (breakpoint_inserted_here_p (tmp_pc))
		    {
		      if (record_debug)
			fprintf_unfiltered (gdb_stdlog,
					    "Process record: break "
					    "at %s.\n",
					    paddress (gdbarch, tmp_pc));
		      if (gdbarch_decr_pc_after_break (gdbarch)
			  && execution_direction == EXEC_FORWARD
			  && !record_resume_step)
			regcache_write_pc (regcache,
					   tmp_pc +
					   gdbarch_decr_pc_after_break (gdbarch));
		      continue_flag = 0;
		    }
		}
	    }

	  if (continue_flag)
	    {
	      if (execution_direction == EXEC_REVERSE)
		{
		  if (record_list->prev)
		    record_list = record_list->prev;
		}
	      else
		{
		  if (record_list->next)
		    record_list = record_list->next;
		}
	    }
	}
      while (continue_flag);

      signal (SIGINT, handle_sigint);

replay_out:
      if (record_get_sig)
	status->value.sig = TARGET_SIGNAL_INT;
      else
	status->value.sig = TARGET_SIGNAL_TRAP;

      discard_cleanups (old_cleanups);
    }

  do_cleanups (set_cleanups);
  return inferior_ptid;
}

static void
record_disconnect (struct target_ops *target, char *args, int from_tty)
{
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, "Process record: record_disconnect\n");

  unpush_target (&record_ops);
  target_disconnect (args, from_tty);
}

static void
record_detach (struct target_ops *ops, char *args, int from_tty)
{
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, "Process record: record_detach\n");

  unpush_target (&record_ops);
  target_detach (args, from_tty);
}

static void
record_mourn_inferior (struct target_ops *ops)
{
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, "Process record: "
			            "record_mourn_inferior\n");

  unpush_target (&record_ops);
  target_mourn_inferior ();
}

/* Close process record target before killing the inferior process.  */

static void
record_kill (struct target_ops *ops)
{
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, "Process record: record_kill\n");

  unpush_target (&record_ops);
  target_kill ();
}

/* Record registers change (by user or by GDB) to list as an instruction.  */

static void
record_registers_change (struct regcache *regcache, int regnum)
{
  /* Check record_insn_num.  */
  record_check_insn_num (0);

  record_arch_list_head = NULL;
  record_arch_list_tail = NULL;

  if (regnum < 0)
    {
      int i;
      for (i = 0; i < gdbarch_num_regs (get_regcache_arch (regcache)); i++)
	{
	  if (record_arch_list_add_reg (regcache, i))
	    {
	      record_list_release (record_arch_list_tail);
	      error (_("Process record: failed to record execution log."));
	    }
	}
    }
  else
    {
      if (record_arch_list_add_reg (regcache, regnum))
	{
	  record_list_release (record_arch_list_tail);
	  error (_("Process record: failed to record execution log."));
	}
    }
  if (record_arch_list_add_end ())
    {
      record_list_release (record_arch_list_tail);
      error (_("Process record: failed to record execution log."));
    }
  record_list->next = record_arch_list_head;
  record_arch_list_head->prev = record_list;
  record_list = record_arch_list_tail;

  if (record_insn_num == record_insn_max_num && record_insn_max_num)
    record_list_release_first ();
  else
    record_insn_num++;
}

static void
record_store_registers (struct target_ops *ops, struct regcache *regcache,
                        int regno)
{
  if (!record_gdb_operation_disable)
    {
      if (RECORD_IS_REPLAY)
	{
	  int n;
	  struct cleanup *old_cleanups;

	  /* Let user choose if he wants to write register or not.  */
	  if (regno < 0)
	    n =
	      nquery (_("Because GDB is in replay mode, changing the "
			"value of a register will make the execution "
			"log unusable from this point onward.  "
			"Change all registers?"));
	  else
	    n =
	      nquery (_("Because GDB is in replay mode, changing the value "
			"of a register will make the execution log unusable "
			"from this point onward.  Change register %s?"),
		      gdbarch_register_name (get_regcache_arch (regcache),
					       regno));

	  if (!n)
	    {
	      /* Invalidate the value of regcache that was set in function
	         "regcache_raw_write".  */
	      if (regno < 0)
		{
		  int i;
		  for (i = 0;
		       i < gdbarch_num_regs (get_regcache_arch (regcache));
		       i++)
		    regcache_invalidate (regcache, i);
		}
	      else
		regcache_invalidate (regcache, regno);

	      error (_("Process record canceled the operation."));
	    }

	  /* Destroy the record from here forward.  */
	  record_list_release_next ();
	}

      record_registers_change (regcache, regno);
    }

  record_beneath_to_store_registers (record_beneath_to_store_registers_ops,
                                     regcache, regno);
}

/* Behavior is conditional on RECORD_IS_REPLAY.
   In replay mode, we cannot write memory unles we are willing to
   invalidate the record/replay log from this point forward.  */

static LONGEST
record_xfer_partial (struct target_ops *ops, enum target_object object,
		     const char *annex, gdb_byte *readbuf,
		     const gdb_byte *writebuf, ULONGEST offset, LONGEST len)
{
  if (!record_gdb_operation_disable
      && (object == TARGET_OBJECT_MEMORY
	  || object == TARGET_OBJECT_RAW_MEMORY) && writebuf)
    {
      if (RECORD_IS_REPLAY)
	{
	  /* Let user choose if he wants to write memory or not.  */
	  if (!nquery (_("Because GDB is in replay mode, writing to memory "
		         "will make the execution log unusable from this "
		         "point onward.  Write memory at address %s?"),
		       paddress (target_gdbarch, offset)))
	    error (_("Process record canceled the operation."));

	  /* Destroy the record from here forward.  */
	  record_list_release_next ();
	}

      /* Check record_insn_num */
      record_check_insn_num (0);

      /* Record registers change to list as an instruction.  */
      record_arch_list_head = NULL;
      record_arch_list_tail = NULL;
      if (record_arch_list_add_mem (offset, len))
	{
	  record_list_release (record_arch_list_tail);
	  if (record_debug)
	    fprintf_unfiltered (gdb_stdlog,
				_("Process record: failed to record "
				  "execution log."));
	  return -1;
	}
      if (record_arch_list_add_end ())
	{
	  record_list_release (record_arch_list_tail);
	  if (record_debug)
	    fprintf_unfiltered (gdb_stdlog,
				_("Process record: failed to record "
				  "execution log."));
	  return -1;
	}
      record_list->next = record_arch_list_head;
      record_arch_list_head->prev = record_list;
      record_list = record_arch_list_tail;

      if (record_insn_num == record_insn_max_num && record_insn_max_num)
	record_list_release_first ();
      else
	record_insn_num++;
    }

  return record_beneath_to_xfer_partial (record_beneath_to_xfer_partial_ops,
                                         object, annex, readbuf, writebuf,
                                         offset, len);
}

/* Behavior is conditional on RECORD_IS_REPLAY.
   We will not actually insert or remove breakpoints when replaying,
   nor when recording.  */

static int
record_insert_breakpoint (struct gdbarch *gdbarch,
			  struct bp_target_info *bp_tgt)
{
  if (!RECORD_IS_REPLAY)
    {
      struct cleanup *old_cleanups = record_gdb_operation_disable_set ();
      int ret = record_beneath_to_insert_breakpoint (gdbarch, bp_tgt);

      do_cleanups (old_cleanups);

      return ret;
    }

  return 0;
}

static int
record_remove_breakpoint (struct gdbarch *gdbarch,
			  struct bp_target_info *bp_tgt)
{
  if (!RECORD_IS_REPLAY)
    {
      struct cleanup *old_cleanups = record_gdb_operation_disable_set ();
      int ret = record_beneath_to_remove_breakpoint (gdbarch, bp_tgt);

      do_cleanups (old_cleanups);

      return ret;
    }

  return 0;
}

static int
record_can_execute_reverse (void)
{
  return 1;
}

static void
init_record_ops (void)
{
  record_ops.to_shortname = "record";
  record_ops.to_longname = "Process record and replay target";
  record_ops.to_doc =
    "Log program while executing and replay execution from log.";
  record_ops.to_open = record_open;
  record_ops.to_close = record_close;
  record_ops.to_resume = record_resume;
  record_ops.to_wait = record_wait;
  record_ops.to_disconnect = record_disconnect;
  record_ops.to_detach = record_detach;
  record_ops.to_mourn_inferior = record_mourn_inferior;
  record_ops.to_kill = record_kill;
  record_ops.to_create_inferior = find_default_create_inferior;
  record_ops.to_store_registers = record_store_registers;
  record_ops.to_xfer_partial = record_xfer_partial;
  record_ops.to_insert_breakpoint = record_insert_breakpoint;
  record_ops.to_remove_breakpoint = record_remove_breakpoint;
  record_ops.to_can_execute_reverse = record_can_execute_reverse;
  record_ops.to_stratum = record_stratum;
  /* Checkpoints */
  record_ops.to_set_checkpoint = record_insert_checkpoint;
  record_ops.to_unset_checkpoint = record_delete_checkpoint;
  record_ops.to_info_checkpoints = record_show_checkpoint_info;
  record_ops.to_restore_checkpoint = record_restore_checkpoint;
  record_ops.to_magic = OPS_MAGIC;
}

static void
show_record_debug (struct ui_file *file, int from_tty,
		   struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Debugging of process record target is %s.\n"),
		    value);
}

/* Alias for "target record".  */

static void
cmd_record_start (char *args, int from_tty)
{
  execute_command ("target record", from_tty);
}

static void
cmd_record_fd_cleanups (void *recfdp)
{
  int recfd = *(int *) recfdp;
  close (recfd);
}

/* Record log save-file format
   Version 1

     Header:
       4 bytes: magic number RECORD_FILE_MAGIC.
                NOTE: be sure to change whenever this file format changes!

     Records: 
      record_end:
       1 byte:  record type (record_end)
      record_reg:
       1 byte:  record type (record_reg)
       8 bytes: register id
      16 bytes: register value
      record_mem:
       1 byte:  record type (record_mem)
       8 bytes: memory address
       8 bytes: memory length
       n bytes: memory value (n == memory length)

   Version 2 (proposed)

     Header:
       4 bytes: magic number RECORD_FILE_MAGIC.
                NOTE: be sure to change whenever this file format changes!
       n bytes: architecture...
       4 bytes: size of register snapshot
       n bytes: register snapshot
       4 bytes: number of section crcs
       n bytes: section names with crcs
       
     Records:
       See version 1.
 */

/* Dump the execution log to a file.  */

#include "elf-bfd.h"

static void
cmd_record_dump (char *args, int from_tty)
{
  char *recfilename, recfilename_buffer[40];
  int recfd;
  struct record_entry *cur_record_list;
  uint32_t magic;
  struct regcache *regcache;
  struct gdbarch *gdbarch;
  struct cleanup *old_cleanups;
  struct cleanup *set_cleanups;
  extern void write_gcore_file (bfd *);
  extern bfd *create_gcore_bfd (char *);
  bfd *obfd;
  int dump_size = 0;
  asection *osec = NULL;
  struct record_entry *p;
  int bfd_offset = 0;


  if (current_target.to_stratum != record_stratum)
    error (_("Process record is not started.\n"));

  if (args && *args)
    recfilename = args;
  else
    {
      /* Default recfile name is "rec.PID".  */
      sprintf (recfilename_buffer, "rec.%d", PIDGET (inferior_ptid));
      recfilename = recfilename_buffer;
    }

  /* Open the dump file.  */
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, 
			_("Saving recording to file '%s'\n"), 
			recfilename);

  /* Open the output file.  */
  obfd = create_gcore_bfd (recfilename);

  /* Need a cleanup that will close the file (FIXME: delete it?).  */
  old_cleanups = make_cleanup_bfd_close (obfd);

  /* Save the current record entry to "cur_record_list".  */
  cur_record_list = record_list;

  /* Get the values of regcache and gdbarch.  */
  regcache = get_current_regcache ();
  gdbarch = get_regcache_arch (regcache);

  /* Disable the GDB operation record.  */
  set_cleanups = record_gdb_operation_disable_set ();

  /* Reverse execute to the begin of record list.  */
  for (; record_list && record_list != &record_first; 
       record_list = record_list->prev)
    record_exec_entry (regcache, gdbarch, record_list);

  /* Compute the size needed for the extra bfd section.  */
  dump_size = 4;	/* magic cookie */
  for (p = &record_first; p; p = p->next)
    switch (p->type)
      {
      case record_end:
	dump_size += 1;
	break;
      case record_reg:
	dump_size += 1 + 8 + MAX_REGISTER_SIZE;
	break;
      case record_mem:
	dump_size += 1 + 8 + 8 + p->u.mem.len;
	break;
      }

  /* Make the new bfd section.  */
  osec = bfd_make_section_anyway (obfd, "precord");
  bfd_set_section_size (obfd, osec, dump_size);
  bfd_set_section_vma (obfd, osec, 0);
  bfd_section_lma (obfd, osec) = 0;
  bfd_set_section_flags (obfd, osec, SEC_ALLOC | SEC_HAS_CONTENTS);

  /* Save corefile state.  */
  write_gcore_file (obfd);

  /* Write out the record log (modified Hui method).  */
  /* Write the magic code.  */
  magic = RECORD_FILE_MAGIC;
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, _("\
  Writing 4-byte magic cookie RECORD_FILE_MAGIC (0x%08x)\n"),
			magic);
  if (bfd_set_section_contents (obfd, osec, &magic, 
				bfd_offset, sizeof (magic)))
    bfd_offset += sizeof (magic);
  else
    error (_("Failed to write 'magic' to %s (%s)"), 
	   recfilename, bfd_errmsg (bfd_get_error ()));

  /* Dump the entries into the new bfd section.  */
  for (p = &record_first; p; p = p->next)
    {
      uint8_t tmpu8;
      uint64_t tmpu64;

      tmpu8 = p->type;
      if (bfd_set_section_contents (obfd, osec, &tmpu8, 
				    bfd_offset, sizeof (tmpu8)))
	bfd_offset += sizeof (tmpu8);
      else
	error (_("Failed to write 'type' to %s (%s)"), 
	       recfilename, bfd_errmsg (bfd_get_error ()));

      switch (p->type)
	{
	case record_reg: /* reg */
	  tmpu64 = p->u.reg.num;
	  if (BYTE_ORDER == LITTLE_ENDIAN)
	    tmpu64 = bswap_64 (tmpu64);

	  if (record_debug)
	    fprintf_unfiltered (gdb_stdlog, _("\
  Writing register %d val 0x%016llx (1 plus 8 plus %d bytes)\n"),
				p->u.reg.num,
				*(ULONGEST *) p->u.reg.val, 
				MAX_REGISTER_SIZE);
	  /* FIXME: register num does not need 8 bytes.  */
	  if (bfd_set_section_contents (obfd, osec, &tmpu64,
					bfd_offset, sizeof (tmpu64)))
	    bfd_offset += sizeof (tmpu64);
	  else
	    error (_("Failed to write regnum to %s (%s)"), 
		   recfilename, bfd_errmsg (bfd_get_error ()));

	  /* FIXME: add a len field, and write the smaller value.  */
	  if (bfd_set_section_contents (obfd, osec, 
					p->u.reg.val,
					bfd_offset, 
					MAX_REGISTER_SIZE))
	    bfd_offset += MAX_REGISTER_SIZE;
	  else
	    error (_("Failed to write regval to %s (%s)"), 
		   recfilename, bfd_errmsg (bfd_get_error ()));
	  break;
	case record_mem: /* mem */
	  tmpu64 = p->u.mem.addr;
	  if (BYTE_ORDER == LITTLE_ENDIAN)
	    tmpu64 = bswap_64 (tmpu64);

	  if (record_debug)
	    fprintf_unfiltered (gdb_stdlog, _("\
  Writing memory 0x%08x (1 plus 8 plus 8 bytes plus %d bytes)\n"),
				(unsigned int) p->u.mem.addr,
				p->u.mem.len);
	  if (bfd_set_section_contents (obfd, osec, &tmpu64, 
					bfd_offset, sizeof (tmpu64)))
	    bfd_offset += sizeof (tmpu64);
	  else
	    error (_("Failed to write memaddr to %s (%s)"),
		   recfilename, bfd_errmsg (bfd_get_error ()));

	  tmpu64 = p->u.mem.len;
	  if (BYTE_ORDER == LITTLE_ENDIAN)
	    tmpu64 = bswap_64 (tmpu64);

	  /* FIXME: len does not need 8 bytes.  */
	  if (bfd_set_section_contents (obfd, osec, &tmpu64, 
					bfd_offset, sizeof (tmpu64)))
	    bfd_offset += sizeof (tmpu64);
	  else
	    error (_("Failed to write memlen to %s (%s)"), 
		   recfilename, bfd_errmsg (bfd_get_error ()));

	  if (bfd_set_section_contents (obfd, osec, 
					p->u.mem.val,
					bfd_offset, 
					p->u.mem.len))
	    bfd_offset += p->u.mem.len;
	  else
	    error (_("Failed to write memval to %s (%s)"),
		   recfilename, bfd_errmsg (bfd_get_error ()));
	  break;
	case record_end:
	  /* FIXME: record the contents of record_end rec.  */
	  if (record_debug)
	    fprintf_unfiltered (gdb_stdlog, _("\
  Writing record_end (1 byte)\n"));
	  break;
	}
    }

  /* Now forward-execute back to the saved entry.  */
  for (record_list = &record_first; 
       record_list && record_list != cur_record_list; 
       record_list = record_list->next)
    record_exec_entry (regcache, gdbarch, record_list);

  /* Clean-ups will close the output file and free malloc memory.  */
  do_cleanups (old_cleanups);

  /* Succeeded.  */
  fprintf_filtered (gdb_stdout, "Saved recfile %s.\n", recfilename);
}

static int
bfdcore_read (bfd *obfd, asection *osec, void *buf, int len, int *offset)
{
  int ret = bfd_get_section_contents (obfd, osec, buf, *offset, len);

  if (ret)
    *offset += len;
  return ret;
}

/* Load the execution log from a file.  */

#include "gdbcore.h"
#include <ctype.h>

static void
cmd_record_load (char *args, int from_tty)
{
  extern void nullify_last_target_wait_ptid (void);
  int recfd;
  uint32_t magic;
  struct cleanup *old_cleanups;
  struct cleanup *old_cleanups2;
  struct record_entry *rec;
  int insn_number = 0;
  bfd *core_bfd;
  asection *osec;
  extern bfd *load_corefile (char *, int);

  if (!args || (args && !*args))
    error (_("Argument for filename required.\n"));

  /* Open the load file.  */
  if (record_debug)
    fprintf_unfiltered (gdb_stdlog, 
			_("Restoring recording from file '%s'\n"), args);

  /* Restore corefile regs and mem sections.  */
  core_bfd = load_corefile (args, from_tty);
  old_cleanups = make_cleanup_bfd_close (core_bfd);

  /* Now need to find our special note section.  */
  osec = bfd_get_section_by_name (core_bfd, "null0");
  printf_filtered ("Find precord section %s.\n",
		   osec ? "succeeded" : "failed");

  if (osec)
    {
      int i, len;
      int bfd_offset = 0;

      if (record_debug)
	fprintf_unfiltered (gdb_stdlog, "osec name = '%s'\n",
			    bfd_section_name (core_bfd, osec));
      len = (int) bfd_section_size (core_bfd, osec);
      printf_filtered ("osec size = %d\n", len);

      /* Check the magic code.  */
      if (!bfdcore_read (core_bfd, osec, &magic, 
			 sizeof (magic), &bfd_offset))
	error (_("Failed to read 'magic' from %s (%s)"),
	       args, bfd_errmsg (bfd_get_error ()));

      if (magic != RECORD_FILE_MAGIC)
	error (_("'%s', version mis-match / file format error."), 
	       args);

      if (record_debug)
	fprintf_unfiltered (gdb_stdlog, _("\
  Reading 4-byte magic cookie RECORD_FILE_MAGIC (0x%08x)\n"),
			    magic);
      if (current_target.to_stratum != record_stratum)
	{
	  /* FIXME need cleanup!  We might error out.  */
	  cmd_record_start (NULL, from_tty);
	  printf_unfiltered (_("Auto start process record.\n"));
	}

      /* Free any existing record log, and load the entries in
	 core_bfd to the new record log.  */
      record_list_release (record_arch_list_tail);
      old_cleanups2 = make_cleanup (record_message_cleanups, 0);

      while (1)
	{
	  uint8_t tmpu8;
	  uint64_t tmpu64;

	  /* FIXME: Check offset for end-of-section.  */
	  if (!bfdcore_read (core_bfd, osec, &tmpu8, 
			     sizeof (tmpu8), &bfd_offset))
	    break;

	  switch (tmpu8)
	    {
	    case record_reg: /* reg */
	      /* FIXME: abstract out into an 'insert' function.  */
	      rec = (struct record_entry *) 
		xmalloc (sizeof (struct record_entry));
	      rec->u.reg.val = (gdb_byte *) xcalloc (1, MAX_REGISTER_SIZE);
	      rec->prev = NULL;
	      rec->next = NULL;
	      rec->type = record_reg;
	      /* Get num.  */
	      /* FIXME: register num does not need 8 bytes.  */
	      if (!bfdcore_read (core_bfd, osec, &tmpu64, 
				 sizeof (tmpu64), &bfd_offset))
		error (_("Failed to read regnum from %s (%s)"),
		       args, bfd_errmsg (bfd_get_error ()));

	      if (BYTE_ORDER == LITTLE_ENDIAN)
		tmpu64 = bswap_64 (tmpu64);
	      rec->u.reg.num = tmpu64;

	      /* Get val.  */
	      if (!bfdcore_read (core_bfd, osec, rec->u.reg.val,
				 MAX_REGISTER_SIZE, &bfd_offset))
		error (_("Failed to read regval from  %s (%s)"),
		       args, bfd_errmsg (bfd_get_error ()));

	      if (record_debug)
		fprintf_unfiltered (gdb_stdlog, _("\
  Reading register %d val 0x%016llx (1 plus 8 plus %d bytes)\n"),
				    rec->u.reg.num, 
				    *(ULONGEST *) rec->u.reg.val, 
				    MAX_REGISTER_SIZE);
	      record_arch_list_add (rec);
	      break;

	    case record_mem: /* mem */
	      rec = (struct record_entry *) 
		xmalloc (sizeof (struct record_entry));
	      rec->prev = NULL;
	      rec->next = NULL;
	      rec->type = record_mem;
	      /* Get addr.  */
	      if (!bfdcore_read (core_bfd, osec, &tmpu64, 
				 sizeof (tmpu64), &bfd_offset))
		error (_("Failed to read memaddr from %s (%s)"),
		       args, bfd_errmsg (bfd_get_error ()));
	      if (BYTE_ORDER == LITTLE_ENDIAN)
		tmpu64 = bswap_64 (tmpu64);
	      rec->u.mem.addr = tmpu64;

	      /* Get len.  */
	      /* FIXME: len does not need 8 bytes.  */
	      if (!bfdcore_read (core_bfd, osec, &tmpu64, 
				 sizeof (tmpu64), &bfd_offset))
		error (_("Failed to read memlen from %s (%s)"),
		       args, bfd_errmsg (bfd_get_error ()));
	      if (BYTE_ORDER == LITTLE_ENDIAN)
		tmpu64 = bswap_64 (tmpu64);
	      rec->u.mem.len = tmpu64;

	      rec->u.mem.mem_entry_not_accessible = 0;
	      rec->u.mem.val = (gdb_byte *) xmalloc (rec->u.mem.len);
	      /* Get val.  */
	      if (!bfdcore_read (core_bfd, osec, rec->u.mem.val,
				 rec->u.mem.len, &bfd_offset))
		error (_("Failed to read memval from %s (%s)"),
		       args, bfd_errmsg (bfd_get_error ()));
	      if (record_debug)
		fprintf_unfiltered (gdb_stdlog, _("\
  Reading memory 0x%08x (1 plus 8 plus %d bytes)\n"),
				    (unsigned int) rec->u.mem.addr,
				    rec->u.mem.len);
	      record_arch_list_add (rec);
	      break;

	    case record_end: /* end */
	      /* FIXME: restore the contents of record_end rec.  */
	      rec = (struct record_entry *) 
		xmalloc (sizeof (struct record_entry));
	      rec->prev = NULL;
	      rec->next = NULL;
	      rec->type = record_end;
	      if (record_debug)
		fprintf_unfiltered (gdb_stdlog, _("\
  Reading record_end (one byte)\n"));
	      record_arch_list_add (rec);
	      insn_number ++;
	      break;

	    default:
	      error (_("Format of '%s' is not right."), args);
	      break;
	    }
	}
    }

  discard_cleanups (old_cleanups2);

  /* Add record_arch_list_head to the end of record list.  (??? FIXME)*/
  for (rec = record_list; rec->next; rec = rec->next);
  rec->next = record_arch_list_head;
  record_arch_list_head->prev = rec;

  /* Update record_insn_num and record_insn_max_num.  */
  record_insn_num += insn_number;
  if (record_insn_num > record_insn_max_num)
    {
      record_insn_max_num = record_insn_num;
      warning (_("Auto increase record/replay buffer limit to %d."),
	       record_insn_max_num);
    }

  do_cleanups (old_cleanups);

  /* Succeeded.  */
  fprintf_filtered (gdb_stdout, "Loaded recfile %s.\n", args);
  registers_changed ();
  reinit_frame_cache ();
  nullify_last_target_wait_ptid ();
  print_stack_frame (get_selected_frame (NULL), 1, SRC_AND_LOC);
}

/* Truncate the record log from the present point
   of replay until the end.  */

static void
cmd_record_delete (char *args, int from_tty)
{
  if (current_target.to_stratum == record_stratum)
    {
      if (RECORD_IS_REPLAY)
	{
	  if (!from_tty || query (_("Delete the log from this point forward "
		                    "and begin to record the running message "
		                    "at current PC?")))
	    record_list_release_next ();
	}
      else
	  printf_unfiltered (_("Already at end of record list.\n"));

    }
  else
    printf_unfiltered (_("Process record is not started.\n"));
}

/* Implement the "stoprecord" command.  */

static void
cmd_record_stop (char *args, int from_tty)
{
  if (current_target.to_stratum == record_stratum)
    {
      if (!record_list || !from_tty || query (_("Delete recorded log and "
	                                        "stop recording?")))
	unpush_target (&record_ops);
    }
  else
    printf_unfiltered (_("Process record is not started.\n"));
}

/* Set upper limit of record log size.  */

static void
set_record_insn_max_num (char *args, int from_tty, struct cmd_list_element *c)
{
  if (record_insn_num > record_insn_max_num && record_insn_max_num)
    {
      printf_unfiltered (_("Record instructions number is bigger than "
		           "record instructions max number.  Auto delete "
		           "the first ones?\n"));

      while (record_insn_num > record_insn_max_num)
	record_list_release_first ();
    }
}

/* Print the current index into the record log (number of insns recorded
   so far).  */

static void
show_record_insn_number (char *ignore, int from_tty)
{
  printf_unfiltered (_("Record instruction number is %d.\n"),
		     record_insn_num);
}

static struct cmd_list_element *record_cmdlist, *set_record_cmdlist,
			       *show_record_cmdlist, *info_record_cmdlist;

static void
set_record_command (char *args, int from_tty)
{
  printf_unfiltered (_("\
\"set record\" must be followed by an apporpriate subcommand.\n"));
  help_list (set_record_cmdlist, "set record ", all_commands, gdb_stdout);
}

static void
show_record_command (char *args, int from_tty)
{
  cmd_show_list (show_record_cmdlist, from_tty, "");
}

static void
info_record_command (char *args, int from_tty)
{
  cmd_show_list (info_record_cmdlist, from_tty, "");
}

/*
 * Process Record checkpoint stuff
 */

struct record_checkpoint_info
{
  struct record_entry *position;	/* pointer into the record log */
  int insn_num;				/* numbered position in log */
  CORE_ADDR pc;				/* program counter of checkpoint */
};

static void *
record_insert_checkpoint (struct checkpoint_info *cp, int from_tty)
{
  struct record_checkpoint_info *rp;
  rp = XZALLOC (struct record_checkpoint_info);
  rp->position = record_list;
  rp->insn_num = record_insn_num;
  rp->pc = stop_pc;	/* FIXME: should I use this?  */
  if (from_tty && info_verbose)
    {
      printf_filtered (_(" at pos 0x%08x, count 0x%08x, pc 0x%08x"),
		       (unsigned int) rp->position,
		       (unsigned int) rp->insn_num,
		       (unsigned int) rp->pc);
    }
  return rp;
}

static void
record_delete_checkpoint (struct checkpoint_info *cp, int from_tty)
{
  xfree (cp->client_data);
}

static void
record_show_checkpoint_info (struct checkpoint_info *cp, int from_tty)
{
  struct record_checkpoint_info *re = cp->client_data;

  printf_filtered (_("\
Checkpoint #%d at pos 0x%08x, count 0x%08x, pc 0x%08x\n"),
		   cp->checkpoint_id, 
		   (unsigned int) re->position,
		   (unsigned int) re->insn_num,
		   (unsigned int) re->pc);
}

static void
record_goto_checkpoint (struct record_entry *checkpoint, 
			enum exec_direction_kind dir)
{
  struct cleanup *set_cleanups = record_gdb_operation_disable_set ();
  struct regcache *regcache = get_current_regcache ();
  struct gdbarch *gdbarch = get_regcache_arch (regcache);

  /* Assume everything is valid: we will hit the checkpoint,
     and we will not hit the end of the recording.  */

  if (dir == EXEC_FORWARD)
    record_list = record_list->next;

  do
    {
      record_exec_entry (regcache, gdbarch, record_list);
      if (dir == EXEC_REVERSE)
	record_list = record_list->prev;
      else
	record_list = record_list->next;
    } while (record_list != checkpoint);
  do_cleanups (set_cleanups);
}

static void
record_restore_checkpoint (struct checkpoint_info *cp, int from_tty)
{
  int i = 0, checkpoint_index = 0, current_index = 0;
  struct record_entry *p;
  struct record_checkpoint_info *rp;

  rp = cp->client_data;
  for (p = &record_first; p != NULL; p = p->next)
    {
      if (p == rp->position)
	checkpoint_index = i;
      if (p == record_list)
	current_index = i;
      if (p->type == record_end)
	i++;
    }
  if (from_tty && info_verbose)
    {
      printf_filtered ("Checkpoint is at index %d\n", checkpoint_index);
      printf_filtered ("Cur point is at index %d\n", current_index);
      printf_filtered ("Counted %d (officially %d)\n", i-1, record_insn_num);
      if (rp->position->type == record_end)
	printf_filtered ("Checkpoint is record_end\n");
      if (record_list->type == record_end)
	printf_filtered ("record_list is record_end\n");
    }

  if (checkpoint_index == 0)
    error (_("Checkpoint index not found.\n"));
  else if (current_index == checkpoint_index)
    error (_("Already at checkpoint.\n"));
  else if (current_index > checkpoint_index)
    {
      if (from_tty)
	printf_filtered ("Go backward to checkpoint.\n");
      record_goto_checkpoint (rp->position, EXEC_REVERSE);
    }
  else
    {
      if (from_tty)
	printf_filtered ("Go forward to checkpoint.\n");
      record_goto_checkpoint (rp->position, EXEC_FORWARD);
    }
}

void
_initialize_record (void)
{
  struct cmd_list_element *c;

  /* Init record_first.  */
  record_first.prev = NULL;
  record_first.next = NULL;
  record_first.type = record_end;

  init_record_ops ();
  add_target (&record_ops);

  add_setshow_zinteger_cmd ("record", no_class, &record_debug,
			    _("Set debugging of record/replay feature."),
			    _("Show debugging of record/replay feature."),
			    _("When enabled, debugging output for "
			      "record/replay feature is displayed."),
			    NULL, show_record_debug, &setdebuglist,
			    &showdebuglist);

  add_prefix_cmd ("record", class_obscure, cmd_record_start,
		  _("Abbreviated form of \"target record\" command."),
 		  &record_cmdlist, "record ", 0, &cmdlist);
  add_com_alias ("rec", "record", class_obscure, 1);
  add_prefix_cmd ("record", class_support, set_record_command,
		  _("Set record options"), &set_record_cmdlist,
		  "set record ", 0, &setlist);
  add_alias_cmd ("rec", "record", class_obscure, 1, &setlist);
  add_prefix_cmd ("record", class_support, show_record_command,
		  _("Show record options"), &show_record_cmdlist,
		  "show record ", 0, &showlist);
  add_alias_cmd ("rec", "record", class_obscure, 1, &showlist);
  add_prefix_cmd ("record", class_support, info_record_command,
		  _("Info record options"), &info_record_cmdlist,
		  "info record ", 0, &infolist);
  add_alias_cmd ("rec", "record", class_obscure, 1, &infolist);

  c = add_cmd ("dump", class_obscure, cmd_record_dump,
	       _("Dump the execution log to a file.\n\
Argument is optional filename.  Default filename is 'rec.<process_id>'."),
               &record_cmdlist);
  set_cmd_completer (c, filename_completer);
  c = add_cmd ("load", class_obscure, cmd_record_load,
	       _("Load the execution log from a file.  Argument is filename."),
               &record_cmdlist);
  set_cmd_completer (c, filename_completer);

  add_cmd ("delete", class_obscure, cmd_record_delete,
	   _("Delete the rest of execution log and start recording it anew."),
           &record_cmdlist);
  add_alias_cmd ("d", "delete", class_obscure, 1, &record_cmdlist);
  add_alias_cmd ("del", "delete", class_obscure, 1, &record_cmdlist);

  add_cmd ("stop", class_obscure, cmd_record_stop,
	   _("Stop the record/replay target."),
           &record_cmdlist);
  add_alias_cmd ("s", "stop", class_obscure, 1, &record_cmdlist);

  /* Record instructions number limit command.  */
  add_setshow_boolean_cmd ("stop-at-limit", no_class,
			   &record_stop_at_limit, _("\
Set whether record/replay stops when record/replay buffer becomes full."), _("\
Show whether record/replay stops when record/replay buffer becomes full."), _("\
Default is ON.\n\
When ON, if the record/replay buffer becomes full, ask user what to do.\n\
When OFF, if the record/replay buffer becomes full,\n\
delete the oldest recorded instruction to make room for each new one."),
			   NULL, NULL,
			   &set_record_cmdlist, &show_record_cmdlist);
  add_setshow_zinteger_cmd ("insn-number-max", no_class,
			    &record_insn_max_num,
			    _("Set record/replay buffer limit."),
			    _("Show record/replay buffer limit."), _("\
Set the maximum number of instructions to be stored in the\n\
record/replay buffer.  Zero means unlimited.  Default is 200000."),
			    set_record_insn_max_num,
			    NULL, &set_record_cmdlist, &show_record_cmdlist);
  add_cmd ("insn-number", class_obscure, show_record_insn_number,
	    _("Show the current number of instructions in the "
	      "record/replay buffer."), &info_record_cmdlist);
}
