/* Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* 
 Written by Steve Chamberlain steve@cygnus.com

 The controller which tells the gnu linker how to behave like one for
 the Hitach H8/300 with IEEE records.
*/

#include "bfd.h"
#include "sysdep.h"

#include "ld.h"
#include "config.h"
#include "ldemul.h"
#include "ldfile.h"
#include "ldmisc.h"

extern  boolean lang_float_flag;


extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;
extern char *ldfile_output_machine_name;

extern bfd *output_bfd;



static void h8300hms_before_parse()
{
  ldfile_output_architecture = bfd_arch_h8300;
}


static void 
h8300hms_after_parse()
{

}

static void
h8300hms_after_allocation()
{

}

static void
h8300hms_before_allocation()
{

}


static void
h8300hms_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  bfd_set_arch_mach(output_bfd,
	            ldfile_output_architecture, ldfile_output_machine);
}

static char *
h8300hms_choose_target()
{
  char *from_outside = getenv(TARGET_ENVIRON);
  if (from_outside != (char *)NULL)
    return from_outside;
  return H8300HMS_TARGET;
}

static void
h8300hms_syslib()
{
  info("%S SYSLIB ignored\n");
}

static void
h8300hms_hll(ignore)
char  *ignore;
{
  info("%S HLL ignored\n");
}

static char *h8300hms_script =  
#include "h8300hms.x"
;
static char *h8300hms_script_option_Ur  =  
#include "h8300hms.x"
;
static char *h8300hms_script_option_r  =  
#include "h8300hms.x"
;

static char *h8300hms_get_script()
{			     
  extern ld_config_type config;
  if (config.relocateable_output == true &&
      config.build_constructors == true) {
    return h8300hms_script_option_Ur;
  }
  if (config.relocateable_output) {
    return h8300hms_script_option_r;
  }
	
  return h8300hms_script;
}
struct ld_emulation_xfer_struct ld_h8300hms_emulation = 
{
  h8300hms_before_parse,
  h8300hms_syslib,
  h8300hms_hll,
  h8300hms_after_parse,
  h8300hms_after_allocation,
  h8300hms_set_output_arch,
  h8300hms_choose_target,
  h8300hms_before_allocation,
  h8300hms_get_script,
};
