/* ldsym.h -
   Copyright (C) 1991 Free Software Foundation, Inc.

   This file is part of GLD, the Gnu Linker.

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

typedef struct user_symbol_struct
{
  /* Point to next symbol in this hash chain */
  struct user_symbol_struct *link;

  /* Name of this symbol.  */
  CONST char *name;			

  /* Pointer to next symbol in order of symbol creation */
  struct user_symbol_struct *next; 

  /* Chain of asymbols we see from input files 
     note that we point to the entry in the canonical table of 
     the pointer to the asymbol, *not* the asymbol. This means
     that we can run back and fix all refs to point to the
     defs nearly for free.
     */
  asymbol **srefs_chain;
  asymbol **sdefs_chain;

  /* only ever point to the largest ever common definition -
   * all the rest are turned into refs 
   * scoms and sdefs are never != NULL at same time
   */
  asymbol **scoms_chain;

  



  /* If this symbol is a constructor */
#define SYM_CONSTRUCTOR 1
  /* If this symbol is a warning symbol */
#define SYM_WARNING 2
  /* IF this is an alias for another symbol */
#define SYM_INDIRECT 4
  /* If this symbol explicitly should be kept, despite discarding
     most others.  */
#define SYM_KEEP 8
  /* If its got -y set */
#define SYM_Y 16
  int flags;
} ldsym_type;

extern CONST char *keepsyms_file;
extern int kept_syms;

ldsym_type *ldsym_get PARAMS ((CONST char *));
ldsym_type *ldsym_get_soft PARAMS ((CONST char *));
void ldsym_print_symbol_table PARAMS ((void));
void ldsym_write PARAMS ((void));
boolean ldsym_undefined PARAMS ((CONST char *));

#define FOR_EACH_LDSYM(x)						\
	extern ldsym_type *symbol_head;					\
	ldsym_type *x;							\
	for (x = symbol_head; x != (ldsym_type *)NULL; x = x->next) 	

void ldsym_init PARAMS ((void));
