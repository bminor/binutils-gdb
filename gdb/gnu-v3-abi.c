/* Abstraction of GNU v3 abi.
   Contributed by Jim Blandy <jimb@redhat.com>
   Copyright 2001 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "value.h"
#include "cp-abi.h"
#include "demangle.h"

struct cp_abi_ops gnu_v3_abi_ops;

static int
gnuv3_is_vtable_name (const char *name)
{
  return strncmp (name, "_ZTV", 4) == 0;
}

static int
gnuv3_is_operator_name (const char *name)
{
  return strncmp (name, "operator", 8) == 0;
}


/* To help us find the components of a vtable, we build ourselves a
   GDB type object representing the vtable structure.  Following the
   V3 ABI, it goes something like this:

   struct gdb_gnu_v3_abi_vtable {

     / * An array of virtual call and virtual base offsets.  The real
         length of this array depends on the class hierarchy; we use
         negative subscripts to access the elements.  Yucky, but
         better than the alternatives.  * /
     ptrdiff_t vcall_and_vbase_offsets[0];

     / * The offset from a virtual pointer referring to this table
         to the top of the complete object.  * /
     ptrdiff_t offset_to_top;

     / * The type_info pointer for this class.  This is really a
         std::type_info *, but GDB doesn't really look at the
         type_info object itself, so we don't bother to get the type
         exactly right.  * /
     void *type_info;

     / * Virtual table pointers in objects point here.  * /

     / * Virtual function pointers.  Like the vcall/vbase array, the
         real length of this table depends on the class hierarchy.  * /
     void (*virtual_functions[0]) ();

   };

   The catch, of course, is that the exact layout of this table
   depends on the ABI --- word size, endianness, alignment, etc.  So
   the GDB type object is actually a per-architecture kind of thing.

   vtable_type_gdbarch_data is a gdbarch per-architecture data pointer
   which refers to the struct type * for this structure, laid out
   appropriately for the architecture.  */
struct gdbarch_data *vtable_type_gdbarch_data;


/* Human-readable names for the numbers of the fields above.  */
enum {
  vtable_field_vcall_and_vbase_offsets,
  vtable_field_offset_to_top,
  vtable_field_type_info,
  vtable_field_virtual_functions
};


/* Return a GDB type representing `struct gdb_gnu_v3_abi_vtable',
   described above, laid out appropriately for ARCH.

   We use this function as the gdbarch per-architecture data
   initialization function.  We assume that the gdbarch framework
   calls the per-architecture data initialization functions after it
   sets current_gdbarch to the new architecture.  */
static void *
build_gdb_vtable_type (struct gdbarch *arch)
{
  struct type *t;
  struct field *field_list, *field;
  int offset;

  struct type *void_ptr_type
    = lookup_pointer_type (builtin_type_void);
  struct type *ptr_to_void_fn_type
    = lookup_pointer_type (lookup_function_type (builtin_type_void));

  /* ARCH can't give us the true ptrdiff_t type, so we guess.  */
  struct type *ptrdiff_type
    = init_type (TYPE_CODE_INT, TARGET_PTR_BIT / TARGET_CHAR_BIT, 0,
                 "ptrdiff_t", 0);

  /* We assume no padding is necessary, since GDB doesn't know
     anything about alignment at the moment.  If this assumption bites
     us, we should add a gdbarch method which, given a type, returns
     the alignment that type requires, and then use that here.  */

  /* Build the field list.  */
  field_list = xmalloc (sizeof (struct field [4]));
  memset (field_list, 0, sizeof (struct field [4]));
  field = &field_list[0];
  offset = 0;

  /* ptrdiff_t vcall_and_vbase_offsets[0]; */
  FIELD_NAME (*field) = "vcall_and_vbase_offsets";
  FIELD_TYPE (*field)
    = create_array_type (0, ptrdiff_type,
                         create_range_type (0, builtin_type_int, 0, -1));
  FIELD_BITPOS (*field) = offset * TARGET_CHAR_BIT;
  offset += TYPE_LENGTH (FIELD_TYPE (*field));
  field++;

  /* ptrdiff_t offset_to_top; */
  FIELD_NAME (*field) = "offset_to_top";
  FIELD_TYPE (*field) = ptrdiff_type;
  FIELD_BITPOS (*field) = offset * TARGET_CHAR_BIT;
  offset += TYPE_LENGTH (FIELD_TYPE (*field));
  field++;

  /* void *type_info; */
  FIELD_NAME (*field) = "type_info";
  FIELD_TYPE (*field) = void_ptr_type;
  FIELD_BITPOS (*field) = offset * TARGET_CHAR_BIT;
  offset += TYPE_LENGTH (FIELD_TYPE (*field));
  field++;

  /* void (*virtual_functions[0]) (); */
  FIELD_NAME (*field) = "virtual_functions";
  FIELD_TYPE (*field)
    = create_array_type (0, ptr_to_void_fn_type,
                         create_range_type (0, builtin_type_int, 0, -1));
  FIELD_BITPOS (*field) = offset * TARGET_CHAR_BIT;
  offset += TYPE_LENGTH (FIELD_TYPE (*field));
  field++;

  /* We assumed in the allocation above that there were four fields.  */
  if (field != field_list + 4)
    abort ();

  t = init_type (TYPE_CODE_STRUCT, offset, 0, 0, 0);
  TYPE_NFIELDS (t) = field - field_list;
  TYPE_FIELDS (t) = field_list;
  TYPE_TAG_NAME (t) = "gdb_gnu_v3_abi_vtable";

  return t;
}


/* Return the offset from the start of the imaginary `struct
   gdb_gnu_v3_abi_vtable' object to the vtable's "address point"
   (i.e., where objects' virtual table pointers point).  */
static int
vtable_address_point_offset ()
{
  struct type *vtable_type = gdbarch_data (vtable_type_gdbarch_data);

  return (TYPE_FIELD_BITPOS (vtable_type, vtable_field_virtual_functions)
          / TARGET_CHAR_BIT);
}


static struct type *
gnuv3_rtti_type (struct value *value,
                 int *full_p, int *top_p, int *using_enc_p)
{
  struct type *vtable_type = gdbarch_data (vtable_type_gdbarch_data);
  struct type *value_type = check_typedef (VALUE_TYPE (value));
  CORE_ADDR vtable_address;
  struct value *vtable;
  struct minimal_symbol *vtable_symbol;
  const char *vtable_symbol_name;
  const char *class_name;
  struct symbol *class_symbol;
  struct type *run_time_type;
  LONGEST offset_to_top;

  /* We only have RTTI for class objects.  */
  if (TYPE_CODE (value_type) != TYPE_CODE_CLASS)
    return NULL;

  /* If we can't find the virtual table pointer for value_type, we
     can't find the RTTI.  */
  fill_in_vptr_fieldno (value_type);
  if (TYPE_VPTR_FIELDNO (value_type) == -1)
    return NULL;

  /* Fetch VALUE's virtual table pointer, and tweak it to point at
     an instance of our imaginary gdb_gnu_v3_abi_vtable structure.   */
  vtable_address
    = value_as_pointer (value_field (value, TYPE_VPTR_FIELDNO (value_type)));
  vtable = value_at_lazy (vtable_type,
                          vtable_address - vtable_address_point_offset (),
                          VALUE_BFD_SECTION (value));
  
  /* Find the linker symbol for this vtable.  */
  vtable_symbol
    = lookup_minimal_symbol_by_pc (VALUE_ADDRESS (vtable)
                                   + VALUE_OFFSET (vtable)
                                   + VALUE_EMBEDDED_OFFSET (vtable));
  if (! vtable_symbol)
    return NULL;
  
  /* The symbol's demangled name should be something like "vtable for
     CLASS", where CLASS is the name of the run-time type of VALUE.
     If we didn't like this approach, we could instead look in the
     type_info object itself to get the class name.  But this way
     should work just as well, and doesn't read target memory.  */
  vtable_symbol_name = SYMBOL_DEMANGLED_NAME (vtable_symbol);
  if (strncmp (vtable_symbol_name, "vtable for ", 11))
    error ("can't find linker symbol for virtual table for `%s' value",
           TYPE_NAME (value_type));
  class_name = vtable_symbol_name + 11;

  /* Try to look up the class name as a type name.  */
  class_symbol = lookup_symbol (class_name, 0, STRUCT_NAMESPACE, 0, 0);
  if (! class_symbol)
    error ("can't find class named `%s', as given by C++ RTTI", class_name);

  /* Make sure the type symbol is sane.  (An earlier version of this
     code would find constructor functions, who have the same name as
     the class.)  */
  if (SYMBOL_CLASS (class_symbol) != LOC_TYPEDEF
      || TYPE_CODE (SYMBOL_TYPE (class_symbol)) != TYPE_CODE_CLASS)
    error ("C++ RTTI gives a class name of `%s', but that isn't a type name",
           class_name);

  /* This is the object's run-time type!  */
  run_time_type = SYMBOL_TYPE (class_symbol);

  /* Get the offset from VALUE to the top of the complete object.
     NOTE: this is the reverse of the meaning of *TOP_P.  */
  offset_to_top
    = value_as_long (value_field (vtable, vtable_field_offset_to_top));

  if (full_p)
    *full_p = (- offset_to_top == VALUE_EMBEDDED_OFFSET (value)
               && (TYPE_LENGTH (VALUE_ENCLOSING_TYPE (value))
                   >= TYPE_LENGTH (run_time_type)));
  if (top_p)
    *top_p = - offset_to_top;
  if (using_enc_p)
    *using_enc_p = 0;

  return run_time_type;
}


static struct value *
gnuv3_virtual_fn_field (struct value **value_p,
                        struct fn_field *f, int j,
			struct type *type, int offset)
{
  struct type *vtable_type = gdbarch_data (vtable_type_gdbarch_data);
  struct value *value = *value_p;
  struct type *value_type = check_typedef (VALUE_TYPE (value));
  struct type *vfn_base;
  CORE_ADDR vtable_address;
  struct value *vtable;
  struct value *vfn;

  /* Some simple sanity checks.  */
  if (TYPE_CODE (value_type) != TYPE_CODE_CLASS)
    error ("Only classes can have virtual functions.");

  /* Find the base class that defines this virtual function.  */
  vfn_base = TYPE_FN_FIELD_FCONTEXT (f, j);
  if (! vfn_base)
    /* In programs compiled with G++ version 1, the debug info doesn't
       say which base class defined the virtual function.  We'll guess
       it's the same base class that has our vtable; this is wrong for
       multiple inheritance, but it's better than nothing.  */
    vfn_base = TYPE_VPTR_BASETYPE (type);

  /* This type may have been defined before its virtual function table
     was.  If so, fill in the virtual function table entry for the
     type now.  */
  if (TYPE_VPTR_FIELDNO (vfn_base) < 0)
    fill_in_vptr_fieldno (vfn_base);

  /* Now that we know which base class is defining our virtual
     function, cast our value to that baseclass.  This takes care of
     any necessary `this' adjustments.  */
  if (vfn_base != value_type)
    /* It would be nicer to simply cast the value to the appropriate
       base class (and I think that is supposed to be legal), but
       value_cast only does the right magic when casting pointers.  */
    value = value_ind (value_cast (vfn_base, value_addr (value)));

  /* Now value is an object of the appropriate base type.  Fetch its
     virtual table.  */
  vtable_address
    = value_as_pointer (value_field (value, TYPE_VPTR_FIELDNO (vfn_base)));
  vtable = value_at_lazy (vtable_type,
                          vtable_address - vtable_address_point_offset (),
                          VALUE_BFD_SECTION (value));

  /* Fetch the appropriate function pointer from the vtable.  */
  vfn = value_subscript (value_field (vtable, vtable_field_virtual_functions),
                         value_from_longest (builtin_type_int,
                                             TYPE_FN_FIELD_VOFFSET (f, j)));

  /* Cast the function pointer to the appropriate type.  */
  vfn = value_cast (lookup_pointer_type (TYPE_FN_FIELD_TYPE (f, j)),
                    vfn);

  return vfn;
}


static void
init_gnuv3_ops (void)
{
  vtable_type_gdbarch_data = register_gdbarch_data (build_gdb_vtable_type, 0);

  gnu_v3_abi_ops.shortname = "gnu-v3";
  gnu_v3_abi_ops.longname = "GNU G++ Version 3 ABI";
  gnu_v3_abi_ops.doc = "G++ Version 3 ABI";
  gnu_v3_abi_ops.is_destructor_name = is_gnu_v3_mangled_dtor;
  gnu_v3_abi_ops.is_constructor_name = is_gnu_v3_mangled_ctor;
  gnu_v3_abi_ops.is_vtable_name = gnuv3_is_vtable_name;
  gnu_v3_abi_ops.is_operator_name = gnuv3_is_operator_name;
  gnu_v3_abi_ops.rtti_type = gnuv3_rtti_type;
  gnu_v3_abi_ops.virtual_fn_field = gnuv3_virtual_fn_field;
}


void
_initialize_gnu_v3_abi (void)
{
  init_gnuv3_ops ();

  register_cp_abi (gnu_v3_abi_ops);
}
