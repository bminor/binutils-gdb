/* GDB support for special malloc using mmap.
   Copyright 1992 Free Software Foundation, Inc.
   Contributed by Cygnus Support, using pieces from other GDB modules.

This file is part of GDB.

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
#include "defs.h"

#if defined (HAVE_MMAP)

/* Redefine the external visible symbols in gmalloc.c to be mmap versions */

#define free			_mmap_free
#define malloc			_mmap_malloc
#define realloc			_mmap_realloc
#define valloc			_mmap_valloc

#define _bytes_free		_mmap__bytes_free
#define _bytes_used		_mmap__bytes_used
#define _chunks_free		_mmap__chunks_free
#define _chunks_used 		_mmap__chunks_used
#define _fraghead		_mmap__fraghead
#define _heapbase 		_mmap__heapbase
#define _heapindex		_mmap__heapindex
#define _heapinfo		_mmap__heapinfo
#define _heaplimit		_mmap__heaplimit

#define __default_morecore	_mmap___default_morecore
#define __free			_mmap___free
#define __free_hook		_mmap___free_hook
#define __malloc_hook		_mmap___malloc_hook
#define __malloc_initialized	_mmap___malloc_initialized
#define __morecore		_mmap___morecore
#define __realloc_hook		_mmap___realloc_hook

/* Arrange that instead of calling sbrk() we call mmap_sbrk() */

#define sbrk			mmap_sbrk

/* Now simply include the standard GNU malloc source, and all the
   externally visible symbols will become _mmap_* versions, and
   _mmap_sbrk will be called to get more core instead of sbrk. */

#include "gmalloc.c"

/* Like mmap_malloc but get error if no storage available.  */

PTR
mmap_xmalloc (size)
     long size;
{
  register char *val = NULL;

  /* Protect against gdb wanting to allocate zero bytes. */

  if (size > 0)
    {
      if ((val = (char *) _mmap_malloc (size)) == NULL)
	{
	  fatal ("virtual memory exhausted.", 0);
	}
    }
  return (val);
}

/* Like mmap_realloc but get error if no storage available.  */

PTR
mmap_xrealloc (ptr, size)
     PTR ptr;
     long size;
{
  register char *val;

  if ((val = (char *) _mmap_realloc (ptr, size)) == NULL)
    {
      fatal ("virtual memory exhausted.", 0);
    }
  return (val);
}

PTR
mmap_malloc (size)
     long size;
{
  return (_mmap_malloc (size));
}

PTR
mmap_realloc (ptr, size)
     PTR ptr;
     long size;
{
  return (_mmap_realloc (ptr, size));
}

void
mmap_free (ptr)
     PTR ptr;
{
  _mmap_free (ptr);
}

#else	/* !defined (HAVE_MMAP) */

static char *errmsg = "This version of gdb does not support dumpable state.";

PTR
mmap_malloc (size)
     long size;
{
  error (errmsg);
}

PTR
mmap_xmalloc (size)
     long size;
{
  error (errmsg);
}

PTR
mmap_realloc (ptr, size)
     PTR ptr;
     long size;
{
  error (errmsg);
}

PTR
mmap_xrealloc (ptr, size)
     PTR ptr;
     long size;
{
  error (errmsg);
}

void
mmap_free (ptr)
     PTR ptr;
{
  error (errmsg);
}

#endif	/* HAVE_MMAP */
