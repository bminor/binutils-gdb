/* BFD back-end for MIPS Extended-Coff files.
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.
   Original version by Per Bothner.
   Full support added by Ian Lance Taylor, ian@cygnus.com.

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "seclet.h"
#include "coff/internal.h"
#include "coff/sym.h"
#include "coff/symconst.h"
#include "coff/ecoff.h"
#include "coff/mips.h"
#include "libcoff.h"
#include "libecoff.h"

/* Prototypes for static functions.  */

static boolean mips_ecoff_bad_format_hook PARAMS ((bfd *abfd, PTR filehdr));
static PTR mips_ecoff_mkobject_hook PARAMS ((bfd *abfd, PTR filehdr,
					     PTR aouthdr));
static void mips_ecoff_swap_reloc_in PARAMS ((bfd *, PTR,
					      struct internal_reloc *));
static void mips_ecoff_swap_reloc_out PARAMS ((bfd *,
					       const struct internal_reloc *,
					       PTR));


/* ECOFF has COFF sections, but the debugging information is stored in
   a completely different format.  ECOFF targets use some of the
   swapping routines from coffswap.h, and some of the generic COFF
   routines in coffgen.c, but, unlike the real COFF targets, do not
   use coffcode.h itself.

   Get the generic COFF swapping routines, except for the reloc,
   symbol, and lineno ones.  Give them ecoff names.  */
#define MIPSECOFF
#define NO_COFF_RELOCS
#define NO_COFF_SYMBOLS
#define NO_COFF_LINENOS
#define coff_swap_filehdr_in mips_ecoff_swap_filehdr_in
#define coff_swap_filehdr_out mips_ecoff_swap_filehdr_out
#define coff_swap_aouthdr_in mips_ecoff_swap_aouthdr_in
#define coff_swap_aouthdr_out mips_ecoff_swap_aouthdr_out
#define coff_swap_scnhdr_in mips_ecoff_swap_scnhdr_in
#define coff_swap_scnhdr_out mips_ecoff_swap_scnhdr_out
#include "coffswap.h"

/* Get the ECOFF swapping routines.  */
#define ECOFF_32
#include "ecoffswap.h"

/* This is the ECOFF backend structure.  The backend_data field of the
   ecoff_tdata structure is set to this when an ECOFF BFD is
   initialized.  This is used by the generic ECOFF routines.  */

static const struct ecoff_backend_data mips_ecoff_backend_data =
{
  /* Supported architecture.  */
  bfd_arch_mips,
  /* Big endian magic number.  */
  MIPS_MAGIC_BIG,
  /* Little endian magic number.  */
  MIPS_MAGIC_LITTLE,
  /* Alignment of debugging information.  E.g., 4.  */
  4,
  /* The page boundary used to align sections in a demand-paged
     executable file.  E.g., 0x1000.  */
  0x1000,
  /* Bitsize of constructor entries.  */
  32,
  /* Sizes of external symbolic information.  */
  sizeof (struct hdr_ext),
  sizeof (struct dnr_ext),
  sizeof (struct pdr_ext),
  sizeof (struct sym_ext),
  sizeof (struct opt_ext),
  sizeof (struct fdr_ext),
  sizeof (struct rfd_ext),
  sizeof (struct ext_ext),
  /* Functions to swap in external symbolic data.  */
  ecoff_swap_hdr_in,
  ecoff_swap_dnr_in,
  ecoff_swap_pdr_in,
  ecoff_swap_sym_in,
  ecoff_swap_opt_in,
  ecoff_swap_fdr_in,
  ecoff_swap_rfd_in,
  ecoff_swap_ext_in,
  /* Functions to swap out external symbolic data.  */
  ecoff_swap_hdr_out,
  ecoff_swap_dnr_out,
  ecoff_swap_pdr_out,
  ecoff_swap_sym_out,
  ecoff_swap_opt_out,
  ecoff_swap_fdr_out,
  ecoff_swap_rfd_out,
  ecoff_swap_ext_out,
  /* External reloc size.  */
  RELSZ,
  /* Reloc swapping functions.  */
  mips_ecoff_swap_reloc_in,
  mips_ecoff_swap_reloc_out
};

/* See whether the magic number matches.  */

static boolean
mips_ecoff_bad_format_hook (abfd, filehdr)
     bfd *abfd;
     PTR filehdr;
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;

  if (MIPS_ECOFF_BADMAG (*internal_f))
    return false;

  return true;
}

/* Create an ECOFF object.  */

static boolean
mips_ecoff_mkobject (abfd)
     bfd *abfd;
{
  abfd->tdata.ecoff_obj_data = ((struct ecoff_tdata *)
				bfd_zalloc (abfd, sizeof (ecoff_data_type)));
  if (abfd->tdata.ecoff_obj_data == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  ecoff_data (abfd)->backend_data = &mips_ecoff_backend_data;

  /* Always create a .scommon section for every BFD.  This is a hack so
     that the linker has something to attach scSCommon symbols to.  */
  bfd_make_section (abfd, SCOMMON);

  return true;
}

/* Create the MIPS ECOFF backend specific information.  */

static PTR
mips_ecoff_mkobject_hook (abfd, filehdr, aouthdr)
     bfd *abfd;
     PTR filehdr;
     PTR aouthdr;
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;
  struct internal_aouthdr *internal_a = (struct internal_aouthdr *) aouthdr;
  ecoff_data_type *ecoff;

  if (mips_ecoff_mkobject (abfd) == false)
    return NULL;

  ecoff = ecoff_data (abfd);
  ecoff->gp_size = 8;
  ecoff->sym_filepos = internal_f->f_symptr;

  if (internal_a != (struct internal_aouthdr *) NULL)
    {
      int i;

      ecoff->text_start = internal_a->text_start;
      ecoff->text_end = internal_a->text_start + internal_a->tsize;
      ecoff->gp = internal_a->gp_value;
      ecoff->gprmask = internal_a->gprmask;
      for (i = 0; i < 4; i++)
	ecoff->cprmask[i] = internal_a->cprmask[i];
      if (internal_a->magic == ECOFF_AOUT_ZMAGIC)
	abfd->flags |= D_PAGED;
    }

  return (PTR) ecoff;
}

/* Reloc handling.  MIPS ECOFF relocs are packed into 8 bytes in
   external form.  They use a bit which indicates whether the symbol
   is external.  */

/* Swap a reloc in.  */

static void
mips_ecoff_swap_reloc_in (abfd, ext_ptr, intern)
     bfd *abfd;
     PTR ext_ptr;
     struct internal_reloc *intern;
{
  const RELOC *ext = (RELOC *) ext_ptr;

  intern->r_vaddr = bfd_h_get_32 (abfd, (bfd_byte *) ext->r_vaddr);
  if (abfd->xvec->header_byteorder_big_p != false)
    {
      intern->r_symndx = (((int) ext->r_bits[0]
			   << RELOC_BITS0_SYMNDX_SH_LEFT_BIG)
			  | ((int) ext->r_bits[1]
			     << RELOC_BITS1_SYMNDX_SH_LEFT_BIG)
			  | ((int) ext->r_bits[2]
			     << RELOC_BITS2_SYMNDX_SH_LEFT_BIG));
      intern->r_type = ((ext->r_bits[3] & RELOC_BITS3_TYPE_BIG)
			>> RELOC_BITS3_TYPE_SH_BIG);
      intern->r_extern = (ext->r_bits[3] & RELOC_BITS3_EXTERN_BIG) != 0;
    }
  else
    {
      intern->r_symndx = (((int) ext->r_bits[0]
			   << RELOC_BITS0_SYMNDX_SH_LEFT_LITTLE)
			  | ((int) ext->r_bits[1]
			     << RELOC_BITS1_SYMNDX_SH_LEFT_LITTLE)
			  | ((int) ext->r_bits[2]
			     << RELOC_BITS2_SYMNDX_SH_LEFT_LITTLE));
      intern->r_type = ((ext->r_bits[3] & RELOC_BITS3_TYPE_LITTLE)
			>> RELOC_BITS3_TYPE_SH_LITTLE);
      intern->r_extern = (ext->r_bits[3] & RELOC_BITS3_EXTERN_LITTLE) != 0;
    }
}

/* Swap a reloc out.  */

static void
mips_ecoff_swap_reloc_out (abfd, intern, dst)
     bfd *abfd;
     const struct internal_reloc *intern;
     PTR dst;
{
  RELOC *ext = (RELOC *) dst;

  bfd_h_put_32 (abfd, intern->r_vaddr, (bfd_byte *) ext->r_vaddr);
  if (abfd->xvec->header_byteorder_big_p != false)
    {
      ext->r_bits[0] = intern->r_symndx >> RELOC_BITS0_SYMNDX_SH_LEFT_BIG;
      ext->r_bits[1] = intern->r_symndx >> RELOC_BITS1_SYMNDX_SH_LEFT_BIG;
      ext->r_bits[2] = intern->r_symndx >> RELOC_BITS2_SYMNDX_SH_LEFT_BIG;
      ext->r_bits[3] = (((intern->r_type << RELOC_BITS3_TYPE_SH_BIG)
			 & RELOC_BITS3_TYPE_BIG)
			| (intern->r_extern ? RELOC_BITS3_EXTERN_BIG : 0));
    }
  else
    {
      ext->r_bits[0] = intern->r_symndx >> RELOC_BITS0_SYMNDX_SH_LEFT_LITTLE;
      ext->r_bits[1] = intern->r_symndx >> RELOC_BITS1_SYMNDX_SH_LEFT_LITTLE;
      ext->r_bits[2] = intern->r_symndx >> RELOC_BITS2_SYMNDX_SH_LEFT_LITTLE;
      ext->r_bits[3] = (((intern->r_type << RELOC_BITS3_TYPE_SH_LITTLE)
			 & RELOC_BITS3_TYPE_LITTLE)
			| (intern->r_extern ? RELOC_BITS3_EXTERN_LITTLE : 0));
    }
}

#ifdef HOST_IRIX4

#include <core.out.h>

struct sgi_core_struct 
{
  int sig;
  char cmd[CORE_NAMESIZE];
};

#define core_hdr(bfd) ((bfd)->tdata.sgi_core_data)
#define core_signal(bfd) (core_hdr(bfd)->sig)
#define core_command(bfd) (core_hdr(bfd)->cmd)

static asection *
make_bfd_asection (abfd, name, flags, _raw_size, vma, filepos)
     bfd *abfd;
     CONST char *name;
     flagword flags;
     bfd_size_type _raw_size;
     bfd_vma vma;
     file_ptr filepos;
{
  asection *asect;

  asect = bfd_make_section (abfd, name);
  if (!asect)
    return NULL;

  asect->flags = flags;
  asect->_raw_size = _raw_size;
  asect->vma = vma;
  asect->filepos = filepos;
  asect->alignment_power = 4;

  return asect;
}

static bfd_target *
ecoff_core_file_p (abfd)
     bfd *abfd;
{
  int val;
  int i;
  char *secname;
  struct coreout coreout;
  struct idesc *idg, *idf, *ids;

  val = bfd_read ((PTR)&coreout, 1, sizeof coreout, abfd);
  if (val != sizeof coreout)
    return 0;

  if (coreout.c_magic != CORE_MAGIC
      || coreout.c_version != CORE_VERSION1)
    return 0;

  core_hdr (abfd) = (struct sgi_core_struct *) bfd_zalloc (abfd, sizeof (struct sgi_core_struct));
  if (!core_hdr (abfd))
    return NULL;

  strncpy (core_command (abfd), coreout.c_name, CORE_NAMESIZE);
  core_signal (abfd) = coreout.c_sigcause;

  bfd_seek (abfd, coreout.c_vmapoffset, SEEK_SET);

  for (i = 0; i < coreout.c_nvmap; i++)
    {
      struct vmap vmap;

      val = bfd_read ((PTR)&vmap, 1, sizeof vmap, abfd);
      if (val != sizeof vmap)
	break;

      switch (vmap.v_type)
	{
	case VDATA:
	  secname = ".data";
	  break;
	case VSTACK:
	  secname = ".stack";
	  break;
	default:
	  continue;
	}

      if (!make_bfd_asection (abfd, secname,
			      SEC_ALLOC+SEC_LOAD+SEC_HAS_CONTENTS,
			      vmap.v_len,
			      vmap.v_vaddr,
			      vmap.v_offset,
			      2))
	return NULL;
    }

  /* Make sure that the regs are contiguous within the core file. */

  idg = &coreout.c_idesc[I_GPREGS];
  idf = &coreout.c_idesc[I_FPREGS];
  ids = &coreout.c_idesc[I_SPECREGS];

  if (idg->i_offset + idg->i_len != idf->i_offset
      || idf->i_offset + idf->i_len != ids->i_offset)
    return 0;			/* Can't deal with non-contig regs */

  bfd_seek (abfd, idg->i_offset, SEEK_SET);

  make_bfd_asection (abfd, ".reg",
		     SEC_ALLOC+SEC_HAS_CONTENTS,
		     idg->i_len + idf->i_len + ids->i_len,
		     0,
		     idg->i_offset);

  /* OK, we believe you.  You're a core file (sure, sure).  */

  return abfd->xvec;
}

static char *
ecoff_core_file_failing_command (abfd)
     bfd *abfd;
{
  return core_command (abfd);
}

static int
ecoff_core_file_failing_signal (abfd)
     bfd *abfd;
{
  return core_signal (abfd);
}

static boolean
ecoff_core_file_matches_executable_p (core_bfd, exec_bfd)
     bfd *core_bfd, *exec_bfd;
{
  return true;			/* XXX - FIXME */
}
#else /* not def HOST_IRIX4 */
#define ecoff_core_file_p _bfd_dummy_target
#define ecoff_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define ecoff_core_file_failing_signal _bfd_dummy_core_file_failing_signal
#define ecoff_core_file_matches_executable_p \
  _bfd_dummy_core_file_matches_executable_p
#endif

/* This is the COFF backend structure.  The backend_data field of the
   bfd_target structure is set to this.  The section reading code in
   coffgen.c uses this structure.  */

static CONST bfd_coff_backend_data mips_ecoff_std_swap_table = {
  (void (*) PARAMS ((bfd *,PTR,int,int,PTR))) bfd_void, /* aux_in */
  (void (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* sym_in */
  (void (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* lineno_in */
  (unsigned (*) PARAMS ((bfd *,PTR,int,int,PTR))) bfd_void, /* aux_out */
  (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* sym_out */
  (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* lineno_out */
  (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* reloc_out */
  mips_ecoff_swap_filehdr_out, mips_ecoff_swap_aouthdr_out,
  mips_ecoff_swap_scnhdr_out,
  FILHSZ, AOUTSZ, SCNHSZ, 0, 0, 0, true,
  mips_ecoff_swap_filehdr_in, mips_ecoff_swap_aouthdr_in,
  mips_ecoff_swap_scnhdr_in, mips_ecoff_bad_format_hook,
  ecoff_set_arch_mach_hook, mips_ecoff_mkobject_hook,
  ecoff_styp_to_sec_flags, ecoff_make_section_hook, ecoff_set_alignment_hook,
  ecoff_slurp_symbol_table
};

bfd_target ecoff_little_vec =
{
  "ecoff-littlemips",		/* name */
  bfd_target_ecoff_flavour,
  false,			/* data byte order is little */
  false,			/* header byte order is little */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* sect
							    flags */
  0,				/* leading underscore */
  ' ',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  4,				/* minimum alignment power */
  _do_getl64, _do_getl_signed_64, _do_putl64,
     _do_getl32, _do_getl_signed_32, _do_putl32,
     _do_getl16, _do_getl_signed_16, _do_putl16, /* data */
  _do_getl64, _do_getl_signed_64, _do_putl64,
     _do_getl32, _do_getl_signed_32, _do_putl32,
     _do_getl16, _do_getl_signed_16, _do_putl16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     ecoff_archive_p, _bfd_dummy_target},
  {bfd_false, mips_ecoff_mkobject,  /* bfd_set_format */
     _bfd_generic_mkarchive, bfd_false},
  {bfd_false, ecoff_write_object_contents, /* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},
  JUMP_TABLE (ecoff),
  (PTR) &mips_ecoff_std_swap_table
};

bfd_target ecoff_big_vec =
{
  "ecoff-bigmips",		/* name */
  bfd_target_ecoff_flavour,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* sect flags */
  0,				/* leading underscore */
  ' ',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  4,				/* minimum alignment power */
  _do_getb64, _do_getb_signed_64, _do_putb64,
     _do_getb32, _do_getb_signed_32, _do_putb32,
     _do_getb16, _do_getb_signed_16, _do_putb16,
  _do_getb64, _do_getb_signed_64, _do_putb64,
     _do_getb32, _do_getb_signed_32, _do_putb32,
     _do_getb16, _do_getb_signed_16, _do_putb16,
 {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
    ecoff_archive_p, ecoff_core_file_p},
 {bfd_false, mips_ecoff_mkobject, /* bfd_set_format */
    _bfd_generic_mkarchive, bfd_false},
 {bfd_false, ecoff_write_object_contents, /* bfd_write_contents */
    _bfd_write_archive_contents, bfd_false},
  JUMP_TABLE(ecoff),
  (PTR) &mips_ecoff_std_swap_table
  /* Note that there is another bfd_target just above this one.  If
     you are adding initializers here, you should be adding them there
     as well.  */
};
