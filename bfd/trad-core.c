/* BFD back end for traditional Unix core files (U-area and raw sections)
   Copyright (C) 1988, 1989, 1991 Free Software Foundation, Inc.
   Written by John Gilmore of Cygnus Support.

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

/* This file does not define a particular back-end, but it defines routines
   that can be used by other back-ends.  */
#include <sysdep.h>
#include "bfd.h"
#include <stdio.h>
#include "libbfd.h"

#include "libaout.h"           /* BFD a.out internal data structures */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <machine/reg.h>

#include <sys/user.h>		/* After a.out.h  */
#include <sys/file.h>

#include <errno.h>

/* need this cast b/c ptr is really void * */
#define core_hdr(bfd) (((struct core_data *) (bfd->tdata))->hdr)
#define core_datasec(bfd) (((struct core_data *) ((bfd)->tdata))->data_section)
#define core_stacksec(bfd) (((struct core_data*)((bfd)->tdata))->stack_section)
#define core_regsec(bfd) (((struct core_data *) ((bfd)->tdata))->reg_section)
#define core_upage(bfd) (((struct core_data *) ((bfd)->tdata))->upage)

/* These are stored in the bfd's tdata */
struct core_data {
  struct user *upage;             /* core file header */
  asection *data_section;
  asection *stack_section;
  asection *reg_section;
};

/* ARGSUSED */
bfd_target *
trad_unix_core_file_p (abfd)
     bfd *abfd;
{
#if HOST_SYS == SUN4_SYS
  return 0;
#else
  int val;
  char *rawptr;
  struct user u;
  unsigned int reg_offset, fp_reg_offset;

  /* 4.2-style (and perhaps also sysV-style) core dump file.  */

  val = bfd_read ((void *)&u, 1, sizeof u, abfd);
  if (val != sizeof u)
    return 0;			/* Too small to be a core file */

  /* Sanity check perhaps??? */
  if (u.u_dsize > 0x1000000)	/* Remember, it's in pages... */
    return 0;
  if (u.u_ssize > 0x1000000)
    return 0;
  /* Check that the size claimed is no greater than the file size. FIXME. */

  /* OK, we believe you.  You're a core file (sure, sure).  */

  /* Allocate both the upage and the struct core_data at once, so
     a single free() will free them both.  */
  rawptr = (char *)zalloc (sizeof (u) + sizeof (struct core_data));
  if (rawptr == NULL) {
    bfd_error = no_memory;
    return 0;
  }
  
  set_tdata (abfd, (struct core_data *)rawptr);
  core_upage (abfd) = (struct user *)(rawptr + sizeof (struct core_data));
  *core_upage (abfd) = u;		/* Save that upage! */

  /* create the sections.  This is raunchy, but bfd_close wants to reclaim
     them */
  core_stacksec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_stacksec (abfd) == NULL) {
loser:
    bfd_error = no_memory;
    free ((void *)rawptr);
    return 0;
  }
  core_datasec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_datasec (abfd) == NULL) {
loser1:
    free ((void *)core_stacksec (abfd));
    goto loser;
  }
  core_regsec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_regsec (abfd) == NULL) {
loser2:
    free ((void *)core_datasec (abfd));
    goto loser1;
  }

  core_stacksec (abfd)->name = ".stack";
  core_datasec (abfd)->name = ".data";
  core_regsec (abfd)->name = ".reg";

  core_stacksec (abfd)->flags = SEC_ALLOC + SEC_LOAD;
  core_datasec (abfd)->flags = SEC_ALLOC + SEC_LOAD;
  core_regsec (abfd)->flags = SEC_ALLOC;

  core_datasec (abfd)->size =  NBPG * u.u_dsize;
  core_stacksec (abfd)->size = NBPG * u.u_ssize;
  core_regsec (abfd)->size = NBPG * UPAGES;  /* Larger than sizeof struct u */

  /* What a hack... we'd like to steal it from the exec file,
     since the upage does not seem to provide it.  FIXME.  */
#ifdef HOST_DATA_START_ADDR
  core_datasec (abfd)->vma = HOST_DATA_START_ADDR;
#else
  core_datasec (abfd)->vma = HOST_TEXT_START_ADDR + (NBPG * u.u_tsize);
#endif
  core_stacksec (abfd)->vma = HOST_STACK_END_ADDR - (NBPG * u.u_ssize);
  core_regsec (abfd)->vma = -1;

  core_datasec (abfd)->filepos = NBPG * UPAGES;
  core_stacksec (abfd)->filepos = (NBPG * UPAGES) + NBPG * u.u_dsize;
  core_regsec (abfd)->filepos = 0;	/* Register segment is the upage */

  /* Align to word at least */
  core_stacksec (abfd)->alignment_power = 2;
  core_datasec (abfd)->alignment_power = 2;
  core_regsec (abfd)->alignment_power = 2;

  abfd->sections = core_stacksec (abfd);
  core_stacksec (abfd)->next = core_datasec (abfd);
  core_datasec (abfd)->next = core_regsec (abfd);
  abfd->section_count = 3;

  return abfd->xvec;
#endif
}

char *
trad_unix_core_file_failing_command (abfd)
     bfd *abfd;
{
  if (*core_upage (abfd)->u_comm)
    return core_upage (abfd)->u_comm;
  else
    return 0;
}

/* ARGSUSED */
int
trad_unix_core_file_failing_signal (ignore_abfd)
     bfd *ignore_abfd;
{
  return -1;		/* FIXME, where is it? */
}

/* ARGSUSED */
boolean
trad_unix_core_file_matches_executable_p  (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  return true;		/* FIXME, We have no way of telling at this point */
}
