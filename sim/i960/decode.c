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

/* The INSN_ prefix is not here and is instead part of the `insn' argument
   to avoid collisions with header files (e.g. `AND' in ansidecl.h).  */
#define IDX(insn) CONCAT2 (I960BASE_,insn)
#define TYPE(insn) CONCAT2 (I960_,insn)

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC i960base_insn_data[I960BASE_INSN_MAX];

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
  { TYPE (INSN_XNOR), IDX (INSN_XNOR), FULL (xnor) FAST (xnor) },
  { TYPE (INSN_XNOR1), IDX (INSN_XNOR1), FULL (xnor1) FAST (xnor1) },
  { TYPE (INSN_XNOR2), IDX (INSN_XNOR2), FULL (xnor2) FAST (xnor2) },
  { TYPE (INSN_XNOR3), IDX (INSN_XNOR3), FULL (xnor3) FAST (xnor3) },
  { TYPE (INSN_NOT), IDX (INSN_NOT), FULL (not) FAST (not) },
  { TYPE (INSN_NOT1), IDX (INSN_NOT1), FULL (not1) FAST (not1) },
  { TYPE (INSN_NOT2), IDX (INSN_NOT2), FULL (not2) FAST (not2) },
  { TYPE (INSN_NOT3), IDX (INSN_NOT3), FULL (not3) FAST (not3) },
  { TYPE (INSN_ORNOT), IDX (INSN_ORNOT), FULL (ornot) FAST (ornot) },
  { TYPE (INSN_ORNOT1), IDX (INSN_ORNOT1), FULL (ornot1) FAST (ornot1) },
  { TYPE (INSN_ORNOT2), IDX (INSN_ORNOT2), FULL (ornot2) FAST (ornot2) },
  { TYPE (INSN_ORNOT3), IDX (INSN_ORNOT3), FULL (ornot3) FAST (ornot3) },
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

#undef FMT
#undef FULL
#undef FAST
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

/* Given an instruction, return a pointer to its IDESC entry.  */

const IDESC *
i960base_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn,
              ARGBUF *abuf)
{
  /* Result of decoder.  */
  I960BASE_INSN_TYPE itype;

  {
    CGEN_INSN_INT insn = base_insn;

    {
      unsigned int val = (((insn >> 24) & (255 << 0)));
      switch (val)
      {
      case 8 : itype = I960BASE_INSN_B; goto extract_fmt_b;
      case 10 : itype = I960BASE_INSN_RET; goto extract_fmt_ret;
      case 16 : itype = I960BASE_INSN_BNO; goto extract_fmt_bno;
      case 17 : itype = I960BASE_INSN_BG; goto extract_fmt_bno;
      case 18 : itype = I960BASE_INSN_BE; goto extract_fmt_bno;
      case 19 : itype = I960BASE_INSN_BGE; goto extract_fmt_bno;
      case 20 : itype = I960BASE_INSN_BL; goto extract_fmt_bno;
      case 21 : itype = I960BASE_INSN_BNE; goto extract_fmt_bno;
      case 22 : itype = I960BASE_INSN_BLE; goto extract_fmt_bno;
      case 23 : itype = I960BASE_INSN_BO; goto extract_fmt_bno;
      case 32 : itype = I960BASE_INSN_TESTNO_REG; goto extract_fmt_testno_reg;
      case 33 : itype = I960BASE_INSN_TESTG_REG; goto extract_fmt_testno_reg;
      case 34 : itype = I960BASE_INSN_TESTE_REG; goto extract_fmt_testno_reg;
      case 35 : itype = I960BASE_INSN_TESTGE_REG; goto extract_fmt_testno_reg;
      case 36 : itype = I960BASE_INSN_TESTL_REG; goto extract_fmt_testno_reg;
      case 37 : itype = I960BASE_INSN_TESTNE_REG; goto extract_fmt_testno_reg;
      case 38 : itype = I960BASE_INSN_TESTLE_REG; goto extract_fmt_testno_reg;
      case 39 : itype = I960BASE_INSN_TESTO_REG; goto extract_fmt_testno_reg;
      case 48 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_BBC_REG; goto extract_fmt_bbc_reg;
          case 4 : itype = I960BASE_INSN_BBC_LIT; goto extract_fmt_bbc_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 49 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPOBG_REG; goto extract_fmt_cmpobl_reg;
          case 4 : itype = I960BASE_INSN_CMPOBG_LIT; goto extract_fmt_cmpobl_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 50 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPOBE_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPOBE_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 51 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPOBGE_REG; goto extract_fmt_cmpobl_reg;
          case 4 : itype = I960BASE_INSN_CMPOBGE_LIT; goto extract_fmt_cmpobl_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 52 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPOBL_REG; goto extract_fmt_cmpobl_reg;
          case 4 : itype = I960BASE_INSN_CMPOBL_LIT; goto extract_fmt_cmpobl_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 53 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPOBNE_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPOBNE_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 54 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPOBLE_REG; goto extract_fmt_cmpobl_reg;
          case 4 : itype = I960BASE_INSN_CMPOBLE_LIT; goto extract_fmt_cmpobl_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 55 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_BBS_REG; goto extract_fmt_bbc_reg;
          case 4 : itype = I960BASE_INSN_BBS_LIT; goto extract_fmt_bbc_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 57 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPIBG_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPIBG_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 58 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPIBE_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPIBE_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 59 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPIBGE_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPIBGE_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 60 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPIBL_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPIBL_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 61 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPIBNE_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPIBNE_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 62 :
        {
          unsigned int val = (((insn >> 11) & (1 << 2)) | ((insn >> 0) & (3 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_CMPIBLE_REG; goto extract_fmt_cmpobe_reg;
          case 4 : itype = I960BASE_INSN_CMPIBLE_LIT; goto extract_fmt_cmpobe_lit;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 88 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOTBIT; goto extract_fmt_notbit;
              case 2 : itype = I960BASE_INSN_AND; goto extract_fmt_mulo;
              case 4 : itype = I960BASE_INSN_ANDNOT; goto extract_fmt_mulo;
              case 6 : itype = I960BASE_INSN_SETBIT; goto extract_fmt_notbit;
              case 8 : itype = I960BASE_INSN_NOTAND; goto extract_fmt_mulo;
              case 12 : itype = I960BASE_INSN_XOR; goto extract_fmt_mulo;
              case 14 : itype = I960BASE_INSN_OR; goto extract_fmt_mulo;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 1 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOR; goto extract_fmt_mulo;
              case 2 : itype = I960BASE_INSN_XNOR; goto extract_fmt_mulo;
              case 4 : itype = I960BASE_INSN_NOT; goto extract_fmt_not;
              case 6 : itype = I960BASE_INSN_ORNOT; goto extract_fmt_mulo;
              case 8 : itype = I960BASE_INSN_CLRBIT; goto extract_fmt_notbit;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 2 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOTBIT1; goto extract_fmt_notbit1;
              case 2 : itype = I960BASE_INSN_AND1; goto extract_fmt_mulo1;
              case 4 : itype = I960BASE_INSN_ANDNOT1; goto extract_fmt_mulo1;
              case 6 : itype = I960BASE_INSN_SETBIT1; goto extract_fmt_notbit1;
              case 8 : itype = I960BASE_INSN_NOTAND1; goto extract_fmt_mulo1;
              case 12 : itype = I960BASE_INSN_XOR1; goto extract_fmt_mulo1;
              case 14 : itype = I960BASE_INSN_OR1; goto extract_fmt_mulo1;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 3 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOR1; goto extract_fmt_mulo1;
              case 2 : itype = I960BASE_INSN_XNOR1; goto extract_fmt_mulo1;
              case 4 : itype = I960BASE_INSN_NOT1; goto extract_fmt_not1;
              case 6 : itype = I960BASE_INSN_ORNOT1; goto extract_fmt_mulo1;
              case 8 : itype = I960BASE_INSN_CLRBIT1; goto extract_fmt_notbit1;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 4 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOTBIT2; goto extract_fmt_notbit2;
              case 2 : itype = I960BASE_INSN_AND2; goto extract_fmt_mulo2;
              case 4 : itype = I960BASE_INSN_ANDNOT2; goto extract_fmt_mulo2;
              case 6 : itype = I960BASE_INSN_SETBIT2; goto extract_fmt_notbit2;
              case 8 : itype = I960BASE_INSN_NOTAND2; goto extract_fmt_mulo2;
              case 12 : itype = I960BASE_INSN_XOR2; goto extract_fmt_mulo2;
              case 14 : itype = I960BASE_INSN_OR2; goto extract_fmt_mulo2;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 5 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOR2; goto extract_fmt_mulo2;
              case 2 : itype = I960BASE_INSN_XNOR2; goto extract_fmt_mulo2;
              case 4 : itype = I960BASE_INSN_NOT2; goto extract_fmt_not2;
              case 6 : itype = I960BASE_INSN_ORNOT2; goto extract_fmt_mulo2;
              case 8 : itype = I960BASE_INSN_CLRBIT2; goto extract_fmt_notbit2;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 6 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOTBIT3; goto extract_fmt_notbit3;
              case 2 : itype = I960BASE_INSN_AND3; goto extract_fmt_mulo3;
              case 4 : itype = I960BASE_INSN_ANDNOT3; goto extract_fmt_mulo3;
              case 6 : itype = I960BASE_INSN_SETBIT3; goto extract_fmt_notbit3;
              case 8 : itype = I960BASE_INSN_NOTAND3; goto extract_fmt_mulo3;
              case 12 : itype = I960BASE_INSN_XOR3; goto extract_fmt_mulo3;
              case 14 : itype = I960BASE_INSN_OR3; goto extract_fmt_mulo3;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 7 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_NOR3; goto extract_fmt_mulo3;
              case 2 : itype = I960BASE_INSN_XNOR3; goto extract_fmt_mulo3;
              case 4 : itype = I960BASE_INSN_NOT3; goto extract_fmt_not3;
              case 6 : itype = I960BASE_INSN_ORNOT3; goto extract_fmt_mulo3;
              case 8 : itype = I960BASE_INSN_CLRBIT3; goto extract_fmt_notbit3;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 89 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_ADDO; goto extract_fmt_mulo;
              case 4 : itype = I960BASE_INSN_SUBO; goto extract_fmt_mulo;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 1 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_SHRO; goto extract_fmt_shlo;
              case 6 : itype = I960BASE_INSN_SHRI; goto extract_fmt_shlo;
              case 8 : itype = I960BASE_INSN_SHLO; goto extract_fmt_shlo;
              case 12 : itype = I960BASE_INSN_SHLI; goto extract_fmt_shlo;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 2 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_ADDO1; goto extract_fmt_mulo1;
              case 4 : itype = I960BASE_INSN_SUBO1; goto extract_fmt_mulo1;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 3 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_SHRO1; goto extract_fmt_shlo1;
              case 6 : itype = I960BASE_INSN_SHRI1; goto extract_fmt_shlo1;
              case 8 : itype = I960BASE_INSN_SHLO1; goto extract_fmt_shlo1;
              case 12 : itype = I960BASE_INSN_SHLI1; goto extract_fmt_shlo1;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 4 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_ADDO2; goto extract_fmt_mulo2;
              case 4 : itype = I960BASE_INSN_SUBO2; goto extract_fmt_mulo2;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 5 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_SHRO2; goto extract_fmt_shlo2;
              case 6 : itype = I960BASE_INSN_SHRI2; goto extract_fmt_shlo2;
              case 8 : itype = I960BASE_INSN_SHLO2; goto extract_fmt_shlo2;
              case 12 : itype = I960BASE_INSN_SHLI2; goto extract_fmt_shlo2;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 6 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_ADDO3; goto extract_fmt_mulo3;
              case 4 : itype = I960BASE_INSN_SUBO3; goto extract_fmt_mulo3;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 7 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_SHRO3; goto extract_fmt_shlo3;
              case 6 : itype = I960BASE_INSN_SHRI3; goto extract_fmt_shlo3;
              case 8 : itype = I960BASE_INSN_SHLO3; goto extract_fmt_shlo3;
              case 12 : itype = I960BASE_INSN_SHLI3; goto extract_fmt_shlo3;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 90 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 8 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_CMPO; goto extract_fmt_cmpo;
              case 2 : itype = I960BASE_INSN_CMPI; goto extract_fmt_cmpi;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 10 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_CMPO1; goto extract_fmt_cmpo1;
              case 2 : itype = I960BASE_INSN_CMPI1; goto extract_fmt_cmpi1;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 12 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_CMPO2; goto extract_fmt_cmpo2;
              case 2 : itype = I960BASE_INSN_CMPI2; goto extract_fmt_cmpi2;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 14 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_CMPO3; goto extract_fmt_cmpo3;
              case 2 : itype = I960BASE_INSN_CMPI3; goto extract_fmt_cmpi3;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 92 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 5 : itype = I960BASE_INSN_MOV; goto extract_fmt_not2;
          case 7 : itype = I960BASE_INSN_MOV1; goto extract_fmt_not3;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 93 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 5 : itype = I960BASE_INSN_MOVL; goto extract_fmt_movl;
          case 7 : itype = I960BASE_INSN_MOVL1; goto extract_fmt_movl1;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 94 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 5 : itype = I960BASE_INSN_MOVT; goto extract_fmt_movt;
          case 7 : itype = I960BASE_INSN_MOVT1; goto extract_fmt_movt1;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 95 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 5 : itype = I960BASE_INSN_MOVQ; goto extract_fmt_movq;
          case 7 : itype = I960BASE_INSN_MOVQ1; goto extract_fmt_movq1;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 100 : itype = I960BASE_INSN_MODAC; goto extract_fmt_modpc;
      case 101 : itype = I960BASE_INSN_MODPC; goto extract_fmt_modpc;
      case 102 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 12 : itype = I960BASE_INSN_CALLS; goto extract_fmt_calls;
          case 15 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 8 : itype = I960BASE_INSN_FMARK; goto extract_fmt_fmark;
              case 10 : itype = I960BASE_INSN_FLUSHREG; goto extract_fmt_flushreg;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 103 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_EMUL; goto extract_fmt_emul;
          case 2 : itype = I960BASE_INSN_EMUL1; goto extract_fmt_emul1;
          case 4 : itype = I960BASE_INSN_EMUL2; goto extract_fmt_emul2;
          case 6 : itype = I960BASE_INSN_EMUL3; goto extract_fmt_emul3;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 112 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : itype = I960BASE_INSN_MULO; goto extract_fmt_mulo;
          case 1 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMO; goto extract_fmt_mulo;
              case 6 : itype = I960BASE_INSN_DIVO; goto extract_fmt_mulo;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 2 : itype = I960BASE_INSN_MULO1; goto extract_fmt_mulo1;
          case 3 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMO1; goto extract_fmt_mulo1;
              case 6 : itype = I960BASE_INSN_DIVO1; goto extract_fmt_mulo1;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 4 : itype = I960BASE_INSN_MULO2; goto extract_fmt_mulo2;
          case 5 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMO2; goto extract_fmt_mulo2;
              case 6 : itype = I960BASE_INSN_DIVO2; goto extract_fmt_mulo2;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 6 : itype = I960BASE_INSN_MULO3; goto extract_fmt_mulo3;
          case 7 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMO3; goto extract_fmt_mulo3;
              case 6 : itype = I960BASE_INSN_DIVO3; goto extract_fmt_mulo3;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 116 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 1 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMI; goto extract_fmt_mulo;
              case 6 : itype = I960BASE_INSN_DIVI; goto extract_fmt_mulo;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 3 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMI1; goto extract_fmt_mulo1;
              case 6 : itype = I960BASE_INSN_DIVI1; goto extract_fmt_mulo1;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 5 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMI2; goto extract_fmt_mulo2;
              case 6 : itype = I960BASE_INSN_DIVI2; goto extract_fmt_mulo2;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          case 7 :
            {
              unsigned int val = (((insn >> 6) & (15 << 0)));
              switch (val)
              {
              case 0 : itype = I960BASE_INSN_REMI3; goto extract_fmt_mulo3;
              case 6 : itype = I960BASE_INSN_DIVI3; goto extract_fmt_mulo3;
              default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
              }
            }
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 128 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDOB_OFFSET; goto extract_fmt_ldob_offset;
          case 4 : itype = I960BASE_INSN_LDOB_INDIRECT; goto extract_fmt_ldob_indirect;
          case 7 : itype = I960BASE_INSN_LDOB_INDIRECT_INDEX; goto extract_fmt_ldob_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDOB_INDIRECT_OFFSET; goto extract_fmt_ldob_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDOB_DISP; goto extract_fmt_ldob_disp;
          case 13 : itype = I960BASE_INSN_LDOB_INDIRECT_DISP; goto extract_fmt_ldob_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDOB_INDEX_DISP; goto extract_fmt_ldob_index_disp;
          case 15 : itype = I960BASE_INSN_LDOB_INDIRECT_INDEX_DISP; goto extract_fmt_ldob_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 130 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_STOB_OFFSET; goto extract_fmt_stob_offset;
          case 4 : itype = I960BASE_INSN_STOB_INDIRECT; goto extract_fmt_stob_indirect;
          case 7 : itype = I960BASE_INSN_STOB_INDIRECT_INDEX; goto extract_fmt_stob_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_STOB_INDIRECT_OFFSET; goto extract_fmt_stob_indirect_offset;
          case 12 : itype = I960BASE_INSN_STOB_DISP; goto extract_fmt_stob_disp;
          case 13 : itype = I960BASE_INSN_STOB_INDIRECT_DISP; goto extract_fmt_stob_indirect_disp;
          case 14 : itype = I960BASE_INSN_STOB_INDEX_DISP; goto extract_fmt_stob_index_disp;
          case 15 : itype = I960BASE_INSN_STOB_INDIRECT_INDEX_DISP; goto extract_fmt_stob_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 132 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 4 : itype = I960BASE_INSN_BX_INDIRECT; goto extract_fmt_bx_indirect;
          case 7 : itype = I960BASE_INSN_BX_INDIRECT_INDEX; goto extract_fmt_bx_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_BX_INDIRECT_OFFSET; goto extract_fmt_bx_indirect_offset;
          case 12 : itype = I960BASE_INSN_BX_DISP; goto extract_fmt_bx_disp;
          case 13 : itype = I960BASE_INSN_BX_INDIRECT_DISP; goto extract_fmt_bx_indirect_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 134 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 4 : itype = I960BASE_INSN_CALLX_INDIRECT; goto extract_fmt_callx_indirect;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_CALLX_INDIRECT_OFFSET; goto extract_fmt_callx_indirect_offset;
          case 12 : itype = I960BASE_INSN_CALLX_DISP; goto extract_fmt_callx_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 136 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDOS_OFFSET; goto extract_fmt_ldos_offset;
          case 4 : itype = I960BASE_INSN_LDOS_INDIRECT; goto extract_fmt_ldos_indirect;
          case 7 : itype = I960BASE_INSN_LDOS_INDIRECT_INDEX; goto extract_fmt_ldos_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDOS_INDIRECT_OFFSET; goto extract_fmt_ldos_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDOS_DISP; goto extract_fmt_ldos_disp;
          case 13 : itype = I960BASE_INSN_LDOS_INDIRECT_DISP; goto extract_fmt_ldos_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDOS_INDEX_DISP; goto extract_fmt_ldos_index_disp;
          case 15 : itype = I960BASE_INSN_LDOS_INDIRECT_INDEX_DISP; goto extract_fmt_ldos_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 138 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_STOS_OFFSET; goto extract_fmt_stos_offset;
          case 4 : itype = I960BASE_INSN_STOS_INDIRECT; goto extract_fmt_stos_indirect;
          case 7 : itype = I960BASE_INSN_STOS_INDIRECT_INDEX; goto extract_fmt_stos_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_STOS_INDIRECT_OFFSET; goto extract_fmt_stos_indirect_offset;
          case 12 : itype = I960BASE_INSN_STOS_DISP; goto extract_fmt_stos_disp;
          case 13 : itype = I960BASE_INSN_STOS_INDIRECT_DISP; goto extract_fmt_stos_indirect_disp;
          case 14 : itype = I960BASE_INSN_STOS_INDEX_DISP; goto extract_fmt_stos_index_disp;
          case 15 : itype = I960BASE_INSN_STOS_INDIRECT_INDEX_DISP; goto extract_fmt_stos_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 140 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDA_OFFSET; goto extract_fmt_lda_offset;
          case 4 : itype = I960BASE_INSN_LDA_INDIRECT; goto extract_fmt_lda_indirect;
          case 7 : itype = I960BASE_INSN_LDA_INDIRECT_INDEX; goto extract_fmt_lda_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDA_INDIRECT_OFFSET; goto extract_fmt_lda_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDA_DISP; goto extract_fmt_lda_disp;
          case 13 : itype = I960BASE_INSN_LDA_INDIRECT_DISP; goto extract_fmt_lda_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDA_INDEX_DISP; goto extract_fmt_lda_index_disp;
          case 15 : itype = I960BASE_INSN_LDA_INDIRECT_INDEX_DISP; goto extract_fmt_lda_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 144 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LD_OFFSET; goto extract_fmt_ld_offset;
          case 4 : itype = I960BASE_INSN_LD_INDIRECT; goto extract_fmt_ld_indirect;
          case 7 : itype = I960BASE_INSN_LD_INDIRECT_INDEX; goto extract_fmt_ld_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LD_INDIRECT_OFFSET; goto extract_fmt_ld_indirect_offset;
          case 12 : itype = I960BASE_INSN_LD_DISP; goto extract_fmt_ld_disp;
          case 13 : itype = I960BASE_INSN_LD_INDIRECT_DISP; goto extract_fmt_ld_indirect_disp;
          case 14 : itype = I960BASE_INSN_LD_INDEX_DISP; goto extract_fmt_ld_index_disp;
          case 15 : itype = I960BASE_INSN_LD_INDIRECT_INDEX_DISP; goto extract_fmt_ld_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 146 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_ST_OFFSET; goto extract_fmt_st_offset;
          case 4 : itype = I960BASE_INSN_ST_INDIRECT; goto extract_fmt_st_indirect;
          case 7 : itype = I960BASE_INSN_ST_INDIRECT_INDEX; goto extract_fmt_st_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_ST_INDIRECT_OFFSET; goto extract_fmt_st_indirect_offset;
          case 12 : itype = I960BASE_INSN_ST_DISP; goto extract_fmt_st_disp;
          case 13 : itype = I960BASE_INSN_ST_INDIRECT_DISP; goto extract_fmt_st_indirect_disp;
          case 14 : itype = I960BASE_INSN_ST_INDEX_DISP; goto extract_fmt_st_index_disp;
          case 15 : itype = I960BASE_INSN_ST_INDIRECT_INDEX_DISP; goto extract_fmt_st_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 152 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDL_OFFSET; goto extract_fmt_ldl_offset;
          case 4 : itype = I960BASE_INSN_LDL_INDIRECT; goto extract_fmt_ldl_indirect;
          case 7 : itype = I960BASE_INSN_LDL_INDIRECT_INDEX; goto extract_fmt_ldl_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDL_INDIRECT_OFFSET; goto extract_fmt_ldl_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDL_DISP; goto extract_fmt_ldl_disp;
          case 13 : itype = I960BASE_INSN_LDL_INDIRECT_DISP; goto extract_fmt_ldl_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDL_INDEX_DISP; goto extract_fmt_ldl_index_disp;
          case 15 : itype = I960BASE_INSN_LDL_INDIRECT_INDEX_DISP; goto extract_fmt_ldl_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 154 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_STL_OFFSET; goto extract_fmt_stl_offset;
          case 4 : itype = I960BASE_INSN_STL_INDIRECT; goto extract_fmt_stl_indirect;
          case 7 : itype = I960BASE_INSN_STL_INDIRECT_INDEX; goto extract_fmt_stl_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_STL_INDIRECT_OFFSET; goto extract_fmt_stl_indirect_offset;
          case 12 : itype = I960BASE_INSN_STL_DISP; goto extract_fmt_stl_disp;
          case 13 : itype = I960BASE_INSN_STL_INDIRECT_DISP; goto extract_fmt_stl_indirect_disp;
          case 14 : itype = I960BASE_INSN_STL_INDEX_DISP; goto extract_fmt_stl_index_disp;
          case 15 : itype = I960BASE_INSN_STL_INDIRECT_INDEX_DISP; goto extract_fmt_stl_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 160 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDT_OFFSET; goto extract_fmt_ldt_offset;
          case 4 : itype = I960BASE_INSN_LDT_INDIRECT; goto extract_fmt_ldt_indirect;
          case 7 : itype = I960BASE_INSN_LDT_INDIRECT_INDEX; goto extract_fmt_ldt_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDT_INDIRECT_OFFSET; goto extract_fmt_ldt_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDT_DISP; goto extract_fmt_ldt_disp;
          case 13 : itype = I960BASE_INSN_LDT_INDIRECT_DISP; goto extract_fmt_ldt_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDT_INDEX_DISP; goto extract_fmt_ldt_index_disp;
          case 15 : itype = I960BASE_INSN_LDT_INDIRECT_INDEX_DISP; goto extract_fmt_ldt_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 162 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_STT_OFFSET; goto extract_fmt_stt_offset;
          case 4 : itype = I960BASE_INSN_STT_INDIRECT; goto extract_fmt_stt_indirect;
          case 7 : itype = I960BASE_INSN_STT_INDIRECT_INDEX; goto extract_fmt_stt_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_STT_INDIRECT_OFFSET; goto extract_fmt_stt_indirect_offset;
          case 12 : itype = I960BASE_INSN_STT_DISP; goto extract_fmt_stt_disp;
          case 13 : itype = I960BASE_INSN_STT_INDIRECT_DISP; goto extract_fmt_stt_indirect_disp;
          case 14 : itype = I960BASE_INSN_STT_INDEX_DISP; goto extract_fmt_stt_index_disp;
          case 15 : itype = I960BASE_INSN_STT_INDIRECT_INDEX_DISP; goto extract_fmt_stt_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 176 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDQ_OFFSET; goto extract_fmt_ldq_offset;
          case 4 : itype = I960BASE_INSN_LDQ_INDIRECT; goto extract_fmt_ldq_indirect;
          case 7 : itype = I960BASE_INSN_LDQ_INDIRECT_INDEX; goto extract_fmt_ldq_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDQ_INDIRECT_OFFSET; goto extract_fmt_ldq_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDQ_DISP; goto extract_fmt_ldq_disp;
          case 13 : itype = I960BASE_INSN_LDQ_INDIRECT_DISP; goto extract_fmt_ldq_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDQ_INDEX_DISP; goto extract_fmt_ldq_index_disp;
          case 15 : itype = I960BASE_INSN_LDQ_INDIRECT_INDEX_DISP; goto extract_fmt_ldq_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 178 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_STQ_OFFSET; goto extract_fmt_stq_offset;
          case 4 : itype = I960BASE_INSN_STQ_INDIRECT; goto extract_fmt_stq_indirect;
          case 7 : itype = I960BASE_INSN_STQ_INDIRECT_INDEX; goto extract_fmt_stq_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_STQ_INDIRECT_OFFSET; goto extract_fmt_stq_indirect_offset;
          case 12 : itype = I960BASE_INSN_STQ_DISP; goto extract_fmt_stq_disp;
          case 13 : itype = I960BASE_INSN_STQ_INDIRECT_DISP; goto extract_fmt_stq_indirect_disp;
          case 14 : itype = I960BASE_INSN_STQ_INDEX_DISP; goto extract_fmt_stq_index_disp;
          case 15 : itype = I960BASE_INSN_STQ_INDIRECT_INDEX_DISP; goto extract_fmt_stq_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 192 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDIB_OFFSET; goto extract_fmt_ldib_offset;
          case 4 : itype = I960BASE_INSN_LDIB_INDIRECT; goto extract_fmt_ldib_indirect;
          case 7 : itype = I960BASE_INSN_LDIB_INDIRECT_INDEX; goto extract_fmt_ldib_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDIB_INDIRECT_OFFSET; goto extract_fmt_ldib_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDIB_DISP; goto extract_fmt_ldib_disp;
          case 13 : itype = I960BASE_INSN_LDIB_INDIRECT_DISP; goto extract_fmt_ldib_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDIB_INDEX_DISP; goto extract_fmt_ldib_index_disp;
          case 15 : itype = I960BASE_INSN_LDIB_INDIRECT_INDEX_DISP; goto extract_fmt_ldib_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      case 200 :
        {
          unsigned int val = (((insn >> 10) & (15 << 0)));
          switch (val)
          {
          case 0 : /* fall through */
          case 1 : /* fall through */
          case 2 : /* fall through */
          case 3 : itype = I960BASE_INSN_LDIS_OFFSET; goto extract_fmt_ldis_offset;
          case 4 : itype = I960BASE_INSN_LDIS_INDIRECT; goto extract_fmt_ldis_indirect;
          case 7 : itype = I960BASE_INSN_LDIS_INDIRECT_INDEX; goto extract_fmt_ldis_indirect_index;
          case 8 : /* fall through */
          case 9 : /* fall through */
          case 10 : /* fall through */
          case 11 : itype = I960BASE_INSN_LDIS_INDIRECT_OFFSET; goto extract_fmt_ldis_indirect_offset;
          case 12 : itype = I960BASE_INSN_LDIS_DISP; goto extract_fmt_ldis_disp;
          case 13 : itype = I960BASE_INSN_LDIS_INDIRECT_DISP; goto extract_fmt_ldis_indirect_disp;
          case 14 : itype = I960BASE_INSN_LDIS_INDEX_DISP; goto extract_fmt_ldis_index_disp;
          case 15 : itype = I960BASE_INSN_LDIS_INDIRECT_INDEX_DISP; goto extract_fmt_ldis_indirect_index_disp;
          default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
          }
        }
      default : itype = I960BASE_INSN_X_INVALID; goto extract_fmt_empty;
      }
    }
  }

  /* The instruction has been decoded, now extract the fields.  */

 extract_fmt_empty:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_empty.f
  EXTRACT_IFMT_EMPTY_VARS /* */

  EXTRACT_IFMT_EMPTY_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_empty", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_fmt_mulo:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_mulo1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_mulo2:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_mulo3:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_notbit:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_notbit1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_notbit2:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_notbit3:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_not:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_not1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_not2:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_not3:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_shlo:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_shlo.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_shlo", "src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

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
    return idesc;
  }

 extract_fmt_shlo1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_shlo1.f
  EXTRACT_IFMT_MULO1_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO1_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (i_src2) = & CPU (h_gr)[f_src2];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_shlo1", "f_src1 0x%x", 'x', f_src1, "src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src2) = f_src2;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_shlo2:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_shlo2.f
  EXTRACT_IFMT_MULO2_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO2_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src2) = f_src2;
  FLD (i_src1) = & CPU (h_gr)[f_src1];
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_shlo2", "f_src2 0x%x", 'x', f_src2, "src1 0x%x", 'x', f_src1, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_shlo3:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_shlo3.f
  EXTRACT_IFMT_MULO3_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO3_CODE

  /* Record the fields for the semantic handler.  */
  FLD (f_src1) = f_src1;
  FLD (f_src2) = f_src2;
  FLD (i_dst) = & CPU (h_gr)[f_srcdst];
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_shlo3", "f_src1 0x%x", 'x', f_src1, "f_src2 0x%x", 'x', f_src2, "dst 0x%x", 'x', f_srcdst, (char *) 0));

#if WITH_PROFILE_MODEL_P
  /* Record the fields for profiling.  */
  if (PROFILE_MODEL_P (current_cpu))
    {
      FLD (out_dst) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_emul:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_emul1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_emul2:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_emul3:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movl:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_src1_1) = ((FLD (f_src1)) + (1));
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movl1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movt:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_src1_1) = ((FLD (f_src1)) + (1));
      FLD (in_h_gr_add__VM_index_of_src1_2) = ((FLD (f_src1)) + (2));
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movt1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movq:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_src1_1) = ((FLD (f_src1)) + (1));
      FLD (in_h_gr_add__VM_index_of_src1_2) = ((FLD (f_src1)) + (2));
      FLD (in_h_gr_add__VM_index_of_src1_3) = ((FLD (f_src1)) + (3));
      FLD (in_src1) = f_src1;
      FLD (out_dst) = f_srcdst;
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_movq1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_modpc:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_lda_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ld_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldob_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldos_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldib_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldis_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ldl_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldl_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldl_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldl_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldl_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldl_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldl_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldl_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldt_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_ldq_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (out_h_gr_add__VM_index_of_dst_1) = ((FLD (f_srcdst)) + (1));
      FLD (out_h_gr_add__VM_index_of_dst_2) = ((FLD (f_srcdst)) + (2));
      FLD (out_h_gr_add__VM_index_of_dst_3) = ((FLD (f_srcdst)) + (3));
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_st_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_st_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stob_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stos_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_stl_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stl_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stl_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stl_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stl_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stl_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stl_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stl_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stt_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_stq_indirect_index_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
      FLD (in_h_gr_add__VM_index_of_st_src_1) = ((FLD (f_srcdst)) + (1));
      FLD (in_h_gr_add__VM_index_of_st_src_2) = ((FLD (f_srcdst)) + (2));
      FLD (in_h_gr_add__VM_index_of_st_src_3) = ((FLD (f_srcdst)) + (3));
      FLD (in_index) = f_index;
      FLD (in_st_src) = f_srcdst;
    }
#endif
#undef FLD
    return idesc;
  }

 extract_fmt_cmpobe_reg:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpobe_lit:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpobl_reg:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpobl_lit:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bbc_reg:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bbc_lit:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpi:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpi1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpi2:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpi3:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpo:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpo1:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpo2:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_cmpo3:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_testno_reg:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bno:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_b:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bx_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bx_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bx_indirect_index:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bx_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_bx_indirect_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_callx_disp:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_callx_indirect:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_callx_indirect_offset:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_ret:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_calls:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_fmark:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
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
    return idesc;
  }

 extract_fmt_flushreg:
  {
    const IDESC *idesc = &i960base_insn_data[itype];
    CGEN_INSN_INT insn = base_insn;
#define FLD(f) abuf->fields.fmt_flushreg.f
  EXTRACT_IFMT_MULO_VARS /* f-opcode f-srcdst f-src2 f-m3 f-m2 f-m1 f-opcode2 f-zero f-src1 */

  EXTRACT_IFMT_MULO_CODE

  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "fmt_flushreg", (char *) 0));

#undef FLD
    return idesc;
  }

}
