/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ */


/* This -*- C -*- source file will someday be machine-generated */

/*** Defines the target vector through which operations dispatch */
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"


extern bfd_target aout_little_vec;
extern bfd_target aout_big_vec;
extern bfd_target srec_vec;
extern bfd_target b_out_vec_little_host;
extern bfd_target b_out_vec_big_host;
extern bfd_target icoff_little_vec;
extern bfd_target icoff_big_vec;
extern bfd_target ieee_vec;
extern bfd_target oasys_vec;
extern bfd_target m88k_bcs_vec;

#ifdef GNU960
#define ICOFF_LITTLE_VEC	icoff_little_vec
#define ICOFF_BIG_VEC		icoff_big_vec
#define B_OUT_VEC_LITTLE_HOST	b_out_vec_little_host
#define B_OUT_VEC_BIG_HOST	b_out_vec_big_host
#endif /* GNU960 */

#ifndef RESTRICTED
#define ICOFF_LITTLE_VEC	icoff_little_vec
#define ICOFF_BIG_VEC		icoff_big_vec
#define B_OUT_VEC_LITTLE_HOST	b_out_vec_little_host
#define B_OUT_VEC_BIG_HOST	b_out_vec_big_host
#define AOUT_VEC_LITTLE_HOST	aout_little_vec
#define AOUT_VEC_BIG_HOST	aout_big_vec
#define OASYS_VEC		oasys_vec
#define IEEE_VEC		ieee_vec
#define M88K_BCS_VEC		m88k_bcs_vec
#define SREC_VEC		srec_vec
#endif
bfd_target *target_vector[] = {

#ifdef DEFAULT_VECTOR
	&DEFAULT_VECTOR,
#endif /* DEFAULT_VECTOR */

#ifdef IEEE_VEC
	&IEEE_VEC,
#endif /* IEEE_VEC */

#ifdef OASYS_VEC
	&OASYS_VEC,
#endif /* OASYS_VEC */

#ifdef AOUT_VEC_LITTLE_HOST
	&AOUT_VEC_LITTLE_HOST,
#endif /* AOUT_LITTLE_VEC */

#ifdef AOUT_VEC_BIG_HOST
	&AOUT_VEC_BIG_HOST,
#endif /* AOUT_BIG_VEC */

#ifdef M88K_BCS_VEC
	&M88K_BCS_VEC,
#endif /* M88K_BCS_VEC */

#ifdef SREC_VEC
	&SREC_VEC,
#endif /* SREC_VEC */
	
#ifdef ICOFF_LITTLE_VEC
	&ICOFF_LITTLE_VEC,
#endif /* ICOFF_LITTLE_VEC */

#ifdef ICOFF_BIG_VEC
	&ICOFF_BIG_VEC,
#endif /* ICOFF_BIG_VEC */

#ifdef B_OUT_VEC_LITTLE_HOST
	&B_OUT_VEC_LITTLE_HOST,
#endif /* B_OUT_VEC_LITTLE_HOST */

#ifdef B_OUT_VEC_BIG_HOST
	&B_OUT_VEC_BIG_HOST,
#endif /* B_OUT_VEC_BIG_HOST */

	NULL, /* end of list marker */
};
