/* hash.c -- gas hash table code
   Copyright (C) 1987-2020 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "as.h"

/* Insert ELEMENT into HTAB.  If the element exists, it is overwritten.  */

void
htab_insert (htab_t htab, PTR element)
{
  void **slot = htab_find_slot (htab, element, INSERT);
  if (slot != NULL && htab->del_f)
    (*htab->del_f) (*slot);

  *slot = element;
}

/* Print statistics about a hash table.  */

void
htab_print_statistics (FILE *f, const char *name, htab_t table)
{
  fprintf (f, "%s hash statistics:\n", name);
  fprintf (f, "\t%u searches\n", table->searches);
  fprintf (f, "\t%u collisions\n", table->collisions);
  fprintf (f, "\t%lu elements\n", (unsigned long) htab_elements (table));
  fprintf (f, "\t%lu table size\n", (unsigned long) htab_size (table));
}
