/* BFD back-end for MIPS Extended-Coff files.
   Copyright 1990, 1991, 1992 Free Software Foundation, Inc.
   Written by Per Bothner.
   Symbol and line number support added by Ian Lance Taylor.

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
  /* The symbol table file position.  */
  file_ptr sym_filepos;

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

/* Get the generic COFF swapping routines, except for the symbol and
   lineno ones.  Give them ecoff names.  */
#define NO_COFF_SYMBOLS
#define NO_COFF_LINENOS
#define coff_swap_reloc_in ecoff_swap_reloc_in
#define coff_swap_reloc_out ecoff_swap_reloc_out
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

/* Get the BFD flags to use for a section.  */

static flagword
DEFUN(styp_to_sec_flags, (abfd, hdr),
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
  struct hdr_ext external_symhdr;
  HDRR *internal_symhdr;
  bfd_size_type raw_base;
  bfd_size_type raw_size;
  PTR raw;
  bfd_size_type internal_size;
  struct fdr_ext *fraw_src;
  struct fdr_ext *fraw_end;
  struct fdr *fdr_ptr;
  struct fdr *fdr_end;
  ecoff_symbol_type *internal;
  ecoff_symbol_type *internal_ptr;
  struct ext_ext *eraw_src;
  struct ext_ext *eraw_end;

  /* If we've already read in the symbol table, do nothing.  */
  if (ecoff_data (abfd)->canonical_symbols != NULL)
    return true;

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

  /* Read the entire symbol table at once.  This expression assumes
     that the external symbols are always the last item.  */
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
     waste time and most programs will never look at this data.  The
     only time the linker needs most of the debugging information
     swapped is when linking big-endian and little-endian MIPS object
     files together, which is not a common occurrence.

     We need to look at the fdr to deal with a lot of information in
     the symbols, so we swap them here.  We also canonicalize the
     symbols.  */
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

  internal_size = bfd_get_symcount (abfd) * sizeof (ecoff_symbol_type);
  internal = (ecoff_symbol_type *) bfd_alloc (abfd, internal_size);
  if (internal == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  internal_ptr = internal;
  eraw_src = ecoff_data (abfd)->external_ext;
  eraw_end = eraw_src + internal_symhdr->iextMax;
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
  fdr_end = fdr_ptr + internal_symhdr->ifdMax;
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
  if (! ecoff_slurp_symbol_table (abfd))
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

  if (! ecoff_slurp_symbol_table (abfd))
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
	CONST char *section_name = symbol->section->name;
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
		  printf ("\n      Type: %s",
			  type_to_string (abfd, aux_base, indx, bigendian));

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

/* Routines that need to be written.  */
#define ecoff_write_object_contents (boolean (*) PARAMS ((bfd *))) bfd_false
#define ecoff_set_section_contents (boolean (*) PARAMS ((bfd *, sec_ptr, PTR, file_ptr, bfd_size_type))) bfd_false
#define ecoff_get_reloc_upper_bound (unsigned int (*) PARAMS ((bfd *, sec_ptr))) bfd_0
#define ecoff_canonicalize_reloc (unsigned int (*) PARAMS ((bfd *, sec_ptr, arelent **, struct symbol_cache_entry **))) bfd_0
#define ecoff_set_arch_mach (boolean (*) PARAMS ((bfd *, enum bfd_architecture, unsigned long))) bfd_false
#define ecoff_sizeof_headers (int (*) PARAMS ((bfd *, boolean))) bfd_0

/* get_lineno could be written for ECOFF, but it would currently only
   be useful for linking ECOFF and COFF files together, which doesn't
   seem too likely.  */
#define ecoff_get_lineno (struct lineno_cache_entry *(*)()) bfd_nullvoidptr

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
#define	ecoff_close_and_cleanup		bfd_generic_close_and_cleanup

#define ecoff_bfd_debug_info_start	bfd_void
#define ecoff_bfd_debug_info_end		bfd_void
#define ecoff_bfd_debug_info_accumulate	\
			(void (*) PARAMS ((bfd *, struct sec *))) bfd_void
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
