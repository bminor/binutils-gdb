/* Routines to link ECOFF debugging information.
   Copyright 1993 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support, <ian@cygnus.com>.

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
#include "coff/internal.h"
#include "coff/sym.h"
#include "coff/symconst.h"
#include "coff/ecoff.h"

static boolean ecoff_add_bytes PARAMS ((char **buf, char **bufend,
					bfd_size_type need));
static bfd_size_type ecoff_add_string PARAMS ((struct ecoff_debug_info *,
					       FDR *fdr, const char *string));
static void ecoff_align_debug PARAMS ((bfd *abfd,
				       struct ecoff_debug_info *debug,
				       const struct ecoff_debug_swap *swap));

/* The minimum amount of data to allocate.  */
#define ALLOC_SIZE (4064)

/* Add bytes to a buffer.  Return success.  */

static boolean
ecoff_add_bytes (buf, bufend, need)
     char **buf;
     char **bufend;
     bfd_size_type need;
{
  bfd_size_type have;
  bfd_size_type want;
  char *newbuf;

  have = *bufend - *buf;
  if (have > need)
    want = ALLOC_SIZE;
  else
    {
      want = need - have;
      if (want < ALLOC_SIZE)
	want = ALLOC_SIZE;
    }
  if (*buf == NULL)
    newbuf = (char *) malloc (have + want);
  else
    newbuf = (char *) realloc (*buf, have + want);
  if (newbuf == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  *buf = newbuf;
  *bufend = *buf + have + want;
  return true;
}

/* Accumulate the debugging information from INPUT_BFD into
   OUTPUT_BFD.  The INPUT_DEBUG argument points to some ECOFF
   debugging information which we want to link into the information
   pointed to by the OUTPUT_DEBUG argument.  OUTPUT_SWAP and
   INPUT_SWAP point to the swapping information needed.  */

boolean
bfd_ecoff_debug_accumulate (output_bfd, output_debug, output_swap,
			    input_bfd, input_debug, input_swap,
			    relocateable)
     bfd *output_bfd;
     struct ecoff_debug_info *output_debug;
     const struct ecoff_debug_swap *output_swap;
     bfd *input_bfd;
     struct ecoff_debug_info *input_debug;
     const struct ecoff_debug_swap *input_swap;
     boolean relocateable;
{
  void (* const swap_sym_in) PARAMS ((bfd *, PTR, SYMR *))
    = input_swap->swap_sym_in;
  void (* const swap_sym_out) PARAMS ((bfd *, const SYMR *, PTR))
    = output_swap->swap_sym_out;
  void (* const swap_fdr_out) PARAMS ((bfd *, const FDR *, PTR))
    = output_swap->swap_fdr_out;
  void (* const swap_rfd_out) PARAMS ((bfd *, const RFDT *, PTR))
    = output_swap->swap_rfd_out;
  HDRR *output_symhdr;
  HDRR *input_symhdr;
  bfd_vma section_adjust[scMax];
  asection *sec;
  char *sym_out;
  char *lraw_src;
  char *lraw_end;
  char *fdr_ptr;
  char *fdr_end;
  bfd_size_type fdr_add;
  char *fdr_out;

  output_symhdr = &output_debug->symbolic_header;
  input_symhdr = &input_debug->symbolic_header;

  /* Make sure the buffers are large enough.  */
#define CHECK(start, end, count, size)				\
  if ((char *) output_debug->end - (char *) output_debug->start	\
      < (output_symhdr->count + input_symhdr->count) * size)	\
    {								\
      if (! ecoff_add_bytes ((char **) &output_debug->start,	\
			     (char **) &output_debug->end,	\
			     ((output_symhdr->count		\
			       + input_symhdr->count)		\
			      * size)))				\
	return false;						\
    }

  CHECK (line, line_end, cbLine, sizeof (unsigned char));
#if 0
  /* I don't think dense number information is needed.  */
  CHECK (external_dnr, external_dnr_end, idnMax,
	 output_swap->external_dnr_size);
#endif
  CHECK (external_pdr, external_pdr_end, ipdMax,
	 output_swap->external_pdr_size);
  CHECK (external_sym, external_sym_end, isymMax,
	 output_swap->external_sym_size);
  CHECK (external_opt, external_opt_end, ioptMax,
	 output_swap->external_opt_size);
  CHECK (external_aux, external_aux_end, iauxMax,
	 sizeof (union aux_ext));
  CHECK (ss, ss_end, issMax, sizeof (char));
  CHECK (external_fdr, external_fdr_end, ifdMax,
	 output_swap->external_fdr_size);

  /* The RFD's are special, since we create them if needed.  */
  {
    bfd_size_type crfd;

    crfd = input_symhdr->crfd;
    if (input_symhdr->crfd == 0)
      input_symhdr->crfd = input_symhdr->ifdMax;
    CHECK (external_rfd, external_rfd_end, crfd,
	   output_swap->external_rfd_size);
    input_symhdr->crfd = crfd;
  }

#undef CHECK

  /* Use section_adjust to hold the value to add to a symbol in a
     particular section.  */
  memset ((PTR) section_adjust, 0, sizeof section_adjust);

#define SET(name, indx) \
  sec = bfd_get_section_by_name (input_bfd, name); \
  if (sec != NULL) \
    section_adjust[indx] = (sec->output_section->vma \
			    + sec->output_offset \
			    - sec->vma);

  SET (".text", scText);
  SET (".data", scData);
  SET (".bss", scBss);
  SET (".sdata", scSData);
  SET (".sbss", scSBss);
  /* scRdata section may be either .rdata or .rodata.  */
  SET (".rdata", scRData);
  SET (".rodata", scRData);
  SET (".init", scInit);
  SET (".fini", scFini);

#undef SET

  /* Swap in the local symbols, adjust their values, and swap them out
     again.  The external symbols are handled separately.  */
  sym_out = ((char *) output_debug->external_sym
	     + output_symhdr->isymMax * output_swap->external_sym_size);
  lraw_src = (char *) input_debug->external_sym;
  lraw_end = lraw_src + input_symhdr->isymMax * input_swap->external_sym_size;
  for (;  lraw_src < lraw_end;  lraw_src += input_swap->external_sym_size)
    {
      SYMR internal_sym;

      (*swap_sym_in) (input_bfd, (PTR) lraw_src, &internal_sym);

      BFD_ASSERT (internal_sym.sc != scCommon
		  && internal_sym.sc != scSCommon);

      /* Adjust the symbol value if appropriate.  */
      switch (internal_sym.st)
	{
	case stNil:
	  if (ECOFF_IS_STAB (&internal_sym))
	    break;
	  /* Fall through.  */
	case stGlobal:
	case stStatic:
	case stLabel:
	case stProc:
	case stStaticProc:
	  internal_sym.value += section_adjust[internal_sym.sc];
	  break;

	default:
	  break;
	}

      (*swap_sym_out) (output_bfd, &internal_sym, sym_out);
      sym_out += output_swap->external_sym_size;
    }

  /* Copy the information that does not need swapping.  */
  memcpy (output_debug->line + output_symhdr->cbLine,
	  input_debug->line,
	  input_symhdr->cbLine * sizeof (unsigned char));
  memcpy (output_debug->external_aux + output_symhdr->iauxMax,
	  input_debug->external_aux,
	  input_symhdr->iauxMax * sizeof (union aux_ext));
  memcpy (output_debug->ss + output_symhdr->issMax,
	  input_debug->ss,
	  input_symhdr->issMax * sizeof (char));

  /* Some of the information may need to be swapped.  */
  if (output_bfd->xvec->header_byteorder_big_p
      == input_bfd->xvec->header_byteorder_big_p)
    {
      /* The two BFD's have the same endianness, so memcpy will
	 suffice.  */
#if 0
      /* I don't think dense number information is needed.  */
      BFD_ASSERT (output_swap->external_dnr_size
		  == input_swap->external_dnr_size);
      if (input_symhdr->idnMax > 0)
	memcpy (((char *) output_debug->external_dnr
		 + output_symhdr->idnMax * output_swap->external_dnr_size),
		input_debug->external_dnr,
		input_symhdr->idnMax * output_swap->external_dnr_size);
#endif
      BFD_ASSERT (output_swap->external_pdr_size
		  == input_swap->external_pdr_size);
      if (input_symhdr->ipdMax > 0)
	memcpy (((char *) output_debug->external_pdr
		 + output_symhdr->ipdMax * output_swap->external_pdr_size),
		input_debug->external_pdr,
		input_symhdr->ipdMax * output_swap->external_pdr_size);
      BFD_ASSERT (output_swap->external_opt_size
		  == input_swap->external_opt_size);
      if (input_symhdr->ioptMax > 0)
	memcpy (((char *) output_debug->external_opt
		 + output_symhdr->ioptMax * output_swap->external_opt_size),
		input_debug->external_opt,
		input_symhdr->ioptMax * output_swap->external_opt_size);
    }
  else
    {
      bfd_size_type outsz, insz;
      char *in;
      char *end;
      char *out;

      /* The two BFD's have different endianness, so we must swap
	 everything in and out.  This code would always work, but it
	 would be slow in the normal case.  */
#if 0
      /* I don't think dense number information is needed.  */
      outsz = output_swap->external_dnr_size;
      insz = input_swap->external_dnr_size;
      in = (char *) input_debug->external_dnr;
      end = in + input_symhdr->idnMax * insz;
      out = ((char *) output_debug->external_dnr
	     + output_symhdr->idnMax * outsz);
      for (; in < end; in += insz, out += outsz)
	{
	  DNR dnr;

	  (*input_swap->swap_dnr_in) (input_bfd, in, &dnr);
	  (*output_swap->swap_dnr_out) (output_bfd, &dnr, out);
	}
#endif

      outsz = output_swap->external_pdr_size;
      insz = input_swap->external_pdr_size;
      in = (char *) input_debug->external_pdr;
      end = in + input_symhdr->ipdMax * insz;
      out = ((char *) output_debug->external_pdr
	     + output_symhdr->ipdMax * outsz);
      for (; in < end; in += insz, out += outsz)
	{
	  PDR pdr;

	  (*input_swap->swap_pdr_in) (input_bfd, in, &pdr);
	  (*output_swap->swap_pdr_out) (output_bfd, &pdr, out);
	}

      outsz = output_swap->external_opt_size;
      insz = input_swap->external_opt_size;
      in = (char *) input_debug->external_opt;
      end = in + input_symhdr->ioptMax * insz;
      out = ((char *) output_debug->external_opt
	     + output_symhdr->ioptMax * outsz);
      for (; in < end; in += insz, out += outsz)
	{
	  OPTR opt;

	  (*input_swap->swap_opt_in) (input_bfd, in, &opt);
	  (*output_swap->swap_opt_out) (output_bfd, &opt, out);
	}
    }

  /* Set ifdbase so that the external symbols know how to adjust their
     ifd values.  */
  input_debug->ifdbase = output_symhdr->ifdMax;

  /* We need to handle the FDR's whether they are swapped or not.  */
  if (input_debug->fdr != (FDR *) NULL)
    {
      fdr_ptr = (char *) input_debug->fdr;
      fdr_add = sizeof (FDR);
    }
  else
    {
      fdr_ptr = (char *) input_debug->external_fdr;
      fdr_add = input_swap->external_fdr_size;
    }
  fdr_end = fdr_ptr + input_symhdr->ifdMax * fdr_add;
  fdr_out = ((char *) output_debug->external_fdr
	     + output_symhdr->ifdMax * output_swap->external_fdr_size);
  for (;
       fdr_ptr < fdr_end;
       fdr_ptr += fdr_add, fdr_out += output_swap->external_fdr_size)
    {
      FDR fdr;

      if (input_debug->fdr != (FDR *) NULL)
	fdr = *(FDR *) fdr_ptr;
      else
	(*input_swap->swap_fdr_in) (input_bfd, (PTR) fdr_ptr, &fdr);

      /* FIXME: It is conceivable that this FDR points to the .init or
	 .fini section, in which case this will not do the right
	 thing.  */
      fdr.adr += section_adjust[scText];

      fdr.issBase += output_symhdr->issMax;
      fdr.isymBase += output_symhdr->isymMax;
      fdr.ilineBase += output_symhdr->ilineMax;
      fdr.ioptBase += output_symhdr->ioptMax;
      fdr.ipdFirst += output_symhdr->ipdMax;
      fdr.iauxBase += output_symhdr->iauxMax;
      fdr.rfdBase += output_symhdr->crfd;

      /* If there are no RFD's, we are going to add some.  We don't
	 want to adjust crfd for this, so that all the FDR's can share
	 the RFD's.  */
      if (input_symhdr->crfd == 0)
	fdr.crfd = input_symhdr->ifdMax;

      if (fdr.cbLine != 0)
	fdr.cbLineOffset += output_symhdr->cbLine;

      (*swap_fdr_out) (output_bfd, &fdr, fdr_out);
    }

  if (input_symhdr->crfd > 0)
    {
      void (* const swap_rfd_in) PARAMS ((bfd *, PTR, RFDT *))
	= input_swap->swap_rfd_in;
      bfd_size_type outsz, insz;
      char *rfd_in;
      char *rfd_end;
      char *rfd_out;

      /* Swap and adjust the RFD's.  RFD's are only created by the
	 linker, so this will only be necessary if one of the input
	 files is the result of a partial link.  Presumably all
	 necessary RFD's are present.  */
      outsz = output_swap->external_rfd_size;
      insz = input_swap->external_rfd_size;
      rfd_in = (char *) input_debug->external_rfd;
      rfd_end = rfd_in + input_symhdr->crfd * insz;
      rfd_out = ((char *) output_debug->external_rfd
		 + output_symhdr->crfd * outsz);
      for (;
	   rfd_in < rfd_end;
	   rfd_in += insz, rfd_out += outsz)
	{
	  RFDT rfd;

	  (*swap_rfd_in) (input_bfd, rfd_in, &rfd);
	  rfd += output_symhdr->ifdMax;
	  (*swap_rfd_out) (output_bfd, &rfd, rfd_out);
	}
      output_symhdr->crfd += input_symhdr->crfd;
    }
  else
    {
      bfd_size_type outsz;
      char *rfd_out;
      char *rfd_end;
      RFDT rfd;

      /* Create RFD's.  Some of the debugging information includes
	 relative file indices.  These indices are taken as indices to
	 the RFD table if there is one, or to the global table if
	 there is not.  If we did not create RFD's, we would have to
	 parse and adjust all the debugging information which contains
	 file indices.  */
      outsz = output_swap->external_rfd_size;
      rfd = output_symhdr->ifdMax;
      rfd_out = ((char *) output_debug->external_rfd
		 + output_symhdr->crfd * outsz);
      rfd_end = (rfd_out + input_symhdr->ifdMax * outsz);
      for (; rfd_out < rfd_end; rfd_out += outsz, rfd++)
	(*swap_rfd_out) (output_bfd, &rfd, rfd_out);
      output_symhdr->crfd += input_symhdr->ifdMax;
    }

  /* Update the counts.  */
  output_symhdr->ilineMax += input_symhdr->ilineMax;
  output_symhdr->cbLine += input_symhdr->cbLine;
#if 0
  /* I don't think dense number information is needed.  */
  output_symhdr->idnMax += input_symhdr->idnMax;
#endif
  output_symhdr->ipdMax += input_symhdr->ipdMax;
  output_symhdr->isymMax += input_symhdr->isymMax;
  output_symhdr->ioptMax += input_symhdr->ioptMax;
  output_symhdr->iauxMax += input_symhdr->iauxMax;
  output_symhdr->issMax += input_symhdr->issMax;
  output_symhdr->ifdMax += input_symhdr->ifdMax;

  return true;
}

/* Add a string to the debugging information we are accumulating.
   Return the offset from the fdr string base.  */

static bfd_size_type
ecoff_add_string (output, fdr, string)
     struct ecoff_debug_info *output;
     FDR *fdr;
     const char *string;
{
  HDRR *symhdr;
  size_t len;
  bfd_size_type ret;

  symhdr = &output->symbolic_header;
  len = strlen (string);
  if (output->ss_end - output->ss < symhdr->issMax + len + 1)
    {
      if (ecoff_add_bytes (&output->ss, &output->ss_end,
			   symhdr->issMax + len + 1) == false)
	return -1;
    }
  memcpy (output->ss + symhdr->issMax, string, len + 1);
  ret = fdr->cbSs;
  symhdr->issMax += len + 1;
  fdr->cbSs += len + 1;
  return ret;
}

/* Add debugging information from a non-ECOFF file.  */

boolean
bfd_ecoff_debug_link_other (output_bfd, output_debug, output_swap, input_bfd)
     bfd *output_bfd;
     struct ecoff_debug_info *output_debug;
     const struct ecoff_debug_swap *output_swap;
     bfd *input_bfd;
{
  void (* const swap_sym_out) PARAMS ((bfd *, const SYMR *, PTR))
    = output_swap->swap_sym_out;
  HDRR *output_symhdr = &output_debug->symbolic_header;
  FDR fdr;
  asection *sec;
  asymbol **symbols;
  asymbol **sym_ptr;
  asymbol **sym_end;

  memset (&fdr, 0, sizeof fdr);

  sec = bfd_get_section_by_name (input_bfd, ".text");
  if (sec != NULL)
    fdr.adr = sec->output_section->vma + sec->output_offset;
  else
    {
      /* FIXME: What about .init or .fini?  */
      fdr.adr = 0;
    }

  fdr.issBase = output_symhdr->issMax;
  fdr.cbSs = 0;
  fdr.rss = ecoff_add_string (output_debug, &fdr,
			      bfd_get_filename (input_bfd));
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
      internal_sym.iss = ecoff_add_string (output_debug, &fdr,
					   (*sym_ptr)->name);

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

      if (((char *) output_debug->external_sym_end
	   - (char *) output_debug->external_sym)
	  < (output_symhdr->isymMax + 1) * output_swap->external_sym_size)
	{
	  if (! ecoff_add_bytes ((char **) &output_debug->external_sym,
				 (char **) &output_debug->external_sym_end,
				 ((output_symhdr->isymMax + 1)
				  * output_swap->external_sym_size)))
	    return false;
	}

      (*swap_sym_out) (output_bfd, &internal_sym,
		       ((char *) output_debug->external_sym
			+ (output_symhdr->isymMax
			   * output_swap->external_sym_size)));
      ++fdr.csym;
      ++output_symhdr->isymMax;
    }

  bfd_release (output_bfd, (PTR) symbols);

  if (((char *) output_debug->external_fdr_end
       - (char *) output_debug->external_fdr)
      < (output_symhdr->ifdMax + 1) * output_swap->external_fdr_size)
    {
      if (! ecoff_add_bytes ((char **) &output_debug->external_fdr,
			     (char **) &output_debug->external_fdr_end,
			     ((output_symhdr->ifdMax + 1)
			      * output_swap->external_fdr_size)))
	return false;
    }

  /* Leave everything else in the FDR zeroed out.  This will cause
     the lang field to be langC.  The fBigendian field will
     indicate little endian format, but it doesn't matter because
     it only applies to aux fields and there are none.  */
  (*output_swap->swap_fdr_out)
    (output_bfd, &fdr,
     ((char *) output_debug->external_fdr
      + output_symhdr->ifdMax * output_swap->external_fdr_size));
  ++output_symhdr->ifdMax;
  return true;
}

/* Set up ECOFF debugging information for the external symbols.  */

boolean
bfd_ecoff_debug_externals (abfd, debug, swap, relocateable, get_extr,
			   set_index)
     bfd *abfd;
     struct ecoff_debug_info *debug;
     const struct ecoff_debug_swap *swap;
     boolean relocateable;
     boolean (*get_extr) PARAMS ((asymbol *, EXTR *));
     void (*set_index) PARAMS ((asymbol *, bfd_size_type));
{
  const bfd_size_type external_ext_size = swap->external_ext_size;
  void (* const swap_ext_out) PARAMS ((bfd *, const EXTR *, PTR))
    = swap->swap_ext_out;
  HDRR * const symhdr = &debug->symbolic_header;
  asymbol **sym_ptr_ptr;
  size_t c;

  sym_ptr_ptr = bfd_get_outsymbols (abfd);
  if (sym_ptr_ptr == NULL)
    return true;

  for (c = bfd_get_symcount (abfd); c > 0; c--, sym_ptr_ptr++)
    {
      asymbol *sym_ptr;
      EXTR esym;

      sym_ptr = *sym_ptr_ptr;

      /* Get the external symbol information.  */
      if ((*get_extr) (sym_ptr, &esym) == false)
	continue;

      /* If we're producing an executable, move common symbols into
	 bss.  */
      if (relocateable == false)
	{
	  if (esym.asym.sc == scCommon)
	    esym.asym.sc = scBss;
	  else if (esym.asym.sc == scSCommon)
	    esym.asym.sc = scSBss;
	}

      if (debug->ssext_end - debug->ssext
	  < symhdr->issExtMax + strlen (sym_ptr->name) + 1)
	{
	  if (ecoff_add_bytes ((char **) &debug->ssext,
			       (char **) &debug->ssext_end,
			       symhdr->issExtMax + strlen (sym_ptr->name) + 1)
	      == false)
	    return false;
	}
      if ((char *) debug->external_ext_end - (char *) debug->external_ext
	  < (symhdr->iextMax + 1) * external_ext_size)
	{
	  if (ecoff_add_bytes ((char **) &debug->external_ext,
			       (char **) &debug->external_ext_end,
			       (symhdr->iextMax + 1) * external_ext_size)
	      == false)
	    return false;
	}

      esym.asym.iss = symhdr->issExtMax;

      if (bfd_is_com_section (sym_ptr->section)
	  || sym_ptr->section == &bfd_und_section)
	esym.asym.value = sym_ptr->value;
      else
	esym.asym.value = (sym_ptr->value
			   + sym_ptr->section->output_offset
			   + sym_ptr->section->output_section->vma);

      (*swap_ext_out) (abfd, &esym,
		       ((char *) debug->external_ext
			+ symhdr->iextMax * swap->external_ext_size));

      if (set_index)
	(*set_index) (sym_ptr, symhdr->iextMax);

      ++symhdr->iextMax;

      strcpy (debug->ssext + symhdr->issExtMax, sym_ptr->name);
      symhdr->issExtMax += strlen (sym_ptr->name) + 1;
    }

  return true;
}

/* Align the ECOFF debugging information.  */

static void
ecoff_align_debug (abfd, debug, swap)
     bfd *abfd;
     struct ecoff_debug_info *debug;
     const struct ecoff_debug_swap *swap;
{
  HDRR * const symhdr = &debug->symbolic_header;
  bfd_size_type debug_align, aux_align, add;

  /* Adjust the counts so that structures are aligned.  The alignment
     of ALLOC_SIZE ensures that we do not have to worry about running
     off the end of the memory block when doing the memset.  */
  debug_align = swap->debug_align;
  aux_align = debug_align / sizeof (union aux_ext);

  add = debug_align - (symhdr->cbLine & (debug_align - 1));
  if (add != debug_align)
    {
      memset (debug->line + symhdr->cbLine, 0, add);
      symhdr->cbLine += add;
    }

  add = debug_align - (symhdr->issMax & (debug_align - 1));
  if (add != debug_align)
    {
      memset (debug->ss + symhdr->issMax, 0, add);
      symhdr->issMax += add;
    }

  add = debug_align - (symhdr->issExtMax & (debug_align - 1));
  if (add != debug_align)
    {
      memset (debug->ssext + symhdr->issExtMax, 0, add);
      symhdr->issExtMax += add;
    }

  add = aux_align - (symhdr->iauxMax & (aux_align - 1));
  if (add != aux_align)
    {
      memset (debug->external_aux + symhdr->iauxMax, 0,
	      add * sizeof (union aux_ext));
      symhdr->iauxMax += add;
    }
}

/* Return the size required by the ECOFF debugging information.  */

bfd_size_type
bfd_ecoff_debug_size (abfd, debug, swap)
     bfd *abfd;
     struct ecoff_debug_info *debug;
     const struct ecoff_debug_swap *swap;
{
  bfd_size_type tot;

  ecoff_align_debug (abfd, debug, swap);
  tot = swap->external_hdr_size;

#define ADD(count, size) \
  tot += debug->symbolic_header.count * size

  ADD (cbLine, sizeof (unsigned char));
  ADD (idnMax, swap->external_dnr_size);
  ADD (ipdMax, swap->external_pdr_size);
  ADD (isymMax, swap->external_sym_size);
  ADD (ioptMax, swap->external_opt_size);
  ADD (iauxMax, sizeof (union aux_ext));
  ADD (issMax, sizeof (char));
  ADD (issExtMax, sizeof (char));
  ADD (ifdMax, swap->external_fdr_size);
  ADD (crfd, swap->external_rfd_size);
  ADD (iextMax, swap->external_ext_size);

#undef ADD

  return tot;
}

/* Write out the ECOFF debugging information.  This function assumes
   that the information (the pointers and counts) in *DEBUG have been
   set correctly.  WHERE is the position in the file to write the
   information to.  This function fills in the file offsets in the
   symbolic header.  */

boolean
bfd_ecoff_write_debug (abfd, debug, swap, where)
     bfd *abfd;
     struct ecoff_debug_info *debug;
     const struct ecoff_debug_swap *swap;
     file_ptr where;
{
  HDRR * const symhdr = &debug->symbolic_header;
  char *buff;

  ecoff_align_debug (abfd, debug, swap);

  /* Go to the right location in the file.  */
  if (bfd_seek (abfd, where, SEEK_SET) != 0)
    return false;

  where += swap->external_hdr_size;

  /* Fill in the file offsets.  */
#define SET(offset, count, size) \
  if (symhdr->count == 0) \
    symhdr->offset = 0; \
  else \
    { \
      symhdr->offset = where; \
      where += symhdr->count * size; \
    }

  SET (cbLineOffset, cbLine, sizeof (unsigned char));
  SET (cbDnOffset, idnMax, swap->external_dnr_size);
  SET (cbPdOffset, ipdMax, swap->external_pdr_size);
  SET (cbSymOffset, isymMax, swap->external_sym_size);
  SET (cbOptOffset, ioptMax, swap->external_opt_size);
  SET (cbAuxOffset, iauxMax, sizeof (union aux_ext));
  SET (cbSsOffset, issMax, sizeof (char));
  SET (cbSsExtOffset, issExtMax, sizeof (char));
  SET (cbFdOffset, ifdMax, swap->external_fdr_size);
  SET (cbRfdOffset, crfd, swap->external_rfd_size);
  SET (cbExtOffset, iextMax, swap->external_ext_size);
#undef SET

  buff = (PTR) alloca (swap->external_hdr_size);
  (*swap->swap_hdr_out) (abfd, symhdr, buff);
  if (bfd_write (buff, 1, swap->external_hdr_size, abfd)
      != swap->external_hdr_size)
    return false;

#define WRITE(ptr, count, size, offset) \
  BFD_ASSERT (symhdr->offset == 0 || bfd_tell (abfd) == symhdr->offset); \
  if (bfd_write (debug->ptr, size, symhdr->count, abfd) \
      != size * symhdr->count) \
    return false;

  WRITE (line, cbLine, sizeof (unsigned char), cbLineOffset);
  WRITE (external_dnr, idnMax, swap->external_dnr_size, cbDnOffset);
  WRITE (external_pdr, ipdMax, swap->external_pdr_size, cbPdOffset);
  WRITE (external_sym, isymMax, swap->external_sym_size, cbSymOffset);
  WRITE (external_opt, ioptMax, swap->external_opt_size, cbOptOffset);
  WRITE (external_aux, iauxMax, sizeof (union aux_ext), cbAuxOffset);
  WRITE (ss, issMax, sizeof (char), cbSsOffset);
  WRITE (ssext, issExtMax, sizeof (char), cbSsExtOffset);
  WRITE (external_fdr, ifdMax, swap->external_fdr_size, cbFdOffset);
  WRITE (external_rfd, crfd, swap->external_rfd_size, cbRfdOffset);
  WRITE (external_ext, iextMax, swap->external_ext_size, cbExtOffset);
#undef WRITE

  return true;
}
