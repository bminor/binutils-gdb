/* Copyright (C) 1991 Free Software Foundation, Inc.
   
This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* This bit does the tree decoration when MRI style link scripts are parsed */

/*
  contributed by Steve Chamberlain
  		 sac@cygnus.com

*/

#include "bfd.h"
#include "sysdep.h" 
#include "ldlang.h"
#include "mri.h"
#include "ldexp.h"

void
DEFUN(mri_output_section, (name, vma),
      CONST char *name AND
      etree_type *vma)

{
  lang_output_section_statement_type *os;

  os =  lang_output_section_statement_lookup(name);

  if (os->addr_tree == (etree_type *)NULL) {
    os->addr_tree = vma;
  }

  os->flags = 0;
  os->block_value = 0;
 }

/* if any ABSOLUTE <name> are in the script, only load those files
marked thus */

void DEFUN(mri_only_load,(name), CONST char *name)
 {



}


void
DEFUN(mri_load,(name), 
      CONST char *name)
{

  lang_add_input_file(name, lang_input_file_is_file_enum, (char *)NULL);
}
