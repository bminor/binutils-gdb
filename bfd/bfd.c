/* Generic BFD library interface and support routines.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Written by Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

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

/* $Id$ */

/*proto*
@section @code{typedef bfd}

A BFD is has type @code{bfd}; objects of this type are the cornerstone
of any application using @code{libbfd}. References though the BFD and
to data in the BFD give the entire BFD functionality.

Here is the struct used to define the type @code{bfd}.  This contains
the major data about the file, and contains pointers to the rest of
the data.

*+++

$struct _bfd 
${
  The filename the application opened the BFD with.

$  CONST char *filename;                

A pointer to the target jump table.

$  struct bfd_target *xvec;


To avoid dragging too many header files into every file that
includes @file{bfd.h}, IOSTREAM has been declared as a "char *", and MTIME
as a "long".  Their correct types, to which they are cast when used,
are "FILE *" and "time_t".  

The iostream is the result of an fopen on the filename.

$  char *iostream;

Is the file being cached @xref{File Caching}.

$  boolean cacheable;

Marks whether there was a default target specified when the BFD was
opened. This is used to select what matching algorithm to use to chose
the back end.

$  boolean target_defaulted;

The caching routines use these to maintain a least-recently-used list of
BFDs (@pxref{File Caching}).

$  struct _bfd *lru_prev, *lru_next;

When a file is closed by the caching routines, BFD retains state
information on the file here:

$  file_ptr where;              

and here:

$  boolean opened_once;

$  boolean mtime_set;
File modified time 

$  long mtime;          

Reserved for an unimplemented file locking extension.

$int ifd;

The format which belongs to the BFD.

$  bfd_format format;

The direction the BFD was opened with

$  enum bfd_direction {no_direction = 0,
$                       read_direction = 1,
$                       write_direction = 2,
$                       both_direction = 3} direction;

Format_specific flags

$  flagword flags;              

Currently my_archive is tested before adding origin to anything. I
believe that this can become always an add of origin, with origin set
to 0 for non archive files.  

$  file_ptr origin;             

Remember when output has begun, to stop strange things happening.

$  boolean output_has_begun;

Pointer to linked list of sections

$  struct sec  *sections;

The number of sections 

$  unsigned int section_count;

Stuff only useful for object files:
The start address.

$  bfd_vma start_address;
Used for input and output

$  unsigned int symcount;
Symbol table for output BFD

$  struct symbol_cache_entry  **outsymbols;             

Architecture of object machine, eg m68k 

$  enum bfd_architecture obj_arch;

Particular machine within arch, e.g. 68010

$  unsigned long obj_machine;

Stuff only useful for archives:

$  PTR arelt_data;              
$  struct _bfd *my_archive;     
$  struct _bfd *next;           
$  struct _bfd *archive_head;   
$  boolean has_armap;           

Used by the back end to hold private data.

$  PTR tdata;

Used by the application to hold private data

$  PTR usrdata;

Where all the allocated stuff under this BFD goes (@pxref{Memory Usage}).

$  struct obstack memory;
$};

*---

*/
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"


short _bfd_host_big_endian = 0x0100;
        /* Accessing the above as (*(char*)&_bfd_host_big_endian), will
           return 1 if the host is big-endian, 0 otherwise.
           (assuming that a short is two bytes long!!!  FIXME)
           (See HOST_IS_BIG_ENDIAN_P in bfd.h.)  */

/** Error handling
    o - Most functions return nonzero on success (check doc for
        precise semantics); 0 or NULL on error.
    o - Internal errors are documented by the value of bfd_error.
        If that is system_call_error then check errno.
    o - The easiest way to report this to the user is to use bfd_perror.
*/

bfd_ec bfd_error = no_error;

char *bfd_errmsgs[] = { "No error",
                        "System call error",
                        "Invalid target",
                        "File in wrong format",
                        "Invalid operation",
                        "Memory exhausted",
                        "No symbols",
                        "No relocation info",
                        "No more archived files",
                        "Malformed archive",
                        "Symbol not found",
                        "File format not recognized",
                        "File format is ambiguous",
                        "Section has no contents",
                        "Nonrepresentable section on output",
                        "#<Invalid error code>"
                       };

static 
void 
DEFUN(bfd_nonrepresentable_section,(abfd, name),
         CONST  bfd * CONST abfd AND
         CONST  char * CONST name)
{
  printf("bfd error writing file %s, format %s can't represent section %s\n",
         abfd->filename, 
         abfd->xvec->name,
         name);
  exit(1);
}

bfd_error_vector_type bfd_error_vector = 
  {
  bfd_nonrepresentable_section 
  };

#if  !defined(ANSI_LIBRARIES) && !defined(__STDC__) || HOST_SYS==SUN4_SYS
char *
strerror (code)
     int code;
{
  extern int sys_nerr;
  extern char *sys_errlist[];

  return (((code < 0) || (code >= sys_nerr)) ? "(unknown error)" :
          sys_errlist [code]);
}
#endif /* not ANSI_LIBRARIES */


char *
bfd_errmsg (error_tag)
     bfd_ec error_tag;
{
#ifndef errno
  extern int errno;
#endif
  if (error_tag == system_call_error)
    return strerror (errno);

  if ((((int)error_tag <(int) no_error) ||
       ((int)error_tag > (int)invalid_error_code)))
    error_tag = invalid_error_code;/* sanity check */

  return bfd_errmsgs [(int)error_tag];
}


void bfd_default_error_trap(error_tag)
bfd_ec error_tag;
{
  printf("bfd assert fail (%s)\n", bfd_errmsg(error_tag));
}

void (*bfd_error_trap)() = bfd_default_error_trap;
void (*bfd_error_nonrepresentabltrap)() = bfd_default_error_trap;

void
DEFUN(bfd_perror,(message),
      CONST char *message)
{
  if (bfd_error == system_call_error)
    perror((char *)message);            /* must be system error then... */
  else {
    if (message == NULL || *message == '\0')
      fprintf (stderr, "%s\n", bfd_errmsg (bfd_error));
    else
      fprintf (stderr, "%s: %s\n", message, bfd_errmsg (bfd_error));
  }
}

 
/** Symbols */

/* returns the number of octets of storage required */

unsigned int
get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (abfd->format != bfd_object) {
    bfd_error = invalid_operation;
    return 0;
  }

  return BFD_SEND (abfd, _get_reloc_upper_bound, (abfd, asect));
}

unsigned int
bfd_canonicalize_reloc (abfd, asect, location, symbols)
     bfd *abfd;
     sec_ptr asect;
     arelent **location;
     asymbol **symbols;
{
  if (abfd->format != bfd_object) {
    bfd_error = invalid_operation;
    return 0;
  }

  return BFD_SEND (abfd, _bfd_canonicalize_reloc, (abfd, asect, location, symbols));
}


boolean
bfd_set_file_flags (abfd, flags)
     bfd *abfd;
     flagword flags;
{
  if (abfd->format != bfd_object) {
    bfd_error = wrong_format;
    return false;
  }

  if (bfd_read_p (abfd)) {
    bfd_error = invalid_operation;
    return false;
  }

  if ((flags & bfd_applicable_file_flags (abfd)) != flags) {
    bfd_error = invalid_operation;
    return false;
  }

  bfd_get_file_flags (abfd) = flags;
return true;
}


void
bfd_set_reloc (ignore_abfd, asect, location, count)
     bfd *ignore_abfd;
     sec_ptr asect;
     arelent **location;
     unsigned int count;
{
  asect->orelocation  = location;
  asect->reloc_count = count;
}

void
bfd_assert(file, line)
char *file;
int line;
{
  printf("bfd assertion fail %s:%d\n",file,line);
}


/*proto* bfd_set_start_address

Marks the entry point of an output BFD. Returns @code{true} on
success, @code{false} otherwise.

*; PROTO(boolean, bfd_set_start_address,(bfd *, bfd_vma));
*/

boolean
bfd_set_start_address(abfd, vma)
bfd *abfd;
bfd_vma vma;
{
  abfd->start_address = vma;
  return true;
}


/*proto*  bfd_get_mtime

Return cached file modification time (e.g. as read from archive header
for archive members, or from file system if we have been called
before); else determine modify time, cache it, and return it.  

*; PROTO(long, bfd_get_mtime, (bfd *));

*/

long
bfd_get_mtime (abfd)
     bfd *abfd;
{
  FILE *fp;
  struct stat buf;

  if (abfd->mtime_set)
    return abfd->mtime;

  fp = bfd_cache_lookup (abfd);
  if (0 != fstat (fileno (fp), &buf))
    return 0;

  abfd->mtime_set = true;
  abfd->mtime = buf.st_mtime;
  return abfd->mtime;
}

/*proto* stuff

*+

#define bfd_sizeof_headers(abfd, reloc) \
     BFD_SEND (abfd, _bfd_sizeof_headers, (abfd, reloc))

#define bfd_find_nearest_line(abfd, section, symbols, offset, filename_ptr, func, line_ptr) \
     BFD_SEND (abfd, _bfd_find_nearest_line,  (abfd, section, symbols, offset, filename_ptr, func, line_ptr))

#define bfd_debug_info_start(abfd) \
        BFD_SEND (abfd, _bfd_debug_info_start, (abfd))

#define bfd_debug_info_end(abfd) \
        BFD_SEND (abfd, _bfd_debug_info_end, (abfd))

#define bfd_debug_info_accumulate(abfd, section) \
        BFD_SEND (abfd, _bfd_debug_info_accumulate, (abfd, section))

#define bfd_stat_arch_elt(abfd, stat) \
        BFD_SEND (abfd, _bfd_stat_arch_elt,(abfd, stat))

#define bfd_coff_swap_aux_in(a,e,t,c,i) \
        BFD_SEND (a, _bfd_coff_swap_aux_in, (a,e,t,c,i))

#define bfd_coff_swap_sym_in(a,e,i) \
        BFD_SEND (a, _bfd_coff_swap_sym_in, (a,e,i))

#define bfd_coff_swap_lineno_in(a,e,i) \
        BFD_SEND ( a, _bfd_coff_swap_lineno_in, (a,e,i))
*-

*/






