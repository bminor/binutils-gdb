/*** coff information for 88k bcs */

/********************** FILE HEADER **********************/
struct external_filehdr {
	char f_magic[2];	/* magic number			*/
	char f_nscns[2];	/* number of sections		*/
	char f_timdat[4];	/* time & date stamp		*/
	char f_symptr[4];	/* file pointer to symtab	*/
	char f_nsyms[4];	/* number of symtab entries	*/
	char f_opthdr[2];	/* sizeof(optional hdr)		*/
	char f_flags[2];	/* flags			*/
};

struct internal_filehdr 
{
  unsigned short	f_magic; /* magic number			*/
  unsigned short	f_nscns; /* number of sections		*/
  long		f_timdat;	 /* time & date stamp		*/
  long		f_symptr;	 /* file pointer to symtab	*/
  long		f_nsyms;	 /* number of symtab entries	*/
  unsigned short	f_opthdr; /* sizeof(optional hdr)		*/
  unsigned short	f_flags;  /* flags			*/
};

/* Bits for f_flags:
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable (no unresolved externel references)
 *	F_LNNO		line nunbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_AR32WR	file has byte ordering of an AR32WR machine (e.g. vax)
 */
#define F_RELFLG	0000001
#define F_EXEC		0000002
#define F_LNNO		0000004
#define F_LSYMS	0000010
#define F_AR32WR	0x100
#define F_AR32W     0x200

#define MC88MAGIC  0540           /* 88k BCS executable */
#define MC88DMAGIC 0541           /* DG/UX executable   */
#define MC88OMAGIC 0555	          /* Object file        */

#define MC88BADMAG(x) (((x).f_magic!=MC88MAGIC) &&((x).f_magic!=MC88DMAGIC) && ((x).f_magic != MC88OMAGIC))

#define	FILHDR	struct external_filehdr
#define	FILHSZ	sizeof(FILHDR)


/********************** AOUT "OPTIONAL HEADER" **********************/


#define PAGEMAGIC3 0414 /* Split i&d, zero mapped */
#define PAGEMAGICBCS 0413

struct internal_aouthdr 
{
  short		magic;		/* type of file				*/
  short		vstamp;		/* version stamp			*/
  unsigned long	tsize;		/* text size in bytes, padded to FW bdry*/
  unsigned long	dsize;		/* initialized data "  "		*/
  unsigned long	bsize;		/* uninitialized data "   "		*/
  unsigned long	entry;		/* entry pt.				*/
  unsigned long	text_start;	/* base of text used for this file */
  unsigned long	data_start;	/* base of data used for this file */
  unsigned long	tagentries;	/* number of tag entries to follow */
} ;


typedef struct 
{
  char 	magic[2];		/* type of file				*/
  char	vstamp[2];		/* version stamp			*/
  char	tsize[4];		/* text size in bytes, padded to FW bdry*/
  char	dsize[4];		/* initialized data "  "		*/
  char	bsize[4];		/* uninitialized data "   "		*/
  char	entry[4];		/* entry pt.				*/
  char 	text_start[4];		/* base of text used for this file */
  char 	data_start[4];		/* base of data used for this file */
  char	tagentries[4];		/* number of tag entries to follow */
}
AOUTHDR;


/* compute size of a header */

#define AOUTSZ (sizeof(AOUTHDR))

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
#define C_BLOCK		100	/* ".bb" or ".eb"		*/
#define C_FCN		101	/* ".bf" or ".ef"		*/
#define C_EOS		102	/* end of structure		*/
#define C_FILE		103	/* file name			*/
#define C_LINE		104	/* line # reformatted as symbol table entry */
#define C_ALIAS	 	105	/* duplicate tag		*/
#define C_HIDDEN	106	/* ext symbol in dmert public lib */
#define C_SHADOW        107     /* shadow symbol                */
#define C_VERSION       108     /* coff version symbol          */


/********************** SECTION HEADER **********************/

struct internal_scnhdr 
{
  char		s_name[8];	/* section name			*/
  long		s_paddr;	/* physical address, aliased s_nlib */
  long		s_vaddr;	/* virtual address		*/
  long		s_size;		/* section size			*/
  long		s_scnptr;	/* file ptr to raw data for section */
  long		s_relptr;	/* file ptr to relocation	*/
  long		s_lnnoptr;	/* file ptr to line numbers	*/
  unsigned long s_nreloc;	/* number of relocation entries	*/
  unsigned long s_nlnno;	/* number of line number entries*/
  long		s_flags;	/* flags			*/
};

struct external_scnhdr 
{
  char		s_name[8];	/* section name			*/
  char		s_paddr[4];	/* physical address, aliased s_nlib */
  char		s_vaddr[4];	/* virtual address		*/
  char		s_size[4];	/* section size			*/
  char		s_scnptr[4];	/* file ptr to raw data for section */
  char		s_relptr[4];	/* file ptr to relocation	*/
  char		s_lnnoptr[4];	/* file ptr to line numbers	*/
  char		s_nreloc[4];	/* number of relocation entries	*/
  char		s_nlnno[4];	/* number of line number entries*/
  char		s_flags[4];	/* flags			*/
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
#define	STYP_TEXT	0x20		/* section contains text only	*/
#define STYP_DATA	0x40		/* section contains data only	*/
#define STYP_BSS	0x80		/* section contains bss only	*/

#define	SCNHDR	struct external_scnhdr
#define	SCNHSZ	sizeof(SCNHDR)


/********************** LINE NUMBERS **********************/

/* 1 line number entry for every "breakpointable" source line in a section.
 * Line numbers are grouped on a per function basis; first entry in a function
 * grouping will have l_lnno = 0 and in place of physical address will be the
 * symbol table index of the function name.
 */
struct external_lineno{
	union {
		char l_symndx[4];	/* function name symbol index, iff l_lnno == 0*/
		char l_paddr[4];	/* (physical) address of line number	*/
	} l_addr;

	char l_lnno[4];

};
struct internal_lineno{
	union {
		long l_symndx;	/* function name symbol index, iff l_lnno == 0*/
		long l_paddr;	/* (physical) address of line number	*/
	} l_addr;

	long l_lnno;

};

#define	LINENO	struct external_lineno
#define	LINESZ	8


/********************** SYMBOLS **********************/

#define SYMNMLEN	8	/* # characters in a symbol name	*/
#define FILNMLEN	14	/* # characters in a file name		*/
#define DIMNUM		4	/* # array dimensions in auxiliary entry */

struct external_syment 
{
  union {
    char e_name[SYMNMLEN];
    struct {
      char e_zeroes[4];
      char e_offset[4];
    } e;
  } e;
  char e_value[4];
  char e_scnum[2];
  char e_type[2];
  char e_sclass[1];
  char e_numaux[1];
  char pad2[2];
};

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
	unsigned short  n_type;		/* type and derived type	*/
	char		n_sclass;	/* storage class		*/
	char		n_numaux;	/* number of aux. entries	*/

};

/*
 * Relocatable symbols have number of the section in which they are defined,
 * or one of the following:
 */
#define N_UNDEF	0	/* undefined symbol				*/
#define N_ABS	-1	/* value of symbol is absolute			*/
#define N_DEBUG	-2	/* debugging symbol -- symbol value is meaningless */

/*
 * Type of a symbol, in low 4 bits of the word
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



/*
 * derived types
 */
#define DT_NON          0
#define DT_PTR		1	/* pointer	*/
#define DT_FCN		2	/* function	*/
#define DT_ARY		3	/* array	*/

#define N_BTMASK	017
#define N_TMASK		060
#define N_BTSHFT	4
#define N_TSHIFT	2

#define BTYPE(x)	((x) & N_BTMASK)


#define ISPTR(x)	(((x) & N_TMASK) == (DT_PTR << N_BTSHFT))
#define ISFCN(x)	(((x) & N_TMASK) == (DT_FCN << N_BTSHFT))
#define ISARY(x)	(((x) & N_TMASK) == (DT_ARY << N_BTSHFT))

#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

union external_auxent {
	struct {
		char x_tagndx[4];	/* str, un, or enum tag indx */
		union {
			struct {
			    char  x_lnno[2]; /* declaration line number */
			    char  x_size[2]; /* str/union/array size */
			} x_lnsz;
			char x_fsize[4];	/* size of function */
		} x_misc;
		union {
			struct {		/* if ISFCN, tag, or .bb */
			    char x_lnnoptr[4];	/* ptr to fcn line # */
			    char x_endndx[4];	/* entry ndx past block end */
			} x_fcn;
			struct {		/* if ISARY, up to 4 dimen. */
			    char x_dimen[DIMNUM][2];
			} x_ary;
		} x_fcnary;
		char x_tvndx[2];		/* tv index */
	} x_sym;

	union {
		char x_fname[FILNMLEN];
		struct {
			char x_zeroes[4];
			char x_offset[4];
		} x_n;
	} x_file;

	struct {
		char x_scnlen[4];			/* section length */
		char x_nreloc[2];	/* # relocation entries */
		char x_nlinno[2];	/* # line numbers */
	} x_scn;

        struct {
		char x_tvfill[4];	/* tv fill value */
		char x_tvlen[2];	/* length of .tv */
		char x_tvran[2][2];	/* tv range */
	} x_tv;		/* info about .tv section (in auxent of symbol .tv)) */


};
union 	internal_auxent 
{
  struct {
    long x_tagndx;		/* str, un, or enum tag indx */
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
	long x_endndx;		/* entry ndx past block end */
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
};

#define	SYMENT	struct external_syment
#define	SYMESZ	20
#define	AUXENT	union external_auxent
#define	AUXESZ	20


/********************** RELOCATION DIRECTIVES **********************/

struct external_reloc {
  char r_vaddr[4];
  char r_symndx[4];
  char r_type[2];
  char r_offset[2];
};

struct internal_reloc {
  long r_vaddr;		/* Virtual address of reference */
  long r_symndx;	/* Index into symbol table	*/
  unsigned short r_type; /* Relocation type		*/
  unsigned short r_offset; /* Hi 16 bits of constant       */
};

/* Only values of r_type GNU/88k cares about */
#define R_PCR16L 128
#define R_PCR26L 129
#define R_VRT16  130
#define R_HVRT16 131
#define R_LVRT16 132
#define R_VRT32  133




#define RELOC struct external_reloc
#define RELSZ  12

#define DEFAULT_SECTION_ALIGNMENT 8 /* double word */
