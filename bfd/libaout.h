/* BFD back-end data structures for a.out (and similar) files.

   We try to encapsulate the differences in a few routines, and otherwise
   share large masses of code.  This means we only have to fix bugs in
   one place, most of the time.  */

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

/* $Id$ */

#define	adata(bfd)		((struct aoutdata *) ((bfd)->tdata))
#define exec_hdr(bfd)		(adata(bfd)->hdr)
#define obj_textsec(bfd)	(adata(bfd)->textsec)
#define obj_datasec(bfd)	(adata(bfd)->datasec)
#define obj_bsssec(bfd)		(adata(bfd)->bsssec)
#define	obj_sym_filepos(bfd)	(adata(bfd)->sym_filepos)
#define	obj_str_filepos(bfd)	(adata(bfd)->str_filepos)

#define	obj_reloc_entry_size(bfd) (adata(bfd)->reloc_entry_size)

typedef struct aout_symbol {
  asymbol symbol;
  short desc;
  char other;
  unsigned char type;
} aout_symbol_type;

struct aoutdata {
  struct exec *hdr;               /* exec file header */
  aout_symbol_type *symbols;		/* symtab for input bfd */

  
  /* For ease, we do this */
  asection *textsec;
  asection *datasec;
  asection *bsssec;

  /* We remember these offsets so that after check_file_format, we have
     no dependencies on the particular format of the exec_hdr.  */
  file_ptr sym_filepos;
  file_ptr str_filepos;

  /* Size of a relocation entry */
  unsigned reloc_entry_size;
};


#define obj_outsymbols(bfd) ((PTR)(((struct aoutdata *) ((bfd)->tdata))->outsymbols))


/* We take the address of the first element of an asymbol to ensure that the
 * macro is only ever applied to an asymbol */
#define aout_symbol(asymbol) ((aout_symbol_type *)(&(asymbol)->the_bfd))

/*#define obj_symbols(bfd) ((((struct aoutdata *) ((bfd)->tdata))->symbols))*/
#define obj_aout_symbols(bfd) ((((struct aoutdata *) (bfd)->tdata))->symbols)
#define obj_arch_flags(bfd) ((((struct aoutdata *) (bfd)->tdata))->arch_flags)

#define get_tdata(x)  ((struct aoutdata *)((x)->tdata))

/* Prototype declarations for functions defined in aout.c.  */

PROTO (boolean, aout_squirt_out_relocs,(bfd *abfd, asection *section));

PROTO (bfd_target *, some_aout_object_p, (bfd *abfd,
					  bfd_target *(*callback)()));
PROTO (boolean,	aout_mkobject, (bfd *abfd));
PROTO (enum machine_type, aout_machine_type, (enum bfd_architecture arch,
						unsigned long machine));
PROTO (boolean,	aout_set_arch_mach, (bfd *abfd, enum bfd_architecture arch,
						unsigned long machine));

PROTO (boolean,	aout_new_section_hook, (bfd *abfd, asection *newsect));
PROTO (boolean, aout_set_section_contents, (bfd *abfd, sec_ptr section,
			 PTR location, file_ptr offset, int count));

PROTO (asymbol *,	aout_make_empty_symbol, (bfd *abfd));
PROTO (boolean,	aout_slurp_symbol_table, (bfd *abfd));
PROTO (void,	aout_write_syms, (bfd *abfd));
PROTO (void,	aout_reclaim_symbol_table, (bfd *abfd));
PROTO (unsigned int, aout_get_symtab_upper_bound, (bfd *abfd));
PROTO (unsigned int, aout_get_symtab, (bfd *abfd, asymbol **location));
PROTO (boolean,	aout_slurp_reloc_table, (bfd *abfd, sec_ptr asect,
					 asymbol **symbols));
PROTO (unsigned int, aout_canonicalize_reloc, (bfd *abfd, sec_ptr section,
					 arelent **relptr, asymbol **symbols));
PROTO (unsigned int, aout_get_reloc_upper_bound, (bfd *abfd, sec_ptr asect));
PROTO (void,	aout_reclaim_reloc, (bfd *ignore_abfd, sec_ptr ignore));
PROTO (alent *,	aout_get_lineno, (bfd *ignore_abfd, asymbol *ignore_symbol));
PROTO (void,	aout_print_symbol, (bfd *ignore_abfd, PTR file,
			    asymbol *symbol, bfd_print_symbol_enum_type how));
PROTO (boolean,	aout_close_and_cleanup, (bfd *abfd));
PROTO (boolean,	aout_find_nearest_line, (bfd *abfd, asection *section,
      asymbol **symbols, bfd_vma offset, CONST char **filename_ptr,
      CONST char **functionname_ptr, unsigned int *line_ptr));
PROTO (int,	aout_sizeof_headers, (bfd *ignore_abfd));

/* Helper routines in aout.c.  */

PROTO (void,	bfd_aout_swap_exec_header_in, (bfd *abfd,
			 unsigned char *raw_bytes, struct exec *execp));

PROTO (void,	bfd_aout_swap_exec_header_out,(bfd *abfd, struct exec *execp,
			 unsigned char *raw_bytes));

/* A.out uses the generic versions of these routines... */

#define	aout_get_section_contents	bfd_generic_get_section_contents
#define	aout_close_and_cleanup		bfd_generic_close_and_cleanup
