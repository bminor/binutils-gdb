/* Type stack for GDB parser.

   Copyright (C) 1986-2025 Free Software Foundation, Inc.

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

#ifndef GDB_TYPE_STACK_H
#define GDB_TYPE_STACK_H

#include "gdbtypes.h"
#include <vector>

struct type;
struct expr_builder;

/* The kind of element on the stack of types and qualifiers, used for
   parsing complicated types.  */
enum type_pieces
{
  /* This is returned by pop() to indicate that the stack is empty.
     Note that it may not be pushed by the user.  */
  tp_end = -1,
  tp_pointer,
  tp_reference,
  tp_rvalue_reference,
  /* An array type, where the dimension is pushed on the stack as
     well.  */
  tp_array,
  tp_function,
  /* A function with argument types; the types are also pushed on the
     stack.  */
  tp_function_with_arguments,
  tp_const,
  tp_volatile,
  /* An address space identifier.  The address space is also pushed on
     the stack.  */
  tp_space_identifier,
  tp_atomic,
  tp_restrict,
  /* A separate type stack, which is also pushed onto this type
     stack.  */
  tp_type_stack,
  /* Fortran-specific, specifies the kind.  */
  tp_kind,
};

/* Items on the type stack.  */
union type_stack_elt
{
  /* An ordinary type qualifier.  */
  enum type_pieces piece;
  /* An integer value.  Currently only used for address spaces.  */
  int int_val;
  /* Another type stack.  */
  struct type_stack *stack_val;
  /* A list of types.  */
  std::vector<struct type *> *typelist_val;
};

/* The type stack is an instance of this structure.  */

struct type_stack
{
public:

  type_stack () = default;

  DISABLE_COPY_AND_ASSIGN (type_stack);

  type_stack *create ()
  {
    type_stack *result = new type_stack ();
    result->m_elements = std::move (m_elements);
    return result;
  }

  /* Insert a new type, TP, at the bottom of the type stack.  If TP is
     tp_pointer, tp_reference or tp_rvalue_reference, it is inserted at the
     bottom.  If TP is a qualifier, it is inserted at slot 1 (just above a
     previous tp_pointer) if there is anything on the stack, or simply pushed
     if the stack is empty.  Other values for TP are invalid.  */

  void insert (enum type_pieces tp);

  /* Push an ordinary element on the type stack.  This can be used for
     any element that doesn't require any extra data.  */
  void push (enum type_pieces tp)
  {
    /* tp_end can only be returned by this class, not pushed.  The
       others require a payload on the stack and so can only be pushed
       by the appropriate method.  */
    gdb_assert (tp != tp_end);
    gdb_assert (!requires_payload (tp));
    type_stack_elt elt;
    elt.piece = tp;
    m_elements.push_back (elt);
  }

  /* Push an element and an integer value onto the stack.  The integer
     value is pushed first, followed by TP.  Only piece kinds that
     accept an integer argument are allowed.  */
  void push (enum type_pieces tp, int n)
  {
    gdb_assert (tp == tp_array || tp == tp_space_identifier || tp == tp_kind);
    type_stack_elt elt;
    elt.int_val = n;
    m_elements.push_back (elt);
    elt.piece = tp;
    m_elements.push_back (elt);
  }

  /* Push the type stack STACK as an element on this type stack.  */

  void push (struct type_stack *stack)
  {
    type_stack_elt elt;
    elt.stack_val = stack;
    m_elements.push_back (elt);
    elt.piece = tp_type_stack;
    m_elements.push_back (elt);
  }

  /* Push a function type with arguments onto this type stack.  LIST
     holds the argument types.  If the final item in LIST is NULL,
     then the function will be varargs.  */

  void push (std::vector<struct type *> *list)
  {
    type_stack_elt elt;
    elt.typelist_val = list;
    m_elements.push_back (elt);
    elt.piece = tp_function_with_arguments;
    m_elements.push_back (elt);
  }

  /* Pop the topmost element of the stack and return it.

     If the stack is empty, tp_end is returned.

     If the topmost element requires a payload -- for example,
     tp_array takes an integer parameter -- then the value is returned
     but *not* popped from the stack.  Instead the appropriate
     payload-specific pop_* method (e.g., pop_int) must then be
     called.  This somewhat odd approach ensures that the stack can't
     be put into an invalid state.  */
  enum type_pieces pop ()
  {
    if (m_elements.empty ())
      return tp_end;
    type_stack_elt elt = m_elements.back ();
    if (!requires_payload (elt.piece))
      m_elements.pop_back ();
    return elt.piece;
  }

  int pop_int ()
  {
    gdb_assert (m_elements.size () >= 2);
    type_stack_elt elt = m_elements.back ();
    m_elements.pop_back ();
    type_pieces tp = elt.piece;
    gdb_assert (tp == tp_array || tp == tp_space_identifier || tp == tp_kind);
    elt = m_elements.back ();
    m_elements.pop_back ();
    return elt.int_val;
  }

  std::vector<struct type *> *pop_typelist ()
  {
    gdb_assert (m_elements.size () >= 2);
    type_stack_elt elt = m_elements.back ();
    m_elements.pop_back ();
    type_pieces tp = elt.piece;
    gdb_assert (tp == tp_function_with_arguments);
    elt = m_elements.back ();
    m_elements.pop_back ();
    return elt.typelist_val;
  }

  /* Pop a type_stack element.  */

  struct type_stack *pop_type_stack ()
  {
    gdb_assert (m_elements.size () >= 2);
    type_stack_elt elt = m_elements.back ();
    m_elements.pop_back ();
    type_pieces tp = elt.piece;
    gdb_assert (tp == tp_type_stack);
    elt = m_elements.back ();
    m_elements.pop_back ();
    return elt.stack_val;
  }

  /* Insert a tp_space_identifier and the corresponding address space
     value into the stack.  STRING is the name of an address space, as
     recognized by address_space_name_to_type_instance_flags.  If the
     stack is empty, the new elements are simply pushed.  If the stack
     is not empty, this function assumes that the first item on the
     stack is a tp_pointer, and the new values are inserted above the
     first item.  */

  void insert (struct gdbarch *gdbarch, const char *string);

  /* Append the elements of the type stack FROM to the type stack
     THIS.  Always returns THIS.  */

  struct type_stack *append (struct type_stack *from)
  {
    m_elements.insert (m_elements.end (), from->m_elements.begin (),
		       from->m_elements.end ());
    return this;
  }

  /* Pop the type stack and return a type_instance_flags that
     corresponds the const/volatile qualifiers on the stack.  This is
     called by the C++ parser when parsing methods types, and as such no
     other kind of type in the type stack is expected.  */

  type_instance_flags follow_type_instance_flags ();

  /* Pop the type stack and return the type which corresponds to
     FOLLOW_TYPE as modified by all the stuff on the stack.  */
  struct type *follow_types (struct type *follow_type);

private:

  /* A helper function for the insert methods.  This does work of
     expanding the type stack and inserting the new element, ELEMENT,
     into the stack at location SLOT.  */

  void insert_into (int slot, union type_stack_elt element)
  {
    gdb_assert (slot <= m_elements.size ());
    m_elements.insert (m_elements.begin () + slot, element);
  }

  /* Return true if TP requires some payload element.  */
  bool requires_payload (type_pieces tp) const
  {
    return (tp == tp_array || tp == tp_kind || tp == tp_type_stack
	    || tp == tp_function_with_arguments
	    || tp == tp_space_identifier);
  }


  /* Elements on the stack.  */
  std::vector<union type_stack_elt> m_elements;
};

#endif /* GDB_TYPE_STACK_H */
