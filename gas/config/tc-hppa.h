/* tc-hppa.h -- Header file for the PA */

/* Copyright (C) 1989, 1993 Free Software Foundation, Inc.

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


/*
   HP PA-RISC support was contributed by the Center for Software Science
   at the University of Utah.
 */

#ifndef _TC_HPPA_H
#define _TC_HPPA_H

#ifndef TC_HPPA
#define TC_HPPA	1
#endif

#ifdef OBJ_ELF
#include "../bfd/elf32-hppa.h"
#endif

#define TARGET_ARCH bfd_arch_hppa
#define TARGET_FORMAT "elf32-hppa"

/* Local labels have an "L$" prefix.  */
#define LOCAL_LABEL(name) ((name)[0] == 'L' && (name)[1] == '$')
#define ASEC_NULL (asection *)0

/* We can do sym1 - sym2 as long as sym2 is $global$ */

#define SEG_DIFF_ALLOWED

typedef enum FPOF
  {
    SGL, DBL, ILLEGAL_FMT, QUAD
  } FP_Operand_Format;

extern char *expr_end;

extern void s_globl (), s_long (), s_short (), s_space (), cons ();
extern void stringer ();
extern unsigned int next_char_of_string ();

extern void pa_big_cons ();
extern void pa_cons ();
extern void pa_data ();
extern void pa_desc ();
extern void pa_float_cons ();
extern void pa_fill ();
extern void pa_lcomm ();
extern void pa_lsym ();
extern void pa_stringer ();
extern void pa_text ();
extern void pa_version ();

int pa_parse_number ();

int pa_parse_fp_cmp_cond ();

FP_Operand_Format pa_parse_fp_format ();

#ifdef __STDC__
int getExpression (char *str);
#else
int getExpression ();
#endif

int getAbsoluteExpression ();

int evaluateAbsolute ();

int pa_build_arg_reloc ();

unsigned int pa_align_arg_reloc ();

void pa_skip ();

int pa_parse_nullif ();

int pa_parse_nonneg_cmpsub_cmpltr ();

int pa_parse_neg_cmpsub_cmpltr ();

int pa_parse_nonneg_add_cmpltr ();

int pa_parse_neg_add_cmpltr ();

int pa_build_arg_reloc ();

void s_seg (), s_proc (), s_data1 ();

void pa_block (), pa_call (), pa_call_args (), pa_callinfo ();
void pa_code (), pa_comm (), pa_copyright (), pa_end ();
void pa_enter ();
void pa_entry (), pa_equ (), pa_exit (), pa_export ();
void pa_export_args (), pa_import (), pa_label (), pa_leave ();
void pa_origin (), pa_proc (), pa_procend (), pa_space ();
void pa_spnum (), pa_subspace (), pa_version ();
void pa_param();

extern const pseudo_typeS md_pseudo_table[];

/*
  PA-89 floating point registers are arranged like this:


  +--------------+--------------+
  |   0 or 16L   |  16 or 16R   |
  +--------------+--------------+
  |   1 or 17L   |  17 or 17R   |
  +--------------+--------------+
  |              |              |

  .              .              .
  .              .              .
  .              .              .

  |              |              |
  +--------------+--------------+
  |  14 or 30L   |  30 or 30R   |
  +--------------+--------------+
  |  15 or 31L   |  31 or 31R   |
  +--------------+--------------+


  The following is a version of pa_parse_number that
  handles the L/R notation and returns the correct
  value to put into the instruction register field.
  The correct value to put into the instruction is
  encoded in the structure 'pa_89_fp_reg_struct'.

 */

struct pa_89_fp_reg_struct
  {
    char number_part;
    char L_R_select;
  };

int need_89_opcode ();
int pa_89_parse_number ();


struct call_desc
  {
    unsigned int arg_reloc;
    unsigned int arg_count;
  };

typedef struct call_desc call_descS;

extern call_descS last_call_desc;

/* GDB debug support */

#if defined(OBJ_SOME)
#define GDB_DEBUG_SPACE_NAME "$GDB_DEBUG$"
#define GDB_STRINGS_SUBSPACE_NAME "$GDB_STRINGS$"
#define GDB_SYMBOLS_SUBSPACE_NAME "$GDB_SYMBOLS$"
#else
#define GDB_DEBUG_SPACE_NAME ".stab"
#define GDB_STRINGS_SUBSPACE_NAME ".stabstr"
#define GDB_SYMBOLS_SUBSPACE_NAME ".stab"
#endif
/* pre-defined subsegments (subspaces) for the HP 9000 Series 800 */

#define SUBSEG_CODE   0
#define SUBSEG_DATA   0
#define SUBSEG_LIT    1
#define SUBSEG_BSS    2
#define SUBSEG_UNWIND 3
#define SUBSEG_GDB_STRINGS 0
#define SUBSEG_GDB_SYMBOLS 1

#define UNWIND_SECTION_NAME	".hppa_unwind"
/* subspace dictionary chain entry structure */

struct subspace_dictionary_chain
  {
#if defined(OBJ_OSFROSE) | defined(OBJ_ELF)
#ifdef OBJ_OSFROSE
    region_command_t *ssd_entry;/* XXX: not sure this is what we need here */
#else
    Elf_Internal_Shdr *ssd_entry;
    unsigned long ssd_vm_addr;
#endif
    char *ssd_name;		/* used until time of writing object file	*/
    /* then we use ssd_entry->regc_region_name	*/
    unsigned char ssd_quadrant;
    unsigned char ssd_sort_key;
    unsigned char ssd_common;
    unsigned char ssd_dup_common;
    unsigned char ssd_loadable;
    unsigned char ssd_code_only;
#else
    subspace_dictS *ssd_entry;	/* this dictionary */
#endif
    int ssd_defined;		/* this subspace has been used */
    int ssd_space_number;	/* space # this subspace is in */
    asection *ssd_seg;		/* this subspace =  this seg */
    int ssd_subseg;		/*                  and subseg */
    int ssd_zero;
    int object_file_index;	/* index of this entry within
                                                     the subspace dictionary of
                                                     the object file (not used until
                                                     the object file is written */
    int ssd_last_align;		/* the size of the last alignment
                                                     request for this subspace */
    struct symbol *ssd_start_sym; /* a symbol whose value is the
                                                     start of this subspace */
    struct subspace_dictionary_chain *ssd_next;	/* next subspace dict. entry */
  };

typedef struct subspace_dictionary_chain subspace_dict_chainS;

/* space dictionary chain entry structure */

struct space_dictionary_chain
  {
#ifdef OBJ_OSFROSE
    region_command_t *sd_entry;	/* XXX: not sure this is what we need here */
    char *sd_name;		/* used until time of writing object file	*/
    /* then we use sd_entry->regc_region_name	*/
    unsigned int sd_loadable;
    unsigned int sd_private;
    unsigned int sd_spnum;
    unsigned char sd_sort_key;
#else
#ifdef OBJ_ELF
    Elf_Internal_Shdr *sd_entry;
    char *sd_name;		/* used until time of writing object file	*/
    /* then we use sd_entry->sh_name	*/
    unsigned int sd_loadable;
    unsigned int sd_private;
    unsigned int sd_spnum;
    unsigned char sd_sort_key;
#else
    space_dictS *sd_entry;	/* this dictionary */
#endif
#endif
    int sd_defined;		/* this space has been used */
    asection *sd_seg;		/* GAS segment to which this space corresponds */
    int sd_last_subseg;		/* current subsegment number we are using */
    subspace_dict_chainS *sd_subspaces;	/* all subspaces in this space */
    struct space_dictionary_chain *sd_next;	/* the next space dict. entry */
  };

typedef struct space_dictionary_chain space_dict_chainS;

/*
    Macros to maintain spaces and subspaces
 */

#ifdef OBJ_OSFROSE
#define SPACE_DEFINED(space_chain)	(space_chain)->sd_defined
#define SPACE_PRIVATE(space_chain)	(space_chain)->sd_private
#define SPACE_LOADABLE(space_chain)	(space_chain)->sd_loadable
#define SPACE_SPNUM(space_chain)	(space_chain)->sd_spnum
#define SPACE_SORT(space_chain)		(space_chain)->sd_sort_key
#define SPACE_NAME(space_chain)		(space_chain)->sd_name

#define SUBSPACE_QUADRANT(ss_chain)	(ss_chain)->ssd_quadrant
#define SUBSPACE_ALIGN(ss_chain)	(ss_chain)->ssd_entry->regc_addralign
#define SUBSPACE_ACCESS(ss_chain)	(ss_chain)->ssd_entry->regc_initprot
#define SUBSPACE_SORT(ss_chain)		(ss_chain)->ssd_sort_key
#define SUBSPACE_COMMON(ss_chain)	(ss_chain)->ssd_common
#define SUBSPACE_ZERO(ss_chain)		(ss_chain)->ssd_zero
#define SUBSPACE_DUP_COMM(ss_chain)	(ss_chain)->ssd_dup_common
#define SUBSPACE_CODE_ONLY(ssch)	((ssch)->ssd_entry->regc_flags & REG_TEXT_T)
#define SET_SUBSPACE_CODE_ONLY(ssch,val) (ssch)->ssd_entry->regc_flags |= ((val) ? REG_TEXT_T : 0)
#define SUBSPACE_LOADABLE(ss_chain)	(ss_chain)->ssd_loadable
#define SUBSPACE_SUBSPACE_START(ss_chain) (ss_chain)->ssd_entry->regc_addr.vm_addr
#define SUBSPACE_SUBSPACE_LENGTH(ss_chain) (ss_chain)->ssd_entry->regc_vm_size
#define SUBSPACE_REGION_NAME(ss_chain)	(ss_chain)->ssd_entry->regc_region_name
#define SUBSPACE_NAME(ss_chain)		(ss_chain)->ssd_name
#endif

#ifdef OBJ_ELF
#define RELOC_EXPANSION_POSSIBLE
#define MAX_RELOC_EXPANSION 5

#define SPACE_DEFINED(space_chain)	(space_chain)->sd_defined
#define SPACE_PRIVATE(space_chain)	(space_chain)->sd_private
#define SPACE_LOADABLE(space_chain)	(space_chain)->sd_loadable
#define SPACE_SPNUM(space_chain)	(space_chain)->sd_spnum
#define SPACE_SORT(space_chain)		(space_chain)->sd_sort_key
#define SPACE_NAME(space_chain)		(space_chain)->sd_name

#define SUBSPACE_QUADRANT(ss_chain)	(ss_chain)->ssd_quadrant
#define SUBSPACE_ALIGN(ss_chain)	(ss_chain)->ssd_entry->sh_addralign
#define SUBSPACE_ACCESS(ss_chain)	(ss_chain)->ssd_entry->sh_flags
#define SUBSPACE_SORT(ss_chain)		(ss_chain)->ssd_sort_key
#define SUBSPACE_COMMON(ss_chain)	(ss_chain)->ssd_common
#define SUBSPACE_ZERO(ss_chain)		(ss_chain)->ssd_zero
#define SUBSPACE_DUP_COMM(ss_chain)	(ss_chain)->ssd_dup_common
#define SUBSPACE_CODE_ONLY(ssch) \
	(((ssch)->ssd_entry->sh_flags & (SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE)) \
	 == (SHF_ALLOC | SHF_EXECINSTR))
#define SET_SUBSPACE_CODE_ONLY(ssch,val) \
		(ssch)->ssd_entry->sh_flags &= ((val) ? ~SHF_WRITE : 0xffffffff)
#define SUBSPACE_LOADABLE(ss_chain)	(ss_chain)->ssd_loadable
#define SUBSPACE_SUBSPACE_START(ss_chain) (ss_chain)->ssd_vm_addr
#define SUBSPACE_SUBSPACE_LENGTH(ss_chain) (ss_chain)->ssd_entry->sh_size
#define SUBSPACE_NAME(ss_chain)		(ss_chain)->ssd_name

#define STAB_FIXUP(frag,toptr,symP,stab_type)	\
		if ( (stab_type == 's' || stab_type == 'n')	\
		    && symP->sy_value.X_op == O_symbol)		\
		  {						\
		     int i = S_GET_TYPE(symP) & N_TYPE;		\
		     fix_new_hppa(frag,  			\
				  toptr-frag->fr_literal, /* where */	\
				  4,		/* size */	\
				  symP->sy_value.X_add_symbol,	/* addr of sym for this stab */	\
				  (offsetT) 0,			\
				  (expressionS *) NULL,		\
				  i == N_UNDF || i == N_ABS,	/* 1 if internal reloc */	\
				  R_HPPA,	/* type */	\
				  e_fsel,	/* fixup fld = F% */	\
				  32,				\
				  0,		/* arg_reloc */	\
				  (char *)0			\
				  );				\
		  }						\
		else if ( stab_type == 'd' )			\
		  {						\
		     fix_new_hppa (frag,			\
				   toptr-frag->fr_literal, /* where */	\
				   4,		/* size */	\
				   symP,	/* addr of sym for this stab */	\
				   (offsetT) 0,			\
				   (expressionS *) NULL,	\
				   0,				\
				   R_HPPA,	/* type */	\
				   e_fsel,	/* fixup fld = F% */	\
				   32,				\
				   0,		/* arg_reloc */	\
				   (char *)0			\
				   );				\
		  }

#endif

#ifdef OBJ_SOM
#define SPACE_DEFINED(space_chain)	(space_chain)->sd_entry->is_defined
#define SPACE_PRIVATE(space_chain)	(space_chain)->sd_entry->is_private
#define SPACE_LOADABLE(space_chain)	(space_chain)->sd_entry->is_loadable
#define SPACE_SPNUM(space_chain)	(space_chain)->sd_entry->space_number
#define SPACE_SORT(space_chain)		(space_chain)->sd_entry->sort_key
#define SPACE_NAME(space_chain)		(space_chain)->sd_entry->name

#define SUBSPACE_QUADRANT(ss_chain)	(ss_chain)->ssd_entry->quadrant
#define SUBSPACE_ALIGN(ss_chain)	(ss_chain)->ssd_entry->alignment
#define SUBSPACE_ACCESS(ss_chain)	(ss_chain)->ssd_entry->access_control_bits
#define SUBSPACE_SORT(ss_chain)		(ss_chain)->ssd_entry->sort_key
#define SUBSPACE_COMMON(ss_chain)	(ss_chain)->ssd_entry->is_common
#define SUBSPACE_ZERO(ss_chain)		(ss_chain)->ssd_zero
#define SUBSPACE_DUP_COMM(ss_chain)	(ss_chain)->ssd_entry->dup_common
#define SUBSPACE_CODE_ONLY(ss_chain)	(ss_chain)->ssd_entry->code_only
#define SUBSPACE_LOADABLE(ss_chain)	(ss_chain)->ssd_entry->is_loadable
#define SUBSPACE_SUBSPACE_START(ss_chain) (ss_chain)->ssd_entry->subspace_start
#define SUBSPACE_SUBSPACE_LENGTH(ss_chain) (ss_chain)->ssd_entry->subspace_length
#define SUBSPACE_NAME(ss_chain)		(ss_chain)->ssd_entry->name
#endif

extern space_dict_chainS *space_dict_root;
extern space_dict_chainS *space_dict_last;

extern space_dict_chainS *current_space;
extern subspace_dict_chainS *current_subspace;

extern space_dict_chainS *create_new_space ();

extern subspace_dict_chainS *create_new_subspace ();

extern subspace_dict_chainS *update_subspace ();

extern space_dict_chainS *is_defined_space ();

extern space_dict_chainS *pa_segment_to_space ();

extern subspace_dict_chainS *is_defined_subspace ();

extern subspace_dict_chainS *pa_subsegment_to_subspace ();

extern space_dict_chainS *pa_find_space_by_number ();

extern unsigned int pa_subspace_start ();

extern int is_last_defined_subspace ();

/* symbol support */

extern struct symbol *pa_get_start_symbol ();

extern struct symbol *pa_set_start_symbol ();

/* default space and subspace dictionaries */

struct default_subspace_dict
  {
    char *name;
    char defined;
    char loadable, code_only, common, dup_common, zero;
    unsigned char sort;
    int access, space_index, alignment, quadrant;
#ifdef OBJ_SOM
    segT segment;
#else
    int def_space_index;	/* this is an index in the default spaces array */
    char *alias;		/* an alias for this section (or NULL if there isn't one) */
#endif
    subsegT subsegment;
  };

extern struct default_subspace_dict pa_def_subspaces[];

struct default_space_dict
  {
    char *name;
    int spnum;
    char loadable;
    char defined;
    char private;
    unsigned char sort;
#ifdef OBJ_SOM
    segT segment;
#else
    asection *segment;
    /* an alias for this section (or NULL if there isn't one) */
    char *alias;
#endif
  };

extern struct default_space_dict pa_def_spaces[];

/*
	Support for keeping track of the most recent label in each
	space.
 */

#define tc_frob_label(sym) pa_define_label (sym)

typedef struct label_symbol_struct
  {
    /* the label symbol */
    struct symbol *lss_label;
    /* the space to which it applies */
    space_dict_chainS *lss_space;
    /* the next label symbol */
    struct label_symbol_struct *lss_next;
  } label_symbolS;

void pa_define_label ();

/* end of label symbol support. */

#define is_DP_relative(exp)			\
  ((exp).X_op == O_subtract			\
   && strcmp((exp).X_op_symbol->bsym->name, "$global$") == 0)

#define is_PC_relative(exp)			\
  ((exp).X_op == O_subtract			\
   && strcmp((exp).X_op_symbol->bsym->name, "$PIC_pcrel$0") == 0)

#define is_complex(exp)				\
  ((exp).X_op != O_constant && (exp).X_op != O_symbol)

#define tc_crawl_symbol_chain(headers) {;}	/* Not used. */

#define tc_headers_hook(headers) {;}	/* Not used. */

#define elf_tc_symbol		hppa_tc_symbol
#define elf_tc_make_sections	hppa_tc_make_sections
extern void elf_hppa_final_processing ();
#define elf_tc_final_processing	elf_hppa_final_processing

/* We need to parse field selectors in .byte, etc.  */

#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) \
  parse_cons_expression_hppa (EXP)
#define TC_CONS_FIX_NEW cons_fix_new_hppa

/* FIXME these used to be prototypes, but both want an expressionS which
   is not defined when this file is included.  */
extern void parse_cons_expression_hppa ();
extern void cons_fix_new_hppa ();

/* On the PA, an equal sign often appears as a condition or nullification
   completer in an instruction.  This can be detected by checking the
   previous character, if the character is a comma, then the equal is
   being used as part of an instruction.  */
#define TC_EQUAL_IN_INSN(C, PTR)	((C) == ',')

/* Similarly for an exclamation point.  It is used in FP comparison
   instructions and as an end of line marker.  When used in an instruction
   it will always follow a comma.  */
#define TC_EOL_IN_INSN(PTR)	(is_end_of_line[*(PTR)] && (PTR)[-1] == ',')

#endif /* _TC_HPPA_H */
