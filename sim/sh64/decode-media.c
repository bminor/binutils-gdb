/* Simulator instruction decoder for sh64_media.

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

#define WANT_CPU sh64
#define WANT_CPU_SH64

#include "sim-main.h"
#include "sim-assert.h"

/* The instruction descriptor array.
   This is computed at runtime.  Space for it is not malloc'd to save a
   teensy bit of cpu in the decoder.  Moving it to malloc space is trivial
   but won't be done until necessary (we don't currently support the runtime
   addition of instructions nor an SMP machine with different cpus).  */
static IDESC sh64_media_insn_data[SH64_MEDIA_INSN_MAX];

/* Commas between elements are contained in the macros.
   Some of these are conditionally compiled out.  */

static const struct insn_sem sh64_media_insn_sem[] =
{
  { VIRTUAL_INSN_X_INVALID, SH64_MEDIA_INSN_X_INVALID, SH64_MEDIA_SFMT_EMPTY },
  { VIRTUAL_INSN_X_AFTER, SH64_MEDIA_INSN_X_AFTER, SH64_MEDIA_SFMT_EMPTY },
  { VIRTUAL_INSN_X_BEFORE, SH64_MEDIA_INSN_X_BEFORE, SH64_MEDIA_SFMT_EMPTY },
  { VIRTUAL_INSN_X_CTI_CHAIN, SH64_MEDIA_INSN_X_CTI_CHAIN, SH64_MEDIA_SFMT_EMPTY },
  { VIRTUAL_INSN_X_CHAIN, SH64_MEDIA_INSN_X_CHAIN, SH64_MEDIA_SFMT_EMPTY },
  { VIRTUAL_INSN_X_BEGIN, SH64_MEDIA_INSN_X_BEGIN, SH64_MEDIA_SFMT_EMPTY },
  { SH_INSN_ADD, SH64_MEDIA_INSN_ADD, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_ADDL, SH64_MEDIA_INSN_ADDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_ADDI, SH64_MEDIA_INSN_ADDI, SH64_MEDIA_SFMT_ADDI },
  { SH_INSN_ADDIL, SH64_MEDIA_INSN_ADDIL, SH64_MEDIA_SFMT_ADDI },
  { SH_INSN_ADDZL, SH64_MEDIA_INSN_ADDZL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_ALLOCO, SH64_MEDIA_INSN_ALLOCO, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_AND, SH64_MEDIA_INSN_AND, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_ANDC, SH64_MEDIA_INSN_ANDC, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_ANDI, SH64_MEDIA_INSN_ANDI, SH64_MEDIA_SFMT_ADDI },
  { SH_INSN_BEQ, SH64_MEDIA_INSN_BEQ, SH64_MEDIA_SFMT_BEQ },
  { SH_INSN_BEQI, SH64_MEDIA_INSN_BEQI, SH64_MEDIA_SFMT_BEQI },
  { SH_INSN_BGE, SH64_MEDIA_INSN_BGE, SH64_MEDIA_SFMT_BEQ },
  { SH_INSN_BGEU, SH64_MEDIA_INSN_BGEU, SH64_MEDIA_SFMT_BEQ },
  { SH_INSN_BGT, SH64_MEDIA_INSN_BGT, SH64_MEDIA_SFMT_BEQ },
  { SH_INSN_BGTU, SH64_MEDIA_INSN_BGTU, SH64_MEDIA_SFMT_BEQ },
  { SH_INSN_BLINK, SH64_MEDIA_INSN_BLINK, SH64_MEDIA_SFMT_BLINK },
  { SH_INSN_BNE, SH64_MEDIA_INSN_BNE, SH64_MEDIA_SFMT_BEQ },
  { SH_INSN_BNEI, SH64_MEDIA_INSN_BNEI, SH64_MEDIA_SFMT_BEQI },
  { SH_INSN_BRK, SH64_MEDIA_INSN_BRK, SH64_MEDIA_SFMT_BRK },
  { SH_INSN_BYTEREV, SH64_MEDIA_INSN_BYTEREV, SH64_MEDIA_SFMT_BYTEREV },
  { SH_INSN_CMPEQ, SH64_MEDIA_INSN_CMPEQ, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_CMPGT, SH64_MEDIA_INSN_CMPGT, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_CMPGTU, SH64_MEDIA_INSN_CMPGTU, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_CMVEQ, SH64_MEDIA_INSN_CMVEQ, SH64_MEDIA_SFMT_CMVEQ },
  { SH_INSN_CMVNE, SH64_MEDIA_INSN_CMVNE, SH64_MEDIA_SFMT_CMVEQ },
  { SH_INSN_FABSD, SH64_MEDIA_INSN_FABSD, SH64_MEDIA_SFMT_FABSD },
  { SH_INSN_FABSS, SH64_MEDIA_INSN_FABSS, SH64_MEDIA_SFMT_FABSS },
  { SH_INSN_FADDD, SH64_MEDIA_INSN_FADDD, SH64_MEDIA_SFMT_FADDD },
  { SH_INSN_FADDS, SH64_MEDIA_INSN_FADDS, SH64_MEDIA_SFMT_FADDS },
  { SH_INSN_FCMPEQD, SH64_MEDIA_INSN_FCMPEQD, SH64_MEDIA_SFMT_FCMPEQD },
  { SH_INSN_FCMPEQS, SH64_MEDIA_INSN_FCMPEQS, SH64_MEDIA_SFMT_FCMPEQS },
  { SH_INSN_FCMPGED, SH64_MEDIA_INSN_FCMPGED, SH64_MEDIA_SFMT_FCMPEQD },
  { SH_INSN_FCMPGES, SH64_MEDIA_INSN_FCMPGES, SH64_MEDIA_SFMT_FCMPEQS },
  { SH_INSN_FCMPGTD, SH64_MEDIA_INSN_FCMPGTD, SH64_MEDIA_SFMT_FCMPEQD },
  { SH_INSN_FCMPGTS, SH64_MEDIA_INSN_FCMPGTS, SH64_MEDIA_SFMT_FCMPEQS },
  { SH_INSN_FCMPUND, SH64_MEDIA_INSN_FCMPUND, SH64_MEDIA_SFMT_FCMPEQD },
  { SH_INSN_FCMPUNS, SH64_MEDIA_INSN_FCMPUNS, SH64_MEDIA_SFMT_FCMPEQS },
  { SH_INSN_FCNVDS, SH64_MEDIA_INSN_FCNVDS, SH64_MEDIA_SFMT_FCNVDS },
  { SH_INSN_FCNVSD, SH64_MEDIA_INSN_FCNVSD, SH64_MEDIA_SFMT_FCNVSD },
  { SH_INSN_FDIVD, SH64_MEDIA_INSN_FDIVD, SH64_MEDIA_SFMT_FADDD },
  { SH_INSN_FDIVS, SH64_MEDIA_INSN_FDIVS, SH64_MEDIA_SFMT_FADDS },
  { SH_INSN_FGETSCR, SH64_MEDIA_INSN_FGETSCR, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_FIPRS, SH64_MEDIA_INSN_FIPRS, SH64_MEDIA_SFMT_FIPRS },
  { SH_INSN_FLDD, SH64_MEDIA_INSN_FLDD, SH64_MEDIA_SFMT_FLDD },
  { SH_INSN_FLDP, SH64_MEDIA_INSN_FLDP, SH64_MEDIA_SFMT_FLDP },
  { SH_INSN_FLDS, SH64_MEDIA_INSN_FLDS, SH64_MEDIA_SFMT_FLDS },
  { SH_INSN_FLDXD, SH64_MEDIA_INSN_FLDXD, SH64_MEDIA_SFMT_FLDXD },
  { SH_INSN_FLDXP, SH64_MEDIA_INSN_FLDXP, SH64_MEDIA_SFMT_FLDXP },
  { SH_INSN_FLDXS, SH64_MEDIA_INSN_FLDXS, SH64_MEDIA_SFMT_FLDXS },
  { SH_INSN_FLOATLD, SH64_MEDIA_INSN_FLOATLD, SH64_MEDIA_SFMT_FCNVSD },
  { SH_INSN_FLOATLS, SH64_MEDIA_INSN_FLOATLS, SH64_MEDIA_SFMT_FABSS },
  { SH_INSN_FLOATQD, SH64_MEDIA_INSN_FLOATQD, SH64_MEDIA_SFMT_FABSD },
  { SH_INSN_FLOATQS, SH64_MEDIA_INSN_FLOATQS, SH64_MEDIA_SFMT_FCNVDS },
  { SH_INSN_FMACS, SH64_MEDIA_INSN_FMACS, SH64_MEDIA_SFMT_FMACS },
  { SH_INSN_FMOVD, SH64_MEDIA_INSN_FMOVD, SH64_MEDIA_SFMT_FABSD },
  { SH_INSN_FMOVDQ, SH64_MEDIA_INSN_FMOVDQ, SH64_MEDIA_SFMT_FMOVDQ },
  { SH_INSN_FMOVLS, SH64_MEDIA_INSN_FMOVLS, SH64_MEDIA_SFMT_FMOVLS },
  { SH_INSN_FMOVQD, SH64_MEDIA_INSN_FMOVQD, SH64_MEDIA_SFMT_FMOVQD },
  { SH_INSN_FMOVS, SH64_MEDIA_INSN_FMOVS, SH64_MEDIA_SFMT_FABSS },
  { SH_INSN_FMOVSL, SH64_MEDIA_INSN_FMOVSL, SH64_MEDIA_SFMT_FMOVSL },
  { SH_INSN_FMULD, SH64_MEDIA_INSN_FMULD, SH64_MEDIA_SFMT_FADDD },
  { SH_INSN_FMULS, SH64_MEDIA_INSN_FMULS, SH64_MEDIA_SFMT_FADDS },
  { SH_INSN_FNEGD, SH64_MEDIA_INSN_FNEGD, SH64_MEDIA_SFMT_FABSD },
  { SH_INSN_FNEGS, SH64_MEDIA_INSN_FNEGS, SH64_MEDIA_SFMT_FABSS },
  { SH_INSN_FPUTSCR, SH64_MEDIA_INSN_FPUTSCR, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_FSQRTD, SH64_MEDIA_INSN_FSQRTD, SH64_MEDIA_SFMT_FABSD },
  { SH_INSN_FSQRTS, SH64_MEDIA_INSN_FSQRTS, SH64_MEDIA_SFMT_FABSS },
  { SH_INSN_FSTD, SH64_MEDIA_INSN_FSTD, SH64_MEDIA_SFMT_FSTD },
  { SH_INSN_FSTP, SH64_MEDIA_INSN_FSTP, SH64_MEDIA_SFMT_FSTP },
  { SH_INSN_FSTS, SH64_MEDIA_INSN_FSTS, SH64_MEDIA_SFMT_FSTS },
  { SH_INSN_FSTXD, SH64_MEDIA_INSN_FSTXD, SH64_MEDIA_SFMT_FSTXD },
  { SH_INSN_FSTXP, SH64_MEDIA_INSN_FSTXP, SH64_MEDIA_SFMT_FSTXP },
  { SH_INSN_FSTXS, SH64_MEDIA_INSN_FSTXS, SH64_MEDIA_SFMT_FSTXS },
  { SH_INSN_FSUBD, SH64_MEDIA_INSN_FSUBD, SH64_MEDIA_SFMT_FADDD },
  { SH_INSN_FSUBS, SH64_MEDIA_INSN_FSUBS, SH64_MEDIA_SFMT_FADDS },
  { SH_INSN_FTRCDL, SH64_MEDIA_INSN_FTRCDL, SH64_MEDIA_SFMT_FCNVDS },
  { SH_INSN_FTRCSL, SH64_MEDIA_INSN_FTRCSL, SH64_MEDIA_SFMT_FABSS },
  { SH_INSN_FTRCDQ, SH64_MEDIA_INSN_FTRCDQ, SH64_MEDIA_SFMT_FABSD },
  { SH_INSN_FTRCSQ, SH64_MEDIA_INSN_FTRCSQ, SH64_MEDIA_SFMT_FCNVSD },
  { SH_INSN_FTRVS, SH64_MEDIA_INSN_FTRVS, SH64_MEDIA_SFMT_FTRVS },
  { SH_INSN_GETCFG, SH64_MEDIA_INSN_GETCFG, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_GETCON, SH64_MEDIA_INSN_GETCON, SH64_MEDIA_SFMT_GETCON },
  { SH_INSN_GETTR, SH64_MEDIA_INSN_GETTR, SH64_MEDIA_SFMT_GETTR },
  { SH_INSN_ICBI, SH64_MEDIA_INSN_ICBI, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_LDB, SH64_MEDIA_INSN_LDB, SH64_MEDIA_SFMT_LDB },
  { SH_INSN_LDL, SH64_MEDIA_INSN_LDL, SH64_MEDIA_SFMT_LDL },
  { SH_INSN_LDQ, SH64_MEDIA_INSN_LDQ, SH64_MEDIA_SFMT_LDQ },
  { SH_INSN_LDUB, SH64_MEDIA_INSN_LDUB, SH64_MEDIA_SFMT_LDB },
  { SH_INSN_LDUW, SH64_MEDIA_INSN_LDUW, SH64_MEDIA_SFMT_LDUW },
  { SH_INSN_LDW, SH64_MEDIA_INSN_LDW, SH64_MEDIA_SFMT_LDUW },
  { SH_INSN_LDHIL, SH64_MEDIA_INSN_LDHIL, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_LDHIQ, SH64_MEDIA_INSN_LDHIQ, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_LDLOL, SH64_MEDIA_INSN_LDLOL, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_LDLOQ, SH64_MEDIA_INSN_LDLOQ, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_LDXB, SH64_MEDIA_INSN_LDXB, SH64_MEDIA_SFMT_LDXB },
  { SH_INSN_LDXL, SH64_MEDIA_INSN_LDXL, SH64_MEDIA_SFMT_LDXB },
  { SH_INSN_LDXQ, SH64_MEDIA_INSN_LDXQ, SH64_MEDIA_SFMT_LDXB },
  { SH_INSN_LDXUB, SH64_MEDIA_INSN_LDXUB, SH64_MEDIA_SFMT_LDXB },
  { SH_INSN_LDXUW, SH64_MEDIA_INSN_LDXUW, SH64_MEDIA_SFMT_LDXB },
  { SH_INSN_LDXW, SH64_MEDIA_INSN_LDXW, SH64_MEDIA_SFMT_LDXB },
  { SH_INSN_MABSL, SH64_MEDIA_INSN_MABSL, SH64_MEDIA_SFMT_BYTEREV },
  { SH_INSN_MABSW, SH64_MEDIA_INSN_MABSW, SH64_MEDIA_SFMT_BYTEREV },
  { SH_INSN_MADDL, SH64_MEDIA_INSN_MADDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MADDW, SH64_MEDIA_INSN_MADDW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MADDSL, SH64_MEDIA_INSN_MADDSL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MADDSUB, SH64_MEDIA_INSN_MADDSUB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MADDSW, SH64_MEDIA_INSN_MADDSW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCMPEQB, SH64_MEDIA_INSN_MCMPEQB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCMPEQL, SH64_MEDIA_INSN_MCMPEQL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCMPEQW, SH64_MEDIA_INSN_MCMPEQW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCMPGTL, SH64_MEDIA_INSN_MCMPGTL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCMPGTUB, SH64_MEDIA_INSN_MCMPGTUB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCMPGTW, SH64_MEDIA_INSN_MCMPGTW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCMV, SH64_MEDIA_INSN_MCMV, SH64_MEDIA_SFMT_MCMV },
  { SH_INSN_MCNVSLW, SH64_MEDIA_INSN_MCNVSLW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCNVSWB, SH64_MEDIA_INSN_MCNVSWB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MCNVSWUB, SH64_MEDIA_INSN_MCNVSWUB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MEXTR1, SH64_MEDIA_INSN_MEXTR1, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MEXTR2, SH64_MEDIA_INSN_MEXTR2, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MEXTR3, SH64_MEDIA_INSN_MEXTR3, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MEXTR4, SH64_MEDIA_INSN_MEXTR4, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MEXTR5, SH64_MEDIA_INSN_MEXTR5, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MEXTR6, SH64_MEDIA_INSN_MEXTR6, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MEXTR7, SH64_MEDIA_INSN_MEXTR7, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMACFXWL, SH64_MEDIA_INSN_MMACFXWL, SH64_MEDIA_SFMT_MCMV },
  { SH_INSN_MMACNFX_WL, SH64_MEDIA_INSN_MMACNFX_WL, SH64_MEDIA_SFMT_MCMV },
  { SH_INSN_MMULL, SH64_MEDIA_INSN_MMULL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMULW, SH64_MEDIA_INSN_MMULW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMULFXL, SH64_MEDIA_INSN_MMULFXL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMULFXW, SH64_MEDIA_INSN_MMULFXW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMULFXRPW, SH64_MEDIA_INSN_MMULFXRPW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMULHIWL, SH64_MEDIA_INSN_MMULHIWL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMULLOWL, SH64_MEDIA_INSN_MMULLOWL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MMULSUMWQ, SH64_MEDIA_INSN_MMULSUMWQ, SH64_MEDIA_SFMT_MCMV },
  { SH_INSN_MOVI, SH64_MEDIA_INSN_MOVI, SH64_MEDIA_SFMT_MOVI },
  { SH_INSN_MPERMW, SH64_MEDIA_INSN_MPERMW, SH64_MEDIA_SFMT_MPERMW },
  { SH_INSN_MSADUBQ, SH64_MEDIA_INSN_MSADUBQ, SH64_MEDIA_SFMT_MCMV },
  { SH_INSN_MSHALDSL, SH64_MEDIA_INSN_MSHALDSL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHALDSW, SH64_MEDIA_INSN_MSHALDSW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHARDL, SH64_MEDIA_INSN_MSHARDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHARDW, SH64_MEDIA_INSN_MSHARDW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHARDSQ, SH64_MEDIA_INSN_MSHARDSQ, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHFHIB, SH64_MEDIA_INSN_MSHFHIB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHFHIL, SH64_MEDIA_INSN_MSHFHIL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHFHIW, SH64_MEDIA_INSN_MSHFHIW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHFLOB, SH64_MEDIA_INSN_MSHFLOB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHFLOL, SH64_MEDIA_INSN_MSHFLOL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHFLOW, SH64_MEDIA_INSN_MSHFLOW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHLLDL, SH64_MEDIA_INSN_MSHLLDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHLLDW, SH64_MEDIA_INSN_MSHLLDW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHLRDL, SH64_MEDIA_INSN_MSHLRDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSHLRDW, SH64_MEDIA_INSN_MSHLRDW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSUBL, SH64_MEDIA_INSN_MSUBL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSUBW, SH64_MEDIA_INSN_MSUBW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSUBSL, SH64_MEDIA_INSN_MSUBSL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSUBSUB, SH64_MEDIA_INSN_MSUBSUB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MSUBSW, SH64_MEDIA_INSN_MSUBSW, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MULSL, SH64_MEDIA_INSN_MULSL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_MULUL, SH64_MEDIA_INSN_MULUL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_NOP, SH64_MEDIA_INSN_NOP, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_NSB, SH64_MEDIA_INSN_NSB, SH64_MEDIA_SFMT_BYTEREV },
  { SH_INSN_OCBI, SH64_MEDIA_INSN_OCBI, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_OCBP, SH64_MEDIA_INSN_OCBP, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_OCBWB, SH64_MEDIA_INSN_OCBWB, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_OR, SH64_MEDIA_INSN_OR, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_ORI, SH64_MEDIA_INSN_ORI, SH64_MEDIA_SFMT_ORI },
  { SH_INSN_PREFI, SH64_MEDIA_INSN_PREFI, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_PTA, SH64_MEDIA_INSN_PTA, SH64_MEDIA_SFMT_PTA },
  { SH_INSN_PTABS, SH64_MEDIA_INSN_PTABS, SH64_MEDIA_SFMT_PTABS },
  { SH_INSN_PTB, SH64_MEDIA_INSN_PTB, SH64_MEDIA_SFMT_PTA },
  { SH_INSN_PTREL, SH64_MEDIA_INSN_PTREL, SH64_MEDIA_SFMT_PTREL },
  { SH_INSN_PUTCFG, SH64_MEDIA_INSN_PUTCFG, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_PUTCON, SH64_MEDIA_INSN_PUTCON, SH64_MEDIA_SFMT_PUTCON },
  { SH_INSN_RTE, SH64_MEDIA_INSN_RTE, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_SHARD, SH64_MEDIA_INSN_SHARD, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SHARDL, SH64_MEDIA_INSN_SHARDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SHARI, SH64_MEDIA_INSN_SHARI, SH64_MEDIA_SFMT_SHARI },
  { SH_INSN_SHARIL, SH64_MEDIA_INSN_SHARIL, SH64_MEDIA_SFMT_SHARIL },
  { SH_INSN_SHLLD, SH64_MEDIA_INSN_SHLLD, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SHLLDL, SH64_MEDIA_INSN_SHLLDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SHLLI, SH64_MEDIA_INSN_SHLLI, SH64_MEDIA_SFMT_SHARI },
  { SH_INSN_SHLLIL, SH64_MEDIA_INSN_SHLLIL, SH64_MEDIA_SFMT_SHARIL },
  { SH_INSN_SHLRD, SH64_MEDIA_INSN_SHLRD, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SHLRDL, SH64_MEDIA_INSN_SHLRDL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SHLRI, SH64_MEDIA_INSN_SHLRI, SH64_MEDIA_SFMT_SHARI },
  { SH_INSN_SHLRIL, SH64_MEDIA_INSN_SHLRIL, SH64_MEDIA_SFMT_SHARIL },
  { SH_INSN_SHORI, SH64_MEDIA_INSN_SHORI, SH64_MEDIA_SFMT_SHORI },
  { SH_INSN_SLEEP, SH64_MEDIA_INSN_SLEEP, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_STB, SH64_MEDIA_INSN_STB, SH64_MEDIA_SFMT_STB },
  { SH_INSN_STL, SH64_MEDIA_INSN_STL, SH64_MEDIA_SFMT_STL },
  { SH_INSN_STQ, SH64_MEDIA_INSN_STQ, SH64_MEDIA_SFMT_STQ },
  { SH_INSN_STW, SH64_MEDIA_INSN_STW, SH64_MEDIA_SFMT_STW },
  { SH_INSN_STHIL, SH64_MEDIA_INSN_STHIL, SH64_MEDIA_SFMT_STHIL },
  { SH_INSN_STHIQ, SH64_MEDIA_INSN_STHIQ, SH64_MEDIA_SFMT_STHIL },
  { SH_INSN_STLOL, SH64_MEDIA_INSN_STLOL, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_STLOQ, SH64_MEDIA_INSN_STLOQ, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_STXB, SH64_MEDIA_INSN_STXB, SH64_MEDIA_SFMT_STXB },
  { SH_INSN_STXL, SH64_MEDIA_INSN_STXL, SH64_MEDIA_SFMT_STXB },
  { SH_INSN_STXQ, SH64_MEDIA_INSN_STXQ, SH64_MEDIA_SFMT_STXB },
  { SH_INSN_STXW, SH64_MEDIA_INSN_STXW, SH64_MEDIA_SFMT_STXB },
  { SH_INSN_SUB, SH64_MEDIA_INSN_SUB, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SUBL, SH64_MEDIA_INSN_SUBL, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_SWAPQ, SH64_MEDIA_INSN_SWAPQ, SH64_MEDIA_SFMT_SWAPQ },
  { SH_INSN_SYNCI, SH64_MEDIA_INSN_SYNCI, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_SYNCO, SH64_MEDIA_INSN_SYNCO, SH64_MEDIA_SFMT_ALLOCO },
  { SH_INSN_TRAPA, SH64_MEDIA_INSN_TRAPA, SH64_MEDIA_SFMT_TRAPA },
  { SH_INSN_XOR, SH64_MEDIA_INSN_XOR, SH64_MEDIA_SFMT_ADD },
  { SH_INSN_XORI, SH64_MEDIA_INSN_XORI, SH64_MEDIA_SFMT_XORI },
};

static const struct insn_sem sh64_media_insn_sem_invalid = {
  VIRTUAL_INSN_X_INVALID, SH64_MEDIA_INSN_X_INVALID, SH64_MEDIA_SFMT_EMPTY
};

/* Initialize an IDESC from the compile-time computable parts.  */

static INLINE void
init_idesc (SIM_CPU *cpu, IDESC *id, const struct insn_sem *t)
{
  const CGEN_INSN *insn_table = CGEN_CPU_INSN_TABLE (CPU_CPU_DESC (cpu))->init_entries;

  id->num = t->index;
  id->sfmt = t->sfmt;
  if ((int) t->type <= 0)
    id->idata = & cgen_virtual_insn_table[- (int) t->type];
  else
    id->idata = & insn_table[t->type];
  id->attrs = CGEN_INSN_ATTRS (id->idata);
  /* Oh my god, a magic number.  */
  id->length = CGEN_INSN_BITSIZE (id->idata) / 8;

#if WITH_PROFILE_MODEL_P
  id->timing = & MODEL_TIMING (CPU_MODEL (cpu)) [t->index];
  {
    SIM_DESC sd = CPU_STATE (cpu);
    SIM_ASSERT (t->index == id->timing->num);
  }
#endif

  /* Semantic pointers are initialized elsewhere.  */
}

/* Initialize the instruction descriptor table.  */

void
sh64_media_init_idesc_table (SIM_CPU *cpu)
{
  IDESC *id,*tabend;
  const struct insn_sem *t,*tend;
  int tabsize = SH64_MEDIA_INSN_MAX;
  IDESC *table = sh64_media_insn_data;

  memset (table, 0, tabsize * sizeof (IDESC));

  /* First set all entries to the `invalid insn'.  */
  t = & sh64_media_insn_sem_invalid;
  for (id = table, tabend = table + tabsize; id < tabend; ++id)
    init_idesc (cpu, id, t);

  /* Now fill in the values for the chosen cpu.  */
  for (t = sh64_media_insn_sem, tend = t + sizeof (sh64_media_insn_sem) / sizeof (*t);
       t != tend; ++t)
    {
      init_idesc (cpu, & table[t->index], t);
    }

  /* Link the IDESC table into the cpu.  */
  CPU_IDESC (cpu) = table;
}

/* Given an instruction, return a pointer to its IDESC entry.  */

const IDESC *
sh64_media_decode (SIM_CPU *current_cpu, IADDR pc,
              CGEN_INSN_INT base_insn, CGEN_INSN_INT entire_insn,
              ARGBUF *abuf)
{
  /* Result of decoder.  */
  SH64_MEDIA_INSN_TYPE itype;

  {
    CGEN_INSN_INT insn = base_insn;

    {
      unsigned int val = (((insn >> 22) & (63 << 4)) | ((insn >> 16) & (15 << 0)));
      switch (val)
      {
      case 1 : itype = SH64_MEDIA_INSN_CMPEQ; goto extract_sfmt_add;      case 3 : itype = SH64_MEDIA_INSN_CMPGT; goto extract_sfmt_add;      case 7 : itype = SH64_MEDIA_INSN_CMPGTU; goto extract_sfmt_add;      case 8 : itype = SH64_MEDIA_INSN_ADDL; goto extract_sfmt_add;      case 9 : itype = SH64_MEDIA_INSN_ADD; goto extract_sfmt_add;      case 10 : itype = SH64_MEDIA_INSN_SUBL; goto extract_sfmt_add;      case 11 : itype = SH64_MEDIA_INSN_SUB; goto extract_sfmt_add;      case 12 : itype = SH64_MEDIA_INSN_ADDZL; goto extract_sfmt_add;      case 13 : itype = SH64_MEDIA_INSN_NSB; goto extract_sfmt_byterev;      case 14 : itype = SH64_MEDIA_INSN_MULUL; goto extract_sfmt_add;      case 15 : itype = SH64_MEDIA_INSN_BYTEREV; goto extract_sfmt_byterev;      case 16 : itype = SH64_MEDIA_INSN_SHLLDL; goto extract_sfmt_add;      case 17 : itype = SH64_MEDIA_INSN_SHLLD; goto extract_sfmt_add;      case 18 : itype = SH64_MEDIA_INSN_SHLRDL; goto extract_sfmt_add;      case 19 : itype = SH64_MEDIA_INSN_SHLRD; goto extract_sfmt_add;      case 22 : itype = SH64_MEDIA_INSN_SHARDL; goto extract_sfmt_add;      case 23 : itype = SH64_MEDIA_INSN_SHARD; goto extract_sfmt_add;      case 25 : itype = SH64_MEDIA_INSN_OR; goto extract_sfmt_add;      case 27 : itype = SH64_MEDIA_INSN_AND; goto extract_sfmt_add;      case 29 : itype = SH64_MEDIA_INSN_XOR; goto extract_sfmt_add;      case 30 : itype = SH64_MEDIA_INSN_MULSL; goto extract_sfmt_add;      case 31 : itype = SH64_MEDIA_INSN_ANDC; goto extract_sfmt_add;      case 33 : itype = SH64_MEDIA_INSN_MADDW; goto extract_sfmt_add;      case 34 : itype = SH64_MEDIA_INSN_MADDL; goto extract_sfmt_add;      case 36 : itype = SH64_MEDIA_INSN_MADDSUB; goto extract_sfmt_add;      case 37 : itype = SH64_MEDIA_INSN_MADDSW; goto extract_sfmt_add;      case 38 : itype = SH64_MEDIA_INSN_MADDSL; goto extract_sfmt_add;      case 41 : itype = SH64_MEDIA_INSN_MSUBW; goto extract_sfmt_add;      case 42 : itype = SH64_MEDIA_INSN_MSUBL; goto extract_sfmt_add;      case 44 : itype = SH64_MEDIA_INSN_MSUBSUB; goto extract_sfmt_add;      case 45 : itype = SH64_MEDIA_INSN_MSUBSW; goto extract_sfmt_add;      case 46 : itype = SH64_MEDIA_INSN_MSUBSL; goto extract_sfmt_add;      case 49 : itype = SH64_MEDIA_INSN_MSHLLDW; goto extract_sfmt_add;      case 50 : itype = SH64_MEDIA_INSN_MSHLLDL; goto extract_sfmt_add;      case 53 : itype = SH64_MEDIA_INSN_MSHALDSW; goto extract_sfmt_add;      case 54 : itype = SH64_MEDIA_INSN_MSHALDSL; goto extract_sfmt_add;      case 57 : itype = SH64_MEDIA_INSN_MSHARDW; goto extract_sfmt_add;      case 58 : itype = SH64_MEDIA_INSN_MSHARDL; goto extract_sfmt_add;      case 59 : itype = SH64_MEDIA_INSN_MSHARDSQ; goto extract_sfmt_add;      case 61 : itype = SH64_MEDIA_INSN_MSHLRDW; goto extract_sfmt_add;      case 62 : itype = SH64_MEDIA_INSN_MSHLRDL; goto extract_sfmt_add;      case 86 : itype = SH64_MEDIA_INSN_FIPRS; goto extract_sfmt_fiprs;      case 94 : itype = SH64_MEDIA_INSN_FTRVS; goto extract_sfmt_ftrvs;      case 96 : itype = SH64_MEDIA_INSN_FABSS; goto extract_sfmt_fabss;      case 97 : itype = SH64_MEDIA_INSN_FABSD; goto extract_sfmt_fabsd;      case 98 : itype = SH64_MEDIA_INSN_FNEGS; goto extract_sfmt_fabss;      case 99 : itype = SH64_MEDIA_INSN_FNEGD; goto extract_sfmt_fabsd;      case 112 : itype = SH64_MEDIA_INSN_FMOVLS; goto extract_sfmt_fmovls;      case 113 : itype = SH64_MEDIA_INSN_FMOVQD; goto extract_sfmt_fmovqd;      case 114 : itype = SH64_MEDIA_INSN_FGETSCR; goto extract_sfmt_alloco;      case 120 : itype = SH64_MEDIA_INSN_FLDXS; goto extract_sfmt_fldxs;      case 121 : itype = SH64_MEDIA_INSN_FLDXD; goto extract_sfmt_fldxd;      case 125 : itype = SH64_MEDIA_INSN_FLDXP; goto extract_sfmt_fldxp;      case 129 : itype = SH64_MEDIA_INSN_CMVEQ; goto extract_sfmt_cmveq;      case 131 : itype = SH64_MEDIA_INSN_SWAPQ; goto extract_sfmt_swapq;      case 133 : itype = SH64_MEDIA_INSN_CMVNE; goto extract_sfmt_cmveq;      case 159 : itype = SH64_MEDIA_INSN_GETCON; goto extract_sfmt_getcon;      case 160 : itype = SH64_MEDIA_INSN_MCMPEQB; goto extract_sfmt_add;      case 161 : itype = SH64_MEDIA_INSN_MCMPEQW; goto extract_sfmt_add;      case 162 : itype = SH64_MEDIA_INSN_MCMPEQL; goto extract_sfmt_add;      case 164 : itype = SH64_MEDIA_INSN_MCMPGTUB; goto extract_sfmt_add;      case 165 : itype = SH64_MEDIA_INSN_MCMPGTW; goto extract_sfmt_add;      case 166 : itype = SH64_MEDIA_INSN_MCMPGTL; goto extract_sfmt_add;      case 167 : itype = SH64_MEDIA_INSN_MEXTR1; goto extract_sfmt_add;      case 169 : itype = SH64_MEDIA_INSN_MABSW; goto extract_sfmt_byterev;      case 170 : itype = SH64_MEDIA_INSN_MABSL; goto extract_sfmt_byterev;      case 171 : itype = SH64_MEDIA_INSN_MEXTR2; goto extract_sfmt_add;      case 173 : itype = SH64_MEDIA_INSN_MPERMW; goto extract_sfmt_mpermw;      case 175 : itype = SH64_MEDIA_INSN_MEXTR3; goto extract_sfmt_add;      case 176 : itype = SH64_MEDIA_INSN_MSHFLOB; goto extract_sfmt_add;      case 177 : itype = SH64_MEDIA_INSN_MSHFLOW; goto extract_sfmt_add;      case 178 : itype = SH64_MEDIA_INSN_MSHFLOL; goto extract_sfmt_add;      case 179 : itype = SH64_MEDIA_INSN_MEXTR4; goto extract_sfmt_add;      case 180 : itype = SH64_MEDIA_INSN_MSHFHIB; goto extract_sfmt_add;      case 181 : itype = SH64_MEDIA_INSN_MSHFHIW; goto extract_sfmt_add;      case 182 : itype = SH64_MEDIA_INSN_MSHFHIL; goto extract_sfmt_add;      case 183 : itype = SH64_MEDIA_INSN_MEXTR5; goto extract_sfmt_add;      case 187 : itype = SH64_MEDIA_INSN_MEXTR6; goto extract_sfmt_add;      case 191 : itype = SH64_MEDIA_INSN_MEXTR7; goto extract_sfmt_add;      case 192 : itype = SH64_MEDIA_INSN_FMOVSL; goto extract_sfmt_fmovsl;      case 193 : itype = SH64_MEDIA_INSN_FMOVDQ; goto extract_sfmt_fmovdq;      case 194 : itype = SH64_MEDIA_INSN_FPUTSCR; goto extract_sfmt_alloco;      case 200 : itype = SH64_MEDIA_INSN_FCMPEQS; goto extract_sfmt_fcmpeqs;      case 201 : itype = SH64_MEDIA_INSN_FCMPEQD; goto extract_sfmt_fcmpeqd;      case 202 : itype = SH64_MEDIA_INSN_FCMPUNS; goto extract_sfmt_fcmpeqs;      case 203 : itype = SH64_MEDIA_INSN_FCMPUND; goto extract_sfmt_fcmpeqd;      case 204 : itype = SH64_MEDIA_INSN_FCMPGTS; goto extract_sfmt_fcmpeqs;      case 205 : itype = SH64_MEDIA_INSN_FCMPGTD; goto extract_sfmt_fcmpeqd;      case 206 : itype = SH64_MEDIA_INSN_FCMPGES; goto extract_sfmt_fcmpeqs;      case 207 : itype = SH64_MEDIA_INSN_FCMPGED; goto extract_sfmt_fcmpeqd;      case 208 : itype = SH64_MEDIA_INSN_FADDS; goto extract_sfmt_fadds;      case 209 : itype = SH64_MEDIA_INSN_FADDD; goto extract_sfmt_faddd;      case 210 : itype = SH64_MEDIA_INSN_FSUBS; goto extract_sfmt_fadds;      case 211 : itype = SH64_MEDIA_INSN_FSUBD; goto extract_sfmt_faddd;      case 212 : itype = SH64_MEDIA_INSN_FDIVS; goto extract_sfmt_fadds;      case 213 : itype = SH64_MEDIA_INSN_FDIVD; goto extract_sfmt_faddd;      case 214 : itype = SH64_MEDIA_INSN_FMULS; goto extract_sfmt_fadds;      case 215 : itype = SH64_MEDIA_INSN_FMULD; goto extract_sfmt_faddd;      case 222 : itype = SH64_MEDIA_INSN_FMACS; goto extract_sfmt_fmacs;      case 224 : itype = SH64_MEDIA_INSN_FMOVS; goto extract_sfmt_fabss;      case 225 : itype = SH64_MEDIA_INSN_FMOVD; goto extract_sfmt_fabsd;      case 228 : itype = SH64_MEDIA_INSN_FSQRTS; goto extract_sfmt_fabss;      case 229 : itype = SH64_MEDIA_INSN_FSQRTD; goto extract_sfmt_fabsd;      case 230 : itype = SH64_MEDIA_INSN_FCNVSD; goto extract_sfmt_fcnvsd;      case 231 : itype = SH64_MEDIA_INSN_FCNVDS; goto extract_sfmt_fcnvds;      case 232 : itype = SH64_MEDIA_INSN_FTRCSL; goto extract_sfmt_fabss;      case 233 : itype = SH64_MEDIA_INSN_FTRCDQ; goto extract_sfmt_fabsd;      case 234 : itype = SH64_MEDIA_INSN_FTRCSQ; goto extract_sfmt_fcnvsd;      case 235 : itype = SH64_MEDIA_INSN_FTRCDL; goto extract_sfmt_fcnvds;      case 236 : itype = SH64_MEDIA_INSN_FLOATLS; goto extract_sfmt_fabss;      case 237 : itype = SH64_MEDIA_INSN_FLOATQD; goto extract_sfmt_fabsd;      case 238 : itype = SH64_MEDIA_INSN_FLOATLD; goto extract_sfmt_fcnvsd;      case 239 : itype = SH64_MEDIA_INSN_FLOATQS; goto extract_sfmt_fcnvds;      case 248 : itype = SH64_MEDIA_INSN_FSTXS; goto extract_sfmt_fstxs;      case 249 : itype = SH64_MEDIA_INSN_FSTXD; goto extract_sfmt_fstxd;      case 253 : itype = SH64_MEDIA_INSN_FSTXP; goto extract_sfmt_fstxp;      case 256 : itype = SH64_MEDIA_INSN_LDXB; goto extract_sfmt_ldxb;      case 257 : itype = SH64_MEDIA_INSN_LDXW; goto extract_sfmt_ldxb;      case 258 : itype = SH64_MEDIA_INSN_LDXL; goto extract_sfmt_ldxb;      case 259 : itype = SH64_MEDIA_INSN_LDXQ; goto extract_sfmt_ldxb;      case 260 : itype = SH64_MEDIA_INSN_LDXUB; goto extract_sfmt_ldxb;      case 261 : itype = SH64_MEDIA_INSN_LDXUW; goto extract_sfmt_ldxb;      case 273 : itype = SH64_MEDIA_INSN_BLINK; goto extract_sfmt_blink;      case 277 : itype = SH64_MEDIA_INSN_GETTR; goto extract_sfmt_gettr;      case 288 : itype = SH64_MEDIA_INSN_MSADUBQ; goto extract_sfmt_mcmv;      case 289 : itype = SH64_MEDIA_INSN_MMACFXWL; goto extract_sfmt_mcmv;      case 291 : itype = SH64_MEDIA_INSN_MCMV; goto extract_sfmt_mcmv;      case 293 : itype = SH64_MEDIA_INSN_MMACNFX_WL; goto extract_sfmt_mcmv;      case 297 : itype = SH64_MEDIA_INSN_MMULSUMWQ; goto extract_sfmt_mcmv;      case 305 : itype = SH64_MEDIA_INSN_MMULW; goto extract_sfmt_add;      case 306 : itype = SH64_MEDIA_INSN_MMULL; goto extract_sfmt_add;      case 309 : itype = SH64_MEDIA_INSN_MMULFXW; goto extract_sfmt_add;      case 310 : itype = SH64_MEDIA_INSN_MMULFXL; goto extract_sfmt_add;      case 312 : itype = SH64_MEDIA_INSN_MCNVSWB; goto extract_sfmt_add;      case 313 : itype = SH64_MEDIA_INSN_MMULFXRPW; goto extract_sfmt_add;      case 314 : itype = SH64_MEDIA_INSN_MMULLOWL; goto extract_sfmt_add;      case 316 : itype = SH64_MEDIA_INSN_MCNVSWUB; goto extract_sfmt_add;      case 317 : itype = SH64_MEDIA_INSN_MCNVSLW; goto extract_sfmt_add;      case 318 : itype = SH64_MEDIA_INSN_MMULHIWL; goto extract_sfmt_add;      case 384 : itype = SH64_MEDIA_INSN_STXB; goto extract_sfmt_stxb;      case 385 : itype = SH64_MEDIA_INSN_STXW; goto extract_sfmt_stxb;      case 386 : itype = SH64_MEDIA_INSN_STXL; goto extract_sfmt_stxb;      case 387 : itype = SH64_MEDIA_INSN_STXQ; goto extract_sfmt_stxb;      case 401 : itype = SH64_MEDIA_INSN_BEQ; goto extract_sfmt_beq;      case 403 : itype = SH64_MEDIA_INSN_BGE; goto extract_sfmt_beq;      case 405 : itype = SH64_MEDIA_INSN_BNE; goto extract_sfmt_beq;      case 407 : itype = SH64_MEDIA_INSN_BGT; goto extract_sfmt_beq;      case 411 : itype = SH64_MEDIA_INSN_BGEU; goto extract_sfmt_beq;      case 415 : itype = SH64_MEDIA_INSN_BGTU; goto extract_sfmt_beq;      case 417 : itype = SH64_MEDIA_INSN_PTABS; goto extract_sfmt_ptabs;      case 421 : itype = SH64_MEDIA_INSN_PTREL; goto extract_sfmt_ptrel;      case 432 : itype = SH64_MEDIA_INSN_NOP; goto extract_sfmt_alloco;      case 433 : itype = SH64_MEDIA_INSN_TRAPA; goto extract_sfmt_trapa;      case 434 : itype = SH64_MEDIA_INSN_SYNCI; goto extract_sfmt_alloco;      case 435 : itype = SH64_MEDIA_INSN_RTE; goto extract_sfmt_alloco;      case 437 : itype = SH64_MEDIA_INSN_BRK; goto extract_sfmt_brk;      case 438 : itype = SH64_MEDIA_INSN_SYNCO; goto extract_sfmt_alloco;      case 439 : itype = SH64_MEDIA_INSN_SLEEP; goto extract_sfmt_alloco;      case 447 : itype = SH64_MEDIA_INSN_PUTCON; goto extract_sfmt_putcon;      case 512 : /* fall through */
      case 513 : /* fall through */
      case 514 : /* fall through */
      case 515 : /* fall through */
      case 516 : /* fall through */
      case 517 : /* fall through */
      case 518 : /* fall through */
      case 519 : /* fall through */
      case 520 : /* fall through */
      case 521 : /* fall through */
      case 522 : /* fall through */
      case 523 : /* fall through */
      case 524 : /* fall through */
      case 525 : /* fall through */
      case 526 : /* fall through */
      case 527 : itype = SH64_MEDIA_INSN_LDB; goto extract_sfmt_ldb;      case 528 : /* fall through */
      case 529 : /* fall through */
      case 530 : /* fall through */
      case 531 : /* fall through */
      case 532 : /* fall through */
      case 533 : /* fall through */
      case 534 : /* fall through */
      case 535 : /* fall through */
      case 536 : /* fall through */
      case 537 : /* fall through */
      case 538 : /* fall through */
      case 539 : /* fall through */
      case 540 : /* fall through */
      case 541 : /* fall through */
      case 542 : /* fall through */
      case 543 : itype = SH64_MEDIA_INSN_LDW; goto extract_sfmt_lduw;      case 544 : /* fall through */
      case 545 : /* fall through */
      case 546 : /* fall through */
      case 547 : /* fall through */
      case 548 : /* fall through */
      case 549 : /* fall through */
      case 550 : /* fall through */
      case 551 : /* fall through */
      case 552 : /* fall through */
      case 553 : /* fall through */
      case 554 : /* fall through */
      case 555 : /* fall through */
      case 556 : /* fall through */
      case 557 : /* fall through */
      case 558 : /* fall through */
      case 559 : itype = SH64_MEDIA_INSN_LDL; goto extract_sfmt_ldl;      case 560 : /* fall through */
      case 561 : /* fall through */
      case 562 : /* fall through */
      case 563 : /* fall through */
      case 564 : /* fall through */
      case 565 : /* fall through */
      case 566 : /* fall through */
      case 567 : /* fall through */
      case 568 : /* fall through */
      case 569 : /* fall through */
      case 570 : /* fall through */
      case 571 : /* fall through */
      case 572 : /* fall through */
      case 573 : /* fall through */
      case 574 : /* fall through */
      case 575 : itype = SH64_MEDIA_INSN_LDQ; goto extract_sfmt_ldq;      case 576 : /* fall through */
      case 577 : /* fall through */
      case 578 : /* fall through */
      case 579 : /* fall through */
      case 580 : /* fall through */
      case 581 : /* fall through */
      case 582 : /* fall through */
      case 583 : /* fall through */
      case 584 : /* fall through */
      case 585 : /* fall through */
      case 586 : /* fall through */
      case 587 : /* fall through */
      case 588 : /* fall through */
      case 589 : /* fall through */
      case 590 : /* fall through */
      case 591 : itype = SH64_MEDIA_INSN_LDUB; goto extract_sfmt_ldb;      case 592 : /* fall through */
      case 593 : /* fall through */
      case 594 : /* fall through */
      case 595 : /* fall through */
      case 596 : /* fall through */
      case 597 : /* fall through */
      case 598 : /* fall through */
      case 599 : /* fall through */
      case 600 : /* fall through */
      case 601 : /* fall through */
      case 602 : /* fall through */
      case 603 : /* fall through */
      case 604 : /* fall through */
      case 605 : /* fall through */
      case 606 : /* fall through */
      case 607 : itype = SH64_MEDIA_INSN_FLDS; goto extract_sfmt_flds;      case 608 : /* fall through */
      case 609 : /* fall through */
      case 610 : /* fall through */
      case 611 : /* fall through */
      case 612 : /* fall through */
      case 613 : /* fall through */
      case 614 : /* fall through */
      case 615 : /* fall through */
      case 616 : /* fall through */
      case 617 : /* fall through */
      case 618 : /* fall through */
      case 619 : /* fall through */
      case 620 : /* fall through */
      case 621 : /* fall through */
      case 622 : /* fall through */
      case 623 : itype = SH64_MEDIA_INSN_FLDP; goto extract_sfmt_fldp;      case 624 : /* fall through */
      case 625 : /* fall through */
      case 626 : /* fall through */
      case 627 : /* fall through */
      case 628 : /* fall through */
      case 629 : /* fall through */
      case 630 : /* fall through */
      case 631 : /* fall through */
      case 632 : /* fall through */
      case 633 : /* fall through */
      case 634 : /* fall through */
      case 635 : /* fall through */
      case 636 : /* fall through */
      case 637 : /* fall through */
      case 638 : /* fall through */
      case 639 : itype = SH64_MEDIA_INSN_FLDD; goto extract_sfmt_fldd;      case 640 : /* fall through */
      case 641 : /* fall through */
      case 642 : /* fall through */
      case 643 : /* fall through */
      case 644 : /* fall through */
      case 645 : /* fall through */
      case 646 : /* fall through */
      case 647 : /* fall through */
      case 648 : /* fall through */
      case 649 : /* fall through */
      case 650 : /* fall through */
      case 651 : /* fall through */
      case 652 : /* fall through */
      case 653 : /* fall through */
      case 654 : /* fall through */
      case 655 : itype = SH64_MEDIA_INSN_STB; goto extract_sfmt_stb;      case 656 : /* fall through */
      case 657 : /* fall through */
      case 658 : /* fall through */
      case 659 : /* fall through */
      case 660 : /* fall through */
      case 661 : /* fall through */
      case 662 : /* fall through */
      case 663 : /* fall through */
      case 664 : /* fall through */
      case 665 : /* fall through */
      case 666 : /* fall through */
      case 667 : /* fall through */
      case 668 : /* fall through */
      case 669 : /* fall through */
      case 670 : /* fall through */
      case 671 : itype = SH64_MEDIA_INSN_STW; goto extract_sfmt_stw;      case 672 : /* fall through */
      case 673 : /* fall through */
      case 674 : /* fall through */
      case 675 : /* fall through */
      case 676 : /* fall through */
      case 677 : /* fall through */
      case 678 : /* fall through */
      case 679 : /* fall through */
      case 680 : /* fall through */
      case 681 : /* fall through */
      case 682 : /* fall through */
      case 683 : /* fall through */
      case 684 : /* fall through */
      case 685 : /* fall through */
      case 686 : /* fall through */
      case 687 : itype = SH64_MEDIA_INSN_STL; goto extract_sfmt_stl;      case 688 : /* fall through */
      case 689 : /* fall through */
      case 690 : /* fall through */
      case 691 : /* fall through */
      case 692 : /* fall through */
      case 693 : /* fall through */
      case 694 : /* fall through */
      case 695 : /* fall through */
      case 696 : /* fall through */
      case 697 : /* fall through */
      case 698 : /* fall through */
      case 699 : /* fall through */
      case 700 : /* fall through */
      case 701 : /* fall through */
      case 702 : /* fall through */
      case 703 : itype = SH64_MEDIA_INSN_STQ; goto extract_sfmt_stq;      case 704 : /* fall through */
      case 705 : /* fall through */
      case 706 : /* fall through */
      case 707 : /* fall through */
      case 708 : /* fall through */
      case 709 : /* fall through */
      case 710 : /* fall through */
      case 711 : /* fall through */
      case 712 : /* fall through */
      case 713 : /* fall through */
      case 714 : /* fall through */
      case 715 : /* fall through */
      case 716 : /* fall through */
      case 717 : /* fall through */
      case 718 : /* fall through */
      case 719 : itype = SH64_MEDIA_INSN_LDUW; goto extract_sfmt_lduw;      case 720 : /* fall through */
      case 721 : /* fall through */
      case 722 : /* fall through */
      case 723 : /* fall through */
      case 724 : /* fall through */
      case 725 : /* fall through */
      case 726 : /* fall through */
      case 727 : /* fall through */
      case 728 : /* fall through */
      case 729 : /* fall through */
      case 730 : /* fall through */
      case 731 : /* fall through */
      case 732 : /* fall through */
      case 733 : /* fall through */
      case 734 : /* fall through */
      case 735 : itype = SH64_MEDIA_INSN_FSTS; goto extract_sfmt_fsts;      case 736 : /* fall through */
      case 737 : /* fall through */
      case 738 : /* fall through */
      case 739 : /* fall through */
      case 740 : /* fall through */
      case 741 : /* fall through */
      case 742 : /* fall through */
      case 743 : /* fall through */
      case 744 : /* fall through */
      case 745 : /* fall through */
      case 746 : /* fall through */
      case 747 : /* fall through */
      case 748 : /* fall through */
      case 749 : /* fall through */
      case 750 : /* fall through */
      case 751 : itype = SH64_MEDIA_INSN_FSTP; goto extract_sfmt_fstp;      case 752 : /* fall through */
      case 753 : /* fall through */
      case 754 : /* fall through */
      case 755 : /* fall through */
      case 756 : /* fall through */
      case 757 : /* fall through */
      case 758 : /* fall through */
      case 759 : /* fall through */
      case 760 : /* fall through */
      case 761 : /* fall through */
      case 762 : /* fall through */
      case 763 : /* fall through */
      case 764 : /* fall through */
      case 765 : /* fall through */
      case 766 : /* fall through */
      case 767 : itype = SH64_MEDIA_INSN_FSTD; goto extract_sfmt_fstd;      case 770 : itype = SH64_MEDIA_INSN_LDLOL; goto extract_sfmt_alloco;      case 771 : itype = SH64_MEDIA_INSN_LDLOQ; goto extract_sfmt_alloco;      case 774 : itype = SH64_MEDIA_INSN_LDHIL; goto extract_sfmt_alloco;      case 775 : itype = SH64_MEDIA_INSN_LDHIQ; goto extract_sfmt_alloco;      case 783 : itype = SH64_MEDIA_INSN_GETCFG; goto extract_sfmt_alloco;      case 784 : itype = SH64_MEDIA_INSN_SHLLIL; goto extract_sfmt_sharil;      case 785 : itype = SH64_MEDIA_INSN_SHLLI; goto extract_sfmt_shari;      case 786 : itype = SH64_MEDIA_INSN_SHLRIL; goto extract_sfmt_sharil;      case 787 : itype = SH64_MEDIA_INSN_SHLRI; goto extract_sfmt_shari;      case 790 : itype = SH64_MEDIA_INSN_SHARIL; goto extract_sfmt_sharil;      case 791 : itype = SH64_MEDIA_INSN_SHARI; goto extract_sfmt_shari;      case 797 : itype = SH64_MEDIA_INSN_XORI; goto extract_sfmt_xori;      case 800 : /* fall through */
      case 801 : /* fall through */
      case 802 : /* fall through */
      case 803 : /* fall through */
      case 804 : /* fall through */
      case 805 : /* fall through */
      case 806 : /* fall through */
      case 807 : /* fall through */
      case 808 : /* fall through */
      case 809 : /* fall through */
      case 810 : /* fall through */
      case 811 : /* fall through */
      case 812 : /* fall through */
      case 813 : /* fall through */
      case 814 : /* fall through */
      case 815 : itype = SH64_MEDIA_INSN_SHORI; goto extract_sfmt_shori;      case 816 : /* fall through */
      case 817 : /* fall through */
      case 818 : /* fall through */
      case 819 : /* fall through */
      case 820 : /* fall through */
      case 821 : /* fall through */
      case 822 : /* fall through */
      case 823 : /* fall through */
      case 824 : /* fall through */
      case 825 : /* fall through */
      case 826 : /* fall through */
      case 827 : /* fall through */
      case 828 : /* fall through */
      case 829 : /* fall through */
      case 830 : /* fall through */
      case 831 : itype = SH64_MEDIA_INSN_MOVI; goto extract_sfmt_movi;      case 832 : /* fall through */
      case 833 : /* fall through */
      case 834 : /* fall through */
      case 835 : /* fall through */
      case 836 : /* fall through */
      case 837 : /* fall through */
      case 838 : /* fall through */
      case 839 : /* fall through */
      case 840 : /* fall through */
      case 841 : /* fall through */
      case 842 : /* fall through */
      case 843 : /* fall through */
      case 844 : /* fall through */
      case 845 : /* fall through */
      case 846 : /* fall through */
      case 847 : itype = SH64_MEDIA_INSN_ADDI; goto extract_sfmt_addi;      case 848 : /* fall through */
      case 849 : /* fall through */
      case 850 : /* fall through */
      case 851 : /* fall through */
      case 852 : /* fall through */
      case 853 : /* fall through */
      case 854 : /* fall through */
      case 855 : /* fall through */
      case 856 : /* fall through */
      case 857 : /* fall through */
      case 858 : /* fall through */
      case 859 : /* fall through */
      case 860 : /* fall through */
      case 861 : /* fall through */
      case 862 : /* fall through */
      case 863 : itype = SH64_MEDIA_INSN_ADDIL; goto extract_sfmt_addi;      case 864 : /* fall through */
      case 865 : /* fall through */
      case 866 : /* fall through */
      case 867 : /* fall through */
      case 868 : /* fall through */
      case 869 : /* fall through */
      case 870 : /* fall through */
      case 871 : /* fall through */
      case 872 : /* fall through */
      case 873 : /* fall through */
      case 874 : /* fall through */
      case 875 : /* fall through */
      case 876 : /* fall through */
      case 877 : /* fall through */
      case 878 : /* fall through */
      case 879 : itype = SH64_MEDIA_INSN_ANDI; goto extract_sfmt_addi;      case 880 : /* fall through */
      case 881 : /* fall through */
      case 882 : /* fall through */
      case 883 : /* fall through */
      case 884 : /* fall through */
      case 885 : /* fall through */
      case 886 : /* fall through */
      case 887 : /* fall through */
      case 888 : /* fall through */
      case 889 : /* fall through */
      case 890 : /* fall through */
      case 891 : /* fall through */
      case 892 : /* fall through */
      case 893 : /* fall through */
      case 894 : /* fall through */
      case 895 : itype = SH64_MEDIA_INSN_ORI; goto extract_sfmt_ori;      case 897 : itype = SH64_MEDIA_INSN_PREFI; goto extract_sfmt_alloco;      case 898 : itype = SH64_MEDIA_INSN_STLOL; goto extract_sfmt_alloco;      case 899 : itype = SH64_MEDIA_INSN_STLOQ; goto extract_sfmt_alloco;      case 900 : itype = SH64_MEDIA_INSN_ALLOCO; goto extract_sfmt_alloco;      case 901 : itype = SH64_MEDIA_INSN_ICBI; goto extract_sfmt_alloco;      case 902 : itype = SH64_MEDIA_INSN_STHIL; goto extract_sfmt_sthil;      case 903 : itype = SH64_MEDIA_INSN_STHIQ; goto extract_sfmt_sthil;      case 904 : itype = SH64_MEDIA_INSN_OCBP; goto extract_sfmt_alloco;      case 905 : itype = SH64_MEDIA_INSN_OCBI; goto extract_sfmt_alloco;      case 908 : itype = SH64_MEDIA_INSN_OCBWB; goto extract_sfmt_alloco;      case 911 : itype = SH64_MEDIA_INSN_PUTCFG; goto extract_sfmt_alloco;      case 913 : itype = SH64_MEDIA_INSN_BEQI; goto extract_sfmt_beqi;      case 917 : itype = SH64_MEDIA_INSN_BNEI; goto extract_sfmt_beqi;      case 928 : /* fall through */
      case 929 : /* fall through */
      case 930 : /* fall through */
      case 931 : /* fall through */
      case 932 : /* fall through */
      case 933 : /* fall through */
      case 934 : /* fall through */
      case 935 : /* fall through */
      case 936 : /* fall through */
      case 937 : /* fall through */
      case 938 : /* fall through */
      case 939 : /* fall through */
      case 940 : /* fall through */
      case 941 : /* fall through */
      case 942 : /* fall through */
      case 943 : itype = SH64_MEDIA_INSN_PTA; goto extract_sfmt_pta;      case 944 : /* fall through */
      case 945 : /* fall through */
      case 946 : /* fall through */
      case 947 : /* fall through */
      case 948 : /* fall through */
      case 949 : /* fall through */
      case 950 : /* fall through */
      case 951 : /* fall through */
      case 952 : /* fall through */
      case 953 : /* fall through */
      case 954 : /* fall through */
      case 955 : /* fall through */
      case 956 : /* fall through */
      case 957 : /* fall through */
      case 958 : /* fall through */
      case 959 : itype = SH64_MEDIA_INSN_PTB; goto extract_sfmt_pta;      default : itype = SH64_MEDIA_INSN_X_INVALID; goto extract_sfmt_empty;
      }
    }
  }

  /* The instruction has been decoded, now extract the fields.  */

 extract_sfmt_empty:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_empty", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_add:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_add", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_addi:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi.f
    UINT f_left;
    INT f_disp10;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10 = EXTRACT_LSB0_INT (insn, 32, 19, 10);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10) = f_disp10;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_addi", "f_disp10 0x%x", 'x', f_disp10, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_alloco:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_alloco", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_beq:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_beq.f
    UINT f_left;
    UINT f_right;
    UINT f_tra;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_tra = EXTRACT_LSB0_UINT (insn, 32, 6, 3);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_tra) = f_tra;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_beq", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_tra 0x%x", 'x', f_tra, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_beqi:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_beqi.f
    UINT f_left;
    INT f_imm6;
    UINT f_tra;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_imm6 = EXTRACT_LSB0_INT (insn, 32, 15, 6);
    f_tra = EXTRACT_LSB0_UINT (insn, 32, 6, 3);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm6) = f_imm6;
  FLD (f_left) = f_left;
  FLD (f_tra) = f_tra;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_beqi", "f_imm6 0x%x", 'x', f_imm6, "f_left 0x%x", 'x', f_left, "f_tra 0x%x", 'x', f_tra, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_blink:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_blink.f
    UINT f_trb;
    UINT f_dest;

    f_trb = EXTRACT_LSB0_UINT (insn, 32, 22, 3);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_trb) = f_trb;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_blink", "f_trb 0x%x", 'x', f_trb, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_brk:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
#define FLD(f) abuf->fields.fmt_empty.f


  /* Record the fields for the semantic handler.  */
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_brk", (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_byterev:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_xori.f
    UINT f_left;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_byterev", "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_cmveq:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_cmveq", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fabsd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fabsd.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;
    UINT f_left_right;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);
  f_left_right = f_left;

  /* Record the fields for the semantic handler.  */
  FLD (f_left_right) = f_left_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fabsd", "f_left_right 0x%x", 'x', f_left_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fabss:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fabsd.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;
    UINT f_left_right;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);
  f_left_right = f_left;

  /* Record the fields for the semantic handler.  */
  FLD (f_left_right) = f_left_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fabss", "f_left_right 0x%x", 'x', f_left_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_faddd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_faddd", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fadds:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fadds", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fcmpeqd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fcmpeqd", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fcmpeqs:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fcmpeqs", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fcnvds:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fabsd.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;
    UINT f_left_right;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);
  f_left_right = f_left;

  /* Record the fields for the semantic handler.  */
  FLD (f_left_right) = f_left_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fcnvds", "f_left_right 0x%x", 'x', f_left_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fcnvsd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fabsd.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;
    UINT f_left_right;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);
  f_left_right = f_left;

  /* Record the fields for the semantic handler.  */
  FLD (f_left_right) = f_left_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fcnvsd", "f_left_right 0x%x", 'x', f_left_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fiprs:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fiprs", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fldd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fldd.f
    UINT f_left;
    SI f_disp10x8;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x8 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (3));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x8) = f_disp10x8;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fldd", "f_disp10x8 0x%x", 'x', f_disp10x8, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fldp:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fldd.f
    UINT f_left;
    SI f_disp10x8;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x8 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (3));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x8) = f_disp10x8;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fldp", "f_disp10x8 0x%x", 'x', f_disp10x8, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_flds:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_flds.f
    UINT f_left;
    SI f_disp10x4;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x4 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (2));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x4) = f_disp10x4;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_flds", "f_disp10x4 0x%x", 'x', f_disp10x4, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fldxd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fldxd", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fldxp:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fldxp", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fldxs:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fldxs", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmacs:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmacs", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmovdq:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fabsd.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;
    UINT f_left_right;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);
  f_left_right = f_left;

  /* Record the fields for the semantic handler.  */
  FLD (f_left_right) = f_left_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmovdq", "f_left_right 0x%x", 'x', f_left_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmovls:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_xori.f
    UINT f_left;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmovls", "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmovqd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_xori.f
    UINT f_left;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmovqd", "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fmovsl:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fabsd.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;
    UINT f_left_right;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);
  f_left_right = f_left;

  /* Record the fields for the semantic handler.  */
  FLD (f_left_right) = f_left_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fmovsl", "f_left_right 0x%x", 'x', f_left_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fstd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fldd.f
    UINT f_left;
    SI f_disp10x8;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x8 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (3));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x8) = f_disp10x8;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fstd", "f_disp10x8 0x%x", 'x', f_disp10x8, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fstp:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fldd.f
    UINT f_left;
    SI f_disp10x8;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x8 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (3));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x8) = f_disp10x8;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fstp", "f_disp10x8 0x%x", 'x', f_disp10x8, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fsts:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_flds.f
    UINT f_left;
    SI f_disp10x4;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x4 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (2));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x4) = f_disp10x4;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fsts", "f_disp10x4 0x%x", 'x', f_disp10x4, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fstxd:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fstxd", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fstxp:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fstxp", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_fstxs:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_fstxs", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ftrvs:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ftrvs", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_getcon:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_xori.f
    UINT f_left;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_getcon", "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_gettr:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_blink.f
    UINT f_trb;
    UINT f_dest;

    f_trb = EXTRACT_LSB0_UINT (insn, 32, 22, 3);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_trb) = f_trb;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_gettr", "f_trb 0x%x", 'x', f_trb, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldb:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi.f
    UINT f_left;
    INT f_disp10;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10 = EXTRACT_LSB0_INT (insn, 32, 19, 10);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10) = f_disp10;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldb", "f_disp10 0x%x", 'x', f_disp10, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldl:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_flds.f
    UINT f_left;
    SI f_disp10x4;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x4 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (2));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x4) = f_disp10x4;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldl", "f_disp10x4 0x%x", 'x', f_disp10x4, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldq:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fldd.f
    UINT f_left;
    SI f_disp10x8;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x8 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (3));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x8) = f_disp10x8;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldq", "f_disp10x8 0x%x", 'x', f_disp10x8, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_lduw:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_lduw.f
    UINT f_left;
    SI f_disp10x2;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x2 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (1));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x2) = f_disp10x2;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_lduw", "f_disp10x2 0x%x", 'x', f_disp10x2, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ldxb:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ldxb", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_mcmv:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_mcmv", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_movi:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_movi.f
    INT f_imm16;
    UINT f_dest;

    f_imm16 = EXTRACT_LSB0_INT (insn, 32, 25, 16);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm16) = f_imm16;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_movi", "f_imm16 0x%x", 'x', f_imm16, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_mpermw:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_mpermw", "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ori:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_ori.f
    UINT f_left;
    INT f_imm10;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_imm10 = EXTRACT_LSB0_INT (insn, 32, 19, 10);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm10) = f_imm10;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ori", "f_imm10 0x%x", 'x', f_imm10, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_pta:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_pta.f
    DI f_disp16;
    UINT f_tra;

    f_disp16 = ((((EXTRACT_LSB0_INT (insn, 32, 25, 16)) << (2))) + (pc));
    f_tra = EXTRACT_LSB0_UINT (insn, 32, 6, 3);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp16) = f_disp16;
  FLD (f_tra) = f_tra;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_pta", "f_disp16 0x%x", 'x', f_disp16, "f_tra 0x%x", 'x', f_tra, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ptabs:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_beq.f
    UINT f_right;
    UINT f_tra;

    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_tra = EXTRACT_LSB0_UINT (insn, 32, 6, 3);

  /* Record the fields for the semantic handler.  */
  FLD (f_right) = f_right;
  FLD (f_tra) = f_tra;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ptabs", "f_right 0x%x", 'x', f_right, "f_tra 0x%x", 'x', f_tra, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_ptrel:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_beq.f
    UINT f_right;
    UINT f_tra;

    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_tra = EXTRACT_LSB0_UINT (insn, 32, 6, 3);

  /* Record the fields for the semantic handler.  */
  FLD (f_right) = f_right;
  FLD (f_tra) = f_tra;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_ptrel", "f_right 0x%x", 'x', f_right, "f_tra 0x%x", 'x', f_tra, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_putcon:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_xori.f
    UINT f_left;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_putcon", "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_shari:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_shari.f
    UINT f_left;
    UINT f_uimm6;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_uimm6 = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_uimm6) = f_uimm6;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_shari", "f_left 0x%x", 'x', f_left, "f_uimm6 0x%x", 'x', f_uimm6, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_sharil:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_shari.f
    UINT f_left;
    UINT f_uimm6;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_uimm6 = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  FLD (f_uimm6) = f_uimm6;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_sharil", "f_left 0x%x", 'x', f_left, "f_uimm6 0x%x", 'x', f_uimm6, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_shori:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_shori.f
    UINT f_uimm16;
    UINT f_dest;

    f_uimm16 = EXTRACT_LSB0_UINT (insn, 32, 25, 16);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_uimm16) = f_uimm16;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_shori", "f_dest 0x%x", 'x', f_dest, "f_uimm16 0x%x", 'x', f_uimm16, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stb:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_addi.f
    UINT f_left;
    INT f_disp10;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10 = EXTRACT_LSB0_INT (insn, 32, 19, 10);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10) = f_disp10;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stb", "f_disp10 0x%x", 'x', f_disp10, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stl:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_flds.f
    UINT f_left;
    SI f_disp10x4;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x4 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (2));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x4) = f_disp10x4;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stl", "f_disp10x4 0x%x", 'x', f_disp10x4, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stq:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_fldd.f
    UINT f_left;
    SI f_disp10x8;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x8 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (3));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x8) = f_disp10x8;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stq", "f_disp10x8 0x%x", 'x', f_disp10x8, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stw:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_lduw.f
    UINT f_left;
    SI f_disp10x2;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp10x2 = ((EXTRACT_LSB0_INT (insn, 32, 19, 10)) << (1));
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp10x2) = f_disp10x2;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stw", "f_disp10x2 0x%x", 'x', f_disp10x2, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_sthil:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_sthil.f
    UINT f_left;
    INT f_disp6;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_disp6 = EXTRACT_LSB0_INT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_disp6) = f_disp6;
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_sthil", "f_disp6 0x%x", 'x', f_disp6, "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_stxb:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_stxb", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_swapq:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_add.f
    UINT f_left;
    UINT f_right;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_right = EXTRACT_LSB0_UINT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_dest) = f_dest;
  FLD (f_left) = f_left;
  FLD (f_right) = f_right;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_swapq", "f_dest 0x%x", 'x', f_dest, "f_left 0x%x", 'x', f_left, "f_right 0x%x", 'x', f_right, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_trapa:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_xori.f
    UINT f_left;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_left) = f_left;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_trapa", "f_left 0x%x", 'x', f_left, (char *) 0));

#undef FLD
    return idesc;
  }

 extract_sfmt_xori:
  {
    const IDESC *idesc = &sh64_media_insn_data[itype];
    CGEN_INSN_INT insn = entire_insn;
#define FLD(f) abuf->fields.sfmt_xori.f
    UINT f_left;
    INT f_imm6;
    UINT f_dest;

    f_left = EXTRACT_LSB0_UINT (insn, 32, 25, 6);
    f_imm6 = EXTRACT_LSB0_INT (insn, 32, 15, 6);
    f_dest = EXTRACT_LSB0_UINT (insn, 32, 9, 6);

  /* Record the fields for the semantic handler.  */
  FLD (f_imm6) = f_imm6;
  FLD (f_left) = f_left;
  FLD (f_dest) = f_dest;
  TRACE_EXTRACT (current_cpu, abuf, (current_cpu, pc, "sfmt_xori", "f_imm6 0x%x", 'x', f_imm6, "f_left 0x%x", 'x', f_left, "f_dest 0x%x", 'x', f_dest, (char *) 0));

#undef FLD
    return idesc;
  }

}
