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

#define S_SET_OTHER(S,V)                (elf_symbol((S)->bsym)->other = (V))
#define S_SET_TYPE(S,T)                 (elf_symbol((S)->bsym)->type = (T))
#define S_SET_DESC(S,D)                 (elf_symbol((S)->bsym)->desc = (D))
#define S_GET_OTHER(S)                  (elf_symbol((S)->bsym)->other)
#define S_GET_TYPE(S)                   (elf_symbol((S)->bsym)->type)
#define S_GET_DESC(S)                   (elf_symbol((S)->bsym)->desc)

#define S_SET_SIZE(S,V)	     (elf_symbol((S)->bsym)->internal_elf_sym.st_size)

extern asection *gdb_section;

#if 0 /* This should not reference i!!
	 If it's really needed, fix it, and the call site if necessary.  */
#define obj_frob_symbol(S,PUNT)	if ( obj_elf_frob_symbol (S, &PUNT) ) { i++; continue; }
#endif

#define obj_write_symbol(S)	obj_elf_write_symbol (S)

#define obj_frob_file()	elf_frob_file()

extern int obj_elf_frob_symbol PARAMS ((struct symbol *, int *));
extern void elf_frob_file PARAMS ((void));
extern void elf_file_symbol PARAMS ((char *));

extern void obj_elf_section PARAMS ((int));
extern void obj_elf_previous PARAMS ((void));

#endif /* _OBJ_ELF_H */
