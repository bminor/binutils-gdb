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
#include "aout/ar.h"
#include "aout/ranlib.h"
#include "coff/mips.h"
#include "coff/internal.h"
#include "coff/sym.h"
#include "coff/symconst.h"
#include "coff/ecoff-ext.h"
#include "libcoff.h"
#include "libecoff.h"

/* Each canonical asymbol really looks like this.  */

typedef struct ecoff_symbol_struct
{
  /* The actual symbol which the rest of BFD works with */
  asymbol symbol;

  /* The fdr for this symbol.  */
  FDR *fdr;

  /* true if this is a local symbol rather than an external one.  */
  boolean local;

  /* A pointer to the unswapped hidden information for this symbol */
  union
    {
      struct sym_ext *lnative;
      struct ext_ext *enative;
    }
  native;
} ecoff_symbol_type;

/* We take the address of the first element of a asymbol to ensure that the
   macro is only ever applied to an asymbol.  */
#define ecoffsymbol(asymbol) ((ecoff_symbol_type *) (&((asymbol)->the_bfd)))

/* The page boundary used to align sections in the executable file.  */
#define ROUND_SIZE 0x1000

/* The linker needs a section to hold small common variables while
   linking.  There is no convenient way to create it when the linker
   needs it, so we always create one for each BFD.  We then avoid
   writing it out.  */
#define SCOMMON ".scommon"

/* MIPS ECOFF has COFF sections, but the debugging information is
   stored in a completely different format.  This files uses the some
   of the swapping routines from coffswap.h, and some of the generic
   COFF routines in coffgen.c, but, unlike the real COFF targets, does
   not use coffcode.h itself.  */

/* Prototypes for static functions.  */

static boolean ecoff_bad_format_hook PARAMS ((bfd *abfd, PTR filehdr));
static asection *ecoff_make_section_hook PARAMS ((bfd *abfd, char *name));
static boolean ecoff_new_section_hook PARAMS ((bfd *abfd, asection *section));
static boolean ecoff_mkobject PARAMS ((bfd *abfd));
static PTR ecoff_mkobject_hook PARAMS ((bfd *abfd, PTR filehdr, PTR aouthdr));
static boolean ecoff_set_arch_mach_hook PARAMS ((bfd *abfd, PTR filehdr));
static long ecoff_sec_to_styp_flags PARAMS ((CONST char *name,
					     flagword flags));
static flagword ecoff_styp_to_sec_flags PARAMS ((bfd *abfd, PTR hdr));
static asymbol *ecoff_make_empty_symbol PARAMS ((bfd *abfd));
static void ecoff_set_symbol_info PARAMS ((bfd *abfd, SYMR *ecoff_sym,
					   asymbol *asym, int ext));
static boolean ecoff_slurp_symbol_table PARAMS ((bfd *abfd));
static unsigned int ecoff_get_symtab_upper_bound PARAMS ((bfd *abfd));
static unsigned int ecoff_get_symtab PARAMS ((bfd *abfd,
					      asymbol **alocation));
static void ecoff_emit_aggregate PARAMS ((bfd *abfd, char *string,
					  RNDXR *rndx, long isym,
					  CONST char *which));
static char *ecoff_type_to_string PARAMS ((bfd *abfd, union aux_ext *aux_ptr,
					   int indx, int bigendian));
static void ecoff_print_symbol PARAMS ((bfd *abfd, PTR filep,
					asymbol *symbol,
					bfd_print_symbol_type how));
static void ecoff_swap_reloc_in PARAMS ((bfd *abfd, RELOC *ext,
					 struct internal_reloc *intern));
static unsigned int ecoff_swap_reloc_out PARAMS ((bfd *abfd, PTR src,
						  PTR dst));
static bfd_reloc_status_type ecoff_generic_reloc PARAMS ((bfd *abfd,
							  arelent *reloc,
							  asymbol *symbol,
							  PTR data,
							  asection *section,
							  bfd *output_bfd));
static bfd_reloc_status_type ecoff_refhi_reloc PARAMS ((bfd *abfd,
							arelent *reloc,
							asymbol *symbol,
							PTR data,
							asection *section,
							bfd *output_bfd));
static bfd_reloc_status_type ecoff_reflo_reloc PARAMS ((bfd *abfd,
							arelent *reloc,
							asymbol *symbol,
							PTR data,
							asection *section,
							bfd *output_bfd));
static bfd_reloc_status_type ecoff_gprel_reloc PARAMS ((bfd *abfd,
							arelent *reloc,
							asymbol *symbol,
							PTR data,
							asection *section,
							bfd *output_bfd));
static boolean ecoff_slurp_reloc_table PARAMS ((bfd *abfd, asection *section,
						asymbol **symbols));
static unsigned int ecoff_canonicalize_reloc PARAMS ((bfd *abfd,
						      asection *section,
						      arelent **relptr,
						      asymbol **symbols));
static CONST struct reloc_howto_struct *ecoff_bfd_reloc_type_lookup
  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));
static boolean ecoff_find_nearest_line PARAMS ((bfd *abfd,
						asection *section,
						asymbol **symbols,
						bfd_vma offset,
						CONST char **filename_ptr,
						CONST char **fnname_ptr,
						unsigned int *retline_ptr));
static void ecoff_clear_output_flags PARAMS ((bfd *abfd));
static boolean ecoff_rel PARAMS ((bfd *output_bfd, bfd_seclet_type *seclet,
				  asection *output_section, PTR data,
				  boolean relocateable));
static boolean ecoff_dump_seclet PARAMS ((bfd *abfd, bfd_seclet_type *seclet,
					  asection *section, PTR data,
					  boolean relocateable));
static long ecoff_add_string PARAMS ((bfd *output_bfd, FDR *fdr,
				      CONST char *string, boolean external));
static boolean ecoff_get_debug PARAMS ((bfd *output_bfd,
					bfd_seclet_type *seclet,
					asection *section,
					boolean relocateable));
static boolean ecoff_bfd_seclet_link PARAMS ((bfd *abfd, PTR data,
					      boolean relocateable));
static boolean ecoff_set_arch_mach PARAMS ((bfd *abfd,
					    enum bfd_architecture arch,
					    unsigned long machine));
static int ecoff_sizeof_headers PARAMS ((bfd *abfd, boolean reloc));
static void ecoff_compute_section_file_positions PARAMS ((bfd *abfd));
static boolean ecoff_set_section_contents PARAMS ((bfd *abfd,
						   asection *section,
						   PTR location,
						   file_ptr offset,
						   bfd_size_type count));
static boolean ecoff_write_object_contents PARAMS ((bfd *abfd));
static unsigned int ecoff_armap_hash PARAMS ((CONST char *s,
					      unsigned int *rehash,
					      unsigned int size,
					      unsigned int hlog));
static boolean ecoff_slurp_armap PARAMS ((bfd *abfd));
static boolean ecoff_write_armap PARAMS ((bfd *abfd, unsigned int elength,
					  struct orl *map,
					  unsigned int orl_count,
					  int stridx));
static bfd_target *ecoff_archive_p PARAMS ((bfd *abfd));

/* Get the generic COFF swapping routines, except for the reloc,
   symbol, and lineno ones.  Give them ecoff names.  */
#define MIPSECOFF
#define NO_COFF_RELOCS
#define NO_COFF_SYMBOLS
#define NO_COFF_LINENOS
#define coff_swap_filehdr_in ecoff_swap_filehdr_in
#define coff_swap_filehdr_out ecoff_swap_filehdr_out
#define coff_swap_aouthdr_in ecoff_swap_aouthdr_in
#define coff_swap_aouthdr_out ecoff_swap_aouthdr_out
#define coff_swap_scnhdr_in ecoff_swap_scnhdr_in
#define coff_swap_scnhdr_out ecoff_swap_scnhdr_out
#include "coffswap.h"

/* This stuff is somewhat copied from coffcode.h.  */

static asection bfd_debug_section = { "*DEBUG*" };

/* See whether the magic number matches.  */

static boolean
ecoff_bad_format_hook (abfd, filehdr)
     bfd *abfd;
     PTR filehdr;
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;

  if (ECOFFBADMAG (*internal_f))
    return false;

  return true;
}

/* This is a hook needed by SCO COFF, but we have nothing to do.  */

static asection *
ecoff_make_section_hook (abfd, name)
     bfd *abfd;
     char *name;
{
  return (asection *) NULL;
}

/* Initialize a new section.  */

static boolean
ecoff_new_section_hook (abfd, section)
     bfd *abfd;
     asection *section;
{
  section->alignment_power = abfd->xvec->align_power_min;

  if (strcmp (section->name, _TEXT) == 0)
    section->flags |= SEC_CODE | SEC_LOAD | SEC_ALLOC;
  else if (strcmp (section->name, _DATA) == 0
	   || strcmp (section->name, _SDATA) == 0)
    section->flags |= SEC_DATA | SEC_LOAD | SEC_ALLOC;
  else if (strcmp (section->name, _RDATA) == 0
	   || strcmp (section->name, _LIT8) == 0
	   || strcmp (section->name, _LIT4) == 0)
    section->flags |= SEC_DATA | SEC_LOAD | SEC_ALLOC | SEC_READONLY;
  else if (strcmp (section->name, _BSS) == 0
	   || strcmp (section->name, _SBSS) == 0)
    section->flags |= SEC_ALLOC;

  /* Probably any other section name is SEC_NEVER_LOAD, but I'm
     uncertain about .init on some systems and I don't know how shared
     libraries work.  */

  return true;
}

/* Set the alignment of a section; we have nothing to do.  */

#define ecoff_set_alignment_hook \
  ((void (*) PARAMS ((bfd *, asection *, PTR))) bfd_void)

/* Create an ECOFF object.  */

static boolean
ecoff_mkobject (abfd)
     bfd *abfd;
{
  abfd->tdata.ecoff_obj_data = ((struct ecoff_tdata *)
				bfd_zalloc (abfd, sizeof (ecoff_data_type)));
  if (abfd->tdata.ecoff_obj_data == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  /* Always create a .scommon section for every BFD.  This is a hack so
     that the linker has something to attach scSCommon symbols to.  */
  bfd_make_section (abfd, SCOMMON);

  return true;
}

/* Create the ECOFF backend specific information.  */

static PTR
ecoff_mkobject_hook (abfd, filehdr, aouthdr)
     bfd *abfd;
     PTR filehdr;
     PTR aouthdr;
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;
  struct internal_aouthdr *internal_a = (struct internal_aouthdr *) aouthdr;
  ecoff_data_type *ecoff;

  if (ecoff_mkobject (abfd) == false)
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
    }

  return (PTR) ecoff;
}

/* Determine the machine architecture and type.  */

static boolean
ecoff_set_arch_mach_hook (abfd, filehdr)
     bfd *abfd;
     PTR filehdr;
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;
  enum bfd_architecture arch;

  switch (internal_f->f_magic)
    {
    case MIPS_MAGIC_1:
    case MIPS_MAGIC_LITTLE:
    case MIPS_MAGIC_BIG:
      arch = bfd_arch_mips;
      break;

    default:
      arch = bfd_arch_obscure;
      break;
    }

  bfd_default_set_arch_mach (abfd, arch, (unsigned long) 0);

  return true;
}

/* Get the section s_flags to use for a section.  */

static long
ecoff_sec_to_styp_flags (name, flags)
     CONST char *name;
     flagword flags;
{
  long styp;

  styp = 0;

  if (strcmp (name, _TEXT) == 0)
    styp = STYP_TEXT;
  else if (strcmp (name, _DATA) == 0)
    styp = STYP_DATA;
  else if (strcmp (name, _SDATA) == 0)
    styp = STYP_SDATA;
  else if (strcmp (name, _RDATA) == 0)
    styp = STYP_RDATA;
  else if (strcmp (name, _LIT8) == 0)
    styp = STYP_LIT8;
  else if (strcmp (name, _LIT4) == 0)
    styp = STYP_LIT4;
  else if (strcmp (name, _BSS) == 0)
    styp = STYP_BSS;
  else if (strcmp (name, _SBSS) == 0)
    styp = STYP_SBSS;
  else if (flags & SEC_CODE) 
    styp = STYP_TEXT;
  else if (flags & SEC_DATA) 
    styp = STYP_DATA;
  else if (flags & SEC_READONLY)
    styp = STYP_RDATA;
  else if (flags & SEC_LOAD)
    styp = STYP_TEXT;
  else
    styp = STYP_BSS;

  if (flags & SEC_NEVER_LOAD)
    styp |= STYP_NOLOAD;

  return styp;
}

/* Get the BFD flags to use for a section.  */

static flagword
ecoff_styp_to_sec_flags (abfd, hdr)
     bfd *abfd;
     PTR hdr;
{
  struct internal_scnhdr *internal_s = (struct internal_scnhdr *) hdr;
  long styp_flags = internal_s->s_flags;
  flagword sec_flags=0;

  if (styp_flags & STYP_NOLOAD)
    sec_flags |= SEC_NEVER_LOAD;

  /* For 386 COFF, at least, an unloadable text or data section is
     actually a shared library section.  */
  if (styp_flags & STYP_TEXT)
    {
      if (sec_flags & SEC_NEVER_LOAD)
	sec_flags |= SEC_CODE | SEC_SHARED_LIBRARY;
      else
	sec_flags |= SEC_CODE | SEC_LOAD | SEC_ALLOC;
    }
  else if ((styp_flags & STYP_DATA)
	   || (styp_flags & STYP_RDATA)
	   || (styp_flags & STYP_SDATA))
    {
      if (sec_flags & SEC_NEVER_LOAD)
	sec_flags |= SEC_DATA | SEC_SHARED_LIBRARY;
      else
	sec_flags |= SEC_DATA | SEC_LOAD | SEC_ALLOC;
      if (styp_flags & STYP_RDATA)
	sec_flags |= SEC_READONLY;
    }
  else if ((styp_flags & STYP_BSS)
	   || (styp_flags & STYP_SBSS))
    {
      sec_flags |= SEC_ALLOC;
    }
  else if (styp_flags & STYP_INFO) 
    {
      sec_flags |= SEC_NEVER_LOAD;
    }
  else if ((styp_flags & STYP_LIT8)
	   || (styp_flags & STYP_LIT4))
    {
      sec_flags |= SEC_DATA | SEC_LOAD | SEC_ALLOC | SEC_READONLY;
    }
  else
    {
      sec_flags |= SEC_ALLOC | SEC_LOAD;
    }

  return sec_flags;
}

/* Read in and swap the important symbolic information for an ECOFF
   object file.  FIXME: This is called by gdb.  If there is ever
   another ECOFF target, it should be moved into some sort of target
   specific structure.  */

boolean
ecoff_slurp_symbolic_info (abfd)
     bfd *abfd;
{
  struct hdr_ext external_symhdr;
  HDRR *internal_symhdr;
  bfd_size_type raw_base;
  bfd_size_type raw_size;
  PTR raw;
  struct fdr_ext *fraw_src;
  struct fdr_ext *fraw_end;
  struct fdr *fdr_ptr;

  /* Check whether we've already gotten it, and whether there's any to
     get.  */
  if (ecoff_data (abfd)->raw_syments != (PTR) NULL)
    return true;
  if (ecoff_data (abfd)->sym_filepos == 0)
    {
      bfd_get_symcount (abfd) = 0;
      return true;
    }

  /* At this point bfd_get_symcount (abfd) holds the number of symbols
     as read from the file header, but on ECOFF this is always the
     size of the symbolic information header.  It would be cleaner to
     handle this when we first read the file in coffgen.c.  */
  if (bfd_get_symcount (abfd) != sizeof (external_symhdr))
    {
      bfd_error = bad_value;
      return false;
    }

  /* Read the symbolic information header.  */
  if (bfd_seek (abfd, ecoff_data (abfd)->sym_filepos, SEEK_SET) == -1
      || (bfd_read ((PTR) &external_symhdr, sizeof (external_symhdr), 1, abfd)
	  != sizeof (external_symhdr)))
    {
      bfd_error = system_call_error;
      return false;
    }
  internal_symhdr = &ecoff_data (abfd)->symbolic_header;
  ecoff_swap_hdr_in (abfd, &external_symhdr, internal_symhdr);

  if (internal_symhdr->magic != magicSym)
    {
      bfd_error = bad_value;
      return false;
    }

  /* Now we can get the correct number of symbols.  */
  bfd_get_symcount (abfd) = (internal_symhdr->isymMax
			     + internal_symhdr->iextMax);

  /* Read all the symbolic information at once.  */
  raw_base = ecoff_data (abfd)->sym_filepos + sizeof (struct hdr_ext);

  if (internal_symhdr->cbExtOffset != 0)
    raw_size = (internal_symhdr->cbExtOffset
		- raw_base
		+ internal_symhdr->iextMax * sizeof (struct ext_ext));
  else
    {
      long cbline, issmax, issextmax;

      cbline = (internal_symhdr->cbLine + 3) &~ 3;
      issmax = (internal_symhdr->issMax + 3) &~ 3;
      issextmax = (internal_symhdr->issExtMax + 3) &~ 3;
      raw_size = (cbline * sizeof (unsigned char)
		  + internal_symhdr->idnMax * sizeof (struct dnr_ext)
		  + internal_symhdr->ipdMax * sizeof (struct pdr_ext)
		  + internal_symhdr->isymMax * sizeof (struct sym_ext)
		  + internal_symhdr->ioptMax * sizeof (struct opt_ext)
		  + internal_symhdr->iauxMax * sizeof (union aux_ext)
		  + issmax * sizeof (char)
		  + issextmax * sizeof (char)
		  + internal_symhdr->ifdMax * sizeof (struct fdr_ext)
		  + internal_symhdr->crfd * sizeof (struct rfd_ext)
		  + internal_symhdr->iextMax * sizeof (struct ext_ext));
    }

  if (raw_size == 0)
    {
      ecoff_data (abfd)->sym_filepos = 0;
      return true;
    }
  raw = (PTR) bfd_alloc (abfd, raw_size);
  if (raw == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  if (bfd_read (raw, raw_size, 1, abfd) != raw_size)
    {
      bfd_error = system_call_error;
      bfd_release (abfd, raw);
      return false;
    }

  ecoff_data (abfd)->raw_size = raw_size;
  ecoff_data (abfd)->raw_syments = raw;

  /* Get pointers for the numeric offsets in the HDRR structure.  */
#define FIX(off1, off2, type) \
  if (internal_symhdr->off1 == 0) \
    ecoff_data (abfd)->off2 = (type *) NULL; \
  else \
    ecoff_data (abfd)->off2 = (type *) ((char *) raw \
					+ internal_symhdr->off1 \
					- raw_base)
  FIX (cbLineOffset, line, unsigned char);
  FIX (cbDnOffset, external_dnr, struct dnr_ext);
  FIX (cbPdOffset, external_pdr, struct pdr_ext);
  FIX (cbSymOffset, external_sym, struct sym_ext);
  FIX (cbOptOffset, external_opt, struct opt_ext);
  FIX (cbAuxOffset, external_aux, union aux_ext);
  FIX (cbSsOffset, ss, char);
  FIX (cbSsExtOffset, ssext, char);
  FIX (cbFdOffset, external_fdr, struct fdr_ext);
  FIX (cbRfdOffset, external_rfd, struct rfd_ext);
  FIX (cbExtOffset, external_ext, struct ext_ext);
#undef FIX

  /* I don't want to always swap all the data, because it will just
     waste time and most programs will never look at it.  The only
     time the linker needs most of the debugging information swapped
     is when linking big-endian and little-endian MIPS object files
     together, which is not a common occurrence.

     We need to look at the fdr to deal with a lot of information in
     the symbols, so we swap them here.  */
  ecoff_data (abfd)->fdr = (struct fdr *) bfd_alloc (abfd,
						     (internal_symhdr->ifdMax *
						      sizeof (struct fdr)));
  if (ecoff_data (abfd)->fdr == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  fdr_ptr = ecoff_data (abfd)->fdr;
  fraw_src = ecoff_data (abfd)->external_fdr;
  fraw_end = fraw_src + internal_symhdr->ifdMax;
  for (; fraw_src < fraw_end; fraw_src++, fdr_ptr++)
    ecoff_swap_fdr_in (abfd, fraw_src, fdr_ptr);

  return true;
}

/* ECOFF symbol table routines.  The ECOFF symbol table is described
   in gcc/mips-tfile.c.  */

/* ECOFF uses two common sections.  One is the usual one, and the
   other is for small objects.  All the small objects are kept
   together, and then referenced via the gp pointer, which yields
   faster assembler code.  This is what we use for the small common
   section.  */
static asection ecoff_scom_section;
static asymbol ecoff_scom_symbol;
static asymbol *ecoff_scom_symbol_ptr;

/* Create an empty symbol.  */

static asymbol *
ecoff_make_empty_symbol (abfd)
     bfd *abfd;
{
  ecoff_symbol_type *new;

  new = (ecoff_symbol_type *) bfd_alloc (abfd, sizeof (ecoff_symbol_type));
  if (new == (ecoff_symbol_type *) NULL)
    {
      bfd_error = no_memory;
      return (asymbol *) NULL;
    }
  memset (new, 0, sizeof *new);
  new->symbol.section = (asection *) NULL;
  new->fdr = (FDR *) NULL;
  new->local = false;
  new->native.lnative = (struct sym_ext *) NULL;
  new->symbol.the_bfd = abfd;
  return &new->symbol;
}

/* Set the BFD flags and section for an ECOFF symbol.  */

static void
ecoff_set_symbol_info (abfd, ecoff_sym, asym, ext)
     bfd *abfd;
     SYMR *ecoff_sym;
     asymbol *asym;
     int ext;
{
  asym->the_bfd = abfd;
  asym->value = ecoff_sym->value;
  asym->section = &bfd_debug_section;
  asym->udata = NULL;

  /* Most symbol types are just for debugging.  */
  switch (ecoff_sym->st)
    {
    case stGlobal:
    case stStatic:
    case stLabel:
    case stProc:
    case stStaticProc:
      break;
    case stNil:
      if (MIPS_IS_STAB (ecoff_sym))
	{
	  asym->flags = BSF_DEBUGGING;
	  return;
	}
      break;
    default:
      asym->flags = BSF_DEBUGGING;
      return;
    }

  if (ext)
    asym->flags = BSF_EXPORT | BSF_GLOBAL;
  else
    asym->flags = BSF_LOCAL;
  switch (ecoff_sym->sc)
    {
    case scNil:
      /* Used for compiler generated labels.  Leave them in the
	 debugging section, and mark them as local.  If BSF_DEBUGGING
	 is set, then nm does not display them for some reason.  If no
	 flags are set then the linker whines about them.  */
      asym->flags = BSF_LOCAL;
      break;
    case scText:
      asym->section = bfd_make_section_old_way (abfd, ".text");
      asym->value -= asym->section->vma;
      break;
    case scData:
      asym->section = bfd_make_section_old_way (abfd, ".data");
      asym->value -= asym->section->vma;
      break;
    case scBss:
      if (ext)
	{
	  asym->section = &bfd_com_section;
	  asym->flags = 0;
	}
      else
	{
	  asym->section = bfd_make_section_old_way (abfd, ".bss");
	  asym->value -= asym->section->vma;
	}
      break;
    case scRegister:
      asym->flags = BSF_DEBUGGING;
      break;
    case scAbs:
      asym->section = &bfd_abs_section;
      break;
    case scUndefined:
      asym->section = &bfd_und_section;
      asym->flags = 0;
      asym->value = 0;
      break;
    case scCdbLocal:
    case scBits:
    case scCdbSystem:
    case scRegImage:
    case scInfo:
    case scUserStruct:
      asym->flags = BSF_DEBUGGING;
      break;
    case scSData:
      asym->section = bfd_make_section_old_way (abfd, ".sdata");
      asym->value -= asym->section->vma;
      break;
    case scSBss:
      asym->section = bfd_make_section_old_way (abfd, ".sbss");
      if (! ext)
	asym->value -= asym->section->vma;
      break;
    case scRData:
      asym->section = bfd_make_section_old_way (abfd, ".rdata");
      asym->value -= asym->section->vma;
      break;
    case scVar:
      asym->flags = BSF_DEBUGGING;
      break;
    case scCommon:
      if (asym->value > ecoff_data (abfd)->gp_size)
	{
	  asym->section = &bfd_com_section;
	  asym->flags = 0;
	  break;
	}
      /* Fall through.  */
    case scSCommon:
      if (ecoff_scom_section.name == NULL)
	{
	  /* Initialize the small common section.  */
	  ecoff_scom_section.name = SCOMMON;
	  ecoff_scom_section.flags = SEC_IS_COMMON;
	  ecoff_scom_section.output_section = &ecoff_scom_section;
	  ecoff_scom_section.symbol = &ecoff_scom_symbol;
	  ecoff_scom_section.symbol_ptr_ptr = &ecoff_scom_symbol_ptr;
	  ecoff_scom_symbol.name = SCOMMON;
	  ecoff_scom_symbol.flags = BSF_SECTION_SYM;
	  ecoff_scom_symbol.section = &ecoff_scom_section;
	  ecoff_scom_symbol_ptr = &ecoff_scom_symbol;
	}
      asym->section = &ecoff_scom_section;
      asym->flags = 0;
      break;
    case scVarRegister:
    case scVariant:
      asym->flags = BSF_DEBUGGING;
      break;
    case scSUndefined:
      asym->section = &bfd_und_section;
      asym->flags = 0;
      asym->value = 0;
      break;
    case scInit:
      asym->section = bfd_make_section_old_way (abfd, ".init");
      asym->value -= asym->section->vma;
      break;
    case scBasedVar:
    case scXData:
    case scPData:
      asym->flags = BSF_DEBUGGING;
      break;
    case scFini:
      asym->section = bfd_make_section_old_way (abfd, ".fini");
      asym->value -= asym->section->vma;
      break;
    default:
      break;
    }
}

/* Read an ECOFF symbol table.  */

static boolean
ecoff_slurp_symbol_table (abfd)
     bfd *abfd;
{
  bfd_size_type internal_size;
  ecoff_symbol_type *internal;
  ecoff_symbol_type *internal_ptr;
  struct ext_ext *eraw_src;
  struct ext_ext *eraw_end;
  FDR *fdr_ptr;
  FDR *fdr_end;

  /* If we've already read in the symbol table, do nothing.  */
  if (ecoff_data (abfd)->canonical_symbols != NULL)
    return true;

  /* Get the symbolic information.  */
  if (ecoff_slurp_symbolic_info (abfd) == false)
    return false;
  if (bfd_get_symcount (abfd) == 0)
    return true;

  internal_size = bfd_get_symcount (abfd) * sizeof (ecoff_symbol_type);
  internal = (ecoff_symbol_type *) bfd_alloc (abfd, internal_size);
  if (internal == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  internal_ptr = internal;
  eraw_src = ecoff_data (abfd)->external_ext;
  eraw_end = eraw_src + ecoff_data (abfd)->symbolic_header.iextMax;
  for (; eraw_src < eraw_end; eraw_src++, internal_ptr++)
    {
      EXTR internal_esym;

      ecoff_swap_ext_in (abfd, eraw_src, &internal_esym);
      internal_ptr->symbol.name = (ecoff_data (abfd)->ssext
				   + internal_esym.asym.iss);
      ecoff_set_symbol_info (abfd, &internal_esym.asym,
			     &internal_ptr->symbol, 1);
      internal_ptr->fdr = ecoff_data (abfd)->fdr + internal_esym.ifd;
      internal_ptr->local = false;
      internal_ptr->native.enative = eraw_src;
    }

  /* The local symbols must be accessed via the fdr's, because the
     string and aux indices are relative to the fdr information.  */
  fdr_ptr = ecoff_data (abfd)->fdr;
  fdr_end = fdr_ptr + ecoff_data (abfd)->symbolic_header.ifdMax;
  for (; fdr_ptr < fdr_end; fdr_ptr++)
    {
      struct sym_ext *lraw_src;
      struct sym_ext *lraw_end;

      lraw_src = ecoff_data (abfd)->external_sym + fdr_ptr->isymBase;
      lraw_end = lraw_src + fdr_ptr->csym;
      for (; lraw_src < lraw_end; lraw_src++, internal_ptr++)
	{
	  SYMR internal_sym;

	  ecoff_swap_sym_in (abfd, lraw_src, &internal_sym);
	  internal_ptr->symbol.name = (ecoff_data (abfd)->ss
				       + fdr_ptr->issBase
				       + internal_sym.iss);
	  ecoff_set_symbol_info (abfd, &internal_sym,
				 &internal_ptr->symbol, 0);
	  internal_ptr->fdr = fdr_ptr;
	  internal_ptr->local = true;
	  internal_ptr->native.lnative = lraw_src;
	}
    }

  ecoff_data (abfd)->canonical_symbols = internal;

  return true;
}

static unsigned int
ecoff_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  if (ecoff_slurp_symbolic_info (abfd) == false
      || bfd_get_symcount (abfd) == 0)
    return 0;

  return (bfd_get_symcount (abfd) + 1) * (sizeof (ecoff_symbol_type *));
}

static unsigned int
ecoff_get_symtab (abfd, alocation)
     bfd *abfd;
     asymbol **alocation;
{
  unsigned int counter = 0;
  ecoff_symbol_type *symbase;
  ecoff_symbol_type **location = (ecoff_symbol_type **) alocation;

  if (ecoff_slurp_symbol_table (abfd) == false
      || bfd_get_symcount (abfd) == 0)
    return 0;

  symbase = ecoff_data (abfd)->canonical_symbols;
  while (counter < bfd_get_symcount (abfd))
    {
      *(location++) = symbase++;
      counter++;
    }
  *location++ = (ecoff_symbol_type *) NULL;
  return bfd_get_symcount (abfd);
}

/* Turn ECOFF type information into a printable string.
   ecoff_emit_aggregate and ecoff_type_to_string are from
   gcc/mips-tdump.c, with swapping added and used_ptr removed.  */

/* Write aggregate information to a string.  */

static void
ecoff_emit_aggregate (abfd, string, rndx, isym, which)
     bfd *abfd;
     char *string;
     RNDXR *rndx;
     long isym;
     CONST char *which;
{
  int ifd = rndx->rfd;
  int indx = rndx->index;
  int sym_base, ss_base;
  CONST char *name;
  
  if (ifd == 0xfff)
    ifd = isym;

  sym_base = ecoff_data (abfd)->fdr[ifd].isymBase;
  ss_base  = ecoff_data (abfd)->fdr[ifd].issBase;
  
  if (indx == indexNil)
    name = "/* no name */";
  else
    {
      SYMR sym;

      indx += sym_base;
      ecoff_swap_sym_in (abfd,
			 ecoff_data (abfd)->external_sym + indx,
			 &sym);
      name = ecoff_data (abfd)->ss + ss_base + sym.iss;
    }

  sprintf (string,
	   "%s %s { ifd = %d, index = %d }",
	   which, name, ifd,
	   indx + ecoff_data (abfd)->symbolic_header.iextMax);
}

/* Convert the type information to string format.  */

static char *
ecoff_type_to_string (abfd, aux_ptr, indx, bigendian)
     bfd *abfd;
     union aux_ext *aux_ptr;
     int indx;
     int bigendian;
{
  AUXU u;
  struct qual {
    unsigned int  type;
    int  low_bound;
    int  high_bound;
    int  stride;
  } qualifiers[7];

  unsigned int basic_type;
  int i;
  static char buffer1[1024];
  static char buffer2[1024];
  char *p1 = buffer1;
  char *p2 = buffer2;
  RNDXR rndx;

  for (i = 0; i < 7; i++)
    {
      qualifiers[i].low_bound = 0;
      qualifiers[i].high_bound = 0;
      qualifiers[i].stride = 0;
    }

  if (AUX_GET_ISYM (bigendian, &aux_ptr[indx]) == -1)
    return "-1 (no type)";
  ecoff_swap_tir_in (bigendian, &aux_ptr[indx++].a_ti, &u.ti);

  basic_type = u.ti.bt;
  qualifiers[0].type = u.ti.tq0;
  qualifiers[1].type = u.ti.tq1;
  qualifiers[2].type = u.ti.tq2;
  qualifiers[3].type = u.ti.tq3;
  qualifiers[4].type = u.ti.tq4;
  qualifiers[5].type = u.ti.tq5;
  qualifiers[6].type = tqNil;

  /*
   * Go get the basic type.
   */
  switch (basic_type)
    {
    case btNil:			/* undefined */
      strcpy (p1, "nil");
      break;

    case btAdr:			/* address - integer same size as pointer */
      strcpy (p1, "address");
      break;

    case btChar:		/* character */
      strcpy (p1, "char");
      break;

    case btUChar:		/* unsigned character */
      strcpy (p1, "unsigned char");
      break;

    case btShort:		/* short */
      strcpy (p1, "short");
      break;

    case btUShort:		/* unsigned short */
      strcpy (p1, "unsigned short");
      break;

    case btInt:			/* int */
      strcpy (p1, "int");
      break;

    case btUInt:		/* unsigned int */
      strcpy (p1, "unsigned int");
      break;

    case btLong:		/* long */
      strcpy (p1, "long");
      break;

    case btULong:		/* unsigned long */
      strcpy (p1, "unsigned long");
      break;

    case btFloat:		/* float (real) */
      strcpy (p1, "float");
      break;

    case btDouble:		/* Double (real) */
      strcpy (p1, "double");
      break;

      /* Structures add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to struct def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case btStruct:		/* Structure (Record) */
      ecoff_swap_rndx_in (bigendian, &aux_ptr[indx].a_rndx, &rndx);
      ecoff_emit_aggregate (abfd, p1, &rndx,
			    AUX_GET_ISYM (bigendian, &aux_ptr[indx+1]),
			    "struct");
      indx++;			/* skip aux words */
      break;

      /* Unions add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to union def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case btUnion:		/* Union */
      ecoff_swap_rndx_in (bigendian, &aux_ptr[indx].a_rndx, &rndx);
      ecoff_emit_aggregate (abfd, p1, &rndx,
			    AUX_GET_ISYM (bigendian, &aux_ptr[indx+1]),
			    "union");
      indx++;			/* skip aux words */
      break;

      /* Enumerations add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to enum def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case btEnum:		/* Enumeration */
      ecoff_swap_rndx_in (bigendian, &aux_ptr[indx].a_rndx, &rndx);
      ecoff_emit_aggregate (abfd, p1, &rndx,
			    AUX_GET_ISYM (bigendian, &aux_ptr[indx+1]),
			    "enum");
      indx++;			/* skip aux words */
      break;

    case btTypedef:		/* defined via a typedef, isymRef points */
      strcpy (p1, "typedef");
      break;

    case btRange:		/* subrange of int */
      strcpy (p1, "subrange");
      break;

    case btSet:			/* pascal sets */
      strcpy (p1, "set");
      break;

    case btComplex:		/* fortran complex */
      strcpy (p1, "complex");
      break;

    case btDComplex:		/* fortran double complex */
      strcpy (p1, "double complex");
      break;

    case btIndirect:		/* forward or unnamed typedef */
      strcpy (p1, "forward/unamed typedef");
      break;

    case btFixedDec:		/* Fixed Decimal */
      strcpy (p1, "fixed decimal");
      break;

    case btFloatDec:		/* Float Decimal */
      strcpy (p1, "float decimal");
      break;

    case btString:		/* Varying Length Character String */
      strcpy (p1, "string");
      break;

    case btBit:			/* Aligned Bit String */
      strcpy (p1, "bit");
      break;

    case btPicture:		/* Picture */
      strcpy (p1, "picture");
      break;

    case btVoid:		/* Void */
      strcpy (p1, "void");
      break;

    default:
      sprintf (p1, "Unknown basic type %d", (int) basic_type);
      break;
    }

  p1 += strlen (buffer1);

  /*
   * If this is a bitfield, get the bitsize.
   */
  if (u.ti.fBitfield)
    {
      int bitsize;

      bitsize = AUX_GET_WIDTH (bigendian, &aux_ptr[indx++]);
      sprintf (p1, " : %d", bitsize);
      p1 += strlen (buffer1);
    }


  /*
   * Deal with any qualifiers.
   */
  if (qualifiers[0].type != tqNil)
    {
      /*
       * Snarf up any array bounds in the correct order.  Arrays
       * store 5 successive words in the aux. table:
       *	word 0	RNDXR to type of the bounds (ie, int)
       *	word 1	Current file descriptor index
       *	word 2	low bound
       *	word 3	high bound (or -1 if [])
       *	word 4	stride size in bits
       */
      for (i = 0; i < 7; i++)
	{
	  if (qualifiers[i].type == tqArray)
	    {
	      qualifiers[i].low_bound =
		AUX_GET_DNLOW (bigendian, &aux_ptr[indx+2]);
	      qualifiers[i].high_bound =
		AUX_GET_DNHIGH (bigendian, &aux_ptr[indx+3]);
	      qualifiers[i].stride =
		AUX_GET_WIDTH (bigendian, &aux_ptr[indx+4]);
	      indx += 5;
	    }
	}

      /*
       * Now print out the qualifiers.
       */
      for (i = 0; i < 6; i++)
	{
	  switch (qualifiers[i].type)
	    {
	    case tqNil:
	    case tqMax:
	      break;

	    case tqPtr:
	      strcpy (p2, "ptr to ");
	      p2 += sizeof ("ptr to ")-1;
	      break;

	    case tqVol:
	      strcpy (p2, "volatile ");
	      p2 += sizeof ("volatile ")-1;
	      break;

	    case tqFar:
	      strcpy (p2, "far ");
	      p2 += sizeof ("far ")-1;
	      break;

	    case tqProc:
	      strcpy (p2, "func. ret. ");
	      p2 += sizeof ("func. ret. ");
	      break;

	    case tqArray:
	      {
		int first_array = i;
		int j;

		/* Print array bounds reversed (ie, in the order the C
		   programmer writes them).  C is such a fun language.... */

		while (i < 5 && qualifiers[i+1].type == tqArray)
		  i++;

		for (j = i; j >= first_array; j--)
		  {
		    strcpy (p2, "array [");
		    p2 += sizeof ("array [")-1;
		    if (qualifiers[j].low_bound != 0)
		      sprintf (p2,
			       "%ld:%ld {%ld bits}",
			       (long) qualifiers[j].low_bound,
			       (long) qualifiers[j].high_bound,
			       (long) qualifiers[j].stride);

		    else if (qualifiers[j].high_bound != -1)
		      sprintf (p2,
			       "%ld {%ld bits}",
			       (long) (qualifiers[j].high_bound + 1),
			       (long) (qualifiers[j].stride));

		    else
		      sprintf (p2, " {%ld bits}", (long) (qualifiers[j].stride));

		    p2 += strlen (p2);
		    strcpy (p2, "] of ");
		    p2 += sizeof ("] of ")-1;
		  }
	      }
	      break;
	    }
	}
    }

  strcpy (p2, buffer1);
  return buffer2;
}

/* Print information about an ECOFF symbol.  */

static void
ecoff_print_symbol (abfd, filep, symbol, how)
     bfd *abfd;
     PTR filep;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *)filep;

  switch (how)
    {
    case bfd_print_symbol_name:
      fprintf (file, "%s", symbol->name);
      break;
    case bfd_print_symbol_more:
      if (ecoffsymbol (symbol)->local)
	{
	  SYMR ecoff_sym;
	
	  ecoff_swap_sym_in (abfd, ecoffsymbol (symbol)->native.lnative,
			     &ecoff_sym);
	  fprintf (file, "ecoff local %lx %x %x",
		   (unsigned long) ecoff_sym.value,
		   (unsigned) ecoff_sym.st, (unsigned) ecoff_sym.sc);
	}
      else
	{
	  EXTR ecoff_ext;

	  ecoff_swap_ext_in (abfd, ecoffsymbol (symbol)->native.enative,
			     &ecoff_ext);
	  fprintf (file, "ecoff extern %lx %x %x",
		   (unsigned long) ecoff_ext.asym.value,
		   (unsigned) ecoff_ext.asym.st,
		   (unsigned) ecoff_ext.asym.sc);
	}
      break;
    case bfd_print_symbol_nm:
      {
	CONST char *section_name = symbol->section->name;

	bfd_print_symbol_vandf ((PTR) file, symbol);
	fprintf (file, " %-5s %s %s",
		 section_name,
		 ecoffsymbol (symbol)->local ? "l" : "e",
		 symbol->name);
      }
      break;
    case bfd_print_symbol_all:
      /* Print out the symbols in a reasonable way */
      {
	char type;
	int pos;
	EXTR ecoff_ext;
	char jmptbl;
	char cobol_main;
	char weakext;

	if (ecoffsymbol (symbol)->local)
	  {
	    ecoff_swap_sym_in (abfd, ecoffsymbol (symbol)->native.lnative,
			       &ecoff_ext.asym);
	    type = 'l';
	    pos = (ecoffsymbol (symbol)->native.lnative
		   - ecoff_data (abfd)->external_sym
		   + ecoff_data (abfd)->symbolic_header.iextMax);
	    jmptbl = ' ';
	    cobol_main = ' ';
	    weakext = ' ';
	  }
	else
	  {
	    ecoff_swap_ext_in (abfd, ecoffsymbol (symbol)->native.enative,
			       &ecoff_ext);
	    type = 'e';
	    pos = (ecoffsymbol (symbol)->native.enative
		   - ecoff_data (abfd)->external_ext);
	    jmptbl = ecoff_ext.jmptbl ? 'j' : ' ';
	    cobol_main = ecoff_ext.cobol_main ? 'c' : ' ';
	    weakext = ecoff_ext.weakext ? 'w' : ' ';
	  }

	fprintf (file, "[%3d] %c %lx st %x sc %x indx %x %c%c%c %s",
		 pos, type, (unsigned long) ecoff_ext.asym.value,
		 (unsigned) ecoff_ext.asym.st,
		 (unsigned) ecoff_ext.asym.sc,
		 (unsigned) ecoff_ext.asym.index,
		 jmptbl, cobol_main, weakext,
		 symbol->name);

	if (ecoffsymbol (symbol)->fdr != NULL
	    && ecoff_ext.asym.index != indexNil)
	  {
	    unsigned indx;
	    int bigendian;
	    long sym_base;
	    union aux_ext *aux_base;

	    indx = ecoff_ext.asym.index;

	    /* sym_base is used to map the fdr relative indices which
	       appear in the file to the position number which we are
	       using.  */
	    sym_base = ecoffsymbol (symbol)->fdr->isymBase;
	    if (ecoffsymbol (symbol)->local)
	      sym_base += ecoff_data (abfd)->symbolic_header.iextMax;

	    /* aux_base is the start of the aux entries for this file;
	       asym.index is an offset from this.  */
	    aux_base = (ecoff_data (abfd)->external_aux
			+ ecoffsymbol (symbol)->fdr->iauxBase);

	    /* The aux entries are stored in host byte order; the
	       order is indicated by a bit in the fdr.  */
	    bigendian = ecoffsymbol (symbol)->fdr->fBigendian;

	    /* This switch is basically from gcc/mips-tdump.c  */
	    switch (ecoff_ext.asym.st)
	      {
	      case stNil:
	      case stLabel:
		break;

	      case stFile:
	      case stBlock:
		printf ("\n      End+1 symbol: %ld", indx + sym_base);
		break;

	      case stEnd:
		if (ecoff_ext.asym.sc == scText
		    || ecoff_ext.asym.sc == scInfo)
		  printf ("\n      First symbol: %ld", indx + sym_base);
		else
		  printf ("\n      First symbol: %ld", 
			  (AUX_GET_ISYM (bigendian,
					 &aux_base[ecoff_ext.asym.index])
			   + sym_base));
		break;

	      case stProc:
	      case stStaticProc:
		if (MIPS_IS_STAB (&ecoff_ext.asym))
		  ;
		else if (ecoffsymbol (symbol)->local)
		  printf ("\n      End+1 symbol: %-7ld   Type:  %s",
			  (AUX_GET_ISYM (bigendian,
					 &aux_base[ecoff_ext.asym.index])
			   + sym_base),
			  ecoff_type_to_string (abfd, aux_base, indx + 1,
						bigendian));
		else
		  printf ("\n      Local symbol: %d",
			  (indx
			   + sym_base
			   + ecoff_data (abfd)->symbolic_header.iextMax));
		break;

	      default:
		if (!MIPS_IS_STAB (&ecoff_ext.asym))
		  printf ("\n      Type: %s",
			  ecoff_type_to_string (abfd, aux_base, indx,
						bigendian));
		break;
	      }
	  }
      }
      break;
    }
}

/* Reloc handling.  MIPS ECOFF relocs are packed into 8 bytes in
   external form.  They use a bit which indicates whether the symbol
   is external.  */

/* Swap a reloc in.  */

static void
ecoff_swap_reloc_in (abfd, ext, intern)
     bfd *abfd;
     RELOC *ext;
     struct internal_reloc *intern;
{
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

static unsigned int
ecoff_swap_reloc_out (abfd, src, dst)
     bfd *abfd;
     PTR src;
     PTR dst;
{
  struct internal_reloc *intern = (struct internal_reloc *) src;
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

  return RELSZ;
}

/* ECOFF relocs are either against external symbols, or against
   sections.  If we are producing relocateable output, and the reloc
   is against an external symbol, and nothing has given us any
   additional addend, the resulting reloc will also be against the
   same symbol.  In such a case, we don't want to change anything
   about the way the reloc is handled, since it will all be done at
   final link time.  Rather than put special case code into
   bfd_perform_relocation, all the reloc types use this howto
   function.  It just short circuits the reloc if producing
   relocateable output against an external symbol.  */

static bfd_reloc_status_type
ecoff_generic_reloc (abfd,
		     reloc_entry,
		     symbol,
		     data,
		     input_section,
		     output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && reloc_entry->addend == 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  return bfd_reloc_continue;
}

/* Do a REFHI relocation.  This has to be done in combination with a
   REFLO reloc, because there is a carry from the REFLO to the REFHI.
   Here we just save the information we need; we do the actual
   relocation when we see the REFLO.  ECOFF requires that the REFLO
   immediately follow the REFHI, so this ought to work.  */

static bfd_byte *ecoff_refhi_addr;
static bfd_vma ecoff_refhi_addend;

static bfd_reloc_status_type
ecoff_refhi_reloc (abfd,
		   reloc_entry,
		   symbol,
		   data,
		   input_section,
		   output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  bfd_reloc_status_type ret;
  bfd_vma relocation;

  /* If we're relocating, and this an external symbol, we don't want
     to change anything.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && reloc_entry->addend == 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  ret = bfd_reloc_ok;
  if (symbol->section == &bfd_und_section
      && output_bfd == (bfd *) NULL)
    ret = bfd_reloc_undefined;

  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc_entry->addend;

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  /* Save the information, and let REFLO do the actual relocation.  */
  ecoff_refhi_addr = (bfd_byte *) data + reloc_entry->address;
  ecoff_refhi_addend = relocation;

  if (output_bfd != (bfd *) NULL)
    reloc_entry->address += input_section->output_offset;

  return ret;
}

/* Do a REFLO relocation.  This is a straightforward 16 bit inplace
   relocation; this function exists in order to do the REFHI
   relocation described above.  */

static bfd_reloc_status_type
ecoff_reflo_reloc (abfd,
		   reloc_entry,
		   symbol,
		   data,
		   input_section,
		   output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  if (ecoff_refhi_addr != (bfd_byte *) NULL)
    {
      unsigned long insn;
      unsigned long val;
      unsigned long vallo;

      /* Do the REFHI relocation.  Note that we actually don't need to
	 know anything about the REFLO itself, except where to find
	 the low 16 bits of the addend needed by the REFHI.  */
      insn = bfd_get_32 (abfd, ecoff_refhi_addr);
      vallo = (bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address)
	       & 0xffff);
      val = ((insn & 0xffff) << 16) + vallo;
      val += ecoff_refhi_addend;

      /* The low order 16 bits are always treated as a signed value.
	 Therefore, a negative value in the low order bits requires an
	 adjustment in the high order bits.  We need to make this
	 adjustment in two ways: once for the bits we took from the
	 data, and once for the bits we are putting back in to the
	 data.  */
      if ((vallo & 0x8000) != 0)
	val -= 0x10000;
      if ((val & 0x8000) != 0)
	val += 0x10000;

      insn = (insn &~ 0xffff) | ((val >> 16) & 0xffff);
      bfd_put_32 (abfd, insn, ecoff_refhi_addr);

      ecoff_refhi_addr = (bfd_byte *) NULL;
    }

  /* Now do the REFLO reloc in the usual way.  */
  return ecoff_generic_reloc (abfd, reloc_entry, symbol, data,
			      input_section, output_bfd);
}

/* Do a GPREL relocation.  This is a 16 bit value which must become
   the offset from the gp register.  */

static bfd_reloc_status_type
ecoff_gprel_reloc (abfd,
		   reloc_entry,
		   symbol,
		   data,
		   input_section,
		   output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  boolean relocateable;
  bfd_vma relocation;
  unsigned long val;
  unsigned long insn;

  /* If we're relocating, and this is an external symbol with no
     addend, we don't want to change anything.  We will only have an
     addend if this is a newly created reloc, not read from an ECOFF
     file.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && reloc_entry->addend == 0)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != (bfd *) NULL)
    relocateable = true;
  else
    {
      relocateable = false;
      output_bfd = symbol->section->output_section->owner;
    }

  if (symbol->section == &bfd_und_section
      && relocateable == false)
    return bfd_reloc_undefined;

  /* We have to figure out the gp value, so that we can adjust the
     symbol value correctly.  We look up the symbol _gp in the output
     BFD.  If we can't find it, we're stuck.  We cache it in the ECOFF
     target data.  We don't need to adjust the symbol value for an
     external symbol if we are producing relocateable output.  */
  if (ecoff_data (output_bfd)->gp == 0
      && (relocateable == false
	  || (symbol->flags & BSF_SECTION_SYM) != 0))
    {
      if (relocateable != false)
	{
	  /* Make up a value.  */
	  ecoff_data (output_bfd)->gp =
	    symbol->section->output_section->vma + 0x4000;
	}
      else
	{
	  unsigned int count;
	  asymbol **sym;
	  unsigned int i;

	  count = bfd_get_symcount (output_bfd);
	  sym = bfd_get_outsymbols (output_bfd);

	  /* We should do something more friendly here, but we don't
	     have a good reloc status to return.  */
	  if (sym == (asymbol **) NULL)
	    abort ();

	  for (i = 0; i < count; i++, sym++)
	    {
	      register CONST char *name;

	      name = bfd_asymbol_name (*sym);
	      if (*name == '_' && strcmp (name, "_gp") == 0)
		{
		  ecoff_data (output_bfd)->gp = bfd_asymbol_value (*sym);
		  break;
		}
	    }

	  /* We should do something more friendly here, but we don't have
	     a good reloc status to return.  */
	  if (i >= count)
	    abort ();
	}
    }

  if (bfd_is_com_section (symbol->section))
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address);

  /* Set val to the offset into the section or symbol.  */
  val = ((insn & 0xffff) + reloc_entry->addend) & 0xffff;
  if (val & 0x8000)
    val -= 0x10000;

  /* Adjust val for the final section location and GP value.  If we
     are producing relocateable output, we don't want to do this for
     an external symbol.  */
  if (relocateable == false
      || (symbol->flags & BSF_SECTION_SYM) != 0)
    val += relocation - ecoff_data (output_bfd)->gp;

  insn = (insn &~ 0xffff) | (val & 0xffff);
  bfd_put_32 (abfd, insn, (bfd_byte *) data + reloc_entry->address);

  if (relocateable != false)
    reloc_entry->address += input_section->output_offset;

  /* Make sure it fit in 16 bits.  */
  if (val >= 0x8000 && val < 0xffff8000)
    return bfd_reloc_outofrange;

  return bfd_reloc_ok;
}

/* How to process the various relocs types.  */

static reloc_howto_type ecoff_howto_table[] =
{
  /* Reloc type 0 is ignored.  The reloc reading code ensures that
     this is a reference to the .abs section, which will cause
     bfd_perform_relocation to do nothing.  */
  HOWTO (ECOFF_R_IGNORE,	/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 8,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 false,			/* complain_on_overflow */
	 0,			/* special_function */
	 "IGNORE",		/* name */
	 false,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 16 bit reference to a symbol, normally from a data section.  */
  HOWTO (ECOFF_R_REFHALF,	/* type */
	 0,			/* rightshift */
	 1,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 true,			/* complain_on_overflow */
	 ecoff_generic_reloc,	/* special_function */
	 "REFHALF",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 32 bit reference to a symbol, normally from a data section.  */
  HOWTO (ECOFF_R_REFWORD,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 true,			/* complain_on_overflow */
	 ecoff_generic_reloc,	/* special_function */
	 "REFWORD",		/* name */
	 true,			/* partial_inplace */
	 0xffffffff,		/* src_mask */
	 0xffffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A 26 bit absolute jump address.  */
  HOWTO (ECOFF_R_JMPADDR,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 true,			/* complain_on_overflow */
	 ecoff_generic_reloc,	/* special_function */
	 "JMPADDR",		/* name */
	 true,			/* partial_inplace */
	 0x3ffffff,		/* src_mask */
	 0x3ffffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* The high 16 bits of a symbol value.  Handled by the function
     ecoff_refhi_reloc.  */
  HOWTO (ECOFF_R_REFHI,		/* type */
	 16,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 true,			/* complain_on_overflow */
	 ecoff_refhi_reloc,	/* special_function */
	 "REFHI",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* The low 16 bits of a symbol value.  */
  HOWTO (ECOFF_R_REFLO,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 true,			/* complain_on_overflow */
	 ecoff_reflo_reloc,	/* special_function */
	 "REFLO",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A reference to an offset from the gp register.  Handled by the
     function ecoff_gprel_reloc.  */
  HOWTO (ECOFF_R_GPREL,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 true,			/* complain_on_overflow */
	 ecoff_gprel_reloc,	/* special_function */
	 "GPREL",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false),		/* pcrel_offset */

  /* A reference to a literal using an offset from the gp register.
     Handled by the function ecoff_gprel_reloc.  */
  HOWTO (ECOFF_R_LITERAL,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 32,			/* bitsize (obsolete) */
	 false,			/* pc_relative */
	 0,			/* bitpos */
	 false,			/* absolute (obsolete) */
	 true,			/* complain_on_overflow */
	 ecoff_gprel_reloc,	/* special_function */
	 "LITERAL",		/* name */
	 true,			/* partial_inplace */
	 0xffff,		/* src_mask */
	 0xffff,		/* dst_mask */
	 false)			/* pcrel_offset */
};

#define ECOFF_HOWTO_COUNT \
  (sizeof ecoff_howto_table / sizeof ecoff_howto_table[0])

/* Read in the relocs for a section.  */

static boolean
ecoff_slurp_reloc_table (abfd, section, symbols)
     bfd *abfd;
     asection *section;
     asymbol **symbols;
{
  RELOC *external_relocs;
  arelent *internal_relocs;
  arelent *rptr;
  unsigned int i;

  if (section->relocation != (arelent *) NULL
      || section->reloc_count == 0
      || (section->flags & SEC_CONSTRUCTOR) != 0)
    return true;

  if (ecoff_slurp_symbol_table (abfd) == false)
    return false;
  
  internal_relocs = (arelent *) bfd_alloc (abfd,
					   (sizeof (arelent)
					    * section->reloc_count));
  external_relocs = (RELOC *) bfd_alloc (abfd, RELSZ * section->reloc_count);
  if (internal_relocs == (arelent *) NULL
      || external_relocs == (RELOC *) NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  if (bfd_seek (abfd, section->rel_filepos, SEEK_SET) != 0)
    return false;
  if (bfd_read (external_relocs, 1, RELSZ * section->reloc_count, abfd)
      != RELSZ * section->reloc_count)
    {
      bfd_error = system_call_error;
      return false;
    }

  for (i = 0, rptr = internal_relocs; i < section->reloc_count; i++, rptr++)
    {
      struct internal_reloc intern;

      ecoff_swap_reloc_in (abfd, external_relocs + i, &intern);

      if (intern.r_type > ECOFF_R_LITERAL)
	abort ();

      if (intern.r_extern)
	{
	  /* r_symndx is an index into the external symbols.  */
	  BFD_ASSERT (intern.r_symndx >= 0
		      && (intern.r_symndx
			  < ecoff_data (abfd)->symbolic_header.iextMax));
	  rptr->sym_ptr_ptr = symbols + intern.r_symndx;
	  rptr->addend = 0;
	}
      else
	{
	  CONST char *sec_name;
	  asection *sec;

	  /* r_symndx is a section key.  */
	  switch (intern.r_symndx)
	    {
	    case RELOC_SECTION_TEXT:  sec_name = ".text";  break;
	    case RELOC_SECTION_RDATA: sec_name = ".rdata"; break;
	    case RELOC_SECTION_DATA:  sec_name = ".data";  break;
	    case RELOC_SECTION_SDATA: sec_name = ".sdata"; break;
	    case RELOC_SECTION_SBSS:  sec_name = ".sbss";  break;
	    case RELOC_SECTION_BSS:   sec_name = ".bss";   break;
	    case RELOC_SECTION_INIT:  sec_name = ".init";  break;
	    case RELOC_SECTION_LIT8:  sec_name = ".lit8";  break;
	    case RELOC_SECTION_LIT4:  sec_name = ".lit4";  break;
	    default: abort ();
	    }

	  sec = bfd_get_section_by_name (abfd, sec_name);
	  if (sec == (asection *) NULL)
	    abort ();
	  rptr->sym_ptr_ptr = sec->symbol_ptr_ptr;

	  rptr->addend = - bfd_get_section_vma (abfd, sec);
	  if (intern.r_type == ECOFF_R_GPREL
	      || intern.r_type == ECOFF_R_LITERAL)
	    rptr->addend += ecoff_data (abfd)->gp;
	}

      rptr->address = intern.r_vaddr - bfd_get_section_vma (abfd, section);
      rptr->howto = &ecoff_howto_table[intern.r_type];

      /* If the type is ECOFF_R_IGNORE, make sure this is a reference
	 to the absolute section so that the reloc is ignored.  */
      if (intern.r_type == ECOFF_R_IGNORE)
	rptr->sym_ptr_ptr = bfd_abs_section.symbol_ptr_ptr;
    }

  bfd_release (abfd, external_relocs);

  section->relocation = internal_relocs;

  return true;
}

/* Get a canonical list of relocs.  */

static unsigned int
ecoff_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     asection *section;
     arelent **relptr;
     asymbol **symbols;
{
  unsigned int count;

  if (section->flags & SEC_CONSTRUCTOR) 
    {
      arelent_chain *chain;

      /* This section has relocs made up by us, not the file, so take
	 them out of their chain and place them into the data area
	 provided.  */
      for (count = 0, chain = section->constructor_chain;
	   count < section->reloc_count;
	   count++, chain = chain->next)
	*relptr++ = &chain->relent;
    }
  else
    { 
      arelent *tblptr;

      if (ecoff_slurp_reloc_table (abfd, section, symbols) == false)
	return 0;

      tblptr = section->relocation;
      if (tblptr == (arelent *) NULL)
	return 0;

      for (count = 0; count < section->reloc_count; count++)
	*relptr++ = tblptr++;
    }

  *relptr = (arelent *) NULL;

  return section->reloc_count;
}

/* Get the howto structure for a generic reloc type.  */

static CONST struct reloc_howto_struct *
ecoff_bfd_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  int ecoff_type;

  switch (code)
    {
    case BFD_RELOC_16:
      ecoff_type = ECOFF_R_REFHALF;
      break;
    case BFD_RELOC_32:
      ecoff_type = ECOFF_R_REFWORD;
      break;
    case BFD_RELOC_MIPS_JMP:
      ecoff_type = ECOFF_R_JMPADDR;
      break;
    case BFD_RELOC_HI16_S:
      ecoff_type = ECOFF_R_REFHI;
      break;
    case BFD_RELOC_LO16:
      ecoff_type = ECOFF_R_REFLO;
      break;
    case BFD_RELOC_MIPS_GPREL:
      ecoff_type = ECOFF_R_GPREL;
      break;
    default:
      return (CONST struct reloc_howto_struct *) NULL;
    }

  return &ecoff_howto_table[ecoff_type];
}

/* Provided a BFD, a section and an offset into the section, calculate
   and return the name of the source file and the line nearest to the
   wanted location.  */

static boolean
ecoff_find_nearest_line (abfd,
			 section,
			 ignore_symbols,
			 offset,
			 filename_ptr,
			 functionname_ptr,
			 retline_ptr)
     bfd *abfd;
     asection *section;
     asymbol **ignore_symbols;
     bfd_vma offset;
     CONST char **filename_ptr;
     CONST char **functionname_ptr;
     unsigned int *retline_ptr;
{
  FDR *fdr_ptr;
  FDR *fdr_start;
  FDR *fdr_end;
  FDR *fdr_hold;
  struct pdr_ext *pdr_ptr;
  struct pdr_ext *pdr_end;
  PDR pdr;
  unsigned char *line_ptr;
  unsigned char *line_end;
  int lineno;

  /* If we're not in the .text section, we don't have any line
     numbers.  */
  if (strcmp (section->name, _TEXT) != 0
      || offset < ecoff_data (abfd)->text_start
      || offset >= ecoff_data (abfd)->text_end)
    return false;

  /* Make sure we have the FDR's.  */
  if (ecoff_slurp_symbolic_info (abfd) == false
      || bfd_get_symcount (abfd) == 0)
    return false;

  /* Each file descriptor (FDR) has a memory address.  Here we track
     down which FDR we want.  The FDR's are stored in increasing
     memory order.  If speed is ever important, this can become a
     binary search.  We must ignore FDR's with no PDR entries; they
     will have the adr of the FDR before or after them.  */
  fdr_start = ecoff_data (abfd)->fdr;
  fdr_end = fdr_start + ecoff_data (abfd)->symbolic_header.ifdMax;
  fdr_hold = (FDR *) NULL;
  for (fdr_ptr = fdr_start; fdr_ptr < fdr_end; fdr_ptr++)
    {
      if (fdr_ptr->cpd == 0)
	continue;
      if (offset < fdr_ptr->adr)
	break;
      fdr_hold = fdr_ptr;
    }
  if (fdr_hold == (FDR *) NULL)
    return false;
  fdr_ptr = fdr_hold;

  /* Each FDR has a list of procedure descriptors (PDR).  PDR's also
     have an address, which is relative to the FDR address, and are
     also stored in increasing memory order.  */
  offset -= fdr_ptr->adr;
  pdr_ptr = ecoff_data (abfd)->external_pdr + fdr_ptr->ipdFirst;
  pdr_end = pdr_ptr + fdr_ptr->cpd;
  ecoff_swap_pdr_in (abfd, pdr_ptr, &pdr);

  /* The address of the first PDR is an offset which applies to the
     addresses of all the PDR's.  */
  offset += pdr.adr;

  for (pdr_ptr++; pdr_ptr < pdr_end; pdr_ptr++)
    {
      ecoff_swap_pdr_in (abfd, pdr_ptr, &pdr);
      if (offset < pdr.adr)
	break;
    }

  /* Now we can look for the actual line number.  The line numbers are
     stored in a very funky format, which I won't try to describe.
     Note that right here pdr_ptr and pdr hold the PDR *after* the one
     we want; we need this to compute line_end.  */
  line_end = ecoff_data (abfd)->line;
  if (pdr_ptr == pdr_end)
    line_end += fdr_ptr->cbLineOffset + fdr_ptr->cbLine;
  else
    line_end += fdr_ptr->cbLineOffset + pdr.cbLineOffset;

  /* Now change pdr and pdr_ptr to the one we want.  */
  pdr_ptr--;
  ecoff_swap_pdr_in (abfd, pdr_ptr, &pdr);

  offset -= pdr.adr;
  lineno = pdr.lnLow;
  line_ptr = (ecoff_data (abfd)->line
	      + fdr_ptr->cbLineOffset
	      + pdr.cbLineOffset);
  while (line_ptr < line_end)
    {
      int delta;
      int count;

      delta = *line_ptr >> 4;
      if (delta >= 0x8)
	delta -= 0x10;
      count = (*line_ptr & 0xf) + 1;
      ++line_ptr;
      if (delta == -8)
	{
	  delta = (((line_ptr[0]) & 0xff) << 8) + ((line_ptr[1]) & 0xff);
	  if (delta >= 0x8000)
	    delta -= 0x10000;
	  line_ptr += 2;
	}
      lineno += delta;
      if (offset < count * 4)
	break;
      offset -= count * 4;
    }

  /* If fdr_ptr->rss is -1, then this file does not have full symbols,
     at least according to gdb/mipsread.c.  */
  if (fdr_ptr->rss == -1)
    {
      *filename_ptr = NULL;
      if (pdr.isym == -1)
	*functionname_ptr = NULL;
      else
	{
	  EXTR proc_ext;

	  ecoff_swap_ext_in (abfd,
			     (ecoff_data (abfd)->external_ext
			      + pdr.isym),
			     &proc_ext);
	  *functionname_ptr = ecoff_data (abfd)->ssext + proc_ext.asym.iss;
	}
    }
  else
    {
      SYMR proc_sym;

      *filename_ptr = ecoff_data (abfd)->ss + fdr_ptr->issBase + fdr_ptr->rss;
      ecoff_swap_sym_in (abfd,
			 (ecoff_data (abfd)->external_sym
			  + fdr_ptr->isymBase
			  + pdr.isym),
			 &proc_sym);
      *functionname_ptr = (ecoff_data (abfd)->ss
			   + fdr_ptr->issBase
			   + proc_sym.iss);
    }
  *retline_ptr = lineno;
  return true;
}

/* We can't use the generic linking routines for ECOFF, because we
   have to handle all the debugging information.  The generic link
   routine just works out the section contents and attaches a list of
   symbols.

   We link by looping over all the seclets.  We make two passes.  On
   the first we set the actual section contents and determine the size
   of the debugging information.  On the second we accumulate the
   debugging information and write it out.

   This currently always accumulates the debugging information, which
   is incorrect, because it ignores the -s and -S options of the
   linker.  The linker needs to be modified to give us that
   information in a more useful format (currently it just provides a
   list of symbols which should appear in the output file).  */

/* Clear the output_has_begun flag for all the input BFD's.  We use it
   to avoid linking in the debugging information for a BFD more than
   once.  */

static void
ecoff_clear_output_flags (abfd)
     bfd *abfd;
{
  register asection *o;
  register bfd_seclet_type *p;

  for (o = abfd->sections; o != (asection *) NULL; o = o->next)
    for (p = o->seclets_head;
	 p != (bfd_seclet_type *) NULL;
	 p = p->next)
      if (p->type == bfd_indirect_seclet)
	p->u.indirect.section->owner->output_has_begun = false;
}

/* Handle an indirect seclet on the first pass.  Set the contents of
   the output section, and accumulate the debugging information if
   any.  */

static boolean
ecoff_rel (output_bfd, seclet, output_section, data, relocateable)
     bfd *output_bfd;
     bfd_seclet_type *seclet;
     asection *output_section;
     PTR data;
     boolean relocateable;
{
  bfd *input_bfd;
  HDRR *output_symhdr;
  HDRR *input_symhdr;

  if ((output_section->flags & SEC_HAS_CONTENTS)
      && !(output_section->flags & SEC_NEVER_LOAD)
      && (output_section->flags & SEC_LOAD)
      && seclet->size)
    {
      data = (PTR) bfd_get_relocated_section_contents (output_bfd,
						       seclet,
						       data,
						       relocateable);
      if (bfd_set_section_contents (output_bfd,
				    output_section,
				    data,
				    seclet->offset,
				    seclet->size)
	  == false)
	{
	  abort();
	}
    }

  input_bfd = seclet->u.indirect.section->owner;

  /* We want to figure out how much space will be required to
     incorporate all the debugging information from input_bfd.  We use
     the output_has_begun field to avoid adding it in more than once.
     The actual incorporation is done in the second pass, in
     ecoff_get_debug.  The code has to parallel that code in its
     manipulations of output_symhdr.  */

  if (input_bfd->output_has_begun)
    return true;
  input_bfd->output_has_begun = true;

  output_symhdr = &ecoff_data (output_bfd)->symbolic_header;

  if (input_bfd->xvec->flavour != bfd_target_ecoff_flavour)
    {
      asymbol **symbols;
      asymbol **sym_ptr;
      asymbol **sym_end;

      /* We just accumulate local symbols from a non-ECOFF BFD.  The
	 external symbols are handled separately.  */

      symbols = (asymbol **) bfd_alloc (output_bfd,
					get_symtab_upper_bound (input_bfd));
      if (symbols == (asymbol **) NULL)
	{
	  bfd_error = no_memory;
	  return false;
	}
      sym_end = symbols + bfd_canonicalize_symtab (input_bfd, symbols);

      for (sym_ptr = symbols; sym_ptr < sym_end; sym_ptr++)
	{
	  size_t len;

	  len = strlen ((*sym_ptr)->name);
	  if (((*sym_ptr)->flags & BSF_EXPORT) == 0)
	    {
	      ++output_symhdr->isymMax;
	      output_symhdr->issMax += len + 1;
	    }
	}

      bfd_release (output_bfd, (PTR) symbols);

      ++output_symhdr->ifdMax;

      return true;
    }

  /* We simply add in the information from another ECOFF BFD.  First
     we make sure we have the symbolic information.  */
  if (ecoff_slurp_symbol_table (input_bfd) == false)
    return false;
  if (bfd_get_symcount (input_bfd) == 0)
    return true;

  input_symhdr = &ecoff_data (input_bfd)->symbolic_header;

  /* Figure out how much information we are going to be putting in.
     The external symbols are handled separately.  */
  output_symhdr->ilineMax += input_symhdr->ilineMax;
  output_symhdr->cbLine += input_symhdr->cbLine;
  output_symhdr->idnMax += input_symhdr->idnMax;
  output_symhdr->ipdMax += input_symhdr->ipdMax;
  output_symhdr->isymMax += input_symhdr->isymMax;
  output_symhdr->ioptMax += input_symhdr->ioptMax;
  output_symhdr->iauxMax += input_symhdr->iauxMax;
  output_symhdr->issMax += input_symhdr->issMax;
  output_symhdr->ifdMax += input_symhdr->ifdMax;

  /* The RFD's are special, since we create them if needed.  */
  if (input_symhdr->crfd > 0)
    output_symhdr->crfd += input_symhdr->crfd;
  else
    output_symhdr->crfd += input_symhdr->ifdMax;

  return true;
}

/* Handle an arbitrary seclet on the first pass.  */

static boolean
ecoff_dump_seclet (abfd, seclet, section, data, relocateable)
     bfd *abfd;
     bfd_seclet_type *seclet;
     asection *section;
     PTR data;
     boolean relocateable;
{
  switch (seclet->type) 
    {
    case bfd_indirect_seclet:
      /* The contents of this section come from another one somewhere
	 else.  */
      return ecoff_rel (abfd, seclet, section, data, relocateable);

    case bfd_fill_seclet:
      /* Fill in the section with fill.value.  This is used to pad out
	 sections, but we must avoid padding the .bss section.  */
      if ((section->flags & SEC_HAS_CONTENTS) == 0)
	{
	  if (seclet->u.fill.value != 0)
	    abort ();
	}
      else
	{
	  char *d = (char *) bfd_alloc (abfd, seclet->size);
	  unsigned int i;
	  boolean ret;

	  for (i = 0; i < seclet->size; i+=2)
	    d[i] = seclet->u.fill.value >> 8;
	  for (i = 1; i < seclet->size; i+=2)
	    d[i] = seclet->u.fill.value;
	  ret = bfd_set_section_contents (abfd, section, d, seclet->offset,
					  seclet->size);
	  bfd_release (abfd, (PTR) d);
	  return ret;
	}
      break;

    default:
      abort();
    }

  return true;
}

/* Add a string to the debugging information we are accumulating for a
   file.  Return the offset from the fdr string base or from the
   external string base.  */

static long
ecoff_add_string (output_bfd, fdr, string, external)
     bfd *output_bfd;
     FDR *fdr;
     CONST char *string;
     boolean external;
{
  HDRR *symhdr;
  size_t len;
  long ret;

  symhdr = &ecoff_data (output_bfd)->symbolic_header;
  len = strlen (string);
  if (external)
    {
      strcpy (ecoff_data (output_bfd)->ssext + symhdr->issExtMax, string);
      ret = symhdr->issExtMax;
      symhdr->issExtMax += len + 1;
    }
  else
    {
      strcpy (ecoff_data (output_bfd)->ss + symhdr->issMax, string);
      ret = fdr->cbSs;
      symhdr->issMax += len + 1;
      fdr->cbSs += len + 1;
    }
  return ret;
}

/* Accumulate the debugging information from an input section.  */

static boolean
ecoff_get_debug (output_bfd, seclet, section, relocateable)
     bfd *output_bfd;
     bfd_seclet_type *seclet;
     asection *section;
     boolean relocateable;
{
  bfd *input_bfd;
  HDRR *output_symhdr;
  HDRR *input_symhdr;
  ecoff_data_type *output_ecoff;
  ecoff_data_type *input_ecoff;
  unsigned int count;
  struct sym_ext *sym_out;
  ecoff_symbol_type *esym_ptr;
  ecoff_symbol_type *esym_end;
  unsigned long pdr_off;
  FDR *fdr_ptr;
  FDR *fdr_end;
  struct fdr_ext *fdr_out;

  input_bfd = seclet->u.indirect.section->owner;

  /* Don't get the information more than once. */
  if (input_bfd->output_has_begun)
    return true;
  input_bfd->output_has_begun = true;

  output_ecoff = ecoff_data (output_bfd);
  output_symhdr = &output_ecoff->symbolic_header;

  if (input_bfd->xvec->flavour != bfd_target_ecoff_flavour)
    {
      FDR fdr;
      asymbol **symbols;
      asymbol **sym_ptr;
      asymbol **sym_end;

      /* This is not an ECOFF BFD.  Just gather the symbols.  */

      memset (&fdr, 0, sizeof fdr);

      fdr.adr = bfd_get_section_vma (output_bfd, section) + seclet->offset;
      fdr.issBase = output_symhdr->issMax;
      fdr.cbSs = 0;
      fdr.rss = ecoff_add_string (output_bfd,
				  &fdr,
				  bfd_get_filename (input_bfd),
				  false);
      fdr.isymBase = output_symhdr->isymMax;

      /* Get the local symbols from the input BFD.  */
      symbols = (asymbol **) bfd_alloc (output_bfd,
					get_symtab_upper_bound (input_bfd));
      if (symbols == (asymbol **) NULL)
	{
	  bfd_error = no_memory;
	  return false;
	}
      sym_end = symbols + bfd_canonicalize_symtab (input_bfd, symbols);

      /* Handle the local symbols.  Any external symbols are handled
	 separately.  */
      fdr.csym = 0;
      for (sym_ptr = symbols; sym_ptr != sym_end; sym_ptr++)
	{
	  SYMR internal_sym;

	  if (((*sym_ptr)->flags & BSF_EXPORT) != 0)
	    continue;
	  memset (&internal_sym, 0, sizeof internal_sym);
	  internal_sym.iss = ecoff_add_string (output_bfd,
					       &fdr,
					       (*sym_ptr)->name,
					       false);

	  if (bfd_is_com_section ((*sym_ptr)->section)
	      || (*sym_ptr)->section == &bfd_und_section)
	    internal_sym.value = (*sym_ptr)->value;
	  else
	    internal_sym.value = ((*sym_ptr)->value
				  + (*sym_ptr)->section->output_offset
				  + (*sym_ptr)->section->output_section->vma);
	  internal_sym.st = stNil;
	  internal_sym.sc = scUndefined;
	  internal_sym.index = indexNil;
	  ecoff_swap_sym_out (output_bfd, &internal_sym,
			      (output_ecoff->external_sym
			       + output_symhdr->isymMax));
	  ++fdr.csym;
	  ++output_symhdr->isymMax;
	}

      bfd_release (output_bfd, (PTR) symbols);

      /* Leave everything else in the FDR zeroed out.  This will cause
	 the lang field to be langC.  The fBigendian field will
	 indicate little endian format, but it doesn't matter because
	 it only applies to aux fields and there are none.  */

      ecoff_swap_fdr_out (output_bfd, &fdr,
			  (output_ecoff->external_fdr
			   + output_symhdr->ifdMax));
      ++output_symhdr->ifdMax;
      return true;
    }

  /* This is an ECOFF BFD.  We want to grab the information from
     input_bfd and attach it to output_bfd.  */
  count = bfd_get_symcount (input_bfd);
  if (count == 0)
    return true;
  input_ecoff = ecoff_data (input_bfd);
  input_symhdr = &input_ecoff->symbolic_header;

  /* I think that it is more efficient to simply copy the debugging
     information from the input BFD to the output BFD.  Because ECOFF
     uses relative pointers for most of the debugging information,
     only a little of it has to be changed at all.  */

  /* Swap in the local symbols, adjust their values, and swap them out
     again.  The external symbols are handled separately.  */
  sym_out = output_ecoff->external_sym + output_symhdr->isymMax;

  esym_ptr = ecoff_data (input_bfd)->canonical_symbols;
  esym_end = esym_ptr + count;
  for (; esym_ptr < esym_end; esym_ptr++)
    {
      if (esym_ptr->local)
	{
	  SYMR sym;

	  ecoff_swap_sym_in (input_bfd, esym_ptr->native.lnative, &sym);

	  /* If we're producing an executable, move common symbols
	     into bss.  */
	  if (relocateable == false)
	    {
	      if (sym.sc == scCommon)
		sym.sc = scBss;
	      else if (sym.sc == scSCommon)
		sym.sc = scSBss;
	    }

	  if (! bfd_is_com_section (esym_ptr->symbol.section)
	      && (esym_ptr->symbol.flags & BSF_DEBUGGING) == 0
	      && esym_ptr->symbol.section != &bfd_und_section)
	    sym.value = (esym_ptr->symbol.value
			 + esym_ptr->symbol.section->output_offset
			 + esym_ptr->symbol.section->output_section->vma);
	  ecoff_swap_sym_out (output_bfd, &sym, sym_out);
	  ++sym_out;
	}
    }

  /* That should have accounted for all the local symbols in
     input_bfd.  */
  BFD_ASSERT ((sym_out - output_ecoff->external_sym) - output_symhdr->isymMax
	      == input_symhdr->isymMax);

  /* Copy the information that does not need swapping.  */
  memcpy (output_ecoff->line + output_symhdr->cbLine,
	  input_ecoff->line,
	  input_symhdr->cbLine * sizeof (unsigned char));
  memcpy (output_ecoff->external_aux + output_symhdr->iauxMax,
	  input_ecoff->external_aux,
	  input_symhdr->iauxMax * sizeof (union aux_ext));
  memcpy (output_ecoff->ss + output_symhdr->issMax,
	  input_ecoff->ss,
	  input_symhdr->issMax * sizeof (char));

  /* Some of the information may need to be swapped.  */
  if (output_bfd->xvec->header_byteorder_big_p
      == input_bfd->xvec->header_byteorder_big_p)
    {
      /* The two BFD's have the same endianness, so memcpy will
	 suffice.  */
      memcpy (output_ecoff->external_dnr + output_symhdr->idnMax,
	      input_ecoff->external_dnr,
	      input_symhdr->idnMax * sizeof (struct dnr_ext));
      memcpy (output_ecoff->external_pdr + output_symhdr->ipdMax,
	      input_ecoff->external_pdr,
	      input_symhdr->ipdMax * sizeof (struct pdr_ext));
      if (input_symhdr->ipdMax == 0)
	pdr_off = 0;
      else
	{
	  PDR pdr;

	  ecoff_swap_pdr_in (input_bfd, input_ecoff->external_pdr, &pdr);
	  pdr_off = pdr.adr;
	}
      memcpy (output_ecoff->external_opt + output_symhdr->ioptMax,
	      input_ecoff->external_opt,
	      input_symhdr->ioptMax * sizeof (struct opt_ext));
    }
  else
    {
      struct dnr_ext *dnr_in;
      struct dnr_ext *dnr_end;
      struct dnr_ext *dnr_out;
      struct pdr_ext *pdr_in;
      struct pdr_ext *pdr_end;
      struct pdr_ext *pdr_out;
      int first_pdr;
      struct opt_ext *opt_in;
      struct opt_ext *opt_end;
      struct opt_ext *opt_out;

      /* The two BFD's have different endianness, so we must swap
	 everything in and out.  This code would always work, but it
	 would be slow in the normal case.  */
      dnr_in = input_ecoff->external_dnr;
      dnr_end = dnr_in + input_symhdr->idnMax;
      dnr_out = output_ecoff->external_dnr + output_symhdr->idnMax;
      for (; dnr_in < dnr_end; dnr_in++, dnr_out++)
	{
	  DNR dnr;

	  ecoff_swap_dnr_in (input_bfd, dnr_in, &dnr);
	  ecoff_swap_dnr_out (output_bfd, &dnr, dnr_out);
	}
      pdr_in = input_ecoff->external_pdr;
      pdr_end = pdr_in + input_symhdr->ipdMax;
      pdr_out = output_ecoff->external_pdr + output_symhdr->ipdMax;
      first_pdr = 1;
      pdr_off = 0;
      for (; pdr_in < pdr_end; pdr_in++, pdr_out++)
	{
	  PDR pdr;

	  ecoff_swap_pdr_in (input_bfd, pdr_in, &pdr);
	  ecoff_swap_pdr_out (output_bfd, &pdr, pdr_out);
	  if (first_pdr)
	    {
	      pdr_off = pdr.adr;
	      first_pdr = 0;
	    }
	}
      opt_in = input_ecoff->external_opt;
      opt_end = opt_in + input_symhdr->ioptMax;
      opt_out = output_ecoff->external_opt + output_symhdr->ioptMax;
      for (; opt_in < opt_end; opt_in++, opt_out++)
	{
	  OPTR opt;

	  ecoff_swap_opt_in (input_bfd, opt_in, &opt);
	  ecoff_swap_opt_out (output_bfd, &opt, opt_out);
	}
    }

  /* Set ifdbase so that the external symbols know how to adjust their
     ifd values.  */
  input_ecoff->ifdbase = output_symhdr->ifdMax;

  fdr_ptr = input_ecoff->fdr;
  fdr_end = fdr_ptr + input_symhdr->ifdMax;
  fdr_out = output_ecoff->external_fdr + output_symhdr->ifdMax;
  for (; fdr_ptr < fdr_end; fdr_ptr++, fdr_out++)
    {
      FDR fdr;

      fdr = *fdr_ptr;

      /* The memory address for this fdr is the address for the seclet
	 plus the offset to this fdr within input_bfd.  For some
	 reason the offset of the first procedure pointer is also
	 added in.  */
      fdr.adr = (bfd_get_section_vma (output_bfd, section)
		 + seclet->offset
		 + (fdr_ptr->adr - input_ecoff->fdr->adr)
		 + pdr_off);

      fdr.issBase += output_symhdr->issMax;
      fdr.isymBase += output_symhdr->isymMax;
      fdr.ilineBase += output_symhdr->ilineMax;
      fdr.ioptBase += output_symhdr->ioptMax;
      fdr.ipdFirst += output_symhdr->ipdMax;
      fdr.iauxBase += output_symhdr->iauxMax;
      fdr.rfdBase += output_symhdr->crfd;

      /* If there are no RFD's, we are going to add some.  We don't
	 want to adjust irfd for this, so that all the FDR's can share
	 the RFD's.  */
      if (input_symhdr->crfd == 0)
	fdr.crfd = input_symhdr->ifdMax;

      if (fdr.cbLine != 0)
	fdr.cbLineOffset += output_symhdr->cbLine;

      ecoff_swap_fdr_out (output_bfd, &fdr, fdr_out);
    }

  if (input_symhdr->crfd > 0)
    {
      struct rfd_ext *rfd_in;
      struct rfd_ext *rfd_end;
      struct rfd_ext *rfd_out;

      /* Swap and adjust the RFD's.  RFD's are only created by the
	 linker, so this will only be necessary if one of the input
	 files is the result of a partial link.  Presumably all
	 necessary RFD's are present.  */
      rfd_in = input_ecoff->external_rfd;
      rfd_end = rfd_in + input_symhdr->crfd;
      rfd_out = output_ecoff->external_rfd + output_symhdr->crfd;
      for (; rfd_in < rfd_end; rfd_in++, rfd_out++)
	{
	  RFDT rfd;

	  ecoff_swap_rfd_in (input_bfd, rfd_in, &rfd);
	  rfd += output_symhdr->ifdMax;
	  ecoff_swap_rfd_out (output_bfd, &rfd, rfd_out);
	}
      output_symhdr->crfd += input_symhdr->crfd;
    }
  else
    {
      struct rfd_ext *rfd_out;
      struct rfd_ext *rfd_end;
      RFDT rfd;

      /* Create RFD's.  Some of the debugging information includes
	 relative file indices.  These indices are taken as indices to
	 the RFD table if there is one, or to the global table if
	 there is not.  If we did not create RFD's, we would have to
	 parse and adjust all the debugging information which contains
	 file indices.  */
      rfd = output_symhdr->ifdMax;
      rfd_out = output_ecoff->external_rfd + output_symhdr->crfd;
      rfd_end = rfd_out + input_symhdr->ifdMax;
      for (; rfd_out < rfd_end; rfd_out++, rfd++)
	ecoff_swap_rfd_out (output_bfd, &rfd, rfd_out);
      output_symhdr->crfd += input_symhdr->ifdMax;
    }

  /* Combine the register masks.  */
  {
    int i;

    output_ecoff->gprmask |= input_ecoff->gprmask;
    for (i = 0; i < 4; i++)
      output_ecoff->cprmask[i] |= input_ecoff->cprmask[i];
  }

  /* Update the counts.  */
  output_symhdr->ilineMax += input_symhdr->ilineMax;
  output_symhdr->cbLine += input_symhdr->cbLine;
  output_symhdr->idnMax += input_symhdr->idnMax;
  output_symhdr->ipdMax += input_symhdr->ipdMax;
  output_symhdr->isymMax += input_symhdr->isymMax;
  output_symhdr->ioptMax += input_symhdr->ioptMax;
  output_symhdr->iauxMax += input_symhdr->iauxMax;
  output_symhdr->issMax += input_symhdr->issMax;
  output_symhdr->ifdMax += input_symhdr->ifdMax;

  return true;
}

/* This is the actual link routine.  It makes two passes over all the
   seclets.  */

static boolean
ecoff_bfd_seclet_link (abfd, data, relocateable)
     bfd *abfd;
     PTR data;
     boolean relocateable;
{
  HDRR *symhdr;
  int ipass;
  register asection *o;
  register bfd_seclet_type *p;
  asymbol **sym_ptr_ptr;
  bfd_size_type size;
  char *raw;

  /* We accumulate the debugging information counts in the symbolic
     header.  */
  symhdr = &ecoff_data (abfd)->symbolic_header;
  symhdr->magic = magicSym;
  /* FIXME: What should the version stamp be?  */
  symhdr->vstamp = 0;
  symhdr->ilineMax = 0;
  symhdr->cbLine = 0;
  symhdr->idnMax = 0;
  symhdr->ipdMax = 0;
  symhdr->isymMax = 0;
  symhdr->ioptMax = 0;
  symhdr->iauxMax = 0;
  symhdr->issMax = 0;
  symhdr->issExtMax = 0;
  symhdr->ifdMax = 0;
  symhdr->crfd = 0;
  symhdr->iextMax = 0;

  /* We need to copy over the debugging symbols from each input BFD.
     When we do this copying, we have to adjust the text address in
     the FDR structures, so we have to know the text address used for
     the input BFD.  Since we only want to copy the symbols once per
     input BFD, but we are going to look at each input BFD multiple
     times (once for each section it provides), we arrange to always
     look at the text section first.  That means that when we copy the
     debugging information, we always know the text address.  So we
     actually do each pass in two sub passes; first the text sections,
     then the non-text sections.  We use the output_has_begun flag to
     determine whether we have copied over the debugging information
     yet.  */

  /* Do the first pass: set the output section contents and count the
     debugging information.  */
  ecoff_clear_output_flags (abfd);
  for (ipass = 0; ipass < 2; ipass++)
    {
      for (o = abfd->sections; o != (asection *) NULL; o = o->next)
	{
	  /* For SEC_CODE sections, (flags & SEC_CODE) == 0 is false,
	     so they are done on pass 0.  For other sections the
	     expression is true, so they are done on pass 1.  */
	  if (((o->flags & SEC_CODE) == 0) != ipass)
	    continue;

	  for (p = o->seclets_head;
	       p != (bfd_seclet_type *) NULL;
	       p = p->next)
	    {
	      if (ecoff_dump_seclet (abfd, p, o, data, relocateable)
		  == false)
		return false;
	    }
	}
    }

  /* We handle the external symbols differently.  We use the ones
     attached to the output_bfd.  The linker will have already
     determined which symbols are to be attached.  Here we just
     determine how much space we will need for them.  */
  sym_ptr_ptr = bfd_get_outsymbols (abfd);
  if (sym_ptr_ptr != NULL)
    {
      asymbol **sym_end;

      sym_end = sym_ptr_ptr + bfd_get_symcount (abfd);
      for (; sym_ptr_ptr < sym_end; sym_ptr_ptr++)
	{
	  if (((*sym_ptr_ptr)->flags & BSF_DEBUGGING) == 0
	      && ((*sym_ptr_ptr)->flags & BSF_LOCAL) == 0)
	    {
	      ++symhdr->iextMax;
	      symhdr->issExtMax += strlen ((*sym_ptr_ptr)->name) + 1;
	    }
	}
    }

  /* Adjust the counts so that structures are longword aligned.  */
  symhdr->cbLine = (symhdr->cbLine + 3) &~ 3;
  symhdr->issMax = (symhdr->issMax + 3) &~ 3;
  symhdr->issExtMax = (symhdr->issExtMax + 3) &~ 3;

  /* Now the counts in symhdr are the correct size for the debugging
     information.  We allocate the right amount of space, and reset
     the counts so that the second pass can use them as indices.  It
     would be possible to output the debugging information directly to
     the file in pass 2, rather than to build it in memory and then
     write it out.  Outputting to the file would require a lot of
     seeks and small writes, though, and I think this approach is
     faster.  */
  size = (symhdr->cbLine * sizeof (unsigned char)
	  + symhdr->idnMax * sizeof (struct dnr_ext)
	  + symhdr->ipdMax * sizeof (struct pdr_ext)
	  + symhdr->isymMax * sizeof (struct sym_ext)
	  + symhdr->ioptMax * sizeof (struct opt_ext)
	  + symhdr->iauxMax * sizeof (union aux_ext)
	  + symhdr->issMax * sizeof (char)
	  + symhdr->issExtMax * sizeof (char)
	  + symhdr->ifdMax * sizeof (struct fdr_ext)
	  + symhdr->crfd * sizeof (struct rfd_ext)
	  + symhdr->iextMax * sizeof (struct ext_ext));
  raw = (char *) bfd_alloc (abfd, size);
  if (raw == (char *) NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  ecoff_data (abfd)->raw_size = size;
  ecoff_data (abfd)->raw_syments = (PTR) raw;

  /* Initialize the raw pointers.  */
#define SET(field, count, type) \
  ecoff_data (abfd)->field = (type *) raw; \
  raw += symhdr->count * sizeof (type)

  SET (line, cbLine, unsigned char);
  SET (external_dnr, idnMax, struct dnr_ext);
  SET (external_pdr, ipdMax, struct pdr_ext);
  SET (external_sym, isymMax, struct sym_ext);
  SET (external_opt, ioptMax, struct opt_ext);
  SET (external_aux, iauxMax, union aux_ext);
  SET (ss, issMax, char);
  SET (ssext, issExtMax, char);
  SET (external_fdr, ifdMax, struct fdr_ext);
  SET (external_rfd, crfd, struct rfd_ext);
  SET (external_ext, iextMax, struct ext_ext);
#undef SET

  /* Reset the counts so the second pass can use them to know how far
     it has gotten.  */
  symhdr->ilineMax = 0;
  symhdr->cbLine = 0;
  symhdr->idnMax = 0;
  symhdr->ipdMax = 0;
  symhdr->isymMax = 0;
  symhdr->ioptMax = 0;
  symhdr->iauxMax = 0;
  symhdr->issMax = 0;
  symhdr->issExtMax = 0;
  symhdr->ifdMax = 0;
  symhdr->crfd = 0;
  symhdr->iextMax = 0;

  /* Do the second pass: accumulate the debugging information.  */
  ecoff_clear_output_flags (abfd);
  for (ipass = 0; ipass < 2; ipass++)
    {
      for (o = abfd->sections; o != (asection *) NULL; o = o->next)
	{
	  if (((o->flags & SEC_CODE) == 0) != ipass)
	    continue;
	  for (p = o->seclets_head;
	       p != (bfd_seclet_type *) NULL;
	       p = p->next)
	    {
	      if (p->type == bfd_indirect_seclet)
		{
		  if (ecoff_get_debug (abfd, p, o, relocateable) == false)
		    return false;
		}
	    }
	}
    }

  /* Put in the external symbols.  */
  sym_ptr_ptr = bfd_get_outsymbols (abfd);
  if (sym_ptr_ptr != NULL)
    {
      char *ssext;
      struct ext_ext *external_ext;

      ssext = ecoff_data (abfd)->ssext;
      external_ext = ecoff_data (abfd)->external_ext;
      for (; *sym_ptr_ptr != NULL; sym_ptr_ptr++)
	{
	  asymbol *sym_ptr;
	  EXTR esym;

	  sym_ptr = *sym_ptr_ptr;

	  if ((sym_ptr->flags & BSF_DEBUGGING) != 0
	      || (sym_ptr->flags & BSF_LOCAL) != 0)
	    continue;

	  /* The enative pointer can be NULL for a symbol created by
	     the linker via ecoff_make_empty_symbol.  */
	  if (bfd_asymbol_flavour (sym_ptr) != bfd_target_ecoff_flavour
	      || (((ecoff_symbol_type *) sym_ptr)->native.enative
		  == (struct ext_ext *) NULL))
	    {
	      esym.jmptbl = 0;
	      esym.cobol_main = 0;
	      esym.weakext = 0;
	      esym.reserved = 0;
	      esym.ifd = ifdNil;
	      /* FIXME: we can do better than this for st and sc.  */
	      esym.asym.st = stGlobal;
	      esym.asym.sc = scAbs;
	      esym.asym.reserved = 0;
	      esym.asym.index = indexNil;
	    }
	  else
	    {
	      ecoff_symbol_type *ecoff_sym_ptr;

	      ecoff_sym_ptr = (ecoff_symbol_type *) sym_ptr;
	      if (ecoff_sym_ptr->local)
		abort ();
	      ecoff_swap_ext_in (abfd, ecoff_sym_ptr->native.enative, &esym);

	      /* If we're producing an executable, move common symbols
		 into bss.  */
	      if (relocateable == false)
		{
		  if (esym.asym.sc == scCommon)
		    esym.asym.sc = scBss;
		  else if (esym.asym.sc == scSCommon)
		    esym.asym.sc = scSBss;
		}

	      /* Adjust the FDR index for the symbol by that used for
		 the input BFD.  */
	      esym.ifd += ecoff_data (bfd_asymbol_bfd (sym_ptr))->ifdbase;
	    }

	  esym.asym.iss = symhdr->issExtMax;

	  if (bfd_is_com_section (sym_ptr->section)
	      || sym_ptr->section == &bfd_und_section)
	    esym.asym.value = sym_ptr->value;
	  else
	    esym.asym.value = (sym_ptr->value
			       + sym_ptr->section->output_offset
			       + sym_ptr->section->output_section->vma);

	  ecoff_swap_ext_out (abfd, &esym, external_ext + symhdr->iextMax);

	  ecoff_set_sym_index (sym_ptr, symhdr->iextMax);

	  ++symhdr->iextMax;

	  strcpy (ssext + symhdr->issExtMax, sym_ptr->name);
	  symhdr->issExtMax += strlen (sym_ptr->name) + 1;
	}
    }

  /* Adjust the counts so that structures are longword aligned.  */
  symhdr->cbLine = (symhdr->cbLine + 3) &~ 3;
  symhdr->issMax = (symhdr->issMax + 3) &~ 3;
  symhdr->issExtMax = (symhdr->issExtMax + 3) &~ 3;

  return true;
}

/* Set the architecture.  The only architecture we support here is
   mips.  We set the architecture anyhow, since many callers ignore
   the return value.  */

static boolean
ecoff_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  bfd_default_set_arch_mach (abfd, arch, machine);
  return arch == bfd_arch_mips;
}

/* Get the size of the section headers.  We do not output the .scommon
   section which we created in ecoff_mkobject.  */

static int
ecoff_sizeof_headers (abfd, reloc)
     bfd *abfd;
     boolean reloc;
{
  return FILHSZ + AOUTSZ + (abfd->section_count - 1) * SCNHSZ;
}

/* Calculate the file position for each section, and set
   reloc_filepos.  */

static void
ecoff_compute_section_file_positions (abfd)
     bfd *abfd;
{
  asection *current;
  file_ptr sofar;
  file_ptr old_sofar;
  boolean first_data;

  if (bfd_get_start_address (abfd)) 
    abfd->flags |= EXEC_P;

  sofar = ecoff_sizeof_headers (abfd, false);

  first_data = true;
  for (current = abfd->sections;
       current != (asection *) NULL;
       current = current->next)
    {
      /* Only deal with sections which have contents */
      if (! (current->flags & SEC_HAS_CONTENTS)
	  || strcmp (current->name, SCOMMON) == 0)
	continue;

      /* On Ultrix, the data sections in an executable file must be
	 aligned to a page boundary within the file.  This does not
	 affect the section size, though.  FIXME: Does this work for
	 other platforms?  */
      if ((abfd->flags & EXEC_P) != 0
	  && first_data != false
	  && (current->flags & SEC_CODE) == 0)
	{
	  sofar = (sofar + ROUND_SIZE - 1) &~ (ROUND_SIZE - 1);
	  first_data = false;
	}

      /* Align the sections in the file to the same boundary on
	 which they are aligned in virtual memory.  */
      old_sofar = sofar;
      sofar = BFD_ALIGN (sofar, 1 << current->alignment_power);

      current->filepos = sofar;

      sofar += current->_raw_size;

      /* make sure that this section is of the right size too */
      old_sofar = sofar;
      sofar = BFD_ALIGN (sofar, 1 << current->alignment_power);
      current->_raw_size += sofar - old_sofar;
    }

  ecoff_data (abfd)->reloc_filepos = sofar;
}

/* Set the contents of a section.  */

static boolean
ecoff_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     asection *section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  if (abfd->output_has_begun == false)
    ecoff_compute_section_file_positions (abfd);

  bfd_seek (abfd, (file_ptr) (section->filepos + offset), SEEK_SET);

  if (count != 0)
    return (bfd_write (location, 1, count, abfd) == count) ? true : false;

  return true;
}

/* Write out an ECOFF file.  */

static boolean
ecoff_write_object_contents (abfd)
     bfd *abfd;
{
  asection *current;
  unsigned int count;
  file_ptr scn_base;
  file_ptr reloc_base;
  file_ptr sym_base;
  unsigned long reloc_size;
  unsigned long text_size;
  unsigned long text_start;
  unsigned long data_size;
  unsigned long data_start;
  unsigned long bss_size;
  struct internal_filehdr internal_f;
  struct internal_aouthdr internal_a;
  int i;

  bfd_error = system_call_error;

  if(abfd->output_has_begun == false)
    ecoff_compute_section_file_positions(abfd);

  if (abfd->sections != (asection *) NULL)
    scn_base = abfd->sections->filepos;
  else
    scn_base = 0;
  reloc_base = ecoff_data (abfd)->reloc_filepos;

  count = 1;
  reloc_size = 0;
  for (current = abfd->sections;
       current != (asection *)NULL; 
       current = current->next) 
    {
      if (strcmp (current->name, SCOMMON) == 0)
	continue;
      current->target_index = count;
      ++count;
      if (current->reloc_count != 0)
	{
	  bfd_size_type relsize;

	  current->rel_filepos = reloc_base;
	  relsize = current->reloc_count * RELSZ;
	  reloc_size += relsize;
	  reloc_base += relsize;
	}
      else
	current->rel_filepos = 0;
    }

  sym_base = reloc_base + reloc_size;

  /* At least on Ultrix, the symbol table of an executable file must
     be aligned to a page boundary.  FIXME: Is this true on other
     platforms?  */
  if ((abfd->flags & EXEC_P) != 0)
    sym_base = (sym_base + ROUND_SIZE - 1) &~ (ROUND_SIZE - 1);

  ecoff_data (abfd)->sym_filepos = sym_base;

  text_size = ecoff_sizeof_headers (abfd, false);
  text_start = 0;
  data_size = 0;
  data_start = 0;
  bss_size = 0;

  /* Write section headers to the file.  */

  internal_f.f_nscns = 0;
  if (bfd_seek (abfd, (file_ptr) (FILHSZ + AOUTSZ), SEEK_SET) != 0)
    return false;
  for (current = abfd->sections;
       current != (asection *) NULL;
       current = current->next)
    {
      struct internal_scnhdr section;
      bfd_vma vma;

      if (strcmp (current->name, SCOMMON) == 0)
	{
	  BFD_ASSERT (bfd_get_section_size_before_reloc (current) == 0
		      && current->reloc_count == 0);
	  continue;
	}

      ++internal_f.f_nscns;

      strncpy (section.s_name, current->name, sizeof section.s_name);

      /* FIXME: is this correct for shared libraries?  I think it is
	 but I have no platform to check.  Ian Lance Taylor.  */
      vma = bfd_get_section_vma (abfd, current);
      if (strcmp (current->name, _LIB) == 0)
	section.s_vaddr = 0;
      else
	section.s_vaddr = vma;

      section.s_paddr = vma;
      section.s_size = bfd_get_section_size_before_reloc (current);

      /* If this section has no size or is unloadable then the scnptr
	 will be 0 too.  */
      if (current->_raw_size == 0
	  || (current->flags & (SEC_LOAD | SEC_HAS_CONTENTS)) == 0)
	section.s_scnptr = 0;
      else
	section.s_scnptr = current->filepos;
      section.s_relptr = current->rel_filepos;

      /* FIXME: the lnnoptr of the .sbss or .sdata section of an
	 object file produced by the assembler is supposed to point to
	 information about how much room is required by objects of
	 various different sizes.  I think this only matters if we
	 want the linker to compute the best size to use, or
	 something.  I don't know what happens if the information is
	 not present.  */
      section.s_lnnoptr = 0;

      section.s_nreloc = current->reloc_count;
      section.s_nlnno = 0;
      section.s_flags = ecoff_sec_to_styp_flags (current->name,
						 current->flags);

      {
	SCNHDR buff;

	ecoff_swap_scnhdr_out (abfd, (PTR) &section, (PTR) &buff);
	if (bfd_write ((PTR) &buff, 1, SCNHSZ, abfd) != SCNHSZ)
	  return false;
      }

      if ((section.s_flags & STYP_TEXT) != 0)
	{
	  text_size += bfd_get_section_size_before_reloc (current);
	  if (text_start == 0 || text_start > vma)
	    text_start = vma;
	}
      else if ((section.s_flags & STYP_RDATA) != 0
	       || (section.s_flags & STYP_DATA) != 0
	       || (section.s_flags & STYP_LIT8) != 0
	       || (section.s_flags & STYP_LIT4) != 0
	       || (section.s_flags & STYP_SDATA) != 0)
	{
	  data_size += bfd_get_section_size_before_reloc (current);
	  if (data_start == 0 || data_start > vma)
	    data_start = vma;
	}
      else if ((section.s_flags & STYP_BSS) != 0
	       || (section.s_flags & STYP_SBSS) != 0)
	bss_size += bfd_get_section_size_before_reloc (current);
    }	

  /* Set up the file header.  */

  if (abfd->xvec->header_byteorder_big_p != false)
    internal_f.f_magic = MIPS_MAGIC_BIG;
  else
    internal_f.f_magic = MIPS_MAGIC_LITTLE;

  /*
    We will NOT put a fucking timestamp in the header here. Every time you
    put it back, I will come in and take it out again.  I'm sorry.  This
    field does not belong here.  We fill it with a 0 so it compares the
    same but is not a reasonable time. -- gnu@cygnus.com
    */
  internal_f.f_timdat = 0;

  if (bfd_get_symcount (abfd) != 0)
    {
      /* The ECOFF f_nsyms field is not actually the number of
	 symbols, it's the size of symbolic information header.  */
      internal_f.f_nsyms = sizeof (struct hdr_ext);
      internal_f.f_symptr = sym_base;
    }
  else
    {
      internal_f.f_nsyms = 0;
      internal_f.f_symptr = 0;
    }

  internal_f.f_opthdr = AOUTSZ;

  internal_f.f_flags = F_LNNO;
  if (reloc_size == 0)
    internal_f.f_flags |= F_RELFLG;
  if (bfd_get_symcount (abfd) == 0)
    internal_f.f_flags |= F_LSYMS;
  if (abfd->flags & EXEC_P)
    internal_f.f_flags |= F_EXEC;

  if (! abfd->xvec->byteorder_big_p)
    internal_f.f_flags |= F_AR32WR;
  else
    internal_f.f_flags |= F_AR32W;

  /* Set up the ``optional'' header.  */
  internal_a.magic = ZMAGIC;

  /* FIXME: This is what Ultrix puts in, and it makes the Ultrix
     linker happy.  But, is it right?  */
  internal_a.vstamp = 0x20a;

  /* At least on Ultrix, these have to be rounded to page boundaries.
     FIXME: Is this true on other platforms?  */
  internal_a.tsize = (text_size + ROUND_SIZE - 1) &~ (ROUND_SIZE - 1);
  internal_a.text_start = text_start &~ (ROUND_SIZE - 1);
  internal_a.dsize = (data_size + ROUND_SIZE - 1) &~ (ROUND_SIZE - 1);
  internal_a.data_start = data_start &~ (ROUND_SIZE - 1);

  /* On Ultrix, the initial portions of the .sbss and .bss segments
     are at the end of the data section.  The bsize field in the
     optional header records how many bss bytes are required beyond
     those in the data section.  The value is not rounded to a page
     boundary.  */
  if (bss_size < internal_a.dsize - data_size)
    bss_size = 0;
  else
    bss_size -= internal_a.dsize - data_size;
  internal_a.bsize = bss_size;
  internal_a.bss_start = internal_a.data_start + internal_a.dsize;

  internal_a.entry = bfd_get_start_address (abfd);

  internal_a.gp_value = ecoff_data (abfd)->gp;

  internal_a.gprmask = ecoff_data (abfd)->gprmask;
  for (i = 0; i < 4; i++)
    internal_a.cprmask[i] = ecoff_data (abfd)->cprmask[i];

  /* Write out the file header and the optional header.  */

  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0)
    return false;

  {
    FILHDR buff;
    ecoff_swap_filehdr_out (abfd, (PTR) &internal_f, (PTR) &buff);
    if (bfd_write ((PTR) &buff, 1, FILHSZ, abfd) != FILHSZ)
      return false;
  }

  {
    AOUTHDR buff;

    ecoff_swap_aouthdr_out (abfd, (PTR) &internal_a, (PTR) &buff);
    if (bfd_write ((PTR) &buff, 1, AOUTSZ, abfd) != AOUTSZ)
      return false;
  }

  /* Write out the relocs.  */
  for (current = abfd->sections;
       current != (asection *) NULL;
       current = current->next)
    {
      RELOC *buff;
      arelent **reloc_ptr_ptr;
      arelent **reloc_end;
      RELOC *out_ptr;

      if (current->reloc_count == 0)
	continue;

      buff = (RELOC *) bfd_alloc (abfd, current->reloc_count * RELSZ);
      if (buff == (RELOC *) NULL)
	{
	  bfd_error = no_memory;
	  return false;
	}

      reloc_ptr_ptr = current->orelocation;
      reloc_end = reloc_ptr_ptr + current->reloc_count;
      out_ptr = buff;
      for (; reloc_ptr_ptr < reloc_end; reloc_ptr_ptr++, out_ptr++)
	{
	  arelent *reloc;
	  asymbol *sym;
	  struct internal_reloc in;
	  
	  memset (&in, 0, sizeof in);

	  reloc = *reloc_ptr_ptr;
	  sym = *reloc->sym_ptr_ptr;

	  /* This must be an ECOFF reloc.  */
	  BFD_ASSERT (reloc->howto != (reloc_howto_type *) NULL
		      && reloc->howto >= ecoff_howto_table
		      && (reloc->howto
			  < (ecoff_howto_table + ECOFF_HOWTO_COUNT)));

	  in.r_vaddr = reloc->address + bfd_get_section_vma (abfd, current);
	  in.r_type = reloc->howto->type;

	  /* If this is a REFHI reloc, the next one must be a REFLO
	     reloc for the same symbol.  */
	  BFD_ASSERT (in.r_type != ECOFF_R_REFHI
		      || (reloc_ptr_ptr < reloc_end
			  && (reloc_ptr_ptr[1]->howto
			      != (reloc_howto_type *) NULL)
			  && (reloc_ptr_ptr[1]->howto->type
			      == ECOFF_R_REFLO)
			  && (sym == *reloc_ptr_ptr[1]->sym_ptr_ptr)));

	  if ((sym->flags & BSF_SECTION_SYM) == 0)
	    {
	      in.r_symndx = ecoff_get_sym_index (*reloc->sym_ptr_ptr);
	      in.r_extern = 1;
	    }
	  else
	    {
	      CONST char *name;

	      name = bfd_get_section_name (abfd, bfd_get_section (sym));
	      if (strcmp (name, ".text") == 0)
		in.r_symndx = RELOC_SECTION_TEXT;
	      else if (strcmp (name, ".rdata") == 0)
		in.r_symndx = RELOC_SECTION_RDATA;
	      else if (strcmp (name, ".data") == 0)
		in.r_symndx = RELOC_SECTION_DATA;
	      else if (strcmp (name, ".sdata") == 0)
		in.r_symndx = RELOC_SECTION_SDATA;
	      else if (strcmp (name, ".sbss") == 0)
		in.r_symndx = RELOC_SECTION_SBSS;
	      else if (strcmp (name, ".bss") == 0)
		in.r_symndx = RELOC_SECTION_BSS;
	      else if (strcmp (name, ".init") == 0)
		in.r_symndx = RELOC_SECTION_INIT;
	      else if (strcmp (name, ".lit8") == 0)
		in.r_symndx = RELOC_SECTION_LIT8;
	      else if (strcmp (name, ".lit4") == 0)
		in.r_symndx = RELOC_SECTION_LIT4;
	      else
		abort ();
	      in.r_extern = 0;
	    }

	  ecoff_swap_reloc_out (abfd, (PTR) &in, (PTR) out_ptr);
	}

      if (bfd_seek (abfd, current->rel_filepos, SEEK_SET) != 0)
	return false;
      if (bfd_write ((PTR) buff, RELSZ, current->reloc_count, abfd)
	  != RELSZ * current->reloc_count)
	return false;
      bfd_release (abfd, (PTR) buff);
    }

  /* Write out the symbolic debugging information.  */
  if (bfd_get_symcount (abfd) > 0)
    {
      HDRR *symhdr;
      unsigned long sym_offset;
      struct hdr_ext buff;

      /* Set up the offsets in the symbolic header.  */
      symhdr = &ecoff_data (abfd)->symbolic_header;
      sym_offset = ecoff_data (abfd)->sym_filepos + sizeof (struct hdr_ext);

#define SET(offset, size, ptr) \
  if (symhdr->size == 0) \
    symhdr->offset = 0; \
  else \
    symhdr->offset = (((char *) ecoff_data (abfd)->ptr \
		       - (char *) ecoff_data (abfd)->raw_syments) \
		      + sym_offset);

      SET (cbLineOffset, cbLine, line);
      SET (cbDnOffset, idnMax, external_dnr);
      SET (cbPdOffset, ipdMax, external_pdr);
      SET (cbSymOffset, isymMax, external_sym);
      SET (cbOptOffset, ioptMax, external_opt);
      SET (cbAuxOffset, iauxMax, external_aux);
      SET (cbSsOffset, issMax, ss);
      SET (cbSsExtOffset, issExtMax, ssext);
      SET (cbFdOffset, ifdMax, external_fdr);
      SET (cbRfdOffset, crfd, external_rfd);
      SET (cbExtOffset, iextMax, external_ext);
#undef SET

      if (bfd_seek (abfd, (file_ptr) ecoff_data (abfd)->sym_filepos,
		    SEEK_SET) != 0)
	return false;
      ecoff_swap_hdr_out (abfd, &ecoff_data (abfd)->symbolic_header, &buff);
      if (bfd_write ((PTR) &buff, 1, sizeof buff, abfd) != sizeof buff)
	return false;
      if (bfd_write ((PTR) ecoff_data (abfd)->raw_syments, 1,
		     ecoff_data (abfd)->raw_size, abfd)
	  != ecoff_data (abfd)->raw_size)
	return false;
    }

  return true;
}

/* Archive handling.  ECOFF uses what appears to be a unique type of
   archive header (which I call an armap).  The byte ordering of the
   armap and the contents are encoded in the name of the armap itself.
   At least for now, we only support archives with the same byte
   ordering in the armap and the contents.

   The first four bytes in the armap are the number of symbol
   definitions.  This is always a power of two.

   This is followed by the symbol definitions.  Each symbol definition
   occupies 8 bytes.  The first four bytes are the offset from the
   start of the armap strings to the null-terminated string naming
   this symbol.  The second four bytes are the file offset to the
   archive member which defines this symbol.  If the second four bytes
   are 0, then this is not actually a symbol definition, and it should
   be ignored.

   The symbols are hashed into the armap with a closed hashing scheme.
   See the functions below for the details of the algorithm.

   We could use the hash table when looking up symbols in a library.
   This would require a new BFD target entry point to replace the
   bfd_get_next_mapent function used by the linker.

   After the symbol definitions comes four bytes holding the size of
   the string table, followed by the string table itself.  */

/* The name of an archive headers looks like this:
   __________E[BL]E[BL]_ (with a trailing space).
   The trailing space is changed to an X if the archive is changed to
   indicate that the armap is out of date.  */

#define ARMAP_BIG_ENDIAN 'B'
#define ARMAP_LITTLE_ENDIAN 'L'
#define ARMAP_MARKER 'E'
#define ARMAP_START "__________"
#define ARMAP_HEADER_MARKER_INDEX 10
#define ARMAP_HEADER_ENDIAN_INDEX 11
#define ARMAP_OBJECT_MARKER_INDEX 12
#define ARMAP_OBJECT_ENDIAN_INDEX 13
#define ARMAP_END_INDEX 14
#define ARMAP_END "_ "

/* This is a magic number used in the hashing algorithm.  */
#define ARMAP_HASH_MAGIC 0x9dd68ab5

/* This returns the hash value to use for a string.  It also sets
   *REHASH to the rehash adjustment if the first slot is taken.  SIZE
   is the number of entries in the hash table, and HLOG is the log
   base 2 of SIZE.  */

static unsigned int
ecoff_armap_hash (s, rehash, size, hlog)
     CONST char *s;
     unsigned int *rehash;
     unsigned int size;
     unsigned int hlog;
{
  unsigned int hash;

  hash = *s++;
  while (*s != '\0')
    hash = ((hash >> 27) | (hash << 5)) + *s++;
  hash *= ARMAP_HASH_MAGIC;
  *rehash = (hash & (size - 1)) | 1;
  return hash >> (32 - hlog);
}

/* Read in the armap.  */

static boolean
ecoff_slurp_armap (abfd)
     bfd *abfd;
{
  char nextname[17];
  unsigned int i;
  struct areltdata *mapdata;
  bfd_size_type parsed_size;
  char *raw_armap;
  struct artdata *ardata;
  unsigned int count;
  char *raw_ptr;
  struct symdef *symdef_ptr;
  char *stringbase;
  
  /* Get the name of the first element.  */
  i = bfd_read ((PTR) nextname, 1, 16, abfd);
  if (i == 0)
      return true;
  if (i != 16)
      return false;

  bfd_seek (abfd, (file_ptr) -16, SEEK_CUR);

  /* See if the first element is an armap.  */
  if (strncmp (nextname, ARMAP_START, sizeof ARMAP_START - 1) != 0
      || nextname[ARMAP_HEADER_MARKER_INDEX] != ARMAP_MARKER
      || (nextname[ARMAP_HEADER_ENDIAN_INDEX] != ARMAP_BIG_ENDIAN
	  && nextname[ARMAP_HEADER_ENDIAN_INDEX] != ARMAP_LITTLE_ENDIAN)
      || nextname[ARMAP_OBJECT_MARKER_INDEX] != ARMAP_MARKER
      || (nextname[ARMAP_OBJECT_ENDIAN_INDEX] != ARMAP_BIG_ENDIAN
	  && nextname[ARMAP_OBJECT_ENDIAN_INDEX] != ARMAP_LITTLE_ENDIAN)
      || strncmp (nextname + ARMAP_END_INDEX,
		  ARMAP_END, sizeof ARMAP_END - 1) != 0)
    {
      bfd_has_map (abfd) = false;
      return true;
    }

  /* Make sure we have the right byte ordering.  */
  if (((nextname[ARMAP_HEADER_ENDIAN_INDEX] == ARMAP_BIG_ENDIAN)
       ^ (abfd->xvec->header_byteorder_big_p != false))
      || ((nextname[ARMAP_OBJECT_ENDIAN_INDEX] == ARMAP_BIG_ENDIAN)
	  ^ (abfd->xvec->byteorder_big_p != false)))
    {
      bfd_error = wrong_format;
      return false;
    }

  /* Read in the armap.  */
  ardata = bfd_ardata (abfd);
  mapdata = snarf_ar_hdr (abfd);
  if (mapdata == (struct areltdata *) NULL)
    return false;
  parsed_size = mapdata->parsed_size;
  bfd_release (abfd, (PTR) mapdata);
    
  raw_armap = (char *) bfd_alloc (abfd, parsed_size);
  if (raw_armap == (char *) NULL)
    {
      bfd_error = no_memory;
      return false;
    }
    
  if (bfd_read ((PTR) raw_armap, 1, parsed_size, abfd) != parsed_size)
    {
      bfd_error = malformed_archive;
      bfd_release (abfd, (PTR) raw_armap);
      return false;
    }
    
  count = bfd_h_get_32 (abfd, (PTR) raw_armap);

  ardata->symdef_count = 0;
  ardata->cache = (struct ar_cache *) NULL;

  /* Hack: overlay the symdefs on top of the raw archive data.  This
     is the way do_slurp_bsd_armap works.  */
  raw_ptr = raw_armap + LONG_SIZE;
  symdef_ptr = (struct symdef *) raw_ptr;
  ardata->symdefs = (carsym *) symdef_ptr;
  stringbase = raw_ptr + count * (2 * LONG_SIZE) + LONG_SIZE;

#ifdef CHECK_ARMAP_HASH
  {
    unsigned int hlog;

    /* Double check that I have the hashing algorithm right by making
       sure that every symbol can be looked up successfully.  */
    hlog = 0;
    for (i = 1; i < count; i <<= 1)
      hlog++;
    BFD_ASSERT (i == count);

    for (i = 0; i < count; i++, raw_ptr += 2 * LONG_SIZE)
      {
	unsigned int name_offset, file_offset;
	unsigned int hash, rehash, srch;
      
	name_offset = bfd_h_get_32 (abfd, (PTR) raw_ptr);
	file_offset = bfd_h_get_32 (abfd, (PTR) (raw_ptr + LONG_SIZE));
	if (file_offset == 0)
	  continue;
	hash = ecoff_armap_hash (stringbase + name_offset, &rehash, count,
				 hlog);
	if (hash == i)
	  continue;

	/* See if we can rehash to this location.  */
	for (srch = (hash + rehash) & (count - 1);
	     srch != hash && srch != i;
	     srch = (srch + rehash) & (count - 1))
	  BFD_ASSERT (bfd_h_get_32 (abfd,
				    (PTR) (raw_armap
					   + LONG_SIZE
					   + (srch * 2 * LONG_SIZE)
					   + LONG_SIZE))
		      != 0);
	BFD_ASSERT (srch == i);
      }
  }

  raw_ptr = raw_armap + LONG_SIZE;
#endif /* CHECK_ARMAP_HASH */

  for (i = 0; i < count; i++, raw_ptr += 2 * LONG_SIZE)
    {
      unsigned int name_offset, file_offset;

      name_offset = bfd_h_get_32 (abfd, (PTR) raw_ptr);
      file_offset = bfd_h_get_32 (abfd, (PTR) (raw_ptr + LONG_SIZE));
      if (file_offset == 0)
	continue;
      symdef_ptr->s.name = stringbase + name_offset;
      symdef_ptr->file_offset = file_offset;
      ++symdef_ptr;
      ++ardata->symdef_count;
    }

  ardata->first_file_filepos = bfd_tell (abfd);
  /* Pad to an even boundary.  */
  ardata->first_file_filepos += ardata->first_file_filepos % 2;

  bfd_has_map (abfd) = true;

  return true;
}

/* Write out an armap.  */

static boolean
ecoff_write_armap (abfd, elength, map, orl_count, stridx)
     bfd *abfd;
     unsigned int elength;
     struct orl *map;
     unsigned int orl_count;
     int stridx;
{
  unsigned int hashsize, hashlog;
  unsigned int symdefsize;
  int padit;
  unsigned int stringsize;
  unsigned int mapsize;
  file_ptr firstreal;
  struct ar_hdr hdr;
  struct stat statbuf;
  unsigned int i;
  bfd_byte temp[LONG_SIZE];
  bfd_byte *hashtable;
  bfd *current;
  bfd *last_elt;

  /* Ultrix appears to use as a hash table size the least power of two
     greater than twice the number of entries.  */
  for (hashlog = 0; (1 << hashlog) <= 2 * orl_count; hashlog++)
    ;
  hashsize = 1 << hashlog;

  symdefsize = hashsize * 2 * LONG_SIZE;
  padit = stridx % 2;
  stringsize = stridx + padit;

  /* Include 8 bytes to store symdefsize and stringsize in output. */
  mapsize = LONG_SIZE + symdefsize + stringsize + LONG_SIZE;

  firstreal = SARMAG + sizeof (struct ar_hdr) + mapsize + elength;

  memset ((PTR) &hdr, 0, sizeof hdr);

  /* Work out the ECOFF armap name.  */
  strcpy (hdr.ar_name, ARMAP_START);
  hdr.ar_name[ARMAP_HEADER_MARKER_INDEX] = ARMAP_MARKER;
  hdr.ar_name[ARMAP_HEADER_ENDIAN_INDEX] =
    (abfd->xvec->header_byteorder_big_p
     ? ARMAP_BIG_ENDIAN
     : ARMAP_LITTLE_ENDIAN);
  hdr.ar_name[ARMAP_OBJECT_MARKER_INDEX] = ARMAP_MARKER;
  hdr.ar_name[ARMAP_OBJECT_ENDIAN_INDEX] =
    abfd->xvec->byteorder_big_p ? ARMAP_BIG_ENDIAN : ARMAP_LITTLE_ENDIAN;
  memcpy (hdr.ar_name + ARMAP_END_INDEX, ARMAP_END, sizeof ARMAP_END - 1);

  /* Write the timestamp of the archive header to be just a little bit
     later than the timestamp of the file, otherwise the linker will
     complain that the index is out of date.  Actually, the Ultrix
     linker just checks the archive name; the GNU linker may check the
     date.  */
  stat (abfd->filename, &statbuf);
  sprintf (hdr.ar_date, "%ld", (long) (statbuf.st_mtime + 60));

  /* The DECstation uses zeroes for the uid, gid and mode of the
     armap.  */
  hdr.ar_uid[0] = '0';
  hdr.ar_gid[0] = '0';
  hdr.ar_mode[0] = '0';

  sprintf (hdr.ar_size, "%-10d", (int) mapsize);

  hdr.ar_fmag[0] = '`';
  hdr.ar_fmag[1] = '\n';

  /* Turn all null bytes in the header into spaces.  */
  for (i = 0; i < sizeof (struct ar_hdr); i++)
   if (((char *)(&hdr))[i] == '\0')
     (((char *)(&hdr))[i]) = ' ';

  if (bfd_write ((PTR) &hdr, 1, sizeof (struct ar_hdr), abfd)
      != sizeof (struct ar_hdr))
    return false;

  bfd_h_put_32 (abfd, hashsize, temp);
  if (bfd_write (temp, 1, LONG_SIZE, abfd) != LONG_SIZE)
    return false;
  
  hashtable = (bfd_byte *) bfd_zalloc (abfd, symdefsize);

  current = abfd->archive_head;
  last_elt = current;
  for (i = 0; i < orl_count; i++)
    {
      unsigned int hash, rehash;

      /* Advance firstreal to the file position of this archive
	 element.  */
      if (((bfd *) map[i].pos) != last_elt)
	{
	  do
	    {
	      firstreal += arelt_size (current) + sizeof (struct ar_hdr);
	      firstreal += firstreal % 2;
	      current = current->next;
	    }
	  while (current != (bfd *) map[i].pos);
	}

      last_elt = current;

      hash = ecoff_armap_hash (*map[i].name, &rehash, hashsize, hashlog);
      if (bfd_h_get_32 (abfd, (PTR) (hashtable
				     + (hash * 2 * LONG_SIZE)
				     + LONG_SIZE))
	  != 0)
	{
	  unsigned int srch;

	  /* The desired slot is already taken.  */
	  for (srch = (hash + rehash) & (hashsize - 1);
	       srch != hash;
	       srch = (srch + rehash) & (hashsize - 1))
	    if (bfd_h_get_32 (abfd, (PTR) (hashtable
					   + (srch * 2 * LONG_SIZE)
					   + LONG_SIZE))
		== 0)
	      break;

	  BFD_ASSERT (srch != hash);

	  hash = srch;
	}
	
      bfd_h_put_32 (abfd, map[i].namidx,
		    (PTR) (hashtable + hash * 2 * LONG_SIZE));
      bfd_h_put_32 (abfd, firstreal,
		    (PTR) (hashtable + hash * 2 * LONG_SIZE + LONG_SIZE));
    }

  if (bfd_write (hashtable, 1, symdefsize, abfd) != symdefsize)
    return false;

  bfd_release (abfd, hashtable);

  /* Now write the strings.  */
  bfd_h_put_32 (abfd, stringsize, temp);
  if (bfd_write (temp, 1, LONG_SIZE, abfd) != LONG_SIZE)
    return false;
  for (i = 0; i < orl_count; i++)
    {
      bfd_size_type len;

      len = strlen (*map[i].name) + 1;
      if (bfd_write ((PTR) (*map[i].name), 1, len, abfd) != len)
	return false;
    }

  /* The spec sez this should be a newline.  But in order to be
     bug-compatible for DECstation ar we use a null.  */
  if (padit)
    {
      if (bfd_write ("\0", 1, 1, abfd) != 1)
	return false;
    }

  return true;
}

/* We just use the generic extended name support.  This is a GNU
   extension.  */
#define ecoff_slurp_extended_name_table	_bfd_slurp_extended_name_table

/* See whether this BFD is an archive.  If it is, read in the armap
   and the extended name table.  */

static bfd_target *
ecoff_archive_p (abfd)
     bfd *abfd;
{
  char armag[SARMAG + 1];

  if (bfd_read ((PTR) armag, 1, SARMAG, abfd) != SARMAG
      || strncmp (armag, ARMAG, SARMAG) != 0)
    {
      bfd_error = wrong_format;
      return (bfd_target *) NULL;
    }

  /* We are setting bfd_ardata(abfd) here, but since bfd_ardata
     involves a cast, we can't do it as the left operand of
     assignment.  */
  abfd->tdata.aout_ar_data =
    (struct artdata *) bfd_zalloc (abfd, sizeof (struct artdata));

  if (bfd_ardata (abfd) == (struct artdata *) NULL)
    {
      bfd_error = no_memory;
      return (bfd_target *) NULL;
    }

  bfd_ardata (abfd)->first_file_filepos = SARMAG;
  
  if (ecoff_slurp_armap (abfd) == false
      || ecoff_slurp_extended_name_table (abfd) == false)
    {
      bfd_release (abfd, bfd_ardata (abfd));
      abfd->tdata.aout_ar_data = (struct artdata *) NULL;
      return (bfd_target *) NULL;
    }
  
  return abfd->xvec;
}

/* This is the COFF backend structure.  The backend_data field of the
   bfd_target structure is set to this.  The section reading code in
   coffgen.c uses this structure.  */

static CONST bfd_coff_backend_data bfd_ecoff_std_swap_table = {
  (void (*) PARAMS ((bfd *,PTR,int,int,PTR))) bfd_void, /* aux_in */
  (void (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* sym_in */
  (void (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* lineno_in */
  (unsigned (*) PARAMS ((bfd *,PTR,int,int,PTR))) bfd_void, /* aux_out */
  (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* sym_out */
  (unsigned (*) PARAMS ((bfd *,PTR,PTR))) bfd_void, /* lineno_out */
  ecoff_swap_reloc_out, ecoff_swap_filehdr_out, ecoff_swap_aouthdr_out,
  ecoff_swap_scnhdr_out,
  FILHSZ, AOUTSZ, SCNHSZ, 0, 0, 0, true,
  ecoff_swap_filehdr_in, ecoff_swap_aouthdr_in, ecoff_swap_scnhdr_in,
  ecoff_bad_format_hook, ecoff_set_arch_mach_hook, ecoff_mkobject_hook,
  ecoff_styp_to_sec_flags, ecoff_make_section_hook, ecoff_set_alignment_hook,
  ecoff_slurp_symbol_table
};

/* get_lineno could be written for ECOFF, but it would currently only
   be useful for linking ECOFF and COFF files together, which doesn't
   seem too likely.  */
#define ecoff_get_lineno \
  ((alent *(*) PARAMS ((bfd *, asymbol *))) bfd_nullvoidptr)

/* These bfd_target functions are defined in other files.  */

#define ecoff_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define ecoff_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define ecoff_core_file_matches_executable_p \
  _bfd_dummy_core_file_matches_executable_p
#define ecoff_truncate_arname		bfd_dont_truncate_arname
#define ecoff_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define ecoff_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	ecoff_get_section_contents	bfd_generic_get_section_contents
#define ecoff_get_reloc_upper_bound	coff_get_reloc_upper_bound
#define	ecoff_close_and_cleanup		bfd_generic_close_and_cleanup
#define ecoff_bfd_debug_info_start	bfd_void
#define ecoff_bfd_debug_info_end	bfd_void
#define ecoff_bfd_debug_info_accumulate	\
  ((void (*) PARAMS ((bfd *, struct sec *))) bfd_void)
#define ecoff_bfd_get_relocated_section_contents \
  bfd_generic_get_relocated_section_contents
#define ecoff_bfd_relax_section		bfd_generic_relax_section
#define ecoff_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)

bfd_target ecoff_little_vec =
{
  "ecoff-littlemips",		/* name */
  bfd_target_ecoff_flavour,
  false,			/* data byte order is little */
  false,			/* header byte order is little */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* sect
							    flags */
  0,				/* leading underscore */
  '/',				/* ar_pad_char */
  15,				/* ar_max_namelen */
  3,				/* minimum alignment power */
  _do_getl64, _do_getl_signed_64, _do_putl64,
     _do_getl32, _do_getl_signed_32, _do_putl32,
     _do_getl16, _do_getl_signed_16, _do_putl16, /* data */
  _do_getl64, _do_getl_signed_64, _do_putl64,
     _do_getl32, _do_getl_signed_32, _do_putl32,
     _do_getl16, _do_getl_signed_16, _do_putl16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
     ecoff_archive_p, _bfd_dummy_target},
  {bfd_false, ecoff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
     bfd_false},
  {bfd_false, ecoff_write_object_contents, /* bfd_write_contents */
     _bfd_write_archive_contents, bfd_false},
  JUMP_TABLE (ecoff),
  (PTR) &bfd_ecoff_std_swap_table
};

bfd_target ecoff_big_vec =
{
  "ecoff-bigmips",		/* name */
  bfd_target_ecoff_flavour,
  true,				/* data byte order is big */
  true,				/* header byte order is big */

  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* sect flags */
  0,				/* leading underscore */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  3,				/* minimum alignment power */
  _do_getb64, _do_getb_signed_64, _do_putb64,
     _do_getb32, _do_getb_signed_32, _do_putb32,
     _do_getb16, _do_getb_signed_16, _do_putb16,
  _do_getb64, _do_getb_signed_64, _do_putb64,
     _do_getb32, _do_getb_signed_32, _do_putb32,
     _do_getb16, _do_getb_signed_16, _do_putb16,
 {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
    ecoff_archive_p, _bfd_dummy_target},
 {bfd_false, ecoff_mkobject, _bfd_generic_mkarchive, /* bfd_set_format */
    bfd_false},
 {bfd_false, ecoff_write_object_contents, /* bfd_write_contents */
    _bfd_write_archive_contents, bfd_false},
  JUMP_TABLE(ecoff),
  (PTR) &bfd_ecoff_std_swap_table
  /* Note that there is another bfd_target just above this one.  If
     you are adding initializers here, you should be adding them there
     as well.  */
};
