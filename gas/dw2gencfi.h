/* dw2gencfi.h - Support for generating Dwarf2 CFI information.
   Copyright 2003 Free Software Foundation, Inc.
   Contributed by Michal Ludvig <mludvig@suse.cz>

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#ifndef DW2GENCFI_H
#define DW2GENCFI_H

#include "elf/dwarf2.h"

struct cfi_config {
  /* Target address length in bytes. (usually 4 or 8).
     Round it up for archs like S/390 with 31b addresses.  */
  unsigned int addr_length;

  /* Alignment of .eh_frame blocks in bytes (usually 1, 4 or 8).  */
  unsigned int eh_align;

  /* Code alignment (1 for x86/amd64 machines, 4 or 8 for
     RISC machines). Consult Dwarf2 standard for details.  */
  int code_align;

  /* Data (stack) alignment (-4 on x86, -8 on amd64, something
     positive on archs where stack grows up).  Consult Dwarf2
     standard for details.  */
  int data_align;

  /* Return address column (0x8 on x86, 0x10 on amd64).  Consult
     Dwarf2 standard for details.  */
  int ra_column;

  /* Relocation type for init_addr FDE record. (BFD_RELOC_64
     on amd64).  */
  int reloc_type;
};

/* Codes of CFI instructions taken from Dwarf2 standard.  */
enum cfi_insn {
  CFA_nop = DW_CFA_nop,
  CFA_set_loc = DW_CFA_set_loc,
  CFA_advance_loc1 = DW_CFA_advance_loc1,
  CFA_advance_loc2 = DW_CFA_advance_loc2,
  CFA_advance_loc4 = DW_CFA_advance_loc4,
  CFA_offset_extended = DW_CFA_offset_extended,
  CFA_resotre_extended = DW_CFA_restore_extended,
  CFA_undefined = DW_CFA_undefined,
  CFA_same_value = DW_CFA_same_value,
  CFA_register = DW_CFA_register,
  CFA_remember_state = DW_CFA_remember_state,
  CFA_restore_state = DW_CFA_restore_state,
  CFA_def_cfa = DW_CFA_def_cfa,
  CFA_def_cfa_register = DW_CFA_def_cfa_register,
  CFA_def_cfa_offset = DW_CFA_def_cfa_offset,
  CFA_advance_loc = DW_CFA_advance_loc,
  CFA_offset = DW_CFA_offset,
  CFA_restore = DW_CFA_restore,

  /* These don't belong to the standard.  */
  CFI_startproc = 0xff00,
  CFI_endproc = 0xff01,
  CFI_adjust_cfa_offset = 0xff10,
  CFI_verbose = 0xffff
};

extern const pseudo_typeS cfi_pseudo_table[];

/* Insert .cfi_* directives to the list of pseudo-ops.  */
void cfi_pop_insert PARAMS ((void));

/* Set/change setup of the CFI machinery.  This change won't
   affect already generated CIEs/FDEs.  */
void cfi_set_config PARAMS ((struct cfi_config *cfg));

/* cfi_finish() is called at the end of file. It will complain if
   the last CFI wasn't properly closed by .cfi_endproc.  */
void cfi_finish PARAMS ((void));

/* Add CFI instruction to the list of instructions
   of the current frame. cfi_add_insn() could be used
   in tc_cfi_frame_initial_instructions() to add instructions
   needed for every frame (ie. those that usually go to CIE).  */
void cfi_add_insn (enum cfi_insn insn, long param0, long param1);

#endif /* DW2GENCFI_H */
