/* BFD support for Sparc binaries under LynxOS.
   Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Descriptor library.

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

#define ARCH 32

#if 0
#define BYTES_IN_WORD 4
#define N_SHARED_LIB(x) 0

#define TEXT_START_ADDR 0
#define PAGE_SIZE 4096
#define SEGMENT_SIZE PAGE_SIZE
#define DEFAULT_ARCH bfd_arch_sparc

#endif

#define MY(OP) CAT(sparclynx_aout_,OP)
#define TARGETNAME "a.out-sparc-lynx"

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#include "aout/sun4.h"
#include "libaout.h"           /* BFD a.out internal data structures */

#include "aout/aout64.h"
#include "aout/stab_gnu.h"
#include "aout/ar.h"

/* This is needed to reject a NewsOS file, e.g. in
   gdb/testsuite/gdb.t10/crossload.exp. <kingdon@cygnus.com>
   I needed to add M_UNKNOWN to recognize a 68000 object, so this will
   probably no longer reject a NewsOS object.  <ian@cygnus.com>. */
#define MACHTYPE_OK(mtype) ((mtype) == M_UNKNOWN \
			    || (mtype) == M_68010 \
			    || (mtype) == M_68020 \
			    || (mtype) == M_SPARC)

/*
The file @code{aoutf1.h} contains the code for BFD's
a.out back end. Control over the generated back end is given by these
two preprocessor names:
@table @code
@item ARCH_SIZE
This value should be either 32 or 64, depending upon the size of an
int in the target format. It changes the sizes of the structs which
perform the memory/disk mapping of structures.

The 64 bit backend may only be used if the host compiler supports 64
ints (eg long long with gcc), by defining the name @code{HOST_64_BIT} in @code{bfd.h}.
With this name defined, @emph{all} bfd operations are performed with 64bit
arithmetic, not just those to a 64bit target.

@item TARGETNAME
The name put into the target vector.
@item
@end table

*/

/*SUPPRESS558*/
/*SUPPRESS529*/

void
DEFUN(NAME(lynxos,set_arch_mach), (abfd, machtype),
      bfd *abfd AND int machtype)
{
  /* Determine the architecture and machine type of the object file.  */
  enum bfd_architecture arch;
  long machine;
  switch (machtype) {

  case M_UNKNOWN:
      /* Some Sun3s make magic numbers without cpu types in them, so
	 we'll default to the 68000. */
    arch = bfd_arch_m68k;
    machine = 68000;
    break;
    
  case M_68010:
  case M_HP200:
    arch = bfd_arch_m68k;
    machine = 68010;
    break;
    
  case M_68020:
  case M_HP300:
    arch = bfd_arch_m68k;
    machine = 68020;
    break;
    
  case M_SPARC:
    arch = bfd_arch_sparc;
    machine = 0;
    break;
    
  case M_386:
  case M_386_DYNIX:
    arch = bfd_arch_i386;
    machine = 0;
    break;
    
  case M_29K:
    arch = bfd_arch_a29k;
    machine = 0;
    break;
    
  case M_HPUX:
    arch = bfd_arch_m68k;
    machine = 0;
    break;

  default:
    arch = bfd_arch_obscure;
    machine = 0;
    break;
  }
  bfd_set_arch_mach(abfd, arch, machine);  
}

#define SET_ARCH_MACH(ABFD, EXEC) \
  NAME(lynxos,set_arch_mach)(ABFD, N_MACHTYPE (EXEC)); \
  choose_reloc_size(ABFD);

/* Determine the size of a relocation entry, based on the architecture */
static void
DEFUN(choose_reloc_size,(abfd),
bfd *abfd)
{
  switch (bfd_get_arch(abfd)) {
  case bfd_arch_sparc:
  case bfd_arch_a29k:
    obj_reloc_entry_size (abfd) = RELOC_EXT_SIZE;
    break;
  default:
    obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
    break;
  }
}

/* Write an object file in Lynxos format.
  Section contents have already been written.  We write the
  file header, symbols, and relocation.  */

static boolean
DEFUN(NAME(aout,sparclynx_write_object_contents),
      (abfd),
      bfd *abfd)
{
  struct external_exec exec_bytes;
  struct internal_exec *execp = exec_hdr (abfd);
    
  /* Magic number, maestro, please!  */
  switch (bfd_get_arch(abfd)) {
  case bfd_arch_m68k:
    switch (bfd_get_mach(abfd)) {
    case 68010:
      N_SET_MACHTYPE(*execp, M_68010);
      break;
    default:
    case 68020:
      N_SET_MACHTYPE(*execp, M_68020);
      break;
    }
    break;
  case bfd_arch_sparc:
    N_SET_MACHTYPE(*execp, M_SPARC);
    break;
  case bfd_arch_i386:
    N_SET_MACHTYPE(*execp, M_386);
    break;
  case bfd_arch_a29k:
    N_SET_MACHTYPE(*execp, M_29K);
    break;
  default:
    N_SET_MACHTYPE(*execp, M_UNKNOWN);
  }
    
  choose_reloc_size(abfd);

#if 0
  /* Some tools want this to be 0, some tools want this to be one.
     Today, it seems that 0 is the most important setting (PR1927) */
  N_SET_FLAGS (*execp, 0x0);
#else

  /* Fri Jun 11 14:23:31 PDT 1993
     FIXME 
     Today's optimal setting is 1.  This is a pain, since it
     reopens 1927.  This should be readdressed by creating a new
     target for each each supported, giving perhaps sun3/m68k
     and sun4/sparc a.out formats.
     */
  N_SET_FLAGS (*execp, 1);
#endif
    
  WRITE_HEADERS(abfd, execp);

  return true;
}

#define MY_set_sizes sparclynx_set_sizes
static boolean
DEFUN (sparclynx_set_sizes, (abfd),
       bfd *abfd)
{
  switch (bfd_get_arch (abfd))
    {
    default:
      return false;
    case bfd_arch_sparc:
      adata(abfd).page_size = 0x2000;
      adata(abfd).segment_size = 0x2000;
      adata(abfd).exec_bytes_size = EXEC_BYTES_SIZE;
      return true;
    case bfd_arch_m68k:
      adata(abfd).page_size = 0x2000;
      adata(abfd).segment_size = 0x20000;
      adata(abfd).exec_bytes_size = EXEC_BYTES_SIZE;
      return true;
    }
}

static CONST struct aout_backend_data sparclynx_aout_backend = {
  0, 1, 0, sparclynx_set_sizes, 0,
};


#define MY_bfd_debug_info_start		bfd_void
#define MY_bfd_debug_info_end		bfd_void
#define MY_bfd_debug_info_accumulate	\
			(void (*) PARAMS ((bfd *, struct sec *))) bfd_void

#define MY_write_object_contents	NAME(aout,sparclynx_write_object_contents)
#define MY_backend_data			&sparclynx_aout_backend

#define TARGET_IS_BIG_ENDIAN_P

#ifdef HOST_LYNX

char *lynx_core_file_failing_command();
int lynx_core_file_failing_signal();
boolean lynx_core_file_matches_executable_p();
bfd_target *lynx_core_file_p();

#define	MY_core_file_failing_command lynx_core_file_failing_command
#define	MY_core_file_failing_signal lynx_core_file_failing_signal
#define	MY_core_file_matches_executable_p lynx_core_file_matches_executable_p
#define	MY_core_file_p lynx_core_file_p

#endif /* HOST_LYNX */

#include "aout-target.h"
