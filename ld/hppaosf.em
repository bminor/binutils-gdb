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


#ifdef HPPAOSF

static void hppaosf_before_parse()
{
  static char *env_variables[] = { "HPPALIB", "HPPABASE", 0 };
  char **p;
  char *env ;

  for ( p = env_variables; *p; p++ ){
    env =  (char *) getenv(*p);
    if (env) {
      ldfile_add_library_path(concat(env,"/lib/libbout",""));
    }
  }
  ldfile_output_architecture = bfd_arch_hppa;
}
#else
static void hppaosf_before_parse()
{
  char *env ;
  env =  getenv("HPPALIB");
  if (env) {
    ldfile_add_library_path(env);
  }
  env = getenv("HPPABASE");
  if (env) {
    ldfile_add_library_path(concat(env,"/lib",""));
  }
  ldfile_output_architecture = bfd_arch_hppa;
}
#endif /* HPPAOSF */

static void
hppaosf_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned long  machine = 0;
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, machine);
}

static char *script = 
#include "hppaosf.x"
;

  
static char *script_reloc =
#include "hppaosf.xr"
 ;


static char *hppaosf_get_script()
{
   extern ld_config_type config;
   if (config.relocateable_output)
     return script_reloc;
   return script;

}

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
  "elf-big"
};
EOF
