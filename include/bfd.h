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

#define BFD_VERSION "2.1"

/* forward declaration */
typedef struct _bfd bfd;

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
#else
typedef struct {int a,b;} bfd_64_type;
typedef unsigned long rawdata_offset;
typedef unsigned long bfd_vma;
typedef unsigned long bfd_offset;
typedef unsigned long bfd_word;
typedef unsigned long bfd_size;
typedef unsigned long symvalue;
typedef unsigned long bfd_size_type;
#define fprintf_vma(s,x) fprintf(s, "%08lx", x)
#endif
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

#define bfd_set_section_vma(bfd, ptr, val) (((ptr)->vma = (val)), ((ptr)->user_set_vma = true), true)
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
  bfd_print_symbol_all,
  bfd_print_symbol_nm	/* Pretty format suitable for nm program. */
} bfd_print_symbol_type;
    


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
CAT(NAME,_bfd_debug_info_accumulate),\
CAT(NAME,_bfd_get_relocated_section_contents),\
CAT(NAME,_bfd_relax_section),\
CAT(NAME,_bfd_seclet_link)

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
void 
bfd_init PARAMS ((void));

bfd *
bfd_openr PARAMS ((CONST char *filename, CONST char*target));

bfd *
bfd_fdopenr PARAMS ((CONST char *filename, CONST char *target, int fd));

bfd *
bfd_openw PARAMS ((CONST char *filename, CONST char *target));

boolean 
bfd_close PARAMS ((bfd *));

boolean 
bfd_close_all_done PARAMS ((bfd *));

bfd_size_type 
bfd_alloc_size PARAMS ((bfd *abfd));

bfd *
bfd_create PARAMS ((CONST char *filename, bfd *template));

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
typedef struct sec 
{
         /* The name of the section, the name isn't a copy, the pointer is
        the same as that passed to bfd_make_section. */

    CONST char *name;


         /* Which section is it 0.nth      */

   int index;                      

         /* The next section in the list belonging to the BFD, or NULL. */

    struct sec *next;

         /* The field flags contains attributes of the section. Some of
           flags are read in from the object file, and some are
           synthesized from other information.  */         

    flagword flags;

#define SEC_NO_FLAGS   0x000

         /* Tells the OS to allocate space for this section when loaded.
           This would clear for a section containing debug information
           only. */
          

#define SEC_ALLOC      0x001
         /* Tells the OS to load the section from the file when loading.
           This would be clear for a .bss section */

#define SEC_LOAD       0x002
         /* The section contains data still to be relocated, so there will
           be some relocation information too. */

#define SEC_RELOC      0x004

         /* Obsolete ? */

#define SEC_BALIGN     0x008

         /* A signal to the OS that the section contains read only
          data. */
#define SEC_READONLY   0x010

         /* The section contains code only. */

#define SEC_CODE       0x020

         /* The section contains data only. */

#define SEC_DATA        0x040

         /* The section will reside in ROM. */

#define SEC_ROM        0x080

         /* The section contains constructor information. This section
           type is used by the linker to create lists of constructors and
           destructors used by <<g++>>. When a back end sees a symbol
           which should be used in a constructor list, it creates a new
           section for the type of name (eg <<__CTOR_LIST__>>), attaches
           the symbol to it and builds a relocation. To build the lists
           of constructors, all the linker has to to is catenate all the
           sections called <<__CTOR_LIST__>> and relocte the data
           contained within - exactly the operations it would peform on
           standard data. */

#define SEC_CONSTRUCTOR 0x100

         /* The section is a constuctor, and should be placed at the
          end of the . */

#define SEC_CONSTRUCTOR_TEXT 0x1100

#define SEC_CONSTRUCTOR_DATA 0x2100

#define SEC_CONSTRUCTOR_BSS  0x3100

         /* The section has contents - a data section could be
           <<SEC_ALLOC>> | <<SEC_HAS_CONTENTS>>, a debug section could be
           <<SEC_HAS_CONTENTS>> */

#define SEC_HAS_CONTENTS 0x200

         /* An instruction to the linker not to output sections
          containing this flag even if they have information which
          would normally be written. */

#define SEC_NEVER_LOAD 0x400

         /* The section is a shared library section.  The linker must leave
           these completely alone, as the vma and size are used when
           the executable is loaded. */

#define SEC_SHARED_LIBRARY 0x800

         /* The section is a common section (symbols may be defined
           multiple times, the value of a symbol is the amount of
           space it requires, and the largest symbol value is the one
           used).  Most targets have exactly one of these (.bss), but
           ECOFF has two. */

#define SEC_IS_COMMON 0x8000
       
   bfd_vma vma;
   boolean user_set_vma;

         /* The size of the section in bytes, as it will be output.
           contains a value even if the section has no contents (eg, the
           size of <<.bss>>). This will be filled in after relocation */

   bfd_size_type _cooked_size;    

         /* The size on disk of the section in bytes originally.  Normally this
	    value is the same as the size, but if some relaxing has
	    been done, then this value will be bigger.  */

   bfd_size_type _raw_size;    

         /* If this section is going to be output, then this value is the
           offset into the output section of the first byte in the input
           section. Eg, if this was going to start at the 100th byte in
           the output section, this value would be 100. */

   bfd_vma output_offset;

         /* The output section through which to map on output. */

   struct sec *output_section;

         /* The alignment requirement of the section, as an exponent - eg
           3 aligns to 2^3 (or 8) */

   unsigned int alignment_power;

         /* If an input section, a pointer to a vector of relocation
           records for the data in this section. */

   struct reloc_cache_entry *relocation;

         /* If an output section, a pointer to a vector of pointers to
           relocation records for the data in this section. */

   struct reloc_cache_entry **orelocation;

         /* The number of relocation records in one of the above  */

   unsigned reloc_count;

         /* Information below is back end specific - and not always used
           or updated 

           File position of section data    */

   file_ptr filepos;      
        
         /* File position of relocation info */

   file_ptr rel_filepos;

         /* File position of line data       */

   file_ptr line_filepos;

         /* Pointer to data for applications */

   PTR userdata;

   struct lang_output_section *otheruserdata;

         /* Attached line number information */

   alent *lineno;
        
         /* Number of line number records   */

   unsigned int lineno_count;

         /* When a section is being output, this value changes as more
           linenumbers are written out */

   file_ptr moving_line_filepos;

         /* what the section number is in the target world  */

   int target_index;

   PTR used_by_bfd;

         /* If this is a constructor section then here is a list of the
           relocations created to relocate items within it. */

   struct relent_chain *constructor_chain;

         /* The BFD which owns the section. */

   bfd *owner;

   boolean reloc_done;
	  /* A symbol which points at this section only */
   struct symbol_cache_entry *symbol;  
   struct symbol_cache_entry **symbol_ptr_ptr;
   struct bfd_seclet *seclets_head;
   struct bfd_seclet *seclets_tail;
} asection ;


#define BFD_ABS_SECTION_NAME "*ABS*"
#define BFD_UND_SECTION_NAME "*UND*"
#define BFD_COM_SECTION_NAME "*COM*"

     /* the absolute section */
 extern   asection bfd_abs_section;
     /* Pointer to the undefined section */
 extern   asection bfd_und_section;
     /* Pointer to the common section */
 extern asection bfd_com_section;

 extern struct symbol_cache_entry *bfd_abs_symbol;
 extern struct symbol_cache_entry *bfd_com_symbol;
 extern struct symbol_cache_entry *bfd_und_symbol;
#define bfd_get_section_size_before_reloc(section) \
     (section->reloc_done ? (abort(),1): (section)->_raw_size)
#define bfd_get_section_size_after_reloc(section) \
     ((section->reloc_done) ? (section)->_cooked_size: (abort(),1))
asection *
bfd_get_section_by_name PARAMS ((bfd *abfd, CONST char *name));

asection *
bfd_make_section_old_way PARAMS ((bfd *, CONST char *name));

asection * 
bfd_make_section PARAMS ((bfd *, CONST char *name));

boolean 
bfd_set_section_flags PARAMS ((bfd *, asection *, flagword));

void 
bfd_map_over_sections PARAMS ((bfd *abfd,
    void (*func)(bfd *abfd,
    asection *sect,
    PTR obj),
    PTR obj));

boolean 
bfd_set_section_size PARAMS ((bfd *, asection *, bfd_size_type val));

boolean 
bfd_set_section_contents
 PARAMS ((bfd *abfd,        
    asection *section,
    PTR data,
    file_ptr offset,
    bfd_size_type count));

boolean 
bfd_get_section_contents 
 PARAMS ((bfd *abfd, asection *section, PTR location,
    file_ptr offset, bfd_size_type count));

enum bfd_architecture 
{
  bfd_arch_unknown,    /* File arch not known */
  bfd_arch_obscure,    /* Arch known, not one of these */
  bfd_arch_m68k,       /* Motorola 68xxx */
  bfd_arch_vax,        /* DEC Vax */   
  bfd_arch_i960,       /* Intel 960 */
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

  bfd_arch_a29k,       /* AMD 29000 */
  bfd_arch_sparc,      /* SPARC */
  bfd_arch_mips,       /* MIPS Rxxxx */
  bfd_arch_i386,       /* Intel 386 */
  bfd_arch_we32k,      /* AT&T WE32xxx */
  bfd_arch_tahoe,      /* CCI/Harris Tahoe */
  bfd_arch_i860,       /* Intel 860 */
  bfd_arch_romp,       /* IBM ROMP PC/RT */
  bfd_arch_alliant,    /* Alliant */
  bfd_arch_convex,     /* Convex */
  bfd_arch_m88k,       /* Motorola 88xxx */
  bfd_arch_pyramid,    /* Pyramid Technology */
  bfd_arch_h8300,      /* Hitachi H8/300 */
  bfd_arch_rs6000,     /* IBM RS/6000 */
  bfd_arch_hppa,       /* HP PA RISC */
  bfd_arch_z8k,        /* Zilog Z8000 */
#define bfd_mach_z8001		1
#define bfd_mach_z8002		2
  bfd_arch_h8500,      /* Hitachi H8/500 */
  bfd_arch_last
  };

typedef struct bfd_arch_info 
{
  int bits_per_word;
  int bits_per_address;
  int bits_per_byte;
  enum bfd_architecture arch;
  long mach;
  char *arch_name;
  CONST  char *printable_name;
  unsigned int section_align_power;
  /* true if this is the default machine for the architecture */
  boolean the_default;	
  CONST struct bfd_arch_info * (*compatible)
	PARAMS ((CONST struct bfd_arch_info *a,
	         CONST struct bfd_arch_info *b));

  boolean (*scan) PARAMS ((CONST struct bfd_arch_info *, CONST char *));
   /* How to disassemble an instruction, producing a printable
     representation on a specified stdio stream.  This isn't
     defined for most processors at present, because of the size
     of the additional tables it would drag in, and because gdb
     wants to use a different interface.  */
  unsigned int (*disassemble) PARAMS ((bfd_vma addr, CONST char *data,
				        PTR stream));

  struct bfd_arch_info *next;
} bfd_arch_info_type;
CONST char *
bfd_printable_name PARAMS ((bfd *abfd));

bfd_arch_info_type *
bfd_scan_arch PARAMS ((CONST char *));

CONST bfd_arch_info_type *
bfd_arch_get_compatible PARAMS ((
    CONST bfd *abfd,
    CONST bfd *bbfd));

void 
bfd_set_arch_info PARAMS ((bfd *, bfd_arch_info_type *));

enum bfd_architecture 
bfd_get_arch PARAMS ((bfd *abfd));

unsigned long 
bfd_get_mach PARAMS ((bfd *abfd));

unsigned int 
bfd_arch_bits_per_byte PARAMS ((bfd *abfd));

unsigned int 
bfd_arch_bits_per_address PARAMS ((bfd *abfd));

bfd_arch_info_type * 
bfd_get_arch_info PARAMS ((bfd *));

bfd_arch_info_type *
bfd_lookup_arch
 PARAMS ((enum bfd_architecture
    arch,
    long machine));

CONST char * 
bfd_printable_arch_mach
 PARAMS ((enum bfd_architecture arch, unsigned long machine));

typedef enum bfd_reloc_status 
{
        /* No errors detected */
  bfd_reloc_ok,

        /* The relocation was performed, but there was an overflow. */
  bfd_reloc_overflow,

        /* The address to relocate was not within the section supplied*/
  bfd_reloc_outofrange,

        /* Used by special functions */
  bfd_reloc_continue,

        /* Unused */
  bfd_reloc_notsupported,

        /* Unsupported relocation size requested.  */
  bfd_reloc_other,

        /* The symbol to relocate against was undefined.*/
  bfd_reloc_undefined,

        /* The relocation was performed, but may not be ok - presently
          generated only when linking i960 coff files with i960 b.out
          symbols. */
  bfd_reloc_dangerous
 }
 bfd_reloc_status_type;


typedef struct reloc_cache_entry 
{
        /* A pointer into the canonical table of pointers  */
  struct symbol_cache_entry **sym_ptr_ptr;

        /* offset in section */
  rawdata_offset address;

        /* addend for relocation value */
  bfd_vma addend;    

        /* Pointer to how to perform the required relocation */
  CONST struct reloc_howto_struct *howto;

} arelent;

typedef CONST struct reloc_howto_struct 
{ 
        /*  The type field has mainly a documetary use - the back end can
           to what it wants with it, though the normally the back end's
           external idea of what a reloc number would be would be stored
           in this field. For example, the a PC relative word relocation
           in a coff environment would have the type 023 - because that's
           what the outside world calls a R_PCRWORD reloc. */
  unsigned int type;

        /*  The value the final relocation is shifted right by. This drops
           unwanted data from the relocation.  */
  unsigned int rightshift;

        /*  The size of the item to be relocated - 0, is one byte, 1 is 2
           bytes, 2 is four bytes.  A negative value indicates that the
	    result is to be subtracted from the data.  */
  int size;

        /*  Now obsolete?  But m68k-coff still uses it... */
  unsigned int bitsize;

        /*  Notes that the relocation is relative to the location in the
           data section of the addend. The relocation function will
           subtract from the relocation value the address of the location
           being relocated. */
  boolean pc_relative;

  unsigned int bitpos;

        /*  Now obsolete */
  boolean absolute;

        /* Causes the relocation routine to return an error if overflow
          is detected when relocating. */
  boolean complain_on_overflow;

        /* If this field is non null, then the supplied function is
          called rather than the normal function. This allows really
          strange relocation methods to be accomodated (e.g., i960 callj
          instructions). */
  bfd_reloc_status_type (*special_function) 
				    PARAMS ((bfd *abfd,
					     arelent *reloc_entry,
                                            struct symbol_cache_entry *symbol,
                                            PTR data,
                                            asection *input_section, 
                                            bfd *output_bfd));

        /* The textual name of the relocation type. */
  char *name;

        /* When performing a partial link, some formats must modify the
          relocations rather than the data - this flag signals this.*/
  boolean partial_inplace;

        /* The src_mask is used to select what parts of the read in data
          are to be used in the relocation sum.  E.g., if this was an 8 bit
          bit of data which we read and relocated, this would be
          0x000000ff. When we have relocs which have an addend, such as
          sun4 extended relocs, the value in the offset part of a
          relocating field is garbage so we never use it. In this case
          the mask would be 0x00000000. */
  bfd_word src_mask;

        /* The dst_mask is what parts of the instruction are replaced
          into the instruction. In most cases src_mask == dst_mask,
          except in the above special case, where dst_mask would be
          0x000000ff, and src_mask would be 0x00000000.   */
  bfd_word dst_mask;           

        /* When some formats create PC relative instructions, they leave
          the value of the pc of the place being relocated in the offset
          slot of the instruction, so that a PC relative relocation can
          be made just by adding in an ordinary offset (e.g., sun3 a.out).
          Some formats leave the displacement part of an instruction
          empty (e.g., m88k bcs), this flag signals the fact.*/
  boolean pcrel_offset;

} reloc_howto_type;
#define HOWTO(C, R,S,B, P, BI, ABS, O, SF, NAME, INPLACE, MASKSRC, MASKDST, PC) \
  {(unsigned)C,R,S,B, P, BI, ABS,O,SF,NAME,INPLACE,MASKSRC,MASKDST,PC}
#define NEWHOWTO( FUNCTION, NAME,SIZE,REL,IN) HOWTO(0,0,SIZE,0,REL,0,false,false,FUNCTION, NAME,false,0,0,IN)

#define HOWTO_PREPARE(relocation, symbol)      \
  {                                            \
  if (symbol != (asymbol *)NULL) {             \
    if (bfd_is_com_section (symbol->section)) { \
      relocation = 0;                          \
    }                                          \
    else {                                     \
      relocation = symbol->value;              \
    }                                          \
  }                                            \
}                      
typedef unsigned char bfd_byte;

typedef struct relent_chain {
  arelent relent;
  struct   relent_chain *next;
} arelent_chain;
bfd_reloc_status_type

bfd_perform_relocation
 PARAMS ((bfd * abfd,
    arelent *reloc_entry,
    PTR data,
    asection *input_section,
    bfd *output_bfd));

typedef enum bfd_reloc_code_real 

{
        /* 16 bits wide, simple reloc */
  BFD_RELOC_16,        

        /* 8 bits wide, but used to form an address like 0xffnn */
  BFD_RELOC_8_FFnn,

        /* 8 bits wide, simple */
  BFD_RELOC_8,

        /* 8 bits wide, pc relative */
  BFD_RELOC_8_PCREL,

        /* The type of reloc used to build a contructor table - at the
          moment probably a 32 bit wide abs address, but the cpu can
          choose. */

  BFD_RELOC_CTOR,

        /* 32 bits wide, simple reloc */
  BFD_RELOC_32,
	 /* 32 bits, PC-relative */
  BFD_RELOC_32_PCREL,

	 /* High 22 bits of 32-bit value; simple reloc.  */
  BFD_RELOC_HI22,
	 /* Low 10 bits.  */
  BFD_RELOC_LO10,

	 /* Reloc types used for i960/b.out.  */
  BFD_RELOC_24_PCREL,
  BFD_RELOC_I960_CALLJ,

  BFD_RELOC_16_PCREL,
	 /* 32-bit pc-relative, shifted right 2 bits (i.e., 30-bit
	   word displacement, e.g. for SPARC) */
  BFD_RELOC_32_PCREL_S2,

   /* now for the sparc/elf codes */
  BFD_RELOC_NONE,		 /* actually used */
  BFD_RELOC_SPARC_WDISP22,
  BFD_RELOC_SPARC22,
  BFD_RELOC_SPARC13,
  BFD_RELOC_SPARC_BASE13,
  BFD_RELOC_SPARC_GOT10,
  BFD_RELOC_SPARC_GOT13,
  BFD_RELOC_SPARC_GOT22,
  BFD_RELOC_SPARC_PC10,
  BFD_RELOC_SPARC_PC22,
  BFD_RELOC_SPARC_WPLT30,
  BFD_RELOC_SPARC_COPY,
  BFD_RELOC_SPARC_GLOB_DAT,
  BFD_RELOC_SPARC_JMP_SLOT,
  BFD_RELOC_SPARC_RELATIVE,
  BFD_RELOC_SPARC_UA32,

   /* this one is a.out specific? */
  BFD_RELOC_SPARC_BASE22,

        /* Bits 27..2 of the relocation address shifted right 2 bits;
         simple reloc otherwise.  */
  BFD_RELOC_MIPS_JMP,

        /* signed 16-bit pc-relative, shifted right 2 bits (e.g. for MIPS) */
  BFD_RELOC_16_PCREL_S2,

        /* High 16 bits of 32-bit value; simple reloc.  */
  BFD_RELOC_HI16,
        /* High 16 bits of 32-bit value but the low 16 bits will be sign
          extended and added to form the final result.  If the low 16
          bits form a negative number, we need to add one to the high value
          to compensate for the borrow when the low bits are added.  */
  BFD_RELOC_HI16_S,
        /* Low 16 bits.  */
  BFD_RELOC_LO16,

   /* this must be the highest numeric value */
  BFD_RELOC_UNUSED
 } bfd_reloc_code_real_type;
CONST struct reloc_howto_struct *

bfd_reloc_type_lookup  PARAMS ((bfd *abfd, bfd_reloc_code_real_type code));

typedef struct symbol_cache_entry 
{
	 /* A pointer to the BFD which owns the symbol. This information
	   is necessary so that a back end can work out what additional
   	   information (invisible to the application writer) is carried
	   with the symbol.

	   This field is *almost* redundant, since you can use section->owner
	   instead, except that some symbols point to the global sections
	   bfd_{abs,com,und}_section.  This could be fixed by making
	   these globals be per-bfd (or per-target-flavor).  FIXME. */

  struct _bfd *the_bfd;  /* Use bfd_asymbol_bfd(sym) to access this field. */

	 /* The text of the symbol. The name is left alone, and not copied - the
	   application may not alter it. */
  CONST char *name;

	 /* The value of the symbol.*/
  symvalue value;

	 /* Attributes of a symbol: */

#define BSF_NO_FLAGS    0x00

	 /* The symbol has local scope; <<static>> in <<C>>. The value
 	   is the offset into the section of the data. */
#define BSF_LOCAL	0x01

	 /* The symbol has global scope; initialized data in <<C>>. The
	   value is the offset into the section of the data. */
#define BSF_GLOBAL	0x02

	 /* Obsolete; should be deleted? */
#define BSF_IMPORT	0x04

	 /* The symbol has global scope, and is exported. The value is
	   the offset into the section of the data. */
#define BSF_EXPORT	0x08

	 /* The symbol is undefined. <<extern>> in <<C>>. The value has
	   no meaning.  Obsolete; should be deleted? */
#define BSF_UNDEFINED_OBS 0x10	

	 /* The symbol is common, initialized to zero; default in
	   <<C>>. The value is the size of the object in bytes. */
#define BSF_FORT_COMM_OBS	0x20	

	 /* A normal C symbol would be one of:
	   <<BSF_LOCAL>>, <<BSF_FORT_COMM>>,  <<BSF_UNDEFINED>> or
	   <<BSF_EXPORT|BSD_GLOBAL>> */

	 /* The symbol is a debugging record. The value has an arbitary
	   meaning. */
#define BSF_DEBUGGING	0x40

	 /* Used by the linker. */
#define BSF_KEEP        0x10000
#define BSF_KEEP_G      0x80000

	 /* Unused; should be deleted? */
#define BSF_WEAK        0x100000
#define BSF_CTOR        0x200000 

        /* This symbol was created to point to a section, e.g. ELF's
	   STT_SECTION symbols.  */
#define BSF_SECTION_SYM 0x400000 

	 /* The symbol used to be a common symbol, but now it is
	   allocated. */
#define BSF_OLD_COMMON  0x800000  

	 /* The default value for common data. */
#define BFD_FORT_COMM_DEFAULT_VALUE 0

	 /* In some files the type of a symbol sometimes alters its
	   location in an output file - ie in coff a <<ISFCN>> symbol
	   which is also <<C_EXT>> symbol appears where it was
	   declared and not at the end of a section.  This bit is set
  	   by the target BFD part to convey this information. */

#define BSF_NOT_AT_END    0x40000

	 /* Signal that the symbol is the label of constructor section. */
#define BSF_CONSTRUCTOR   0x1000000

	 /* Signal that the symbol is a warning symbol. If the symbol
	   is a warning symbol, then the value field (I know this is
	   tacky) will point to the asymbol which when referenced will
	   cause the warning. */
#define BSF_WARNING       0x2000000

	 /* Signal that the symbol is indirect. The value of the symbol
	   is a pointer to an undefined asymbol which contains the
	   name to use instead. */
#define BSF_INDIRECT      0x4000000

	 /* BSF_FILE marks symbols that contain a file name.  This is used
	   for ELF STT_FILE symbols.  */
#define BSF_FILE          0x08000000

  flagword flags;

	 /* A pointer to the section to which this symbol is 
	   relative.  This will always be non NULL, there are special
          sections for undefined and absolute symbols */
  struct sec *section;

	 /* Back end special data. This is being phased out in favour
	   of making this a union. */
  PTR udata;

} asymbol;
#define get_symtab_upper_bound(abfd) \
     BFD_SEND (abfd, _get_symtab_upper_bound, (abfd))
#define bfd_canonicalize_symtab(abfd, location) \
     BFD_SEND (abfd, _bfd_canonicalize_symtab,\
                  (abfd, location))
boolean 
bfd_set_symtab  PARAMS ((bfd *, asymbol **, unsigned int ));

void 
bfd_print_symbol_vandf PARAMS ((PTR file, asymbol *symbol));

#define bfd_make_empty_symbol(abfd) \
     BFD_SEND (abfd, _bfd_make_empty_symbol, (abfd))
#define bfd_make_debug_symbol(abfd,ptr,size) \
        BFD_SEND (abfd, _bfd_make_debug_symbol, (abfd, ptr, size))
int 
bfd_decode_symclass PARAMS ((asymbol *symbol));

struct _bfd 
{
     /* The filename the application opened the BFD with.  */
    CONST char *filename;                

     /* A pointer to the target jump table.             */
    struct bfd_target *xvec;

     /* To avoid dragging too many header files into every file that
       includes `<<bfd.h>>', IOSTREAM has been declared as a "char
       *", and MTIME as a "long".  Their correct types, to which they
       are cast when used, are "FILE *" and "time_t".    The iostream
       is the result of an fopen on the filename. */
    char *iostream;

     /* Is the file being cached */

    boolean cacheable;

     /* Marks whether there was a default target specified when the
       BFD was opened. This is used to select what matching algorithm
       to use to chose the back end. */

    boolean target_defaulted;

     /* The caching routines use these to maintain a
       least-recently-used list of BFDs */

    struct _bfd *lru_prev, *lru_next;

     /* When a file is closed by the caching routines, BFD retains
       state information on the file here: 
     */

    file_ptr where;              

     /* and here:*/

    boolean opened_once;

     /* Set if we have a locally maintained mtime value, rather than
       getting it from the file each time: */

    boolean mtime_set;

     /* File modified time, if mtime_set is true: */

    long mtime;          

     /* Reserved for an unimplemented file locking extension.*/

    int ifd;

     /* The format which belongs to the BFD.*/

    bfd_format format;

     /* The direction the BFD was opened with*/

    enum bfd_direction {no_direction = 0,
                        read_direction = 1,
                        write_direction = 2,
                        both_direction = 3} direction;

     /* Format_specific flags*/

    flagword flags;              

     /* Currently my_archive is tested before adding origin to
       anything. I believe that this can become always an add of
       origin, with origin set to 0 for non archive files.   */

    file_ptr origin;             

     /* Remember when output has begun, to stop strange things
       happening. */
    boolean output_has_begun;

     /* Pointer to linked list of sections*/
    struct sec  *sections;

     /* The number of sections */
    unsigned int section_count;

     /* Stuff only useful for object files: 
       The start address. */
    bfd_vma start_address;

     /* Used for input and output*/
    unsigned int symcount;

     /* Symbol table for output BFD*/
    struct symbol_cache_entry  **outsymbols;             

     /* Pointer to structure which contains architecture information*/
    struct bfd_arch_info *arch_info;

     /* Stuff only useful for archives:*/
    PTR arelt_data;              
    struct _bfd *my_archive;     
    struct _bfd *next;           
    struct _bfd *archive_head;   
    boolean has_armap;           

     /* Used by the back end to hold private data. */

    union 
      {
      struct aout_data_struct *aout_data;
      struct artdata *aout_ar_data;
      struct _oasys_data *oasys_obj_data;
      struct _oasys_ar_data *oasys_ar_data;
      struct coff_tdata *coff_obj_data;
      struct ecoff_tdata *ecoff_obj_data;
      struct ieee_data_struct *ieee_data;
      struct ieee_ar_data_struct *ieee_ar_data;
      struct srec_data_struct *srec_data;
      struct tekhex_data_struct *tekhex_data;
      struct elf_obj_tdata *elf_obj_data;
      struct bout_data_struct *bout_data;
      struct sun_core_struct *sun_core_data;
      struct trad_core_struct *trad_core_data;
      struct hppa_data_struct *hppa_data;
      struct hppa_core_struct *hppa_core_data;
      PTR any;
      } tdata;
  
     /* Used by the application to hold private data*/
    PTR usrdata;

     /* Where all the allocated stuff under this BFD goes */
    struct obstack memory;

     /* Is this really needed in addition to usrdata?  */
    asymbol **ld_symbols;
};

unsigned int 
bfd_get_reloc_upper_bound PARAMS ((bfd *abfd, asection *sect));

unsigned int 
bfd_canonicalize_reloc
 PARAMS ((bfd *abfd,
    asection *sec,
    arelent **loc,
    asymbol	**syms));

boolean 
bfd_set_file_flags PARAMS ((bfd *abfd, flagword flags));

void 
bfd_set_reloc
 PARAMS ((bfd *abfd, asection *sec, arelent **rel, unsigned int count)
    
    );

boolean 
bfd_set_start_address PARAMS ((bfd *, bfd_vma));

long 
bfd_get_mtime PARAMS ((bfd *));

long 
bfd_get_size PARAMS ((bfd *));

#define bfd_sizeof_headers(abfd, reloc) \
     BFD_SEND (abfd, _bfd_sizeof_headers, (abfd, reloc))

#define bfd_find_nearest_line(abfd, sec, syms, off, file, func, line) \
     BFD_SEND (abfd, _bfd_find_nearest_line,  (abfd, sec, syms, off, file, func, line))

        /* Do these three do anything useful at all, for any back end?  */
#define bfd_debug_info_start(abfd) \
        BFD_SEND (abfd, _bfd_debug_info_start, (abfd))

#define bfd_debug_info_end(abfd) \
        BFD_SEND (abfd, _bfd_debug_info_end, (abfd))

#define bfd_debug_info_accumulate(abfd, section) \
        BFD_SEND (abfd, _bfd_debug_info_accumulate, (abfd, section))


#define bfd_stat_arch_elt(abfd, stat) \
        BFD_SEND (abfd, _bfd_stat_arch_elt,(abfd, stat))

#define bfd_set_arch_mach(abfd, arch, mach)\
        BFD_SEND ( abfd, _bfd_set_arch_mach, (abfd, arch, mach))

#define bfd_get_relocated_section_contents(abfd, seclet, data, relocateable) \
	BFD_SEND (abfd, _bfd_get_relocated_section_contents, (abfd, seclet, data, relocateable))
 
#define bfd_relax_section(abfd, section, symbols) \
       BFD_SEND (abfd, _bfd_relax_section, (abfd, section, symbols))

#define bfd_seclet_link(abfd, data, relocateable) \
       BFD_SEND (abfd, _bfd_seclet_link, (abfd, data, relocateable))
symindex 
bfd_get_next_mapent PARAMS ((bfd *, symindex previous, carsym ** sym));

boolean 
bfd_set_archive_head PARAMS ((bfd *output, bfd *new_head));

bfd *
bfd_get_elt_at_index PARAMS ((bfd * archive, int index));

bfd* 
bfd_openr_next_archived_file PARAMS ((bfd *archive, bfd *previous));

CONST char *
bfd_core_file_failing_command PARAMS ((bfd *));

int 
bfd_core_file_failing_signal PARAMS ((bfd *));

boolean 
core_file_matches_executable_p
 PARAMS ((bfd *core_bfd, bfd *exec_bfd));

#define BFD_SEND(bfd, message, arglist) \
               ((*((bfd)->xvec->message)) arglist)
#define BFD_SEND_FMT(bfd, message, arglist) \
            (((bfd)->xvec->message[(int)((bfd)->format)]) arglist)
typedef struct bfd_target
{
  char *name;
  enum target_flavour {
    bfd_target_unknown_flavour,
    bfd_target_aout_flavour,
    bfd_target_coff_flavour,
    bfd_target_ecoff_flavour,
    bfd_target_elf_flavour,
    bfd_target_ieee_flavour,
    bfd_target_oasys_flavour,
    bfd_target_tekhex_flavour,
    bfd_target_srec_flavour,
    bfd_target_hppa_flavour} flavour;
  boolean byteorder_big_p;
  boolean header_byteorder_big_p;
  flagword object_flags;       
  flagword section_flags;
  char symbol_leading_char;
  char ar_pad_char;            
  unsigned short ar_max_namelen;
  unsigned int align_power_min;
  bfd_vma      (*bfd_getx64) PARAMS ((bfd_byte *));
  void         (*bfd_putx64) PARAMS ((bfd_vma, bfd_byte *));
  bfd_vma      (*bfd_getx32) PARAMS ((bfd_byte *));
  void         (*bfd_putx32) PARAMS ((bfd_vma, bfd_byte *));
  bfd_vma      (*bfd_getx16) PARAMS ((bfd_byte *));
  void         (*bfd_putx16) PARAMS ((bfd_vma, bfd_byte *));
  bfd_vma      (*bfd_h_getx64) PARAMS ((bfd_byte *));
  void         (*bfd_h_putx64) PARAMS ((bfd_vma, bfd_byte *));
  bfd_vma      (*bfd_h_getx32) PARAMS ((bfd_byte *));
  void         (*bfd_h_putx32) PARAMS ((bfd_vma, bfd_byte *));
  bfd_vma      (*bfd_h_getx16) PARAMS ((bfd_byte *));
  void         (*bfd_h_putx16) PARAMS ((bfd_vma, bfd_byte *));
  struct bfd_target * (*_bfd_check_format[bfd_type_end]) PARAMS ((bfd *));
  boolean             (*_bfd_set_format[bfd_type_end]) PARAMS ((bfd *));
  boolean             (*_bfd_write_contents[bfd_type_end]) PARAMS ((bfd *));
  char *   (*_core_file_failing_command) PARAMS ((bfd *));
  int      (*_core_file_failing_signal) PARAMS ((bfd *));
  boolean  (*_core_file_matches_executable_p) PARAMS ((bfd *, bfd *));
  boolean  (*_bfd_slurp_armap) PARAMS ((bfd *));
  boolean  (*_bfd_slurp_extended_name_table) PARAMS ((bfd *));
  void     (*_bfd_truncate_arname) PARAMS ((bfd *, CONST char *, char *));
  boolean  (*write_armap) PARAMS ((bfd *arch, 
                              unsigned int elength,
                              struct orl *map,
                              unsigned int orl_count, 
                              int stridx));
  boolean       (*_close_and_cleanup) PARAMS ((bfd *));
  boolean       (*_bfd_set_section_contents) PARAMS ((bfd *, sec_ptr, PTR,
                                            file_ptr, bfd_size_type));
  boolean       (*_bfd_get_section_contents) PARAMS ((bfd *, sec_ptr, PTR, 
                                            file_ptr, bfd_size_type));
  boolean       (*_new_section_hook) PARAMS ((bfd *, sec_ptr));
  unsigned int  (*_get_symtab_upper_bound) PARAMS ((bfd *));
  unsigned int  (*_bfd_canonicalize_symtab) PARAMS ((bfd *,
                                              struct symbol_cache_entry **));
  unsigned int  (*_get_reloc_upper_bound) PARAMS ((bfd *, sec_ptr));
  unsigned int  (*_bfd_canonicalize_reloc) PARAMS ((bfd *, sec_ptr, arelent **,
                                              struct symbol_cache_entry **));
  struct symbol_cache_entry  *
                (*_bfd_make_empty_symbol) PARAMS ((bfd *));
  void          (*_bfd_print_symbol) PARAMS ((bfd *, PTR,
                                      struct symbol_cache_entry *,
                                      bfd_print_symbol_type));
#define bfd_print_symbol(b,p,s,e) BFD_SEND(b, _bfd_print_symbol, (b,p,s,e))
  alent *    (*_get_lineno) PARAMS ((bfd *, struct symbol_cache_entry *));

  boolean    (*_bfd_set_arch_mach) PARAMS ((bfd *, enum bfd_architecture,
                    unsigned long));

  bfd *      (*openr_next_archived_file) PARAMS ((bfd *arch, bfd *prev));
 
  boolean    (*_bfd_find_nearest_line) PARAMS ((bfd *abfd,
                    struct sec *section, struct symbol_cache_entry **symbols,
                    bfd_vma offset, CONST char **file, CONST char **func,
                    unsigned int *line));
 
  int        (*_bfd_stat_arch_elt) PARAMS ((bfd *, struct stat *));

  int        (*_bfd_sizeof_headers) PARAMS ((bfd *, boolean));

  void       (*_bfd_debug_info_start) PARAMS ((bfd *));
  void       (*_bfd_debug_info_end) PARAMS ((bfd *));
  void       (*_bfd_debug_info_accumulate) PARAMS ((bfd *, struct sec *));

  bfd_byte * (*_bfd_get_relocated_section_contents) PARAMS ((bfd *,
                    struct bfd_seclet *, bfd_byte *data,
                    boolean relocateable));

  boolean    (*_bfd_relax_section) PARAMS ((bfd *, struct sec *,
                    struct symbol_cache_entry **));

  boolean    (*_bfd_seclet_link) PARAMS ((bfd *, PTR data,
                     boolean relocateable));
  /* See documentation on reloc types.  */
 CONST struct reloc_howto_struct *
       (*reloc_type_lookup) PARAMS ((bfd *abfd,
                                     bfd_reloc_code_real_type code));

  /* Back-door to allow format-aware applications to create debug symbols
    while using BFD for everything else.  Currently used by the assembler
    when creating COFF files.  */
 asymbol *  (*_bfd_make_debug_symbol) PARAMS ((
       bfd *abfd,
       void *ptr,
       unsigned long size));
 PTR backend_data;
} bfd_target;
bfd_target *
bfd_find_target PARAMS ((CONST char *, bfd *));

CONST char **
bfd_target_list PARAMS ((void));

boolean 
bfd_check_format PARAMS ((bfd *abfd, bfd_format format));

boolean 
bfd_set_format PARAMS ((bfd *, bfd_format));

CONST char *
bfd_format_string PARAMS ((bfd_format));

#endif
