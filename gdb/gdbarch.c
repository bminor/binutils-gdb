/* Semi-dynamic architecture support for GDB, the GNU debugger.
   Copyright 1998, Free Software Foundation, Inc.

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
#include "bfd.h"
#include "gdbcmd.h"


/* start-sanitize-carp start-sanitize-vr4xxx */
/* Convenience macro for allocting memory. */

#ifndef XMALLOC
#define XMALLOC(TYPE) (TYPE*) xmalloc (sizeof (TYPE))
#endif

/* end-sanitize-carp end-sanitize-vr4xxx */

/* Non-zero if we want to trace architecture code.  */

#ifndef GDBARCH_DEBUG
#define GDBARCH_DEBUG 0
#endif
int gdbarch_debug = GDBARCH_DEBUG;

/* start-sanitize-carp start-sanitize-vr4xxx */

/* Maintain the struct gdbarch object */

struct gdbarch
{
  /* basic architectural information */
  const struct bfd_arch_info *bfd_arch_info;
  int byte_order;

  /* target specific vector. */
  struct gdbarch_tdep *tdep;

  /* per-architecture data-pointers */
  int nr_data;
  void **data;

  /* per-architecture swap-regions */
  struct gdbarch_swap *swap;

  /* Multi-arch values.

     When adding to the below you must also: declare/define set/get
     value functions; override the corresponding macro in gdbarch.h;
     if zero/NULL is not a suitable default, initialize the field in
     gdbarch_alloc(); confirm that the target updated the value
     correctly in verify_gdbarch(); add a fprintf_unfiltered call to
     gdbarch_update() so that the new field is dumped out; append an
     initial value to the static variable ``default_gdbarch'' (base
     values on the host's c-type system). */

  int long_bit;
  int long_long_bit;
  int ptr_bit;

};


struct gdbarch_tdep *
gdbarch_tdep (gdbarch)
     struct gdbarch *gdbarch;
{
  return gdbarch->tdep;
}

const struct bfd_arch_info *
gdbarch_bfd_arch_info (gdbarch)
     struct gdbarch *gdbarch;
{
  return gdbarch->bfd_arch_info;
}

int
gdbarch_byte_order (gdbarch)
     struct gdbarch *gdbarch;
{
  return gdbarch->byte_order;
}

int
gdbarch_long_bit (gdbarch)
     struct gdbarch *gdbarch;
{
  return gdbarch->long_bit;
}

void
set_gdbarch_long_bit (gdbarch, long_bit)
     struct gdbarch *gdbarch;
     int long_bit;
{
  gdbarch->long_bit = long_bit;
}

int
gdbarch_long_long_bit (gdbarch)
     struct gdbarch *gdbarch;
{
  return gdbarch->long_long_bit;
}

void
set_gdbarch_long_long_bit (gdbarch, long_long_bit)
     struct gdbarch *gdbarch;
     int long_long_bit;
{
  gdbarch->long_long_bit = long_long_bit;
}

int
gdbarch_ptr_bit (gdbarch)
     struct gdbarch *gdbarch;
{
  return gdbarch->ptr_bit;
}

void
set_gdbarch_ptr_bit (gdbarch, ptr_bit)
     struct gdbarch *gdbarch;
     int ptr_bit;
{
  gdbarch->ptr_bit = ptr_bit;
}


/* Ensure that all values in a GDBARCH are reasonable. XXX - should
   this instead return a success/fail indication? */

static void
verify_gdbarch (gdbarch)
     struct gdbarch *gdbarch;
{
  /* fundamental */
  if (gdbarch->byte_order == 0)
    fatal ("verify_gdbarch: byte-order unset");
  if (gdbarch->bfd_arch_info == NULL)
    fatal ("verify_gdbarch: bfd_arch_info unset");
  /* more general */
  if (gdbarch->long_bit == 0)
    fatal ("verify_gdbarch: long_bit invalid");
  if (gdbarch->long_long_bit == 0)
    fatal ("verify_gdbarch: long_long_bit invalid");
  if (gdbarch->ptr_bit == 0)
    fatal ("verify_gdbarch: ptr_bit invalid");
}


/* Keep a registrary of per-architecture data-pointers required by GDB
   modules. */

struct gdbarch_data
{
  int index;
};

struct gdbarch_data_registration
{
  gdbarch_data_ftype *init;
  struct gdbarch_data *data;
  struct gdbarch_data_registration *next;
};

struct gdbarch_data_registrary
{
  int nr;
  struct gdbarch_data_registration *registrations;
};

struct gdbarch_data_registrary gdbarch_data_registrary =
{
  0, NULL,
};

struct gdbarch_data *
register_gdbarch_data (init)
     gdbarch_data_ftype *init;
{
  struct gdbarch_data_registration **curr;
  for (curr = &gdbarch_data_registrary.registrations;
       (*curr) != NULL;
       curr = &(*curr)->next);
  (*curr) = XMALLOC (struct gdbarch_data_registration);
  (*curr)->next = NULL;
  (*curr)->init = init;
  (*curr)->data = XMALLOC (struct gdbarch_data);
  (*curr)->data->index = gdbarch_data_registrary.nr++;
  return (*curr)->data;
}


/* Walk through all the registered users initializing each in turn. */

static void init_gdbarch_data PARAMS ((struct gdbarch *));
static void
init_gdbarch_data (gdbarch)
     struct gdbarch *gdbarch;
{
  struct gdbarch_data_registration *rego;
  gdbarch->nr_data = gdbarch_data_registrary.nr + 1;
  gdbarch->data = xmalloc (sizeof (void*) * gdbarch->nr_data);
  for (rego = gdbarch_data_registrary.registrations;
       rego != NULL;
       rego = rego->next)
    {
      if (rego->data->index < gdbarch->nr_data)
	gdbarch->data[rego->data->index] = rego->init ();
    }
}


/* Return the current value of the specified per-architecture
   data-pointer. */

void *
gdbarch_data (data)
     struct gdbarch_data *data;
{
  if (data->index >= current_gdbarch->nr_data)
    fatal ("gdbarch_data: request for non-existant data.");
  return current_gdbarch->data[data->index];
}



/* Keep a registrary of swaped data required by GDB modules. */

struct gdbarch_swap
{
  void *swap;
  struct gdbarch_swap_registration *source;
  struct gdbarch_swap *next;
};

struct gdbarch_swap_registration
{
  void *data;
  unsigned long sizeof_data;
  gdbarch_swap_ftype *init;
  struct gdbarch_swap_registration *next;
};

struct gdbarch_swap_registrary
{
  int nr;
  struct gdbarch_swap_registration *registrations;
};

struct gdbarch_swap_registrary gdbarch_swap_registrary = 
{
  0, NULL,
};

void
register_gdbarch_swap (data, sizeof_data, init)
     void *data;
     unsigned long sizeof_data;
     gdbarch_swap_ftype *init;
{
  struct gdbarch_swap_registration **rego;
  for (rego = &gdbarch_swap_registrary.registrations;
       (*rego) != NULL;
       rego = &(*rego)->next);
  (*rego) = XMALLOC (struct gdbarch_swap_registration);
  (*rego)->next = NULL;
  (*rego)->init = init;
  (*rego)->data = data;
  (*rego)->sizeof_data = sizeof_data;
}


static void init_gdbarch_swap PARAMS ((struct gdbarch *));
static void
init_gdbarch_swap (gdbarch)
     struct gdbarch *gdbarch;
{
  struct gdbarch_swap_registration *rego;
  struct gdbarch_swap **curr = &gdbarch->swap;
  for (rego = gdbarch_swap_registrary.registrations;
       rego != NULL;
       rego = rego->next)
    {
      if (rego->data != NULL)
	{
	  (*curr) = XMALLOC (struct gdbarch_swap);
	  (*curr)->source = rego;
	  (*curr)->swap = xmalloc (rego->sizeof_data);
	  (*curr)->next = NULL;
	  memset (rego->data, 0, rego->sizeof_data);
	  curr = &(*curr)->next;
	}
      if (rego->init != NULL)
	rego->init ();
    }
}

static void swapout_gdbarch_swap PARAMS ((struct gdbarch *));
static void
swapout_gdbarch_swap (gdbarch)
     struct gdbarch *gdbarch;
{
  struct gdbarch_swap *curr;
  for (curr = gdbarch->swap;
       curr != NULL;
       curr = curr->next)
    memcpy (curr->swap, curr->source->data, curr->source->sizeof_data);
}

static void swapin_gdbarch_swap PARAMS ((struct gdbarch *));
static void
swapin_gdbarch_swap (gdbarch)
     struct gdbarch *gdbarch;
{
  struct gdbarch_swap *curr;
  for (curr = gdbarch->swap;
       curr != NULL;
       curr = curr->next)
    memcpy (curr->source->data, curr->swap, curr->source->sizeof_data);
}


/* Keep a registrary of the architectures known by GDB. */

struct gdbarch_init_registration
{
  enum bfd_architecture bfd_architecture;
  gdbarch_init_ftype *init;
  struct gdbarch_list *arches;
  struct gdbarch_init_registration *next;
};

static struct gdbarch_init_registration *gdbarch_init_registrary = NULL;

void
register_gdbarch_init (bfd_architecture, init)
     enum bfd_architecture bfd_architecture;
     gdbarch_init_ftype *init;
{
  struct gdbarch_init_registration **curr;
  const struct bfd_arch_info *bfd_arch_info;
  /* Check that BFD reconizes this architecture */
  bfd_arch_info = bfd_lookup_arch (bfd_architecture, 0);
  if (bfd_arch_info == NULL)
    {
      fatal ("Attempt to register unknown architecture (%d)", bfd_architecture);
    }
  /* Check that we haven't seen this architecture before */
  for (curr = &gdbarch_init_registrary;
       (*curr) != NULL;
       curr = &(*curr)->next)
    {
      if (bfd_architecture == (*curr)->bfd_architecture)
	fatal ("Duplicate registraration of architecture (%s)",
	       bfd_arch_info->printable_name);
    }
  /* log it */
  if (gdbarch_debug)
    fprintf_unfiltered (gdb_stderr, "register_gdbarch_init (%s, 0x%08lx)\n",
			bfd_arch_info->printable_name,
			(long) init);
  /* Append it */
  (*curr) = XMALLOC (struct gdbarch_init_registration);
  (*curr)->bfd_architecture = bfd_architecture;
  (*curr)->init = init;
  (*curr)->arches = NULL;
  (*curr)->next = NULL;
}
  


/* Look for an architecture using gdbarch_info.  Base search on only
   BFD_ARCH_INFO and BYTE_ORDER. */

struct gdbarch_list *
gdbarch_list_lookup_by_info (arches, info)
     struct gdbarch_list *arches;
     const struct gdbarch_info *info;
{
  for (; arches != NULL; arches = arches->next)
    {
      if (info->bfd_arch_info != arches->gdbarch->bfd_arch_info)
	continue;
      if (info->byte_order != arches->gdbarch->byte_order)
	continue;
      return arches;
    }
  return NULL;
}


/* Create a new ``struct gdbarch'' based in information provied by
   ``struct gdbarch_info'' */

struct gdbarch *
gdbarch_alloc (info, tdep)
     const struct gdbarch_info *info;
     struct gdbarch_tdep *tdep;
{
  struct gdbarch *gdbarch = XMALLOC (struct gdbarch);
  memset (gdbarch, 0, sizeof (*gdbarch));

  gdbarch->tdep = tdep;

  gdbarch->bfd_arch_info = info->bfd_arch_info;
  gdbarch->byte_order = info->byte_order;

  return gdbarch;
}

/* Update the current architecture. Return ZERO if the update request
   failed. */

int
gdbarch_update (info)
     struct gdbarch_info info;
{
  struct gdbarch *new_gdbarch;
  struct gdbarch_list **list;
  struct gdbarch_init_registration *rego;

  /* Fill in any missing bits. Most important is the bfd_architecture
     which is used to select the target architecture. */
  if (info.bfd_architecture == bfd_arch_unknown)
    {
      if (info.bfd_arch_info != NULL)
	info.bfd_architecture = info.bfd_arch_info->arch;
      else if (info.abfd != NULL)
	info.bfd_architecture = bfd_get_arch (info.abfd);
      /* FIXME - should query BFD for its default architecture. */
      else
	info.bfd_architecture = current_gdbarch->bfd_arch_info->arch;
    }
  if (info.bfd_arch_info == NULL)
    {
      if (target_architecture_auto && info.abfd != NULL)
	info.bfd_arch_info = bfd_get_arch_info (info.abfd);
      else
	info.bfd_arch_info = current_gdbarch->bfd_arch_info;
    }
  if (info.byte_order == 0)
    {
      if (target_byte_order_auto && info.abfd != NULL)
	info.byte_order = (bfd_big_endian (info.abfd) ? BIG_ENDIAN
			   : bfd_little_endian (info.abfd) ? LITTLE_ENDIAN
			   : 0);
      else
	info.byte_order = current_gdbarch->byte_order;
    }
  /* A default for abfd? */

  /* Find the target that knows about this architecture. */
  for (rego = gdbarch_init_registrary;
       rego != NULL && rego->bfd_architecture != info.bfd_architecture;
       rego = rego->next);
  if (rego == NULL)
    {
      if (gdbarch_debug)
	fprintf_unfiltered (gdb_stderr, "gdbarch_update: No matching architecture\n");
      return 0;
    }

  if (gdbarch_debug)
    {
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: info.bfd_architecture %d (%s)\n",
			  info.bfd_architecture,
			  bfd_lookup_arch (info.bfd_architecture, 0)->printable_name);
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: info.bfd_arch_info %s\n",
			  (info.bfd_arch_info != NULL
			   ? info.bfd_arch_info->printable_name
			   : "(null)"));
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: info.byte_order %d (%s)\n",
			  info.byte_order,
			  (info.byte_order == BIG_ENDIAN ? "big"
			   : info.byte_order == LITTLE_ENDIAN ? "little"
			   : "default"));
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: info.abfd 0x%lx\n",
			  (long) info.abfd);
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: info.tdep_info 0x%lx\n",
			  (long) info.tdep_info);
    }

  /* Ask the target for a replacement architecture. */
  new_gdbarch = rego->init (info, rego->arches);

  /* Did the target like it?  No. Reject the change. */
  if (new_gdbarch == NULL)
    {
      if (gdbarch_debug)
	fprintf_unfiltered (gdb_stderr, "gdbarch_update: Target rejected architecture\n");
      return 0;
    }

  /* Did the architecture change?  No. Do nothing. */
  if (current_gdbarch == new_gdbarch)
    {
      if (gdbarch_debug)
	fprintf_unfiltered (gdb_stderr, "gdbarch_update: Architecture 0x%08lx (%s) unchanged\n",
			    (long) new_gdbarch,
			    new_gdbarch->bfd_arch_info->printable_name);
      return 1;
    }

  /* Swap all data belonging to the old target out */
  swapout_gdbarch_swap (current_gdbarch);

  /* Is this a pre-existing architecture?  Yes. Swap it in.  */
  for (list = &rego->arches;
       (*list) != NULL;
       list = &(*list)->next)
    {
      if ((*list)->gdbarch == new_gdbarch)
	{
	  if (gdbarch_debug)
	    fprintf_unfiltered (gdb_stderr, "gdbarch_update: Previous architecture 0x%08lx (%s) selected\n",
				(long) new_gdbarch,
				new_gdbarch->bfd_arch_info->printable_name);
	  current_gdbarch = new_gdbarch;
	  swapin_gdbarch_swap (new_gdbarch);
	  return 1;
	}
    }
    
  /* Append this new architecture to this targets list. */
  (*list) = XMALLOC (struct gdbarch_list);
  (*list)->next = NULL;
  (*list)->gdbarch = new_gdbarch;

  /* Switch to this new architecture.  Dump it out. */
  current_gdbarch = new_gdbarch;
  if (gdbarch_debug)
    {
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: New architecture 0x%08lx (%s) selected\n",
			  (long) new_gdbarch,
			  new_gdbarch->bfd_arch_info->printable_name);
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: TARGET_BYTE_ORDER = %d (%s)\n",
			  TARGET_BYTE_ORDER,
			  (TARGET_BYTE_ORDER == BIG_ENDIAN ? "big"
			   : TARGET_BYTE_ORDER == LITTLE_ENDIAN ? "little"
			   : "default"));
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: TARGET_LONG_BIT = %d\n",
			  TARGET_LONG_BIT);
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: TARGET_LONG_LONG_BIT = %d\n",
			  TARGET_LONG_LONG_BIT);
      fprintf_unfiltered (gdb_stderr,
			  "gdbarch_update: TARGET_PTR_BIT = %d\n",
			  TARGET_PTR_BIT);
    }
  
  /* Check that the newly installed architecture is valid.  */
  verify_gdbarch (new_gdbarch);

  /* Initialize the per-architecture memory (swap) areas.
     CURRENT_GDBARCH must be update before these modules are
     called. */
  init_gdbarch_swap (new_gdbarch);
  
  /* Initialize the per-architecture data-pointer of all parties that
     registered an interest in this architecture.  CURRENT_GDBARCH
     must be updated before these modules are called. */
  init_gdbarch_data (new_gdbarch);
  
  return 1;
}


/* end-sanitize-carp end-sanitize-vr4xxx */

/* Functions to manipulate the endianness of the target.  */

#ifdef TARGET_BYTE_ORDER_SELECTABLE
/* compat - Catch old targets that expect a selectable byte-order to
   default to BIG_ENDIAN */
#ifndef TARGET_BYTE_ORDER_DEFAULT
#define TARGET_BYTE_ORDER_DEFAULT BIG_ENDIAN
#endif
#endif
#ifndef TARGET_BYTE_ORDER_DEFAULT
/* compat - Catch old non byte-order selectable targets that do not
   define TARGET_BYTE_ORDER_DEFAULT and instead expect
   TARGET_BYTE_ORDER to be used as the default.  For targets that
   defined neither TARGET_BYTE_ORDER nor TARGET_BYTE_ORDER_DEFAULT the
   below will get a strange compiler warning. */
#define TARGET_BYTE_ORDER_DEFAULT TARGET_BYTE_ORDER
#endif
int target_byte_order = TARGET_BYTE_ORDER_DEFAULT;
int target_byte_order_auto = 1;

/* Chain containing the \"set endian\" commands.  */
static struct cmd_list_element *endianlist = NULL;

/* Called by ``show endian''.  */
static void show_endian PARAMS ((char *, int));
static void
show_endian (args, from_tty)
     char *args;
     int from_tty;
{
  char *msg =
    (TARGET_BYTE_ORDER_AUTO
     ? "The target endianness is set automatically (currently %s endian)\n"
     : "The target is assumed to be %s endian\n");
  printf_unfiltered (msg, (TARGET_BYTE_ORDER == BIG_ENDIAN ? "big" : "little"));
}

/* Called if the user enters ``set endian'' without an argument.  */
static void set_endian PARAMS ((char *, int));
static void
set_endian (args, from_tty)
     char *args;
     int from_tty;
{
  printf_unfiltered ("\"set endian\" must be followed by \"auto\", \"big\" or \"little\".\n");
  show_endian (args, from_tty);
}

/* Called by ``set endian big''.  */
static void set_endian_big PARAMS ((char *, int));
static void
set_endian_big (args, from_tty)
     char *args;
     int from_tty;
{
  if (TARGET_BYTE_ORDER_SELECTABLE_P)
    {
      target_byte_order = BIG_ENDIAN;
      target_byte_order_auto = 0;
      /* start-sanitize-carp start-sanitize-vr4xxx */
      if (GDB_MULTI_ARCH)
	{
	  struct gdbarch_info info;
	  memset (&info, 0, sizeof info);
	  info.byte_order = BIG_ENDIAN;
	  gdbarch_update (info);
	}
      /* end-sanitize-carp end-sanitize-vr4xxx */
    }
  else
    {
      printf_unfiltered ("Byte order is not selectable.");
      show_endian (args, from_tty);
    }
}

/* Called by ``set endian little''.  */
static void set_endian_little PARAMS ((char *, int));
static void
set_endian_little (args, from_tty)
     char *args;
     int from_tty;
{
  if (TARGET_BYTE_ORDER_SELECTABLE_P)
    {
      target_byte_order = LITTLE_ENDIAN;
      target_byte_order_auto = 0;
      /* start-sanitize-carp start-sanitize-vr4xxx */
      if (GDB_MULTI_ARCH)
	{
	  struct gdbarch_info info;
	  memset (&info, 0, sizeof info);
	  info.byte_order = LITTLE_ENDIAN;
	  gdbarch_update (info);
	}
      /* end-sanitize-carp end-sanitize-vr4xxx */
    }
  else
    {
      printf_unfiltered ("Byte order is not selectable.");
      show_endian (args, from_tty);
    }
}

/* Called by ``set endian auto''.  */
static void set_endian_auto PARAMS ((char *, int));
static void
set_endian_auto (args, from_tty)
     char *args;
     int from_tty;
{
  if (TARGET_BYTE_ORDER_SELECTABLE_P)
    {
      target_byte_order_auto = 1;
    }
  else
    {
      printf_unfiltered ("Byte order is not selectable.");
      show_endian (args, from_tty);
    }
}

/* Set the endianness from a BFD.  */
static void set_endian_from_file PARAMS ((bfd *));
static void
set_endian_from_file (abfd)
     bfd *abfd;
{
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

int target_architecture_auto = 1;
extern const struct bfd_arch_info bfd_default_arch_struct;
const struct bfd_arch_info *target_architecture = &bfd_default_arch_struct;
int (*target_architecture_hook) PARAMS ((const struct bfd_arch_info *ap));

/* Do the real work of changing the current architecture */
static void
set_arch (arch)
     const struct bfd_arch_info *arch;
{
  /* FIXME: Is it compatible with gdb? */
  /* Check with the target on the setting */
  if (target_architecture_hook != NULL
      && !target_architecture_hook (arch))
    printf_unfiltered ("Target does not support `%s' architecture.\n",
		       arch->printable_name);
  else
    {
      target_architecture_auto = 0;
      target_architecture = arch;
    }
}

/* Called if the user enters ``show architecture'' without an argument. */
static void show_architecture PARAMS ((char *, int));
static void
show_architecture (args, from_tty)
     char *args;
     int from_tty;
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
static void set_architecture PARAMS ((char *, int));
static void
set_architecture (args, from_tty)
     char *args;
     int from_tty;
{
  if (args == NULL)
    {
      printf_unfiltered ("\"set architecture\" must be followed by \"auto\" or an architecture name.\n");
    }
  else if (strcmp (args, "auto") == 0)
    {
      target_architecture_auto = 1;
    }
  /* start-sanitize-carp start-sanitize-vr4xxx */
  else if (GDB_MULTI_ARCH)
    {
      const struct bfd_arch_info *arch = bfd_scan_arch (args);
      if (arch == NULL)
	printf_unfiltered ("Architecture `%s' not reconized.\n", args);
      else
	{
	  struct gdbarch_info info;
	  memset (&info, 0, sizeof info);
	  info.bfd_arch_info = arch;
	  if (gdbarch_update (info))
	    target_architecture_auto = 0;
	  else
	    printf_unfiltered ("Architecture `%s' not reconized.\n", args);
	}
    }
  /* end-sanitize-carp end-sanitize-vr4xxx */
  else
    {
      const struct bfd_arch_info *arch = bfd_scan_arch (args);
      if (arch != NULL)
	set_arch (arch);
      else
	printf_unfiltered ("Architecture `%s' not reconized.\n", args);
    }
}

/* Called if the user enters ``info architecture'' without an argument. */
static void info_architecture PARAMS ((char *, int));
static void
info_architecture (args, from_tty)
     char *args;
     int from_tty;
{
  enum bfd_architecture a;
  /* start-sanitize-carp start-sanitize-vr4xxx */
  if (GDB_MULTI_ARCH)
    {
      if (gdbarch_init_registrary != NULL)
	{
	  struct gdbarch_init_registration *rego;
	  printf_filtered ("Available architectures are:\n");
	  for (rego = gdbarch_init_registrary;
	       rego != NULL;
	       rego = rego->next)
	    {
	      const struct bfd_arch_info *ap;
	      ap = bfd_lookup_arch (rego->bfd_architecture, 0);
	      if (ap != NULL)
		{
		  do
		    {
		      printf_filtered (" %s", ap->printable_name);
		      ap = ap->next;
		    }
		  while (ap != NULL);
		  printf_filtered ("\n");
		}
	    }
	}
      else
	{
	  printf_filtered ("There are no available architectures.\n");
	}
      return;
    }
  /* end-sanitize-carp end-sanitize-vr4xxx */
  printf_filtered ("Available architectures are:\n");
  for (a = bfd_arch_obscure + 1; a < bfd_arch_last; a++)
    {
      const struct bfd_arch_info *ap = bfd_lookup_arch (a, 0);
      if (ap != NULL)
	{
	  do
	    {
	      printf_filtered (" %s", ap->printable_name);
	      ap = ap->next;
	    }
	  while (ap != NULL);
	  printf_filtered ("\n");
	}
    }
}

/* Set the architecture from arch/machine */
void
set_architecture_from_arch_mach (arch, mach)
     enum bfd_architecture arch;
     unsigned long mach;
{
  const struct bfd_arch_info *wanted = bfd_lookup_arch (arch, mach);
  if (wanted != NULL)
    set_arch (wanted);
  else
    fatal ("hardwired architecture/machine not reconized");
}

/* Set the architecture from a BFD */
static void set_architecture_from_file PARAMS ((bfd *));
static void
set_architecture_from_file (abfd)
     bfd *abfd;
{
  const struct bfd_arch_info *wanted = bfd_get_arch_info (abfd);
  if (target_architecture_auto)
    {
      if (target_architecture_hook != NULL
	  && !target_architecture_hook (wanted))
	warning ("Target may not support %s architecture",
		 wanted->printable_name);
      target_architecture = wanted;
    }
  else if (wanted != target_architecture)
    {
      warning ("%s architecture file may be incompatible with %s target.",
	       wanted->printable_name,
	       target_architecture->printable_name);
    }
}



/* Disassembler */

/* Pointer to the target-dependent disassembly function.  */
int (*tm_print_insn) PARAMS ((bfd_vma, disassemble_info *));
disassemble_info tm_print_insn_info;



/* Set the dynamic target-system-dependant parameters (architecture,
   byte-order) using information found in the BFD */

void
set_gdbarch_from_file (abfd)
     bfd *abfd;
{
  /* start-sanitize-carp start-sanitize-vr4xxx */
  if (GDB_MULTI_ARCH)
    {
      struct gdbarch_info info;
      memset (&info, 0, sizeof info);
      info.abfd = abfd;
      gdbarch_update (info);
      return;
    }
  /* end-sanitize-carp end-sanitize-vr4xxx */
  set_architecture_from_file (abfd);
  set_endian_from_file (abfd);
}

/* start-sanitize-carp start-sanitize-vr4xxx */

/* The default architecture uses host values (for want of a better
   choice). */

struct gdbarch default_gdbarch = {
  /* basic architecture information */
  &bfd_default_arch_struct,
  TARGET_BYTE_ORDER_DEFAULT,
  /* target specific vector */
  NULL,
  /*per-architecture data-pointers and swap regions */
  0, NULL, NULL,
  /* Multi-arch values */
  8 * sizeof (long), /* long */
  8 * sizeof (LONGEST), /* long long */
  8 * sizeof (void*), /* ptr */
};
struct gdbarch *current_gdbarch = &default_gdbarch;

/* end-sanitize-carp end-sanitize-vr4xxx */

extern void _initialize_gdbarch PARAMS ((void));
void
_initialize_gdbarch ()
{
  add_prefix_cmd ("endian", class_support, set_endian,
		  "Set endianness of target.",
		  &endianlist, "set endian ", 0, &setlist);
  add_cmd ("big", class_support, set_endian_big,
	   "Set target as being big endian.", &endianlist);
  add_cmd ("little", class_support, set_endian_little,
	   "Set target as being little endian.", &endianlist);
  add_cmd ("auto", class_support, set_endian_auto,
	   "Select target endianness automatically.", &endianlist);
  add_cmd ("endian", class_support, show_endian,
	   "Show endianness of target.", &showlist);

  add_cmd ("architecture", class_support, set_architecture,
	   "Set architecture of target.", &setlist);
  add_alias_cmd ("processor", "architecture", class_support, 1, &setlist);
  add_cmd ("architecture", class_support, show_architecture,
	   "Show architecture of target.", &showlist);
  add_cmd ("architecture", class_support, info_architecture,
	   "List supported target architectures", &infolist);

  INIT_DISASSEMBLE_INFO_NO_ARCH (tm_print_insn_info, gdb_stdout, (fprintf_ftype)fprintf_filtered);
  tm_print_insn_info.flavour = bfd_target_unknown_flavour;
  tm_print_insn_info.read_memory_func = dis_asm_read_memory;
  tm_print_insn_info.memory_error_func = dis_asm_memory_error;
  tm_print_insn_info.print_address_func = dis_asm_print_address;

#ifdef MAINTENANCE_CMDS
  add_show_from_set (add_set_cmd ("archdebug",
				  class_maintenance,
				  var_zinteger,
				  (char *)&gdbarch_debug,
				  "Set architecture debugging.\n\
When non-zero, architecture debugging is enabled.", &setlist),
		     &showlist);
#endif
}
