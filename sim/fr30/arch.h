/* Simulator header for fr30.

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

#ifndef FR30_ARCH_H
#define FR30_ARCH_H

#define TARGET_BIG_ENDIAN 1

/* Cover fns for register access.  */
USI a_fr30_h_pc_get (SIM_CPU *);
void a_fr30_h_pc_set (SIM_CPU *, USI);
SI a_fr30_h_gr_get (SIM_CPU *, UINT);
void a_fr30_h_gr_set (SIM_CPU *, UINT, SI);
SI a_fr30_h_cr_get (SIM_CPU *, UINT);
void a_fr30_h_cr_set (SIM_CPU *, UINT, SI);
SI a_fr30_h_dr_get (SIM_CPU *, UINT);
void a_fr30_h_dr_set (SIM_CPU *, UINT, SI);
USI a_fr30_h_ps_get (SIM_CPU *);
void a_fr30_h_ps_set (SIM_CPU *, USI);
SI a_fr30_h_r13_get (SIM_CPU *);
void a_fr30_h_r13_set (SIM_CPU *, SI);
SI a_fr30_h_r14_get (SIM_CPU *);
void a_fr30_h_r14_set (SIM_CPU *, SI);
SI a_fr30_h_r15_get (SIM_CPU *);
void a_fr30_h_r15_set (SIM_CPU *, SI);
BI a_fr30_h_nbit_get (SIM_CPU *);
void a_fr30_h_nbit_set (SIM_CPU *, BI);
BI a_fr30_h_zbit_get (SIM_CPU *);
void a_fr30_h_zbit_set (SIM_CPU *, BI);
BI a_fr30_h_vbit_get (SIM_CPU *);
void a_fr30_h_vbit_set (SIM_CPU *, BI);
BI a_fr30_h_cbit_get (SIM_CPU *);
void a_fr30_h_cbit_set (SIM_CPU *, BI);
BI a_fr30_h_ibit_get (SIM_CPU *);
void a_fr30_h_ibit_set (SIM_CPU *, BI);
BI a_fr30_h_sbit_get (SIM_CPU *);
void a_fr30_h_sbit_set (SIM_CPU *, BI);
BI a_fr30_h_tbit_get (SIM_CPU *);
void a_fr30_h_tbit_set (SIM_CPU *, BI);
BI a_fr30_h_d0bit_get (SIM_CPU *);
void a_fr30_h_d0bit_set (SIM_CPU *, BI);
BI a_fr30_h_d1bit_get (SIM_CPU *);
void a_fr30_h_d1bit_set (SIM_CPU *, BI);
UQI a_fr30_h_ccr_get (SIM_CPU *);
void a_fr30_h_ccr_set (SIM_CPU *, UQI);
UQI a_fr30_h_scr_get (SIM_CPU *);
void a_fr30_h_scr_set (SIM_CPU *, UQI);
UQI a_fr30_h_ilm_get (SIM_CPU *);
void a_fr30_h_ilm_set (SIM_CPU *, UQI);

/* Enum declaration for model types.  */
typedef enum model_type {
  MODEL_FR30_1, MODEL_MAX
} MODEL_TYPE;

#define MAX_MODELS ((int) MODEL_MAX)

/* Enum declaration for unit types.  */
typedef enum unit_type {
  UNIT_NONE, UNIT_FR30_1_U_STM, UNIT_FR30_1_U_LDM, UNIT_FR30_1_U_STORE
 , UNIT_FR30_1_U_LOAD, UNIT_FR30_1_U_CTI, UNIT_FR30_1_U_EXEC, UNIT_MAX
} UNIT_TYPE;

#define MAX_UNITS (3)

#endif /* FR30_ARCH_H */
