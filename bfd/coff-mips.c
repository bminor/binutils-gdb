/* BFD back-end for MIPS Extended-Coff files.
   Copyright 1990, 1991, 1992 Free Software Foundation, Inc.
   Original version by Per Bothner.
   Full support by Ian Lance Taylor, ian@cygnus.com.

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
#include "coff/mips.h"
#include "coff/internal.h"
#include "coff/sym.h"
#include "coff/symconst.h"
#include "coff/ecoff-ext.h"
#include "libcoff.h"

/* `Tdata' information kept for ECOFF files.  */

#define ecoff_data(abfd) ((abfd)->tdata.ecoff_obj_data)

typedef struct ecoff_tdata
{
  /* The reloc file position, set by
     ecoff_compute_section_file_positions.  */
  file_ptr reloc_filepos;

  /* The symbol table file position, set by ecoff_mkobject_hook.  */
  file_ptr sym_filepos;

  /* The cached gp value.  This is used when relocating.  */
  bfd_vma gp;

  /* The size of the unswapped ECOFF symbolic information.  */
  bfd_size_type raw_size;

  /* The unswapped ECOFF symbolic information.  */
  PTR raw_syments;

  /* The swapped ECOFF symbolic header.  */
  HDRR symbolic_header;

  /* Pointers to the unswapped symbolic information.  */
  unsigned char *line;
  struct dnr_ext *external_dnr;
  struct pdr_ext *external_pdr;
  struct sym_ext *external_sym;
  struct opt_ext *external_opt;
  union aux_ext *external_aux;
  char *ss;
  char *ssext;
  struct fdr_ext *external_fdr;
  struct rfd_ext *external_rfd;
  struct ext_ext *external_ext;

  /* The swapped fdr information.  */
  FDR *fdr;

  /* The canonical BFD symbols.  */
  struct ecoff_symbol_struct *canonical_symbols;

} ecoff_data_type;

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

/* MIPS ECOFF has COFF sections, but the debugging information is
   stored in a completely different format.  This files uses the some
   of the swapping routines from coffswap.h, and some of the generic
   COFF routines in coffgen.c, but, unlike the real COFF targets, does
   not use coffcode.h itself.  */

/* Get the generic COFF swapping routines, except for the reloc,
   symbol, and lineno ones.  Give them ecoff names.  */
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
DEFUN(ecoff_bad_format_hook, (abfd, filehdr),
      bfd *abfd AND
      PTR filehdr)
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;

  if (ECOFFBADMAG (*internal_f))
    return false;

  return true;
}

/* This is a hook needed by SCO COFF, but we have nothing to do.  */

static asection *
DEFUN (ecoff_make_section_hook, (abfd, name),
       bfd *abfd AND
       char *name)
{
  return (asection *) NULL;
}

/* Initialize a new section.  */

static boolean
DEFUN (ecoff_new_section_hook, (abfd, section),
       bfd *abfd AND
       asection *section)
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

#define ecoff_set_alignment_hook \
  ((void (*) PARAMS ((bfd *, asection *, PTR))) bfd_void)

static boolean
DEFUN (ecoff_mkobject, (abfd),
       bfd *abfd)
{
  abfd->tdata.ecoff_obj_data = ((struct ecoff_tdata *)
				bfd_zalloc (abfd, sizeof(ecoff_data_type)));
  if (abfd->tdata.ecoff_obj_data == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  return true;
}

/* Create the COFF backend specific information.  */

static          PTR
DEFUN(ecoff_mkobject_hook,(abfd, filehdr),
      bfd            *abfd AND
      PTR	     filehdr)
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;
  ecoff_data_type *ecoff;

  if (ecoff_mkobject (abfd) == false)
    return NULL;

  ecoff = ecoff_data (abfd);
  ecoff->sym_filepos = internal_f->f_symptr;
  return (PTR) ecoff;
}

/* Determine the machine architecture and type.  */
static boolean
DEFUN (ecoff_set_arch_mach_hook, (abfd, filehdr),
       bfd *abfd AND
       PTR filehdr)
{
  long machine;
  enum bfd_architecture arch;
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;

  machine = 0;
  switch (internal_f->f_magic) {
  case  MIPS_MAGIC_1:
  case  MIPS_MAGIC_2:
  case  MIPS_MAGIC_3:
    arch = bfd_arch_mips;
    machine = 0;
    break;

  default:			/* Unreadable input file type */
    arch = bfd_arch_obscure;
    break;
  }

  bfd_default_set_arch_mach(abfd, arch, machine);
  return true;
}

/* Get the section s_flags to use for a section.  */

static long
DEFUN (sec_to_styp_flags, (name, flags),
       CONST char *name AND
       flagword flags)
{
  long styp;

  styp = 0;

  if (strcmp (name, _TEXT) == 0)
    styp = STYP_TEXT;
  else if (strcmp (name, _DATA) == 0)
    styp == STYP_DATA;
  else if (strcmp (name, _SDATA) == 0)
    styp == STYP_SDATA;
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
DEFUN (styp_to_sec_flags, (abfd, hdr),
       bfd *abfd AND
       PTR hdr)
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
   object file.  */

static boolean
DEFUN (ecoff_slurp_symbolic_info, (abfd),
       bfd *abfd)
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

  /* Read all the symbolic information at once.  This expression
     assumes that the external symbols are always the last item.  */
  raw_base = ecoff_data (abfd)->sym_filepos + sizeof (external_symhdr);
  raw_size = (internal_symhdr->cbExtOffset - raw_base
	      + internal_symhdr->iextMax * sizeof (struct ext_ext));
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

/* Create an empty symbol.  */

static asymbol *
DEFUN (ecoff_make_empty_symbol, (abfd),
       bfd *abfd)
{
  ecoff_symbol_type *new;

  new = (ecoff_symbol_type *) bfd_alloc (abfd, sizeof (ecoff_symbol_type));
  if (new == (ecoff_symbol_type *) NULL)
    {
      bfd_error = no_memory;
      return (asymbol *) NULL;
    }
  new->symbol.section = (asection *) NULL;
  new->fdr = (FDR *) NULL;
  new->local = false;
  new->native.lnative = (struct sym_ext *) NULL;
  new->symbol.the_bfd = abfd;
  return &new->symbol;
}

/* Set the BFD flags and section for an ECOFF symbol.  */

static void
DEFUN (ecoff_set_symbol_info, (abfd, ecoff_sym, asym, ext),
       bfd *abfd AND
       SYMR *ecoff_sym AND
       asymbol *asym AND
       int ext)
{
  asym->the_bfd = abfd;
  asym->value = ecoff_sym->value;
  asym->section = &bfd_debug_section;
  asym->udata = NULL;
  if (ext)
    asym->flags = BSF_EXPORT | BSF_GLOBAL;
  else
    asym->flags = BSF_LOCAL;
  switch (ecoff_sym->sc)
    {
    case scNil:
      asym->flags = 0;
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
      asym->section = &bfd_com_section;
      asym->flags = 0;
      break;
    case scRegister:
      asym->flags = BSF_DEBUGGING;
      break;
    case scAbs:
      asym->section = &bfd_abs_section;
      asym->flags = 0;
      break;
    case scUndefined:
      asym->section = &bfd_und_section;
      asym->flags = 0;
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
      asym->section = &bfd_com_section;
      asym->flags = 0;
      break;
    case scRData:
      asym->section = bfd_make_section_old_way (abfd, ".rdata");
      asym->value -= asym->section->vma;
      break;
    case scVar:
      asym->flags = BSF_DEBUGGING;
      break;
    case scCommon:
    case scSCommon:
      asym->section = &bfd_com_section;
      asym->flags = 0;
      break;
    case scVarRegister:
    case scVariant:
      asym->flags = BSF_DEBUGGING;
      break;
    case scSUndefined:
      asym->section = &bfd_und_section;
      asym->flags = 0;
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
      asym->flags = 0;
      break;
    }
}

/* Read an ECOFF symbol table.  */

static boolean
DEFUN (ecoff_slurp_symbol_table, (abfd),
       bfd *abfd)
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
DEFUN (ecoff_get_symtab_upper_bound, (abfd),
       bfd *abfd)
{
  if (ecoff_slurp_symbolic_info (abfd) == false
      || bfd_get_symcount (abfd) == 0)
    return 0;

  return (bfd_get_symcount (abfd) + 1) * (sizeof (ecoff_symbol_type *));
}

static unsigned int
DEFUN (ecoff_get_symtab, (abfd, alocation),
       bfd *abfd AND
       asymbol **alocation)
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
   emit_aggregate and type_to_string are from gcc/mips-tdump.c, with
   swapping added and used_ptr removed.  */

/* Write aggregate information to a string.  */

static void
DEFUN (emit_aggregate, (abfd, string, rndx, isym, which),
       bfd *abfd AND
       char *string AND
       RNDXR *rndx AND
       long isym AND
       CONST char *which)
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
DEFUN (type_to_string, (abfd, aux_ptr, indx, bigendian),
       bfd *abfd AND
       union aux_ext *aux_ptr AND
       int indx AND
       int bigendian)
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
      emit_aggregate (abfd, p1, &rndx,
		      AUX_GET_ISYM (bigendian, &aux_ptr[indx+1]),
		      "struct");
      indx++;			/* skip aux words */
      break;

      /* Unions add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to union def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case btUnion:		/* Union */
      ecoff_swap_rndx_in (bigendian, &aux_ptr[indx].a_rndx, &rndx);
      emit_aggregate (abfd, p1, &rndx,
		      AUX_GET_ISYM (bigendian, &aux_ptr[indx+1]),
		      "union");
      indx++;			/* skip aux words */
      break;

      /* Enumerations add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to enum def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case btEnum:		/* Enumeration */
      ecoff_swap_rndx_in (bigendian, &aux_ptr[indx].a_rndx, &rndx);
      emit_aggregate (abfd, p1, &rndx,
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
DEFUN (ecoff_print_symbol, (abfd, filep, symbol, how),
       bfd *abfd AND
       PTR filep AND
       asymbol *symbol AND
       bfd_print_symbol_type how)
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
			  type_to_string (abfd, aux_base, indx + 1,
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
			  type_to_string (abfd, aux_base, indx, bigendian));
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
DEFUN (ecoff_swap_reloc_in, (abfd, ext, intern),
       bfd *abfd AND
       RELOC *ext AND
       struct internal_reloc *intern)
{
  intern->r_vaddr = bfd_h_get_32 (abfd, (bfd_byte *) ext->r_vaddr);
  if (abfd->xvec->header_byteorder_big_p != false)
    {
      intern->r_symndx = ((ext->r_bits[0]
			   << RELOC_BITS0_SYMNDX_SH_LEFT_BIG)
			  | (ext->r_bits[1]
			     << RELOC_BITS1_SYMNDX_SH_LEFT_BIG)
			  | (ext->r_bits[2]
			     << RELOC_BITS2_SYMNDX_SH_LEFT_BIG));
      intern->r_type = ((ext->r_bits[3] & RELOC_BITS3_TYPE_BIG)
			>> RELOC_BITS3_TYPE_SH_BIG);
      intern->r_extern = (ext->r_bits[3] & RELOC_BITS3_EXTERN_BIG) != 0;
    }
  else
    {
      intern->r_symndx = ((ext->r_bits[0]
			   << RELOC_BITS0_SYMNDX_SH_LEFT_LITTLE)
			  | (ext->r_bits[1]
			     << RELOC_BITS1_SYMNDX_SH_LEFT_LITTLE)
			  | (ext->r_bits[2]
			     << RELOC_BITS2_SYMNDX_SH_LEFT_LITTLE));
      intern->r_type = ((ext->r_bits[3] & RELOC_BITS3_TYPE_LITTLE)
			>> RELOC_BITS3_TYPE_SH_LITTLE);
      intern->r_extern = (ext->r_bits[3] & RELOC_BITS3_EXTERN_LITTLE) != 0;
    }
}

/* Swap a reloc out.  */

static unsigned int
DEFUN (ecoff_swap_reloc_out, (abfd, src, dst),
       bfd *abfd AND
       PTR src AND
       PTR dst)
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

/* Do a REFHI relocation.  The next reloc must be the corresponding
   REFLO.  This has to be done in a function so that carry is handled
   correctly.  */

static bfd_reloc_status_type
DEFUN (ecoff_refhi_reloc, (abfd,
			   reloc_entry,
			   symbol,
			   data,
			   input_section,
			   output_bfd),
       bfd *abfd AND
       arelent *reloc_entry AND
       asymbol *symbol AND
       PTR data AND
       asection *input_section AND
       bfd *output_bfd)
{
  bfd_reloc_status_type ret;
  arelent *rello;
  bfd_vma relocation;
  asection *reloc_target_output_section;
  unsigned long val;
  unsigned long insn;

  ret = bfd_reloc_ok;
  if (symbol->section == &bfd_und_section
      && output_bfd == (bfd *) NULL)
    ret = bfd_reloc_undefined;

  rello = reloc_entry + 1;
  BFD_ASSERT (rello->howto->type == ECOFF_R_REFLO
	      && *rello->sym_ptr_ptr == *reloc_entry->sym_ptr_ptr);

  if (symbol->section == &bfd_com_section)
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  if (output_bfd != (bfd *) NULL) 
    reloc_entry->address += input_section->output_offset;

  insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address);
  val = (((insn & 0xffff) << 16)
	 + (bfd_get_32 (abfd, (bfd_byte *) data + rello->address) & 0xffff));
  insn = (insn &~ 0xffff) | (((val + symbol->value) >> 16) & 0xffff);
  bfd_put_32 (abfd, insn, (bfd_byte *) data + reloc_entry->address);

  return ret;
}

/* Do a GPREL relocation.  This is a 16 bit value which must become
   the offset from the gp register.  */

static bfd_reloc_status_type
DEFUN (ecoff_gprel_reloc, (abfd,
			   reloc_entry,
			   symbol,
			   data,
			   input_section,
			   output_bfd),
       bfd *abfd AND
       arelent *reloc_entry AND
       asymbol *symbol AND
       PTR data AND
       asection *input_section AND
       bfd *output_bfd)
{
  bfd_reloc_status_type ret;
  bfd_vma relocation;
  unsigned long val;
  unsigned long insn;

  /* If this is a partial link, we don't need to do anything unusual
     here.  */
  if (output_bfd != (bfd *) NULL)
    return bfd_reloc_continue;

  ret = bfd_reloc_ok;
  if (symbol->section == &bfd_und_section)
    ret = bfd_reloc_undefined;

  /* Otherwise we have to figure out the gp value, so that we can
     adjust the symbol value correctly.  We look up the symbol _gp in
     the output BFD.  If we can't find it, we're stuck.  We cache it
     in the ECOFF target data.  */
  output_bfd = symbol->section->output_section->owner;
  if (ecoff_data (output_bfd)->gp == 0)
    {
      unsigned int count;
      asymbol **sym;
      unsigned int i;

      count = bfd_get_symcount (output_bfd);
      sym = bfd_get_outsymbols (output_bfd);

      /* We should do something more friendly here, but we don't have
	 a good reloc status to return.  */
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

  if (symbol->section == &bfd_com_section)
    relocation = 0;
  else
    relocation = symbol->value;

  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;

  if (reloc_entry->address > input_section->_cooked_size)
    return bfd_reloc_outofrange;

  insn = bfd_get_32 (abfd, (bfd_byte *) data + reloc_entry->address);
  val = insn & 0xffff;
  if ((val & 0x8000) != 0)
    val -= 0x10000;
  val -= ecoff_data (output_bfd)->gp;
  insn = (insn &~ 0xffff) | (val & 0xffff);
  bfd_put_32 (abfd, insn, (bfd_byte *) data + reloc_entry->address);

  return ret;
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
	 0,			/* special_function */
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
	 0,			/* special_function */
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
	 0,			/* special_function */
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
	 0,			/* special_function */
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

/* Read in the relocs for a section.  */

static boolean
DEFUN (ecoff_slurp_reloc_table, (abfd, section, symbols),
       bfd *abfd AND
       asection *section AND
       asymbol **symbols)
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

      if (intern.r_type < 0 || intern.r_type > ECOFF_R_LITERAL)
	abort ();

      if (intern.r_extern)
	{
	  /* r_symndx is an index into the external symbols.  */
	  BFD_ASSERT (intern.r_symndx >= 0
		      && (intern.r_symndx
			  < ecoff_data (abfd)->symbolic_header.iextMax));
	  rptr->sym_ptr_ptr = symbols + intern.r_symndx;
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
	}

      rptr->address = intern.r_vaddr - bfd_get_section_vma (abfd, section);
      rptr->addend = 0;
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
DEFUN (ecoff_canonicalize_reloc, (abfd, section, relptr, symbols),
       bfd *abfd AND
       asection *section AND
       arelent **relptr AND
       asymbol **symbols)
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

/* Provided a BFD, a section and an offset into the section, calculate
   and return the name of the source file and the line nearest to the
   wanted location.  */

static boolean
DEFUN (ecoff_find_nearest_line, (abfd,
				 section,
				 ignore_symbols,
				 offset,
				 filename_ptr,
				 functionname_ptr,
				 retline_ptr),
       bfd *abfd AND
       asection *section AND
       asymbol **ignore_symbols AND
       bfd_vma offset AND
       CONST char **filename_ptr AND
       CONST char **functionname_ptr AND
       unsigned int *retline_ptr)
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
  SYMR proc_sym;

  /* If we're not in the .text section, we don't have any line
     numbers.  */
  if (strcmp (section->name, _TEXT) != 0)
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
      if (offset < fdr_ptr->adr)
	break;
      if (fdr_ptr->cpd > 0)
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
  if (offset < pdr.adr)
    return false;
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

  /* If offset is too large, this line is not interesting.  */
  if (offset > 100)
    return false;

  *filename_ptr = ecoff_data (abfd)->ss + fdr_ptr->issBase + fdr_ptr->rss;
  ecoff_swap_sym_in (abfd,
		     (ecoff_data (abfd)->external_sym
		      + fdr_ptr->isymBase
		      + pdr.isym),
		     &proc_sym);
  *functionname_ptr = ecoff_data (abfd)->ss + proc_sym.iss;
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
DEFUN (ecoff_clear_output_flags, (abfd),
       bfd *abfd)
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
DEFUN (ecoff_rel, (output_bfd, seclet, output_section, data, relocateable),
       bfd *output_bfd AND
       bfd_seclet_type *seclet AND
       asection *output_section AND
       PTR data AND
       boolean relocateable)
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

      /* We just accumulate symbols from a non-ECOFF BFD.  */

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
	  else
	    {
	      ++output_symhdr->iextMax;
	      output_symhdr->issExtMax += len + 1;
	    }
	}

      bfd_release (output_bfd, (PTR) symbols);

      ++output_symhdr->ifdMax;

      return true;
    }

  /* We simply add in the information from another ECOFF BFD.  First
     we make sure we have the symbolic information.  */
  if (ecoff_slurp_symbolic_info (input_bfd) == false)
    return false;
  if (bfd_get_symcount (input_bfd) == 0)
    return true;

  input_symhdr = &ecoff_data (input_bfd)->symbolic_header;

  output_symhdr->ilineMax += input_symhdr->ilineMax;
  output_symhdr->cbLine += input_symhdr->cbLine;
  output_symhdr->idnMax += input_symhdr->idnMax;
  output_symhdr->ipdMax += input_symhdr->ipdMax;
  output_symhdr->isymMax += input_symhdr->isymMax;
  output_symhdr->ioptMax += input_symhdr->ioptMax;
  output_symhdr->iauxMax += input_symhdr->iauxMax;
  output_symhdr->issMax += input_symhdr->issMax;
  output_symhdr->issExtMax += input_symhdr->issExtMax;
  output_symhdr->ifdMax += input_symhdr->ifdMax;
  output_symhdr->iextMax += input_symhdr->iextMax;

  /* The RFD's are special, since we create them if needed.  */
  if (input_symhdr->crfd > 0)
    output_symhdr->crfd += input_symhdr->crfd;
  else
    output_symhdr->crfd += input_symhdr->ifdMax;

  return true;
}

/* Handle an arbitrary seclet on the first pass.  */

static boolean
DEFUN (ecoff_dump_seclet, (abfd, seclet, section, data, relocateable),
       bfd *abfd AND
       bfd_seclet_type *seclet AND
       asection *section AND
       PTR data AND
       boolean relocateable)
{
  switch (seclet->type) 
    {
    case bfd_indirect_seclet:
      /* The contents of this section come from another one somewhere
	 else.  */
      return ecoff_rel (abfd, seclet, section, data, relocateable);

    case bfd_fill_seclet:
      /* Fill in the section with us.  */
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

    default:
      abort();
    }

  return true;
}

/* Add a string to the debugging information we are accumulating for a
   file.  Return the offset from the fdr string base or from the
   external string base.  */

static long
DEFUN (ecoff_add_string, (output_bfd, fdr, string, external),
       bfd *output_bfd AND
       FDR *fdr AND
       CONST char *string AND
       boolean external)
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
DEFUN (ecoff_get_debug, (output_bfd, seclet, section),
       bfd *output_bfd AND
       bfd_seclet_type *seclet AND
       asection *section)
{
  bfd *input_bfd;
  HDRR *output_symhdr;
  HDRR *input_symhdr;
  ecoff_data_type *output_ecoff;
  ecoff_data_type *input_ecoff;
  FDR *fdr_ptr;
  FDR *fdr_end;
  struct fdr_ext *fdr_out;
  struct ext_ext *ext_ptr;
  struct ext_ext *ext_end;
  struct ext_ext *ext_out;
  long iss;
  long isym;
  long iline;
  long iopt;
  long ipdr;
  long iaux;
  long irfd;
  long cbline;

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

      /* Get the symbols from the input BFD.  */
      symbols = (asymbol **) bfd_alloc (output_bfd,
					get_symtab_upper_bound (input_bfd));
      if (symbols == (asymbol **) NULL)
	{
	  bfd_error = no_memory;
	  return false;
	}
      sym_end = symbols + bfd_canonicalize_symtab (input_bfd, symbols);

      /* Handle the local symbols.  */
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
	  internal_sym.value = (*sym_ptr)->value;
	  internal_sym.st = stNil;
	  internal_sym.sc = scUndefined;
	  internal_sym.index = indexNil;
	  ecoff_swap_sym_out (output_bfd, &internal_sym,
			      (output_ecoff->external_sym
			       + output_symhdr->isymMax));
	  ++fdr.csym;
	  ++output_symhdr->isymMax;
	}

      /* Handle the external symbols.  */
      for (sym_ptr = symbols; sym_ptr != sym_end; sym_ptr++)
	{
	  EXTR internal_ext;

	  if (((*sym_ptr)->flags & BSF_EXPORT) == 0)
	    continue;
	  memset (&internal_ext, 0, sizeof internal_ext);
	  internal_ext.ifd = output_symhdr->ifdMax;
	  internal_ext.asym.iss = ecoff_add_string (output_bfd,
						    &fdr,
						    (*sym_ptr)->name,
						    true);
	  internal_ext.asym.value = (*sym_ptr)->value;
	  internal_ext.asym.st = stNil;
	  internal_ext.asym.sc = scUndefined;
	  internal_ext.asym.reserved = 0;
	  internal_ext.asym.index = indexNil;
	  ecoff_swap_ext_out (output_bfd, &internal_ext,
			      (output_ecoff->external_ext
			       + output_symhdr->iextMax));
	  ++output_symhdr->iextMax;
	}

      bfd_release (output_bfd, (PTR) symbols);

      /* Leave everything else zeroed out.  This will cause the lang
	 field to be langC.  The fBigendian field will indicate little
	 endian format, but it doesn't matter because it only applies
	 to aux fields and there are none.  */

      ecoff_swap_fdr_out (output_bfd, &fdr,
			  (output_ecoff->external_fdr
			   + output_symhdr->ifdMax));
      ++output_symhdr->ifdMax;
      return true;
    }

  /* This is an ECOFF BFD.  We want to grab the information from
     input_bfd and attach it to output_bfd.  */
  if (bfd_get_symcount (input_bfd) == 0)
    return true;
  input_ecoff = ecoff_data (input_bfd);
  input_symhdr = &input_ecoff->symbolic_header;

  /* Because ECOFF uses relative pointers for most of the debugging
     information, much of it can simply be copied from input_bfd to
     output_bfd.  */
  memcpy (output_ecoff->line + output_symhdr->cbLineOffset,
	  input_ecoff->line,
	  input_symhdr->cbLine * sizeof (unsigned char));
  memcpy (output_ecoff->external_aux + output_symhdr->iauxMax,
	  input_ecoff->external_aux,
	  input_symhdr->iauxMax * sizeof (union aux_ext));
  memcpy (output_ecoff->ss + output_symhdr->issMax,
	  input_ecoff->ss,
	  input_symhdr->issMax * sizeof (char));
  memcpy (output_ecoff->ssext + output_symhdr->issExtMax,
	  input_ecoff->ssext,
	  input_symhdr->issExtMax * sizeof (char));

  /* Some of the information may need to be swapped.  */
  if (output_bfd->xvec->byteorder_big_p == input_bfd->xvec->byteorder_big_p)
    {
      /* The two BFD's have the same endianness, so memcpy will
	 suffice.  */
      memcpy (output_ecoff->external_dnr + output_symhdr->idnMax,
	      input_ecoff->external_dnr,
	      input_symhdr->idnMax * sizeof (struct dnr_ext));
      memcpy (output_ecoff->external_pdr + output_symhdr->ipdMax,
	      input_ecoff->external_pdr,
	      input_symhdr->ipdMax * sizeof (struct pdr_ext));
      memcpy (output_ecoff->external_sym + output_symhdr->isymMax,
	      input_ecoff->external_sym,
	      input_symhdr->isymMax * sizeof (struct sym_ext));
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
      struct sym_ext *sym_in;
      struct sym_ext *sym_end;
      struct sym_ext *sym_out;
      struct opt_ext *opt_in;
      struct opt_ext *opt_end;
      struct opt_ext *opt_out;

      /* The two BFD's have different endianness, so we must swap
	 everything in and out.  This code would always work, but it
	 would be very slow in the normal case.  */
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
      for (; pdr_in < pdr_end; pdr_in++, pdr_out++)
	{
	  PDR pdr;

	  ecoff_swap_pdr_in (input_bfd, pdr_in, &pdr);
	  ecoff_swap_pdr_out (output_bfd, &pdr, pdr_out);
	}
      sym_in = input_ecoff->external_sym;
      sym_end = sym_in + input_symhdr->isymMax;
      sym_out = output_ecoff->external_sym + output_symhdr->isymMax;
      for (; sym_in < sym_end; sym_in++, sym_out++)
	{
	  SYMR sym;

	  ecoff_swap_sym_in (input_bfd, sym_in, &sym);
	  ecoff_swap_sym_out (output_bfd, &sym, sym_out);
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

  /* Step through the FDR's of input_bfd, adjust the offsets, and
     swap them out.  */
  iss = output_symhdr->issMax;
  isym = output_symhdr->isymMax;
  iline = output_symhdr->ilineMax;
  iopt = output_symhdr->ioptMax;
  ipdr = output_symhdr->ipdMax;
  iaux = output_symhdr->iauxMax;
  irfd = output_symhdr->crfd;
  cbline = output_symhdr->cbLineOffset;

  fdr_ptr = input_ecoff->fdr;
  fdr_end = fdr_ptr + input_symhdr->ifdMax;
  fdr_out = output_ecoff->external_fdr + output_symhdr->ifdMax;
  for (; fdr_ptr < fdr_end; fdr_ptr++, fdr_out++)
    {
      FDR fdr;

      fdr = *fdr_ptr;

      /* The memory address for this fdr is the address for the seclet
	 plus the offset to this fdr within input_bfd.  */
      fdr.adr = (bfd_get_section_vma (output_bfd, section)
		 + seclet->offset
		 + (fdr_ptr->adr - input_ecoff->fdr->adr));

      fdr.issBase = iss;
      iss += fdr.cbSs * sizeof (char);
      fdr.isymBase = isym;
      isym += fdr.csym * sizeof (struct sym_ext);
      fdr.ilineBase = iline;
      iline += fdr.cline;
      fdr.ioptBase = iopt;
      iopt += fdr.copt * sizeof (struct opt_ext);
      fdr.ipdFirst = ipdr;
      ipdr += fdr.cpd * sizeof (struct pdr_ext);
      fdr.iauxBase = iaux;
      iaux += fdr.caux * sizeof (union aux_ext);
      fdr.rfdBase = irfd;
      irfd += fdr.crfd * sizeof (struct rfd_ext);
      fdr.cbLineOffset = cbline;
      cbline += fdr.cbLine * sizeof (unsigned char);

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

  /* We have to swap the external symbols in and out because we have
     to adjust the file descriptor indices.  */
  ext_ptr = input_ecoff->external_ext;
  ext_end = ext_ptr + input_symhdr->iextMax;
  ext_out = output_ecoff->external_ext + output_symhdr->iextMax;
  for (; ext_ptr < ext_end; ext_ptr++, ext_out++)
    {
      EXTR ext;

      ecoff_swap_ext_in (input_bfd, ext_ptr, &ext);
      ext.ifd += output_symhdr->ifdMax;
      ecoff_swap_ext_out (output_bfd, &ext, ext_out);
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
  output_symhdr->issExtMax += input_symhdr->issExtMax;
  output_symhdr->ifdMax += input_symhdr->ifdMax;
  output_symhdr->iextMax += input_symhdr->iextMax;

  /* Double check that the counts we got by stepping through the FDR's
     match the counts we got from input_symhdr.  */
  BFD_ASSERT (output_symhdr->issMax == iss
	      && output_symhdr->isymMax == isym
	      && output_symhdr->ilineMax == iline
	      && output_symhdr->ioptMax == iopt
	      && output_symhdr->ipdMax == ipdr
	      && output_symhdr->iauxMax == iaux
	      && output_symhdr->cbLineOffset == cbline);

  return true;
}

/* This is the actual link routine.  It makes two passes over all the
   seclets.  */

static boolean
DEFUN (ecoff_bfd_seclet_link, (abfd, data, relocateable),
       bfd *abfd AND
       PTR data AND
       boolean relocateable)
{
  HDRR *symhdr;
  int ipass;
  register asection *o;
  register bfd_seclet_type *p;
  bfd_size_type size;
  char *raw;

  /* We accumulate the debugging information counts in the symbolic
     header.  */
  symhdr = &ecoff_data (abfd)->symbolic_header;
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

  /* We are only going to look at each input BFD once.  It is
     convenient to look at the code section first, so that the first
     time we look at a BFD we can set the text address (otherwise,
     when we get to the text section, we would have to be able to
     track down the file information associated with that BFD).  So we
     actually do each pass in two sub passes; first the code sections,
     then the non-code sections.  */

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
		  if (ecoff_get_debug (abfd, p, o) == false)
		    return false;
		}
	    }
	}
    }

  return true;
}

/* Set the architecture.  The only architecture we support here is
   mips.  We set the architecture anyhow, since many callers ignore
   the return value.  */

static boolean
DEFUN (ecoff_set_arch_mach, (abfd, arch, machine),
       bfd *abfd AND
       enum bfd_architecture arch AND
       unsigned long machine)
{
  bfd_default_set_arch_mach (abfd, arch, machine);
  return arch == bfd_arch_mips;
}

/* Calculate the file position for each section, and set
   reloc_filepos.  */

static void
DEFUN (ecoff_compute_section_file_positions, (abfd),
       bfd *abfd)
{
  asection *current;
  asection *previous;
  file_ptr sofar;
  file_ptr old_sofar;

  sofar = FILHSZ;

  if (bfd_get_start_address (abfd)) 
    {
      /* A start address may have been added to the original file. In
	 this case it will need an optional header to record it.  */
      abfd->flags |= EXEC_P;
    }

  if (abfd->flags & EXEC_P)
    sofar += AOUTSZ;

  sofar += abfd->section_count * SCNHSZ;

  previous = (asection *) NULL;
  for (current = abfd->sections;
       current != (asection *) NULL;
       current = current->next)
    {
      /* Only deal with sections which have contents */
      if (! (current->flags & SEC_HAS_CONTENTS))
	continue;

      /* Align the sections in the file to the same boundary on
	 which they are aligned in virtual memory.  */
      old_sofar = sofar;
      sofar = BFD_ALIGN (sofar, 1 << current->alignment_power);
      if (previous != (asection *) NULL)
	previous->_raw_size += sofar - old_sofar;

      current->filepos = sofar;

      sofar += current->_raw_size;

      /* make sure that this section is of the right size too */
      old_sofar = sofar;
      sofar = BFD_ALIGN (sofar, 1 << current->alignment_power);
      current->_raw_size += sofar - old_sofar;
      previous = current;
    }

  ecoff_data (abfd)->reloc_filepos = sofar;
}

/* Set the contents of a section.  */

static boolean
DEFUN (ecoff_set_section_contents, (abfd, section, location, offset, count),
       bfd *abfd AND
       asection *section AND
       PTR location AND
       file_ptr offset AND
       bfd_size_type count)
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
DEFUN (ecoff_write_object_contents, (abfd),
       bfd *abfd)
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
  ecoff_data (abfd)->sym_filepos = sym_base;

  text_size = 0;
  text_start = 0;
  data_size = 0;
  data_start = 0;
  bss_size = 0;

  /* Write section headers to the file.  */

  internal_f.f_nscns = 0;
  if (bfd_seek (abfd,
		(file_ptr) ((abfd->flags & EXEC_P) ?
			    (FILHSZ + AOUTSZ) : FILHSZ),
		SEEK_SET) != 0)
    return false;
  for (current = abfd->sections;
       current != (asection *) NULL;
       current = current->next)
    {
      struct internal_scnhdr section;
      bfd_vma vma;

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
      section.s_flags = sec_to_styp_flags (current->name, current->flags);

      {
	SCNHDR buff;

	ecoff_swap_scnhdr_out (abfd, (PTR) &section, (PTR) &buff);
	if (bfd_write ((PTR) &buff, 1, SCNHSZ, abfd) != SCNHSZ)
	  return false;
      }

      /* FIXME: These numbers don't add up to what the MIPS tools
	 produce, although I don't think it matters.  */
      if ((section.s_flags & STYP_TEXT) != 0
	  || (section.s_flags & STYP_RDATA) != 0
	  || (section.s_flags & STYP_LIT8) != 0
	  || (section.s_flags & STYP_LIT4) != 0)
	{
	  text_size += bfd_get_section_size_before_reloc (current);
	  if (text_start == 0 || text_start > vma)
	    text_start = vma;
	}
      else if ((section.s_flags & STYP_DATA) != 0
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

  internal_f.f_magic = MIPS_MAGIC_2;

  /*
    We will NOT put a fucking timestamp in the header here. Every time you
    put it back, I will come in and take it out again.  I'm sorry.  This
    field does not belong here.  We fill it with a 0 so it compares the
    same but is not a reasonable time. -- gnu@cygnus.com
    */
  internal_f.f_timdat = 0;

  internal_f.f_nsyms =  bfd_get_symcount(abfd);
  if (internal_f.f_nsyms != 0)
    internal_f.f_symptr = sym_base;
  else
    internal_f.f_symptr = 0;

  if (abfd->flags & EXEC_P)
    internal_f.f_opthdr = AOUTSZ;
  else
    internal_f.f_opthdr = 0;

  internal_f.f_flags = 0;
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

  /* Set up the optional header.  */

  if ((abfd->flags & EXEC_P) != 0)
    {
      internal_a.magic = ZMAGIC;

      /* FIXME: What should this be?  */
      internal_a.vstamp = 0;

      internal_a.tsize = text_size;
      internal_a.text_start = text_start;
      internal_a.dsize = data_size;
      internal_a.data_start = data_start;
      internal_a.bsize = bss_size;

      internal_a.entry = bfd_get_start_address (abfd);

      /* FIXME: The MIPS optional header is larger than this....  */
    }

  /* Write out the file header and the optional header.  */

  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0)
    return false;

  {
    FILHDR buff;
    ecoff_swap_filehdr_out (abfd, (PTR) &internal_f, (PTR) &buff);
    if (bfd_write ((PTR) &buff, 1, FILHSZ, abfd) != FILHSZ)
      return false;
  }

  if ((abfd->flags & EXEC_P) != 0)
    {
      AOUTHDR buff;

      ecoff_swap_aouthdr_out (abfd, (PTR) &internal_a, (PTR) &buff);
      if (bfd_write ((PTR) &buff, 1, AOUTSZ, abfd) != AOUTSZ)
	return false;
    }

  /* Write out the relocs.  */

  /* Write out the symbolic debugging information.  */
  if (bfd_get_symcount (abfd) > 0)
    {
      struct hdr_ext buff;

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
  styp_to_sec_flags, ecoff_make_section_hook, ecoff_set_alignment_hook,
  ecoff_slurp_symbol_table
};

/* get_lineno could be written for ECOFF, but it would currently only
   be useful for linking ECOFF and COFF files together, which doesn't
   seem too likely.  */
#define ecoff_get_lineno \
  ((alent *(*) PARAMS ((bfd *, asymbol *))) bfd_nullvoidptr)

#define ecoff_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define ecoff_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define ecoff_core_file_matches_executable_p	_bfd_dummy_core_file_matches_executable_p
#define ecoff_slurp_armap		bfd_slurp_coff_armap
#define ecoff_slurp_extended_name_table	_bfd_slurp_extended_name_table
#define ecoff_write_armap		coff_write_armap
#define ecoff_truncate_arname		bfd_dont_truncate_arname
#define ecoff_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define ecoff_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	ecoff_get_section_contents	bfd_generic_get_section_contents
#define ecoff_get_reloc_upper_bound	coff_get_reloc_upper_bound
#define	ecoff_close_and_cleanup		bfd_generic_close_and_cleanup
#define ecoff_sizeof_headers		coff_sizeof_headers
#define ecoff_bfd_debug_info_start	bfd_void
#define ecoff_bfd_debug_info_end	bfd_void
#define ecoff_bfd_debug_info_accumulate	\
  ((void (*) PARAMS ((bfd *, struct sec *))) bfd_void)
#define ecoff_bfd_get_relocated_section_contents  bfd_generic_get_relocated_section_contents
#define ecoff_bfd_relax_section		bfd_generic_relax_section

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
  _do_getl64, _do_putl64,	_do_getl32, _do_putl32, _do_getl16, _do_putl16, /* data */
  _do_getl64, _do_putl64,	_do_getl32, _do_putl32, _do_getl16, _do_putl16, /* hdrs */

  {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
   bfd_generic_archive_p, _bfd_dummy_target},
  {bfd_false, ecoff_mkobject, bfd_false, /* bfd_set_format */
   bfd_false},
  {bfd_false, ecoff_write_object_contents, bfd_false, bfd_false},
  JUMP_TABLE (ecoff),
  0, 0,
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
  _do_getb64, _do_putb64,	_do_getb32, _do_putb32, _do_getb16, _do_putb16,
  _do_getb64, _do_putb64,	_do_getb32, _do_putb32, _do_getb16, _do_putb16,
 {_bfd_dummy_target, coff_object_p, /* bfd_check_format */
   bfd_generic_archive_p, _bfd_dummy_target},
 {bfd_false, ecoff_mkobject, bfd_false, /* bfd_set_format */
   bfd_false},
 {bfd_false, ecoff_write_object_contents, /* bfd_write_contents */
   bfd_false, bfd_false},
  JUMP_TABLE(ecoff),
  0, 0,
  (PTR) &bfd_ecoff_std_swap_table
  /* Note that there is another bfd_target just above this one.  If
     you are adding initializers here, you should be adding them there
     as well.  */
};
