/* Simulator header for m32r.

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

#ifndef M32R_ARCH_H
#define M32R_ARCH_H

#define TARGET_BIG_ENDIAN 1

/* Cover fns for register access.  */
USI a_m32r_h_pc_get (SIM_CPU *);
void a_m32r_h_pc_set (SIM_CPU *, USI);
SI a_m32r_h_gr_get (SIM_CPU *, UINT);
void a_m32r_h_gr_set (SIM_CPU *, UINT, SI);
USI a_m32r_h_cr_get (SIM_CPU *, UINT);
void a_m32r_h_cr_set (SIM_CPU *, UINT, USI);
DI a_m32r_h_accum_get (SIM_CPU *);
void a_m32r_h_accum_set (SIM_CPU *, DI);
DI a_m32r_h_accums_get (SIM_CPU *, UINT);
void a_m32r_h_accums_set (SIM_CPU *, UINT, DI);
BI a_m32r_h_cond_get (SIM_CPU *);
void a_m32r_h_cond_set (SIM_CPU *, BI);
UQI a_m32r_h_psw_get (SIM_CPU *);
void a_m32r_h_psw_set (SIM_CPU *, UQI);
UQI a_m32r_h_bpsw_get (SIM_CPU *);
void a_m32r_h_bpsw_set (SIM_CPU *, UQI);
UQI a_m32r_h_bbpsw_get (SIM_CPU *);
void a_m32r_h_bbpsw_set (SIM_CPU *, UQI);
BI a_m32r_h_lock_get (SIM_CPU *);
void a_m32r_h_lock_set (SIM_CPU *, BI);

/* Enum declaration for model types.  */
typedef enum model_type {
  MODEL_M32R_D, MODEL_TEST
 , MODEL_MAX
} MODEL_TYPE;

#define MAX_MODELS ((int) MODEL_MAX)

/* Enum declaration for unit types.  */
typedef enum unit_type {
  UNIT_NONE, UNIT_M32R_D_U_STORE, UNIT_M32R_D_U_LOAD, UNIT_M32R_D_U_CTI
 , UNIT_M32R_D_U_MAC, UNIT_M32R_D_U_CMP, UNIT_M32R_D_U_EXEC, UNIT_TEST_U_EXEC
 , UNIT_MAX
} UNIT_TYPE;

#define MAX_UNITS (2)

#endif /* M32R_ARCH_H */
