/* BFD back end for Lynx core files
   Copyright 1993 Free Software Foundation, Inc.
   Written by Stu Grossman of Cygnus Support.

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

#ifdef HOST_LYNX		/* Core files only work locally for now */

/* These are stored in the bfd's tdata */

struct lynx_core_struct 
{
  int sig;
  char cmd[PNMLEN + 1];
};

#define core_hdr(bfd) ((bfd)->tdata.lynx_core_data)
#define core_signal(bfd) (core_hdr(bfd)->sig)
#define core_command(bfd) (core_hdr(bfd)->cmd)

/* Handle Lynx core dump file.  */

static asection *
make_bfd_asection (abfd, name, flags, _raw_size, vma, filepos)
     bfd *abfd;
     CONST char *name;
     flagword flags;
     bfd_size_type _raw_size;
     bfd_vma vma;
     file_ptr filepos;
{
  asection *asect;
  char *newname;

  newname = bfd_alloc (abfd, strlen (name) + 1);
  if (!newname)
    return NULL;

  strcpy (newname, name);

  asect = bfd_make_section (abfd, newname);
  if (!asect)
    return NULL;

  asect->flags = flags;
  asect->_raw_size = _raw_size;
  asect->vma = vma;
  asect->filepos = filepos;
  asect->alignment_power = 2;

  return asect;
}

/* ARGSUSED */
bfd_target *
lynx_core_file_p (abfd)
     bfd *abfd;
{
  int val;
  int secnum;
  struct pssentry pss;
  size_t tcontext_size;
  core_st_t *threadp;
  int pagesize;
  asection *newsect;

  pagesize = getpagesize ();	/* Serious cross-target issue here...  This
				   really needs to come from a system-specific
				   header file.  */

  /* Get the pss entry from the core file */

  bfd_seek (abfd, 0, SEEK_SET);

  val = bfd_read ((void *)&pss, 1, sizeof pss, abfd);
  if (val != sizeof pss)
    {
      /* Too small to be a core file */
      bfd_error = wrong_format;
      return NULL;
  }

  core_hdr (abfd) = (struct lynx_core_struct *)
    bfd_zalloc (abfd, sizeof (struct lynx_core_struct));

  if (!core_hdr (abfd))
    {
      bfd_error = no_memory;
      return NULL;
    }

  strncpy (core_command (abfd), pss.pname, PNMLEN + 1);

  /* Compute the size of the thread contexts */

  tcontext_size = pss.threadcnt * sizeof (core_st_t);

  /* Allocate space for the thread contexts */

  threadp = (core_st_t *)bfd_zalloc (abfd, tcontext_size);
  if (!threadp)
    {
      bfd_error = no_memory;
      return NULL;
    }

  /* Save thread contexts */

  bfd_seek (abfd, pagesize, SEEK_SET);

  val = bfd_read ((void *)threadp, pss.threadcnt, sizeof (core_st_t), abfd);

  if (val != tcontext_size)
    {
      /* Probably too small to be a core file */
      bfd_error = wrong_format;
      return NULL;
    }
  
  core_signal (abfd) = threadp->currsig;

  newsect = make_bfd_asection (abfd, ".stack",
			       SEC_ALLOC + SEC_LOAD + SEC_HAS_CONTENTS,
			       pss.ssize,
			       pss.slimit,
			       pagesize + tcontext_size);
  if (!newsect)
    {
      bfd_error = no_memory;
      return NULL;
    }

  newsect = make_bfd_asection (abfd, ".data",
			       SEC_ALLOC + SEC_LOAD + SEC_HAS_CONTENTS,
			       pss.data_len + pss.bss_len,
			       pss.data_start,
			       pagesize + tcontext_size + pss.ssize);
  if (!newsect)
    {
      bfd_error = no_memory;
      return NULL;
    }

  for (secnum = 0; secnum < pss.threadcnt; secnum++)
    {
      char secname[100];

      sprintf (secname, ".reg%d", threadp[secnum].tid);
      newsect = make_bfd_asection (abfd, secname,
				   SEC_ALLOC + SEC_HAS_CONTENTS,
				   sizeof (core_st_t),
				   0,
				   pagesize + secnum * sizeof (core_st_t));
      if (!newsect)
	{
	  bfd_error = no_memory;
	  return NULL;
	}
    }

  return abfd->xvec;
}

char *
lynx_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_command (abfd);
}

/* ARGSUSED */
int
lynx_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_signal (abfd);
}

/* ARGSUSED */
boolean
lynx_core_file_matches_executable_p  (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  return true;		/* FIXME, We have no way of telling at this point */
}

#endif /* HOST_LYNX */
