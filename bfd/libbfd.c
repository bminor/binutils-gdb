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

/*** libbfd.c -- random bfd support routines used internally only. */
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"



/** Dummies for targets that don't want or need to implement
   certain operations */

boolean
_bfd_dummy_new_section_hook (ignore, ignore_newsect)
     bfd *ignore;
     asection *ignore_newsect;
{
  return true;
}

boolean
bfd_false (ignore)
     bfd *ignore;
{
  return false;
}

boolean
bfd_true (ignore)
     bfd *ignore;
{
  return true;
}

void *
bfd_nullvoidptr(ignore)
bfd *ignore;
{
  return (void *)NULL;
}
int 
bfd_0(ignore)
bfd *ignore;
{
  return 0;
}
unsigned int 
bfd_0u(ignore)
bfd *ignore;
{
   return 0;
}

void 
bfd_void(ignore)
bfd *ignore;
{
}

boolean
_bfd_dummy_core_file_matches_executable_p (ignore_core_bfd, ignore_exec_bfd)
     bfd *ignore_core_bfd;
bfd *ignore_exec_bfd;
{
  bfd_error = invalid_operation;
  return false;
}

/* of course you can't initialize a function to be the same as another, grr */

char *
_bfd_dummy_core_file_failing_command (ignore_abfd)
     bfd *ignore_abfd;
{
  return (char *)NULL;
}

int
_bfd_dummy_core_file_failing_signal (ignore_abfd)
     bfd *ignore_abfd;
{
  return 0;
}

bfd_target *
_bfd_dummy_target (ignore_abfd)
     bfd *ignore_abfd;
{
  return 0;
}

/** zalloc -- allocate and clear storage */


#ifndef zalloc
char *
zalloc (size)
     size_t size;
{
  char *ptr = malloc (size);

  if ((ptr != NULL) && (size != 0))
    bzero (ptr, size);

  return ptr;
}
#endif

/* Some IO code */


/* Note that archive entries don't have streams; they share their parent's.
   This allows someone to play with the iostream behind bfd's back.

   Also, note that the origin pointer points to the beginning of a file's
   contents (0 for non-archive elements).  For archive entries this is the
   first octet in the file, NOT the beginning of the archive header. */

size_t
bfd_read (ptr, size, nitems, abfd)
     void *ptr;
     size_t size;
     size_t nitems;
     bfd *abfd;
{
  return fread (ptr, 1, size*nitems, bfd_cache_lookup(abfd));
}

size_t
bfd_write (ptr, size, nitems, abfd)
     void *ptr;
     size_t size;
     size_t nitems;
     bfd *abfd;
{
  return  fwrite (ptr, 1, size*nitems, bfd_cache_lookup(abfd));
}

int
bfd_seek (abfd, position, direction)
bfd *abfd;
file_ptr position;
int direction;
{
	/* For the time being, a bfd may not seek to it's end.  The
	   problem is that we don't easily have a way to recognize
	   the end of an element in an archive. */

	BFD_ASSERT(direction == SEEK_SET
		   || direction == SEEK_CUR);
	
	if (direction == SEEK_SET && abfd->my_archive != NULL) 
	    {
		    /* This is a set within an archive, so we need to
		       add the base of the object within the archive */
		    return(fseek(bfd_cache_lookup(abfd),
				 position + abfd->origin,
				 direction));
	    }
	else 
	    {
		    return(fseek(bfd_cache_lookup(abfd),  position, direction));
	    }	
}

long
bfd_tell (abfd)
     bfd *abfd;
{
	file_ptr ptr;

	ptr = ftell (bfd_cache_lookup(abfd));

	if (abfd->my_archive)
	    ptr -= abfd->origin;
	return ptr;
}

/** Make a string table */

/* Add string to table pointed to by table, at location starting with free_ptr.
   resizes the table if necessary (if it's NULL, creates it, ignoring
   table_length).  Updates free_ptr, table, table_length */

boolean
bfd_add_to_string_table (table, new_string, table_length, free_ptr)
     char **table, **free_ptr;
     char *new_string;
     unsigned int *table_length;
{
  size_t string_length = strlen (new_string) + 1; /* include null here */
  char *base = *table;
  size_t space_length = *table_length;
  unsigned int offset = (base ? *free_ptr - base : 0);

  if (base == NULL) {
    /* Avoid a useless regrow if we can (but of course we still
       take it next time */
    space_length = (string_length < DEFAULT_STRING_SPACE_SIZE ?
		    DEFAULT_STRING_SPACE_SIZE : string_length+1);
    base = zalloc (space_length);

    if (base == NULL) {
      bfd_error = no_memory;
      return false;
    }
  }

  if ((size_t)(offset + string_length) >= space_length) {
    /* Make sure we will have enough space */
    while ((size_t)(offset + string_length) >= space_length) 
      space_length += space_length/2; /* grow by 50% */

    base = (char *) realloc (base, space_length);
    if (base == NULL) {
      bfd_error = no_memory;
      return false;
    }

  }

  memcpy (base + offset, new_string, string_length);
  *table = base;
  *table_length = space_length;
  *free_ptr = base + offset + string_length;
  
  return true;
}

/** The do-it-yourself (byte) sex-change kit */

/* The middle letter e.g. get<b>short indicates Big or Little endian
   target machine.  It doesn't matter what the byte order of the host
   machine is; these routines work for either.  */

/* FIXME: Should these take a count argument?
   Answer (gnu@cygnus.com):  No, but perhaps they should be inline
 			     functions in swap.h #ifdef __GNUC__. 
  			     Gprof them later and find out.  */

short
_do_getbshort (addr)
	register bfd_byte *addr;
{
	return (addr[0] << 8) | addr[1];
}

short
_do_getlshort (addr)
	register bfd_byte *addr;
{
	return (addr[1] << 8) | addr[0];
}

void
_do_putbshort (data, addr)
	int data;		/* Actually short, but ansi C sucks */
	register bfd_byte *addr;
{
	addr[0] = (bfd_byte)(data >> 8);
	addr[1] = (bfd_byte )data;
}

void
_do_putlshort (data, addr)
	int data;		/* Actually short, but ansi C sucks */
	register bfd_byte *addr;
{
	addr[0] = (bfd_byte )data;
	addr[1] = (bfd_byte)(data >> 8);
}

long
_do_getblong (addr)
	register bfd_byte *addr;
{
	return ((((addr[0] << 8) | addr[1]) << 8) | addr[2]) << 8 | addr[3];
}

long
_do_getllong (addr)
	register bfd_byte *addr;
{
	return ((((addr[3] << 8) | addr[2]) << 8) | addr[1]) << 8 | addr[0];
}

void
_do_putblong (data, addr)
	unsigned long data;
	register bfd_byte *addr;
{
	addr[0] = (bfd_byte)(data >> 24);
	addr[1] = (bfd_byte)(data >> 16);
	addr[2] = (bfd_byte)(data >>  8);
	addr[3] = (bfd_byte)data;
}

void
_do_putllong (data, addr)
	unsigned long data;
	register bfd_byte *addr;
{
	addr[0] = (bfd_byte)data;
	addr[1] = (bfd_byte)(data >>  8);
	addr[2] = (bfd_byte)(data >> 16);
	addr[3] = (bfd_byte)(data >> 24);
}







