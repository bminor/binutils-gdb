/* Helper routines for C++ support in GDB.
   Copyright 2002, 2003 Free Software Foundation, Inc.

   Contributed by MontaVista Software and Stanford University.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef CP_SUPPORT_H
#define CP_SUPPORT_H

/* Opaque declarations.  */

struct obstack;
struct symbol;

/* This struct is designed to store data from using directives.  It
   says that names from namespace INNER should be visible within
   namespace OUTER.  OUTER should always be a strict initial substring
   of INNER.  These form a linked list; NEXT is the next element of
   the list.  */

struct using_direct
{
  char *inner;
  char *outer;
  struct using_direct *next;
};

extern char *class_name_from_physname (const char *physname);

extern char *method_name_from_physname (const char *physname);

extern unsigned int cp_find_first_component (const char *name);

extern unsigned int cp_entire_prefix_len (const char *name);

extern struct using_direct *cp_add_using (const char *name,
					  unsigned int inner_len,
					  unsigned int outer_len,
					  struct using_direct *next);

extern struct using_direct *cp_copy_usings (struct using_direct *using,
					    struct obstack *obstack);

extern struct symbol *cp_check_namespace_symbol (const char *name, int len);

extern struct symbol *cp_lookup_namespace_symbol (const char *name);

extern void cp_check_possible_namespace_symbols (const char *name);

extern struct symbol *cp_lookup_possible_namespace_symbol (const char *name);

extern int cp_is_anonymous (const char *namespace);

extern char *cp_func_name (const char *full_name);

#endif /* CP_SUPPORT_H */
