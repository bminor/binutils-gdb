/* ho-hppaosf.h  OSF/1 host-specific header file.
   Copyright 1987, 1991, 1992 Free Software Foundation, Inc.
   
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

/* HP PA-RISC and OSF/1 support was contributed by the Center for
   Software Science at the University of Utah.  */

#define M_HPPAOSF 1

#ifdef __STDC__ != 1
#define NO_STDARG
#endif /* not ansi */

#define HAVE_STRERROR
extern int errno;

#include <ctype.h>

/* SKV 12/08/92. Include correctly configured declaration of alloca. */

#include "../libiberty/alloca-conf.h"

/* end of ho-hppaosf.h */
