/* ho-mpw.h  Host-specific header file for MPW.
   Copyright (C) 1993 Free Software Foundation, Inc.

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

#ifndef MPW
#define MPW
#endif

#define bzero(a,b) memset(a,0,b)
#define bcopy(a,b,c) memcpy(b,a,c)
#define bcmp(a,b,c) memcmp(a,b,c)

#if 0 /* this doesn't work, sigh */
#ifndef __STDC__
#define __STDC__
#endif
#endif

#define M_ANSI 1

#include <stdlib.h>
#include <string.h>
#include <memory.h>

char *strdup();

#ifdef MPW_C

#undef  __PTR_TO_INT
#define __PTR_TO_INT(P) ((int)(P))
#undef __INT_TO_PTR
#define __INT_TO_PTR(P) ((char *)(P))

#endif

#define sys_nerr	_sys_nerr
#define sys_errlist	_sys_errlist

#ifdef __STDIO__
FILE *mpw_fopen();
#endif

#define fopen mpw_fopen
#define fseek mpw_fseek
#define abort mpw_abort

#define TRUE_FALSE_ALREADY_DEFINED

/* <cr> is an end-of-line marker. */

#define CR_EOL

#define ABORT_ON_ERROR

#define LOSING_COMPILER

/* Arrange for the debugger hook to be checked right when the assembler
   starts up. */

#define HOST_SPECIAL_INIT(argc,argv)  mpw_special_init ((argv)[0]);

#define WANT_FOPEN_BIN

#define HAVE_ERRNO_H

/* end of ho-mpw.h */
