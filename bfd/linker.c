/* linker.c -- BFD linker routines
   Copyright 1993 Free Software Foundation, Inc.
   Written by Steve Chamberlain and Ian Lance Taylor, Cygnus Support

This file is part of BFD

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "genlink.h"

/*
SECTION
	Linker Functions

@cindex Linker
	The linker uses three special entry points in the BFD target
	vector.  It is not necessary to write special routines for
	these entry points when creating a new BFD back end, since
	generic versions are provided.  However, writing them can
	speed up linking and make it use significantly less runtime
	memory.

	The first routine creates a hash table used by the other
	routines.  The second routine adds the symbols from an object
	file to the hash table.  The third routine takes all the
	object files and links them together to create the output
	file.  These routines are designed so that the linker proper
	does not need to know anything about the symbols in the object
	files that it is linking.  The linker merely arranges the
	sections as directed by the linker script and lets BFD handle
	the details of symbols and relocs.

	The second routine and third routines are passed a pointer to
	a <<struct bfd_link_info>> structure (defined in
	<<bfdlink.h>>) which holds information relevant to the link,
	including the linker hash table (which was created by the
	first routine) and a set of callback functions to the linker
	proper.

	The generic linker routines are in <<linker.c>>, and use the
	header file <<genlink.h>>.  As of this writing, the only back
	ends which have implemented versions of these routines are
	a.out (in <<aoutx.h>>) and ECOFF (in <<ecoff.c>>).  The a.out
	routines are used as examples throughout this section.

@menu	
@* Creating a Linker Hash Table::
@* Adding Symbols to the Hash Table::
@* Performing the Final Link::
@end menu

INODE
Creating a Linker Hash Table, Adding Symbols to the Hash Table, Linker Functions, Linker Functions
SUBSECTION
	Creating a linker hash table

@cindex _bfd_link_hash_table_create in target vector
@cindex target vector (_bfd_link_hash_table_create)
	The linker routines must create a hash table, which must be
	derived from <<struct bfd_link_hash_table>> described in
	<<bfdlink.c>>.  @xref{Hash Tables} for information on how to
	create a derived hash table.  This entry point is called using
	the target vector of the linker output file.

	The <<_bfd_link_hash_table_create>> entry point must allocate
	and initialize an instance of the desired hash table.  If the
	back end does not require any additional information to be
	stored with the entries in the hash table, the entry point may
	simply create a <<struct bfd_link_hash_table>>.  Most likely,
	however, some additional information will be needed.

	For example, with each entry in the hash table the a.out
	linker keeps the index the symbol has in the final output file
	(this index number is used so that when doing a relocateable
	link the symbol index used in the output file can be quickly
	filled in when copying over a reloc).  The a.out linker code
	defines the required structures and functions for a hash table
	derived from <<struct bfd_link_hash_table>>.  The a.out linker
	hash table is created by the function
	<<NAME(aout,link_hash_table_create)>>; it simply allocates
	space for the hash table, initializes it, and returns a
	pointer to it.

	When writing the linker routines for a new back end, you will
	generally not know exactly which fields will be required until
	you have finished.  You should simply create a new hash table
	which defines no additional fields, and then simply add fields
	as they become necessary.

INODE
Adding Symbols to the Hash Table, Performing the Final Link, Creating a Linker Hash Table, Linker Functions
SUBSECTION
	Adding symbols to the hash table

@cindex _bfd_link_add_symbols in target vector
@cindex target vector (_bfd_link_add_symbols)
	The linker proper will call the <<_bfd_link_add_symbols>>
	entry point for each object file or archive which is to be
	linked (typically these are the files named on the command
	line, but some may also come from the linker script).  The
	entry point is responsible for examining the file.  For an
	object file, BFD must add any relevant symbol information to
	the hash table.  For an archive, BFD must determine which
	elements of the archive should be used and adding them to the
	link.

	The a.out version of this entry point is
	<<NAME(aout,link_add_symbols)>>.

@menu
@* Differing file formats::
@* Adding symbols from an object file::
@* Adding symbols from an archive::
@end menu

INODE
Differing file formats, Adding symbols from an object file, Adding Symbols to the Hash Table, Adding Symbols to the Hash Table
SUBSUBSECTION
	Differing file formats

	Normally all the files involved in a link will be of the same
	format, but it is also possible to link together different
	format object files, and the back end must support that.  The
	<<_bfd_link_add_symbols>> entry point is called via the target
	vector of the file to be added.  This has an important
	consequence: the function may not assume that the hash table
	is the type created by the corresponding
	<<_bfd_link_hash_table_create>> vector.  All the
	<<_bfd_link_add_symbols>> function can assume about the hash
	table is that it is derived from <<struct
	bfd_link_hash_table>>.

	Sometimes the <<_bfd_link_add_symbols>> function must store
	some information in the hash table entry to be used by the
	<<_bfd_final_link>> function.  In such a case the <<creator>>
	field of the hash table must be checked to make sure that the
	hash table was created by an object file of the same format.

	The <<_bfd_final_link>> routine must be prepared to handle a
	hash entry without any extra information added by the
	<<_bfd_link_add_symbols>> function.  A hash entry without
	extra information will also occur when the linker script
	directs the linker to create a symbol.  Note that, regardless
	of how a hash table entry is added, all the fields will be
	initialized to some sort of null value by the hash table entry
	initialization function.

	See <<ecoff_link_add_externals>> for an example of how to
	check the <<creator>> field before saving information (in this
	case, the ECOFF external symbol debugging information) in a
	hash table entry.

INODE
Adding symbols from an object file, Adding symbols from an archive, Differing file formats, Adding Symbols to the Hash Table
SUBSUBSECTION
	Adding symbols from an object file

	When the <<_bfd_link_add_symbols>> routine is passed an object
	file, it must add all externally visible symbols in that
	object file to the hash table.  The actual work of adding the
	symbol to the hash table is normally handled by the function
	<<_bfd_generic_link_add_one_symbol>>.  The
	<<_bfd_link_add_symbols>> routine is responsible for reading
	all the symbols from the object file and passing the correct
	information to <<_bfd_generic_link_add_one_symbol>>.

	The <<_bfd_link_add_symbols>> routine should not use
	<<bfd_canonicalize_symtab>> to read the symbols.  The point of
	providing this routine is to avoid the overhead of converting
	the symbols into generic <<asymbol>> structures.

@findex _bfd_generic_link_add_one_symbol
	<<_bfd_generic_link_add_one_symbol>> handles the details of
	combining common symbols, warning about multiple definitions,
	and so forth.  It takes arguments which describe the symbol to
	add, notably symbol flags, a section, and an offset.  The
	symbol flags include such things as <<BSF_WEAK>> or
	<<BSF_INDIRECT>>.  The section is a section in the object
	file, or something like <<bfd_und_section>> for an undefined
	symbol or <<bfd_com_section>> for a common symbol.

	If the <<_bfd_final_link>> routine is also going to need to
	read the symbol information, the <<_bfd_link_add_symbols>>
	routine should save it somewhere attached to the object file
	BFD.  However, the information should only be saved if the
	<<keep_memory>> field of the <<info>> argument is true, so
	that the <<-no-keep-memory>> linker switch is effective.

	The a.out function which adds symbols from an object file is
	<<aout_link_add_object_symbols>>, and most of the interesting
	work is in <<aout_link_add_symbols>>.  The latter saves
	pointers to the hash tables entries created by
	<<_bfd_generic_link_add_one_symbol>> indexed by symbol number,
	so that the <<_bfd_final_link>> routine does not have to call
	the hash table lookup routine to locate the entry.

INODE
Adding symbols from an archive, , Adding symbols from an object file, Adding Symbols to the Hash Table
SUBSUBSECTION
	Adding symbols from an archive

	When the <<_bfd_link_add_symbols>> routine is passed an
	archive, it must look through the symbols defined by the
	archive and decide which elements of the archive should be
	included in the link.  For each such element it must call the
	<<add_archive_element>> linker callback, and it must add the
	symbols from the object file to the linker hash table.

@findex _bfd_generic_link_add_archive_symbols
	In most cases the work of looking through the symbols in the
	archive should be done by the
	<<_bfd_generic_link_add_archive_symbols>> function.  This
	function builds a hash table from the archive symbol table and
	looks through the list of undefined symbols to see which
	elements should be included.
	<<_bfd_generic_link_add_archive_symbols>> is passed a function
	to call to make the final decision about adding an archive
	element to the link and to do the actual work of adding the
	symbols to the linker hash table.

	The function passed to
	<<_bfd_generic_link_add_archive_symbols>> must read the
	symbols of the archive element and decide whether the archive
	element should be included in the link.  If the element is to
	be included, the <<add_archive_element>> linker callback
	routine must be called with the element as an argument, and
	the elements symbols must be added to the linker hash table
	just as though the element had itself been passed to the
	<<_bfd_link_add_symbols>> function.

	When the a.out <<_bfd_link_add_symbols>> function receives an
	archive, it calls <<_bfd_generic_link_add_archive_symbols>>
	passing <<aout_link_check_archive_element>> as the function
	argument. <<aout_link_check_archive_element>> calls
	<<aout_link_check_ar_symbols>>.  If the latter decides to add
	the element (an element is only added if it provides a real,
	non-common, definition for a previously undefined or common
	symbol) it calls the <<add_archive_element>> callback and then
	<<aout_link_check_archive_element>> calls
	<<aout_link_add_symbols>> to actually add the symbols to the
	linker hash table.

	The ECOFF back end is unusual in that it does not normally
	call <<_bfd_generic_link_add_archive_symbols>>, because ECOFF
	archives already contain a hash table of symbols.  The ECOFF
	back end searches the archive itself to avoid the overhead of
	creating a new hash table.

INODE
Performing the Final Link, , Adding Symbols to the Hash Table, Linker Functions
SUBSECTION
	Performing the final link

@cindex _bfd_link_final_link in target vector
@cindex target vector (_bfd_final_link)
	When all the input files have been processed, the linker calls
	the <<_bfd_final_link>> entry point of the output BFD.  This
	routine is responsible for producing the final output file,
	which has several aspects.  It must relocate the contents of
	the input sections and copy the data into the output sections.
	It must build an output symbol table including any local
	symbols from the input files and the global symbols from the
	hash table.  When producing relocateable output, it must
	modify the input relocs and write them into the output file.
	There may also be object format dependent work to be done.

	The linker will also call the <<write_object_contents>> entry
	point when the BFD is closed.  The two entry points must work
	together in order to produce the correct output file.

	The details of how this works are inevitably dependent upon
	the specific object file format.  The a.out
	<<_bfd_final_link>> routine is <<NAME(aout,final_link)>>.

@menu
@* Information provided by the linker::
@* Relocating the section contents::
@* Writing the symbol table::
@end menu

INODE
Information provided by the linker, Relocating the section contents, Performing the Final Link, Performing the Final Link
SUBSUBSECTION
	Information provided by the linker

	Before the linker calls the <<_bfd_final_link>> entry point,
	it sets up some data structures for the function to use.

	The <<input_bfds>> field of the <<bfd_link_info>> structure
	will point to a list of all the input files included in the
	link.  These files are linked through the <<link_next>> field
	of the <<bfd>> structure.

	Each section in the output file will have a list of
	<<link_order>> structures attached to the <<link_order_head>>
	field (the <<link_order>> structure is defined in
	<<bfdlink.h>>).  These structures describe how to create the
	contents of the output section in terms of the contents of
	various input sections, fill constants, and, eventually, other
	types of information.

INODE
Relocating the section contents, Writing the symbol table, Information provided by the linker, Performing the Final Link
SUBSUBSECTION
	Relocating the section contents

	The <<_bfd_final_link>> function should look through the
	<<link_order>> structures attached to each section of the
	output file.  Each <<link_order>> structure should either be
	handled specially, or it should be passed to the function
	<<_bfd_default_link_order>> which will do the right thing
	(<<_bfd_default_link_order>> is defined in <<linker.c>>).

	For efficiency, a <<link_order>> of type
	<<bfd_indirect_link_order>> whose associated section belongs
	to a BFD of the same format as the output BFD must be handled
	specially.  This type of <<link_order>> describes part of an
	output section in terms of a section belonging to one of the
	input files.  The <<_bfd_final_link>> function should read the
	contents of the section and any associated relocs, apply the
	relocs to the section contents, and write out the modified
	section contents.  If performing a relocateable link, the
	relocs themselves must also be modified and written out.

@findex _bfd_relocate_contents
@findex _bfd_final_link_relocate
	The functions <<_bfd_relocate_contents>> and
	<<_bfd_final_link_relocate>> provide some general support for
	performing the actual relocations, notably overflow checking.
	Their arguments include information about the symbol the
	relocation is against and a <<reloc_howto_type>> argument
	which describes the relocation to perform.  These functions
	are defined in <<reloc.c>>.

	The a.out function which handles reading, relocating, and
	writing section contents is <<aout_link_input_section>>.  The
	actual relocation is done in <<aout_link_input_section_std>>
	and <<aout_link_input_section_ext>>.

INODE
Writing the symbol table, , Relocating the section contents, Performing the Final Link
SUBSUBSECTION
	Writing the symbol table

	The <<_bfd_final_link>> function must gather all the symbols
	in the input files and write them out.  It must also write out
	all the symbols in the global hash table.  This must be
	controlled by the <<strip>> and <<discard>> fields of the
	<<bfd_link_info>> structure.

	The local symbols of the input files will not have been
	entered into the linker hash table.  The <<_bfd_final_link>>
	routine must consider each input file and include the symbols
	in the output file.  It may be convenient to do this when
	looking through the <<link_order>> structures, or it may be
	done by stepping through the <<input_bfds>> list.

	The <<_bfd_final_link>> routine must also traverse the global
	hash table to gather all the externally visible symbols.  It
	is possible that most of the externally visible symbols may be
	written out when considering the symbols of each input file,
	but it is still necessary to traverse the hash table since the
	linker script may have defined some symbols that are not in
	any of the input files.  The <<written>> field in the
	<<bfd_link_hash_entry>> structure may be used to determine
	which entries in the hash table have not already been written
	out.

	The <<strip>> field of the <<bfd_link_info>> structure
	controls which symbols are written out.  The possible values
	are listed in <<bfdlink.h>>.  If the value is <<strip_some>>,
	then the <<keep_hash>> field of the <<bfd_link_info>>
	structure is a hash table of symbols to keep; each symbol
	should be looked up in this hash table, and only symbols which
	are present should be included in the output file.

	If the <<strip>> field of the <<bfd_link_info>> structure
	permits local symbols to be written out, the <<discard>> field
	is used to further controls which local symbols are included
	in the output file.  If the value is <<discard_l>>, then all
	local symbols which begin with a certain prefix are discarded;
	this prefix is described by the <<lprefix>> and
	<<lprefix_len>> fields of the <<bfd_link_info>> structure.

	The a.out backend handles symbols by calling
	<<aout_link_write_symbols>> on each input BFD and then
	traversing the global hash table with the function
	<<aout_link_write_other_symbol>>.  It builds a string table
	while writing out the symbols, which is written to the output
	file at the end of <<NAME(aout,final_link)>>.
*/

static struct bfd_hash_entry *generic_link_hash_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *,
	   const char *));
static boolean generic_link_add_object_symbols
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean generic_link_check_archive_element
  PARAMS ((bfd *, struct bfd_link_info *, boolean *pneeded));
static boolean generic_link_add_symbol_list
  PARAMS ((bfd *, struct bfd_link_info *, bfd_size_type count, asymbol **));
static boolean generic_add_output_symbol
  PARAMS ((bfd *, size_t *psymalloc, asymbol *));
static boolean default_fill_link_order
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   struct bfd_link_order *));
static boolean default_indirect_link_order
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   struct bfd_link_order *));

/* The link hash table structure is defined in bfdlink.h.  It provides
   a base hash table which the backend specific hash tables are built
   upon.  */

/* Routine to create an entry in the link hash table.  */

struct bfd_hash_entry *
_bfd_link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct bfd_link_hash_entry *ret = (struct bfd_link_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct bfd_link_hash_entry *) NULL)
    ret = ((struct bfd_link_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct bfd_link_hash_entry)));

  /* Call the allocation method of the superclass.  */
  ret = ((struct bfd_link_hash_entry *)
	 bfd_hash_newfunc ((struct bfd_hash_entry *) ret, table, string));

  /* Initialize the local fields.  */
  ret->type = bfd_link_hash_new;
  ret->written = false;
  ret->next = NULL;

  return (struct bfd_hash_entry *) ret;
}

/* Initialize a link hash table.  The BFD argument is the one
   responsible for creating this table.  */

boolean
_bfd_link_hash_table_init (table, abfd, newfunc)
     struct bfd_link_hash_table *table;
     bfd *abfd;
     struct bfd_hash_entry *(*newfunc) PARAMS ((struct bfd_hash_entry *,
						struct bfd_hash_table *,
						const char *));
{
  table->creator = abfd->xvec;
  table->undefs = NULL;
  table->undefs_tail = NULL;
  return bfd_hash_table_init (&table->table, newfunc);
}

/* Look up a symbol in a link hash table.  If follow is true, we
   follow bfd_link_hash_indirect and bfd_link_hash_warning links to
   the real symbol.  */

struct bfd_link_hash_entry *
bfd_link_hash_lookup (table, string, create, copy, follow)
     struct bfd_link_hash_table *table;
     const char *string;
     boolean create;
     boolean copy;
     boolean follow;
{
  struct bfd_link_hash_entry *ret;

  ret = ((struct bfd_link_hash_entry *)
	 bfd_hash_lookup (&table->table, string, create, copy));

  if (follow && ret != (struct bfd_link_hash_entry *) NULL)
    {
      while (ret->type == bfd_link_hash_indirect
	     || ret->type == bfd_link_hash_warning)
	ret = ret->u.i.link;
    }

  return ret;
}

/* Traverse a generic link hash table.  The only reason this is not a
   macro is to do better type checking.  This code presumes that an
   argument passed as a struct bfd_hash_entry * may be caught as a
   struct bfd_link_hash_entry * with no explicit cast required on the
   call.  */

void 
bfd_link_hash_traverse (table, func, info)
     struct bfd_link_hash_table *table;
     boolean (*func) PARAMS ((struct bfd_link_hash_entry *, PTR));
     PTR info;
{
  bfd_hash_traverse (&table->table,
		     ((boolean (*) PARAMS ((struct bfd_hash_entry *, PTR)))
		      func),
		     info);
}

/* Add a symbol to the linker hash table undefs list.  */

INLINE void
bfd_link_add_undef (table, h)
     struct bfd_link_hash_table *table;
     struct bfd_link_hash_entry *h;
{
  BFD_ASSERT (h->next == NULL);
  if (table->undefs_tail != (struct bfd_link_hash_entry *) NULL)
    table->undefs_tail->next = h;
  if (table->undefs == (struct bfd_link_hash_entry *) NULL)
    table->undefs = h;
  table->undefs_tail = h;
}

/* Routine to create an entry in an generic link hash table.  */

static struct bfd_hash_entry *
generic_link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct generic_link_hash_entry *ret =
    (struct generic_link_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct generic_link_hash_entry *) NULL)
    ret = ((struct generic_link_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct generic_link_hash_entry)));

  /* Call the allocation method of the superclass.  */
  ret = ((struct generic_link_hash_entry *)
	 _bfd_link_hash_newfunc ((struct bfd_hash_entry *) ret,
				 table, string));

  /* Set local fields.  */
  ret->sym = NULL;

  return (struct bfd_hash_entry *) ret;
}

/* Create an generic link hash table.  */

struct bfd_link_hash_table *
_bfd_generic_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct generic_link_hash_table *ret;

  ret = ((struct generic_link_hash_table *)
	 bfd_xmalloc (sizeof (struct generic_link_hash_table)));
  if (! _bfd_link_hash_table_init (&ret->root, abfd,
				   generic_link_hash_newfunc))
    {
      free (ret);
      return (struct bfd_link_hash_table *) NULL;
    }
  return &ret->root;
}

/* Generic function to add symbols from an object file to the global
   hash table.  */

boolean
_bfd_generic_link_add_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  boolean ret;

  switch (bfd_get_format (abfd))
    {
    case bfd_object:
      ret = generic_link_add_object_symbols (abfd, info);
      break;
    case bfd_archive:
      ret = _bfd_generic_link_add_archive_symbols
	(abfd, info, generic_link_check_archive_element);
      break;
    default:
      bfd_error = wrong_format;
      ret = false;
    }

  /* If we might be using the C based alloca function, make sure we
     have dumped the symbol tables we just allocated.  */
#ifndef __GNUC__
#ifndef alloca
  alloca (0);
#endif
#endif

  return ret;
}

/* Add symbols from an object file to the global hash table.  */

static boolean
generic_link_add_object_symbols (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  size_t symsize;
  asymbol **symbols;
  bfd_size_type symbol_count;

  symsize = get_symtab_upper_bound (abfd);
  symbols = (asymbol **) alloca (symsize);
  symbol_count = bfd_canonicalize_symtab (abfd, symbols);

  return generic_link_add_symbol_list (abfd, info, symbol_count, symbols);
}

/* We build a hash table of all symbols defined in an archive.  */

/* An archive symbol may be defined by multiple archive elements.
   This linked list is used to hold the elements.  */

struct archive_list
{
  struct archive_list *next;
  int indx;
};

/* An entry in an archive hash table.  */

struct archive_hash_entry
{
  struct bfd_hash_entry root;
  /* Where the symbol is defined.  */
  struct archive_list *defs;
};

/* An archive hash table itself.  */

struct archive_hash_table
{
  struct bfd_hash_table table;
};

static struct bfd_hash_entry *archive_hash_newfunc
  PARAMS ((struct bfd_hash_entry *, struct bfd_hash_table *, const char *));
static boolean archive_hash_table_init
  PARAMS ((struct archive_hash_table *,
	   struct bfd_hash_entry *(*) (struct bfd_hash_entry *,
				       struct bfd_hash_table *,
				       const char *)));

/* Create a new entry for an archive hash table.  */

static struct bfd_hash_entry *
archive_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct archive_hash_entry *ret = (struct archive_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct archive_hash_entry *) NULL)
    ret = ((struct archive_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct archive_hash_entry)));

  /* Call the allocation method of the superclass.  */
  ret = ((struct archive_hash_entry *)
	 bfd_hash_newfunc ((struct bfd_hash_entry *) ret, table, string));

  /* Initialize the local fields.  */
  ret->defs = (struct archive_list *) NULL;

  return (struct bfd_hash_entry *) ret;
}

/* Initialize an archive hash table.  */

static boolean
archive_hash_table_init (table, newfunc)
     struct archive_hash_table *table;
     struct bfd_hash_entry *(*newfunc) PARAMS ((struct bfd_hash_entry *,
						struct bfd_hash_table *,
						const char *));
{
  return bfd_hash_table_init (&table->table, newfunc);
}

/* Look up an entry in an archive hash table.  */

#define archive_hash_lookup(t, string, create, copy) \
  ((struct archive_hash_entry *) \
   bfd_hash_lookup (&(t)->table, (string), (create), (copy)))

/* Free an archive hash table.  */

#define archive_hash_table_free(t) bfd_hash_table_free (&(t)->table)

/* Generic function to add symbols from an archive file to the global
   hash file.  This function presumes that the archive symbol table
   has already been read in (this is normally done by the
   bfd_check_format entry point).  It looks through the undefined and
   common symbols and searches the archive symbol table for them.  If
   it finds an entry, it includes the associated object file in the
   link.

   The old linker looked through the archive symbol table for
   undefined symbols.  We do it the other way around, looking through
   undefined symbols for symbols defined in the archive.  The
   advantage of the newer scheme is that we only have to look through
   the list of undefined symbols once, whereas the old method had to
   re-search the symbol table each time a new object file was added.

   The CHECKFN argument is used to see if an object file should be
   included.  CHECKFN should set *PNEEDED to true if the object file
   should be included, and must also call the bfd_link_info
   add_archive_element callback function and handle adding the symbols
   to the global hash table.  CHECKFN should only return false if some
   sort of error occurs.

   For some formats, such as a.out, it is possible to look through an
   object file but not actually include it in the link.  The
   archive_pass field in a BFD is used to avoid checking the symbols
   of an object files too many times.  When an object is included in
   the link, archive_pass is set to -1.  If an object is scanned but
   not included, archive_pass is set to the pass number.  The pass
   number is incremented each time a new object file is included.  The
   pass number is used because when a new object file is included it
   may create new undefined symbols which cause a previously examined
   object file to be included.  */

boolean
_bfd_generic_link_add_archive_symbols (abfd, info, checkfn)
     bfd *abfd;
     struct bfd_link_info *info;
     boolean (*checkfn) PARAMS ((bfd *, struct bfd_link_info *,
				 boolean *pneeded));
{
  carsym *arsyms;
  carsym *arsym_end;
  register carsym *arsym;
  int pass;
  struct archive_hash_table arsym_hash;
  int indx;
  struct bfd_link_hash_entry **pundef;

  if (! bfd_has_map (abfd))
    {
      bfd_error = no_symbols;
      return false;
    }

  arsyms = bfd_ardata (abfd)->symdefs;
  arsym_end = arsyms + bfd_ardata (abfd)->symdef_count;

  /* In order to quickly determine whether an symbol is defined in
     this archive, we build a hash table of the symbols.  */
  if (! archive_hash_table_init (&arsym_hash, archive_hash_newfunc))
    return false;
  for (arsym = arsyms, indx = 0; arsym < arsym_end; arsym++, indx++)
    {
      struct archive_hash_entry *arh;
      struct archive_list *l;

      arh = archive_hash_lookup (&arsym_hash, arsym->name, true, false);
      if (arh == (struct archive_hash_entry *) NULL)
	return false;
      l = (struct archive_list *) alloca (sizeof (struct archive_list));
      l->next = arh->defs;
      arh->defs = l;
      l->indx = indx;
    }

  pass = 1;

  /* New undefined symbols are added to the end of the list, so we
     only need to look through it once.  */
  pundef = &info->hash->undefs;
  while (*pundef != (struct bfd_link_hash_entry *) NULL)
    {
      struct bfd_link_hash_entry *h;
      struct archive_hash_entry *arh;
      struct archive_list *l;

      h = *pundef;

      /* When a symbol is defined, it is not necessarily removed from
	 the list.  */
      if (h->type != bfd_link_hash_undefined
	  && h->type != bfd_link_hash_common)
	{
	  /* Remove this entry from the list, for general cleanliness
	     and because we are going to look through the list again
	     if we search any more libraries.  We can't remove the
	     entry if it is the tail, because that would lose any
	     entries we add to the list later on.  */
	  if (*pundef != info->hash->undefs_tail)
	    *pundef = (*pundef)->next;
	  else
	    pundef = &(*pundef)->next;
	  continue;
	}

      /* Look for this symbol in the archive symbol map.  */
      arh = archive_hash_lookup (&arsym_hash, h->root.string, false, false);
      if (arh == (struct archive_hash_entry *) NULL)
	{
	  pundef = &(*pundef)->next;
	  continue;
	}

      /* Look at all the objects which define this symbol.  */
      for (l = arh->defs; l != (struct archive_list *) NULL; l = l->next)
	{
	  bfd *element;
	  boolean needed;

	  /* If the symbol has gotten defined along the way, quit.  */
	  if (h->type != bfd_link_hash_undefined
	      && h->type != bfd_link_hash_common)
	    break;

	  element = bfd_get_elt_at_index (abfd, l->indx);
	  if (element == (bfd *) NULL)
	    return false;

	  /* If we've already included this element, or if we've
	     already checked it on this pass, continue.  */
	  if (element->archive_pass == -1
	      || element->archive_pass == pass)
	    continue;

	  /* If we can't figure this element out, just ignore it.  */
	  if (! bfd_check_format (element, bfd_object))
	    {
	      element->archive_pass = -1;
	      continue;
	    }

	  /* CHECKFN will see if this element should be included, and
	     go ahead and include it if appropriate.  */
	  if (! (*checkfn) (element, info, &needed))
	    return false;

	  if (! needed)
	    element->archive_pass = pass;
	  else
	    {
	      element->archive_pass = -1;

	      /* Increment the pass count to show that we may need to
		 recheck object files which were already checked.  */
	      ++pass;
	    }
	}

      pundef = &(*pundef)->next;
    }

  archive_hash_table_free (&arsym_hash);

  return true;
}

/* See if we should include an archive element.  */

static boolean
generic_link_check_archive_element (abfd, info, pneeded)
     bfd *abfd;
     struct bfd_link_info *info;
     boolean *pneeded;
{
  size_t symsize;
  asymbol **symbols;
  bfd_size_type symbol_count;
  asymbol **pp, **ppend;

  *pneeded = false;

  symsize = get_symtab_upper_bound (abfd);
  symbols = (asymbol **) alloca (symsize);
  symbol_count = bfd_canonicalize_symtab (abfd, symbols);

  pp = symbols;
  ppend = symbols + symbol_count;
  for (; pp < ppend; pp++)
    {
      asymbol *p;
      struct bfd_link_hash_entry *h;

      p = *pp;

      /* We are only interested in globally visible symbols.  */
      if (! bfd_is_com_section (p->section)
	  && (p->flags & (BSF_GLOBAL | BSF_INDIRECT | BSF_WEAK)) == 0)
	continue;

      /* We are only interested if we know something about this
	 symbol, and it is undefined or common.  An undefined weak
	 symbol (type bfd_link_hash_weak) is not considered to be a
	 reference when pulling files out of an archive.  See the SVR4
	 ABI, p. 4-27.  */
      h = bfd_link_hash_lookup (info->hash, bfd_asymbol_name (p), false,
				false, true);
      if (h == (struct bfd_link_hash_entry *) NULL
	  || (h->type != bfd_link_hash_undefined
	      && h->type != bfd_link_hash_common))
	continue;

      /* P is a symbol we are looking for.  */

      if (! bfd_is_com_section (p->section))
	{
	  /* This object file defines this symbol, so pull it in.  */
	  if (! (*info->callbacks->add_archive_element) (info, abfd,
							 bfd_asymbol_name (p)))
	    return false;
	  if (! generic_link_add_symbol_list (abfd, info, symbol_count,
					      symbols))
	    return false;
	  *pneeded = true;
	  return true;
	}

      /* P is a common symbol.  */

      if (h->type == bfd_link_hash_undefined)
	{
	  bfd *symbfd;

	  symbfd = h->u.undef.abfd;
	  if (symbfd == (bfd *) NULL)
	    {
	      /* This symbol was created as undefined from outside
		 BFD.  We assume that we should link in the object
		 file.  This is for the -u option in the linker.  */
	      if (! (*info->callbacks->add_archive_element)
		  (info, abfd, bfd_asymbol_name (p)))
		return false;
	      *pneeded = true;
	      return true;
	    }

	  /* Turn the symbol into a common symbol but do not link in
	     the object file.  This is how a.out works.  Object
	     formats that require different semantics must implement
	     this function differently.  This symbol is already on the
	     undefs list.  We add the section to a common section
	     attached to symbfd to ensure that it is in a BFD which
	     will be linked in.  */
	  h->type = bfd_link_hash_common;
	  h->u.c.size = bfd_asymbol_value (p);
	  if (p->section == &bfd_com_section)
	    h->u.c.section = bfd_make_section_old_way (symbfd, "COMMON");
	  else
	    h->u.c.section = bfd_make_section_old_way (symbfd,
						       p->section->name);
	  h->u.c.section->flags = SEC_ALLOC;
	}
      else
	{
	  /* Adjust the size of the common symbol if necessary.  This
	     is how a.out works.  Object formats that require
	     different semantics must implement this function
	     differently.  */
	  if (bfd_asymbol_value (p) > h->u.c.size)
	    h->u.c.size = bfd_asymbol_value (p);
	}
    }

  /* This archive element is not needed.  */
  return true;
}

/* Add the symbol from an object file to the global hash table.  */

static boolean
generic_link_add_symbol_list (abfd, info, symbol_count, symbols)
     bfd *abfd;
     struct bfd_link_info *info;
     bfd_size_type symbol_count;
     asymbol **symbols;
{
  asymbol **pp, **ppend;

  pp = symbols;
  ppend = symbols + symbol_count;
  for (; pp < ppend; pp++)
    {
      asymbol *p;

      p = *pp;

      if ((p->flags & (BSF_INDIRECT
		       | BSF_WARNING
		       | BSF_GLOBAL
		       | BSF_CONSTRUCTOR
		       | BSF_WEAK)) != 0
	  || bfd_get_section (p) == &bfd_und_section
	  || bfd_is_com_section (bfd_get_section (p))
	  || bfd_get_section (p) == &bfd_ind_section)
	{
	  const char *name;
	  const char *string;
	  struct generic_link_hash_entry *h;

	  name = bfd_asymbol_name (p);
	  if ((p->flags & BSF_INDIRECT) != 0
	      || p->section == &bfd_ind_section)
	    string = bfd_asymbol_name ((asymbol *) p->value);
	  else if ((p->flags & BSF_WARNING) != 0)
	    {
	      /* The name of P is actually the warning string, and the
		 value is actually a pointer to the symbol to warn
		 about.  */
	      string = name;
	      name = bfd_asymbol_name ((asymbol *) p->value);
	    }
	  else
	    string = NULL;

	  /* We pass the constructor argument as false, for
	     compatibility.  As backends are converted they can
	     arrange to pass the right value (the right value is the
	     size of a function pointer if gcc uses collect2 for the
	     object file format, zero if it does not).
	     FIXME: We pass the bitsize as 32, which is just plain
	     wrong, but actually doesn't matter very much.  */
	  if (! (_bfd_generic_link_add_one_symbol
		 (info, abfd, name, p->flags, bfd_get_section (p),
		  p->value, string, false, 0, 32,
		  (struct bfd_link_hash_entry **) &h)))
	    return false;

	  /* Save the BFD symbol so that we don't lose any backend
	     specific information that may be attached to it.  We only
	     want this one if it gives more information than the
	     existing one; we don't want to replace a defined symbol
	     with an undefined one.  This routine may be called with a
	     hash table other than the generic hash table, so we only
	     do this if we are certain that the hash table is a
	     generic one.  */
	  if (info->hash->creator == abfd->xvec)
	    {
	      if (h->sym == (asymbol *) NULL
		  || (bfd_get_section (p) != &bfd_und_section
		      && (! bfd_is_com_section (bfd_get_section (p))
			  || (bfd_get_section (h->sym) == &bfd_und_section))))
		{
		  h->sym = p;
		  /* BSF_OLD_COMMON is a hack to support COFF reloc
		     reading, and it should go away when the COFF
		     linker is switched to the new version.  */
		  if (bfd_is_com_section (bfd_get_section (p)))
		    p->flags |= BSF_OLD_COMMON;
		}
	    }
	}
    }

  return true;
}

/* We use a state table to deal with adding symbols from an object
   file.  The first index into the state table describes the symbol
   from the object file.  The second index into the state table is the
   type of the symbol in the hash table.  */

/* The symbol from the object file is turned into one of these row
   values.  */

enum link_row
{
  UNDEF_ROW,		/* Undefined.  */
  UNDEFW_ROW,		/* Weak undefined.  */
  DEF_ROW,		/* Defined.  */
  DEFW_ROW,		/* Weak defined.  */
  COMMON_ROW,		/* Common.  */
  INDR_ROW,		/* Indirect.  */
  WARN_ROW,		/* Warning.  */
  SET_ROW		/* Member of set.  */
};

/* The actions to take in the state table.  */

enum link_action
{
  FAIL,		/* Abort. */
  UND,		/* Mark symbol undefined.  */
  WEAK,		/* Mark symbol weak undefined.  */
  DEF,		/* Mark symbol defined.  */
  COM,		/* Mark symbol common.  */
  CREF,		/* Possibly warn about common reference to defined symbol.  */
  CDEF,		/* Define existing common symbol.  */
  NOACT,	/* No action.  */
  BIG,		/* Mark symbol common using largest size.  */
  MDEF,		/* Multiple definition error.  */
  IND,		/* Make indirect symbol.  */
  SET,		/* Add value to set.  */
  MWARN,	/* Make warning symbol.  */
  WARN,		/* Issue warning.  */
  CYCLE,	/* Repeat with symbol pointed to.  */
  WARNC		/* Issue warning and then CYCLE.  */
};

/* The state table itself.  The first index is a link_row and the
   second index is a bfd_link_hash_type.  */

static const enum link_action link_action[8][7] =
{
  /* current\prev    new    undef  weak   def    com    indr   warn  */
  /* UNDEF_ROW 	*/  {UND,   NOACT, NOACT, NOACT, NOACT, CYCLE, WARNC },
  /* UNDEFW_ROW	*/  {WEAK,  WEAK,  NOACT, NOACT, NOACT, CYCLE, WARNC },
  /* DEF_ROW 	*/  {DEF,   DEF,   DEF,   MDEF,  CDEF,  CYCLE, CYCLE },
  /* DEFW_ROW 	*/  {DEF,   DEF,   DEF,   NOACT, NOACT, CYCLE, CYCLE },
  /* COMMON_ROW	*/  {COM,   COM,   COM,   CREF,  BIG,   CYCLE, WARNC },
  /* INDR_ROW	*/  {IND,   IND,   IND,   MDEF,  MDEF,  MDEF,  WARNC },
  /* WARN_ROW   */  {MWARN, WARN,  WARN,  MWARN, MWARN, MWARN, NOACT },
  /* SET_ROW	*/  {SET,   SET,   SET,   SET,   SET,   CYCLE, WARNC }
};

/* Add a symbol to the global hash table.
   ABFD is the BFD the symbol comes from.
   NAME is the name of the symbol.
   FLAGS is the BSF_* bits associated with the symbol.
   SECTION is the section in which the symbol is defined; this may be
     bfd_und_section or bfd_com_section.
   VALUE is the value of the symbol, relative to the section.
   STRING is used for either an indirect symbol, in which case it is
     the name of the symbol to indirect to, or a warning symbol, in
     which case it is the warning string.
   COPY is true if NAME or STRING must be copied into locally
     allocated memory if they need to be saved.
   CONSTRUCTOR is true if we should automatically collect gcc
     constructor or destructor names.
   BITSIZE is the number of bits in constructor or set entries.
   HASHP, if not NULL, is a place to store the created hash table
     entry.  */

boolean
_bfd_generic_link_add_one_symbol (info, abfd, name, flags, section, value,
				  string, copy, constructor, bitsize, hashp)
     struct bfd_link_info *info;
     bfd *abfd;
     const char *name;
     flagword flags;
     asection *section;
     bfd_vma value;
     const char *string;
     boolean copy;
     boolean constructor;
     unsigned int bitsize;
     struct bfd_link_hash_entry **hashp;
{
  enum link_row row;
  struct bfd_link_hash_entry *h;
  boolean cycle;

  if (section == &bfd_ind_section
      || (flags & BSF_INDIRECT) != 0)
    row = INDR_ROW;
  else if ((flags & BSF_WARNING) != 0)
    row = WARN_ROW;
  else if ((flags & BSF_CONSTRUCTOR) != 0)
    row = SET_ROW;
  else if (section == &bfd_und_section)
    {
      if ((flags & BSF_WEAK) != 0)
	row = UNDEFW_ROW;
      else
	row = UNDEF_ROW;
    }
  else if ((flags & BSF_WEAK) != 0)
    row = DEFW_ROW;
  else if (bfd_is_com_section (section))
    row = COMMON_ROW;
  else
    row = DEF_ROW;

  h = bfd_link_hash_lookup (info->hash, name, true, copy, false);
  if (h == (struct bfd_link_hash_entry *) NULL)
    {
      if (hashp != (struct bfd_link_hash_entry **) NULL)
	*hashp = NULL;
      return false;
    }

  if (info->notice_hash != (struct bfd_hash_table *) NULL
      && (bfd_hash_lookup (info->notice_hash, name, false, false)
	  != (struct bfd_hash_entry *) NULL))
    {
      if (! (*info->callbacks->notice) (info, name, abfd, section, value))
	return false;
    }

  if (hashp != (struct bfd_link_hash_entry **) NULL)
    *hashp = h;

  do
    {
      enum link_action action;

      cycle = false;
      action = link_action[(int) row][(int) h->type];
      switch (action)
	{
	case FAIL:
	  abort ();
	case UND:
	  h->type = bfd_link_hash_undefined;
	  h->u.undef.abfd = abfd;
	  bfd_link_add_undef (info->hash, h);
	  break;
	case WEAK:
	  h->type = bfd_link_hash_weak;
	  h->u.undef.abfd = abfd;
	  break;
	case CDEF:
	  BFD_ASSERT (h->type == bfd_link_hash_common);
	  if (! ((*info->callbacks->multiple_common)
		 (info, name,
		  h->u.c.section->owner, bfd_link_hash_common, h->u.c.size,
		  abfd, bfd_link_hash_defined, (bfd_vma) 0)))
	    return false;
	  /* Fall through.  */
	case DEF:
	  h->type = bfd_link_hash_defined;
	  h->u.def.section = section;
	  h->u.def.value = value;

	  /* If we have been asked to, we act like collect2 and
	     identify all functions that might be global constructors
	     and destructors and pass them up in a callback.  We only
	     do this for certain object file types, since many object
	     file types can handle this automatically.  */
	  if (constructor && name[0] == '_')
	    {
	      const char *s;

	      /* A constructor or destructor name starts like this:
		   _+GLOBAL_[_.$][ID][_.$]
		 where the first [_.$] and the second are the same
		 character (we accept any character there, in case a
		 new object file format comes along with even worse
		 naming restrictions).  */

#define CONS_PREFIX "GLOBAL_"
#define CONS_PREFIX_LEN (sizeof CONS_PREFIX - 1)

	      s = name + 1;
	      while (*s == '_')
		++s;
	      if (s[0] == 'G'
		  && strncmp (s, CONS_PREFIX, CONS_PREFIX_LEN - 1) == 0)
		{
		  char c;

		  c = s[CONS_PREFIX_LEN + 1];
		  if ((c == 'I' || c == 'D')
		      && s[CONS_PREFIX_LEN] == s[CONS_PREFIX_LEN + 2])
		    {
		      if (! ((*info->callbacks->constructor)
			     (info,
			      c == 'I' ? true : false, bitsize,
			      name, abfd, section, value)))
			return false;
		    }
		}
	    }

	  break;
	case COM:
	  if (h->type == bfd_link_hash_new)
	    bfd_link_add_undef (info->hash, h);
	  h->type = bfd_link_hash_common;
	  h->u.c.size = value;
	  if (section == &bfd_com_section)
	    {
	      h->u.c.section = bfd_make_section_old_way (abfd, "COMMON");
	      h->u.c.section->flags = SEC_ALLOC;
	    }
	  else if (section->owner != abfd)
	    {
	      h->u.c.section = bfd_make_section_old_way (abfd, section->name);
	      h->u.c.section->flags = SEC_ALLOC;
	    }
	  else
	    h->u.c.section = section;
	  break;
	case NOACT:
	  break;
	case BIG:
	  BFD_ASSERT (h->type == bfd_link_hash_common);
	  if (! ((*info->callbacks->multiple_common)
		 (info, name,
		  h->u.c.section->owner, bfd_link_hash_common, h->u.c.size,
		  abfd, bfd_link_hash_common, value)))
	    return false;
	  if (value > h->u.c.size)
	    h->u.c.size = value;
	  break;
	case CREF:
	  BFD_ASSERT (h->type == bfd_link_hash_defined);
	  if (! ((*info->callbacks->multiple_common)
		 (info, name,
		  h->u.def.section->owner, bfd_link_hash_defined, (bfd_vma) 0,
		  abfd, bfd_link_hash_common, value)))
	    return false;
	  break;
	case MDEF:
	  {
	    asection *msec;
	    bfd_vma mval;

	    switch (h->type)
	      {
	      case bfd_link_hash_defined:
		msec = h->u.def.section;
		mval = h->u.def.value;
		break;
	      case bfd_link_hash_common:
		msec = &bfd_com_section;
		mval = h->u.c.size;
		break;
	      case bfd_link_hash_indirect:
		msec = &bfd_ind_section;
		mval = 0;
		break;
	      default:
		abort ();
	      }
	      
	    if (! ((*info->callbacks->multiple_definition)
		   (info, name, msec->owner, msec, mval, abfd, section,
		    value)))
	      return false;
	  }
	  break;
	case IND:
	  {
	    struct bfd_link_hash_entry *inh;

	    /* STRING is the name of the symbol we want to indirect
	       to.  */
	    inh = bfd_link_hash_lookup (info->hash, string, true, copy,
					false);
	    if (inh == (struct bfd_link_hash_entry *) NULL)
	      return false;
	    if (inh->type == bfd_link_hash_new)
	      {
		inh->type = bfd_link_hash_undefined;
		inh->u.undef.abfd = abfd;
		bfd_link_add_undef (info->hash, inh);
	      }
	    h->type = bfd_link_hash_indirect;
	    h->u.i.link = inh;
	  }
	  break;
	case SET:
	  if (! (*info->callbacks->add_to_set) (info, h, bitsize, abfd,
						section, value))
	    return false;
	  break;
	case WARN:
	case WARNC:
	  if (h->u.i.warning != NULL)
	    {
	      if (! (*info->callbacks->warning) (info, h->u.i.warning))
		return false;
	      /* Only issue a warning once.  */
	      h->u.i.warning = NULL;
	    }
	  if (action == WARN)
	    break;
	  /* Fall through.  */
	case CYCLE:
	  h = h->u.i.link;
	  cycle = true;
	  break;
	case MWARN:
	  {
	    struct bfd_link_hash_entry *sub;

	    /* STRING is the warning to give.  */
	    sub = ((struct bfd_link_hash_entry *)
		   bfd_hash_allocate (&info->hash->table,
				      sizeof (struct bfd_link_hash_entry)));
	    *sub = *h;
	    h->type = bfd_link_hash_warning;
	    h->u.i.link = sub;
	    if (! copy)
	      h->u.i.warning = string;
	    else
	      {
		char *w;

		w = bfd_hash_allocate (&info->hash->table,
				       strlen (string) + 1);
		strcpy (w, string);
		h->u.i.warning = w;
	      }
	  }
	  break;
	}
    }
  while (cycle);

  return true;
}

/* Generic final link routine.  */

boolean
_bfd_generic_final_link (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  bfd *sub;
  asection *o;
  struct bfd_link_order *p;
  size_t outsymalloc;
  struct generic_write_global_symbol_info wginfo;

  abfd->outsymbols = (asymbol **) NULL;
  abfd->symcount = 0;
  outsymalloc = 0;

  /* Build the output symbol table.  This also reads in the symbols
     for all the input BFDs, keeping them in the outsymbols field.  */
  for (sub = info->input_bfds; sub != (bfd *) NULL; sub = sub->link_next)
    if (! _bfd_generic_link_output_symbols (abfd, sub, info, &outsymalloc))
      return false;

  /* Accumulate the global symbols.  */
  wginfo.output_bfd = abfd;
  wginfo.psymalloc = &outsymalloc;
  _bfd_generic_link_hash_traverse (_bfd_generic_hash_table (info),
				   _bfd_generic_link_write_global_symbol,
				   (PTR) &wginfo);

  if (info->relocateable)
    {
      /* Allocate space for the output relocs for each section.  */
      for (o = abfd->sections;
	   o != (asection *) NULL;
	   o = o->next)
	{
	  o->reloc_count = 0;
	  for (p = o->link_order_head;
	       p != (struct bfd_link_order *) NULL;
	       p = p->next)
	    {
	      if (p->type == bfd_indirect_link_order)
		{
		  asection *input_section;
		  bfd *input_bfd;
		  bfd_size_type relsize;
		  arelent **relocs;
		  bfd_size_type reloc_count;

		  input_section = p->u.indirect.section;
		  input_bfd = input_section->owner;
		  relsize = bfd_get_reloc_upper_bound (input_bfd,
						       input_section);
		  relocs = (arelent **) bfd_xmalloc (relsize);
		  reloc_count =
		    bfd_canonicalize_reloc (input_bfd, input_section,
					    relocs,
					    bfd_get_outsymbols (input_bfd));
		  BFD_ASSERT (reloc_count == input_section->reloc_count);
		  o->reloc_count += reloc_count;
		  free (relocs);
		}
	    }
	  if (o->reloc_count > 0)
	    {
	      o->orelocation = ((arelent **)
				bfd_alloc (abfd,
					   (o->reloc_count
					    * sizeof (arelent *))));
	      /* Reset the count so that it can be used as an index
		 when putting in the output relocs.  */
	      o->reloc_count = 0;
	    }
	}
    }

  /* Handle all the link order information for the sections.  */
  for (o = abfd->sections;
       o != (asection *) NULL;
       o = o->next)
    {
      for (p = o->link_order_head;
	   p != (struct bfd_link_order *) NULL;
	   p = p->next)
	{
	  if (! _bfd_default_link_order (abfd, info, o, p))
	    return false;
	}
    }

  return true;
}

/* Add an output symbol to the output BFD.  */

static boolean
generic_add_output_symbol (output_bfd, psymalloc, sym)
     bfd *output_bfd;
     size_t *psymalloc;
     asymbol *sym;
{
  if (output_bfd->symcount >= *psymalloc)
    {
      asymbol **newsyms;

      if (*psymalloc == 0)
	*psymalloc = 124;
      else
	*psymalloc *= 2;
      if (output_bfd->outsymbols == (asymbol **) NULL)
	newsyms = (asymbol **) malloc (*psymalloc * sizeof (asymbol *));
      else
	newsyms = (asymbol **) realloc (output_bfd->outsymbols,
					*psymalloc * sizeof (asymbol *));
      if (newsyms == (asymbol **) NULL)
	{
	  bfd_error = no_memory;
	  return false;
	}
      output_bfd->outsymbols = newsyms;
    }

  output_bfd->outsymbols[output_bfd->symcount] = sym;
  ++output_bfd->symcount;

  return true;
}

/* Handle the symbols for an input BFD.  */

boolean
_bfd_generic_link_output_symbols (output_bfd, input_bfd, info, psymalloc)
     bfd *output_bfd;
     bfd *input_bfd;
     struct bfd_link_info *info;
     size_t *psymalloc;
{
  size_t symsize;
  asymbol **sym_ptr;
  asymbol **sym_end;

  symsize = get_symtab_upper_bound (input_bfd);
  input_bfd->outsymbols = (asymbol **) bfd_alloc (input_bfd, symsize);
  input_bfd->symcount = bfd_canonicalize_symtab (input_bfd,
						 input_bfd->outsymbols);

  /* Create a filename symbol if we are supposed to.  */
  if (info->create_object_symbols_section != (asection *) NULL)
    {
      asection *sec;

      for (sec = input_bfd->sections;
	   sec != (asection *) NULL;
	   sec = sec->next)
	{
	  if (sec->output_section == info->create_object_symbols_section)
	    {
	      asymbol *newsym;

	      newsym = bfd_make_empty_symbol (input_bfd);
	      newsym->name = input_bfd->filename;
	      newsym->value = 0;
	      newsym->flags = BSF_LOCAL | BSF_FILE;
	      newsym->section = sec;

	      if (! generic_add_output_symbol (output_bfd, psymalloc,
					       newsym))
		return false;

	      break;
	    }
	}
    }

  /* Adjust the values of the globally visible symbols, and write out
     local symbols.  */
  sym_ptr = bfd_get_outsymbols (input_bfd);
  sym_end = sym_ptr + bfd_get_symcount (input_bfd);
  for (; sym_ptr < sym_end; sym_ptr++)
    {
      asymbol *sym;
      struct generic_link_hash_entry *h;
      boolean output;

      h = (struct generic_link_hash_entry *) NULL;
      sym = *sym_ptr;
      if ((sym->flags & (BSF_INDIRECT
			 | BSF_WARNING
			 | BSF_GLOBAL
			 | BSF_CONSTRUCTOR
			 | BSF_WEAK)) != 0
	  || bfd_get_section (sym) == &bfd_und_section
	  || bfd_is_com_section (bfd_get_section (sym))
	  || bfd_get_section (sym) == &bfd_ind_section)
	{
	  h = _bfd_generic_link_hash_lookup (_bfd_generic_hash_table (info),
					     bfd_asymbol_name (sym),
					     false, false, true);
	  if (h != (struct generic_link_hash_entry *) NULL)
	    {
	      /* Force all references to this symbol to point to
		 the same area in memory.  It is possible that
		 this routine will be called with a hash table
		 other than a generic hash table, so we double
		 check that.  */
	      if (info->hash->creator == input_bfd->xvec)
		{
		  if (h->sym != (asymbol *) NULL)
		    *sym_ptr = sym = h->sym;
		}

	      switch (h->root.type)
		{
		default:
		case bfd_link_hash_new:
		  abort ();
		case bfd_link_hash_undefined:
		case bfd_link_hash_weak:
		  break;
		case bfd_link_hash_defined:
		  sym->value = h->root.u.def.value;
		  sym->section = h->root.u.def.section;
		  sym->flags |= BSF_GLOBAL;
		  break;
		case bfd_link_hash_common:
		  sym->value = h->root.u.c.size;
		  sym->flags |= BSF_GLOBAL;
		  if (! bfd_is_com_section (sym->section))
		    {
		      BFD_ASSERT (sym->section == &bfd_und_section);
		      sym->section = &bfd_com_section;
		    }
		  /* We do not set the section of the symbol to
		     h->root.u.c.section.  That value was saved so
		     that we would know where to allocate the symbol
		     if it was defined.  In this case the type is
		     still bfd_link_hash_common, so we did not define
		     it, so we do not want to use that section.  */
		  break;
		}
	    }
	}

      /* This switch is straight from the old code in
	 write_file_locals in ldsym.c.  */
      if (info->strip == strip_some
	  && (bfd_hash_lookup (info->keep_hash, bfd_asymbol_name (sym),
			       false, false)
	      == (struct bfd_hash_entry *) NULL))
	output = false;
      else if ((sym->flags & (BSF_GLOBAL | BSF_WEAK)) != 0)
	{
	  /* If this symbol is marked as occurring now, rather
	     than at the end, output it now.  This is used for
	     COFF C_EXT FCN symbols.  FIXME: There must be a
	     better way.  */
	  if (bfd_asymbol_bfd (sym) == input_bfd
	      && (sym->flags & BSF_NOT_AT_END) != 0)
	    output = true;
	  else
	    output = false;
	}
      else if (sym->section == &bfd_ind_section)
	output = false;
      else if ((sym->flags & BSF_DEBUGGING) != 0)
	{
	  if (info->strip == strip_none)
	    output = true;
	  else
	    output = false;
	}
      else if (sym->section == &bfd_und_section
	       || bfd_is_com_section (sym->section))
	output = false;
      else if ((sym->flags & BSF_LOCAL) != 0)
	{
	  if ((sym->flags & BSF_WARNING) != 0)
	    output = false;
	  else
	    {
	      switch (info->discard)
		{
		default:
		case discard_all:
		  output = false;
		  break;
		case discard_l:
		  if (bfd_asymbol_name (sym)[0] == info->lprefix[0]
		      && (info->lprefix_len == 1
			  || strncmp (bfd_asymbol_name (sym), info->lprefix,
				      info->lprefix_len) == 0))
		    output = false;
		  else
		    output = true;
		  break;
		case discard_none:
		  output = true;
		  break;
		}
	    }
	}
      else if ((sym->flags & BSF_CONSTRUCTOR))
	{
	  if (info->strip != strip_all)
	    output = true;
	  else
	    output = false;
	}
      else
	abort ();

      if (output)
	{
	  if (! generic_add_output_symbol (output_bfd, psymalloc, sym))
	    return false;
	  if (h != (struct generic_link_hash_entry *) NULL)
	    h->root.written = true;
	}
    }

  return true;
}

/* Write out a global symbol, if it hasn't already been written out.
   This is called for each symbol in the hash table.  */

boolean
_bfd_generic_link_write_global_symbol (h, data)
     struct generic_link_hash_entry *h;
     PTR data;
{
  struct generic_write_global_symbol_info *wginfo =
    (struct generic_write_global_symbol_info *) data;
  asymbol *sym;

  if (h->root.written)
    return true;

  if (h->sym != (asymbol *) NULL)
    {
      sym = h->sym;
      BFD_ASSERT (strcmp (bfd_asymbol_name (sym), h->root.root.string) == 0);
    }
  else
    {
      sym = bfd_make_empty_symbol (wginfo->output_bfd);
      sym->name = h->root.root.string;
      sym->flags = 0;
    }

  switch (h->root.type)
    {
    default:
    case bfd_link_hash_new:
      abort ();
    case bfd_link_hash_undefined:
      sym->section = &bfd_und_section;
      sym->value = 0;
      break;
    case bfd_link_hash_weak:
      sym->section = &bfd_und_section;
      sym->value = 0;
      sym->flags |= BSF_WEAK;
    case bfd_link_hash_defined:
      sym->section = h->root.u.def.section;
      sym->value = h->root.u.def.value;
      break;
    case bfd_link_hash_common:
      sym->value = h->root.u.c.size;
      if (! bfd_is_com_section (sym->section))
	{
	  BFD_ASSERT (sym->section == &bfd_und_section);
	  sym->section = &bfd_com_section;
	}
      /* Do not set the section; see _bfd_generic_link_output_symbols.  */
      break;
    case bfd_link_hash_indirect:
    case bfd_link_hash_warning:
      /* FIXME: What should we do here?  */
      break;
    }

  sym->flags |= BSF_GLOBAL;

  if (! generic_add_output_symbol (wginfo->output_bfd, wginfo->psymalloc,
				   sym))
    {
      /* FIXME: No way to return failure.  */
      abort ();
    }

  h->root.written = true;

  return true;
}

/* Allocate a new link_order for a section.  */

struct bfd_link_order *
bfd_new_link_order (abfd, section)
     bfd *abfd;
     asection *section;
{
  struct bfd_link_order *new;

  new = ((struct bfd_link_order *)
	 bfd_alloc_by_size_t (abfd, sizeof (struct bfd_link_order)));

  new->type = bfd_undefined_link_order;
  new->offset = 0;
  new->size = 0;
  new->next = (struct bfd_link_order *) NULL;

  if (section->link_order_tail != (struct bfd_link_order *) NULL)
    section->link_order_tail->next = new;
  else
    section->link_order_head = new;
  section->link_order_tail = new;

  return new;
}

/* Default link order processing routine.  */

boolean
_bfd_default_link_order (abfd, info, sec, link_order)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     struct bfd_link_order *link_order;
{
  switch (link_order->type)
    {
    case bfd_undefined_link_order:
    default:
      abort ();
    case bfd_indirect_link_order:
      return default_indirect_link_order (abfd, info, sec, link_order);
    case bfd_fill_link_order:
      return default_fill_link_order (abfd, info, sec, link_order);
    }
}

/* Default routine to handle a bfd_fill_link_order.  */

/*ARGSUSED*/
static boolean
default_fill_link_order (abfd, info, sec, link_order)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     struct bfd_link_order *link_order;
{
  size_t size;
  char *space;
  size_t i;
  int fill;

  BFD_ASSERT ((sec->flags & SEC_HAS_CONTENTS) != 0);

  size = (size_t) link_order->size;
  space = (char *) alloca (size);
  fill = link_order->u.fill.value;
  for (i = 0; i < size; i += 2)
    space[i] = fill >> 8;
  for (i = 1; i < size; i += 2)
    space[i] = fill;
  return bfd_set_section_contents (abfd, sec, space,
				   (file_ptr) link_order->offset,
				   link_order->size);
}

/* Default routine to handle a bfd_indirect_link_order.  */

static boolean
default_indirect_link_order (output_bfd, info, output_section, link_order)
     bfd *output_bfd;
     struct bfd_link_info *info;
     asection *output_section;
     struct bfd_link_order *link_order;
{
  asection *input_section;
  bfd *input_bfd;
  bfd_byte *contents;

  BFD_ASSERT ((output_section->flags & SEC_HAS_CONTENTS) != 0);

  if (link_order->size == 0)
    return true;

  input_section = link_order->u.indirect.section;
  input_bfd = input_section->owner;

  BFD_ASSERT (input_section->output_section == output_section);
  BFD_ASSERT (input_section->output_offset == link_order->offset);
  BFD_ASSERT (bfd_section_size (input_bfd, input_section) == link_order->size);

  if (info->relocateable
      && input_section->reloc_count > 0
      && output_section->orelocation == (arelent **) NULL)
    {
      /* Space has not been allocated for the output relocations.
	 This can happen when we are called by a specific backend
	 because somebody is attempting to link together different
	 types of object files.  Handling this case correctly is
	 difficult, and sometimes impossible.  */
      abort ();
    }

  /* Get the canonical symbols.  The generic linker will always have
     retrieved them by this point, but we may be being called by a
     specific linker when linking different types of object files
     together.  */
  if (bfd_get_outsymbols (input_bfd) == (asymbol **) NULL)
    {
      size_t symsize;

      symsize = get_symtab_upper_bound (input_bfd);
      input_bfd->outsymbols = (asymbol **) bfd_alloc (input_bfd, symsize);
      input_bfd->symcount = bfd_canonicalize_symtab (input_bfd,
						     input_bfd->outsymbols);
    }

  /* Get and relocate the section contents.  */
  contents = (bfd_byte *) alloca (bfd_section_size (input_bfd, input_section));
  contents = (bfd_get_relocated_section_contents
	      (output_bfd, info, link_order, contents, info->relocateable,
	       bfd_get_outsymbols (input_bfd)));

  /* Output the section contents.  */
  if (! bfd_set_section_contents (output_bfd, output_section, (PTR) contents,
				  link_order->offset, link_order->size))
    return false;

  return true;
}
