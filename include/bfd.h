 	  /* A -*- C -*- header file for the bfd library */

/* bfd.h -- The only header file required by users of the bfd library */

/* Copyright (C) 1990, 1991 Free Software Foundation, Inc.

This file is part of BFD, the Binary File Diddler.

BFD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

BFD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BFD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

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

#define BFD_VERSION "1.10"

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
#define printf_vma(x)	 printf(    "%08x", x)
#define fprintf_vma(s,x) fprintf(s, "%08x", x)
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

/* This enum gives the object file's CPU architecture, in a global sense.
   E.g. what processor family does it belong to?  There is another field,
   which indicates what processor within the family is in use.  */
enum bfd_architecture {
	bfd_arch_unknown,	/* File arch not known */
	bfd_arch_obscure,	/* File arch known, not one of these */
	bfd_arch_m68k,		/* Motorola 68xxx */
	bfd_arch_vax,		/* DEC Vax */
	bfd_arch_i960,		/* Intel 960 */
 			
 				/* The order of the following is important.
 				 * A lower number indicates a machine type
 				 * that only accepts a subset of the
 				 * instructions available to machines with
 				 * higher numbers.
 				 *
 				 * The exception is the "ca", which is
 				 * incompatible with all other machines except
 				 * "core".
 				 */
#define	bfd_mach_i960_core	1
#define	bfd_mach_i960_ka_sa	2
#define	bfd_mach_i960_kb_sb	3
#define	bfd_mach_i960_mc	4
#define	bfd_mach_i960_xa	5
#define	bfd_mach_i960_ca	6

	bfd_arch_a29k,		/* AMD 29000 */
	bfd_arch_sparc,		/* Sun (SPARC International) SPARC */
	bfd_arch_mips,		/* MIPS Rxxxx */
	bfd_arch_i386,		/* Intel 386 */
	bfd_arch_ns32k,		/* National Semiconductor 32xxx */
	bfd_arch_tahoe,		/* CCI/Harris Tahoe */
	bfd_arch_i860,		/* Intel 860 */
	bfd_arch_romp,		/* IBM ROMP RS/6000 */
	bfd_arch_alliant,	/* Alliant */
	bfd_arch_convex,	/* Convex */
	bfd_arch_m88k,		/* Motorola 88xxx */
	bfd_arch_pyramid,	/* Pyramid Technology */
	bfd_arch_h8_300,	/* Hitachi H8/300 */
	bfd_arch_last
};

/* symbols and relocation */

typedef unsigned long symindex;

#define BFD_NO_MORE_SYMBOLS ((symindex) ~0)

typedef enum {bfd_symclass_unknown = 0,
	      bfd_symclass_fcommon, /* fortran common symbols */
	      bfd_symclass_global, /* global symbol, what a surprise */
	      bfd_symclass_debugger, /* some debugger symbol */
	      bfd_symclass_undefined /* none known */
	    } symclass;


typedef int symtype;		/* Who knows, yet? */

/* Symbol cache classifications: (Bfd-Symbol-Flag_FOOBAR) */
#define BSF_NO_FLAGS    0x00
#define BSF_LOCAL	0x01	/* bfd_symclass_unknown */
#define BSF_GLOBAL	0x02	/* bfd_symclass_global */
#define BSF_IMPORT	0x04
#define BSF_EXPORT	0x08
#define BSF_UNDEFINED	0x10	/* bfd_symclass_undefined */
#define BSF_FORT_COMM	0x20	/* bfd_symclass_fcommon */
#define BSF_DEBUGGING	0x40	/* bfd_symclass_debugger */
#define BSF_ABSOLUTE	0x80
#define BSF_KEEP        0x10000
#define BSF_WARNING     0x20000
#define BSF_KEEP_G      0x80000
#define BSF_WEAK        0x100000
#define BSF_CTOR        0x200000  /* Symbol is a con/destructor */
#define BSF_FAKE        0x400000  /* Symbol doesn't really exist */
#define BSF_OLD_COMMON  0x800000  /* Symbol used to be common,
				    but now is allocated */

/* If symbol is fort_comm, then value is size, and this is the contents */
#define BFD_FORT_COMM_DEFAULT_VALUE 0

/* in some files the type of a symbol sometimes alters its location
   in an output file - ie in coff a ISFCN symbol which is also C_EXT 
   symbol appears where it was declared and not at the end of a section. 
   This bit is set by the target bfd part to convey this information.  */
#define BSF_NOT_AT_END    0x40000


/* general purpose part of a symbol;
   target specific parts will be found in libcoff.h, liba.out.h etc */
typedef struct symbol_cache_entry 
{
  struct _bfd *the_bfd;	/* Just a way to find out host type */
  CONST char *name;
  symvalue value;
  flagword flags;
  struct sec *section;
  PTR udata;			/* Target-specific stuff */
} asymbol;

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

/* Relocation stuff */

/* Either: sym will point to a symbol and isextern will be 0, *OR*
   sym will be NULL and isextern will be a symbol type (eg N_TEXT)
   which means the location should be relocated relative to the
   segment origin.  This is because we won't necessarily have a symbol
   which is guaranteed to point to the segment origin. */

typedef enum bfd_reloc_status {
  bfd_reloc_ok,
  bfd_reloc_overflow,
  bfd_reloc_outofrange,
  bfd_reloc_continue,
  bfd_reloc_notsupported,
  bfd_reloc_other,
  bfd_reloc_undefined,
  bfd_reloc_dangerous}
 bfd_reloc_status_enum_type;

typedef CONST struct rint 
{ 
  unsigned int type;
  unsigned int rightshift;
  unsigned int size;
  unsigned int bitsize;
  boolean pc_relative;
  unsigned int bitpos;

  boolean absolute;
  boolean complain_on_overflow;
  bfd_reloc_status_enum_type (*special_function)();
  char *name;
  boolean partial_inplace;
  /* Two mask fields,
     the src_mask is used to select what parts of the read in data are to
     be used in the relocation sum. Eg, if this was an 8 bit bit of data
     which we read and relocated, this would be 0x000000ff. When we have
     relocs which have an addend, such as sun4 extended relocs, the value
     in the offset part of a relocating field is garbage so we never use
     it. In this case the mask would be 0x00000000.
     
     The dst_mask is what parts of the instruction are replaced into the
     instruction. In most cases src_mask == dst_mask, except in the above
     special case, where dst_mask would be 0x000000ff, and src_mask would
     be 0x00000000.
     */
  bfd_word src_mask;		/* What things to take from the source */
  bfd_word dst_mask;		/* What things to put into the dest */

  /* Does a pc rel offset already have the offset of the jump from the
     beginnining of the module in place - eg on the sun3, a pcrel
     instruction always has a negative number in place, containing the
     displacement from the beginning of the module. 88k bcs has a zero
     in there, so you have to work out the offset yourself.  */ 
  boolean pcrel_offset;
} reloc_howto_type;

#define HOWTO(CODE, RT,SIZE,BITSIZE, PCREL, BITPOS, ABS, OVERFLOW, SF, NAME, INPLACE, MASKSRC, MASKDST, PCRELDONE) \
{(unsigned)CODE,RT,SIZE,BITSIZE, PCREL,  BITPOS,ABS,OVERFLOW,SF,NAME,INPLACE,MASKSRC,MASKDST,PCRELDONE}

typedef unsigned char bfd_byte;

typedef struct reloc_cache_entry 
{
  /* A pointer into the canonicalized table for the symbol */
  asymbol **sym_ptr_ptr;

  rawdata_offset address;	/* offset in section */
  bfd_vma addend;		/* addend for relocation value */
  struct sec *section;   /* if sym is null this is the section */
  reloc_howto_type *howto;

} arelent;

typedef struct relent_chain {
  arelent relent;
  struct   relent_chain *next;
} arelent_chain;
  
/* Used in generating armaps.  Perhaps just a forward definition would do? */
struct orl {			/* output ranlib */
  char **name;			/* symbol name */ 
  file_ptr pos;			/* element number or file position */
  int namidx;			/* index into string table */
};



/* Linenumber stuff */
typedef struct lineno_cache_entry {
  unsigned int line_number;	/* Linenumber from start of function*/  
  union {
    asymbol *sym;		/* Function name */
    unsigned long offset;	/* Offset into section */
  } u;
} alent;

/* object and core file sections */

/* Section flag definitions */
#define SEC_NO_FLAGS 000
#define SEC_ALLOC  001
#define SEC_LOAD   002
#define SEC_RELOC  004
#define SEC_BALIGN 010
#define SEC_READONLY 020
#define SEC_CODE   040
#define SEC_DATA   0100
#define SEC_ROM    0200
#define SEC_CONSTRUCTOR 0400
#define SEC_HAS_CONTENTS	(0x200)

typedef struct sec 
{
  CONST char *name;
  struct sec *next;
  flagword flags;

  bfd_vma vma;
  bfd_size_type size;    

  /* The output_offset is the indent into the output section of
     this section. If this is the first section to go into
     an output section, this value will be 0...  */
  bfd_vma output_offset;
  struct sec *output_section;
  unsigned int alignment_power; /* eg 4 aligns to 2^4*/

  arelent *relocation;		/* for input files */
  arelent **orelocation;	/* for output files */

  unsigned reloc_count;
  file_ptr filepos;		/* File position of section data */
  file_ptr rel_filepos;		/* File position of relocation info */
  file_ptr line_filepos;
  PTR *userdata;
  struct lang_output_section *otheruserdata;
  int index;			/* Which section is it 0..nth */
  alent *lineno; 
  unsigned int lineno_count;

  /* When a section is being output, this value changes as more
     linenumbers are written out */
  file_ptr moving_line_filepos;

  /* what the section number is in the target world */
  unsigned int target_index;

  PTR used_by_bfd;

  /* If this is a constructor section then here is a list of relents */
  arelent_chain *constructor_chain;
} asection;

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

typedef enum {no_error = 0, system_call_error, invalid_target,
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
    

/* The BFD target structure.

   This structure is how to find out everything BFD knows about a target.
   It includes things like its byte order, name, what routines to call
   to do various operations, etc.   

   Every BFD points to a target structure with its "xvec" member.  */

/* Shortcut for declaring fields which are prototyped function pointers,
   while avoiding anguish on compilers that don't support protos. */

#define SDEF(ret, name, arglist) PROTO(ret,(*name),arglist)
#define SDEF_FMT(ret, name, arglist) PROTO(ret,(*name[bfd_type_end]),arglist)

/* These macros are used to dispatch to functions through the bfd_target
   vector.  They are used in a number of macros further down in bfd.h,
   and are also used when calling various routines by hand inside the
   bfd implementation.  The "arglist" argument must be parenthesized;
   it contains all the arguments to the called function.  */

#define BFD_SEND(bfd, message, arglist) ((*((bfd)->xvec->message)) arglist)
/* For operations which index on the bfd format */
#define BFD_SEND_FMT(bfd, message, arglist) \
                 (((bfd)->xvec->message[(int)((bfd)->format)]) arglist)

/*  This is the struct which defines the type of BFD this is.  The
    "xvec" member of the struct bfd itself points here.  Each module
    that implements access to a different target under BFD, defines
    one of these.  */

/* FIXME, these names should be rationalised with the names of the entry points
   which call them.  Too bad we can't have one macro to define them both! */
typedef struct bfd_target
{
  /* identifies the kind of target, eg SunOS4, Ultrix, etc */
  char *name;

  /* This is bogus.  Anything that can be told from the "flavour"
     by some user program should be an independent attribute that can
     be queried instead.  FIXME.  -- gnu@cygnus.com */
  enum target_flavour_enum {
    bfd_target_aout_flavour_enum,
    bfd_target_coff_flavour_enum,
    bfd_target_ieee_flavour_enum,
    bfd_target_oasys_flavour_enum,
    bfd_target_srec_flavour_enum} flavour;

  boolean byteorder_big_p;	/* Order of bytes in data sections */
  boolean header_byteorder_big_p; /* Order of bytes in header */

  flagword object_flags;	/* these are the ones that may be set */
  flagword section_flags;	/* ditto */

  char ar_pad_char;		/* filenames in archives padded w/this char */
  unsigned short ar_max_namelen; /* this could be a char too! */

  /* Byte swapping for data */
  /* Note that these don't take bfd as first arg.  Certain other handlers
     could do the same. */
  SDEF (bfd_64_type,	bfd_getx64, (bfd_byte *));
  SDEF (void,		bfd_putx64, (bfd_64_type, bfd_byte *));
  SDEF (unsigned int,	bfd_getx32, (bfd_byte *));
  SDEF (void,		bfd_putx32, (unsigned long, bfd_byte *));
  SDEF (unsigned int,	bfd_getx16, (bfd_byte *));
  SDEF (void,		bfd_putx16, (int, bfd_byte *));

  /* Byte swapping for headers */
  SDEF (bfd_64_type,	bfd_h_getx64, (bfd_byte *));
  SDEF (void,		bfd_h_putx64, (bfd_64_type, bfd_byte *));
  SDEF (unsigned int,	bfd_h_getx32, (bfd_byte *));
  SDEF (void,		bfd_h_putx32, (unsigned long, bfd_byte *));
  SDEF (unsigned int,	bfd_h_getx16, (bfd_byte *));
  SDEF (void,		bfd_h_putx16, (int, bfd_byte *));

  /* Format-dependent */
  /* Check the format of a file being read.  Return bfd_target * or zero. */
  SDEF_FMT (struct bfd_target *, _bfd_check_format, (bfd *));
  /* Set the format of a file being written.  */
  SDEF_FMT (boolean,		 _bfd_set_format, (bfd *));
  /* Write cached information into a file being written, at bfd_close.  */
  SDEF_FMT (boolean,		 _bfd_write_contents, (bfd *));

  /* All these are defined in JUMP_TABLE */
  /* Core files */
  SDEF (char *,	 _core_file_failing_command, (bfd *));
  SDEF (int,	 _core_file_failing_signal, (bfd *));
  SDEF (boolean, _core_file_matches_executable_p, (bfd *, bfd *));

  /* Archives */
  SDEF (boolean, _bfd_slurp_armap, (bfd *));
  SDEF (boolean, _bfd_slurp_extended_name_table, (bfd *));
  SDEF (void,	 _bfd_truncate_arname, (bfd *, CONST char *, char *));
  SDEF (boolean, write_armap, (bfd *arch, unsigned int elength,
			       struct orl *map, int orl_count, int
			       stridx));

  /* All the standard stuff */
  SDEF (boolean, _close_and_cleanup, (bfd *)); /* free any allocated data */
  SDEF (boolean, _bfd_set_section_contents, (bfd *, sec_ptr, PTR,
					    file_ptr, bfd_size_type));
  SDEF (boolean, _bfd_get_section_contents, (bfd *, sec_ptr, PTR, 
					     file_ptr, bfd_size_type));
  SDEF (boolean, _new_section_hook, (bfd *, sec_ptr));

  /* Symbols and relocation */
  SDEF (unsigned int, _get_symtab_upper_bound, (bfd *));
  SDEF (unsigned int, _bfd_canonicalize_symtab, (bfd *, asymbol **));
  SDEF (unsigned int, _get_reloc_upper_bound, (bfd *, sec_ptr));
  SDEF (unsigned int, _bfd_canonicalize_reloc, (bfd *, sec_ptr, arelent **,
						asymbol**));

  /* FIXME: For steve -- clean up later */
  SDEF (asymbol *, _bfd_make_empty_symbol, (bfd *));
  SDEF (void,	   _bfd_print_symbol, (bfd *, PTR, asymbol *,
				       bfd_print_symbol_enum_type));
  SDEF (alent *,   _get_lineno, (bfd *, asymbol *));

  SDEF (boolean,   _bfd_set_arch_mach, (bfd *, enum bfd_architecture,
				        unsigned long));

  SDEF (bfd *,	 openr_next_archived_file, (bfd *arch, bfd *prev));
  SDEF (boolean, _bfd_find_nearest_line,
        (bfd *abfd, asection *section, asymbol **symbols,bfd_vma offset,
	 CONST char **file, CONST char **func, unsigned int *line));
  SDEF (int,	 _bfd_stat_arch_elt, (bfd *, struct stat *));

  SDEF (int,	 _bfd_sizeof_headers, (bfd *, boolean));
} bfd_target;

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
CAT(NAME,_sizeof_headers)

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

/* Finally!  The BFD struct itself.  This contains the major data about
   the file, and contains pointers to the rest of the data.

   To avoid dragging too many header files into every file that
   includes bfd.h, IOSTREAM has been declared as a "char *", and MTIME
   as a "long".  Their correct types, to which they are cast when used,
   are "FILE *" and "time_t".  */

struct _bfd 
{
  CONST char *filename;		/* could be null; filename user opened with */
  bfd_target *xvec;		/* operation jump table */
  char *iostream;		/* stdio FILE *, unless an archive element */

  boolean cacheable;		/* iostream can be closed if desired */
  boolean target_defaulted;	/* Target type was default, not specific,
				   so we can try all the targets if needed. */
  struct _bfd *lru_prev;	/* Used for file caching */
  struct _bfd *lru_next;	/* Used for file caching */
  file_ptr where;		/* Where the file was when closed */
  boolean opened_once;
  boolean mtime_set;		/* Flag indicating mtime is available */
  long mtime;			/* File modified time */
  int ifd;			/* for output files, channel we locked. */
  bfd_format format;
  enum bfd_direction {no_direction = 0,
			read_direction = 1,
			write_direction = 2,
			both_direction = 3} direction;

  flagword flags;		/* format_specific */

  /* Currently my_archive is tested before adding origin to anything. I
     believe that this can become always an add of origin, with origin set
     to 0 for non archive files.  FIXME-soon. */
  file_ptr origin;		/* for archive contents */
  boolean output_has_begun;	/* cf bfd_set_section_size */
  asection *sections;		/* Pointer to linked list of sections */
  unsigned int section_count;	/* The number of sections */

  /* Some object file stuff */
  bfd_vma start_address;	/* for object files only, of course */
  unsigned int symcount;	/* used for input and output */
  asymbol **outsymbols;		/* symtab for output bfd */
  enum bfd_architecture obj_arch; /* Architecture of object machine, eg m68k */
  unsigned long obj_machine;	/* Particular machine within arch, e.g. 68010 */

  /* Archive stuff.  strictly speaking we don't need all three bfd* vars,
     but doing so would allow recursive archives! */
  PTR arelt_data;		/* needed if this came from an archive */
  struct _bfd *my_archive;	/* if this is an archive element */
  struct _bfd *next;		/* output chain pointer */
  struct _bfd *archive_head;	/* for output archive */
  boolean has_armap;		/* if an arch; has it an armap? */
  
  PTR tdata;			/* target-specific storage */
  PTR usrdata;			/* application-specific storage */

  /* Should probably be enabled here always, so that library may be changed
     to switch this on and off, while user code may remain unchanged */
#ifdef BFD_LOCKS
  struct flock *lock;
  char *actual_name;		/* for output files, name given to open()  */
#endif

  /* Where all the allocated stuff under this BFD goes */
  struct obstack memory;
};

/* The various callable routines */
PROTO (bfd_size_type, bfd_alloc_size,(bfd *abfd));
PROTO (char *,	bfd_printable_arch_mach,(enum bfd_architecture, unsigned long));
PROTO (char *,	bfd_format_string, (bfd_format format));

PROTO (char**,	bfd_target_list, ());
PROTO (bfd *,	bfd_openr, (CONST char *filename, CONST char *target));
PROTO (bfd *,	bfd_fdopenr,(CONST char *filename, CONST char *target, int fd));
PROTO (bfd *,	bfd_openw, (CONST char *filename, CONST char *target));
PROTO (bfd *,	bfd_create, (CONST char *filename, CONST bfd *abfd));
PROTO (boolean,	bfd_close, (bfd *abfd));
PROTO (long,	bfd_get_mtime, (bfd *abfd));
PROTO (bfd *,	bfd_openr_next_archived_file, (bfd *obfd, bfd *last_file));
PROTO (boolean,	bfd_set_archive_head, (bfd *output_archive, bfd *new_head));
PROTO (boolean,	bfd_check_format, (bfd *abfd, bfd_format format));
PROTO (boolean,	bfd_set_format, (bfd *abfd, bfd_format format));
PROTO (char *,	bfd_core_file_failing_command, (bfd *abfd));
PROTO (int,	bfd_core_file_failing_signal, (bfd *abfd));
PROTO (boolean,	core_file_matches_executable_p, (bfd *core_bfd, bfd *exec_bfd));
PROTO (sec_ptr,	bfd_get_section_by_name, (bfd *abfd, CONST char *name));
PROTO (void,	bfd_map_over_sections, (bfd *abfd, void (*operation)(),
				     PTR user_storage));
PROTO (sec_ptr,	bfd_make_section, (bfd *abfd, CONST char *CONST name));
PROTO (boolean,	bfd_set_section_flags, (bfd *abfd, sec_ptr section,
					flagword flags));
PROTO (boolean,	bfd_set_file_flags, (bfd *abfd, flagword flags));
PROTO (boolean,	bfd_arch_compatible,  (bfd *abfd, bfd *bbfd,
				    enum bfd_architecture *res_arch,
				    unsigned long *res_machine));

PROTO (boolean,	bfd_set_section_size, (bfd *abfd, sec_ptr ptr,
				       unsigned long val));
PROTO (boolean,	bfd_get_section_contents, (bfd *abfd, sec_ptr section,
					   PTR location, file_ptr offset,
					   bfd_size_type count));
PROTO (boolean,	bfd_set_section_contents, (bfd *abfd, sec_ptr section,
					   PTR location, file_ptr offset,
					   bfd_size_type count));

PROTO (unsigned long, bfd_get_next_mapent, (bfd *abfd, symindex prev,
					    carsym **entry));
PROTO (bfd *,	bfd_get_elt_at_index, (bfd *abfd, int index));
PROTO (boolean,	bfd_set_symtab, (bfd *abfd, asymbol **location,
				 unsigned int symcount));
PROTO (unsigned int, get_reloc_upper_bound, (bfd *abfd, sec_ptr asect));
PROTO (unsigned int, bfd_canonicalize_reloc, (bfd *abfd, sec_ptr asect,
					      arelent **location,
					      asymbol **canon));
PROTO (void,	bfd_set_reloc, (bfd *abfd, sec_ptr asect, arelent **location,
			     unsigned int count));
PROTO (boolean,	bfd_set_start_address, (bfd *,bfd_vma));

PROTO (void,	bfd_print_symbol_vandf, (PTR, asymbol *));
PROTO (bfd_reloc_status_enum_type, bfd_perform_relocation,
       (bfd *, arelent*, PTR, asection *, bfd*));

PROTO (bfd_vma,	bfd_log2, (bfd_vma));
#define bfd_symbol_same_target(abfd, symbol) \
( ( ((symbol)->the_bfd->xvec) == (abfd)->xvec) ? true:false)

PROTO(boolean,	bfd_scan_arch_mach,(CONST char *, enum bfd_architecture *,
				   unsigned long *));

/* For speed and simplicity, we turn calls to these interface routines
   directly into jumps through the transfer vector.  */

#define bfd_set_arch_mach(abfd, arch, mach) \
     BFD_SEND (abfd, _bfd_set_arch_mach, (abfd, arch, mach))

#define bfd_sizeof_headers(abfd, reloc) \
     BFD_SEND (abfd, _bfd_sizeof_headers, (abfd, reloc))


#define get_symtab_upper_bound(abfd) \
     BFD_SEND (abfd, _get_symtab_upper_bound, (abfd))

#define bfd_canonicalize_symtab(abfd, location) \
     BFD_SEND (abfd, _bfd_canonicalize_symtab, (abfd, location))


#define bfd_make_empty_symbol(abfd) \
     BFD_SEND (abfd, _bfd_make_empty_symbol, (abfd))

#define bfd_print_symbol(abfd, file, symbol, how) \
     BFD_SEND (abfd, _bfd_print_symbol, (abfd, file, symbol, how))

#define bfd_get_lineno(abfd, symbol) \
     BFD_SEND (abfd, _get_lineno, (abfd, symbol))

#define bfd_stat_arch_elt(abfd, buf) \
     BFD_SEND (abfd, _bfd_stat_arch_elt, (abfd, buf))

#define bfd_find_nearest_line(abfd, section, symbols, offset, filename_ptr, func, line_ptr) \
     BFD_SEND (abfd, _bfd_find_nearest_line, \
	 (abfd, section,symbols, offset, filename_ptr, func, line_ptr))

/* Some byte-swapping i/o operations */
#define LONGLONG_SIZE 8
#define LONG_SIZE 4
#define SHORT_SIZE 2
#define BYTE_SIZE 1

#define bfd_put_8(abfd, val, ptr)	(*((char *)ptr) = (char)val)
#define bfd_get_8(abfd, ptr)		(*((char *)ptr))

#define bfd_put_32(abfd, val, ptr)	BFD_SEND(abfd, bfd_putx32,   (val,ptr))
#define bfd_get_32(abfd, ptr)		BFD_SEND(abfd, bfd_getx32,   (ptr))

#define bfd_put_64(abfd, val, ptr)	BFD_SEND(abfd, bfd_putx64,   (val,ptr))
#define bfd_get_64(abfd, ptr)		BFD_SEND(abfd, bfd_getx64,   (ptr))

#define bfd_put_16(abfd, val, ptr)	BFD_SEND(abfd, bfd_putx16,  (val,ptr))
#define bfd_get_16(abfd, ptr)		BFD_SEND(abfd, bfd_getx16,  (ptr))

#define	bfd_h_put_8(abfd, val, ptr)	bfd_put_8 (abfd, val, ptr)
#define	bfd_h_get_8(abfd, ptr)		bfd_get_8 (abfd, ptr)

#define bfd_h_put_32(abfd, val, ptr)	BFD_SEND(abfd, bfd_h_putx32, \
							(val, (bfd_byte *) ptr))
#define bfd_h_get_32(abfd, ptr)		BFD_SEND(abfd, bfd_h_getx32, \
							((bfd_byte *) ptr))

#define bfd_h_put_64(abfd, val, ptr)	BFD_SEND(abfd, bfd_h_putx64, \
							(val, (bfd_byte *) ptr))
#define bfd_h_get_64(abfd, ptr)		BFD_SEND(abfd, bfd_h_getx64, \
							((bfd_byte *) ptr))

#define bfd_h_put_16(abfd, val, ptr)	BFD_SEND(abfd, bfd_h_putx16, \
							(val, (bfd_byte *) ptr))
#define bfd_h_get_16(abfd, ptr)		BFD_SEND(abfd, bfd_h_getx16, \
							((bfd_byte *) ptr))

/* General purpose one fits all.  The do { } while (0) makes a single 
   statement out of it, for use in things like nested if-statements.
   
   The idea is to create your external ref as a byte array of the
   right size eg:
   char foo[4];
   char bar[2];
   then you may do things like:
   bfd_h_put_x(abfd, 1, &foo);
   and bfd_h_get_x(abfd,& bar);
*/

#define bfd_h_put_x(abfd, val, ptr) \
  do {  \
       if (sizeof((ptr)) == LONGLONG_SIZE) \
		bfd_h_put_64  (abfd, val, (ptr));\
       if (sizeof((ptr)) == LONG_SIZE) \
		bfd_h_put_32  (abfd, val, (ptr));\
  else if (sizeof((ptr)) == SHORT_SIZE) \
		bfd_h_put_16 (abfd, val, (ptr));\
  else if (sizeof((ptr)) == BYTE_SIZE) \
		bfd_h_put_8  (abfd, val, (ptr));\
  else abort(); } while (0)

#define bfd_h_get_x(abfd, ptr) \
  ((sizeof((ptr))==LONGLONG_SIZE) ? bfd_h_get_64 (abfd, &(ptr[0])):\
  (sizeof((ptr))==LONG_SIZE) ?  bfd_h_get_32 (abfd, &(ptr[0])):\
   (sizeof((ptr))==SHORT_SIZE) ? bfd_h_get_16(abfd, &(ptr[0])):\
    bfd_h_get_8 (abfd, &(ptr[0])))
#ifdef GNU960

#define BFD_COFF_FORMAT	bfd_target_coff_flavour_enum
#define BFD_BOUT_FORMAT	bfd_target_aout_flavour_enum

/*
 * Return nonzero iff specified bfd is for big-endian target
 */
#define BFD_BIG_ENDIAN_FILE_P(abfd)	\
			((abfd)->xvec->header_byteorder_big_p == true)

/*
 * Return nonzero iff specified bfd is for coff target
 */
#define BFD_COFF_FILE_P(abfd)	((abfd)->xvec->flavour == BFD_COFF_FORMAT)

/*
 * The names of the only targets the GNU/960 release cares about
 */
#define BFD_BIG_COFF_TARG	"coff-Intel-big"
#define BFD_LITTLE_COFF_TARG	"coff-Intel-little"
#define BFD_BIG_BOUT_TARG	"b.out.big"
#define BFD_LITTLE_BOUT_TARG	"b.out.little"

extern PROTO (char *, bfd_make_targ_name,( enum target_flavour_enum format, int bigendian));

#endif /* GNU960 */

#endif /* __BFD_H_SEEN__ */
