/* Cleanup routines for GDB, the GNU debugger.

   Copyright (C) 1986, 1988-2012 Free Software Foundation, Inc.

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

/* Chain of cleanup actions established with make_cleanup,
   to be executed if an error happens.  */

/* Cleaned up after a failed command.  */
static struct cleanup *cleanup_chain;

/* Cleaned up when gdb exits.  */
static struct cleanup *final_cleanup_chain;

/* Main worker routine to create a cleanup.
   PMY_CHAIN is a pointer to either cleanup_chain or final_cleanup_chain.
   FUNCTION is the function to call to perform the cleanup.
   ARG is passed to FUNCTION when called.
   FREE_ARG, if non-NULL, is called after the cleanup is performed.

   The result is a pointer to the previous chain pointer
   to be passed later to do_cleanups or discard_cleanups.  */

struct cleanup *
make_my_cleanup2 (struct cleanup **pmy_chain, make_cleanup_ftype *function,
		  void *arg,  void (*free_arg) (void *))
{
  struct cleanup *new
    = (struct cleanup *) xmalloc (sizeof (struct cleanup));
  struct cleanup *old_chain = *pmy_chain;

  new->next = *pmy_chain;
  new->function = function;
  new->free_arg = free_arg;
  new->arg = arg;
  *pmy_chain = new;

  return old_chain;
}

/* Worker routine to create a cleanup without a destructor.
   PMY_CHAIN is a pointer to either cleanup_chain or final_cleanup_chain.
   FUNCTION is the function to call to perform the cleanup.
   ARG is passed to FUNCTION when called.

   The result is a pointer to the previous chain pointer
   to be passed later to do_cleanups or discard_cleanups.  */

struct cleanup *
make_my_cleanup (struct cleanup **pmy_chain, make_cleanup_ftype *function,
		 void *arg)
{
  return make_my_cleanup2 (pmy_chain, function, arg, NULL);
}

/* Add a new cleanup to the cleanup_chain,
   and return the previous chain pointer
   to be passed later to do_cleanups or discard_cleanups.
   Args are FUNCTION to clean up with, and ARG to pass to it.  */

struct cleanup *
make_cleanup (make_cleanup_ftype *function, void *arg)
{
  return make_my_cleanup (&cleanup_chain, function, arg);
}

/* Same as make_cleanup except also includes TDOR, a destructor to free ARG.
   DTOR is invoked when the cleanup is performed or when it is discarded.  */

struct cleanup *
make_cleanup_dtor (make_cleanup_ftype *function, void *arg,
		   void (*dtor) (void *))
{
  return make_my_cleanup2 (&cleanup_chain,
			   function, arg, dtor);
}

/* Same as make_cleanup except the cleanup is added to final_cleanup_chain.  */

struct cleanup *
make_final_cleanup (make_cleanup_ftype *function, void *arg)
{
  return make_my_cleanup (&final_cleanup_chain, function, arg);
}

/* Worker routine to perform cleanups.
   PMY_CHAIN is a pointer to either cleanup_chain or final_cleanup_chain.
   OLD_CHAIN is the result of a "make" cleanup routine.
   Cleanups are performed until we get back to the old end of the chain.  */

static void
do_my_cleanups (struct cleanup **pmy_chain,
		struct cleanup *old_chain)
{
  struct cleanup *ptr;

  while ((ptr = *pmy_chain) != old_chain)
    {
      *pmy_chain = ptr->next;	/* Do this first in case of recursion.  */
      (*ptr->function) (ptr->arg);
      if (ptr->free_arg)
	(*ptr->free_arg) (ptr->arg);
      xfree (ptr);
    }
}

/* Discard cleanups and do the actions they describe
   until we get back to the point OLD_CHAIN in the cleanup_chain.  */

void
do_cleanups (struct cleanup *old_chain)
{
  do_my_cleanups (&cleanup_chain, old_chain);
}

/* Discard cleanups and do the actions they describe
   until we get back to the point OLD_CHAIN in the final_cleanup_chain.  */

void
do_final_cleanups (struct cleanup *old_chain)
{
  do_my_cleanups (&final_cleanup_chain, old_chain);
}

/* Main worker routine to discard cleanups.
   PMY_CHAIN is a pointer to either cleanup_chain or final_cleanup_chain.
   OLD_CHAIN is the result of a "make" cleanup routine.
   Cleanups are discarded until we get back to the old end of the chain.  */

void
discard_my_cleanups (struct cleanup **pmy_chain,
		     struct cleanup *old_chain)
{
  struct cleanup *ptr;

  while ((ptr = *pmy_chain) != old_chain)
    {
      *pmy_chain = ptr->next;
      if (ptr->free_arg)
	(*ptr->free_arg) (ptr->arg);
      xfree (ptr);
    }
}

/* Discard cleanups, not doing the actions they describe,
   until we get back to the point OLD_CHAIN in the cleanup chain.  */

void
discard_cleanups (struct cleanup *old_chain)
{
  discard_my_cleanups (&cleanup_chain, old_chain);
}

/* Discard final cleanups, not doing the actions they describe,
   until we get back to the point OLD_CHAIN in the final cleanup chain.  */

void
discard_final_cleanups (struct cleanup *old_chain)
{
  discard_my_cleanups (&final_cleanup_chain, old_chain);
}

/* Main worker routine to save cleanups.
   PMY_CHAIN is a pointer to either cleanup_chain or final_cleanup_chain.
   The chain is emptied and the result is a pointer to the old chain.  */

struct cleanup *
save_my_cleanups (struct cleanup **pmy_chain)
{
  struct cleanup *old_chain = *pmy_chain;

  *pmy_chain = 0;
  return old_chain;
}

/* Set the cleanup_chain to 0, and return the old cleanup_chain.  */

struct cleanup *
save_cleanups (void)
{
  return save_my_cleanups (&cleanup_chain);
}

/* Set the final_cleanup_chain to 0, and return the old
   final_cleanup_chain.  */

struct cleanup *
save_final_cleanups (void)
{
  return save_my_cleanups (&final_cleanup_chain);
}

/* Main worker routine to save cleanups.
   PMY_CHAIN is a pointer to either cleanup_chain or final_cleanup_chain.
   The chain is restored from CHAIN.  */

void
restore_my_cleanups (struct cleanup **pmy_chain, struct cleanup *chain)
{
  *pmy_chain = chain;
}

/* Restore the cleanup chain from a previously saved chain.  */

void
restore_cleanups (struct cleanup *chain)
{
  restore_my_cleanups (&cleanup_chain, chain);
}

/* Restore the final cleanup chain from a previously saved chain.  */

void
restore_final_cleanups (struct cleanup *chain)
{
  restore_my_cleanups (&final_cleanup_chain, chain);
}

/* Provide a known function that does nothing, to use as a base for
   a possibly long chain of cleanups.  This is useful where we
   use the cleanup chain for handling normal cleanups as well as dealing
   with cleanups that need to be done as a result of a call to error().
   In such cases, we may not be certain where the first cleanup is, unless
   we have a do-nothing one to always use as the base.  */

void
null_cleanup (void *arg)
{
}
