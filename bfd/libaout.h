/* BFD back-end data structures for a.out (and similar) files.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* We try to encapsulate the differences in the various a.out file
   variants in a few routines, and otherwise share large masses of code.
   This means we only have to fix bugs in one place, most of the time.  */

/* $Id$ */

#ifdef __STDC__
#define CAT3(a,b,c) a##b##c
#else
#define CAT3(a,b,c) a/**/b/**/c
#endif

/* Parameterize the a.out code based on whether it is being built
   for a 32-bit architecture or a 64-bit architecture.  */
#if ARCH_SIZE==64
#define GET_WORD bfd_h_get_64
#define GET_SWORD (int64_type)GET_WORD
#define PUT_WORD bfd_h_put_64
#define NAME(x,y) CAT3(x,_64_,y)
#define JNAME(x) CAT(x,_64)
#define BYTES_IN_WORD 8
#else
#define GET_WORD bfd_h_get_32
#define GET_SWORD (int32_type)GET_WORD
#define PUT_WORD bfd_h_put_32
#define NAME(x,y) CAT3(x,_32_,y)
#define JNAME(x) CAT(x,_32)
#define BYTES_IN_WORD 4
#endif

#define	adata(bfd)		((struct aoutdata *) ((bfd)->tdata))
#define exec_hdr(bfd)		(adata(bfd)->hdr)
#define obj_textsec(bfd)	(adata(bfd)->textsec)
#define obj_datasec(bfd)	(adata(bfd)->datasec)
#define obj_bsssec(bfd)		(adata(bfd)->bsssec)
#define	obj_sym_filepos(bfd)	(adata(bfd)->sym_filepos)
#define	obj_str_filepos(bfd)	(adata(bfd)->str_filepos)

#define	obj_reloc_entry_size(bfd) (adata(bfd)->reloc_entry_size)

/* Declare these types at file level, since they are used in parameter
   lists, which have wierd scope.  */
struct external_exec;
struct internal_exec;

typedef struct aout_symbol {
  asymbol symbol;
  short desc;
  char other;
  unsigned char type;
} aout_symbol_type;

struct aoutdata {
  struct internal_exec *hdr;               /* exec file header */
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

/* Prototype declarations for functions defined in aoutx.h  */

PROTO (boolean, NAME(aout,squirt_out_relocs),(bfd *abfd, asection *section));

PROTO (bfd_target *, NAME(aout,some_aout_object_p), (bfd *abfd,
					  bfd_target *(*callback)()));
PROTO (boolean,	NAME(aout,mkobject), (bfd *abfd));
PROTO (enum machine_type, NAME(aout,machine_type), (enum bfd_architecture arch,
						unsigned long machine));
PROTO (boolean,	NAME(aout,set_arch_mach), (bfd *abfd, enum bfd_architecture arch,
						unsigned long machine));

PROTO (boolean,	NAME(aout,new_section_hook), (bfd *abfd, asection *newsect));
PROTO (boolean, NAME(aout,set_section_contents), (bfd *abfd, sec_ptr section,
			 PTR location, file_ptr offset, bfd_size_type count));

PROTO (asymbol *,NAME(aout,make_empty_symbol), (bfd *abfd));
PROTO (boolean,	NAME(aout,slurp_symbol_table), (bfd *abfd));
PROTO (void,	NAME(aout,write_syms), (bfd *abfd));
PROTO (void,	NAME(aout,reclaim_symbol_table), (bfd *abfd));
PROTO (unsigned int, NAME(aout,get_symtab_upper_bound), (bfd *abfd));
PROTO (unsigned int, NAME(aout,get_symtab), (bfd *abfd, asymbol **location));
PROTO (boolean,	NAME(aout,slurp_reloc_table), (bfd *abfd, sec_ptr asect,
					 asymbol **symbols));
PROTO (unsigned int, NAME(aout,canonicalize_reloc), (bfd *abfd, sec_ptr section,
					 arelent **relptr, asymbol **symbols));
PROTO (unsigned int, NAME(aout,get_reloc_upper_bound), (bfd *abfd, sec_ptr asect));
PROTO (void,	NAME(aout,reclaim_reloc), (bfd *ignore_abfd, sec_ptr ignore));
PROTO (alent *,	NAME(aout,get_lineno), (bfd *ignore_abfd, asymbol *ignore_symbol));
PROTO (void,	NAME(aout,print_symbol), (bfd *ignore_abfd, PTR file,
			    asymbol *symbol, bfd_print_symbol_type how));
PROTO (boolean,	NAME(aout,close_and_cleanup), (bfd *abfd));
PROTO (boolean,	NAME(aout,find_nearest_line), (bfd *abfd, asection *section,
      asymbol **symbols, bfd_vma offset, CONST char **filename_ptr,
      CONST char **functionname_ptr, unsigned int *line_ptr));
PROTO (int,	NAME(aout,sizeof_headers), (bfd *ignore_abfd, boolean exec));


PROTO (void,	NAME(aout,swap_exec_header_in), (bfd *abfd,
			 struct external_exec *raw_bytes, struct internal_exec *execp));

PROTO (void,	NAME(aout,swap_exec_header_out),(bfd *abfd, struct internal_exec *execp,
			 struct external_exec *raw_bytes));

/* A.out uses the generic versions of these routines... */

#define	aout_32_get_section_contents	bfd_generic_get_section_contents
#define	aout_32_close_and_cleanup	bfd_generic_close_and_cleanup

#define	aout_64_get_section_contents	bfd_generic_get_section_contents
#define	aout_64_close_and_cleanup	bfd_generic_close_and_cleanup

/* Calculate the file positions of the parts of a newly read aout header */
#define WORK_OUT_FILE_POSITIONS(abfd, execp)				\
  obj_datasec (abfd)->vma = N_DATADDR(*execp);			 	\
  obj_bsssec (abfd)->vma = N_BSSADDR(*execp);				\
  obj_textsec (abfd)->vma = N_TXTADDR(*execp);				\
  									\
  /* The file offsets of the sections */				\
  obj_textsec (abfd)->filepos = N_TXTOFF (*execp);			\
  obj_datasec (abfd)->filepos = N_DATOFF (*execp);			\
  									\
  /* The file offsets of the relocation info */				\
  obj_textsec (abfd)->rel_filepos = N_TRELOFF(*execp);			\
  obj_datasec (abfd)->rel_filepos = N_DRELOFF(*execp);			\
									\
  /* The file offsets of the string table and symbol table.  */		\
  obj_sym_filepos (abfd) = N_SYMOFF (*execp);				\
  obj_str_filepos (abfd) = N_STROFF (*execp);				\


#define WRITE_HEADERS(abfd, execp)					      \
      {									      \
        if ((obj_textsec(abfd)->vma & 0xff )== EXEC_BYTES_SIZE)  \
		abfd->flags |= D_PAGED; 		\
	else						\
		abfd->flags &= ~D_PAGED;		\
	if (abfd->flags & D_PAGED) 					      \
	    {								      \
	      execp->a_text = obj_textsec (abfd)->size + EXEC_BYTES_SIZE;     \
	      N_SET_MAGIC (*execp, ZMAGIC);				      \
	    } 								      \
	else 								      \
	    {								      \
	      execp->a_text = obj_textsec (abfd)->size;			      \
	      if (abfd->flags & WP_TEXT)				      \
	        { N_SET_MAGIC (*execp, NMAGIC); }			      \
	      else							      \
	  	{ N_SET_MAGIC(*execp, OMAGIC); }			      \
	    }								      \
	if (abfd->flags & D_PAGED) 					      \
	    {								      \
	      data_pad = ((obj_datasec(abfd)->size + PAGE_SIZE -1)	      \
			  & (- PAGE_SIZE)) - obj_datasec(abfd)->size;	      \
	  								      \
	      if (data_pad > obj_bsssec(abfd)->size)			      \
		execp->a_bss = 0;					      \
	      else 							      \
		execp->a_bss = obj_bsssec(abfd)->size - data_pad;	      \
	      execp->a_data = obj_datasec(abfd)->size + data_pad;	      \
	    }								      \
	else 								      \
	    {								      \
	      execp->a_data = obj_datasec (abfd)->size;			      \
	      execp->a_bss = obj_bsssec (abfd)->size;			      \
	    }								      \
    									      \
	execp->a_syms = bfd_get_symcount (abfd) * EXTERNAL_LIST_SIZE;	      \
	execp->a_entry = bfd_get_start_address (abfd);			      \
    									      \
	execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	NAME(aout,swap_exec_header_out) (abfd, execp, &exec_bytes);	      \
									      \
	bfd_seek (abfd, 0L, false);					      \
	bfd_write ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd);	      \
	/* Now write out reloc info, followed by syms and strings */	      \
  									      \
	if (bfd_get_symcount (abfd) != 0) 				      \
	    {								      \
	      bfd_seek (abfd,						      \
			(long)(N_SYMOFF(*execp)), false);		      \
									      \
	      NAME(aout,write_syms)(abfd);				      \
									      \
	      bfd_seek (abfd,	(long)(N_TRELOFF(*execp)), false);	      \
									      \
	      if (!NAME(aout,squirt_out_relocs) (abfd, obj_textsec (abfd))) return false; \
	      bfd_seek (abfd, (long)(N_DRELOFF(*execp)), false);	      \
									      \
	      if (!NAME(aout,squirt_out_relocs)(abfd, obj_datasec (abfd))) return false; \
	    }								      \
      }									      
