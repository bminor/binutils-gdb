# This shell script emits a C file. -*- C -*-
#   Copyright 2001 Free Software Foundation, Inc.
#
# This file is part of GLD, the Gnu Linker.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

# This file is sourced from elf32.em and mmo.em, used to define
# MMIX-specific things common to ELF and MMO.

cat >>e${EMULATION_NAME}.c <<EOF
#include "elf/mmix.h"

static void mmix_set_reg_section_vma PARAMS ((void));

/* We need to set the VMA of the .MMIX.reg_contents section when it has
   been allocated.  */

static void
mmix_set_reg_section_vma ()
{
  asection *sec
    = bfd_get_section_by_name (output_bfd, MMIX_REG_CONTENTS_SECTION_NAME);
  bfd_vma regvma;

  /* If there's no register section, we don't need to do anything.  */
  if (sec == NULL)
    return;

  regvma = 256 * 8 - sec->_raw_size - 8;

  /* If we start on a local register, we have too many global registers.  */
  if (regvma < 32 * 8)
    einfo ("%X%P: Too many global registers: %u, max 223\n",
	   (unsigned) sec->_raw_size / 8);

  /* Set vma to correspond to first such register number * 8.  */
  bfd_set_section_vma (output_bfd, sec, regvma);

  /* ??? Why isn't the section size (_cooked_size) set?  Doesn't it get
     set regardless of presence of relocations?  */
  if (sec->_cooked_size == 0
      && ! bfd_set_section_size (output_bfd, sec, sec->_raw_size))
    {
      einfo ("%X%P: Can't set section %s size to %u\n",
	     MMIX_REG_CONTENTS_SECTION_NAME, (unsigned) sec->_raw_size);
      return;
    }

  /* Simplify symbol output for the register section (without contents;
     created for register symbols) by setting the output offset to 0.  */
  sec = bfd_get_section_by_name (output_bfd, MMIX_REG_SECTION_NAME);
  if (sec == NULL)
    return;

  bfd_set_section_vma (abfd, sec, 0);

}
EOF

LDEMUL_AFTER_ALLOCATION=mmix_set_reg_section_vma
