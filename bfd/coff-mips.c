/* MIPS Extended-Coff handler for Binary File Diddling.
   Written by Per Bothner.

   FIXME, Needs Copyleft here.  */

/* This does not compile on anything but a MIPS yet (and I haven't been
   able to test it there either since the latest merge!).  So it stays
   out by default.  */
#ifdef ECOFF_BFD

#define MIPS 1
#include "libbfd.h"
#include "sysdep.h"
#include "libcoff.h"		/* to allow easier abstraction-breaking */

#include "intel-coff.h"




static reloc_howto_type howto_table[] = 
{
  {0},
  {1},
  {2},
  {3},
  {4},
  {5},
  {6},
  {7},
  {8},
  {9},
  {10},
  {11},
  {12},
  {13},
  {14},
  {15},
  {16},
  {  R_RELLONG, 0, 2, 32, 0, 0, true, true},
  {18},
  {19},
  {20},
  {21},
  {22},
  {23},
  {24},
  {  R_IPRMED, 2, 2,22,1,0, true, true},
  {26},
/* What do we do with this - ? */
#if 1
  {  R_OPTCALL, 0,2,32,0,0, true, true},
#else
  {  R_OPTCALL, 0,3,32,0,0, true, true},
#endif
};


#define ALIGN(this, boundary) \
  ((( (this) + ((boundary) -1)) & (~((boundary)-1))))


/* Support for Motorola 88k bcs coff as well as Intel 960 coff */


#include <stdio.h>
#include <string.h>


/* Align an address by rounding it up to a power of two.  It leaves the
   address unchanged if align == 0 (2^0 = alignment of 1 byte) */
#define	i960_align(addr, align)	\
	( ((addr) + ((1<<(align))-1)) & (-1 << (align)))

#define TAG_SECTION_NAME ".tagbits"

/* Libraries shouldn't be doing this stuff anyway! */
void fatal();
/* void warning(); */


/* initialize a section structure with information
 * peculiar to this particular implementation of coff 
 */

static void
ecoff_new_section_hook(abfd, section)
bfd *abfd;
asection *section;
{

  section->output_file_alignment = DEFAULT_SECTION_ALIGNMENT;
  section->subsection_alignment = section->output_file_alignment;
  if (abfd->flags & D_PAGED)
    {
      /**
	If the output object file is demand paged then the
	text section starts at the filehdr, with the first 
	usefull bit of data at the end of the filehdr+opthdr+
	scnhdrs. Since we don't know how many sections will
	be put into the output file, we have to recalculate
	the section pads after each additional section has
	been created
	**/
      asection *ptr = abfd->sections;
      unsigned int padding = FILHSZ + AOUTSZ +SCNHSZ * abfd->section_count;

      padding = ALIGN(padding, ptr->output_file_alignment);
      while (ptr) 
	{
	  ptr->start_pad = padding;
	  /* Only the first section is padded  */
	  padding = 0;
	  ptr = ptr->next;
	}
    }
  else 
    {

      /* If the object is not demand paged, then all the sections
	 have no padding 
	 */
      section->start_pad = 0;
    }



}
/* actually it makes itself and its children from the file headers */
static boolean
make_a_section_from_file (abfd, hdr)
bfd *abfd;
     struct scnhdr *hdr;

{
  asection *return_section ;

  { char *name = (char *)xmalloc(9); 

    strncpy(name, (char *)&hdr->s_name[0], 8);

    return_section = bfd_make_section(abfd,  name);
    (return_section->name)[8] = 0;
  }

  /* s_paddr is presumed to be = to s_vaddr */
  /* FIXME -- needs to call swapping routines */
#define assign(to, from) return_section->to = hdr->from
  assign (vma, s_vaddr);
  assign (original_vma, s_vaddr);
  assign (size, s_size);
  assign (filepos, s_scnptr);
  assign (rel_filepos, s_relptr);
  assign (reloc_count, s_nreloc);
#ifdef I960
  assign (alignment, s_align);
#endif
  assign (line_filepos, s_lnnoptr);
/*  return_section->linesize =   hdr->s_nlnno * sizeof (struct lineno);*/

#undef assign
  return_section->lineno_count = hdr->s_nlnno;
  return_section->userdata = (void *)NULL;
  return_section->next = (asection *)NULL;
  if ((hdr->s_flags & STYP_TEXT) || (hdr->s_flags & STYP_DATA))
    return_section->flags = (SEC_LOAD | SEC_ALLOC);
  else if (hdr->s_flags & STYP_BSS)
    return_section->flags = SEC_ALLOC;

  if (hdr->s_nreloc != 0) return_section->flags |= SEC_RELOC;

  return true;
}

bfd_target *
ecoff_real_object_p (abfd, nscns, opthdr)
     bfd *abfd;
     unsigned short nscns, opthdr;
{
  struct icofdata *tdata;
  char *file_info;		/* buffer for all the headers */
  long readsize;		/* length of file_info */
  struct filehdr* filehdr;	/* points into file_info */
  struct scnhdr *sections;	/* points into file_info */

  /* OK, now we know the format, read in the filehdr, soi-disant
     "optional header", and all the sections.*/
  readsize = sizeof(struct filehdr) + opthdr + (nscns * sizeof (struct scnhdr));
  file_info = xmalloc (readsize);
  if (file_info == NULL) {
    bfd_error = no_memory;
    return 0;
  }
  if (bfd_seek (abfd, 0, false) < 0) return 0;
  if (bfd_read (file_info, 1, readsize, abfd) != readsize) return 0;
  filehdr = (struct filehdr *) file_info;
  sections = (struct scnhdr *) (file_info + sizeof (struct filehdr) + opthdr);

  /* Now copy data as required; construct all asections etc */
  tdata = (struct icofdata *) xmalloc (sizeof (struct icofdata) +
				      sizeof (AOUTHDR));
  if (tdata == NULL) {
    bfd_error = no_memory;
    return 0;
  }

  if (nscns != 0) 
  {
    unsigned int i;
    for (i = 0; i < nscns; i++) 
      {
	make_a_section_from_file (abfd, sections + i);
      }
  }

#ifdef I960
  /* OK, now make a section for the tagbits if there were any */
#if 0
  {
    AOUTHDR *aouthdr;		/* points into tdata! */
    aouthdr = (AOUTHDR *) (((char *) tdata) + sizeof (struct icofdata));
    if (aouthdr->tagentries != 0) {
      asection *tag_section = (asection *) xmalloc (sizeof (asection));
      if (tag_section == NULL) {
	free (tdata);
	return 0;
      }
      tag_section->size = aouthdr->tagentries * sizeof (TAGBITS);
      tag_section->name = TAG_SECTION_NAME;
      tag_section->filepos = readsize; /* not surprisingly */
      /* Put this one first */
      tag_section->next = abfd->sections;
      abfd->sections = tag_section;
    }
  }
#endif
#endif
abfd->flags |= HAS_RELOC | HAS_LINENO | HAS_LOCALS;


  /* FIXME, the guess should be set by OR-ing info from the sections */
  if ((filehdr->f_flags & F_RELFLG) != F_RELFLG) abfd->flags &= ~HAS_RELOC;
  if ((filehdr->f_flags & F_EXEC) == F_EXEC)     abfd->flags |= EXEC_P;
  if ((filehdr->f_flags & F_LNNO) != F_LNNO)     abfd->flags &= ~HAS_LINENO;
  if ((filehdr->f_flags & F_LSYMS) != F_LSYMS)   abfd->flags &= ~HAS_LOCALS;
  abfd->tdata = tdata;
  bfd_get_symcount (abfd) = filehdr->f_nsyms;
  if (filehdr->f_nsyms) abfd->flags |= HAS_SYMS;
   
  tdata->sym_filepos = filehdr->f_symptr;
  tdata->hdr = (struct aouthdr *)(file_info + sizeof (struct filehdr));
  tdata->symbols = (esymbol *)NULL;
  bfd_get_start_address (abfd) = exec_hdr (abfd)->entry;
  return abfd->xvec;
}

bfd_target *
ecoff_object_p (abfd)
     bfd *abfd;
{
  unsigned short magic, nscns, opthdr;

  bfd_error = no_error;

  /* figure out how much to read */
  if (bfd_read (&magic, 1, sizeof (magic), abfd) != sizeof (magic))
    return 0;

  magic = bfd_h_getshort (abfd, (unsigned char *)&magic);
  if (magic != (abfd->xvec->byteorder_big_p ? 0x160 :  0x162)) {
    bfd_error = wrong_format;
    return 0;
  }
  if (bfd_read (&nscns, 1, sizeof (nscns), abfd) != sizeof (nscns))
    return 0;
  nscns = bfd_h_getshort (abfd, (unsigned char *)&nscns);

  if (bfd_seek (abfd, ((sizeof (long)) * 3), true) < 0) return false;
  if (bfd_read (&opthdr, 1, sizeof (opthdr), abfd) != sizeof (opthdr))
    return 0;
  opthdr = bfd_h_getshort (abfd, (unsigned char *)&opthdr);

  return ecoff_real_object_p (abfd, nscns, opthdr);
}

static boolean
ecoff_mkobject (abfd)
     bfd *abfd;
{
  char *rawptr;


  bfd_error = no_error;

  /* Use an intermediate variable for clarity */
  rawptr =  xmalloc (sizeof (struct icofdata) + sizeof (AOUTHDR));
  if (rawptr == NULL) {
    bfd_error = no_memory;
    return false;
  }

  abfd->tdata = (struct icofdata *) rawptr;
  exec_hdr (abfd) = (AOUTHDR *) (rawptr + sizeof (struct icofdata));

  return true;
}

static void
ecoff_count_linenumbers(abfd)
bfd *abfd;
{
  unsigned int limit = bfd_get_symcount(abfd);
  unsigned int i = 0;
  esymbol **p = (esymbol **)(abfd->outsymbols);
  {
    asection *s = abfd->sections;
    while (s) {
      if (s->lineno_count != 0) {
	fatal("Bad initial state");
      }
      s = s->next;
    }
  }

  while (i < limit) 
    {
      esymbol *q = *p;
      if (q->c.lineno) 
	{
	  /* This symbol has a linenumber, increment the
	   * owning section's linenumber count */
	  alent *l = q->c.lineno;
	  q->c.section->lineno_count++;
	  l++;
	  while (l->line_number) {
	    q->c.section->lineno_count++;
	    l++;
	  }
	}
      p++;
      i++;
    }
}

/*
 run through the internal symbol table and make all the
 pointers and things within the table point to the right places
 */

static void
ecoff_mangle_symbols(abfd)
bfd *abfd;
{
  esymbol **p = (esymbol **)(abfd->outsymbols);
  unsigned int native_index = 0;
  unsigned int last_file_index = 0;
  unsigned int limit = bfd_get_symcount(abfd);
  struct syment *last_file_symbol = (struct syment *)NULL;
  while (limit--) {
    esymbol *q = *p;
    struct syment *native = q->native;
    if(native) {
      /* Alter the native representation */

      native->n_value = q->c.value;
      if (q->c.flags & BSF_FORT_COMM) {
	native->n_scnum = 0;
      }
      else if (q->c.flags & BSF_DEBUGGING) {
	native->n_scnum = -2;
      }
      else if (q->c.flags & BSF_UNDEFINED) {
	native->n_scnum = 0;
      }
      else if (q->c.flags & BSF_ABSOLUTE) {
	native->n_scnum = -1;
      }
      else {
	native->n_scnum = q->c.section->index + 1;
      }
      if (native->n_numaux) 
	{
	  union auxent *a = (union auxent *)(native+1);
	  /* Relocate symbol indexes */
	  if (ISFCN(native->n_type))
	    {
	      a->x_sym.x_fcnary.x_fcn.x_endndx += last_file_index;
	    }
	  else if(native->n_sclass == C_BLOCK && q->c.name[1] == 'b')
	    {
	      a->x_sym.x_fcnary.x_fcn.x_endndx += last_file_index;
	    }

	  else if (native->n_sclass == C_STRTAG) 
	    {
	      a->x_sym.x_fcnary.x_fcn.x_endndx += last_file_index;
	    }
	  else       if (native->n_sclass == C_MOS || native->n_sclass == C_EOS || native->n_sclass == C_MOE) 
	    {
	      a->x_sym.x_tagndx += last_file_index;
	    }
	}


      switch (native->n_sclass) {
      case C_MOS:
      case C_EOS:
      case C_REGPARM:
      case C_REG:
      case 19:			/*C_REGARG:FIXME */
	/* Fix so that they have an absolute section */
	native->n_scnum= -1;
	break;

      case C_FILE:
	/* Chain all the .file symbols together */
	if(last_file_symbol) {
	  last_file_symbol->n_value = native_index;
	}
	last_file_symbol = native;
	last_file_index = native_index;
	break;
      case C_NULL:
      case C_AUTO:
      case C_EXT:
      case C_EXTDEF:
      case C_LABEL:
      case C_ULABEL:
      case C_USTATIC:
      case C_STRTAG:
      case C_FCN:
      case C_BLOCK:
      case C_STAT:
      case C_LEAFPROC:
	break;
      default:
	/* Bogus: This should be returning an error code, not printing
	   something out! */
	/* warning("Unrecognised sclass %d", native->n_sclass); */
	break;
      }
      native_index += 1 + native->n_numaux;
    }
    else {
      native_index++;
    }
    p++;
  }
}
static void
ecoff_write_symbols(abfd)
bfd *abfd;
{
}


void
ecoff_write_linenumbers(abfd)
bfd *abfd;
{
}


asymbol *
ecoff_make_empty_symbol(abfd, n)
bfd *abfd;
unsigned int n;
{
  unsigned int j;
  esymbol *new = (esymbol *)xmalloc(sizeof(esymbol) * n);
  for (j= 0; j < n; j++) {
    new[j].native = 0;
    new[j].c.lineno = (alent *)NULL;
  }
  return (asymbol *)new;
}

/*SUPPRESS 558*/
/*SUPPRESS 529*/
boolean
ecoff_write_object_contents (abfd)
     bfd *abfd;
{
  return false;
}

/* Calculate the file position for each section. */

static void
ecoff_compute_section_file_positions (abfd)
     bfd *abfd;
{
  file_ptr sofar = sizeof (struct filehdr) + sizeof (AOUTHDR);
  asection *current;

  sofar += abfd->section_count * sizeof (struct scnhdr);
  for (current = abfd->sections; current != NULL; current = current->next) {
    sofar = ALIGN(sofar, current->output_file_alignment);
    current->filepos = sofar;
    /* Only add sections which are loadable */
    if (current->flags & SEC_LOAD) sofar += current->size;
#if 0
    if(current->filepos & (current->alignment-1)) {
      sofar += current->alignment - (current->filepos &(current->alignment-1));
      current->filepos = (current->filepos + current->alignment) & -current->alignment;
    }
#endif
  }
  obj_relocbase (abfd) = sofar;
}

boolean
ecoff_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     unsigned char *location;
     file_ptr offset;
      int count;
{
    return false;
}

boolean
ecoff_set_section_linenos (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     unsigned char *location;
     file_ptr offset;
      int count;
{
   return 0;
}


boolean
ecoff_close_and_cleanup (abfd)
     bfd *abfd;
{
  return false;
}





static void *
buy_and_read(abfd, where, relative, size)
bfd *abfd;
int where;
boolean relative;
unsigned int size;
{
  void *area = (void *)xmalloc(size);
  if (!area) {
    bfd_error = no_memory;
    return 0;
  }
  bfd_seek(abfd, where, relative);
  if (bfd_read(area, 1, size, abfd) != size){
    bfd_error = system_call_error;
    free(area);
    return 0;
  }
  return area;
}

static
struct sec_struct *section_from_bfd_index(abfd, index)
bfd *abfd;
int index;
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
ecoff_get_symcount_upper_bound (abfd)
     bfd *abfd;
{
fatal("call to ecoff_get_symcount_upper_bound");
return 0;
}

static symindex
ecoff_get_first_symbol (abfd)
     bfd * abfd;
{
  return 0;
}

static symindex
ecoff_get_next_symbol (abfd, oidx)
     bfd *abfd;
     symindex oidx;
{
  if (oidx == BFD_NO_MORE_SYMBOLS) return BFD_NO_MORE_SYMBOLS;
  return ++oidx >= bfd_get_symcount (abfd) ? BFD_NO_MORE_SYMBOLS : oidx;
}

static char *
ecoff_symbol_name (abfd, idx)
     bfd *abfd;
     symindex idx;
{
  return (obj_symbols (abfd) + idx)->c.name;
}

static long
ecoff_symbol_value (abfd, idx)
     bfd *abfd;
     symindex idx;
{
  return (obj_symbols (abfd) + idx)->c.value;
}

static symclass
ecoff_classify_symbol (abfd, idx)
     bfd *abfd;
     symindex idx;
{
  esymbol *sym = obj_symbols (abfd) + idx;

  if ((sym->c.flags & BSF_FORT_COMM) != 0)   return bfd_symclass_fcommon;
  if ((sym->c.flags & BSF_GLOBAL) != 0)    return bfd_symclass_global;
  if ((sym->c.flags & BSF_DEBUGGING) != 0)  return bfd_symclass_debugger;
  if ((sym->c.flags & BSF_UNDEFINED) != 0) return bfd_symclass_undefined;

  return bfd_symclass_unknown;
}

static boolean
ecoff_symbol_hasclass (abfd, idx, class)
     bfd *abfd;
     symindex idx;
     symclass class;
{

  esymbol *sym = obj_symbols (abfd) + idx;

  switch (class) {

  case bfd_symclass_fcommon:   return (sym->c.flags & BSF_FORT_COMM) != 0;
  case bfd_symclass_global:    return (sym->c.flags & BSF_GLOBAL) != 0;
  case bfd_symclass_debugger:  return (sym->c.flags & BSF_DEBUGGING) != 0;
  case bfd_symclass_undefined: return (sym->c.flags & BSF_UNDEFINED) != 0;

  default: return false;
  }
}




static
boolean
ecoff_slurp_line_table (abfd, asect)
  bfd *abfd;
  asection *asect;
{
  return true;
}

static boolean
ecoff_slurp_symbol_table(abfd)
     bfd *abfd;
{
  struct syment *native_symbols;
  esymbol *cached_area;
  char *string_table = (char *)NULL;
  unsigned int string_table_size;
  unsigned int number_of_symbols = 0;
  if (obj_symbols (abfd)) return true;
  bfd_seek(abfd, obj_sym_filepos(abfd), false);
  /* Read in the symbol table */
  native_symbols =
    (struct syment *)buy_and_read(abfd,
				  obj_sym_filepos(abfd),
				  false,
				  bfd_get_symcount(abfd) * sizeof(struct syment));
  if (!native_symbols) {
    return false;
  }


  /* Allocate enough room for all the symbols in cached form */
  cached_area = (esymbol *)xmalloc(bfd_get_symcount(abfd) * sizeof(esymbol));


  {

    esymbol *dst = cached_area;
    unsigned int last_native_index = bfd_get_symcount(abfd);
    unsigned int this_index = 0;
    while (this_index < last_native_index) 
      {
	struct syment *src = native_symbols + this_index;
	if (src->n_zeroes == 0) {
	  /* This symbol has a name in the string table */
	  /* Which means that we'll have to read it in */

	  /* Fetch the size of the string table which is straight after the
	   * symbol table
	   */
	  if (string_table == (char *)NULL) {
	    if (bfd_read(&string_table_size, sizeof(string_table_size), 1, abfd) !=
		sizeof(string_table_size)) {
	      fatal("Corrupt coff format");
	    }
	    else {
	      /* Read the string table */
	      string_table = 
		(char *)buy_and_read(abfd,0, true,
				     string_table_size - sizeof(string_table_size)) ;

	    }
	  }


	dst->c.name = string_table + src->n_offset - 4;
	}
	else {
	  /* Otherwise we have to buy somewhere for this name */
	  dst->c.name = xmalloc (SYMNMLEN+1);
	  strncpy(dst->c.name, src->n_name, SYMNMLEN);
	  dst->c.name[SYMNMLEN+1] = '\0';	/* Be sure to terminate it */
	}  

	/* We use the native name field to point to the cached field */
	src->n_zeroes = (long)dst;
	dst->c.section = section_from_bfd_index(abfd, src->n_scnum);
	switch (src->n_sclass) 
	  {
#ifdef I960
	  case C_LEAFPROC:
	    dst->c.value = src->n_value - dst->c.section->vma;
	    dst->c.flags = BSF_EXPORT | BSF_GLOBAL;
	    dst->c.flags |= BSF_NOT_AT_END;
	    break;

#endif

	  case C_EXT:
	    if (src->n_scnum == 0) {
	      if (src->n_value == 0) 
		{
		  dst->c.flags =  BSF_UNDEFINED;
		}
	      else {
		dst->c.flags = BSF_FORT_COMM;
		dst->c.value = src->n_value;
	      }
	    }
	    else {

	      /* Base the value as an index from the base of the section */
	      if (dst->c.section == (asection *)NULL)  
		{
	      dst->c.flags = BSF_EXPORT | BSF_GLOBAL | BSF_ABSOLUTE;
		  dst->c.value = src->n_value;
		}
	      else 
		{
	      dst->c.flags = BSF_EXPORT | BSF_GLOBAL;
		  dst->c.value = src->n_value - dst->c.section->vma;	
		}
	      if (ISFCN(src->n_type)) {
		/* A function ext does not go at the end of a file*/
		dst->c.flags |= BSF_NOT_AT_END;
	      }

	    }

	    break;
	  case C_STAT	:	/* static			*/
	  case C_LABEL	:	/* label			*/
	    dst->c.flags = BSF_LOCAL;
	    /* Base the value as an index from the base of the section */
	    dst->c.value = src->n_value - dst->c.section->vma;	
	    break;

	  case C_MOS	:	/* member of structure	*/
	  case C_EOS	:	/* end of structure		*/
	  case C_REGPARM	: /* register parameter		*/
	  case C_REG	:	/* register variable		*/
	  case 19           :	/* Intel specific REGARG FIXME */
	  case C_TPDEF	:	/* type definition		*/

 	  case C_ARG:
          case C_AUTO:		/* automatic variable */
	  case C_FIELD:		/* bit field */
	  case C_ENTAG	:	/* enumeration tag		*/
	  case C_MOE	:	/* member of enumeration	*/
	  case C_MOU	:	/* member of union		*/
	  case C_UNTAG	:	/* union tag			*/

	    dst->c.flags = BSF_DEBUGGING;
	    dst->c.value = src->n_value;
	    break;

	  case C_FILE	:	/* file name			*/
	  case C_STRTAG	:	/* structure tag		*/
	    dst->c.flags = BSF_DEBUGGING;
	    dst->c.value = src->n_value ;

	    break;
	  case C_BLOCK	:	/* ".bb" or ".eb"		*/
	  case C_FCN	:	/* ".bf" or ".ef"		*/
	    dst->c.flags = BSF_LOCAL;
	    /* Base the value as an index from the base of the section */
	    dst->c.value = src->n_value - dst->c.section->vma;	

	    break;
	  case C_EFCN	:	/* physical end of function	*/
	  case C_NULL:
	  case C_EXTDEF	:	/* external definition		*/
	  case C_ULABEL	:	/* undefined label		*/
	  case C_USTATIC	: /* undefined static		*/
	  case C_LINE	:	/* line # reformatted as symbol table entry */
	  case C_ALIAS	:	/* duplicate tag		*/
	  case C_HIDDEN	:	/* ext symbol in dmert public lib */

	  default:

	    printf("SICK%d\n",src->n_sclass);
	    dst->c.flags = BSF_DEBUGGING;
	    dst->c.value = src->n_value ;

	    break;
	  }




	if (dst->c.flags == 0) fatal("OOOO dear");

	dst->native = src;
	dst->c.udata = 0;
	dst->c.lineno = (alent *)NULL;
	this_index += src->n_numaux + 1;
	dst++;
	number_of_symbols++;
      }

  }
  obj_symbols(abfd) = cached_area;
  obj_raw_syments(abfd) = native_symbols;
  bfd_get_symcount(abfd) = number_of_symbols;

  /* Slurp the line tables for each section too */
  {
    asection *p;
    p = abfd->sections;
    while (p) {
      ecoff_slurp_line_table(abfd, p);
      p =p->next;
    }
  }
  return true;
}

unsigned int
ecoff_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  if (!ecoff_slurp_symbol_table (abfd)) return 0;

  return (bfd_get_symcount (abfd)+1) * (sizeof (esymbol *));
}


unsigned int
ecoff_get_symtab(abfd, alocation)
bfd *abfd;
asymbol **alocation;
{
  unsigned int counter = 0;
  esymbol *symbase;
  esymbol **location = (esymbol **)(alocation);

  if (!ecoff_slurp_symbol_table (abfd)) return 0;

  for (symbase = obj_symbols (abfd); counter++ < bfd_get_symcount (abfd);)
    *(location++) = symbase++;
  *location++ =0;
  return counter;
}

unsigned int
ecoff_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  if (bfd_get_format (abfd) != bfd_object) {
    bfd_error = invalid_operation;
    return 0;
  }

  return   (asect->reloc_count + 1) * sizeof(arelent *);
}




boolean
ecoff_slurp_reloc_table (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  struct reloc *native_relocs;
  arelent *reloc_cache;

  if (asect->relocation) return true;
  if (asect->reloc_count ==0) return true;
  if (!ecoff_slurp_symbol_table (abfd)) return false;

  native_relocs = (struct reloc *)buy_and_read(abfd,
					       asect->rel_filepos,
					       false,
					       sizeof(struct reloc) * asect->reloc_count);
  reloc_cache = (arelent *)xmalloc(asect->reloc_count * sizeof(arelent ));

  {

    unsigned int counter = 0;
    arelent *cache_ptr = reloc_cache;
    struct reloc *src = native_relocs;
    while (counter < asect->reloc_count) 
      {
	cache_ptr->address = src->r_vaddr  - asect->original_vma;
	cache_ptr->sym = (asymbol *)(src->r_symndx +
				     obj_raw_syments (abfd))->n_zeroes;
	/* The symbols that we have read in have been relocated as if their
	 * sections started at 0. But the offsets refering to the symbols
	 * in the raw data have not been modified, so we have to have
	 * a negative addend to compensate
	 */
	if (cache_ptr->sym->section) {
	  cache_ptr->addend = - cache_ptr->sym->section->original_vma;
	}
	else {
	  /* If the symbol doesn't have a section then it hasn't been relocated,
	   * so we don't have to fix it 
	   */
	  cache_ptr->addend = 0;
	}

	cache_ptr->section = 0;
#if I960
	cache_ptr->howto = howto_table + src->r_type;
#endif
#if M88
	if (src->r_type >= R_PCR16L && src->r_type <= R_VRT32) 
	  {
	    cache_ptr->howto = howto_table + src->r_type - R_PCR16L;
	  }
	else 
	  {
	    fatal("unrecognised reloc type %d\n", src->r_type);
	  }
#endif
	cache_ptr++;
	src++;
	counter++;
      }

  }

  free (native_relocs);
  asect->relocation = reloc_cache;
  return true;
}


/* This is stupid.  This function should be a boolean predicate */
unsigned int
ecoff_canonicalize_reloc (abfd, section, relptr)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
{
    return 0;
}

bfd_target ecoff_little_vec =
    {"ecoff-littlemips",      /* name */
       false,			/* data byte order is little */
       false,			/* header byte order is little */

       (HAS_RELOC | EXEC_P |	/* object flags */
	HAS_LINENO | HAS_DEBUG |
	HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

       (SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
       0,			/* valid reloc types */
       '/',			/* ar_pad_char */
       15,			/* ar_max_namelen */
       ecoff_close_and_cleanup,	/* _close_and_cleanup */
       ecoff_set_section_contents, /* bfd_set_section_contents */
       ecoff_new_section_hook,	/* new_section_hook */
       _bfd_dummy_core_file_failing_command, /* _core_file_failing_command */
       _bfd_dummy_core_file_failing_signal, /* _core_file_failing_signal */
       _bfd_dummy_core_file_matches_executable_p, /* _core_file_matches_ex...p */

       bfd_slurp_coff_armap,	/* bfd_slurp_armap */
       _bfd_slurp_extended_name_table, /* bfd_slurp_extended_name_table*/
       bfd_dont_truncate_arname, /* bfd_truncate_arname */

       ecoff_get_symtab_upper_bound, /* get_symtab_upper_bound */
       ecoff_get_symtab,		/* canonicalize_symtab */
       (void (*)())bfd_false,		/* bfd_reclaim_symbol_table */
       ecoff_get_reloc_upper_bound, /* get_reloc_upper_bound */
       ecoff_canonicalize_reloc,	/* bfd_canonicalize_reloc */
       (void (*)())bfd_false,		/* bfd_reclaim_reloc */

       ecoff_get_symcount_upper_bound,	/* bfd_get_symcount_upper_bound */
       ecoff_get_first_symbol,		/* bfd_get_first_symbol */
       ecoff_get_next_symbol,		/* bfd_get_next_symbol */
       ecoff_classify_symbol,		/* bfd_classify_symbol */
       ecoff_symbol_hasclass,		/* bfd_symbol_hasclass */
       ecoff_symbol_name,	        /* bfd_symbol_name */
       ecoff_symbol_value,		/* bfd_symbol_value */

       _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* data */
       _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* hdrs */

       {_bfd_dummy_target, ecoff_object_p, /* bfd_check_format */
	  bfd_generic_archive_p, _bfd_dummy_target},
       {bfd_false, ecoff_mkobject, bfd_false, /* bfd_set_format */
	  bfd_false},
       ecoff_make_empty_symbol,


     };

bfd_target ecoff_big_vec =
    {"ecoff-bigmips",      /* name */
       true,			/* data byte order is big */
       true,			/* header byte order is big */

       (HAS_RELOC | EXEC_P |	/* object flags */
	HAS_LINENO | HAS_DEBUG |
	HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

       (SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
       0,			/* valid reloc types */
       '/',			/* ar_pad_char */
       15,			/* ar_max_namelen */
       ecoff_close_and_cleanup,	/* _close_and_cleanup */
       ecoff_set_section_contents, /* bfd_set_section_contents */
       ecoff_new_section_hook,	/* new_section_hook */
       _bfd_dummy_core_file_failing_command, /* _core_file_failing_command */
       _bfd_dummy_core_file_failing_signal, /* _core_file_failing_signal */
       _bfd_dummy_core_file_matches_executable_p, /* _core_file_matches_ex...p */

       bfd_slurp_coff_armap,	/* bfd_slurp_armap */
       _bfd_slurp_extended_name_table, /* bfd_slurp_extended_name_table*/
       bfd_dont_truncate_arname, /* bfd_truncate_arname */

       ecoff_get_symtab_upper_bound, /* get_symtab_upper_bound */
       ecoff_get_symtab,		/* canonicalize_symtab */
       (void (*)())bfd_false,		/* bfd_reclaim_symbol_table */
       ecoff_get_reloc_upper_bound, /* get_reloc_upper_bound */
       ecoff_canonicalize_reloc,	/* bfd_canonicalize_reloc */
       (void (*)())bfd_false,		/* bfd_reclaim_reloc */

       ecoff_get_symcount_upper_bound,	/* bfd_get_symcount_upper_bound */
       ecoff_get_first_symbol,		/* bfd_get_first_symbol */
       ecoff_get_next_symbol,		/* bfd_get_next_symbol */
       ecoff_classify_symbol,		/* bfd_classify_symbol */
       ecoff_symbol_hasclass,		/* bfd_symbol_hasclass */
       ecoff_symbol_name,	        /* bfd_symbol_name */
       ecoff_symbol_value,		/* bfd_symbol_value */

       _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* data */
       _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

       {_bfd_dummy_target, ecoff_object_p, /* bfd_check_format */
	  bfd_generic_archive_p, _bfd_dummy_target},
       {bfd_false, ecoff_mkobject, bfd_false, /* bfd_set_format */
	  bfd_false},
       ecoff_make_empty_symbol,


     };

#endif /* ECOFF_BFD */
