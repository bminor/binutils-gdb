/* OBSOLETE /* Macro definitions for running GDB on Apple Macintoshes. */
/* OBSOLETE    Copyright 1994, 1995, 2001 Free Software Foundation, Inc. */
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
/* OBSOLETE #include "mpw.h" */
/* OBSOLETE  */
/* OBSOLETE #include "fopen-bin.h" */
/* OBSOLETE  */
/* OBSOLETE #include "spin.h" */
/* OBSOLETE  */
/* OBSOLETE #define CANT_FORK */
/* OBSOLETE  */
/* OBSOLETE /* Map these standard functions to versions that can do I/O in a console */
/* OBSOLETE    window. */ */
/* OBSOLETE  */
/* OBSOLETE #define printf hacked_printf */
/* OBSOLETE #define fprintf hacked_fprintf */
/* OBSOLETE #define vprintf hacked_vfprintf */
/* OBSOLETE #define fputs hacked_fputs */
/* OBSOLETE #define fputc hacked_fputc */
/* OBSOLETE #undef putc */
/* OBSOLETE #define putc hacked_putc */
/* OBSOLETE #define fflush hacked_fflush */
/* OBSOLETE  */
/* OBSOLETE #define fgetc hacked_fgetc */
/* OBSOLETE  */
/* OBSOLETE #define POSIX_UTIME */
/* OBSOLETE  */
/* OBSOLETE /* '.' indicates drivers on the Mac, so we need a different filename. */ */
/* OBSOLETE  */
/* OBSOLETE #define GDBINIT_FILENAME "_gdbinit" */
/* OBSOLETE  */
/* OBSOLETE /* Commas are more common to separate dirnames in a path on Macs. */ */
/* OBSOLETE  */
/* OBSOLETE #define DIRNAME_SEPARATOR ',' */
/* OBSOLETE  */
/* OBSOLETE /* This is a real crufty hack. */ */
/* OBSOLETE  */
/* OBSOLETE #define HAVE_TERMIO */
/* OBSOLETE  */
/* OBSOLETE /* Addons to the basic MPW-supported signal list. */ */
/* OBSOLETE  */
/* OBSOLETE #ifndef SIGQUIT */
/* OBSOLETE #define SIGQUIT (1<<6) */
/* OBSOLETE #endif */
/* OBSOLETE #ifndef SIGHUP */
/* OBSOLETE #define SIGHUP (1<<7) */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* If __STDC__ is on, then this definition will be missing. */ */
/* OBSOLETE  */
/* OBSOLETE #ifndef fileno */
/* OBSOLETE #define fileno(p)	(p)->_file */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #ifndef R_OK */
/* OBSOLETE #define R_OK 4 */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE extern int StandAlone; */
/* OBSOLETE  */
/* OBSOLETE extern int mac_app; */
