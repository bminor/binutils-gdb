/* BFD back-end for VERSAdos-E objects.

   Versados is a Motorola trademark.

   Copyright 1995 Free Software Foundation, Inc.
   Written by Steve Chamberlain of Cygnus Support <sac@cygnus.com>.

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

/*
   SUBSECTION
   VERSAdos-E relocateable object file format

   DESCRIPTION

   This module supports reading of VERSAdos relocateable
   object files.

   A VERSAdos file looks like contains

   o Indentification Record
   o External Symbol Definition Record
   o Object Text Recrod
   o End Record


 */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "libiberty.h"


static boolean versados_mkobject PARAMS ((bfd *));
static boolean versados_scan PARAMS ((bfd *));
static const bfd_target *versados_object_p PARAMS ((bfd *));


#define VHEADER '1'
#define VEXTDEF '2'
#define VOTR '3'
#define VEND '4'


#define ES_BASE 17 /* first symbol has esdid 17 */

/* Per file target dependent information */

/* one for each section */
struct esdid
{
  asection *section;		/* ptr to bfd version */
  unsigned char *contents;	/* used to build image */
  int pc; 
  int relocs;			/* reloc count, valid end of pass 1 */
  int donerel;			/* have relocs been translated */
};

typedef struct versados_data_struct
{
  int es_done;			/* count of symbol index, starts at ES_BASE */
  asymbol *symbols;		/* pointer to local symbols */
  char *strings;		/* strings of all the above */
  int stringlen;		/* len of string table (valid end of pass1) */
  int nsyms;			/* number of symbols (valid end of pass1) */
  int nsecsyms;			/* number of sections */

  int pass_2_done; 

  struct esdid e[16];		/* per section info */
  int alert;			/* to see if we're trampling */
  asymbol *rest[256 - 16];	/* per symbol info */

}
tdata_type;

#define VDATA(abfd)       (abfd->tdata.versados_data)
#define EDATA(abfd, n)    (abfd->tdata.versados_data->e[n])
#define RDATA(abfd, n)    (abfd->tdata.versados_data->rest[n])

struct ext_otr
  {
    unsigned char size;
    char type;
    char map[4];
    unsigned char esdid;
    unsigned char data[200];
  };

struct ext_vheader
  {
    unsigned char size;
    char type;			/* record type */
    char name[10];		/* module name */
    char rev;			/* module rev number */
    char lang;
    char vol[4];
    char user[2];
    char cat[8];
    char fname[8];
    char ext[2];
    char time[3];
    char date[3];
    char rest[211];
  };

struct ext_esd
  {
    unsigned char size;
    char type;
    unsigned char esd_entries[1];
  };
#define ESD_ABS 0
#define EST_SHRT_REL_SEC 1
#define EST_STD_REL_SEC 2
#define ESD_XDEF_IN_SEC 4
#define ESD_XREF_SYM    7
union ext_any
  {
    char size;
    struct ext_vheader header;
    struct ext_esd esd;
    struct ext_otr otr;
  };

/* Initialize by filling in the hex conversion array. */





/* Set up the tdata information.  */

static boolean
versados_mkobject (abfd)
     bfd *abfd;
{
  if (abfd->tdata.versados_data == NULL)
    {
      tdata_type *tdata = (tdata_type *) bfd_alloc (abfd, sizeof (tdata_type));
      if (tdata == NULL)
	{
	  bfd_set_error (bfd_error_no_memory);
	  return false;
	}
      abfd->tdata.versados_data = tdata;
      tdata->symbols = NULL;
      VDATA(abfd)->alert = 0x12345678;
    }

  bfd_default_set_arch_mach (abfd, bfd_arch_m68k, 0);
  return true;
}


/* Report a problem in an S record file.  FIXME: This probably should
   not call fprintf, but we really do need some mechanism for printing
   error messages.  */



static asymbol *
versados_new_symbol (abfd, snum, name, val, sec)
     bfd *abfd;
     int snum;
     const char *name;
     bfd_vma val;
     asection *sec;
{
  asymbol *n = VDATA (abfd)->symbols + snum;
  n->name = name;
  n->value = val;
  n->section = sec;
  n->the_bfd = abfd;
  n->flags = 0;
  return n;
}


static int
get_record (abfd, ptr)
     bfd *abfd;
     union ext_any *ptr;
{
  bfd_read (&ptr->size, 1, 1, abfd);
  if (bfd_read ((char *) ptr + 1, 1, ptr->size, abfd) != ptr->size)
    return 0;
  return 1;
}

int
get_4 (pp)
     unsigned char **pp;
{
  unsigned char *p = *pp;
  *pp += 4;
  return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | (p[3] << 0);
}

void
get_10 (pp, name)
     unsigned char **pp;
     char *name;
{
  char *p = (char *) *pp;
  int len = 10;
  *pp += len;
  while (*p != ' '
	 && len)
    {
      *name++ = *p++;
      len--;
    }
  *name = 0;
}

static char *
new_symbol_string (abfd, name)
     bfd *abfd;
     char *name;
{
  char *n = VDATA (abfd)->strings;
  strcpy (VDATA (abfd)->strings, name);
  VDATA (abfd)->strings += strlen (VDATA (abfd)->strings) + 1;
  return n;
}


static void
process_esd (abfd, esd, pass)
     bfd *abfd;
     struct ext_esd *esd;
     int pass;
{
  /* Read through the ext def for the est entries */
  int togo = esd->size - 2;
  bfd_vma size;
  bfd_vma start;
  asection *sec;
  char name[11];
  unsigned char *ptr = esd->esd_entries;
  unsigned char *end = ptr + togo;
  while (ptr < end)
    {
      int scn = *ptr & 0xf;
      int typ = (*ptr >> 4) & 0xf;

      /* Declare this section */
      sprintf (name, "%d", scn);
      sec = bfd_make_section_old_way (abfd, strdup (name));
      sec->target_index = scn;
      EDATA (abfd, scn).section = sec;
      ptr++;
      switch (typ)
	{
	default:
	  abort ();

	case ESD_XREF_SYM:
	  {
	    int snum = VDATA (abfd)->nsyms++;
	    get_10 (&ptr, name);
	    if (pass == 1)
	      {
		VDATA (abfd)->stringlen += strlen (name) + 1;
	      }
	    else
	      {
		int esidx;
		asymbol *s;
		char *n = new_symbol_string (abfd, name);
		s = versados_new_symbol (abfd, snum, n, 0,
					 &bfd_und_section, scn);
		esidx = VDATA (abfd)->es_done++;
		RDATA (abfd, esidx - ES_BASE) = s;
	      }
	  }
	  break;
	case ESD_ABS:
	  size = get_4 (&ptr);
	  start = get_4 (&ptr);
	  break;
	case EST_STD_REL_SEC:
	case EST_SHRT_REL_SEC:
	  {
	    sec->_raw_size = get_4 (&ptr);
	    sec->flags |= SEC_HAS_CONTENTS | SEC_LOAD | SEC_ALLOC;
	  }
	  break;
	case ESD_XDEF_IN_SEC:
	  {
	    int snum = VDATA (abfd)->nsyms++;
	    long val;
	    get_10 (&ptr, name);
	    val = get_4 (&ptr);
	    if (pass == 1)
	      {
		/* Just remember the symbol */
		VDATA (abfd)->stringlen += strlen (name) + 1;
	      }
	    else
	      {
		asymbol *s;
		char *n = new_symbol_string (abfd, name);
		s = versados_new_symbol (abfd, snum, n, val, sec, scn);
		s->flags |= BSF_GLOBAL;
	      }
	  }
	  break;
	}
    }
}

#define R_RELWORD 1
#define R_RELLONG 2
#define R_RELWORD_NEG 3
#define R_RELLONG_NEG 4

reloc_howto_type versados_howto_table[] =
{
  HOWTO (R_RELWORD, 0, 1, 16, false,
	 0, complain_overflow_bitfield, 0,
	 "+v16", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_RELLONG, 0, 2, 32, false,
	 0, complain_overflow_bitfield, 0,
	 "+v32", true, 0xffffffff, 0xffffffff, false),

  HOWTO (R_RELWORD_NEG, 0, -1, 16, false,
	 0, complain_overflow_bitfield, 0,
	 "-v16", true, 0x0000ffff, 0x0000ffff, false),
  HOWTO (R_RELLONG_NEG, 0, -2, 32, false,
	 0, complain_overflow_bitfield, 0,
	 "-v32", true, 0xffffffff, 0xffffffff, false),
};


static void
process_otr (abfd, otr, pass)
     bfd *abfd;
     struct ext_otr *otr;
     int pass;
{
  unsigned long shift;
  long val;
  unsigned char *srcp = otr->data;
  unsigned char *endp = (unsigned char *) otr + otr->size;
  unsigned int bits = (otr->map[0] << 24)
  | (otr->map[1] << 16)
  | (otr->map[2] << 8)
  | (otr->map[3] << 0);

  unsigned char *contents = EDATA (abfd, otr->esdid - 1).contents;

  int dst_idx = EDATA (abfd, otr->esdid - 1).pc;
  for (shift = (1 << 31); srcp < endp; shift >>= 1)
    {
      if (bits & shift)
	{
	  int flag = *srcp++;
	  int esdids = (flag >> 5) & 3;
	  int sizeinwords = ((flag >> 3) & 1) ? 2 : 1;
	  int offsetlen = flag & 7;

	  int j;
	  for (j = 0; j < esdids; j++)
	    {
	      int esdid = *srcp++;
	      /* don't process zero esdids */
	      if (esdid)
		{
		  int rn = EDATA (abfd, otr->esdid - 1).relocs++;
		  if (pass == 1)
		    {
		      /* this is the first pass over the data, 
		         just remember that we need a reloc */
		    }
		  else
		    {
		      arelent *n =
		      EDATA (abfd, otr->esdid - 1).section->relocation + rn;
		      n->address = dst_idx;

		      n->sym_ptr_ptr = (asymbol **) esdid;
		      n->addend = 0;
		      n->howto
			= versados_howto_table
			+ ((j & 1) * 2)
			+ (sizeinwords - 1);

		    }

		}
	    }

	  if (offsetlen)
	    {
	      /* MSB is sign extended */
	      val = srcp[0] | ((srcp[0] & 0x80) ? ~0xff : 0);
	      for (j = 1; j < offsetlen; j++)
		{
		  val = val << 8;
		  val |= srcp[j];
		}
	      srcp += offsetlen;
	    }
	  else
	    val = 0;

	  /* Insert addend into stream */
	  if (pass == 2)
	    switch (sizeinwords)
	      {
	      case 2:
		contents[dst_idx++] = val >> 24;
		contents[dst_idx++] = val >> 16;
		/* fall through */
	      case 1:
		contents[dst_idx++] = val >> 8;
		contents[dst_idx++] = val >> 0;
		break;
	      }

	}
      else
	{
	  if (pass == 2)
	    {
	      /* absolute code, comes in 16 bit lumps */
	      contents[dst_idx] = srcp[0];
	      contents[dst_idx + 1] = srcp[1];
	    }
	  dst_idx += 2;
	  srcp += 2;
	}
    }
  EDATA (abfd, otr->esdid - 1).pc = dst_idx;

}

static boolean
versados_scan (abfd)
     bfd *abfd;
{
  int loop = 1;
  int i;
  int j;

  while (loop)
    {
      union ext_any any;
      if (!get_record (abfd, &any))
	return true;
      switch (any.header.type)
	{
	case VHEADER:
	  break;
	case VEND:
	  loop = 0;
	  break;
	case VEXTDEF:
	  process_esd (abfd, &any.esd, 1);
	  break;
	case VOTR:
	  process_otr (abfd, &any.otr, 1);
	  break;
	}
    }

  /* Now allocate space for the relocs and sections */

  for (i = 0; i < 16; i++)
    {
      struct esdid *esdid = &EDATA (abfd, i);
      if (esdid->section)
	{
	  esdid->section->relocation
	    = (arelent *) bfd_alloc (abfd, sizeof (arelent) * esdid->relocs);

	  esdid->contents
	    = (unsigned char *) bfd_alloc (abfd, esdid->section->_raw_size);

	  esdid->pc = 0;
	  esdid->section->reloc_count = esdid->relocs;
	  if (esdid->relocs)
	    esdid->section->flags |= SEC_RELOC;
	  esdid->relocs = 0;

	  /* Add an entry into the symbol table for it */
	  abfd->symcount++;
	  VDATA (abfd)->stringlen += strlen (esdid->section->name) + 1;
	}
    }
  VDATA (abfd)->symbols = (asymbol *) bfd_alloc (abfd,
				     sizeof (asymbol) * abfd->symcount);
  VDATA (abfd)->strings = bfd_alloc (abfd, VDATA (abfd)->stringlen);
  abfd->symcount = VDATA (abfd)->nsyms;

  /* Actually fill in the section symbols */

  for (j = 0, i = 0; i < 16; i++)
    {
      struct esdid *esdid = &EDATA (abfd, i);
      asection *sec = esdid->section;
      if (sec)
	{
	  asymbol *s = VDATA (abfd)->symbols + j;
	  s->name = new_symbol_string (abfd, sec->name);
	  s->section = sec;
	  s->flags = BSF_LOCAL;
	  s->value = 0;
	  s->the_bfd = abfd;
	  j++;
	}
    }
  abfd->symcount += j;
  if (abfd->symcount)
    abfd->flags |= HAS_SYMS;
  VDATA (abfd)->nsecsyms = j;
  VDATA (abfd)->nsyms = j;
  return 1;
}



/* Check whether an existing file is a versados  file.  */

static const bfd_target *
versados_object_p (abfd)
     bfd *abfd;
{
  struct ext_vheader ext;
  unsigned char len;

  if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET))
    return NULL;


  bfd_read (&len, 1, 1, abfd);
  if (bfd_read (&ext.type, 1, len, abfd) != len
      || ext.type != '1')
    {
      bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  /* ok, looks like a record, build the tdata and read 
     in.. */

  if (!versados_mkobject (abfd)
      || !versados_scan (abfd))
    return NULL;

  return abfd->xvec;
}


static boolean
versados_pass_2 (abfd)
     bfd *abfd;
{
  union ext_any any;

  if (VDATA (abfd)->pass_2_done)
    return 1;

  if (bfd_seek (abfd, 0, SEEK_SET) != 0)
    return 0;

  VDATA (abfd)->es_done = ES_BASE;

  /* read records till we get to where we want to be */

  while (1)
    {
      get_record (abfd, &any);
      switch (any.header.type)
	{
	case VEND:
	  VDATA (abfd)->pass_2_done = 1;
	  return 1;
	case VEXTDEF:
	  process_esd (abfd, &any.esd, 2);
	  break;
	case VOTR:
	  process_otr (abfd, &any.otr, 2);
	  break;
	}
    }
}

static boolean
versados_get_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     asection *section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  if (!versados_pass_2 (abfd))
    return false;

  memcpy (location,
	  EDATA (abfd, section->target_index).contents + offset,
	  count);

  return true;
}

static boolean
versados_set_section_contents (abfd, section, location, offset, bytes_to_do)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type bytes_to_do;
{
  return false;
}


/*ARGSUSED */
static int
versados_sizeof_headers (abfd, exec)
     bfd *abfd;
     boolean exec;
{
  return 0;
}

static asymbol *
versados_make_empty_symbol (abfd)
     bfd *abfd;
{
  asymbol *new = (asymbol *) bfd_zalloc (abfd, sizeof (asymbol));
  if (new)
    new->the_bfd = abfd;
  return new;
}

/* Return the amount of memory needed to read the symbol table.  */

static long
versados_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  return (bfd_get_symcount (abfd) + 1) * sizeof (asymbol *);
}

/* Return the symbol table.  */

static long
versados_get_symtab (abfd, alocation)
     bfd *abfd;
     asymbol **alocation;
{
  unsigned int symcount = bfd_get_symcount (abfd);
  int i;
  asymbol *s;

  versados_pass_2 (abfd);

  for (i = 0, s = VDATA (abfd)->symbols;
       i < symcount;
       s++, i++)
    {
      *alocation++ = s;
    }

  *alocation = NULL;

  return symcount;
}

/*ARGSUSED */
void
versados_get_symbol_info (ignore_abfd, symbol, ret)
     bfd *ignore_abfd;
     asymbol *symbol;
     symbol_info *ret;
{
  bfd_symbol_info (symbol, ret);
}

/*ARGSUSED */
void
versados_print_symbol (ignore_abfd, afile, symbol, how)
     bfd *ignore_abfd;
     PTR afile;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *) afile;
  switch (how)
    {
    case bfd_print_symbol_name:
      fprintf (file, "%s", symbol->name);
      break;
    default:
      bfd_print_symbol_vandf ((PTR) file, symbol);
      fprintf (file, " %-5s %s",
	       symbol->section->name,
	       symbol->name);

    }
}

long
versados_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  return (asect->reloc_count + 1) * sizeof (arelent *);
}


long
versados_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  int count;
  arelent *src;

  versados_pass_2 (abfd);
  src = section->relocation;
  if (!EDATA (abfd, section->target_index).donerel)
    {
      EDATA (abfd, section->target_index).donerel = 1;
      /* translate from indexes to symptr ptrs */
      for (count = 0; count < section->reloc_count; count++)
	{
	  int esdid = (int) src[count].sym_ptr_ptr;
	  if (esdid < ES_BASE)	/* Section relative thing */
	    {
	      src[count].sym_ptr_ptr 
		= EDATA (abfd, esdid - 1).section->symbol_ptr_ptr;
	    }
	  else
	    {
	      src[count].sym_ptr_ptr
		= symbols + esdid - 17 + VDATA (abfd)->nsecsyms;
	    }

	}
    }

  for (count = 0; count < section->reloc_count; count++)
    {
      *relptr++ = src++;
    }
  *relptr = 0;
  return section->reloc_count;
}

#define	versados_close_and_cleanup _bfd_generic_close_and_cleanup
#define versados_bfd_free_cached_info _bfd_generic_bfd_free_cached_info
#define versados_new_section_hook _bfd_generic_new_section_hook

#define versados_bfd_is_local_label bfd_generic_is_local_label
#define versados_get_lineno _bfd_nosymbols_get_lineno
#define versados_find_nearest_line _bfd_nosymbols_find_nearest_line
#define versados_bfd_make_debug_symbol _bfd_nosymbols_bfd_make_debug_symbol

#define versados_bfd_reloc_type_lookup _bfd_norelocs_bfd_reloc_type_lookup

#define versados_set_arch_mach bfd_default_set_arch_mach

#define versados_bfd_get_relocated_section_contents \
  bfd_generic_get_relocated_section_contents
#define versados_bfd_relax_section bfd_generic_relax_section
#define versados_bfd_link_hash_table_create _bfd_generic_link_hash_table_create
#define versados_bfd_link_add_symbols _bfd_generic_link_add_symbols
#define versados_bfd_final_link _bfd_generic_final_link
#define versados_bfd_link_split_section _bfd_generic_link_split_section

const bfd_target versados_vec =
{
  "versados",			/* name */
  bfd_target_versados_flavour,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),
  (SEC_CODE | SEC_DATA | SEC_ROM | SEC_HAS_CONTENTS
   | SEC_ALLOC | SEC_LOAD | SEC_RELOC),		/* section flags */
  0,				/* leading underscore */
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  1,				/* minimum alignment */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* hdrs */

  {
    _bfd_dummy_target,
    versados_object_p,		/* bfd_check_format */
    _bfd_dummy_target,
    _bfd_dummy_target,
  },
  {
    bfd_false,
    versados_mkobject,
    _bfd_generic_mkarchive,
    bfd_false,
  },
  {				/* bfd_write_contents */
    bfd_false,
    bfd_false,
    _bfd_write_archive_contents,
    bfd_false,
  },

  BFD_JUMP_TABLE_GENERIC (versados),
  BFD_JUMP_TABLE_COPY (_bfd_generic),
  BFD_JUMP_TABLE_CORE (_bfd_nocore),
  BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),
  BFD_JUMP_TABLE_SYMBOLS (versados),
  BFD_JUMP_TABLE_RELOCS (versados),
  BFD_JUMP_TABLE_WRITE (versados),
  BFD_JUMP_TABLE_LINK (versados),
  BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),

  (PTR) 0
};
