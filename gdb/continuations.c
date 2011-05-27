/* Continuations for GDB, the GNU debugger.

   Copyright (C) 1986, 1988, 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996,
   1997, 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008,
   2009, 2010, 2011 Free Software Foundation, Inc.

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
#include "gdbthread.h"
#include "inferior.h"

struct continuation
{
  struct continuation *next;
  void (*function) (void *);
  void (*free_arg) (void *);
  void *arg;
};

typedef void (make_continuation_ftype) (void *);

/* Add a new continuation to the continuation chain, and return the
   previous chain pointer to be passed later to do_continuations or
   discard_continuations.  Args are FUNCTION to run the continuation
   up with, and ARG to pass to it.  */

static struct continuation *
make_continuation (struct continuation **pmy_chain,
		   make_continuation_ftype *function,
		   void *arg,  void (*free_arg) (void *))
{
  struct continuation *new = XNEW (struct continuation);
  struct continuation *old_chain = *pmy_chain;

  new->next = *pmy_chain;
  new->function = function;
  new->free_arg = free_arg;
  new->arg = arg;
  *pmy_chain = new;

  return old_chain;
}

static void
do_my_continuations (struct continuation **pmy_chain,
		     struct continuation *old_chain)
{
  struct continuation *ptr;

  while ((ptr = *pmy_chain) != old_chain)
    {
      *pmy_chain = ptr->next;	/* Do this first in case of recursion.  */
      (*ptr->function) (ptr->arg);
      if (ptr->free_arg)
	(*ptr->free_arg) (ptr->arg);
      xfree (ptr);
    }
}

void
discard_my_continuations (struct continuation **pmy_chain,
			  struct continuation *old_chain)
{
  struct continuation *ptr;

  while ((ptr = *pmy_chain) != old_chain)
    {
      *pmy_chain = ptr->next;
      if (ptr->free_arg)
	(*ptr->free_arg) (ptr->arg);
      xfree (ptr);
    }
}

/* Add a continuation to the continuation list of THREAD.  The new
   continuation will be added at the front.  */

void
add_continuation (struct thread_info *thread,
		  void (*continuation_hook) (void *), void *args,
		  void (*continuation_free_args) (void *))
{
  struct continuation *continuations = thread->continuations;
  make_cleanup_ftype *continuation_hook_fn = continuation_hook;

  make_continuation (&continuations,
		     continuation_hook_fn,
		     args,
		     continuation_free_args);

  thread->continuations = continuations;
}

/* Add a continuation to the continuation list of INFERIOR.  The new
   continuation will be added at the front.  */

void
add_inferior_continuation (void (*continuation_hook) (void *), void *args,
			   void (*continuation_free_args) (void *))
{
  struct inferior *inf = current_inferior ();
  struct continuation *continuations = inf->continuations;
  make_cleanup_ftype *continuation_hook_fn = continuation_hook;

  make_continuation (&continuations,
		     continuation_hook_fn,
		     args,
		     continuation_free_args);

  inf->continuations = continuations;
}

/* Do all continuations of the current inferior.  */

void
do_all_inferior_continuations (void)
{
  struct continuation *continuations;
  struct inferior *inf = current_inferior ();

  if (inf->continuations == NULL)
    return;

  /* Copy the list header into another pointer, and set the global
     list header to null, so that the global list can change as a side
     effect of invoking the continuations and the processing of the
     preexisting continuations will not be affected.  */

  continuations = inf->continuations;
  inf->continuations = NULL;

  /* Work now on the list we have set aside.  */
  do_my_continuations (&continuations, NULL);
}

/* Get rid of all the inferior-wide continuations of INF.  */

void
discard_all_inferior_continuations (struct inferior *inf)
{
  struct continuation *continuation_ptr = inf->continuations;

  discard_my_continuations (&continuation_ptr, NULL);
  inf->continuations = NULL;
}

static void
restore_thread_cleanup (void *arg)
{
  ptid_t *ptid_p = arg;

  switch_to_thread (*ptid_p);
}

/* Walk down the continuation list of PTID, and execute all the
   continuations.  There is a problem though.  In some cases new
   continuations may be added while we are in the middle of this loop.
   If this happens they will be added in the front, and done before we
   have a chance of exhausting those that were already there.  We need
   to then save the beginning of the list in a pointer and do the
   continuations from there on, instead of using the global beginning
   of list as our iteration pointer.  */

static void
do_all_continuations_ptid (ptid_t ptid,
			   struct continuation **continuations_p)
{
  struct cleanup *old_chain;
  struct continuation *continuations;
  ptid_t current_thread;

  if (*continuations_p == NULL)
    return;

  current_thread = inferior_ptid;

  /* Restore selected thread on exit.  Don't try to restore the frame
     as well, because:

     - When running continuations, the selected frame is always #0.

     - The continuations may trigger symbol file loads, which may
     change the frame layout (frame ids change), which would trigger
     a warning if we used make_cleanup_restore_current_thread.  */

  old_chain = make_cleanup (restore_thread_cleanup, &current_thread);

  /* Let the continuation see this thread as selected.  */
  switch_to_thread (ptid);

  /* Copy the list header into another pointer, and set the global
     list header to null, so that the global list can change as a side
     effect of invoking the continuations and the processing of the
     preexisting continuations will not be affected.  */

  continuations = *continuations_p;
  *continuations_p = NULL;

  /* Work now on the list we have set aside.  */
  do_my_continuations (&continuations, NULL);

  do_cleanups (old_chain);
}

/* Callback for iterate over threads.  */

static int
do_all_continuations_thread_callback (struct thread_info *thread, void *data)
{
  do_all_continuations_ptid (thread->ptid, &thread->continuations);
  return 0;
}

/* Do all continuations of thread THREAD.  */

void
do_all_continuations_thread (struct thread_info *thread)
{
  do_all_continuations_thread_callback (thread, NULL);
}

/* Do all continuations of all threads.  */

void
do_all_continuations (void)
{
  iterate_over_threads (do_all_continuations_thread_callback, NULL);
}

/* Callback for iterate over threads.  */

static int
discard_all_continuations_thread_callback (struct thread_info *thread,
					   void *data)
{
  struct continuation *continuation_ptr = thread->continuations;

  discard_my_continuations (&continuation_ptr, NULL);
  thread->continuations = NULL;
  return 0;
}

/* Get rid of all the continuations of THREAD.  */

void
discard_all_continuations_thread (struct thread_info *thread)
{
  discard_all_continuations_thread_callback (thread, NULL);
}

/* Get rid of all the continuations of all threads.  */

void
discard_all_continuations (void)
{
  iterate_over_threads (discard_all_continuations_thread_callback, NULL);
}


/* Add a continuation to the intermediate continuation list of THREAD.
   The new continuation will be added at the front.  */

void
add_intermediate_continuation (struct thread_info *thread,
			       void (*continuation_hook)
			       (void *), void *args,
			       void (*continuation_free_args) (void *))
{
  struct continuation *continuations = thread->intermediate_continuations;
  make_cleanup_ftype *continuation_hook_fn = continuation_hook;

  make_continuation (&continuations,
		     continuation_hook_fn,
		     args,
		     continuation_free_args);

  thread->intermediate_continuations = continuations;
}

/* Walk down the cmd_continuation list, and execute all the
   continuations.  There is a problem though.  In some cases new
   continuations may be added while we are in the middle of this
   loop.  If this happens they will be added in the front, and done
   before we have a chance of exhausting those that were already
   there.  We need to then save the beginning of the list in a pointer
   and do the continuations from there on, instead of using the
   global beginning of list as our iteration pointer.  */

static int
do_all_intermediate_continuations_thread_callback (struct thread_info *thread,
						   void *data)
{
  do_all_continuations_ptid (thread->ptid,
			     &thread->intermediate_continuations);
  return 0;
}

/* Do all intermediate continuations of thread THREAD.  */

void
do_all_intermediate_continuations_thread (struct thread_info *thread)
{
  do_all_intermediate_continuations_thread_callback (thread, NULL);
}

/* Do all intermediate continuations of all threads.  */

void
do_all_intermediate_continuations (void)
{
  iterate_over_threads (do_all_intermediate_continuations_thread_callback,
			NULL);
}

/* Callback for iterate over threads.  */

static int
discard_all_intermediate_continuations_thread_callback (struct thread_info *thread,
							void *data)
{
  struct continuation *continuation_ptr = thread->intermediate_continuations;

  discard_my_continuations (&continuation_ptr, NULL);
  thread->intermediate_continuations = NULL;
  return 0;
}

/* Get rid of all the intermediate continuations of THREAD.  */

void
discard_all_intermediate_continuations_thread (struct thread_info *thread)
{
  discard_all_intermediate_continuations_thread_callback (thread, NULL);
}

/* Get rid of all the intermediate continuations of all threads.  */

void
discard_all_intermediate_continuations (void)
{
  iterate_over_threads (discard_all_intermediate_continuations_thread_callback,
			NULL);
}
