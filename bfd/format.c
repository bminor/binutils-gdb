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

/*
SECTION
	File formats

	A format is a BFD concept of high level file contents type. The
	formats supported by BFD are: 

	o <<bfd_object>>

	The BFD may contain data, symbols, relocations and debug info.

	o <<bfd_archive>>

	The BFD contains other BFDs and an optional index.

	o <<bfd_core>>

	The BFD contains the result of an executable core dump.


*/

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

/* IMPORT from targets.c.  */
extern CONST size_t bfd_default_vector_entries;

/*
FUNCTION
	bfd_check_format

SYNOPSIS
	boolean bfd_check_format(bfd *abfd, bfd_format format);

DESCRIPTION
	Verify if the file attached to the BFD @var{abfd} is compatible
	with the format @var{format} (i.e., one of <<bfd_object>>,
	<<bfd_archive>> or <<bfd_core>>).

	If the BFD has been set to a specific target before the
	call, only the named target and format combination is
	checked. If the target has not been set, or has been set to
	<<default>>, then all the known target backends is
	interrogated to determine a match.  If the default target
	matches, it is used.  If not, exactly one target must recognize
	the file, or an error results.

	The function returns <<true>> on success, otherwise <<false>>
	with one of the following error codes:  

	o <<invalid_operation>> -
	if <<format>> is not one of <<bfd_object>>, <<bfd_archive>> or
	<<bfd_core>>.

	o <<system_call_error>> -
	if an error occured during a read - even some file mismatches
	can cause system_call_errors.

	o <<file_not_recognised>> -
	none of the backends recognised the file format.

	o <<file_ambiguously_recognized>> -
	more than one backend recognised the file format.
*/

boolean
bfd_check_format (abfd, format)
     bfd *abfd;
     bfd_format format;
{
  return bfd_check_format_matches (abfd, format, NULL);
}

/*
FUNCTION
	bfd_check_format_matches

SYNOPSIS
	boolean bfd_check_format_matches(bfd *abfd, bfd_format format, char ***matching);

DESCRIPTION
	Like <<bfd_check_format>>, except when it returns false with
	<<bfd_errno>> set to <<file_ambiguously_recognized>>.  In that
	case, if @var{matching} is not NULL, it will be filled in with
	a NULL-terminated list of the names of the formats that matched,
	allocated with <<malloc>>.
	Then the user may choose a format and try again.

	When done with the list that @var{matching} points to, the caller
	should free it.  
*/

boolean
bfd_check_format_matches (abfd, format, matching)
     bfd *abfd;
     bfd_format format;
     char ***matching;
{
  bfd_target **target, *save_targ, *right_targ;
  char **matching_vector;
  int match_count;

  if (!bfd_read_p (abfd) ||
      ((int)(abfd->format) < (int)bfd_unknown) ||
      ((int)(abfd->format) >= (int)bfd_type_end)) {
    bfd_error = invalid_operation;
    return false;
  }

  if (abfd->format != bfd_unknown)
    return (abfd->format == format)? true: false;


  /* Since the target type was defaulted, check them 
     all in the hope that one will be uniquely recognized.  */

  save_targ = abfd->xvec;
  match_count = 0;
  if (matching)
    {
      *matching = matching_vector =
	bfd_xmalloc_by_size_t (sizeof (char *) *
			       (bfd_default_vector_entries + 1));
      matching_vector[0] = NULL;
    }
  right_targ = 0;


  /* presume the answer is yes */
  abfd->format = format;

  /* If the target type was explicitly specified, just check that target.  */

  if (!abfd->target_defaulted) {
    bfd_seek (abfd, (file_ptr)0, SEEK_SET);	/* rewind! */

    right_targ = BFD_SEND_FMT (abfd, _bfd_check_format, (abfd));
    if (right_targ) {
      abfd->xvec = right_targ;		/* Set the target as returned */
      if (matching)
	free (matching_vector);
      return true;			/* File position has moved, BTW */
    }
  }

  for (target = target_vector; *target != NULL; target++) {
    bfd_target *temp;

    abfd->xvec = *target;	/* Change BFD's target temporarily */
    bfd_seek (abfd, (file_ptr)0, SEEK_SET);
    /* If _bfd_check_format neglects to set bfd_error, assume wrong_format.
       We didn't used to even pay any attention to bfd_error, so I suspect
       that some _bfd_check_format might have this problem.  */
    bfd_error = wrong_format;
    temp = BFD_SEND_FMT (abfd, _bfd_check_format, (abfd));
    if (temp) {				/* This format checks out as ok! */
      right_targ = temp;
      match_count++;
      if (matching)
	{
	  matching_vector[match_count] = temp->name;
	  matching_vector[match_count] = NULL;
	}
      /* If this is the default target, accept it, even if other targets
	 might match.  People who want those other targets have to set 
	 the GNUTARGET variable.  */
      if (temp == default_vector[0])
	{
	  match_count = 1;
	  if (matching)
	    {
	      matching_vector[0] = temp->name;
	      matching_vector[1] = NULL;
	    }
	  break;
	}
#ifdef GNU960
      /* Big- and little-endian b.out archives look the same, but it doesn't
       * matter: there is no difference in their headers, and member file byte
       * orders will (I hope) be handled appropriately by bfd.  Ditto for big
       * and little coff archives.  And the 4 coff/b.out object formats are
       * unambiguous.  So accept the first match we find.
       */
      break;
#endif
    } else if (bfd_error != wrong_format) {
      abfd->xvec = save_targ;
      abfd->format = bfd_unknown;
      if (matching && bfd_error != file_ambiguously_recognized)
	free (matching_vector);
      return false;
    }
  }

  if (match_count == 1) {
    abfd->xvec = right_targ;		/* Change BFD's target permanently */
    if (matching)
      free (matching_vector);
    return true;			/* File position has moved, BTW */
  }

  abfd->xvec = save_targ;		/* Restore original target type */
  abfd->format = bfd_unknown;		/* Restore original format */
  if (match_count == 0)
    {
      bfd_error = file_not_recognized;
      if (matching)
	free (matching_vector);
    }
  else
    bfd_error = file_ambiguously_recognized;
  return false;
}

/*
FUNCTION
	bfd_set_format

SYNOPSIS
	boolean bfd_set_format(bfd *abfd, bfd_format format);

DESCRIPTION
	This function sets the file format of the BFD @var{abfd} to the
	format @var{format}. If the target set in the BFD does not
	support the format requested, the format is invalid, or the BFD
	is not open for writing, then an error occurs.

*/

boolean
bfd_set_format (abfd, format)
     bfd *abfd;
     bfd_format format;
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


/*
FUNCTION
	bfd_format_string

SYNOPSIS
	CONST char *bfd_format_string(bfd_format format);

DESCRIPTION
	Return a pointer to a const string
	<<invalid>>, <<object>>, <<archive>>, <<core>>, or <<unknown>>,
	depending upon the value of @var{format}.
*/

CONST char *
bfd_format_string (format)
     bfd_format format;
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
