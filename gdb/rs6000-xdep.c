/* IBM RS/6000 host-dependent code for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989, 1991 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "target.h"

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <sys/ptrace.h>
#include <sys/reg.h>

#include <a.out.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/core.h>
#include <sys/ldr.h>
#include <sys/utsname.h>

extern int errno;
extern int attach_flag;

/* Conversion from gdb-to-system special purpose register numbers.. */

static int special_regs[] = {
  IAR,				/* PC_REGNUM	*/
  MSR,				/* PS_REGNUM	*/
  CR,				/* CR_REGNUM	*/
  LR,				/* LR_REGNUM	*/
  CTR,				/* CTR_REGNUM	*/
  XER,				/* XER_REGNUM   */
  MQ				/* MQ_REGNUM	*/
};


/* Nonzero if we just simulated a single step break. */
extern int one_stepped;

extern char register_valid[];


void
fetch_inferior_registers (regno)
  int regno;
{
  int ii;
  extern char registers[];

  if (regno < 0) {			/* for all registers */

    /* read 32 general purpose registers. */

    for (ii=0; ii < 32; ++ii)
      *(int*)&registers[REGISTER_BYTE (ii)] = 
	ptrace (PT_READ_GPR, inferior_pid, ii, 0, 0);

    /* read general purpose floating point registers. */

    for (ii=0; ii < 32; ++ii)
      ptrace (PT_READ_FPR, inferior_pid, 
	(int*)&registers [REGISTER_BYTE (FP0_REGNUM+ii)], FPR0+ii, 0);

    /* read special registers. */
    for (ii=0; ii <= LAST_SP_REGNUM-FIRST_SP_REGNUM; ++ii)
      *(int*)&registers[REGISTER_BYTE (FIRST_SP_REGNUM+ii)] = 
	ptrace (PT_READ_GPR, inferior_pid, special_regs[ii], 0, 0);

    registers_fetched ();
    return;
  }

  /* else an individual register is addressed. */

  else if (regno < FP0_REGNUM) {		/* a GPR */
    *(int*)&registers[REGISTER_BYTE (regno)] =
	ptrace (PT_READ_GPR, inferior_pid, regno, 0, 0);
  }
  else if (regno <= FPLAST_REGNUM) {		/* a FPR */
    ptrace (PT_READ_FPR, inferior_pid,
	(int*)&registers [REGISTER_BYTE (regno)], (regno-FP0_REGNUM+FPR0), 0);
  }
  else if (regno <= LAST_SP_REGNUM) {		/* a special register */
    *(int*)&registers[REGISTER_BYTE (regno)] =
	ptrace (PT_READ_GPR, inferior_pid,
		special_regs[regno-FIRST_SP_REGNUM], 0, 0);
  }
  else
    fprintf (stderr, "gdb error: register no %d not implemented.\n", regno);

  register_valid [regno] = 1;
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (regno)
     int regno;
{
  extern char registers[];

  errno = 0;

  if (regno == -1) {			/* for all registers..	*/
      int ii;

       /* execute one dummy instruction (which is a breakpoint) in inferior
          process. So give kernel a chance to do internal house keeping.
	  Otherwise the following ptrace(2) calls will mess up user stack
	  since kernel will get confused about the bottom of the stack (%sp) */

       exec_one_dummy_insn ();

      /* write general purpose registers first! */
      for ( ii=GPR0; ii<=GPR31; ++ii) {
	ptrace (PT_WRITE_GPR, inferior_pid, ii,
		*(int*)&registers[REGISTER_BYTE (ii)], 0);
	if ( errno ) { 
	  perror ("ptrace write_gpr"); errno = 0;
	}
      }

      /* write floating point registers now. */
      for ( ii=0; ii < 32; ++ii) {
	ptrace (PT_WRITE_FPR, inferior_pid, 
		  (int*)&registers[REGISTER_BYTE (FP0_REGNUM+ii)], FPR0+ii, 0);
        if ( errno ) {
	  perror ("ptrace write_fpr"); errno = 0;
        }
      }

      /* write special registers. */
      for (ii=0; ii <= LAST_SP_REGNUM-FIRST_SP_REGNUM; ++ii) {
        ptrace (PT_WRITE_GPR, inferior_pid, special_regs[ii],
		 *(int*)&registers[REGISTER_BYTE (FIRST_SP_REGNUM+ii)], 0);
	if ( errno ) {
	  perror ("ptrace write_gpr"); errno = 0;
	}
      }
  }

  /* else, a specific register number is given... */

  else if (regno < FP0_REGNUM) {		/* a GPR */

    ptrace (PT_WRITE_GPR, inferior_pid, regno,
		*(int*)&registers[REGISTER_BYTE (regno)], 0);
  }

  else if (regno <= FPLAST_REGNUM) {		/* a FPR */
    ptrace (PT_WRITE_FPR, inferior_pid, 
	(int*)&registers[REGISTER_BYTE (regno)], regno-FP0_REGNUM+FPR0, 0);
  }

  else if (regno <= LAST_SP_REGNUM) {		/* a special register */

    ptrace (PT_WRITE_GPR, inferior_pid, special_regs [regno-FIRST_SP_REGNUM],
		*(int*)&registers[REGISTER_BYTE (regno)], 0);
  }

  else
    fprintf (stderr, "Gdb error: register no %d not implemented.\n", regno);

  if ( errno ) {
    perror ("ptrace write");  errno = 0;
  }
}

void
fetch_core_registers (core_reg_sect, core_reg_size, which, reg_addr)
     char *core_reg_sect;
     unsigned core_reg_size;
     int which;
     unsigned int reg_addr;	/* Unused in this version */
{
  /* fetch GPRs and special registers from the first register section
     in core bfd. */
  if (which == 0) {

    /* copy GPRs first. */
    bcopy (core_reg_sect, registers, 32 * 4);

    /* gdb's internal register template and bfd's register section layout
       should share a common include file. FIXMEmgo */
    /* then comes special registes. They are supposed to be in the same
       order in gdb template and bfd `.reg' section. */
    core_reg_sect += (32 * 4);
    bcopy (core_reg_sect, &registers [REGISTER_BYTE (FIRST_SP_REGNUM)],
    			(LAST_SP_REGNUM - FIRST_SP_REGNUM + 1) * 4);
  }

  /* fetch floating point registers from register section 2 in core bfd. */
  else if (which == 2)
    bcopy (core_reg_sect, &registers [REGISTER_BYTE (FP0_REGNUM)], 32 * 8);

  else
    fprintf (stderr, "Gdb error: unknown parameter to fetch_core_registers().\n");
}


frameless_function_invocation (fi)
struct frame_info *fi;
{
  CORE_ADDR func_start;
  int frameless, dummy;

  func_start = get_pc_function_start (fi->pc) + FUNCTION_START_OFFSET;

  /* If we failed to find the start of the function, it is a mistake
     to inspect the instructions. */

  if (!func_start)
    return 0;

  function_frame_info (func_start, &frameless, &dummy, &dummy, &dummy);
  return frameless;
}


/* aixcoff_relocate_symtab -	hook for symbol table relocation.
   also reads shared libraries.. */

aixcoff_relocate_symtab (pid)
unsigned int pid;
{
#define	MAX_LOAD_SEGS 64		/* maximum number of load segments */

    struct ld_info *ldi;
    int temp;

    ldi = (void *) alloca(MAX_LOAD_SEGS * sizeof (*ldi));

    /* According to my humble theory, aixcoff has some timing problems and
       when the user stack grows, kernel doesn't update stack info in time
       and ptrace calls step on user stack. That is why we sleep here a little,
       and give kernel to update its internals. */

    usleep (36000);

    errno = 0;
    ptrace(PT_LDINFO, pid, ldi, MAX_LOAD_SEGS * sizeof(*ldi), ldi);
    if (errno) {
      perror_with_name ("ptrace ldinfo");
      return 0;
    }

    vmap_ldinfo(ldi);

   do {
     add_text_to_loadinfo (ldi->ldinfo_textorg, ldi->ldinfo_dataorg);
    } while (ldi->ldinfo_next
	     && (ldi = (void *) (ldi->ldinfo_next + (char *) ldi)));

#if 0
  /* Now that we've jumbled things around, re-sort them.  */
  sort_minimal_symbols ();
#endif

  /* relocate the exec and core sections as well. */
  vmap_exec ();
}


/* Keep an array of load segment information and their TOC table addresses.
   This info will be useful when calling a shared library function by hand. */
   
typedef struct {
  unsigned long textorg, dataorg, toc_offset;
} LoadInfo;

#define	LOADINFOLEN	10

static	LoadInfo *loadInfo = NULL;
static	int	loadInfoLen = 0;
static	int	loadInfoTocIndex = 0;
int	aix_loadInfoTextIndex = 0;


xcoff_init_loadinfo ()
{
  loadInfoTocIndex = 0;
  aix_loadInfoTextIndex = 0;

  if (loadInfoLen == 0) {
    loadInfo = (void*) xmalloc (sizeof (LoadInfo) * LOADINFOLEN);
    loadInfoLen = LOADINFOLEN;
  }
}


free_loadinfo ()
{
  if (loadInfo)
    free (loadInfo);
  loadInfo = NULL;
  loadInfoLen = 0;
  loadInfoTocIndex = 0;
  aix_loadInfoTextIndex = 0;
}


xcoff_add_toc_to_loadinfo (unsigned long tocaddr)
{
  while (loadInfoTocIndex >= loadInfoLen) {
    loadInfoLen += LOADINFOLEN;
    loadInfo = (void*) xrealloc (loadInfo, sizeof(LoadInfo) * loadInfoLen);
  }
  loadInfo [loadInfoTocIndex++].toc_offset = tocaddr;
}


add_text_to_loadinfo (unsigned long textaddr, unsigned long dataaddr)
{
  while (aix_loadInfoTextIndex >= loadInfoLen) {
    loadInfoLen += LOADINFOLEN;
    loadInfo = (void*) xrealloc (loadInfo, sizeof(LoadInfo) * loadInfoLen);
  }
  loadInfo [aix_loadInfoTextIndex].textorg = textaddr;
  loadInfo [aix_loadInfoTextIndex].dataorg = dataaddr;
  ++aix_loadInfoTextIndex;
}


unsigned long
find_toc_address (unsigned long pc)
{
  int ii, toc_entry, tocbase = 0;

  for (ii=0; ii < aix_loadInfoTextIndex; ++ii)
    if (pc > loadInfo [ii].textorg && loadInfo [ii].textorg > tocbase) {
      toc_entry = ii;
      tocbase =  loadInfo [ii].textorg;
    }

  return loadInfo [toc_entry].dataorg + loadInfo [toc_entry].toc_offset;
}


/* execute one dummy breakpoint instruction. This way we give kernel
   a chance to do some housekeeping and update inferior's internal data,
   including u_area. */

exec_one_dummy_insn ()
{
#define	DUMMY_INSN_ADDR	(TEXT_SEGMENT_BASE)+0x200

  unsigned long shadow;
  unsigned int status, pid;

  /* We plant one dummy breakpoint into DUMMY_INSN_ADDR address. We assume that
     this address will never be executed again by the real code. */

  target_insert_breakpoint (DUMMY_INSN_ADDR, &shadow);

  errno = 0;
  ptrace (PT_CONTINUE, inferior_pid, DUMMY_INSN_ADDR, 0, 0);
  if (errno)
    perror ("pt_continue");

  do {
    pid = wait (&status);
  } while (pid != inferior_pid);
    
  target_remove_breakpoint (DUMMY_INSN_ADDR, &shadow);
}


/* Return the number of initial trap signals we need to ignore once the inferior
   process starts running. This will be `2' for aix-3.1, `3' for aix-3.2 */

int
aix_starting_inferior_traps ()
{
  struct utsname unamebuf;

  if (uname (&unamebuf) == -1)
    fatal ("uname(3) failed.");

  /* Assume the future versions will behave like 3.2 and return '3' for
     anything other than 3.1x. The extra trap in 3.2 is the "trap after the
     program is loaded" signal. */
  
  if (unamebuf.version[0] == '3' && unamebuf.release[0] == '1')
    return 2;
  else
    return 3;
}
