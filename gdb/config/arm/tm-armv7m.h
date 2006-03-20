/* Definitions to target GDB to ARMV7M embedded systems.
   Copyright 2006 CodeSourcery, Inc.

   This file is part of GDB.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef TM_ARMV7M_H
#define TM_ARMV7M_H

/* Include the common embeddedd ARM definitions.  */
#include "arm/tm-embed.h"

/* Tell GDB we are building for ARM V7M.  */
#define ARM_V7M

#endif /* TM_ARMV7M_H */
