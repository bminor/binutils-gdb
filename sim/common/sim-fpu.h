/* Simulator Floating-point support.
   Copyright (C) 1997 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

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
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */



#ifndef _SIM_FPU_H_
#define _SIM_FPU_H_

/* the FPU intermediate type */

typedef struct _sim_fpu {
  double val;
} sim_fpu;


/* Arrithmetic operators */

INLINE_SIM_FPU (sim_fpu) sim_fpu_add (sim_fpu l, sim_fpu r);
INLINE_SIM_FPU (sim_fpu) sim_fpu_sub (sim_fpu l, sim_fpu r);
INLINE_SIM_FPU (sim_fpu) sim_fpu_mul (sim_fpu l, sim_fpu r);
INLINE_SIM_FPU (sim_fpu) sim_fpu_div (sim_fpu l, sim_fpu r);
INLINE_SIM_FPU (sim_fpu) sim_fpu_inv (sim_fpu l);
INLINE_SIM_FPU (sim_fpu) sim_fpu_sqrt (sim_fpu sqr);


/* Conversions single/double to/from fpu type */

INLINE_SIM_FPU (sim_fpu) sim_fpu_32to (unsigned32 s);
INLINE_SIM_FPU (sim_fpu) sim_fpu_64to (unsigned64 s);

INLINE_SIM_FPU (unsigned32) sim_fpu_to32 (sim_fpu s);
INLINE_SIM_FPU (unsigned64) sim_fpu_to64 (sim_fpu s);

INLINE_SIM_FPU (float) sim_fpu_2f (sim_fpu f);
INLINE_SIM_FPU (double) sim_fpu_2d (sim_fpu d);

INLINE_SIM_FPU (sim_fpu) sim_fpu_f2 (float f);
INLINE_SIM_FPU (sim_fpu) sim_fpu_d2 (double d);


/* Signalling or NonSignalling NaN */

INLINE_SIM_FPU (int) sim_fpu_is_nan (sim_fpu s);


/* compare l with r; return <0, ==0, >0 accordingly */

INLINE_SIM_FPU (int) sim_fpu_cmp (sim_fpu l, sim_fpu r);


#endif
