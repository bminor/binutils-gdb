/* SOM object file format.
   Copyright (C) 1993 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2,
   or (at your option) any later version.

   GAS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with GAS; see the file COPYING.  If not, write
   to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

   Written by the Center for Software Science at the University of Utah
   and by Cygnus Support.  */

#include "as.h"
#include "subsegs.h"
#include "aout/stab_gnu.h"
#include "obstack.h"

/* SOM does not need any pseudo-ops.  */

const pseudo_typeS obj_pseudo_table[] =
{
  {NULL}
};

/* SOM does not use this.  */

void
obj_read_begin_hook ()
{
}

/* Handle a .version directive.  FIXME.  We just parse the .version
   directive and throw away the results!.  */

void
obj_som_version (unused)
     int unused;
{
  SKIP_WHITESPACE ()
  if (*input_line_pointer == '\"')
    {
      ++input_line_pointer;
      while (is_a_char (next_char_of_string ()))
	;
    }
  else
    {
      as_bad ("Expected quoted string");
    }
  demand_empty_rest_of_line ();
}

/* Perform any initialization necessary for stabs support.

   For SOM we need to create the space which will contain the
   two stabs subspaces.  Additionally we need to set up the
   space/subspace relationships and set space/subspace attributes
   which BFD does not understand.  */

void
obj_som_init_stab_section (seg)
     segT seg;
{
  segT saved_seg = now_seg, space;
  subsegT saved_subseg = now_subseg;

  /* Make the space which will contain the debug subspaces.  */
  space = bfd_make_section_old_way (stdoutput, "$GDB_DEBUG$");

  /* Set SOM specific attributes for the space.  In particular we set
     the space "defined", "private", "sort_key", and "spnum" values.  */
  obj_set_section_attributes (space, 1, 1, 255, 2);

  /* Set the containing space for both stab sections to be $GDB_DEBUG$
     (just created above).  Also set some attributes which BFD does
     not understand.  In particular, access bits, sort keys, and load
     quadrant.  */
  obj_set_subsection_attributes (seg, space, 0x1f, 255, 0);

  /* Likewise for the $GDB_STRINGS$ subspace.  Note the section
     hasn't been created at the time of this call, so we create
     it now.  */
  seg = subseg_new ("$GDB_STRINGS$", 0);
  obj_set_subsection_attributes (seg, space, 0x1f, 254, 0);

  subseg_set (saved_seg, saved_subseg);
}
