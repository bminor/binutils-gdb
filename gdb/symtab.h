/* Symbol table definitions for GDB.
   Copyright (C) 1986 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

/* An obstack to hold objects that should be freed
   when we load a new symbol table.
   This includes the symbols made by dbxread
   and the types that are not permanent.  */

extern struct obstack *symbol_obstack;

/* Some definitions and declarations to go with use of obstacks.  */
#define obstack_chunk_alloc xmalloc
#define obstack_chunk_free free
extern char *xmalloc ();
extern void free ();

/* gdb can know one or several symbol tables at the same time;
   the ultimate intent is to have one for each separately-compiled module.
   Each such symbol table is recorded by a struct symtab, and they
   are all chained together.  */

/* In addition, gdb can record any number of miscellaneous undebuggable
   functions' addresses.  In a system that appends _ to function names,
   the _'s are removed from the names stored in this table.  */

struct misc_function
{
  char *name;
  CORE_ADDR address;
};

/* Address and length of the vector recording all misc function names/addresses.  */

struct misc_function *misc_function_vector;
int misc_function_count;

#include "symseg.h"

/* Each source file is represented by a struct symtab.
   These objects are chained through the `next' field.  */

struct symtab
  {
    /* Chain of all existing symtabs.  */
    struct symtab *next;
    /* List of all symbol scope blocks for this symtab.  */
    struct blockvector *blockvector;
    /* Table mapping core addresses to line numbers for this file.  */
    struct linetable *linetable;
    /* Vector containing all types defined for this symtab.  */
    struct typevector *typevector;
    /* Name of this source file.  */
    char *filename;
    /* This component says how to free the data we point to:
       free_contents => do a tree walk and free each object.
       free_nothing => do nothing; some other symtab will free
         the data this one uses.
       free_linetable => free just the linetable.  */
    enum free_code {free_nothing, free_contents, free_linetable}
      free_code;
    /* Pointer to one block storage to be freed, if nonzero.  */
    char *free_ptr;
    /* Total number of lines found in source file.  */
    int nlines;
    /* Array mapping line number to character position.  */
    int *line_charpos;
    /* Language of this source file.  */
    enum language language;
    /* String of version information.  May be zero.  */
    char *version;
    /* String of compilation information.  May be zero.  */
    char *compilation;
    /* Offset within loader symbol table
       of first local symbol for this file.  */
    int ldsymoff;
    /* Full name of file as found by searching the source path.
       0 if not yet known.  */
    char *fullname;
  };

/* This is the list of struct symtab's that gdb considers current.  */

struct symtab *symtab_list;

/* This symtab variable specifies the current file for printing source lines */

struct symtab *current_source_symtab;

/* This is the next line to print for listing source lines.  */

int current_source_line;

#define BLOCKLIST(symtab) (symtab)->blockvector
#define BLOCKVECTOR(symtab) (symtab)->blockvector

#define TYPEVECTOR(symtab) (symtab)->typevector

#define LINELIST(symtab) (symtab)->linetable
#define LINETABLE(symtab) (symtab)->linetable

/* Macros normally used to access components of symbol table structures.  */

#define BLOCKLIST_NBLOCKS(blocklist) (blocklist)->nblocks
#define BLOCKLIST_BLOCK(blocklist,n) (blocklist)->block[n]
#define BLOCKVECTOR_NBLOCKS(blocklist) (blocklist)->nblocks
#define BLOCKVECTOR_BLOCK(blocklist,n) (blocklist)->block[n]

#define TYPEVECTOR_NTYPES(typelist) (typelist)->length
#define TYPEVECTOR_TYPE(typelist,n) (typelist)->type[n]

#define BLOCK_START(bl) (bl)->startaddr
#define BLOCK_END(bl) (bl)->endaddr
#define BLOCK_NSYMS(bl) (bl)->nsyms
#define BLOCK_SYM(bl, n) (bl)->sym[n]
#define BLOCK_FUNCTION(bl) (bl)->function
#define BLOCK_SUPERBLOCK(bl) (bl)->superblock

/* Nonzero if symbols of block BL should be sorted alphabetically.  */
#define BLOCK_SHOULD_SORT(bl) ((bl)->nsyms >= 40)

#define SYMBOL_NAME(symbol) (symbol)->name
#define SYMBOL_NAMESPACE(symbol) (symbol)->namespace
#define SYMBOL_CLASS(symbol) (symbol)->class
#define SYMBOL_VALUE(symbol) (symbol)->value.value
#define SYMBOL_VALUE_BYTES(symbol) (symbol)->value.bytes
#define SYMBOL_BLOCK_VALUE(symbol) (symbol)->value.block
#define SYMBOL_TYPE(symbol) (symbol)->type

/* This appears in a type's flags word
   if it is a (pointer to a|function returning a)* built in scalar type.
   These types are never freed.  */
#define TYPE_FLAG_PERM 4

#define TYPE_NAME(thistype) (thistype)->name
#define TYPE_TARGET_TYPE(thistype) (thistype)->target_type
#define TYPE_POINTER_TYPE(thistype) (thistype)->pointer_type
#define TYPE_FUNCTION_TYPE(thistype) (thistype)->function_type
#define TYPE_LENGTH(thistype) (thistype)->length
#define TYPE_FLAGS(thistype) (thistype)->flags
#define TYPE_UNSIGNED(thistype) ((thistype)->flags & TYPE_FLAG_UNSIGNED)
#define TYPE_CODE(thistype) (thistype)->code
#define TYPE_NFIELDS(thistype) (thistype)->nfields
#define TYPE_FIELDS(thistype) (thistype)->fields

#define TYPE_FIELD(thistype, n) (thistype)->fields[n]
#define TYPE_FIELD_TYPE(thistype, n) (thistype)->fields[n].type
#define TYPE_FIELD_NAME(thistype, n) (thistype)->fields[n].name
#define TYPE_FIELD_VALUE(thistype, n) (* (int*) &(thistype)->fields[n].type)
#define TYPE_FIELD_BITPOS(thistype, n) (thistype)->fields[n].bitpos
#define TYPE_FIELD_BITSIZE(thistype, n) (thistype)->fields[n].bitsize
#define TYPE_FIELD_PACKED(thistype, n) (thistype)->fields[n].bitsize

/* Functions that work on the objects described above */

extern struct symtab *lookup_symtab ();
extern struct symbol *lookup_symbol ();
extern struct type *lookup_typename ();
extern struct type *lookup_unsigned_typename ();
extern struct type *lookup_struct ();
extern struct type *lookup_union ();
extern struct type *lookup_enum ();
extern struct type *lookup_pointer_type ();
extern struct type *lookup_function_type ();
extern struct symbol *block_function ();
extern struct symbol *find_pc_function ();
extern int find_pc_misc_function ();

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

struct symtab_and_line
{
  struct symtab *symtab;
  int line;
  CORE_ADDR pc;
  CORE_ADDR end;
};

/* Given a pc value, return line number it is in.
   Second arg nonzero means if pc is on the boundary
   use the previous statement's line number.  */

struct symtab_and_line find_pc_line ();

/* Given a string, return the line specified by it.
   For commands like "list" and "breakpoint".  */

struct symtab_and_line decode_line_spec ();
struct symtab_and_line decode_line_1 ();
