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
 *
 */

#define TARGET_BYTE_ORDER_BIG_P 1

#include <ansidecl.h>
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"


void (*bfd_error_trap)();

/*SUPPRESS558*/
/*SUPPRESS529*/





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
static  reloc_howto_type howto_table_ext[] = 
{
  /* type                   rs   size bsz  pcrel bitpos  abs ovrf sf name partial inplace mask*/
{ (unsigned int) RELOC_8,      0,  0,  	8,  false, 0, true,  true,0,"8", 	false, 0,0x000000ff},
{ (unsigned int) RELOC_16,     0,  1, 	16, false, 0, true,  true,0,"16", 	false, 0,0x0000ffff},
{ (unsigned int) RELOC_32,     0,  2, 	32, false, 0, true,  true,0,"32", 	false, 0,0xffffffff},
{ (unsigned int) RELOC_DISP8,  0,  0, 	8,  true,  0, false, true,0,"DISP8", 	false, 0,0x000000ff},
{ (unsigned int) RELOC_DISP16, 0,  1, 	16, true,  0, false, true,0,"DISP16", 	false, 0,0x0000ffff},
{ (unsigned int) RELOC_DISP32, 0,  2, 	32, true,  0, false, true,0,"DISP32", 	false, 0,0xffffffff},
{ (unsigned int) RELOC_WDISP30,2,  2, 	30, true,  0, false, true,0,"WDISP30", 	false, 0,0x3fffffff},
{ (unsigned int) RELOC_WDISP22,2,  2, 	22, true,  0, false, true,0,"WDISP22", 	false, 0,0x003fffff},
{ (unsigned int) RELOC_HI22,   10, 2, 	22, false, 0, false, true,0,"HI22",	false, 0,0x003fffff},
{ (unsigned int) RELOC_22,      0, 2, 	22, false, 0, false, true,0,"22",       false, 0,0x003fffff},
{ (unsigned int) RELOC_13, 	0, 2, 	13, false, 0, false, true,0,"13",       false, 0,0x00001fff},
{ (unsigned int) RELOC_LO10, 	0, 2, 	10, false, 0, false, true,0,"LO10",     false, 0,0x000003ff},
{ (unsigned int) RELOC_SFA_BASE,0, 2, 	32, false, 0, false, true,0,"SFA_BASE", false, 0,0xffffffff},
{ (unsigned int) RELOC_SFA_OFF13,0,2, 	32, false, 0, false, true,0,"SFA_OFF13",false, 0,0xffffffff},
{ (unsigned int) RELOC_BASE10, 0,  2, 	16, false, 0, false, true,0,"BASE10",   false, 0,0x0000ffff},
{ (unsigned int) RELOC_BASE13, 0,  2,	13, false, 0, false, true,0,"BASE13",   false, 0,0x00001fff},
{ (unsigned int) RELOC_BASE22, 0,  2,	0,  false, 0, false, true,0,"BASE22",   false, 0,0x00000000},
{ (unsigned int) RELOC_PC10,   0,  2,	10, false, 0, false, true,0,"PC10",	false, 0,0x000003ff},
{ (unsigned int) RELOC_PC22,   0,  2,	22, false, 0, false, true,0,"PC22",	false, 0,0x003fffff},
{ (unsigned int) RELOC_JMP_TBL,0,  2,	32, false, 0, false, true,0,"JMP_TBL",	false, 0,0xffffffff},
{ (unsigned int) RELOC_SEGOFF16,0, 2,	0,  false, 0, false, true,0,"SEGOFF16",	false, 0,0x00000000},
{ (unsigned int) RELOC_GLOB_DAT,0, 2,	0,  false, 0, false, true,0,"GLOB_DAT",	false, 0,0x00000000},
{ (unsigned int) RELOC_JMP_SLOT,0, 2,	0,  false, 0, false, true,0,"JMP_SLOT",	false, 0,0x00000000},
{ (unsigned int) RELOC_RELATIVE,0, 2,	0,  false, 0, false, true,0,"RELATIVE",	false, 0,0x00000000},
{ (unsigned int) RELOC_JUMPTARG,2, 13,	16, true,  0, false, true,0,"JUMPTARG",	false, 0,0x0000ffff},
{ (unsigned int) RELOC_CONST,	0, 13,	16, false, 0, false, true,0,"CONST",	false, 0,0x0000ffff},
{ (unsigned int) RELOC_CONSTH, 16, 13,	16, false, 0, false, true,0,"CONSTH",	false, 0,0x0000ffff},
};

/* Convert standard reloc records to "arelent" format (incl byte swap).  */

static  reloc_howto_type howto_table_std[] = {
  /* type                   rs   size bsz  pcrel bitpos  abs ovrf sf name*/
{ (unsigned int) 0,	       0,  0,  	8,  false, 0, true,  true,0,"8",	true, 0x000000ff,0x000000ff},
{ (unsigned int) 1,	       0,  1, 	16, false, 0, true,  true,0,"16",	true, 0x0000ffff,0x0000ffff},
{ (unsigned int) 2,	       0,  2, 	32, false, 0, true,  true,0,"32",	true, 0xffffffff,0xffffffff},
{ (unsigned int) 3,	       0,  3, 	64, false, 0, true,  true,0,"64",       true, 0xdeaddead,0xdeaddead},
{ (unsigned int) 4,	       0,  0, 	8,  true,  0, false, true,0,"DISP8",    true, 0x000000ff,0x000000ff},
{ (unsigned int) 5,	       0,  1, 	16, true,  0, false, true,0,"DISP16",   true, 0x0000ffff,0x0000ffff},
{ (unsigned int) 6,	       0,  2, 	32, true,  0, false, true,0,"DISP32",   true, 0xffffffff,0xffffffff},
{ (unsigned int) 7,	       0,  3, 	64, true,  0, false, true,0,"DISP64",   true, 0xfeedface,0xfeedface},
};


bfd_error_vector_type bfd_error_vector;
/** a.out files */


PROTO (void , sunos4_write_syms, ());
PROTO (static boolean,sunos4_squirt_out_relocs,(bfd *abfd, asection *section));


static size_t
reloc_size_func(abfd)
bfd *abfd;
{
  switch (bfd_get_architecture (abfd)) {
  case bfd_arch_sparc:
  case bfd_arch_a29k:
    return  RELOC_EXT_SIZE;
  default:
    return  RELOC_STD_SIZE;
  }
}

static void
DEFUN(bfd_aout_swap_exec_header_in,(abfd, raw_bytes, execp),
      bfd *abfd AND
      unsigned char *raw_bytes AND
      struct exec *execp)
{
  struct exec_bytes *bytes = (struct exec_bytes *)raw_bytes;

  /* Now fill in fields in the execp, from the bytes in the raw data.  */
  execp->a_info   = bfd_h_getlong (abfd, bytes->a_info);
  execp->a_text   = bfd_h_getlong (abfd, bytes->a_text);
  execp->a_data   = bfd_h_getlong (abfd, bytes->a_data);
  execp->a_bss    = bfd_h_getlong (abfd, bytes->a_bss);
  execp->a_syms   = bfd_h_getlong (abfd, bytes->a_syms);
  execp->a_entry  = bfd_h_getlong (abfd, bytes->a_entry);
  execp->a_trsize = bfd_h_getlong (abfd, bytes->a_trsize);
  execp->a_drsize = bfd_h_getlong (abfd, bytes->a_drsize);
}

static void
DEFUN(bfd_aout_swap_exec_header_out,(abfd, execp, raw_bytes),
     bfd *abfd AND
     struct exec *execp AND 
     unsigned char *raw_bytes)
{
  struct exec_bytes *bytes = (struct exec_bytes *)raw_bytes;

  /* Now fill in fields in the raw data, from the fields in the exec struct. */
  bfd_h_putlong (abfd, execp->a_info  , bytes->a_info);
  bfd_h_putlong (abfd, execp->a_text  , bytes->a_text);
  bfd_h_putlong (abfd, execp->a_data  , bytes->a_data);
  bfd_h_putlong (abfd, execp->a_bss   , bytes->a_bss);
  bfd_h_putlong (abfd, execp->a_syms  , bytes->a_syms);
  bfd_h_putlong (abfd, execp->a_entry , bytes->a_entry);
  bfd_h_putlong (abfd, execp->a_trsize, bytes->a_trsize);
  bfd_h_putlong (abfd, execp->a_drsize, bytes->a_drsize);
}

/* Steve wants some way to frob this stuff from Saber while he's debugging
   ld, so we have these funny shadow functions */
/* ZMAGIC's start at 0 (making the exec part of the text section),
  other formats start after the exec
*/
static unsigned int n_txtoff(ptr)
struct exec *ptr;
{return N_MAGIC(*ptr)== ZMAGIC ? 0: sizeof(struct exec);}

static unsigned int n_datoff(ptr)
struct exec *ptr;
{return n_txtoff(ptr) + ptr->a_text;}

static unsigned int n_treloff(ptr)
struct exec *ptr;
{return n_datoff(ptr) + ptr->a_data;}

static unsigned int n_dreloff(ptr)
struct exec *ptr;
{return n_treloff(ptr) + ptr->a_trsize;}

static unsigned int n_symoff(ptr)
struct exec *ptr;
{return n_dreloff(ptr) + ptr->a_drsize;}

static unsigned int n_stroff(ptr)
struct exec *ptr;
{return n_symoff(ptr) + ptr->a_syms;}

static
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
  unsigned char magicbuf[4];	/* Raw bytes of magic number from file */
  unsigned long magic;		/* Swapped magic number */
  unsigned char exec_bytes[EXEC_BYTES_SIZE];	/* Raw bytes of exec hdr */
  struct exec *execp;
  PTR rawptr;

  bfd_error = system_call_error;

  if (bfd_read ((PTR)magicbuf, 1, sizeof (magicbuf), abfd) !=
      sizeof (magicbuf))
    return 0;
  magic = bfd_h_getlong (abfd, magicbuf);

  /* Baroque syntax to mask deficiencies of the Sun compiler */
  /* if (N_BADMAG (*((struct exec *) &magic))) return 0; */
  if (n_badmag ((struct exec *) &magic)) return 0;

  if (bfd_seek (abfd, 0L, false) < 0) return 0;

  if (bfd_read ((PTR) exec_bytes, 1, EXEC_BYTES_SIZE, abfd)
      != EXEC_BYTES_SIZE) {
    bfd_error = wrong_format;
    return 0;
  }

  /* Use an intermediate variable for clarity */
  rawptr = (PTR) bfd_zalloc (abfd, sizeof (struct sunexdata) + sizeof (struct exec));

  if (rawptr == NULL) {
    bfd_error = no_memory;
    return 0;
  }

  set_tdata (abfd, ((struct sunexdata *) rawptr));
  exec_hdr (abfd) = execp =
    (struct exec *) ((char *)rawptr + sizeof (struct sunexdata));

  bfd_aout_swap_exec_header_in (abfd, exec_bytes, execp);

  /* Set the file flags */
  abfd->flags = NO_FLAGS;
  if (execp->a_drsize || execp->a_trsize)
    abfd->flags |= HAS_RELOC;
  if (execp->a_entry) 
    abfd->flags |= EXEC_P;
  if (execp->a_syms) 
    abfd->flags |= HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS;


  if (N_MAGIC (*execp) == ZMAGIC) abfd->flags |= D_PAGED;
  if (N_MAGIC (*execp) == NMAGIC) abfd->flags |= WP_TEXT;

  /* Determine the architecture and machine type of the object file.  */
  abfd->obj_arch = bfd_arch_unknown;	/* Default values */
  abfd->obj_machine = 0;
  switch (N_MACHTYPE (*execp)) {

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
  obj_str_filepos (abfd) = n_stroff (execp);
  obj_sym_filepos (abfd) = n_symoff (execp);

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
  obj_datasec (abfd)->vma = N_DATADDR(*execp);
  obj_bsssec (abfd)->vma = N_BSSADDR(*execp);
  obj_textsec (abfd)->vma = N_TXTADDR(*execp);

  obj_textsec (abfd)->filepos = N_TXTOFF(*execp);
  obj_datasec (abfd)->filepos = N_DATOFF(*execp);

  obj_textsec (abfd)->rel_filepos = N_TROFF(*execp);
  obj_datasec (abfd)->rel_filepos = N_DROFF(*execp);

  obj_textsec (abfd)->flags = (execp->a_trsize != 0 ?
                               (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_HAS_CONTENTS) :
                               (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS));
  obj_datasec (abfd)->flags = (execp->a_drsize != 0 ?
                               (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_HAS_CONTENTS) :
                               (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS));
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
  rawptr =  bfd_zalloc (abfd,sizeof (struct sunexdata) + sizeof (struct exec));

  if (rawptr == NULL) {
    bfd_error = no_memory;
    return false;
  }

  abfd->tdata = (PTR)((struct sunexdata *) rawptr);
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
    case 0:		arch_flags = M_68010; break;
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
  size_t data_pad = 0;
  unsigned char exec_bytes[EXEC_BYTES_SIZE];
  struct exec *execp = exec_hdr (abfd);

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
		     reloc_size_func(abfd));
		       
  execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *
		     reloc_size_func(abfd));

  bfd_aout_swap_exec_header_out (abfd, execp, exec_bytes);

  bfd_seek (abfd, 0L, false);
  bfd_write ((PTR) exec_bytes, 1, EXEC_BYTES_SIZE, abfd);

  /* Now write out reloc info, followed by syms and strings */

  if (bfd_get_symcount (abfd) != 0) 
    {
      bfd_seek (abfd,
		(long)(N_SYMOFF(*execp)), false);

      sunos4_write_syms (abfd);

      bfd_seek (abfd,	(long)(N_TROFF(*execp)), false);

      if (!sunos4_squirt_out_relocs (abfd, obj_textsec (abfd))) return false;
      bfd_seek (abfd, (long)(N_DROFF(*execp)), false);

      if (!sunos4_squirt_out_relocs (abfd, obj_datasec (abfd))) return false;
    }
  return true;
}

/** core files */
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

/* FIXME:  It's worse than we expect.  This struct contains TWO substructs
   neither of whose size we know, WITH STUFF IN BETWEEN THEM!  We can't
   even portably access the stuff in between!  */

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

bfd_target *
sunos4_core_file_p (abfd)
     bfd *abfd;
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
  core_mag = bfd_h_getlong (abfd, longbuf);

  if (core_mag != CORE_MAGIC) return 0;

  /* SunOS core headers can vary in length; second word is size; */
  if (bfd_read ((PTR)longbuf, 1, sizeof (longbuf), abfd) !=
	 sizeof (longbuf))
    return 0;
  core_size = bfd_h_getlong (abfd, longbuf);
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
    free ((PTR)rawptr);
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
    free ((PTR)rawptr);
    return 0;
  }
  core_datasec (abfd) = (asection *) bfd_zalloc (abfd, sizeof (asection));
  if (core_datasec (abfd) == NULL) {
loser1:
    free ((PTR)core_stacksec (abfd));
    goto loser;
  }
  core_regsec (abfd) = (asection *) bfd_zalloc (abfd, sizeof (asection));
  if (core_regsec (abfd) == NULL) {
loser2:
    free ((PTR)core_datasec (abfd));
    goto loser1;
  }
  core_reg2sec (abfd) = (asection *) bfd_zalloc (abfd, sizeof (asection));
  if (core_reg2sec (abfd) == NULL) {
    free ((PTR)core_regsec (abfd));
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

  return (bcmp ((char *)&core_hdr (core_bfd), (char*) &exec_hdr (exec_bfd),
                sizeof (struct exec)) == 0) ? true : false;
}

/* byte-swap core structure */
/* FIXME, this needs more work to swap IN a core struct from raw bytes */
static void
swapcore (abfd, core)
     bfd *abfd;
     struct core *core;
{
  unsigned char exec_bytes[EXEC_BYTES_SIZE];

  core->c_magic = bfd_h_getlong (abfd, (unsigned char *)&core->c_magic);
  core->c_len   = bfd_h_getlong (abfd, (unsigned char *)&core->c_len  );
  /* Leave integer registers in target byte order.  */
  bcopy ((char *)&(core->c_aouthdr), (char *)exec_bytes, EXEC_BYTES_SIZE);
  bfd_aout_swap_exec_header_in (abfd, exec_bytes, &core->c_aouthdr);
  core->c_signo = bfd_h_getlong (abfd, (unsigned char *)&core->c_signo);
  core->c_tsize = bfd_h_getlong (abfd, (unsigned char *)&core->c_tsize);
  core->c_dsize = bfd_h_getlong (abfd, (unsigned char *)&core->c_dsize);
  core->c_ssize = bfd_h_getlong (abfd, (unsigned char *)&core->c_ssize);
  /* Leave FP registers in target byte order.  */
  /* Leave "c_ucode" unswapped for now, since we can't find it easily.  */
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
  if (section != obj_bsssec(abfd)) {
    bfd_seek (abfd, section->filepos + offset, SEEK_SET);

    if (count) {
      return (bfd_write ((PTR)location, 1, count, abfd) == count) ?
	true : false;
    }
    return false;
  }
  return true;
}
boolean
sunos4_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
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
      arelent_chain *reloc = (arelent_chain *)bfd_alloc(abfd, sizeof(arelent_chain));

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

      reloc->relent.howto = howto_table_ext +CTOR_TABLE_RELOC_IDX;
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
     PTR cache_ptr_g;
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

      bfd_error_vector.nonrepresentable_section(abfd, bfd_get_output_section(cache_ptr)->name);

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
    (aout_symbol_type *)bfd_zalloc (abfd, sizeof (aout_symbol_type));
  new->symbol.the_bfd = abfd;

  return &new->symbol;
}

boolean
DEFUN(sunos4_slurp_symbol_table, (abfd),
      bfd *abfd)
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
  if (bfd_read ((PTR)&string_size, 4, 1, abfd) != 4)
    return false;
  string_size = bfd_h_getlong (abfd, (unsigned char *)&string_size);

  symbol_count = symbol_size / sizeof (struct nlist);

  strings = bfd_alloc(abfd, string_size + 1);
  cached = bfd_zalloc(abfd, symbol_count * sizeof(aout_symbol_type));
  syms = bfd_alloc(abfd, symbol_size);

  bfd_seek (abfd, obj_sym_filepos (abfd), SEEK_SET);
  if (bfd_read ((PTR)syms, 1, symbol_size, abfd) != symbol_size) {
  bailout:
    return false;
  }

  bfd_seek (abfd, obj_str_filepos (abfd), SEEK_SET);
  if (bfd_read ((PTR)strings, 1, string_size, abfd) != string_size) {
    goto bailout;
  }

  /* OK, now walk the new symtable, cacheing symbol properties */
    {
      register struct nlist *sym_pointer;
      register struct nlist *sym_end = syms + symbol_count;
      register aout_symbol_type *cache_ptr = cached;

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
  bfd_release (abfd, (PTR)syms);

  return true;
}


void
DEFUN(sunos4_write_syms,(abfd),
     bfd *abfd)
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
    translate_to_native_sym_flags (&nsp, (PTR)g, abfd);


    bfd_h_putshort (abfd, nsp.n_desc, (unsigned char *)&nsp.n_desc);
    bfd_h_putlong  (abfd, nsp.n_value, (unsigned char *)&nsp.n_value);
    bfd_write((PTR)&nsp,1, sizeof(nsp), abfd);
  }


  /* Now output the strings.  Be sure to put string length into correct
   * byte ordering before writing it.
   */
  bfd_h_putlong  (abfd, stindex, (unsigned char *)&stindex);

  bfd_write((PTR)&stindex, 1, sizeof(stindex), abfd);
  
  generic = bfd_get_outsymbols(abfd);
  for (count = 0; count < bfd_get_symcount(abfd); count++) 
    {
      asymbol *g = *(generic++);

      if (g->name != (char *)NULL) 
	{
	  size_t length = strlen(g->name)+1;
	  bfd_write((PTR)g->name, 1, length, abfd);
	}
      if ((g->flags & BSF_FAKE)==0) {
	g->name = itos(count);	/* smash the generic symbol */
      }
    }
}


void
DEFUN(sunos4_reclaim_symbol_table,(abfd),
     bfd *abfd)
{

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


/* Standard reloc stuff */
/* Output standard relocation information to a file in target byte order. */

void
swap_std_reloc_out (abfd, p, natptr, count)
     bfd *abfd;
     arelent **p;		/* Generic relocation struct */
     struct reloc_std_bytes *natptr;
     unsigned int count;
{
  int r_index;
  int r_extern;
  unsigned int r_length;
  int r_pcrel;
  int r_baserel, r_jmptable, r_relative;
  unsigned int r_addend;
  unsigned int idx;
  for (idx = 0; idx < count; idx++, p++, natptr++) 
    {
      arelent *g = *p;
      bfd_h_putlong (abfd, g->address, natptr->r_address);

      r_length = g->howto->size; /* Size as a power of two */
      r_pcrel  = (int) g->howto->pc_relative;	/* Relative to PC? */
      /* r_baserel, r_jmptable, r_relative???  FIXME-soon */
      r_baserel = 0;
      r_jmptable = 0;
      r_relative = 0;

      r_addend = g->addend;	/* Start here, see how it goes */

      /* name was clobbered by sunos4_write_syms to be symbol index */

      if (g->sym_ptr_ptr != NULL) 
	{
	  if ((*(g->sym_ptr_ptr))->section) {
	    /* put the section offset into the addend for output */
	    r_addend += (*(g->sym_ptr_ptr))->section->vma;
	  }

	  r_index = stoi((*(g->sym_ptr_ptr))->name);
	  r_extern = 1;
	}
      else {
	r_extern = 0;
	if (g->section == NULL) {
	  BFD_ASSERT(0);
	  r_index = N_ABS | N_EXT;
	}
	else  if(g->section->output_section == obj_textsec(abfd)) {
	  r_index = N_TEXT | N_EXT;
	  r_addend += g->section->output_section->vma;
	}
	else if (g->section->output_section == obj_datasec(abfd)) {
	  r_index = N_DATA | N_EXT;
	  r_addend += g->section->output_section->vma;
	}
	else if (g->section->output_section == obj_bsssec(abfd)) {
	  r_index = N_BSS | N_EXT ;
	  r_addend += g->section->output_section->vma;
	}
	else {
	  BFD_ASSERT(0);
	}
      }

      /* now the fun stuff */
      if (abfd->xvec->header_byteorder_big_p != false) {
	natptr->r_index[0] = r_index >> 16;
	natptr->r_index[1] = r_index >> 8;
	natptr->r_index[2] = r_index;
	natptr->r_bits[0] =
	  (r_extern?    RELOC_STD_BITS_EXTERN_BIG: 0)
	    | (r_pcrel?     RELOC_STD_BITS_PCREL_BIG: 0)
	      | (r_baserel?   RELOC_STD_BITS_BASEREL_BIG: 0)
		| (r_jmptable?  RELOC_STD_BITS_JMPTABLE_BIG: 0)
		  | (r_relative?  RELOC_STD_BITS_RELATIVE_BIG: 0)
		    | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG);
      } else {
	natptr->r_index[2] = r_index >> 16;
	natptr->r_index[1] = r_index >> 8;
	natptr->r_index[0] = r_index;
	natptr->r_bits[0] =
	  (r_extern?    RELOC_STD_BITS_EXTERN_LITTLE: 0)
	    | (r_pcrel?     RELOC_STD_BITS_PCREL_LITTLE: 0)
	      | (r_baserel?   RELOC_STD_BITS_BASEREL_LITTLE: 0)
		| (r_jmptable?  RELOC_STD_BITS_JMPTABLE_LITTLE: 0)
		  | (r_relative?  RELOC_STD_BITS_RELATIVE_LITTLE: 0)
		    | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE);
      }


    }
}


/* Extended stuff */
/* Output extended relocation information to a file in target byte order. */

void
swap_ext_reloc_out (abfd, p, natptr, count)
     bfd *abfd;
     arelent **p;		/* Generic relocation struct */
     register struct reloc_ext_bytes *natptr;
     unsigned int count;
{

  int r_index;
  int r_extern;
  unsigned int r_type;
  unsigned int r_addend;
  unsigned int idx;
  for (idx = 0; idx < count; idx++, p++, natptr++) {
    arelent *g = *p;

    bfd_h_putlong (abfd, g->address, natptr->r_address);

    /* Find a type in the output format which matches the input howto - 
       at the moment we assume input format == output format FIXME!! */
    r_type = (enum reloc_type) g->howto->type;

    r_addend = g->addend;	/* Start here, see how it goes */

    /* name was clobbered by sunos4_write_syms to be symbol index*/

    if (g->sym_ptr_ptr != NULL) 
      {
	if ((*(g->sym_ptr_ptr))->section) {
	  /* put the section offset into the addend for output */
	  r_addend += (*(g->sym_ptr_ptr))->section->vma;
	}

	r_index = stoi((*(g->sym_ptr_ptr))->name);
	r_extern = 1;
      }
    else {
      r_extern = 0;
      if (g->section == NULL) {
	BFD_ASSERT(0);
	r_index = N_ABS | N_EXT;
      }
      else  if(g->section->output_section == obj_textsec(abfd)) {
	r_index = N_TEXT | N_EXT;
	r_addend += g->section->output_section->vma;
      }
      else if (g->section->output_section == obj_datasec(abfd)) {
	r_index = N_DATA | N_EXT;
	r_addend += g->section->output_section->vma;
      }
      else if (g->section->output_section == obj_bsssec(abfd)) {
	r_index = N_BSS | N_EXT ;
	r_addend += g->section->output_section->vma;
      }
      else {
	BFD_ASSERT(0);
      }
    }

    /* now the fun stuff */
    if (abfd->xvec->header_byteorder_big_p != false) {
      natptr->r_index[0] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[2] = r_index;
      natptr->r_bits[0] =
	(r_extern? RELOC_EXT_BITS_EXTERN_BIG: 0)
	  || (r_type << RELOC_EXT_BITS_TYPE_SH_BIG);
    } else {
      natptr->r_index[2] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[0] = r_index;
      natptr->r_bits[0] =
	(r_extern? RELOC_EXT_BITS_EXTERN_LITTLE: 0)
	  || (r_type << RELOC_EXT_BITS_TYPE_SH_LITTLE);
    }

    bfd_h_putlong (abfd, r_addend, natptr->r_addend);
  }
}
#define MOVE_ADDRESS(ad)       						\
  if (r_extern) {							\
    cache_ptr->sym_ptr_ptr = symbols + r_index;				\
    cache_ptr->section = (asection *)NULL;				\
      cache_ptr->addend = ad;						\
  } else {								\
    cache_ptr->sym_ptr_ptr = (asymbol **)NULL;				\
    switch (r_index) {							\
    case N_TEXT:							\
    case N_TEXT | N_EXT:						\
      cache_ptr->section = obj_textsec(abfd);				\
      cache_ptr->addend = ad  - su->textsec->vma;			\
      break;								\
    case N_DATA:							\
    case N_DATA | N_EXT:						\
      cache_ptr->section = obj_datasec(abfd);				\
      cache_ptr->addend = ad - su->datasec->vma;			\
      break;								\
    case N_BSS:								\
    case N_BSS | N_EXT:							\
      cache_ptr->section = obj_bsssec(abfd);				\
      cache_ptr->addend = ad - su->bsssec->vma;				\
      break;								\
    case N_ABS:								\
    case N_ABS | N_EXT:							\
      BFD_ASSERT(1);							\
      break;								\
    default:								\
      BFD_ASSERT(1);							\
      break;								\
    }									\
  }     								\

void
swap_ext_reloc_in (abfd, bytes, cache_ptr, symbols)
     bfd *abfd;
     struct reloc_ext_bytes *bytes;
     arelent *cache_ptr;
     asymbol **symbols;
{
  int r_index;
  int r_extern;
  unsigned int r_type;
  struct sunexdata *su = (struct sunexdata *)(abfd->tdata);

  cache_ptr->address = bfd_h_getlong (abfd, bytes->r_address);

  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
    r_index =  (bytes->r_index[0] << 16)
	     | (bytes->r_index[1] << 8)
	     |  bytes->r_index[2];
    r_extern = (0 != (bytes->r_bits[0] & RELOC_EXT_BITS_EXTERN_BIG));
    r_type   =       (bytes->r_bits[0] & RELOC_EXT_BITS_TYPE_BIG)
				      >> RELOC_EXT_BITS_TYPE_SH_BIG;
  } else {
    r_index =  (bytes->r_index[2] << 16)
	     | (bytes->r_index[1] << 8)
	     |  bytes->r_index[0];
    r_extern = (0 != (bytes->r_bits[0] & RELOC_EXT_BITS_EXTERN_LITTLE));
    r_type   =       (bytes->r_bits[0] & RELOC_EXT_BITS_TYPE_LITTLE)
				      >> RELOC_EXT_BITS_TYPE_SH_LITTLE;
  }

  cache_ptr->howto =  howto_table_ext + r_type;
  MOVE_ADDRESS(bfd_h_getlong(abfd,bytes->r_addend));
									 
}

void
swap_std_reloc_in (abfd, bytes, cache_ptr, symbols)
     bfd *abfd;
 struct reloc_std_bytes *bytes;
     arelent *cache_ptr;
     asymbol **symbols;
{
  int r_index;
  int r_extern;
  unsigned int r_length;
  int r_pcrel;
  int r_baserel, r_jmptable, r_relative;
  struct sunexdata *su = (struct sunexdata *)(abfd->tdata);
  cache_ptr->address = bfd_h_getlong (abfd, bytes->r_address);

  /* now the fun stuff */
  if (abfd->xvec->header_byteorder_big_p != false) {
    r_index =  (bytes->r_index[0] << 16)
      | (bytes->r_index[1] << 8)
	|  bytes->r_index[2];
    r_extern  = (0 != (bytes->r_bits[0] & RELOC_STD_BITS_EXTERN_BIG));
    r_pcrel   = (0 != (bytes->r_bits[0] & RELOC_STD_BITS_PCREL_BIG));
    r_baserel = (0 != (bytes->r_bits[0] & RELOC_STD_BITS_BASEREL_BIG));
    r_jmptable= (0 != (bytes->r_bits[0] & RELOC_STD_BITS_JMPTABLE_BIG));
    r_relative= (0 != (bytes->r_bits[0] & RELOC_STD_BITS_RELATIVE_BIG));
    r_length  =       (bytes->r_bits[0] & RELOC_STD_BITS_LENGTH_BIG) 
      >> RELOC_STD_BITS_LENGTH_SH_BIG;
  } else {
    r_index =  (bytes->r_index[2] << 16)
      | (bytes->r_index[1] << 8)
	|  bytes->r_index[0];
    r_extern  = (0 != (bytes->r_bits[0] & RELOC_STD_BITS_EXTERN_LITTLE));
    r_pcrel   = (0 != (bytes->r_bits[0] & RELOC_STD_BITS_PCREL_LITTLE));
    r_baserel = (0 != (bytes->r_bits[0] & RELOC_STD_BITS_BASEREL_LITTLE));
    r_jmptable= (0 != (bytes->r_bits[0] & RELOC_STD_BITS_JMPTABLE_LITTLE));
    r_relative= (0 != (bytes->r_bits[0] & RELOC_STD_BITS_RELATIVE_LITTLE));
    r_length  =       (bytes->r_bits[0] & RELOC_STD_BITS_LENGTH_LITTLE) 
      >> RELOC_STD_BITS_LENGTH_SH_LITTLE;
  }

  cache_ptr->howto =  howto_table_std + r_length + 4 * r_pcrel;
  /* FIXME-soon:  Roll baserel, jmptable, relative bits into howto setting */

  MOVE_ADDRESS(0);
}

/* Reloc hackery */

boolean
sunos4_slurp_reloc_table (abfd, asect, symbols)
     bfd *abfd;
     sec_ptr asect;
     asymbol **symbols;
{
  unsigned int count;
  size_t reloc_size;
  PTR relocs;
  arelent *reloc_cache;
  size_t each_size;

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
  each_size = reloc_size_func(abfd);

  count = reloc_size / each_size;


  reloc_cache = (arelent *) bfd_zalloc (abfd, (size_t)(count * sizeof
						       (arelent)));
  relocs =  bfd_alloc (abfd, reloc_size);

  if (bfd_read ( relocs, 1, reloc_size, abfd) != reloc_size) {
    bfd_error = system_call_error;
    return false;
  }

  if (each_size == RELOC_EXT_SIZE)
    {
      register struct reloc_ext_bytes *rptr = (struct reloc_ext_bytes *) relocs;
      unsigned int counter = 0;
      arelent *cache_ptr = reloc_cache;

      for (; counter < count; counter++, rptr++, cache_ptr++) {
	swap_ext_reloc_in(abfd, rptr, cache_ptr, symbols);
      }
    }
  else {
    register struct reloc_std_bytes *rptr = (struct reloc_std_bytes *) relocs;
    unsigned int counter = 0;
    arelent *cache_ptr = reloc_cache;

    for (; counter < count; counter++, rptr++, cache_ptr++) {
	swap_std_reloc_in(abfd, rptr, cache_ptr, symbols);
    }

  }
bfd_release (abfd,relocs);
  asect->relocation = reloc_cache;
  asect->reloc_count = count;
  return true;
}



/* Write out a relocation section into an object file.  */

static boolean
sunos4_squirt_out_relocs (abfd, section)
     bfd *abfd;
     asection *section;
{
  arelent **generic;
  unsigned char *native;
  size_t each_size;

  unsigned int count = section->reloc_count;
  size_t natsize;

  if (count == 0) return true;

  each_size = reloc_size_func(abfd);
  natsize = each_size * count;
  native = (unsigned char *) bfd_zalloc (abfd, natsize);
  if (!native) {
    bfd_error = no_memory;
    return false;
  }

  generic = section->orelocation;

  if (each_size == RELOC_EXT_SIZE) 
    {
      swap_ext_reloc_out (abfd,
			  generic,
			  (struct reloc_ext_bytes *)native,
			  count);
    }
  else 
    {
      swap_std_reloc_out(abfd, generic, native, count);
    }

  if ( bfd_write ((PTR) native, 1, natsize, abfd) != natsize) {
    bfd_release(abfd, native);
    return false;
  }
  bfd_release (abfd, native);

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
            ((exec_hdr(abfd)->a_drsize / reloc_size_func(abfd))
             +1));

  if (asect == obj_textsec (abfd))
    return (sizeof (arelent *) *
            ((exec_hdr(abfd)->a_trsize / reloc_size_func(abfd))
             +1));

  bfd_error = invalid_operation;
  return 0;
}

void
sunos4_reclaim_reloc (ignore_abfd, section)
     bfd *ignore_abfd;
     sec_ptr section;
{

}


alent *
sunos4_get_lineno(ignore_abfd, ignore_symbol)
bfd *ignore_abfd;
PTR ignore_symbol;
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
   CONST char *section_name = symbol->section == (asection *)NULL ?
	"*abs" : symbol->section->name;

      bfd_print_symbol_vandf((PTR)file,symbol);

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

  return true;
}

/* 
 provided a bfd, a section and an offset into the section, calculate
 and return the name of the source file and the line nearest to the
 wanted location.
*/
 
boolean
DEFUN(sunos4_find_nearest_line,(abfd,
				section,
				symbols,
				offset,
				filename_ptr,
				functionname_ptr,
				line_ptr),
      bfd *abfd AND
      asection *section AND
      asymbol **symbols AND
      bfd_vma offset AND
      CONST char **filename_ptr AND
      CONST char **functionname_ptr AND
      unsigned int *line_ptr)
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
	    CONST char *function = func->name;
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

#define sunos4_openr_next_archived_file bfd_generic_openr_next_archived_file
#define sunos4_generic_stat_arch_elt bfd_generic_stat_arch_elt
#define sunos4_slurp_armap bfd_slurp_bsd_armap
#define sunos4_slurp_extended_name_table bfd_true
#define sunos4_write_armap bsd_write_armap
#define sunos4_truncate_arname bfd_bsd_truncate_arname
bfd_target aout_big_vec =
{
  "a.out-generic-big",		/* name */
  bfd_target_aout_flavour_enum,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* data */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

    {_bfd_dummy_target, sunos4_object_p,
       bfd_generic_archive_p, sunos4_core_file_p},
    {bfd_false, sunos4_mkobject,
       _bfd_generic_mkarchive, bfd_false},

  JUMP_TABLE(sunos4)
  };


bfd_target aout_little_vec =
{
  "a.out-generic-little",		/* name */
  bfd_target_aout_flavour_enum,
  false,			/* target byte order */
  false,			/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* data */
  _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* hdrs */


    {_bfd_dummy_target, sunos4_object_p,
       bfd_generic_archive_p, sunos4_core_file_p},
    {bfd_false, sunos4_mkobject,
       _bfd_generic_mkarchive, bfd_false},

  JUMP_TABLE(sunos4)
  };

