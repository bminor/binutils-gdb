/* Native-dependent code for Linux running on i386's, for GDB.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "inferior.h"
#include "gdbcore.h"

/* For i386_linux_skip_solib_resolver */
#include "symtab.h"
#include "frame.h"
#include "symfile.h"
#include "objfiles.h"

#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/procfs.h>

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif

/* This is a duplicate of the table in i386-xdep.c. */

static int regmap[] = 
{
  EAX, ECX, EDX, EBX,
  UESP, EBP, ESI, EDI,
  EIP, EFL, CS, SS,
  DS, ES, FS, GS,
};


/* Given a pointer to a general register set in struct user format
   (gregset_t *), unpack the register contents and supply them as
   gdb's idea of the current register values. */
void
supply_gregset (gregsetp)
     gregset_t *gregsetp;
{
  register int regi;
  register greg_t *regp = (greg_t *) gregsetp;

  for (regi = 0; regi < NUM_GREGS; regi++)
    {
      supply_register (regi, (char *) (regp + regmap[regi]));
    }
}

/* Fill in a gregset_t object with selected data from a gdb-format
   register file.
   - GREGSETP points to the gregset_t object to be filled.
   - GDB_REGS points to the GDB-style register file providing the data.
   - VALID is an array indicating which registers in GDB_REGS are
     valid; the parts of *GREGSETP that would hold registers marked
     invalid in GDB_REGS are left unchanged.  If VALID is zero, all
     registers are assumed to be valid.  */
void
convert_to_gregset (gregset_t *gregsetp,
		    char *gdb_regs,
		    signed char *valid)
{
  int regi;
  register greg_t *regp = (greg_t *) gregsetp;

  for (regi = 0; regi < NUM_GREGS; regi++)
    if (! valid || valid[regi])
      *(regp + regmap[regi]) = * (int *) &registers[REGISTER_BYTE (regi)];
}

void
fill_gregset (gregset_t *gregsetp,
	      int regno)
{
  if (regno == -1)
    convert_to_gregset (gregsetp, registers, 0);
  else
    {
      signed char valid[NUM_GREGS];
      memset (valid, 0, sizeof (valid));
      valid[regno] = 1;
      convert_to_gregset (gregsetp, valid, valid);
    }
}


/* Where does st(N) start in the fpregset_t structure F?  */
#define FPREGSET_T_FPREG_OFFSET(f, n) \
  ((char *) &(f)->st_space + (n) * 10)

/* Fill GDB's register file with the floating-point register values in
   *FPREGSETP.  */
void 
supply_fpregset (fpregset_t *fpregsetp)
{
  int i;

  /* Supply the floating-point registers.  */
  for (i = 0; i < 8; i++)
    supply_register (FP0_REGNUM + i, FPREGSET_T_FPREG_OFFSET (fpregsetp, i));

  supply_register (FCTRL_REGNUM, (char *) &fpregsetp->cwd);
  supply_register (FSTAT_REGNUM, (char *) &fpregsetp->swd);
  supply_register (FTAG_REGNUM,  (char *) &fpregsetp->twd);
  supply_register (FCOFF_REGNUM, (char *) &fpregsetp->fip);
  supply_register (FDS_REGNUM,   (char *) &fpregsetp->fos);
  supply_register (FDOFF_REGNUM, (char *) &fpregsetp->foo);
  
  /* Extract the code segment and opcode from the  "fcs" member.  */
  {
    long l;

    l = fpregsetp->fcs & 0xffff;
    supply_register (FCS_REGNUM, (char *) &l);

    l = (fpregsetp->fcs >> 16) & ((1 << 11) - 1);
    supply_register (FOP_REGNUM, (char *) &l);
  }
}


/* Fill in an fpregset_t structure with selected data from a
   gdb-format register file.
   - FPREGSETP points to the structure to be filled. 
   - GDB_REGS points to the GDB-style register file providing the data.
   - VALID is an array indicating which registers in GDB_REGS are
     valid; the parts of *FPREGSETP that would hold registers marked
     invalid in GDB_REGS are left unchanged.  If VALID is zero, all
     registers are assumed to be valid.  */
void
convert_to_fpregset (fpregset_t *fpregsetp,
		     char *gdb_regs,
		     signed char *valid)
{
  int i;

  /* Fill in the floating-point registers.  */
  for (i = 0; i < 8; i++)
    if (!valid || valid[i])
      memcpy (FPREGSET_T_FPREG_OFFSET (fpregsetp, i),
	      &registers[REGISTER_BYTE (FP0_REGNUM + i)],
	      REGISTER_RAW_SIZE(FP0_REGNUM + i));

#define fill(MEMBER, REGNO)						\
  if (! valid || valid[(REGNO)])					\
    memcpy (&fpregsetp->MEMBER, &registers[REGISTER_BYTE (REGNO)],	\
	    sizeof (fpregsetp->MEMBER))

  fill (cwd, FCTRL_REGNUM);
  fill (swd, FSTAT_REGNUM);
  fill (twd, FTAG_REGNUM);
  fill (fip, FCOFF_REGNUM);
  fill (foo, FDOFF_REGNUM);
  fill (fos, FDS_REGNUM);

#undef fill

  if (! valid || valid[FCS_REGNUM])
    fpregsetp->fcs
      = ((fpregsetp->fcs & ~0xffff)
	 | (* (int *) &registers[REGISTER_BYTE (FCS_REGNUM)] & 0xffff));

  if (! valid || valid[FOP_REGNUM])
    fpregsetp->fcs
      = ((fpregsetp->fcs & 0xffff)
	 | ((*(int *) &registers[REGISTER_BYTE (FOP_REGNUM)] & ((1 << 11) - 1))
	    << 16));
}


/* Given a pointer to a floating point register set in (fpregset_t *)
   format, update all of the registers from gdb's idea of the current
   floating point register set.  */

void
fill_fpregset (fpregset_t *fpregsetp,
	       int regno)
{
  convert_to_fpregset (fpregsetp, registers, 0);
}


/* Get the whole floating point state of the process and store the
   floating point stack into registers[].  */
static void
fetch_fpregs ()
{
  int ret, regno;
  fpregset_t buf;

  ret = ptrace (PTRACE_GETFPREGS, inferior_pid,	0, (int) &buf);
  if (ret < 0)
    {
      warning ("Couldn't get floating point status");
      return;
    }

  /* ptrace fills an fpregset_t, so we can use the same function we do
     for core files.  */
  supply_fpregset (&buf);
}


/* Set the inferior's floating-point registers to the values in
   registers[] --- but only those registers marked valid.  */
static void
store_fpregs ()
{
  int ret;
  fpregset_t buf;

  ret = ptrace (PTRACE_GETFPREGS, inferior_pid,	0, (int) &buf);
  if (ret < 0)
    {
      warning ("Couldn't get floating point status");
      return;
    }

  convert_to_fpregset (&buf, registers, register_valid);

  ret = ptrace (PTRACE_SETFPREGS, inferior_pid, 0, (int) &buf);
  if (ret < 0)
    {
      warning ("Couldn't write floating point status");
      return;
    }
}


/* Read the general registers from the process, and store them
   in registers[].  */
static void
fetch_regs ()
{
  int ret, regno;
  gregset_t buf;

  ret = ptrace (PTRACE_GETREGS, inferior_pid, 0, (int) &buf);
  if (ret < 0)
    {
      warning ("Couldn't get registers");
      return;
    }

  supply_gregset (&buf);
}


/* Set the inferior's general registers to the values in registers[]
   --- but only those registers marked as valid.  */
static void
store_regs ()
{
  int ret, regno;
  gregset_t buf;

  ret = ptrace (PTRACE_GETREGS, inferior_pid, 0, (int) &buf);
  if (ret < 0)
    {
      warning ("Couldn't get registers");
      return;
    }

  convert_to_gregset (&buf, registers, register_valid);

  ret = ptrace (PTRACE_SETREGS, inferior_pid, 0, (int)buf);
  if (ret < 0)
    {
      warning ("Couldn't write registers");
      return;
    }
}


/* Fetch registers from the child process.
   Fetch all if regno == -1, otherwise fetch all ordinary
   registers or all floating point registers depending
   upon the value of regno. */

void
fetch_inferior_registers (int regno)
{
  if (regno < NUM_GREGS || regno == -1)
    fetch_regs ();

  if (regno >= NUM_GREGS || regno == -1)
    fetch_fpregs ();
}


/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register, which
   then determines whether we store all ordinary
   registers or all of the floating point registers. */

void
store_inferior_registers (regno)
     int regno;
{
  if (regno < NUM_GREGS || regno == -1)
    store_regs ();

  if (regno >= NUM_GREGS || regno == -1)
    store_fpregs ();
}


/* Find the minimal symbol named NAME, and return both the minsym
   struct and its objfile.  This probably ought to be in minsym.c, but
   everything there is trying to deal with things like C++ and
   SOFUN_ADDRESS_MAYBE_TURQUOISE, ...  Since this is so simple, it may
   be considered too special-purpose for general consumption.  */

static struct minimal_symbol *
find_minsym_and_objfile (char *name, struct objfile **objfile_p)
{
  struct objfile *objfile;

  ALL_OBJFILES (objfile)
    {
      struct minimal_symbol *msym;

      ALL_OBJFILE_MSYMBOLS (objfile, msym)
	{
	  if (SYMBOL_NAME (msym)
	      && STREQ (SYMBOL_NAME (msym), name))
	    {
	      *objfile_p = objfile;
	      return msym;
	    }
	}
    }

  return 0;
}


static CORE_ADDR
skip_hurd_resolver (CORE_ADDR pc)
{
  /* The HURD dynamic linker is part of the GNU C library, so many
     GNU/Linux distributions use it.  (All ELF versions, as far as I
     know.)  An unresolved PLT entry points to "_dl_runtime_resolve",
     which calls "fixup" to patch the PLT, and then passes control to
     the function.

     We look for the symbol `_dl_runtime_resolve', and find `fixup' in
     the same objfile.  If we are at the entry point of `fixup', then
     we set a breakpoint at the return address (at the top of the
     stack), and continue.
  
     It's kind of gross to do all these checks every time we're
     called, since they don't change once the executable has gotten
     started.  But this is only a temporary hack --- upcoming versions
     of Linux will provide a portable, efficient interface for
     debugging programs that use shared libraries.  */

  struct objfile *objfile;
  struct minimal_symbol *resolver 
    = find_minsym_and_objfile ("_dl_runtime_resolve", &objfile);

  if (resolver)
    {
      struct minimal_symbol *fixup
	= lookup_minimal_symbol ("fixup", 0, objfile);

      if (fixup && SYMBOL_VALUE_ADDRESS (fixup) == pc)
	return (SAVED_PC_AFTER_CALL (get_current_frame ()));
    }

  return 0;
}      


/* See the comments for SKIP_SOLIB_RESOLVER at the top of infrun.c.
   This function:
   1) decides whether a PLT has sent us into the linker to resolve
      a function reference, and 
   2) if so, tells us where to set a temporary breakpoint that will
      trigger when the dynamic linker is done.  */

CORE_ADDR
i386_linux_skip_solib_resolver (CORE_ADDR pc)
{
  CORE_ADDR result;

  /* Plug in functions for other kinds of resolvers here.  */
  result = skip_hurd_resolver (pc);
  if (result)
    return result;

  return 0;
}
