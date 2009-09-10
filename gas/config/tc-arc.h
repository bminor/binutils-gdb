/* tc-arc.h - Macros and type defines for the ARC.
   Copyright 1994, 1995, 1997, 1998, 2000, 2001, 2002, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).

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

#define TC_ARC 1

#define TARGET_BYTES_BIG_ENDIAN 0

#define LOCAL_LABELS_FB 1

#define TARGET_ARCH bfd_arch_arc

#define DIFF_EXPR_OK
#define REGISTER_PREFIX '%'

#ifdef LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif

#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif

#define LITTLE_ENDIAN   1234

#define BIG_ENDIAN      4321

/* The endianness of the target format may change based on command
   line arguments.  */
extern const char * arc_target_format;

#define DEFAULT_TARGET_FORMAT  "elf32-littlearc"
#define TARGET_FORMAT          arc_target_format
#define DEFAULT_BYTE_ORDER     LITTLE_ENDIAN
#define WORKING_DOT_WORD
#define LISTING_HEADER         "ARC GAS "

/* The ARC needs to parse reloc specifiers in .word.  */

/* We need to take care of not having section relative fixups for the 
   fixups with respect to Position Independent Code */
#define tc_fix_adjustable(X)  tc_arc_fix_adjustable(X)
extern int tc_arc_fix_adjustable (struct fix *);

extern void arc_parse_cons_expression (struct expressionS *, unsigned);
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) \
  arc_parse_cons_expression (EXP, NBYTES)

extern void arc_cons_fix_new (struct frag *, int, int, struct expressionS *);
#define TC_CONS_FIX_NEW(FRAG, WHERE, NBYTES, EXP) \
  arc_cons_fix_new (FRAG, WHERE, NBYTES, EXP)

#define GLOBAL_OFFSET_TABLE_NAME "_GLOBAL_OFFSET_TABLE_"
#define DYNAMIC_STRUCT_NAME "_DYNAMIC"

extern void arc_check_label (symbolS *labelsym);
#define tc_check_label(ls)      arc_check_label (ls)

/* This hook is required to parse register names as operands. */
#define md_parse_name(name, exp, m, c) arc_parse_name (name, exp)
extern int arc_parse_name (const char *, struct expressionS *);

/* Define this macro to enforce non-register operands to be prefixed with
 * the @ character.  */
/* #define ENFORCE_AT_PREFIX */

#define DWARF2_LINE_MIN_INSN_LENGTH (arc_mach_a4 ? 4 : 2)

/* Values passed to md_apply_fix don't include the symbol value.  */
#define MD_APPLY_SYM_VALUE(FIX) 0

/* No shared lib support, so we don't need to ensure externally
   visible symbols can be overridden.  */
#define EXTERN_FORCE_RELOC 0

#include "opcode/arc.h" /* for arc_insn */

struct enriched_insn
{
  arc_insn insn;
  unsigned short delay_slot;
  unsigned short limm;
  asymbol *sym;
};

struct loop_target
{
  /* Pointer to the symbol.  */
  asymbol* symbol;
  
  /* Contains the last two instructions before the loop target.  */
  struct enriched_insn prev_two_insns[2];
};

/* Extra stuff that we need to keep track of for each symbol.  */
struct arc_tc_sy
{
  struct loop_target loop_target;
#if 0
  /* The real name, if the symbol was renamed.  */
  char *real_name;
#endif
};

#define TC_SYMFIELD_TYPE struct arc_tc_sy
#define tc_symbol_new_hook(symbolP) (symbolP)->sy_tc.loop_target.symbol = 0;

#if 0
/* Finish up the symbol.  */
extern int arc_frob_symbol (struct symbol *);
#define tc_frob_symbol(sym, punt) punt = arc_frob_symbol (sym)
#endif

/* To handle alignment.  */
/* Used to restrict the amount of memory allocated for representing
   the alignment code.  */
#define MAX_MEM_FOR_RS_ALIGN_CODE (arc_mach_a4 ? 3+4 : 1+2)
/* HANDLE_ALIGN called after all the assembly has been done,
   so we can fill in all the rs_align_code type frags with
   nop instructions.  */
#define HANDLE_ALIGN(FRAGP)	 arc_handle_align(FRAGP)
extern void arc_handle_align (fragS* fragP);


/* register class field size in extension section */
#define RCLASS_SET_SIZE 4
/* operand format field size in extension section */
#define OPD_FORMAT_SIZE 8

#define	FLAG_3OP	 0x1
#define FLAG_2OP	 0x2
#define FLAG_NOP	 0x4
#define FLAG_1OP	 0x8
#define FLAG_IGN_DEST	 0x10
#define FLAG_4OP_U9	 0x20
#define FLAG_FLAG	 0x40     // Flag bit is part of instruction encoding
#define FLAG_3OP_U8	 0x80     // SIMD engine decode for VLD/VST/VMOV

#define FLAG_SCALE_1	 0x100 // optional scale factors for SIMD U8 encoding
#define FLAG_SCALE_2	 0x200
#define FLAG_SCALE_3	 0x400
#define FLAG_SCALE_4	 0x800
#define FLAG_EXT_S16	 0x1000
#define FLAG_AS		 0x2000
#define FLAG_AP		 0x4000
#define FLAG_AM		 0x6000

#define FLAG_FMT1	 0x10000
#define FLAG_FMT2	 0x20000
#define FLAG_FMT3	 0x30000

