/* BFD backend for generic a.out flavour 1 */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <ansidecl.h>
#include <sysdep.h>
struct external_exec;
#include <a.out.sun4.h>
#include "bfd.h"
#include "libaout.h"           
#include "libbfd.h"



#include "aout64.h"
#include "stab.gnu.h"
#include "ar.h"



void (*bfd_error_trap)();

static bfd_target *sunos4_callback ();

/*SUPPRESS558*/
/*SUPPRESS529*/

bfd_target *
DEFUN(NAME(sunos,object_p), (abfd),
     bfd *abfd)
{
  unsigned char magicbuf[4];	/* Raw bytes of magic number from file */
  unsigned long magic;		/* Swapped magic number */

  bfd_error = system_call_error;

  if (bfd_read ((PTR)magicbuf, 1 , 4, abfd) !=
      sizeof (magicbuf))
    return 0;
  magic = bfd_h_get_32 (abfd, magicbuf);

  if (N_BADMAG (*((struct internal_exec *) &magic))) return 0;

  return NAME(aout,some_aout_object_p) (abfd, sunos4_callback);
}

  /* Determine the size of a relocation entry, based on the architecture */
static void
DEFUN(choose_reloc_size,(abfd),
bfd *abfd)
  {
    switch (abfd->obj_arch) {
    case bfd_arch_sparc:
    case bfd_arch_a29k:
      obj_reloc_entry_size (abfd) = RELOC_EXT_SIZE;
      break;
    default:
      obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;
      break;
    }
  }

/* Set parameters about this a.out file that are machine-dependent.
   This routine is called from some_aout_object_p just before it returns.  */

static bfd_target *
sunos4_callback (abfd)
     bfd *abfd;
{
  struct internal_exec *execp = exec_hdr (abfd);

  WORK_OUT_FILE_POSITIONS(abfd, execp);  

  /* Determine the architecture and machine type of the object file.  */
  switch (N_MACHTYPE (*exec_hdr (abfd))) {
    
  case M_UNKNOWN:
    abfd->obj_arch = bfd_arch_unknown;
    abfd->obj_machine = 0;
    break;
    
  case M_68010:
    abfd->obj_arch = bfd_arch_m68k;
    abfd->obj_machine = 68010;
    break;
    
  case M_68020:
    abfd->obj_arch = bfd_arch_m68k;
    abfd->obj_machine = 68020;
    break;
    
  case M_SPARC:
    abfd->obj_arch = bfd_arch_sparc;
    abfd->obj_machine = 0;
    break;
    
  case M_386:
    abfd->obj_arch = bfd_arch_i386;
    abfd->obj_machine = 0;
    break;
    
  case M_29K:
    abfd->obj_arch = bfd_arch_a29k;
    abfd->obj_machine = 0;
    break;
    
  default:
    abfd->obj_arch = bfd_arch_obscure;
    abfd->obj_machine = 0;
    break;
  }
  
  choose_reloc_size(abfd);
  return abfd->xvec;
}


/* Write an object file in SunOS format.
Section contents have already been written.  We write the
file header, symbols, and relocation.  */

boolean
DEFUN(NAME(aout,sunos4_write_object_contents),(abfd),
      bfd *abfd)

  {
    bfd_size_type data_pad = 0;
    struct external_exec exec_bytes;
    struct internal_exec *execp = exec_hdr (abfd);
    
    
    
    execp->a_text = obj_textsec (abfd)->size;
    
    /* Magic number, maestro, please!  */
    switch (bfd_get_architecture(abfd)) {
    case bfd_arch_m68k:
      switch (bfd_get_machine(abfd)) {
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

    /* FIXME */
    N_SET_FLAGS (*execp, 0x81);
    
    WRITE_HEADERS(abfd, execp);

  return true;
}

/* core files */

#define CORE_MAGIC 0x080456
#define CORE_NAMELEN 16

/* The core structure is taken from the Sun documentation.
Unfortunately, they don't document the FPA structure, or at least I
can't find it easily.  Fortunately the core header contains its own
length.  So this shouldn't cause problems, except for c_ucode, which
so far we don't use but is easy to find with a little arithmetic. */

/* But the reg structure can be gotten from the SPARC processor handbook.
This really should be in a GNU include file though so that gdb can use
the same info. */
struct regs {
  int r_psr;
  int r_pc;
  int r_npc;
  int r_y;
  int r_g1;
  int r_g2;
  int r_g3;
  int r_g4;
  int r_g5;
  int r_g6;
  int r_g7;
  int r_o0;
  int r_o1;
  int r_o2;
  int r_o3;
  int r_o4;
  int r_o5;
  int r_o6;
  int r_o7;
};

/* Taken from Sun documentation: */

/* FIXME:  It's worse than we expect.  This struct contains TWO substructs
neither of whose size we know, WITH STUFF IN BETWEEN THEM!  We can't
even portably access the stuff in between!  */

struct core {
  int c_magic;			/* Corefile magic number */
  int c_len;			/* Sizeof (struct core) */
  struct regs c_regs;		/* General purpose registers -- MACHDEP SIZE */
  struct internal_exec c_aouthdr;	/* A.out header */
  int c_signo;			/* Killing signal, if any */
  int c_tsize;			/* Text size (bytes) */
  int c_dsize;			/* Data size (bytes) */
  int c_ssize;			/* Stack size (bytes) */
  char c_cmdname[CORE_NAMELEN + 1]; /* Command name */
  double fp_stuff[1];		    /* external FPU state (size unknown by us) */
  /* The type "double" is critical here, for alignment.
    SunOS declares a struct here, but the struct's alignment
      is double since it contains doubles.  */
  int c_ucode;			/* Exception no. from u_code */
  /* (this member is not accessible by name since we don't
    portably know the size of fp_stuff.) */
};

/* Supposedly the user stack grows downward from the bottom of kernel memory.
Presuming that this remains true, this definition will work. */
#define USRSTACK (-(128*1024*1024))

PROTO (static void, swapcore, (bfd *abfd, struct core *core));

/* need this cast b/c ptr is really void * */
#define core_hdr(bfd) (((struct suncordata *) (bfd->tdata))->hdr)
#define core_datasec(bfd) (((struct suncordata *) ((bfd)->tdata))->data_section)
#define core_stacksec(bfd) (((struct suncordata*)((bfd)->tdata))->stack_section)
#define core_regsec(bfd) (((struct suncordata *) ((bfd)->tdata))->reg_section)
#define core_reg2sec(bfd) (((struct suncordata *) ((bfd)->tdata))->reg2_section)

/* These are stored in the bfd's tdata */
struct suncordata {
struct core *hdr;             /* core file header */
asection *data_section;
asection *stack_section;
asection *reg_section;
asection *reg2_section;
};

static bfd_target *
DEFUN(sunos4_core_file_p,(abfd),
      bfd *abfd)
{
  unsigned char longbuf[4];	/* Raw bytes of various header fields */
  int core_size;
  int core_mag;
  struct core *core;
  char *rawptr;
  
  bfd_error = system_call_error;
  
  if (bfd_read ((PTR)longbuf, 1, sizeof (longbuf), abfd) !=
      sizeof (longbuf))
    return 0;
  core_mag = bfd_h_get_32 (abfd, longbuf);

  if (core_mag != CORE_MAGIC) return 0;

  /* SunOS core headers can vary in length; second word is size; */
  if (bfd_read ((PTR)longbuf, 1, sizeof (longbuf), abfd) !=
      sizeof (longbuf))
    return 0;
  core_size = bfd_h_get_32 (abfd, longbuf);
  /* Sanity check */
  if (core_size > 20000)
    return 0;

  if (bfd_seek (abfd, 0L, false) < 0) return 0;

  rawptr = bfd_zalloc (abfd, core_size + sizeof (struct suncordata));
  if (rawptr == NULL) {
    bfd_error = no_memory;
    return 0;
  }

  core = (struct core *) (rawptr + sizeof (struct suncordata));

  if ((bfd_read ((PTR) core, 1, core_size, abfd)) != core_size) {
    bfd_error = system_call_error;
    bfd_release (abfd, rawptr);
    return 0;
  }

  swapcore (abfd, core);
  set_tdata (abfd, ((struct suncordata *) rawptr));
  core_hdr (abfd) = core;

  /* create the sections.  This is raunchy, but bfd_close wants to reclaim
     them */
  core_stacksec (abfd) = (asection *) bfd_zalloc (abfd, sizeof (asection));
  if (core_stacksec (abfd) == NULL) {
  loser:
    bfd_error = no_memory;
    bfd_release (abfd, rawptr);
    return 0;
  }
  core_datasec (abfd) = (asection *) bfd_zalloc (abfd, sizeof (asection));
  if (core_datasec (abfd) == NULL) {
  loser1:
    bfd_release (abfd, core_stacksec (abfd));
    goto loser;
  }
  core_regsec (abfd) = (asection *) bfd_zalloc (abfd, sizeof (asection));
  if (core_regsec (abfd) == NULL) {
  loser2:
    bfd_release (abfd, core_datasec (abfd));
    goto loser1;
  }
  core_reg2sec (abfd) = (asection *) bfd_zalloc (abfd, sizeof (asection));
  if (core_reg2sec (abfd) == NULL) {
    bfd_release (abfd, core_regsec (abfd));
    goto loser2;
  }

  core_stacksec (abfd)->name = ".stack";
  core_datasec (abfd)->name = ".data";
  core_regsec (abfd)->name = ".reg";
  core_reg2sec (abfd)->name = ".reg2";

  core_stacksec (abfd)->flags = SEC_ALLOC + SEC_LOAD;
  core_datasec (abfd)->flags = SEC_ALLOC + SEC_LOAD;
  core_regsec (abfd)->flags = SEC_ALLOC;
  core_reg2sec (abfd)->flags = SEC_ALLOC;

  core_stacksec (abfd)->size = core->c_ssize;
  core_datasec (abfd)->size = core->c_dsize;
  core_regsec (abfd)->size = (sizeof core->c_regs);
  /* Float regs take up end of struct, except c_ucode.  */
  core_reg2sec (abfd)->size = core_size - (sizeof core->c_ucode) -
    (file_ptr)(((struct core *)0)->fp_stuff);

  core_stacksec (abfd)->vma = (USRSTACK - core->c_ssize);
  core_datasec (abfd)->vma = N_DATADDR(core->c_aouthdr);
  core_regsec (abfd)->vma = -1;
  core_reg2sec (abfd)->vma = -1;

  core_stacksec (abfd)->filepos = core->c_len + core->c_dsize;
  core_datasec (abfd)->filepos = core->c_len;
  /* In file header: */
  core_regsec (abfd)->filepos = (file_ptr)(&((struct core *)0)->c_regs);
  core_reg2sec (abfd)->filepos = (file_ptr)(((struct core *)0)->fp_stuff);

  /* Align to word at least */
  core_stacksec (abfd)->alignment_power = 2;
  core_datasec (abfd)->alignment_power = 2;
  core_regsec (abfd)->alignment_power = 2;
  core_reg2sec (abfd)->alignment_power = 2;

  abfd->sections = core_stacksec (abfd);
  core_stacksec (abfd)->next = core_datasec (abfd);
  core_datasec (abfd)->next = core_regsec (abfd);
  core_regsec (abfd)->next = core_reg2sec (abfd);

  abfd->section_count = 4;

  return abfd->xvec;
}

static char *sunos4_core_file_failing_command (abfd)
bfd *abfd;
  {
  return core_hdr (abfd)->c_cmdname;
}

static int
DEFUN(sunos4_core_file_failing_signal,(abfd),
      bfd *abfd)
{
  return core_hdr (abfd)->c_signo;
}

static boolean
DEFUN(sunos4_core_file_matches_executable_p, (core_bfd, exec_bfd),
      bfd *core_bfd AND
      bfd *exec_bfd)
{
  if (core_bfd->xvec != exec_bfd->xvec) {
    bfd_error = system_call_error;
    return false;
  }

  return (bcmp ((char *)&core_hdr (core_bfd), (char*) &exec_hdr (exec_bfd),
		sizeof (struct internal_exec)) == 0) ? true : false;
}

/* byte-swap core structure */
/* FIXME, this needs more work to swap IN a core struct from raw bytes */
static void
DEFUN(swapcore,(abfd, core),
      bfd *abfd AND
      struct core *core)
{
  struct external_exec exec_bytes;
  
  core->c_magic = bfd_h_get_32 (abfd, (unsigned char *)&core->c_magic);
  core->c_len   = bfd_h_get_32 (abfd, (unsigned char *)&core->c_len  );
  /* Leave integer registers in target byte order.  */
  bcopy ((char *)&(core->c_aouthdr), (char *)&exec_bytes, EXEC_BYTES_SIZE);
  NAME(aout,swap_exec_header_in)(abfd, &exec_bytes, &core->c_aouthdr);
  core->c_signo = bfd_h_get_32 (abfd, (unsigned char *)&core->c_signo);
  core->c_tsize = bfd_h_get_32 (abfd, (unsigned char *)&core->c_tsize);
  core->c_dsize = bfd_h_get_32 (abfd, (unsigned char *)&core->c_dsize);
  core->c_ssize = bfd_h_get_32 (abfd, (unsigned char *)&core->c_ssize);
  /* Leave FP registers in target byte order.  */
  /* Leave "c_ucode" unswapped for now, since we can't find it easily.  */
}

/* We use BFD generic archive files.  */
#define	aout_32_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout_32_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	aout_32_slurp_armap			bfd_slurp_bsd_armap
#define	aout_32_slurp_extended_name_table	bfd_true
#define	aout_32_write_armap			bsd_write_armap
#define	aout_32_truncate_arname			bfd_bsd_truncate_arname
#define aout_32_machine_type 			sunos_machine_type

#define	aout_32_core_file_failing_command 	sunos4_core_file_failing_command
#define	aout_32_core_file_failing_signal	sunos4_core_file_failing_signal
#define	aout_32_core_file_matches_executable_p	sunos4_core_file_matches_executable_p


#define	aout_64_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	aout_64_generic_stat_arch_elt		bfd_generic_stat_arch_elt
#define	aout_64_slurp_armap			bfd_slurp_bsd_armap
#define	aout_64_slurp_extended_name_table	bfd_true
#define	aout_64_write_armap			bsd_write_armap
#define	aout_64_truncate_arname			bfd_bsd_truncate_arname
#define aout_64_machine_type 			sunos_machine_type

#define	aout_64_core_file_failing_command 	sunos4_core_file_failing_command
#define	aout_64_core_file_failing_signal	sunos4_core_file_failing_signal
#define	aout_64_core_file_matches_executable_p	sunos4_core_file_matches_executable_p

/* We implement these routines ourselves, rather than using the generic
a.out versions.  */
#define	aout_write_object_contents	sunos4_write_object_contents

bfd_target VECNAME =
  {
TARGETNAME,
  bfd_target_aout_flavour_enum,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
   (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
   ' ',							  /* ar_pad_char */
   16,							  /* ar_max_namelen */
   _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16, /* data */
   _do_getb64, _do_putb64, _do_getb32, _do_putb32, _do_getb16, _do_putb16,  /* hdrs */
   
  {_bfd_dummy_target, NAME(sunos,object_p),
   bfd_generic_archive_p, sunos4_core_file_p},
  {bfd_false, NAME(aout,mkobject),
   _bfd_generic_mkarchive, bfd_false},
  {bfd_false, NAME(aout,sunos4_write_object_contents), /* bfd_write_contents */
   _bfd_write_archive_contents, bfd_false},
   
   JUMP_TABLE(JNAME(aout))
};
