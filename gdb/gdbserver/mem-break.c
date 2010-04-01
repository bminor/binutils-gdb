/* Memory breakpoint operations for the remote server for GDB.
   Copyright (C) 2002, 2003, 2005, 2007, 2008, 2009, 2010
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

#include "server.h"

const unsigned char *breakpoint_data;
int breakpoint_len;

#define MAX_BREAKPOINT_LEN 8

/* GDB will never try to install multiple breakpoints at the same
   address.  But, we need to keep track of internal breakpoints too,
   and so we do need to be able to install multiple breakpoints at the
   same address transparently.  We keep track of two different, and
   closely related structures.  A raw breakpoint, which manages the
   low level, close to the metal aspect of a breakpoint.  It holds the
   breakpoint address, and a buffer holding a copy of the instructions
   that would be in memory had not been a breakpoint there (we call
   that the shadow memory of the breakpoint).  We occasionally need to
   temporarilly uninsert a breakpoint without the client knowing about
   it (e.g., to step over an internal breakpoint), so we keep an
   `inserted' state associated with this low level breakpoint
   structure.  There can only be one such object for a given address.
   Then, we have (a bit higher level) breakpoints.  This structure
   holds a callback to be called whenever a breakpoint is hit, a
   high-level type, and a link to a low level raw breakpoint.  There
   can be many high-level breakpoints at the same address, and all of
   them will point to the same raw breakpoint, which is reference
   counted.  */

/* The low level, physical, raw breakpoint.  */
struct raw_breakpoint
{
  struct raw_breakpoint *next;

  /* A reference count.  Each high level breakpoint referencing this
     raw breakpoint accounts for one reference.  */
  int refcount;

  /* The breakpoint's insertion address.  There can only be one raw
     breakpoint for a given PC.  */
  CORE_ADDR pc;

  /* The breakpoint's shadow memory.  */
  unsigned char old_data[MAX_BREAKPOINT_LEN];

  /* Non-zero if this breakpoint is currently inserted in the
     inferior.  */
  int inserted;
};

/* The type of a breakpoint.  */
enum bkpt_type
  {
    /* A GDB breakpoint, requested with a Z0 packet.  */
    gdb_breakpoint,

    /* A basic-software-single-step breakpoint.  */
    reinsert_breakpoint,

    /* Any other breakpoint type that doesn't require specific
       treatment goes here.  E.g., an event breakpoint.  */
    other_breakpoint,
  };

/* A high level (in gdbserver's perspective) breakpoint.  */
struct breakpoint
{
  struct breakpoint *next;

  /* The breakpoint's type.  */
  enum bkpt_type type;

  /* Link to this breakpoint's raw breakpoint.  This is always
     non-NULL.  */
  struct raw_breakpoint *raw;

  /* Function to call when we hit this breakpoint.  If it returns 1,
     the breakpoint shall be deleted; 0 or if this callback is NULL,
     it will be left inserted.  */
  int (*handler) (CORE_ADDR);
};

static struct raw_breakpoint *
find_raw_breakpoint_at (CORE_ADDR where)
{
  struct process_info *proc = current_process ();
  struct raw_breakpoint *bp;

  for (bp = proc->raw_breakpoints; bp != NULL; bp = bp->next)
    if (bp->pc == where)
      return bp;

  return NULL;
}

static struct raw_breakpoint *
set_raw_breakpoint_at (CORE_ADDR where)
{
  struct process_info *proc = current_process ();
  struct raw_breakpoint *bp;
  int err;

  if (breakpoint_data == NULL)
    error ("Target does not support breakpoints.");

  bp = find_raw_breakpoint_at (where);
  if (bp != NULL)
    {
      bp->refcount++;
      return bp;
    }

  bp = xcalloc (1, sizeof (*bp));
  bp->pc = where;
  bp->refcount = 1;

  err = (*the_target->read_memory) (where, bp->old_data,
				    breakpoint_len);
  if (err != 0)
    {
      if (debug_threads)
	fprintf (stderr,
		 "Failed to read shadow memory of"
		 " breakpoint at 0x%s (%s).\n",
		 paddress (where), strerror (err));
      free (bp);
      return NULL;
    }

  err = (*the_target->write_memory) (where, breakpoint_data,
				     breakpoint_len);
  if (err != 0)
    {
      if (debug_threads)
	fprintf (stderr,
		 "Failed to insert breakpoint at 0x%s (%s).\n",
		 paddress (where), strerror (err));
      free (bp);
      return NULL;
    }

  /* Link the breakpoint in.  */
  bp->inserted = 1;
  bp->next = proc->raw_breakpoints;
  proc->raw_breakpoints = bp;
  return bp;
}

struct breakpoint *
set_breakpoint_at (CORE_ADDR where, int (*handler) (CORE_ADDR))
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp;
  struct raw_breakpoint *raw;

  raw = set_raw_breakpoint_at (where);

  if (raw == NULL)
    {
      /* warn? */
      return NULL;
    }

  bp = xcalloc (1, sizeof (struct breakpoint));
  bp->type = other_breakpoint;

  bp->raw = raw;
  bp->handler = handler;

  bp->next = proc->breakpoints;
  proc->breakpoints = bp;

  return bp;
}

static int
delete_raw_breakpoint (struct process_info *proc, struct raw_breakpoint *todel)
{
  struct raw_breakpoint *bp, **bp_link;
  int ret;

  bp = proc->raw_breakpoints;
  bp_link = &proc->raw_breakpoints;

  while (bp)
    {
      if (bp == todel)
	{
	  if (bp->inserted)
	    {
	      struct raw_breakpoint *prev_bp_link = *bp_link;

	      *bp_link = bp->next;

	      ret = (*the_target->write_memory) (bp->pc, bp->old_data,
						 breakpoint_len);
	      if (ret != 0)
		{
		  /* Something went wrong, relink the breakpoint.  */
		  *bp_link = prev_bp_link;

		  if (debug_threads)
		    fprintf (stderr,
			     "Failed to uninsert raw breakpoint "
			     "at 0x%s (%s) while deleting it.\n",
			     paddress (bp->pc), strerror (ret));
		  return ret;
		}

	    }
	  else
	    *bp_link = bp->next;

	  free (bp);
	  return 0;
	}
      else
	{
	  bp_link = &bp->next;
	  bp = *bp_link;
	}
    }

  warning ("Could not find raw breakpoint in list.");
  return ENOENT;
}

static int
release_breakpoint (struct process_info *proc, struct breakpoint *bp)
{
  int newrefcount;
  int ret;

  newrefcount = bp->raw->refcount - 1;
  if (newrefcount == 0)
    {
      ret = delete_raw_breakpoint (proc, bp->raw);
      if (ret != 0)
	return ret;
    }
  else
    bp->raw->refcount = newrefcount;

  free (bp);

  return 0;
}

static int
delete_breakpoint_1 (struct process_info *proc, struct breakpoint *todel)
{
  struct breakpoint *bp, **bp_link;
  int err;

  bp = proc->breakpoints;
  bp_link = &proc->breakpoints;

  while (bp)
    {
      if (bp == todel)
	{
	  *bp_link = bp->next;

	  err = release_breakpoint (proc, bp);
	  if (err != 0)
	    return err;

	  bp = *bp_link;
	  return 0;
	}
      else
	{
	  bp_link = &bp->next;
	  bp = *bp_link;
	}
    }

  warning ("Could not find breakpoint in list.");
  return ENOENT;
}

static int
delete_breakpoint (struct breakpoint *todel)
{
  struct process_info *proc = current_process ();
  return delete_breakpoint_1 (proc, todel);
}

static struct breakpoint *
find_gdb_breakpoint_at (CORE_ADDR where)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp;

  for (bp = proc->breakpoints; bp != NULL; bp = bp->next)
    if (bp->type == gdb_breakpoint && bp->raw->pc == where)
      return bp;

  return NULL;
}

int
set_gdb_breakpoint_at (CORE_ADDR where)
{
  struct breakpoint *bp;

  if (breakpoint_data == NULL)
    return 1;

  bp = set_breakpoint_at (where, NULL);
  if (bp == NULL)
    return -1;

  bp->type = gdb_breakpoint;
  return 0;
}

int
delete_gdb_breakpoint_at (CORE_ADDR addr)
{
  struct breakpoint *bp;
  int err;

  if (breakpoint_data == NULL)
    return 1;

  bp = find_gdb_breakpoint_at (addr);
  if (bp == NULL)
    return -1;

  err = delete_breakpoint (bp);
  if (err)
    return -1;

  return 0;
}

int
gdb_breakpoint_here (CORE_ADDR where)
{
  struct breakpoint *bp = find_gdb_breakpoint_at (where);

  return (bp != NULL);
}

void
set_reinsert_breakpoint (CORE_ADDR stop_at)
{
  struct breakpoint *bp;

  bp = set_breakpoint_at (stop_at, NULL);
  bp->type = reinsert_breakpoint;
}

void
delete_reinsert_breakpoints (void)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp, **bp_link;

  bp = proc->breakpoints;
  bp_link = &proc->breakpoints;

  while (bp)
    {
      if (bp->type == reinsert_breakpoint)
	{
	  *bp_link = bp->next;
	  release_breakpoint (proc, bp);
	  bp = *bp_link;
	}
      else
	{
	  bp_link = &bp->next;
	  bp = *bp_link;
	}
    }
}

static void
uninsert_raw_breakpoint (struct raw_breakpoint *bp)
{
  if (bp->inserted)
    {
      int err;

      bp->inserted = 0;
      err = (*the_target->write_memory) (bp->pc, bp->old_data,
					 breakpoint_len);
      if (err != 0)
	{
	  bp->inserted = 1;

	  if (debug_threads)
	    fprintf (stderr,
		     "Failed to uninsert raw breakpoint at 0x%s (%s).\n",
		     paddress (bp->pc), strerror (err));
	}
    }
}

void
uninsert_breakpoints_at (CORE_ADDR pc)
{
  struct raw_breakpoint *bp;

  bp = find_raw_breakpoint_at (pc);
  if (bp == NULL)
    {
      /* This can happen when we remove all breakpoints while handling
	 a step-over.  */
      if (debug_threads)
	fprintf (stderr,
		 "Could not find breakpoint at 0x%s "
		 "in list (uninserting).\n",
		 paddress (pc));
      return;
    }

  if (bp->inserted)
    uninsert_raw_breakpoint (bp);
}

static void
reinsert_raw_breakpoint (struct raw_breakpoint *bp)
{
  int err;

  if (bp->inserted)
    error ("Breakpoint already inserted at reinsert time.");

  err = (*the_target->write_memory) (bp->pc, breakpoint_data,
				     breakpoint_len);
  if (err == 0)
    bp->inserted = 1;
  else if (debug_threads)
    fprintf (stderr,
	     "Failed to reinsert breakpoint at 0x%s (%s).\n",
	     paddress (bp->pc), strerror (err));
}

void
reinsert_breakpoints_at (CORE_ADDR pc)
{
  struct raw_breakpoint *bp;

  bp = find_raw_breakpoint_at (pc);
  if (bp == NULL)
    {
      /* This can happen when we remove all breakpoints while handling
	 a step-over.  */
      if (debug_threads)
	fprintf (stderr,
		 "Could not find raw breakpoint at 0x%s "
		 "in list (reinserting).\n",
		 paddress (pc));
      return;
    }

  reinsert_raw_breakpoint (bp);
}

void
check_breakpoints (CORE_ADDR stop_pc)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp, **bp_link;

  bp = proc->breakpoints;
  bp_link = &proc->breakpoints;

  while (bp)
    {
      if (bp->raw->pc == stop_pc)
	{
	  if (!bp->raw->inserted)
	    {
	      warning ("Hit a removed breakpoint?");
	      return;
	    }

	  if (bp->handler != NULL && (*bp->handler) (stop_pc))
	    {
	      *bp_link = bp->next;

	      release_breakpoint (proc, bp);

	      bp = *bp_link;
	      continue;
	    }
	}

      bp_link = &bp->next;
      bp = *bp_link;
    }
}

void
set_breakpoint_data (const unsigned char *bp_data, int bp_len)
{
  breakpoint_data = bp_data;
  breakpoint_len = bp_len;
}

int
breakpoint_here (CORE_ADDR addr)
{
  return (find_raw_breakpoint_at (addr) != NULL);
}

int
breakpoint_inserted_here (CORE_ADDR addr)
{
  struct raw_breakpoint *bp;

  bp = find_raw_breakpoint_at (addr);

  return (bp != NULL && bp->inserted);
}

void
check_mem_read (CORE_ADDR mem_addr, unsigned char *buf, int mem_len)
{
  struct process_info *proc = current_process ();
  struct raw_breakpoint *bp = proc->raw_breakpoints;
  CORE_ADDR mem_end = mem_addr + mem_len;

  for (; bp != NULL; bp = bp->next)
    {
      CORE_ADDR bp_end = bp->pc + breakpoint_len;
      CORE_ADDR start, end;
      int copy_offset, copy_len, buf_offset;

      if (mem_addr >= bp_end)
	continue;
      if (bp->pc >= mem_end)
	continue;

      start = bp->pc;
      if (mem_addr > start)
	start = mem_addr;

      end = bp_end;
      if (end > mem_end)
	end = mem_end;

      copy_len = end - start;
      copy_offset = start - bp->pc;
      buf_offset = start - mem_addr;

      if (bp->inserted)
	memcpy (buf + buf_offset, bp->old_data + copy_offset, copy_len);
    }
}

void
check_mem_write (CORE_ADDR mem_addr, unsigned char *buf, int mem_len)
{
  struct process_info *proc = current_process ();
  struct raw_breakpoint *bp = proc->raw_breakpoints;
  CORE_ADDR mem_end = mem_addr + mem_len;

  for (; bp != NULL; bp = bp->next)
    {
      CORE_ADDR bp_end = bp->pc + breakpoint_len;
      CORE_ADDR start, end;
      int copy_offset, copy_len, buf_offset;

      if (mem_addr >= bp_end)
	continue;
      if (bp->pc >= mem_end)
	continue;

      start = bp->pc;
      if (mem_addr > start)
	start = mem_addr;

      end = bp_end;
      if (end > mem_end)
	end = mem_end;

      copy_len = end - start;
      copy_offset = start - bp->pc;
      buf_offset = start - mem_addr;

      memcpy (bp->old_data + copy_offset, buf + buf_offset, copy_len);
      if (bp->inserted)
	memcpy (buf + buf_offset, breakpoint_data + copy_offset, copy_len);
    }
}

/* Delete all breakpoints, and un-insert them from the inferior.  */

void
delete_all_breakpoints (void)
{
  struct process_info *proc = current_process ();

  while (proc->breakpoints)
    delete_breakpoint_1 (proc, proc->breakpoints);
}

/* Release all breakpoints, but do not try to un-insert them from the
   inferior.  */

void
free_all_breakpoints (struct process_info *proc)
{
  struct raw_breakpoint *raw_bp;

  for (raw_bp = proc->raw_breakpoints; raw_bp != NULL; raw_bp = raw_bp->next)
    raw_bp->inserted = 0;

  /* Note: use PROC explicitly instead of deferring to
     delete_all_breakpoints --- CURRENT_INFERIOR may already have been
     released when we get here.  There should be no call to
     current_process from here on.  */
  while (proc->breakpoints)
    delete_breakpoint_1 (proc, proc->breakpoints);
}
