/* ISA definitions header for compact.

THIS FILE IS MACHINE GENERATED WITH CGEN.

Copyright (C) 1996, 1997, 1998, 1999, 2000 Free Software Foundation, Inc.

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

#ifndef DEFS_SH64_COMPACT_H
#define DEFS_SH64_COMPACT_H

/* Instruction argument buffer.  */

union sem_fields {
  struct { /* no operands */
    int empty;
  } fmt_empty;
  struct { /*  */
    SI f_dn;
  } sfmt_fcnvds_compact;
  struct { /*  */
    IADDR i_disp12;
  } sfmt_bra_compact;
  struct { /*  */
    IADDR i_disp8;
  } sfmt_bf_compact;
  struct { /*  */
    SI f_imm4x2;
    UINT f_rm;
  } sfmt_movw11_compact;
  struct { /*  */
    SI f_imm8x2;
    UINT f_rn;
  } sfmt_movw10_compact;
  struct { /*  */
    SI f_imm4x2;
    UINT f_rn;
  } sfmt_movw5_compact;
  struct { /*  */
    SI f_imm8x4;
    UINT f_rn;
  } sfmt_movl10_compact;
  struct { /*  */
    UINT f_imm4;
    UINT f_rm;
  } sfmt_movb5_compact;
  struct { /*  */
    SI f_vm;
    SI f_vn;
  } sfmt_fipr_compact;
  struct { /*  */
    UINT f_imm8;
    UINT f_rn;
  } sfmt_addi_compact;
  struct { /*  */
    SI f_imm4x4;
    UINT f_rm;
    UINT f_rn;
  } sfmt_movl5_compact;
#if WITH_SCACHE_PBB
  /* Writeback handler.  */
  struct {
    /* Pointer to argbuf entry for insn whose results need writing back.  */
    const struct argbuf *abuf;
  } write;
  /* x-before handler */
  struct {
    /*const SCACHE *insns[MAX_PARALLEL_INSNS];*/
    int first_p;
  } before;
  /* x-after handler */
  struct {
    int empty;
  } after;
  /* This entry is used to terminate each pbb.  */
  struct {
    /* Number of insns in pbb.  */
    int insn_count;
    /* Next pbb to execute.  */
    SCACHE *next;
    SCACHE *branch_target;
  } chain;
#endif
};

/* The ARGBUF struct.  */
struct argbuf {
  /* These are the baseclass definitions.  */
  IADDR addr;
  const IDESC *idesc;
  char trace_p;
  char profile_p;
  /* ??? Temporary hack for skip insns.  */
  char skip_count;
  char unused;
  /* cpu specific data follows */
  union sem semantic;
  int written;
  union sem_fields fields;
};

/* A cached insn.

   ??? SCACHE used to contain more than just argbuf.  We could delete the
   type entirely and always just use ARGBUF, but for future concerns and as
   a level of abstraction it is left in.  */

struct scache {
  struct argbuf argbuf;
};

/* Macros to simplify extraction, reading and semantic code.
   These define and assign the local vars that contain the insn's fields.  */

#define EXTRACT_IFMT_EMPTY_VARS \
  unsigned int length;
#define EXTRACT_IFMT_EMPTY_CODE \
  length = 0; \

#define EXTRACT_IFMT_ADD_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_rm; \
  UINT f_sub4; \
  unsigned int length;
#define EXTRACT_IFMT_ADD_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_sub4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4); \

#define EXTRACT_IFMT_ADDI_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_imm8; \
  unsigned int length;
#define EXTRACT_IFMT_ADDI_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_AND_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_rm; \
  UINT f_sub4; \
  unsigned int length;
#define EXTRACT_IFMT_AND_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_sub4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4); \

#define EXTRACT_IFMT_ANDI_COMPACT_VARS \
  UINT f_op8; \
  UINT f_imm8; \
  unsigned int length;
#define EXTRACT_IFMT_ANDI_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_ANDB_COMPACT_VARS \
  UINT f_op8; \
  UINT f_imm8; \
  unsigned int length;
#define EXTRACT_IFMT_ANDB_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_imm8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_BF_COMPACT_VARS \
  UINT f_op8; \
  SI f_disp8; \
  unsigned int length;
#define EXTRACT_IFMT_BF_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_disp8 = ((((EXTRACT_LSB0_INT (insn, 16, 7, 8)) << (1))) + (((pc) + (4)))); \

#define EXTRACT_IFMT_BRA_COMPACT_VARS \
  UINT f_op4; \
  SI f_disp12; \
  unsigned int length;
#define EXTRACT_IFMT_BRA_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_disp12 = ((((EXTRACT_LSB0_INT (insn, 16, 11, 12)) << (1))) + (((pc) + (4)))); \

#define EXTRACT_IFMT_BRAF_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_sub8; \
  unsigned int length;
#define EXTRACT_IFMT_BRAF_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_sub8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_BRK_COMPACT_VARS \
  UINT f_op16; \
  unsigned int length;
#define EXTRACT_IFMT_BRK_COMPACT_CODE \
  length = 2; \
  f_op16 = EXTRACT_LSB0_UINT (insn, 16, 15, 16); \

#define EXTRACT_IFMT_FABS_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_sub8; \
  unsigned int length;
#define EXTRACT_IFMT_FABS_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_sub8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_FADD_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_rm; \
  UINT f_sub4; \
  unsigned int length;
#define EXTRACT_IFMT_FADD_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_sub4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4); \

#define EXTRACT_IFMT_FCNVDS_COMPACT_VARS \
  UINT f_op4; \
  SI f_dn; \
  UINT f_8_1; \
  UINT f_sub8; \
  unsigned int length;
#define EXTRACT_IFMT_FCNVDS_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_dn = ((EXTRACT_LSB0_UINT (insn, 16, 11, 3)) << (1)); \
  f_8_1 = EXTRACT_LSB0_UINT (insn, 16, 8, 1); \
  f_sub8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_FIPR_COMPACT_VARS \
  UINT f_op4; \
  SI f_vn; \
  SI f_vm; \
  UINT f_sub8; \
  unsigned int length;
#define EXTRACT_IFMT_FIPR_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_vn = ((EXTRACT_LSB0_UINT (insn, 16, 11, 2)) << (2)); \
  f_vm = ((EXTRACT_LSB0_UINT (insn, 16, 9, 2)) << (2)); \
  f_sub8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_FLDS_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_sub8; \
  unsigned int length;
#define EXTRACT_IFMT_FLDS_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_sub8 = EXTRACT_LSB0_UINT (insn, 16, 7, 8); \

#define EXTRACT_IFMT_FMAC_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_rm; \
  UINT f_sub4; \
  unsigned int length;
#define EXTRACT_IFMT_FMAC_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_sub4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4); \

#define EXTRACT_IFMT_FMOV2_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_rm; \
  UINT f_sub4; \
  unsigned int length;
#define EXTRACT_IFMT_FMOV2_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_sub4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4); \

#define EXTRACT_IFMT_FMOV5_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_rm; \
  UINT f_sub4; \
  unsigned int length;
#define EXTRACT_IFMT_FMOV5_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_sub4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4); \

#define EXTRACT_IFMT_FTRV_COMPACT_VARS \
  UINT f_op4; \
  SI f_vn; \
  UINT f_sub10; \
  unsigned int length;
#define EXTRACT_IFMT_FTRV_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_vn = ((EXTRACT_LSB0_UINT (insn, 16, 11, 2)) << (2)); \
  f_sub10 = EXTRACT_LSB0_UINT (insn, 16, 9, 10); \

#define EXTRACT_IFMT_MOVB5_COMPACT_VARS \
  UINT f_op8; \
  UINT f_rm; \
  UINT f_imm4; \
  unsigned int length;
#define EXTRACT_IFMT_MOVB5_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_imm4 = EXTRACT_LSB0_UINT (insn, 16, 3, 4); \

#define EXTRACT_IFMT_MOVL4_COMPACT_VARS \
  UINT f_op8; \
  SI f_imm8x4; \
  unsigned int length;
#define EXTRACT_IFMT_MOVL4_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_imm8x4 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (2)); \

#define EXTRACT_IFMT_MOVL5_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  UINT f_rm; \
  SI f_imm4x4; \
  unsigned int length;
#define EXTRACT_IFMT_MOVL5_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_imm4x4 = ((EXTRACT_LSB0_UINT (insn, 16, 3, 4)) << (2)); \

#define EXTRACT_IFMT_MOVL10_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  SI f_imm8x4; \
  unsigned int length;
#define EXTRACT_IFMT_MOVL10_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_imm8x4 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (2)); \

#define EXTRACT_IFMT_MOVW4_COMPACT_VARS \
  UINT f_op8; \
  SI f_imm8x2; \
  unsigned int length;
#define EXTRACT_IFMT_MOVW4_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_imm8x2 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (1)); \

#define EXTRACT_IFMT_MOVW5_COMPACT_VARS \
  UINT f_op8; \
  UINT f_rn; \
  SI f_imm4x2; \
  unsigned int length;
#define EXTRACT_IFMT_MOVW5_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_imm4x2 = ((EXTRACT_LSB0_UINT (insn, 16, 3, 4)) << (1)); \

#define EXTRACT_IFMT_MOVW10_COMPACT_VARS \
  UINT f_op4; \
  UINT f_rn; \
  SI f_imm8x2; \
  unsigned int length;
#define EXTRACT_IFMT_MOVW10_COMPACT_CODE \
  length = 2; \
  f_op4 = EXTRACT_LSB0_UINT (insn, 16, 15, 4); \
  f_rn = EXTRACT_LSB0_UINT (insn, 16, 11, 4); \
  f_imm8x2 = ((EXTRACT_LSB0_UINT (insn, 16, 7, 8)) << (1)); \

#define EXTRACT_IFMT_MOVW11_COMPACT_VARS \
  UINT f_op8; \
  UINT f_rm; \
  SI f_imm4x2; \
  unsigned int length;
#define EXTRACT_IFMT_MOVW11_COMPACT_CODE \
  length = 2; \
  f_op8 = EXTRACT_LSB0_UINT (insn, 16, 15, 8); \
  f_rm = EXTRACT_LSB0_UINT (insn, 16, 7, 4); \
  f_imm4x2 = ((EXTRACT_LSB0_UINT (insn, 16, 3, 4)) << (1)); \

#endif /* DEFS_SH64_COMPACT_H */
