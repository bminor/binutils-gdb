/* tc-mips.c -- header file for tc-mips.c.
   Copyright (C) 1993 Free Software Foundation, Inc.
   Contributed by the OSF and Ralph Campbell.
   Written by Keith Knowles and Ralph Campbell, working independently.
   Modified for ECOFF support by Ian Lance Taylor of Cygnus Support.

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

#define TC_MIPS

#define TARGET_ARCH bfd_arch_mips

#define ONLY_STANDARD_ESCAPES
#define BACKSLASH_V
#define WORKING_DOT_WORD	1
#define OLD_FLOAT_READS
#define REPEAT_CONS_EXPRESSIONS
#define LOCAL_LABELS_FB

#define LOCAL_LABEL(name) ((name)[0] == '$')

#define md_undefined_symbol(name)	(0)
#define md_operand(x)

#define LITTLE_ENDIAN   1234
#define BIG_ENDIAN      4321

/* If neither TARGET_BYTES_BIG_ENDIAN nor TARGET_BYTES_LITTLE_ENDIAN
   is specified, default to big endian.  */
#ifndef TARGET_BYTES_BIG_ENDIAN
#ifndef TARGET_BYTES_LITTLE_ENDIAN
#define TARGET_BYTES_BIG_ENDIAN
#endif
#endif

#ifdef TARGET_BYTES_BIG_ENDIAN
#define BYTE_ORDER	BIG_ENDIAN
#else
#define BYTE_ORDER      LITTLE_ENDIAN
#endif

#ifndef TARGET_FORMAT 
#ifdef OBJ_AOUT
#ifdef TARGET_BYTES_BIG_ENDIAN
#define TARGET_FORMAT "aout-mips-big"
#else
#define TARGET_FORMAT "aout-mips-little"
#endif
#endif /* OBJ_AOUT */
#ifdef OBJ_ECOFF
#ifdef TARGET_BYTES_BIG_ENDIAN
#define TARGET_FORMAT "ecoff-bigmips"
#else
#define TARGET_FORMAT "ecoff-littlemips"
#endif
#endif /* OBJ_ECOFF */
#ifdef OBJ_ELF
#ifdef TARGET_BYTES_BIG_ENDIAN
#define TARGET_FORMAT "elf32-bigmips"
#else
#define TARGET_FORMAT "elf32-littlemips"
#endif
#endif /* OBJ_ELF */
#endif /* ! defined (TARGET_FORMAT) */

struct mips_cl_insn {
    unsigned long		insn_opcode;
    const struct mips_opcode	*insn_mo;
};

#ifndef BFD_ASSEMBLER
#define md_convert_frag(h,f)		{as_fatal ("MIPS convert_frag\n");}
#else
#define md_convert_frag(b,s,f)		{as_fatal ("MIPS convert_frag\n");}
#endif

extern int tc_get_register PARAMS ((void));

#define tc_frob_label(sym) mips_define_label (sym)
extern void mips_define_label PARAMS ((struct symbol *));

#define TC_CONS_FIX_NEW cons_fix_new_mips
extern void cons_fix_new_mips ();
