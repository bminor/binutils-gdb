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

   $Log$
   Revision 1.1.1.1  1991/03/21 21:28:29  gumby
   Back from Intel with Steve

 * Revision 1.1  1991/03/21  21:28:28  gumby
 * Initial revision
 *
 * Revision 1.2  1991/03/15  18:45:55  rich
 * foo
 *
 * Revision 1.1  1991/03/13  00:48:13  chrisb
 * Initial revision
 *
 * Revision 1.6  1991/03/10  09:31:20  rich
 *  Modified Files:
 *  	Makefile config.h ld-emul.c ld-emul.h ld-gld.c ld-gld960.c
 *  	ld-lnk960.c ld.h lddigest.c ldexp.c ldexp.h ldfile.c ldfile.h
 *  	ldgram.y ldinfo.h ldlang.c ldlang.h ldlex.h ldlex.l ldmain.c
 *  	ldmain.h ldmisc.c ldmisc.h ldsym.c ldsym.h ldversion.c
 *  	ldversion.h ldwarn.h ldwrite.c ldwrite.h y.tab.h
 *
 * As of this round of changes, ld now builds on all hosts of (Intel960)
 * interest and copy passes my copy test on big endian hosts again.
 *
 * Revision 1.5  1991/03/09  03:23:47  sac
 * Now looks in G960BASE if I960BASE isn't defined.
 *
 * Revision 1.4  1991/03/06  02:23:35  sac
 * Added support for partial linking.
 *
 * Revision 1.3  1991/02/22  17:14:58  sac
 * Added RCS keywords and copyrights
 *
*/

/* 

   Written by Steve Chamberlain steve@cygnus.com

 * intel coff loader emulation specific stuff
 */

#include "sysdep.h"
#include "bfd.h"

/*#include "archures.h"*/
#include "ld.h"
#include "config.h"
#include "ld-emul.h"
#include "ldmisc.h"
#include "ldlang.h"
#include "ldfile.h"

extern  boolean lang_float_flag;
extern bfd *output_bfd;



extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;
extern char *ldfile_output_machine_name;


typedef struct lib_list {
  char *name;
  struct lib_list *next;
} lib_list_type;

static lib_list_type *hll_list;
static lib_list_type **hll_list_tail = &hll_list;

static lib_list_type *syslib_list;
static lib_list_type **syslib_list_tail = &syslib_list;


static void
append(list, name)
lib_list_type ***list;
char *name;
{
  lib_list_type *element = 
    (lib_list_type *)(ldmalloc(sizeof(lib_list_type)));

  element->name = name;
  element->next = (lib_list_type *)NULL;
  **list = element;
  *list = &element->next;

}

static boolean had_hll = false;
static boolean had_hll_name = false;
static void
lnk960_hll(name)
char *name;
{
  had_hll = true;
  if (name != (char *)NULL) {
    had_hll_name = true;
    append(&hll_list_tail, name);
  }
}

static void 
lnk960_syslib(name)
char *name;
{
  append(&syslib_list_tail,name);
}



static void 
lnk960_before_parse()
{
  char *name = getenv("I960BASE");

  if (name == (char *)NULL) {
    name = getenv("G960BASE");
    if (name == (char *)NULL) {
      info("%P%F I960BASE and G960BASE not set\n");
    }
  }


  ldfile_add_library_path(concat(name,"/lib",""));
  ldfile_output_architecture = bfd_arch_i960;
  ldfile_output_machine = bfd_mach_i960_core;
}

static void
add_on(list, search)
lib_list_type *list;
lang_input_file_enum_type search;
{
  while (list) {
    lang_add_input_file(list->name,
			search,
			(char *)NULL);
    list = list->next;
  }
}
static void lnk960_after_parse()
{

  /* If there has been no arch, default to -KB */
  if (ldfile_output_machine_name[0] ==0) {
    ldfile_add_arch("kb");
  }

  /* if there has been no hll list then add our own */

  if(had_hll && !had_hll_name) {
    append(&hll_list_tail,"c");
    if (lang_float_flag == true) {
      append(&hll_list_tail,"m");
    }
    else {
      append(&hll_list_tail,"mstub");
    }
    if (ldfile_output_machine == bfd_mach_i960_ka_sa ||
	ldfile_output_machine == bfd_mach_i960_ca) {
      {
	append(&hll_list_tail,"f");
      }
    }
  }



  add_on(hll_list, lang_input_file_is_l_enum);
  add_on(syslib_list, lang_input_file_is_search_file_enum);


}

static void
lnk960_before_allocation()
{
}
static void
lnk960_after_allocation()
{
  lang_abs_symbol_at_end_of(".text","_etext");
  lang_abs_symbol_at_end_of(".data","_edata");
  lang_abs_symbol_at_end_of(".bss","_end");
}

static struct
 {
   unsigned  long number;
   char *name; 
 }
machine_table[] = {
	bfd_mach_i960_core	,"CORE",
	bfd_mach_i960_kb_sb	,"KB",
	bfd_mach_i960_kb_sb	,"SB",
	bfd_mach_i960_mc	,"MC",
	bfd_mach_i960_xa	,"XA",
	bfd_mach_i960_ca	,"CA",
	bfd_mach_i960_ka_sa	,"KA",
	bfd_mach_i960_ka_sa	,"SA",

	bfd_mach_i960_core	,"core",
	bfd_mach_i960_kb_sb	,"kb",
	bfd_mach_i960_kb_sb	,"sb",
	bfd_mach_i960_mc	,"mc",
	bfd_mach_i960_xa	,"xa",
	bfd_mach_i960_ca	,"ca",
	bfd_mach_i960_ka_sa	,"ka",
	bfd_mach_i960_ka_sa	,"sa",
	0,(char *)NULL
};

static void
lnk960_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned int i;
  ldfile_output_machine = bfd_mach_i960_core;
  for (i= 0; machine_table[i].name != (char*)NULL; i++) {
    if (strcmp(ldfile_output_machine_name,machine_table[i].name)==0) {
      ldfile_output_machine = machine_table[i].number;
      break;
    }
  }
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, ldfile_output_machine);
}

static char *
lnk960_choose_target()
{
  char *from_outside = getenv(TARGET_ENVIRON);
  if (from_outside != (char *)NULL)
    return from_outside;
  return LNK960_TARGET;
}

/* The default script if none is offered */
static char *lnk960_script = "\
SECTIONS \
{ \
  .text : \
  { \
    *(.text) \
    }  \
_etext = .;\
  .data  SIZEOF(.text) + ADDR(.text):\
  { \
    *(.data) \
    }  \
_edata = .; \
  .bss   SIZEOF(.data) + ADDR(.data) :    \
  { \
   _bss_start = . ;\
   *(.bss)  \
   [COMMON] \
    } \
_end = . ; \
} \
";

static char *lnk960_script_relocateable = "\
SECTIONS \
{ \
  .text 0x40000000: \
  { \
    *(.text) \
    }  \
  .data  0:\
  { \
    *(.data) \
    }  \
  .bss   SIZEOF(.data) + ADDR(.data) :    \
  { \
   *(.bss)  \
   [COMMON] \
    } \
} \
";

static char *lnk960_get_script()
{
extern ld_config_type config;
if (config.relocateable_output) {
  return lnk960_script_relocateable;
}
return lnk960_script;


}
struct ld_emulation_xfer_struct ld_lnk960_emulation = 
{
  lnk960_before_parse,
  lnk960_syslib,
  lnk960_hll,
  lnk960_after_parse,
  lnk960_after_allocation,
  lnk960_set_output_arch,
  lnk960_choose_target,
  lnk960_before_allocation,
  lnk960_get_script,
};
