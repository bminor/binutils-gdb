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

/* Enumeration of possible demangling styles.

   Lucid and cfront styles are still kept logically distinct, even though
   they now both behave identically.  The resulting style is actual the
   union of both.  I.E. either style recognizes both "__pt__" and "__rf__"
   for operator "->", even though the first is lucid style and the second
   is cfront/ARM style. (FIXME?) */

extern enum demangling_styles
{
  unknown_demangling = 0,
  auto_demangling,
  gnu_demangling,
  lucid_demangling,
  cfront_demangling
} current_demangling_style;

/* Define string names for the various demangling styles. */

#define AUTO_DEMANGLING_STYLE_STRING	"auto"
#define GNU_DEMANGLING_STYLE_STRING	"gnu"
#define LUCID_DEMANGLING_STYLE_STRING	"lucid"
#define CFRONT_DEMANGLING_STYLE_STRING	"cfront"

/* Some macros to test what demangling style is active. */

#define AUTO_DEMANGLING (current_demangling_style == auto_demangling)
#define GNU_DEMANGLING (current_demangling_style == gnu_demangling)
#define LUCID_DEMANGLING (current_demangling_style == lucid_demangling)
#define CFRONT_DEMANGLING (current_demangling_style == cfront_demangling)

extern void
set_demangling_style PARAMS ((char *));
