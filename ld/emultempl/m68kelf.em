# This shell script emits a C file. -*- C -*-
#   Copyright (C) 2000 Free Software Foundation, Inc.
#   Written by Michael Sokolov <msokolov@ivan.Harhan.ORG>, based on armelf.em
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

# This file is sourced from elf32.em, and defines extra m68k ELF
# specific routines.
#
cat >>e${EMULATION_NAME}.c <<EOF

static void m68k_elf_after_open PARAMS((void));
static void check_sections PARAMS ((bfd *, asection *, PTR));
static void m68k_elf_after_allocation PARAMS ((void));

/* This function is run after all the input files have been opened.
   We create a .emreloc section for each input file with a non zero
   .data section.  The BFD backend will fill in these sections with
   magic numbers which can be used to relocate the data section at run
   time.  */

static void
m68k_elf_after_open ()
{
  bfd *abfd;

  /* Call the standard elf routine.  */
  gld${EMULATION_NAME}_after_open ();

  if (! command_line.embedded_relocs
      || link_info.relocateable)
    return;

  for (abfd = link_info.input_bfds; abfd != NULL; abfd = abfd->link_next)
    {
      asection *datasec;

      /* As first-order business, make sure that each input BFD is ELF. It
         better be, as we are directly calling a ELF backend function.  */
      if (bfd_get_flavour (abfd) != bfd_target_elf_flavour)
	einfo ("%F%B: all input objects must be ELF for --embedded-relocs\n");

      datasec = bfd_get_section_by_name (abfd, ".data");

      /* Note that we assume that the reloc_count field has already
         been set up.  We could call bfd_get_reloc_upper_bound, but
         that returns the size of a memory buffer rather than a reloc
         count.  We do not want to call bfd_canonicalize_reloc,
         because although it would always work it would force us to
         read in the relocs into BFD canonical form, which would waste
         a significant amount of time and memory.  */
      if (datasec != NULL && datasec->reloc_count > 0)
	{
	  asection *relsec;

	  relsec = bfd_make_section (abfd, ".emreloc");
	  if (relsec == NULL
	      || ! bfd_set_section_flags (abfd, relsec,
					  (SEC_ALLOC
					   | SEC_LOAD
					   | SEC_HAS_CONTENTS
					   | SEC_IN_MEMORY))
	      || ! bfd_set_section_alignment (abfd, relsec, 2)
	      || ! bfd_set_section_size (abfd, relsec,
					 datasec->reloc_count * 12))
	    einfo ("%F%B: can not create .emreloc section: %E\n");
	}

      /* Double check that all other data sections are empty, as is
         required for embedded PIC code.  */
      bfd_map_over_sections (abfd, check_sections, (PTR) datasec);
    }
}

/* Check that of the data sections, only the .data section has
   relocs.  This is called via bfd_map_over_sections.  */

static void
check_sections (abfd, sec, datasec)
     bfd *abfd;
     asection *sec;
     PTR datasec;
{
  if ((bfd_get_section_flags (abfd, sec) & SEC_DATA)
      && sec != (asection *) datasec
      && sec->reloc_count != 0)
    einfo ("%B%X: section %s has relocs; can not use --embedded-relocs\n",
	   abfd, bfd_get_section_name (abfd, sec));
}

/* This function is called after the section sizes and offsets have
   been set.  If we are generating embedded relocs, it calls a special
   BFD backend routine to do the work.  */

static void
m68k_elf_after_allocation ()
{
  bfd *abfd;

  /* Call the standard elf routine.  */
  gld${EMULATION_NAME}_before_allocation ();

  if (! command_line.embedded_relocs
      || link_info.relocateable)
    return;

  for (abfd = link_info.input_bfds; abfd != NULL; abfd = abfd->link_next)
    {
      asection *datasec, *relsec;
      char *errmsg;

      datasec = bfd_get_section_by_name (abfd, ".data");

      if (datasec == NULL || datasec->reloc_count == 0)
	continue;

      relsec = bfd_get_section_by_name (abfd, ".emreloc");
      ASSERT (relsec != NULL);

      if (! bfd_m68k_elf32_create_embedded_relocs (abfd, &link_info,
						   datasec, relsec,
						   &errmsg))
	{
	  if (errmsg == NULL)
	    einfo ("%B%X: can not create runtime reloc information: %E\n",
		   abfd);
	  else
	    einfo ("%X%B: can not create runtime reloc information: %s\n",
		   abfd, errmsg);
	}
    }
}

EOF

# We have our own after_open and after_allocation functions, but they call
# the standard routines, so give them a different name.
LDEMUL_AFTER_OPEN=m68k_elf_after_open
LDEMUL_AFTER_ALLOCATION=m68k_elf_after_allocation
