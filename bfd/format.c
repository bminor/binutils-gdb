/* Generic BFD support for file formats.
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

/*doc*
@section File Formats
A format is a BFD concept of high level file contents. The
formats supported by BFD are:
@table @code
@item bfd_object
The BFD may contain data, symbols, relocations and debug info.
@item bfd_archive
The BFD contains other BFDs and an optional index.
@item bfd_core
The BFD contains the result of an executable core dump.
@end table
*/
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"


extern bfd_target *target_vector[];
extern bfd_target *default_vector[];


/*proto*
*i bfd_check_format
This routine is supplied a BFD and a format. It attempts to verify if
the file attatched to the BFD is indeed compatible with the format
specified (ie, one of @code{bfd_object}, @code{bfd_archive} or
@code{bfd_core}).

If the BFD has been set to a specific @var{target} before the call,
only the named target and format combination will be checked. If the
target has not been set, or has been set to @code{default} then all
the known target backends will be interrogated to determine a match.

The function returns @code{true} on success, otherwise @code{false}
with one of the following error codes: 
@table @code
@item 
invalid_operation
if @code{format} is not one of @code{bfd_object}, @code{bfd_archive}
or @code{bfd_core}.
@item system_call_error
if an error occured during a read -  even some file mismatches can
cause system_call_errros
@item file_not_recognised
none of the backends recognised the file format
@item file_ambiguously_recognized
more than one backend recognised the file format.
@end table
*; PROTO(boolean, bfd_check_format, (bfd *abfd, bfd_format format));
*-*/

boolean
DEFUN(bfd_check_format,(abfd, format),
      bfd *abfd AND
      bfd_format format)
{
  bfd_target **target, *save_targ, *right_targ;
  int match_count;

  if (!bfd_read_p (abfd) ||
      ((int)(abfd->format) < (int)bfd_unknown) ||
      ((int)(abfd->format) >= (int)bfd_type_end)) {
    bfd_error = invalid_operation;
    return false;
  }

  if (abfd->format != bfd_unknown)
    return (abfd->format == format)? true: false;

  /* presume the answer is yes */
  abfd->format = format;

  /* If the target type was explicitly specified, just check that target.  */

  if (!abfd->target_defaulted) {
    bfd_seek (abfd, (file_ptr)0, SEEK_SET);	/* rewind! */

    right_targ = BFD_SEND_FMT (abfd, _bfd_check_format, (abfd));
    if (right_targ) {
      abfd->xvec = right_targ;		/* Set the target as returned */
      return true;			/* File position has moved, BTW */
    }
    abfd->format = bfd_unknown;
    return false;			/* Specified target is not right */
  }

  /* Since the target type was defaulted, check them 
     all in the hope that one will be uniquely recognized.  */

  save_targ = abfd->xvec;
  match_count = 0;
  right_targ = 0;

  for (target = target_vector; *target != NULL; target++) {
    bfd_target *temp;

    abfd->xvec = *target;	/* Change BFD's target temporarily */
    bfd_seek (abfd, (file_ptr)0, SEEK_SET);
    temp = BFD_SEND_FMT (abfd, _bfd_check_format, (abfd));
    if (temp) {				/* This format checks out as ok! */
      right_targ = temp;
      match_count++;
      /* If this is the default target, accept it, even if other targets
	 might match.  People who want those other targets have to set 
	 the GNUTARGET variable.  */
      if (temp == default_vector[0])
	break;
#ifdef GNU960
      /* Big- and little-endian b.out archives look the same, but it doesn't
       * matter: there is no difference in their headers, and member file byte
       * orders will (I hope) be handled appropriately by bfd.  Ditto for big
       * and little coff archives.  And the 4 coff/b.out object formats are
       * unambiguous.  So accept the first match we find.
       */
      break;
#endif
    }
  }

  if (match_count == 1) {
    abfd->xvec = right_targ;		/* Change BFD's target permanently */
    return true;			/* File position has moved, BTW */
  }

  abfd->xvec = save_targ;		/* Restore original target type */
  abfd->format = bfd_unknown;		/* Restore original format */
  bfd_error = ((match_count == 0) ? file_not_recognized :
	       file_ambiguously_recognized);
  return false;
}
/*proto*
*i bfd_set_format
This function sets the file format of the supplied BFD to the format
requested. If the target set in the BFD does not support the format
requested, the format is illegal or the BFD is not open for writing
than an error occurs.
*; PROTO(boolean,bfd_set_format,(bfd *, bfd_format));
*-*/
boolean
DEFUN(bfd_set_format,(abfd, format),
      bfd *abfd AND
      bfd_format format)
{

  if (bfd_read_p (abfd) ||
      ((int)abfd->format < (int)bfd_unknown) ||
      ((int)abfd->format >= (int)bfd_type_end)) {
    bfd_error = invalid_operation;
    return false;
  }

  if (abfd->format != bfd_unknown)
    return (abfd->format == format) ? true:false;

  /* presume the answer is yes */
  abfd->format = format;

  if (!BFD_SEND_FMT (abfd, _bfd_set_format, (abfd))) {
    abfd->format = bfd_unknown;
    return false;
  }

  return true;
}


/*proto*
*i bfd_format_string
This function takes one argument, and enumerated type (bfd_format) and
returns a pointer to a const string "invalid", "object", "archive",
"core" or "unknown" depending upon the value of the enumeration.
*; PROTO(CONST char *, bfd_format_string, (bfd_format));
*-*/

CONST char *
DEFUN(bfd_format_string,(format),
     bfd_format format)
{
  if (((int)format <(int) bfd_unknown) 
      || ((int)format >=(int) bfd_type_end)) 
    return "invalid";
  
  switch (format) {
  case bfd_object:
    return "object";		/* linker/assember/compiler output */
  case bfd_archive: 
    return "archive";		/* object archive file */
  case bfd_core: 
    return "core";		/* core dump */
  default: 
    return "unknown";
  }
}
