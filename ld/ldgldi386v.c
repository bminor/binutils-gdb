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
 * emulate the original gld running on an i386v system
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



static void gldi386v_before_parse()
{
#ifndef NOTDEF		/* Cross developing for now */
  ldfile_add_library_path("/usr/local/lib/386gcc-lib/lib");
  ldfile_add_library_path("/usr/local/lib/386gcc-lib/usr/lib");
  ldfile_add_library_path("/usr/local/lib/386gcc-lib/usr/local/lib");
#endif
  ldfile_output_architecture = bfd_arch_i386;
}


static void 
gldi386v_after_parse()
{

}

static void
gldi386v_after_allocation()
{

}

static void
gldi386v_before_allocation()
{

}

static void
gldi386v_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned long  machine = 0;
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, machine);
}

static char *
gldi386v_choose_target()
{
  char *from_outside = getenv(TARGET_ENVIRON);
  if (from_outside != (char *)NULL)
    return from_outside;
  return GLDi386v_TARGET;
}

static void
gldi386v_syslib()
{
  info("%S SYSLIB ignored\n");
}

static void
gldi386v_hll(ignore)
char  *ignore;
{
  info("%S HLL ignored\n");
}

static char *gldi386v_script  =  
#include "ldgldi386v.x"
;

static char *gldi386v_script_option_Ur  =  
#include "ldgldi386vUr.x"
;
static char *gldi386v_script_option_r  =  
#include "ldgldi386vr.x"
;			     
static char *gldi386v_get_script()
{			     
  extern ld_config_type config;
  if (config.relocateable_output == true &&
      config.build_constructors == true) {
    return gldi386v_script_option_Ur;
  }
  if (config.relocateable_output) {
    return gldi386v_script_option_r;
  }
	
  return gldi386v_script;
}
struct ld_emulation_xfer_struct ld_gldi386v_emulation = 
{
  gldi386v_before_parse,
  gldi386v_syslib,
  gldi386v_hll,
  gldi386v_after_parse,
  gldi386v_after_allocation,
  gldi386v_set_output_arch,
  gldi386v_choose_target,
  gldi386v_before_allocation,
  gldi386v_get_script,
};

