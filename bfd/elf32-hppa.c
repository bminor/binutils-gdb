/* BFD back-end for HP PA-RISC ELF files.
   Copyright (C) 1990, 91, 92, 93, 94 Free Software Foundation, Inc.

   Written by

	Center for Software Science
	Department of Computer Science
	University of Utah

This file is part of BFD, the Binary File Descriptor library.

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
#include "bfdlink.h"
#include "libelf.h"

/* Note there isn't much error handling code in here yet.  Unexpected
   conditions are handled by just calling abort.  FIXME damnit! */
 
/* ELF32/HPPA relocation support

	This file contains ELF32/HPPA relocation support as specified
	in the Stratus FTX/Golf Object File Format (SED-1762) dated
	November 19, 1992.  */

#include "elf32-hppa.h"
#include "aout/aout64.h"
#include "hppa_stubs.h"

/* The basic stub types supported.  If/when shared libraries are
   implemented some form of IMPORT and EXPORT stubs will be needed.  */
typedef enum
{
  HPPA_STUB_ILLEGAL,
  HPPA_STUB_ARG_RELOC,
  HPPA_STUB_LONG_CALL,
} hppa_stub_type;

/* This is a list of all the stubs for a particular BFD.  */

typedef struct elf32_hppa_stub_name_list_struct
{
  /* The symbol associated with this stub.  */
  asymbol *sym;
  /* Pointer to chain of all stub chains.  */
  struct elf32_hppa_stub_description_struct *stub_desc;
  /* Pointer to the stub contents (eg instructions).  */  
  int *stub_secp;
  /* Size of this stub?  (in what units?  FIXME).  */
  unsigned size;
  /* Pointer to the next stub entry in the chain.  */
  struct elf32_hppa_stub_name_list_struct *next;
} elf32_hppa_stub_name_list;

/* This is a linked list in which each entry describes all the
   linker stubs for a particular bfd.  */
   
typedef struct elf32_hppa_stub_description_struct
{
  /* The next group of stubs.  */
  struct elf32_hppa_stub_description_struct *next;
  /* Used to identify this group of stubs as belonging
     to a particular bfd.  */
  bfd *this_bfd;
  /* FIXME: The stub section for this group of stubs?  Is
     this redundant with stub_listP->sym->section?  */
  asection *stub_sec;
  /* FIXME: what the hell is this?  */
  unsigned relocs_allocated_cnt;
  /* The current real size of the stubs (in bytes?).  */
  unsigned real_size;
  /* How much space we have allocated for stubs (in bytes?).  */
  unsigned allocated_size;
  /* Pointer to the first available space for new stubs.  */
  int *stub_secp;
  /* Pointer to the beginning of the stubs.  FIXME: Why an int *
     above and a char * here?  */
  char *stub_contents;
  /* The list of stubs for this bfd.  */
  elf32_hppa_stub_name_list *stub_listP;
  /* I guess we just carry this around for fun.  */
  struct bfd_link_info *link_info;
} elf32_hppa_stub_description;

/* FIXME.  */
#define ARGUMENTS	0
#define RETURN_VALUE	1

/* The various argument relocations that may be performed.
   Note GRX,GRY really means ARGX,ARGY.  */
typedef enum
{
  /* No relocation.  */
  NO_ARG_RELOC,
  /* Relocate 32 bits from general to FP register.  */
  R_TO_FR,
  /* Relocate 64 bits from arg0,arg1 to FParg1.  */
  R01_TO_FR,
  /* Relocate 64 bits from arg2,arg3 to FParg3.  */
  R23_TO_FR,
  /* Relocate 32 bits from FP to general register.  */
  FR_TO_R,
  /* Relocate 64 bits from FParg1 to arg0,arg1.  */
  FR_TO_R01,
  /* Relocate 64 bits from FParg3 to arg2,arg3.  */
  FR_TO_R23,
  /* Death.  */
  ARG_RELOC_ERR,
} arg_reloc_type;

/* Where (what register type) is an argument comming from?  */
typedef enum
{
  /* Not in a register.  */
  AR_NO,
  /* In a general argument register.  */
  AR_GR,
  /* In right half of a FP argument register.  */
  AR_FR,
  /* In upper (left) half of a FP argument register.  */
  AR_FU,
  /* In general argument register pair 0 (arg0, arg1).  */
  AR_DBL01,
  /* In general argument register pair 1 (arg2, arg3).  */
  AR_DBL23,
} arg_location;

/* What is being relocated (eg which argument or the return value).  */
typedef enum
{
  ARG0, ARG1, ARG2, ARG3, RETVAL,
} arg_reloc_location;

/* Horizontal represents callee's argument location information, vertical
   represents caller's argument location information.  Value at a particular
   X, Y location represents what (if any) argument relocation needs to
   be performed to make caller and callee agree.  */
static CONST arg_reloc_type mismatches[6][6] =
{
 {NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC,
    NO_ARG_RELOC, NO_ARG_RELOC},
 {NO_ARG_RELOC, NO_ARG_RELOC, R_TO_FR, ARG_RELOC_ERR,
    R01_TO_FR, ARG_RELOC_ERR},
 {NO_ARG_RELOC, FR_TO_R, NO_ARG_RELOC, ARG_RELOC_ERR,
    ARG_RELOC_ERR, ARG_RELOC_ERR},
 {ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR,
    ARG_RELOC_ERR, ARG_RELOC_ERR},
 {NO_ARG_RELOC, FR_TO_R01, NO_ARG_RELOC, ARG_RELOC_ERR,
    NO_ARG_RELOC, ARG_RELOC_ERR},
 {NO_ARG_RELOC, FR_TO_R23, NO_ARG_RELOC, ARG_RELOC_ERR,
    ARG_RELOC_ERR, NO_ARG_RELOC},
};

/* Likewise for the return value.  */
static CONST arg_reloc_type retval_mismatches[6][6] =
{
 {NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC, NO_ARG_RELOC,
    NO_ARG_RELOC, NO_ARG_RELOC},
 {NO_ARG_RELOC, NO_ARG_RELOC, FR_TO_R, ARG_RELOC_ERR,
    FR_TO_R01, ARG_RELOC_ERR},
 {NO_ARG_RELOC, R_TO_FR, NO_ARG_RELOC, ARG_RELOC_ERR,
    ARG_RELOC_ERR, ARG_RELOC_ERR},
 {ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR, ARG_RELOC_ERR,
    ARG_RELOC_ERR, ARG_RELOC_ERR},
 {NO_ARG_RELOC, R01_TO_FR, NO_ARG_RELOC, ARG_RELOC_ERR,
    NO_ARG_RELOC, ARG_RELOC_ERR},
 {NO_ARG_RELOC, R23_TO_FR, NO_ARG_RELOC, ARG_RELOC_ERR,
    ARG_RELOC_ERR, NO_ARG_RELOC},
};

/* Used for index mapping in symbol-extension sections.  */
struct elf32_hppa_symextn_map_struct
{
  int old_index;
  bfd *bfd;
  asymbol *sym;
  int new_index;
};

static bfd_reloc_status_type hppa_elf_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd *, char **));

static unsigned long hppa_elf_relocate_insn 
  PARAMS ((bfd *, asection *, unsigned long, unsigned long, long,
	   long, unsigned long, unsigned long, unsigned long));

static long get_symbol_value PARAMS ((asymbol *));

static bfd_reloc_status_type hppa_elf_reloc
  PARAMS ((bfd *, arelent *, asymbol *, PTR, asection *, bfd*, char **));

static CONST reloc_howto_type * elf_hppa_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));

static symext_entryS elf32_hppa_get_sym_extn PARAMS ((bfd *, asymbol *, int));

static elf32_hppa_stub_description * find_stubs PARAMS ((bfd *, asection *));

static elf32_hppa_stub_description * new_stub
  PARAMS ((bfd *, asection *, struct bfd_link_info *));

static arg_reloc_type type_of_mismatch PARAMS ((int, int, int));

static elf32_hppa_stub_name_list * find_stub_by_name
  PARAMS ((bfd *, asection *, char *));

static elf32_hppa_stub_name_list * add_stub_by_name
  PARAMS ((bfd *, asection *, asymbol *, struct bfd_link_info *));

static void hppa_elf_stub_finish PARAMS ((bfd *));

static void hppa_elf_stub_reloc
  PARAMS ((elf32_hppa_stub_description *, bfd *, asymbol **, int,
	   elf32_hppa_reloc_type));

static int hppa_elf_arg_reloc_needed_p
  PARAMS ((bfd *, arelent *, arg_reloc_type [5], symext_entryS));

static asymbol * hppa_elf_build_linker_stub
  PARAMS ((bfd *, bfd *, struct bfd_link_info *, arelent *,
	   arg_reloc_type [5], int, unsigned *, hppa_stub_type));

static void hppa_elf_create_stub_sec 
  PARAMS ((bfd *, bfd *, asection **, struct bfd_link_info *));

static int hppa_elf_long_branch_needed_p
  PARAMS ((bfd *, asection *, arelent *, asymbol *, unsigned));

static boolean hppa_elf_set_section_contents
  PARAMS ((bfd *, sec_ptr, PTR, file_ptr, bfd_size_type));

static void elf_info_to_howto
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rela *));

static void elf32_hppa_backend_symbol_processing PARAMS ((bfd *, asymbol *));

static boolean elf32_hppa_backend_section_processing
  PARAMS ((bfd *, Elf32_Internal_Shdr *));

static boolean elf32_hppa_backend_symbol_table_processing
  PARAMS ((bfd *, elf_symbol_type *, int));

static boolean elf32_hppa_backend_section_from_shdr
  PARAMS ((bfd *, Elf32_Internal_Shdr *, char *));

static boolean elf32_hppa_backend_fake_sections
  PARAMS ((bfd *, Elf_Internal_Shdr *, asection *));

static boolean elf32_hppa_backend_section_from_bfd_section
  PARAMS ((bfd *, Elf32_Internal_Shdr *, asection *, int *));

static void elf32_hppa_backend_begin_write_processing PARAMS ((bfd *));

static void elf32_hppa_backend_final_write_processing PARAMS ((bfd *));

static void add_entry_to_symext_chain
  PARAMS ((bfd *, elf_symbol_type *, int, symext_chainS **, symext_chainS **));

static void
elf_hppa_tc_make_sections PARAMS ((bfd *, symext_chainS *));

static boolean hppa_elf_is_local_label PARAMS ((bfd *, asymbol *));

/* ELF/PA relocation howto entries.  */

static reloc_howto_type elf_hppa_howto_table[ELF_HOWTO_TABLE_SIZE] =
{
  {R_PARISC_NONE, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_NONE"},
  {R_PARISC_DIR32, 0, 0, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DIR32"},
  {R_PARISC_DIR21L, 0, 0, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DIR21L"},
  {R_PARISC_DIR17R, 0, 0, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DIR17R"},
  {R_PARISC_DIR17F, 0, 0, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DIR17F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DIR14R, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DIR14R"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PCREL21L, 0, 0, 21, true, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PCREL21L"},
  {R_PARISC_PCREL17R, 0, 0, 17, true, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PCREL17R"},
  {R_PARISC_PCREL17F, 0, 0, 17, true, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PCREL17F"},
  {R_PARISC_PCREL17C, 0, 0, 17, true, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PCREL17C"},
  {R_PARISC_PCREL14R, 0, 0, 14, true, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PCREL14R"},
  {R_PARISC_PCREL14F, 0, 0, 14, true, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PCREL14F"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DPREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DPREL21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DPREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DPREL14R"},
  {R_PARISC_DPREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DPREL14F"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DLTREL21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DLTREL14R"},
  {R_PARISC_DLTREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DLTREL14F"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTIND21L, 0, 0, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DLTIND21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DLTIND14R, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DLTIND14R"},
  {R_PARISC_DLTIND14F, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_DLTIND14F"},

  {R_PARISC_SETBASE, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_SETBASE"},
  {R_PARISC_BASEREL32, 0, 0, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_BASEREL32"},
  {R_PARISC_BASEREL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_BASEREL21L"},
  {R_PARISC_BASEREL17R, 0, 0, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_BASEREL17R"},
  {R_PARISC_BASEREL17F, 0, 0, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_BASEREL17F"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_BASEREL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_BASEREL14R"},
  {R_PARISC_BASEREL14F, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_BASEREL14F"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_TEXTREL32, 0, 0, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_TEXTREL32"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_DATAREL32, 0, 0, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},


  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PLABEL32, 0, 0, 32, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PLABEL32"},
  {R_PARISC_PLABEL21L, 0, 0, 21, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PLABEL21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PLABEL14R, 0, 0, 14, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PLABEL14R"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},


  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PLTIND21L, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PLTIND21L"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_UNIMPLEMENTED"},
  {R_PARISC_PLTIND14R, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PLTIND14R"},
  {R_PARISC_PLTIND14F, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_PLTIND14F"},


  {R_PARISC_COPY, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_COPY"},
  {R_PARISC_GLOB_DAT, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_GLOB_DAT"},
  {R_PARISC_JMP_SLOT, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_JMP_SLOT"},
  {R_PARISC_RELATIVE, 0, 0, 0, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_RELATIVE"},
  {R_PARISC_STUB_CALL_17, 0, 0, 17, false, 0, complain_overflow_bitfield, hppa_elf_reloc, "R_PARISC_STUB_CALL_17"},

  {R_PARISC_UNIMPLEMENTED, 0, 0, 0, false, 0, complain_overflow_dont, NULL, "R_PARISC_UNIMPLEMENTED"},
};

static symext_chainS *symext_rootP;
static symext_chainS *symext_lastP;
static int symext_chain_size;
static long global_value;
static long GOT_value;
static asymbol *global_symbol;
static int global_sym_defined;
static symext_entryS *symextn_contents;
static elf32_hppa_stub_description *elf_hppa_stub_rootP;
static boolean stubs_finished = false;
static struct elf32_hppa_symextn_map_struct *elf32_hppa_symextn_map;
static int elf32_hppa_symextn_map_size;

static char *linker_stubs = NULL;
static int linker_stubs_size = 0;
static int linker_stubs_max_size = 0;
#define STUB_ALLOC_INCR	100
#define STUB_SYM_BUFFER_INC	5

/* Relocate the given INSN given the various input parameters.

   FIXME: endianness and sizeof (long) issues abound here.  */

static unsigned long
hppa_elf_relocate_insn (abfd, input_sect, insn, address, sym_value,
			r_addend, r_format, r_field, pcrel)
     bfd *abfd;
     asection *input_sect;
     unsigned long insn;
     unsigned long address;
     long sym_value;
     long r_addend;
     unsigned long r_format;
     unsigned long r_field;
     unsigned long pcrel;
{
  unsigned char opcode = get_opcode (insn);
  long constant_value;
  unsigned arg_reloc;

  switch (opcode)
    {
    case LDO:
    case LDB:
    case LDH:
    case LDW:
    case LDWM:
    case STB:
    case STH:
    case STW:
    case STWM:
    case COMICLR:
    case SUBI:
    case ADDIT:
    case ADDI:
    case LDIL:
    case ADDIL:
      constant_value = HPPA_R_CONSTANT (r_addend);

      if (pcrel)
	sym_value -= address;

      sym_value = hppa_field_adjust (sym_value, constant_value, r_field);
      return hppa_rebuild_insn (abfd, insn, sym_value, r_format);

    case BL:
    case BE:
    case BLE:
      arg_reloc = HPPA_R_ARG_RELOC (r_addend);

      /* XXX computing constant_value is not needed??? */
      constant_value = assemble_17 ((insn & 0x001f0000) >> 16,
				    (insn & 0x00001ffc) >> 2,
				    insn & 1);

      constant_value = (constant_value << 15) >> 15;
      if (pcrel)
	{
	  sym_value -=
	    address + input_sect->output_offset
	    + input_sect->output_section->vma;
	  sym_value = hppa_field_adjust (sym_value, -8, r_field);
	}
      else
	sym_value = hppa_field_adjust (sym_value, constant_value, r_field);

      return hppa_rebuild_insn (abfd, insn, sym_value >> 2, r_format);

    default:
      if (opcode == 0)
	{
	  constant_value = HPPA_R_CONSTANT (r_addend);

	  if (pcrel)
	    sym_value -= address;

	  return hppa_field_adjust (sym_value, constant_value, r_field);
	}
      else
	abort ();
    }
}

/* Return the relocated value of the given symbol.  */

static long
get_symbol_value (symbol)
     asymbol *symbol;
{
  if (symbol == NULL
      || symbol->section == &bfd_com_section)
    return 0;
  else
    return symbol->value + symbol->section->output_section->vma
      + symbol->section->output_offset;
}

/* Return one (or more) BFD relocations which implement the base
   relocation with modifications based on format and field.  */

elf32_hppa_reloc_type **
hppa_elf_gen_reloc_type (abfd, base_type, format, field)
     bfd *abfd;
     elf32_hppa_reloc_type base_type;
     int format;
     int field;
{
  elf32_hppa_reloc_type *finaltype;
  elf32_hppa_reloc_type **final_types;

  /* Allocate slots for the BFD relocation.  */
  final_types = (elf32_hppa_reloc_type **)
    bfd_alloc_by_size_t (abfd, sizeof (elf32_hppa_reloc_type *) * 2);
  BFD_ASSERT (final_types != 0); /* FIXME */

  /* Allocate space for the relocation itself.  */
  finaltype = (elf32_hppa_reloc_type *)
    bfd_alloc_by_size_t (abfd, sizeof (elf32_hppa_reloc_type));
  BFD_ASSERT (finaltype != 0);	/* FIXME */

  /* Some reasonable defaults.  */
  final_types[0] = finaltype;
  final_types[1] = NULL;

#define final_type finaltype[0]

  final_type = base_type;

  /* Just a tangle of nested switch statements to deal with the braindamage
     that a different field selector means a completely different relocation
     for PA ELF.  */
  switch (base_type)
    {
    case R_HPPA:
      switch (format)
	{
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_DIR14R;
	      break;
	    case e_rtsel:
	      final_type = R_PARISC_DLTREL14R;
	      break;
	    case e_tsel:
	      final_type = R_PARISC_DLTREL14F;
	      break;
	    case e_rpsel:
	      final_type = R_PARISC_PLABEL14R;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	case 17:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_PARISC_DIR17F;
	      break;
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_DIR17R;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	case 21:
	  switch (field)
	    {
	    case e_lsel:
	    case e_lrsel:
	      final_type = R_PARISC_DIR21L;
	      break;
	    case e_ltsel:
	      final_type = R_PARISC_DLTREL21L;
	      break;
	    case e_lpsel:
	      final_type = R_PARISC_PLABEL21L;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	case 32:
	  switch (field)
	    {
	    case e_fsel:
	      final_type = R_PARISC_DIR32;
	      break;
	    case e_psel:
	      final_type = R_PARISC_PLABEL32;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	default:
	  abort ();
	  break;
	}
      break;


    case R_HPPA_GOTOFF:
      switch (format)
	{
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_DPREL14R;
	      break;
	    case e_fsel:
	      final_type = R_PARISC_DPREL14F;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	case 21:
	  switch (field)
	    {
	    case e_lrsel:
	    case e_lsel:
	      final_type = R_PARISC_DPREL21L;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	default:
	  abort ();
	  break;
	}
      break;


    case R_HPPA_PCREL_CALL:
      switch (format)
	{
	case 14:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_PCREL14R;
	      break;
	    case e_fsel:
	      final_type = R_PARISC_PCREL14F;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	case 17:
	  switch (field)
	    {
	    case e_rsel:
	    case e_rrsel:
	      final_type = R_PARISC_PCREL17R;
	      break;
	    case e_fsel:
	      final_type = R_PARISC_PCREL17F;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	case 21:
	  switch (field)
	    {
	    case e_lsel:
	    case e_lrsel:
	      final_type = R_PARISC_PCREL21L;
	      break;
	    default:
	      abort ();
	      break;
	    }
	  break;

	default:
	  abort ();
	  break;
	}
      break;

    default:
      abort ();
      break;
    }

  return final_types;
}

#undef final_type


/* Actually perform a relocation.  */

static bfd_reloc_status_type
hppa_elf_reloc (abfd, reloc_entry, symbol_in, data, input_section, output_bfd,
		error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol_in;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  unsigned long insn;
  long sym_value = 0;
  unsigned long addr = reloc_entry->address;
  bfd_byte *hit_data = addr + (bfd_byte *) data;
  unsigned long r_type = reloc_entry->howto->type;
  unsigned long r_field = e_fsel;
  boolean r_pcrel = reloc_entry->howto->pc_relative;
  unsigned r_format = reloc_entry->howto->bitsize;
  long r_addend = reloc_entry->addend;

  /* If only performing a partial link, get out early.  */
  if (output_bfd)
    {
      reloc_entry->address += input_section->output_offset;

      /* Work around lossage in generic elf code to write relocations.
	 (maps different section symbols into the same symbol index).  */
      if ((symbol_in->flags & BSF_SECTION_SYM)
	  && symbol_in->section)
	reloc_entry->addend += symbol_in->section->output_offset;
      return bfd_reloc_ok;
    }

  /* If performing final link and the symbol we're relocating against
     is undefined, then return an error.  */ 
  if (symbol_in && symbol_in->section == &bfd_und_section)
    return bfd_reloc_undefined;

  /* Get the final relocated value.  */
  sym_value = get_symbol_value (symbol_in);

  /* Compute the value of $global$.
     FIXME: None of this should be necessary.  $global$ is just a 
     marker and shouldn't really figure into these computations.

     Once that's fixed we'll need to teach this backend to change
     DP-relative relocations involving symbols in the text section
     to be simple absolute relocations.  */
  if (!global_sym_defined)
    {
      if (global_symbol)
	{
	  global_value = (global_symbol->value
			  + global_symbol->section->output_section->vma
			  + global_symbol->section->output_offset);
	  GOT_value = global_value;
	  global_sym_defined++;
	}
    }

  /* Get the instruction word.  */
  insn = bfd_get_32 (abfd, hit_data);

  switch (r_type)
    {
    case R_PARISC_NONE:
      break;

    case R_PARISC_DIR32:
    case R_PARISC_DIR17F:
    case R_PARISC_PCREL17F:
    case R_PARISC_PCREL17C:
    case R_PARISC_PLABEL32:
    case R_PARISC_PCREL14F:
      r_field = e_fsel;
      goto do_basic_type_1;
    case R_PARISC_DIR21L:
    case R_PARISC_PCREL21L:
    case R_PARISC_PLABEL21L:
      r_field = e_lrsel;
      goto do_basic_type_1;
    case R_PARISC_DIR17R:
    case R_PARISC_PCREL17R:
    case R_PARISC_DIR14R:
    case R_PARISC_PCREL14R:
    case R_PARISC_PLABEL14R:
      r_field = e_rrsel;
      goto do_basic_type_1;

    case R_PARISC_DPREL21L:
      r_field = e_lrsel;
      sym_value -= GOT_value;
      goto do_basic_type_1;
    case R_PARISC_DPREL14R:
      r_field = e_rrsel;
      sym_value -= GOT_value;
      goto do_basic_type_1;
    case R_PARISC_DPREL14F:
      r_field = e_fsel;
      sym_value -= GOT_value;
      goto do_basic_type_1;


do_basic_type_1:
      insn = hppa_elf_relocate_insn (abfd, input_section, insn, addr,
				     sym_value, r_addend, r_format,
				     r_field, r_pcrel);
      break;


    /* This is a linker internal relocation.  */
    case R_PARISC_STUB_CALL_17:
      /* This relocation is for a branch to a long branch stub.
	 Change instruction to a BLE,N.  It may also be necessary
	 to interchange the branch and its delay slot.
	 The original instruction stream is

	    bl <foo>,r		; call foo using register r as
				; the return pointer
	    XXX			; delay slot instruction

	 The new instruction stream will be:

	    XXX			; delay slot instruction
	    ble <foo_stub>	; call the long call stub for foo
				; using r31 as the return pointer

	 This braindamage is necessary because the compiler may put
	 an instruction which uses %r31 in the delay slot of the original
	 call.  By changing the call instruction from a "bl" to a "ble"
	 %r31 gets clobbered before the delay slot executes.  This
	 also means the stub has to play funny games to make sure
	 we return to the instruction just after the BLE rather than
	 two instructions after the BLE.

	 We do not interchange the branch and delay slot if the delay
	 slot was already nullified, or if the instruction in the delay
	 slot modifies the return pointer to avoid an unconditional
	 jump after the call returns (GCC optimization).

	 None of this horseshit would be necessary if we put the
	 stubs between functions and just redirected the "bl" to
	 the stub.  Live and learn.  */
	 
      /* Is this instruction nullified?  (does this ever happen?)  */
      if (insn & 2)
        {
	  insn = BLE_N_XXX_0_0;
	  bfd_put_32 (abfd, insn, hit_data);
	  r_type = R_PARISC_DIR17F;
	  r_pcrel = 0;
	  insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					 addr, sym_value, r_addend,
					 r_format, r_field, r_pcrel);
        }
      else
	{
	  /* So much for the trivial case...  */
	  unsigned long old_delay_slot_insn = bfd_get_32 (abfd, hit_data + 4);
	  unsigned rtn_reg = (insn & 0x03e00000) >> 21;

	  if (get_opcode (old_delay_slot_insn) == LDO)
	    {
	      unsigned ldo_src_reg = (old_delay_slot_insn & 0x03e00000) >> 21;
	      unsigned ldo_target_reg = (old_delay_slot_insn & 0x001f0000) >> 16;

	      /* If the target of the LDO is the same as the return
		 register then there is no reordering.  We can leave the
		 instuction as a non-nullified BLE in this case.

		 FIXME:  This test looks wrong.  If we had a ble using
		 ldo_target_reg as the *source* we'd fuck this up.  */
	      if (ldo_target_reg == rtn_reg)
		{
		  unsigned long new_delay_slot_insn = old_delay_slot_insn;

		  BFD_ASSERT (ldo_src_reg == ldo_target_reg);
		  new_delay_slot_insn &= 0xfc00ffff;
		  new_delay_slot_insn |= ((31 << 21) | (31 << 16));
		  bfd_put_32 (abfd, new_delay_slot_insn, hit_data + 4);
		  insn = BLE_XXX_0_0;
		  r_type = R_PARISC_DIR17F;
		  r_pcrel = 0;
		  insn = hppa_elf_relocate_insn (abfd, input_section, insn,
						 addr, sym_value, r_addend,
						 r_format, r_field, r_pcrel);
		  bfd_put_32 (abfd, insn, hit_data);
		  return bfd_reloc_ok;
		}
              else if (rtn_reg == 31)
                {
                  /* The return register is r31, so this is a millicode
		     call.  Do not perform any instruction reordering.  */
	          insn = BLE_XXX_0_0;
	          r_type = R_PARISC_DIR17F;
	          r_pcrel = 0;
	          insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					         addr, sym_value,
					         r_addend, r_format,
					         r_field, r_pcrel);
	          bfd_put_32 (abfd, insn, hit_data);
	          return bfd_reloc_ok;
                }
	      else
		{
		  /* Check to see if the delay slot instruction has a
		     relocation.  If so, we need to change the address
		     field of it because the instruction it relocates
		     is going to be moved.  Oh what a mess.  */
		  arelent * next_reloc_entry = reloc_entry+1;

		  if (next_reloc_entry->address == reloc_entry->address + 4)
		    next_reloc_entry->address -= 4;

		  insn = old_delay_slot_insn;
		  bfd_put_32 (abfd, insn, hit_data);
		  insn = BLE_N_XXX_0_0;
		  bfd_put_32 (abfd, insn, hit_data + 4);
		  r_type = R_PARISC_DIR17F;
		  r_pcrel = 0;
		  insn = hppa_elf_relocate_insn (abfd, input_section, insn,
						 addr + 4,
						 sym_value, r_addend,
						 r_format, r_field, r_pcrel);
		  bfd_put_32 (abfd, insn, hit_data + 4);
		  return bfd_reloc_ok;
		}
	    }
	  /* Same comments as above regarding incorrect test.  */
          else if (rtn_reg == 31)
            {
              /* The return register is r31, so this is a millicode call.
		 Perform no instruction reordering in this case.  */
	      insn = BLE_XXX_0_0;
	      r_type = R_PARISC_DIR17F;
	      r_pcrel = 0;
	      insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					     addr, sym_value,
					     r_addend, r_format,
					     r_field, r_pcrel);
	      bfd_put_32 (abfd, insn, hit_data);
	      return bfd_reloc_ok;
            }
	  else
	    {
	      /* Check to see if the delay slot instruction has a
		 relocation.  If so, we need to change its address
		 field  because the instruction it relocates is going
		 to be moved.  */
	      arelent * next_reloc_entry = reloc_entry+1;

	      if (next_reloc_entry->address == reloc_entry->address + 4)
		next_reloc_entry->address -= 4;

	      insn = old_delay_slot_insn;
	      bfd_put_32 (abfd, insn, hit_data);
	      insn = BLE_N_XXX_0_0;
	      bfd_put_32 (abfd, insn, hit_data + 4);
	      r_type = R_PARISC_DIR17F;
	      r_pcrel = 0;
	      insn = hppa_elf_relocate_insn (abfd, input_section, insn,
					     addr + 4, sym_value,
					     r_addend, r_format,
					     r_field, r_pcrel);
	      bfd_put_32 (abfd, insn, hit_data + 4);
	      return bfd_reloc_ok;
	    }
	}
      break;
      
    /* Something we don't know how to handle.  */
    default:
      *error_message = (char *) "Unrecognized reloc";
      return bfd_reloc_notsupported;
    }

  /* Update the instruction word.  */
  bfd_put_32 (abfd, insn, hit_data);
  return (bfd_reloc_ok);
}

/* Return the address of the howto table entry to perform the CODE
   relocation for an ARCH machine.  */

static CONST reloc_howto_type *
elf_hppa_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  if ((int) code < (int) R_PARISC_UNIMPLEMENTED)
    {
      BFD_ASSERT ((int) elf_hppa_howto_table[(int) code].type == (int) code);
      return &elf_hppa_howto_table[(int) code];
    }
  return NULL;
}

/* Return true if SYM represents a local label symbol.  */

static boolean
hppa_elf_is_local_label (abfd, sym)
     bfd *abfd;
     asymbol *sym;
{
  return (sym->name[0] == 'L' && sym->name[1] == '$');
}

/* Do any backend specific processing when beginning to write an object
   file.  For PA ELF we need to determine the size of the symbol extension
   section *before* any other output processing happens.  */

static void
elf32_hppa_backend_begin_write_processing (abfd)
     bfd *abfd;
{
  int i;
  asection *symextn_sec;

  /* Size up the symbol extension section.  We can't built it just
     yet as the elf_symbol_map hasn't been built.  */
  if (abfd->outsymbols == NULL || symext_chain_size != 0)
    return;

  /* Look at each symbol, and determine if it will need an entry in
     the symbol extension section.  */
  for (i = 0; i < abfd->symcount; i++)
    {
      elf_symbol_type *symbol = (elf_symbol_type *)abfd->outsymbols[i];

      /* Only functions ever need an entry in the symbol extension
	 section.  */
      if (!(symbol->symbol.flags & BSF_FUNCTION))
	continue;

      /* And only if they specify the locations of their arguments.  */
      if (symbol->tc_data.hppa_arg_reloc == 0)
	continue;

      /* Yup.  This function symbol needs an entry.  */
      symext_chain_size += 2 * sizeof (symext_entryS);
    }

  /* Now create the section and set its size.  We'll fill in the
     contents later.  */
  symextn_sec = bfd_get_section_by_name (abfd, SYMEXTN_SECTION_NAME);
  if (symextn_sec == NULL)
    {
      symextn_sec = bfd_make_section (abfd, SYMEXTN_SECTION_NAME);
      bfd_set_section_flags (abfd, symextn_sec,
			     SEC_LOAD | SEC_HAS_CONTENTS | SEC_DATA);
      symextn_sec->output_section = symextn_sec;
      symextn_sec->output_offset = 0;
      bfd_set_section_alignment (abfd, symextn_sec, 2);
      bfd_set_section_size (abfd, symextn_sec, symext_chain_size);
    }

}

/* Perform any processing needed late in the object file writing process.
   For PA ELF we build and set the contents of the symbol extension
   section.  */

static void
elf32_hppa_backend_final_write_processing (abfd)
     bfd *abfd;
{
  asection *symextn_sec;
  int i, *symtab_map = (int *) elf_sym_extra (abfd);

  /* Now build the symbol extension section.  */
  if (symext_chain_size == 0)
    return; 

  /* Look at each symbol, adding the appropriate information to the
     symbol extension section list as necessary.  */
  for (i = 0; i < abfd->symcount; i++)
    {
      elf_symbol_type *symbol = (elf_symbol_type *) abfd->outsymbols[i];

      /* Only functions ever need an entry in the symbol extension
	 section.  */
      if (!(symbol->symbol.flags & BSF_FUNCTION))
	continue;

      /* And only if they specify the locations of their arguments.  */
      if (symbol->tc_data.hppa_arg_reloc == 0)
	continue;

      /* Add this symbol's information to the chain.  */
      add_entry_to_symext_chain (abfd, symbol, symtab_map[i],
				 &symext_rootP, &symext_lastP);
    }

  /* Now fill in the contents of the symbol extension chain.  */
  elf_hppa_tc_make_sections (abfd, symext_rootP);

  /* And attach that as the section's contents.  */
  symextn_sec = bfd_get_section_by_name (abfd, SYMEXTN_SECTION_NAME);
  if (symextn_sec == (asection *) 0)
    abort();

  symextn_sec->contents = (void *)symextn_contents;
  
  bfd_set_section_contents (abfd, symextn_sec, symextn_sec->contents, 
			    symextn_sec->output_offset, symextn_sec->_raw_size);
}

/* Update the symbol extention chain to include the symbol pointed to
   by SYMBOLP if SYMBOLP is a function symbol.  Used internally and by GAS.  */

static void
add_entry_to_symext_chain (abfd, symbol, sym_idx, symext_root, symext_last)
     bfd *abfd;
     elf_symbol_type *symbol;
     int sym_idx;
     symext_chainS **symext_root;
     symext_chainS **symext_last;
{
  symext_chainS *symextP;
  unsigned int arg_reloc = symbol->tc_data.hppa_arg_reloc;

  /* Allocate memory and initialize this entry.  */
  symextP = (symext_chainS *) bfd_alloc (abfd, sizeof (symext_chainS) * 2);
  if (!symextP)
    {
      bfd_set_error (bfd_error_no_memory);
      abort();			/* FIXME */
    }

  symextP[0].entry = ELF32_PARISC_SX_WORD (PARISC_SXT_SYMNDX, sym_idx);
  symextP[0].next = &symextP[1];

  symextP[1].entry = ELF32_PARISC_SX_WORD (PARISC_SXT_ARG_RELOC, arg_reloc);
  symextP[1].next = NULL;

  /* Now update the chain itself so it can be walked later to build
     the symbol extension section.  */
  if (*symext_root == NULL)
    {
      *symext_root = &symextP[0];
      *symext_last = &symextP[1];
    }
  else
    {
      (*symext_last)->next = &symextP[0];
      *symext_last = &symextP[1];
    }
}

/* Build the symbol extension section.  Used internally and by GAS.  */ 

static void
elf_hppa_tc_make_sections (abfd, symext_root)
     bfd *abfd;
     symext_chainS *symext_root;
{
  symext_chainS *symextP;
  int i;
  asection *symextn_sec;

  /* FIXME: Huh?  I don't see what this is supposed to do for us.  */
  hppa_elf_stub_finish (abfd);

  symextn_sec = bfd_get_section_by_name (abfd, SYMEXTN_SECTION_NAME);

  /* Grab some memory for the contents of the symbol extension section
     itself.  */
  symextn_contents = (symext_entryS *) bfd_zalloc (abfd,
						   symextn_sec->_raw_size);
  if (!symextn_contents)
    {
      bfd_set_error (bfd_error_no_memory);
      abort();			/* FIXME */
    }

  /* Fill in the contents of the symbol extension chain.  */
  for (i = 0, symextP = symext_root; symextP; symextP = symextP->next, ++i)
    symextn_contents[i] = symextP->entry;

  return;
}

/* Return the symbol extension record of type TYPE for the symbol SYM.  */

static symext_entryS
elf32_hppa_get_sym_extn (abfd, sym, type)
     bfd *abfd;
     asymbol *sym;
     int type;
{
  switch (type)
    {
    case PARISC_SXT_SYMNDX:
    case PARISC_SXT_NULL:
      return (symext_entryS) 0;
    case PARISC_SXT_ARG_RELOC:
      {
	elf_symbol_type *esymP = (elf_symbol_type *) sym;

	return (symext_entryS) esymP->tc_data.hppa_arg_reloc;
      }
    /* This should never happen.  */
    default:
      abort();
    }
}

/* Search the chain of stub descriptions and locate the stub
   description for this the given section within the given bfd.

   FIXME:  I see yet another wonderful linear linked list search
   here.  This is probably bad.  */

static elf32_hppa_stub_description *
find_stubs (abfd, stub_sec)
     bfd *abfd;
     asection *stub_sec;
{
  elf32_hppa_stub_description *stubP;

  for (stubP = elf_hppa_stub_rootP; stubP; stubP = stubP->next)
    {
      /* Is this the right one?  */ 
      if (stubP->this_bfd == abfd && stubP->stub_sec == stub_sec)
	return stubP;
    }
  return NULL;
}

static elf32_hppa_stub_description *
new_stub (abfd, stub_sec, link_info)
     bfd *abfd;
     asection *stub_sec;
     struct bfd_link_info *link_info;
{
  elf32_hppa_stub_description *stub = find_stubs (abfd, stub_sec);

  /* If we found a list for this bfd, then use it.  */
  if (stub)
    return stub;

  /* Nope, allocate and initialize a new entry in the stub list chain.  */
  stub = (elf32_hppa_stub_description *)
    bfd_zalloc (abfd, sizeof (elf32_hppa_stub_description));
  if (stub)
    {
      stub->this_bfd = abfd;
      stub->stub_sec = stub_sec;
      stub->real_size = 0;
      stub->allocated_size = 0;
      stub->stub_contents = NULL;
      stub->stub_secp = NULL;
      stub->link_info = link_info;

      stub->next = elf_hppa_stub_rootP;
      elf_hppa_stub_rootP = stub;
    }
  else
    {
      bfd_set_error (bfd_error_no_memory);
      abort();			/* FIXME */
    }

  return stub;
}

/* Try and locate a stub with the name NAME within the stubs
   associated with ABFD.  More linked list searches.  */

static elf32_hppa_stub_name_list *
find_stub_by_name (abfd, stub_sec, name)
     bfd *abfd;
     asection *stub_sec;
     char *name;
{
  /* Find the stubs associated with this bfd.  */
  elf32_hppa_stub_description *stub = find_stubs (abfd, stub_sec);

  /* If found, then we have to walk down them looking for a match.  */
  if (stub)
    {
      elf32_hppa_stub_name_list *name_listP;

      for (name_listP = stub->stub_listP;
	   name_listP;
	   name_listP = name_listP->next)
	{
	  if (!strcmp (name_listP->sym->name, name))
	    return name_listP;
	}
    }

  /* Not found.  */
  return 0;
}

/* Add a new stub (SYM) to the list of stubs associated with the given BFD.  */
static elf32_hppa_stub_name_list *
add_stub_by_name(abfd, stub_sec, sym, link_info)
     bfd *abfd;
     asection *stub_sec;
     asymbol *sym;
     struct bfd_link_info *link_info;
{
  elf32_hppa_stub_description *stub = find_stubs (abfd, stub_sec);
  elf32_hppa_stub_name_list *stub_entry;

  /* If no stubs are associated with this bfd, then we have to make
     a chain-of-stubs associated with this bfd.  */
  if (!stub)
    stub = new_stub (abfd, stub_sec, link_info);

  if (stub)
    {
      /* Allocate and initialize an entry in the stub chain.  */
      stub_entry = (elf32_hppa_stub_name_list *)
	bfd_zalloc (abfd, sizeof (elf32_hppa_stub_name_list));

      if (stub_entry)
	{
	  stub_entry->size = 0;
	  stub_entry->sym = sym;
	  stub_entry->stub_desc = stub;
	  /* First byte of this stub is the pointer to
	     the next available location in the stub buffer.  */
	  stub_entry->stub_secp = stub->stub_secp;
	  /* Add it to the chain.  */
	  if (stub->stub_listP)
	    stub_entry->next = stub->stub_listP;
	  else
	    stub_entry->next = NULL;
	  stub->stub_listP = stub_entry;
	  return stub_entry;
	}
      else
	{
	  bfd_set_error (bfd_error_no_memory);
	  abort();		/* FIXME */
	}
    }
  /* Death by mis-adventure.  */
  abort ();
  return (elf32_hppa_stub_name_list *)NULL;
}

/* For the given caller/callee argument location information and the
   type of relocation (arguments or return value), return the type
   of argument relocation needed to make caller and callee happy.  */

static arg_reloc_type
type_of_mismatch (caller_bits, callee_bits, type)
     int caller_bits;
     int callee_bits;
     int type;
{
  switch (type)
    {
    case ARGUMENTS:
      return mismatches[caller_bits][callee_bits];
    case RETURN_VALUE:
      return retval_mismatches[caller_bits][callee_bits];
    }
  return ARG_RELOC_ERR;
}

/* Extract specific argument location bits for WHICH from the
   the full argument location information in AR.  */
#define EXTRACT_ARBITS(ar, which)	((ar) >> (8 - ((which) * 2))) & 3

/* Add the new instruction INSN into the stub area denoted by ENTRY.
   FIXME:  Looks like more cases where we assume sizeof (int) ==
   sizeof (insn) which may not be true if building cross tools.  */
#define NEW_INSTRUCTION(entry, insn)	\
{ \
  *((entry)->stub_desc->stub_secp)++ = (insn);	\
  (entry)->stub_desc->real_size += sizeof (int);	\
  (entry)->size += sizeof(int);	\
  bfd_set_section_size((entry)->stub_desc->this_bfd,	\
		       (entry)->stub_desc->stub_sec,	\
		       (entry)->stub_desc->real_size);	\
}

/* Find the offset of the current stub?  Looks more like it
   finds the offset of the last instruction to me.  */
#define CURRENT_STUB_OFFSET(entry)	\
  ((char *)(entry)->stub_desc->stub_secp \
   - (char *)(entry)->stub_desc->stub_contents - 4)

/* All the stubs have already been built, finish up stub stuff
   by applying relocations to the stubs.  */

static void
hppa_elf_stub_finish (output_bfd)
     bfd *output_bfd;
{
  elf32_hppa_stub_description *stub_list = elf_hppa_stub_rootP;

  /* If the stubs have been finished, then we're already done.  */
  if (stubs_finished)
    return;

  /* Walk down the list of stub lists. */
  for (; stub_list; stub_list = stub_list->next)
    {
      /* If this list has stubs, then do something.  */
      if (stub_list->real_size)
	{
	  bfd *stub_bfd = stub_list->this_bfd;
	  asection *stub_sec = bfd_get_section_by_name (stub_bfd,
							".PARISC.stubs");
	  long reloc_size;
	  arelent **reloc_vector;
	  long reloc_count;

	  /* Some sanity checking.  */
	  BFD_ASSERT (stub_sec == stub_list->stub_sec);
	  BFD_ASSERT (stub_sec);

	  /* For stub sections raw_size == cooked_size.  Also update
	     reloc_done as we're handling the relocs now.  */
	  stub_sec->_cooked_size = stub_sec->_raw_size;
	  stub_sec->reloc_done = true;

	  /* Make space to hold the relocations for the stub section.  */
	  reloc_size = bfd_get_reloc_upper_bound (stub_bfd, stub_sec);
	  if (reloc_size < 0)
	    {
	      /* FIXME: Should return an error.  */
	      abort ();
	    }
	  reloc_vector = (arelent **) malloc (reloc_size);
	  if (reloc_vector == NULL && reloc_size != 0)
	    {
	      /* FIXME: should be returning an error so the caller can
		 clean up */
	      abort ();
	    }

	  /* If we have relocations, do them.  */
	  reloc_count = bfd_canonicalize_reloc (stub_bfd, stub_sec,
						reloc_vector,
						output_bfd->outsymbols);
	  if (reloc_count < 0)
	    {
	      /* FIXME: Should return an error.  */
	      abort ();
	    }
	  if (reloc_count > 0)
	    {
	      arelent **parent;
	      for (parent = reloc_vector; *parent != NULL; parent++)
		{
		  char *err = NULL;
		  bfd_reloc_status_type r =
		    bfd_perform_relocation (stub_bfd, *parent,
					    stub_list->stub_contents,
					    stub_sec, (bfd *) NULL, &err);

		  /* If there was an error, tell someone about it.  */
		  if (r != bfd_reloc_ok)
		    {
		      struct bfd_link_info *link_info = stub_list->link_info;

		      switch (r)
			{
			case bfd_reloc_undefined:
			  if (! ((*link_info->callbacks->undefined_symbol)
				 (link_info,
				  bfd_asymbol_name (*(*parent)->sym_ptr_ptr),
				  stub_bfd, stub_sec, (*parent)->address)))
			    abort ();
			  break;
			case bfd_reloc_dangerous:
			  if (! ((*link_info->callbacks->reloc_dangerous)
				 (link_info, err, stub_bfd, stub_sec,
				  (*parent)->address)))
			    abort ();
			  break;
			case bfd_reloc_overflow:
			  {
			    if (! ((*link_info->callbacks->reloc_overflow)
				   (link_info,
				    bfd_asymbol_name (*(*parent)->sym_ptr_ptr),
				    (*parent)->howto->name,
				    (*parent)->addend,
				    stub_bfd, stub_sec,
				    (*parent)->address)))
			      abort ();
			  }
			  break;
			case bfd_reloc_outofrange:
			default:
			  abort ();
			  break;
			}
		    }
		}
	    }
	  free (reloc_vector);

	  /* All done with the relocations.  Set the final contents
	     of the stub section.  FIXME: no check of return value!  */
	  bfd_set_section_contents (output_bfd, stub_sec,
				    stub_list->stub_contents,
				    0, stub_list->real_size);
	}
    }
  /* All done.  */
  stubs_finished = true;
}

/* Allocate a new relocation entry to be used in a linker stub.  */

static void
hppa_elf_stub_reloc (stub_desc, output_bfd, target_sym, offset, type)
     elf32_hppa_stub_description *stub_desc;
     bfd *output_bfd;
     asymbol **target_sym;
     int offset;
     elf32_hppa_reloc_type type;
{
  arelent relent;
  int size;
  Elf_Internal_Shdr *rela_hdr;

  /* I really don't like the realloc nonsense in here.  FIXME.  */
  if (stub_desc->relocs_allocated_cnt == stub_desc->stub_sec->reloc_count)
    {
      /* Allocate the first few relocation entries.  */
      if (stub_desc->stub_sec->relocation == NULL)
	{
	  stub_desc->relocs_allocated_cnt = STUB_RELOC_INCR;
	  size = sizeof (arelent) * stub_desc->relocs_allocated_cnt;
	  stub_desc->stub_sec->relocation = (arelent *) bfd_zmalloc (size);
	}
      else
	{
	  /* We've used all the entries we've already allocated.  So get
	     some more.  */
	  stub_desc->relocs_allocated_cnt += STUB_RELOC_INCR;
	  size = sizeof (arelent) * stub_desc->relocs_allocated_cnt;
	  stub_desc->stub_sec->relocation = (arelent *)
	    realloc (stub_desc->stub_sec->relocation, size);
	}
      if (!stub_desc->stub_sec->relocation)
	{
	  bfd_set_error (bfd_error_no_memory);
	  abort ();		/* FIXME */
	}
    }

  rela_hdr = &elf_section_data(stub_desc->stub_sec)->rel_hdr;
  rela_hdr->sh_size += sizeof(Elf32_External_Rela);

  /* Fill in the details. */
  relent.address = offset;
  relent.addend = 0;
  relent.sym_ptr_ptr = target_sym;
  relent.howto = bfd_reloc_type_lookup (stub_desc->this_bfd, type);

  /* Save it in the array of relocations for the stub section.  */
  memcpy (&stub_desc->stub_sec->relocation[stub_desc->stub_sec->reloc_count++],
	  &relent, sizeof (arelent));
}

/* Build an argument relocation stub.  RTN_ADJUST is a hint that an
   adjust to the return pointer from within the stub itself may be
   needed.  */

static asymbol *
hppa_elf_build_linker_stub (abfd, output_bfd, link_info, reloc_entry,
			    stub_types, rtn_adjust, data, linker_stub_type)
     bfd *abfd;
     bfd *output_bfd;
     struct bfd_link_info *link_info;
     arelent *reloc_entry;
     arg_reloc_type stub_types[5];
     int rtn_adjust;
     unsigned *data;
     hppa_stub_type linker_stub_type;
{
  int i;
  boolean milli, dyncall;
  char stub_sym_name[128];
  elf32_hppa_stub_name_list *stub_entry;
  /* Some initialization.  */
  unsigned insn = data[0];
  asymbol *stub_sym = NULL;
  asymbol **orig_sym = reloc_entry->sym_ptr_ptr;
  asection *stub_sec = bfd_get_section_by_name (abfd, ".PARISC.stubs");
  elf32_hppa_stub_description *stub_desc = find_stubs (abfd, stub_sec);

  /* Perform some additional checks on whether we should really do the
     return adjustment.  For example, if the instruction is nullified
     or if the delay slot contains an instruction that modifies the return
     pointer, then the branch instructions should not be rearranged
     (rtn_adjust is false).  */
  if (insn & 2 || insn == 0)
    rtn_adjust = false;
  else
    {
      unsigned delay_insn = data[1];

      if (get_opcode (delay_insn) == LDO
	  && (((insn & 0x03e00000) >> 21) == ((delay_insn & 0x001f0000) >> 16)))
	rtn_adjust = false;
    }

  /* Some special code for long-call stubs.  */
  if (linker_stub_type == HPPA_STUB_LONG_CALL)
    {
      
      /* Is this a millicode call?  If so, the return address
	 comes in on r31 rather than r2 (rp) so a slightly
	 different code sequence is needed.  */
      unsigned rtn_reg = (insn & 0x03e00000) >> 21;
      if (rtn_reg == 31)
	milli = true;
      
      /* Dyncall is special because the user code has already
	 put the return pointer in %r2 (aka RP).  Other millicode
	 calls have the return pointer in %r31.  */
      if (strcmp ((*orig_sym)->name, "$$dyncall") == 0)
	dyncall = true;
      
      /* If we are creating a call from a stub to another stub, then
	 never do the instruction reordering.  We can tell if we are
	 going to be calling one stub from another by the fact that
	 the symbol name has '_stub_' (arg. reloc. stub) or '_lb_stub_'
	 prepended to the name.  Alternatively, the section of the
	 symbol will be '.PARISC.stubs'.  This is only an issue
	 for long-calls; they are the only stubs allowed to call another
	 stub.  */
      if ((strncmp ((*orig_sym)->name, "_stub_", 6) == 0)
	  || (strncmp ((*orig_sym)->name, "_lb_stub_", 9) == 0))
	{
	  BFD_ASSERT (strcmp ((*orig_sym)->section->name, ".PARISC.stubs")
		      == 0);
	  rtn_adjust = false;
	}
    }

  /* Create the stub section if necessary.  */
  if (!stub_sec)
    {
      BFD_ASSERT (stub_desc == NULL);
      hppa_elf_create_stub_sec (abfd, output_bfd, &stub_sec, link_info);
      stub_desc = new_stub (abfd, stub_sec, link_info);
    }

  /* Make the stub if we did not find one already.  */
  if (!stub_desc)
    stub_desc = new_stub (abfd, stub_sec, link_info);

  /* Allocate space to write the stub.
     FIXME:  Why using realloc?!?  */
  if (!stub_desc->stub_contents)
    {
      stub_desc->allocated_size = STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) malloc (STUB_BUFFER_INCR);
    }
  else if ((stub_desc->allocated_size - stub_desc->real_size) < STUB_MAX_SIZE)
    {
      stub_desc->allocated_size = stub_desc->allocated_size + STUB_BUFFER_INCR;
      stub_desc->stub_contents = (char *) realloc (stub_desc->stub_contents,
						   stub_desc->allocated_size);
    }

  /* If no memory die.  (I seriously doubt the other routines
     are prepared to get a NULL return value).  */
  if (!stub_desc->stub_contents)
    {
      bfd_set_error (bfd_error_no_memory);
      abort ();
    }

  /* Generate an appropriate name for this stub.  */
  if (linker_stub_type == HPPA_STUB_ARG_RELOC)
    sprintf (stub_sym_name,
	     "_stub_%s_%02d_%02d_%02d_%02d_%02d_%s",
	     reloc_entry->sym_ptr_ptr[0]->name,
	     stub_types[0], stub_types[1], stub_types[2],
	     stub_types[3], stub_types[4],
	     rtn_adjust ? "RA" : "");
  else
    sprintf (stub_sym_name,
	     "_lb_stub_%s_%s", reloc_entry->sym_ptr_ptr[0]->name,
	     rtn_adjust ? "RA" : "");


  stub_desc->stub_secp
    = (int *) (stub_desc->stub_contents + stub_desc->real_size);
  stub_entry = find_stub_by_name (abfd, stub_sec, stub_sym_name);

  /* See if we already have one by this name.  */
  if (stub_entry)
    {
      /* Yes, re-use it.  Redirect the original relocation from the
	 old symbol (a function symbol) to the stub (the stub will call
	 the original function).  */
      stub_sym = stub_entry->sym;
      reloc_entry->sym_ptr_ptr = (asymbol **) bfd_zalloc (abfd,
							  sizeof (asymbol **));
      if (reloc_entry->sym_ptr_ptr == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  abort ();
	}
      reloc_entry->sym_ptr_ptr[0] = stub_sym;
      if (linker_stub_type == HPPA_STUB_LONG_CALL
	  || (reloc_entry->howto->type != R_PARISC_PLABEL32
	      && (get_opcode(insn) == BLE
		  || get_opcode (insn) == BE
		  || get_opcode (insn) == BL)))
	reloc_entry->howto = bfd_reloc_type_lookup (abfd, R_PARISC_STUB_CALL_17);
    }
  else
    {
      /* Create a new symbol to point to this stub.  */
      stub_sym = bfd_make_empty_symbol (abfd);
      if (!stub_sym)
	{
	  bfd_set_error (bfd_error_no_memory);
	  abort ();
	}
      stub_sym->name = bfd_zalloc (abfd, strlen (stub_sym_name) + 1);
      if (!stub_sym->name)
	{
	  bfd_set_error (bfd_error_no_memory);
	  abort ();
	}
      strcpy ((char *) stub_sym->name, stub_sym_name);
      stub_sym->value
	= (char *) stub_desc->stub_secp - (char *) stub_desc->stub_contents;
      stub_sym->section = stub_sec;
      stub_sym->flags = BSF_LOCAL | BSF_FUNCTION;
      stub_entry = add_stub_by_name (abfd, stub_sec, stub_sym, link_info);

      /* Redirect the original relocation from the old symbol (a function)
	 to the stub (the stub calls the function).  */
      reloc_entry->sym_ptr_ptr = (asymbol **) bfd_zalloc (abfd,
							  sizeof (asymbol **));
      if (reloc_entry->sym_ptr_ptr == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  abort ();
	}
      reloc_entry->sym_ptr_ptr[0] = stub_sym;
      if (linker_stub_type == HPPA_STUB_LONG_CALL
	  || (reloc_entry->howto->type != R_PARISC_PLABEL32
	      && (get_opcode (insn) == BLE
		  || get_opcode (insn) == BE
		  || get_opcode (insn) == BL)))
	reloc_entry->howto = bfd_reloc_type_lookup (abfd, R_PARISC_STUB_CALL_17);

      /* Now generate the code for the stub.  Starting with two
	 common instructions.

	 FIXME:  Do we still need the SP adjustment?
		 Do we still need to muck with space registers?  */
      NEW_INSTRUCTION (stub_entry, LDSID_31_1)
      NEW_INSTRUCTION (stub_entry, MTSP_1_SR0)

      if (linker_stub_type == HPPA_STUB_ARG_RELOC)
	{
	  NEW_INSTRUCTION (stub_entry, ADDI_8_SP)

	  /* Examine each argument, generating code to relocate it
	     into a different register if necessary.  */
	    for (i = ARG0; i < ARG3; i++)
	      {
		switch (stub_types[i])
		  {

		  case NO_ARG_RELOC:
		    continue;

		  case R_TO_FR:
		    switch (i)
		      {
		      case ARG0:
			NEW_INSTRUCTION (stub_entry, STWS_ARG0_M8SP)
			NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG0)
			  break;
		      case ARG1:
			NEW_INSTRUCTION (stub_entry, STWS_ARG1_M8SP)
		        NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG1)
			  break;
		      case ARG2:
			NEW_INSTRUCTION (stub_entry, STWS_ARG2_M8SP)
			NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG2)
			  break;
		      case ARG3:
			NEW_INSTRUCTION (stub_entry, STWS_ARG3_M8SP)
			NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FARG3)
			  break;
		      }
		    continue;
	      
		  case R01_TO_FR:
		    switch (i)
		      {
		      case ARG0:
			NEW_INSTRUCTION (stub_entry, STWS_ARG0_M4SP)
			NEW_INSTRUCTION (stub_entry, STWS_ARG1_M8SP)
			NEW_INSTRUCTION (stub_entry, FLDDS_M8SP_FARG1)
			  break;
		      default:
			abort ();
			break;
		      }
		    continue;

		  case R23_TO_FR:
		    switch (i)
		      {
		      case ARG2:
			NEW_INSTRUCTION (stub_entry, STWS_ARG2_M4SP)
			NEW_INSTRUCTION (stub_entry, STWS_ARG3_M8SP)
			NEW_INSTRUCTION (stub_entry, FLDDS_M8SP_FARG3)
			  break;
		      default:
			abort ();
			break;
		      }
		    continue;

		  case FR_TO_R:
		    switch (i)
		      {
		      case ARG0:
			NEW_INSTRUCTION (stub_entry, FSTWS_FARG0_M8SP)
			NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG0)
			  break;
		      case ARG1:
			NEW_INSTRUCTION (stub_entry, FSTWS_FARG1_M8SP)
			NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG1)
			  break;
		      case ARG2:
			NEW_INSTRUCTION (stub_entry, FSTWS_FARG2_M8SP)
			NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG2)
			  break;
		      case ARG3:
			NEW_INSTRUCTION (stub_entry, FSTWS_FARG3_M8SP)
			NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG3)
			  break;
		      }
		    continue;
	      
		  case FR_TO_R01:
		    switch (i)
		      {
		      case ARG0:
			NEW_INSTRUCTION (stub_entry, FSTDS_FARG1_M8SP)
			NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG0)
			NEW_INSTRUCTION (stub_entry, LDWS_M8SP_ARG1)
			  break;
		      default:
			abort ();
			break;
		      }
		    continue;
	      
		  case FR_TO_R23:
		    switch (i)
		      {
		      case ARG2:
			NEW_INSTRUCTION (stub_entry, FSTDS_FARG3_M8SP)
			NEW_INSTRUCTION (stub_entry, LDWS_M4SP_ARG2)
			NEW_INSTRUCTION (stub_entry, LDWS_M8SP_ARG3)
			  break;
		      default:
			abort ();
			break;
		      }
		    continue;

		  default:
		    abort ();
		    break;
		  }
	      }

	  /* Put the stack pointer back.  FIXME:  Is this still necessary?  */
	  NEW_INSTRUCTION (stub_entry, ADDI_M8_SP_SP)
	}

      /* Common code again.  Return pointer adjustment and the like.  */
      if (!dyncall)
	{
	  /* This isn't dyncall.  */ 
	  if (!milli)
	    {
	      /* It's not a millicode call, so get the correct return
		 value into %r2 (aka RP).  */
	      if (rtn_adjust)
		NEW_INSTRUCTION (stub_entry, ADDI_M4_31_RP)
	      else
		NEW_INSTRUCTION (stub_entry, COPY_31_2)
	    }
	  else
	    {
	      /* It is a millicode call, so get the correct return
		 value into %r1?!?.  FIXME:  Shouldn't this be
		 %r31?  Yes, and a little re-arrangement of the
		 code below would make that possible.  */
	      if (rtn_adjust)
		NEW_INSTRUCTION (stub_entry, ADDI_M4_31_1)
	      else
		NEW_INSTRUCTION (stub_entry, COPY_31_1)
	    }
	}
      else
	{
	  /* This is dyncall, so the code is a little different as the
	     return pointer is already in %r2 (aka RP).  */
	  if (rtn_adjust)
	    NEW_INSTRUCTION (stub_entry, ADDI_M4_31_RP)
	}

      /* Save the return address.  */
      if (linker_stub_type == HPPA_STUB_ARG_RELOC)
	NEW_INSTRUCTION (stub_entry, STW_RP_M8SP)

      /* Long branch to the target function.  */
      NEW_INSTRUCTION (stub_entry, LDIL_XXX_31)
      hppa_elf_stub_reloc (stub_entry->stub_desc,
			   abfd, orig_sym,
			   CURRENT_STUB_OFFSET (stub_entry),
			   R_PARISC_DIR21L);
      NEW_INSTRUCTION (stub_entry, BLE_XXX_0_31)
      hppa_elf_stub_reloc (stub_entry->stub_desc,
			   abfd, orig_sym,
			   CURRENT_STUB_OFFSET (stub_entry),
			   R_PARISC_DIR17R);

      if (linker_stub_type == HPPA_STUB_ARG_RELOC)
	{
	  /* In delay slot of long-call, copy %r31 into %r2 so that
	     the callee can return in the normal fashion.  */
	  NEW_INSTRUCTION (stub_entry, COPY_31_2)

	  /* Restore the return address.  */
	  NEW_INSTRUCTION (stub_entry, LDW_M8SP_RP)
      
	  /* Generate the code to move the return value around.  */
	    switch (stub_types[RETVAL])
	      {
	      case NO_ARG_RELOC:
		break;
	  
	      case R_TO_FR:
		NEW_INSTRUCTION (stub_entry, STWS_RET0_M8SP)
		NEW_INSTRUCTION (stub_entry, FLDWS_M8SP_FRET0)
		  break;
	  
	      case FR_TO_R:
		NEW_INSTRUCTION (stub_entry, FSTWS_FRET0_M8SP)
		NEW_INSTRUCTION (stub_entry, LDWS_M4SP_RET0)
		  break;

	      default:
		abort ();
		break;
	      }

	  /* Return back to the main code stream.  */
	  NEW_INSTRUCTION (stub_entry, BV_N_0_RP)
	}
      else
	{
	  if (!dyncall)
	    {
	      /* Get return address into %r31.  Both variants may be necessary
		 (I think) as we could be cascading into another stub.  */
	      if (!milli)
		NEW_INSTRUCTION (stub_entry, COPY_2_31)
	      else
		NEW_INSTRUCTION (stub_entry, COPY_1_31)
	    }
	  else
	    {
	      /* Get the return address into %r31 too.  Might be necessary
		 (I think) as we could be cascading into another stub.  */
	      NEW_INSTRUCTION (stub_entry, COPY_2_31)
	    }

	  /* No need for a return to the main stream.  */
	}
    }
  return stub_sym;
}

/* Return nonzero if an argument relocation will be needed to call
   the function (symbol in RELOC_ENTRY) assuming the caller has
   argument relocation bugs CALLER_AR.  */

static int
hppa_elf_arg_reloc_needed_p (abfd, reloc_entry, stub_types, caller_ar)
     bfd *abfd;
     arelent *reloc_entry;
     arg_reloc_type stub_types[5];
     symext_entryS caller_ar;
{
  /* If the symbol is still undefined, then it's impossible to know
     if an argument relocation is needed.  */
  if (reloc_entry->sym_ptr_ptr[0] 
      && reloc_entry->sym_ptr_ptr[0]->section != &bfd_und_section)
    {
      symext_entryS callee_ar = elf32_hppa_get_sym_extn (abfd,
						reloc_entry->sym_ptr_ptr[0],
							 PARISC_SXT_ARG_RELOC);

      /* Now examine all the argument and return value location
	 information to determine if a relocation stub will be needed.  */
      if (caller_ar && callee_ar)
	{
	  arg_location caller_loc[5];
	  arg_location callee_loc[5];

	  /* Extract the location information for the return value
	     and argument registers separately.  */
	  callee_loc[RETVAL] = EXTRACT_ARBITS (callee_ar, RETVAL);
	  caller_loc[RETVAL] = EXTRACT_ARBITS (caller_ar, RETVAL);
	  callee_loc[ARG0] = EXTRACT_ARBITS (callee_ar, ARG0);
	  caller_loc[ARG0] = EXTRACT_ARBITS (caller_ar, ARG0);
	  callee_loc[ARG1] = EXTRACT_ARBITS (callee_ar, ARG1);
	  caller_loc[ARG1] = EXTRACT_ARBITS (caller_ar, ARG1);
	  callee_loc[ARG2] = EXTRACT_ARBITS (callee_ar, ARG2);
	  caller_loc[ARG2] = EXTRACT_ARBITS (caller_ar, ARG2);
	  callee_loc[ARG3] = EXTRACT_ARBITS (callee_ar, ARG3);
	  caller_loc[ARG3] = EXTRACT_ARBITS (caller_ar, ARG3);

	  /* Check some special combinations.  For example, if FU 
	     appears in ARG1 or ARG3, we can move it to ARG0 or ARG2,
	     respectively.  (I guess this braindamage is correct?  It'd
	     take an hour or two of reading PA calling conventions to
	     really know).  */

	  if (caller_loc[ARG0] == AR_FU || caller_loc[ARG1] == AR_FU)
	    {
	      caller_loc[ARG0] = AR_DBL01;
	      caller_loc[ARG1] = AR_NO;
	    }
	  if (caller_loc[ARG2] == AR_FU || caller_loc[ARG3] == AR_FU)
	    {
	      caller_loc[ARG2] = AR_DBL23;
	      caller_loc[ARG3] = AR_NO;
	    }
	  if (callee_loc[ARG0] == AR_FU || callee_loc[ARG1] == AR_FU)
	    {
	      callee_loc[ARG0] = AR_DBL01;
	      callee_loc[ARG1] = AR_NO;
	    }
	  if (callee_loc[ARG2] == AR_FU || callee_loc[ARG3] == AR_FU)
	    {
	      callee_loc[ARG2] = AR_DBL23;
	      callee_loc[ARG3] = AR_NO;
	    }

	  /* Now look up potential mismatches.  */
	  stub_types[ARG0] = type_of_mismatch (caller_loc[ARG0],
					       callee_loc[ARG0],
					       ARGUMENTS);
	  stub_types[ARG1] = type_of_mismatch (caller_loc[ARG1],
					       callee_loc[ARG1],
					       ARGUMENTS);
	  stub_types[ARG2] = type_of_mismatch (caller_loc[ARG2],
					       callee_loc[ARG2],
					       ARGUMENTS);
	  stub_types[ARG3] = type_of_mismatch (caller_loc[ARG3],
					       callee_loc[ARG3],
					       ARGUMENTS);
	  stub_types[RETVAL] = type_of_mismatch (caller_loc[RETVAL],
						 callee_loc[RETVAL],
						 RETURN_VALUE);

	  /* If any of the arguments or return value need an argument
	     relocation, then we will need an argument relocation stub.  */
	  if (stub_types[ARG0] != NO_ARG_RELOC
	      || stub_types[ARG1] != NO_ARG_RELOC
	      || stub_types[ARG2] != NO_ARG_RELOC
	      || stub_types[ARG3] != NO_ARG_RELOC
	      || stub_types[RETVAL] != NO_ARG_RELOC)
	    return 1;
	}
    }
  return 0;
}

/* Create the linker stub section.  */

static void
hppa_elf_create_stub_sec (abfd, output_bfd, secptr, link_info)
     bfd *abfd;
     bfd *output_bfd;
     asection **secptr;
     struct bfd_link_info *link_info;
{
  asection *output_text_section;
  
  output_text_section = bfd_get_section_by_name (output_bfd, ".text");
  *secptr = bfd_make_section (abfd, ".PARISC.stubs");
  bfd_set_section_flags (abfd, *secptr,
			 SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD
			 | SEC_RELOC | SEC_CODE | SEC_READONLY);
  (*secptr)->output_section = output_text_section->output_section;
  (*secptr)->output_offset = 0;
  
  /* Set up the ELF section header for this new section.  This
     is basically the same processing as elf_make_sections().  
     elf_make_sections is static and therefore not accessable
     here.  */
  {
    Elf_Internal_Shdr *this_hdr;
    this_hdr = &elf_section_data ((*secptr))->this_hdr;
    
    /* Set the sizes of this section.  The contents have already
       been set up ?!?  */
    this_hdr->sh_addr = (*secptr)->vma;
    this_hdr->sh_size = (*secptr)->_raw_size;
    
    /* Set appropriate flags for sections with relocations.  */
    if ((*secptr)->flags & SEC_RELOC)
      {
	Elf_Internal_Shdr *rela_hdr;
	int use_rela_p = get_elf_backend_data (abfd)->use_rela_p;
	
	rela_hdr = &elf_section_data ((*secptr))->rel_hdr;
	
	if (use_rela_p)
	  {
	    rela_hdr->sh_type = SHT_RELA;
	    rela_hdr->sh_entsize = sizeof (Elf32_External_Rela);
	  }
	else
	  {
	    rela_hdr->sh_type = SHT_REL;
	    rela_hdr->sh_entsize = sizeof (Elf32_External_Rel);
	  }
	rela_hdr->sh_flags = 0;
	rela_hdr->sh_addr = 0;
	rela_hdr->sh_offset = 0;
	rela_hdr->sh_addralign = 0;
	rela_hdr->size = 0;
      }
    
    if ((*secptr)->flags & SEC_ALLOC)
      this_hdr->sh_flags |= SHF_ALLOC;
    
    if (!((*secptr)->flags & SEC_READONLY))
      this_hdr->sh_flags |= SHF_WRITE;
    
    if ((*secptr)->flags & SEC_CODE)
      this_hdr->sh_flags |= SHF_EXECINSTR;
  }
  
  bfd_set_section_alignment (abfd, *secptr, 2);
}

/* Return nonzero if a long-call stub will be needed to call the
   function (symbol in RELOC_ENTRY).  */

static int
hppa_elf_long_branch_needed_p (abfd, asec, reloc_entry, symbol, insn)
     bfd *abfd;
     asection *asec;
     arelent *reloc_entry;
     asymbol *symbol;
     unsigned insn;
{
  long sym_value = get_symbol_value (symbol);
  int fmt = reloc_entry->howto->bitsize;
  unsigned char op = get_opcode (insn);
  unsigned raddr;

#define too_far(val,num_bits) \
  ((int)(val) > (1 << (num_bits)) - 1) || ((int)(val) < (-1 << (num_bits)))

  switch (op)
    {
    case BL:
      raddr =
	reloc_entry->address + asec->output_offset + asec->output_section->vma;
      /* If the symbol and raddr (relocated addr?) are too far away from
	 each other, then a long-call stub will be needed.  */
      if (too_far (sym_value - raddr, fmt + 1))
	  return 1;
      break;
    }  
  return 0;
}

/* Search the given section and determine if linker stubs will be
   needed for any calls within that section.

   Return any new stub symbols created.

   Used out of hppaelf.em in the linker.  */
   
asymbol *
hppa_look_for_stubs_in_section (stub_bfd, abfd, output_bfd, asec,
				new_sym_cnt, link_info)
     bfd *stub_bfd;
     bfd *abfd;
     bfd *output_bfd;
     asection *asec;
     int *new_sym_cnt;
     struct bfd_link_info *link_info;
{
  int i;
  arg_reloc_type stub_types[5];
  asymbol *new_syms = NULL;
  int new_cnt = 0;
  int new_max = 0;
  arelent **reloc_vector = NULL;

  /* Relocations are in different places depending on whether this is
     an output section or an input section.  Also, the relocations are
     in different forms.  Sigh.  Luckily, we have bfd_canonicalize_reloc()
     to straighten this out for us . */
  if (asec->reloc_count > 0)
    {
      reloc_vector
	= (arelent **) malloc (asec->reloc_count * (sizeof (arelent *) + 1));
      if (reloc_vector == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  goto error_return;
	}

      /* Make sure the canonical symbols are hanging around in a convient
	 location.  */
      if (bfd_get_outsymbols (abfd) == NULL)
	{
	  long symsize;
	  long symcount;

	  symsize = bfd_get_symtab_upper_bound (abfd);
	  if (symsize < 0)
	    goto error_return;
	  abfd->outsymbols = (asymbol **) bfd_alloc (abfd, symsize);
	  if (!abfd->outsymbols && symsize != 0)
	    {
	      bfd_set_error (bfd_error_no_memory);
	      goto error_return;
	    }
	  symcount = bfd_canonicalize_symtab (abfd, abfd->outsymbols);
	  if (symcount < 0)
	    goto error_return;
	  abfd->symcount = symcount;
	}

      /* Now get the relocations.  */
      if (bfd_canonicalize_reloc (abfd, asec, reloc_vector,
				  bfd_get_outsymbols (abfd)) < 0)
	goto error_return;

      /* Examine each relocation entry in this section.  */
      for (i = 0; i < asec->reloc_count; i++)
	{
	  arelent *rle = reloc_vector[i];

	  switch (rle->howto->type)
	    {
	    /* Any call could need argument relocation stubs, and
	       some may need long-call stubs.  */
	    case R_PARISC_PCREL21L:
	    case R_PARISC_PCREL17R:
	    case R_PARISC_PCREL17F:
	    case R_PARISC_PCREL17C:
	    case R_PARISC_PCREL14R:
	    case R_PARISC_PCREL14F:
	      {
		symext_entryS caller_ar
		  = (symext_entryS) HPPA_R_ARG_RELOC (rle->addend);
		unsigned insn[2];

		/* We'll need this for the long-call checks.  */
		bfd_get_section_contents (abfd, asec, insn, rle->address,
					  sizeof(insn));

		/* See if this call needs an argument relocation stub.  */
		if (hppa_elf_arg_reloc_needed_p (abfd, rle, stub_types,
						 caller_ar))
		  {
		    /* Generate a stub and keep track of the new symbol.  */
		    asymbol *r;

		    if (new_cnt == new_max)
		      {
			new_max += STUB_SYM_BUFFER_INC;
			new_syms = (asymbol *)
			  realloc (new_syms, new_max * sizeof (asymbol));
			if (new_syms == NULL)
			  {
			    bfd_set_error (bfd_error_no_memory);
			    goto error_return;
			  }
		      }

		    /* Build the argument relocation stub.  */
		    r = hppa_elf_build_linker_stub (stub_bfd, output_bfd,
						    link_info, rle,
						    stub_types, true, insn, 
						    HPPA_STUB_ARG_RELOC);
		    new_syms[new_cnt++] = *r;
		  }

		/* See if this call needs a long-call stub.  */
		if (hppa_elf_long_branch_needed_p (abfd, asec, rle,
						   rle->sym_ptr_ptr[0],
						   insn[0]))
		  {
		    /* Generate a stub and keep track of the new symbol.  */
		    asymbol *r;

		    if (new_cnt == new_max)
		      {
			new_max += STUB_SYM_BUFFER_INC;
			new_syms = (asymbol *)
			  realloc (new_syms, (new_max * sizeof (asymbol)));
			if (! new_syms)
			  {
			    bfd_set_error (bfd_error_no_memory);
			    goto error_return;
			  }
		      }

		    /* Build the long-call stub.  */
		    r = hppa_elf_build_linker_stub (stub_bfd, output_bfd,
						    link_info, rle,
						    NULL, true, insn, 
						    HPPA_STUB_LONG_CALL);
		    new_syms[new_cnt++] = *r;
		  }
	      }
	      break;

	    /* PLABELs may need argument relocation stubs.  */
	    case R_PARISC_PLABEL32:
	    case R_PARISC_PLABEL21L:
	    case R_PARISC_PLABEL14R:
	      {
		/* On a plabel relocation, assume the arguments of the
		   caller are set up in general registers (indirect
		   calls only use general registers.
		   NOTE:  0x155 = ARGW0=GR,ARGW1=GR,ARGW2=GR,RETVAL=GR.  */
		symext_entryS caller_ar = (symext_entryS) 0x155;
		unsigned insn[2];

		/* Do we really need this?  */
		bfd_get_section_contents (abfd, asec, insn, rle->address,
					  sizeof(insn));

		/* See if this call needs an argument relocation stub.  */
		if (hppa_elf_arg_reloc_needed_p (abfd, rle, stub_types,
						 caller_ar))
		  {
		    /* Generate a plabel stub and keep track of the
		       new symbol.  */
		    asymbol *r;
		    int rtn_adjust;

		    if (new_cnt == new_max)
		      {
			new_max += STUB_SYM_BUFFER_INC;
			new_syms = (asymbol *) realloc (new_syms, new_max
							* sizeof (asymbol));
		      }

		    /* Determine whether a return adjustment
		       (see the relocation code for relocation type 
		       R_PARISC_STUB_CALL_17) is possible.  Basically,
		       determine whether we are looking at a branch or not.  */
		    if (rle->howto->type == R_PARISC_PLABEL32)
		      rtn_adjust = false;
		    else
		      {
			switch (get_opcode(insn[0]))
			  {
			  case BLE:
			  case BE:
			    rtn_adjust = true;
			    break;
			  default:
			    rtn_adjust = false;
			  }
		      }

		    /* Build the argument relocation stub.  */
		    r = hppa_elf_build_linker_stub (stub_bfd, output_bfd,
						    link_info, rle, stub_types,
						    rtn_adjust, insn,
						    HPPA_STUB_ARG_RELOC);
		    new_syms[new_cnt++] = *r;
		  }
	      }
	      break;

	    default:
	      break;
	    }
	}
    }

  if (reloc_vector != NULL)
    free (reloc_vector);
  /* Return the new symbols and update the counters.  */
  *new_sym_cnt = new_cnt;
  return new_syms;

 error_return:
  if (reloc_vector != NULL)
    free (reloc_vector);
  /* FIXME: This is bogus.  We should be returning NULL.  But do the callers
     check for that?  */
  abort ();
}

/* Set the contents of a particular section at a particular location.  */

static boolean
hppa_elf_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  /* Linker stubs are handled a little differently.  */
  if (! strcmp (section->name, ".PARISC.stubs"))
    {
      if (linker_stubs_max_size < offset + count)
	{
	  linker_stubs_max_size = offset + count + STUB_ALLOC_INCR;
	  linker_stubs = (char *)realloc (linker_stubs, linker_stubs_max_size);
	  if (! linker_stubs)
	    abort ();
	}

      if (offset + count > linker_stubs_size)
	linker_stubs_size = offset + count;

      /* Set the contents.  */
      memcpy(linker_stubs + offset, location, count);
      return (true);
    }
  /* Ignore write requests for the symbol extension section until we've
     had the chance to rebuild it ourselves.  */
  else if (! strcmp (section->name, ".PARISC.symextn") && !symext_chain_size)
    return true;
  else
    return bfd_elf32_set_section_contents (abfd, section, location,
					   offset, count);
}

/* Get the contents of the given section.
   
   This is special for PA ELF because some sections (such as linker stubs)
   may reside in memory rather than on disk, or in the case of the symbol
   extension section, the contents may need to be generated from other
   information contained in the BFD.  */

boolean
hppa_elf_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  /* If this is the linker stub section, then its contents are contained
     in memory rather than on disk.  FIXME.  Is that always right?  What
     about the case where a final executable is read in and a user tries
     to get the contents of this section?  In that case the contents would
     be on disk like everything else.  */
  if (strcmp (section->name, ".PARISC.stubs") == 0)
    {
      elf32_hppa_stub_description *stub_desc = find_stubs (abfd, section);
      
      if (count == 0)
	return true;
      
      /* Sanity check our arguments.  */
      if ((bfd_size_type) (offset + count) > section->_raw_size
	  || (bfd_size_type) (offset + count) > stub_desc->real_size)
	return (false);
      
      memcpy (location, stub_desc->stub_contents + offset, count);
      return (true);
    }
  else
    /* It's not the linker stub section, use the generic routines.  */
    return _bfd_generic_get_section_contents (abfd, section, location,
					      offset, count);
}

/* Translate from an elf into field into a howto relocation pointer.  */

static void
elf_info_to_howto (abfd, cache_ptr, dst)
     bfd *abfd;
     arelent *cache_ptr;
     Elf32_Internal_Rela *dst;
{
  BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_PARISC_UNIMPLEMENTED);
  cache_ptr->howto = &elf_hppa_howto_table[ELF32_R_TYPE (dst->r_info)];
}

/* Do PA ELF specific processing for symbols.  Needed to find the
   value of $global$.  */

static void
elf32_hppa_backend_symbol_processing (abfd, sym)
     bfd *abfd;
     asymbol *sym;
{
  /* Is this a definition of $global$?  If so, keep it because it will be
    needed if any relocations are performed.  */
  if (!strcmp (sym->name, "$global$")
      && sym->section != &bfd_und_section)
    {
      global_symbol = sym;
    }
}

/* Do some PA ELF specific work after reading in the symbol table.
   In particular attach the argument relocation from the 
   symbol extension section to the appropriate symbols.  */
static boolean
elf32_hppa_backend_symbol_table_processing (abfd, esyms,symcnt)
     bfd *abfd;
     elf_symbol_type *esyms;
     int symcnt;
{
  Elf32_Internal_Shdr *symextn_hdr =
    bfd_elf_find_section (abfd, SYMEXTN_SECTION_NAME);
  int i, current_sym_idx = 0;

  /* If no symbol extension existed, then all symbol extension information
     is assumed to be zero.  */
  if (symextn_hdr == NULL)
    {
      for (i = 0; i < symcnt; i++)
	esyms[i].tc_data.hppa_arg_reloc = 0;
      return (true);
    }

  /* Allocate a buffer of the appropriate size for the symextn section.  */
  symextn_hdr->contents = bfd_zalloc(abfd,symextn_hdr->sh_size);
  if (!symextn_hdr->contents)
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  symextn_hdr->size = symextn_hdr->sh_size;
	
  /* Read in the symextn section.  */
  if (bfd_seek (abfd, symextn_hdr->sh_offset, SEEK_SET) == -1)
    return false;
  if (bfd_read ((PTR) symextn_hdr->contents, 1, symextn_hdr->size, abfd) 
      != symextn_hdr->size)
    return false;

  /* Parse entries in the symbol extension section, updating the symtab
     entries as we go */
  for (i = 0; i < symextn_hdr->size / sizeof(symext_entryS); i++)
    {
      symext_entryS *seP = ((symext_entryS *)symextn_hdr->contents) + i;
      int se_value = ELF32_PARISC_SX_VAL (*seP);
      int se_type = ELF32_PARISC_SX_TYPE (*seP);

      switch (se_type)
	{
	case PARISC_SXT_NULL:
	  break;

	case PARISC_SXT_SYMNDX:
	  if (se_value >= symcnt)
	    {
	      bfd_set_error (bfd_error_bad_value);
	      return (false);
	    }
	  current_sym_idx = se_value - 1;
	  break;

	case PARISC_SXT_ARG_RELOC:
	  esyms[current_sym_idx].tc_data.hppa_arg_reloc = se_value;
	  break;

	default:
	  bfd_set_error (bfd_error_bad_value);
	  return (false);
	}
    }
  return (true);
}

/* Perform on PA ELF specific processing once a section has been
   read in.  In particular keep the symbol indexes correct for
   the symbol extension information.  */

static boolean
elf32_hppa_backend_section_processing (abfd, secthdr)
     bfd *abfd;
     Elf32_Internal_Shdr *secthdr;
{
  int i, j, k;

  if (secthdr->sh_type == SHT_PARISC_SYMEXTN)
    {
      for (i = 0; i < secthdr->size / sizeof (symext_entryS); i++)
	{
	  symext_entryS *seP = ((symext_entryS *)secthdr->contents) + i;
	  int se_value = ELF32_PARISC_SX_VAL (*seP);
	  int se_type = ELF32_PARISC_SX_TYPE (*seP);
	  
	  switch (se_type)
	    {
	    case PARISC_SXT_NULL:
	      break;
	      
	    case PARISC_SXT_SYMNDX:
	      for (j = 0; j < abfd->symcount; j++)
		{
		  /* Locate the map entry for this symbol and modify the
		     symbol extension section symbol index entry to reflect
		     the new symbol table index.  */
		  for (k = 0; k < elf32_hppa_symextn_map_size; k++)
		    {
		      if (elf32_hppa_symextn_map[k].old_index == se_value
			  && elf32_hppa_symextn_map[k].bfd
			     == abfd->outsymbols[j]->the_bfd
			  && elf32_hppa_symextn_map[k].sym
			     == abfd->outsymbols[j])
			{
			  bfd_put_32(abfd,
				     ELF32_PARISC_SX_WORD (PARISC_SXT_SYMNDX, j),
				     (char *)seP);
			}
		    }
		}
	      break;
	      
	    case PARISC_SXT_ARG_RELOC:
	      break;
	      
	    default:
	      bfd_set_error (bfd_error_bad_value);
	      return (false);
	    }
	}
    }
  return true;
}

/* What does this really do?  Just determine if there is an appropriate
   mapping from ELF section headers to backend sections?  More symbol
   extension braindamage.  */

static boolean
elf32_hppa_backend_section_from_shdr (abfd, hdr, name)
     bfd *abfd;
     Elf32_Internal_Shdr *hdr;
     char *name;
{
  asection *newsect;

  if (hdr->sh_type == SHT_PARISC_SYMEXTN)
    {
      BFD_ASSERT (strcmp (name, ".PARISC.symextn") == 0);

      /* Bits that get saved. This one is real.  */
      if (!hdr->rawdata)
	{
	  newsect = bfd_make_section (abfd, name);
	  if (newsect != NULL)
	    {
	      newsect->vma = hdr->sh_addr;
	      newsect->_raw_size = hdr->sh_size;
	      newsect->filepos = hdr->sh_offset;
	      newsect->flags |= SEC_HAS_CONTENTS;
	      newsect->alignment_power = hdr->sh_addralign;

	      if (hdr->sh_flags & SHF_ALLOC)
		{
		  newsect->flags |= SEC_ALLOC;
		  newsect->flags |= SEC_LOAD;
		}

	      if (!(hdr->sh_flags & SHF_WRITE))
		newsect->flags |= SEC_READONLY;

	      if (hdr->sh_flags & SHF_EXECINSTR)
		newsect->flags |= SEC_CODE;
	      else
		newsect->flags |= SEC_DATA;

	      hdr->rawdata = (void *) newsect;
	    }
	}
      return true;
    }
  return false;
}

/* Return true if the given section is a fake section.  */

static boolean
elf32_hppa_backend_fake_sections (abfd, secthdr, asect)
     bfd *abfd;
     Elf_Internal_Shdr *secthdr;
     asection *asect;
{

  if (strcmp(asect->name, ".PARISC.symextn") == 0)
    {
      secthdr->sh_type = SHT_PARISC_SYMEXTN;
      secthdr->sh_flags = 0;
      secthdr->sh_info = elf_section_data(asect)->rel_hdr.sh_link;
      secthdr->sh_link = elf_onesymtab(abfd);
      return true;
    }

  if (!strcmp (asect->name, ".PARISC.unwind"))
    {
      secthdr->sh_type = SHT_PROGBITS;
      /* Unwind descriptors are not part of the program memory image.  */
      secthdr->sh_flags = 0;
      secthdr->sh_info = 0;
      secthdr->sh_link = 0;
      secthdr->sh_entsize = 16;
      return true;
    }

  /* @@ Should this be CPU specific??  KR */
  if (!strcmp (asect->name, ".stabstr"))
    {
      secthdr->sh_type = SHT_STRTAB;
      secthdr->sh_flags = 0;
      secthdr->sh_info = 0;
      secthdr->sh_link = 0;
      secthdr->sh_entsize = 0;
      return true;
    }

  return false;
}

/* Return true if there is a mapping from bfd section into a
   backend section.  */

static boolean
elf32_hppa_backend_section_from_bfd_section (abfd, hdr, asect, ignored)
     bfd *abfd;
     Elf32_Internal_Shdr *hdr;
     asection *asect;
     int *ignored;
{
  if (hdr->sh_type == SHT_PARISC_SYMEXTN)
    {
      if (hdr->rawdata)
	{
	  if (((struct sec *) (hdr->rawdata)) == asect)
	    {
	      BFD_ASSERT (strcmp (asect->name, ".PARISC.symextn") == 0);
	      return true;
	    }
	}
    }
  else if (hdr->sh_type == SHT_STRTAB)
    {
      if (hdr->rawdata)
	{
	  if (((struct sec *) (hdr->rawdata)) == asect)
	    {
	      BFD_ASSERT (strcmp (asect->name, ".stabstr") == 0);
	      return true;
	    }
	}
    }

  return false;
}

#define bfd_elf32_bfd_reloc_type_lookup	elf_hppa_reloc_type_lookup
#define elf_backend_section_from_bfd_section	elf32_hppa_backend_section_from_bfd_section

#define elf_backend_symbol_processing	elf32_hppa_backend_symbol_processing
#define elf_backend_symbol_table_processing	elf32_hppa_backend_symbol_table_processing

#define bfd_elf32_get_section_contents		hppa_elf_get_section_contents
#define bfd_elf32_set_section_contents		hppa_elf_set_section_contents
#define bfd_elf32_bfd_is_local_label		hppa_elf_is_local_label

#define elf_backend_section_processing	elf32_hppa_backend_section_processing

#define elf_backend_section_from_shdr	elf32_hppa_backend_section_from_shdr
#define elf_backend_fake_sections	elf32_hppa_backend_fake_sections
#define elf_backend_begin_write_processing \
  elf32_hppa_backend_begin_write_processing
#define elf_backend_final_write_processing \
  elf32_hppa_backend_final_write_processing

#define TARGET_BIG_SYM		bfd_elf32_hppa_vec
#define TARGET_BIG_NAME		"elf32-hppa"
#define ELF_ARCH		bfd_arch_hppa
#define ELF_MACHINE_CODE	EM_PARISC
#define ELF_MAXPAGESIZE		0x1000

#include "elf32-target.h"
