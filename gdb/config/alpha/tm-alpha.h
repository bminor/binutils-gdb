/* Definitions to make GDB run on an Alpha box under OSF1.  This is
   also used by the Alpha/Netware and Alpha GNU/Linux targets.

   Copyright 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2002, 2004 Free
   Software Foundation, Inc.

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

#ifndef TM_ALPHA_H
#define TM_ALPHA_H

#include "bfd.h"
#include "coff/sym.h"		/* Needed for PDR below.  */
#include "coff/symconst.h"

struct frame_info;
struct symbol;

/* Special symbol found in blocks associated with routines.  We can hang
   alpha_extra_func_info_t's off of this.  */

#define MDEBUG_EFI_SYMBOL_NAME "__GDB_EFI_INFO__"
extern void ecoff_relocate_efi (struct symbol *, CORE_ADDR);

#define RA_REGNUM 26		/* XXXJRT needed by mdebugread.c */

/* Specific information about a procedure.
   This overlays the ALPHA's PDR records, 
   alpharead.c (ab)uses this to save memory */

struct mdebug_extra_func_info
{
  long numargs;		/* number of args to procedure (was iopt) */
  PDR pdr;			/* Procedure descriptor record */
};

#endif /* TM_ALPHA_H */
