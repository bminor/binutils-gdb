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


/* HP PA-RISC support was contributed by the Center for Software Science
   at the University of Utah.  */

#include <stdio.h>
#include <ctype.h>

#include "as.h"
#include "subsegs.h"

#include "../bfd/libhppa.h"
#include "../bfd/libbfd.h"

/* Be careful, this file includes data *declarations*.  */
#include "opcode/hppa.h"

/* A "convient" place to put object file dependencies which do
   not need to be seen outside of tc-hppa.c.  */
#ifdef OBJ_ELF
/* Names of various debugging spaces/subspaces.  */
#define GDB_DEBUG_SPACE_NAME ".stab"
#define GDB_STRINGS_SUBSPACE_NAME ".stabstr"
#define GDB_SYMBOLS_SUBSPACE_NAME ".stab"
#define UNWIND_SECTION_NAME ".hppa_unwind"
/* Nonzero if CODE is a fixup code needing further processing.  */

/* Object file formats specify relocation types.  */
typedef elf32_hppa_reloc_type reloc_type;

/* Object file formats specify BFD symbol types.  */
typedef elf_symbol_type obj_symbol_type;

/* How to generate a relocation.  */
#define hppa_gen_reloc_type hppa_elf_gen_reloc_type

/* Who knows.  */
#define obj_version obj_elf_version

/* Use space aliases.  */
#define USE_ALIASES 1

/* Some local functions only used by ELF.  */
static void pa_build_symextn_section PARAMS ((void));
static void hppa_tc_make_symextn_section PARAMS ((void));
#endif

#ifdef OBJ_SOM
/* Names of various debugging spaces/subspaces.  */
#define GDB_DEBUG_SPACE_NAME "$GDB_DEBUG$"
#define GDB_STRINGS_SUBSPACE_NAME "$GDB_STRINGS$"
#define GDB_SYMBOLS_SUBSPACE_NAME "$GDB_SYMBOLS$"
#define UNWIND_SECTION_NAME "$UNWIND$"

/* Object file formats specify relocation types.  */
typedef int reloc_type;

/* Who knows.  */
#define obj_version obj_som_version

/* Do not use space aliases.  */
#define USE_ALIASES 0

/* How to generate a relocation.  */
#define hppa_gen_reloc_type hppa_som_gen_reloc_type

/* Object file formats specify BFD symbol types.  */
typedef som_symbol_type obj_symbol_type;
#endif

/* Various structures and types used internally in tc-hppa.c.  */

/* Unwind table and descriptor.  FIXME: Sync this with GDB version.  */

struct unwind_desc
  {
    unsigned int cannot_unwind:1;
    unsigned int millicode:1;
    unsigned int millicode_save_rest:1;
    unsigned int region_desc:2;
    unsigned int save_sr:2;
    unsigned int entry_fr:4;
    unsigned int entry_gr:5;
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

struct unwind_table
  {
    /* Starting and ending offsets of the region described by
       descriptor.  */
    unsigned int start_offset;
    unsigned int end_offset;
    struct unwind_desc descriptor;
  };

/* This structure is used by the .callinfo, .enter, .leave pseudo-ops to
   control the entry and exit code they generate. It is also used in
   creation of the correct stack unwind descriptors.

   NOTE:  GAS does not support .enter and .leave for the generation of
   prologues and epilogues.  FIXME.

   The fields in structure roughly correspond to the arguments available on the
   .callinfo pseudo-op.  */

struct call_info
  {
    /* Should sr3 be saved in the prologue?  */
    int entry_sr;

    /* Does this function make calls?  */
    int makes_calls;

    /* The unwind descriptor being built.  */
    struct unwind_table ci_unwind;

    /* Name of this function.  */
    symbolS *start_symbol;

    /* (temporary) symbol used to mark the end of this function.  */
    symbolS *end_symbol;

    /* frags associated with start and end of this function.  */
    fragS *start_frag;
    fragS *end_frag;

    /* frags for starting/ending offset of this descriptor.  */
    fragS *start_offset_frag;
    fragS *end_offset_frag;

    /* The location within {start,end}_offset_frag to find the 
       {start,end}_offset.  */
    int start_frag_where;
    int end_frag_where;

    /* Fixups (relocations) for start_offset and end_offset.  */
    fixS *start_fix;
    fixS *end_fix;

    /* Next entry in the chain.  */
    struct call_info *ci_next;
  };

/* Operand formats for FP instructions.   Note not all FP instructions
   allow all four formats to be used (for example fmpysub only allows
   SGL and DBL).  */
typedef enum
  {
    SGL, DBL, ILLEGAL_FMT, QUAD
  }
fp_operand_format;

/* This fully describes the symbol types which may be attached to
   an EXPORT or IMPORT directive.  Only SOM uses this formation
   (ELF has no need for it).  */
typedef enum
{
  SYMBOL_TYPE_UNKNOWN,
  SYMBOL_TYPE_ABSOLUTE,
  SYMBOL_TYPE_CODE,
  SYMBOL_TYPE_DATA,
  SYMBOL_TYPE_ENTRY,
  SYMBOL_TYPE_MILLICODE,
  SYMBOL_TYPE_PLABEL,
  SYMBOL_TYPE_PRI_PROG,
  SYMBOL_TYPE_SEC_PROG,
} pa_symbol_type;

/* This structure contains information needed to assemble 
   individual instructions.  */
struct pa_it
  {
    /* Holds the opcode after parsing by pa_ip.  */
    unsigned long opcode;

    /* Holds an expression associated with the current instruction.  */
    expressionS exp;

    /* Does this instruction use PC-relative addressing.  */
    int pcrel;

    /* Floating point formats for operand1 and operand2.  */
    fp_operand_format fpof1;
    fp_operand_format fpof2;

    /* Holds the field selector for this instruction
       (for example L%, LR%, etc).  */
    long field_selector;

    /* Holds any argument relocation bits associated with this
       instruction.  (instruction should be some sort of call).  */
    long arg_reloc;

    /* The format specification for this instruction.  */
    int format;

    /* The relocation (if any) associated with this instruction.  */
    reloc_type reloc;
  };

/* PA-89 floating point registers are arranged like this:


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
   encoded in the structure 'pa_89_fp_reg_struct'.  */

struct pa_89_fp_reg_struct
  {
    /* The register number.  */
    char number_part;

    /* L/R selector.  */
    char l_r_select;
  };

/* Additional information needed to build argument relocation stubs.  */
struct call_desc
  {
    /* The argument relocation specification.  */
    unsigned int arg_reloc;

    /* Number of arguments.  */
    unsigned int arg_count;
  };

/* This structure defines an entry in the subspace dictionary
   chain.  */

struct subspace_dictionary_chain
  {
    /* Index of containing space.  */
    unsigned long ssd_space_index;

    /* Nonzero if this space has been defined by the user code.  */
    unsigned int ssd_defined;

    /* Which quadrant within the space this subspace should be loaded into.  */
    unsigned char ssd_quadrant;

    /* Alignment (in bytes) for this subspace.  */
    unsigned long ssd_alignment;

    /* Access control bits to determine read/write/execute permissions
       as well as gateway privilege promotions.  */
    unsigned char ssd_access_control_bits;

    /* A sorting key so that it is possible to specify ordering of
       subspaces within a space.  */
    unsigned char ssd_sort_key;

    /* Nonzero of this space should be zero filled.  */
    unsigned long ssd_zero;

    /* Nonzero if this is a common subspace.  */
    unsigned char ssd_common;

    /* Nonzero if this is a common subspace which allows symbols to be 
       multiply defined.  */
    unsigned char ssd_dup_common;

    /* Nonzero if this subspace is loadable.  Note loadable subspaces
       must be contained within loadable spaces; unloadable subspaces
       must be contained in unloadable spaces.  */
    unsigned char ssd_loadable;

    /* Nonzero if this subspace contains only code.  */ 
    unsigned char ssd_code_only;

    /* Starting offset of this subspace.  */
    unsigned long ssd_subspace_start;

    /* Length of this subspace.  */
    unsigned long ssd_subspace_length;

    /* Name of this subspace.  */
    char *ssd_name;

    /* GAS segment and subsegment associated with this subspace.  */
    asection *ssd_seg;
    int ssd_subseg;

    /* Index of this subspace within the subspace dictionary of the object
       file.  Not used until object file is written.  */
    int object_file_index;

    /* The size of the last alignment request for this subspace.  */
    int ssd_last_align;

    /* Next space in the subspace dictionary chain.  */
    struct subspace_dictionary_chain *ssd_next;
  };

typedef struct subspace_dictionary_chain ssd_chain_struct;

/* This structure defines an entry in the subspace dictionary
   chain.  */

struct space_dictionary_chain
  {

    /* Holds the index into the string table of the name of this 
       space.  */
    unsigned int sd_name_index;

    /* Nonzero if the space is loadable.  */
    unsigned int sd_loadable;

    /* Nonzero if this space has been defined by the user code or 
       as a default space.  */
    unsigned int sd_defined;

    /* Nonzero if this spaces has been defined by the user code.  */
    unsigned int sd_user_defined;

    /* Nonzero if this space is not sharable.  */
    unsigned int sd_private;

    /* The space number (or index).  */
    unsigned int sd_spnum;

    /* The sort key for this space.  May be used to determine how to lay
       out the spaces within the object file.  */
    unsigned char sd_sort_key;

    /* The name of this subspace.  */
    char *sd_name;

    /* GAS segment to which this subspace corresponds.  */
    asection *sd_seg;

    /* Current subsegment number being used.  */
    int sd_last_subseg;

    /* The chain of subspaces contained within this space.  */
    ssd_chain_struct *sd_subspaces;

    /* The next entry in the space dictionary chain.  */
    struct space_dictionary_chain *sd_next;
  };

typedef struct space_dictionary_chain sd_chain_struct;

/* Structure for previous label tracking.  Needed so that alignments,
   callinfo declarations, etc can be easily attached to a particular
   label.  */
typedef struct label_symbol_struct
  {
    struct symbol *lss_label;
    sd_chain_struct *lss_space;
    struct label_symbol_struct *lss_next;
  }
label_symbol_struct;

/* This structure defines attributes of the default subspace
   dictionary entries.  */

struct default_subspace_dict
  {
    /* Name of the subspace.  */ 
    char *name;

    /* FIXME.  Is this still needed?  */
    char defined;

    /* Nonzero if this subspace is loadable.  */
    char loadable;

    /* Nonzero if this subspace contains only code.  */
    char code_only;

    /* Nonzero if this is a common subspace.  */
    char common;

    /* Nonzero if this is a common subspace which allows symbols
       to be multiply defined.  */
    char dup_common;

    /* Nonzero if this subspace should be zero filled.  */
    char zero;

    /* Sort key for this subspace.  */
    unsigned char sort;

    /* Access control bits for this subspace.  Can represent RWX access
       as well as privilege level changes for gateways.  */
    int access;

    /* Index of containing space.  */
    int space_index;

    /* Alignment (in bytes) of this subspace.  */
    int alignment;

    /* Quadrant within space where this subspace should be loaded.  */
    int quadrant;

    /* An index into the default spaces array.  */
    int def_space_index;

    /* An alias for this section (or NULL if no alias exists).  */
    char *alias;

    /* Subsegment associated with this subspace.  */
    subsegT subsegment;
  };

/* This structure defines attributes of the default space
   dictionary entries.  */

struct default_space_dict
  {
    /* Name of the space.  */
    char *name;

    /* Space number.  It is possible to identify spaces within
       assembly code numerically!  */
    int spnum;

    /* Nonzero if this space is loadable.  */
    char loadable;

    /* Nonzero if this space is "defined".  FIXME is still needed */
    char defined;

    /* Nonzero if this space can not be shared.  */
    char private;

    /* Sort key for this space.  */
    unsigned char sort;

    /* Segment associated with this space.  */
    asection *segment;

    /* An alias for this section (or NULL if no alias exists).  */
    char *alias;
  };

/* Extra information needed to perform fixups (relocations) on the PA.  */
struct hppa_fix_struct
{
    /* The field selector.  */
    int fx_r_field;

    /* Type of fixup.  */
    int fx_r_type;

    /* Format of fixup.  */
    int fx_r_format;

    /* Argument relocation bits.  */
    long fx_arg_reloc;

    /* The unwind descriptor associated with this fixup.  */
    char fx_unwind[8];
};

/* Structure to hold information about predefined registers.  */

struct pd_reg
{
  char *name;
  int value;
};

/* This structure defines the mapping from a FP condition string
   to a condition number which can be recorded in an instruction.  */
struct fp_cond_map
{
  char *string;
  int cond;
};

/* This structure defines a mapping from a field selector
   string to a field selector type.  */
struct selector_entry
{
  char *prefix;
  int field_selector;
};

/* Prototypes for functions local to tc-hppa.c.  */

static fp_operand_format pa_parse_fp_format PARAMS ((char **s));
static void pa_cons PARAMS ((int));
static void pa_data PARAMS ((int));
static void pa_desc PARAMS ((int));
static void pa_float_cons PARAMS ((int));
static void pa_fill PARAMS ((int));
static void pa_lcomm PARAMS ((int));
static void pa_lsym PARAMS ((int));
static void pa_stringer PARAMS ((int));
static void pa_text PARAMS ((int));
static void pa_version PARAMS ((int));
static int pa_parse_fp_cmp_cond PARAMS ((char **));
static int get_expression PARAMS ((char *));
static int pa_get_absolute_expression PARAMS ((char *));
static int evaluate_absolute PARAMS ((expressionS, int));
static unsigned int pa_build_arg_reloc PARAMS ((char *));
static unsigned int pa_align_arg_reloc PARAMS ((unsigned int, unsigned int));
static int pa_parse_nullif PARAMS ((char **));
static int pa_parse_nonneg_cmpsub_cmpltr PARAMS ((char **, int));
static int pa_parse_neg_cmpsub_cmpltr PARAMS ((char **, int));
static int pa_parse_neg_add_cmpltr PARAMS ((char **, int));
static int pa_parse_nonneg_add_cmpltr PARAMS ((char **, int));
static void pa_block PARAMS ((int));
static void pa_call PARAMS ((int));
static void pa_call_args PARAMS ((struct call_desc *));
static void pa_callinfo PARAMS ((int));
static void pa_code PARAMS ((int));
static void pa_comm PARAMS ((int));
static void pa_copyright PARAMS ((int));
static void pa_end PARAMS ((int));
static void pa_enter PARAMS ((int));
static void pa_entry PARAMS ((int));
static void pa_equ PARAMS ((int));
static void pa_exit PARAMS ((int));
static void pa_export PARAMS ((int));
static void pa_export_args PARAMS ((symbolS *));
static void pa_import PARAMS ((int));
static void pa_label PARAMS ((int));
static void pa_leave PARAMS ((int));
static void pa_origin PARAMS ((int));
static void pa_proc PARAMS ((int));
static void pa_procend PARAMS ((int));
static void pa_space PARAMS ((int));
static void pa_spnum PARAMS ((int));
static void pa_subspace PARAMS ((int));
static void pa_param PARAMS ((int));
static void pa_undefine_label PARAMS ((void));
static int need_89_opcode PARAMS ((struct pa_it *, 
				   struct pa_89_fp_reg_struct *));
static int pa_parse_number PARAMS ((char **, struct pa_89_fp_reg_struct *));
static label_symbol_struct *pa_get_label PARAMS ((void));
static sd_chain_struct *create_new_space PARAMS ((char *, int, char,
						  char, char, char,
						  asection *, int));
static ssd_chain_struct * create_new_subspace PARAMS ((sd_chain_struct *,
						       char *, char, char,
						       char, char, char,
						       char, int, int, int,
						       int, asection *));
static ssd_chain_struct *update_subspace PARAMS ((sd_chain_struct *,
						  char *, char, char, char,
						  char, char, char, int,
						  int, int, int, subsegT));
static sd_chain_struct *is_defined_space PARAMS ((char *));
static ssd_chain_struct *is_defined_subspace PARAMS ((char *));
static sd_chain_struct *pa_segment_to_space PARAMS ((asection *));
static ssd_chain_struct * pa_subsegment_to_subspace PARAMS ((asection *,
							     subsegT));
static sd_chain_struct *pa_find_space_by_number PARAMS ((int));
static unsigned int pa_subspace_start PARAMS ((sd_chain_struct *, int));
static void pa_ip PARAMS ((char *));
static void fix_new_hppa PARAMS ((fragS *, int, short int, symbolS *,
				  long, expressionS *, int,
				  bfd_reloc_code_real_type, long,
				  int, long, char *));
static void md_apply_fix_1 PARAMS ((fixS *, long));
static int is_end_of_statement PARAMS ((void));
static int reg_name_search PARAMS ((char *));
static int pa_chk_field_selector PARAMS ((char **));
static int is_same_frag PARAMS ((fragS *, fragS *));
static void pa_build_unwind_subspace PARAMS ((struct call_info *));
static void process_exit PARAMS ((void));
static sd_chain_struct *pa_parse_space_stmt PARAMS ((char *, int));
static void pa_align_subseg PARAMS ((asection *, subsegT));
static int log2 PARAMS ((int));
static int pa_next_subseg PARAMS ((sd_chain_struct *));
static unsigned int pa_stringer_aux PARAMS ((char *));
static void pa_spaces_begin PARAMS ((void));
     

/* File and gloally scoped variable declarations.  */

/* Root and final entry in the space chain.  */
static sd_chain_struct *space_dict_root;
static sd_chain_struct *space_dict_last;

/* The current space and subspace.  */
static sd_chain_struct *current_space;
static ssd_chain_struct *current_subspace;

/* Root of the call_info chain.  */
static struct call_info *call_info_root;

/* The last call_info (for functions) structure
   seen so it can be associated with fixups and
   function labels.  */
static struct call_info *last_call_info;

/* The last call description (for actual calls).  */ 
static struct call_desc last_call_desc;

/* Relaxation isn't supported for the PA yet.  */
const relax_typeS md_relax_table[] = {0};

/* Jumps are always the same size -- one instruction.  */ 
int md_short_jump_size = 4;
int md_long_jump_size = 4;

/* handle of the OPCODE hash table */
static struct hash_control *op_hash = NULL;

/* This array holds the chars that always start a comment.  If the
   pre-processor is disabled, these aren't very useful.  */
const char comment_chars[] = ";";

/* Table of pseudo ops for the PA.  FIXME -- how many of these
   are now redundant with the overall GAS and the object file
   dependent tables?  */
const pseudo_typeS md_pseudo_table[] =
{
  /* align pseudo-ops on the PA specify the actual alignment requested,
     not the log2 of the requested alignment.  */
  {"align", s_align_bytes, 8},
  {"ALIGN", s_align_bytes, 8},
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
  {"octa", pa_cons, 16},
  {"OCTA", pa_cons, 16},
  {"org", pa_origin, 0},
  {"ORG", pa_origin, 0},
  {"origin", pa_origin, 0},
  {"ORIGIN", pa_origin, 0},
  {"param", pa_param, 0},
  {"PARAM", pa_param, 0},
  {"proc", pa_proc, 0},
  {"PROC", pa_proc, 0},
  {"procend", pa_procend, 0},
  {"PROCEND", pa_procend, 0},
  {"quad", pa_cons, 8},
  {"QUAD", pa_cons, 8},
  {"reg", pa_equ, 1},
  {"REG", pa_equ, 1},
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
   .line and .file directives will appear in the pre-processed output.

   Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output.

   Also note that '/*' will always start a comment.  */
const char line_comment_chars[] = "#";

/* This array holds the characters which act as line separators.  */
const char line_separator_chars[] = "!";

/* Chars that can be used to separate mant from exp in floating point nums.  */
const char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant.
   As in 0f12.456 or 0d1.2345e12. 

   Be aware that MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT may have to be 
   changed in read.c.  Ideally it shouldn't hae to know abou it at
   all, but nothing is ideal around here.  */
const char FLT_CHARS[] = "rRsSfFdDxXpP";

static struct pa_it the_insn;

/* Points to the end of an expression just parsed by get_expressoin
   and friends.  FIXME.  This shouldn't be handled with a file-global
   variable.  */
static char *expr_end;

/* Nonzero if a .callinfo appeared within the current procedure.  */
static int callinfo_found;

/* Nonzero if the assembler is currently within a .entry/.exit pair.  */
static int within_entry_exit;

/* Nonzero if the assembler has completed exit processing for the
   current procedure.  */
static int exit_processing_complete;

/* Nonzero if the assembler is currently within a procedure definition.  */
static int within_procedure;

/* Handle on strucutre which keep track of the last symbol
   seen in each subspace.  */
static label_symbol_struct *label_symbols_rootp = NULL;

/* Holds the last field selector.  */
static int hppa_field_selector;

/* Nonzero if errors are to be printed.  */
static int print_errors = 1;

/* List of registers that are pre-defined:

   Each general register has one predefined name of the form
   %r<REGNUM> which has the value <REGNUM>.  

   Space and control registers are handled in a similar manner,
   but use %sr<REGNUM> and %cr<REGNUM> as their predefined names. 

   Likewise for the floating point registers, but of the form
   %fr<REGNUM>.  Floating point registers have additional predefined
   names with 'L' and 'R' suffixes (e.g. %fr19L, %fr19R) which
   again have the value <REGNUM>.

   Many registers also have synonyms:

   %r26 - %r23 have %arg0 - %arg3 as synonyms
   %r28 - %r29 have %ret0 - %ret1 as synonyms
   %r30 has %sp as a synonym

   Almost every control register has a synonym; they are not listed
   here for brevity.  

   The table is sorted. Suitable for searching by a binary search. */

static const struct pd_reg pre_defined_registers[] =
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

/* This table is sorted by order of the length of the string. This is
   so we check for <> before we check for <. If we had a <> and checked 
   for < first, we would get a false match.  */
static const struct fp_cond_map fp_cond_map [] =
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

static const struct selector_entry selector_table[] =
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

/* default space and subspace dictionaries */

#define GDB_SYMBOLS          GDB_SYMBOLS_SUBSPACE_NAME
#define GDB_STRINGS          GDB_STRINGS_SUBSPACE_NAME

/* pre-defined subsegments (subspaces) for the HPPA.  */
#define SUBSEG_CODE   0
#define SUBSEG_DATA   0
#define SUBSEG_LIT    1
#define SUBSEG_BSS    2
#define SUBSEG_UNWIND 3
#define SUBSEG_GDB_STRINGS 0
#define SUBSEG_GDB_SYMBOLS 1

static struct default_subspace_dict pa_def_subspaces[] =
{
  {"$CODE$", 1, 1, 1, 0, 0, 0, 24, 0x2c, 0, 8, 0, 0, ".text", SUBSEG_CODE},
  {"$DATA$", 1, 1, 0, 0, 0, 0, 24, 0x1f, 1, 8, 1, 1, ".data", SUBSEG_DATA},
  {"$LIT$", 1, 1, 0, 0, 0, 0, 16, 0x2c, 0, 8, 0, 0, ".text", SUBSEG_LIT},
  {"$BSS$", 1, 1, 0, 0, 0, 1, 80, 0x1f, 1, 8, 1, 1, ".bss", SUBSEG_BSS},
  {"$UNWIND$", 1, 1, 0, 0, 0, 0, 64, 0x2c, 0, 4, 0, 0, ".hppa_unwind", SUBSEG_UNWIND},
  {NULL, 0, 1, 0, 0, 0, 0, 255, 0x1f, 0, 4, 0, 0, 0}
};

static struct default_space_dict pa_def_spaces[] =
{
  {"$TEXT$", 0, 1, 1, 0, 8, ASEC_NULL, ".text"},
  {"$PRIVATE$", 1, 1, 1, 1, 16, ASEC_NULL, ".data"},
  {NULL, 0, 0, 0, 0, 0, ASEC_NULL, NULL}
};

/* Misc local definitions used by the assembler.  */

/* Return nonzero if the string pointed to by S potentially represents
   a right or left half of a FP register  */
#define IS_R_SELECT(S)   (*(S) == 'R' || *(S) == 'r')
#define IS_L_SELECT(S)   (*(S) == 'L' || *(S) == 'l')

/* These macros are used to maintain spaces/subspaces.  */
#define SPACE_DEFINED(space_chain)	(space_chain)->sd_defined
#define SPACE_USER_DEFINED(space_chain) (space_chain)->sd_user_defined
#define SPACE_PRIVATE(space_chain)	(space_chain)->sd_private
#define SPACE_LOADABLE(space_chain)	(space_chain)->sd_loadable
#define SPACE_SPNUM(space_chain)	(space_chain)->sd_spnum
#define SPACE_SORT(space_chain)		(space_chain)->sd_sort_key
#define SPACE_NAME(space_chain)		(space_chain)->sd_name
#define SPACE_NAME_INDEX(space_chain)   (space_chain)->sd_name_index

#define SUBSPACE_SPACE_INDEX(ss_chain)  (ss_chain)->ssd_space_index
#define SUBSPACE_DEFINED(ss_chain)	(ss_chain)->ssd_defined
#define SUBSPACE_QUADRANT(ss_chain)	(ss_chain)->ssd_quadrant
#define SUBSPACE_ALIGN(ss_chain)	(ss_chain)->ssd_alignment
#define SUBSPACE_ACCESS(ss_chain)	(ss_chain)->ssd_access_control_bits
#define SUBSPACE_SORT(ss_chain)		(ss_chain)->ssd_sort_key
#define SUBSPACE_COMMON(ss_chain)	(ss_chain)->ssd_common
#define SUBSPACE_ZERO(ss_chain)		(ss_chain)->ssd_zero
#define SUBSPACE_DUP_COMM(ss_chain)	(ss_chain)->ssd_dup_common
#define SUBSPACE_CODE_ONLY(ss_chain)    (ss_chain)->ssd_code_only
#define SUBSPACE_LOADABLE(ss_chain)	(ss_chain)->ssd_loadable
#define SUBSPACE_SUBSPACE_START(ss_chain) (ss_chain)->ssd_subspace_start
#define SUBSPACE_SUBSPACE_LENGTH(ss_chain) (ss_chain)->ssd_subspace_length
#define SUBSPACE_NAME(ss_chain)		(ss_chain)->ssd_name

#define is_DP_relative(exp)			\
  ((exp).X_op == O_subtract			\
   && strcmp((exp).X_op_symbol->bsym->name, "$global$") == 0)

#define is_PC_relative(exp)			\
  ((exp).X_op == O_subtract			\
   && strcmp((exp).X_op_symbol->bsym->name, "$PIC_pcrel$0") == 0)

#define is_complex(exp)				\
  ((exp).X_op != O_constant && (exp).X_op != O_symbol)

/* Actual functions to implement the PA specific code for the assembler.  */

/* Returns a pointer to the label_symbol_struct for the current space.
   or NULL if no label_symbol_struct exists for the current space.  */

static label_symbol_struct *
pa_get_label ()
{
  label_symbol_struct *label_chain;
  sd_chain_struct *space_chain = current_space;

  for (label_chain = label_symbols_rootp;
       label_chain;
       label_chain = label_chain->lss_next)
    if (space_chain == label_chain->lss_space && label_chain->lss_label)
      return label_chain;

  return NULL;
}

/* Defines a label for the current space.  If one is already defined,
   this function will replace it with the new label.  */

void
pa_define_label (symbol)
     symbolS *symbol;
{
  label_symbol_struct *label_chain = pa_get_label ();
  sd_chain_struct *space_chain = current_space;

  if (label_chain)
    label_chain->lss_label = symbol;
  else
    {
      /* Create a new label entry and add it to the head of the chain.  */
      label_chain
	= (label_symbol_struct *) xmalloc (sizeof (label_symbol_struct));
      label_chain->lss_label = symbol;
      label_chain->lss_space = space_chain;
      label_chain->lss_next = NULL;

      if (label_symbols_rootp)
	label_chain->lss_next = label_symbols_rootp;

      label_symbols_rootp = label_chain;
    }
}

/* Removes a label definition for the current space.
   If there is no label_symbol_struct entry, then no action is taken.  */

static void
pa_undefine_label ()
{
  label_symbol_struct *label_chain;
  label_symbol_struct *prev_label_chain = NULL;
  sd_chain_struct *space_chain = current_space;

  for (label_chain = label_symbols_rootp;
       label_chain;
       label_chain = label_chain->lss_next)
    {
      if (space_chain == label_chain->lss_space && label_chain->lss_label)
	{
	  /* Remove the label from the chain and free its memory.  */
	  if (prev_label_chain)
	    prev_label_chain->lss_next = label_chain->lss_next;
	  else
	    label_symbols_rootp = label_chain->lss_next;

	  free (label_chain);
	  break;
	}
      prev_label_chain = label_chain;
    }
}


/* An HPPA-specific version of fix_new.  This is required because the HPPA
   code needs to keep track of some extra stuff.  Each call to fix_new_hppa
   results in the creation of an instance of an hppa_fix_struct.  An
   hppa_fix_struct stores the extra information along with a pointer to the
   original fixS.  This is attached to the original fixup via the 
   tc_fix_data field.  */

static void
fix_new_hppa (frag, where, size, add_symbol, offset, exp, pcrel,
	      r_type, r_field, r_format, arg_reloc, unwind_desc)
     fragS *frag;
     int where;
     short int size;
     symbolS *add_symbol;
     long offset;
     expressionS *exp;
     int pcrel;
     bfd_reloc_code_real_type r_type;
     long r_field;
     int r_format;
     long arg_reloc;
     char *unwind_desc;
{
  fixS *new_fix;

  struct hppa_fix_struct *hppa_fix = (struct hppa_fix_struct *)
    obstack_alloc (&notes, sizeof (struct hppa_fix_struct));

  if (exp != NULL)
    new_fix = fix_new_exp (frag, where, size, exp, pcrel, r_type);
  else
    new_fix = fix_new (frag, where, size, add_symbol, offset, pcrel, r_type);
  new_fix->tc_fix_data = hppa_fix;
  hppa_fix->fx_r_type = r_type;
  hppa_fix->fx_r_field = r_field;
  hppa_fix->fx_r_format = r_format;
  hppa_fix->fx_arg_reloc = arg_reloc;
  if (unwind_desc)
    bcopy (unwind_desc, hppa_fix->fx_unwind, 8);

}

/* Parse a .byte, .word, .long expression for the HPPA.  Called by
   cons via the TC_PARSE_CONS_EXPRESSION macro.  */

void
parse_cons_expression_hppa (exp)
     expressionS *exp;
{
  hppa_field_selector = pa_chk_field_selector (&input_line_pointer);
  expression (exp);
}

/* This fix_new is called by cons via TC_CONS_FIX_NEW.
   hppa_field_selector is set by the parse_cons_expression_hppa.  */

void
cons_fix_new_hppa (frag, where, size, exp)
     fragS *frag;
     int where;
     int size;
     expressionS *exp;
{
  unsigned int reloc_type;

  if (is_DP_relative (*exp))
    reloc_type = R_HPPA_GOTOFF;
  else if (is_complex (*exp))
    reloc_type = R_HPPA_COMPLEX;
  else
    reloc_type = R_HPPA;

  if (hppa_field_selector != e_psel && hppa_field_selector != e_fsel)
    as_warn ("Invalid field selector.  Assuming F%%.");

  fix_new_hppa (frag, where, size,
		(symbolS *) NULL, (offsetT) 0, exp, 0, reloc_type,
		hppa_field_selector, 32, 0, (char *) 0);
}

/* This function is called once, at assembler startup time.  It should
   set up all the tables, etc. that the MD part of the assembler will need.  */

void
md_begin ()
{
  char *retval = NULL;
  int lose = 0;
  unsigned int i = 0;

  last_call_info = NULL;
  call_info_root = NULL;

  /* Folding of text and data segments fails miserably on the PA.
     Warn user and disable "-R" option.  */
  if (flagseen['R'])
    {
      as_warn ("-R option not supported on this target.");
      flag_readonly_data_in_text = 0;
      flagseen['R'] = 0;
    }

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
	  as_fatal ("Internal error: can't hash `%s': %s\n", name, retval);
	  lose = 1;
	}
      do
	{
	  if ((pa_opcodes[i].match & pa_opcodes[i].mask) 
	      != pa_opcodes[i].match)
	    {
	      fprintf (stderr, "internal error: losing opcode: `%s' \"%s\"\n",
		       pa_opcodes[i].name, pa_opcodes[i].args);
	      lose = 1;
	    }
	  ++i;
	}
      while (i < NUMOPCODES && !strcmp (pa_opcodes[i].name, name));
    }

  if (lose)
    as_fatal ("Broken assembler.  No assembly attempted.");

  /* SOM will change text_section.  To make sure we never put
     anything into the old one switch to the new one now.  */
  subseg_set (text_section, 0);
}

/* Called at the end of assembling a source file.  Nothing to do
   at this point on the PA.  */

void
md_end ()
{
  return;
}

/* Assemble a single instruction storing it into a frag.  */
void
md_assemble (str)
     char *str;
{
  char *to;

  /* The had better be something to assemble.  */
  assert (str);

  /* Assemble the instruction.  Results are saved into "the_insn".  */
  pa_ip (str);

  /* Get somewhere to put the assembled instrution.  */
  to = frag_more (4);

  /* Output the opcode. */
  md_number_to_chars (to, the_insn.opcode, 4);

  /* If necessary output more stuff.  */
  if (the_insn.reloc != R_HPPA_NONE)
    fix_new_hppa (frag_now, (to - frag_now->fr_literal), 4, NULL,
		  (offsetT) 0, &the_insn.exp, the_insn.pcrel,
		  the_insn.reloc, the_insn.field_selector,
		  the_insn.format, the_insn.arg_reloc, NULL);

}

/* Do the real work for assembling a single instruction.  Store results
   into the global "the_insn" variable.

   FIXME:  Should define and use some functions/macros to handle
   various common insertions of information into the opcode.  */

static void
pa_ip (str)
     char *str;
{
  char *error_message = "";
  char *s, c, *argstart, *name, *save_s;
  const char *args;
  int match = FALSE;
  int comma = 0;
  int reg, s2, s3, m, a, uu, cmpltr, nullif, flag, sfu, cond;
  unsigned int im21, im14, im11, im5;
  unsigned long i, opcode;
  struct pa_opcode *insn;

  /* Skip to something interesting.  */
  for (s = str; isupper (*s) || islower (*s) || (*s >= '0' && *s <= '3'); ++s)
    ;

  switch (*s)
    {

    case '\0':
      break;

    case ',':
      comma = 1;

      /*FALLTHROUGH */

    case ' ':
      *s++ = '\0';
      break;

    default:
      as_bad ("Unknown opcode: `%s'", str);
      exit (1);
    }

  save_s = str;

  /* Convert everything into lower case.  */
  while (*save_s)
    {
      if (isupper (*save_s))
	*save_s = tolower (*save_s);
      save_s++;
    }

  /* Look up the opcode in the has table.  */
  if ((insn = (struct pa_opcode *) hash_find (op_hash, str)) == NULL)
    {
      as_bad ("Unknown opcode: `%s'", str);
      return;
    }

  if (comma)
    {
      *--s = ',';
    }

  /* Mark the location where arguments for the instruction start, then
     start processing them.  */
  argstart = s;
  for (;;)
    {
      /* Do some initialization.  */
      opcode = insn->match;
      bzero (&the_insn, sizeof (the_insn));

      the_insn.reloc = R_HPPA_NONE;

      /* Build the opcode, checking as we go to make
         sure that the operands match.  */
      for (args = insn->args;; ++args)
	{
	  switch (*args)
	    {

	    /* End of arguments.  */
	    case '\0':
	      if (*s == '\0')
		match = TRUE;
	      break;

	    case '+':
	      if (*s == '+')
		{
		  ++s;
		  continue;
		}
	      if (*s == '-')
		continue;
	      break;

	    /* These must match exactly.  */
	    case '(':
	    case ')':
	    case ',':
	    case ' ':
	      if (*s++ == *args)
		continue;
	      break;

	    /* Handle a 5 bit register or control register field at 10.  */
	    case 'b':
	    case '^':
	      reg = pa_parse_number (&s, 0);
	      if (reg < 32 && reg >= 0)
		{
		  opcode |= reg << 21;
		  continue;
		}
	      break;

	    /* Handle a 5 bit register field at 15.  */
	    case 'x':
	      reg = pa_parse_number (&s, 0);
	      if (reg < 32 && reg >= 0)
		{
		  opcode |= reg << 16;
		  continue;
		}
	      break;

	    /* Handle a 5 bit register field at 31.  */
	    case 'y':
	    case 't':
	      reg = pa_parse_number (&s, 0);
	      if (reg < 32 && reg >= 0)
		{
		  opcode |= reg;
		  continue;
		}
	      break;

	    /* Handle a 5 bit field length at 31.  */
	    case 'T':
	      pa_get_absolute_expression (s);
	      if (the_insn.exp.X_op == O_constant)
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

	    /* Handle a 5 bit immediate at 15.  */
	    case '5':
	      pa_get_absolute_expression (s);
	      if (the_insn.exp.X_add_number > 15)
		{
		  as_bad ("5 bit immediate > 15. Set to 15");
		  the_insn.exp.X_add_number = 15;
		}
	      else if (the_insn.exp.X_add_number < -16)
		{
		  as_bad ("5 bit immediate < -16. Set to -16");
		  the_insn.exp.X_add_number = -16;
		}

	      low_sign_unext (evaluate_absolute (the_insn.exp,
						 the_insn.field_selector),
			      5, &im5);
	      opcode |= (im5 << 16);
	      s = expr_end;
	      continue;

	    /* Handle a 2 bit space identifier at 17.  */
	    case 's':
	      s2 = pa_parse_number (&s, 0);
	      if (s2 < 4 && s2 >= 0)
		{
		  opcode |= s2 << 14;
		  continue;
		}
	      break;

	    /* Handle a 3 bit space identifier at 18.  */
	    case 'S':
	      s3 = pa_parse_number (&s, 0);
	      if (s3 < 8 && s3 >= 0)
		{
		  dis_assemble_3 (s3, &s3);
		  opcode |= s3 << 13;
		  continue;
		}
	      break;

	    /* Handle a completer for an indexing load or store.  */
	    case 'c':
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
		    as_bad ("Invalid Indexed Load Completer.");
		  s++;
		  i++;
		}
	      if (i > 2)
		as_bad ("Invalid Indexed Load Completer Syntax.");
	      while (*s == ' ' || *s == '\t')
		s++;

	      opcode |= m << 5;
	      opcode |= uu << 13;
	      continue;

	    /* Handle a short load/store completer.  */
	    case 'C':
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
		    as_bad ("Invalid Short Load/Store Completer.");
		  s += 2;
		}
	      while (*s == ' ' || *s == '\t')
		s++;
	      opcode |= m << 5;
	      opcode |= a << 13;
	      continue;

	    /* Handle a stbys completer.  */
	    case 'Y':
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
		    as_bad ("Invalid Store Bytes Short Completer");
		  s++;
		  i++;
		}
	      if (i > 2)
		as_bad ("Invalid Store Bytes Short Completer");
	      while (*s == ' ' || *s == '\t')
		s++;
	      opcode |= m << 5;
	      opcode |= a << 13;
	      continue;

	    /* Handle a non-negated compare/stubtract condition.  */
	    case '<':
	      cmpltr = pa_parse_nonneg_cmpsub_cmpltr (&s, 1);
	      if (cmpltr < 0)
		{
		  as_bad ("Invalid Compare/Subtract Condition: %c", *s);
		  cmpltr = 0;
		}
	      opcode |= cmpltr << 13;
	      continue;

	    /* Handle a negated or non-negated compare/subtract condition.  */
	    case '?':
	      save_s = s;
	      cmpltr = pa_parse_nonneg_cmpsub_cmpltr (&s, 1);
	      if (cmpltr < 0)
		{
		  s = save_s;
		  cmpltr = pa_parse_neg_cmpsub_cmpltr (&s, 1);
		  if (cmpltr < 0)
		    {
		      as_bad ("Invalid Compare/Subtract Condition.");
		      cmpltr = 0;
		    }
		  else
		    {
		      /* Negated condition requires an opcode change.  */
		      opcode |= 1 << 27;
		    }
		}
	      opcode |= cmpltr << 13;
	      continue;

	    /* Handle a negated or non-negated add condition.  */
	    case '!':
	      save_s = s;
	      cmpltr = pa_parse_nonneg_add_cmpltr (&s, 1);
	      if (cmpltr < 0)
		{
		  s = save_s;
		  cmpltr = pa_parse_neg_add_cmpltr (&s, 1);
		  if (cmpltr < 0)
		    {
		      as_bad ("Invalid Compare/Subtract Condition");
		      cmpltr = 0;
		    }
		  else
		    {
		      /* Negated condition requires an opcode change.  */
		      opcode |= 1 << 27;
		    }
		}
	      opcode |= cmpltr << 13;
	      continue;

	    /* Handle a compare/subtract condition.  */
	    case 'a':
	      cmpltr = 0;
	      flag = 0;
	      save_s = s;
	      if (*s == ',')
		{
		  cmpltr = pa_parse_nonneg_cmpsub_cmpltr (&s, 0);
		  if (cmpltr < 0)
		    {
		      flag = 1;
		      s = save_s;
		      cmpltr = pa_parse_neg_cmpsub_cmpltr (&s, 0);
		      if (cmpltr < 0)
			{
			  as_bad ("Invalid Compare/Subtract Condition");
			}
		    }
		}
	      opcode |= cmpltr << 13;
	      opcode |= flag << 12;
	      continue;

	    /* Handle a non-negated add condition.  */
	    case 'd':
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
		    cmpltr = 1;
		  else if (strcmp (name, "<") == 0)
		    cmpltr = 2;
		  else if (strcmp (name, "<=") == 0)
		    cmpltr = 3;
		  else if (strcasecmp (name, "nuv") == 0)
		    cmpltr = 4;
		  else if (strcasecmp (name, "znv") == 0)
		    cmpltr = 5;
		  else if (strcasecmp (name, "sv") == 0)
		    cmpltr = 6;
		  else if (strcasecmp (name, "od") == 0)
		    cmpltr = 7;
		  else if (strcasecmp (name, "n") == 0)
		    nullif = 1;
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
		    as_bad ("Invalid Add Condition: %s", name);
		  *s = c;
		}
	      nullif = pa_parse_nullif (&s);
	      opcode |= nullif << 1;
	      opcode |= cmpltr << 13;
	      opcode |= flag << 12;
	      continue;

	    /* Handle a logical instruction condition.  */
	    case '&':
	      cmpltr = 0;
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
		    cmpltr = 1;
		  else if (strcmp (name, "<") == 0)
		    cmpltr = 2;
		  else if (strcmp (name, "<=") == 0)
		    cmpltr = 3;
		  else if (strcasecmp (name, "od") == 0)
		    cmpltr = 7;
		  else if (strcasecmp (name, "tr") == 0)
		    {
		      cmpltr = 0;
		      flag = 1;
		    }
		  else if (strcmp (name, "<>") == 0)
		    {
		      cmpltr = 1;
		      flag = 1;
		    }
		  else if (strcmp (name, ">=") == 0)
		    {
		      cmpltr = 2;
		      flag = 1;
		    }
		  else if (strcmp (name, ">") == 0)
		    {
		      cmpltr = 3;
		      flag = 1;
		    }
		  else if (strcasecmp (name, "ev") == 0)
		    {
		      cmpltr = 7;
		      flag = 1;
		    }
		  else
		    as_bad ("Invalid Logical Instruction Condition.");
		  *s = c;
		}
	      opcode |= cmpltr << 13;
	      opcode |= flag << 12;
	      continue;

	    /* Handle a unit instruction condition.  */
	    case 'U':
	      cmpltr = 0;
	      flag = 0;
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
		      flag = 1;
		      s += 2;
		    }
		  else if (strncasecmp (s, "nbz", 3) == 0)
		    {
		      cmpltr = 2;
		      flag = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "nhz", 3) == 0)
		    {
		      cmpltr = 3;
		      flag = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "ndc", 3) == 0)
		    {
		      cmpltr = 4;
		      flag = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "nbc", 3) == 0)
		    {
		      cmpltr = 6;
		      flag = 1;
		      s += 3;
		    }
		  else if (strncasecmp (s, "nhc", 3) == 0)
		    {
		      cmpltr = 7;
		      flag = 1;
		      s += 3;
		    }
		  else
		    as_bad ("Invalid Logical Instruction Condition.");
		}
	      opcode |= cmpltr << 13;
	      opcode |= flag << 12;
	      continue;

	    /* Handle a shift/extract/deposit condition.  */
	    case '|':
	    case '>':
	      cmpltr = 0;
	      if (*s == ',')
		{
		  save_s = s++;
		  name = s;
		  while (*s != ',' && *s != ' ' && *s != '\t')
		    s += 1;
		  c = *s;
		  *s = 0x00;
		  if (strcmp (name, "=") == 0)
		    cmpltr = 1;
		  else if (strcmp (name, "<") == 0)
		    cmpltr = 2;
		  else if (strcasecmp (name, "od") == 0)
		    cmpltr = 3;
		  else if (strcasecmp (name, "tr") == 0)
		    cmpltr = 4;
		  else if (strcmp (name, "<>") == 0)
		    cmpltr = 5;
		  else if (strcmp (name, ">=") == 0)
		    cmpltr = 6;
		  else if (strcasecmp (name, "ev") == 0)
		    cmpltr = 7;
		  /* Handle movb,n.  Put things back the way they were.  
		     This includes moving s back to where it started.  */
		  else if (strcasecmp (name, "n") == 0 && *args == '|')
		    {
		      *s = c;
		      s = save_s;
		      continue;
		    }
		  else
		    as_bad ("Invalid Shift/Extract/Deposit Condition.");
		  *s = c;
		}
	      opcode |= cmpltr << 13;
	      continue;

	    /* Handle bvb and bb conditions.  */
	    case '~':
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
		    as_bad ("Invalid Bit Branch Condition: %c", *s);
		}
	      opcode |= cmpltr << 13;
	      continue;

	    /* Handle a 5 bit immediate at 31.  */
	    case 'V':
	      get_expression (s);
	      low_sign_unext (evaluate_absolute (the_insn.exp,
						 the_insn.field_selector),
			      5, &im5);
	      opcode |= im5;
	      s = expr_end;
	      continue;

	    /* Handle an unsigned 5 bit immediate at 31.  */
	    case 'r':
	      get_expression (s);
	      im5 = evaluate_absolute (the_insn.exp, the_insn.field_selector);
	      if (im5 > 31)
		{
		  as_bad ("Operand out of range. Was: %d. Should be [0..31].",
			  im5);
		  im5 = im5 & 0x1f;
		}
	      opcode |= im5;
	      s = expr_end;
	      continue;

	    /* Handle an unsigned 5 bit immediate at 15.  */
	    case 'R':
	      get_expression (s);
	      im5 = evaluate_absolute (the_insn.exp, the_insn.field_selector);
	      if (im5 > 31)
		{
		  as_bad ("Operand out of range. Was: %d. Should be [0..31].",
			  im5);
		  im5 = im5 & 0x1f;
		}
	      opcode |= im5 << 16;
	      s = expr_end;
	      continue;

	    /* Handle a 11 bit immediate at 31.  */
	    case 'i':
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      get_expression (s);
	      if (the_insn.exp.X_op == O_constant)
		{
		  low_sign_unext (evaluate_absolute (the_insn.exp,
						     the_insn.field_selector),
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

	    /* Handle a 14 bit immediate at 31.  */
	    case 'j':
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      get_expression (s);
	      if (the_insn.exp.X_op == O_constant)
		{
		  low_sign_unext (evaluate_absolute (the_insn.exp,
						     the_insn.field_selector),
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

	    /* Handle a 21 bit immediate at 31.  */
	    case 'k':
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      get_expression (s);
	      if (the_insn.exp.X_op == O_constant)
		{
		  dis_assemble_21 (evaluate_absolute (the_insn.exp,
						      the_insn.field_selector),
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

	    /* Handle a nullification completer for branch instructions.  */
	    case 'n':
	      nullif = pa_parse_nullif (&s);
	      opcode |= nullif << 1;
	      continue;

	    /* Handle a 12 bit branch displacement.  */
	    case 'w':
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      get_expression (s);
	      the_insn.pcrel = 1;
	      if (!strcmp (S_GET_NAME (the_insn.exp.X_add_symbol), "L0\001"))
		{
		  unsigned int w1, w, result;

		  sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 12,
			      &result);
		  dis_assemble_12 (result, &w1, &w);
		  opcode |= ((w1 << 2) | w);
		}
	      else
		{
		  if (is_complex (the_insn.exp))
		    the_insn.reloc = R_HPPA_COMPLEX_PCREL_CALL;
		  else
		    the_insn.reloc = R_HPPA_PCREL_CALL;
		  the_insn.format = 12;
		  the_insn.arg_reloc = last_call_desc.arg_reloc;
		  bzero (&last_call_desc, sizeof (struct call_desc));
		}
	      s = expr_end;
	      continue;

	    /* Handle a 17 bit branch displacement.  */
	    case 'W':
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      get_expression (s);
	      the_insn.pcrel = 1;
	      if (the_insn.exp.X_add_symbol)
		{
		  if (!strcmp (S_GET_NAME (the_insn.exp.X_add_symbol),
			       "L0\001"))
		    {
		      unsigned int w2, w1, w, result;

		      sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 17,
				  &result);
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
		      bzero (&last_call_desc, sizeof (struct call_desc));
		    }
		}
	      else
		{
		  unsigned int w2, w1, w, result;

		  sign_unext (the_insn.exp.X_add_number >> 2, 17, &result);
		  dis_assemble_17 (result, &w1, &w2, &w);
		  opcode |= ((w2 << 2) | (w1 << 16) | w);
		}
	      s = expr_end;
	      continue;

	    /* Handle an absolute 17 bit branch target.  */
	    case 'z':
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      get_expression (s);
	      the_insn.pcrel = 0;
	      if (the_insn.exp.X_add_symbol)
		{
		  if (!strcmp (S_GET_NAME (the_insn.exp.X_add_symbol),
			       "L0\001"))
		    {
		      unsigned int w2, w1, w, result;

		      sign_unext ((the_insn.exp.X_add_number - 8) >> 2, 17,
				  &result);
		      dis_assemble_17 (result, &w1, &w2, &w);
		      opcode |= ((w2 << 2) | (w1 << 16) | w);
		    }
		  else
		    {
		      if (is_complex (the_insn.exp))
			the_insn.reloc = R_HPPA_COMPLEX_ABS_CALL;
		      else
			the_insn.reloc = R_HPPA_ABS_CALL;
		      the_insn.format = 17;
		    }
		}
	      else
		{
		  unsigned int w2, w1, w, result;

		  sign_unext (the_insn.exp.X_add_number >> 2, 17, &result);
		  dis_assemble_17 (result, &w1, &w2, &w);
		  opcode |= ((w2 << 2) | (w1 << 16) | w);
		}
	      s = expr_end;
	      continue;

	    /* Handle a 5 bit shift count at 26.  */
	    case 'p':
	      get_expression (s);
	      if (the_insn.exp.X_op == O_constant)
		opcode |= (((31 - the_insn.exp.X_add_number) & 0x1f) << 5);
	      s = expr_end;
	      continue;

	    /* Handle a 5 bit bit position at 26.  */
	    case 'P':
	      get_expression (s);
	      if (the_insn.exp.X_op == O_constant)
		opcode |= (the_insn.exp.X_add_number & 0x1f) << 5;
	      s = expr_end;
	      continue;

	    /* Handle a 5 bit immediate at 10.  */
	    case 'Q':
	      get_expression (s);
	      im5 = evaluate_absolute (the_insn.exp, the_insn.field_selector);
	      if (im5 > 31)
		{
		  as_bad ("Operand out of range. Was: %d. Should be [0..31].",
			  im5);
		  im5 = im5 & 0x1f;
		}
	      opcode |= im5 << 21;
	      s = expr_end;
	      continue;

	    /* Handle a 13 bit immediate at 18.  */
	    case 'A':
	      pa_get_absolute_expression (s);
	      if (the_insn.exp.X_op == O_constant)
		opcode |= (the_insn.exp.X_add_number & 0x1fff) << 13;
	      s = expr_end;
	      continue;

	    /* Handle a system control completer.  */
	    case 'Z':
	      if (*s == ',' && (*(s + 1) == 'm' || *(s + 1) == 'M'))
		{
		  m = 1;
		  s += 2;
		}
	      else
		m = 0;

	      opcode |= m << 5;
	      while (*s == ' ' || *s == '\t')
		s++;
	      continue;

	    /* Handle a 26 bit immediate at 31.  */
	    case 'D':
	      the_insn.field_selector = pa_chk_field_selector (&s);
	      get_expression (s);
	      if (the_insn.exp.X_op == O_constant)
		{
		  opcode |= ((evaluate_absolute (the_insn.exp,
						 the_insn.field_selector)
			      & 0x1ffffff) << 1);
		}
	      else
		as_bad ("Invalid DIAG operand");
	      s = expr_end;
	      continue;

	    /* Handle a 3 bit SFU identifier at 25.  */
	    case 'f':
	      sfu = pa_parse_number (&s, 0);
	      if ((sfu > 7) || (sfu < 0))
		as_bad ("Invalid SFU identifier: %02x", sfu);
	      opcode |= (sfu & 7) << 6;
	      continue;

	    /* We don't support any of these.  FIXME.  */
	    case 'O':
	      get_expression (s);
	      s = expr_end;
	      abort ();
	      continue;

	    /* Handle a source FP operand format completer.  */
	    case 'F':
	      flag = pa_parse_fp_format (&s);
	      opcode |= (int) flag << 11;
	      the_insn.fpof1 = flag;
	      continue;

	    /* Handle a destination FP operand format completer.  */
	    case 'G':

	      /* pa_parse_format needs the ',' prefix.  */
	      s--;
	      flag = pa_parse_fp_format (&s);
	      opcode |= (int) flag << 13;
	      the_insn.fpof2 = flag;
	      continue;

	    /* Handle FP compare conditions.  */
	    case 'M':
	      cond = pa_parse_fp_cmp_cond (&s);
	      opcode |= cond;
	      continue;

	    /* Handle L/R register halves like 't'.  */
	    case 'v':
	      {
		struct pa_89_fp_reg_struct result;

		pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    opcode |= (result.number_part & 0x1f);

		    /* 0x30 opcodes are FP arithmetic operation opcodes
		       and need to be turned into 0x38 opcodes.  This
		       is not necessary for loads/stores.  */
		    if (need_89_opcode (&the_insn, &result))
		      {
			if ((opcode & 0xfc000000) == 0x30000000)
			  {
			    opcode |= (result.l_r_select & 1) << 6;
			    opcode |= 1 << 27;
			  }
			else
			  {
			    opcode |= (result.l_r_select & 1) << 6;
			  }
		      }
		    continue;
		  }
	      }
	      break;

	    /* Handle L/R register halves like 'b'.  */
	    case 'E':
	      {
		struct pa_89_fp_reg_struct result;

		pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    opcode |= (result.number_part & 0x1f) << 21;
		    if (need_89_opcode (&the_insn, &result))
		      {
			opcode |= (result.l_r_select & 1) << 7;
			opcode |= 1 << 27;
		      }
		    continue;
		  }
	      }
	      break;

	    /* Handle L/R register halves like 'x'.  */
	    case 'X':
	      {
		struct pa_89_fp_reg_struct result;

		pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    opcode |= (result.number_part & 0x1f) << 16;
		    if (need_89_opcode (&the_insn, &result))
		      {
			opcode |= (result.l_r_select & 1) << 12;
			opcode |= 1 << 27;
		      }
		    continue;
		  }
	      }
	      break;

	    /* Handle a 5 bit register field at 10.  */
	    case '4':
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.l_r_select & 1) << 4;
		      }
		    opcode |= result.number_part << 21;
		    continue;
		  }
	      }
	      break;

	    /* Handle a 5 bit register field at 15.  */
	    case '6':
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.l_r_select & 1) << 4;
		      }
		    opcode |= result.number_part << 16;
		    continue;
		  }
	      }
	      break;

	    /* Handle a 5 bit register field at 31.  */
	    case '7':
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.l_r_select & 1) << 4;
		      }
		    opcode |= result.number_part;
		    continue;
		  }
	      }
	      break;

	    /* Handle a 5 bit register field at 20.  */
	    case '8':
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.l_r_select & 1) << 4;
		      }
		    opcode |= result.number_part << 11;
		    continue;
		  }
	      }
	      break;

	    /* Handle a 5 bit register field at 25.  */
	    case '9':
	      {
		struct pa_89_fp_reg_struct result;
		int status;

		status = pa_parse_number (&s, &result);
		if (result.number_part < 32 && result.number_part >= 0)
		  {
		    if (the_insn.fpof1 == SGL)
		      {
			result.number_part &= 0xF;
			result.number_part |= (result.l_r_select & 1) << 4;
		      }
		    opcode |= result.number_part << 6;
		    continue;
		  }
	      }
	      break;

	    /* Handle a floating point operand format at 26.
	       Only allows single and double precision.  */
	    case 'H':
	      flag = pa_parse_fp_format (&s);
	      switch (flag)
		{
		case SGL:
		  opcode |= 0x20;
		case DBL:
		  the_insn.fpof1 = flag;
		  continue;

		case QUAD:
		case ILLEGAL_FMT:
		default:
		  as_bad ("Invalid Floating Point Operand Format.");
		}
	      break;

	    default:
	      abort ();
	    }
	  break;
	}

      /* Check if the args matched.  */
      if (match == FALSE)
	{
	  if (&insn[1] - pa_opcodes < NUMOPCODES
	      && !strcmp (insn->name, insn[1].name))
	    {
	      ++insn;
	      s = argstart;
	      continue;
	    }
	  else
	    {
	      as_bad ("Invalid operands %s", error_message);
	      return;
	    }
	}
      break;
    }

  the_insn.opcode = opcode;
  return;
}

/* Turn a string in input_line_pointer into a floating point constant of type
   type, and store the appropriate bytes in *litP.  The number of LITTLENUMS
   emitted is stored in *sizeP .  An error message or NULL is returned.  */

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
      md_number_to_chars (litP, (valueT) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }
  return NULL;
}

/* Write out big-endian.  */

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

/* Translate internal representation of relocation info to BFD target
   format.  */

arelent **
tc_gen_reloc (section, fixp)
     asection *section;
     fixS *fixp;
{
  arelent *reloc;
  struct hppa_fix_struct *hppa_fixp = fixp->tc_fix_data;
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

#ifdef OBJ_ELF
  /* Yuk.  I would really like to push all this ELF specific unwind
     crud into BFD and the linker.  That's how SOM does it -- and
     if we could make ELF emulate that then we could share more code
     in GAS (and potentially a gnu-linker later).

     Unwind section relocations are handled in a special way.
     The relocations for the .unwind section are originally
     built in the usual way.  That is, for each unwind table
     entry there are two relocations:  one for the beginning of
     the function and one for the end.

     The first time we enter this function we create a
     relocation of the type R_HPPA_UNWIND_ENTRIES.  The addend
     of the relocation is initialized to 0.  Each additional
     pair of times this function is called for the unwind
     section represents an additional unwind table entry.  Thus,
     the addend of the relocation should end up to be the number
     of unwind table entries.  */
  if (strcmp (UNWIND_SECTION_NAME, section->name) == 0)
    {
      if (unwind_reloc_entryP == NULL)
	{
	  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, 
						   sizeof (arelent));
	  assert (reloc != 0);
	  unwind_reloc_entryP = reloc;
	  unwind_reloc_fixp_cnt++;
	  unwind_reloc_entryP->address
	    = fixp->fx_frag->fr_address + fixp->fx_where;
	  /* A pointer to any function will do.  We only
	     need one to tell us what section the unwind
	     relocations are for. */
	  unwind_reloc_entryP->sym_ptr_ptr = &fixp->fx_addsy->bsym;
	  hppa_fixp->fx_r_type = code = R_HPPA_UNWIND_ENTRIES;
	  fixp->fx_r_type = R_HPPA_UNWIND;
	  unwind_reloc_entryP->howto = bfd_reloc_type_lookup (stdoutput, code);
	  unwind_reloc_entryP->addend = unwind_reloc_fixp_cnt / 2;
	  relocs = (arelent **) bfd_alloc_by_size_t (stdoutput,
						     sizeof (arelent *) * 2);
	  assert (relocs != 0);
	  relocs[0] = unwind_reloc_entryP;
	  relocs[1] = NULL;
	  return relocs;
	}
      unwind_reloc_fixp_cnt++;
      unwind_reloc_entryP->addend = unwind_reloc_fixp_cnt / 2;

      return &no_relocs;
    }
#endif

  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput, sizeof (arelent));
  assert (reloc != 0);

  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  codes = hppa_gen_reloc_type (stdoutput,
			       fixp->fx_r_type,
			       hppa_fixp->fx_r_format,
			       hppa_fixp->fx_r_field);

  for (n_relocs = 0; codes[n_relocs]; n_relocs++)
    ;

  relocs = (arelent **)
    bfd_alloc_by_size_t (stdoutput, sizeof (arelent *) * n_relocs + 1);
  assert (relocs != 0);

  reloc = (arelent *) bfd_alloc_by_size_t (stdoutput,
					   sizeof (arelent) * n_relocs);
  if (n_relocs > 0)
    assert (reloc != 0);

  for (i = 0; i < n_relocs; i++)
    relocs[i] = &reloc[i];

  relocs[n_relocs] = NULL;

#ifdef OBJ_ELF
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

      /* Now, do any processing that is dependent on the relocation type.  */
      switch (code)
	{
	case R_HPPA_PLABEL_32:
	case R_HPPA_PLABEL_11:
	case R_HPPA_PLABEL_14:
	case R_HPPA_PLABEL_L21:
	case R_HPPA_PLABEL_R11:
	case R_HPPA_PLABEL_R14:
	  /* For plabel relocations, the addend of the
	     relocation should be either 0 (no static link) or 2
	     (static link required).

	     FIXME: assume that fx_addnumber contains this
	     information */
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
	  /* The constant is stored in the instruction.  */
	  reloc->addend = HPPA_R_ADDEND (hppa_fixp->fx_arg_reloc, 0);
	  break;
	default:
	  reloc->addend = fixp->fx_addnumber;
	  break;
	}
      break;
    }
#else /* OBJ_SOM */

  /* Preliminary relocation handling for SOM.  Needs to handle
     COMPLEX relocations (yes, I've seen them occur) and it will
     need to handle R_ENTRY/R_EXIT relocations in the very near future
     (for generating unwinds).  */
  switch (fixp->fx_r_type)
    {
    case R_HPPA_COMPLEX:
    case R_HPPA_COMPLEX_PCREL_CALL:
    case R_HPPA_COMPLEX_ABS_CALL:
      abort();
      break;
    default:
      assert (n_relocs == 1);
      
      code = *codes[0];
      
      reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
      reloc->howto = bfd_reloc_type_lookup (stdoutput, code);
      reloc->address = fixp->fx_frag->fr_address + fixp->fx_where ;
      reloc->addend = 0;

      switch (code)
	{
	case R_PCREL_CALL:
	case R_ABS_CALL:
	  reloc->addend = HPPA_R_ADDEND (hppa_fixp->fx_arg_reloc, 0);
	  break;
	default:
	  reloc->addend = fixp->fx_addnumber;
	  break;
	}
      break;
    }
#endif

  return relocs;
}

/* Process any machine dependent frag types.  */

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
  int align = bfd_get_section_alignment (stdoutput, segment);
  int align2 = (1 << align) - 1;

  return (size + align2) & ~align2;

}

/* Create a short jump from FROM_ADDR to TO_ADDR.  Not used on the PA.  */
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

/* Create a long jump from FROM_ADDR to TO_ADDR.  Not used on the PA.  */
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

/* Return the approximate size of a frag before relaxation has occurred.  */
int
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

/* Parse machine dependent options.  There are none on the PA.  */ 
int
md_parse_option (argP, cntP, vecP)
     char **argP;
     int *cntP;
     char ***vecP;
{
  return 1;
}

/* We have no need to default values of symbols.  */

symbolS *
md_undefined_symbol (name)
     char *name;
{
  return 0;
}

/* Parse an operand that is machine-specific.
   We just return without modifying the expression as we have nothing
   to do on the PA.  */

void
md_operand (expressionP)
     expressionS *expressionP;
{
}

/* Helper function for md_apply_fix.  Actually determine if the fix
   can be applied, and if so, apply it.

   If a fix is applied, then set fx_addsy to NULL which indicates
   the fix was applied and need not be emitted into the object file.  */

static void
md_apply_fix_1 (fixP, val)
     fixS *fixP;
     long val;
{
  char *buf = fixP->fx_where + fixP->fx_frag->fr_literal;
  struct hppa_fix_struct *hppa_fixP = fixP->tc_fix_data;
  long new_val, result;
  unsigned int w1, w2, w;

  /* There should have been an HPPA specific fixup associated
     with the GAS fixup.  */
  if (hppa_fixP)
    {
      unsigned long buf_wd = bfd_get_32 (stdoutput, buf);
      unsigned char fmt = bfd_hppa_insn2fmt (buf_wd);

      /* Sanity check the fixup type.  */
      /* Is this really necessary?  */
      if (fixP->fx_r_type == R_HPPA_NONE)
	fmt = 0;

      /* Remember this value for emit_reloc.  FIXME, is this braindamage
	 documented anywhere!?!  */
      fixP->fx_addnumber = val;

      /* Check if this is an undefined symbol.  No relocation can
         possibly be performed in this case.  */
      if ((fixP->fx_addsy && fixP->fx_addsy->bsym->section == &bfd_und_section)
	  || (fixP->fx_subsy
	      && fixP->fx_subsy->bsym->section == &bfd_und_section))
	return;

      switch (fmt)
	{
	/* Handle all opcodes with the 'j' operand type.  */
	case 14:
	  new_val = hppa_field_adjust (val, 0, hppa_fixP->fx_r_field);

	  /* Mask off 14 bits to be changed.  */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffffc000,
		      buf);
	  low_sign_unext (new_val, 14, &result);
	  break;

	/* Handle all opcodes with the 'k' operand type.  */
	case 21:
	  new_val = hppa_field_adjust (val, 0, hppa_fixP->fx_r_field);

	  /* Mask off 21 bits to be changed.  */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffe00000,
		      buf);
	  dis_assemble_21 (new_val, &result);
	  break;

	/* Handle all the opcodes with the 'i' operand type.  */
	case 11:
	  new_val = hppa_field_adjust (val, 0, hppa_fixP->fx_r_field);

	  /* Mask off 11 bits to be changed.  */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffff800,
		      buf);
	  low_sign_unext (new_val, 11, &result);
	  break;

	/* Handle all the opcodes with the 'w' operand type.  */
	case 12:
	  new_val = hppa_field_adjust (val, 0, hppa_fixP->fx_r_field);

	  /* Mask off 11 bits to be changed.  */
	  sign_unext ((new_val - 8) >> 2, 12, &result);
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffffe002,
		      buf);

	  dis_assemble_12 (result, &w1, &w);
	  result = ((w1 << 2) | w);
	  fixP->fx_addsy = NULL;
	  break;

#define too_far(VAL, NUM_BITS) \
  (((int)(VAL) > (1 << (NUM_BITS)) - 1) || ((int)(VAL) < (-1 << (NUM_BITS))))

#define stub_needed(CALLER, CALLEE) \
  ((CALLEE) && (CALLER) && ((CALLEE) != (CALLER)))

	/* Handle some of the opcodes with the 'W' operand type.  */
	case 17:
    	  /* If a long-call stub or argument relocation stub is
	     needed, then we can not apply this relocation, instead
	     the linker must handle it.  */
	  if (too_far (val, 18)
	      || stub_needed (((obj_symbol_type *)
			       fixP->fx_addsy->bsym)->tc_data.hppa_arg_reloc,
			      hppa_fixP->fx_arg_reloc))
	    return;

	  /* No stubs were needed, we can perform this relocation.  */
	  new_val = hppa_field_adjust (val, 0, hppa_fixP->fx_r_field);

	  /* Mask off 17 bits to be changed.  */
	  bfd_put_32 (stdoutput,
		      bfd_get_32 (stdoutput, buf) & 0xffe0e002,
		      buf);
	  sign_unext ((new_val - 8) >> 2, 17, &result);
	  dis_assemble_17 (result, &w1, &w2, &w);
	  result = ((w2 << 2) | (w1 << 16) | w);
	  fixP->fx_addsy = NULL;
	  break;

#undef too_far
#undef stub_needed

	case 32:
#ifdef OBJ_ELF
	  if (hppa_fixP->fx_r_type == R_HPPA_UNWIND_ENTRY
	      || hppa_fixP->fx_r_type == R_HPPA_UNWIND_ENTRIES)
	    result = fixP->fx_addnumber;
	  else
#endif
	    {
	      result = 0;
	      fixP->fx_addnumber = fixP->fx_offset;
	      bfd_put_32 (stdoutput, 0, buf);
	      return;
	    }
	  break;

	case 0:
	  return;

	default:
	  as_bad ("bad relocation type/fmt: 0x%02x/0x%02x",
		  fixP->fx_r_type, fmt);
	  return;
	}

      /* Insert the relocation.  */
      buf[0] |= (result & 0xff000000) >> 24;
      buf[1] |= (result & 0x00ff0000) >> 16;
      buf[2] |= (result & 0x0000ff00) >> 8;
      buf[3] |= result & 0x000000ff;
    }
  else
    printf ("no hppa_fixup entry for this fixup (fixP = 0x%x, type = 0x%x)\n",
	    (unsigned int) fixP, fixP->fx_r_type);
}

/* Apply a fix into a frag's data (if possible).  */

int
md_apply_fix (fixP, valp)
     fixS *fixP;
     valueT *valp;
{
  md_apply_fix_1 (fixP, (long) *valp);
  return 1;
}

/* Exactly what point is a PC-relative offset relative TO?
   On the PA, they're relative to the address of the offset.  */

long
md_pcrel_from (fixP)
     fixS *fixP;
{
  return fixP->fx_where + fixP->fx_frag->fr_address;
}

/* Return nonzero if the input line pointer is at the end of 
   a statement.  */

static int
is_end_of_statement ()
{
  return ((*input_line_pointer == '\n')
	  || (*input_line_pointer == ';')
	  || (*input_line_pointer == '!'));
}

/* Read a number from S.  The number might come in one of many forms,
   the most common will be a hex or decimal constant, but it could be
   a pre-defined register (Yuk!), or an absolute symbol.

   Return a number or -1 for failure.

   When parsing PA-89 FP register numbers RESULT will be
   the address of a structure to return information about
   L/R half of FP registers, store results there as appropriate.

   pa_parse_number can not handle negative constants and will fail
   horribly if it is passed such a constant.  */

static int
pa_parse_number (s, result)
     char **s;
     struct pa_89_fp_reg_struct *result;
{
  int num;
  char *name;
  char c;
  symbolS *sym;
  int status;
  char *p = *s;

  /* Skip whitespace before the number.  */
  while (*p == ' ' || *p == '\t')
    p = p + 1;

  /* Store info in RESULT if requested by caller.  */
  if (result)
    {
      result->number_part = -1;
      result->l_r_select = -1;
    }
  num = -1;

  if (isdigit (*p))
    {
      /* Looks like a number.  */
      num = 0;

      if (*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X'))
	{
	  /* The number is specified in hex.  */
	  p += 2;
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
	  /* The number is specified in decimal.  */
	  while (isdigit (*p))
	    {
	      num = num * 10 + *p - '0';
	      ++p;
	    }
	}

      /* Store info in RESULT if requested by the caller.  */
      if (result)
	{
	  result->number_part = num;

	  if (IS_R_SELECT (p))
	    {
	      result->l_r_select = 1;
	      ++p;
	    }
	  else if (IS_L_SELECT (p))
	    {
	      result->l_r_select = 0;
	      ++p;
	    }
	  else
	    result->l_r_select = 0;
	}
    }
  else if (*p == '%')
    {
      /* The number might be a predefined register.  */
      num = 0;
      name = p;
      p++;
      c = *p;
      /* Tege hack: Special case for general registers as the general
         code makes a binary search with case translation, and is VERY 
         slow. */
      if (c == 'r')
	{
	  p++;
	  if (*p == 'e' && *(p + 1) == 't'
	      && (*(p + 2) == '0' || *(p + 2) == '1'))
	    {
	      p += 2;
	      num = *p - '0' + 28;
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
	  /* Do a normal register search.  */
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

      /* Store info in RESULT if requested by caller.  */
      if (result)
	{
	  result->number_part = num;
	  if (IS_R_SELECT (p - 1))
	    result->l_r_select = 1;
	  else if (IS_L_SELECT (p - 1))
	    result->l_r_select = 0;
	  else
	    result->l_r_select = 0;
	}
    }
  else
    {
      /* And finally, it could be a symbol in the absolute section which
         is effectively a constant.  */
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
	  if (S_GET_SEGMENT (sym) == &bfd_abs_section)
	    num = S_GET_VALUE (sym);
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

      /* Store info in RESULT if requested by caller.  */
      if (result)
	{
	  result->number_part = num;
	  if (IS_R_SELECT (p - 1))
	    result->l_r_select = 1;
	  else if (IS_L_SELECT (p - 1))
	    result->l_r_select = 0;
	  else
	    result->l_r_select = 0;
	}
    }

  *s = p;
  return num;
}

#define REG_NAME_CNT	(sizeof(pre_defined_registers) / sizeof(struct pd_reg))

/* Given NAME, find the register number associated with that name, return
   the integer value associated with the given name or -1 on failure.  */

static int
reg_name_search (name)
     char *name;
{
  int middle, low, high;

  low = 0;
  high = REG_NAME_CNT - 1;

  do
    {
      middle = (low + high) / 2;
      if (strcasecmp (name, pre_defined_registers[middle].name) < 0)
	high = middle - 1;
      else
	low = middle + 1;
    }
  while (!((strcasecmp (name, pre_defined_registers[middle].name) == 0) ||
	   (low > high)));

  if (strcasecmp (name, pre_defined_registers[middle].name) == 0)
    return (pre_defined_registers[middle].value);
  else
    return (-1);
}


/* Return nonzero if the given INSN and L/R information will require
   a new PA-89 opcode.  */

static int
need_89_opcode (insn, result)
     struct pa_it *insn;
     struct pa_89_fp_reg_struct *result;
{
  if (result->l_r_select == 1 && !(insn->fpof1 == DBL && insn->fpof2 == DBL))
    return TRUE;
  else
    return FALSE;
}

/* Parse a condition for a fcmp instruction.  Return the numerical
   code associated with the condition.  */
 
static int
pa_parse_fp_cmp_cond (s)
     char **s;
{
  int cond, i;

  cond = 0;

  for (i = 0; i < 32; i++)
    {
      if (strncasecmp (*s, fp_cond_map[i].string,
		       strlen (fp_cond_map[i].string)) == 0)
	{
	  cond = fp_cond_map[i].cond;
	  *s += strlen (fp_cond_map[i].string);
	  while (**s == ' ' || **s == '\t')
	    *s = *s + 1;
	  return cond;
	}
    }

  as_bad ("Invalid FP Compare Condition: %c", **s);
  return 0;
}

/* Parse an FP operand format completer returning the completer
   type.  */
   
static fp_operand_format
pa_parse_fp_format (s)
     char **s;
{
  int format;

  format = SGL;
  if (**s == ',')
    {
      *s += 1;
      if (strncasecmp (*s, "sgl", 3) == 0)
	{
	  format = SGL;
	  *s += 4;
	}
      else if (strncasecmp (*s, "dbl", 3) == 0)
	{
	  format = DBL;
	  *s += 4;
	}
      else if (strncasecmp (*s, "quad", 4) == 0)
	{
	  format = QUAD;
	  *s += 5;
	}
      else
	{
	  format = ILLEGAL_FMT;
	  as_bad ("Invalid FP Operand Format: %3s", *s);
	}
    }
  while (**s == ' ' || **s == '\t' || **s == 0)
    *s = *s + 1;

  return format;
}

/* Convert from a selector string into a selector type.  */

static int
pa_chk_field_selector (str)
     char **str;
{
  int selector;
  struct selector_entry *tablep;

  selector = e_fsel;

  /* Read past any whitespace.  */
  while (**str == ' ' || **str == '\t' || **str == '\n' || **str == '\f')
    *str = *str + 1;

  /* Yuk.  Looks like a linear search through the table.  With the
     frequence of some selectors it might make sense to sort the
     table by usage.  */
  for (tablep = selector_table; tablep->prefix; tablep++)
    {
      if (strncasecmp (tablep->prefix, *str, strlen (tablep->prefix)) == 0)
	{
	  *str += strlen (tablep->prefix);
	  selector = tablep->field_selector;
	  break;
	}
    }
  return selector;
}

/* Mark (via expr_end) the end of an expression (I think).  FIXME.  */ 

static int
get_expression (str)
     char *str;
{
  char *save_in;
  asection *seg;

  save_in = input_line_pointer;
  input_line_pointer = str;
  seg = expression (&the_insn.exp);
  if (!(seg == absolute_section
	|| seg == undefined_section
	|| SEG_NORMAL (seg)))
    {
      as_warn ("Bad segment in expression."); 
      expr_end = input_line_pointer;
      input_line_pointer = save_in;
      return 1;
    }
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
  return 0;
}

/* Mark (via expr_end) the end of an absolute expression.  FIXME. */
static int
pa_get_absolute_expression (str)
     char *str;
{
  char *save_in;

  save_in = input_line_pointer;
  input_line_pointer = str;
  expression (&the_insn.exp);
  if (the_insn.exp.X_op != O_constant)
    {
      as_warn ("Bad segment (should be absolute).");
      expr_end = input_line_pointer;
      input_line_pointer = save_in;
      return 1;
    }
  expr_end = input_line_pointer;
  input_line_pointer = save_in;
  return 0;
}

/* Evaluate an absolute expression EXP which may be modified by 
   the selector FIELD_SELECTOR.  Return the value of the expression.  */
static int
evaluate_absolute (exp, field_selector)
     expressionS exp;
     int field_selector;
{
  int value;

  value = exp.X_add_number;

  switch (field_selector)
    {
    /* No change.  */
    case e_fsel:
      break;

    /* If bit 21 is on then add 0x800 and arithmetic shift right 11 bits.  */
    case e_lssel:
      if (value & 0x00000400)
	value += 0x800;
      value = (value & 0xfffff800) >> 11;
      break;

    /* Sign extend from bit 21.  */
    case e_rssel:
      if (value & 0x00000400)
	value |= 0xfffff800;
      else
	value &= 0x7ff;
      break;

    /* Arithmetic shift right 11 bits.  */
    case e_lsel:
      value = (value & 0xfffff800) >> 11;
      break;

    /* Set bits 0-20 to zero.  */
    case e_rsel:
      value = value & 0x7ff;
      break;

    /* Add 0x800 and arithmetic shift right 11 bits.  */
    case e_ldsel:
      value += 0x800;


      value = (value & 0xfffff800) >> 11;
      break;

    /* Set bitgs 0-21 to one.  */
    case e_rdsel:
      value |= 0xfffff800;
      break;

    /* This had better get fixed.  It looks like we're quickly moving
       to LR/RR.  FIXME.  */
    case e_rrsel:
    case e_lrsel:
      abort ();

    default:
      BAD_CASE (field_selector);
      break;
    }
  return value;
}

/* Given an argument location specification return the associated
   argument location number.  */

static unsigned int
pa_build_arg_reloc (type_name)
     char *type_name;
{

  if (strncasecmp (type_name, "no", 2) == 0)
    return 0;
  if (strncasecmp (type_name, "gr", 2) == 0)
    return 1;
  else if (strncasecmp (type_name, "fr", 2) == 0)
    return 2;
  else if (strncasecmp (type_name, "fu", 2) == 0)
    return 3;
  else
    as_bad ("Invalid argument location: %s\n", type_name);

  return 0;
}

/* Encode and return an argument relocation specification for
   the given register in the location specified by arg_reloc.  */

static unsigned int
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
      as_bad ("Invalid argument description: %d", reg);
    }

  return new_reloc;
}

/* Parse a PA nullification completer (,n).  Return nonzero if the
   completer was found; return zero if no completer was found.  */

static int
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
	  as_bad ("Invalid Nullification: (%c)", **s);
	  nullif = 0;
	}
      *s = *s + 1;
    }
  while (**s == ' ' || **s == '\t')
    *s = *s + 1;

  return nullif;
}

/* Parse a non-negated compare/subtract completer returning the
   number (for encoding in instrutions) of the given completer.

   ISBRANCH specifies whether or not this is parsing a condition
   completer for a branch (vs a nullification completer for a
   computational instruction.  */

static int
pa_parse_nonneg_cmpsub_cmpltr (s, isbranch)
     char **s;
     int isbranch;
{
  int cmpltr;
  char *name = *s + 1;
  char c;
  char *save_s = *s;

  cmpltr = 0;
  if (**s == ',')
    {
      *s += 1;
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
      /* If we have something like addb,n then there is no condition
         completer.  */
      else if (strcasecmp (name, "n") == 0 && isbranch)
	{
	  cmpltr = 0;
	}
      else
	{
	  cmpltr = -1;
	}
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  /* Reset pointers if this was really a ,n for a branch instruction.  */
  if (cmpltr == 0 && *name == 'n' && isbranch)
    *s = save_s;

  return cmpltr;
}

/* Parse a negated compare/subtract completer returning the
   number (for encoding in instrutions) of the given completer.

   ISBRANCH specifies whether or not this is parsing a condition
   completer for a branch (vs a nullification completer for a
   computational instruction.  */

static int
pa_parse_neg_cmpsub_cmpltr (s, isbranch)
     char **s;
     int isbranch;
{
  int cmpltr;
  char *name = *s + 1;
  char c;
  char *save_s = *s;

  cmpltr = 0;
  if (**s == ',')
    {
      *s += 1;
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
      /* If we have something like addb,n then there is no condition
         completer.  */
      else if (strcasecmp (name, "n") == 0 && isbranch)
	{
	  cmpltr = 0;
	}
      else
	{
	  cmpltr = -1;
	}
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  /* Reset pointers if this was really a ,n for a branch instruction.  */
  if (cmpltr == 0 && *name == 'n' && isbranch)
    *s = save_s;

  return cmpltr;
}

/* Parse a non-negated addition completer returning the number
   (for encoding in instrutions) of the given completer.

   ISBRANCH specifies whether or not this is parsing a condition
   completer for a branch (vs a nullification completer for a
   computational instruction.  */

static int
pa_parse_nonneg_add_cmpltr (s, isbranch)
     char **s;
     int isbranch;
{
  int cmpltr;
  char *name = *s + 1;
  char c;
  char *save_s = *s;

  cmpltr = 0;
  if (**s == ',')
    {
      *s += 1;
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
      /* If we have something like addb,n then there is no condition
         completer.  */
      else if (strcasecmp (name, "n") == 0 && isbranch)
	{
	  cmpltr = 0;
	}
      else
	{
	  cmpltr = -1;
	}
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  /* Reset pointers if this was really a ,n for a branch instruction.  */
  if (cmpltr == 0 && *name == 'n' && isbranch)
    *s = save_s;

  return cmpltr;
}

/* Parse a negated addition completer returning the number
   (for encoding in instrutions) of the given completer.

   ISBRANCH specifies whether or not this is parsing a condition
   completer for a branch (vs a nullification completer for a
   computational instruction.  */

static int
pa_parse_neg_add_cmpltr (s, isbranch)
     char **s;
     int isbranch;
{
  int cmpltr;
  char *name = *s + 1;
  char c;
  char *save_s = *s;

  cmpltr = 0;
  if (**s == ',')
    {
      *s += 1;
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
      /* If we have something like addb,n then there is no condition
         completer.  */
      else if (strcasecmp (name, "n") == 0 && isbranch)
	{
	  cmpltr = 0;
	}
      else
	{
	  cmpltr = -1;
	}
      **s = c;
    }
  if (cmpltr >= 0)
    {
      while (**s == ' ' || **s == '\t')
	*s = *s + 1;
    }

  /* Reset pointers if this was really a ,n for a branch instruction.  */
  if (cmpltr == 0 && *name == 'n' && isbranch)
    *s = save_s;

  return cmpltr;
}

/* Handle a .BLOCK type pseudo-op.  */

static void
pa_block (z)
     int z;
{
  char *p;
  long int temp_fill;
  unsigned int temp_size;
  int i;

  temp_size = get_absolute_expression ();

  /* Always fill with zeros, that's what the HP assembler does.  */
  temp_fill = 0;

  p = frag_var (rs_fill, (int) temp_size, (int) temp_size, 
		(relax_substateT) 0, (symbolS *) 0, 1, NULL);
  bzero (p, temp_size);

  /* Convert 2 bytes at a time.  */

  for (i = 0; i < temp_size; i += 2)
    {
      md_number_to_chars (p + i,
			  (valueT) temp_fill,
			  (int) ((temp_size - i) > 2 ? 2 : (temp_size - i)));
    }

  pa_undefine_label ();
  demand_empty_rest_of_line ();
  return;
}

/* Handle a .CALL pseudo-op.  This involves storing away information
   about where arguments are to be found so the linker can detect
   (and correct) argument location mismatches between caller and callee.  */

static void
pa_call (unused)
     int unused;
{
  pa_call_args (&last_call_desc);
  demand_empty_rest_of_line ();
  return;
}

/* Do the dirty work of building a call descriptor which describes
   where the caller placed arguments to a function call.  */

static void
pa_call_args (call_desc)
     struct call_desc *call_desc;
{
  char *name, c, *p;
  unsigned int temp, arg_reloc;

  while (!is_end_of_statement ())
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      /* Process a source argument.  */
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
      /* Process a return value.  */
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
	  as_bad ("Invalid .CALL argument: %s", name);
	}
      p = input_line_pointer;
      *p = c;
      if (!is_end_of_statement ())
	input_line_pointer++;
    }
}

/* Return TRUE if FRAG1 and FRAG2 are the same.  */

static int
is_same_frag (frag1, frag2)
     fragS *frag1;
     fragS *frag2;
{

  if (frag1 == NULL)
    return (FALSE);
  else if (frag2 == NULL)
    return (FALSE);
  else if (frag1 == frag2)
    return (TRUE);
  else if (frag2->fr_type == rs_fill && frag2->fr_fix == 0)
    return (is_same_frag (frag1, frag2->fr_next));
  else
    return (FALSE);
}

/* Build an entry in the UNWIND subspace from the given 
   function attributes in CALL_INFO.  */
   
static void
pa_build_unwind_subspace (call_info)
     struct call_info *call_info;
{
  char *unwind;
  asection *seg, *save_seg;
  subsegT subseg, save_subseg;
  int i;
  char c, *p;

  /* Get into the right seg/subseg.  This may involve creating
     the seg the first time through.  Make sure to have the
     old seg/subseg so that we can reset things when we are done.  */
  subseg = SUBSEG_UNWIND;
  seg = bfd_get_section_by_name (stdoutput, UNWIND_SECTION_NAME);
  if (seg == ASEC_NULL)
    {
      seg = bfd_make_section_old_way (stdoutput, UNWIND_SECTION_NAME);
      bfd_set_section_flags (stdoutput, seg,
			     SEC_READONLY | SEC_HAS_CONTENTS
			     | SEC_LOAD | SEC_RELOC);
    }

  save_seg = now_seg;
  save_subseg = now_subseg;
  subseg_set (seg, subseg);


  /* Get some space to hold relocation information for the unwind
     descriptor.  */
  p = frag_more (4);
  call_info->start_offset_frag = frag_now;
  call_info->start_frag_where = p - frag_now->fr_literal;

  /* Relocation info. for start offset of the function.  */
  fix_new_hppa (frag_now, p - frag_now->fr_literal, 4,
		call_info->start_symbol, (offsetT) 0,
		(expressionS *) NULL, 0, R_HPPA_UNWIND, e_fsel, 32, 0,
		(char *) 0);

  /* We need to search for the first relocation involving the start_symbol of
     this call_info descriptor.  */
  {
    fixS *fixP;

    call_info->start_fix = seg_info (now_seg)->fix_root;
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

  /* Relocation info. for end offset of the function.  */
  fix_new_hppa (frag_now, p - frag_now->fr_literal, 4,
		call_info->end_symbol, (offsetT) 0,
		(expressionS *) NULL, 0, R_HPPA_UNWIND, e_fsel, 32, 0,
		(char *) 0);

  /* We need to search for the first relocation involving the end_symbol of
     this call_info descriptor.  */
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

  /* Dump it. */
  unwind = (char *) &call_info->ci_unwind;
  for (i = 8; i < sizeof (struct unwind_table); i++)
    {
      c = *(unwind + i);
      {
	FRAG_APPEND_1_CHAR (c);
      }
    }

  /* Return back to the original segment/subsegment.  */
  subseg_set (save_seg, save_subseg);
}

/* Process a .CALLINFO pseudo-op.  This information is used later
   to build unwind descriptors and maybe one day to support
   .ENTER and .LEAVE.  */

static void
pa_callinfo (unused)
     int unused;
{
  char *name, c, *p;
  int temp;

  /* .CALLINFO must appear within a procedure definition.  */
  if (!within_procedure)
    as_bad (".callinfo is not within a procedure definition");

  /* Mark the fact that we found the .CALLINFO for the
     current procedure.  */
  callinfo_found = TRUE;

  /* Iterate over the .CALLINFO arguments.  */
  while (!is_end_of_statement ())
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      /* Frame size specification.  */
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

  	  /* callinfo is in bytes and unwind_desc is in 8 byte units.  */
	  last_call_info->ci_unwind.descriptor.frame_size = temp / 8;

	}
      /* Entry register (GR, GR and SR) specifications.  */
      else if ((strncasecmp (name, "entry_gr", 8) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = get_absolute_expression ();
	  /* The HP assembler accepts 19 as the high bound for ENTRY_GR
	     even though %r19 is caller saved.  I think this is a bug in 
	     the HP assembler, and we are not going to emulate it.  */
	  if (temp < 3 || temp > 18)
	    as_bad ("Value for ENTRY_GR must be in the range 3..18\n");
	  last_call_info->ci_unwind.descriptor.entry_gr = temp - 2;
	}
      else if ((strncasecmp (name, "entry_fr", 8) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = get_absolute_expression ();
	  /* Similarly the HP assembler takes 31 as the high bound even 
	     though %fr21 is the last callee saved floating point register.  */
	  if (temp < 12 || temp > 21)
	    as_bad ("Value for ENTRY_FR must be in the range 12..21\n");
	  last_call_info->ci_unwind.descriptor.entry_fr = temp - 11;
	}
      else if ((strncasecmp (name, "entry_sr", 8) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = get_absolute_expression ();
	  if (temp != 3)
	    as_bad ("Value for ENTRY_SR must be 3\n");
	  last_call_info->entry_sr = temp - 2;
	}
      /* Note whether or not this function performs any calls.  */
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
      /* Should RP be saved into the stack.  */
      else if ((strncasecmp (name, "save_rp", 7) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->ci_unwind.descriptor.save_rp = 1;
	}
      /* Likewise for SP.  */
      else if ((strncasecmp (name, "save_sp", 7) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->ci_unwind.descriptor.save_sp = 1;
	}
      /* Is this an unwindable procedure.  If so mark it so
	 in the unwind descriptor.  */
      else if ((strncasecmp (name, "no_unwind", 9) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->ci_unwind.descriptor.cannot_unwind = 1;
	}
      /* Is this an interrupt routine.  If so mark it in the
	 unwind descriptor.  */
      else if ((strncasecmp (name, "hpux_int", 7) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  last_call_info->ci_unwind.descriptor.hpux_interrupt_marker = 1;
	}
      else
	{
	  as_bad ("Invalid .CALLINFO argument: %s", name);
	}
      if (!is_end_of_statement ())
	input_line_pointer++;
    }

  demand_empty_rest_of_line ();
  return;
}

/* Switch into the code subspace.  */

static void
pa_code (unused)
     int unused;
{
  sd_chain_struct *sdchain;

  /* First time through it might be necessary to create the 
     $TEXT$ space.  */
  if ((sdchain = is_defined_space ("$TEXT$")) == NULL)
    {
      sdchain = create_new_space (pa_def_spaces[0].name,
				  pa_def_spaces[0].spnum,
				  pa_def_spaces[0].loadable,
				  pa_def_spaces[0].defined,
				  pa_def_spaces[0].private,
				  pa_def_spaces[0].sort,
				  pa_def_spaces[0].segment, 0);
    }

  SPACE_DEFINED (sdchain) = 1;
  subseg_set (text_section, SUBSEG_CODE);
  demand_empty_rest_of_line ();
  return;
}

/* This is different than the standard GAS s_comm(). On HP9000/800 machines,
   the .comm pseudo-op has the following symtax:

   <label> .comm <length>

   where <label> is optional and is a symbol whose address will be the start of
   a block of memory <length> bytes long. <length> must be an absolute
   expression.  <length> bytes will be allocated in the current space
   and subspace.  */

static void
pa_comm (unused)
     int unused;
{
  unsigned int size;
  symbolS *symbol;
  label_symbol_struct *label_symbol = pa_get_label ();

  if (label_symbol)
    symbol = label_symbol->lss_label;
  else
    symbol = NULL;

  SKIP_WHITESPACE ();
  size = get_absolute_expression ();

  if (symbol)
    {
      /* It is incorrect to check S_IS_DEFINED at this point as 
	 the symbol will *always* be defined.  FIXME.  How to 
	 correctly determine when this label really as been 
	 defined before.  */
      if (S_GET_VALUE (symbol))
	{
	  if (S_GET_VALUE (symbol) != size)
	    {
	      as_warn ("Length of .comm \"%s\" is already %d. Not changed.",
		       S_GET_NAME (symbol), S_GET_VALUE (symbol));
	      return;
	    }
	}
      else
	{
	  S_SET_VALUE (symbol, size);
	  S_SET_SEGMENT (symbol, &bfd_und_section);
	  S_SET_EXTERNAL (symbol);
	}
    }
  demand_empty_rest_of_line ();
}

/* Process a .COPYRIGHT pseudo-op.  */

static void
pa_copyright (unused)
     int unused;
{
  char *name;
  char c;

  SKIP_WHITESPACE ();
  if (*input_line_pointer == '\"')
    {
      ++input_line_pointer;
      name = input_line_pointer;
      while ((c = next_char_of_string ()) >= 0)
	;
      c = *input_line_pointer;
      *input_line_pointer = '\0';
      *(input_line_pointer - 1) = '\0';
      {
	/* FIXME.  Not supported */
	abort ();
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

/* Process a .END pseudo-op.  */

static void
pa_end (unused)
     int unused;
{
  demand_empty_rest_of_line ();
  return;
}

/* Process a .ENTER pseudo-op.  This is not supported.  */ 
static void
pa_enter (unused)
     int unused;
{
  abort();
  return;
}

/* Process a .ENTRY pseudo-op.  .ENTRY marks the beginning of the
   procesure.  */
static void
pa_entry (unused)
     int unused;
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

  /* Go back to the last symbol and turn on the BSF_FUNCTION flag.
     It will not be on if no .EXPORT pseudo-op exists (static function).  */
  last_call_info->start_symbol->bsym->flags |= BSF_FUNCTION;

  return;
}

/* Handle a .EQU pseudo-op.  */

static void
pa_equ (reg)
     int reg;
{
  label_symbol_struct *label_symbol = pa_get_label ();
  symbolS *symbol;

  if (label_symbol)
    {
      symbol = label_symbol->lss_label;
      S_SET_VALUE (symbol, (unsigned int) get_absolute_expression ());
      S_SET_SEGMENT (symbol, &bfd_abs_section);
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

/* Helper function.  Does processing for the end of a function.  This
   usually involves creating some relocations or building special
   symbols to mark the end of the function.  */

static void
process_exit ()
{
  char *where;

  where = frag_more (0);

  /* ELF does not have EXIT relocations.  All we do is create a
     temporary symbol marking the end of the function.  */
  {
    char *name = (char *) xmalloc (strlen ("L$\001end_") +
		    strlen (S_GET_NAME (last_call_info->start_symbol)) + 1);

    if (name)
      {
	symbolS *symbolP;

	strcpy (name, "L$\001end_");
	strcat (name, S_GET_NAME (last_call_info->start_symbol));

	symbolP = symbol_find (name);
	if (symbolP)
	  as_warn ("Symbol '%s' already defined.", name);
	else
	  {
	    /* symbol value should be the offset of the
	       last instruction of the function */
	    symbolP = symbol_new (name, now_seg,
				  (valueT) (obstack_next_free (&frags) 
					    - frag_now->fr_literal - 4),
				  frag_now);

	    assert (symbolP);
	    symbolP->bsym->flags = BSF_LOCAL;
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

  /* Stuff away the location of the frag for the end of the function,
     and call pa_build_unwind_subspace to add an entry in the unwind
     table.  */
  last_call_info->end_frag = frag_now;
  pa_build_unwind_subspace (last_call_info);
  exit_processing_complete = TRUE;
}

/* Process a .EXIT pseudo-op.  */

static void
pa_exit (unused)
     int unused;
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

/* Process a .EXPORT directive.  This makes functions external
   and provides information such as argument relocation entries 
   to callers.  */

static void
pa_export (unused)
     int unused;
{
  char *name, c, *p;
  symbolS *symbol;

  name = input_line_pointer;
  c = get_symbol_end ();
  /* Make sure the given symbol exists.  */
  if ((symbol = symbol_find_or_make (name)) == NULL)
    {
      as_bad ("Cannot define export symbol: %s\n", name);
      p = input_line_pointer;
      *p = c;
      input_line_pointer++;
    }
  else
    {
      /* OK.  Set the external bits and process argument relocations.  */
      S_SET_EXTERNAL (symbol);
      p = input_line_pointer;
      *p = c;
      if (!is_end_of_statement ())
	{
	  input_line_pointer++;
	  pa_export_args (symbol);
#ifdef OBJ_ELF
	  pa_build_symextn_section ();
#endif
	}
    }

  demand_empty_rest_of_line ();
  return;
}

/* Helper function to process arguments to a .EXPORT pseudo-op.  */

static void
pa_export_args (symbolP)
     symbolS *symbolP;
{
  char *name, c, *p;
  unsigned int temp, arg_reloc;
  pa_symbol_type type = SYMBOL_TYPE_UNKNOWN;
  obj_symbol_type *symbol = (obj_symbol_type *) symbolP->bsym;

  if (strncasecmp (input_line_pointer, "absolute", 8) == 0)
    {
      input_line_pointer += 8;
      symbolP->bsym->flags &= ~BSF_FUNCTION;
      S_SET_SEGMENT (symbolP, &bfd_abs_section);
      type = SYMBOL_TYPE_ABSOLUTE;
    }
  else if (strncasecmp (input_line_pointer, "code", 4) == 0)
    {
      input_line_pointer += 4;
      symbolP->bsym->flags &= ~BSF_FUNCTION;
      type = SYMBOL_TYPE_CODE;
    }
  else if (strncasecmp (input_line_pointer, "data", 4) == 0)
    {
      input_line_pointer += 4;
      symbolP->bsym->flags &= ~BSF_FUNCTION;
      type = SYMBOL_TYPE_DATA;
    }
  else if ((strncasecmp (input_line_pointer, "entry", 5) == 0))
    {
      input_line_pointer += 5;
      symbolP->bsym->flags |= BSF_FUNCTION;
      type = SYMBOL_TYPE_ENTRY;
    }
  else if (strncasecmp (input_line_pointer, "millicode", 9) == 0)
    {
      input_line_pointer += 9;
      symbolP->bsym->flags |= BSF_FUNCTION;
      type = SYMBOL_TYPE_MILLICODE;
    }
  else if (strncasecmp (input_line_pointer, "plabel", 6) == 0)
    {
      input_line_pointer += 6;
      symbolP->bsym->flags &= ~BSF_FUNCTION;
      type = SYMBOL_TYPE_PLABEL;
    }
  else if (strncasecmp (input_line_pointer, "pri_prog", 8) == 0)
    {
      input_line_pointer += 8;
      symbolP->bsym->flags |= BSF_FUNCTION;
      type = SYMBOL_TYPE_PRI_PROG;
    }
  else if (strncasecmp (input_line_pointer, "sec_prog", 8) == 0)
    {
      input_line_pointer += 8;
      symbolP->bsym->flags |= BSF_FUNCTION;
      type = SYMBOL_TYPE_SEC_PROG;
    }

  /* SOM requires much more information about symbol types
     than BFD understands.  This is how we get this information
     to the SOM BFD backend.  */
#ifdef obj_set_symbol_type
  obj_set_symbol_type (symbolP->bsym, (int) type);
#endif

  /* Now that the type of the exported symbol has been handled,
     handle any argument relocation information.  */
  while (!is_end_of_statement ())
    {
      if (*input_line_pointer == ',')
	input_line_pointer++;
      name = input_line_pointer;
      c = get_symbol_end ();
      /* Argument sources.  */
      if ((strncasecmp (name, "argw", 4) == 0))
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = atoi (name + 4);
	  name = input_line_pointer;
	  c = get_symbol_end ();
	  arg_reloc = pa_align_arg_reloc (temp, pa_build_arg_reloc (name));
	  symbol->tc_data.hppa_arg_reloc |= arg_reloc;
	  *input_line_pointer = c;
	}
      /* The return value.  */
      else if ((strncasecmp (name, "rtnval", 6)) == 0)
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  name = input_line_pointer;
	  c = get_symbol_end ();
	  arg_reloc = pa_build_arg_reloc (name);
	  symbol->tc_data.hppa_arg_reloc |= arg_reloc;
	  *input_line_pointer = c;
	}
      /* Privelege level.  */
      else if ((strncasecmp (name, "priv_lev", 8)) == 0)
	{
	  p = input_line_pointer;
	  *p = c;
	  input_line_pointer++;
	  temp = atoi (input_line_pointer);
	  c = get_symbol_end ();
	  *input_line_pointer = c;
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

/* Handle an .IMPORT pseudo-op.  Any symbol referenced in a given
   assembly file must either be defined in the assembly file, or
   explicitly IMPORTED from another.  */

static void
pa_import (unused)
     int unused;
{
  char *name, c, *p;
  symbolS *symbol;

  name = input_line_pointer;
  c = get_symbol_end ();

  symbol = symbol_find_or_make (name);
  p = input_line_pointer;
  *p = c;

  if (!is_end_of_statement ())
    {
      input_line_pointer++;
      pa_export_args (symbol);
    }
  else
    {
      /* Sigh.  To be compatable with the HP assembler and to help
	 poorly written assembly code, we assign a type based on 
	 the the current segment.  Note only BSF_FUNCTION really
	 matters, we do not need to set the full SYMBOL_TYPE_* info here.  */
      if (now_seg == text_section)
	symbol->bsym->flags |= BSF_FUNCTION;

      /* If the section is undefined, then the symbol is undefined
         Since this is an import, leave the section undefined.  */
      S_SET_SEGMENT (symbol, &bfd_und_section);
    }

  demand_empty_rest_of_line ();
  return;
}

/* Handle a .LABEL pseudo-op.  */

static void
pa_label (unused)
     int unused;
{
  char *name, c, *p;

  name = input_line_pointer;
  c = get_symbol_end ();

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

/* Handle a .LEAVE pseudo-op.  This is not supported yet.  */

static void
pa_leave (unused)
     int unused;
{
  abort();
}

/* Handle a .ORIGIN pseudo-op.  */

static void
pa_origin (unused)
     int unused;
{
  s_org (0);
  pa_undefine_label ();
  return;
}

/* Handle a .PARAM pseudo-op.  This is much like a .EXPORT, except it
   is for static functions.  FIXME.  Should share more code with .EXPORT.  */

static void
pa_param (unused)
     int unused;
{
  char *name, c, *p;
  symbolS *symbol;

  name = input_line_pointer;
  c = get_symbol_end ();

  if ((symbol = symbol_find_or_make (name)) == NULL)
    {
      as_bad ("Cannot define static symbol: %s\n", name);
      p = input_line_pointer;
      *p = c;
      input_line_pointer++;
    }
  else
    {
      S_CLEAR_EXTERNAL (symbol);
      p = input_line_pointer;
      *p = c;
      if (!is_end_of_statement ())
	{
	  input_line_pointer++;
	  pa_export_args (symbol);
	}
    }

  demand_empty_rest_of_line ();
  return;
}

/* Handle a .PROC pseudo-op.  It is used to mark the beginning
   of a procedure from a syntatical point of view.  */

static void
pa_proc (unused)
     int unused;
{
  struct call_info *call_info;

  if (within_procedure)
    as_fatal ("Nested procedures");

  /* Reset global variables for new procedure.  */
  callinfo_found = FALSE;
  within_procedure = TRUE;
  exit_processing_complete = FALSE;

  /* Create another call_info structure.  */
  call_info = (struct call_info *) xmalloc (sizeof (struct call_info));

  if (!call_info)
    as_fatal ("Cannot allocate unwind descriptor\n");

  bzero (call_info, sizeof (struct call_info));

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
  call_info->ci_unwind.descriptor.hpux_interrupt_marker = 0;
  call_info->entry_sr = ~0;
  call_info->makes_calls = 1;

  /* If we got a .PROC pseudo-op, we know that the function is defined
     locally.  Make sure it gets into the symbol table.  */
  {
    label_symbol_struct *label_symbol = pa_get_label ();

    if (label_symbol)
      {
	if (label_symbol->lss_label)
	  {
	    last_call_info->start_symbol = label_symbol->lss_label;
	    label_symbol->lss_label->bsym->flags |= BSF_FUNCTION;
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

/* Process the syntatical end of a procedure.  Make sure all the 
   appropriate pseudo-ops were found within the procedure.  */

static void
pa_procend (unused)
     int unused;
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

/* Parse the parameters to a .SPACE directive; if CREATE_FLAG is nonzero,
   then create a new space entry to hold the information specified
   by the parameters to the .SPACE directive.  */

static sd_chain_struct *
pa_parse_space_stmt (space_name, create_flag)
     char *space_name;
     int create_flag;
{
  char *name, *ptemp, c;
  char loadable, defined, private, sort;
  int spnum;
  asection *seg = NULL;
  sd_chain_struct *space;

  /* load default values */
  spnum = 0;
  sort = 0;
  loadable = TRUE;
  defined = TRUE;
  private = FALSE;
  if (strcasecmp (space_name, "$TEXT$") == 0)
    {
      seg = pa_def_spaces[0].segment;
      sort = pa_def_spaces[0].sort;
    }
  else if (strcasecmp (space_name, "$PRIVATE$") == 0)
    {
      seg = pa_def_spaces[1].segment;
      sort = pa_def_spaces[1].sort;
    }

  if (!is_end_of_statement ())
    {
      print_errors = FALSE;
      ptemp = input_line_pointer + 1;
      /* First see if the space was specified as a number rather than
         as a name.  According to the PA assembly manual the rest of 
         the line should be ignored.  */
      if ((spnum = pa_parse_number (&ptemp, 0)) >= 0)
	input_line_pointer = ptemp;
      else
	{
	  while (!is_end_of_statement ())
	    {
	      input_line_pointer++;
	      name = input_line_pointer;
	      c = get_symbol_end ();
	      if ((strncasecmp (name, "SPNUM", 5) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  spnum = get_absolute_expression ();
		}
	      else if ((strncasecmp (name, "SORT", 4) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  sort = get_absolute_expression ();
		}
	      else if ((strncasecmp (name, "UNLOADABLE", 10) == 0))
		{
		  *input_line_pointer = c;
		  loadable = FALSE;
		}
	      else if ((strncasecmp (name, "NOTDEFINED", 10) == 0))
		{
		  *input_line_pointer = c;
		  defined = FALSE;
		}
	      else if ((strncasecmp (name, "PRIVATE", 7) == 0))
		{
		  *input_line_pointer = c;
		  private = TRUE;
		}
	      else
		{
		  as_bad ("Invalid .SPACE argument");
		  *input_line_pointer = c;
		  if (! is_end_of_statement ())
		    input_line_pointer++;
		}
	    }
	}
      print_errors = TRUE;
    }

  if (create_flag && seg == NULL)
    seg = subseg_new (space_name, 0);
    
  /* If create_flag is nonzero, then create the new space with
     the attributes computed above.  Else set the values in 
     an already existing space -- this can only happen for
     the first occurence of a built-in space.  */
  if (create_flag)
    space = create_new_space (space_name, spnum, loadable, defined,
			      private, sort, seg, 1);
  else
    {
      space = is_defined_space (space_name);
      SPACE_SPNUM (space) = spnum;
      SPACE_LOADABLE (space) = loadable & 1;
      SPACE_DEFINED (space) = defined & 1;
      SPACE_USER_DEFINED (space) = 1;
      SPACE_PRIVATE (space) = private & 1;
      SPACE_SORT (space) = sort & 0xff;
      space->sd_seg = seg;
    }

#ifdef obj_set_section_attributes
  obj_set_section_attributes (seg, defined, private, sort, spnum);
#endif

  return space;
}

/* Adjust the frag's alignment according to the alignment needs
   of the given subspace/subsegment.  */

static void
pa_align_subseg (seg, subseg)
     asection *seg;
     subsegT subseg;
{
  ssd_chain_struct *now_subspace;
  int alignment;
  int shift = 0;

  now_subspace = pa_subsegment_to_subspace (seg, subseg);
  if (now_subspace)
    {
      if (SUBSPACE_ALIGN (now_subspace) == 0)
	alignment = now_subspace->ssd_last_align;
      else if (now_subspace->ssd_last_align > SUBSPACE_ALIGN (now_subspace))
	alignment = now_subspace->ssd_last_align;
      else
	alignment = SUBSPACE_ALIGN (now_subspace);

      while ((1 << shift) < alignment)
	shift++;
    }
  else
    shift = bfd_get_section_alignment (stdoutput, seg);

  frag_align (shift, 0);
}

/* Handle a .SPACE pseudo-op; this switches the current space to the
   given space, creating the new space if necessary.  */

static void
pa_space (unused)
     int unused;
{
  char *name, c, *space_name, *save_s;
  int temp;
  sd_chain_struct *sd_chain;

  if (within_procedure)
    {
      as_bad ("Can\'t change spaces within a procedure definition. Ignored");
      ignore_rest_of_line ();
    }
  else
    {
      /* Check for some of the predefined spaces.   FIXME: most of the code
         below is repeated several times, can we extract the common parts
         and place them into a subroutine or something similar?  */
      if (strncasecmp (input_line_pointer, "$text$", 6) == 0)
	{
	  input_line_pointer += 6;
	  sd_chain = is_defined_space ("$TEXT$");
	  if (sd_chain == NULL)
	    sd_chain = pa_parse_space_stmt ("$TEXT$", 1);
	  else if (SPACE_USER_DEFINED (sd_chain) == 0)
	    sd_chain = pa_parse_space_stmt ("$TEXT$", 0);

	  current_space = sd_chain;

	  /* No need to align if we are already there.  */
	  if (now_seg != text_section)
	    pa_align_subseg (now_seg, now_subseg);

	  subseg_set (text_section, sd_chain->sd_last_subseg);

	  current_subspace
	    = pa_subsegment_to_subspace (text_section,
					 sd_chain->sd_last_subseg);
	  demand_empty_rest_of_line ();
	  return;
	}
      if (strncasecmp (input_line_pointer, "$private$", 9) == 0)
	{
	  input_line_pointer += 9;
	  sd_chain = is_defined_space ("$PRIVATE$");
	  if (sd_chain == NULL)
	    sd_chain = pa_parse_space_stmt ("$PRIVATE$", 1);
	  else if (SPACE_USER_DEFINED (sd_chain) == 0)
	    sd_chain = pa_parse_space_stmt ("$PRIVATE$", 0);

	  current_space = sd_chain;

	  /* No need to align if we are already there.  */
	  if (now_seg != data_section)
	    pa_align_subseg (now_seg, now_subseg);

	  subseg_set (data_section, sd_chain->sd_last_subseg);
	  current_subspace
	    = pa_subsegment_to_subspace (data_section,
					 sd_chain->sd_last_subseg);
	  demand_empty_rest_of_line ();
	  return;
	}
      if (!strncasecmp (input_line_pointer,
			GDB_DEBUG_SPACE_NAME,
			strlen (GDB_DEBUG_SPACE_NAME)))
	{
	  input_line_pointer += strlen (GDB_DEBUG_SPACE_NAME);
	  sd_chain = is_defined_space (GDB_DEBUG_SPACE_NAME);
	  if (sd_chain == NULL)
	    sd_chain = pa_parse_space_stmt (GDB_DEBUG_SPACE_NAME, 1);
	  else if (SPACE_USER_DEFINED (sd_chain) == 0)
	    sd_chain = pa_parse_space_stmt (GDB_DEBUG_SPACE_NAME, 0);

	  current_space = sd_chain;

	  {
	    asection *gdb_section
	    = bfd_make_section_old_way (stdoutput, GDB_DEBUG_SPACE_NAME);

	    /* No need to align if we are already there.  */
	    if (strcmp (segment_name (now_seg), GDB_DEBUG_SPACE_NAME) != 0)
	      pa_align_subseg (now_seg, now_subseg);

	    subseg_set (gdb_section, sd_chain->sd_last_subseg);
	    current_subspace
	      = pa_subsegment_to_subspace (gdb_section,
					   sd_chain->sd_last_subseg);
	  }
	  demand_empty_rest_of_line ();
	  return;
	}

      /* It could be a space specified by number.  */
      print_errors = 0;
      save_s = input_line_pointer;
      if ((temp = pa_parse_number (&input_line_pointer, 0)) >= 0)
	{
	  if (sd_chain = pa_find_space_by_number (temp))
	    {
	      current_space = sd_chain;

	      if (now_seg != sd_chain->sd_seg)
		pa_align_subseg (now_seg, now_subseg);
	      subseg_set (sd_chain->sd_seg, sd_chain->sd_last_subseg);
	      current_subspace
		= pa_subsegment_to_subspace (sd_chain->sd_seg,
					     sd_chain->sd_last_subseg);
	      demand_empty_rest_of_line ();
	      return;
	    }
	}

      /* Not a number, attempt to create a new space.  */
      print_errors = 1;
      input_line_pointer = save_s;
      name = input_line_pointer;
      c = get_symbol_end ();
      space_name = xmalloc (strlen (name) + 1);
      strcpy (space_name, name);
      *input_line_pointer = c;

      sd_chain = pa_parse_space_stmt (space_name, 1);
      current_space = sd_chain;

      if (now_seg != sd_chain->sd_seg)
	pa_align_subseg (now_seg, now_subseg);
      subseg_set (sd_chain->sd_seg, sd_chain->sd_last_subseg);
      current_subspace = pa_subsegment_to_subspace (sd_chain->sd_seg,
						  sd_chain->sd_last_subseg);
      demand_empty_rest_of_line ();
    }
  return;
}

/* Switch to a new space.  (I think).  FIXME.  */ 

static void
pa_spnum (unused)
     int unused;
{
  char *name;
  char c;
  char *p;
  sd_chain_struct *space;

  name = input_line_pointer;
  c = get_symbol_end ();
  space = is_defined_space (name);
  if (space)
    {
      p = frag_more (4);
      md_number_to_chars (p, SPACE_SPNUM (space), 4);
    }
  else
    as_warn ("Undefined space: '%s' Assuming space number = 0.", name);

  *input_line_pointer = c;
  demand_empty_rest_of_line ();
  return;
}

/* If VALUE is an exact power of two between zero and 2^31, then 
   return log2 (VALUE).  Else return -1.  */

static int
log2 (value)
     int value;
{
  int shift = 0;

  while ((1 << shift) != value && shift < 32)
    shift++;

  if (shift >= 32)
    return -1;
  else
    return shift;
}

/* Handle a .SUBSPACE pseudo-op; this switches the current subspace to the
   given subspace, creating the new subspace if necessary. 

   FIXME.  Should mirror pa_space more closely, in particular how 
   they're broken up into subroutines.  */

static void
pa_subspace (unused)
     int unused;
{
  char *name, *ss_name, *alias, c;
  char loadable, code_only, common, dup_common, zero, sort;
  int i, access, space_index, alignment, quadrant, applicable, flags;
  sd_chain_struct *space;
  ssd_chain_struct *ssd;
  asection *section;

  if (within_procedure)
    {
      as_bad ("Can\'t change subspaces within a procedure definition. Ignored");
      ignore_rest_of_line ();
    }
  else
    {
      name = input_line_pointer;
      c = get_symbol_end ();
      ss_name = xmalloc (strlen (name) + 1);
      strcpy (ss_name, name);
      *input_line_pointer = c;

      /* Load default values.  */
      sort = 0;
      access = 0x7f;
      loadable = 1;
      common = 0;
      dup_common = 0;
      code_only = 0;
      zero = 0;
      space_index = ~0;
      alignment = 0;
      quadrant = 0;
      alias = NULL;

      space = current_space;
      ssd = is_defined_subspace (ss_name);
      /* Allow user to override the builtin attributes of subspaces.  But
	 only allow the attributes to be changed once!  */
      if (ssd && SUBSPACE_DEFINED (ssd))
	{
	  subseg_set (ssd->ssd_seg, ssd->ssd_subseg);
	  if (!is_end_of_statement ())
	    as_warn ("Parameters of an existing subspace can\'t be modified");
	  demand_empty_rest_of_line ();
	  return;
	}
      else
	{
	  /* A new subspace.  Load default values if it matches one of
	     the builtin subspaces.  */
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
		  alignment = pa_def_subspaces[i].alignment;
		  quadrant = pa_def_subspaces[i].quadrant;
		  access = pa_def_subspaces[i].access;
		  sort = pa_def_subspaces[i].sort;
		  if (USE_ALIASES && pa_def_subspaces[i].alias)
		    alias = pa_def_subspaces[i].alias;
		  break;
		}
	      i++;
	    }
	}

      /* We should be working with a new subspace now.  Fill in
         any information as specified by the user.  */
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
		  quadrant = get_absolute_expression ();
		}
	      else if ((strncasecmp (name, "ALIGN", 5) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  alignment = get_absolute_expression ();
		  if (log2 (alignment) == -1)
		    {
		      as_bad ("Alignment must be a power of 2");
		      alignment = 1;
		    }
		}
	      else if ((strncasecmp (name, "ACCESS", 6) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  access = get_absolute_expression ();
		}
	      else if ((strncasecmp (name, "SORT", 4) == 0))
		{
		  *input_line_pointer = c;
		  input_line_pointer++;
		  sort = get_absolute_expression ();
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
	      else if ((strncasecmp (name, "FIRST", 5) == 0))
		as_bad ("FIRST not supported as a .SUBSPACE argument");
	      else
		as_bad ("Invalid .SUBSPACE argument");
	      if (!is_end_of_statement ())
		input_line_pointer++;
	    }
	}

      /* Compute a reasonable set of BFD flags based on the information
	 in the .subspace directive.  */
      applicable = bfd_applicable_section_flags (stdoutput);
      flags = 0;
      if (loadable)
	flags |= (SEC_ALLOC | SEC_LOAD);
      if (code_only)
	flags |= SEC_CODE;
      if (common || dup_common)
	flags |= SEC_IS_COMMON;

      /* This is a zero-filled subspace (eg BSS).  */
      if (zero)
	flags &= ~SEC_LOAD;

      flags |= SEC_RELOC | SEC_HAS_CONTENTS;
      applicable &= flags;

      /* If this is an existing subspace, then we want to use the 
	 segment already associated with the subspace.

	 FIXME NOW!  ELF BFD doesn't appear to be ready to deal with
	 lots of sections.  It might be a problem in the PA ELF
	 code, I do not know yet.  For now avoid creating anything
	 but the "standard" sections for ELF.  */
      if (ssd)
	section = ssd->ssd_seg;
      else if (alias)
	section = subseg_new (alias, 0);
      else if (! alias && USE_ALIASES)
	{
	  as_warn ("Ignoring subspace decl due to ELF BFD bugs.");
	  demand_empty_rest_of_line ();
	  return;
	}
      else 
	section = subseg_new (ss_name, 0);

      /* Now set the flags.  */
      bfd_set_section_flags (stdoutput, section, applicable);

      /* Record any alignment request for this section.  */
      record_alignment (section, log2 (alignment));

      /* Set the starting offset for this section.  */
      bfd_set_section_vma (stdoutput, section,
			   pa_subspace_start (space, quadrant));
	 
      /* Now that all the flags are set, update an existing subspace,
         or create a new one.  */
      if (ssd)

	current_subspace = update_subspace (space, ss_name, loadable,
					    code_only, common, dup_common,
					    sort, zero, access, space_index,
					    alignment, quadrant, 
					    section);
      else
	current_subspace = create_new_subspace (space, ss_name, loadable,
						code_only, common,
						dup_common, zero, sort,
						access, space_index,
						alignment, quadrant, section);

      demand_empty_rest_of_line ();
      current_subspace->ssd_seg = section;
      subseg_set (current_subspace->ssd_seg, current_subspace->ssd_subseg);
    }
  SUBSPACE_DEFINED (current_subspace) = 1;
  return;
}


/* Create default space and subspace dictionaries.  */

static void 
pa_spaces_begin ()
{
  int i;

  space_dict_root = NULL;
  space_dict_last = NULL;

  i = 0;
  while (pa_def_spaces[i].name)
    {
      char *name;

      /* Pick the right name to use for the new section.  */
      if (pa_def_spaces[i].alias && USE_ALIASES)
	name = pa_def_spaces[i].alias;
      else
        name = pa_def_spaces[i].name;

      pa_def_spaces[i].segment = subseg_new (name, 0);
      create_new_space (pa_def_spaces[i].name, pa_def_spaces[i].spnum,
			pa_def_spaces[i].loadable, pa_def_spaces[i].defined,
			pa_def_spaces[i].private, pa_def_spaces[i].sort,
			pa_def_spaces[i].segment, 0);
      i++;
    }

  i = 0;
  while (pa_def_subspaces[i].name)
    {
      char *name;
      int applicable, subsegment;
      asection *segment = NULL;
      sd_chain_struct *space;

      /* Pick the right name for the new section and pick the right
	 subsegment number.  */
      if (pa_def_subspaces[i].alias && USE_ALIASES)
	{
	  name = pa_def_subspaces[i].alias;
	  subsegment = pa_def_subspaces[i].subsegment;
	}
      else
	{
	  name = pa_def_subspaces[i].name;
	  subsegment = 0;
	}
  
      /* Create the new section.  */
      segment = subseg_new (name, subsegment);


      /* For SOM we want to replace the standard .text, .data, and .bss
	 sections with our own.  */
      if (! strcmp (pa_def_subspaces[i].name, "$CODE$") && ! USE_ALIASES)
	{
	  text_section = segment;
	  applicable = bfd_applicable_section_flags (stdoutput);
	  bfd_set_section_flags (stdoutput, text_section,
				 applicable & (SEC_ALLOC | SEC_LOAD 
					       | SEC_RELOC | SEC_CODE 
					       | SEC_READONLY 
					       | SEC_HAS_CONTENTS));
	}
      else if (! strcmp (pa_def_subspaces[i].name, "$DATA$") && ! USE_ALIASES)
	{
	  data_section = segment;
	  applicable = bfd_applicable_section_flags (stdoutput);
	  bfd_set_section_flags (stdoutput, data_section,
				 applicable & (SEC_ALLOC | SEC_LOAD 
					       | SEC_RELOC
					       | SEC_HAS_CONTENTS));
	  
	  
	}
      else if (! strcmp (pa_def_subspaces[i].name, "$BSS$") && ! USE_ALIASES)
	{
	  bss_section = segment;
	  applicable = bfd_applicable_section_flags (stdoutput);
	  bfd_set_section_flags (stdoutput, bss_section,
				 applicable & SEC_ALLOC);
	}

      /* Find the space associated with this subspace.  */
      space = pa_segment_to_space (pa_def_spaces[pa_def_subspaces[i].
						 def_space_index].segment);
      if (space == NULL)
	{
	  as_fatal ("Internal error: Unable to find containing space for %s.",
		    pa_def_subspaces[i].name);
	}

      create_new_subspace (space, name,
			   pa_def_subspaces[i].loadable,
			   pa_def_subspaces[i].code_only,
			   pa_def_subspaces[i].common,
			   pa_def_subspaces[i].dup_common,
			   pa_def_subspaces[i].zero,
			   pa_def_subspaces[i].sort,
			   pa_def_subspaces[i].access,
			   pa_def_subspaces[i].space_index,
			   pa_def_subspaces[i].alignment,
			   pa_def_subspaces[i].quadrant,
			   segment);
      i++;
    }
}



/* Create a new space NAME, with the appropriate flags as defined
   by the given parameters.

   Add the new space to the space dictionary chain in numerical
   order as defined by the SORT entries.  */

static sd_chain_struct *
create_new_space (name, spnum, loadable, defined, private,
		  sort, seg, user_defined)
     char *name;
     int spnum;
     char loadable;
     char defined;
     char private;
     char sort;
     asection *seg;
     int user_defined;
{
  sd_chain_struct *chain_entry;

  chain_entry = (sd_chain_struct *) xmalloc (sizeof (sd_chain_struct));
  if (!chain_entry)
    as_fatal ("Out of memory: could not allocate new space chain entry: %s\n",
	      name);

  SPACE_NAME (chain_entry) = (char *) xmalloc (strlen (name) + 1);
  strcpy (SPACE_NAME (chain_entry), name);
  SPACE_NAME_INDEX (chain_entry) = 0;
  SPACE_LOADABLE (chain_entry) = loadable;
  SPACE_DEFINED (chain_entry) = defined;
  SPACE_USER_DEFINED (chain_entry) = user_defined;
  SPACE_PRIVATE (chain_entry) = private;
  SPACE_SPNUM (chain_entry) = spnum;
  SPACE_SORT (chain_entry) = sort;

  chain_entry->sd_seg = seg;
  chain_entry->sd_last_subseg = -1;
  chain_entry->sd_next = NULL;

  /* Find spot for the new space based on its sort key.  */
  if (!space_dict_last)
    space_dict_last = chain_entry;

  if (space_dict_root == NULL)
    space_dict_root = chain_entry;
  else
    {
      sd_chain_struct *chain_pointer;
      sd_chain_struct *prev_chain_pointer;

      chain_pointer = space_dict_root;
      prev_chain_pointer = NULL;

      while (chain_pointer)
	{
	  if (SPACE_SORT (chain_pointer) <= SPACE_SORT (chain_entry))
	    {
	      prev_chain_pointer = chain_pointer;
	      chain_pointer = chain_pointer->sd_next;
	    }
	  else
	    break;
	}

      /* At this point we've found the correct place to add the new
         entry.  So add it and update the linked lists as appropriate.  */
      if (prev_chain_pointer)
	{
	  chain_entry->sd_next = chain_pointer;
	  prev_chain_pointer->sd_next = chain_entry;
	}
      else
	{
	  space_dict_root = chain_entry;
	  chain_entry->sd_next = chain_pointer;
	}

      if (chain_entry->sd_next == NULL)
	space_dict_last = chain_entry;
    }

  /* This is here to catch predefined spaces which do not get
     modified by the user's input.  Another call is found at
     the bottom of pa_parse_space_stmt to handle cases where
     the user modifies a predefined space.  */
#ifdef obj_set_section_attributes
  obj_set_section_attributes (seg, defined, private, sort, spnum);
#endif

  return chain_entry;
}

/* Create a new subspace NAME, with the appropriate flags as defined
   by the given parameters.

   Add the new subspace to the subspace dictionary chain in numerical
   order as defined by the SORT entries.  */

static ssd_chain_struct *
create_new_subspace (space, name, loadable, code_only, common,
		     dup_common, is_zero, sort, access, space_index,
		     alignment, quadrant, seg)
     sd_chain_struct *space;
     char *name;
     char loadable, code_only, common, dup_common, is_zero;
     char sort;
     int access;
     int space_index;
     int alignment;
     int quadrant;
     asection *seg;
{
  ssd_chain_struct *chain_entry;

  chain_entry = (ssd_chain_struct *) xmalloc (sizeof (ssd_chain_struct));
  if (!chain_entry)
    as_fatal ("Out of memory: could not allocate new subspace chain entry: %s\n", name);

  SUBSPACE_NAME (chain_entry) = (char *) xmalloc (strlen (name) + 1);
  strcpy (SUBSPACE_NAME (chain_entry), name);

  SUBSPACE_ACCESS (chain_entry) = access;
  SUBSPACE_LOADABLE (chain_entry) = loadable;
  SUBSPACE_COMMON (chain_entry) = common;
  SUBSPACE_DUP_COMM (chain_entry) = dup_common;
  SUBSPACE_SORT (chain_entry) = sort;
  SUBSPACE_CODE_ONLY (chain_entry) = code_only;
  SUBSPACE_ALIGN (chain_entry) = alignment;
  SUBSPACE_QUADRANT (chain_entry) = quadrant;
  SUBSPACE_SUBSPACE_START (chain_entry) = pa_subspace_start (space, quadrant);
  SUBSPACE_SPACE_INDEX (chain_entry) = space_index;
  SUBSPACE_ZERO (chain_entry) = is_zero;

  chain_entry->ssd_subseg = USE_ALIASES ? pa_next_subseg (space) : 0;
  chain_entry->ssd_seg = seg;
  chain_entry->ssd_last_align = 1;
  chain_entry->ssd_next = NULL;

  /* Find spot for the new subspace based on its sort key.  */
  if (space->sd_subspaces == NULL)
    space->sd_subspaces = chain_entry;
  else
    {
      ssd_chain_struct *chain_pointer;
      ssd_chain_struct *prev_chain_pointer;

      chain_pointer = space->sd_subspaces;
      prev_chain_pointer = NULL;

      while (chain_pointer)
	{
	  if (SUBSPACE_SORT (chain_pointer) <= SUBSPACE_SORT (chain_entry))
	    {
	      prev_chain_pointer = chain_pointer;
	      chain_pointer = chain_pointer->ssd_next;
	    }
	  else
	    break;

	}

      /* Now we have somewhere to put the new entry.  Insert it and update
         the links.  */
      if (prev_chain_pointer)
	{
	  chain_entry->ssd_next = chain_pointer;
	  prev_chain_pointer->ssd_next = chain_entry;
	}
      else
	{
	  space->sd_subspaces = chain_entry;
	  chain_entry->ssd_next = chain_pointer;
	}
    }

#ifdef obj_set_subsection_attributes
  obj_set_subsection_attributes (seg, space->sd_seg, access, 
				 sort, quadrant);
#endif

  return chain_entry;

}

/* Update the information for the given subspace based upon the
   various arguments.   Return the modified subspace chain entry.  */

static ssd_chain_struct *
update_subspace (space, name, loadable, code_only, common, dup_common, sort,
		 zero, access, space_index, alignment, quadrant, subseg)
     sd_chain_struct *space;
     char *name;
     char loadable;
     char code_only;
     char common;
     char dup_common;
     char zero;
     char sort;
     int access;
     int space_index;
     int alignment;
     int quadrant;
     subsegT subseg;
{
  ssd_chain_struct *chain_entry;

  if ((chain_entry = is_defined_subspace (name)))
    {
      SUBSPACE_ACCESS (chain_entry) = access;
      SUBSPACE_LOADABLE (chain_entry) = loadable;
      SUBSPACE_COMMON (chain_entry) = common;
      SUBSPACE_DUP_COMM (chain_entry) = dup_common;
      SUBSPACE_CODE_ONLY (chain_entry) = 1;
      SUBSPACE_SORT (chain_entry) = sort;
      SUBSPACE_ALIGN (chain_entry) = alignment;
      SUBSPACE_QUADRANT (chain_entry) = quadrant;
      SUBSPACE_SPACE_INDEX (chain_entry) = space_index;
      SUBSPACE_ZERO (chain_entry) = zero;
    }
  else
    chain_entry = NULL;

#ifdef obj_set_subsection_attributes
  obj_set_subsection_attributes (subseg, space->sd_seg, access, 
				 sort, quadrant);
#endif

  return chain_entry;

}

/* Return the space chain entry for the space with the name NAME or
   NULL if no such space exists.  */

static sd_chain_struct *
is_defined_space (name)
     char *name;
{
  sd_chain_struct *chain_pointer;

  for (chain_pointer = space_dict_root;
       chain_pointer;
       chain_pointer = chain_pointer->sd_next)
    {
      if (strcmp (SPACE_NAME (chain_pointer), name) == 0)
	return chain_pointer;
    }

  /* No mapping from segment to space was found.  Return NULL.  */
  return NULL;
}

/* Find and return the space associated with the given seg.  If no mapping 
   from the given seg to a space is found, then return NULL.

   Unlike subspaces, the number of spaces is not expected to grow much,
   so a linear exhaustive search is OK here.  */

static sd_chain_struct *
pa_segment_to_space (seg)
     asection *seg;
{
  sd_chain_struct *space_chain;

  /* Walk through each space looking for the correct mapping.  */
  for (space_chain = space_dict_root;
       space_chain;
       space_chain = space_chain->sd_next)
    {
      if (space_chain->sd_seg == seg)
	return space_chain;
    }

  /* Mapping was not found.  Return NULL.  */
  return NULL;
}

/* Return the space chain entry for the subspace with the name NAME or
   NULL if no such subspace exists.

   Uses a linear search through all the spaces and subspaces, this may
   not be appropriate if we ever being placing each function in its
   own subspace.  */

static ssd_chain_struct *
is_defined_subspace (name)
     char *name;
{
  sd_chain_struct*space_chain;
  ssd_chain_struct *subspace_chain;

  /* Walk through each space.  */
  for (space_chain = space_dict_root;
       space_chain;
       space_chain = space_chain->sd_next)
    {
      /* Walk through each subspace looking for a name which matches.  */
      for (subspace_chain = space_chain->sd_subspaces;
	   subspace_chain;
	   subspace_chain = subspace_chain->ssd_next)
	if (strcmp (SUBSPACE_NAME (subspace_chain), name) == 0)
	  return subspace_chain;
    }

  /* Subspace wasn't found.  Return NULL.  */
  return NULL;
}

/* Find and return the subspace associated with the given seg.  If no
   mapping from the given seg to a subspace is found, then return NULL.

   If we ever put each procedure/function within its own subspace 
   (to make life easier on the compiler and linker), then this will have
   to become more efficient.  */

static ssd_chain_struct *
pa_subsegment_to_subspace (seg, subseg)
     asection *seg;
     subsegT subseg;
{
  sd_chain_struct *space_chain;
  ssd_chain_struct *subspace_chain;

  /* Walk through each space.  */
  for (space_chain = space_dict_root;
       space_chain;
       space_chain = space_chain->sd_next)
    {
      if (space_chain->sd_seg == seg)
	{
	  /* Walk through each subspace within each space looking for
	     the correct mapping.  */
	  for (subspace_chain = space_chain->sd_subspaces;
	       subspace_chain;
	       subspace_chain = subspace_chain->ssd_next)
	    if (subspace_chain->ssd_subseg == (int) subseg)
	      return subspace_chain;
	}
    }

  /* No mapping from subsegment to subspace found.  Return NULL.  */
  return NULL;
}

/* Given a number, try and find a space with the name number.  

   Return a pointer to a space dictionary chain entry for the space
   that was found or NULL on failure.  */

static sd_chain_struct *
pa_find_space_by_number (number)
     int number;
{
  sd_chain_struct *space_chain;

  for (space_chain = space_dict_root;
       space_chain;
       space_chain = space_chain->sd_next)
    {
      if (SPACE_SPNUM (space_chain) == number)
	return space_chain;
    }

  /* No appropriate space found.  Return NULL.  */
  return NULL;
}

/* Return the starting address for the given subspace.  If the starting
   address is unknown then return zero.  */

static unsigned int
pa_subspace_start (space, quadrant)
     sd_chain_struct *space;
     int quadrant;
{
  /* FIXME.  Assumes everyone puts read/write data at 0x4000000, this
     is not correct for the PA OSF1 port.  */
  if ((strcasecmp (SPACE_NAME (space), "$PRIVATE$") == 0) && quadrant == 1)
    return 0x40000000;
  else if (space->sd_seg == data_section && quadrant == 1)
    return 0x40000000;
  else
    return 0;
}

/* FIXME.  Needs documentation.  */
static int
pa_next_subseg (space)
     sd_chain_struct *space;
{

  space->sd_last_subseg++;
  return space->sd_last_subseg;
}

/* Helper function for pa_stringer.  Used to find the end of 
   a string.  */

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

/* Handle a .STRING type pseudo-op.  */

static void
pa_stringer (append_zero)
     int append_zero;
{
  char *s, num_buf[4];
  unsigned int c;
  int i;

  /* Preprocess the string to handle PA-specific escape sequences.
     For example, \xDD where DD is a hexidecimal number should be 
     changed to \OOO where OOO is an octal number.  */

  /* Skip the opening quote.  */
  s = input_line_pointer + 1;

  while (is_a_char (c = pa_stringer_aux (s++)))
    {
      if (c == '\\')
	{
	  c = *s;
	  switch (c)
	    {
	      /* Handle \x<num>.  */
	    case 'x':
	      {
		unsigned int number;
		int num_digit;
		char dg;
		char *s_start = s;

		/* Get pas the 'x'.  */
		s++;
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
		break;
	      }
	    /* This might be a "\"", skip over the escaped char.  */
	    default:
	      s++;
	      break;
	    }
	}
    }
  stringer (append_zero);
  pa_undefine_label ();
}

/* Handle a .VERSION pseudo-op.  */

static void
pa_version (unused)
     int unused;
{
  obj_version (0);
  pa_undefine_label ();
}

/* Just like a normal cons, but when finished we have to undefine
   the latest space label.  */

static void
pa_cons (nbytes)
     int nbytes;
{
  cons (nbytes);
  pa_undefine_label ();
}

/* Switch to the data space.  As usual delete our label.  */

static void
pa_data (unused)
     int unused;
{
  s_data (0);
  pa_undefine_label ();
}

/* FIXME.  What's the purpose of this pseudo-op?  */

static void
pa_desc (unused)
     int unused;
{
  pa_undefine_label ();
}

/* Like float_cons, but we need to undefine our label.  */
 
static void
pa_float_cons (float_type)
     int float_type;
{
  float_cons (float_type);
  pa_undefine_label ();
}

/* Like s_fill, but delete our label when finished.  */

static void
pa_fill (unused)
     int unused;
{
  s_fill (0);
  pa_undefine_label ();
}

/* Like lcomm, but delete our label when finished.  */

static void
pa_lcomm (needs_align)
     int needs_align;
{
  s_lcomm (needs_align);
  pa_undefine_label ();
}

/* Like lsym, but delete our label when finished.  */

static void
pa_lsym (unused)
     int unused;
{
  s_lsym (0);
  pa_undefine_label ();
}

/* Switch to the text space.  Like s_text, but delete our 
   label when finished.  */
static void
pa_text (unused)
     int unused;
{
  s_text (0);
  pa_undefine_label ();
}

/* On the PA relocations which involve function symbols must not be 
   adjusted.  This so that the linker can know when/how to create argument
   relocation stubs for indirect calls and calls to static functions.

   FIXME.  Also reject R_HPPA relocations which are 32 bits
   wide.  Helps with code lables in arrays for SOM.  (SOM BFD code
   needs to generate relocations to push the addend and symbol value
   onto the stack, add them, then pop the value off the stack and
   use it in a relocation -- yuk.  */

int
hppa_fix_adjustable (fixp) 
     fixS *fixp;
{
  struct hppa_fix_struct *hppa_fix;

  hppa_fix = fixp->tc_fix_data;

  if (fixp->fx_r_type == R_HPPA && hppa_fix->fx_r_format == 32)
    return 0;

  if (fixp->fx_addsy == 0 
      || (fixp->fx_addsy->bsym->flags & BSF_FUNCTION) == 0)
    return 1;

  return 0;
}
  
/* Now for some ELF specific code.  FIXME.  */
#ifdef OBJ_ELF
static symext_chainS *symext_rootP;
static symext_chainS *symext_lastP;

/* Do any symbol processing requested by the target-cpu or target-format.  */

void
hppa_tc_symbol (abfd, symbolP, sym_idx)
     bfd *abfd;
     elf_symbol_type *symbolP;
     int sym_idx;
{
  symext_chainS *symextP;
  unsigned int arg_reloc;

  /* Only functions can have argument relocations.  */
  if (!(symbolP->symbol.flags & BSF_FUNCTION))
    return;

  arg_reloc = symbolP->tc_data.hppa_arg_reloc;

  /* If there are no argument relocation bits, then no relocation is
     necessary.  Do not add this to the symextn section.  */
  if (arg_reloc == 0)
    return;

  symextP = (symext_chainS *) bfd_alloc (abfd, sizeof (symext_chainS) * 2);

  symextP[0].entry = ELF32_HPPA_SX_WORD (HPPA_SXT_SYMNDX, sym_idx);
  symextP[0].next = &symextP[1];

  symextP[1].entry = ELF32_HPPA_SX_WORD (HPPA_SXT_ARG_RELOC, arg_reloc);
  symextP[1].next = NULL;

  if (symext_rootP == NULL)
    {
      symext_rootP = &symextP[0];
      symext_lastP = &symextP[1];
    }
  else
    {
      symext_lastP->next = &symextP[0];
      symext_lastP = &symextP[1];
    }
}

/* Make sections needed by the target cpu and/or target format.  */
void
hppa_tc_make_sections (abfd)
     bfd *abfd;
{
  symext_chainS *symextP;
  int size, n;
  asection *symextn_sec;
  segT save_seg = now_seg;
  subsegT save_subseg = now_subseg;

  /* Build the symbol extension section.  */
  hppa_tc_make_symextn_section ();

  /* Force some calculation to occur.  */
  bfd_set_section_contents (stdoutput, stdoutput->sections, "", 0, 0);

  hppa_elf_stub_finish (abfd);

  /* If no symbols for the symbol extension section, then stop now.  */
  if (symext_rootP == NULL)
    return;

  /* Count the number of symbols for the symbol extension section.  */
  for (n = 0, symextP = symext_rootP; symextP; symextP = symextP->next, ++n)
    ;

  size = sizeof (symext_entryS) * n;

  /* Switch to the symbol extension section.  */
  symextn_sec = subseg_new (SYMEXTN_SECTION_NAME, 0);

  frag_wane (frag_now);
  frag_new (0);

  for (symextP = symext_rootP; symextP; symextP = symextP->next)
    {
      char *ptr;
      int *symtab_map = elf_sym_extra (abfd);
      int idx;

      /* First, patch the symbol extension record to reflect the true
         symbol table index.  */

      if (ELF32_HPPA_SX_TYPE (symextP->entry) == HPPA_SXT_SYMNDX)
	{
	  idx = ELF32_HPPA_SX_VAL (symextP->entry) - 1;
	  symextP->entry = ELF32_HPPA_SX_WORD (HPPA_SXT_SYMNDX,
					       symtab_map[idx]);
	}

      ptr = frag_more (sizeof (symextP->entry));
      md_number_to_chars (ptr, symextP->entry, sizeof (symextP->entry));
    }

  frag_now->fr_fix = obstack_next_free (&frags) - frag_now->fr_literal;
  frag_wane (frag_now);

  /* Switch back to the original segment.  */
  subseg_set (save_seg, save_subseg);

  return;
}

/* Make the symbol extension section.  */

static void
hppa_tc_make_symextn_section ()
{
  if (symext_rootP)
    {
      symext_chainS *symextP;
      int n;
      unsigned int size;
      segT symextn_sec;
      segT save_seg = now_seg;
      subsegT save_subseg = now_subseg;

      for (n = 0, symextP = symext_rootP; symextP; symextP = symextP->next, ++n)
	;

      size = sizeof (symext_entryS) * n;

      symextn_sec = subseg_new (SYMEXTN_SECTION_NAME, 0);

      bfd_set_section_flags (stdoutput, symextn_sec,
			     SEC_LOAD | SEC_HAS_CONTENTS | SEC_DATA);
      bfd_set_section_size (stdoutput, symextn_sec, size);

      /* Now, switch back to the original segment.  */
      subseg_set (save_seg, save_subseg);
    }
}

/* Build the symbol extension section.  */

static void
pa_build_symextn_section ()
{
  segT seg;
  asection *save_seg = now_seg;
  subsegT subseg = (subsegT) 0;
  subsegT save_subseg = now_subseg;

  seg = subseg_new (".hppa_symextn", subseg);
  bfd_set_section_flags (stdoutput,
			 seg,
			 SEC_HAS_CONTENTS | SEC_READONLY 
			 | SEC_ALLOC | SEC_LOAD);

  subseg_set (save_seg, save_subseg);

}

/* For ELF, this function serves one purpose:  to setup the st_size
   field of STT_FUNC symbols.  To do this, we need to scan the
   call_info structure list, determining st_size in one of two possible
   ways:

   1. call_info->start_frag->fr_fix has the size of the fragment.
   This approach assumes that the function was built into a
   single fragment.  This works for most cases, but might fail.
   For example, if there was a segment change in the middle of
   the function.

   2. The st_size field is the difference in the addresses of the
   call_info->start_frag->fr_address field and the fr_address
   field of the next fragment with fr_type == rs_fill and
   fr_fix != 0.  */

void
elf_hppa_final_processing ()
{
  struct call_info *call_info_pointer;

  for (call_info_pointer = call_info_root;
       call_info_pointer;
       call_info_pointer = call_info_pointer->ci_next)
    {
      elf_symbol_type *esym
	= (elf_symbol_type *) call_info_pointer->start_symbol->bsym;
      esym->internal_elf_sym.st_size =
	S_GET_VALUE (call_info_pointer->end_symbol)
	  - S_GET_VALUE (call_info_pointer->start_symbol) + 4;
    }
}
#endif
