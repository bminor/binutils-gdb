/* config.h -

   Copyright (C) 1991 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

   GLD is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   GLD is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GLD; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* Look in this environment name for the linker to pretend to be */
#define EMULATION_ENVIRON "LDEMULATION"
/* If in there look for the strings: */
#define GLD_EMULATION_NAME "gld"
#define VANILLA_EMULATION_NAME "vanilla"
#define GLD68K_EMULATION_NAME "gld68k"
#define GLD960_EMULATION_NAME "gld960"
#define LNK960_EMULATION_NAME "lnk960"
/* Otherwise default to this emulation */
#ifdef GNU960
#define DEFAULT_EMULATION GLD960_EMULATION_NAME
#else
#define DEFAULT_EMULATION GLD68K_EMULATION_NAME
#endif


/* Look in this variable for a target format */
#define TARGET_ENVIRON "GNUTARGET"
/* If not there then choose this */
#define GLD_TARGET "a.out-generic-big"
#define GLD68K_TARGET "a.out-generic-big"
#define LNK960_TARGET "coff-Intel-big"
#define GLD960_TARGET "b.out.big"
#define VANILLA_TARGET "a.out-generic-big"




