/* A -*- C -*- header file for the bfd library
   Copyright 1990, 1991 Free Software Foundation, Inc.
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

#define BFD_VERSION "0.18"

/* forward declaration */
typedef struct _bfd bfd;

/* General rules: functions which are boolean return true on success
   and false on failure (unless they're a predicate).   -- bfd.doc */
/* I'm sure this is going to break something and someone is going to
   force me to change it. */
/* typedef enum boolean {false, true} boolean; */
/* Yup, SVR4 has a "typedef enum boolean" in <sys/types.h>  -fnf */
typedef enum bfd_boolean {false, true} boolean;

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
	      no_debug_section,
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
  bfd_print_symbol_name,
  bfd_print_symbol_more,
  bfd_print_symbol_all,
  bfd_print_symbol_nm, /* Pretty format suitable for nm program. */
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
CAT(NAME,_bfd_debug_info_accumulate)

#define COFF_SWAP_TABLE \
 coff_swap_aux_in, coff_swap_sym_in, coff_swap_lineno_in, \
 coff_swap_aux_out, coff_swap_sym_out, \
 coff_swap_lineno_out, coff_swap_reloc_out, \
 coff_swap_filehdr_out, coff_swap_aouthdr_out, \
 coff_swap_scnhdr_out



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



#define BYTE_SIZE 1
#define SHORT_SIZE 2
#define LONG_SIZE 4



/* ANd more from the source */







void EXFUN(bfd_init, (void));
bfd *EXFUN(bfd_openr, (CONST char *filename, CONST char*target));
bfd *EXFUN(bfd_fdopenr, (CONST char *filename, CONST char *target, int fd));
bfd *EXFUN(bfd_openw, (CONST char *filename, CONST char *target));
boolean EXFUN(bfd_close, (bfd *));
boolean EXFUN(bfd_close_all_done, (bfd *));
bfd *EXFUN(bfd_create, (CONST char *filename, bfd *template));
bfd_size_type EXFUN(bfd_alloc_size, (bfd *abfd));
PTR  EXFUN(bfd_xmalloc, ( bfd_size_type size));
void EXFUN(bfd_write_bigendian_4byte_int, (bfd *abfd,  int i));
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
typedef struct sec {
    CONST char *name;
    struct sec *next;
    flagword flags;
#define SEC_NO_FLAGS   0x000
#define SEC_ALLOC      0x001
#define SEC_LOAD       0x002
#define SEC_RELOC      0x004
#define SEC_BALIGN     0x008
#define SEC_READONLY   0x010
#define SEC_CODE       0x020
#define SEC_DATA        0x040
#define SEC_ROM        0x080
#define SEC_CONSTRUCTOR 0x100
#define SEC_CONSTRUCTOR_TEXT 0x1100
#define SEC_CONSTRUCTOR_DATA 0x2100
#define SEC_CONSTRUCTOR_BSS  0x3100
#define SEC_HAS_CONTENTS 0x200
#define SEC_NEVER_LOAD 0x400
   bfd_vma vma;
   bfd_size_type size;    
   bfd_vma output_offset;
   struct sec *output_section;
   unsigned int alignment_power;
   struct reloc_cache_entry *relocation;
   struct reloc_cache_entry **orelocation;
   unsigned reloc_count;
   int index;                      
   file_ptr filepos;      
   file_ptr rel_filepos;
   file_ptr line_filepos;
   PTR userdata;
   struct lang_output_section *otheruserdata;
   alent *lineno;
   unsigned int lineno_count;
   file_ptr moving_line_filepos;
   unsigned int target_index;
   PTR used_by_bfd;
   struct relent_chain *constructor_chain;
   bfd *owner;
} asection ;
asection *EXFUN(bfd_get_section_by_name, (bfd *abfd, CONST char *name));
asection *EXFUN(bfd_make_section_old_way, (bfd *, CONST char *name));
asection * EXFUN(bfd_make_section, (bfd *, CONST char *name));
boolean EXFUN(bfd_set_section_flags, (bfd *, asection *, flagword));
void EXFUN(bfd_map_over_sections, (bfd *abfd, void (*func)(), PTR obj));
boolean EXFUN(bfd_set_section_size, (bfd *, asection *, bfd_size_type val));
boolean EXFUN(bfd_set_section_contents
, (bfd *abfd,        
asection *section,
PTR data,
file_ptr offset,
bfd_size_type count));
boolean EXFUN(bfd_get_section_contents 
, (bfd *abfd, asection *section, PTR location,
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
  bfd_arch_ns32k,      /* National Semiconductor 32xxx */
  bfd_arch_tahoe,      /* CCI/Harris Tahoe */
  bfd_arch_i860,       /* Intel 860 */
  bfd_arch_romp,       /* IBM ROMP PC/RT */
  bfd_arch_alliant,    /* Alliant */
  bfd_arch_convex,     /* Convex */
  bfd_arch_m88k,       /* Motorola 88xxx */
  bfd_arch_pyramid,    /* Pyramid Technology */
  bfd_arch_h8300,      /* Hitachi H8/300 */
  bfd_arch_rs6000,     /* IBM RS/6000 */
  bfd_arch_last
  };
typedef int bfd_reloc_code_type;

typedef struct bfd_arch_info 
{
  int bits_per_word;
  int bits_per_address;
  int bits_per_byte;
  enum bfd_architecture arch;
  long mach;
  char *arch_name;
  CONST  char *printable_name;
 /* true if this is the default machine for the architecture */
  boolean the_default;	
  CONST struct bfd_arch_info * EXFUN((*compatible),
	(CONST struct bfd_arch_info *a,
	 CONST struct bfd_arch_info *b));

  boolean EXFUN((*scan),(CONST struct bfd_arch_info *,CONST char *));
  unsigned int EXFUN((*disassemble),(bfd_vma addr, CONST char *data,
				     PTR stream));
  CONST struct reloc_howto_struct *EXFUN((*reloc_type_lookup),
    (CONST struct bfd_arch_info *,
    bfd_reloc_code_type  code));

  struct bfd_arch_info *next;

} bfd_arch_info_type;
CONST char *EXFUN(bfd_printable_name, (bfd *abfd));
bfd_arch_info_type *EXFUN(bfd_scan_arch, (CONST char *));
CONST bfd_arch_info_type *EXFUN(bfd_arch_get_compatible, (
CONST bfd *abfd,
CONST bfd *bbfd));
extern bfd_arch_info_type bfd_default_arch_struct;
void EXFUN(bfd_set_arch_info, (bfd *, bfd_arch_info_type *));
enum bfd_architecture EXFUN(bfd_get_arch, (bfd *abfd));
unsigned long EXFUN(bfd_get_mach, (bfd *abfd));
unsigned int EXFUN(bfd_arch_bits_per_byte, (bfd *abfd));
unsigned int EXFUN(bfd_arch_bits_per_address, (bfd *abfd));
bfd_arch_info_type * EXFUN(bfd_get_arch_info, (bfd *));
bfd_arch_info_type *EXFUN(bfd_lookup_arch
, (enum bfd_architecture
arch,
long machine));
CONST char * EXFUN(bfd_printable_arch_mach
, (enum bfd_architecture arch, unsigned long machine));
typedef enum bfd_reloc_status {
  bfd_reloc_ok,
  bfd_reloc_overflow,
  bfd_reloc_outofrange,
  bfd_reloc_continue,
  bfd_reloc_notsupported,
  bfd_reloc_other,
  bfd_reloc_undefined,
  bfd_reloc_dangerous
   }
 bfd_reloc_status_type;
typedef struct reloc_cache_entry 
{
  struct symbol_cache_entry **sym_ptr_ptr;
  rawdata_offset address;
  bfd_vma addend;    
  struct sec *section;
  CONST struct reloc_howto_struct *howto;
} arelent;
typedef CONST struct reloc_howto_struct 
{ 
  unsigned int type;
  unsigned int rightshift;
  unsigned int size;
  unsigned int bitsize;
  boolean pc_relative;
  unsigned int bitpos;
  boolean absolute;
  boolean complain_on_overflow;
  bfd_reloc_status_type (*special_function)();
  char *name;
  boolean partial_inplace;
  bfd_word src_mask;
  bfd_word dst_mask;           
  boolean pcrel_offset;
} reloc_howto_type;
#define HOWTO(C, R,S,B, P, BI, ABS, O, SF, NAME, INPLACE, MASKSRC, MASKDST, PC) \
  {(unsigned)C,R,S,B, P, BI, ABS,O,SF,NAME,INPLACE,MASKSRC,MASKDST,PC}
#define NEWHOWTO( FUNCTION, NAME,SIZE,REL,IN) HOWTO(0,0,SIZE,0,REL,0,false,false,FUNCTION, NAME,false,0,0,IN)

#define HOWTO_PREPARE(relocation, symbol) 	\
  {						\
  if (symbol != (asymbol *)NULL) {		\
    if (symbol->flags & BSF_FORT_COMM) {	\
      relocation = 0;				\
    }						\
    else {					\
      relocation = symbol->value;		\
    }						\
  }						\
  if (symbol->section != (asection *)NULL) {	\
    relocation += symbol->section->output_section->vma +	\
      symbol->section->output_offset;		\
  }						\
}			
typedef unsigned char bfd_byte;

typedef struct relent_chain {
  arelent relent;
  struct   relent_chain *next;
} arelent_chain;
bfd_reloc_status_type
EXFUN(bfd_perform_relocation
, (bfd * abfd,
arelent *reloc_entry,
PTR data,
asection *input_section,
bfd *output_bfd));
typedef enum bfd_reloc_code_real {
  BFD_RELOC_16,	
  BFD_RELOC_8_FFnn,
  BFD_RELOC_8,
  BFD_RELOC_8_PCREL,
  BFD_RELOC_CTOR
 } bfd_reloc_code_real_type;
CONST struct reloc_howto_struct *
EXFUN(bfd_reloc_type_lookup
, (CONST bfd_arch_info_type *arch, bfd_reloc_code_type code));
typedef struct symbol_cache_entry 
{
  struct _bfd *the_bfd;
   CONST char *name;
   symvalue value;
#define BSF_NO_FLAGS    0x00
#define BSF_LOCAL	0x01
#define BSF_GLOBAL	0x02
#define BSF_IMPORT	0x04
#define BSF_EXPORT	0x08
#define BSF_UNDEFINED	0x10	
#define BSF_FORT_COMM	0x20	
#define BSF_DEBUGGING	0x40
#define BSF_ABSOLUTE	0x80
#define BSF_KEEP        0x10000
#define BSF_KEEP_G      0x80000
#define BSF_WEAK        0x100000
#define BSF_CTOR        0x200000 
#define BSF_FAKE        0x400000 
#define BSF_OLD_COMMON  0x800000  
#define BFD_FORT_COMM_DEFAULT_VALUE 0
#define BSF_NOT_AT_END    0x40000
#define BSF_CONSTRUCTOR   0x1000000
#define BSF_WARNING       0x2000000
#define BSF_INDIRECT     0x4000000
  flagword flags;
  struct sec *section;
  PTR udata;	
} asymbol;
#define get_symtab_upper_bound(abfd) \
     BFD_SEND (abfd, _get_symtab_upper_bound, (abfd))
#define bfd_canonicalize_symtab(abfd, location) \
     BFD_SEND (abfd, _bfd_canonicalize_symtab,\
                  (abfd, location))
boolean EXFUN(bfd_set_symtab , (bfd *, asymbol **, unsigned int ));
void EXFUN(bfd_print_symbol_vandf, (PTR file, asymbol *symbol));
#define bfd_make_empty_symbol(abfd) \
     BFD_SEND (abfd, _bfd_make_empty_symbol, (abfd))
int EXFUN(bfd_decode_symclass, (asymbol *symbol));
struct _bfd 
{
  CONST char *filename;                
  struct bfd_target *xvec;
  char *iostream;
  boolean cacheable;
  boolean target_defaulted;
  struct _bfd *lru_prev, *lru_next;
  file_ptr where;              
  boolean opened_once;
  boolean mtime_set;
  long mtime;          
  int ifd;
  bfd_format format;
  enum bfd_direction {no_direction = 0,
                       read_direction = 1,
                       write_direction = 2,
                       both_direction = 3} direction;
  flagword flags;              
  file_ptr origin;             
  boolean output_has_begun;
  struct sec  *sections;
  unsigned int section_count;
  bfd_vma start_address;
  unsigned int symcount;
  struct symbol_cache_entry  **outsymbols;             
  struct bfd_arch_info *arch_info;
  PTR arelt_data;              
  struct _bfd *my_archive;     
  struct _bfd *next;           
  struct _bfd *archive_head;   
  boolean has_armap;           
  PTR tdata;
  PTR usrdata;
  struct obstack memory;
};
boolean EXFUN(bfd_set_start_address, (bfd *, bfd_vma));
long EXFUN(bfd_get_mtime, (bfd *));
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

#define bfd_set_arch_mach(abfd, arch, mach)\
        BFD_SEND ( abfd, _bfd_set_arch_mach, (abfd, arch, mach))

#define bfd_coff_swap_reloc_out(abfd, i, o) \
        BFD_SEND (abfd, _bfd_coff_swap_reloc_out, (abfd, i, o))

#define bfd_coff_swap_lineno_out(abfd, i, o) \
        BFD_SEND (abfd, _bfd_coff_swap_lineno_out, (abfd, i, o))

#define bfd_coff_swap_aux_out(abfd, i, t,c,o) \
        BFD_SEND (abfd, _bfd_coff_swap_aux_out, (abfd, i,t,c, o))

#define bfd_coff_swap_sym_out(abfd, i,o) \
        BFD_SEND (abfd, _bfd_coff_swap_sym_out, (abfd, i, o))

#define bfd_coff_swap_scnhdr_out(abfd, i,o) \
        BFD_SEND (abfd, _bfd_coff_swap_scnhdr_out, (abfd, i, o))

#define bfd_coff_swap_filehdr_out(abfd, i,o) \
        BFD_SEND (abfd, _bfd_coff_swap_filehdr_out, (abfd, i, o))

#define bfd_coff_swap_aouthdr_out(abfd, i,o) \
        BFD_SEND (abfd, _bfd_coff_swap_aouthdr_out, (abfd, i, o))

symindex EXFUN(bfd_get_next_mapent, (bfd *, symindex, carsym **));
boolean EXFUN(bfd_set_archive_head, (bfd *output, bfd *new_head));
bfd *EXFUN(bfd_get_elt_at_index, (bfd *, int));
bfd* EXFUN(bfd_openr_next_archived_file, (bfd *archive, bfd *previous));
CONST char *EXFUN(bfd_core_file_failing_command, (bfd *));
int EXFUN(bfd_core_file_failing_signal, (bfd *));
boolean EXFUN(core_file_matches_executable_p
, (bfd *core_bfd, bfd *exec_bfd));
#define SDEF(ret, name, arglist) \
                PROTO(ret,(*name),arglist)
#define SDEF_FMT(ret, name, arglist) \
                PROTO(ret,(*name[bfd_type_end]),arglist)
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
    bfd_target_elf_flavour,
    bfd_target_ieee_flavour,
    bfd_target_oasys_flavour,
    bfd_target_srec_flavour} flavour;
  boolean byteorder_big_p;
  boolean header_byteorder_big_p;
  flagword object_flags;       
  flagword section_flags;
  char ar_pad_char;            
 unsigned short ar_max_namelen;
  unsigned int align_power_min;
  SDEF (bfd_vma,      bfd_getx64, (bfd_byte *));
  SDEF (void,         bfd_putx64, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma, bfd_getx32, (bfd_byte *));
  SDEF (void,         bfd_putx32, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma, bfd_getx16, (bfd_byte *));
  SDEF (void,         bfd_putx16, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma,   bfd_h_getx64, (bfd_byte *));
  SDEF (void,          bfd_h_putx64, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma,  bfd_h_getx32, (bfd_byte *));
  SDEF (void,          bfd_h_putx32, (bfd_vma, bfd_byte *));
  SDEF (bfd_vma,  bfd_h_getx16, (bfd_byte *));
  SDEF (void,          bfd_h_putx16, (bfd_vma, bfd_byte *));
  SDEF_FMT (struct bfd_target *, _bfd_check_format, (bfd *));
  SDEF_FMT (boolean,            _bfd_set_format, (bfd *));
  SDEF_FMT (boolean,            _bfd_write_contents, (bfd *));
  SDEF (char *, _core_file_failing_command, (bfd *));
  SDEF (int,    _core_file_failing_signal, (bfd *));
  SDEF (boolean, _core_file_matches_executable_p, (bfd *, bfd *));
 SDEF (boolean, _bfd_slurp_armap, (bfd *));
 SDEF (boolean, _bfd_slurp_extended_name_table, (bfd *));
 SDEF (void,   _bfd_truncate_arname, (bfd *, CONST char *, char *));
 SDEF (boolean, write_armap, (bfd *arch, 
                              unsigned int elength,
                              struct orl *map,
                              unsigned int orl_count, 
                              int stridx));
  SDEF (boolean, _close_and_cleanup, (bfd *));
  SDEF (boolean, _bfd_set_section_contents, (bfd *, sec_ptr, PTR,
                                            file_ptr, bfd_size_type));
  SDEF (boolean, _bfd_get_section_contents, (bfd *, sec_ptr, PTR, 
                                            file_ptr, bfd_size_type));
  SDEF (boolean, _new_section_hook, (bfd *, sec_ptr));
  SDEF (unsigned int, _get_symtab_upper_bound, (bfd *));
  SDEF (unsigned int, _bfd_canonicalize_symtab,
           (bfd *, struct symbol_cache_entry **));
  SDEF (unsigned int, _get_reloc_upper_bound, (bfd *, sec_ptr));
  SDEF (unsigned int, _bfd_canonicalize_reloc, (bfd *, sec_ptr, arelent **,
                                               struct symbol_cache_entry**));
  SDEF (struct symbol_cache_entry  *, _bfd_make_empty_symbol, (bfd *));
  SDEF (void,     _bfd_print_symbol, (bfd *, PTR, struct symbol_cache_entry  *,
                                      bfd_print_symbol_type));
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

 SDEF(unsigned int, _bfd_coff_swap_aux_out,(
       bfd   	*abfd,
       PTR	in,
       int    	type,
       int    	class,
       PTR    	ext));

 SDEF(unsigned int, _bfd_coff_swap_sym_out,(
      bfd      *abfd,
      PTR	in,
      PTR	ext));

 SDEF(unsigned int, _bfd_coff_swap_lineno_out,(
      	bfd   	*abfd,
      	PTR	in,
	PTR	ext));

 SDEF(unsigned int, _bfd_coff_swap_reloc_out,(
      	bfd     *abfd,
     	PTR	src,
	PTR	dst));

 SDEF(unsigned int, _bfd_coff_swap_filehdr_out,(
      	bfd  	*abfd,
	PTR 	in,
	PTR 	out));

 SDEF(unsigned int, _bfd_coff_swap_aouthdr_out,(
      	bfd 	*abfd,
	PTR 	in,
	PTR	out));

 SDEF(unsigned int, _bfd_coff_swap_scnhdr_out,(
      	bfd  	*abfd,
      	PTR	in,
	PTR	out));

} bfd_target;
bfd_target *EXFUN(bfd_find_target, (CONST char *, bfd *));
CONST char **EXFUN(bfd_target_list, ());
boolean EXFUN(bfd_check_format, (bfd *abfd, bfd_format format));
boolean EXFUN(bfd_set_format, (bfd *, bfd_format));
CONST char *EXFUN(bfd_format_string, (bfd_format));
#endif
