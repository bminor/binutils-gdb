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


/*  FIXME:  These routine absolutely depends upon (NUM_REGS - NUM_FREGS)
    being less than or equal to the number of registers that can be stored
    in a gregset_t.  Note that with the current scheme there will typically
    be more registers actually stored in a gregset_t that what we know
    about.  This is bogus and should be fixed. */

/*  Given a pointer to a general register set in /proc format (gregset_t *),
    unpack the register contents and supply them as gdb's idea of the current
    register values. */

void
supply_gregset (gregsetp)
     gregset_t *gregsetp;
{
  register int regi;
  register greg_t *regp = (greg_t *) gregsetp;

  for (regi = 0 ; regi < (NUM_REGS - NUM_FREGS) ; regi++)
    {
      supply_register (regi, (char *) (regp + regmap[regi]));
    }
}

void
fill_gregset (gregsetp, regno)
     gregset_t *gregsetp;
     int regno;
{
  int regi;
  register greg_t *regp = (greg_t *) gregsetp;

  for (regi = 0 ; regi < (NUM_REGS - NUM_FREGS) ; regi++)
    {
      if ((regno == -1) || (regno == regi))
	{
	  *(regp + regmap[regi]) = *(int *) &registers[REGISTER_BYTE (regi)];
	}
    }
}


/*  Given a pointer to a floating point register set in (fpregset_t *)
    format, unpack the register contents and supply them as gdb's
    idea of the current floating point register values. */

void 
supply_fpregset (fpregsetp)
     fpregset_t *fpregsetp;
{
  register int regi;
  char *from;
  from = (char *) &(fpregsetp->st_space[0]);
  for (regi = FPSTART_REGNUM ; regi <= FPEND_REGNUM ; regi++)
    {
      supply_register(regi, from);
      from += REGISTER_RAW_SIZE(regi);
    }
}

/*  Given a pointer to a floating point register set in (fpregset_t *)
    format, update all of the registers from gdb's idea
    of the current floating point register set. */

void
fill_fpregset (fpregsetp, regno)
     fpregset_t *fpregsetp;
     int regno;
{
  int regi;
  char *to;
  char *from;

  to = (char *) &(fpregsetp->st_space[0]);
  for (regi = FPSTART_REGNUM ; regi <= FPEND_REGNUM ; regi++)
    {
      from = (char *) &registers[REGISTER_BYTE (regi)];
      memcpy (to, from, REGISTER_RAW_SIZE (regi));
      to += REGISTER_RAW_SIZE(regi);
    }
}

/*
  Get the whole floating point state of the process and
  store the floating point stack into registers[].
  */
static void
fetch_fpregs(void)
{
  int ret, regno;
  char buf[FPREG_BYTES];

  ret = ptrace (PTRACE_GETFPREGS, inferior_pid,	0, (int)buf);
  if ( ret < 0 )
    {
      warning ("Couldn't get floating point status");
      return;
    }

  for ( regno = 0; regno < NUM_FREGS; regno++ )
    {
      if ( regno < 7 )
	supply_register (NUM_REGS-NUM_FREGS+regno, buf + regno*4);
      else
	supply_register (NUM_REGS-NUM_FREGS+regno,
			 buf + FPENV_BYTES + (regno-7)*FPREG_RAW_SIZE);
    }

}


/*
  Get the whole floating point state of the process and
  replace the contents from registers[].
  */
static void
store_fpregs(void)
{
  int ret, regno;
  char buf[FPREG_BYTES];

  ret = ptrace (PTRACE_GETFPREGS, inferior_pid,	0, (int)buf);
  if ( ret < 0 )
    {
      warning ("Couldn't get floating point status");
      return;
    }

  for ( regno = 0; regno < NUM_FREGS; regno++ )
    {
      if ( register_valid[regno] )
	{
	  if ( regno < 7 )
	    {
	      read_register_gen (NUM_REGS-NUM_FREGS+regno,
				 buf + regno*4);
	    }
	  else
	    {
	      read_register_gen (NUM_REGS-NUM_FREGS+regno,
				 buf + FPENV_BYTES + (regno-7)*FPREG_RAW_SIZE);
	    }
	}
    }

  ret = ptrace (PTRACE_SETFPREGS, inferior_pid, 0, (int)buf);
  if ( ret < 0 )
    {
      warning ("Couldn't write floating point status");
      return;
    }

}


/*
  Get state of all non-fp registers of the process and
  store into registers[].
  */
static void
fetch_regs(void)
{
  int ret, regno;
  char buf[17*sizeof(unsigned int)];

  ret = ptrace (PTRACE_GETREGS, inferior_pid, 0, (int)buf);
  if ( ret < 0 )
    {
      warning ("Couldn't get registers");
      return;
    }

  for ( regno = 0; regno < NUM_REGS-NUM_FREGS; regno++ )
    supply_register (regno, buf + register_addr (regno, U_REGS_OFFSET));

}


/*
  Get the whole non-floating-point register state of the process and
  replace them in the process from registers[].
  */
static void
store_regs(void)
{
  int ret, regno;
  char buf[17*sizeof(unsigned int)];

  ret = ptrace (PTRACE_GETREGS, inferior_pid, 0, (int)buf);
  if ( ret < 0 )
    {
      warning ("Couldn't get registers");
      return;
    }

  for ( regno = 0; regno < NUM_REGS-NUM_FREGS; regno++ )
    {
      if ( register_valid[regno] )
	read_register_gen (regno, buf + register_addr (regno, U_REGS_OFFSET));
    }

  ret = ptrace (PTRACE_SETREGS, inferior_pid, 0, (int)buf);

  if ( ret < 0 )
    {
      warning ("Couldn't write floating point status");
      return;
    }

}


/* Fetch registers from the child process.
   Fetch all if regno == -1, otherwise fetch all ordinary
   registers or all floating point registers depending
   upon the value of regno. */

void
fetch_inferior_registers (regno)
     int regno;
{
  if ( (regno < NUM_REGS - NUM_FREGS) || (regno == -1) )
    fetch_regs();

  if ( (regno >= NUM_REGS - NUM_FREGS) || (regno == -1) )
    fetch_fpregs();
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
  if ( (regno < NUM_REGS - NUM_FREGS) || (regno == -1) )
    store_regs();

  if ( (regno >= NUM_REGS - NUM_FREGS) || (regno == -1) )
    store_fpregs();
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
