cat >em_${EMULATION_NAME}.c <<EOF
/* Copyright (C) 1993 Free Software Foundation, Inc.

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
 Written by Steve Chamberlain sac@cygnus.com

 The controller which tells the gnu linker how to behave like one for
 the Hitach H8/500.
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



static void h8500hms_before_parse()
{
  ldfile_output_architecture = bfd_arch_h8500;
}

static char *h8500hms_script =  
#include "h8500hms.x"
;
static char *h8500hms_script_option_Ur  =  
#include "h8500hms.xu"
;
static char *h8500hms_script_option_r  =  
#include "h8500hms.xr"
;

static char *h8500hms_get_script()
{			     
  extern ld_config_type config;
  if (config.relocateable_output == true &&
      config.build_constructors == true) {
    return h8500hms_script_option_Ur;
  }
  if (config.relocateable_output) {
    return h8500hms_script_option_r;
  }
	
  return h8500hms_script;
}
struct ld_emulation_xfer_struct ld_h8500hms_emulation = 
{
  h8500hms_before_parse,
  syslib_default,
  hll_default,
  after_parse_default,
  after_allocation_default,
  set_output_arch_default,
  ldemul_default_target,
  before_allocation_default,
  h8500hms_get_script,
  "h8500hms"
};
EOF
