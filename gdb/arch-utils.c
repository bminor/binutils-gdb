/* Dynamic architecture support for GDB, the GNU debugger.
   Copyright 1998-1999, Free Software Foundation, Inc.

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

#include "defs.h"

#if GDB_MULTI_ARCH
#include "gdbcmd.h"
#include "inferior.h"		/* enum CALL_DUMMY_LOCATION et.al. */
#else
/* Just include everything in sight so that the every old definition
   of macro is visible. */
#include "gdb_string.h"
#include <ctype.h>
#include "symtab.h"
#include "frame.h"
#include "inferior.h"
#include "breakpoint.h"
#include "gdb_wait.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "target.h"
#include "gdbthread.h"
#include "annotate.h"
#include "symfile.h"		/* for overlay functions */
#endif

#include "version.h"

#include "floatformat.h"

/* Convenience macro for allocting typesafe memory. */

#ifndef XMALLOC
#define XMALLOC(TYPE) (TYPE*) xmalloc (sizeof (TYPE))
#endif


/* Use the program counter to determine the contents and size
   of a breakpoint instruction.  If no target-dependent macro
   BREAKPOINT_FROM_PC has been defined to implement this function,
   assume that the breakpoint doesn't depend on the PC, and
   use the values of the BIG_BREAKPOINT and LITTLE_BREAKPOINT macros.
   Return a pointer to a string of bytes that encode a breakpoint
   instruction, stores the length of the string to *lenptr,
   and optionally adjust the pc to point to the correct memory location
   for inserting the breakpoint.  */

unsigned char *
legacy_breakpoint_from_pc (CORE_ADDR * pcptr, int *lenptr)
{
  /* {BIG_,LITTLE_}BREAKPOINT is the sequence of bytes we insert for a
     breakpoint.  On some machines, breakpoints are handled by the
     target environment and we don't have to worry about them here.  */
#ifdef BIG_BREAKPOINT
  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    {
      static unsigned char big_break_insn[] = BIG_BREAKPOINT;
      *lenptr = sizeof (big_break_insn);
      return big_break_insn;
    }
#endif
#ifdef LITTLE_BREAKPOINT
  if (TARGET_BYTE_ORDER != BIG_ENDIAN)
    {
      static unsigned char little_break_insn[] = LITTLE_BREAKPOINT;
      *lenptr = sizeof (little_break_insn);
      return little_break_insn;
    }
#endif
#ifdef BREAKPOINT
  {
    static unsigned char break_insn[] = BREAKPOINT;
    *lenptr = sizeof (break_insn);
    return break_insn;
  }
#endif
  *lenptr = 0;
  return NULL;
}

int
generic_frameless_function_invocation_not (struct frame_info *fi)
{
  return 0;
}

int
generic_return_value_on_stack_not (struct type *type)
{
  return 0;
}

char *
legacy_register_name (int i)
{
#ifdef REGISTER_NAMES
  static char *names[] = REGISTER_NAMES;
  if (i < 0 || i >= (sizeof (names) / sizeof (*names)))
    return NULL;
  else
    return names[i];
#else
  internal_error ("legacy_register_name: called.");
  return NULL;
#endif
}

#if defined (CALL_DUMMY)
LONGEST legacy_call_dummy_words[] = CALL_DUMMY;
#else
LONGEST legacy_call_dummy_words[1];
#endif
int legacy_sizeof_call_dummy_words = sizeof (legacy_call_dummy_words);

void
generic_remote_translate_xfer_address (CORE_ADDR gdb_addr, int gdb_len,
				       CORE_ADDR * rem_addr, int *rem_len)
{
  *rem_addr = gdb_addr;
  *rem_len = gdb_len;
}

int
generic_prologue_frameless_p (CORE_ADDR ip)
{
#ifdef SKIP_PROLOGUE_FRAMELESS_P
  return ip == SKIP_PROLOGUE_FRAMELESS_P (ip);
#else
  return ip == SKIP_PROLOGUE (ip);
#endif
}


/* Helper functions for INNER_THAN */

int
core_addr_lessthan (lhs, rhs)
     CORE_ADDR lhs;
     CORE_ADDR rhs;
{
  return (lhs < rhs);
}

int
core_addr_greaterthan (lhs, rhs)
     CORE_ADDR lhs;
     CORE_ADDR rhs;
{
  return (lhs > rhs);
}


/* Helper functions for TARGET_{FLOAT,DOUBLE}_FORMAT */

const struct floatformat *
default_float_format (struct gdbarch *gdbarch)
{
#if GDB_MULTI_ARCH
  int byte_order = gdbarch_byte_order (gdbarch);
#else
  int byte_order = TARGET_BYTE_ORDER;
#endif
  switch (byte_order)
    {
    case BIG_ENDIAN:
      return &floatformat_ieee_single_big;
    case LITTLE_ENDIAN:
      return &floatformat_ieee_single_little;
    default:
      internal_error ("default_float_format: bad byte order");
    }
}


const struct floatformat *
default_double_format (struct gdbarch *gdbarch)
{
#if GDB_MULTI_ARCH
  int byte_order = gdbarch_byte_order (gdbarch);
#else
  int byte_order = TARGET_BYTE_ORDER;
#endif
  switch (byte_order)
    {
    case BIG_ENDIAN:
      return &floatformat_ieee_double_big;
    case LITTLE_ENDIAN:
      return &floatformat_ieee_double_little;
    default:
      internal_error ("default_double_format: bad byte order");
    }
}

/* Misc helper functions for targets. */

int
frame_num_args_unknown (fi)
     struct frame_info *fi;
{
  return -1;
}


int
generic_register_convertible_not (num)
     int num;
{
  return 0;
}
  

/* Functions to manipulate the endianness of the target.  */

#ifdef TARGET_BYTE_ORDER_SELECTABLE
/* compat - Catch old targets that expect a selectable byte-order to
   default to BIG_ENDIAN */
#ifndef TARGET_BYTE_ORDER_DEFAULT
#define TARGET_BYTE_ORDER_DEFAULT BIG_ENDIAN
#endif
#endif
#if !TARGET_BYTE_ORDER_SELECTABLE_P
#ifndef TARGET_BYTE_ORDER_DEFAULT
/* compat - Catch old non byte-order selectable targets that do not
   define TARGET_BYTE_ORDER_DEFAULT and instead expect
   TARGET_BYTE_ORDER to be used as the default.  For targets that
   defined neither TARGET_BYTE_ORDER nor TARGET_BYTE_ORDER_DEFAULT the
   below will get a strange compiler warning. */
#define TARGET_BYTE_ORDER_DEFAULT TARGET_BYTE_ORDER
#endif
#endif
#ifndef TARGET_BYTE_ORDER_DEFAULT
#define TARGET_BYTE_ORDER_DEFAULT BIG_ENDIAN /* arbitrary */
#endif
/* ``target_byte_order'' is only used when non- multi-arch.
   Multi-arch targets obtain the current byte order using
   TARGET_BYTE_ORDER which is controlled by gdbarch.*. */
int target_byte_order = TARGET_BYTE_ORDER_DEFAULT;
int target_byte_order_auto = 1;

static const char endian_big[] = "big";
static const char endian_little[] = "little";
static const char endian_auto[] = "auto";
static const char *endian_enum[] =
{
  endian_big,
  endian_little,
  endian_auto,
  NULL,
};
static const char *set_endian_string;

/* Called by ``show endian''.  */

static void
show_endian (char *args, int from_tty)
{
  if (TARGET_BYTE_ORDER_AUTO)
    printf_unfiltered ("The target endianness is set automatically (currently %s endian)\n",
		       (TARGET_BYTE_ORDER == BIG_ENDIAN ? "big" : "little"));
  else
    printf_unfiltered ("The target is assumed to be %s endian\n",
		       (TARGET_BYTE_ORDER == BIG_ENDIAN ? "big" : "little"));
}

static void
set_endian (char *ignore_args, int from_tty, struct cmd_list_element *c)
{
  if (!TARGET_BYTE_ORDER_SELECTABLE_P)
    {
      printf_unfiltered ("Byte order is not selectable.");
    }
  else if (set_endian_string == endian_auto)
    {
      target_byte_order_auto = 1;
    }
  else if (set_endian_string == endian_little)
    {
      target_byte_order_auto = 0;
      if (GDB_MULTI_ARCH)
	{
	  struct gdbarch_info info;
	  memset (&info, 0, sizeof info);
	  info.byte_order = LITTLE_ENDIAN;
	  gdbarch_update (info);
	}
      else
	{
	  target_byte_order = LITTLE_ENDIAN;
	}
    }
  else if (set_endian_string == endian_big)
    {
      target_byte_order_auto = 0;
      if (GDB_MULTI_ARCH)
	{
	  struct gdbarch_info info;
	  memset (&info, 0, sizeof info);
	  info.byte_order = BIG_ENDIAN;
	  gdbarch_update (info);
	}
      else
	{
	  target_byte_order = BIG_ENDIAN;
	}
    }
  else
    internal_error ("set_endian: bad value");
  show_endian (NULL, from_tty);
}

/* Set the endianness from a BFD.  */

static void
set_endian_from_file (bfd *abfd)
{
  if (GDB_MULTI_ARCH)
    internal_error ("set_endian_from_file: not for multi-arch");
  if (TARGET_BYTE_ORDER_SELECTABLE_P)
    {
      int want;
      
      if (bfd_big_endian (abfd))
	want = BIG_ENDIAN;
      else
	want = LITTLE_ENDIAN;
      if (TARGET_BYTE_ORDER_AUTO)
	target_byte_order = want;
      else if (TARGET_BYTE_ORDER != want)
	warning ("%s endian file does not match %s endian target.",
		 want == BIG_ENDIAN ? "big" : "little",
		 TARGET_BYTE_ORDER == BIG_ENDIAN ? "big" : "little");
    }
  else
    {
      if (bfd_big_endian (abfd)
	  ? TARGET_BYTE_ORDER != BIG_ENDIAN
	  : TARGET_BYTE_ORDER == BIG_ENDIAN)
	warning ("%s endian file does not match %s endian target.",
		 bfd_big_endian (abfd) ? "big" : "little",
		 TARGET_BYTE_ORDER == BIG_ENDIAN ? "big" : "little");
    }
}


/* Functions to manipulate the architecture of the target */

enum set_arch { set_arch_auto, set_arch_manual };

int target_architecture_auto = 1;

const char *set_architecture_string;

/* Old way of changing the current architecture. */

extern const struct bfd_arch_info bfd_default_arch_struct;
const struct bfd_arch_info *target_architecture = &bfd_default_arch_struct;
int (*target_architecture_hook) (const struct bfd_arch_info *ap);

static int
arch_ok (const struct bfd_arch_info *arch)
{
  if (GDB_MULTI_ARCH)
    internal_error ("arch_ok: not multi-arched");
  /* Should be performing the more basic check that the binary is
     compatible with GDB. */
  /* Check with the target that the architecture is valid. */
  return (target_architecture_hook == NULL
	  || target_architecture_hook (arch));
}

static void
set_arch (const struct bfd_arch_info *arch,
          enum set_arch type)
{
  if (GDB_MULTI_ARCH)
    internal_error ("set_arch: not multi-arched");
  switch (type)
    {
    case set_arch_auto:
      if (!arch_ok (arch))
	warning ("Target may not support %s architecture",
		 arch->printable_name);
      target_architecture = arch;
      break;
    case set_arch_manual:
      if (!arch_ok (arch))
	{
	  printf_unfiltered ("Target does not support `%s' architecture.\n",
			     arch->printable_name);
	}
      else
	{
	  target_architecture_auto = 0;
	  target_architecture = arch;
	}
      break;
    }
  if (gdbarch_debug)
    gdbarch_dump (current_gdbarch, gdb_stdlog);
}

/* Set the architecture from arch/machine (deprecated) */

void
set_architecture_from_arch_mach (enum bfd_architecture arch,
				 unsigned long mach)
{
  const struct bfd_arch_info *wanted = bfd_lookup_arch (arch, mach);
  if (GDB_MULTI_ARCH)
    internal_error ("set_architecture_from_arch_mach: not multi-arched");
  if (wanted != NULL)
    set_arch (wanted, set_arch_manual);
  else
    internal_error ("gdbarch: hardwired architecture/machine not reconized");
}

/* Set the architecture from a BFD (deprecated) */

static void
set_architecture_from_file (bfd *abfd)
{
  const struct bfd_arch_info *wanted = bfd_get_arch_info (abfd);
  if (GDB_MULTI_ARCH)
    internal_error ("set_architecture_from_file: not multi-arched");
  if (target_architecture_auto)
    {
      set_arch (wanted, set_arch_auto);
    }
  else if (wanted != target_architecture)
    {
      warning ("%s architecture file may be incompatible with %s target.",
	       wanted->printable_name,
	       target_architecture->printable_name);
    }
}


/* Called if the user enters ``show architecture'' without an
   argument. */

static void
show_architecture (char *args, int from_tty)
{
  const char *arch;
  arch = TARGET_ARCHITECTURE->printable_name;
  if (target_architecture_auto)
    printf_filtered ("The target architecture is set automatically (currently %s)\n", arch);
  else
    printf_filtered ("The target architecture is assumed to be %s\n", arch);
}


/* Called if the user enters ``set architecture'' with or without an
   argument. */

static void
set_architecture (char *ignore_args, int from_tty, struct cmd_list_element *c)
{
  if (strcmp (set_architecture_string, "auto") == 0)
    {
      target_architecture_auto = 1;
    }
  else if (GDB_MULTI_ARCH)
    {
      struct gdbarch_info info;
      memset (&info, 0, sizeof info);
      info.bfd_arch_info = bfd_scan_arch (set_architecture_string);
      if (info.bfd_arch_info == NULL)
	internal_error ("set_architecture: bfd_scan_arch failed");
      if (gdbarch_update (info))
	target_architecture_auto = 0;
      else
	printf_unfiltered ("Architecture `%s' not reconized.\n",
			   set_architecture_string);
    }
  else
    {
      const struct bfd_arch_info *arch
	= bfd_scan_arch (set_architecture_string);
      if (arch == NULL)
	internal_error ("set_architecture: bfd_scan_arch failed");
      set_arch (arch, set_arch_manual);
    }
  show_architecture (NULL, from_tty);
}

/* Called if the user enters ``info architecture'' without an argument. */

static void
info_architecture (char *args, int from_tty)
{
  printf_filtered ("Available architectures are:\n");
  if (GDB_MULTI_ARCH)
    {
      const char **arches = gdbarch_printable_names ();
      const char **arch;
      for (arch = arches; *arch != NULL; arch++)
	{
	  printf_filtered (" %s", *arch);
	}
      free (arches);
    }
  else
    {
      enum bfd_architecture a;
      for (a = bfd_arch_obscure + 1; a < bfd_arch_last; a++)
	{
	  const struct bfd_arch_info *ap;
	  for (ap = bfd_lookup_arch (a, 0);
	       ap != NULL;
	       ap = ap->next)
	    {
	      printf_filtered (" %s", ap->printable_name);
	      ap = ap->next;
	    }
	}
    }
  printf_filtered ("\n");
}

/* Set the dynamic target-system-dependant parameters (architecture,
   byte-order) using information found in the BFD */

void
set_gdbarch_from_file (abfd)
     bfd *abfd;
{
  if (GDB_MULTI_ARCH)
    {
      struct gdbarch_info info;
      memset (&info, 0, sizeof info);
      info.abfd = abfd;
      gdbarch_update (info);
    }
  else
    {
      set_architecture_from_file (abfd);
      set_endian_from_file (abfd);
    }
}

/* Initialize the current architecture.  Update the ``set
   architecture'' command so that it specifies a list of valid
   architectures.  */

#ifdef DEFAULT_BFD_ARCH
extern const bfd_arch_info_type DEFAULT_BFD_ARCH;
static const bfd_arch_info_type *default_bfd_arch = &DEFAULT_BFD_ARCH;
#else
static const bfd_arch_info_type *default_bfd_arch;
#endif

#ifdef DEFAULT_BFD_VEC
extern const bfd_target DEFAULT_BFD_VEC;
static const bfd_target *default_bfd_vec = &DEFAULT_BFD_VEC;
#else
static const bfd_target *default_bfd_vec;
#endif

void
initialize_current_architecture (void)
{
  const char **arches = gdbarch_printable_names ();

  /* determine a default architecture and byte order. */
  struct gdbarch_info info;
  memset (&info, 0, sizeof (info));
  
  /* Find a default architecture. */
  if (info.bfd_arch_info == NULL
      && default_bfd_arch != NULL)
    info.bfd_arch_info = default_bfd_arch;
  if (info.bfd_arch_info == NULL)
    {
      /* Choose the architecture by taking the first one
	 alphabetically. */
      const char *chosen = arches[0];
      const char **arch;
      for (arch = arches; *arch != NULL; arch++)
	{
	  if (strcmp (*arch, chosen) < 0)
	    chosen = *arch;
	}
      if (chosen == NULL)
	internal_error ("initialize_current_architecture: No arch");
      info.bfd_arch_info = bfd_scan_arch (chosen);
      if (info.bfd_arch_info == NULL)
	internal_error ("initialize_current_architecture: Arch not found");
    }

  /* take several guesses at a byte order. */
  /* NB: can't use TARGET_BYTE_ORDER_DEFAULT as its definition is
     forced above. */
  if (info.byte_order == 0
      && default_bfd_vec != NULL)
    {
      /* Extract BFD's default vector's byte order. */
      switch (default_bfd_vec->byteorder)
	{
	case BFD_ENDIAN_BIG:
	  info.byte_order = BIG_ENDIAN;
	  break;
	case BFD_ENDIAN_LITTLE:
	  info.byte_order = LITTLE_ENDIAN;
	  break;
	default:
	  break;
	}
    }
  if (info.byte_order == 0)
    {
      /* look for ``*el-*'' in the target name. */
      const char *chp;
      chp = strchr (target_name, '-');
      if (chp != NULL
	  && chp - 2 >= target_name
	  && strncmp (chp - 2, "el", 2) == 0)
	info.byte_order = LITTLE_ENDIAN;
    }
  if (info.byte_order == 0)
    {
      /* Wire it to big-endian!!! */
      info.byte_order = BIG_ENDIAN;
    }

  if (GDB_MULTI_ARCH)
    {
      gdbarch_update (info);
    }

  /* Create the ``set architecture'' command appending ``auto'' to the
     list of architectures. */
  {
    struct cmd_list_element *c;
    /* Append ``auto''. */
    int nr;
    for (nr = 0; arches[nr] != NULL; nr++);
    arches = xrealloc (arches, sizeof (char*) * (nr + 2));
    arches[nr + 0] = "auto";
    arches[nr + 1] = NULL;
    /* FIXME: add_set_enum_cmd() uses an array of ``char *'' instead
       of ``const char *''.  We just happen to know that the casts are
       safe. */
    c = add_set_enum_cmd ("architecture", class_support,
			  (char **) arches, (char **) &set_architecture_string,
			  "Set architecture of target.",
			  &setlist);
    c->function.sfunc = set_architecture;
    add_alias_cmd ("processor", "architecture", class_support, 1, &setlist);
    /* Don't use set_from_show - need to print both auto/manual and
       current setting. */
    add_cmd ("architecture", class_support, show_architecture,
	     "Show the current target architecture", &showlist);
    c = add_cmd ("architecture", class_support, info_architecture,
		 "List supported target architectures", &infolist);
    deprecate_cmd (c, "set architecture");
  }
}


/* */

extern initialize_file_ftype _initialize_gdbarch_utils;

void
_initialize_gdbarch_utils (void)
{
  struct cmd_list_element *c;
  c = add_set_enum_cmd ("endian", class_support,
			(char **) endian_enum, (char **) &set_endian_string,
			"Set endianness of target.",
			&setlist);
  c->function.sfunc = set_endian;
  /* Don't use set_from_show - need to print both auto/manual and
     current setting. */
  add_cmd ("endian", class_support, show_endian,
	   "Show the current byte-order", &showlist);
}
