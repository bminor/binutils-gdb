/* budbg.c -- Interfaces to the generic debugging information routines.
   Copyright (C) 1995 Free Software Foundation, Inc.
   Written by Ian Lance Taylor <ian@cygnus.com>.

   This file is part of GNU Binutils.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#ifndef BUDBG_H
#define BUDBG_H

#include <stdio.h>

/* Routine used to read generic debugging information.  */

extern PTR read_debugging_info PARAMS ((bfd *));

/* Routine used to print generic debugging information.  */

extern boolean print_debugging_info PARAMS ((FILE *, PTR));

/* Routines used to read stabs information.  */

extern PTR start_stab PARAMS ((PTR));

extern boolean finish_stab PARAMS ((PTR, PTR));

extern boolean parse_stab PARAMS ((PTR, PTR, int, int, bfd_vma, const char *));

#endif
