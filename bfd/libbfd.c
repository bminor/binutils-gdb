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
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"



/** Dummies for targets that don't want or need to implement
   certain operations */

boolean
DEFUN(_bfd_dummy_new_section_hook,(ignore, ignore_newsect),
      bfd *ignore AND
      asection *ignore_newsect)
{
  return true;
}

boolean
DEFUN(bfd_false ,(ignore),
      bfd *ignore)
{
  return false;
}

boolean
DEFUN(bfd_true,(ignore),
      bfd *ignore)
{
  return true;
}

PTR
DEFUN(bfd_nullvoidptr,(ignore),
      bfd *ignore)
{
  return (PTR)NULL;
}

int 
DEFUN(bfd_0,(ignore),
      bfd *ignore)
{
  return 0;
}

unsigned int 
DEFUN(bfd_0u,(ignore),
      bfd *ignore)
{
   return 0;
}

void 
DEFUN(bfd_void,(ignore),
      bfd *ignore)
{
}

boolean
DEFUN(_bfd_dummy_core_file_matches_executable_p,(ignore_core_bfd, ignore_exec_bfd),
      bfd *ignore_core_bfd AND
      bfd *ignore_exec_bfd)
{
  bfd_error = invalid_operation;
  return false;
}

/* of course you can't initialize a function to be the same as another, grr */

char *
DEFUN(_bfd_dummy_core_file_failing_command,(ignore_abfd),
      bfd *ignore_abfd)
{
  return (char *)NULL;
}

int
DEFUN(_bfd_dummy_core_file_failing_signal,(ignore_abfd),
     bfd *ignore_abfd)
{
  return 0;
}

bfd_target *
DEFUN(_bfd_dummy_target,(ignore_abfd),
     bfd *ignore_abfd)
{
  return 0;
}

/** zalloc -- allocate and clear storage */


#ifndef zalloc
char *
DEFUN(zalloc,(size),
      bfd_size_type size)
{
  char *ptr = (char *) malloc ((int)size);

  if ((ptr != NULL) && (size != 0))
   memset(ptr,0, size);

  return ptr;
}
#endif

/* Some IO code */


/* Note that archive entries don't have streams; they share their parent's.
   This allows someone to play with the iostream behind bfd's back.

   Also, note that the origin pointer points to the beginning of a file's
   contents (0 for non-archive elements).  For archive entries this is the
   first octet in the file, NOT the beginning of the archive header. */

static 
int DEFUN(real_read,(where, a,b, file),
	  PTR where AND
	  int a AND
	  int b AND
	  FILE *file)
{
  return fread(where, a,b,file);
}
bfd_size_type
DEFUN(bfd_read,(ptr, size, nitems, abfd),
      PTR ptr AND
      bfd_size_type size AND
      bfd_size_type nitems AND
      bfd *abfd)
{
  return (bfd_size_type)real_read (ptr, 1, (int)(size*nitems), bfd_cache_lookup(abfd));
}

bfd_size_type
DEFUN(bfd_write,(ptr, size, nitems, abfd),
      PTR ptr AND
      bfd_size_type size AND
      bfd_size_type nitems AND
      bfd *abfd)
{
  return fwrite (ptr, 1, (int)(size*nitems), bfd_cache_lookup(abfd));
}

int
DEFUN(bfd_seek,(abfd, position, direction),
      bfd * CONST abfd AND
      CONST file_ptr position AND
      CONST int direction)
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
DEFUN(bfd_tell,(abfd),
      bfd *abfd)
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
DEFUN(bfd_add_to_string_table,(table, new_string, table_length, free_ptr),
      char **table AND
      char *new_string AND
      unsigned int *table_length AND
      char **free_ptr)
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

unsigned int
DEFUN(_do_getb16,(addr),
      register bfd_byte *addr)
{
	return (addr[0] << 8) | addr[1];
}

unsigned int
DEFUN(_do_getl16,(addr),
      register bfd_byte *addr)
{
	return (addr[1] << 8) | addr[0];
}

void
DEFUN(_do_putb16,(data, addr),
      int data AND
      register bfd_byte *addr)
{
	addr[0] = (bfd_byte)(data >> 8);
	addr[1] = (bfd_byte )data;
}

void
DEFUN(_do_putl16,(data, addr),
      int data AND		
      register bfd_byte *addr)
{
	addr[0] = (bfd_byte )data;
	addr[1] = (bfd_byte)(data >> 8);
}

unsigned int
DEFUN(_do_getb32,(addr),
      register bfd_byte *addr)
{
	return ((((addr[0] << 8) | addr[1]) << 8) | addr[2]) << 8 | addr[3];
}

unsigned int
_do_getl32 (addr)
	register bfd_byte *addr;
{
	return ((((addr[3] << 8) | addr[2]) << 8) | addr[1]) << 8 | addr[0];
}

bfd_64_type
DEFUN(_do_getb64,(addr),
      register bfd_byte *addr)
{
#ifdef HOST_64_BIT
  bfd_64_type low, high;

  high= ((((((((addr[0]) << 8) |
	      addr[1]) << 8) |
	    addr[2]) << 8) |
	  addr[3]) );

  low = ((((((((addr[4]) << 8) |
	      addr[5]) << 8) |
	    addr[6]) << 8) |
	  addr[7]));

  return high << 32 | low;
#else
  BFD_FAIL();
#endif

}

bfd_64_type
DEFUN(_do_getl64,(addr),
      register bfd_byte *addr)
{
  bfd_64_type low, high;
#ifdef HOST_64_BIT
  high= (((((((addr[7] << 8) |
	      addr[6]) << 8) |
	    addr[5]) << 8) |
	  addr[4]));

  low = (((((((addr[3] << 8) |
	      addr[2]) << 8) |
	    addr[1]) << 8) |
	  addr[0]) );

  return high << 32 | low;
#else
  BFD_FAIL();
#endif
}

void
DEFUN(_do_putb32,(data, addr),
      unsigned long data AND
      register bfd_byte *addr)
{
	addr[0] = (bfd_byte)(data >> 24);
	addr[1] = (bfd_byte)(data >> 16);
	addr[2] = (bfd_byte)(data >>  8);
	addr[3] = (bfd_byte)data;
}

void
DEFUN(_do_putl32,(data, addr),
      unsigned long data AND
      register bfd_byte *addr)
{
	addr[0] = (bfd_byte)data;
	addr[1] = (bfd_byte)(data >>  8);
	addr[2] = (bfd_byte)(data >> 16);
	addr[3] = (bfd_byte)(data >> 24);
}
void
DEFUN(_do_putb64,(data, addr),
	bfd_64_type data AND
	register bfd_byte *addr)
{
#ifdef HOST_64_BIT
  addr[0] = (bfd_byte)(data >> (7*8));
  addr[1] = (bfd_byte)(data >> (6*8));
  addr[2] = (bfd_byte)(data >> (5*8));
  addr[3] = (bfd_byte)(data >> (4*8));
  addr[4] = (bfd_byte)(data >> (3*8));
  addr[5] = (bfd_byte)(data >> (2*8));
  addr[6] = (bfd_byte)(data >> (1*8));
  addr[7] = (bfd_byte)(data >> (0*8));
#else
  BFD_FAIL();
#endif

}

void
DEFUN(_do_putl64,(data, addr),
      bfd_64_type data AND
      register bfd_byte *addr)
{
#ifdef HOST_64_BIT
  addr[7] = (bfd_byte)(data >> (7*8));
  addr[6] = (bfd_byte)(data >> (6*8));
  addr[5] = (bfd_byte)(data >> (5*8));
  addr[4] = (bfd_byte)(data >> (4*8));
  addr[3] = (bfd_byte)(data >> (3*8));
  addr[2] = (bfd_byte)(data >> (2*8));
  addr[1] = (bfd_byte)(data >> (1*8));
  addr[0] = (bfd_byte)(data >> (0*8));
#else
  BFD_FAIL();
#endif

}


/* Default implementation */

boolean
DEFUN(bfd_generic_get_section_contents, (abfd, section, location, offset, count),
      bfd *abfd AND
      sec_ptr section AND
      PTR location AND
      file_ptr offset AND
      bfd_size_type count)
{
    if (count == 0)
	return true;
    if ((bfd_size_type)offset >= section->size
	|| bfd_seek(abfd,(file_ptr)( section->filepos + offset), SEEK_SET) == -1
	|| bfd_read(location, (bfd_size_type)1, count, abfd) != count)
	return (false); /* on error */
    return (true);
}
