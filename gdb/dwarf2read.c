/* DWARF 2 debugging format support for GDB.
   Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001
   Free Software Foundation, Inc.

   Adapted by Gary Funck (gary@intrepid.com), Intrepid Technology,
   Inc.  with support from Florida State University (under contract
   with the Ada Joint Program Office), and Silicon Graphics, Inc.
   Initial contribution by Brent Benson, Harris Computer Systems, Inc.,
   based on Fred Fish's (Cygnus Support) implementation of DWARF 1
   support in dwarfread.c
   Largely rewritten by Daniel Berlin (dan@cgsoftware.com)

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "bfd.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "symfile.h"
#include "objfiles.h"
#include "elf/dwarf2.h"
#include "buildsym.h"
#include "demangle.h"
#include "expression.h"
#include "filenames.h"	/* for DOSish file names */

#include "language.h"
#include "complaints.h"
#include "bcache.h"
#include "frame.h"
#include <fcntl.h>
#include "gdb_string.h"
#include <sys/types.h>
#include <search.h>
#ifndef DWARF2_REG_TO_REGNUM
#define DWARF2_REG_TO_REGNUM(REG) (REG)
#endif

/* .debug_pubnames header
   Because of alignment constraints, this structure has padding and cannot
   be mapped directly onto the beginning of the .debug_info section.  */
typedef struct pubnames_header
{
  unsigned int length;		/* length of the .debug_pubnames
				   contribution  */
  unsigned char version;	/* version number -- 2 for DWARF
				   version 2 */
  unsigned int info_offset;	/* offset into .debug_info section */
  unsigned int info_size;	/* byte size of .debug_info section
				   portion */
}
_PUBNAMES_HEADER;
#define _ACTUAL_PUBNAMES_HEADER_SIZE 13

/* .debug_pubnames header
   Because of alignment constraints, this structure has padding and cannot
   be mapped directly onto the beginning of the .debug_info section.  */
typedef struct aranges_header
{
  unsigned int length;		/* byte len of the .debug_aranges
				   contribution */
  unsigned short version;	/* version number -- 2 for DWARF
				   version 2 */
  unsigned int info_offset;	/* offset into .debug_info section */
  unsigned char addr_size;	/* byte size of an address */
  unsigned char seg_size;	/* byte size of segment descriptor */
}
_ARANGES_HEADER;
#define _ACTUAL_ARANGES_HEADER_SIZE 12

/* .debug_line statement program prologue
   Because of alignment constraints, this structure has padding and cannot
   be mapped directly onto the beginning of the .debug_info section.  */
typedef struct statement_prologue
{
  unsigned int total_length;	/* byte length of the statement
				   information */
  unsigned short version;	/* version number -- 2 for DWARF
				   version 2 */
  unsigned int prologue_length;	/* # bytes between prologue &
				   stmt program */
  unsigned char minimum_instruction_length;	/* byte size of
						   smallest instr */
  unsigned char default_is_stmt;	/* initial value of is_stmt
					   register */
  char line_base;
  unsigned char line_range;
  unsigned char opcode_base;	/* number assigned to first special
				   opcode */
  unsigned char *standard_opcode_lengths;
}
_STATEMENT_PROLOGUE;

/* offsets and sizes of debugging sections */

static file_ptr dwarf2_info_offset;
static file_ptr dwarf2_abbrev_offset;
static file_ptr dwarf2_line_offset;
static file_ptr dwarf2_pubnames_offset;
static file_ptr dwarf2_aranges_offset;
static file_ptr dwarf2_loc_offset;
static file_ptr dwarf2_macinfo_offset;
static file_ptr dwarf2_str_offset;
static file_ptr dwarf2_frame_offset;

static unsigned int dwarf2_info_size;
static unsigned int dwarf2_abbrev_size;
static unsigned int dwarf2_line_size;
static unsigned int dwarf2_pubnames_size;
static unsigned int dwarf2_aranges_size;
static unsigned int dwarf2_loc_size;
static unsigned int dwarf2_macinfo_size;
static unsigned int dwarf2_str_size;
static unsigned int dwarf2_frame_size;

/* names of the debugging sections */

#define INFO_SECTION     ".debug_info"
#define ABBREV_SECTION   ".debug_abbrev"
#define LINE_SECTION     ".debug_line"
#define PUBNAMES_SECTION ".debug_pubnames"
#define ARANGES_SECTION  ".debug_aranges"
#define LOC_SECTION      ".debug_loc"
#define MACINFO_SECTION  ".debug_macinfo"
#define STR_SECTION      ".debug_str"
#define FRAME_SECTION    ".debug_frame"

/* local data types */

/* The data in a compilation unit header, after target2host
   translation, looks like this.  */
struct comp_unit_head
{
  unsigned long length;
  short version;
  unsigned int abbrev_offset;
  unsigned char addr_size;
  unsigned char signed_addr_p;
  unsigned int offset_size;	/* size of file offsets; either 4 or 8 */
  unsigned int initial_length_size;	/* size of the length field; either
					   4 or 12 */
};

/* The data in the .debug_line statement prologue looks like this.  */
struct line_head
{
  unsigned int total_length;
  unsigned short version;
  unsigned int prologue_length;
  unsigned char minimum_instruction_length;
  unsigned char default_is_stmt;
  int line_base;
  unsigned char line_range;
  unsigned char opcode_base;
  unsigned char *standard_opcode_lengths;
};

/* When we construct a partial symbol table entry we only
   need this much information. */
struct partial_die_info
{
  enum dwarf_tag tag;
  unsigned char has_children;
  unsigned char is_external;
  unsigned char is_declaration;
  unsigned char has_type;
  unsigned int offset;
  unsigned int abbrev;
  char *name;
  CORE_ADDR lowpc;
  CORE_ADDR highpc;
  struct dwarf2_block *locdesc;
  unsigned int language;
  char *sibling;
};

/* This data structure holds the information of an abbrev. */
struct abbrev_info
{
  unsigned int number;		/* number identifying abbrev */
  enum dwarf_tag tag;		/* dwarf tag */
  int has_children;		/* boolean */
  unsigned int num_attrs;	/* number of attributes */
  struct attr_abbrev *attrs;	/* an array of attribute descriptions */
  struct abbrev_info *next;	/* next in chain */
};

struct attr_abbrev
{
  enum dwarf_attribute name;
  enum dwarf_form form;
};

/* This data structure holds a complete die structure. */
struct die_info
{
  enum dwarf_tag tag;		/* Tag indicating type of die */
  unsigned short has_children;	/* Does the die have children */
  unsigned int abbrev;		/* Abbrev number */
  unsigned int offset;		/* Offset in .debug_info section */
  unsigned int num_attrs;	/* Number of attributes */
  struct attribute *attrs;	/* An array of attributes */
  struct die_info *next_ref;	/* Next die in ref hash table */
  struct die_info *next;	/* Next die in linked list */
  struct type *type;		/* Cached type information */
};

/* Attributes have a name and a value */
struct attribute
{
  enum dwarf_attribute name;
  enum dwarf_form form;
  union
  {
    char *str;
    struct dwarf2_block *blk;
    unsigned long unsnd;
    long int snd;
    CORE_ADDR addr;
  }
  u;
};

/* Get at parts of an attribute structure */

#define DW_STRING(attr)    ((attr)->u.str)
#define DW_UNSND(attr)     ((attr)->u.unsnd)
#define DW_BLOCK(attr)     ((attr)->u.blk)
#define DW_SND(attr)       ((attr)->u.snd)
#define DW_ADDR(attr)	   ((attr)->u.addr)

/* Blocks are a bunch of untyped bytes. */
struct dwarf2_block
{
  unsigned int size;
  char *data;
};
struct comp_unit_instance
{
  bfd *abfd;
  int offset;
  struct comp_unit_head cu_header;
  int cu_header_offset;

  /* We only hold one compilation unit's abbrevs in
     memory at any one time.  */
#ifndef ABBREV_HASH_SIZE
#define ABBREV_HASH_SIZE 121
#endif
#ifndef ATTR_ALLOC_CHUNK
#define ATTR_ALLOC_CHUNK 4
#endif

  struct abbrev_info *dwarf2_abbrevs[ABBREV_HASH_SIZE];

  /* A hash table of die offsets for following references.  */
#ifndef REF_HASH_SIZE
#define REF_HASH_SIZE 1021
#endif

  struct die_info *die_ref_table[REF_HASH_SIZE];



  /* Allocate fields for structs, unions and enums in this size.  */
#ifndef DW_FIELD_ALLOC_CHUNK
#define DW_FIELD_ALLOC_CHUNK 4
#endif

  /* The language we are debugging.  */
  enum language cu_language;
  const struct language_defn *cu_language_defn;

  struct comp_unit_instance *next;
};

#ifndef COMP_UNIT_HASH_SIZE
#define COMP_UNIT_HASH_SIZE  121
#endif

static struct comp_unit_instance *compilation_units[COMP_UNIT_HASH_SIZE];

static struct obstack dwarf2_tmp_obstack;
/* Actually data from the sections.  */
static char *dwarf2_info_buffer;
static char *dwarf2_abbrev_buffer;
static char *dwarf2_line_buffer;
static char *dwarf2_frame_buffer;

/* The generic symbol table building routines have separate lists for
   file scope symbols and all all other scopes (local scopes).  So
   we need to select the right one to pass to add_symbol_to_list().
   We do it by keeping a pointer to the correct list in list_in_scope.

   FIXME:  The original dwarf code just treated the file scope as the first
   local scope, and all other local scopes as nested local scopes, and worked
   fine.  Check to see if we really need to distinguish these
   in buildsym.c.  */
static struct pending **list_in_scope = &file_symbols;

/* FIXME: decode_locdesc sets these variables to describe the location
   to the caller.  These ought to be a structure or something.   If
   none of the flags are set, the object lives at the address returned
   by decode_locdesc.  */

static int optimized_out;	/* No ops in location in expression,
				   so object was optimized out.  */
static int isreg;		/* Object lives in register.
				   decode_locdesc's return value is
				   the register number.  */
static int offreg;		/* Object's address is the sum of the
				   register specified by basereg, plus
				   the offset returned.  */
static int basereg;		/* See `offreg'.  */
static int isderef;		/* Value described by flags above is
				   the address of a pointer to the object.  */
static int islocal;		/* Variable is at the returned offset
				   from the frame start, but there's
				   no identified frame pointer for
				   this function, so we can't say
				   which register it's relative to;
				   use LOC_LOCAL.  */

/* DW_AT_frame_base values for the current function.
   frame_base_reg is -1 if DW_AT_frame_base is missing, otherwise it
   contains the register number for the frame register.
   frame_base_offset is the offset from the frame register to the
   virtual stack frame. */
static int frame_base_reg;
static CORE_ADDR frame_base_offset;

/* This value is added to each symbol value.  FIXME:  Generalize to
   the section_offsets structure used by dbxread (once this is done,
   pass the appropriate section number to end_symtab).  */
static CORE_ADDR baseaddr;	/* Add to each symbol value */

/* We put a pointer to this structure in the read_symtab_private field
   of the psymtab.
   The complete dwarf information for an objfile is kept in the
   psymbol_obstack, so that absolute die references can be handled.
   Most of the information in this structure is related to an entire
   object file and could be passed via the sym_private field of the objfile.
   It is however conceivable that dwarf2 might not be the only type
   of symbols read from an object file.  */

struct dwarf2_pinfo
{
  /* Pointer to start of dwarf info buffer for the objfile.  */

  char *dwarf2_info_buffer;

  /* Offset in dwarf2_info_buffer for this compilation unit. */

  unsigned long dwarf2_info_offset;

  /* Pointer to start of dwarf abbreviation buffer for the objfile.  */

  char *dwarf2_abbrev_buffer;

  /* Size of dwarf abbreviation section for the objfile.  */

  unsigned int dwarf2_abbrev_size;

  /* Pointer to start of dwarf line buffer for the objfile.  */

  char *dwarf2_line_buffer;

  char *dwarf2_frame_buffer;
 
};

#define PST_PRIVATE(p) ((struct dwarf2_pinfo *)(p)->read_symtab_private)
#define DWARF2_INFO_BUFFER(p) (PST_PRIVATE(p)->dwarf2_info_buffer)
#define DWARF2_INFO_OFFSET(p) (PST_PRIVATE(p)->dwarf2_info_offset)
#define DWARF2_ABBREV_BUFFER(p) (PST_PRIVATE(p)->dwarf2_abbrev_buffer)
#define DWARF2_ABBREV_SIZE(p) (PST_PRIVATE(p)->dwarf2_abbrev_size)
#define DWARF2_FRAME_BUFFER(p) (PST_PRIVATE(p)->dwarf2_frame_buffer)
#define DWARF2_LINE_BUFFER(p) (PST_PRIVATE(p)->dwarf2_line_buffer)
/* Maintain an array of referenced fundamental types for the current
   compilation unit being read.  For DWARF version 1, we have to construct
   the fundamental types on the fly, since no information about the
   fundamental types is supplied.  Each such fundamental type is created by
   calling a language dependent routine to create the type, and then a
   pointer to that type is then placed in the array at the index specified
   by it's FT_<TYPENAME> value.  The array has a fixed size set by the
   FT_NUM_MEMBERS compile time constant, which is the number of predefined
   fundamental types gdb knows how to construct.  */
static struct type *ftypes[FT_NUM_MEMBERS];	/* Fundamental types */

/* FIXME: We might want to set this from BFD via bfd_arch_bits_per_byte,
   but this would require a corresponding change in unpack_field_as_long
   and friends.  */
static int bits_per_byte = 8;

/* The routines that read and process dies for a C struct or C++ class
   pass lists of data member fields and lists of member function fields
   in an instance of a field_info structure, as defined below.  */
struct field_info
{
  /* List of data member and baseclasses fields. */
  struct nextfield
  {
    struct nextfield *next;
    int accessibility;
    int virtuality;
    struct field field;
  }
   *fields;

  /* Number of fields.  */
  int nfields;

  /* Number of baseclasses.  */
  int nbaseclasses;

  /* Set if the accesibility of one of the fields is not public.  */
  int non_public_fields;

  /* Member function fields array, entries are allocated in the order they
     are encountered in the object file.  */
  struct nextfnfield
  {
    struct nextfnfield *next;
    struct fn_field fnfield;
  }
   *fnfields;

  /* Member function fieldlist array, contains name of possibly overloaded
     member function, number of overloaded member functions and a pointer
     to the head of the member function field chain.  */
  struct fnfieldlist
  {
    char *name;
    int length;
    struct nextfnfield *head;
  }
   *fnfieldlists;

  /* Number of entries in the fnfieldlists array.  */
  int nfnfields;
};

/* FIXME: Kludge to mark a varargs function type for C++ member function
   argument processing.  */
#define TYPE_FLAG_VARARGS	(1 << 10)

/* Dwarf2 has no clean way to discern C++ static and non-static member
   functions. G++ helps GDB by marking the first parameter for non-static
   member functions (which is the this pointer) as artificial.
   We pass this information between dwarf2_add_member_fn and
   read_subroutine_type via TYPE_FIELD_ARTIFICIAL.  */
#define TYPE_FIELD_ARTIFICIAL	TYPE_FIELD_BITPOS

/* Various complaints about symbol reading that don't abort the process */

static struct complaint dwarf2_const_ignored = {
  "type qualifier 'const' ignored", 0, 0
};
static struct complaint dwarf2_volatile_ignored = {
  "type qualifier 'volatile' ignored", 0, 0
};
static struct complaint dwarf2_non_const_array_bound_ignored = {
  "non-constant array bounds form '%s' ignored", 0, 0
};
static struct complaint dwarf2_missing_line_number_section = {
  "missing .debug_line section", 0, 0
};
static struct complaint dwarf2_mangled_line_number_section = {
  "mangled .debug_line section", 0, 0
};
static struct complaint dwarf2_unsupported_die_ref_attr = {
  "unsupported die ref attribute form: '%s'", 0, 0
};
static struct complaint dwarf2_unsupported_stack_op = {
  "unsupported stack op: '%s'", 0, 0
};
static struct complaint dwarf2_complex_location_expr = {
  "location expression too complex", 0, 0
};
static struct complaint dwarf2_unsupported_tag = {
  "unsupported tag: '%s'", 0, 0
};
static struct complaint dwarf2_unsupported_at_encoding = {
  "unsupported DW_AT_encoding: '%s'", 0, 0
};
static struct complaint dwarf2_unsupported_at_frame_base = {
  "unsupported DW_AT_frame_base for function '%s'", 0, 0
};
static struct complaint dwarf2_unexpected_tag = {
  "unexepected tag in read_type_die: '%s'", 0, 0
};
static struct complaint dwarf2_missing_at_frame_base = {
  "DW_AT_frame_base missing for DW_OP_fbreg", 0, 0
};
static struct complaint dwarf2_bad_static_member_name = {
  "unrecognized static data member name '%s'", 0, 0
};
static struct complaint dwarf2_unsupported_accessibility = {
  "unsupported accessibility %d", 0, 0
};
static struct complaint dwarf2_bad_member_name_complaint = {
  "cannot extract member name from '%s'", 0, 0
};
static struct complaint dwarf2_missing_member_fn_type_complaint = {
  "member function type missing for '%s'", 0, 0
};
static struct complaint dwarf2_vtbl_not_found_complaint = {
  "virtual function table pointer not found when defining class '%s'", 0, 0
};
static struct complaint dwarf2_absolute_sibling_complaint = {
  "ignoring absolute DW_AT_sibling", 0, 0
};
static struct complaint dwarf2_const_value_length_mismatch = {
  "const value length mismatch for '%s', got %d, expected %d", 0, 0
};
static struct complaint dwarf2_unsupported_const_value_attr = {
  "unsupported const value attribute form: '%s'", 0, 0
};

/* Externals references.  */
extern int info_verbose;	/* From main.c; nonzero => verbose */

/* local function prototypes */

static void dwarf2_locate_sections (bfd *, asection *, PTR);

static void dwarf2_build_psymtabs_easy (struct objfile *, int);

static void dwarf2_build_psymtabs_hard (struct objfile *, int);

static char *scan_partial_symbols (char *, struct objfile *,
				   CORE_ADDR *, CORE_ADDR *,
				   const struct comp_unit_instance *);

static void add_partial_symbol (struct partial_die_info *, struct objfile *,
				const struct comp_unit_instance *);

static void dwarf2_psymtab_to_symtab (struct partial_symtab *);

static void psymtab_to_symtab_1 (struct partial_symtab *);

static char *dwarf2_read_section (struct objfile *, file_ptr, unsigned int);

static void dwarf2_read_abbrevs (bfd *, struct comp_unit_instance *,
				 unsigned int);

static void dwarf2_empty_abbrev_table (PTR);

static struct abbrev_info *dwarf2_lookup_abbrev (unsigned int,
						 const struct
						 comp_unit_instance *);

static char *read_partial_die (struct partial_die_info *,
			       bfd *, char *, int *,
			       const struct comp_unit_instance *);

static char *read_full_die (struct die_info **, bfd *, char *,
			    const struct comp_unit_instance *);

static char *read_attribute (struct attribute *, struct attr_abbrev *,
			     bfd *, char *,
			     const struct comp_unit_instance *);

static unsigned int read_1_byte (bfd *, char *);

static int read_1_signed_byte (bfd *, char *);

static unsigned int read_2_bytes (bfd *, char *);

static unsigned int read_4_bytes (bfd *, char *);

static unsigned long read_8_bytes (bfd *, char *);

static CORE_ADDR read_address (bfd *, char *ptr,
			       const struct comp_unit_instance *,
			       int *bytes_read);

static LONGEST read_initial_length (bfd *, char *,
				    struct comp_unit_head *, int *bytes_read);

static LONGEST read_offset (bfd *, char *, const struct comp_unit_head *,
			    int *bytes_read);

static char *read_n_bytes (bfd *, char *, unsigned int);

static char *read_string (bfd *, char *, unsigned int *);

static unsigned long read_unsigned_leb128 (bfd *, char *, unsigned int *);

static long read_signed_leb128 (bfd *, char *, unsigned int *);

static void set_cu_language (struct comp_unit_instance *, unsigned int);

static struct attribute *dwarf2_attr (struct die_info *, unsigned int,
				     const struct comp_unit_instance *);

static int die_is_declaration (struct die_info *,
			       const struct comp_unit_instance *);

static void dwarf2_decode_lines (unsigned int, char *, bfd *,
				const struct comp_unit_instance *);

static void dwarf2_start_subfile (char *, char *);

static struct symbol *new_symbol (struct die_info *, struct type *,
				  struct objfile *,
				  const struct comp_unit_instance *);

static void dwarf2_const_value (struct attribute *, struct symbol *,
				struct objfile *,
				const struct comp_unit_instance *);

static void dwarf2_const_value_data (struct attribute *attr,
				     struct symbol *sym, int bits);

static struct type *die_type (struct die_info *, struct objfile *,
			      const struct comp_unit_instance *);

static struct type *die_containing_type (struct die_info *, struct objfile *,
					 const struct comp_unit_instance *);

static struct type *tag_type_to_type (struct die_info *, struct objfile *,
				      const struct comp_unit_instance *);

static void read_type_die (struct die_info *, struct objfile *,
			   const struct comp_unit_instance *);

static void read_typedef (struct die_info *, struct objfile *,
			  const struct comp_unit_instance *);

static void read_base_type (struct die_info *, struct objfile *,
			    const struct comp_unit_instance *);

static void read_file_scope (struct die_info *, struct objfile *,
			     const struct comp_unit_instance *);

static void read_func_scope (struct die_info *, struct objfile *,
			     const struct comp_unit_instance *);

static void read_lexical_block_scope (struct die_info *, struct objfile *,
				      const struct comp_unit_instance *);

static int dwarf2_get_pc_bounds (struct die_info *,
				 CORE_ADDR *, CORE_ADDR *, struct objfile *,
				 const struct comp_unit_instance *);

static void dwarf2_add_field (struct field_info *, struct die_info *,
			      struct objfile *,
			      const struct comp_unit_instance *);

static void dwarf2_attach_fields_to_type (struct field_info *,
					  struct type *, struct objfile *);

static void dwarf2_add_member_fn (struct field_info *,
				  struct die_info *, struct type *,
				  struct objfile *objfile,
				  const struct comp_unit_instance *);

static void dwarf2_attach_fn_fields_to_type (struct field_info *,
					     struct type *, struct objfile *);

static void read_structure_scope (struct die_info *, struct objfile *,
				  const struct comp_unit_instance *, const char *);

static void read_common_block (struct die_info *, struct objfile *,
			       const struct comp_unit_instance *);

static void read_enumeration (struct die_info *, struct objfile *,
			      const struct comp_unit_instance *);

static struct type *dwarf2_base_type (int, int, struct objfile *,
				     const struct comp_unit_instance *);

static CORE_ADDR decode_locdesc (struct dwarf2_block *, struct objfile *,
				 const struct comp_unit_instance *);

static void read_array_type (struct die_info *, struct objfile *,
			     const struct comp_unit_instance *);

static void read_tag_pointer_type (struct die_info *, struct objfile *,
				   const struct comp_unit_instance *);

static void read_tag_ptr_to_member_type (struct die_info *, struct objfile *,
					 const struct comp_unit_instance *);

static void read_tag_reference_type (struct die_info *, struct objfile *,
				     const struct comp_unit_instance *);

static void read_tag_const_type (struct die_info *, struct objfile *,
				 const struct comp_unit_instance *);

static void read_tag_volatile_type (struct die_info *, struct objfile *,
				    const struct comp_unit_instance *);

static void read_tag_string_type (struct die_info *, struct objfile *,
				  const struct comp_unit_instance *);

static void read_subroutine_type (struct die_info *, struct objfile *,
				  const struct comp_unit_instance *);

struct die_info *read_comp_unit_dies (char *, bfd *,
				      const struct comp_unit_instance *);

static void free_die_list (struct die_info *);

static struct cleanup *make_cleanup_free_die_list (struct die_info *);

static struct comp_unit_instance *dwarf2_get_cu_by_offset (bfd *, int);

static struct comp_unit_instance *dwarf2_get_comp_unit (bfd *, int);

static struct comp_unit_instance *dwarf2_get_or_read_comp_unit (bfd *, int);

static struct comp_unit_instance *dwarf2_read_comp_unit (bfd *, int);

static void process_die (struct die_info *, struct objfile *,
			 const struct comp_unit_instance *);

static char *dwarf2_linkage_name (struct die_info *,
				  const struct comp_unit_instance *);

static char *dwarf2_tag_name (unsigned int);

static char *dwarf2_attr_name (unsigned int);

static char *dwarf2_form_name (unsigned int);

static char *dwarf2_stack_op_name (unsigned int);

static char *dwarf2_bool_name (unsigned int);

static char *dwarf2_type_encoding_name (unsigned int);

struct die_info *sibling_die (struct die_info *);

void dump_die (struct die_info *, const struct comp_unit_instance *);

void store_in_ref_table (unsigned int, struct die_info *,
			 struct comp_unit_instance *);

static void dwarf2_empty_hash_tables (struct comp_unit_instance *);

static unsigned int dwarf2_get_ref_die_offset (const struct comp_unit_instance
					       *, struct attribute *);

struct die_info *follow_die_ref (unsigned int,
				 const struct comp_unit_instance *);

static struct type *dwarf2_fundamental_type (struct objfile *, int,
					     const struct comp_unit_instance
					     *);

/* memory allocation interface */

static void dwarf2_free_tmp_obstack (PTR);

static struct dwarf2_block *dwarf2_alloc_block (void);

static struct abbrev_info *dwarf2_alloc_abbrev (void);

static struct die_info *dwarf2_alloc_die (void);

static struct comp_unit_instance *dwarf2_alloc_comp_unit_instance (void);

static void dwarf2_read_frame_info (struct objfile *);

/* Try to locate the sections we need for DWARF 2 debugging
   information and return true if we have enough to do something.  */

int
dwarf2_has_info (bfd * abfd)
{
  dwarf2_info_offset = dwarf2_abbrev_offset = dwarf2_line_offset = 0;
  bfd_map_over_sections (abfd, dwarf2_locate_sections, NULL);
  if (dwarf2_info_offset && dwarf2_abbrev_offset)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

/* This function is mapped across the sections and remembers the
   offset and size of each of the debugging sections we are interested
   in.  */

static void
dwarf2_locate_sections (bfd * ignore_abfd, asection * sectp, PTR ignore_ptr)
{
  if (STREQ (sectp->name, INFO_SECTION))
    {
      dwarf2_info_offset = sectp->filepos;
      dwarf2_info_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, ABBREV_SECTION))
    {
      dwarf2_abbrev_offset = sectp->filepos;
      dwarf2_abbrev_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, LINE_SECTION))
    {
      dwarf2_line_offset = sectp->filepos;
      dwarf2_line_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, PUBNAMES_SECTION))
    {
      dwarf2_pubnames_offset = sectp->filepos;
      dwarf2_pubnames_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, ARANGES_SECTION))
    {
      dwarf2_aranges_offset = sectp->filepos;
      dwarf2_aranges_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, LOC_SECTION))
    {
      dwarf2_loc_offset = sectp->filepos;
      dwarf2_loc_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, MACINFO_SECTION))
    {
      dwarf2_macinfo_offset = sectp->filepos;
      dwarf2_macinfo_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, STR_SECTION))
    {
      dwarf2_str_offset = sectp->filepos;
      dwarf2_str_size = bfd_get_section_size_before_reloc (sectp);
    }
  else if (STREQ (sectp->name, FRAME_SECTION))
    {
      dwarf2_frame_offset = sectp->filepos;
      dwarf2_frame_size = bfd_get_section_size_before_reloc (sectp);
    }
}

/* Build a partial symbol table.  */

void
dwarf2_build_psymtabs (struct objfile *objfile, int mainline)
{
  /* We definitely need the .debug_info and .debug_abbrev sections */

  dwarf2_info_buffer = dwarf2_read_section (objfile,
					   dwarf2_info_offset,
					   dwarf2_info_size);
  dwarf2_abbrev_buffer = dwarf2_read_section (objfile,
					     dwarf2_abbrev_offset,
					     dwarf2_abbrev_size);
  dwarf2_line_buffer = dwarf2_read_section (objfile,
					   dwarf2_line_offset,
					   dwarf2_line_size);
  dwarf2_frame_buffer = dwarf2_read_section (objfile,
					    dwarf2_frame_offset,
					    dwarf2_frame_size);
  dwarf2_read_frame_info (objfile);
  if (mainline || objfile->global_psymbols.size == 0 ||
      objfile->static_psymbols.size == 0)
    {
      init_psymbol_list (objfile, 1024);
    }

  if (dwarf2_aranges_offset && dwarf2_pubnames_offset)
    {
      /* Things are significantly easier if we have .debug_aranges and
         .debug_pubnames sections */

      dwarf2_build_psymtabs_easy (objfile, mainline);
    }
#if 0
  else
    /* only test this case for now */
    {
#endif
      /* In this case we have to work a bit harder */
      dwarf2_build_psymtabs_hard (objfile, mainline);
#if 0
    }
#endif
}

char *aranges_buffer, *pubnames_buffer;

/* Build the partial symbol table from the information in the
   .debug_pubnames and .debug_aranges sections.  */

static char * 
get_pubname_for_offset (bfd *abfd, int looking_for_offset, const struct comp_unit_instance *cui)
{
  char  *pubnames_ptr;
  unsigned int entry_length, version, info_offset, info_size;
  pubnames_ptr = pubnames_buffer;
  while ((pubnames_ptr - pubnames_buffer) < dwarf2_pubnames_size)
    {
      struct comp_unit_head cu_header;
      int bytes_read;
      unsigned char *data;
      unsigned long offset;

      entry_length = read_4_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 4;
      version = read_2_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 2;
      info_offset = read_4_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 4;
      info_size = read_4_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 4;
      data = pubnames_ptr;

      do
	{
	  offset = read_4_bytes (abfd, pubnames_ptr);
	  if (offset != 0)
	    {
	      pubnames_ptr += 4;
	      data = pubnames_ptr;
	      pubnames_ptr += strlen ((char *) data) + 1;
	      if (cui->offset == info_offset && offset == looking_for_offset-1)
	      {
		      return data;
	      }
	    }
	}
      while (offset != 0);
    }
  return NULL;
}

static void
dwarf2_build_psymtabs_easy (struct objfile *objfile, int mainline)
{
  bfd *abfd = objfile->obfd;
  char *aranges_ptr, *pubnames_ptr;
  unsigned int entry_length, version, info_offset, info_size;

  pubnames_buffer = dwarf2_read_section (objfile,
					 dwarf2_pubnames_offset,
					 dwarf2_pubnames_size);
  pubnames_ptr = pubnames_buffer;
  while ((pubnames_ptr - pubnames_buffer) < dwarf2_pubnames_size)
    {
      struct comp_unit_head cu_header;
      int bytes_read;
      unsigned char *data;
      unsigned long offset;

      entry_length = read_4_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 4;
      version = read_2_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 2;
      info_offset = read_4_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 4;
      info_size = read_4_bytes (abfd, pubnames_ptr);
      pubnames_ptr += 4;
      data = pubnames_ptr;

      do
	{
	  offset = read_4_bytes (abfd, pubnames_ptr);
	  if (offset != 0)
	    {
	      pubnames_ptr += 4;
	      data = pubnames_ptr;
	      pubnames_ptr += strlen ((char *) data) + 1;
	    }
	}
      while (offset != 0);


    }

  aranges_buffer = dwarf2_read_section (objfile,
					dwarf2_aranges_offset,
					dwarf2_aranges_size);
}

/* Read in the comp unit header information from the debug_info at
   info_ptr. */

static char *
read_comp_unit_head (struct comp_unit_head *cu_header,
		     char *info_ptr, bfd * abfd)
{
  int signed_addr;
  int bytes_read;
  cu_header->length = read_initial_length (abfd, info_ptr, cu_header,
					   &bytes_read);
  info_ptr += bytes_read;
  cu_header->version = read_2_bytes (abfd, info_ptr);
  info_ptr += 2;
  cu_header->abbrev_offset = read_offset (abfd, info_ptr, cu_header,
					  &bytes_read);
  info_ptr += bytes_read;
  cu_header->addr_size = read_1_byte (abfd, info_ptr);
  info_ptr += 1;
  signed_addr = bfd_get_sign_extend_vma (abfd);
  if (signed_addr < 0)
    internal_error (__FILE__, __LINE__,
		    "read_comp_unit_head: dwarf from non elf file");
  cu_header->signed_addr_p = signed_addr;
  return info_ptr;
}
static struct comp_unit_instance *
dwarf2_get_cu_by_offset (bfd * abfd, int offset)
{
  int i;
  struct comp_unit_instance *cui;

  for (i = 0; i < COMP_UNIT_HASH_SIZE; i++)
    {
      cui = compilation_units[i];
      while (cui)
	{
	  if (offset >= cui->offset
	      && offset <= (cui->offset + cui->cu_header.length))
	    {
	      if (cui->abfd == abfd)
		return cui;
	    }
	  cui = cui->next;
	}
    }
  return NULL;
}
static struct comp_unit_instance *
dwarf2_get_comp_unit (bfd * abfd, int offset)
{
  struct comp_unit_instance *cui;
  int h;

  h = (offset % COMP_UNIT_HASH_SIZE);
  cui = compilation_units[h];
  while (cui)
    {
      if (cui->offset == offset && cui->abfd == abfd)
	{
	  return cui;
	}
      cui = cui->next;
    }
  return NULL;
}
static struct comp_unit_instance *
dwarf2_get_or_read_comp_unit (bfd * abfd, int offset)
{
  struct comp_unit_instance *cui;
  cui = dwarf2_get_comp_unit (abfd, offset);
  if (cui != NULL)
    return cui;
  cui = dwarf2_read_comp_unit (abfd, offset);
  return cui;
}

static struct comp_unit_instance *
dwarf2_read_comp_unit (bfd * abfd, int offset)
{
  int h;
  char *info_ptr;
  struct comp_unit_instance *cui = dwarf2_alloc_comp_unit_instance ();
  h = (offset % COMP_UNIT_HASH_SIZE);
  cui->next = compilation_units[h];
  compilation_units[h] = cui;

  info_ptr = dwarf2_info_buffer + offset;
  cui->offset = offset;
  cui->abfd = abfd;

  /* read in the comp_unit header  */
  info_ptr = read_comp_unit_head (&cui->cu_header, info_ptr, abfd);

  /* Read the abbrevs for this compilation unit  */
  dwarf2_read_abbrevs (abfd, cui, cui->cu_header.abbrev_offset);
//  make_cleanup (dwarf2_empty_abbrev_table, cui);

  return cui;

}

/* Build the partial symbol table by doing a quick pass through the
   .debug_info and .debug_abbrev sections.  */

static void
dwarf2_build_psymtabs_hard (struct objfile *objfile, int mainline)
{
  /* Instead of reading this into a big buffer, we should probably use
     mmap()  on architectures that support it. (FIXME) */
  bfd *abfd = objfile->obfd;
  char *info_ptr, *abbrev_ptr;
  char *beg_of_comp_unit;
  struct partial_die_info comp_unit_die;
  struct partial_symtab *pst;
  struct cleanup *back_to;
  int comp_unit_has_pc_info;
  CORE_ADDR lowpc, highpc;
  struct comp_unit_instance *cui;
  int i;
  info_ptr = dwarf2_info_buffer;
  abbrev_ptr = dwarf2_abbrev_buffer;
  obstack_init (&dwarf2_tmp_obstack);
  back_to = make_cleanup (dwarf2_free_tmp_obstack, NULL);

#if 0
  while ((unsigned int) (info_ptr - dwarf2_info_buffer)
	 + ((info_ptr - dwarf2_info_buffer) % 4) < dwarf2_info_size)
    {
      beg_of_comp_unit = info_ptr;
      cui =
	dwarf2_get_or_read_comp_unit (abfd,
				      beg_of_comp_unit - dwarf2_info_buffer);
      info_ptr =
	beg_of_comp_unit + cui->cu_header.length +
	cui->cu_header.initial_length_size;
    }
  info_ptr = dwarf2_info_buffer;
#endif
  while ((unsigned int) (info_ptr - dwarf2_info_buffer)
	 + ((info_ptr - dwarf2_info_buffer) % 4) < dwarf2_info_size)
    {
      struct comp_unit_head cu_header;
      beg_of_comp_unit = info_ptr;
      cui =
	dwarf2_get_or_read_comp_unit (abfd,
				      beg_of_comp_unit - dwarf2_info_buffer);
      info_ptr = read_comp_unit_head (&cu_header, info_ptr, abfd);

      if (cui->cu_header.version != 2)
	{
	  error ("Dwarf Error: wrong version in compilation unit header.");
	  return;
	}
      if (cui->cu_header.abbrev_offset >= dwarf2_abbrev_size)
	{
	  error
	    ("Dwarf Error: bad offset (0x%lx) in compilation unit header (offset 0x%lx + 6).",
	     (long) cui->cu_header.abbrev_offset,
	     (long) (beg_of_comp_unit - dwarf2_info_buffer));
	  return;
	}
      if (beg_of_comp_unit + cui->cu_header.length + cui->cu_header.initial_length_size
	  > dwarf2_info_buffer + dwarf2_info_size)
	{
	  error
	    ("Dwarf Error: bad length (0x%lx) in compilation unit header (offset 0x%lx + 0).",
	     (long) cui->cu_header.length,
	     (long) (beg_of_comp_unit - dwarf2_info_buffer));
	  return;
	}
      /* Read the compilation unit die */
      info_ptr = read_partial_die (&comp_unit_die, abfd, info_ptr,
				   &comp_unit_has_pc_info, cui);

      /* Set the language we're debugging */
      set_cu_language (cui, comp_unit_die.language);

      /* Allocate a new partial symbol table structure */
      pst = start_psymtab_common (objfile, objfile->section_offsets,
				  comp_unit_die.name ? comp_unit_die.
				  name : "", comp_unit_die.lowpc,
				  objfile->global_psymbols.next,
				  objfile->static_psymbols.next);

      pst->read_symtab_private = (char *)
	obstack_alloc (&objfile->psymbol_obstack,
		       sizeof (struct dwarf2_pinfo));
      cui->cu_header_offset = beg_of_comp_unit - dwarf2_info_buffer;
      DWARF2_INFO_BUFFER (pst) = dwarf2_info_buffer;
      DWARF2_INFO_OFFSET (pst) = beg_of_comp_unit - dwarf2_info_buffer;
      DWARF2_ABBREV_BUFFER (pst) = dwarf2_abbrev_buffer;
      DWARF2_ABBREV_SIZE (pst) = dwarf2_abbrev_size;
      DWARF2_LINE_BUFFER (pst) = dwarf2_line_buffer;
      DWARF2_FRAME_BUFFER (pst) = dwarf2_frame_buffer;
      baseaddr = ANOFFSET (objfile->section_offsets, SECT_OFF_TEXT (objfile));

      /* Store the function that reads in the rest of the symbol table */
      pst->read_symtab = dwarf2_psymtab_to_symtab;

      /* Check if comp unit has_children.
         If so, read the rest of the partial symbols from this comp unit.
         If not, there's no more debug_info for this comp unit. */
      if (comp_unit_die.has_children)
	{
	  info_ptr = scan_partial_symbols (info_ptr, objfile, &lowpc, &highpc,
					   cui);
	  /* If the compilation unit didn't have an explicit address range,
	     then use the information extracted from its child dies.  */
	  if (!comp_unit_has_pc_info)
	    {
	      comp_unit_die.lowpc = lowpc;
	      comp_unit_die.highpc = highpc;
	    }
	}
      pst->textlow = comp_unit_die.lowpc + baseaddr;
      pst->texthigh = comp_unit_die.highpc + baseaddr;

      pst->n_global_syms = objfile->global_psymbols.next -
	(objfile->global_psymbols.list + pst->globals_offset);
      pst->n_static_syms = objfile->static_psymbols.next -
	(objfile->static_psymbols.list + pst->statics_offset);
      sort_pst_symbols (pst);

      /* If there is already a psymtab or symtab for a file of this
         name, remove it. (If there is a symtab, more drastic things
         also happen.) This happens in VxWorks.  */
      free_named_symtabs (pst->filename);

      info_ptr = beg_of_comp_unit + cui->cu_header.length
	+ cui->cu_header.initial_length_size;
    }
  do_cleanups (back_to);
}

/* Read in all interesting dies to the end of the compilation unit.  */

static char *
scan_partial_symbols (char *info_ptr, struct objfile *objfile,
		      CORE_ADDR * lowpc, CORE_ADDR * highpc,
		      const struct comp_unit_instance *cui)
{
  bfd *abfd = objfile->obfd;
  struct partial_die_info pdi;

  /* This function is called after we've read in the comp_unit_die in
     order to read its children.  We start the nesting level at 1 since
     we have pushed 1 level down in order to read the comp unit's children.
     The comp unit itself is at level 0, so we stop reading when we pop
     back to that level. */

  int nesting_level = 1;
  int has_pc_info;

  *lowpc = ((CORE_ADDR) - 1);
  *highpc = ((CORE_ADDR) 0);

  while (nesting_level)
    {
      info_ptr = read_partial_die (&pdi, abfd, info_ptr, &has_pc_info, cui);

      if (pdi.name)
	{
	  switch (pdi.tag)
	    {
	    case DW_TAG_subprogram:
	      if (has_pc_info)
		{
		  if (pdi.lowpc < *lowpc)
		    {
		      *lowpc = pdi.lowpc;
		    }
		  if (pdi.highpc > *highpc)
		    {
		      *highpc = pdi.highpc;
		    }
		  if ((pdi.is_external || nesting_level == 1)
		      && !pdi.is_declaration)
		    {
		      add_partial_symbol (&pdi, objfile, cui);
		    }
		}
	      break;
	    case DW_TAG_variable:
	    case DW_TAG_typedef:
	    case DW_TAG_class_type:
	    case DW_TAG_structure_type:
	    case DW_TAG_union_type:
	    case DW_TAG_enumeration_type:
	      if ((pdi.is_external || nesting_level == 1)
		  && !pdi.is_declaration)
		{
		  add_partial_symbol (&pdi, objfile, cui);
		}
	      break;
	    case DW_TAG_enumerator:
	      /* File scope enumerators are added to the partial symbol
	         table.  */
	      if (nesting_level == 2)
		add_partial_symbol (&pdi, objfile, cui);
	      break;
	    case DW_TAG_base_type:
	      /* File scope base type definitions are added to the partial
	         symbol table.  */
	      if (nesting_level == 1)
		add_partial_symbol (&pdi, objfile, cui);
	      break;
	    default:
	      break;
	    }
	}

      /* If the die has a sibling, skip to the sibling.
         Do not skip enumeration types, we want to record their
         enumerators.  */
      if (pdi.sibling && pdi.tag != DW_TAG_enumeration_type)
	{
	  info_ptr = pdi.sibling;
	}
      else if (pdi.has_children)
	{
	  /* Die has children, but the optional DW_AT_sibling attribute
	     is missing.  */
	  nesting_level++;
	}

      if (pdi.tag == 0)
	{
	  nesting_level--;
	}
    }

  /* If we didn't find a lowpc, set it to highpc to avoid complaints
     from `maint check'.  */
  if (*lowpc == ((CORE_ADDR) - 1))
    *lowpc = *highpc;
  return info_ptr;
}

static void
add_partial_symbol (struct partial_die_info *pdi, struct objfile *objfile,
		    const struct comp_unit_instance *cui)
{
  CORE_ADDR addr = 0;

  switch (pdi->tag)
    {
    case DW_TAG_subprogram:
      if (pdi->is_external)
	{
	  /*prim_record_minimal_symbol (pdi->name, pdi->lowpc + baseaddr,
	     mst_text, objfile); */
	  add_psymbol_to_list (pdi->name, strlen (pdi->name),
			       VAR_NAMESPACE, LOC_BLOCK,
			       &objfile->global_psymbols,
			       0, pdi->lowpc + baseaddr, cui->cu_language,
			       objfile);
	}
      else
	{
	  /*prim_record_minimal_symbol (pdi->name, pdi->lowpc + baseaddr,
	     mst_file_text, objfile); */
	  add_psymbol_to_list (pdi->name, strlen (pdi->name),
			       VAR_NAMESPACE, LOC_BLOCK,
			       &objfile->static_psymbols,
			       0, pdi->lowpc + baseaddr, cui->cu_language,
			       objfile);
	}
      break;
    case DW_TAG_variable:
      if (pdi->is_external)
	{
	  /* Global Variable.
	     Don't enter into the minimal symbol tables as there is
	     a minimal symbol table entry from the ELF symbols already.
	     Enter into partial symbol table if it has a location
	     descriptor or a type.
	     If the location descriptor is missing, new_symbol will create
	     a LOC_UNRESOLVED symbol, the address of the variable will then
	     be determined from the minimal symbol table whenever the variable
	     is referenced.
	     The address for the partial symbol table entry is not
	     used by GDB, but it comes in handy for debugging partial symbol
	     table building.  */

	  if (pdi->locdesc)
	    addr = decode_locdesc (pdi->locdesc, objfile, cui);
	  if (pdi->locdesc || pdi->has_type)
	    add_psymbol_to_list (pdi->name, strlen (pdi->name),
				 VAR_NAMESPACE, LOC_STATIC,
				 &objfile->global_psymbols,
				 0, addr + baseaddr, cui->cu_language,
				 objfile);
	}
      else
	{
	  /* Static Variable. Skip symbols without location descriptors.  */
	  if (pdi->locdesc == NULL)
	    return;
	  addr = decode_locdesc (pdi->locdesc, objfile, cui);
	  /*prim_record_minimal_symbol (pdi->name, addr + baseaddr,
	     mst_file_data, objfile); */
	  add_psymbol_to_list (pdi->name, strlen (pdi->name),
			       VAR_NAMESPACE, LOC_STATIC,
			       &objfile->static_psymbols,
			       0, addr + baseaddr, cui->cu_language, objfile);
	}
      break;
    case DW_TAG_typedef:
    case DW_TAG_base_type:
      add_psymbol_to_list (pdi->name, strlen (pdi->name),
			   VAR_NAMESPACE, LOC_TYPEDEF,
			   &objfile->static_psymbols,
			   0, (CORE_ADDR) 0, cui->cu_language, objfile);
      break;
    case DW_TAG_class_type:
    case DW_TAG_structure_type:
    case DW_TAG_union_type:
    case DW_TAG_enumeration_type:
      /* Skip aggregate types without children, these are external
         references.  */
      if (pdi->has_children == 0)
	return;
      add_psymbol_to_list (pdi->name, strlen (pdi->name),
			   STRUCT_NAMESPACE, LOC_TYPEDEF,
			   &objfile->static_psymbols,
			   0, (CORE_ADDR) 0, cui->cu_language, objfile);

      if (cui->cu_language == language_cplus)
	{
	  /* For C++, these implicitly act as typedefs as well. */
	  add_psymbol_to_list (pdi->name, strlen (pdi->name),
			       VAR_NAMESPACE, LOC_TYPEDEF,
			       &objfile->static_psymbols,
			       0, (CORE_ADDR) 0, cui->cu_language, objfile);
	}
      break;
    case DW_TAG_enumerator:
      add_psymbol_to_list (pdi->name, strlen (pdi->name),
			   VAR_NAMESPACE, LOC_CONST,
			   &objfile->static_psymbols,
			   0, (CORE_ADDR) 0, cui->cu_language, objfile);
      break;
    default:
      break;
    }
}

/* Expand this partial symbol table into a full symbol table.  */

static void
dwarf2_psymtab_to_symtab (struct partial_symtab *pst)
{
  /* FIXME: This is barely more than a stub.  */
  if (pst != NULL)
    {
      if (pst->readin)
	{
	  warning ("bug: psymtab for %s is already read in.", pst->filename);
	}
      else
	{
	  if (info_verbose)
	    {
	      printf_filtered ("Reading in symbols for %s...", pst->filename);
	      gdb_flush (gdb_stdout);
	    }

	  psymtab_to_symtab_1 (pst);

	  /* Finish up the debug error message.  */
	  if (info_verbose)
	    printf_filtered ("done.\n");
	}
    }
}

static void
psymtab_to_symtab_1 (struct partial_symtab *pst)
{
  struct objfile *objfile = pst->objfile;
  bfd *abfd = objfile->obfd;
  struct comp_unit_instance *cui;
  struct die_info *dies;
  unsigned long offset;
  CORE_ADDR lowpc, highpc;
  struct die_info *child_die;
  char *info_ptr;
  struct symtab *symtab;
  struct cleanup *back_to;
  int i = 0;
  /* Set local variables from the partial symbol table info.  */
  offset = DWARF2_INFO_OFFSET (pst);
  dwarf2_info_buffer = DWARF2_INFO_BUFFER (pst);
  dwarf2_abbrev_buffer = DWARF2_ABBREV_BUFFER (pst);
  dwarf2_abbrev_size = DWARF2_ABBREV_SIZE (pst);
  dwarf2_line_buffer = DWARF2_LINE_BUFFER (pst);
  dwarf2_frame_buffer = DWARF2_FRAME_BUFFER (pst);
  baseaddr = ANOFFSET (pst->section_offsets, SECT_OFF_TEXT (objfile));
  cui = dwarf2_get_or_read_comp_unit (abfd, offset);
  cui->cu_header_offset = offset;
  info_ptr = dwarf2_info_buffer + offset;

  obstack_init (&dwarf2_tmp_obstack);
  back_to = make_cleanup (dwarf2_free_tmp_obstack, NULL);

  buildsym_init ();
  make_cleanup (really_free_pendings, NULL);

  /* read in the comp_unit header, again, because we don't know it's size */
  info_ptr = read_comp_unit_head (&cui->cu_header, info_ptr, abfd);
  dies = read_comp_unit_dies (info_ptr, abfd, cui);

  make_cleanup_free_die_list (dies);

  /* Do line number decoding in read_file_scope () */
  process_die (dies, objfile, cui);

  if (!dwarf2_get_pc_bounds (dies, &lowpc, &highpc, objfile, cui))
    {
      /* Some compilers don't define a DW_AT_high_pc attribute for
         the compilation unit.   If the DW_AT_high_pc is missing,
         synthesize it, by scanning the DIE's below the compilation unit.  */
      highpc = 0;
      if (dies->has_children)
	{
	  child_die = dies->next;
	  while (child_die && child_die->tag)
	    {
	      if (child_die->tag == DW_TAG_subprogram)
		{
		  CORE_ADDR low, high;

		  if (dwarf2_get_pc_bounds
		      (child_die, &low, &high, objfile, cui))
		    {
		      highpc = max (highpc, high);
		    }
		}
	      child_die = sibling_die (child_die);
	    }
	}
    }
  symtab = end_symtab (highpc + baseaddr, objfile, SECT_OFF_TEXT (objfile));

  /* Set symtab language to language from DW_AT_language.
     If the compilation is from a C file generated by language preprocessors,
     do not set the language if it was already deduced by start_subfile.  */
  if (symtab != NULL
      && !(cui->cu_language == language_c && symtab->language != language_c))
    {
      symtab->language = cui->cu_language;
    }
  pst->symtab = symtab;
  pst->readin = 1;
  sort_symtab_syms (symtab);
  xfree (dwarf2_info_buffer);
  xfree (dwarf2_abbrev_buffer);
  xfree (dwarf2_line_buffer);
  xfree (dwarf2_frame_buffer);
  do_cleanups (back_to);
}

/* Process a die and its children.  */

static void
process_die (struct die_info *die, struct objfile *objfile,
	     const struct comp_unit_instance *cui)
{
  switch (die->tag)
    {
    case DW_TAG_padding:
      break;
    case DW_TAG_compile_unit:
      read_file_scope (die, objfile, cui);
      break;
    case DW_TAG_subprogram:
      read_subroutine_type (die, objfile, cui);
      read_func_scope (die, objfile, cui);
      break;
    case DW_TAG_inlined_subroutine:
   /*   DJB - handled now  */
      read_subroutine_type (die, objfile, cui);
      read_func_scope (die, objfile, cui);
      break;
    case DW_TAG_lexical_block:
      read_lexical_block_scope (die, objfile, cui);
      break;
    case DW_TAG_class_type:
    case DW_TAG_structure_type:
    case DW_TAG_union_type:
      read_structure_scope (die, objfile, cui, NULL);
      break;
    case DW_TAG_enumeration_type:
      read_enumeration (die, objfile, cui);
      break;
    case DW_TAG_subroutine_type:
      read_subroutine_type (die, objfile, cui);
      break;
    case DW_TAG_array_type:
      read_array_type (die, objfile, cui);
      break;
    case DW_TAG_pointer_type:
      read_tag_pointer_type (die, objfile, cui);
      break;
    case DW_TAG_ptr_to_member_type:
      read_tag_ptr_to_member_type (die, objfile, cui);
      break;
    case DW_TAG_reference_type:
      read_tag_reference_type (die, objfile, cui);
      break;
    case DW_TAG_string_type:
      read_tag_string_type (die, objfile, cui);
      break;
    case DW_TAG_base_type:
      read_base_type (die, objfile, cui);
      if (dwarf2_attr (die, DW_AT_name, cui))
	{
	  /* Add a typedef symbol for the base type definition.  */
	  new_symbol (die, die->type, objfile, cui);
	}
      break;
    case DW_TAG_common_block:
      read_common_block (die, objfile, cui);
      break;
    case DW_TAG_common_inclusion:
      break;
    default:
      new_symbol (die, NULL, objfile, cui);
      break;
    }
}

static void
read_file_scope (struct die_info *die, struct objfile *objfile,
		 const struct comp_unit_instance *cui)
{
  unsigned int line_offset = 0;
  CORE_ADDR lowpc = ((CORE_ADDR) - 1);
  CORE_ADDR highpc = ((CORE_ADDR) 0);
  struct attribute *attr;
  char *name = "<unknown>";
  char *comp_dir = NULL;
  struct die_info *child_die;
  bfd *abfd = objfile->obfd;

  if (!dwarf2_get_pc_bounds (die, &lowpc, &highpc, objfile, cui))
    {
      if (die->has_children)
	{
	  child_die = die->next;
	  while (child_die && child_die->tag)
	    {
	      if (child_die->tag == DW_TAG_subprogram)
		{
		  CORE_ADDR low, high;

		  if (dwarf2_get_pc_bounds
		      (child_die, &low, &high, objfile, cui))
		    {
		      lowpc = min (lowpc, low);
		      highpc = max (highpc, high);
		    }
		}
	      child_die = sibling_die (child_die);
	    }
	}
    }

  /* If we didn't find a lowpc, set it to highpc to avoid complaints
     from finish_block.  */
  if (lowpc == ((CORE_ADDR) - 1))
    lowpc = highpc;
  lowpc += baseaddr;
  highpc += baseaddr;

  attr = dwarf2_attr (die, DW_AT_name, cui);
  if (attr)
    {
      name = DW_STRING (attr);
    }
  attr = dwarf2_attr (die, DW_AT_comp_dir, cui);
  if (attr)
    {
      comp_dir = DW_STRING (attr);
      if (comp_dir)
	{
	  /* Irix 6.2 native cc prepends <machine>.: to the compilation
	     directory, get rid of it.  */
	  char *cp = strchr (comp_dir, ':');

	  if (cp && cp != comp_dir && cp[-1] == '.' && cp[1] == '/')
	    comp_dir = cp + 1;
	}
    }

  if (objfile->ei.entry_point >= lowpc && objfile->ei.entry_point < highpc)
    {
      objfile->ei.entry_file_lowpc = lowpc;
      objfile->ei.entry_file_highpc = highpc;
    }

  attr = dwarf2_attr (die, DW_AT_language, cui);
  if (attr)
    {
      set_cu_language ((struct comp_unit_instance *) cui, DW_UNSND (attr));
    }

  /* We assume that we're processing GCC output. */
  processing_gcc_compilation = 2;
#if 0
  /* FIXME:Do something here.  */
  if (dip->at_producer != NULL)
    {
      handle_producer (dip->at_producer);
    }
#endif

  /* The compilation unit may be in a different language or objfile,
     zero out all remembered fundamental types.  */
  memset (ftypes, 0, FT_NUM_MEMBERS * sizeof (struct type *));

  start_symtab (name, comp_dir, lowpc);
  record_debugformat ("DWARF 2");

  /* Decode line number information if present.  */
  attr = dwarf2_attr (die, DW_AT_stmt_list, cui);
  if (attr)
    {
      line_offset = DW_UNSND (attr);
      dwarf2_decode_lines (line_offset, comp_dir, abfd, cui);
    }

  /* Process all dies in compilation unit.  */
  if (die->has_children)
    {
      child_die = die->next;
      while (child_die && child_die->tag)
	{
	  process_die (child_die, objfile, cui);
	  child_die = sibling_die (child_die);
	}
    }
}

static void
read_func_scope (struct die_info *die, struct objfile *objfile,
		 const struct comp_unit_instance *cui)
{
  register struct context_stack *new;
  CORE_ADDR lowpc;
  CORE_ADDR highpc;
  struct die_info *child_die;
  struct attribute *attr;
  char *name;
  struct dwarf2_block *d2loc;
  void *data;
  name = dwarf2_linkage_name (die, cui);

  /* Ignore functions with missing or empty names and functions with
     missing or invalid low and high pc attributes.  */
  if (name == NULL
      || !dwarf2_get_pc_bounds (die, &lowpc, &highpc, objfile, cui))
    return;

  lowpc += baseaddr;
  highpc += baseaddr;

  if (objfile->ei.entry_point >= lowpc && objfile->ei.entry_point < highpc)
    {
      objfile->ei.entry_func_lowpc = lowpc;
      objfile->ei.entry_func_highpc = highpc;
    }

  /* Decode DW_AT_frame_base location descriptor if present, keep result
     for DW_OP_fbreg operands in decode_locdesc.  */
  frame_base_reg = -1;
  frame_base_offset = 0;
  attr = dwarf2_attr (die, DW_AT_frame_base, cui);
  if (attr)
    {
      CORE_ADDR addr = decode_locdesc (DW_BLOCK (attr), objfile, cui);
      if (isderef)
	complain (&dwarf2_unsupported_at_frame_base, name);
      else if (isreg)
	frame_base_reg = addr;
      else if (offreg)
	{
	  frame_base_reg = basereg;
	  frame_base_offset = addr;
	}
      else
	complain (&dwarf2_unsupported_at_frame_base, name);
    }

  new = push_context (0, lowpc);
  new->name = new_symbol (die, die->type, objfile, cui);
  if (attr)
    {
#if 0
	    SYMBOL_FRAME_LOC_EXPR (new->name) = obstack_alloc (&objfile->symbol_obstack, sizeof (struct dwarf2_block));
      d2loc = SYMBOL_FRAME_LOC_EXPR (new->name);
      memset (d2loc, 0, DW_BLOCK (attr)->size);
      d2loc->size = DW_BLOCK (attr)->size;
      d2loc->data = xmalloc (DW_BLOCK (attr)->size);
      memset (d2loc->data, 0, DW_BLOCK (attr)->size);
      memcpy (d2loc->data, DW_BLOCK (attr)->data, DW_BLOCK (attr)->size);
      data = d2loc->data;
      d2loc->data = bcache (data, DW_BLOCK (attr)->size, &objfile->psymbol_cache);
      xfree (data);
#endif
    }
  list_in_scope = &local_symbols;

  if (die->has_children)
    {
      child_die = die->next;
      while (child_die && child_die->tag)
	{
	  process_die (child_die, objfile, cui);
	  child_die = sibling_die (child_die);
	}
    }

  new = pop_context ();
  /* Make a block for the local symbols within.  */
  finish_block (new->name, &local_symbols, new->old_blocks,
		lowpc, highpc, objfile);
  list_in_scope = &file_symbols;
}

/* Process all the DIES contained within a lexical block scope.  Start
   a new scope, process the dies, and then close the scope.  */

static void
read_lexical_block_scope (struct die_info *die, struct objfile *objfile,
			  const struct comp_unit_instance *cui)
{
  register struct context_stack *new;
  CORE_ADDR lowpc, highpc;
  struct die_info *child_die;

  /* Ignore blocks with missing or invalid low and high pc attributes.  */
  if (!dwarf2_get_pc_bounds (die, &lowpc, &highpc, objfile, cui))
    return;
  lowpc += baseaddr;
  highpc += baseaddr;

  push_context (0, lowpc);
  if (die->has_children)
    {
      child_die = die->next;
      while (child_die && child_die->tag)
	{
	  process_die (child_die, objfile, cui);
	  child_die = sibling_die (child_die);
	}
    }
  new = pop_context ();

  if (local_symbols != NULL)
    {
      finish_block (0, &local_symbols, new->old_blocks, new->start_addr,
		    highpc, objfile);
    }
  local_symbols = new->locals;
}

/* Get low and high pc attributes from a die.
   Return 1 if the attributes are present and valid, otherwise, return 0.  */

static int
dwarf2_get_pc_bounds (struct die_info *die, CORE_ADDR * lowpc,
		      CORE_ADDR * highpc, struct objfile *objfile,
		      const struct comp_unit_instance *cui)
{
  struct attribute *attr;
  CORE_ADDR low;
  CORE_ADDR high;

  attr = dwarf2_attr (die, DW_AT_low_pc, cui);
  if (attr)
    low = DW_ADDR (attr);
  else
    return 0;
  attr = dwarf2_attr (die, DW_AT_high_pc, cui);
  if (attr)
    high = DW_ADDR (attr);
  else
    return 0;

  if (high < low)
    return 0;

  /* When using the GNU linker, .gnu.linkonce. sections are used to
     eliminate duplicate copies of functions and vtables and such.
     The linker will arbitrarily choose one and discard the others.
     The AT_*_pc values for such functions refer to local labels in
     these sections.  If the section from that file was discarded, the
     labels are not in the output, so the relocs get a value of 0.
     If this is a discarded function, mark the pc bounds as invalid,
     so that GDB will ignore it.  */
  if (low == 0 && (bfd_get_file_flags (objfile->obfd) & HAS_RELOC) == 0)
    return 0;

  *lowpc = low;
  *highpc = high;
  return 1;
}

/* Add an aggregate field to the field list.  */

static void
dwarf2_add_field (struct field_info *fip, struct die_info *die,
		  struct objfile *objfile,
		  const struct comp_unit_instance *cui)
{
  struct nextfield *new_field;
  struct attribute *attr;
  struct field *fp;
  char *fieldname = "";

  /* Allocate a new field list entry and link it in.  */
  new_field = (struct nextfield *) xmalloc (sizeof (struct nextfield));
  make_cleanup (xfree, new_field);
  memset (new_field, 0, sizeof (struct nextfield));
  new_field->next = fip->fields;
  fip->fields = new_field;
  fip->nfields++;

  /* Handle accessibility and virtuality of field.
     The default accessibility for members is public, the default
     accessibility for inheritance is private.  */
  if (die->tag != DW_TAG_inheritance)
    new_field->accessibility = DW_ACCESS_public;
  else
    new_field->accessibility = DW_ACCESS_private;
  new_field->virtuality = DW_VIRTUALITY_none;

  attr = dwarf2_attr (die, DW_AT_accessibility, cui);
  if (attr)
    new_field->accessibility = DW_UNSND (attr);
  if (new_field->accessibility != DW_ACCESS_public)
    fip->non_public_fields = 1;
  attr = dwarf2_attr (die, DW_AT_virtuality, cui);
  if (attr)
    new_field->virtuality = DW_UNSND (attr);

  fp = &new_field->field;
  if (die->tag == DW_TAG_member)
    {
      /* Get type of field.  */
      fp->type = die_type (die, objfile, cui);

      /* Get bit size of field (zero if none).  */
      attr = dwarf2_attr (die, DW_AT_bit_size, cui);
      if (attr)
	{
	  FIELD_BITSIZE (*fp) = DW_UNSND (attr);
	}
      else
	{
	  FIELD_BITSIZE (*fp) = 0;
	}

      /* Get bit offset of field.  */
      attr = dwarf2_attr (die, DW_AT_data_member_location, cui);
      if (attr)
	{
	  FIELD_BITPOS (*fp) =
	    decode_locdesc (DW_BLOCK (attr), objfile, cui) * bits_per_byte;
	}
      else
	FIELD_BITPOS (*fp) = 0;
      attr = dwarf2_attr (die, DW_AT_bit_offset, cui);
      if (attr)
	{
	  if (BITS_BIG_ENDIAN)
	    {
	      /* For big endian bits, the DW_AT_bit_offset gives the
	         additional bit offset from the MSB of the containing
	         anonymous object to the MSB of the field.  We don't
	         have to do anything special since we don't need to
	         know the size of the anonymous object.  */
	      FIELD_BITPOS (*fp) += DW_UNSND (attr);
	    }
	  else
	    {
	      /* For little endian bits, compute the bit offset to the
	         MSB of the anonymous object, subtract off the number of
	         bits from the MSB of the field to the MSB of the
	         object, and then subtract off the number of bits of
	         the field itself.  The result is the bit offset of
	         the LSB of the field.  */
	      int anonymous_size;
	      int bit_offset = DW_UNSND (attr);

	      attr = dwarf2_attr (die, DW_AT_byte_size, cui);
	      if (attr)
		{
		  /* The size of the anonymous object containing
		     the bit field is explicit, so use the
		     indicated size (in bytes).  */
		  anonymous_size = DW_UNSND (attr);
		}
	      else
		{
		  /* The size of the anonymous object containing
		     the bit field must be inferred from the type
		     attribute of the data member containing the
		     bit field.  */
		  anonymous_size = TYPE_LENGTH (fp->type);
		}
	      FIELD_BITPOS (*fp) += anonymous_size * bits_per_byte
		- bit_offset - FIELD_BITSIZE (*fp);
	    }
	}

      /* Get name of field.  */
      attr = dwarf2_attr (die, DW_AT_name, cui);
      if (attr && DW_STRING (attr))
	fieldname = DW_STRING (attr);
      fp->name =		/*obsavestring (fieldname, strlen (fieldname),
				   &objfile->type_obstack); */ bcache (fieldname,
					       strlen (fieldname) + 1, &objfile->
					       psymbol_cache);


      /* Change accessibility for artificial fields (e.g. virtual table
         pointer or virtual base class pointer) to private.  */
      if (dwarf2_attr (die, DW_AT_artificial, cui))
	{
	  new_field->accessibility = DW_ACCESS_private;
	  fip->non_public_fields = 1;
	}
    }
  else if (die->tag == DW_TAG_variable)
    {
      char *physname;

      /* C++ static member.
         Get name of field.  */
      attr = dwarf2_attr (die, DW_AT_name, cui);
      if (attr && DW_STRING (attr))
	fieldname = DW_STRING (attr);
      else
	return;

      /* Get physical name.  */
      physname = dwarf2_linkage_name (die, cui);

      SET_FIELD_PHYSNAME (*fp, bcache (physname, strlen (physname) + 1, &objfile->psymbol_cache)	/*obsavestring (physname, strlen (physname),
													   &objfile->type_obstack) */ );
      FIELD_TYPE (*fp) = die_type (die, objfile, cui);
      FIELD_NAME (*fp) =	/*obsavestring (fieldname, strlen (fieldname),
				   &objfile->type_obstack); */ bcache (physname,
					       strlen (physname) + 1, &objfile->
					       psymbol_cache);
    }
  else if (die->tag == DW_TAG_inheritance)
    {
      /* C++ base class field.  */
      attr = dwarf2_attr (die, DW_AT_data_member_location, cui);
      if (attr)
	FIELD_BITPOS (*fp) = (decode_locdesc (DW_BLOCK (attr), objfile, cui)
			      * bits_per_byte);
      FIELD_BITSIZE (*fp) = 0;
      FIELD_TYPE (*fp) = die_type (die, objfile, cui);
      FIELD_NAME (*fp) = xstrdup(type_name_no_tag (fp->type));
      fip->nbaseclasses++;
    }
}

/* Create the vector of fields, and attach it to the type.  */

static void
dwarf2_attach_fields_to_type (struct field_info *fip, struct type *type,
			      struct objfile *objfile)
{
  int nfields = fip->nfields;

  /* Record the field count, allocate space for the array of fields,
     and create blank accessibility bitfields if necessary.  */
  TYPE_NFIELDS (type) = nfields;
  TYPE_FIELDS (type) = (struct field *)
    TYPE_ALLOC (type, sizeof (struct field) * nfields);
  memset (TYPE_FIELDS (type), 0, sizeof (struct field) * nfields);

  if (fip->non_public_fields)
    {
      ALLOCATE_CPLUS_STRUCT_TYPE (type);

      TYPE_FIELD_PRIVATE_BITS (type) =
	(B_TYPE *) TYPE_ALLOC (type, B_BYTES (nfields));
      B_CLRALL (TYPE_FIELD_PRIVATE_BITS (type), nfields);

      TYPE_FIELD_PROTECTED_BITS (type) =
	(B_TYPE *) TYPE_ALLOC (type, B_BYTES (nfields));
      B_CLRALL (TYPE_FIELD_PROTECTED_BITS (type), nfields);

      TYPE_FIELD_IGNORE_BITS (type) =
	(B_TYPE *) TYPE_ALLOC (type, B_BYTES (nfields));
      B_CLRALL (TYPE_FIELD_IGNORE_BITS (type), nfields);
    }

  /* If the type has baseclasses, allocate and clear a bit vector for
     TYPE_FIELD_VIRTUAL_BITS.  */
  if (fip->nbaseclasses)
    {
      int num_bytes = B_BYTES (fip->nbaseclasses);
      char *pointer;

      ALLOCATE_CPLUS_STRUCT_TYPE (type);
      pointer = (char *) TYPE_ALLOC (type, num_bytes);
      TYPE_FIELD_VIRTUAL_BITS (type) = (B_TYPE *) pointer;
      B_CLRALL (TYPE_FIELD_VIRTUAL_BITS (type), fip->nbaseclasses);
      TYPE_N_BASECLASSES (type) = fip->nbaseclasses;
    }

  /* Copy the saved-up fields into the field vector.  Start from the head
     of the list, adding to the tail of the field array, so that they end
     up in the same order in the array in which they were added to the list.  */
  while (nfields-- > 0)
    {
      TYPE_FIELD (type, nfields) = fip->fields->field;
      switch (fip->fields->accessibility)
	{
	case DW_ACCESS_private:
	  SET_TYPE_FIELD_PRIVATE (type, nfields);
	  break;

	case DW_ACCESS_protected:
	  SET_TYPE_FIELD_PROTECTED (type, nfields);
	  break;

	case DW_ACCESS_public:
	  break;

	default:
	  /* Unknown accessibility.  Complain and treat it as public.  */
	  {
	    complain (&dwarf2_unsupported_accessibility,
		      fip->fields->accessibility);
	  }
	  break;
	}
      if (nfields < fip->nbaseclasses)
	{
	  switch (fip->fields->virtuality)
	    {
	    case DW_VIRTUALITY_virtual:
	    case DW_VIRTUALITY_pure_virtual:
	      SET_TYPE_FIELD_VIRTUAL (type, nfields);
	      break;
	    }
	}
      fip->fields = fip->fields->next;
    }
}

/* Add a member function to the proper fieldlist.  */

static void
dwarf2_add_member_fn (struct field_info *fip, struct die_info *die,
		      struct type *type, struct objfile *objfile,
		      const struct comp_unit_instance *cui)
{
  struct attribute *attr;
  struct fnfieldlist *flp;
  int i;
  struct fn_field *fnp;
  char *fieldname;
  char *physname;
  struct nextfnfield *new_fnfield;

  /* Get name of member function.  */
  attr = dwarf2_attr (die, DW_AT_name, cui);
  if (attr && DW_STRING (attr))
    fieldname = DW_STRING (attr);
  else
    return;

  /* Get the mangled name.  */
  physname = dwarf2_linkage_name (die, cui);

  /* Look up member function name in fieldlist.  */
  for (i = 0; i < fip->nfnfields; i++)
    {
      if (STREQ (fip->fnfieldlists[i].name, fieldname))
	break;
    }

  /* Create new list element if necessary.  */
  if (i < fip->nfnfields)
    flp = &fip->fnfieldlists[i];
  else
    {
      if ((fip->nfnfields % DW_FIELD_ALLOC_CHUNK) == 0)
	{
	  fip->fnfieldlists = (struct fnfieldlist *)
	    xrealloc (fip->fnfieldlists,
		      (fip->nfnfields + DW_FIELD_ALLOC_CHUNK)
		      * sizeof (struct fnfieldlist));
	  if (fip->nfnfields == 0)
	    make_cleanup (free_current_contents, &fip->fnfieldlists);
	}
      flp = &fip->fnfieldlists[fip->nfnfields];
      flp->name = bcache (fieldname, strlen(fieldname)+1, &objfile->psymbol_cache);
      flp->length = 0;
      flp->head = NULL;
      fip->nfnfields++;
    }

  /* Create a new member function field and chain it to the field list
     entry. */
  new_fnfield = (struct nextfnfield *) xmalloc (sizeof (struct nextfnfield));
  make_cleanup (xfree, new_fnfield);
  memset (new_fnfield, 0, sizeof (struct nextfnfield));
  new_fnfield->next = flp->head;
  flp->head = new_fnfield;
  flp->length++;

  /* Fill in the member function field info.  */
  fnp = &new_fnfield->fnfield;
  fnp->physname =		/*obsavestring (physname, strlen (physname),
				   &objfile->type_obstack); */ bcache (physname, strlen (physname) + 1,
					   &objfile->psymbol_cache);
  fnp->type = alloc_type (objfile);
  if (die->type && TYPE_CODE (die->type) == TYPE_CODE_FUNC)
    {
      struct type *return_type = FUNCTION_RETURN_VALUE (die->type);
      struct type **arg_types;
      int nparams = FUNCTION_NUM_ARGUMENTS (die->type);
      int iparams;

      /* Copy argument types from the subroutine type.  */
      arg_types = (struct type **)
	TYPE_ALLOC (fnp->type, (nparams + 1) * sizeof (struct type *));
      for (iparams = 0; iparams < nparams; iparams++)
	arg_types[iparams] = FUNCTION_ARGUMENT_TYPE (die->type, iparams);

      /* Set last entry in argument type vector.  */
      if (TYPE_FLAGS (die->type) & TYPE_FLAG_VARARGS)
	arg_types[nparams] = NULL;
      else
	arg_types[nparams] = dwarf2_fundamental_type (objfile, FT_VOID, cui);

      smash_to_method_type (fnp->type, type, return_type, arg_types);

      /* Handle static member functions.
         Dwarf2 has no clean way to discern C++ static and non-static
         member functions. G++ helps GDB by marking the first
         parameter for non-static member functions (which is the
         this pointer) as artificial. We obtain this information
         from read_subroutine_type via TYPE_FIELD_ARTIFICIAL.  */
      if (nparams == 0 || TYPE_FIELD_ARTIFICIAL (die->type, 0) == 0)
	fnp->voffset = VOFFSET_STATIC;
    }
  else
    complain (&dwarf2_missing_member_fn_type_complaint, physname);

  /* Get fcontext from DW_AT_containing_type if present.  */
  if (dwarf2_attr (die, DW_AT_containing_type, cui) != NULL)
    fnp->fcontext = die_containing_type (die, objfile, cui);

  /* dwarf2 doesn't have stubbed physical names, so the setting of is_const
     and is_volatile is irrelevant, as it is needed by gdb_mangle_name only.  */

  /* Get accessibility.  */
  attr = dwarf2_attr (die, DW_AT_accessibility, cui);
  if (attr)
    {
      switch (DW_UNSND (attr))
	{
	case DW_ACCESS_private:
	  fnp->is_private = 1;
	  break;
	case DW_ACCESS_protected:
	  fnp->is_protected = 1;
	  break;
	}
    }

  /* Get index in virtual function table if it is a virtual member function.  */
  attr = dwarf2_attr (die, DW_AT_vtable_elem_location, cui);
  if (attr)
    fnp->voffset = decode_locdesc (DW_BLOCK (attr), objfile, cui) + 2;
}

/* Create the vector of member function fields, and attach it to the type.  */

static void
dwarf2_attach_fn_fields_to_type (struct field_info *fip, struct type *type,
				 struct objfile *objfile)
{
  struct fnfieldlist *flp;
  int total_length = 0;
  int i;

  ALLOCATE_CPLUS_STRUCT_TYPE (type);
  TYPE_FN_FIELDLISTS (type) = (struct fn_fieldlist *)
    TYPE_ALLOC (type, sizeof (struct fn_fieldlist) * fip->nfnfields);

  for (i = 0, flp = fip->fnfieldlists; i < fip->nfnfields; i++, flp++)
    {
      struct nextfnfield *nfp = flp->head;
      struct fn_fieldlist *fn_flp = &TYPE_FN_FIELDLIST (type, i);
      int k;

      TYPE_FN_FIELDLIST_NAME (type, i) = flp->name;
      TYPE_FN_FIELDLIST_LENGTH (type, i) = flp->length;
      fn_flp->fn_fields = (struct fn_field *)
	TYPE_ALLOC (type, sizeof (struct fn_field) * flp->length);
      for (k = flp->length; (k--, nfp); nfp = nfp->next)
	fn_flp->fn_fields[k] = nfp->fnfield;

      total_length += flp->length;
    }

  TYPE_NFN_FIELDS (type) = fip->nfnfields;
  TYPE_NFN_FIELDS_TOTAL (type) = total_length;
}

/* Called when we find the DIE that starts a structure or union scope
   (definition) to process all dies that define the members of the
   structure or union.

   NOTE: we need to call struct_type regardless of whether or not the
   DIE has an at_name attribute, since it might be an anonymous
   structure or union.  This gets the type entered into our set of
   user defined types.

   However, if the structure is incomplete (an opaque struct/union)
   then suppress creating a symbol table entry for it since gdb only
   wants to find the one with the complete definition.  Note that if
   it is complete, we just call new_symbol, which does it's own
   checking about whether the struct/union is anonymous or not (and
   suppresses creating a symbol table entry itself).  
 
   To handle naming of nested structure scopes, we prepend prefix to the name.
 */

static void
read_structure_scope (struct die_info *die, struct objfile *objfile,
		      const struct comp_unit_instance *cui, const char *prefix)
{
  struct type *type;
  struct attribute *attr;

  type = alloc_type (objfile);

  INIT_CPLUS_SPECIFIC (type);
  attr = dwarf2_attr (die, DW_AT_name, cui);
  if (attr && DW_STRING (attr))
    {
      if (prefix != NULL)
      {
	      char *structname;
	      structname = xcalloc (strlen (DW_STRING (attr)) + strlen (prefix) + strlen("::") + 1, sizeof (char));
	      strcat (structname, prefix);
	      strcat (structname, "::");
	      strcat (structname, DW_STRING (attr));
	      TYPE_TAG_NAME (type) = bcache (structname, strlen (structname) + 1, &objfile->psymbol_cache);
	      xfree (structname);
      }
      else
      {
	      TYPE_TAG_NAME (type) =	/*obsavestring (DW_STRING (attr),
				   strlen (DW_STRING (attr)),
				   &objfile->type_obstack); */ bcache (DW_STRING (attr),
					       strlen (DW_STRING (attr)) + 1,
					       &objfile->psymbol_cache);
      }
    }

  if (die->tag == DW_TAG_structure_type)
    {
      TYPE_CODE (type) = TYPE_CODE_STRUCT;
    }
  else if (die->tag == DW_TAG_union_type)
    {
      TYPE_CODE (type) = TYPE_CODE_UNION;
    }
  else
    {
      /* FIXME: TYPE_CODE_CLASS is currently defined to TYPE_CODE_STRUCT
         in gdbtypes.h.  */
      TYPE_CODE (type) = TYPE_CODE_CLASS;
    }

  attr = dwarf2_attr (die, DW_AT_byte_size, cui);
  if (attr)
    {
      TYPE_LENGTH (type) = DW_UNSND (attr);
    }
  else
    {
      TYPE_LENGTH (type) = 0;
    }

  /* We need to add the type field to the die immediately so we don't
     infinitely recurse when dealing with pointers to the structure
     type within the structure itself. */
  die->type = type;

  /* We cannot assume that a declaration is a stub. You can define and
     declare something in the same place. */ 
  if (die->has_children)
    {
      struct field_info fi;
      struct die_info *child_die;
      struct cleanup *back_to = make_cleanup (null_cleanup, NULL);

      memset (&fi, 0, sizeof (struct field_info));

      child_die = die->next;

      while (child_die && child_die->tag)
	{
	  if (child_die->tag == DW_TAG_member)
	    {
	      dwarf2_add_field (&fi, child_die, objfile, cui);
	    }
	  else if (child_die->tag == DW_TAG_structure_type)
	    {
	      read_structure_scope (child_die, objfile, cui, TYPE_TAG_NAME (type));
	    }
	  else if (child_die->tag == DW_TAG_variable)
	    {
	      /* C++ static member.  */
	      dwarf2_add_field (&fi, child_die, objfile, cui);
	    }
	  else if (child_die->tag == DW_TAG_subprogram)
	    {
	      /* C++ member function. */
	      process_die (child_die, objfile, cui);
	      dwarf2_add_member_fn (&fi, child_die, type, objfile, cui);
	    }
	  else if (child_die->tag == DW_TAG_inheritance)
	    {
	      /* C++ base class field.  */
	      dwarf2_add_field (&fi, child_die, objfile, cui);
	    }
	  else
	    {
	      process_die (child_die, objfile, cui);
	    }
	  child_die = sibling_die (child_die);
	}

      /* Attach fields and member functions to the type.  */
      if (fi.nfields)
	dwarf2_attach_fields_to_type (&fi, type, objfile);
      if (fi.nfnfields)
	{
	  dwarf2_attach_fn_fields_to_type (&fi, type, objfile);

	  /* Get the type which refers to the base class (possibly this
	     class itself) which contains the vtable pointer for the current
	     class from the DW_AT_containing_type attribute.  */

	  if (dwarf2_attr (die, DW_AT_containing_type, cui) != NULL)
	    {
	      struct type *t = die_containing_type (die, objfile, cui);
	      TYPE_VPTR_BASETYPE (type) = t;
	      if (type == t)
		{
		  static const char vptr_name[] =
		    { '_', 'v', 'p', 't', 'r', '\0' };
		  int i;

		  /* Our own class provides vtbl ptr.  */
		  for (i = TYPE_NFIELDS (t) - 1;
		       i >= TYPE_N_BASECLASSES (t); --i)
		    {
		      char *fieldname = TYPE_FIELD_NAME (t, i);

		      if (STREQN
			  (fieldname, vptr_name, strlen (vptr_name) - 1)
			  && is_cplus_marker (fieldname[strlen (vptr_name)]))
			{
			  TYPE_VPTR_FIELDNO (type) = i;
			  break;
			}
		    }

		  /* Complain if virtual function table field not found.  */
		  if (i < TYPE_N_BASECLASSES (t))
		    complain (&dwarf2_vtbl_not_found_complaint,
			      TYPE_TAG_NAME (type) ? TYPE_TAG_NAME (type) :
			      "");
		}
	      else
		{
		  TYPE_VPTR_FIELDNO (type) = TYPE_VPTR_FIELDNO (t);
		}
	    }
	}

      new_symbol (die, type, objfile, cui);

      do_cleanups (back_to);
    }
  else
    {
      /* No children, must be stub. */
      TYPE_FLAGS (type) |= TYPE_FLAG_STUB;
    }

  die->type = type;
}

/* Given a pointer to a die which begins an enumeration, process all
   the dies that define the members of the enumeration.

   This will be much nicer in draft 6 of the DWARF spec when our
   members will be dies instead squished into the DW_AT_element_list
   attribute.

   NOTE: We reverse the order of the element list.  */

static void
read_enumeration (struct die_info *die, struct objfile *objfile,
		  const struct comp_unit_instance *cui)
{
  struct die_info *child_die;
  struct type *type;
  struct field *fields;
  struct attribute *attr;
  struct symbol *sym;
  int num_fields;
  int unsigned_enum = 1;

  type = alloc_type (objfile);

  TYPE_CODE (type) = TYPE_CODE_ENUM;
  attr = dwarf2_attr (die, DW_AT_name, cui);
  if (attr && DW_STRING (attr))
    {
      TYPE_TAG_NAME (type) =	/*obsavestring (DW_STRING (attr), strlen (DW_STRING (attr))
				   &objfile->type_obstack); */
	bcache (DW_STRING (attr), strlen (DW_STRING (attr)) + 1,
		&objfile->psymbol_cache);
    }

  attr = dwarf2_attr (die, DW_AT_byte_size, cui);
  if (attr)
    {
      TYPE_LENGTH (type) = DW_UNSND (attr);
    }
  else
    {
      TYPE_LENGTH (type) = 0;
    }

  num_fields = 0;
  fields = NULL;
  if (die->has_children)
    {
      child_die = die->next;
      while (child_die && child_die->tag)
	{
	  if (child_die->tag != DW_TAG_enumerator)
	    {
	      process_die (child_die, objfile, cui);
	    }
	  else
	    {
	      attr = dwarf2_attr (child_die, DW_AT_name, cui);
	      if (attr)
		{
		  sym = new_symbol (child_die, type, objfile, cui);
		  if (SYMBOL_VALUE (sym) < 0)
		    unsigned_enum = 0;

		  if ((num_fields % DW_FIELD_ALLOC_CHUNK) == 0)
		    {
		      fields = (struct field *)
			xrealloc (fields,
				  (num_fields + DW_FIELD_ALLOC_CHUNK)
				  * sizeof (struct field));
		    }

		  FIELD_NAME (fields[num_fields]) = SYMBOL_NAME (sym);
		  FIELD_TYPE (fields[num_fields]) = NULL;
		  FIELD_BITPOS (fields[num_fields]) = SYMBOL_VALUE (sym);
		  FIELD_BITSIZE (fields[num_fields]) = 0;

		  num_fields++;
		}
	    }

	  child_die = sibling_die (child_die);
	}

      if (num_fields)
	{
	  TYPE_NFIELDS (type) = num_fields;
	  TYPE_FIELDS (type) = (struct field *)
	    TYPE_ALLOC (type, sizeof (struct field) * num_fields);
	  memcpy (TYPE_FIELDS (type), fields,
		  sizeof (struct field) * num_fields);
	  xfree (fields);
	}
#if TYPEFIX
      if (unsigned_enum)
	TYPE_FLAGS (type) |= TYPE_FLAG_UNSIGNED;
#endif
    }
  die->type = type;
  new_symbol (die, type, objfile, cui);
}

/* Extract all information from a DW_TAG_array_type DIE and put it in
   the DIE's type field.  For now, this only handles one dimensional
   arrays.  */

static void
read_array_type (struct die_info *die, struct objfile *objfile,
		 const struct comp_unit_instance *cui)
{
  struct die_info *child_die;
  struct type *type = NULL;
  struct type *element_type, *index_type;
  struct range_type *range_type;
  struct range_type **range_types = NULL;
  struct attribute *attr;
  int ndim = 0;
  struct cleanup *back_to;

  /* Return if we've already decoded this type. */
  if (die->type)
    {
      return;
    }

  element_type = die_type (die, objfile, cui);

  /* Irix 6.2 native cc creates array types without children for
     arrays with unspecified length.  */
  if (die->has_children == 0)
    {
      index_type = dwarf2_fundamental_type (objfile, FT_INTEGER, cui);
      range_type = make_range_type (objfile, index_type, 0, -1);
      die->type = (struct type *)make_array_type (objfile, element_type, range_type);
      return;
    }

  back_to = make_cleanup (null_cleanup, NULL);
  child_die = die->next;
  while (child_die && child_die->tag)
    {
      if (child_die->tag == DW_TAG_subrange_type)
	{
	  unsigned int low, high;

	  /* Default bounds to an array with unspecified length.  */
	  low = 0;
	  high = -1;
	  if (cui->cu_language == language_fortran)
	    {
	      /* FORTRAN implies a lower bound of 1, if not given.  */
	      low = 1;
	    }

	  index_type = die_type (child_die, objfile, cui);
	  attr = dwarf2_attr (child_die, DW_AT_lower_bound, cui);
	  if (attr)
	    {
	      if (attr->form == DW_FORM_sdata)
		{
		  low = DW_SND (attr);
		}
	      else if (attr->form == DW_FORM_udata
		       || attr->form == DW_FORM_data1
		       || attr->form == DW_FORM_data2
		       || attr->form == DW_FORM_data4)
		{
		  low = DW_UNSND (attr);
		}
	      else
		{
		  complain (&dwarf2_non_const_array_bound_ignored,
			    dwarf2_form_name (attr->form));
#ifdef FORTRAN_HACK
		  die->type = lookup_pointer_type (element_type);
		  return;
#else
		  low = 0;
#endif
		}
	    }
	  attr = dwarf2_attr (child_die, DW_AT_upper_bound, cui);
	  if (attr)
	    {
	      if (attr->form == DW_FORM_sdata)
		{
		  high = DW_SND (attr);
		}
	      else if (attr->form == DW_FORM_udata
		       || attr->form == DW_FORM_data1
		       || attr->form == DW_FORM_data2
		       || attr->form == DW_FORM_data4)
		{
		  high = DW_UNSND (attr);
		}
	      else if (attr->form == DW_FORM_block1)
		{
		  /* GCC encodes arrays with unspecified or dynamic length
		     with a DW_FORM_block1 attribute.
		     FIXME: GDB does not yet know how to handle dynamic
		     arrays properly, treat them as arrays with unspecified
		     length for now.  */
		  high = -1;
		}
	      else
		{
		  complain (&dwarf2_non_const_array_bound_ignored,
			    dwarf2_form_name (attr->form));
#ifdef FORTRAN_HACK
		  die->type = lookup_pointer_type (element_type);
		  return;
#else
		  high = 1;
#endif
		}
	    }

	  /* Create a range type and save it for array type creation.  */
	  if ((ndim % DW_FIELD_ALLOC_CHUNK) == 0)
	    {
	      range_types = (struct range_type **)
		xrealloc (range_types, (ndim + DW_FIELD_ALLOC_CHUNK)
			  * sizeof (struct range_type *));
	      if (ndim == 0)
		make_cleanup (free_current_contents, &range_types);
	    }
	  range_types[ndim++] =
	    make_range_type (objfile, index_type, low, high);
	}
      child_die = sibling_die (child_die);
    }

  /* Dwarf2 dimensions are output from left to right, create the
     necessary array types in backwards order.  */
  type = element_type;
  while (ndim-- > 0)
    type = (struct type *)make_array_type (objfile, type, range_types[ndim]);

  do_cleanups (back_to);

  /* Install the type in the die. */
  die->type = type;
}

/* First cut: install each common block member as a global variable.  */

static void
read_common_block (struct die_info *die, struct objfile *objfile,
		   const struct comp_unit_instance *cui)
{
  struct die_info *child_die;
  struct attribute *attr;
  struct symbol *sym;
  CORE_ADDR base = (CORE_ADDR) 0;

  attr = dwarf2_attr (die, DW_AT_location, cui);
  if (attr)
    {
      base = decode_locdesc (DW_BLOCK (attr), objfile, cui);
    }
  if (die->has_children)
    {
      child_die = die->next;
      while (child_die && child_die->tag)
	{
	  sym = new_symbol (child_die, NULL, objfile, cui);
	  attr = dwarf2_attr (child_die, DW_AT_data_member_location, cui);
	  if (attr)
	    {
	      SYMBOL_VALUE_ADDRESS (sym) =
		base + decode_locdesc (DW_BLOCK (attr), objfile, cui);
	      add_symbol_to_list (sym, &global_symbols);
	    }
	  child_die = sibling_die (child_die);
	}
    }
}

/* Extract all information from a DW_TAG_pointer_type DIE and add to
   the user defined type vector.  */

static void
read_tag_pointer_type (struct die_info *die, struct objfile *objfile,
		       const struct comp_unit_instance *cui)
{
  struct type *type;
  struct attribute *attr;

  if (die->type)
    {
      return;
    }

  type = lookup_pointer_type (die_type (die, objfile, cui));
  attr = dwarf2_attr (die, DW_AT_byte_size, cui);
  if (attr)
    {
      TYPE_LENGTH (type) = DW_UNSND (attr);
    }
  else
    {
      TYPE_LENGTH (type) = cui->cu_header.addr_size;
    }
  die->type = type;
}

/* Extract all information from a DW_TAG_ptr_to_member_type DIE and add to
   the user defined type vector.  */

static void
read_tag_ptr_to_member_type (struct die_info *die, struct objfile *objfile,
			     const struct comp_unit_instance *cui)
{
  struct type *type;
  struct type *to_type;
  struct type *domain;

  if (die->type)
    {
      return;
    }

  type = alloc_type (objfile);
  to_type = die_type (die, objfile, cui);
  domain = die_containing_type (die, objfile, cui);
  smash_to_member_type (type, domain, to_type);

  die->type = type;
}

/* Extract all information from a DW_TAG_reference_type DIE and add to
   the user defined type vector.  */

static void
read_tag_reference_type (struct die_info *die, struct objfile *objfile,
			 const struct comp_unit_instance *cui)
{
  struct type *type;
  struct attribute *attr;

  if (die->type)
    {
      return;
    }

  type = lookup_reference_type (die_type (die, objfile, cui));
  attr = dwarf2_attr (die, DW_AT_byte_size, cui);
  if (attr)
    {
      TYPE_LENGTH (type) = DW_UNSND (attr);
    }
  else
    {
      TYPE_LENGTH (type) = cui->cu_header.addr_size;
    }
  die->type = type;
}

static void
read_tag_const_type (struct die_info *die, struct objfile *objfile,
		     const struct comp_unit_instance *cui)
{
  struct type *base_type;

  if (die->type)
    {
      return;
    }

  base_type = die_type (die, objfile, cui);
  die->type = make_cv_type (1, TYPE_VOLATILE (base_type), base_type, 0);
}

static void
read_tag_volatile_type (struct die_info *die, struct objfile *objfile,
			const struct comp_unit_instance *cui)
{
  struct type *base_type;

  if (die->type)
    {
      return;
    }

  base_type = die_type (die, objfile, cui);
  die->type = make_cv_type (TYPE_CONST (base_type), 1, base_type, 0);
}

/* Extract all information from a DW_TAG_string_type DIE and add to
   the user defined type vector.  It isn't really a user defined type,
   but it behaves like one, with other DIE's using an AT_user_def_type
   attribute to reference it.  */

static void
read_tag_string_type (struct die_info *die, struct objfile *objfile,
		      const struct comp_unit_instance *cui)
{
  struct type *type, *index_type, *char_type;
  struct range_type *range_type;
  struct attribute *attr;
  unsigned int length;

  if (die->type)
    {
      return;
    }

  attr = dwarf2_attr (die, DW_AT_string_length, cui);
  if (attr)
    {
      length = DW_UNSND (attr);
    }
  else
    {
      length = 1;
    }
  index_type = dwarf2_fundamental_type (objfile, FT_INTEGER, cui);
  range_type = make_range_type (objfile, index_type, 1, length);
/*  TYPEFIX : Is this still necessary?
    char_type = dwarf2_fundamental_type (objfile, FT_CHAR, cui);*/
  type = (struct type *)make_string_type (objfile, range_type);
  die->type = type;
}

/* Handle DIES due to C code like:

   struct foo
   {
   int (*funcp)(int a, long l);
   int b;
   };

   ('funcp' generates a DW_TAG_subroutine_type DIE)
 */

static void
read_subroutine_type (struct die_info *die, struct objfile *objfile,
		      const struct comp_unit_instance *cui)
{
#if TYPEFIX
  struct type *type;		/* Type that this function returns */
  struct type *ftype;		/* Function that returns above type */
  struct attribute *attr;

  /* Decode the type that this subroutine returns */
  if (die->type)
      return;

  type = die_type (die, objfile, cui);
  ftype = lookup_function_type (type);

  /* All functions in C++ have prototypes.  */
  attr = dwarf2_attr (die, DW_AT_prototyped, cui);
  if ((attr && (DW_UNSND (attr) != 0)) || cui->cu_language == language_cplus)
    TYPE_FLAGS (ftype) |= TYPE_FLAG_PROTOTYPED;

  if (die->has_children)
    {
      struct die_info *child_die;
      int nparams = 0;
      int iparams = 0;

      /* Count the number of parameters.
         FIXME: GDB currently ignores vararg functions, but knows about
         vararg member functions.  */
      child_die = die->next;
      while (child_die && child_die->tag)
	{
	  if (child_die->tag == DW_TAG_formal_parameter)
	    nparams++;
	  else if (child_die->tag == DW_TAG_unspecified_parameters)
	    TYPE_FLAGS (ftype) |= TYPE_FLAG_VARARGS;
	  child_die = sibling_die (child_die);
	}

      /* Allocate storage for parameters and fill them in.  */
      TYPE_NFIELDS (ftype) = nparams;
      TYPE_FIELDS (ftype) = (struct field *)
	TYPE_ALLOC (ftype, nparams * sizeof (struct field));

      child_die = die->next;
      while (child_die && child_die->tag)
	{
	  if (child_die->tag == DW_TAG_formal_parameter)
	    {
	      /* Dwarf2 has no clean way to discern C++ static and non-static
	         member functions. G++ helps GDB by marking the first
	         parameter for non-static member functions (which is the
	         this pointer) as artificial. We pass this information
	         to dwarf2_add_member_fn via TYPE_FIELD_ARTIFICIAL.  */
	      attr = dwarf2_attr (child_die, DW_AT_artificial, cui);
	      if (attr)
		TYPE_FIELD_ARTIFICIAL (ftype, iparams) = DW_UNSND (attr);
	      else
		TYPE_FIELD_ARTIFICIAL (ftype, iparams) = 0;
	      TYPE_FIELD_TYPE (ftype, iparams) = die_type (child_die, objfile,
							   cui);
	      iparams++;
	    }
	  child_die = sibling_die (child_die);
	}
    }

  die->type = ftype;
#endif
}

static void
read_typedef (struct die_info *die, struct objfile *objfile,
	      const struct comp_unit_instance *cui)
{
  struct type *type;

  if (!die->type)
    {
      struct attribute *attr;
      struct type *xtype;

      xtype = die_type (die, objfile, cui);

      type = alloc_type (objfile);
      TYPE_CODE (type) = TYPE_CODE_TYPEDEF;
      TYPE_FLAGS (type) |= TYPE_FLAG_TARGET_STUB;
      TYPE_TARGET_TYPE (type) = xtype;
      attr = dwarf2_attr (die, DW_AT_name, cui);
      if (attr && DW_STRING (attr))
	TYPE_NAME (type) = /*obsavestring (DW_STRING (attr),
				   strlen (DW_STRING (attr)),
				   &objfile->type_obstack); */ bcache (DW_STRING (attr),
						 strlen (DW_STRING (attr)) + 1,
						 &objfile->psymbol_cache);

      die->type = type;
    }
}

/* Find a representation of a given base type and install
   it in the TYPE field of the die.  */

static void
read_base_type (struct die_info *die, struct objfile *objfile,
		const struct comp_unit_instance *cui)
{
  struct type *type;
  struct attribute *attr;
  int encoding = 0, size = 0;

  /* If we've already decoded this die, this is a no-op. */
  if (die->type)
    {
      return;
    }

  attr = dwarf2_attr (die, DW_AT_encoding, cui);
  if (attr)
    {
      encoding = DW_UNSND (attr);
    }
  attr = dwarf2_attr (die, DW_AT_byte_size, cui);
  if (attr)
    {
      size = DW_UNSND (attr);
    }
  attr = dwarf2_attr (die, DW_AT_name, cui);
  if (attr && DW_STRING (attr))
    {
      enum type_code code = TYPE_CODE_INT;
      int is_unsigned = 0;

      switch (encoding)
	{
	case DW_ATE_address:
	  /* Turn DW_ATE_address into a void * pointer.  */
	  code = TYPE_CODE_PTR;
	  is_unsigned = 1;
	  break;
	case DW_ATE_boolean:
	  code = TYPE_CODE_BOOL;
	  is_unsigned = 1;
	  break;
	case DW_ATE_complex_float:
	  code = TYPE_CODE_COMPLEX;
	  break;
	case DW_ATE_float:
	  code = TYPE_CODE_FLT;
	  break;
	case DW_ATE_signed:
	case DW_ATE_signed_char:
	  break;
	case DW_ATE_unsigned:
	case DW_ATE_unsigned_char:
	  is_unsigned = 1;
	  break;
	default:
	  complain (&dwarf2_unsupported_at_encoding,
		    dwarf2_type_encoding_name (encoding));
	  break;
	}
      if (encoding == DW_ATE_address)
	type = (struct type *)make_pointer_type (objfile, dwarf2_fundamental_type (objfile, FT_VOID, cui));
      else
	type = init_type (code, size, is_unsigned, DW_STRING (attr), objfile);

    }
  else
    {
      type = dwarf2_base_type (encoding, size, objfile, cui);
    }
  die->type = type;
}

/* Read a whole compilation unit into a linked list of dies.  */

struct die_info *
read_comp_unit_dies (char *info_ptr, bfd * abfd,
		     const struct comp_unit_instance *cui)
{
  struct die_info *first_die, *last_die, *die;
  char *cur_ptr;
  int nesting_level;

  /* Reset die reference table; we are
     building new ones now.  */
  dwarf2_empty_hash_tables ((struct comp_unit_instance *) cui);

  cur_ptr = info_ptr;
  nesting_level = 0;
  first_die = last_die = NULL;
  do
    {
      cur_ptr = read_full_die (&die, abfd, cur_ptr, cui);
      if (die->has_children)
	{
	  nesting_level++;
	}
      if (die->tag == 0)
	{
	  nesting_level--;
	}

      die->next = NULL;

      /* Enter die in reference hash table */
      store_in_ref_table (die->offset, die,
			  (struct comp_unit_instance *) cui);

      if (!first_die)
	{
	  first_die = last_die = die;
	}
      else
	{
	  last_die->next = die;
	  last_die = die;
	}
    }
  while (nesting_level > 0);
  return first_die;
}

/* Free a linked list of dies.  */

static void
free_die_list (struct die_info *dies)
{
  struct die_info *die, *next;

  die = dies;
  while (die)
    {
      next = die->next;
      xfree (die->attrs);
      xfree (die);
      die = next;
    }
}

static void
do_free_die_list_cleanup (void *dies)
{
  free_die_list (dies);
}

static struct cleanup *
make_cleanup_free_die_list (struct die_info *dies)
{
  return make_cleanup (do_free_die_list_cleanup, dies);
}


/* Read the contents of the section at OFFSET and of size SIZE from the
   object file specified by OBJFILE and return it.  */

static char *
dwarf2_read_section (struct objfile *objfile, file_ptr offset,
		     unsigned int size)
{
  bfd *abfd = objfile->obfd;
  char *buf;

  if (size == 0)
    return NULL;
  buf = (char *) xmalloc (size);
  if ((bfd_seek (abfd, offset, SEEK_SET) != 0) ||
      (bfd_read (buf, size, 1, abfd) != size))
    {
      buf = NULL;
      error ("Dwarf Error: Can't read DWARF data from '%s'",
	     bfd_get_filename (abfd));
    }
  return buf;
}

/* In DWARF version 2, the description of the debugging information is
   stored in a separate .debug_abbrev section.  Before we read any
   dies from a section we read in all abbreviations and install them
   in a hash table.  */

static void
dwarf2_read_abbrevs (bfd * abfd, struct comp_unit_instance *cui,
		     unsigned int offset)
{
  char *abbrev_ptr;
  struct abbrev_info *cur_abbrev;
  unsigned int abbrev_number, bytes_read, abbrev_name;
  unsigned int abbrev_form, hash_number;

  /* empty the table */
  dwarf2_empty_abbrev_table (cui);

  abbrev_ptr = dwarf2_abbrev_buffer + offset;
  abbrev_number = read_unsigned_leb128 (abfd, abbrev_ptr, &bytes_read);
  abbrev_ptr += bytes_read;

  /* loop until we reach an abbrev number of 0 */
  while (abbrev_number)
    {
      cur_abbrev = dwarf2_alloc_abbrev ();

      /* read in abbrev header */
      cur_abbrev->number = abbrev_number;
      cur_abbrev->tag = read_unsigned_leb128 (abfd, abbrev_ptr, &bytes_read);
      abbrev_ptr += bytes_read;
      cur_abbrev->has_children = read_1_byte (abfd, abbrev_ptr);
      abbrev_ptr += 1;

      /* now read in declarations */
      abbrev_name = read_unsigned_leb128 (abfd, abbrev_ptr, &bytes_read);
      abbrev_ptr += bytes_read;
      abbrev_form = read_unsigned_leb128 (abfd, abbrev_ptr, &bytes_read);
      abbrev_ptr += bytes_read;
      while (abbrev_name)
	{
	  if ((cur_abbrev->num_attrs % ATTR_ALLOC_CHUNK) == 0)
	    {
	      cur_abbrev->attrs = (struct attr_abbrev *)
		xrealloc (cur_abbrev->attrs,
			  (cur_abbrev->num_attrs + ATTR_ALLOC_CHUNK)
			  * sizeof (struct attr_abbrev));
	    }
	  cur_abbrev->attrs[cur_abbrev->num_attrs].name = abbrev_name;
	  cur_abbrev->attrs[cur_abbrev->num_attrs++].form = abbrev_form;
	  abbrev_name = read_unsigned_leb128 (abfd, abbrev_ptr, &bytes_read);
	  abbrev_ptr += bytes_read;
	  abbrev_form = read_unsigned_leb128 (abfd, abbrev_ptr, &bytes_read);
	  abbrev_ptr += bytes_read;
	}

      hash_number = abbrev_number % ABBREV_HASH_SIZE;
      cur_abbrev->next = cui->dwarf2_abbrevs[hash_number];
      cui->dwarf2_abbrevs[hash_number] = cur_abbrev;

      /* Get next abbreviation.
         Under Irix6 the abbreviations for a compilation unit are not
         always properly terminated with an abbrev number of 0.
         Exit loop if we encounter an abbreviation which we have
         already read (which means we are about to read the abbreviations
         for the next compile unit) or if the end of the abbreviation
         table is reached.  */
      if ((unsigned int) (abbrev_ptr - dwarf2_abbrev_buffer)
	  >= dwarf2_abbrev_size)
	break;
      abbrev_number = read_unsigned_leb128 (abfd, abbrev_ptr, &bytes_read);
      abbrev_ptr += bytes_read;
      if (dwarf2_lookup_abbrev (abbrev_number, cui) != NULL)
	break;
    }
}

/* Empty the abbrev table for a new compilation unit.  */

/* ARGSUSED */
static void
dwarf2_empty_abbrev_table (PTR cu)
{
  int i;
  struct abbrev_info *abbrev, *next;
  struct comp_unit_instance *cui = (struct comp_unit_instance *) cu;
  for (i = 0; i < ABBREV_HASH_SIZE; ++i)
    {
      next = NULL;
      abbrev = cui->dwarf2_abbrevs[i];
      while (abbrev)
	{
	  next = abbrev->next;
	  xfree (abbrev->attrs);
	  xfree (abbrev);
	  abbrev = next;
	}
      cui->dwarf2_abbrevs[i] = NULL;
    }
}

/* Lookup an abbrev_info structure in the abbrev hash table.  */

static struct abbrev_info *
dwarf2_lookup_abbrev (unsigned int number,
		      const struct comp_unit_instance *cui)
{
  unsigned int hash_number;
  struct abbrev_info *abbrev;
  struct comp_unit_instance *cui_for_die;


  hash_number = number % ABBREV_HASH_SIZE;
  abbrev = cui->dwarf2_abbrevs[hash_number];

  while (abbrev)
    {
      if (abbrev->number == number)
	return abbrev;
      else
	abbrev = abbrev->next;
    }
  return NULL;
}

/* Read a minimal amount of information into the minimal die structure.  */

static char *
read_partial_die (struct partial_die_info *part_die, bfd * abfd,
		  char *info_ptr, int *has_pc_info,
		  const struct comp_unit_instance *cui)
{
  unsigned int abbrev_number, bytes_read, i;
  struct abbrev_info *abbrev;
  struct attribute attr;
  struct attribute spec_attr;
  int found_spec_attr = 0;
  int has_low_pc_attr = 0;
  int has_high_pc_attr = 0;


  memset(part_die, 0, sizeof(struct partial_die_info));
  *has_pc_info = 0;
  abbrev_number = read_unsigned_leb128 (abfd, info_ptr, &bytes_read);
  info_ptr += bytes_read;
  if (!abbrev_number)
    return info_ptr;

  abbrev = dwarf2_lookup_abbrev (abbrev_number, cui);
  if (!abbrev)
    {
      error ("Dwarf Error: Could not find abbrev number %d.", abbrev_number);
    }
  part_die->offset = info_ptr - dwarf2_info_buffer;
  part_die->tag = abbrev->tag;
  part_die->has_children = abbrev->has_children;
  part_die->abbrev = abbrev_number;

  for (i = 0; i < abbrev->num_attrs; ++i)
    {
      info_ptr = read_attribute (&attr, &abbrev->attrs[i], abfd,
				 info_ptr, cui);

      /* Store the data if it is of an attribute we want to keep in a
         partial symbol table.  */
      switch (attr.name)
	{
	case DW_AT_name:
	  /* Prefer DW_AT_MIPS_linkage_name over DW_AT_name.  */
	  if (part_die->name == NULL)
	    part_die->name = DW_STRING (&attr);
	  break;
	case DW_AT_MIPS_linkage_name:
	  part_die->name = DW_STRING (&attr);
	  break;
	case DW_AT_low_pc:
	  has_low_pc_attr = 1;
	  part_die->lowpc = DW_ADDR (&attr);
	  break;
	case DW_AT_high_pc:
	  has_high_pc_attr = 1;
	  part_die->highpc = DW_ADDR (&attr);
	  break;
	case DW_AT_location:
	  part_die->locdesc = DW_BLOCK (&attr);
	  break;
	case DW_AT_language:
	  part_die->language = DW_UNSND (&attr);
	  break;
	case DW_AT_external:
	  part_die->is_external = DW_UNSND (&attr);
	  break;
	case DW_AT_declaration:
	  part_die->is_declaration = DW_UNSND (&attr);
	  break;
	case DW_AT_type:
	  part_die->has_type = 1;
	  break;
	case DW_AT_abstract_origin:
	case DW_AT_specification:
	  found_spec_attr = 1;
	  spec_attr = attr;
	  break;
	case DW_AT_sibling:
	  /* Ignore absolute siblings, they might point outside of
	     the current compile unit.  */
	  if (attr.form == DW_FORM_ref_addr)
	    complain (&dwarf2_absolute_sibling_complaint);
	  else
	    part_die->sibling =
	      dwarf2_info_buffer + dwarf2_get_ref_die_offset (cui, &attr);
	  break;
	default:
	  break;
	}
    }

  /* If we found a reference attribute and the die has no name, try
     to find a name in the referred to die.  */

  if (found_spec_attr && part_die->name == NULL)
    {
      struct partial_die_info spec_die;
      char *spec_ptr;
      int dummy;
      part_die->name = get_pubname_for_offset (abfd, part_die->offset,  cui);
      spec_ptr =
	dwarf2_info_buffer + dwarf2_get_ref_die_offset (cui, &spec_attr);
      read_partial_die (&spec_die, abfd, spec_ptr, &dummy, cui);
      if (spec_die.name)
	{
	  if (part_die->name == NULL)
		  part_die->name = spec_die.name;

	  /* Copy DW_AT_external attribute if it is set.  */
	  if (spec_die.is_external)
	    part_die->is_external = spec_die.is_external;
	}
    }

  /* When using the GNU linker, .gnu.linkonce. sections are used to
     eliminate duplicate copies of functions and vtables and such.
     The linker will arbitrarily choose one and discard the others.
     The AT_*_pc values for such functions refer to local labels in
     these sections.  If the section from that file was discarded, the
     labels are not in the output, so the relocs get a value of 0.
     If this is a discarded function, mark the pc bounds as invalid,
     so that GDB will ignore it.  */
  if (has_low_pc_attr && has_high_pc_attr
      && part_die->lowpc < part_die->highpc
      && (part_die->lowpc != 0 || (bfd_get_file_flags (abfd) & HAS_RELOC)))
    *has_pc_info = 1;
  return info_ptr;
}

/* Read the die from the .debug_info section buffer.  And set diep to
   point to a newly allocated die with its information.  */

static char *
read_full_die (struct die_info **diep, bfd * abfd, char *info_ptr,
	       const struct comp_unit_instance *cui)
{
  unsigned int abbrev_number, bytes_read, i, offset;
  struct abbrev_info *abbrev;
  struct die_info *die;

  offset = info_ptr - dwarf2_info_buffer;
  abbrev_number = read_unsigned_leb128 (abfd, info_ptr, &bytes_read);
  info_ptr += bytes_read;
  if (!abbrev_number)
    {
      die = dwarf2_alloc_die ();
      die->tag = 0;
      die->abbrev = abbrev_number;
      die->type = NULL;
      *diep = die;
      return info_ptr;
    }

  abbrev = dwarf2_lookup_abbrev (abbrev_number, cui);
  if (!abbrev)
    {
      error ("Dwarf Error: could not find abbrev number %d.", abbrev_number);
    }
  die = dwarf2_alloc_die ();
  die->offset = offset;
  die->tag = abbrev->tag;
  die->has_children = abbrev->has_children;
  die->abbrev = abbrev_number;
  die->type = NULL;

  die->num_attrs = abbrev->num_attrs;
  die->attrs = (struct attribute *)
    xmalloc (die->num_attrs * sizeof (struct attribute));

  for (i = 0; i < abbrev->num_attrs; ++i)
    {
      info_ptr = read_attribute (&die->attrs[i], &abbrev->attrs[i],
				 abfd, info_ptr, cui);
    }

  *diep = die;
  return info_ptr;
}

/* Read an attribute described by an abbreviated attribute.  */

static char *
read_attribute (struct attribute *attr, struct attr_abbrev *abbrev,
		bfd * abfd, char *info_ptr,
		const struct comp_unit_instance *cui)
{
  unsigned int bytes_read;
  struct dwarf2_block *blk;

  attr->name = abbrev->name;
  attr->form = abbrev->form;
  switch (abbrev->form)
    {
    case DW_FORM_addr:
    case DW_FORM_ref_addr:
      DW_ADDR (attr) = read_address (abfd, info_ptr, cui, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_block2:
      blk = dwarf2_alloc_block ();
      blk->size = read_2_bytes (abfd, info_ptr);
      info_ptr += 2;
      blk->data = read_n_bytes (abfd, info_ptr, blk->size);
      info_ptr += blk->size;
      DW_BLOCK (attr) = blk;
      break;
    case DW_FORM_block4:
      blk = dwarf2_alloc_block ();
      blk->size = read_4_bytes (abfd, info_ptr);
      info_ptr += 4;
      blk->data = read_n_bytes (abfd, info_ptr, blk->size);
      info_ptr += blk->size;
      DW_BLOCK (attr) = blk;
      break;
    case DW_FORM_data2:
      DW_UNSND (attr) = read_2_bytes (abfd, info_ptr);
      info_ptr += 2;
      break;
    case DW_FORM_data4:
      DW_UNSND (attr) = read_4_bytes (abfd, info_ptr);
      info_ptr += 4;
      break;
    case DW_FORM_data8:
      DW_UNSND (attr) = read_8_bytes (abfd, info_ptr);
      info_ptr += 8;
      break;
    case DW_FORM_string:
      DW_STRING (attr) = read_string (abfd, info_ptr, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_block:
      blk = dwarf2_alloc_block ();
      blk->size = read_unsigned_leb128 (abfd, info_ptr, &bytes_read);
      info_ptr += bytes_read;
      blk->data = read_n_bytes (abfd, info_ptr, blk->size);
      info_ptr += blk->size;
      DW_BLOCK (attr) = blk;
      break;
    case DW_FORM_block1:
      blk = dwarf2_alloc_block ();
      blk->size = read_1_byte (abfd, info_ptr);
      info_ptr += 1;
      blk->data = read_n_bytes (abfd, info_ptr, blk->size);
      info_ptr += blk->size;
      DW_BLOCK (attr) = blk;
      break;
    case DW_FORM_data1:
      DW_UNSND (attr) = read_1_byte (abfd, info_ptr);
      info_ptr += 1;
      break;
    case DW_FORM_flag:
      DW_UNSND (attr) = read_1_byte (abfd, info_ptr);
      info_ptr += 1;
      break;
    case DW_FORM_sdata:
      DW_SND (attr) = read_signed_leb128 (abfd, info_ptr, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_udata:
      DW_UNSND (attr) = read_unsigned_leb128 (abfd, info_ptr, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_ref1:
      DW_UNSND (attr) = read_1_byte (abfd, info_ptr);
      info_ptr += 1;
      break;
    case DW_FORM_ref2:
      DW_UNSND (attr) = read_2_bytes (abfd, info_ptr);
      info_ptr += 2;
      break;
    case DW_FORM_ref4:
      DW_UNSND (attr) = read_4_bytes (abfd, info_ptr);
      info_ptr += 4;
      break;
    case DW_FORM_ref8:
      DW_UNSND (attr) = read_8_bytes (abfd, info_ptr);
      info_ptr += 8;
      break;
    case DW_FORM_ref_udata:
      DW_UNSND (attr) = read_unsigned_leb128 (abfd, info_ptr, &bytes_read);
      info_ptr += bytes_read;
      break;
    case DW_FORM_strp:
    case DW_FORM_indirect:
    default:
      error ("Dwarf Error: Cannot handle %s in DWARF reader.",
	     dwarf2_form_name (abbrev->form));
    }
  return info_ptr;
}

/* read dwarf information from a buffer */

static unsigned int
read_1_byte (bfd * abfd, char *buf)
{
  return bfd_get_8 (abfd, (bfd_byte *) buf);
}

static int
read_1_signed_byte (bfd * abfd, char *buf)
{
  return bfd_get_signed_8 (abfd, (bfd_byte *) buf);
}

static unsigned int
read_2_bytes (bfd * abfd, char *buf)
{
  return bfd_get_16 (abfd, (bfd_byte *) buf);
}

static int
read_2_signed_bytes (bfd * abfd, char *buf)
{
  return bfd_get_signed_16 (abfd, (bfd_byte *) buf);
}

static unsigned int
read_4_bytes (bfd * abfd, char *buf)
{
  return bfd_get_32 (abfd, (bfd_byte *) buf);
}

static int
read_4_signed_bytes (bfd * abfd, char *buf)
{
  return bfd_get_signed_32 (abfd, (bfd_byte *) buf);
}

static unsigned long
read_8_bytes (bfd * abfd, char *buf)
{
  return bfd_get_64 (abfd, (bfd_byte *) buf);
}

static CORE_ADDR
read_address (bfd * abfd, char *buf, const struct comp_unit_instance *cui,
	      int *bytes_read)
{
  CORE_ADDR retval = 0;

  if (cui->cu_header.signed_addr_p)
    {
      switch (cui->cu_header.addr_size)
	{
	case 2:
	  retval = bfd_get_signed_16 (abfd, (bfd_byte *) buf);
	  break;
	case 4:
	  retval = bfd_get_signed_32 (abfd, (bfd_byte *) buf);
	  break;
	case 8:
	  retval = bfd_get_signed_64 (abfd, (bfd_byte *) buf);
	  break;
	default:
	  internal_error (__FILE__, __LINE__,
			  "read_address: bad switch, signed");
	}
    }
  else
    {
      switch (cui->cu_header.addr_size)
	{
	case 2:
	  retval = bfd_get_16 (abfd, (bfd_byte *) buf);
	  break;
	case 4:
	  retval = bfd_get_32 (abfd, (bfd_byte *) buf);
	  break;
	case 8:
	  retval = bfd_get_64 (abfd, (bfd_byte *) buf);
	  break;
	default:
	  internal_error (__FILE__, __LINE__,
			  "read_address: bad switch, unsigned");
	}
    }

  *bytes_read = cui->cu_header.addr_size;
  return retval;
}

/* Reads the initial length from a section.  The (draft) DWARF 2.1
   specification allows the initial length to take up either 4 bytes
   or 12 bytes.  If the first 4 bytes are 0xffffffff, then the next 8
   bytes describe the length and all offsets will be 8 bytes in length
   instead of 4.

   The value returned via bytes_read should be used to increment
   the relevant pointer after calling read_initial_length().
   
   As a side effect, this function sets the fields initial_length_size
   and offset_size in cu_header to the values appropriate for the
   length field.  (The format of the initial length field determines
   the width of file offsets to be fetched later with fetch_offset().)
   
   [ Note:  read_initial_length() and read_offset() are based on the
     document entitled "DWARF Debugging Information Format", revision
     2.1, draft 4, dated July 20, 2000.  This document was obtained
     from:

	http://reality.sgi.com/dehnert_engr/dwarf/dwarf2p1-draft4-000720.pdf
     
     This document is only a draft and is subject to change.  (So beware.)

     - Kevin, Aug 4, 2000
   ] */

static LONGEST
read_initial_length (bfd * abfd, char *buf, struct comp_unit_head *cu_header,
		     int *bytes_read)
{
  LONGEST retval = 0;

  retval = bfd_get_32 (abfd, (bfd_byte *) buf);

  if (retval == 0xffffffff)
    {
      retval = bfd_get_64 (abfd, (bfd_byte *) buf + 4);
      *bytes_read = 12;
      if (cu_header != NULL)
	{
	  cu_header->initial_length_size = 12;
	  cu_header->offset_size = 8;
	}
    }
  else
    {
      *bytes_read = 4;
      if (cu_header != NULL)
	{
	  cu_header->initial_length_size = 4;
	  cu_header->offset_size = 4;
	}
    }

  return retval;
}

/* Read an offset from the data stream.  The size of the offset is
   given by cu_header->offset_size. */

static LONGEST
read_offset (bfd * abfd, char *buf, const struct comp_unit_head *cu_header,
	     int *bytes_read)
{
  LONGEST retval = 0;

  switch (cu_header->offset_size)
    {
    case 4:
      retval = bfd_get_32 (abfd, (bfd_byte *) buf);
      *bytes_read = 4;
      break;
    case 8:
      retval = bfd_get_64 (abfd, (bfd_byte *) buf);
      *bytes_read = 8;
      break;
    default:
      internal_error (__FILE__, __LINE__, "read_offset: bad switch");
    }

  return retval;
}

static char *
read_n_bytes (bfd * abfd, char *buf, unsigned int size)
{
  /* If the size of a host char is 8 bits, we can return a pointer
     to the buffer, otherwise we have to copy the data to a buffer
     allocated on the temporary obstack.  */
#if HOST_CHAR_BIT == 8
  return buf;
#else
  char *ret;
  unsigned int i;

  ret = obstack_alloc (&dwarf2_tmp_obstack, size);
  for (i = 0; i < size; ++i)
    {
      ret[i] = bfd_get_8 (abfd, (bfd_byte *) buf);
      buf++;
    }
  return ret;
#endif
}

static char *
read_string (bfd * abfd, char *buf, unsigned int *bytes_read_ptr)
{
  /* If the size of a host char is 8 bits, we can return a pointer
     to the string, otherwise we have to copy the string to a buffer
     allocated on the temporary obstack.  */
#if HOST_CHAR_BIT == 8
  if (*buf == '\0')
    {
      *bytes_read_ptr = 1;
      return NULL;
    }
  *bytes_read_ptr = strlen (buf) + 1;
  return buf;
#else
  int byte;
  unsigned int i = 0;

  while ((byte = bfd_get_8 (abfd, (bfd_byte *) buf)) != 0)
    {
      obstack_1grow (&dwarf2_tmp_obstack, byte);
      i++;
      buf++;
    }
  if (i == 0)
    {
      *bytes_read_ptr = 1;
      return NULL;
    }
  obstack_1grow (&dwarf2_tmp_obstack, '\0');
  *bytes_read_ptr = i + 1;
  return obstack_finish (&dwarf2_tmp_obstack);
#endif
}

static unsigned long
read_unsigned_leb128 (bfd * abfd, char *buf, unsigned int *bytes_read_ptr)
{
  unsigned long result;
  unsigned int num_read;
  int i, shift;
  unsigned char byte;

  result = 0;
  shift = 0;
  num_read = 0;
  i = 0;
  while (1)
    {
      byte = bfd_get_8 (abfd, (bfd_byte *) buf);
      buf++;
      num_read++;
      result |= ((unsigned long) (byte & 127) << shift);
      if ((byte & 128) == 0)
	{
	  break;
	}
      shift += 7;
    }
  *bytes_read_ptr = num_read;
  return result;
}

static long
read_signed_leb128 (bfd * abfd, char *buf, unsigned int *bytes_read_ptr)
{
  long result;
  int i, shift, size, num_read;
  unsigned char byte;

  result = 0;
  shift = 0;
  size = 32;
  num_read = 0;
  i = 0;
  while (1)
    {
      byte = bfd_get_8 (abfd, (bfd_byte *) buf);
      buf++;
      num_read++;
      result |= ((long) (byte & 127) << shift);
      shift += 7;
      if ((byte & 128) == 0)
	{
	  break;
	}
    }
  if ((shift < size) && (byte & 0x40))
    {
      result |= -(1 << shift);
    }
  *bytes_read_ptr = num_read;
  return result;
}

static void
set_cu_language (struct comp_unit_instance *cui, unsigned int lang)
{
  switch (lang)
    {
    case DW_LANG_C89:
    case DW_LANG_C:
      cui->cu_language = language_c;
      break;
    case DW_LANG_C_plus_plus:
      cui->cu_language = language_cplus;
      break;
    case DW_LANG_Fortran77:
    case DW_LANG_Fortran90:
      cui->cu_language = language_fortran;
      break;
    case DW_LANG_Mips_Assembler:
      cui->cu_language = language_asm;
      break;
    case DW_LANG_Java:
      cui->cu_language = language_java;
      break;
    case DW_LANG_Ada83:
    case DW_LANG_Cobol74:
    case DW_LANG_Cobol85:
    case DW_LANG_Pascal83:
    case DW_LANG_Modula2:
    default:
      cui->cu_language = language_unknown;
      break;
    }
  cui->cu_language_defn = language_def (cui->cu_language);
}

/* Return the named attribute or NULL if not there.  */

static struct attribute *
dwarf2_attr (struct die_info *die, unsigned int name,
	    const struct comp_unit_instance *cui)
{
  unsigned int i;
  struct attribute *spec = NULL;

  for (i = 0; i < die->num_attrs; ++i)
    {
      if (die->attrs[i].name == name)
	  return &die->attrs[i];

      if (die->attrs[i].name == DW_AT_specification
	  || die->attrs[i].name == DW_AT_abstract_origin)
	spec = &die->attrs[i];
    }
  if (spec)
    {
      struct die_info *ref_die =
	follow_die_ref (dwarf2_get_ref_die_offset (cui, spec), cui);
      if (ref_die)
	return dwarf2_attr (ref_die, name, cui);
    }
  return NULL;
}

/* Decode the line number information for the compilation unit whose
   line number info is at OFFSET in the .debug_line section.
   The compilation directory of the file is passed in COMP_DIR.  */

struct filenames
{
  unsigned int num_files;
  struct fileinfo
  {
    char *name;
    unsigned int dir;
    unsigned int time;
    unsigned int size;
  }
   *files;
};

struct directories
{
  unsigned int num_dirs;
  char **dirs;
};

static void
dwarf2_decode_lines (unsigned int offset, char *comp_dir, bfd * abfd,
		    const struct comp_unit_instance *cui)
{
  char *line_ptr;
  char *line_end;
  struct line_head lh;
  struct cleanup *back_to;
  unsigned int i, bytes_read;
  char *cur_file, *cur_dir;
  unsigned char op_code, extended_op, adj_opcode;

#define FILE_ALLOC_CHUNK 5
#define DIR_ALLOC_CHUNK 5

  struct filenames files;
  struct directories dirs;

  if (dwarf2_line_buffer == NULL)
    {
      complain (&dwarf2_missing_line_number_section);
      return;
    }

  files.num_files = 0;
  files.files = NULL;

  dirs.num_dirs = 0;
  dirs.dirs = NULL;

  line_ptr = dwarf2_line_buffer + offset;

  /* read in the prologue */
  lh.total_length = read_initial_length (abfd, line_ptr, NULL, &bytes_read);
  line_ptr += bytes_read;
  line_end = line_ptr + lh.total_length;
  lh.version = read_2_bytes (abfd, line_ptr);
  line_ptr += 2;
  lh.prologue_length =
    read_offset (abfd, line_ptr, &cui->cu_header, &bytes_read);
  line_ptr += bytes_read;
  lh.minimum_instruction_length = read_1_byte (abfd, line_ptr);
  line_ptr += 1;
  lh.default_is_stmt = read_1_byte (abfd, line_ptr);
  line_ptr += 1;
  lh.line_base = read_1_signed_byte (abfd, line_ptr);
  line_ptr += 1;
  lh.line_range = read_1_byte (abfd, line_ptr);
  line_ptr += 1;
  lh.opcode_base = read_1_byte (abfd, line_ptr);
  line_ptr += 1;
  lh.standard_opcode_lengths = (unsigned char *)
    xmalloc (lh.opcode_base * sizeof (unsigned char));
  back_to = make_cleanup (free_current_contents, &lh.standard_opcode_lengths);

  lh.standard_opcode_lengths[0] = 1;
  for (i = 1; i < lh.opcode_base; ++i)
    {
      lh.standard_opcode_lengths[i] = read_1_byte (abfd, line_ptr);
      line_ptr += 1;
    }

  /* Read directory table  */
  while ((cur_dir = read_string (abfd, line_ptr, &bytes_read)) != NULL)
    {
      line_ptr += bytes_read;
      if ((dirs.num_dirs % DIR_ALLOC_CHUNK) == 0)
	{
	  dirs.dirs = (char **)
	    xrealloc (dirs.dirs,
		      (dirs.num_dirs + DIR_ALLOC_CHUNK) * sizeof (char *));
	  if (dirs.num_dirs == 0)
	    make_cleanup (free_current_contents, &dirs.dirs);
	}
      dirs.dirs[dirs.num_dirs++] = cur_dir;
    }
  line_ptr += bytes_read;

  /* Read file name table */
  while ((cur_file = read_string (abfd, line_ptr, &bytes_read)) != NULL)
    {
      line_ptr += bytes_read;
      if ((files.num_files % FILE_ALLOC_CHUNK) == 0)
	{
	  files.files = (struct fileinfo *)
	    xrealloc (files.files,
		      (files.num_files + FILE_ALLOC_CHUNK)
		      * sizeof (struct fileinfo));
	  if (files.num_files == 0)
	    make_cleanup (free_current_contents, &files.files);
	}
      files.files[files.num_files].name = cur_file;
      files.files[files.num_files].dir =
	read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
      line_ptr += bytes_read;
      files.files[files.num_files].time =
	read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
      line_ptr += bytes_read;
      files.files[files.num_files].size =
	read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
      line_ptr += bytes_read;
      files.num_files++;
    }
  line_ptr += bytes_read;

  /* Read the statement sequences until there's nothing left.  */
  while (line_ptr < line_end)
    {
      /* state machine registers  */
      CORE_ADDR address = 0;
      unsigned int file = 1;
      unsigned int line = 1;
      unsigned int column = 0;
      int is_stmt = lh.default_is_stmt;
      int basic_block = 0;
      int end_sequence = 0;

      /* Start a subfile for the current file of the state machine.  */
      if (files.num_files >= file)
	{
	  /* The file and directory tables are 0 based, the references
	     are 1 based.  */
	  dwarf2_start_subfile (files.files[file - 1].name,
				(files.files[file - 1].dir
				 ? dirs.dirs[files.files[file - 1].dir - 1]
				 : comp_dir));
	}

      /* Decode the table. */
      while (!end_sequence)
	{
	  op_code = read_1_byte (abfd, line_ptr);
	  line_ptr += 1;
	  switch (op_code)
	    {
	    case DW_LNS_extended_op:
	      line_ptr += 1;	/* ignore length */
	      extended_op = read_1_byte (abfd, line_ptr);
	      line_ptr += 1;
	      switch (extended_op)
		{
		case DW_LNE_end_sequence:
		  end_sequence = 1;
		  /* Don't call record_line here.  The end_sequence
		     instruction provides the address of the first byte
		     *after* the last line in the sequence; it's not the
		     address of any real source line.  However, the GDB
		     linetable structure only records the starts of lines,
		     not the ends.  This is a weakness of GDB.  */
		  break;
		case DW_LNE_set_address:
		  address = read_address (abfd, line_ptr, cui, &bytes_read);
		  line_ptr += bytes_read;
		  address += baseaddr;
		  break;
		case DW_LNE_define_file:
		  cur_file = read_string (abfd, line_ptr, &bytes_read);
		  line_ptr += bytes_read;
		  if ((files.num_files % FILE_ALLOC_CHUNK) == 0)
		    {
		      files.files = (struct fileinfo *)
			xrealloc (files.files,
				  (files.num_files + FILE_ALLOC_CHUNK)
				  * sizeof (struct fileinfo));
		      if (files.num_files == 0)
			make_cleanup (free_current_contents, &files.files);
		    }
		  files.files[files.num_files].name = cur_file;
		  files.files[files.num_files].dir =
		    read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		  line_ptr += bytes_read;
		  files.files[files.num_files].time =
		    read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		  line_ptr += bytes_read;
		  files.files[files.num_files].size =
		    read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
		  line_ptr += bytes_read;
		  files.num_files++;
		  break;
		default:
		  complain (&dwarf2_mangled_line_number_section);
		  goto done;
		}
	      break;
	    case DW_LNS_copy:
	      record_line (current_subfile, line, address);
	      basic_block = 0;
	      break;
	    case DW_LNS_advance_pc:
	      address += lh.minimum_instruction_length
		* read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
	      line_ptr += bytes_read;
	      break;
	    case DW_LNS_advance_line:
	      line += read_signed_leb128 (abfd, line_ptr, &bytes_read);
	      line_ptr += bytes_read;
	      break;
	    case DW_LNS_set_file:
	      /* The file and directory tables are 0 based, the references
	         are 1 based.  */
	      file = read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
	      line_ptr += bytes_read;
	      dwarf2_start_subfile
		(files.files[file - 1].name,
		 (files.files[file - 1].dir
		  ? dirs.dirs[files.files[file - 1].dir - 1] : comp_dir));
	      break;
	    case DW_LNS_set_column:
	      column = read_unsigned_leb128 (abfd, line_ptr, &bytes_read);
	      line_ptr += bytes_read;
	      break;
	    case DW_LNS_negate_stmt:
	      is_stmt = (!is_stmt);
	      break;
	    case DW_LNS_set_basic_block:
	      basic_block = 1;
	      break;
	      /* Add to the address register of the state machine the
	         address increment value corresponding to special opcode
	         255.  Ie, this value is scaled by the minimum instruction
	         length since special opcode 255 would have scaled the
	         the increment.  */
	    case DW_LNS_const_add_pc:
	      address += (lh.minimum_instruction_length
			  * ((255 - lh.opcode_base) / lh.line_range));
	      break;
	    case DW_LNS_fixed_advance_pc:
	      address += read_2_bytes (abfd, line_ptr);
	      line_ptr += 2;
	      break;
	    default:		/* special operand */
	      adj_opcode = op_code - lh.opcode_base;
	      address += (adj_opcode / lh.line_range)
		* lh.minimum_instruction_length;
	      line += lh.line_base + (adj_opcode % lh.line_range);
	      /* append row to matrix using current values */
	      record_line (current_subfile, line, address);
	      basic_block = 1;
	    }
	}
    }
done:
  do_cleanups (back_to);
}

/* Start a subfile for DWARF.  FILENAME is the name of the file and
   DIRNAME the name of the source directory which contains FILENAME
   or NULL if not known.
   This routine tries to keep line numbers from identical absolute and
   relative file names in a common subfile.

   Using the `list' example from the GDB testsuite, which resides in
   /srcdir and compiling it with Irix6.2 cc in /compdir using a filename
   of /srcdir/list0.c yields the following debugging information for list0.c:

   DW_AT_name:          /srcdir/list0.c
   DW_AT_comp_dir:              /compdir
   files.files[0].name: list0.h
   files.files[0].dir:  /srcdir
   files.files[1].name: list0.c
   files.files[1].dir:  /srcdir

   The line number information for list0.c has to end up in a single
   subfile, so that `break /srcdir/list0.c:1' works as expected.  */

static void
dwarf2_start_subfile (char *filename, char *dirname)
{
  /* If the filename isn't absolute, try to match an existing subfile
     with the full pathname.  */

  if (!IS_ABSOLUTE_PATH (filename) && dirname != NULL)
    {
      struct subfile *subfile;
      char *fullname = concat (dirname, "/", filename, NULL);

      for (subfile = subfiles; subfile; subfile = subfile->next)
	{
	  if (FILENAME_CMP (subfile->name, fullname) == 0)
	    {
	      current_subfile = subfile;
	      xfree (fullname);
	      return;
	    }
	}
      xfree (fullname);
    }
  start_subfile (filename, dirname);
}

/* Given a pointer to a DWARF information entry, figure out if we need
   to make a symbol table entry for it, and if so, create a new entry
   and return a pointer to it.
   If TYPE is NULL, determine symbol type from the die, otherwise
   used the passed type.  */

static struct symbol *
new_symbol (struct die_info *die, struct type *type, struct objfile *objfile,
	    const struct comp_unit_instance *cui)
{
  struct symbol *sym = NULL;
  char *name;
  struct attribute *attr = NULL;
  struct attribute *attr2 = NULL;
  CORE_ADDR addr;

  name = dwarf2_linkage_name (die, cui);
  if (name)
    {
      sym = (struct symbol *) obstack_alloc (&objfile->symbol_obstack,
					     sizeof (struct symbol));
      OBJSTAT (objfile, n_syms++);
      memset (sym, 0, sizeof (struct symbol));
      SYMBOL_NAME (sym) =	/* obsavestring (name, strlen (name),
				   &objfile->symbol_obstack); */ bcache (name, strlen (name) + 1,
						 &objfile->psymbol_cache);

      /* Default assumptions.
         Use the passed type or decode it from the die.  */
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      SYMBOL_CLASS (sym) = LOC_STATIC;
      if (type != NULL)
	SYMBOL_TYPE (sym) = type;
      else
	SYMBOL_TYPE (sym) = die_type (die, objfile, cui);
      attr = dwarf2_attr (die, DW_AT_decl_line, cui);
      if (attr)
	{
	  SYMBOL_LINE (sym) = DW_UNSND (attr);
	}

      /* If this symbol is from a C++ compilation, then attempt to
         cache the demangled form for future reference.  This is a
         typical time versus space tradeoff, that was decided in favor
         of time because it sped up C++ symbol lookups by a factor of
         about 20. */

      SYMBOL_LANGUAGE (sym) = cui->cu_language;
      SYMBOL_INIT_DEMANGLED_NAME (sym, &objfile->symbol_obstack);
      switch (die->tag)
	{
	case DW_TAG_label:
	  attr = dwarf2_attr (die, DW_AT_low_pc, cui);
	  if (attr)
	    {
	      SYMBOL_VALUE_ADDRESS (sym) = DW_ADDR (attr) + baseaddr;
	    }
	  SYMBOL_CLASS (sym) = LOC_LABEL;
	  break;
	case DW_TAG_inlined_subroutine:
	case DW_TAG_subprogram:
	  /* SYMBOL_BLOCK_VALUE (sym) will be filled in later by
	     finish_block.  */
	  SYMBOL_CLASS (sym) = LOC_BLOCK;
	  attr2 = dwarf2_attr (die, DW_AT_external, cui);
	  if (attr2 && (DW_UNSND (attr2) != 0))
	    add_symbol_to_list (sym, &global_symbols);
	  else
	    add_symbol_to_list (sym, list_in_scope);
	  break;
	case DW_TAG_variable:
	  /* Compilation with minimal debug info may result in variables
	     with missing type entries. Change the misleading `void' type
	     to something sensible.  */
	  if (TYPE_CODE (SYMBOL_TYPE (sym)) == TYPE_CODE_VOID)
	    SYMBOL_TYPE (sym) = init_type (TYPE_CODE_INT,
					   TARGET_INT_BIT / HOST_CHAR_BIT, 0,
					   "<variable, no debug info>",
					   objfile);
	  attr = dwarf2_attr (die, DW_AT_const_value, cui);
	  if (attr)
	    {
	      dwarf2_const_value (attr, sym, objfile, cui);
	      attr2 = dwarf2_attr (die, DW_AT_external, cui);
	      if (attr2 && (DW_UNSND (attr2) != 0))
		add_symbol_to_list (sym, &global_symbols);
	      else
		add_symbol_to_list (sym, list_in_scope);
	      break;
	    }
	  attr = dwarf2_attr (die, DW_AT_location, cui);
	  if (attr)
	    {
	      attr2 = dwarf2_attr (die, DW_AT_external, cui);
	      if (attr2 && (DW_UNSND (attr2) != 0))
		{
		  SYMBOL_VALUE_ADDRESS (sym) =
		    decode_locdesc (DW_BLOCK (attr), objfile, cui);
		  add_symbol_to_list (sym, &global_symbols);

		  /* In shared libraries the address of the variable
		     in the location descriptor might still be relocatable,
		     so its value could be zero.
		     Enter the symbol as a LOC_UNRESOLVED symbol, if its
		     value is zero, the address of the variable will then
		     be determined from the minimal symbol table whenever
		     the variable is referenced.  */
		  if (SYMBOL_VALUE_ADDRESS (sym))
		    {
		      fixup_symbol_section (sym, objfile);
		      SYMBOL_VALUE_ADDRESS (sym) +=
			ANOFFSET (objfile->section_offsets,
				  SYMBOL_SECTION (sym));
		      SYMBOL_CLASS (sym) = LOC_STATIC;
		    }
		  else
		    SYMBOL_CLASS (sym) = LOC_UNRESOLVED;
		}
	      else
		{
		  struct dwarf2_block *d2loc;
		  void *data;
#if 0
		  SYMBOL_LOC_EXPR (sym) =
		    obstack_alloc (&objfile->symbol_obstack, sizeof (struct dwarf2_block));
		  d2loc = SYMBOL_LOC_EXPR (sym);
		  memset (d2loc, 0, sizeof (struct dwarf2_block));
		  d2loc->size = DW_BLOCK (attr)->size;
		  d2loc->data = xmalloc (DW_BLOCK (attr)->size);
		  memset (d2loc->data, 0, DW_BLOCK (attr)->size);
		  memcpy (d2loc->data, DW_BLOCK (attr)->data,
			  DW_BLOCK (attr)->size);
		  data = d2loc->data;
		  d2loc->data =
		    bcache (data, DW_BLOCK (attr)->size,
			    &objfile->psymbol_cache);
		  xfree (data);
#endif
		  SYMBOL_VALUE (sym) = addr =
		    decode_locdesc (DW_BLOCK (attr), objfile, cui);
		  add_symbol_to_list (sym, list_in_scope);
		  if (optimized_out)
		    {
		      SYMBOL_CLASS (sym) = LOC_OPTIMIZED_OUT;
		    }
		  else if (isreg)
		    {
		      SYMBOL_CLASS (sym) = LOC_REGISTER;
		      SYMBOL_VALUE (sym) =
			DWARF2_REG_TO_REGNUM (SYMBOL_VALUE (sym));
		    }
		  else if (offreg)
		    {
		      SYMBOL_CLASS (sym) = LOC_BASEREG;
		      SYMBOL_BASEREG (sym) = DWARF2_REG_TO_REGNUM (basereg);
		    }
		  else if (islocal)
		    {
		      SYMBOL_CLASS (sym) = LOC_LOCAL;
		    }
		  else
		    {
		      fixup_symbol_section (sym, objfile);
		      SYMBOL_VALUE_ADDRESS (sym) =
			addr + ANOFFSET (objfile->section_offsets,
					 SYMBOL_SECTION (sym));
		      SYMBOL_CLASS (sym) = LOC_STATIC;
		    }
		}
	    }
	  else
	    {
	      /* We do not know the address of this symbol.
	         If it is an external symbol and we have type information
	         for it, enter the symbol as a LOC_UNRESOLVED symbol.
	         The address of the variable will then be determined from
	         the minimal symbol table whenever the variable is
	         referenced.  */
	      attr2 = dwarf2_attr (die, DW_AT_external, cui);
	      if (attr2 && (DW_UNSND (attr2) != 0)
		  && dwarf2_attr (die, DW_AT_type, cui) != NULL)
		{
		  SYMBOL_CLASS (sym) = LOC_UNRESOLVED;
		  add_symbol_to_list (sym, &global_symbols);
		}
	    }
	  break;
	case DW_TAG_formal_parameter:
	  attr = dwarf2_attr (die, DW_AT_location, cui);
	  if (attr)
	    {
	      struct dwarf2_block *d2loc;
	      void *data;
#if 0
	      SYMBOL_LOC_EXPR (sym) = obstack_alloc (&objfile->symbol_obstack, sizeof (struct dwarf2_block));
	      d2loc = SYMBOL_LOC_EXPR (sym);
	      memset (d2loc, 0, sizeof (struct dwarf2_block));
	      d2loc->size = DW_BLOCK (attr)->size;
	      d2loc->data = xmalloc (DW_BLOCK (attr)->size);
	      memset (d2loc->data, 0, DW_BLOCK (attr)->size);
	      memcpy (d2loc->data, DW_BLOCK (attr)->data,
		      DW_BLOCK (attr)->size);
	      data = d2loc->data;
	      d2loc->data =
		bcache (data, DW_BLOCK (attr)->size, &objfile->psymbol_cache);
	      xfree (data);
#endif
	      SYMBOL_VALUE (sym) =
		decode_locdesc (DW_BLOCK (attr), objfile, cui);
	      if (isreg)
		{
		  SYMBOL_CLASS (sym) = LOC_REGPARM;
		  SYMBOL_VALUE (sym) =
		    DWARF2_REG_TO_REGNUM (SYMBOL_VALUE (sym));
		}
	      else if (offreg)
		{
		  if (isderef)
		    {
		      if (basereg != frame_base_reg)
			complain (&dwarf2_complex_location_expr);
		      SYMBOL_CLASS (sym) = LOC_REF_ARG;
		    }
		  else
		    {
		      SYMBOL_CLASS (sym) = LOC_BASEREG_ARG;
		      SYMBOL_BASEREG (sym) = DWARF2_REG_TO_REGNUM (basereg);
		    }
		}
	      else
		{
		  SYMBOL_CLASS (sym) = LOC_ARG;
		}
	    }
	  attr = dwarf2_attr (die, DW_AT_const_value, cui);
	  if (attr)
	    {
	      dwarf2_const_value (attr, sym, objfile, cui);
	    }
	  add_symbol_to_list (sym, list_in_scope);
	  break;
	case DW_TAG_unspecified_parameters:
	  /* From varargs functions; gdb doesn't seem to have any
	     interest in this information, so just ignore it for now.
	     (FIXME?) */
	  break;
	case DW_TAG_class_type:
	case DW_TAG_structure_type:
	case DW_TAG_union_type:
	case DW_TAG_enumeration_type:
	  SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	  SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
	  /* For nested structures, the tag name is the correct symbol name, and, if we don't already have a type name, the symbol name is the correct type name. */
	  if (TYPE_TAG_NAME (type) != NULL)
	    {
	      if (TYPE_NAME (type) == NULL && SYMBOL_NAME (sym) != NULL)
		      TYPE_NAME (type) = SYMBOL_NAME (sym);
	      SYMBOL_NAME (sym) = TYPE_TAG_NAME (type);
	    }
	  add_symbol_to_list (sym, list_in_scope);
          
	  /* The semantics of C++ state that "struct foo { ... }" also
	     defines a typedef for "foo". Synthesize a typedef symbol so
	     that "ptype foo" works as expected.  */
	  if (cui->cu_language == language_cplus)
	    {
	      struct symbol *typedef_sym = (struct symbol *)
		obstack_alloc (&objfile->symbol_obstack,
			       sizeof (struct symbol));
	      *typedef_sym = *sym;
	      SYMBOL_NAMESPACE (typedef_sym) = VAR_NAMESPACE;
	      if (TYPE_NAME (SYMBOL_TYPE (sym)) == 0)
		TYPE_NAME (SYMBOL_TYPE (sym)) =
/*		  obsavestring (SYMBOL_NAME (sym),
				strlen (SYMBOL_NAME (sym)),
				&objfile->type_obstack);*/
		  bcache (SYMBOL_NAME (sym), strlen (SYMBOL_NAME (sym)) + 1,
			  &objfile->psymbol_cache);
	      add_symbol_to_list (typedef_sym, list_in_scope);
	    }
	  break;
	case DW_TAG_typedef:
	case DW_TAG_base_type:
	  SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
	  add_symbol_to_list (sym, list_in_scope);
	  break;
	case DW_TAG_enumerator:
	  attr = dwarf2_attr (die, DW_AT_const_value, cui);
	  if (attr)
	    {
	      dwarf2_const_value (attr, sym, objfile, cui);
	    }
	  add_symbol_to_list (sym, list_in_scope);
	  break;
	default:
	  /* Not a tag we recognize.  Hopefully we aren't processing
	     trash data, but since we must specifically ignore things
	     we don't recognize, there is nothing else we should do at
	     this point. */
	  complain (&dwarf2_unsupported_tag, dwarf2_tag_name (die->tag));
	  break;
	}
    }
  return (sym);
}

/* Copy constant value from an attribute to a symbol.  */

static void
dwarf2_const_value (struct attribute *attr, struct symbol *sym,
		    struct objfile *objfile,
		    const struct comp_unit_instance *cui)
{
  struct dwarf2_block *blk;

  switch (attr->form)
    {
    case DW_FORM_addr:
      if (TYPE_LENGTH (SYMBOL_TYPE (sym)) != cui->cu_header.addr_size)
	complain (&dwarf2_const_value_length_mismatch, SYMBOL_NAME (sym),
		  cui->cu_header.addr_size, TYPE_LENGTH (SYMBOL_TYPE (sym)));
      SYMBOL_VALUE_BYTES (sym) = (char *)
	obstack_alloc (&objfile->symbol_obstack, cui->cu_header.addr_size);
      store_address (SYMBOL_VALUE_BYTES (sym), cui->cu_header.addr_size,
		     DW_ADDR (attr));
      SYMBOL_CLASS (sym) = LOC_CONST_BYTES;
      break;
    case DW_FORM_block1:
    case DW_FORM_block2:
    case DW_FORM_block4:
    case DW_FORM_block:
      blk = DW_BLOCK (attr);
      if (TYPE_LENGTH (SYMBOL_TYPE (sym)) != blk->size)
	complain (&dwarf2_const_value_length_mismatch, SYMBOL_NAME (sym),
		  blk->size, TYPE_LENGTH (SYMBOL_TYPE (sym)));
      SYMBOL_VALUE_BYTES (sym) = (char *)
	obstack_alloc (&objfile->symbol_obstack, blk->size);
      memcpy (SYMBOL_VALUE_BYTES (sym), blk->data, blk->size);
      SYMBOL_CLASS (sym) = LOC_CONST_BYTES;
      break;

      /* The DW_AT_const_value attributes are supposed to carry the
         symbol's value "represented as it would be on the target
         architecture."  By the time we get here, it's already been
         converted to host endianness, so we just need to sign- or
         zero-extend it as appropriate.  */
    case DW_FORM_data1:
      dwarf2_const_value_data (attr, sym, 8);
      break;
    case DW_FORM_data2:
      dwarf2_const_value_data (attr, sym, 16);
      break;
    case DW_FORM_data4:
      dwarf2_const_value_data (attr, sym, 32);
      break;
    case DW_FORM_data8:
      dwarf2_const_value_data (attr, sym, 64);
      break;

    case DW_FORM_sdata:
      SYMBOL_VALUE (sym) = DW_SND (attr);
      SYMBOL_CLASS (sym) = LOC_CONST;
      break;

    case DW_FORM_udata:
      SYMBOL_VALUE (sym) = DW_UNSND (attr);
      SYMBOL_CLASS (sym) = LOC_CONST;
      break;

    default:
      complain (&dwarf2_unsupported_const_value_attr,
		dwarf2_form_name (attr->form));
      SYMBOL_VALUE (sym) = 0;
      SYMBOL_CLASS (sym) = LOC_CONST;
      break;
    }
}


/* Given an attr with a DW_FORM_dataN value in host byte order, sign-
   or zero-extend it as appropriate for the symbol's type.  */
static void
dwarf2_const_value_data (struct attribute *attr, struct symbol *sym, int bits)
{
  LONGEST l = DW_UNSND (attr);

  if (bits < sizeof (l) * 8)
    {
      if (TYPE_UNSIGNED (SYMBOL_TYPE (sym)))
	l &= ((LONGEST) 1 << bits) - 1;
      else
	l = (l << (sizeof (l) * 8 - bits)) >> (sizeof (l) * 8 - bits);
    }

  SYMBOL_VALUE (sym) = l;
  SYMBOL_CLASS (sym) = LOC_CONST;
}


/* Return the type of the die in question using its DW_AT_type attribute.  */

static struct type *
die_type (struct die_info *die, struct objfile *objfile,
	  const struct comp_unit_instance *cui)
{
  struct type *type;
  struct attribute *type_attr;
  struct die_info *type_die;
  unsigned int ref;

  type_attr = dwarf2_attr (die, DW_AT_type, cui);
  if (!type_attr)
    {
      /* A missing DW_AT_type represents a void type.  */
      return dwarf2_fundamental_type (objfile, FT_VOID, cui);
    }
  else
    {
      ref = dwarf2_get_ref_die_offset (cui, type_attr);
      type_die = follow_die_ref (ref, cui);
      if (!type_die)
	{
	  error ("Dwarf Error: Cannot find referent at offset %d.", ref);
	  return NULL;
	}
    }
  type = tag_type_to_type (type_die, objfile, cui);
  if (!type)
    {
      dump_die (type_die, cui);
      error
	("Dwarf Error: Problem turning type die at offset into gdb type.");
    }
  return type;
}

/* Return the containing type of the die in question using its
   DW_AT_containing_type attribute.  */

static struct type *
die_containing_type (struct die_info *die, struct objfile *objfile,
		     const struct comp_unit_instance *cui)
{
  struct type *type = NULL;
  struct attribute *type_attr;
  struct die_info *type_die = NULL;
  unsigned int ref;

  type_attr = dwarf2_attr (die, DW_AT_containing_type, cui);
  if (type_attr)
    {
      ref = dwarf2_get_ref_die_offset (cui, type_attr);
      type_die = follow_die_ref (ref, cui);
      if (!type_die)
	{
	  error ("Dwarf Error: Cannot find referent at offset %d.", ref);
	  return NULL;
	}
      type = tag_type_to_type (type_die, objfile, cui);
    }
  if (!type)
    {
      if (type_die)
	dump_die (type_die, cui);
      error ("Dwarf Error: Problem turning containing type into gdb type.");
    }
  return type;
}

static struct type *
tag_type_to_type (struct die_info *die, struct objfile *objfile,
		  const struct comp_unit_instance *cui)
{
  if (die->type)
    {
      return die->type;
    }
  else
    {
      read_type_die (die, objfile, cui);
      if (!die->type)
	{
	  dump_die (die, cui);
	  error ("Dwarf Error: Cannot find type of die.");
	}
      return die->type;
    }
}

static void
read_type_die (struct die_info *die, struct objfile *objfile,
	       const struct comp_unit_instance *cui)
{
  switch (die->tag)
    {
    case DW_TAG_class_type:
    case DW_TAG_structure_type:
    case DW_TAG_union_type:
      read_structure_scope (die, objfile, cui, NULL);
      break;
    case DW_TAG_enumeration_type:
      read_enumeration (die, objfile, cui);
      break;
    case DW_TAG_subprogram:
    case DW_TAG_subroutine_type:
      read_subroutine_type (die, objfile, cui);
      break;
    case DW_TAG_array_type:
      read_array_type (die, objfile, cui);
      break;
    case DW_TAG_pointer_type:
      read_tag_pointer_type (die, objfile, cui);
      break;
    case DW_TAG_ptr_to_member_type:
      read_tag_ptr_to_member_type (die, objfile, cui);
      break;
    case DW_TAG_reference_type:
      read_tag_reference_type (die, objfile, cui);
      break;
    case DW_TAG_const_type:
      read_tag_const_type (die, objfile, cui);
      break;
    case DW_TAG_volatile_type:
      read_tag_volatile_type (die, objfile, cui);
      break;
    case DW_TAG_string_type:
      read_tag_string_type (die, objfile, cui);
      break;
    case DW_TAG_typedef:
      read_typedef (die, objfile, cui);
      break;
    case DW_TAG_base_type:
      read_base_type (die, objfile, cui);
      break;
    default:
      complain (&dwarf2_unexpected_tag, dwarf2_tag_name (die->tag));
      break;
    }
}

static struct type *
dwarf2_base_type (int encoding, int size, struct objfile *objfile,
		 const struct comp_unit_instance *cui)
{
  /* FIXME - this should not produce a new (struct type *)
     every time.  It should cache base types.  */
  struct type *type;
  switch (encoding)
    {
    case DW_ATE_address:
      type = dwarf2_fundamental_type (objfile, FT_VOID, cui);
      return type;
    case DW_ATE_boolean:
      type = dwarf2_fundamental_type (objfile, FT_BOOLEAN, cui);
      return type;
    case DW_ATE_complex_float:
      if (size == 16)
	  type = dwarf2_fundamental_type (objfile, FT_DBL_PREC_COMPLEX, cui);
      else
	  type = dwarf2_fundamental_type (objfile, FT_COMPLEX, cui);
      return type;
    case DW_ATE_float:
      if (size == 8)
	  type = dwarf2_fundamental_type (objfile, FT_DBL_PREC_FLOAT, cui);
      else
	  type = dwarf2_fundamental_type (objfile, FT_FLOAT, cui);
      return type;
    case DW_ATE_signed:
      switch (size)
	{
	case 1:
	  type = dwarf2_fundamental_type (objfile, FT_SIGNED_CHAR, cui);
	  break;
	case 2:
	  type = dwarf2_fundamental_type (objfile, FT_SIGNED_SHORT, cui);
	  break;
	default:
	case 4:
	  type = dwarf2_fundamental_type (objfile, FT_SIGNED_INTEGER, cui);
	  break;
	}
      return type;
    case DW_ATE_signed_char:
      type = dwarf2_fundamental_type (objfile, FT_SIGNED_CHAR, cui);
      return type;
    case DW_ATE_unsigned:
      switch (size)
	{
	case 1:
	  type = dwarf2_fundamental_type (objfile, FT_UNSIGNED_CHAR, cui);
	  break;
	case 2:
	  type = dwarf2_fundamental_type (objfile, FT_UNSIGNED_SHORT, cui);
	  break;
	default:
	case 4:
	  type = dwarf2_fundamental_type (objfile, FT_UNSIGNED_INTEGER, cui);
	  break;
	}
      return type;
    case DW_ATE_unsigned_char:
      type = dwarf2_fundamental_type (objfile, FT_UNSIGNED_CHAR, cui);
      return type;
    default:
      type = dwarf2_fundamental_type (objfile, FT_SIGNED_INTEGER, cui);
      return type;
    }
}

/* Return sibling of die, NULL if no sibling.  */

struct die_info *
sibling_die (struct die_info *die)
{
  int nesting_level = 0;
  
  if (!die->has_children)
    {
      if (die->next && (die->next->tag == 0))
	return NULL;
      else
	return die->next;
    }
  else
    {
      do
	{
	  if (die->has_children)
	    nesting_level++;
	  if (die->tag == 0)
	    nesting_level--;
	  die = die->next;
	}
      while (nesting_level);
      if (die && (die->tag == 0))
	return NULL;
      else
	return die;
    }
}

/* Get linkage name of a die, return NULL if not found.  */

static char *
dwarf2_linkage_name (struct die_info *die,
		     const struct comp_unit_instance *cui)
{
  struct attribute *attr;

  attr = dwarf2_attr (die, DW_AT_MIPS_linkage_name, cui);
  if (attr && DW_STRING (attr))
    return DW_STRING (attr);
  attr = dwarf2_attr (die, DW_AT_name, cui);
  if (attr && DW_STRING (attr))
    return DW_STRING (attr);
  return NULL;
}

/* Convert a DIE tag into its string name.  */

static char *
dwarf2_tag_name (register unsigned tag)
{
  switch (tag)
    {
    case DW_TAG_padding:
      return "DW_TAG_padding";
    case DW_TAG_array_type:
      return "DW_TAG_array_type";
    case DW_TAG_class_type:
      return "DW_TAG_class_type";
    case DW_TAG_entry_point:
      return "DW_TAG_entry_point";
    case DW_TAG_enumeration_type:
      return "DW_TAG_enumeration_type";
    case DW_TAG_formal_parameter:
      return "DW_TAG_formal_parameter";
    case DW_TAG_imported_declaration:
      return "DW_TAG_imported_declaration";
    case DW_TAG_label:
      return "DW_TAG_label";
    case DW_TAG_lexical_block:
      return "DW_TAG_lexical_block";
    case DW_TAG_member:
      return "DW_TAG_member";
    case DW_TAG_pointer_type:
      return "DW_TAG_pointer_type";
    case DW_TAG_reference_type:
      return "DW_TAG_reference_type";
    case DW_TAG_compile_unit:
      return "DW_TAG_compile_unit";
    case DW_TAG_string_type:
      return "DW_TAG_string_type";
    case DW_TAG_structure_type:
      return "DW_TAG_structure_type";
    case DW_TAG_subroutine_type:
      return "DW_TAG_subroutine_type";
    case DW_TAG_typedef:
      return "DW_TAG_typedef";
    case DW_TAG_union_type:
      return "DW_TAG_union_type";
    case DW_TAG_unspecified_parameters:
      return "DW_TAG_unspecified_parameters";
    case DW_TAG_variant:
      return "DW_TAG_variant";
    case DW_TAG_common_block:
      return "DW_TAG_common_block";
    case DW_TAG_common_inclusion:
      return "DW_TAG_common_inclusion";
    case DW_TAG_inheritance:
      return "DW_TAG_inheritance";
    case DW_TAG_inlined_subroutine:
      return "DW_TAG_inlined_subroutine";
    case DW_TAG_module:
      return "DW_TAG_module";
    case DW_TAG_ptr_to_member_type:
      return "DW_TAG_ptr_to_member_type";
    case DW_TAG_set_type:
      return "DW_TAG_set_type";
    case DW_TAG_subrange_type:
      return "DW_TAG_subrange_type";
    case DW_TAG_with_stmt:
      return "DW_TAG_with_stmt";
    case DW_TAG_access_declaration:
      return "DW_TAG_access_declaration";
    case DW_TAG_base_type:
      return "DW_TAG_base_type";
    case DW_TAG_catch_block:
      return "DW_TAG_catch_block";
    case DW_TAG_const_type:
      return "DW_TAG_const_type";
    case DW_TAG_constant:
      return "DW_TAG_constant";
    case DW_TAG_enumerator:
      return "DW_TAG_enumerator";
    case DW_TAG_file_type:
      return "DW_TAG_file_type";
    case DW_TAG_friend:
      return "DW_TAG_friend";
    case DW_TAG_namelist:
      return "DW_TAG_namelist";
    case DW_TAG_namelist_item:
      return "DW_TAG_namelist_item";
    case DW_TAG_packed_type:
      return "DW_TAG_packed_type";
    case DW_TAG_subprogram:
      return "DW_TAG_subprogram";
    case DW_TAG_template_type_param:
      return "DW_TAG_template_type_param";
    case DW_TAG_template_value_param:
      return "DW_TAG_template_value_param";
    case DW_TAG_thrown_type:
      return "DW_TAG_thrown_type";
    case DW_TAG_try_block:
      return "DW_TAG_try_block";
    case DW_TAG_variant_part:
      return "DW_TAG_variant_part";
    case DW_TAG_variable:
      return "DW_TAG_variable";
    case DW_TAG_volatile_type:
      return "DW_TAG_volatile_type";
    case DW_TAG_MIPS_loop:
      return "DW_TAG_MIPS_loop";
    case DW_TAG_format_label:
      return "DW_TAG_format_label";
    case DW_TAG_function_template:
      return "DW_TAG_function_template";
    case DW_TAG_class_template:
      return "DW_TAG_class_template";
    default:
      return "DW_TAG_<unknown>";
    }
}

/* Convert a DWARF attribute code into its string name.  */

static char *
dwarf2_attr_name (register unsigned attr)
{
  switch (attr)
    {
    case DW_AT_sibling:
      return "DW_AT_sibling";
    case DW_AT_location:
      return "DW_AT_location";
    case DW_AT_name:
      return "DW_AT_name";
    case DW_AT_ordering:
      return "DW_AT_ordering";
    case DW_AT_subscr_data:
      return "DW_AT_subscr_data";
    case DW_AT_byte_size:
      return "DW_AT_byte_size";
    case DW_AT_bit_offset:
      return "DW_AT_bit_offset";
    case DW_AT_bit_size:
      return "DW_AT_bit_size";
    case DW_AT_element_list:
      return "DW_AT_element_list";
    case DW_AT_stmt_list:
      return "DW_AT_stmt_list";
    case DW_AT_low_pc:
      return "DW_AT_low_pc";
    case DW_AT_high_pc:
      return "DW_AT_high_pc";
    case DW_AT_language:
      return "DW_AT_language";
    case DW_AT_member:
      return "DW_AT_member";
    case DW_AT_discr:
      return "DW_AT_discr";
    case DW_AT_discr_value:
      return "DW_AT_discr_value";
    case DW_AT_visibility:
      return "DW_AT_visibility";
    case DW_AT_import:
      return "DW_AT_import";
    case DW_AT_string_length:
      return "DW_AT_string_length";
    case DW_AT_common_reference:
      return "DW_AT_common_reference";
    case DW_AT_comp_dir:
      return "DW_AT_comp_dir";
    case DW_AT_const_value:
      return "DW_AT_const_value";
    case DW_AT_containing_type:
      return "DW_AT_containing_type";
    case DW_AT_default_value:
      return "DW_AT_default_value";
    case DW_AT_inline:
      return "DW_AT_inline";
    case DW_AT_is_optional:
      return "DW_AT_is_optional";
    case DW_AT_lower_bound:
      return "DW_AT_lower_bound";
    case DW_AT_producer:
      return "DW_AT_producer";
    case DW_AT_prototyped:
      return "DW_AT_prototyped";
    case DW_AT_return_addr:
      return "DW_AT_return_addr";
    case DW_AT_start_scope:
      return "DW_AT_start_scope";
    case DW_AT_stride_size:
      return "DW_AT_stride_size";
    case DW_AT_upper_bound:
      return "DW_AT_upper_bound";
    case DW_AT_abstract_origin:
      return "DW_AT_abstract_origin";
    case DW_AT_accessibility:
      return "DW_AT_accessibility";
    case DW_AT_address_class:
      return "DW_AT_address_class";
    case DW_AT_artificial:
      return "DW_AT_artificial";
    case DW_AT_base_types:
      return "DW_AT_base_types";
    case DW_AT_calling_convention:
      return "DW_AT_calling_convention";
    case DW_AT_count:
      return "DW_AT_count";
    case DW_AT_data_member_location:
      return "DW_AT_data_member_location";
    case DW_AT_decl_column:
      return "DW_AT_decl_column";
    case DW_AT_decl_file:
      return "DW_AT_decl_file";
    case DW_AT_decl_line:
      return "DW_AT_decl_line";
    case DW_AT_declaration:
      return "DW_AT_declaration";
    case DW_AT_discr_list:
      return "DW_AT_discr_list";
    case DW_AT_encoding:
      return "DW_AT_encoding";
    case DW_AT_external:
      return "DW_AT_external";
    case DW_AT_frame_base:
      return "DW_AT_frame_base";
    case DW_AT_friend:
      return "DW_AT_friend";
    case DW_AT_identifier_case:
      return "DW_AT_identifier_case";
    case DW_AT_macro_info:
      return "DW_AT_macro_info";
    case DW_AT_namelist_items:
      return "DW_AT_namelist_items";
    case DW_AT_priority:
      return "DW_AT_priority";
    case DW_AT_segment:
      return "DW_AT_segment";
    case DW_AT_specification:
      return "DW_AT_specification";
    case DW_AT_static_link:
      return "DW_AT_static_link";
    case DW_AT_type:
      return "DW_AT_type";
    case DW_AT_use_location:
      return "DW_AT_use_location";
    case DW_AT_variable_parameter:
      return "DW_AT_variable_parameter";
    case DW_AT_virtuality:
      return "DW_AT_virtuality";
    case DW_AT_vtable_elem_location:
      return "DW_AT_vtable_elem_location";

#ifdef MIPS
    case DW_AT_MIPS_fde:
      return "DW_AT_MIPS_fde";
    case DW_AT_MIPS_loop_begin:
      return "DW_AT_MIPS_loop_begin";
    case DW_AT_MIPS_tail_loop_begin:
      return "DW_AT_MIPS_tail_loop_begin";
    case DW_AT_MIPS_epilog_begin:
      return "DW_AT_MIPS_epilog_begin";
    case DW_AT_MIPS_loop_unroll_factor:
      return "DW_AT_MIPS_loop_unroll_factor";
    case DW_AT_MIPS_software_pipeline_depth:
      return "DW_AT_MIPS_software_pipeline_depth";
    case DW_AT_MIPS_linkage_name:
      return "DW_AT_MIPS_linkage_name";
#endif

    case DW_AT_sf_names:
      return "DW_AT_sf_names";
    case DW_AT_src_info:
      return "DW_AT_src_info";
    case DW_AT_mac_info:
      return "DW_AT_mac_info";
    case DW_AT_src_coords:
      return "DW_AT_src_coords";
    case DW_AT_body_begin:
      return "DW_AT_body_begin";
    case DW_AT_body_end:
      return "DW_AT_body_end";
    default:
      return "DW_AT_<unknown>";
    }
}

/* Convert a DWARF value form code into its string name.  */

static char *
dwarf2_form_name (register unsigned form)
{
  switch (form)
    {
    case DW_FORM_addr:
      return "DW_FORM_addr";
    case DW_FORM_block2:
      return "DW_FORM_block2";
    case DW_FORM_block4:
      return "DW_FORM_block4";
    case DW_FORM_data2:
      return "DW_FORM_data2";
    case DW_FORM_data4:
      return "DW_FORM_data4";
    case DW_FORM_data8:
      return "DW_FORM_data8";
    case DW_FORM_string:
      return "DW_FORM_string";
    case DW_FORM_block:
      return "DW_FORM_block";
    case DW_FORM_block1:
      return "DW_FORM_block1";
    case DW_FORM_data1:
      return "DW_FORM_data1";
    case DW_FORM_flag:
      return "DW_FORM_flag";
    case DW_FORM_sdata:
      return "DW_FORM_sdata";
    case DW_FORM_strp:
      return "DW_FORM_strp";
    case DW_FORM_udata:
      return "DW_FORM_udata";
    case DW_FORM_ref_addr:
      return "DW_FORM_ref_addr";
    case DW_FORM_ref1:
      return "DW_FORM_ref1";
    case DW_FORM_ref2:
      return "DW_FORM_ref2";
    case DW_FORM_ref4:
      return "DW_FORM_ref4";
    case DW_FORM_ref8:
      return "DW_FORM_ref8";
    case DW_FORM_ref_udata:
      return "DW_FORM_ref_udata";
    case DW_FORM_indirect:
      return "DW_FORM_indirect";
    default:
      return "DW_FORM_<unknown>";
    }
}

/* Convert a DWARF stack opcode into its string name.  */

static char *
dwarf2_stack_op_name (register unsigned op)
{
  switch (op)
    {
    case DW_OP_addr:
      return "DW_OP_addr";
    case DW_OP_deref:
      return "DW_OP_deref";
    case DW_OP_const1u:
      return "DW_OP_const1u";
    case DW_OP_const1s:
      return "DW_OP_const1s";
    case DW_OP_const2u:
      return "DW_OP_const2u";
    case DW_OP_const2s:
      return "DW_OP_const2s";
    case DW_OP_const4u:
      return "DW_OP_const4u";
    case DW_OP_const4s:
      return "DW_OP_const4s";
    case DW_OP_const8u:
      return "DW_OP_const8u";
    case DW_OP_const8s:
      return "DW_OP_const8s";
    case DW_OP_constu:
      return "DW_OP_constu";
    case DW_OP_consts:
      return "DW_OP_consts";
    case DW_OP_dup:
      return "DW_OP_dup";
    case DW_OP_drop:
      return "DW_OP_drop";
    case DW_OP_over:
      return "DW_OP_over";
    case DW_OP_pick:
      return "DW_OP_pick";
    case DW_OP_swap:
      return "DW_OP_swap";
    case DW_OP_rot:
      return "DW_OP_rot";
    case DW_OP_xderef:
      return "DW_OP_xderef";
    case DW_OP_abs:
      return "DW_OP_abs";
    case DW_OP_and:
      return "DW_OP_and";
    case DW_OP_div:
      return "DW_OP_div";
    case DW_OP_minus:
      return "DW_OP_minus";
    case DW_OP_mod:
      return "DW_OP_mod";
    case DW_OP_mul:
      return "DW_OP_mul";
    case DW_OP_neg:
      return "DW_OP_neg";
    case DW_OP_not:
      return "DW_OP_not";
    case DW_OP_or:
      return "DW_OP_or";
    case DW_OP_plus:
      return "DW_OP_plus";
    case DW_OP_plus_uconst:
      return "DW_OP_plus_uconst";
    case DW_OP_shl:
      return "DW_OP_shl";
    case DW_OP_shr:
      return "DW_OP_shr";
    case DW_OP_shra:
      return "DW_OP_shra";
    case DW_OP_xor:
      return "DW_OP_xor";
    case DW_OP_bra:
      return "DW_OP_bra";
    case DW_OP_eq:
      return "DW_OP_eq";
    case DW_OP_ge:
      return "DW_OP_ge";
    case DW_OP_gt:
      return "DW_OP_gt";
    case DW_OP_le:
      return "DW_OP_le";
    case DW_OP_lt:
      return "DW_OP_lt";
    case DW_OP_ne:
      return "DW_OP_ne";
    case DW_OP_skip:
      return "DW_OP_skip";
    case DW_OP_lit0:
      return "DW_OP_lit0";
    case DW_OP_lit1:
      return "DW_OP_lit1";
    case DW_OP_lit2:
      return "DW_OP_lit2";
    case DW_OP_lit3:
      return "DW_OP_lit3";
    case DW_OP_lit4:
      return "DW_OP_lit4";
    case DW_OP_lit5:
      return "DW_OP_lit5";
    case DW_OP_lit6:
      return "DW_OP_lit6";
    case DW_OP_lit7:
      return "DW_OP_lit7";
    case DW_OP_lit8:
      return "DW_OP_lit8";
    case DW_OP_lit9:
      return "DW_OP_lit9";
    case DW_OP_lit10:
      return "DW_OP_lit10";
    case DW_OP_lit11:
      return "DW_OP_lit11";
    case DW_OP_lit12:
      return "DW_OP_lit12";
    case DW_OP_lit13:
      return "DW_OP_lit13";
    case DW_OP_lit14:
      return "DW_OP_lit14";
    case DW_OP_lit15:
      return "DW_OP_lit15";
    case DW_OP_lit16:
      return "DW_OP_lit16";
    case DW_OP_lit17:
      return "DW_OP_lit17";
    case DW_OP_lit18:
      return "DW_OP_lit18";
    case DW_OP_lit19:
      return "DW_OP_lit19";
    case DW_OP_lit20:
      return "DW_OP_lit20";
    case DW_OP_lit21:
      return "DW_OP_lit21";
    case DW_OP_lit22:
      return "DW_OP_lit22";
    case DW_OP_lit23:
      return "DW_OP_lit23";
    case DW_OP_lit24:
      return "DW_OP_lit24";
    case DW_OP_lit25:
      return "DW_OP_lit25";
    case DW_OP_lit26:
      return "DW_OP_lit26";
    case DW_OP_lit27:
      return "DW_OP_lit27";
    case DW_OP_lit28:
      return "DW_OP_lit28";
    case DW_OP_lit29:
      return "DW_OP_lit29";
    case DW_OP_lit30:
      return "DW_OP_lit30";
    case DW_OP_lit31:
      return "DW_OP_lit31";
    case DW_OP_reg0:
      return "DW_OP_reg0";
    case DW_OP_reg1:
      return "DW_OP_reg1";
    case DW_OP_reg2:
      return "DW_OP_reg2";
    case DW_OP_reg3:
      return "DW_OP_reg3";
    case DW_OP_reg4:
      return "DW_OP_reg4";
    case DW_OP_reg5:
      return "DW_OP_reg5";
    case DW_OP_reg6:
      return "DW_OP_reg6";
    case DW_OP_reg7:
      return "DW_OP_reg7";
    case DW_OP_reg8:
      return "DW_OP_reg8";
    case DW_OP_reg9:
      return "DW_OP_reg9";
    case DW_OP_reg10:
      return "DW_OP_reg10";
    case DW_OP_reg11:
      return "DW_OP_reg11";
    case DW_OP_reg12:
      return "DW_OP_reg12";
    case DW_OP_reg13:
      return "DW_OP_reg13";
    case DW_OP_reg14:
      return "DW_OP_reg14";
    case DW_OP_reg15:
      return "DW_OP_reg15";
    case DW_OP_reg16:
      return "DW_OP_reg16";
    case DW_OP_reg17:
      return "DW_OP_reg17";
    case DW_OP_reg18:
      return "DW_OP_reg18";
    case DW_OP_reg19:
      return "DW_OP_reg19";
    case DW_OP_reg20:
      return "DW_OP_reg20";
    case DW_OP_reg21:
      return "DW_OP_reg21";
    case DW_OP_reg22:
      return "DW_OP_reg22";
    case DW_OP_reg23:
      return "DW_OP_reg23";
    case DW_OP_reg24:
      return "DW_OP_reg24";
    case DW_OP_reg25:
      return "DW_OP_reg25";
    case DW_OP_reg26:
      return "DW_OP_reg26";
    case DW_OP_reg27:
      return "DW_OP_reg27";
    case DW_OP_reg28:
      return "DW_OP_reg28";
    case DW_OP_reg29:
      return "DW_OP_reg29";
    case DW_OP_reg30:
      return "DW_OP_reg30";
    case DW_OP_reg31:
      return "DW_OP_reg31";
    case DW_OP_breg0:
      return "DW_OP_breg0";
    case DW_OP_breg1:
      return "DW_OP_breg1";
    case DW_OP_breg2:
      return "DW_OP_breg2";
    case DW_OP_breg3:
      return "DW_OP_breg3";
    case DW_OP_breg4:
      return "DW_OP_breg4";
    case DW_OP_breg5:
      return "DW_OP_breg5";
    case DW_OP_breg6:
      return "DW_OP_breg6";
    case DW_OP_breg7:
      return "DW_OP_breg7";
    case DW_OP_breg8:
      return "DW_OP_breg8";
    case DW_OP_breg9:
      return "DW_OP_breg9";
    case DW_OP_breg10:
      return "DW_OP_breg10";
    case DW_OP_breg11:
      return "DW_OP_breg11";
    case DW_OP_breg12:
      return "DW_OP_breg12";
    case DW_OP_breg13:
      return "DW_OP_breg13";
    case DW_OP_breg14:
      return "DW_OP_breg14";
    case DW_OP_breg15:
      return "DW_OP_breg15";
    case DW_OP_breg16:
      return "DW_OP_breg16";
    case DW_OP_breg17:
      return "DW_OP_breg17";
    case DW_OP_breg18:
      return "DW_OP_breg18";
    case DW_OP_breg19:
      return "DW_OP_breg19";
    case DW_OP_breg20:
      return "DW_OP_breg20";
    case DW_OP_breg21:
      return "DW_OP_breg21";
    case DW_OP_breg22:
      return "DW_OP_breg22";
    case DW_OP_breg23:
      return "DW_OP_breg23";
    case DW_OP_breg24:
      return "DW_OP_breg24";
    case DW_OP_breg25:
      return "DW_OP_breg25";
    case DW_OP_breg26:
      return "DW_OP_breg26";
    case DW_OP_breg27:
      return "DW_OP_breg27";
    case DW_OP_breg28:
      return "DW_OP_breg28";
    case DW_OP_breg29:
      return "DW_OP_breg29";
    case DW_OP_breg30:
      return "DW_OP_breg30";
    case DW_OP_breg31:
      return "DW_OP_breg31";
    case DW_OP_regx:
      return "DW_OP_regx";
    case DW_OP_fbreg:
      return "DW_OP_fbreg";
    case DW_OP_bregx:
      return "DW_OP_bregx";
    case DW_OP_piece:
      return "DW_OP_piece";
    case DW_OP_deref_size:
      return "DW_OP_deref_size";
    case DW_OP_xderef_size:
      return "DW_OP_xderef_size";
    case DW_OP_nop:
      return "DW_OP_nop";
    default:
      return "OP_<unknown>";
    }
}

static char *
dwarf2_bool_name (unsigned mybool)
{
  if (mybool)
    return "TRUE";
  else
    return "FALSE";
}

/* Convert a DWARF type code into its string name.  */

static char *
dwarf2_type_encoding_name (register unsigned enc)
{
  switch (enc)
    {
    case DW_ATE_address:
      return "DW_ATE_address";
    case DW_ATE_boolean:
      return "DW_ATE_boolean";
    case DW_ATE_complex_float:
      return "DW_ATE_complex_float";
    case DW_ATE_float:
      return "DW_ATE_float";
    case DW_ATE_signed:
      return "DW_ATE_signed";
    case DW_ATE_signed_char:
      return "DW_ATE_signed_char";
    case DW_ATE_unsigned:
      return "DW_ATE_unsigned";
    case DW_ATE_unsigned_char:
      return "DW_ATE_unsigned_char";
    default:
      return "DW_ATE_<unknown>";
    }
}

void
dump_die (struct die_info *die, const struct comp_unit_instance *cui)
{
  unsigned int i;

  fprintf (stderr, "Die: %s (abbrev = %d, offset = %d)\n",
	   dwarf2_tag_name (die->tag), die->abbrev, die->offset);
  fprintf (stderr, "\thas children: %s\n",
	   dwarf2_bool_name (die->has_children));

  fprintf (stderr, "\tattributes:\n");
  for (i = 0; i < die->num_attrs; ++i)
    {
      fprintf (stderr, "\t\t%s (%s) ",
	       dwarf2_attr_name (die->attrs[i].name),
	       dwarf2_form_name (die->attrs[i].form));
      switch (die->attrs[i].form)
	{
	case DW_FORM_ref_addr:
	case DW_FORM_addr:
	  fprintf (stderr, "address: ");
	  print_address_numeric (DW_ADDR (&die->attrs[i]), 1, gdb_stderr);
	  break;
	case DW_FORM_block2:
	case DW_FORM_block4:
	case DW_FORM_block:
	case DW_FORM_block1:
	  fprintf (stderr, "block: size %d", DW_BLOCK (&die->attrs[i])->size);
	  break;
	case DW_FORM_data1:
	case DW_FORM_data2:
	case DW_FORM_data4:
	case DW_FORM_data8:
	case DW_FORM_ref1:
	case DW_FORM_ref2:
	case DW_FORM_ref4:
	case DW_FORM_udata:
	case DW_FORM_sdata:
	  fprintf (stderr, "constant: %ld", DW_UNSND (&die->attrs[i]));
	  break;
	case DW_FORM_string:
	  fprintf (stderr, "string: \"%s\"",
		   DW_STRING (&die->attrs[i])
		   ? DW_STRING (&die->attrs[i]) : "");
	  break;
	case DW_FORM_flag:
	  if (DW_UNSND (&die->attrs[i]))
	    fprintf (stderr, "flag: TRUE");
	  else
	    fprintf (stderr, "flag: FALSE");
	  break;
	case DW_FORM_strp:	/* we do not support separate string
				   section yet */
	case DW_FORM_indirect:	/* we do not handle indirect yet */
	default:
	  fprintf (stderr, "unsupported attribute form: %d.",
		   die->attrs[i].form);
	}
      fprintf (stderr, "\n");
    }
}

void
store_in_ref_table (unsigned int offset, struct die_info *die,
		    struct comp_unit_instance *cui)
{
  int h;
  struct die_info *old;

  h = (offset % REF_HASH_SIZE);
  old = cui->die_ref_table[h];
  die->next_ref = old;
  cui->die_ref_table[h] = die;
}


static void
dwarf2_empty_hash_tables (struct comp_unit_instance *cui)
{
  memset (cui->die_ref_table, 0, sizeof (cui->die_ref_table));
}

static unsigned int
dwarf2_get_ref_die_offset (const struct comp_unit_instance *cui,
			   struct attribute *attr)
{
  unsigned int result = 0;
  struct comp_unit_instance *cui_for_die;

  switch (attr->form)
    {
    case DW_FORM_ref_addr:
      result = DW_ADDR (attr);
      break;
    case DW_FORM_ref1:
    case DW_FORM_ref2:
    case DW_FORM_ref4:
    case DW_FORM_ref8:
    case DW_FORM_ref_udata:
      result = cui->cu_header_offset + DW_UNSND (attr);
      break;
    default:
      complain (&dwarf2_unsupported_die_ref_attr,
		dwarf2_form_name (attr->form));
    }
  return result;
}

struct die_info *
follow_die_ref (unsigned int offset, const struct comp_unit_instance *cui)
{
  struct die_info *die = NULL;
  int h;
  struct comp_unit_instance *cui_for_die;
  cui_for_die = dwarf2_get_cu_by_offset (cui->abfd, offset);
  if (cui_for_die == 0)
    error ("Can't find CU for die");
  if (cui_for_die != cui)
    {
      read_full_die (&die, cui_for_die->abfd, dwarf2_info_buffer + offset,
		     cui_for_die);
      return die;
    }
  h = (offset % REF_HASH_SIZE);
  die = cui->die_ref_table[h];
  while (die)
    {
      if (die->offset == offset)
	{
	  return die;
	}
      die = die->next_ref;
    }
  die = NULL;
  read_full_die (&die, cui->abfd, dwarf2_info_buffer + offset, cui);
  return die;
}

static struct type *
dwarf2_fundamental_type (struct objfile *objfile, int typeid,
			 const struct comp_unit_instance *cui)
{
  if (typeid < 0 || typeid >= FT_NUM_MEMBERS)
    {
      error ("Dwarf Error: internal error - invalid fundamental type id %d.",
	     typeid);
    }

  /* Look for this particular type in the fundamental type vector.  If
     one is not found, create and install one appropriate for the
     current language and the current target machine. */

  if (ftypes[typeid] == NULL)
    {
      ftypes[typeid] = cui->cu_language_defn->la_fund_type (objfile, typeid);
    }

  return (ftypes[typeid]);
}

/* Decode simple location descriptions.
   Given a pointer to a dwarf block that defines a location, compute
   the location and return the value.

   FIXME: This is a kludge until we figure out a better
   way to handle the location descriptions.
   Gdb's design does not mesh well with the DWARF2 notion of a location
   computing interpreter, which is a shame because the flexibility goes unused.
   FIXME: Implement more operations as necessary.

   A location description containing no operations indicates that the
   object is optimized out. The global optimized_out flag is set for
   those, the return value is meaningless.

   When the result is a register number, the global isreg flag is set,
   otherwise it is cleared.

   When the result is a base register offset, the global offreg flag is set
   and the register number is returned in basereg, otherwise it is cleared.

   When the DW_OP_fbreg operation is encountered without a corresponding
   DW_AT_frame_base attribute, the global islocal flag is set.
   Hopefully the machine dependent code knows how to set up a virtual
   frame pointer for the local references.

   Note that stack[0] is unused except as a default error return.
   Note that stack overflow is not yet handled.  */

static CORE_ADDR
decode_locdesc (struct dwarf2_block *blk, struct objfile *objfile,
		const struct comp_unit_instance *cui)
{
  int i;
  int size = blk->size;
  char *data = blk->data;
  CORE_ADDR stack[64];
  int stacki;
  unsigned int bytes_read, unsnd;
  unsigned char op;

  i = 0;
  stacki = 0;
  stack[stacki] = 0;
  isreg = 0;
  offreg = 0;
  isderef = 0;
  islocal = 0;
  optimized_out = 1;

  while (i < size)
    {
      optimized_out = 0;
      op = data[i++];
      switch (op)
	{
	case DW_OP_reg0:
	case DW_OP_reg1:
	case DW_OP_reg2:
	case DW_OP_reg3:
	case DW_OP_reg4:
	case DW_OP_reg5:
	case DW_OP_reg6:
	case DW_OP_reg7:
	case DW_OP_reg8:
	case DW_OP_reg9:
	case DW_OP_reg10:
	case DW_OP_reg11:
	case DW_OP_reg12:
	case DW_OP_reg13:
	case DW_OP_reg14:
	case DW_OP_reg15:
	case DW_OP_reg16:
	case DW_OP_reg17:
	case DW_OP_reg18:
	case DW_OP_reg19:
	case DW_OP_reg20:
	case DW_OP_reg21:
	case DW_OP_reg22:
	case DW_OP_reg23:
	case DW_OP_reg24:
	case DW_OP_reg25:
	case DW_OP_reg26:
	case DW_OP_reg27:
	case DW_OP_reg28:
	case DW_OP_reg29:
	case DW_OP_reg30:
	case DW_OP_reg31:
	  isreg = 1;
	  stack[++stacki] = op - DW_OP_reg0;
	  break;

	case DW_OP_regx:
	  isreg = 1;
	  unsnd = read_unsigned_leb128 (NULL, (data + i), &bytes_read);
	  i += bytes_read;
#if defined(HARRIS_TARGET) && defined(_M88K)
	  /* The Harris 88110 gdb ports have long kept their special reg
	     numbers between their gp-regs and their x-regs.  This is
	     not how our dwarf is generated.  Punt. */
	  unsnd += 6;
#endif
	  stack[++stacki] = unsnd;
	  break;

	case DW_OP_breg0:
	case DW_OP_breg1:
	case DW_OP_breg2:
	case DW_OP_breg3:
	case DW_OP_breg4:
	case DW_OP_breg5:
	case DW_OP_breg6:
	case DW_OP_breg7:
	case DW_OP_breg8:
	case DW_OP_breg9:
	case DW_OP_breg10:
	case DW_OP_breg11:
	case DW_OP_breg12:
	case DW_OP_breg13:
	case DW_OP_breg14:
	case DW_OP_breg15:
	case DW_OP_breg16:
	case DW_OP_breg17:
	case DW_OP_breg18:
	case DW_OP_breg19:
	case DW_OP_breg20:
	case DW_OP_breg21:
	case DW_OP_breg22:
	case DW_OP_breg23:
	case DW_OP_breg24:
	case DW_OP_breg25:
	case DW_OP_breg26:
	case DW_OP_breg27:
	case DW_OP_breg28:
	case DW_OP_breg29:
	case DW_OP_breg30:
	case DW_OP_breg31:
	  offreg = 1;
	  basereg = op - DW_OP_breg0;
	  stack[++stacki] =
	    read_signed_leb128 (NULL, (data + i), &bytes_read);
	  i += bytes_read;
	  break;

	case DW_OP_bregx:
	  offreg = 1;
	  basereg = read_unsigned_leb128 (NULL, (data + i), &bytes_read);
	  i += bytes_read;
	  stack[++stacki] =
	    read_signed_leb128 (NULL, (data + i), &bytes_read);
	  i += bytes_read;
	  break;

	case DW_OP_fbreg:
	  stack[++stacki] =
	    read_signed_leb128 (NULL, (data + i), &bytes_read);
	  i += bytes_read;
	  if (frame_base_reg >= 0)
	    {
	      offreg = 1;
	      basereg = frame_base_reg;
	      stack[stacki] += frame_base_offset;
	    }
	  else
	    {
	      complain (&dwarf2_missing_at_frame_base);
	      islocal = 1;
	    }
	  break;

	case DW_OP_addr:
	  stack[++stacki] = read_address (objfile->obfd, &data[i],
					  cui, &bytes_read);
	  i += bytes_read;
	  break;

	case DW_OP_const1u:
	  stack[++stacki] = read_1_byte (objfile->obfd, &data[i]);
	  i += 1;
	  break;

	case DW_OP_const1s:
	  stack[++stacki] = read_1_signed_byte (objfile->obfd, &data[i]);
	  i += 1;
	  break;

	case DW_OP_const2u:
	  stack[++stacki] = read_2_bytes (objfile->obfd, &data[i]);
	  i += 2;
	  break;

	case DW_OP_const2s:
	  stack[++stacki] = read_2_signed_bytes (objfile->obfd, &data[i]);
	  i += 2;
	  break;

	case DW_OP_const4u:
	  stack[++stacki] = read_4_bytes (objfile->obfd, &data[i]);
	  i += 4;
	  break;

	case DW_OP_const4s:
	  stack[++stacki] = read_4_signed_bytes (objfile->obfd, &data[i]);
	  i += 4;
	  break;

	case DW_OP_constu:
	  stack[++stacki] = read_unsigned_leb128 (NULL, (data + i),
						  &bytes_read);
	  i += bytes_read;
	  break;

	case DW_OP_consts:
	  stack[++stacki] =
	    read_signed_leb128 (NULL, (data + i), &bytes_read);
	  i += bytes_read;
	  break;

	case DW_OP_plus:
	  stack[stacki - 1] += stack[stacki];
	  stacki--;
	  break;

	case DW_OP_plus_uconst:
	  stack[stacki] +=
	    read_unsigned_leb128 (NULL, (data + i), &bytes_read);
	  i += bytes_read;
	  break;

	case DW_OP_minus:
	  stack[stacki - 1] = stack[stacki] - stack[stacki - 1];
	  stacki--;
	  break;

	case DW_OP_deref:
	  isderef = 1;
	  /* If we're not the last op, then we definitely can't encode
	     this using GDB's address_class enum.  */
	  if (i < size)
	    complain (&dwarf2_complex_location_expr);
	  break;

	default:
	  complain (&dwarf2_unsupported_stack_op, dwarf2_stack_op_name (op));
	  return (stack[stacki]);
	}
    }
  return (stack[stacki]);
}

/* memory allocation interface */

/* ARGSUSED */
static void
dwarf2_free_tmp_obstack (PTR ignore)
{
  obstack_free (&dwarf2_tmp_obstack, NULL);
}

static struct dwarf2_block *
dwarf2_alloc_block (void)
{
  struct dwarf2_block *blk;

  blk = (struct dwarf2_block *)
    obstack_alloc (&dwarf2_tmp_obstack, sizeof (struct dwarf2_block));
  return (blk);
}

static struct abbrev_info *
dwarf2_alloc_abbrev (void)
{
  struct abbrev_info *abbrev;

  abbrev = (struct abbrev_info *) xmalloc (sizeof (struct abbrev_info));
  memset (abbrev, 0, sizeof (struct abbrev_info));
  return (abbrev);
}

static struct comp_unit_instance *
dwarf2_alloc_comp_unit_instance (void)
{
  struct comp_unit_instance *cui;

  cui =
    (struct comp_unit_instance *)
    xmalloc (sizeof (struct comp_unit_instance));
  memset (cui, 0, sizeof (struct comp_unit_instance));
  return cui;
}
static struct die_info *
dwarf2_alloc_die (void)
{
  struct die_info *die;

  die = (struct die_info *) xmalloc (sizeof (struct die_info));
  memset (die, 0, sizeof (struct die_info));
  return (die);
}
typedef struct Frame_Chunk
{
  struct Frame_Chunk *next;
  unsigned int fde_offset;
  unsigned char *chunk_start;
  int ncols;
  /* DW_CFA_{undefined,same_value,offset,register,unreferenced}  */
  short int *col_type;
  int *col_offset;
  char *augmentation;
  unsigned int code_factor;
  signed int data_factor;
  unsigned long pc_begin;
  unsigned long pc_range;
  int cfa_reg;
  int cfa_offset;
  int ra;
}
Frame_Chunk;

/* A marker for a col_type that means this column was never referenced
   in the frame info.  */
#define DW_CFA_unreferenced (-1)

static void
frame_need_space (fc, reg)
     Frame_Chunk *fc;
     int reg;
{
  int prev = fc->ncols;

  if (reg < fc->ncols)
    return;

  fc->ncols = reg + 1;
  fc->col_type = (short int *) xrealloc (fc->col_type,
					 fc->ncols * sizeof (short int));
  fc->col_offset = (int *) xrealloc (fc->col_offset,
				     fc->ncols * sizeof (int));

  while (prev < fc->ncols)
    {
      fc->col_type[prev] = DW_CFA_unreferenced;
      fc->col_offset[prev] = 0;
      prev++;
    }
}

static void
frame_display_row (fc, need_col_headers, max_regs)
     Frame_Chunk *fc;
     int *need_col_headers;
     int *max_regs;
{
  int r;
  char tmp[100];

  if (*max_regs < fc->ncols)
    *max_regs = fc->ncols;

  if (*need_col_headers)
    {
      *need_col_headers = 0;

      printf ("   LOC   CFA      ");

      for (r = 0; r < *max_regs; r++)
	if (fc->col_type[r] != DW_CFA_unreferenced)
	  {
	    if (r == fc->ra)
	      printf ("ra   ");
	    else
	      printf ("r%-4d", r);
	  }

      printf ("\n");
    }

  printf ("%08x ", (unsigned int) fc->pc_begin);
  sprintf (tmp, "r%d%+d", fc->cfa_reg, fc->cfa_offset);
  printf ("%-8s ", tmp);

  for (r = 0; r < fc->ncols; r++)
    {
      if (fc->col_type[r] != DW_CFA_unreferenced)
	{
	  switch (fc->col_type[r])
	    {
	    case DW_CFA_undefined:
	      strcpy (tmp, "u");
	      break;
	    case DW_CFA_same_value:
	      strcpy (tmp, "s");
	      break;
	    case DW_CFA_offset:
	      sprintf (tmp, "c%+d", fc->col_offset[r]);
	      break;
	    case DW_CFA_register:
	      sprintf (tmp, "r%d", fc->col_offset[r]);
	      break;
	    default:
	      strcpy (tmp, "n/a");
	      break;
	    }
	  printf ("%-5s", tmp);
	}
    }
  printf ("\n");
}

#define GET(N)	read_n_bytes (abfd, start, N); start += N
#define LEB()	read_unsigned_leb128 (abfd, start, & length_return); start += length_return
#define SLEB()	read_signed_leb128 (abfd, start, & length_return); start += length_return
static Frame_Chunk **fde_array;
static unsigned int fde_array_size=0;
static unsigned int fde_array_elems=0;
static int
compare_fde (const void *a, const void *b)
{
  Frame_Chunk **first, **second;
  first = (Frame_Chunk **) a;
  second = (Frame_Chunk **) b;
  return (*first)->pc_begin - (*second)->pc_begin;
}
static int
dwarf2_read_frame_info_1 (struct objfile *obj, unsigned char *start, unsigned char *end)
{
  bfd *abfd = obj->obfd;
  unsigned char *section_start = start;
  Frame_Chunk *chunks = 0;
  Frame_Chunk *remembered_state = 0;
  Frame_Chunk *rs;
  int is_eh = 0;
  int length_return;
  int max_regs = 0;


  while (start < end)
    {
      unsigned char *saved_start;
      unsigned char *block_end;
      unsigned long length;
      unsigned long cie_id;
      Frame_Chunk *fc;
      Frame_Chunk *cie;
      int need_col_headers = 1;

      saved_start = start;
      length = read_4_bytes (abfd, start);
      start += 4;

      if (length == 0)
	return 1;

      block_end = saved_start + length + 4;
      cie_id = read_4_bytes (abfd, start);
      start += 4;

      printf ("\n%08x %08lx %08lx ", saved_start - section_start, length, cie_id);

      if (is_eh ? (cie_id == 0) : (cie_id == DW_CIE_ID))
	{
	  fc = (Frame_Chunk *) xmalloc (sizeof (Frame_Chunk));
	  memset (fc, 0, sizeof (Frame_Chunk));

	  fc->next = chunks;
	  chunks = fc;
	  fc->chunk_start = saved_start;
	  fc->ncols = 0;
	  fc->col_type = (short int *) xmalloc (sizeof (short int));
	  fc->col_offset = (int *) xmalloc (sizeof (int));
	  frame_need_space (fc, max_regs - 1);

	  start++;		/* version */
	  fc->augmentation = start;

	  while (*start)
	    start++;

	  start++;		/* skip past NUL */

	  if (fc->augmentation[0] == 'z')
	    {
	      int xtra;
	      fc->code_factor = LEB ();
	      fc->data_factor = SLEB ();
	      fc->ra = read_1_byte (abfd, start);
	      start += 1;
	      xtra = LEB ();
	      printf ("skipping %d extra bytes\n", xtra);
	      start += xtra;
	    }
	  else if (strcmp (fc->augmentation, "eh") == 0)
	    {
	      start += 4;
	      fc->code_factor = LEB ();
	      fc->data_factor = SLEB ();
	      fc->ra = read_1_byte (abfd, start);
	      start += 1;
	    }
	  else
	    {
	      fc->code_factor = LEB ();
	      fc->data_factor = SLEB ();
	      fc->ra = read_1_byte (abfd, start);
	      start += 1;
	    }
	  cie = fc;
        printf ("CIE \"%s\" cf=%d df=%d ra=%d\n",
                fc->augmentation, fc->code_factor, fc->data_factor, fc->ra);

	  frame_need_space (fc, fc->ra);
	}
      else
	{
	  unsigned char *look_for;

	  fc = (struct Frame_Chunk *) xmalloc (sizeof (Frame_Chunk));
	  memset (fc, 0, sizeof (Frame_Chunk));

	  look_for = is_eh ? start - 4 - cie_id : (unsigned char *) cie_id;
	  fc->fde_offset = saved_start - section_start;
	  fc->pc_begin = read_4_bytes (abfd, start) + ANOFFSET (obj->section_offsets, SECT_OFF_TEXT (obj));
	  start += 4;
	  fc->pc_range = read_4_bytes (abfd, start);
	  start += 4;
	  if (fde_array_elems+1 > fde_array_size)
	    {
	      fde_array = xrealloc(fde_array, ((fde_array_size+1) * 2) * sizeof (Frame_Chunk *));
	      fde_array_size += 1;
	      fde_array_size *= 2;
	    }
	  fde_array_elems += 1;
	  fde_array[fde_array_elems-1]=fc;
	  for (cie = chunks;
	       cie && (cie->chunk_start != section_start + (int) look_for);
	       cie = cie->next);
	  if (!cie)
	    {
	      warning ("Invalid CIE pointer %08lx in FDE at %08lx\n", cie_id,
		       (long unsigned int)saved_start);
	      start = block_end;
	      fc->ncols = 0;
	      fc->col_type = (short int *) xmalloc (sizeof (short int));
	      fc->col_offset = (int *) xmalloc (sizeof (int));
	      frame_need_space (fc, max_regs - 1);
	      cie = fc;
	      fc->augmentation = "";
	    }
	  else
	    {
	      fc->ncols = cie->ncols;
	      fc->col_type =
		(short int *) xmalloc (fc->ncols * sizeof (short int));
	      fc->col_offset = (int *) xmalloc (fc->ncols * sizeof (int));
	      memcpy (fc->col_type, cie->col_type,
		      fc->ncols * sizeof (short int));
	      memcpy (fc->col_offset, cie->col_offset,
		      fc->ncols * sizeof (int));
	      fc->augmentation = cie->augmentation;
	      fc->code_factor = cie->code_factor;
	      fc->data_factor = cie->data_factor;
	      fc->cfa_reg = cie->cfa_reg;
	      fc->cfa_offset = cie->cfa_offset;
	      fc->ra = cie->ra;
	      frame_need_space (fc, max_regs - 1);
	    }

	  if (cie->augmentation[0] == 'z')
	    {
	      unsigned long l = LEB ();
	      start += l;
	    }

	  printf ("FDE cie=%08x pc=%08lx..%08lx\n",
		  cie->chunk_start-section_start, fc->pc_begin,
		  fc->pc_begin + fc->pc_range);
	}
#if 0
      /* At this point, fc is the current chunk, cie (if any) is set, and we're
         about to interpret instructions for the chunk.  */

      /* This exists for readelf maintainers.  */
//#define FDEBUG 1

      {
	/* Start by making a pass over the chunk, allocating storage
	   and taking note of what registers are used.  */
	unsigned char *tmp = start;

	while (start < block_end)
	  {
	    unsigned op, opa;
	    unsigned long reg;
	    bfd_vma vma;

	    op = *start++;
	    opa = op & 0x3f;
	    if (op & 0xc0)
	      op &= 0xc0;

	    /* Warning: if you add any more cases to this switch, be
	       sure to add them to the corresponding switch below.  */
	    switch (op)
	      {
	      case DW_CFA_advance_loc:
		break;
	      case DW_CFA_offset:
		LEB ();
		frame_need_space (fc, opa);
		fc->col_type[opa] = DW_CFA_undefined;
		break;
	      case DW_CFA_restore:
		frame_need_space (fc, opa);
		fc->col_type[opa] = DW_CFA_undefined;
		break;
	      case DW_CFA_set_loc:
		start += sizeof (vma);
		break;
	      case DW_CFA_advance_loc1:
		start += 1;
		break;
	      case DW_CFA_advance_loc2:
		start += 2;
		break;
	      case DW_CFA_advance_loc4:
		start += 4;
		break;
	      case DW_CFA_offset_extended:
		reg = LEB ();
		LEB ();
		frame_need_space (fc, reg);
		fc->col_type[reg] = DW_CFA_undefined;
		break;
	      case DW_CFA_restore_extended:
		reg = LEB ();
		frame_need_space (fc, reg);
		fc->col_type[reg] = DW_CFA_undefined;
		break;
	      case DW_CFA_undefined:
		reg = LEB ();
		frame_need_space (fc, reg);
		fc->col_type[reg] = DW_CFA_undefined;
		break;
	      case DW_CFA_same_value:
		reg = LEB ();
		frame_need_space (fc, reg);
		fc->col_type[reg] = DW_CFA_undefined;
		break;
	      case DW_CFA_register:
		reg = LEB ();
		LEB ();
		frame_need_space (fc, reg);
		fc->col_type[reg] = DW_CFA_undefined;
		break;
	      case DW_CFA_def_cfa:
		LEB ();
		LEB ();
		break;
	      case DW_CFA_def_cfa_register:
		LEB ();
		break;
	      case DW_CFA_def_cfa_offset:
		LEB ();
		break;
#ifndef DW_CFA_GNU_args_size
#define DW_CFA_GNU_args_size 0x2e
#endif
	      case DW_CFA_GNU_args_size:
		LEB ();
		break;
#ifndef DW_CFA_GNU_negative_offset_extended
#define DW_CFA_GNU_negative_offset_extended 0x2f
#endif
	      case DW_CFA_GNU_negative_offset_extended:
		reg = LEB ();
		LEB ();
		frame_need_space (fc, reg);
		fc->col_type[reg] = DW_CFA_undefined;

	      default:
		break;
	      }
	  }
	start = tmp;
      }
      /* Now we know what registers are used, make a second pass over
         the chunk, this time actually printing out the info.  */

      while (start < block_end)
	{
	  unsigned op, opa;
	  unsigned long ul, reg, roffs;
	  long l, ofs;
	  bfd_vma vma;

	  op = *start++;
	  opa = op & 0x3f;
	  if (op & 0xc0)
	    op &= 0xc0;

	  /* Warning: if you add any more cases to this switch, be
	     sure to add them to the corresponding switch above.  */
	  switch (op)
	    {
	    case DW_CFA_advance_loc:
	      frame_display_row (fc, &need_col_headers, &max_regs);
#if FDEBUG
	      printf ("  DW_CFA_advance_loc: %08x = %08x + %d*%d\n",
		      fc->pc_begin + opa * fc->code_factor, fc->pc_begin, opa,
		      fc->code_factor);
#endif
	      fc->pc_begin += opa * fc->code_factor;
	      break;

	    case DW_CFA_offset:
	      roffs = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_offset: r%d = cfa[%d*%d]\n", opa, roffs,
		      fc->data_factor);
#endif
	      fc->col_type[opa] = DW_CFA_offset;
	      fc->col_offset[opa] = roffs * fc->data_factor;
	      break;

	    case DW_CFA_restore:
#if FDEBUG
	      printf ("  DW_CFA_restore: r%d\n", opa);
#endif
	      fc->col_type[opa] = cie->col_type[opa];
	      fc->col_offset[opa] = cie->col_offset[opa];
	      break;

	    case DW_CFA_set_loc:
	      frame_display_row (fc, &need_col_headers, &max_regs);
	      vma = read_n_bytes (abfd, start, sizeof (vma));
	      start += sizeof (vma);
#if FDEBUG
	      printf ("  DW_CFA_set_loc: %08x\n", vma);
#endif
	      fc->pc_begin = vma;
	      break;

	    case DW_CFA_advance_loc1:
	      frame_display_row (fc, &need_col_headers, &max_regs);
	      ofs = read_1_byte (abfd, start);
	      start += 1;
#if FDEBUG
	      printf ("  DW_CFA_advance_loc1: %08x = %08x + %d*%d\n",
		      fc->pc_begin + ofs * fc->code_factor, fc->pc_begin, ofs,
		      fc->code_factor);
#endif
	      fc->pc_begin += ofs * fc->code_factor;
	      break;

	    case DW_CFA_advance_loc2:
	      frame_display_row (fc, &need_col_headers, &max_regs);
	      ofs = read_2_bytes (abfd, start);
	      start += 2;
#if FDEBUG
	      printf ("  DW_CFA_advance_loc2: %08x = %08x + %d*%d\n",
		      fc->pc_begin + ofs * fc->code_factor, fc->pc_begin, ofs,
		      fc->code_factor);
#endif
	      fc->pc_begin += ofs * fc->code_factor;
	      break;

	    case DW_CFA_advance_loc4:
	      frame_display_row (fc, &need_col_headers, &max_regs);
	      ofs = read_4_bytes (abfd, start);
	      start += 4;
#if FDEBUG
	      printf ("  DW_CFA_advance_loc4: %08x = %08x + %d*%d\n",
		      fc->pc_begin + ofs * fc->code_factor, fc->pc_begin, ofs,
		      fc->code_factor);
#endif
	      fc->pc_begin += ofs * fc->code_factor;
	      break;

	    case DW_CFA_offset_extended:
	      reg = LEB ();
	      roffs = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_offset_extended: r%d = cfa[%d*%d]\n", reg,
		      roffs, fc->data_factor);
#endif
	      fc->col_type[reg] = DW_CFA_offset;
	      fc->col_offset[reg] = roffs * fc->data_factor;
	      break;

	    case DW_CFA_restore_extended:
	      reg = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_restore_extended: r%d\n", reg);
#endif
	      fc->col_type[reg] = cie->col_type[reg];
	      fc->col_offset[reg] = cie->col_offset[reg];
	      break;

	    case DW_CFA_undefined:
	      reg = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_undefined: r%d\n", reg);
#endif
	      fc->col_type[reg] = DW_CFA_undefined;
	      fc->col_offset[reg] = 0;
	      break;

	    case DW_CFA_same_value:
	      reg = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_same_value: r%d\n", reg);
#endif
	      fc->col_type[reg] = DW_CFA_same_value;
	      fc->col_offset[reg] = 0;
	      break;

	    case DW_CFA_register:
	      reg = LEB ();
	      roffs = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_register: r%d\n", reg);
#endif
	      fc->col_type[reg] = DW_CFA_register;
	      fc->col_offset[reg] = roffs;
	      break;

	    case DW_CFA_remember_state:
#if FDEBUG
	      printf ("  DW_CFA_remember_state\n");
#endif
	      rs = (Frame_Chunk *) xmalloc (sizeof (Frame_Chunk));
	      rs->ncols = fc->ncols;
	      rs->col_type =
		(short int *) xmalloc (rs->ncols * sizeof (short int));
	      rs->col_offset = (int *) xmalloc (rs->ncols * sizeof (int));
	      memcpy (rs->col_type, fc->col_type, rs->ncols);
	      memcpy (rs->col_offset, fc->col_offset,
		      rs->ncols * sizeof (int));
	      rs->next = remembered_state;
	      remembered_state = rs;
	      break;

	    case DW_CFA_restore_state:
#if FDEBUG
	      printf ("  DW_CFA_restore_state\n");
#endif
	      rs = remembered_state;
	      remembered_state = rs->next;
	      frame_need_space (fc, rs->ncols - 1);
	      memcpy (fc->col_type, rs->col_type, rs->ncols);
	      memcpy (fc->col_offset, rs->col_offset,
		      rs->ncols * sizeof (int));
	      xfree (rs->col_type);
	      xfree (rs->col_offset);
	      xfree (rs);
	      break;

	    case DW_CFA_def_cfa:
	      fc->cfa_reg = LEB ();
	      fc->cfa_offset = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_def_cfa: reg %d ofs %d\n", fc->cfa_reg,
		      fc->cfa_offset);
#endif
	      break;

	    case DW_CFA_def_cfa_register:
	      fc->cfa_reg = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_def_cfa_reg: %d\n", fc->cfa_reg);
#endif
	      break;

	    case DW_CFA_def_cfa_offset:
	      fc->cfa_offset = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_def_cfa_offset: %d\n", fc->cfa_offset);
#endif
	      break;

	    case DW_CFA_nop:
#if FDEBUG
	      printf ("  DW_CFA_nop\n");
#endif
	      break;

#ifndef DW_CFA_GNU_window_save
#define DW_CFA_GNU_window_save 0x2d
#endif
	    case DW_CFA_GNU_window_save:
#if FDEBUG
	      printf ("  DW_CFA_GNU_window_save\n");
#endif
	      break;

#ifndef DW_CFA_GNU_args_size
#define DW_CFA_GNU_args_size 0x2e
#endif
	    case DW_CFA_GNU_args_size:
	      ul = LEB ();
#if FDEBUG
	      printf ("  DW_CFA_GNU_args_size: %d\n", ul);
#endif
	      break;

#ifndef DW_CFA_GNU_negative_offset_extended
#define DW_CFA_GNU_negative_offset_extended 0x2f
#endif
	    case DW_CFA_GNU_negative_offset_extended:
	      reg = LEB ();
	      l = -LEB ();
	      frame_need_space (fc, reg);
#if FDEBUG
	      printf
		("  DW_CFA_GNU_negative_offset_extended: r%d = cfa[%d*%d]\n",
		 reg, l, fc->data_factor);
#endif
	      fc->col_type[reg] = DW_CFA_offset;
	      fc->col_offset[reg] = l * fc->data_factor;
	      break;

	    default:
	      fprintf (stderr, "unsupported or unknown DW_CFA_%d\n", op);
	      start = block_end;
	    }
	}

      frame_display_row (fc, &need_col_headers, &max_regs);
#endif
      start = block_end;
    }

  printf ("\n");

  return 1;
}

#undef GET
#undef LEB
#undef SLEB
#if 0
      /* At this point, fc is the current chunk, cie (if any) is set, and we're
         about to interpret instructions for the chunk.  */

      /* This exists for readelf maintainers.  */
//#define FDEBUG 1

{
  /* Start by making a pass over the chunk, allocating storage
     and taking note of what registers are used.  */
  unsigned char *tmp = start;

  while (start < block_end)
    {
      unsigned op, opa;
      unsigned long reg;
      bfd_vma vma;

      op = *start++;
      opa = op & 0x3f;
      if (op & 0xc0)
	op &= 0xc0;

      /* Warning: if you add any more cases to this switch, be
         sure to add them to the corresponding switch below.  */
      switch (op)
	{
	case DW_CFA_advance_loc:
	  break;
	case DW_CFA_offset:
	  LEB ();
	  frame_need_space (fc, opa);
	  fc->col_type[opa] = DW_CFA_undefined;
	  break;
	case DW_CFA_restore:
	  frame_need_space (fc, opa);
	  fc->col_type[opa] = DW_CFA_undefined;
	  break;
	case DW_CFA_set_loc:
	  start += sizeof (vma);
	  break;
	case DW_CFA_advance_loc1:
	  start += 1;
	  break;
	case DW_CFA_advance_loc2:
	  start += 2;
	  break;
	case DW_CFA_advance_loc4:
	  start += 4;
	  break;
	case DW_CFA_offset_extended:
	  reg = LEB ();
	  LEB ();
	  frame_need_space (fc, reg);
	  fc->col_type[reg] = DW_CFA_undefined;
	  break;
	case DW_CFA_restore_extended:
	  reg = LEB ();
	  frame_need_space (fc, reg);
	  fc->col_type[reg] = DW_CFA_undefined;
	  break;
	case DW_CFA_undefined:
	  reg = LEB ();
	  frame_need_space (fc, reg);
	  fc->col_type[reg] = DW_CFA_undefined;
	  break;
	case DW_CFA_same_value:
	  reg = LEB ();
	  frame_need_space (fc, reg);
	  fc->col_type[reg] = DW_CFA_undefined;
	  break;
	case DW_CFA_register:
	  reg = LEB ();
	  LEB ();
	  frame_need_space (fc, reg);
	  fc->col_type[reg] = DW_CFA_undefined;
	  break;
	case DW_CFA_def_cfa:
	  LEB ();
	  LEB ();
	  break;
	case DW_CFA_def_cfa_register:
	  LEB ();
	  break;
	case DW_CFA_def_cfa_offset:
	  LEB ();
	  break;
#ifndef DW_CFA_GNU_args_size
#define DW_CFA_GNU_args_size 0x2e
#endif
	case DW_CFA_GNU_args_size:
	  LEB ();
	  break;
#ifndef DW_CFA_GNU_negative_offset_extended
#define DW_CFA_GNU_negative_offset_extended 0x2f
#endif
	case DW_CFA_GNU_negative_offset_extended:
	  reg = LEB ();
	  LEB ();
	  frame_need_space (fc, reg);
	  fc->col_type[reg] = DW_CFA_undefined;

	default:
	  break;
	}
    }
  start = tmp;
}

      /* Now we know what registers are used, make a second pass over
         the chunk, this time actually printing out the info.  */

while (start < block_end)
  {
    unsigned op, opa;
    unsigned long ul, reg, roffs;
    long l, ofs;
    bfd_vma vma;

    op = *start++;
    opa = op & 0x3f;
    if (op & 0xc0)
      op &= 0xc0;

    /* Warning: if you add any more cases to this switch, be
       sure to add them to the corresponding switch above.  */
    switch (op)
      {
      case DW_CFA_advance_loc:
	frame_display_row (fc, &need_col_headers, &max_regs);
#if FDEBUG
	printf ("  DW_CFA_advance_loc: %08x = %08x + %d*%d\n",
		fc->pc_begin + opa * fc->code_factor, fc->pc_begin, opa,
		fc->code_factor);
#endif
	fc->pc_begin += opa * fc->code_factor;
	break;

      case DW_CFA_offset:
	roffs = LEB ();
#if FDEBUG
	printf ("  DW_CFA_offset: r%d = cfa[%d*%d]\n", opa, roffs,
		fc->data_factor);
#endif
	fc->col_type[opa] = DW_CFA_offset;
	fc->col_offset[opa] = roffs * fc->data_factor;
	break;

      case DW_CFA_restore:
#if FDEBUG
	printf ("  DW_CFA_restore: r%d\n", opa);
#endif
	fc->col_type[opa] = cie->col_type[opa];
	fc->col_offset[opa] = cie->col_offset[opa];
	break;

      case DW_CFA_set_loc:
	frame_display_row (fc, &need_col_headers, &max_regs);
	vma = read_n_bytes (abfd, start, sizeof (vma));
	start += sizeof (vma);
#if FDEBUG
	printf ("  DW_CFA_set_loc: %08x\n", vma);
#endif
	fc->pc_begin = vma;
	break;

      case DW_CFA_advance_loc1:
	frame_display_row (fc, &need_col_headers, &max_regs);
	ofs = read_1_byte (abfd, start);
	start += 1;
#if FDEBUG
	printf ("  DW_CFA_advance_loc1: %08x = %08x + %d*%d\n",
		fc->pc_begin + ofs * fc->code_factor, fc->pc_begin, ofs,
		fc->code_factor);
#endif
	fc->pc_begin += ofs * fc->code_factor;
	break;

      case DW_CFA_advance_loc2:
	frame_display_row (fc, &need_col_headers, &max_regs);
	ofs = read_2_bytes (abfd, start);
	start += 2;
#if FDEBUG
	printf ("  DW_CFA_advance_loc2: %08x = %08x + %d*%d\n",
		fc->pc_begin + ofs * fc->code_factor, fc->pc_begin, ofs,
		fc->code_factor);
#endif
	fc->pc_begin += ofs * fc->code_factor;
	break;

      case DW_CFA_advance_loc4:
	frame_display_row (fc, &need_col_headers, &max_regs);
	ofs = read_4_bytes (abfd, start);
	start += 4;
#if FDEBUG
	printf ("  DW_CFA_advance_loc4: %08x = %08x + %d*%d\n",
		fc->pc_begin + ofs * fc->code_factor, fc->pc_begin, ofs,
		fc->code_factor);
#endif
	fc->pc_begin += ofs * fc->code_factor;
	break;

      case DW_CFA_offset_extended:
	reg = LEB ();
	roffs = LEB ();
#if FDEBUG
	printf ("  DW_CFA_offset_extended: r%d = cfa[%d*%d]\n", reg, roffs,
		fc->data_factor);
#endif
	fc->col_type[reg] = DW_CFA_offset;
	fc->col_offset[reg] = roffs * fc->data_factor;
	break;

      case DW_CFA_restore_extended:
	reg = LEB ();
#if FDEBUG
	printf ("  DW_CFA_restore_extended: r%d\n", reg);
#endif
	fc->col_type[reg] = cie->col_type[reg];
	fc->col_offset[reg] = cie->col_offset[reg];
	break;

      case DW_CFA_undefined:
	reg = LEB ();
#if FDEBUG
	printf ("  DW_CFA_undefined: r%d\n", reg);
#endif
	fc->col_type[reg] = DW_CFA_undefined;
	fc->col_offset[reg] = 0;
	break;

      case DW_CFA_same_value:
	reg = LEB ();
#if FDEBUG
	printf ("  DW_CFA_same_value: r%d\n", reg);
#endif
	fc->col_type[reg] = DW_CFA_same_value;
	fc->col_offset[reg] = 0;
	break;

      case DW_CFA_register:
	reg = LEB ();
	roffs = LEB ();
#if FDEBUG
	printf ("  DW_CFA_register: r%d\n", reg);
#endif
	fc->col_type[reg] = DW_CFA_register;
	fc->col_offset[reg] = roffs;
	break;

      case DW_CFA_remember_state:
#if FDEBUG
	printf ("  DW_CFA_remember_state\n");
#endif
	rs = (Frame_Chunk *) xmalloc (sizeof (Frame_Chunk));
	rs->ncols = fc->ncols;
	rs->col_type = (short int *) xmalloc (rs->ncols * sizeof (short int));
	rs->col_offset = (int *) xmalloc (rs->ncols * sizeof (int));
	memcpy (rs->col_type, fc->col_type, rs->ncols);
	memcpy (rs->col_offset, fc->col_offset, rs->ncols * sizeof (int));
	rs->next = remembered_state;
	remembered_state = rs;
	break;

      case DW_CFA_restore_state:
#if FDEBUG
	printf ("  DW_CFA_restore_state\n");
#endif
	rs = remembered_state;
	remembered_state = rs->next;
	frame_need_space (fc, rs->ncols - 1);
	memcpy (fc->col_type, rs->col_type, rs->ncols);
	memcpy (fc->col_offset, rs->col_offset, rs->ncols * sizeof (int));
 xfree (rs->col_type);
 xfree (rs->col_offset);
 xfree (rs);
	break;

      case DW_CFA_def_cfa:
	fc->cfa_reg = LEB ();
	fc->cfa_offset = LEB ();
#if FDEBUG
	printf ("  DW_CFA_def_cfa: reg %d ofs %d\n", fc->cfa_reg,
		fc->cfa_offset);
#endif
	break;

      case DW_CFA_def_cfa_register:
	fc->cfa_reg = LEB ();
#if FDEBUG
	printf ("  DW_CFA_def_cfa_reg: %d\n", fc->cfa_reg);
#endif
	break;

      case DW_CFA_def_cfa_offset:
	fc->cfa_offset = LEB ();
#if FDEBUG
	printf ("  DW_CFA_def_cfa_offset: %d\n", fc->cfa_offset);
#endif
	break;

      case DW_CFA_nop:
#if FDEBUG
	printf ("  DW_CFA_nop\n");
#endif
	break;

#ifndef DW_CFA_GNU_window_save
#define DW_CFA_GNU_window_save 0x2d
#endif
      case DW_CFA_GNU_window_save:
#if FDEBUG
	printf ("  DW_CFA_GNU_window_save\n");
#endif
	break;

#ifndef DW_CFA_GNU_args_size
#define DW_CFA_GNU_args_size 0x2e
#endif
      case DW_CFA_GNU_args_size:
	ul = LEB ();
#if FDEBUG
	printf ("  DW_CFA_GNU_args_size: %d\n", ul);
#endif
	break;

#ifndef DW_CFA_GNU_negative_offset_extended
#define DW_CFA_GNU_negative_offset_extended 0x2f
#endif
      case DW_CFA_GNU_negative_offset_extended:
	reg = LEB ();
	l = -LEB ();
	frame_need_space (fc, reg);
#if FDEBUG
	printf ("  DW_CFA_GNU_negative_offset_extended: r%d = cfa[%d*%d]\n",
		reg, l, fc->data_factor);
#endif
	fc->col_type[reg] = DW_CFA_offset;
	fc->col_offset[reg] = l * fc->data_factor;
	break;

      default:
	fprintf (stderr, "unsupported or unknown DW_CFA_%d\n", op);
	start = block_end;
      }
  }

frame_display_row (fc, &need_col_headers, &max_regs);
#endif
/* Read in the .debug_frame/.eh_frame section, and process it */
static void
dwarf2_read_frame_info (struct objfile *obj)
{
  dwarf2_read_frame_info_1 (obj, dwarf2_frame_buffer,
			   dwarf2_frame_buffer + dwarf2_frame_size);
  qsort(fde_array, fde_array_elems, sizeof(Frame_Chunk *), compare_fde);
}

/* Get the Call Frame Annotation info for a given PC. Uses a binary
   search through the fde_array. */
Frame_Chunk *
dwarf2_get_cfa_for_addr (CORE_ADDR pc)
{
  size_t lo, hi;
  Frame_Chunk *chunk=NULL;
  lo=0;
  hi=fde_array_elems;
  do
    {
      size_t i = (lo + hi) / 2;
      chunk = fde_array[i];
      if (pc < chunk->pc_begin)
	hi = i;
      else if (pc >= chunk->pc_begin + chunk->pc_range)
	lo = i + 1;
      else 
	return chunk;
    } while (lo < hi);
  
  return chunk;
}
/* We can search the fde array ourselves very inexpensively, so there
   is no need to pass along the actual frame program data. */
void 
dwarf2_execute_cfa_program (struct frame_info *frame)
{
  
}
