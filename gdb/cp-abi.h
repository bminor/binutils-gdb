/* Abstraction of various C++ ABI's we support, and the info we need
   to get from them.
   Contributed by Daniel Berlin <dberlin@redhat.com>
   Copyright 2001 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or
   modify
   it under the terms of the GNU General Public License as published
   by
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

#ifndef CP_ABI_H_
#define CP_ABI_H_ 1

/* Kinds of constructors.  All these values are guaranteed to be
   non-zero.  */
enum ctor_kinds {

  /* Initialize a complete object, including virtual bases, using
     memory provided by caller.  */
  complete_object_ctor = 1,

  /* Initialize a base object of some larger object.  */
  base_object_ctor,

  /* An allocating complete-object constructor.  */
  complete_object_allocating_ctor
};


/* Kinds of destructors.  All these values are guaranteed to be
   non-zero.  */
enum dtor_kinds {

  /* A destructor which finalizes the entire object, and then calls
     `delete' on its storage.  */
  deleting_dtor = 1,

  /* A destructor which finalizes the entire object, but does not call
     `delete'.  */
  complete_object_dtor,

  /* A destructor which finalizes a subobject of some larger object.  */
  base_object_dtor
};
  

struct cp_abi_ops
{
  const char *shortname;
  const char *longname;
  const char *doc;

  /* The functions here that attempt to determine what sort of thing a
     mangled name refers to may well be revised in the future.  It
     would certainly be cleaner to carry this information explicitly
     in GDB's data structures than to derive it from the mangled name.  */

  /* Return non-zero iff NAME is the mangled name of a constructor.
     Actually, return an `enum ctor_kind' value describing what *kind*
     of constructor it is.  */
  enum ctor_kinds (*is_constructor_name) (const char *name);

  /* Return non-zero iff NAME is the mangled name of a destructor.
     Actually, return an `enum dtor_kind' value describing what *kind*
     of destructor it is.  */
  enum dtor_kinds (*is_destructor_name) (const char *name);

  /* Return non-zero iff NAME is the mangled name of a vtable.  */
  int (*is_vtable_name) (const char *name);

  /* Return non-zero iff NAME is the un-mangled name of an operator,
     perhaps scoped within some class.  */
  int (*is_operator_name) (const char *name);

  value_ptr (*virtual_fn_field) (value_ptr * arg1p, struct fn_field * f,
				 int j, struct type * type, int offset);

  /* Find the real run-time type of a value using RTTI.
   * V is a pointer to the value.
   * A pointer to the struct type entry of the run-time type
   * is returneed.
   * FULL is a flag that is set only if the value V includes
   * the entire contents of an object of the RTTI type.
   * TOP is the offset to the top of the enclosing object of
   * the real run-time type.  This offset may be for the embedded
   * object, or for the enclosing object of V.
   * USING_ENC is the flag that distinguishes the two cases.
   * If it is 1, then the offset is for the enclosing object,
   * otherwise for the embedded object.
   *
   */

  struct type *(*rtti_type) (value_ptr v, int *full, int *top,
			     int *using_enc);
};


extern struct cp_abi_ops *cp_abis;

extern int num_cp_abis;

extern struct cp_abi_ops current_cp_abi;

extern enum ctor_kinds is_constructor_name (const char *name);
extern enum dtor_kinds is_destructor_name (const char *name);
extern int is_vtable_name (const char *name);
extern int is_operator_name (const char *name);
extern value_ptr value_virtual_fn_field (value_ptr * arg1p,
					 struct fn_field *f, int j,
					 struct type *type, int offset);
extern struct type *value_rtti_type (value_ptr v, int *full, int *top,
				     int *using_enc);
extern int register_cp_abi (struct cp_abi_ops abi);
extern int switch_to_cp_abi (const char *short_name);

#endif

