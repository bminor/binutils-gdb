
/* All the swapping routines:
*/



static void
DEFUN(swap_reloc_in,(abfd, reloc_src, reloc_dst),
      bfd            *abfd AND
      RELOC *reloc_src AND
      struct internal_reloc *reloc_dst)
{
  reloc_dst->r_vaddr = bfd_h_getlong(abfd, reloc_src->r_vaddr);
  reloc_dst->r_symndx = bfd_h_getlong(abfd, reloc_src->r_symndx);
  reloc_dst->r_type = bfd_h_getshort(abfd, reloc_src->r_type);
#if M88
  reloc_dst->r_offset = bfd_h_getshort(abfd, reloc_src->r_offset);
#endif
}

static void
DEFUN(swap_reloc_out,(abfd, reloc_src, reloc_dst),
      bfd            *abfd AND
      struct internal_reloc *reloc_src AND
      struct external_reloc *reloc_dst)
{
  bfd_h_putlong(abfd, reloc_src->r_vaddr, reloc_dst->r_vaddr);
  bfd_h_putlong(abfd, reloc_src->r_symndx, reloc_dst->r_symndx);
  bfd_h_putshort(abfd, reloc_src->r_type, reloc_dst->r_type);
#if M88
  bfd_h_putshort(abfd, reloc_src->r_offset, reloc_dst->r_offset);
#endif

}

static void 
DEFUN(swap_filehdr_in,(abfd, filehdr_src, filehdr_dst),
      bfd            *abfd AND
      FILHDR         *filehdr_src AND
      struct internal_filehdr *filehdr_dst)
{
  filehdr_dst->f_magic = bfd_h_get_x(abfd, filehdr_src->f_magic);
  filehdr_dst->f_nscns = bfd_h_get_x(abfd,filehdr_src-> f_nscns);
  filehdr_dst->f_timdat = bfd_h_get_x(abfd,filehdr_src-> f_timdat);
  filehdr_dst->f_symptr = bfd_h_get_x(abfd,filehdr_src-> f_symptr);
  filehdr_dst->f_nsyms = bfd_h_get_x(abfd,filehdr_src-> f_nsyms);
  filehdr_dst->f_opthdr = bfd_h_get_x(abfd,filehdr_src-> f_opthdr);
  filehdr_dst->f_flags = bfd_h_get_x(abfd,filehdr_src-> f_flags);
}

static void 
DEFUN(swap_filehdr_out,(abfd, filehdr_in, filehdr_out),
      bfd            *abfd AND
      struct internal_filehdr *filehdr_in AND
      FILHDR         *filehdr_out)
{
  bfd_h_put_x(abfd, filehdr_in->f_magic, filehdr_out->f_magic);
  bfd_h_put_x(abfd, filehdr_in->f_nscns, filehdr_out->f_nscns);
  bfd_h_put_x(abfd, filehdr_in->f_timdat, filehdr_out->f_timdat);
  bfd_h_put_x(abfd, filehdr_in->f_symptr, filehdr_out->f_symptr);
  bfd_h_put_x(abfd, filehdr_in->f_nsyms, filehdr_out->f_nsyms);
  bfd_h_put_x(abfd, filehdr_in->f_opthdr, filehdr_out->f_opthdr);
  bfd_h_put_x(abfd, filehdr_in->f_flags, filehdr_out->f_flags);
}


static void 
DEFUN(bfd_coff_swap_sym_in,(abfd, ext, in),
      bfd            *abfd AND
      SYMENT *ext AND
      struct internal_syment      *in)
{
  if( ext->e.e_name[0] == 0) {
    in->_n._n_n._n_zeroes = 0;
    in->_n._n_n._n_offset = bfd_h_getlong(abfd, ext->e.e.e_offset);
  }
  else {
    memcpy(in->_n._n_name, ext->e.e_name, SYMNMLEN);
  }
  in->n_value = bfd_h_get_x(abfd, ext->e_value);
  in->n_scnum = bfd_h_get_x(abfd, ext->e_scnum);
  in->n_type = bfd_h_get_x(abfd, ext->e_type);
  in->n_sclass = bfd_h_get_x(abfd, ext->e_sclass);
  in->n_numaux = bfd_h_get_x(abfd, ext->e_numaux);
}

static void 
DEFUN(bfd_coff_swap_sym_out,(abfd,in,  ext),
      bfd            *abfd AND
      struct internal_syment      *in AND
      SYMENT *ext)
{
  if(in->_n._n_name[0] == 0) {
    bfd_h_putlong(abfd, 0, ext->e.e.e_zeroes);
    bfd_h_putlong(abfd, in->_n._n_n._n_offset,  ext->e.e.e_offset);
  }
  else {
    memcpy(ext->e.e_name, in->_n._n_name, SYMNMLEN);
  }
  bfd_h_put_x(abfd,  in->n_value , ext->e_value);
  bfd_h_put_x(abfd,  in->n_scnum , ext->e_scnum);
  bfd_h_put_x(abfd,  in->n_type , ext->e_type);
  bfd_h_put_x(abfd,  in->n_sclass , ext->e_sclass);
  bfd_h_put_x(abfd,  in->n_numaux , ext->e_numaux);
}

static void
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
      in->x_file.x_n.x_offset  = bfd_h_getlong(abfd, ext->x_file.x_n.x_offset);
    }

    break;
  case C_STAT:
#ifdef C_LEAFSTAT
  case C_LEAFSTAT:
#endif
  case C_HIDDEN:
    if (type == T_NULL) {
      in->x_scn.x_scnlen = bfd_h_get_x(abfd, ext->x_scn.x_scnlen);
      in->x_scn.x_nreloc = bfd_h_get_x(abfd, ext->x_scn.x_nreloc);
      in->x_scn.x_nlinno = bfd_h_get_x(abfd, ext->x_scn.x_nlinno);
      break;
    }
  default:
    in->x_sym.x_tagndx = bfd_h_get_x(abfd, ext->x_sym.x_tagndx);
    in->x_sym.x_tvndx = bfd_h_get_x(abfd, ext->x_sym.x_tvndx);

    if (ISARY(type) || class == C_BLOCK) {
      in->x_sym.x_fcnary.x_ary.x_dimen[0] = bfd_h_get_x(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[0]);
      in->x_sym.x_fcnary.x_ary.x_dimen[1] = bfd_h_get_x(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[1]);
      in->x_sym.x_fcnary.x_ary.x_dimen[2] = bfd_h_get_x(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[2]);
      in->x_sym.x_fcnary.x_ary.x_dimen[3] = bfd_h_get_x(abfd, ext->x_sym.x_fcnary.x_ary.x_dimen[3]);
    }
    else {
      in->x_sym.x_fcnary.x_fcn.x_lnnoptr = bfd_h_get_x(abfd, ext->x_sym.x_fcnary.x_fcn.x_lnnoptr);
      in->x_sym.x_fcnary.x_fcn.x_endndx = bfd_h_get_x(abfd, ext->x_sym.x_fcnary.x_fcn.x_endndx);
    }
    if (ISFCN(type)) {
      in->x_sym.x_misc.x_fsize = bfd_h_get_x(abfd, ext->x_sym.x_misc.x_fsize);
    }
    else {
      in->x_sym.x_misc.x_lnsz.x_lnno = bfd_h_get_x(abfd, ext->x_sym.x_misc.x_lnsz.x_lnno);
      in->x_sym.x_misc.x_lnsz.x_size = bfd_h_get_x(abfd, ext->x_sym.x_misc.x_lnsz.x_size);
    }
  }
}

static void
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
      bfd_h_put_x(abfd, 0, ext->x_file.x_n.x_zeroes );
      bfd_h_put_x(abfd, in->x_file.x_n.x_offset, ext->x_file.x_n.x_offset);
    }

    break;
  case C_STAT:
#ifdef C_LEAFSTAT
  case C_LEAFSTAT:
#endif
  case C_HIDDEN:
    if (type == T_NULL) {
      bfd_h_put_x(abfd, in->x_scn.x_scnlen, ext->x_scn.x_scnlen);
      bfd_h_put_x(abfd, in->x_scn.x_nreloc, ext->x_scn.x_nreloc);
      bfd_h_put_x(abfd, in->x_scn.x_nlinno, ext->x_scn.x_nlinno);
      break;
    }
  default:
    bfd_h_put_x(abfd, in->x_sym.x_tagndx, ext->x_sym.x_tagndx);
    bfd_h_put_x(abfd, in->x_sym.x_tvndx , ext->x_sym.x_tvndx);

    if (ISARY(type) || class == C_BLOCK) {
      bfd_h_put_x(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[0],ext->x_sym.x_fcnary.x_ary.x_dimen[0]);
      bfd_h_put_x(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[1],ext->x_sym.x_fcnary.x_ary.x_dimen[1]);
      bfd_h_put_x(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[2],ext->x_sym.x_fcnary.x_ary.x_dimen[2]);
      bfd_h_put_x(abfd, in->x_sym.x_fcnary.x_ary.x_dimen[3],ext->x_sym.x_fcnary.x_ary.x_dimen[3]);
    }
    else {
      bfd_h_put_x(abfd, in->x_sym.x_fcnary.x_fcn.x_lnnoptr, ext->x_sym.x_fcnary.x_fcn.x_lnnoptr);
      bfd_h_put_x(abfd, in->x_sym.x_fcnary.x_fcn.x_endndx, ext->x_sym.x_fcnary.x_fcn.x_endndx);
    }
    if (ISFCN(type)) {
      bfd_h_put_x(abfd, in->x_sym.x_misc.x_fsize, ext->x_sym.x_misc.x_fsize);
    }
    else {
      bfd_h_put_x(abfd, in->x_sym.x_misc.x_lnsz.x_lnno, ext->x_sym.x_misc.x_lnsz.x_lnno);
      bfd_h_put_x(abfd, in->x_sym.x_misc.x_lnsz.x_size, ext->x_sym.x_misc.x_lnsz.x_size);
    }
  }
}

static void
DEFUN(bfd_coff_swap_lineno_in,(abfd, ext, in),
      bfd            *abfd AND
      LINENO *ext AND
      struct internal_lineno      *in)
{
    in->l_addr.l_symndx = bfd_h_get_x(abfd, ext->l_addr.l_symndx);
    in->l_lnno = bfd_h_get_x(abfd, ext->l_lnno);
}

static void
DEFUN(bfd_coff_swap_lineno_out,(abfd, in, ext),
      bfd            *abfd AND
      struct internal_lineno      *in AND
      struct external_lineno *ext)
{
  bfd_h_put_x(abfd, in->l_addr.l_symndx, ext->l_addr.l_symndx);
  bfd_h_put_x(abfd, in->l_lnno, ext->l_lnno);
}




static void 
DEFUN(swap_aouthdr_in,(abfd, aouthdr_ext, aouthdr_int),
      bfd            *abfd AND
      AOUTHDR        *aouthdr_ext AND
      struct internal_aouthdr *aouthdr_int)
{
  aouthdr_int->magic = bfd_h_get_x(abfd, aouthdr_ext->magic);
  aouthdr_int->vstamp = bfd_h_get_x(abfd, aouthdr_ext->vstamp);
  aouthdr_int->tsize = bfd_h_get_x(abfd, aouthdr_ext->tsize);
  aouthdr_int->dsize = bfd_h_get_x(abfd, aouthdr_ext->dsize);
  aouthdr_int->bsize = bfd_h_get_x(abfd, aouthdr_ext->bsize);
  aouthdr_int->entry = bfd_h_get_x(abfd, aouthdr_ext->entry);
  aouthdr_int->text_start = bfd_h_get_x(abfd, aouthdr_ext->text_start);
  aouthdr_int->data_start = bfd_h_get_x(abfd, aouthdr_ext->data_start);
#ifdef I960
  aouthdr_int->tagentries = bfd_h_get_x(abfd, aouthdr_ext->tagentries);
#endif
}

static void 
DEFUN(swap_aouthdr_out,(abfd, aouthdr_in, aouthdr_out),
      bfd            *abfd AND
      struct internal_aouthdr *aouthdr_in AND
      AOUTHDR        *aouthdr_out)
{
  bfd_h_put_x(abfd, aouthdr_in->magic, aouthdr_out->magic);
  bfd_h_put_x(abfd, aouthdr_in->vstamp, aouthdr_out->vstamp);
  bfd_h_put_x(abfd, aouthdr_in->tsize, aouthdr_out->tsize);
  bfd_h_put_x(abfd, aouthdr_in->dsize, aouthdr_out->dsize);
  bfd_h_put_x(abfd, aouthdr_in->bsize, aouthdr_out->bsize);
  bfd_h_put_x(abfd, aouthdr_in->entry, aouthdr_out->entry);
  bfd_h_put_x(abfd, aouthdr_in->text_start, aouthdr_out->text_start);
  bfd_h_put_x(abfd, aouthdr_in->data_start, aouthdr_out->data_start);
#ifdef I960
  bfd_h_put_x(abfd, aouthdr_in->tagentries, aouthdr_out->tagentries);
#endif
}

static void 
DEFUN(swap_scnhdr_in,(abfd, scnhdr_ext, scnhdr_int),
      bfd            *abfd AND
      SCNHDR         *scnhdr_ext AND
      struct internal_scnhdr *scnhdr_int)
{
  memcpy(scnhdr_int->s_name, scnhdr_ext->s_name, sizeof(scnhdr_int->s_name));
  scnhdr_int->s_vaddr = bfd_h_get_x(abfd, scnhdr_ext->s_vaddr);
  scnhdr_int->s_paddr = bfd_h_get_x(abfd, scnhdr_ext->s_paddr);
  scnhdr_int->s_size = bfd_h_get_x(abfd, scnhdr_ext->s_size);
  scnhdr_int->s_scnptr = bfd_h_get_x(abfd, scnhdr_ext->s_scnptr);
  scnhdr_int->s_relptr = bfd_h_get_x(abfd, scnhdr_ext->s_relptr);
  scnhdr_int->s_lnnoptr = bfd_h_get_x(abfd, scnhdr_ext->s_lnnoptr);
  scnhdr_int->s_nreloc = bfd_h_get_x(abfd, scnhdr_ext->s_nreloc);
  scnhdr_int->s_nlnno = bfd_h_get_x(abfd, scnhdr_ext->s_nlnno);
  scnhdr_int->s_flags = bfd_h_get_x(abfd, scnhdr_ext->s_flags);
#ifdef I960
  scnhdr_int->s_align = bfd_h_get_x(abfd, scnhdr_ext->s_align);
#endif
}

static void 
DEFUN(swap_scnhdr_out,(abfd, scnhdr_int, scnhdr_ext),
      bfd            *abfd AND
      struct internal_scnhdr *scnhdr_int AND
      SCNHDR         *scnhdr_ext)
{
  memcpy(scnhdr_ext->s_name, scnhdr_int->s_name, sizeof(scnhdr_int->s_name));
  bfd_h_put_x(abfd, scnhdr_int->s_vaddr, scnhdr_ext->s_vaddr);
  bfd_h_put_x(abfd, scnhdr_int->s_paddr, scnhdr_ext->s_paddr);
  bfd_h_put_x(abfd, scnhdr_int->s_size, scnhdr_ext->s_size);
  bfd_h_put_x(abfd, scnhdr_int->s_scnptr, scnhdr_ext->s_scnptr);
  bfd_h_put_x(abfd, scnhdr_int->s_relptr, scnhdr_ext->s_relptr);
  bfd_h_put_x(abfd, scnhdr_int->s_lnnoptr, scnhdr_ext->s_lnnoptr);
  bfd_h_put_x(abfd, scnhdr_int->s_nreloc, scnhdr_ext->s_nreloc);
  bfd_h_put_x(abfd, scnhdr_int->s_nlnno, scnhdr_ext->s_nlnno);
  bfd_h_put_x(abfd, scnhdr_int->s_flags, scnhdr_ext->s_flags);
#ifdef I960
  bfd_h_put_x(abfd, scnhdr_int->s_align, scnhdr_ext->s_align);
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
      swap_scnhdr_in(abfd, external_sections + i, &tmp);
      make_a_section_from_file(abfd,&tmp);
    }
  }
  /* Determine the machine architecture and type.  */
  abfd->obj_machine = 0;
  switch (internal_f->f_magic) {
#ifdef MIPS
#ifdef MIPSEBMAGIC
  case  SMIPSEBMAGIC:
  case  SMIPSELMAGIC:
  case  MIPSEBUMAGIC:
  case  MIPSELUMAGIC:
  case MIPSEBMAGIC:
  case MIPSELMAGIC:
    abfd->obj_arch = bfd_arch_mips;
    abfd->obj_machine = 0;
    break;
#endif
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
    
    swap_filehdr_in(abfd, &filehdr, &internal_f);
    
    if (BADMAG(internal_f)) {
      bfd_error = wrong_format;
      return 0;
    }
    nscns =internal_f.f_nscns;
    
    if (internal_f.f_opthdr) {
      if (bfd_read((PTR) &opthdr, 1,AOUTSZ, abfd) != AOUTSZ) {
	return 0;
      }
      swap_aouthdr_in(abfd, &opthdr, &internal_a);
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


