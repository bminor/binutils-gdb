/* OBSOLETE /* Host definitions for GDB running on an a29k NYU Ultracomputer */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1993 Free Software Foundation, Inc. */
/* OBSOLETE    Contributed by David Wood (wood@lab.ultra.nyu.edu). */
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
/* OBSOLETE /* Here at NYU we have what we call an ULTRA3 PE board.  So */
/* OBSOLETE    ifdefs for ULTRA3 are my doing.  At this point in time, */
/* OBSOLETE    I don't know of any other Unixi running on the a29k.  */ */
/* OBSOLETE  */
/* OBSOLETE #define HOST_BYTE_ORDER BIG_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE #define HAVE_WAIT_STRUCT */
/* OBSOLETE  */
/* OBSOLETE #ifndef L_SET */
/* OBSOLETE #define L_SET   0		/* set the seek pointer */ */
/* OBSOLETE #define L_INCR  1		/* increment the seek pointer */ */
/* OBSOLETE #define L_XTND  2		/* extend the file size */ */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #ifndef O_RDONLY */
/* OBSOLETE #define O_RDONLY 0 */
/* OBSOLETE #define O_WRONLY 1 */
/* OBSOLETE #define O_RDWR	  2 */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE #ifndef F_OK */
/* OBSOLETE #define R_OK 4 */
/* OBSOLETE #define W_OK 2 */
/* OBSOLETE #define X_OK 1 */
/* OBSOLETE #define F_OK 0 */
/* OBSOLETE #endif */
/* OBSOLETE  */
/* OBSOLETE /* System doesn't provide siginterrupt().  */ */
/* OBSOLETE #define	NO_SIGINTERRUPT */
/* OBSOLETE  */
/* OBSOLETE /* System uses a `short' to hold a process group ID.  */ */
/* OBSOLETE #define	SHORT_PGRP */
