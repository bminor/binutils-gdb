/* Simulator header for m32r.

This file is machine generated.

Copyright (C) 1996, 1997 Free Software Foundation, Inc.

This file is part of the GNU Binutils and/or GDB, the GNU debugger.

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

#ifndef M32R_ARCH_DEFS_H
#define M32R_ARCH_DEFS_H

#define MAX_INSNS 128

#define TARGET_BIG_ENDIAN 1

/* Word and address accesses.  */
/* FIXME: Later need to allow runtime selection.  */

#define GETTWI(addr) GETTSI (addr)
#define GETTUWI(addr) GETTUSI (addr)
#define GETTAI(addr) GETTSI (addr)

#define SETTWI(addr, val) SETTSI ((addr), (val))
#define SETTUWI(addr, val) SETTUSI ((addr), (val))
#define SETTAI(addr, val) SETTSI ((addr), (val))

#define GETMEMWI(cpu, addr) GETMEMSI ((cpu), (addr))
#define GETMEMUWI(cpu, addr) GETMEMUSI ((cpu), (addr))
#define GETMEMAI(cpu, addr) GETMEMSI ((cpu), (addr))

#define SETMEMWI(cpu, addr, val) SETMEMSI ((cpu), (addr), (val))
#define SETMEMUWI(cpu, addr, val) SETMEMUSI ((cpu), (addr), (val))
#define SETMEMAI(cpu, addr, val) SETMEMSI ((cpu), (addr), (val))

/* FIXME: Used? */
#define SETMEM(addr, val, len) (*STATE_MEM_WRITE (current_state)) (current_state, (addr), (val), (len))

/* FIXME */
typedef SI WI;
typedef USI UWI;

/* CPU state information.  */
typedef struct {
  /* program counter */
  SI pc;
  /* general registers */
  SI h_gr[16];
  /* control registers */
  SI h_cr[7];
  /* accumulator */
  DI h_accum;
  /* condition bit */
  UBI h_cond;
  /* sm */
  UBI h_sm;
  /* bsm */
  UBI h_bsm;
  /* ie */
  UBI h_ie;
  /* bie */
  UBI h_bie;
  /* bcond */
  UBI h_bcond;
  /* bpc */
  SI h_bpc;
} CPU_DATA;

#ifdef USING_SIM_BASE_H /* FIXME:quick hack */
#define CPU(x) (STATE_CPU_CPU (current_state, 0)->x)
#else
#define CPU(x) (STATE_CPU (current_state).x)
#endif

/* CPU profiling state information.  */
typedef struct {
  /* general registers */
  unsigned long h_gr;
} CPU_PROFILE;

/* Enum declaration for mode types.  */
typedef enum mode_type {
  MODE_VM, MODE_BI, MODE_QI, MODE_HI,
  MODE_SI, MODE_DI, MODE_UBI, MODE_UQI,
  MODE_UHI, MODE_USI, MODE_UDI, MODE_SF,
  MODE_DF, MODE_XF, MODE_TF, MODE_MAX
} MODE_TYPE;

#define MAX_MODES ((int) MODE_MAX)

/* Return name of instruction numbered INSN.  */
#define INSN_NAME(insn) (m32r_cgen_insn_table_entries[insn].name)

/* Enum declaration for model types.  */
typedef enum model_type {
  MODEL_M32R_D, MODEL_TEST, MODEL_MAX
} MODEL_TYPE;

#define MAX_MODELS ((int) MODEL_MAX)

/* Enum declaration for unit types.  */
typedef enum unit_type {
  UNIT_NONE, UNIT_M32R_D_U_STORE, UNIT_M32R_D_U_LOAD, UNIT_M32R_D_U_EXEC,
  UNIT_TEST_U_EXEC, UNIT_MAX
} UNIT_TYPE;

#define MAX_UNITS (1)

typedef struct argbuf {
  union {
  struct {
    SI * f_r1;
    SI * f_r2;
  } fmt_0;
  struct {
    SI * f_r1;
    SI * f_r2;
    HI f_simm16;
  } fmt_1;
  struct {
    SI * f_r1;
    SI * f_r2;
    USI f_uimm16;
  } fmt_2;
  struct {
    SI * f_r1;
    SI * f_r2;
    UHI f_uimm16;
  } fmt_3;
  struct {
    SI * f_r1;
    SI f_simm8;
  } fmt_4;
  struct {
    SI * f_r1;
    SI * f_r2;
    SI f_simm16;
  } fmt_5;
  struct {
    IADDR f_disp8;
  } fmt_6;
  struct {
    IADDR f_disp24;
  } fmt_7;
  struct {
    SI * f_r1;
    SI * f_r2;
    IADDR f_disp16;
  } fmt_8;
  struct {
    SI * f_r2;
    IADDR f_disp16;
  } fmt_9;
  struct {
    SI * f_r1;
    SI * f_r2;
  } fmt_10;
  struct {
    SI * f_r2;
    SI f_simm16;
  } fmt_11;
  struct {
    SI * f_r2;
    USI f_uimm16;
  } fmt_12;
  struct {
    SI * f_r1;
    SI * f_r2;
  } fmt_13;
  struct {
    SI * f_r2;
  } fmt_14;
  struct {
    SI * f_r1;
    ADDR f_uimm24;
  } fmt_15;
  struct {
    SI * f_r1;
    HI f_simm16;
  } fmt_16;
  struct {
    SI * f_r1;
  } fmt_17;
  struct {
    SI * f_r1;
    UINT f_r2;
  } fmt_18;
  struct {
    SI * f_r1;
  } fmt_19;
  struct {
    UINT f_r1;
    SI * f_r2;
  } fmt_20;
  struct {
    int empty;
  } fmt_21;
  struct {
    SI * f_r1;
    UHI f_hi16;
  } fmt_22;
  struct {
    SI * f_r1;
    USI f_uimm5;
  } fmt_23;
  struct {
    SI * f_r1;
    SI * f_r2;
    HI f_simm16;
  } fmt_24;
  struct {
    USI f_uimm4;
  } fmt_25;
  } fields;
  unsigned int length;
  PCADDR addr;
#if 1 || defined (MODULE_trace) || defined (MODULE_profile) /*FIXME:wip*/
  const struct cgen_insn *opcode;
#endif
#if 1 || WITH_PROFILE_MODEL_P /*FIXME:wip*/
  unsigned long h_gr_get;
  unsigned long h_gr_set;
#endif
} ARGBUF;

#endif /* M32R_ARCH_DEFS_H */
