/*
 * a.out specifics for Sequent Symmetry running Dynix 3.x
 */
#ifndef A_OUT_DYNIX3_H
#define A_OUT_DYNIX3_H

/* struct exec for Dynix 3
 *
 * a_gdtbl and a_bootstrap are only for standalone binaries.
 * Shared data fields are not supported by the kernel as of Dynix 3.1,
 * but are supported by Dynix compiler programs.
 */
struct external_exec {
	unsigned char e_info[4];
	unsigned char e_text[4];
	unsigned char e_data[4];
	unsigned char e_bss[4];
	unsigned char e_syms[4];
	unsigned char e_entry[4];
	unsigned char e_trsize[4];
	unsigned char e_drsize[4];
	unsigned char e_g_code[8], e_g_data[8], e_g_desc[8];
	unsigned char e_shdata[4];
	unsigned char e_shbss[4];
	unsigned char e_shdrsize[4];
	unsigned char e_bootstrap[44];
	unsigned char e_reserved[12];
	unsigned char e_version[4];
};

/*
 * Register information and structs for Dynix 3,
 * culled from various system header files.
 */

/*
 * 80387 structure, from ptrace(2) and in u area
 */
struct  fpusave {
        unsigned short fpu_control, fpu_rsvd1; /* control word */
        unsigned short fpu_status, fpu_rsvd2;  /* status word */
        unsigned short fpu_tag, fpu_rsvd3;     /* tag word */
        unsigned long  fpu_ip;                 /* IP offset */
        unsigned short fpu_cs, fpu_rsvd4;      /* CS selector */
        unsigned long  fpu_data_offset;        /* data offset */
        unsigned short fpu_op_sel, fpu_rsvd5;  /* operand selector */
        unsigned short fpu_stack[8][5];        /* 8 80-bit temp-reals from FPU stack*/
};

/*
 * WTL1167 structure, from ptrace(2) and in u area
 */
#define FPA_NREGS 31

struct  fpasave {
        long            fpa_pcr;                /* context register */
        long            fpa_regs[FPA_NREGS];    /* register contents */
};

/*
 * structure used by ptrace(2) XPT_RREGS and XPT_WREGS
 */
struct  pt_regset {
        int     pr_eax;
        int     pr_ebx;
        int     pr_ecx;
        int     pr_edx;
        int     pr_esi;
        int     pr_edi;
        int     pr_ebp;
        int     pr_esp;
        int     pr_eip;
        int     pr_flags;
        struct  fpusave pr_fpu;
        struct  fpasave pr_fpa;
};

/*
 * Register offsets in u area of core file
 */
#define SS	(5)
#define ESP	(4)
#define FLAGS	(3)
#define CS	(2)
#define EIP	(1)
#define EAX	(0)
#define	ECX	(-1)
#define EDX	(-2)
#define EBX	(-3)
#define EBP	(-5)
#define ESI	(-6)
#define EDI	(-7)

/*
 * Important offsets into Dynix struct user, for use in examination of a
 * core file in a vaguely machine independant way.  For lack of
 * anything better, we use u_ar0 as a magic number, since it appears
 * to have an identical value under all versions of Dynix 3.
 */
#define U_AR0_OFFSET		0x8
#define U_AR0_VALUE		0x7fffffe8
#define U_TSIZE_OFFSET		0x60
#define U_DSIZE_OFFSET		0x64
#define U_SSIZE_OFFSET		0x68
#define U_FPUSAVE_OFFSET	0x3ff
#define U_FPASAVE_OFFSET	0x3b0

#define	EXEC_BYTES_SIZE	(4 + BYTES_IN_WORD * 7)

#define OMAGIC	0x12eb		/* .o */
#define ZMAGIC	0x22eb		/* zero @ 0, demand load */
#define XMAGIC	0x32eb		/* invalid @ 0, demand load */
#define SMAGIC	0x42eb		/* standalone, not supported here */

#define N_BADMAG(x) ((OMAGIC != N_MAGIC(x)) && \
		     (ZMAGIC != N_MAGIC(x)) && \
		     (XMAGIC != N_MAGIC(x)) && \
		     (SMAGIC != N_MAGIC(x)))

#define TEXT_START_ADDR 0x1000

#define PAGE_SIZE	0x1000
#define SEGMENT_SIZE PAGE_SIZE

#define STACK_END_ADDR (0x40000000 - PAGE_SIZE)

#define N_SET_MACHTYPE(exec, machtype) \
((exec).a_info = \
 ((exec).a_info&0xff00ffff) | ((((int)(machtype))&0xff) << 16))

#define N_SET_FLAGS(exec, flags) \
((exec).a_info = \
 ((exec).a_info&0x00ffffff) | (((flags) & 0xff) << 24))

#define N_SET_MAGIC(exec, magic) \
((exec).a_info = (((exec).a_info & 0xffff0000) | ((magic) & 0xffff)))

#define N_MACHTYPE(exec) ((enum machine_type)(((exec).a_info >> 16) & 0xff))
#define N_MAGIC(x) ((x).a_info & 0xffff)

#define N_MAGIC_EXTERNAL(x) ((x).a_magic)

#define N_ADDRADJ(x) ((ZMAGIC == N_MAGIC(x) || XMAGIC == N_MAGIC(x)) ? 0x1000 : 0)

#if 0
/* Ignore shared segments for now... */
#define N_TXTOFF(x) ((OMAGIC == N_MAGIC(x)) ? sizeof(struct exec) : 0)
#define N_DATAOFF(x) (N_TXTOFF(x) + (x).a_text - N_ADDRADJ(x))
#define N_SHDATAOFF(x) (N_DATAOFF(x) + (x).a_data)
#define N_TROFF(x) (N_SHDATAOFF(x) + (x).a_shdata)
#define N_DROFF(x) (N_TROFF(x) + (x).a_trsize)
#define N_SHDROFF(x) (N_DROFF(x) + (x).a_drsize)
#define N_SYMOFF(x) (N_SHDROFF(x) + (x).a_shdrsize)
#define N_STROFF(x) (N_SYMOFF(x) + (x).a_syms)
#endif

#define N_TXTOFF(x) ((OMAGIC == N_MAGIC(x)) ? sizeof(x) : 0)
#define N_DATOFF(x) (N_TXTOFF(x) + (x).a_text - N_ADDRADJ(x))
#define N_TRELOFF(x) (N_DATOFF(x) + (x).a_data)
#define N_DRELOFF(x) (N_TRELOFF(x) + (x).a_trsize)
#define N_SYMOFF(x) (N_DRELOFF(x) + (x).a_drsize)
#define N_STROFF(x) (N_SYMOFF(x) + (x).a_syms)

#define N_TXTADDR(x) \
	(((OMAGIC == N_MAGIC(x)) || (SMAGIC == N_MAGIC(x))) ? 0 \
	 : TEXT_START_ADDR)

#define N_DATADDR(x) \
	(OMAGIC == N_MAGIC(x) ? (N_TXTADDR(x) + (x).a_text) \
	 : (SEGMENT_SIZE + ((N_TXTADDR(x) + (x).a_text - 1) & \
			    ~(SEGMENT_SIZE - 1))))

#define N_BSSADDR(x) (N_DATADDR(x) + (x).a_data)

/* This may not be quite right */
#define N_TXTSIZE(x) ((x).a_text)
/* relocation stuff */


/* Relocations 

  There	are two types of relocation flavours for a.out systems,
  standard and extended. The standard form is used on systems where
  the instruction has room for all the bits of an offset to the operand, whilst the
  extended form is used when an address operand has to be split over n
  instructions. Eg, on the 68k, each move instruction can reference
  the target with a displacement of 16 or 32 bits. On the sparc, move
  instructions use an offset of 14 bits, so the offset is stored in
  the reloc field, and the data in the section is ignored.
*/

/* This structure describes a single relocation to be performed.
   The text-relocation section of the file is a vector of these structures,
   all of which apply to the text section.
   Likewise, the data-relocation section applies to the data section.  */

struct reloc_std_external {
  bfd_byte	r_address[BYTES_IN_WORD];	/* offset of of data to relocate 	*/
  bfd_byte r_index[3];	/* symbol table index of symbol 	*/
  bfd_byte r_type[1];	/* relocation type			*/
};

#define	RELOC_STD_BITS_PCREL_BIG	0x80
#define	RELOC_STD_BITS_PCREL_LITTLE	0x01

#define	RELOC_STD_BITS_LENGTH_BIG	0x60
#define	RELOC_STD_BITS_LENGTH_SH_BIG	5	/* To shift to units place */
#define	RELOC_STD_BITS_LENGTH_LITTLE	0x06
#define	RELOC_STD_BITS_LENGTH_SH_LITTLE	1

#define	RELOC_STD_BITS_EXTERN_BIG	0x10
#define	RELOC_STD_BITS_EXTERN_LITTLE	0x08

#define	RELOC_STD_BITS_BASEREL_BIG	0x08
#define	RELOC_STD_BITS_BASEREL_LITTLE	0x08

#define	RELOC_STD_BITS_JMPTABLE_BIG	0x04
#define	RELOC_STD_BITS_JMPTABLE_LITTLE	0x04

#define	RELOC_STD_BITS_RELATIVE_BIG	0x02
#define	RELOC_STD_BITS_RELATIVE_LITTLE	0x02

#define	RELOC_STD_SIZE	(BYTES_IN_WORD + 3 + 1)		/* Bytes per relocation entry */

struct reloc_std_internal
{
  bfd_vma r_address;		/* Address (within segment) to be relocated.  */
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
     in files the symbol table.
     0 => relocate with the address of a segment.
     r_symbolnum is N_TEXT, N_DATA, N_BSS or N_ABS
     (the N_EXT bit may be set also, but signifies nothing).  */
  unsigned int r_extern:1;
  /* The next three bits are for SunOS shared libraries, and seem to
     be undocumented.  */
  unsigned int r_baserel:1;	/* Linkage table relative */
  unsigned int r_jmptable:1;	/* pc-relative to jump table */
  unsigned int r_relative:1;	/* "relative relocation" */
  /* unused */
  unsigned int r_pad:1;		/* Padding -- set to zero */
};


/* EXTENDED RELOCS  */

struct reloc_ext_external {
  bfd_byte r_address[BYTES_IN_WORD];	/* offset of of data to relocate 	*/
  bfd_byte r_index[3];	/* symbol table index of symbol 	*/
  bfd_byte r_type[1];	/* relocation type			*/
  bfd_byte r_addend[BYTES_IN_WORD];	/* datum addend				*/
};

#define	RELOC_EXT_BITS_EXTERN_BIG	0x80
#define	RELOC_EXT_BITS_EXTERN_LITTLE	0x01

#define	RELOC_EXT_BITS_TYPE_BIG		0x1F
#define	RELOC_EXT_BITS_TYPE_SH_BIG	0
#define	RELOC_EXT_BITS_TYPE_LITTLE	0xF8
#define	RELOC_EXT_BITS_TYPE_SH_LITTLE	3

#define	RELOC_EXT_SIZE	(BYTES_IN_WORD + 3 + 1 + BYTES_IN_WORD)	/* Bytes per relocation entry */

enum reloc_type
{





  /* simple relocations */
  RELOC_8,			/* data[0:7] = addend + sv 		*/
  RELOC_16,			/* data[0:15] = addend + sv 		*/
  RELOC_32,			/* data[0:31] = addend + sv 		*/
  /* pc-rel displacement */
  RELOC_DISP8,			/* data[0:7] = addend - pc + sv 	*/
  RELOC_DISP16,			/* data[0:15] = addend - pc + sv 	*/
  RELOC_DISP32,			/* data[0:31] = addend - pc + sv 	*/
  /* Special */
  RELOC_WDISP30,		/* data[0:29] = (addend + sv - pc)>>2 	*/
  RELOC_WDISP22,		/* data[0:21] = (addend + sv - pc)>>2 	*/
  RELOC_HI22,			/* data[0:21] = (addend + sv)>>10 	*/
  RELOC_22,			/* data[0:21] = (addend + sv) 		*/
  RELOC_13,			/* data[0:12] = (addend + sv)		*/
  RELOC_LO10,			/* data[0:9] = (addend + sv)		*/
  RELOC_SFA_BASE,		
  RELOC_SFA_OFF13,
  /* P.I.C. (base-relative) */
  RELOC_BASE10,  		/* Not sure - maybe we can do this the */
  RELOC_BASE13,			/* right way now */
  RELOC_BASE22,
  /* for some sort of pc-rel P.I.C. (?) */
  RELOC_PC10,
  RELOC_PC22,
  /* P.I.C. jump table */
  RELOC_JMP_TBL,
  /* reputedly for shared libraries somehow */
  RELOC_SEGOFF16,
  RELOC_GLOB_DAT,
  RELOC_JMP_SLOT,
  RELOC_RELATIVE,

  RELOC_11,	
  RELOC_WDISP2_14,
  RELOC_WDISP19,
  RELOC_HHI22,			/* data[0:21] = (addend + sv) >> 42     */
  RELOC_HLO10,			/* data[0:9] = (addend + sv) >> 32      */
  
  /* 29K relocation types */
  RELOC_JUMPTARG,
  RELOC_CONST,
  RELOC_CONSTH,
  

  /* Q .
     What are the other ones,
     Since this is a clean slate, can we throw away the ones we dont
     understand ? Should we sort the values ? What about using a
     microcode format like the 68k ?
     */
  NO_RELOC
  };


struct reloc_internal {
  bfd_vma r_address;		/* offset of of data to relocate 	*/
  long	r_index;		/* symbol table index of symbol 	*/
  enum reloc_type r_type;	/* relocation type			*/
  bfd_vma r_addend;		/* datum addend				*/
};

/* Q.
   Should the length of the string table be 4 bytes or 8 bytes ?

   Q.
   What about archive indexes ?

 */

#define EXTERNAL_NLIST_SIZE (BYTES_IN_WORD+4+BYTES_IN_WORD)

/*
 * All executables under Dynix are demand paged with read-only text,
 * Thus no NMAGIC.
 *
 * ZMAGIC has a page of 0s at virtual 0,
 * XMAGIC has an invalid page at virtual 0
 */

#define WRITE_HEADERS(abfd, execp)					      \
      {									      \
	if (abfd->flags & D_PAGED) 					      \
	    {								      \
	      execp->a_text = obj_textsec (abfd)->_raw_size;		      \
	      /* Kludge to distinguish old- and new-style ZMAGIC.	      \
	         The latter includes the exec header in the text size. */     \
	      if (obj_textsec(abfd)->filepos == EXEC_BYTES_SIZE)	      \
		execp->a_text += EXEC_BYTES_SIZE;			      \
	      N_SET_MAGIC (*execp, ZMAGIC);				      \
	    } 								      \
	else 								      \
	    {								      \
	      execp->a_text = obj_textsec (abfd)->_raw_size;		      \
	      if (abfd->flags & WP_TEXT)				      \
	        { N_SET_MAGIC (*execp, ZMAGIC); }			      \
	      else							      \
	  	{ N_SET_MAGIC(*execp, OMAGIC); }			      \
	    }								      \
	if (abfd->flags & D_PAGED) 					      \
	    {								      \
	      data_pad = BFD_ALIGN(obj_datasec(abfd)->_raw_size, PAGE_SIZE)   \
		  - obj_datasec(abfd)->_raw_size;			      \
	  								      \
	      if (data_pad > obj_bsssec(abfd)->_raw_size)		      \
		execp->a_bss = 0;					      \
	      else 							      \
		execp->a_bss = obj_bsssec(abfd)->_raw_size - data_pad;	      \
	      execp->a_data = obj_datasec(abfd)->_raw_size + data_pad;	      \
	    }								      \
	else 								      \
	    {								      \
	      execp->a_data = obj_datasec (abfd)->_raw_size;		      \
	      execp->a_bss = obj_bsssec (abfd)->_raw_size;		      \
	    }								      \
    									      \
	execp->a_syms = bfd_get_symcount (abfd) * EXTERNAL_NLIST_SIZE;	      \
	execp->a_entry = bfd_get_start_address (abfd);			      \
    									      \
	execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *		      \
			   obj_reloc_entry_size (abfd));		      \
	NAME(aout,swap_exec_header_out) (abfd, execp, &exec_bytes);	      \
									      \
	bfd_seek (abfd, 0L, false);					      \
	bfd_write ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd);	      \
	/* Now write out reloc info, followed by syms and strings */	      \
  									      \
	if (bfd_get_symcount (abfd) != 0) 				      \
	    {								      \
	      bfd_seek (abfd,						      \
			(long)(N_SYMOFF(*execp)), false);		      \
									      \
	      NAME(aout,write_syms)(abfd);				      \
									      \
	      bfd_seek (abfd,	(long)(N_TRELOFF(*execp)), false);	      \
									      \
	      if (!NAME(aout,squirt_out_relocs) (abfd, obj_textsec (abfd))) return false; \
	      bfd_seek (abfd, (long)(N_DRELOFF(*execp)), false);	      \
									      \
	      if (!NAME(aout,squirt_out_relocs)(abfd, obj_datasec (abfd))) return false; \
	    }								      \
      }									      
#endif
