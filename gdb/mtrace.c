/* More debugging hooks for `malloc'.
   Copyright 1991 Free Software Foundation
		  Written April 2, 1991 by John Gilmore of Cygnus Support
		  Based on mcheck.c by Mike Haertel.

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

#include <stdio.h>
#include "ansidecl.h"

/* size_t may be defined in the system-supplied stdio.h.  */
/* So just kludge it.  */
#define size_t unsigned int
#define ptrdiff_t int
#define __ONEFILE

/* We can't declare malloc and realloc here because we don't know
   if they are char * or void *, and the compiler will give an error
   if we get it wrong and they happen to be defined in some header
   file e.g. <stdio.h>.  We can't include <stdlib.h> here because
   it has some incompatability with our own includes, e.g. size_t or 
   whatever.  So we just punt.  This causes malloc and realloc to
   default to returning "int", which works for most cases we care
   about.  FIXME-somehow.  */
/* #include <stdlib.h> */
#include "gmalloc.h"

extern char *getenv();

FILE *mallstream;
char mallenv[] = "MALLOC_TRACE";
static char mallbuf[BUFSIZ];		/* Buffer for the output */

/* Address to breakpoint on accesses to... */
PTR mallwatch;

/* Old hook values.  */
static void EXFUN((*old_free_hook), (PTR ptr));
static PTR EXFUN((*old_malloc_hook), (size_t size));
static PTR EXFUN((*old_realloc_hook), (PTR ptr, size_t size));

/* This function is called when the block being alloc'd, realloc'd, or
   freed has an address matching the variable "mallwatch".  In a debugger,
   set "mallwatch" to the address of interest, then put a breakpoint on
   tr_break.  */

void
tr_break()
{
  ;
}

static void
DEFUN(tr_freehook, (ptr), PTR ptr)
{
  fprintf(mallstream, "- %08x\n", ptr);	/* Be sure to print it first */
  if (ptr == mallwatch)
    tr_break();
  __free_hook = old_free_hook;
  free(ptr);
  __free_hook = tr_freehook;
}

static PTR
DEFUN(tr_mallochook, (size), size_t size)
{
  PTR hdr;

  __malloc_hook = old_malloc_hook;
  hdr = (PTR) malloc(size);
  __malloc_hook = tr_mallochook;

  /* We could be printing a NULL here; that's OK */
  fprintf (mallstream, "+ %08x %x\n", hdr, size);

  if (hdr == mallwatch)
    tr_break();

  return hdr;
}

static PTR
DEFUN(tr_reallochook, (ptr, size), PTR ptr AND size_t size)
{
  PTR hdr;

  if (ptr == mallwatch)
    tr_break();

  __free_hook = old_free_hook;
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  hdr = (PTR) realloc(ptr, size);
  __free_hook = tr_freehook;
  __malloc_hook = tr_mallochook;
  __realloc_hook = tr_reallochook;
  if (hdr == NULL) {
    fprintf (mallstream, "! %08x %x\n", ptr, size);	/* Failed realloc */
  } else {
    fprintf (mallstream, "< %08x\n> %08x %x\n", ptr, hdr, size);
  }

  if (hdr == mallwatch)
    tr_break();

  return hdr;
}

/* We enable tracing if either the environment variable MALLOC_TRACE
   is set, or if the variable mallwatch has been patched to an address
   that the debugging user wants us to stop on.  When patching mallwatch,
   don't forget to set a breakpoint on tr_break!  */

void
mtrace()
{
  char *mallfile;

  mallfile = getenv (mallenv);
  if (mallfile || mallwatch) {
    mallstream = fopen (mallfile? mallfile: "/dev/null", "w");
    if (mallstream) {
      /* Be sure it doesn't malloc its buffer! */
      setbuf (mallstream, mallbuf);
      fprintf (mallstream, "= Start\n");
      old_free_hook = __free_hook;
      __free_hook = tr_freehook;
      old_malloc_hook = __malloc_hook;
      __malloc_hook = tr_mallochook;
      old_realloc_hook = __realloc_hook;
      __realloc_hook = tr_reallochook;
    }
  }
}
