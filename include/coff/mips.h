/* Rudimentary ECOFF support on MIPS machines. 
   This lacks symbol information, normally provided on MIPS Unix systems
   in the files <sym.h> and <symconst.h>.  */

/********************** FILE HEADER **********************/

struct external_filehdr {
  unsigned char f_magic[2];	/* magic number			*/
  unsigned char f_nscns[2];	/* number of sections		*/
  unsigned char f_timdat[4];	/* time & date stamp		*/
  unsigned char f_symptr[4];	/* file pointer to symtab	*/
  unsigned char f_nsyms[4];	/* number of symtab entries	*/
  unsigned char f_opthdr[2];	/* sizeof(optional hdr)		*/
  unsigned char f_flags[2];	/* flags			*/
};


/* Mips magic numbers used in filehdr.  MIPS_MAGIC_LITTLE is used on
   little endian machines.  MIPS_MAGIC_BIG is used on big endian
   machines.  Where is MIPS_MAGIC_1 from?  */
#define MIPS_MAGIC_1 0x0180
#define MIPS_MAGIC_LITTLE 0x0162
#define MIPS_MAGIC_BIG 0x0160

#define ECOFFBADMAG(x) (((x).f_magic!=MIPS_MAGIC_1) && \
			((x).f_magic!=MIPS_MAGIC_LITTLE) &&\
			((x).f_magic!=MIPS_MAGIC_BIG))


#define	FILHDR	struct external_filehdr
#define	FILHSZ	20

/********************** AOUT "OPTIONAL HEADER" **********************/


typedef struct external_aouthdr
{
  unsigned char magic[2];	/* type of file				*/
  unsigned char	vstamp[2];	/* version stamp			*/
  unsigned char	tsize[4];	/* text size in bytes, padded to FW bdry*/
  unsigned char	dsize[4];	/* initialized data "  "		*/
  unsigned char	bsize[4];	/* uninitialized data "   "		*/
  unsigned char	entry[4];	/* entry pt.				*/
  unsigned char text_start[4];	/* base of text used for this file */
  unsigned char data_start[4];	/* base of data used for this file */
  unsigned char bss_start[4];	/* base of bss used for this file */
  unsigned char gprmask[4];	/* ?? */
  unsigned char cprmask[4][4];	/* ?? */
  unsigned char gp_value[4];	/* value for gp register */
} AOUTHDR;

/* compute size of a header */

#define AOUTSZ (sizeof(AOUTHDR))

#define OMAGIC		0407	/* not demand paged (ld -N).  */
#define ZMAGIC          0413    /* demand load format, eg normal ld output */

/********************** SECTION HEADER **********************/

struct external_scnhdr {
  unsigned char	s_name[8];	/* section name			*/
  unsigned char	s_paddr[4];	/* physical address, aliased s_nlib */
  unsigned char	s_vaddr[4];	/* virtual address		*/
  unsigned char	s_size[4];	/* section size			*/
  unsigned char	s_scnptr[4];	/* file ptr to raw data for section */
  unsigned char	s_relptr[4];	/* file ptr to relocation	*/
  unsigned char	s_lnnoptr[4];	/* file ptr to line numbers	*/
  unsigned char	s_nreloc[2];	/* number of relocation entries	*/
  unsigned char	s_nlnno[2];	/* number of line number entries*/
  unsigned char	s_flags[4];	/* flags			*/
};

#define	SCNHDR	struct external_scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

/*
 * names of "special" sections
 */
#define _TEXT   ".text"
#define _DATA   ".data"
#define _BSS    ".bss"
#define _RDATA	".rdata"
#define _SDATA	".sdata"
#define _SBSS	".sbss"
#define _LIT4	".lit4"
#define _LIT8	".lit8"
#define _LIB	".lib"

#define DEFAULT_DATA_SECTION_ALIGNMENT 4
#define DEFAULT_BSS_SECTION_ALIGNMENT 4
#define DEFAULT_TEXT_SECTION_ALIGNMENT 16
/* For new sections we havn't heard of before */
#define DEFAULT_SECTION_ALIGNMENT 4

/* MIPS ECOFF uses some additional section types.  */
#define STYP_RDATA 0x100
#define STYP_SDATA 0x200
#define STYP_SBSS 0x400
#define STYP_LIT8 0x8000000
#define STYP_LIT4 0x10000000

/* I don't know when this is used.  */
#define STYP_OTHER_LOAD 0x80000000

/********************** RELOCATION DIRECTIVES **********************/

struct external_reloc {
  unsigned char r_vaddr[4];
  unsigned char r_bits[4];
};

#define RELOC struct external_reloc
#define RELSZ 8

/* MIPS ECOFF uses a packed 8 byte format for relocs.  These constants
   are used to unpack the r_bits field.  */

#define RELOC_BITS0_SYMNDX_SH_LEFT_BIG		16
#define RELOC_BITS0_SYMNDX_SH_LEFT_LITTLE	0

#define RELOC_BITS1_SYMNDX_SH_LEFT_BIG		8
#define RELOC_BITS1_SYMNDX_SH_LEFT_LITTLE	8

#define RELOC_BITS2_SYMNDX_SH_LEFT_BIG		0
#define RELOC_BITS2_SYMNDX_SH_LEFT_LITTLE	16

#define RELOC_BITS3_TYPE_BIG			0x1E
#define RELOC_BITS3_TYPE_SH_BIG			1
#define RELOC_BITS3_TYPE_LITTLE			0x78
#define RELOC_BITS3_TYPE_SH_LITTLE		3

#define RELOC_BITS3_EXTERN_BIG			0x01
#define RELOC_BITS3_EXTERN_LITTLE		0x80

/* We store the extern field in the r_offset field of a struct
   internal_reloc.  FIXME: Do this more sensibly.  */
#define r_extern r_offset

/* If the extern bit is 1, then r_symndx is an index into the external
   symbol table.  If the extern bit is 0, then r_symndx indicates a
   section, and is one of the following values.  */
#define RELOC_SECTION_TEXT	1
#define RELOC_SECTION_RDATA	2
#define RELOC_SECTION_DATA	3
#define RELOC_SECTION_SDATA	4
#define RELOC_SECTION_SBSS	5
#define RELOC_SECTION_BSS	6
#define RELOC_SECTION_INIT	7
#define RELOC_SECTION_LIT8	8
#define RELOC_SECTION_LIT4	9

/* The r_type field is one of the following values.  I don't know if
   any other values can appear.  These seem to be all that occur in
   the Ultrix 4.2 libraries.  */
#define ECOFF_R_IGNORE	0
#define ECOFF_R_REFHALF	1
#define ECOFF_R_REFWORD	2
#define ECOFF_R_JMPADDR	3
#define ECOFF_R_REFHI	4
#define ECOFF_R_REFLO	5
#define ECOFF_R_GPREL	6
#define ECOFF_R_LITERAL	7

/********************** STABS **********************/

/* gcc uses mips-tfile to output type information in special stabs
   entries.  These must match the corresponding definition in
   gcc/config/mips.h.  At some point, these should probably go into a
   shared include file, but currently gcc and gdb do not share any
   directories. */
#define CODE_MASK 0x8F300
#define MIPS_IS_STAB(sym) (((sym)->index & 0xFFF00) == CODE_MASK)
#define MIPS_MARK_STAB(code) ((code)+CODE_MASK)
#define MIPS_UNMARK_STAB(code) ((code)-CODE_MASK)
#define STABS_SYMBOL "@stabs"

/********************** COFF **********************/

/* gcc also uses mips-tfile to output COFF debugging information.
   These are the values it uses when outputting the .type directive.
   These should also be in a shared include file.  */
#define N_BTMASK	(017)
#define N_TMASK		(060)
#define N_BTSHFT	(4)
#define N_TSHIFT	(2)
