/* Support for the generic parts of most COFF variants, for BFD.
   Copyright 1995 Free Software Foundation, Inc.
   Written by Cygnus Support.

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
*/



#define coff_bfd_print_private_bfd_data pe_print_private_bfd_data
#define coff_mkobject pe_mkobject
#define coff_mkobject_hook pe_mkobject_hook


#ifndef GET_FCN_LNNOPTR
#define GET_FCN_LNNOPTR(abfd, ext)  bfd_h_get_32(abfd, (bfd_byte *) ext->x_sym.x_fcnary.x_fcn.x_lnnoptr)
#endif

#ifndef GET_FCN_ENDNDX
#define GET_FCN_ENDNDX(abfd, ext)  bfd_h_get_32(abfd, (bfd_byte *) ext->x_sym.x_fcnary.x_fcn.x_endndx)
#endif

#ifndef PUT_FCN_LNNOPTR
#define PUT_FCN_LNNOPTR(abfd, in, ext)  bfd_h_put_32(abfd,  in, (bfd_byte *) ext->x_sym.x_fcnary.x_fcn.x_lnnoptr)
#endif
#ifndef PUT_FCN_ENDNDX
#define PUT_FCN_ENDNDX(abfd, in, ext) bfd_h_put_32(abfd, in, (bfd_byte *) ext->x_sym.x_fcnary.x_fcn.x_endndx)
#endif
#ifndef GET_LNSZ_LNNO
#define GET_LNSZ_LNNO(abfd, ext) bfd_h_get_16(abfd, (bfd_byte *) ext->x_sym.x_misc.x_lnsz.x_lnno)
#endif
#ifndef GET_LNSZ_SIZE
#define GET_LNSZ_SIZE(abfd, ext) bfd_h_get_16(abfd, (bfd_byte *) ext->x_sym.x_misc.x_lnsz.x_size)
#endif
#ifndef PUT_LNSZ_LNNO
#define PUT_LNSZ_LNNO(abfd, in, ext) bfd_h_put_16(abfd, in, (bfd_byte *)ext->x_sym.x_misc.x_lnsz.x_lnno)
#endif
#ifndef PUT_LNSZ_SIZE
#define PUT_LNSZ_SIZE(abfd, in, ext) bfd_h_put_16(abfd, in, (bfd_byte*) ext->x_sym.x_misc.x_lnsz.x_size)
#endif
#ifndef GET_SCN_SCNLEN
#define GET_SCN_SCNLEN(abfd,  ext) bfd_h_get_32(abfd, (bfd_byte *) ext->x_scn.x_scnlen)
#endif
#ifndef GET_SCN_NRELOC
#define GET_SCN_NRELOC(abfd,  ext) bfd_h_get_16(abfd, (bfd_byte *)ext->x_scn.x_nreloc)
#endif
#ifndef GET_SCN_NLINNO
#define GET_SCN_NLINNO(abfd, ext)  bfd_h_get_16(abfd, (bfd_byte *)ext->x_scn.x_nlinno)
#endif
#ifndef PUT_SCN_SCNLEN
#define PUT_SCN_SCNLEN(abfd,in, ext) bfd_h_put_32(abfd, in, (bfd_byte *) ext->x_scn.x_scnlen)
#endif
#ifndef PUT_SCN_NRELOC
#define PUT_SCN_NRELOC(abfd,in, ext) bfd_h_put_16(abfd, in, (bfd_byte *)ext->x_scn.x_nreloc)
#endif
#ifndef PUT_SCN_NLINNO
#define PUT_SCN_NLINNO(abfd,in, ext)  bfd_h_put_16(abfd,in, (bfd_byte  *) ext->x_scn.x_nlinno)
#endif
#ifndef GET_LINENO_LNNO
#define GET_LINENO_LNNO(abfd, ext) bfd_h_get_16(abfd, (bfd_byte *) (ext->l_lnno));
#endif
#ifndef PUT_LINENO_LNNO
#define PUT_LINENO_LNNO(abfd,val, ext) bfd_h_put_16(abfd,val,  (bfd_byte *) (ext->l_lnno));
#endif

/* The f_symptr field in the filehdr is sometimes 64 bits.  */
#ifndef GET_FILEHDR_SYMPTR
#define GET_FILEHDR_SYMPTR bfd_h_get_32
#endif
#ifndef PUT_FILEHDR_SYMPTR
#define PUT_FILEHDR_SYMPTR bfd_h_put_32
#endif

/* Some fields in the aouthdr are sometimes 64 bits.  */
#ifndef GET_AOUTHDR_TSIZE
#define GET_AOUTHDR_TSIZE bfd_h_get_32
#endif
#ifndef PUT_AOUTHDR_TSIZE
#define PUT_AOUTHDR_TSIZE bfd_h_put_32
#endif
#ifndef GET_AOUTHDR_DSIZE
#define GET_AOUTHDR_DSIZE bfd_h_get_32
#endif
#ifndef PUT_AOUTHDR_DSIZE
#define PUT_AOUTHDR_DSIZE bfd_h_put_32
#endif
#ifndef GET_AOUTHDR_BSIZE
#define GET_AOUTHDR_BSIZE bfd_h_get_32
#endif
#ifndef PUT_AOUTHDR_BSIZE
#define PUT_AOUTHDR_BSIZE bfd_h_put_32
#endif
#ifndef GET_AOUTHDR_ENTRY
#define GET_AOUTHDR_ENTRY bfd_h_get_32
#endif
#ifndef PUT_AOUTHDR_ENTRY
#define PUT_AOUTHDR_ENTRY bfd_h_put_32
#endif
#ifndef GET_AOUTHDR_TEXT_START
#define GET_AOUTHDR_TEXT_START bfd_h_get_32
#endif
#ifndef PUT_AOUTHDR_TEXT_START
#define PUT_AOUTHDR_TEXT_START bfd_h_put_32
#endif
#ifndef GET_AOUTHDR_DATA_START
#define GET_AOUTHDR_DATA_START bfd_h_get_32
#endif
#ifndef PUT_AOUTHDR_DATA_START
#define PUT_AOUTHDR_DATA_START bfd_h_put_32
#endif

/* Some fields in the scnhdr are sometimes 64 bits.  */
#ifndef GET_SCNHDR_PADDR
#define GET_SCNHDR_PADDR bfd_h_get_32
#endif
#ifndef PUT_SCNHDR_PADDR
#define PUT_SCNHDR_PADDR bfd_h_put_32
#endif
#ifndef GET_SCNHDR_VADDR
#define GET_SCNHDR_VADDR bfd_h_get_32
#endif
#ifndef PUT_SCNHDR_VADDR
#define PUT_SCNHDR_VADDR bfd_h_put_32
#endif
#ifndef GET_SCNHDR_SIZE
#define GET_SCNHDR_SIZE bfd_h_get_32
#endif
#ifndef PUT_SCNHDR_SIZE
#define PUT_SCNHDR_SIZE bfd_h_put_32
#endif
#ifndef GET_SCNHDR_SCNPTR
#define GET_SCNHDR_SCNPTR bfd_h_get_32
#endif
#ifndef PUT_SCNHDR_SCNPTR
#define PUT_SCNHDR_SCNPTR bfd_h_put_32
#endif
#ifndef GET_SCNHDR_RELPTR
#define GET_SCNHDR_RELPTR bfd_h_get_32
#endif
#ifndef PUT_SCNHDR_RELPTR
#define PUT_SCNHDR_RELPTR bfd_h_put_32
#endif
#ifndef GET_SCNHDR_LNNOPTR
#define GET_SCNHDR_LNNOPTR bfd_h_get_32
#endif
#ifndef PUT_SCNHDR_LNNOPTR
#define PUT_SCNHDR_LNNOPTR bfd_h_put_32
#endif



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
  return sizeof(struct external_reloc);
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
  filehdr_dst->f_symptr =
    bfd_h_get_32 (abfd, (bfd_byte *) filehdr_src->f_symptr);
  filehdr_dst->f_nsyms = bfd_h_get_32(abfd, (bfd_byte *)filehdr_src-> f_nsyms);
  filehdr_dst->f_opthdr = bfd_h_get_16(abfd, (bfd_byte *)filehdr_src-> f_opthdr);
  filehdr_dst->f_flags = bfd_h_get_16(abfd, (bfd_byte *)filehdr_src-> f_flags);
}

static  unsigned int
coff_swap_filehdr_out (abfd, in, out)
     bfd       *abfd;
     PTR	in;
     PTR	out;
{
  struct internal_filehdr *filehdr_in = (struct internal_filehdr *)in;
  FILHDR *filehdr_out = (FILHDR *)out;

  if (bfd_get_section_by_name (abfd, ".reloc"))
    filehdr_in->f_flags &= ~F_RELFLG;

  if (pe_data (abfd)->dll)
    filehdr_in->f_flags |= F_DLL;

  filehdr_in->pe.e_magic    = DOSMAGIC;
  filehdr_in->pe.e_cblp     = 0x90;
  filehdr_in->pe.e_cp       = 0x3;
  filehdr_in->pe.e_crlc     = 0x0;
  filehdr_in->pe.e_cparhdr  = 0x4;
  filehdr_in->pe.e_minalloc = 0x0;
  filehdr_in->pe.e_maxalloc = 0xffff;
  filehdr_in->pe.e_ss       = 0x0;
  filehdr_in->pe.e_sp       = 0xb8;
  filehdr_in->pe.e_csum     = 0x0;
  filehdr_in->pe.e_ip       = 0x0;
  filehdr_in->pe.e_cs       = 0x0;
  filehdr_in->pe.e_lfarlc   = 0x40;
  filehdr_in->pe.e_ovno     = 0x0;
  {
    int idx;
    for (idx=0; idx < 4; idx++)
      filehdr_in->pe.e_res[idx] = 0x0;
  }
  filehdr_in->pe.e_oemid   = 0x0;
  filehdr_in->pe.e_oeminfo = 0x0;
  {
    int idx;
    for (idx=0; idx < 10; idx++)
      filehdr_in->pe.e_res2[idx] = 0x0;
  }
  filehdr_in->pe.e_lfanew = 0x80;

  /* this next collection of data are mostly just characters.  It appears
     to be constant within the headers put on NT exes */
  filehdr_in->pe.dos_message[0]  = 0x0eba1f0e;
  filehdr_in->pe.dos_message[1]  = 0xcd09b400;
  filehdr_in->pe.dos_message[2]  = 0x4c01b821;
  filehdr_in->pe.dos_message[3]  = 0x685421cd;
  filehdr_in->pe.dos_message[4]  = 0x70207369;
  filehdr_in->pe.dos_message[5]  = 0x72676f72;
  filehdr_in->pe.dos_message[6]  = 0x63206d61;
  filehdr_in->pe.dos_message[7]  = 0x6f6e6e61;
  filehdr_in->pe.dos_message[8]  = 0x65622074;
  filehdr_in->pe.dos_message[9]  = 0x6e757220;
  filehdr_in->pe.dos_message[10] = 0x206e6920;
  filehdr_in->pe.dos_message[11] = 0x20534f44;
  filehdr_in->pe.dos_message[12] = 0x65646f6d;
  filehdr_in->pe.dos_message[13] = 0x0a0d0d2e;
  filehdr_in->pe.dos_message[14] = 0x24;
  filehdr_in->pe.dos_message[15] = 0x0;
  filehdr_in->pe.nt_signature = NT_SIGNATURE;



  bfd_h_put_16(abfd, filehdr_in->f_magic, (bfd_byte *) filehdr_out->f_magic);
  bfd_h_put_16(abfd, filehdr_in->f_nscns, (bfd_byte *) filehdr_out->f_nscns);

  bfd_h_put_32(abfd, time (0), (bfd_byte *) filehdr_out->f_timdat);
  PUT_FILEHDR_SYMPTR (abfd, (bfd_vma) filehdr_in->f_symptr,
		      (bfd_byte *) filehdr_out->f_symptr);
  bfd_h_put_32(abfd, filehdr_in->f_nsyms, (bfd_byte *) filehdr_out->f_nsyms);
  bfd_h_put_16(abfd, filehdr_in->f_opthdr, (bfd_byte *) filehdr_out->f_opthdr);
  bfd_h_put_16(abfd, filehdr_in->f_flags, (bfd_byte *) filehdr_out->f_flags);


  /* put in extra dos header stuff.  This data remains essentially
     constant, it just has to be tacked on to the beginning of all exes 
     for NT */
  bfd_h_put_16(abfd, filehdr_in->pe.e_magic, (bfd_byte *) filehdr_out->e_magic);
  bfd_h_put_16(abfd, filehdr_in->pe.e_cblp, (bfd_byte *) filehdr_out->e_cblp);
  bfd_h_put_16(abfd, filehdr_in->pe.e_cp, (bfd_byte *) filehdr_out->e_cp);
  bfd_h_put_16(abfd, filehdr_in->pe.e_crlc, (bfd_byte *) filehdr_out->e_crlc);
  bfd_h_put_16(abfd, filehdr_in->pe.e_cparhdr, 
	       (bfd_byte *) filehdr_out->e_cparhdr);
  bfd_h_put_16(abfd, filehdr_in->pe.e_minalloc, 
	       (bfd_byte *) filehdr_out->e_minalloc);
  bfd_h_put_16(abfd, filehdr_in->pe.e_maxalloc, 
	       (bfd_byte *) filehdr_out->e_maxalloc);
  bfd_h_put_16(abfd, filehdr_in->pe.e_ss, (bfd_byte *) filehdr_out->e_ss);
  bfd_h_put_16(abfd, filehdr_in->pe.e_sp, (bfd_byte *) filehdr_out->e_sp);
  bfd_h_put_16(abfd, filehdr_in->pe.e_csum, (bfd_byte *) filehdr_out->e_csum);
  bfd_h_put_16(abfd, filehdr_in->pe.e_ip, (bfd_byte *) filehdr_out->e_ip);
  bfd_h_put_16(abfd, filehdr_in->pe.e_cs, (bfd_byte *) filehdr_out->e_cs);
  bfd_h_put_16(abfd, filehdr_in->pe.e_lfarlc, (bfd_byte *) filehdr_out->e_lfarlc);
  bfd_h_put_16(abfd, filehdr_in->pe.e_ovno, (bfd_byte *) filehdr_out->e_ovno);
  {
    int idx;
    for (idx=0; idx < 4; idx++)
      bfd_h_put_16(abfd, filehdr_in->pe.e_res[idx], 
		   (bfd_byte *) filehdr_out->e_res[idx]);
  }
  bfd_h_put_16(abfd, filehdr_in->pe.e_oemid, (bfd_byte *) filehdr_out->e_oemid);
  bfd_h_put_16(abfd, filehdr_in->pe.e_oeminfo,
	       (bfd_byte *) filehdr_out->e_oeminfo);
  {
    int idx;
    for (idx=0; idx < 10; idx++)
      bfd_h_put_16(abfd, filehdr_in->pe.e_res2[idx],
		   (bfd_byte *) filehdr_out->e_res2[idx]);
  }
  bfd_h_put_32(abfd, filehdr_in->pe.e_lfanew, (bfd_byte *) filehdr_out->e_lfanew);

  {
    int idx;
    for (idx=0; idx < 16; idx++)
      bfd_h_put_32(abfd, filehdr_in->pe.dos_message[idx],
		   (bfd_byte *) filehdr_out->dos_message[idx]);
  }

  /* also put in the NT signature */
  bfd_h_put_32(abfd, filehdr_in->pe.nt_signature, 
	       (bfd_byte *) filehdr_out->nt_signature);




  return sizeof(FILHDR);
}



static void
coff_swap_sym_in (abfd, ext1, in1)
     bfd            *abfd;
     PTR ext1;
     PTR in1;
{
  SYMENT *ext = (SYMENT *)ext1;
  struct internal_syment      *in = (struct internal_syment *)in1;

  if( ext->e.e_name[0] == 0) {
    in->_n._n_n._n_zeroes = 0;
    in->_n._n_n._n_offset = bfd_h_get_32(abfd, (bfd_byte *) ext->e.e.e_offset);
  }
  else {
#if SYMNMLEN != E_SYMNMLEN
    -> Error, we need to cope with truncating or extending SYMNMLEN!;
#else
    memcpy(in->_n._n_name, ext->e.e_name, SYMNMLEN);
#endif
  }
  in->n_value = bfd_h_get_32(abfd, (bfd_byte *) ext->e_value); 
  in->n_scnum = bfd_h_get_16(abfd, (bfd_byte *) ext->e_scnum);
  if (sizeof(ext->e_type) == 2){
    in->n_type = bfd_h_get_16(abfd, (bfd_byte *) ext->e_type);
  }
  else {
    in->n_type = bfd_h_get_32(abfd, (bfd_byte *) ext->e_type);
  }
  in->n_sclass = bfd_h_get_8(abfd, ext->e_sclass);
  in->n_numaux = bfd_h_get_8(abfd, ext->e_numaux);

  /* The section symbols for the .idata$ sections have class 68, which MS
     documentation indicates is a section symbol.  The problem is that the
     value field in the symbol is simply a copy of the .idata section's flags
     rather than something useful.  When these symbols are encountered, change
     the value to 0 and the section number to 1 so that they will be handled
     somewhat correctly in the bfd code. */
  if (in->n_sclass == 0x68) {
    in->n_value = 0x0;
    in->n_scnum = 1;
    /* I have tried setting the class to 3 and using the following to set
       the section number.  This will put the address of the pointer to the
       string kernel32.dll at addresses 0 and 0x10 off start of idata section
       which is not correct */
    /*    if (strcmp (in->_n._n_name, ".idata$4") == 0) */
    /*      in->n_scnum = 3; */
    /*    else */
    /*      in->n_scnum = 2; */
  }
}

static unsigned int
coff_swap_sym_out (abfd, inp, extp)
     bfd       *abfd;
     PTR	inp;
     PTR	extp;
{
  struct internal_syment *in = (struct internal_syment *)inp;
  SYMENT *ext =(SYMENT *)extp;
  if(in->_n._n_name[0] == 0) {
    bfd_h_put_32(abfd, 0, (bfd_byte *) ext->e.e.e_zeroes);
    bfd_h_put_32(abfd, in->_n._n_n._n_offset, (bfd_byte *)  ext->e.e.e_offset);
  }
  else {
#if SYMNMLEN != E_SYMNMLEN
    -> Error, we need to cope with truncating or extending SYMNMLEN!;
#else
    memcpy(ext->e.e_name, in->_n._n_name, SYMNMLEN);
#endif
  }
  bfd_h_put_32(abfd,  in->n_value , (bfd_byte *) ext->e_value);
  bfd_h_put_16(abfd,  in->n_scnum , (bfd_byte *) ext->e_scnum);
  if (sizeof(ext->e_type) == 2)
    {
      bfd_h_put_16(abfd,  in->n_type , (bfd_byte *) ext->e_type);
    }
  else
    {
      bfd_h_put_32(abfd,  in->n_type , (bfd_byte *) ext->e_type);
    }
  bfd_h_put_8(abfd,  in->n_sclass , ext->e_sclass);
  bfd_h_put_8(abfd,  in->n_numaux , ext->e_numaux);
  return sizeof(SYMENT);
}

static void
coff_swap_aux_in (abfd, ext1, type, class, indx, numaux, in1)
     bfd            *abfd;
     PTR 	      ext1;
     int             type;
     int             class;
     int	      indx;
     int	      numaux;
     PTR 	      in1;
{
  AUXENT    *ext = (AUXENT *)ext1;
  union internal_auxent *in = (union internal_auxent *)in1;

  switch (class) {
  case C_FILE:
    if (ext->x_file.x_fname[0] == 0) {
      in->x_file.x_n.x_zeroes = 0;
      in->x_file.x_n.x_offset = 
	bfd_h_get_32(abfd, (bfd_byte *) ext->x_file.x_n.x_offset);
    } else {
#if FILNMLEN != E_FILNMLEN
      -> Error, we need to cope with truncating or extending FILNMLEN!;
#else
      memcpy (in->x_file.x_fname, ext->x_file.x_fname, FILNMLEN);
#endif
    }
    return;


  case C_STAT:
#ifdef C_LEAFSTAT
  case C_LEAFSTAT:
#endif
  case C_HIDDEN:
    if (type == T_NULL) {
      in->x_scn.x_scnlen = GET_SCN_SCNLEN(abfd, ext);
      in->x_scn.x_nreloc = GET_SCN_NRELOC(abfd, ext);
      in->x_scn.x_nlinno = GET_SCN_NLINNO(abfd, ext);
      return;
    }
    break;
  }

  in->x_sym.x_tagndx.l = bfd_h_get_32(abfd, (bfd_byte *) ext->x_sym.x_tagndx);
#ifndef NO_TVNDX
  in->x_sym.x_tvndx = bfd_h_get_16(abfd, (bfd_byte *) ext->x_sym.x_tvndx);
#endif

  if (class == C_BLOCK || ISFCN (type) || ISTAG (class))
    {
      in->x_sym.x_fcnary.x_fcn.x_lnnoptr = GET_FCN_LNNOPTR (abfd, ext);
      in->x_sym.x_fcnary.x_fcn.x_endndx.l = GET_FCN_ENDNDX (abfd, ext);
    }
  else
    {
#if DIMNUM != E_DIMNUM
 #error we need to cope with truncating or extending DIMNUM
#endif
      in->x_sym.x_fcnary.x_ary.x_dimen[0] =
	bfd_h_get_16 (abfd, (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[0]);
      in->x_sym.x_fcnary.x_ary.x_dimen[1] =
	bfd_h_get_16 (abfd, (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[1]);
      in->x_sym.x_fcnary.x_ary.x_dimen[2] =
	bfd_h_get_16 (abfd, (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[2]);
      in->x_sym.x_fcnary.x_ary.x_dimen[3] =
	bfd_h_get_16 (abfd, (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[3]);
    }

  if (ISFCN(type)) {
    in->x_sym.x_misc.x_fsize = bfd_h_get_32(abfd, (bfd_byte *) ext->x_sym.x_misc.x_fsize);
  }
  else {
    in->x_sym.x_misc.x_lnsz.x_lnno = GET_LNSZ_LNNO(abfd, ext);
    in->x_sym.x_misc.x_lnsz.x_size = GET_LNSZ_SIZE(abfd, ext);
  }
}

static unsigned int
coff_swap_aux_out (abfd, inp, type, class, indx, numaux, extp)
     bfd   *abfd;
     PTR 	inp;
     int   type;
     int   class;
     int   indx;
     int   numaux;
     PTR	extp;
{
  union internal_auxent *in = (union internal_auxent *)inp;
  AUXENT *ext = (AUXENT *)extp;

  memset((PTR)ext, 0, AUXESZ);
  switch (class) {
  case C_FILE:
    if (in->x_file.x_fname[0] == 0) {
      bfd_h_put_32(abfd, 0, (bfd_byte *) ext->x_file.x_n.x_zeroes);
      bfd_h_put_32(abfd,
	      in->x_file.x_n.x_offset,
	      (bfd_byte *) ext->x_file.x_n.x_offset);
    }
    else {
#if FILNMLEN != E_FILNMLEN
      -> Error, we need to cope with truncating or extending FILNMLEN!;
#else
      memcpy (ext->x_file.x_fname, in->x_file.x_fname, FILNMLEN);
#endif
    }
    return sizeof (AUXENT);


  case C_STAT:
#ifdef C_LEAFSTAT
  case C_LEAFSTAT:
#endif
  case C_HIDDEN:
    if (type == T_NULL) {
      PUT_SCN_SCNLEN(abfd, in->x_scn.x_scnlen, ext);
      PUT_SCN_NRELOC(abfd, in->x_scn.x_nreloc, ext);
      PUT_SCN_NLINNO(abfd, in->x_scn.x_nlinno, ext);
      return sizeof (AUXENT);
    }
    break;
  }

  bfd_h_put_32(abfd, in->x_sym.x_tagndx.l, (bfd_byte *) ext->x_sym.x_tagndx);
#ifndef NO_TVNDX
  bfd_h_put_16(abfd, in->x_sym.x_tvndx , (bfd_byte *) ext->x_sym.x_tvndx);
#endif

  if (class == C_BLOCK || ISFCN (type) || ISTAG (class))
    {
      PUT_FCN_LNNOPTR(abfd,  in->x_sym.x_fcnary.x_fcn.x_lnnoptr, ext);
      PUT_FCN_ENDNDX(abfd,  in->x_sym.x_fcnary.x_fcn.x_endndx.l, ext);
    }
  else
    {
#if DIMNUM != E_DIMNUM
 #error we need to cope with truncating or extending DIMNUM
#endif
      bfd_h_put_16 (abfd, in->x_sym.x_fcnary.x_ary.x_dimen[0],
		    (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[0]);
      bfd_h_put_16 (abfd, in->x_sym.x_fcnary.x_ary.x_dimen[1],
		    (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[1]);
      bfd_h_put_16 (abfd, in->x_sym.x_fcnary.x_ary.x_dimen[2],
		    (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[2]);
      bfd_h_put_16 (abfd, in->x_sym.x_fcnary.x_ary.x_dimen[3],
		    (bfd_byte *) ext->x_sym.x_fcnary.x_ary.x_dimen[3]);
    }

  if (ISFCN (type))
    bfd_h_put_32 (abfd, in->x_sym.x_misc.x_fsize,
	     (bfd_byte *)  ext->x_sym.x_misc.x_fsize);
  else
    {
      PUT_LNSZ_LNNO (abfd, in->x_sym.x_misc.x_lnsz.x_lnno, ext);
      PUT_LNSZ_SIZE (abfd, in->x_sym.x_misc.x_lnsz.x_size, ext);
    }

  return sizeof(AUXENT);
}


static void
coff_swap_lineno_in (abfd, ext1, in1)
     bfd            *abfd;
     PTR ext1;
     PTR in1;
{
  LINENO *ext = (LINENO *)ext1;
  struct internal_lineno      *in = (struct internal_lineno *)in1;

  in->l_addr.l_symndx = bfd_h_get_32(abfd, (bfd_byte *) ext->l_addr.l_symndx);
  in->l_lnno = GET_LINENO_LNNO(abfd, ext);
}

static unsigned int
coff_swap_lineno_out (abfd, inp, outp)
     bfd       *abfd;
     PTR	inp;
     PTR	outp;
{
  struct internal_lineno *in = (struct internal_lineno *)inp;
  struct external_lineno *ext = (struct external_lineno *)outp;
  bfd_h_put_32(abfd, in->l_addr.l_symndx, (bfd_byte *)
	  ext->l_addr.l_symndx);

  PUT_LINENO_LNNO (abfd, in->l_lnno, ext);
  return sizeof(struct external_lineno);
}



static void
coff_swap_aouthdr_in (abfd, aouthdr_ext1, aouthdr_int1)
     bfd            *abfd;
     PTR aouthdr_ext1;
     PTR aouthdr_int1;
{
  struct internal_extra_pe_aouthdr *a;
  PEAOUTHDR *src = (PEAOUTHDR *)(aouthdr_ext1);
  AOUTHDR        *aouthdr_ext = (AOUTHDR *) aouthdr_ext1;
  struct internal_aouthdr *aouthdr_int = (struct internal_aouthdr *)aouthdr_int1;

  aouthdr_int->magic = bfd_h_get_16(abfd, (bfd_byte *) aouthdr_ext->magic);
  aouthdr_int->vstamp = bfd_h_get_16(abfd, (bfd_byte *) aouthdr_ext->vstamp);
  aouthdr_int->tsize =
    GET_AOUTHDR_TSIZE (abfd, (bfd_byte *) aouthdr_ext->tsize);
  aouthdr_int->dsize =
    GET_AOUTHDR_DSIZE (abfd, (bfd_byte *) aouthdr_ext->dsize);
  aouthdr_int->bsize =
    GET_AOUTHDR_BSIZE (abfd, (bfd_byte *) aouthdr_ext->bsize);
  aouthdr_int->entry =
    GET_AOUTHDR_ENTRY (abfd, (bfd_byte *) aouthdr_ext->entry);
  aouthdr_int->text_start =
    GET_AOUTHDR_TEXT_START (abfd, (bfd_byte *) aouthdr_ext->text_start);
  aouthdr_int->data_start =
    GET_AOUTHDR_DATA_START (abfd, (bfd_byte *) aouthdr_ext->data_start);

  a = &aouthdr_int->pe;
  a->ImageBase = bfd_h_get_32 (abfd, src->ImageBase);
  a->SectionAlignment = bfd_h_get_32 (abfd, src->SectionAlignment);
  a->FileAlignment = bfd_h_get_32 (abfd, src->FileAlignment);
  a->MajorOperatingSystemVersion = 
    bfd_h_get_16 (abfd, src->MajorOperatingSystemVersion);
  a->MinorOperatingSystemVersion = 
    bfd_h_get_16 (abfd, src->MinorOperatingSystemVersion);
  a->MajorImageVersion = bfd_h_get_16 (abfd, src->MajorImageVersion);
  a->MinorImageVersion = bfd_h_get_16 (abfd, src->MinorImageVersion);
  a->MajorSubsystemVersion = bfd_h_get_16 (abfd, src->MajorSubsystemVersion);
  a->MinorSubsystemVersion = bfd_h_get_16 (abfd, src->MinorSubsystemVersion);
  a->Reserved1 = bfd_h_get_32 (abfd, src->Reserved1);
  a->SizeOfImage = bfd_h_get_32 (abfd, src->SizeOfImage);
  a->SizeOfHeaders = bfd_h_get_32 (abfd, src->SizeOfHeaders);
  a->CheckSum = bfd_h_get_32 (abfd, src->CheckSum);
  a->Subsystem = bfd_h_get_16 (abfd, src->Subsystem);
  a->DllCharacteristics = bfd_h_get_16 (abfd, src->DllCharacteristics);
  a->SizeOfStackReserve = bfd_h_get_32 (abfd, src->SizeOfStackReserve);
  a->SizeOfStackCommit = bfd_h_get_32 (abfd, src->SizeOfStackCommit);
  a->SizeOfHeapReserve = bfd_h_get_32 (abfd, src->SizeOfHeapReserve);
  a->SizeOfHeapCommit = bfd_h_get_32 (abfd, src->SizeOfHeapCommit);
  a->LoaderFlags = bfd_h_get_32 (abfd, src->LoaderFlags);
  a->NumberOfRvaAndSizes = bfd_h_get_32 (abfd, src->NumberOfRvaAndSizes);

  {
    int idx;
    for (idx=0; idx < 16; idx++)
      {
	a->DataDirectory[idx].VirtualAddress =
	  bfd_h_get_32 (abfd, src->DataDirectory[idx][0]);
	a->DataDirectory[idx].Size =
	  bfd_h_get_32 (abfd, src->DataDirectory[idx][1]);
      }
  }
}


static void add_data_entry (abfd, aout, idx, name, base)
     bfd *abfd;
     struct internal_extra_pe_aouthdr *aout;
     int idx;
     char *name;
     bfd_vma base;
{
  asection *sec = bfd_get_section_by_name (abfd, name);

  /* add import directory information if it exists */
  if (sec != NULL)
    {
      aout->DataDirectory[idx].VirtualAddress = sec->lma - base;
      aout->DataDirectory[idx].Size = sec->_raw_size;
      sec->flags |= SEC_DATA;
    }
}


static unsigned int
coff_swap_aouthdr_out (abfd, in, out)
     bfd       *abfd;
     PTR	in;
     PTR	out;
{
  struct internal_aouthdr *aouthdr_in = (struct internal_aouthdr *)in;
  struct internal_extra_pe_aouthdr *extra = &pe_data (abfd)->pe_opthdr;
  PEAOUTHDR *aouthdr_out = (PEAOUTHDR *)out;

  bfd_vma sa = extra->SectionAlignment;
  bfd_vma fa = extra->FileAlignment;
  bfd_vma ib = extra->ImageBase ;

  if (aouthdr_in->tsize) 
    aouthdr_in->text_start -= ib;
  if (aouthdr_in->dsize) 
    aouthdr_in->data_start -= ib;
  if (aouthdr_in->entry) 
    aouthdr_in->entry -= ib;

#define FA(x)  (((x) + fa -1 ) & (- fa))
#define SA(x)  (((x) + sa -1 ) & (- sa))

  /* We like to have the sizes aligned */

  aouthdr_in->bsize = FA (aouthdr_in->bsize);


  extra->NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;

  /* first null out all data directory entries .. */
  memset (extra->DataDirectory, sizeof (extra->DataDirectory), 0);

  add_data_entry (abfd, extra, 0, ".edata", ib);
  add_data_entry (abfd, extra, 1, ".idata", ib);
  add_data_entry (abfd, extra, 2, ".rsrc" ,ib);
  add_data_entry (abfd, extra, 5, ".reloc", ib);
  {
    asection *sec;
    bfd_vma dsize= 0;
    bfd_vma isize = SA(abfd->sections->filepos);
    bfd_vma tsize= 0;
    for (sec = abfd->sections; sec; sec = sec->next)
      {
	int rounded = FA(sec->_raw_size);
	if (sec->flags & SEC_DATA) 
	  dsize += rounded;
	if (sec->flags & SEC_CODE)
	  tsize += rounded;
	isize += SA(rounded);
      }

    aouthdr_in->dsize = dsize;
    aouthdr_in->tsize = tsize;
    extra->SizeOfImage = isize;
  }

  extra->SizeOfHeaders = abfd->sections->filepos;
  bfd_h_put_16(abfd, aouthdr_in->magic, (bfd_byte *) aouthdr_out->standard.magic);
  bfd_h_put_16(abfd, aouthdr_in->vstamp, (bfd_byte *) aouthdr_out->standard.vstamp);
  PUT_AOUTHDR_TSIZE (abfd, aouthdr_in->tsize, (bfd_byte *) aouthdr_out->standard.tsize);
  PUT_AOUTHDR_DSIZE (abfd, aouthdr_in->dsize, (bfd_byte *) aouthdr_out->standard.dsize);
  PUT_AOUTHDR_BSIZE (abfd, aouthdr_in->bsize, (bfd_byte *) aouthdr_out->standard.bsize);
  PUT_AOUTHDR_ENTRY (abfd, aouthdr_in->entry, (bfd_byte *) aouthdr_out->standard.entry);
  PUT_AOUTHDR_TEXT_START (abfd, aouthdr_in->text_start,
			  (bfd_byte *) aouthdr_out->standard.text_start);
  PUT_AOUTHDR_DATA_START (abfd, aouthdr_in->data_start,
			  (bfd_byte *) aouthdr_out->standard.data_start);


  bfd_h_put_32 (abfd, extra->ImageBase, 
		(bfd_byte *) aouthdr_out->ImageBase);
  bfd_h_put_32 (abfd, extra->SectionAlignment,
		(bfd_byte *) aouthdr_out->SectionAlignment);
  bfd_h_put_32 (abfd, extra->FileAlignment,
		(bfd_byte *) aouthdr_out->FileAlignment);
  bfd_h_put_16 (abfd, extra->MajorOperatingSystemVersion,
		(bfd_byte *) aouthdr_out->MajorOperatingSystemVersion);
  bfd_h_put_16 (abfd, extra->MinorOperatingSystemVersion,
		(bfd_byte *) aouthdr_out->MinorOperatingSystemVersion);
  bfd_h_put_16 (abfd, extra->MajorImageVersion,
		(bfd_byte *) aouthdr_out->MajorImageVersion);
  bfd_h_put_16 (abfd, extra->MinorImageVersion,
		(bfd_byte *) aouthdr_out->MinorImageVersion);
  bfd_h_put_16 (abfd, extra->MajorSubsystemVersion,
		(bfd_byte *) aouthdr_out->MajorSubsystemVersion);
  bfd_h_put_16 (abfd, extra->MinorSubsystemVersion,
		(bfd_byte *) aouthdr_out->MinorSubsystemVersion);
  bfd_h_put_32 (abfd, extra->Reserved1,
		(bfd_byte *) aouthdr_out->Reserved1);
  bfd_h_put_32 (abfd, extra->SizeOfImage,
		(bfd_byte *) aouthdr_out->SizeOfImage);
  bfd_h_put_32 (abfd, extra->SizeOfHeaders,
		(bfd_byte *) aouthdr_out->SizeOfHeaders);
  bfd_h_put_32 (abfd, extra->CheckSum,
		(bfd_byte *) aouthdr_out->CheckSum);
  bfd_h_put_16 (abfd, extra->Subsystem,
		(bfd_byte *) aouthdr_out->Subsystem);
  bfd_h_put_16 (abfd, extra->DllCharacteristics,
		(bfd_byte *) aouthdr_out->DllCharacteristics);
  bfd_h_put_32 (abfd, extra->SizeOfStackReserve,
		(bfd_byte *) aouthdr_out->SizeOfStackReserve);
  bfd_h_put_32 (abfd, extra->SizeOfStackCommit,
		(bfd_byte *) aouthdr_out->SizeOfStackCommit);
  bfd_h_put_32 (abfd, extra->SizeOfHeapReserve,
		(bfd_byte *) aouthdr_out->SizeOfHeapReserve);
  bfd_h_put_32 (abfd, extra->SizeOfHeapCommit,
		(bfd_byte *) aouthdr_out->SizeOfHeapCommit);
  bfd_h_put_32 (abfd, extra->LoaderFlags,
		(bfd_byte *) aouthdr_out->LoaderFlags);
  bfd_h_put_32 (abfd, extra->NumberOfRvaAndSizes,
		(bfd_byte *) aouthdr_out->NumberOfRvaAndSizes);
  {
    int idx;
    for (idx=0; idx < 16; idx++)
      {
	bfd_h_put_32 (abfd, extra->DataDirectory[idx].VirtualAddress,
		      (bfd_byte *) aouthdr_out->DataDirectory[idx][0]);
	bfd_h_put_32 (abfd, extra->DataDirectory[idx].Size,
		      (bfd_byte *) aouthdr_out->DataDirectory[idx][1]);
      }
  }


  return sizeof(AOUTHDR);
}

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

  scnhdr_int->s_nreloc = bfd_h_get_16(abfd, (bfd_byte *) scnhdr_ext->s_nreloc);
  scnhdr_int->s_nlnno = bfd_h_get_16(abfd, (bfd_byte *) scnhdr_ext->s_nlnno);

  if (scnhdr_int->s_vaddr != 0) 
    {
      scnhdr_int->s_vaddr += pe_data (abfd)->pe_opthdr.ImageBase;
    }
}

static unsigned int
 coff_swap_scnhdr_out (abfd, in, out)
bfd       *abfd;
PTR	in;
PTR	out;
{
  struct internal_scnhdr *scnhdr_int = (struct internal_scnhdr *)in;
  SCNHDR *scnhdr_ext = (SCNHDR *)out;
  unsigned int ret = sizeof (SCNHDR);

  memcpy(scnhdr_ext->s_name, scnhdr_int->s_name, sizeof(scnhdr_int->s_name));


  PUT_SCNHDR_VADDR (abfd, 
		    (scnhdr_int->s_vaddr 
		     - pe_data(abfd)->pe_opthdr.ImageBase),
		    (bfd_byte *) scnhdr_ext->s_vaddr);

  /* NT wants the physical address data to be the size (s_size data) of
     the section */
  PUT_SCNHDR_PADDR (abfd, scnhdr_int->s_size,
		    (bfd_byte *) scnhdr_ext->s_paddr);
  /* NT wants the size data to be rounded up to the next NT_FILE_ALIGNMENT
     value except for the BSS section, its s_size should be 0 */
  if (strcmp (scnhdr_int->s_name, _BSS) == 0)
    PUT_SCNHDR_SIZE (abfd, 0, (bfd_byte *) scnhdr_ext->s_size);
  else
    {
      bfd_vma rounded_size;
      rounded_size = ((scnhdr_int->s_size + NT_FILE_ALIGNMENT - 1) / 
		      NT_FILE_ALIGNMENT) *
			NT_FILE_ALIGNMENT;
      PUT_SCNHDR_SIZE (abfd, rounded_size, (bfd_byte *) scnhdr_ext->s_size);
    }

  PUT_SCNHDR_SCNPTR (abfd, scnhdr_int->s_scnptr,
		     (bfd_byte *) scnhdr_ext->s_scnptr);
  PUT_SCNHDR_RELPTR (abfd, scnhdr_int->s_relptr,
		     (bfd_byte *) scnhdr_ext->s_relptr);
  PUT_SCNHDR_LNNOPTR (abfd, scnhdr_int->s_lnnoptr,
		      (bfd_byte *) scnhdr_ext->s_lnnoptr);

  /* Extra flags must be set when dealing with NT.  All sections should also
     have the IMAGE_SCN_MEM_READ (0x40000000) flag set.  In addition, the
     .text section must have IMAGE_SCN_MEM_EXECUTE (0x20000000) and the data
     sections (.idata, .data, .bss, .CRT) must have IMAGE_SCN_MEM_WRITE set
     (this is especially important when dealing with the .idata section since
     the addresses for routines from .dlls must be overwritten).  If .reloc
     section data is ever generated, we must add IMAGE_SCN_MEM_DISCARDABLE
     (0x02000000).  Also, the resource data should also be read and
     writable.  */
  {
    int flags = scnhdr_int->s_flags;
    if (strcmp (scnhdr_int->s_name, ".data")  == 0 ||
	strcmp (scnhdr_int->s_name, ".CRT")   == 0 ||
	strcmp (scnhdr_int->s_name, ".rsrc")  == 0 ||
	strcmp (scnhdr_int->s_name, ".bss")   == 0)
      flags |= IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
    else if (strcmp (scnhdr_int->s_name, ".text") == 0)
      flags |= IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;
    else if (strcmp (scnhdr_int->s_name, ".reloc") == 0)
      flags = SEC_DATA| IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_DISCARDABLE;
    else if (strcmp (scnhdr_int->s_name, ".idata") == 0)
      flags = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | SEC_DATA;     
    else if (strcmp (scnhdr_int->s_name, ".rdata") == 0
	     || strcmp (scnhdr_int->s_name, ".edata") == 0)
      flags =  IMAGE_SCN_MEM_READ | SEC_DATA;     

    bfd_h_put_32(abfd, flags, (bfd_byte *) scnhdr_ext->s_flags);
  }

  if (scnhdr_int->s_nlnno <= 0xffff)
    bfd_h_put_16(abfd, scnhdr_int->s_nlnno, (bfd_byte *) scnhdr_ext->s_nlnno);
  else
    {
      (*_bfd_error_handler) ("%s: line number overflow: 0x%lx > 0xffff",
			     bfd_get_filename (abfd),
			     scnhdr_int->s_nlnno);
      bfd_set_error (bfd_error_file_truncated);
      bfd_h_put_16 (abfd, 0xffff, (bfd_byte *) scnhdr_ext->s_nlnno);
      ret = 0;
    }
  if (scnhdr_int->s_nreloc <= 0xffff)
    bfd_h_put_16(abfd, scnhdr_int->s_nreloc, (bfd_byte *) scnhdr_ext->s_nreloc);
  else
    {
      (*_bfd_error_handler) ("%s: reloc overflow: 0x%lx > 0xffff",
			     bfd_get_filename (abfd),
			     scnhdr_int->s_nreloc);
      bfd_set_error (bfd_error_file_truncated);
      bfd_h_put_16 (abfd, 0xffff, (bfd_byte *) scnhdr_ext->s_nreloc);
      ret = 0;
    }
  return ret;
}
/**********************************************************************/
static boolean
pe_print_private_bfd_data (abfd, vfile)
     bfd*abfd;
     void *vfile;
{
  FILE *file = vfile;
  int j;
  pe_data_type *pe = pe_data (abfd);
  struct internal_extra_pe_aouthdr *i = &pe->pe_opthdr;
  fprintf (file,"ImageBase\t\t%08x\n", i->ImageBase);
  fprintf (file,"SectionAlignment\t%08x\n", i->SectionAlignment);
  fprintf (file,"FileAlignment\t\t%08x\n", i->FileAlignment);
  fprintf (file,"MajorOSystemVersion\t%d\n", i->MajorOperatingSystemVersion);
  fprintf (file,"MinorOSystemVersion\t%d\n", i->MinorOperatingSystemVersion);
  fprintf (file,"MajorImageVersion\t%d\n", i->MajorImageVersion);
  fprintf (file,"MinorImageVersion\t%d\n", i->MinorImageVersion);
  fprintf (file,"MajorSubsystemVersion\t%d\n", i->MajorSubsystemVersion);
  fprintf (file,"MinorSubsystemVersion\t%d\n", i->MinorSubsystemVersion);
  fprintf (file,"Reserved1\t\t%08x\n", i->Reserved1);
  fprintf (file,"SizeOfImage\t\t%08x\n", i->SizeOfImage);
  fprintf (file,"SizeOfHeaders\t\t%08x\n", i->SizeOfHeaders);
  fprintf (file,"CheckSum\t\t%08x\n", i->CheckSum);
  fprintf (file,"Subsystem\t\t%08x\n", i->Subsystem);
  fprintf (file,"DllCharacteristics\t%08x\n", i->DllCharacteristics);
  fprintf (file,"SizeOfStackReserve\t%08x\n", i->SizeOfStackReserve);
  fprintf (file,"SizeOfStackCommit\t%08x\n", i->SizeOfStackCommit);
  fprintf (file,"SizeOfHeapReserve\t%08x\n", i->SizeOfHeapReserve);
  fprintf (file,"SizeOfHeapCommit\t%08x\n", i->SizeOfHeapCommit);
  fprintf (file,"LoaderFlags\t\t%08x\n", i->LoaderFlags);
  fprintf (file,"NumberOfRvaAndSizes\t%08x\n", i->NumberOfRvaAndSizes);

  for (j = 0; j < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; j++) 
    {
      fprintf(file, "Entry %2d %08x %08x\n", 
	      j, 
	      i->DataDirectory[j].VirtualAddress,
	      i->DataDirectory[j].Size);
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
    {
      bfd_set_error (bfd_error_no_memory);
      return false;
    }
  pe =pe_data (abfd);
  pe->coff.pe = 1;
  return true;
}

/* Create the COFF backend specific information.  */
static PTR
pe_mkobject_hook (abfd, filehdr, aouthdr)
     bfd * abfd;
     PTR filehdr;
     PTR aouthdr;
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

  obj_raw_syment_count (abfd) =
    obj_conv_table_size (abfd) =
      internal_f->f_nsyms;

  pe->pe_opthdr = ((struct internal_aouthdr *)aouthdr)->pe;
  return (PTR) pe;
}




