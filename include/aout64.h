#ifndef __A_OUT_64_H__
#define __A_OUT_64_H__


/* This is the layout on disk of the 64 bit exec header. */

struct external_exec 
{
  bfd_byte e_info[4];		/* magic number and stuff		*/
  bfd_byte e_text[BYTES_IN_WORD]; /* length of text section in bytes	*/
  bfd_byte e_data[BYTES_IN_WORD]; /* length of data section in bytes	*/
  bfd_byte e_bss[BYTES_IN_WORD]; /* length of bss area in bytes 		*/
  bfd_byte e_syms[BYTES_IN_WORD]; /* length of symbol table in bytes 	*/
  bfd_byte e_entry[BYTES_IN_WORD]; /* start address 			*/
  bfd_byte e_trsize[BYTES_IN_WORD]; /* length of text relocation info	*/
  bfd_byte e_drsize[BYTES_IN_WORD]; /* length of data relocation info 	*/
};


#define	EXEC_BYTES_SIZE	(4 + BYTES_IN_WORD * 7)

/* This is the layout in memory of a "struct exec" while we process it.  */
struct internal_exec
  {
    long a_info;		/* Magic number and flags packed		*/
    bfd_vma a_text;		/* length of text, in bytes 			*/
    bfd_vma a_data;		/* length of data, in bytes 			*/
    bfd_vma a_bss;		/* length of uninitialized data area for file	*/
    bfd_vma a_syms;		/* length of symbol table data in file		*/
    bfd_vma a_entry;		/* start address 				*/
    bfd_vma a_trsize;		/* length of relocation info for text, in bytes */
    bfd_vma a_drsize;		/* length of relocation info for data, in bytes */
  };


/* Magic number is written 
< MSB                >
31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
< FLAGS             > <    MACHINE TYPE     >	<  MAGIC  				     >
*/
enum machine_type {
  M_UNKNOWN = 0,
  M_68010 = 1,
  M_68020 = 2,
  M_SPARC = 3,
  /* skip a bunch so we dont run into any of suns numbers */
  M_386 = 100,
  M_29K = 101,
  M_NEWONE = 200,
  M_NEWTWO = 201,

};

#define N_DYNAMIC(exec) ((exec).a_info & 0x8000000)

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

/* By default, segment size is constant.  But on some machines, it can
   be a function of the a.out header (e.g. machine type).  */
#ifndef	N_SEGSIZE
#define	N_SEGSIZE(x)	SEGMENT_SIZE
#endif

#define _N_HDROFF(x)	(N_SEGSIZE(x) - EXEC_BYTES_SIZE)
/* address in an a.out of the text section. When demand paged, it's
 set up a bit to make nothing at 0, when an object file it's 0.
 There's a special hack case when the entry point is < TEXT_START_ADDR
 for executables, then the real start is 0 
*/

#define N_TXTADDR(x) \
    (N_MAGIC(x)==OMAGIC? 0 \
     : (N_MAGIC(x) == ZMAGIC && (x).a_entry < TEXT_START_ADDR)? 0 \
     : TEXT_START_ADDR)

/* offset in an a.out of the start of the text section. When demand
   paged, this is the start of the file
*/

#define N_TXTOFF(x)	( (N_MAGIC((x)) == ZMAGIC) ? 0 : EXEC_BYTES_SIZE)
#if ARCH_SIZE==64
#define PAGE_SIZE 0x2000
#define OMAGIC 0x1001		/* Code indicating object file  */
#define ZMAGIC 0x1002		/* Code indicating demand-paged executable.  */
#define NMAGIC 0x1003		/* Code indicating pure executable.  */
#else
#ifndef PAGE_SIZE
#define PAGE_SIZE 0x2000
#endif
#define OMAGIC 0407		/* Code indicating object file or impure executable.  */
#define NMAGIC 0410		/* Code indicating pure executable.  */
#define ZMAGIC 0413		/* Code indicating demand-paged executable.  */
#endif

#define N_BADMAG(x)	  (N_MAGIC(x) != OMAGIC		\
			&& N_MAGIC(x) != NMAGIC		\
  			&& N_MAGIC(x) != ZMAGIC)



#define N_DATADDR(x) \
    (N_MAGIC(x)==OMAGIC? (N_TXTADDR(x)+(x).a_text) \
     :  (N_SEGSIZE(x) + ((N_TXTADDR(x)+(x).a_text-1) & ~(N_SEGSIZE(x)-1))))

#define N_BSSADDR(x) (N_DATADDR(x) + (x).a_data)


#define N_DATOFF(x)	( N_TXTOFF(x) + (x).a_text )
#define N_TRELOFF(x)	( N_DATOFF(x) + (x).a_data )
#define N_DRELOFF(x)	( N_TRELOFF(x) + (x).a_trsize )
#define N_SYMOFF(x)	( N_DRELOFF(x) + (x).a_drsize )
#define N_STROFF(x)	( N_SYMOFF(x) + (x).a_syms )


/* Symbols */
struct external_nlist {
  bfd_byte e_strx[BYTES_IN_WORD]; /* index into string table of symbol name 	*/
  bfd_byte e_type[1];	       /* type of symbol				*/
  bfd_byte e_other[1];	       /* misc info (usually empty)			*/
  bfd_byte e_desc[2];	       /* description field				*/
  bfd_byte e_value[BYTES_IN_WORD];/* value of symbol				*/
};

#define EXTERNAL_LIST_SIZE (BYTES_IN_WORD+4+BYTES_IN_WORD)
struct internal_nlist {
  char *strx;				/* index into string table of symbol name 	*/
  uint8_type n_type;			/* type of symbol				*/
  uint8_type n_other;			/* misc info (usually empty)			*/
  uint16_type n_desc;		       	/* description field				*/
  bfd_vma n_value;			/* value of symbol				*/
};

/* The n_type field is packed : 

  7 6 5 4 3 2 1 0
                ^-	if set the symbol is externaly visible 
		0 	local
		1 	N_EXT external
	  ^ ^ ^---- 	select which section the symbol belongs to
	0 0 0 0 x	 N_UNDF, undefined
	0 0 0 1	x	 N_ABS,  no section, base at 0
	0 0 1 0	x  	 N_TEXT, text section
 	0 0 1 1	x  	 N_DATA, data section	
	0 1 0 0	x  	 N_BSS,  bss section
	^----------     if set the symbol is a set element
	1 0 1 0 x	 N_SETA	absolute set element symbol
	1 0 1 1 x	 N_SETT text set element symbol
	1 1 0 0 x	 N_SETD data set element symbol
	1 1 0 1 x	 N_SETB bss set element symbol
	1 1 1 0 x	 N_SETV pointer to set vector in data area
        1 1 1 1 0 	 N_TYPE mask for all of the above

  1 1 1 0 0 0 0 0	 N_STAB type is a stab
*/

#define N_UNDF	0			
#define N_ABS 	2
#define N_TEXT 	4
#define N_DATA 	6
#define N_BSS 	8
#define N_FN	15
#define N_EXT 	1
#define N_TYPE  0x1e
#define N_STAB 	0xe0

#define N_INDR 0x0a

/* The following symbols refer to set elements.
   All the N_SET[ATDB] symbols with the same name form one set.
   Space is allocated for the set in the text section, and each set
   elements value is stored into one word of the space.
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
  
  /* All the new ones I can think of *//*v9*/

  RELOC_64,			/* data[0:63] = addend + sv 		*//*v9*/
  RELOC_DISP64,			/* data[0:63] = addend - pc + sv 	*//*v9*/
  RELOC_WDISP21,		/* data[0:20] = (addend + sv - pc)>>2 	*//*v9*/
  RELOC_DISP21,			/* data[0:20] = addend - pc + sv        *//*v9*/
  RELOC_DISP14,			/* data[0:13] = addend - pc + sv 	*//*v9*/
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

#endif				/* __A_OUT_GNU_H__ */
