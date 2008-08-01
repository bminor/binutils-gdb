/* Record and reverse target for GDB, the GNU debugger.

   Copyright (C) 2008 Free Software Foundation, Inc.

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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "target.h"
#include "gdbcmd.h"
#include "regcache.h"
#include "inferior.h"
#include "gdbthread.h"
#include "record.h"

#include <signal.h>

#define DEFAULT_RECORD_INSN_MAX_NUM	200000

int record_debug = 0;

record_t record_first;
record_t *record_list = &record_first;
int record_list_status = 1;	/* 0 normal 1 to the begin 2 to the end */
record_t *record_arch_list_head = NULL;
record_t *record_arch_list_tail = NULL;
struct regcache *record_regcache = NULL;

extern void displaced_step_fixup (ptid_t event_ptid,
				  enum target_signal signal);

/* 0 ask user. 1 auto delete the last record_t. */
static int record_insn_max_mode = 0;
static int record_insn_max_num = DEFAULT_RECORD_INSN_MAX_NUM;
static int record_insn_num = 0;

struct target_ops record_ops;
int record_resume_step = 0;
enum exec_direction_kind record_execdir = EXEC_FORWARD;
static int record_get_sig = 0;
static sigset_t record_maskall;
static int record_not_record = 0;
int record_regcache_raw_write_regnum = 0;
int record_will_store_registers = 0;

extern struct bp_location *bp_location_chain;
extern ptid_t displaced_step_ptid;
extern CORE_ADDR displaced_step_original, displaced_step_copy;

static void
record_list_release (record_t * rec)
{
  record_t *tmp;

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
	{
	  xfree (tmp->u.reg.val);
	}
      else if (tmp->type == record_mem)
	{
	  xfree (tmp->u.mem.val);
	}
      xfree (tmp);
    }

  if (rec != &record_first)
    {
      xfree (rec);
    }
}

static void
record_list_release_next (void)
{
  record_t *rec = record_list;
  record_t *tmp = rec->next;
  rec->next = NULL;
  while (tmp)
    {
      rec = tmp->next;
      if (tmp->type == record_reg)
	{
	  record_insn_num--;
	}
      else if (tmp->type == record_reg)
	{
	  xfree (tmp->u.reg.val);
	}
      else if (tmp->type == record_mem)
	{
	  xfree (tmp->u.mem.val);
	}
      xfree (tmp);
      tmp = rec;
    }
}

static void
record_list_release_first (void)
{
  record_t *tmp = NULL;
  enum record_type type;

  if (!record_first.next)
    {
      return;
    }

  while (1)
    {
      type = record_first.next->type;

      if (type == record_reg)
	{
	  xfree (record_first.next->u.reg.val);
	}
      else if (type == record_mem)
	{
	  xfree (record_first.next->u.mem.val);
	}
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
	{
	  break;
	}
    }

  record_insn_num--;
}

/* Add a record_t to "record_arch_list". */
static void
record_arch_list_add (record_t * rec)
{
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

/* Record the value of a register("num") to "record_arch_list". */
int
record_arch_list_add_reg (int num)
{
  record_t *rec;

  if (record_debug > 1)
    {
      fprintf_unfiltered (gdb_stdlog,
			  "Record: add reg num = %d to record list.\n", num);
    }

  rec = (record_t *) xmalloc (sizeof (record_t));
  rec->u.reg.val = (gdb_byte *) xmalloc (MAX_REGISTER_SIZE);
  rec->prev = NULL;
  rec->next = NULL;
  rec->type = record_reg;
  rec->u.reg.num = num;

  regcache_raw_read (record_regcache, num, rec->u.reg.val);

  record_arch_list_add (rec);

  return (0);
}

/* Record the value of a part of memroy that address is "addr" and length is
   "len" to "record_arch_list". */
int
record_arch_list_add_mem (CORE_ADDR addr, int len)
{
  record_t *rec;

  if (record_debug > 1)
    {
      fprintf_unfiltered (gdb_stdlog,
			  "Record: add mem addr = 0x%s len = %d to record list.\n",
			  paddr_nz (addr), len);
    }

  if (!addr)
    {
      return (0);
    }

  rec = (record_t *) xmalloc (sizeof (record_t));
  rec->u.mem.val = (gdb_byte *) xmalloc (len);
  rec->prev = NULL;
  rec->next = NULL;
  rec->type = record_mem;
  rec->u.mem.addr = addr;
  rec->u.mem.len = len;

  if (target_read_memory (addr, rec->u.mem.val, len))
    {
      fprintf_unfiltered (gdb_stdlog,
			  "Record: read memory addr = 0x%s len = %d error.\n",
			  paddr_nz (addr), len);
      xfree (rec->u.mem.val);
      xfree (rec);
      return (-1);
    }

  record_arch_list_add (rec);

  return (0);
}

/* Add a "record_end" type record_t to "record_arch_list". */
int
record_arch_list_add_end (int need_dasm)
{
  record_t *rec;

  if (record_debug > 1)
    {
      fprintf_unfiltered (gdb_stdlog,
			  "Record: add end need_dasm = %d to arch list.\n",
			  need_dasm);
    }

  rec = (record_t *) xmalloc (sizeof (record_t));
  rec->prev = NULL;
  rec->next = NULL;
  rec->type = record_end;

  rec->u.need_dasm = need_dasm;

  record_arch_list_add (rec);

  return (0);
}

static void
record_message_cleanups (void *ignore)
{
  record_list_release (record_arch_list_tail);

  /* Clean for displaced stepping */
  if (!ptid_equal (displaced_step_ptid, null_ptid))
    displaced_step_fixup (displaced_step_ptid, TARGET_SIGNAL_TRAP);

  set_executing (inferior_ptid, 0);
  normal_stop ();
}

/* Before inferior step (When GDB record the running message, inferior only can
   step.), GDB will call this function to record the values to "record_list".
   This function will call "gdbarch_record" to record the running message of
   inferior and set them to "record_arch_list". And add it to "record_list". */

void
record_message (struct gdbarch *gdbarch)
{
  int ret;
  struct cleanup *old_cleanups = make_cleanup (record_message_cleanups, 0);

  record_arch_list_head = NULL;
  record_arch_list_tail = NULL;
  record_regcache = get_current_regcache ();

  /* Check record_insn_num */
  if (record_insn_max_num)
    {
      gdb_assert (record_insn_num <= record_insn_max_num);
      if (record_insn_num == record_insn_max_num)
	{
	  /* Ask user how to do */
	  if (!record_insn_max_mode)
	    {
	      int q;
	      target_terminal_ours ();
	      q =
		yquery (_
			("The record instructions number (record-insn-number) is same with the record instructions max number (record-insn-number-max). Do you want to open auto delete first record_t function (record-auto-delete)?"));
	      target_terminal_inferior ();
	      if (q)
		{
		  record_insn_max_mode = 1;
		}
	      else
		{
		  error (_("Record: record pause the program."));
		}
	    }
	}
    }

  /* Deal with displaced stepping */
  if (!ptid_equal (displaced_step_ptid, null_ptid))
    {
      if (record_debug)
	{
	  fprintf_unfiltered (gdb_stdlog,
			      "Record: this stepping is displaced stepping. Change PC register to original address 0x%s before call gdbarch_record. After that, change it back to 0x%s.\n",
			      paddr_nz (displaced_step_original),
			      paddr_nz (displaced_step_copy));
	}
      regcache_write_pc (record_regcache, displaced_step_original);
      ret = gdbarch_record (gdbarch, displaced_step_copy);
      regcache_write_pc (record_regcache, displaced_step_copy);
    }
  else
    {
      ret = gdbarch_record (gdbarch, regcache_read_pc (record_regcache));
    }

  if (ret > 0)
    error (_("Record: record pause the program."));
  if (ret < 0)
    error (_("Record: record message error."));

  discard_cleanups (old_cleanups);

  record_list->next = record_arch_list_head;
  record_arch_list_head->prev = record_list;
  record_list = record_arch_list_tail;

  if (record_insn_num == record_insn_max_num && record_insn_max_num)
    {
      record_list_release_first ();
    }
  else
    {
      record_insn_num++;
    }
}

/* Things to clean up if we QUIT out of function that set record_not_record.  */
static void
record_not_record_cleanups (void *ignore)
{
  record_not_record = 0;
}

void
record_not_record_set (void)
{
  struct cleanup *old_cleanups = make_cleanup (record_not_record_cleanups, 0);
  record_not_record = 1;
}

static void
record_open (char *name, int from_tty)
{
  if (record_debug)
    {
      fprintf_unfiltered (gdb_stdlog, "Record: record_open\n");
    }

  /* check exec */
  if (!target_has_execution)
    {
      error (_("Record: the program is not being run."));
    }

  if (!gdbarch_record_p (current_gdbarch))
    {
      error (_
	     ("Record: the current architecture don't support record function."));
    }

  /* Check if record target is already running */
  if (RECORD_IS_USED)
    {
      if (!nquery
	  (_
	   ("Record target already running, do you want delete the old running message?")))
	{
	  return;
	}
    }

  record_insn_num = 0;
  push_target (&record_ops);
}

static void
record_close (int quitting)
{
  if (record_debug)
    {
      fprintf_unfiltered (gdb_stdlog, "Record: record_close\n");
    }
  record_list_release (record_list);
  record_list_status = 1;
  record_execdir = EXEC_FORWARD;
}

static void
record_resume (ptid_t ptid, int step, enum target_signal siggnal)
{
  record_resume_step = step;

  if (!RECORD_IS_REPLAY)
    {
      record_message (current_gdbarch);
      return record_ops.beneath->to_resume (ptid, 1, siggnal);
    }
}

static void
record_sig_handler (int signo)
{
  if (record_debug)
    {
      fprintf_unfiltered (gdb_stdlog, "Record: get a signal\n");
    }
  record_resume_step = 1;
  record_get_sig = 1;
}

static ptid_t
record_wait (ptid_t ptid, struct target_waitstatus *status)
{
  if (record_debug)
    {
      fprintf_unfiltered (gdb_stdlog,
			  "Record: record_wait record_resume_step = %d\n",
			  record_resume_step);
    }

  if (!RECORD_IS_REPLAY)
    {
      return record_ops.beneath->to_wait (ptid, status);
    }
  else
    {
      struct sigaction act, old_act;
      int need_dasm = 0;
      struct regcache *regcache = get_current_regcache ();
      int con = 1;
      int first_record_end = 1;

      record_get_sig = 0;
      act.sa_handler = record_sig_handler;
      act.sa_mask = record_maskall;
      act.sa_flags = SA_RESTART;
      if (sigaction (SIGINT, &act, &old_act))
	{
	  perror_with_name (_("Record: sigaction"));
	}
      /* If GDB is in terminal_inferior, it will not get the signal.
         And in GDB replay mode, GDB don't need to in terminal_inferior
         because inferior will not executed.
         Then set it to terminal_ours to make GDB get the signal. */
      target_terminal_ours ();

      do
	{

	  /* check state */
	  if ((record_execdir == EXEC_REVERSE && !record_list->prev
	       && record_list_status == 1) || (record_execdir != EXEC_REVERSE
					       && !record_list->next
					       && record_list_status == 2))
	    {
	      if (record_list_status == 2)
		{
		  fprintf_unfiltered (gdb_stdlog,
				      "Record: running to the end of record list.\n");
		}
	      else if (record_list_status == 1)
		{
		  fprintf_unfiltered (gdb_stdlog,
				      "Record: running to the begin of record list.\n");
		}
	      stop_soon = STOP_QUIETLY;
	      break;
	    }
	  record_list_status = 0;

	  /* set register and memory according to record_list */
	  if (record_list->type == record_reg)
	    {
	      /* reg */
	      gdb_byte reg[MAX_REGISTER_SIZE];
	      if (record_debug > 1)
		{
		  fprintf_unfiltered (gdb_stdlog,
				      "Record: record_reg to inferior num = %d.\n",
				      record_list->u.reg.num);
		}
	      regcache_cooked_read (regcache, record_list->u.reg.num, reg);
	      regcache_cooked_write (regcache, record_list->u.reg.num,
				     record_list->u.reg.val);
	      memcpy (record_list->u.reg.val, reg, MAX_REGISTER_SIZE);
	    }
	  else if (record_list->type == record_mem)
	    {
	      /* mem */
	      gdb_byte *mem = alloca (record_list->u.mem.len);
	      if (record_debug > 1)
		{
		  fprintf_unfiltered (gdb_stdlog,
				      "Record: record_mem to inferior addr = 0x%s len = %d.\n",
				      paddr_nz (record_list->u.mem.addr),
				      record_list->u.mem.len);
		}
	      if (target_read_memory
		  (record_list->u.mem.addr, mem, record_list->u.mem.len))
		{
		  error (_("Record: read memory addr = 0x%s len = %d error."),
			 paddr_nz (record_list->u.mem.addr),
			 record_list->u.mem.len);
		}
	      if (target_write_memory
		  (record_list->u.mem.addr, record_list->u.mem.val,
		   record_list->u.mem.len))
		{
		  error (_
			 ("Record: write memory addr = 0x%s len = %d error."),
			 paddr_nz (record_list->u.mem.addr),
			 record_list->u.mem.len);
		}
	      memcpy (record_list->u.mem.val, mem, record_list->u.mem.len);
	    }
	  else
	    {
	      CORE_ADDR tmp_pc;
	      struct bp_location *bl;
	      struct breakpoint *b;

	      if (record_debug > 1)
		{
		  fprintf_unfiltered (gdb_stdlog,
				      "Record: record_end to inferior need_dasm = %d.\n",
				      record_list->u.need_dasm);
		}

	      if (record_execdir == EXEC_FORWARD)
		{
		  need_dasm = record_list->u.need_dasm;
		}
	      if (need_dasm)
		{
		  gdbarch_record_dasm (current_gdbarch);
		}

	      if (first_record_end && record_execdir == EXEC_REVERSE)
		{
		  /* When reverse excute, the first record_end is the part of current instruction */
		  first_record_end = 0;
		}
	      else
		{
		  /* In EXEC_REVERSE mode, this is the record_end of prev
		     instruction.
		     In EXEC_FORWARD mode, this is the record_end of current
		     instruction. */
		  /* step */
		  if (record_resume_step)
		    {
		      if (record_debug > 1)
			{
			  fprintf_unfiltered (gdb_stdlog, "Record: step.\n");
			}
		      con = 0;
		    }

		  /* check breakpoint */
		  tmp_pc = read_pc ();
		  for (bl = bp_location_chain; bl; bl = bl->global_next)
		    {
		      b = bl->owner;
		      gdb_assert (b);
		      if (b->enable_state != bp_enabled
			  && b->enable_state != bp_permanent)
			continue;

		      if (b->type == bp_watchpoint || b->type == bp_catch_fork
			  || b->type == bp_catch_vfork
			  || b->type == bp_catch_exec
			  || b->type == bp_hardware_watchpoint
			  || b->type == bp_read_watchpoint
			  || b->type == bp_access_watchpoint)
			{
			  continue;
			}
		      if (bl->address == tmp_pc)
			{
			  if (record_debug)
			    {
			      fprintf_unfiltered (gdb_stdlog,
						  "Record: break at 0x%s.\n",
						  paddr_nz (tmp_pc));
			    }
			  con = 0;
			  break;
			}
		    }
		}
	      if (record_execdir == EXEC_REVERSE)
		{
		  need_dasm = record_list->u.need_dasm;
		}
	    }

	  if (record_execdir == EXEC_REVERSE)
	    {
	      if (record_list->prev && con)
		record_list = record_list->prev;
	      else
		record_list_status = 1;
	    }
	  else
	    {
	      if (record_list->next)
		record_list = record_list->next;
	      else
		record_list_status = 2;
	    }

	}
      while (con);

      if (sigaction (SIGALRM, &old_act, NULL))
	{
	  perror_with_name (_("Record: sigaction"));
	}

      status->kind = TARGET_WAITKIND_STOPPED;
      if (record_get_sig)
	{
	  status->value.sig = TARGET_SIGNAL_INT;
	}
      else
	{
	  status->value.sig = TARGET_SIGNAL_TRAP;
	}
    }

  return inferior_ptid;
}

static void
record_disconnect (struct target_ops *target, char *args, int from_tty)
{
  if (record_debug)
    {
      fprintf_unfiltered (gdb_stdlog, "Record: record_disconnect\n");
    }
  unpush_target (&record_ops);
  target_disconnect (args, from_tty);
}

static void
record_detach (char *args, int from_tty)
{
  if (record_debug)
    {
      fprintf_unfiltered (gdb_stdlog, "Record: record_detach\n");
    }
  unpush_target (&record_ops);
  target_detach (args, from_tty);
}

/* Close record target before kill the inferior process. */
static void
record_kill (void)
{
  if (record_debug)
    {
      fprintf_unfiltered (gdb_stdlog, "Record: record_kill\n");
    }
  unpush_target (&record_ops);
  target_kill ();
}

/* Record registers change to list as an instruction */
static void
record_registers_change (struct regcache *regcache, int regnum)
{
  record_arch_list_head = NULL;
  record_arch_list_tail = NULL;
  record_regcache = get_current_regcache ();
  if (regnum < 0)
    {
      int i;
      for (i = 0; i < gdbarch_num_regs (get_regcache_arch (regcache)); i++)
	{
	  if (record_arch_list_add_reg (i))
	    {
	      record_list_release (record_arch_list_tail);
	      error (_("Record: record message error."));
	    }
	}
    }
  else
    {
      if (record_arch_list_add_reg (regnum))
	{
	  record_list_release (record_arch_list_tail);
	  error (_("Record: record message error."));
	}
    }
  if (record_arch_list_add_end (0))
    {
      record_list_release (record_arch_list_tail);
      error (_("Record: record message error."));
    }
  record_list->next = record_arch_list_head;
  record_arch_list_head->prev = record_list;
  record_list = record_arch_list_tail;
}

/* XXX: I don't know how to do if GDB call function target_store_registers
   without call function target_prepare_to_store. */
static void
record_prepare_to_store (struct regcache *regcache)
{
  if (!record_not_record)
    {
      if (RECORD_IS_REPLAY)
	{
	  struct cleanup *old_cleanups;
	  /* Let user choice if he want to write register or not. */
	  if (!nquery (_
		       ("Becuse GDB is in replay mode, this operation will destory the record in the next if set the value of register %s. Do you want GDB do it?"),
		       gdbarch_register_name (get_regcache_arch
					      (regcache),
					      record_regcache_raw_write_regnum)))
	    {
	      error (_("Record: record cancel the operation."));
	    }

	  /* Destory the record in the next */
	  record_list_release_next ();
	}

      record_registers_change (regcache, record_regcache_raw_write_regnum);
    }
  record_ops.beneath->to_prepare_to_store (regcache);
}

static LONGEST
record_xfer_partial (struct target_ops *ops, enum target_object object,
		     const char *annex, gdb_byte * readbuf,
		     const gdb_byte * writebuf, ULONGEST offset, LONGEST len)
{
  if (!record_not_record
      && (object == TARGET_OBJECT_MEMORY
	  || object == TARGET_OBJECT_RAW_MEMORY) && writebuf)
    {
      if (RECORD_IS_REPLAY)
	{
	  /* Let user choice if he want to write memory or not. */
	  if (!nquery (_
		       ("Becuse GDB is in replay mode, this operation will destory the record in the next if write memory that addr is 0x%s and size is %lld. Do you want GDB do it?"),
		       paddr_nz (offset), len))
	    {
	      return -1;
	    }

	  /* Destory the record in the next */
	  record_list_release_next ();
	}

      /* Record registers change to list as an instruction */
      record_arch_list_head = NULL;
      record_arch_list_tail = NULL;
      record_regcache = get_current_regcache ();
      if (record_arch_list_add_mem (offset, len))
	{
	  record_list_release (record_arch_list_tail);
	  fprintf_unfiltered (gdb_stdlog, _("Record: record message error."));
	  return -1;
	}
      if (record_arch_list_add_end (0))
	{
	  record_list_release (record_arch_list_tail);
	  fprintf_unfiltered (gdb_stdlog, _("Record: record message error."));
	  return -1;
	}
      record_list->next = record_arch_list_head;
      record_arch_list_head->prev = record_list;
      record_list = record_arch_list_tail;
    }

  return record_ops.beneath->to_xfer_partial (ops, object, annex, readbuf,
					      writebuf, offset, len);
}

static int
record_insert_breakpoint (struct bp_target_info *bp_tgt)
{
  if (!RECORD_IS_REPLAY)
    {
      return record_ops.beneath->to_insert_breakpoint (bp_tgt);
    }

  return 0;
}

static int
record_remove_breakpoint (struct bp_target_info *bp_tgt)
{
  if (!RECORD_IS_REPLAY)
    {
      return record_ops.beneath->to_remove_breakpoint (bp_tgt);
    }

  return 0;
}

static enum exec_direction_kind
record_get_execdir (void)
{
  if (record_debug > 1)
    printf_filtered ("Record: execdir is %s\n",
		     record_execdir == EXEC_FORWARD ? "forward" :
		     record_execdir == EXEC_REVERSE ? "reverse" : "unknown");
  return record_execdir;
}

static int
record_set_execdir (enum exec_direction_kind dir)
{
  if (record_debug)
    printf_filtered ("Record: set execdir: %s\n",
		     dir == EXEC_FORWARD ? "forward" :
		     dir == EXEC_REVERSE ? "reverse" : "bad direction");

  /* FIXME: check target for capability.  */
  if (dir == EXEC_FORWARD || dir == EXEC_REVERSE)
    return (record_execdir = dir);
  else
    return EXEC_ERROR;
}

static void
init_record_ops (void)
{
  record_ops.to_shortname = "record";
  record_ops.to_longname = "Record and reverse target";
  record_ops.to_doc =
    "Record the program running message and replay this message.";
  record_ops.to_open = record_open;
  record_ops.to_close = record_close;
  record_ops.to_resume = record_resume;
  record_ops.to_wait = record_wait;
  record_ops.to_disconnect = record_disconnect;
  record_ops.to_detach = record_detach;
  record_ops.to_kill = record_kill;
  record_ops.to_create_inferior = find_default_create_inferior;	/* Make record suppport command "run". */
  record_ops.to_prepare_to_store = record_prepare_to_store;
  record_ops.to_xfer_partial = record_xfer_partial;
  record_ops.to_insert_breakpoint = record_insert_breakpoint;
  record_ops.to_remove_breakpoint = record_remove_breakpoint;
  record_ops.to_get_execdir = record_get_execdir;
  record_ops.to_set_execdir = record_set_execdir;
  record_ops.to_stratum = record_stratum;
  record_ops.to_magic = OPS_MAGIC;
}

static void
show_record_debug (struct ui_file *file, int from_tty,
		   struct cmd_list_element *c, const char *value)
{
  fprintf_filtered (file, _("Debugging of record target is %s.\n"), value);
}

static void
cmd_record_start (char *args, int from_tty)
{
  execute_command ("target record", from_tty);
}

static void
cmd_record_delete (char *args, int from_tty)
{
  if (RECORD_IS_USED)
    {
      if (RECORD_IS_REPLAY)
	{
	  if (!from_tty || query (_
				  ("Record: delete the next running messages and begin to record the running message at current address?")))
	    {
	      record_list_release_next ();
	    }
	}
      else
	{
	  printf_unfiltered (_
			     ("Record: GDB already at the end of record list.\n"));
	}

    }
  else
    {
      printf_unfiltered (_("Record: record target is not started.\n"));
    }
}

static void
cmd_record_stop (char *args, int from_tty)
{
  if (RECORD_IS_USED)
    {
      if (!record_list || !from_tty || query (_
					      ("Record: delete all the record messages and stop record target?")))
	{
	  unpush_target (&record_ops);
	}
    }
  else
    {
      printf_unfiltered (_("Record: record target is not started.\n"));
    }
}

static void
set_record_insn_max_num (char *args, int from_tty, struct cmd_list_element *c)
{
  if (record_insn_num > record_insn_max_num && record_insn_max_num)
    {
      printf_unfiltered (_
			 ("Record: record instructions number is bigger than record instructions max number. Auto delete the first ones.\n"));

      while (record_insn_num > record_insn_max_num)
	{
	  record_list_release_first ();
	}
    }
}

static void
show_record_insn_number (char *ignore, int from_tty)
{
  printf_unfiltered (_("Record instructions number is %d.\n"),
		     record_insn_num);
}

void
_initialize_record (void)
{
  /* init record_maskall */
  if (sigfillset (&record_maskall) == -1)
    {
      perror_with_name (_("Record: sigfillset"));
    }

  /* init record_first */
  record_first.prev = NULL;
  record_first.next = NULL;
  record_first.type = record_end;
  record_first.u.need_dasm = 0;

  init_record_ops ();
  add_target (&record_ops);

  add_setshow_zinteger_cmd ("record", no_class, &record_debug,
			    _("Set debugging of record/replay feature."),
			    _("Show debugging of record/replay feature."),
			    _("When enabled, debugging output for record/replay feature is displayed."),
			    NULL, show_record_debug, &setdebuglist,
			    &showdebuglist);

  add_com ("record", class_obscure, cmd_record_start,
	   _("Abbreviated form of \"target record\" command."));

  add_com_alias ("rec", "record", class_obscure, 1);

  /* XXX: I try to use some simple commands such as "disconnect" and
     "detach" to support this functions.  But these commands all have
     other affect to GDB such as call function "no_shared_libraries".
     So I add special commands to GDB.  */
  add_com ("delrecord", class_obscure, cmd_record_delete,
	   _("When record target running in replay mode, delete the next running messages and begin to record the running message at current address."));
  add_com_alias ("dr", "delrecord", class_obscure, 1);
  add_com ("stoprecord", class_obscure, cmd_record_stop,
	   _("Stop the record/replay target."));
  add_com_alias ("sr", "stoprecord", class_obscure, 1);

  /* Record instructions number limit command. */
  /* Teawater -- tell me if I got this one wrong.  MVS  */
  add_setshow_zinteger_cmd ("record-auto-delete", no_class,
			    &record_insn_max_mode,
			    _("Set record/replay auto delete mode."),
			    _("Show record/replay auto delete mode."),
			    _("\
When enabled, if the record/replay buffer becomes full,\n\
delete it and start new recording."),
			    NULL, NULL, &setlist, &showlist);
  add_setshow_zinteger_cmd ("record-insn-number-max", no_class,
			    &record_insn_max_num,
			    _("Set record/replay buffer limit."),
			    _("Show record/replay buffer limit."),
			    _("\
Set the maximum number of instructions to be stored in the\n\
record/replay buffer.  Zero means unlimited (default 200000).",
			    set_record_insn_max_num, NULL, &setlist,
			    &showlist);
  add_info ("record-insn-number", show_record_insn_number,
	    _("\
Show the current number of instructions in the record/replay buffer."));
}
