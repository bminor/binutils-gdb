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



bfd_target *target_vector[] = 
  {
#ifndef GNU960
    &ieee_vec,
    &oasys_vec,
    &aout_little_vec,
    &aout_big_vec,
/*    &m88k_bcs_vec,*/
    &srec_vec,
#endif /* GNU960 */
    &icoff_little_vec,
    &icoff_big_vec,
    &b_out_vec_little_host,
    &b_out_vec_big_host,
    NULL
    };


