/* MIPS Extended-Coff handler for Binary File Diddling.
   Written by Per Bothner.

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* This does not compile on anything but a MIPS yet (and I haven't been
   able to test it there either since the latest merge!).  So it stays
   out by default.  */

#define MIPS 1
#include <stdio.h>
#include <string.h>

#include "bfd.h"
#include "libbfd.h"
#include "sysdep.h"

#include "intel-coff.h"
#include "libcoff.h"		/* to allow easier abstraction-breaking */

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
/* FIXME: What do we do with this - ? */
#if 1
  {  R_OPTCALL, 0,2,32,0,0, true, true},
#else
  {  R_OPTCALL, 0,3,32,0,0, true, true},
#endif
};


#define ALIGN(this, boundary) \
  ((( (this) + ((boundary) -1)) & (~((boundary)-1))))


/* Support for Motorola 88k bcs coff as well as Intel 960 coff */



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

static boolean
ecoff_new_section_hook(abfd, section)
bfd *abfd;
asection *section;
{
    return true;
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
/*  assign (vma, s_vaddr);*/
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
  file_info = (char*)bfd_alloc (abfd, readsize);
  if (file_info == NULL) {
    bfd_error = no_memory;
    return 0;
  }
  if (bfd_seek (abfd, 0, false) < 0) return 0;
  if (bfd_read (file_info, 1, readsize, abfd) != readsize) return 0;
  filehdr = (struct filehdr *) file_info;
  sections = (struct scnhdr *) (file_info + sizeof (struct filehdr) + opthdr);

  /* Now copy data as required; construct all asections etc */
  tdata = (struct icofdata *) bfd_zalloc (abfd, sizeof (struct icofdata) +
				                sizeof (AOUTHDR));
  if (tdata == NULL) {
    bfd_release (abfd, file_info);
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

  abfd->flags |= HAS_RELOC | HAS_LINENO | HAS_LOCALS;

  /* FIXME, the guess should be set by OR-ing info from the sections */
  if ((filehdr->f_flags & F_RELFLG) != F_RELFLG) abfd->flags &= ~HAS_RELOC;
  if ((filehdr->f_flags & F_EXEC) == F_EXEC)     abfd->flags |= EXEC_P;
  if ((filehdr->f_flags & F_LNNO) != F_LNNO)     abfd->flags &= ~HAS_LINENO;
  if ((filehdr->f_flags & F_LSYMS) != F_LSYMS)   abfd->flags &= ~HAS_LOCALS;
  set_tdata (abfd, tdata);
  bfd_get_symcount (abfd) = filehdr->f_nsyms;
  if (filehdr->f_nsyms) abfd->flags |= HAS_SYMS;
   
  tdata->sym_filepos = filehdr->f_symptr;
/* FIXME, needs byte swapping */
  tdata->hdr = *(struct aouthdr *)(file_info + sizeof (struct filehdr));
  tdata->symbols = (coff_symbol_type *)NULL;
  bfd_get_start_address (abfd) = exec_hdr (abfd).entry;
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

  if (bfd_seek (abfd,(file_ptr) ((sizeof (long)) * 3), true) < 0) 
    return 0;
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
  rawptr = (char*)bfd_zalloc (abfd, sizeof (struct icofdata));
  if (rawptr == NULL) {
    bfd_error = no_memory;
    return false;
  }
  set_tdata (abfd, rawptr);
  return true;
}

static void
ecoff_count_linenumbers(abfd)
bfd *abfd;
{
  unsigned int limit = bfd_get_symcount(abfd);
  unsigned int i;
  asymbol **p;
  {
    asection *s = abfd->sections->output_section;
    while (s) {
      BFD_ASSERT(s->lineno_count == 0);
      s = s->next;
    }
  }


  for (p = abfd->outsymbols,i = 0; i < limit; i++, p++)
    {
      asymbol *q_maybe = *p;
      if (q_maybe->the_bfd->xvec->flavour == bfd_target_coff_flavour_enum) {
	coff_symbol_type *q = coffsymbol(q_maybe);
	if (q->lineno) 
	  {
	    /* This symbol has a linenumber, increment the
	     * owning section's linenumber count */
	    alent *l = q->lineno;
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
  coff_symbol_type *new = (coff_symbol_type *)xmalloc(sizeof(coff_symbol_type));
  new->native = 0;
  new->lineno = (alent *)NULL;
  new->symbol.the_bfd = abfd;
  return &new->symbol;
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
    abort();
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

static
struct sec *section_from_bfd_index(abfd, index)
bfd *abfd;
int index;
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
  return (obj_symbols (abfd) + idx)->symbol.name;
}

static long
ecoff_symbol_value (abfd, idx)
     bfd *abfd;
     symindex idx;
{
  return (obj_symbols (abfd) + idx)->symbol.value;
}

static symclass
ecoff_classify_symbol (abfd, idx)
     bfd *abfd;
     symindex idx;
{
    abort();
}

static boolean
ecoff_symbol_hasclass (abfd, idx, class)
     bfd *abfd;
     symindex idx;
     symclass class;
{
    abort();
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
    abort();
}

unsigned int
ecoff_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  if (!ecoff_slurp_symbol_table (abfd)) return 0;

  return (bfd_get_symcount (abfd)+1) * (sizeof (coff_symbol_type *));
}


unsigned int
ecoff_get_symtab(abfd, alocation)
bfd *abfd;
asymbol **alocation;
{
  unsigned int counter = 0;
  coff_symbol_type *symbase;
  coff_symbol_type **location = (coff_symbol_type **)(alocation);

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
    abort();
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

boolean
ecoff_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     int count;
{
  if (count) {
    if (offset >= section->size) return false;

    bfd_seek (abfd, section->filepos + offset, SEEK_SET);

    return (bfd_read (location, 1, count, abfd) == count) ? true:false;
  }
  else return true;
}

alent *
ecoff_get_lineno(ignore_abfd, ignore_symbol)
bfd *ignore_abfd;
PTR ignore_symbol;
{
return (alent *)NULL;
}

#define ecoff_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define	ecoff_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define	ecoff_core_file_matches_executable_p	_bfd_dummy_core_file_matches_executable_p
#define	ecoff_slurp_armap		bfd_false
#define	ecoff_slurp_extended_name_table	bfd_false
#define	ecoff_truncate_arname		bfd_void
#define	ecoff_write_armap		bfd_false
#define	ecoff_print_symbol		bfd_void
#define	ecoff_set_arch_mach		bfd_false
#define	ecoff_openr_next_archived_file	bfd_generic_openr_next_archived_file
#define	ecoff_find_nearest_line		bfd_false
#define	ecoff_generic_stat_arch_elt	bfd_generic_stat_arch_elt
#define	ecoff_sizeof_headers		bfd_0

bfd_target ecoff_little_vec =
    {"ecoff-littlemips",      /* name */
       bfd_target_coff_flavour_enum,
       false,			/* data byte order is little */
       false,			/* header byte order is little */

       (HAS_RELOC | EXEC_P |	/* object flags */
	HAS_LINENO | HAS_DEBUG |
	HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

       (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
       '/',			/* ar_pad_char */
       15,			/* ar_max_namelen */
       _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* data */
       _do_getllong, _do_putllong, _do_getlshort, _do_putlshort, /* hdrs */

       {_bfd_dummy_target, ecoff_object_p, /* bfd_check_format */
	  bfd_generic_archive_p, _bfd_dummy_target},
       {bfd_false, ecoff_mkobject, bfd_false, /* bfd_set_format */
	  bfd_false},
       JUMP_TABLE (ecoff)
};

bfd_target ecoff_big_vec =
    {"ecoff-bigmips",      /* name */
    bfd_target_coff_flavour_enum,
       true,			/* data byte order is big */
       true,			/* header byte order is big */

       (HAS_RELOC | EXEC_P |	/* object flags */
	HAS_LINENO | HAS_DEBUG |
	HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT),

       (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
       ' ',			/* ar_pad_char */
       16,			/* ar_max_namelen */
       _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* data */
       _do_getblong, _do_putblong, _do_getbshort, _do_putbshort, /* hdrs */

       {_bfd_dummy_target, ecoff_object_p, /* bfd_check_format */
	  bfd_generic_archive_p, _bfd_dummy_target},
       {bfd_false, ecoff_mkobject, bfd_false, /* bfd_set_format */
	  bfd_false},
       JUMP_TABLE(ecoff)
};
