/* bfd back-end for HP PA-RISC SOM objects.
   Copyright (C) 1990, 1991, 1992, 1993 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

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

#include "bfd.h"
#include "sysdep.h"

#if defined (HOST_HPPAHPUX) || defined (HOST_HPPABSD)

#include "libbfd.h"
#include "som.h"
#include "libhppa.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <machine/reg.h>
#include <sys/user.h>		/* After a.out.h  */
#include <sys/file.h>
#include <errno.h>

/* Magic not defined in standard HP-UX header files until 8.0 */

#ifndef CPU_PA_RISC1_0
#define CPU_PA_RISC1_0 0x20B
#endif /* CPU_PA_RISC1_0 */

#ifndef CPU_PA_RISC1_1
#define CPU_PA_RISC1_1 0x210
#endif /* CPU_PA_RISC1_1 */

#ifndef _PA_RISC1_0_ID
#define _PA_RISC1_0_ID CPU_PA_RISC1_0
#endif /* _PA_RISC1_0_ID */

#ifndef _PA_RISC1_1_ID
#define _PA_RISC1_1_ID CPU_PA_RISC1_1
#endif /* _PA_RISC1_1_ID */

#ifndef _PA_RISC_MAXID
#define _PA_RISC_MAXID	0x2FF
#endif /* _PA_RISC_MAXID */

#ifndef _PA_RISC_ID
#define _PA_RISC_ID(__m_num)		\
    (((__m_num) == _PA_RISC1_0_ID) ||	\
     ((__m_num) >= _PA_RISC1_1_ID && (__m_num) <= _PA_RISC_MAXID))
#endif /* _PA_RISC_ID */

/* Size (in chars) of the temporary buffers used during fixup and string
   table writes.   */
   
#define SOM_TMP_BUFSIZE 8192


/* SOM allows any one of the four previous relocations to be reused
   with a "R_PREV_FIXUP" relocation entry.  Since R_PREV_FIXUP
   relocations are always a single byte, using a R_PREV_FIXUP instead
   of some multi-byte relocation makes object files smaller. 

   Note one side effect of using a R_PREV_FIXUP is the relocation that
   is being repeated moves to the front of the queue.  */
struct reloc_queue
  {
    unsigned char *reloc;
    unsigned int size;
  } reloc_queue[4];

/* This fully describes the symbol types which may be attached to
   an EXPORT or IMPORT directive.  Only SOM uses this formation
   (ELF has no need for it).  */
typedef enum
{
  SYMBOL_TYPE_UNKNOWN,
  SYMBOL_TYPE_ABSOLUTE,
  SYMBOL_TYPE_CODE,
  SYMBOL_TYPE_DATA,
  SYMBOL_TYPE_ENTRY,
  SYMBOL_TYPE_MILLICODE,
  SYMBOL_TYPE_PLABEL,
  SYMBOL_TYPE_PRI_PROG,
  SYMBOL_TYPE_SEC_PROG,
} pa_symbol_type;

struct section_to_type
{
  char *section;
  char type;
};

/* Forward declarations */

static boolean som_mkobject PARAMS ((bfd *));
static bfd_target * som_object_setup PARAMS ((bfd *,
					      struct header *,
					      struct som_exec_auxhdr *));
static asection * make_unique_section PARAMS ((bfd *, CONST char *, int));
static boolean setup_sections PARAMS ((bfd *, struct header *));
static bfd_target * som_object_p PARAMS ((bfd *));
static boolean som_write_object_contents PARAMS ((bfd *));
static boolean som_slurp_string_table PARAMS ((bfd *));
static unsigned int som_slurp_symbol_table PARAMS ((bfd *));
static unsigned int som_get_symtab_upper_bound PARAMS ((bfd *));
static unsigned int som_canonicalize_reloc PARAMS ((bfd *, sec_ptr,
						    arelent **, asymbol **));
static unsigned int som_get_reloc_upper_bound PARAMS ((bfd *, sec_ptr));
static unsigned int som_set_reloc_info PARAMS ((unsigned char *, unsigned int,
						arelent *, asection *,
						asymbol **, boolean));
static boolean som_slurp_reloc_table PARAMS ((bfd *, asection *,
					      asymbol **, boolean));
static unsigned int som_get_symtab PARAMS ((bfd *, asymbol **));
static asymbol * som_make_empty_symbol PARAMS ((bfd *));
static void som_print_symbol PARAMS ((bfd *, PTR,
				      asymbol *, bfd_print_symbol_type));
static boolean som_new_section_hook PARAMS ((bfd *, asection *));
static boolean som_set_section_contents PARAMS ((bfd *, sec_ptr, PTR,
						 file_ptr, bfd_size_type));
static boolean som_set_arch_mach PARAMS ((bfd *, enum bfd_architecture,
					  unsigned long));
static boolean som_find_nearest_line PARAMS ((bfd *, asection *,
					      asymbol **, bfd_vma,
					      CONST char **,
					      CONST char **,
					      unsigned int *));
static void som_get_symbol_info PARAMS ((bfd *, asymbol *, symbol_info *));
static asection * som_section_from_subspace_index PARAMS ((bfd *, 
							   unsigned int));
static int log2 PARAMS ((unsigned int));
static bfd_reloc_status_type hppa_som_reloc PARAMS ((bfd *, arelent *,
						     asymbol *, PTR,
						     asection *, bfd *));
static void som_initialize_reloc_queue PARAMS ((struct reloc_queue *));
static void som_reloc_queue_insert PARAMS ((unsigned char *, unsigned int,
					    struct reloc_queue *));
static void som_reloc_queue_fix PARAMS ((struct reloc_queue *, unsigned int));
static int som_reloc_queue_find PARAMS ((unsigned char *, unsigned int,
					 struct reloc_queue *));
static unsigned char * try_prev_fixup PARAMS ((bfd *, int *, unsigned char *,
					       unsigned int,
					       struct reloc_queue *));

static unsigned char * som_reloc_skip PARAMS ((bfd *, unsigned int,
					       unsigned char *, unsigned int *,
					       struct reloc_queue *));
static unsigned char * som_reloc_addend PARAMS ((bfd *, int, unsigned char *,
					         unsigned int *,
						 struct reloc_queue *));
static unsigned char * som_reloc_call PARAMS ((bfd *, unsigned char *,
					       unsigned int *,
					       arelent *, int,
					       struct reloc_queue *));
static unsigned long som_count_spaces PARAMS ((bfd *));
static unsigned long som_count_subspaces PARAMS ((bfd *));
static int compare_syms PARAMS ((asymbol **, asymbol **));
static unsigned long som_compute_checksum PARAMS ((bfd *));
static boolean som_prep_headers PARAMS ((bfd *));
static int som_sizeof_headers PARAMS ((bfd *, boolean));
static boolean som_write_headers PARAMS ((bfd *));
static boolean som_build_and_write_symbol_table PARAMS ((bfd *));
static void som_prep_for_fixups PARAMS ((bfd *, asymbol **, unsigned long));
static boolean som_write_fixups PARAMS ((bfd *, unsigned long, unsigned int *));
static boolean som_write_space_strings PARAMS ((bfd *, unsigned long,
						unsigned int *));
static boolean som_write_symbol_strings PARAMS ((bfd *, unsigned long,
						 asymbol **, unsigned int,
						 unsigned *));
static boolean som_begin_writing PARAMS ((bfd *));
static const reloc_howto_type * som_bfd_reloc_type_lookup
	PARAMS ((bfd_arch_info_type *, bfd_reloc_code_real_type));
static char som_section_type PARAMS ((const char *));
static int som_decode_symclass PARAMS ((asymbol *));


/* Map SOM section names to POSIX/BSD single-character symbol types.

   This table includes all the standard subspaces as defined in the 
   current "PRO ABI for PA-RISC Systems", $UNWIND$ which for 
   some reason was left out, and sections specific to embedded stabs.  */

static const struct section_to_type stt[] = {
  {"$TEXT$", 't'},
  {"$SHLIB_INFO$", 't'},
  {"$MILLICODE$", 't'},
  {"$LIT$", 't'},
  {"$CODE$", 't'},
  {"$UNWIND_START$", 't'},
  {"$UNWIND$", 't'},
  {"$PRIVATE$", 'd'},
  {"$PLT$", 'd'},
  {"$SHLIB_DATA$", 'd'},
  {"$DATA$", 'd'},
  {"$SHORTDATA$", 'g'},
  {"$DLT$", 'd'},
  {"$GLOBAL$", 'g'},
  {"$SHORTBSS$", 's'},
  {"$BSS$", 'b'},
  {"$GDB_STRINGS$", 'N'},
  {"$GDB_SYMBOLS$", 'N'},
  {0, 0}
};

/* About the relocation formatting table...

   There are 256 entries in the table, one for each possible
   relocation opcode available in SOM.  We index the table by
   the relocation opcode.  The names and operations are those
   defined by a.out_800 (4).

   Right now this table is only used to count and perform minimal
   processing on relocation streams so that they can be internalized
   into BFD and symbolically printed by utilities.  To make actual use 
   of them would be much more difficult, BFD's concept of relocations
   is far too simple to handle SOM relocations.  The basic assumption
   that a relocation can be completely processed independent of other
   relocations before an object file is written is invalid for SOM.

   The SOM relocations are meant to be processed as a stream, they
   specify copying of data from the input section to the output section
   while possibly modifying the data in some manner.  They also can 
   specify that a variable number of zeros or uninitialized data be
   inserted on in the output segment at the current offset.  Some
   relocations specify that some previous relocation be re-applied at
   the current location in the input/output sections.  And finally a number
   of relocations have effects on other sections (R_ENTRY, R_EXIT,
   R_UNWIND_AUX and a variety of others).  There isn't even enough room
   in the BFD relocation data structure to store enough information to
   perform all the relocations.

   Each entry in the table has three fields. 

   The first entry is an index into this "class" of relocations.  This
   index can then be used as a variable within the relocation itself.

   The second field is a format string which actually controls processing
   of the relocation.  It uses a simple postfix machine to do calculations
   based on variables/constants found in the string and the relocation
   stream.  

   The third field specifys whether or not this relocation may use 
   a constant (V) from the previous R_DATA_OVERRIDE rather than a constant
   stored in the instruction.

   Variables:  
  
   L = input space byte count
   D = index into class of relocations
   M = output space byte count
   N = statement number (unused?)
   O = stack operation
   R = parameter relocation bits
   S = symbol index
   U = 64 bits of stack unwind and frame size info (we only keep 32 bits)
   V = a literal constant (usually used in the next relocation)
   P = a previous relocation
  
   Lower case letters (starting with 'b') refer to following 
   bytes in the relocation stream.  'b' is the next 1 byte,
   c is the next 2 bytes, d is the next 3 bytes, etc...  
   This is the variable part of the relocation entries that
   makes our life a living hell.

   numerical constants are also used in the format string.  Note
   the constants are represented in decimal. 

   '+', "*" and "=" represents the obvious postfix operators.
   '<' represents a left shift. 

   Stack Operations:

   Parameter Relocation Bits:

   Unwind Entries:  
   
   Previous Relocations:  The index field represents which in the queue
   of 4 previous fixups should be re-applied.

   Literal Constants:  These are generally used to represent addend
   parts of relocations when these constants are not stored in the
   fields of the instructions themselves.  For example the instruction
   addil foo-$global$-0x1234 would use an override for "0x1234" rather
   than storing it into the addil itself.  */

struct fixup_format
{
  int D;
  char *format;
};

static const struct fixup_format som_fixup_formats[256] =
{
  /* R_NO_RELOCATION */
  0,   "LD1+4*=",       /* 0x00 */
  1,   "LD1+4*=",	/* 0x01 */
  2,   "LD1+4*=",	/* 0x02 */
  3,   "LD1+4*=",	/* 0x03 */
  4,   "LD1+4*=",	/* 0x04 */
  5,   "LD1+4*=",	/* 0x05 */
  6,   "LD1+4*=",	/* 0x06 */
  7,   "LD1+4*=",	/* 0x07 */
  8,   "LD1+4*=",	/* 0x08 */
  9,   "LD1+4*=",	/* 0x09 */
  10,  "LD1+4*=",	/* 0x0a */
  11,  "LD1+4*=",	/* 0x0b */
  12,  "LD1+4*=",	/* 0x0c */
  13,  "LD1+4*=",	/* 0x0d */
  14,  "LD1+4*=",	/* 0x0e */
  15,  "LD1+4*=",	/* 0x0f */
  16,  "LD1+4*=",	/* 0x10 */
  17,  "LD1+4*=",	/* 0x11 */
  18,  "LD1+4*=",	/* 0x12 */
  19,  "LD1+4*=",	/* 0x13 */
  20,  "LD1+4*=",	/* 0x14 */
  21,  "LD1+4*=",	/* 0x15 */
  22,  "LD1+4*=",	/* 0x16 */
  23,  "LD1+4*=",	/* 0x17 */
  0,   "LD8<b+1+4*=",	/* 0x18 */
  1,   "LD8<b+1+4*=",	/* 0x19 */
  2,   "LD8<b+1+4*=",	/* 0x1a */
  3,   "LD8<b+1+4*=",	/* 0x1b */
  0,   "LD16<c+1+4*=",	/* 0x1c */
  1,   "LD16<c+1+4*=",	/* 0x1d */
  2,   "LD16<c+1+4*=",	/* 0x1e */
  0,   "Ld1+=",         /* 0x1f */
  /* R_ZEROES */
  0,    "Lb1+4*=",	/* 0x20 */
  1,    "Ld1+=",	/* 0x21 */
  /* R_UNINIT */
  0,    "Lb1+4*=",	/* 0x22 */
  1,    "Ld1+=",	/* 0x23 */
  /* R_RELOCATION */
  0,    "L4=",          /* 0x24 */
  /* R_DATA_ONE_SYMBOL */
  0,    "L4=Sb=",	/* 0x25 */
  1,    "L4=Sd=",	/* 0x26 */
  /* R_DATA_PLEBEL */
  0,    "L4=Sb=",	/* 0x27 */
  1,    "L4=Sd=",	/* 0x28 */
  /* R_SPACE_REF */
  0,    "L4=",          /* 0x29 */
  /* R_REPEATED_INIT */
  0,    "L4=Mb1+4*=",	/* 0x2a */
  1,    "Lb4*=Mb1+L*=",	/* 0x2b */
  2,    "Lb4*=Md1+4*=",	/* 0x2c */
  3,    "Ld1+=Me1+=",	/* 0x2d */
  /* R_RESERVED */
  0,   	"",	        /* 0x2e */
  0,   	"",	        /* 0x2f */
  /* R_PCREL_CALL */
  0,    "L4=RD=Sb=",	/* 0x30 */
  1,    "L4=RD=Sb=",	/* 0x31 */
  2,    "L4=RD=Sb=",	/* 0x32 */
  3,    "L4=RD=Sb=",	/* 0x33 */
  4,    "L4=RD=Sb=",	/* 0x34 */
  5,    "L4=RD=Sb=",	/* 0x35 */
  6,    "L4=RD=Sb=",	/* 0x36 */
  7,    "L4=RD=Sb=",	/* 0x37 */
  8,    "L4=RD=Sb=",	/* 0x38 */
  9,    "L4=RD=Sb=",	/* 0x39 */
  0,    "L4=RD8<b+=Sb=",/* 0x3a */
  1,    "L4=RD8<b+=Sb=",/* 0x3b */
  0,    "L4=RD8<b+=Sd=",/* 0x3c */
  1,    "L4=RD8<b+=Sd=",/* 0x3d */
  /* R_RESERVED */
  0,    "",	        /* 0x3e */
  0,    "",	        /* 0x3f */
  /* R_ABS_CALL */
  0,    "L4=RD=Sb=",	/* 0x40 */
  1,    "L4=RD=Sb=",	/* 0x41 */
  2,    "L4=RD=Sb=",	/* 0x42 */
  3,    "L4=RD=Sb=",	/* 0x43 */
  4,    "L4=RD=Sb=",	/* 0x44 */
  5,    "L4=RD=Sb=",	/* 0x45 */
  6,    "L4=RD=Sb=",	/* 0x46 */
  7,    "L4=RD=Sb=",	/* 0x47 */
  8,    "L4=RD=Sb=",	/* 0x48 */
  9,    "L4=RD=Sb=",	/* 0x49 */
  0,    "L4=RD8<b+=Sb=",/* 0x4a */
  1,    "L4=RD8<b+=Sb=",/* 0x4b */
  0,    "L4=RD8<b+=Sd=",/* 0x4c */
  1,    "L4=RD8<b+=Sd=",/* 0x4d */
  /* R_RESERVED */
  0,     "",	        /* 0x4e */
  0,     "",	        /* 0x4f */
  /* R_DP_RELATIVE */
  0,    "L4=SD=",	/* 0x50 */
  1,    "L4=SD=",	/* 0x51 */
  2,    "L4=SD=",	/* 0x52 */
  3,    "L4=SD=",	/* 0x53 */
  4,    "L4=SD=",	/* 0x54 */
  5,    "L4=SD=",	/* 0x55 */
  6,    "L4=SD=",	/* 0x56 */
  7,    "L4=SD=",	/* 0x57 */
  8,    "L4=SD=",	/* 0x58 */
  9,    "L4=SD=",	/* 0x59 */
  10,   "L4=SD=",	/* 0x5a */
  11,   "L4=SD=",	/* 0x5b */
  12,   "L4=SD=",	/* 0x5c */
  13,   "L4=SD=",	/* 0x5d */
  14,   "L4=SD=",	/* 0x5e */
  15,   "L4=SD=",	/* 0x5f */
  16,   "L4=SD=",	/* 0x60 */
  17,   "L4=SD=",	/* 0x61 */
  18,   "L4=SD=",	/* 0x62 */
  19,   "L4=SD=",	/* 0x63 */
  20,   "L4=SD=",	/* 0x64 */
  21,   "L4=SD=",	/* 0x65 */
  22,   "L4=SD=",	/* 0x66 */
  23,   "L4=SD=",	/* 0x67 */
  24,   "L4=SD=",	/* 0x68 */
  25,   "L4=SD=",	/* 0x69 */
  26,   "L4=SD=",	/* 0x6a */
  27,   "L4=SD=",	/* 0x6b */
  28,   "L4=SD=",	/* 0x6c */
  29,   "L4=SD=",	/* 0x6d */
  30,   "L4=SD=",	/* 0x6e */
  31,   "L4=SD=",	/* 0x6f */
  32,   "L4=Sb=",	/* 0x70 */
  33,   "L4=Sd=",	/* 0x71 */
  /* R_RESERVED */
  0,    "",	        /* 0x72 */
  0,    "",	        /* 0x73 */
  0,    "",	        /* 0x74 */
  0,    "",	        /* 0x75 */
  0,    "",	        /* 0x76 */
  0,    "",      	/* 0x77 */
  /* R_DLT_REL */
  0,    "L4=Sb=",	/* 0x78 */
  1,    "L4=Sd=",	/* 0x79 */
  /* R_RESERVED */
  0,    "",        	/* 0x7a */
  0,    "",	        /* 0x7b */
  0,    "",	        /* 0x7c */
  0,    "",	        /* 0x7d */
  0,    "",	        /* 0x7e */
  0,    "",	        /* 0x7f */
  /* R_CODE_ONE_SYMBOL */
  0,    "L4=SD=",	/* 0x80 */
  1,    "L4=SD=",	/* 0x81 */
  2,    "L4=SD=",	/* 0x82 */
  3,    "L4=SD=",	/* 0x83 */
  4,    "L4=SD=",	/* 0x84 */
  5,    "L4=SD=",	/* 0x85 */
  6,    "L4=SD=",	/* 0x86 */
  7,    "L4=SD=",	/* 0x87 */
  8,    "L4=SD=",	/* 0x88 */
  9,    "L4=SD=",	/* 0x89 */
  10,   "L4=SD=",	/* 0x8q */
  11,   "L4=SD=",	/* 0x8b */
  12,   "L4=SD=",	/* 0x8c */
  13,   "L4=SD=",	/* 0x8d */
  14,   "L4=SD=",	/* 0x8e */
  15,   "L4=SD=",	/* 0x8f */
  16,   "L4=SD=",	/* 0x90 */
  17,   "L4=SD=",	/* 0x91 */
  18,   "L4=SD=",	/* 0x92 */
  19,   "L4=SD=",	/* 0x93 */
  20,   "L4=SD=",	/* 0x94 */
  21,   "L4=SD=",	/* 0x95 */
  22,   "L4=SD=",	/* 0x96 */
  23,   "L4=SD=",	/* 0x97 */
  24,   "L4=SD=",	/* 0x98 */
  25,   "L4=SD=",	/* 0x99 */
  26,   "L4=SD=",	/* 0x9a */
  27,   "L4=SD=",	/* 0x9b */
  28,   "L4=SD=",	/* 0x9c */
  29,   "L4=SD=",	/* 0x9d */
  30,   "L4=SD=",	/* 0x9e */
  31,   "L4=SD=",	/* 0x9f */
  32,   "L4=Sb=",	/* 0xa0 */
  33,   "L4=Sd=",	/* 0xa1 */
  /* R_RESERVED */
  0,    "",	        /* 0xa2 */
  0,    "",	        /* 0xa3 */
  0,    "",	        /* 0xa4 */
  0,    "",	        /* 0xa5 */
  0,    "",	        /* 0xa6 */
  0,    "",	        /* 0xa7 */
  0,    "",	        /* 0xa8 */
  0,    "",	        /* 0xa9 */
  0,    "",	        /* 0xaa */
  0,    "",	        /* 0xab */
  0,    "",	        /* 0xac */
  0,    "",	        /* 0xad */
  /* R_MILLI_REL */
  0,    "L4=Sb=",	/* 0xae */
  1,    "L4=Sd=",	/* 0xaf */
  /* R_CODE_PLABEL */
  0,    "L4=Sb=",	/* 0xb0 */
  1,    "L4=Sd=",	/* 0xb1 */
  /* R_BREAKPOINT */
  0,    "L4=",	        /* 0xb2 */
  /* R_ENTRY */
  0,    "Ui=",	        /* 0xb3 */
  1,    "Uf=",	        /* 0xb4 */
  /* R_ALT_ENTRY */
  0,    "",	        /* 0xb5 */
  /* R_EXIT */
  0,    "",		/* 0xb6 */
  /* R_BEGIN_TRY */
  0,    "",	        /* 0xb7 */
  /* R_END_TRY */
  0,    "R0=",	        /* 0xb8 */
  1,    "Rb4*=",	/* 0xb9 */
  2,    "Rd4*=",	/* 0xba */
  /* R_BEGIN_BRTAB */
  0,    "",	        /* 0xbb */
  /* R_END_BRTAB */
  0,    "",	        /* 0xbc */
  /* R_STATEMENT */
  0,    "Nb=",	        /* 0xbd */
  1,    "Nc=",	        /* 0xbe */
  2,    "Nd=",	        /* 0xbf */
  /* R_DATA_EXPR */
  0,    "L4=",	        /* 0xc0 */
  /* R_CODE_EXPR */
  0,    "L4=",	        /* 0xc1 */
  /* R_FSEL */
  0,    "",		/* 0xc2 */
  /* R_LSEL */
  0,    "",		/* 0xc3 */
  /* R_RSEL */
  0,    "",		/* 0xc4 */
  /* R_N_MODE */
  0,    "",		/* 0xc5 */
  /* R_S_MODE */
  0,    "",		/* 0xc6 */
  /* R_D_MODE */
  0,    "",		/* 0xc7 */
  /* R_R_MODE */
  0,    "",		/* 0xc8 */
  /* R_DATA_OVERRIDE */
  0,    "V0=",	        /* 0xc9 */
  1,    "Vb=",	        /* 0xca */
  2,    "Vc=",	        /* 0xcb */
  3,    "Vd=",	        /* 0xcc */
  4,    "Ve=",	        /* 0xcd */
  /* R_TRANSLATED */
  0,    "",	        /* 0xce */
  /* R_RESERVED */
  0,    "",	        /* 0xcf */
  /* R_COMP1 */
  0,    "Ob=",	        /* 0xd0 */
  /* R_COMP2 */
  0,    "Ob=Sd=",	/* 0xd1 */
  /* R_COMP3 */
  0,    "Ob=Ve=",	/* 0xd2 */
  /* R_PREV_FIXUP */
  0,    "P",   	        /* 0xd3 */
  1,    "P",	        /* 0xd4 */
  2,    "P",	        /* 0xd5 */
  3,    "P",	        /* 0xd6 */
  /* R_RESERVED */
  0,	"",		/* 0xd7 */
  0,	"",		/* 0xd8 */
  0,	"",		/* 0xd9 */
  0,	"",		/* 0xda */
  0,	"",		/* 0xdb */
  0,	"",		/* 0xdc */
  0,	"",		/* 0xdd */
  0,	"",		/* 0xde */
  0,	"",		/* 0xdf */
  0,	"",		/* 0xe0 */
  0,	"",		/* 0xe1 */
  0,	"",		/* 0xe2 */
  0,	"",		/* 0xe3 */
  0,	"",		/* 0xe4 */
  0,	"",		/* 0xe5 */
  0,	"",		/* 0xe6 */
  0,	"",		/* 0xe7 */
  0,	"",		/* 0xe8 */
  0,	"",		/* 0xe9 */
  0,	"",		/* 0xea */
  0,	"",		/* 0xeb */
  0,	"",		/* 0xec */
  0,	"",		/* 0xed */
  0,	"",		/* 0xee */
  0,	"",		/* 0xef */
  0,	"",		/* 0xf0 */
  0,	"",		/* 0xf1 */
  0,	"",		/* 0xf2 */
  0,	"",		/* 0xf3 */
  0,	"",		/* 0xf4 */
  0,	"",		/* 0xf5 */
  0,	"",		/* 0xf6 */
  0,	"",		/* 0xf7 */
  0,	"",		/* 0xf8 */
  0,	"",		/* 0xf9 */
  0,	"",		/* 0xfa */
  0,	"",		/* 0xfb */
  0,	"",		/* 0xfc */
  0,	"",		/* 0xfd */
  0,	"",		/* 0xfe */
  0,	"",		/* 0xff */
};

static const int comp1_opcodes[] =
{
  0x00,
  0x40,
  0x41,
  0x42,
  0x43,
  0x44,
  0x45,
  0x46,
  0x47,
  0x48,
  0x49,
  0x4a,
  0x4b,
  0x60,
  0x80,
  0xa0,
  0xc0,
  -1
};

static const int comp2_opcodes[] =
{
  0x00,
  0x80,
  0x82,
  0xc0,
  -1
};

static const int comp3_opcodes[] =
{
  0x00,
  0x02,
  -1
};

/* These apparently are not in older versions of hpux reloc.h.  */
#ifndef R_DLT_REL
#define R_DLT_REL 0x78
#endif

#ifndef R_AUX_UNWIND
#define R_AUX_UNWIND 0xcf
#endif

#ifndef R_SEC_STMT
#define R_SEC_STMT 0xd7
#endif

static reloc_howto_type som_hppa_howto_table[] =
{
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_NO_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_NO_RELOCATION"},
  {R_ZEROES, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ZEROES"},
  {R_ZEROES, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ZEROES"},
  {R_UNINIT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_UNINIT"},
  {R_UNINIT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_UNINIT"},
  {R_RELOCATION, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RELOCATION"},
  {R_DATA_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_ONE_SYMBOL"},
  {R_DATA_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_ONE_SYMBOL"},
  {R_DATA_PLABEL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_PLABEL"},
  {R_DATA_PLABEL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_PLABEL"},
  {R_SPACE_REF, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_SPACE_REF"},
  {R_REPEATED_INIT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "REPEATED_INIT"},
  {R_REPEATED_INIT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "REPEATED_INIT"},
  {R_REPEATED_INIT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "REPEATED_INIT"},
  {R_REPEATED_INIT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "REPEATED_INIT"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_PCREL_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PCREL_CALL"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_ABS_CALL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ABS_CALL"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_DP_RELATIVE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DP_RELATIVE"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_DLT_REL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DLT_REL"},
  {R_DLT_REL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DLT_REL"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_CODE_ONE_SYMBOL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_ONE_SYMBOL"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_MILLI_REL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_MILLI_REL"},
  {R_MILLI_REL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_MILLI_REL"},
  {R_CODE_PLABEL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_PLABEL"},
  {R_CODE_PLABEL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_PLABEL"},
  {R_BREAKPOINT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_BREAKPOINT"},
  {R_ENTRY, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ENTRY"},
  {R_ENTRY, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ENTRY"},
  {R_ALT_ENTRY, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_ALT_ENTRY"},
  {R_EXIT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_EXIT"},
  {R_BEGIN_TRY, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_BEGIN_TRY"},
  {R_END_TRY, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_END_TRY"},
  {R_END_TRY, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_END_TRY"},
  {R_END_TRY, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_END_TRY"},
  {R_BEGIN_BRTAB, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_BEGIN_BRTAB"},
  {R_END_BRTAB, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_END_BRTAB"},
  {R_STATEMENT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_STATEMENT"},
  {R_STATEMENT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_STATEMENT"},
  {R_STATEMENT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_STATEMENT"},
  {R_DATA_EXPR, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_EXPR"},
  {R_CODE_EXPR, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_CODE_EXPR"},
  {R_FSEL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_FSEL"},
  {R_LSEL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_LSEL"},
  {R_RSEL, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RSEL"},
  {R_N_MODE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_N_MODE"},
  {R_S_MODE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_S_MODE"},
  {R_D_MODE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_D_MODE"},
  {R_R_MODE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_R_MODE"},
  {R_DATA_OVERRIDE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_OVERRIDE"},
  {R_DATA_OVERRIDE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_OVERRIDE"},
  {R_DATA_OVERRIDE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_OVERRIDE"},
  {R_DATA_OVERRIDE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_OVERRIDE"},
  {R_DATA_OVERRIDE, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_DATA_OVERRIDE"},
  {R_TRANSLATED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_TRANSLATED"},
  {R_AUX_UNWIND, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_AUX_UNWIND"},
  {R_COMP1, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_COMP1"},
  {R_COMP2, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_COMP2"},
  {R_COMP3, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_COMP3"},
  {R_PREV_FIXUP, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PREV_FIXUP"},
  {R_PREV_FIXUP, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PREV_FIXUP"},
  {R_PREV_FIXUP, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PREV_FIXUP"},
  {R_PREV_FIXUP, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_PREV_FIXUP"},
  {R_SEC_STMT, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_SEC_STMT"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"},
  {R_RESERVED, 0, 0, 32, false, 0, 0, hppa_som_reloc, "R_RESERVED"}};
  

/* Initialize the SOM relocation queue.  By definition the queue holds
   the last four multibyte fixups.  */
  
static void
som_initialize_reloc_queue (queue)
     struct reloc_queue *queue;
{
  queue[0].reloc = NULL;
  queue[0].size = 0;
  queue[1].reloc = NULL;
  queue[1].size = 0;
  queue[2].reloc = NULL;
  queue[2].size = 0;
  queue[3].reloc = NULL;
  queue[3].size = 0;
}

/* Insert a new relocation into the relocation queue.  */

static void
som_reloc_queue_insert (p, size, queue)
     unsigned char *p;
     unsigned int size;
     struct reloc_queue *queue;
{
  queue[3].reloc = queue[2].reloc;
  queue[3].size = queue[2].size;
  queue[2].reloc = queue[1].reloc;
  queue[2].size = queue[1].size;
  queue[1].reloc = queue[0].reloc;
  queue[1].size = queue[0].size;
  queue[0].reloc = p;
  queue[0].size = size;
}

/* When an entry in the relocation queue is reused, the entry moves
   to the front of the queue.  */

static void
som_reloc_queue_fix (queue, index)
     struct reloc_queue *queue;
     unsigned int index;
{
  if (index == 0)
    return;

  if (index == 1)
    {
      unsigned char *tmp1 = queue[0].reloc;
      unsigned int tmp2 = queue[0].size;
      queue[0].reloc = queue[1].reloc;
      queue[0].size = queue[1].size;
      queue[1].reloc = tmp1;
      queue[1].size = tmp2;
      return;
    }

  if (index == 2)
    {
      unsigned char *tmp1 = queue[0].reloc;
      unsigned int tmp2 = queue[0].size;
      queue[0].reloc = queue[2].reloc;
      queue[0].size = queue[2].size;
      queue[2].reloc = queue[1].reloc;
      queue[2].size = queue[1].size;
      queue[1].reloc = tmp1;
      queue[1].size = tmp2;
      return;
    }

  if (index == 3)
    {
      unsigned char *tmp1 = queue[0].reloc;
      unsigned int tmp2 = queue[0].size;
      queue[0].reloc = queue[3].reloc;
      queue[0].size = queue[3].size;
      queue[3].reloc = queue[2].reloc;
      queue[3].size = queue[2].size;
      queue[2].reloc = queue[1].reloc;
      queue[2].size = queue[1].size;
      queue[1].reloc = tmp1;
      queue[1].size = tmp2;
      return;
    }
  abort();
}

/* Search for a particular relocation in the relocation queue.  */

static int
som_reloc_queue_find (p, size, queue)
     unsigned char *p;
     unsigned int size;
     struct reloc_queue *queue;
{
  if (!bcmp (p, queue[0].reloc, size)
      && size == queue[0].size)
    return 0;
  if (!bcmp (p, queue[1].reloc, size)
      && size == queue[1].size)
    return 1;
  if (!bcmp (p, queue[2].reloc, size)
      && size == queue[2].size)
    return 2;
  if (!bcmp (p, queue[3].reloc, size)
      && size == queue[3].size)
    return 3;
  return -1;
}

static unsigned char *
try_prev_fixup (abfd, subspace_reloc_sizep, p, size, queue)
     bfd *abfd;
     int *subspace_reloc_sizep;
     unsigned char *p;
     unsigned int size;
     struct reloc_queue *queue;
{
  int queue_index = som_reloc_queue_find (p, size, queue);

  if (queue_index != -1)
    {
      /* Found this in a previous fixup.  Undo the fixup we
	 just built and use R_PREV_FIXUP instead.  We saved 
	 a total of size - 1 bytes in the fixup stream.  */
      bfd_put_8 (abfd, R_PREV_FIXUP + queue_index, p);
      p += 1;
      *subspace_reloc_sizep += 1;
      som_reloc_queue_fix (queue, queue_index);
    }
  else
    {
      som_reloc_queue_insert (p, size, queue);
      *subspace_reloc_sizep += size;
      p += size;
    }
  return p;
}

/* Emit the proper R_NO_RELOCATION fixups to map the next SKIP
   bytes without any relocation.  Update the size of the subspace
   relocation stream via SUBSPACE_RELOC_SIZE_P; also return the 
   current pointer into the relocation stream.  */

static unsigned char *
som_reloc_skip (abfd, skip, p, subspace_reloc_sizep, queue)
     bfd *abfd;
     unsigned int skip;
     unsigned char *p;
     unsigned int *subspace_reloc_sizep;
     struct reloc_queue *queue;
{
  /* Use a 4 byte R_NO_RELOCATION entry with a maximal value
     then R_PREV_FIXUPs to get the difference down to a
     reasonable size.  */
  if (skip >= 0x1000000)
    {
      skip -= 0x1000000;
      bfd_put_8 (abfd, R_NO_RELOCATION + 31, p);
      bfd_put_8 (abfd, 0xff, p + 1);
      bfd_put_16 (abfd, 0xffff, p + 2);
      p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 4, queue);
      while (skip >= 0x1000000)
	{
	  skip -= 0x1000000;
	  bfd_put_8 (abfd, R_PREV_FIXUP, p);
	  p++;
	  *subspace_reloc_sizep += 1;
	  /* No need to adjust queue here since we are repeating the
	     most recent fixup.  */
	}
    }
  
  /* The difference must be less than 0x1000000.  Use one 
     more R_NO_RELOCATION entry to get to the right difference.  */
  if ((skip & 3) == 0 && skip <= 0xc0000 && skip > 0)
    {
      /* Difference can be handled in a simple single-byte
	 R_NO_RELOCATION entry.  */
      if (skip <= 0x60)
	{
	  bfd_put_8 (abfd, R_NO_RELOCATION + (skip >> 2) - 1, p);
	  *subspace_reloc_sizep += 1;
	  p++;
	}
      /* Handle it with a two byte R_NO_RELOCATION entry.  */
      else if (skip <= 0x1000)
	{
	  bfd_put_8 (abfd, R_NO_RELOCATION + 24 + (((skip >> 2) - 1) >> 8), p);
	  bfd_put_8 (abfd, (skip >> 2) - 1, p + 1);
	  p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 2, queue);
	}
      /* Handle it with a three byte R_NO_RELOCATION entry.  */
      else
	{
	  bfd_put_8 (abfd, R_NO_RELOCATION + 28 + (((skip >> 2) - 1) >> 16), p);
	  bfd_put_16 (abfd, (skip >> 2) - 1, p + 1);
	  p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 3, queue);
	}
    }
  /* Ugh.  Punt and use a 4 byte entry.  */
  else if (skip > 0)
    {
      bfd_put_8 (abfd, R_NO_RELOCATION + 31, p);
      bfd_put_8 (abfd, skip >> 16, p + 1);
      bfd_put_16 (abfd, skip, p + 2);
      p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 4, queue);
    }
  return p;
}

/* Emit the proper R_DATA_OVERRIDE fixups to handle a nonzero addend
   from a BFD relocation.  Update the size of the subspace relocation
   stream via SUBSPACE_RELOC_SIZE_P; also return the current pointer
   into the relocation stream.  */

static unsigned char *
som_reloc_addend (abfd, addend, p, subspace_reloc_sizep, queue)
     bfd *abfd;
     int addend;
     unsigned char *p;
     unsigned int *subspace_reloc_sizep;
     struct reloc_queue *queue;
{
  if ((unsigned)(addend) + 0x80 < 0x100)
    {
      bfd_put_8 (abfd, R_DATA_OVERRIDE + 1, p);
      bfd_put_8 (abfd, addend, p + 1);
      p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 2, queue); 
    }
  else if ((unsigned) (addend) + 0x8000 < 0x10000)
    {
      bfd_put_8 (abfd, R_DATA_OVERRIDE + 2, p);
      bfd_put_16 (abfd, addend, p + 1);
      p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 3, queue);
    }
  else if ((unsigned) (addend) + 0x800000 < 0x1000000)
    {
      bfd_put_8 (abfd, R_DATA_OVERRIDE + 3, p);
      bfd_put_8 (abfd, addend >> 16, p + 1);
      bfd_put_16 (abfd, addend, p + 2);
      p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 4, queue);
    }
  else
    {
      bfd_put_8 (abfd, R_DATA_OVERRIDE + 4, p);
      bfd_put_32 (abfd, addend, p + 1);
      p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 5, queue);
    }
  return p;
}

/* Handle a single function call relocation.  */

static unsigned char *
som_reloc_call (abfd, p, subspace_reloc_sizep, bfd_reloc, sym_num, queue)
     bfd *abfd;
     unsigned char *p;
     unsigned int *subspace_reloc_sizep;
     arelent *bfd_reloc;
     int sym_num;
     struct reloc_queue *queue;
{
  int arg_bits = HPPA_R_ARG_RELOC (bfd_reloc->addend);
  int rtn_bits = arg_bits & 0x3;
  int type, done = 0;
  
  /* You'll never believe all this is necessary to handle relocations
     for function calls.  Having to compute and pack the argument
     relocation bits is the real nightmare.
     
     If you're interested in how this works, just forget it.  You really
     do not want to know about this braindamage.  */

  /* First see if this can be done with a "simple" relocation.  Simple
     relocations have a symbol number < 0x100 and have simple encodings
     of argument relocations.  */

  if (sym_num < 0x100)
    {
      switch (arg_bits)
	{
	case 0:
	case 1:
	  type = 0;
	  break;
	case 1 << 8:
	case 1 << 8 | 1:
	  type = 1;
	  break;
	case 1 << 8 | 1 << 6:
	case 1 << 8 | 1 << 6 | 1:
	  type = 2;
	  break;
	case 1 << 8 | 1 << 6 | 1 << 4:
	case 1 << 8 | 1 << 6 | 1 << 4 | 1:
	  type = 3;
	  break;
	case 1 << 8 | 1 << 6 | 1 << 4 | 1 << 2:
	case 1 << 8 | 1 << 6 | 1 << 4 | 1 << 2 | 1:
	  type = 4;
	  break;
	default:
	  /* Not one of the easy encodings.  This will have to be
	     handled by the more complex code below.  */
	  type = -1;
	  break;
	}
      if (type != -1)
	{
	  /* Account for the return value too.  */
	  if (rtn_bits)
	    type += 5;

	  /* Emit a 2 byte relocation.  Then see if it can be handled
	     with a relocation which is already in the relocation queue.  */
	  bfd_put_8 (abfd, bfd_reloc->howto->type + type, p);
	  bfd_put_8 (abfd, sym_num, p + 1);
	  p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 2, queue);
	  done = 1;
	}
    }
  
  /* If this could not be handled with a simple relocation, then do a hard
     one.  Hard relocations occur if the symbol number was too high or if
     the encoding of argument relocation bits is too complex.  */
  if (! done)
    {
      /* Don't ask about these magic sequences.  I took them straight
	 from gas-1.36 which took them from the a.out man page.  */
      type = rtn_bits;
      if ((arg_bits >> 6 & 0xf) == 0xe)
	type += 9 * 40;
      else
	type += (3 * (arg_bits >> 8 & 3) + (arg_bits >> 6 & 3)) * 40;
      if ((arg_bits >> 2 & 0xf) == 0xe)
	type += 9 * 4;
      else
	type += (3 * (arg_bits >> 4 & 3) + (arg_bits >> 2 & 3)) * 4;
      
      /* Output the first two bytes of the relocation.  These describe
	 the length of the relocation and encoding style.  */
      bfd_put_8 (abfd, bfd_reloc->howto->type + 10
		 + 2 * (sym_num >= 0x100) + (type >= 0x100),
		 p);
      bfd_put_8 (abfd, type, p + 1);
      
      /* Now output the symbol index and see if this bizarre relocation
	 just happened to be in the relocation queue.  */
      if (sym_num < 0x100)
	{
	  bfd_put_8 (abfd, sym_num, p + 2);
	  p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 3, queue);
	}
      else
	{
	  bfd_put_8 (abfd, sym_num >> 16, p + 2);
	  bfd_put_16 (abfd, sym_num, p + 3);
	  p = try_prev_fixup (abfd, subspace_reloc_sizep, p, 5, queue);
	}
    }
  return p;
}


/* Return the logarithm of X, base 2, considering X unsigned. 
   Abort if X is not a power of two -- this should never happen (FIXME:
   It will happen on corrupt executables.  GDB should give an error, not
   a coredump, in that case).  */

static int
log2 (x)
     unsigned int x;
{
  int log = 0;

  /* Test for 0 or a power of 2.  */
  if (x == 0 || x != (x & -x))
    abort();

  while ((x >>= 1) != 0)
    log++;
  return log;
}

static bfd_reloc_status_type
hppa_som_reloc (abfd, reloc_entry, symbol_in, data, input_section, output_bfd)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol_in;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
{
  if (output_bfd)
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }
  return bfd_reloc_ok;
}

/* Given a generic HPPA relocation type, the instruction format,
   and a field selector, return an appropriate SOM reloation.

   FIXME.  Need to handle %RR, %LR and the like as field selectors.
   These will need to generate multiple SOM relocations.  */ 

int **
hppa_som_gen_reloc_type (abfd, base_type, format, field)
     bfd *abfd;
     int base_type;
     int format;
     enum hppa_reloc_field_selector_type field;
{
  int *final_type, **final_types;

  final_types = (int **) bfd_alloc_by_size_t (abfd, sizeof (int *) * 3);
  final_type = (int *) bfd_alloc_by_size_t (abfd, sizeof (int));

  /* The field selector may require additional relocations to be 
     generated.  It's impossible to know at this moment if additional
     relocations will be needed, so we make them.  The code to actually
     write the relocation/fixup stream is responsible for removing
     any redundant relocations.  */
  switch (field)
    {
      case e_fsel:
      case e_psel:
      case e_lpsel:
      case e_rpsel:
      case e_tsel:
      case e_ltsel:
      case e_rtsel:
	final_types[0] = final_type;
	final_types[1] = NULL;
	final_types[2] = NULL;
	*final_type = base_type;
	break;

      case e_lssel:
      case e_rssel:
	final_types[0] = (int *) bfd_alloc_by_size_t (abfd, sizeof (int));
	*final_types[0] = R_S_MODE;
	final_types[1] = final_type;
	final_types[2] = NULL;
	*final_type = base_type;
	break;

      case e_lsel:
      case e_rsel:
	final_types[0] = (int *) bfd_alloc_by_size_t (abfd, sizeof (int));
	*final_types[0] = R_N_MODE;
	final_types[1] = final_type;
	final_types[2] = NULL;
	*final_type = base_type;
	break;

      case e_ldsel:
      case e_rdsel:
	final_types[0] = (int *) bfd_alloc_by_size_t (abfd, sizeof (int));
	*final_types[0] = R_D_MODE;
	final_types[1] = final_type;
	final_types[2] = NULL;
	*final_type = base_type;
	break;

      case e_lrsel:
      case e_rrsel:
	final_types[0] = (int *) bfd_alloc_by_size_t (abfd, sizeof (int));
	*final_types[0] = R_R_MODE;
	final_types[1] = final_type;
	final_types[2] = NULL;
	*final_type = base_type;
	break;
    }
  
  switch (base_type)
    {
    case R_HPPA:
      /* PLABELs get their own relocation type.  */
      if (field == e_psel
	  || field == e_lpsel
	  || field == e_rpsel)
	  {
	    /* A PLABEL relocation that has a size of 32 bits must
	       be a R_DATA_PLABEL.  All others are R_CODE_PLABELs.  */
	    if (format == 32)
	      *final_type = R_DATA_PLABEL;
	    else
	      *final_type = R_CODE_PLABEL;
	  }
      /* A relocatoin in the data space is always a full 32bits.  */
      else if (format == 32)
	*final_type = R_DATA_ONE_SYMBOL;

      break;

    case R_HPPA_GOTOFF:
      /* More PLABEL special cases.  */
      if (field == e_psel
	  || field == e_lpsel
	  || field == e_rpsel)
	*final_type = R_DATA_PLABEL;
      break;

    case R_HPPA_NONE:
    case R_HPPA_ABS_CALL:
    case R_HPPA_PCREL_CALL:
    case R_HPPA_COMPLEX:
    case R_HPPA_COMPLEX_PCREL_CALL:
    case R_HPPA_COMPLEX_ABS_CALL:
      /* Right now we can default all these.  */
      break;
    }
  return final_types;
}

/* Return the address of the correct entry in the PA SOM relocation
   howto table.  */

static const reloc_howto_type *
som_bfd_reloc_type_lookup (arch, code)
     bfd_arch_info_type *arch;
     bfd_reloc_code_real_type code;
{
  if ((int) code < (int) R_NO_RELOCATION + 255)
    {
      BFD_ASSERT ((int) som_hppa_howto_table[(int) code].type == (int) code);
      return &som_hppa_howto_table[(int) code];
    }

  return (reloc_howto_type *) 0;
}

/* Perform some initialization for an object.  Save results of this
   initialization in the BFD.  */

static bfd_target *
som_object_setup (abfd, file_hdrp, aux_hdrp)
     bfd *abfd;
     struct header *file_hdrp;
     struct som_exec_auxhdr *aux_hdrp;
{
  /* som_mkobject will set bfd_error if som_mkobject fails.  */
  if (som_mkobject (abfd) != true)
    return 0;

  /* Set BFD flags based on what information is available in the SOM.  */
  abfd->flags = NO_FLAGS;
  if (! file_hdrp->entry_offset)
    abfd->flags |= HAS_RELOC;
  else
    abfd->flags |= EXEC_P;
  if (file_hdrp->symbol_total)
    abfd->flags |= HAS_LINENO | HAS_DEBUG | HAS_SYMS | HAS_LOCALS;

  bfd_get_start_address (abfd) = aux_hdrp->exec_entry;
  bfd_default_set_arch_mach (abfd, bfd_arch_hppa, 0);
  bfd_get_symcount (abfd) = file_hdrp->symbol_total;

  /* Initialize the saved symbol table and string table to NULL.  
     Save important offsets and sizes from the SOM header into
     the BFD.  */
  obj_som_stringtab (abfd) = (char  *) NULL;
  obj_som_symtab (abfd) = (som_symbol_type *) NULL;
  obj_som_stringtab_size (abfd) = file_hdrp->symbol_strings_size;
  obj_som_sym_filepos (abfd) = file_hdrp->symbol_location;
  obj_som_str_filepos (abfd) = file_hdrp->symbol_strings_location;
  obj_som_reloc_filepos (abfd) = file_hdrp->fixup_request_location;

  return abfd->xvec;
}

/* Create a new BFD section for NAME.  If NAME already exists, then create a
   new unique name, with NAME as the prefix.  This exists because SOM .o files
   may have more than one $CODE$ subspace.  */

static asection *
make_unique_section (abfd, name, num)
     bfd *abfd;
     CONST char *name;
     int num;
{
  asection *sect;
  char *newname;
  char altname[100];

  sect = bfd_make_section (abfd, name);
  while (!sect)
    {
      sprintf (altname, "%s-%d", name, num++);
      sect = bfd_make_section (abfd, altname);
    }

  newname = bfd_alloc (abfd, strlen (sect->name) + 1);
  strcpy (newname, sect->name);

  sect->name = newname;
  return sect;
}

/* Convert all of the space and subspace info into BFD sections.  Each space
   contains a number of subspaces, which in turn describe the mapping between
   regions of the exec file, and the address space that the program runs in.
   BFD sections which correspond to spaces will overlap the sections for the
   associated subspaces.  */

static boolean
setup_sections (abfd, file_hdr)
     bfd *abfd;
     struct header *file_hdr;
{
  char *space_strings;
  int space_index;
  unsigned int total_subspaces = 0;

  /* First, read in space names */

  space_strings = alloca (file_hdr->space_strings_size);
  if (!space_strings)
    return false;

  if (bfd_seek (abfd, file_hdr->space_strings_location, SEEK_SET) < 0)
    return false;
  if (bfd_read (space_strings, 1, file_hdr->space_strings_size, abfd)
      != file_hdr->space_strings_size)
    return false;

  /* Loop over all of the space dictionaries, building up sections */
  for (space_index = 0; space_index < file_hdr->space_total; space_index++)
    {
      struct space_dictionary_record space;
      struct subspace_dictionary_record subspace, save_subspace;
      int subspace_index;
      asection *space_asect;

      /* Read the space dictionary element */
      if (bfd_seek (abfd, file_hdr->space_location
		    + space_index * sizeof space, SEEK_SET) < 0)
	return false;
      if (bfd_read (&space, 1, sizeof space, abfd) != sizeof space)
	return false;

      /* Setup the space name string */
      space.name.n_name = space.name.n_strx + space_strings;

      /* Make a section out of it */
      space_asect = make_unique_section (abfd, space.name.n_name, space_index);
      if (!space_asect)
	return false;

      /* Now, read in the first subspace for this space */
      if (bfd_seek (abfd, file_hdr->subspace_location
		    + space.subspace_index * sizeof subspace,
		    SEEK_SET) < 0)
	return false;
      if (bfd_read (&subspace, 1, sizeof subspace, abfd) != sizeof subspace)
	return false;
      /* Seek back to the start of the subspaces for loop below */
      if (bfd_seek (abfd, file_hdr->subspace_location
		    + space.subspace_index * sizeof subspace,
		    SEEK_SET) < 0)
	return false;

      /* Setup the start address and file loc from the first subspace record */
      space_asect->vma = subspace.subspace_start;
      space_asect->filepos = subspace.file_loc_init_value;
      space_asect->alignment_power = log2 (subspace.alignment);

      /* Initialize save_subspace so we can reliably determine if this
	 loop placed any useful values into it.  */
      bzero (&save_subspace, sizeof (struct subspace_dictionary_record));

      /* Loop over the rest of the subspaces, building up more sections */
      for (subspace_index = 0; subspace_index < space.subspace_quantity;
	   subspace_index++)
	{
	  asection *subspace_asect;

	  /* Read in the next subspace */
	  if (bfd_read (&subspace, 1, sizeof subspace, abfd)
	      != sizeof subspace)
	    return false;

	  /* Setup the subspace name string */
	  subspace.name.n_name = subspace.name.n_strx + space_strings;

	  /* Make a section out of this subspace */
	  subspace_asect = make_unique_section (abfd, subspace.name.n_name,
				     space.subspace_index + subspace_index);

	  if (!subspace_asect)
	    return false;

	  /* Keep an easy mapping between subspaces and sections.  */
	  som_section_data (subspace_asect)->subspace_index 
	    = total_subspaces++;

	  /* Set SEC_READONLY and SEC_CODE/SEC_DATA as specified
	     by the access_control_bits in the subspace header.  */
	  switch (subspace.access_control_bits >> 4)
	    {
	    /* Readonly data.  */  
	    case 0x0:
	      subspace_asect->flags |= SEC_DATA | SEC_READONLY;
	      break;

	    /* Normal data.  */  
	    case 0x1:
	      subspace_asect->flags |= SEC_DATA;
	      break;

	    /* Readonly code and the gateways.
	       Gateways have other attributes which do not map
	       into anything BFD knows about.  */
	    case 0x2:
	    case 0x4:
	    case 0x5:
	    case 0x6:
	    case 0x7:
	      subspace_asect->flags |= SEC_CODE | SEC_READONLY;
	      break;

	    /* dynamic (writable) code.  */
	    case 0x3:
	      subspace_asect->flags |= SEC_CODE;
	      break;
	    }
	  
	  if (subspace.dup_common || subspace.is_common) 
	    subspace_asect->flags |= SEC_IS_COMMON;
	  else if (subspace.subspace_length > 0)
	    subspace_asect->flags |= SEC_HAS_CONTENTS;
	  if (subspace.is_loadable)
	    subspace_asect->flags |= SEC_ALLOC | SEC_LOAD;
	  if (subspace.code_only)
	    subspace_asect->flags |= SEC_CODE;

	  /* Both file_loc_init_value and initialization_length will
	     be zero for a BSS like subspace.  */
	  if (subspace.file_loc_init_value == 0
	      && subspace.initialization_length == 0)
	    subspace_asect->flags &= ~(SEC_DATA | SEC_LOAD);

	  /* This subspace has relocations.
	     The fixup_request_quantity is a byte count for the number of
	     entries in the relocation stream; it is not the actual number
	     of relocations in the subspace.  */
	  if (subspace.fixup_request_quantity != 0)
	    {
	      subspace_asect->flags |= SEC_RELOC;
	      subspace_asect->rel_filepos = subspace.fixup_request_index;
	      som_section_data (subspace_asect)->reloc_size
		= subspace.fixup_request_quantity;
	      /* We can not determine this yet.  When we read in the 
		 relocation table the correct value will be filled in.  */
	      subspace_asect->reloc_count = -1;
	    }

	  /* Update save_subspace if appropriate.  */
	  if (subspace.file_loc_init_value > save_subspace.file_loc_init_value)
	    save_subspace = subspace;

	  subspace_asect->vma = subspace.subspace_start;
	  subspace_asect->_cooked_size = subspace.subspace_length;
	  subspace_asect->_raw_size = subspace.subspace_length;
	  subspace_asect->alignment_power = log2 (subspace.alignment);
	  subspace_asect->filepos = subspace.file_loc_init_value;
	}

      /* Yow! there is no subspace within the space which actually 
         has initialized information in it; this should never happen
         as far as I know.  */
      if (!save_subspace.file_loc_init_value)
	abort ();

      /* Setup the sizes for the space section based upon the info in the
         last subspace of the space.  */
      space_asect->_cooked_size = save_subspace.subspace_start
	- space_asect->vma + save_subspace.subspace_length;
      space_asect->_raw_size = save_subspace.file_loc_init_value
	- space_asect->filepos + save_subspace.initialization_length;
    }
  return true;
}

/* Read in a SOM object and make it into a BFD.  */

static bfd_target *
som_object_p (abfd)
     bfd *abfd;
{
  struct header file_hdr;
  struct som_exec_auxhdr aux_hdr;

  if (bfd_read ((PTR) & file_hdr, 1, FILE_HDR_SIZE, abfd) != FILE_HDR_SIZE)
    {
      bfd_error = system_call_error;
      return 0;
    }

  if (!_PA_RISC_ID (file_hdr.system_id))
    {
      bfd_error = wrong_format;
      return 0;
    }

  switch (file_hdr.a_magic)
    {
    case RELOC_MAGIC:
    case EXEC_MAGIC:
    case SHARE_MAGIC:
    case DEMAND_MAGIC:
#ifdef DL_MAGIC
    case DL_MAGIC:
#endif
#ifdef SHL_MAGIC
    case SHL_MAGIC:
#endif
#ifdef EXECLIBMAGIC
    case EXECLIBMAGIC:
#endif
#ifdef SHARED_MAGIC_CNX
    case SHARED_MAGIC_CNX:
#endif
      break;
    default:
      bfd_error = wrong_format;
      return 0;
    }

  if (file_hdr.version_id != VERSION_ID
      && file_hdr.version_id != NEW_VERSION_ID)
    {
      bfd_error = wrong_format;
      return 0;
    }

  /* If the aux_header_size field in the file header is zero, then this
     object is an incomplete executable (a .o file).  Do not try to read
     a non-existant auxiliary header.  */
  bzero (&aux_hdr, sizeof (struct som_exec_auxhdr));
  if (file_hdr.aux_header_size != 0)
    {
      if (bfd_read ((PTR) & aux_hdr, 1, AUX_HDR_SIZE, abfd) != AUX_HDR_SIZE)
	{
	  bfd_error = wrong_format;
	  return 0;
	}
    }

  if (!setup_sections (abfd, &file_hdr))
    {
      /* setup_sections does not bubble up a bfd error code.  */
      bfd_error = bad_value;
      return 0;
    }

  /* This appears to be a valid SOM object.  Do some initialization.  */
  return som_object_setup (abfd, &file_hdr, &aux_hdr);
}

/* Create a SOM object.  */

static boolean
som_mkobject (abfd)
     bfd *abfd;
{
  /* Allocate memory to hold backend information.  */
  abfd->tdata.som_data = (struct som_data_struct *)
    bfd_zalloc (abfd, sizeof (struct som_data_struct));
  if (abfd->tdata.som_data == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  obj_som_file_hdr (abfd) = bfd_zalloc (abfd, sizeof (struct header));
  if (obj_som_file_hdr (abfd) == NULL)

    {
      bfd_error = no_memory;
      return false;
    }
  return true;
}

/* Initialize some information in the file header.  This routine makes
   not attempt at doing the right thing for a full executable; it
   is only meant to handle relocatable objects.  */

static boolean
som_prep_headers (abfd)
     bfd *abfd;
{
  struct header *file_hdr = obj_som_file_hdr (abfd);
  asection *section;

  /* FIXME.  This should really be conditional based on whether or not
     PA1.1 instructions/registers have been used.  */
  file_hdr->system_id = HP9000S800_ID;

  /* FIXME.  Only correct for building relocatable objects.  */
  if (abfd->flags & EXEC_P)
    abort ();
  else
    file_hdr->a_magic = RELOC_MAGIC;

  /* Only new format SOM is supported.  */
  file_hdr->version_id = NEW_VERSION_ID;

  /* These fields are optional, and embedding timestamps is not always
     a wise thing to do, it makes comparing objects during a multi-stage
     bootstrap difficult.  */
  file_hdr->file_time.secs = 0;
  file_hdr->file_time.nanosecs = 0; 

  if (abfd->flags & EXEC_P)
    abort ();
  else
    {
      file_hdr->entry_space = 0;
      file_hdr->entry_subspace = 0;
      file_hdr->entry_offset = 0;
    }
  
  /* FIXME.  I do not know if we ever need to put anything other
     than zero in this field.  */
  file_hdr->presumed_dp = 0;

  /* Now iterate over the sections translating information from
     BFD sections to SOM spaces/subspaces.  */

  for (section = abfd->sections; section != NULL; section = section->next)
    {
      /* Ignore anything which has not been marked as a space or
	 subspace.  */
      if (som_section_data (section)->is_space == 0

	  && som_section_data (section)->is_subspace == 0)
	continue;

      if (som_section_data (section)->is_space)
	{
	  /* Set space attributes.  Note most attributes of SOM spaces
	     are set based on the subspaces it contains.  */
	  som_section_data (section)->space_dict.loader_fix_index = -1;
	  som_section_data (section)->space_dict.init_pointer_index = -1;
	}
      else
	{
	  /* Set subspace attributes.  Basic stuff is done here, additional
	     attributes are filled in later as more information becomes
	     available.  */
	  if (section->flags & SEC_IS_COMMON)
	    {
	      som_section_data (section)->subspace_dict.dup_common = 1;
	      som_section_data (section)->subspace_dict.is_common = 1;
	    }

	  if (section->flags & SEC_ALLOC)
	    som_section_data (section)->subspace_dict.is_loadable = 1;

	  if (section->flags & SEC_CODE)
	    som_section_data (section)->subspace_dict.code_only = 1;

	  som_section_data (section)->subspace_dict.subspace_start = 
	    section->vma;
	  som_section_data (section)->subspace_dict.subspace_length =
	    bfd_section_size (abfd, section);
	  som_section_data (section)->subspace_dict.initialization_length =
	    bfd_section_size (abfd, section);
	  som_section_data (section)->subspace_dict.alignment = 
	    1 << section->alignment_power;
	}
    }
  return true;
}

/* Count and return the number of spaces attached to the given BFD.  */

static unsigned long
som_count_spaces (abfd)
     bfd *abfd;
{
  int count = 0;
  asection *section;

  for (section = abfd->sections; section != NULL; section = section->next)
    count += som_section_data (section)->is_space;

  return count;
}

/* Count the number of subspaces attached to the given BFD.  */

static unsigned long
som_count_subspaces (abfd)
     bfd *abfd;
{
  int count = 0;
  asection *section;

  for (section = abfd->sections; section != NULL; section = section->next)
    count += som_section_data (section)->is_subspace;

  return count;
}

/* Return -1, 0, 1 indicating the relative ordering of sym1 and sym2.

   We desire symbols to be ordered starting with the symbol with the
   highest relocation count down to the symbol with the lowest relocation
   count.  Doing so compacts the relocation stream.  */

static int
compare_syms (sym1, sym2)
     asymbol **sym1;
     asymbol **sym2;

{
  unsigned int count1, count2;
  
  /* Get relocation count for each symbol.  Note that the count
     is stored in the udata pointer for section symbols!  */
  if ((*sym1)->flags & BSF_SECTION_SYM)
    count1 = (int)(*sym1)->udata;
  else
    count1 = (*som_symbol_data ((*sym1)))->reloc_count;

  if ((*sym2)->flags & BSF_SECTION_SYM)
    count2 = (int)(*sym2)->udata;
  else
    count2 = (*som_symbol_data ((*sym2)))->reloc_count;

  /* Return the appropriate value.  */
  if (count1 < count2)
    return 1;
  else if (count1 > count2)
    return -1;
  return 0;
}

/* Perform various work in preparation for emitting the fixup stream.  */

static void
som_prep_for_fixups (abfd, syms, num_syms)
     bfd *abfd;
     asymbol **syms;
     unsigned long num_syms;
{
  int i;
  asection *section;

  /* Most SOM relocations involving a symbol have a length which is
     dependent on the index of the symbol.  So symbols which are
     used often in relocations should have a small index.  */

  /* First initialize the counters for each symbol.  */
  for (i = 0; i < num_syms; i++)
    {
      /* Handle a section symbol; these have no pointers back to the 
	 SOM symbol info.  So we just use the pointer field (udata)
	 to hold the relocation count.

	 FIXME.  While we're here set the name of any section symbol
	 to something which will not screw GDB.  How do other formats
	 deal with this?!?  */
      if (som_symbol_data (syms[i]) == NULL)
	{
	  syms[i]->flags |= BSF_SECTION_SYM;
	  syms[i]->name = "L$0\002";
	  syms[i]->udata = (PTR) 0;
	}
      else
	(*som_symbol_data (syms[i]))->reloc_count = 0;
    }

  /* Now that the counters are initialized, make a weighted count
     of how often a given symbol is used in a relocation.  */
  for (section = abfd->sections; section != NULL; section = section->next)
    {
      int i;

      /* Does this section have any relocations?  */
      if (section->reloc_count <= 0)
	continue;

      /* Walk through each relocation for this section.  */
      for (i = 1; i < section->reloc_count; i++)
	{
	  arelent *reloc = section->orelocation[i];
	  int scale;

	  /* If no symbol, then there is no counter to increase.  */
	  if (reloc->sym_ptr_ptr == NULL)
	    continue;

	  /* Scaling to encourage symbols involved in R_DP_RELATIVE 
	     and R_CODE_ONE_SYMBOL relocations to come first.  These
	     two relocations have single byte versions if the symbol
	     index is very small.  */
	  if (reloc->howto->type == R_DP_RELATIVE
	      || reloc->howto->type == R_CODE_ONE_SYMBOL)
	    scale = 2;
	  else
	    scale = 1;

	  /* Handle section symbols by ramming the count in the udata
	     field.  It will not be used and the count is very important
	     for these symbols.  */
	  if ((*reloc->sym_ptr_ptr)->flags & BSF_SECTION_SYM)
	    {
	      (*reloc->sym_ptr_ptr)->udata =
		(PTR) ((int) (*reloc->sym_ptr_ptr)->udata + scale);
	      continue;
	    }

	  /* A normal symbol.  Increment the count.  */
	  (*som_symbol_data ((*reloc->sym_ptr_ptr)))->reloc_count += scale;
	}
    }

  /* Now sort the symbols.  */
  qsort (syms, num_syms, sizeof (asymbol *), compare_syms);

  /* Compute the symbol indexes, they will be needed by the relocation
     code.  */
  for (i = 0; i < num_syms; i++)
    {
      /* A section symbol.  Again, there is no pointer to backend symbol
	 information, so we reuse (abuse) the udata field again.  */
      if (syms[i]->flags & BSF_SECTION_SYM)
	syms[i]->udata = (PTR) i;
      else
        (*som_symbol_data (syms[i]))->index = i;
    }
}

static boolean
som_write_fixups (abfd, current_offset, total_reloc_sizep)
     bfd *abfd;
     unsigned long current_offset;
     unsigned int *total_reloc_sizep;
{
  unsigned int i, j;
  unsigned char *tmp_space, *p;
  unsigned int total_reloc_size = 0;
  unsigned int subspace_reloc_size = 0;
  unsigned int num_spaces = obj_som_file_hdr (abfd)->space_total;
  asection *section = abfd->sections;

  /* Get a chunk of memory that we can use as buffer space, then throw
     away.  */
  tmp_space = alloca (SOM_TMP_BUFSIZE);
  bzero (tmp_space, SOM_TMP_BUFSIZE);
  p = tmp_space;

  /* All the fixups for a particular subspace are emitted in a single
     stream.  All the subspaces for a particular space are emitted
     as a single stream.

     So, to get all the locations correct one must iterate through all the
     spaces, for each space iterate through its subspaces and output a
     fixups stream.  */
  for (i = 0; i < num_spaces; i++)
    {
      asection *subsection;

      /* Find a space.  */
      while (som_section_data (section)->is_space == 0)
	section = section->next;

      /* Now iterate through each of its subspaces.  */
      for (subsection = abfd->sections;
	   subsection != NULL;
	   subsection = subsection->next)
	{
	  int reloc_offset, current_rounding_mode;

	  /* Find a subspace of this space.  */
	  if (som_section_data (subsection)->is_subspace == 0
	      || som_section_data (subsection)->containing_space != section)
	    continue;

	  /* If this subspace had no relocations, then we're finished 
	     with it.  */
	  if (subsection->reloc_count <= 0)
	    {
	      som_section_data (subsection)->subspace_dict.fixup_request_index
		= -1;
	      continue;
	    }

	  /* This subspace has some relocations.  Put the relocation stream
	     index into the subspace record.  */
	  som_section_data (subsection)->subspace_dict.fixup_request_index
	    = total_reloc_size;

	  /* To make life easier start over with a clean slate for 
	     each subspace.  Seek to the start of the relocation stream
	     for this subspace in preparation for writing out its fixup
	     stream.  */
	  if (bfd_seek (abfd, current_offset + total_reloc_size, SEEK_SET) != 0)
	    {
	      bfd_error = system_call_error;
	      return false;
	    }

	  /* Buffer space has already been allocated.  Just perform some
	     initialization here.  */
	  p = tmp_space;
	  subspace_reloc_size = 0;
	  reloc_offset = 0;
	  som_initialize_reloc_queue (reloc_queue);
	  current_rounding_mode = R_N_MODE;

	  /* Translate each BFD relocation into one or more SOM 
	     relocations.  */
	  for (j = 0; j < subsection->reloc_count; j++)
	    {
	      arelent *bfd_reloc = subsection->orelocation[j];
	      unsigned int skip;
	      int sym_num;

	      /* Get the symbol number.  Remember it's stored in a 
		 special place for section symbols.  */
	      if ((*bfd_reloc->sym_ptr_ptr)->flags & BSF_SECTION_SYM)
		sym_num = (int) (*bfd_reloc->sym_ptr_ptr)->udata;
	      else
		sym_num = (*som_symbol_data ((*bfd_reloc->sym_ptr_ptr)))->index;
	      
	      /* If there is not enough room for the next couple relocations,
		 then dump the current buffer contents now.  Also reinitialize
		 the relocation queue. 

		 FIXME.  We assume here that no BFD relocation will expand
		 to more than 100 bytes of SOM relocations.  This should (?!?)
		 be quite safe.  */
	      if (p - tmp_space + 100 > SOM_TMP_BUFSIZE)
		{
		  if (bfd_write ((PTR) tmp_space, p - tmp_space, 1, abfd)
		      != p - tmp_space)
		    {
		      bfd_error = system_call_error;
		      return false;
		    }
		  p = tmp_space;
		  som_initialize_reloc_queue (reloc_queue);
		}

	      /* Emit R_NO_RELOCATION fixups to map any bytes which were
		 skipped.  */
	      skip = bfd_reloc->address - reloc_offset;
	      p = som_reloc_skip (abfd, skip, p,
				  &subspace_reloc_size, reloc_queue);

	      /* Update reloc_offset for the next iteration.

		 Many relocations do not consume input bytes.  They
		 are markers, or set state necessary to perform some
		 later relocation.  */
	      switch (bfd_reloc->howto->type)
		{
		/* This only needs to handle relocations that may be
		   made by hppa_som_gen_reloc.  */
		case R_ENTRY:
		case R_EXIT:
		case R_N_MODE:
		case R_S_MODE:
		case R_D_MODE:
		case R_R_MODE:
		  reloc_offset = bfd_reloc->address;
		  break;

		default:
		  reloc_offset = bfd_reloc->address + 4;
		  break;
		}

	      /* Now the actual relocation we care about.  */
	      switch (bfd_reloc->howto->type)
		{
		case R_PCREL_CALL:
		case R_ABS_CALL:
		  p = som_reloc_call (abfd, p, &subspace_reloc_size,
				      bfd_reloc, sym_num, reloc_queue);
		  break;

		case R_CODE_ONE_SYMBOL:
		case R_DP_RELATIVE:
		  /* Account for any addend.  */
		  if (bfd_reloc->addend)
		    p = som_reloc_addend (abfd, bfd_reloc->addend, p, 
					  &subspace_reloc_size, reloc_queue);

		  if (sym_num < 0x20)
		    {
		      bfd_put_8 (abfd, bfd_reloc->howto->type + sym_num, p);
		      subspace_reloc_size += 1;
		      p += 1;
		    }
		  else if (sym_num < 0x100)
		    {
		      bfd_put_8 (abfd, bfd_reloc->howto->type + 32, p);
		      bfd_put_8 (abfd, sym_num, p + 1);
		      p = try_prev_fixup (abfd, &subspace_reloc_size, p,
					  2, reloc_queue);
		    }
		  else if (sym_num < 0x10000000)
		    {
		      bfd_put_8 (abfd, bfd_reloc->howto->type + 33, p);
		      bfd_put_8 (abfd, sym_num >> 16, p + 1);
		      bfd_put_16 (abfd, sym_num, p + 2); 
		      p = try_prev_fixup (abfd, &subspace_reloc_size,
					  p, 4, reloc_queue);
		    }
		  else
		    abort ();
		  break;

		case R_DATA_ONE_SYMBOL:
		case R_DATA_PLABEL:
		case R_CODE_PLABEL:
		  /* Account for any addend.  */
		  if (bfd_reloc->addend)
		    p = som_reloc_addend (abfd, bfd_reloc->addend, p, 
					  &subspace_reloc_size, reloc_queue);

		  if (sym_num < 0x100)
		    {
		      bfd_put_8 (abfd, bfd_reloc->howto->type, p);
		      bfd_put_8 (abfd, sym_num, p + 1);
		      p = try_prev_fixup (abfd, &subspace_reloc_size, p,
					  2, reloc_queue);
		    }
		  else if (sym_num < 0x10000000)
		    {
		      bfd_put_8 (abfd, bfd_reloc->howto->type + 1, p);
		      bfd_put_8 (abfd, sym_num >> 16, p + 1);
		      bfd_put_16 (abfd, sym_num, p + 2); 
		      p = try_prev_fixup (abfd, &subspace_reloc_size,
					  p, 4, reloc_queue);
		    }
		  else
		    abort ();
		  break;

		case R_ENTRY:
		  {
		    int *descp
		       = (int *) (*som_symbol_data ((*bfd_reloc->sym_ptr_ptr)))->unwind;
		    bfd_put_8 (abfd, R_ENTRY, p);
		    bfd_put_32 (abfd, descp[0], p + 1);
		    bfd_put_32 (abfd, descp[1], p + 5);
		    p = try_prev_fixup (abfd, &subspace_reloc_size,
					p, 9, reloc_queue);
		    break;
		  }
		  
		case R_EXIT:
		  bfd_put_8 (abfd, R_EXIT, p);
		  subspace_reloc_size += 1;
		  p += 1;
		  break;

		case R_N_MODE:
		case R_S_MODE:
		case R_D_MODE:
		case R_R_MODE:
		  /* If this relocation requests the current rounding
		     mode, then it is redundant.  */
		  if (bfd_reloc->howto->type != current_rounding_mode)
		    {
		      bfd_put_8 (abfd, bfd_reloc->howto->type, p);
		      subspace_reloc_size += 1;
		      p += 1;
		      current_rounding_mode = bfd_reloc->howto->type;
		    }
		  break;

		/* Put a "R_RESERVED" relocation in the stream if
		   we hit something we do not understand.  The linker
		   will complain loudly if this ever happens.  */
		default:
		  bfd_put_8 (abfd, 0xff, p);
		  subspace_reloc_size += 1;
		  p += 1;
		  break;
		}
	    }

	  /* Last BFD relocation for a subspace has been processed.
	     Map the rest of the subspace with R_NO_RELOCATION fixups.  */
	  p = som_reloc_skip (abfd, bfd_section_size (abfd, subsection) 
			              - reloc_offset,
			      p, &subspace_reloc_size, reloc_queue);

	  /* Scribble out the relocations.  */
	  if (bfd_write ((PTR) tmp_space, p - tmp_space, 1, abfd)
	      != p - tmp_space)
	    {
	      bfd_error = system_call_error;
	      return false;
	    }
	  p = tmp_space;

	  total_reloc_size += subspace_reloc_size;
	  som_section_data (subsection)->subspace_dict.fixup_request_quantity
	    = subspace_reloc_size;
	}
      section = section->next;
    }
  *total_reloc_sizep = total_reloc_size;
  return true;
}

/* Write out the space/subspace string table.  */

static boolean
som_write_space_strings (abfd, current_offset, string_sizep)
     bfd *abfd;
     unsigned long current_offset;
     unsigned int *string_sizep;
{
  unsigned char *tmp_space, *p;
  unsigned int strings_size = 0;
  asection *section;

  /* Get a chunk of memory that we can use as buffer space, then throw
     away.  */
  tmp_space = alloca (SOM_TMP_BUFSIZE);
  bzero (tmp_space, SOM_TMP_BUFSIZE);
  p = tmp_space;

  /* Seek to the start of the space strings in preparation for writing
     them out.  */
  if (bfd_seek (abfd, current_offset, SEEK_SET) != 0)
    {
      bfd_error = system_call_error;
      return false;
    }

  /* Walk through all the spaces and subspaces (order is not important)
     building up and writing string table entries for their names.  */
  for (section = abfd->sections; section != NULL; section = section->next)
    {
      int length;

      /* Only work with space/subspaces; avoid any other sections
	 which might have been made (.text for example).  */
      if (som_section_data (section)->is_space == 0
	  && som_section_data (section)->is_subspace == 0)
	continue;

      /* Get the length of the space/subspace name.  */
      length = strlen (section->name);

      /* If there is not enough room for the next entry, then dump the
	 current buffer contents now.  Each entry will take 4 bytes to
	 hold the string length + the string itself + null terminator.  */
      if (p - tmp_space + 5 + length > SOM_TMP_BUFSIZE)
	{
	  if (bfd_write ((PTR) tmp_space, p - tmp_space, 1, abfd)
	      != p - tmp_space) 
	    {
	      bfd_error = system_call_error;
	      return false;
	    }
	  /* Reset to beginning of the buffer space.  */
	  p = tmp_space;
	}

      /* First element in a string table entry is the length of the
	 string.  Alignment issues are already handled.  */
      bfd_put_32 (abfd, length, p);
      p += 4;
      strings_size += 4;

      /* Record the index in the space/subspace records.  */
      if (som_section_data (section)->is_space)
	som_section_data (section)->space_dict.name.n_strx = strings_size;
      else
	som_section_data (section)->subspace_dict.name.n_strx = strings_size;

      /* Next comes the string itself + a null terminator.  */
      strcpy (p, section->name);
      p += length + 1;
      strings_size += length + 1;

      /* Always align up to the next word boundary.  */
      while (strings_size % 4)
	{
	  bfd_put_8 (abfd, 0, p);
	  p++;
	  strings_size++;
	}
    }

  /* Done with the space/subspace strings.  Write out any information
     contained in a partial block.  */
  if (bfd_write ((PTR) tmp_space, p - tmp_space, 1, abfd) != p - tmp_space)
    {
      bfd_error = system_call_error;
      return false;
    }
  *string_sizep = strings_size;
  return true;
}

/* Write out the symbol string table.  */

static boolean
som_write_symbol_strings (abfd, current_offset, syms, num_syms, string_sizep)
     bfd *abfd;
     unsigned long current_offset;
     asymbol **syms;
     unsigned int num_syms;
     unsigned int *string_sizep;
{
  unsigned int i;
  unsigned char *tmp_space, *p;
  unsigned int strings_size = 0;

  /* Get a chunk of memory that we can use as buffer space, then throw
     away.  */
  tmp_space = alloca (SOM_TMP_BUFSIZE);
  bzero (tmp_space, SOM_TMP_BUFSIZE);
  p = tmp_space;

  /* Seek to the start of the space strings in preparation for writing
     them out.  */
  if (bfd_seek (abfd, current_offset, SEEK_SET) != 0)
    {
      bfd_error = system_call_error;
      return false;
    }

  for (i = 0; i < num_syms; i++)
    {
      int length = strlen (syms[i]->name);

      /* If there is not enough room for the next entry, then dump the
	 current buffer contents now.  */
     if (p - tmp_space + 5 + length > SOM_TMP_BUFSIZE)
	{
	  if (bfd_write ((PTR) tmp_space, p - tmp_space, 1, abfd)
	      != p - tmp_space)
	    {
	      bfd_error = system_call_error;
	      return false;
	    }
	  /* Reset to beginning of the buffer space.  */
	  p = tmp_space;
	}

      /* First element in a string table entry is the length of the
	 string.  This must always be 4 byte aligned.  This is also
	 an appropriate time to fill in the string index field in the
	 symbol table entry.  */
      bfd_put_32 (abfd, length, p);
      strings_size += 4;
      p += 4;

      /* Next comes the string itself + a null terminator.  */
      strcpy (p, syms[i]->name);

      /* ACK.  FIXME.  */
      syms[i]->name = (char *)strings_size;
      p += length + 1;
      strings_size += length + 1;

      /* Always align up to the next word boundary.  */
      while (strings_size % 4)
        {
	  bfd_put_8 (abfd, 0, p);
	  strings_size++;
	  p++;
        }
    }

  /* Scribble out any partial block.  */
  if (bfd_write ((PTR) tmp_space, p - tmp_space, 1, abfd) != p - tmp_space)
    {
      bfd_error = system_call_error;
      return false;
    }

  *string_sizep = strings_size;
  return true;
}

/* Compute variable information to be placed in the SOM headers, 
   space/subspace dictionaries, relocation streams, etc.  Begin
   writing parts of the object file.  */

static boolean 
som_begin_writing (abfd)
     bfd *abfd;
{
  unsigned long current_offset = 0;
  int strings_size = 0;
  unsigned int total_reloc_size = 0;
  unsigned long num_spaces, num_subspaces, num_syms, i;
  asection *section;
  asymbol **syms = bfd_get_outsymbols (abfd);
  unsigned int total_subspaces = 0;

  /* The file header will always be first in an object file, 
     everything else can be in random locations.  To keep things
     "simple" BFD will lay out the object file in the manner suggested
     by the PRO ABI for PA-RISC Systems.  */

  /* Before any output can really begin offsets for all the major
     portions of the object file must be computed.  So, starting
     with the initial file header compute (and sometimes write)
     each portion of the object file.  */

  /* Make room for the file header, it's contents are not complete
     yet, so it can not be written at this time.  */
  current_offset += sizeof (struct header);  

  /* Any auxiliary headers will follow the file header.  Right now
     we support only the copyright and version headers.  */
  obj_som_file_hdr (abfd)->aux_header_location = current_offset;
  obj_som_file_hdr (abfd)->aux_header_size = 0;
  if (obj_som_version_hdr (abfd) != NULL)
    {
      unsigned int len;

      bfd_seek (abfd, current_offset, SEEK_SET);

      /* Write the aux_id structure and the string length.  */
      len = sizeof (struct aux_id) + sizeof (unsigned int);
      obj_som_file_hdr (abfd)->aux_header_size += len;
      current_offset += len;
      if (bfd_write ((PTR) obj_som_version_hdr (abfd), len, 1, abfd) != len)
	{
	  bfd_error = system_call_error;
	  return false;
	}

      /* Write the version string.  */
      len = obj_som_version_hdr (abfd)->string_length;
      obj_som_file_hdr (abfd)->aux_header_size += len;
      current_offset += len;
      if (bfd_write ((PTR) obj_som_version_hdr (abfd)->user_string,
		     len, 1, abfd) != len)
	{
	  bfd_error = system_call_error;
	  return false;
	}
    }

  if (obj_som_copyright_hdr (abfd) != NULL)
    {
      unsigned int len;

      bfd_seek (abfd, current_offset, SEEK_SET);

      /* Write the aux_id structure and the string length.  */
      len = sizeof (struct aux_id) + sizeof (unsigned int);
      obj_som_file_hdr (abfd)->aux_header_size += len;
      current_offset += len;
      if (bfd_write ((PTR) obj_som_copyright_hdr (abfd), len, 1, abfd) != len)
	{
	  bfd_error = system_call_error;
	  return false;
	}

      /* Write the copyright string.  */
      len = obj_som_copyright_hdr (abfd)->string_length;
      obj_som_file_hdr (abfd)->aux_header_size += len;
      current_offset += len;
      if (bfd_write ((PTR) obj_som_copyright_hdr (abfd)->copyright,
		     len, 1, abfd) != len)
	{
	  bfd_error = system_call_error;
	  return false;
	}
    }

  /* Next comes the initialization pointers; we have no initialization
     pointers, so current offset does not change.  */
  obj_som_file_hdr (abfd)->init_array_location = current_offset;
  obj_som_file_hdr (abfd)->init_array_total = 0;

  /* Next are the space records.  These are fixed length records.

     Count the number of spaces to determine how much room is needed
     in the object file for the space records.

     The names of the spaces are stored in a separate string table,
     and the index for each space into the string table is computed
     below.  Therefore, it is not possible to write the space headers
     at this time.  */
  num_spaces = som_count_spaces (abfd);
  obj_som_file_hdr (abfd)->space_location = current_offset;
  obj_som_file_hdr (abfd)->space_total = num_spaces;
  current_offset += num_spaces * sizeof (struct space_dictionary_record);

  /* Next are the subspace records.  These are fixed length records.

     Count the number of subspaes to determine how much room is needed
     in the object file for the subspace records.

     A variety if fields in the subspace record are still unknown at
     this time (index into string table, fixup stream location/size, etc).  */
  num_subspaces = som_count_subspaces (abfd);
  obj_som_file_hdr (abfd)->subspace_location = current_offset;
  obj_som_file_hdr (abfd)->subspace_total = num_subspaces;
  current_offset += num_subspaces * sizeof (struct subspace_dictionary_record);

  /* Next is the string table for the space/subspace names.  We will
     build and write the string table on the fly.  At the same time
     we will fill in the space/subspace name index fields.  */

  /* The string table needs to be aligned on a word boundary.  */
  if (current_offset % 4)
    current_offset += (4 - (current_offset % 4));

  /* Mark the offset of the space/subspace string table in the 
     file header.  */
  obj_som_file_hdr (abfd)->space_strings_location = current_offset;

  /* Scribble out the space strings.  */
  if (som_write_space_strings (abfd, current_offset, &strings_size) == false)
    return false;

  /* Record total string table size in the header and update the
     current offset.  */
  obj_som_file_hdr (abfd)->space_strings_size = strings_size;
  current_offset += strings_size;

  /* Next is the symbol table.  These are fixed length records.

     Count the number of symbols to determine how much room is needed
     in the object file for the symbol table.

     The names of the symbols are stored in a separate string table,
     and the index for each symbol name into the string table is computed
     below.  Therefore, it is not possible to write the symobl table
     at this time.  */
  num_syms = bfd_get_symcount (abfd);
  obj_som_file_hdr (abfd)->symbol_location = current_offset;
  obj_som_file_hdr (abfd)->symbol_total = num_syms;
  current_offset += num_syms * sizeof (struct symbol_dictionary_record);

  /* Do prep work before handling fixups.  */
  som_prep_for_fixups (abfd, syms, num_syms);

  /* Next comes the fixup stream which starts on a word boundary.  */
  if (current_offset % 4)
    current_offset += (4 - (current_offset % 4)); 
  obj_som_file_hdr (abfd)->fixup_request_location = current_offset;

  /* Write the fixups and update fields in subspace headers which
     relate to the fixup stream.  */
  if (som_write_fixups (abfd, current_offset, &total_reloc_size) == false)
    return false;

  /* Record the total size of the fixup stream in the file header.  */
  obj_som_file_hdr (abfd)->fixup_request_total = total_reloc_size;
  current_offset += total_reloc_size;

  /* Next are the symbol strings.
     Align them to a word boundary.  */
  if (current_offset % 4)
    current_offset += (4 - (current_offset % 4));
  obj_som_file_hdr (abfd)->symbol_strings_location = current_offset;

  /* Scribble out the symbol strings.  */
  if (som_write_symbol_strings (abfd, current_offset, syms, 
				num_syms, &strings_size)
      == false)
    return false;

  /* Record total string table size in header and update the
     current offset.  */
  obj_som_file_hdr (abfd)->symbol_strings_size = strings_size;
  current_offset += strings_size;

  /* Next is the compiler records.  We do not use these.  */
  obj_som_file_hdr (abfd)->compiler_location = current_offset;
  obj_som_file_hdr (abfd)->compiler_total = 0;

  /* Now compute the file positions for the loadable subspaces.  */

  section = abfd->sections;
  for (i = 0; i < num_spaces; i++)
    {
      asection *subsection;

      /* Find a space.  */
      while (som_section_data (section)->is_space == 0)
	section = section->next;

      /* Now look for all its subspaces.  */
      for (subsection = abfd->sections;
	   subsection != NULL;
	   subsection = subsection->next)
	{
	  
	  if (som_section_data (subsection)->is_subspace == 0
	      || som_section_data (subsection)->containing_space != section
	      || (subsection->flags & SEC_ALLOC) == 0)
	    continue;

	  som_section_data (subsection)->subspace_index = total_subspaces++;
	  /* This is real data to be loaded from the file.  */
	  if (subsection->flags & SEC_LOAD)
	    {
	      som_section_data (subsection)->subspace_dict.file_loc_init_value
		= current_offset;
	      section->filepos = current_offset;
	      current_offset += bfd_section_size (abfd, subsection); 
	    }
	  /* Looks like uninitialized data.  */
	  else
	    {
	      som_section_data (subsection)->subspace_dict.file_loc_init_value
		= 0;
	      som_section_data (subsection)->subspace_dict.
		initialization_length = 0;
	    }
	}
      /* Goto the next section.  */
      section = section->next; 
    }

  /* Finally compute the file positions for unloadable subspaces.  */

  obj_som_file_hdr (abfd)->unloadable_sp_location = current_offset;
  section = abfd->sections;
  for (i = 0; i < num_spaces; i++)
    {
      asection *subsection;

      /* Find a space.  */
      while (som_section_data (section)->is_space == 0)
	section = section->next;

      /* Now look for all its subspaces.  */
      for (subsection = abfd->sections;
	   subsection != NULL;
	   subsection = subsection->next)
	{
	  
	  if (som_section_data (subsection)->is_subspace == 0
	      || som_section_data (subsection)->containing_space != section
	      || (subsection->flags & SEC_ALLOC) != 0)
	    continue;

	  som_section_data (subsection)->subspace_index = total_subspaces++;
	  /* This is real data to be loaded from the file.  */
	  if ((subsection->flags & SEC_LOAD) == 0)
	    {
	      som_section_data (subsection)->subspace_dict.file_loc_init_value
		= current_offset;
	      section->filepos = current_offset;
	      current_offset += bfd_section_size (abfd, subsection); 
	    }
	  /* Looks like uninitialized data.  */
	  else
	    {
	      som_section_data (subsection)->subspace_dict.file_loc_init_value
		= 0;
	      som_section_data (subsection)->subspace_dict.
		initialization_length = bfd_section_size (abfd, subsection);
	    }
	}
      /* Goto the next section.  */
      section = section->next; 
    }

  obj_som_file_hdr (abfd)->unloadable_sp_size
    = current_offset - obj_som_file_hdr (abfd)->unloadable_sp_location;

  /* Loader fixups are not supported in any way shape or form.  */
  obj_som_file_hdr (abfd)->loader_fixup_location = 0;
  obj_som_file_hdr (abfd)->loader_fixup_total = 0;

  /* Done.  Store the total size of the SOM.  */
  obj_som_file_hdr (abfd)->som_length = current_offset;
  return true;
}

/* Finally, scribble out the various headers to the disk.  */

static boolean
som_write_headers (abfd)
     bfd *abfd;
{
  int num_spaces = som_count_spaces (abfd);
  int i;
  int subspace_index = 0;
  file_ptr location;
  asection *section;

  /* Subspaces are written first so that we can set up information
     about them in their containing spaces as the subspace is written.  */

  /* Seek to the start of the subspace dictionary records.  */
  location = obj_som_file_hdr (abfd)->subspace_location;
  bfd_seek (abfd, location, SEEK_SET);
  section = abfd->sections;
  /* Now for each loadable space write out records for its subspaces.  */
  for (i = 0; i < num_spaces; i++)
    {
      asection *subsection;

      /* Find a space.  */
      while (som_section_data (section)->is_space == 0)
	section = section->next;

      /* Now look for all its subspaces.  */
      for (subsection = abfd->sections;
	   subsection != NULL;
	   subsection = subsection->next)
	{
	  
	  /* Skip any section which does not correspond to a space
	     or subspace.  Or does not have SEC_ALLOC set (and therefore
	     has no real bits on the disk).  */
	  if (som_section_data (subsection)->is_subspace == 0
	      || som_section_data (subsection)->containing_space != section
	      || (subsection->flags & SEC_ALLOC) == 0)
	    continue;

	  /* If this is the first subspace for this space, then save
	     the index of the subspace in its containing space.  Also
	     set "is_loadable" in the containing space.  */

	  if (som_section_data (section)->space_dict.subspace_quantity == 0)
	    {
	      som_section_data (section)->space_dict.is_loadable = 1;
	      som_section_data (section)->space_dict.subspace_index
		= subspace_index;
	    }

	  /* Increment the number of subspaces seen and the number of
	     subspaces contained within the current space.  */
	  subspace_index++;
	  som_section_data (section)->space_dict.subspace_quantity++;

	  /* Mark the index of the current space within the subspace's
	     dictionary record.  */
	  som_section_data (subsection)->subspace_dict.space_index = i;
	  
	  /* Dump the current subspace header.  */
	  if (bfd_write ((PTR) &som_section_data (subsection)->subspace_dict,
			 sizeof (struct subspace_dictionary_record), 1, abfd)
	      != sizeof (struct subspace_dictionary_record))
	    {
	      bfd_error = system_call_error;
	      return false;
	    }
	}
      /* Goto the next section.  */
      section = section->next; 
    }

  /* Now repeat the process for unloadable subspaces.  */
  section = abfd->sections;
  /* Now for each space write out records for its subspaces.  */
  for (i = 0; i < num_spaces; i++)
    {
      asection *subsection;

      /* Find a space.  */
      while (som_section_data (section)->is_space == 0)
	section = section->next;

      /* Now look for all its subspaces.  */
      for (subsection = abfd->sections;
	   subsection != NULL;
	   subsection = subsection->next)
	{
	  
	  /* Skip any section which does not correspond to a space or
	     subspace, or which SEC_ALLOC set (and therefore handled
	     in the loadable spaces/subspaces code above.  */

	  if (som_section_data (subsection)->is_subspace == 0
	      || som_section_data (subsection)->containing_space != section
	      || (subsection->flags & SEC_ALLOC) != 0)
	    continue;

	  /* If this is the first subspace for this space, then save
	     the index of the subspace in its containing space.  Clear
	     "is_loadable".  */

	  if (som_section_data (section)->space_dict.subspace_quantity == 0)
	    {
	      som_section_data (section)->space_dict.is_loadable = 0;
	      som_section_data (section)->space_dict.subspace_index
		= subspace_index;
	    }

	  /* Increment the number of subspaces seen and the number of
	     subspaces contained within the current space.  */
	  som_section_data (section)->space_dict.subspace_quantity++;
	  subspace_index++; 

	  /* Mark the index of the current space within the subspace's
	     dictionary record.  */
	  som_section_data (subsection)->subspace_dict.space_index = i;
	  
	  /* Dump this subspace header.  */
	  if (bfd_write ((PTR) &som_section_data (subsection)->subspace_dict,
			 sizeof (struct subspace_dictionary_record), 1, abfd)
	      != sizeof (struct subspace_dictionary_record))
	    {
	      bfd_error = system_call_error;
	      return false;
	    }
	}
      /* Goto the next section.  */
      section = section->next; 
    }

  /* All the subspace dictiondary records are written, and all the
     fields are set up in the space dictionary records.

     Seek to the right location and start writing the space
     dictionary records.  */
  location = obj_som_file_hdr (abfd)->space_location;
  bfd_seek (abfd, location, SEEK_SET);

  section = abfd->sections;
  for (i = 0; i < num_spaces; i++)
    {

      /* Find a space.  */
      while (som_section_data (section)->is_space == 0)
	section = section->next;

      /* Dump its header  */
      if (bfd_write ((PTR) &som_section_data (section)->space_dict,
		     sizeof (struct space_dictionary_record), 1, abfd)
	  != sizeof (struct space_dictionary_record))
	{
	  bfd_error = system_call_error;
	  return false;
	}

      /* Goto the next section.  */
      section = section->next;
    }

  /* Only thing left to do is write out the file header.  It is always
     at location zero.  Seek there and write it.  */
  bfd_seek (abfd, (file_ptr) 0, SEEK_SET);
  if (bfd_write ((PTR) obj_som_file_hdr (abfd),
		 sizeof (struct header), 1, abfd)
      != sizeof (struct header))
    {
      bfd_error = system_call_error;
      return false;
    }
  return true;
}

/* Compute and return the checksum for a SOM file header.  */

static unsigned long
som_compute_checksum (abfd)
     bfd *abfd;
{
  unsigned long checksum, count, i;
  unsigned long *buffer = (unsigned long *) obj_som_file_hdr (abfd);

  checksum = 0;
  count = sizeof (struct header) / sizeof (unsigned long);
  for (i = 0; i < count; i++)
    checksum ^= *(buffer + i);

  return checksum;
}

/* Build and write, in one big chunk, the entire symbol table for
   this BFD.  */

static boolean
som_build_and_write_symbol_table (abfd)
     bfd *abfd;
{
  unsigned int num_syms = bfd_get_symcount (abfd);
  file_ptr symtab_location = obj_som_file_hdr (abfd)->symbol_location;
  asymbol **bfd_syms = bfd_get_outsymbols (abfd);
  struct symbol_dictionary_record *som_symtab;
  int i, symtab_size;

  /* Compute total symbol table size and allocate a chunk of memory
     to hold the symbol table as we build it.  */
  symtab_size = num_syms * sizeof (struct symbol_dictionary_record);
  som_symtab = (struct symbol_dictionary_record *) alloca (symtab_size);
  bzero (som_symtab, symtab_size);

  /* Walk over each symbol.  */
  for (i = 0; i < num_syms; i++)
    {
      /* This is really an index into the symbol strings table.  
	 By the time we get here, the index has already been 
	 computed and stored into the name field in the BFD symbol.  */
      som_symtab[i].name.n_strx = (int) bfd_syms[i]->name;

      /* The HP SOM linker requires detailed type information about
	 all symbols (including undefined symbols!).  Unfortunately,
	 the type specified in an import/export statement does not
	 always match what the linker wants.  Severe braindamage.  */
	 
      /* Section symbols will not have a SOM symbol type assigned to
	 them yet.  Assign all section symbols type ST_DATA.  */
      if (bfd_syms[i]->flags & BSF_SECTION_SYM)
	som_symtab[i].symbol_type = ST_DATA;
      else
	{
	  /* Common symbols must have scope SS_UNSAT and type
	     ST_STORAGE or the linker will choke.  */
	  if (bfd_syms[i]->section == &bfd_com_section)
	    {
	      som_symtab[i].symbol_scope = SS_UNSAT;
	      som_symtab[i].symbol_type = ST_STORAGE;
	    }

	  /* It is possible to have a symbol without an associated
	     type.  This happens if the user imported the symbol
	     without a type and the symbol was never defined
	     locally.  If BSF_FUNCTION is set for this symbol, then
	     assign it type ST_CODE (the HP linker requires undefined
	     external functions to have type ST_CODE rather than ST_ENTRY.  */
	  else if (((*som_symbol_data (bfd_syms[i]))->som_type
		    == SYMBOL_TYPE_UNKNOWN)
		   && (bfd_syms[i]->section == &bfd_und_section)
		   && (bfd_syms[i]->flags & BSF_FUNCTION))
	    som_symtab[i].symbol_type = ST_CODE;

	  /* Handle function symbols which were defined in this file.
	     They should have type ST_ENTRY.  Also retrieve the argument
	     relocation bits from the SOM backend information.  */
	  else if (((*som_symbol_data (bfd_syms[i]))->som_type
		    == SYMBOL_TYPE_ENTRY)
		   || (((*som_symbol_data (bfd_syms[i]))->som_type
			== SYMBOL_TYPE_CODE)
		       && (bfd_syms[i]->flags & BSF_FUNCTION))
		   || (((*som_symbol_data (bfd_syms[i]))->som_type
			== SYMBOL_TYPE_UNKNOWN)
		       && (bfd_syms[i]->flags & BSF_FUNCTION)))
	    {
	      som_symtab[i].symbol_type = ST_ENTRY;
	      som_symtab[i].arg_reloc
		= (*som_symbol_data (bfd_syms[i]))->tc_data.hppa_arg_reloc;
	    }

	  /* If the type is unknown at this point, it should be
	     ST_DATA (functions were handled as special cases above).  */
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_UNKNOWN)
	    som_symtab[i].symbol_type = ST_DATA;

	  /* From now on it's a very simple mapping.  */
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_ABSOLUTE)
	    som_symtab[i].symbol_type = ST_ABSOLUTE;
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_CODE)
	    som_symtab[i].symbol_type = ST_CODE;
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_DATA)
	    som_symtab[i].symbol_type = ST_DATA;
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_MILLICODE)
	    som_symtab[i].symbol_type = ST_MILLICODE;
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_PLABEL)
	    som_symtab[i].symbol_type = ST_PLABEL;
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_PRI_PROG)
	    som_symtab[i].symbol_type = ST_PRI_PROG;
	  else if ((*som_symbol_data (bfd_syms[i]))->som_type
		   == SYMBOL_TYPE_SEC_PROG)
	    som_symtab[i].symbol_type = ST_SEC_PROG;
	}
	
      /* Now handle the symbol's scope.  Exported data which is not
	 in the common section has scope SS_UNIVERSAL.  Note scope
	 of common symbols was handled earlier!  */
      if (bfd_syms[i]->flags & BSF_EXPORT
	  && bfd_syms[i]->section != &bfd_com_section)
	som_symtab[i].symbol_scope = SS_UNIVERSAL;
      /* Any undefined symbol at this point has a scope SS_UNSAT.  */
      else if (bfd_syms[i]->section == &bfd_und_section)
	som_symtab[i].symbol_scope = SS_UNSAT;
      /* Anything else which is not in the common section has scope
	 SS_LOCAL.  */
      else if (bfd_syms[i]->section != &bfd_com_section)
	som_symtab[i].symbol_scope = SS_LOCAL;

      /* Now set the symbol_info field.  It has no real meaning
	 for undefined or common symbols, but the HP linker will
	 choke if it's not set to some "reasonable" value.  We
	 use zero as a reasonable value.  */
      if (bfd_syms[i]->section == &bfd_com_section
	  || bfd_syms[i]->section == &bfd_und_section)
	som_symtab[i].symbol_info = 0;
      /* For all other symbols, the symbol_info field contains the 
	 subspace index of the space this symbol is contained in.  */
      else
	som_symtab[i].symbol_info
	  = som_section_data (bfd_syms[i]->section)->subspace_index;

      /* Set the symbol's value.  */
      som_symtab[i].symbol_value
	= bfd_syms[i]->value + bfd_syms[i]->section->vma;
    }

  /* Egad.  Everything is ready, seek to the right location and
     scribble out the symbol table.  */
  if (bfd_seek (abfd, symtab_location, SEEK_SET) != 0)
    {
      bfd_error = system_call_error;
      return false;
    }

  if (bfd_write ((PTR) som_symtab, symtab_size, 1, abfd) != symtab_size)
    {
      bfd_error  = system_call_error;
      return false;
    }
  return true; 
}

/* Write an object in SOM format.  */  

static boolean
som_write_object_contents (abfd)
     bfd *abfd;
{
  if (abfd->output_has_begun == false)
    {
      /* Set up fixed parts of the file, space, and subspace headers.
	 Notify the world that output has begun.  */
      som_prep_headers (abfd);
      abfd->output_has_begun = true;
      /* Start writing the object file.  This include all the string
	 tables, fixup streams, and other portions of the object file.  */
      som_begin_writing (abfd);
    }

  /* Now that the symbol table information is complete, build and
     write the symbol table.  */
  if (som_build_and_write_symbol_table (abfd) == false)
    return false;

  /* Compute the checksum for the file header just before writing
     the header to disk.  */
  obj_som_file_hdr (abfd)->checksum = som_compute_checksum (abfd);
  return (som_write_headers (abfd));
}


/* Read and save the string table associated with the given BFD.  */

static boolean
som_slurp_string_table (abfd)
     bfd *abfd;
{
  char *stringtab;

  /* Use the saved version if its available.  */
  if (obj_som_stringtab (abfd) != NULL)
    return true;

  /* Allocate and read in the string table.  */
  stringtab = bfd_zalloc (abfd, obj_som_stringtab_size (abfd));
  if (stringtab == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  if (bfd_seek (abfd, obj_som_str_filepos (abfd), SEEK_SET) < 0)
    {
      bfd_error = system_call_error;
      return false;
    }
  
  if (bfd_read (stringtab, obj_som_stringtab_size (abfd), 1, abfd)
      != obj_som_stringtab_size (abfd))
    {
      bfd_error = system_call_error;
      return false;
    }

  /* Save our results and return success. */
  obj_som_stringtab (abfd) = stringtab;
  return true;
}

/* Return the amount of data (in bytes) required to hold the symbol
   table for this object.  */

static unsigned int
som_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  if (!som_slurp_symbol_table (abfd))
    return 0;

  return (bfd_get_symcount (abfd) + 1) * (sizeof (som_symbol_type *));
}

/* Convert from a SOM subspace index to a BFD section.  */

static asection *
som_section_from_subspace_index (abfd, index)
     bfd *abfd;
     unsigned int index;
{
  asection *section;

  for (section = abfd->sections; section != NULL; section = section->next)
    if (som_section_data (section)->subspace_index == index)
      return section;

  /* Should never happen.  */
  abort();
}

/* Read and save the symbol table associated with the given BFD.  */

static unsigned int
som_slurp_symbol_table (abfd)
     bfd *abfd;
{
  int symbol_count = bfd_get_symcount (abfd);
  int symsize = sizeof (struct symbol_dictionary_record);
  char *stringtab;
  struct symbol_dictionary_record *buf, *bufp, *endbufp;
  som_symbol_type *sym, *symbase;

  /* Return saved value if it exists.  */
  if (obj_som_symtab (abfd) != NULL)
    return true;

  /* Sanity checking.  Make sure there are some symbols and that 
     we can read the string table too.  */
  if (symbol_count == 0)
    {
      bfd_error = no_symbols;
      return false;
    }

  if (!som_slurp_string_table (abfd))
    return false;

  stringtab = obj_som_stringtab (abfd);

  symbase = (som_symbol_type *)
    bfd_zalloc (abfd, symbol_count * sizeof (som_symbol_type));
  if (symbase == NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  /* Read in the external SOM representation.  */
  buf = alloca (symbol_count * symsize);
  if (buf == NULL)
    {
      bfd_error = no_memory;
      return false;
    }
  if (bfd_seek (abfd, obj_som_sym_filepos (abfd), SEEK_SET) < 0)
    {
      bfd_error = system_call_error;
      return false;
    }
  if (bfd_read (buf, symbol_count * symsize, 1, abfd) 
      != symbol_count * symsize)
    {
      bfd_error = no_symbols;
      return (false);
    }

  /* Iterate over all the symbols and internalize them.  */
  endbufp = buf + symbol_count;
  for (bufp = buf, sym = symbase; bufp < endbufp; ++bufp)
    {

      /* I don't think we care about these.  */
      if (bufp->symbol_type == ST_SYM_EXT
	  || bufp->symbol_type == ST_ARG_EXT)
	continue;

      /* Some reasonable defaults.  */
      sym->symbol.the_bfd = abfd;
      sym->symbol.name = bufp->name.n_strx + stringtab;
      sym->symbol.value = bufp->symbol_value;
      sym->symbol.section = 0;
      sym->symbol.flags = 0;

      switch (bufp->symbol_type)
	{
	case ST_ENTRY:
	case ST_PRI_PROG:
	case ST_SEC_PROG:
	case ST_MILLICODE:
	  sym->symbol.flags |= BSF_FUNCTION;
	  sym->symbol.value &= ~0x3;
	  break;

	case ST_STUB:
	case ST_CODE:
	  sym->symbol.value &= ~0x3;

	default:
	  break;
	}

      /* Handle scoping and section information.  */
      switch (bufp->symbol_scope)
	{
	/* symbol_info field is undefined for SS_EXTERNAL and SS_UNSAT symbols,
	   so the section associated with this symbol can't be known.  */
	case SS_EXTERNAL:
	case SS_UNSAT:
	  if (bufp->symbol_type != ST_STORAGE)
	    sym->symbol.section = &bfd_und_section;
	  else
	    sym->symbol.section = &bfd_com_section;
	  sym->symbol.flags |= (BSF_EXPORT | BSF_GLOBAL);
	  break;

	case SS_UNIVERSAL:
	  sym->symbol.flags |= (BSF_EXPORT | BSF_GLOBAL);
	  sym->symbol.section
	    = som_section_from_subspace_index (abfd, bufp->symbol_info);
	  sym->symbol.value -= sym->symbol.section->vma;
	  break;

#if 0
	/* SS_GLOBAL and SS_LOCAL are two names for the same thing.
	   Sound dumb?  It is.  */
	case SS_GLOBAL:
#endif
	case SS_LOCAL:
	  sym->symbol.flags |= BSF_LOCAL;
	  sym->symbol.section
	    = som_section_from_subspace_index (abfd, bufp->symbol_info);
	  sym->symbol.value -= sym->symbol.section->vma;
	  break;
	}

      /* Mark symbols left around by the debugger.  */
      if (strlen (sym->symbol.name) >= 2
	  && sym->symbol.name[0] == 'L'
	  && (sym->symbol.name[1] == '$' || sym->symbol.name[2] == '$'
	      || sym->symbol.name[3] == '$'))
	sym->symbol.flags |= BSF_DEBUGGING;

      /* Note increment at bottom of loop, since we skip some symbols
         we can not include it as part of the for statement.  */
      sym++;
    }

  /* Save our results and return success.  */
  obj_som_symtab (abfd) = symbase;
  return (true);
}

/* Canonicalize a SOM symbol table.  Return the number of entries
   in the symbol table.  */

static unsigned int
som_get_symtab (abfd, location)
     bfd *abfd;
     asymbol **location;
{
  int i;
  som_symbol_type *symbase;

  if (!som_slurp_symbol_table (abfd))
    return 0;

  i = bfd_get_symcount (abfd);
  symbase = obj_som_symtab (abfd);

  for (; i > 0; i--, location++, symbase++)
    *location = &symbase->symbol;

  /* Final null pointer.  */
  *location = 0;
  return (bfd_get_symcount (abfd));
}

/* Make a SOM symbol.  There is nothing special to do here.  */

static asymbol *
som_make_empty_symbol (abfd)
     bfd *abfd;
{
  som_symbol_type *new =
  (som_symbol_type *) bfd_zalloc (abfd, sizeof (som_symbol_type));
  if (new == NULL)
    {
      bfd_error = no_memory;
      return 0;
    }
  new->symbol.the_bfd = abfd;

  return &new->symbol;
}

/* Print symbol information.  */

static void
som_print_symbol (ignore_abfd, afile, symbol, how)
     bfd *ignore_abfd;
     PTR afile;
     asymbol *symbol;
     bfd_print_symbol_type how;
{
  FILE *file = (FILE *) afile;
  switch (how)
    {
    case bfd_print_symbol_name:
      fprintf (file, "%s", symbol->name);
      break;
    case bfd_print_symbol_more:
      fprintf (file, "som ");
      fprintf_vma (file, symbol->value);
      fprintf (file, " %lx", (long) symbol->flags);
      break;
    case bfd_print_symbol_all:
      {
	CONST char *section_name;
	section_name = symbol->section ? symbol->section->name : "(*none*)";
	bfd_print_symbol_vandf ((PTR) file, symbol);
	fprintf (file, " %s\t%s", section_name, symbol->name);
	break;
      }
    }
}

/* Count or process variable-length SOM fixup records.

   To avoid code duplication we use this code both to compute the number
   of relocations requested by a stream, and to internalize the stream.

   When computing the number of relocations requested by a stream the
   variables rptr, section, and symbols have no meaning.

   Return the number of relocations requested by the fixup stream.  When
   not just counting 

   This needs at least two or three more passes to get it cleaned up.  */

static unsigned int
som_set_reloc_info (fixup, end, internal_relocs, section, symbols, just_count)
     unsigned char *fixup;
     unsigned int end;
     arelent *internal_relocs;
     asection *section;
     asymbol **symbols;
     boolean just_count;
{
  unsigned int op, varname;
  unsigned char *end_fixups = &fixup[end];
  const struct fixup_format *fp;
  char *cp;
  unsigned char *save_fixup;
  int variables[26], stack[20], c, v, count, prev_fixup, *sp;
  const int *subop;
  arelent *rptr= internal_relocs;
  unsigned int offset = just_count ? 0 : section->vma;

#define	var(c)		variables[(c) - 'A']
#define	push(v)		(*sp++ = (v))
#define	pop()		(*--sp)
#define	emptystack()	(sp == stack)

  som_initialize_reloc_queue (reloc_queue);
  bzero (variables, sizeof (variables));
  bzero (stack, sizeof (stack));
  count = 0;
  prev_fixup = 0;
  sp = stack;

  while (fixup < end_fixups)
    {

      /* Save pointer to the start of this fixup.  We'll use
	 it later to determine if it is necessary to put this fixup
	 on the queue.  */
      save_fixup = fixup;

      /* Get the fixup code and its associated format.  */
      op = *fixup++;
      fp = &som_fixup_formats[op];

      /* Handle a request for a previous fixup.  */
      if (*fp->format == 'P')
	{
	  /* Get pointer to the beginning of the prev fixup, move
	     the repeated fixup to the head of the queue.  */
	  fixup = reloc_queue[fp->D].reloc;
	  som_reloc_queue_fix (reloc_queue, fp->D);
	  prev_fixup = 1;

	  /* Get the fixup code and its associated format.  */
	  op = *fixup++;
	  fp = &som_fixup_formats[op];
	}

      /* If we are not just counting, set some reasonable defaults.  */
      if (! just_count)
	{
	  rptr->address = offset;
	  rptr->howto = &som_hppa_howto_table[op];
	  rptr->addend = 0;
	}

      /* Set default input length to 0.  Get the opcode class index
	 into D.  */
      var ('L') = 0;
      var ('D') = fp->D;

      /* Get the opcode format.  */
      cp = fp->format;

      /* Process the format string.  Parsing happens in two phases,
	 parse RHS, then assign to LHS.  Repeat until no more 
	 characters in the format string.  */
      while (*cp)
	{
	  /* The variable this pass is going to compute a value for.  */
	  varname = *cp++;

	  /* Start processing RHS.  Continue until a NULL or '=' is found.  */
	  do
	    {
	      c = *cp++;

	      /* If this is a variable, push it on the stack.  */
	      if (isupper (c))
		push (var (c));

	      /* If this is a lower case letter, then it represents
		 additional data from the fixup stream to be pushed onto
		 the stack.  */
	      else if (islower (c))
		{
		  for (v = 0; c > 'a'; --c)
		    v = (v << 8) | *fixup++;
		  push (v);
		}

	      /* A decimal constant.  Push it on the stack.  */
	      else if (isdigit (c))
		{
		  v = c - '0';
		  while (isdigit (*cp))
		    v = (v * 10) + (*cp++ - '0');
		  push (v);
		}
	      else

		/* An operator.  Pop two two values from the stack and
		   use them as operands to the given operation.  Push
		   the result of the operation back on the stack.  */
		switch (c)
		  {
		  case '+':
		    v = pop ();
		    v += pop ();
		    push (v);
		    break;
		  case '*':
		    v = pop ();
		    v *= pop ();
		    push (v);
		    break;
		  case '<':
		    v = pop ();
		    v = pop () << v;
		    push (v);
		    break;
		  default:
		    abort ();
		  }
	    }
	  while (*cp && *cp != '=');

	  /* Move over the equal operator.  */
	  cp++;

	  /* Pop the RHS off the stack.  */
	  c = pop ();

	  /* Perform the assignment.  */
	  var (varname) = c;

	  /* Handle side effects. and special 'O' stack cases.  */
	  switch (varname)
	    {
	    /* Consume some bytes from the input space.  */
	    case 'L':
	      offset += c;
	      break;
	    /* A symbol to use in the relocation.  Make a note
	       of this if we are not just counting.  */
	    case 'S':
	      if (! just_count)
		rptr->sym_ptr_ptr = &symbols[c];
	      break;
	    /* Handle the linker expression stack.  */
	    case 'O':
	      switch (op)
		{
		case R_COMP1:
		  subop = comp1_opcodes;
		  break;
		case R_COMP2:
		  subop = comp2_opcodes;
		  break;
		case R_COMP3:
		  subop = comp3_opcodes;
		  break;
		default:
		  abort ();
		}
	      while (*subop <= (unsigned char) c)
		++subop;
	      --subop;
	      break;
	    default:
	      break;
	    }
	}

      /* If we used a previous fixup, clean up after it.  */
      if (prev_fixup)
	{
	  fixup = save_fixup + 1;
	  prev_fixup = 0;
	}
      /* Queue it.  */
      else if (fixup > save_fixup + 1)
	som_reloc_queue_insert (save_fixup, fixup - save_fixup, reloc_queue);

      /* We do not pass R_DATA_OVERRIDE or R_NO_RELOCATION 
	 fixups to BFD.  */
      if (som_hppa_howto_table[op].type != R_DATA_OVERRIDE
	  && som_hppa_howto_table[op].type != R_NO_RELOCATION)
	{
	  /* Done with a single reloction. Loop back to the top.  */
	  if (! just_count)
	    {
	      rptr->addend = var ('V');
	      rptr++;
	    }
	  count++;
	  /* Now that we've handled a "full" relocation, reset
	     some state.  */
	  bzero (variables, sizeof (variables));
	  bzero (stack, sizeof (stack));
	}
    }
  return count;

#undef var
#undef push
#undef pop
#undef emptystack
}

/* Read in the relocs (aka fixups in SOM terms) for a section. 

   som_get_reloc_upper_bound calls this routine with JUST_COUNT 
   set to true to indicate it only needs a count of the number
   of actual relocations.  */

static boolean
som_slurp_reloc_table (abfd, section, symbols, just_count)
     bfd *abfd;
     asection *section;
     asymbol **symbols;
     boolean just_count;
{
  char *external_relocs;
  unsigned int fixup_stream_size;
  arelent *internal_relocs;
  unsigned int num_relocs;

  fixup_stream_size = som_section_data (section)->reloc_size;
  /* If there were no relocations, then there is nothing to do.  */
  if (section->reloc_count == 0)
    return true;

  /* If reloc_count is -1, then the relocation stream has not been 
     parsed.  We must do so now to know how many relocations exist.  */
  if (section->reloc_count == -1)
    {
      external_relocs = (char *) bfd_zalloc (abfd, fixup_stream_size);
      if (external_relocs == (char *) NULL)
	{
	  bfd_error = no_memory;
	  return false;
	}
      /* Read in the external forms. */
      if (bfd_seek (abfd,
		    obj_som_reloc_filepos (abfd) + section->rel_filepos,
		    SEEK_SET)
	  != 0)
	{
	  bfd_error = system_call_error;
	  return false;
	}
      if (bfd_read (external_relocs, 1, fixup_stream_size, abfd)
	  != fixup_stream_size)
	{
	  bfd_error = system_call_error;
	  return false;
	}
      /* Let callers know how many relocations found.
	 also save the relocation stream as we will
	 need it again.  */
      section->reloc_count = som_set_reloc_info (external_relocs,
						 fixup_stream_size,
						 NULL, NULL, NULL, true);

      som_section_data (section)->reloc_stream = external_relocs;
    }

  /* If the caller only wanted a count, then return now.  */
  if (just_count)
    return true;

  num_relocs = section->reloc_count;
  external_relocs = som_section_data (section)->reloc_stream;
  /* Return saved information about the relocations if it is available.  */
  if (section->relocation != (arelent *) NULL)
    return true;

  internal_relocs = (arelent *) bfd_zalloc (abfd,
					    num_relocs * sizeof (arelent));
  if (internal_relocs == (arelent *) NULL)
    {
      bfd_error = no_memory;
      return false;
    }

  /* Process and internalize the relocations.  */
  som_set_reloc_info (external_relocs, fixup_stream_size,
		      internal_relocs, section, symbols, false);

  /* Save our results and return success.  */
  section->relocation = internal_relocs;
  return (true);
}

/* Return the number of bytes required to store the relocation
   information associated with the given section.  */ 

static unsigned int
som_get_reloc_upper_bound (abfd, asect)
     bfd *abfd;
     sec_ptr asect;
{
  /* If section has relocations, then read in the relocation stream
     and parse it to determine how many relocations exist.  */
  if (asect->flags & SEC_RELOC)
    {
      if (som_slurp_reloc_table (abfd, asect, NULL, true))
	return (asect->reloc_count + 1) * sizeof (arelent);
    }
  /* Either there are no relocations or an error occurred while 
     reading and parsing the relocation stream.  */ 
  return 0;
}

/* Convert relocations from SOM (external) form into BFD internal
   form.  Return the number of relocations.  */

static unsigned int
som_canonicalize_reloc (abfd, section, relptr, symbols)
     bfd *abfd;
     sec_ptr section;
     arelent **relptr;
     asymbol **symbols;
{
  arelent *tblptr;
  int count;

  if (som_slurp_reloc_table (abfd, section, symbols, false) == false)
    return 0;

  count = section->reloc_count;
  tblptr = section->relocation;
  if (tblptr == (arelent *) NULL)
    return 0;

  while (count--)
    *relptr++ = tblptr++;

  *relptr = (arelent *) NULL;
  return section->reloc_count;
}

extern bfd_target som_vec;

/* A hook to set up object file dependent section information.  */

static boolean
som_new_section_hook (abfd, newsect)
     bfd *abfd;
     asection *newsect;
{
  newsect->used_by_bfd = (struct som_section_data_struct *)
    bfd_zalloc (abfd, sizeof (struct som_section_data_struct));
  newsect->alignment_power = 3;

  /* Initialize the subspace_index field to -1 so that it does
     not match a subspace with an index of 0.  */
  som_section_data (newsect)->subspace_index = -1;

  /* We allow more than three sections internally */
  return true;
}

/* Set backend info for sections which can not be described
   in the BFD data structures.  */

void
bfd_som_set_section_attributes (section, defined, private, sort_key, spnum)
     asection *section;
     char defined;
     char private;
     unsigned char sort_key;
     int spnum;
{
  struct space_dictionary_record *space_dict;

  som_section_data (section)->is_space = 1;
  space_dict = &som_section_data (section)->space_dict;
  space_dict->is_defined = defined;
  space_dict->is_private = private;
  space_dict->sort_key = sort_key;
  space_dict->space_number = spnum;
}

/* Set backend info for subsections which can not be described 
   in the BFD data structures.  */

void
bfd_som_set_subsection_attributes (section, container, access,
				   sort_key, quadrant)
     asection *section;
     asection *container;
     int access;
     unsigned char sort_key;
     int quadrant;
{
  struct subspace_dictionary_record *subspace_dict;
  som_section_data (section)->is_subspace = 1;
  subspace_dict = &som_section_data (section)->subspace_dict;
  subspace_dict->access_control_bits = access;
  subspace_dict->sort_key = sort_key;
  subspace_dict->quadrant = quadrant;
  som_section_data (section)->containing_space = container;
}

/* Set the full SOM symbol type.  SOM needs far more symbol information
   than any other object file format I'm aware of.  It is mandatory
   to be able to know if a symbol is an entry point, millicode, data,
   code, absolute, storage request, or procedure label.  If you get
   the symbol type wrong your program will not link.  */

void
bfd_som_set_symbol_type (symbol, type)
     asymbol *symbol;
     unsigned int type;
{
  (*som_symbol_data (symbol))->som_type = type;
}

/* Attach 64bits of unwind information to a symbol (which hopefully
   is a function of some kind!).  It would be better to keep this
   in the R_ENTRY relocation, but there is not enough space.  */

void
bfd_som_attach_unwind_info (symbol, unwind_desc)
     asymbol *symbol;
     char *unwind_desc;
{
  (*som_symbol_data (symbol))->unwind = unwind_desc;
}

/* Attach an auxiliary header to the BFD backend so that it may be
   written into the object file.  */
void
bfd_som_attach_aux_hdr (abfd, type, string)
     bfd *abfd;
     int type;
     char *string;
{
  if (type == VERSION_AUX_ID)
    {
      int len = strlen (string);

      if (len % 4)
	len += (4 - (len % 4));
      obj_som_version_hdr (abfd)
	= bfd_zalloc (abfd,
		      sizeof (struct aux_id) + sizeof (unsigned int) + len);
      obj_som_version_hdr (abfd)->header_id.type = VERSION_AUX_ID;
      obj_som_version_hdr (abfd)->header_id.length
	= sizeof (struct aux_id) + sizeof (unsigned int) + len;
      obj_som_version_hdr (abfd)->string_length = len;
      strcpy (obj_som_version_hdr (abfd)->user_string, string);
    }
  else if (type == COPYRIGHT_AUX_ID)
    {
      int len = strlen (string);

      if (len % 4)
	len += (4 - (len % 4));
      obj_som_copyright_hdr (abfd)
	= bfd_zalloc (abfd,
		      sizeof (struct aux_id) + sizeof (unsigned int) + len);
      obj_som_copyright_hdr (abfd)->header_id.type = COPYRIGHT_AUX_ID;
      obj_som_version_hdr (abfd)->header_id.length
	= sizeof (struct aux_id) + sizeof (unsigned int) + len;
      obj_som_copyright_hdr (abfd)->string_length = len;
      strcpy (obj_som_copyright_hdr (abfd)->copyright, string);
    }
  else
    abort ();
}

static boolean
som_set_section_contents (abfd, section, location, offset, count)
     bfd *abfd;
     sec_ptr section;
     PTR location;
     file_ptr offset;
     bfd_size_type count;
{
  if (abfd->output_has_begun == false)
    {
      /* Set up fixed parts of the file, space, and subspace headers.
	 Notify the world that output has begun.  */
      som_prep_headers (abfd);
      abfd->output_has_begun = true;
      /* Start writing the object file.  This include all the string
	 tables, fixup streams, and other portions of the object file.  */
      som_begin_writing (abfd);
    }

  /* Only write subspaces which have "real" contents (eg. the contents
     are not generated at run time by the OS).  */
  if (som_section_data (section)->is_subspace != 1
      || ((section->flags & (SEC_LOAD | SEC_DEBUGGING)) == 0))
    return true;

  /* Seek to the proper offset within the object file and write the
     data.  */
  offset += som_section_data (section)->subspace_dict.file_loc_init_value; 
  if (bfd_seek (abfd, offset, SEEK_SET) == -1)
    {
      bfd_error = system_call_error;
      return false;
    }

  if (bfd_write ((PTR) location, 1, count, abfd) != count)
    {
      bfd_error = system_call_error;
      return false;
    }
  return true;
}

static boolean
som_set_arch_mach (abfd, arch, machine)
     bfd *abfd;
     enum bfd_architecture arch;
     unsigned long machine;
{
  /* Allow any architecture to be supported by the SOM backend */
  return bfd_default_set_arch_mach (abfd, arch, machine);
}

static boolean
som_find_nearest_line (abfd, section, symbols, offset, filename_ptr,
			functionname_ptr, line_ptr)
     bfd *abfd;
     asection *section;
     asymbol **symbols;
     bfd_vma offset;
     CONST char **filename_ptr;
     CONST char **functionname_ptr;
     unsigned int *line_ptr;
{
  fprintf (stderr, "som_find_nearest_line unimplemented\n");
  fflush (stderr);
  abort ();
  return (false);
}

static int
som_sizeof_headers (abfd, reloc)
     bfd *abfd;
     boolean reloc;
{
  fprintf (stderr, "som_sizeof_headers unimplemented\n");
  fflush (stderr);
  abort ();
  return (0);
}

/* Return the single-character symbol type corresponding to
   SOM section S, or '?' for an unknown SOM section.  */

static char
som_section_type (s)
     const char *s;
{
  const struct section_to_type *t;

  for (t = &stt[0]; t->section; t++)
    if (!strcmp (s, t->section))
      return t->type;
  return '?';
}

static int
som_decode_symclass (symbol)
     asymbol *symbol;
{
  char c;

  if (bfd_is_com_section (symbol->section))
    return 'C';
  if (symbol->section == &bfd_und_section)
    return 'U';
  if (symbol->section == &bfd_ind_section)
    return 'I';
  if (!(symbol->flags & (BSF_GLOBAL|BSF_LOCAL)))
    return '?';

  if (symbol->section == &bfd_abs_section)
    c = 'a';
  else if (symbol->section)
    c = som_section_type (symbol->section->name);
  else
    return '?';
  if (symbol->flags & BSF_GLOBAL)
    c = toupper (c);
  return c;
}

/* Return information about SOM symbol SYMBOL in RET.  */

static void
som_get_symbol_info (ignore_abfd, symbol, ret)
     bfd *ignore_abfd;
     asymbol *symbol;
     symbol_info *ret;
{
  ret->type = som_decode_symclass (symbol);
  if (ret->type != 'U')
    ret->value = symbol->value+symbol->section->vma;
  else
    ret->value = 0;
  ret->name = symbol->name;
}

/* End of miscellaneous support functions. */

#define som_bfd_debug_info_start        bfd_void
#define som_bfd_debug_info_end          bfd_void
#define som_bfd_debug_info_accumulate   (PROTO(void,(*),(bfd*, struct sec *))) bfd_void

#define som_openr_next_archived_file    bfd_generic_openr_next_archived_file
#define som_generic_stat_arch_elt       bfd_generic_stat_arch_elt
#define som_slurp_armap                  bfd_false
#define som_slurp_extended_name_table    _bfd_slurp_extended_name_table
#define som_truncate_arname              (void (*)())bfd_nullvoidptr
#define som_write_armap                  0

#define som_get_lineno                   (struct lineno_cache_entry *(*)())bfd_nullvoidptr
#define	som_close_and_cleanup	           bfd_generic_close_and_cleanup
#define som_get_section_contents          bfd_generic_get_section_contents

#define som_bfd_get_relocated_section_contents \
 bfd_generic_get_relocated_section_contents
#define som_bfd_relax_section bfd_generic_relax_section
#define som_bfd_seclet_link bfd_generic_seclet_link
#define som_bfd_make_debug_symbol \
  ((asymbol *(*) PARAMS ((bfd *, void *, unsigned long))) bfd_nullvoidptr)

/* Core file support is in the hpux-core backend.  */
#define som_core_file_failing_command	_bfd_dummy_core_file_failing_command
#define som_core_file_failing_signal	_bfd_dummy_core_file_failing_signal
#define som_core_file_matches_executable_p	_bfd_dummy_core_file_matches_executable_p

bfd_target som_vec =
{
  "som",			/* name */
  bfd_target_som_flavour,
  true,				/* target byte order */
  true,				/* target headers byte order */
  (HAS_RELOC | EXEC_P |		/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | WP_TEXT | D_PAGED),
  (SEC_CODE | SEC_DATA | SEC_ROM | SEC_HAS_CONTENTS
   | SEC_ALLOC | SEC_LOAD | SEC_RELOC),		/* section flags */

/* leading_symbol_char: is the first char of a user symbol
   predictable, and if so what is it */
  0,
  ' ',				/* ar_pad_char */
  16,				/* ar_max_namelen */
  3,				/* minimum alignment */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
  bfd_getb32, bfd_getb_signed_32, bfd_putb32,
  bfd_getb16, bfd_getb_signed_16, bfd_putb16,	/* hdrs */
  {_bfd_dummy_target,
   som_object_p,		/* bfd_check_format */
   bfd_generic_archive_p,
   _bfd_dummy_target
  },
  {
    bfd_false,
    som_mkobject,
    _bfd_generic_mkarchive,
    bfd_false
  },
  {
    bfd_false,
    som_write_object_contents,
    _bfd_write_archive_contents,
    bfd_false,
  },
#undef som
  JUMP_TABLE (som),
  (PTR) 0
};

#endif /* HOST_HPPAHPUX || HOST_HPPABSD */
