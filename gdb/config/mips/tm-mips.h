/* Definitions to make GDB run on a mips box under 4.3bsd.

   Copyright 1986, 1987, 1989, 1991, 1992, 1993, 1994, 1995, 1996,
   1997, 1998, 1999, 2000, 2002, 2003 Free Software Foundation, Inc.

   Contributed by Per Bothner (bothner@cs.wisc.edu) at U.Wisconsin
   and by Alessandro Forin (af@cs.cmu.edu) at CMU..

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

#ifndef TM_MIPS_H
#define TM_MIPS_H 1

#include "regcache.h"

struct frame_info;
struct symbol;
struct type;
struct value;

#include <bfd.h>
#include "coff/sym.h"		/* Needed for PDR below.  */
#include "coff/symconst.h"

/* Return non-zero if PC points to an instruction which will cause a step
   to execute both the instruction at PC and an instruction at PC+4.  */
extern int mips_step_skips_delay (CORE_ADDR);
#define STEP_SKIPS_DELAY_P (1)
#define STEP_SKIPS_DELAY(pc) (mips_step_skips_delay (pc))

#define RA_REGNUM 31		/* Contains return address value */
#define PS_REGNUM 32		/* Contains processor status */
#define	FIRST_EMBED_REGNUM 74	/* First CP0 register for embedded use */
#define	PRID_REGNUM 89		/* Processor ID */
#define	LAST_EMBED_REGNUM 89	/* Last one */

/* Special symbol found in blocks associated with routines.  We can hang
   mips_extra_func_info_t's off of this.  */

#define MIPS_EFI_SYMBOL_NAME "__GDB_EFI_INFO__"
extern void ecoff_relocate_efi (struct symbol *, CORE_ADDR);

/* Specific information about a procedure.
   This overlays the MIPS's PDR records, 
   mipsread.c (ab)uses this to save memory */

typedef struct mips_extra_func_info
  {
    long numargs;		/* number of args to procedure (was iopt) */
    bfd_vma high_addr;		/* upper address bound */
    long frame_adjust;		/* offset of FP from SP (used on MIPS16) */
    PDR pdr;			/* Procedure descriptor record */
  }
 *mips_extra_func_info_t;

/* Functions for dealing with MIPS16 call and return stubs.  */
#define DEPRECATED_IGNORE_HELPER_CALL(pc)			mips_ignore_helper (pc)
extern int mips_ignore_helper (CORE_ADDR pc);

#endif /* TM_MIPS_H */
