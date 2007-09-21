/* DWARF 2 Arange-Set.
   Copyright 2007 Free Software Foundation, Inc.
   Contributed by Doug Kwan, Google Inc.
 
   This file is part of BFD.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* Scalable DWARF2 Arange Set.
 
   The original code in dwarf2.c uses an unsorted singly-linked list to
   represent aranges in a compilation unit.  Looking up for an address
   became very in-efficient for extremely large binaries with many
   compilation units, each of which having long list of aranges.

   The arange-set implemented here supports insertion and address
   containment queries for an arbitrary large collection of aranges in
   an efficient manner.  In addition, it also supports aranges with
   values.

     Arange insertion with value.

   For valued arange-set, we need to specify 4 operations during set
   creation.  If unspecified, reasonable default behaviours are assumed.
   The operations define how arange insertion merges two identical aranges
   with different values. The 4 operations are:

	Equality
	Copy
	Combination
	Deletion

   When arange_set_insert () inserts an arange. It breaks the to-be-inserted
   arange into smaller aranges using the boundaries of any overlapping
   aranges as cutting point.  In addition, arange_set_insert () may also
   splilt any existing arange that overlap the ends of the to-be-inserted
   arange.  After such splitting of the new and existing aranges, the
   to-be-inserted arange becomes a collection of smaller aranges, each of
   which either does not overlapping with any existing arange or overlapping
   completely with one existing arange.  While splitting aranges, values
   are copied using the Copy operation specified in the set.

   The for each smaller new arange, arange_set_insert () inserts the new
   arange according to these rules:

   1. If there is no overlapping existing arange, insert new arange.

   2. If there is an overlapping existing arange and its value equals
      to the inserted value according to the value equality operation
      of the set, do nothing.

   3. If there is an overlapping existing arange and its value is not
      the inserted value according to the value equality operation,
      combine the inserted value with that of the existing arange using
      the value combination operation of set.
 
   If as a result of insertion, there are adjacent aranges with equal values,
   the adjacent aranges will be merge.  */

#ifndef ARANGE_SET_H
#define ARANGE_SET_H

#include "sysdep.h"
#include "bfd.h"

/* An arange_set is a pointer to an arange_set_s struct, whose implementation
   is opaque to clients using the arange set.  */
typedef struct arange_set_s *arange_set;

#ifndef _WIN64
  typedef unsigned long int arange_set_uhostptr_t;
#else
  typedef unsigned long long arange_set_uhostptr_t;
#endif

/* Type of value attached to an arange.  This should be wide enough to be
   converted from and back to any type without loss.  */
typedef arange_set_uhostptr_t arange_value_type;

/* Type of function that is used to allocate memory for an arange-set.  */
typedef void* (*arange_set_allocate_fn)(int, void*);

/* Type of function that is used to deallocate memory of an arange-set.  */
typedef void (*arange_set_deallocate_fn)(void*, void*);

/* Type of function that is called for each arange during a traversal of
   the set containing that arange.  */
typedef int (*arange_set_foreach_fn)(bfd_vma, bfd_vma, arange_value_type,
				     void *);

/* Type of function that is called to test equality of range values. */
typedef bfd_boolean (*arange_value_equal_fn)(arange_value_type,
					     arange_value_type, void *);

/* Type of function that is called to copy a range value. */
typedef arange_value_type (*arange_value_copy_fn)(arange_value_type, void *);

/* Type of function that is called to combine two range values. */
typedef arange_value_type (*arange_value_combine_fn)(arange_value_type,
						     arange_value_type,
						     void *);

/* Type of function that is called to delete a range value. */
typedef void (*arange_value_delete_fn)(arange_value_type, void *);

/* Create an arange set.  Return the new set of NULL if there is any
   error.  */
extern arange_set arange_set_new (arange_set_allocate_fn,
				  arange_set_deallocate_fn,
				  bfd_boolean,
				  arange_value_equal_fn,
				  arange_value_copy_fn,
				  arange_value_combine_fn,
				  arange_value_delete_fn,
				  void *);

/*  Delete an arange set.  */
extern void arange_set_delete (arange_set);

/* Return TRUE if an only if arange set is empty.  */
extern bfd_boolean arange_set_empty_p (arange_set);

/* Check to see if a given address is in an arange set.  Return TRUE if the
   address is inside one of the aranges and if also low_ptr and high_ptr are
   not NULL, return the boundaries of the arange.

   If the address is not in any arange in set, return FALSE. */
extern bfd_boolean arange_set_lookup_address (arange_set, bfd_vma, bfd_vma *,
					      bfd_vma *, arange_value_type *);

/* Insert an arange [low,high] into a set.  Note that the address high is
   also included where as in DWARF2 an address range between low & high means
   [low,high).

   If the set is created with no capability of storing values, the value
   argument is ignored.  Otherwise, the value is stored in the inserted range.
   If there are overlapping ranges, values are combined according to
   value_combine_fn.

   If value is an object, arange_set_insert () takes ownership of that objec.
   Caller should not deallocate objects that are passed to arange_set_insert().

   Return TRUE if and only if there is no error.  */
extern bfd_boolean arange_set_insert (arange_set, bfd_vma, bfd_vma,
				      arange_value_type);

/* Return TRUE if and only if arange set supports arang evalues.  */
extern bfd_boolean arange_set_has_values_p (arange_set);

/* Traverse aranges in a set.  For each arange in ascending order of
   low addresses, call foreach_fn with arange boundaries and data.
   If any invocation of foreach_fn returns a non-zero value, stop traversal
   and return that value. Otherwise, return 0.  */
extern int arange_set_foreach (arange_set, arange_set_foreach_fn, void *);

/* Return TRUE if two values are considered equal by the value comparison
   function of an arange_set.  If the arange set does not support values or
   if it has no value equality function specified, this function performs
   a bit-wise comparison of its input.  */
extern bfd_boolean arange_set_value_equal_p (arange_set, arange_value_type,
					     arange_value_type);

/* Duplicate a value. If the arange set does not support values or if it
   has no value copying function specified, this function returns the input
   value.  */
extern arange_value_type arange_set_copy_value (arange_set, arange_value_type);

/* Allocate memory using the allocator of an arange set.  */
extern void * arange_set_allocate (arange_set, int);

/* Deallocate memory allocated from arange_set_allocate ().  */
extern void arange_set_deallocate (arange_set, void *);

#endif /* ARANGE_SET_H */
