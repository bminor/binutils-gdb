/* Support for the generic parts of PE/PEI, for BFD.
   Copyright 1995, 1996, 1997, 1998, 1999 Free Software Foundation, Inc.
   Written by Cygnus Solutions.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
Most of this hacked by  Steve Chamberlain,
			sac@cygnus.com

PE/PEI rearrangement (and code added): Donn Terry
                                       Softway Systems, Inc.
*/

/* Hey look, some documentation [and in a place you expect to find it]!

   The main reference for the pei format is "Microsoft Portable Executable
   and Common Object File Format Specification 4.1".  Get it if you need to
   do some serious hacking on this code.

   Another reference:
   "Peering Inside the PE: A Tour of the Win32 Portable Executable
   File Format", MSJ 1994, Volume 9.

   The *sole* difference between the pe format and the pei format is that the
   latter has an MSDOS 2.0 .exe header on the front that prints the message
   "This app must be run under Windows." (or some such).
   (FIXME: Whether that statement is *really* true or not is unknown.
   Are there more subtle differences between pe and pei formats?
   For now assume there aren't.  If you find one, then for God sakes
   document it here!)

   The Microsoft docs use the word "image" instead of "executable" because
   the former can also refer to a DLL (shared library).  Confusion can arise
   because the `i' in `pei' also refers to "image".  The `pe' format can
   also create images (i.e. executables), it's just that to run on a win32
   system you need to use the pei format.

   FIXME: Please add more docs here so the next poor fool that has to hack
   on this code has a chance of getting something accomplished without
   wasting too much time.
*/

#include "libpei.h"

static boolean (*pe_saved_coff_bfd_print_private_bfd_data)
    PARAMS ((bfd *, PTR)) =
#ifndef coff_bfd_print_private_bfd_data
     NULL;
#else
     coff_bfd_print_private_bfd_data;
#undef coff_bfd_print_private_bfd_data
#endif

static boolean pe_print_private_bfd_data PARAMS ((bfd *, PTR));
#define coff_bfd_print_private_bfd_data pe_print_private_bfd_data


static boolean (*pe_saved_coff_bfd_copy_private_bfd_data)
    PARAMS ((bfd *, bfd *)) =
#ifndef coff_bfd_copy_private_bfd_data
     NULL;
#else
     coff_bfd_copy_private_bfd_data;
#undef coff_bfd_copy_private_bfd_data
#endif

static boolean pe_bfd_copy_private_bfd_data PARAMS ((bfd *, bfd *));
#define coff_bfd_copy_private_bfd_data pe_bfd_copy_private_bfd_data

#define coff_mkobject      pe_mkobject
#define coff_mkobject_hook pe_mkobject_hook

static void coff_swap_reloc_in PARAMS ((bfd *, PTR, PTR));
static unsigned int coff_swap_reloc_out PARAMS ((bfd *, PTR, PTR));
static void coff_swap_filehdr_in PARAMS ((bfd *, PTR, PTR));
static void coff_swap_scnhdr_in PARAMS ((bfd *, PTR, PTR));
static boolean pe_mkobject PARAMS ((bfd *));
static PTR pe_mkobject_hook PARAMS ((bfd *, PTR, PTR));

/**********************************************************************/

static void
coff_swap_reloc_in (abfd, src, dst)
     bfd *abfd;
     PTR src;
     PTR dst;
{
  RELOC *reloc_src = (RELOC *) src;
  struct internal_reloc *reloc_dst = (struct internal_reloc *) dst;

  reloc_dst->r_vaddr = bfd_h_get_32(abfd, (bfd_byte *)reloc_src->r_vaddr);
  reloc_dst->r_symndx = bfd_h_get_signed_32(abfd, (bfd_byte *) reloc_src->r_symndx);

  reloc_dst->r_type = bfd_h_get_16(abfd, (bfd_byte *) reloc_src->r_type);

#ifdef SWAP_IN_RELOC_OFFSET
  reloc_dst->r_offset = SWAP_IN_RELOC_OFFSET(abfd,
					     (bfd_byte *) reloc_src->r_offset);
#endif
}


static unsigned int
coff_swap_reloc_out (abfd, src, dst)
     bfd       *abfd;
     PTR	src;
     PTR	dst;
{
  struct internal_reloc *reloc_src = (struct internal_reloc *)src;
  struct external_reloc *reloc_dst = (struct external_reloc *)dst;
  bfd_h_put_32(abfd, reloc_src->r_vaddr, (bfd_byte *) reloc_dst->r_vaddr);
  bfd_h_put_32(abfd, reloc_src->r_symndx, (bfd_byte *) reloc_dst->r_symndx);

  bfd_h_put_16(abfd, reloc_src->r_type, (bfd_byte *)
	       reloc_dst->r_type);

#ifdef SWAP_OUT_RELOC_OFFSET
  SWAP_OUT_RELOC_OFFSET(abfd,
			reloc_src->r_offset,
			(bfd_byte *) reloc_dst->r_offset);
#endif
#ifdef SWAP_OUT_RELOC_EXTRA
  SWAP_OUT_RELOC_EXTRA(abfd,reloc_src, reloc_dst);
#endif
  return RELSZ;
}


static void
coff_swap_filehdr_in (abfd, src, dst)
     bfd            *abfd;
     PTR	     src;
     PTR	     dst;
{
  FILHDR *filehdr_src = (FILHDR *) src;
  struct internal_filehdr *filehdr_dst = (struct internal_filehdr *) dst;
  filehdr_dst->f_magic = bfd_h_get_16(abfd, (bfd_byte *) filehdr_src->f_magic);
  filehdr_dst->f_nscns = bfd_h_get_16(abfd, (bfd_byte *)filehdr_src-> f_nscns);
  filehdr_dst->f_timdat = bfd_h_get_32(abfd, (bfd_byte *)filehdr_src-> f_timdat);

  filehdr_dst->f_nsyms = bfd_h_get_32(abfd, (bfd_byte *)filehdr_src-> f_nsyms);
  filehdr_dst->f_flags = bfd_h_get_16(abfd, (bfd_byte *)filehdr_src-> f_flags);
  filehdr_dst->f_symptr = bfd_h_get_32 (abfd, (bfd_byte *) filehdr_src->f_symptr);

#ifdef COFF_IMAGE_WITH_PE
  /* There are really two magic numbers involved; the magic number
     that says this is a NT executable (PEI) and the magic number that
     determines the architecture.  The former is DOSMAGIC, stored in
     the e_magic field.  The latter is stored in the f_magic field.
     If the NT magic number isn't valid, the architecture magic number
     could be mimicked by some other field (specifically, the number
     of relocs in section 3).  Since this routine can only be called
     correctly for a PEI file, check the e_magic number here, and, if
     it doesn't match, clobber the f_magic number so that we don't get
     a false match.  */
  if (bfd_h_get_16 (abfd, (bfd_byte *) filehdr_src->e_magic) != DOSMAGIC)
    filehdr_dst->f_magic = -1;
#endif

  /* Other people's tools sometimes generate headers with an nsyms but
     a zero symptr.  */
  if (filehdr_dst->f_nsyms != 0 && filehdr_dst->f_symptr == 0)
    {
      filehdr_dst->f_nsyms = 0;
      filehdr_dst->f_flags |= F_LSYMS;
    }

  filehdr_dst->f_opthdr = bfd_h_get_16(abfd, 
				       (bfd_byte *)filehdr_src-> f_opthdr);
}

#ifdef COFF_IMAGE_WITH_PE
#define coff_swap_filehdr_out _bfd_pei_only_swap_filehdr_out
#else
#define coff_swap_filehdr_out _bfd_pe_only_swap_filehdr_out
#endif


static void
coff_swap_scnhdr_in (abfd, ext, in)
     bfd            *abfd;
     PTR	     ext;
     PTR	     in;
{
  SCNHDR *scnhdr_ext = (SCNHDR *) ext;
  struct internal_scnhdr *scnhdr_int = (struct internal_scnhdr *) in;

  memcpy(scnhdr_int->s_name, scnhdr_ext->s_name, sizeof(scnhdr_int->s_name));
  scnhdr_int->s_vaddr =
    GET_SCNHDR_VADDR (abfd, (bfd_byte *) scnhdr_ext->s_vaddr);
  scnhdr_int->s_paddr =
    GET_SCNHDR_PADDR (abfd, (bfd_byte *) scnhdr_ext->s_paddr);
  scnhdr_int->s_size =
    GET_SCNHDR_SIZE (abfd, (bfd_byte *) scnhdr_ext->s_size);
  scnhdr_int->s_scnptr =
    GET_SCNHDR_SCNPTR (abfd, (bfd_byte *) scnhdr_ext->s_scnptr);
  scnhdr_int->s_relptr =
    GET_SCNHDR_RELPTR (abfd, (bfd_byte *) scnhdr_ext->s_relptr);
  scnhdr_int->s_lnnoptr =
    GET_SCNHDR_LNNOPTR (abfd, (bfd_byte *) scnhdr_ext->s_lnnoptr);
  scnhdr_int->s_flags = bfd_h_get_32(abfd, (bfd_byte *) scnhdr_ext->s_flags);

  /* MS handles overflow of line numbers by carrying into the reloc
     field (it appears).  Since it's supposed to be zero for PE
     *IMAGE* format, that's safe.  This is still a bit iffy.  */
#ifdef COFF_IMAGE_WITH_PE
  scnhdr_int->s_nlnno =
    (bfd_h_get_16 (abfd, (bfd_byte *) scnhdr_ext->s_nlnno)
     + (bfd_h_get_16 (abfd, (bfd_byte *) scnhdr_ext->s_nreloc) << 16));
  scnhdr_int->s_nreloc = 0;
#else
  scnhdr_int->s_nreloc = bfd_h_get_16 (abfd,
				       (bfd_byte *) scnhdr_ext->s_nreloc);
  scnhdr_int->s_nlnno = bfd_h_get_16 (abfd,
				      (bfd_byte *) scnhdr_ext->s_nlnno);
#endif

  if (scnhdr_int->s_vaddr != 0) 
    {
      scnhdr_int->s_vaddr += pe_data (abfd)->pe_opthdr.ImageBase;
      scnhdr_int->s_vaddr &= 0xffffffff;
    }

  /* If this section holds uninitialized data, use the virtual size
     (stored in s_paddr) instead of the physical size.  */
  if ((scnhdr_int->s_flags & IMAGE_SCN_CNT_UNINITIALIZED_DATA) != 0)
    {
      scnhdr_int->s_size = scnhdr_int->s_paddr;
      /* This code used to set scnhdr_int->s_paddr to 0.  However,
         coff_set_alignment_hook stores s_paddr in virt_size, which
         only works if it correctly holds the virtual size of the
         section.  */
    }
}

static boolean
pe_mkobject (abfd)
     bfd * abfd;
{
  pe_data_type *pe;
  abfd->tdata.pe_obj_data = 
    (struct pe_tdata *) bfd_zalloc (abfd, sizeof (pe_data_type));

  if (abfd->tdata.pe_obj_data == 0)
    return false;

  pe = pe_data (abfd);

  pe->coff.pe = 1;

  /* in_reloc_p is architecture dependent.  */
  pe->in_reloc_p = in_reloc_p;
  return true;
}

/* Create the COFF backend specific information.  */
static PTR
pe_mkobject_hook (abfd, filehdr, aouthdr)
     bfd * abfd;
     PTR filehdr;
     PTR aouthdr ATTRIBUTE_UNUSED;
{
  struct internal_filehdr *internal_f = (struct internal_filehdr *) filehdr;
  pe_data_type *pe;

  if (pe_mkobject (abfd) == false)
    return NULL;

  pe = pe_data (abfd);
  pe->coff.sym_filepos = internal_f->f_symptr;
  /* These members communicate important constants about the symbol
     table to GDB's symbol-reading code.  These `constants'
     unfortunately vary among coff implementations...  */
  pe->coff.local_n_btmask = N_BTMASK;
  pe->coff.local_n_btshft = N_BTSHFT;
  pe->coff.local_n_tmask = N_TMASK;
  pe->coff.local_n_tshift = N_TSHIFT;
  pe->coff.local_symesz = SYMESZ;
  pe->coff.local_auxesz = AUXESZ;
  pe->coff.local_linesz = LINESZ;

  pe->coff.timestamp = internal_f->f_timdat;

  obj_raw_syment_count (abfd) =
    obj_conv_table_size (abfd) =
      internal_f->f_nsyms;

  pe->real_flags = internal_f->f_flags;

  if ((internal_f->f_flags & F_DLL) != 0)
    pe->dll = 1;

  if ((internal_f->f_flags & IMAGE_FILE_DEBUG_STRIPPED) == 0)
    abfd->flags |= HAS_DEBUG;

#ifdef COFF_IMAGE_WITH_PE
  if (aouthdr) 
    pe->pe_opthdr = ((struct internal_aouthdr *)aouthdr)->pe;
#endif

#ifdef ARM 
  if (! _bfd_coff_arm_set_private_flags (abfd, internal_f->f_flags))
    coff_data (abfd) ->flags = 0;
#endif
  
  return (PTR) pe;
}

static boolean
pe_print_private_bfd_data (abfd, vfile)
     bfd *abfd;
     PTR vfile;
{
  FILE *file = (FILE *) vfile;

  if (!_bfd_pe_print_private_bfd_data_common (abfd, vfile))
    return false;

  if (pe_saved_coff_bfd_print_private_bfd_data != NULL)
    {
      fputc ('\n', file);

      return pe_saved_coff_bfd_print_private_bfd_data (abfd, vfile);
    }

  return true;
}

/* Copy any private info we understand from the input bfd
   to the output bfd.  */

static boolean
pe_bfd_copy_private_bfd_data (ibfd, obfd)
     bfd *ibfd, *obfd;
{
  if (!_bfd_pe_bfd_copy_private_bfd_data_common (ibfd, obfd))
    return false;

  if (pe_saved_coff_bfd_copy_private_bfd_data)
    return pe_saved_coff_bfd_copy_private_bfd_data (ibfd, obfd);

  return true;
}

#define coff_bfd_copy_private_section_data \
  _bfd_pe_bfd_copy_private_section_data

#define coff_get_symbol_info _bfd_pe_get_symbol_info
