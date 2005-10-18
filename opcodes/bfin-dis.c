/* Disassemble ADI Blackfin Instructions.
   Copyright 2005 Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opcode/bfin.h"

#define M_S2RND 1
#define M_T     2
#define M_W32   3
#define M_FU    4
#define M_TFU   6
#define M_IS    8
#define M_ISS2  9
#define M_IH    11
#define M_IU    12

#ifndef PRINTF
#define PRINTF printf
#endif

#ifndef EXIT
#define EXIT exit
#endif

typedef long TIword;

#define HOST_LONG_WORD_SIZE (sizeof(long)*8)

#define XFIELD(w,p,s) (((w)&((1<<(s))-1)<<(p))>>(p))

#define SIGNEXTEND(v, n) ((v << (HOST_LONG_WORD_SIZE - (n))) >> (HOST_LONG_WORD_SIZE - (n)))
#define MASKBITS(val, bits) (val & (( 1 << bits)-1))


#include "dis-asm.h"
typedef enum
{
  c_0, c_1, c_4, c_2, c_uimm2, c_uimm3, c_imm3, c_pcrel4,
  c_imm4, c_uimm4s4, c_uimm4, c_uimm4s2, c_negimm5s4, c_imm5, c_uimm5, c_imm6,
  c_imm7, c_imm8, c_uimm8, c_pcrel8, c_uimm8s4, c_pcrel8s4, c_lppcrel10, c_pcrel10,
  c_pcrel12, c_imm16s4, c_luimm16, c_imm16, c_huimm16, c_rimm16, c_imm16s2, c_uimm16s4,
  c_uimm16, c_pcrel24,
} const_forms_t;

static struct
{
  char *name;
  int nbits;
  char reloc;
  char issigned;
  char pcrel;
  char scale;
  char offset;
  char negative;
  char positive;
} constant_formats[] =
{
  { "0", 0, 0, 1, 0, 0, 0, 0, 0},
  { "1", 0, 0, 1, 0, 0, 0, 0, 0},
  { "4", 0, 0, 1, 0, 0, 0, 0, 0},
  { "2", 0, 0, 1, 0, 0, 0, 0, 0},
  { "uimm2", 2, 0, 0, 0, 0, 0, 0, 0},
  { "uimm3", 3, 0, 0, 0, 0, 0, 0, 0},
  { "imm3", 3, 0, 1, 0, 0, 0, 0, 0},
  { "pcrel4", 4, 1, 0, 1, 1, 0, 0, 0},
  { "imm4", 4, 0, 1, 0, 0, 0, 0, 0},
  { "uimm4s4", 4, 0, 0, 0, 2, 0, 0, 1},
  { "uimm4", 4, 0, 0, 0, 0, 0, 0, 0},
  { "uimm4s2", 4, 0, 0, 0, 1, 0, 0, 1},
  { "negimm5s4", 5, 0, 1, 0, 2, 0, 1, 0},
  { "imm5", 5, 0, 1, 0, 0, 0, 0, 0},
  { "uimm5", 5, 0, 0, 0, 0, 0, 0, 0},
  { "imm6", 6, 0, 1, 0, 0, 0, 0, 0},
  { "imm7", 7, 0, 1, 0, 0, 0, 0, 0},
  { "imm8", 8, 0, 1, 0, 0, 0, 0, 0},
  { "uimm8", 8, 0, 0, 0, 0, 0, 0, 0},
  { "pcrel8", 8, 1, 0, 1, 1, 0, 0, 0},
  { "uimm8s4", 8, 0, 0, 0, 2, 0, 0, 0},
  { "pcrel8s4", 8, 1, 1, 1, 2, 0, 0, 0},
  { "lppcrel10", 10, 1, 0, 1, 1, 0, 0, 0},
  { "pcrel10", 10, 1, 1, 1, 1, 0, 0, 0},
  { "pcrel12", 12, 1, 1, 1, 1, 0, 0, 0},
  { "imm16s4", 16, 0, 1, 0, 2, 0, 0, 0},
  { "luimm16", 16, 1, 0, 0, 0, 0, 0, 0},
  { "imm16", 16, 0, 1, 0, 0, 0, 0, 0},
  { "huimm16", 16, 1, 0, 0, 0, 0, 0, 0},
  { "rimm16", 16, 1, 1, 0, 0, 0, 0, 0},
  { "imm16s2", 16, 0, 1, 0, 1, 0, 0, 0},
  { "uimm16s4", 16, 0, 0, 0, 2, 0, 0, 0},
  { "uimm16", 16, 0, 0, 0, 0, 0, 0, 0},
  { "pcrel24", 24, 1, 1, 1, 1, 0, 0, 0}
};

int _print_insn_bfin (bfd_vma pc, disassemble_info * outf);
int print_insn_bfin (bfd_vma pc, disassemble_info * outf);

static char *
fmtconst (const_forms_t cf, TIword x, bfd_vma pc, disassemble_info * outf)
{
  static char buf[60];

  if (constant_formats[cf].reloc)
    {
      bfd_vma ea = (((constant_formats[cf].pcrel ? SIGNEXTEND (x, constant_formats[cf].nbits)
		      : x) + constant_formats[cf].offset) << constant_formats[cf].scale);
      if (constant_formats[cf].pcrel)
	ea += pc;

      outf->print_address_func (ea, outf);
      return "";
    }

  /* Negative constants have an implied sign bit.  */
  if (constant_formats[cf].negative)
    {
      int nb = constant_formats[cf].nbits + 1;
      x = x | (1 << constant_formats[cf].nbits);
      x = SIGNEXTEND (x, nb);
    }
  else
    x = constant_formats[cf].issigned ? SIGNEXTEND (x, constant_formats[cf].nbits) : x;

  if (constant_formats[cf].offset)
    x += constant_formats[cf].offset;

  if (constant_formats[cf].scale)
    x <<= constant_formats[cf].scale;

  if (constant_formats[cf].issigned && x < 0)
    sprintf (buf, "%ld", x);
  else
    sprintf (buf, "0x%lx", x);

  return buf;
}

#undef SIGNEXTEND
#undef MASKBITS
#undef HOST_LONG_WORD_SIZE
#define HOST_LONG_WORD_SIZE (sizeof(long)*8)
#define SIGNEXTEND(v, n) (((long)(v) << (HOST_LONG_WORD_SIZE - (n))) >> (HOST_LONG_WORD_SIZE - (n)))
#define MASKBITS(val, bits) (val & (( 1 << bits)-1))

enum machine_registers
{
  REG_RL0, REG_RL1, REG_RL2, REG_RL3, REG_RL4, REG_RL5, REG_RL6, REG_RL7,
  REG_RH0, REG_RH1, REG_RH2, REG_RH3, REG_RH4, REG_RH5, REG_RH6, REG_RH7,
  REG_R0, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7,
  REG_R1_0, REG_R3_2, REG_R5_4, REG_R7_6, REG_P0, REG_P1, REG_P2, REG_P3,
  REG_P4, REG_P5, REG_SP, REG_FP, REG_A0x, REG_A1x, REG_A0w, REG_A1w,
  REG_A0, REG_A1, REG_I0, REG_I1, REG_I2, REG_I3, REG_M0, REG_M1,
  REG_M2, REG_M3, REG_B0, REG_B1, REG_B2, REG_B3, REG_L0, REG_L1,
  REG_L2, REG_L3,
  REG_AZ, REG_AN, REG_AC0, REG_AC1, REG_AV0, REG_AV1, REG_AV0S, REG_AV1S,
  REG_AQ, REG_V, REG_VS,
  REG_sftreset, REG_omode, REG_excause, REG_emucause, REG_idle_req, REG_hwerrcause, REG_CC, REG_LC0,
  REG_LC1, REG_GP, REG_ASTAT, REG_RETS, REG_LT0, REG_LB0, REG_LT1, REG_LB1,
  REG_CYCLES, REG_CYCLES2, REG_USP, REG_SEQSTAT, REG_SYSCFG, REG_RETI, REG_RETX, REG_RETN,
  REG_RETE, REG_EMUDAT, REG_BR0, REG_BR1, REG_BR2, REG_BR3, REG_BR4, REG_BR5, REG_BR6,
  REG_BR7, REG_PL0, REG_PL1, REG_PL2, REG_PL3, REG_PL4, REG_PL5, REG_SLP, REG_FLP,
  REG_PH0, REG_PH1, REG_PH2, REG_PH3, REG_PH4, REG_PH5, REG_SHP, REG_FHP,
  REG_IL0, REG_IL1, REG_IL2, REG_IL3, REG_ML0, REG_ML1, REG_ML2, REG_ML3,
  REG_BL0, REG_BL1, REG_BL2, REG_BL3, REG_LL0, REG_LL1, REG_LL2, REG_LL3,
  REG_IH0, REG_IH1, REG_IH2, REG_IH3, REG_MH0, REG_MH1, REG_MH2, REG_MH3,
  REG_BH0, REG_BH1, REG_BH2, REG_BH3, REG_LH0, REG_LH1, REG_LH2, REG_LH3,
  REG_LASTREG,
};

enum reg_class
{
  rc_dregs_lo, rc_dregs_hi, rc_dregs, rc_dregs_pair, rc_pregs, rc_spfp, rc_dregs_hilo, rc_accum_ext,
  rc_accum_word, rc_accum, rc_iregs, rc_mregs, rc_bregs, rc_lregs, rc_dpregs, rc_gregs,
  rc_regs, rc_statbits, rc_ignore_bits, rc_ccstat, rc_counters, rc_dregs2_sysregs1, rc_open, rc_sysregs2,
  rc_sysregs3, rc_allregs,
  LIM_REG_CLASSES
};

static char *reg_names[] =
{
  "R0.L", "R1.L", "R2.L", "R3.L", "R4.L", "R5.L", "R6.L", "R7.L",
  "R0.H", "R1.H", "R2.H", "R3.H", "R4.H", "R5.H", "R6.H", "R7.H",
  "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
  "R1:0", "R3:2", "R5:4", "R7:6", "P0", "P1", "P2", "P3",
  "P4", "P5", "SP", "FP", "A0.x", "A1.x", "A0.w", "A1.w",
  "A0", "A1", "I0", "I1", "I2", "I3", "M0", "M1",
  "M2", "M3", "B0", "B1", "B2", "B3", "L0", "L1",
  "L2", "L3",
  "AZ", "AN", "AC0", "AC1", "AV0", "AV1", "AV0S", "AV1S",
  "AQ", "V", "VS",
  "sftreset", "omode", "excause", "emucause", "idle_req", "hwerrcause", "CC", "LC0",
  "LC1", "GP", "ASTAT", "RETS", "LT0", "LB0", "LT1", "LB1",
  "CYCLES", "CYCLES2", "USP", "SEQSTAT", "SYSCFG", "RETI", "RETX", "RETN",
  "RETE", "EMUDAT",
  "R0.B", "R1.B", "R2.B", "R3.B", "R4.B", "R5.B", "R6.B", "R7.B",
  "P0.L", "P1.L", "P2.L", "P3.L", "P4.L", "P5.L", "SP.L", "FP.L",
  "P0.H", "P1.H", "P2.H", "P3.H", "P4.H", "P5.H", "SP.H", "FP.H",
  "I0.L", "I1.L", "I2.L", "I3.L", "M0.L", "M1.L", "M2.L", "M3.L",
  "B0.L", "B1.L", "B2.L", "B3.L", "L0.L", "L1.L", "L2.L", "L3.L",
  "I0.H", "I1.H", "I2.H", "I3.H", "M0.H", "M1.H", "M2.H", "M3.H",
  "B0.H", "B1.H", "B2.H", "B3.H", "L0.H", "L1.H", "L2.H", "L3.H",
  "LASTREG",
  0
};

#define REGNAME(x) ((x) < REG_LASTREG ? (reg_names[x]) : "...... Illegal register .......")

/* RL(0..7).  */
static enum machine_registers decode_dregs_lo[] =
{
  REG_RL0, REG_RL1, REG_RL2, REG_RL3, REG_RL4, REG_RL5, REG_RL6, REG_RL7,
};

#define dregs_lo(x) REGNAME(decode_dregs_lo[(x) & 7])

/* RH(0..7).  */
static enum machine_registers decode_dregs_hi[] =
{
  REG_RH0, REG_RH1, REG_RH2, REG_RH3, REG_RH4, REG_RH5, REG_RH6, REG_RH7,
};

#define dregs_hi(x) REGNAME(decode_dregs_hi[(x) & 7])

/* R(0..7).  */
static enum machine_registers decode_dregs[] =
{
  REG_R0, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7,
};

#define dregs(x) REGNAME(decode_dregs[(x) & 7])

/* R BYTE(0..7).  */
static enum machine_registers decode_dregs_byte[] =
{
  REG_BR0, REG_BR1, REG_BR2, REG_BR3, REG_BR4, REG_BR5, REG_BR6, REG_BR7,
};

#define dregs_byte(x) REGNAME(decode_dregs_byte[(x) & 7])
#define dregs_pair(x) REGNAME(decode_dregs_pair[(x) & 7])

/* P(0..5) SP FP.  */
static enum machine_registers decode_pregs[] =
{
  REG_P0, REG_P1, REG_P2, REG_P3, REG_P4, REG_P5, REG_SP, REG_FP,
};

#define pregs(x)	REGNAME(decode_pregs[(x) & 7])
#define spfp(x)		REGNAME(decode_spfp[(x) & 1])
#define dregs_hilo(x,i)	REGNAME(decode_dregs_hilo[((i)<<3)|x])
#define accum_ext(x)	REGNAME(decode_accum_ext[(x) & 1])
#define accum_word(x)	REGNAME(decode_accum_word[(x) & 1])
#define accum(x)	REGNAME(decode_accum[(x) & 1])

/* I(0..3).  */
static enum machine_registers decode_iregs[] =
{
  REG_I0, REG_I1, REG_I2, REG_I3,
};

#define iregs(x) REGNAME(decode_iregs[(x) & 3])

/* M(0..3).  */
static enum machine_registers decode_mregs[] =
{
  REG_M0, REG_M1, REG_M2, REG_M3,
};

#define mregs(x) REGNAME(decode_mregs[(x) & 3])
#define bregs(x) REGNAME(decode_bregs[(x) & 3])
#define lregs(x) REGNAME(decode_lregs[(x) & 3])

/* dregs pregs.  */
static enum machine_registers decode_dpregs[] =
{
  REG_R0, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7,
  REG_P0, REG_P1, REG_P2, REG_P3, REG_P4, REG_P5, REG_SP, REG_FP,
};

#define dpregs(x) REGNAME(decode_dpregs[(x) & 15])

/* [dregs pregs].  */
static enum machine_registers decode_gregs[] =
{
  REG_R0, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7,
  REG_P0, REG_P1, REG_P2, REG_P3, REG_P4, REG_P5, REG_SP, REG_FP,
};

#define gregs(x,i) REGNAME(decode_gregs[((i)<<3)|x])

/* [dregs pregs (iregs mregs) (bregs lregs)].  */
static enum machine_registers decode_regs[] =
{
  REG_R0, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7,
  REG_P0, REG_P1, REG_P2, REG_P3, REG_P4, REG_P5, REG_SP, REG_FP,
  REG_I0, REG_I1, REG_I2, REG_I3, REG_M0, REG_M1, REG_M2, REG_M3,
  REG_B0, REG_B1, REG_B2, REG_B3, REG_L0, REG_L1, REG_L2, REG_L3,
};

#define regs(x,i) REGNAME(decode_regs[((i)<<3)|x])

/* [dregs pregs (iregs mregs) (bregs lregs) Low Half].  */
static enum machine_registers decode_regs_lo[] =
{
  REG_RL0, REG_RL1, REG_RL2, REG_RL3, REG_RL4, REG_RL5, REG_RL6, REG_RL7,
  REG_PL0, REG_PL1, REG_PL2, REG_PL3, REG_PL4, REG_PL5, REG_SLP, REG_FLP,
  REG_IL0, REG_IL1, REG_IL2, REG_IL3, REG_ML0, REG_ML1, REG_ML2, REG_ML3,
  REG_BL0, REG_BL1, REG_BL2, REG_BL3, REG_LL0, REG_LL1, REG_LL2, REG_LL3,
};

#define regs_lo(x,i) REGNAME(decode_regs_lo[((i)<<3)|x])
/* [dregs pregs (iregs mregs) (bregs lregs) High Half].  */
static enum machine_registers decode_regs_hi[] =
{
  REG_RH0, REG_RH1, REG_RH2, REG_RH3, REG_RH4, REG_RH5, REG_RH6, REG_RH7,
  REG_PH0, REG_PH1, REG_PH2, REG_PH3, REG_PH4, REG_PH5, REG_SHP, REG_FHP,
  REG_IH0, REG_IH1, REG_IH2, REG_IH3, REG_MH0, REG_MH1, REG_LH2, REG_MH3,
  REG_BH0, REG_BH1, REG_BH2, REG_BH3, REG_LH0, REG_LH1, REG_LH2, REG_LH3,
};

#define regs_hi(x,i) REGNAME(decode_regs_hi[((i)<<3)|x])

static enum machine_registers decode_statbits[] =
{
  REG_AZ, REG_AN, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_AQ, REG_LASTREG,
  REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_AC0, REG_AC1, REG_LASTREG, REG_LASTREG,
  REG_AV0, REG_AV0S, REG_AV1, REG_AV1S, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG,
  REG_V, REG_VS, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG,
};

#define statbits(x)	REGNAME(decode_statbits[(x) & 31])
#define ignore_bits(x)	REGNAME(decode_ignore_bits[(x) & 7])
#define ccstat(x)	REGNAME(decode_ccstat[(x) & 0])

/* LC0 LC1.  */
static enum machine_registers decode_counters[] =
{
  REG_LC0, REG_LC1,
};

#define counters(x) REGNAME(decode_counters[(x) & 1])
#define dregs2_sysregs1(x) REGNAME(decode_dregs2_sysregs1[(x) & 7])

/* [dregs pregs (iregs mregs) (bregs lregs)
   dregs2_sysregs1 open sysregs2 sysregs3].  */
static enum machine_registers decode_allregs[] =
{
  REG_R0, REG_R1, REG_R2, REG_R3, REG_R4, REG_R5, REG_R6, REG_R7,
  REG_P0, REG_P1, REG_P2, REG_P3, REG_P4, REG_P5, REG_SP, REG_FP,
  REG_I0, REG_I1, REG_I2, REG_I3, REG_M0, REG_M1, REG_M2, REG_M3,
  REG_B0, REG_B1, REG_B2, REG_B3, REG_L0, REG_L1, REG_L2, REG_L3,
  REG_A0x, REG_A0w, REG_A1x, REG_A1w, REG_GP, REG_LASTREG, REG_ASTAT, REG_RETS,
  REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG, REG_LASTREG,
  REG_LC0, REG_LT0, REG_LB0, REG_LC1, REG_LT1, REG_LB1, REG_CYCLES, REG_CYCLES2,
  REG_USP, REG_SEQSTAT, REG_SYSCFG, REG_RETI, REG_RETX, REG_RETN, REG_RETE, REG_EMUDAT, REG_LASTREG,
};

#define allregs(x,i)	REGNAME(decode_allregs[((i) << 3) | x])
#define uimm16s4(x)	fmtconst(c_uimm16s4, x, 0, outf)
#define pcrel4(x)	fmtconst(c_pcrel4, x, pc, outf)
#define pcrel8(x)	fmtconst(c_pcrel8, x, pc, outf)
#define pcrel8s4(x)	fmtconst(c_pcrel8s4, x, pc, outf)
#define pcrel10(x)	fmtconst(c_pcrel10, x, pc, outf)
#define pcrel12(x)	fmtconst(c_pcrel12, x, pc, outf)
#define negimm5s4(x)	fmtconst(c_negimm5s4, x, 0, outf)
#define rimm16(x)	fmtconst(c_rimm16, x, 0, outf)
#define huimm16(x)	fmtconst(c_huimm16, x, 0, outf)
#define imm16(x)	fmtconst(c_imm16, x, 0, outf)
#define uimm2(x)	fmtconst(c_uimm2, x, 0, outf)
#define uimm3(x)	fmtconst(c_uimm3, x, 0, outf)
#define luimm16(x)	fmtconst(c_luimm16, x, 0, outf)
#define uimm4(x)	fmtconst(c_uimm4, x, 0, outf)
#define uimm5(x)	fmtconst(c_uimm5, x, 0, outf)
#define imm16s2(x)	fmtconst(c_imm16s2, x, 0, outf)
#define uimm8(x)	fmtconst(c_uimm8, x, 0, outf)
#define imm16s4(x)	fmtconst(c_imm16s4, x, 0, outf)
#define uimm4s2(x)	fmtconst(c_uimm4s2, x, 0, outf)
#define uimm4s4(x)	fmtconst(c_uimm4s4, x, 0, outf)
#define lppcrel10(x)	fmtconst(c_lppcrel10, x, pc, outf)
#define imm3(x)		fmtconst(c_imm3, x, 0, outf)
#define imm4(x)		fmtconst(c_imm4, x, 0, outf)
#define uimm8s4(x)	fmtconst(c_uimm8s4, x, 0, outf)
#define imm5(x)		fmtconst(c_imm5, x, 0, outf)
#define imm6(x)		fmtconst(c_imm6, x, 0, outf)
#define imm7(x)		fmtconst(c_imm7, x, 0, outf)
#define imm8(x)		fmtconst(c_imm8, x, 0, outf)
#define pcrel24(x)	fmtconst(c_pcrel24, x, pc, outf)
#define uimm16(x)	fmtconst(c_uimm16, x, 0, outf)

/* (arch.pm)arch_disassembler_functions.  */
#define notethat(x)

#ifndef OUTS
#define OUTS(p,txt) ((p) ? (((txt)[0]) ? (p->fprintf_func)(p->stream, txt) :0) :0)
#endif


static void
amod0 (int s0, int x0, disassemble_info *outf)
{
  if (s0 == 0 && x0 == 0)
    {
      notethat ("(NS)");
      return;
    }
  else if (s0 == 1 && x0 == 0)
    {
      notethat ("(S)");
      OUTS (outf, "(S)");
      return;
    }
  else if (s0 == 0 && x0 == 1)
    {
      notethat ("(CO)");
      OUTS (outf, "(CO)");
      return;
    }
  else if (s0 == 1 && x0 == 1)
    {
      notethat ("(SCO)");
      OUTS (outf, "(SCO)");
      return;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return;
}

static void
amod1 (int s0, int x0, disassemble_info *outf)
{
  if (s0 == 0 && x0 == 0)
    {
      notethat ("(NS)");
      OUTS (outf, "(NS)");
      return;
    }
  else if (s0 == 1 && x0 == 0)
    {
      notethat ("(S)");
      OUTS (outf, "(S)");
      return;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return;
}

static void
amod0amod2 (int s0, int x0, int aop0, disassemble_info *outf)
{
  if (s0 == 0 && x0 == 0 && aop0 == 0)
    {
      notethat ("(NS)");
      return;
    }
  else if (s0 == 1 && x0 == 0 && aop0 == 0)
    {
      notethat ("(S)");
      OUTS (outf, "(S)");
      return;
    }
  else if (s0 == 0 && x0 == 1 && aop0 == 0)
    {
      notethat ("(CO)");
      OUTS (outf, "(CO)");
      return;
    }
  else if (s0 == 1 && x0 == 1 && aop0 == 0)
    {
      notethat ("(SCO)");
      OUTS (outf, "(SCO)");
      return;
    }
  else if (s0 == 0 && x0 == 0 && aop0 == 2)
    {
      notethat ("(ASR)");
      OUTS (outf, "(ASR)");
      return;
    }
  else if (s0 == 1 && x0 == 0 && aop0 == 2)
    {
      notethat ("(S,ASR)");
      OUTS (outf, "(S,ASR)");
      return;
    }
  else if (s0 == 0 && x0 == 1 && aop0 == 2)
    {
      notethat ("(CO,ASR)");
      OUTS (outf, "(CO,ASR)");
      return;
    }
  else if (s0 == 1 && x0 == 1 && aop0 == 2)
    {
      notethat ("(SCO,ASR)");
      OUTS (outf, "(SCO,ASR)");
      return;
    }
  else if (s0 == 0 && x0 == 0 && aop0 == 3)
    {
      notethat ("(ASL)");
      OUTS (outf, "(ASL)");
      return;
    }
  else if (s0 == 1 && x0 == 0 && aop0 == 3)
    {
      notethat ("(S,ASL)");
      OUTS (outf, "(S,ASL)");
      return;
    }
  else if (s0 == 0 && x0 == 1 && aop0 == 3)
    {
      notethat ("(CO,ASL)");
      OUTS (outf, "(CO,ASL)");
      return;
    }
  else if (s0 == 1 && x0 == 1 && aop0 == 3)
    {
      notethat ("(SCO,ASL)");
      OUTS (outf, "(SCO,ASL)");
      return;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return;
}

static void
searchmod (int r0, disassemble_info *outf)
{
  if ((r0 == 0))
    {
      notethat ("GT");
      OUTS (outf, "GT");
      return;
    }
  else if ((r0 == 1))
    {
      notethat ("GE");
      OUTS (outf, "GE");
      return;
    }
  else if ((r0 == 2))
    {
      notethat ("LT");
      OUTS (outf, "LT");
      return;
    }
  else if ((r0 == 3))
    {
      notethat ("LE");
      OUTS (outf, "LE");
      return;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return;
}

static void
aligndir (int r0, disassemble_info *outf)
{
  if ((r0 == 0))
    {
      notethat ("");
      return;
    }
  else if ((r0 == 1))
    {
      notethat ("(R)");
      OUTS (outf, "(R)");
      return;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return;
}

static int
decode_multfunc (int h0, int h1, int src0, int src1, disassemble_info * outf)
{
  char *s0, *s1;

  if (h0)
    s0 = dregs_hi (src0);
  else
    s0 = dregs_lo (src0);

  if (h1)
    s1 = dregs_hi (src1);
  else
    s1 = dregs_lo (src1);

  OUTS (outf, s0);
  OUTS (outf, " * ");
  OUTS (outf, s1);
  return 0;
}

static int
decode_macfunc (int which, int op, int h0, int h1, int src0, int src1, disassemble_info * outf)
{
  char *a;
  char *sop = "<unknown op>";

  if (which)
    a = "a1";
  else
    a = "a0";

  if (op == 3)
    {
      OUTS (outf, a);
      return 0;
    }

  switch (op)
    {
    case 0:
      sop = "=";
      break;
    case 1:
      sop = "+=";
      break;
    case 2:
      sop = "-=";
      break;
    }

  OUTS (outf, a);
  OUTS (outf, " ");
  OUTS (outf, sop);
  OUTS (outf, " ");
  decode_multfunc (h0, h1, src0, src1, outf);

  return 0;
}

static void
decode_optmode (int mod, int MM, disassemble_info *outf)
{
  if (mod == 0 && MM == 0)
    return;

  OUTS (outf, " (");

  if (MM && !mod)
    {
      OUTS (outf, "M)");
      return;
    }

  if (MM)
    OUTS (outf, "M, ");
  
  if (mod == M_S2RND)
    OUTS (outf, "S2RND");
  else if (mod == M_T)
    OUTS (outf, "T");
  else if (mod == M_W32)
    OUTS (outf, "W32");
  else if (mod == M_FU)
    OUTS (outf, "FU");
  else if (mod == M_TFU)
    OUTS (outf, "TFU");
  else if (mod == M_IS)
    OUTS (outf, "IS");
  else if (mod == M_ISS2)
    OUTS (outf, "ISS2");
  else if (mod == M_IH)
    OUTS (outf, "IH");
  else if (mod == M_IU)
    OUTS (outf, "IU");
  else
    abort ();

  OUTS (outf, ")");
}
static int
decode_ProgCtrl_0 (TIword iw0, disassemble_info *outf)
{
/* ProgCtrl
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |.prgfunc.......|.poprnd........|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int poprnd  = ((iw0 >> ProgCtrl_poprnd_bits) & ProgCtrl_poprnd_mask);
  int prgfunc = ((iw0 >> ProgCtrl_prgfunc_bits) & ProgCtrl_prgfunc_mask);

  if (prgfunc == 0 && poprnd == 0)
    {
      notethat ("NOP");
      OUTS (outf, "NOP");
      return 1 * 2;
    }
  else if (prgfunc == 1 && poprnd == 0)
    {
      notethat ("RTS");
      OUTS (outf, "RTS");
      return 1 * 2;
    }
  else if (prgfunc == 1 && poprnd == 1)
    {
      notethat ("RTI");
      OUTS (outf, "RTI");
      return 1 * 2;
    }
  else if (prgfunc == 1 && poprnd == 2)
    {
      notethat ("RTX");
      OUTS (outf, "RTX");
      return 1 * 2;
    }
  else if (prgfunc == 1 && poprnd == 3)
    {
      notethat ("RTN");
      OUTS (outf, "RTN");
      return 1 * 2;
    }
  else if (prgfunc == 1 && poprnd == 4)
    {
      notethat ("RTE");
      OUTS (outf, "RTE");
      return 1 * 2;
    }
  else if (prgfunc == 2 && poprnd == 0)
    {
      notethat ("IDLE");
      OUTS (outf, "IDLE");
      return 1 * 2;
    }
  else if (prgfunc == 2 && poprnd == 3)
    {
      notethat ("CSYNC");
      OUTS (outf, "CSYNC");
      return 1 * 2;
    }
  else if (prgfunc == 2 && poprnd == 4)
    {
      notethat ("SSYNC");
      OUTS (outf, "SSYNC");
      return 1 * 2;
    }
  else if (prgfunc == 2 && poprnd == 5)
    {
      notethat ("EMUEXCPT");
      OUTS (outf, "EMUEXCPT");
      return 1 * 2;
    }
  else if (prgfunc == 3)
    {
      notethat ("CLI dregs");
      OUTS (outf, "CLI  ");
      OUTS (outf, dregs (poprnd));
      return 1 * 2;
    }
  else if (prgfunc == 4)
    {
      notethat ("STI dregs");
      OUTS (outf, "STI  ");
      OUTS (outf, dregs (poprnd));
      return 1 * 2;
    }
  else if (prgfunc == 5)
    {
      notethat ("JUMP ( pregs )");
      OUTS (outf, "JUMP  (");
      OUTS (outf, pregs (poprnd));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (prgfunc == 6)
    {
      notethat ("CALL ( pregs )");
      OUTS (outf, "CALL  (");
      OUTS (outf, pregs (poprnd));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (prgfunc == 7)
    {
      notethat ("CALL ( PC + pregs )");
      OUTS (outf, "CALL  (PC+");
      OUTS (outf, pregs (poprnd));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (prgfunc == 8)
    {
      notethat ("JUMP ( PC + pregs )");
      OUTS (outf, "JUMP  (PC+");
      OUTS (outf, pregs (poprnd));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (prgfunc == 9)
    {
      notethat ("RAISE uimm4");
      OUTS (outf, "RAISE  ");
      OUTS (outf, uimm4 (poprnd));
      return 1 * 2;
    }
  else if (prgfunc == 10)
    {
      notethat ("EXCPT uimm4");
      OUTS (outf, "EXCPT  ");
      OUTS (outf, uimm4 (poprnd));
      return 1 * 2;
    }
  else if (prgfunc == 11)
    {
      notethat ("TESTSET ( pregs )");
      OUTS (outf, "TESTSET  (");
      OUTS (outf, pregs (poprnd));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_CaCTRL_0 (TIword iw0, disassemble_info *outf)
{
/* CaCTRL
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 1 |.a.|.op....|.reg.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int a   = ((iw0 >> CaCTRL_a_bits) & CaCTRL_a_mask);
  int op  = ((iw0 >> CaCTRL_op_bits) & CaCTRL_op_mask);
  int reg = ((iw0 >> CaCTRL_reg_bits) & CaCTRL_reg_mask);

  if (a == 0 && op == 0)
    {
      notethat ("PREFETCH [ pregs ]");
      OUTS (outf, "PREFETCH[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (a == 0 && op == 1)
    {
      notethat ("FLUSHINV [ pregs ]");
      OUTS (outf, "FLUSHINV[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (a == 0 && op == 2)
    {
      notethat ("FLUSH [ pregs ]");
      OUTS (outf, "FLUSH[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (a == 0 && op == 3)
    {
      notethat ("IFLUSH [ pregs ]");
      OUTS (outf, "IFLUSH[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (a == 1 && op == 0)
    {
      notethat ("PREFETCH [ pregs ++ ]");
      OUTS (outf, "PREFETCH[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (a == 1 && op == 1)
    {
      notethat ("FLUSHINV [ pregs ++ ]");
      OUTS (outf, "FLUSHINV[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (a == 1 && op == 2)
    {
      notethat ("FLUSH [ pregs ++ ]");
      OUTS (outf, "FLUSH[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (a == 1 && op == 3)
    {
      notethat ("IFLUSH [ pregs ++ ]");
      OUTS (outf, "IFLUSH[");
      OUTS (outf, pregs (reg));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_PushPopReg_0 (TIword iw0, disassemble_info *outf)
{

/* PushPopReg
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 0 | 0 | 0 | 1 | 0 |.W.|.grp.......|.reg.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int W   = ((iw0 >> PushPopReg_W_bits) & PushPopReg_W_mask);
  int grp = ((iw0 >> PushPopReg_grp_bits) & PushPopReg_grp_mask);
  int reg = ((iw0 >> PushPopReg_reg_bits) & PushPopReg_reg_mask);

  if (W == 0)
    {
      notethat ("allregs = [ SP ++ ]");
      OUTS (outf, allregs (reg, grp));
      OUTS (outf, " = [SP++]");
      return 1 * 2;
    }
  else if (W == 1)
    {
      notethat ("[ -- SP ] = allregs");
      OUTS (outf, "[--SP] = ");
      OUTS (outf, allregs (reg, grp));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_PushPopMultiple_0 (TIword iw0, disassemble_info *outf)
{
/* PushPopMultiple
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 0 | 1 | 0 |.d.|.p.|.W.|.dr........|.pr........|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int p  = ((iw0 >> PushPopMultiple_p_bits) & PushPopMultiple_p_mask);
  int d  = ((iw0 >> PushPopMultiple_d_bits) & PushPopMultiple_d_mask);
  int W  = ((iw0 >> PushPopMultiple_W_bits) & PushPopMultiple_W_mask);
  int dr = ((iw0 >> PushPopMultiple_dr_bits) & PushPopMultiple_dr_mask);
  int pr = ((iw0 >> PushPopMultiple_pr_bits) & PushPopMultiple_pr_mask);

  char ps[5], ds[5];
  sprintf (ps, "%d", pr);
  sprintf (ds, "%d", dr);

  if (W == 1 && d == 1 && p == 1)
    {
      notethat ("[ -- SP ] = ( R7 : reglim , P5 : reglim )");
      OUTS (outf, "[--SP] = (R7:");
      OUTS (outf, ds);
      OUTS (outf, ", P5:");
      OUTS (outf, ps);
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (W == 1 && d == 1 && p == 0)
    {
      notethat ("[ -- SP ] = ( R7 : reglim )");
      OUTS (outf, "[--SP] = (R7:");
      OUTS (outf, ds);
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (W == 1 && d == 0 && p == 1)
    {
      notethat ("[ -- SP ] = ( P5 : reglim )");
      OUTS (outf, "[--SP] = (P5:");
      OUTS (outf, ps);
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (W == 0 && d == 1 && p == 1)
    {
      notethat ("( R7 : reglim , P5 : reglim ) = [ SP ++ ]");
      OUTS (outf, "(R7:");
      OUTS (outf, ds);
      OUTS (outf, ", P5:");
      OUTS (outf, ps);
      OUTS (outf, ") = [SP++]");
      return 1 * 2;
    }
  else if (W == 0 && d == 1 && p == 0)
    {
      notethat ("( R7 : reglim ) = [ SP ++ ]");
      OUTS (outf, "(R7:");
      OUTS (outf, ds);
      OUTS (outf, ") = [SP++]");
      return 1 * 2;
    }
  else if (W == 0 && d == 0 && p == 1)
    {
      notethat ("( P5 : reglim ) = [ SP ++ ]");
      OUTS (outf, "(P5:");
      OUTS (outf, ps);
      OUTS (outf, ") = [SP++]");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_ccMV_0 (TIword iw0, disassemble_info *outf)
{
/* ccMV
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 0 | 1 | 1 |.T.|.d.|.s.|.dst.......|.src.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int s  = ((iw0 >> CCmv_s_bits) & CCmv_s_mask);
  int d  = ((iw0 >> CCmv_d_bits) & CCmv_d_mask);
  int T  = ((iw0 >> CCmv_T_bits) & CCmv_T_mask);
  int src = ((iw0 >> CCmv_src_bits) & CCmv_src_mask);
  int dst = ((iw0 >> CCmv_dst_bits) & CCmv_dst_mask);

  if (T == 1)
    {
      notethat ("IF CC gregs = gregs");
      OUTS (outf, "IF CC ");
      OUTS (outf, gregs (dst, d));
      OUTS (outf, " = ");
      OUTS (outf, gregs (src, s));
      return 1 * 2;
    }
  else if (T == 0)
    {
      notethat ("IF ! CC gregs = gregs");
      OUTS (outf, "IF ! CC ");
      OUTS (outf, gregs (dst, d));
      OUTS (outf, " = ");
      OUTS (outf, gregs (src, s));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_CCflag_0 (TIword iw0, disassemble_info *outf)
{
/* CCflag
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 1 |.I.|.opc.......|.G.|.y.........|.x.........|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int x = ((iw0 >> CCflag_x_bits) & CCflag_x_mask);
  int y = ((iw0 >> CCflag_y_bits) & CCflag_y_mask);
  int I = ((iw0 >> CCflag_I_bits) & CCflag_I_mask);
  int G = ((iw0 >> CCflag_G_bits) & CCflag_G_mask);
  int opc = ((iw0 >> CCflag_opc_bits) & CCflag_opc_mask);

  if (opc == 0 && I == 0 && G == 0)
    {
      notethat ("CC = dregs == dregs");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "==");
      OUTS (outf, dregs (y));
      return 1 * 2;
    }
  else if (opc == 1 && I == 0 && G == 0)
    {
      notethat ("CC = dregs < dregs");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<");
      OUTS (outf, dregs (y));
      return 1 * 2;
    }
  else if (opc == 2 && I == 0 && G == 0)
    {
      notethat ("CC = dregs <= dregs");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<=");
      OUTS (outf, dregs (y));
      return 1 * 2;
    }
  else if (opc == 3 && I == 0 && G == 0)
    {
      notethat ("CC = dregs < dregs ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<");
      OUTS (outf, dregs (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 4 && I == 0 && G == 0)
    {
      notethat ("CC = dregs <= dregs ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<=");
      OUTS (outf, dregs (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 0 && I == 1 && G == 0)
    {
      notethat ("CC = dregs == imm3");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "==");
      OUTS (outf, imm3 (y));
      return 1 * 2;
    }
  else if (opc == 1 && I == 1 && G == 0)
    {
      notethat ("CC = dregs < imm3");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<");
      OUTS (outf, imm3 (y));
      return 1 * 2;
    }
  else if (opc == 2 && I == 1 && G == 0)
    {
      notethat ("CC = dregs <= imm3");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<=");
      OUTS (outf, imm3 (y));
      return 1 * 2;
    }
  else if (opc == 3 && I == 1 && G == 0)
    {
      notethat ("CC = dregs < uimm3 ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<");
      OUTS (outf, uimm3 (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 4 && I == 1 && G == 0)
    {
      notethat ("CC = dregs <= uimm3 ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (x));
      OUTS (outf, "<=");
      OUTS (outf, uimm3 (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 0 && I == 0 && G == 1)
    {
      notethat ("CC = pregs == pregs");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "==");
      OUTS (outf, pregs (y));
      return 1 * 2;
    }
  else if (opc == 1 && I == 0 && G == 1)
    {
      notethat ("CC = pregs < pregs");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<");
      OUTS (outf, pregs (y));
      return 1 * 2;
    }
  else if (opc == 2 && I == 0 && G == 1)
    {
      notethat ("CC = pregs <= pregs");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<=");
      OUTS (outf, pregs (y));
      return 1 * 2;
    }
  else if (opc == 3 && I == 0 && G == 1)
    {
      notethat ("CC = pregs < pregs ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<");
      OUTS (outf, pregs (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 4 && I == 0 && G == 1)
    {
      notethat ("CC = pregs <= pregs ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<=");
      OUTS (outf, pregs (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 0 && I == 1 && G == 1)
    {
      notethat ("CC = pregs == imm3");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "==");
      OUTS (outf, imm3 (y));
      return 1 * 2;
    }
  else if (opc == 1 && I == 1 && G == 1)
    {
      notethat ("CC = pregs < imm3");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<");
      OUTS (outf, imm3 (y));
      return 1 * 2;
    }
  else if (opc == 2 && I == 1 && G == 1)
    {
      notethat ("CC = pregs <= imm3");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<=");
      OUTS (outf, imm3 (y));
      return 1 * 2;
    }
  else if (opc == 3 && I == 1 && G == 1)
    {
      notethat ("CC = pregs < uimm3 ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<");
      OUTS (outf, uimm3 (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 4 && I == 1 && G == 1)
    {
      notethat ("CC = pregs <= uimm3 ( IU )");
      OUTS (outf, "CC=");
      OUTS (outf, pregs (x));
      OUTS (outf, "<=");
      OUTS (outf, uimm3 (y));
      OUTS (outf, "(IU)");
      return 1 * 2;
    }
  else if (opc == 5 && I == 0 && G == 0)
    {
      notethat ("CC = A0 == A1");
      OUTS (outf, "CC=A0==A1");
      return 1 * 2;
    }
  else if (opc == 6 && I == 0 && G == 0)
    {
      notethat ("CC = A0 < A1");
      OUTS (outf, "CC=A0<A1");
      return 1 * 2;
    }
  else if (opc == 7 && I == 0 && G == 0)
    {
      notethat ("CC = A0 <= A1");
      OUTS (outf, "CC=A0<=A1");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_CC2dreg_0 (TIword iw0, disassemble_info *outf)
{
/* CC2dreg
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0 | 0 |.op....|.reg.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int op  = ((iw0 >> CC2dreg_op_bits) & CC2dreg_op_mask);
  int reg = ((iw0 >> CC2dreg_reg_bits) & CC2dreg_reg_mask);

  if (op == 0)
    {
      notethat ("dregs = CC");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=CC");
      return 1 * 2;
    }
  else if (op == 1)
    {
      notethat ("CC = dregs");
      OUTS (outf, "CC=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (op == 3)
    {
      notethat ("CC =! CC");
      OUTS (outf, "CC=!CC");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_CC2stat_0 (TIword iw0, disassemble_info *outf)
{
/* CC2stat
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 0 | 0 | 0 | 1 | 1 |.D.|.op....|.cbit..............|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int D    = ((iw0 >> CC2stat_D_bits) & CC2stat_D_mask);
  int op   = ((iw0 >> CC2stat_op_bits) & CC2stat_op_mask);
  int cbit = ((iw0 >> CC2stat_cbit_bits) & CC2stat_cbit_mask);

  if (op == 0 && D == 0)
    {
      notethat ("CC = statbits");
      OUTS (outf, "CC = ");
      OUTS (outf, statbits (cbit));
      return 1 * 2;
    }
  else if (op == 1 && D == 0)
    {
      notethat ("CC |= statbits");
      OUTS (outf, "CC|=");
      OUTS (outf, statbits (cbit));
      return 1 * 2;
    }
  else if (op == 2 && D == 0)
    {
      notethat ("CC &= statbits");
      OUTS (outf, "CC&=");
      OUTS (outf, statbits (cbit));
      return 1 * 2;
    }
  else if (op == 3 && D == 0)
    {
      notethat ("CC ^= statbits");
      OUTS (outf, "CC^=");
      OUTS (outf, statbits (cbit));
      return 1 * 2;
    }
  else if (op == 0 && D == 1)
    {
      notethat ("statbits = CC");
      OUTS (outf, statbits (cbit));
      OUTS (outf, "=CC");
      return 1 * 2;
    }
  else if (op == 1 && D == 1)
    {
      notethat ("statbits |= CC");
      OUTS (outf, statbits (cbit));
      OUTS (outf, "|=CC");
      return 1 * 2;
    }
  else if (op == 2 && D == 1)
    {
      notethat ("statbits &= CC");
      OUTS (outf, statbits (cbit));
      OUTS (outf, "&=CC");
      return 1 * 2;
    }
  else if (op == 3 && D == 1)
    {
      notethat ("statbits ^= CC");
      OUTS (outf, statbits (cbit));
      OUTS (outf, "^=CC");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_BRCC_0 (TIword iw0, bfd_vma pc, disassemble_info *outf)
{
/* BRCC
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 0 | 1 |.T.|.B.|.offset................................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int B = ((iw0 >> BRCC_B_bits) & BRCC_B_mask);
  int T = ((iw0 >> BRCC_T_bits) & BRCC_T_mask);
  int offset = ((iw0 >> BRCC_offset_bits) & BRCC_offset_mask);

  if (T == 1 && B == 1)
    {
      notethat ("IF CC JUMP pcrel10 ( BP )");
      OUTS (outf, "IF CC JUMP ");
      OUTS (outf, pcrel10 (offset));
      OUTS (outf, "(BP)");
      return 1 * 2;
    }
  else if (T == 0 && B == 1)
    {
      notethat ("IF !CC JUMP pcrel10 ( BP )");
      OUTS (outf, "IF ! CC JUMP ");
      OUTS (outf, pcrel10 (offset));
      OUTS (outf, "(BP)");
      return 1 * 2;
    }
  else if (T == 1)
    {
      notethat ("IF CC JUMP pcrel10");
      OUTS (outf, "IF CC JUMP ");
      OUTS (outf, pcrel10 (offset));
      return 1 * 2;
    }
  else if (T == 0)
    {
      notethat ("IF !CC JUMP pcrel10");
      OUTS (outf, "IF ! CC JUMP ");
      OUTS (outf, pcrel10 (offset));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_UJUMP_0 (TIword iw0, bfd_vma pc, disassemble_info *outf)
{
/* UJUMP
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 1 | 0 |.offset........................................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int offset = ((iw0 >> UJump_offset_bits) & UJump_offset_mask);

  notethat ("JUMP.S pcrel12");
  OUTS (outf, "JUMP.S  ");
  OUTS (outf, pcrel12 (offset));
  return 1 * 2;
}

static int
decode_REGMV_0 (TIword iw0, disassemble_info *outf)
{
/* REGMV
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 0 | 1 | 1 |.gd........|.gs........|.dst.......|.src.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int gs  = ((iw0 >> RegMv_gs_bits) & RegMv_gs_mask);
  int gd  = ((iw0 >> RegMv_gd_bits) & RegMv_gd_mask);
  int src = ((iw0 >> RegMv_src_bits) & RegMv_src_mask);
  int dst = ((iw0 >> RegMv_dst_bits) & RegMv_dst_mask);

  notethat ("allregs = allregs");
  OUTS (outf, allregs (dst, gd));
  OUTS (outf, "=");
  OUTS (outf, allregs (src, gs));
  return 1 * 2;
}

static int
decode_ALU2op_0 (TIword iw0, disassemble_info *outf)
{
/* ALU2op
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 1 | 0 | 0 | 0 | 0 |.opc...........|.src.......|.dst.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int src = ((iw0 >> ALU2op_src_bits) & ALU2op_src_mask);
  int opc = ((iw0 >> ALU2op_opc_bits) & ALU2op_opc_mask);
  int dst = ((iw0 >> ALU2op_dst_bits) & ALU2op_dst_mask);

  if (opc == 0)
    {
      notethat ("dregs >>>= dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, ">>>=");
      OUTS (outf, dregs (src));
      return 1 * 2;
    }
  else if (opc == 1)
    {
      notethat ("dregs >>= dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, ">>=");
      OUTS (outf, dregs (src));
      return 1 * 2;
    }
  else if (opc == 2)
    {
      notethat ("dregs <<= dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "<<=");
      OUTS (outf, dregs (src));
      return 1 * 2;
    }
  else if (opc == 3)
    {
      notethat ("dregs *= dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "*=");
      OUTS (outf, dregs (src));
      return 1 * 2;
    }
  else if (opc == 4)
    {
      notethat ("dregs = (dregs + dregs) << 1");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=(");
      OUTS (outf, dregs (dst));
      OUTS (outf, "+");
      OUTS (outf, dregs (src));
      OUTS (outf, ")<<1");
      return 1 * 2;
    }
  else if (opc == 5)
    {
      notethat ("dregs = (dregs + dregs) << 2");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=(");
      OUTS (outf, dregs (dst));
      OUTS (outf, "+");
      OUTS (outf, dregs (src));
      OUTS (outf, ")<<2");
      return 1 * 2;
    }
  else if (opc == 8)
    {
      notethat ("DIVQ (dregs , dregs)");
      OUTS (outf, "DIVQ(");
      OUTS (outf, dregs (dst));
      OUTS (outf, ",");
      OUTS (outf, dregs (src));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (opc == 9)
    {
      notethat ("DIVS (dregs , dregs)");
      OUTS (outf, "DIVS(");
      OUTS (outf, dregs (dst));
      OUTS (outf, ",");
      OUTS (outf, dregs (src));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (opc == 10)
    {
      notethat ("dregs = dregs_lo (X)");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src));
      OUTS (outf, "(X)");
      return 1 * 2;
    }
  else if (opc == 11)
    {
      notethat ("dregs = dregs_lo (Z)");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src));
      OUTS (outf, "(Z)");
      return 1 * 2;
    }
  else if (opc == 12)
    {
      notethat ("dregs = dregs_byte (X)");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs_byte (src));
      OUTS (outf, "(X)");
      return 1 * 2;
    }
  else if (opc == 13)
    {
      notethat ("dregs = dregs_byte (Z)");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs_byte (src));
      OUTS (outf, "(Z)");
      return 1 * 2;
    }
  else if (opc == 14)
    {
      notethat ("dregs = - dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=-");
      OUTS (outf, dregs (src));
      return 1 * 2;
    }
  else if (opc == 15)
    {
      notethat ("dregs = ~ dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=~");
      OUTS (outf, dregs (src));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_PTR2op_0 (TIword iw0, disassemble_info *outf)
{
/* PTR2op
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 1 | 0 | 0 | 0 | 1 | 0 |.opc.......|.src.......|.dst.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int src = ((iw0 >> PTR2op_src_bits) & PTR2op_dst_mask);
  int opc = ((iw0 >> PTR2op_opc_bits) & PTR2op_opc_mask);
  int dst = ((iw0 >> PTR2op_dst_bits) & PTR2op_dst_mask);

  if (opc == 0)
    {
      notethat ("pregs -= pregs");
      OUTS (outf, pregs (dst));
      OUTS (outf, "-=");
      OUTS (outf, pregs (src));
      return 1 * 2;
    }
  else if (opc == 1)
    {
      notethat ("pregs = pregs << 2");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, pregs (src));
      OUTS (outf, "<<2");
      return 1 * 2;
    }
  else if (opc == 3)
    {
      notethat ("pregs = pregs >> 2");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, pregs (src));
      OUTS (outf, ">>2");
      return 1 * 2;
    }
  else if (opc == 4)
    {
      notethat ("pregs = pregs >> 1");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, pregs (src));
      OUTS (outf, ">>1");
      return 1 * 2;
    }
  else if (opc == 5)
    {
      notethat ("pregs += pregs ( BREV )");
      OUTS (outf, pregs (dst));
      OUTS (outf, "+=");
      OUTS (outf, pregs (src));
      OUTS (outf, "(BREV)");
      return 1 * 2;
    }
  else if (opc == 6)
    {
      notethat ("pregs = (pregs + pregs) << 1");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=(");
      OUTS (outf, pregs (dst));
      OUTS (outf, "+");
      OUTS (outf, pregs (src));
      OUTS (outf, ")<<1");
      return 1 * 2;
    }
  else if (opc == 7)
    {
      notethat ("pregs = (pregs + pregs) << 2");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=(");
      OUTS (outf, pregs (dst));
      OUTS (outf, "+");
      OUTS (outf, pregs (src));
      OUTS (outf, ")<<2");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LOGI2op_0 (TIword iw0, disassemble_info *outf)
{
/* LOGI2op
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 1 | 0 | 0 | 1 |.opc.......|.src...............|.dst.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int src = ((iw0 >> LOGI2op_src_bits) & LOGI2op_src_mask);
  int opc = ((iw0 >> LOGI2op_opc_bits) & LOGI2op_opc_mask);
  int dst = ((iw0 >> LOGI2op_dst_bits) & LOGI2op_dst_mask);

  if (opc == 0)
    {
      notethat ("CC = ! BITTST ( dregs , uimm5 )");
      OUTS (outf, "CC = ! BITTST (");
      OUTS (outf, dregs (dst));
      OUTS (outf, ",");
      OUTS (outf, uimm5 (src));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (opc == 1)
    {
      notethat ("CC = BITTST ( dregs , uimm5 )");
      OUTS (outf, "CC = BITTST (");
      OUTS (outf, dregs (dst));
      OUTS (outf, ",");
      OUTS (outf, uimm5 (src));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (opc == 2)
    {
      notethat ("BITSET ( dregs , uimm5 )");
      OUTS (outf, "BITSET (");
      OUTS (outf, dregs (dst));
      OUTS (outf, ",");
      OUTS (outf, uimm5 (src));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (opc == 3)
    {
      notethat ("BITTGL ( dregs , uimm5 )");
      OUTS (outf, "BITTGL (");
      OUTS (outf, dregs (dst));
      OUTS (outf, ",");
      OUTS (outf, uimm5 (src));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (opc == 4)
    {
      notethat ("BITCLR ( dregs , uimm5 )");
      OUTS (outf, "BITCLR (");
      OUTS (outf, dregs (dst));
      OUTS (outf, ",");
      OUTS (outf, uimm5 (src));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (opc == 5)
    {
      notethat ("dregs >>>= uimm5");
      OUTS (outf, dregs (dst));
      OUTS (outf, ">>>=");
      OUTS (outf, uimm5 (src));
      return 1 * 2;
    }
  else if (opc == 6)
    {
      notethat ("dregs >>= uimm5");
      OUTS (outf, dregs (dst));
      OUTS (outf, ">>=");
      OUTS (outf, uimm5 (src));
      return 1 * 2;
    }
  else if (opc == 7)
    {
      notethat ("dregs <<= uimm5");
      OUTS (outf, dregs (dst));
      OUTS (outf, "<<=");
      OUTS (outf, uimm5 (src));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_COMP3op_0 (TIword iw0, disassemble_info *outf)
{
/* COMP3op
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 1 | 0 | 1 |.opc.......|.dst.......|.src1......|.src0......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int opc  = ((iw0 >> COMP3op_opc_bits) & COMP3op_opc_mask);
  int dst  = ((iw0 >> COMP3op_dst_bits) & COMP3op_dst_mask);
  int src0 = ((iw0 >> COMP3op_src0_bits) & COMP3op_src0_mask);
  int src1 = ((iw0 >> COMP3op_src1_bits) & COMP3op_src1_mask);

  if (opc == 5 && src1 == src0)
    {
      notethat ("pregs = pregs << 1");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, pregs (src0));
      OUTS (outf, "<<1");
      return 1 * 2;
    }
  else if (opc == 1)
    {
      notethat ("dregs = dregs - dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs (src1));
      return 1 * 2;
    }
  else if (opc == 2)
    {
      notethat ("dregs = dregs & dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "&");
      OUTS (outf, dregs (src1));
      return 1 * 2;
    }
  else if (opc == 3)
    {
      notethat ("dregs = dregs | dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "|");
      OUTS (outf, dregs (src1));
      return 1 * 2;
    }
  else if (opc == 4)
    {
      notethat ("dregs = dregs ^ dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "^");
      OUTS (outf, dregs (src1));
      return 1 * 2;
    }
  else if (opc == 5)
    {
      notethat ("pregs = pregs + pregs");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, pregs (src0));
      OUTS (outf, "+");
      OUTS (outf, pregs (src1));
      return 1 * 2;
    }
  else if (opc == 6)
    {
      notethat ("pregs = pregs + (pregs << 1)");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, pregs (src0));
      OUTS (outf, "+(");
      OUTS (outf, pregs (src1));
      OUTS (outf, "<<1)");
      return 1 * 2;
    }
  else if (opc == 7)
    {
      notethat ("pregs = pregs + (pregs << 2)");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, pregs (src0));
      OUTS (outf, "+(");
      OUTS (outf, pregs (src1));
      OUTS (outf, "<<2)");
      return 1 * 2;
    }
  else if (opc == 0)
    {
      notethat ("dregs = dregs + dregs");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs (src1));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_COMPI2opD_0 (TIword iw0, disassemble_info *outf)
{
/* COMPI2opD
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 1 | 1 | 0 | 0 |.op|..src......................|.dst.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int op  = ((iw0 >> COMPI2opD_op_bits) & COMPI2opD_op_mask);
  int dst = ((iw0 >> COMPI2opD_dst_bits) & COMPI2opD_dst_mask);
  int src = ((iw0 >> COMPI2opD_src_bits) & COMPI2opD_src_mask);

  if (op == 0)
    {
      notethat ("dregs = imm7 (x)");
      OUTS (outf, dregs (dst));
      OUTS (outf, "=");
      OUTS (outf, imm7 (src));
      OUTS (outf, "(x)");
      return 1 * 2;
    }
  else if (op == 1)
    {
      notethat ("dregs += imm7");
      OUTS (outf, dregs (dst));
      OUTS (outf, "+=");
      OUTS (outf, imm7 (src));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_COMPI2opP_0 (TIword iw0, disassemble_info *outf)
{
/* COMPI2opP
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 0 | 1 | 1 | 0 | 1 |.op|.src.......................|.dst.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int op  = ((iw0 >> COMPI2opP_op_bits) & COMPI2opP_op_mask);
  int src = ((iw0 >> COMPI2opP_src_bits) & COMPI2opP_src_mask);
  int dst = ((iw0 >> COMPI2opP_dst_bits) & COMPI2opP_dst_mask);

  if (op == 0)
    {
      notethat ("pregs = imm7");
      OUTS (outf, pregs (dst));
      OUTS (outf, "=");
      OUTS (outf, imm7 (src));
      return 1 * 2;
    }
  else if (op == 1)
    {
      notethat ("pregs += imm7");
      OUTS (outf, pregs (dst));
      OUTS (outf, "+=");
      OUTS (outf, imm7 (src));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LDSTpmod_0 (TIword iw0, disassemble_info *outf)
{
/* LDSTpmod
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 0 | 0 | 0 |.W.|.aop...|.reg.......|.idx.......|.ptr.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int W   = ((iw0 >> LDSTpmod_W_bits) & LDSTpmod_W_mask);
  int aop = ((iw0 >> LDSTpmod_aop_bits) & LDSTpmod_aop_mask);
  int idx = ((iw0 >> LDSTpmod_idx_bits) & LDSTpmod_idx_mask);
  int ptr = ((iw0 >> LDSTpmod_ptr_bits) & LDSTpmod_ptr_mask);
  int reg = ((iw0 >> LDSTpmod_reg_bits) & LDSTpmod_reg_mask);

  if (aop == 1 && W == 0 && idx == ptr)
    {
      notethat ("dregs_lo = W [ pregs ]");
      OUTS (outf, dregs_lo (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 2 && W == 0 && idx == ptr)
    {
      notethat ("dregs_hi = W [ pregs ]");
      OUTS (outf, dregs_hi (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 1 && W == 1 && idx == ptr)
    {
      notethat ("W [ pregs ] = dregs_lo");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]=");
      OUTS (outf, dregs_lo (reg));
      return 1 * 2;
    }
  else if (aop == 2 && W == 1 && idx == ptr)
    {
      notethat ("W [ pregs ] = dregs_hi");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]=");
      OUTS (outf, dregs_hi (reg));
      return 1 * 2;
    }
  else if (aop == 0 && W == 0)
    {
      notethat ("dregs = [ pregs ++ pregs ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 1 && W == 0)
    {
      notethat ("dregs_lo = W [ pregs ++ pregs ]");
      OUTS (outf, dregs_lo (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 2 && W == 0)
    {
      notethat ("dregs_hi = W [ pregs ++ pregs ]");
      OUTS (outf, dregs_hi (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 3 && W == 0)
    {
      notethat ("dregs = W [ pregs ++ pregs ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "] (Z)");
      return 1 * 2;
    }
  else if (aop == 3 && W == 1)
    {
      notethat ("dregs = W [ pregs ++ pregs ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "](X)");
      return 1 * 2;
    }
  else if (aop == 0 && W == 1)
    {
      notethat ("[ pregs ++ pregs ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 1 && W == 1)
    {
      notethat (" W [ pregs ++ pregs ] = dregs_lo");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "]=");
      OUTS (outf, dregs_lo (reg));
      return 1 * 2;
    }
  else if (aop == 2 && W == 1)
    {
      notethat (" W[ pregs ++ pregs ] = dregs_hi");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++");
      OUTS (outf, pregs (idx));
      OUTS (outf, "]=");
      OUTS (outf, dregs_hi (reg));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_dagMODim_0 (TIword iw0, disassemble_info *outf)
{
/* dagMODim
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 0 | 0 | 1 | 1 | 1 | 1 | 0 |.br| 1 | 1 |.op|.m.....|.i.....|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int i  = ((iw0 >> DagMODim_i_bits) & DagMODim_i_mask);
  int m  = ((iw0 >> DagMODim_m_bits) & DagMODim_m_mask);
  int br = ((iw0 >> DagMODim_br_bits) & DagMODim_br_mask);
  int op = ((iw0 >> DagMODim_op_bits) & DagMODim_op_mask);

  if (op == 0 && br == 1)
    {
      notethat ("iregs += mregs ( BREV )");
      OUTS (outf, iregs (i));
      OUTS (outf, "+=");
      OUTS (outf, mregs (m));
      OUTS (outf, "(BREV)");
      return 1 * 2;
    }
  else if (op == 0)
    {
      notethat ("iregs += mregs");
      OUTS (outf, iregs (i));
      OUTS (outf, "+=");
      OUTS (outf, mregs (m));
      return 1 * 2;
    }
  else if (op == 1)
    {
      notethat ("iregs -= mregs");
      OUTS (outf, iregs (i));
      OUTS (outf, "-=");
      OUTS (outf, mregs (m));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_dagMODik_0 (TIword iw0, disassemble_info *outf)
{
/* dagMODik
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 0 | 0 | 1 | 1 | 1 | 1 | 1 | 0 | 1 | 1 | 0 |.op....|.i.....|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int i  = ((iw0 >> DagMODik_i_bits) & DagMODik_i_mask);
  int op = ((iw0 >> DagMODik_op_bits) & DagMODik_op_mask);

  if (op == 0)
    {
      notethat ("iregs += 2");
      OUTS (outf, iregs (i));
      OUTS (outf, "+=2");
      return 1 * 2;
    }
  else if (op == 1)
    {
      notethat ("iregs -= 2");
      OUTS (outf, iregs (i));
      OUTS (outf, "-=2");
      return 1 * 2;
    }
  else if (op == 2)
    {
      notethat ("iregs += 4");
      OUTS (outf, iregs (i));
      OUTS (outf, "+=4");
      return 1 * 2;
    }
  else if (op == 3)
    {
      notethat ("iregs -= 4");
      OUTS (outf, iregs (i));
      OUTS (outf, "-=4");
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_dspLDST_0 (TIword iw0, disassemble_info *outf)
{
/* dspLDST
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 0 | 0 | 1 | 1 | 1 |.W.|.aop...|.m.....|.i.....|.reg.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int i   = ((iw0 >> DspLDST_i_bits) & DspLDST_i_mask);
  int m   = ((iw0 >> DspLDST_m_bits) & DspLDST_m_mask);
  int W   = ((iw0 >> DspLDST_W_bits) & DspLDST_W_mask);
  int aop = ((iw0 >> DspLDST_aop_bits) & DspLDST_aop_mask);
  int reg = ((iw0 >> DspLDST_reg_bits) & DspLDST_reg_mask);

  if (aop == 0 && W == 0 && m == 0)
    {
      notethat ("dregs = [ iregs ++ ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (aop == 0 && W == 0 && m == 1)
    {
      notethat ("dregs_lo = W [ iregs ++ ]");
      OUTS (outf, dregs_lo (reg));
      OUTS (outf, "=W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (aop == 0 && W == 0 && m == 2)
    {
      notethat ("dregs_hi = W [ iregs ++ ]");
      OUTS (outf, dregs_hi (reg));
      OUTS (outf, "=W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (aop == 1 && W == 0 && m == 0)
    {
      notethat ("dregs = [ iregs -- ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, iregs (i));
      OUTS (outf, "--]");
      return 1 * 2;
    }
  else if (aop == 1 && W == 0 && m == 1)
    {
      notethat ("dregs_lo = W [ iregs -- ]");
      OUTS (outf, dregs_lo (reg));
      OUTS (outf, "=W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "--]");
      return 1 * 2;
    }
  else if (aop == 1 && W == 0 && m == 2)
    {
      notethat ("dregs_hi = W [ iregs -- ]");
      OUTS (outf, dregs_hi (reg));
      OUTS (outf, "=W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "--]");
      return 1 * 2;
    }
  else if (aop == 2 && W == 0 && m == 0)
    {
      notethat ("dregs = [ iregs ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, iregs (i));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 2 && W == 0 && m == 1)
    {
      notethat ("dregs_lo = W [ iregs ]");
      OUTS (outf, dregs_lo (reg));
      OUTS (outf, "=W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 2 && W == 0 && m == 2)
    {
      notethat ("dregs_hi = W [ iregs ]");
      OUTS (outf, dregs_hi (reg));
      OUTS (outf, "=W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 0 && W == 1 && m == 0)
    {
      notethat ("[ iregs ++ ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 0 && W == 1 && m == 1)
    {
      notethat ("W [ iregs ++ ] = dregs_lo");
      OUTS (outf, "W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++]=");
      OUTS (outf, dregs_lo (reg));
      return 1 * 2;
    }
  else if (aop == 0 && W == 1 && m == 2)
    {
      notethat ("W [ iregs ++ ] = dregs_hi");
      OUTS (outf, "W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++]=");
      OUTS (outf, dregs_hi (reg));
      return 1 * 2;
    }
  else if (aop == 1 && W == 1 && m == 0)
    {
      notethat ("[ iregs -- ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, iregs (i));
      OUTS (outf, "--]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 1 && W == 1 && m == 1)
    {
      notethat ("W [ iregs -- ] = dregs_lo");
      OUTS (outf, "W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "--]=");
      OUTS (outf, dregs_lo (reg));
      return 1 * 2;
    }
  else if (aop == 1 && W == 1 && m == 2)
    {
      notethat ("W [ iregs -- ] = dregs_hi");
      OUTS (outf, "W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "--]=");
      OUTS (outf, dregs_hi (reg));
      return 1 * 2;
    }
  else if (aop == 2 && W == 1 && m == 0)
    {
      notethat ("[ iregs ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, iregs (i));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 2 && W == 1 && m == 1)
    {
      notethat (" W [ iregs ] = dregs_lo");
      OUTS (outf, "W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "]=");
      OUTS (outf, dregs_lo (reg));
      return 1 * 2;
    }
  else if (aop == 2 && W == 1 && m == 2)
    {
      notethat (" W [ iregs ] = dregs_hi");
      OUTS (outf, "W[");
      OUTS (outf, iregs (i));
      OUTS (outf, "]=");
      OUTS (outf, dregs_hi (reg));
      return 1 * 2;
    }
  else if (aop == 3 && W == 0)
    {
      notethat ("dregs = [ iregs ++ mregs ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++");
      OUTS (outf, mregs (m));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 3 && W == 1)
    {
      notethat ("[ iregs ++ mregs ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, iregs (i));
      OUTS (outf, "++");
      OUTS (outf, mregs (m));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LDST_0 (TIword iw0, disassemble_info *outf)
{
/* LDST
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 0 | 0 | 1 |.sz....|.W.|.aop...|.Z.|.ptr.......|.reg.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int Z   = ((iw0 >> LDST_Z_bits) & LDST_Z_mask);
  int W   = ((iw0 >> LDST_W_bits) & LDST_W_mask);
  int sz  = ((iw0 >> LDST_sz_bits) & LDST_sz_mask);
  int aop = ((iw0 >> LDST_aop_bits) & LDST_aop_mask);
  int reg = ((iw0 >> LDST_reg_bits) & LDST_reg_mask);
  int ptr = ((iw0 >> LDST_ptr_bits) & LDST_ptr_mask);

  if (aop == 0 && sz == 0 && Z == 0 && W == 0)
    {
      notethat ("dregs = [ pregs ++ ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (aop == 0 && sz == 0 && Z == 1 && W == 0)
    {
      notethat ("pregs = [ pregs ++ ]");
      OUTS (outf, pregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++]");
      return 1 * 2;
    }
  else if (aop == 0 && sz == 1 && Z == 0 && W == 0)
    {
      notethat ("dregs = W [ pregs ++ ] (z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++] (Z)");
      return 1 * 2;
    }
  else if (aop == 0 && sz == 1 && Z == 1 && W == 0)
    {
      notethat ("dregs = W [ pregs ++ ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++](X)");
      return 1 * 2;
    }
  else if (aop == 0 && sz == 2 && Z == 0 && W == 0)
    {
      notethat ("dregs = B [ pregs ++ ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++] (Z)");
      return 1 * 2;
    }
  else if (aop == 0 && sz == 2 && Z == 1 && W == 0)
    {
      notethat ("dregs = B [ pregs ++ ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++](X)");
      return 1 * 2;
    }
  else if (aop == 1 && sz == 0 && Z == 0 && W == 0)
    {
      notethat ("dregs = [ pregs -- ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--]");
      return 1 * 2;
    }
  else if (aop == 1 && sz == 0 && Z == 1 && W == 0)
    {
      notethat ("pregs = [ pregs -- ]");
      OUTS (outf, pregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--]");
      return 1 * 2;
    }
  else if (aop == 1 && sz == 1 && Z == 0 && W == 0)
    {
      notethat ("dregs = W [ pregs -- ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--] (Z)");
      return 1 * 2;
    }
  else if (aop == 1 && sz == 1 && Z == 1 && W == 0)
    {
      notethat ("dregs = W [ pregs -- ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--](X)");
      return 1 * 2;
    }
  else if (aop == 1 && sz == 2 && Z == 0 && W == 0)
    {
      notethat ("dregs = B [ pregs -- ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--] (Z)");
      return 1 * 2;
    }
  else if (aop == 1 && sz == 2 && Z == 1 && W == 0)
    {
      notethat ("dregs = B [ pregs -- ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--](X)");
      return 1 * 2;
    }
  else if (aop == 2 && sz == 0 && Z == 0 && W == 0)
    {
      notethat ("dregs = [ pregs ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 2 && sz == 0 && Z == 1 && W == 0)
    {
      notethat ("pregs = [ pregs ]");
      OUTS (outf, pregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (aop == 2 && sz == 1 && Z == 0 && W == 0)
    {
      notethat ("dregs = W [ pregs ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "] (Z)");
      return 1 * 2;
    }
  else if (aop == 2 && sz == 1 && Z == 1 && W == 0)
    {
      notethat ("dregs = W [ pregs ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "](X)");
      return 1 * 2;
    }
  else if (aop == 2 && sz == 2 && Z == 0 && W == 0)
    {
      notethat ("dregs = B [ pregs ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "] (Z)");
      return 1 * 2;
    }
  else if (aop == 2 && sz == 2 && Z == 1 && W == 0)
    {
      notethat ("dregs = B [ pregs ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "](X)");
      return 1 * 2;
    }
  else if (aop == 0 && sz == 0 && Z == 0 && W == 1)
    {
      notethat ("[ pregs ++ ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 0 && sz == 0 && Z == 1 && W == 1)
    {
      notethat ("[ pregs ++ ] = pregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++]=");
      OUTS (outf, pregs (reg));
      return 1 * 2;
    }
  else if (aop == 0 && sz == 1 && Z == 0 && W == 1)
    {
      notethat ("W [ pregs ++ ] = dregs");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 0 && sz == 2 && Z == 0 && W == 1)
    {
      notethat ("B [ pregs ++ ] = dregs");
      OUTS (outf, "B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "++]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 1 && sz == 0 && Z == 0 && W == 1)
    {
      notethat ("[ pregs -- ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 1 && sz == 0 && Z == 1 && W == 1)
    {
      notethat ("[ pregs -- ] = pregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--]=");
      OUTS (outf, pregs (reg));
      return 1 * 2;
    }
  else if (aop == 1 && sz == 1 && Z == 0 && W == 1)
    {
      notethat ("W [ pregs -- ] = dregs");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 1 && sz == 2 && Z == 0 && W == 1)
    {
      notethat ("B [ pregs -- ] = dregs");
      OUTS (outf, "B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "--]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 2 && sz == 0 && Z == 0 && W == 1)
    {
      notethat ("[ pregs ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 2 && sz == 0 && Z == 1 && W == 1)
    {
      notethat ("[ pregs ] = pregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]=");
      OUTS (outf, pregs (reg));
      return 1 * 2;
    }
  else if (aop == 2 && sz == 1 && Z == 0 && W == 1)
    {
      notethat ("W [ pregs ] = dregs");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (aop == 2 && sz == 2 && Z == 0 && W == 1)
    {
      notethat ("B [ pregs ] = dregs");
      OUTS (outf, "B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LDSTiiFP_0 (TIword iw0, disassemble_info *outf)
{
/* LDSTiiFP
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 0 | 1 | 1 | 1 | 0 |.W.|.offset............|.reg...........|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int reg = ((iw0 >> LDSTiiFP_reg_bits) & LDSTiiFP_reg_mask);
  int offset = ((iw0 >> LDSTiiFP_offset_bits) & LDSTiiFP_offset_mask);
  int W = ((iw0 >> LDSTiiFP_W_bits) & LDSTiiFP_W_mask);

  if (W == 0)
    {
      notethat ("dpregs = [ FP - negimm5s4 ]");
      OUTS (outf, dpregs (reg));
      OUTS (outf, "=[FP");
      OUTS (outf, negimm5s4 (offset));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (W == 1)
    {
      notethat ("[ FP - negimm5s4 ] = dpregs");
      OUTS (outf, "[FP");
      OUTS (outf, negimm5s4 (offset));
      OUTS (outf, "]=");
      OUTS (outf, dpregs (reg));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LDSTii_0 (TIword iw0, disassemble_info *outf)
{
/* LDSTii
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 0 | 1 |.W.|.op....|.offset........|.ptr.......|.reg.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int reg = ((iw0 >> LDSTii_reg_bit) & LDSTii_reg_mask);
  int ptr = ((iw0 >> LDSTii_ptr_bit) & LDSTii_ptr_mask);
  int offset = ((iw0 >> LDSTii_offset_bit) & LDSTii_offset_mask);
  int op = ((iw0 >> LDSTii_op_bit) & LDSTii_op_mask);
  int W = ((iw0 >> LDSTii_W_bit) & LDSTii_W_mask);

  if (W == 0 && op == 0)
    {
      notethat ("dregs = [ pregs + uimm4s4 ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, uimm4s4 (offset));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (W == 0 && op == 1)
    {
      notethat ("dregs = W [ pregs + uimm4s2 ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, uimm4s2 (offset));
      OUTS (outf, "] (Z)");
      return 1 * 2;
    }
  else if (W == 0 && op == 2)
    {
      notethat ("dregs = W [ pregs + uimm4s2 ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, uimm4s2 (offset));
      OUTS (outf, "](X)");
      return 1 * 2;
    }
  else if (W == 0 && op == 3)
    {
      notethat ("pregs = [ pregs + uimm4s4 ]");
      OUTS (outf, pregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, uimm4s4 (offset));
      OUTS (outf, "]");
      return 1 * 2;
    }
  else if (W == 1 && op == 0)
    {
      notethat ("[ pregs + uimm4s4 ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, uimm4s4 (offset));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (W == 1 && op == 1)
    {
      notethat ("W [ pregs + uimm4s2 ] = dregs");
      OUTS (outf, "W");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, uimm4s2 (offset));
      OUTS (outf, "]");
      OUTS (outf, "=");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (W == 1 && op == 3)
    {
      notethat ("[ pregs + uimm4s4 ] = pregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, uimm4s4 (offset));
      OUTS (outf, "]=");
      OUTS (outf, pregs (reg));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LoopSetup_0 (TIword iw0, TIword iw1, bfd_vma pc, disassemble_info *outf)
{
/* LoopSetup
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 1 | 0 | 0 | 0 | 0 | 0 | 1 |.rop...|.c.|.soffset.......|
|.reg...........| - | - |.eoffset...............................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int c   = ((iw0 >> (LoopSetup_c_bits - 16)) & LoopSetup_c_mask);
  int reg = ((iw1 >> LoopSetup_reg_bits) & LoopSetup_reg_mask);
  int rop = ((iw0 >> (LoopSetup_rop_bits - 16)) & LoopSetup_rop_mask);
  int soffset = ((iw0 >> (LoopSetup_soffset_bits - 16)) & LoopSetup_soffset_mask);
  int eoffset = ((iw1 >> LoopSetup_eoffset_bits) & LoopSetup_eoffset_mask);

  if (rop == 0)
    {
      notethat ("LSETUP ( pcrel4 , lppcrel10 ) counters");
      OUTS (outf, "LSETUP");
      OUTS (outf, "(");
      OUTS (outf, pcrel4 (soffset));
      OUTS (outf, ",");
      OUTS (outf, lppcrel10 (eoffset));
      OUTS (outf, ")");
      OUTS (outf, counters (c));
      return 2 * 2;
    }
  else if (rop == 1)
    {
      notethat ("LSETUP ( pcrel4 , lppcrel10 ) counters = pregs");
      OUTS (outf, "LSETUP");
      OUTS (outf, "(");
      OUTS (outf, pcrel4 (soffset));
      OUTS (outf, ",");
      OUTS (outf, lppcrel10 (eoffset));
      OUTS (outf, ")");
      OUTS (outf, counters (c));
      OUTS (outf, "=");
      OUTS (outf, pregs (reg));
      return 2 * 2;
    }
  else if (rop == 3)
    {
      notethat ("LSETUP ( pcrel4 , lppcrel10 ) counters = pregs >> 1");
      OUTS (outf, "LSETUP");
      OUTS (outf, "(");
      OUTS (outf, pcrel4 (soffset));
      OUTS (outf, ",");
      OUTS (outf, lppcrel10 (eoffset));
      OUTS (outf, ")");
      OUTS (outf, counters (c));
      OUTS (outf, "=");
      OUTS (outf, pregs (reg));
      OUTS (outf, ">>1");
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LDIMMhalf_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* LDIMMhalf
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 1 | 0 | 0 | 0 | 0 | 1 |.Z.|.H.|.S.|.grp...|.reg.......|
|.hword.........................................................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int H = ((iw0 >> (LDIMMhalf_H_bits - 16)) & LDIMMhalf_H_mask);
  int Z = ((iw0 >> (LDIMMhalf_Z_bits - 16)) & LDIMMhalf_Z_mask);
  int S = ((iw0 >> (LDIMMhalf_S_bits - 16)) & LDIMMhalf_S_mask);
  int reg = ((iw0 >> (LDIMMhalf_reg_bits - 16)) & LDIMMhalf_reg_mask);
  int grp = ((iw0 >> (LDIMMhalf_grp_bits - 16)) & LDIMMhalf_grp_mask);
  int hword = ((iw1 >> LDIMMhalf_hword_bits) & LDIMMhalf_hword_mask);

  if (grp == 0 && H == 0 && S == 0 && Z == 0)
    {
      notethat ("dregs_lo = imm16");
      OUTS (outf, dregs_lo (reg));
      OUTS (outf, "=");
      OUTS (outf, imm16 (hword));
      return 2 * 2;
    }
  else if (grp == 0 && H == 1 && S == 0 && Z == 0)
    {
      notethat ("dregs_hi = imm16");
      OUTS (outf, dregs_hi (reg));
      OUTS (outf, "=");
      OUTS (outf, imm16 (hword));
      return 2 * 2;
    }
  else if (grp == 0 && H == 0 && S == 1 && Z == 0)
    {
      notethat ("dregs = imm16 (x)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=");
      OUTS (outf, imm16 (hword));
      OUTS (outf, " (X)");
      return 2 * 2;
    }
  else if (H == 0 && S == 1 && Z == 0)
    {
      notethat ("regs = imm16 (x)");
      OUTS (outf, regs (reg, grp));
      OUTS (outf, "=");
      OUTS (outf, imm16 (hword));
      OUTS (outf, " (X)");
      return 2 * 2;
    }
  else if (H == 0 && S == 0 && Z == 1)
    {
      notethat ("regs = luimm16 (Z)");
      OUTS (outf, regs (reg, grp));
      OUTS (outf, "=");
      OUTS (outf, luimm16 (hword));
      OUTS (outf, "(Z)");
      return 2 * 2;
    }
  else if (H == 0 && S == 0 && Z == 0)
    {
      notethat ("regs_lo = luimm16");
      OUTS (outf, regs_lo (reg, grp));
      OUTS (outf, "=");
      OUTS (outf, luimm16 (hword));
      return 2 * 2;
    }
  else if (H == 1 && S == 0 && Z == 0)
    {
      notethat ("regs_hi = huimm16");
      OUTS (outf, regs_hi (reg, grp));
      OUTS (outf, "=");
      OUTS (outf, huimm16 (hword));
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_CALLa_0 (TIword iw0, TIword iw1, bfd_vma pc, disassemble_info *outf)
{
/* CALLa
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 1 | 0 | 0 | 0 | 1 |.S.|.msw...........................|
|.lsw...........................................................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int S   = ((iw0 >> (CALLa_S_bits - 16)) & CALLa_S_mask);
  int lsw = ((iw1 >> 0) & 0xffff);
  int msw = ((iw0 >> 0) & 0xff);

  if (S == 1)
    {
      notethat ("CALL  pcrel24");
      OUTS (outf, "CALL  ");
      OUTS (outf, pcrel24 (((msw) << 16) | (lsw)));
      return 2 * 2;
    }
  else if (S == 0)
    {
      notethat ("JUMP.L  pcrel24");
      OUTS (outf, "JUMP.L  ");
      OUTS (outf, pcrel24 (((msw) << 16) | (lsw)));
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_LDSTidxI_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* LDSTidxI
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 1 | 0 | 0 | 1 |.W.|.Z.|.sz....|.ptr.......|.reg.......|
|.offset........................................................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int Z = ((iw0 >> (LDSTidxI_Z_bits - 16)) & LDSTidxI_Z_mask);
  int W = ((iw0 >> (LDSTidxI_W_bits - 16)) & LDSTidxI_W_mask);
  int sz = ((iw0 >> (LDSTidxI_sz_bits - 16)) & LDSTidxI_sz_mask);
  int reg = ((iw0 >> (LDSTidxI_reg_bits - 16)) & LDSTidxI_reg_mask);
  int ptr = ((iw0 >> (LDSTidxI_ptr_bits - 16)) & LDSTidxI_ptr_mask);
  int offset = ((iw1 >> LDSTidxI_offset_bits) & LDSTidxI_offset_mask);

  if (W == 0 && sz == 0 && Z == 0)
    {
      notethat ("dregs = [ pregs + imm16s4 ]");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16s4 (offset));
      OUTS (outf, "]");
      return 2 * 2;
    }
  else if (W == 0 && sz == 0 && Z == 1)
    {
      notethat ("pregs = [ pregs + imm16s4 ]");
      OUTS (outf, pregs (reg));
      OUTS (outf, "=[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16s4 (offset));
      OUTS (outf, "]");
      return 2 * 2;
    }
  else if (W == 0 && sz == 1 && Z == 0)
    {
      notethat ("dregs = W [ pregs + imm16s2 ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16s2 (offset));
      OUTS (outf, "] (Z)");
      return 2 * 2;
    }
  else if (W == 0 && sz == 1 && Z == 1)
    {
      notethat ("dregs = W [ pregs + imm16s2 ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16s2 (offset));
      OUTS (outf, "](X)");
      return 2 * 2;
    }
  else if (W == 0 && sz == 2 && Z == 0)
    {
      notethat ("dregs = B [ pregs + imm16 ] (Z)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16 (offset));
      OUTS (outf, "] (Z)");
      return 2 * 2;
    }
  else if (W == 0 && sz == 2 && Z == 1)
    {
      notethat ("dregs = B [ pregs + imm16 ] (X)");
      OUTS (outf, dregs (reg));
      OUTS (outf, "=B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16 (offset));
      OUTS (outf, "](X)");
      return 2 * 2;
    }
  else if (W == 1 && sz == 0 && Z == 0)
    {
      notethat ("[ pregs + imm16s4 ] = dregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16s4 (offset));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 2 * 2;
    }
  else if (W == 1 && sz == 0 && Z == 1)
    {
      notethat ("[ pregs + imm16s4 ] = pregs");
      OUTS (outf, "[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16s4 (offset));
      OUTS (outf, "]=");
      OUTS (outf, pregs (reg));
      return 2 * 2;
    }
  else if (W == 1 && sz == 1 && Z == 0)
    {
      notethat ("W [ pregs + imm16s2 ] = dregs");
      OUTS (outf, "W[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16s2 (offset));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 2 * 2;
    }
  else if (W == 1 && sz == 2 && Z == 0)
    {
      notethat ("B [ pregs + imm16 ] = dregs");
      OUTS (outf, "B[");
      OUTS (outf, pregs (ptr));
      OUTS (outf, "+");
      OUTS (outf, imm16 (offset));
      OUTS (outf, "]=");
      OUTS (outf, dregs (reg));
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_linkage_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* linkage
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 1 | 0 | 1 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |.R.|
|.framesize.....................................................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int R = ((iw0 >> (Linkage_R_bits - 16)) & Linkage_R_mask);
  int framesize = ((iw1 >> Linkage_framesize_bits) & Linkage_framesize_mask);

  if (R == 0)
    {
      notethat ("LINK uimm16s4");
      OUTS (outf, "LINK ");
      OUTS (outf, uimm16s4 (framesize));
      return 2 * 2;
    }
  else if (R == 1)
    {
      notethat ("UNLINK");
      OUTS (outf, "UNLINK");
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_dsp32mac_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* dsp32mac
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 0 | 0 |.M.| 0 | 0 |.mmod..........|.MM|.P.|.w1|.op1...|
|.h01|.h11|.w0|.op0...|.h00|.h10|.dst.......|.src0......|.src1......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int op1 = ((iw0 >> (DSP32Mac_op1_bits - 16)) & DSP32Mac_op1_mask);
  int w1 = ((iw0 >> (DSP32Mac_w1_bits - 16)) & DSP32Mac_w1_mask);
  int P = ((iw0 >> (DSP32Mac_p_bits - 16)) & DSP32Mac_p_mask);
  int MM = ((iw0 >> (DSP32Mac_MM_bits - 16)) & DSP32Mac_MM_mask);
  int mmod = ((iw0 >> (DSP32Mac_mmod_bits - 16)) & DSP32Mac_mmod_mask);
  int w0 = ((iw1 >> DSP32Mac_w0_bits) & DSP32Mac_w0_mask);
  int src0 = ((iw1 >> DSP32Mac_src0_bits) & DSP32Mac_src0_mask);
  int src1 = ((iw1 >> DSP32Mac_src1_bits) & DSP32Mac_src1_mask);
  int dst = ((iw1 >> DSP32Mac_dst_bits) & DSP32Mac_dst_mask);
  int h10 = ((iw1 >> DSP32Mac_h10_bits) & DSP32Mac_h10_mask);
  int h00 = ((iw1 >> DSP32Mac_h00_bits) & DSP32Mac_h00_mask);
  int op0 = ((iw1 >> DSP32Mac_op0_bits) & DSP32Mac_op0_mask);
  int h11 = ((iw1 >> DSP32Mac_h11_bits) & DSP32Mac_h11_mask);
  int h01 = ((iw1 >> DSP32Mac_h01_bits) & DSP32Mac_h01_mask);

  if (w0 == 0 && w1 == 0 && op1 == 3 && op0 == 3)
    return 0;

  if (op1 == 3 && MM)
    return 0;

  if ((w1 || w0) && mmod == M_W32)
    return 0;

  if (((1 << mmod) & (P ? 0x31b : 0x1b5f)) == 0)
    return 0;

  if (w1 == 1 || op1 != 3)
    {
      if (w1)
	OUTS (outf, P ? dregs (dst + 1) : dregs_hi (dst));

      if (op1 == 3)
	OUTS (outf, " = A1");
      else
	{
	  if (w1)
	    OUTS (outf, " = (");
	  decode_macfunc (1, op1, h01, h11, src0, src1, outf);
	  if (w1)
	    OUTS (outf, ")");
	}

      if (w0 == 1 || op0 != 3)
	{
	  if (MM)
	    OUTS (outf, " (M)");
	  MM = 0;
	  OUTS (outf, ", ");
	}
    }

  if (w0 == 1 || op0 != 3)
    {
      if (w0)
	OUTS (outf, P ? dregs (dst) : dregs_lo (dst));

      if (op0 == 3)
	OUTS (outf, " = A0");
      else
	{
	  if (w0)
	    OUTS (outf, " = (");
	  decode_macfunc (0, op0, h00, h10, src0, src1, outf);
	  if (w0)
	    OUTS (outf, ")");
	}
    }

  decode_optmode (mmod, MM, outf);

  return 4;
}

static int
decode_dsp32mult_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* dsp32mult
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 0 | 0 |.M.| 0 | 1 |.mmod..........|.MM|.P.|.w1|.op1...|
|.h01|.h11|.w0|.op0...|.h00|.h10|.dst.......|.src0......|.src1......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int w1 = ((iw0 >> (DSP32Mac_w1_bits - 16)) & DSP32Mac_w1_mask);
  int P = ((iw0 >> (DSP32Mac_p_bits - 16)) & DSP32Mac_p_mask);
  int MM = ((iw0 >> (DSP32Mac_MM_bits - 16)) & DSP32Mac_MM_mask);
  int mmod = ((iw0 >> (DSP32Mac_mmod_bits - 16)) & DSP32Mac_mmod_mask);
  int w0 = ((iw1 >> DSP32Mac_w0_bits) & DSP32Mac_w0_mask);
  int src0 = ((iw1 >> DSP32Mac_src0_bits) & DSP32Mac_src0_mask);
  int src1 = ((iw1 >> DSP32Mac_src1_bits) & DSP32Mac_src1_mask);
  int dst = ((iw1 >> DSP32Mac_dst_bits) & DSP32Mac_dst_mask);
  int h10 = ((iw1 >> DSP32Mac_h10_bits) & DSP32Mac_h10_mask);
  int h00 = ((iw1 >> DSP32Mac_h00_bits) & DSP32Mac_h00_mask);
  int h11 = ((iw1 >> DSP32Mac_h11_bits) & DSP32Mac_h11_mask);
  int h01 = ((iw1 >> DSP32Mac_h01_bits) & DSP32Mac_h01_mask);

  if (w1 == 0 && w0 == 0)
    return 0;
  if (((1 << mmod) & (P ? 0x313 : 0x1b57)) == 0)
    return 0;
  
  if (w1)
    {
      OUTS (outf, P ? dregs (dst | 1) : dregs_hi (dst));
      OUTS (outf, " = ");
      decode_multfunc (h01, h11, src0, src1, outf);

      if (w0)
	{
	  if (MM)
	    OUTS (outf, " (M)");
	  MM = 0;
	  OUTS (outf, ", ");
	}
    }

  if (w0)
    {
      OUTS (outf, dregs (dst));
      OUTS (outf, " = ");
      decode_multfunc (h00, h10, src0, src1, outf);
    }

  decode_optmode (mmod, MM, outf);
  return 4;
}

static int
decode_dsp32alu_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* dsp32alu
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 0 | 0 |.M.| 1 | 0 | - | - | - |.HL|.aopcde............|
|.aop...|.s.|.x.|.dst0......|.dst1......|.src0......|.src1......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int s    = ((iw1 >> DSP32Alu_s_bits) & DSP32Alu_s_mask);
  int x    = ((iw1 >> DSP32Alu_x_bits) & DSP32Alu_x_mask);
  int aop  = ((iw1 >> DSP32Alu_aop_bits) & DSP32Alu_aop_mask);
  int src0 = ((iw1 >> DSP32Alu_src0_bits) & DSP32Alu_src0_mask);
  int src1 = ((iw1 >> DSP32Alu_src1_bits) & DSP32Alu_src1_mask);
  int dst0 = ((iw1 >> DSP32Alu_dst0_bits) & DSP32Alu_dst0_mask);
  int dst1 = ((iw1 >> DSP32Alu_dst1_bits) & DSP32Alu_dst1_mask);
  int HL   = ((iw0 >> (DSP32Alu_HL_bits - 16)) & DSP32Alu_HL_mask);
  int aopcde = ((iw0 >> (DSP32Alu_aopcde_bits - 16)) & DSP32Alu_aopcde_mask);

  if (aop == 0 && aopcde == 9 && HL == 0 && s == 0)
    {
      notethat ("A0.L = dregs_lo");
      OUTS (outf, "A0.L=");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 9 && HL == 1 && s == 0)
    {
      notethat ("A1.H = dregs_hi");
      OUTS (outf, "A1.H=");
      OUTS (outf, dregs_hi (src0));
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 9 && HL == 0 && s == 0)
    {
      notethat ("A1.L = dregs_lo");
      OUTS (outf, "A1.L=");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 9 && HL == 1 && s == 0)
    {
      notethat ("A0.H = dregs_hi");
      OUTS (outf, "A0.H=");
      OUTS (outf, dregs_hi (src0));
      return 2 * 2;
    }
  else if (x == 1 && HL == 1 && aop == 3 && aopcde == 5)
    {
      notethat ("dregs_hi = dregs - dregs (RND20)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND20)");
      return 2 * 2;
    }
  else if (x == 1 && HL == 1 && aop == 2 && aopcde == 5)
    {
      notethat ("dregs_hi = dregs + dregs (RND20)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND20)");
      return 2 * 2;
    }
  else if (x == 0 && HL == 0 && aop == 1 && aopcde == 5)
    {
      notethat ("dregs_lo = dregs - dregs (RND12)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND12)");
      return 2 * 2;
    }
  else if (x == 0 && HL == 0 && aop == 0 && aopcde == 5)
    {
      notethat ("dregs_lo = dregs + dregs (RND12)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND12)");
      return 2 * 2;
    }
  else if (x == 1 && HL == 0 && aop == 3 && aopcde == 5)
    {
      notethat ("dregs_lo = dregs - dregs (RND20)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND20)");
      return 2 * 2;
    }
  else if (x == 0 && HL == 1 && aop == 0 && aopcde == 5)
    {
      notethat ("dregs_hi = dregs + dregs (RND12)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND12)");
      return 2 * 2;
    }
  else if (x == 1 && HL == 0 && aop == 2 && aopcde == 5)
    {
      notethat ("dregs_lo = dregs + dregs (RND20)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND20)");
      return 2 * 2;
    }
  else if (x == 0 && HL == 1 && aop == 1 && aopcde == 5)
    {
      notethat ("dregs_hi = dregs - dregs (RND12)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs (src1));
      OUTS (outf, "(RND12)");
      return 2 * 2;
    }
  else if (HL == 1 && aop == 0 && aopcde == 2)
    {
      notethat ("dregs_hi = dregs_lo + dregs_lo amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 1 && aop == 1 && aopcde == 2)
    {
      notethat ("dregs_hi = dregs_lo + dregs_hi amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 1 && aop == 2 && aopcde == 2)
    {
      notethat ("dregs_hi = dregs_hi + dregs_lo amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 1 && aop == 3 && aopcde == 2)
    {
      notethat ("dregs_hi = dregs_hi + dregs_hi amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 0 && aopcde == 3)
    {
      notethat ("dregs_lo = dregs_lo - dregs_lo amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 1 && aopcde == 3)
    {
      notethat ("dregs_lo = dregs_lo - dregs_hi amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 3 && aopcde == 2)
    {
      notethat ("dregs_lo = dregs_hi + dregs_hi amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 1 && aop == 0 && aopcde == 3)
    {
      notethat ("dregs_hi = dregs_lo - dregs_lo amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 1 && aop == 1 && aopcde == 3)
    {
      notethat ("dregs_hi = dregs_lo - dregs_hi amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 1 && aop == 2 && aopcde == 3)
    {
      notethat ("dregs_hi = dregs_hi - dregs_lo amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 1 && aop == 3 && aopcde == 3)
    {
      notethat ("dregs_hi = dregs_hi - dregs_hi amod1");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 2 && aopcde == 2)
    {
      notethat ("dregs_lo = dregs_hi + dregs_lo amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 1 && aopcde == 2)
    {
      notethat ("dregs_lo = dregs_lo + dregs_hi amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 2 && aopcde == 3)
    {
      notethat ("dregs_lo = dregs_hi - dregs_lo amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 3 && aopcde == 3)
    {
      notethat ("dregs_lo = dregs_hi - dregs_hi amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aop == 0 && aopcde == 2)
    {
      notethat ("dregs_lo = dregs_lo + dregs_lo amod1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 9 && s == 1)
    {
      notethat ("A0 = dregs");
      OUTS (outf, "A0=");
      OUTS (outf, dregs (src0));
      return 2 * 2;
    }
  else if (aop == 3 && aopcde == 11 && s == 0)
    {
      notethat ("A0 -= A1");
      OUTS (outf, "A0-=A1");
      return 2 * 2;
    }
  else if (aop == 3 && aopcde == 11 && s == 1)
    {
      notethat ("A0 -= A1 (W32)");
      OUTS (outf, "A0-=A1(W32)");
      return 2 * 2;
    }
  else if (aop == 3 && aopcde == 22 && HL == 1)
    {
      notethat ("dregs = BYTEOP2M ( dregs_pair , dregs_pair ) (TH , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2M(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(TH");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 3 && aopcde == 22 && HL == 0)
    {
      notethat ("dregs = BYTEOP2M ( dregs_pair , dregs_pair ) (TL , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2M(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(TL");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 22 && HL == 1)
    {
      notethat ("dregs = BYTEOP2M ( dregs_pair , dregs_pair ) (RNDH , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2M(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(RNDH");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 22 && HL == 0)
    {
      notethat ("dregs = BYTEOP2M ( dregs_pair , dregs_pair ) (RNDL , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2M(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(RNDL");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 22 && HL == 1)
    {
      notethat ("dregs = BYTEOP2P ( dregs_pair , dregs_pair ) (TH , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(TH");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 22 && HL == 0)
    {
      notethat ("dregs = BYTEOP2P ( dregs_pair , dregs_pair ) (TL , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(TL");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 22 && HL == 1)
    {
      notethat ("dregs = BYTEOP2P ( dregs_pair , dregs_pair ) (RNDH , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(RNDH");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 22 && HL == 0)
    {
      notethat ("dregs = BYTEOP2P ( dregs_pair , dregs_pair ) (RNDL , aligndir)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP2P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(RNDL");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 0 && s == 0 && aopcde == 8)
    {
      notethat ("A0 = 0");
      OUTS (outf, "A0=0");
      return 2 * 2;
    }
  else if (aop == 0 && s == 1 && aopcde == 8)
    {
      notethat ("A0 = A0 (S)");
      OUTS (outf, "A0=A0(S)");
      return 2 * 2;
    }
  else if (aop == 1 && s == 0 && aopcde == 8)
    {
      notethat ("A1 = 0");
      OUTS (outf, "A1=0");
      return 2 * 2;
    }
  else if (aop == 1 && s == 1 && aopcde == 8)
    {
      notethat ("A1 = A1 (S)");
      OUTS (outf, "A1=A1(S)");
      return 2 * 2;
    }
  else if (aop == 2 && s == 0 && aopcde == 8)
    {
      notethat ("A1 = A0 = 0");
      OUTS (outf, "A1=A0=0");
      return 2 * 2;
    }
  else if (aop == 2 && s == 1 && aopcde == 8)
    {
      notethat ("A1 = A1 (S) , A0 = A0 (S)");
      OUTS (outf, "A1=A1(S),A0=A0(S)");
      return 2 * 2;
    }
  else if (aop == 3 && s == 0 && aopcde == 8)
    {
      notethat ("A0 = A1");
      OUTS (outf, "A0=A1");
      return 2 * 2;
    }
  else if (aop == 3 && s == 1 && aopcde == 8)
    {
      notethat ("A1 = A0");
      OUTS (outf, "A1=A0");
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 9 && s == 0)
    {
      notethat ("A0.x = dregs_lo");
      OUTS (outf, "A0.x=");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (aop == 1 && HL == 0 && aopcde == 11)
    {
      notethat ("dregs_lo = ( A0 += A1 )");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=(A0+=A1)");
      return 2 * 2;
    }
  else if (aop == 3 && HL == 0 && aopcde == 16)
    {
      notethat ("A1 = ABS A1, A0 = ABS A0");
      OUTS (outf, "A1= ABS A0,A0= ABS A0");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 23 && HL == 1)
    {
      notethat ("dregs = BYTEOP3P ( dregs_pair , dregs_pair ) (HI , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP3P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(HI");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 3 && aopcde == 9 && s == 0)
    {
      notethat ("A1.x = dregs_lo");
      OUTS (outf, "A1.x=");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (aop == 1 && HL == 1 && aopcde == 16)
    {
      notethat ("A1 = ABS A1");
      OUTS (outf, "A1= ABS A1");
      return 2 * 2;
    }
  else if (aop == 0 && HL == 1 && aopcde == 16)
    {
      notethat ("A1 = ABS A0");
      OUTS (outf, "A1= ABS A0");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 9 && s == 1)
    {
      notethat ("A1 = dregs");
      OUTS (outf, "A1=");
      OUTS (outf, dregs (src0));
      return 2 * 2;
    }
  else if (HL == 0 && aop == 3 && aopcde == 12)
    {
      notethat ("dregs_lo = dregs (RND)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "(RND)");
      return 2 * 2;
    }
  else if (aop == 1 && HL == 0 && aopcde == 16)
    {
      notethat ("A0 = ABS A1");
      OUTS (outf, "A0= ABS A1");
      return 2 * 2;
    }
  else if (aop == 0 && HL == 0 && aopcde == 16)
    {
      notethat ("A0 = ABS A0");
      OUTS (outf, "A0= ABS A0");
      return 2 * 2;
    }
  else if (aop == 3 && HL == 0 && aopcde == 15)
    {
      notethat ("dregs = - dregs (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=-");
      OUTS (outf, dregs (src0));
      OUTS (outf, "(V)");
      return 2 * 2;
    }
  else if (aop == 3 && s == 1 && HL == 0 && aopcde == 7)
    {
      notethat ("dregs = - dregs (S)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=-");
      OUTS (outf, dregs (src0));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (aop == 3 && s == 0 && HL == 0 && aopcde == 7)
    {
      notethat ("dregs = - dregs (NS)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=-");
      OUTS (outf, dregs (src0));
      OUTS (outf, "(NS)");
      return 2 * 2;
    }
  else if (aop == 1 && HL == 1 && aopcde == 11)
    {
      notethat ("dregs_hi = ( A0 += A1 )");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=(A0+=A1)");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 11 && s == 0)
    {
      notethat ("A0 += A1");
      OUTS (outf, "A0+=A1");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 11 && s == 1)
    {
      notethat ("A0 += A1 (W32)");
      OUTS (outf, "A0+=A1(W32)");
      return 2 * 2;
    }
  else if (aop == 3 && HL == 0 && aopcde == 14)
    {
      notethat ("A1 = - A1 , A0 = - A0");
      OUTS (outf, "A1=-A1,A0=-A0");
      return 2 * 2;
    }
  else if (HL == 1 && aop == 3 && aopcde == 12)
    {
      notethat ("dregs_hi = dregs (RND)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "(RND)");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 23 && HL == 0)
    {
      notethat ("dregs = BYTEOP3P ( dregs_pair , dregs_pair ) (LO , R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP3P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(LO");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 0 && HL == 0 && aopcde == 14)
    {
      notethat ("A0 = - A0");
      OUTS (outf, "A0=-A0");
      return 2 * 2;
    }
  else if (aop == 1 && HL == 0 && aopcde == 14)
    {
      notethat ("A0 = - A1");
      OUTS (outf, "A0=-A1");
      return 2 * 2;
    }
  else if (aop == 0 && HL == 1 && aopcde == 14)
    {
      notethat ("A1 = - A0");
      OUTS (outf, "A1=-A0");
      return 2 * 2;
    }
  else if (aop == 1 && HL == 1 && aopcde == 14)
    {
      notethat ("A1 = - A1");
      OUTS (outf, "A1=-A1");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 12)
    {
      notethat ("dregs_hi=dregs_lo=SIGN(dregs_hi)*dregs_hi + SIGN(dregs_lo)*dregs_lo)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=SIGN(");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, ")*");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, "+SIGN(");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ")*");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 0)
    {
      notethat ("dregs = dregs -|+ dregs amod0");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-|+");
      OUTS (outf, dregs (src1));
      OUTS (outf, " ");
      amod0 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 12)
    {
      notethat ("dregs = A1.L + A1.H , dregs = A0.L + A0.H");
      OUTS (outf, dregs (dst1));
      OUTS (outf, "=A1.L+A1.H,");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=A0.L+A0.H");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 4)
    {
      notethat ("dregs = dregs + dregs , dregs = dregs - dregs amod1");
      OUTS (outf, dregs (dst1));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (HL == 0 && aopcde == 1)
    {
      notethat ("dregs = dregs +|+ dregs , dregs = dregs -|- dregs (amod0, amod2)");
      OUTS (outf, dregs (dst1));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+|+");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-|-");
      OUTS (outf, dregs (src1));
      amod0amod2 (s, x, aop, outf);
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 11)
    {
      notethat ("dregs = ( A0 += A1 )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=(A0+=A1)");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 10)
    {
      notethat ("dregs_lo = A0.x");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=A0.x");
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 10)
    {
      notethat ("dregs_lo = A1.x");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=A1.x");
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 0)
    {
      notethat ("dregs = dregs +|- dregs amod0");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+|-");
      OUTS (outf, dregs (src1));
      OUTS (outf, " ");
      amod0 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 3 && aopcde == 0)
    {
      notethat ("dregs = dregs -|- dregs amod0");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-|-");
      OUTS (outf, dregs (src1));
      OUTS (outf, " ");
      amod0 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 4)
    {
      notethat ("dregs = dregs - dregs amod1");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-");
      OUTS (outf, dregs (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 17)
    {
      notethat ("dregs = A1 + A0, dregs = A1 - A0 amod1");
      OUTS (outf, dregs (dst1));
      OUTS (outf, "=A1+A0,");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=A1-A0 ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 17)
    {
      notethat ("dregs = A0 + A1, dregs = A0 - A1 amod1");
      OUTS (outf, dregs (dst1));
      OUTS (outf, "=A0+A1,");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=A0-A1 ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 18)
    {
      notethat ("SAA ( dregs_pair , dregs_pair ) aligndir");
      OUTS (outf, "SAA(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ") ");
      aligndir (s, outf);
      return 2 * 2;
    }
  else if (aop == 3 && aopcde == 18)
    {
      notethat ("DISALGNEXCPT");
      OUTS (outf, "DISALGNEXCPT");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 20)
    {
      notethat ("dregs = BYTEOP1P ( dregs_pair , dregs_pair ) aligndir");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP1P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")");
      aligndir (s, outf);
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 20)
    {
      notethat ("dregs = BYTEOP1P ( dregs_pair , dregs_pair ) (T, R)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEOP1P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ")(T");
      if (s == 1)
	OUTS (outf, ", R)");
      else
	OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 21)
    {
      notethat ("( dregs , dregs ) = BYTEOP16P ( dregs_pair , dregs_pair ) aligndir");
      OUTS (outf, "(");
      OUTS (outf, dregs (dst1));
      OUTS (outf, ",");
      OUTS (outf, dregs (dst0));
      OUTS (outf, ")=BYTEOP16P(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ") ");
      aligndir (s, outf);
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 21)
    {
      notethat ("( dregs , dregs ) = BYTEOP16M ( dregs_pair , dregs_pair ) aligndir");
      OUTS (outf, "(");
      OUTS (outf, dregs (dst1));
      OUTS (outf, ",");
      OUTS (outf, dregs (dst0));
      OUTS (outf, ")=BYTEOP16M(");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src1));
      OUTS (outf, ") ");
      aligndir (s, outf);
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 7)
    {
      notethat ("dregs = ABS dregs");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ABS ");
      OUTS (outf, dregs (src0));
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 7)
    {
      notethat ("dregs = MIN ( dregs , dregs )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=MIN(");
      OUTS (outf, dregs (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 7)
    {
      notethat ("dregs = MAX ( dregs , dregs )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=MAX(");
      OUTS (outf, dregs (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 2 && aopcde == 6)
    {
      notethat ("dregs = ABS dregs (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ABS ");
      OUTS (outf, dregs (src0));
      OUTS (outf, "(V)");
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 6)
    {
      notethat ("dregs = MIN ( dregs , dregs ) (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=MIN(");
      OUTS (outf, dregs (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1));
      OUTS (outf, ")(V)");
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 6)
    {
      notethat ("dregs = MAX ( dregs , dregs ) (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=MAX(");
      OUTS (outf, dregs (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1));
      OUTS (outf, ")(V)");
      return 2 * 2;
    }
  else if (HL == 1 && aopcde == 1)
    {
      notethat ("dregs = dregs +|- dregs, dregs = dregs -|+ dregs (amod0, amod2)");
      OUTS (outf, dregs (dst1));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+|-");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "-|+");
      OUTS (outf, dregs (src1));
      amod0amod2 (s, x, aop, outf);
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 4)
    {
      notethat ("dregs = dregs + dregs amod1");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+");
      OUTS (outf, dregs (src1));
      OUTS (outf, " ");
      amod1 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 0)
    {
      notethat ("dregs = dregs +|+ dregs amod0");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src0));
      OUTS (outf, "+|+");
      OUTS (outf, dregs (src1));
      OUTS (outf, " ");
      amod0 (s, x, outf);
      return 2 * 2;
    }
  else if (aop == 0 && aopcde == 24)
    {
      notethat ("dregs = BYTEPACK ( dregs , dregs )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=BYTEPACK(");
      OUTS (outf, dregs (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (aop == 1 && aopcde == 24)
    {
      notethat ("( dregs , dregs ) = BYTEUNPACK dregs_pair aligndir");
      OUTS (outf, "(");
      OUTS (outf, dregs (dst1));
      OUTS (outf, ",");
      OUTS (outf, dregs (dst0));
      OUTS (outf, ") = BYTEUNPACK ");
      OUTS (outf, dregs (src0 + 1));
      OUTS (outf, ":");
      OUTS (outf, imm5 (src0));
      OUTS (outf, " ");
      aligndir (s, outf);
      return 2 * 2;
    }
  else if (aopcde == 13)
    {
      notethat ("( dregs , dregs ) = SEARCH dregs (searchmod)");
      OUTS (outf, "(");
      OUTS (outf, dregs (dst1));
      OUTS (outf, ",");
      OUTS (outf, dregs (dst0));
      OUTS (outf, ") = SEARCH ");
      OUTS (outf, dregs (src0));
      OUTS (outf, "(");
      searchmod (aop, outf);
      OUTS (outf, ")");
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_dsp32shift_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* dsp32shift
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 0 | 0 |.M.| 1 | 1 | 0 | 0 | - | - |.sopcde............|
|.sop...|.HLs...|.dst0......| - | - | - |.src0......|.src1......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int HLs  = ((iw1 >> DSP32Shift_HLs_bits) & DSP32Shift_HLs_mask);
  int sop  = ((iw1 >> DSP32Shift_sop_bits) & DSP32Shift_sop_mask);
  int src0 = ((iw1 >> DSP32Shift_src0_bits) & DSP32Shift_src0_mask);
  int src1 = ((iw1 >> DSP32Shift_src1_bits) & DSP32Shift_src1_mask);
  int dst0 = ((iw1 >> DSP32Shift_dst0_bits) & DSP32Shift_dst0_mask);
  int sopcde = ((iw0 >> (DSP32Shift_sopcde_bits - 16)) & DSP32Shift_sopcde_mask);
  const char *acc01 = (HLs & 1) == 0 ? "A0" : "A1";


  if (HLs == 0 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_lo = ASHIFT dregs_lo BY dregs_lo");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (HLs == 1 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_lo = ASHIFT dregs_hi BY dregs_lo");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (HLs == 2 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_hi = ASHIFT dregs_lo BY dregs_lo");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (HLs == 3 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_hi = ASHIFT dregs_hi BY dregs_lo");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (HLs == 0 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_lo = ASHIFT dregs_lo BY dregs_lo (S)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (HLs == 1 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_lo = ASHIFT dregs_hi BY dregs_lo (S)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (HLs == 2 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_hi = ASHIFT dregs_lo BY dregs_lo (S)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (HLs == 3 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_hi = ASHIFT dregs_hi BY dregs_lo (S)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 0)
    {
      notethat ("dregs_hilo = LSHIFT dregs_hilo BY dregs_lo");
      OUTS (outf, (HLs & 2) == 0 ? dregs_lo (dst0) : dregs_hi (dst0));
      OUTS (outf, "= LSHIFT ");
      OUTS (outf, (HLs & 1) == 0 ? dregs_lo (src1) : dregs_hi (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 3)
    {
      notethat ("An = ASHIFT An BY dregs_lo");
      OUTS (outf, acc01);
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, acc01);
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 3)
    {
      notethat ("An = LSHIFT An BY dregs_lo");
      OUTS (outf, acc01);
      OUTS (outf, "= LSHIFT ");
      OUTS (outf, acc01);
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 3)
    {
      notethat ("An = ROT An BY dregs_lo");
      OUTS (outf, acc01);
      OUTS (outf, "= ROT ");
      OUTS (outf, acc01);
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 3)
    {
      notethat ("dregs = ROT dregs BY dregs_lo");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ROT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 1)
    {
      notethat ("dregs = ASHIFT dregs BY dregs_lo (V, S)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(V,S)");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 1)
    {
      notethat ("dregs = ASHIFT dregs BY dregs_lo (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(V)");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 2)
    {
      notethat ("dregs = ASHIFT dregs BY dregs_lo");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 2)
    {
      notethat ("dregs = ASHIFT dregs BY dregs_lo (S)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ASHIFT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 2)
    {
      notethat ("dregs = SHIFT dregs BY dregs_lo");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=SHIFT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 2)
    {
      notethat ("dregs = ROT dregs BY dregs_lo");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ROT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 1)
    {
      notethat ("dregs = SHIFT dregs BY dregs_lo (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=SHIFT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, "(V)");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 4)
    {
      notethat ("dregs = PACK ( dregs_lo , dregs_lo )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=PACK");
      OUTS (outf, "(");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 4)
    {
      notethat ("dregs = PACK ( dregs_lo , dregs_hi )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=PACK(");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 4)
    {
      notethat ("dregs = PACK ( dregs_hi , dregs_lo )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=PACK(");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 4)
    {
      notethat ("dregs = PACK ( dregs_hi , dregs_hi )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=PACK(");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_hi (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 5)
    {
      notethat ("dregs_lo = SIGNBITS dregs");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=SIGNBITS ");
      OUTS (outf, dregs (src1));
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 5)
    {
      notethat ("dregs_lo = SIGNBITS dregs_lo");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=SIGNBITS ");
      OUTS (outf, dregs_lo (src1));
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 5)
    {
      notethat ("dregs_lo = SIGNBITS dregs_hi");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=SIGNBITS ");
      OUTS (outf, dregs_hi (src1));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 6)
    {
      notethat ("dregs_lo = SIGNBITS A0");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=SIGNBITS A0");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 6)
    {
      notethat ("dregs_lo = SIGNBITS A1");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=SIGNBITS A1");
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 6)
    {
      notethat ("dregs_lo = ONES dregs");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=ONES ");
      OUTS (outf, dregs (src1));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 7)
    {
      notethat ("dregs_lo = EXPADJ (dregs , dregs_lo)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=EXPADJ (");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 7)
    {
      notethat ("dregs_lo = EXPADJ (dregs , dregs_lo) (V)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=EXPADJ (");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ") (V)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 7)
    {
      notethat ("dregs_lo = EXPADJ (dregs_lo , dregs_lo)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=EXPADJ (");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 7)
    {
      notethat ("dregs_lo = EXPADJ (dregs_hi , dregs_lo)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=EXPADJ (");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 8)
    {
      notethat ("BITMUX (dregs , dregs , A0) (ASR)");
      OUTS (outf, "BITMUX (");
      OUTS (outf, dregs (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",A0 )(ASR)");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 8)
    {
      notethat ("BITMUX (dregs , dregs , A0) (ASL)");
      OUTS (outf, "BITMUX (");
      OUTS (outf, dregs (src0));
      OUTS (outf, ",");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",A0 )(ASL)");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 9)
    {
      notethat ("dregs_lo = VIT_MAX (dregs) (ASL)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=VIT_MAX (");
      OUTS (outf, dregs (src1));
      OUTS (outf, ") (ASL)");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 9)
    {
      notethat ("dregs_lo = VIT_MAX (dregs) (ASR)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=VIT_MAX (");
      OUTS (outf, dregs (src1));
      OUTS (outf, ") (ASR)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 9)
    {
      notethat ("dregs = VIT_MAX ( dregs , dregs ) (ASL)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=VIT_MAX(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")(ASL)");
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 9)
    {
      notethat ("dregs = VIT_MAX ( dregs , dregs ) (ASR)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=VIT_MAX(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")(ASR)");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 10)
    {
      notethat ("dregs = EXTRACT ( dregs , dregs_lo ) (Z)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=EXTRACT(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ") (Z)");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 10)
    {
      notethat ("dregs = EXTRACT ( dregs , dregs_lo ) (X)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=EXTRACT(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs_lo (src0));
      OUTS (outf, ")(X)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 10)
    {
      notethat ("dregs = DEPOSIT ( dregs , dregs )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=DEPOSIT(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 10)
    {
      notethat ("dregs = DEPOSIT ( dregs , dregs ) (X)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=DEPOSIT(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")(X)");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 11)
    {
      notethat ("dregs_lo = CC = BXORSHIFT ( A0 , dregs )");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=CC=BXORSHIFT(A0,");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 11)
    {
      notethat ("dregs_lo = CC = BXOR (A0 , dregs)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=CC=BXOR(A0,");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 12)
    {
      notethat ("A0 = BXORSHIFT ( A0 , A1 , CC )");
      OUTS (outf, "A0=BXORSHIFT(A0,A1 ,CC)");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 12)
    {
      notethat ("dregs_lo = CC = BXOR (A0 , A1 , CC)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=CC=BXOR( A0,A1 ,CC )");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 13)
    {
      notethat ("dregs = ALIGN8 ( dregs , dregs )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=ALIGN8(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 13)
    {
      notethat ("dregs = ALIGN16 ( dregs , dregs )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=ALIGN16(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 13)
    {
      notethat ("dregs = ALIGN24 ( dregs , dregs )");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=ALIGN24(");
      OUTS (outf, dregs (src1));
      OUTS (outf, ",");
      OUTS (outf, dregs (src0));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_dsp32shiftimm_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* dsp32shiftimm
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 0 | 0 |.M.| 1 | 1 | 0 | 1 | - | - |.sopcde............|
|.sop...|.HLs...|.dst0......|.immag.................|.src1......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int src1 = ((iw1 >> DSP32ShiftImm_src1_bits) & DSP32ShiftImm_src1_mask);
  int sop = ((iw1 >> DSP32ShiftImm_sop_bits) & DSP32ShiftImm_sop_mask);
  int bit8 = ((iw1 >> 8) & 0x1);
  int immag = ((iw1 >> DSP32ShiftImm_immag_bits) & DSP32ShiftImm_immag_mask);
  int newimmag = (-(iw1 >> DSP32ShiftImm_immag_bits) & DSP32ShiftImm_immag_mask);
  int dst0 = ((iw1 >> DSP32ShiftImm_dst0_bits) & DSP32ShiftImm_dst0_mask);
  int sopcde = ((iw0 >> (DSP32ShiftImm_sopcde_bits - 16)) & DSP32ShiftImm_sopcde_mask);
  int HLs = ((iw1 >> DSP32ShiftImm_HLs_bits) & DSP32ShiftImm_HLs_mask);


  if (HLs == 0 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_lo = dregs_lo >>> uimm4");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ">>>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 1 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_lo = dregs_hi >>> uimm4");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, ">>>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 2 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_hi = dregs_lo >>> uimm4");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ">>>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 3 && sop == 0 && sopcde == 0)
    {
      notethat ("dregs_hi = dregs_hi >>> uimm4");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, ">>>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 0 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_lo = dregs_lo << uimm4 (S)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm4 (immag));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (HLs == 1 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_lo = dregs_hi << uimm4 (S)");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm4 (immag));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (HLs == 2 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_hi = dregs_lo << uimm4 (S)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm4 (immag));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (HLs == 3 && sop == 1 && sopcde == 0)
    {
      notethat ("dregs_hi = dregs_hi << uimm4 (S)");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm4 (immag));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (HLs == 0 && sop == 2 && sopcde == 0 && bit8 == 0)
    {
      notethat ("dregs_lo = dregs_lo << uimm4");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm4 (immag));
      return 2 * 2;
    }
  else if (HLs == 0 && sop == 2 && sopcde == 0 && bit8 == 1)
    {
      notethat ("dregs_lo = dregs_lo >> uimm4");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ">>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 1 && sop == 2 && sopcde == 0)
    {
      notethat ("dregs_lo = dregs_hi >> uimm4");
      OUTS (outf, dregs_lo (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, ">>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 2 && sop == 2 && sopcde == 0 && bit8 == 1)
    {
      notethat ("dregs_hi = dregs_lo >> uimm4");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, ">>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 2 && sop == 2 && sopcde == 0 && bit8 == 0)
    {
      notethat ("dregs_hi = dregs_lo << uimm4");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_lo (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm4 (immag));
      return 2 * 2;
    }
  else if (HLs == 3 && sop == 2 && sopcde == 0 && bit8 == 1)
    {
      notethat ("dregs_hi = dregs_hi >> uimm4");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, ">>");
      OUTS (outf, uimm4 (newimmag));
      return 2 * 2;
    }
  else if (HLs == 3 && sop == 2 && sopcde == 0 && bit8 == 0)
    {
      notethat ("dregs_hi = dregs_hi << uimm4");
      OUTS (outf, dregs_hi (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs_hi (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm4 (immag));
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 3 && HLs == 1)
    {
      notethat ("A1 = ROT A1 BY imm6");
      OUTS (outf, "A1= ROT A1 BY ");
      OUTS (outf, imm6 (immag));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 3 && HLs == 0 && bit8 == 0)
    {
      notethat ("A0 = A0 << uimm5");
      OUTS (outf, "A0=A0<<");
      OUTS (outf, uimm5 (immag));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 3 && HLs == 0 && bit8 == 1)
    {
      notethat ("A0 = A0 >>> uimm5");
      OUTS (outf, "A0=A0>>>");
      OUTS (outf, uimm5 (newimmag));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 3 && HLs == 1 && bit8 == 0)
    {
      notethat ("A1 = A1 << uimm5");
      OUTS (outf, "A1=A1<<");
      OUTS (outf, uimm5 (immag));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 3 && HLs == 1 && bit8 == 1)
    {
      notethat ("A1 = A1 >>> uimm5");
      OUTS (outf, "A1=A1>>>");
      OUTS (outf, uimm5 (newimmag));
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 3 && HLs == 0)
    {
      notethat ("A0 = A0 >> uimm5");
      OUTS (outf, "A0=A0>>");
      OUTS (outf, uimm5 (newimmag));
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 3 && HLs == 1)
    {
      notethat ("A1 = A1 >> uimm5");
      OUTS (outf, "A1=A1>>");
      OUTS (outf, uimm5 (newimmag));
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 3 && HLs == 0)
    {
      notethat ("A0 = ROT A0 BY imm6");
      OUTS (outf, "A0= ROT A0 BY ");
      OUTS (outf, imm6 (immag));
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 1 && bit8 == 0)
    {
      notethat ("dregs = dregs << uimm5 (V, S)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm5 (immag));
      OUTS (outf, " (V, S)");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 1 && bit8 == 1)
    {
      notethat ("dregs = dregs >>> uimm5 (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, ">>>");
      OUTS (outf, imm5 (-immag));
      OUTS (outf, " (V)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 1 && bit8 == 1)
    {
      notethat ("dregs = dregs >> uimm5 (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, " >> ");
      OUTS (outf, uimm5 (newimmag));
      OUTS (outf, " (V)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 1 && bit8 == 0)
    {
      notethat ("dregs = dregs << imm5 (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, "<<");
      OUTS (outf, imm5 (immag));
      OUTS (outf, " (V)");
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 1)
    {
      notethat ("dregs = dregs >>> uimm5 (V)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, ">>>");
      OUTS (outf, uimm5 (newimmag));
      OUTS (outf, " (V)");
      return 2 * 2;
    }
  else if (sop == 1 && sopcde == 2)
    {
      notethat ("dregs = dregs << uimm5 (S)");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm5 (immag));
      OUTS (outf, "(S)");
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 2 && bit8 == 1)
    {
      notethat ("dregs = dregs >> uimm5");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, ">>");
      OUTS (outf, uimm5 (newimmag));
      return 2 * 2;
    }
  else if (sop == 2 && sopcde == 2 && bit8 == 0)
    {
      notethat ("dregs = dregs << uimm5");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, "<<");
      OUTS (outf, uimm5 (immag));
      return 2 * 2;
    }
  else if (sop == 3 && sopcde == 2)
    {
      notethat ("dregs = ROT dregs BY imm6");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "= ROT ");
      OUTS (outf, dregs (src1));
      OUTS (outf, " BY ");
      OUTS (outf, imm6 (immag));
      return 2 * 2;
    }
  else if (sop == 0 && sopcde == 2)
    {
      notethat ("dregs = dregs >>> uimm5");
      OUTS (outf, dregs (dst0));
      OUTS (outf, "=");
      OUTS (outf, dregs (src1));
      OUTS (outf, ">>>");
      OUTS (outf, uimm5 (newimmag));
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_pseudoDEBUG_0 (TIword iw0, disassemble_info *outf)
{
/* pseudoDEBUG
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 1 | 1 | 1 | 0 | 0 | 0 |.fn....|.grp.......|.reg.......|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int fn  = ((iw0 >> PseudoDbg_fn_bits) & PseudoDbg_fn_mask);
  int grp = ((iw0 >> PseudoDbg_grp_bits) & PseudoDbg_grp_mask);
  int reg = ((iw0 >> PseudoDbg_reg_bits) & PseudoDbg_reg_mask);

  if (reg == 0 && fn == 3)
    {
      notethat ("DBG A0");
      OUTS (outf, "DBG A0");
      return 1 * 2;
    }
  else if (reg == 1 && fn == 3)
    {
      notethat ("DBG A1");
      OUTS (outf, "DBG A1");
      return 1 * 2;
    }
  else if (reg == 3 && fn == 3)
    {
      notethat ("ABORT");
      OUTS (outf, "ABORT");
      return 1 * 2;
    }
  else if (reg == 4 && fn == 3)
    {
      notethat ("HLT");
      OUTS (outf, "HLT");
      return 1 * 2;
    }
  else if (reg == 5 && fn == 3)
    {
      notethat ("DBGHALT");
      OUTS (outf, "DBGHALT");
      return 1 * 2;
    }
  else if (reg == 6 && fn == 3)
    {
      notethat ("DBGCMPLX ( dregs )");
      OUTS (outf, "DBGCMPLX(");
      OUTS (outf, dregs (grp));
      OUTS (outf, ")");
      return 1 * 2;
    }
  else if (reg == 7 && fn == 3)
    {
      notethat ("DBG");
      OUTS (outf, "DBG");
      return 1 * 2;
    }
  else if (grp == 0 && fn == 2)
    {
      notethat ("OUTC dregs");
      OUTS (outf, "OUTC");
      OUTS (outf, dregs (reg));
      return 1 * 2;
    }
  else if (fn == 0)
    {
      notethat ("DBG allregs");
      OUTS (outf, "DBG");
      OUTS (outf, allregs (reg, grp));
      return 1 * 2;
    }
  else if (fn == 1)
    {
      notethat ("PRNT allregs");
      OUTS (outf, "PRNT");
      OUTS (outf, allregs (reg, grp));
      return 1 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

static int
decode_pseudodbg_assert_0 (TIword iw0, TIword iw1, disassemble_info *outf)
{
/* pseudodbg_assert
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
| 1 | 1 | 1 | 1 | 0 | - | - | - | - | - |.dbgop.....|.regtest...|
|.expected......................................................|
+---+---+---+---|---+---+---+---|---+---+---+---|---+---+---+---+
*/
  int expected = ((iw1 >> PseudoDbg_Assert_expected_bits) & PseudoDbg_Assert_expected_mask);
  int dbgop = ((iw0 >> (PseudoDbg_Assert_dbgop_bits - 16)) & PseudoDbg_Assert_dbgop_mask);
  int regtest = ((iw0 >> (PseudoDbg_Assert_regtest_bits - 16)) & PseudoDbg_Assert_regtest_mask);

  if (dbgop == 0)
    {
      notethat ("DBGA ( dregs_lo , uimm16 )");
      OUTS (outf, "DBGA(");
      OUTS (outf, dregs_lo (regtest));
      OUTS (outf, ",");
      OUTS (outf, uimm16 (expected));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (dbgop == 1)
    {
      notethat ("DBGA ( dregs_hi , uimm16 )");
      OUTS (outf, "DBGA(");
      OUTS (outf, dregs_hi (regtest));
      OUTS (outf, ",");
      OUTS (outf, uimm16 (expected));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (dbgop == 2)
    {
      notethat ("DBGAL ( dregs , uimm16 )");
      OUTS (outf, "DBGAL(");
      OUTS (outf, dregs (regtest));
      OUTS (outf, ",");
      OUTS (outf, uimm16 (expected));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else if (dbgop == 3)
    {
      notethat ("DBGAH ( dregs , uimm16 )");
      OUTS (outf, "DBGAH(");
      OUTS (outf, dregs (regtest));
      OUTS (outf, ",");
      OUTS (outf, uimm16 (expected));
      OUTS (outf, ")");
      return 2 * 2;
    }
  else
    goto illegal_instruction;
illegal_instruction:
  return 0;
}

int
_print_insn_bfin (bfd_vma pc, disassemble_info *outf)
{

  bfd_byte buf[4];
  TIword iw0;
  TIword iw1;
  int status;
  status = (*outf->read_memory_func) (pc & ~0x1, buf, 2, outf);
  status = (*outf->read_memory_func) ((pc + 2) & ~0x1, buf + 2, 2, outf);

  iw0 = bfd_getl16 (buf);
  iw1 = bfd_getl16 (buf + 2);

  if ((iw0 & 0xf7ff) == 0xc003 && iw1 == 0x1800)
    {
      OUTS (outf, "mnop");
      return 4;
    }
  else if ((iw0 & 0xff00) == 0x0000)
    {
      int rv = decode_ProgCtrl_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xffc0) == 0x0240)
    {
      int rv = decode_CaCTRL_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xff80) == 0x0100)
    {
      int rv = decode_PushPopReg_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfe00) == 0x0400)
    {
      int rv = decode_PushPopMultiple_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfe00) == 0x0600)
    {
      int rv = decode_ccMV_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf800) == 0x0800)
    {
      int rv = decode_CCflag_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xffe0) == 0x0200)
    {
      int rv = decode_CC2dreg_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xff00) == 0x0300)
    {
      int rv = decode_CC2stat_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf000) == 0x1000)
    {
      int rv = decode_BRCC_0 (iw0, pc, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf000) == 0x2000)
    {
      int rv = decode_UJUMP_0 (iw0, pc, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf000) == 0x3000)
    {
      int rv = decode_REGMV_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfc00) == 0x4000)
    {
      int rv = decode_ALU2op_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfe00) == 0x4400)
    {
      int rv = decode_PTR2op_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf800) == 0x4800)
    {
      int rv = decode_LOGI2op_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf000) == 0x5000)
    {
      int rv = decode_COMP3op_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf800) == 0x6000)
    {
      int rv = decode_COMPI2opD_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf800) == 0x6800)
    {
      int rv = decode_COMPI2opP_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf000) == 0x8000)
    {
      int rv = decode_LDSTpmod_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xff60) == 0x9e60)
    {
      int rv = decode_dagMODim_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfff0) == 0x9f60)
    {
      int rv = decode_dagMODik_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfc00) == 0x9c00)
    {
      int rv = decode_dspLDST_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf000) == 0x9000)
    {
      int rv = decode_LDST_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfc00) == 0xb800)
    {
      int rv = decode_LDSTiiFP_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xe000) == 0xA000)
    {
      int rv = decode_LDSTii_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xff80) == 0xe080 && (iw1 & 0x0C00) == 0x0000)
    {
      int rv = decode_LoopSetup_0 (iw0, iw1, pc, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xff00) == 0xe100 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_LDIMMhalf_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfe00) == 0xe200 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_CALLa_0 (iw0, iw1, pc, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfc00) == 0xe400 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_LDSTidxI_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xfffe) == 0xe800 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_linkage_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf600) == 0xc000 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_dsp32mac_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf600) == 0xc200 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_dsp32mult_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf7c0) == 0xc400 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_dsp32alu_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf780) == 0xc600 && (iw1 & 0x01c0) == 0x0000)
    {
      int rv = decode_dsp32shift_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xf780) == 0xc680 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_dsp32shiftimm_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }
  else if ((iw0 & 0xff00) == 0xf800)
    {
      int rv = decode_pseudoDEBUG_0 (iw0, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
#if 0
    }
  else if ((iw0 & 0xFF00) == 0xF900)
    {

      int rv = decode_pseudoOChar_0 (iw0, iw1, pc, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
#endif
    }
  else if ((iw0 & 0xFFC0) == 0xf000 && (iw1 & 0x0000) == 0x0000)
    {
      int rv = decode_pseudodbg_assert_0 (iw0, iw1, outf);
      if (rv)
	return rv;
      goto illegal_instruction;
    }

illegal_instruction:
  return 0;
}


int
print_insn_bfin (bfd_vma pc, disassemble_info *outf)
{
  bfd_byte buf[2];
  unsigned short iw0;
  int status;
  int count = 0;

  status = (*outf->read_memory_func) (pc & ~0x01, buf, 2, outf);
  iw0 = bfd_getl16 (buf);

  count += _print_insn_bfin (pc, outf);

  /* Proper display of multiple issue instructions.  */

  if ((iw0 & 0xc000) == 0xc000 && (iw0 & BIT_MULTI_INS)
      && ((iw0 & 0xe800) != 0xe800 /* not Linkage */ ))
    {
      outf->fprintf_func (outf->stream, " || ");
      count += _print_insn_bfin (pc + 4, outf);
      outf->fprintf_func (outf->stream, " || ");
      count += _print_insn_bfin (pc + 6, outf);
    }
  if (count == 0)
    {
      outf->fprintf_func (outf->stream, "ILLEGAL");
      return 2;
    }
  outf->fprintf_func (outf->stream, ";");
  return count;
}
