/* te-ppcnw.h -- Power PC running Netware environment declarations.
   Copyright (C) 1994 Free Software Foundation, Inc.
   
   This file is part of GAS, the GNU Assembler.
   
   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Added these, because if we don't know what we're targetting we may
   need an assembler version of libgcc, and that will use local
   labels.  */
#define LOCAL_LABELS_DOLLAR
#define LOCAL_LABELS_FB

/* these define interfaces */
#include "obj-format.h"

/* gcc uses escape sequences for ppc/netware */

#undef NO_STRING_ESCAPES
