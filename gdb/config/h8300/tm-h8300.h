/* Parameters for execution on a H8/300 series machine.
   Copyright 1992, 1993, 1994, 1996, 1998, 1999, 2000
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */
/* Contributed by Steve Chamberlain sac@cygnus.com */

#define GDB_MULTI_ARCH GDB_MULTI_ARCH_PARTIAL

/* NOTE: ezannoni 2000-07-18: these variables are part of sim, defined
   in sim/h8300/compile.c.  They really should not be used this
   way. Because of this we cannot get rid of the macro
   GDB_TARGET_IS_H8300 in remote-e7000.c */
extern int h8300hmode;
extern int h8300smode;
#define GDB_TARGET_IS_H8300

/* Needed for remote.c */
#define REMOTE_BREAKPOINT { 0x57, 0x30}		/* trapa #3 */
/* Needed for remote-hms.c */
#define CCR_REGNUM 8
/* Needed for remote-e7000.c */
#define NUM_REALREGS (h8300smode?11:10)

