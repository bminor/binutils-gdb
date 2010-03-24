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

struct breakpoint
{
  struct breakpoint *next;
  CORE_ADDR pc;
  unsigned char old_data[MAX_BREAKPOINT_LEN];

  /* Non-zero if this breakpoint is currently inserted in the
     inferior.  */
  int inserted;

  /* Function to call when we hit this breakpoint.  If it returns 1,
     the breakpoint shall be deleted; 0, it will be left inserted.  */
  int (*handler) (CORE_ADDR);
};

static struct breakpoint *
set_raw_breakpoint_at (CORE_ADDR where)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp;
  int err;

  if (breakpoint_data == NULL)
    error ("Target does not support breakpoints.");

  bp = xcalloc (1, sizeof (*bp));
  bp->pc = where;

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
  bp->next = proc->breakpoints;
  proc->breakpoints = bp;
  return bp;
}

void
set_breakpoint_at (CORE_ADDR where, int (*handler) (CORE_ADDR))
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp;

  bp = set_raw_breakpoint_at (where);

  if (bp == NULL)
    {
      /* warn? */
      return;
    }

  bp = xcalloc (1, sizeof (struct breakpoint));
  bp->handler = handler;

  bp->next = proc->breakpoints;
  proc->breakpoints = bp;
}

static void
delete_breakpoint (struct breakpoint *bp)
{
  struct process_info *proc = current_process ();
  struct breakpoint *cur;

  if (proc->breakpoints == bp)
    {
      proc->breakpoints = bp->next;
      (*the_target->write_memory) (bp->pc, bp->old_data,
				   breakpoint_len);
      free (bp);
      return;
    }
  cur = proc->breakpoints;
  while (cur->next)
    {
      if (cur->next == bp)
	{
	  cur->next = bp->next;
	  (*the_target->write_memory) (bp->pc, bp->old_data,
				       breakpoint_len);
	  free (bp);
	  return;
	}
    }
  warning ("Could not find breakpoint in list.");
}

static struct breakpoint *
find_breakpoint_at (CORE_ADDR where)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp = proc->breakpoints;

  while (bp != NULL)
    {
      if (bp->pc == where)
	return bp;
      bp = bp->next;
    }

  return NULL;
}

void
delete_breakpoint_at (CORE_ADDR addr)
{
  struct breakpoint *bp = find_breakpoint_at (addr);
  if (bp != NULL)
    delete_breakpoint (bp);
}

void
set_reinsert_breakpoint (CORE_ADDR stop_at)
{
  set_breakpoint_at (stop_at, NULL);
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
      *bp_link = bp->next;
      delete_breakpoint (bp);
      bp = *bp_link;
    }
}

static void
uninsert_breakpoint (struct breakpoint *bp)
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
  struct breakpoint *bp;

  bp = find_breakpoint_at (pc);
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
    uninsert_breakpoint (bp);
}

static void
reinsert_breakpoint (struct breakpoint *bp)
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
  struct breakpoint *bp;

  bp = find_breakpoint_at (pc);
  if (bp == NULL)
    {
      /* This can happen when we remove all breakpoints while handling
	 a step-over.  */
      if (debug_threads)
	fprintf (stderr,
		 "Could not find breakpoint at 0x%s "
		 "in list (reinserting).\n",
		 paddress (pc));
      return;
    }

  reinsert_breakpoint (bp);
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
      if (bp->pc == stop_pc)
	{
	  if (!bp->inserted)
	    {
	      warning ("Hit a removed breakpoint?");
	      return;
	    }

	  if (bp->handler != NULL && (*bp->handler) (stop_pc))
	    {
	      *bp_link = bp->next;

	      delete_breakpoint (bp);

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
  struct process_info *proc = current_process ();
  struct breakpoint *bp;

  for (bp = proc->breakpoints; bp != NULL; bp = bp->next)
    if (bp->pc == addr)
      return 1;

  return 0;
}

int
breakpoint_inserted_here (CORE_ADDR addr)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp;

  for (bp = proc->breakpoints; bp != NULL; bp = bp->next)
    if (bp->pc == addr && bp->inserted)
      return 1;

  return 0;
}

void
check_mem_read (CORE_ADDR mem_addr, unsigned char *buf, int mem_len)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp = proc->breakpoints;
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

      memcpy (buf + buf_offset, bp->old_data + copy_offset, copy_len);
    }
}

void
check_mem_write (CORE_ADDR mem_addr, unsigned char *buf, int mem_len)
{
  struct process_info *proc = current_process ();
  struct breakpoint *bp = proc->breakpoints;
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
    delete_breakpoint (proc->breakpoints);
}

/* Release all breakpoints, but do not try to un-insert them from the
   inferior.  */

void
free_all_breakpoints (struct process_info *proc)
{
  struct breakpoint *bp;

  while (proc->breakpoints)
    {
      bp = proc->breakpoints;
      proc->breakpoints = bp->next;
      free (bp);
    }
}
