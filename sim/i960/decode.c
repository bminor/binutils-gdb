/* Simulator instruction decoder for i960base.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation, Inc.

This file is part of the GNU Simulators.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*/

#define WANT_CPU i960base
#define WANT_CPU_I960BASE

#include "sim-main.h"
#include "sim-assert.h"

/* FIXME: Need to review choices for the following.  */

#if WITH_SEM_SWITCH_FULL
#define FULL(fn)
#else
#define FULL(fn) CONCAT3 (i960base,_sem_,fn) ,
#endif

#if WITH_FAST
#if WITH_SEM_SWITCH_FAST
#define FAST(fn)
#else
#define FAST(fn) CONCAT3 (i960base,_semf_,fn) , /* f for fast */
#endif
#else
#define FAST(fn)
#endif

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC i960base_insn_data[I960BASE_INSN_MAX];

/* The INSN_ prefix is not here and is instead part of the `insn' argument
   to avoid collisions with header files (e.g. `AND' in ansidecl.h).  */
#define IDX(insn) CONCAT2 (I960BASE_,insn)
#define TYPE(insn) CONCAT2 (I960_,insn)

/* Commas between elements are contained in the macros.
   Some of these are conditionally compiled out.  */

static const struct insn_sem i960base_insn_sem[] =
{
  { VIRTUAL_INSN_X_INVALID, IDX (INSN_X_INVALID), FULL (x_invalid) FAST (x_invalid) },
  { VIRTUAL_INSN_X_AFTER, IDX (INSN_X_AFTER), FULL (x_after) FAST (x_after) },
  { VIRTUAL_INSN_X_BEFORE, IDX (INSN_X_BEFORE), FULL (x_before) FAST (x_before) },
  { VIRTUAL_INSN_X_CTI_CHAIN, IDX (INSN_X_CTI_CHAIN), FULL (x_cti_chain) FAST (x_cti_chain) },
  { VIRTUAL_INSN_X_CHAIN, IDX (INSN_X_CHAIN), FULL (x_chain) FAST (x_chain) },
  { VIRTUAL_INSN_X_BEGIN, IDX (INSN_X_BEGIN), FULL (x_begin) FAST (x_begin) },
  { TYPE (INSN_MULO), IDX (INSN_MULO), FULL (mulo) FAST (mulo) },
  { TYPE (INSN_MULO1), IDX (INSN_MULO1), FULL (mulo1) FAST (mulo1) },
  { TYPE (INSN_MULO2), IDX (INSN_MULO2), FULL (mulo2) FAST (mulo2) },
  { TYPE (INSN_MULO3), IDX (INSN_MULO3), FULL (mulo3) FAST (mulo3) },
  { TYPE (INSN_REMO), IDX (INSN_REMO), FULL (remo) FAST (remo) },
  { TYPE (INSN_REMO1), IDX (INSN_REMO1), FULL (remo1) FAST (remo1) },
  { TYPE (INSN_REMO2), IDX (INSN_REMO2), FULL (remo2) FAST (remo2) },
  { TYPE (INSN_REMO3), IDX (INSN_REMO3), FULL (remo3) FAST (remo3) },
  { TYPE (INSN_DIVO), IDX (INSN_DIVO), FULL (divo) FAST (divo) },
  { TYPE (INSN_DIVO1), IDX (INSN_DIVO1), FULL (divo1) FAST (divo1) },
  { TYPE (INSN_DIVO2), IDX (INSN_DIVO2), FULL (divo2) FAST (divo2) },
  { TYPE (INSN_DIVO3), IDX (INSN_DIVO3), FULL (divo3) FAST (divo3) },
  { TYPE (INSN_REMI), IDX (INSN_REMI), FULL (remi) FAST (remi) },
  { TYPE (INSN_REMI1), IDX (INSN_REMI1), FULL (remi1) FAST (remi1) },
  { TYPE (INSN_REMI2), IDX (INSN_REMI2), FULL (remi2) FAST (remi2) },
  { TYPE (INSN_REMI3), IDX (INSN_REMI3), FULL (remi3) FAST (remi3) },
  { TYPE (INSN_DIVI), IDX (INSN_DIVI), FULL (divi) FAST (divi) },
  { TYPE (INSN_DIVI1), IDX (INSN_DIVI1), FULL (divi1) FAST (divi1) },
  { TYPE (INSN_DIVI2), IDX (INSN_DIVI2), FULL (divi2) FAST (divi2) },
  { TYPE (INSN_DIVI3), IDX (INSN_DIVI3), FULL (divi3) FAST (divi3) },
  { TYPE (INSN_ADDO), IDX (INSN_ADDO), FULL (addo) FAST (addo) },
  { TYPE (INSN_ADDO1), IDX (INSN_ADDO1), FULL (addo1) FAST (addo1) },
  { TYPE (INSN_ADDO2), IDX (INSN_ADDO2), FULL (addo2) FAST (addo2) },
  { TYPE (INSN_ADDO3), IDX (INSN_ADDO3), FULL (addo3) FAST (addo3) },
  { TYPE (INSN_SUBO), IDX (INSN_SUBO), FULL (subo) FAST (subo) },
  { TYPE (INSN_SUBO1), IDX (INSN_SUBO1), FULL (subo1) FAST (subo1) },
  { TYPE (INSN_SUBO2), IDX (INSN_SUBO2), FULL (subo2) FAST (subo2) },
  { TYPE (INSN_SUBO3), IDX (INSN_SUBO3), FULL (subo3) FAST (subo3) },
  { TYPE (INSN_NOTBIT), IDX (INSN_NOTBIT), FULL (notbit) FAST (notbit) },
  { TYPE (INSN_NOTBIT1), IDX (INSN_NOTBIT1), FULL (notbit1) FAST (notbit1) },
  { TYPE (INSN_NOTBIT2), IDX (INSN_NOTBIT2), FULL (notbit2) FAST (notbit2) },
  { TYPE (INSN_NOTBIT3), IDX (INSN_NOTBIT3), FULL (notbit3) FAST (notbit3) },
  { TYPE (INSN_AND), IDX (INSN_AND), FULL (and) FAST (and) },
  { TYPE (INSN_AND1), IDX (INSN_AND1), FULL (and1) FAST (and1) },
  { TYPE (INSN_AND2), IDX (INSN_AND2), FULL (and2) FAST (and2) },
  { TYPE (INSN_AND3), IDX (INSN_AND3), FULL (and3) FAST (and3) },
  { TYPE (INSN_ANDNOT), IDX (INSN_ANDNOT), FULL (andnot) FAST (andnot) },
  { TYPE (INSN_ANDNOT1), IDX (INSN_ANDNOT1), FULL (andnot1) FAST (andnot1) },
  { TYPE (INSN_ANDNOT2), IDX (INSN_ANDNOT2), FULL (andnot2) FAST (andnot2) },
  { TYPE (INSN_ANDNOT3), IDX (INSN_ANDNOT3), FULL (andnot3) FAST (andnot3) },
  { TYPE (INSN_SETBIT), IDX (INSN_SETBIT), FULL (setbit) FAST (setbit) },
  { TYPE (INSN_SETBIT1), IDX (INSN_SETBIT1), FULL (setbit1) FAST (setbit1) },
  { TYPE (INSN_SETBIT2), IDX (INSN_SETBIT2), FULL (setbit2) FAST (setbit2) },
  { TYPE (INSN_SETBIT3), IDX (INSN_SETBIT3), FULL (setbit3) FAST (setbit3) },
  { TYPE (INSN_NOTAND), IDX (INSN_NOTAND), FULL (notand) FAST (notand) },
  { TYPE (INSN_NOTAND1), IDX (INSN_NOTAND1), FULL (notand1) FAST (notand1) },
  { TYPE (INSN_NOTAND2), IDX (INSN_NOTAND2), FULL (notand2) FAST (notand2) },
  { TYPE (INSN_NOTAND3), IDX (INSN_NOTAND3), FULL (notand3) FAST (notand3) },
  { TYPE (INSN_XOR), IDX (INSN_XOR), FULL (xor) FAST (xor) },
  { TYPE (INSN_XOR1), IDX (INSN_XOR1), FULL (xor1) FAST (xor1) },
  { TYPE (INSN_XOR2), IDX (INSN_XOR2), FULL (xor2) FAST (xor2) },
  { TYPE (INSN_XOR3), IDX (INSN_XOR3), FULL (xor3) FAST (xor3) },
  { TYPE (INSN_OR), IDX (INSN_OR), FULL (or) FAST (or) },
  { TYPE (INSN_OR1), IDX (INSN_OR1), FULL (or1) FAST (or1) },
  { TYPE (INSN_OR2), IDX (INSN_OR2), FULL (or2) FAST (or2) },
  { TYPE (INSN_OR3), IDX (INSN_OR3), FULL (or3) FAST (or3) },
  { TYPE (INSN_NOR), IDX (INSN_NOR), FULL (nor) FAST (nor) },
  { TYPE (INSN_NOR1), IDX (INSN_NOR1), FULL (nor1) FAST (nor1) },
  { TYPE (INSN_NOR2), IDX (INSN_NOR2), FULL (nor2) FAST (nor2) },
  { TYPE (INSN_NOR3), IDX (INSN_NOR3), FULL (nor3) FAST (nor3) },
  { TYPE (INSN_NOT), IDX (INSN_NOT), FULL (not) FAST (not) },
  { TYPE (INSN_NOT1), IDX (INSN_NOT1), FULL (not1) FAST (not1) },
  { TYPE (INSN_NOT2), IDX (INSN_NOT2), FULL (not2) FAST (not2) },
  { TYPE (INSN_NOT3), IDX (INSN_NOT3), FULL (not3) FAST (not3) },
  { TYPE (INSN_CLRBIT), IDX (INSN_CLRBIT), FULL (clrbit) FAST (clrbit) },
  { TYPE (INSN_CLRBIT1), IDX (INSN_CLRBIT1), FULL (clrbit1) FAST (clrbit1) },
  { TYPE (INSN_CLRBIT2), IDX (INSN_CLRBIT2), FULL (clrbit2) FAST (clrbit2) },
  { TYPE (INSN_CLRBIT3), IDX (INSN_CLRBIT3), FULL (clrbit3) FAST (clrbit3) },
  { TYPE (INSN_SHLO), IDX (INSN_SHLO), FULL (shlo) FAST (shlo) },
  { TYPE (INSN_SHLO1), IDX (INSN_SHLO1), FULL (shlo1) FAST (shlo1) },
  { TYPE (INSN_SHLO2), IDX (INSN_SHLO2), FULL (shlo2) FAST (shlo2) },
  { TYPE (INSN_SHLO3), IDX (INSN_SHLO3), FULL (shlo3) FAST (shlo3) },
  { TYPE (INSN_SHRO), IDX (INSN_SHRO), FULL (shro) FAST (shro) },
  { TYPE (INSN_SHRO1), IDX (INSN_SHRO1), FULL (shro1) FAST (shro1) },
  { TYPE (INSN_SHRO2), IDX (INSN_SHRO2), FULL (shro2) FAST (shro2) },
  { TYPE (INSN_SHRO3), IDX (INSN_SHRO3), FULL (shro3) FAST (shro3) },
  { TYPE (INSN_SHLI), IDX (INSN_SHLI), FULL (shli) FAST (shli) },
  { TYPE (INSN_SHLI1), IDX (INSN_SHLI1), FULL (shli1) FAST (shli1) },
  { TYPE (INSN_SHLI2), IDX (INSN_SHLI2), FULL (shli2) FAST (shli2) },
  { TYPE (INSN_SHLI3), IDX (INSN_SHLI3), FULL (shli3) FAST (shli3) },
  { TYPE (INSN_SHRI), IDX (INSN_SHRI), FULL (shri) FAST (shri) },
  { TYPE (INSN_SHRI1), IDX (INSN_SHRI1), FULL (shri1) FAST (shri1) },
  { TYPE (INSN_SHRI2), IDX (INSN_SHRI2), FULL (shri2) FAST (shri2) },
  { TYPE (INSN_SHRI3), IDX (INSN_SHRI3), FULL (shri3) FAST (shri3) },
  { TYPE (INSN_EMUL), IDX (INSN_EMUL), FULL (emul) FAST (emul) },
  { TYPE (INSN_EMUL1), IDX (INSN_EMUL1), FULL (emul1) FAST (emul1) },
  { TYPE (INSN_EMUL2), IDX (INSN_EMUL2), FULL (emul2) FAST (emul2) },
  { TYPE (INSN_EMUL3), IDX (INSN_EMUL3), FULL (emul3) FAST (emul3) },
  { TYPE (INSN_MOV), IDX (INSN_MOV), FULL (mov) FAST (mov) },
  { TYPE (INSN_MOV1), IDX (INSN_MOV1), FULL (mov1) FAST (mov1) },
  { TYPE (INSN_MOVL), IDX (INSN_MOVL), FULL (movl) FAST (movl) },
  { TYPE (INSN_MOVL1), IDX (INSN_MOVL1), FULL (movl1) FAST (movl1) },
  { TYPE (INSN_MOVT), IDX (INSN_MOVT), FULL (movt) FAST (movt) },
  { TYPE (INSN_MOVT1), IDX (INSN_MOVT1), FULL (movt1) FAST (movt1) },
  { TYPE (INSN_MOVQ), IDX (INSN_MOVQ), FULL (movq) FAST (movq) },
  { TYPE (INSN_MOVQ1), IDX (INSN_MOVQ1), FULL (movq1) FAST (movq1) },
  { TYPE (INSN_MODPC), IDX (INSN_MODPC), FULL (modpc) FAST (modpc) },
  { TYPE (INSN_MODAC), IDX (INSN_MODAC), FULL (modac) FAST (modac) },
  { TYPE (INSN_LDA_OFFSET), IDX (INSN_LDA_OFFSET), FULL (lda_offset) FAST (lda_offset) },
  { TYPE (INSN_LDA_INDIRECT_OFFSET), IDX (INSN_LDA_INDIRECT_OFFSET), FULL (lda_indirect_offset) FAST (lda_indirect_offset) },
  { TYPE (INSN_LDA_INDIRECT), IDX (INSN_LDA_INDIRECT), FULL (lda_indirect) FAST (lda_indirect) },
  { TYPE (INSN_LDA_INDIRECT_INDEX), IDX (INSN_LDA_INDIRECT_INDEX), FULL (lda_indirect_index) FAST (lda_indirect_index) },
  { TYPE (INSN_LDA_DISP), IDX (INSN_LDA_DISP), FULL (lda_disp) FAST (lda_disp) },
  { TYPE (INSN_LDA_INDIRECT_DISP), IDX (INSN_LDA_INDIRECT_DISP), FULL (lda_indirect_disp) FAST (lda_indirect_disp) },
  { TYPE (INSN_LDA_INDEX_DISP), IDX (INSN_LDA_INDEX_DISP), FULL (lda_index_disp) FAST (lda_index_disp) },
  { TYPE (INSN_LDA_INDIRECT_INDEX_DISP), IDX (INSN_LDA_INDIRECT_INDEX_DISP), FULL (lda_indirect_index_disp) FAST (lda_indirect_index_disp) },
  { TYPE (INSN_LD_OFFSET), IDX (INSN_LD_OFFSET), FULL (ld_offset) FAST (ld_offset) },
  { TYPE (INSN_LD_INDIRECT_OFFSET), IDX (INSN_LD_INDIRECT_OFFSET), FULL (ld_indirect_offset) FAST (ld_indirect_offset) },
  { TYPE (INSN_LD_INDIRECT), IDX (INSN_LD_INDIRECT), FULL (ld_indirect) FAST (ld_indirect) },
  { TYPE (INSN_LD_INDIRECT_INDEX), IDX (INSN_LD_INDIRECT_INDEX), FULL (ld_indirect_index) FAST (ld_indirect_index) },
  { TYPE (INSN_LD_DISP), IDX (INSN_LD_DISP), FULL (ld_disp) FAST (ld_disp) },
  { TYPE (INSN_LD_INDIRECT_DISP), IDX (INSN_LD_INDIRECT_DISP), FULL (ld_indirect_disp) FAST (ld_indirect_disp) },
  { TYPE (INSN_LD_INDEX_DISP), IDX (INSN_LD_INDEX_DISP), FULL (ld_index_disp) FAST (ld_index_disp) },
  { TYPE (INSN_LD_INDIRECT_INDEX_DISP), IDX (INSN_LD_INDIRECT_INDEX_DISP), FULL (ld_indirect_index_disp) FAST (ld_indirect_index_disp) },
  { TYPE (INSN_LDOB_OFFSET), IDX (INSN_LDOB_OFFSET), FULL (ldob_offset) FAST (ldob_offset) },
  { TYPE (INSN_LDOB_INDIRECT_OFFSET), IDX (INSN_LDOB_INDIRECT_OFFSET), FULL (ldob_indirect_offset) FAST (ldob_indirect_offset) },
  { TYPE (INSN_LDOB_INDIRECT), IDX (INSN_LDOB_INDIRECT), FULL (ldob_indirect) FAST (ldob_indirect) },
  { TYPE (INSN_LDOB_INDIRECT_INDEX), IDX (INSN_LDOB_INDIRECT_INDEX), FULL (ldob_indirect_index) FAST (ldob_indirect_index) },
  { TYPE (INSN_LDOB_DISP), IDX (INSN_LDOB_DISP), FULL (ldob_disp) FAST (ldob_disp) },
  { TYPE (INSN_LDOB_INDIRECT_DISP), IDX (INSN_LDOB_INDIRECT_DISP), FULL (ldob_indirect_disp) FAST (ldob_indirect_disp) },
  { TYPE (INSN_LDOB_INDEX_DISP), IDX (INSN_LDOB_INDEX_DISP), FULL (ldob_index_disp) FAST (ldob_index_disp) },
  { TYPE (INSN_LDOB_INDIRECT_INDEX_DISP), IDX (INSN_LDOB_INDIRECT_INDEX_DISP), FULL (ldob_indirect_index_disp) FAST (ldob_indirect_index_disp) },
  { TYPE (INSN_LDOS_OFFSET), IDX (INSN_LDOS_OFFSET), FULL (ldos_offset) FAST (ldos_offset) },
  { TYPE (INSN_LDOS_INDIRECT_OFFSET), IDX (INSN_LDOS_INDIRECT_OFFSET), FULL (ldos_indirect_offset) FAST (ldos_indirect_offset) },
  { TYPE (INSN_LDOS_INDIRECT), IDX (INSN_LDOS_INDIRECT), FULL (ldos_indirect) FAST (ldos_indirect) },
  { TYPE (INSN_LDOS_INDIRECT_INDEX), IDX (INSN_LDOS_INDIRECT_INDEX), FULL (ldos_indirect_index) FAST (ldos_indirect_index) },
  { TYPE (INSN_LDOS_DISP), IDX (INSN_LDOS_DISP), FULL (ldos_disp) FAST (ldos_disp) },
  { TYPE (INSN_LDOS_INDIRECT_DISP), IDX (INSN_LDOS_INDIRECT_DISP), FULL (ldos_indirect_disp) FAST (ldos_indirect_disp) },
  { TYPE (INSN_LDOS_INDEX_DISP), IDX (INSN_LDOS_INDEX_DISP), FULL (ldos_index_disp) FAST (ldos_index_disp) },
  { TYPE (INSN_LDOS_INDIRECT_INDEX_DISP), IDX (INSN_LDOS_INDIRECT_INDEX_DISP), FULL (ldos_indirect_index_disp) FAST (ldos_indirect_index_disp) },
  { TYPE (INSN_LDIB_OFFSET), IDX (INSN_LDIB_OFFSET), FULL (ldib_offset) FAST (ldib_offset) },
  { TYPE (INSN_LDIB_INDIRECT_OFFSET), IDX (INSN_LDIB_INDIRECT_OFFSET), FULL (ldib_indirect_offset) FAST (ldib_indirect_offset) },
  { TYPE (INSN_LDIB_INDIRECT), IDX (INSN_LDIB_INDIRECT), FULL (ldib_indirect) FAST (ldib_indirect) },
  { TYPE (INSN_LDIB_INDIRECT_INDEX), IDX (INSN_LDIB_INDIRECT_INDEX), FULL (ldib_indirect_index) FAST (ldib_indirect_index) },
  { TYPE (INSN_LDIB_DISP), IDX (INSN_LDIB_DISP), FULL (ldib_disp) FAST (ldib_disp) },
  { TYPE (INSN_LDIB_INDIRECT_DISP), IDX (INSN_LDIB_INDIRECT_DISP), FULL (ldib_indirect_disp) FAST (ldib_indirect_disp) },
  { TYPE (INSN_LDIB_INDEX_DISP), IDX (INSN_LDIB_INDEX_DISP), FULL (ldib_index_disp) FAST (ldib_index_disp) },
  { TYPE (INSN_LDIB_INDIRECT_INDEX_DISP), IDX (INSN_LDIB_INDIRECT_INDEX_DISP), FULL (ldib_indirect_index_disp) FAST (ldib_indirect_index_disp) },
  { TYPE (INSN_LDIS_OFFSET), IDX (INSN_LDIS_OFFSET), FULL (ldis_offset) FAST (ldis_offset) },
  { TYPE (INSN_LDIS_INDIRECT_OFFSET), IDX (INSN_LDIS_INDIRECT_OFFSET), FULL (ldis_indirect_offset) FAST (ldis_indirect_offset) },
  { TYPE (INSN_LDIS_INDIRECT), IDX (INSN_LDIS_INDIRECT), FULL (ldis_indirect) FAST (ldis_indirect) },
  { TYPE (INSN_LDIS_INDIRECT_INDEX), IDX (INSN_LDIS_INDIRECT_INDEX), FULL (ldis_indirect_index) FAST (ldis_indirect_index) },
  { TYPE (INSN_LDIS_DISP), IDX (INSN_LDIS_DISP), FULL (ldis_disp) FAST (ldis_disp) },
  { TYPE (INSN_LDIS_INDIRECT_DISP), IDX (INSN_LDIS_INDIRECT_DISP), FULL (ldis_indirect_disp) FAST (ldis_indirect_disp) },
  { TYPE (INSN_LDIS_INDEX_DISP), IDX (INSN_LDIS_INDEX_DISP), FULL (ldis_index_disp) FAST (ldis_index_disp) },
  { TYPE (INSN_LDIS_INDIRECT_INDEX_DISP), IDX (INSN_LDIS_INDIRECT_INDEX_DISP), FULL (ldis_indirect_index_disp) FAST (ldis_indirect_index_disp) },
  { TYPE (INSN_LDL_OFFSET), IDX (INSN_LDL_OFFSET), FULL (ldl_offset) FAST (ldl_offset) },
  { TYPE (INSN_LDL_INDIRECT_OFFSET), IDX (INSN_LDL_INDIRECT_OFFSET), FULL (ldl_indirect_offset) FAST (ldl_indirect_offset) },
  { TYPE (INSN_LDL_INDIRECT), IDX (INSN_LDL_INDIRECT), FULL (ldl_indirect) FAST (ldl_indirect) },
  { TYPE (INSN_LDL_INDIRECT_INDEX), IDX (INSN_LDL_INDIRECT_INDEX), FULL (ldl_indirect_index) FAST (ldl_indirect_index) },
  { TYPE (INSN_LDL_DISP), IDX (INSN_LDL_DISP), FULL (ldl_disp) FAST (ldl_disp) },
  { TYPE (INSN_LDL_INDIRECT_DISP), IDX (INSN_LDL_INDIRECT_DISP), FULL (ldl_indirect_disp) FAST (ldl_indirect_disp) },
  { TYPE (INSN_LDL_INDEX_DISP), IDX (INSN_LDL_INDEX_DISP), FULL (ldl_index_disp) FAST (ldl_index_disp) },
  { TYPE (INSN_LDL_INDIRECT_INDEX_DISP), IDX (INSN_LDL_INDIRECT_INDEX_DISP), FULL (ldl_indirect_index_disp) FAST (ldl_indirect_index_disp) },
  { TYPE (INSN_LDT_OFFSET), IDX (INSN_LDT_OFFSET), FULL (ldt_offset) FAST (ldt_offset) },
  { TYPE (INSN_LDT_INDIRECT_OFFSET), IDX (INSN_LDT_INDIRECT_OFFSET), FULL (ldt_indirect_offset) FAST (ldt_indirect_offset) },
  { TYPE (INSN_LDT_INDIRECT), IDX (INSN_LDT_INDIRECT), FULL (ldt_indirect) FAST (ldt_indirect) },
  { TYPE (INSN_LDT_INDIRECT_INDEX), IDX (INSN_LDT_INDIRECT_INDEX), FULL (ldt_indirect_index) FAST (ldt_indirect_index) },
  { TYPE (INSN_LDT_DISP), IDX (INSN_LDT_DISP), FULL (ldt_disp) FAST (ldt_disp) },
  { TYPE (INSN_LDT_INDIRECT_DISP), IDX (INSN_LDT_INDIRECT_DISP), FULL (ldt_indirect_disp) FAST (ldt_indirect_disp) },
  { TYPE (INSN_LDT_INDEX_DISP), IDX (INSN_LDT_INDEX_DISP), FULL (ldt_index_disp) FAST (ldt_index_disp) },
  { TYPE (INSN_LDT_INDIRECT_INDEX_DISP), IDX (INSN_LDT_INDIRECT_INDEX_DISP), FULL (ldt_indirect_index_disp) FAST (ldt_indirect_index_disp) },
  { TYPE (INSN_LDQ_OFFSET), IDX (INSN_LDQ_OFFSET), FULL (ldq_offset) FAST (ldq_offset) },
  { TYPE (INSN_LDQ_INDIRECT_OFFSET), IDX (INSN_LDQ_INDIRECT_OFFSET), FULL (ldq_indirect_offset) FAST (ldq_indirect_offset) },
  { TYPE (INSN_LDQ_INDIRECT), IDX (INSN_LDQ_INDIRECT), FULL (ldq_indirect) FAST (ldq_indirect) },
  { TYPE (INSN_LDQ_INDIRECT_INDEX), IDX (INSN_LDQ_INDIRECT_INDEX), FULL (ldq_indirect_index) FAST (ldq_indirect_index) },
  { TYPE (INSN_LDQ_DISP), IDX (INSN_LDQ_DISP), FULL (ldq_disp) FAST (ldq_disp) },
  { TYPE (INSN_LDQ_INDIRECT_DISP), IDX (INSN_LDQ_INDIRECT_DISP), FULL (ldq_indirect_disp) FAST (ldq_indirect_disp) },
  { TYPE (INSN_LDQ_INDEX_DISP), IDX (INSN_LDQ_INDEX_DISP), FULL (ldq_index_disp) FAST (ldq_index_disp) },
  { TYPE (INSN_LDQ_INDIRECT_INDEX_DISP), IDX (INSN_LDQ_INDIRECT_INDEX_DISP), FULL (ldq_indirect_index_disp) FAST (ldq_indirect_index_disp) },
  { TYPE (INSN_ST_OFFSET), IDX (INSN_ST_OFFSET), FULL (st_offset) FAST (st_offset) },
  { TYPE (INSN_ST_INDIRECT_OFFSET), IDX (INSN_ST_INDIRECT_OFFSET), FULL (st_indirect_offset) FAST (st_indirect_offset) },
  { TYPE (INSN_ST_INDIRECT), IDX (INSN_ST_INDIRECT), FULL (st_indirect) FAST (st_indirect) },
  { TYPE (INSN_ST_INDIRECT_INDEX), IDX (INSN_ST_INDIRECT_INDEX), FULL (st_indirect_index) FAST (st_indirect_index) },
  { TYPE (INSN_ST_DISP), IDX (INSN_ST_DISP), FULL (st_disp) FAST (st_disp) },
  { TYPE (INSN_ST_INDIRECT_DISP), IDX (INSN_ST_INDIRECT_DISP), FULL (st_indirect_disp) FAST (st_indirect_disp) },
  { TYPE (INSN_ST_INDEX_DISP), IDX (INSN_ST_INDEX_DISP), FULL (st_index_disp) FAST (st_index_disp) },
  { TYPE (INSN_ST_INDIRECT_INDEX_DISP), IDX (INSN_ST_INDIRECT_INDEX_DISP), FULL (st_indirect_index_disp) FAST (st_indirect_index_disp) },
  { TYPE (INSN_STOB_OFFSET), IDX (INSN_STOB_OFFSET), FULL (stob_offset) FAST (stob_offset) },
  { TYPE (INSN_STOB_INDIRECT_OFFSET), IDX (INSN_STOB_INDIRECT_OFFSET), FULL (stob_indirect_offset) FAST (stob_indirect_offset) },
  { TYPE (INSN_STOB_INDIRECT), IDX (INSN_STOB_INDIRECT), FULL (stob_indirect) FAST (stob_indirect) },
  { TYPE (INSN_STOB_INDIRECT_INDEX), IDX (INSN_STOB_INDIRECT_INDEX), FULL (stob_indirect_index) FAST (stob_indirect_index) },
  { TYPE (INSN_STOB_DISP), IDX (INSN_STOB_DISP), FULL (stob_disp) FAST (stob_disp) },
  { TYPE (INSN_STOB_INDIRECT_DISP), IDX (INSN_STOB_INDIRECT_DISP), FULL (stob_indirect_disp) FAST (stob_indirect_disp) },
  { TYPE (INSN_STOB_INDEX_DISP), IDX (INSN_STOB_INDEX_DISP), FULL (stob_index_disp) FAST (stob_index_disp) },
  { TYPE (INSN_STOB_INDIRECT_INDEX_DISP), IDX (INSN_STOB_INDIRECT_INDEX_DISP), FULL (stob_indirect_index_disp) FAST (stob_indirect_index_disp) },
  { TYPE (INSN_STOS_OFFSET), IDX (INSN_STOS_OFFSET), FULL (stos_offset) FAST (stos_offset) },
  { TYPE (INSN_STOS_INDIRECT_OFFSET), IDX (INSN_STOS_INDIRECT_OFFSET), FULL (stos_indirect_offset) FAST (stos_indirect_offset) },
  { TYPE (INSN_STOS_INDIRECT), IDX (INSN_STOS_INDIRECT), FULL (stos_indirect) FAST (stos_indirect) },
  { TYPE (INSN_STOS_INDIRECT_INDEX), IDX (INSN_STOS_INDIRECT_INDEX), FULL (stos_indirect_index) FAST (stos_indirect_index) },
  { TYPE (INSN_STOS_DISP), IDX (INSN_STOS_DISP), FULL (stos_disp) FAST (stos_disp) },
  { TYPE (INSN_STOS_INDIRECT_DISP), IDX (INSN_STOS_INDIRECT_DISP), FULL (stos_indirect_disp) FAST (stos_indirect_disp) },
  { TYPE (INSN_STOS_INDEX_DISP), IDX (INSN_STOS_INDEX_DISP), FULL (stos_index_disp) FAST (stos_index_disp) },
  { TYPE (INSN_STOS_INDIRECT_INDEX_DISP), IDX (INSN_STOS_INDIRECT_INDEX_DISP), FULL (stos_indirect_index_disp) FAST (stos_indirect_index_disp) },
  { TYPE (INSN_STL_OFFSET), IDX (INSN_STL_OFFSET), FULL (stl_offset) FAST (stl_offset) },
  { TYPE (INSN_STL_INDIRECT_OFFSET), IDX (INSN_STL_INDIRECT_OFFSET), FULL (stl_indirect_offset) FAST (stl_indirect_offset) },
  { TYPE (INSN_STL_INDIRECT), IDX (INSN_STL_INDIRECT), FULL (stl_indirect) FAST (stl_indirect) },
  { TYPE (INSN_STL_INDIRECT_INDEX), IDX (INSN_STL_INDIRECT_INDEX), FULL (stl_indirect_index) FAST (stl_indirect_index) },
  { TYPE (INSN_STL_DISP), IDX (INSN_STL_DISP), FULL (stl_disp) FAST (stl_disp) },
  { TYPE (INSN_STL_INDIRECT_DISP), IDX (INSN_STL_INDIRECT_DISP), FULL (stl_indirect_disp) FAST (stl_indirect_disp) },
  { TYPE (INSN_STL_INDEX_DISP), IDX (INSN_STL_INDEX_DISP), FULL (stl_index_disp) FAST (stl_index_disp) },
  { TYPE (INSN_STL_INDIRECT_INDEX_DISP), IDX (INSN_STL_INDIRECT_INDEX_DISP), FULL (stl_indirect_index_disp) FAST (stl_indirect_index_disp) },
  { TYPE (INSN_STT_OFFSET), IDX (INSN_STT_OFFSET), FULL (stt_offset) FAST (stt_offset) },
  { TYPE (INSN_STT_INDIRECT_OFFSET), IDX (INSN_STT_INDIRECT_OFFSET), FULL (stt_indirect_offset) FAST (stt_indirect_offset) },
  { TYPE (INSN_STT_INDIRECT), IDX (INSN_STT_INDIRECT), FULL (stt_indirect) FAST (stt_indirect) },
  { TYPE (INSN_STT_INDIRECT_INDEX), IDX (INSN_STT_INDIRECT_INDEX), FULL (stt_indirect_index) FAST (stt_indirect_index) },
  { TYPE (INSN_STT_DISP), IDX (INSN_STT_DISP), FULL (stt_disp) FAST (stt_disp) },
  { TYPE (INSN_STT_INDIRECT_DISP), IDX (INSN_STT_INDIRECT_DISP), FULL (stt_indirect_disp) FAST (stt_indirect_disp) },
  { TYPE (INSN_STT_INDEX_DISP), IDX (INSN_STT_INDEX_DISP), FULL (stt_index_disp) FAST (stt_index_disp) },
  { TYPE (INSN_STT_INDIRECT_INDEX_DISP), IDX (INSN_STT_INDIRECT_INDEX_DISP), FULL (stt_indirect_index_disp) FAST (stt_indirect_index_disp) },
  { TYPE (INSN_STQ_OFFSET), IDX (INSN_STQ_OFFSET), FULL (stq_offset) FAST (stq_offset) },
  { TYPE (INSN_STQ_INDIRECT_OFFSET), IDX (INSN_STQ_INDIRECT_OFFSET), FULL (stq_indirect_offset) FAST (stq_indirect_offset) },
  { TYPE (INSN_STQ_INDIRECT), IDX (INSN_STQ_INDIRECT), FULL (stq_indirect) FAST (stq_indirect) },
  { TYPE (INSN_STQ_INDIRECT_INDEX), IDX (INSN_STQ_INDIRECT_INDEX), FULL (stq_indirect_index) FAST (stq_indirect_index) },
  { TYPE (INSN_STQ_DISP), IDX (INSN_STQ_DISP), FULL (stq_disp) FAST (stq_disp) },
  { TYPE (INSN_STQ_INDIRECT_DISP), IDX (INSN_STQ_INDIRECT_DISP), FULL (stq_indirect_disp) FAST (stq_indirect_disp) },
  { TYPE (INSN_STQ_INDEX_DISP), IDX (INSN_STQ_INDEX_DISP), FULL (stq_index_disp) FAST (stq_index_disp) },
  { TYPE (INSN_STQ_INDIRECT_INDEX_DISP), IDX (INSN_STQ_INDIRECT_INDEX_DISP), FULL (stq_indirect_index_disp) FAST (stq_indirect_index_disp) },
  { TYPE (INSN_CMPOBE_REG), IDX (INSN_CMPOBE_REG), FULL (cmpobe_reg) FAST (cmpobe_reg) },
  { TYPE (INSN_CMPOBE_LIT), IDX (INSN_CMPOBE_LIT), FULL (cmpobe_lit) FAST (cmpobe_lit) },
  { TYPE (INSN_CMPOBNE_REG), IDX (INSN_CMPOBNE_REG), FULL (cmpobne_reg) FAST (cmpobne_reg) },
  { TYPE (INSN_CMPOBNE_LIT), IDX (INSN_CMPOBNE_LIT), FULL (cmpobne_lit) FAST (cmpobne_lit) },
  { TYPE (INSN_CMPOBL_REG), IDX (INSN_CMPOBL_REG), FULL (cmpobl_reg) FAST (cmpobl_reg) },
  { TYPE (INSN_CMPOBL_LIT), IDX (INSN_CMPOBL_LIT), FULL (cmpobl_lit) FAST (cmpobl_lit) },
  { TYPE (INSN_CMPOBLE_REG), IDX (INSN_CMPOBLE_REG), FULL (cmpoble_reg) FAST (cmpoble_reg) },
  { TYPE (INSN_CMPOBLE_LIT), IDX (INSN_CMPOBLE_LIT), FULL (cmpoble_lit) FAST (cmpoble_lit) },
  { TYPE (INSN_CMPOBG_REG), IDX (INSN_CMPOBG_REG), FULL (cmpobg_reg) FAST (cmpobg_reg) },
  { TYPE (INSN_CMPOBG_LIT), IDX (INSN_CMPOBG_LIT), FULL (cmpobg_lit) FAST (cmpobg_lit) },
  { TYPE (INSN_CMPOBGE_REG), IDX (INSN_CMPOBGE_REG), FULL (cmpobge_reg) FAST (cmpobge_reg) },
  { TYPE (INSN_CMPOBGE_LIT), IDX (INSN_CMPOBGE_LIT), FULL (cmpobge_lit) FAST (cmpobge_lit) },
  { TYPE (INSN_CMPIBE_REG), IDX (INSN_CMPIBE_REG), FULL (cmpibe_reg) FAST (cmpibe_reg) },
  { TYPE (INSN_CMPIBE_LIT), IDX (INSN_CMPIBE_LIT), FULL (cmpibe_lit) FAST (cmpibe_lit) },
  { TYPE (INSN_CMPIBNE_REG), IDX (INSN_CMPIBNE_REG), FULL (cmpibne_reg) FAST (cmpibne_reg) },
  { TYPE (INSN_CMPIBNE_LIT), IDX (INSN_CMPIBNE_LIT), FULL (cmpibne_lit) FAST (cmpibne_lit) },
  { TYPE (INSN_CMPIBL_REG), IDX (INSN_CMPIBL_REG), FULL (cmpibl_reg) FAST (cmpibl_reg) },
  { TYPE (INSN_CMPIBL_LIT), IDX (INSN_CMPIBL_LIT), FULL (cmpibl_lit) FAST (cmpibl_lit) },
  { TYPE (INSN_CMPIBLE_REG), IDX (INSN_CMPIBLE_REG), FULL (cmpible_reg) FAST (cmpible_reg) },
  { TYPE (INSN_CMPIBLE_LIT), IDX (INSN_CMPIBLE_LIT), FULL (cmpible_lit) FAST (cmpible_lit) },
  { TYPE (INSN_CMPIBG_REG), IDX (INSN_CMPIBG_REG), FULL (cmpibg_reg) FAST (cmpibg_reg) },
  { TYPE (INSN_CMPIBG_LIT), IDX (INSN_CMPIBG_LIT), FULL (cmpibg_lit) FAST (cmpibg_lit) },
  { TYPE (INSN_CMPIBGE_REG), IDX (INSN_CMPIBGE_REG), FULL (cmpibge_reg) FAST (cmpibge_reg) },
  { TYPE (INSN_CMPIBGE_LIT), IDX (INSN_CMPIBGE_LIT), FULL (cmpibge_lit) FAST (cmpibge_lit) },
  { TYPE (INSN_BBC_REG), IDX (INSN_BBC_REG), FULL (bbc_reg) FAST (bbc_reg) },
  { TYPE (INSN_BBC_LIT), IDX (INSN_BBC_LIT), FULL (bbc_lit) FAST (bbc_lit) },
  { TYPE (INSN_BBS_REG), IDX (INSN_BBS_REG), FULL (bbs_reg) FAST (bbs_reg) },
  { TYPE (INSN_BBS_LIT), IDX (INSN_BBS_LIT), FULL (bbs_lit) FAST (bbs_lit) },
  { TYPE (INSN_CMPI), IDX (INSN_CMPI), FULL (cmpi) FAST (cmpi) },
  { TYPE (INSN_CMPI1), IDX (INSN_CMPI1), FULL (cmpi1) FAST (cmpi1) },
  { TYPE (INSN_CMPI2), IDX (INSN_CMPI2), FULL (cmpi2) FAST (cmpi2) },
  { TYPE (INSN_CMPI3), IDX (INSN_CMPI3), FULL (cmpi3) FAST (cmpi3) },
  { TYPE (INSN_CMPO), IDX (INSN_CMPO), FULL (cmpo) FAST (cmpo) },
  { TYPE (INSN_CMPO1), IDX (INSN_CMPO1), FULL (cmpo1) FAST (cmpo1) },
  { TYPE (INSN_CMPO2), IDX (INSN_CMPO2), FULL (cmpo2) FAST (cmpo2) },
  { TYPE (INSN_CMPO3), IDX (INSN_CMPO3), FULL (cmpo3) FAST (cmpo3) },
  { TYPE (INSN_TESTNO_REG), IDX (INSN_TESTNO_REG), FULL (testno_reg) FAST (testno_reg) },
  { TYPE (INSN_TESTG_REG), IDX (INSN_TESTG_REG), FULL (testg_reg) FAST (testg_reg) },
  { TYPE (INSN_TESTE_REG), IDX (INSN_TESTE_REG), FULL (teste_reg) FAST (teste_reg) },
  { TYPE (INSN_TESTGE_REG), IDX (INSN_TESTGE_REG), FULL (testge_reg) FAST (testge_reg) },
  { TYPE (INSN_TESTL_REG), IDX (INSN_TESTL_REG), FULL (testl_reg) FAST (testl_reg) },
  { TYPE (INSN_TESTNE_REG), IDX (INSN_TESTNE_REG), FULL (testne_reg) FAST (testne_reg) },
  { TYPE (INSN_TESTLE_REG), IDX (INSN_TESTLE_REG), FULL (testle_reg) FAST (testle_reg) },
  { TYPE (INSN_TESTO_REG), IDX (INSN_TESTO_REG), FULL (testo_reg) FAST (testo_reg) },
  { TYPE (INSN_BNO), IDX (INSN_BNO), FULL (bno) FAST (bno) },
  { TYPE (INSN_BG), IDX (INSN_BG), FULL (bg) FAST (bg) },
  { TYPE (INSN_BE), IDX (INSN_BE), FULL (be) FAST (be) },
  { TYPE (INSN_BGE), IDX (INSN_BGE), FULL (bge) FAST (bge) },
  { TYPE (INSN_BL), IDX (INSN_BL), FULL (bl) FAST (bl) },
  { TYPE (INSN_BNE), IDX (INSN_BNE), FULL (bne) FAST (bne) },
  { TYPE (INSN_BLE), IDX (INSN_BLE), FULL (ble) FAST (ble) },
  { TYPE (INSN_BO), IDX (INSN_BO), FULL (bo) FAST (bo) },
  { TYPE (INSN_B), IDX (INSN_B), FULL (b) FAST (b) },
  { TYPE (INSN_BX_INDIRECT_OFFSET), IDX (INSN_BX_INDIRECT_OFFSET), FULL (bx_indirect_offset) FAST (bx_indirect_offset) },
  { TYPE (INSN_BX_INDIRECT), IDX (INSN_BX_INDIRECT), FULL (bx_indirect) FAST (bx_indirect) },
  { TYPE (INSN_BX_INDIRECT_INDEX), IDX (INSN_BX_INDIRECT_INDEX), FULL (bx_indirect_index) FAST (bx_indirect_index) },
  { TYPE (INSN_BX_DISP), IDX (INSN_BX_DISP), FULL (bx_disp) FAST (bx_disp) },
  { TYPE (INSN_BX_INDIRECT_DISP), IDX (INSN_BX_INDIRECT_DISP), FULL (bx_indirect_disp) FAST (bx_indirect_disp) },
  { TYPE (INSN_CALLX_DISP), IDX (INSN_CALLX_DISP), FULL (callx_disp) FAST (callx_disp) },
  { TYPE (INSN_CALLX_INDIRECT), IDX (INSN_CALLX_INDIRECT), FULL (callx_indirect) FAST (callx_indirect) },
  { TYPE (INSN_CALLX_INDIRECT_OFFSET), IDX (INSN_CALLX_INDIRECT_OFFSET), FULL (callx_indirect_offset) FAST (callx_indirect_offset) },
  { TYPE (INSN_RET), IDX (INSN_RET), FULL (ret) FAST (ret) },
  { TYPE (INSN_CALLS), IDX (INSN_CALLS), FULL (calls) FAST (calls) },
  { TYPE (INSN_FMARK), IDX (INSN_FMARK), FULL (fmark) FAST (fmark) },
  { TYPE (INSN_FLUSHREG), IDX (INSN_FLUSHREG), FULL (flushreg) FAST (flushreg) },
};

static const struct insn_sem i960base_insn_sem_invalid =
{
  VIRTUAL_INSN_X_INVALID, IDX (INSN_X_INVALID), FULL (x_invalid) FAST (x_invalid)
};

#undef IDX
#undef TYPE

/* Initialize an IDESC from the compile-time computable parts.  */

static INLINE void
init_idesc (SIM_CPU *cpu, IDESC *id, const struct insn_sem *t)
{
  const CGEN_INSN *insn_table = CGEN_CPU_INSN_TABLE (CPU_CPU_DESC (cpu))->init_entries;

  id->num = t->index;
  if ((int) t->type <= 0)
    id->idata = & cgen_virtual_insn_table[- (int) t->type];
  else
    id->idata = & insn_table[t->type];
  id->attrs = CGEN_INSN_ATTRS (id->idata);
  /* Oh my god, a magic number.  */
  id->length = CGEN_INSN_BITSIZE (id->idata) / 8;
#if ! WITH_SEM_SWITCH_FULL
  id->sem_full = t->sem_full;
#endif
#if WITH_FAST && ! WITH_SEM_SWITCH_FAST
  id->sem_fast = t->sem_fast;
#endif
#if WITH_PROFILE_MODEL_P
  id->timing = & MODEL_TIMING (CPU_MODEL (cpu)) [t->index];
  {
    SIM_DESC sd = CPU_STATE (cpu);
    SIM_ASSERT (t->index == id->timing->num);
  }
#endif
}

/* Initialize the instruction descriptor table.  */

void
i960base_init_idesc_table (SIM_CPU *cpu)
{
  IDESC *id,*tabend;
  const struct insn_sem *t,*tend;
  int tabsize = I960BASE_INSN_MAX;
  IDESC *table = i960base_insn_data;

  memset (table, 0, tabsize * sizeof (IDESC));

  /* First set all entries to the `invalid insn'.  */
  t = & i960base_insn_sem_invalid;
  for (id = table, tabend = table + tabsize; id < tabend; ++id)
    init_idesc (cpu, id, t);

  /* Now fill in the values for the chosen cpu.  */
  for (t = i960base_insn_sem, tend = t + sizeof (i960base_insn_sem) / sizeof (*t);
       t != tend; ++t)
    {
      init_idesc (cpu, & table[t->index], t);
    }

  /* Link the IDESC table into the cpu.  */
  CPU_IDESC (cpu) = table;
}

/* Enum declaration for all instruction semantic formats.  */
typedef enum sfmt {
  FMT_EMPTY, FMT_MULO, FMT_MULO1, FMT_MULO2
 , FMT_MULO3, FMT_NOTBIT, FMT_NOTBIT1, FMT_NOTBIT2
 , FMT_NOTBIT3, FMT_NOT, FMT_NOT1, FMT_NOT2
 , FMT_NOT3, FMT_EMUL, FMT_EMUL1, FMT_EMUL2
 , FMT_EMUL3, FMT_MOVL, FMT_MOVL1, FMT_MOVT
 , FMT_MOVT1, FMT_MOVQ, FMT_MOVQ1, FMT_MODPC
 , FMT_LDA_OFFSET, FMT_LDA_INDIRECT_OFFSET, FMT_LDA_INDIRECT, FMT_LDA_INDIRECT_INDEX
 , FMT_LDA_DISP, FMT_LDA_INDIRECT_DISP, FMT_LDA_INDEX_DISP, FMT_LDA_INDIRECT_INDEX_DISP
 , FMT_LD_OFFSET, FMT_LD_INDIRECT_OFFSET, FMT_LD_INDIRECT, FMT_LD_INDIRECT_INDEX
 , FMT_LD_DISP, FMT_LD_INDIRECT_DISP, FMT_LD_INDEX_DISP, FMT_LD_INDIRECT_INDEX_DISP
 , FMT_LDOB_OFFSET, FMT_LDOB_INDIRECT_OFFSET, FMT_LDOB_INDIRECT, FMT_LDOB_INDIRECT_INDEX
 , FMT_LDOB_DISP, FMT_LDOB_INDIRECT_DISP, FMT_LDOB_INDEX_DISP, FMT_LDOB_INDIRECT_INDEX_DISP
 , FMT_LDOS_OFFSET, FMT_LDOS_INDIRECT_OFFSET, FMT_LDOS_INDIRECT, FMT_LDOS_INDIRECT_INDEX
 , FMT_LDOS_DISP, FMT_LDOS_INDIRECT_DISP, FMT_LDOS_INDEX_DISP, FMT_LDOS_INDIRECT_INDEX_DISP
 , FMT_LDIB_OFFSET, FMT_LDIB_INDIRECT_OFFSET, FMT_LDIB_INDIRECT, FMT_LDIB_INDIRECT_INDEX
 , FMT_LDIB_DISP, FMT_LDIB_INDIRECT_DISP, FMT_LDIB_INDEX_DISP, FMT_LDIB_INDIRECT_INDEX_DISP
 , FMT_LDIS_OFFSET, FMT_LDIS_INDIRECT_OFFSET, FMT_LDIS_INDIRECT, FMT_LDIS_INDIRECT_INDEX
 , FMT_LDIS_DISP, FMT_LDIS_INDIRECT_DISP, FMT_LDIS_INDEX_DISP, FMT_LDIS_INDIRECT_INDEX_DISP
 , FMT_LDL_OFFSET, FMT_LDL_INDIRECT_OFFSET, FMT_LDL_INDIRECT, FMT_LDL_INDIRECT_INDEX
 , FMT_LDL_DISP, FMT_LDL_INDIRECT_DISP, FMT_LDL_INDEX_DISP, FMT_LDL_INDIRECT_INDEX_DISP
 , FMT_LDT_OFFSET, FMT_LDT_INDIRECT_OFFSET, FMT_LDT_INDIRECT, FMT_LDT_INDIRECT_INDEX
 , FMT_LDT_DISP, FMT_LDT_INDIRECT_DISP, FMT_LDT_INDEX_DISP, FMT_LDT_INDIRECT_INDEX_DISP
 , FMT_LDQ_OFFSET, FMT_LDQ_INDIRECT_OFFSET, FMT_LDQ_INDIRECT, FMT_LDQ_INDIRECT_INDEX
 , FMT_LDQ_DISP, FMT_LDQ_INDIRECT_DISP, FMT_LDQ_INDEX_DISP, FMT_LDQ_INDIRECT_INDEX_DISP
 , FMT_ST_OFFSET, FMT_ST_INDIRECT_OFFSET, FMT_ST_INDIRECT, FMT_ST_INDIRECT_INDEX
 , FMT_ST_DISP, FMT_ST_INDIRECT_DISP, FMT_ST_INDEX_DISP, FMT_ST_INDIRECT_INDEX_DISP
 , FMT_STOB_OFFSET, FMT_STOB_INDIRECT_OFFSET, FMT_STOB_INDIRECT, FMT_STOB_INDIRECT_INDEX
 , FMT_STOB_DISP, FMT_STOB_INDIRECT_DISP, FMT_STOB_INDEX_DISP, FMT_STOB_INDIRECT_INDEX_DISP
 , FMT_STOS_OFFSET, FMT_STOS_INDIRECT_OFFSET, FMT_STOS_INDIRECT, FMT_STOS_INDIRECT_INDEX
 , FMT_STOS_DISP, FMT_STOS_INDIRECT_DISP, FMT_STOS_INDEX_DISP, FMT_STOS_INDIRECT_INDEX_DISP
 , FMT_STL_OFFSET, FMT_STL_INDIRECT_OFFSET, FMT_STL_INDIRECT, FMT_STL_INDIRECT_INDEX
 , FMT_STL_DISP, FMT_STL_INDIRECT_DISP, FMT_STL_INDEX_DISP, FMT_STL_INDIRECT_INDEX_DISP
 , FMT_STT_OFFSET, FMT_STT_INDIRECT_OFFSET, FMT_STT_INDIRECT, FMT_STT_INDIRECT_INDEX
 , FMT_STT_DISP, FMT_STT_INDIRECT_DISP, FMT_STT_INDEX_DISP, FMT_STT_INDIRECT_INDEX_DISP
 , FMT_STQ_OFFSET, FMT_STQ_INDIRECT_OFFSET, FMT_STQ_INDIRECT, FMT_STQ_INDIRECT_INDEX
 , FMT_STQ_DISP, FMT_STQ_INDIRECT_DISP, FMT_STQ_INDEX_DISP, FMT_STQ_INDIRECT_INDEX_DISP
 , FMT_CMPOBE_REG, FMT_CMPOBE_LIT, FMT_CMPOBL_REG, FMT_CMPOBL_LIT
 , FMT_BBC_REG, FMT_BBC_LIT, FMT_CMPI, FMT_CMPI1
 , FMT_CMPI2, FMT_CMPI3, FMT_CMPO, FMT_CMPO1
 , FMT_CMPO2, FMT_CMPO3, FMT_TESTNO_REG, FMT_BNO
 , FMT_B, FMT_BX_INDIRECT_OFFSET, FMT_BX_INDIRECT, FMT_BX_INDIRECT_INDEX
 , FMT_BX_DISP, FMT_BX_INDIRECT_DISP, FMT_CALLX_DISP, FMT_CALLX_INDIRECT
 , FMT_CALLX_INDIRECT_OFFSET, FMT_RET, FMT_CALLS, FMT_FMARK
 , FMT_FLUSHREG
} SFMT;

/* The decoder uses this to record insns and direct extraction handling.  */

typedef struct {
  const IDESC *idesc;
#ifdef __GNUC__
  void *sfmt;
#else
  enum sfmt sfmt;
#endif
} DECODE_DESC;

/* Macro to go from decode phase to extraction phase.  */

#ifdef __GNUC__
#define GOTO_EXTRACT(id) goto *(id)->sfmt
#else
#define GOTO_EXTRACT(id) goto extract
#endif

/* The decoder needs a slightly different computed goto switch control.  */
#ifdef __GNUC__
#define DECODE_SWITCH(N, X) goto *labels_##N[X];
#else
#define DECODE_SWITCH(N, X) switch (X)
#endif

/* Given an instruction, return a pointer to its IDESC entry.  */

const IDESC *
i960base_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn,
              ARGBUF *abuf)
{
  /* Result of decoder, used by extractor.  */
  const DECODE_DESC *idecode;

  /* First decode the instruction.  */

  {
#define I(insn) & i960base_insn_data[CONCAT2 (I960BASE_,insn)]
#ifdef __GNUC__
#define E(fmt) && case_ex_##fmt
#else
#define E(fmt) fmt
#endif
    CGEN_INSN_INT insn = base_insn;
    static const DECODE_DESC idecode_invalid = { I (INSN_X_INVALID), E (FMT_EMPTY) };

    {
#ifdef __GNUC__
      static const void *labels_0[256] = {
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_48, && case_0_49, && case_0_50, && case_0_51, 
        && case_0_52, && case_0_53, && case_0_54, && case_0_55, 
        && default_0, && case_0_57, && case_0_58, && case_0_59, 
        && case_0_60, && case_0_61, && case_0_62, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_88, && case_0_89, && case_0_90, && default_0, 
        && case_0_92, && case_0_93, && case_0_94, && case_0_95, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && case_0_102, && case_0_103, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_112, && default_0, && default_0, && default_0, 
        && case_0_116, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_128, && default_0, && case_0_130, && default_0, 
        && case_0_132, && default_0, && case_0_134, && default_0, 
        && case_0_136, && default_0, && case_0_138, && default_0, 
        && case_0_140, && default_0, && default_0, && default_0, 
        && case_0_144, && default_0, && case_0_146, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_152, && default_0, && case_0_154, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_160, && default_0, && case_0_162, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_176, && default_0, && case_0_178, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_192, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && case_0_200, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
        && default_0, && default_0, && default_0, && default_0, 
      };
#endif
      static const DECODE_DESC insns[256] = {
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_B), E (FMT_B) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_RET), E (FMT_RET) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_BNO), E (FMT_BNO) }, { I (INSN_BG), E (FMT_BNO) }, 
        { I (INSN_BE), E (FMT_BNO) }, { I (INSN_BGE), E (FMT_BNO) }, 
        { I (INSN_BL), E (FMT_BNO) }, { I (INSN_BNE), E (FMT_BNO) }, 
        { I (INSN_BLE), E (FMT_BNO) }, { I (INSN_BO), E (FMT_BNO) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_TESTNO_REG), E (FMT_TESTNO_REG) }, { I (INSN_TESTG_REG), E (FMT_TESTNO_REG) }, 
        { I (INSN_TESTE_REG), E (FMT_TESTNO_REG) }, { I (INSN_TESTGE_REG), E (FMT_TESTNO_REG) }, 
        { I (INSN_TESTL_REG), E (FMT_TESTNO_REG) }, { I (INSN_TESTNE_REG), E (FMT_TESTNO_REG) }, 
        { I (INSN_TESTLE_REG), E (FMT_TESTNO_REG) }, { I (INSN_TESTO_REG), E (FMT_TESTNO_REG) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { 0 }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { 0 }, 
        { 0 }, { 0 }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_MODAC), E (FMT_MODPC) }, { I (INSN_MODPC), E (FMT_MODPC) }, 
        { 0 }, { 0 }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
        { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
      };
      unsigned int val;
      val = (((insn >> 24) & (255 << 0)));
      DECODE_SWITCH (0, val)
        {
        CASE (0, 48) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_BBC_REG), E (FMT_BBC_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BBC_LIT), E (FMT_BBC_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 49) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPOBG_REG), E (FMT_CMPOBL_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPOBG_LIT), E (FMT_CMPOBL_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 50) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPOBE_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPOBE_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 51) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPOBGE_REG), E (FMT_CMPOBL_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPOBGE_LIT), E (FMT_CMPOBL_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 52) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPOBL_REG), E (FMT_CMPOBL_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPOBL_LIT), E (FMT_CMPOBL_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 53) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPOBNE_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPOBNE_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 54) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPOBLE_REG), E (FMT_CMPOBL_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPOBLE_LIT), E (FMT_CMPOBL_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 55) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_BBS_REG), E (FMT_BBC_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BBS_LIT), E (FMT_BBC_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 57) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPIBG_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPIBG_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 58) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPIBE_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPIBE_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 59) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPIBGE_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPIBGE_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 60) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPIBL_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPIBL_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 61) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPIBNE_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPIBNE_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 62) :
          {
            static const DECODE_DESC insns[8] = {
              { I (INSN_CMPIBLE_REG), E (FMT_CMPOBE_REG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CMPIBLE_LIT), E (FMT_CMPOBE_LIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 88) :
          {
#ifdef __GNUC__
            static const void *labels_0_88[16] = {
              && case_0_88_0, && case_0_88_1, && case_0_88_2, && case_0_88_3, 
              && case_0_88_4, && case_0_88_5, && case_0_88_6, && case_0_88_7, 
              && default_0_88, && default_0_88, && default_0_88, && default_0_88, 
              && default_0_88, && default_0_88, && default_0_88, && default_0_88, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { 0 }, { 0 }, 
              { 0 }, { 0 }, 
              { 0 }, { 0 }, 
              { 0 }, { 0 }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val;
            val = (((insn >> 10) & (15 << 0)));
            DECODE_SWITCH (0_88, val)
              {
              CASE (0_88, 0) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOTBIT), E (FMT_NOTBIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_AND), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_ANDNOT), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SETBIT), E (FMT_NOTBIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOTAND), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_XOR), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_OR), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_88, 1) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOR), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOT), E (FMT_NOT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CLRBIT), E (FMT_NOTBIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_88, 2) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOTBIT1), E (FMT_NOTBIT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_AND1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_ANDNOT1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SETBIT1), E (FMT_NOTBIT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOTAND1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_XOR1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_OR1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_88, 3) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOR1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOT1), E (FMT_NOT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CLRBIT1), E (FMT_NOTBIT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_88, 4) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOTBIT2), E (FMT_NOTBIT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_AND2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_ANDNOT2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SETBIT2), E (FMT_NOTBIT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOTAND2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_XOR2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_OR2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_88, 5) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOR2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOT2), E (FMT_NOT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CLRBIT2), E (FMT_NOTBIT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_88, 6) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOTBIT3), E (FMT_NOTBIT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_AND3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_ANDNOT3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SETBIT3), E (FMT_NOTBIT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOTAND3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_XOR3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_OR3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_88, 7) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_NOR3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_NOT3), E (FMT_NOT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CLRBIT3), E (FMT_NOTBIT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              DEFAULT (0_88) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_88)
          }
        CASE (0, 89) :
          {
#ifdef __GNUC__
            static const void *labels_0_89[16] = {
              && case_0_89_0, && case_0_89_1, && case_0_89_2, && case_0_89_3, 
              && case_0_89_4, && case_0_89_5, && case_0_89_6, && case_0_89_7, 
              && default_0_89, && default_0_89, && default_0_89, && default_0_89, 
              && default_0_89, && default_0_89, && default_0_89, && default_0_89, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { 0 }, { 0 }, 
              { 0 }, { 0 }, 
              { 0 }, { 0 }, 
              { 0 }, { 0 }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val;
            val = (((insn >> 10) & (15 << 0)));
            DECODE_SWITCH (0_89, val)
              {
              CASE (0_89, 0) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_ADDO), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SUBO), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_89, 1) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_SHRO), E (FMT_NOTBIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHRI), E (FMT_NOTBIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLO), E (FMT_NOTBIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLI), E (FMT_NOTBIT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_89, 2) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_ADDO1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SUBO1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_89, 3) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_SHRO1), E (FMT_NOTBIT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHRI1), E (FMT_NOTBIT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLO1), E (FMT_NOTBIT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLI1), E (FMT_NOTBIT1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_89, 4) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_ADDO2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SUBO2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_89, 5) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_SHRO2), E (FMT_NOTBIT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHRI2), E (FMT_NOTBIT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLO2), E (FMT_NOTBIT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLI2), E (FMT_NOTBIT2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_89, 6) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_ADDO3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SUBO3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_89, 7) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_SHRO3), E (FMT_NOTBIT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHRI3), E (FMT_NOTBIT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLO3), E (FMT_NOTBIT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_SHLI3), E (FMT_NOTBIT3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              DEFAULT (0_89) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_89)
          }
        CASE (0, 90) :
          {
#ifdef __GNUC__
            static const void *labels_0_90[16] = {
              && default_0_90, && default_0_90, && default_0_90, && default_0_90, 
              && default_0_90, && default_0_90, && default_0_90, && default_0_90, 
              && case_0_90_8, && default_0_90, && case_0_90_10, && default_0_90, 
              && case_0_90_12, && default_0_90, && case_0_90_14, && default_0_90, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { 0 }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val;
            val = (((insn >> 10) & (15 << 0)));
            DECODE_SWITCH (0_90, val)
              {
              CASE (0_90, 8) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_CMPO), E (FMT_CMPO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CMPI), E (FMT_CMPI) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_90, 10) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_CMPO1), E (FMT_CMPO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CMPI1), E (FMT_CMPI1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_90, 12) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_CMPO2), E (FMT_CMPO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CMPI2), E (FMT_CMPI2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_90, 14) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_CMPO3), E (FMT_CMPO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_CMPI3), E (FMT_CMPI3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              DEFAULT (0_90) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_90)
          }
        CASE (0, 92) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOV), E (FMT_NOT2) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOV1), E (FMT_NOT3) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 93) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOVL), E (FMT_MOVL) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOVL1), E (FMT_MOVL1) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 94) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOVT), E (FMT_MOVT) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOVT1), E (FMT_MOVT1) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 95) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOVQ), E (FMT_MOVQ) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_MOVQ1), E (FMT_MOVQ1) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 102) :
          {
#ifdef __GNUC__
            static const void *labels_0_102[16] = {
              && default_0_102, && default_0_102, && default_0_102, && default_0_102, 
              && default_0_102, && default_0_102, && default_0_102, && default_0_102, 
              && default_0_102, && default_0_102, && default_0_102, && default_0_102, 
              && default_0_102, && default_0_102, && default_0_102, && case_0_102_15, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CALLS), E (FMT_CALLS) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
            };
            unsigned int val;
            val = (((insn >> 10) & (15 << 0)));
            DECODE_SWITCH (0_102, val)
              {
              CASE (0_102, 15) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_FMARK), E (FMT_FMARK) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_FLUSHREG), E (FMT_FLUSHREG) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              DEFAULT (0_102) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_102)
          }
        CASE (0, 103) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_EMUL), E (FMT_EMUL) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_EMUL1), E (FMT_EMUL1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_EMUL2), E (FMT_EMUL2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_EMUL3), E (FMT_EMUL3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 112) :
          {
#ifdef __GNUC__
            static const void *labels_0_112[16] = {
              && default_0_112, && case_0_112_1, && default_0_112, && case_0_112_3, 
              && default_0_112, && case_0_112_5, && default_0_112, && case_0_112_7, 
              && default_0_112, && default_0_112, && default_0_112, && default_0_112, 
              && default_0_112, && default_0_112, && default_0_112, && default_0_112, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { I (INSN_MULO), E (FMT_MULO) }, { 0 }, 
              { I (INSN_MULO1), E (FMT_MULO1) }, { 0 }, 
              { I (INSN_MULO2), E (FMT_MULO2) }, { 0 }, 
              { I (INSN_MULO3), E (FMT_MULO3) }, { 0 }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val;
            val = (((insn >> 10) & (15 << 0)));
            DECODE_SWITCH (0_112, val)
              {
              CASE (0_112, 1) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMO), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVO), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_112, 3) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMO1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVO1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_112, 5) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMO2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVO2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_112, 7) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMO3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVO3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              DEFAULT (0_112) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_112)
          }
        CASE (0, 116) :
          {
#ifdef __GNUC__
            static const void *labels_0_116[16] = {
              && default_0_116, && case_0_116_1, && default_0_116, && case_0_116_3, 
              && default_0_116, && case_0_116_5, && default_0_116, && case_0_116_7, 
              && default_0_116, && default_0_116, && default_0_116, && default_0_116, 
              && default_0_116, && default_0_116, && default_0_116, && default_0_116, 
            };
#endif
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { 0 }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val;
            val = (((insn >> 10) & (15 << 0)));
            DECODE_SWITCH (0_116, val)
              {
              CASE (0_116, 1) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMI), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVI), E (FMT_MULO) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_116, 3) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMI1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVI1), E (FMT_MULO1) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_116, 5) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMI2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVI2), E (FMT_MULO2) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              CASE (0_116, 7) :
                {
                  static const DECODE_DESC insns[16] = {
                    { I (INSN_REMI3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_DIVI3), E (FMT_MULO3) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                    { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
                  };
                  unsigned int val = (((insn >> 6) & (15 << 0)));
                  idecode = &insns[val];
                  GOTO_EXTRACT (idecode);
                }
              DEFAULT (0_116) :
                idecode = &insns[val];
                GOTO_EXTRACT (idecode);
              }
            ENDSWITCH (0_116)
          }
        CASE (0, 128) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDOB_OFFSET), E (FMT_LDOB_OFFSET) }, { I (INSN_LDOB_OFFSET), E (FMT_LDOB_OFFSET) }, 
              { I (INSN_LDOB_OFFSET), E (FMT_LDOB_OFFSET) }, { I (INSN_LDOB_OFFSET), E (FMT_LDOB_OFFSET) }, 
              { I (INSN_LDOB_INDIRECT), E (FMT_LDOB_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDOB_INDIRECT_INDEX), E (FMT_LDOB_INDIRECT_INDEX) }, 
              { I (INSN_LDOB_INDIRECT_OFFSET), E (FMT_LDOB_INDIRECT_OFFSET) }, { I (INSN_LDOB_INDIRECT_OFFSET), E (FMT_LDOB_INDIRECT_OFFSET) }, 
              { I (INSN_LDOB_INDIRECT_OFFSET), E (FMT_LDOB_INDIRECT_OFFSET) }, { I (INSN_LDOB_INDIRECT_OFFSET), E (FMT_LDOB_INDIRECT_OFFSET) }, 
              { I (INSN_LDOB_DISP), E (FMT_LDOB_DISP) }, { I (INSN_LDOB_INDIRECT_DISP), E (FMT_LDOB_INDIRECT_DISP) }, 
              { I (INSN_LDOB_INDEX_DISP), E (FMT_LDOB_INDEX_DISP) }, { I (INSN_LDOB_INDIRECT_INDEX_DISP), E (FMT_LDOB_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 130) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_STOB_OFFSET), E (FMT_STOB_OFFSET) }, { I (INSN_STOB_OFFSET), E (FMT_STOB_OFFSET) }, 
              { I (INSN_STOB_OFFSET), E (FMT_STOB_OFFSET) }, { I (INSN_STOB_OFFSET), E (FMT_STOB_OFFSET) }, 
              { I (INSN_STOB_INDIRECT), E (FMT_STOB_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_STOB_INDIRECT_INDEX), E (FMT_STOB_INDIRECT_INDEX) }, 
              { I (INSN_STOB_INDIRECT_OFFSET), E (FMT_STOB_INDIRECT_OFFSET) }, { I (INSN_STOB_INDIRECT_OFFSET), E (FMT_STOB_INDIRECT_OFFSET) }, 
              { I (INSN_STOB_INDIRECT_OFFSET), E (FMT_STOB_INDIRECT_OFFSET) }, { I (INSN_STOB_INDIRECT_OFFSET), E (FMT_STOB_INDIRECT_OFFSET) }, 
              { I (INSN_STOB_DISP), E (FMT_STOB_DISP) }, { I (INSN_STOB_INDIRECT_DISP), E (FMT_STOB_INDIRECT_DISP) }, 
              { I (INSN_STOB_INDEX_DISP), E (FMT_STOB_INDEX_DISP) }, { I (INSN_STOB_INDIRECT_INDEX_DISP), E (FMT_STOB_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 132) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_BX_INDIRECT), E (FMT_BX_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_BX_INDIRECT_INDEX), E (FMT_BX_INDIRECT_INDEX) }, 
              { I (INSN_BX_INDIRECT_OFFSET), E (FMT_BX_INDIRECT_OFFSET) }, { I (INSN_BX_INDIRECT_OFFSET), E (FMT_BX_INDIRECT_OFFSET) }, 
              { I (INSN_BX_INDIRECT_OFFSET), E (FMT_BX_INDIRECT_OFFSET) }, { I (INSN_BX_INDIRECT_OFFSET), E (FMT_BX_INDIRECT_OFFSET) }, 
              { I (INSN_BX_DISP), E (FMT_BX_DISP) }, { I (INSN_BX_INDIRECT_DISP), E (FMT_BX_INDIRECT_DISP) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 134) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CALLX_INDIRECT), E (FMT_CALLX_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_CALLX_INDIRECT_OFFSET), E (FMT_CALLX_INDIRECT_OFFSET) }, { I (INSN_CALLX_INDIRECT_OFFSET), E (FMT_CALLX_INDIRECT_OFFSET) }, 
              { I (INSN_CALLX_INDIRECT_OFFSET), E (FMT_CALLX_INDIRECT_OFFSET) }, { I (INSN_CALLX_INDIRECT_OFFSET), E (FMT_CALLX_INDIRECT_OFFSET) }, 
              { I (INSN_CALLX_DISP), E (FMT_CALLX_DISP) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 136) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDOS_OFFSET), E (FMT_LDOS_OFFSET) }, { I (INSN_LDOS_OFFSET), E (FMT_LDOS_OFFSET) }, 
              { I (INSN_LDOS_OFFSET), E (FMT_LDOS_OFFSET) }, { I (INSN_LDOS_OFFSET), E (FMT_LDOS_OFFSET) }, 
              { I (INSN_LDOS_INDIRECT), E (FMT_LDOS_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDOS_INDIRECT_INDEX), E (FMT_LDOS_INDIRECT_INDEX) }, 
              { I (INSN_LDOS_INDIRECT_OFFSET), E (FMT_LDOS_INDIRECT_OFFSET) }, { I (INSN_LDOS_INDIRECT_OFFSET), E (FMT_LDOS_INDIRECT_OFFSET) }, 
              { I (INSN_LDOS_INDIRECT_OFFSET), E (FMT_LDOS_INDIRECT_OFFSET) }, { I (INSN_LDOS_INDIRECT_OFFSET), E (FMT_LDOS_INDIRECT_OFFSET) }, 
              { I (INSN_LDOS_DISP), E (FMT_LDOS_DISP) }, { I (INSN_LDOS_INDIRECT_DISP), E (FMT_LDOS_INDIRECT_DISP) }, 
              { I (INSN_LDOS_INDEX_DISP), E (FMT_LDOS_INDEX_DISP) }, { I (INSN_LDOS_INDIRECT_INDEX_DISP), E (FMT_LDOS_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 138) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_STOS_OFFSET), E (FMT_STOS_OFFSET) }, { I (INSN_STOS_OFFSET), E (FMT_STOS_OFFSET) }, 
              { I (INSN_STOS_OFFSET), E (FMT_STOS_OFFSET) }, { I (INSN_STOS_OFFSET), E (FMT_STOS_OFFSET) }, 
              { I (INSN_STOS_INDIRECT), E (FMT_STOS_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_STOS_INDIRECT_INDEX), E (FMT_STOS_INDIRECT_INDEX) }, 
              { I (INSN_STOS_INDIRECT_OFFSET), E (FMT_STOS_INDIRECT_OFFSET) }, { I (INSN_STOS_INDIRECT_OFFSET), E (FMT_STOS_INDIRECT_OFFSET) }, 
              { I (INSN_STOS_INDIRECT_OFFSET), E (FMT_STOS_INDIRECT_OFFSET) }, { I (INSN_STOS_INDIRECT_OFFSET), E (FMT_STOS_INDIRECT_OFFSET) }, 
              { I (INSN_STOS_DISP), E (FMT_STOS_DISP) }, { I (INSN_STOS_INDIRECT_DISP), E (FMT_STOS_INDIRECT_DISP) }, 
              { I (INSN_STOS_INDEX_DISP), E (FMT_STOS_INDEX_DISP) }, { I (INSN_STOS_INDIRECT_INDEX_DISP), E (FMT_STOS_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 140) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDA_OFFSET), E (FMT_LDA_OFFSET) }, { I (INSN_LDA_OFFSET), E (FMT_LDA_OFFSET) }, 
              { I (INSN_LDA_OFFSET), E (FMT_LDA_OFFSET) }, { I (INSN_LDA_OFFSET), E (FMT_LDA_OFFSET) }, 
              { I (INSN_LDA_INDIRECT), E (FMT_LDA_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDA_INDIRECT_INDEX), E (FMT_LDA_INDIRECT_INDEX) }, 
              { I (INSN_LDA_INDIRECT_OFFSET), E (FMT_LDA_INDIRECT_OFFSET) }, { I (INSN_LDA_INDIRECT_OFFSET), E (FMT_LDA_INDIRECT_OFFSET) }, 
              { I (INSN_LDA_INDIRECT_OFFSET), E (FMT_LDA_INDIRECT_OFFSET) }, { I (INSN_LDA_INDIRECT_OFFSET), E (FMT_LDA_INDIRECT_OFFSET) }, 
              { I (INSN_LDA_DISP), E (FMT_LDA_DISP) }, { I (INSN_LDA_INDIRECT_DISP), E (FMT_LDA_INDIRECT_DISP) }, 
              { I (INSN_LDA_INDEX_DISP), E (FMT_LDA_INDEX_DISP) }, { I (INSN_LDA_INDIRECT_INDEX_DISP), E (FMT_LDA_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 144) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LD_OFFSET), E (FMT_LD_OFFSET) }, { I (INSN_LD_OFFSET), E (FMT_LD_OFFSET) }, 
              { I (INSN_LD_OFFSET), E (FMT_LD_OFFSET) }, { I (INSN_LD_OFFSET), E (FMT_LD_OFFSET) }, 
              { I (INSN_LD_INDIRECT), E (FMT_LD_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LD_INDIRECT_INDEX), E (FMT_LD_INDIRECT_INDEX) }, 
              { I (INSN_LD_INDIRECT_OFFSET), E (FMT_LD_INDIRECT_OFFSET) }, { I (INSN_LD_INDIRECT_OFFSET), E (FMT_LD_INDIRECT_OFFSET) }, 
              { I (INSN_LD_INDIRECT_OFFSET), E (FMT_LD_INDIRECT_OFFSET) }, { I (INSN_LD_INDIRECT_OFFSET), E (FMT_LD_INDIRECT_OFFSET) }, 
              { I (INSN_LD_DISP), E (FMT_LD_DISP) }, { I (INSN_LD_INDIRECT_DISP), E (FMT_LD_INDIRECT_DISP) }, 
              { I (INSN_LD_INDEX_DISP), E (FMT_LD_INDEX_DISP) }, { I (INSN_LD_INDIRECT_INDEX_DISP), E (FMT_LD_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 146) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_ST_OFFSET), E (FMT_ST_OFFSET) }, { I (INSN_ST_OFFSET), E (FMT_ST_OFFSET) }, 
              { I (INSN_ST_OFFSET), E (FMT_ST_OFFSET) }, { I (INSN_ST_OFFSET), E (FMT_ST_OFFSET) }, 
              { I (INSN_ST_INDIRECT), E (FMT_ST_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_ST_INDIRECT_INDEX), E (FMT_ST_INDIRECT_INDEX) }, 
              { I (INSN_ST_INDIRECT_OFFSET), E (FMT_ST_INDIRECT_OFFSET) }, { I (INSN_ST_INDIRECT_OFFSET), E (FMT_ST_INDIRECT_OFFSET) }, 
              { I (INSN_ST_INDIRECT_OFFSET), E (FMT_ST_INDIRECT_OFFSET) }, { I (INSN_ST_INDIRECT_OFFSET), E (FMT_ST_INDIRECT_OFFSET) }, 
              { I (INSN_ST_DISP), E (FMT_ST_DISP) }, { I (INSN_ST_INDIRECT_DISP), E (FMT_ST_INDIRECT_DISP) }, 
              { I (INSN_ST_INDEX_DISP), E (FMT_ST_INDEX_DISP) }, { I (INSN_ST_INDIRECT_INDEX_DISP), E (FMT_ST_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 152) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDL_OFFSET), E (FMT_LDL_OFFSET) }, { I (INSN_LDL_OFFSET), E (FMT_LDL_OFFSET) }, 
              { I (INSN_LDL_OFFSET), E (FMT_LDL_OFFSET) }, { I (INSN_LDL_OFFSET), E (FMT_LDL_OFFSET) }, 
              { I (INSN_LDL_INDIRECT), E (FMT_LDL_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDL_INDIRECT_INDEX), E (FMT_LDL_INDIRECT_INDEX) }, 
              { I (INSN_LDL_INDIRECT_OFFSET), E (FMT_LDL_INDIRECT_OFFSET) }, { I (INSN_LDL_INDIRECT_OFFSET), E (FMT_LDL_INDIRECT_OFFSET) }, 
              { I (INSN_LDL_INDIRECT_OFFSET), E (FMT_LDL_INDIRECT_OFFSET) }, { I (INSN_LDL_INDIRECT_OFFSET), E (FMT_LDL_INDIRECT_OFFSET) }, 
              { I (INSN_LDL_DISP), E (FMT_LDL_DISP) }, { I (INSN_LDL_INDIRECT_DISP), E (FMT_LDL_INDIRECT_DISP) }, 
              { I (INSN_LDL_INDEX_DISP), E (FMT_LDL_INDEX_DISP) }, { I (INSN_LDL_INDIRECT_INDEX_DISP), E (FMT_LDL_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 154) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_STL_OFFSET), E (FMT_STL_OFFSET) }, { I (INSN_STL_OFFSET), E (FMT_STL_OFFSET) }, 
              { I (INSN_STL_OFFSET), E (FMT_STL_OFFSET) }, { I (INSN_STL_OFFSET), E (FMT_STL_OFFSET) }, 
              { I (INSN_STL_INDIRECT), E (FMT_STL_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_STL_INDIRECT_INDEX), E (FMT_STL_INDIRECT_INDEX) }, 
              { I (INSN_STL_INDIRECT_OFFSET), E (FMT_STL_INDIRECT_OFFSET) }, { I (INSN_STL_INDIRECT_OFFSET), E (FMT_STL_INDIRECT_OFFSET) }, 
              { I (INSN_STL_INDIRECT_OFFSET), E (FMT_STL_INDIRECT_OFFSET) }, { I (INSN_STL_INDIRECT_OFFSET), E (FMT_STL_INDIRECT_OFFSET) }, 
              { I (INSN_STL_DISP), E (FMT_STL_DISP) }, { I (INSN_STL_INDIRECT_DISP), E (FMT_STL_INDIRECT_DISP) }, 
              { I (INSN_STL_INDEX_DISP), E (FMT_STL_INDEX_DISP) }, { I (INSN_STL_INDIRECT_INDEX_DISP), E (FMT_STL_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 160) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDT_OFFSET), E (FMT_LDT_OFFSET) }, { I (INSN_LDT_OFFSET), E (FMT_LDT_OFFSET) }, 
              { I (INSN_LDT_OFFSET), E (FMT_LDT_OFFSET) }, { I (INSN_LDT_OFFSET), E (FMT_LDT_OFFSET) }, 
              { I (INSN_LDT_INDIRECT), E (FMT_LDT_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDT_INDIRECT_INDEX), E (FMT_LDT_INDIRECT_INDEX) }, 
              { I (INSN_LDT_INDIRECT_OFFSET), E (FMT_LDT_INDIRECT_OFFSET) }, { I (INSN_LDT_INDIRECT_OFFSET), E (FMT_LDT_INDIRECT_OFFSET) }, 
              { I (INSN_LDT_INDIRECT_OFFSET), E (FMT_LDT_INDIRECT_OFFSET) }, { I (INSN_LDT_INDIRECT_OFFSET), E (FMT_LDT_INDIRECT_OFFSET) }, 
              { I (INSN_LDT_DISP), E (FMT_LDT_DISP) }, { I (INSN_LDT_INDIRECT_DISP), E (FMT_LDT_INDIRECT_DISP) }, 
              { I (INSN_LDT_INDEX_DISP), E (FMT_LDT_INDEX_DISP) }, { I (INSN_LDT_INDIRECT_INDEX_DISP), E (FMT_LDT_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 162) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_STT_OFFSET), E (FMT_STT_OFFSET) }, { I (INSN_STT_OFFSET), E (FMT_STT_OFFSET) }, 
              { I (INSN_STT_OFFSET), E (FMT_STT_OFFSET) }, { I (INSN_STT_OFFSET), E (FMT_STT_OFFSET) }, 
              { I (INSN_STT_INDIRECT), E (FMT_STT_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_STT_INDIRECT_INDEX), E (FMT_STT_INDIRECT_INDEX) }, 
              { I (INSN_STT_INDIRECT_OFFSET), E (FMT_STT_INDIRECT_OFFSET) }, { I (INSN_STT_INDIRECT_OFFSET), E (FMT_STT_INDIRECT_OFFSET) }, 
              { I (INSN_STT_INDIRECT_OFFSET), E (FMT_STT_INDIRECT_OFFSET) }, { I (INSN_STT_INDIRECT_OFFSET), E (FMT_STT_INDIRECT_OFFSET) }, 
              { I (INSN_STT_DISP), E (FMT_STT_DISP) }, { I (INSN_STT_INDIRECT_DISP), E (FMT_STT_INDIRECT_DISP) }, 
              { I (INSN_STT_INDEX_DISP), E (FMT_STT_INDEX_DISP) }, { I (INSN_STT_INDIRECT_INDEX_DISP), E (FMT_STT_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 176) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDQ_OFFSET), E (FMT_LDQ_OFFSET) }, { I (INSN_LDQ_OFFSET), E (FMT_LDQ_OFFSET) }, 
              { I (INSN_LDQ_OFFSET), E (FMT_LDQ_OFFSET) }, { I (INSN_LDQ_OFFSET), E (FMT_LDQ_OFFSET) }, 
              { I (INSN_LDQ_INDIRECT), E (FMT_LDQ_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDQ_INDIRECT_INDEX), E (FMT_LDQ_INDIRECT_INDEX) }, 
              { I (INSN_LDQ_INDIRECT_OFFSET), E (FMT_LDQ_INDIRECT_OFFSET) }, { I (INSN_LDQ_INDIRECT_OFFSET), E (FMT_LDQ_INDIRECT_OFFSET) }, 
              { I (INSN_LDQ_INDIRECT_OFFSET), E (FMT_LDQ_INDIRECT_OFFSET) }, { I (INSN_LDQ_INDIRECT_OFFSET), E (FMT_LDQ_INDIRECT_OFFSET) }, 
              { I (INSN_LDQ_DISP), E (FMT_LDQ_DISP) }, { I (INSN_LDQ_INDIRECT_DISP), E (FMT_LDQ_INDIRECT_DISP) }, 
              { I (INSN_LDQ_INDEX_DISP), E (FMT_LDQ_INDEX_DISP) }, { I (INSN_LDQ_INDIRECT_INDEX_DISP), E (FMT_LDQ_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 178) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_STQ_OFFSET), E (FMT_STQ_OFFSET) }, { I (INSN_STQ_OFFSET), E (FMT_STQ_OFFSET) }, 
              { I (INSN_STQ_OFFSET), E (FMT_STQ_OFFSET) }, { I (INSN_STQ_OFFSET), E (FMT_STQ_OFFSET) }, 
              { I (INSN_STQ_INDIRECT), E (FMT_STQ_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_STQ_INDIRECT_INDEX), E (FMT_STQ_INDIRECT_INDEX) }, 
              { I (INSN_STQ_INDIRECT_OFFSET), E (FMT_STQ_INDIRECT_OFFSET) }, { I (INSN_STQ_INDIRECT_OFFSET), E (FMT_STQ_INDIRECT_OFFSET) }, 
              { I (INSN_STQ_INDIRECT_OFFSET), E (FMT_STQ_INDIRECT_OFFSET) }, { I (INSN_STQ_INDIRECT_OFFSET), E (FMT_STQ_INDIRECT_OFFSET) }, 
              { I (INSN_STQ_DISP), E (FMT_STQ_DISP) }, { I (INSN_STQ_INDIRECT_DISP), E (FMT_STQ_INDIRECT_DISP) }, 
              { I (INSN_STQ_INDEX_DISP), E (FMT_STQ_INDEX_DISP) }, { I (INSN_STQ_INDIRECT_INDEX_DISP), E (FMT_STQ_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 192) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDIB_OFFSET), E (FMT_LDIB_OFFSET) }, { I (INSN_LDIB_OFFSET), E (FMT_LDIB_OFFSET) }, 
              { I (INSN_LDIB_OFFSET), E (FMT_LDIB_OFFSET) }, { I (INSN_LDIB_OFFSET), E (FMT_LDIB_OFFSET) }, 
              { I (INSN_LDIB_INDIRECT), E (FMT_LDIB_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDIB_INDIRECT_INDEX), E (FMT_LDIB_INDIRECT_INDEX) }, 
              { I (INSN_LDIB_INDIRECT_OFFSET), E (FMT_LDIB_INDIRECT_OFFSET) }, { I (INSN_LDIB_INDIRECT_OFFSET), E (FMT_LDIB_INDIRECT_OFFSET) }, 
              { I (INSN_LDIB_INDIRECT_OFFSET), E (FMT_LDIB_INDIRECT_OFFSET) }, { I (INSN_LDIB_INDIRECT_OFFSET), E (FMT_LDIB_INDIRECT_OFFSET) }, 
              { I (INSN_LDIB_DISP), E (FMT_LDIB_DISP) }, { I (INSN_LDIB_INDIRECT_DISP), E (FMT_LDIB_INDIRECT_DISP) }, 
              { I (INSN_LDIB_INDEX_DISP), E (FMT_LDIB_INDEX_DISP) }, { I (INSN_LDIB_INDIRECT_INDEX_DISP), E (FMT_LDIB_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        CASE (0, 200) :
          {
            static const DECODE_DESC insns[16] = {
              { I (INSN_LDIS_OFFSET), E (FMT_LDIS_OFFSET) }, { I (INSN_LDIS_OFFSET), E (FMT_LDIS_OFFSET) }, 
              { I (INSN_LDIS_OFFSET), E (FMT_LDIS_OFFSET) }, { I (INSN_LDIS_OFFSET), E (FMT_LDIS_OFFSET) }, 
              { I (INSN_LDIS_INDIRECT), E (FMT_LDIS_INDIRECT) }, { I (INSN_X_INVALID), E (FMT_EMPTY) }, 
              { I (INSN_X_INVALID), E (FMT_EMPTY) }, { I (INSN_LDIS_INDIRECT_INDEX), E (FMT_LDIS_INDIRECT_INDEX) }, 
              { I (INSN_LDIS_INDIRECT_OFFSET), E (FMT_LDIS_INDIRECT_OFFSET) }, { I (INSN_LDIS_INDIRECT_OFFSET), E (FMT_LDIS_INDIRECT_OFFSET) }, 
              { I (INSN_LDIS_INDIRECT_OFFSET), E (FMT_LDIS_INDIRECT_OFFSET) }, { I (INSN_LDIS_INDIRECT_OFFSET), E (FMT_LDIS_INDIRECT_OFFSET) }, 
              { I (INSN_LDIS_DISP), E (FMT_LDIS_DISP) }, { I (INSN_LDIS_INDIRECT_DISP), E (FMT_LDIS_INDIRECT_DISP) }, 
              { I (INSN_LDIS_INDEX_DISP), E (FMT_LDIS_INDEX_DISP) }, { I (INSN_LDIS_INDIRECT_INDEX_DISP), E (FMT_LDIS_INDIRECT_INDEX_DISP) }, 
            };
            unsigned int val = (((insn >> 10) & (15 << 0)));
            idecode = &insns[val];
            GOTO_EXTRACT (idecode);
          }
        DEFAULT (0) :
          idecode = &insns[val];
          GOTO_EXTRACT (idecode);
        }
      ENDSWITCH (0)
    }
#undef I
#undef E
  }

  /* The instruction has been decoded, now extract the fields.  */

 extract:
  {
#ifndef __GNUC__
    switch (idecode->sfmt)
#endif
      {

  CASE (ex, FMT_EMPTY) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_empty.f
  EXTRACT_IFMT_EMPTY_VARS /* */

  EXTRACT_IFMT_EMPTY_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_empty", (char *) 0));

#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MULO) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mulo.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulo", "src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MULO1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mulo1.f
  EXTRACT_IFMT_MULO1_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulo1", "f_src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MULO2) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mulo2.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src2) = f_src2;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulo2", "f_src2 0x%x", 'x', f_src2, "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MULO3) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_mulo3.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_src2) = f_src2;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_mulo3", "f_src1 0x%x", 'x', f_src1, "f_src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOTBIT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_notbit.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_notbit", "src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOTBIT1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_notbit1.f
  EXTRACT_IFMT_MULO1_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_notbit1", "f_src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOTBIT2) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_notbit2.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src2) = f_src2;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_notbit2", "f_src2 0x%x", 'x', f_src2, "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOTBIT3) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_notbit3.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_src2) = f_src2;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_notbit3", "f_src1 0x%x", 'x', f_src1, "f_src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_not.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_not", "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOT1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_not1.f
  EXTRACT_IFMT_MULO1_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_not1", "f_src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOT2) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_not2.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_not2", "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_NOT3) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_not3.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_not3", "f_src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_EMUL) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_emul.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_emul", "f_srcdst 0x%x", 'x', f_srcdst, "src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_EMUL1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_emul1.f
  EXTRACT_IFMT_MULO1_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_src1) = f_src1;
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_emul1", "f_srcdst 0x%x", 'x', f_srcdst, "f_src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_EMUL2) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_emul2.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_src2) = f_src2;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_emul2", "f_srcdst 0x%x", 'x', f_srcdst, "f_src2 0x%x", 'x', f_src2, "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_EMUL3) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_emul3.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_src1) = f_src1;
  FLD (f_src2) = f_src2;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_emul3", "f_srcdst 0x%x", 'x', f_srcdst, "f_src1 0x%x", 'x', f_src1, "f_src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MOVL) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movl.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_srcdst) = f_srcdst;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movl", "f_src1 0x%x", 'x', f_src1, "f_srcdst 0x%x", 'x', f_srcdst, "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_src1_const__WI_1) = ((FLD (f_src1)) + (1));
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MOVL1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movl1.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_src1) = f_src1;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movl1", "f_srcdst 0x%x", 'x', f_srcdst, "f_src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MOVT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movt.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_srcdst) = f_srcdst;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movt", "f_src1 0x%x", 'x', f_src1, "f_srcdst 0x%x", 'x', f_srcdst, "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_src1_const__WI_1) = ((FLD (f_src1)) + (1));
      FLD (in_h_gr_add__VM_index_of_src1_const__WI_2) = ((FLD (f_src1)) + (2));
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MOVT1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movt1.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_src1) = f_src1;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movt1", "f_srcdst 0x%x", 'x', f_srcdst, "f_src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MOVQ) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movq.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_srcdst) = f_srcdst;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movq", "f_src1 0x%x", 'x', f_src1, "f_srcdst 0x%x", 'x', f_srcdst, "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_src1_const__WI_1) = ((FLD (f_src1)) + (1));
      FLD (in_h_gr_add__VM_index_of_src1_const__WI_2) = ((FLD (f_src1)) + (2));
      FLD (in_h_gr_add__VM_index_of_src1_const__WI_3) = ((FLD (f_src1)) + (3));
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MOVQ1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_movq1.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_src1) = f_src1;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_movq1", "f_srcdst 0x%x", 'x', f_srcdst, "f_src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_MODPC) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_modpc.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_modpc", "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_offset", "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_indirect", "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_disp", "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDA_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_lda_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_lda_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_offset", "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_indirect", "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_disp", "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LD_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ld_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ld_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_offset", "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_indirect", "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_disp", "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOB_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldob_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldob_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_offset", "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_indirect", "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_disp", "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDOS_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldos_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldos_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_offset", "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_indirect", "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_disp", "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIB_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldib_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldib_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_offset", "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_indirect", "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_disp", "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDIS_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldis_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldis_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_indirect_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_indirect", "f_srcdst 0x%x", 'x', f_srcdst, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_indirect_index", "f_srcdst 0x%x", 'x', f_srcdst, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_indirect_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDL_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldl_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldl_indirect_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_indirect_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_indirect", "f_srcdst 0x%x", 'x', f_srcdst, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_indirect_index", "f_srcdst 0x%x", 'x', f_srcdst, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_indirect_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDT_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldt_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldt_indirect_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_indirect_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_indirect", "f_srcdst 0x%x", 'x', f_srcdst, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_indirect_index", "f_srcdst 0x%x", 'x', f_srcdst, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_indirect_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_LDQ_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_ldq_indirect_index_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ldq_indirect_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_const__WI_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_offset", "f_offset 0x%x", 'x', f_offset, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_indirect_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_indirect.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_indirect", "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_indirect_index.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_disp", "f_optdisp 0x%x", 'x', f_optdisp, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_indirect_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_ST_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_st_indirect_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_st_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_offset", "f_offset 0x%x", 'x', f_offset, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_indirect_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_indirect.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_indirect", "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_indirect_index.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_disp", "f_optdisp 0x%x", 'x', f_optdisp, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_indirect_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOB_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stob_indirect_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stob_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_offset", "f_offset 0x%x", 'x', f_offset, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_indirect_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_indirect.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_indirect", "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_indirect_index.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_disp", "f_optdisp 0x%x", 'x', f_optdisp, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_indirect_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STOS_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stos_indirect_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stos_indirect_index_disp", "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_indirect_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_indirect_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_indirect.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_indirect", "f_srcdst 0x%x", 'x', f_srcdst, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_indirect_index.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_indirect_index", "f_srcdst 0x%x", 'x', f_srcdst, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_indirect_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_indirect_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STL_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stl_indirect_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stl_indirect_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_indirect_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_indirect_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_indirect.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_indirect", "f_srcdst 0x%x", 'x', f_srcdst, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_indirect_index.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_indirect_index", "f_srcdst 0x%x", 'x', f_srcdst, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_indirect_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_indirect_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STT_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stt_indirect_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stt_indirect_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_indirect_offset.f
  EXTRACT_IFMT_ST_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_ST_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_indirect_offset", "f_srcdst 0x%x", 'x', f_srcdst, "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_indirect.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_indirect", "f_srcdst 0x%x", 'x', f_srcdst, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_indirect_index.f
  EXTRACT_IFMT_ST_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_indirect_index", "f_srcdst 0x%x", 'x', f_srcdst, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_indirect_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_indirect_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_STQ_INDIRECT_INDEX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_stq_indirect_index_disp.f
  EXTRACT_IFMT_ST_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_ST_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_srcdst) = f_srcdst;
  FLD (f_optdisp) = f_optdisp;
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  FLD (i_st_src) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_stq_indirect_index_disp", "f_srcdst 0x%x", 'x', f_srcdst, "f_optdisp 0x%x", 'x', f_optdisp, "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, "st_src 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_const__WI_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPOBE_REG) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_reg.f
  EXTRACT_IFMT_CMPOBE_REG_VARS /* f-opcode f-br-src1 f-br-src2 f-br-m1 f-br-disp f-br-zero */

  EXTRACT_IFMT_CMPOBE_REG_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_br_disp) = f_br_disp;
  FLD (i_br_src1) = & CPU (h_gr)[f_br_src1];
  FLD (i_br_src2) = & CPU (h_gr)[f_br_src2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpobe_reg", "br_disp 0x%x", 'x', f_br_disp, "br_src1 0x%x", 'x', f_br_src1, "br_src2 0x%x", 'x', f_br_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_br_src1) = f_br_src1;
      FLD (in_br_src2) = f_br_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPOBE_LIT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobe_lit.f
  EXTRACT_IFMT_CMPOBE_LIT_VARS /* f-opcode f-br-src1 f-br-src2 f-br-m1 f-br-disp f-br-zero */

  EXTRACT_IFMT_CMPOBE_LIT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_br_src1) = f_br_src1;
  FLD (i_br_disp) = f_br_disp;
  FLD (i_br_src2) = & CPU (h_gr)[f_br_src2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpobe_lit", "f_br_src1 0x%x", 'x', f_br_src1, "br_disp 0x%x", 'x', f_br_disp, "br_src2 0x%x", 'x', f_br_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_br_src2) = f_br_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPOBL_REG) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_reg.f
  EXTRACT_IFMT_CMPOBE_REG_VARS /* f-opcode f-br-src1 f-br-src2 f-br-m1 f-br-disp f-br-zero */

  EXTRACT_IFMT_CMPOBE_REG_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_br_disp) = f_br_disp;
  FLD (i_br_src1) = & CPU (h_gr)[f_br_src1];
  FLD (i_br_src2) = & CPU (h_gr)[f_br_src2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpobl_reg", "br_disp 0x%x", 'x', f_br_disp, "br_src1 0x%x", 'x', f_br_src1, "br_src2 0x%x", 'x', f_br_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_br_src1) = f_br_src1;
      FLD (in_br_src2) = f_br_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPOBL_LIT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_cmpobl_lit.f
  EXTRACT_IFMT_CMPOBE_LIT_VARS /* f-opcode f-br-src1 f-br-src2 f-br-m1 f-br-disp f-br-zero */

  EXTRACT_IFMT_CMPOBE_LIT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_br_src1) = f_br_src1;
  FLD (i_br_disp) = f_br_disp;
  FLD (i_br_src2) = & CPU (h_gr)[f_br_src2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpobl_lit", "f_br_src1 0x%x", 'x', f_br_src1, "br_disp 0x%x", 'x', f_br_disp, "br_src2 0x%x", 'x', f_br_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_br_src2) = f_br_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BBC_REG) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bbc_reg.f
  EXTRACT_IFMT_CMPOBE_REG_VARS /* f-opcode f-br-src1 f-br-src2 f-br-m1 f-br-disp f-br-zero */

  EXTRACT_IFMT_CMPOBE_REG_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_br_disp) = f_br_disp;
  FLD (i_br_src1) = & CPU (h_gr)[f_br_src1];
  FLD (i_br_src2) = & CPU (h_gr)[f_br_src2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bbc_reg", "br_disp 0x%x", 'x', f_br_disp, "br_src1 0x%x", 'x', f_br_src1, "br_src2 0x%x", 'x', f_br_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_br_src1) = f_br_src1;
      FLD (in_br_src2) = f_br_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BBC_LIT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bbc_lit.f
  EXTRACT_IFMT_CMPOBE_LIT_VARS /* f-opcode f-br-src1 f-br-src2 f-br-m1 f-br-disp f-br-zero */

  EXTRACT_IFMT_CMPOBE_LIT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_br_src1) = f_br_src1;
  FLD (i_br_disp) = f_br_disp;
  FLD (i_br_src2) = & CPU (h_gr)[f_br_src2];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bbc_lit", "f_br_src1 0x%x", 'x', f_br_src1, "br_disp 0x%x", 'x', f_br_disp, "br_src2 0x%x", 'x', f_br_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_br_src2) = f_br_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPI) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpi.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpi", "src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (in_src2) = f_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPI1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpi1.f
  EXTRACT_IFMT_MULO1_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpi1", "f_src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPI2) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpi2.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src2) = f_src2;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpi2", "f_src2 0x%x", 'x', f_src2, "src1 0x%x", 'x', f_src1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPI3) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpi3.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_src2) = f_src2;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpi3", "f_src1 0x%x", 'x', f_src1, "f_src2 0x%x", 'x', f_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPO) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpo.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpo", "src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (in_src2) = f_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPO1) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpo1.f
  EXTRACT_IFMT_MULO1_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpo1", "f_src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_src2;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPO2) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpo2.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src2) = f_src2;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpo2", "f_src2 0x%x", 'x', f_src2, "src1 0x%x", 'x', f_src1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CMPO3) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_cmpo3.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_src2) = f_src2;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_cmpo3", "f_src1 0x%x", 'x', f_src1, "f_src2 0x%x", 'x', f_src2, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_TESTNO_REG) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_testno_reg.f
  EXTRACT_IFMT_CMPOBE_REG_VARS /* f-opcode f-br-src1 f-br-src2 f-br-m1 f-br-disp f-br-zero */

  EXTRACT_IFMT_CMPOBE_REG_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_br_src1) = & CPU (h_gr)[f_br_src1];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_testno_reg", "br_src1 0x%x", 'x', f_br_src1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_br_src1) = f_br_src1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BNO) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bno.f
  EXTRACT_IFMT_BNO_VARS /* f-opcode f-ctrl-disp f-ctrl-zero */

  EXTRACT_IFMT_BNO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_ctrl_disp) = f_ctrl_disp;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bno", "ctrl_disp 0x%x", 'x', f_ctrl_disp, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_B) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_b.f
  EXTRACT_IFMT_BNO_VARS /* f-opcode f-ctrl-disp f-ctrl-zero */

  EXTRACT_IFMT_BNO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_ctrl_disp) = f_ctrl_disp;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_b", "ctrl_disp 0x%x", 'x', f_ctrl_disp, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BX_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bx_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BX_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bx_indirect", "abase 0x%x", 'x', f_abase, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BX_INDIRECT_INDEX) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect_index.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_scale) = f_scale;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  FLD (i_index) = & CPU (h_gr)[f_index];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bx_indirect_index", "f_scale 0x%x", 'x', f_scale, "abase 0x%x", 'x', f_abase, "index 0x%x", 'x', f_index, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_index) = f_index;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bx_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bx_disp", "f_optdisp 0x%x", 'x', f_optdisp, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_BX_INDIRECT_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_bx_indirect_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_bx_indirect_disp", "f_optdisp 0x%x", 'x', f_optdisp, "abase 0x%x", 'x', f_abase, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CALLX_DISP) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_callx_disp.f
  EXTRACT_IFMT_LDA_DISP_VARS /* f-opcode f-optdisp f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_DISP_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_optdisp) = f_optdisp;
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_callx_disp", "f_optdisp 0x%x", 'x', f_optdisp, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_0) = 0;
      FLD (in_h_gr_1) = 1;
      FLD (in_h_gr_10) = 10;
      FLD (in_h_gr_11) = 11;
      FLD (in_h_gr_12) = 12;
      FLD (in_h_gr_13) = 13;
      FLD (in_h_gr_14) = 14;
      FLD (in_h_gr_15) = 15;
      FLD (in_h_gr_2) = 2;
      FLD (in_h_gr_3) = 3;
      FLD (in_h_gr_31) = 31;
      FLD (in_h_gr_4) = 4;
      FLD (in_h_gr_5) = 5;
      FLD (in_h_gr_6) = 6;
      FLD (in_h_gr_7) = 7;
      FLD (in_h_gr_8) = 8;
      FLD (in_h_gr_9) = 9;
      FLD (out_h_gr_0) = 0;
      FLD (out_h_gr_1) = 1;
      FLD (out_h_gr_10) = 10;
      FLD (out_h_gr_11) = 11;
      FLD (out_h_gr_12) = 12;
      FLD (out_h_gr_13) = 13;
      FLD (out_h_gr_14) = 14;
      FLD (out_h_gr_15) = 15;
      FLD (out_h_gr_2) = 2;
      FLD (out_h_gr_3) = 3;
      FLD (out_h_gr_31) = 31;
      FLD (out_h_gr_4) = 4;
      FLD (out_h_gr_5) = 5;
      FLD (out_h_gr_6) = 6;
      FLD (out_h_gr_7) = 7;
      FLD (out_h_gr_8) = 8;
      FLD (out_h_gr_9) = 9;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CALLX_INDIRECT) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_callx_indirect.f
  EXTRACT_IFMT_LDA_INDIRECT_VARS /* f-opcode f-srcdst f-abase f-modeb f-scale f-zerob f-index */

  EXTRACT_IFMT_LDA_INDIRECT_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_callx_indirect", "abase 0x%x", 'x', f_abase, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_0) = 0;
      FLD (in_h_gr_1) = 1;
      FLD (in_h_gr_10) = 10;
      FLD (in_h_gr_11) = 11;
      FLD (in_h_gr_12) = 12;
      FLD (in_h_gr_13) = 13;
      FLD (in_h_gr_14) = 14;
      FLD (in_h_gr_15) = 15;
      FLD (in_h_gr_2) = 2;
      FLD (in_h_gr_3) = 3;
      FLD (in_h_gr_31) = 31;
      FLD (in_h_gr_4) = 4;
      FLD (in_h_gr_5) = 5;
      FLD (in_h_gr_6) = 6;
      FLD (in_h_gr_7) = 7;
      FLD (in_h_gr_8) = 8;
      FLD (in_h_gr_9) = 9;
      FLD (out_h_gr_0) = 0;
      FLD (out_h_gr_1) = 1;
      FLD (out_h_gr_10) = 10;
      FLD (out_h_gr_11) = 11;
      FLD (out_h_gr_12) = 12;
      FLD (out_h_gr_13) = 13;
      FLD (out_h_gr_14) = 14;
      FLD (out_h_gr_15) = 15;
      FLD (out_h_gr_2) = 2;
      FLD (out_h_gr_3) = 3;
      FLD (out_h_gr_31) = 31;
      FLD (out_h_gr_4) = 4;
      FLD (out_h_gr_5) = 5;
      FLD (out_h_gr_6) = 6;
      FLD (out_h_gr_7) = 7;
      FLD (out_h_gr_8) = 8;
      FLD (out_h_gr_9) = 9;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CALLX_INDIRECT_OFFSET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_callx_indirect_offset.f
  EXTRACT_IFMT_LDA_OFFSET_VARS /* f-opcode f-srcdst f-abase f-modea f-zeroa f-offset */

  EXTRACT_IFMT_LDA_OFFSET_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_offset) = f_offset;
  FLD (i_abase) = & CPU (h_gr)[f_abase];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_callx_indirect_offset", "f_offset 0x%x", 'x', f_offset, "abase 0x%x", 'x', f_abase, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_abase) = f_abase;
      FLD (in_h_gr_0) = 0;
      FLD (in_h_gr_1) = 1;
      FLD (in_h_gr_10) = 10;
      FLD (in_h_gr_11) = 11;
      FLD (in_h_gr_12) = 12;
      FLD (in_h_gr_13) = 13;
      FLD (in_h_gr_14) = 14;
      FLD (in_h_gr_15) = 15;
      FLD (in_h_gr_2) = 2;
      FLD (in_h_gr_3) = 3;
      FLD (in_h_gr_31) = 31;
      FLD (in_h_gr_4) = 4;
      FLD (in_h_gr_5) = 5;
      FLD (in_h_gr_6) = 6;
      FLD (in_h_gr_7) = 7;
      FLD (in_h_gr_8) = 8;
      FLD (in_h_gr_9) = 9;
      FLD (out_h_gr_0) = 0;
      FLD (out_h_gr_1) = 1;
      FLD (out_h_gr_10) = 10;
      FLD (out_h_gr_11) = 11;
      FLD (out_h_gr_12) = 12;
      FLD (out_h_gr_13) = 13;
      FLD (out_h_gr_14) = 14;
      FLD (out_h_gr_15) = 15;
      FLD (out_h_gr_2) = 2;
      FLD (out_h_gr_3) = 3;
      FLD (out_h_gr_31) = 31;
      FLD (out_h_gr_4) = 4;
      FLD (out_h_gr_5) = 5;
      FLD (out_h_gr_6) = 6;
      FLD (out_h_gr_7) = 7;
      FLD (out_h_gr_8) = 8;
      FLD (out_h_gr_9) = 9;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_RET) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_ret.f
  EXTRACT_IFMT_BNO_VARS /* f-opcode f-ctrl-disp f-ctrl-zero */

  EXTRACT_IFMT_BNO_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_ret", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_h_gr_0) = 0;
      FLD (in_h_gr_2) = 2;
      FLD (in_h_gr_31) = 31;
      FLD (out_h_gr_0) = 0;
      FLD (out_h_gr_1) = 1;
      FLD (out_h_gr_10) = 10;
      FLD (out_h_gr_11) = 11;
      FLD (out_h_gr_12) = 12;
      FLD (out_h_gr_13) = 13;
      FLD (out_h_gr_14) = 14;
      FLD (out_h_gr_15) = 15;
      FLD (out_h_gr_2) = 2;
      FLD (out_h_gr_3) = 3;
      FLD (out_h_gr_31) = 31;
      FLD (out_h_gr_4) = 4;
      FLD (out_h_gr_5) = 5;
      FLD (out_h_gr_6) = 6;
      FLD (out_h_gr_7) = 7;
      FLD (out_h_gr_8) = 8;
      FLD (out_h_gr_9) = 9;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_CALLS) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_calls.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_calls", "src1 0x%x", 'x', f_src1, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_FMARK) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.cti.fields.fmt_fmark.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  SEM_BRANCH_INIT_EXTRACT (abuf);
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_fmark", (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
    }
#endif
#undef FLD
  BREAK (ex);
  }

  CASE (ex, FMT_FLUSHREG) :
  {
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_flushreg.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_flushreg", (char *) 0));

#undef FLD
  BREAK (ex);
  }


      }
    ENDSWITCH (ex)

  }

  return idecode->idesc;
}
