/* Target-dependent code for Solaris.

   Copyright (C) 2006-2020 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "frame.h"
#include "symtab.h"
#include "inferior.h"
#include "objfiles.h"

#include "sol2-tdep.h"

/* The Solaris signal trampolines reside in libc.  For normal signals,
   the function `sigacthandler' is used.  This signal trampoline will
   call the signal handler using the System V calling convention,
   where the third argument is a pointer to an instance of
   `ucontext_t', which has a member `uc_mcontext' that contains the
   saved registers.  Incidentally, the kernel passes the `ucontext_t'
   pointer as the third argument of the signal trampoline too, and
   `sigacthandler' simply passes it on.  However, if you link your
   program with "-L/usr/ucblib -R/usr/ucblib -lucb", the function
   `ucbsigvechandler' will be used, which invokes the using the BSD
   convention, where the third argument is a pointer to an instance of
   `struct sigcontext'.  It is the `ucbsigvechandler' function that
   converts the `ucontext_t' to a `sigcontext', and back.  Unless the
   signal handler modifies the `struct sigcontext' we can safely
   ignore this.  */

static int
sol2_pc_in_sigtramp (CORE_ADDR pc, const char *name)
{
  return (name && (strcmp (name, "sigacthandler") == 0
		   || strcmp (name, "ucbsigvechandler") == 0
		   || strcmp (name, "__sighndlr") == 0));
}

/* Return whether THIS_FRAME corresponds to a Solaris sigtramp routine.  */

int
sol2_sigtramp_p (struct frame_info *this_frame)
{
  CORE_ADDR pc = get_frame_pc (this_frame);
  const char *name;

  find_pc_partial_function (pc, &name, NULL, NULL);
  return sol2_pc_in_sigtramp (pc, name);
}

/* Unglobalize NAME.  */

static const char *
sol2_static_transform_name (const char *name)
{
  /* The Sun compilers (Sun ONE Studio, Forte Developer, Sun WorkShop,
     SunPRO) convert file static variables into global values, a
     process known as globalization.  In order to do this, the
     compiler will create a unique prefix and prepend it to each file
     static variable.  For static variables within a function, this
     globalization prefix is followed by the function name (nested
     static variables within a function are supposed to generate a
     warning message, and are left alone).  The procedure is
     documented in the Stabs Interface Manual, which is distributed
     with the compilers, although version 4.0 of the manual seems to
     be incorrect in some places, at least for SPARC.  The
     globalization prefix is encoded into an N_OPT stab, with the form
     "G=<prefix>".  The globalization prefix always seems to start
     with a dollar sign '$' (sparc) resp. a dot '.' (x86); a dot '.'
     is used as a separator.  So we  simply strip everything up until
     the last dot.  */
  int prefix;
  
  switch (gdbarch_bfd_arch_info (target_gdbarch ())->arch)
    {
    case bfd_arch_i386:
      prefix = '.';
      break;
    case bfd_arch_sparc:
      prefix = '$';
      break;
    default:
      internal_error (__FILE__, __LINE__, "Unexpected arch");
      break;
    }

  if (name[0] == prefix)
    {
      const char *p = strrchr (name, '.');
      if (p)
        return p + 1;
    }

  return name;
}

static CORE_ADDR
sol2_skip_solib_resolver (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  struct bound_minimal_symbol msym;

  msym = lookup_minimal_symbol("elf_bndr", NULL, NULL);
  if (msym.minsym && BMSYMBOL_VALUE_ADDRESS (msym) == pc)
    return frame_unwind_caller_pc (get_current_frame ());

  return 0;
}

/* This is how we want PTIDs from Solaris core files to be printed.  */

static std::string
sol2_core_pid_to_str (struct gdbarch *gdbarch, ptid_t ptid)
{
  struct inferior *inf;
  int pid;

  /* Check whether we're printing an LWP (gdb thread) or a process.  */
  pid = ptid.lwp ();
  if (pid != 0)
    {
      /* A thread.  */
      return string_printf ("LWP %ld", ptid.lwp ());
    }

  /* GDB didn't use to put a NT_PSTATUS note in Solaris cores.  If
     that's missing, then we're dealing with a fake PID corelow.c made up.  */
  inf = find_inferior_ptid (current_inferior ()->process_target (), ptid);
  if (inf == NULL || inf->fake_pid_p)
    return "<core>";

  /* Not fake; print as usual.  */
  return normal_pid_to_str (ptid);
}

/* To be called from GDB_OSABI_SOLARIS handlers.  */

void
sol2_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  /* The Sun compilers (Sun ONE Studio, Forte Developer, Sun WorkShop, SunPRO)
     compiler puts out 0 instead of the address in N_SO stabs.  Starting with
     SunPRO 3.0, the compiler does this for N_FUN stabs too.  */
  set_gdbarch_sofun_address_maybe_missing (gdbarch, 1);

  /* The Sun compilers also do "globalization"; see the comment in
     sol2_static_transform_name for more information.  */
  set_gdbarch_static_transform_name (gdbarch, sol2_static_transform_name);

  /* Solaris uses SVR4-style shared libraries.  */
  set_gdbarch_skip_solib_resolver (gdbarch, sol2_skip_solib_resolver);

  /* How to print LWP PTIDs from core files.  */
  set_gdbarch_core_pid_to_str (gdbarch, sol2_core_pid_to_str);
}
