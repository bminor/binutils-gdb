# This shell script emits a C file. -*- C -*-
# It does some substitutions.
cat >e${EMULATION_NAME}.c <<EOF
/* For WINDOWS_NT */
/* This file is a copy of ei385coff which was originally generated on a Linux
   system.  It has been modified to provide a decent default script file
   for the NT PE format. */

/* The original file generated returned different default scripts depending
   on whether certain switches were set, but these switches pertain to the
   Linux system and that particular version of coff.  In the NT case, we
   only determine if the subsystem is console or windows in order to select
   the correct entry point by default. */ 
  

/* emulate the original gld for the given armpe
   Copyright (C) 1991, 1993 Free Software Foundation, Inc.
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

#define TARGET_IS_armpe

#include "bfd.h"
#include "sysdep.h"
#include "bfdlink.h"

#include "ld.h"
#include "config.h"
#include "ldmain.h"
#include "ldemul.h"
#include "ldfile.h"
#include "ldmisc.h"

static void gldarmpe_before_parse PARAMS ((void));
static char *gldarmpe_get_script PARAMS ((int *isfile));

static void
gldarmpe_before_parse()
{
#ifndef TARGET_			/* I.e., if not generic.  */
  ldfile_output_architecture = bfd_arch_arm;
#endif /* not TARGET_ */
}

static char *
gldarmpe_get_script(isfile)
     int *isfile;
{			     
  *isfile = 0;

  if (link_info.subsystem == windows)
    return
"OUTPUT_FORMAT(\"coff-arm\")\n\
SEARCH_DIR(/lib); SEARCH_DIR(/usr/lib); SEARCH_DIR(/usr/local/lib); SEARCH_DIR(/usr/local/arm-coff/lib);\n\
ENTRY(_WinMainCRTStartup)\n\
SECTIONS\n\
{\n\
  .text 0x401000 : {\n\
    *(.text)\n\
    ;\n\
  }\n\
  .bss BLOCK(0x1000) :\n\
  { 					\n\
    *(.bss)\n\
    *(COMMON)\n\
    ;\n\
  }\n\
  .rdata BLOCK(0x1000) :\n\
  { 					\n\
    *(.rdata)\n\
    ;\n\
  }\n\
  .data BLOCK(0x1000) : {\n\
    *(.data)\n\
    *(.data2)\n\
    ;\n\
  }\n\
  .idata BLOCK(0x1000) :\n\
  { 					\n\
    *(.idata\$\2)\n\
    *(.idata\$\3)\n\
    *(.idata\$\4)\n\
    *(.idata\$\5)\n\
    *(.idata\$\6)\n\
    *(.idata\$\7)\n\
    ;\n\
  }\n\
  .CRT BLOCK(0x1000) :\n\
  { 					\n\
    *(".CRT\$XCA")\n\
    *(fucl .CRT\$XCC)\n\
    *(.CRT\$XCZ)\n\
    *(.CRT\$XIA)\n\
    *(.CRT\$XIC)\n\
    *(.CRT\$XIZ)\n\
    *(.CRT\$XLA)\n\
    *(.CRT\$XLZ)\n\
    *(.CRT\$XPA)\n\
    *(.CRT\$XPX)\n\
    *(.CRT\$XPZ)\n\
    *(.CRT\$XTA)\n\
    *(.CRT\$XTZ)\n\
    ;\n\
  }\n\
  .rsrc BLOCK(0x1000) :\n\
  { 					\n\
    *(.rsrc\$01)\n\
    *(.rsrc\$02)\n\
    ;\n\
  }\n\
  .reloc BLOCK(0x1000) :\n\
  { 					\n\
    *(.reloc)\n\
    ;\n\
  }\n\
  .junk BLOCK(0x1000) :\n\
  { 					\n\
    *(.debug\$S)\n\
    *(.debug\$T)\n\
    *(.debug\$F)\n\
    *(.drectve)\n\
    ;\n\
  }\n\
}\n\n"
  ; else return
"OUTPUT_FORMAT(\"coff-arm\")\n\
SEARCH_DIR(/lib); SEARCH_DIR(/usr/lib); SEARCH_DIR(/usr/local/lib); SEARCH_DIR(/usr/local/arm-coff/lib);\n\
ENTRY(_mainCRTStartup)\n\
SECTIONS\n\
{\n\
  .text 0x401000 : {\n\
    *(.text)\n\
    ;\n\
  }\n\
  .bss BLOCK(0x1000) :\n\
  { 					\n\
    *(.bss)\n\
    *(COMMON)\n\
    ;\n\
  }\n\
  .rdata BLOCK(0x1000) :\n\
  { 					\n\
    *(.rdata)\n\
    ;\n\
  }\n\
  .data BLOCK(0x1000) : {\n\
    *(.data)\n\
    *(.data2)\n\
    ;\n\
  }\n\
  .idata BLOCK(0x1000) :\n\
  { 					\n\
    *(.idata\$\2)\n\
    *(.idata\$\3)\n\
    *(.idata\$\4)\n\
    *(.idata\$\5)\n\
    *(.idata\$\6)\n\
    *(.idata\$\7)\n\
    ;\n\
  }\n\
  .CRT BLOCK(0x1000) :\n\
  { 					\n\
    *(.CRT\$XCA)\n\
    *(.CRT\$XCC)\n\
    *(.CRT\$XCZ)\n\
    *(.CRT\$XIA)\n\
    *(.CRT\$XIC)\n\
    *(.CRT\$XIZ)\n\
    *(.CRT\$XLA)\n\
    *(.CRT\$XLZ)\n\
    *(.CRT\$XPA)\n\
    *(.CRT\$XPX)\n\
    *(.CRT\$XPZ)\n\
    *(.CRT\$XTA)\n\
    *(.CRT\$XTZ)\n\
    ;\n\
  }\n\
  .rsrc BLOCK(0x1000) :\n\
  { 					\n\
    *(.rsrc\$01)\n\
    *(.rsrc\$02)\n\
    ;\n\
  }\n\
  .reloc BLOCK(0x1000) :\n\
  { 					\n\
    *(.reloc)\n\
    ;\n\
  }\n\
  .junk BLOCK(0x1000) :\n\
  { 					\n\
    *(.debug\$S)\n\
    *(.debug\$T)\n\
    *(.debug\$F)\n\
    *(.drectve)\n\
    ;\n\
  }\n\
}\n\n"
  ; 
}

struct ld_emulation_xfer_struct ld_armpe_emulation = 
{
  gldarmpe_before_parse,
  syslib_default,
  hll_default,
  after_parse_default,
  after_allocation_default,
  set_output_arch_default,
  ldemul_default_target,
  before_allocation_default,
  gldarmpe_get_script,
  "armpe",
  "pei-arm"
};
EOF