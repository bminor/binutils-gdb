/* Standard debugging hooks for `malloc'.
   Copyright 1990 Free Software Foundation
		  Written May 1989 by Mike Haertel.

   The author may be reached (Email) at the address mike@ai.mit.edu,
   or (US mail) as Mike Haertel c/o Free Software Foundation.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "ansidecl.h"
#include <stdlib.h>
#include "gmalloc.h"

/* Old hook values.  */
static void EXFUN((*old_free_hook), (PTR ptr));
static PTR EXFUN((*old_malloc_hook), (size_t size));
static PTR EXFUN((*old_realloc_hook), (PTR ptr, size_t size));

/* Function to call when something awful happens. */
static void EXFUN((*abortfunc), (void)) = abort;

/* Arbitrary magical numbers.  */
#define MAGICWORD	0xfedabeeb
#define MAGICBYTE	((char) 0xd7)

struct hdr
  {
    size_t size;		/* Exact size requested by user.  */
    unsigned int magic;		/* Magic number to check header integrity.  */
  };

static void
DEFUN(checkhdr, (hdr), CONST struct hdr *hdr)
{
  if (hdr->magic != MAGICWORD || ((char *) &hdr[1])[hdr->size] != MAGICBYTE)
    (*abortfunc)();
}

static void
DEFUN(freehook, (ptr), PTR ptr)
{
  struct hdr *hdr = ((struct hdr *) ptr) - 1;
  checkhdr(hdr);
  hdr->magic = 0;
  __free_hook = old_free_hook;
  free(hdr);
  __free_hook = freehook;
}

static PTR
DEFUN(mallochook, (size), size_t size)
{
  struct hdr *hdr;

  __malloc_hook = old_malloc_hook;
  hdr = (struct hdr *) malloc(sizeof(struct hdr) + size + 1);
  __malloc_hook = mallochook;
  if (hdr == NULL)
    return NULL;

  hdr->size = size;
  hdr->magic = MAGICWORD;
  ((char *) &hdr[1])[size] = MAGICBYTE;
  return (PTR) (hdr + 1);
}

static PTR
DEFUN(reallochook, (ptr, size), PTR ptr AND size_t size)
{
  struct hdr *hdr = ((struct hdr *) ptr) - 1;

  checkhdr(hdr);
  __free_hook = old_free_hook;
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  hdr = (struct hdr *) realloc((PTR) hdr, sizeof(struct hdr) + size + 1);
  __free_hook = freehook;
  __malloc_hook = mallochook;
  __realloc_hook = reallochook;
  if (hdr == NULL)
    return NULL;

  hdr->size = size;
  ((char *) &hdr[1])[size] = MAGICBYTE;
  return (PTR) (hdr + 1);
}

void
DEFUN(mcheck, (func), void EXFUN((*func), (void)))
{
  static int mcheck_used = 0;

  if (func)
    abortfunc = func;

  /* These hooks may not be safely inserted if malloc is already in use.  */
  if (!__malloc_initialized && !mcheck_used)
    {
      old_free_hook = __free_hook;
      __free_hook = freehook;
      old_malloc_hook = __malloc_hook;
      __malloc_hook = mallochook;
      old_realloc_hook = __realloc_hook;
      __realloc_hook = reallochook;
      mcheck_used = 1;
    }
}
