/*  This file is part of the program GDB.

    Copyright (C) 1997, Free Software Foundation, Inc.

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 
    */


/* from Michael Meissner, macro's to handle concating symbols better */

#ifndef _SIM_XCAT_H_
#define _SIM_XCAT_H_

#if defined __STDC__ || defined ALMOST_STDC
#define CONCAT2(a,b)	 a##b
#define CONCAT3(a,b,c)	 a##b##c
#define CONCAT4(a,b,c,d) a##b##c##d
#define STRINGX(s) #s
#else
#define CONCAT2(a,b)	 a/**/b
#define CONCAT3(a,b,c)	 a/**/b/**/c
#define CONCAT4(a,b,c,d) a/**/b/**/c/**/d
#define STRINGX(s) "?"
#endif

#define XCONCAT2(a,b)     CONCAT2(a,b)
#define XCONCAT3(a,b,c)   CONCAT3(a,b,c)
#define XCONCAT4(a,b,c,d) CONCAT4(a,b,c,d)

#define XSTRING(s) STRINGX(s) 

#endif _SIM_XCAT_H_
