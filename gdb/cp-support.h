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
struct symbol;

extern char *class_name_from_physname (const char *physname);

extern char *method_name_from_physname (const char *physname);

extern const char *cp_find_first_component (const char *name);

/* This is a struct to store data from "using directives" and similar
   language constructs.  NAME is a pointer to a string; its initial
   substrings of length OUTER_LENGTH and INNER_LENGTH should both be
   fully-qualified namespace names.  (And OUTER_LENGTH should be
   strictly less than INNER_LENGTH).  The meaning is that names in the
   inner namespace should be imported into outer.

   For example, if it is used to represent the directive "using
   namespace std;" then NAME should start with "std", INNER_LENGTH
   should be 0, and OUTER_LENGTH should be "3".  For a more
   complicated example, if there is an anonymous namespace with a
   named namespace A, then NAME should start with "A::(anonymous
   namespace)", INNER_LENGTH should be 1, and OUTER_LENGTH should be
   strlen ("A::(anonymous namespace)").  */

/* FIXME: carlton/2002-10-07: That anonymous namespace example isn't
   that great, since it really depends not only on what the
   demangler's output is but also on the fact that the demangler's
   output doesn't depend on the name of the file in question.  Which,
   alas, it doesn't, but should, leaving us with no way to distinguish
   between anonymous namespaces in different files.  Sigh...  */

struct using_direct
{
  const char *name;
  unsigned short outer_length;
  unsigned short inner_length;
};

/* This is a struct for a linked list of using_direct's.  */

struct using_direct_node
{
  struct using_direct *current;
  struct using_direct_node *next;
};

/* This is used by struct block to store namespace-related info for
   C++ files, namely using declarations and the current namespace in
   scope.  */

struct namespace_info
{
  struct using_direct_node *using;
  const char *scope;
};

extern struct
using_direct_node *cp_add_using_obstack (const char *name,
					 unsigned short outer_length,
					 unsigned short inner_length,
					 struct using_direct_node *next,
					 struct obstack *obstack);

extern
struct using_direct_node *cp_add_using_xmalloc (const char *name,
						unsigned short outer_length,
						unsigned short inner_length,
						struct using_direct_node
						*next);

extern
struct using_direct_node *cp_copy_usings (struct using_direct_node *tocopy,
					  struct using_direct_node *tail);

extern void cp_free_usings (struct using_direct_node *using);

extern struct symbol *cp_check_namespace_symbol (const char *name, int len);
