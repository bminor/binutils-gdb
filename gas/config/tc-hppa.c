/* tc-hppa.c -- Assemble for the PA
   Copyright (C) 1989 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <ctype.h>

#include "as.h"
#include "subsegs.h"

/* #include "../bfd/libhppa.h" */
/*
 * Unwind table and descriptor.
 */

struct unwind_desc
  {
    unsigned int cannot_unwind:1;
    unsigned int millicode:1;
    unsigned int millicode_save_rest:1;
    unsigned int region_desc:2;
    unsigned int save_sr:2;
    unsigned int entry_fr:4;	/* number saved */
    unsigned int entry_gr:5;	/* number saved */
    unsigned int args_stored:1;
    unsigned int call_fr:5;
    unsigned int call_gr:5;
    unsigned int save_sp:1;
    unsigned int save_rp:1;
    unsigned int save_rp_in_frame:1;
    unsigned int extn_ptr_defined:1;
    unsigned int cleanup_defined:1;

    unsigned int hpe_interrupt_marker:1;
    unsigned int hpux_interrupt_marker:1;
    unsigned int reserved:3;
    unsigned int frame_size:27;
  };

typedef struct unwind_desc unwind_descS;

struct unwind_table
  {
    unsigned int start_offset;	/* starting offset (from SR4) of applicable region */
    unsigned int end_offset;	/* ending offset (from SR4) of applicable region */
    unwind_descS descriptor;
  };

typedef struct unwind_table unwind_tableS;

/*
 * This structure is used by the .callinfo, .enter, .leave pseudo-ops to
 * control the entry and exit code they generate. It is also used in
 * creation of the correct stack unwind descriptors.
 *
 * The fields in structure roughly correspond to the arguments available on the
 * .callinfo pseudo-op.
 */

struct call_info
  {
    int frame;
    int entry_sr;
    int makes_calls;
    int hpux_int;
    unwind_tableS ci_unwind;	/* the unwind descriptor we are building */
    symbolS *start_symbol;	/* name of function (used in relocation info) */
    symbolS *end_symbol;	/* temporary symbol used to mark the */
    /* end of the function (used in */
    /* relocation info) */
    fragS *start_frag;		/* frag associated w/ start of this function */
    fragS *end_frag;		/* frag associated w/ end of this function */
    fragS *start_offset_frag;	/* frag for start offset of this descriptor */
    int start_frag_where;	/* where in start_offset_frag is start_offset */
    fixS *start_fix;		/* fixup for the start_offset */
    fragS *end_offset_frag;	/* frag for start offset of this descriptor */
    int end_frag_where;		/* where in end_offset_frag is end_offset */
    fixS *end_fix;		/* fixup for the end_offset */
    struct call_info *ci_next;	/* the next call_info structure */
  };

typedef struct call_info call_infoS;

call_infoS *last_call_info;
call_infoS *call_info_root;
call_descS last_call_desc;

/* A structure used during assembly of individual instructions */

struct pa_it
  {
    char *error;
    unsigned long opcode;
    /* symbol_dictS *nlistp; *//*** used to be:    struct nlist *nlistp; */
    asymbol *nlistp;
    expressionS exp;
    int pcrel;
    FP_Operand_Format fpof1;	/* Floating Point Operand Format, operand 1 */
    FP_Operand_Format fpof2;	/* Floating Point Operand Format, operand 2 */
    /* (used only for class 1 instructions --   */
    /* the conversion instructions)             */
#ifdef OBJ_SOM
    long field_selector;
    unsigned int reloc;
    int code;
    long arg_reloc;
    unwind_descS unwind;
#endif
#ifdef OBJ_ELF
    elf32_hppa_reloc_type reloc;
    long field_selector;
    int format;
    long arg_reloc;
    unwind_descS unwind;
#endif
  };

extern struct pa_it the_insn;

/* careful, this file includes data *declarations* */
#include "opcode/hppa.h"

void md_begin ();
void md_end ();
void md_number_to_chars ();
void md_assemble ();
char *md_atof ();
void md_convert_frag ();
void md_create_short_jump ();
void md_create_long_jump ();
int md_estimate_size_before_relax ();
void md_number_to_imm ();
void md_number_to_disp ();
void md_number_to_field ();
void md_ri_to_chars ();
void emit_relocations ();
static void pa_ip ();

const relax_typeS md_relax_table[] =
{0};
/* handle of the OPCODE hash table */
static struct hash_control *op_hash = NULL;

int md_short_jump_size = 4;
int md_long_jump_size = 4;

/* This array holds the chars that always start a comment.  If the
    pre-processor is disabled, these aren't very useful */
const char comment_chars[] = ";";	/* JF removed '|' from comment_chars */

const pseudo_typeS
  md_pseudo_table[] =
{
  {"align", s_align_bytes, 0},	/* .align 4 means .align to 4-byte boundary */
  {"ALIGN", s_align_bytes, 0},	/* .align 4 means .align to 4-byte boundary */
  {"block", pa_block, 1},
  {"BLOCK", pa_block, 1},
  {"blockz", pa_block, 0},
  {"BLOCKZ", pa_block, 0},
  {"byte", pa_cons, 1},
  {"BYTE", pa_cons, 1},
  {"call", pa_call, 0},
  {"CALL", pa_call, 0},
  {"callinfo", pa_callinfo, 0},
  {"CALLINFO", pa_callinfo, 0},
  {"code", pa_code, 0},
  {"CODE", pa_code, 0},
  {"comm", pa_comm, 0},
  {"COMM", pa_comm, 0},
  {"copyright", pa_copyright, 0},
  {"COPYRIGHT", pa_copyright, 0},
  {"data", pa_data, 0},
  {"DATA", pa_data, 0},
  {"desc", pa_desc, 0},
  {"DESC", pa_desc, 0},
  {"double", pa_float_cons, 'd'},
  {"DOUBLE", pa_float_cons, 'd'},
  {"end", pa_end, 0},
  {"END", pa_end, 0},
  {"enter", pa_enter, 0},
  {"ENTER", pa_enter, 0},
  {"entry", pa_entry, 0},
  {"ENTRY", pa_entry, 0},
  {"equ", pa_equ, 0},
  {"EQU", pa_equ, 0},
  {"exit", pa_exit, 0},
  {"EXIT", pa_exit, 0},
  {"export", pa_export, 0},
  {"EXPORT", pa_export, 0},
  {"fill", pa_fill, 0},
  {"FILL", pa_fill, 0},
  {"float", pa_float_cons, 'f'},
  {"FLOAT", pa_float_cons, 'f'},
  {"half", pa_cons, 2},
  {"HALF", pa_cons, 2},
  {"import", pa_import, 0},
  {"IMPORT", pa_import, 0},
  {"int", pa_cons, 4},
  {"INT", pa_cons, 4},
  {"label", pa_label, 0},
  {"LABEL", pa_label, 0},
  {"lcomm", pa_lcomm, 0},
  {"LCOMM", pa_lcomm, 0},
  {"leave", pa_leave, 0},
  {"LEAVE", pa_leave, 0},
  {"long", pa_cons, 4},
  {"LONG", pa_cons, 4},
  {"lsym", pa_lsym, 0},
  {"LSYM", pa_lsym, 0},
  {"octa", pa_big_cons, 16},
  {"OCTA", pa_big_cons, 16},
  {"org", pa_origin, 0},
  {"ORG", pa_origin, 0},
  {"origin", pa_origin, 0},
  {"ORIGIN", pa_origin, 0},
  {"proc", pa_proc, 0},
  {"PROC", pa_proc, 0},
  {"procend", pa_procend, 0},
  {"PROCEND", pa_procend, 0},
  {"quad", pa_big_cons, 8},
  {"QUAD", pa_big_cons, 8},
  {"reg", pa_equ, 1},		/* very similar to .equ */
  {"REG", pa_equ, 1},		/* very similar to .equ */
  {"short", pa_cons, 2},
  {"SHORT", pa_cons, 2},
  {"single", pa_float_cons, 'f'},
  {"SINGLE", pa_float_cons, 'f'},
  {"space", pa_space, 0},
  {"SPACE", pa_space, 0},
  {"spnum", pa_spnum, 0},
  {"SPNUM", pa_spnum, 0},
  {"string", pa_stringer, 0},
  {"STRING", pa_stringer, 0},
  {"stringz", pa_stringer, 1},
  {"STRINGZ", pa_stringer, 1},
  {"subspa", pa_subspace, 0},
  {"SUBSPA", pa_subspace, 0},
  {"text", pa_text, 0},
  {"TEXT", pa_text, 0},
  {"version", pa_version, 0},
  {"VERSION", pa_version, 0},
  {"word", pa_cons, 4},
  {"WORD", pa_cons, 4},
  {NULL, 0, 0}
};

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output. */
/* Also note that '/*' will always start a comment */
const char line_comment_chars[] = "#";

const char line_separator_chars[] = "!";

/* Chars that can be used to separate mant from exp in floating point nums */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant */
/* As in 0f12.456 */
/* or    0d1.2345e12 */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

/* Also be aware that MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT may have to be
   changed in read.c .  Ideally it shouldn't have to know about it at all,
   but nothing is ideal around here.
 */

static unsigned char octal[256];
#ifndef isoctal
#define isoctal(c)  octal[c]
#endif
static unsigned char toHex[256];

struct pa_it set_insn;		/* this structure is defined above */

/* SKV 12/22/92.  Added prev_insn, prev_fix, and initialized the_insn
   so that we can recognize instruction sequences such as (ldil, ble)
   and generate the appropriate fixups. */

struct pa_it the_insn =
{
  NULL,				/* error */
  0,				/* opcode */
  NULL,				/* nlistp */
  {
    NULL,			/* exp.X_add_symbol */
    NULL,			/* exp.X_subtract_symbol */
    0,				/* exp.X_add_number */
    NULL			/* exp.asection */
  },
  0,				/* pcrel */
  0,				/* fpof1 */
  0,				/* fpof2 */
  0,				/* reloc */
  0,				/* field_selector */
  0,				/* code */
  0,				/* arg_reloc */
  {				/* unwind */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }
};

#ifdef OBJ_ELF

struct pa_it prev_insn;
char prev_str[10] = "";
fixS *prev_fix = NULL;
fixS *curr_fix = NULL;

#endif /* OBJ_ELF */

#ifdef __STDC__
void print_insn (struct pa_it *insn);
#else
void print_insn ();
#endif
char *expr_end;

symbolS *label_symbolP;		/* the last label symbol encountered */
/* saved here in case a .equ is encountered */
int label_symbol_defined;

int callinfo_found;		/* T if a .callinfo appeared within the current */
/*  procedure definition and F otherwise        */

int within_entry_exit;		/* T if the assembler is currently within a     */
/* .entry/.exit pair and F otherwise            */

int exit_processing_complete;	/* T is the assembler has completed exit */
/* processing for the current procedure  */
/* and F otherwise                       */

int within_procedure;		/* T if the assembler is currently within a  */
/* a procedure definition and F otherwise    */

void ignore_rest_of_line ();	/* a useful function in read.c */

/* default space and subspace dictionaries */

#define GDB_SYMBOLS          GDB_SYMBOLS_SUBSPACE_NAME
#define GDB_STRINGS          GDB_STRINGS_SUBSPACE_NAME

#if defined(OBJ_ELF)
struct default_subspace_dict pa_def_subspaces[] =
{
  {"$CODE$", 0, 1, 0, 0, 0, 0, 24, 0x2c, 0, 8, 0, 0, ".text", SUBSEG_CODE},
  {"$DATA$", 0, 1, 0, 0, 0, 0, 24, 0x1f, 0, 8, 1, 1, ".data", SUBSEG_DATA},
  {"$LIT$", 0, 1, 0, 0, 0, 0, 16, 0x2c, 0, 8, 0, 0, ".text", SUBSEG_LIT},
  {"$BSS$", 0, 1, 0, 0, 0, 1, 80, 0x1f, 0, 8, 1, 1, ".bss", SUBSEG_BSS},
  {"$UNWIND$", 0, 1, 0, 0, 0, 0, 64, 0x2c, 0, 4, 0, 0, ".hppa_unwind", SUBSEG_UNWIND},
  {GDB_STRINGS, 0, 0, 0, 0, 0, 0, 254, 0x1f, 0, 4, 0, 2, ".stabstr", SUBSEG_GDB_STRINGS},
  {GDB_SYMBOLS, 0, 0, 0, 0, 0, 0, 255, 0x1f, 0, 4, 0, 2, ".stab", SUBSEG_GDB_SYMBOLS},
  {NULL, 0, 1, 0, 0, 0, 0, 255, 0x1f, 0, 4, 0, 0, 0}
};

struct default_space_dict pa_def_spaces[] =
{
  {"$TEXT$", 0, 1, 0, 0, 8, ASEC_NULL, ".text"},
  {"$PRIVATE$", 0, 1, 0, 0, 16, ASEC_NULL, ".data"},
  {GDB_DEBUG_SPACE_NAME, 0, 0, 0, 0, 255, ASEC_NULL, ".stab"},
  {NULL, 0, 0, 0, 0, 0, ASEC_NULL, NULL}
};
#else
struct default_subspace_dict pa_def_subspaces[] =
{
  {"$CODE$", 0, 1, 0, 0, 0, 0, 24, 0x2c, 0, 8, 0, SEG_TEXT, SUBSEG_CODE},
  {"$DATA$", 0, 1, 0, 0, 0, 0, 24, 0x1f, 0, 8, 1, SEG_DATA, SUBSEG_DATA},
  {"$LIT$", 0, 1, 0, 0, 0, 0, 16, 0x2c, 0, 8, 0, SEG_TEXT, SUBSEG_LIT},
  {"$BSS$", 0, 1, 0, 0, 0, 1, 80, 0x1f, 0, 8, 1, SEG_DATA, SUBSEG_BSS},
  {"$UNWIND$", 0, 1, 0, 0, 0, 0, 64, 0x2c, 0, 4, 0, SEG_TEXT, SUBSEG_UNWIND},
  {GDB_STRINGS, 0, 0, 0, 0, 0, 0, 254, 0x1f, 0, 4, 0, SEG_GDB, SUBSEG_GDB_STRINGS
  },
  {GDB_SYMBOLS, 0, 0, 0, 0, 0, 0, 255, 0x1f, 0, 4, 0, SEG_GDB, SUBSEG_GDB_SYMBOLS
  },
  {NULL, 0, 1, 0, 0, 0, 0, 255, 0x1f, 0, 4, 0, SEG_GOOF, 0}
};

struct default_space_dict pa_def_spaces[] =
{
  {"$TEXT$", 0, 1, 0, 0, 8, SEG_TEXT},
  {"$PRIVATE$", 0, 1, 0, 0, 16, SEG_DATA},
  {GDB_DEBUG_SPACE_NAME, 0, 0, 0, 0, 255, SEG_GDB},
  {NULL, 0, 0, 0, 0, 0, SEG_GOOF}
};
#endif

#ifdef USG
#define bcmp(b1,b2,n) memcmp((b1),(b2),(n))
#define index strchr
#endif

#ifndef FALSE
#define FALSE   (0)
#define TRUE    (!FALSE)
#endif /* no FALSE yet */

/*
	Support for keeping track of the most recent label in each
	space.
 */

/*
	PA_PSEUDO_OP_MOVES_PC

	A predicate that returns true if the pseudo-operation or
	assembly directive results in a movement in the current
	location.  All instructions cause movement in the current
	location.
 */

static const char *movers[] =
{
/* these entries from 'static pseudo_typeS potable[]' in pa-read.c */
  "ascii", "asciz",
  "byte",
  "comm",
  "data", "desc", "double",
  "fill", "float",
  "globl",
  "half",
  "int",
  "lcomm", "long", "lsym",
  "octa", "org",
  "quad",
  "short", "single",
  "text",
  "word",
/* these entries from 'pseudo_typeS md_pseudo_table[]' in pa-aux.c */
  "block", "blockz",
  "code", "copyright",
  "equ",
  "origin",
  "reg",			/* very similar to .equ */
  "string", "stringz",
  "version",
  NULL				/* end sentinel */
};

int
pa_pseudo_op_moves_pc (name)
     char *name;
{
  int i = 0;
  while (movers[i])
    {
      if (strcmp (name, movers[i++]) == 0)
	return 1;
    }

  return 0;
}

/*
	Support for keeping track of the most recent label in each
	space.
 */

/* XXX:  NOTE:  label_symbolS is defined in pa.h */

label_symbolS *label_symbols_rootP = NULL;

/*
	PA_GET_LABEL

	Returns a pointer to the label_symbolS for the current space.
 */

label_symbolS *
pa_get_label ()
{
  label_symbolS *lssP;
  space_dict_chainS *now_sdcP = pa_segment_to_space (now_seg);

  for (lssP = label_symbols_rootP; lssP; lssP = lssP->lss_next)
    {
      if (now_sdcP == lssP->lss_space && lssP->lss_label)
	return lssP;
    }

  return (label_symbolS *) NULL;
}

/*
	PA_LABEL_IS_DEFINED

	A predicate to determine whether a useable label is defined in
	the current space.
 */

int
pa_label_is_defined ()
{
  return (int) pa_get_label ();
}

/*
	PA_DEFINE_LABEL

	Defines a label for the current space.  If one is already defined,
	this function will replace it with the new label.
 */

void
pa_define_label (symbolP)
     symbolS *symbolP;
{
  label_symbolS *lssP = pa_get_label ();
  space_dict_chainS *now_sdcP = pa_segment_to_space (now_seg);

  if (lssP)
    {
      lssP->lss_label = symbolP;
    }
  else
    {
      lssP = (label_symbolS *) xmalloc (sizeof (label_symbolS));
      lssP->lss_label = symbolP;
      lssP->lss_space = now_sdcP;
      lssP->lss_next = (label_symbolS *) NULL;

      if (label_symbols_rootP)
	{
	  lssP->lss_next = label_symbols_rootP;
	}
      label_symbols_rootP = lssP;
    }
}

/*
	PA_UNDEFINE_LABEL

	Removes a label definition for the current space.
	If there is no label_symbolS entry, then no action is taken.
 */

void
pa_undefine_label ()
{
  label_symbolS *lssP;
  label_symbolS *prevP = (label_symbolS *) NULL;
  space_dict_chainS *now_sdcP = pa_segment_to_space (now_seg);

  for (lssP = label_symbols_rootP; lssP; lssP = lssP->lss_next)
    {
      if (now_sdcP == lssP->lss_space && lssP->lss_label)
	{
	  if (prevP)
	    prevP->lss_next = lssP->lss_next;
	  else
	    label_symbols_rootP = lssP->lss_next;

	  free (lssP);
	  break;
	}
      prevP = lssP;
    }
}

/* end of label symbol support. */


/* An HPPA-specific version of fix_new.  This is required because the HPPA	*/
/* code needs to keep track of some extra stuff.  Each call to fix_new_hppa	*/
/* results in the creation of an instance of an hppa_fixS.  An hppa_fixS	*/
/* stores the extra information along with a pointer to the original fixS.	*/

typedef struct hppa_fix_struct
  {
    fixS *fx_fixP;
    int fx_r_field;
    int fx_r_type;
    int fx_r_format;
    long fx_arg_reloc;
    call_infoS *fx_call_infop;
    char fx_unwind[8];
    struct hppa_fix_struct *fx_next;
  } hppa_fixS;

hppa_fixS *hppa_fix_root = NULL;

void 
fix_new_hppa (frag, where, size, add_symbol, sub_symbol, offset, pcrel,
	      r_type, r_field, r_format, arg_reloc, unwind_desc)
     fragS *frag;		/* Which frag? */
     int where;			/* Where in that frag? */
     short int size;		/* 1, 2  or 4 usually. */
     symbolS *add_symbol;	/* X_add_symbol. */
     symbolS *sub_symbol;	/* X_subtract_symbol. */
     long offset;		/* X_add_number. */
     int pcrel;			/* TRUE if PC-relative relocation. */
#ifdef BFD_ASSEMBLER
     bfd_reloc_code_real_type r_type;	/* Relocation type */
#else
     int r_type;		/* Relocation type */
#endif
     long r_field;		/* F, R, L, etc */
     int r_format;		/* 11,12,14,17,21,32, etc */
     long arg_reloc;
     char *unwind_desc;
{
  fixS *new_fix = fix_new (frag, where, size,
			   add_symbol, sub_symbol,
			   offset, pcrel, r_type);

  hppa_fixS *hppa_fix = (hppa_fixS *) obstack_alloc (&notes, sizeof (hppa_fixS));

  hppa_fix->fx_fixP = new_fix;
  hppa_fix->fx_r_field = r_field;
  hppa_fix->fx_r_format = r_format;
  hppa_fix->fx_arg_reloc = arg_reloc;
  hppa_fix->fx_next = (hppa_fixS *) 0;
  hppa_fix->fx_call_infop = last_call_info;
  if (unwind_desc)
    bcopy (unwind_desc, hppa_fix->fx_unwind, 8);

  if (hppa_fix_root)
    hppa_fix->fx_next = hppa_fix_root;

  hppa_fix_root = hppa_fix;

  /* SKV 12/22/92.  Added prev_insn, prev_fix, and initialized the_insn
   so that we can recognize instruction sequences such as (ldil, ble)
   and generate the appropriate fixups. */

#ifdef OBJ_ELF

  curr_fix = new_fix;

#endif /* OBJ_ELF */
}

/* Parse a .byte, .word, .long expression for the HPPA.  Called by
   cons via the TC_PARSE_CONS_EXPRESSION macro.  */

static int hppa_field_selector;

void
parse_cons_expression_hppa (exp)
     expressionS *exp;
{
  hppa_field_selector = pa_chk_field_selector (&input_line_pointer);
  expression (&exp);
}

/* This fix_new is called by cons via TC_CONS_FIX_NEW.
   hppa_field_selector is set by the parse_cons_expression_hppa.  */

void
cons_fix_new_hppa (frag, where, size, exp)
     fragS *frag;		/* Which frag? */
     int where;			/* Where in that frag? */
     int size;			/* 1, 2  or 4 usually. */
     expressionS *exp;		/* Expression. */
{
  unsigned int reloc_type;

  if (is_DP_relative (*exp))
    reloc_type = R_HPPA_GOTOFF;
  else if (is_complex (*exp))
    reloc_type = R_HPPA_COMPLEX;
  else
    reloc_type = R_HPPA;

  if (hppa_field_selector != e_psel && hppa_field_selector != e_fsel)
    as_warn("Invalid field selector.  Assuming F%%.");

  fix_new_hppa (frag, where, size
		exp->X_add_symbol,
		exp->X_subtract_symbol,
		exp->X_add_number, 0, reloc_type,
		hppa_field_selector, 32, 0, (char *) 0);
}

/* Given a FixS, find the hppa_fixS associated with it. */
hppa_fixS *
hppa_find_hppa_fix (fix)
     fixS *fix;
{
  hppa_fixS *hfP;

  for (hfP = hppa_fix_root; hfP; hfP = hfP->fx_next)
    {
      if (hfP->fx_fixP == fix)
	return hfP;
    }

  return (hppa_fixS *) 0;
}

/* This function is called once, at assembler startup time.  It should
   set up all the tables, etc. that the MD part of the assembler will need.  */
void
md_begin ()
{
  register char *retval = NULL;
  int lose = 0;
  register unsigned int i = 0;
  void pa_spaces_begin ();	/* forward declaration */

  last_call_info = NULL;
  call_info_root = NULL;

  pa_spaces_begin ();

  op_hash = hash_new ();
  if (op_hash == NULL)
    as_fatal ("Virtual memory exhausted");

  while (i < NUMOPCODES)
    {
      const char *name = pa_opcodes[i].name;
      retval = hash_insert (op_hash, name, &pa_opcodes[i]);
      if (retval != NULL && *retval != '\0')
	{
	  as_fatal ("Internal error: can't hash `%s': %s\n",
		    pa_opcodes[i].name, retval);
	  lose = 1;
	}
      do
	{
	  if ((pa_opcodes[i].match & pa_opcodes[i].mask) != pa_opcodes[i].match)
	    {
	      fprintf (stderr, "internal error: losing opcode: `%s' \"%s\"\n",
		       pa_opcodes[i].name, pa_opcodes[i].args);
	      lose = 1;
	    }
	  ++i;
	}
      while (i < NUMOPCODES
	     && !strcmp (pa_opcodes[i].name, name));
    }

  if (lose)
    as_fatal ("Broken assembler.  No assembly attempted.");

  for (i = '0'; i < '8'; ++i)
    octal[i] = 1;
  for (i = '0'; i <= '9'; ++i)
    toHex[i] = i - '0';
  for (i = 'a'; i <= 'f'; ++i)
    toHex[i] = i + 10 - 'a';
  for (i = 'A'; i <= 'F'; ++i)
    toHex[i] = i + 10 - 'A';

}

void
md_end ()
{
  return;
}

void
md_assemble (str)
     char *str;
{
  char *toP;

  assert (str);
  pa_ip (str);
  toP = frag_more (4);
  /* put out the opcode */
  md_number_to_chars (toP, the_insn.opcode, 4);

  /* put out the symbol-dependent stuff */
#if defined ( OBJ_SOM )
  if (the_insn.reloc != R_NO_RELOCATION)
    {
#else
#if defined ( OBJ_ELF )
  if (the_insn.reloc != R_HPPA_NONE)
    {
#endif
#endif

#if defined(OBJ_ELF)
      fix_new_hppa (frag_now,	/* which frag */
		    (toP - frag_now->fr_literal),	/* where */
		    4,		/* size */
		    the_insn.exp.X_add_symbol,
		    the_insn.exp.X_subtract_symbol,
		    the_insn.exp.X_add_number,
		    the_insn.pcrel,
		    the_insn.reloc,
		    the_insn.field_selector,
		    the_insn.format,
		    the_insn.arg_reloc,
		    (char *) 0);
#endif
#ifdef OBJ_SOM
      fix_new (frag_now,	/* which frag */
	       (toP - frag_now->fr_literal),	/* where */
	       4,		/* size */
	       the_insn.exp.X_add_symbol,
	       the_insn.exp.X_subtract_symbol,
	       the_insn.exp.X_add_number,
	       the_insn.pcrel,
	       the_insn.reloc,
	       the_insn.field_selector,
	       the_insn.code,
	       the_insn.arg_reloc,
	       (char *) 0);
#endif
    }

  /* SKV 12/22/92.  Added prev_insn, prev_fix, and initialized the_insn
   so that we can recognize instruction sequences such as (ldil, ble)
   and generate the appropriate fixups. */

#ifdef OBJ_ELF

  prev_insn = the_insn;
  strncpy (prev_str, str, 10);
  if (prev_insn.reloc = R_HPPA_NONE)
    {
      prev_fix = NULL;
    }
  else
    {
      prev_fix = curr_fix;
    }

#endif /* OBJ_ELF */
}

static void
pa_ip (str)
     char *str;
{
  char *error_message = "";
  char *s;
  const char *args;
  char c;
  unsigned long i;
  struct pa_opcode *insn;
  char *argsStart;
  unsigned long opcode;
  unsigned int mask;
  int match = FALSE;
  int comma = 0;

  int reg, reg1, reg2, s2, s3;
  unsigned int im21, im14, im11, im5;
  int m, a, uu, f;
  int cmpltr, nullif, flag;
  int sfu, cond;
  char *name;
  char *p, *save_s;

#ifdef PA_DEBUG
  fprintf (stderr, "STATEMENT: \"%s\"\n", str);
#endif
  for (s = str; isupper (*s) || islower (*s) || (*s >= '0' && *s <= '3'); ++s)
    ;
  switch (*s)
    {

    case '\0':
      break;

    case ',':
      comma = 1;

      /*FALLTHROUGH*/

    case ' ':
      *s++ = '\0';
      break;

    default:
      as_bad ("Unknown opcode: `%s'", str);
      exit (1);
    }

  save_s = str;

  while (*save_s)
    {
      if (isupper (*save_s))
	*save_s = tolower (*save_s);
      save_s++;
    }

  if ((insn = (struct pa_opcode *) hash_find (op_hash, str)) == NULL)
    {
      as_bad ("Unknown opcode: `%s'", str);
      return;
    }
  if (comma)
    {
      *--s = ',';
    }
  argsStart = s;
  for (;;)
    {
      opcode = insn->match;
      bzero (&the_insn, sizeof (the_insn));
#if defined( OBJ_SOM )
      the_insn.reloc = R_NO_RELOCATION;
#else
#if defined ( OBJ_ELF )
      the_insn.reloc = R_HPPA_NONE;
#endif
#endif
      /*
	 * Build the opcode, checking as we go to make
	 * sure that the operands match
	 */
      for (args = insn->args;; ++args)
	{

	  switch (*args)
	    {

	    case '\0':		/* end of args */
	      if (*s == '\0')
		{
		  match = TRUE;
		}
	      break;

	    case '+':
	      if (*s == '+')
		{
		  ++s;
		  continue;
		}
	      if (*s == '-')
		{
		  continue;
		}
	      break;

	    case '(':		/* these must match exactly */
	    case ')':
	    case ',':
	    case ' ':
	      if (*s++ == *args)
		continue;
	      break;

	    case 'b':		/* 5 bit register field at 10 */
	    case '^':		/* 5 bit control register field at 10 */
	      reg = pa_parse_number (&s);
	      if (reg < 32 && reg >= 0)
		{
		  opcode |= reg << 21;
		  continue;
		}
	      break;
	    case 'x':		/* 5 bit register field at 15 */
	      reg = pa_parse_number (&s);
	      if (reg < 32 && reg >= 0)
		{
		  opcode |= reg << 16;
		  continue;
		}
	      break;
	    case 't':		/* 5 bit register field at 31 */
	      reg = pa_parse_number (&s);
	      if (reg < 32 && reg >= 0)
		{
		  opcode |= reg;
		  continue;
		}
	      break;
	    case 'T':		/* 5 bit field length at 31 (encoded as 32-T) */
	      /*
		reg = pa_parse_number(&s);
	       */
	      getAbsoluteExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  reg = the_insn.exp.X_add_number;
		  if (reg <= 32 && reg > 0)
		    {
		      opcode |= 32 - reg;
		      s = expr_end;
		      continue;
		    }
		}
	      break;
	    case '5':		/* 5 bit immediate at 15 */
	      getAbsoluteExpression (s);
	      /** PJH: The following 2 calls to as_bad() might eventually **/
	      /**      want to end up as as_warn().  **/
	      if (the_insn.exp.X_add_number > 15)
		{
		  as_bad ("5 bit immediate > 15. Set to 15",
			  the_insn.exp.X_add_number);
		  the_insn.exp.X_add_number = 15;
		}
	      else if (the_insn.exp.X_add_number < -16)
		{
		  as_bad ("5 bit immediate < -16. Set to -16",
			  the_insn.exp.X_add_number);
		  the_insn.exp.X_add_number = -16;
		}

	      low_sign_unext (evaluateAbsolute (the_insn.exp, the_insn.field_selector),
			      5, &im5);
	      opcode |= (im5 << 16);
	      s = expr_end;
	      continue;

	    case 's':		/* 2 bit space identifier at 17 */
	      s2 = pa_parse_number (&s);
	      if (s2 < 4 && s2 >= 0)
		{
		  opcode |= s2 << 14;
		  continue;
		}
	      break;
	    case 'S':		/* 3 bit space identifier at 18 */
	      s3 = pa_parse_number (&s);
	      if (s3 < 8 && s3 >= 0)
		{
		  dis_assemble_3 (s3, &s3);
		  opcode |= s3 << 13;
		  continue;
		}
	      break;
	    case 'c':		/* indexed load completer. */
	      uu = 0;
	      m = 0;
	      i = 0;
	      while (*s == ',' && i < 2)
		{
		  s++;
		  if (strncasecmp (s, "sm", 2) == 0)
		    {
		      uu = 1;
		      m = 1;
		      s++;
		      i++;
		    }
		  else if (strncasecmp (s, "m", 1) == 0)
		    m = 1;
		  else if (strncasecmp (s, "s", 1) == 0)
		    uu = 1;
		  else
		    as_bad ("Unrecognized Indexed Load Completer...assuming 0");
		  s++;
		  i++;
		}
	      if (i > 2)
		as_bad ("Illegal Indexed Load Completer Syntax...extras ignored");
	      /* pa_skip(&s); */
	      while (*s == ' ' || *s == '\t')
		s++;

	      opcode |= m << 5;
	      opcode |= uu << 13;
	      continue;
	    case 'C':		/* short load and store completer */
	      a = 0;
	      m = 0;
	      if (*s == ',')
		{
		  s++;
		  if (strncasecmp (s, "ma", 2) == 0)
		    {
		      a = 0;
		      m = 1;
		    }
		  else if (strncasecmp (s, "mb", 2) == 0)
		    {
		      a = 1;
		      m = 1;
		    }
		  else
		    as_bad ("Unrecognized Indexed Load Completer...assuming 0");
		  s += 2;
		}
	      /* pa_skip(&s); */
	      while (*s == ' ' || *s == '\t')
		s++;
	      opcode |= m << 5;
	      opcode |= a << 13;
	      continue;
	    case 'Y':		/* Store Bytes Short completer */
	      a = 0;
	      m = 0;
	      i = 0;
	      while (*s == ',' && i < 2)
		{
		  s++;
		  if (strncasecmp (s, "m", 1) == 0)
		    m = 1;
		  else if (strncasecmp (s, "b", 1) == 0)
		    a = 0;
		  else if (strncasecmp (s, "e", 1) == 0)
		    a = 1;
		  else
		    as_bad ("Unrecognized Store Bytes Short Completer...assuming 0");
		  s++;
		  i++;
		}
	      /**		if ( i >= 2 ) **/
	      if (i > 2)
		as_bad ("Illegal Store Bytes Short Completer...extras ignored");
	      while (*s == ' ' || *s == '\t')	/* skip to next operand */
		s++;
	      opcode |= m << 5;
	      opcode |= a << 13;
	      continue;
	    case '<':		/* non-negated compare/subtract conditions. */
	      cmpltr = pa_parse_nonneg_cmpsub_cmpltr (&s);
	      if (cmpltr < 0)
		{
		  as_bad ("Unrecognized Compare/Subtract Condition: %c", *s);
		  cmpltr = 0;
		}
	      opcode |= cmpltr << 13;
	      continue;
	    case '?':		/* negated or non-negated cmp/sub conditions. */
	      /* used only by ``comb'' and ``comib'' pseudo-ops */
	      save_s = s;
	      cmpltr = pa_parse_nonneg_cmpsub_cmpltr (&s);
	      if (cmpltr < 0)
		{
		  s = save_s;
		  cmpltr = pa_parse_neg_cmpsub_cmpltr (&s);
		  if (cmpltr < 0)
		    {
		      as_bad ("Unrecognized Compare/Subtract Condition: %c", *s);
		      cmpltr = 0;
		    }
		  else
		    {
		      opcode |= 1 << 27;	/* required opcode change to make
		                        COMIBT into a COMIBF or a
		                        COMBT into a COMBF or a
			                ADDBT into a ADDBF or a
			                ADDIBT into a ADDIBF */
		    }
		}
	      opcode |= cmpltr << 13;
	      continue;
	    case '!':		/* negated or non-negated add conditions. */
	      /* used only by ``addb'' and ``addib'' pseudo-ops */
	      save_s = s;
	      cmpltr = pa_parse_nonneg_add_cmpltr (&s);
	      if (cmpltr < 0)
		{
		  s = save_s;
		  cmpltr = pa_parse_neg_add_cmpltr (&s);
		  if (cmpltr < 0)
		    {
		      as_bad ("Unrecognized Compare/Subtract Condition: %c", *s);
		      cmpltr = 0;
		    }
		  else
		    {
		      opcode |= 1 << 27;	/* required opcode change to make
		    COMIBT into a COMIBF or a
		      COMBT into a COMBF or a
			ADDBT into a ADDBF or a
			  ADDIBT into a ADDIBF */
		    }
		}
	      opcode |= cmpltr << 13;
	      continue;
	    case 'a':		/* compare/subtract conditions */
	      cmpltr = 0;
	      f = 0;
	      save_s = s;
	      if (*s == ',')
		{
		  cmpltr = pa_parse_nonneg_cmpsub_cmpltr (&s);
		  if (cmpltr < 0)
		    {
		      f = 1;
		      s = save_s;
		      cmpltr = pa_parse_neg_cmpsub_cmpltr (&s);
		      if (cmpltr < 0)
			{
			  as_bad ("Unrecognized Compare/Subtract Condition");
			}
		    }
		}
	      opcode |= cmpltr << 13;
	      opcode |= f << 12;
	      continue;
	    case 'd':		/* non-negated add conditions */
	      cmpltr = 0;
	      nullif = 0;
	      flag = 0;
	      if (*s == ',')
		{
		  s++;
		  name = s;
		  while (*s != ',' && *s != ' ' && *s != '\t')
		    s += 1;
		  c = *s;
		  *s = 0x00;
		  if (strcmp (name, "=") == 0)
		    {
		      cmpltr = 1;
		    }
		  else if (strcmp (name, "<") == 0)
		    {
		      cmpltr = 2;
		    }
		  else if (strcmp (name, "<=") == 0)
		    {
		      cmpltr = 3;
		    }
		  else if (strcasecmp (name, "nuv") == 0)
		    {
		      cmpltr = 4;
		    }
		  else if (strcasecmp (name, "znv") == 0)
		    {
		      cmpltr = 5;
		    }
		  else if (strcasecmp (name, "sv") == 0)
		    {
		      cmpltr = 6;
		    }
		  else if (strcasecmp (name, "od") == 0)
		    {
		      cmpltr = 7;
		    }
		  else if (strcasecmp (name, "n") == 0)
		    {
		      nullif = 1;
		    }
		  else if (strcasecmp (name, "tr") == 0)
		    {
		      cmpltr = 0;
		      flag = 1;
		    }
		  else if (strcasecmp (name, "<>") == 0)
		    {
		      cmpltr = 1;
		      flag = 1;
		    }
		  else if (strcasecmp (name, ">=") == 0)
		    {
		      cmpltr = 2;
		      flag = 1;
		    }
		  else if (strcasecmp (name, ">") == 0)
		    {
		      cmpltr = 3;
		      flag = 1;
		    }
		  else if (strcasecmp (name, "uv") == 0)
		    {
		      cmpltr = 4;
		      flag = 1;
		    }
		  else if (strcasecmp (name, "vnz") == 0)
		    {
		      cmpltr = 5;
		      flag = 1;
		    }
		  else if (strcasecmp (name, "nsv") == 0)
		    {
		      cmpltr = 6;
		      flag = 1;
		    }
		  else if (strcasecmp (name, "ev") == 0)
		    {
		      cmpltr = 7;
		      flag = 1;
		    }
		  else
		    as_bad ("Unrecognized Add Condition: %s", name);
		  *s = c;
		}
	      nullif = pa_parse_nullif (&s);
	      opcode |= nullif << 1;
	      opcode |= cmpltr << 13;
	      opcode |= flag << 12;
	      continue;
	    case '&':		/* logical instruction conditions */
	      cmpltr = 0;
	      f = 0;
	      if (*s == ',')
		{
		  s++;
		  name = s;
		  while (*s != ',' && *s != ' ' && *s != '\t')
		    s += 1;
		  c = *s;
		  *s = 0x00;
		  if (strcmp (name, "=") == 0)
		    {
		      cmpltr = 1;
		    }
		  else if (strcmp (name, "<") == 0)
		    {
		      cmpltr = 2;
		    }
		  else if (strcmp (name, "<=") == 0)
		    {
		      cmpltr = 3;
		    }
		  else if (strcasecmp (name, "od") == 0)
		    {
		      cmpltr = 7;
		    }
		  else if (strcasecmp (name, "tr") == 0)
		    {
		      cmpltr = 0;
		      f = 1;
		    }
		  else if (strcmp (name, "<>") == 0)
		    {
		      cmpltr = 1;
		      f = 1;
		    }
		  else if (strcmp (name, ">=") == 0)
		    {
		      cmpltr = 2;
		      f = 1;
		    }
		  else if (strcmp (name, ">") == 0)
		    {
		      cmpltr = 3;
		      f = 1;
		    }
		  else if (strcasecmp (name, "ev") == 0)
		    {
		      cmpltr = 7;
		      f = 1;
		    }
		  else
		    as_bad ("Unrecognized Logical Instruction Condition: %s", name);
		  *s = c;
		}
	      opcode |= cmpltr << 13;
	      opcode |= f << 12;
	      continue;
	    case 'U':		/* unit instruction conditions */
	      cmpltr = 0;
	      f = 0;
	      if (*s == ',')
		{
		  s++;
		  if (strncasecmp (s, "sbz", 3) == 0)
		    {
		      cmpltr = 2;
		      s += 3;
		    }
		  else if (strncasecmp (s, "shz", 3) == 0)
		    {
		      cmpltr = 3;
		      s += 3;
		    }
		  else if (strncasecmp (s, "sdc", 3) == 0)
		    {
		      cmpltr = 4;
		      s += 3;
		    }
		  else if (strncasecmp (s, "sbc", 3) == 0)
		    {
		      cmpltr = 6;
		      s += 3;
		    }
		  else if (strncasecmp (s, "shc", 3) == 0)
		    {
		      cmpltr = 7;
		      s += 3;
		    }
		  else if (strncasecmp (s, "tr", 2) == 0)
		    {
		      cmpltr = 0;
		      f = 1;
		      s += 2;
		    }
		  else if (strncasecmp (s, "nbz", 3) == 0)
		    {
		      cmpltr = 2;
		      f = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "nhz", 3) == 0)
		    {
		      cmpltr = 3;
		      f = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "ndc", 3) == 0)
		    {
		      cmpltr = 4;
		      f = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "nbc", 3) == 0)
		    {
		      cmpltr = 6;
		      f = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "nhc", 3) == 0)
		    {
		      cmpltr = 7;
		      f = 1;
		      s += 3;
		    }
		  else
		    as_bad ("Unrecognized Logical Instruction Condition: %c", *s);
		}
	      opcode |= cmpltr << 13;
	      opcode |= f << 12;
	      continue;
	    case '>':		/* shift/extract/deposit conditions. */
	      cmpltr = 0;
	      if (*s == ',')
		{
		  s++;
		  name = s;
		  while (*s != ',' && *s != ' ' && *s != '\t')
		    s += 1;
		  c = *s;
		  *s = 0x00;
		  if (strcmp (name, "=") == 0)
		    {
		      cmpltr = 1;
		    }
		  else if (strcmp (name, "<") == 0)
		    {
		      cmpltr = 2;
		    }
		  else if (strcasecmp (name, "od") == 0)
		    {
		      cmpltr = 3;
		    }
		  else if (strcasecmp (name, "tr") == 0)
		    {
		      cmpltr = 4;
		    }
		  else if (strcmp (name, "<>") == 0)
		    {
		      cmpltr = 5;
		    }
		  else if (strcmp (name, ">=") == 0)
		    {
		      cmpltr = 6;
		    }
		  else if (strcasecmp (name, "ev") == 0)
		    {
		      cmpltr = 7;
		    }
		  else
		    as_bad ("Unrecognized Shift/Extract/Deposit Condition: %s", name);
		  *s = c;
		}
	      opcode |= cmpltr << 13;
	      continue;
	    case '~':		/* bvb,bb conditions */
	      cmpltr = 0;
	      if (*s == ',')
		{
		  s++;
		  if (strncmp (s, "<", 1) == 0)
		    {
		      cmpltr = 2;
		      s++;
		    }
		  else if (strncmp (s, ">=", 2) == 0)
		    {
		      cmpltr = 6;
		      s += 2;
		    }
		  else
		    as_bad ("Unrecognized Bit Branch Condition: %c", *s);
		}
	      opcode |= cmpltr << 13;
	      continue;
	    case 'V':		/* 5  bit immediate at 31 */
	      getExpression (s);
	      low_sign_unext (evaluateAbsolute (the_insn.exp, the_insn.field_selector),
			      5, &im5);
	      opcode |= im5;
	      s = expr_end;
	      continue;
	    case 'r':		/* 5  bit immediate at 31 */
	      /* (unsigned value for the break instruction) */
	      getExpression (s);
	      im5 = evaluateAbsolute (the_insn.exp, the_insn.field_selector);
	      if (im5 > 31 || im5 < 0)
		{
		  as_bad ("Operand out of range. Was: %d. Should be [0..31]. Assuming %d.\n", im5, im5 & 0x1f);
		  im5 = im5 & 0x1f;
		}
	      opcode |= im5;
	      s = expr_end;
	      continue;
	    case 'R':		/* 5  bit immediate at 15 */
	      /* (unsigned value for the ssm and rsm instruction) */
	      getExpression (s);
	      im5 = evaluateAbsolute (the_insn.exp, the_insn.field_selector);
	      if (im5 > 31 || im5 < 0)
		{
		  as_bad ("Operand out of range. Was: %d. Should be [0..31]. Assuming %d.\n", im5, im5 & 0x1f);
		  im5 = im5 & 0x1f;
		}
	      opcode |= im5 << 16;
	      s = expr_end;
	      continue;
	    case 'i':		/* 11 bit immediate at 31 */
#ifdef OBJ_SOM
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  low_sign_unext (evaluateAbsolute (the_insn.exp, the_insn.field_selector),
				  11, &im11);
		  opcode |= im11;
		}
	      else
		{
		  the_insn.reloc = R_CODE_ONE_SYMBOL;
		  the_insn.code = 'i';
		  the_insn.field_selector = the_insn.exp.field_selector;
		}
	      s = expr_end;
	      continue;
#else
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  low_sign_unext (evaluateAbsolute (the_insn.exp, the_insn.field_selector),
				  11, &im11);
		  opcode |= im11;
		}
	      else
		{
		  if (is_DP_relative (the_insn.exp))
		    the_insn.reloc = R_HPPA_GOTOFF;
		  else if (is_PC_relative (the_insn.exp))
		    the_insn.reloc = R_HPPA_PCREL_CALL;
		  else if (is_complex (the_insn.exp))
		    the_insn.reloc = R_HPPA_COMPLEX;
		  else
		    the_insn.reloc = R_HPPA;
		  the_insn.format = 11;
		}
	      s = expr_end;
	      continue;
#endif
	    case 'j':		/* 14 bit immediate at 31 */
#ifdef OBJ_SOM
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  low_sign_unext (evaluateAbsolute (the_insn.exp, field_selector),
				  14, &im14);
		  if (the_insn.exp.field_selector == e_rsel)
		    opcode |= (im14 & 0xfff);
		  else
		    opcode |= im14;
		}
	      else
		{
		  the_insn.reloc = R_CODE_ONE_SYMBOL;
		  the_insn.code = 'j';
		  the_insn.field_selector = the_insn.exp.field_selector;
		}
	      s = expr_end;
	      continue;
#else
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  low_sign_unext (evaluateAbsolute (the_insn.exp, the_insn.field_selector),
				  14, &im14);
		  if (the_insn.field_selector == e_rsel)
		    opcode |= (im14 & 0xfff);
		  else
		    opcode |= im14;
		}
	      else
		{
		  if (is_DP_relative (the_insn.exp))
		    the_insn.reloc = R_HPPA_GOTOFF;
		  else if (is_PC_relative (the_insn.exp))
		    the_insn.reloc = R_HPPA_PCREL_CALL;
		  else if (is_complex (the_insn.exp))
		    the_insn.reloc = R_HPPA_COMPLEX;
		  else
		    the_insn.reloc = R_HPPA;
		  the_insn.format = 14;
		}
	      s = expr_end;
	      continue;
#endif

	    case 'k':		/* 21 bit immediate at 31 */
#ifdef OBJ_SOM
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  dis_assemble_21 (evaluateAbsolute (the_insn.exp, the_insn.field_selector),
				   &im21);
		  opcode |= im21;
		}
	      else
		{
		  the_insn.reloc = R_CODE_ONE_SYMBOL;
		  the_insn.code = 'k';
		  the_insn.field_selector = the_insn.exp.field_selector;
		}
	      s = expr_end;
	      continue;
#else
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  dis_assemble_21 (evaluateAbsolute (the_insn.exp, the_insn.field_selector),
				   &im21);
		  opcode |= im21;
		}
	      else
		{
		  if (is_DP_relative (the_insn.exp))
		    the_insn.reloc = R_HPPA_GOTOFF;
		  else if (is_PC_relative (the_insn.exp))
		    the_insn.reloc = R_HPPA_PCREL_CALL;
		  else if (is_complex (the_insn.exp))
		    the_insn.reloc = R_HPPA_COMPLEX;
		  else
		    the_insn.reloc = R_HPPA;
		  the_insn.format = 21;
		}
	      s = expr_end;
	      continue;
#endif

	    case 'n':		/* nullification for branch instructions */
	      nullif = pa_parse_nullif (&s);
	      opcode |= nullif << 1;
	      continue;
	    case 'w':		/* 12 bit branch displacement */
#ifdef OBJ_SOM
	      getExpression (s);
	      the_insn.pcrel = 1;
	      if (strcmp (the_insn.exp.X_add_symbol->sy_nlist.n_un.n_name, "L0\001") == 0)
		{
		  unsigned int w1, w, result;

		  sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 12, &result);
		  dis_assemble_12 (result, &w1, &w);
		  opcode |= ((w1 << 2) | w);
		  the_insn.exp.X_add_symbol->sy_ref = FALSE;
		}
	      else
		{
		  /* this has to be wrong -- dont know what is right! */
		  the_insn.reloc = R_PCREL_CALL;
		  the_insn.code = 'w';
		  the_insn.field_selector = the_insn.exp.field_selector;
		  the_insn.arg_reloc = last_call_desc.arg_reloc;
		  bzero (&last_call_desc, sizeof (call_descS));
		}
	      s = expr_end;
	      continue;
#else
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      getExpression (s);
	      the_insn.pcrel = 1;
	      if (strcmp (S_GET_NAME (the_insn.exp.X_add_symbol), "L0\001") == 0)
		{
		  unsigned int w1, w, result;

		  sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 12, &result);
		  dis_assemble_12 (result, &w1, &w);
		  opcode |= ((w1 << 2) | w);
		  /* the_insn.exp.X_add_symbol->sy_ref = FALSE; *//* XXX: not sure how to do this in BFD */
		}
	      else
		{
		  if (is_complex (the_insn.exp))
		    the_insn.reloc = R_HPPA_COMPLEX_PCREL_CALL;
		  else
		    the_insn.reloc = R_HPPA_PCREL_CALL;
		  the_insn.format = 12;
		  the_insn.arg_reloc = last_call_desc.arg_reloc;
		  bzero (&last_call_desc, sizeof (call_descS));
		}
	      s = expr_end;
	      continue;
#endif
	    case 'W':		/* 17 bit branch displacement */
#if defined(OBJ_ELF)
	      the_insn.field_selector = pa_chk_field_selector (&s);
#endif
	      getExpression (s);
	      the_insn.pcrel = 1;
#ifdef OBJ_SOM
	      if (strcmp (the_insn.exp.X_add_symbol->sy_nlist.n_un.n_name, "L0\001") == 0)
		{
		  unsigned int w2, w1, w, result;

		  sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 17, &result);
		  dis_assemble_17 (result, &w1, &w2, &w);
		  opcode |= ((w2 << 2) | (w1 << 16) | w);
		  the_insn.exp.X_add_symbol->sy_ref = FALSE;
		}
	      else
		{
		  /* this has to be wrong -- dont know what is right! */
		  the_insn.reloc = R_PCREL_CALL;
		  the_insn.code = 'W';
		  the_insn.field_selector = the_insn.exp.field_selector;
		  the_insn.arg_reloc = last_call_desc.arg_reloc;
		  bzero (&last_call_desc, sizeof (call_descS));
		}
#else
	      if (the_insn.exp.X_add_symbol)
		{
		  if (strcmp (S_GET_NAME (the_insn.exp.X_add_symbol), "L0\001") == 0)
		    {
		      unsigned int w2, w1, w, result;

		      sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 17, &result);
		      dis_assemble_17 (result, &w1, &w2, &w);
		      opcode |= ((w2 << 2) | (w1 << 16) | w);
		    }
		  else
		    {
		      if (is_complex (the_insn.exp))
			the_insn.reloc = R_HPPA_COMPLEX_PCREL_CALL;
		      else
			the_insn.reloc = R_HPPA_PCREL_CALL;
		      the_insn.format = 17;
		      the_insn.arg_reloc = last_call_desc.arg_reloc;
		      bzero (&last_call_desc, sizeof (call_descS));
		    }
		}
	      else
		{
		  unsigned int w2, w1, w, result;

		  sign_unext (the_insn.exp.X_add_number >> 2, 17, &result);
		  dis_assemble_17 (result, &w1, &w2, &w);
		  opcode |= ((w2 << 2) | (w1 << 16) | w);
		}
#endif
	      s = expr_end;
	      continue;
	    case 'z':		/* 17 bit branch displacement (not pc-relative) */
#if defined(OBJ_ELF)
	      the_insn.field_selector = pa_chk_field_selector (&s);
#endif
	      getExpression (s);
	      the_insn.pcrel = 0;
#ifdef OBJ_SOM
	      if (strcmp (the_insn.exp.X_add_symbol->sy_nlist.n_un.n_name, "L0\001") == 0)
		{
		  unsigned int w2, w1, w, result;

		  sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 17, &result);
		  dis_assemble_17 (result, &w1, &w2, &w);
		  opcode |= ((w2 << 2) | (w1 << 16) | w);
		  the_insn.exp.X_add_symbol->sy_ref = FALSE;
		}
	      else
		{
		  /* this has to be wrong -- dont know what is right! */
		  the_insn.reloc = R_PCREL_CALL;
		  the_insn.code = 'W';
		  the_insn.field_selector = the_insn.exp.field_selector;
		  the_insn.arg_reloc = last_call_desc.arg_reloc;
		  bzero (&last_call_desc, sizeof (call_descS));
		}
#else
	      if (the_insn.exp.X_add_symbol)
		{
		  if (strcmp (S_GET_NAME (the_insn.exp.X_add_symbol), "L0\001") == 0)
		    {
		      unsigned int w2, w1, w, result;

		      sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 17, &result);
		      dis_assemble_17 (result, &w1, &w2, &w);
		      opcode |= ((w2 << 2) | (w1 << 16) | w);
		    }
		  else
		    {
		      if (is_complex (the_insn.exp))
			{
			  the_insn.reloc = R_HPPA_COMPLEX_ABS_CALL;
			}
		      else
			{
			  the_insn.reloc = R_HPPA_ABS_CALL;
			}
		      /* This could also be part of an instruction sequence of
				 interest.  If so, check to make sure that the previous
				 instruction's fixup is appropriate.  (ble, be instructions
                                 affect the reloc of immediately preceding ldil
                                 instructions.) */
		      if (strcasecmp (prev_str, "ldil") == 0 &&
			  prev_insn.exp.X_add_symbol == the_insn.exp.X_add_symbol &&
			  prev_insn.exp.X_subtract_symbol == the_insn.exp.X_subtract_symbol &&
			  prev_insn.exp.X_seg == the_insn.exp.X_seg &&
			  prev_insn.exp.X_add_number == the_insn.exp.X_add_number &&
			  prev_fix != NULL)
			prev_fix->fx_r_type = the_insn.reloc;

		      the_insn.format = 17;
		      the_insn.arg_reloc = last_call_desc.arg_reloc;
		      bzero (&last_call_desc, sizeof (call_descS));
		    }
		}
	      else
		{
		  unsigned int w2, w1, w, result;

		  sign_unext (the_insn.exp.X_add_number >> 2, 17, &result);
		  dis_assemble_17 (result, &w1, &w2, &w);
		  opcode |= ((w2 << 2) | (w1 << 16) | w);
		}
#endif
	      s = expr_end;
	      continue;
	    case 'p':		/* 5 bit shift count at 26 (to support SHD instr.) */
	      /* value is encoded in instr. as 31-p where p is   */
	      /* the value scanned here */
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  opcode |= (((31 - the_insn.exp.X_add_number) & 0x1f) << 5);
		}
	      s = expr_end;
	      continue;
	    case 'P':		/* 5-bit bit position at 26 */
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  opcode |= (the_insn.exp.X_add_number & 0x1f) << 5;
		}
	      s = expr_end;
	      continue;
	    case 'Q':		/* 5  bit immediate at 10 */
	      /* (unsigned bit position value for the bb instruction) */
	      getExpression (s);
	      im5 = evaluateAbsolute (the_insn.exp, the_insn.field_selector);
	      if (im5 > 31 || im5 < 0)
		{
		  as_bad ("Operand out of range. Was: %d. Should be [0..31]. Assuming %d.\n", im5, im5 & 0x1f);
		  im5 = im5 & 0x1f;
		}
	      opcode |= im5 << 21;
	      s = expr_end;
	      continue;
	    case 'A':		/* 13 bit immediate at 18 (to support BREAK instr.) */
	      getAbsoluteExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		opcode |= (the_insn.exp.X_add_number & 0x1fff) << 13;
	      s = expr_end;
	      continue;
	    case 'Z':		/* System Control Completer(for LDA, LHA, etc.) */
	      if (*s == ',' && (*(s + 1) == 'm' || *(s + 1) == 'M'))
		{
		  m = 1;
		  s += 2;
		}
	      else
		m = 0;

	      opcode |= m << 5;
	      while (*s == ' ' || *s == '\t')	/* skip to next operand */
		s++;

	      continue;
	    case 'D':		/* 26 bit immediate at 31 (to support DIAG instr.) */
	      /* the action (and interpretation of this operand is
			     implementation dependent) */
#if defined(OBJ_ELF)
	      the_insn.field_selector = pa_chk_field_selector (&s);
#endif
	      getExpression (s);
	      if (the_insn.exp.X_seg == &bfd_abs_section)
		{
		  opcode |= ((evaluateAbsolute (the_insn.exp, the_insn.field_selector) & 0x1ffffff) << 1);
#ifdef NEW_SOM			/* XXX what replaces this? */
		  /* PJH: VERY unsure about the following */
		  the_insn.field_selector = the_insn.exp.field_selector;
#endif
		}
	      else
		as_bad ("Illegal DIAG operand");
	      s = expr_end;
	      continue;
	    case 'f':		/* 3 bit Special Function Unit (SFU) identifier at 25 */
	      sfu = pa_parse_number (&s);
	      if ((sfu > 7) || (sfu < 0))
		as_bad ("Illegal SFU identifier: %02x", sfu);
	      opcode |= (sfu & 7) << 6;
	      continue;
	    case 'O':		/* 20 bit SFU op. split between 15 bits at 20
	                   and 5 bits at 31 */
	      getExpression (s);
	      s = expr_end;
	      continue;
	    case 'o':		/* 15 bit Special Function Unit operation at 20 */
	      getExpression (s);
	      s = expr_end;
	      continue;
	    case '2':		/* 22 bit SFU op. split between 17 bits at 20
	                   and 5 bits at 31 */
	      getExpression (s);
	      s = expr_end;
	      continue;
	    case '1':		/* 15 bit SFU op. split between 10 bits at 20
	                   and 5 bits at 31 */
	      getExpression (s);
	      s = expr_end;
	      continue;
	    case '0':		/* 10 bit SFU op. split between 5 bits at 20
	                   and 5 bits at 31 */
	      getExpression (s);
	      s = expr_end;
	      continue;
	    case 'u':		/* 3 bit coprocessor unit identifier at 25 */
	      getExpression (s);
	      s = expr_end;
	      continue;
	    case 'F':		/* Source FP Operand Format Completer (2 bits at 20) */
	      f = pa_parse_fp_format (&s);
	      opcode |= (int) f << 11;
	      the_insn.fpof1 = f;
	      continue;
	    case 'G':		/* Destination FP Operand Format Completer (2 bits at 18) */
	      s--;		/* need to pass the previous comma to pa_parse_fp_format */
	      f = pa_parse_fp_format (&s);
	      opcode |= (int) f << 13;
	      the_insn.fpof2 = f;
	      continue;
	    case 'M':		/* FP Compare Conditions (encoded as 5 bits at 31) */
	      cond = pa_parse_fp_cmp_cond (&s);
	      opcode |= cond;
	      continue;

	    case 'v':		/* a 't' type extended to handle L/R register halves. */
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    opcode |= (result.number_part & 0x1f);

		    /* 0x30 opcodes are FP arithmetic operation opcodes */
		    /* load/store FP opcodes do not get converted to 0x38 */
		    /* opcodes like the 0x30 opcodes do */
		    if (need_89_opcode (&the_insn, &result))
		      {
			if ((opcode & 0xfc000000) == 0x30000000)
			  {
			    opcode |= (result.L_R_select & 1) << 6;
			    opcode |= 1 << 27;
			  }
			else
			  {
			    opcode |= (result.L_R_select & 1) << 6;
			  }
		      }
		    continue;
		  }
	      }
	      break;
	    case 'E':		/* a 'b' type extended to handle L/R register halves. */
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    opcode |= (result.number_part & 0x1f) << 21;
		    if (need_89_opcode (&the_insn, &result))
		      {
			opcode |= (result.L_R_select & 1) << 7;
			opcode |= 1 << 27;
		      }
		    continue;
		  }
	      }
	      break;

	    case 'X':		/* an 'x' type extended to handle L/R register halves. */
	      {
		struct pa_89_fp_reg_struct result;
		int status;


		pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    opcode |= (result.number_part & 0x1f) << 16;
		    if (need_89_opcode (&the_insn, &result))
		      {
			opcode |= (result.L_R_select & 1) << 12;
			opcode |= 1 << 27;
		      }
		    continue;
		  }
	      }
	      break;

	    case '4':		/* 5 bit register field at 10
				 (used in 'fmpyadd' and 'fmpysub') */
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.L_R_select & 1) << 4;
		      }
		    opcode |= result.number_part << 21;
		    continue;
		  }
	      }
	      break;

	    case '6':		/* 5 bit register field at 15
				 (used in 'fmpyadd' and 'fmpysub') */
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.L_R_select & 1) << 4;
		      }
		    opcode |= result.number_part << 16;
		    continue;
		  }
	      }
	      break;

	    case '7':		/* 5 bit register field at 31
				 (used in 'fmpyadd' and 'fmpysub') */
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.L_R_select & 1) << 4;
		      }
		    opcode |= result.number_part;
		    continue;
		  }
	      }
	      break;

	    case '8':		/* 5 bit register field at 20
				 (used in 'fmpyadd' and 'fmpysub') */
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.L_R_select & 1) << 4;
		      }
		    opcode |= result.number_part << 11;
		    continue;
		  }
	      }
	      break;

	    case '9':		/* 5 bit register field at 25
				 (used in 'fmpyadd' and 'fmpysub') */
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_89_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.L_R_select & 1) << 4;
		      }
		    opcode |= result.number_part << 6;
		    continue;
		  }
	      }
	      break;

	    case 'H':		/* Floating Point Operand Format at 26 for       */
	      /* 'fmpyadd' and 'fmpysub' (very similar to 'F') */
	      /* bits are switched from other FP Operand       */
	      /* formats. 1=SGL, 1=<none>, 0=DBL               */
	      f = pa_parse_fp_format (&s);
	      switch (f)
		{
		case SGL:
		  opcode |= 0x20;
		case DBL:
		  the_insn.fpof1 = f;
		  continue;

		case QUAD:
		case ILLEGAL_FMT:
		default:
		  as_bad ("Illegal Floating Point Operand Format for this instruction: '%s'", *s);
		}
	      break;

	    default:
	      abort ();
	    }
	  break;
	}
    error:
      if (match == FALSE)
	{
	  /* Args don't match.  */
	  if (&insn[1] - pa_opcodes < NUMOPCODES
	      && !strcmp (insn->name, insn[1].name))
	    {
	      ++insn;
	      s = argsStart;
	      continue;
	    }
	  else
	    {
	      as_bad ("Illegal operands %s", error_message);
	      return;
	    }
	}
      break;
    }

  the_insn.opcode = opcode;

#ifdef PA_DEBUG
  if (the_insn.exp.X_add_symbol && the_insn.exp.X_subtract_symbol)
    print_insn_short (&the_insn);
  fprintf (stderr, "*********** END OF STATEMENT\n");
#endif

  return;
}

/*
    This is identical to the md_atof in m68k.c.  I think this is right,
    but I'm not sure.

   Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message is returned, or NULL on OK.
 */

/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

char *
md_atof (type, litP, sizeP)
     char type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[MAX_LITTLENUMS];
  LITTLENUM_TYPE *wordP;
  char *t;
  char *atof_ieee ();

  switch (type)
    {

    case 'f':
    case 'F':
    case 's':
    case 'S':
      prec = 2;
      break;

    case 'd':
    case 'D':
    case 'r':
    case 'R':
      prec = 4;
      break;

    case 'x':
    case 'X':
      prec = 6;
      break;

    case 'p':
    case 'P':
      prec = 6;
      break;

    default:
      *sizeP = 0;
      return "Bad call to MD_ATOF()";
    }
  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;
  *sizeP = prec * sizeof (LITTLENUM_TYPE);
  for (wordP = words; prec--;)
    {
      md_number_to_chars (litP, (long) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }
  return "";			/* Someone should teach Dean about null pointers */
}

/*
 * Write out big-endian.
 */
void
md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{

  switch (n)
    {

    case 4:
      *buf++ = val >> 24;
      *buf++ = val >> 16;
    case 2:
      *buf++ = val >> 8;
    case 1:
      *buf = val;
      break;

    default:
      abort ();
    }
  return;
}

void
md_create_short_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr, to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  fprintf (stderr, "pa_create_short_jmp\n");
  abort ();
}

void
md_number_to_disp (buf, val, n)
     char *buf;
     long val;
     int n;
{
  fprintf (stderr, "md_number_to_disp\n");
  abort ();
}

void
md_number_to_field (buf, val, fix)
     char *buf;
     long val;
     void *fix;
{
  fprintf (stderr, "pa_number_to_field\n");
  abort ();
}

/* the bit-field entries in the relocation_info struct plays hell
   with the byte-order problems of cross-assembly.  So as a hack,
   I added this mach. dependent ri twiddler.  Ugly, but it gets
   you there. -KWK */
/* on sparc: first 4 bytes are normal unsigned long address, next three
   bytes are index, most sig. byte first.  Byte 7 is broken up with
   bit 7 as external, bits 6 & 5 unused, and the lower
   five bits as relocation type.  Next 4 bytes are long int addend. */
/* Thanx and a tip of the hat to Michael Bloom, mb@ttidca.tti.com */

#ifdef OBJ_SOM
void
md_ri_to_chars (ri_p, ri)
     struct reloc_info_pa *ri_p, ri;
{
  unsigned char the_bytes[sizeof (*ri_p)];
#if defined(OBJ_SOM) | defined(OBJ_OSFROSE) | defined(OBJ_ELF)
  /* not sure what, if any, changes are required for new-style cross-assembly */
#else
  the_bytes[0] = ((ri.need_data_ref << 7) & 0x80) | ((ri.arg_reloc & 0x03f8) >> 3);
  the_bytes[1] = ((ri.arg_reloc & 0x07) << 5) | ri.expression_type;
  the_bytes[2] = ((ri.exec_level << 6) & 0xc0) | ri.fixup_format;
  the_bytes[3] = ri.fixup_field & 0xff;
  md_number_to_chars (&the_bytes[4], ri.subspace_offset, sizeof (ri.subspace_offset));
  md_number_to_chars (&the_bytes[8], ri.symbol_index_one, sizeof (ri.symbol_index_one));
  md_number_to_chars (&the_bytes[12], ri.symbol_index_two, sizeof (ri.symbol_index_two));
  md_number_to_chars (&the_bytes[16], ri.fixup_constant, sizeof (ri.fixup_constant));

  /* now put it back where you found it, Junior... */
  bcopy (the_bytes, (char *) ri_p, sizeof (*ri_p));
#endif

}

#endif


/* Translate internal representation of relocation info to BFD target
   format.  */
/* This may need additional work to make sure that SOM support is complete. */
#ifdef OBJ_ELF
arelent **
tc_gen_reloc (section, fixp)
     asection *section;
     fixS *fixp;
{
  arelent *reloc;
  hppa_fixS *hppa_fixp = hppa_find_hppa_fix (fixp);
  bfd_reloc_code_real_type code;
  static int unwind_reloc_fixp_cnt = 0;
  static arelent *unwind_reloc_entryP = NULL;
  static arelent *no_relocs = NULL;
  arelent **relocs;
  bfd_reloc_code_real_type **codes;
  int n_relocs;
  int i;

  if (fixp->fx_addsy == 0)
    return &no_relocs;
  assert (hppa_fixp != 0);
  assert (section != 0);

  /* unwind section relocations are handled in a special way. */
  /* The relocations for the .unwind section are originally */
  /* built in the usual way.  That is, for each unwind table */
  /* entry there are two relocations:  one for the beginning of */
  /* the function and one for the end.	*/

  /* The first time we enter this function we create a */
  /* relocation of the type R_HPPA_UNWIND_ENTRIES.  The addend */
  /* of the relocation is initialized to 0.  Each additional */
  /* pair of times this function is called for the unwind */
  /* section represents an additional unwind table entry.  Thus, */
  /* the addend of the relocation should end up to be the number */
  /* of unwind table entries. */
  if (strcmp (UNWIND_SECTION_NAME, section->name) == 0)
    {
      if (unwind_reloc_entryP == NULL)
	{
	  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));
	  assert (reloc != 0);
	  unwind_reloc_entryP = reloc;
	  unwind_reloc_fixp_cnt++;
	  unwind_reloc_entryP->address = fixp->fx_frag->fr_address + fixp->fx_where;
	  /* a pointer any function will do.  We only */
	  /* need one to tell us what section the unwind */
	  /* relocations are for. */
	  unwind_reloc_entryP->sym_ptr_ptr = &fixp->fx_addsy->bsym;
	  code = R_HPPA_UNWIND_ENTRIES;
	  unwind_reloc_entryP->howto = bfd_reloc_type_lookup (stdoutput, code);
	  unwind_reloc_entryP->addend = unwind_reloc_fixp_cnt / 2;
	  relocs = (arelent **) bfd_alloc_by_size_t (stdoutput, sizeof (arelent *) * 2);
	  assert (relocs != 0);
	  relocs[0] = unwind_reloc_entryP;
	  relocs[1] = NULL;
	  return relocs;
	}
      unwind_reloc_fixp_cnt++;
      unwind_reloc_entryP->addend = unwind_reloc_fixp_cnt / 2;

      return &no_relocs;
    }

  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));
  assert (reloc != 0);

  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  /* XXX: might need additional processing here	*/
  /* hppa_elf_gen_reloc_type() is defined in the	*/
  /* ELF/PA BFD back-end				*/
  codes = hppa_elf_gen_reloc_type (stdoutput,
				   fixp->fx_r_type,
				   hppa_fixp->fx_r_format,
				   hppa_fixp->fx_r_field);

  for (n_relocs = 0; codes[n_relocs]; n_relocs++)
    ;

  relocs = (arelent **) bfd_alloc_by_size_t (stdoutput, sizeof (arelent *) * n_relocs + 1);
  assert (relocs != 0);

  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent) * n_relocs);
  if (n_relocs > 0)
    assert (reloc != 0);

  for (i = 0; i < n_relocs; i++)
    relocs[i] = &reloc[i];

  relocs[n_relocs] = NULL;

  switch (fixp->fx_r_type)
    {
    case R_HPPA_COMPLEX:
    case R_HPPA_COMPLEX_PCREL_CALL:
    case R_HPPA_COMPLEX_ABS_CALL:
      assert (n_relocs == 5);

      for (i = 0; i < n_relocs; i++)
	{
	  reloc[i].sym_ptr_ptr = NULL;
	  reloc[i].address = 0;
	  reloc[i].addend = 0;
	  reloc[i].howto = bfd_reloc_type_lookup (stdoutput, *codes[i]);
	  assert (reloc[i].howto && *codes[i] == reloc[i].howto->type);
	}

      reloc[0].sym_ptr_ptr = &fixp->fx_addsy->bsym;
      reloc[1].sym_ptr_ptr = &fixp->fx_subsy->bsym;
      reloc[4].address = fixp->fx_frag->fr_address + fixp->fx_where;

      if (fixp->fx_r_type == R_HPPA_COMPLEX)
	reloc[3].addend = fixp->fx_addnumber;
      else if (fixp->fx_r_type == R_HPPA_COMPLEX_PCREL_CALL ||
	       fixp->fx_r_type == R_HPPA_COMPLEX_ABS_CALL)
	reloc[1].addend = fixp->fx_addnumber;

      break;

    default:
      assert (n_relocs == 1);

      code = *codes[0];

      reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
      reloc->howto = bfd_reloc_type_lookup (stdoutput, code);
      reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
      reloc->addend = 0;	/* default */

      assert (reloc->howto && code == reloc->howto->type);

      /* Now, do any processing that is dependent on the relocation */
      /* type. */
      switch (code)
	{
	case R_HPPA_PLABEL_32:
	case R_HPPA_PLABEL_11:
	case R_HPPA_PLABEL_14:
	case R_HPPA_PLABEL_L21:
	case R_HPPA_PLABEL_R11:
	case R_HPPA_PLABEL_R14:
	  /* For plabel relocations, the addend of the */
	  /* relocation should be either 0 (no static link) or 2 */
	  /* (static link required).	*/
	  /* XXX: assume that fx_addnumber contains this */
	  /* information */
	  reloc->addend = fixp->fx_addnumber;
	  break;

	case R_HPPA_ABS_CALL_11:
	case R_HPPA_ABS_CALL_14:
	case R_HPPA_ABS_CALL_17:
	case R_HPPA_ABS_CALL_L21:
	case R_HPPA_ABS_CALL_R11:
	case R_HPPA_ABS_CALL_R14:
	case R_HPPA_ABS_CALL_R17:
	case R_HPPA_ABS_CALL_LS21:
	case R_HPPA_ABS_CALL_RS11:
	case R_HPPA_ABS_CALL_RS14:
	case R_HPPA_ABS_CALL_RS17:
	case R_HPPA_ABS_CALL_LD21:
	case R_HPPA_ABS_CALL_RD11:
	case R_HPPA_ABS_CALL_RD14:
	case R_HPPA_ABS_CALL_RD17:
	case R_HPPA_ABS_CALL_LR21:
	case R_HPPA_ABS_CALL_RR14:
	case R_HPPA_ABS_CALL_RR17:

	case R_HPPA_PCREL_CALL_11:
	case R_HPPA_PCREL_CALL_14:
	case R_HPPA_PCREL_CALL_17:
	case R_HPPA_PCREL_CALL_L21:
	case R_HPPA_PCREL_CALL_R11:
	case R_HPPA_PCREL_CALL_R14:
	case R_HPPA_PCREL_CALL_R17:
	case R_HPPA_PCREL_CALL_LS21:
	case R_HPPA_PCREL_CALL_RS11:
	case R_HPPA_PCREL_CALL_RS14:
	case R_HPPA_PCREL_CALL_RS17:
	case R_HPPA_PCREL_CALL_LD21:
	case R_HPPA_PCREL_CALL_RD11:
	case R_HPPA_PCREL_CALL_RD14:
	case R_HPPA_PCREL_CALL_RD17:
	case R_HPPA_PCREL_CALL_LR21:
	case R_HPPA_PCREL_CALL_RR14:
	case R_HPPA_PCREL_CALL_RR17:
	  /* constant is stored in the instruction */
	  reloc->addend = ELF32_HPPA_R_ADDEND (hppa_fixp->fx_arg_reloc, 0);
	  break;
	default:
	  reloc->addend = fixp->fx_addnumber;
	  break;
	}
      break;
    }

  return relocs;
}

#else
arelent *
tc_gen_reloc (section, fixp)
     asection *section;
     fixS *fixp;
{
  arelent *reloc;
  hppa_fixS *hppa_fixp = hppa_find_hppa_fix (fixp);
  bfd_reloc_code_real_type code;

  if (fixp->fx_addsy == 0)
    return 0;
  assert (hppa_fixp != 0);
  assert (section != 0);

  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));
  assert (reloc != 0);

  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  /* XXX: might need additional processing here   */
  /* hppa_elf_gen_reloc_type() is defined in the  */
  /* ELF/PA BFD back-end                          */
  code = hppa_elf_gen_reloc_type (stdoutput,
				  fixp->fx_r_type,
				  hppa_fixp->fx_r_format,
				  hppa_fixp->fx_r_field);
  reloc->howto = bfd_reloc_type_lookup (stdoutput, code);

  assert (code == reloc->howto->type);

  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->addend = 0;		/* default */

  /* Now, do any processing that is dependent on the relocation */
  /* type. (Is there any?) */
  switch (code)
    {
    default:
      reloc->addend = fixp->fx_addnumber;
      break;
    }

  return reloc;
}

#endif

void
md_convert_frag (abfd, sec, fragP)
     register bfd *abfd;
     register asection *sec;
     register fragS *fragP;
{
  unsigned int address;

  if (fragP->fr_type == rs_machine_dependent)
    {
      switch ((int) fragP->fr_subtype)
	{
	case 0:
	  fragP->fr_type = rs_fill;
	  know (fragP->fr_var == 1);
	  know (fragP->fr_next);
	  address = fragP->fr_address + fragP->fr_fix;
	  if (address % fragP->fr_offset)
	    {
	      fragP->fr_offset =
		fragP->fr_next->fr_address
		- fragP->fr_address
		- fragP->fr_fix;
	    }
	  else
	    fragP->fr_offset = 0;
	  break;
	}
    }
}

/* Round up a section size to the appropriate boundary. */
valueT
md_section_align (segment, size)
     asection *segment;
     valueT size;
{
  return (size + 7) & ~7;	/* Round all sects to multiple of 8 */
}				/* md_section_align() */

void
md_create_long_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr, to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  fprintf (stderr, "pa_create_long_jump\n");
  abort ();
}

int
/* md_estimate_size_before_relax(fragP, segtype) */
md_estimate_size_before_relax (fragP, segment)
     register fragS *fragP;
     asection *segment;
{
  int size;

  size = 0;

  while ((fragP->fr_fix + size) % fragP->fr_offset)
    size++;

  return size;
}

int
md_parse_option (argP, cntP, vecP)
     char **argP;
     int *cntP;
     char ***vecP;
{
  return 1;
}

/* We have no need to default values of symbols. */

/* ARGSUSED */
symbolS *
md_undefined_symbol (name)
     char *name;
{
  return 0;
}				/*md_undefined_symbol() */

/* Parse an operand that is machine-specific.
   We just return without modifying the expression if we have nothing
   to do.  */

/* ARGSUSED */
void
md_operand (expressionP)
     expressionS *expressionP;
{
}

/* Apply a fixS to the frags, now that we know the value it ought to
   hold. */

int 
apply_field_selector (value, constant, field_selector)
     long value;
     long constant;
     int field_selector;
{
  /* hppa_field_adjust() is defined in the HPPA target */
  return hppa_field_adjust (value, constant, field_selector);
}

void 
md_apply_fix_1 (fixP, val)
     fixS *fixP;
     long val;
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  hppa_fixS *hppa_fixP = hppa_find_hppa_fix (fixP);
  long new_val;
  long result;
  unsigned int w1, w2, w;
  /* The following routine is defined in the ELF/PA back-end	*/
  extern unsigned char hppa_elf_insn2fmt ();

  if (hppa_fixP)
    {
      unsigned long buf_wd = bfd_get_32 (stdoutput, buf);
      unsigned char fmt = hppa_elf_insn2fmt (fixP->fx_r_type, buf_wd);

      assert (fixP->fx_r_type < R_HPPA_UNIMPLEMENTED);
      assert (fixP->fx_r_type >= R_HPPA_NONE);

      fixP->fx_addnumber = val;	/* Remember value for emit_reloc */

      /* Check if this is an undefined symbol.  No relocation can	*/
      /* possibly be performed in this case.				*/

      if ((fixP->fx_addsy && fixP->fx_addsy->bsym->section == &bfd_und_section)
	  || (fixP->fx_subsy && fixP->fx_subsy->bsym->section == &bfd_und_section))
	return;

      /* Perform some processing particular to unwind */
      /* relocations */

      if (hppa_fixP->fx_call_infop
	  && (((fixP == hppa_fixP->fx_call_infop->start_fix)
	       && (fixP->fx_addsy ==
		   hppa_fixP->fx_call_infop->start_symbol))
	      || ((fixP == hppa_fixP->fx_call_infop->end_fix)
		  && (fixP->fx_addsy ==
		      hppa_fixP->fx_call_infop->end_symbol))
	  ))
	val += fixP->fx_addsy->sy_frag->fr_address;

      switch (fmt)
	{

	case 14:		/* all the opcodes with the 'j' operand type */
	  new_val = apply_field_selector (val, 0, hppa_fixP->fx_r_field);
	  /* need to check for overflow here */

	  /* mask off 14 bits to be changed */
	  /* *(long *)buf = *(long *)buf & 0xffffc000; */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffffc000,
		      buf);
	  low_sign_unext (new_val, 14, &result);
	  break;

	case 21:		/* all the opcodes with the 'k' operand type */
	  new_val = apply_field_selector (val, 0, hppa_fixP->fx_r_field);
	  /* need to check for overflow here */

	  /* mask off 21 bits to be changed */
	  /* *(long *)buf = *(long *)buf & 0xffe00000; */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffe00000,
		      buf);
	  dis_assemble_21 (new_val, &result);
	  break;

	case 11:		/* all the opcodes with the 'i' operand type */
	  new_val = apply_field_selector (val, 0, hppa_fixP->fx_r_field);
	  /* need to check for overflow here */

	  /* mask off 11 bits to be changed */
	  /* *(long *)buf = *(long *)buf & 0xffff800; */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffff800,
		      buf);
	  low_sign_unext (new_val, 11, &result);
	  break;

	case 12:		/* all the opcodes with the 'w' operand type */
	  new_val = apply_field_selector (val, 0, hppa_fixP->fx_r_field);

	  /* mask off 11 bits to be changed */
	  sign_unext ((new_val - 8) >> 2, 12, &result);
	  /* *(long *)buf = *(long *)buf & 0xffffe002; */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffffe002,
		      buf);

	  dis_assemble_12 (result, &w1, &w);
	  result = ((w1 << 2) | w);
	  break;

	case 17:		/* some of the opcodes with the 'W' operand type */
	  new_val = apply_field_selector (val, 0, hppa_fixP->fx_r_field);
	  /* need to check for overflow here */

	  /* mask off 17 bits to be changed */
	  /* *(long *)buf = *(long *)buf & 0xffe0e002; */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffe0e002,
		      buf);
	  sign_unext ((new_val - 8) >> 2, 17, &result);
	  dis_assemble_17 (result, &w1, &w2, &w);
	  result = ((w2 << 2) | (w1 << 16) | w);
	  break;

	case 32:
	  new_val = apply_field_selector (val, 0, hppa_fixP->fx_r_field);
	  result = new_val;	/* no transformation on result */
	  /* *(long *)buf = 0; *//* clear out everything */
	  bfd_put_32 (stdoutput, 0, buf);	/* clear out everything */
	  break;

	case 0:
	  return;

	default:
	  as_bad ("bad relocation type/fmt: 0x%02x/0x%02x",
		  fixP->fx_r_type, fmt);
	  return;
	}
      buf[0] |= (result & 0xff000000) >> 24;
      buf[1] |= (result & 0x00ff0000) >> 16;
      buf[2] |= (result & 0x0000ff00) >> 8;
      buf[3] |= result & 0x000000ff;
      /* We've now adjusted for fx_addnumber, we can */
      /* forget it now. */
      fixP->fx_addnumber = 0;
    }
  else
    {
      printf ("no hppa_fixup entry for this fixup (fixP = 0x%x, type = 0x%x)\n",
	      fixP, fixP->fx_r_type);
    }
}				/* md_apply_fix_1() */

#ifdef BFD_ASSEMBLER
int
md_apply_fix (fixP, valp)
     fixS *fixP;
     long *valp;
{
  md_apply_fix_1 (fixP, *valp);
  return 1;
}

#else
void
md_apply_fix (fixP, val)
     fixS *fixP;
     long val;
{
  md_apply_fix_1 (fixP, val);
}

#endif

/* Exactly what point is a PC-relative offset relative TO?
   On the PA, they're relative to the address of the offset.
   (??? Is this right?  FIXME-SOON) */
long 
md_pcrel_from (fixP)
     fixS *fixP;
{
  return fixP->fx_where + fixP->fx_frag->fr_address;
}				/* md_pcrel_from() */

int 
is_end_of_statement ()
{
  return ((*input_line_pointer == '\n')
	  || (*input_line_pointer == ';')
	  || (*input_line_pointer == '!'));
}

/* pa-aux.c -- Assembler for the PA - PA-RISC specific support routines */

struct aux_hdr_list *aux_hdr_root = NULL;

int print_errors = 1;

void 
pa_skip (s)
     char **s;
{
  while (**s == ' ' || **s == '\t')
    *s = *s + 1;
}

int 
pa_parse_number (s)
     char **s;
{
  int num;
  char *name;
  char c;
  symbolS *sym;
  int status;
  char *p = *s;

  while (*p == ' ' || *p == '\t')
    p = p + 1;
  num = -1;			/* assume invalid number to begin with */
  if (isdigit (*p))
    {
      num = 0;			/* now we know it is a number */

      if (*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X'))
	{			/* hex input */
	  p = p + 2;
	  while (isdigit (*p) || ((*p >= 'a') && (*p <= 'f'))
		 || ((*p >= 'A') && (*p <= 'F')))
	    {
	      if (isdigit (*p))
		num = num * 16 + *p - '0';
	      else if (*p >= 'a' && *p <= 'f')
		num = num * 16 + *p - 'a' + 10;
	      else
		num = num * 16 + *p - 'A' + 10;
	      ++p;
	    }
	}
      else
	{
	  while (isdigit (*p))
	    {
	      num = num * 10 + *p - '0';
	      ++p;
	    }
	}
    }
  else if (*p == '%')
    {				/* could be a pre-defined register */
      num = 0;
      name = p;
      p++;
      c = *p;
      /* tege hack: Special case for general registers
       as the general code makes a binary search with case translation,
       and is VERY slow.  */
      if (c == 'r')
	{
	  p++;
	  if (*p == 'e' && *(p + 1) == 't' && (*(p + 2) == '0' || *(p + 2) == '1'))
	    {
	      p += 2;
	      num = *p - '0' + 28;	/* r28 is ret0 */
	      p++;
	    }
	  else if (!isdigit (*p))
	    as_bad ("Undefined register: '%s'. ASSUMING 0", name);
	  else
	    {
	      do
		num = num * 10 + *p++ - '0';
	      while (isdigit (*p));
	    }
	}
      else
	{
	  while (is_part_of_name (c))
	    {
	      p = p + 1;
	      c = *p;
	    }
	  *p = 0;
	  status = reg_name_search (name);
	  if (status >= 0)
	    num = status;
	  else
	    {
	      if (print_errors)
		as_bad ("Undefined register: '%s'. ASSUMING 0", name);
	      else
		num = -1;
	    }
	  *p = c;
	}
    }
  else
    {
      num = 0;
      name = p;
      c = *p;
      while (is_part_of_name (c))
	{
	  p = p + 1;
	  c = *p;
	}
      *p = 0;
      if ((sym = symbol_find (name)) != NULL)
	{
#ifdef OBJ_SOM
	  if (sym->pa_sy_type == ST_ABSOLUTE)
	    {
	      num = sym->pa_sy_value;
#else
	  if (S_GET_SEGMENT (sym) == &bfd_abs_section)
	    {
	      num = S_GET_VALUE (sym);
#endif
	    }
	  else
	    {
	      if (print_errors)
		as_bad ("Non-absolute constant: '%s'. ASSUMING 0", name);
	      else
		num = -1;
	    }
	}
      else
	{
	  if (print_errors)
	    as_bad ("Undefined absolute constant: '%s'. ASSUMING 0", name);
	  else
	    num = -1;
	}
      *p = c;
    }

  *s = p;
  return num;
}

struct pd_reg
  {
    char *name;
    int value;
  };

/*	List of registers that are pre-defined:

	General Registers:

	Name	Value		Name	Value
	%r0	0		%r16	16
	%r1	1		%r17	17
	%r2	2		%r18	18
	%r3	3		%r19	19
	%r4	4		%r20	20
	%r5	5		%r21	21
	%r6	6		%r22	22
	%r7	7		%r23	23
	%r8	8		%r24	24
	%r9	9		%r25	25
	%r10	10		%r26	26
	%r11	11		%r27	27
	%r12	12		%r28	28
	%r13	13		%r29	29
	%r14	14		%r30	30
	%r15	15		%r31	31

	Floating-point Registers:
	[NOTE:  Also includes L and R versions of these (e.g. %fr19L, %fr19R)]

	Name	Value		Name	Value
	%fr0	0		%fr16	16
	%fr1	1		%fr17	17
	%fr2	2		%fr18	18
	%fr3	3		%fr19	19
	%fr4	4		%fr20	20
	%fr5	5		%fr21	21
	%fr6	6		%fr22	22
	%fr7	7		%fr23	23
        %fr8	8		%fr24	24
	%fr9	9		%fr25	25
	%fr10	10		%fr26	26
	%fr11	11		%fr27	27
	%fr12	12		%fr28	28
	%fr13	13		%fr29	29
	%fr14	14		%fr30	30
	%fr15	15		%fr31	31

	Space Registers:

	Name	Value		Name	Value
	%sr0	0		%sr4	4
	%sr1	1		%sr5	5
	%sr2	2		%sr6	6
	%sr3	3		%sr7	7

	Control registers and their synonyms:

	Names			Value
	%cr0	%rctr		0
	%cr8	%pidr1		8
	%cr9	%pidr2		9
	%cr10	%ccr		10
	%cr11	%sar		11
	%cr12	%pidr3		12
	%cr13	%pidr4		13
	%cr14	%iva		14
	%cr15	%eiem		15
	%cr16	%itmr		16
	%cr17	%pcsq		17
	%cr18	%pcoq		18
	%cr19	%iir		19
	%cr20	%isr		20
	%cr21	%ior		21
	%cr22	%ipsw		22
	%cr23	%eirr		23
	%cr24	%tr0 %ppda	24
	%cr25	%tr1 %hta	25
	%cr26	%tr2		26
	%cr27	%tr3		27
	%cr28	%tr4		28
	%cr29	%tr5		29
	%cr30	%tr6		30
	%cr31	%tr7		31

	Miscellaneous registers and their synonyms:

	Names			Value
	%arg0			26
	%arg1			25
	%arg2			24
	%arg3			23
	%sp			30
	%ret0			28
	%ret1			29
*/

/* This table is sorted. Suitable for searching by a binary search. */

static struct pd_reg pre_defined_registers[] =
{
  {"%arg0", 26},
  {"%arg1", 25},
  {"%arg2", 24},
  {"%arg3", 23},
  {"%cr0", 0},
  {"%cr10", 10},
  {"%cr11", 11},
  {"%cr12", 12},
  {"%cr13", 13},
  {"%cr14", 14},
  {"%cr15", 15},
  {"%cr16", 16},
  {"%cr17", 17},
  {"%cr18", 18},
  {"%cr19", 19},
  {"%cr20", 20},
  {"%cr21", 21},
  {"%cr22", 22},
  {"%cr23", 23},
  {"%cr24", 24},
  {"%cr25", 25},
  {"%cr26", 26},
  {"%cr27", 27},
  {"%cr28", 28},
  {"%cr29", 29},
  {"%cr30", 30},
  {"%cr31", 31},
  {"%cr8", 8},
  {"%cr9", 9},
  {"%eiem", 15},
  {"%eirr", 23},
  {"%fr0", 0},
  {"%fr0L", 0},
  {"%fr0R", 0},
  {"%fr1", 1},
  {"%fr10", 10},
  {"%fr10L", 10},
  {"%fr10R", 10},
  {"%fr11", 11},
  {"%fr11L", 11},
  {"%fr11R", 11},
  {"%fr12", 12},
  {"%fr12L", 12},
  {"%fr12R", 12},
  {"%fr13", 13},
  {"%fr13L", 13},
  {"%fr13R", 13},
  {"%fr14", 14},
  {"%fr14L", 14},
  {"%fr14R", 14},
  {"%fr15", 15},
  {"%fr15L", 15},
  {"%fr15R", 15},
  {"%fr16", 16},
  {"%fr16L", 16},
  {"%fr16R", 16},
  {"%fr17", 17},
  {"%fr17L", 17},
  {"%fr17R", 17},
  {"%fr18", 18},
  {"%fr18L", 18},
  {"%fr18R", 18},
  {"%fr19", 19},
  {"%fr19L", 19},
  {"%fr19R", 19},
  {"%fr1L", 1},
  {"%fr1R", 1},
  {"%fr2", 2},
  {"%fr20", 20},
  {"%fr20L", 20},
  {"%fr20R", 20},
  {"%fr21", 21},
  {"%fr21L", 21},
  {"%fr21R", 21},
  {"%fr22", 22},
  {"%fr22L", 22},
  {"%fr22R", 22},
  {"%fr23", 23},
  {"%fr23L", 23},
  {"%fr23R", 23},
  {"%fr24", 24},
  {"%fr24L", 24},
  {"%fr24R", 24},
  {"%fr25", 25},
  {"%fr25L", 25},
  {"%fr25R", 25},
  {"%fr26", 26},
  {"%fr26L", 26},
  {"%fr26R", 26},
  {"%fr27", 27},
  {"%fr27L", 27},
  {"%fr27R", 27},
  {"%fr28", 28},
  {"%fr28L", 28},
  {"%fr28R", 28},
  {"%fr29", 29},
  {"%fr29L", 29},
  {"%fr29R", 29},
  {"%fr2L", 2},
  {"%fr2R", 2},
  {"%fr3", 3},
  {"%fr30", 30},
  {"%fr30L", 30},
  {"%fr30R", 30},
  {"%fr31", 31},
  {"%fr31L", 31},
  {"%fr31R", 31},
  {"%fr3L", 3},
  {"%fr3R", 3},
  {"%fr4", 4},
  {"%fr4L", 4},
  {"%fr4R", 4},
  {"%fr5", 5},
  {"%fr5L", 5},
  {"%fr5R", 5},
  {"%fr6", 6},
  {"%fr6L", 6},
  {"%fr6R", 6},
  {"%fr7", 7},
  {"%fr7L", 7},
  {"%fr7R", 7},
  {"%fr8", 8},
  {"%fr8L", 8},
  {"%fr8R", 8},
  {"%fr9", 9},
  {"%fr9L", 9},
  {"%fr9R", 9},
  {"%hta", 25},
  {"%iir", 19},
  {"%ior", 21},
  {"%ipsw", 22},
  {"%isr", 20},
  {"%itmr", 16},
  {"%iva", 14},
  {"%pcoq", 18},
  {"%pcsq", 17},
  {"%pidr1", 8},
  {"%pidr2", 9},
  {"%pidr3", 12},
  {"%pidr4", 13},
  {"%ppda", 24},
  {"%r0", 0},
  {"%r1", 1},
  {"%r10", 10},
  {"%r11", 11},
  {"%r12", 12},
  {"%r13", 13},
  {"%r14", 14},
  {"%r15", 15},
  {"%r16", 16},
  {"%r17", 17},
  {"%r18", 18},
  {"%r19", 19},
  {"%r2", 2},
  {"%r20", 20},
  {"%r21", 21},
  {"%r22", 22},
  {"%r23", 23},
  {"%r24", 24},
  {"%r25", 25},
  {"%r26", 26},
  {"%r27", 27},
  {"%r28", 28},
  {"%r29", 29},
  {"%r3", 3},
  {"%r30", 30},
  {"%r31", 31},
  {"%r4", 4},
  {"%r4L", 4},
  {"%r4R", 4},
  {"%r5", 5},
  {"%r5L", 5},
  {"%r5R", 5},
  {"%r6", 6},
  {"%r6L", 6},
  {"%r6R", 6},
  {"%r7", 7},
  {"%r7L", 7},
  {"%r7R", 7},
  {"%r8", 8},
  {"%r8L", 8},
  {"%r8R", 8},
  {"%r9", 9},
  {"%r9L", 9},
  {"%r9R", 9},
  {"%rctr", 0},
  {"%ret0", 28},
  {"%ret1", 29},
  {"%sar", 11},
  {"%sp", 30},
  {"%sr0", 0},
  {"%sr1", 1},
  {"%sr2", 2},
  {"%sr3", 3},
  {"%sr4", 4},
  {"%sr5", 5},
  {"%sr6", 6},
  {"%sr7", 7},
  {"%tr0", 24},
  {"%tr1", 25},
  {"%tr2", 26},
  {"%tr3", 27},
  {"%tr4", 28},
  {"%tr5", 29},
  {"%tr6", 30},
  {"%tr7", 31}
};

#define REG_NAME_CNT	(sizeof(pre_defined_registers) / sizeof(struct pd_reg))

int 
reg_name_search (name)
     char *name;
{
  int x, l, r;

  l = 0;
  r = REG_NAME_CNT - 1;

  do
    {
      x = (l + r) / 2;
      if (strcasecmp (name, pre_defined_registers[x].name) < 0)
	r = x - 1;
      else
	l = x + 1;
    }
  while (!((strcasecmp (name, pre_defined_registers[x].name) == 0) ||
	   (l > r)));

  if (strcasecmp (name, pre_defined_registers[x].name) == 0)
    return (pre_defined_registers[x].value);
  else
    return (-1);

}

int 
is_pre_defined_register (s)
     char *s;
{
  if (reg_name_search (s) >= 0)
    return (TRUE);
  else
    return (FALSE);
}

int 
is_R_select (s)
     char *s;
{

  if (*s == 'R' || *s == 'r')
    return (TRUE);
  else
    return (FALSE);
}

int 
is_L_select (s)
     char *s;
{

  if (*s == 'L' || *s == 'l')
    return (TRUE);
  else
    return (FALSE);
}

int 
need_89_opcode (insn, result)
     struct pa_it *insn;
     struct pa_89_fp_reg_struct *result;
{
  /* if ( result->L_R_select == 1 || insn->fpof1 == DBL || insn->fpof2 == DBL ) */
  /* if (result->L_R_select == 1 && !(insn->fpof1 == DBL || insn->fpof2 == DBL) ) */
  if (result->L_R_select == 1 && !(insn->fpof1 == DBL && insn->fpof2 == DBL))
    /* if ( insn->fpof1 == DBL || insn->fpof2 == DBL ) */
    return TRUE;
  else
    return FALSE;
}

int
pa_89_parse_number (s, result)
     char **s;
     struct pa_89_fp_reg_struct *result;
{
  int num;
  char *name;
  char c;
  symbolS *sym;
  int status;
  char *p = *s;

  while (*p == ' ' || *p == '\t')
    p = p + 1;
  num = -1;			/* assume invalid number to begin with */
  result->number_part = -1;
  result->L_R_select = -1;

  if (isdigit (*p))
    {
      num = 0;			/* now we know it is a number */

      if (*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X'))
	{			/* hex input */
	  p = p + 2;
	  while (isdigit (*p) || ((*p >= 'a') && (*p <= 'f'))
		 || ((*p >= 'A') && (*p <= 'F')))
	    {
	      if (isdigit (*p))
		num = num * 16 + *p - '0';
	      else if (*p >= 'a' && *p <= 'f')
		num = num * 16 + *p - 'a' + 10;
	      else
		num = num * 16 + *p - 'A' + 10;
	      ++p;
	    }
	}
      else
	{
	  while (isdigit (*p))
	    {
	      num = num * 10 + *p - '0';
	      ++p;
	    }
	}

      result->number_part = num;

      if (is_R_select (p))
	{
	  result->L_R_select = 1;
	  ++p;
	}
      else if (is_L_select (p))
	{
	  result->L_R_select = 0;
	  ++p;
	}
      else
	result->L_R_select = 0;

    }
  else if (*p == '%')
    {				/* could be a pre-defined register */
      num = 0;
      name = p;
      p++;
      c = *p;
      /* tege hack: Special case for general registers
       as the general code makes a binary search with case translation,
       and is VERY slow.  */
      if (c == 'r')
	{
	  p++;
	  if (*p == 'e' && *(p + 1) == 't' && (*(p + 2) == '0' || *(p + 2) == '1'))
	    {
	      p += 2;
	      num = *p - '0' + 28;	/* r28 is ret0 */
	      p++;
	    }
	  else if (!isdigit (*p))
	    as_bad ("Undefined register: '%s'. ASSUMING 0", name);
	  else
	    {
	      do
		num = num * 10 + *p++ - '0';
	      while (isdigit (*p));
	    }
	}
      else
	{
	  while (is_part_of_name (c))
	    {
	      p = p + 1;
	      c = *p;
	    }
	  *p = 0;
	  status = reg_name_search (name);
	  if (status >= 0)
	    num = status;
	  else
	    {
	      if (print_errors)
		as_bad ("Undefined register: '%s'. ASSUMING 0", name);
	      else
		num = -1;
	    }
	  *p = c;
	}

      result->number_part = num;

      if (is_R_select (p - 1))
	result->L_R_select = 1;
      else if (is_L_select (p - 1))
	result->L_R_select = 0;
      else
	result->L_R_select = 0;

    }
  else
    {
      num = 0;
      name = p;
      c = *p;
      while (is_part_of_name (c))
	{
	  p = p + 1;
	  c = *p;
	}
      *p = 0;
      if ((sym = symbol_find (name)) != NULL)
	{
#ifdef OBJ_SOM
	  if (sym->pa_sy_type == ST_ABSOLUTE)
	    {
	      num = sym->pa_sy_value;
#else
	  if (S_GET_SEGMENT (sym) == &bfd_abs_section)
	    {
	      num = S_GET_VALUE (sym);
#endif
	    }
	  else
	    {
	      if (print_errors)
		as_bad ("Non-absolute constant: '%s'. ASSUMING 0", name);
	      else
		num = -1;
	    }
	}
      else
	{
	  if (print_errors)
	    as_bad ("Undefined absolute constant: '%s'. ASSUMING 0", name);
	  else
	    num = -1;
	}
      *p = c;
      result->number_part = num;

      if (is_R_select (p - 1))
	result->L_R_select = 1;
      else if (is_L_select (p - 1))
	result->L_R_select = 0;
      else
	result->L_R_select = 0;

    }

  *s = p;
  return num;

}

int 
pa_parse_fp_cmp_cond (s)
     char **s;
{
  int cond, i;
  struct possibleS
    {
      char *string;
      int cond;
    };

  /*
     This table is sorted by order of the length of the string. This is so we
     check for <> before we check for <. If we had a <> and checked for < first,
     we would get a false match.
   */
  static struct possibleS poss[] =
  {
    {"false?", 0},
    {"false", 1},
    {"true?", 30},
    {"true", 31},
    {"!<=>", 3},
    {"!?>=", 8},
    {"!?<=", 16},
    {"!<>", 7},
    {"!>=", 11},
    {"!?>", 12},
    {"?<=", 14},
    {"!<=", 19},
    {"!?<", 20},
    {"?>=", 22},
    {"!?=", 24},
    {"!=t", 27},
    {"<=>", 29},
    {"=t", 5},
    {"?=", 6},
    {"?<", 10},
    {"<=", 13},
    {"!>", 15},
    {"?>", 18},
    {">=", 21},
    {"!<", 23},
    {"<>", 25},
    {"!=", 26},
    {"!?", 28},
    {"?", 2},
    {"=", 4},
    {"<", 9},
    {">", 17}
  };

  cond = 0;

  for (i = 0; i < 32; i++)
    {
      if (strncasecmp (*s, poss[i].string, strlen (poss[i].string)) == 0)
	{
	  cond = poss[i].cond;
	  *s += strlen (poss[i].string);
	  while (**s == ' ' || **s == '\t')
	    *s = *s + 1;
	  return cond;
	}
    }

  as_bad ("Illegal FP Compare Condition: %c", **s);
  return 0;
}

FP_Operand_Format 
pa_parse_fp_format (s)
     char **s;
{
  int f;

  f = SGL;
  if (**s == ',')
    {
      *s += 1;
      if (strncasecmp (*s, "sgl", 3) == 0)
	{
	  f = SGL;
	  *s += 4;
	}
      else if (strncasecmp (*s, "dbl", 3) == 0)
	{
	  f = DBL;
	  *s += 4;
	}
      else if (strncasecmp (*s, "quad", 4) == 0)
	{
	  f = QUAD;
	  *s += 5;
	}
      else
	{
	  f = ILLEGAL_FMT;
	  as_bad ("Unrecognized FP Operand Format: %3s", *s);
	}
    }
  while (**s == ' ' || **s == '\t' || **s == 0)
    *s = *s + 1;

  return f;
}

#if defined(OBJ_ELF)
int 
pa_chk_field_selector (str)
     char **str;
{
  int selector;
  struct selector_entry
    {
      char *prefix;
      int field_selector;
    };
  static struct selector_entry selector_table[] =
  {
    {"F'", e_fsel},
    {"F%", e_fsel},
    {"LS'", e_lssel},
    {"LS%", e_lssel},
    {"RS'", e_rssel},
    {"RS%", e_rssel},
    {"L'", e_lsel},
    {"L%", e_lsel},
    {"R'", e_rsel},
    {"R%", e_rsel},
    {"LD'", e_ldsel},
    {"LD%", e_ldsel},
    {"RD'", e_rdsel},
    {"RD%", e_rdsel},
    {"LR'", e_lrsel},
    {"LR%", e_lrsel},
    {"RR'", e_rrsel},
    {"RR%", e_rrsel},
    {"P'", e_psel},
    {"P%", e_psel},
    {"RP'", e_rpsel},
    {"RP%", e_rpsel},
    {"LP'", e_lpsel},
    {"LP%", e_lpsel},
    {"T'", e_tsel},
    {"T%", e_tsel},
    {"RT'", e_rtsel},
    {"RT%", e_rtsel},
    {"LT'", e_ltsel},
    {"LT%", e_ltsel},
    {NULL, e_fsel}
  };
  struct selector_entry *tableP;

  selector = e_fsel;

  while (**str == ' ' || **str == '\t' || **str == '\n' || **str == '\f')
    {
      *str = *str + 1;
    }
  for (tableP = selector_table; tableP->prefix; tableP++)
    {
      if (strncasecmp (tableP->prefix, *str, strlen (tableP->prefix)) == 0)
	{
	  *str += strlen (tableP->prefix);
	  selector = tableP->field_selector;
	  break;
	}
    }
  return selector;
}

int
getExpression (str)
     char *str;
{
  char *save_in;
  asection *seg;

  save_in = input_line_pointer;
  input_line_pointer = str;
  seg = expression (&the_insn.exp);

  if (!(seg == &bfd_abs_section
	|| seg == &bfd_und_section
	|| seg == text_section
	|| seg == data_section
	|| seg == bss_section
	|| seg == diff_section
	|| seg == big_section
	|| seg == absent_section))
    {
      the_insn.error = "bad segment";
      expr_end = input_line_pointer;
      input_line_pointer = save_in;
      return 1;
    }
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
  return 0;
}

int
getAbsoluteExpression (str)
     char *str;
{
  char *save_in;
  asection *seg;

  save_in = input_line_pointer;
  input_line_pointer = str;
  seg = expression (&the_insn.exp);

  if (seg != &bfd_abs_section)
    {
      the_insn.error = "segment should be ABSOLUTE";
      expr_end = input_line_pointer;
      input_line_pointer = save_in;
      return 1;
    }
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
  return 0;
}

#else
int
getExpression (str)
     char *str;
{
  char *save_in;
  segT seg;

  save_in = input_line_pointer;
  input_line_pointer = str;
  switch (seg = expression (&the_insn.exp))
    {

    case SEG_ABSOLUTE:
    case SEG_TEXT:
    case SEG_DATA:
    case SEG_BSS:
    case SEG_UNKNOWN:
    case SEG_DIFFERENCE:
    case SEG_BIG:
    case SEG_GDB:
    case SEG_MILLICODE:
    case SEG_NONE:
      break;

    default:
      the_insn.error = "illegal segment";
      expr_end = input_line_pointer;
      input_line_pointer = save_in;
      return 1;
    }
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
  return 0;
}

int
getAbsoluteExpression (str)
     char *str;
{
  char *save_in;
  segT seg;

  save_in = input_line_pointer;
  input_line_pointer = str;
  switch (seg = expression (&the_insn.exp))
    {

    case SEG_ABSOLUTE:
      break;

    default:
      the_insn.error = "segment should be ABSOLUTE";
      expr_end = input_line_pointer;
      input_line_pointer = save_in;
      return 1;
    }
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
  return 0;
}

#endif

int 
evaluateAbsolute (exp, field_selector)
     expressionS exp;
     int field_selector;
{
  int value;

  value = exp.X_add_number;

  if (exp.X_add_symbol)
    {
      value += S_GET_VALUE (exp.X_add_symbol);
    }
  if (exp.X_subtract_symbol)
    {
      value -= S_GET_VALUE (exp.X_subtract_symbol);
    }

  switch (field_selector)
    {
    case e_fsel:		/* F  : no change                      */
      break;

    case e_lssel:		/* LS : if (bit 21) then add 0x800
                              arithmetic shift right 11 bits */
      if (value & 0x00000400)
	value += 0x800;
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rssel:		/* RS : Sign extend from bit 21        */
      if (value & 0x00000400)
	value |= 0xfffff800;
      else
	value &= 0x7ff;
      break;

    case e_lsel:		/* L  : Arithmetic shift right 11 bits */
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rsel:		/* R  : Set bits 0-20 to zero          */
      value = value & 0x7ff;
      break;

    case e_ldsel:		/* LD : Add 0x800, arithmetic shift
                              right 11 bits                  */
      value += 0x800;
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rdsel:		/* RD : Set bits 0-20 to one           */
      value |= 0xfffff800;
      break;

    case e_lrsel:		/* LR : L with "rounded" constant      */
      /* XXX: this isn't right.  Need to add a "rounded" constant */
      /* XXX: (presumably from X_add_number)                      */
      value = (value & 0xfffff800) >> 11;
      break;

    case e_rrsel:		/* RR : R with "rounded" constant      */
      /* XXX: this isn't right.  Need to add a "rounded" constant */
      /* XXX: (presumably from X_add_number)                      */
      value = value & 0x7ff;
      break;

    default:
      BAD_CASE (field_selector);
      break;
    }
  return value;
}

int 
pa_build_arg_reloc (type_name)
     char *type_name;
{

  if (strncasecmp (type_name, "no", 2) == 0)
    {
      return 0;
    }
  if (strncasecmp (type_name, "gr", 2) == 0)
    {
      return 1;
    }
  else if (strncasecmp (type_name, "fr", 2) == 0)
    {
      return 2;
    }
  else if (strncasecmp (type_name, "fu", 2) == 0)
    {
      return 3;
    }
  else
    as_bad ("Unrecognized argument location: %s\n", type_name);

  return 0;
}

unsigned int 
pa_align_arg_reloc (reg, arg_reloc)
     unsigned int reg;
     unsigned int arg_reloc;
{
  unsigned int new_reloc;

  new_reloc = arg_reloc;
  switch (reg)
    {
    case 0:
      new_reloc <<= 8;
      break;
    case 1:
      new_reloc <<= 6;
      break;
    case 2:
      new_reloc <<= 4;
      break;
    case 3:
      new_reloc <<= 2;
      break;
    default:
      as_bad ("Illegal argument description: %d", reg);
    }

  return new_reloc;
}

int 
pa_parse_nullif (s)
     char **s;
{
  int nullif;

  nullif = 0;
  if (**s == ',')
    {
      *s = *s + 1;
      if (strncasecmp (*s, "n", 1) == 0)
	nullif = 1;
      else
	{
	  as_bad ("Unrecognized Nullification: (%c)", **s);
	  nullif = 0;
	}
      *s = *s + 1;
    }
  while (**s == ' ' || **s == '\t')
    *s = *s + 1;

  return nullif;
}

#if 0
int 
pa_parse_nonneg_cmpsub_cmpltr (s)
     char **s;
{
  int cmpltr;
  char *name;
  char c;

  cmpltr = -1;
  /** cmpltr = 0; **/
  if (**s == ',')
    {
      *s += 1;
      name = *s;
      while (**s != ',' && **s != ' ' && **s != '\t')
	*s += 1;
      c = **s;
      **s = 0x00;
      if (strcmp (name, "=") == 0)
	{
	  cmpltr = 1;
	}
      else if (strcmp (name, "<") == 0)
	{
	  cmpltr = 2;
	}
      else if (strcmp (name, "<=") == 0)
	{
	  cmpltr = 3;
	}
      else if (strcmp (name, "<<") == 0)
	{
	  cmpltr = 4;
	}
      else if (strcmp (name, "<<=") == 0)
	{
	  cmpltr = 5;
	}
      else if (strcasecmp (name, "sv") == 0)
	{
	  cmpltr = 6;
	}
      else if (strcasecmp (name, "od") == 0)
	{
	  cmpltr = 7;
	}
      /**
    else
      cmpltr = -1;
    **/
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  return cmpltr;
}

#endif
int 
pa_parse_nonneg_cmpsub_cmpltr (s)
     char **s;
{
  int cmpltr;
  char *name;
  char c;

  cmpltr = 0;
  if (**s == ',')
    {
      *s += 1;
      name = *s;
      while (**s != ',' && **s != ' ' && **s != '\t')
	*s += 1;
      c = **s;
      **s = 0x00;
      if (strcmp (name, "=") == 0)
	{
	  cmpltr = 1;
	}
      else if (strcmp (name, "<") == 0)
	{
	  cmpltr = 2;
	}
      else if (strcmp (name, "<=") == 0)
	{
	  cmpltr = 3;
	}
      else if (strcmp (name, "<<") == 0)
	{
	  cmpltr = 4;
	}
      else if (strcmp (name, "<<=") == 0)
	{
	  cmpltr = 5;
	}
      else if (strcasecmp (name, "sv") == 0)
	{
	  cmpltr = 6;
	}
      else if (strcasecmp (name, "od") == 0)
	{
	  cmpltr = 7;
	}
      else
	cmpltr = -1;
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  return cmpltr;
}

int 
pa_parse_neg_cmpsub_cmpltr (s)
     char **s;
{
  int cmpltr;
  char *name;
  char c;

  cmpltr = -1;
  if (**s == ',')
    {
      *s += 1;
      name = *s;
      while (**s != ',' && **s != ' ' && **s != '\t')
	*s += 1;
      c = **s;
      **s = 0x00;
      if (strcasecmp (name, "tr") == 0)
	{
	  cmpltr = 0;
	}
      else if (strcmp (name, "<>") == 0)
	{
	  cmpltr = 1;
	}
      else if (strcmp (name, ">=") == 0)
	{
	  cmpltr = 2;
	}
      else if (strcmp (name, ">") == 0)
	{
	  cmpltr = 3;
	}
      else if (strcmp (name, ">>=") == 0)
	{
	  cmpltr = 4;
	}
      else if (strcmp (name, ">>") == 0)
	{
	  cmpltr = 5;
	}
      else if (strcasecmp (name, "nsv") == 0)
	{
	  cmpltr = 6;
	}
      else if (strcasecmp (name, "ev") == 0)
	{
	  cmpltr = 7;
	}
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  return cmpltr;
}

int 
pa_parse_nonneg_add_cmpltr (s)
     char **s;
{
  int cmpltr;
  char *name;
  char c;

  cmpltr = -1;
  if (**s == ',')
    {
      *s += 1;
      name = *s;
      while (**s != ',' && **s != ' ' && **s != '\t')
	*s += 1;
      c = **s;
      **s = 0x00;
      if (strcmp (name, "=") == 0)
	{
	  cmpltr = 1;
	}
      else if (strcmp (name, "<") == 0)
	{
	  cmpltr = 2;
	}
      else if (strcmp (name, "<=") == 0)
	{
	  cmpltr = 3;
	}
      else if (strcasecmp (name, "nuv") == 0)
	{
	  cmpltr = 4;
	}
      else if (strcasecmp (name, "znv") == 0)
	{
	  cmpltr = 5;
	}
      else if (strcasecmp (name, "sv") == 0)
	{
	  cmpltr = 6;
	}
      else if (strcasecmp (name, "od") == 0)
	{
	  cmpltr = 7;
	}
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  return cmpltr;
}

int 
pa_parse_neg_add_cmpltr (s)
     char **s;
{
  int cmpltr;
  char *name;
  char c;

  cmpltr = -1;
  if (**s == ',')
    {
      *s += 1;
      name = *s;
      while (**s != ',' && **s != ' ' && **s != '\t')
	*s += 1;
      c = **s;
      **s = 0x00;
      if (strcasecmp (name, "tr") == 0)
	{
	  cmpltr = 0;
	}
      else if (strcmp (name, "<>") == 0)
	{
	  cmpltr = 1;
	}
      else if (strcmp (name, ">=") == 0)
	{
	  cmpltr = 2;
	}
      else if (strcmp (name, ">") == 0)
	{
	  cmpltr = 3;
	}
      else if (strcmp (name, "uv") == 0)
	{
	  cmpltr = 4;
	}
      else if (strcmp (name, "vnz") == 0)
	{
	  cmpltr = 5;
	}
      else if (strcasecmp (name, "nsv") == 0)
	{
	  cmpltr = 6;
	}
      else if (strcasecmp (name, "ev") == 0)
	{
	  cmpltr = 7;
	}
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  return cmpltr;
}

void 
s_seg ()
{

  if (strncmp (input_line_pointer, "\"text\"", 6) == 0)
    {
      input_line_pointer += 6;
      s_text ();
      return;
    }
  if (strncmp (input_line_pointer, "\"data\"", 6) == 0)
    {
      input_line_pointer += 6;
      s_data ();
      return;
    }
  if (strncmp (input_line_pointer, "\"data1\"", 7) == 0)
    {
      input_line_pointer += 7;
      s_data1 ();
      return;
    }
  as_bad ("Unknown segment type");
  demand_empty_rest_of_line ();
  return;
}

void 
s_private ()
{
  register int temp;

  temp = get_absolute_expression ();
#ifdef OBJ_SOM
  subseg_new (SEG_DATA, (subsegT) temp);
#else
  subseg_new (".data", (subsegT) temp);
#endif
  demand_empty_rest_of_line ();
}

void 
s_data1 ()
{
#ifdef OBJ_SOM
  subseg_new (SEG_DATA, 1);
#else
  subseg_new (".data", 1);
#endif
  demand_empty_rest_of_line ();
  return;
}

void 
s_proc ()
{
  extern char is_end_of_line[];

  while (!is_end_of_line[*input_line_pointer])
    {
      ++input_line_pointer;
    }
  ++input_line_pointer;
  return;
}

void 
pa_block (z)
     int z;
{
  register char *p;
  register long int temp_fill;
  register long int temp_size;
  register int i;

  temp_size = get_absolute_expression ();

  if (z)
    {				/* fill with zeroes even if not requested to do so. */
      temp_fill = 0;		/* HP assembler does this too. */
    }
  else
    {
      temp_fill = 0;
    }

  if (temp_size <= 0)
    {
      as_bad ("size < 0, .block ignored");
      temp_size = 0;
    }
  p = frag_var (rs_fill,
		(int) temp_size,
	(int) temp_size, (relax_substateT) 0, (symbolS *) 0, 1, (char *) 0);
  bzero (p, (int) temp_size);

  /* convert 2 bytes at a time */

  for (i = 0; i < temp_size; i += 2)
    {
      md_number_to_chars (p + i,
			  temp_fill,
			  (int) ((temp_size - i) > 2 ? 2 : (temp_size - i)));
    }

  pa_undefine_label ();
  demand_empty_rest_of_line ();
  return;
}

void 
pa_call ()
{

  pa_call_args (&last_call_desc);
  demand_empty_rest_of_line ();
  return;
}

void 
pa_call_args (call_desc)
     register call_descS *call_desc;
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register unsigned int arg_reloc;

  while (!is_end_of_statement ())
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      if ((strncasecmp (name, "argw", 4) == 0))
	{
	  temp = atoi (name + 4);
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  name = input_line_pointer;
	  c = get_symbol_end ();
	  arg_reloc = pa_build_arg_reloc (name);
	  call_desc->arg_reloc |= pa_align_arg_reloc (temp, arg_reloc);
	}
      else if ((strncasecmp (name, "rtnval", 6) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  name = input_line_pointer;
	  c = get_symbol_end ();
	  arg_reloc = pa_build_arg_reloc (name);
	  call_desc->arg_reloc |= (arg_reloc & 0x3);
	}
      else
	{
	  as_bad ("Unrecognized .CALL argument: %s", name);
	}
      p = input_line_pointer;
      *p = c;
      if (!is_end_of_statement ())
	input_line_pointer++;
    }
}

static int
is_same_frag (frag1P, frag2P)
     fragS *frag1P;
     fragS *frag2P;
{

  if (frag1P == NULL)
    return (FALSE);
  else if (frag2P == NULL)
    return (FALSE);
  else if (frag1P == frag2P)
    return (TRUE);
  else if (frag2P->fr_type == rs_fill && frag2P->fr_fix == 0)
    is_same_frag (frag1P, frag2P->fr_next);
  else
    return (FALSE);
}

#ifdef OBJ_ELF
static void
pa_build_unwind_subspace (call_info)
     call_infoS *call_info;
{
  char *unwindP;
  asection *seg;
  asection *save_seg;
  subsegT subseg, save_subseg;
  int i;
  char c;
  char *p;

  subseg = SUBSEG_UNWIND;
  seg = bfd_get_section_by_name (stdoutput, UNWIND_SECTION_NAME);
  if (seg == ASEC_NULL)
    {
      seg = bfd_make_section_old_way (stdoutput, UNWIND_SECTION_NAME);
    }
  bfd_set_section_flags (stdoutput,
			 seg,
			 SEC_READONLY | SEC_ALLOC | SEC_LOAD | SEC_RELOC);

  /* callinfo.frame is in bytes and unwind_desc is in 8 byte units */
  call_info->ci_unwind.descriptor.frame_size = call_info->frame / 8;

  /* Now, dump the unwind descriptor to the $UNWIND$ subspace. This
     creates a couple of relocations */

  save_seg = now_seg;
  save_subseg = now_subseg;
  subseg_new ((char *) seg->name, subseg);
  unwindP = (char *) &call_info->ci_unwind;

  p = frag_more (4);
  call_info->start_offset_frag = frag_now;
  call_info->start_frag_where = p - frag_now->fr_literal;

  /* relocation info. for start offset of the function */

  fix_new_hppa (frag_now, p - frag_now->fr_literal, 4,
		call_info->start_symbol, (symbolS *) NULL,
		0, 0, R_HPPA, e_fsel, 32, 0, (char *) 0);

  /** we need to search for the first relocation involving the start_symbol of **/
  /** this call_info descriptor **/

  {
    fixS *fixP;

    call_info->start_fix = seg_info (now_seg)->fix_root;	/* the default */
    for (fixP = call_info->start_fix; fixP; fixP = fixP->fx_next)
      {
	if (fixP->fx_addsy == call_info->start_symbol
	    || fixP->fx_subsy == call_info->start_symbol)
	  {
	    call_info->start_fix = fixP;
	    break;
	  }
      }
  }

  p = frag_more (4);
  call_info->end_offset_frag = frag_now;
  call_info->end_frag_where = p - frag_now->fr_literal;

  /* relocation info. for end offset of the function */

  fix_new_hppa (frag_now, p - frag_now->fr_literal, 4,
		call_info->end_symbol, (symbolS *) NULL,
		0, 0, R_HPPA, e_fsel, 32, 0, (char *) 0);

  /** we need to search for the first relocation involving the start_symbol of **/
  /** this call_info descriptor **/

  {
    fixS *fixP;

    call_info->end_fix = seg_info (now_seg)->fix_root;	/* the default */
    for (fixP = call_info->end_fix; fixP; fixP = fixP->fx_next)
      {
	if (fixP->fx_addsy == call_info->end_symbol
	    || fixP->fx_subsy == call_info->end_symbol)
	  {
	    call_info->end_fix = fixP;
	    break;
	  }
      }
  }

  for (i = 8; i < sizeof (unwind_tableS); i++)
    {
      c = *(unwindP + i);
      {
	FRAG_APPEND_1_CHAR (c);
      }
    }

  subseg_new ((char *) save_seg->name, save_subseg);
}

#else
#ifdef OBJ_SOM
static void
pa_build_unwind_subspace (call_info)
     call_infoS *call_info;
{
  space_dict_chainS *spaceP;
  subspace_dict_chainS *subspaceP;
  char *unwindP;
  char defined, loadable, code_only, common, dup_common, is_zero, sort;
  int access, space_index, alignment, quadrant;
  segT seg, save_seg;
  subsegT subseg, save_subseg;
  int i;
  char c;
  char *p;

  defined = 1;
  loadable = 1;
  code_only = 0;
  common = 0;
  dup_common = 0;
  is_zero = 0;
  sort = 0x40;
  access = 0x2c;
  space_index = 0;
  alignment = 8;
  quadrant = 0;
  subseg = SUBSEG_UNWIND;
  seg = SEG_TEXT;

  spaceP = pa_segment_to_space (seg);

  if ((subspaceP = is_defined_subspace ("$UNWIND$", SUBSEG_UNWIND)))
    {
      update_subspace ("$UNWIND$", defined, loadable, code_only, common, dup_common,
		    sort, is_zero, access, space_index, alignment, quadrant,
		       SUBSEG_UNWIND);
    }
  else
    {
      subspaceP = create_new_subspace (spaceP, "$UNWIND$", defined, loadable, code_only,
				  common, dup_common, is_zero, sort, access,
				     space_index, alignment, quadrant, seg);
    }


  /* callinfo.frame is in bytes and unwind_desc is in 8 byte units */
  call_info->ci_unwind.descriptor.frame_size = call_info->frame / 8;

  /* Now, dump the unwind descriptor to the $UNWIND$ subspace. This
     creates a couple of relocations */

  save_seg = now_seg;
  save_subseg = now_subseg;
  subseg_new (seg, subseg);
  unwindP = (char *) &call_info->ci_unwind;

  p = frag_more (4);
  call_info->start_offset_frag = frag_now;
  call_info->start_frag_where = p - frag_now->fr_literal;

  /* relocation info. for start offset of the function */

  fix_new (frag_now, p - frag_now->fr_literal, 4,
	   call_info->start_symbol, (symbolS *) NULL,
	   0, 0, R_DATA_ONE_SYMBOL, e_fsel, 0, 0, (char *) 0);

  /** we need to search for the first relocation involving the start_symbol of **/
  /** this call_info descriptor **/

  {
    fixS *fixP;

    call_info->start_fix = seg_info (now_seg)->fix_root;	/* the default */
    for (fixP = call_info->start_fix; fixP; fixP = fixP->fx_next)
      {
	/*
      if ( ( fixP->fx_addsy == call_info->start_symbol ||
	     fixP->fx_subsy == call_info->start_symbol )
	  &&
	   ( fixP->fx_frag == call_info->start_symbol->sy_frag ) ) {
       */
	if ((fixP->fx_addsy == call_info->start_symbol ||
	     fixP->fx_subsy == call_info->start_symbol)
	    &&
	    (is_same_frag (fixP->fx_frag, call_info->start_symbol->sy_frag)))
	  {
	    call_info->start_fix = fixP;
	    break;
	  }
      }
  }

  p = frag_more (4);
  call_info->end_offset_frag = frag_now;
  call_info->end_frag_where = p - frag_now->fr_literal;

  /* relocation info. for end offset of the function */

  fix_new (frag_now, p - frag_now->fr_literal, 4,
	   call_info->start_symbol, (symbolS *) NULL,
	   0, 0, R_DATA_ONE_SYMBOL, e_fsel, 0, 0, (char *) 0);

  /** we need to search for the first relocation involving the start_symbol of **/
  /** this call_info descriptor **/

  {
    fixS *fixP;

    call_info->end_fix = seg_info (now_seg)->fix_root;	/* the default */
    for (fixP = call_info->end_fix; fixP; fixP = fixP->fx_next)
      {
	/*
      if ( ( fixP->fx_addsy == call_info->start_symbol ||
	     fixP->fx_subsy == call_info->start_symbol )
	  &&
	   ( fixP->fx_frag == call_info->start_symbol->sy_frag ) ) {
       */
	if ((fixP->fx_addsy == call_info->start_symbol ||
	     fixP->fx_subsy == call_info->start_symbol)
	    &&
	    (is_same_frag (fixP->fx_frag, call_info->start_symbol->sy_frag)))
	  {
	    call_info->end_fix = fixP;
	    break;
	  }
      }
  }

  for (i = 8; i < sizeof (unwind_tableS); i++)
    {
      c = *(unwindP + i);
      {
	FRAG_APPEND_1_CHAR (c);
      }
    }

  subseg_new (save_seg, save_subseg);

}

#endif
#endif

void 
pa_callinfo ()
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register symbolS *symbolP;

  if (!within_procedure)
    as_bad (".callinfo is not within a procedure definition");

  callinfo_found = TRUE;

  while (!is_end_of_statement ())
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      if ((strncasecmp (name, "frame", 5) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = get_absolute_expression ();
	  if ((temp & 0x3) != 0)
	    {
	      as_bad ("FRAME parameter must be a multiple of 8: %d\n", temp);
	      temp = 0;
	    }
	  last_call_info->frame = temp;
	}
      else if ((strncasecmp (name, "entry_gr", 8) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = get_absolute_expression ();
	  last_call_info->ci_unwind.descriptor.entry_gr = temp;
	}
      else if ((strncasecmp (name, "entry_fr", 8) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = get_absolute_expression ();
	  last_call_info->ci_unwind.descriptor.entry_fr = temp;
	}
      else if ((strncasecmp (name, "entry_sr", 8) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = get_absolute_expression ();
	  last_call_info->entry_sr = temp;
	}
      else if ((strncasecmp (name, "calls", 5) == 0) ||
	       (strncasecmp (name, "caller", 6) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->makes_calls = 1;
	}
      else if ((strncasecmp (name, "no_calls", 8) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->makes_calls = 0;
	}
      else if ((strncasecmp (name, "save_rp", 7) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->ci_unwind.descriptor.save_rp = 1;
	}
      else if ((strncasecmp (name, "save_sp", 7) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->ci_unwind.descriptor.save_sp = 1;
	}
      else if ((strncasecmp (name, "no_unwind", 9) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->ci_unwind.descriptor.cannot_unwind = 1;
	}
      else if ((strncasecmp (name, "hpux_int", 7) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->hpux_int = 1;
	}
      else
	{
	  as_bad ("Unrecognized .CALLINFO argument: %s", name);
	}
      if (!is_end_of_statement ())
	input_line_pointer++;
    }

  demand_empty_rest_of_line ();
  return;
}

void 
pa_code ()
{
  space_dict_chainS *sdchain;

  if ((sdchain = is_defined_space ("$TEXT$")) == NULL)
    {
      sdchain = create_new_space (pa_def_spaces[0].name, pa_def_spaces[0].spnum,
			pa_def_spaces[0].loadable, pa_def_spaces[0].defined,
			    pa_def_spaces[0].private, pa_def_spaces[0].sort,
				  1, pa_def_spaces[0].segment);
    }

  SPACE_DEFINED (sdchain) = 1;
#ifdef OBJ_SOM

  subseg_new (SEG_TEXT, SUBSEG_CODE);
#else

  subseg_new (".text", SUBSEG_CODE);
#endif

  demand_empty_rest_of_line ();
  return;
}

/*
 * This is different than the standard GAS s_comm(). On HP9000/800 machines,
 * the .comm pseudo-op has the following symtax:
 *
 *     <label> .comm <length>
 *
 * where <label> is optional and is a symbol whose address will be the start of
 * a block of memory <length> bytes long. <length> must be an absolute expressio
n.
 * <length> bytes will be allocated in the current space and subspace.
 *
 */

void
pa_comm ()
{
  register char *p;
  register int size, i;
  register symbolS *symbolP;
  register label_symbolS *label_symbolP = pa_get_label ();

  if (label_symbolP)
    symbolP = label_symbolP->lss_label;
  else
    symbolP = NULL;

  SKIP_WHITESPACE ();
  if ((size = get_absolute_expression ()) < 0)
    {
      as_warn (".COMMon length (%d.) <0! Ignored.", size);
      ignore_rest_of_line ();
      return;
    }

  if (symbolP)
    {
#ifdef OBJ_SOM
      if (symbolP->pa_sy_type == ST_STORAGE &&
	  symbolP->pa_sy_scope == SS_UNSAT)
	{
	  if (symbolP->pa_sy_value != size)
	    {
	      as_warn ("Length of .comm \"%s\" is already %d. Not changed to %d.",
		       symbolP->pa_sy_name, symbolP->pa_sy_value, size);
	      return;
	    }
	}
      else
	{
	  symbolP->pa_sy_value = size;
	  symbolP->pa_sy_scope = SS_UNSAT;
	  symbolP->pa_sy_type = ST_STORAGE;
	  symbolP->sy_ref = sym_def;
	}
#endif
#ifdef OBJ_ELF
      if (S_IS_DEFINED (symbolP) && S_GET_SEGMENT (symbolP) == bss_section)
	{
	  as_bad ("Ignoring attempt to re-define symbol");
	  ignore_rest_of_line ();
	  return;
	}
      if (S_GET_VALUE (symbolP))
	{
	  if (S_GET_VALUE (symbolP) != size)
	    {
	      as_warn ("Length of .comm \"%s\" is already %d. Not changed to %d.",
		       S_GET_NAME (symbolP), S_GET_VALUE (symbolP), size);
	      return;
	    }
	}
      else
	{
	  S_SET_VALUE (symbolP, size);
	  S_SET_SEGMENT (symbolP, bss_section);
	  S_SET_EXTERNAL (symbolP);
	}
#endif
    }


  demand_empty_rest_of_line ();
}

void 
pa_copyright ()
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register symbolS *symbolP;

  SKIP_WHITESPACE ();
  if (*input_line_pointer == '\"')
    {
      ++input_line_pointer;	/* -> 1st char of string. */
      name = input_line_pointer;
      while ((c = next_char_of_string ()) >= 0)
	;
      c = *input_line_pointer;
      *input_line_pointer = '\0';
      *(input_line_pointer - 1) = '\0';
      {
#ifdef OBJ_SOM
#define PREFIX	"Copyright "
#define MIDFIX  ". All rights reserved. No part of this program may be photocopied, reproduced, or transmitted without prior written consent of "
#define SUFFIX "."

	struct aux_hdr_list *aux_hdr_entry;
	int len;
	char *company_name = name;
	char *date_part;

	date_part = (char *) index (name, ',');
	if (date_part)
	  {
	    *date_part = 0x00;
	    date_part++;
	  }

	len =
	  strlen (PREFIX) +
	  strlen (MIDFIX) +
	  strlen (SUFFIX) +
	  2 * strlen (name);

	if (date_part)
	  {
	    len += strlen (date_part) + strlen (",");
	  }

	aux_hdr_entry = (struct aux_hdr_list *) malloc (sizeof (struct aux_hdr_list));
	if (aux_hdr_root)
	  {
	    aux_hdr_entry->ahl_next = aux_hdr_root;
	    aux_hdr_root = aux_hdr_entry;
	  }
	else
	  {
	    aux_hdr_entry->ahl_next = NULL;
	    aux_hdr_root = aux_hdr_entry;
	  }
	aux_hdr_entry->type = COPYRIGHT_AUX_ID;
	aux_hdr_entry->contents.cpy.header_id.append = 1;
	aux_hdr_entry->contents.cpy.header_id.type = COPYRIGHT_AUX_ID;
	aux_hdr_entry->contents.cpy.header_id.length = len + sizeof (unsigned int);
	while (aux_hdr_entry->contents.usr_str.header_id.length % 4)
	  aux_hdr_entry->contents.usr_str.header_id.length += 1;

	aux_hdr_entry->contents.cpy.string_length = len;
	aux_hdr_entry->contents.cpy.copyright = (char *) (malloc (len + 1));
	strcpy (aux_hdr_entry->contents.cpy.copyright, PREFIX);
	strcat (aux_hdr_entry->contents.cpy.copyright, name);
	if (date_part)
	  {
	    strcat (aux_hdr_entry->contents.cpy.copyright, ",");
	    strcat (aux_hdr_entry->contents.cpy.copyright, date_part);
	  }
	strcat (aux_hdr_entry->contents.cpy.copyright, MIDFIX);
	strcat (aux_hdr_entry->contents.cpy.copyright, name);
	strcat (aux_hdr_entry->contents.cpy.copyright, SUFFIX);
	aux_hdr_entry->contents.cpy.copyright[len] = NULL;
#undef PREFIX
#undef MIDFIX
#undef SUFFIX
#endif /* OBJ_SOM */
      }
      *input_line_pointer = c;
    }
  else
    {
      as_bad ("Expected \"-ed string");
    }
  pa_undefine_label ();
  demand_empty_rest_of_line ();
}

void 
pa_end ()
{

  demand_empty_rest_of_line ();
  return;
}

void 
pa_enter ()
{

  as_bad (".ENTER encountered. gas doesn't generate entry code sequences.");
  pa_entry ();
  return;
}

void 
pa_entry ()
{
  char *where;

  if (!within_procedure)
    as_bad ("Misplaced .entry. Ignored.");
  else
    {
      if (!callinfo_found)
	as_bad ("Missing .callinfo.");

      last_call_info->start_frag = frag_now;
    }
  demand_empty_rest_of_line ();
  within_entry_exit = TRUE;
  where = frag_more (0);
#ifdef OBJ_SOM
  fix_new (frag_now, where - frag_now->fr_literal, 0,
	   last_call_info->start_symbol, (symbolS *) NULL, 0, 0,
     R_ENTRY, e_fsel, 0, 0, (char *) &last_call_info->ci_unwind.descriptor);
#else
#ifdef OBJ_ELF
  /* XXX: no ENTRY relocation for PA ELF.  What do we do instead? */
#else
  fix_new_hppa (frag_now, where - frag_now->fr_literal, 0,
		last_call_info->start_symbol, (symbolS *) NULL, 0, 0,
		R_HPPA_ENTRY, 0, 0, 0,
		(char *) &last_call_info->ci_unwind.descriptor);
#endif
#endif
  return;
}

void 
pa_equ (reg)
     int reg;
{
  register label_symbolS *label_symbolP = pa_get_label ();
  register symbolS *symbolP;

  if (label_symbolP)
    {
      symbolP = label_symbolP->lss_label;
#ifdef OBJ_SOM
      symbolP->pa_sy_value = get_absolute_expression ();
      symbolP->pa_sy_type = ST_ABSOLUTE;
      symbolP->sy_ref = sym_unref;
      symbolP->sy_equ = 1;
#else
      S_SET_VALUE (symbolP, get_absolute_expression ());
      S_SET_SEGMENT (symbolP, &bfd_abs_section);
#endif
    }
  else
    {
      if (reg)
	as_bad (".REG must use a label");
      else
	as_bad (".EQU must use a label");
    }

  pa_undefine_label ();
  demand_empty_rest_of_line ();
  return;
}

void 
process_exit ()
{
  char *where;

  where = frag_more (0);
#ifdef OBJ_SOM
  fix_new (frag_now, where - frag_now->fr_literal, 0,
	   last_call_info->start_symbol, (symbolS *) NULL, 0,
	   0, R_EXIT, e_fsel, 0, 0, (char *) NULL);
#endif
#ifdef OBJ_ELF
  /* XXX: no EXIT relocation for PA ELF.  All we do is create a */
  /* temporary symbol marking the end of the function. */
  {
    char *name = (char *) xmalloc (strlen ("L\001end_") +
		    strlen (S_GET_NAME (last_call_info->start_symbol)) + 1);

    if (name)
      {
	symbolS *symbolP;

	strcpy (name, "L\001end_");
	strcat (name, S_GET_NAME (last_call_info->start_symbol));

	symbolP = symbol_find (name);
	if (symbolP)
	  as_warn ("Symbol '%s' already defined.", name);
	else
	  {
	    /* symbol value should be the offset of the */
	    /* last instruction of the function */
	    symbolP = symbol_new (name,
				  now_seg,
	    (valueT) (obstack_next_free (&frags) - frag_now->fr_literal - 4),
				  frag_now);

	    assert (symbolP);

	    symbolP->bsym->flags = last_call_info->start_symbol->bsym->flags;
	    symbol_table_insert (symbolP);
	  }
	if (symbolP)
	  last_call_info->end_symbol = symbolP;
	else
	  as_bad ("Symbol '%s' could not be created.", name);

      }
    else
      as_bad ("No memory for symbol name.");
  }
#else
  fix_new_hppa (frag_now, where - frag_now->fr_literal, 0,
		last_call_info->start_symbol, (symbolS *) NULL, 0,
		0, R_HPPA_EXIT, 0, 0, 0, (char *) NULL);
#endif
  last_call_info->end_frag = frag_now;

  pa_build_unwind_subspace (last_call_info);

  exit_processing_complete = TRUE;
}

void 
pa_exit ()
{

  if (!within_procedure)
    as_bad (".EXIT must appear within a procedure");
  else
    {
      if (!callinfo_found)
	as_bad ("Missing .callinfo");
      else
	{
	  if (!within_entry_exit)
	    as_bad ("No .ENTRY for this .EXIT");
	  else
	    {
	      within_entry_exit = FALSE;
	      process_exit ();
	    }
	}
    }
  demand_empty_rest_of_line ();
  return;
}

void 
pa_export ()
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register int regno;
  register symbolS *symbolP;

  name = input_line_pointer;
  c = get_symbol_end ();
  /* just after name is now '\0' */

  if ((symbolP = symbol_find_or_make (name)) == NULL)
    {
      as_bad ("Cannot define export symbol: %s\n", name);
      p = input_line_pointer;
      *p = c;
      input_line_pointer++;
    }
  else
    {
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_scope = SS_UNIVERSAL;
      /* determination of the symbol_type field will have to wait until
	 we know the subspace index (within the object file) of the subspace
	 containing this symbol */
#else
      /* S_SET_SEGMENT(symbolP,&bfd_und_section); */
      S_SET_EXTERNAL (symbolP);
      /* symbolP->sy_frag = frag_now; */
#endif

      p = input_line_pointer;
      *p = c;
      if (!is_end_of_statement ())
	{
	  input_line_pointer++;
	  pa_export_args (symbolP);
	}
    }

  demand_empty_rest_of_line ();
  return;
}

void 
pa_export_args (symbolP)
     register symbolS *symbolP;
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register unsigned int arg_reloc;
#ifdef OBJ_ELF
  elf_symbol_type *esymbolP = (elf_symbol_type *) (symbolP->bsym);
#endif

  if (strncasecmp (input_line_pointer, "absolute", 8) == 0)
    {
      input_line_pointer += 8;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_ABSOLUTE;
#else
      S_SET_SEGMENT (symbolP, &bfd_abs_section);
#endif
    }
  else if (strncasecmp (input_line_pointer, "code", 4) == 0)
    {
      input_line_pointer += 4;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_CODE;
#else
      /* S_SET_SEGMENT(symbolP,text_section); */
#endif
    }
  else if (strncasecmp (input_line_pointer, "data", 4) == 0)
    {
      input_line_pointer += 4;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_DATA;
#else
      /* S_SET_SEGMENT(symbolP,data_section); */
#endif
    }
  else if ((strncasecmp (input_line_pointer, "entry", 5) == 0))
    {
      input_line_pointer += 5;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_ENTRY;
#else
      symbolP->bsym->flags |= BSF_FUNCTION;
#endif
    }
  else if (strncasecmp (input_line_pointer, "millicode", 9) == 0)
    {
      input_line_pointer += 9;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_MILLICODE;
#endif
    }
  else if (strncasecmp (input_line_pointer, "plabel", 6) == 0)
    {
      input_line_pointer += 6;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_PLABEL;
#endif
    }
  else if (strncasecmp (input_line_pointer, "pri_prog", 8) == 0)
    {
      input_line_pointer += 8;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_PRI_PROG;
#endif
    }
  else if (strncasecmp (input_line_pointer, "sec_prog", 8) == 0)
    {
      input_line_pointer += 8;
#ifdef OBJ_SOM
      symbolP->pa_sy_dict.symbol_type = ST_SEC_PROG;
#endif
    }

  while (!is_end_of_statement ())
    {
      if (*input_line_pointer == ',')
	input_line_pointer++;
      name = input_line_pointer;
      c = get_symbol_end ();
      if ((strncasecmp (name, "argw", 4) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = atoi (name + 4);
	  name = input_line_pointer;
	  c = get_symbol_end ();
	  arg_reloc = pa_align_arg_reloc (temp, pa_build_arg_reloc (name));
#ifdef OBJ_SOM
	  symbolP->pa_sy_dict.arg_reloc |= arg_reloc;
#else
	  esymbolP->tc_data.hppa_arg_reloc |= arg_reloc;
#endif
	  *input_line_pointer = c;
	}
      else if ((strncasecmp (name, "rtnval", 6)) == 0)
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  name = input_line_pointer;
	  c = get_symbol_end ();
	  arg_reloc = pa_build_arg_reloc (name);
#ifdef OBJ_SOM
	  symbolP->pa_sy_dict.arg_reloc |= arg_reloc;
#else
	  esymbolP->tc_data.hppa_arg_reloc |= arg_reloc;
#endif
	  *input_line_pointer = c;
	}
      else if ((strncasecmp (name, "priv_lev", 8)) == 0)
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  /*** temp = get_absolute_expression (); ***/
	  temp = atoi (input_line_pointer);
	  c = get_symbol_end ();
	  *input_line_pointer = c;
#ifdef OBJ_SOM
	  symbolP->sy_priv_lev = temp & 3;	/* this is stored in symbol_value later */
#endif
	}
      else
	{
	  as_bad ("Undefined .EXPORT/.IMPORT argument (ignored): %s", name);
	  p = input_line_pointer;
	  *p = c;
	}
      if (!is_end_of_statement ())
	input_line_pointer++;
    }
}

void 
pa_import ()
{
  register char *name;
  register char c;
  register char *p;
  register symbolS *symbolP;
  register expressionS resultP;	/* Deliver result here. */

  name = input_line_pointer;
  c = get_symbol_end ();
  /* just after name is now '\0' */

  symbolP = symbol_find_or_make (name);
#if defined(OBJ_ELF)
  /* symbolP->bsym->flags |= BSF_IMPORT; *//* XXX BSF_IMPORT is obsolete */
#else
  /* Check to see if this symbol has already been exported (this means its */
  /* defined locally and the import statement is redundant). */
  /* If it has not been exported, go ahead and mark this symbol as SS_UNSAT */
  /* (an unsatisfied external symbol) */

  /* But, if the symbol has already been referenced (sy_ref == TRUE),
       leave it alone. */

  if (!symbolP->sy_ref)
    {
      if (symbolP->pa_sy_dict.symbol_scope != SS_UNIVERSAL)
	{
	  symbolP->pa_sy_dict.symbol_scope = SS_UNSAT;
	  symbolP->sy_ref = FALSE;
	}
    }
#endif

  p = input_line_pointer;
  *p = c;

  if (!is_end_of_statement ())
    {
      input_line_pointer++;

      pa_export_args (symbolP);
#ifdef OBJ_ELF
      /* In ELF, since this is an import, leave the section undefined.	*/
      /* S_SET_SEGMENT(symbolP,&bfd_und_section); */
#endif
    }
  else
    {
#ifdef OBJ_SOM
      /* no further arguments, assign a default type according
	       to the current subspace (CODE or DATA) */
      switch (now_seg)
	{
	case SEG_TEXT:
	  symbolP->pa_sy_dict.symbol_type = ST_CODE;
	  break;
	case SEG_ABSOLUTE:
	  symbolP->pa_sy_dict.symbol_type = ST_ABSOLUTE;
	  break;
	default:
	  symbolP->pa_sy_dict.symbol_type = ST_DATA;
	}
#else
      /* In ELF, if the section is undefined, then the symbol is undefined */
      /* Since this is an import, leave the section undefined.	*/
      S_SET_SEGMENT (symbolP, &bfd_und_section);
#endif
    }


  demand_empty_rest_of_line ();
  return;
}

void 
pa_label ()
{
  register char *name;
  register char c;
  register char *p;

  name = input_line_pointer;
  c = get_symbol_end ();
  /* just after name is now '\0' */

  if (strlen (name) > 0)
    {
      colon (name);
      p = input_line_pointer;
      *p = c;
    }
  else
    {
      as_warn ("Missing label name on .LABEL");
    }

  if (!is_end_of_statement ())
    {
      as_warn ("extra .LABEL arguments ignored.");
      ignore_rest_of_line ();
    }
  demand_empty_rest_of_line ();
  return;
}

void 
pa_leave ()
{

  as_bad (".LEAVE encountered. gas doesn't generate exit code sequences.");
  pa_exit ();
  return;
}

void 
pa_origin ()
{
  s_org ();			/* ORG actually allows another argument (the fill value)
                    but maybe this is OK? */
  pa_undefine_label ();
  return;
}

void 
pa_proc ()
{
  call_infoS *call_info;

  if (within_procedure)
    as_fatal ("Nested procedures");

  callinfo_found = FALSE;
  within_procedure = TRUE;
  exit_processing_complete = FALSE;

  /* create another call_info structure */

  call_info = (call_infoS *) xmalloc (sizeof (call_infoS));

  if (!call_info)
    as_fatal ("Cannot allocate unwind descriptor\n");

  bzero (call_info, sizeof (call_infoS));

  call_info->ci_next = NULL;

  if (call_info_root == NULL)
    {
      call_info_root = call_info;
      last_call_info = call_info;
    }
  else
    {
      last_call_info->ci_next = call_info;
      last_call_info = call_info;
    }

  /* set up defaults on call_info structure */

  call_info->ci_unwind.descriptor.cannot_unwind = 0;
  call_info->ci_unwind.descriptor.region_desc = 1;
  call_info->entry_sr = ~0;
  call_info->makes_calls = 1;
  call_info->hpux_int = 0;

  /* If we got a .PROC pseudo-op, we know that the function is defined
     locally. Make sure it gets into the symbol table */
  {
    label_symbolS *label_symbolP = pa_get_label ();

    if (label_symbolP)
      {
	if (label_symbolP->lss_label)
	  {
#ifdef OBJ_SOM
	    label_symbolP->lss_label->sy_ref |= sym_def;
#endif
	    last_call_info->start_symbol = label_symbolP->lss_label;
	    label_symbolP->lss_label->bsym->flags |= BSF_FUNCTION;
	  }
	else
	  as_bad ("Missing function name for .PROC (corrupted label)");
      }
    else
      as_bad ("Missing function name for .PROC");
  }

  demand_empty_rest_of_line ();
  return;
}

void 
pa_procend ()
{

  if (!within_procedure)
    as_bad ("misplaced .procend");

  if (!callinfo_found)
    as_bad ("Missing .callinfo for this procedure");

  if (within_entry_exit)
    as_bad ("Missing .EXIT for a .ENTRY");

  if (!exit_processing_complete)
    process_exit ();

  within_procedure = FALSE;
  demand_empty_rest_of_line ();
  return;
}

space_dict_chainS *
pa_parse_space_stmt (space_name, create_flag)
     char *space_name;
     int create_flag;
{
  register char *name;
  register char c;
  register char *p;
  register int temp;

  char *ptemp;
  int spnum;
  char loadable;
  char defined;
  char private;
  char sort;
#ifdef OBJ_SOM
  segT seg;
#else
  asection *seg;
#endif
  space_dict_chainS *space;

  /* load default values */
  spnum = 0;
  loadable = TRUE;
  defined = TRUE;
  private = FALSE;
  if (strcasecmp (space_name, "$TEXT$") == 0)
    {
#ifdef OBJ_SOM
      seg = SEG_TEXT;
#else
      seg = text_section;
#endif
      sort = 8;
    }
  else
    {
#ifdef OBJ_SOM
      seg = SEG_DATA;
#else
      seg = data_section;
#endif
      sort = 16;
    }

  if (!is_end_of_statement ())
    {
      print_errors = FALSE;
      ptemp = input_line_pointer + 1;
      if ((temp = pa_parse_number (&ptemp)) >= 0)
	{
	  spnum = temp;
	  input_line_pointer = ptemp;
	}
      else
	{
	  while (!is_end_of_statement ())
	    {
	      input_line_pointer++;
	      name = input_line_pointer;
	      c = get_symbol_end ();
	      if ((strncasecmp (name, "SPNUM", 5) == 0))
		{
		  p = input_line_pointer;
		  *p = c;
		  input_line_pointer++;
		  temp = get_absolute_expression ();
		  spnum = temp;
		}
	      else if ((strncasecmp (name, "SORT", 4) == 0))
		{
		  p = input_line_pointer;
		  *p = c;
		  input_line_pointer++;
		  temp = get_absolute_expression ();
		  sort = temp;
		}
	      else if ((strncasecmp (name, "UNLOADABLE", 10) == 0))
		{
		  p = input_line_pointer;
		  *p = c;
		  loadable = FALSE;
		}
	      else if ((strncasecmp (name, "NOTDEFINED", 10) == 0))
		{
		  p = input_line_pointer;
		  *p = c;
		  defined = FALSE;
		}
	      else if ((strncasecmp (name, "PRIVATE", 7) == 0))
		{
		  p = input_line_pointer;
		  *p = c;
		  private = TRUE;
		}
	      else
		{
		  as_bad ("Unrecognized .SPACE argument");
		  p = input_line_pointer;
		  *p = c;
		}
	    }
	}
      print_errors = TRUE;
    }
  if (create_flag)
    space = create_new_space (space_name, spnum, loadable, defined, private, sort, 1, seg);
  else
    {				/* if no creation of new space, this must be the first */
      /* occurrence of a built-in space */
      space = is_defined_space (space_name);
      SPACE_SPNUM (space) = spnum;
      SPACE_LOADABLE (space) = loadable & 1;
      SPACE_DEFINED (space) = defined & 1;
      SPACE_PRIVATE (space) = private & 1;
      SPACE_SORT (space) = sort & 0xff;
      space->sd_defined = 1;
      space->sd_seg = seg;
    }
  return space;
}

void 
pa_align_subseg (seg, subseg)
#ifdef OBJ_SOM
     segT seg;
#else
     asection *seg;
#endif
     subsegT subseg;
{
  subspace_dict_chainS *now_subspace;
  int alignment;
  int shift;

  now_subspace = pa_subsegment_to_subspace (seg, subseg);
  if (SUBSPACE_ALIGN (now_subspace) == 0)
    alignment = now_subspace->ssd_last_align;
  else if (now_subspace->ssd_last_align > SUBSPACE_ALIGN (now_subspace))
    alignment = now_subspace->ssd_last_align;
  else
    alignment = SUBSPACE_ALIGN (now_subspace);

  shift = 0;
  while ((1 << shift) < alignment)
    shift++;

  frag_align (shift, 0);
}

void 
pa_space ()
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register symbolS *symbolP;
  register space_dict_chainS *sd_chain;
  char space_name[40];

  if (within_procedure)
    {
      as_bad ("Can\'t change spaces within a procedure definition. Ignored");
      ignore_rest_of_line ();
    }
  else
    {
      if (strncasecmp (input_line_pointer, "$text$", 6) == 0)
	{
	  input_line_pointer += 6;
	  sd_chain = is_defined_space ("$TEXT$");
	  if (sd_chain == NULL)
	    sd_chain = pa_parse_space_stmt ("$TEXT$", 1);
	  else if (sd_chain->sd_defined == 0)
	    sd_chain = pa_parse_space_stmt ("$TEXT$", 0);

	  current_space = sd_chain;
	  SPACE_DEFINED (current_space) = 1;
	  current_space->sd_defined = 1;
#ifdef OBJ_SOM
	  if (now_seg != SEG_TEXT)	/* no need to align if we are already there */
#else
	  if (now_seg != text_section)	/* no need to align if we are already there */
#endif
	    pa_align_subseg (now_seg, now_subseg);

#ifdef OBJ_SOM
	  subseg_new (SEG_TEXT, sd_chain->sd_last_subseg);
	  current_subspace = pa_subsegment_to_subspace (SEG_TEXT,
						  sd_chain->sd_last_subseg);
#else
	  subseg_new ((char *) text_section->name, sd_chain->sd_last_subseg);
	  current_subspace = pa_subsegment_to_subspace (text_section,
						  sd_chain->sd_last_subseg);
#endif
	  demand_empty_rest_of_line ();
	  return;
	}
      if (strncasecmp (input_line_pointer, "$private$", 9) == 0)
	{
	  input_line_pointer += 9;
	  sd_chain = is_defined_space ("$PRIVATE$");
	  if (sd_chain == NULL)
	    sd_chain = pa_parse_space_stmt ("$PRIVATE$", 1);
	  else if (sd_chain->sd_defined == 0)
	    sd_chain = pa_parse_space_stmt ("$PRIVATE$", 0);

	  current_space = sd_chain;
	  SPACE_DEFINED (current_space) = 1;
	  current_space->sd_defined = 1;
#ifdef OBJ_SOM
	  if (now_seg != SEG_DATA)	/* no need to align if we are already there */
#else
	  if (now_seg != data_section)	/* no need to align if we are already there */
#endif
	    pa_align_subseg (now_seg, now_subseg);
#ifdef OBJ_SOM
	  subseg_new (SEG_DATA, sd_chain->sd_last_subseg);
	  current_subspace = pa_subsegment_to_subspace (SEG_DATA,
						  sd_chain->sd_last_subseg);
#else
	  subseg_new ((char *) data_section->name, sd_chain->sd_last_subseg);
	  current_subspace = pa_subsegment_to_subspace (data_section,
						  sd_chain->sd_last_subseg);
#endif
	  demand_empty_rest_of_line ();
	  return;
	}
      if (strncasecmp (input_line_pointer,
		  GDB_DEBUG_SPACE_NAME, strlen (GDB_DEBUG_SPACE_NAME)) == 0)
	{
	  input_line_pointer += strlen (GDB_DEBUG_SPACE_NAME);
	  sd_chain = is_defined_space (GDB_DEBUG_SPACE_NAME);
	  if (sd_chain == NULL)
	    sd_chain = pa_parse_space_stmt (GDB_DEBUG_SPACE_NAME, 1);
	  else if (sd_chain->sd_defined == 0)
	    sd_chain = pa_parse_space_stmt (GDB_DEBUG_SPACE_NAME, 0);

	  current_space = sd_chain;
	  SPACE_DEFINED (current_space) = 1;
	  current_space->sd_defined = 1;
#ifdef OBJ_SOM
	  if (now_seg != SEG_GDB)	/* no need to align if we are already there */
	    pa_align_subseg (now_seg, now_subseg);
	  subseg_new (SEG_GDB, sd_chain->sd_last_subseg);
	  current_subspace = pa_subsegment_to_subspace (SEG_GDB,
						  sd_chain->sd_last_subseg);
#else
	  if (now_seg != gdb_section)	/* no need to align if we are already there */
	    pa_align_subseg (now_seg, now_subseg);
	  subseg_new ((char *) gdb_section->name, sd_chain->sd_last_subseg);
	  current_subspace = pa_subsegment_to_subspace (gdb_section,
						  sd_chain->sd_last_subseg);
#endif
	  demand_empty_rest_of_line ();
	  return;
	}

      /* it could be a space specified by number */

      if ((temp = pa_parse_number (&input_line_pointer)) >= 0)
	{
	  if (sd_chain = pa_find_space_by_number (temp))
	    {
	      current_space = sd_chain;
	      SPACE_DEFINED (current_space) = 1;
	      current_space->sd_defined = 1;
	      if (now_seg != sd_chain->sd_seg)	/* don't align if we're already there */
		pa_align_subseg (now_seg, now_subseg);
#ifdef OBJ_SOM
	      subseg_new (sd_chain->sd_seg, sd_chain->sd_last_subseg);
#else
	      subseg_new ((char *) sd_chain->sd_seg->name, sd_chain->sd_last_subseg);
#endif
	      current_subspace = pa_subsegment_to_subspace (sd_chain->sd_seg,
						  sd_chain->sd_last_subseg);
	      demand_empty_rest_of_line ();
	      return;
	    }
	}

      /* not a number, attempt to create a new space */

      name = input_line_pointer;
      c = get_symbol_end ();
      space_name[0] = 0x00;
      strcpy (space_name, name);
      *input_line_pointer = c;

      sd_chain = pa_parse_space_stmt (space_name, 1);
      current_space = sd_chain;
      SPACE_DEFINED (current_space) = 1;
      current_space->sd_defined = 1;
      if (now_seg != sd_chain->sd_seg)	/* don't align if we're already there */
	pa_align_subseg (now_seg, now_subseg);
#ifdef OBJ_SOM
      subseg_new (sd_chain->sd_seg, sd_chain->sd_last_subseg);
#else
      subseg_new ((char *) sd_chain->sd_seg->name, sd_chain->sd_last_subseg);
#endif
      current_subspace = pa_subsegment_to_subspace (sd_chain->sd_seg,
						  sd_chain->sd_last_subseg);
      demand_empty_rest_of_line ();
    }
  return;
}

void 
pa_spnum ()
{
  register char *name;
  register char c;
  register char *p;
  space_dict_chainS *space;

  name = input_line_pointer;
  c = get_symbol_end ();
  space = is_defined_space (name);
  if (space)
    {
      p = frag_more (4);
      /* put bytes in right order. */
      md_number_to_chars (p, SPACE_SPNUM (space), 4);
    }
  else
    as_warn ("Undefined space: '%s' Assuming space number = 0.", name);

  *input_line_pointer = c;
  demand_empty_rest_of_line ();
  return;
}

static
int 
is_power_of_2 (value)
     int value;
{
  int shift;

  shift = 0;
  while ((1 << shift) != value && shift < 32)
    shift++;

  if (shift >= 32)
    shift = 0;
  return shift;
}

void 
pa_subspace ()
{
  register char *name;
  register char c;
  register char *p;
  register int temp;
  register symbolS *symbolP;

  char loadable, code_only, common, dup_common, zero;
  char sort;
  int number;
  int i;
  int access;
  int space_index;
  int alignment;
  int quadrant;
  int subseg;
  space_dict_chainS *space;
  subspace_dict_chainS *ssd;
  subspace_dict_chainS *ssdCh;
  char *ss_name;
  int is_power_of_2 ();

  if (within_procedure)
    {
      as_bad ("Can\'t change subspaces within a procedure definition. Ignored");
      ignore_rest_of_line ();
    }
  else
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      space = pa_segment_to_space (now_seg);
      ssd = is_defined_subspace (name, space->sd_last_subseg);

      ss_name = xmalloc (strlen (name) + 1);
      strcpy (ss_name, name);

      *input_line_pointer = c;

      /* load default values */
      sort = 0;
      access = 0x7f;
      loadable = 1;
      common = 0;
      dup_common = 0;
      code_only = 0;
      zero = 0;
      space_index = ~0;		/* filled in when the .o file is written */
      alignment = 0;		/* alignment=0 means no ALIGN= appeared on the .SUBSPA */
      /* pseudo-op. The default will be the largest .ALIGN */
      /* encountered (or 1 if no .ALIGN is encountered) */
      quadrant = 0;

      if (ssd)
	{
	  if (ssd->ssd_defined)
	    {
#ifdef OBJ_SOM
	      subseg_new (now_seg, ssd->ssd_subseg);
#else
	      /* subseg_new(now_seg->name,ssd->ssd_subseg); */
	      subseg_new ((char *) ssd->ssd_seg->name, ssd->ssd_subseg);
#endif
	      if (!is_end_of_statement ())
		{
		  as_warn ("Parameters of an existing subspace can\'t be modified");
		}
	      demand_empty_rest_of_line ();
	      return;
	    }
	  else
	    {
	      ssd->ssd_defined = 1;
	    }
	}
      else
	{
	  /* a new subspace */
	  /* load default values */
	  i = 0;
	  while (pa_def_subspaces[i].name)
	    {
	      if (strcasecmp (pa_def_subspaces[i].name, ss_name) == 0)
		{
		  loadable = pa_def_subspaces[i].loadable;
		  common = pa_def_subspaces[i].common;
		  dup_common = pa_def_subspaces[i].dup_common;
		  code_only = pa_def_subspaces[i].code_only;
		  zero = pa_def_subspaces[i].zero;
		  space_index = pa_def_subspaces[i].space_index;
		  /* alignment = pa_def_subspaces[i].alignment; */
		  alignment = 0;
		  quadrant = pa_def_subspaces[i].quadrant;
		  access = pa_def_subspaces[i].access;
		  sort = pa_def_subspaces[i].sort;
		  break;
		}
	      i++;
	    }
	}

      if (!is_end_of_statement ())
	{
	  input_line_pointer++;
	  while (!is_end_of_statement ())
	    {
	      name = input_line_pointer;
	      c = get_symbol_end ();
	      if ((strncasecmp (name, "QUAD", 4) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  temp = get_absolute_expression ();
		  quadrant = temp;
		}
	      else if ((strncasecmp (name, "ALIGN", 5) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  temp = get_absolute_expression ();
		  alignment = temp;
		  if (!is_power_of_2 (alignment))
		    {
		      as_bad ("Alignment must be a power of 2");
		      alignment = 1;
		    }
		}
	      else if ((strncasecmp (name, "ACCESS", 6) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  temp = get_absolute_expression ();
		  access = temp;
		}
	      else if ((strncasecmp (name, "SORT", 4) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  temp = get_absolute_expression ();
		  sort = temp;
		}
	      else if ((strncasecmp (name, "CODE_ONLY", 9) == 0))
		{
		  *input_line_pointer = c;
		  code_only = 1;
		}
	      else if ((strncasecmp (name, "UNLOADABLE", 10) == 0))
		{
		  *input_line_pointer = c;
		  loadable = 0;
		}
	      else if ((strncasecmp (name, "COMMON", 6) == 0))
		{
		  *input_line_pointer = c;
		  common = 1;
		}
	      else if ((strncasecmp (name, "DUP_COMM", 8) == 0))
		{
		  *input_line_pointer = c;
		  dup_common = 1;
		}
	      else if ((strncasecmp (name, "ZERO", 4) == 0))
		{
		  *input_line_pointer = c;
		  zero = 1;
		}
	      else
		{
		  as_bad ("Unrecognized .SUBSPACE argument");
		}
	      if (!is_end_of_statement ())
		input_line_pointer++;
	    }
	}
      space = pa_segment_to_space (now_seg);
      if (ssd)
	{
	  current_subspace = update_subspace (ss_name, 1, loadable, code_only,
				     common, dup_common, sort, zero, access,
					   space_index, alignment, quadrant,
					      ssd->ssd_subseg);
	}
      else
	{
	  current_subspace = create_new_subspace (space, ss_name, 1, loadable, code_only,
					     common, dup_common, zero, sort,
					     access, space_index, alignment,
						  quadrant, now_seg);
	}
      SUBSPACE_SUBSPACE_START (current_subspace) = pa_subspace_start (space, quadrant);

      demand_empty_rest_of_line ();
#ifdef OBJ_SOM
      subseg_new (current_subspace->ssd_seg, current_subspace->ssd_subseg);
#else
      subseg_new ((char *) current_subspace->ssd_seg->name, current_subspace->ssd_subseg);
#endif
    }
  return;
}

/* For ELF, this function serves one purpose:  to setup the st_size	*/
/* field of STT_FUNC symbols.  To do this, we need to scan the 		*/
/* call_info structure list, determining st_size in one of two possible	*/
/* ways:								*/

/*	1. call_info->start_frag->fr_fix has the size of the fragment.	*/
/*	   This approach assumes that the function was built into a 	*/
/*	   single fragment.  This works for most cases, but might fail.	*/
/*	   For example, if there was a segment change in the middle of	*/
/*	   the function.						*/

/*	2. The st_size field is the difference in the addresses of the	*/
/*	   call_info->start_frag->fr_address field and the fr_address	*/
/*	   field of the next fragment with fr_type == rs_fill and	*/
/*	   fr_fix != 0.							*/

void 
elf_hppa_final_processing_hook ()
{
  extern call_infoS *call_info_root;

  call_infoS *ciP;

  for (ciP = call_info_root; ciP; ciP = ciP->ci_next)
    {
      elf_symbol_type *esym = (elf_symbol_type *) ciP->start_symbol->bsym;
      esym->internal_elf_sym.st_size =
	ciP->end_symbol->bsym->value
	- ciP->start_symbol->bsym->value + 4;
    }
}

/* pa-spaces.c -- Space/subspace support for the HP PA-RISC version of GAS */

/* for space, subspace, and symbol maintenance on HP 9000 Series 800 */

space_dict_chainS *space_dict_root;
space_dict_chainS *space_dict_last;

space_dict_chainS *current_space;
subspace_dict_chainS *current_subspace;

extern symbolS *start_symbol_root;
extern symbolS *start_symbol_last;

void 
pa_spaces_begin ()
{
  space_dict_chainS *space;
  int i;
  subsegT now_subseg = now_subseg;

  space_dict_root = NULL;
  space_dict_last = NULL;

  start_symbol_root = NULL;
  start_symbol_last = NULL;

  /* create default space and subspace dictionaries */

  i = 0;
  while (pa_def_spaces[i].name)
    {
      if (pa_def_spaces[i].alias)
	pa_def_spaces[i].segment = subseg_new (pa_def_spaces[i].alias, 0);
      else
	pa_def_spaces[i].segment = bfd_make_section_old_way (stdoutput, pa_def_spaces[i].name);

      create_new_space (pa_def_spaces[i].name, pa_def_spaces[i].spnum,
			pa_def_spaces[i].loadable, pa_def_spaces[i].defined,
			pa_def_spaces[i].private, pa_def_spaces[i].sort, 0,
			pa_def_spaces[i].segment);
      i++;
    }

  i = 0;
  while (pa_def_subspaces[i].name)
    {
      space = pa_segment_to_space (pa_def_spaces[pa_def_subspaces[i].def_space_index].segment);
      if (space)
	{
	  char *name = pa_def_subspaces[i].alias;
	  if (!name)
	    name = pa_def_subspaces[i].name;
	  create_new_subspace (space, name,
			       pa_def_subspaces[i].defined,
			       pa_def_subspaces[i].loadable,
		  pa_def_subspaces[i].code_only, pa_def_subspaces[i].common,
		   pa_def_subspaces[i].dup_common, pa_def_subspaces[i].zero,
		       pa_def_subspaces[i].sort, pa_def_subspaces[i].access,
			       pa_def_subspaces[i].space_index,
			       pa_def_subspaces[i].alignment,
			       pa_def_subspaces[i].quadrant,
		pa_def_spaces[pa_def_subspaces[i].def_space_index].segment);
	  subseg_new (name, pa_def_subspaces[i].subsegment);
	}
      else
	as_fatal ("Internal error: space missing for subspace \"%s\"\n",
		  pa_def_subspaces[i].name);
      i++;
    }
}

space_dict_chainS *
create_new_space (name, spnum, loadable, defined, private, sort, defined_in_file, seg)
     char *name;
     int spnum;
     char loadable;
     char defined;
     char private;
     char sort;
     char defined_in_file;
     asection *seg;

{
  Elf_Internal_Shdr *new_space;
  space_dict_chainS *chain_entry;

  new_space = (Elf_Internal_Shdr *) xmalloc (sizeof (Elf_Internal_Shdr));
  if (!new_space)
    as_fatal ("Out of memory: could not allocate new Elf_Internal_Shdr: %s\n", name);

  /*
  new_space->space_number = spnum;
  new_space->is_loadable = loadable & 1;
  new_space->is_defined = defined & 1;
  new_space->is_private = private & 1;
  new_space->sort_key = sort & 0xff;

  new_space->loader_fix_index = ~0;
  new_space->loader_fix_quantity = 0;
  new_space->init_pointer_index = ~0;
  new_space->init_pointer_quantity = 0;
  new_space->subspace_quantity = 0;
  */

  chain_entry = (space_dict_chainS *) xmalloc (sizeof (space_dict_chainS));
  if (!chain_entry)
    as_fatal ("Out of memory: could not allocate new space chain entry: %s\n", name);

  SPACE_NAME (chain_entry) = (char *) xmalloc (strlen (name) + 1);
  strcpy (SPACE_NAME (chain_entry), name);

  chain_entry->sd_entry = new_space;
  chain_entry->sd_defined = defined_in_file;
  chain_entry->sd_seg = seg;
  chain_entry->sd_last_subseg = -1;
  chain_entry->sd_next = NULL;

  SPACE_SPNUM (chain_entry) = spnum;
  SPACE_LOADABLE (chain_entry) = loadable & 1;
  SPACE_DEFINED (chain_entry) = defined & 1;
  SPACE_PRIVATE (chain_entry) = private & 1;
  SPACE_SORT (chain_entry) = sort & 0xff;

  /* find spot for the new space based on its sort key */

  if (!space_dict_last)
    space_dict_last = chain_entry;

  if (space_dict_root == NULL)	/* if root is null, it is very easy */
    space_dict_root = chain_entry;
  else
    {
      space_dict_chainS *sdcP;
      space_dict_chainS *last_sdcP;

      sdcP = space_dict_root;
      last_sdcP = NULL;

      while (sdcP)
	{
	  if (SPACE_SORT (sdcP) < SPACE_SORT (chain_entry))
	    {
	      last_sdcP = sdcP;
	      sdcP = sdcP->sd_next;
	    }
	  else if (SPACE_SORT (sdcP) == SPACE_SORT (chain_entry))
	    {
	      last_sdcP = sdcP;
	      sdcP = sdcP->sd_next;
	    }
	  else if (SPACE_SORT (sdcP) > SPACE_SORT (chain_entry))
	    {
	      break;
	    }
	}

      if (last_sdcP)
	{
	  chain_entry->sd_next = sdcP;
	  last_sdcP->sd_next = chain_entry;
	}
      else
	{
	  space_dict_root = chain_entry;
	  chain_entry->sd_next = sdcP;
	}

      if (chain_entry->sd_next == NULL)
	space_dict_last = chain_entry;
    }

  return chain_entry;
}

subspace_dict_chainS
* create_new_subspace (space, name, defined, loadable, code_only, common, dup_common,
	       is_zero, sort, access, space_index, alignment, quadrant, seg)
     space_dict_chainS *space;
     char *name;
     char defined, loadable, code_only, common, dup_common, is_zero;
     char sort;
     int access;
     int space_index;
     int alignment;
     int quadrant;
     asection *seg;
{
  Elf_Internal_Shdr *new_subspace;
  subspace_dict_chainS *chain_entry;
  symbolS *start_symbol;

  new_subspace = (Elf_Internal_Shdr *) xmalloc (sizeof (Elf_Internal_Shdr));
  if (!new_subspace)
    as_fatal ("Out of memory: could not allocate new Elf_Internal_Shdr: %s\n",
	      name);

  /*
  new_subspace->space_index = space_index;
  new_subspace->fixup_request_index = ~0;
  */

  chain_entry = (subspace_dict_chainS *) xmalloc (sizeof (subspace_dict_chainS));
  if (!chain_entry)
    as_fatal ("Out of memory: could not allocate new subspace chain entry: %s\n", name);

  chain_entry->ssd_entry = new_subspace;
  SUBSPACE_NAME (chain_entry) = (char *) xmalloc (strlen (name) + 1);
  strcpy (SUBSPACE_NAME (chain_entry), name);

  SUBSPACE_ACCESS (chain_entry) = access & 0x7f;
  SUBSPACE_LOADABLE (chain_entry) = loadable & 1;
  SUBSPACE_COMMON (chain_entry) = common & 1;
  SUBSPACE_DUP_COMM (chain_entry) = dup_common & 1;
  SUBSPACE_SORT (chain_entry) = sort & 0xff;
  SET_SUBSPACE_CODE_ONLY (chain_entry, code_only & 1);
  SUBSPACE_ALIGN (chain_entry) = alignment & 0xffff;
  SUBSPACE_QUADRANT (chain_entry) = quadrant & 0x3;
  SUBSPACE_SUBSPACE_START (chain_entry) = pa_subspace_start (space, quadrant);

  chain_entry->ssd_defined = defined;
  chain_entry->ssd_space_number = space_index;
  chain_entry->ssd_subseg = pa_next_subseg (space);
  chain_entry->ssd_seg = seg;
  SUBSPACE_ZERO (chain_entry) = is_zero;
  chain_entry->ssd_last_align = 1;
  chain_entry->ssd_next = NULL;

  /* find spot for the new subspace based on its sort key */

  if (space->sd_subspaces == NULL)	/* if root is null, it is very easy */
    space->sd_subspaces = chain_entry;
  else
    {
      subspace_dict_chainS *ssdcP;
      subspace_dict_chainS *last_ssdcP;

      ssdcP = space->sd_subspaces;
      last_ssdcP = NULL;

      while (ssdcP)
	{
	  if (SUBSPACE_SORT (ssdcP) < SUBSPACE_SORT (chain_entry))
	    {
	      last_ssdcP = ssdcP;
	      ssdcP = ssdcP->ssd_next;
	    }
	  else if (SUBSPACE_SORT (ssdcP) == SUBSPACE_SORT (chain_entry))
	    {
	      last_ssdcP = ssdcP;
	      ssdcP = ssdcP->ssd_next;
	    }
	  else if (SUBSPACE_SORT (ssdcP) > SUBSPACE_SORT (chain_entry))
	    {
	      break;
	    }
	}

      if (last_ssdcP)
	{
	  chain_entry->ssd_next = ssdcP;
	  last_ssdcP->ssd_next = chain_entry;
	}
      else
	{
	  space->sd_subspaces = chain_entry;
	  chain_entry->ssd_next = ssdcP;
	}
    }

  start_symbol = pa_set_start_symbol (seg, space->sd_last_subseg);
  chain_entry->ssd_start_sym = start_symbol;
  return chain_entry;

}

subspace_dict_chainS
* update_subspace (name, defined, loadable, code_only, common, dup_common, sort, zero,
		   access, space_index, alignment, quadrant, subseg)
     char *name;
     char defined, loadable, code_only, common, dup_common, zero;
     char sort;
     int access;
     int space_index;
     int alignment;
     int quadrant;
     subsegT subseg;
{
  subspace_dict_chainS *chain_entry;
  subspace_dict_chainS *is_defined_subspace ();

  if ((chain_entry = is_defined_subspace (name, subseg)))
    {

      SUBSPACE_ACCESS (chain_entry) = access & 0x7f;
      SUBSPACE_LOADABLE (chain_entry) = loadable & 1;
      SUBSPACE_COMMON (chain_entry) = common & 1;
      SUBSPACE_DUP_COMM (chain_entry) = dup_common & 1;
      SET_SUBSPACE_CODE_ONLY (chain_entry, code_only & 1);
      SUBSPACE_SORT (chain_entry) = sort & 0xff;
      /* chain_entry->ssd_entry->space_index = space_index; */
      SUBSPACE_ALIGN (chain_entry) = alignment & 0xffff;
      SUBSPACE_QUADRANT (chain_entry) = quadrant & 0x3;

      chain_entry->ssd_defined = defined;
      chain_entry->ssd_space_number = space_index;
      SUBSPACE_ZERO (chain_entry) = zero;
    }
  else
    chain_entry = NULL;

  return chain_entry;

}

space_dict_chainS *
is_defined_space (name)
     char *name;
{
  space_dict_chainS *spaceCh;

  for (spaceCh = space_dict_root; spaceCh; spaceCh = spaceCh->sd_next)
    {
      if (strcmp (SPACE_NAME (spaceCh), name) == 0)
	{
	  return spaceCh;
	}
    }

  return NULL;
}

space_dict_chainS *
pa_segment_to_space (seg)
     asection *seg;
{
  space_dict_chainS *spaceCh;

  for (spaceCh = space_dict_root; spaceCh; spaceCh = spaceCh->sd_next)
    {
      if (spaceCh->sd_seg == seg)
	{
	  return spaceCh;
	}
    }

  return NULL;
}

subspace_dict_chainS *
is_defined_subspace (name, subseg)
     char *name;
     subsegT subseg;
{
  space_dict_chainS *spaceCh;
  subspace_dict_chainS *subspCh;

  for (spaceCh = space_dict_root; spaceCh; spaceCh = spaceCh->sd_next)
    {
      for (subspCh = spaceCh->sd_subspaces; subspCh; subspCh = subspCh->ssd_next)
	{
	  /*
	if ( strcmp(SUBSPACE_NAME(subspCh),name) == 0 &&
	    subspCh->ssd_subseg == subseg ) {
       */
	  if (strcmp (SUBSPACE_NAME (subspCh), name) == 0)
	    {
	      return subspCh;
	    }
	}
    }
  return NULL;
}

subspace_dict_chainS *
pa_subsegment_to_subspace (seg, subseg)
     asection *seg;
     subsegT subseg;
{
  space_dict_chainS *spaceCh;
  subspace_dict_chainS *subspCh;

  for (spaceCh = space_dict_root; spaceCh; spaceCh = spaceCh->sd_next)
    {
      if (spaceCh->sd_seg == seg)
	{
	  for (subspCh = spaceCh->sd_subspaces; subspCh; subspCh = subspCh->ssd_next)
	    {
	      if (subspCh->ssd_subseg == (int) subseg)
		{
		  return subspCh;
		}
	    }
	}
    }

  return NULL;
}

space_dict_chainS *
pa_find_space_by_number (number)
     int number;
{
  space_dict_chainS *spaceCh;

  for (spaceCh = space_dict_root; spaceCh; spaceCh = spaceCh->sd_next)
    {
      if (SPACE_SPNUM (spaceCh) == number)
	{
	  return spaceCh;
	}
    }

  return NULL;
}

unsigned int 
pa_subspace_start (space, quadrant)
     space_dict_chainS *space;
     int quadrant;
{
  if ((strcasecmp (SPACE_NAME (space), "$PRIVATE$") == 0) &&
      quadrant == 1)
    {
      return 0x40000000;
    }
  else if (space->sd_seg == data_section && quadrant == 1)
    {				/* in case name is */
      /* already converted */
      /* to a space dict- */
      /* ionary index */
      return 0x40000000;
    }
  else
    return 0;
}

int 
pa_next_subseg (space)
     space_dict_chainS *space;
{

  space->sd_last_subseg++;
  return space->sd_last_subseg;
}

int 
is_last_defined_subspace (ssd)
     subspace_dict_chainS *ssd;
{

  for (; ssd; ssd = ssd->ssd_next)
    {
      if (ssd->ssd_defined)
	return FALSE;
    }

  return TRUE;
}

symbolS *
pa_get_start_symbol (seg, subseg)
     asection *seg;
     subsegT subseg;
{
  symbolS *start_symbol;
  subspace_dict_chainS *ssd;

  start_symbol = NULL;

  /* each time a new space is created, build a symbol called LS$START_seg_subseg$ */
  /* where <space-name> is the name of the space */
  /* the start symbol will be SS_LOCAL and ST_CODE */

  if (seg == bfd_make_section_old_way (stdoutput, ".text") ||
      seg == bfd_make_section_old_way (stdoutput, ".data") ||
      seg == bfd_make_section_old_way (stdoutput, GDB_DEBUG_SPACE_NAME))
    {
      ssd = pa_subsegment_to_subspace (seg, subseg);
      if (ssd)
	{
	  start_symbol = ssd->ssd_start_sym;
	}
      else
	as_fatal ("Internal error: missing subspace for (seg,subseg)=('%s',%d)",
		  seg->name, subseg);
    }
  else
    as_fatal ("Internal error: attempt to find start symbol for unloadable segment: '%s'",
	      seg->name);

  return start_symbol;
}

/*
  Function to define a symbol whose address is the beginning of a subspace.
  This function assumes the symbol is to be defined for the current subspace.
 */

symbolS *
pa_set_start_symbol (seg, subseg)
     asection *seg;
     subsegT subseg;
{
  symbolS *start_symbol;
  subspace_dict_chainS *ssd;
  char *symbol_name;

  symbol_name = (char *) xmalloc (strlen ("LS$START__000000$") + strlen (seg->name) + 1);

  sprintf (symbol_name, "LS$START_%s_%03d$", seg->name, subseg);

  start_symbol
    = symbol_new (symbol_name, seg, 0, frag_now);	/* XXX: not sure if value s.b. 0 or frag s.b. NULL */

  start_symbol->bsym->flags = BSF_LOCAL;	/* XXX: isn't there a macro defined for this? */

  /* each time a new space is created, build a symbol called LS$START_seg_subseg$ */
  /* where <space-name> is the name of the space */
  /* the start symbol will be SS_LOCAL and ST_CODE */
  /* This function assumes that (seg,subseg) is a new subsegment(subspace) */

  if (seg == bfd_make_section_old_way (stdoutput, ".text") ||
      seg == bfd_make_section_old_way (stdoutput, ".data") ||
      seg == bfd_make_section_old_way (stdoutput, GDB_DEBUG_SPACE_NAME))
    {
      ssd = pa_subsegment_to_subspace (seg, subseg);
      if (ssd)
	{
	  ssd->ssd_start_sym = start_symbol;
	}
      else
	as_fatal ("Internal error: missing subspace for (seg,subseg)=('%s',%d)",
		  seg, subseg);
    }
  else
    as_fatal ("Internal error: attempt to define start symbol for unloadable segment: '%s'",
	      seg->name);

  return start_symbol;
}

static unsigned int
pa_stringer_aux (s)
     char *s;
{
  unsigned int c = *s & CHAR_MASK;
  switch (c)
    {
    case '\"':
      c = NOT_A_CHAR;
      break;
    default:
      break;
    }
  return c;
}

void
pa_stringer (append_zero)	/* Worker to do .ascii etc statements. */
     /* Checks end-of-line. */
     register int append_zero;	/* 0: don't append '\0', else 1 */
{
  char *s;
  unsigned int c;
  char num_buf[4];
  int i;

  /* Preprocess the string to handle PA-specific escape sequences.	*/
  /* For example, \xDD where DD is a hexidecimal number should be	*/
  /* changed to \OOO where OOO is an octal number.			*/

  s = input_line_pointer + 1;	/* skip the opening quote */

  while (is_a_char (c = pa_stringer_aux (s++)))
    {
      if (c == '\\')
	{
	  c = *s;
	  switch (c)
	    {
	    case 'x':
	      {
		unsigned int number;
		int num_digit;
		char dg;
		char *s_start = s;

		s++;		/* get past the 'x' */
		for (num_digit = 0, number = 0, dg = *s;
		     num_digit < 2
		     && (isdigit (dg) || (dg >= 'a' && dg <= 'f')
			 || (dg >= 'A' && dg <= 'F'));
		     num_digit++)
		  {
		    if (isdigit (dg))
		      number = number * 16 + dg - '0';
		    else if (dg >= 'a' && dg <= 'f')
		      number = number * 16 + dg - 'a' + 10;
		    else
		      number = number * 16 + dg - 'A' + 10;

		    s++;
		    dg = *s;
		  }
		if (num_digit > 0)
		  {
		    switch (num_digit)
		      {
		      case 1:
			sprintf (num_buf, "%02o", number);
			break;
		      case 2:
			sprintf (num_buf, "%03o", number);
			break;
		      }
		    for (i = 0; i <= num_digit; i++)
		      s_start[i] = num_buf[i];
		  }
	      }
	      break;
	    }
	}
    }
  stringer (append_zero);
  pa_undefine_label ();
}

void
pa_version ()
{
#ifdef OBJ_ELF
  obj_elf_version ();
#endif
  pa_undefine_label ();
}

void
pa_cons (nbytes)
     register unsigned int nbytes;	/* 1=.byte, 2=.word, 4=.long */
{
  cons (nbytes);
  pa_undefine_label ();
}

void
pa_data ()
{
  s_data ();
  pa_undefine_label ();
}

void
pa_desc ()
{

#ifdef OBJ_ELF
  obj_elf_desc ();
#endif
  pa_undefine_label ();
}

void
pa_float_cons (float_type)
     register int float_type;	/* 'f':.ffloat ... 'F':.float ... */
{
  float_cons (float_type);
  pa_undefine_label ();
}

void
pa_fill ()
{
  s_fill ();
  pa_undefine_label ();
}

void
pa_lcomm (needs_align)
     /* 1 if this was a ".bss" directive, which may require a 3rd argument
	(alignment); 0 if it was an ".lcomm" (2 args only)  */
     int needs_align;
{
  s_lcomm (needs_align);
  pa_undefine_label ();
}

void
pa_lsym ()
{
  s_lsym ();
  pa_undefine_label ();
}

void
pa_big_cons (nbytes)
     register int nbytes;
{
  big_cons (nbytes);
  pa_undefine_label ();
}

void
pa_text ()
{
  s_text ();
  pa_undefine_label ();
}
