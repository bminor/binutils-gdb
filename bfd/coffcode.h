/* Support for Intel 960 COFF and Motorola 88k BCS COFF (and maybe others) */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software
   Foundation; either version 1, or (at your option) any later version.

BFD is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
   details.

You should have received a copy of the GNU General Public License along with
   BFD; see the file COPYING.  If not, write to the Free Software Foundation,
   675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* $Id$ */
/* Most of this hacked by Steve Chamberlain, steve@cygnus.com */

#include "archures.h"		/* Machine architectures and types */

/* SUPPRESS 558 */
/* SUPPRESS 590 */
/* SUPPRESS 529 */
/* SUPPRESS 530 */

/* Align an address upward to a boundary, expressed as a number of bytes.
   E.g. align to an 8-byte boundary with argument of 8.  */
#define ALIGN(this, boundary) \
  ((( (this) + ((boundary) -1)) & (~((boundary)-1))))

/* Align an address upward to a power of two.  Argument is the power
   of two, e.g. 8-byte alignment uses argument of 3 (8 == 2^3).  */
#define	i960_align(addr, align)	\
	( ((addr) + ((1<<(align))-1)) & (-1 << (align)))

#define sp(x) bfd_h_put_x(abfd, x, &x)

#define PUTWORD bfd_h_put_32
#define PUTHALF bfd_h_put_16
#ifndef I960
#define GDB_EXPORT static
#else
#define GDB_EXPORT  /* nothing */
#endif

PROTO(static void,force_indices_file_symbol_relative,(bfd *abfd,
						      struct internal_syment *symtab));


/* void warning(); */
extern asection abs_section;

static int
DEFUN(get_index,(symbol),
      asymbol        *symbol)
{
    return (int) symbol->value;
}

static void
DEFUN(set_index,(symbol, idx),
      asymbol        *symbol AND
      unsigned int    idx)
{
    symbol->value = idx;
}





/* All the swapping routines:
*/


GDB_EXPORT
void
DEFUN(bfd_swap_reloc_in,(abfd, reloc_src, reloc_dst),
      bfd            *abfd AND
      RELOC *reloc_src AND
      struct internal_reloc *reloc_dst)
{
  reloc_dst->r_vaddr = bfd_h_get_32(abfd, reloc_src->r_vaddr);
  reloc_dst->r_symndx = bfd_h_get_32(abfd, reloc_src->r_symndx);
  reloc_dst->r_type = bfd_h_get_16(abfd, reloc_src->r_type);
#if M88
  reloc_dst->r_offset = bfd_h_get_16(abfd, reloc_src->r_offset);
#endif
}

GDB_EXPORT
 void
DEFUN(bfd_swap_reloc_out,(abfd, reloc_src, reloc_dst),
      bfd            *abfd AND
      struct internal_reloc *reloc_src AND
      struct external_reloc *reloc_dst)
{
  bfd_h_put_32(abfd, reloc_src->r_vaddr, reloc_dst->r_vaddr);
  bfd_h_put_32(abfd, reloc_src->r_symndx, reloc_dst->r_symndx);
  bfd_h_put_16(abfd, reloc_src->r_type, reloc_dst->r_type);
#if M88
  bfd_h_put_16(abfd, reloc_src->r_offset, reloc_dst->r_offset);
#endif

}

GDB_EXPORT  void 
DEFUN(bfd_swap_filehdr_in,(abfd, filehdr_src, filehdr_dst),
      bfd            *abfd AND
      FILHDR         *filehdr_src AND
      struct internal_filehdr *filehdr_dst)
{
  filehdr_dst->f_magic = bfd_h_get_16(abfd, filehdr_src->f_magic);
  filehdr_dst->f_nscns = bfd_h_get_16(abfd,filehdr_src-> f_nscns);
  filehdr_dst->f_timdat = bfd_h_get_32(abfd,filehdr_src-> f_timdat);
  filehdr_dst->f_symptr = bfd_h_get_32(abfd,filehdr_src-> f_symptr);
  filehdr_dst->f_nsyms = bfd_h_get_32(abfd,filehdr_src-> f_nsyms);
  filehdr_dst->f_opthdr = bfd_h_get_16(abfd,filehdr_src-> f_opthdr);
  filehdr_dst->f_flags = bfd_h_get_16(abfd,filehdr_src-> f_flags);
}

GDB_EXPORT  void 
DEFUN(bfd_swap_filehdr_out,(abfd, filehdr_in, filehdr_out),
      bfd            *abfd AND
      struct internal_filehdr *filehdr_in AND
      FILHDR         *filehdr_out)
{
  bfd_h_put_16(abfd, filehdr_in->f_magic, filehdr_out->f_magic);
  bfd_h_put_16(abfd, filehdr_in->f_nscns, filehdr_out->f_nscns);
  bfd_h_put_32(abfd, filehdr_in->f_timdat, filehdr_out->f_timdat);
  bfd_h_put_32(abfd, filehdr_in->f_symptr, filehdr_out->f_symptr);
  bfd_h_put_32(abfd, filehdr_in->f_nsyms, filehdr_out->f_nsyms);
  bfd_h_put_16(abfd, filehdr_in->f_opthdr, filehdr_out->f_opthdr);
  bfd_h_put_16(abfd, filehdr_in->f_flags, filehdr_out->f_flags);
}


GDB_EXPORT 
void 
DEFUN(bfd_coff_swap_sym_in,(abfd, ext, in),
      bfd            *abfd AND
      SYMENT *ext AND
      struct internal_syment      *in)
{
  if( ext->e.e_name[0] == 0) {
    in->_n._n_n._n_zeroes = 0;
    in->_n._n_n._n_offset = bfd_h_get_32(abfd, ext->e.e.e_offset);
  }
  else {
    memcpy(in->_n._n_name, ext->e.e_name, SYMNMLEN);
  }
  in->n_value = bfd_h_get_32(abfd, ext->e_value);
  in->n_scnum = bfd_h_get_16(abfd, ext->e_scnum);
  if (sizeof(ext->e_type) == 2){
    in->n_type = bfd_h_get_16(abfd, ext->e_type);
  }
  else {
    in->n_type = bfd_h_get_32(abfd, ext->e_type);
  }
  in->n_sclass = bfd_h_get_8(abfd, ext->e_sclass);
  in->n_numaux = bfd_h_get_8(abfd, ext->e_numaux);
}

GDB_EXPORT void 
DEFUN(bfd_coff_swap_sym_out,(abfd,in,  ext),
      bfd            *abfd AND
      struct internal_syment      *in AND
      SYMENT *ext)
{
  if(in->_n._n_name[0] == 0) {
    bfd_h_put_32(abfd, 0, ext->e.e.e_zeroes);
    bfd_h_put_32(abfd, in->_n._n_n._n_offset,  ext->e.e.e_offset);
  }
  else {
    memcpy(ext->e.e_name, in->_n._n_name, SYMNMLEN);
  }
  bfd_h_put_32(abfd,  in->n_value , ext->e_value);
  bfd_h_put_16(abfd,  in->n_scnum , ext->e_scnum);
  if (sizeof(ext->e_type) == 2) 
      {
	bfd_h_put_16(abfd,  in->n_type , ext->e_type);
      }
  else
      {
	bfd_h_put_32(abfd,  in->n_type , ext->e_type);
      }
  bfd_h_put_8(abfd,  in->n_sclass , ext->e_sclass);
  bfd_h_put_8(abfd,  in->n_numaux , ext->e_numaux);
}

GDB_EXPORT void
DEFUN(bfd_coff_swap_aux_in,(abfd, ext, type, class, in),
      bfd            *abfd AND
      AUXENT    *ext AND
      int             type AND
      int             class AND
      union internal_auxent  *in)
{
  switch (class) {
  case C_FILE:
    if (ext->x_file.x_fname[0] == 0) {
      in->x_file.x_n.x_zeroes = 0;
      in->x_file.x_n.x_offset  = bfd_h_get_32(abfd, ext->x_file.x_n.x_offset);
    }

    break;
  case C_STAT:
#ifdef C_LEAFSTAT
  case C_LEAFSTAT:
#endif
  case C_HIDDEN:
    if (type == T_NULL) {
      in->x_scn.x_scnlen = bfd_h_get_32(abfd, ext->x_scn.x_scnlen);
      in->x_scn.x_nreloc = bfd_h_get_16(abfd, ext->x_scn.x_nreloc);
      in->x_scn.x_nlinno = bfd_h_get_16(abfd, ext->x_scn.x_nlinno);
      break;
    }
  default:
    in->x_sym.x_tagndx = bfd_h_get_32(abfd, ext->x_sym.x_tagndx);
    in->x_sym.x_tvndx = bfd_h_get_16(abfd, ext->x_sym.x_tvndx);

    if (ISARY(type) || class == C_BLOCK) {
      in->x_sym.x_fcnary.x_ary.x_dimen[0] = bfd_h_get_16(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[0]);
      in->x_sym.x_fcnary.x_ary.x_dimen[1] = bfd_h_get_16(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[1]);
      in->x_sym.x_fcnary.x_ary.x_dimen[2] = bfd_h_get_16(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[2]);
      in->x_sym.x_fcnary.x_ary.x_dimen[3] = bfd_h_get_16(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[3]);
    }
    else {
      in->x_sym.x_fcnary.x_fcn.x_lnnoptr = bfd_h_get_32(abfd, ext->x_sym.x_fcnary.x_fcn.x_lnnoptr);
      in->x_sym.x_fcnary.x_fcn.x_endndx = bfd_h_get_32(abfd, ext->x_sym.x_fcnary.x_fcn.x_endndx);
    }
    if (ISFCN(type)) {
      in->x_sym.x_misc.x_fsize = bfd_h_get_32(abfd, ext->x_sym.x_misc.x_fsize);
    }
    else {
      in->x_sym.x_misc.x_lnsz.x_lnno = bfd_h_get_16(abfd, ext->x_sym.x_misc.x_lnsz.x_lnno);
      in->x_sym.x_misc.x_lnsz.x_size = bfd_h_get_16(abfd, ext->x_sym.x_misc.x_lnsz.x_size);
    }
  }
}

GDB_EXPORT void
DEFUN(bfd_coff_swap_aux_out,(abfd, in, type, class, ext),
  bfd   *abfd AND
  union internal_auxent *in AND
  int    type AND
  int    class AND
  AUXENT *ext)
{
  switch (class) {
  case C_FILE:
    if (in->x_file.x_fname[0] == 0) {
      PUTWORD(abfd, 0, ext->x_file.x_n.x_zeroes );
      PUTWORD(abfd, in->x_file.x_n.x_offset, ext->x_file.x_n.x_offset);
    }

    break;
  case C_STAT:
#ifdef C_LEAFSTAT
  case C_LEAFSTAT:
#endif
  case C_HIDDEN:
    if (type == T_NULL) {
      PUTWORD(abfd, in->x_scn.x_scnlen, ext->x_scn.x_scnlen);
      PUTWORD(abfd, in->x_scn.x_nreloc, ext->x_scn.x_nreloc);
      PUTWORD(abfd, in->x_scn.x_nlinno, ext->x_scn.x_nlinno);
      break;
    }
  default:
    PUTWORD(abfd, in->x_sym.x_tagndx, ext->x_sym.x_tagndx);
    PUTWORD(abfd, in->x_sym.x_tvndx , ext->x_sym.x_tvndx);

    if (ISARY(type) || class == C_BLOCK) {
      bfd_h_put_16(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[0],ext->x_sym.x_fcnary.x_ary.x_dimen[0]);
      bfd_h_put_16(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[1],ext->x_sym.x_fcnary.x_ary.x_dimen[1]);
      bfd_h_put_16(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[2],ext->x_sym.x_fcnary.x_ary.x_dimen[2]);
      bfd_h_put_16(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[3],ext->x_sym.x_fcnary.x_ary.x_dimen[3]);
    }
    else {
      PUTWORD(abfd, in->x_sym.x_fcnary.x_fcn.x_lnnoptr, ext->x_sym.x_fcnary.x_fcn.x_lnnoptr);
      PUTWORD(abfd, in->x_sym.x_fcnary.x_fcn.x_endndx, ext->x_sym.x_fcnary.x_fcn.x_endndx);
    }
    if (ISFCN(type)) {
      PUTWORD(abfd, in->x_sym.x_misc.x_fsize, ext->x_sym.x_misc.x_fsize);
    }
    else {
      bfd_h_put_16(abfd, in->x_sym.x_misc.x_lnsz.x_lnno, ext->x_sym.x_misc.x_lnsz.x_lnno);
      bfd_h_put_16(abfd, in->x_sym.x_misc.x_lnsz.x_size, ext->x_sym.x_misc.x_lnsz.x_size);
    }
  }
}

GDB_EXPORT void
DEFUN(bfd_coff_swap_lineno_in,(abfd, ext, in),
      bfd            *abfd AND
      LINENO *ext AND
      struct internal_lineno      *in)
{
    in->l_addr.l_symndx = bfd_h_get_32(abfd, ext->l_addr.l_symndx);
    in->l_lnno = bfd_h_get_16(abfd, ext->l_lnno);
}

GDB_EXPORT void
DEFUN(bfd_coff_swap_lineno_out,(abfd, in, ext),
      bfd            *abfd AND
      struct internal_lineno      *in AND
      struct external_lineno *ext)
{
  PUTWORD(abfd, in->l_addr.l_symndx, ext->l_addr.l_symndx);
  PUTHALF(abfd, in->l_lnno, ext->l_lnno);
}




GDB_EXPORT void 
DEFUN(bfd_swap_aouthdr_in,(abfd, aouthdr_ext, aouthdr_int),
      bfd            *abfd AND
      AOUTHDR        *aouthdr_ext AND
      struct internal_aouthdr *aouthdr_int)
{
  aouthdr_int->magic = bfd_h_get_16(abfd, aouthdr_ext->magic);
  aouthdr_int->vstamp = bfd_h_get_16(abfd, aouthdr_ext->vstamp);
  aouthdr_int->tsize = bfd_h_get_32(abfd, aouthdr_ext->tsize);
  aouthdr_int->dsize = bfd_h_get_32(abfd, aouthdr_ext->dsize);
  aouthdr_int->bsize = bfd_h_get_32(abfd, aouthdr_ext->bsize);
  aouthdr_int->entry = bfd_h_get_32(abfd, aouthdr_ext->entry);
  aouthdr_int->text_start = bfd_h_get_32(abfd, aouthdr_ext->text_start);
  aouthdr_int->data_start = bfd_h_get_32(abfd, aouthdr_ext->data_start);
#ifdef I960
  aouthdr_int->tagentries = bfd_h_get_32(abfd, aouthdr_ext->tagentries);
#endif
}

GDB_EXPORT void 
DEFUN(bfd_swap_aouthdr_out,(abfd, aouthdr_in, aouthdr_out),
      bfd            *abfd AND
      struct internal_aouthdr *aouthdr_in AND
      AOUTHDR        *aouthdr_out)
{
  bfd_h_put_16(abfd, aouthdr_in->magic, aouthdr_out->magic);
  bfd_h_put_16(abfd, aouthdr_in->vstamp, aouthdr_out->vstamp);
  bfd_h_put_32(abfd, aouthdr_in->tsize, aouthdr_out->tsize);
  bfd_h_put_32(abfd, aouthdr_in->dsize, aouthdr_out->dsize);
  bfd_h_put_32(abfd, aouthdr_in->bsize, aouthdr_out->bsize);
  bfd_h_put_32(abfd, aouthdr_in->entry, aouthdr_out->entry);
  bfd_h_put_32(abfd, aouthdr_in->text_start, aouthdr_out->text_start);
  bfd_h_put_32(abfd, aouthdr_in->data_start, aouthdr_out->data_start);
#ifdef I960
  bfd_h_put_32(abfd, aouthdr_in->tagentries, aouthdr_out->tagentries);
#endif
}

GDB_EXPORT void 
DEFUN(bfd_coff_swap_scnhdr_in,(abfd, scnhdr_ext, scnhdr_int),
      bfd            *abfd AND
      SCNHDR         *scnhdr_ext AND
      struct internal_scnhdr *scnhdr_int)
{
  memcpy(scnhdr_int->s_name, scnhdr_ext->s_name, sizeof(scnhdr_int->s_name));
  scnhdr_int->s_vaddr = bfd_h_get_32(abfd, scnhdr_ext->s_vaddr);
  scnhdr_int->s_paddr = bfd_h_get_32(abfd, scnhdr_ext->s_paddr);
  scnhdr_int->s_size = bfd_h_get_32(abfd, scnhdr_ext->s_size);
  scnhdr_int->s_scnptr = bfd_h_get_32(abfd, scnhdr_ext->s_scnptr);
  scnhdr_int->s_relptr = bfd_h_get_32(abfd, scnhdr_ext->s_relptr);
  scnhdr_int->s_lnnoptr = bfd_h_get_32(abfd, scnhdr_ext->s_lnnoptr);
  scnhdr_int->s_nreloc = bfd_h_get_16(abfd, scnhdr_ext->s_nreloc);
  scnhdr_int->s_nlnno = bfd_h_get_16(abfd, scnhdr_ext->s_nlnno);
  scnhdr_int->s_flags = bfd_h_get_32(abfd, scnhdr_ext->s_flags);
#ifdef I960
  scnhdr_int->s_align = bfd_h_get_32(abfd, scnhdr_ext->s_align);
#endif
}

static void 
DEFUN(swap_scnhdr_out,(abfd, scnhdr_int, scnhdr_ext),
      bfd            *abfd AND
      struct internal_scnhdr *scnhdr_int AND
      SCNHDR         *scnhdr_ext)
{
  memcpy(scnhdr_ext->s_name, scnhdr_int->s_name, sizeof(scnhdr_int->s_name));
  PUTWORD(abfd, scnhdr_int->s_vaddr, scnhdr_ext->s_vaddr);
  PUTWORD(abfd, scnhdr_int->s_paddr, scnhdr_ext->s_paddr);
  PUTWORD(abfd, scnhdr_int->s_size, scnhdr_ext->s_size);
  PUTWORD(abfd, scnhdr_int->s_scnptr, scnhdr_ext->s_scnptr);
  PUTWORD(abfd, scnhdr_int->s_relptr, scnhdr_ext->s_relptr);
  PUTWORD(abfd, scnhdr_int->s_lnnoptr, scnhdr_ext->s_lnnoptr);
  PUTWORD(abfd, scnhdr_int->s_nreloc, scnhdr_ext->s_nreloc);
  PUTHALF(abfd, scnhdr_int->s_nlnno, scnhdr_ext->s_nlnno);
  PUTHALF(abfd, scnhdr_int->s_flags, scnhdr_ext->s_flags);
#ifdef I960
  PUTWORD(abfd, scnhdr_int->s_align, scnhdr_ext->s_align);
#endif
}

/*
   initialize a section structure with information peculiar to this
   particular implementation of coff
*/

static          boolean
DEFUN(coff_new_section_hook,(abfd_ignore, section_ignore),
      bfd            *abfd_ignore AND
      asection       *section_ignore)
{
#ifdef MC88MAGIC
  /* FIXME, shouldn't this ifdef be on something that says we are
     actually COMPILING FOR an 88K coff file, rather than simply
     knowing its magic number? */
  /* Align to at least 16 bytes */
  section_ignore->alignment_power = 4;
#endif
#if M68
  section_ignore->alignment_power = 3;
#endif
  return true;
}

/* Take a section header read from a coff file (in HOST byte order),
   and make a BFD "section" out of it.  */
static          boolean
DEFUN(make_a_section_from_file,(abfd, hdr),
      bfd            *abfd AND
      struct internal_scnhdr  *hdr)
{
    asection       *return_section;

    {
	/* Assorted wastage to null-terminate the name, thanks AT&T! */
	char *name = bfd_alloc(abfd, sizeof (hdr->s_name)+1);
	if (name == NULL) {
	    bfd_error = no_memory;
	    return false;
	}
	strncpy(name, (char *) &hdr->s_name[0], sizeof (hdr->s_name));
	name[sizeof (hdr->s_name)] = 0;

	return_section = bfd_make_section(abfd, name);
    }

    /* s_paddr is presumed to be = to s_vaddr */
#define assign(to, from) return_section->to = hdr->from
    assign(vma, s_vaddr);
    /* assign (vma, s_vaddr); */
    assign(size, s_size);
    assign(filepos, s_scnptr);
    assign(rel_filepos, s_relptr);
    assign(reloc_count, s_nreloc);
#ifdef I960
    {
	/* FIXME, use a temp var rather than alignment_power */
	assign(alignment_power, s_align);
	{
	    unsigned int    i;
	    for (i = 0; i < 32; i++) {
		if ((1 << i) >= (int) (return_section->alignment_power)) {
		    return_section->alignment_power = i;
		    break;
		}
	    }
	}
    }
#endif
    assign(line_filepos, s_lnnoptr);
    /*
       return_section->linesize =   hdr->s_nlnno * sizeof (struct lineno);
    */

#undef assign
    return_section->lineno_count = hdr->s_nlnno;
    return_section->userdata = NULL;
    return_section->next = (asection *) NULL;
    return_section->flags = 0;
    if ((hdr->s_flags & STYP_TEXT) || (hdr->s_flags & STYP_DATA))
	return_section->flags = (SEC_LOAD | SEC_ALLOC);
    else if (hdr->s_flags & STYP_BSS)
	return_section->flags = SEC_ALLOC;

    if (hdr->s_nreloc != 0)
	return_section->flags |= SEC_RELOC;
    if (hdr->s_scnptr != 0)
	return_section->flags |= SEC_HAS_CONTENTS;
    return true;
}
static          boolean
DEFUN(coff_mkobject,(abfd),
      bfd            *abfd)
{
  set_tdata (abfd, bfd_zalloc (abfd,sizeof(coff_data_type)));
  if (coff_data(abfd) == 0) {
    bfd_error = no_memory;
    return false;
  }
  coff_data(abfd)->relocbase = 0;
  return true;
}

static
bfd_target     *
DEFUN(coff_real_object_p,(abfd, nscns, internal_f, internal_a),
    bfd            *abfd AND
    unsigned        nscns AND
  struct internal_filehdr *internal_f AND
  struct internal_aouthdr *internal_a)
{
  coff_data_type *coff;

  size_t          readsize;	/* length of file_info */
  SCNHDR *external_sections;
  
  /* Build a play area */
  if (coff_mkobject(abfd) != true)
    return 0;
  coff = coff_data(abfd);
  
  
  external_sections = (SCNHDR *)bfd_alloc(abfd, readsize = (nscns * SCNHSZ));
  if (bfd_read((PTR)external_sections, 1, readsize, abfd) != readsize) {
    goto fail;
  }
  
  
  
  /* Now copy data as required; construct all asections etc */
  coff->symbol_index_slew = 0;
  coff->relocbase =0;
  coff->raw_syment_count = 0;
  coff->raw_linenos = 0;
  coff->raw_syments = 0;
  coff->sym_filepos =0;
  coff->flags = internal_f->f_flags;
  if (nscns != 0) {
    unsigned int    i;
    for (i = 0; i < nscns; i++) {
      struct internal_scnhdr tmp;
      bfd_coff_swap_scnhdr_in(abfd, external_sections + i, &tmp);
      make_a_section_from_file(abfd,&tmp);
    }
  }
  /* Determine the machine architecture and type.  */
  abfd->obj_machine = 0;
  switch (internal_f->f_magic) {
#ifdef MIPS
case  MIPS_MAGIC_1:
case  MIPS_MAGIC_2:
case  MIPS_MAGIC_3:
    abfd->obj_arch = bfd_arch_mips;
    abfd->obj_machine = 0;
    break;
#endif

#ifdef MC68MAGIC
  case MC68MAGIC:
  case M68MAGIC:
    abfd->obj_arch = bfd_arch_m68k;
    abfd->obj_machine = 68020;
    break;
#endif
#ifdef MC88MAGIC
  case MC88MAGIC:
  case MC88DMAGIC:
  case MC88OMAGIC:
    abfd->obj_arch = bfd_arch_m88k;
    abfd->obj_machine = 88100;
    break;
#endif
#ifdef I960
#ifdef I960ROMAGIC
  case I960ROMAGIC:
  case I960RWMAGIC:
    abfd->obj_arch = bfd_arch_i960;
    switch (F_I960TYPE & internal_f->f_flags) 
	{
	default:
	case F_I960CORE:
	  abfd->obj_machine = bfd_mach_i960_core;
	  break;
	case F_I960KB:
	  abfd->obj_machine = bfd_mach_i960_kb_sb;
	  break;
	case F_I960MC:
	  abfd->obj_machine = bfd_mach_i960_mc;
	  break;
	case F_I960XA:
	  abfd->obj_machine = bfd_mach_i960_xa;
	  break;
	case F_I960CA:
	  abfd->obj_machine = bfd_mach_i960_ca;
	  break;
	case F_I960KA:
	  abfd->obj_machine = bfd_mach_i960_ka_sa;
	  break;
	  
	}
    break;
#endif
#endif
    
  default:			/* Unreadable input file type */
    abfd->obj_arch = bfd_arch_obscure;
    break;
  }
  
  if (!(internal_f->f_flags & F_RELFLG))
    abfd->flags |= HAS_RELOC;
  if ((internal_f->f_flags & F_EXEC))
    abfd->flags |= EXEC_P;
  if (!(internal_f->f_flags & F_LNNO))
    abfd->flags |= HAS_LINENO;
  if (!(internal_f->f_flags & F_LSYMS))
    abfd->flags |= HAS_LOCALS;
  
  
  bfd_get_symcount(abfd) = internal_f->f_nsyms;
  if (internal_f->f_nsyms)
    abfd->flags |= HAS_SYMS;
  
  coff->sym_filepos = internal_f->f_symptr;
  
  
  
  coff->symbols = (coff_symbol_type *) NULL;
  bfd_get_start_address(abfd) = internal_f->f_opthdr ? internal_a->entry : 0;
  
  return abfd->xvec;
 fail:
  bfd_release(abfd, coff);
  return (bfd_target *)NULL;
}

static bfd_target *
DEFUN(coff_object_p,(abfd),
      bfd            *abfd)
  {
    int   nscns;
    FILHDR filehdr;
    AOUTHDR opthdr;
    struct internal_filehdr internal_f;
    struct internal_aouthdr internal_a;
    
    bfd_error = system_call_error;
    
    /* figure out how much to read */
    if (bfd_read((PTR) &filehdr, 1, FILHSZ, abfd) != FILHSZ)
      return 0;
    
    bfd_swap_filehdr_in(abfd, &filehdr, &internal_f);
    
    if (BADMAG(internal_f)) {
      bfd_error = wrong_format;
      return 0;
    }
    nscns =internal_f.f_nscns;
    
    if (internal_f.f_opthdr) {
      if (bfd_read((PTR) &opthdr, 1,AOUTSZ, abfd) != AOUTSZ) {
	return 0;
      }
      bfd_swap_aouthdr_in(abfd, &opthdr, &internal_a);
    }
    
    /* Seek past the opt hdr stuff */
    bfd_seek(abfd, internal_f.f_opthdr + FILHSZ, SEEK_SET);
    
    /* if the optional header is NULL or not the correct size then
      quit; the only difference I can see between m88k dgux headers (MC88DMAGIC)
	and Intel 960 readwrite headers (I960WRMAGIC) is that the
	  optional header is of a different size.

	But the mips keeps extra stuff in it's opthdr, so dont check
	when doing that
	    */
    
#ifndef MIPS    
    if (internal_f.f_opthdr != 0 && AOUTSZ != internal_f.f_opthdr)
      return (bfd_target *)NULL;
#endif
    
    return coff_real_object_p(abfd, nscns, &internal_f, &internal_a);
  }




/* 
Takes a bfd and a symbol, returns a pointer to the coff specific area
of the symbol if there is one.
*/
static coff_symbol_type *
DEFUN(coff_symbol_from,(abfd, symbol),
      bfd            *abfd AND
      asymbol        *symbol)
{
  if (symbol->the_bfd->xvec->flavour != bfd_target_coff_flavour_enum) 
    return (coff_symbol_type *)NULL;

  if (symbol->the_bfd->tdata == (PTR)NULL)
    return (coff_symbol_type *)NULL;

  return  (coff_symbol_type *) symbol;
}







static void 
DEFUN(coff_count_linenumbers,(abfd),
      bfd            *abfd)
{
    unsigned int    limit = bfd_get_symcount(abfd);
    unsigned int    i;
    asymbol       **p;
    {
	asection       *s = abfd->sections->output_section;
	while (s) {
	    BFD_ASSERT(s->lineno_count == 0);
	    s = s->next;
	}
    }


    for (p = abfd->outsymbols, i = 0; i < limit; i++, p++) {
	asymbol        *q_maybe = *p;
	if (q_maybe->the_bfd->xvec->flavour == bfd_target_coff_flavour_enum) {
	    coff_symbol_type *q = coffsymbol(q_maybe);
	    if (q->lineno) {
		/*
		   This symbol has a linenumber, increment the owning
		   section's linenumber count
		*/
		alent          *l = q->lineno;
		q->symbol.section->output_section->lineno_count++;
		l++;
		while (l->line_number) {
		    q->symbol.section->output_section->lineno_count++;
		    l++;
		}
	    }
	}
    }
}

/*
 This function returns true if the supplied SYMENT has an AUXENT with
 a tagndx field which should be relocated.

 The coff book says that all auxents have this and should be moved,
 but all the actual implementations I've looked at do this ..
 (sac@cygnus.com)

*/
static boolean
DEFUN(uses_x_sym_x_tagndx_p,(abfd, native),
      bfd *abfd AND
      struct internal_syment *native)
{
    if (BTYPE(native->n_type) == T_STRUCT) return true;
    if (BTYPE(native->n_type) == T_UNION)  return true;
    if (BTYPE(native->n_type) == T_ENUM)   return true;
    return false;
}


/* 
This procedure runs through the native entries in a coff symbol table
and links up all the elements which should point to one another, in
particular these are:

strtag, entag and untags have an auxent endindex which points to the
first syment after the .eos. This is simple to do, we just keep a
pointer to the symbol with the most recent pending strtag and patch it
when we see the eos. This works since coff structs are never nested.

ISFCN type entries have an endindex which points to the next static or
extern in the table, thereby skipping the function contents.
The coff book says that an ISFCN's tagindex
points to the first .bf for the function, so far I havn't seen it
used. We do this using the same mechanism as strtags.

Each file entry has a value which points to the next file entry,
the last file entry points to the first extern symbol in the table
which is not an ISFCN.

Each .bb entry points to the matching .eb entry, but these are nested
so we keep a stack of them.

The tagndx of .eos items points to the strtag attached to them, this
is simply the last_tagndx again. 

The tagndx of items with type strtag point to the defining struct.
This bit is complicated; We know that a struct ref and def must be
within the same file, so all the natives will be in the same vector.
This means that we can subtracts two pointers and get the index
differences between to items, used to work out the true index of the
target. 

We store in the name field of each syment the actual native index
applied so we can dig it out through a pointer.  */

static void 
DEFUN(coff_mangle_symbols,(bfd_ptr),
      bfd *bfd_ptr)
{
  unsigned int symbol_count = bfd_get_symcount(bfd_ptr);
  asymbol **symbol_ptr_ptr = bfd_ptr->outsymbols;
  struct internal_syment *last_tagndx = (struct internal_syment *)NULL;
  struct internal_syment *last_file = (struct internal_syment *)NULL;
  struct internal_syment *last_fcn = (struct internal_syment *)NULL;
  struct internal_syment *block_stack[50];
  struct internal_syment **last_block = &block_stack[0];
  boolean first_time = true;  
  unsigned int symbol_index;
  unsigned int native_index = 0;

  for (symbol_index = 0; symbol_index < symbol_count; symbol_index++) {
    coff_symbol_type *coff_symbol_ptr =
      coff_symbol_from(bfd_ptr, symbol_ptr_ptr[symbol_index]);
    if (coff_symbol_ptr == (coff_symbol_type *)NULL) {
      /* 
	This symbol has no coff information in it, it will take up
	only one slot in the output symbol table
	*/
      native_index++;
    }
    else {
      struct internal_syment *syment = coff_symbol_ptr->native;
      if (syment == (struct internal_syment *)NULL) {
	native_index++;
      }
      else {
	/* Normalize the symbol flags */
	if (coff_symbol_ptr->symbol.flags & BSF_FORT_COMM) {
	  /* a common symbol is undefined with a value */
	  syment->n_scnum = N_UNDEF;
	  syment->n_value = coff_symbol_ptr->symbol.value;
	}
	else if (coff_symbol_ptr->symbol.flags & BSF_DEBUGGING) {
	  syment->n_value = coff_symbol_ptr->symbol.value;
	}
	else if (coff_symbol_ptr->symbol.flags & BSF_UNDEFINED) {
	  syment->n_scnum = N_UNDEF;
	  syment->n_value = 0;
	}	  
	else if (coff_symbol_ptr->symbol.flags & BSF_ABSOLUTE) {
	  syment->n_scnum = N_ABS;
	  syment->n_value = coff_symbol_ptr->symbol.value;
	}	  
	else {
	  syment->n_scnum	 = 
	    coff_symbol_ptr->symbol.section->output_section->index+1;
	  
	  syment->n_value = 
	    coff_symbol_ptr->symbol.value +
	      coff_symbol_ptr->symbol.section->output_offset +
		coff_symbol_ptr->symbol.section->output_section->vma;
	}
	
	
	/* If this symbol ties up something then do it */

	if (syment->n_sclass == C_FILE && last_file != (struct internal_syment *)NULL)
	  {
	     last_file->n_value = native_index;
	  }
	else if ((syment->n_sclass == C_EXT 
		  || syment->n_sclass == C_STAT 
#ifdef C_LEAFEXT
		  || syment->n_sclass == C_LEAFEXT 
		  || syment->n_sclass == C_LEAFSTAT
#endif
		  )
		 && last_fcn != (struct internal_syment *)NULL) 
	    {
	      union internal_auxent *auxent = (union internal_auxent *)(last_fcn+1);
	      auxent->x_sym.x_fcnary.x_fcn.x_endndx = native_index;
	      last_fcn = (struct internal_syment *)NULL;
	      
	    }
	else if (syment->n_sclass == C_EOS && last_tagndx != (struct internal_syment*)NULL)
	    {
	      union internal_auxent *auxent = (union internal_auxent *)(last_tagndx+1);
	      /* Remember that we keep the native index in the offset 
		 so patch the beginning of the struct to point to this
		 */
	      auxent->x_sym.x_tagndx =    last_tagndx->_n._n_n._n_offset;
	      auxent->x_sym.x_fcnary.x_fcn.x_endndx = syment->n_numaux + 1 + native_index;
	      /* Now point the eos to the structure */
	      auxent = (union internal_auxent *)(syment+1);
	      auxent->x_sym.x_tagndx =  last_tagndx->_n._n_n._n_offset;
	    }
	else if (syment->n_sclass == C_BLOCK 
		 && coff_symbol_ptr->symbol.name[1] == 'e') 
	    {
	      union internal_auxent *auxent = (union internal_auxent *)((*(--last_block))+1);
	      auxent->x_sym.x_fcnary.x_fcn.x_endndx = native_index + syment->n_numaux + 1;
	    }
	if (syment->n_sclass == C_EXT 
	    && !ISFCN(syment->n_type) 
	    && first_time == true 
	    && last_file != (struct internal_syment *)NULL) {
	  /* This is the first external symbol seen which isn't a 
	     function place it in the last .file entry */
	  last_file->n_value = native_index;
	  first_time = false;
	}
#ifdef C_LEAFPROC
	if (syment->n_sclass == C_LEAFPROC &&
	    syment->n_numaux == 2) {
	  union internal_auxent *auxent = (union internal_auxent *)(syment+2);
	  /* This is the definition of a leaf proc, we'll relocate the 
	     address */
	  auxent->x_bal.x_balntry = 	  
	    coff_symbol_ptr->symbol.section->output_offset + 
	      coff_symbol_ptr->symbol.section->output_section->vma +
		auxent->x_bal.x_balntry   ;
	}
#endif
	/* If this symbol needs to be tied up then remember some facts */
	if (syment->n_sclass == C_FILE) 
	    {
	      last_file = syment;
	    }
	if (syment->n_numaux != 0) {
	  /*
	    If this symbol would like to point to something in the
	    future then remember where it is 
	    */
	  if (uses_x_sym_x_tagndx_p(bfd_ptr, syment)) {
	    /* 
	      If this is a ref to a structure then we'll tie it up 
	      now - there are never any forward refs for one 
	      */
	    if (syment->n_sclass == C_STRTAG ||
		syment->n_sclass == C_ENTAG ||
		syment->n_sclass == C_UNTAG) {
	      last_tagndx = syment;
	    }
	    else {
	      /*
		This is a ref to a structure - the structure must
		have been defined within the same file, and previous
		to this point, so we can deduce the new tagndx
		directly.
		*/
	      union internal_auxent *auxent = (union internal_auxent *)(syment+1);
	      bfd *bfd_ptr = coff_symbol_ptr->symbol.the_bfd;
	      struct internal_syment *base = obj_raw_syments(bfd_ptr);	      
	      auxent->x_sym.x_tagndx = base[auxent->x_sym.x_tagndx]._n._n_n._n_offset;
	      
	      
	    }
	  }
	  if (ISFCN(syment->n_type)) {
	    last_fcn = syment;
	  }
	  if (syment->n_sclass == C_BLOCK 
	      && coff_symbol_ptr->symbol.name[1] == 'b')
	      {
		*last_block++ = syment;
	      }
	}
	syment->_n._n_n._n_offset = native_index;
	native_index = native_index + 1 + syment->n_numaux;
      }
    }
  }
}


static void 
DEFUN(coff_write_symbols,(abfd),
bfd            *abfd)
{
  unsigned int    i;
  unsigned int    limit = bfd_get_symcount(abfd);
  unsigned int    written = 0;
  struct internal_syment          dummy;
  asymbol       **p;
  unsigned int    string_size = 0;
    
    
  /* Seek to the right place */
  bfd_seek(abfd, obj_sym_filepos(abfd), SEEK_SET);
    
  /* Output all the symbols we have */
    
  written = 0;
  for (p = abfd->outsymbols, i = 0; i < limit; i++, p++) {
    asymbol        *symbol = *p;
    coff_symbol_type *c_symbol = coff_symbol_from(abfd, symbol);
      
    unsigned int    j;
    struct internal_syment         *native;
    if (c_symbol == (coff_symbol_type *) NULL ||
	c_symbol->native == (struct internal_syment *) NULL) {
      /*
	This symbol has been created by the loader, or come from a non
	coff format. It  has no native element to inherit, make our
	own
	*/
	
      native = &dummy;
      native->n_type =  T_NULL;
#ifdef I960
      native->n_flags =  0;
#endif
      if (symbol->flags & BSF_ABSOLUTE) {
	native->n_scnum  =  N_ABS;
	native->n_value =  symbol->value;
      }
      else if (symbol->flags & (BSF_UNDEFINED | BSF_FORT_COMM)) {
	native->n_scnum =  N_UNDEF;
	native->n_value =  symbol->value;
      }
      else if (symbol->flags & BSF_DEBUGGING) {
	/*
	  remove name so it doesn't take up any space
	  */
	symbol->name = "";
	continue;
      }
      else {
	native->n_scnum  =   symbol->section->output_section->index +
	  1;
	native->n_value =   symbol->value +
	  symbol->section->output_section->vma +
	    symbol->section->output_offset;
#ifdef I960
	/* Copy the any flags from the the file hdr into the symbol  */
	  {
	    coff_symbol_type *c = coff_symbol_from(abfd, symbol);
	    if (c != (coff_symbol_type *)NULL) {
	      native->n_flags =   c->symbol.the_bfd->flags;
	    }
	  }
#endif
      }
	
#ifdef HASPAD1
      native->pad1[0] = 0;
      native->pad1[0] = 0;
#endif
	
      native->n_type =  0;
      if (symbol->flags & BSF_LOCAL)
	native->n_sclass =  C_STAT;
      else 
	native->n_sclass =  C_EXT;
      native->n_numaux =  0;
    }
    else
      /*
	Does this symbol have an ascociated line number - if so then
	make it remember this symbol index. Also tag the auxent of
	this symbol to point to the right place in the lineno table
	*/
	{
	  alent          *lineno = c_symbol->lineno;
	  native = c_symbol->native;
	  if (lineno) {
	    unsigned int    count = 0;
	    lineno[count].u.offset = written;
	    if (native->n_numaux) {
	      union internal_auxent  *a = (union internal_auxent *) (native + 1);
		
	      a->x_sym.x_fcnary.x_fcn.x_lnnoptr =  
		c_symbol->symbol.section->output_section->moving_line_filepos;
	    }
	    /*
	      And count and relocate all other linenumbers
	      */
	    count++;
	    while (lineno[count].line_number) {
	      lineno[count].u.offset +=
		c_symbol->symbol.section->output_section->vma +
		  c_symbol->symbol.section->output_offset;
	      count++;
	    }
	    c_symbol->symbol.section->output_section->moving_line_filepos +=
	      count * LINESZ;
	      
	  }
	}			/* if symbol new to coff */
      
    /* Fix the symbol names */
      {
	unsigned int    name_length;
	if (symbol->name == (char *) NULL) {
	  /*
	    coff symbols always have names, so we'll make one up
	    */
	  symbol->name = "strange";
	}
	name_length = strlen(symbol->name);
	if (name_length <= SYMNMLEN) {
	  /* This name will fit into the symbol neatly */
	  strncpy(native->_n._n_name, symbol->name, SYMNMLEN);
	}
	else {
	  native->_n._n_n._n_offset =  string_size + 4;
	  native->_n._n_n._n_zeroes = 0;
	  string_size += name_length + 1;
	}
	  {
	    unsigned int    numaux = native->n_numaux;
	    int             type = native->n_type;
	    int             class =  native->n_sclass;
	    SYMENT buf;
	    bfd_coff_swap_sym_out(abfd, native, &buf);
	    bfd_write((PTR)& buf, 1, SYMESZ, abfd);
	    for (j = 0; j != native->n_numaux;
		 j++) {
	      AUXENT buf1;
	      bfd_coff_swap_aux_out(abfd,
				    (union internal_auxent *)(native + j + 1), type, class, &buf1);
	      bfd_write((PTR) (native + j + 1), 1, AUXESZ, abfd);
	    }
	    /*
	      Reuse somewhere in the symbol to keep the index
	      */
	    set_index(symbol, written);
	    written += 1 + numaux;
	  }
      }
  }				/* for each out symbol */
    
  bfd_get_symcount(abfd) = written;
  /* Now write out strings */
    
  if (string_size) {
    unsigned int    size = string_size + 4;
    size =  size;
    bfd_write((PTR) &size, 1, sizeof(size), abfd);
    for (p = abfd->outsymbols, i = 0; i < limit; i++, p++) {
      asymbol        *q = *p;
      size_t          name_length = strlen(q->name);
      if (name_length > SYMNMLEN) {
	bfd_write((PTR) (q->name), 1, name_length + 1, abfd);
      }
    }
  }
  else {
    /* We would normally not write anything here, but we'll write
       out 4 so that any stupid coff reader which tries to read
       the string table even when there isn't one won't croak.
       */
      
    uint32e_type size = 4;
    size =  size;
    bfd_write((PTR)&size, 1, sizeof(size), abfd);
      
  }
    
}

static void 
coff_write_relocs(abfd)
bfd            *abfd;
  {
    asection       *s;
    for (s = abfd->sections; s != (asection *) NULL; s = s->next) {
      unsigned int    i;
      struct external_reloc dst;
      
      arelent       **p = s->orelocation;
      bfd_seek(abfd, s->rel_filepos, SEEK_SET);
      for (i = 0; i < s->reloc_count; i++) {
	struct internal_reloc    n;
	arelent        *q = p[i];
	memset((PTR)&n, 0, sizeof(n));
	n.r_vaddr = q->address + s->vma;
	if (q->sym_ptr_ptr) {
	  n.r_symndx = get_index((*(q->sym_ptr_ptr)));
	}
#ifdef SELECT_RELOC
	/* Work out reloc type from what is required */
	SELECT_RELOC(n.r_type, q->howto);
#else
	n.r_type = q->howto->type;
#endif
	bfd_swap_reloc_out(abfd, &n, &dst);
	bfd_write((PTR) &n, 1, RELSZ, abfd);
      }
    }
  }

static void 
DEFUN(coff_write_linenumbers,(abfd),
      bfd            *abfd)
  {
    asection       *s;
    for (s = abfd->sections; s != (asection *) NULL; s = s->next) {
      if (s->lineno_count) {
	asymbol       **q = abfd->outsymbols;
	bfd_seek(abfd, s->line_filepos, SEEK_SET);
	/* Find all the linenumbers in this section */
	while (*q) {
	  asymbol        *p = *q;
	  alent          *l = BFD_SEND(p->the_bfd, _get_lineno, (p->the_bfd, p));
	  if (l) {
	    /* Found a linenumber entry, output */
	    struct internal_lineno  out;
	    LINENO buff;
	    bzero( (PTR)&out, sizeof(out));
	    out.l_lnno = 0;
	    out.l_addr.l_symndx = l->u.offset;
	    bfd_coff_swap_lineno_out(abfd, &out, &buff);
	    bfd_write((PTR) &buff, 1, LINESZ, abfd);
	    l++;
	    while (l->line_number) {
	      out.l_lnno = l->line_number;
	      out.l_addr.l_symndx = l->u.offset;
	      bfd_coff_swap_lineno_out(abfd, &out, &buff);
	      bfd_write((PTR) &buff, 1, LINESZ, abfd);
	      l++;
	    }
	  }
	  q++;
	}
      }
    }
  }


static asymbol *
coff_make_empty_symbol(abfd)
bfd            *abfd;
  {
    coff_symbol_type *new = (coff_symbol_type *) bfd_alloc(abfd, sizeof(coff_symbol_type));
    if (new == NULL) {
      bfd_error = no_memory;
      return (NULL);
    }			/* on error */
    new->native = 0;
    new->lineno = (alent *) NULL;
    new->symbol.the_bfd = abfd;
    return &new->symbol;
  }

static void 
coff_print_symbol(ignore_abfd, file, symbol, how)
bfd            *ignore_abfd;
FILE           *file;
asymbol        *symbol;
bfd_print_symbol_enum_type how;
  {
    switch (how) {
    case bfd_print_symbol_name_enum:
      fprintf(file, "%s", symbol->name);
      break;
    case bfd_print_symbol_type_enum:
      fprintf(file, "coff %lx %lx", (unsigned long) coffsymbol(symbol)->native,
	      (unsigned long) coffsymbol(symbol)->lineno);
      break;
    case bfd_print_symbol_all_enum:
	{
	  CONST char           *section_name = symbol->section == (asection *) NULL ?
	    "*abs" : symbol->section->name;
	  bfd_print_symbol_vandf((PTR) file, symbol);
	  
	  fprintf(file, " %-5s %s %s %s",
		  section_name,
		  coffsymbol(symbol)->native ? "n" : "g",
		  coffsymbol(symbol)->lineno ? "l" : " ",
		  symbol->name);
	}
      
      
      break;
    }
  }

static alent   *
coff_get_lineno(ignore_abfd, symbol)
bfd            *ignore_abfd;
asymbol        *symbol;
  {
    return coffsymbol(symbol)->lineno;
  }

/*
Set flags and magic number of a coff file from architecture and machine
type.  Result is true if we can represent the arch&type, false if not.
*/
static          boolean
coff_set_flags(abfd, magicp, flagsp)
bfd            *abfd;
unsigned       *magicp,
*flagsp;
  {
    
    switch (abfd->obj_arch) {
      
#ifdef I960ROMAGIC
      
    case bfd_arch_i960:
      
	{
	  unsigned        flags;
	  *magicp = I960ROMAGIC;
	  /*
	    ((bfd_get_file_flags(abfd) & WP_TEXT) ? I960ROMAGIC :
	     I960RWMAGIC);   FIXME???
	       */
	  switch (abfd->obj_machine) {
	  case bfd_mach_i960_core:
	    flags = F_I960CORE;
	    break;
	  case bfd_mach_i960_kb_sb:
	    flags = F_I960KB;
	    break;
	  case bfd_mach_i960_mc:
	    flags = F_I960MC;
	    break;
	  case bfd_mach_i960_xa:
	    flags = F_I960XA;
	    break;
	  case bfd_mach_i960_ca:
	    flags = F_I960CA;
	    break;
	  case bfd_mach_i960_ka_sa:
	    flags = F_I960KA;
	    break;
	  default:
	    return false;
	  }
	  *flagsp = flags;
	  return true;
	}
	break;
#endif
#ifdef MIPS
      case bfd_arch_mips:
	  *magicp = MIPS_MAGIC_2;
	return true;
	break;
#endif
#ifdef MC68MAGIC
      case bfd_arch_m68k:
	*magicp = MC68MAGIC;
	return true;
#endif
	
#ifdef MC88MAGIC
      case bfd_arch_m88k:
	*magicp = MC88OMAGIC;
	return true;
	break;
#endif
	
      default:		/* Unknown architecture */
	return false;
      }
    
    return false;
  }


static          boolean
coff_set_arch_mach(abfd, arch, machine)
bfd            *abfd;
enum bfd_architecture arch;
unsigned long   machine;
  {
    unsigned        dummy1,
    dummy2;
    abfd->obj_arch = arch;
    abfd->obj_machine = machine;
    if (arch != bfd_arch_unknown &&
	coff_set_flags(abfd, &dummy1, &dummy2) != true)
      return false;		/* We can't represent this type */
    return true;	/* We're easy ... */
  }


/* Calculate the file position for each section. */

static void 
coff_compute_section_file_positions(abfd)
bfd            *abfd;
  {
    asection       *current;
    file_ptr        sofar = FILHSZ;
    if (bfd_get_start_address(abfd)) {
      /*
	A start address may have been added to the original file. In this
	case it will need an optional header to record it.
	  */
      abfd->flags |= EXEC_P;
    }
    if (abfd->flags & EXEC_P)
      sofar += AOUTSZ;
    
    
    sofar += abfd->section_count * SCNHSZ;
    
    for (current = abfd->sections; current != NULL; current =
	 current->next) {
      /* Only deal with sections which have contents */
      if (!(current->flags & SEC_HAS_CONTENTS))
	continue;
      
      /* Align the sections in the file to the same boundary on 
	which they are aligned in virtual memory.  I960 doesn't
	  do this (FIXME) so we can stay in sync with Intel.  960
	    doesn't yet page from files... */
#ifndef I960
      sofar = ALIGN(sofar, 1 << current->alignment_power);
#endif
      /* FIXME, in demand paged files, the low order bits of the file
	offset must match the low order bits of the virtual address.
	  "Low order" is apparently implementation defined.  Add code
	    here to round sofar up to match the virtual address.  */
      
      current->filepos = sofar;
      sofar += current->size;
    }
    obj_relocbase(abfd) = sofar;
  }




/* SUPPRESS 558 */
/* SUPPRESS 529 */
static          boolean
DEFUN(coff_write_object_contents,(abfd),
bfd            *abfd)
{
  asection       *current;
  boolean         hasrelocs = false;
  boolean         haslinno = false;
  file_ptr        reloc_base;
  file_ptr        lineno_base;
  file_ptr        sym_base;
  file_ptr        scn_base;
  file_ptr        data_base;
  unsigned long   reloc_size = 0;
  unsigned long   lnno_size = 0;
  asection       *text_sec = NULL;
  asection       *data_sec = NULL;
  asection       *bss_sec = NULL;

  struct internal_filehdr internal_f;
  struct internal_aouthdr internal_a;
    
  struct icofdata *coff = obj_icof(abfd);
    
    
  bfd_error = system_call_error;
    
    
  if(abfd->output_has_begun == false) {
    coff_compute_section_file_positions(abfd);
  }
    
  if (abfd->sections != (asection *)NULL) {
    scn_base = abfd->sections->filepos;
  }
  else {
    scn_base = 0;
  }
  if (bfd_seek(abfd, scn_base, SEEK_SET) != 0)
    return false;
  reloc_base = obj_relocbase(abfd);
    
  /* Make a pass through the symbol table to count line number entries and
     put them into the correct asections */
    
  coff_count_linenumbers(abfd);
  data_base = scn_base;
    
  /* Work out the size of the reloc and linno areas */
    
  for (current = abfd->sections; current != NULL; current = current->next) {
    reloc_size += current->reloc_count * RELSZ;
    lnno_size += current->lineno_count * LINESZ;
    data_base += SCNHSZ;
  }
    
  lineno_base = reloc_base + reloc_size;
  sym_base = lineno_base + lnno_size;
    
  /* Indicate in each section->line_filepos its actual file address */
  for (current = abfd->sections; current != NULL; current = current->next) {
    if (current->lineno_count) {
      current->line_filepos = lineno_base;
      current->moving_line_filepos = lineno_base;
      lineno_base += current->lineno_count * LINESZ;
    }
    else {
      current->line_filepos = 0;
    }
    if (current->reloc_count) {
      current->rel_filepos = reloc_base;
      reloc_base += current->reloc_count * sizeof(struct internal_reloc);
    }
    else {
      current->rel_filepos = 0;
    }
  }
    
  /* Write section headers to the file.  */
    
  bfd_seek(abfd,
	   (file_ptr) ((abfd->flags & EXEC_P) ?
		       (FILHSZ + AOUTSZ) : FILHSZ),
	   SEEK_SET);
    
    {
#if 0
      unsigned int    pad = abfd->flags & D_PAGED ? data_base : 0;
#endif
      unsigned int    pad = 0;
	
      for (current = abfd->sections; current != NULL; current = current->next) {
	struct internal_scnhdr section;
	strncpy(&(section.s_name[0]), current->name, 8);
	section.s_vaddr = current->vma + pad;
	section.s_paddr = current->vma + pad;
	section.s_size = current->size - pad;
	/*
	  If this section has no size or is unloadable then the scnptr
	  will be 0 too
	  */
	if (current->size - pad == 0 ||
	    (current->flags & SEC_LOAD) == 0) {
	  section.s_scnptr = 0;
	    
	}
	else {
	  section.s_scnptr = current->filepos;
	}
	section.s_relptr = current->rel_filepos;
	section.s_lnnoptr = current->line_filepos;
	section.s_nreloc = current->reloc_count;
	section.s_nlnno = current->lineno_count;
	if (current->reloc_count != 0)
	  hasrelocs = true;
	if (current->lineno_count != 0)
	  haslinno = true;
	  
	if (!strcmp(current->name, _TEXT)) {
	  text_sec = current;
	  section.s_flags = STYP_TEXT; /* kinda stupid */
	}
	else if (!strcmp(current->name, _DATA)) {
	  data_sec = current;
	  section.s_flags = STYP_DATA; /* kinda stupid */
	}
	else if (!strcmp(current->name, _BSS)) {
	  bss_sec = current;
	  section.s_flags = STYP_BSS; /* kinda stupid */
	}
	  
	  
#ifdef I960
	section.s_align = (current->alignment_power
			   ? 1 << current->alignment_power
			   : 0);

#endif
	  {
	    SCNHDR          buff;

	    swap_scnhdr_out(abfd, &section, &buff);
	    bfd_write((PTR) (&buff), 1, SCNHSZ, abfd);

	  }
	pad = 0;
      }
    }

  /* OK, now set up the filehdr... */
  internal_f.f_nscns = abfd->section_count;
  /*
    We will NOT put a fucking timestamp in the header here. Every time you
    put it back, I will come in and take it out again. I'm sorry. This
    field does not belong here.  We fill it with a 0 so it compares the
    same but is not a reasonable time. -- gnu@cygnus.com
    */
  /*
    Well, I like it, so I'm conditionally compiling it in.
    steve@cygnus.com
    */
#ifdef COFF_TIMESTAMP
  internal_f.f_timdat = time(0);
#else
  internal_f.f_timdat = 0;
#endif

  if (bfd_get_symcount(abfd) != 0)
    internal_f.f_symptr = sym_base;
  else
    internal_f.f_symptr = 0;

  internal_f.f_flags = 0;

  if (abfd->flags & EXEC_P)
    internal_f.f_opthdr = AOUTSZ;
  else
    internal_f.f_opthdr = 0;

  if (!hasrelocs)
    internal_f.f_flags |= F_RELFLG;
  if (!haslinno)
    internal_f.f_flags |= F_LNNO;
  if (0 == bfd_get_symcount(abfd))
    internal_f.f_flags |= F_LSYMS;
  if (abfd->flags & EXEC_P)
    internal_f.f_flags |= F_EXEC;
#if M88
  internal_f.f_flags |= F_AR32W;
#else
  if (!abfd->xvec->byteorder_big_p)
    internal_f.f_flags |= F_AR32WR;
#endif
  /*
    FIXME, should do something about the other byte orders and
    architectures.
    */

  /* Set up architecture-dependent stuff */

    { int   magic = 0;
      int   flags = 0;
      coff_set_flags(abfd, &magic, &flags);
      internal_f.f_magic = magic;
      internal_f.f_flags = flags;

      /* ...and the "opt"hdr... */

#ifdef I960
      internal_a.magic = (magic == I960ROMAGIC ? NMAGIC : OMAGIC); 
#endif
#if M88
      internal_a.magic = PAGEMAGICBCS;
#endif
    }
  /* Now should write relocs, strings, syms */
  obj_sym_filepos(abfd) = sym_base;

  if (bfd_get_symcount(abfd) != 0) {
    coff_mangle_symbols(abfd);
    coff_write_symbols(abfd);
    coff_write_linenumbers(abfd);
    coff_write_relocs(abfd);
  }
  if (text_sec) {
    internal_a.tsize = text_sec->size;
    internal_a.text_start =text_sec->size ? text_sec->vma : 0;
  }
  if (data_sec) {
    internal_a.dsize = data_sec->size;
    internal_a.data_start = data_sec->size ? data_sec->vma      : 0;
  }
  if (bss_sec) {
    internal_a.bsize =  bss_sec->size;
  }

  internal_a.entry = bfd_get_start_address(abfd);
  internal_f.f_nsyms =  bfd_get_symcount(abfd);

  /* now write them */
  if (bfd_seek(abfd, 0L, SEEK_SET) != 0)
    return false;
    {
      FILHDR buff;
      bfd_swap_filehdr_out(abfd, &internal_f, &buff);
      bfd_write((PTR) &buff, 1, FILHSZ, abfd);
    }
  if (abfd->flags & EXEC_P) {
    AOUTHDR buff;
    bfd_swap_aouthdr_out(abfd, &internal_a, &buff);
    bfd_write((PTR) &buff, 1, AOUTSZ, abfd);
  }
  return true;
}

static          boolean
coff_set_section_contents(abfd, section, location, offset, count)
    bfd            *abfd;
    sec_ptr         section;
    PTR             location;
    file_ptr        offset;
    size_t          count;
{
    if (abfd->output_has_begun == false)	/* set by bfd.c handler */
	coff_compute_section_file_positions(abfd);

    bfd_seek(abfd, (file_ptr) (section->filepos + offset), SEEK_SET);

    if (count != 0) {
	return (bfd_write(location, 1, count, abfd) == count) ? true : false;
    }
    return true;
}
#if 0
static          boolean
coff_close_and_cleanup(abfd)
    bfd            *abfd;
{
  if (!bfd_read_p(abfd))
    switch (abfd->format) {
    case bfd_archive:
      if (!_bfd_write_archive_contents(abfd))
	return false;
      break;
    case bfd_object:
      if (!coff_write_object_contents(abfd))
	return false;
      break;
    default:
      bfd_error = invalid_operation;
      return false;
    }

  /* We depend on bfd_close to free all the memory on the obstack.  */
  /* FIXME if bfd_release is not using obstacks! */
  return true;
}

#endif
static PTR 
buy_and_read(abfd, where, seek_direction, size)
    bfd            *abfd;
    file_ptr        where;
    int             seek_direction;
    size_t          size;
{
    PTR             area = (PTR) bfd_alloc(abfd, size);
    if (!area) {
	bfd_error = no_memory;
	return (NULL);
    }
    bfd_seek(abfd, where, seek_direction);
    if (bfd_read(area, 1, size, abfd) != size) {
	bfd_error = system_call_error;
	return (NULL);
    }				/* on error */
    return (area);
}				/* buy_and_read() */

static void 
DEFUN(offset_symbol_indices,(abfd, symtab, count, offset),
      bfd *abfd AND
      struct internal_syment         *symtab AND
      unsigned long   count AND
      long            offset)
{
  struct internal_syment         *end = symtab + count;
  for (; symtab < end; ++symtab) {
    if (symtab->n_sclass == C_FILE) {
      symtab->n_value = 0;
    }
    else if (symtab->n_sclass == C_ALIAS) {
      /*
	These guys have indices in their values.
	*/
      symtab->n_value = symtab->n_value + offset;
    }
    else if (symtab->n_numaux) {
      /*
	anybody else without an aux, has no indices.
	*/
      
      if (symtab->n_sclass == C_EOS
	  || (BTYPE(symtab->n_type) == T_STRUCT
	      && symtab->n_sclass != C_STRTAG)
	  || BTYPE(symtab->n_type) == T_UNION
	  || BTYPE(symtab->n_type) == T_ENUM) {
	/* If the tagndx is 0 then the struct hasn't really been
	   defined, so leave it alone */
	
	if(((union internal_auxent *) (symtab + 1))->x_sym.x_tagndx != 0) {
	  ((union internal_auxent *) (symtab + 1))->x_sym.x_tagndx += offset;
	}
	
      }				/* These guys have a tagndx */
      if (symtab->n_sclass == C_STRTAG
	  || symtab->n_sclass == C_UNTAG
	  || symtab->n_sclass == C_ENTAG
	  || symtab->n_sclass == C_BLOCK
	  || symtab->n_sclass == C_FCN
	  || ISFCN(symtab->n_type)) {
	
	((union internal_auxent *) (symtab +
				    1))->x_sym.x_fcnary.x_fcn.x_endndx
				      += offset;
	
      }				/* These guys have an endndx */
#ifndef I960
      if (ISFCN(symtab->n_type)) {
	((union internal_auxent *) (symtab + 1))->x_sym.x_tvndx += offset;
      }				/* These guys have a tvndx.  I think...
				   (FIXME) */
#endif				/* Not I960 */
      
    }				/* if value, else if aux */
    symtab += symtab->n_numaux;
  }				/* walk the symtab */
  
  return;
}				/* offset_symbol_indices() */

#if 0
/* swap the entire symbol table - we c*/
static void 
swap_raw_symtab(abfd, raw_symtab)
bfd            *abfd;
SYMENT         *raw_symtab;
  {
    long            i;
    SYMENT         *end = raw_symtab + bfd_get_symcount(abfd);
    for (; raw_symtab < end; ++raw_symtab) {
      bfd_coff_swap_sym(abfd, raw_symtab);
      
      for (i = raw_symtab->n_numaux; i; --i, ++raw_symtab) {
	bfd_coff_swap_aux(abfd,
			  (AUXENT *)(raw_symtab + 1),
			  raw_symtab->n_type,
			  raw_symtab->n_sclass);
      }				/* swap all the aux entries */
    }				/* walk the symbol table */
    
    return;
  }				/* swap_raw_symtab() */
#endif
/*
read a symbol table into freshly mallocated memory, swap it, and knit the
symbol names into a normalized form. By normalized here I mean that all
symbols have an n_offset pointer that points to a NULL terminated string.
Oh, and the first symbol MUST be a C_FILE.  If there wasn't one there
before, put one there.
*/

static struct internal_syment  *
DEFUN(get_normalized_symtab,(abfd),
bfd            *abfd)
{

  struct internal_syment         *internal;
  struct internal_syment         *internal_ptr;
  struct internal_syment         *internal_end;
  SYMENT *raw;
  SYMENT *raw_src;
  SYMENT *raw_end;
  char           *string_table = NULL;
  unsigned long   size;
  char string_table_size_buffer[4];
  unsigned long   string_table_size = 0;
  unsigned int raw_size;
  if (obj_raw_syments(abfd) != (struct internal_syment *)NULL) {
    return obj_raw_syments(abfd);
  }
  if ((size = bfd_get_symcount(abfd) * sizeof(struct internal_syment)) == 0) {
    bfd_error = no_symbols;
    return (NULL);
  }

  internal = (struct internal_syment *)bfd_alloc(abfd, size);
  internal_end = internal + bfd_get_symcount(abfd);

  raw_size =      bfd_get_symcount(abfd) * SYMESZ;
  raw = (SYMENT *)bfd_alloc(abfd,raw_size);

  if (bfd_seek(abfd, obj_sym_filepos(abfd), SEEK_SET) == -1
      || bfd_read((PTR)raw, raw_size, 1, abfd) != raw_size) {
    bfd_error = system_call_error;
    return (NULL);
  }
  /* mark the end of the symbols */
  raw_end = raw + bfd_get_symcount(abfd);
  /*
    FIXME SOMEDAY.  A string table size of zero is very weird, but
    probably possible.  If one shows up, it will probably kill us.
    */

  /* Swap all the raw entries */
  for (raw_src = raw, internal_ptr = internal; raw_src < raw_end; raw_src++, internal_ptr++) {
    unsigned int i;
    bfd_coff_swap_sym_in(abfd, raw_src,internal_ptr);    
    for (i = internal_ptr->n_numaux; i; --i, raw_src++, internal_ptr++) {
      bfd_coff_swap_aux_in(abfd, (AUXENT *)(raw_src +1), internal_ptr->n_type,
			   internal_ptr->n_sclass, (union
						    internal_auxent *)(internal_ptr +1));
    }
  }
      
  /* Free all the raw stuff */
  bfd_release(abfd, raw_src);

  for (internal_ptr = internal; internal_ptr < internal_end; internal_ptr ++) {

    if (internal_ptr->_n._n_n._n_zeroes != 0) {
      /*
	This is a "short" name.  Make it long.
	*/
      unsigned long   i = 0;
      char           *newstring = NULL;
      /*
	find the length of this string without walking into memory
	that isn't ours.
	*/
	
      for (i = 0; i < 8; ++i) {
	if (internal_ptr->_n._n_name[i] == '\0') {
	  break;
	}			/* if end of string */
      }				/* possible lengths of this string. */
	
      if ((newstring = (PTR) bfd_alloc(abfd, ++i)) == NULL) {
	bfd_error = no_memory;
	return (NULL);
      }				/* on error */
      bzero(newstring, i);
      strncpy(newstring, internal_ptr->_n._n_name, i-1);
      internal_ptr->_n._n_n._n_offset =  (int) newstring;
      internal_ptr->_n._n_n._n_zeroes = 0;
	
    }
    else {
      if (string_table == NULL) {
	/*
	  NOTE: we don't read the string table until now because we
	  don't necessarily know that we have one until now.
	  */
	/*
	  At this point we should be "seek"'d to the end of the
	  symbols === the symbol table size.
	  */
	  
	if (bfd_read((char *) string_table_size_buffer,
		     sizeof(string_table_size_buffer),
		     1, abfd) != sizeof(string_table_size)) {
	  bfd_error = system_call_error;
	  return (NULL);
	}			/* on error */
	  
	string_table_size = bfd_h_get_32(abfd, string_table_size_buffer);
	  
	if ((string_table = (PTR) bfd_alloc(abfd, string_table_size -= 4)) == NULL) {
	  bfd_error = no_memory;
	  return (NULL);
	}			/* on mallocation error */
	if (bfd_read(string_table, string_table_size, 1, abfd) != string_table_size) {
	  bfd_error = system_call_error;
	  return (NULL);
	}			/* on error */
      }				/* have not yet read the string table. */
      /*
	This is a long name already. Just point it at the string in
	memory.
	*/
      internal_ptr->_n._n_n._n_offset = (int) (string_table - 4 +  internal_ptr->_n._n_n._n_offset);

    }				/* switch on type of symbol name */
      
    internal_ptr += internal_ptr->n_numaux;
  }				/* for each symbol */
#if 0
#ifndef GNU960
  /* I'm not sure of the repercussions of this, so the Intel
     folks will always do the force
     */
  if (obj_symbol_slew(abfd) > 0) 
    force_indices_file_symbol_relative(abfd, internal);
#else
  force_indices_file_symbol_relative(abfd, internal);
#endif
#endif
  obj_raw_syments(abfd) = internal;
  obj_string_table(abfd) = string_table;
    
  return (internal);
}				/* get_normalized_symtab() */

static
struct sec *
DEFUN(section_from_bfd_index,(abfd, index),
      bfd            *abfd AND
      int             index)
{
  if (index > 0) {
    struct sec *answer = abfd->sections;
    while (--index) {
      answer = answer->next;
    }
    return answer;
  }
  return 0;
}




static boolean
coff_slurp_line_table(abfd, asect)
bfd            *abfd;
asection       *asect;
  {
    LINENO  *native_lineno;
    alent          *lineno_cache;
    
    BFD_ASSERT(asect->lineno == (alent *) NULL);
    
    native_lineno = (LINENO *) buy_and_read(abfd,
					    asect->line_filepos,
					    SEEK_SET,
					    (size_t) (LINESZ *
						      asect->lineno_count));
    lineno_cache =
      (alent *) bfd_alloc(abfd, (size_t) ((asect->lineno_count + 1) * sizeof(alent)));
    if (lineno_cache == NULL) {
      bfd_error = no_memory;
      return false;
    } else {	
      unsigned int    counter = 0;
      alent          *cache_ptr = lineno_cache;
      LINENO  *src = native_lineno;
      
      while (counter < asect->lineno_count) {
	struct internal_lineno dst;
	bfd_coff_swap_lineno_in(abfd, src, &dst);
	cache_ptr->line_number = dst.l_lnno;
	
	if (cache_ptr->line_number == 0) {
	  coff_symbol_type *sym =
	    (coff_symbol_type *) (dst.l_addr.l_symndx
				  + obj_symbol_slew(abfd)
				  + obj_raw_syments(abfd))->_n._n_n._n_zeroes;
	  cache_ptr->u.sym = (asymbol *) sym;
	  sym->lineno = cache_ptr;
	}
	else {
	  cache_ptr->u.offset = dst.l_addr.l_paddr
	    - bfd_section_vma(abfd, asect);
	}				/* If no linenumber expect a symbol index */
	
	cache_ptr++;
	src++;
	counter++;
      }
      cache_ptr->line_number = 0;
      
    }
    asect->lineno = lineno_cache;
    /* FIXME, free native_lineno here, or use alloca or something. */
    return true;
  }				/* coff_slurp_line_table() */

#if 0
static struct internal_syment  *
DEFUN(find_next_file_symbol,(abfd, current, end),
      bfd *abfd AND
      struct internal_syment *current AND
      struct internal_syment *end)
{
  current += current->n_numaux + 1;
    
  while (current < end) {
    if (current->n_sclass== C_FILE) {
      return (current);
    }	
    current += current->n_numaux + 1;
  }
  return end;
}
#endif

/*
Note that C_FILE symbols can, and some do, have more than 1 aux entry.
*/
#if 0
static void
DEFUN(force_indices_file_symbol_relative,(abfd, symtab),
      bfd            *abfd AND
     struct internal_syment         *symtab)
{
  struct internal_syment         *end = symtab + bfd_get_symcount(abfd);
  struct internal_syment         *current;
  struct internal_syment         *next;
  /* the first symbol had damn well better be a C_FILE. */
  BFD_ASSERT(symtab->n_sclass == C_FILE);
    
  for (current = find_next_file_symbol(abfd, symtab, end);
       current < end;
       current = next) {
    offset_symbol_indices(abfd, current,
			  ((next =
			    find_next_file_symbol(abfd, current,
						  end)) - current),
			  symtab - current);
  }
  return;
}	
#endif
static          boolean
DEFUN(coff_slurp_symbol_table,(abfd),
      bfd            *abfd)
  {
    struct internal_syment         *native_symbols;
    coff_symbol_type *cached_area;
    unsigned int   *table_ptr;
    
    unsigned int    number_of_symbols = 0;
    if (obj_symbols(abfd))
      return true;
    bfd_seek(abfd, obj_sym_filepos(abfd), SEEK_SET);
    
    /* Read in the symbol table */
    if ((native_symbols = get_normalized_symtab(abfd)) == NULL) {
      return (false);
    }				/* on error */
    
    
    /* Allocate enough room for all the symbols in cached form */
    cached_area =
      (coff_symbol_type *)
	bfd_alloc(abfd, (size_t) (bfd_get_symcount(abfd) * sizeof(coff_symbol_type)));
    
    if (cached_area == NULL) {
      bfd_error = no_memory;
      return false;
    }				/* on error */
    table_ptr =
      (unsigned int *)
	bfd_alloc(abfd, (size_t) (bfd_get_symcount(abfd) * sizeof(unsigned int)));
    
    if (table_ptr == NULL) {
      bfd_error = no_memory;
      return false;
    } else {
      coff_symbol_type *dst = cached_area;
      unsigned int    last_native_index = bfd_get_symcount(abfd);
      unsigned int    this_index = 0;
      while (this_index < last_native_index) {
	struct internal_syment         *src = native_symbols + this_index;
	table_ptr[this_index] = number_of_symbols;
	dst->symbol.the_bfd = abfd;
	
	dst->symbol.name = (char *)(src->_n._n_n._n_offset);
	/*
	  We use the native name field to point to the cached field
	    */
	src->_n._n_n._n_zeroes = (int) dst;
	dst->symbol.section = section_from_bfd_index(abfd,
						     src->n_scnum);
	switch (src->n_sclass) {
#ifdef I960
	case C_LEAFEXT:
#if 0
	  dst->symbol.value = src->n_value - dst->symbol.section->vma;
	  dst->symbol.flags = BSF_EXPORT | BSF_GLOBAL;
	  dst->symbol.flags |= BSF_NOT_AT_END;
#endif
	  /* Fall through to next case */
	  
#endif
	  
	case C_EXT:
	  if ((src->n_scnum) == 0) {
	    if ((src->n_value) == 0) {
	      dst->symbol.flags = BSF_UNDEFINED;
	      dst->symbol.value= 0;
	    }
	    else {
	      dst->symbol.flags = BSF_FORT_COMM;
	      dst->symbol.value = (src->n_value);
	    }
	  }
	  else {
	    /*
	      Base the value as an index from the base of the
		section
		  */
	    if (dst->symbol.section == (asection *) NULL) {
	      dst->symbol.flags = BSF_EXPORT | BSF_GLOBAL | BSF_ABSOLUTE;
	      dst->symbol.value = src->n_value;
	    }
	    else {
	      dst->symbol.flags = BSF_EXPORT | BSF_GLOBAL;
	      dst->symbol.value = src->n_value - dst->symbol.section->vma;
	    }
	    if (ISFCN((src->n_type))) {
	      /*
		A function ext does not go at the end of a file
		  */
	      dst->symbol.flags |= BSF_NOT_AT_END;
	    }
	  }
	  
	  break;
	case C_STAT:			/* static			 */
#ifdef I960
	case C_LEAFSTAT:		/* static leaf procedure        */
#endif
	case C_LABEL:			/* label			 */
	  dst->symbol.flags = BSF_LOCAL;
	  /*
	    Base the value as an index from the base of the section
	      */
	  dst->symbol.value = (src->n_value) - dst->symbol.section->vma;
	  break;
	  
	case C_MOS:			/* member of structure	 */
	case C_EOS:			/* end of structure		 */
	case C_REGPARM:			/* register parameter		 */
	case C_REG:			/* register variable		 */
#ifdef C_AUTOARG
	case C_AUTOARG:			/* 960-specific storage class */
#endif
	case C_TPDEF:			/* type definition		 */
	  
	case C_ARG:
	case C_AUTO:			/* automatic variable */
	case C_FIELD:			/* bit field */
	case C_ENTAG:			/* enumeration tag		 */
	case C_MOE:			/* member of enumeration	 */
	case C_MOU:			/* member of union		 */
	case C_UNTAG:			/* union tag			 */
	  
	  dst->symbol.flags = BSF_DEBUGGING;
	  dst->symbol.value = (src->n_value);
	  break;
	  
	case C_FILE:			/* file name			 */
	case C_STRTAG:			/* structure tag		 */
	  dst->symbol.flags = BSF_DEBUGGING;
	  dst->symbol.value = (src->n_value);
	  
	  break;
	case C_BLOCK:			/* ".bb" or ".eb"		 */
	case C_FCN:			/* ".bf" or ".ef"		 */
	  dst->symbol.flags = BSF_LOCAL;
	  /*
	    Base the value as an index from the base of the section
	      */
	  dst->symbol.value = (src->n_value) - dst->symbol.section->vma;
	  
	  break;
	case C_EFCN:			/* physical end of function	 */
	case C_NULL:
	case C_EXTDEF:			/* external definition		 */
	case C_ULABEL:			/* undefined label		 */
	case C_USTATIC:			/* undefined static		 */
	case C_LINE:			/* line # reformatted as symbol table entry */
	case C_ALIAS:			/* duplicate tag		 */
	case C_HIDDEN:			/* ext symbol in dmert public lib */
	  
	default:
	  
	  abort();
	  dst->symbol.flags = BSF_DEBUGGING;
	  dst->symbol.value = (src->n_value);
	  
	  break;
	}
	
	BFD_ASSERT(dst->symbol.flags != 0);
	
	dst->native = src;
	
	dst->symbol.udata = 0;
	dst->lineno = (alent *) NULL;
	this_index += (src->n_numaux) + 1;
	dst++;
	number_of_symbols++;
      }				/* walk the native symtab */
    }				/* bfdize the native symtab */
    
    obj_symbols(abfd) = cached_area;
    obj_raw_syments(abfd) = native_symbols;
    
    bfd_get_symcount(abfd) = number_of_symbols;
    obj_convert(abfd) = table_ptr;
    /* Slurp the line tables for each section too */
      {
	asection       *p;
	p = abfd->sections;
	while (p) {
	  coff_slurp_line_table(abfd, p);
	  p = p->next;
	}
      }
    return true;
  }				/* coff_slurp_symbol_table() */

static unsigned int
coff_get_symtab_upper_bound(abfd)
bfd            *abfd;
  {
    if (!coff_slurp_symbol_table(abfd))
      return 0;
    
    return (bfd_get_symcount(abfd) + 1) * (sizeof(coff_symbol_type *));
  }


static unsigned int
coff_get_symtab(abfd, alocation)
bfd            *abfd;
asymbol       **alocation;
  {
    unsigned int    counter = 0;
    coff_symbol_type *symbase;
    coff_symbol_type **location = (coff_symbol_type **) (alocation);
    if (!coff_slurp_symbol_table(abfd))
      return 0;
    
    for (symbase = obj_symbols(abfd); counter++ < bfd_get_symcount(abfd);)
      *(location++) = symbase++;
    *location++ = 0;
    return bfd_get_symcount(abfd);
  }

static unsigned int
coff_get_reloc_upper_bound(abfd, asect)
bfd            *abfd;
sec_ptr         asect;
  {
    if (bfd_get_format(abfd) != bfd_object) {
      bfd_error = invalid_operation;
      return 0;
    }
    return (asect->reloc_count + 1) * sizeof(arelent *);
  }

static          boolean
DEFUN(coff_slurp_reloc_table,(abfd, asect, symbols),
      bfd            *abfd AND
      sec_ptr         asect AND
      asymbol       **symbols)
  {
    RELOC   *native_relocs;
    arelent        *reloc_cache;
    if (asect->relocation)
      return true;
    if (asect->reloc_count == 0)
      return true;
    if (!coff_slurp_symbol_table(abfd))
      return false;
    native_relocs =
      (RELOC *) buy_and_read(abfd,
			     asect->rel_filepos,
			     SEEK_SET,
			     (size_t) (RELSZ *
				       asect->reloc_count));
    reloc_cache = (arelent *)
      bfd_alloc(abfd, (size_t) (asect->reloc_count * sizeof(arelent)));
    
    if (reloc_cache == NULL) {
      bfd_error = no_memory;
      return false;
    } {				/* on error */
      arelent        *cache_ptr;
      RELOC   *src;
      for (cache_ptr = reloc_cache,
	   src = native_relocs;
	   cache_ptr < reloc_cache + asect->reloc_count;
	   cache_ptr++,
	   src++) {
	struct internal_reloc dst;
	asymbol        *ptr;
	bfd_swap_reloc_in(abfd, src, &dst);
	dst.r_symndx += obj_symbol_slew(abfd);
	cache_ptr->sym_ptr_ptr = symbols + obj_convert(abfd)[dst.r_symndx];
	
	ptr = *(cache_ptr->sym_ptr_ptr);
	cache_ptr->address = dst.r_vaddr;
	/*
	  The symbols definitions that we have read in have been
	  relocated as if their sections started at 0. But the offsets
	  refering to the symbols in the raw data have not been
	  modified, so we have to have a negative addend to compensate.
	  
	  Note that symbols which used to be common must be left alone
	  */
	
	if (ptr->the_bfd == abfd 
	    && ptr->section != (asection *) NULL 
	    && ((ptr->flags & BSF_OLD_COMMON)== 0)) 
	    {
	      cache_ptr->addend = -(ptr->section->vma + ptr->value);
	    }
	else {
	  cache_ptr->addend = 0;
	}
	
	cache_ptr->address -= asect->vma;
	
	cache_ptr->section = (asection *) NULL;
	
#if I960
	cache_ptr->howto = howto_table + dst.r_type;
#endif
#if M68
	cache_ptr->howto = howto_table + dst.r_type - R_RELBYTE;
#endif
#if M88
	if (dst.r_type >= R_PCR16L && dst.r_type <= R_VRT32) {
	  cache_ptr->howto = howto_table + dst.r_type - R_PCR16L;
	  cache_ptr->addend += dst.r_offset << 16;
	}
	else {
	  BFD_ASSERT(0);
	}
#endif
	
      }
      
    }
    
    asect->relocation = reloc_cache;
    return true;
  }


/* This is stupid.  This function should be a boolean predicate */
static unsigned int
coff_canonicalize_reloc(abfd, section, relptr, symbols)
bfd            *abfd;
sec_ptr         section;
arelent       **relptr;
asymbol       **symbols;
  {
    arelent        *tblptr = section->relocation;
    unsigned int    count = 0;
    if (!(tblptr || coff_slurp_reloc_table(abfd, section, symbols)))
      return 0;
    tblptr = section->relocation;
    if (!tblptr)
      return 0;
    
    for (; count++ < section->reloc_count;)
      *relptr++ = tblptr++;
    
    *relptr = 0;
    
    return section->reloc_count;
  }


/*
provided a bfd, a section and an offset into the section, calculate and
return the name of the source file and the line nearest to the wanted
location.
*/

static          boolean
DEFUN(coff_find_nearest_line,(abfd,
			      section,
			      symbols,
			      offset,
			      filename_ptr,
			      functionname_ptr,
			      line_ptr),
      bfd            *abfd AND
      asection       *section AND
      asymbol       **symbols AND
      bfd_vma         offset AND
      CONST char      **filename_ptr AND
      CONST char       **functionname_ptr AND
      unsigned int   *line_ptr)
{
  static bfd     *cache_abfd;
  static asection *cache_section;
  static bfd_vma  cache_offset;
  static unsigned int cache_i;
  static alent   *cache_l;
    
  unsigned int    i = 0;
  struct icofdata *cof = obj_icof(abfd);
  /* Run through the raw syments if available */
  struct internal_syment         *p;
  alent          *l;
  unsigned int    line_base = 0;
    
    
  *filename_ptr = 0;
  *functionname_ptr = 0;
  *line_ptr = 0;
    
  /* Don't try and find line numbers in a non coff file */
  if (abfd->xvec->flavour != bfd_target_coff_flavour_enum)
    return false;
    
  if (cof == (struct icofdata *)NULL)
    return false;
    
  p = cof->raw_syments;
  /*
    I don't know for sure what's right, but this isn't it. First off, an
    object file may not have any C_FILE's in it.  After
    get_normalized_symtab(), it should have at least 1, the one I put
    there, but otherwise, all bets are off.  Point #2, the first C_FILE
    isn't necessarily the right C_FILE because any given object may have
    many.  I think you'll have to track sections as they coelesce in order
    to find the C_STAT symbol for this section. Then you'll have to work
    backwards to find the previous C_FILE, or choke if you get to a C_STAT
    for the same kind of section.  That will mean that the original object
    file didn't have a C_FILE. xoxorich.
    */
    
#ifdef WEREBEINGPEDANTIC
  return false;
#endif
    
  for (i = 0; i < cof->raw_syment_count; i++) {
    if (p->n_sclass == C_FILE) {
      /* File name is embeded in auxent */
      /*
	This isn't right.  The fname should probably be normalized
	during get_normalized_symtab().  In any case, what was here
	wasn't right because a SYMENT.n_name isn't an
	AUXENT.x_file.x_fname. xoxorich.
	*/
	
      *filename_ptr = ((AUXENT *) (p + 1))->x_file.x_fname;
      break;
    }
    p += 1 +  p->n_numaux;
  }
  /* Now wander though the raw linenumbers of the section */
  /*
    If this is the same bfd as we were previously called with and this is
    the same section, and the offset we want is further down then we can
    prime the lookup loop
    */
  if (abfd == cache_abfd &&
      section == cache_section &&
      offset >= cache_offset) {
    i = cache_i;
    l = cache_l;
  }
  else {
    i = 0;
    l = section->lineno;
  }
    
  for (; i < section->lineno_count; i++) {
    if (l->line_number == 0) {
      /* Get the symbol this line number points at */
      coff_symbol_type *coff = (coff_symbol_type *) (l->u.sym);
      *functionname_ptr = coff->symbol.name;
      if (coff->native) {
	struct internal_syment  *s = coff->native;
	s = s + 1 + s->n_numaux;
	/*
	  S should now point to the .bf of the function
	  */
	if (s->n_numaux) {
	  /*
	    The linenumber is stored in the auxent
	    */
	  union internal_auxent   *a = (union internal_auxent *) (s + 1);
	  line_base = a->x_sym.x_misc.x_lnsz.x_lnno;
	}
      }
    }
    else {
      if (l->u.offset > offset)
	break;
      *line_ptr = l->line_number + line_base + 1;
    }
    l++;
  }
    
  cache_abfd = abfd;
  cache_section = section;
  cache_offset = offset;
  cache_i = i;
  cache_l = l;
  return true;
}

#ifdef GNU960
file_ptr
coff_sym_filepos(abfd)
bfd *abfd;
  {
    return obj_sym_filepos(abfd);
  }
#endif


static int 
DEFUN(coff_sizeof_headers,(abfd, reloc),
      bfd *abfd AND
      boolean reloc)
  {
    size_t size;
    
    if (reloc == false) {
      size = FILHSZ + AOUTSZ;
    }
    else {
      size = FILHSZ;
    }
    
    size +=  abfd->section_count * SCNHSZ;
    return size;
  }


#define coff_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define coff_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define coff_core_file_matches_executable_p	_bfd_dummy_core_file_matches_executable_p
#define coff_slurp_armap		bfd_slurp_coff_armap
#define coff_slurp_extended_name_table	_bfd_slurp_extended_name_table
#define coff_truncate_arname		bfd_dont_truncate_arname
#define coff_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define coff_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	coff_get_section_contents	bfd_generic_get_section_contents
#define	coff_close_and_cleanup		bfd_generic_close_and_cleanup
