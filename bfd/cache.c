/*** cache.c -- Allows you to have more bfds open than your system has fds. */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ */
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"

/* The maximum number of FDs opened by bfd */
#define BFD_CACHE_MAX_OPEN 10

/* when this exceeds BFD_CACHE_MAX_OPEN, we get to work */
static int open_files;

static bfd *cache_sentinel;	/* Chain of bfds with active fds we've
				   opened */

bfd *bfd_last_cache;		/* Zero, or a pointer to the topmost
				   bfd on the chain.  This is used by the
				   bfd_cache_lookup() macro in libbfd.h
				   to determine when it can avoid a function
				   call.  */

static void bfd_cache_delete();


static void
DEFUN_VOID(close_one)
{
    bfd *kill = cache_sentinel;
    if (kill == 0)		/* Nothing in the cache */
	return ;

    /* We can only close files that want to play this game.  */
    while (!kill->cacheable) {
	kill = kill->lru_prev;
	if (kill == cache_sentinel) /* Nobody wants to play */
	   return ;
    }

    kill->where = ftell((FILE *)(kill->iostream));
    bfd_cache_delete(kill);
}

/* Cuts the bfd abfd out of the chain in the cache */
static void 
DEFUN(snip,(abfd),
      bfd *abfd)
{
  abfd->lru_prev->lru_next = abfd->lru_next;
  abfd->lru_next->lru_prev = abfd->lru_prev; 
  if (cache_sentinel == abfd) cache_sentinel = (bfd *)NULL;
}

static void
DEFUN(bfd_cache_delete,(abfd),
      bfd *abfd)
{
  fclose ((FILE *)(abfd->iostream));
  snip (abfd);
  abfd->iostream = NULL;
  open_files--;
  bfd_last_cache = 0;
}
  
static bfd *
DEFUN(insert,(x,y),
      bfd *x AND
      bfd *y)
{
  if (y) {
    x->lru_next = y;
    x->lru_prev = y->lru_prev;
    y->lru_prev->lru_next = x;
    y->lru_prev = x;

  }
  else {
    x->lru_prev = x;
    x->lru_next = x;
  }
  return x;
}


/* Initialize a BFD by putting it on the cache LRU.  */
void
DEFUN(bfd_cache_init,(abfd),
      bfd *abfd)
{
  cache_sentinel = insert(abfd, cache_sentinel);
}

void
DEFUN(bfd_cache_close,(abfd),
      bfd *abfd)
{
  /* If this file is open then remove from the chain */
  if (abfd->iostream) 
    {
      bfd_cache_delete(abfd);
    }
}

/* Call the OS to open a file for this BFD.  Returns the FILE *
   (possibly null) that results from this operation.  Sets up the
   BFD so that future accesses know the file is open.  */

FILE *
DEFUN(bfd_open_file, (abfd),
      bfd *abfd)
{
    abfd->cacheable = true;	/* Allow it to be closed later. */
    if(open_files >= BFD_CACHE_MAX_OPEN) {
	close_one();
    }
    switch (abfd->direction) {
      case read_direction:
      case no_direction:
	abfd->iostream = (char *) fopen(abfd->filename, "r");
	break;
      case both_direction:
      case write_direction:
	if (abfd->opened_once == true) {
	    abfd->iostream = (char *) fopen(abfd->filename, "r+");
	    if (!abfd->iostream) {
		abfd->iostream = (char *) fopen(abfd->filename, "w+");
	    }
	} else {
	    /*open for creat */
	    abfd->iostream = (char *) fopen(abfd->filename, "w");
	    abfd->opened_once = true;
	}
	break;
    }
    if (abfd->iostream) {
	open_files++;
	bfd_cache_init (abfd);
    }

    return (FILE *)(abfd->iostream);
}

/* Find a file descriptor for this BFD.  If necessary, open it.
   If there are already more than BFD_CACHE_MAX_OPEN files open, try to close
   one first, to avoid running out of file descriptors.  */

FILE *
DEFUN(bfd_cache_lookup_worker,(abfd),
      bfd *abfd)
{
  if (abfd->my_archive) 
      {
	abfd = abfd->my_archive;
      }
  /* Is this file already open .. if so then quick exit */
  if (abfd->iostream) 
      {
	if (abfd != cache_sentinel) {
	  /* Place onto head of lru chain */
	  snip (abfd);
	  cache_sentinel = insert(abfd, cache_sentinel);
	}
      }
  /* This is a bfd without a stream -
     so it must have been closed or never opened.
     find an empty cache entry and use it.  */
  else 
      {

	if (open_files >= BFD_CACHE_MAX_OPEN) 
	    {
	      close_one();
	    }

	BFD_ASSERT(bfd_open_file (abfd) != (FILE *)NULL) ;
	fseek((FILE *)(abfd->iostream), abfd->where, false);
      }
  bfd_last_cache = abfd;
  return (FILE *)(abfd->iostream);
}
