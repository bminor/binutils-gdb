/* ELF object file format.
   Copyright (C) 1992, 1993 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* HP PA-RISC support was contributed by the Center for Software Science
   at the University of Utah.  */

#ifndef _OBJ_ELF_H
#define _OBJ_ELF_H

#define OBJ_ELF 1

#include <bfd.h>

#define BYTES_IN_WORD 4		/* for now */
#include "../bfd/libelf.h"

#define TARGET_SYMBOL_FIELDS	int local:1; unsigned long sy_name_offset;

#include "targ-cpu.h"

#ifndef FALSE
#define FALSE 0
#define TRUE  !FALSE
#endif

/* should be conditional on address size! */
#define elf_symbol(asymbol) ((elf_symbol_type *)(&(asymbol)->the_bfd))

#define S_GET_SIZE(S) (elf_symbol ((S)->bsym)->internal_elf_sym.st_size)
#define S_SET_SIZE(S,V) \
  (elf_symbol((S)->bsym)->internal_elf_sym.st_size = (V))

extern asection *gdb_section;

#if 0 /* This should not reference i!!
	 If it's really needed, fix it, and the call site if necessary.  */
#define obj_frob_symbol(S,PUNT)	if ( obj_elf_frob_symbol (S, &PUNT) ) { i++; continue; }
#endif

/* Copy over the function bit and size of a forwarded symbol.  */
#define obj_frob_forward_symbol(sym)					\
  (((sym)->bsym->flags |=						\
    ((sym)->sy_value.X_add_symbol->bsym->flags & BSF_FUNCTION)),	\
   S_SET_SIZE ((sym), S_GET_SIZE ((sym)->sy_value.X_add_symbol)))

#define obj_write_symbol(S)	obj_elf_write_symbol (S)

#define obj_frob_file()	elf_frob_file()

extern int obj_elf_frob_symbol PARAMS ((struct symbol *, int *));
extern void elf_frob_file PARAMS ((void));
extern void elf_file_symbol PARAMS ((char *));

extern int obj_elf_write_symbol PARAMS ((struct symbol *));

extern void obj_elf_section PARAMS ((int));
extern void obj_elf_previous PARAMS ((int));

/* Stabs go in a separate section.  */
#define SEPARATE_STAB_SECTIONS

/* We need 12 bytes at the start of the section to hold some initial
   information.  */
#define INIT_STAB_SECTION(seg) (seg_info (seg)->stabu.p = frag_more (12))

/* Set the filename offset.  */
#define OBJ_PROCESS_STAB(seg, string, stroff, type, other, desc)	\
  ((type) == N_SO							\
   ? (md_number_to_chars (seg_info (seg)->stabu.p,			\
			  (valueT) (stroff),				\
			  4),						\
      0)								\
   : 0)

#endif /* _OBJ_ELF_H */
