

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
   $Id$ 
 *
*/
/*
 * clearing house for ld emulation states 
 */

#include "sysdep.h"
#include "bfd.h"

#include "config.h"
#include "ld.h"
#include "ld-emul.h"
#include "ldmisc.h"

extern ld_emulation_xfer_type ld_lnk960_emulation;
extern ld_emulation_xfer_type ld_gld_emulation;
extern ld_emulation_xfer_type ld_vanilla_emulation;
extern ld_emulation_xfer_type ld_gld68k_emulation;
extern ld_emulation_xfer_type ld_gld960_emulation;



ld_emulation_xfer_type *ld_emulation;

void
ldemul_hll(name)
char *name;
{
  ld_emulation->hll(name);
}


void ldemul_syslib(name)
char *name;
{
  ld_emulation->syslib(name);
}

void
ldemul_after_parse()
{
  ld_emulation->after_parse();
}

void
ldemul_before_parse()
{
  ld_emulation->before_parse();
}

void 
ldemul_after_allocation()
{
  ld_emulation->after_allocation();
}

void 
ldemul_before_allocation()
{
  if (ld_emulation->before_allocation) {
    ld_emulation->before_allocation();
  }
}


void
ldemul_set_output_arch()
{
  ld_emulation->set_output_arch();
}

char *
ldemul_choose_target()
{
  return ld_emulation->choose_target();
}

char *
ldemul_get_script()
{
  return ld_emulation->get_script();
}

void
ldemul_choose_mode(target)
char *target;
{
  if (strcmp(target,LNK960_EMULATION_NAME)==0) {
    ld_emulation = &ld_lnk960_emulation;
  }
  else if (strcmp(target,GLD_EMULATION_NAME)==0) {
    ld_emulation = &ld_gld_emulation;
  }
  else if (strcmp(target,VANILLA_EMULATION_NAME)==0) {
    ld_emulation = &ld_vanilla_emulation;
  }
  else if (strcmp(target,GLD68K_EMULATION_NAME)==0) {
    ld_emulation = &ld_gld68k_emulation;
  }
  else if (strcmp(target,GLD960_EMULATION_NAME)==0) {
    ld_emulation = &ld_gld960_emulation;
  }
  else {
    info("%P%F unrecognised emulation mode: %s\n",target);
  }
}



