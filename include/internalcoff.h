/********************** FILE HEADER **********************/
struct internal_filehdr 
{
  unsigned short	f_magic; /* magic number			*/
  unsigned short	f_nscns; /* number of sections		*/
  long		f_timdat;	/* time & date stamp		*/
  long		f_symptr;	/* file pointer to symtab	*/
  long		f_nsyms;	/* number of symtab entries	*/
  unsigned short	f_opthdr; /* sizeof(optional hdr)		*/
  unsigned short	f_flags; /* flags			*/
};

/* Bits for f_flags:
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable (no unresolved external references)
 *	F_LNNO		line numbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_AR32WR	file has byte ordering of an AR32WR machine (e.g. vax)
 */

#define F_RELFLG	(0x0001)
#define F_EXEC		(0x0002)
#define F_LNNO		(0x0004)
#define F_LSYMS		(0x0008)
#define F_AR32WR	(0x0010)
#define F_AR32W     0x200
/********************** AOUT "OPTIONAL HEADER" **********************/
struct internal_aouthdr {
	short		magic;	/* type of file				*/
	short		vstamp;	/* version stamp			*/
	unsigned long	tsize;	/* text size in bytes, padded to FW bdry*/
	unsigned long	dsize;	/* initialized data "  "		*/
	unsigned long	bsize;	/* uninitialized data "   "		*/
	unsigned long	entry;	/* entry pt.				*/
	unsigned long	text_start;	/* base of text used for this file */
	unsigned long	data_start;	/* base of data used for this file */
	unsigned long	tagentries;	/* number of tag entries to follow */
} ;

/********************** STORAGE CLASSES **********************/

#define C_EFCN		-1	/* physical end of function	*/
#define C_NULL		0
#define C_AUTO		1	/* automatic variable		*/
#define C_EXT		2	/* external symbol		*/
#define C_STAT		3	/* static			*/
#define C_REG		4	/* register variable		*/
#define C_EXTDEF	5	/* external definition		*/
#define C_LABEL		6	/* label			*/
#define C_ULABEL	7	/* undefined label		*/
#define C_MOS		8	/* member of structure		*/
#define C_ARG		9	/* function argument		*/
#define C_STRTAG	10	/* structure tag		*/
#define C_MOU		11	/* member of union		*/
#define C_UNTAG		12	/* union tag			*/
#define C_TPDEF		13	/* type definition		*/
#define C_USTATIC	14	/* undefined static		*/
#define C_ENTAG		15	/* enumeration tag		*/
#define C_MOE		16	/* member of enumeration	*/
#define C_REGPARM	17	/* register parameter		*/
#define C_FIELD		18	/* bit field			*/
#define C_AUTOARG	19	/* auto argument		*/
#define C_LASTENT	20	/* dummy entry (end of block)	*/
#define C_BLOCK		100	/* ".bb" or ".eb"		*/
#define C_FCN		101	/* ".bf" or ".ef"		*/
#define C_EOS		102	/* end of structure		*/
#define C_FILE		103	/* file name			*/
#define C_LINE		104	/* line # reformatted as symbol table entry */
#define C_ALIAS	 	105	/* duplicate tag		*/
#define C_HIDDEN	106	/* ext symbol in dmert public lib */

	/* New storage classes for 80960 */
 
/* C_LEAFPROC is obsolete.  Use C_LEAFEXT or C_LEAFSTAT */
#define C_LEAFPROC	108	/* Leaf procedure, "call" via BAL */

#define C_SCALL		107	/* Procedure reachable via system call */
#define C_LEAFEXT       108	/* External leaf */
#define C_LEAFSTAT      113     /* Static leaf */
#define C_OPTVAR	109	/* Optimized variable		*/
#define C_DEFINE	110	/* Preprocessor #define		*/
#define C_PRAGMA	111	/* Advice to compiler or linker	*/
#define C_SEGMENT	112	/* 80960 segment name		*/

/********************** SECTION HEADER **********************/
struct internal_scnhdr {
	char		s_name[8];	/* section name			*/
	long		s_paddr;	/* physical address, aliased s_nlib */
	long		s_vaddr;	/* virtual address		*/
	long		s_size;		/* section size			*/
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation	*/
	long		s_lnnoptr;	/* file ptr to line numbers	*/
#ifdef M88
	unsigned long 	s_nreloc;
	unsigned long   s_nlnno;
#else
	unsigned short	s_nreloc;	/* number of relocation entries	*/
	unsigned short	s_nlnno;	/* number of line number entries*/
#endif
	long		s_flags;	/* flags			*/
#ifdef I960
	long		s_align;	
#endif
};


/*
 * names of "special" sections
 */
#define _TEXT	".text"
#define _DATA	".data"
#define _BSS	".bss"

/*
 * s_flags "type"
 */
#define STYP_REG	 (0x0000) /* "regular": allocated, relocated, loaded */
#define STYP_DSECT	 (0x0001) /* "dummy":  relocated only*/
#define STYP_NOLOAD	 (0x0002) /* "noload": allocated, relocated, not loaded */
#define STYP_GROUP	 (0x0004) /* "grouped": formed of input sections */
#define STYP_PAD	 (0x0008) /* "padding": not allocated, not relocated, loaded */
#define STYP_COPY	 (0x0010) /* "copy": for decision function used by field update;  not allocated, not relocated,
				     loaded; reloc & lineno entries processed normally */
#define STYP_TEXT	 (0x0020) /* section contains text only */
#define S_SHRSEG	 (0x0020) /* In 3b Update files (output of ogen), sections which appear in SHARED segments of the Pfile
				     will have the S_SHRSEG flag set by ogen, to inform dufr that updating 1 copy of the proc. will
				     update all process invocations. */
#define STYP_DATA	 (0x0040) /* section contains data only */
#define STYP_BSS	 (0x0080) /* section contains bss only */
#define S_NEWFCN	 (0x0100) /* In a minimal file or an update file, a new function (as compared with a replaced function) */
#define STYP_INFO	 (0x0200) /* comment: not allocated not relocated, not loaded */
#define STYP_OVER	 (0x0400) /* overlay: relocated not allocated or loaded */
#define STYP_LIB	 (0x0800) /* for .lib: same as INFO */
#define STYP_MERGE	 (0x2000) /* merge section -- combines with text, data or bss sections only */
#define STYP_REVERSE_PAD (0x4000) /* section will be padded with no-op instructions wherever padding is necessary and there is a
				     word of contiguous bytes beginning on a word boundary. */
/********************** LINE NUMBERS **********************/

/* 1 line number entry for every "breakpointable" source line in a section.
 * Line numbers are grouped on a per function basis; first entry in a function
 * grouping will have l_lnno = 0 and in place of physical address will be the
 * symbol table index of the function name.
 */

struct internal_lineno {
	union {
		long l_symndx;	/* function name symbol index, iff l_lnno == 0*/
		long l_paddr;	/* (physical) address of line number	*/
	} l_addr;
#ifdef M88
	unsigned long	l_lnno;	/* line number		*/
#else
	unsigned short	l_lnno;	/* line number		*/
#endif
};


/********************** SYMBOLS **********************/

struct internal_syment {
	union {
		char	_n_name[SYMNMLEN];	/* old COFF version	*/
		struct {
			long	_n_zeroes;	/* new == 0		*/
			long	_n_offset;	/* offset into string table */
		} _n_n;
		char	*_n_nptr[2];	/* allows for overlaying	*/
	} _n;
	long		n_value;	/* value of symbol		*/
	short		n_scnum;	/* section number		*/
	unsigned short	n_flags;	/* copy of flags from filhdr	*/
	unsigned short	n_type;		/* type and derived type	*/
#if __STDC__
	signed
#endif
	char		n_sclass;	/* storage class		*/
	char		n_numaux;	/* number of aux. entries	*/
};

/* Relocatable symbols have number of the section in which they are defined,
   or one of the following: */

#define N_UNDEF	((short)0)  /* undefined symbol */
#define N_ABS	((short)-1) /* value of symbol is absolute */
#define N_DEBUG	((short)-2) /* debugging symbol -- value is meaningless */
#define N_TV	((short)-3) /* indicates symbol needs preload transfer vector */
#define P_TV	((short)-4) /* indicates symbol needs postload transfer vector*/

/*
 * Type of a symbol, in low N bits of the word
 */
#define T_NULL		0
#define T_VOID		1	/* function argument (only used by compiler) */
#define T_CHAR		2	/* character		*/
#define T_SHORT		3	/* short integer	*/
#define T_INT		4	/* integer		*/
#define T_LONG		5	/* long integer		*/
#define T_FLOAT		6	/* floating point	*/
#define T_DOUBLE	7	/* double word		*/
#define T_STRUCT	8	/* structure 		*/
#define T_UNION		9	/* union 		*/
#define T_ENUM		10	/* enumeration 		*/
#define T_MOE		11	/* member of enumeration*/
#define T_UCHAR		12	/* unsigned character	*/
#define T_USHORT	13	/* unsigned short	*/
#define T_UINT		14	/* unsigned integer	*/
#define T_ULONG		15	/* unsigned long	*/
#define T_LNGDBL	16	/* long double		*/

/*
 * derived types, in n_type
*/
#define DT_NON		(0)	/* no derived type */
#define DT_PTR		(1)	/* pointer */
#define DT_FCN		(2)	/* function */
#define DT_ARY		(3)	/* array */

#define BTYPE(x)	((x) & N_BTMASK)

#define ISPTR(x)	(((x) & N_TMASK) == (DT_PTR << N_BTSHFT))
#define ISFCN(x)	(((x) & N_TMASK) == (DT_FCN << N_BTSHFT))
#define ISARY(x)	(((x) & N_TMASK) == (DT_ARY << N_BTSHFT))
#define ISTAG(x)	((x)==C_STRTAG||(x)==C_UNTAG||(x)==C_ENTAG)
#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))


union internal_auxent 
{
  struct {
    union {
      long l;			/* str, un, or enum tag indx */
      struct coff_ptr_struct *p;
    } x_tagndx;

    union {
      struct {
	unsigned short x_lnno;	/* declaration line number */
	unsigned short x_size;	/* str/union/array size */
      } x_lnsz;
      long x_fsize;		/* size of function */
    } x_misc;
    union {

      struct {			/* if ISFCN, tag, or .bb */
	long x_lnnoptr;		/* ptr to fcn line # */
	union { 		/* entry ndx past block end */
	  long  l;
	  struct coff_ptr_struct *p;

	} x_endndx;


      } x_fcn;
      struct {			/* if ISARY, up to 4 dimen. */
	unsigned short x_dimen[DIMNUM];
      } x_ary;
    } x_fcnary;
    unsigned short x_tvndx;	/* tv index */
  } x_sym;

  union {
    char x_fname[FILNMLEN];
    struct {
      long x_zeroes;
      long x_offset;
    } x_n;
  } x_file;

  struct {
    long x_scnlen;		/* section length */
    unsigned short x_nreloc;	/* # relocation entries */
    unsigned short x_nlinno;	/* # line numbers */
  } x_scn;

  struct {
    long		x_tvfill; /* tv fill value */
    unsigned short	x_tvlen; /* length of .tv */
    unsigned short	x_tvran[2]; /* tv range */
  } x_tv;			/* info about .tv section (in auxent of symbol .tv)) */

  /******************************************
   *  I960-specific *2nd* aux. entry formats
   ******************************************/
  struct {
    /* This is a very old typo that keeps getting propagated. */
#define x_stdindx x_stindx
    long x_stindx;		/* sys. table entry */
  } x_sc;			/* system call entry */

  struct {
    unsigned long x_balntry;	/* BAL entry point */
  } x_bal;			/* BAL-callable function */

  struct {
    unsigned long	x_timestamp; /* time stamp */
    char 	x_idstring[20];	/* producer identity string */
  } x_ident;			/* Producer ident info */


};

/********************** RELOCATION DIRECTIVES **********************/

struct internal_reloc {
	long r_vaddr;		/* Virtual address of reference */
	long r_symndx;		/* Index into symbol table	*/
	unsigned short r_type;	/* Relocation type		*/
#if M88
	unsigned short r_offset;
#endif
};

#define R_RELBYTE	017
#define R_RELWORD	020
#define R_PCRBYTE	022
#define R_PCRWORD	023
#define R_PCRLONG	024

#define R_DIR32		06
#define	R_PCLONG	020
#define R_RELBYTE	017
#define R_RELWORD	020

#define R_PCRBYTE	022
#define R_PCRWORD	023
#define R_PCRLONG	024

#define R_PCR16L 128
#define R_PCR26L 129
#define R_VRT16  130
#define R_HVRT16 131
#define R_LVRT16 132
#define R_VRT32  133
#define R_RELLONG	(0x11)	/* Direct 32-bit relocation */
#define R_IPRSHORT	(0x18)
#define R_IPRMED 	(0x19)	/* 24-bit ip-relative relocation */
#define R_IPRLONG	(0x1a)
#define R_OPTCALL	(0x1b)	/* 32-bit optimizable call (leafproc/sysproc) */
#define R_OPTCALLX	(0x1c)	/* 64-bit optimizable call (leafproc/sysproc) */
#define R_GETSEG	(0x1d)
#define R_GETPA		(0x1e)
#define R_TAGWORD	(0x1f)
