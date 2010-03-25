/* Definitions for TI C6X assembler.
   Copyright 2010
   Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#define TC_TIC6X 1
#define TARGET_BYTES_BIG_ENDIAN 0
#define WORKING_DOT_WORD
#define DOUBLEBAR_PARALLEL
#define DWARF2_LINE_MIN_INSN_LENGTH 2
#define MD_APPLY_SYM_VALUE(FIX) 0
#define TC_PREDICATE_START_CHAR '['
#define TC_PREDICATE_END_CHAR ']'
/* For TI C6X, we keep spaces in what the preprocessor considers
   operands as they may separate functional unit specifiers from
   operands.  */
#define TC_KEEP_OPERAND_SPACES 1

#define TARGET_ARCH 	bfd_arch_tic6x
#define TARGET_FORMAT	(target_big_endian	\
			 ? "elf32-tic6x-be"	\
			 : "elf32-tic6x-le")

typedef struct
{
  /* Number of instructions in the current execute packet.  */
  unsigned int num_execute_packet_insns;

  /* Whether a label has been seen since the last instruction or data
     (in which case a following instruction may not have parallel
     bars, but must start a new execute packet).  */
  bfd_boolean seen_label;

  /* Whether compact instructions are forbidden here.  */
  bfd_boolean nocmp;

  /* If there is a current execute packet, a pointer to the
     least-significant byte of the last instruction in it (for setting
     the p-bit).  */
  char *last_insn_lsb;

  /* If there has been an SPMASK instruction in the current execute
     packet, a pointer to the first byte in it (for processing
     ||^); otherwise NULL.  */
  char *spmask_addr;

  /* If an SPLOOP-family instruction has been seen, and a following
     SPKERNEL-family instruction has not yet been seen, the ii value
     from the SPLOOP instruction (in the range 1 to 14); otherwise
     0.  */
  int sploop_ii;
} tic6x_segment_info_type;
#define TC_SEGMENT_INFO_TYPE tic6x_segment_info_type

typedef struct
{
  /* Whether this fix was for an ADDA instruction.  If so, a constant
     resulting from resolving the fix should be implicitly shifted
     left (it represents a value to be encoded literally in the
     instruction, whereas a non-constant represents a DP-relative
     value counting in the appropriate units).  */
  bfd_boolean fix_adda;
} tic6x_fix_info;
#define TC_FIX_TYPE tic6x_fix_info
#define TC_INIT_FIX_DATA(fixP) tic6x_init_fix_data (fixP)
struct fix;
extern void tic6x_init_fix_data (struct fix *fixP);

#define md_after_parse_args() tic6x_after_parse_args ()
extern void tic6x_after_parse_args (void);

#define md_cleanup() tic6x_cleanup ()
extern void tic6x_cleanup (void);

#define md_cons_align(n) tic6x_cons_align (n)
extern void tic6x_cons_align (int n);

#define md_parse_name(name, exprP, mode, nextcharP)	\
  tic6x_parse_name (name, exprP, mode, nextcharP)
extern int tic6x_parse_name (const char *name, expressionS *exprP,
			     enum expr_mode mode, char *nextchar);

#define md_start_line_hook() tic6x_start_line_hook ()
extern void tic6x_start_line_hook (void);

#define TC_CONS_FIX_NEW(frag, where, size, exp)	\
  tic6x_cons_fix_new (frag, where, size, exp)
extern void tic6x_cons_fix_new (fragS *frag, int where, int size,
				expressionS *exp);

#define tc_frob_label(sym) tic6x_frob_label (sym)
extern void tic6x_frob_label (symbolS *sym);

#define tc_unrecognized_line(c) tic6x_unrecognized_line (c)
extern int tic6x_unrecognized_line (int c);
