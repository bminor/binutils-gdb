/* Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

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
   This module writes out the final image by reading sections from the
   input files, relocating them and writing them out

   There are two main paths through this module, one for normal
   operation and one for partial linking.

   During  normal operation, raw section data is read along with the
   associated relocation information, the relocation info applied and
   the section data written out on a section by section basis.

   When partially linking, all the relocation records are read to work
   out how big the output relocation vector will be. Then raw data is
   read, relocated and written section by section.

   Written by Steve Chamberlain sac@cygnus.com

*/


#include "bfd.h"
#include "sysdep.h"

#include "ld.h"
#include "ldexp.h"
#include "ldlang.h"
#include "ldwrite.h"
#include "ldmisc.h"
#include "ldsym.h"
#include "ldgram.h"
#include "ldmain.h"
#include "relax.h"

static void
read_relocs (abfd, section, symbols)
     bfd * abfd;
     asection * section;
     asymbol ** symbols;
{
  /* Work out the output section ascociated with this input section */
  asection *output_section = section->output_section;

  bfd_size_type reloc_size = bfd_get_reloc_upper_bound (abfd, section);
  arelent **reloc_vector = (arelent **) ldmalloc (reloc_size);

  if (bfd_canonicalize_reloc (abfd,
			      section,
			      reloc_vector,
			      symbols))
    {
      output_section->reloc_count += section->reloc_count;
    }
}


static void
setup_rel ()
{
  /*
    Run through each section of each file and work work out the total
    number of relocation records which will finally be in each output
    section
    */

  LANG_FOR_EACH_INPUT_SECTION
  (statement, abfd, section,
   (read_relocs (abfd, section, statement->asymbols)));



  /*
    Now run though all the output sections and allocate the space for
    all the relocations
    */
  LANG_FOR_EACH_OUTPUT_SECTION
    (section,
     (section->orelocation =
      (arelent **) ldmalloc ((bfd_size_type) (sizeof (arelent **) *
					      section->reloc_count)),
      section->reloc_count = 0));
}

void
ldwrite ()
{
  PTR data_area = (PTR) ldmalloc (largest_section);

  ldsym_write ();

  if (config.relocateable_output == true)
    setup_rel ();

  write_relax (output_bfd, data_area, config.relocateable_output);

  free (data_area);

  /* Output the symbol table (both globals and locals).  */

  /* Print a map, if requested and possible.  */

  if (config.map_file)
  {
    ldsym_print_symbol_table ();
    lang_map ();
  }
}
