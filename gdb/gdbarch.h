/* Architecture commands for GDB, the GNU debugger.
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

#ifndef GDBARCH_H
#define GDBARCH_H

/* start-sanitize-carp start-sanitize-vr4xxx */

#ifndef GDB_MULTI_ARCH
#define GDB_MULTI_ARCH 0
#endif

extern struct gdbarch *current_gdbarch;


/* When GDB_MULTI_ARCH override any earlier definitions of the
   below. */

extern const struct bfd_arch_info *gdbarch_bfd_arch_info PARAMS ((struct gdbarch*));
#if GDB_MULTI_ARCH
#undef TARGET_ARCHITECTURE
#define TARGET_ARCHITECTURE (gdbarch_bfd_arch_info (current_gdbarch))
#endif

extern int gdbarch_byte_order PARAMS ((struct gdbarch*));
#if GDB_MULTI_ARCH
#undef TARGET_BYTE_ORDER
#define TARGET_BYTE_ORDER (gdbarch_byte_order (current_gdbarch))
#endif

extern int gdbarch_long_bit  PARAMS ((struct gdbarch*));
extern void set_gdbarch_long_bit PARAMS ((struct gdbarch*, int));
#if GDB_MULTI_ARCH
#undef TARGET_LONG_BIT
#define TARGET_LONG_BIT (gdbarch_long_bit (current_gdbarch))
#endif

extern int gdbarch_long_long_bit  PARAMS ((struct gdbarch*));
extern void set_gdbarch_long_long_bit PARAMS ((struct gdbarch*, int));
#if GDB_MULTI_ARCH
#undef TARGET_LONG_LONG_BIT
#define TARGET_LONG_LONG_BIT (gdbarch_long_long_bit (current_gdbarch))
#endif

extern int gdbarch_ptr_bit  PARAMS ((struct gdbarch*));
extern void set_gdbarch_ptr_bit PARAMS ((struct gdbarch*, int));
#if GDB_MULTI_ARCH
#undef TARGET_PTR_BIT
#define TARGET_PTR_BIT (gdbarch_ptr_bit (current_gdbarch))
#endif

extern struct gdbarch_tdep *gdbarch_tdep PARAMS ((struct gdbarch*));


/* Mechanism for co-ordinating the selection of a specific
   architecture.

   GDB targets (*-tdep.c) can register an interest in a specific
   architecture.  Other GDB components can register a need to maintain
   per-architecture data.

   The mechanisms below ensures that only a loose connection between
   the set-architecture command and the various GDB components exists.
   Each component can independantly register their need to maintain
   architecture specific data with gdbarch.

   Pragmatics:

   Previously, a single TARGET_ARCHITECTURE_HOOK was provided.  It
   didn't scale.

   The more traditional mega-struct containing architecture specific
   data for all the various GDB components was also considered.  Since
   GDB is built from a variable number of (fairly independant)
   components this global aproach was considered non-applicable. */


/* Register a new architectural family with GDB.

   Register support for the specified architecture with GDB.  When
   ever gdbarch determines that this architecture has been selected,
   the specifed INIT function is called.

   INIT takes two parameters: INFO which contains the information
   available to gdbarch about the (possibly new) architecture; ARCHES
   which is a list of the previously created ``struct gdbarch'' for
   this architecture.  When possible, and when no other value was
   provided, INFO is initialized using either the ABFD or the current
   GDBARCH.

   The INIT function shall return any of: NULL indicating that it
   doesn't reconize the selected architecture; an existing ``struct
   gdbarch'' from the ARCHES list (indicating that the new
   architecture is just a synonym for an earlier architecture); create
   and then return a new ``struct gdbarch'' for this new architecture
   (using gdbarch_alloc()).  */

struct gdbarch_list
{
  struct gdbarch *gdbarch;
  struct gdbarch_list *next;
};

struct gdbarch_info
{
  /* Default: bfd_arch_unknown. */
  enum bfd_architecture bfd_architecture;

  /* Default: NULL */
  const struct bfd_arch_info *bfd_arch_info;

  /* Default: 0 */
  int byte_order;

  /* Default: NULL */
  bfd *abfd;

  /* Default: NULL */
  struct gdbarch_tdep_info *tdep_info;
};

typedef struct gdbarch *(gdbarch_init_ftype) PARAMS ((struct gdbarch_info info, struct gdbarch_list *arches));

extern void register_gdbarch_init PARAMS ((enum bfd_architecture, gdbarch_init_ftype *));

/* Helper function.  Search ARCHES for a gdbarch that matches
   information provided by INFO. */

extern struct gdbarch_list *gdbarch_list_lookup_by_info PARAMS ((struct gdbarch_list *arches,  const struct gdbarch_info *info));

/* Helper function.  Create a preliminary ``struct gdbarch''.  Perform
   basic initialization using values from the INFO structure. */

extern struct gdbarch *gdbarch_alloc PARAMS ((const struct gdbarch_info *, struct gdbarch_tdep *));

/* Helper function. Force the updating of the current architecture.
   Used by targets that have added their own target specific
   architecture manipulation commands. */

extern int gdbarch_update PARAMS ((struct gdbarch_info));



/* Register per-architecture data-pointer.

   Reserve space for a per-architecture data-pointer.  An identifier
   for the reserved data-pointer is returned.  That identifer should
   be saved in a local static.

   When a new architecture is selected, INIT() is called.  When a
   previous architecture is re-selected, the per-architecture
   data-pointer for that previous architecture is restored (INIT() is
   not called).

   INIT() shall return the initial value for the per-architecture
   data-pointer for the current architecture.

   Multiple registrarants for any architecture are allowed (and
   strongly encouraged).  */

typedef void *(gdbarch_data_ftype) PARAMS ((void));
extern struct gdbarch_data *register_gdbarch_data PARAMS ((gdbarch_data_ftype *init));


/* Return the value of the per-architecture data-pointer for the
   current architecture. */

extern void *gdbarch_data PARAMS ((struct gdbarch_data*));


/* Register per-architecture memory region.

   For legacy code, provide a memory-region swap mechanism.
   Per-architecture memory blocks are created, these being swapped
   whenever the architecture is changed.  For a new architecture, the
   memory region is initialized with zero (0) and the INIT function is
   called.

   Memory regions are swapped / initialized in the order that they are
   registered.  NULL DATA and/or INIT values can be specified. */

typedef void (gdbarch_swap_ftype) PARAMS ((void));
extern void register_gdbarch_swap PARAMS ((void *data, unsigned long size, gdbarch_swap_ftype *init));


/* end-sanitize-carp end-sanitize-vr4xxx */

/* The target-system-dependant byte order is dynamic */

/* TARGET_BYTE_ORDER_SELECTABLE_P determines if the target endianness
   is selectable at runtime.  The user can use the `set endian'
   command to change it.  TARGET_BYTE_ORDER_AUTO is nonzero when
   target_byte_order should be auto-detected (from the program image
   say). */

#ifndef TARGET_BYTE_ORDER_SELECTABLE_P
/* compat - Catch old targets that define TARGET_BYTE_ORDER_SLECTABLE
   when they should have defined TARGET_BYTE_ORDER_SELECTABLE_P 1 */
#ifdef TARGET_BYTE_ORDER_SELECTABLE
#define TARGET_BYTE_ORDER_SELECTABLE_P 1
#else
#define TARGET_BYTE_ORDER_SELECTABLE_P 0
#endif
#endif

extern int target_byte_order;
#ifdef TARGET_BYTE_ORDER_SELECTABLE
/* compat - Catch old targets that define TARGET_BYTE_ORDER_SELECTABLE
   and expect defs.h to re-define TARGET_BYTE_ORDER. */
#undef TARGET_BYTE_ORDER
#endif
#ifndef TARGET_BYTE_ORDER
#define TARGET_BYTE_ORDER (target_byte_order + 0)
#endif

extern int target_byte_order_auto;
#ifndef TARGET_BYTE_ORDER_AUTO
#define TARGET_BYTE_ORDER_AUTO (target_byte_order_auto + 0)
#endif



/* The target-system-dependant BFD architecture is dynamic */

extern int target_architecture_auto;
#ifndef TARGET_ARCHITECTURE_AUTO
#define TARGET_ARCHITECTURE_AUTO (target_architecture_auto + 0)
#endif

extern const struct bfd_arch_info *target_architecture;
#ifndef TARGET_ARCHITECTURE
#define TARGET_ARCHITECTURE (target_architecture + 0)
#endif

/* Notify the target dependant backend of a change to the selected
   architecture. A zero return status indicates that the target did
   not like the change. */

extern int (*target_architecture_hook) PARAMS ((const struct bfd_arch_info *)); 



/* The target-system-dependant disassembler is semi-dynamic */

#include "dis-asm.h"		/* Get defs for disassemble_info */

extern int dis_asm_read_memory PARAMS ((bfd_vma memaddr, bfd_byte *myaddr,
					int len, disassemble_info *info));

extern void dis_asm_memory_error PARAMS ((int status, bfd_vma memaddr,
					  disassemble_info *info));

extern void dis_asm_print_address PARAMS ((bfd_vma addr,
					   disassemble_info *info));

extern int (*tm_print_insn) PARAMS ((bfd_vma, disassemble_info*));
extern disassemble_info tm_print_insn_info;
#ifndef TARGET_PRINT_INSN
#define TARGET_PRINT_INSN(vma, info) (*tm_print_insn) (vma, info)
#endif
#ifndef TARGET_PRINT_INSN_INFO
#define TARGET_PRINT_INSN_INFO (&tm_print_insn_info)
#endif



/* Set the dynamic target-system-dependant parameters (architecture,
   byte-order, ...) using information found in the BFD */

extern void set_gdbarch_from_file PARAMS ((bfd *));


/* Explicitly set the dynamic target-system-dependant parameters based
   on bfd_architecture and machine. */

extern void set_architecture_from_arch_mach PARAMS ((enum bfd_architecture, unsigned long));


/* gdbarch trace variable */
extern int gdbarch_debug;

#endif
