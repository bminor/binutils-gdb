/*
   
 bfd backend for oasys objects.


 Object files contain records in order:

 optional header
 symbol records
 section records
 data records
 debugging records
 end record



   Written by Steve Chamberlain
   steve@cygnus.com



 */


#include <ansidecl.h>
#include "sysdep.h"
#include "bfd.h"
#include "libbfd.h"
#include "obstack.h"
#include "oasys.h"
#include "liboasys.h"



#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

typedef void generic_symbol_type;


void DEFUN(oasys_read_record,(abfd, record),
      bfd *abfd AND 
      oasys_record_union_type *record)
{

  bfd_read(record, 1, sizeof(record->header), abfd);

  bfd_read(((char *)record )+ sizeof(record->header),
	   1, record->header.length - sizeof(record->header),
	   abfd);
}
static size_t
oasys_string_length(record)
oasys_record_union_type *record;
{
return  record->header.length
	- ((char *)record->symbol.name - (char *)record);
}

/*****************************************************************************/

/*

Slurp the symbol table by reading in all the records at the start file
till we get to the first section record.

We'll sort the symbols into  two lists, defined and undefined. The
undefined symbols will also be sorted by refno. We do this by placing
all undefined symbols at the front of the table moving in, and the
defined symbols at the end of the table moving back.

*/

static boolean
oasys_slurp_symbol_table(abfd)
bfd *abfd;
{
  oasys_record_union_type record;
  oasys_data_type *data = oasys_data(abfd);
  boolean loop = true;
  asymbol *dest_undefined;
  asymbol *dest_defined;
  asymbol *dest;
  char *string_ptr;


  if (data->symbols != (asymbol *)NULL) {
    return true;
  }
  /* Buy enough memory for all the symbols and all the names */
  data->symbols = 
    (asymbol *)malloc(sizeof(asymbol) * abfd->symcount);
  data->strings = malloc(data->symbol_string_length);

  dest_undefined = data->symbols;
  dest_defined = data->symbols + abfd->symcount -1;

  string_ptr = data->strings;
  bfd_seek(abfd, (file_ptr)0, SEEK_SET);
  while (loop) {
    oasys_read_record(abfd, &record);
    switch (record.header.type) {
    case oasys_record_is_header_enum:
      break;
    case oasys_record_is_local_enum:
    case oasys_record_is_symbol_enum:
	{
	  size_t length = oasys_string_length(&record);
	  switch (record.symbol.relb[0] & RELOCATION_TYPE_BITS) {
	  case RELOCATION_TYPE_ABS:
	    dest = dest_defined--;
	    dest->section = 0;
	    dest->flags = BSF_ABSOLUTE | BSF_EXPORT | BSF_GLOBAL;
	    dest_defined--;
	    break;
	  case RELOCATION_TYPE_REL:
	    dest = dest_defined--;
	    dest->section =
	      oasys_data(abfd)->sections[record.symbol.relb[0] &
					 RELOCATION_SECT_BITS];
	    if (record.header.type == oasys_record_is_local_enum) 
		{
		  dest->flags =  BSF_LOCAL;
		}
	    else {

	      dest->flags = BSF_EXPORT | BSF_GLOBAL;
	    }
	    break;
	  case RELOCATION_TYPE_UND:
	    dest = dest_undefined++;
	    dest->section = (asection *)NULL;
	    dest->flags = BSF_UNDEFINED;
	    break;
	  case RELOCATION_TYPE_COM:
	    dest = dest_defined--;
	    dest->name = string_ptr;
	    dest->the_bfd = abfd;

	    dest->section = (asection *)NULL;
	    dest->flags = BSF_FORT_COMM;
	    break;
	  }
	  dest->name = string_ptr;
	  dest->the_bfd = abfd;

	  dest->value = bfd_h_getlong(abfd, &record.symbol.value);
	  memcpy(string_ptr, record.symbol.name, length);
	  string_ptr[length] =0;
	  string_ptr += length +1;
	}
      break;
    default:
      loop = false;
    }
  }
  return true;

}

size_t
oasys_get_symtab_upper_bound (abfd)
bfd *abfd;
{
  oasys_slurp_symbol_table (abfd);

  return (abfd->symcount != 0) ? 
    (abfd->symcount+1) * (sizeof (oasys_symbol_type *)) : 0;
}

/* 
*/

extern bfd_target oasys_vec;

unsigned int
oasys_get_symtab (abfd, location)
bfd *abfd;
asymbol **location;
{
  asymbol *symbase ;
  unsigned int counter ;
  if (oasys_slurp_symbol_table(abfd) == false) {
    return 0;
  }
  symbase = oasys_data(abfd)->symbols;
  for (counter = 0; counter < abfd->symcount; counter++) {
    *(location++) = symbase++;
  }
  *location = 0;
  return abfd->symcount;
}

/***********************************************************************
*  archive stuff 
*/
#define swap(x) x = bfd_h_get_x(abfd, &x);
bfd_target *
oasys_archive_p(abfd)
bfd *abfd;
{
  oasys_archive_header_type header;
  unsigned int i;
  
  bfd_seek(abfd, (file_ptr) 0, false);

  
  bfd_read(&header, 1, sizeof(header), abfd);

  
  swap(header.version);
  swap(header.mod_count);
  swap(header.mod_tbl_offset);
  swap(header.sym_tbl_size);
  swap(header.sym_count);
  swap(header.sym_tbl_offset);
  swap(header.xref_count);
  swap(header.xref_lst_offset);

  /*
     There isn't a magic number in an Oasys archive, so the best we
     can do to verify reasnableness is to make sure that the values in
     the header are too weird
     */

  if (header.version>10000 ||
      header.mod_count>10000 ||
      header.sym_count>100000 ||
      header.xref_count > 100000) return (bfd_target *)NULL;

  /*
     That all worked, lets buy the space for the header and read in
     the headers.
     */
  {
    oasys_ar_data_type *ar =
      (oasys_ar_data_type*) malloc(sizeof(oasys_ar_data_type));


    oasys_module_info_type *module = 
      (oasys_module_info_type*)
	malloc(sizeof(oasys_module_info_type) * header.mod_count);

    oasys_module_table_type record;

    oasys_ar_data(abfd) =ar;
    ar->module = module;
    ar->module_count = header.mod_count;

    bfd_seek(abfd , header.mod_tbl_offset, SEEK_SET);
    for (i = 0; i < header.mod_count; i++) {

      bfd_read(&record, 1, sizeof(record), abfd);
      swap(record.mod_size);
      swap(record.file_offset);
      swap(record.mod_name_length);
      module[i].name = malloc(record.mod_name_length+1);

      bfd_read(module[i].name, 1, record.mod_name_length +1, abfd);
      /* SKip some stuff */
      bfd_seek(abfd, record.dep_count * sizeof(int32_type),
	    SEEK_CUR);

      module[i].size = record.mod_size;
      module[i].pos = record.file_offset;
    }
      
  }
  return abfd->xvec;
}

#define MAX_SECS 16
bfd_target *
oasys_object_p (abfd)
bfd *abfd;
{
  oasys_data_type *oasys;
  oasys_data_type static_data;

  boolean loop = true;


  boolean had_usefull = false;

  memset((PTR)static_data.sections, 0xff, sizeof(static_data.sections));
    
  /* Point to the start of the file */
  bfd_seek(abfd, (file_ptr)0, SEEK_SET);
  static_data.symbol_string_length = 0;
  /* Inspect the records, but only keep the section info -
     remember the size of the symbols
     */
  while (loop) {
    oasys_record_union_type record;
    oasys_read_record(abfd, &record);


    switch ((oasys_record_enum_type)(record.header.type)) {
    case oasys_record_is_header_enum:
      had_usefull = true;
      break;
    case oasys_record_is_symbol_enum:
    case oasys_record_is_local_enum:
      /* Count symbols and remember their size for a future malloc   */
      abfd->symcount++;
      static_data.symbol_string_length += 1 + oasys_string_length(&record);
      had_usefull = true;
      break;
    case oasys_record_is_section_enum:
      {
	asection *s;
	char *buffer;
	unsigned int section_number;
	if (record.section.header.length != sizeof(record.section))
	  {
	    return (bfd_target *)NULL;
	  }
	buffer = malloc(3);
	section_number= record.section.relb & RELOCATION_SECT_BITS;
	sprintf(buffer,"%u", section_number);
	s = bfd_make_section(abfd,buffer);
	static_data.sections[section_number] = s;
	switch (record.section.relb & RELOCATION_TYPE_BITS) {
	case RELOCATION_TYPE_ABS:
	case RELOCATION_TYPE_REL:
	  break;
	case RELOCATION_TYPE_UND:
	case RELOCATION_TYPE_COM:
	  BFD_FAIL();
	}


	s->size  = bfd_h_getlong(abfd, & record.section.value) ;
	s->vma = bfd_h_getlong(abfd, &record.section.vma);
	s->flags |= SEC_LOAD | SEC_HAS_CONTENTS;
	had_usefull = true;
      }
      break;
    case oasys_record_is_data_enum:
      static_data.first_data_record = bfd_tell(abfd) - record.header.length;
    case oasys_record_is_debug_enum:
    case oasys_record_is_module_enum:
    case oasys_record_is_named_section_enum:
    case oasys_record_is_end_enum:
      if (had_usefull == false) return (bfd_target *)NULL;
      loop = false;
      break;
    default:
      return (bfd_target *)NULL;
    }
  }
  oasys_data(abfd) = (oasys_data_type
		      *)malloc(sizeof(oasys_data_type));
  oasys = oasys_data(abfd);
  * oasys = static_data;

  oasys->symbols = (asymbol *)NULL;
  /* 
     Oasys support several architectures, but I can't see a simple way
     to discover which one is in a particular file - we'll guess 
     */
  abfd->obj_arch = bfd_arch_m68k;
  abfd->obj_machine =0;
  if (abfd->symcount != 0) {
    abfd->flags |= HAS_SYMS;
  }
  return abfd->xvec;
}


void 
oasys_print_symbol(ignore_abfd, file,  symbol, how)
bfd *ignore_abfd;
FILE *file;
asymbol *symbol;
bfd_print_symbol_enum_type how;
{
  switch (how) {
  case bfd_print_symbol_name_enum:
  case bfd_print_symbol_type_enum:
    fprintf(file,"%s", symbol->name);
    break;
  case bfd_print_symbol_all_enum:
    {
      char *section_name = symbol->section == (asection *)NULL ?
	"*abs" : symbol->section->name;

      bfd_print_symbol_vandf((void *)file,symbol);

      fprintf(file," %-5s %s",
	      section_name,
	      symbol->name);
    }
    break;
  }
}
/*
 The howto table is build using the top two bits of a reloc byte to
 index into it. The bits are PCREL,WORD/LONG
*/
static reloc_howto_type howto_table[]= 
{
/* T rs size bsz pcrel bitpos abs ovr sf name partial inplace mask */

{  0, 0,  1,   16, false,0,   true,true,0,"abs16",true,0x0000ffff},
{  0, 0,  2,   32, false,0,   true,true,0,"abs32",true,0xffffffff},
{  0, 0,  1,   16, true,0,   true,true,0,"pcrel16",true,0x0000ffff},
{  0, 0,  2,   32, true,0,   true,true,0,"pcrel32",true,0xffffffff}
};

/* Read in all the section data and relocation stuff too */
static boolean oasys_slurp_section_data(abfd)
bfd *abfd;
{
  oasys_record_union_type record;
  oasys_data_type *data = oasys_data(abfd);
  boolean loop = true;

  oasys_per_section_type *per ;

  asection *s;

  /* Buy enough memory for all the section data and relocations */
  for (s = abfd->sections; s != (asection *)NULL; s= s->next) {
    per =  oasys_per_section(s);
    if (per->data != (bfd_byte*)NULL) return true;
    per->data = (bfd_byte *) malloc(s->size);
    obstack_init(&per->reloc_obstack);
    per->reloc_tail_ptr = (oasys_reloc_type **)&(s->relocation);
  }

  bfd_seek(abfd, data->first_data_record, SEEK_SET);
  while (loop) {
    oasys_read_record(abfd, &record);
    switch (record.header.type) {
    case oasys_record_is_header_enum:
      break;
    case oasys_record_is_data_enum:
      {

	uint8e_type *src = record.data.data;
	uint8e_type *end_src = ((uint8e_type *)&record) + record.header.length;
	unsigned int relbit;
	bfd_byte *dst_ptr ;
	bfd_byte *dst_base_ptr ;
	asection *section;
	unsigned int count;

	bfd_vma dst_offset = bfd_h_getlong(abfd, record.data.addr);
	section = data->sections[record.data.relb & RELOCATION_SECT_BITS];
	per =  oasys_per_section(section);
dst_base_ptr = 	dst_ptr = oasys_per_section(section)->data + dst_offset;

	while (src < end_src) {
	  uint8e_type mod_byte = *src++;
	  count = 8;

	  for (relbit = 1; count-- != 0; relbit <<=1) 
	    {
	      if (relbit & mod_byte) 
		{
		  uint8e_type reloc = *src;
		  /* This item needs to be relocated */
		  switch (reloc & RELOCATION_TYPE_BITS) {
		  case RELOCATION_TYPE_ABS:

		    break;

		  case RELOCATION_TYPE_REL: 
		    {
		      /* Relocate the item relative to the section */
		      oasys_reloc_type *r =
			(oasys_reloc_type *)
			  obstack_alloc(&per->reloc_obstack,
					sizeof(oasys_reloc_type));
		      *(per->reloc_tail_ptr) = r;
		      per->reloc_tail_ptr = &r->next;
		      r->next= (oasys_reloc_type *)NULL;
		      /* Reference to undefined symbol */
		      src++;
		      /* There is no symbol */
		      r->symbol = 0;
		      /* Work out the howto */
		      r->relent.section =
			data->sections[reloc & RELOCATION_SECT_BITS];
		      r->relent.addend = 0;
		      r->relent.address = dst_ptr - dst_base_ptr;
		      r->relent.howto = &howto_table[reloc>>6];
		      section->reloc_count++;

		    }
		    break;


		  case RELOCATION_TYPE_UND:
		    { 
		      oasys_reloc_type *r =
			(oasys_reloc_type *)
			  obstack_alloc(&per->reloc_obstack,
					sizeof(oasys_reloc_type));
		      *(per->reloc_tail_ptr) = r;
		      per->reloc_tail_ptr = &r->next;
		      r->next= (oasys_reloc_type *)NULL;
		      /* Reference to undefined symbol */
		      src++;
		      /* Get symbol number */
		      r->symbol = (src[0]<<8) | src[1];
		      /* Work out the howto */
		      r->relent.section = (asection *)NULL;
		      r->relent.addend = 0;
		      r->relent.address = dst_ptr - dst_base_ptr;
		      r->relent.howto = &howto_table[reloc>>6];

		      section->reloc_count++;
		      src+=2;
		    }
		    break;
		  case RELOCATION_TYPE_COM:
		    BFD_FAIL();
		  }
		}
	      *dst_ptr++ = *src++;
	    }
	}	  
      }
      break;
    case oasys_record_is_local_enum:
    case oasys_record_is_symbol_enum:
    case oasys_record_is_section_enum:
      break;
    default:
      loop = false;
    }
  }
  return true;

}





boolean
oasys_new_section_hook (abfd, newsect)
bfd *abfd;
asection *newsect;
{
  newsect->used_by_bfd = (oasys_per_section_type *)
    malloc(sizeof(oasys_per_section_type));
  oasys_per_section( newsect)->data = (bfd_byte *)NULL;
  oasys_per_section(newsect)->section = newsect;
  oasys_per_section(newsect)->offset  = 0;
  return true;
}


unsigned int
oasys_get_reloc_upper_bound (abfd, asect)
bfd *abfd;
sec_ptr asect;
{
  oasys_slurp_section_data(abfd);
  return (asect->reloc_count+1) * sizeof(arelent *);
}

static boolean
oasys_get_section_contents (abfd, section, location, offset, count)
bfd *abfd;
sec_ptr section;
void  *location;
file_ptr offset;
unsigned      int count;
{
  oasys_per_section_type *p = section->used_by_bfd;
  oasys_slurp_section_data(abfd);
  (void)  memcpy(location, p->data + offset, count);
  return true;
}


unsigned int
oasys_canonicalize_reloc (abfd, section, relptr, symbols)
bfd *abfd;
sec_ptr section;
arelent **relptr;
asymbol **symbols;
{
  oasys_reloc_type *src = (oasys_reloc_type *)(section->relocation);
  while (src != (oasys_reloc_type *)NULL) {
    if (src->relent.section == (asection *)NULL) {
    src->relent.sym_ptr_ptr = symbols + src->symbol;
  }
    *relptr ++ = &src->relent;
    src = src->next;
  }
  *relptr = (arelent *)NULL;
  return section->reloc_count;
}

boolean
oasys_set_arch_mach (abfd, arch, machine)
bfd *abfd;
enum bfd_architecture arch;
unsigned long machine;
{
  abfd->obj_arch = arch;
  abfd->obj_machine = machine;
  return true;
}

boolean
oasys_mkobject(abfd)
bfd *abfd;
{
  oasys_data_type *oasys =
    (oasys_data_type *) malloc(sizeof(oasys_data_type));
  oasys_data(abfd) = oasys;
  if (oasys == (oasys_data_type *)NULL) {
    bfd_error = no_memory;
    return false;
  }

  return true;
}







static void
init_for_output(abfd)
bfd *abfd;
{
  asection *s; 
  for (s = abfd->sections; s != (asection *)NULL; s = s->next) {
    if (s->size != 0) {
      oasys_per_section(s)->data = (bfd_byte *)(malloc(s->size));
    }
  }
}

/** exec and core file sections */

/* set section contents is complicated with OASYS since the format is 
* not a byte image, but a record stream.
*/
boolean
oasys_set_section_contents (abfd, section, location, offset, count)
bfd *abfd;
sec_ptr section;
unsigned char *location;
file_ptr offset;
int count;
{
  if (oasys_per_section(section)->data == (bfd_byte *)NULL) {
    init_for_output(abfd);
  }
  (void) memcpy(oasys_per_section(section)->data + offset, location, count);
  return true;
}




/* Native-level interface to symbols. */

/* We read the symbols into a buffer, which is discarded when this
function exits.  We read the strings into a buffer large enough to
hold them all plus all the cached symbol entries. */

asymbol *
oasys_make_empty_symbol (abfd)
bfd *abfd;
{

  oasys_symbol_type  *new =
    (oasys_symbol_type *)zalloc (sizeof (oasys_symbol_type));
  new->symbol.the_bfd = abfd;
  return &new->symbol;

}

void
oasys_reclaim_symbol_table (abfd)
bfd *abfd;
{
#if 0
  asection *section;

  if (!bfd_get_symcount (abfd)) return;

  for (section = abfd->sections; section != NULL; section = section->next)
    if (section->relocation) {
      free ((void *)section->relocation);
      section->relocation = NULL;
      section->reloc_count = 0;
    }

  bfd_get_symcount (abfd) = 0;
  free ((void *)obj_aout_symbols (abfd));
  obj_aout_symbols (abfd) = (aout_symbol_type *)NULL;
#endif
}




/* Obsbolete procedural interface; better to look at the cache directly */

/* User should have checked the file flags; perhaps we should return
BFD_NO_MORE_SYMBOLS if there are none? */

int
oasys_get_symcount_upper_bound (abfd)
bfd *abfd;
{
#if 0
  /* In case we're doing an output file or something...?  */
  if (bfd_get_symcount (abfd)) return bfd_get_symcount (abfd);

  return (exec_hdr (abfd)->a_syms) / (sizeof (struct nlist));
#endif
}

symindex
oasys_get_first_symbol (ignore_abfd)
bfd * ignore_abfd;
{
  return 0;
}

symindex
oasys_get_next_symbol (abfd, oidx)
bfd *abfd;
symindex oidx;
{
#if 0
  if (oidx == BFD_NO_MORE_SYMBOLS) return BFD_NO_MORE_SYMBOLS;
  return ++oidx >= bfd_get_symcount (abfd) ? BFD_NO_MORE_SYMBOLS :
  oidx;
#endif
}

char *
oasys_symbol_name (abfd, idx)
bfd *abfd;
symindex idx;
{
#if 0
  return (obj_aout_symbols (abfd) + idx)->symbol.name;
#endif
}

long
oasys_symbol_value (abfd, idx)
bfd *abfd;
symindex idx;
{
#if 0
  return (obj_aout_symbols (abfd) + idx)->symbol.value;
#endif
}

symclass
oasys_classify_symbol (abfd, idx)
bfd *abfd;
symindex idx;
{
#if 0
  aout_symbol_type *sym = obj_aout_symbols (abfd) + idx;

  if ((sym->symbol.flags & BSF_FORT_COMM) != 0)   return bfd_symclass_fcommon;
  if ((sym->symbol.flags & BSF_GLOBAL) != 0)    return bfd_symclass_global;
  if ((sym->symbol.flags & BSF_DEBUGGING) != 0)  return bfd_symclass_debugger;
  if ((sym->symbol.flags & BSF_UNDEFINED) != 0) return bfd_symclass_undefined;
#endif
  return bfd_symclass_unknown;
}

boolean
oasys_symbol_hasclass (abfd, idx, class)
bfd *abfd;
symindex idx;
symclass class;
{
#if 0
  aout_symbol_type *sym = obj_aout_symbols (abfd) + idx;
  switch (class) {
  case bfd_symclass_fcommon:
    return (sym->symbol.flags & BSF_FORT_COMM) ? true :false;
  case bfd_symclass_global:
    return (sym->symbol.flags & BSF_GLOBAL) ? true:false;
  case bfd_symclass_debugger:
    return (sym->symbol.flags & BSF_DEBUGGING) ? true:false;;
  case bfd_symclass_undefined:
    return (sym->symbol.flags & BSF_UNDEFINED) ? true:false;;
  default: return false;
  }
#endif
}


void
oasys_reclaim_reloc (ignore_abfd, section)
bfd *ignore_abfd;
sec_ptr section;
{
#if 0
  if (section->relocation) {
    free (section->relocation);
    section->relocation = NULL;
    section->reloc_count = 0;
  }
#endif
}

boolean
oasys_close_and_cleanup (abfd)
bfd *abfd;
{
  if (bfd_read_p (abfd) == false)
    switch (abfd->format) {
    case bfd_archive:
      if (!_bfd_write_archive_contents (abfd)) {
	return false;
      }
      break;
    case bfd_object:
/*      if (!oasys_write_object_contents (abfd)) */{
	return false;
      }
      break;
    default:
      bfd_error = invalid_operation;
      return false;
    }


  if (oasys_data(abfd) != (oasys_data_type *)NULL) {
    /* FIXME MORE LEAKS */

  }

  return true;
}

static bfd *
oasys_openr_next_archived_file(arch, prev)
bfd *arch;
bfd *prev;
{
  oasys_ar_data_type *ar = oasys_ar_data(arch);
  oasys_module_info_type *p;
  /* take the next one from the arch state, or reset */
  if (prev == (bfd *)NULL) {
    /* Reset the index - the first two entries are bogus*/
    ar->module_index = 0;
  }

  p = ar->module + ar->module_index;
  ar->module_index++;

  if (ar->module_index <= ar->module_count) {
    if (p->abfd == (bfd *)NULL) {
      p->abfd = _bfd_create_empty_archive_element_shell(arch);
      p->abfd->origin = p->pos;
      p->abfd->filename = p->name;

      /* Fixup a pointer to this element for the member */
      p->abfd->arelt_data = (void *)p;
    }
    return p->abfd;
  }
  else {
    bfd_error = no_more_archived_files;
    return (bfd *)NULL;
  }
}

static boolean
oasys_find_nearest_line(abfd,
			 section,
			 symbols,
			 offset,
			 filename_ptr,
			 functionname_ptr,
			 line_ptr)
bfd *abfd;
asection *section;
asymbol **symbols;
bfd_vma offset;
char **filename_ptr;
char **functionname_ptr;
unsigned int *line_ptr;
{
  return false;

}

static int
oasys_stat_arch_elt(abfd, buf)
bfd *abfd;
struct stat *buf;
{
  oasys_module_info_type *mod = abfd->arelt_data;
  if (mod == (oasys_module_info_type *)NULL) {
    bfd_error = invalid_operation;
    return -1;
  }
  else {
    buf->st_size = mod->size;
    buf->st_mode = 0666;
  return 0;
  }


}


/*SUPPRESS 460 */
bfd_target oasys_vec =
{
  "oasys",			/* name */
  bfd_target_oasys_flavour_enum,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_CODE|SEC_DATA|SEC_ROM|SEC_HAS_CONTENTS
   |SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  0,				/* valid reloc types */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  oasys_close_and_cleanup,	/* _close_and_cleanup */
  oasys_set_section_contents,	/* bfd_set_section_contents */
  oasys_get_section_contents,
  oasys_new_section_hook,	/*   new_section_hook */
  0,				/* _core_file_failing_command */
  0,				/* _core_file_failing_signal */
  0,				/* _core_file_matches_ex...p */

  0,				/* bfd_slurp_bsd_armap,		      bfd_slurp_armap */
  bfd_true,			/* bfd_slurp_extended_name_table */
  bfd_bsd_truncate_arname,	/* bfd_truncate_arname */

  oasys_get_symtab_upper_bound,	/* get_symtab_upper_bound */
  oasys_get_symtab,		/* canonicalize_symtab */
  0,				/* oasys_reclaim_symbol_table,            bfd_reclaim_symbol_table */
  oasys_get_reloc_upper_bound,	/* get_reloc_upper_bound */
  oasys_canonicalize_reloc,	/* bfd_canonicalize_reloc */
  0,				/*  oasys_reclaim_reloc,                   bfd_reclaim_reloc */
  0,				/* oasys_get_symcount_upper_bound,        bfd_get_symcount_upper_bound */
  0,				/* oasys_get_first_symbol,                bfd_get_first_symbol */
  0,				/* oasys_get_next_symbol,                 bfd_get_next_symbol */
  0,				/* oasys_classify_symbol,                 bfd_classify_symbol */
  0,				/* oasys_symbol_hasclass,                 bfd_symbol_hasclass */
  0,				/* oasys_symbol_name,                     bfd_symbol_name */
  0,				/* oasys_symbol_value,                    bfd_symbol_value */

  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* data */
  _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

  {_bfd_dummy_target,
     oasys_object_p,		/* bfd_check_format */
     oasys_archive_p,
     bfd_false
     },
  {
    bfd_false,
    oasys_mkobject, 
    _bfd_generic_mkarchive,
    bfd_false
    },
  oasys_make_empty_symbol,
  oasys_print_symbol,
  bfd_false,			/*	oasys_get_lineno,*/
  oasys_set_arch_mach,		/* bfd_set_arch_mach,*/
  bfd_false,
  oasys_openr_next_archived_file,
  oasys_find_nearest_line,	/* bfd_find_nearest_line */
  oasys_stat_arch_elt,		/* bfd_stat_arch_elt */
};
