/* Helper routines for C++ support in GDB.
   Copyright 2002 Free Software Foundation, Inc.

   Contributed by MontaVista Software.

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

/* Opaque declarations.  */

struct obstack;

extern char *class_name_from_physname (const char *physname);

extern char *method_name_from_physname (const char *physname);

extern const char *cp_locate_arguments (const char *name);

extern const char *cp_find_last_component (const char *first,
					   const char *last);

extern const char *cp_find_first_component (const char *name);

/* This is a struct to store data from "using directives" and similar
   language constructs.  It contains two strings, OUTER and INNER;
   both should be fully-qualified namespace names, OUTER should be an
   initial substring of INNER, and it says that names in the namespace
   INNER should be imported into namespace OUTER.  For example, if it
   is used to represent the directive "using namespace std;" then
   INNER should be "std" and new should be "".  For a more complicated
   example, if there is an anonymous namespace with a named namespace
   A, then INNER should be "A::(anonymous namespace)" and new should
   be "A".  */

/* FIXME: carlton/2002-10-07: That anonymous namespace example isn't
   that great, since it really depends not only on what the
   demangler's output is but also on the fact that the demangler's
   output doesn't depend on the name of the file in question.  Which,
   alas, it doesn't, but should, leaving us with no way to distinguish
   between anonymous namespaces in different files.  Sigh...  */

struct using_data
{
  const char *outer;
  const char *inner;
};

/* This is a struct for a linked list of using_data's.  */

struct using_data_node
{
  struct using_data *current;
  struct using_data_node *next;
};

extern struct using_data_node *cp_add_using (const char *outer,
					     const char *inner,
					     struct using_data_node *next,
					     struct obstack *obstack);
