/* Native support for GNU/Linux x86.

   Copyright 1986, 1987, 1989, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 1999,
   2000, 2001, 2002, 2003, 2005, 2007, 2008, 2009, 2010
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef NM_LINUX_H
#define NM_LINUX_H

#include "config/nm-linux.h"

#ifdef HAVE_PTRACE_GETFPXREGS
/* Include register set support for the SSE registers.  */
#define FILL_FPXREGSET
#endif

#endif /* nm-linux.h */
