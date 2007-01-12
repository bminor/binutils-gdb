/* spu.h -- Assembler for spu

   Copyright 2006, 2007 Free Software Foundation, Inc.

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
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#define TC_SPU

#ifdef OBJ_ELF
#define TARGET_FORMAT "elf32-spu"
#define TARGET_ARCH bfd_arch_spu
#define TARGET_NAME "elf32-spu"
#endif

#define TARGET_BYTES_BIG_ENDIAN 1

#ifndef TARGET_NAME
#define TARGET_NAME "coff-spu"
#endif

#ifndef TARGET_ARCH
#define TARGET_ARCH bfd_arch_spu
#endif

#define COFF_MAGIC SPU_MAGIC
#define BFD_ARCH bfd_arch_spu

#define NEED_FX_R_TYPE
#define TC_KEEP_FX_OFFSET
/* #define TC_CONS_RELOC RELOC_32 */

/* If defined, fixS will have a member named tc_fix_data of this type.  */
#define TC_FIX_TYPE int
#define TC_INIT_FIX_DATA(FIXP) ((FIXP)->tc_fix_data = 0)

/* Don't reduce function symbols to section symbols.  */
#define tc_fix_adjustable(FIXP) (!S_IS_FUNCTION ((FIXP)->fx_addsy))

/* Values passed to md_apply_fix don't include symbol values.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

/* The spu uses pseudo-ops with no leading period.  */
#define NO_PSEUDO_DOT 1

/* Don't warn on word overflow; it happens on %hi relocs.  */
#undef WARN_SIGNED_OVERFLOW_WORD

#ifdef OBJ_ELF
#define DIFF_EXPR_OK
#endif

#define WORKING_DOT_WORD

#define md_number_to_chars number_to_chars_bigendian

#define md_convert_frag(b,s,f)		{as_fatal (_("spu convert_frag\n"));}

/* We don't need to do anything special for undefined symbols.  */
#define md_undefined_symbol(s) 0

/* We have no special operand handling.  */
#define md_operand(e)

/* Fill in rs_align_code fragments.  */
extern void spu_handle_align PARAMS ((fragS *));
#define HANDLE_ALIGN(frag)  spu_handle_align (frag)

#define MAX_MEM_FOR_RS_ALIGN_CODE  (7 + 8)

#ifdef SPUCOFF

/* Whether a reloc should be output.  */
#define TC_COUNT_RELOC(fixp) ((fixp)->fx_addsy != NULL || (fixp)->fx_subsy != NULL)

/* Get the BFD reloc type to use for a gas fixS structure.  */
#define TC_COFF_FIX2RTYPE(fixp) tc_coff_fix2rtype (fixp)

/* No special hook needed for symbols.  */
#define tc_coff_symbol_emit_hook(s)

/* Align sections to a four byte boundary.  */
#ifndef max
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#endif
#define SUB_SEGMENT_ALIGN(SEG)	max (section_alignment[(int) (SEG)], 4)

#endif /* SPUCOFF */
