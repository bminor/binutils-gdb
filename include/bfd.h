/* A -*- C -*- header file for the bfd library
   Copyright 1990, 1991 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of BFD, the Binary File Diddler.

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

/* bfd.h -- The only header file required by users of the bfd library 

This file is generated from various .c files, if you change it, your
bits may be lost.

All the prototypes and definitions following the comment "THE FOLLOWING
IS EXTRACTED FROM THE SOURCE" are extracted from the source files for
BFD.  If you change it, someone oneday will extract it from the source
again, and your changes will be lost.  To save yourself from this bind,
change the definitions in the source in the bfd directory.  Type "make
docs" and then "make headers" in that directory, and magically this file
will change to reflect your changes.

If you don't have the tools to perform the extraction, then you are
safe from someone on your system trampling over your header files.
You should still maintain the equivalence between the source and this
file though; every change you make to the .c file should be reflected
here.  */

#ifndef __BFD_H_SEEN__
#define __BFD_H_SEEN__

#include "ansidecl.h"
#include "obstack.h"

/* Make it easier to declare prototypes (puts conditional here) */
#ifndef PROTO
#	if __STDC__
#		define PROTO(type, name, arglist) type name arglist
#	else
#		define PROTO(type, name, arglist) type name ()
#	endif
#endif

#define BFD_VERSION "1.15"

/* forward declaration */
typedef struct _bfd bfd;

/* General rules: functions which are boolean return true on success
   and false on failure (unless they're a predicate).   -- bfd.doc */
/* I'm sure this is going to break something and someone is going to
   force me to change it. */
typedef enum boolean {false, true} boolean;

/* Try to avoid breaking stuff */
typedef  long int file_ptr;

/* Support for different sizes of target format ints and addresses */

#ifdef	HOST_64_BIT
typedef HOST_64_BIT rawdata_offset;
typedef HOST_64_BIT bfd_vma;
typedef HOST_64_BIT bfd_word;
typedef HOST_64_BIT bfd_offset;
typedef HOST_64_BIT bfd_size_type;
typedef HOST_64_BIT symvalue;
typedef HOST_64_BIT bfd_64_type;
#define fprintf_vma(s,x) \
		fprintf(s,"%08x%08x", uint64_typeHIGH(x), uint64_typeLOW(x))
#define printf_vma(x) \
		printf(   "%08x%08x", uint64_typeHIGH(x), uint64_typeLOW(x))
#else
typedef struct {int a,b;} bfd_64_type;
typedef unsigned long rawdata_offset;
typedef unsigned long bfd_vma;
typedef unsigned long bfd_offset;
typedef unsigned long bfd_word;
typedef unsigned long bfd_size;
typedef unsigned long symvalue;
typedef unsigned long bfd_size_type;
#define printf_vma(x)	 printf(    "%08lx", x)
#define fprintf_vma(s,x) fprintf(s, "%08lx", x)
#endif

typedef unsigned int flagword;	/* 32 bits of flags */

/** File formats */

typedef enum bfd_format {
	      bfd_unknown = 0,	/* file format is unknown */
	      bfd_object,	/* linker/assember/compiler output */
	      bfd_archive,	/* object archive file */
	      bfd_core,		/* core dump */
	      bfd_type_end}	/* marks the end; don't use it! */
         bfd_format;

/* Object file flag values */
#define NO_FLAGS    0
#define HAS_RELOC   001
#define EXEC_P      002
#define HAS_LINENO  004
#define HAS_DEBUG   010
#define HAS_SYMS    020
#define HAS_LOCALS  040
#define DYNAMIC     0100
#define WP_TEXT     0200
#define D_PAGED     0400


/* symbols and relocation */

typedef unsigned long symindex;

#define BFD_NO_MORE_SYMBOLS ((symindex) ~0)

typedef enum bfd_symclass {
	      bfd_symclass_unknown = 0,
	      bfd_symclass_fcommon, /* fortran common symbols */
	      bfd_symclass_global, /* global symbol, what a surprise */
	      bfd_symclass_debugger, /* some debugger symbol */
	      bfd_symclass_undefined /* none known */
	    } symclass;


typedef int symtype;		/* Who knows, yet? */


/* general purpose part of a symbol;
   target specific parts will be found in libcoff.h, liba.out.h etc */


#define bfd_get_section(x) ((x)->section)
#define bfd_get_output_section(x) ((x)->section->output_section)
#define bfd_set_section(x,y) ((x)->section) = (y)
#define bfd_asymbol_base(x) ((x)->section?((x)->section->vma):0)
#define bfd_asymbol_value(x) (bfd_asymbol_base(x) + x->value)
#define bfd_asymbol_name(x) ((x)->name)

/* This is a type pun with struct ranlib on purpose! */
typedef struct carsym {
  char *name;
  file_ptr file_offset;		/* look here to find the file */
} carsym;			/* to make these you call a carsymogen */

  
/* Used in generating armaps.  Perhaps just a forward definition would do? */
struct orl {			/* output ranlib */
  char **name;			/* symbol name */ 
  file_ptr pos;			/* bfd* or file position */
  int namidx;			/* index into string table */
};



/* Linenumber stuff */
typedef struct lineno_cache_entry {
  unsigned int line_number;	/* Linenumber from start of function*/  
  union {
 struct symbol_cache_entry *sym;		/* Function name */
    unsigned long offset;	/* Offset into section */
  } u;
} alent;

/* object and core file sections */


#define	align_power(addr, align)	\
	( ((addr) + ((1<<(align))-1)) & (-1 << (align)))

typedef struct sec *sec_ptr;

#define bfd_section_name(bfd, ptr) ((ptr)->name)
#define bfd_section_size(bfd, ptr) ((ptr)->size)
#define bfd_section_vma(bfd, ptr) ((ptr)->vma)
#define bfd_section_alignment(bfd, ptr) ((ptr)->alignment_power)
#define bfd_get_section_flags(bfd, ptr) ((ptr)->flags)
#define bfd_get_section_userdata(bfd, ptr) ((ptr)->userdata)

#define bfd_set_section_vma(bfd, ptr, val) (((ptr)->vma = (val)), true)
#define bfd_set_section_alignment(bfd, ptr, val) (((ptr)->alignment_power = (val)),true)
#define bfd_set_section_userdata(bfd, ptr, val) (((ptr)->userdata = (val)),true)

typedef struct stat stat_type; 

/** Error handling */

typedef enum bfd_error {
	      no_error = 0, system_call_error, invalid_target,
	      wrong_format, invalid_operation, no_memory,
	      no_symbols, no_relocation_info,
	      no_more_archived_files, malformed_archive,
	      symbol_not_found, file_not_recognized,
	      file_ambiguously_recognized, no_contents,
		bfd_error_nonrepresentable_section,
	      invalid_error_code} bfd_ec;

extern bfd_ec bfd_error;

typedef struct bfd_error_vector {
  PROTO(void,(* nonrepresentable_section ),(CONST bfd  *CONST abfd,
					    CONST char *CONST name));
} bfd_error_vector_type;

PROTO (char *, bfd_errmsg, ());
PROTO (void, bfd_perror, (CONST char *message));


typedef enum bfd_print_symbol
{ 
  bfd_print_symbol_name_enum,
  bfd_print_symbol_type_enum,
  bfd_print_symbol_all_enum
} bfd_print_symbol_enum_type;
    


/* The code that implements targets can initialize a jump table with this
   macro.  It must name all its routines the same way (a prefix plus
   the standard routine suffix), or it must #define the routines that
   are not so named, before calling JUMP_TABLE in the initializer.  */

/* Semi-portable string concatenation in cpp */
#ifndef CAT
#ifdef __STDC__
#define CAT(a,b) a##b
#else
#define CAT(a,b) a/**/b
#endif
#endif

#define JUMP_TABLE(NAME)\
CAT(NAME,_core_file_failing_command),\
CAT(NAME,_core_file_failing_signal),\
CAT(NAME,_core_file_matches_executable_p),\
CAT(NAME,_slurp_armap),\
CAT(NAME,_slurp_extended_name_table),\
CAT(NAME,_truncate_arname),\
CAT(NAME,_write_armap),\
CAT(NAME,_close_and_cleanup),	\
CAT(NAME,_set_section_contents),\
CAT(NAME,_get_section_contents),\
CAT(NAME,_new_section_hook),\
CAT(NAME,_get_symtab_upper_bound),\
CAT(NAME,_get_symtab),\
CAT(NAME,_get_reloc_upper_bound),\
CAT(NAME,_canonicalize_reloc),\
CAT(NAME,_make_empty_symbol),\
CAT(NAME,_print_symbol),\
CAT(NAME,_get_lineno),\
CAT(NAME,_set_arch_mach),\
CAT(NAME,_openr_next_archived_file),\
CAT(NAME,_find_nearest_line),\
CAT(NAME,_generic_stat_arch_elt),\
CAT(NAME,_sizeof_headers),\
CAT(NAME,_bfd_debug_info_start),\
CAT(NAME,_bfd_debug_info_end),\
CAT(NAME,_bfd_debug_info_accumulate)

#define COFF_SWAP_TABLE coff_swap_aux_in, coff_swap_sym_in, coff_swap_lineno_in,

/* User program access to BFD facilities */

extern CONST short _bfd_host_big_endian;
#define HOST_BYTE_ORDER_BIG_P	(*(char *)&_bfd_host_big_endian)

/* The bfd itself */

/* Cast from const char * to char * so that caller can assign to
   a char * without a warning.  */
#define bfd_get_filename(abfd) ((char *) (abfd)->filename)
#define bfd_get_format(abfd) ((abfd)->format)
#define bfd_get_target(abfd) ((abfd)->xvec->name)
#define bfd_get_file_flags(abfd) ((abfd)->flags)
#define bfd_applicable_file_flags(abfd) ((abfd)->xvec->object_flags)
#define bfd_applicable_section_flags(abfd) ((abfd)->xvec->section_flags)
#define bfd_my_archive(abfd) ((abfd)->my_archive);
#define bfd_has_map(abfd) ((abfd)->has_armap)
#define bfd_header_twiddle_required(abfd) \
        ((((abfd)->xvec->header_byteorder_big_p)		\
	  != (boolean)HOST_BYTE_ORDER_BIG_P) ? true:false)

#define bfd_valid_reloc_types(abfd) ((abfd)->xvec->valid_reloc_types)
#define bfd_usrdata(abfd) ((abfd)->usrdata)

#define bfd_get_start_address(abfd) ((abfd)->start_address)
#define bfd_get_symcount(abfd) ((abfd)->symcount)
#define bfd_get_outsymbols(abfd) ((abfd)->outsymbols)
#define bfd_count_sections(abfd) ((abfd)->section_count)
#define bfd_get_architecture(abfd) ((abfd)->obj_arch)
#define bfd_get_machine(abfd) ((abfd)->obj_machine)



#define BYTE_SIZE 1
#define SHORT_SIZE 2
#define LONG_SIZE 4



/*THE FOLLOWING IS EXTRACTED FROM THE SOURCE */

/* Opens the file supplied (using fopen) with the target supplied, it
returns a pointer to the created bfd.

If NULL is returned then an error has occured.
Possible errors are no_memory, invalid_target or system_call error.
*/
PROTO(bfd*, bfd_openr, (CONST char *filename,CONST char*target));
/* bfd_fdopenr is to bfd_fopenr much like  fdopen is to fopen. It opens a bfd on
a file already described by the @var{fd} supplied. 

Possible errors are no_memory, invalid_target and system_call error.
*/
 PROTO(bfd *, bfd_fdopenr,
    (CONST char *filename, CONST char *target, int fd));
/* Creates a bfd, associated with file @var{filename}, using the file
format @var{target}, and returns a pointer to it.

Possible errors are system_call_error, no_memory, invalid_target.
*/
PROTO(bfd *, bfd_openw, (CONST char *filename, CONST char *target));
/* This function closes a bfd. If the bfd was open for writing, then
pending operations are completed and the file written out and closed.
If the created file is executable, then @code{chmod} is called to mark
it as such.

All memory attatched to the bfd's obstacks is released. 

@code{true} is returned if all is ok, otherwise @code{false}.
*/
PROTO(boolean, bfd_close,(bfd *));
/* This routine creates a new bfd in the manner of bfd_openw, but without
opening a file. The new bfd takes the target from the target used by
@var{template}. The format is always set to @code{bfd_object}.

*/
PROTO(bfd *, bfd_create, (CONST char *filename, bfd *template));
/* Return the number of bytes in the obstacks connected to the supplied
bfd.
*/
PROTO(bfd_size_type,bfd_alloc_size,(bfd *abfd));
/* This enum gives the object file's CPU
architecture, in a global sense.  E.g. what processor family does it
belong to?  There is another field, which indicates what processor
within the family is in use.  The machine gives a number which
distingushes different versions of the architecture, containing for
example 2 and 3 for Intel i960 KA and i960 KB, and 68020 and 68030 for
Motorola 68020 and 68030.

*/
enum bfd_architecture 
{
  bfd_arch_unknown,   /* File arch not known */
  bfd_arch_obscure,   /* Arch known, not one of these */
  bfd_arch_m68k,      /* Motorola 68xxx */
  bfd_arch_vax,       /* DEC Vax */   
  bfd_arch_i960,      /* Intel 960 */
    /* The order of the following is important.
       lower number indicates a machine type that 
       only accepts a subset of the instructions
       available to machines with higher numbers.
       The exception is the "ca", which is
       incompatible with all other machines except 
       "core". */

#define bfd_mach_i960_core      1
#define bfd_mach_i960_ka_sa     2
#define bfd_mach_i960_kb_sb     3
#define bfd_mach_i960_mc        4
#define bfd_mach_i960_xa        5
#define bfd_mach_i960_ca        6

  bfd_arch_a29k,      /* AMD 29000 */
  bfd_arch_sparc,     /* SPARC */
  bfd_arch_mips,      /* MIPS Rxxxx */
  bfd_arch_i386,      /* Intel 386 */
  bfd_arch_ns32k,     /* National Semiconductor 32xxx */
  bfd_arch_tahoe,     /* CCI/Harris Tahoe */
  bfd_arch_i860,      /* Intel 860 */
  bfd_arch_romp,      /* IBM ROMP RS/6000 */
  bfd_arch_alliant,   /* Alliant */
  bfd_arch_convex,    /* Convex */
  bfd_arch_m88k,      /* Motorola 88xxx */
  bfd_arch_pyramid,   /* Pyramid Technology */
  bfd_arch_h8_300,    /* Hitachi H8/300 */
  bfd_arch_last
  };

/* stuff
*/

/* Return a printable string representing the architecture and machine
type. The result is only good until the next call to
bfd_printable_arch_mach.  
*/
PROTO(CONST char *,bfd_printable_arch_mach,
    (enum bfd_architecture arch, unsigned long machine));
/* Scan a string and attempt to turn it into an archive and machine type combination.  
*/
PROTO(boolean, bfd_scan_arch_mach,
    (CONST char *, enum bfd_architecture *, unsigned long *));
/* This routine is used to determine whether two BFDs' architectures and machine types are
compatible.  It calculates the lowest common denominator between the
two architectures and machine types implied by the bfds and sets the
objects pointed at by @var{archp} and @var{machine} if non NULL. 

This routine returns @code{true} if the bfds are of compatible type,
otherwise @code{false}.
*/
PROTO(boolean, bfd_arch_compatible,
     (bfd *abfd,
     bfd *bbfd,
     enum bfd_architecture *archp,
     unsigned long *machinep));
/* Set atch mach
*/
#define bfd_set_arch_mach(abfd, arch, mach) \
     BFD_SEND (abfd, _bfd_set_arch_mach,\
                    (abfd, arch, mach))

/* These macros as used for reading and writing raw data in sections;
each access (except for bytes) is vectored through the target format
of the bfd and mangled accordingly. The mangling performs any
necessary endian translations and removes alignment restrictions.
*/
#define bfd_put_8(abfd, val, ptr) \
                (*((char *)ptr) = (char)val)
#define bfd_get_8(abfd, ptr) \
                (*((char *)ptr))
#define bfd_put_16(abfd, val, ptr) \
                BFD_SEND(abfd, bfd_putx16, (val,ptr))
#define bfd_get_16(abfd, ptr) \
                BFD_SEND(abfd, bfd_getx16, (ptr))
#define bfd_put_32(abfd, val, ptr) \
                BFD_SEND(abfd, bfd_putx32, (val,ptr))
#define bfd_get_32(abfd, ptr) \
                BFD_SEND(abfd, bfd_getx32, (ptr))
#define bfd_put_64(abfd, val, ptr) \
                BFD_SEND(abfd, bfd_putx64, (val, ptr))
#define bfd_get_64(abfd, ptr) \
                BFD_SEND(abfd, bfd_getx64, (ptr))

/* These macros have the same function as their @code{bfd_get_x}
bretherin, except that they are used for removing information for the
header records of object files. Believe it or not, some object files
keep their header records in big endian order, and their data in little
endan order.
*/
#define bfd_h_put_8(abfd, val, ptr) \
                (*((char *)ptr) = (char)val)
#define bfd_h_get_8(abfd, ptr) \
                (*((char *)ptr))
#define bfd_h_put_16(abfd, val, ptr) \
                BFD_SEND(abfd, bfd_h_putx16,(val,ptr))
#define bfd_h_get_16(abfd, ptr) \
                BFD_SEND(abfd, bfd_h_getx16,(ptr))
#define bfd_h_put_32(abfd, val, ptr) \
                BFD_SEND(abfd, bfd_h_putx32,(val,ptr))
#define bfd_h_get_32(abfd, ptr) \
                BFD_SEND(abfd, bfd_h_getx32,(ptr))
#define bfd_h_put_64(abfd, val, ptr) \
                BFD_SEND(abfd, bfd_h_putx64,(val, ptr))
#define bfd_h_get_64(abfd, ptr) \
                BFD_SEND(abfd, bfd_h_getx64,(ptr))

/* The shape of a section struct:

*/
typedef struct sec {

    /* The name of the section, the name isn't a copy, the pointer is
    the same as that passed to bfd_make_section.
    */
    CONST char *name;

    /* The next section in the list belonging to the bfd, or NULL.
    */
    struct sec *next;

    /* The field flags contains attributes of the section. Some of these
    flags are read in from the object file, and some are synthesized from
    other information. 
    */
flagword flags;
#define SEC_NO_FLAGS   0x000

    /* Tells the OS to allocate space for this section when loaded.
    This would clear for a section containing debug information only.
    */
#define SEC_ALLOC      0x001

    /* Tells the OS to load the section from the file when loading.
    This would be clear for a .bss section 
    */
#define SEC_LOAD       0x002

    /* The section contains data still to be relocated, so there will be some
    relocation information too.
    */
#define SEC_RELOC      0x004

    /* Obsolete ? 
    */
#define SEC_BALIGN     0x008

    /* A signal to the OS that the section contains read only data.
    */
#define SEC_READONLY   0x010

    /* The section contains code only.
    */
#define SEC_CODE       0x020

    /* The section contains data only.
    */
#define SEC_DATA        0x040

    /* The section will reside in ROM.
    */
#define SEC_ROM        0x080

    /* The section contains constructor information. This section type is
    used by the linker to create lists of constructors and destructors
    used by @code{g++}. When a back end sees a symbol which should be used
    in a constructor list, it creates a new section for the type of name
    (eg @code{__CTOR_LIST__}), attatches the symbol to it and builds a
    relocation. To build the lists of constructors, all the linker has to
    to is catenate all the sections called @code{__CTOR_LIST__} and
    relocte the data contained within - exactly the operations it would
    peform on standard data.
    */
#define SEC_CONSTRUCTOR 0x100

    /* The section has contents - a bss section could be
    @code{SEC_ALLOC} | @code{SEC_HAS_CONTENTS}, a debug section could be
    @code{SEC_HAS_CONTENTS}
    */
#define SEC_HAS_CONTENTS 0x200

    /* An instruction to the linker not to output sections containing
    this flag even if they have information which would normally be written.
    */
#define SEC_NEVER_LOAD 0x400

    /* The base address of the section in the address space of the target.
    */
   bfd_vma vma;

    /* The size of the section in bytes of the loaded section. This contains
    a value even if the section has no contents (eg, the size of @code{.bss}).
    */
   bfd_size_type size;    

    /* If this section is going to be output, then this value is the
    offset into the output section of the first byte in the input
    section. Eg, if this was going to start at the 100th byte in the
    output section, this value would be 100. 
    */
   bfd_vma output_offset;

    /* The output section through which to map on output.
    */
   struct sec *output_section;

    /* The alignment requirement of the section, as an exponent - eg 3
    aligns to 2^3 (or 8) 
    */
   unsigned int alignment_power;

    /* If an input section, a pointer to a vector of relocation records for
    the data in this section.
    */
   struct reloc_cache_entry *relocation;

    /* If an output section, a pointer to a vector of pointers to
    relocation records for the data in this section.
    */
   struct reloc_cache_entry **orelocation;

    /* The number of relocation records in one of the above 
    */
   unsigned reloc_count;

    /* Which section is it 0..nth     
    */
   int index;                      

    /* Information below is back end specific - and not always used or
    updated 
    
    File position of section data   
    */
   file_ptr filepos;      

    /* File position of relocation info        
    */
   file_ptr rel_filepos;

    /* File position of line data              
    */
   file_ptr line_filepos;

    /* Pointer to data for applications        
    */
   PTR userdata;
   struct lang_output_section *otheruserdata;

    /* Attached line number information        
    */
   alent *lineno;

    /* Number of line number records   
    */
   unsigned int lineno_count;

    /* When a section is being output, this value changes as more
    linenumbers are written out 
    */
   file_ptr moving_line_filepos;

    /* what the section number is in the target world 
    */
   unsigned int target_index;
   PTR used_by_bfd;

    /* If this is a constructor section then here is a list of the
    relocations created to relocate items within it.
    */
   struct relent_chain *constructor_chain;

    /* The bfd which owns the section.
    */
   bfd *owner;
} asection ;

/* Runs through the provided @var{abfd} and returns the @code{asection}
who's name matches that provided, otherwise NULL. @xref{Sections}, for more information.

*/
PROTO(asection *, bfd_get_section_by_name,
    (bfd *abfd, CONST char *name));
/* This function creates a new empty section called @var{name} and attatches it
to the end of the chain of sections for @var{bfd}. An attempt to
create a section with a name which is already in use, returns the old
section by that name instead.

Possible errors are:
@table @code
@item invalid_operation
If output has already started for this bfd.
@item no_memory
If obstack alloc fails.
@end table

*/
PROTO(asection *, bfd_make_section, (bfd *, CONST char *name));
/* Attempts to set the attributes of the section named in the bfd
supplied to the value. Returns true on success, false on error.
Possible error returns are:
@table @code
@item invalid operation
The section cannot have one or more of the attributes requested. For
example, a .bss section in @code{a.out} may not have the
@code{SEC_HAS_CONTENTS} field set.
@end table

*/
PROTO(boolean, bfd_set_section_flags,
       (bfd *, asection *, flagword));
/* Calls the provided function @var{func} for each section attatched to
the bfd @var{abfd}, passing @var{obj} as an argument. The function
will be called as if by 

@example
  func(abfd, the_section, obj);
@end example


*/
PROTO(void, bfd_map_over_sections,
            (bfd *abfd, void (*func)(), PTR obj));
/* This is the prefered method for iterating over sections, an
alternative would be to use a loop:

@example
   section *p;
   for (p = abfd->sections; p != NULL; p = p->next)
      func(abfd, p, ...)
@end example*/

/* Sets @var{section} to the size @var{val}. If the operation is ok, then
@code{true} is returned, else @code{false}. 

Possible error returns:
@table @code
@item invalid_operation
Writing has started to the bfd, so setting the size is invalid
@end table 

*/
PROTO(boolean, bfd_set_section_size,
     (bfd *, asection *, bfd_size_type val));
/* Sets the contents of the section @var{section} in bfd @var{abfd} to
the data starting in memory at @var{data}. The data is written to the
output section starting at offset @var{offset} for @var{count} bytes.

Normally @code{true} is returned, else @code{false}. Possible error
returns are:
@table @code
@item no_contents
The output section does not have the @code{SEC_HAS_CONTENTS}
attribute, so nothing can be written to it.
@item and some more too
@end table
This routine is front end to the back end function @code{_bfd_set_section_contents}.

*/
PROTO(boolean, bfd_set_section_contents,
         (bfd *abfd,        
         asection *section,
         PTR data,
         file_ptr offset,
         bfd_size_type count));
/* This function reads data from @var{section} in bfd @var{abfd} into
memory starting at @var{location}. The data is read at an offset of
@var{offset} from the start of the input section, and is read for
@var{count} bytes.

If the contents of a constuctor with the @code{SEC_CONSTUCTOR} flag
set are requested, then the @var{location} is filled with zeroes.

If no errors occur, @code{true} is returned, else @code{false}.
Possible errors are:

@table @code
@item unknown yet
@end table

*/
PROTO(boolean, bfd_get_section_contents, 
        (bfd *abfd, asection *section, PTR location,
         file_ptr offset, bfd_size_type count));
/* @subsection typedef asymbol
An @code{asymbol} has the form:

*/
typedef struct symbol_cache_entry 
{

    /* A pointer to the bfd which owns the symbol. This information is
    necessary so that a back end can work out what additional (invisible to
    the application writer) information is carried with the symbol. 
    */
  struct _bfd *the_bfd;

    /* The text of the symbol. The name is left alone, and not copied - the
    application may not alter it. 
    */
   CONST char *name;

    /* The value of the symbol.
    */
   symvalue value;

    /* Attributes of a symbol:
    */
#define BSF_NO_FLAGS    0x00

    /* The symbol has local scope; @code{static} in @code{C}. The value is
    the offset into the section of the data.
    */
#define BSF_LOCAL	0x01

    /* The symbol has global scope; initialized data in @code{C}. The value
    is the offset into the section of the data.
    */
#define BSF_GLOBAL	0x02

    /* Obsolete
    */
#define BSF_IMPORT	0x04

    /* The symbol has global scope, and is exported. The value is the offset
    into the section of the data.
    */
#define BSF_EXPORT	0x08

    /* The symbol is undefined. @code{extern} in @code{C}. The value has no meaning.
    */
#define BSF_UNDEFINED	0x10	

    /* The symbol is common, initialized to zero; default in @code{C}. The
    value is the size of the object in bytes.
    */
#define BSF_FORT_COMM	0x20	

    /* A normal @code{C} symbol would be one of:
    @code{BSF_LOCAL}, @code{BSF_FORT_COMM},  @code{BSF_UNDEFINED} or @code{BSF_EXPORT|BSD_GLOBAL}
    
    The symbol is a debugging record. The value has an arbitary meaning.
    */
#define BSF_DEBUGGING	0x40

    /* The symbol has no section attached, any value is the actual value and
    is not a relative offset to a section.
    */
#define BSF_ABSOLUTE	0x80

    /* Used by the linker
    */
#define BSF_KEEP        0x10000
#define BSF_WARNING     0x20000
#define BSF_KEEP_G      0x80000

    /* Unused
    */
#define BSF_WEAK        0x100000
#define BSF_CTOR        0x200000 
#define BSF_FAKE        0x400000 

    /* The symbol used to be a common symbol, but now it is allocated.
    */
#define BSF_OLD_COMMON  0x800000  

    /* The default value for common data.
    */
#define BFD_FORT_COMM_DEFAULT_VALUE 0

    /* In some files the type of a symbol sometimes alters its location
    in an output file - ie in coff a @code{ISFCN} symbol which is also @code{C_EXT}
    symbol appears where it was declared and not at the end of a section. 
    This bit is set by the target bfd part to convey this information. 
    */
#define BSF_NOT_AT_END    0x40000
  flagword flags;

    /* Aointer to the section to which this symbol is relative, or 0 if the
    symbol is absolute or undefined. Note that it is not sufficient to set
    this location to 0 to mark a symbol as absolute - the flag
    @code{BSF_ABSOLUTE} must be set also.
    */
  struct sec *section;

    /* Back end special data. This is being phased out in favour of making
    this a union.
    */
  PTR udata;	
} asymbol;

/* Returns the number of bytes required in a vector of pointers to
@code{asymbols} for all the symbols in the supplied bfd, including a
terminal NULL pointer. If there are no symbols in the bfd, then 0 is
returned.
*/
#define get_symtab_upper_bound(abfd) \
     BFD_SEND (abfd, _get_symtab_upper_bound, (abfd))

/* Supplied a bfd and a pointer to an uninitialized vector of pointers.
This reads in the symbols from the bfd, and fills in the table with
pointers to the symbols, and a trailing NULL. The routine returns the
actual number of symbol pointers not including the NULL.

*/
#define bfd_canonicalize_symtab(abfd, location) \
     BFD_SEND (abfd, _bfd_canonicalize_symtab,\
                  (abfd, location))


/* Provided a table of pointers to to symbols and a count, writes to the
output bfd the symbols when closed.

*/
PROTO(boolean, bfd_set_symtab, (bfd *, asymbol **, unsigned int ));
/* Prints the value and flags of the symbol supplied to the stream file.

*/
PROTO(void, bfd_print_symbol_vandf, (PTR file, asymbol *symbol));
/* This function creates a new @code{asymbol} structure for the bfd, and
returns a pointer to it.

This routine is necessary, since each back end has private information
surrounding the @code{asymbol}. Building your own @code{asymbol} and
pointing to it will not create the private information, and will cause
problems later on.
*/
#define bfd_make_empty_symbol(abfd) \
     BFD_SEND (abfd, _bfd_make_empty_symbol, (abfd))

/* @section typedef bfd

Pointers to bfd structs are the cornerstone of any application using
libbfd. References though the bfd and to data in the bfd give the
entire bfd functionality.

Finally!  The BFD struct itself.  This contains the major data about
the file, and contains pointers to the rest of the data.

*/
struct _bfd 
{

    /*   The filename the application opened the bfd with.
    */
  CONST char *filename;                

    /* A pointer to the target jump table.
    */
  struct bfd_target *xvec;

    /* To avoid dragging too many header files into every file that
    includes bfd.h, IOSTREAM has been declared as a "char *", and MTIME
    as a "long".  Their correct types, to which they are cast when used,
    are "FILE *" and "time_t".  
    
    The iostream is the result of an fopen on the filename.
    */
  char *iostream;

    /* Is the file being cached @xref{File Caching}.
    */
  boolean cacheable;

    /* Marks whether there was a default target specified when the bfd was
    opened. This is used to select what matching algorithm to use to chose
    the back end.
    */
  boolean target_defaulted;

    /* The caching routines use these to maintain an LRU list of bfds.
    */
  struct _bfd *lru_prev, *lru_next;

    /* When a file is closed by the caching routines, it retains the state
    here:
    */
  file_ptr where;              

    /* and here:
    */
  boolean opened_once;
  boolean mtime_set;

    /* File modified time 
    */
  long mtime;          

    /* For output files, channel we locked (is this used?).
    */
int ifd;

    /* The format which belongs to the bfd.
    */
  bfd_format format;

    /* The direction the bfd was opened with
    */
  enum bfd_direction {no_direction = 0,
                       read_direction = 1,
                       write_direction = 2,
                       both_direction = 3} direction;

    /* Format_specific flags
    */
  flagword flags;              

    /* Currently my_archive is tested before adding origin to anything. I
    believe that this can become always an add of origin, with origin set
    to 0 for non archive files.  
    */
  file_ptr origin;             

    /* Remember when output has begun, to stop strange things happening.
    */
  boolean output_has_begun;

    /* Pointer to linked list of sections
    */
  struct sec  *sections;

    /* The number of sections 
    */
  unsigned int section_count;

    /* Stuff only usefull for object files:
    The start address.
    */
  bfd_vma start_address;

    /* Used for input and output
    */
  unsigned int symcount;

    /* Symtab for output bfd
    */
  struct symbol_cache_entry  **outsymbols;             

    /* Architecture of object machine, eg m68k 
    */
  enum bfd_architecture obj_arch;

    /* Particular machine within arch, e.g. 68010
    */
  unsigned long obj_machine;

    /* Stuff only usefull for archives:
    */
  PTR arelt_data;              
  struct _bfd *my_archive;     
  struct _bfd *next;           
  struct _bfd *archive_head;   
  boolean has_armap;           

    /* Used by the back end to hold private data.
    */
  PTR tdata;

    /* Used by the application to hold private data
    */
  PTR usrdata;

    /* Where all the allocated stuff under this BFD goes 
    */
  struct obstack memory;
};

/* Marks the entry point of an output bfd. Returns @code{true} on
success, @code{false} otherwise.

*/
PROTO(boolean, bfd_set_start_address,(bfd *, bfd_vma));
/* Return cached file modification time (e.g. as read from archive header
for archive members, or from file system if we have been called
before); else determine modify time, cache it, and return it.  

*/
PROTO(long, bfd_get_mtime, (bfd *));
#define bfd_sizeof_headers(abfd, reloc) \
     BFD_SEND (abfd, _bfd_sizeof_headers, (abfd, reloc))

#define bfd_find_nearest_line(abfd, section, symbols, offset, filename_ptr, func, line_ptr) \
     BFD_SEND (abfd, _bfd_find_nearest_line,  (abfd, section, symbols, offset, filename_ptr, func, line_ptr))

#define bfd_debug_info_start(abfd) \
        BFD_SEND (abfd, _bfd_debug_info_start, (abfd))

#define bfd_debug_info_end(abfd) \
        BFD_SEND (abfd, _bfd_debug_info_end, (abfd))

#define bfd_debug_info_accumulate(abfd, section) \
        BFD_SEND (abfd, _bfd_debug_info_accumulate, (abfd, section))

#define bfd_stat_arch_elt(abfd, stat) \
        BFD_SEND (abfd, _bfd_stat_arch_elt,(abfd, stat))

#define bfd_coff_swap_aux_in(a,e,t,c,i) \
        BFD_SEND (a, _bfd_coff_swap_aux_in, (a,e,t,c,i))

#define bfd_coff_swap_sym_in(a,e,i) \
        BFD_SEND (a, _bfd_coff_swap_sym_in, (a,e,i))

#define bfd_coff_swap_lineno_in(a,e,i) \
        BFD_SEND ( a, _bfd_coff_swap_lineno_in, (a,e,i))

/* What this does
*/
PROTO(symindex, bfd_get_next_mapent, (bfd *, symindex, carsym **));
/* Used whilst processing archives. Sets the head of the chain of bfds
contained in an archive to @var{new_head}. (see chapter on archives)
*/
PROTO(boolean, bfd_set_archive_head, (bfd *output, bfd *new_head));

PROTO(bfd *, bfd_get_elt_at_index, (bfd *, int));


/* Initially provided a bfd containing an archive and NULL, opens a bfd
on the first contained element and returns that. Subsequent calls to
bfd_openr_next_archived_file should pass the archive and the previous
return value to return a created bfd to the next contained element.
NULL is returned when there are no more.

*/
PROTO(bfd*, bfd_openr_next_archived_file,
               (bfd *archive, bfd *previous));
/* Returns a read-only string explaining what program was running when
it failed and produced the core file being read

*/
PROTO(CONST char *, bfd_core_file_failing_command, (bfd *));
/* Returns the signal number which caused the core dump which generated
the file the bfd is attatched to.

*/
PROTO(int, bfd_core_file_failing_signal, (bfd *));
/* Returns @code{true} if the core file attatched to @var{core_bfd} was
generated by a run of the executable file attatched to @var{exec_bfd},
or else @code{false}.
*/
PROTO(boolean, core_file_matches_executable_p,
    (bfd *core_bfd, bfd *exec_bfd));
/* The relocation routine returns as a status an enumerated type:

*/
typedef enum bfd_reloc_status {

    /* No errors detected
    */
  bfd_reloc_ok,

    /* The relocation was performed, but there was an overflow.
    */
  bfd_reloc_overflow,

    /* The address to relocate was not within the section supplied
    */
  bfd_reloc_outofrange,

    /* Used by special functions
    */
  bfd_reloc_continue,

    /* Unused 
    */
  bfd_reloc_notsupported,

    /* Unsupported relocation size requested. 
    */
  bfd_reloc_other,

    /* The symbol to relocate against was undefined.
    */
  bfd_reloc_undefined,

    /* The relocaction was performed, but may not be ok - presently generated
    only when linking i960 coff files with i960 b.out symbols.
    */
  bfd_reloc_dangerous
   }
 bfd_reloc_status_enum_type;
typedef struct reloc_cache_entry 
{

    /* A pointer into the canonical table of pointers 
    */
  struct symbol_cache_entry **sym_ptr_ptr;

    /* offset in section                 
    */
  rawdata_offset address;

    /* addend for relocation value        
    */
  bfd_vma addend;    

    /* if sym is null this is the section 
    */
  struct sec *section;

    /* Pointer to how to perform the required relocation
    */
  CONST struct reloc_howto_struct *howto;
} arelent;

/* The @code{reloc_howto_type} is a structure which contains all the
information that bfd needs to know to tie up a back end's data.

*/
typedef CONST struct reloc_howto_struct 
{ 

    /* The type field has mainly a documetary use - the back end can to what
    it wants with it, though the normally the back end's external idea of
    what a reloc number would be would be stored in this field. For
    example, the a PC relative word relocation in a coff environment would
    have the type 023 - because that's what the outside world calls a
    R_PCRWORD reloc.
    */
  unsigned int type;

    /* The value the final relocation is shifted right by. This drops
    unwanted data from the relocation. 
    */
  unsigned int rightshift;

    /* The size of the item to be relocated - 0, is one byte, 1 is 2 bytes, 3
    is four bytes.
    */
  unsigned int size;

    /* Now obsolete
    */
  unsigned int bitsize;

    /* Notes that the relocation is relative to the location in the data
    section of the addend. The relocation function will subtract from the
    relocation value the address of the location being relocated.
    */
  boolean pc_relative;

    /* Now obsolete
    */
  unsigned int bitpos;

    /* Now obsolete
    */
  boolean absolute;

    /* Causes the relocation routine to return an error if overflow is
    detected when relocating.
    */
  boolean complain_on_overflow;

    /* If this field is non null, then the supplied function is called rather
    than the normal function. This allows really strange relocation
    methods to be accomodated (eg, i960 callj instructions).
    */
  bfd_reloc_status_enum_type (*special_function)();

    /* The textual name of the relocation type.
    */
  char *name;

    /* When performing a partial link, some formats must modify the
    relocations rather than the data - this flag signals this.
    */
  boolean partial_inplace;

    /* The src_mask is used to select what parts of the read in data are to
    be used in the relocation sum. Eg, if this was an 8 bit bit of data
    which we read and relocated, this would be 0x000000ff. When we have
    relocs which have an addend, such as sun4 extended relocs, the value
    in the offset part of a relocating field is garbage so we never use
    it. In this case the mask would be 0x00000000.
    */
  bfd_word src_mask;

    /* The dst_mask is what parts of the instruction are replaced into the
    instruction. In most cases src_mask == dst_mask, except in the above
    special case, where dst_mask would be 0x000000ff, and src_mask would
    be 0x00000000.
    */
  bfd_word dst_mask;           

    /* When some formats create PC relative instructions, they leave the
    value of the pc of the place being relocated in the offset slot of the
    instruction, so that a PC relative relocation can be made just by
    adding in an ordinary offset (eg sun3 a.out). Some formats leave the
    displacement part of an instruction empty (eg m88k bcs), this flag
    signals the fact.
    */
  boolean pcrel_offset;
} reloc_howto_type;

/* The HOWTO define is horrible and will go away.
*/
#define HOWTO(C, R,S,B, P, BI, ABS, O, SF, NAME, INPLACE, MASKSRC, MASKDST, PC) \
  {(unsigned)C,R,S,B, P, BI, ABS,O,SF,NAME,INPLACE,MASKSRC,MASKDST,PC}
typedef unsigned char bfd_byte;

typedef struct relent_chain {
  arelent relent;
  struct   relent_chain *next;
} arelent_chain;


/* If an output_bfd is supplied to this function the generated image
will be relocatable, the relocations are copied to the output file
after they have been changed to reflect the new state of the world.
There are two ways of reflecting the results of partial linkage in an
output file; by modifying the output data in place, and by modifying
the relocation record. Some native formats (eg basic a.out and basic
coff) have no way of specifying an addend in the relocation type, so
the addend has to go in the output data.  This is no big deal since in
these formats the output data slot will always be big enough for the
addend. Complex reloc types with addends were invented to solve just
this problem.
*/
PROTO(bfd_reloc_status_enum_type,
                bfd_perform_relocation,
                        (bfd * abfd,
                        arelent *reloc_entry,
                        PTR data,
                        asection *input_section,
                        bfd *output_bfd));
/* @node bfd_target
@subsection bfd_target
This structure contains everything that BFD knows about a target.
It includes things like its byte order, name, what routines to call
to do various operations, etc.   

Every BFD points to a target structure with its "xvec" member. 


Shortcut for declaring fields which are prototyped function pointers,
while avoiding anguish on compilers that don't support protos.
*/
#define SDEF(ret, name, arglist) \
                PROTO(ret,(*name),arglist)
#define SDEF_FMT(ret, name, arglist) \
                PROTO(ret,(*name[bfd_type_end]),arglist)

/* These macros are used to dispatch to functions through the bfd_target
vector. They are used in a number of macros further down in bfd.h, and
are also used when calling various routines by hand inside the bfd
implementation.  The "arglist" argument must be parenthesized; it
contains all the arguments to the called function.
*/
#define BFD_SEND(bfd, message, arglist) \
               ((*((bfd)->xvec->message)) arglist)

/* For operations which index on the bfd format 
*/
#define BFD_SEND_FMT(bfd, message, arglist) \
            (((bfd)->xvec->message[(int)((bfd)->format)]) arglist)

/* This is the struct which defines the type of BFD this is.  The
"xvec" member of the struct bfd itself points here.  Each module
that implements access to a different target under BFD, defines
one of these.

FIXME, these names should be rationalised with the names of the
entry points which call them. Too bad we can't have one macro to
define them both! 

*/
typedef struct bfd_target
{

    /* identifies the kind of target, eg SunOS4, Ultrix, etc 
    */
  char *name;

    /* The "flavour" of a back end is a general indication about the contents
    of a file.
    */
  enum target_flavour_enum {
    bfd_target_aout_flavour_enum,
    bfd_target_coff_flavour_enum,
    bfd_target_ieee_flavour_enum,
    bfd_target_oasys_flavour_enum,
    bfd_target_srec_flavour_enum} flavour;

    /* The order of bytes within the data area of a file.
    */
  boolean byteorder_big_p;

    /* The order of bytes within the header parts of a file.
    */
  boolean header_byteorder_big_p;

    /* This is a mask of all the flags which an executable may have set -
    from the set @code{NO_FLAGS}, @code{HAS_RELOC}, ...@code{D_PAGED}.
    */
  flagword object_flags;       

    /* This is a mask of all the flags which a section may have set - from
    the set @code{SEC_NO_FLAGS}, @code{SEC_ALLOC}, ...@code{SET_NEVER_LOAD}.
    */
  flagword section_flags;

    /* The pad character for filenames within an archive header.
    */
  char ar_pad_char;            

    /* The maximum number of characters in an archive header.
    */
 unsigned short ar_max_namelen;

    /* The minimum alignment restriction for any section.
    */
  unsigned int align_power_min;

    /* Entries for byte swapping for data. These are different to the other
    entry points, since they don't take bfd as first arg.  Certain other handlers
    could do the same.
    */
  SDEF (bfd_vma,      bfd_getx64, (bfd_byte *));
  SDEF (void,         bfd_putx64, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma, bfd_getx32, (bfd_byte *));
  SDEF (void,         bfd_putx32, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma, bfd_getx16, (bfd_byte *));
  SDEF (void,         bfd_putx16, (bfd_vma, bfd_byte *));

    /* Byte swapping for the headers
    */
  SDEF (bfd_vma,   bfd_h_getx64, (bfd_byte *));
  SDEF (void,          bfd_h_putx64, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma,  bfd_h_getx32, (bfd_byte *));
  SDEF (void,          bfd_h_putx32, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma,  bfd_h_getx16, (bfd_byte *));
  SDEF (void,          bfd_h_putx16, (bfd_vma, bfd_byte *));

    /* Format dependent routines, these turn into vectors of entry points
    within the target vector structure; one for each format to check.
    
    Check the format of a file being read.  Return bfd_target * or zero. 
    */
  SDEF_FMT (struct bfd_target *, _bfd_check_format, (bfd *));

    /* Set the format of a file being written.  
    */
  SDEF_FMT (boolean,            _bfd_set_format, (bfd *));

    /* Write cached information into a file being written, at bfd_close. 
    */
  SDEF_FMT (boolean,            _bfd_write_contents, (bfd *));

    /* The following functions are defined in @code{JUMP_TABLE}. The idea is
    that the back end writer of @code{foo} names all the routines
    @code{foo_}@var{entry_point}, @code{JUMP_TABLE} will built the entries
    in this structure in the right order.
    
    Core file entry points
    */
  SDEF (char *, _core_file_failing_command, (bfd *));
  SDEF (int,    _core_file_failing_signal, (bfd *));
  SDEF (boolean, _core_file_matches_executable_p, (bfd *, bfd *));

    /* Archive entry points
    */
 SDEF (boolean, _bfd_slurp_armap, (bfd *));
 SDEF (boolean, _bfd_slurp_extended_name_table, (bfd *));
 SDEF (void,   _bfd_truncate_arname, (bfd *, CONST char *, char *));
 SDEF (boolean, write_armap, (bfd *arch, 
                              unsigned int elength,
                              struct orl *map,
                              int orl_count, 
                              int stridx));

    /* Standard stuff.
    */
  SDEF (boolean, _close_and_cleanup, (bfd *));
  SDEF (boolean, _bfd_set_section_contents, (bfd *, sec_ptr, PTR,
                                            file_ptr, bfd_size_type));
  SDEF (boolean, _bfd_get_section_contents, (bfd *, sec_ptr, PTR, 
                                            file_ptr, bfd_size_type));
  SDEF (boolean, _new_section_hook, (bfd *, sec_ptr));

    /* Symbols and reloctions
    */
 SDEF (unsigned int, _get_symtab_upper_bound, (bfd *));
  SDEF (unsigned int, _bfd_canonicalize_symtab,
           (bfd *, struct symbol_cache_entry **));
  SDEF (unsigned int, _get_reloc_upper_bound, (bfd *, sec_ptr));
  SDEF (unsigned int, _bfd_canonicalize_reloc, (bfd *, sec_ptr, arelent **,
                                               struct symbol_cache_entry**));
  SDEF (struct symbol_cache_entry  *, _bfd_make_empty_symbol, (bfd *));
  SDEF (void,     _bfd_print_symbol, (bfd *, PTR, struct symbol_cache_entry  *,
                                      bfd_print_symbol_enum_type));
#define bfd_print_symbol(b,p,s,e) BFD_SEND(b, _bfd_print_symbol, (b,p,s,e))
  SDEF (alent *,   _get_lineno, (bfd *, struct symbol_cache_entry  *));

  SDEF (boolean,   _bfd_set_arch_mach, (bfd *, enum bfd_architecture,
                                       unsigned long));

  SDEF (bfd *,  openr_next_archived_file, (bfd *arch, bfd *prev));
  SDEF (boolean, _bfd_find_nearest_line,
        (bfd *abfd, struct sec  *section,
         struct symbol_cache_entry  **symbols,bfd_vma offset,
        CONST char **file, CONST char **func, unsigned int *line));
  SDEF (int,    _bfd_stat_arch_elt, (bfd *, struct stat *));

  SDEF (int,    _bfd_sizeof_headers, (bfd *, boolean));

  SDEF (void, _bfd_debug_info_start, (bfd *));
  SDEF (void, _bfd_debug_info_end, (bfd *));
  SDEF (void, _bfd_debug_info_accumulate, (bfd *, struct sec  *));

    /* Special entry points for gdb to swap in coff symbol table parts
    */
  SDEF(void, _bfd_coff_swap_aux_in,(
       bfd            *abfd ,
       PTR             ext,
       int             type,
       int             class ,
       PTR             in));

  SDEF(void, _bfd_coff_swap_sym_in,(
       bfd            *abfd ,
       PTR             ext,
       PTR             in));

  SDEF(void, _bfd_coff_swap_lineno_in,  (
       bfd            *abfd,
       PTR            ext,
       PTR             in));

} bfd_target;

/* Returns a pointer to the transfer vector for the object target
named target_name.  If target_name is NULL, chooses the one in the
environment variable GNUTARGET; if that is null or not defined then
the first entry in the target list is chosen.  Passing in the
string "default" or setting the environment variable to "default"
will cause the first entry in the target list to be returned,
and "target_defaulted" will be set in the bfd.  This causes
bfd_check_format to loop over all the targets to find the one
that matches the file being read.  
*/
PROTO(bfd_target *, bfd_find_target,(CONST char *, bfd *));
/* This function returns a freshly malloced NULL-terminated vector of the
names of all the valid bfd targets. Do not modify the names 
*/
PROTO(CONST char **,bfd_target_list,());
/* This routine is supplied a bfd and a format. It attempts to verify if
the file attatched to the bfd is indeed compatible with the format
specified (ie, one of @code{bfd_object}, @code{bfd_archive} or
@code{bfd_core}).

If the bfd has been set to a specific @var{target} before the call,
only the named target and format combination will be checked. If the
target has not been set, or has been set to @code{default} then all
the known target backends will be interrogated to determine a match.

The function returns @code{true} on success, otherwise @code{false}
with one of the following error codes: 
@table @code
@item 
invalid_operation
if @code{format} is not one of @code{bfd_object}, @code{bfd_archive}
or @code{bfd_core}.
@item system_call_error
if an error occured during a read -  even some file mismatches can
cause system_call_errros
@item file_not_recognised
none of the backends recognised the file format
@item file_ambiguously_recognized
more than one backend recognised the file format.
@end table
*/
PROTO(boolean, bfd_check_format, (bfd *abfd, bfd_format format));
/* This function sets the file format of the supplied bfd to the format
requested. If the target set in the bfd does not support the format
requested, the format is illegal or the bfd is not open for writing
than an error occurs.
*/
PROTO(boolean,bfd_set_format,(bfd *, bfd_format));
/* This function takes one argument, and enumerated type (bfd_format) and
returns a pointer to a const string "invalid", "object", "archive",
"core" or "unknown" depending upon the value of the enumeration.
*/
PROTO(CONST char *, bfd_format_string, (bfd_format));
#endif

