/* OBSOLETE /* Definitions for hosting on WIN32, for GDB. */
/* OBSOLETE    Copyright 1995, 1996, 1998, 2001 Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #define HOST_BYTE_ORDER LITTLE_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE #include "fopen-bin.h" */
/* OBSOLETE  */
/* OBSOLETE #define GDBINIT_FILENAME "gdb.ini" */
/* OBSOLETE  */
/* OBSOLETE #define SLASH_P(X) ((X)=='\\' || (X) == '/') */
/* OBSOLETE #define ROOTED_P(X) ((SLASH_P((X)[0]))|| ((X)[1] ==':')) */
/* OBSOLETE #define SLASH_CHAR '/' */
/* OBSOLETE #define SLASH_STRING "/" */
/* OBSOLETE  */
/* OBSOLETE /* Define this lseek(n) != nth byte of file */ */
/* OBSOLETE #define LSEEK_NOT_LINEAR */
/* OBSOLETE  */
/* OBSOLETE /* If under Cygwin, provide backwards compatibility with older */
/* OBSOLETE    Cygwin compilers that don't define the current cpp define. */ */
/* OBSOLETE #ifdef __CYGWIN32__ */
/* OBSOLETE #ifndef __CYGWIN__ */
/* OBSOLETE #define __CYGWIN__ */
/* OBSOLETE #endif */
/* OBSOLETE #endif  */
