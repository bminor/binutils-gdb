/* DWARF debugging format support for GDB.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support, portions based on dbxread.c,
   mipsread.c, coffread.c, and dwarfread.c from a Data General SVR4 gdb port.

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

/*

FIXME: Figure out how to get the frame pointer register number in the
execution environment of the target.  Remove R_FP kludge

FIXME: Add generation of dependencies list to partial symtab code.

FIXME: Currently we ignore host/target byte ordering and integer size
differences.  Should remap data from external form to an internal form
before trying to use it.

FIXME: Resolve minor differences between what information we put in the
partial symbol table and what dbxread puts in.  For example, we don't yet
put enum constants there.  And dbxread seems to invent a lot of typedefs
we never see.  Use the new printpsym command to see the partial symbol table
contents.

FIXME: Change forward declarations of static functions to allow for compilers
without prototypes.

FIXME: Figure out a better way to tell gdb (all the debug reading routines)
the names of the gccX_compiled flags.

FIXME: Figure out a better way to tell gdb about the name of the function
contain the user's entry point (I.E. main())

FIXME: The current DWARF specification has a very strong bias towards
machines with 32-bit integers, as it assumes that many attributes of the
program (such as an address) will fit in such an integer.  There are many
references in the spec to things that are 2, 4, or 8 bytes long.  Given that
we will probably run into problems on machines where some of these assumptions
are invalid (64-bit ints for example), we don't bother at this time to try to
make this code more flexible and just use shorts, ints, and longs (and their
sizes) where it seems appropriate.  I.E. we use a short int to hold DWARF
tags, and assume that the tag size in the file is the same as sizeof(short).

FIXME: Figure out how to get the name of the symbol indicating that a module
has been compiled with gcc (gcc_compiledXX) in a more portable way than
hardcoding it into the object file readers.

FIXME: See other FIXME's and "ifdef 0" scattered throughout the code for
other things to work on, if you get bored. :-)

*/

#include <stdio.h>
#include <varargs.h>
#include <fcntl.h>

#include "defs.h"
#include "param.h"
#include "bfd.h"
#include "symtab.h"
#include "symfile.h"
#include "dwarf.h"
#include "ansidecl.h"

#ifdef MAINTENANCE	/* Define to 1 to compile in some maintenance stuff */
#define SQUAWK(stuff) dwarfwarn stuff
#else
#define SQUAWK(stuff)
#endif

#ifndef R_FP		/* FIXME */
#define R_FP 14		/* Kludge to get frame pointer register number */
#endif

typedef unsigned int DIEREF;	/* Reference to a DIE */

#define GCC_COMPILED_FLAG_SYMBOL "gcc_compiled%"	/* FIXME */
#define GCC2_COMPILED_FLAG_SYMBOL "gcc2_compiled%"	/* FIXME */

#define STREQ(a,b)		(strcmp(a,b)==0)

extern CORE_ADDR startup_file_start;	/* From blockframe.c */
extern CORE_ADDR startup_file_end;	/* From blockframe.c */
extern CORE_ADDR entry_scope_lowpc;	/* From blockframe.c */
extern CORE_ADDR entry_scope_highpc;	/* From blockframc.c */
extern CORE_ADDR main_scope_lowpc;	/* From blockframe.c */
extern CORE_ADDR main_scope_highpc;	/* From blockframc.c */
extern int info_verbose;		/* From main.c; nonzero => verbose */


/* The DWARF debugging information consists of two major pieces,
   one is a block of DWARF Information Entries (DIE's) and the other
   is a line number table.  The "struct dieinfo" structure contains
   the information for a single DIE, the one currently being processed.

   In order to make it easier to randomly access the attribute fields
   of the current DIE, which are specifically unordered within the DIE
   each DIE is scanned and an instance of the "struct dieinfo"
   structure is initialized.

   Initialization is done in two levels.  The first, done by basicdieinfo(),
   just initializes those fields that are vital to deciding whether or not
   to use this DIE, how to skip past it, etc.  The second, done by the
   function completedieinfo(), fills in the rest of the information.

   Attributes which have block forms are not interpreted at the time
   the DIE is scanned, instead we just save pointers to the start
   of their value fields.

   Some fields have a flag <name>_p that is set when the value of the
   field is valid (I.E. we found a matching attribute in the DIE).  Since
   we may want to test for the presence of some attributes in the DIE,
   such as AT_is_external, without restricting the values of the field,
   we need someway to note that we found such an attribute.
   
 */
   
typedef char BLOCK;

struct dieinfo {
  char *	die;			/* Pointer to the raw DIE data */
  long		dielength;		/* Length of the raw DIE data */
  DIEREF	dieref;			/* Offset of this DIE */
  short		dietag;			/* Tag for this DIE */
  long		at_padding;
  long		at_sibling;
  BLOCK *	at_location;
  char *	at_name;
  unsigned short at_fund_type;
  BLOCK *	at_mod_fund_type;
  long		at_user_def_type;
  BLOCK *	at_mod_u_d_type;
  short		at_ordering;
  BLOCK *	at_subscr_data;
  long		at_byte_size;
  short		at_bit_offset;
  long		at_bit_size;
  BLOCK *	at_deriv_list;
  BLOCK *	at_element_list;
  long		at_stmt_list;
  long		at_low_pc;
  long		at_high_pc;
  long		at_language;
  long		at_member;
  long		at_discr;
  BLOCK *	at_discr_value;
  short		at_visibility;
  long		at_import;
  BLOCK *	at_string_length;
  char *	at_comp_dir;
  char *	at_producer;
  long		at_loclist;
  long		at_frame_base;
  short		at_incomplete;
  long		at_start_scope;
  long		at_stride_size;
  long		at_src_info;
  short		at_prototyped;
  BLOCK *	at_const_data;
  short		at_is_external;
  unsigned int	at_is_external_p:1;
  unsigned int	at_stmt_list_p:1;
};

static int diecount;	/* Approximate count of dies for compilation unit */
static struct dieinfo *curdie;	/* For warnings and such */

static char *dbbase;	/* Base pointer to dwarf info */
static int dbroff;	/* Relative offset from start of .debug section */
static char *lnbase;	/* Base pointer to line section */
static int isreg;	/* Kludge to identify register variables */

static CORE_ADDR baseaddr;	/* Add to each symbol value */

/* Each partial symbol table entry contains a pointer to private data for the
   read_symtab() function to use when expanding a partial symbol table entry
   to a full symbol table entry.  For DWARF debugging info, this data is
   contained in the following structure and macros are provided for easy
   access to the members given a pointer to a partial symbol table entry.

   dbfoff	Always the absolute file offset to the start of the ".debug"
		section for the file containing the DIE's being accessed.

   dbroff	Relative offset from the start of the ".debug" access to the
		first DIE to be accessed.  When building the partial symbol
		table, this value will be zero since we are accessing the
		entire ".debug" section.  When expanding a partial symbol
		table entry, this value will be the offset to the first
		DIE for the compilation unit containing the symbol that
		triggers the expansion.

   dblength	The size of the chunk of DIE's being examined, in bytes.

   lnfoff	The absolute file offset to the line table fragment.  Ignored
		when building partial symbol tables, but used when expanding
		them, and contains the absolute file offset to the fragment
		of the ".line" section containing the line numbers for the
		current compilation unit.
 */

struct dwfinfo {
  int dbfoff;		/* Absolute file offset to start of .debug section */
  int dbroff;		/* Relative offset from start of .debug section */
  int dblength;		/* Size of the chunk of DIE's being examined */
  int lnfoff;		/* Absolute file offset to line table fragment */
};

#define DBFOFF(p) (((struct dwfinfo *)((p)->read_symtab_private))->dbfoff)
#define DBROFF(p) (((struct dwfinfo *)((p)->read_symtab_private))->dbroff)
#define DBLENGTH(p) (((struct dwfinfo *)((p)->read_symtab_private))->dblength)
#define LNFOFF(p) (((struct dwfinfo *)((p)->read_symtab_private))->lnfoff)

/*  Record the symbols defined for each context in a linked list.  We don't
    create a struct block for the context until we know how long to make it.
    Global symbols for each file are maintained in the global_symbols list. */

struct pending_symbol {
  struct pending_symbol *next;		/* Next pending symbol */
  struct symbol *symbol;		/* The actual symbol */
};

static struct pending_symbol *global_symbols;	/* global funcs and vars */
static struct block *global_symbol_block;

/*  Line number entries are read into a dynamically expandable vector before
    being added to the symbol table section.  Once we know how many there are
    we can add them. */

static struct linetable *line_vector;	/* Vector of line numbers. */
static int line_vector_index;		/* Index of next entry.  */
static int line_vector_length;		/* Current allocation limit */

/* Scope information is kept in a scope tree, one node per scope.  Each time
   a new scope is started, a child node is created under the current node
   and set to the current scope.  Each time a scope is closed, the current
   scope moves back up the tree to the parent of the current scope.

   Each scope contains a pointer to the list of symbols defined in the scope,
   a pointer to the block vector for the scope, a pointer to the symbol
   that names the scope (if any), and the range of PC values that mark
   the start and end of the scope.  */

struct scopenode {
    struct scopenode *parent;
    struct scopenode *child;
    struct scopenode *sibling;
    struct pending_symbol *symbols;
    struct block *block;
    struct symbol *namesym;
    CORE_ADDR lowpc;
    CORE_ADDR highpc;
};

static struct scopenode *scopetree;
static struct scopenode *scope;

/* DIES which have user defined types or modified user defined types refer to
   other DIES for the type information.  Thus we need to associate the offset
   of a DIE for a user defined type with a pointer to the type information.

   Originally this was done using a simple but expensive algorithm, with an
   array of unsorted structures, each containing an offset/type-pointer pair.
   This array was scanned linearly each time a lookup was done.  The result
   was that gdb was spending over half it's startup time munging through this
   array of pointers looking for a structure that had the right offset member.

   The second attempt used the same array of structures, but the array was
   sorted using qsort each time a new offset/type was recorded, and a binary
   search was used to find the type pointer for a given DIE offset.  This was
   even slower, due to the overhead of sorting the array each time a new
   offset/type pair was entered.

   The third attempt uses a fixed size array of type pointers, indexed by a
   value derived from the DIE offset.  Since the minimum DIE size is 4 bytes,
   we can divide any DIE offset by 4 to obtain a unique index into this fixed
   size array.  Since each element is a 4 byte pointer, it takes exactly as
   much memory to hold this array as to hold the DWARF info for a given
   compilation unit.  But it gets freed as soon as we are done with it. */

static struct type **utypes;	/* Pointer to array of user type pointers */
static int numutypes;		/* Max number of user type pointers */

/* Forward declarations of static functions so we don't have to worry
   about ordering within this file.  The EXFUN macro may be slightly
   misleading.  Should probably be called DCLFUN instead, or something
   more intuitive, since it can be used for both static and external
   definitions. */

static void dwarfwarn ();	/* EXFUN breaks with <varargs.h> (FIXME)*/

static void
EXFUN (scan_partial_symbols, (char *thisdie AND char *enddie));

static void
EXFUN (scan_compilation_units,
       (char *filename AND CORE_ADDR addr AND char *thisdie AND char *enddie
	AND unsigned int dbfoff AND unsigned int lnoffset));

static struct partial_symtab *
EXFUN(start_psymtab, (char *symfile_name AND CORE_ADDR addr
		      AND char *filename AND CORE_ADDR textlow
		      AND CORE_ADDR texthigh AND int dbfoff
		      AND int curoff AND int culength AND int lnfoff
		      AND struct partial_symbol *global_syms
		      AND struct partial_symbol *static_syms));
static void
EXFUN(add_partial_symbol, (struct dieinfo *dip));

static void
EXFUN(add_psymbol_to_list,
      (struct psymbol_allocation_list *listp AND char *name
      AND enum namespace space AND enum address_class class
      AND CORE_ADDR value));

static void
EXFUN(init_psymbol_list, (int total_symbols));

static void
EXFUN(basicdieinfo, (struct dieinfo *dip AND char *diep));

static void
EXFUN(completedieinfo, (struct dieinfo *dip));

static void
EXFUN(dwarf_psymtab_to_symtab, (struct partial_symtab *pst));

static void
EXFUN(psymtab_to_symtab_1, (struct partial_symtab *pst AND int desc ));

static struct symtab *
EXFUN(read_ofile_symtab, (struct partial_symtab *pst AND int desc));

static void
EXFUN(process_dies, (char *thisdie AND char *enddie));

static void
EXFUN(read_lexical_block_scope,
      (struct dieinfo *dip AND char *thisdie AND char *enddie));

static void
EXFUN(read_structure_scope,
     (struct dieinfo *dip AND char *thisdie AND char *enddie));

static struct type *
EXFUN(decode_array_element_type, (char *scan AND char *end));

static struct type *
EXFUN(decode_subscr_data, (char *scan AND char *end));

static void
EXFUN(read_array_type, (struct dieinfo *dip));

static void
EXFUN(read_subroutine_type,
     (struct dieinfo *dip AND char *thisdie AND char *enddie));

static void
EXFUN(read_enumeration,
     (struct dieinfo *dip AND char *thisdie AND char *enddie));

static struct type *
EXFUN(struct_type,
      (struct dieinfo *dip AND char *thisdie AND char *enddie));

static struct type *
EXFUN(enum_type, (struct dieinfo *dip));

static void
EXFUN(read_func_scope,
      (struct dieinfo *dip AND char *thisdie AND char *enddie));

static void
EXFUN(read_file_scope,
      (struct dieinfo *dip AND char *thisdie AND char *enddie));

static void
EXFUN(start_symtab, (void));

static void
EXFUN(end_symtab, (char *filename AND long language));

static int
EXFUN(scopecount, (struct scopenode *node));

static void
EXFUN(openscope,
      (struct symbol *namesym AND CORE_ADDR lowpc AND CORE_ADDR highpc));

static void
EXFUN(freescope, (struct scopenode *node));

static struct block *
EXFUN(buildblock, (struct pending_symbol *syms));

static void
EXFUN(closescope, (void));

static void
EXFUN(record_line, (int line AND CORE_ADDR pc));

static void
EXFUN(decode_line_numbers, (char *linetable));

static struct type *
EXFUN(decode_die_type, (struct dieinfo *dip));

static struct type *
EXFUN(decode_mod_fund_type, (char *typedata));

static struct type *
EXFUN(decode_mod_u_d_type, (char *typedata));

static struct type *
EXFUN(decode_modified_type,
      (unsigned char *modifiers AND unsigned short modcount AND int mtype));

static struct type *
EXFUN(decode_fund_type, (unsigned short fundtype));

static char *
EXFUN(create_name, (char *name AND struct obstack *obstackp));

static void
EXFUN(add_symbol_to_list,
      (struct symbol *symbol AND struct pending_symbol **listhead));

static struct block **
EXFUN(gatherblocks, (struct block **dest AND struct scopenode *node));

static struct blockvector *
EXFUN(make_blockvector, (void));

static struct type *
EXFUN(lookup_utype, (DIEREF dieref));

static struct type *
EXFUN(alloc_utype, (DIEREF dieref AND struct type *usetype));

static struct symbol *
EXFUN(new_symbol, (struct dieinfo *dip));

static int
EXFUN(locval, (char *loc));

static void
EXFUN(record_misc_function, (char *name AND CORE_ADDR address));

static int
EXFUN(compare_psymbols,
      (struct partial_symbol *s1 AND struct partial_symbol *s2));


/*

GLOBAL FUNCTION

	dwarf_build_psymtabs -- build partial symtabs from DWARF debug info

SYNOPSIS

	void dwarf_build_psymtabs (int desc, char *filename, CORE_ADDR addr,
	     int mainline, unsigned int dbfoff, unsigned int dbsize,
	     unsigned int lnoffset, unsigned int lnsize)

DESCRIPTION

	This function is called upon to build partial symtabs from files
	containing DIE's (Dwarf Information Entries) and DWARF line numbers.

	It is passed a file descriptor for an open file containing the DIES
	and line number information, the corresponding filename for that
	file, a base address for relocating the symbols, a flag indicating
	whether or not this debugging information is from a "main symbol
	table" rather than a shared library or dynamically linked file,
	and file offset/size pairs for the DIE information and line number
	information.

RETURNS

	No return value.

 */

void
DEFUN(dwarf_build_psymtabs,
      (desc, filename, addr, mainline, dbfoff, dbsize, lnoffset, lnsize),
      int desc AND
      char *filename AND
      CORE_ADDR addr AND
      int mainline AND
      unsigned int dbfoff AND
      unsigned int dbsize AND
      unsigned int lnoffset AND
      unsigned int lnsize)
{
  struct cleanup *back_to;
  
  dbbase = xmalloc (dbsize);
  dbroff = 0;
  if ((lseek (desc, dbfoff, 0) != dbfoff) ||
      (read (desc, dbbase, dbsize) != dbsize))
    {
      free (dbbase);
      error ("can't read DWARF data from '%s'", filename);
    }
  back_to = make_cleanup (free, dbbase);
  
  /* If we are reinitializing, or if we have never loaded syms yet, init.
     Since we have no idea how many DIES we are looking at, we just guess
     some arbitrary value. */
  
  if (mainline || global_psymbols.size == 0 || static_psymbols.size == 0)
    {
      init_psymbol_list (1024);
    }
  
  init_misc_bunches ();
  make_cleanup (discard_misc_bunches, 0);
  
  /* Follow the compilation unit sibling chain, building a partial symbol
     table entry for each one.  Save enough information about each compilation
     unit to locate the full DWARF information later. */
  
  scan_compilation_units (filename, addr, dbbase, dbbase + dbsize,
			  dbfoff, lnoffset);
  
  /* Go over the miscellaneous functions and install them in the miscellaneous
     function vector. */
  
  condense_misc_bunches (!mainline);
  do_cleanups (back_to);
}


/*

LOCAL FUNCTION

	record_misc_function -- add entry to miscellaneous function vector

SYNOPSIS

	static void record_misc_function (char *name, CORE_ADDR address)

DESCRIPTION

	Given a pointer to the name of a symbol that should be added to the
	miscellaneous function vector, and the address associated with that
	symbol, records this information for later use in building the
	miscellaneous function vector.

NOTES

	FIXME:  For now we just use mf_text as the type.  This should be
	fixed.
 */

static void
DEFUN(record_misc_function, (name, address), char *name AND CORE_ADDR address)
{
  prim_record_misc_function (obsavestring (name, strlen (name)), address,
			     mf_text);
}

/*

LOCAL FUNCTION

	dwarfwarn -- issue a DWARF related warning

DESCRIPTION

	Issue warnings about DWARF related things that aren't serious enough
	to warrant aborting with an error, but should not be ignored either.
	This includes things like detectable corruption in DIE's, missing
	DIE's, unimplemented features, etc.

	In general, running across tags or attributes that we don't recognize
	is not considered to be a problem and we should not issue warnings
	about such.

NOTES

	We mostly follow the example of the error() routine, but without
	returning to command level.  It is arguable about whether warnings
	should be issued at all, and if so, where they should go (stdout or
	stderr).

	We assume that curdie is valid and contains at least the basic
	information for the DIE where the problem was noticed.
*/

static void
dwarfwarn (va_alist)
     va_dcl
{
  va_list ap;
  char *fmt;
  
  va_start (ap);
  fmt = va_arg (ap, char *);
  warning_setup ();
  fprintf (stderr, "DWARF warning (ref 0x%x): ", curdie -> dieref);
  if (curdie -> at_name)
    {
      fprintf (stderr, "'%s': ", curdie -> at_name);
    }
  vfprintf (stderr, fmt, ap);
  fprintf (stderr, "\n");
  fflush (stderr);
  va_end (ap);
}

/*

LOCAL FUNCTION

	compare_psymbols -- compare two partial symbols by name

DESCRIPTION

	Given pointer to two partial symbol table entries, compare
	them by name and return -N, 0, or +N (ala strcmp).  Typically
	used by sorting routines like qsort().

NOTES

	This is a copy from dbxread.c.  It should be moved to a generic
	gdb file and made available for all psymtab builders (FIXME).

	Does direct compare of first two characters before punting
	and passing to strcmp for longer compares.  Note that the
	original version had a bug whereby two null strings or two
	identically named one character strings would return the
	comparison of memory following the null byte.

 */

static int
DEFUN(compare_psymbols, (s1, s2), 
      struct partial_symbol *s1 AND
      struct partial_symbol *s2)
{
  register char *st1 = SYMBOL_NAME (s1);
  register char *st2 = SYMBOL_NAME (s2);

  if ((st1[0] - st2[0]) || !st1[0])
    {
      return (st1[0] - st2[0]);
    }
  else if ((st1[1] - st2[1]) || !st1[1])
    {
      return (st1[1] - st2[1]);
    }
  else
    {
      return (strcmp (st1 + 2, st2 + 2));
    }
}

/*

LOCAL FUNCTION

	read_lexical_block_scope -- process all dies in a lexical block

SYNOPSIS

	static void read_lexical_block_scope (struct dieinfo *dip,
		char *thisdie, char *enddie)

DESCRIPTION

	Process all the DIES contained within a lexical block scope.
	Start a new scope, process the dies, and then close the scope.

 */

static void
DEFUN(read_lexical_block_scope, (dip, thisdie, enddie),
     struct dieinfo *dip AND
     char *thisdie AND
     char *enddie)
{
  openscope (NULL, dip -> at_low_pc, dip -> at_high_pc);
  process_dies (thisdie + dip -> dielength, enddie);
  closescope ();
}

/*

LOCAL FUNCTION

	lookup_utype -- look up a user defined type from die reference

SYNOPSIS

	static type *lookup_utype (DIEREF dieref)

DESCRIPTION

	Given a DIE reference, lookup the user defined type associated with
	that DIE, if it has been registered already.  If not registered, then
	return NULL.  Alloc_utype() can be called to register an empty
	type for this reference, which will be filled in later when the
	actual referenced DIE is processed.
 */

static struct type *
DEFUN(lookup_utype, (dieref), DIEREF dieref)
{
  struct type *type = NULL;
  int utypeidx;
  
  utypeidx = (dieref - dbroff) / 4;
  if ((utypeidx < 0) || (utypeidx >= numutypes))
    {
      dwarfwarn ("reference to DIE (0x%x) outside compilation unit", dieref);
    }
  else
    {
      type = *(utypes + utypeidx);
    }
  return (type);
}


/*

LOCAL FUNCTION

	alloc_utype  -- add a user defined type for die reference

SYNOPSIS

	static type *alloc_utype (DIEREF dieref, struct type *utypep)

DESCRIPTION

	Given a die reference DIEREF, and a possible pointer to a user
	defined type UTYPEP, register that this reference has a user
	defined type and either use the specified type in UTYPEP or
	make a new empty type that will be filled in later.

	We should only be called after calling lookup_utype() to verify that
	there is not currently a type registered for DIEREF.
 */

static struct type *
DEFUN(alloc_utype, (dieref, utypep),
     DIEREF dieref AND
     struct type *utypep)
{
  struct type **typep;
  int utypeidx;
  
  utypeidx = (dieref - dbroff) / 4;
  typep = utypes + utypeidx;
  if ((utypeidx < 0) || (utypeidx >= numutypes))
    {
      utypep = builtin_type_int;
      dwarfwarn ("reference to DIE (0x%x) outside compilation unit", dieref);
    }
  else if (*typep != NULL)
    {
      utypep = *typep;
      SQUAWK (("internal error: dup user type allocation"));
    }
  else
    {
      if (utypep == NULL)
	{
	  utypep = (struct type *)
	    obstack_alloc (symbol_obstack, sizeof (struct type));
	  (void) memset (utypep, 0, sizeof (struct type));
	}
      *typep = utypep;
    }
  return (utypep);
}

/*

LOCAL FUNCTION

	decode_die_type -- return a type for a specified die

SYNOPSIS

	static struct type *decode_die_type (struct dieinfo *dip)

DESCRIPTION

	Given a pointer to a die information structure DIP, decode the
	type of the die and return a pointer to the decoded type.  All
	dies without specific types default to type int.
 */

static struct type *
DEFUN(decode_die_type, (dip), struct dieinfo *dip)
{
  struct type *type = NULL;
  
  if (dip -> at_fund_type != 0)
    {
      type = decode_fund_type (dip -> at_fund_type);
    }
  else if (dip -> at_mod_fund_type != NULL)
    {
      type = decode_mod_fund_type (dip -> at_mod_fund_type);
    }
  else if (dip -> at_user_def_type)
    {
      if ((type = lookup_utype (dip -> at_user_def_type)) == NULL)
	{
	  type = alloc_utype (dip -> at_user_def_type, NULL);
	}
    }
  else if (dip -> at_mod_u_d_type)
    {
      type = decode_mod_u_d_type (dip -> at_mod_u_d_type);
    }
  else
    {
      type = builtin_type_int;
    }
  return (type);
}

/*

LOCAL FUNCTION

	struct_type -- compute and return the type for a struct or union

SYNOPSIS

	static struct type *struct_type (struct dieinfo *dip, char *thisdie,
	    char *enddie)

DESCRIPTION

	Given pointer to a die information structure for a die which
	defines a union or structure, and pointers to the raw die data
	that define the range of dies which define the members, compute
	and return the user defined type for the structure or union.
 */

static struct type *
DEFUN(struct_type, (dip, thisdie, enddie),
     struct dieinfo *dip AND
     char *thisdie AND
     char *enddie)
{
  struct type *type;
  struct nextfield {
    struct nextfield *next;
    struct field field;
  };
  struct nextfield *list = NULL;
  struct nextfield *new;
  int nfields = 0;
  int n;
  char *tpart1;
  char *tpart2;
  char *tpart3;
  struct dieinfo mbr;
  
  if ((type = lookup_utype (dip -> dieref)) == NULL)
    {
      type = alloc_utype (dip -> dieref, NULL);
    }
  switch (dip -> dietag)
    {
    case TAG_structure_type:
      TYPE_CODE (type) = TYPE_CODE_STRUCT;
      tpart1 = "struct ";
      break;
    case TAG_union_type:
      TYPE_CODE (type) = TYPE_CODE_UNION;
      tpart1 = "union ";
      break;
    default:
      tpart1 = "";
      SQUAWK (("missing structure or union tag"));
      TYPE_CODE (type) = TYPE_CODE_UNDEF;
      break;
    }
  if (dip -> at_name == NULL)
    {
      tpart2 = "{...}";
    }
  else
    {
      tpart2 = dip -> at_name;
    }
  if (dip -> at_byte_size == 0)
    {
      tpart3 = " <opaque>";
    } else {
      TYPE_LENGTH (type) = dip -> at_byte_size;
      tpart3 = "";
    }
  TYPE_NAME (type) = concat (tpart1, tpart2, tpart3);
  thisdie += dip -> dielength;
  while (thisdie < enddie)
    {
      basicdieinfo (&mbr, thisdie);
      completedieinfo (&mbr);
      if (mbr.dielength <= sizeof (long))
	{
	  break;
	}
      switch (mbr.dietag)
	{
	case TAG_member:
	  /* Get space to record the next field's data.  */
	  new = (struct nextfield *) alloca (sizeof (struct nextfield));
	  new -> next = list;
	  list = new;
	  /* Save the data.  */
	  list -> field.name = savestring (mbr.at_name, strlen (mbr.at_name));
	  list -> field.type = decode_die_type (&mbr);
	  list -> field.bitpos = 8 * locval (mbr.at_location);
	  list -> field.bitsize = 0;
	  nfields++;
	  break;
	default:
	  SQUAWK (("bad member of '%s'", TYPE_NAME (type)));
	  break;
	}
      thisdie += mbr.dielength;
    }
  /* Now create the vector of fields, and record how big it is.  */
  TYPE_NFIELDS (type) = nfields;
  TYPE_FIELDS (type) = (struct field *)
    obstack_alloc (symbol_obstack, sizeof (struct field) * nfields);
  /* Copy the saved-up fields into the field vector.  */
  for (n = nfields; list; list = list -> next)
    {
      TYPE_FIELD (type, --n) = list -> field;
    }	
  return (type);
}

/*

LOCAL FUNCTION

	read_structure_scope -- process all dies within struct or union

SYNOPSIS

	static void read_structure_scope (struct dieinfo *dip,
		char *thisdie, char *enddie)

DESCRIPTION

	Called when we find the DIE that starts a structure or union
	scope (definition) to process all dies that define the members
	of the structure or union.  DIP is a pointer to the die info
	struct for the DIE that names the structure or union.

NOTES

	Note that we need to call struct_type regardless of whether or not
	we have a symbol, since we might have a structure or union without
	a tag name (thus no symbol for the tagname).
 */

static void
DEFUN(read_structure_scope, (dip, thisdie, enddie),
     struct dieinfo *dip AND
     char *thisdie AND
     char *enddie)
{
  struct type *type;
  struct symbol *sym;
  
  type = struct_type (dip, thisdie, enddie);
  if ((sym = new_symbol (dip)) != NULL)
    {
      SYMBOL_TYPE (sym) = type;
    }
}

/*

LOCAL FUNCTION

	decode_array_element_type -- decode type of the array elements

SYNOPSIS

	static struct type *decode_array_element_type (char *scan, char *end)

DESCRIPTION

	As the last step in decoding the array subscript information for an
	array DIE, we need to decode the type of the array elements.  We are
	passed a pointer to this last part of the subscript information and
	must return the appropriate type.  If the type attribute is not
	recognized, just warn about the problem and return type int.
 */

static struct type *
DEFUN(decode_array_element_type, (scan, end), char *scan AND char *end)
{
  struct type *typep;
  short attribute;
  DIEREF dieref;
  unsigned short fundtype;
  
  (void) memcpy (&attribute, scan, sizeof (short));
  scan += sizeof (short);
  switch (attribute)
    {
    case AT_fund_type:
      (void) memcpy (&fundtype, scan, sizeof (short));
      typep = decode_fund_type (fundtype);
      break;
    case AT_mod_fund_type:
      typep = decode_mod_fund_type (scan);
      break;
    case AT_user_def_type:
      (void) memcpy (&dieref, scan, sizeof (DIEREF));
      if ((typep = lookup_utype (dieref)) == NULL)
	{
	  typep = alloc_utype (dieref, NULL);
	}
      break;
    case AT_mod_u_d_type:
      typep = decode_mod_u_d_type (scan);
      break;
    default:
      SQUAWK (("bad array element type attribute 0x%x", attribute));
      typep = builtin_type_int;
      break;
    }
  return (typep);
}

/*

LOCAL FUNCTION

	decode_subscr_data -- decode array subscript and element type data

SYNOPSIS

	static struct type *decode_subscr_data (char *scan, char *end)

DESCRIPTION

	The array subscripts and the data type of the elements of an
	array are described by a list of data items, stored as a block
	of contiguous bytes.  There is a data item describing each array
	dimension, and a final data item describing the element type.
	The data items are ordered the same as their appearance in the
	source (I.E. leftmost dimension first, next to leftmost second,
	etc).

	We are passed a pointer to the start of the block of bytes
	containing the data items, and a pointer to the first byte past
	the data.  This function decodes the data and returns a type.

BUGS
	FIXME:  This code only implements the forms currently used
	by the AT&T and GNU C compilers.

	The end pointer is supplied for error checking, maybe we should
	use it for that...
 */

static struct type *
DEFUN(decode_subscr_data, (scan, end), char *scan AND char *end)
{
  struct type *typep = NULL;
  struct type *nexttype;
  int format;
  short fundtype;
  long lowbound;
  long highbound;
  
  format = *scan++;
  switch (format)
    {
    case FMT_ET:
      typep = decode_array_element_type (scan, end);
      break;
    case FMT_FT_C_C:
      (void) memcpy (&fundtype, scan, sizeof (short));
      scan += sizeof (short);
      if (fundtype != FT_integer && fundtype != FT_signed_integer
	  && fundtype != FT_unsigned_integer)
	{
	  SQUAWK (("array subscripts must be integral types, not type 0x%x",
		     fundtype));
	}
      else
	{
	  (void) memcpy (&lowbound, scan, sizeof (long));
	  scan += sizeof (long);
	  (void) memcpy (&highbound, scan, sizeof (long));
	  scan += sizeof (long);
	  nexttype = decode_subscr_data (scan, end);
	  if (nexttype != NULL)
	    {
	      typep = (struct type *)
		obstack_alloc (symbol_obstack, sizeof (struct type));
	      (void) memset (typep, 0, sizeof (struct type));
	      TYPE_CODE (typep) = TYPE_CODE_ARRAY;
	      TYPE_LENGTH (typep) = TYPE_LENGTH (nexttype);
	      TYPE_LENGTH (typep) *= lowbound + highbound + 1;
	      TYPE_TARGET_TYPE (typep) = nexttype;
	    }		    
	}
      break;
    case FMT_FT_C_X:
    case FMT_FT_X_C:
    case FMT_FT_X_X:
    case FMT_UT_C_C:
    case FMT_UT_C_X:
    case FMT_UT_X_C:
    case FMT_UT_X_X:
      SQUAWK (("array subscript format 0x%x not handled yet", format));
      break;
    default:
      SQUAWK (("unknown array subscript format %x", format));
      break;
    }
  return (typep);
}

/*

LOCAL FUNCTION

	read_array_type -- read TAG_array_type DIE

SYNOPSIS

	static void read_array_type (struct dieinfo *dip)

DESCRIPTION

	Extract all information from a TAG_array_type DIE and add to
	the user defined type vector.
 */

static void
DEFUN(read_array_type, (dip), struct dieinfo *dip)
{
  struct type *type;
  char *sub;
  char *subend;
  short temp;
  
  if (dip -> at_ordering != ORD_row_major)
    {
      /* FIXME:  Can gdb even handle column major arrays? */
      SQUAWK (("array not row major; not handled correctly"));
    }
  if ((sub = dip -> at_subscr_data) != NULL)
    {
      (void) memcpy (&temp, sub, sizeof (short));
      subend = sub + sizeof (short) + temp;
      sub += sizeof (short);
      type = decode_subscr_data (sub, subend);
      if (type == NULL)
	{
	  type = alloc_utype (dip -> dieref, NULL);
	  TYPE_CODE (type) = TYPE_CODE_ARRAY;
	  TYPE_TARGET_TYPE (type) = builtin_type_int;
	  TYPE_LENGTH (type) = 1 * TYPE_LENGTH (TYPE_TARGET_TYPE (type));
	}
      else
	{
	  type = alloc_utype (dip -> dieref, type);
	}
    }
}

/*

LOCAL FUNCTION

	read_subroutine_type -- process TAG_subroutine_type dies

SYNOPSIS

	static void read_subroutine_type (struct dieinfo *dip, char thisdie,
		char *enddie)

DESCRIPTION

	Handle DIES due to C code like:

	struct foo {
	    int (*funcp)(int a, long l);  (Generates TAG_subroutine_type DIE)
	    int b;
	};

NOTES

	The parameter DIES are currently ignored.  See if gdb has a way to
	include this info in it's type system, and decode them if so.  Is
	this what the type structure's "arg_types" field is for?  (FIXME)
 */

static void
DEFUN(read_subroutine_type, (dip, thisdie, enddie),
     struct dieinfo *dip AND
     char *thisdie AND
     char *enddie)
{
  struct type *type;
  
  type = decode_die_type (dip);
  type = lookup_function_type (type);
  type = alloc_utype (dip -> dieref, type);
}

/*

LOCAL FUNCTION

	read_enumeration -- process dies which define an enumeration

SYNOPSIS

	static void read_enumeration (struct dieinfo *dip, char *thisdie,
		char *enddie)

DESCRIPTION

	Given a pointer to a die which begins an enumeration, process all
	the dies that define the members of the enumeration.

NOTES

	Note that we need to call enum_type regardless of whether or not we
	have a symbol, since we might have an enum without a tag name (thus
	no symbol for the tagname).
 */

static void
DEFUN(read_enumeration, (dip, thisdie, enddie),
     struct dieinfo *dip AND
     char *thisdie AND
     char *enddie)
{
  struct type *type;
  struct symbol *sym;
  
  type = enum_type (dip);
  if ((sym = new_symbol (dip)) != NULL)
    {
      SYMBOL_TYPE (sym) = type;
    }
}

/*

LOCAL FUNCTION

	enum_type -- decode and return a type for an enumeration

SYNOPSIS

	static type *enum_type (struct dieinfo *dip)

DESCRIPTION

	Given a pointer to a die information structure for the die which
	starts an enumeration, process all the dies that define the members
	of the enumeration and return a type pointer for the enumeration.
 */

static struct type *
DEFUN(enum_type, (dip), struct dieinfo *dip)
{
  struct type *type;
  struct nextfield {
    struct nextfield *next;
    struct field field;
  };
  struct nextfield *list = NULL;
  struct nextfield *new;
  int nfields = 0;
  int n;
  char *tpart1;
  char *tpart2;
  char *tpart3;
  char *scan;
  char *listend;
  long temp;
  
  if ((type = lookup_utype (dip -> dieref)) == NULL)
    {
      type = alloc_utype (dip -> dieref, NULL);
    }
  TYPE_CODE (type) = TYPE_CODE_ENUM;
  tpart1 = "enum ";
  if (dip -> at_name == NULL)
    {
      tpart2 = "{...}";
    } else {
      tpart2 = dip -> at_name;
    }
  if (dip -> at_byte_size == 0)
    {
      tpart3 = " <opaque>";
    }
  else
    {
      TYPE_LENGTH (type) = dip -> at_byte_size;
      tpart3 = "";
    }
  TYPE_NAME (type) = concat (tpart1, tpart2, tpart3);
  if ((scan = dip -> at_element_list) != NULL)
    {
      (void) memcpy (&temp, scan, sizeof (temp));
      listend = scan + temp + sizeof (temp);
      scan += sizeof (temp);
      while (scan < listend)
	{
	  new = (struct nextfield *) alloca (sizeof (struct nextfield));
	  new -> next = list;
	  list = new;
	  list -> field.type = NULL;
	  list -> field.bitsize = 0;
	  (void) memcpy (&list -> field.bitpos, scan, sizeof (long));
	  scan += sizeof (long);
	  list -> field.name = savestring (scan, strlen (scan));
	  scan += strlen (scan) + 1;
	  nfields++;
	}
    }
  /* Now create the vector of fields, and record how big it is.  */
  TYPE_NFIELDS (type) = nfields;
  TYPE_FIELDS (type) = (struct field *)
    obstack_alloc (symbol_obstack, sizeof (struct field) * nfields);
  /* Copy the saved-up fields into the field vector.  */
  for (n = nfields; list; list = list -> next)
    {
      TYPE_FIELD (type, --n) = list -> field;
    }	
  return (type);
}

/*

LOCAL FUNCTION

	read_func_scope -- process all dies within a function scope

SYNOPSIS

	static void read_func_scope (struct dieinfo dip, char *thisdie,
	    char *enddie)

DESCRIPTION

	Process all dies within a given function scope.  We are passed
	a die information structure pointer DIP for the die which
	starts the function scope, and pointers into the raw die data
	that define the dies within the function scope.

	For now, we ignore lexical block scopes within the function.
	The problem is that AT&T cc does not define a DWARF lexical
	block scope for the function itself, while gcc defines a
	lexical block scope for the function.  We need to think about
	how to handle this difference, or if it is even a problem.
	(FIXME)
 */

static void
DEFUN(read_func_scope, (dip, thisdie, enddie),
     struct dieinfo *dip AND
     char *thisdie AND
     char *enddie)
{
  struct symbol *sym;
  
  if (entry_point >= dip -> at_low_pc && entry_point < dip -> at_high_pc)
    {
      entry_scope_lowpc = dip -> at_low_pc;
      entry_scope_highpc = dip -> at_high_pc;
    }
  if (strcmp (dip -> at_name, "main") == 0)	/* FIXME: hardwired name */
    {
      main_scope_lowpc = dip -> at_low_pc;
      main_scope_highpc = dip -> at_high_pc;
    }
  sym = new_symbol (dip);
  openscope (sym, dip -> at_low_pc, dip -> at_high_pc);
  process_dies (thisdie + dip -> dielength, enddie);
  closescope ();
}

/*

LOCAL FUNCTION

	read_file_scope -- process all dies within a file scope

SYNOPSIS

	static void read_file_scope (struct dieinfo *dip, char *thisdie
		char *enddie)

DESCRIPTION

	Process all dies within a given file scope.  We are passed a
	pointer to the die information structure for the die which
	starts the file scope, and pointers into the raw die data which
	mark the range of dies within the file scope.

	When the partial symbol table is built, the file offset for the line
	number table for each compilation unit is saved in the partial symbol
	table entry for that compilation unit.  As the symbols for each
	compilation unit are read, the line number table is read into memory
	and the variable lnbase is set to point to it.  Thus all we have to
	do is use lnbase to access the line number table for the current
	compilation unit.
 */

static void
DEFUN(read_file_scope, (dip, thisdie, enddie),
     struct dieinfo *dip AND
     char *thisdie AND
     char *enddie)
{
  struct cleanup *back_to;
  
  if (entry_point >= dip -> at_low_pc && entry_point < dip -> at_high_pc)
    {
      startup_file_start = dip -> at_low_pc;
      startup_file_end = dip -> at_high_pc;
    }
  numutypes = (enddie - thisdie) / 4;
  utypes = (struct type **) xmalloc (numutypes * sizeof (struct type *));
  back_to = make_cleanup (free, utypes);
  (void) memset (utypes, 0, numutypes * sizeof (struct type *));
  start_symtab ();
  openscope (NULL, dip -> at_low_pc, dip -> at_high_pc);
  decode_line_numbers (lnbase);
  process_dies (thisdie + dip -> dielength, enddie);
  closescope ();
  end_symtab (dip -> at_name, dip -> at_language);
  do_cleanups (back_to);
  utypes = NULL;
  numutypes = 0;
}

/*

LOCAL FUNCTION

	start_symtab -- do initialization for starting new symbol table

SYNOPSIS

	static void start_symtab (void)

DESCRIPTION

	Called whenever we are starting to process dies for a new
	compilation unit, to perform initializations.  Right now
	the only thing we really have to do is initialize storage
	space for the line number vector.

 */

static void
DEFUN_VOID (start_symtab)
{
  int nbytes;

  line_vector_index = 0;
  line_vector_length = 1000;
  nbytes = sizeof (struct linetable);
  nbytes += line_vector_length * sizeof (struct linetable_entry);
  line_vector = (struct linetable *) xmalloc (nbytes);
}

/*

LOCAL FUNCTION

	process_dies -- process a range of DWARF Information Entries

SYNOPSIS

	static void process_dies (char *thisdie, char *enddie)

DESCRIPTION

	Process all DIE's in a specified range.  May be (and almost
	certainly will be) called recursively.
 */

static void
DEFUN(process_dies, (thisdie, enddie), char *thisdie AND char *enddie)
{
  char *nextdie;
  struct dieinfo di;
  
  while (thisdie < enddie)
    {
      basicdieinfo (&di, thisdie);
      if (di.dielength < sizeof (long))
	{
	  break;
	}
      else if (di.dietag == TAG_padding)
	{
	  nextdie = thisdie + di.dielength;
	}
      else
	{
	  completedieinfo (&di);
	  if (di.at_sibling != 0)
	    {
	      nextdie = dbbase + di.at_sibling - dbroff;
	    }
	  else
	    {
	      nextdie = thisdie + di.dielength;
	    }
	  switch (di.dietag)
	    {
	    case TAG_compile_unit:
	      read_file_scope (&di, thisdie, nextdie);
	      break;
	    case TAG_global_subroutine:
	    case TAG_subroutine:
	      if (!di.at_is_external_p)
		{
		  read_func_scope (&di, thisdie, nextdie);
		}
	      break;
	    case TAG_lexical_block:
	      read_lexical_block_scope (&di, thisdie, nextdie);
	      break;
	    case TAG_structure_type:
	    case TAG_union_type:
	      read_structure_scope (&di, thisdie, nextdie);
	      break;
	    case TAG_enumeration_type:
	      read_enumeration (&di, thisdie, nextdie);
	      break;
	    case TAG_subroutine_type:
	      read_subroutine_type (&di, thisdie, nextdie);
	      break;
	    case TAG_array_type:
	      read_array_type (&di);
	      break;
	    default:
	      (void) new_symbol (&di);
	      break;
	    }
	}
      thisdie = nextdie;
    }
}

/*

LOCAL FUNCTION

	end_symtab -- finish processing for a compilation unit

SYNOPSIS

	static void end_symtab (char *filename, long language)

DESCRIPTION

	Complete the symbol table entry for the current compilation
	unit.  Make the struct symtab and put it on the list of all
	such symtabs.

 */

static void
DEFUN(end_symtab, (filename, language), char *filename AND long language)
{
  struct symtab *symtab;
  struct blockvector *blockvector;
  int nbytes;
  
  /* Ignore a file that has no functions with real debugging info.  */
  if (global_symbols == NULL && scopetree -> block == NULL)
    {
      free (line_vector);
      line_vector = NULL;
      line_vector_length = -1;
      freescope (scopetree);
      scope = scopetree = NULL;
    }
  
  /* Create the blockvector that points to all the file's blocks.  */
  
  blockvector = make_blockvector ();
  
  /* Now create the symtab object for this source file.  */
  
  symtab = (struct symtab *) xmalloc (sizeof (struct symtab));
  (void) memset (symtab, 0, sizeof (struct symtab));
  
  symtab -> free_ptr = 0;
  
  /* Fill in its components.  */
  symtab -> blockvector = blockvector;
  symtab -> free_code = free_linetable;
  symtab -> filename = savestring (filename, strlen (filename));
  
  /* Save the line number information. */
  
  line_vector -> nitems = line_vector_index;
  nbytes = sizeof (struct linetable);
  if (line_vector_index > 1)
    {
      nbytes += (line_vector_index - 1) * sizeof (struct linetable_entry);
    }
  symtab -> linetable = (struct linetable *) xrealloc (line_vector, nbytes);
  symtab -> nlines = 0;
  symtab -> line_charpos = 0;
  
  /* FIXME:  The following may need to be expanded for other languages */
  if (language == LANG_C89 || language == LANG_C) 
    {
      symtab -> language = language_c;
    }
  
  /* Link the new symtab into the list of such.  */
  symtab -> next = symtab_list;
  symtab_list = symtab;
  
  /* Recursively free the scope tree */
  freescope (scopetree);
  scope = scopetree = NULL;
  
  /* Reinitialize for beginning of new file. */
  line_vector = 0;
  line_vector_length = -1;
}

/*

LOCAL FUNCTION

	scopecount -- count the number of enclosed scopes

SYNOPSIS

	static int scopecount (struct scopenode *node)

DESCRIPTION

	Given pointer to a node, compute the size of the subtree which is
	rooted in this node, which also happens to be the number of scopes
	to the subtree.
 */

static int
DEFUN(scopecount, (node), struct scopenode *node)
{
  int count = 0;
  
  if (node != NULL)
    {
      count += scopecount (node -> child);
      count += scopecount (node -> sibling);
      count++;
    }
  return (count);
}

/*

LOCAL FUNCTION

	openscope -- start a new lexical block scope

SYNOPSIS

	static void openscope (struct symbol *namesym, CORE_ADDR lowpc,
		CORE_ADDR highpc)

DESCRIPTION

	Start a new scope by allocating a new scopenode, adding it as the
	next child of the current scope (if any) or as the root of the
	scope tree, and then making the new node the current scope node.
 */

static void
DEFUN(openscope, (namesym, lowpc, highpc),
     struct symbol *namesym AND
     CORE_ADDR lowpc AND
     CORE_ADDR highpc)
{
  struct scopenode *new;
  struct scopenode *child;
  
  new = (struct scopenode *) xmalloc (sizeof (*new));
  (void) memset (new, 0, sizeof (*new));
  new -> namesym = namesym;
  new -> lowpc = lowpc;
  new -> highpc = highpc;
  if (scope == NULL)
    {
      scopetree = new;
    }
  else if ((child = scope -> child) == NULL)
    {
      scope -> child = new;
      new -> parent = scope;
    }
  else
    {
    while (child -> sibling != NULL)
      {
	child = child -> sibling;
      }
    child -> sibling = new;
    new -> parent = scope;
  }
  scope = new;
}

/*

LOCAL FUNCTION

	freescope -- free a scope tree rooted at the given node

SYNOPSIS

	static void freescope (struct scopenode *node)

DESCRIPTION

	Given a pointer to a node in the scope tree, free the subtree
	rooted at that node.  First free all the children and sibling
	nodes, and then the node itself.  Used primarily for cleaning
	up after ourselves and returning memory to the system.
 */

static void
DEFUN(freescope, (node), struct scopenode *node)
{
  if (node != NULL)
    {
      freescope (node -> child);
      freescope (node -> sibling);
      free (node);
    }
}

/*

LOCAL FUNCTION

	buildblock -- build a new block from pending symbols list

SYNOPSIS

	static struct block *buildblock (struct pending_symbol *syms)

DESCRIPTION

	Given a pointer to a list of symbols, build a new block and free
	the symbol list structure.  Also check each symbol to see if it
	is the special symbol that flags that this block was compiled by
	gcc, and if so, mark the block appropriately.
 */

static struct block *
DEFUN(buildblock, (syms), struct pending_symbol *syms)
{
  struct pending_symbol *next, *next1;
  int i;
  struct block *newblock;
  int nbytes;
  
  for (next = syms, i = 0 ; next ; next = next -> next, i++) {;}
  
  /* Allocate a new block */
  
  nbytes = sizeof (struct block);
  if (i > 1)
    {
      nbytes += (i - 1) * sizeof (struct symbol *);
    }
  newblock = (struct block *) obstack_alloc (symbol_obstack, nbytes);
  (void) memset (newblock, 0, nbytes);
  
  /* Copy the symbols into the block.  */
  
  BLOCK_NSYMS (newblock) = i;
  for (next = syms ; next ; next = next -> next)
    {
      BLOCK_SYM (newblock, --i) = next -> symbol;
      if (STREQ (GCC_COMPILED_FLAG_SYMBOL, SYMBOL_NAME (next -> symbol)) ||
	  STREQ (GCC2_COMPILED_FLAG_SYMBOL, SYMBOL_NAME (next -> symbol)))
	{
	  BLOCK_GCC_COMPILED (newblock) = 1;
	}
    }    
  
  /* Now free the links of the list, and empty the list.  */
  
  for (next = syms ; next ; next = next1)
    {
      next1 = next -> next;
      free (next);
    }
  
  return (newblock);
}

/*

LOCAL FUNCTION

	closescope -- close a lexical block scope

SYNOPSIS

	static void closescope (void)

DESCRIPTION

	Close the current lexical block scope.  Closing the current scope
	is as simple as moving the current scope pointer up to the parent
	of the current scope pointer.  But we also take this opportunity
	to build the block for the current scope first, since we now have
	all of it's symbols.
 */

static void
DEFUN_VOID(closescope)
{
  struct scopenode *child;
  
  if (scope == NULL)
    {
      error ("DWARF parse error, too many close scopes");
    }
  else
    {
      if (scope -> parent == NULL)
	{
	  global_symbol_block = buildblock (global_symbols);
	  global_symbols = NULL;
	  BLOCK_START (global_symbol_block) = scope -> lowpc + baseaddr;
	  BLOCK_END (global_symbol_block) = scope -> highpc + baseaddr;
	}
      scope -> block = buildblock (scope -> symbols);
      scope -> symbols = NULL;
      BLOCK_START (scope -> block) = scope -> lowpc + baseaddr;
      BLOCK_END (scope -> block) = scope -> highpc + baseaddr;
    
      /* Put the local block in as the value of the symbol that names it.  */
    
      if (scope -> namesym)
	{
	  SYMBOL_BLOCK_VALUE (scope -> namesym) = scope -> block;
	  BLOCK_FUNCTION (scope -> block) = scope -> namesym;
	}
    
    /*  Install this scope's local block as the superblock of all child
	scope blocks. */
    
    for (child = scope -> child ; child ; child = child -> sibling)
      {
	BLOCK_SUPERBLOCK (child -> block) = scope -> block;
      }
    
      scope = scope -> parent;
    }
}

/*

LOCAL FUNCTION

	record_line -- record a line number entry in the line vector

SYNOPSIS

	static void record_line (int line, CORE_ADDR pc)

DESCRIPTION

	Given a line number and the corresponding pc value, record
	this pair in the line number vector, expanding the vector as
	necessary.
 */

static void
DEFUN(record_line, (line, pc), int line AND CORE_ADDR pc)
{
  struct linetable_entry *e;
  int nbytes;
  
  /* Make sure line vector is big enough.  */
  
  if (line_vector_index + 2 >= line_vector_length)
    {
      line_vector_length *= 2;
      nbytes = sizeof (struct linetable);
      nbytes += (line_vector_length * sizeof (struct linetable_entry));
      line_vector = (struct linetable *) xrealloc (line_vector, nbytes);
    }
  e = line_vector -> item + line_vector_index++;
  e -> line = line;
  e -> pc = pc;
}

/*

LOCAL FUNCTION

	decode_line_numbers -- decode a line number table fragment

SYNOPSIS

	static void decode_line_numbers (char *tblscan, char *tblend,
		long length, long base, long line, long pc)

DESCRIPTION

	Translate the DWARF line number information to gdb form.

	The ".line" section contains one or more line number tables, one for
	each ".line" section from the objects that were linked.

	The AT_stmt_list attribute for each TAG_source_file entry in the
	".debug" section contains the offset into the ".line" section for the
	start of the table for that file.

	The table itself has the following structure:

	<table length><base address><source statement entry>
	4 bytes       4 bytes       10 bytes

	The table length is the total size of the table, including the 4 bytes
	for the length information.

	The base address is the address of the first instruction generated
	for the source file.

	Each source statement entry has the following structure:

	<line number><statement position><address delta>
	4 bytes      2 bytes             4 bytes

	The line number is relative to the start of the file, starting with
	line 1.

	The statement position either -1 (0xFFFF) or the number of characters
	from the beginning of the line to the beginning of the statement.

	The address delta is the difference between the base address and
	the address of the first instruction for the statement.

	Note that we must copy the bytes from the packed table to our local
	variables before attempting to use them, to avoid alignment problems
	on some machines, particularly RISC processors.

BUGS

	Does gdb expect the line numbers to be sorted?  They are now by
	chance/luck, but are not required to be.  (FIXME)

	The line with number 0 is unused, gdb apparently can discover the
	span of the last line some other way. How?  (FIXME)
 */

static void
DEFUN(decode_line_numbers, (linetable), char *linetable)
{
  char *tblscan;
  char *tblend;
  long length;
  long base;
  long line;
  long pc;
  
  if (linetable != NULL)
    {
      tblscan = tblend = linetable;
      (void) memcpy (&length, tblscan, sizeof (long));
      tblscan += sizeof (long);
      tblend += length;
      (void) memcpy (&base, tblscan, sizeof (long));
      base += baseaddr;
      tblscan += sizeof (long);
      while (tblscan < tblend)
	{
	  (void) memcpy (&line, tblscan, sizeof (long));
	  tblscan += sizeof (long) + sizeof (short);
	  (void) memcpy (&pc, tblscan, sizeof (long));
	  tblscan += sizeof (long);
	  pc += base;
	  if (line > 0)
	    {
	      record_line (line, pc);
	    }
	}
    }
}

/*

LOCAL FUNCTION

	add_symbol_to_list -- add a symbol to head of current symbol list

SYNOPSIS

	static void add_symbol_to_list (struct symbol *symbol, struct
		pending_symbol **listhead)

DESCRIPTION

	Given a pointer to a symbol and a pointer to a pointer to a
	list of symbols, add this symbol as the current head of the
	list.  Typically used for example to add a symbol to the
	symbol list for the current scope.

 */

static void
DEFUN(add_symbol_to_list, (symbol, listhead),
     struct symbol *symbol AND struct pending_symbol **listhead)
{
  struct pending_symbol *link;
  
  if (symbol != NULL)
    {
      link = (struct pending_symbol *) xmalloc (sizeof (*link));
      link -> next = *listhead;
      link -> symbol = symbol;
      *listhead = link;
    }
}

/*

LOCAL FUNCTION

	gatherblocks -- walk a scope tree and build block vectors

SYNOPSIS

	static struct block **gatherblocks (struct block **dest,
		struct scopenode *node)

DESCRIPTION

	Recursively walk a scope tree rooted in the given node, adding blocks
	to the array pointed to by DEST, in preorder.  I.E., first we add the
	block for the current scope, then all the blocks for child scopes,
	and finally all the blocks for sibling scopes.
 */

static struct block **
DEFUN(gatherblocks, (dest, node),
      struct block **dest AND struct scopenode *node)
{
  if (node != NULL)
    {
      *dest++ = node -> block;
      dest = gatherblocks (dest, node -> child);
      dest = gatherblocks (dest, node -> sibling);
    }
  return (dest);
}

/*

LOCAL FUNCTION

	make_blockvector -- make a block vector from current scope tree

SYNOPSIS

	static struct blockvector *make_blockvector (void)

DESCRIPTION

	Make a blockvector from all the blocks in the current scope tree.
	The first block is always the global symbol block, followed by the
	block for the root of the scope tree which is the local symbol block,
	followed by all the remaining blocks in the scope tree, which are all
	local scope blocks.

NOTES

	Note that since the root node of the scope tree is created at the time
	each file scope is entered, there are always at least two blocks,
	neither of which may have any symbols, but always contribute a block
	to the block vector.  So the test for number of blocks greater than 1
	below is unnecessary given bug free code.

	The resulting block structure varies slightly from that produced
	by dbxread.c, in that block 0 and block 1 are sibling blocks while
	with dbxread.c, block 1 is a child of block 0.  This does not
	seem to cause any problems, but probably should be fixed. (FIXME)
 */

static struct blockvector *
DEFUN_VOID(make_blockvector)
{
  struct blockvector *blockvector = NULL;
  int i;
  int nbytes;
  
  /* Recursively walk down the tree, counting the number of blocks.
     Then add one to account for the global's symbol block */
  
  i = scopecount (scopetree) + 1;
  nbytes = sizeof (struct blockvector);
  if (i > 1)
    {
      nbytes += (i - 1) * sizeof (struct block *);
    }
  blockvector = (struct blockvector *)
    obstack_alloc (symbol_obstack, nbytes);
  
  /* Copy the blocks into the blockvector. */
  
  BLOCKVECTOR_NBLOCKS (blockvector) = i;
  BLOCKVECTOR_BLOCK (blockvector, 0) = global_symbol_block;
  gatherblocks (&BLOCKVECTOR_BLOCK (blockvector, 1), scopetree);
  
  return (blockvector);
}

/*

LOCAL FUNCTION

	locval -- compute the value of a location attribute

SYNOPSIS

	static int locval (char *loc)

DESCRIPTION

	Given pointer to a string of bytes that define a location, compute
	the location and return the value.

	When computing values involving the current value of the frame pointer,
	the value zero is used, which results in a value relative to the frame
	pointer, rather than the absolute value.  This is what GDB wants
	anyway.
    
	When the result is a register number, the global isreg flag is set,
	otherwise it is cleared.  This is a kludge until we figure out a better
	way to handle the problem.  Gdb's design does not mesh well with the
	DWARF notion of a location computing interpreter, which is a shame
	because the flexibility goes unused.

NOTES

	Note that stack[0] is unused except as a default error return.
	Note that stack overflow is not yet handled.
 */

static int
DEFUN(locval, (loc), char *loc)
{
  unsigned short nbytes;
  auto int stack[64];
  int stacki;
  char *end;
  long regno;
  
  (void) memcpy (&nbytes, loc, sizeof (short));
  end = loc + sizeof (short) + nbytes;
  stacki = 0;
  stack[stacki] = 0;
  isreg = 0;
  for (loc += sizeof (short); loc < end; loc += sizeof (long))
    {
      switch (*loc++) {
      case 0:
	/* error */
	loc = end;
	break;
      case OP_REG:
	/* push register (number) */
	(void) memcpy (&stack[++stacki], loc, sizeof (long));
	isreg = 1;
	break;
      case OP_BASEREG:
	/* push value of register (number) */
	/* Actually, we compute the value as if register has 0 */
	(void) memcpy (&regno, loc, sizeof (long));
	if (regno == R_FP)
	  {
	    stack[++stacki] = 0;
	  }
	else
	  {
	    stack[++stacki] = 0;
	    SQUAWK (("BASEREG %d not handled!", regno));
	  }
	break;
      case OP_ADDR:
	/* push address (relocated address) */
	(void) memcpy (&stack[++stacki], loc, sizeof (long));
	break;
      case OP_CONST:
	/* push constant (number) */
	(void) memcpy (&stack[++stacki], loc, sizeof (long));
	break;
      case OP_DEREF2:
	/* pop, deref and push 2 bytes (as a long) */
	SQUAWK (("OP_DEREF2 address %#x not handled", stack[stacki]));
	break;
      case OP_DEREF4:	/* pop, deref and push 4 bytes (as a long) */
	SQUAWK (("OP_DEREF4 address %#x not handled", stack[stacki]));
	break;
      case OP_ADD:	/* pop top 2 items, add, push result */
	stack[stacki - 1] += stack[stacki];
	stacki--;
	break;
      }
    }
  return (stack[stacki]);
}

/*

LOCAL FUNCTION

	read_ofile_symtab -- build a full symtab entry from chunk of DIE's

SYNOPSIS

	static struct symtab *read_ofile_symtab (struct partial_symtab *pst,
		int desc)

DESCRIPTION

	DESC is the file descriptor for the file, positioned at the
	beginning of the symtab
	SYM_SIZE is the size of the symbol section to read
	TEXT_OFFSET is the beginning of the text segment we are reading
	symbols for
	TEXT_SIZE is the size of the text segment read in.
	OFFSET is a relocation offset which gets added to each symbol

 */

static struct symtab *
DEFUN(read_ofile_symtab, (pst, desc),
      struct partial_symtab *pst AND
      int desc)
{
  struct cleanup *back_to;
  long lnsize;
  int foffset;
  
  /* Allocate a buffer for the entire chunk of DIE's for this compilation
     unit, seek to the location in the file, and read in all the DIE's. */

  diecount = 0;
  dbbase = xmalloc (DBLENGTH(pst));
  dbroff = DBROFF(pst);
  foffset = DBFOFF(pst) + dbroff;
  if ((lseek (desc, foffset, 0) != foffset) ||
      (read (desc, dbbase, DBLENGTH(pst)) != DBLENGTH(pst)))
    {
      free (dbbase);
      error ("can't read DWARF data");
    }
  back_to = make_cleanup (free, dbbase);

  /* If there is a line number table associated with this compilation unit
     then read the first long word from the line number table fragment, which
     contains the size of the fragment in bytes (including the long word
     itself).  Allocate a buffer for the fragment and read it in for future
     processing. */

  lnbase = NULL;
  if (LNFOFF (pst))
    {
      if ((lseek (desc, LNFOFF (pst), 0) != LNFOFF (pst)) ||
	  (read (desc, &lnsize, sizeof(long)) != sizeof(long)))
	{
	  error ("can't read DWARF line number table size");
	}
      lnbase = xmalloc (lnsize);
      if ((lseek (desc, LNFOFF (pst), 0) != LNFOFF (pst)) ||
	  (read (desc, lnbase, lnsize) != lnsize))
	{
	  free (lnbase);
	  error ("can't read DWARF line numbers");
	}
      make_cleanup (free, lnbase);
    }

  process_dies (dbbase, dbbase + DBLENGTH(pst));
  do_cleanups (back_to);
  return (symtab_list);
}

/*

LOCAL FUNCTION

	psymtab_to_symtab_1 -- do grunt work for building a full symtab entry

SYNOPSIS

	static void psymtab_to_symtab_1 (struct partial_symtab *pst, int desc)

DESCRIPTION

	Called once for each partial symbol table entry that needs to be
	expanded into a full symbol table entry.

*/

static void
DEFUN(psymtab_to_symtab_1,
      (pst, desc),
      struct partial_symtab *pst AND
      int desc)
{
  int i;
  
  if (!pst)
    {
      return;
    }
  if (pst->readin)
    {
      fprintf (stderr, "Psymtab for %s already read in.  Shouldn't happen.\n",
	       pst -> filename);
      return;
    }
  
  /* Read in all partial symtabs on which this one is dependent */
  for (i = 0; i < pst -> number_of_dependencies; i++)
    if (!pst -> dependencies[i] -> readin)
      {
	/* Inform about additional files that need to be read in.  */
	if (info_verbose)
	  {
	    fputs_filtered (" ", stdout);
	    wrap_here ("");
	    fputs_filtered ("and ", stdout);
	    wrap_here ("");
	    printf_filtered ("%s...", pst -> dependencies[i] -> filename);
	    wrap_here ("");		/* Flush output */
	    fflush (stdout);
	  }
	psymtab_to_symtab_1 (pst -> dependencies[i], desc);
      }
  
  if (DBLENGTH(pst))		/* Otherwise it's a dummy */
    {
      /* Init stuff necessary for reading in symbols */
      pst -> symtab = read_ofile_symtab (pst, desc);
      if (info_verbose)
	{
	  printf_filtered ("%d DIE's, sorting...", diecount);
	  fflush (stdout);
	}
      sort_symtab_syms (pst -> symtab);
    }
  pst -> readin = 1;
}

/*

LOCAL FUNCTION

	dwarf_psymtab_to_symtab -- build a full symtab entry from partial one

SYNOPSIS

	static void dwarf_psymtab_to_symtab (struct partial_symtab *pst)

DESCRIPTION

	This is the DWARF support entry point for building a full symbol
	table entry from a partial symbol table entry.  We are passed a
	pointer to the partial symbol table entry that needs to be expanded.

*/

static void
DEFUN(dwarf_psymtab_to_symtab, (pst), struct partial_symtab *pst)
{
  int desc;
  struct cleanup *old_chain;
  bfd *sym_bfd;
  
  if (!pst)
    {
      return;
    }
  if (pst -> readin)
    {
      fprintf (stderr, "Psymtab for %s already read in.  Shouldn't happen.\n",
	       pst -> filename);
      return;
    }
  
  if (DBLENGTH(pst) || pst -> number_of_dependencies)
    {
      /* Print the message now, before starting serious work, to avoid
	 disconcerting pauses.  */
      if (info_verbose)
	{
	  printf_filtered ("Reading in symbols for %s...", pst -> filename);
	  fflush (stdout);
	}
      
      /* Open symbol file.  Symbol_file_command guarantees that the symbol
	 file name will be absolute, so there is no need for openp.  */
      desc = open (pst -> symfile_name, O_RDONLY, 0);
      
      if (desc < 0)
	{
	  perror_with_name (pst -> symfile_name);
	}
      
      sym_bfd = bfd_fdopenr (pst -> symfile_name, NULL, desc);
      if (!sym_bfd)
	{
	  (void) close (desc);
	  error ("Could not open `%s' to read symbols: %s",
		 pst -> symfile_name, bfd_errmsg (bfd_error));
	}
      old_chain = make_cleanup (bfd_close, sym_bfd);
      if (!bfd_check_format (sym_bfd, bfd_object))
	{
	  error ("\"%s\": can't read symbols: %s.",
		 pst -> symfile_name, bfd_errmsg (bfd_error));
	}
      
      psymtab_to_symtab_1 (pst, desc);
      
#if 0 /* FIXME:  Check to see what dbxread is doing here and see if
	 we need to do an equivalent or is this something peculiar to
	 stabs/a.out format. */
      /* Match with global symbols.  This only needs to be done once,
         after all of the symtabs and dependencies have been read in.   */
      scan_file_globals ();
#endif
      
      do_cleanups (old_chain);
      
      /* Finish up the debug error message.  */
      if (info_verbose)
	{
	  printf_filtered ("done.\n");
	}
    }
}

/*

LOCAL FUNCTION

	init_psymbol_list -- initialize storage for partial symbols

SYNOPSIS

	static void init_psymbol_list (int total_symbols)

DESCRIPTION

	Initializes storage for all of the partial symbols that will be
	created by dwarf_build_psymtabs and subsidiaries.
 */

static void
DEFUN(init_psymbol_list, (total_symbols), int total_symbols)
{
  /* Free any previously allocated psymbol lists.  */
  
  if (global_psymbols.list)
    {
      free (global_psymbols.list);
    }
  if (static_psymbols.list)
    {
      free (static_psymbols.list);
    }
  
  /* Current best guess is that there are approximately a twentieth
     of the total symbols (in a debugging file) are global or static
     oriented symbols */
  
  global_psymbols.size = total_symbols / 10;
  static_psymbols.size = total_symbols / 10;
  global_psymbols.next = global_psymbols.list = (struct partial_symbol *)
    xmalloc (global_psymbols.size * sizeof (struct partial_symbol));
  static_psymbols.next = static_psymbols.list = (struct partial_symbol *)
    xmalloc (static_psymbols.size * sizeof (struct partial_symbol));
}

/*

LOCAL FUNCTION

	start_psymtab -- allocate and partially fill a partial symtab entry

DESCRIPTION

	Allocate and partially fill a partial symtab.  It will be completely
	filled at the end of the symbol list.
   
	SYMFILE_NAME is the name of the symbol-file we are reading from, and
	ADDR is the address relative to which its symbols are (incremental)
	or 0 (normal).  FILENAME is the name of the compilation unit that
	these symbols were defined in, and they appear starting a address
	TEXTLOW.  DBROFF is the absolute file offset in SYMFILE_NAME where
	the full symbols can be read for compilation unit FILENAME.
	GLOBAL_SYMS and STATIC_SYMS are pointers to the current end of the
	psymtab vector.

 */

static struct partial_symtab *
DEFUN(start_psymtab,
      (symfile_name, addr, filename, textlow, texthigh, dbfoff, curoff,
       culength, lnfoff, global_syms, static_syms),
      char *symfile_name AND
      CORE_ADDR addr AND
      char *filename AND
      CORE_ADDR textlow AND
      CORE_ADDR texthigh AND
      int dbfoff AND
      int curoff AND
      int culength AND
      int lnfoff AND
      struct partial_symbol *global_syms AND
      struct partial_symbol *static_syms)
{
  struct partial_symtab *result;

  result = (struct partial_symtab *)
      obstack_alloc (psymbol_obstack, sizeof (struct partial_symtab));
  (void) memset (result, 0, sizeof (struct partial_symtab));
  result -> addr = addr;
  result -> symfile_name =  create_name (symfile_name, psymbol_obstack);
  result -> filename = create_name (filename, psymbol_obstack);
  result -> textlow = textlow;
  result -> texthigh = texthigh;
  result -> read_symtab_private = (char *) obstack_alloc (psymbol_obstack,
						  sizeof (struct dwfinfo));
  DBFOFF (result) = dbfoff;
  DBROFF (result) = curoff;
  DBLENGTH (result) = culength;
  LNFOFF (result)  = lnfoff;
  result -> readin = 0;
  result -> symtab = NULL;
  result -> read_symtab = dwarf_psymtab_to_symtab;
  result -> globals_offset = global_syms - global_psymbols.list;
  result -> statics_offset = static_syms - static_psymbols.list;
  
  result->n_global_syms = 0;
  result->n_static_syms = 0;
  
  return result;
}

/*

LOCAL FUNCTION

	add_psymbol_to_list -- add a partial symbol to given list

DESCRIPTION

	Add a partial symbol to one of the partial symbol vectors (pointed to
	by listp).  The vector is grown as necessary.

 */

static void
DEFUN(add_psymbol_to_list,
      (listp, name, space, class, value),
      struct psymbol_allocation_list *listp AND
      char *name AND
      enum namespace space AND
      enum address_class class AND
      CORE_ADDR value)
{
  struct partial_symbol *psym;
  int newsize;
  
  if (listp -> next >= listp -> list + listp -> size)
    {
      newsize = listp -> size * 2;
      listp -> list = (struct partial_symbol *)
	xrealloc (listp -> list, (newsize * sizeof (struct partial_symbol)));
      /* Next assumes we only went one over.  Should be good if program works
	 correctly */
      listp -> next = listp -> list + listp -> size;
      listp -> size = newsize;
    }
  psym = listp -> next++;
  SYMBOL_NAME (psym) = create_name (name, psymbol_obstack);
  SYMBOL_NAMESPACE (psym) = space;
  SYMBOL_CLASS (psym) = class;
  SYMBOL_VALUE (psym) = value;
}

/*

LOCAL FUNCTION

	add_partial_symbol -- add symbol to partial symbol table

DESCRIPTION

	Given a DIE, if it is one of the types that we want to
	add to a partial symbol table, finish filling in the die info
	and then add a partial symbol table entry for it.

*/

static void
DEFUN(add_partial_symbol, (dip), struct dieinfo *dip)
{
  switch (dip -> dietag)
    {
    case TAG_global_subroutine:
      record_misc_function (dip -> at_name, dip -> at_low_pc);
      add_psymbol_to_list (&global_psymbols, dip -> at_name, VAR_NAMESPACE,
			   LOC_BLOCK, dip -> at_low_pc);
      break;
    case TAG_global_variable:
      add_psymbol_to_list (&global_psymbols, dip -> at_name, VAR_NAMESPACE,
			   LOC_STATIC, 0);
      break;
    case TAG_subroutine:
      add_psymbol_to_list (&static_psymbols, dip -> at_name, VAR_NAMESPACE,
			   LOC_BLOCK, dip -> at_low_pc);
      break;
    case TAG_local_variable:
      add_psymbol_to_list (&static_psymbols, dip -> at_name, VAR_NAMESPACE,
			   LOC_STATIC, 0);
      break;
    case TAG_typedef:
      add_psymbol_to_list (&static_psymbols, dip -> at_name, VAR_NAMESPACE,
			   LOC_TYPEDEF, 0);
      break;
    case TAG_structure_type:
    case TAG_union_type:
    case TAG_enumeration_type:
      add_psymbol_to_list (&static_psymbols, dip -> at_name, STRUCT_NAMESPACE,
			   LOC_TYPEDEF, 0);
      break;
    }
}

/*

LOCAL FUNCTION

	scan_partial_symbols -- scan DIE's within a single compilation unit

DESCRIPTION

	Process the DIE's within a single compilation unit, looking for
	interesting DIE's that contribute to the partial symbol table entry
	for this compilation unit.  Since we cannot follow any sibling
	chains without reading the complete DIE info for every DIE,
	it is probably faster to just sequentially check each one to
	see if it is one of the types we are interested in, and if
	so, then extracting all the attributes info and generating a
	partial symbol table entry.

 */

static void
DEFUN(scan_partial_symbols, (thisdie, enddie), char *thisdie AND char *enddie)
{
  char *nextdie;
  struct dieinfo di;
  
  while (thisdie < enddie)
    {
      basicdieinfo (&di, thisdie);
      if (di.dielength < sizeof (long))
	{
	  break;
	}
      else
	{
	  nextdie = thisdie + di.dielength;
	  switch (di.dietag)
	    {
	    case TAG_global_subroutine:
	    case TAG_global_variable:
	    case TAG_subroutine:
	    case TAG_local_variable:
	    case TAG_typedef:
	    case TAG_structure_type:
	    case TAG_union_type:
	    case TAG_enumeration_type:
	      completedieinfo (&di);
	      /* Don't attempt to add anonymous structures, unions, or
		 enumerations since they have no name.  Also check that
		 this is the place where the actual definition occurs,
		 rather than just a reference to an external. */
	      if (di.at_name != NULL && !di.at_is_external_p)
		{
		  add_partial_symbol (&di);
		}
	      break;
	    }
	}
      thisdie = nextdie;
    }
}

/*

LOCAL FUNCTION

	scan_compilation_units -- build a psymtab entry for each compilation

DESCRIPTION

	This is the top level dwarf parsing routine for building partial
	symbol tables.

	It scans from the beginning of the DWARF table looking for the first
	TAG_compile_unit DIE, and then follows the sibling chain to locate
	each additional TAG_compile_unit DIE.
   
	For each TAG_compile_unit DIE it creates a partial symtab structure,
	calls a subordinate routine to collect all the compilation unit's
	global DIE's, file scope DIEs, typedef DIEs, etc, and then links the
	new partial symtab structure into the partial symbol table.  It also
	records the appropriate information in the partial symbol table entry
	to allow the chunk of DIE's and line number table for this compilation
	unit to be located and re-read later, to generate a complete symbol
	table entry for the compilation unit.

	Thus it effectively partitions up a chunk of DIE's for multiple
	compilation units into smaller DIE chunks and line number tables,
	and associates them with a partial symbol table entry.

NOTES

	If any compilation unit has no line number table associated with
	it for some reason (a missing at_stmt_list attribute, rather than
	just one with a value of zero, which is valid) then we ensure that
	the recorded file offset is zero so that the routine which later
	reads line number table fragments knows that there is no fragment
	to read.

RETURNS

	Returns no value.

 */

static void
DEFUN(scan_compilation_units,
      (filename, addr, thisdie, enddie, dbfoff, lnoffset),
      char *filename AND
      CORE_ADDR addr AND
      char *thisdie AND
      char *enddie AND
      unsigned int dbfoff AND
      unsigned int lnoffset)
{
  char *nextdie;
  struct dieinfo di;
  struct partial_symtab *pst;
  int culength;
  int curoff;
  int curlnoffset;

  while (thisdie < enddie)
    {
      basicdieinfo (&di, thisdie);
      if (di.dielength < sizeof (long))
	{
	  break;
	}
      else if (di.dietag != TAG_compile_unit)
	{
	  nextdie = thisdie + di.dielength;
	}
      else
	{
	  completedieinfo (&di);
	  if (di.at_sibling != 0)
	    {
	      nextdie = dbbase + di.at_sibling - dbroff;
	    }
	  else
	    {
	      nextdie = thisdie + di.dielength;
	    }
	  curoff = thisdie - dbbase;
	  culength = nextdie - thisdie;
	  curlnoffset = di.at_stmt_list_p ? lnoffset + di.at_stmt_list : 0;
	  pst = start_psymtab (filename, addr, di.at_name,
				     di.at_low_pc, di.at_high_pc,
				     dbfoff, curoff, culength, curlnoffset,
				     global_psymbols.next,
				     static_psymbols.next);
	  scan_partial_symbols (thisdie + di.dielength, nextdie);
	  pst -> n_global_syms = global_psymbols.next -
	    (global_psymbols.list + pst -> globals_offset);
	  pst -> n_static_syms = static_psymbols.next - 
	    (static_psymbols.list + pst -> statics_offset);
	  /* Sort the global list; don't sort the static list */
	  qsort (global_psymbols.list + pst -> globals_offset,
		 pst -> n_global_syms, sizeof (struct partial_symbol),
		 compare_psymbols);
	  /* If there is already a psymtab or symtab for a file of this name,
	     remove it. (If there is a symtab, more drastic things also
	     happen.)  This happens in VxWorks.  */
	  free_named_symtabs (pst -> filename);
	  /* Place the partial symtab on the partial symtab list */
	  pst -> next = partial_symtab_list;
	  partial_symtab_list = pst;
	}
      thisdie = nextdie;      
    }
}

/*

LOCAL FUNCTION

	new_symbol -- make a symbol table entry for a new symbol

SYNOPSIS

	static struct symbol *new_symbol (struct dieinfo *dip)

DESCRIPTION

	Given a pointer to a DWARF information entry, figure out if we need
	to make a symbol table entry for it, and if so, create a new entry
	and return a pointer to it.
 */

static struct symbol *
DEFUN(new_symbol, (dip), struct dieinfo *dip)
{
  struct symbol *sym = NULL;
  
  if (dip -> at_name != NULL)
    {
      sym = (struct symbol *) obstack_alloc (symbol_obstack,
					     sizeof (struct symbol));
      (void) memset (sym, 0, sizeof (struct symbol));
      SYMBOL_NAME (sym) = create_name (dip -> at_name, symbol_obstack);
      /* default assumptions */
      SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
      SYMBOL_CLASS (sym) = LOC_STATIC;
      SYMBOL_TYPE (sym) = decode_die_type (dip);
      switch (dip -> dietag)
	{
	case TAG_label:
	  SYMBOL_VALUE (sym) = dip -> at_low_pc + baseaddr;
	  SYMBOL_CLASS (sym) = LOC_LABEL;
	  break;
	case TAG_global_subroutine:
	case TAG_subroutine:
	  SYMBOL_VALUE (sym) = dip -> at_low_pc + baseaddr;
	  SYMBOL_TYPE (sym) = lookup_function_type (SYMBOL_TYPE (sym));
	  SYMBOL_CLASS (sym) = LOC_BLOCK;
	  if (dip -> dietag == TAG_global_subroutine)
	    {
	      add_symbol_to_list (sym, &global_symbols);
	    }
	  else
	    {
	      add_symbol_to_list (sym, &scope -> symbols);
	    }
	  break;
	case TAG_global_variable:
	case TAG_local_variable:
	  if (dip -> at_location != NULL)
	    {
	      SYMBOL_VALUE (sym) = locval (dip -> at_location);
	    }
	  if (dip -> dietag == TAG_global_variable)
	    {
	      add_symbol_to_list (sym, &global_symbols);
	      SYMBOL_CLASS (sym) = LOC_STATIC;
	      SYMBOL_VALUE (sym) += baseaddr;
	    }
	  else
	    {
	      add_symbol_to_list (sym, &scope -> symbols);
	      if (scope -> parent != NULL)
		{
		  if (isreg)
		    {
		      SYMBOL_CLASS (sym) = LOC_REGISTER;
		    }
		  else
		    {
		      SYMBOL_CLASS (sym) = LOC_LOCAL;
		    }
		}
	      else
		{
		  SYMBOL_CLASS (sym) = LOC_STATIC;
		  SYMBOL_VALUE (sym) += baseaddr;
		}
	    }
	  break;
	case TAG_formal_parameter:
	  if (dip -> at_location != NULL)
	    {
	      SYMBOL_VALUE (sym) = locval (dip -> at_location);
	    }
	  add_symbol_to_list (sym, &scope -> symbols);
	  if (isreg)
	    {
	      SYMBOL_CLASS (sym) = LOC_REGPARM;
	    }
	  else
	    {
	      SYMBOL_CLASS (sym) = LOC_ARG;
	    }
	  break;
	case TAG_unspecified_parameters:
	  /* From varargs functions; gdb doesn't seem to have any interest in
	     this information, so just ignore it for now. (FIXME?) */
	  break;
	case TAG_structure_type:
	case TAG_union_type:
	case TAG_enumeration_type:
	  SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	  SYMBOL_NAMESPACE (sym) = STRUCT_NAMESPACE;
	  add_symbol_to_list (sym, &scope -> symbols);
	  break;
	case TAG_typedef:
	  SYMBOL_CLASS (sym) = LOC_TYPEDEF;
	  SYMBOL_NAMESPACE (sym) = VAR_NAMESPACE;
	  add_symbol_to_list (sym, &scope -> symbols);
	  break;
	default:
	  /* Not a tag we recognize.  Hopefully we aren't processing trash
	     data, but since we must specifically ignore things we don't
	     recognize, there is nothing else we should do at this point. */
	  break;
	}
    }
  return (sym);
}

/*

LOCAL FUNCTION

	decode_mod_fund_type -- decode a modified fundamental type

SYNOPSIS

	static struct type *decode_mod_fund_type (char *typedata)

DESCRIPTION

	Decode a block of data containing a modified fundamental
	type specification.  TYPEDATA is a pointer to the block,
	which consists of a two byte length, containing the size
	of the rest of the block.  At the end of the block is a
	two byte value that gives the fundamental type.  Everything
	in between are type modifiers.

	We simply compute the number of modifiers and call the general
	function decode_modified_type to do the actual work.
*/

static struct type *
DEFUN(decode_mod_fund_type, (typedata), char *typedata)
{
  struct type *typep = NULL;
  unsigned short modcount;
  unsigned char *modifiers;
  
  /* Get the total size of the block, exclusive of the size itself */
  (void) memcpy (&modcount, typedata, sizeof (short));
  /* Deduct the size of the fundamental type bytes at the end of the block. */
  modcount -= sizeof (short);
  /* Skip over the two size bytes at the beginning of the block. */
  modifiers = (unsigned char *) typedata + sizeof (short);
  /* Now do the actual decoding */
  typep = decode_modified_type (modifiers, modcount, AT_mod_fund_type);
  return (typep);
}

/*

LOCAL FUNCTION

	decode_mod_u_d_type -- decode a modified user defined type

SYNOPSIS

	static struct type *decode_mod_u_d_type (char *typedata)

DESCRIPTION

	Decode a block of data containing a modified user defined
	type specification.  TYPEDATA is a pointer to the block,
	which consists of a two byte length, containing the size
	of the rest of the block.  At the end of the block is a
	four byte value that gives a reference to a user defined type.
	Everything in between are type modifiers.

	We simply compute the number of modifiers and call the general
	function decode_modified_type to do the actual work.
*/

static struct type *
DEFUN(decode_mod_u_d_type, (typedata), char *typedata)
{
  struct type *typep = NULL;
  unsigned short modcount;
  unsigned char *modifiers;
  
  /* Get the total size of the block, exclusive of the size itself */
  (void) memcpy (&modcount, typedata, sizeof (short));
  /* Deduct the size of the reference type bytes at the end of the block. */
  modcount -= sizeof (long);
  /* Skip over the two size bytes at the beginning of the block. */
  modifiers = (unsigned char *) typedata + sizeof (short);
  /* Now do the actual decoding */
  typep = decode_modified_type (modifiers, modcount, AT_mod_u_d_type);
  return (typep);
}

/*

LOCAL FUNCTION

	decode_modified_type -- decode modified user or fundamental type

SYNOPSIS

	static struct type *decode_modified_type (unsigned char *modifiers,
	    unsigned short modcount, int mtype)

DESCRIPTION

	Decode a modified type, either a modified fundamental type or
	a modified user defined type.  MODIFIERS is a pointer to the
	block of bytes that define MODCOUNT modifiers.  Immediately
	following the last modifier is a short containing the fundamental
	type or a long containing the reference to the user defined
	type.  Which one is determined by MTYPE, which is either
	AT_mod_fund_type or AT_mod_u_d_type to indicate what modified
	type we are generating.

	We call ourself recursively to generate each modified type,`
	until MODCOUNT reaches zero, at which point we have consumed
	all the modifiers and generate either the fundamental type or
	user defined type.  When the recursion unwinds, each modifier
	is applied in turn to generate the full modified type.

NOTES

	If we find a modifier that we don't recognize, and it is not one
	of those reserved for application specific use, then we issue a
	warning and simply ignore the modifier.

BUGS

	We currently ignore MOD_const and MOD_volatile.  (FIXME)

 */

static struct type *
DEFUN(decode_modified_type,
      (modifiers, modcount, mtype),
      unsigned char *modifiers AND unsigned short modcount AND int mtype)
{
  struct type *typep = NULL;
  unsigned short fundtype;
  DIEREF dieref;
  unsigned char modifier;
  
  if (modcount == 0)
    {
      switch (mtype)
	{
	case AT_mod_fund_type:
	  (void) memcpy (&fundtype, modifiers, sizeof (short));
	  typep = decode_fund_type (fundtype);
	  break;
	case AT_mod_u_d_type:
	  (void) memcpy (&dieref, modifiers, sizeof (DIEREF));
	  if ((typep = lookup_utype (dieref)) == NULL)
	    {
	      typep = alloc_utype (dieref, NULL);
	    }
	  break;
	default:
	  SQUAWK (("botched modified type decoding (mtype 0x%x)", mtype));
	  typep = builtin_type_int;
	  break;
	}
    }
  else
    {
      modifier = *modifiers++;
      typep = decode_modified_type (modifiers, --modcount, mtype);
      switch (modifier)
	{
	case MOD_pointer_to:
	  typep = lookup_pointer_type (typep);
	  break;
	case MOD_reference_to:
	  typep = lookup_reference_type (typep);
	  break;
	case MOD_const:
	  SQUAWK (("type modifier 'const' ignored"));	/* FIXME */
	  break;
	case MOD_volatile:
	  SQUAWK (("type modifier 'volatile' ignored"));	/* FIXME */
	  break;
	default:
	  if (!(MOD_lo_user <= modifier && modifier <= MOD_hi_user))
	    {
	      SQUAWK (("unknown type modifier %u", modifier));
	    }
	  break;
	}
    }
  return (typep);
}

/*

LOCAL FUNCTION

	decode_fund_type -- translate basic DWARF type to gdb base type

DESCRIPTION

	Given an integer that is one of the fundamental DWARF types,
	translate it to one of the basic internal gdb types and return
	a pointer to the appropriate gdb type (a "struct type *").

NOTES

	If we encounter a fundamental type that we are unprepared to
	deal with, and it is not in the range of those types defined
	as application specific types, then we issue a warning and
	treat the type as builtin_type_int.
*/

static struct type *
DEFUN(decode_fund_type, (fundtype), unsigned short fundtype)
{
  struct type *typep = NULL;
  
  switch (fundtype)
    {

    case FT_void:
      typep = builtin_type_void;
      break;
    
    case FT_pointer:		/* (void *) */
      typep = lookup_pointer_type (builtin_type_void);
      break;
    
    case FT_char:
    case FT_signed_char:
      typep = builtin_type_char;
      break;
    
    case FT_short:
    case FT_signed_short:
      typep = builtin_type_short;
      break;
    
    case FT_integer:
    case FT_signed_integer:
    case FT_boolean:		/* Was FT_set in AT&T version */
      typep = builtin_type_int;
      break;
    
    case FT_long:
    case FT_signed_long:
      typep = builtin_type_long;
      break;
    
    case FT_float:
      typep = builtin_type_float;
      break;
    
    case FT_dbl_prec_float:
      typep = builtin_type_double;
      break;
    
    case FT_unsigned_char:
      typep = builtin_type_unsigned_char;
      break;
    
    case FT_unsigned_short:
      typep = builtin_type_unsigned_short;
      break;
    
    case FT_unsigned_integer:
      typep = builtin_type_unsigned_int;
      break;
    
    case FT_unsigned_long:
      typep = builtin_type_unsigned_long;
      break;
    
    case FT_ext_prec_float:
      typep = builtin_type_long_double;
      break;
    
    case FT_complex:
      typep = builtin_type_complex;
      break;
    
    case FT_dbl_prec_complex:
      typep = builtin_type_double_complex;
      break;
    
    case FT_long_long:
    case FT_signed_long_long:
      typep = builtin_type_long_long;
      break;

    case FT_unsigned_long_long:
      typep = builtin_type_unsigned_long_long;
      break;

    }

  if ((typep == NULL) && !(FT_lo_user <= fundtype && fundtype <= FT_hi_user))
    {
      SQUAWK (("unexpected fundamental type 0x%x", fundtype));
      typep = builtin_type_void;
    }
    
  return (typep);
}

/*

LOCAL FUNCTION

	create_name -- allocate a fresh copy of a string on an obstack

DESCRIPTION

	Given a pointer to a string and a pointer to an obstack, allocates
	a fresh copy of the string on the specified obstack.

*/

static char *
DEFUN(create_name, (name, obstackp), char *name AND struct obstack *obstackp)
{
  int length;
  char *newname;

  length = strlen (name) + 1;
  newname = (char *) obstack_alloc (obstackp, length);
  (void) strcpy (newname, name);
  return (newname);
}

/*

LOCAL FUNCTION

	basicdieinfo -- extract the minimal die info from raw die data

SYNOPSIS

	void basicdieinfo (char *diep, struct dieinfo *dip)

DESCRIPTION

	Given a pointer to raw DIE data, and a pointer to an instance of a
	die info structure, this function extracts the basic information
	from the DIE data required to continue processing this DIE, along
	with some bookkeeping information about the DIE.

	The information we absolutely must have includes the DIE tag,
	and the DIE length.  If we need the sibling reference, then we
	will have to call completedieinfo() to process all the remaining
	DIE information.

	Note that since there is no guarantee that the data is properly
	aligned in memory for the type of access required (indirection
	through anything other than a char pointer), we use memcpy to
	shuffle data items larger than a char.  Possibly inefficient, but
	quite portable.

	We also take care of some other basic things at this point, such
	as ensuring that the instance of the die info structure starts
	out completely zero'd and that curdie is initialized for use
	in error reporting if we have a problem with the current die.

NOTES

	All DIE's must have at least a valid length, thus the minimum
	DIE size is sizeof (long).  In order to have a valid tag, the
	DIE size must be at least sizeof (short) larger, otherwise they
	are forced to be TAG_padding DIES.

	Padding DIES must be at least sizeof(long) in length, implying that
	if a padding DIE is used for alignment and the amount needed is less
	than sizeof(long) then the padding DIE has to be big enough to align
	to the next alignment boundry.
 */

static void
DEFUN(basicdieinfo, (dip, diep), struct dieinfo *dip AND char *diep)
{
  curdie = dip;
  (void) memset (dip, 0, sizeof (struct dieinfo));
  dip -> die = diep;
  dip -> dieref = dbroff + (diep - dbbase);
  (void) memcpy (&dip -> dielength, diep, sizeof (long));
  if (dip -> dielength < sizeof (long))
    {
      dwarfwarn ("malformed DIE, bad length (%d bytes)", dip -> dielength);
    }
  else if (dip -> dielength < (sizeof (long) + sizeof (short)))
    {
      dip -> dietag = TAG_padding;
    }
  else
    {
      (void) memcpy (&dip -> dietag, diep + sizeof (long), sizeof (short));
    }
}

/*

LOCAL FUNCTION

	completedieinfo -- finish reading the information for a given DIE

SYNOPSIS

	void completedieinfo (struct dieinfo *dip)

DESCRIPTION

	Given a pointer to an already partially initialized die info structure,
	scan the raw DIE data and finish filling in the die info structure
	from the various attributes found.
   
	Note that since there is no guarantee that the data is properly
	aligned in memory for the type of access required (indirection
	through anything other than a char pointer), we use memcpy to
	shuffle data items larger than a char.  Possibly inefficient, but
	quite portable.

NOTES

	Each time we are called, we increment the diecount variable, which
	keeps an approximate count of the number of dies processed for
	each compilation unit.  This information is presented to the user
	if the info_verbose flag is set.

 */

static void
DEFUN(completedieinfo, (dip), struct dieinfo *dip)
{
  char *diep;			/* Current pointer into raw DIE data */
  char *end;			/* Terminate DIE scan here */
  unsigned short attr;		/* Current attribute being scanned */
  unsigned short form;		/* Form of the attribute */
  short block2sz;		/* Size of a block2 attribute field */
  long block4sz;		/* Size of a block4 attribute field */
  
  diecount++;
  diep = dip -> die;
  end = diep + dip -> dielength;
  diep += sizeof (long) + sizeof (short);
  while (diep < end)
    {
      (void) memcpy (&attr, diep, sizeof (short));
      diep += sizeof (short);
      switch (attr)
	{
	case AT_fund_type:
	  (void) memcpy (&dip -> at_fund_type, diep, sizeof (short));
	  break;
	case AT_ordering:
	  (void) memcpy (&dip -> at_ordering, diep, sizeof (short));
	  break;
	case AT_bit_offset:
	  (void) memcpy (&dip -> at_bit_offset, diep, sizeof (short));
	  break;
	case AT_visibility:
	  (void) memcpy (&dip -> at_visibility, diep, sizeof (short));
	  break;
	case AT_sibling:
	  (void) memcpy (&dip -> at_sibling, diep, sizeof (long));
	  break;
	case AT_stmt_list:
	  (void) memcpy (&dip -> at_stmt_list, diep, sizeof (long));
	  dip -> at_stmt_list_p = 1;
	  break;
	case AT_low_pc:
	  (void) memcpy (&dip -> at_low_pc, diep, sizeof (long));
	  break;
	case AT_high_pc:
	  (void) memcpy (&dip -> at_high_pc, diep, sizeof (long));
	  break;
	case AT_language:
	  (void) memcpy (&dip -> at_language, diep, sizeof (long));
	  break;
	case AT_user_def_type:
	  (void) memcpy (&dip -> at_user_def_type, diep, sizeof (long));
	  break;
	case AT_byte_size:
	  (void) memcpy (&dip -> at_byte_size, diep, sizeof (long));
	  break;
	case AT_bit_size:
	  (void) memcpy (&dip -> at_bit_size, diep, sizeof (long));
	  break;
	case AT_member:
	  (void) memcpy (&dip -> at_member, diep, sizeof (long));
	  break;
	case AT_discr:
	  (void) memcpy (&dip -> at_discr, diep, sizeof (long));
	  break;
	case AT_import:
	  (void) memcpy (&dip -> at_import, diep, sizeof (long));
	  break;
	case AT_location:
	  dip -> at_location = diep;
	  break;
	case AT_mod_fund_type:
	  dip -> at_mod_fund_type = diep;
	  break;
	case AT_subscr_data:
	  dip -> at_subscr_data = diep;
	  break;
	case AT_mod_u_d_type:
	  dip -> at_mod_u_d_type = diep;
	  break;
	case AT_deriv_list:
	  dip -> at_deriv_list = diep;
	  break;
	case AT_element_list:
	  dip -> at_element_list = diep;
	  break;
	case AT_discr_value:
	  dip -> at_discr_value = diep;
	  break;
	case AT_string_length:
	  dip -> at_string_length = diep;
	  break;
	case AT_name:
	  dip -> at_name = diep;
	  break;
	case AT_comp_dir:
	  dip -> at_comp_dir = diep;
	  break;
	case AT_producer:
	  dip -> at_producer = diep;
	  break;
	case AT_loclist:
	  (void) memcpy (&dip -> at_loclist, diep, sizeof (long));
	  break;
	case AT_frame_base:
	  (void) memcpy (&dip -> at_frame_base, diep, sizeof (long));
	  break;
	case AT_incomplete:
	  (void) memcpy (&dip -> at_incomplete, diep, sizeof (short));
	  break;
	case AT_start_scope:
	  (void) memcpy (&dip -> at_start_scope, diep, sizeof (long));
	  break;
	case AT_stride_size:
	  (void) memcpy (&dip -> at_stride_size, diep, sizeof (long));
	  break;
	case AT_src_info:
	  (void) memcpy (&dip -> at_src_info, diep, sizeof (long));
	  break;
	case AT_prototyped:
	  (void) memcpy (&dip -> at_prototyped, diep, sizeof (short));
	  break;
	case AT_const_data:
	  dip -> at_const_data = diep;
	  break;
	case AT_is_external:
	  (void) memcpy (&dip -> at_is_external, diep, sizeof (short));
	  dip -> at_is_external_p = 1;
	  break;
	default:
	  /* Found an attribute that we are unprepared to handle.  However
	     it is specifically one of the design goals of DWARF that
	     consumers should ignore unknown attributes.  As long as the
	     form is one that we recognize (so we know how to skip it),
	     we can just ignore the unknown attribute. */
	  break;
	}
      form = attr & 0xF;
      switch (form)
	{
	case FORM_DATA2:
	  diep += sizeof (short);
	  break;
	case FORM_DATA4:
	  diep += sizeof (long);
	  break;
	case FORM_DATA8:
	  diep += 8 * sizeof (char);	/* sizeof (long long) ? */
	  break;
	case FORM_ADDR:
	case FORM_REF:
	  diep += sizeof (long);
	  break;
	case FORM_BLOCK2:
	  (void) memcpy (&block2sz, diep, sizeof (short));
	  block2sz += sizeof (short);
	  diep += block2sz;
	  break;
	case FORM_BLOCK4:
	  (void) memcpy (&block4sz, diep, sizeof (long));
	  block4sz += sizeof (long);
	  diep += block4sz;
	  break;
	case FORM_STRING:
	  diep += strlen (diep) + 1;
	  break;
	default:
	  SQUAWK (("unknown attribute form (0x%x), skipped rest", form));
	  diep = end;
	  break;
	}
    }
}
