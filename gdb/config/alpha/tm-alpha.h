/* Definitions to make GDB run on an Alpha box under OSF1.  This is
   also used by the Alpha/Netware and Alpha GNU/Linux targets.

   Copyright 1993, 1994, 1995, 1996, 1998, 1999, 2000, 2002 Free
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

#define GDB_MULTI_ARCH GDB_MULTI_ARCH_PARTIAL

#include "regcache.h"
#include "bfd.h"
#include "coff/sym.h"		/* Needed for PDR below.  */
#include "coff/symconst.h"

struct frame_info;
struct type;
struct value;
struct symbol;

/* Number of traps that happen between exec'ing the shell 
 * to run an inferior, and when we finally get to 
 * the inferior code.  This is 2 on most implementations.
 */
#define START_INFERIOR_TRAPS_EXPECTED 3

/* Offset from address of function to start of its code.
   Zero on most machines.  */

#define FUNCTION_START_OFFSET 0

/* Are we currently handling a signal ?  */

#define IN_SIGTRAMP(pc, name) alpha_osf_in_sigtramp ((pc), (name))
extern int alpha_osf_in_sigtramp (CORE_ADDR, char *);

#define BREAKPOINT {0x80, 0, 0, 0}	/* call_pal bpt */

/* Special symbol found in blocks associated with routines.  We can hang
   alpha_extra_func_info_t's off of this.  */

#define MIPS_EFI_SYMBOL_NAME "__GDB_EFI_INFO__"
extern void ecoff_relocate_efi (struct symbol *, CORE_ADDR);

#define RA_REGNUM 26		/* XXXJRT needed by mdebugread.c */

/* Specific information about a procedure.
   This overlays the ALPHA's PDR records, 
   alpharead.c (ab)uses this to save memory */

typedef struct alpha_extra_func_info
  {
    long numargs;		/* number of args to procedure (was iopt) */
    PDR pdr;			/* Procedure descriptor record */
  }
 *alpha_extra_func_info_t;

/* Define the extra_func_info that mipsread.c needs.
   FIXME: We should define our own PDR interface, perhaps in a separate
   header file. This would get rid of the <bfd.h> inclusion in all sources
   and would abstract the mips/alpha interface from ecoff.  */
#define mips_extra_func_info alpha_extra_func_info
#define mips_extra_func_info_t alpha_extra_func_info_t


#define PRINT_EXTRA_FRAME_INFO(fi) alpha_print_extra_frame_info ((fi))
extern void alpha_print_extra_frame_info (struct frame_info *);


/* It takes two values to specify a frame on the ALPHA.  Sigh.

   In fact, at the moment, the *PC* is the primary value that sets up
   a frame.  The PC is looked up to see what function it's in; symbol
   information from that function tells us which register is the frame
   pointer base, and what offset from there is the "virtual frame pointer".
   (This is usually an offset from SP.)  FIXME -- this should be cleaned
   up so that the primary value is the SP, and the PC is used to disambiguate
   multiple functions with the same SP that are at different stack levels. */

#define SETUP_ARBITRARY_FRAME(argc, argv) \
  alpha_setup_arbitrary_frame (argc, argv)
extern struct frame_info *alpha_setup_arbitrary_frame (int, CORE_ADDR *);

/* If PC is in a shared library trampoline code, return the PC
   where the function itself actually starts.  If not, return 0.  */
#define SKIP_TRAMPOLINE_CODE(pc)  find_solib_trampoline_target (pc)

/* Return TRUE if procedure descriptor PROC is a procedure descriptor
   that refers to a dynamically generated sigtramp function.

   OSF/1 doesn't use dynamic sigtramp functions, so this is always
   FALSE.  */

#define PROC_DESC_IS_DYN_SIGTRAMP(proc)	(0)
#define SET_PROC_DESC_IS_DYN_SIGTRAMP(proc)

/* If PC is inside a dynamically generated sigtramp function, return
   how many bytes the program counter is beyond the start of that
   function.  Otherwise, return a negative value.

   OSF/1 doesn't use dynamic sigtramp functions, so this always
   returns -1.  */

#define DYNAMIC_SIGTRAMP_OFFSET(pc)	(-1)

/* Translate a signal handler frame into the address of the sigcontext
   structure.  */

#define SIGCONTEXT_ADDR(frame) \
  (read_memory_integer ((frame)->next ? frame->next->frame : frame->frame, 8))

/* If FRAME refers to a sigtramp frame, return the address of the next
   frame.  */

#define FRAME_PAST_SIGTRAMP_FRAME(frame, pc) \
  (alpha_osf_skip_sigtramp_frame (frame, pc))
extern CORE_ADDR alpha_osf_skip_sigtramp_frame (struct frame_info *,
						CORE_ADDR);

/* Single step based on where the current instruction will take us.  */
extern void alpha_software_single_step (enum target_signal, int);

#endif /* TM_ALPHA_H */
