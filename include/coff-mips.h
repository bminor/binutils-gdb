/* Rudimentary ECOFF support on MIPS machines. 
   This lacks symbol information, normally provided on MIPS Unix systems
   in the files <sym.h> and <symconst.h>.  */

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


/* Mips magics */
#define MIPS_MAGIC_1 0x0180
#define MIPS_MAGIC_2 0x0162
#define MIPS_MAGIC_3 0x0160

#define ECOFFBADMAG(x) (((x).f_magic!=MIPS_MAGIC_1) && \
			((x).f_magic!=MIPS_MAGIC_2) &&\
			((x).f_magic!=MIPS_MAGIC_3))


#define	FILHDR	struct external_filehdr
#define	FILHSZ	20

/********************** AOUT "OPTIONAL HEADER" **********************/


typedef struct external_aouthdr
{
  char 	magic[2];		/* type of file				*/
  char	vstamp[2];		/* version stamp			*/
  char	tsize[4];		/* text size in bytes, padded to FW bdry*/
  char	dsize[4];		/* initialized data "  "		*/
  char	bsize[4];		/* uninitialized data "   "		*/
  char	entry[4];		/* entry pt.				*/
  char 	text_start[4];		/* base of text used for this file */
  char 	data_start[4];		/* base of data used for this file */
} AOUTHDR;

/* compute size of a header */

#define AOUTSZ (sizeof(AOUTHDR))


/********************** SECTION HEADER **********************/

struct external_scnhdr {
	char		s_name[8];	/* section name			*/
	char		s_paddr[4];	/* physical address, aliased s_nlib */
	char		s_vaddr[4];	/* virtual address		*/
	char		s_size[4];	/* section size			*/
	char		s_scnptr[4];	/* file ptr to raw data for section */
	char		s_relptr[4];	/* file ptr to relocation	*/
	char		s_lnnoptr[4];	/* file ptr to line numbers	*/
	char		s_nreloc[2];	/* number of relocation entries	*/
	char		s_nlnno[2];	/* number of line number entries*/
	char		s_flags[4];	/* flags			*/
      };

#define	SCNHDR	struct external_scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

#define DEFAULT_DATA_SECTION_ALIGNMENT 4
#define DEFAULT_BSS_SECTION_ALIGNMENT 4
#define DEFAULT_TEXT_SECTION_ALIGNMENT 16
/* For new sections we havn't heard of before */
#define DEFAULT_SECTION_ALIGNMENT 4

/********************** RELOCATION DIRECTIVES **********************/

struct external_reloc {
  char r_vaddr[4];
  char r_symndx[4];
  char r_type[2];
  char pad[2];
};


/* Relevent values for r_type and ecoff.  Would someone please document them */

#define RELOC struct external_reloc
#define RELSZ 12

/* These definitions are not used in ecoff, but they make it possible
   for ecoff to use the same "internalcoff.h" as other coff implementations. */

#define SYMNMLEN	8	/* # characters in a symbol name	*/
#define FILNMLEN	14	/* # characters in a file name		*/
#define DIMNUM		4	/* # array dimensions in auxiliary entry */

