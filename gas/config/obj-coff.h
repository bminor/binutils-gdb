/* coff object file format
   Copyright (C) 1989, 1990, 1991, 1992, 1994 Free Software Foundation, Inc.

   This file is part of GAS.

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

#ifndef OBJ_FORMAT_H
#define OBJ_FORMAT_H

#define OBJ_COFF 1

#include "targ-cpu.h"

#include "bfd.h"

/* This internal_lineno crap is to stop namespace pollution from the
   bfd internal coff headerfile. */

#define internal_lineno bfd_internal_lineno
#include "coff/internal.h"
#undef internal_lineno

#include "../bfd/libcoff.h"

#ifdef TC_PPC
#include "coff/rs6000.h"
#endif

#ifdef TC_SPARC
#include "coff/sparc.h"
#ifdef TE_LYNX
#define TARGET_FORMAT "coff-sparc-lynx"
#else
#define TARGET_FORMAT "coff-sparc"
#endif
#endif

#ifdef TC_I386
#include "coff/i386.h"
#ifndef TARGET_FORMAT
#define TARGET_FORMAT "coff-i386"
#endif
#endif

#ifdef TC_M68K
#include "coff/m68k.h"
#ifndef TARGET_FORMAT
#define TARGET_FORMAT "coff-m68k"
#endif
#endif

#ifdef TC_A29K
#include "coff/a29k.h"
#define TARGET_FORMAT "coff-a29k-big"
#endif

#ifdef TC_I960
#include "coff/i960.h"
#define TARGET_FORMAT "coff-Intel-little"
#endif

/* SYMBOL TABLE */

/* targets may also set this */
#undef SYMBOLS_NEED_BACKPOINTERS
#define SYMBOLS_NEED_BACKPOINTERS 1

/* Alter the field names, for now, until we've fixed up the other
   references to use the new name.  */
#ifdef TC_I960
#define TC_SYMFIELD_TYPE	struct symbol *
#define sy_tc			bal
#endif

#define OBJ_SYMFIELD_TYPE	unsigned long
#define sy_obj			sy_flags

#ifndef OBJ_COFF_MAX_AUXENTRIES
#define OBJ_COFF_MAX_AUXENTRIES 1
#endif


#define SYM_AUXENT(S)	(&coffsymbol ((S)->bsym)->native[1].u.auxent)

#define DO_NOT_STRIP	0
#define DO_STRIP	1

extern void obj_coff_section PARAMS ((int));

/* The number of auxiliary entries */
#define S_GET_NUMBER_AUXILIARY(s)	(coffsymbol((s)->bsym)->native->u.syment.n_numaux)
/* The number of auxiliary entries */
#define S_SET_NUMBER_AUXILIARY(s,v)	(S_GET_NUMBER_AUXILIARY (s) = (v))

/* True if a symbol name is in the string table, i.e. its length is > 8. */
#define S_IS_STRING(s)		(strlen(S_GET_NAME(s)) > 8 ? 1 : 0)

extern int S_SET_DATA_TYPE PARAMS ((struct symbol *, int));
extern int S_SET_STORAGE_CLASS PARAMS ((struct symbol *, int));
extern int S_GET_STORAGE_CLASS PARAMS ((struct symbol *));
extern void SA_SET_SYM_ENDNDX PARAMS ((struct symbol *, struct symbol *));

/* Auxiliary entry macros. SA_ stands for symbol auxiliary */
/* Omit the tv related fields */
/* Accessors */

#define SA_GET_SYM_TAGNDX(s)	(SYM_AUXENT (s)->x_sym.x_tagndx.l)
#define SA_GET_SYM_LNNO(s)	(SYM_AUXENT (s)->x_sym.x_misc.x_lnsz.x_lnno)
#define SA_GET_SYM_SIZE(s)	(SYM_AUXENT (s)->x_sym.x_misc.x_lnsz.x_size)
#define SA_GET_SYM_FSIZE(s)	(SYM_AUXENT (s)->x_sym.x_misc.x_fsize)
#define SA_GET_SYM_LNNOPTR(s)	(SYM_AUXENT (s)->x_sym.x_fcnary.x_fcn.x_lnnoptr)
#define SA_GET_SYM_ENDNDX(s)	(SYM_AUXENT (s)->x_sym.x_fcnary.x_fcn.x_endndx)
#define SA_GET_SYM_DIMEN(s,i)	(SYM_AUXENT (s)->x_sym.x_fcnary.x_ary.x_dimen[(i)])
#define SA_GET_FILE_FNAME(s)	(SYM_AUXENT (s)->x_file.x_fname)
#define SA_GET_SCN_SCNLEN(s)	(SYM_AUXENT (s)->x_scn.x_scnlen)
#define SA_GET_SCN_NRELOC(s)	(SYM_AUXENT (s)->x_scn.x_nreloc)
#define SA_GET_SCN_NLINNO(s)	(SYM_AUXENT (s)->x_scn.x_nlinno)

#define SA_SET_SYM_LNNO(s,v)	(SYM_AUXENT (s)->x_sym.x_misc.x_lnsz.x_lnno=(v))
#define SA_SET_SYM_SIZE(s,v)	(SYM_AUXENT (s)->x_sym.x_misc.x_lnsz.x_size=(v))
#define SA_SET_SYM_FSIZE(s,v)	(SYM_AUXENT (s)->x_sym.x_misc.x_fsize=(v))
#define SA_SET_SYM_LNNOPTR(s,v)	(SYM_AUXENT (s)->x_sym.x_fcnary.x_fcn.x_lnnoptr=(v))
#define SA_SET_SYM_DIMEN(s,i,v)	(SYM_AUXENT (s)->x_sym.x_fcnary.x_ary.x_dimen[(i)]=(v))
#define SA_SET_FILE_FNAME(s,v)	strncpy(SYM_AUXENT (s)->x_file.x_fname,(v),FILNMLEN)
#define SA_SET_SCN_SCNLEN(s,v)	(SYM_AUXENT (s)->x_scn.x_scnlen=(v))
#define SA_SET_SCN_NRELOC(s,v)	(SYM_AUXENT (s)->x_scn.x_nreloc=(v))
#define SA_SET_SCN_NLINNO(s,v)	(SYM_AUXENT (s)->x_scn.x_nlinno=(v))

/*
 * Internal use only definitions. SF_ stands for symbol flags.
 *
 * These values can be assigned to sy_symbol.ost_flags field of a symbolS.
 *
 * You'll break i960 if you shift the SYSPROC bits anywhere else.  for
 * more on the balname/callname hack, see tc-i960.h.  b.out is done
 * differently.
 */

#define SF_I960_MASK	(0x000001ff)	/* Bits 0-8 are used by the i960 port. */
#define SF_SYSPROC	(0x0000003f)	/* bits 0-5 are used to store the sysproc number */
#define SF_IS_SYSPROC	(0x00000040)	/* bit 6 marks symbols that are sysprocs */
#define SF_BALNAME	(0x00000080)	/* bit 7 marks BALNAME symbols */
#define SF_CALLNAME	(0x00000100)	/* bit 8 marks CALLNAME symbols */

#define SF_NORMAL_MASK	(0x0000ffff)	/* bits 12-15 are general purpose. */

#define SF_STATICS	(0x00001000)	/* Mark the .text & all symbols */
#define SF_DEFINED	(0x00002000)	/* Symbol is defined in this file */
#define SF_STRING	(0x00004000)	/* Symbol name length > 8 */
#define SF_LOCAL	(0x00008000)	/* Symbol must not be emitted */

#define SF_DEBUG_MASK	(0xffff0000)	/* bits 16-31 are debug info */

#define SF_FUNCTION	(0x00010000)	/* The symbol is a function */
#define SF_PROCESS	(0x00020000)	/* Process symbol before write */
#define SF_TAGGED	(0x00040000)	/* Is associated with a tag */
#define SF_TAG		(0x00080000)	/* Is a tag */
#define SF_DEBUG	(0x00100000)	/* Is in debug or abs section */
#define SF_GET_SEGMENT	(0x00200000)	/* Get the section of the forward symbol. */
/* All other bits are unused. */

/* Accessors */
#define SF_GET(s)		((s)->sy_flags)
#define SF_GET_DEBUG(s)		((s)->bsym->flags & BSF_DEBUGGING)
#define SF_SET_DEBUG(s)		((s)->bsym->flags |= BSF_DEBUGGING)
#define SF_GET_NORMAL_FIELD(s)	(SF_GET (s) & SF_NORMAL_MASK)
#define SF_GET_DEBUG_FIELD(s)	(SF_GET (s) & SF_DEBUG_MASK)
#define SF_GET_FILE(s)		(SF_GET (s) & SF_FILE)
#define SF_GET_STATICS(s)	(SF_GET (s) & SF_STATICS)
#define SF_GET_DEFINED(s)	(SF_GET (s) & SF_DEFINED)
#define SF_GET_STRING(s)	(SF_GET (s) & SF_STRING)
#define SF_GET_LOCAL(s)		(SF_GET (s) & SF_LOCAL)
#define SF_GET_FUNCTION(s)      (SF_GET (s) & SF_FUNCTION)
#define SF_GET_PROCESS(s)	(SF_GET (s) & SF_PROCESS)
#define SF_GET_TAGGED(s)	(SF_GET (s) & SF_TAGGED)
#define SF_GET_TAG(s)		(SF_GET (s) & SF_TAG)
#define SF_GET_GET_SEGMENT(s)	(SF_GET (s) & SF_GET_SEGMENT)
#define SF_GET_I960(s)		(SF_GET (s) & SF_I960_MASK)	/* used by i960 */
#define SF_GET_BALNAME(s)	(SF_GET (s) & SF_BALNAME)	/* used by i960 */
#define SF_GET_CALLNAME(s)	(SF_GET (s) & SF_CALLNAME)	/* used by i960 */
#define SF_GET_IS_SYSPROC(s)	(SF_GET (s) & SF_IS_SYSPROC)	/* used by i960 */
#define SF_GET_SYSPROC(s)	(SF_GET (s) & SF_SYSPROC)	/* used by i960 */

/* Modifiers */
#define SF_SET(s,v)		(SF_GET (s) = (v))
#define SF_SET_NORMAL_FIELD(s,v)(SF_GET (s) |= ((v) & SF_NORMAL_MASK))
#define SF_SET_DEBUG_FIELD(s,v)	(SF_GET (s) |= ((v) & SF_DEBUG_MASK))
#define SF_SET_FILE(s)		(SF_GET (s) |= SF_FILE)
#define SF_SET_STATICS(s)	(SF_GET (s) |= SF_STATICS)
#define SF_SET_DEFINED(s)	(SF_GET (s) |= SF_DEFINED)
#define SF_SET_STRING(s)	(SF_GET (s) |= SF_STRING)
#define SF_SET_LOCAL(s)		(SF_GET (s) |= SF_LOCAL)
#define SF_CLEAR_LOCAL(s)	(SF_GET (s) &= ~SF_LOCAL)
#define SF_SET_FUNCTION(s)      (SF_GET (s) |= SF_FUNCTION)
#define SF_SET_PROCESS(s)	(SF_GET (s) |= SF_PROCESS)
#define SF_SET_TAGGED(s)	(SF_GET (s) |= SF_TAGGED)
#define SF_SET_TAG(s)		(SF_GET (s) |= SF_TAG)
#define SF_SET_GET_SEGMENT(s)	(SF_GET (s) |= SF_GET_SEGMENT)
#define SF_SET_I960(s,v)	(SF_GET (s) |= ((v) & SF_I960_MASK))	/* used by i960 */
#define SF_SET_BALNAME(s)	(SF_GET (s) |= SF_BALNAME)	/* used by i960 */
#define SF_SET_CALLNAME(s)	(SF_GET (s) |= SF_CALLNAME)	/* used by i960 */
#define SF_SET_IS_SYSPROC(s)	(SF_GET (s) |= SF_IS_SYSPROC)	/* used by i960 */
#define SF_SET_SYSPROC(s,v)	(SF_GET (s) |= ((v) & SF_SYSPROC))	/* used by i960 */

/* --------------  Line number handling ------- */
extern int text_lineno_number;
extern int coff_line_base;

#define obj_emit_lineno(WHERE,LINE,FILE_START)	abort ()
extern void coff_add_linesym PARAMS ((struct symbol *));


void c_dot_file_symbol PARAMS ((char *filename));

#ifndef tc_coff_symbol_emit_hook
void tc_coff_symbol_emit_hook PARAMS ((/* symbolS * */));
#endif

extern void coff_frob_symbol PARAMS ((struct symbol *, int *));
extern void coff_frob_file PARAMS ((void));
extern void coff_frob_section PARAMS ((segT));
#define obj_frob_symbol(S,P) 	coff_frob_symbol(S,&P)
#define obj_frob_file()		coff_frob_file ()
#define obj_frob_section(S)	coff_frob_section (S)

extern struct symbol *coff_last_function;

/* Forward the segment of a forwarded symbol.  */
#ifndef TE_I386AIX
#define obj_frob_forward_symbol(symp) \
  (SF_GET_GET_SEGMENT (symp) \
   ? (S_SET_SEGMENT (symp, S_GET_SEGMENT (symp->sy_value.X_add_symbol)), 0) \
   : 0)
#else
#define obj_frob_forward_symbol(symp) \
  (SF_GET_GET_SEGMENT (symp) && S_GET_SEGMENT (symp) == SEG_UNKNOWN \
   ? (S_SET_SEGMENT (symp, S_GET_SEGMENT (symp->sy_value.X_add_symbol)), 0) \
   : 0)
#endif

/* sanity check */

#ifdef TC_I960
#ifndef C_LEAFSTAT
hey ! Where is the C_LEAFSTAT definition ? i960 - coff support is depending on it.
#endif /* no C_LEAFSTAT */
#endif /* TC_I960 */

/* Stabs in a coff file go into their own section.  */
#define SEPARATE_STAB_SECTIONS

/* We need 12 bytes at the start of the section to hold some initial
   information.  */
extern void obj_coff_init_stab_section PARAMS ((segT));
#define INIT_STAB_SECTION(seg) obj_coff_init_stab_section (seg)

#endif /* OBJ_FORMAT_H */
