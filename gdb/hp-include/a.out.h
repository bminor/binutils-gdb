/* Special version of <a.out.h> for use under hp-ux.
   Copyright (C) 1988 Free Software Foundation, Inc. */

/* The `exec' structure and overall layout must be close to HP's when
   we are running on an HP system, otherwise we will not be able to
   execute the resulting file. */

/* Allow this file to be included twice. */
#ifndef EXEC_PAGESIZE

struct exec
  {
    unsigned short a_machtype;	/* machine type */
    unsigned short a_magic;	/* magic number */
    unsigned long a_spare1;
    unsigned long a_spare2;
    unsigned long a_text;	/* size of text segment */
    unsigned long a_data;	/* size of data segment */
    unsigned long a_bss;	/* size of bss segment */
    unsigned long a_trsize;	/* text relocation size */
    unsigned long a_drsize;	/* data relocation size */
    unsigned long a_spare3;	/* HP = pascal interface size */
    unsigned long a_spare4;	/* HP = symbol table size */
    unsigned long a_spare5;	/* HP = debug name table size */
    unsigned long a_entry;	/* entry point */
    unsigned long a_spare6;	/* HP = source line table size */
    unsigned long a_spare7;	/* HP = value table size */
    unsigned long a_syms;	/* symbol table size */
    unsigned long a_spare8;
  };

#define OMAGIC 0x107
#define NMAGIC 0x108
#define ZMAGIC 0x10B

#define N_BADTYPE(x)							\
  ((((x) . a_magic) != OMAGIC)						\
   && (((x) . a_magic) != NMAGIC)					\
   && (((x) . a_magic) != ZMAGIC))

#define HP98x6_ID 0x20A
#define HP9000S200_ID 0x20C

#define N_BADMACH(x)							\
  ((((x) . a_machtype) != HP9000S200_ID)				\
   && (((x) . a_machtype) != HP98x6_ID))

#define N_BADMAG(x) ((N_BADTYPE (x)) || (N_BADMACH (x)))

#define EXEC_PAGESIZE 0x1000

#define EXEC_ALIGN(offset)						\
  (((offset) + (EXEC_PAGESIZE - 1)) & (~ (EXEC_PAGESIZE - 1)))

#define N_TXTOFF(x)							\
  ((((x) . a_magic) == ZMAGIC)						\
   ? (EXEC_ALIGN (sizeof (struct exec)))				\
   : (sizeof (struct exec)))

#define N_DATOFF(x)							\
  ((((x) . a_magic) == ZMAGIC)						\
   ? ((EXEC_ALIGN (sizeof (struct exec)))				\
      + (EXEC_ALIGN ((x) . a_text)))					\
   : ((sizeof (struct exec)) + ((x) . a_text)))

#define N_TROFF(x)							\
  ((((x) . a_magic) == ZMAGIC)						\
   ? ((EXEC_ALIGN (sizeof (struct exec)))				\
      + (EXEC_ALIGN ((x) . a_text))					\
      + (EXEC_ALIGN ((x) . a_data)))					\
   : ((sizeof (struct exec)) + ((x) . a_text) + ((x) . a_data)))

#define N_SYMOFF(x) ((N_TROFF (x)) + ((x) . a_trsize) + ((x) . a_drsize))

#define N_STROFF(x) ((N_SYMOFF (x)) + ((x) . a_syms))

/* Macros which take exec structures as arguments and tell where the
   various pieces will be loaded. */

#define N_TXTADDR(x) 0

#define N_DATADDR(x)							\
  ((((x) . a_magic) == OMAGIC)						\
   ? ((x) . a_text)							\
   : (EXEC_ALIGN ((x) . a_text)))

#define N_BSSADDR(x) ((N_DATADDR (x)) + ((x) . a_data))

struct relocation_info
  {
    long r_address;		/* address which is relocated */
    unsigned int
      r_symbolnum : 24,		/* local symbol ordinal */
      r_pcrel : 1,		/* was relocated pc relative already */
      r_length : 2,		/* 0=byte, 1=word, 2=long */
      r_extern : 1,		/* does not include value of sym referenced */
      : 4;			/* nothing, yet */
  };

struct nlist
  {
    union
      {
	char *n_name;		/* for use when in-core */
	long n_strx;		/* index into file string table */
      } n_un;
    unsigned char n_type;	/* type flag (N_TEXT,..)  */
    char n_other;		/* unused */
    short n_desc;		/* see <stab.h> */
    unsigned long n_value;	/* value of symbol (or sdb offset) */
  };

/* Simple values for n_type. */
#define	N_UNDF	0x00		/* undefined */
#define	N_ABS	0x02		/* absolute */
#define	N_TEXT	0x04		/* text */
#define	N_DATA	0x06		/* data */
#define	N_BSS	0x08		/* bss */
#define	N_COMM	0x12		/* common (internal to ld) */
#define	N_FN	0x1F		/* file name symbol */

#define	N_EXT	0x01		/* external bit, or'ed in */
#define	N_TYPE	0x1E		/* mask for all the type bits */

/* dbx entries have some of the N_STAB bits set.
   These are given in <stab.h> */
#define	N_STAB	0xE0		/* if any of these bits set, a dbx symbol */

/* Format for namelist values. */
#define	N_FORMAT "%08x"

#endif /* EXEC_PAGESIZE */
