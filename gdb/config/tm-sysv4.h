/* Macro definitions for GDB on all SVR4 target systems.
   Copyright 1991, 1992, 1993 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support (fnf@cygnus.com).

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
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* For SVR4 shared libraries, each call to a library routine goes through
   a small piece of trampoline code in the ".init" section.  Although each
   of these fragments is labeled with the name of the routine being called,
   the gdb symbol reading code deliberately ignores them so it won't confuse
   them with the real functions.  It does however know about the label that
   precedes all of the fragments, which is "_init".  Thus when we lookup a
   function that corresponds to a PC value which is in one of the trampoline
   fragments, we'll appear to be in the function "_init".  The following
   macro will evaluate to nonzero when NAME is valid and matches "_init".
   The horribly ugly wait_for_inferior() routine uses this macro to detect
   when we have stepped into one of these fragments. */

#define IN_SOLIB_TRAMPOLINE(pc,name) ((name) && (STREQ ("_init", name)))

/* It is unknown which, if any, SVR4 assemblers do not accept dollar signs
   in identifiers.  The default in G++ is to use dots instead, for all SVR4
   systems, so we make that our default also.  FIXME: There should be some
   way to get G++ to tell us what CPLUS_MARKER it is using, perhaps by
   stashing it in the debugging information as part of the name of an
   invented symbol ("gcc_cplus_marker$" for example). */

#undef CPLUS_MARKER
#define CPLUS_MARKER '.'
