/* coff.gnu.h
   Copyright (C) 1987 Free Software Foundation, Inc.
   
   This file is part of GAS, the GNU Assembler.
   
   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   
   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*** coff information for 80960.  Origins: Intel, AMD, etc., natch. */

/*
 * At this point I'm sure this file is right for i960 and I'm pretty sure it's
 * right for a29k, although it hasn't been tested rigorously.  Please feel free
 * to add your own machine's description here.  Without that info, it isn't
 * possible to build cross development tools from elsewhere nor is it easy to
 * continue to support your machines format.
 *
 * The TC_foo ifdef's are mine.  They are what gas uses.  The other ifdef's
 * remain for documentation from other scavenged files.   xoxorich.
 */

/* $Id$ */

/********************** FILE HEADER **********************/

struct filehdr {
	unsigned short	f_magic;	/* magic number			*/
	unsigned short	f_nscns;	/* number of sections		*/
	long		f_timdat;	/* time & date stamp		*/
	long		f_symptr;	/* file pointer to symtab	*/
	long		f_nsyms;	/* number of symtab entries	*/
	unsigned short	f_opthdr;	/* sizeof(optional hdr)		*/
	unsigned short	f_flags;	/* flags			*/
};

/* Bits for f_flags:
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable (no unresolved externel references)
 *	F_LNNO		line nunbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_AR32WR	file has byte ordering of an AR32WR machine (e.g. vax)
 */
#define F_RELFLG	(0x0001)
#define F_EXEC		(0x0002)
#define F_LNNO		(0x0004)
#define F_LSYMS		(0x0008)

#ifdef TC_I960
#define F_AR32WR	(0x0010) /* File has 32 bits per word, least
				    significant byte first. */
#else /* TC_I960 */
#define F_AR32WR	(0x0100)
#endif /* TC_I960 */

#define F_MINMAL	(0x0010) /* ??? */
#define F_UPDATE	(0x0020) /* ??? */
#define F_SWABD		(0x0040) /* ??? */
#define F_AR16WR	(0x0080) /* File has the byte ordering used by
				    the PDP*-11/70 processor. */
#define F_AR32W		(0x0200) /* File has 32 bits per word, most
				    significant byte first. */

/*
 *	Intel 80960 (I960) processor flags.
 *	F_I960TYPE == mask for processor type field. 
 */

#define	F_I960TYPE	(0xf000)
#define	F_I960CORE	(0x1000)
#define	F_I960KB	(0x2000)
#define	F_I960SB	(0x2000)
#define	F_I960MC	(0x3000)
#define	F_I960XA	(0x4000)
#define	F_I960CA	(0x5000)
#define	F_I960KA	(0x6000)
#define	F_I960SA	(0x6000)

/*
 * i80960 Magic Numbers
 */

#define I960ROMAGIC	(0x160)	/* read-only text segments */
#define I960RWMAGIC	(0x161)	/* read-write text segments */

#define I960BADMAG(x) (((x).f_magic != I960ROMAGIC) && ((x).f_magic != I960RWMAGIC))

#define	SIPFBOMAGIC	(0x17a)	/* Am29000 (Byte 0 is MSB - Big Endian) */
#define	SIPRBOMAGIC	(0x17b)	/* Am29000 (Byte 0 is LSB - Little Endian) */

#define A29KBADMAG(x)	(((x).f_magic != SIPFBOMAGIC) && ((x).f_magic != SIPRBOMAGIC))

#define	FILHDR	struct filehdr
#define	FILHSZ	sizeof(FILHDR)


/********************** AOUT "OPTIONAL HEADER" **********************/

typedef struct {
	unsigned long	phys_addr;
	unsigned long	bitarray;
} TAGBITS;

/*  These appear to be used only by exec(2).  I don't know who cares
    about them in a cross development environment.  In any case, this
    is my collection after researching the issue for a few hours.
    Apparently, most have these have remained essentially unchanged
    since v7 days, although a few new ones have been added.  xoxorich. */

#define BAD0MAGIC	(0401) /* (?) "lpd (UNIX/RT)" */
#define BAD1MAGIC	(0405) /* (?) overlay */
#define	OMAGIC		(0407) /* old impure format. data immediately
				  follows text. both sections are rw. */
#define	NMAGIC		(0410) /* split i&d, read-only text */
#define A_MAGIC3	(0411) /* (?) "separated I&D" */
#define	ZMAGIC		(0413) /* like NMAGIC, but demand loaded */
#define PAGEMAGIC2	(0414) /* (?) like ZMAGIC, but address zero
				  explicitly unmapped. */
#define REGMAGIC	(0414) /* (?) a PAGEMAGIC2 alias? */
#define PAGEMAGIC3	(0415) /* (?) like ZMAGIC, but address zero mapped. */
#define A_MAGIC5	(0437) /* (?) "system overlay, separated I&D" */
/* intended for non-unix cross development */
#define SASMAGIC	(010000) /* Single Address Space */    
#define MASMAGIC	(020000) /* (?) "Multiple (separate I & D) Address Spaces" */

typedef	struct aouthdr {
	short		magic;	/* type of file				*/
	short		vstamp;	/* version stamp			*/
	unsigned long	tsize;	/* text size in bytes, padded to FW bdry*/
	unsigned long	dsize;	/* initialized data "  "		*/
	unsigned long	bsize;	/* uninitialized data "   "		*/
#if U3B
	unsigned long	dum1;
	unsigned long	dum2;	/* pad to entry point	*/
#endif
	unsigned long	entry;	/* entry pt.				*/
	unsigned long	text_start;	/* base of text used for this file */
	unsigned long	data_start;	/* base of data used for this file */
	/* CAREFUL: some formats omit the tagentries member. */
	unsigned long	tagentries;	/* number of tag entries to
					   follow (always zero for i960) */
} AOUTHDR;

/* return a pointer to the tag bits array */

#define TAGPTR(aout) ((TAGBITS *) (&(aout.tagentries)+1))

/* compute size of a header */

/*#define AOUTSZ(aout) (sizeof(AOUTHDR)+(aout.tagentries*sizeof(TAGBITS)))*/
#define AOUTSZ (sizeof(AOUTHDR))


/********************** STORAGE CLASSES **********************/

#define C_EFCN		-1	/* physical end of function */
#define C_NULL		0
#define C_AUTO		1	/* automatic variable */
#define C_EXT		2	/* external symbol */
#define C_STAT		3	/* static */
#define C_REG		4	/* register variable */
#define C_EXTDEF	5	/* external definition */
#define C_LABEL		6	/* label */
#define C_ULABEL	7	/* undefined label */
#define C_MOS		8	/* member of structure */
#define C_ARG		9	/* function argument */
#define C_STRTAG	10	/* structure tag */
#define C_MOU		11	/* member of union */
#define C_UNTAG		12	/* union tag */
#define C_TPDEF		13	/* type definition */
#define C_USTATIC	14	/* undefined static */
#define C_ENTAG		15	/* enumeration tag */
#define C_MOE		16	/* member of enumeration */
#define C_REGPARM	17	/* register parameter */
#define C_FIELD		18	/* bit field */

#ifdef TC_I960
#define C_AUTOARG	19	/* auto argument */
#define C_LASTENT	20	/* dummy entry (end of block) */
#endif /* TC_I960 */

#ifdef TC_A29K
#define C_GLBLREG	19		/* global register */
#define C_EXTREG	20		/* external global register */
#define	C_DEFREG	21		/* ext. def. of global register */
#define C_STARTOF	22		/* as29 $SIZEOF and $STARTOF symbols */
#endif /* TC_A29K */

#define C_BLOCK		100	/* ".bb" or ".eb" */
#define C_FCN		101	/* ".bf" or ".ef" */
#define C_EOS		102	/* end of structure */
#define C_FILE		103	/* file name */
#define C_LINE		104	/* line # reformatted as symbol table entry */
#define C_ALIAS	 	105	/* duplicate tag */
#define C_HIDDEN	106	/* ext symbol in dmert public lib. like static,
				   used to avoid name conflicts. */

#ifdef TC_I960
/* New storage classes for 80960 */
#define C_SCALL		107	/* Procedure reachable via system call	*/
/* C_LEAFPROC is obsolete.  Use C_LEAFEXT or C_LEAFSTAT */
#define C_LEAFPROC	108	/* Leaf procedure, "call" via BAL */
#define C_LEAFEXT       108
#define C_OPTVAR	109	/* Optimized variable */
#define C_DEFINE	110	/* Preprocessor #define */
#define C_PRAGMA	111	/* Advice to compiler or linker	*/
#define C_SEGMENT	112	/* 80960 segment name */
#define C_LEAFSTAT      113     /* Static leaf */
#endif /* TC_I960 */

#ifdef TC_A29K
#define C_SHADOW	107 /* shadow symbol */
#endif /* TC_A29K */

/********************** SECTION HEADER **********************/

struct scnhdr {
	char		s_name[8];	/* section name */
	long		s_paddr;	/* physical address, aliased s_nlib */
	long		s_vaddr;	/* virtual address */
	long		s_size;		/* section size */
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation */
	long		s_lnnoptr;	/* file ptr to line numbers */
	unsigned short	s_nreloc;	/* number of relocation entries	*/
	unsigned short	s_nlnno;	/* number of line number entries */
	long		s_flags;	/* flags */
	
#ifdef TC_I960
	unsigned long	s_align;	/* section alignment */
#endif /* TC_I960 */
};

#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

/*
 * names of "special" sections
 */
#define _TEXT	".text"
#define _DATA	".data"
#define _BSS	".bss"

/*
 * s_flags "type"
 */

/*
 * In instances where it is necessary for a linker to
 * produce an output file which contains text or data not
 * based at virtual address 0, e.g. for a ROM, then the
 * linker should accept address base information as command
 * input and use PAD sections to skip over unused addresses.
 * (at least for a29k.  Maybe others.)
 */

#define STYP_REG	 (0x0000) /* "regular" section: allocated, relocated, loaded */
#define STYP_DSECT	 (0x0001) /* "dummy" section: not allocated, relocated, not loaded */
#define STYP_NOLOAD	 (0x0002) /* "noload" section: allocated, relocated, not loaded */
#define STYP_GROUP	 (0x0004) /* "grouped" section: formed of input sections */
#define STYP_PAD	 (0x0008) /* "padding" section: not allocated, not relocated, loaded */
#define STYP_COPY	 (0x0010) /* "copy" section: for decision function used by field update;  not allocated, not relocated,
				     loaded; reloc & lineno entries processed normally */
#define STYP_TEXT	 (0x0020) /* section contains text only */
#define S_SHRSEG	 (0x0020) /* In 3b Update files (output of ogen), sections which appear in SHARED segments of the Pfile
				     will have the S_SHRSEG flag set by ogen, to inform dufr that updating 1 copy of the proc. will
				     update all process invocations. */
#define STYP_DATA	 (0x0040) /* section contains data only */
#define STYP_BSS	 (0x0080) /* section contains bss only */
#define S_NEWFCN	 (0x0100) /* In a minimal file or an update file, a new function (as compared with a replaced function) */
#define STYP_INFO	 (0x0200) /* comment section : not allocated not relocated, not loaded */
#define STYP_OVER	 (0x0400) /* overlay section : relocated not allocated or loaded */
#define STYP_LIB	 (0x0800) /* for .lib section : same as INFO */
#define STYP_MERGE	 (0x2000) /* merge section -- combines with text, data or bss sections only */
#define STYP_REVERSE_PAD (0x4000) /* section will be padded with no-op instructions wherever padding is necessary and there is a
				     word of contiguous bytes beginning on a word boundary. */

#ifdef TC_A29K
/* NOTE:  The use of STYP_BSSREG for relocation is not yet defined. */
#define	STYP_BSSREG	0x1200	/* Global register area (like STYP_INFO) */
#define STYP_ENVIR	0x2200	/* Environment (like STYP_INFO) */
#define STYP_ABS	0x4000	/* Absolute (allocated, not reloc, loaded) */
#define STYP_LIT	0x8020	/* Literal data (like STYP_TEXT) */
#endif /* TC_A29K */

/********************** LINE NUMBERS **********************/

/* 1 line number entry for every "breakpointable" source line in a section.
 * Line numbers are grouped on a per function basis; first entry in a function
 * grouping will have l_lnno = 0 and in place of physical address will be the
 * symbol table index of the function name.
 */
struct lineno {
	union {
		long l_symndx;	/* symbol index of function name, iff l_lnno == 0*/
		long l_paddr;	/* (physical) address of line number */
	} l_addr;
	unsigned short	l_lnno;	/* line number */
#ifdef TC_I960
	/* not used on a29k */
	char padding[2];	/* force alignment */
#endif /* TC_I960 */
};

#define	LINENO	struct lineno
#define	LINESZ	sizeof(LINENO) 


/********************** SYMBOLS **********************/

#define SYMNMLEN	8	/* # characters in a symbol name */
#define FILNMLEN	14	/* # characters in a file name */
#define DIMNUM		4	/* # array dimensions in auxiliary entry */

struct syment {
	union {
		char	_n_name[SYMNMLEN];	/* old COFF version */
		struct {
			long	_n_zeroes;	/* new == 0 */
			long	_n_offset;	/* offset into string table */
		} _n_n;
		char	*_n_nptr[2];	/* allows for overlaying */
	} _n;
	long		n_value;	/* value of symbol */
	short		n_scnum;	/* section number */
	
#ifdef TC_I960
	/* This isn't yet used on the i960.  In some formats this
	   is two bytes of padding.  In others, it is missing entirely. */
	unsigned short	n_flags;	/* copy of flags from filhdr */
#endif /* TC_I960 */
	
#ifdef TC_A29K
	unsigned short	n_type;		/* type and derived type */
#else /* TC_A29K */
	/* at least i960 uses long */
	unsigned long	n_type;		/* type and derived type */
#endif /* TC_A29K */
	
	char		n_sclass;	/* storage class */
	char		n_numaux;	/* number of aux. entries */
	
#ifndef TC_A29K
	char		pad2[2];	/* force alignment */
#endif /* TC_A29K */
};

#define	SYMENT	struct syment
#define	SYMESZ	sizeof(SYMENT) /* This had better also be sizeof(AUXENT) */

#define n_name		_n._n_name
#define n_ptr		_n._n_nptr[1]
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset
    
    /*
     * Relocatable symbols have number of the section in which they are defined,
     * or one of the following:
     */
    
#define N_SCNUM	((short) 1-65535) /* section num where symbol defined */
#define N_UNDEF	((short)0)  /* undefined symbol */
#define N_ABS	((short)-1) /* value of symbol is absolute */
#define N_DEBUG	((short)-2) /* debugging symbol -- symbol value is meaningless */
#define N_TV	((short)-3) /* indicates symbol needs preload transfer vector */
#define P_TV	((short)-4) /* indicates symbol needs transfer vector (postload) */

/*
 * Type of a symbol, in low 4 bits of the word
 */
#define T_NULL		0 /* type not assigned */
#define T_VOID		1 /* function argument (only used by compiler) (but now real void). */
#define T_CHAR		2 /* character */
#define T_SHORT		3 /* short integer */
#define T_INT		4 /* integer */
#define T_LONG		5 /* long integer */
#define T_FLOAT		6 /* floating point */
#define T_DOUBLE	7 /* double word */
#define T_STRUCT	8 /* structure */
#define T_UNION		9 /* union */
#define T_ENUM		10 /* enumeration */
#define T_MOE		11 /* member of enumeration */
#define T_UCHAR		12 /* unsigned character */
#define T_USHORT	13 /* unsigned short */
#define T_UINT		14 /* unsigned integer */
#define T_ULONG		15 /* unsigned long */

#ifdef TC_I960
#define T_LNGDBL	16	/* long double */
#endif /* TC_I960 */

/*
 * derived types, in n_type
 */
#define DT_NON		(0)	/* no derived type */
#define DT_PTR		(1)	/* pointer */
#define DT_FCN		(2)	/* function */
#define DT_ARY		(3)	/* array */

#ifndef TC_I960

#define N_BTMASK	(0x0f)
#define N_TMASK		(0x30)
#define N_BTSHFT	(4)
#define N_TSHIFT	(2)

#else /* TC_I960 */

#define N_BTMASK	(0x1f)
#define N_TMASK		(0x60)
#define N_BTSHFT	(5)
#define N_TSHIFT	(2)

#endif /* TC_I960 */

#define BTYPE(x)	((x) & N_BTMASK)

#define ISPTR(x)	(((x) & N_TMASK) == (DT_PTR << N_BTSHFT))
#define ISFCN(x)	(((x) & N_TMASK) == (DT_FCN << N_BTSHFT))
#define ISARY(x)	(((x) & N_TMASK) == (DT_ARY << N_BTSHFT))

#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

union auxent {
	struct {
		long x_tagndx;	/* str, un, or enum tag indx */
		union {
			struct {
				unsigned short x_lnno; /* declaration line number */
				unsigned short x_size; /* str/union/array size */
			} x_lnsz;
			long x_fsize;	/* size of function */
		} x_misc;
		union {
			struct {		/* if ISFCN, tag, or .bb */
				long x_lnnoptr;	/* ptr to fcn line # */
				long x_endndx;	/* entry ndx past block end */
			} x_fcn;
			struct {		/* if ISARY, up to 4 dimen. */
				unsigned short x_dimen[DIMNUM];
			} x_ary;
		} x_fcnary;
		unsigned short x_tvndx;		/* tv index */
	} x_sym;
	
	/* This was just a struct x_file with x_fname only in a29k.  xoxorich. */
	union {
		char x_fname[FILNMLEN];
		struct {
			long x_zeroes;
			long x_offset;
		} x_n;
	} x_file;
	
	struct {
		long x_scnlen;			/* section length */
		unsigned short x_nreloc;	/* # relocation entries */
		unsigned short x_nlinno;	/* # line numbers */
	} x_scn;
	
	struct {
		long		x_tvfill;	/* tv fill value */
		unsigned short	x_tvlen;	/* length of .tv */
		
		/* This field was typo'd x_tvrna on a29k. xoxorich. */
		unsigned short	x_tvran[2];	/* tv range */
	} x_tv;		/* info about .tv section (in auxent of symbol .tv)) */
	
#ifdef TC_I960
	/******************************************
	 *  I960-specific *2nd* aux. entry formats
	 ******************************************/
	struct {
		/* This is a very old typo that keeps getting propogated. */
#define x_stdindx x_stindx
		long x_stindx;	/* sys. table entry */
	} x_sc;	/* system call entry */
	
	struct {
		unsigned long x_balntry; /* BAL entry point */
	} x_bal; /* BAL-callable function */
	
	struct {
		unsigned long	x_timestamp;	        /* time stamp */
		char 	x_idstring[20];	        /* producer identity string */
	} x_ident;	                        /* Producer ident info */
	
	char a[sizeof(struct syment)];	/* force auxent/syment sizes to match */
#endif /* TC_I960 */
};

#define	AUXENT	union auxent
#define	AUXESZ	sizeof(AUXENT) /* This had better also be sizeof(SYMENT) */

#if VAX || I960
#	define _ETEXT	"_etext"
#else
#	define _ETEXT	"etext"
#endif

/********************** RELOCATION DIRECTIVES **********************/

struct reloc {
	long r_vaddr;		/* Virtual address of reference */
	long r_symndx;		/* Index into symbol table */
	unsigned short r_type;	/* Relocation type */
#ifdef TC_I960
	/* not used for a29k */
	char pad[2];		/* Unused */
#endif /* TC_I960 */
};

#define RELOC struct reloc
#define RELSZ sizeof(RELOC)

#ifdef TC_I960
#define R_RELLONG	(0x11)	/* Direct 32-bit relocation */
#define R_IPRSHORT	(0x18)
#define R_IPRMED 	(0x19)	/* 24-bit ip-relative relocation */
#define R_IPRLONG	(0x1a)
#define R_OPTCALL	(0x1b)	/* 32-bit optimizable call (leafproc/sysproc) */
#define R_OPTCALLX	(0x1c)	/* 64-bit optimizable call (leafproc/sysproc) */
#define R_GETSEG	(0x1d)
#define R_GETPA		(0x1e)
#define R_TAGWORD	(0x1f)
#endif /* TC_I960 */

#ifdef TC_A29K
/*
 * NOTE: All the "I" forms refer to Am29000 instruction
 * formats.  The linker is expected to know how the numeric
 * information is split and/or aligned within the
 * instruction word(s).  R_BYTE works for instructions, too.
 *
 * If the parameter to a CONSTH instruction is a relocatable
 * type, two relocation records are written.  The first has
 * an r_type of R_IHIHALF (33 octal) and a normal r_vaddr
 * and r_symndx.  The second relocation record has an r_type
 * of R_IHCONST (34 octal), a normal r_vaddr (which is
 * redundant), and an r_symndx containing the 32-bit
 * constant offset to the relocation instead of the actual
 * symbol table index.  This second record is always
 * written, even if the constant offset is zero.  The
 * constant fields of the instruction are set to zero.
 */

#define	R_ABS		(0x00) /* reference is absolute */
#define	R_IREL		(0x18) /* instruction relative (jmp/call) */
#define	R_IABS		(0x19) /* instruction absolute (jmp/call) */
#define	R_ILOHALF	(0x1a) /* instruction low half  (const)  */
#define	R_IHIHALF	(0x1b) /* instruction high half (consth) part 1 */
#define	R_IHCONST	(0x1c) /* instruction high half (consth) part 2
				  constant offset of R_IHIHALF relocation */
#define	R_BYTE		(0x1d) /* relocatable byte value */
#define R_HWORD		(0x1e) /* relocatable halfword value */
#define R_WORD		(0x1f) /* relocatable word value */
#define	R_IGLBLRC	(0x20) /* instruction global register RC */
#define	R_IGLBLRA	(0x21) /* instruction global register RA */
#define	R_IGLBLRB	(0x22) /* instruction global register RB */
#endif /* TC_A29K */


#define DEFAULT_DATA_SECTION_ALIGNMENT 4
#define DEFAULT_BSS_SECTION_ALIGNMENT 4
#define DEFAULT_TEXT_SECTION_ALIGNMENT 16
/* For new sections we haven't heard of before */
#define DEFAULT_SECTION_ALIGNMENT 4

/*
 * Local Variables:
 * comment-column: 0
 * End:
 */

/* end of coff.gnu.h */
