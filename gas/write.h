/* write.h

   Copyright (C) 1987, 1992, 1993 Free Software Foundation, Inc.

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

#ifndef TC_I960
#ifdef hpux
#define EXEC_MACHINE_TYPE HP9000S200_ID
#endif
#endif /* TC_I960 */

#ifndef LOCAL_LABEL
#ifdef DOT_LABEL_PREFIX
#define LOCAL_LABEL(name) (name[0] =='.' \
			   && ( name [1] == 'L' || name [1] == '.' ))
#else /* not defined DOT_LABEL_PREFIX */
#define LOCAL_LABEL(name) (name [0] == 'L' )
#endif /* not defined DOT_LABEL_PREFIX */
#endif /* LOCAL_LABEL */

#define S_LOCAL_NAME(s) (LOCAL_LABEL(S_GET_NAME(s)))

#include "bit_fix.h"

/*
 * FixSs may be built up in any order.
 */

struct fix
  {
    /* These small fields are grouped together for compactness of
       this structure, and efficiency of access on some architectures.  */
    char fx_pcrel_adjust;	/* pc-relative offset adjust */
    unsigned fx_pcrel : 1;	/* TRUE: pc-relative. */
    unsigned fx_im_disp : 1;	/* TRUE: value is a displacement */
    unsigned fx_bsr : 1;	/* sequent-hack */
    /* TRUE if target is a 'callj' (used by i960) */
    unsigned fx_callj : 1;
    short int fx_size;		/* How many bytes are involved? */

    fragS *fx_frag;		/* Which frag? */
    long fx_where;		/* Where is the 1st byte to fix up? */
    symbolS *fx_addsy;		/* NULL or Symbol whose value we add in. */
    symbolS *fx_subsy;		/* NULL or Symbol whose value we subtract. */
    valueT fx_offset;		/* Absolute number we add in. */
    struct fix *fx_next;	/* NULL or -> next fixS. */
    bit_fixS *fx_bit_fixP;	/* IF NULL no bitfix's to do */
#ifdef BFD_ASSEMBLER
    bfd_reloc_code_real_type fx_r_type;
#else
#ifdef NEED_FX_R_TYPE
    /* Hack for machines where the type of reloc can't be
       worked out by looking at how big it is.  */
    int fx_r_type;
#endif
#endif

    /* This field is sort of misnamed.  It appears to be a sort of random
       scratch field, for use by the back ends.  The main gas code doesn't
       do anything but initialize it to zero.  The use of it does need to
       be coordinated between the cpu and format files, though.  E.g., some
       coff targets pass the `addend' field from the cpu file via this
       field.  I don't know why the `fx_offset' field above can't be used
       for that; investigate later and document. KR  */
    valueT fx_addnumber;
  };

typedef struct fix fixS;

#ifndef BFD_ASSEMBLER
extern char *next_object_file_charP;

#ifndef MANY_SEGMENTS
COMMON fixS *text_fix_root, *text_fix_tail;	/* Chains fixSs. */
COMMON fixS *data_fix_root, *data_fix_tail;	/* Chains fixSs. */
COMMON fixS *bss_fix_root, *bss_fix_tail;	/* Chains fixSs. */
#endif
COMMON fixS **seg_fix_rootP, **seg_fix_tailP;	/* -> one of above. */
#endif

extern long string_byte_count;
extern int section_alignment[];

bit_fixS *bit_fix_new PARAMS ((int size, int offset, long base_type,
			       long base_adj, long min, long max, long add));
void append PARAMS ((char **charPP, char *fromP, unsigned long length));
void record_alignment PARAMS ((segT seg, int align));
void write_object_file PARAMS ((void));
void relax_segment PARAMS ((struct frag * seg_frag_root, segT seg_type));

#ifdef BFD_ASSEMBLER
fixS *fix_new PARAMS ((fragS * frag, int where, int size,
		       symbolS * add_symbol, offsetT offset, int pcrel,
		       bfd_reloc_code_real_type r_type));
fixS *fix_new_exp PARAMS ((fragS * frag, int where, int size,
			   expressionS *exp, int pcrel,
			   bfd_reloc_code_real_type r_type));
#else
fixS *fix_new PARAMS ((fragS * frag, int where, int size,
		       symbolS * add_symbol, offsetT offset, int pcrel,
		       int r_type));
fixS *fix_new_exp PARAMS ((fragS * frag, int where, int size,
			   expressionS *exp, int pcrel, int r_type));
#endif

/* end of write.h */
