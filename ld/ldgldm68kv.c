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
 *  $Id$
*/

/* 
 * emulate the original gld running on an m68kv system
 *
 *  Written by David Wood @ New York University 
 */


#include "sysdep.h"
#include "bfd.h"

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



static void gldm68kv_before_parse()
{
#ifndef NOTDEF		/* Cross developing for now */
  ldfile_add_library_path("/usr/triton/lib/68kgcc-lib/lib");
  ldfile_add_library_path("/usr/triton/lib/68kgcc-lib/usr/lib");
  ldfile_add_library_path("/usr/triton/lib/68kgcc-lib/usr/local/lib");
#endif
  ldfile_output_architecture = bfd_arch_m68k;
}


static void 
gldm68kv_after_parse()
{

}

static void
gldm68kv_after_allocation()
{

}

static void
gldm68kv_before_allocation()
{

}

static void
gldm68kv_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned long  machine = 0;
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, machine);
}

static char *
gldm68kv_choose_target()
{
  char *from_outside = getenv(TARGET_ENVIRON);
  if (from_outside != (char *)NULL)
    return from_outside;
  return GLDm68kv_TARGET;
}

static void
gldm68kv_syslib()
{
  info("%S SYSLIB ignored\n");
}

static void
gldm68kv_hll(ignore)
char  *ignore;
{
  info("%S HLL ignored\n");
}

static char *gldm68kv_script  =  
#include "ldgldm68kv.x"
;

static char *gldm68kv_script_option_Ur  =  
#include "ldgldm68kvUr.x"
;
static char *gldm68kv_script_option_r  =  
#include "ldgldm68kvr.x"
;			     
static char *gldm68kv_get_script()
{			     
  extern ld_config_type config;
  if (config.relocateable_output == true &&
      config.build_constructors == true) {
    return gldm68kv_script_option_Ur;
  }
  if (config.relocateable_output) {
    return gldm68kv_script_option_r;
  }
	
  return gldm68kv_script;
}
struct ld_emulation_xfer_struct ld_gldm68kv_emulation = 
{
  gldm68kv_before_parse,
  gldm68kv_syslib,
  gldm68kv_hll,
  gldm68kv_after_parse,
  gldm68kv_after_allocation,
  gldm68kv_set_output_arch,
  gldm68kv_choose_target,
  gldm68kv_before_allocation,
  gldm68kv_get_script,
};

