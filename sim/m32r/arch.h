/* Simulator header for m32r.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

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

#define MAX_INSNS 166

#define TARGET_BIG_ENDIAN 1

/* Shorthand macro for fetching registers.  */
#define CPU(x) (CPU_CGEN_HW (current_cpu)->x)

/* Macros to determine which cpus are supported.  */
#define HAVE_CPU_M32R
/* start-sanitize-m32rx */
/*#define HAVE_CPU_M32RX*/
/* end-sanitize-m32rx */

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
  MODEL_M32R_D, MODEL_TEST, MODEL_M32RX, MODEL_MAX
} MODEL_TYPE;

#define MAX_MODELS ((int) MODEL_MAX)

/* Enum declaration for unit types.  */
typedef enum unit_type {
  UNIT_NONE, UNIT_M32R_D_U_STORE, UNIT_M32R_D_U_LOAD, UNIT_M32R_D_U_EXEC,
  UNIT_TEST_U_EXEC, UNIT_M32RX_U_EXEC, UNIT_MAX
} UNIT_TYPE;

#define MAX_UNITS (1)

#endif /* M32R_ARCH_H */
