#ifndef __A_OUT_GNU_H__
#define __A_OUT_GNU_H__

#include "target.h"		/* Figure out which target and host systems */

#define __GNU_EXEC_MACROS__

#ifndef __STRUCT_EXEC_OVERRIDE__

struct exec
{
  unsigned long a_info;		/* Use macros N_MAGIC, etc for access */
  unsigned a_text;		/* length of text, in bytes */
  unsigned a_data;		/* length of data, in bytes */
  unsigned a_bss;		/* length of uninitialized data area for file, in bytes */
  unsigned a_syms;		/* length of symbol table data in file, in bytes */
  unsigned a_entry;		/* start address */
  unsigned a_trsize;		/* length of relocation info for text, in bytes */
  unsigned a_drsize;		/* length of relocation info for data, in bytes */
};

#endif /* __STRUCT_EXEC_OVERRIDE__ */

/* these go in the N_MACHTYPE field */
/* These symbols could be defined by code from Suns...punt 'em */
#undef M_OLDSUN2
#undef M_68010
#undef M_68020
#undef M_SPARC
enum machine_type {
  M_OLDSUN2 = 0,
  M_68010 = 1,
  M_68020 = 2,
  M_SPARC = 3,
  /* skip a bunch so we don't run into any of sun's numbers */
  M_386 = 100,
  M_29K = 101,
};

#define N_MAGIC(exec) ((exec).a_info & 0xffff)
#define N_MACHTYPE(exec) ((enum machine_type)(((exec).a_info >> 16) & 0xff))
#define N_FLAGS(exec) (((exec).a_info >> 24) & 0xff)
#define N_SET_INFO(exec, magic, type, flags) \
	((exec).a_info = ((magic) & 0xffff) \
	 | (((int)(type) & 0xff) << 16) \
	 | (((flags) & 0xff) << 24))
#define N_SET_MAGIC(exec, magic) \
	((exec).a_info = (((exec).a_info & 0xffff0000) | ((magic) & 0xffff)))

#define N_SET_MACHTYPE(exec, machtype) \
	((exec).a_info = \
	 ((exec).a_info&0xff00ffff) | ((((int)(machtype))&0xff) << 16))

#define N_SET_FLAGS(exec, flags) \
	((exec).a_info = \
	 ((exec).a_info&0x00ffffff) | (((flags) & 0xff) << 24))

/* Code indicating object file or impure executable.  */
#define OMAGIC 0407
/* Code indicating pure executable.  */
#define NMAGIC 0410
/* Code indicating demand-paged executable.  */
#define ZMAGIC 0413

#define N_BADMAG(x)					\
 (N_MAGIC(x) != OMAGIC && N_MAGIC(x) != NMAGIC		\
  && N_MAGIC(x) != ZMAGIC)

#define _N_BADMAG(x)					\
 (N_MAGIC(x) != OMAGIC && N_MAGIC(x) != NMAGIC		\
  && N_MAGIC(x) != ZMAGIC)

#define _N_HDROFF(x) (1024 - sizeof (struct exec))

#define N_TXTOFF(x) \
 (N_MAGIC(x) == ZMAGIC ? _N_HDROFF((x)) + sizeof (struct exec) : sizeof (struct exec))

#define N_DATOFF(x) (N_TXTOFF(x) + (x).a_text)

#define N_TRELOFF(x) (N_DATOFF(x) + (x).a_data)

#define N_DRELOFF(x) (N_TRELOFF(x) + (x).a_trsize)

#define N_SYMOFF(x) (N_DRELOFF(x) + (x).a_drsize)

#define N_STROFF(x) (N_SYMOFF(x) + (x).a_syms)

/* Address of text segment in memory after it is loaded.  */
/* Don't load things at zero, it encourages zero-pointer bugs */
#ifndef TEXT_START_ADDR
#define	TEXT_START_ADDR 0x10000
#endif
#define N_TXTADDR(x) TEXT_START_ADDR

/* Address of data segment in memory after it is loaded.
   Note that it is up to you to define SEGMENT_SIZE
   on machines not listed here.  */
#ifndef SEGMENT_SIZE
#if defined(vax) || defined(hp300) || defined(pyr)
#define SEGMENT_SIZE page_size
#endif
#ifdef	sony
#define	SEGMENT_SIZE	0x2000
#endif	/* Sony.  */
#ifdef is68k
#define SEGMENT_SIZE 0x20000
#endif
#if defined(m68k) && defined(PORTAR)
#define PAGE_SIZE 0x400
#define SEGMENT_SIZE PAGE_SIZE
#endif
#endif

#define _N_SEGMENT_ROUND(x) (((x) + SEGMENT_SIZE - 1) & ~(SEGMENT_SIZE - 1))

#define _N_TXTENDADDR(x) (N_TXTADDR(x)+(x).a_text)

#ifndef N_DATADDR
#define N_DATADDR(x) \
    (N_MAGIC(x)==OMAGIC? (_N_TXTENDADDR(x)) \
     : (_N_SEGMENT_ROUND (_N_TXTENDADDR(x))))
#endif

/* Address of bss segment in memory after it is loaded.  */
#define N_BSSADDR(x) (N_DATADDR(x) + (x).a_data)

struct nlist {
  union {
    char *n_name;
    struct nlist *n_next;
    long n_strx;
  } n_un;
  unsigned char n_type;
  char n_other;
  short n_desc;
  unsigned long n_value;
};

#define N_UNDF 0
#define N_ABS 2
#define N_TEXT 4
#define N_DATA 6
#define N_BSS 8
#define N_FN 15

#define N_EXT 1
#define N_TYPE 036
#define N_STAB 0340

/* The following type indicates the definition of a symbol as being
   an indirect reference to another symbol.  The other symbol
   appears as an undefined reference, immediately following this symbol.

   Indirection is asymmetrical.  The other symbol's value will be used
   to satisfy requests for the indirect symbol, but not vice versa.
   If the other symbol does not have a definition, libraries will
   be searched to find a definition.  */
#define N_INDR 0xa

/* The following symbols refer to set elements.
   All the N_SET[ATDB] symbols with the same name form one set.
   Space is allocated for the set in the text section, and each set
   element's value is stored into one word of the space.
   The first word of the space is the length of the set (number of elements).

   The address of the set is made into an N_SETV symbol
   whose name is the same as the name of the set.
   This symbol acts like a N_DATA global symbol
   in that it can satisfy undefined external references.  */

/* These appear as input to LD, in a .o file.  */
#define	N_SETA	0x14		/* Absolute set element symbol */
#define	N_SETT	0x16		/* Text set element symbol */
#define	N_SETD	0x18		/* Data set element symbol */
#define	N_SETB	0x1A		/* Bss set element symbol */

/* This is output from LD.  */
#define N_SETV	0x1C		/* Pointer to set vector in data area.  */

/* This structure describes a single relocation to be performed.
   The text-relocation section of the file is a vector of these structures,
   all of which apply to the text section.
   Likewise, the data-relocation section applies to the data section.  */

#if TARGET == TARGET_SPARC || TARGET == TARGET_AM29K
/*
 * The following enum and struct were borrowed from
 * sunOS  /usr/include/sun4/a.out.h  and extended to handle
 * other machines.
 */

enum reloc_type
{
    RELOC_8,        RELOC_16,        RELOC_32,       RELOC_DISP8,
    RELOC_DISP16,   RELOC_DISP32,    RELOC_WDISP30,  RELOC_WDISP22,
    RELOC_HI22,     RELOC_22,        RELOC_13,       RELOC_LO10,
    RELOC_SFA_BASE, RELOC_SFA_OFF13, RELOC_BASE10,   RELOC_BASE13,
    RELOC_BASE22,   RELOC_PC10,      RELOC_PC22,     RELOC_JMP_TBL,
    RELOC_SEGOFF16, RELOC_GLOB_DAT,  RELOC_JMP_SLOT, RELOC_RELATIVE,

/* 29K relocation types */
    RELOC_JUMPTARG, RELOC_CONST,     RELOC_CONSTH,

    NO_RELOC
};

#define	RELOC_TYPE_NAMES \
"8",		"16",		"32",		"DISP8",	\
"DISP16",	"DISP32",	"WDISP30",	"WDISP22",	\
"HI22",		"22",		"13",		"LO10",		\
"SFA_BASE",	"SFAOFF13",	"BASE10",	"BASE13",	\
"BASE22",	"PC10",		"PC22",		"JMP_TBL",	\
"SEGOFF16",	"GLOB_DAT",	"JMP_SLOT",	"RELATIVE",	\
"JUMPTARG",	"CONST",	"CONSTH",			\
"NO_RELOC",							\
"XXX_28", "XXX_29", "XXX_30", "XXX_31"

struct reloc_info_extended
{
  unsigned long r_address;
  unsigned int  r_index:24;
# define	r_symbolnum  r_index
  unsigned	r_extern:1;
  unsigned	:2;
  enum reloc_type r_type:5;
  long int	r_addend;
};

/* Let programs know what they're dealing with */
#define	RELOC_EXTENDED			1

#undef relocation_info
#define relocation_info	                reloc_info_extended
#define RELOC_ADDRESS(r)		((r)->r_address)
#define RELOC_EXTERN_P(r)               ((r)->r_extern)
#define RELOC_TYPE(r)                   ((r)->r_index)
#define	RELOC_EXTENDED_TYPE(r)		((r)->r_type)
#define RELOC_SYMBOL(r)                 ((r)->r_index)
#define RELOC_MEMORY_SUB_P(r)		0
#define RELOC_MEMORY_ADD_P(r)           0
#define RELOC_ADD_EXTRA(r)              ((r)->r_addend)
#define RELOC_PCREL_P(r)             \
        (  ((r)->r_type >= RELOC_DISP8 && (r)->r_type <= RELOC_WDISP22) \
	 || (r)->r_type == RELOC_JUMPTARG )
#define RELOC_VALUE_RIGHTSHIFT(r)       (reloc_target_rightshift[(r)->r_type])
#define RELOC_TARGET_SIZE(r)            (reloc_target_size[(r)->r_type])
#define RELOC_TARGET_BITPOS(r)          0
#define RELOC_TARGET_BITSIZE(r)         (reloc_target_bitsize[(r)->r_type])

/* Note that these are very dependent on the order of the enums in
   enum reloc_type (in a.out.h); if they change the following must be
   changed */
/* Also note that some of these may be incorrect; I have no information */
#ifndef __STDC__
#define const	/**/
#endif
static const int reloc_target_rightshift[] = {
  0, 0, 0, 0,
  0, 0, 2, 2,
 10, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  0, 0, 0, 0,
  2, 0,16,	/* 29K jumptarg, const, consth */
  0,
};
#define RELOC_SIZE_SPLIT16	13
static const int reloc_target_size[] = {
  0, 1, 2, 0,
  1, 2, 2, 2,
  2, 2, 2, 2,
  2, 2, 2, 2,
  2, 2, 2, 2,
  2, 2, 2, 2,
  RELOC_SIZE_SPLIT16, RELOC_SIZE_SPLIT16, RELOC_SIZE_SPLIT16,
  0,
};
static const int reloc_target_bitsize[] = {
  8, 16, 32, 8,
 16, 32, 30, 22,
 22, 22, 13, 10,
 32, 32, 16,  0,
  0,  0,  0,  0,	/* dunno */
  0,  0,  0,  0,
 16, 16, 16,		/* 29K jumptarg, const, consth */
  0,
};

#define	MAX_ALIGNMENT	(sizeof (double))

#else  /* Not SPARC or AM29K */

struct relocation_info
{
  /* Address (within segment) to be relocated.  */
  int r_address;
  /* The meaning of r_symbolnum depends on r_extern.  */
  unsigned int r_symbolnum:24;
  /* Nonzero means value is a pc-relative offset
     and it should be relocated for changes in its own address
     as well as for changes in the symbol or section specified.  */
  unsigned int r_pcrel:1;
  /* Length (as exponent of 2) of the field to be relocated.
     Thus, a value of 2 indicates 1<<2 bytes.  */
  unsigned int r_length:2;
  /* 1 => relocate with value of symbol.
          r_symbolnum is the index of the symbol
	  in file's the symbol table.
     0 => relocate with the address of a segment.
          r_symbolnum is N_TEXT, N_DATA, N_BSS or N_ABS
	  (the N_EXT bit may be set also, but signifies nothing).  */
  unsigned int r_extern:1;
  /* Four bits that aren't used, but when writing an object file
     it is desirable to clear them.  */
  unsigned int r_pad:4;
};
#endif

/*
 * Ok.  Following are the relocation information macros.  If your
 * system should not be able to use the default set (below), you must
 * define the following:

 *   relocation_info: This must be typedef'd (or #define'd) to the type
 * of structure that is stored in the relocation info section of your
 * a.out files.  Often this is defined in the a.out.h for your system.
 *
 *   RELOC_ADDRESS (rval): Offset into the current section of the
 * <whatever> to be relocated.  *Must be an lvalue*.
 *
 *   RELOC_EXTERN_P (rval):  Is this relocation entry based on an
 * external symbol (1), or was it fully resolved upon entering the
 * loader (0) in which case some combination of the value in memory
 * (if RELOC_MEMORY_ADD_P) and the extra (if RELOC_ADD_EXTRA) contains
 * what the value of the relocation actually was.  *Must be an lvalue*.
 *
 *   RELOC_TYPE (rval): If this entry was fully resolved upon
 * entering the loader, what type should it be relocated as?
 *
 *   RELOC_EXTENDED_TYPE (rval): If this entry is for a machine using
 * extended relocatino, what type of field is it?  (For example, on RISC
 * machines, odd-sized displacements or split displacements occur.)
 *
 *   RELOC_SYMBOL (rval): If this entry was not fully resolved upon
 * entering the loader, what is the index of it's symbol in the symbol
 * table?  *Must be a lvalue*.
 *
 *   RELOC_MEMORY_ADD_P (rval): This should return true if the final
 * relocation value output here should be added to memory, or if the
 * section of memory described should simply be set to the relocation
 * value.
 *
 *   RELOC_ADD_EXTRA (rval): (Optional) This macro, if defined, gives
 * an extra value to be added to the relocation value based on the
 * individual relocation entry.  *Must be an lvalue if defined*.
 *
 *   RELOC_PCREL_P (rval): True if the relocation value described is
 * pc relative.
 *
 *   RELOC_VALUE_RIGHTSHIFT (rval): Number of bits right to shift the
 * final relocation value before putting it where it belongs.
 *
 *   RELOC_TARGET_SIZE (rval): log to the base 2 of the number of
 * bytes of size this relocation entry describes; 1 byte == 0; 2 bytes
 * == 1; 4 bytes == 2, and etc.  This is somewhat redundant (we could
 * do everything in terms of the bit operators below), but having this
 * macro could end up producing better code on machines without fancy
 * bit twiddling.  Also, it's easier to understand/code big/little
 * endian distinctions with this macro.
 *
 *   RELOC_TARGET_BITPOS (rval): The starting bit position within the
 * object described in RELOC_TARGET_SIZE in which the relocation value
 * will go.
 *
 *   RELOC_TARGET_BITSIZE (rval): How many bits are to be replaced
 * with the bits of the relocation value.  It may be assumed by the
 * code that the relocation value will fit into this many bits.  This
 * may be larger than RELOC_TARGET_SIZE if such be useful.
 *
 *
 *		Things I haven't implemented
 *		----------------------------
 *
 *    Values for RELOC_TARGET_SIZE other than 0, 1, or 2.
 *
 *    Pc relative relocation for External references.
 */
#if TARGET == TARGET_SEQUENT
#define RELOC_ADDRESS(r)		((r)->r_address)
#define RELOC_EXTERN_P(r)		((r)->r_extern)
#define RELOC_TYPE(r)		((r)->r_symbolnum)
#define RELOC_SYMBOL(r)		((r)->r_symbolnum)
#define RELOC_MEMORY_SUB_P(r)	((r)->r_bsr)
#define RELOC_MEMORY_ADD_P(r)	1
#undef RELOC_ADD_EXTRA
#define RELOC_PCREL_P(r)		((r)->r_pcrel || (r)->r_bsr)
#define RELOC_VALUE_RIGHTSHIFT(r)	0
#define RELOC_TARGET_SIZE(r)		((r)->r_length)
#define RELOC_TARGET_BITPOS(r)	0
#define RELOC_TARGET_BITSIZE(r)	32
#endif

/* Default macros */
#ifndef RELOC_ADDRESS
#define RELOC_ADDRESS(r)		((r)->r_address)
#define RELOC_EXTERN_P(r)		((r)->r_extern)
#define RELOC_TYPE(r)			((r)->r_symbolnum)
#define RELOC_SYMBOL(r)			((r)->r_symbolnum)
#define RELOC_MEMORY_SUB_P(r)		0
#define RELOC_MEMORY_ADD_P(r)		1
#undef RELOC_ADD_EXTRA
#define RELOC_PCREL_P(r)		((r)->r_pcrel)
#define RELOC_VALUE_RIGHTSHIFT(r)	0
#define RELOC_TARGET_SIZE(r)		((r)->r_length)
#define RELOC_TARGET_BITPOS(r)		0
#define RELOC_TARGET_BITSIZE(r)		32
#endif

/* Maximum alignment required of a common'd variable.  If a var of this
   size or larger is allocated in BSS when nobody defines it, it gets
   this alignment.  */

#ifndef MAX_ALIGNMENT
#define	MAX_ALIGNMENT	(sizeof (int))
#endif


/* Definitions for routines that read and write GNU a.out files */

enum objfile_kind {
	OBJFILE_ERROR,
	OBJFILE_UNKNOWN,
	OBJFILE_SINGLE,
	OBJFILE_ARCHIVE,
};

enum objfile_kind read_aout_header();	/* (desc, &header)  read&swap header */

/* Read an a.out header from DESC and call rel_fn(DESC, header)
   if it is an object file, lib_fn(DESC) if it is a library, else
   call err_fn("msg") */
void handle_aout_header();		/* (desc, rel_fn, lib_fn, err_fn) */

/* Byte-swapping definitions */

void swap_aoutheader();		/* BSD a.out header */
short swap_getshort ();
void swap_putshort();
long swap_getlong ();
void swap_putlong();
void swap_reloc_info_in();	/* BSD relocation information */
void swap_reloc_info_out();	/* BSD relocation information */
void swap_nlists();		/* BSD symbol name lists */
void swap_root_updates();	/* GDB Symseg */

/* Bring on the encapsulation, if configured in! */
#ifdef COFF_ENCAPSULATE
#include "a.out.encap.h"
#endif

#endif /* __A_OUT_GNU_H__ */
