/* struct_symbol.h - Internal symbol structure
   Copyright (C) 1987, 1992 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef __struc_symbol_h__
#define __struc_symbol_h__

#ifdef BFD_ASSEMBLER
/* The BFD code wants to walk the list in both directions.  */
#undef  SYMBOLS_NEED_BACKPOINTERS
#define SYMBOLS_NEED_BACKPOINTERS
#endif

/* our version of an nlist node */
struct symbol
{
#ifndef BFD_ASSEMBLER
  /* The (4-origin) position of sy_name in the symbol table of the object
     file.  This will be 0 for (nameless) .stabd symbols.

     Not used until write_object_file() time. */
  unsigned long sy_name_offset;

  /* What we write in .o file (if permitted).  */
  obj_symbol_type sy_symbol;

  /* The 24 bit symbol number.  Symbol numbers start at 0 and are unsigned. */
  long sy_number;
#else
  /* BFD symbol */
  asymbol *bsym;
#endif

  struct symbol *sy_next;	/* forward chain, or NULL */
#ifdef SYMBOLS_NEED_BACKPOINTERS
  struct symbol *sy_previous;	/* backward chain, or NULL */
#endif /* SYMBOLS_NEED_BACKPOINTERS */

  struct frag *sy_frag;		/* NULL or -> frag this symbol attaches to. */

  struct symbol *sy_forward;	/* value is really that of this other symbol */

  int written : 1;

#ifdef TARGET_SYMBOL_FIELDS
  TARGET_SYMBOL_FIELDS
#endif
};

typedef struct symbol symbolS;

#ifndef WORKING_DOT_WORD
struct broken_word
  {
    struct broken_word *next_broken_word;	/* One of these strucs per .word x-y */
    fragS *frag;		/* Which frag its in */
    char *word_goes_here;	/* Where in the frag it is */
    fragS *dispfrag;		/* where to add the break */
    symbolS *add;		/* symbol_x */
    symbolS *sub;		/* - symbol_y */
    offsetT addnum;		/* + addnum */
    int added;			/* nasty thing happend yet? */
    /* 1: added and has a long-jump */
    /* 2: added but uses someone elses long-jump */
    struct broken_word *use_jump;	/* points to broken_word with a similar
					 long-jump */
  };
extern struct broken_word *broken_words;
#endif /* ndef WORKING_DOT_WORD */

/*
 * Current means for getting from symbols to segments and vice verse.
 * This will change for infinite-segments support (e.g. COFF).
 */
/* #define	SYMBOL_TYPE_TO_SEGMENT(symP)  ( N_TYPE_seg [(int) (symP)->sy_type & N_TYPE] ) */
extern const segT N_TYPE_seg[];	/* subseg.c */

#define	SEGMENT_TO_SYMBOL_TYPE(seg)  ( seg_N_TYPE [(int) (seg)] )
extern const short seg_N_TYPE[];/* subseg.c */

#define	N_REGISTER	30	/* Fake N_TYPE value for SEG_REGISTER */

#ifdef SYMBOLS_NEED_BACKPOINTERS

void symbol_clear_list_pointers PARAMS ((symbolS * symbolP));
void symbol_insert PARAMS ((symbolS * addme, symbolS * target,
			    symbolS ** rootP, symbolS ** lastP));
void symbol_remove PARAMS ((symbolS * symbolP, symbolS ** rootP,
			    symbolS ** lastP));
void verify_symbol_chain PARAMS ((symbolS * rootP, symbolS * lastP));

#define symbol_previous(s) ((s)->sy_previous)

#else /* SYMBOLS_NEED_BACKPOINTERS */

#define symbol_clear_list_pointers(clearme) {clearme->sy_next = NULL;}

#endif /* SYMBOLS_NEED_BACKPOINTERS */

void symbol_append PARAMS ((symbolS * addme, symbolS * target,
			    symbolS ** rootP, symbolS ** lastP));

#define symbol_next(s)	((s)->sy_next)

#endif /* __struc_symbol_h__ */

/* end of struc-symbol.h */
