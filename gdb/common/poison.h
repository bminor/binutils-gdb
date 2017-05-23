/* Poison symbols at compile time.

   Copyright (C) 2017 Free Software Foundation, Inc.

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

#ifndef COMMON_POISON_H
#define COMMON_POISON_H

#include "traits.h"

/* Poison memset of non-POD types.  The idea is catching invalid
   initialization of non-POD structs that is easy to be introduced as
   side effect of refactoring.  For example, say this:

 struct S { VEC(foo_s) *m_data; };

is converted to this at some point:

 struct S {
   S() { m_data.reserve (10); }
   std::vector<foo> m_data;
 };

and old code was initializing S objects like this:

 struct S s;
 memset (&s, 0, sizeof (S)); // whoops, now wipes vector.

Declaring memset as deleted for non-POD types makes the memset above
be a compile-time error.  */

/* Helper for SFINAE.  True if "T *" is memsettable.  I.e., if T is
   either void, or POD.  */
template<typename T>
struct IsMemsettable
  : gdb::Or<std::is_void<T>,
	    std::is_pod<T>>
{};

template <typename T,
	  typename = gdb::Requires<gdb::Not<IsMemsettable<T>>>>
void *memset (T *s, int c, size_t n) = delete;

#if HAVE_IS_TRIVIALLY_COPYABLE

/* Similarly, poison memcpy and memmove of non trivially-copyable
   types, which is undefined.  */

/* True if "T *" is relocatable.  I.e., copyable with memcpy/memmove.
   I.e., T is either trivially copyable, or void.  */
template<typename T>
struct IsRelocatable
  : gdb::Or<std::is_void<T>,
	    std::is_trivially_copyable<T>>
{};

/* True if both source and destination are relocatable.  */

template <typename D, typename S>
using BothAreRelocatable
  = gdb::And<IsRelocatable<D>, IsRelocatable<S>>;

template <typename D, typename S,
	  typename = gdb::Requires<gdb::Not<BothAreRelocatable<D, S>>>>
void *memcpy (D *dest, const S *src, size_t n) = delete;

template <typename D, typename S,
	  typename = gdb::Requires<gdb::Not<BothAreRelocatable<D, S>>>>
void *memmove (D *dest, const S *src, size_t n) = delete;

#endif /* HAVE_IS_TRIVIALLY_COPYABLE */

#endif /* COMMON_POISON_H */
