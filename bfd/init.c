/* bfd initialization stuff
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
#include <sysdep.h>
#include "bfd.h"
#include "libbfd.h"

static boolean initialized = false;

/*proto* bfd_init

This routine must be called before any other bfd function to initialize
magical internal data structures.

*; void EXFUN(bfd_init,(void));
*/

void DEFUN_VOID(bfd_init)
{
  if (initialized == false) {
    initialized = true;

    bfd_arch_init();
  }
}


/*proto-internal* bfd_check_init

This routine is called before any other bfd function using initialized
data is used to ensure that the structures have been initialized.
Soon this function will go away, and the bfd library will assume that
bfd_init has been called.

*; void EXFUN(bfd_check_init,(void));
*/

void DEFUN_VOID(bfd_check_init)
{
  if (initialized == false) {
    fprintf(stderr,"The bfd library now requires you to call bfd_init()\n");
    fprintf(stderr,"before any other calls to bfd routines. Please\n");
    fprintf(stderr,"change your source\n");
    bfd_init();
  }
}
