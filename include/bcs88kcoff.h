/*** coff information for 88k bcs */

#ifndef M88
#define M88
#endif

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
#define F_RELFLG	0000001
#define F_EXEC		0000002
#define F_LNNO		0000004
#define F_LSYMS	0000010
#define F_AR32WR	0000400


#define MC88MAGIC 0555           /* 88k BCS executable */
#define MC88DMAGIC 0541          /* DG/UX executable   */


#define MC88BADMAG(x) (((x).f_magic!=MC88MAGIC) && ((x).f_magic!=MC88DMAGIC))

#define	FILHDR	struct filehdr
#define	FILHSZ	sizeof(FILHDR)


/********************** AOUT "OPTIONAL HEADER" **********************/


#define PAGEMAGIC3 0414 /* Split i&d, zero mapped */
typedef	struct aouthdr {
	short		magic;	/* type of file				*/
	short		vstamp;	/* version stamp			*/
	unsigned long	tsize;	/* text size in bytes, padded to FW bdry*/
	unsigned long	dsize;	/* initialized data "  "		*/
	unsigned long	bsize;	/* uninitialized data "   "		*/

	unsigned long	entry;	/* entry pt.				*/
	unsigned long	text_start;	/* base of text used for this file */
	unsigned long	data_start;	/* base of data used for this file */

} AOUTHDR;


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

struct scnhdr {
	char		s_name[8];	/* section name			*/
	long		s_paddr;	/* physical address, aliased s_nlib */
	long		s_vaddr;	/* virtual address		*/
	long		s_size;		/* section size			*/
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation	*/
	long		s_lnnoptr;	/* file ptr to line numbers	*/
	long    	s_nreloc;	/* number of relocation entries	*/
	long  	        s_nlnno;	/* number of line number entries*/
	long		s_flags;	/* flags			*/
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

#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)


/********************** LINE NUMBERS **********************/

/* 1 line number entry for every "breakpointable" source line in a section.
 * Line numbers are grouped on a per function basis; first entry in a function
 * grouping will have l_lnno = 0 and in place of physical address will be the
 * symbol table index of the function name.
 */
struct lineno{
	union {
		long l_symndx;	/* function name symbol index, iff l_lnno == 0*/
		long l_paddr;	/* (physical) address of line number	*/
	} l_addr;

	long l_lnno;

};

#define	LINENO	struct lineno
#define	LINESZ	sizeof(LINENO) 


/********************** SYMBOLS **********************/

#define SYMNMLEN	8	/* # characters in a symbol name	*/
#define FILNMLEN	14	/* # characters in a file name		*/
#define DIMNUM		4	/* # array dimensions in auxiliary entry */


struct syment {
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
	char		pad2[2];	/* force alignment		*/
};

#define n_name		_n._n_name
#define n_zeroes	_n._n_n._n_zeroes
#define n_offset	_n._n_n._n_offset

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

union auxent {
	struct {
		long x_tagndx;	/* str, un, or enum tag indx */
		union {
			struct {
			    unsigned long x_lnno; /* declaration line number */
			    unsigned long x_size; /* str/union/array size */
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

	union {
		char x_fname[FILNMLEN];
		struct {
			long x_zeroes;
			long x_offset;
		} x_n;
	} x_file;

	struct {
		long x_scnlen;			/* section length */
		unsigned long x_nreloc;	/* # relocation entries */
		unsigned long x_nlinno;	/* # line numbers */
	} x_scn;


};

#define	SYMENT	struct syment
#define	SYMESZ	sizeof(SYMENT)	
#define	AUXENT	union auxent
#define	AUXESZ	sizeof(AUXENT)


/********************** RELOCATION DIRECTIVES **********************/

struct reloc {
	long r_vaddr;		/* Virtual address of reference */
	long r_symndx;		/* Index into symbol table	*/
	unsigned short r_type;	/* Relocation type		*/
	unsigned short r_offset;/* Hi 16 bits of constant       */
};

/* Only values of r_type GNU/88k cares about */
#define R_PCR16L 128
#define R_PCR26L 129
#define R_VRT16  130
#define R_HVRT16 131
#define R_LVRT16 132
#define R_VRT32  133




#define RELOC struct reloc
#define RELSZ sizeof(RELOC)

#define DEFAULT_SECTION_ALIGNMENT 8 /* double word */
