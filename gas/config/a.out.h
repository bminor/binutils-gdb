/* This file describes the a.out file format
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

#ifdef USE_HP_INC_HDR
#include "../binutils/hp-include/a.out.h"
#else

#ifdef USE_SYSTEM_HDR
#include "/usr/include/a.out.h"
#else

#ifdef USE_HP_HDR
/* The `exec' structure and overall layout must be close to HP's when
   we are running on an HP system, otherwise we will not be able to
   execute the resulting file. */

struct exec
{
	unsigned short a_machtype;	/* machine type */
	unsigned short a_magic;	/* magic number */
	unsigned long a_spare1;
	unsigned long a_spare2;
	unsigned long a_text;		/* length of text, in bytes */
	unsigned long a_data;		/* length of data, in bytes */
	unsigned long a_bss;		/* length of uninitialized data area for file, in bytes */
	unsigned long a_trsize;	/* length of relocation info for text, in bytes */
	unsigned long a_drsize;	/* length of relocation info for data, in bytes */
	unsigned long a_spare3;	/* HP = pascal interface size */
	unsigned long a_spare4;	/* HP = symbol table size */
	unsigned long a_spare5;	/* HP = debug name table size */
	unsigned long a_entry;	/* start address */
	unsigned long a_spare6;	/* HP = source line table size */
	unsigned long a_spare7;	/* HP = value table size */
	unsigned long a_syms;		/* length of symbol table data in file, in bytes */
	unsigned long a_spare8;
};

#define N_MAGIC(exec) ((exec) . a_magic)
#define N_MACHTYPE(exec) ((exec) . a_machtype)
#define N_SET_MAGIC(exec, magic) (((exec) . a_magic) = (magic))
#define N_SET_MACHTYPE(exec, machtype) (((exec) . a_machtype) = (machtype))

#define N_BADMAG(x) ((_N_BADMAG (x)) || (_N_BADMACH (x)))

#define _N_BADMACH(x) \
    (((N_MACHTYPE (x)) != HP9000S200_ID) \
     && ((N_MACHTYPE (x)) != HP98x6_ID))

#define _N_BADMAG(x) \
    (((x).a_magic)!=OMAGIC && ((x).a_magic)!=NMAGIC && ((x).a_magic)!=ZMAGIC)

#define HP98x6_ID 0x20A
#define HP9000S200_ID 0x20C

#else

/* A Generic machine. . . */

/* JF I'm not sure where this file came from.  I put the permit.text message in
   it anyway.  This file came to me as part of the original VAX assembler, but had
   no copyright notices in it. */

struct exec {
	long	a_magic;	/* number identifies as .o file and gives type of such. */
	unsigned a_text;	/* length of text, in bytes */
	unsigned a_data;	/* length of data, in bytes */
	unsigned a_bss;		/* length of uninitialized data area for file, in bytes */
	unsigned a_syms;	/* length of symbol table data in file, in bytes */
	unsigned a_entry;	/* start address */
	unsigned a_trsize;	/* length of relocation info for text, in bytes */
	unsigned a_drsize;	/* length of relocation info for data, in bytes */
};

#define N_BADMAG(x) \
    (((x).a_magic)!=OMAGIC && ((x).a_magic)!=NMAGIC && ((x).a_magic)!=ZMAGIC)

#endif

/* From here down is common to both the HP and the generic machine */
#define OMAGIC 0407
#define NMAGIC 0410
#define ZMAGIC 0413


#define N_TXTOFF(x) \
    ((x).a_magic == ZMAGIC ? 1024 : sizeof(struct exec))

#define N_SYMOFF(x) \
    (N_TXTOFF(x) + (x).a_text + (x).a_data + (x).a_trsize + (x).a_drsize)

#define N_STROFF(x) \
    (N_SYMOFF(x) + (x).a_syms)

struct nlist {
	union {
		char	*n_name;
		struct nlist *n_next;
		long	n_strx;
	} n_un;
	char	n_type;
	char	n_other;
	short	n_desc;
	unsigned n_value;
};

#define N_UNDF	0
#define N_ABS	2
#define N_TEXT	4
#define N_DATA	6
#define N_BSS	8
#define N_FN	31		/* JF: Someone claims this should be 31 instead of
				   15.  I just inherited this file; I didn't write
				   it.  Who is right? */


#define N_EXT 1
#define N_TYPE 036
#define N_STAB 0340

struct relocation_info {
	int r_address;
	unsigned r_symbolnum:24;
	unsigned r_pcrel:1;
	unsigned r_length:2;
	unsigned r_extern:1;
	unsigned r_bsr:1;	/* OVE: used on ns32k based systems, if you want */
	unsigned r_disp:1;	/* OVE: used on ns32k based systems, if you want */
	unsigned nuthin:2;
};

#endif
#endif

/* end of a.out.h */
