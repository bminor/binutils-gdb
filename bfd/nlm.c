/* NLM (NetWare Loadable Module) executable support for BFD.
   Copyright (C) 1993 Free Software Foundation, Inc.

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#define ARCH_SIZE 0
#include "libnlm.h"

boolean
DEFUN (nlm_mkobject, (abfd), bfd * abfd)
{
#if 0	/* nlm_object_p does the nlm_tdata allocation */
  nlm_tdata (abfd) =
    (struct nlm_obj_tdata *) bfd_zalloc (abfd, sizeof (struct nlm_obj_tdata));
  if (nlm_tdata (abfd) == NULL)
    {
      bfd_error = no_memory;
      return (false);
    }
#endif
  /* since everything is done at close time, do we need any initialization? */
  return (true);
}
