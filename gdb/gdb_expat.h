/* Slightly more portable version of <expat.h>.

   Copyright (C) 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#if !defined(GDB_EXPAT_H)
#define GDB_EXPAT_H

#include <expat.h>

/* Expat 1.95.x does not define these; this is the definition
   recommended by the expat 2.0 headers.  */
#ifndef XML_STATUS_OK
# define XML_STATUS_OK    1
# define XML_STATUS_ERROR 0
#endif

#endif /* !defined(GDB_EXPAT_H) */
