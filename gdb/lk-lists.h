/* Iterators for internal data structures of the Linux kernel.

   Copyright (C) 2016 Free Software Foundation, Inc.

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

#ifndef __LK_LISTS_H__
#define __LK_LISTS_H__

extern CORE_ADDR lk_list_head_next (CORE_ADDR curr, const char *sname,
				    const char *fname);

/* Iterator over field SNAME->FNAME of type struct list_head starting at
   address START of type struct list_head.  This iterator is intended to be
   used for lists initiated with macro LIST_HEAD (include/linux/list.h) in
   the kernel, i.e. lists that START is a global variable of type struct
   list_head and _not_ of type struct SNAME as the rest of the list.  Thus
   START will not be iterated over but only be used to start/terminate the
   iteration.  */

#define lk_list_for_each(next, start, sname, fname)		\
  for ((next) = lk_list_head_next ((start), #sname, #fname);	\
       (next) != (start);					\
       (next) = lk_list_head_next ((next), #sname, #fname))

/* Iterator over struct SNAME linked together via field SNAME->FNAME of type
   struct list_head starting at address START of type struct SNAME.  In
   contrast to the iterator above, START is a "full" member of the list and
   thus will be iterated over.  */

#define lk_list_for_each_container(cont, start, sname, fname)	\
  CORE_ADDR _next;						\
  bool _first_loop = true;					\
  for ((cont) = (start),					\
       _next = (start) + LK_OFFSET (sname, fname);		\
								\
       (cont) != (start) || _first_loop;			\
								\
       _next = lk_list_head_next (_next, #sname, #fname),	\
       (cont) = LK_CONTAINER_OF (_next, sname, fname),		\
       _first_loop = false)

#endif /* __LK_LISTS_H__ */
