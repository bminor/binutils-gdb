/* Cleanups.
   Copyright (C) 1986, 1988-2005, 2007-2012 Free Software Foundation, Inc.

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

#ifndef CLEANUPS_H
#define CLEANUPS_H

/* The cleanup list records things that have to be undone
   if an error happens (descriptors to be closed, memory to be freed, etc.)
   Each link in the chain records a function to call and an
   argument to give it.

   Use make_cleanup to add an element to the cleanup chain.
   Use do_cleanups to do all cleanup actions back to a given
   point in the chain.  Use discard_cleanups to remove cleanups
   from the chain back to a given point, not doing them.

   If the argument is pointer to allocated memory, then you need
   to additionally set the 'free_arg' member to a function that will
   free that memory.  This function will be called both when the cleanup
   is executed and when it's discarded.  */

struct cleanup
  {
    struct cleanup *next;
    void (*function) (void *);
    void (*free_arg) (void *);
    void *arg;
  };

/* NOTE: cagney/2000-03-04: This typedef is strictly for the
   make_cleanup function declarations below.  Do not use this typedef
   as a cast when passing functions into the make_cleanup() code.
   Instead either use a bounce function or add a wrapper function.
   Calling a f(char*) function with f(void*) is non-portable.  */
typedef void (make_cleanup_ftype) (void *);

/* WARNING: The result of the "make cleanup" routines is not the intuitive
   choice of being a handle on the just-created cleanup.  Instead it is an
   opaque handle of the cleanup mechanism and represents all cleanups created
   from that point onwards.  */

extern struct cleanup *make_cleanup (make_cleanup_ftype *, void *);

extern struct cleanup *make_cleanup_dtor (make_cleanup_ftype *, void *,
					  void (*dtor) (void *));

extern struct cleanup *make_final_cleanup (make_cleanup_ftype *, void *);

extern struct cleanup *make_my_cleanup (struct cleanup **,
					make_cleanup_ftype *, void *);
extern struct cleanup *make_my_cleanup2 (struct cleanup **,
					 make_cleanup_ftype *, void *,
					 void (*free_arg) (void *));

/* A special value to pass to do_cleanups and do_final_cleanups
   to tell them to do all cleanups.  */
#define	ALL_CLEANUPS	((struct cleanup *)0)

extern void do_cleanups (struct cleanup *);
extern void do_final_cleanups (struct cleanup *);

extern void discard_cleanups (struct cleanup *);
extern void discard_final_cleanups (struct cleanup *);
extern void discard_my_cleanups (struct cleanup **, struct cleanup *);

extern struct cleanup *save_cleanups (void);
extern struct cleanup *save_final_cleanups (void);
extern struct cleanup *save_my_cleanups (struct cleanup **);

extern void restore_cleanups (struct cleanup *);
extern void restore_final_cleanups (struct cleanup *);
extern void restore_my_cleanups (struct cleanup **, struct cleanup *);

/* A no-op cleanup.
   This is useful when you want to establish a known reference point
   to pass to do_cleanups.  */
extern void null_cleanup (void *);

#endif /* CLEANUPS_H */
