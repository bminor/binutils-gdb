/* ECOFF object file format.
   Copyright (C) 1993, 1994 Free Software Foundation, Inc.
   Contributed by Cygnus Support.
   This file was put together by Ian Lance Taylor <ian@cygnus.com>.

   This file is part of GAS.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "as.h"
#include "coff/internal.h"
#include "bfd/libcoff.h"
#include "bfd/libecoff.h"

/* Almost all of the ECOFF support is actually in ecoff.c in the main
   gas directory.  This file mostly just arranges to call that one at
   the right times.  */

/* These are the pseudo-ops we support in this file.  Only those
   relating to debugging information are supported here.

   The following pseudo-ops from the Kane and Heinrich MIPS book
   should be defined here, but are currently unsupported: .aent,
   .bgnb, .endb, .verstamp, .vreg.

   The following pseudo-ops from the Kane and Heinrich MIPS book are
   MIPS CPU specific, and should be defined by tc-mips.c: .alias,
   .extern, .galive, .gjaldef, .gjrlive, .livereg, .noalias, .option,
   .rdata, .sdata, .set.

   The following pseudo-ops from the Kane and Heinrich MIPS book are
   not MIPS CPU specific, but are also not ECOFF specific.  I have
   only listed the ones which are not already in read.c.  It's not
   completely clear where these should be defined, but tc-mips.c is
   probably the most reasonable place: .asciiz, .asm0, .endr, .err,
   .half, .lab, .repeat, .struct, .weakext.  */

const pseudo_typeS obj_pseudo_table[] =
{
  /* COFF style debugging information. .ln is not used; .loc is used
     instead.  */
  { "def",	ecoff_directive_def,	0 },
  { "dim",	ecoff_directive_dim,	0 },
  { "endef",	ecoff_directive_endef,	0 },
  { "file",	ecoff_directive_file,	0 },
  { "scl",	ecoff_directive_scl,	0 },
  { "size",	ecoff_directive_size,	0 },
  { "tag",	ecoff_directive_tag,	0 },
  { "type",	ecoff_directive_type,	0 },
  { "val",	ecoff_directive_val,	0 },

  /* ECOFF specific debugging information.  */
  { "begin",	ecoff_directive_begin,	0 },
  { "bend",	ecoff_directive_bend,	0 },
  { "end",	ecoff_directive_end,	0 },
  { "ent",	ecoff_directive_ent,	0 },
  { "fmask",	ecoff_directive_fmask,	0 },
  { "frame",	ecoff_directive_frame,	0 },
  { "loc",	ecoff_directive_loc,	0 },
  { "mask",	ecoff_directive_mask,	0 },

  /* These are used on Irix.  I don't know how to implement them.  */
  { "bgnb",	s_ignore,		0 },
  { "endb",	s_ignore,		0 },
  { "verstamp",	s_ignore,		0 },

  /* Sentinel.  */
  { NULL }
};

/* Swap out the symbols and debugging information for BFD.  */

void
ecoff_frob_file ()
{
  const struct ecoff_debug_swap * const debug_swap
    = &ecoff_backend (stdoutput)->debug_swap;
  bfd_vma addr;
  asection *sec;
  HDRR *hdr;
  char *buf;
  char *set;

  /* Set the section VMA values.  */
  addr = 0;
  for (sec = stdoutput->sections; sec != (asection *) NULL; sec = sec->next)
    {
      bfd_set_section_vma (stdoutput, sec, addr);
      addr += bfd_section_size (stdoutput, sec);
    }

  /* Build the ECOFF debugging information.  */
  hdr = &ecoff_data (stdoutput)->debug_info.symbolic_header;
  ecoff_build_debug (hdr, &buf, debug_swap);

  /* Finish up the ecoff_tdata structure.  */
  set = buf;
#define SET(ptr, count, type, size) \
  if (hdr->count == 0) \
    ecoff_data (stdoutput)->debug_info.ptr = (type) NULL; \
  else \
    { \
      ecoff_data (stdoutput)->debug_info.ptr = (type) set; \
      set += hdr->count * size; \
    }

  SET (line, cbLine, unsigned char *, sizeof (unsigned char));
  SET (external_dnr, idnMax, PTR, debug_swap->external_dnr_size);
  SET (external_pdr, ipdMax, PTR, debug_swap->external_pdr_size);
  SET (external_sym, isymMax, PTR, debug_swap->external_sym_size);
  SET (external_opt, ioptMax, PTR, debug_swap->external_opt_size);
  SET (external_aux, iauxMax, union aux_ext *, sizeof (union aux_ext));
  SET (ss, issMax, char *, sizeof (char));
  SET (ssext, issExtMax, char *, sizeof (char));
  SET (external_rfd, crfd, PTR, debug_swap->external_rfd_size);
  SET (external_fdr, ifdMax, PTR, debug_swap->external_fdr_size);
  SET (external_ext, iextMax, PTR, debug_swap->external_ext_size);

#undef SET

  /* Fill in the register masks.  */
  {
    asection *regsec;
    struct ecoff_reginfo s;

    regsec = bfd_make_section (stdoutput, REGINFO);
    know (regsec != NULL);

    if (bfd_get_section_contents (stdoutput, regsec, (PTR) &s,
				  (file_ptr) 0, sizeof s) == false)
      as_fatal ("Can't read REGINFO section");

#ifdef TC_MIPS
    /* Fill in the MIPS register masks.  It's probably not worth
       setting up a generic interface for this.  */
    s.gprmask = mips_gprmask;
    s.cprmask[0] = mips_cprmask[0];
    s.cprmask[1] = mips_cprmask[1];
    s.cprmask[2] = mips_cprmask[2];
    s.cprmask[3] = mips_cprmask[3];
#endif

    if (bfd_set_section_contents (stdoutput, regsec, (PTR) &s,
				  (file_ptr) 0, sizeof s) == false)
      as_fatal ("Can't write REGINFO section");
  }
}

/* This is called by the ECOFF code to set the external information
   for a symbol.  We just pass it on to BFD, which expects the swapped
   information to be stored in the native field of the symbol.  */

void
obj_ecoff_set_ext (sym, ext)
     symbolS *sym;
     EXTR *ext;
{
  const struct ecoff_debug_swap * const debug_swap
    = &ecoff_backend (stdoutput)->debug_swap;
  ecoff_symbol_type *esym;

  know (bfd_asymbol_flavour (sym->bsym) == bfd_target_ecoff_flavour);
  esym = ecoffsymbol (sym->bsym);
  esym->local = false;
  esym->native = xmalloc (debug_swap->external_ext_size);
  (*debug_swap->swap_ext_out) (stdoutput, ext, esym->native);
}
