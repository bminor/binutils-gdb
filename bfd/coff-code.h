/*
   Copyright (C) 1990, 1991 Free Software Foundation, Inc.

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
/*
Most of this hacked by Steve Chamberlain, steve@cygnus.com
*/

#include "archures.h"		/* Machine architectures and types */

/* SUPPRESS 558 */
/* SUPPRESS 590 */
/* SUPPRESS 529 */
/* SUPPRESS 530 */

#define ALIGN(this, boundary) \
  ((( (this) + ((boundary) -1)) & (~((boundary)-1))))

#define sp(x) bfd_h_put_x(abfd, x, &x)



/*
   Align an address by rounding it up to a power of two.  It leaves the
   address unchanged if align == 0 (2^0 = alignment of 1 byte)
*/
#define	i960_align(addr, align)	\
	( ((addr) + ((1<<(align))-1)) & (-1 << (align)))


/* All the swapping routines */

static void 
swap_reloc(abfd, reloc)
    bfd            *abfd;
    RELOC          *reloc;
{
    sp(reloc->r_vaddr);
    sp(reloc->r_symndx);
    sp(reloc->r_type);
}


static void 
swap_filehdr(abfd, filehdr)
    bfd            *abfd;
    FILHDR         *filehdr;
{
    sp(filehdr->f_magic);
    sp(filehdr->f_nscns);
    sp(filehdr->f_timdat);
    sp(filehdr->f_symptr);
    sp(filehdr->f_nsyms);
    sp(filehdr->f_opthdr);
    sp(filehdr->f_flags);


}

static void 
swap_aouthdr(abfd, aouthdr)
    bfd            *abfd;
    AOUTHDR        *aouthdr;
{

    sp(aouthdr->magic);
    sp(aouthdr->vstamp);
    sp(aouthdr->tsize);
    sp(aouthdr->dsize);
    sp(aouthdr->bsize);
    sp(aouthdr->entry);
    sp(aouthdr->text_start);
    sp(aouthdr->data_start);
#ifdef I960
    sp(aouthdr->tagentries);
#endif
}

static void 
swap_scnhdr(abfd, scnhdr)
    bfd            *abfd;
    SCNHDR         *scnhdr;
{
    sp(scnhdr->s_vaddr);
    sp(scnhdr->s_paddr);
    sp(scnhdr->s_size);
    sp(scnhdr->s_scnptr);
    sp(scnhdr->s_relptr);
    sp(scnhdr->s_lnnoptr);
    sp(scnhdr->s_nreloc);
    sp(scnhdr->s_nlnno);
    sp(scnhdr->s_flags);
#ifdef I960
    sp(scnhdr->s_align);
#endif
}

static void 
swap_name(abfd, ptr)
    bfd            *abfd;
    long           *ptr;
{
    if (ptr[0] == 0) {
	/* There is an index which needs to be swapped */
	bfd_h_put_x(abfd, ptr[1], (ptr + 1));
    }
    else {
	/* This is a string .. leave it alone */
    }
}

static void 
bfd_coff_swap_sym(abfd, se)
    bfd            *abfd;
    SYMENT         *se;
{
    swap_name(abfd, se->n_name);
    bfd_h_put_x(abfd, se->n_value, &se->n_value);
    bfd_h_put_x(abfd, se->n_scnum, &se->n_scnum);
    bfd_h_put_x(abfd, se->n_type, &se->n_type);
    bfd_h_put_x(abfd, se->n_sclass, &se->n_sclass);
    bfd_h_put_x(abfd, se->n_numaux, &se->n_numaux);
}

static void 
bfd_coff_swap_aux(abfd, au, type, class)
    bfd            *abfd;
    AUXENT         *au;
    int             type;
    int             class;
{
    switch (class) {
    case C_FILE:
	swap_name(abfd, &au->x_file.x_n);
	break;
    case C_STAT:
#ifdef C_LEAFSTAT
    case C_LEAFSTAT:
#endif
    case C_HIDDEN:
	if (type == T_NULL) {
	    sp(au->x_scn.x_scnlen);
	    sp(au->x_scn.x_nreloc);
	    sp(au->x_scn.x_nlinno);
	    break;
	}
    default:
	sp(au->x_sym.x_tagndx);
	sp(au->x_sym.x_tvndx);

	if (ISARY(type)) {
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[0]);
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[1]);
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[2]);
	    sp(au->x_sym.x_fcnary.x_ary.x_dimen[3]);
	}
	else {
	    sp(au->x_sym.x_fcnary.x_fcn.x_lnnoptr);
	    sp(au->x_sym.x_fcnary.x_fcn.x_endndx);
	}
	if (ISFCN(type)) {
	    sp(au->x_sym.x_misc.x_fsize);
	}
	else {
	    sp(au->x_sym.x_misc.x_lnsz.x_lnno);
	    sp(au->x_sym.x_misc.x_lnsz.x_size);
	}
    }
}

static void 
bfd_coff_swap_lineno(abfd, lineno)
    bfd            *abfd;
    LINENO         *lineno;
{
    sp(lineno->l_addr.l_symndx);
    sp(lineno->l_lnno);
}


/* void warning(); */
extern asection abs_section;



static int
get_index(symbol)
    asymbol        *symbol;
{
    return (int) symbol->value;
}

static void 
set_index(symbol, idx)
    asymbol        *symbol;
    unsigned int    idx;
{
    symbol->value = idx;
}

/*
   initialize a section structure with information peculiar to this
   particular implementation of coff
*/

static          boolean
coff_new_section_hook(abfd_ignore, section_ignore)
    bfd            *abfd_ignore;
    asection       *section_ignore;
{
  return true;
}
/* actually it makes itself and its children from the file headers */
static          boolean
make_a_section_from_file(abfd, hdr)
    bfd            *abfd;
    struct scnhdr  *hdr;

{
    asection       *return_section;
    {
	char           *name = malloc(9);
	if (name == NULL) {
	    bfd_error = no_memory;
	    return (BFD_FAILURE);
	}			/* on error */
	strncpy(name, (char *) &hdr->s_name[0], 8);

	return_section = bfd_make_section(abfd, name);
	(return_section->name)[8] = 0;
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


static
bfd_target     *
coff_real_object_p(abfd, nscns, opthdr)
    bfd            *abfd;
    unsigned        nscns,
                    opthdr;
{
  struct icofdata *tdata;
  char           *file_info;	/* buffer for all the headers */
  size_t          readsize;	/* length of file_info */
  struct filehdr *filehdr;	/* points into file_info */
  struct scnhdr  *sections;	/* points into file_info */
  /*
     OK, now we know the format, read in the filehdr, soi-disant "optional
     header", and all the sections.
     */
  readsize = sizeof(struct filehdr)
    + opthdr
      + (nscns * sizeof(struct scnhdr));

  file_info = malloc(readsize);
  if (file_info == NULL) {
    bfd_error = no_memory;
    return 0;
  }
  if (bfd_seek(abfd, 0L, SEEK_SET) < 0)
    return 0;
  if (bfd_read((PTR) file_info, 1, readsize, abfd) != readsize)
    return 0;
  filehdr = (struct filehdr *) file_info;
  sections = (struct scnhdr *) (file_info + sizeof(struct filehdr) + opthdr);


  swap_filehdr(abfd, filehdr);

  /* Now copy data as required; construct all asections etc */
  tdata = (struct icofdata *) malloc(sizeof(struct icofdata) +
				     sizeof(AOUTHDR));
  if (tdata == NULL) {
    bfd_error = no_memory;
    return 0;
  }
  tdata->symbol_index_slew = 0;
  tdata->relocbase =0;
  tdata->raw_syment_count = 0;
  tdata->raw_linenos = 0;
  tdata->raw_syments = 0;
  tdata->sym_filepos =0;
  tdata->flags = filehdr->f_flags;
  if (nscns != 0) {
    unsigned int    i;
    for (i = 0; i < nscns; i++) {
      swap_scnhdr(abfd, sections + i);
      make_a_section_from_file(abfd, sections + i);
    }
  }
  /* Determine the machine architecture and type.  */
  abfd->obj_machine = 0;
  switch (filehdr->f_magic) {
#ifdef MC68MAGIC
  case MC68MAGIC:
  case MC68DMAGIC:
    abfd->obj_arch = bfd_arch_m68k;
    abfd->obj_machine = 68020;
    break;
#endif
#ifdef MC88MAGIC
  case MC88MAGIC:
  case MC88DMAGIC:
    abfd->obj_arch = bfd_arch_m88k;
    abfd->obj_machine = 88100;
    break;
#endif
#ifdef I960ROMAGIC
  case I960ROMAGIC:
  case I960RWMAGIC:
    abfd->obj_arch = bfd_arch_i960;
    switch (F_I960TYPE & filehdr->f_flags) 
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

  default:			/* Unreadable input file type */
    abfd->obj_arch = bfd_arch_obscure;
    break;
  }

  if (!(filehdr->f_flags & F_RELFLG))
    abfd->flags |= HAS_RELOC;
  if ((filehdr->f_flags & F_EXEC))
    abfd->flags |= EXEC_P;
  if (!(filehdr->f_flags & F_LNNO))
    abfd->flags |= HAS_LINENO;
  if (!(filehdr->f_flags & F_LSYMS))
    abfd->flags |= HAS_LOCALS;

  abfd->tdata = (PTR) tdata;
  bfd_get_symcount(abfd) = filehdr->f_nsyms;
  if (filehdr->f_nsyms)
    abfd->flags |= HAS_SYMS;

  tdata->sym_filepos = filehdr->f_symptr;
  tdata->hdr = (struct aouthdr *) (file_info + sizeof(struct filehdr));

  swap_aouthdr(abfd, tdata->hdr);

  tdata->symbols = (coff_symbol_type *) NULL;
  bfd_get_start_address(abfd) = opthdr ? exec_hdr(abfd)->entry : 0;

  return abfd->xvec;
}


/* 
Takes a bfd and a symbol, returns a pointer to the coff specific area
of the symbol if there is one.
*/
static coff_symbol_type *
coff_symbol_from(abfd, symbol)
    bfd            *abfd;
    asymbol        *symbol;
{
  if (symbol->the_bfd->xvec->flavour != bfd_target_coff_flavour_enum) 
    return (coff_symbol_type *)NULL;

  if (symbol->the_bfd->tdata == (PTR)NULL)
    return (coff_symbol_type *)NULL;

  return  (coff_symbol_type *) symbol;
}



static bfd_target *
coff_object_p(abfd)
    bfd            *abfd;
{
    unsigned short  magic,
                    nscns,
                    opthdr;
    bfd_error = system_call_error;

    /* figure out how much to read */
    if (bfd_read((PTR) &magic, 1, sizeof(magic), abfd) != sizeof(magic))
	return 0;

    magic = bfd_h_getshort(abfd, (bfd_byte *) (&magic));

    if (BADMAG(*((struct filehdr *) & magic))) {
	bfd_error = wrong_format;
	return 0;
    }
    if (bfd_read((PTR) &nscns, 1, sizeof(nscns), abfd) != sizeof(nscns))
	return 0;
    nscns = bfd_h_getshort(abfd, (unsigned char *) &nscns);
    if (bfd_seek(abfd, (file_ptr) & (((struct filehdr *) NULL)->f_opthdr), SEEK_SET)
	< 0)
	return (bfd_target *) NULL;
    if (bfd_read((PTR) &opthdr, 1, sizeof(opthdr), abfd) != sizeof(opthdr))
	return (bfd_target *) NULL;
    opthdr = bfd_h_getshort(abfd, (unsigned char *) &opthdr);

    return coff_real_object_p(abfd, nscns, opthdr);
}

static          boolean
coff_mkobject(abfd)
    bfd            *abfd;
{
    char           *rawptr;

    bfd_error = system_call_error;

    /* Use an intermediate variable for clarity */
    rawptr = zalloc(sizeof(struct icofdata) + sizeof(AOUTHDR));
    if (rawptr == NULL) {
	bfd_error = no_memory;
	return false;
    }
    abfd->tdata = (PTR) ((struct icofdata *) rawptr);
    exec_hdr(abfd) = (AOUTHDR *) (rawptr + sizeof(struct icofdata));
    obj_relocbase(abfd) =0;
    return true;
}



static void 
coff_count_linenumbers(abfd)
    bfd            *abfd;
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
uses_x_sym_x_tagndx_p(native)
SYMENT *native;
{
    if (BTYPE(native->n_type) == T_STRUCT) return true;
    if (BTYPE(native->n_type) == T_UNION) return true;
    if (BTYPE(native->n_type) == T_ENUM)return true;
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
applied so we can dig it out through a pointer.


*/
static void 
coff_mangle_symbols(bfd_ptr)
bfd *bfd_ptr;
{
  unsigned int symbol_count = bfd_get_symcount(bfd_ptr);
  asymbol **symbol_ptr_ptr = bfd_ptr->outsymbols;
  SYMENT *last_tagndx = (SYMENT *)NULL;
  SYMENT *last_file = (SYMENT *)NULL;
  SYMENT *last_fcn = (SYMENT *)NULL;
  SYMENT *block_stack[50];
  SYMENT **last_block = &block_stack[0];
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
      SYMENT *syment = coff_symbol_ptr->native;
      if (syment == (SYMENT *)NULL) {
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
	  syment->n_scnum =
	    coff_symbol_ptr->symbol.section->output_section->index+1;
	  syment->n_value = 
	    coff_symbol_ptr->symbol.value +
	      coff_symbol_ptr->symbol.section->output_offset +
		coff_symbol_ptr->symbol.section->output_section->vma;
	}


	/* If this symbol ties up something then do it */

	if (syment->n_sclass == C_FILE && last_file != (SYMENT *)NULL)
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
		 && last_fcn != (SYMENT *)NULL) 
	  {
	    AUXENT *auxent = (AUXENT *)(last_fcn+1);
	    auxent->x_sym.x_fcnary.x_fcn.x_endndx = native_index;
	    last_fcn = (SYMENT *)NULL;

	  }
	else if (syment->n_sclass == C_EOS && last_tagndx != (SYMENT*)NULL)
	  {
	    AUXENT *auxent = (AUXENT *)(last_tagndx+1);
	    /* Remember that we keep the native index in the offset 
	       so patch the beginning of the struct to point to this
	       */
	    auxent->x_sym.x_tagndx = last_tagndx->n_offset;
	    auxent->x_sym.x_fcnary.x_fcn.x_endndx =
	      native_index + syment->n_numaux + 1 ;
	    /* Now point the eos to the structure */
	    auxent = (AUXENT *)(syment+1);
	    auxent->x_sym.x_tagndx = last_tagndx->n_offset;
	      
	    
	  }
	else if (syment->n_sclass == C_BLOCK 
		 && coff_symbol_ptr->symbol.name[1] == 'e') 
	  {
	    AUXENT *auxent = (AUXENT *)((*(--last_block))+1);
	    auxent->x_sym.x_fcnary.x_fcn.x_endndx = 
	      native_index + syment->n_numaux + 1;
	  }
	if (syment->n_sclass == C_EXT 
	    && !ISFCN(syment->n_type) 
	    && first_time == true 
	    && last_file != (SYMENT *)NULL) {
	  /* This is the first external symbol seen which isn't a 
	     function place it in the last .file entry */
	  last_file->n_value = native_index;
	  first_time = false;
	}
#ifdef C_LEAFPROC
	if (syment->n_sclass == C_LEAFPROC && syment->n_numaux == 2) {
	  AUXENT *auxent = (AUXENT *)(syment+2);
	  /* This is the definition of a leaf proc, we'll relocate the 
	  address */

	  auxent->x_bal.x_balntry +=
	      coff_symbol_ptr->symbol.section->output_offset + 
	    coff_symbol_ptr->symbol.section->output_section->vma ;
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
	  if (uses_x_sym_x_tagndx_p(syment)) {
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
	      AUXENT *auxent = (AUXENT *)(syment+1);
	      bfd *bfd_ptr = coff_symbol_ptr->symbol.the_bfd;
	      SYMENT *base = obj_raw_syments(bfd_ptr);	      
	      auxent->x_sym.x_tagndx = base[auxent->x_sym.x_tagndx].n_offset;

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
	syment->n_offset = native_index;
	native_index = native_index + 1 + syment->n_numaux;
      }
    }
  }
}
	  
	  
	  

	  
	  

static void 
coff_write_symbols(abfd)
    bfd            *abfd;
{
  unsigned int    i;
  unsigned int    limit = bfd_get_symcount(abfd);
  unsigned int    written = 0;
  SYMENT          dummy;
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
    SYMENT         *native;
    if (c_symbol == (coff_symbol_type *) NULL ||
	c_symbol->native == (SYMENT *) NULL) {
      /*
	 This symbol has been created by the loader, or come from a non
	 coff format. It  has no native element to inherit, make our
	 own
	 */

      native = &dummy;
      native->n_type = T_NULL;
#ifdef I960
      native->n_flags = 0;
#endif
      if (symbol->flags & BSF_ABSOLUTE) {
	native->n_scnum = N_ABS;
	native->n_value = symbol->value;
      }
      else if (symbol->flags & (BSF_UNDEFINED | BSF_FORT_COMM)) {
	native->n_scnum = N_UNDEF;
	native->n_value = symbol->value;
      }
      else if (symbol->flags & BSF_DEBUGGING) {
	/*
	   remove name so it doesn't take up any space
	   */
	symbol->name = "";
#if 0				/* FIXME -- Steve hasn't decided what to do
				   with these */
	/*
	   Don't do anything with debugs from the loader
	   */
	native->n_scnum = N_DEBUG;
#endif
	continue;
      }
      else {
	native->n_scnum = symbol->section->output_section->index + 1;
	native->n_value = symbol->value +
	  symbol->section->output_section->vma +
	    symbol->section->output_offset;
#ifdef I960
	/* Copy the any flags from the the file hdr into the symbol  */
	{
	  coff_symbol_type *c = coff_symbol_from(abfd, symbol);
	  if (c != (coff_symbol_type *)NULL) {
	    native->n_flags = c->symbol.the_bfd->flags;
	  }
	}
#endif
	}



#ifdef HASPAD1
      native->pad1[0] = 0;
      native->pad1[0] = 0;
#endif

      native->pad2[0] = 0;
      native->pad2[1] = 0;

      native->n_type = 0;
      native->n_sclass = C_EXT;
      native->n_numaux = 0;
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
	    union auxent   *a = (union auxent *) (native + 1);
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
	    count * sizeof(struct lineno);

	}
      }	/* if symbol new to coff */

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
	strncpy(native->n_name, symbol->name, SYMNMLEN);
      }
      else {
	native->n_offset = string_size + 4;
	native->n_zeroes = 0;
	string_size += name_length + 1;
      }
      {
	unsigned int    numaux = native->n_numaux;
	int             type = native->n_type;
	int             class = native->n_sclass;
	bfd_coff_swap_sym(abfd, native);
	bfd_write((PTR) native, 1, SYMESZ, abfd);
	for (j = 0; j != native->n_numaux; j++) {
	  bfd_coff_swap_aux(abfd, native + j + 1, type, class);
	  bfd_write((PTR) (native + j + 1), 1, AUXESZ, abfd);

	}
	/*
	   Reuse somewhere in the symbol to keep the index
	   */
	set_index(symbol, written);
	written += 1 + numaux;
      }
    }
  } /* for each out symbol */

  bfd_get_symcount(abfd) = written;
  /* Now write out strings */

  if (string_size) {
    unsigned int    size = string_size + 4;
    bfd_h_put_x(abfd, size, &size);
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
    bfd_h_put_x(abfd, size, &size);
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
	arelent       **p = s->orelocation;
	bfd_seek(abfd, s->rel_filepos, SEEK_SET);
	for (i = 0; i < s->reloc_count; i++) {
	    struct reloc    n;
	    arelent        *q = p[i];
	    memset((PTR)&n, 0, sizeof(n));
	    n.r_vaddr = q->address + s->vma;
	    n.r_symndx = get_index((*(q->sym_ptr_ptr)));
	    n.r_type = q->howto->type;
	    swap_reloc(abfd, &n);
	    bfd_write((PTR) &n, 1, RELSZ, abfd);
	}
    }
}
static void 
coff_write_linenumbers(abfd)
    bfd            *abfd;
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
		    struct lineno   out;
		    bzero( (PTR)&out, sizeof(out));
		    out.l_lnno = 0;
		    out.l_addr.l_symndx = l->u.offset;
		    bfd_coff_swap_lineno(abfd, &out);
		    bfd_write((PTR) &out, 1, LINESZ, abfd);
		    l++;
		    while (l->line_number) {
			out.l_lnno = l->line_number;
			out.l_addr.l_symndx = l->u.offset;
			bfd_coff_swap_lineno(abfd, &out);
			bfd_write((PTR) &out, 1, LINESZ, abfd);
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
    coff_symbol_type *new = (coff_symbol_type *) malloc(sizeof(coff_symbol_type));
    if (new == NULL) {
	bfd_error = no_memory;
	return (NULL);
    }				/* on error */
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
	    char           *section_name = symbol->section == (asection *) NULL ?
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
#ifdef MC68MAGIC
    case bfd_arch_m68k:
	*magicp = MC68MAGIC;
	return true;
#endif
#ifdef M88MAGIC
    case bfd_arch_m88k:
	*magicp = MC88MAGIC;
	return true;
	break;
#endif

    default:			/* Unknown architecture */
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
    return true;		/* We're easy ... */
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

    for (current = abfd->sections; current != NULL; current = current->next) {
	current->filepos = sofar;
	/* Only add sections which have contents */
	if (current->flags & SEC_HAS_CONTENTS)
	    sofar += current->size;
    }
    obj_relocbase(abfd) = sofar;
}




/* SUPPRESS 558 */
/* SUPPRESS 529 */
static          boolean
coff_write_object_contents(abfd)
    bfd            *abfd;
{
  struct filehdr  file_header;
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
  unsigned        magic,
  flags;
  bfd_error = system_call_error;


  if(abfd->output_has_begun == false) {
    coff_compute_section_file_positions(abfd);
  }
  scn_base = (file_ptr) (sizeof(struct filehdr)
			 + ((abfd->flags & EXEC_P) ? sizeof(AOUTHDR) : 0));

  if (bfd_seek(abfd, scn_base, SEEK_SET) != 0)
    return false;
  reloc_base = obj_relocbase(abfd);


      


  /*
     Make a pass through the symbol table to count line number entries and
     put them into the correct asections
     */
  coff_count_linenumbers(abfd);
  data_base = scn_base;
  /* Work out the size of the reloc and linno areas */

  for (current = abfd->sections; current != NULL; current = current->next) {
    reloc_size += current->reloc_count * sizeof(struct reloc);
    lnno_size += current->lineno_count * sizeof(struct lineno);
    data_base += sizeof(struct scnhdr);
  }


  lineno_base = reloc_base + reloc_size;
  sym_base = lineno_base + lnno_size;

  /* Indicate in each section->line_filepos its actual file address */
  for (current = abfd->sections; current != NULL; current = current->next) {
    if (current->lineno_count) {
      current->line_filepos = lineno_base;
      current->moving_line_filepos = lineno_base;
      lineno_base += current->lineno_count * sizeof(struct lineno);

    }
    else {
      current->line_filepos = 0;
    }
    if (current->reloc_count) {
      current->rel_filepos = reloc_base;
      reloc_base += current->reloc_count * sizeof(struct reloc);
    }
    else {
      current->rel_filepos = 0;
    }
  }



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
      SCNHDR          section;
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
	section.s_flags = STYP_TEXT; /* kind stupid optimisation */
      }
      else {

	if (!strcmp(current->name, _DATA)) {
	  data_sec = current;
	  section.s_flags = STYP_DATA; /* kind stupid
					  optimisation */
	}
	else if (!strcmp(current->name, _BSS)) {
	  bss_sec = current;
	  section.s_flags = STYP_BSS; /* kind stupid optimisation */
	}
      }


#ifdef I960
      section.s_align = (current->alignment_power
			 ? 1 << current->alignment_power
			 : 0);

#endif
      swap_scnhdr(abfd, &section);
      bfd_write((PTR) (&section), 1, SCNHSZ, abfd);
      pad = 0;
    }

  }
  /* OK, now set up the filehdr... */

  bfd_h_put_x(abfd, abfd->section_count, &file_header.f_nscns);
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
  bfd_h_put_x(abfd, time(0), &file_header.f_timdat);
#else
  bfd_h_put_x(abfd, 0, &file_header.f_timdat);
#endif

  if (bfd_get_symcount(abfd) != 0)
    bfd_h_put_x(abfd, sym_base, &file_header.f_symptr);
  else
    bfd_h_put_x(abfd, 0, &file_header.f_symptr);

  file_header.f_flags = 0;

  if (abfd->flags & EXEC_P)
    bfd_h_put_x(abfd, sizeof(AOUTHDR), &file_header.f_opthdr);
  else
    bfd_h_put_x(abfd, 0, &file_header.f_opthdr);

  if (!hasrelocs)
    file_header.f_flags |= F_RELFLG;
  if (!haslinno)
    file_header.f_flags |= F_LNNO;
  if (0 == bfd_get_symcount(abfd))
    file_header.f_flags |= F_LSYMS;
  if (abfd->flags & EXEC_P)
    file_header.f_flags |= F_EXEC;
  if (!abfd->xvec->byteorder_big_p)
    file_header.f_flags |= F_AR32WR;
  /*
     FIXME, should do something about the other byte orders and
     architectures.
     */

  /* Set up architecture-dependent stuff */

  magic = 0;
  flags = 0;
  coff_set_flags(abfd, &magic, &flags);
  file_header.f_flags |= flags;

  bfd_h_put_x(abfd, magic, &file_header.f_magic);
  bfd_h_put_x(abfd, file_header.f_flags, &file_header.f_flags);

  /* ...and the "opt"hdr... */
#ifdef I960
  bfd_h_put_x(abfd, (magic == I960ROMAGIC ? NMAGIC : OMAGIC),
	      &(exec_hdr(abfd)->magic));
#endif
#if M88
  exec_hdr(abfd)->magic = PAGEMAGIC3;
#endif

  /* Now should write relocs, strings, syms */
  obj_sym_filepos(abfd) = sym_base;

  if (bfd_get_symcount(abfd) != 0) {
    coff_mangle_symbols(abfd);
    coff_write_symbols(abfd);
    coff_write_linenumbers(abfd);
    coff_write_relocs(abfd);
  }
  if (text_sec) {
    bfd_h_put_x(abfd, text_sec->size, &exec_hdr(abfd)->tsize);
    bfd_h_put_x(abfd,
		text_sec->size ? text_sec->vma : 0,
		&exec_hdr(abfd)->text_start);
  }
  if (data_sec) {
    bfd_h_put_x(abfd, data_sec->size, &exec_hdr(abfd)->dsize);
    bfd_h_put_x(abfd,
		data_sec->size ? data_sec->vma : 0,
		&exec_hdr(abfd)->data_start);
  }
  if (bss_sec) {
    bfd_h_put_x(abfd, bss_sec->size, &exec_hdr(abfd)->bsize);
  }
  bfd_h_put_x(abfd, bfd_get_start_address(abfd), &exec_hdr(abfd)->entry);


  /* now write them */
  bfd_h_put_x(abfd, bfd_get_symcount(abfd), &file_header.f_nsyms);
  if (bfd_seek(abfd, 0L, SEEK_SET) != 0)
    return false;

  bfd_write((PTR) &file_header, 1, FILHSZ, abfd);

  if (abfd->flags & EXEC_P) {

    bfd_write((PTR) exec_hdr(abfd), 1, AOUTSZ, abfd);
  }
  return true;
}



static          boolean
coff_set_section_contents(abfd, section, location, offset, count)
    bfd            *abfd;
    sec_ptr         section;
    PTR location;
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
static          boolean
coff_get_section_contents(abfd, section, location, offset, count)
    bfd            *abfd;
    sec_ptr         section;
    PTR location;
    file_ptr        offset;
    int             count;
{
    if (count == 0
	|| offset >= section->size
	|| bfd_seek(abfd, section->filepos + offset, SEEK_SET) == -1
	|| bfd_read(location, 1, count, abfd) != count) {
	return (false);
    }				/* on error */
    return (true);
}				/* coff_get_section_contents() */


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

#define cleaner(ptr) if (abfd->ptr) free (abfd->ptr)
  cleaner(tdata);

  if (abfd->my_archive)
    cleaner(filename);

#undef cleaner
  return true;
}






static PTR 
buy_and_read(abfd, where, seek_direction, size)
    bfd            *abfd;
    file_ptr        where;
    int             seek_direction;
    size_t          size;
{
  PTR area = (PTR) malloc(size);
  if (!area) {
    bfd_error = no_memory;
    return (NULL);
  }
  bfd_seek(abfd, where, seek_direction);
  if (bfd_read(area, 1, size, abfd) != size) {
    bfd_error = system_call_error;
    free(area);
    return (NULL);
  }				/* on error */
  return (area);
}				/* buy_and_read() */

static void 
offset_symbol_indices(symtab, count, offset)
    SYMENT         *symtab;
    unsigned long   count;
    long            offset;
{
    SYMENT         *end = symtab + count;
    for (; symtab < end; ++symtab) {
	if (symtab->n_sclass == C_FILE) {
	    symtab->n_value = 0;
	}
	else if (symtab->n_sclass == C_ALIAS) {
	    /*
	       These guys have indices in their values.
	    */
	    symtab->n_value += offset;

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

		((AUXENT *) (symtab + 1))->x_sym.x_tagndx += offset;

	    }			/* These guys have a tagndx */
	    if (symtab->n_sclass == C_STRTAG
		|| symtab->n_sclass == C_UNTAG
		|| symtab->n_sclass == C_ENTAG
		|| symtab->n_sclass == C_BLOCK
		|| symtab->n_sclass == C_FCN
		|| ISFCN(symtab->n_type)) {

		((AUXENT *) (symtab + 1))->x_sym.x_fcnary.x_fcn.x_endndx += offset;

	    }			/* These guys have an endndx */
#ifndef I960
	    if (ISFCN(symtab->n_type)) {
		((AUXENT *) (symtab + 1))->x_sym.x_tvndx += offset;
	    }			/* These guys have a tvndx.  I think...
				   (FIXME) */
#endif				/* Not I960 */

	}			/* if value, else if aux */
	symtab += symtab->n_numaux;
    }				/* walk the symtab */

    return;
}				/* offset_symbol_indices() */
/* swap the entire symbol table */
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
			      raw_symtab + 1,
			      raw_symtab->n_type,
			      raw_symtab->n_sclass);
	}			/* swap all the aux entries */
    }				/* walk the symbol table */

    return;
}				/* swap_raw_symtab() */
/*
   read a symbol table into freshly mallocated memory, swap it, and knit the
   symbol names into a normalized form. By normalized here I mean that all
   symbols have an n_offset pointer that points to a NULL terminated string.
   Oh, and the first symbol MUST be a C_FILE.  If there wasn't one there
   before, put one there.
*/

static SYMENT  *
get_normalized_symtab(abfd)
    bfd            *abfd;
{
    SYMENT         *end;
    SYMENT         *retval;
    SYMENT         *s;
    char           *string_table = NULL;
    unsigned long   size;
    unsigned long   string_table_size = 0;
    /*
       I used to SEEK_END here to read the symtab and string table all at
       once.  This fails if this bfd is really an archive element.  Thus, the
       somewhat convoluted approach to reading in the string table. xoxorich.
    */
    /*
       $if ((bfd_seek(abfd, 0, SEEK_END) == -1) || ((end_of_file =
       bfd_tell(abfd)) == -1) || (bfd_seek(abfd, obj_sym_filepos(abfd),
       SEEK_SET) == -1) || ((beginning_of_symtab = bfd_tell(abfd)) == -1)) {
    
    bfd_error = system_call_error; return(NULL); }$
    *//* on error */

    /* $if ((size = end_of_file - beginning_of_symtab) == 0) {$ */

    if ((size = bfd_get_symcount(abfd) * sizeof(SYMENT)) == 0) {
	bfd_error = no_symbols;
	return (NULL);
    }				/* no symbols */
    /*
       This is a hack.  Some tool chains fail to put a C_FILE symbol at the
       beginning of the symbol table.  To make life simpler for our users, we
       inject one if it wasn't there originally.
    
    We'd like to keep all of this bfd's native symbols in one block to keep
       table traversals simple.  To do that, we need to know whether we will
       be prepending the C_FILE symbol before we read the rest of the table.
    */
    if ((s = (SYMENT *) malloc(sizeof(SYMENT) * 2)) == NULL) {
	bfd_error = no_memory;
	return (NULL);
    }				/* on error */
    if (bfd_seek(abfd, obj_sym_filepos(abfd), SEEK_SET) == -1
	|| bfd_read(s, sizeof(SYMENT), 1, abfd) != sizeof(SYMENT)) {
	bfd_error = system_call_error;
	return (NULL);
    }				/* on error */
    bfd_coff_swap_sym(abfd, s);

    if (s->n_sclass == C_FILE) {
	obj_symbol_slew(abfd) = 0;

	if ((retval = (SYMENT *) malloc(size)) == NULL) {
	    bfd_error = no_memory;
	    return (NULL);
	}			/* on malloc error */
    }
    else {
	unsigned long   namelength = 0;
	char           *filename;
	obj_symbol_slew(abfd) = 2;

	if ((retval = (SYMENT *) malloc(size
					+ (obj_symbol_slew(abfd)
					   * sizeof(SYMENT)))) == NULL) {
	    bfd_error = no_memory;
	    return (NULL);
	}			/* on malloc error */
	bzero((char *) retval, size + (obj_symbol_slew(abfd) * sizeof(SYMENT)));

#define FILE_ENTRY_NAME	".file"

	if ((retval->n_offset = (int) malloc(strlen(FILE_ENTRY_NAME)
					     + 1)) == NULL) {
	    bfd_error = no_memory;
	    return (NULL);
	}			/* on malloc error */
	strcpy((char *) retval->n_offset, FILE_ENTRY_NAME);
	retval->n_sclass = C_FILE;
	retval->n_scnum = N_DEBUG;
	retval->n_numaux = 1;

#undef FILE_ENTRY_NAME

	if ((filename = bfd_get_filename(abfd)) == NULL) {
	    filename = "fake";
	}			/* if we know it's name */
	if ((namelength = strlen(filename)) <= FILNMLEN) {
	    strncpy(((AUXENT *) (retval + 1))->x_file.x_fname, filename, FILNMLEN);
	}
	else {
	    if ((((AUXENT *) (retval + 1))->x_file.x_n.x_offset
		 = (int) malloc(namelength+1)) == NULL) {
		bfd_error = no_memory;
		return (NULL);
	    }			/* on error */
	    strcpy((char *) (((AUXENT *) (retval + 1))->x_file.x_n.x_offset),
		   filename);

	}			/* if "short" name */
    }				/* missing file entry. */

    free(s);

    if (bfd_seek(abfd, obj_sym_filepos(abfd), SEEK_SET) == -1
	|| bfd_read(retval + obj_symbol_slew(abfd), size, 1, abfd) != size) {
	bfd_error = system_call_error;
	return (NULL);
    }				/* on read error */
    /* mark the end of the symbols */
    end = retval + obj_symbol_slew(abfd) + bfd_get_symcount(abfd);
    /*
       FIXME SOMEDAY.  A string table size of zero is very weird, but
       probably possible.  If one shows up, it will probably kill us.
    */

    swap_raw_symtab(abfd, retval + obj_symbol_slew(abfd));

    /* ...and normalize symbol names. */

    for (s = retval + obj_symbol_slew(abfd); s < end; ++s) {
	if (s->n_zeroes != 0) {
	    /*
	       This is a "short" name.  Make it long.
	    */
	    unsigned long   i = 0;
	    char           *newstring = NULL;
	    /*
	       find the length of this string without walking into memory
	       that isn't ours.
	    */

	    for (i = 0; i <= 8; ++i) {
		if (s->n_name[i] == '\0') {
		    break;
		}		/* if end of string */
	    }			/* possible lengths of this string. */

	    if ((newstring = malloc(++i)) == NULL) {
		bfd_error = no_memory;
		return (NULL);
	    }			/* on error */
	    bzero(newstring, i);
	    strncpy(newstring, s->n_name, i -1 );
	    s->n_offset = (int) newstring;
	    s->n_zeroes = 0;

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

		if (bfd_read((char *) &string_table_size, sizeof(string_table_size), 1, abfd) != sizeof(string_table_size)) {
		    bfd_error = system_call_error;
		    return (NULL);
		}		/* on error */
		sp(string_table_size);

		if ((string_table = malloc(string_table_size -= 4)) == NULL) {
		    bfd_error = no_memory;
		    return (NULL);
		}		/* on mallocation error */
		if (bfd_read(string_table, string_table_size, 1, abfd) != string_table_size) {
		    bfd_error = system_call_error;
		    return (NULL);
		}		/* on error */
	    }			/* have not yet read the string table. */
	    /*
	       This is a long name already. Just point it at the string in
	       memory.
	    */
	    s->n_offset = (int) (string_table - 4 + s->n_offset);
	}			/* switch on type of symbol name */

	s += s->n_numaux;
    }				/* for each symbol */
    /*
       If we had to insert a C_FILE symbol, then everybody's indices are off
       by 2, so correct them.
    */

    if (obj_symbol_slew(abfd) > 0) {
	offset_symbol_indices(retval + 2, bfd_get_symcount(abfd), 2);

	/* and let the world know there are two more of them. */
	bfd_get_symcount(abfd) += 2;
    }				/* if we added a C_FILE */
    obj_raw_syments(abfd) = retval;
    obj_string_table(abfd) = string_table;

    return (retval);
}				/* get_normalized_symtab() */

static
struct sec_struct *
section_from_bfd_index(abfd, index)
    bfd            *abfd;
    int             index;
{
    if (index > 0) {
	struct sec_struct *answer = abfd->sections;
	while (--index) {
	    answer = answer->next;
	}
	return answer;
    }
    return 0;
}

static int
coff_get_symcount_upper_bound(ignore_abfd)
    bfd            *ignore_abfd;
{
    BFD_ASSERT(0);
    return 0;
}

static          symindex
coff_get_first_symbol(ignore_abfd)
    bfd            *ignore_abfd;
{
    return 0;
}

static          symindex
coff_get_next_symbol(abfd, oidx)
    bfd            *abfd;
    symindex        oidx;
{
    if (oidx == BFD_NO_MORE_SYMBOLS)
	return BFD_NO_MORE_SYMBOLS;
    return ++oidx >= bfd_get_symcount(abfd) ? BFD_NO_MORE_SYMBOLS : oidx;
}

static char    *
coff_symbol_name(abfd, idx)
    bfd            *abfd;
    symindex        idx;
{
    return (obj_symbols(abfd) + idx)->symbol.name;
}

static long
coff_symbol_value(abfd, idx)
    bfd            *abfd;
    symindex        idx;
{
    return (obj_symbols(abfd) + idx)->symbol.value;
}

static          symclass
coff_classify_symbol(abfd, idx)
    bfd            *abfd;
    symindex        idx;
{
    coff_symbol_type *sym = obj_symbols(abfd) + idx;
    if ((sym->symbol.flags & BSF_FORT_COMM) != 0)
	return bfd_symclass_fcommon;
    if ((sym->symbol.flags & BSF_GLOBAL) != 0)
	return bfd_symclass_global;
    if ((sym->symbol.flags & BSF_DEBUGGING) != 0)
	return bfd_symclass_debugger;
    if ((sym->symbol.flags & BSF_UNDEFINED) != 0)
	return bfd_symclass_undefined;

    return bfd_symclass_unknown;
}

static          boolean
coff_symbol_hasclass(abfd, idx, class)
    bfd            *abfd;
    symindex        idx;
    symclass        class;
{

    coff_symbol_type *sym = obj_symbols(abfd) + idx;
    switch (class) {
    case bfd_symclass_fcommon:
	return (sym->symbol.flags & BSF_FORT_COMM) ? true : false;
    case bfd_symclass_global:
	return (sym->symbol.flags & BSF_GLOBAL) ? true : false;
    case bfd_symclass_debugger:
	return (sym->symbol.flags & BSF_DEBUGGING) ? true : false;;
    case bfd_symclass_undefined:
	return (sym->symbol.flags & BSF_UNDEFINED) ? true : false;;
    default:
	return false;
    }

}




static
                boolean
coff_slurp_line_table(abfd, asect)
    bfd            *abfd;
    asection       *asect;
{
    struct lineno  *native_lineno;
    alent          *lineno_cache;
    BFD_ASSERT(asect->lineno == (alent *) NULL);


    native_lineno = (struct lineno *) buy_and_read(abfd,
						   asect->line_filepos,
						   SEEK_SET,
					   (size_t) (sizeof(struct lineno) *
						     asect->lineno_count));
    lineno_cache =
	(alent *) malloc((size_t) ((asect->lineno_count + 1) * sizeof(alent)));
    if (lineno_cache == NULL) {
	bfd_error = no_memory;
	return (BFD_FAILURE);
    } {				/* on error */
	unsigned int    counter = 0;
	alent          *cache_ptr = lineno_cache;
	struct lineno  *src = native_lineno;
	while (counter < asect->lineno_count) {
	    bfd_coff_swap_lineno(abfd, src);
	    cache_ptr->line_number = src->l_lnno;

	    if (cache_ptr->line_number == 0) {
		coff_symbol_type *sym =
		(coff_symbol_type *) (src->l_addr.l_symndx
				      + obj_symbol_slew(abfd)
				      + obj_raw_syments(abfd))->n_zeroes;
		cache_ptr->u.sym = (asymbol *) sym;
		sym->lineno = cache_ptr;
	    }
	    else {
		cache_ptr->u.offset = src->l_addr.l_paddr
		    - bfd_section_vma(abfd, asect);
	    }			/* If no linenumber expect a symbol index */

	    cache_ptr++;
	    src++;
	    counter++;
	}
	cache_ptr->line_number = 0;

    }
    free(native_lineno);
    asect->lineno = lineno_cache;
    return true;
}				/* coff_slurp_line_table() */

static SYMENT  *
find_next_file_symbol(current, end)
    SYMENT         *current;
    SYMENT         *end;
{
    /* ignore the first symbol which is probably a C_FILE. */

    current += current->n_numaux + 1;

    for (; current < end; ++current) {
	if (current->n_sclass == C_FILE) {
	    return (current);
	}			/* found one */
	current += current->n_numaux;
    }				/* walk the remaining table */

    /* not found */
    return (end);
}				/* find_next_file_symbol() */
/*
   Note that C_FILE symbols can, and some do, have more than 1 aux entry.
*/

static void 
force_indices_file_symbol_relative(abfd, symtab)
    bfd            *abfd;
    SYMENT         *symtab;
{
    SYMENT         *end = symtab + bfd_get_symcount(abfd);
    SYMENT         *current;
    SYMENT         *next;
    /* the first symbol had damn well better be a C_FILE. */
    BFD_ASSERT(symtab->n_sclass == C_FILE);
    symtab->n_value = 0;

    for (current = find_next_file_symbol(symtab, end);
	 current < end;
	 current = next) {
	offset_symbol_indices(current,
			      ((next =
				find_next_file_symbol(current,
						      end)) - current),
			      symtab - current);
    }				/* walk the table */

    return;
}				/* force_indices_file_symbol_relative() */

static          boolean
coff_slurp_symbol_table(abfd)
    bfd            *abfd;
{
    SYMENT         *native_symbols;
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
    force_indices_file_symbol_relative(abfd, native_symbols);

    /* Allocate enough room for all the symbols in cached form */
    cached_area =
	(coff_symbol_type *)
	malloc((size_t) (bfd_get_symcount(abfd) * sizeof(coff_symbol_type)));

    if (cached_area == NULL) {
	bfd_error = no_memory;
	return (BFD_FAILURE);
    }				/* on error */
    table_ptr =
	(unsigned int *)
	malloc((size_t) (bfd_get_symcount(abfd) * sizeof(unsigned int)));

    if (table_ptr == NULL) {
	bfd_error = no_memory;
	return (BFD_FAILURE);
    } {				/* on error */
	coff_symbol_type *dst = cached_area;
	unsigned int    last_native_index = bfd_get_symcount(abfd);
	unsigned int    this_index = 0;
	while (this_index < last_native_index) {
	    SYMENT         *src = native_symbols + this_index;
	    table_ptr[this_index] = number_of_symbols;
	    dst->symbol.the_bfd = abfd;

	    dst->symbol.name = (char *) src->n_offset;	/* which was normalized
							   to point to a null
							   terminated string. */
	    /*
	       We use the native name field to point to the cached field
	    */
	    src->n_zeroes = (int) dst;
	    dst->symbol.section = section_from_bfd_index(abfd, src->n_scnum);

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
		if (src->n_scnum == 0) {
		    if (src->n_value == 0) {
			dst->symbol.flags = BSF_UNDEFINED;
		    }
		    else {
			dst->symbol.flags = BSF_FORT_COMM;
			dst->symbol.value = src->n_value;
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
		    if (ISFCN(src->n_type)) {
			/*
			   A function ext does not go at the end of a file
			*/
			dst->symbol.flags |= BSF_NOT_AT_END;
		    }
		}

		break;
	    case C_STAT:	/* static			 */
#ifdef I960
	    case C_LEAFSTAT:	/* static leaf procedure        */
#endif
	    case C_LABEL:	/* label			 */
		dst->symbol.flags = BSF_LOCAL;
		/*
		   Base the value as an index from the base of the section
		*/
		dst->symbol.value = src->n_value - dst->symbol.section->vma;
		break;

	    case C_MOS:	/* member of structure	 */
	    case C_EOS:	/* end of structure		 */
	    case C_REGPARM:	/* register parameter		 */
	    case C_REG:	/* register variable		 */
#ifdef C_AUTOARG
	    case C_AUTOARG:	/* 960-specific storage class */
#endif
	    case C_TPDEF:	/* type definition		 */

	    case C_ARG:
	    case C_AUTO:	/* automatic variable */
	    case C_FIELD:	/* bit field */
	    case C_ENTAG:	/* enumeration tag		 */
	    case C_MOE:	/* member of enumeration	 */
	    case C_MOU:	/* member of union		 */
	    case C_UNTAG:	/* union tag			 */

		dst->symbol.flags = BSF_DEBUGGING;
		dst->symbol.value = src->n_value;
		break;

	    case C_FILE:	/* file name			 */
	    case C_STRTAG:	/* structure tag		 */
		dst->symbol.flags = BSF_DEBUGGING;
		dst->symbol.value = src->n_value;

		break;
	    case C_BLOCK:	/* ".bb" or ".eb"		 */
	    case C_FCN:	/* ".bf" or ".ef"		 */
		dst->symbol.flags = BSF_LOCAL;
		/*
		   Base the value as an index from the base of the section
		*/
		dst->symbol.value = src->n_value - dst->symbol.section->vma;

		break;
	    case C_EFCN:	/* physical end of function	 */
	    case C_NULL:
	    case C_EXTDEF:	/* external definition		 */
	    case C_ULABEL:	/* undefined label		 */
	    case C_USTATIC:	/* undefined static		 */
	    case C_LINE:	/* line # reformatted as symbol table entry */
	    case C_ALIAS:	/* duplicate tag		 */
	    case C_HIDDEN:	/* ext symbol in dmert public lib */

	    default:

		printf("SICK%d\n", src->n_sclass);
		abort();
		dst->symbol.flags = BSF_DEBUGGING;
		dst->symbol.value = src->n_value;

		break;
	    }




	    BFD_ASSERT(dst->symbol.flags != 0);

	    dst->native = src;

	    dst->symbol.udata = 0;
	    dst->lineno = (alent *) NULL;
	    this_index += src->n_numaux + 1;
	    dst++;
	    number_of_symbols++;
	}			/* walk the native symtab */

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
coff_slurp_reloc_table(abfd, asect, symbols)
    bfd            *abfd;
    sec_ptr         asect;
    asymbol       **symbols;
{
  struct reloc   *native_relocs;
  arelent        *reloc_cache;
  if (asect->relocation)
    return true;
  if (asect->reloc_count == 0)
    return true;
  if (!coff_slurp_symbol_table(abfd))
    return false;
  native_relocs =
    (struct reloc *) buy_and_read(abfd,
				  asect->rel_filepos,
				  SEEK_SET,
				  (size_t) (sizeof(struct reloc) *
					    asect->reloc_count));
  reloc_cache = (arelent *)
    malloc((size_t) (asect->reloc_count * sizeof(arelent)));

  if (reloc_cache == NULL) {
    bfd_error = no_memory;
    return (BFD_FAILURE);
  } {				/* on error */
    arelent        *cache_ptr;
    struct reloc   *src;
    for (cache_ptr = reloc_cache,
	 src = native_relocs;
	 cache_ptr < reloc_cache + asect->reloc_count;
	 cache_ptr++,
	 src++) {
      asymbol        *ptr;
      swap_reloc(abfd, src);
      src->r_symndx += obj_symbol_slew(abfd);
      cache_ptr->sym_ptr_ptr = symbols + obj_convert(abfd)[src->r_symndx];

      ptr = *(cache_ptr->sym_ptr_ptr);
      cache_ptr->address = src->r_vaddr;
      /*
	 The symbols definitions that we have read in have been
	 relocated as if their sections started at 0. But the offsets
	 refering to the symbols in the raw data have not been
	 modified, so we have to have a negative addend to compensate.
	 */

      if (ptr->the_bfd == abfd && ptr->section != (asection *) NULL) {
	cache_ptr->addend = -(ptr->section->vma + ptr->value);
      }
      else {
	cache_ptr->addend = 0;
      }

      cache_ptr->address -= asect->vma;

      cache_ptr->section = (asection *) NULL;

#if I960
      cache_ptr->howto = howto_table + src->r_type;
#endif
#if M88
      if (src->r_type >= R_PCR16L && src->r_type <= R_VRT32) {
	cache_ptr->howto = howto_table + src->r_type - R_PCR16L;
      }
      else {
	BFD_ASSERT(0);
      }
#endif
#if M68
      cache_ptr->howto = howto_table + src->r_type;
#endif

    }

  }

  free(native_relocs);
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
coff_find_nearest_line(abfd,
		       section,
		       symbols,
		       offset,
		       filename_ptr,
		       functionname_ptr,
		       line_ptr)
    bfd            *abfd;
    asection       *section;
    asymbol       **symbols;
    bfd_vma         offset;
    char          **filename_ptr;
    char          **functionname_ptr;
    unsigned int   *line_ptr;
{
  static bfd     *cache_abfd;
  static asection *cache_section;
  static bfd_vma  cache_offset;
  static unsigned int cache_i;
  static alent   *cache_l;

  unsigned int    i = 0;
  struct icofdata *cof = obj_icof(abfd);
  /* Run through the raw syments if available */
  SYMENT         *p;
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
    p += 1 + p->n_numaux;
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
	struct syment  *s = coff->native;
	s = s + 1 + s->n_numaux;
	/*
	   S should now point to the .bf of the function
	   */
	if (s->n_numaux) {
	  /*
	     The linenumber is stored in the auxent
	     */
	  union auxent   *a = (union auxent *) (s + 1);
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
