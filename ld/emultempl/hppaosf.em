# This shell script emits a C file. -*- C -*-
# It does some substitutions.
cat >em_${EMULATION_NAME}.c <<EOF
/* An emulation for HP PA-RISC OSF/1 linkers.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Steve Chamberlain steve@cygnus.com

This file is part of GLD, the Gnu Linker.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

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

static void hppaosf_before_parse()
{
  ldfile_output_architecture = bfd_arch_hppa;
}

static void
hppaosf_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned long  machine = 0;
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, machine);
}

static char *
hppaosf_get_script(isfile)
     int *isfile;
EOF

if test -n "$COMPILE_IN"
then
# Scripts compiled in.

# sed commands to quote an ld script as a C string.
sc='s/["\\]/\\&/g
s/$/\\n\\/
1s/^/"{/
$s/$/n}"/
'

cat >>em_${EMULATION_NAME}.c <<EOF
{			     
  extern ld_config_type config;

  *isfile = 0;

  if (config.relocateable_output == true && config.build_constructors == true)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xu`;
  else if (config.relocateable_output == true)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xr`;
  else if (!config.text_read_only)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xbn`;
  else if (!config.magic_demand_paged)
    return `sed "$sc" ldscripts/${EMULATION_NAME}.xn`;
  else
    return `sed "$sc" ldscripts/${EMULATION_NAME}.x`;
}
EOF

else
# Scripts read from the filesystem.

cat >>em_${EMULATION_NAME}.c <<EOF
{			     
  extern ld_config_type config;

  *isfile = 1;

  if (config.relocateable_output == true && config.build_constructors == true)
    return "ldscripts/${EMULATION_NAME}.xu";
  else if (config.relocateable_output == true)
    return "ldscripts/${EMULATION_NAME}.xr";
  else if (!config.text_read_only)
    return "ldscripts/${EMULATION_NAME}.xbn";
  else if (!config.magic_demand_paged)
    return "ldscripts/${EMULATION_NAME}.xn";
  else
    return "ldscripts/${EMULATION_NAME}.x";
}
EOF

fi

cat >>em_${EMULATION_NAME}.c <<EOF

struct ld_emulation_xfer_struct ld_hppaosf_emulation = 
{
  hppaosf_before_parse,
  syslib_default,
  hll_default,
  after_parse_default,
  after_allocation_default,
  hppaosf_set_output_arch,
  ldemul_default_target,
  before_allocation_default,
  hppaosf_get_script,
  "hppaosf",
  "elf32-hppa"
};
EOF
