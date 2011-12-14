/* Mach-O object file format for gas, the assembler.
   Copyright 2009, 2011 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3,
   or (at your option) any later version.

   GAS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#ifndef _OBJ_MACH_O_H
#define _OBJ_MACH_O_H

/* Tag to validate Mach-O object file format processing */
#define OBJ_MACH_O 1

#include "bfd/mach-o.h"

#include "targ-cpu.h"

#define OUTPUT_FLAVOR bfd_target_mach_o_flavour

/* All our align expressions are power of two.  */
#define USE_ALIGN_PTWO

/* Common symbols can carry alignment information.  */
#ifndef S_SET_ALIGN
#define S_SET_ALIGN(S,V) do {\
  bfd_mach_o_asymbol *s = (bfd_mach_o_asymbol *) symbol_get_bfdsym (S);\
  s->n_desc = (s->n_desc & 0xf0ff) | (((V) & 0x0f) << 8);\
} while (0)
#endif

extern const pseudo_typeS mach_o_pseudo_table[];

#ifndef obj_pop_insert
#define obj_pop_insert() pop_insert (mach_o_pseudo_table)
#endif

#define obj_sec_sym_ok_for_reloc(SEC)	1

#define obj_read_begin_hook()	{;}
#define obj_symbol_new_hook(s)	{;}

#define EMIT_SECTION_SYMBOLS		0

#endif /* _OBJ_MACH_O_H */
