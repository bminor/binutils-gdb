/* bfd howto manager.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Written by Steve Chamberlain of Cygnus Support.

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

/* The howto manager 


When an application wants to create a relocation, but doesn't know
what the target machine might call it, it can find out by using this
bit of code.

*/

#include <sysdep.h>
#include <bfd.h>
#include "libbfd.h"
/*proto* bfd_reloc_code_enum_type

*+++

$typedef enum 
${

16 bits wide, simple reloc 

$  BFD_RELOC_16,	

8 bits wide, but used to form an address like 0xffnn

$  BFD_RELOC_8_FFnn,

8 bits wide, simple

$  BFD_RELOC_8,

8 bits wide, pc relative

$  BFD_RELOC_8_PCREL
$ } bfd_reloc_code_enum_real_type;

*---

*/



/*proto* bfd_reloc_type_lookup
This routine returns a pointer to a howto struct which when invoked,
will perform the supplied relocation on data from the architecture
noted.

[Note] This function will go away.

*; PROTO(struct reloc_howto_struct *,
	bfd_reloc_type_lookup,
	(enum bfd_architecture arch, bfd_reloc_code_enum_type code));
*/


struct reloc_howto_struct *
DEFUN(bfd_reloc_type_lookup,(arch, code),
	enum bfd_architecture arch AND
	bfd_reloc_code_enum_type code)
{
  return arch_functions(arch,0)->reloc_type_lookup(code);
}



