/* This file is ranlib.h
   Copyright (C) 1987-1992 Free Software Foundation, Inc.
   
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

/*
 * Structure of the __.SYMDEF table of contents for an archive.
 * __.SYMDEF begins with a word giving the number of ranlib structures
 * which immediately follow, and then continues with a string
 * table consisting of a word giving the number of bytes of strings
 * which follow and then the strings themselves.
 * The ran_strx fields index the string table whose first byte is numbered 0.
 */
struct	ranlib {
	union {
		off_t	ran_strx;	/* string table index of */
		char	*ran_name;	/* symbol defined by */
	} ran_un;
	off_t	ran_off;		/* library member at this offset */
};

/* end of ranlib.h */
