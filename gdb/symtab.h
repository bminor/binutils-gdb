/* Symbol table definitions for GDB.
   Copyright (C) 1986, 1989, 1991 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (SYMTAB_H)
#define SYMTAB_H 1
#include "obstack.h"

/* An obstack to hold objects that should be freed
   when we load a new symbol table.
   This includes the symbols made by dbxread
   and the types that are not permanent.  */

extern struct obstack *symbol_obstack;
extern struct obstack *psymbol_obstack;

/* Some definitions and declarations to go with use of obstacks.  */
#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free

/* Some macros for char-based bitfields.  */
#define B_SET(a,x) (a[x>>3] |= (1 << (x&7)))
#define B_CLR(a,x) (a[x>>3] &= ~(1 << (x&7)))
#define B_TST(a,x) (a[x>>3] & (1 << (x&7)))
#define B_TYPE		unsigned char
#define	B_BYTES(x)	( 1 + ((x)>>3) )
#define	B_CLRALL(a,x) bzero (a, B_BYTES(x))

/* gdb can know one or several symbol tables at the same time;
   the ultimate intent is to have one for each separately-compiled module.
   Each such symbol table is recorded by a struct symtab, and they
   are all chained together.  */

/* In addition, gdb can record any number of miscellaneous undebuggable
   functions' addresses.  In a system that appends _ to function names,
   the _'s are removed from the names stored in this table.  */

/* Actually, the misc function list is used to store *all* of the
   global symbols (text, data, bss, and abs).  It is sometimes used
   to figure out what symtabs to read in.  The "type" field is used
   occasionally.  Calling it the misc "function" vector is now a misnomer.

   The misc_info field is available for machine-specific information
   that can be cached along with a misc function vector entry.  The
   AMD 29000 tdep.c uses it to remember things it has decoded from the
   instructions in the function header, so it doesn't have to rederive
   the info constantly (over a serial line).  It is initialized to zero
   and stays that way until target-dependent code sets it.  */

enum misc_function_type {mf_unknown = 0, mf_text, mf_data, mf_bss, mf_abs};

struct misc_function
{
  char *name;
  CORE_ADDR address;
  char *misc_info;	/* Random pointer to misc info.  void * but for old C */
  enum misc_function_type type;
};

/* Address and length of the vector recording all misc function names/addresses.  */

struct misc_function *misc_function_vector;
int misc_function_count;

/* Different kinds of data types are distinguished by the `code' field.  */

enum type_code
{
  TYPE_CODE_UNDEF,		/* Not used; catches errors */
  TYPE_CODE_PTR,		/* Pointer type */
  TYPE_CODE_ARRAY,		/* Array type, lower bound zero */
  TYPE_CODE_STRUCT,		/* C struct or Pascal record */
  TYPE_CODE_UNION,		/* C union or Pascal variant part */
  TYPE_CODE_ENUM,		/* Enumeration type */
  TYPE_CODE_FUNC,		/* Function type */
  TYPE_CODE_INT,		/* Integer type */
  TYPE_CODE_FLT,		/* Floating type */
  TYPE_CODE_VOID,		/* Void type (values zero length) */
  TYPE_CODE_SET,		/* Pascal sets */
  TYPE_CODE_RANGE,		/* Range (integers within spec'd bounds) */
  TYPE_CODE_PASCAL_ARRAY,	/* Array with explicit type of index */
  TYPE_CODE_ERROR,              /* Unknown type */

  /* C++ */
  TYPE_CODE_MEMBER,		/* Member type */
  TYPE_CODE_METHOD,		/* Method type */
  TYPE_CODE_REF,		/* C++ Reference types */

  /* Modula-2 */
  TYPE_CODE_CHAR,		/* *real* character type */
  TYPE_CODE_BOOL,		/* Builtin Modula-2 BOOLEAN */
};

/* This appears in a type's flags word for an unsigned integer type.  */
#define TYPE_FLAG_UNSIGNED 1
/* This appears in a type's flags word
   if it is a (pointer to a|function returning a)* built in scalar type.
   These types are never freed.  */
#define TYPE_FLAG_PERM 4
/* This appears in a type's flags word if it is a stub type (eg. if
   someone referenced a type that wasn't definined in a source file
   via (struct sir_not_appearing_in_this_film *)).  */
#define TYPE_FLAG_STUB 8

struct type
{
  /* Code for kind of type */
  enum type_code code;
  /* Name of this type, or zero if none.
     This is used for printing only, except by poorly designed C++ code.
     Type names specified as input are defined by symbols.  */
  char *name;
  /* Length in bytes of storage for a value of this type */
  unsigned length;
  /* For a pointer type, describes the type of object pointed to.
     For an array type, describes the type of the elements.
     For a function or method type, describes the type of the value.
     For a range type, describes the type of the full range.
     Unused otherwise.  */
  struct type *target_type;

  /* Type that is a pointer to this type.
     Zero if no such pointer-to type is known yet.
     The debugger may add the address of such a type
     if it has to construct one later.  */ 
  struct type *pointer_type;
  /* C++: also need a reference type.  */
  struct type *reference_type;
  /* Type that is a function returning this type.
     Zero if no such function type is known here.
     The debugger may add the address of such a type
     if it has to construct one later.  */
  struct type *function_type;

  /* Flags about this type.  */
  short flags;
  /* Number of fields described for this type */
  short nfields;
  /* For structure and union types, a description of each field.
     For set and pascal array types, there is one "field",
     whose type is the domain type of the set or array.
     For range types, there are two "fields",
     the minimum and maximum values (both inclusive).
     For enum types, each possible value is described by one "field".

     Using a pointer to a separate array of fields
     allows all types to have the same size, which is useful
     because we can allocate the space for a type before
     we know what to put in it.  */
  struct field
    {
      /* Position of this field, counting in bits from start of
	 containing structure.  For a function type, this is the
	 position in the argument list of this argument.
	 For a range bound or enum value, this is the value itself.  */
      int bitpos;
      /* Size of this field, in bits, or zero if not packed.
	 For an unpacked field, the field's type's length
	 says how many bytes the field occupies.  */
      int bitsize;
      /* In a struct or enum type, type of this field.
	 In a function type, type of this argument.
	 In an array type, the domain-type of the array.  */
      struct type *type;
      /* Name of field, value or argument.
	 Zero for range bounds and array domains.  */
      char *name;
    } *fields;

  /* For types with virtual functions, VPTR_BASETYPE is the base class which
     defined the virtual function table pointer.  VPTR_FIELDNO is
     the field number of that pointer in the structure.

     For types that are pointer to member types, VPTR_BASETYPE
     is the type that this pointer is a member of.

     Unused otherwise.  */
  struct type *vptr_basetype;

  int vptr_fieldno;

  /* Slot to point to additional language-specific fields of this type.  */
  union
    {
      struct type **arg_types;
      struct cplus_struct_type *cplus_stuff;
    } type_specific;
};

/* C++ language-specific information for TYPE_CODE_STRUCT and TYPE_CODE_UNION
   nodes.  */
struct cplus_struct_type
{
  B_TYPE *virtual_field_bits; /* if base class is virtual */
  B_TYPE *private_field_bits;
  B_TYPE *protected_field_bits;

  /* Number of methods described for this type */
  short nfn_fields;
  /* Number of base classes this type derives from. */
  short n_baseclasses;

  /* Number of methods described for this type plus all the
     methods that it derives from.  */
  int nfn_fields_total;

  /* For classes, structures, and unions, a description of each field,
     which consists of an overloaded name, followed by the types of
     arguments that the method expects, and then the name after it
     has been renamed to make it distinct.  */
  struct fn_fieldlist
    {
      /* The overloaded name.  */
      char *name;
      /* The number of methods with this name.  */
      int length;
      /* The list of methods.  */
      struct fn_field
	{
	  /* The return value of the method */
	  struct type *type;
	  /* The argument list */
	  struct type **args;
	  /* The name after it has been processed */
	  char *physname;

	  /* For virtual functions.   */
	  /* First baseclass that defines this virtual function.   */
	  struct type *fcontext;
	  unsigned int is_const : 1;
	  unsigned int is_volatile : 1;
	  /* Index into that baseclass's virtual function table,
	     minus 2; else if static: VOFFSET_STATIC; else: 0.  */
	  unsigned voffset : 30;
#	  define VOFFSET_STATIC 1
	} *fn_fields;

      B_TYPE *private_fn_field_bits;
      B_TYPE *protected_fn_field_bits;

    } *fn_fieldlists;

  unsigned char via_protected;
  unsigned char via_public;
};

/* All of the name-scope contours of the program
   are represented by `struct block' objects.
   All of these objects are pointed to by the blockvector.

   Each block represents one name scope.
   Each lexical context has its own block.

   The first two blocks in the blockvector are special.
   The first one contains all the symbols defined in this compilation
   whose scope is the entire program linked together.
   The second one contains all the symbols whose scope is the
   entire compilation excluding other separate compilations.
   In C, these correspond to global symbols and static symbols.

   Each block records a range of core addresses for the code that
   is in the scope of the block.  The first two special blocks
   give, for the range of code, the entire range of code produced
   by the compilation that the symbol segment belongs to.

   The blocks appear in the blockvector
   in order of increasing starting-address,
   and, within that, in order of decreasing ending-address.

   This implies that within the body of one function
   the blocks appear in the order of a depth-first tree walk.  */

struct blockvector
{
  /* Number of blocks in the list.  */
  int nblocks;
  /* The blocks themselves.  */
  struct block *block[1];
};

/* Special block numbers */
#define GLOBAL_BLOCK	0
#define	STATIC_BLOCK	1
#define	FIRST_LOCAL_BLOCK	2

struct block
{
  /* Addresses in the executable code that are in this block.
     Note: in an unrelocated symbol segment in a file,
     these are always zero.  They can be filled in from the
     N_LBRAC and N_RBRAC symbols in the loader symbol table.  */
  CORE_ADDR startaddr, endaddr;
  /* The symbol that names this block,
     if the block is the body of a function;
     otherwise, zero.
     Note: In an unrelocated symbol segment in an object file,
     this field may be zero even when the block has a name.
     That is because the block is output before the name
     (since the name resides in a higher block).
     Since the symbol does point to the block (as its value),
     it is possible to find the block and set its name properly.  */
  struct symbol *function;
  /* The `struct block' for the containing block, or 0 if none.  */
  /* Note that in an unrelocated symbol segment in an object file
     this pointer may be zero when the correct value should be
     the second special block (for symbols whose scope is one compilation).
     This is because the compiler ouptuts the special blocks at the
     very end, after the other blocks.   */
  struct block *superblock;
  /* A flag indicating whether or not the fucntion corresponding
     to this block was compiled with gcc or not.  If there is no
     function corresponding to this block, this meaning of this flag
     is undefined.  (In practice it will be 1 if the block was created
     while processing a file compiled with gcc and 0 when not). */
  unsigned char gcc_compile_flag;
  /* Number of local symbols.  */
  int nsyms;
  /* The symbols.  */
  struct symbol *sym[1];
};

/* Represent one symbol name; a variable, constant, function or typedef.  */

/* Different name spaces for symbols.  Looking up a symbol specifies
   a namespace and ignores symbol definitions in other name spaces.

   VAR_NAMESPACE is the usual namespace.
   In C, this contains variables, function names, typedef names
   and enum type values.

   STRUCT_NAMESPACE is used in C to hold struct, union and enum type names.
   Thus, if `struct foo' is used in a C program,
   it produces a symbol named `foo' in the STRUCT_NAMESPACE.

   LABEL_NAMESPACE may be used for names of labels (for gotos);
   currently it is not used and labels are not recorded at all.  */

/* For a non-global symbol allocated statically,
   the correct core address cannot be determined by the compiler.
   The compiler puts an index number into the symbol's value field.
   This index number can be matched with the "desc" field of
   an entry in the loader symbol table.  */

enum namespace
{
  UNDEF_NAMESPACE, VAR_NAMESPACE, STRUCT_NAMESPACE, LABEL_NAMESPACE,
};

/* An address-class says where to find the value of a symbol.  */

enum address_class
{
  LOC_UNDEF,		/* Not used; catches errors */
  LOC_CONST,		/* Value is constant int SYMBOL_VALUE, host byteorder */
  LOC_STATIC,		/* Value is at fixed address SYMBOL_VALUE_ADDRESS */
  LOC_REGISTER,		/* Value is in register */
  LOC_ARG,		/* Value is at spec'd offset in arglist */
  LOC_REF_ARG,		/* Value address is at spec'd offset in arglist. */
  LOC_REGPARM,		/* Value is at spec'd offset in register window */
  LOC_LOCAL,		/* Value is at spec'd offset in stack frame */
  LOC_TYPEDEF,		/* Value not used; definition in SYMBOL_TYPE
			   Symbols in the namespace STRUCT_NAMESPACE
			   all have this class.  */
  LOC_LABEL,		/* Value is address SYMBOL_VALUE_ADDRESS in the code */
  LOC_BLOCK,		/* Value is address SYMBOL_VALUE_BLOCK of a
			   `struct block'.  Function names have this class. */
  LOC_CONST_BYTES,	/* Value is a constant byte-sequence pointed to by
			   SYMBOL_VALUE_ADDRESS, in target byte order.  */
  LOC_LOCAL_ARG,	/* Value is arg at spec'd offset in stack frame.
			   Differs from LOC_LOCAL in that symbol is an
			   argument; differs from LOC_ARG in that we find it
			   in the frame (FRAME_LOCALS_ADDRESS), not in the
			   arglist (FRAME_ARGS_ADDRESS).  Added for i960,
			   which passes args in regs then copies to frame.  */
};

struct symbol
{
  /* Symbol name */
  char *name;
  /* Name space code.  */
  enum namespace namespace;
  /* Address class */
  enum address_class class;
  /* Data type of value */
  struct type *type;

  /* Line number of definition.  */
  unsigned short line;
  
  /* constant value, or address if static, or register number,
     or offset in arguments, or offset in stack frame.  All of
     these are in host byte order (though what they point to might
     be in target byte order, e.g. LOC_CONST_BYTES).  */
  union
    {
      long value;		/* for LOC_CONST, LOC_REGISTER, LOC_ARG, 
				   LOC_REF_ARG, LOC_REGPARM, LOC_LOCAL */
      struct block *block;      /* for LOC_BLOCK */
      char *bytes;		/* for LOC_CONST_BYTES */
      CORE_ADDR address;	/* for LOC_STATIC, LOC_LABEL */
      struct symbol *chain;	/* for opaque typedef struct chain */
    }
  value;
};


/* A partial_symbol records the name, namespace, and address class of
   symbols whose types we have not parsed yet.  For functions, it also
   contains their memory address, so we can find them from a PC value.
   Each partial_symbol sits in a partial_symtab, all of which are chained
   on the partial_symtab_list and which points to the corresponding 
   normal symtab once the partial_symtab has been referenced.  */

struct partial_symbol
{
  /* Symbol name */
  char *name;
  /* Name space code.  */
  enum namespace namespace;
  /* Address class (for info_symbols) */
  enum address_class class;
  /* Value (only used for static functions currently).  Done this
     way so that we can use the struct symbol macros.
     Note that the address of a function is SYMBOL_VALUE_ADDRESS (pst)
     in a partial symbol table, but BLOCK_START (SYMBOL_BLOCK_VALUE (st))
     in a symbol table.  */
  union
    {
      long value;
      CORE_ADDR address;
    }
  value;
};

/* Source-file information.
   This describes the relation between source files and line numbers
   and addresses in the program text.  */

struct sourcevector
{
  int length;			/* Number of source files described */
  struct source *source[1];	/* Descriptions of the files */
};

/* Each item represents a line-->pc (or the reverse) mapping.  This is
   somewhat more wasteful of space than one might wish, but since only
   the files which are actually debugged are read in to core, we don't
   waste much space.

   Each item used to be an int; either minus a line number, or a
   program counter.  If it represents a line number, that is the line
   described by the next program counter value.  If it is positive, it
   is the program counter at which the code for the next line starts.  */

struct linetable_entry
{
  int line;
  CORE_ADDR pc;
};

struct linetable
{
  int nitems;
  struct linetable_entry item[1];
};

/* All the information on one source file.  */

struct source
{
  char *name;			/* Name of file */
  struct linetable contents;
};

/* Each source file is represented by a struct symtab. 
   These objects are chained through the `next' field.  */

struct symtab
  {
    /* Chain of all existing symtabs.  */
    struct symtab *next;
    /* List of all symbol scope blocks for this symtab.  */
    struct blockvector *blockvector;
    /* Table mapping core addresses to line numbers for this file.
       Can be NULL if none.  */
    struct linetable *linetable;
    /* Name of this source file.  */
    char *filename;
    /* Directory in which it was compiled, or NULL if we don't know.  */
    char *dirname;
    /* This component says how to free the data we point to:
       free_contents => do a tree walk and free each object.
       free_nothing => do nothing; some other symtab will free
         the data this one uses.
       free_linetable => free just the linetable.  */
    enum free_code {free_nothing, free_contents, free_linetable}
      free_code;
    /* Pointer to one block of storage to be freed, if nonzero.  */
    /* This is IN ADDITION to the action indicated by free_code.  */
    char *free_ptr;
    /* Total number of lines found in source file.  */
    int nlines;
    /* Array mapping line number to character position.  */
    int *line_charpos;
    /* Language of this source file.  */
    enum language language;
    /* String of version information.  May be zero.  */
    char *version;
    /* Full name of file as found by searching the source path.
       0 if not yet known.  */
    char *fullname;

    /* Object file from which this symbol information was read.  */
    struct objfile *objfile;
    /* Chain of all symtabs owned by that objfile.  */
    struct symtab *objfile_chain;

    /* Anything extra for this symtab.  This is for target machines
       with special debugging info of some sort (which cannot just
       be represented in a normal symtab).  */
#if defined (EXTRA_SYMTAB_INFO)
    EXTRA_SYMTAB_INFO
#endif
  };

/* Each source file that has not been fully read in is represented by
   a partial_symtab.  This contains the information on where in the
   executable the debugging symbols for a specific file are, and a
   list of names of global symbols which are located in this file.
   They are all chained on partial_symtab_list.

   Even after the source file has been read into a symtab, the
   partial_symtab remains around.  They are allocated on an obstack,
   psymbol_obstack.  FIXME, this is bad for dynamic linking or VxWorks-
   style execution of a bunch of .o's.  */
struct partial_symtab
{
  /* Chain of all existing partial symtabs.  */
  struct partial_symtab *next;
  /* Name of the source file which this partial_symtab defines */
  char *filename;

  /* Information about the object file from which symbols should be read.  */
  struct objfile *objfile;
  /* Chain of psymtabs owned by this objfile */
  struct partial_symtab *objfile_chain;

  /* Address relative to which the symbols in this file are.  Need to
     relocate by this amount when reading in symbols from the symbol
     file.  */
  CORE_ADDR addr;
  /* Range of text addresses covered by this file; texthigh is the
     beginning of the next section. */
  CORE_ADDR textlow, texthigh;
  /* Array of pointers to all of the partial_symtab's which this one
     depends on.  Since this array can only be set to previous or
     the current (?) psymtab, this dependency tree is guaranteed not
     to have any loops. */
  struct partial_symtab **dependencies;
  int number_of_dependencies;
  /* Global symbol list.  This list will be sorted after readin to
     improve access.  Binary search will be the usual method of
     finding a symbol within it. globals_offset is an integer offset
     within global_psymbols[].  */
  int globals_offset, n_global_syms;
  /* Static symbol list.  This list will *not* be sorted after readin;
     to find a symbol in it, exhaustive search must be used.  This is
     reasonable because searches through this list will eventually
     lead to either the read in of a files symbols for real (assumed
     to take a *lot* of time; check) or an error (and we don't care
     how long errors take).  This is an offset and size within
     static_psymbols[].  */
  int statics_offset, n_static_syms;
  /* Pointer to symtab eventually allocated for this source file, 0 if
     !readin or if we haven't looked for the symtab after it was readin.  */
  struct symtab *symtab;
  /* Pointer to function which will read in the symtab corresponding to
     this psymtab.  */
  void (*read_symtab) ();
  /* Information that lets read_symtab() locate the part of the symbol table
     that this psymtab corresponds to.  This information is private to the
     format-dependent symbol reading routines.  For further detail examine
     the various symbol reading modules.  Should really be (void *) but is
     (char *) as with other such gdb variables.  (FIXME) */
  char *read_symtab_private;
  /* Non-zero if the symtab corresponding to this psymtab has been
     readin */
  unsigned char readin;
};

/* A fast way to get from a psymtab to its symtab (after the first time).  */
#define	PSYMTAB_TO_SYMTAB(pst)  ((pst)->symtab? 		\
				 (pst)->symtab: 		\
				 psymtab_to_symtab (pst) )

/* This is the list of struct symtab's that gdb considers current.  */

struct symtab *symtab_list;

/* This is the list of struct partial_symtab's that gdb may need to access */

struct partial_symtab *partial_symtab_list;

/* This symtab variable specifies the current file for printing source lines */

struct symtab *current_source_symtab;

/* This is the next line to print for listing source lines.  */

int current_source_line;

#define BLOCKVECTOR(symtab) (symtab)->blockvector

#define LINETABLE(symtab) (symtab)->linetable

/* Macros normally used to access components of symbol table structures.  */

#define BLOCKVECTOR_NBLOCKS(blocklist) (blocklist)->nblocks
#define BLOCKVECTOR_BLOCK(blocklist,n) (blocklist)->block[n]

#define BLOCK_START(bl) (bl)->startaddr
#define BLOCK_END(bl) (bl)->endaddr
#define BLOCK_NSYMS(bl) (bl)->nsyms
#define BLOCK_SYM(bl, n) (bl)->sym[n]
#define BLOCK_FUNCTION(bl) (bl)->function
#define BLOCK_SUPERBLOCK(bl) (bl)->superblock
#define BLOCK_GCC_COMPILED(bl) (bl)->gcc_compile_flag

/* Nonzero if symbols of block BL should be sorted alphabetically.  */
#define BLOCK_SHOULD_SORT(bl) ((bl)->nsyms >= 40)

#define SYMBOL_NAME(symbol) (symbol)->name
#define SYMBOL_NAMESPACE(symbol) (symbol)->namespace
#define SYMBOL_CLASS(symbol) (symbol)->class
#define SYMBOL_VALUE(symbol) (symbol)->value.value
#define SYMBOL_VALUE_ADDRESS(symbol) (symbol)->value.address
#define SYMBOL_VALUE_BYTES(symbol) (symbol)->value.bytes
#define SYMBOL_BLOCK_VALUE(symbol) (symbol)->value.block
#define SYMBOL_VALUE_CHAIN(symbol) (symbol)->value.chain
#define SYMBOL_TYPE(symbol) (symbol)->type
#define SYMBOL_LINE(symbol) (symbol)->line

#define TYPE_NAME(thistype) (thistype)->name
#define TYPE_TARGET_TYPE(thistype) (thistype)->target_type
#define TYPE_POINTER_TYPE(thistype) (thistype)->pointer_type
#define TYPE_REFERENCE_TYPE(thistype) (thistype)->reference_type
#define TYPE_FUNCTION_TYPE(thistype) (thistype)->function_type
#define TYPE_LENGTH(thistype) (thistype)->length
#define TYPE_FLAGS(thistype) (thistype)->flags
#define TYPE_UNSIGNED(thistype) ((thistype)->flags & TYPE_FLAG_UNSIGNED)
#define TYPE_CODE(thistype) (thistype)->code
#define TYPE_NFIELDS(thistype) (thistype)->nfields
#define TYPE_FIELDS(thistype) (thistype)->fields
/* C++ */
#define TYPE_VPTR_BASETYPE(thistype) (thistype)->vptr_basetype
#define TYPE_DOMAIN_TYPE(thistype) (thistype)->vptr_basetype
#define TYPE_VPTR_FIELDNO(thistype) (thistype)->vptr_fieldno
#define TYPE_FN_FIELDS(thistype) TYPE_CPLUS_SPECIFIC(thistype)->fn_fields
#define TYPE_NFN_FIELDS(thistype) TYPE_CPLUS_SPECIFIC(thistype)->nfn_fields
#define TYPE_NFN_FIELDS_TOTAL(thistype) TYPE_CPLUS_SPECIFIC(thistype)->nfn_fields_total
#define TYPE_ARG_TYPES(thistype) (thistype)->type_specific.arg_types
#define TYPE_CPLUS_SPECIFIC(thistype) (thistype)->type_specific.cplus_stuff
#define TYPE_BASECLASS(thistype,index) (thistype)->fields[index].type
#define TYPE_N_BASECLASSES(thistype) TYPE_CPLUS_SPECIFIC(thistype)->n_baseclasses
#define TYPE_BASECLASS_NAME(thistype,index) (thistype)->fields[index].name
#define TYPE_BASECLASS_BITPOS(thistype,index) (thistype)->fields[index].bitpos
#define BASETYPE_VIA_PUBLIC(thistype, index) (!TYPE_FIELD_PRIVATE(thistype, index))
#define BASETYPE_VIA_VIRTUAL(thistype, index) \
  B_TST(TYPE_CPLUS_SPECIFIC(thistype)->virtual_field_bits, (index))

#define TYPE_FIELD(thistype, n) (thistype)->fields[n]
#define TYPE_FIELD_TYPE(thistype, n) (thistype)->fields[n].type
#define TYPE_FIELD_NAME(thistype, n) (thistype)->fields[n].name
#define TYPE_FIELD_VALUE(thistype, n) (* (int*) &(thistype)->fields[n].type)
#define TYPE_FIELD_BITPOS(thistype, n) (thistype)->fields[n].bitpos
#define TYPE_FIELD_BITSIZE(thistype, n) (thistype)->fields[n].bitsize
#define TYPE_FIELD_PACKED(thistype, n) (thistype)->fields[n].bitsize

#define TYPE_FIELD_PRIVATE_BITS(thistype) \
  TYPE_CPLUS_SPECIFIC(thistype)->private_field_bits
#define TYPE_FIELD_PROTECTED_BITS(thistype) \
  TYPE_CPLUS_SPECIFIC(thistype)->protected_field_bits
#define TYPE_FIELD_VIRTUAL_BITS(thistype) \
  TYPE_CPLUS_SPECIFIC(thistype)->virtual_field_bits
#define SET_TYPE_FIELD_PRIVATE(thistype, n) \
  B_SET (TYPE_CPLUS_SPECIFIC(thistype)->private_field_bits, (n))
#define SET_TYPE_FIELD_PROTECTED(thistype, n) \
  B_SET (TYPE_CPLUS_SPECIFIC(thistype)->protected_field_bits, (n))
#define SET_TYPE_FIELD_VIRTUAL(thistype, n) \
  B_SET (TYPE_CPLUS_SPECIFIC(thistype)->virtual_field_bits, (n))
#define TYPE_FIELD_PRIVATE(thistype, n) \
  B_TST(TYPE_CPLUS_SPECIFIC(thistype)->private_field_bits, (n))
#define TYPE_FIELD_PROTECTED(thistype, n) \
B_TST(TYPE_CPLUS_SPECIFIC(thistype)->protected_field_bits, (n))
#define TYPE_FIELD_VIRTUAL(thistype, n) \
       B_TST(TYPE_CPLUS_SPECIFIC(thistype)->virtual_field_bits, (n))

#define TYPE_FIELD_STATIC(thistype, n) ((thistype)->fields[n].bitpos == -1)
#define TYPE_FIELD_STATIC_PHYSNAME(thistype, n) ((char *)(thistype)->fields[n].bitsize)

#define TYPE_FN_FIELDLISTS(thistype) TYPE_CPLUS_SPECIFIC(thistype)->fn_fieldlists
#define TYPE_FN_FIELDLIST(thistype, n) TYPE_CPLUS_SPECIFIC(thistype)->fn_fieldlists[n]
#define TYPE_FN_FIELDLIST1(thistype, n) TYPE_CPLUS_SPECIFIC(thistype)->fn_fieldlists[n].fn_fields
#define TYPE_FN_FIELDLIST_NAME(thistype, n) TYPE_CPLUS_SPECIFIC(thistype)->fn_fieldlists[n].name
#define TYPE_FN_FIELDLIST_LENGTH(thistype, n) TYPE_CPLUS_SPECIFIC(thistype)->fn_fieldlists[n].length

#define TYPE_FN_FIELD(thisfn, n) (thisfn)[n]
#define TYPE_FN_FIELD_NAME(thisfn, n) (thisfn)[n].name
#define TYPE_FN_FIELD_TYPE(thisfn, n) (thisfn)[n].type
#define TYPE_FN_FIELD_ARGS(thisfn, n) TYPE_ARG_TYPES ((thisfn)[n].type)
#define TYPE_FN_FIELD_PHYSNAME(thisfn, n) (thisfn)[n].physname
#define TYPE_FN_FIELD_VIRTUAL_P(thisfn, n) ((thisfn)[n].voffset > 1)
#define TYPE_FN_FIELD_STATIC_P(thisfn, n) ((thisfn)[n].voffset == VOFFSET_STATIC)
#define TYPE_FN_FIELD_VOFFSET(thisfn, n) ((thisfn)[n].voffset-2)
#define TYPE_FN_FIELD_FCONTEXT(thisfn, n) ((thisfn)[n].fcontext)

#define TYPE_FN_PRIVATE_BITS(thisfn) (thisfn).private_fn_field_bits
#define TYPE_FN_PROTECTED_BITS(thisfn) (thisfn).protected_fn_field_bits
#define SET_TYPE_FN_PRIVATE(thisfn, n) B_SET ((thisfn).private_fn_field_bits, n)
#define SET_TYPE_FN_PROTECTED(thisfn, n) B_SET ((thisfn).protected_fn_field_bits, n)
#define TYPE_FN_PRIVATE(thisfn, n) B_TST ((thisfn).private_fn_field_bits, n)
#define TYPE_FN_PROTECTED(thisfn, n) B_TST ((thisfn).protected_fn_field_bits, n)

/* The virtual function table is now an array of structures
   which have the form { int16 offset, delta; void *pfn; }. 

   In normal virtual function tables, OFFSET is unused.
   DELTA is the amount which is added to the apparent object's base
   address in order to point to the actual object to which the
   virtual function should be applied.
   PFN is a pointer to the virtual function.  */
  
#define VTBL_FNADDR_OFFSET 2

/* Macro that yields non-zero value iff NAME is the prefix
   for C++ operator names.  If you leave out the parenthesis
   here you will lose!

   Currently 'o' 'p' CPLUS_MARKER is used for both the symbol in the
   symbol-file and the names in gdb's symbol table.  */
#define OPNAME_PREFIX_P(NAME) ((NAME)[0] == 'o' && (NAME)[1] == 'p' \
			       && (NAME)[2] == CPLUS_MARKER)

#define VTBL_PREFIX_P(NAME) ((NAME)[3] == CPLUS_MARKER	\
			     && !strncmp ((NAME), "_vt", 3))

/* Functions that work on the objects described above */

extern struct symtab *lookup_symtab ();
extern struct symbol *lookup_symbol ();
extern struct symbol *lookup_block_symbol ();
extern int lookup_misc_func ();
extern void check_stub_type ();
extern void check_stub_method ();
extern struct type *lookup_primitive_typename ();
extern struct type *lookup_typename ();
extern struct type *lookup_unsigned_typename ();
extern struct type *lookup_struct ();
extern struct type *lookup_union ();
extern struct type *lookup_enum ();
extern struct type *lookup_struct_elt_type ();
extern struct type *lookup_pointer_type ();
extern struct type *lookup_function_type ();
extern struct type *create_array_type ();
extern struct symbol *block_function ();
extern struct symbol *find_pc_function ();
extern int find_pc_partial_function ();
extern void clear_pc_function_cache ();
extern struct partial_symtab *lookup_partial_symtab ();
extern struct partial_symtab *find_pc_psymtab ();
extern struct symtab *find_pc_symtab ();
extern struct partial_symbol *find_pc_psymbol ();
extern int find_pc_misc_function ();
extern int find_pc_line_pc_range ();
extern char *type_name_no_tag ();
extern int contained_in();

/* C++ stuff.  */
extern struct type *lookup_template_type ();
extern struct type *lookup_reference_type ();
extern struct type *lookup_member_type ();
extern void smash_to_method_type ();
void smash_to_member_type (
#ifdef __STDC__
			   struct type *, struct type *, struct type *
#endif
			   );
extern struct type *allocate_stub_method ();
/* end of C++ stuff.  */

extern void reread_symbols ();

extern struct type *builtin_type_void;
extern struct type *builtin_type_char;
extern struct type *builtin_type_short;
extern struct type *builtin_type_int;
extern struct type *builtin_type_long;
extern struct type *builtin_type_unsigned_char;
extern struct type *builtin_type_unsigned_short;
extern struct type *builtin_type_unsigned_int;
extern struct type *builtin_type_unsigned_long;
extern struct type *builtin_type_float;
extern struct type *builtin_type_double;
extern struct type *builtin_type_long_double;
extern struct type *builtin_type_complex;
extern struct type *builtin_type_double_complex;
/* This type represents a type that was unrecognized in symbol
   read-in.  */
extern struct type *builtin_type_error;

extern struct type *builtin_type_long_long;
extern struct type *builtin_type_unsigned_long_long;

/* Modula-2 types */
extern struct type *builtin_type_m2_char;
extern struct type *builtin_type_m2_int;
extern struct type *builtin_type_m2_card;
extern struct type *builtin_type_m2_real;
extern struct type *builtin_type_m2_bool;

/* LONG_LONG is defined if the host has "long long".  */
#ifdef LONG_LONG
#define BUILTIN_TYPE_LONGEST builtin_type_long_long
#define BUILTIN_TYPE_UNSIGNED_LONGEST builtin_type_unsigned_long_long
/* This should not be a typedef, because "unsigned LONGEST" needs
   to work.  */
#define LONGEST long long

#else /* not LONG_LONG.  */

#define BUILTIN_TYPE_LONGEST builtin_type_long
#define BUILTIN_TYPE_UNSIGNED_LONGEST builtin_type_unsigned_long
#define LONGEST long

#endif /* not LONG_LONG.  */

struct symtab_and_line
{
  struct symtab *symtab;
  int line;
  CORE_ADDR pc;
  CORE_ADDR end;
};

struct symtabs_and_lines
{
  struct symtab_and_line *sals;
  int nelts;
};

/* Given a pc value, return line number it is in.
   Second arg nonzero means if pc is on the boundary
   use the previous statement's line number.  */

struct symtab_and_line find_pc_line ();

/* Given a symtab and line number, return the pc there.  */
extern CORE_ADDR find_line_pc ();
extern int find_line_pc_range ();

/* Given a string, return the line specified by it.
   For commands like "list" and "breakpoint".  */

struct symtabs_and_lines decode_line_spec ();
struct symtabs_and_lines decode_line_spec_1 ();
struct symtabs_and_lines decode_line_1 ();

/* Symmisc.c */
void free_symtab ();

/* Symbol-reading stuff in symfile.c and solib.c.  */
struct symtab *psymtab_to_symtab ();
void clear_solib ();
void symbol_file_add ();

/* source.c */
int identify_source_line ();
void print_source_lines ();
void forget_cached_source_info (
#ifdef __STDC__
				void
#endif
				);
void select_source_symtab (
#ifdef __STDC__
			   struct symtab *
#endif
			   );

char **make_symbol_completion_list ();

/* Maximum and minimum values of built-in types */
#define	MAX_OF_TYPE(t)	\
   TYPE_UNSIGNED(t) ? UMAX_OF_SIZE(TYPE_LENGTH(t)) : MAX_OF_SIZE(TYPE_LENGTH(t))

#define MIN_OF_TYPE(t)	\
   TYPE_UNSIGNED(t) ? UMIN_OF_SIZE(TYPE_LENGTH(t)) : MIN_OF_SIZE(TYPE_LENGTH(t))

#endif /* symtab.h not already included.  */
