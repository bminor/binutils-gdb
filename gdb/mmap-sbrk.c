/* GDB support for an sbrk-like function that uses mmap.
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

#include "defs.h"

#ifdef HAVE_MMAP

#include <fcntl.h>
#include <sys/mman.h>

#ifdef i386
#define MMAP_BASE ((caddr_t) 0x81000000)
#define MMAP_PGSZ 0x00002000  /* Must be multiple of real page size */
#else
#define MMAP_BASE ((caddr_t) 0xC2000000)
#define MMAP_PGSZ 0x00002000  /* Must be multiple of real page size */
#endif

#define PAGE_ALIGN(addr) (caddr_t) (((long)(addr) + MMAP_PGSZ - 1) & \
				    ~(MMAP_PGSZ - 1))

static caddr_t mbase = MMAP_BASE;	/* Current base of mmap'd region */
static caddr_t mbreak = MMAP_BASE;	/* Current "break" address */
static caddr_t mtop = MMAP_BASE;	/* Current top of mmap'd region */

static int fd = -1;			/* Open fd for /dev/zero */


/*  Provide a utility routine for other modules to obtain compatible
    page alignment. */

PTR
mmap_page_align (addr)
     PTR addr;
{
  return (PAGE_ALIGN (addr));
}

/*  Return the base address of the start of the mmap'd region.  Note that
    we can find the end of the region at anytime by calling mmap_sbrk(0) */

PTR
mmap_base ()
{
  return (mbase);
}

/*  Works like sbrk(), but uses mmap to add to or subtract from a
    memory region. */

PTR
mmap_sbrk (size)
     int size;
{
  PTR result = NULL;
  int minc;
  caddr_t moveto;

  if (size == 0)
    {
      /* Just return the current "break" value. */
      result = mbreak;
    }
  else if (size < 0)
    {
      /* We are deallocating memory.  If the amount requested would cause
	 us to try to deallocate back past the base of the mmap'd region
	 then do nothing, and return NULL.  Otherwise, deallocate the
	 memory and return the old break value. */
      if (mbreak + size >= mbase)
	{
	  result = (PTR) mbreak;
	  mbreak += size;
	  moveto = PAGE_ALIGN (mbreak);
	  munmap (moveto, (size_t) (mtop - moveto));
	  mtop = moveto;
	}
    }
  else
    {
      /* We are allocating memory.  Make sure we have an open file
	 descriptor and then go on to get the memory. */
      if ((fd == -1) && (fd = open ("/dev/zero", O_RDONLY)) < 0)
	{
	  result = NULL;
	}
      else if (mbreak + size > mtop)
	{
	  /* The request would move us past the end of the currently
	     mapped memory, so map in enough more memory to satisfy
	     the request. */
	  moveto = PAGE_ALIGN (mbreak + size);
	  if (mmap (mtop, moveto - mtop, PROT_READ | PROT_WRITE,
		    MAP_PRIVATE | MAP_FIXED, fd, 0) == mtop)
	    {
	      mtop = moveto;
	      result = (PTR) mbreak;
	      mbreak += size;
	    }
	}
      else
	{
	  result = (PTR) mbreak;
	  mbreak += size;
	}
    }
  return (result);
}

PTR
mmap_remap (base, mapsize, fd, foffset)
     PTR base;
     long mapsize;
     int fd;
     long foffset;
{
  /* FIXME:  Quick hack, needs error checking and other attention. */
  munmap (mbase, (size_t) (mtop - mbase));
  mbase = base;
  mtop = mbase + mapsize;
  base = mmap (base, mapsize, PROT_READ | PROT_WRITE,
	       MAP_PRIVATE | MAP_FIXED, dup (fd), foffset);
  return (base);
}


#endif	/* HAVE_MMAP */
