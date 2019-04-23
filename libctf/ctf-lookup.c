/* Type lookup.
   Copyright (C) 2019 Free Software Foundation, Inc.

   This file is part of libctf.

   libctf is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not see
   <http://www.gnu.org/licenses/>.  */

#include <ctf-impl.h>
#include <elf.h>
#include <string.h>

/* Return the pointer to the internal CTF type data corresponding to the
   given type ID.  If the ID is invalid, the function returns NULL.
   This function is not exported outside of the library.  */

const ctf_type_t *
ctf_lookup_by_id (ctf_file_t **fpp, ctf_id_t type)
{
  ctf_file_t *fp = *fpp;	/* Caller passes in starting CTF container.  */
  ctf_id_t idx;

  if ((fp->ctf_flags & LCTF_CHILD) && LCTF_TYPE_ISPARENT (fp, type)
      && (fp = fp->ctf_parent) == NULL)
    {
      (void) ctf_set_errno (*fpp, ECTF_NOPARENT);
      return NULL;
    }

  idx = LCTF_TYPE_TO_INDEX (fp, type);
  if (idx > 0 && (unsigned long) idx <= fp->ctf_typemax)
    {
      *fpp = fp;		/* Function returns ending CTF container.  */
      return (LCTF_INDEX_TO_TYPEPTR (fp, idx));
    }

  /* If this container is writable, check for a dynamic type.  */

  if (fp->ctf_flags & LCTF_RDWR)
    {
      ctf_dtdef_t *dtd;

      if ((dtd = ctf_dynamic_type (fp, type)) != NULL)
	{
	  *fpp = fp;
	  return &dtd->dtd_data;
	}
    }
  (void) ctf_set_errno (*fpp, ECTF_BADID);
  return NULL;
}

