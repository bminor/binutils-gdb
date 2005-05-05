/* sb.c - string buffer manipulation routines
   Copyright 1994, 1995, 2000, 2003 Free Software Foundation, Inc.

   Written by Steve and Judy Chamberlain of Cygnus Support,
      sac@cygnus.com

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "config.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include "libiberty.h"
#include "sb.h"

/* These routines are about manipulating strings.

   They are managed in things called `sb's which is an abbreviation
   for string buffers.  An sb has to be created, things can be glued
   on to it, and at the end of it's life it should be freed.  The
   contents should never be pointed at whilst it is still growing,
   since it could be moved at any time

   eg:
   sb_new (&foo);
   sb_grow... (&foo,...);
   use foo->ptr[*];
   sb_kill (&foo);

*/

static int dsize = 5;
static void sb_check (sb *, int);

/* Statistics of sb structures.  */
static int string_count[sb_max_power_two];

/* Free list of sb structures.  */
static sb_list_vector free_list;

/* initializes an sb.  */

static void
sb_build (sb *ptr, int size)
{
  /* see if we can find one to allocate */
  sb_element *e;

  if (size > sb_max_power_two)
    abort ();

  e = free_list.size[size];
  if (!e)
    {
      /* nothing there, allocate one and stick into the free list */
      e = (sb_element *) xmalloc (sizeof (sb_element) + (1 << size));
      e->next = free_list.size[size];
      e->size = 1 << size;
      free_list.size[size] = e;
      string_count[size]++;
    }

  /* remove from free list */

  free_list.size[size] = e->next;

  /* copy into callers world */
  ptr->ptr = e->data;
  ptr->pot = size;
  ptr->len = 0;
  ptr->item = e;
}

void
sb_new (sb *ptr)
{
  sb_build (ptr, dsize);
}

/* deallocate the sb at ptr */

void
sb_kill (sb *ptr)
{
  /* return item to free list */
  ptr->item->next = free_list.size[ptr->pot];
  free_list.size[ptr->pot] = ptr->item;
}

/* add the sb at s to the end of the sb at ptr */

void
sb_add_sb (sb *ptr, sb *s)
{
  sb_check (ptr, s->len);
  memcpy (ptr->ptr + ptr->len, s->ptr, s->len);
  ptr->len += s->len;
}

/* make sure that the sb at ptr has room for another len characters,
   and grow it if it doesn't.  */

static void
sb_check (sb *ptr, int len)
{
  if (ptr->len + len >= 1 << ptr->pot)
    {
      sb tmp;
      int pot = ptr->pot;
      while (ptr->len + len >= 1 << pot)
	pot++;
      sb_build (&tmp, pot);
      sb_add_sb (&tmp, ptr);
      sb_kill (ptr);
      *ptr = tmp;
    }
}

/* make the sb at ptr point back to the beginning.  */

void
sb_reset (sb *ptr)
{
  ptr->len = 0;
}

/* add character c to the end of the sb at ptr.  */

void
sb_add_char (sb *ptr, int c)
{
  sb_check (ptr, 1);
  ptr->ptr[ptr->len++] = c;
}

/* add null terminated string s to the end of sb at ptr.  */

void
sb_add_string (sb *ptr, const char *s)
{
  int len = strlen (s);
  sb_check (ptr, len);
  memcpy (ptr->ptr + ptr->len, s, len);
  ptr->len += len;
}

/* like sb_name, but don't include the null byte in the string.  */

char *
sb_terminate (sb *in)
{
  sb_add_char (in, 0);
  --in->len;
  return in->ptr;
}

/* start at the index idx into the string in sb at ptr and skip
   whitespace. return the index of the first non whitespace character */

int
sb_skip_white (int idx, sb *ptr)
{
  while (idx < ptr->len
	 && (ptr->ptr[idx] == ' '
	     || ptr->ptr[idx] == '\t'))
    idx++;
  return idx;
}

/* start at the index idx into the sb at ptr. skips whitespace,
   a comma and any following whitespace. returns the index of the
   next character.  */

int
sb_skip_comma (int idx, sb *ptr)
{
  while (idx < ptr->len
	 && (ptr->ptr[idx] == ' '
	     || ptr->ptr[idx] == '\t'))
    idx++;

  if (idx < ptr->len
      && ptr->ptr[idx] == ',')
    idx++;

  while (idx < ptr->len
	 && (ptr->ptr[idx] == ' '
	     || ptr->ptr[idx] == '\t'))
    idx++;

  return idx;
}
