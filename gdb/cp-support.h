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

extern char *class_name_from_physname (const char *physname);

extern char *method_name_from_physname (const char *physname);

/* This is a struct to store data from "using directives" and similar
   language constructs.  It contains two strings, OLD and NEW; both
   should be fully-qualified namespace names, NEW should be an initial
   substring of OLD, and it says that names in the namespace OLD
   should be imported into namespace NEW.  For example, if it is used
   to represent the directive "using namespace std;" then OLD should
   be "std" and new should be "".  For a more complicated example, if
   there is an anonymous namespace with a named namespace A, then OLD
   should be "A::(anonymous namespace)" and new should be "A".  */

/* FIXME: carlton/2002-10-07: That anonymous namespace example isn't
   that great, since it really depends not only on what the
   demangler's output is but also on the fact that the demangler's
   output doesn't depend on the name of the file in question.  Which,
   alas, it doesn't, but should, leaving us with no way to distinguish
   between anonymous namespaces in different files.  Sigh...  */

struct using_data
{
  const char *old;
  const char *new;
};

/* This is a struct for a linked list of using_data's.  */

struct using_data_node
{
  struct using_data *current;
  struct using_data_node *next;
};
