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
 *
*/

/* 
 *  A vanilla emulation with no defaults
 *
 *  Written by Steve Chamberlain steve@cygnus.com
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



static void vanilla_before_parse()
{
}


static void 
vanilla_after_parse()
{

}

static void
vanilla_after_allocation()
{

}

static void
vanilla_before_allocation()
{

}


static void
vanilla_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned long  machine = 0;
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, machine);
}

static char *
vanilla_choose_target()
{
  char *from_outside = getenv(TARGET_ENVIRON);
  if (from_outside != (char *)NULL)
    return from_outside;
  return VANILLA_TARGET;
}

static void
vanilla_syslib()
{
  info("%S SYSLIB ignored\n");
}

static void
vanilla_hll(ignore)
char  *ignore;
{
  info("%S HLL ignored\n");
}

			     
static char *vanilla_get_script()
{			     
return "";
}

struct ld_emulation_xfer_struct ld_vanilla_emulation = 
{
  vanilla_before_parse,
  vanilla_syslib,
  vanilla_hll,
  vanilla_after_parse,
  vanilla_after_allocation,
  vanilla_set_output_arch,
  vanilla_choose_target,
  vanilla_before_allocation,
  vanilla_get_script,
};

