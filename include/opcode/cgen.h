/* Header file for targets using CGEN: Cpu tools GENerator.

Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

This file is part of GDB, the GNU debugger, and the GNU Binutils.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef CGEN_H
#define CGEN_H

/* Prepend the arch name, defined in <arch>-opc.h, and _cgen_ to symbol S.
   The lack of spaces in the arg list is important for non-stdc systems.
   This file is included by <arch>-opc.h.
   It can be included independently of <arch>-opc.h, in which case the arch
   dependent portions will be declared as "unknown_cgen_foo".  */

#ifndef CGEN_SYM
#define CGEN_SYM(s) CONCAT3 (unknown,_cgen_,s)
#endif

/* This file contains the static (unchanging) pieces and as much other stuff
   as we can reasonably put here.  It's generally cleaner to put stuff here
   rather than having it machine generated if possible.  */

/* The assembler syntax is made up of expressions (duh...).
   At the lowest level the values are mnemonics, register names, numbers, etc.
   Above that are subexpressions, if any (an example might be the
   "effective address" in m68k cpus).  At the second highest level are the
   insns themselves.  Above that are pseudo-insns, synthetic insns, and macros,
   if any.
*/

/* Lots of cpu's have a fixed insn size, or one which rarely changes,
   and it's generally easier to handle these by treating the insn as an
   integer type, rather than an array of characters.  So we allow targets
   to control this.  When an integer type the value is in host byte order,
   when an array of characters the value is in target byte order.  */

typedef unsigned int CGEN_INSN_INT;
#if CGEN_INT_INSN_P
typedef CGEN_INSN_INT CGEN_INSN_BYTES;
typedef CGEN_INSN_INT *CGEN_INSN_BYTES_PTR;
#else
typedef unsigned char *CGEN_INSN_BYTES;
typedef unsigned char *CGEN_INSN_BYTES_PTR;
#endif

#ifdef __GNUC__
#define CGEN_INLINE inline
#else
#define CGEN_INLINE
#endif

enum cgen_endian
{
  CGEN_ENDIAN_UNKNOWN,
  CGEN_ENDIAN_LITTLE,
  CGEN_ENDIAN_BIG
};

/* Forward decl.  */
typedef struct cgen_insn CGEN_INSN;

/* Opaque pointer version for use by external world.  */
typedef struct cgen_opcode_table * CGEN_OPCODE_DESC;

/* Attributes.
   Attributes are used to describe various random things.
   ??? Will need to be revisited when expression support is added.  */

/* Struct to record attribute information.  */
typedef struct
{
  unsigned char num_nonbools;
  unsigned int bool;
  unsigned int nonbool[1];
} CGEN_ATTR;

/* Define a structure member for attributes with N non-boolean entries.
   The attributes are sorted so that the non-boolean ones come first.
   num_nonbools: count of nonboolean attributes
   bool: values of boolean attributes
   nonbool: values of non-boolean attributes
   There is a maximum of 32 attributes total.  */
#define CGEN_ATTR_TYPE(n) \
struct { unsigned char num_nonbools; \
	   unsigned int bool; \
	   unsigned int nonbool[(n) ? (n) : 1]; }

/* Return the boolean attributes.  */
#define CGEN_ATTR_BOOLS(a) ((a)->bool)

/* Given an attribute number, return its mask.  */
#define CGEN_ATTR_MASK(attr) (1 << (attr))

/* Return the value of boolean attribute ATTR in ATTRS.  */
#define CGEN_BOOL_ATTR(attrs, attr) \
((CGEN_ATTR_MASK (attr) & (attrs)) != 0)

/* Return value of attribute ATTR in ATTR_TABLE for OBJ.
   OBJ is a pointer to the entity that has the attributes.
   It's not used at present but is reserved for future purposes.  */
#define CGEN_ATTR_VALUE(obj, attr_table, attr) \
((unsigned int) (attr) < (attr_table)->num_nonbools \
 ? ((attr_table)->nonbool[attr]) \
 : (((attr_table)->bool & (1 << (attr))) != 0))

/* Attribute name/value tables.
   These are used to assist parsing of descriptions at runtime.  */

typedef struct
{
  const char * name;
  int value;
} CGEN_ATTR_ENTRY;

/* For each domain (ifld,operand,insn), list of attributes.  */

typedef struct
{
  const char * name;
  /* NULL for boolean attributes.  */
  const CGEN_ATTR_ENTRY * vals;
} CGEN_ATTR_TABLE;

/* Parse result (also extraction result).

   The result of parsing an insn is stored here.
   To generate the actual insn, this is passed to the insert handler.
   When printing an insn, the result of extraction is stored here.
   To print the insn, this is passed to the print handler.

   It is machine generated so we don't define it here,
   but we do need a forward decl for the handler fns.

   There is one member for each possible field in the insn.
   The type depends on the field.
   Also recorded here is the computed length of the insn for architectures
   where it varies.
*/

typedef struct cgen_fields CGEN_FIELDS;

/* Total length of the insn, as recorded in the `fields' struct.  */
/* ??? The field insert handler has lots of opportunities for optimization
   if it ever gets inlined.  On architectures where insns all have the same
   size, may wish to detect that and make this macro a constant - to allow
   further optimizations.  */
#define CGEN_FIELDS_BITSIZE(fields) ((fields)->length)

/* Extraction support for variable length insn sets.  */

/* When disassembling we don't know the number of bytes to read at the start.
   So the first CGEN_BASE_INSN_BITSIZE bytes are read at the start and the rest
   are read when needed.  This struct controls this.  It is basically the
   disassemble_info stuff, except that we provide a cache for values already
   read (since bytes can typically be read several times to fetch multiple
   operands that may be in them), and that extraction of fields is needed
   in contexts other than disassembly.  */

typedef struct {
  /* A pointer to the disassemble_info struct.
     We don't require dis-asm.h so we use PTR for the type here.
     If NULL, BYTES is full of valid data (VALID == -1).  */
  PTR dis_info;
  /* Points to a working buffer of sufficient size.  */
  unsigned char *bytes;
  /* Mask of bytes that are valid in BYTES.  */
  unsigned int valid;
} CGEN_EXTRACT_INFO;

/* Associated with each insn or expression is a set of "handlers" for
   performing operations like parsing, printing, etc.  These require a bfd_vma
   value to be passed around but we don't want all applications to need bfd.h.
   So this stuff is only provided if bfd.h has been included.  */

#ifdef BFD_VERSION

/* Parse handler.
   OD is an opcode table descriptor.
   INSN is a pointer to a struct describing the insn being parsed.
   STRP is a pointer to a pointer to the text being parsed.
   FIELDS is a pointer to a cgen_fields struct in which the results are placed.
   If the expression is successfully parsed, *STRP is updated.
   If not it is left alone.
   The result is NULL if success or an error message.  */
typedef const char * (cgen_parse_fn)
     PARAMS ((CGEN_OPCODE_DESC /*od*/, const CGEN_INSN * /*insn*/,
	      const char ** /*strp*/, CGEN_FIELDS * /*fields*/));

/* Insert handler.
   OD is an opcode table descriptor.
   INSN is a pointer to a struct describing the insn being parsed.
   FIELDS is a pointer to a cgen_fields struct from which the values
   are fetched.
   INSNP is a pointer to a buffer in which to place the insn.
   PC is the pc value of the insn.
   The result is an error message or NULL if success.  */
typedef const char * (cgen_insert_fn)
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN * /*insn*/,
	      CGEN_FIELDS * /*fields*/, CGEN_INSN_BYTES_PTR /*insnp*/,
	      bfd_vma /*pc*/));

/* Extract handler.
   OD is an opcode table descriptor.
   INSN is a pointer to a struct describing the insn being parsed.
   The second argument is a pointer to a struct controlling extraction
   (only used for variable length insns).
   EX_INFO is a pointer to a struct for controlling reading of further
   bytes for the insn.
   BASE_INSN is the first CGEN_BASE_INSN_SIZE bytes (host order).
   FIELDS is a pointer to a cgen_fields struct in which the results are placed.
   PC is the pc value of the insn.
   The result is the length of the insn in bits or zero if not recognized.  */
typedef int (cgen_extract_fn)
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN * /*insn*/,
	      CGEN_EXTRACT_INFO * /*ex_info*/, unsigned long /*base_insn*/,
	      CGEN_FIELDS * /*fields*/, bfd_vma /*pc*/));

/* Print handler.
   OD is an opcode table descriptor.
   INFO is a pointer to the disassembly info.
   Eg: disassemble_info.  It's defined as `PTR' so this file can be included
   without dis-asm.h.
   INSN is a pointer to a struct describing the insn being printed.
   FIELDS is a pointer to a cgen_fields struct.
   PC is the pc value of the insn.
   LEN is the length of the insn, in bits.  */
typedef void (cgen_print_fn)
     PARAMS ((CGEN_OPCODE_DESC /*od*/, PTR /*info*/,
	      const CGEN_INSN * /*insn*/,
	      CGEN_FIELDS * /*fields*/, bfd_vma /*pc*/, int /*len*/));

/* The `parse' and `insert' fields are indices into these tables.
   The elements are pointer to specialized handler functions.
   Element 0 is special, it means use the default handler.  */
extern cgen_parse_fn * const CGEN_SYM (parse_handlers) [];
#define CGEN_PARSE_FN(x) (CGEN_SYM (parse_handlers)[(x)->base.parse])
extern cgen_insert_fn * const CGEN_SYM (insert_handlers) [];
#define CGEN_INSERT_FN(x) (CGEN_SYM (insert_handlers)[(x)->base.insert])

/* Likewise for the `extract' and `print' fields.  */
extern cgen_extract_fn * const CGEN_SYM (extract_handlers) [];
#define CGEN_EXTRACT_FN(x) (CGEN_SYM (extract_handlers)[(x)->base.extract])
extern cgen_print_fn * const CGEN_SYM (print_handlers) [];
#define CGEN_PRINT_FN(x) (CGEN_SYM (print_handlers)[(x)->base.print])

/* Default insn parser, printer.  */
extern cgen_parse_fn CGEN_SYM (parse_insn);
extern cgen_insert_fn CGEN_SYM (insert_insn);
extern cgen_extract_fn CGEN_SYM (extract_insn);
extern cgen_print_fn CGEN_SYM (print_insn);

#endif /* BFD_VERSION */

/* Base class of parser/printer.
   (Don't read too much into the use of the phrase "base class".
   It's a name I'm using to organize my thoughts.)

   Instructions and expressions all share this data in common.
   It's a collection of the common elements needed to parse, insert, extract,
   and print each of them.
   This is an underutilized facility, and exists as a potential escape hatch
   for handling more complicated assembler syntaxes.  */

struct cgen_base
{
  /* Indices into the handler tables.
     We could use pointers here instead, but in the case of the insn table,
     90% of them would be identical and that's a lot of redundant data.
     0 means use the default (what the default is is up to the code).
     Using indices also keeps assembler code out of the disassembler and
     vice versa.  */
  unsigned char parse, insert, extract, print;
};

/* Assembler interface.

   The interface to the assembler is intended to be clean in the sense that
   libopcodes.a is a standalone entity and could be used with any assembler.
   Not that one would necessarily want to do that but rather that it helps
   keep a clean interface.  The interface will obviously be slanted towards
   GAS, but at least it's a start.
   ??? Note that one possible use of the assembler besides GAS is GDB.

   Parsing is controlled by the assembler which calls
   CGEN_SYM (assemble_insn).  If it can parse and build the entire insn
   it doesn't call back to the assembler.  If it needs/wants to call back
   to the assembler, cgen_parse_operand_fn is called which can either

   - return a number to be inserted in the insn
   - return a "register" value to be inserted
     (the register might not be a register per pe)
   - queue the argument and return a marker saying the expression has been
     queued (eg: a fix-up)
   - return an error message indicating the expression wasn't recognizable

   The result is an error message or NULL for success.
   The parsed value is stored in the bfd_vma *.  */

/* Values for indicating what the caller wants.  */
enum cgen_parse_operand_type
{
  CGEN_PARSE_OPERAND_INIT,
  CGEN_PARSE_OPERAND_INTEGER,
  CGEN_PARSE_OPERAND_ADDRESS
};

/* Values for indicating what was parsed.  */
enum cgen_parse_operand_result
{
  CGEN_PARSE_OPERAND_RESULT_NUMBER,
  CGEN_PARSE_OPERAND_RESULT_REGISTER,
  CGEN_PARSE_OPERAND_RESULT_QUEUED,
  CGEN_PARSE_OPERAND_RESULT_ERROR
};

#ifdef BFD_VERSION /* Don't require bfd.h unnecessarily.  */
typedef const char * (cgen_parse_operand_fn)
     PARAMS ((CGEN_OPCODE_DESC,
	      enum cgen_parse_operand_type, const char **, int, int,
	      enum cgen_parse_operand_result *, bfd_vma *));
#else
typedef const char * (cgen_parse_operand_fn) ();
#endif

/* Set the cgen_parse_operand_fn callback.  */
extern void cgen_set_parse_operand_fn
     PARAMS ((CGEN_OPCODE_DESC, cgen_parse_operand_fn));

/* Called before trying to match a table entry with the insn.  */
extern void cgen_init_parse_operand PARAMS ((CGEN_OPCODE_DESC));

/* Operand values (keywords, integers, symbols, etc.)  */

/* Types of assembler elements.  */

enum cgen_asm_type
{
  CGEN_ASM_KEYWORD, CGEN_ASM_MAX
};

/* List of hardware elements.  */

typedef struct cgen_hw_entry
{
  /* The type of this entry, one of `enum hw_type'.
     This is an int and not the enum as the latter may not be declared yet.  */
  int type;
  const struct cgen_hw_entry * next;
  char * name;
  enum cgen_asm_type asm_type;
  PTR asm_data;
#ifndef CGEN_HW_NBOOL_ATTRS
#define CGEN_HW_NBOOL_ATTRS 1
#endif
  const CGEN_ATTR_TYPE (CGEN_HW_NBOOL_ATTRS) attrs;
#define CGEN_HW_ATTRS(hw) (&(hw)->attrs)
} CGEN_HW_ENTRY;

/* Return value of attribute ATTR in HW.  */
#define CGEN_HW_ATTR(hw, attr) \
CGEN_ATTR_VALUE (hw, CGEN_HW_ATTRS (hw), attr)

extern const CGEN_HW_ENTRY * cgen_hw_lookup_by_name
     PARAMS ((CGEN_OPCODE_DESC, const char *));
extern const CGEN_HW_ENTRY * cgen_hw_lookup_by_num
     PARAMS ((CGEN_OPCODE_DESC, int));

/* This struct is used to describe things like register names, etc.  */

typedef struct cgen_keyword_entry
{
  /* Name (as in register name).  */
  char * name;

  /* Value (as in register number).
     The value cannot be -1 as that is used to indicate "not found".
     IDEA: Have "FUNCTION" attribute? [function is called to fetch value].  */
  int value;

  /* Attributes.
     This should, but technically needn't, appear last.  It is a variable sized
     array in that one architecture may have 1 nonbool attribute and another
     may have more.  Having this last means the non-architecture specific code
     needn't care.  */
  /* ??? Moving this last should be done by treating keywords like insn lists
     and moving the `next' fields into a CGEN_KEYWORD_LIST struct.  */
  /* FIXME: Not used yet.  */
#ifndef CGEN_KEYWORD_NBOOL_ATTRS
#define CGEN_KEYWORD_NBOOL_ATTRS 1
#endif
  const CGEN_ATTR_TYPE (CGEN_KEYWORD_NBOOL_ATTRS) attrs;

  /* Next name hash table entry.  */
  struct cgen_keyword_entry *next_name;
  /* Next value hash table entry.  */
  struct cgen_keyword_entry *next_value;
} CGEN_KEYWORD_ENTRY;

/* Top level struct for describing a set of related keywords
   (e.g. register names).

   This struct supports runtime entry of new values, and hashed lookups.  */

typedef struct cgen_keyword
{
  /* Pointer to initial [compiled in] values.  */
  CGEN_KEYWORD_ENTRY * init_entries;
  
  /* Number of entries in `init_entries'.  */
  unsigned int num_init_entries;
  
  /* Hash table used for name lookup.  */
  CGEN_KEYWORD_ENTRY ** name_hash_table;
  
  /* Hash table used for value lookup.  */
  CGEN_KEYWORD_ENTRY ** value_hash_table;
  
  /* Number of entries in the hash_tables.  */
  unsigned int hash_table_size;
  
  /* Pointer to null keyword "" entry if present.  */
  const CGEN_KEYWORD_ENTRY * null_entry;
} CGEN_KEYWORD;

/* Structure used for searching.  */

typedef struct
{
  /* Table being searched.  */
  const CGEN_KEYWORD * table;
  
  /* Specification of what is being searched for.  */
  const char * spec;
  
  /* Current index in hash table.  */
  unsigned int current_hash;
  
  /* Current element in current hash chain.  */
  CGEN_KEYWORD_ENTRY * current_entry;
} CGEN_KEYWORD_SEARCH;

/* Lookup a keyword from its name.  */
const CGEN_KEYWORD_ENTRY * cgen_keyword_lookup_name
  PARAMS ((CGEN_KEYWORD *, const char *));
/* Lookup a keyword from its value.  */
const CGEN_KEYWORD_ENTRY * cgen_keyword_lookup_value
  PARAMS ((CGEN_KEYWORD *, int));
/* Add a keyword.  */
void cgen_keyword_add PARAMS ((CGEN_KEYWORD *, CGEN_KEYWORD_ENTRY *));
/* Keyword searching.
   This can be used to retrieve every keyword, or a subset.  */
CGEN_KEYWORD_SEARCH cgen_keyword_search_init
  PARAMS ((CGEN_KEYWORD *, const char *));
const CGEN_KEYWORD_ENTRY *cgen_keyword_search_next
  PARAMS ((CGEN_KEYWORD_SEARCH *));

/* Operand value support routines.  */

extern const char * cgen_parse_keyword
     PARAMS ((CGEN_OPCODE_DESC, const char **, CGEN_KEYWORD *, long *));
#ifdef BFD_VERSION /* Don't require bfd.h unnecessarily.  */
extern const char * cgen_parse_signed_integer
     PARAMS ((CGEN_OPCODE_DESC, const char **, int, long *));
extern const char * cgen_parse_unsigned_integer
     PARAMS ((CGEN_OPCODE_DESC, const char **, int, unsigned long *));
extern const char * cgen_parse_address
     PARAMS ((CGEN_OPCODE_DESC, const char **, int, int,
	      enum cgen_parse_operand_result *, bfd_vma *));
extern const char * cgen_validate_signed_integer
     PARAMS ((long, long, long));
extern const char * cgen_validate_unsigned_integer
     PARAMS ((unsigned long, unsigned long, unsigned long));
#endif

/* Operand modes.  */

/* ??? This duplicates the values in arch.h.  Revisit.
   These however need the CGEN_ prefix [as does everything in this file].  */
/* ??? Targets may need to add their own modes so we may wish to move this
   to <arch>-opc.h, or add a hook.  */

enum cgen_mode {
  CGEN_MODE_VOID, /* FIXME: rename simulator's VM to VOID */
  CGEN_MODE_BI, CGEN_MODE_QI, CGEN_MODE_HI, CGEN_MODE_SI, CGEN_MODE_DI,
  CGEN_MODE_UBI, CGEN_MODE_UQI, CGEN_MODE_UHI, CGEN_MODE_USI, CGEN_MODE_UDI,
  CGEN_MODE_SF, CGEN_MODE_DF, CGEN_MODE_XF, CGEN_MODE_TF,
  CGEN_MODE_MAX
};

/* FIXME: Until simulator is updated.  */
#define CGEN_MODE_VM CGEN_MODE_VOID

/* This struct defines each entry in the operand table.  */

typedef struct cgen_operand
{
  /* Name as it appears in the syntax string.  */
  char * name;

  /* The hardware element associated with this operand.  */
  const CGEN_HW_ENTRY *hw;

  /* FIXME: We don't yet record ifield definitions, which we should.
     When we do it might make sense to delete start/length (since they will
     be duplicated in the ifield's definition) and replace them with a
     pointer to the ifield entry.  Note that as more complicated situations
     need to be handled, going more and more with an OOP paradigm will help
     keep the complication under control.  Of course, this was the goal from
     the start, but getting there in one step was too much too soon.  */

  /* Bit position (msb of first byte = bit 0).
     This is just a hint, and may be unused in more complex operands.
     May be unused for a modifier.  */
  unsigned char start;

  /* The number of bits in the operand.
     This is just a hint, and may be unused in more complex operands.
     May be unused for a modifier.  */
  unsigned char length;

#if 0 /* ??? Interesting idea but relocs tend to get too complicated,
	 and ABI dependent, for simple table lookups to work.  */
  /* Ideally this would be the internal (external?) reloc type.  */
  int reloc_type;
#endif

  /* Attributes.
     This should, but technically needn't, appear last.  It is a variable sized
     array in that one architecture may have 1 nonbool attribute and another
     may have more.  Having this last means the non-architecture specific code
     needn't care, now or tomorrow.  */
#ifndef CGEN_OPERAND_NBOOL_ATTRS
#define CGEN_OPERAND_NBOOL_ATTRS 1
#endif
  const CGEN_ATTR_TYPE (CGEN_OPERAND_NBOOL_ATTRS) attrs;
#define CGEN_OPERAND_ATTRS(operand) (&(operand)->attrs)
} CGEN_OPERAND;

/* Return value of attribute ATTR in OPERAND.  */
#define CGEN_OPERAND_ATTR(operand, attr) \
CGEN_ATTR_VALUE (operand, CGEN_OPERAND_ATTRS (operand), attr)

/* The operand table is currently a very static entity.  */
extern const CGEN_OPERAND CGEN_SYM (operand_table)[];

enum cgen_operand_type;

#define CGEN_OPERAND_INDEX(operand) ((int) ((operand) - CGEN_SYM (operand_table)))
/* FIXME: Rename, cpu-opc.h defines this as the typedef of the enum.  */
#define CGEN_OPERAND_TYPE(operand) ((enum cgen_operand_type) CGEN_OPERAND_INDEX (operand))
#define CGEN_OPERAND_ENTRY(n) (& CGEN_SYM (operand_table) [n])

/* Instruction operand instances.

   For each instruction, a list of the hardware elements that are read and
   written are recorded.  */

/* The type of the instance.  */
enum cgen_operand_instance_type {
  /* End of table marker.  */
  CGEN_OPERAND_INSTANCE_END = 0,
  CGEN_OPERAND_INSTANCE_INPUT, CGEN_OPERAND_INSTANCE_OUTPUT
};

typedef struct
{
  /* The type of this operand.  */
  enum cgen_operand_instance_type type;
#define CGEN_OPERAND_INSTANCE_TYPE(opinst) ((opinst)->type)

  /* Name of operand.  */
  const char *name;
#define CGEN_OPERAND_INSTANCE_NAME(opinst) ((opinst)->name)

  /* The hardware element referenced.  */
  const CGEN_HW_ENTRY *hw;
#define CGEN_OPERAND_INSTANCE_HW(opinst) ((opinst)->hw)

  /* The mode in which the operand is being used.  */
  enum cgen_mode mode;
#define CGEN_OPERAND_INSTANCE_MODE(opinst) ((opinst)->mode)

  /* The operand table entry or NULL if there is none (i.e. an explicit
     hardware reference).  */
  const CGEN_OPERAND *operand;
#define CGEN_OPERAND_INSTANCE_OPERAND(opinst) ((opinst)->operand)

  /* If `operand' is NULL, the index (e.g. into array of registers).  */
  int index;
#define CGEN_OPERAND_INSTANCE_INDEX(opinst) ((opinst)->index)
} CGEN_OPERAND_INSTANCE;

/* Syntax string.

   Each insn format and subexpression has one of these.

   The syntax "string" consists of characters (n > 0 && n < 128), and operand
   values (n >= 128), and is terminated by 0.  Operand values are 128 + index
   into the operand table.  The operand table doesn't exist in C, per se, as
   the data is recorded in the parse/insert/extract/print switch statements. */

#ifndef CGEN_MAX_SYNTAX_BYTES
#define CGEN_MAX_SYNTAX_BYTES 16
#endif

typedef struct
{
  unsigned char syntax[CGEN_MAX_SYNTAX_BYTES];
} CGEN_SYNTAX;

#define CGEN_SYNTAX_STRING(syn) (syn->syntax)
#define CGEN_SYNTAX_CHAR_P(c) ((c) < 128)
#define CGEN_SYNTAX_CHAR(c) (c)
#define CGEN_SYNTAX_FIELD(c) ((c) - 128)
#define CGEN_SYNTAX_MAKE_FIELD(c) ((c) + 128)

/* ??? I can't currently think of any case where the mnemonic doesn't come
   first [and if one ever doesn't building the hash tables will be tricky].
   However, we treat mnemonics as just another operand of the instruction.
   A value of 1 means "this is where the mnemonic appears".  1 isn't
   special other than it's a non-printable ASCII char.  */
#define CGEN_SYNTAX_MNEMONIC       1
#define CGEN_SYNTAX_MNEMONIC_P(ch) ((ch) == CGEN_SYNTAX_MNEMONIC)

/* Instruction formats.

   Instructions are grouped by format.  Associated with an instruction is its
   format.  Each opcode table entry contains a format table entry.
   ??? There is usually very few formats compared with the number of insns,
   so one can reduce the size of the opcode table by recording the format table
   as a separate entity.  Given that we currently don't, format table entries
   are also distinguished by their operands.  This increases the size of the
   table, but reduces the number of tables.  It's all minutiae anyway so it
   doesn't really matter [at this point in time].

   ??? Support for variable length ISA's is wip.  */

typedef struct
{
  /* Length that MASK and VALUE have been calculated to
     [VALUE is recorded elsewhere].
     Normally it is CGEN_BASE_INSN_BITSIZE.  On [V]LIW architectures where
     the base insn size may be larger than the size of an insn, this field is
     less than CGEN_BASE_INSN_BITSIZE.  */
  unsigned char mask_length;

  /* Total length of instruction, in bits.  */
  unsigned char length;

  /* Mask to apply to the first MASK_LENGTH bits.
     Each insn's value is stored with the insn.
     The first step in recognizing an insn for disassembly is
     (opcode & mask) == value.  */
  unsigned int mask;
} CGEN_FORMAT;

/* This struct defines each entry in the instruction table.  */

struct cgen_insn
{
  /* This field is an array of functions that operand on this entry.  */
  struct cgen_base base;
#define CGEN_INSN_BASE(insn) (&(insn)->base)

  /* Each real instruction is enumerated.
     The simulator use to use this value but currently no longer does.
     This may go away in time.  */
  int num;
#define CGEN_INSN_NUM(insn) ((insn)->num)

  /* Name of entry (that distinguishes it from all other entries).
     This is used, for example, in simulator profiling results.  */
  /* ??? If mnemonics have operands, try to print full mnemonic.  */
  const char * name;
#define CGEN_INSN_NAME(insn) ((insn)->name)

  /* Mnemonic.  This is used when parsing and printing the insn.
     In the case of insns that have operands on the mnemonics, this is
     only the constant part.  E.g. for conditional execution of an `add' insn,
     where the full mnemonic is addeq, addne, etc., this is only "add".  */
  const char * mnemonic;
#define CGEN_INSN_MNEMONIC(insn) ((insn)->mnemonic)

  /* Syntax string.  */
  const CGEN_SYNTAX syntax;
#define CGEN_INSN_SYNTAX(insn) (& (insn)->syntax)

  /* Format entry.  */
  const CGEN_FORMAT format;
#define CGEN_INSN_MASK_BITSIZE(insn) ((insn)->format.mask_length)
#define CGEN_INSN_BITSIZE(insn) ((insn)->format.length)

  /* Instruction opcode value.  */
  unsigned int value;
#define CGEN_INSN_VALUE(insn) ((insn)->value)
#define CGEN_INSN_MASK(insn) ((insn)->format.mask)

#if 0 /* ??? Disabled for now as there is a problem with embedded newlines
	 and the table is already pretty big.  Should perhaps be moved
	 to a file of its own.  */
  /* Semantics, as CDL.  */
  /* ??? Note that the operand instance table could be computed at runtime
     if we parse this and cache the results.  */
  const char *cdx;
#define CGEN_INSN_CDX(insn) ((insn)->cdx)
#endif

  /* Opaque pointer to "subclass" specific data.
     In the case of real insns this points to a NULL entry terminated
     table of operands used, or NULL if none.
     In the case of macro insns this points to data to control the expansion.
     ??? I'd rather not get carried away and lay things out with pedantic
     purity right now.  Sure, other fields might better be tucked away in
     `data'.  Not now.  */
  PTR data;
#define CGEN_INSN_DATA(insn) ((insn)->data)
#define CGEN_INSN_OPERANDS(insn) ((CGEN_OPERAND_INSTANCE *) (insn)->data)

  /* Attributes.
     This must appear last.  It is a variable sized array in that one
     architecture may have 1 nonbool attribute and another may have more.
     Having this last means the non-architecture specific code needn't
     care.  */
#ifndef CGEN_INSN_NBOOL_ATTRS
#define CGEN_INSN_NBOOL_ATTRS 1
#endif
  const CGEN_ATTR_TYPE (CGEN_INSN_NBOOL_ATTRS) attrs;
#define CGEN_INSN_ATTRS(insn) (&(insn)->attrs)
/* Return value of attribute ATTR in INSN.  */
#define CGEN_INSN_ATTR(insn, attr) \
CGEN_ATTR_VALUE (insn, CGEN_INSN_ATTRS (insn), attr)
};

/* Return non-zero if INSN is the "invalid" insn marker.  */
#define CGEN_INSN_INVALID_P(insn) (CGEN_INSN_MNEMONIC (insn) == 0)

/* Instruction lists.
   This is used for adding new entries and for creating the hash lists.  */

typedef struct cgen_insn_list
{
  struct cgen_insn_list * next;
  const CGEN_INSN * insn;
} CGEN_INSN_LIST;

/* The table of instructions.  */

typedef struct
{
  /* Pointer to initial [compiled in] entries.  */
  const CGEN_INSN * init_entries;
  
  /* Size of an entry (since the attribute member is variable sized).  */
  unsigned int entry_size;
  
  /* Number of entries in `init_entries', including trailing NULL entry.  */
  unsigned int num_init_entries;
  
  /* Values added at runtime.  */
  CGEN_INSN_LIST * new_entries;
} CGEN_INSN_TABLE;

/* ??? This is currently used by the simulator.  */
extern const CGEN_INSN CGEN_SYM (insn_table_entries)[];

/* Return number of instructions.  This includes any added at runtime.  */

extern int cgen_insn_count PARAMS ((CGEN_OPCODE_DESC));
extern int cgen_macro_insn_count PARAMS ((CGEN_OPCODE_DESC));

/* Macro instructions.
   Macro insns aren't real insns, they map to one or more real insns.
   E.g. An architecture's "nop" insn may actually be an "mv r0,r0" or
   some such.

   Macro insns can expand to nothing (e.g. a nop that is optimized away).
   This is useful in multi-insn macros that build a constant in a register.
   Of course this isn't the default behaviour and must be explicitly enabled.

   Assembly of macro-insns is relatively straightforward.  Disassembly isn't.
   However, disassembly of at least some kinds of macro insns is important
   in order that the disassembled code preserve the readability of the original
   insn.  What is attempted here is to disassemble all "simple" macro-insns,
   where "simple" is currently defined to mean "expands to one real insn".

   Simple macro-insns are handled specially.  They are emitted as ALIAS's
   of real insns.  This simplifies their handling since there's usually more
   of them than any other kind of macro-insn, and proper disassembly of them
   falls out for free.  */

/* For each macro-insn there may be multiple expansion possibilities,
   depending on the arguments.  This structure is accessed via the `data'
   member of CGEN_INSN.  */

typedef struct cgen_minsn_expansion {
  /* Function to do the expansion.
     If the expansion fails (e.g. "no match") NULL is returned.
     Space for the expansion is obtained with malloc.
     It is up to the caller to free it.  */
  const char * (* fn) PARAMS ((const struct cgen_minsn_expansion *,
			       const char *, const char **, int *, CGEN_OPERAND **));
#define CGEN_MIEXPN_FN(ex) ((ex)->fn)

  /* Instruction(s) the macro expands to.
     The format of STR is defined by FN.
     It is typically the assembly code of the real insn, but it could also be
     the original Scheme expression or a tokenized form of it (with FN being
     an appropriate interpreter).  */
  const char * str;
#define CGEN_MIEXPN_STR(ex) ((ex)->str)
} CGEN_MINSN_EXPANSION;

/* Normal expander.
   When supported, this function will convert the input string to another
   string and the parser will be invoked recursively.  The output string
   may contain further macro invocations.  */

extern const char * cgen_expand_macro_insn
     PARAMS ((CGEN_OPCODE_DESC, const struct cgen_minsn_expansion *,
	      const char *, const char **, int *, CGEN_OPERAND **));

/* The assembler insn table is hashed based on some function of the mnemonic
   (the actually hashing done is up to the target, but we provide a few
   examples like the first letter or a function of the entire mnemonic).  */

#ifndef CGEN_ASM_HASH_P
#define CGEN_ASM_HASH_P(insn) 1
#endif

/* INSN is the CGEN_INSN entry when building the hash table and NULL
   when looking up the insn during assembly.  */
#ifndef CGEN_ASM_HASH
#define CGEN_ASM_HASH_SIZE 127
#ifdef CGEN_MNEMONIC_OPERANDS
#define CGEN_ASM_HASH(mnem) (*(unsigned char *) (mnem) % CGEN_ASM_HASH_SIZE)
#else
#define CGEN_ASM_HASH(mnem) (*(unsigned char *) (mnem) % CGEN_ASM_HASH_SIZE) /*FIXME*/
#endif
#endif

extern CGEN_INSN_LIST * cgen_asm_lookup_insn
     PARAMS ((CGEN_OPCODE_DESC, const char *));
#define CGEN_ASM_LOOKUP_INSN(od, string) cgen_asm_lookup_insn ((od), (string))
#define CGEN_ASM_NEXT_INSN(insn) ((insn)->next)

/* The disassembler insn table is hashed based on some function of machine
   instruction (the actually hashing done is up to the target).  */

#ifndef CGEN_DIS_HASH_P
#define CGEN_DIS_HASH_P(insn) 1
#endif

/* It doesn't make much sense to provide a default here,
   but while this is under development we do.
   BUFFER is a pointer to the bytes of the insn.
   VALUE is the first CGEN_BASE_INSN_SIZE bytes as an int in host order.  */
#ifndef CGEN_DIS_HASH
#define CGEN_DIS_HASH_SIZE 256
#define CGEN_DIS_HASH(buffer, value) (*(unsigned char *) (buffer))
#endif

extern CGEN_INSN_LIST * cgen_dis_lookup_insn
     PARAMS ((CGEN_OPCODE_DESC, const char *, unsigned long));
#define CGEN_DIS_LOOKUP_INSN(od, buf, value) cgen_dis_lookup_insn ((od), (buf), (value))
#define CGEN_DIS_NEXT_INSN(insn) ((insn)->next)

/* The opcode table.
   A copy of this is created when the opcode table is "opened".
   All global state information is recorded here.  */
/* ??? This is all low level implementation stuff here that might be better
   put in an internal file (much like the distinction between bfd.h and
   libbfd.h).  That is an extra complication that is left for later.  */

typedef struct cgen_opcode_table
{
  const CGEN_HW_ENTRY * hw_list;
#define CGEN_OPCODE_HW_LIST(od) ((od)->hw_list)

  const CGEN_OPERAND * operand_table;
#define CGEN_OPCODE_OPERAND_TABLE(od) ((od)->operand_table)

  CGEN_INSN_TABLE insn_table;
#define CGEN_OPCODE_INSN_TABLE(od) (& (od)->insn_table)

  /* Macro instructions are defined separately and are combined with real
     insns during hash table computation.  */
  CGEN_INSN_TABLE macro_insn_table;
#define CGEN_OPCODE_MACRO_INSN_TABLE(od) (& (od)->macro_insn_table)

  /* Return non-zero if insn should be added to hash table.  */
  int (* asm_hash_p) PARAMS ((const CGEN_INSN *));
#define CGEN_OPCODE_ASM_HASH_P(od) ((od)->asm_hash_p)

  /* Assembler hash function.  */
  unsigned int (* asm_hash) PARAMS ((const char *));
#define CGEN_OPCODE_ASM_HASH(od) ((od)->asm_hash)

  /* Number of entries in assembler hash table.  */
  unsigned int asm_hash_size;
#define CGEN_OPCODE_ASM_HASH_SIZE(od) ((od)->asm_hash_size)

  /* Return non-zero if insn should be added to hash table.  */
  int (* dis_hash_p) PARAMS ((const CGEN_INSN *));
#define CGEN_OPCODE_DIS_HASH_P(od) ((od)->dis_hash_p)

  /* Disassembler hash function.  */
  unsigned int (* dis_hash) PARAMS ((const char *, unsigned long));
#define CGEN_OPCODE_DIS_HASH(od) ((od)->dis_hash)

  /* Number of entries in disassembler hash table.  */
  unsigned int dis_hash_size;
#define CGEN_OPCODE_DIS_HASH_SIZE(od) ((od)->dis_hash_size)

  /* Operand parser callback.  */
  cgen_parse_operand_fn * parse_operand_fn;
#define CGEN_OPCODE_PARSE_OPERAND_FN(od) ((od)->parse_operand_fn)

  /* Current machine (a la BFD machine number).  */
  int mach;
#define CGEN_OPCODE_MACH(od) ((od)->mach)

  /* Current endian.  */
  enum cgen_endian endian;
#define CGEN_OPCODE_ENDIAN(od) ((od)->endian)

  /* Current insn endian.  */
  enum cgen_endian insn_endian;
#define CGEN_OPCODE_INSN_ENDIAN(od) ((od)->insn_endian)

  /* Assembler instruction hash table.  */
  CGEN_INSN_LIST **asm_hash_table;
#define CGEN_OPCODE_ASM_HASH_TABLE(od) ((od)->asm_hash_table)
  CGEN_INSN_LIST *asm_hash_table_entries;
#define CGEN_OPCODE_ASM_HASH_TABLE_ENTRIES(od) ((od)->asm_hash_table_entries)

  /* Disassembler instruction hash table.  */
  CGEN_INSN_LIST ** dis_hash_table;
#define CGEN_OPCODE_DIS_HASH_TABLE(od) ((od)->dis_hash_table)
  CGEN_INSN_LIST * dis_hash_table_entries;
#define CGEN_OPCODE_DIS_HASH_TABLE_ENTRIES(od) ((od)->dis_hash_table_entries)

} CGEN_OPCODE_TABLE;

/* Prototypes of major functions.  */
/* FIXME: Move all CGEN_SYM-defined functions into CGEN_OPCODE_DESC.  */

/* Open an opcode table for use.  */
extern CGEN_OPCODE_DESC CGEN_SYM (opcode_open)
     PARAMS ((int, enum cgen_endian));
/* Close it.  */
extern void CGEN_SYM (opcode_close) PARAMS ((CGEN_OPCODE_DESC));

/* `init_tables' must be called before `xxx_supported'.  */
extern void CGEN_SYM (init_tables) PARAMS ((int));

/* Initialize an opcode table for assembler or disassembler use.
   These must be called immediately after opcode_open.  */
extern void CGEN_SYM (init_asm) PARAMS ((CGEN_OPCODE_DESC));
extern void CGEN_SYM (init_dis) PARAMS ((CGEN_OPCODE_DESC));

/* Change the mach and/or endianness.  */
extern void cgen_set_cpu PARAMS ((CGEN_OPCODE_DESC, int, enum cgen_endian));

extern const CGEN_INSN * CGEN_SYM (assemble_insn)
     PARAMS ((CGEN_OPCODE_DESC, const char *, CGEN_FIELDS *,
	      CGEN_INSN_BYTES_PTR, char **));

extern const CGEN_KEYWORD CGEN_SYM (operand_mach);
extern int CGEN_SYM (get_mach) PARAMS ((const char *));

extern const CGEN_INSN * CGEN_SYM (lookup_insn)
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN *,
	      CGEN_INSN_BYTES, int, CGEN_FIELDS *, int));
extern void CGEN_SYM (get_insn_operands)
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN *,
	      const CGEN_FIELDS *, int *));
extern const CGEN_INSN * CGEN_SYM (lookup_get_insn_operands)
     PARAMS ((CGEN_OPCODE_DESC, const CGEN_INSN *,
	      CGEN_INSN_BYTES, int, int *));

/* Get/set fields in the CGEN_FIELDS struct.  */
int CGEN_SYM (get_int_operand)
     PARAMS ((int, const CGEN_FIELDS *));
void CGEN_SYM (set_int_operand)
     PARAMS ((int, CGEN_FIELDS *, int));
#ifdef BFD_VERSION /* Don't require bfd.h unnecessarily.  */
bfd_vma CGEN_SYM (get_vma_operand)
     PARAMS ((int, const CGEN_FIELDS *));
void CGEN_SYM (set_vma_operand)
     PARAMS ((int, CGEN_FIELDS *, bfd_vma));
#endif

extern const char * CGEN_SYM (parse_operand)
     PARAMS ((CGEN_OPCODE_DESC, int, const char **, CGEN_FIELDS *));

#ifdef BFD_VERSION /* Don't require bfd.h unnecessarily.  */
extern const char * CGEN_SYM (insert_operand)
     PARAMS ((CGEN_OPCODE_DESC, int, CGEN_FIELDS *, CGEN_INSN_BYTES_PTR,
	      bfd_vma));
extern int CGEN_SYM (extract_operand)
     PARAMS ((CGEN_OPCODE_DESC, int, CGEN_EXTRACT_INFO *, CGEN_INSN_BYTES,
	      CGEN_FIELDS *, bfd_vma));
#endif

/* Cover fns to bfd_get/set.  */
extern CGEN_INSN_INT cgen_get_insn_value
     PARAMS ((CGEN_OPCODE_DESC, unsigned char *, int));
extern void cgen_put_insn_value
     PARAMS ((CGEN_OPCODE_DESC, unsigned char *, int, CGEN_INSN_INT));

/* Read in a cpu description file.  */
extern const char * cgen_read_cpu_file
     PARAMS ((CGEN_OPCODE_DESC, const char * /*filename*/));

#endif /* CGEN_H */
