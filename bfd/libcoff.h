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

/* BFD COFF object file private structure.  */

/* Object file tdata; access macros */

#define obj_icof(bfd)		((struct icofdata *) ((bfd)->tdata))
#define coff_data(bfd)		((struct icofdata *) ((bfd)->tdata))
#define exec_hdr(bfd)		(obj_icof(bfd)->hdr)
#define obj_symbols(bfd)	(obj_icof(bfd)->symbols)
#define	obj_sym_filepos(bfd)	(obj_icof(bfd)->sym_filepos)

#define obj_relocbase(bfd)	(obj_icof(bfd)->relocbase)
#define obj_raw_syments(bfd)	(obj_icof(bfd)->raw_syments)
#define obj_convert(bfd)	(obj_icof(bfd)->conversion_table)
#if CFILE_STUFF
#define obj_symbol_slew(bfd)	(obj_icof(bfd)->symbol_index_slew)
#else
#define obj_symbol_slew(bfd) 0
#endif
#define obj_string_table(bfd)	(obj_icof(bfd)->string_table)

typedef struct {
  asymbol symbol;
  struct internal_syment *native;
  struct lineno_cache_entry *lineno;
} coff_symbol_type;

typedef struct icofdata 
{
/*  struct internal_aouthdr hdr;		/* exec file header */
  coff_symbol_type  *symbols;	/* symtab for input bfd */
  unsigned int *conversion_table;
  file_ptr sym_filepos;

  long symbol_index_slew;	/* used during read to mark whether a
				   C_FILE symbol as been added. */

  struct internal_syment *raw_syments;
  struct lineno *raw_linenos;
  unsigned int raw_syment_count;
  char *string_table;
  unsigned short flags;
  /* These are only valid once writing has begun */
  long int relocbase;
} coff_data_type;

/* We take the address of the first element of a asymbol to ensure that the
 * macro is only ever applied to an asymbol.  */
#define coffsymbol(asymbol) ((coff_symbol_type *)(&((asymbol)->the_bfd)))


/* Generic COFF swapping routines -- also used by GDB.  */
PROTO (void, bfd_coff_swap_sym, (bfd *, SYMENT *));
PROTO (void, bfd_coff_swap_aux, (bfd *, AUXENT *, int, int));
PROTO (void, bfd_coff_swap_lineno, (bfd *, LINENO *));
