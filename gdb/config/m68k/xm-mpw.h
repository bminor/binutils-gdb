/* Macro definitions for running GDB on Apple Macintoshes.
   Copyright (C) 1994 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "m68k/xm-m68k.h"

#include "fopen-bin.h"

#define CANT_FORK

/* MPW C is basically ANSI, but doesn't actually enable __STDC__,
   nor does it allow __STDC__ to be #defined. */

#ifndef ALMOST_STDC
#define ALMOST_STDC
#endif

#ifdef BFD_TRUE_FALSE
#ifndef false
#define false mpw_false
#endif
#ifndef true
#define true mpw_true
#endif
#endif

#ifdef MPW_C

#undef  __PTR_TO_INT
#define __PTR_TO_INT(P) ((int)(P))
#undef __INT_TO_PTR
#define __INT_TO_PTR(P) ((char *)(P))

#endif

#include <stdio.h>

FILE *mpw_fopen ();
int mpw_fseek ();
void mpw_abort ();

/* Map these standard functions to improved versions in libiberty. */

#define fopen mpw_fopen
#define open mpw_open
#define fseek mpw_fseek
#define abort mpw_abort

/* Map these standard functions to versions that can do I/O in a console
   window. */

#define printf hacked_printf
#define fprintf hacked_fprintf
#define vprintf hacked_vfprintf
#define fputs hacked_fputs
#define fputc hacked_fputc
#undef putc
#define putc hacked_putc
#define fflush hacked_fflush

#define fgetc hacked_fgetc

/* Define as macros so as to mask the previous enum. */

#ifndef BFD_TRUE_FALSE
#ifndef false
#define false ffalse
#endif
#ifndef true
#define true ttrue
#endif
#endif

#ifndef TRUE_FALSE_ALREADY_DEFINED
#define TRUE_FALSE_ALREADY_DEFINED
#endif

#define POSIX_UTIME

/* No declaration of strdup in MPW's string.h, oddly enough. */

char *strdup (char *s1);

/* '.' indicates drivers on the Mac, so we need a different filename. */

#define GDBINIT_FILENAME "_gdbinit"

/* Commas are more common to separate dirnames in a path on Macs. */

#define DIRNAME_SEPARATOR ','

/* This is a real crufty hack. */

#define HAVE_TERMIO

/* Addons to the basic MPW-supported signal list. */

#ifndef SIGQUIT
#define SIGQUIT (1<<6)
#endif
#ifndef SIGHUP
#define SIGHUP (1<<7)
#endif

/* If __STDC__ is on, then this definition will be missing. */

#ifndef fileno
#define fileno(p)	(p)->_file
#endif

#ifndef R_OK
#define R_OK 4
#endif

extern int StandAlone;

extern int mac_app;
