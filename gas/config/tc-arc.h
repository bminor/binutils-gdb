/* tc-arc.h - Macros and type defines for the ARC.
   Copyright (C) 1994, 1995 Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2,
   or (at your option) any later version.

   GAS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with GAS; see the file COPYING.  If not, write
   to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#define TC_ARC 1

#define LOCAL_LABELS_FB

#define TARGET_ARCH bfd_arch_arc
#define TARGET_FORMAT "elf32-arc"
#define LOCAL_LABEL(name)	((name)[0] == '.' && (name)[1] == 'L')
#define WORKING_DOT_WORD

#define LISTING_HEADER "ARC GAS "

#define TC_HANDLES_FX_DONE

#if 0
/* Extra stuff that we need to keep track of for each symbol.  */
struct arc_tc_sy
{
  /* The real name, if the symbol was renamed.  */
  char *real_name;
};

#define TC_SYMFIELD_TYPE struct arc_tc_sy

/* Finish up the symbol.  */
extern int arc_frob_symbol PARAMS ((struct symbol *));
#define tc_frob_symbol(sym, punt) punt = arc_frob_symbol (sym)
#endif
