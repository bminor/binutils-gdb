/* Defs for interface to demanglers.
   Copyright 1992 Free Software Foundation, Inc.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#define DMGL_PARAMS	(1 << 0)	/* Include function args */
#define DMGL_ANSI	(1 << 1)	/* Include const, volatile, etc */

extern enum demangling_styles
{
  unknown_demangling = 0,
  auto_demangling,
  gnu_demangling,
  lucid_demangling,
  cfront_demangling
} current_demangling_style;

#define GNU_DEMANGLING_STYLE_STRING	"gnu"
#define LUCID_DEMANGLING_STYLE_STRING	"lucid"
#define CFRONT_DEMANGLING_STYLE_STRING	"cfront"
#define AUTO_DEMANGLING_STYLE_STRING	"auto"

extern void
set_demangling_style PARAMS ((char *));
