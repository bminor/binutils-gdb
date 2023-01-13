# This shell script emits C code -*- C -*-
# to keep track of the machine type of Z80 object files
# It does some substitutions.
#   Copyright (C) 2005-2020 Free Software Foundation, Inc.
# This file is part of the GNU Binutils.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
# MA 02110-1301, USA.

fragment <<EOF
/* --- \begin{z80.em} */

#include "elf/z80.h"

static void
gld${EMULATION_NAME}_after_open (void);

static int result_mach_type;

struct z80_mach_info
{
  unsigned eflags;
  unsigned bfd_mach;
  const int *compat; /* back compatible machines */
};

static const int
back_compat_z80[] = {bfd_mach_z80, -1};

static const int
back_compat_z180[] = {bfd_mach_z180, bfd_mach_z80, -1};

static const int
back_compat_ez80[] = {bfd_mach_ez80_z80, bfd_mach_z180, bfd_mach_z80, -1};

static const struct z80_mach_info
z80_mach_info[] =
{
  { EF_Z80_MACH_Z80,      bfd_mach_z80,       NULL },
  { EF_Z80_MACH_Z80,      bfd_mach_z80strict, back_compat_z80 },
  { EF_Z80_MACH_Z80,      bfd_mach_z80full,   back_compat_z80 },
  { EF_Z80_MACH_Z180,     bfd_mach_z180,      back_compat_z80 },
  { EF_Z80_MACH_EZ80_Z80, bfd_mach_ez80_z80,  back_compat_z180 },
  { EF_Z80_MACH_EZ80_ADL, bfd_mach_ez80_adl,  back_compat_ez80 },
  { EF_Z80_MACH_Z80N,     bfd_mach_z80n,      back_compat_z80 },
  { EF_Z80_MACH_GBZ80,    bfd_mach_gbz80,     NULL },
  { EF_Z80_MACH_R800,     bfd_mach_r800,      back_compat_z80 }
};
/*
static const struct z80_mach_info *
z80_mach_info_by_eflags (unsigned int eflags)
{
  const struct z80_mach_info *p;
  const struct z80_mach_info *e;

  eflags &= EF_Z80_MACH_MSK;
  p = &z80_mach_info[0];
  e = &z80_mach_info[sizeof(z80_mach_info)/sizeof(*z80_mach_info)];
  for (; p != e; ++p)
    if (eflags == p->eflags)
      return p;
  return NULL;
}*/

static const struct z80_mach_info *
z80_mach_info_by_mach (unsigned int bfd_mach)
{
  const struct z80_mach_info *p;
  const struct z80_mach_info *e;

  p = &z80_mach_info[0];
  e = &z80_mach_info[sizeof(z80_mach_info)/sizeof(*z80_mach_info)];
  for (; p != e; ++p)
    if (bfd_mach == p->bfd_mach)
      return p;
  return NULL;
}

static const struct z80_mach_info *
z80_combine_mach (const struct z80_mach_info *m1,
		  const struct z80_mach_info *m2)
{
  int i;
  int mach;
  if (m1->compat != NULL)
    for (i = 0; (mach = m1->compat[i]) >= 0; ++i)
      if ((unsigned)mach == m2->bfd_mach)
	return m1;
  if (m2->compat != NULL)
    for (i = 0; (mach = m2->compat[i]) >= 0; ++i)
      if ((unsigned)mach == m1->bfd_mach)
	return m2;
  /* incompatible mach */
  return NULL;
}

/* Set the machine type of the output file based on result_mach_type.  */
static void
z80_after_open (void)
{
  const struct z80_mach_info *mach = NULL;
  bfd *abfd;

  /* For now, make sure all object files are of the same architecture.
     We may try to merge object files with different architecture together.  */
  for (abfd = link_info.input_bfds; abfd != NULL; abfd = abfd->link.next)
    {
      const struct z80_mach_info *new_mach;
      /*new_mach = z80_mach_info_by_eflags (elf_elfheader (abfd)->e_flags);*/
      new_mach = z80_mach_info_by_mach(bfd_get_mach (abfd));
      if (mach == NULL)
	mach = new_mach;
      else if (mach != new_mach)
	mach = z80_combine_mach (mach, new_mach);
      if (mach == NULL)
	einfo (_("%F%P: %pB: Instruction sets of object files incompatible\n"),
	       abfd);
    }
  if (mach != NULL)
    {
      bfd_set_arch_mach (link_info.output_bfd, bfd_arch_z80, mach->bfd_mach);
      result_mach_type = mach->bfd_mach;
    }
  else
    einfo (_("%F%P: %pB: Unknown machine type\n"),
	   abfd);

  /* Call the standard elf routine.  */
  gld${EMULATION_NAME}_after_open ();
}

#ifndef TARGET_IS_elf32z80
static void
gld${EMULATION_NAME}_after_open (void)
{
}
#endif

/* --- \end{z80.em} */
EOF

LDEMUL_AFTER_OPEN=z80_after_open
