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

/* BFD back-end data structures for a.out (and similar) files.

   We try to encapsulate the differences in a few routines, and otherwise
   share large masses of code.  This means we only have to fix bugs in
   one place, most of the time.  */

/** a.out files */

#define exec_hdr(bfd) (((struct sunexdata *) ((bfd)->tdata))->hdr)
/*#define obj_symbols(bfd) ((((struct sunexdata *) ((bfd)->tdata))->symbols))*/
#define obj_textsec(bfd)  (((struct sunexdata *) ((bfd)->tdata))->textsec)
#define obj_datasec(bfd)  (((struct sunexdata *) ((bfd)->tdata))->datasec)
#define obj_bsssec(bfd)  (((struct sunexdata *) ((bfd)->tdata))->bsssec)
#define	obj_sym_filepos(bfd) (((struct sunexdata *) ((bfd)->tdata))->sym_filepos)
#define	obj_str_filepos(bfd) (((struct sunexdata *) ((bfd)->tdata))->str_filepos)



typedef struct {
  asymbol symbol;
  short desc;
char other;
unsigned char type;
} aout_symbol_type;


struct sunexdata {
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
};


#define obj_outsymbols(bfd) ((void *)(((struct sunexdata *) ((bfd)->tdata))->outsymbols))



/* We case the address of the first element of a asymbol to ensure that the
 * macro is only every applied to an asymbol
 */
#define aout_symbol(asymbol) ((aout_symbol_type *)(&(asymbol)->the_bfd))

/*#define obj_symbols(bfd) ((((struct sunexdata *) ((bfd)->tdata))->symbols))*/
#define obj_aout_symbols(bfd) ((((struct sunexdata *) (bfd)->tdata))->symbols)
#define obj_arch_flags(bfd) ((((struct sunexdata *) (bfd)->tdata))->arch_flags)

#define get_tdata(x)  ((struct sunexdata *)((x)->tdata))
#define set_tdata(x,y)  ((x)->tdata = (void *) (y))


