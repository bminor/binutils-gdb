/* Target-dependent code for HPUX running on PA-RISC, for GDB.

   Copyright 2002, 2003 Free Software Foundation, Inc.

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
#include "arch-utils.h"
#include "gdbcore.h"
#include "osabi.h"
#include "gdb_string.h"
#include "frame.h"
#include "symtab.h"
#include "objfiles.h"
#include "inferior.h"
#include "infcall.h"

#include <dl.h>
#include <machine/save_state.h>

/* Forward declarations.  */
extern void _initialize_hppa_hpux_tdep (void);
extern initialize_file_ftype _initialize_hppa_hpux_tdep;

typedef struct
  {
    struct minimal_symbol *msym;
    CORE_ADDR solib_handle;
    CORE_ADDR return_val;
  }
args_for_find_stub;

/* This is declared in symtab.c; set to 1 in hp-symtab-read.c */
extern int hp_som_som_object_present;

/* In breakpoint.c */
extern int exception_catchpoints_are_fragile;

/* FIXME: brobecker 2002-12-25.  The following functions will eventually
   become static, after the multiarching conversion is done.  */
int hppa_hpux_pc_in_sigtramp (CORE_ADDR pc, char *name);
void hppa32_hpux_frame_saved_pc_in_sigtramp (struct frame_info *fi,
                                             CORE_ADDR *tmp);
void hppa32_hpux_frame_base_before_sigtramp (struct frame_info *fi,
                                             CORE_ADDR *tmp);
void hppa32_hpux_frame_find_saved_regs_in_sigtramp (struct frame_info *fi,
                                                    CORE_ADDR *fsr);
void hppa64_hpux_frame_saved_pc_in_sigtramp (struct frame_info *fi,
                                             CORE_ADDR *tmp);
void hppa64_hpux_frame_base_before_sigtramp (struct frame_info *fi,
                                             CORE_ADDR *tmp);
void hppa64_hpux_frame_find_saved_regs_in_sigtramp (struct frame_info *fi,
                                                    CORE_ADDR *fsr);

int
hppa_hpux_pc_in_sigtramp (CORE_ADDR pc, char *name)
{
  /* Actually, for a PA running HPUX the kernel calls the signal handler
     without an intermediate trampoline.  Luckily the kernel always sets
     the return pointer for the signal handler to point to _sigreturn.  */
  return (name && (strcmp ("_sigreturn", name) == 0));
}

/* For hppa32_hpux_frame_saved_pc_in_sigtramp, 
   hppa32_hpux_frame_base_before_sigtramp and
   hppa32_hpux_frame_find_saved_regs_in_sigtramp:

   The signal context structure pointer is always saved at the base
   of the frame which "calls" the signal handler.  We only want to find
   the hardware save state structure, which lives 10 32bit words into
   sigcontext structure.

   Within the hardware save state structure, registers are found in the
   same order as the register numbers in GDB.

   At one time we peeked at %r31 rather than the PC queues to determine
   what instruction took the fault.  This was done on purpose, but I don't
   remember why.  Looking at the PC queues is really the right way, and
   I don't remember why that didn't work when this code was originally
   written.  */

void
hppa32_hpux_frame_saved_pc_in_sigtramp (struct frame_info *fi, CORE_ADDR *tmp)
{
  *tmp = read_memory_integer (get_frame_base (fi) + (43 * 4), 4);
}

void
hppa32_hpux_frame_base_before_sigtramp (struct frame_info *fi,
                                        CORE_ADDR *tmp)
{
  *tmp = read_memory_integer (get_frame_base (fi) + (40 * 4), 4);
}

void
hppa32_hpux_frame_find_saved_regs_in_sigtramp (struct frame_info *fi,
					       CORE_ADDR *fsr)
{
  int i;
  const CORE_ADDR tmp = get_frame_base (fi) + (10 * 4);

  for (i = 0; i < NUM_REGS; i++)
    {
      if (i == SP_REGNUM)
	fsr[SP_REGNUM] = read_memory_integer (tmp + SP_REGNUM * 4, 4);
      else
	fsr[i] = tmp + i * 4;
    }
}

/* For hppa64_hpux_frame_saved_pc_in_sigtramp, 
   hppa64_hpux_frame_base_before_sigtramp and
   hppa64_hpux_frame_find_saved_regs_in_sigtramp:

   These functions are the PA64 ABI equivalents of the 32bits counterparts
   above. See the comments there.

   For PA64, the save_state structure is at an offset of 24 32-bit words
   from the sigcontext structure. The 64 bit general registers are at an
   offset of 640 bytes from the beginning of the save_state structure,
   and the floating pointer register are at an offset of 256 bytes from
   the beginning of the save_state structure.  */

void
hppa64_hpux_frame_saved_pc_in_sigtramp (struct frame_info *fi, CORE_ADDR *tmp)
{
  *tmp = read_memory_integer
           (get_frame_base (fi) + (24 * 4) + 640 + (33 * 8), 8);
}

void
hppa64_hpux_frame_base_before_sigtramp (struct frame_info *fi,
                                        CORE_ADDR *tmp)
{
  *tmp = read_memory_integer
           (get_frame_base (fi) + (24 * 4) + 640 + (30 * 8), 8);
}

void
hppa64_hpux_frame_find_saved_regs_in_sigtramp (struct frame_info *fi,
					       CORE_ADDR *fsr)
{
  int i;
  const CORE_ADDR tmp1 = get_frame_base (fi) + (24 * 4) + 640;
  const CORE_ADDR tmp2 = get_frame_base (fi) + (24 * 4) + 256;

  for (i = 0; i < NUM_REGS; i++)
    {
      if (i == SP_REGNUM)
        fsr[SP_REGNUM] = read_memory_integer (tmp1 + SP_REGNUM * 8, 8);
      else if (i >= FP0_REGNUM)
        fsr[i] = tmp2 + (i - FP0_REGNUM) * 8;
      else
        fsr[i] = tmp1 + i * 8;
    }
}

/* Exception handling support for the HP-UX ANSI C++ compiler.
   The compiler (aCC) provides a callback for exception events;
   GDB can set a breakpoint on this callback and find out what
   exception event has occurred. */

/* The name of the hook to be set to point to the callback function */
static char HP_ACC_EH_notify_hook[] = "__eh_notify_hook";
/* The name of the function to be used to set the hook value */
static char HP_ACC_EH_set_hook_value[] = "__eh_set_hook_value";
/* The name of the callback function in end.o */
static char HP_ACC_EH_notify_callback[] = "__d_eh_notify_callback";
/* Name of function in end.o on which a break is set (called by above) */
static char HP_ACC_EH_break[] = "__d_eh_break";
/* Name of flag (in end.o) that enables catching throws */
static char HP_ACC_EH_catch_throw[] = "__d_eh_catch_throw";
/* Name of flag (in end.o) that enables catching catching */
static char HP_ACC_EH_catch_catch[] = "__d_eh_catch_catch";
/* The enum used by aCC */
typedef enum
  {
    __EH_NOTIFY_THROW,
    __EH_NOTIFY_CATCH
  }
__eh_notification;

/* Is exception-handling support available with this executable? */
static int hp_cxx_exception_support = 0;
/* Has the initialize function been run? */
int hp_cxx_exception_support_initialized = 0;
/* Similar to above, but imported from breakpoint.c -- non-target-specific */
extern int exception_support_initialized;
/* Address of __eh_notify_hook */
static CORE_ADDR eh_notify_hook_addr = 0;
/* Address of __d_eh_notify_callback */
static CORE_ADDR eh_notify_callback_addr = 0;
/* Address of __d_eh_break */
static CORE_ADDR eh_break_addr = 0;
/* Address of __d_eh_catch_catch */
static CORE_ADDR eh_catch_catch_addr = 0;
/* Address of __d_eh_catch_throw */
static CORE_ADDR eh_catch_throw_addr = 0;
/* Sal for __d_eh_break */
static struct symtab_and_line *break_callback_sal = 0;

/* Code in end.c expects __d_pid to be set in the inferior,
   otherwise __d_eh_notify_callback doesn't bother to call
   __d_eh_break!  So we poke the pid into this symbol
   ourselves.
   0 => success
   1 => failure  */
int
setup_d_pid_in_inferior (void)
{
  CORE_ADDR anaddr;
  struct minimal_symbol *msymbol;
  char buf[4];			/* FIXME 32x64? */

  /* Slam the pid of the process into __d_pid; failing is only a warning!  */
  msymbol = lookup_minimal_symbol ("__d_pid", NULL, symfile_objfile);
  if (msymbol == NULL)
    {
      warning ("Unable to find __d_pid symbol in object file.");
      warning ("Suggest linking executable with -g (links in /opt/langtools/lib/end.o).");
      return 1;
    }

  anaddr = SYMBOL_VALUE_ADDRESS (msymbol);
  store_unsigned_integer (buf, 4, PIDGET (inferior_ptid)); /* FIXME 32x64? */
  if (target_write_memory (anaddr, buf, 4))	/* FIXME 32x64? */
    {
      warning ("Unable to write __d_pid");
      warning ("Suggest linking executable with -g (links in /opt/langtools/lib/end.o).");
      return 1;
    }
  return 0;
}

/* elz: Used to lookup a symbol in the shared libraries.
   This function calls shl_findsym, indirectly through a
   call to __d_shl_get. __d_shl_get is in end.c, which is always
   linked in by the hp compilers/linkers. 
   The call to shl_findsym cannot be made directly because it needs
   to be active in target address space. 
   inputs: - minimal symbol pointer for the function we want to look up
   - address in target space of the descriptor for the library
   where we want to look the symbol up.
   This address is retrieved using the 
   som_solib_get_solib_by_pc function (somsolib.c). 
   output: - real address in the library of the function.          
   note: the handle can be null, in which case shl_findsym will look for
   the symbol in all the loaded shared libraries.
   files to look at if you need reference on this stuff:
   dld.c, dld_shl_findsym.c
   end.c
   man entry for shl_findsym */

CORE_ADDR
find_stub_with_shl_get (struct minimal_symbol *function, CORE_ADDR handle)
{
  struct symbol *get_sym, *symbol2;
  struct minimal_symbol *buff_minsym, *msymbol;
  struct type *ftype;
  struct value **args;
  struct value *funcval;
  struct value *val;

  int x, namelen, err_value, tmp = -1;
  CORE_ADDR endo_buff_addr, value_return_addr, errno_return_addr;
  CORE_ADDR stub_addr;


  args = alloca (sizeof (struct value *) * 8);		/* 6 for the arguments and one null one??? */
  funcval = find_function_in_inferior ("__d_shl_get");
  get_sym = lookup_symbol ("__d_shl_get", NULL, VAR_DOMAIN, NULL, NULL);
  buff_minsym = lookup_minimal_symbol ("__buffer", NULL, NULL);
  msymbol = lookup_minimal_symbol ("__shldp", NULL, NULL);
  symbol2 = lookup_symbol ("__shldp", NULL, VAR_DOMAIN, NULL, NULL);
  endo_buff_addr = SYMBOL_VALUE_ADDRESS (buff_minsym);
  namelen = strlen (DEPRECATED_SYMBOL_NAME (function));
  value_return_addr = endo_buff_addr + namelen;
  ftype = check_typedef (SYMBOL_TYPE (get_sym));

  /* do alignment */
  if ((x = value_return_addr % 64) != 0)
    value_return_addr = value_return_addr + 64 - x;

  errno_return_addr = value_return_addr + 64;


  /* set up stuff needed by __d_shl_get in buffer in end.o */

  target_write_memory (endo_buff_addr, DEPRECATED_SYMBOL_NAME (function), namelen);

  target_write_memory (value_return_addr, (char *) &tmp, 4);

  target_write_memory (errno_return_addr, (char *) &tmp, 4);

  target_write_memory (SYMBOL_VALUE_ADDRESS (msymbol),
		       (char *) &handle, 4);

  /* now prepare the arguments for the call */

  args[0] = value_from_longest (TYPE_FIELD_TYPE (ftype, 0), 12);
  args[1] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 1), SYMBOL_VALUE_ADDRESS (msymbol));
  args[2] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 2), endo_buff_addr);
  args[3] = value_from_longest (TYPE_FIELD_TYPE (ftype, 3), TYPE_PROCEDURE);
  args[4] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 4), value_return_addr);
  args[5] = value_from_pointer (TYPE_FIELD_TYPE (ftype, 5), errno_return_addr);

  /* now call the function */

  val = call_function_by_hand (funcval, 6, args);

  /* now get the results */

  target_read_memory (errno_return_addr, (char *) &err_value, sizeof (err_value));

  target_read_memory (value_return_addr, (char *) &stub_addr, sizeof (stub_addr));
  if (stub_addr <= 0)
    error ("call to __d_shl_get failed, error code is %d", err_value);

  return (stub_addr);
}

/* Cover routine for find_stub_with_shl_get to pass to catch_errors */
static int
cover_find_stub_with_shl_get (void *args_untyped)
{
  args_for_find_stub *args = args_untyped;
  args->return_val = find_stub_with_shl_get (args->msym, args->solib_handle);
  return 0;
}

/* Initialize exception catchpoint support by looking for the
   necessary hooks/callbacks in end.o, etc., and set the hook value to
   point to the required debug function

   Return 0 => failure
   1 => success          */

static int
initialize_hp_cxx_exception_support (void)
{
  struct symtabs_and_lines sals;
  struct cleanup *old_chain;
  struct cleanup *canonical_strings_chain = NULL;
  int i;
  char *addr_start;
  char *addr_end = NULL;
  char **canonical = (char **) NULL;
  int thread = -1;
  struct symbol *sym = NULL;
  struct minimal_symbol *msym = NULL;
  struct objfile *objfile;
  asection *shlib_info;

  /* Detect and disallow recursion.  On HP-UX with aCC, infinite
     recursion is a possibility because finding the hook for exception
     callbacks involves making a call in the inferior, which means
     re-inserting breakpoints which can re-invoke this code */

  static int recurse = 0;
  if (recurse > 0)
    {
      hp_cxx_exception_support_initialized = 0;
      exception_support_initialized = 0;
      return 0;
    }

  hp_cxx_exception_support = 0;

  /* First check if we have seen any HP compiled objects; if not,
     it is very unlikely that HP's idiosyncratic callback mechanism
     for exception handling debug support will be available!
     This will percolate back up to breakpoint.c, where our callers
     will decide to try the g++ exception-handling support instead. */
  if (!hp_som_som_object_present)
    return 0;

  /* We have a SOM executable with SOM debug info; find the hooks */

  /* First look for the notify hook provided by aCC runtime libs */
  /* If we find this symbol, we conclude that the executable must
     have HP aCC exception support built in.  If this symbol is not
     found, even though we're a HP SOM-SOM file, we may have been
     built with some other compiler (not aCC).  This results percolates
     back up to our callers in breakpoint.c which can decide to
     try the g++ style of exception support instead.
     If this symbol is found but the other symbols we require are
     not found, there is something weird going on, and g++ support
     should *not* be tried as an alternative.

     ASSUMPTION: Only HP aCC code will have __eh_notify_hook defined.  
     ASSUMPTION: HP aCC and g++ modules cannot be linked together. */

  /* libCsup has this hook; it'll usually be non-debuggable */
  msym = lookup_minimal_symbol (HP_ACC_EH_notify_hook, NULL, NULL);
  if (msym)
    {
      eh_notify_hook_addr = SYMBOL_VALUE_ADDRESS (msym);
      hp_cxx_exception_support = 1;
    }
  else
    {
      warning ("Unable to find exception callback hook (%s).", HP_ACC_EH_notify_hook);
      warning ("Executable may not have been compiled debuggable with HP aCC.");
      warning ("GDB will be unable to intercept exception events.");
      eh_notify_hook_addr = 0;
      hp_cxx_exception_support = 0;
      return 0;
    }

  /* Next look for the notify callback routine in end.o */
  /* This is always available in the SOM symbol dictionary if end.o is linked in */
  msym = lookup_minimal_symbol (HP_ACC_EH_notify_callback, NULL, NULL);
  if (msym)
    {
      eh_notify_callback_addr = SYMBOL_VALUE_ADDRESS (msym);
      hp_cxx_exception_support = 1;
    }
  else
    {
      warning ("Unable to find exception callback routine (%s).", HP_ACC_EH_notify_callback);
      warning ("Suggest linking executable with -g (links in /opt/langtools/lib/end.o).");
      warning ("GDB will be unable to intercept exception events.");
      eh_notify_callback_addr = 0;
      return 0;
    }

#ifndef GDB_TARGET_IS_HPPA_20W
  /* Check whether the executable is dynamically linked or archive bound */
  /* With an archive-bound executable we can use the raw addresses we find
     for the callback function, etc. without modification. For an executable
     with shared libraries, we have to do more work to find the plabel, which
     can be the target of a call through $$dyncall from the aCC runtime support
     library (libCsup) which is linked shared by default by aCC. */
  /* This test below was copied from somsolib.c/somread.c.  It may not be a very
     reliable one to test that an executable is linked shared. pai/1997-07-18 */
  shlib_info = bfd_get_section_by_name (symfile_objfile->obfd, "$SHLIB_INFO$");
  if (shlib_info && (bfd_section_size (symfile_objfile->obfd, shlib_info) != 0))
    {
      /* The minsym we have has the local code address, but that's not the
         plabel that can be used by an inter-load-module call. */
      /* Find solib handle for main image (which has end.o), and use that
         and the min sym as arguments to __d_shl_get() (which does the equivalent
         of shl_findsym()) to find the plabel. */

      args_for_find_stub args;
      static char message[] = "Error while finding exception callback hook:\n";

      args.solib_handle = som_solib_get_solib_by_pc (eh_notify_callback_addr);
      args.msym = msym;
      args.return_val = 0;

      recurse++;
      catch_errors (cover_find_stub_with_shl_get, &args, message,
		    RETURN_MASK_ALL);
      eh_notify_callback_addr = args.return_val;
      recurse--;

      exception_catchpoints_are_fragile = 1;

      if (!eh_notify_callback_addr)
	{
	  /* We can get here either if there is no plabel in the export list
	     for the main image, or if something strange happened (?) */
	  warning ("Couldn't find a plabel (indirect function label) for the exception callback.");
	  warning ("GDB will not be able to intercept exception events.");
	  return 0;
	}
    }
  else
    exception_catchpoints_are_fragile = 0;
#endif

  /* Now, look for the breakpointable routine in end.o */
  /* This should also be available in the SOM symbol dict. if end.o linked in */
  msym = lookup_minimal_symbol (HP_ACC_EH_break, NULL, NULL);
  if (msym)
    {
      eh_break_addr = SYMBOL_VALUE_ADDRESS (msym);
      hp_cxx_exception_support = 1;
    }
  else
    {
      warning ("Unable to find exception callback routine to set breakpoint (%s).", HP_ACC_EH_break);
      warning ("Suggest linking executable with -g (link in /opt/langtools/lib/end.o).");
      warning ("GDB will be unable to intercept exception events.");
      eh_break_addr = 0;
      return 0;
    }

  /* Next look for the catch enable flag provided in end.o */
  sym = lookup_symbol (HP_ACC_EH_catch_catch, (struct block *) NULL,
		       VAR_DOMAIN, 0, (struct symtab **) NULL);
  if (sym)			/* sometimes present in debug info */
    {
      eh_catch_catch_addr = SYMBOL_VALUE_ADDRESS (sym);
      hp_cxx_exception_support = 1;
    }
  else
    /* otherwise look in SOM symbol dict. */
    {
      msym = lookup_minimal_symbol (HP_ACC_EH_catch_catch, NULL, NULL);
      if (msym)
	{
	  eh_catch_catch_addr = SYMBOL_VALUE_ADDRESS (msym);
	  hp_cxx_exception_support = 1;
	}
      else
	{
	  warning ("Unable to enable interception of exception catches.");
	  warning ("Executable may not have been compiled debuggable with HP aCC.");
	  warning ("Suggest linking executable with -g (link in /opt/langtools/lib/end.o).");
	  return 0;
	}
    }

  /* Next look for the catch enable flag provided end.o */
  sym = lookup_symbol (HP_ACC_EH_catch_catch, (struct block *) NULL,
		       VAR_DOMAIN, 0, (struct symtab **) NULL);
  if (sym)			/* sometimes present in debug info */
    {
      eh_catch_throw_addr = SYMBOL_VALUE_ADDRESS (sym);
      hp_cxx_exception_support = 1;
    }
  else
    /* otherwise look in SOM symbol dict. */
    {
      msym = lookup_minimal_symbol (HP_ACC_EH_catch_throw, NULL, NULL);
      if (msym)
	{
	  eh_catch_throw_addr = SYMBOL_VALUE_ADDRESS (msym);
	  hp_cxx_exception_support = 1;
	}
      else
	{
	  warning ("Unable to enable interception of exception throws.");
	  warning ("Executable may not have been compiled debuggable with HP aCC.");
	  warning ("Suggest linking executable with -g (link in /opt/langtools/lib/end.o).");
	  return 0;
	}
    }

  /* Set the flags */
  hp_cxx_exception_support = 2;	/* everything worked so far */
  hp_cxx_exception_support_initialized = 1;
  exception_support_initialized = 1;

  return 1;
}

/* Target operation for enabling or disabling interception of
   exception events.
   KIND is either EX_EVENT_THROW or EX_EVENT_CATCH
   ENABLE is either 0 (disable) or 1 (enable).
   Return value is NULL if no support found;
   -1 if something went wrong,
   or a pointer to a symtab/line struct if the breakpointable
   address was found. */

struct symtab_and_line *
child_enable_exception_callback (enum exception_event_kind kind, int enable)
{
  char buf[4];

  if (!exception_support_initialized || !hp_cxx_exception_support_initialized)
    if (!initialize_hp_cxx_exception_support ())
      return NULL;

  switch (hp_cxx_exception_support)
    {
    case 0:
      /* Assuming no HP support at all */
      return NULL;
    case 1:
      /* HP support should be present, but something went wrong */
      return (struct symtab_and_line *) -1;	/* yuck! */
      /* there may be other cases in the future */
    }

  /* Set the EH hook to point to the callback routine */
  store_unsigned_integer (buf, 4, enable ? eh_notify_callback_addr : 0);	/* FIXME 32x64 problem */
  /* pai: (temp) FIXME should there be a pack operation first? */
  if (target_write_memory (eh_notify_hook_addr, buf, 4))	/* FIXME 32x64 problem */
    {
      warning ("Could not write to target memory for exception event callback.");
      warning ("Interception of exception events may not work.");
      return (struct symtab_and_line *) -1;
    }
  if (enable)
    {
      /* Ensure that __d_pid is set up correctly -- end.c code checks this. :-( */
      if (PIDGET (inferior_ptid) > 0)
	{
	  if (setup_d_pid_in_inferior ())
	    return (struct symtab_and_line *) -1;
	}
      else
	{
	  warning ("Internal error: Invalid inferior pid?  Cannot intercept exception events.");
	  return (struct symtab_and_line *) -1;
	}
    }

  switch (kind)
    {
    case EX_EVENT_THROW:
      store_unsigned_integer (buf, 4, enable ? 1 : 0);
      if (target_write_memory (eh_catch_throw_addr, buf, 4))	/* FIXME 32x64? */
	{
	  warning ("Couldn't enable exception throw interception.");
	  return (struct symtab_and_line *) -1;
	}
      break;
    case EX_EVENT_CATCH:
      store_unsigned_integer (buf, 4, enable ? 1 : 0);
      if (target_write_memory (eh_catch_catch_addr, buf, 4))	/* FIXME 32x64? */
	{
	  warning ("Couldn't enable exception catch interception.");
	  return (struct symtab_and_line *) -1;
	}
      break;
    default:
      error ("Request to enable unknown or unsupported exception event.");
    }

  /* Copy break address into new sal struct, malloc'ing if needed. */
  if (!break_callback_sal)
    {
      break_callback_sal = (struct symtab_and_line *) xmalloc (sizeof (struct symtab_and_line));
    }
  init_sal (break_callback_sal);
  break_callback_sal->symtab = NULL;
  break_callback_sal->pc = eh_break_addr;
  break_callback_sal->line = 0;
  break_callback_sal->end = eh_break_addr;

  return break_callback_sal;
}

/* Record some information about the current exception event */
static struct exception_event_record current_ex_event;
/* Convenience struct */
static struct symtab_and_line null_symtab_and_line =
{NULL, 0, 0, 0};

/* Report current exception event.  Returns a pointer to a record
   that describes the kind of the event, where it was thrown from,
   and where it will be caught.  More information may be reported
   in the future */
struct exception_event_record *
child_get_current_exception_event (void)
{
  CORE_ADDR event_kind;
  CORE_ADDR throw_addr;
  CORE_ADDR catch_addr;
  struct frame_info *fi, *curr_frame;
  int level = 1;

  curr_frame = get_current_frame ();
  if (!curr_frame)
    return (struct exception_event_record *) NULL;

  /* Go up one frame to __d_eh_notify_callback, because at the
     point when this code is executed, there's garbage in the
     arguments of __d_eh_break. */
  fi = find_relative_frame (curr_frame, &level);
  if (level != 0)
    return (struct exception_event_record *) NULL;

  select_frame (fi);

  /* Read in the arguments */
  /* __d_eh_notify_callback() is called with 3 arguments:
     1. event kind catch or throw
     2. the target address if known
     3. a flag -- not sure what this is. pai/1997-07-17 */
  event_kind = read_register (ARG0_REGNUM);
  catch_addr = read_register (ARG1_REGNUM);

  /* Now go down to a user frame */
  /* For a throw, __d_eh_break is called by
     __d_eh_notify_callback which is called by
     __notify_throw which is called
     from user code.
     For a catch, __d_eh_break is called by
     __d_eh_notify_callback which is called by
     <stackwalking stuff> which is called by
     __throw__<stuff> or __rethrow_<stuff> which is called
     from user code. */
  /* FIXME: Don't use such magic numbers; search for the frames */
  level = (event_kind == EX_EVENT_THROW) ? 3 : 4;
  fi = find_relative_frame (curr_frame, &level);
  if (level != 0)
    return (struct exception_event_record *) NULL;

  select_frame (fi);
  throw_addr = get_frame_pc (fi);

  /* Go back to original (top) frame */
  select_frame (curr_frame);

  current_ex_event.kind = (enum exception_event_kind) event_kind;
  current_ex_event.throw_sal = find_pc_line (throw_addr, 1);
  current_ex_event.catch_sal = find_pc_line (catch_addr, 1);

  return &current_ex_event;
}

static void
hppa_hpux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  set_gdbarch_deprecated_pc_in_sigtramp (gdbarch, hppa_hpux_pc_in_sigtramp);
}

static void
hppa_hpux_som_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  hppa_hpux_init_abi (info, gdbarch);
}

static void
hppa_hpux_elf_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  hppa_hpux_init_abi (info, gdbarch);
}

void
_initialize_hppa_hpux_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_hppa, 0, GDB_OSABI_HPUX_SOM,
                          hppa_hpux_som_init_abi);
  gdbarch_register_osabi (bfd_arch_hppa, bfd_mach_hppa20w, GDB_OSABI_HPUX_ELF,
                          hppa_hpux_elf_init_abi);
}
