/*** bfd backend for sunos binaries */

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

/* $Id$
 * $Log$
 * Revision 1.1.1.1  1991/03/21 21:11:23  gumby
 * Back from Intel with Steve
 *
 * Revision 1.1  1991/03/21  21:11:23  gumby
 * Initial revision
 *
 * Revision 1.2  1991/03/15  18:16:52  rich
 * *** empty log message ***
 *
 * Revision 1.12  1991/03/10  19:11:41  rich
 *  Modified Files:
 *  	bfd.c coff-code.h libbfd.c libbfd.h srec.c sunos.c
 *
 * Working bugs out of coff support.
 *
 * Revision 1.11  1991/03/09  03:40:04  rich
 *  Modified Files:
 *  	Makefile b.out.c liba.out.h libbfd.c sunos.c sysdep.h
 *
 * Changes dictated by porting binutils.
 *
 * Revision 1.10  1991/03/08  07:52:02  sac
 * Reinstalled things which went away after latest merge from Intel.
 *
 * Fixed a couple of problems in symbol handling too.
 *
 * Revision 1.9  1991/03/08  04:18:16  rich
 * *** empty log message ***
 *
 * Revision 1.8  1991/03/07  21:57:26  sac
 * Moved type info out of the asymbol into the private space.
 * Cleaned up C++ stuff
 *
 * Revision 1.7  1991/03/06  21:49:02  sac
 * Modified bfd_find_filename to return name of function too.
 *
 * Revision 1.6  1991/03/06  02:19:36  sac
 * Moved howto table, added support for constructor sections and provided
 * sunos4_find_nearest_line
 *
 * Revision 1.5  1991/03/05  16:25:44  sac
 * Modified howto vector to include inplace and mask fields.
 *
 */

#define TARGET_BYTE_ORDER_BIG_P 1
#define TARGET TARGET_SPARC

#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include <stdio.h>


/*SUPPRESS558*/
/*SUPPRESS529*/




typedef void generic_symbol_type;
/* These values are correct for the SPARC.  I dunno about anything else */
#define PAGE_SIZE 0x02000
#define SEGMENT_SIZE PAGE_SIZE
#define TEXT_START_ADDR PAGE_SIZE
#include "a.out.gnu.h"
#include "stab.gnu.h"
#include "ar.h"
#include "liba.out.h"           /* BFD a.out internal data structures */

#include "a.out.sun4.h"

#define CTOR_TABLE_RELOC_IDX 2
static  reloc_howto_type howto_table[] = 
{
  /* type                   rs   size bsz  pcrel bitpos  abs ovrf sf name partial inplace mask*/
{ (unsigned int) RELOC_8,      0,  0,  	8,  false, 0, true,  true,0,"8", 	false, 0x000000ff},
{ (unsigned int) RELOC_16,     0,  1, 	16, false, 0, true,  true,0,"16", 	false, 0x0000ffff},
{ (unsigned int) RELOC_32,     0,  2, 	32, false, 0, true,  true,0,"32", 	false,  0xffffffff},
{ (unsigned int) RELOC_DISP8,  0,  0, 	8,  true,  0, false, true,0,"DISP8", 	false, 0x000000ff},
{ (unsigned int) RELOC_DISP16, 0,  1, 	16, true,  0, false, true,0,"DISP16", 	false, 0x0000ffff},
{ (unsigned int) RELOC_DISP32, 0,  2, 	32, true,  0, false, true,0,"DISP32", 	false, 0xffffffff},
{ (unsigned int) RELOC_WDISP30,2,  2, 	30, true,  0, false, true,0,"WDISP30", 	false, 0x3fffffff},
{ (unsigned int) RELOC_WDISP22,2,  2, 	22, true,  0, false, true,0,"WDISP22", 	false, 0x003fffff},
{ (unsigned int) RELOC_HI22,   10, 2, 	22, false, 0, false, true,0,"HI22",	false, 0x003fffff},
{ (unsigned int) RELOC_22,      0, 2, 	22, false, 0, false, true,0,"22",       false, 0x003fffff},
{ (unsigned int) RELOC_13, 	0, 2, 	13, false, 0, false, true,0,"13",       false, 0x00001fff},
{ (unsigned int) RELOC_LO10, 	0, 2, 	10, false, 0, false, true,0,"LO10",     false, 0x000003ff},
{ (unsigned int) RELOC_SFA_BASE,0, 2, 	32, false, 0, false, true,0,"SFA_BASE", false, 0xffffffff},
{ (unsigned int) RELOC_SFA_OFF13,0,2, 	32, false, 0, false, true,0,"SFA_OFF13",false, 0xffffffff},
{ (unsigned int) RELOC_BASE10, 0,  2, 	16, false, 0, false, true,0,"BASE10",   false, 0x0000ffff},
{ (unsigned int) RELOC_BASE13, 0,  2,	13, false, 0, false, true,0,"BASE13",   false, 0x00001fff},
{ (unsigned int) RELOC_BASE22, 0,  2,	0,  false, 0, false, true,0,"BASE22",   false, 0x00000000},
{ (unsigned int) RELOC_PC10,   0,  2,	10, false, 0, false, true,0,"PC10",	false, 0x000003ff},
{ (unsigned int) RELOC_PC22,   0,  2,	22, false, 0, false, true,0,"PC22",	false, 0x003fffff},
{ (unsigned int) RELOC_JMP_TBL,0,  2,	32, false, 0, false, true,0,"JMP_TBL",	false, 0xffffffff},
{ (unsigned int) RELOC_SEGOFF16,0, 2,	0,  false, 0, false, true,0,"SEGOFF16",	false, 0x00000000},
{ (unsigned int) RELOC_GLOB_DAT,0, 2,	0,  false, 0, false, true,0,"GLOB_DAT",	false, 0x00000000},
{ (unsigned int) RELOC_JMP_SLOT,0, 2,	0,  false, 0, false, true,0,"JMP_SLOT",	false, 0x00000000},
{ (unsigned int) RELOC_RELATIVE,0, 2,	0,  false, 0, false, true,0,"RELATIVE",	false, 0x00000000},
{ (unsigned int) RELOC_JUMPTARG,2, 13,	16, true,  0, false, true,0,"JUMPTARG",	false, 0x0000ffff},
{ (unsigned int) RELOC_CONST,	0, 13,	16, false, 0, false, true,0,"CONST",	false, 0x0000ffff},
{ (unsigned int) RELOC_CONSTH, 16, 13,	16, false, 0, false, true,0,"CONSTH",	false, 0x0000ffff},
};

/** a.out files */

PROTO (static void, swap_exec_header, (bfd *abfd, struct exec *execp));
PROTO (void , sunos4_write_syms, ());
PROTO (static boolean,sunos4_squirt_out_relocs,(bfd *abfd, asection *section));

/* Steve wants some way to frob this stuff from Saber while he's debugging
   ld, so we have these funny shadow functions */
/* ZMAGIC's start at 0 (making the exec part of the text section),
  other formats start after the exec
*/
unsigned int n_txtoff(ptr)
struct exec *ptr;
{return N_MAGIC(*ptr)== ZMAGIC ? 0: sizeof(struct exec);}

unsigned int n_datoff(ptr)
struct exec *ptr;
{return n_txtoff(ptr) + ptr->a_text;}

unsigned int n_treloff(ptr)
struct exec *ptr;
{return n_datoff(ptr) + ptr->a_data;}

unsigned int n_dreloff(ptr)
struct exec *ptr;
{return n_treloff(ptr) + ptr->a_trsize;}

unsigned int n_symoff(ptr)
struct exec *ptr;
{return n_dreloff(ptr) + ptr->a_drsize;}

unsigned int n_stroff(ptr)
struct exec *ptr;
{return n_symoff(ptr) + ptr->a_syms;}

unsigned int n_badmag(ptr)
     struct exec *ptr;
{
  switch (N_MAGIC(*ptr)) {
  case OMAGIC: case NMAGIC: case ZMAGIC: return 0;
  default: return 1;
  }
}


bfd_target *
sunos4_object_p (abfd)
     bfd *abfd;
{
  unsigned long magic;
  struct exec anexec;           /* save consing when you don't have to. */
  struct exec *execp = &anexec;
  void *rawptr;

  bfd_error = system_call_error;

  if (bfd_read ((void *)&magic, 1, sizeof (magic), abfd) != sizeof (magic))
    return 0;
  magic = bfd_h_getlong (abfd, &magic);

  /* Baroque syntax to mask deficiencies of the Sun compiler */
  /* if (N_BADMAG (*((struct exec *) &magic))) return 0; */
  if (n_badmag ((struct exec *) &magic)) return 0;

  if (bfd_seek (abfd, 0L, SEEK_SET) < 0) return 0;

  if (bfd_read ((void *) execp, 1, sizeof (struct exec), abfd)
      != sizeof (struct exec)) {
    bfd_error = wrong_format;
    return 0;
  }

  /* Use an intermediate variable for clarity */
  rawptr = (void *) zalloc (sizeof (struct sunexdata) + sizeof (struct exec));

  if (rawptr == NULL) {
    bfd_error = no_memory;
    return 0;
  }

  abfd->tdata =(void *)( (struct sunexdata *) rawptr);
  exec_hdr (abfd) =
    (struct exec *) ((char *)rawptr + sizeof (struct sunexdata));

  swap_exec_header (abfd, execp);
  memcpy (exec_hdr (abfd), execp, sizeof (struct exec));

  /* Set the file flags */
  abfd->flags = NO_FLAGS;
  if (execp->a_drsize || execp->a_trsize)
    abfd->flags |= HAS_RELOC;
  if (execp->a_entry) 
    abfd->flags |= EXEC_P;
  if (execp->a_syms) 
    abfd->flags |= HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS;


  if (N_MAGIC (anexec) == ZMAGIC) abfd->flags |= D_PAGED;
  if (N_MAGIC (anexec) == NMAGIC) abfd->flags |= WP_TEXT;

  /* Determine the architecture and machine type of the object file.  */
  abfd->obj_arch = bfd_arch_unknown;	/* Default values */
  abfd->obj_machine = 0;
  switch (N_MACHTYPE (anexec)) {

  case M_UNKNOWN:
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
	break;

  case M_386:
	abfd->obj_arch = bfd_arch_i386;
	break;

  case M_29K:
	abfd->obj_arch = bfd_arch_a29k;
	break;

  default:
	abfd->obj_arch = bfd_arch_obscure;
	break;
  }

  bfd_get_start_address (abfd) = execp->a_entry;

  /* Remember the positions of the string table and symbol table.  */
  obj_str_filepos (abfd) = n_stroff (&anexec);
  obj_sym_filepos (abfd) = n_symoff (&anexec);

  /* create the sections.  This is raunchy, but bfd_close wants to reclaim
     them */
  obj_textsec (abfd) = (asection *)NULL;
  obj_datasec (abfd) = (asection *)NULL;
  obj_bsssec (abfd) = (asection *)NULL;
  obj_aout_symbols(abfd) = (aout_symbol_type *)NULL;
  (void)bfd_make_section(abfd, ".text");
  (void)bfd_make_section(abfd, ".data");
  (void)bfd_make_section(abfd, ".bss");

  obj_datasec (abfd)->size = execp->a_data;
  obj_bsssec (abfd)->size = execp->a_bss;
  obj_textsec (abfd)->size = execp->a_text;
  obj_datasec (abfd)->vma = N_DATADDR(anexec);
  obj_bsssec (abfd)->vma = N_BSSADDR(anexec);
  obj_textsec (abfd)->vma = N_TXTADDR(anexec);

  obj_textsec (abfd)->filepos = n_txtoff(&anexec);
  obj_datasec (abfd)->filepos = n_datoff(&anexec);

  obj_textsec (abfd)->rel_filepos = n_treloff(&anexec);
  obj_datasec (abfd)->rel_filepos = n_dreloff(&anexec);

  obj_textsec (abfd)->flags =
    (execp->a_trsize != 0 ?
                 (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_HAS_CONTENTS) :
                 (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS));
  obj_datasec (abfd)->flags =
    (execp->a_drsize != 0 ?
                               (SEC_ALLOC | SEC_LOAD | SEC_RELOC  | SEC_HAS_CONTENTS) :
                               (SEC_ALLOC | SEC_LOAD  | SEC_HAS_CONTENTS));
  obj_bsssec (abfd)->flags = SEC_ALLOC;

  abfd->sections = obj_textsec (abfd);
  obj_textsec (abfd)->next = obj_datasec (abfd);
  obj_datasec (abfd)->next = obj_bsssec (abfd);
  return abfd->xvec;
}


boolean
sunos4_mkobject (abfd)
     bfd *abfd;
{
  char *rawptr;

  bfd_error = system_call_error;

  /* Use an intermediate variable for clarity */
  rawptr =  zalloc (sizeof (struct sunexdata) + sizeof (struct exec));

  if (rawptr == NULL) {
    bfd_error = no_memory;
    return false;
  }

  abfd->tdata = (void *)((struct sunexdata *) rawptr);
  exec_hdr (abfd) = (struct exec *) (rawptr + sizeof (struct sunexdata));

  /* For simplicity's sake we just make all the sections right here. */

  obj_textsec (abfd) = (asection *)NULL;
  obj_datasec (abfd) = (asection *)NULL;
  obj_bsssec (abfd) = (asection *)NULL;
  bfd_make_section (abfd, ".text");
  bfd_make_section (abfd, ".data");
  bfd_make_section (abfd, ".bss");

  return true;
}

/* Keep track of machine architecture and machine type for a.out's.
   Return the machine_type for a particular arch&machine, or M_UNKNOWN
   if that exact arch&machine can't be represented in a.out format.

   If the architecture is understood, machine type 0 (default) should
   always be understood.  */

static enum machine_type
aout_machine_type (arch, machine)
     enum bfd_architecture arch;
     unsigned long machine;
{
  enum machine_type arch_flags;

  arch_flags = M_UNKNOWN;

  switch (arch) {
  case bfd_arch_sparc:
	if (machine == 0)	arch_flags = M_SPARC;
	break;

  case bfd_arch_m68k:
	switch (machine) {
	case 0:			arch_flags = M_UNKNOWN; break;
	case 68000:		arch_flags = M_UNKNOWN;	break;
	case 68010:		arch_flags = M_68010; break;
	case 68020:		arch_flags = M_68020; break;
	default:		arch_flags = M_UNKNOWN; break;
	}
	break;

  case bfd_arch_i386:
	if (machine == 0)	arch_flags = M_386;
	break;

  case bfd_arch_a29k:
	if (machine == 0)	arch_flags = M_29K;
	break;

  default:
	arch_flags = M_UNKNOWN;
	break;
  }
  return arch_flags;
}

boolean
sunos4_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  abfd->obj_arch = arch;
  abfd->obj_machine = machine;
  if (arch != bfd_arch_unknown &&
      aout_machine_type (arch, machine) == M_UNKNOWN)
    return false;		/* We can't represent this type */
  return true;			/* We're easy ... */
}

boolean
sunos4_write_object_contents (abfd)
     bfd *abfd;
{
  int data_pad = 0;
  struct exec *execp = exec_hdr (abfd);



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
  execp->a_text = obj_textsec (abfd)->size;
  N_SET_MAGIC (*execp, OMAGIC);
  if (abfd->flags & D_PAGED) {
    execp->a_text = obj_textsec (abfd)->size + sizeof(struct exec);
    N_SET_MAGIC (*execp, ZMAGIC);
  } else if (abfd->flags & WP_TEXT) {
    N_SET_MAGIC (*execp, NMAGIC);
  }
  N_SET_FLAGS (*execp, 0x1);	/* copied from ld.c; who the hell knows? */

  if (abfd->flags & D_PAGED) 
    {
      data_pad = ((obj_datasec(abfd)->size + PAGE_SIZE -1)
		  & (- PAGE_SIZE)) - obj_datasec(abfd)->size;

      if (data_pad > obj_bsssec(abfd)->size)
	execp->a_bss = 0;
      else 
	execp->a_bss = obj_bsssec(abfd)->size - data_pad;
      execp->a_data = obj_datasec(abfd)->size + data_pad;

    }
  else {
    execp->a_data = obj_datasec (abfd)->size;
    execp->a_bss = obj_bsssec (abfd)->size;
  }

  execp->a_syms = bfd_get_symcount (abfd) * sizeof (struct nlist);
  execp->a_entry = bfd_get_start_address (abfd);
  execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *
		     sizeof (struct reloc_info_extended));
  execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *
		     sizeof (struct reloc_info_extended));;

  swap_exec_header (abfd, execp);

  bfd_seek (abfd, 0L, SEEK_SET);
  bfd_write ((void *) execp, 1, sizeof (struct exec), abfd);

  /* Now write out reloc info, followed by syms and strings */

  if (bfd_get_symcount (abfd) != 0) 
    {
      bfd_seek (abfd,
		(long)(N_SYMOFF(*execp)), SEEK_SET);

      sunos4_write_syms (abfd);

      bfd_seek (abfd,	(long)(N_TROFF(*execp)), SEEK_SET);

      if (!sunos4_squirt_out_relocs (abfd, obj_textsec (abfd))) return false;
      bfd_seek (abfd, (long)(N_DROFF(*execp)), SEEK_SET);

      if (!sunos4_squirt_out_relocs (abfd, obj_datasec (abfd))) return false;
    }
  return true;
}

static void
swap_exec_header (abfd, execp)
bfd *abfd;
     struct exec *execp;
{
  if (bfd_header_twiddle_required(abfd)) {
    /* execp->a_info = bfd_h_getlong (abfd, &execp->a_info); */
    *(unsigned long *) execp =
		      bfd_h_getlong (abfd, (unsigned long *) execp);
    execp->a_text   = bfd_h_getlong (abfd, &execp->a_text);
    execp->a_data   = bfd_h_getlong (abfd, &execp->a_data);
    execp->a_bss    = bfd_h_getlong (abfd, &execp->a_bss);
    execp->a_syms   = bfd_h_getlong (abfd, &execp->a_syms);
    execp->a_entry  = bfd_h_getlong (abfd, &execp->a_entry);
    execp->a_trsize = bfd_h_getlong (abfd, &execp->a_trsize);
    execp->a_drsize = bfd_h_getlong (abfd, &execp->a_drsize);
  }
} /* swap_exec_header() */

/** core files */

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

struct core {
  int c_magic;			/* Corefile magic number */
  int c_len;			/* Sizeof (struct core) */
  struct regs c_regs;		/* General purpose registers */
  struct exec c_aouthdr;	/* A.out header */
  int c_signo;			/* Killing signal, if any */
  int c_tsize;			/* Text size (bytes) */
  int c_dsize;			/* Data size (bytes) */
  int c_ssize;			/* Stack size (bytes) */
  char c_cmdname[CORE_NAMELEN + 1]; /* Command name */
  double fp_stuff[1];		/* external FPU state (size unknown by us) */
			/* The type "double" is critical here, for alignment.
			   SunOS declares a struct here, but the struct's
			   alignment is double since it contains doubles. */
  int c_ucode;			/* Exception no. from u_code */
			/* (this member not accessible by name since we don't
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
#define core_regsec2(bfd) (((struct suncordata *) ((bfd)->tdata))->reg2_section)

/* These are stored in the bfd's tdata */
struct suncordata {
  struct core *hdr;             /* core file header */
  asection *data_section;
  asection *stack_section;
  asection *reg_section;
  asection *reg2_section;
};

bfd_target *
sunos4_core_file_p (abfd)
     bfd *abfd;
{
  /* includes redundent variables for code clarity */
  int core_size;
  int core_mag;
  struct core *core;
  char *rawptr;

  bfd_error = system_call_error;

  if (bfd_read ((void *)&core_mag, 1, sizeof (int), abfd) != sizeof (int))
    return 0;
  core_mag = bfd_h_getlong(abfd, &core_mag);

  if (core_mag != CORE_MAGIC) return 0;

  /* SunOS core headers can vary in length; second word is size; */
  if (bfd_read ((void *)&core_size, 1, sizeof (int), abfd) != sizeof (int))
    return 0;
  core_size = bfd_h_getlong(abfd, &core_size);

  if (bfd_seek (abfd, 0L, SEEK_SET) < 0) return 0;

  rawptr = zalloc (core_size + sizeof (struct suncordata));
  if (rawptr == NULL) {
    bfd_error = no_memory;
    return 0;
  }

  core = (struct core *) (rawptr + sizeof (struct suncordata));

  if ((bfd_read ((void *) core, 1, core_size, abfd)) != core_size) {
    bfd_error = system_call_error;
    free ((void *)rawptr);
    return 0;
  }

  swapcore (abfd, core);
  abfd->tdata = (void *)((struct suncordata *) rawptr);
  core_hdr (abfd) = core;

  /* create the sections.  This is raunchy, but bfd_close wants to reclaim
     them */
  core_stacksec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_stacksec (abfd) == NULL) {
  loser:
    bfd_error = no_memory;
    free ((void *)rawptr);
    return 0;
  }
  core_datasec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_datasec (abfd) == NULL) {
  loser1:
    free ((void *)core_stacksec (abfd));
    goto loser;
  }
  core_regsec (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_regsec (abfd) == NULL) {
  loser2:
    free ((void *)core_datasec (abfd));
    goto loser1;
  }
  core_regsec2 (abfd) = (asection *) zalloc (sizeof (asection));
  if (core_regsec2 (abfd) == NULL) {
    free ((void *)core_regsec (abfd));
    goto loser2;
  }

  core_stacksec (abfd)->name = ".stack";
  core_datasec (abfd)->name = ".data";
  core_regsec (abfd)->name = ".reg";
  core_regsec2 (abfd)->name = ".reg2";

  core_stacksec (abfd)->flags = SEC_ALLOC + SEC_LOAD;
  core_datasec (abfd)->flags = SEC_ALLOC + SEC_LOAD;
  core_regsec (abfd)->flags = SEC_ALLOC;
  core_regsec2 (abfd)->flags = SEC_ALLOC;

  core_stacksec (abfd)->size = core->c_ssize;
  core_datasec (abfd)->size = core->c_dsize;
  core_regsec (abfd)->size = (sizeof core->c_regs);
  /* Float regs take up end of struct, except c_ucode. */
  core_regsec2 (abfd)->size = core_size - (sizeof core->c_ucode) - 
				(file_ptr)(((struct core *)0)->fp_stuff);

  core_stacksec (abfd)->vma = (USRSTACK - core->c_ssize);
  core_datasec (abfd)->vma = N_DATADDR(core->c_aouthdr);
  core_regsec (abfd)->vma = -1;
  core_regsec2 (abfd)->vma = -1;

  core_stacksec (abfd)->filepos = core->c_len + core->c_dsize;
  core_datasec (abfd)->filepos = core->c_len;
                        /* In file header: */
  core_regsec (abfd)->filepos = (file_ptr)(&((struct core *)0)->c_regs);
  core_regsec2 (abfd)->filepos = (file_ptr)(((struct core *)0)->fp_stuff);

  /* Align to word at least */
  core_stacksec (abfd)->alignment_power = 2;
  core_datasec (abfd)->alignment_power = 2;
  core_regsec (abfd)->alignment_power = 2;
  core_regsec2 (abfd)->alignment_power = 2;

  abfd->sections = core_stacksec (abfd);
  core_stacksec (abfd)->next = core_datasec (abfd);
  core_datasec (abfd)->next = core_regsec (abfd);
  core_regsec (abfd)->next = core_regsec2 (abfd);

  abfd->section_count = 4;

  return abfd->xvec;
}

char *
sunos4_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_hdr (abfd)->c_cmdname;
}

int
sunos4_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_hdr (abfd)->c_signo;
}

boolean
sunos4_core_file_matches_executable_p  (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  if (core_bfd->xvec != exec_bfd->xvec) {
    bfd_error = system_call_error;
    return false;
  }

  return (bcmp (&core_hdr (core_bfd), &exec_hdr (exec_bfd),
                sizeof (struct exec)) == 0) ? true : false;
}

/* byte-swap core structure */
static void
swapcore (abfd, core)
bfd *abfd;
     struct core *core;
{
  if (bfd_header_twiddle_required(abfd)) {
    core->c_magic = bfd_h_getlong (abfd, &core->c_magic);
    core->c_len   = bfd_h_getlong (abfd, &core->c_len);
    /* regs */
    swap_exec_header (abfd, &(core->c_aouthdr));
    core->c_signo = bfd_h_getlong (abfd, &core->c_signo);
    core->c_tsize = bfd_h_getlong (abfd, &core->c_tsize);
    core->c_dsize = bfd_h_getlong (abfd, &core->c_dsize);
    core->c_ssize = bfd_h_getlong (abfd, &core->c_ssize);
    core->c_ucode = bfd_h_getlong (abfd, &core->c_ucode);
    /* I don't understand how to swap an FP register */
  }
}

/** exec and core file sections */

boolean
sunos4_new_section_hook (abfd, newsect)
     bfd *abfd;
     asection *newsect;
{
  /* align to double at least */
  newsect->alignment_power = 3;

  if (bfd_get_format (abfd) == bfd_object) {
    if (obj_textsec(abfd) == NULL && !strcmp(newsect->name, ".text")) {
      obj_textsec(abfd)= newsect;
      return true;
    }

    if (obj_datasec(abfd) == NULL && !strcmp(newsect->name, ".data")) {
      obj_datasec(abfd) = newsect;
      return true;
    }

    if (obj_bsssec(abfd) == NULL && !strcmp(newsect->name, ".bss")) {
      obj_bsssec(abfd) = newsect;
      return true;
    }
  }

#if 0				/* FIXME -- this is temporary for steve */
  bfd_error = invalid_operation;

  return false;
#endif

  /* We allow more than three sections internally */
  return true;
}

boolean
sunos4_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     unsigned char *location;
     file_ptr offset;
      int count;
{
  if (abfd->output_has_begun == false)
    {				/* set by bfd.c handler */
      if ((obj_textsec (abfd) == NULL) || (obj_datasec (abfd) == NULL)
	  
	  /*||
	    (obj_textsec (abfd)->size == 0) || (obj_datasec (abfd)->size=
	    0)*/
	  ) 
	{
	  bfd_error = invalid_operation;
	  return false;
	}


#if 0
      if (abfd->flags & D_PAGED)
	{
	  obj_textsec (abfd)->filepos = sizeof(struct exec);
	  obj_datasec(abfd)->filepos =  obj_textsec (abfd)->size;
	}
      else 
#endif
{
	obj_textsec (abfd)->filepos = sizeof(struct exec);
	obj_datasec(abfd)->filepos = obj_textsec(abfd)->filepos  + obj_textsec (abfd)->size;

      }
    }
  /* regardless, once we know what we're doing, we might as well get going */

  bfd_seek (abfd, section->filepos + offset, SEEK_SET);

  if (count) {
    return (bfd_write ((void *)location, 1, count, abfd) == count) ? true : false;
  }
  return false;
}
boolean
sunos4_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     void  *location;
     file_ptr offset;
     int count;
{
  if (count) {
    if (offset >= section->size) return false;

    bfd_seek (abfd, section->filepos + offset, SEEK_SET);

    return (bfd_read (location, 1, count, abfd) == count) ? true:false;
  }
  else return true;
}


/* Classify stabs symbols */


#define sym_in_text_section(sym) \
     (((sym)->n_type  & (N_ABS | N_TEXT | N_DATA | N_BSS))== N_TEXT)

#define sym_in_data_section(sym) \
     (((sym)->n_type  & (N_ABS | N_TEXT | N_DATA | N_BSS))== N_DATA)

#define sym_in_bss_section(sym) \
     (((sym)->n_type  & (N_ABS | N_TEXT | N_DATA | N_BSS))== N_BSS)

/* Symbol is undefined if type is N_UNDF|N_EXT and if it has
   zero in the "value" field.  Nonzeroes there are fortrancommon
   symbols.  */
#define sym_is_undefined(sym) \
        ((sym)->n_type == (N_UNDF | N_EXT) && (sym)->n_value == 0)

/* Symbol is a global definition if N_EXT is on and if it has
   a nonzero type field.  */
#define sym_is_global_defn(sym) \
        (((sym)->n_type & N_EXT) && (sym)->n_type & N_TYPE)

/* Symbol is debugger info if any bits outside N_TYPE or N_EXT
   are on.  */
#define sym_is_debugger_info(sym) \
        ((sym)->n_type & ~(N_EXT | N_TYPE))

#define sym_is_fortrancommon(sym)       \
        (((sym)->n_type == (N_EXT)) && (sym)->n_value != 0)

/* Symbol is absolute if it has N_ABS set */
#define sym_is_absolute(sym) \
 	       (((sym)->n_type  & N_TYPE)== N_ABS)


#define sym_is_indirect(sym) \
	       (((sym)->n_type  & N_ABS)== N_ABS)

/* Only in their own functions for ease of debugging; when sym flags have
   stabilised these should be inlined into their (single) caller */

static void
translate_from_native_sym_flags (sym_pointer, cache_ptr, abfd)
     struct nlist *sym_pointer;
     aout_symbol_type *cache_ptr;
     bfd *abfd;
{
  switch (cache_ptr->type & N_TYPE) {
  case N_SETA:
  case N_SETT:
  case N_SETD:
  case N_SETB:
    {
      asection *section = bfd_make_section(abfd,
					   cache_ptr->symbol.name);
      arelent_chain *reloc = (arelent_chain *)malloc(sizeof(arelent_chain));

      switch ( (cache_ptr->type  & N_TYPE) ) {
      case N_SETA:
	reloc->relent.section =  (asection *)NULL;
	cache_ptr->symbol.section = (asection *)NULL;
	break;
      case N_SETT:
	reloc->relent.section = (asection *)obj_textsec(abfd);
	cache_ptr->symbol.value -= reloc->relent.section->vma;
	break;
      case N_SETD:
	reloc->relent.section = (asection *)obj_datasec(abfd);
	cache_ptr->symbol.value -= reloc->relent.section->vma;
	break;
      case N_SETB:
	reloc->relent.section = (asection *)obj_bsssec(abfd);
	cache_ptr->symbol.value -= reloc->relent.section->vma;
	break;
      }
      cache_ptr->symbol.section = reloc->relent.section;
      reloc->relent.addend = cache_ptr->symbol.value ;
      /* 
	 We modify the symbol to belong to a section depending upon the
	 name of the symbol - probably __CTOR__ or __DTOR__ but we don't
	 really care, and add to the size of the section to contain a
	 pointer to the symbol. Build a reloc entry to relocate to this
	 symbol attached to this section.
	 */


      section->flags = SEC_CONSTRUCTOR;
      section->reloc_count++;
      section->alignment_power = 2;
      reloc->relent.sym_ptr_ptr = (asymbol **)NULL;
      reloc->next = section->constructor_chain;
      section->constructor_chain = reloc;
      reloc->relent.address = section->size;
      section->size += sizeof(int *);

      reloc->relent.howto = howto_table +CTOR_TABLE_RELOC_IDX;
      cache_ptr->symbol.flags |=  BSF_DEBUGGING ;
      }
    break;
  default:

    if (sym_is_debugger_info (sym_pointer)) {
      cache_ptr->symbol.flags = BSF_DEBUGGING ;
      /* Work out the section correct for this symbol */
      switch (sym_pointer->n_type & N_TYPE) 
	{
	case N_TEXT:
	case N_FN:
	  cache_ptr->symbol.section = obj_textsec (abfd);
	  cache_ptr->symbol.value -= obj_textsec(abfd)->vma;
	  break;
	case N_DATA:
	  cache_ptr->symbol.value  -= obj_datasec(abfd)->vma;
	  cache_ptr->symbol.section = obj_datasec (abfd);
	  break;
	case N_BSS :
	  cache_ptr->symbol.section = obj_bsssec (abfd);
	  cache_ptr->symbol.value -= obj_bsssec(abfd)->vma;
	  break;
	case N_ABS:
	default:
	  cache_ptr->symbol.section = 0;
	  break;
	}
    }
    else {
      if (sym_is_fortrancommon (sym_pointer))
	{
	  cache_ptr->symbol.flags = BSF_FORT_COMM;
	  cache_ptr->symbol.section = (asection *)NULL;
	}
      else {
	if (sym_is_undefined (sym_pointer)) {
	  cache_ptr->symbol.flags = BSF_UNDEFINED;
	}
	else if (sym_is_global_defn (sym_pointer)) {
	  cache_ptr->symbol.flags = BSF_GLOBAL | BSF_EXPORT;
	}

	else if (sym_is_absolute (sym_pointer)) {
	  cache_ptr->symbol.flags = BSF_ABSOLUTE;
	}
	else {
	  cache_ptr->symbol.flags = BSF_LOCAL;
	}

	/* In a.out, the value of a symbol is always relative to the 
	 * start of the file, if this is a data symbol we'll subtract
	 * the size of the text section to get the section relative
	 * value. If this is a bss symbol (which would be strange)
	 * we'll subtract the size of the previous two sections
	 * to find the section relative address.
	 */

	if (sym_in_text_section (sym_pointer))   {
	  cache_ptr->symbol.value -= obj_textsec(abfd)->vma;
	  cache_ptr->symbol.section = obj_textsec (abfd);
	}
	else if (sym_in_data_section (sym_pointer)){
	  cache_ptr->symbol.value -= obj_datasec(abfd)->vma;
	  cache_ptr->symbol.section = obj_datasec (abfd);
	}
	else if (sym_in_bss_section(sym_pointer)) {
	  cache_ptr->symbol.section = obj_bsssec (abfd);
	  cache_ptr->symbol.value -= obj_bsssec(abfd)->vma;
	}
	else {
	  cache_ptr->symbol.section = (asection *)NULL;
	  cache_ptr->symbol.flags |= BSF_ABSOLUTE;
	}
      }
    }
  }
}
void
translate_to_native_sym_flags (sym_pointer, cache_ptr_g, abfd)
     struct nlist *sym_pointer;
     generic_symbol_type *cache_ptr_g;
     bfd *abfd;
{
  asymbol *cache_ptr = (asymbol *)cache_ptr_g;

  /* FIXME check for wrigin bss */
  if (bfd_get_section(cache_ptr)) {
    if (bfd_get_output_section(cache_ptr) == obj_bsssec (abfd)) {
      sym_pointer->n_type |= N_BSS;
    }
    else if (bfd_get_output_section(cache_ptr) == obj_datasec (abfd)) {
      sym_pointer->n_type |= N_DATA;
    }
    else  if (bfd_get_output_section(cache_ptr) == obj_textsec (abfd)) {
      sym_pointer->n_type |= N_TEXT;
    }
    else {
      BFD_ASSERT(0);
    }
    /* Turn the symbol from section relative to absolute again */
    sym_pointer->n_value +=
      cache_ptr->section->output_section->vma 
	+ cache_ptr->section->output_offset ;
  }
  else {
    sym_pointer->n_type |= N_ABS;
  }

  if (cache_ptr->flags & (BSF_FORT_COMM | BSF_UNDEFINED)) {
    sym_pointer->n_type = (N_UNDF | N_EXT);
    return;
  }

  if (cache_ptr->flags & BSF_ABSOLUTE) {
    sym_pointer->n_type |= N_ABS;
  }

  if (cache_ptr->flags & (BSF_GLOBAL | BSF_EXPORT)) {
    sym_pointer->n_type |= N_EXT;
  }
  if (cache_ptr->flags & BSF_DEBUGGING) {
    sym_pointer->n_type = ((aout_symbol_type *)cache_ptr)->type;
  }
  
}

/* Native-level interface to symbols. */

/* We read the symbols into a buffer, which is discarded when this
   function exits.  We read the strings into a buffer large enough to
   hold them all plus all the cached symbol entries. */

asymbol *
sunos4_make_empty_symbol (abfd)
bfd *abfd;
{
  aout_symbol_type  *new =
    (aout_symbol_type *)zalloc (sizeof (aout_symbol_type));
  new->symbol.the_bfd = abfd;

  return &new->symbol;
}

boolean
sunos4_slurp_symbol_table (abfd)
     bfd *abfd;
{
  unsigned int symbol_count;
  size_t symbol_size;
  size_t string_size;
  struct nlist *syms;
  char *strings;
  aout_symbol_type *cached;

  /* If there's no work to be done, don't do any */
  if (obj_aout_symbols (abfd) != (aout_symbol_type *)NULL) return true;
  symbol_size = exec_hdr(abfd)->a_syms;
  if (symbol_size == 0) {
    bfd_error = no_symbols;
    return false;
  }

  bfd_seek (abfd, obj_str_filepos (abfd), SEEK_SET);
  if (bfd_read ((void *)&string_size, 4, 1, abfd) != 4)
    return false;
  string_size = bfd_h_getlong (abfd, (unsigned char *)&string_size);

  symbol_count = symbol_size / sizeof (struct nlist);

  /* Malloc (should alloca) space for native symbols, and
     malloc space for string table and symbol cache. */

  syms = (struct nlist *) zalloc (symbol_size);
  if (syms == NULL) {
    bfd_error = no_memory;
    return false;
  }

  cached = (aout_symbol_type *) zalloc ((size_t)(string_size + 1 +
                               (symbol_count * sizeof (aout_symbol_type))));
  if (cached == NULL) {
    bfd_error = no_memory;
    free ((void *)syms);
    return false;
  }

  strings = ((char *) cached) + (symbol_count * sizeof (aout_symbol_type));

  bfd_seek (abfd, obj_sym_filepos (abfd), SEEK_SET);
  if (bfd_read ((void *)syms, 1, symbol_size, abfd) != symbol_size) {
  bailout:
    free ((void *)cached);
    free ((void*)syms);
    return false;
  }

  bfd_seek (abfd, obj_str_filepos (abfd), SEEK_SET);
  if (bfd_read ((void *)strings, 1, string_size, abfd) != string_size) {
    goto bailout;
  }

  /* OK, now walk the new symtable, cacheing symbol properties */
  {
    register struct nlist *sym_pointer;
    register struct nlist *sym_end = syms + symbol_count;
    register aout_symbol_type *cache_ptr = cached;

    if (bfd_header_twiddle_required (abfd) == true) {
      /* run through the table and byte swap if needed */
      for (sym_pointer = syms; sym_pointer < sym_end;  sym_pointer++) {
        sym_pointer->n_un.n_strx =
	  bfd_h_get_x (abfd, &sym_pointer->n_un.n_strx);
        sym_pointer->n_desc =
	  bfd_h_get_x (abfd, &sym_pointer->n_desc);
        sym_pointer->n_value =
	  bfd_h_get_x (abfd, &sym_pointer->n_value);
	sym_pointer->n_other = (char)
	  bfd_h_get_x(abfd, &sym_pointer->n_other);
	sym_pointer->n_type = (char)
	  bfd_h_get_x(abfd, &sym_pointer->n_type);

      }
    }
    /* Run through table and copy values */
    for (sym_pointer = syms, cache_ptr = cached;
	 sym_pointer < sym_end; sym_pointer++, cache_ptr++) 
      {
	cache_ptr->symbol.the_bfd = abfd;
	if (sym_pointer->n_un.n_strx)
	  cache_ptr->symbol.name = sym_pointer->n_un.n_strx + strings;
	else
	  cache_ptr->symbol.name = (char *)NULL;
	cache_ptr->symbol.value = sym_pointer->n_value;
	cache_ptr->desc = sym_pointer->n_desc;
	cache_ptr->other = sym_pointer->n_other;
	cache_ptr->type = sym_pointer->n_type;
	cache_ptr->symbol.udata = 0;
	translate_from_native_sym_flags (sym_pointer, cache_ptr, abfd);

      }
  }

  obj_aout_symbols (abfd) =  cached;
  bfd_get_symcount (abfd) = symbol_count;
  free ((void *)syms);

  return true;
}


void
sunos4_write_syms (abfd)
     bfd *abfd;
{
  unsigned int count ;
  asymbol **generic = bfd_get_outsymbols (abfd);

  unsigned int stindex = sizeof(stindex); /* initial string length */

  for (count = 0; count < bfd_get_symcount (abfd); count++) {
    asymbol *g = generic[count];
    struct nlist nsp;

    if (g->name) {
      unsigned int length = strlen(g->name) +1;
      bfd_h_putlong  (abfd, stindex, (unsigned char *)&nsp.n_un.n_strx);
      stindex += length;
    }
    else {
      bfd_h_putlong  (abfd, 0, (unsigned char *)&nsp.n_un.n_strx);
    }

    if (g->the_bfd->xvec->flavour == abfd->xvec->flavour) 
      {
	nsp.n_desc = aout_symbol( g)->desc;
	nsp.n_other = aout_symbol(g)->other;
	nsp.n_type = aout_symbol(g)->type;
      }
    else
      {
	nsp.n_desc = 0;
	nsp.n_other = 0;
	nsp.n_type = 0;
      }


    nsp.n_value = g->value;
    translate_to_native_sym_flags (&nsp, (generic_symbol_type *)g, abfd);


    bfd_h_putshort (abfd, nsp.n_desc, (unsigned char *)&nsp.n_desc);
    bfd_h_putlong  (abfd, nsp.n_value, (unsigned char *)&nsp.n_value);
    bfd_write((void *)&nsp,1, sizeof(nsp), abfd);
  }


  /* Now output the strings.  Be sure to put string length into correct
   * byte ordering before writing it.
   */
  bfd_h_putlong  (abfd, stindex, (unsigned char *)&stindex);

  bfd_write((void *)&stindex, 1, sizeof(stindex), abfd);
  
  generic = bfd_get_outsymbols(abfd);
  for (count = 0; count < bfd_get_symcount(abfd); count++) 
    {
      asymbol *g = *(generic++);

      if (g->name != (char *)NULL) 
	{
	  size_t length = strlen(g->name)+1;
	  bfd_write((void *)g->name, 1, length, abfd);
	}
      if ((g->flags & BSF_FAKE)==0) {
	g->name = itos(count);	/* smash the generic symbol */
      }
    }
}


void
sunos4_reclaim_symbol_table (abfd)
     bfd *abfd;
{
  asection *section;

  if (!bfd_get_symcount (abfd)) return;

  for (section = abfd->sections;
       section != (asection *) NULL;
       section = section->next)
    if (section->relocation) {
      free ((void *)section->relocation);
      section->relocation = NULL;
      section->reloc_count = 0;
    }

  bfd_get_symcount (abfd) = 0;
  free ((void *)obj_aout_symbols (abfd));
  obj_aout_symbols (abfd) = (aout_symbol_type *)NULL;
}

unsigned int
sunos4_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  if (!sunos4_slurp_symbol_table (abfd)) return 0;

  return (bfd_get_symcount (abfd)+1) * (sizeof (aout_symbol_type *));
}

unsigned int
sunos4_get_symtab (abfd, location)
     bfd *abfd;
     asymbol **location;
{
  unsigned int counter = 0;
  aout_symbol_type *symbase;

  if (!sunos4_slurp_symbol_table (abfd)) return 0;

  for (symbase = obj_aout_symbols(abfd); counter++ < bfd_get_symcount (abfd);)
    *(location++) = (asymbol *)( symbase++);
  *location++ =0;
  return bfd_get_symcount(abfd);
}


/* Obsolete procedural interface; better to look at the cache directly */

/* User should have checked the file flags; perhaps we should return
   BFD_NO_MORE_SYMBOLS if there are none? */

int
sunos4_get_symcount_upper_bound (abfd)
     bfd *abfd;
{
  /* In case we're doing an output file or something...?  */
  if (bfd_get_symcount (abfd)) return bfd_get_symcount (abfd);

  return (exec_hdr (abfd)->a_syms) / (sizeof (struct nlist));
}

symindex
sunos4_get_first_symbol (ignore_abfd)
     bfd * ignore_abfd;
{
  return 0;
}

symindex
sunos4_get_next_symbol (abfd, oidx)
     bfd *abfd;
     symindex oidx;
{
  if (oidx == BFD_NO_MORE_SYMBOLS) return BFD_NO_MORE_SYMBOLS;
  return ++oidx >= bfd_get_symcount (abfd) ? BFD_NO_MORE_SYMBOLS : oidx;
}

char *
sunos4_symbol_name (abfd, idx)
     bfd *abfd;
     symindex idx;
{
  return (obj_aout_symbols (abfd) + idx)->symbol.name;
}

long
sunos4_symbol_value (abfd, idx)
     bfd *abfd;
     symindex idx;
{
  return (obj_aout_symbols (abfd) + idx)->symbol.value;
}

symclass
sunos4_classify_symbol (abfd, idx)
     bfd *abfd;
     symindex idx;
{
  aout_symbol_type *sym = obj_aout_symbols (abfd) + idx;

  if ((sym->symbol.flags & BSF_FORT_COMM) != 0)   return bfd_symclass_fcommon;
  if ((sym->symbol.flags & BSF_GLOBAL) != 0)    return bfd_symclass_global;
  if ((sym->symbol.flags & BSF_DEBUGGING) != 0)  return bfd_symclass_debugger;
  if ((sym->symbol.flags & BSF_UNDEFINED) != 0) return bfd_symclass_undefined;

  return bfd_symclass_unknown;
}

boolean
sunos4_symbol_hasclass (abfd, idx, class)
     bfd *abfd;
     symindex idx;
     symclass class;
{
  aout_symbol_type *sym = obj_aout_symbols (abfd) + idx;
  switch (class) {
  case bfd_symclass_fcommon:
    return (sym->symbol.flags & BSF_FORT_COMM) ? true :false;
  case bfd_symclass_global:
    return (sym->symbol.flags & BSF_GLOBAL) ? true:false;
  case bfd_symclass_debugger:
    return (sym->symbol.flags & BSF_DEBUGGING) ? true:false;;
  case bfd_symclass_undefined:
    return (sym->symbol.flags & BSF_UNDEFINED) ? true:false;;
  default: return false;
  }
}

/** Some reloc hackery */


boolean
sunos4_slurp_reloc_table (abfd, asect, symbols)
     bfd *abfd;
     sec_ptr asect;
     asymbol **symbols;
{
  unsigned int count;
  size_t reloc_size;
  struct reloc_info_extended *relocs;
  arelent *reloc_cache;

  if (asect->relocation) return true;

  if (asect->flags & SEC_CONSTRUCTOR) return true;

  if (asect == obj_datasec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_drsize;
    goto doit;
  }

  if (asect == obj_textsec (abfd)) {
    reloc_size = exec_hdr(abfd)->a_trsize;
    goto doit;
  }

  bfd_error = invalid_operation;
  return false;

 doit:
  bfd_seek (abfd, asect->rel_filepos, SEEK_SET);
  count = reloc_size / sizeof (struct reloc_info_extended);

  relocs = (struct reloc_info_extended *) malloc (reloc_size);
  if (!relocs) {
    bfd_error = no_memory;
    return false;
  }
  reloc_cache = (arelent *) zalloc ((size_t)(count * sizeof (arelent)));
  if (reloc_cache == (arelent *)NULL) {
    free ((void *)relocs);
    bfd_error = no_memory;
    return false;
  }

  if (bfd_read ((void*) relocs, 1, reloc_size, abfd) != reloc_size) {
    bfd_error = system_call_error;
    free (reloc_cache);
    free (relocs);
    return false;
  }

  {
    register struct reloc_info_extended *rptr = relocs;
    unsigned int counter = 0;
    arelent *cache_ptr = reloc_cache;

    for (; counter < count; counter++, rptr++, cache_ptr++) {
      /* john's old swap_reloc was a hell of a lot hairier... */
      /* FIXME, it needs to be!!! */
      rptr->r_address = bfd_h_getlong (abfd, &rptr->r_address);
      /* FIXME NOW!
       *  The following can't be done because r_index is a bit field:
       *
       * rptr->r_index   = bfd_h_getlong (abfd, &rptr->r_index);
       */
      rptr->r_addend  = bfd_h_getlong (abfd, &rptr->r_addend);
      if (rptr->r_extern) {
	/* If this bit is set then the r_index is a index into the symbol table
	 * if the bit is not set then r_index contains a section map.
	 * We either fill in the sym entry with a pointer to the symbol,
	 * or point to the correct section
	 */
	
	cache_ptr->sym_ptr_ptr = symbols + rptr->r_index;
	cache_ptr->addend =  rptr->r_addend;
	cache_ptr->section = (asection *)NULL;
      }
      else 
	{
	  /* Remember that in a.out symbols are relative to the
	 beginning of the file rather than sections ? (look in
	 translate_from_native_sym_flags) The reloc entry addend
	 has added to it the offset into the  file of the data, so
	 subtract the base to make the reloc section relative */

	  cache_ptr->sym_ptr_ptr = (asymbol **)NULL;
	  switch (rptr->r_index) {
	  case N_TEXT:
	  case N_TEXT | N_EXT:
	    cache_ptr->section = obj_textsec(abfd);
	    cache_ptr->addend = rptr->r_addend -  obj_textsec(abfd)->vma  ;
	    break;
	  case N_DATA:
	  case N_DATA | N_EXT:
	    cache_ptr->section = obj_datasec(abfd);
	    cache_ptr->addend = rptr->r_addend -  obj_datasec(abfd)->vma  ;
	    break;
	  case N_BSS:
	  case N_BSS | N_EXT:
	    cache_ptr->section = obj_bsssec(abfd);
	    cache_ptr->addend = rptr->r_addend - obj_bsssec(abfd)->vma;
	    break;
	  case N_ABS:
	  case N_ABS | N_EXT:
	    BFD_ASSERT(1);
	    break;
	  default:
	    BFD_ASSERT(1);
	    break;
	  }
	
	}

      cache_ptr->address = rptr->r_address;
      cache_ptr->howto =  howto_table + (unsigned int)( rptr->r_type);
    }
  }

  free (relocs);
  asect->relocation = reloc_cache;
  asect->reloc_count = count;
  return true;
}

static boolean
sunos4_squirt_out_relocs (abfd, section)
     bfd *abfd;
     asection *section;
{
  arelent **generic;

  unsigned int count = section->reloc_count;
  struct reloc_info_extended *native, *natptr;
  size_t natsize = count * sizeof (struct reloc_info_extended);

  if (count == 0) return true;
  generic   = section->orelocation;
  native = ((struct reloc_info_extended *) zalloc (natsize));
  if (!native) {
    bfd_error = no_memory;
    return false;
  }

  for (natptr = native; count != 0; --count, ++natptr, ++generic) 
    {
      arelent *g = *generic;

      natptr->r_address = g->address;
      /* Find a type in the output format which matches the input howto - 
       * at the moment we assume input format == output format FIXME!!
       */
      natptr->r_type = (enum reloc_type) g->howto->type;
      /* name clobbered by sunos4_write_syms to be symbol index*/

      if (g->sym_ptr_ptr != (asymbol **)NULL) 
	{
	  if ((*(g->sym_ptr_ptr))->section) {
	    /* replace the section offset into the addent */
	    g->addend += (*(g->sym_ptr_ptr))->section->vma;
	  }

	  natptr->r_index = stoi((*(g->sym_ptr_ptr))->name);
	BFD_ASSERT(natptr->r_index < 0xfff);
	  natptr->r_extern = 1;
	  natptr->r_addend = g->addend;
	}
      else {
	natptr->r_extern = 0;
	if (g->section == (asection *)NULL) {
	  BFD_ASSERT(0);
	  natptr->r_index = N_ABS | N_EXT;
	  natptr->r_addend = g->addend;
	}
	else  if(g->section->output_section == obj_textsec(abfd)) {
	  natptr->r_index = N_TEXT | N_EXT;
	  natptr->r_addend = g->addend + obj_textsec(abfd)->vma;
	}
	else if (g->section->output_section == obj_datasec(abfd)) {
	  natptr->r_index = N_DATA | N_EXT;
	  natptr->r_addend = g->addend + obj_datasec(abfd)->vma;
	}
	else if (g->section->output_section == obj_bsssec(abfd)) {
	  natptr->r_index = N_BSS | N_EXT ;
	  natptr->r_addend = g->addend + obj_bsssec(abfd)->vma;

	}
	else {
	  BFD_ASSERT(0);
	}
      }
  
      if ( bfd_header_twiddle_required(abfd) ){
        bfd_h_putlong ( abfd, natptr->r_address, &natptr->r_address );

        /* FIXME -- NOW!
	 *
         * I came through here as part of my campaign to make compile-time
         * definition of host and target byte orders unnecessary. The new
	 * code won't even compile because but r_index is a bit field.
	 * But the simple-minded byte swap that was here before wasn't going
	 * to work on a bit field anyway.
	 *
         * The old code used to be #ifdef'd on 
         *		TARGET_BYTE_ORDER_BIG_P != HOST_BYTE_ORDER_BIG_P
         * Apparently, it was never tested in such a condition: the
         * macro invocations here (of swapoutlong) had the wrong number
         * of arguments and would never have compiled.
         * 			Chris
        bfd_h_putlong ( abfd, natptr->r_index, &natptr->r_index );
         */

        bfd_h_putlong ( abfd, natptr->r_addend, &natptr->r_addend );
      }

    }

  if ( bfd_write ((void *) native, 1, natsize, abfd) != natsize) {
    free(native);
    return false;
  }
  free (native);

  return true;
}

/* This is stupid.  This function should be a boolean predicate */
unsigned int
sunos4_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  arelent *tblptr = section->relocation;
  unsigned int count;

  if (!(tblptr || sunos4_slurp_reloc_table (abfd, section, symbols)))
    return 0;

  if (section->flags & SEC_CONSTRUCTOR) {
    arelent_chain *chain = section->constructor_chain;
    for (count = 0; count < section->reloc_count; count ++) {
      *relptr ++ = &chain->relent;
      chain = chain->next;
    }
  }
  else {
    tblptr = section->relocation;
    if (!tblptr) return 0;

    for (count = 0; count++ < section->reloc_count;) 
      {
	*relptr++ = tblptr++;
      }
  }
  *relptr = 0;

  return section->reloc_count;
}

unsigned int
sunos4_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (bfd_get_format (abfd) != bfd_object) {
    bfd_error = invalid_operation;
    return 0;
  }
  if (asect->flags & SEC_CONSTRUCTOR) {
    return (sizeof (arelent *) * (asect->reloc_count+1));
  }

  if (asect == obj_datasec (abfd))
    return (sizeof (arelent *) *
            ((exec_hdr(abfd)->a_drsize / sizeof (struct reloc_info_extended))
             +1));

  if (asect == obj_textsec (abfd))
    return (sizeof (arelent *) *
            ((exec_hdr(abfd)->a_trsize / sizeof (struct reloc_info_extended))
             +1));

  bfd_error = invalid_operation;
  return 0;
}

void
sunos4_reclaim_reloc (ignore_abfd, section)
     bfd *ignore_abfd;
     sec_ptr section;
{
  if (section->relocation) {
    free (section->relocation);
    section->relocation = NULL;
    section->reloc_count = 0;
    }
}


alent *
sunos4_get_lineno(ignore_abfd, ignore_symbol)
bfd *ignore_abfd;
generic_symbol_type *ignore_symbol;
{
return (alent *)NULL;
}

void 
sunos4_print_symbol(ignore_abfd, file,  symbol, how)
bfd *ignore_abfd;
FILE *file;
asymbol *symbol;
bfd_print_symbol_enum_type how;
{
  switch (how) {
  case bfd_print_symbol_name_enum:
    fprintf(file,"%s", symbol->name);
    break;
  case bfd_print_symbol_type_enum:
    fprintf(file,"%4x %2x %2x",(unsigned)(aout_symbol(symbol)->desc & 0xffff),
	    (unsigned)(   aout_symbol(symbol)->other  & 0xff),
	    (unsigned)(aout_symbol(symbol)->type));
    break;
  case bfd_print_symbol_all_enum:
    {
      char *section_name = symbol->section == (asection *)NULL ?
	"*abs" : symbol->section->name;

      bfd_print_symbol_vandf((void *)file,symbol);

      fprintf(file," %-5s %04x %02x %02x %s",
	      section_name,
	      (unsigned)(aout_symbol(symbol)->desc & 0xffff),
	      (unsigned)(aout_symbol(symbol)->other  & 0xff),
	      (unsigned)(aout_symbol(symbol)->type  & 0xff),
	      symbol->name);
    }
    break;
  }
}
/* Once we know all the stuff that could be consed, we know how to clean
   it up.  So why don't we? */

boolean
sunos4_close_and_cleanup (abfd)
     bfd *abfd;
{
  if (!bfd_read_p (abfd))
    switch (abfd->format) {
    case bfd_archive:
      if (!_bfd_write_archive_contents (abfd)) return false; break;
    case bfd_object:
      if (!sunos4_write_object_contents (abfd))  return false; break;
    default: bfd_error = invalid_operation; return false;
    }

#define cleaner(ptr) if (abfd->ptr) free (abfd->ptr)
  cleaner (tdata);

  if (abfd->my_archive)
    cleaner (filename);

#undef cleaner
  return true;
}

/* 
 provided a bfd, a section and an offset into the section, calculate
 and return the name of the source file and the line nearest to the
 wanted location.
*/
 
boolean
sunos4_find_nearest_line(abfd,
			 section,
			 symbols,
			 offset,
			 filename_ptr,
			 functionname_ptr,
			 line_ptr)
bfd *abfd;
asection *section;
asymbol **symbols;
bfd_vma offset;
char **filename_ptr;
char **functionname_ptr;
unsigned int *line_ptr;
{
  /* Run down the file looking for the filename, function and linenumber */
  asymbol **p;
  static  char buffer[100];
  bfd_vma high_line_vma = ~0;
  bfd_vma low_func_vma = 0;
  asymbol *func = 0;
  *filename_ptr = abfd->filename;
  *functionname_ptr = 0;
  *line_ptr = 0;
  if (symbols != (asymbol **)NULL) {
    for (p = symbols; *p; p++) {
      aout_symbol_type  *q = (aout_symbol_type *)(*p);
      switch (q->type){
      case N_SO:
	*filename_ptr = q->symbol.name;
	if (obj_textsec(abfd) != section) {
	  return true;
	}
	break;
      case N_SLINE:

      case N_DSLINE:
      case N_BSLINE:
	/* We'll keep this if it resolves nearer than the one we have already */
	if (q->symbol.value >= offset &&
	    q->symbol.value < high_line_vma) {
	  *line_ptr = q->desc;
	  high_line_vma = q->symbol.value;
	}
	break;
      case N_FUN:
	{
	  /* We'll keep this if it is nearer than the one we have already */
	  if (q->symbol.value >= low_func_vma &&
	      q->symbol.value <= offset) {
	    low_func_vma = q->symbol.value;
	    func = (asymbol *)q;
	  }
	  if (*line_ptr && func) {
	    char *function = func->name;
	    char *p;
	    strncpy(buffer, function, sizeof(buffer)-1);
	    buffer[sizeof(buffer)-1] = 0;
	    /* Have to remove : stuff */
	    p = strchr(buffer,':');
	    if (p != NULL) {*p = NULL; }
	    *functionname_ptr = buffer;
	    return true;

	  }
	}
	break;
      }
    }
  }
  
  return true;

}

bfd_target aoutvec =
{
  "a.out-generic-big",		/* name */
  bfd_target_aout_flavour_enum,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  0,				/* valid reloc types */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  sunos4_close_and_cleanup,	/* _close_and_cleanup */
  sunos4_set_section_contents,	/* bfd_set_section_contents */
  sunos4_get_section_contents,	/* bfd_get_section_contents */
  sunos4_new_section_hook,	/* new_section_hook */
  sunos4_core_file_failing_command, /* _core_file_failing_command */
  sunos4_core_file_failing_signal, /* _core_file_failing_signal */
  sunos4_core_file_matches_executable_p, /* _core_file_matches_ex...p */

  bfd_slurp_bsd_armap,		/* bfd_slurp_armap */
  bfd_true,			/* bfd_slurp_extended_name_table */
  bfd_bsd_truncate_arname,	/* bfd_truncate_arname */

  sunos4_get_symtab_upper_bound, /* get_symtab_upper_bound */
  sunos4_get_symtab,		/* canonicalize_symtab */
  sunos4_reclaim_symbol_table,	/* bfd_reclaim_symbol_table */
  sunos4_get_reloc_upper_bound,	/* get_reloc_upper_bound */
  sunos4_canonicalize_reloc,	/* bfd_canonicalize_reloc */
  sunos4_reclaim_reloc,		/* bfd_reclaim_reloc */
  sunos4_get_symcount_upper_bound, /* bfd_get_symcount_upper_bound */
  sunos4_get_first_symbol,	/* bfd_get_first_symbol */
  sunos4_get_next_symbol,	/* bfd_get_next_symbol */
  sunos4_classify_symbol,	/* bfd_classify_symbol */
  sunos4_symbol_hasclass,	/* bfd_symbol_hasclass */
  sunos4_symbol_name,		/* bfd_symbol_name */
  sunos4_symbol_value,		/* bfd_symbol_value */

  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* data */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

  {_bfd_dummy_target, sunos4_object_p, /* bfd_check_format */
     bfd_generic_archive_p, sunos4_core_file_p},
  {bfd_false, sunos4_mkobject,	/* bfd_zxset_format */
     _bfd_generic_mkarchive, bfd_false},
  sunos4_make_empty_symbol,
  sunos4_print_symbol,
  sunos4_get_lineno,
  sunos4_set_arch_mach,
  bsd_write_armap,
  bfd_generic_openr_next_archived_file,
  sunos4_find_nearest_line,	/* bfd_find_nearest_line */
  bfd_generic_stat_arch_elt /* bfd_stat_arch_elt */
  };

