/* Main header file for the bfd library -- portable access to object files.
   ==> The bfd.h file is generated from bfd-in.h and various .c files; if you
   ==> change it, your changes will probably be lost.
   Copyright 1990, 1991, 1992, 1993 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

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

The bfd.h file is generated from bfd-in.h and various .c files; if you
change it, your changes will probably be lost.

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

#define BFD_VERSION "2.2"

#define BFD_ARCH_SIZE @WORDSIZE@

#if BFD_ARCH_SIZE >= 64
#define BFD64
#endif

/* 64-bit type definition (if any) from bfd's sysdep.h goes here */


/* forward declaration */
typedef struct _bfd bfd;

/* To squelch erroneous compiler warnings ("illegal pointer
   combination") from the SVR3 compiler, we would like to typedef
   boolean to int (it doesn't like functions which return boolean.
   Making sure they are never implicitly declared to return int
   doesn't seem to help).  But this file is not configured based on
   the host.  */
/* General rules: functions which are boolean return true on success
   and false on failure (unless they're a predicate).   -- bfd.doc */
/* I'm sure this is going to break something and someone is going to
   force me to change it. */
/* typedef enum boolean {false, true} boolean; */
/* Yup, SVR4 has a "typedef enum boolean" in <sys/types.h>  -fnf */
typedef enum bfd_boolean {false, true} boolean;

/* A pointer to a position in a file.  */
/* FIXME:  This should be using off_t from <sys/types.h>.
   For now, try to avoid breaking stuff by not including <sys/types.h> here.
   This will break on systems with 64-bit file offsets (e.g. 4.4BSD).
   Probably the best long-term answer is to avoid using file_ptr AND off_t 
   in this header file, and to handle this in the BFD implementation
   rather than in its interface.  */
/* typedef off_t	file_ptr; */
typedef long int file_ptr;

/* Support for different sizes of target format ints and addresses.  If the
   host implements 64-bit values, it defines HOST_64_BIT to be the appropriate
   type.  Otherwise, this code will fall back on gcc's "long long" type if gcc
   is being used.  HOST_64_BIT must be defined in such a way as to be a valid
   type name by itself or with "unsigned" prefixed.  It should be a signed
   type by itself.

   If neither is the case, then compilation will fail if 64-bit targets are
   requested.  If you don't request any 64-bit targets, you should be safe. */

#ifdef	BFD64

#if defined (__GNUC__) && !defined (HOST_64_BIT)
#define HOST_64_BIT long long
typedef HOST_64_BIT int64_type;
typedef unsigned HOST_64_BIT uint64_type;
#endif

typedef unsigned HOST_64_BIT bfd_vma;
typedef HOST_64_BIT bfd_signed_vma;
typedef unsigned HOST_64_BIT bfd_size_type;
typedef unsigned HOST_64_BIT symvalue;
#define fprintf_vma(s,x) \
		fprintf(s,"%08x%08x", uint64_typeHIGH(x), uint64_typeLOW(x))
#else /* not BFD64  */

/* Represent a target address.  Also used as a generic unsigned type
   which is guaranteed to be big enough to hold any arithmetic types
   we need to deal with.  */
typedef unsigned long bfd_vma;

/* A generic signed type which is guaranteed to be big enough to hold any
   arithmetic types we need to deal with.  Can be assumed to be compatible
   with bfd_vma in the same way that signed and unsigned ints are compatible
   (as parameters, in assignment, etc).  */
typedef long bfd_signed_vma;

typedef unsigned long symvalue;
typedef unsigned long bfd_size_type;

/* Print a bfd_vma x on stream s.  */
#define fprintf_vma(s,x) fprintf(s, "%08lx", x)
#endif /* not BFD64  */
#define printf_vma(x) fprintf_vma(stdout,x)

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
#define NO_FLAGS    	0x00
#define HAS_RELOC   	0x01
#define EXEC_P      	0x02
#define HAS_LINENO  	0x04
#define HAS_DEBUG   	0x08
#define HAS_SYMS    	0x10
#define HAS_LOCALS  	0x20
#define DYNAMIC     	0x40
#define WP_TEXT     	0x80
#define D_PAGED     	0x100
#define BFD_IS_RELAXABLE 0x200

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
#define bfd_asymbol_base(x) ((x)->section->vma)
#define bfd_asymbol_value(x) (bfd_asymbol_base(x) + (x)->value)
#define bfd_asymbol_name(x) ((x)->name)
/*Perhaps future: #define bfd_asymbol_bfd(x) ((x)->section->owner)*/
#define bfd_asymbol_bfd(x) ((x)->the_bfd)
#define bfd_asymbol_flavour(x) (bfd_asymbol_bfd(x)->xvec->flavour)

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

#define bfd_get_section_name(bfd, ptr) ((ptr)->name + 0)
#define bfd_get_section_vma(bfd, ptr) ((ptr)->vma + 0)
#define bfd_get_section_alignment(bfd, ptr) ((ptr)->alignment_power + 0)
#define bfd_section_name(bfd, ptr) ((ptr)->name)
#define bfd_section_size(bfd, ptr) (bfd_get_section_size_before_reloc(ptr))
#define bfd_section_vma(bfd, ptr) ((ptr)->vma)
#define bfd_section_alignment(bfd, ptr) ((ptr)->alignment_power)
#define bfd_get_section_flags(bfd, ptr) ((ptr)->flags + 0)
#define bfd_get_section_userdata(bfd, ptr) ((ptr)->userdata)

#define bfd_is_com_section(ptr) (((ptr)->flags & SEC_IS_COMMON) != 0)

#define bfd_set_section_vma(bfd, ptr, val) (((ptr)->vma = (ptr)->lma= (val)), ((ptr)->user_set_vma = true), true)
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
	      no_debug_section, bad_value,

	      /* An input file is shorter than expected.  */
	      file_truncated,
	      
	      invalid_error_code} bfd_ec;

extern bfd_ec bfd_error;
struct reloc_cache_entry;
struct bfd_seclet;


typedef struct bfd_error_vector {
  void (* nonrepresentable_section ) PARAMS ((CONST bfd  *CONST abfd,
					      CONST char *CONST name));
  void (* undefined_symbol) PARAMS ((CONST struct reloc_cache_entry *rel,
				     CONST struct bfd_seclet *sec));
  void (* reloc_value_truncated) PARAMS ((CONST struct
					  reloc_cache_entry *rel,
					  struct bfd_seclet *sec));

  void (* reloc_dangerous) PARAMS ((CONST struct reloc_cache_entry *rel,
				    CONST struct bfd_seclet *sec));
  
} bfd_error_vector_type;

CONST char *bfd_errmsg PARAMS ((bfd_ec error_tag));
void bfd_perror PARAMS ((CONST char *message));


typedef enum bfd_print_symbol
{ 
  bfd_print_symbol_name,
  bfd_print_symbol_more,
  bfd_print_symbol_all
} bfd_print_symbol_type;
    

/* Information about a symbol that nm needs.  */

typedef struct _symbol_info
{
  symvalue value;
  char type;                   /*  */
  CONST char *name;            /* Symbol name.  */
  char stab_other;             /* Unused. */
  short stab_desc;             /* Info for N_TYPE.  */
  CONST char *stab_name;
} symbol_info;

/* The code that implements targets can initialize a jump table with this
   macro.  It must name all its routines the same way (a prefix plus
   the standard routine suffix), or it must #define the routines that
   are not so named, before calling JUMP_TABLE in the initializer.  */

/* Semi-portable string concatenation in cpp */
#ifndef CAT
#ifdef __STDC__
#define CAT(a,b) a##b
#define CAT3(a,b,c) a##b##c
#else
#define CAT(a,b) a/**/b
#define CAT3(a,b,c) a/**/b/**/c
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
CAT(NAME,_get_symbol_info),\
CAT(NAME,_get_lineno),\
CAT(NAME,_set_arch_mach),\
CAT(NAME,_openr_next_archived_file),\
CAT(NAME,_find_nearest_line),\
CAT(NAME,_generic_stat_arch_elt),\
CAT(NAME,_sizeof_headers),\
CAT(NAME,_bfd_debug_info_start),\
CAT(NAME,_bfd_debug_info_end),\
CAT(NAME,_bfd_debug_info_accumulate),\
CAT(NAME,_bfd_get_relocated_section_contents),\
CAT(NAME,_bfd_relax_section),\
CAT(NAME,_bfd_seclet_link),\
CAT(NAME,_bfd_reloc_type_lookup),\
CAT(NAME,_bfd_make_debug_symbol)

#define COFF_SWAP_TABLE (PTR) &bfd_coff_std_swap_table


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
#define bfd_my_archive(abfd) ((abfd)->my_archive)
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

#define bfd_get_symbol_leading_char(abfd) ((abfd)->xvec->symbol_leading_char)

#define BYTE_SIZE 1
#define SHORT_SIZE 2
#define LONG_SIZE 4

/* And more from the source.  */
