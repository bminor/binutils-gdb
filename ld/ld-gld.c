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
 *  $Log$
 *  Revision 1.1.1.1  1991/03/21 21:28:25  gumby
 *  Back from Intel with Steve
 *
 * Revision 1.1  1991/03/21  21:28:24  gumby
 * Initial revision
 *
 * Revision 1.2  1991/03/15  18:45:55  rich
 * foo
 *
 * Revision 1.1  1991/03/13  00:48:11  chrisb
 * Initial revision
 *
 * Revision 1.7  1991/03/10  09:31:18  rich
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
 * Revision 1.6  1991/03/09  03:23:12  sac
 * Added -Ur loader script.
 *
 * Revision 1.5  1991/03/06  21:59:29  sac
 * Completed G++ support
 *
 * Revision 1.4  1991/03/06  02:23:34  sac
 * Added support for partial linking.
 *
 * Revision 1.3  1991/02/22  17:14:56  sac
 * Added RCS keywords and copyrights
 *
*/

/* 
 * emulate the original gld
 *
 *  Written by Steve Chamberlain steve@cygnus.com
 */


#include "sysdep.h"
#include "bfd.h"


#include "ld.h"
#include "config.h"
#include "ld-emul.h"
#include "ldfile.h"
#include "ldmisc.h"

extern  boolean lang_float_flag;


extern enum bfd_architecture ldfile_output_architecture;
extern unsigned long ldfile_output_machine;
extern char *ldfile_output_machine_name;

extern bfd *output_bfd;



static void gld_before_parse()
{
  ldfile_add_library_path("/lib");
  ldfile_add_library_path("/usr/lib");
  ldfile_add_library_path("/usr/local/lib/lib");
  ldfile_output_architecture = bfd_arch_sparc;
}


static void 
gld_after_parse()
{

}

static void
gld_after_allocation()
{

}

static void
gld_before_allocation()
{

}


static void
gld_set_output_arch()
{
  /* Set the output architecture and machine if possible */
  unsigned long  machine = 0;
  bfd_set_arch_mach(output_bfd, ldfile_output_architecture, machine);
}

static char *
gld_choose_target()
{
  char *from_outside = getenv(TARGET_ENVIRON);
  if (from_outside != (char *)NULL)
    return from_outside;
  return GLD_TARGET;
}

static void
gld_syslib()
{
  info("%S SYSLIB ignored\n");
}

static void
gld_hll(ignore)
char  *ignore;
{
  info("%S HLL ignored\n");
}

static char *gld_script = "  \
SEARCH_DIR(/lib) 			\
SEARCH_DIR(/usr/lib) 			\
SEARCH_DIR(/usr/local/lib) 		\
__DYNAMIC = 0; 				\
SECTIONS 				\
{ 					\
  .text 0x2020 BLOCK(0x2000): 		\
  { 					\
   CREATE_OBJECT_SYMBOLS 		\
    *(.text) 				\
    _etext = ALIGN( 0x2000);       	\
    }  					\
  .data  ALIGN(0x2000)  :		\
  { 					\
    *(.data) 				\
    ___DTOR_LIST__=. ;               	\
    LONG((___CTOR_LIST__ - .)/4 -2)  	\
    *(___DTOR_LIST__)                   \
    LONG(0)                             \
    ___CTOR_LIST__=. ;               	\
    LONG((_edata  - .)/4 -2)            \
    *(___CTOR_LIST__)                   \
    LONG(0)                             \
      _edata = .; 			\
  }  					\
  .bss   SIZEOF(.data) + ADDR(.data) :	\
  { 					\
   *(.bss)	 			\
   [COMMON] 				\
     _end=.;				\
    } 					\
}";


static char *gld_script_option_Ur = "\
SEARCH_DIR(/lib) 			\
SEARCH_DIR(/usr/lib) 			\
SEARCH_DIR(/usr/local/lib) 		\
SECTIONS 				\
{ 					\
  .text 0: 				\
  { 					\
   CREATE_OBJECT_SYMBOLS                \
    *(.text) 				\
    } 					\
  .data SIZEOF(.text) + ADDR(.text) :	\
  { 					\
    *(.data) 				\
    ___DTOR_LIST__=. ;               	\
    LONG((___CTOR_LIST__ - .)/4 -2)  	\
    *(___DTOR_LIST__)                   \
    LONG(0)                             \
    ___CTOR_LIST__=. ;               	\
    LONG((___end_list__  - .)/4 -2)     \
    *(___CTOR_LIST__)                   \
    LONG(0)                             \
    ___end_list__ = . ;			\
    } 					\
  .bss SIZEOF(.data) + ADDR(.data) :	\
  {					\
    *(.bss)				\
   [COMMON]				\
    } 					\
} 					\
";			     

static char *gld_script_option_r = "\
SEARCH_DIR(/lib) 			\
SEARCH_DIR(/usr/lib) 			\
SEARCH_DIR(/usr/local/lib) 		\
SECTIONS 				\
{ 					\
  .text 0: 				\
  { 					\
   CREATE_OBJECT_SYMBOLS                \
    *(.text) 				\
    } 					\
  .data SIZEOF(.text) + ADDR(.text) :	\
  { 					\
    *(.data) 				\
    } 					\
  .bss SIZEOF(.data) + ADDR(.data) :	\
  {					\
    *(.bss)				\
   [COMMON]				\
    } 					\
} 					\
";			     
			     
static char *gld_get_script()
{			     
  extern ld_config_type config;
  if (config.relocateable_output == true &&
      config.build_constructors == true) {
    return gld_script_option_Ur;
  }
  if (config.relocateable_output) {
    return gld_script_option_r;
  }
	
  return gld_script;
}
struct ld_emulation_xfer_struct ld_gld_emulation = 
{
  gld_before_parse,
  gld_syslib,
  gld_hll,
  gld_after_parse,
  gld_after_allocation,
  gld_set_output_arch,
  gld_choose_target,
  gld_before_allocation,
  gld_get_script,
};

