/* Common target dependent code for GDB on HPPA systems.
   Copyright 2003 Free Software Foundation, Inc.

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

#ifndef HPPA_TDEP_H
#define HPPA_TDEP_H

enum { HPPA_INSTRUCTION_SIZE = 4 };

/* Target-dependent structure in gdbarch.  */
struct gdbarch_tdep
{
  /* The number of bytes in an address.  For now, this field is designed
     to allow us to differentiate hppa32 from hppa64 targets.  */
  int bytes_per_address;
};

/*
 * Unwind table and descriptor.
 */

struct unwind_table_entry
  {
    CORE_ADDR region_start;
    CORE_ADDR region_end;

    unsigned int Cannot_unwind:1;	/* 0 */
    unsigned int Millicode:1;	/* 1 */
    unsigned int Millicode_save_sr0:1;	/* 2 */
    unsigned int Region_description:2;	/* 3..4 */
    unsigned int reserved1:1;	/* 5 */
    unsigned int Entry_SR:1;	/* 6 */
    unsigned int Entry_FR:4;	/* number saved *//* 7..10 */
    unsigned int Entry_GR:5;	/* number saved *//* 11..15 */
    unsigned int Args_stored:1;	/* 16 */
    unsigned int Variable_Frame:1;	/* 17 */
    unsigned int Separate_Package_Body:1;	/* 18 */
    unsigned int Frame_Extension_Millicode:1;	/* 19 */
    unsigned int Stack_Overflow_Check:1;	/* 20 */
    unsigned int Two_Instruction_SP_Increment:1;	/* 21 */
    unsigned int Ada_Region:1;	/* 22 */
    unsigned int cxx_info:1;	/* 23 */
    unsigned int cxx_try_catch:1;	/* 24 */
    unsigned int sched_entry_seq:1;	/* 25 */
    unsigned int reserved2:1;	/* 26 */
    unsigned int Save_SP:1;	/* 27 */
    unsigned int Save_RP:1;	/* 28 */
    unsigned int Save_MRP_in_frame:1;	/* 29 */
    unsigned int extn_ptr_defined:1;	/* 30 */
    unsigned int Cleanup_defined:1;	/* 31 */

    unsigned int MPE_XL_interrupt_marker:1;	/* 0 */
    unsigned int HP_UX_interrupt_marker:1;	/* 1 */
    unsigned int Large_frame:1;	/* 2 */
    unsigned int Pseudo_SP_Set:1;	/* 3 */
    unsigned int reserved4:1;	/* 4 */
    unsigned int Total_frame_size:27;	/* 5..31 */

    /* This is *NOT* part of an actual unwind_descriptor in an object
       file.  It is *ONLY* part of the "internalized" descriptors that
       we create from those in a file.
     */
    struct
      {
	unsigned int stub_type:4;	/* 0..3 */
	unsigned int padding:28;	/* 4..31 */
      }
    stub_unwind;
  };

/* HP linkers also generate unwinds for various linker-generated stubs.
   GDB reads in the stubs from the $UNWIND_END$ subspace, then 
   "converts" them into normal unwind entries using some of the reserved
   fields to store the stub type.  */

/* The gaps represent linker stubs used in MPE and space for future
   expansion.  */
enum unwind_stub_types
  {
    LONG_BRANCH = 1,
    PARAMETER_RELOCATION = 2,
    EXPORT = 10,
    IMPORT = 11,
    IMPORT_SHLIB = 12,
  };

/* We use the objfile->obj_private pointer for two things:
 * 1.  An unwind table;
 *
 * 2.  A pointer to any associated shared library object.
 *
 * #defines are used to help refer to these objects.
 */

/* Info about the unwind table associated with an object file.
 * This is hung off of the "objfile->obj_private" pointer, and
 * is allocated in the objfile's psymbol obstack.  This allows
 * us to have unique unwind info for each executable and shared
 * library that we are debugging.
 */
struct hppa_unwind_info
  {
    struct unwind_table_entry *table;	/* Pointer to unwind info */
    struct unwind_table_entry *cache;	/* Pointer to last entry we found */
    int last;			/* Index of last entry */
  };

struct hppa_objfile_private
  {
    struct hppa_unwind_info *unwind_info;	/* a pointer */
    struct so_list *so_info;	/* a pointer  */
    CORE_ADDR dp;
  };

extern const struct objfile_data *hppa_objfile_priv_data;

#endif  /* HPPA_TDEP_H */
