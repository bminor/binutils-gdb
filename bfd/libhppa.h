/* HP PA-RISC SOM object file format:  definitions internal to BFD.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.

   Contributed by the Center for Software Science at the
   University of Utah (pa-gdb-bugs@cs.utah.edu).

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _LIBHPPA_H
#define _LIBHPPA_H

#include "../bfd/sysdep.h"

#ifdef HOST_HPPAHPUX

#include <sys/core.h>
#include <sys/utsname.h>

#endif /* HOST_HPPAHPUX */

#ifdef HOST_HPPABSD

/* BSD uses a completely different scheme for object file identification.
   so for now, define _PA_RISC_ID to accept any random value for a model
   number.  */

#include <a.out.h>

/* Not a very swift place to put it, but that's where the BSD port
   puts them.  */
#include "/hpux/usr/include/sys/core.h"
#ifndef _PA_RISC_ID
#define _PA_RISC_ID(__m_num) 1
#endif

#endif /* HOST_HPPABSD */

/* Defining MAXCOMLEN avoids bringing in several (7 or 8) otherwise
   useless include files which tend to clutter up the namespace.  */

#define MAXCOMLEN 16

#define BYTES_IN_WORD 4

struct header;
struct som_exec_auxhdr;
struct subspace_dictionary;

#define FILE_HDR_SIZE sizeof(struct header)
#define AUX_HDR_SIZE sizeof(struct som_exec_auxhdr)

typedef struct hppa_symbol
{
  asymbol symbol;
  short desc;
  char other;
  unsigned char type;
} hppa_symbol_type;

struct hppadata
{
  struct header *file_hdr;
  struct som_exec_auxhdr *aux_hdr;
  hppa_symbol_type *symbols;

  /* We remember these offsets so that after check_file_format, we have
     no dependencies on the particular format of the exec_hdr.  */

  file_ptr sym_filepos;
  file_ptr str_filepos;

  unsigned stringtab_size;

  /* Size of a symbol table entry in external form */
  unsigned hp_symbol_entry_size;
};

struct hppa_data_struct {
  struct hppadata a;
};

#define padata(bfd)		((bfd)->tdata.hppa_data->a)
#define obj_file_hdr(bfd)	(padata(bfd).file_hdr)
#define obj_aux_hdr(bfd)	(padata(bfd).aux_hdr)
#define obj_pa_symbols(bfd)	(padata(bfd).symbols)
#define obj_sym_filepos(bfd)	(padata(bfd).sym_filepos)
#define obj_str_filepos(bfd)	(padata(bfd).str_filepos)
#define obj_stringtab_size(bfd)	(padata(bfd).stringtab_size)

/* We take the address of the first element of an asymbol to ensure that the
   macro is only ever applied to an asymbol */
#define hppa_symbol(asymbol) ((hppa_symbol_type *)(&(asymbol)->the_bfd))


/* These are stored in the bfd's tdata */

struct hppa_core_struct 
{
  int sig;
  char cmd[MAXCOMLEN + 1];
  asection *data_section;
  asection *stack_section;
  asection *reg_section;
};

#define core_hdr(bfd) ((bfd)->tdata.hppa_core_data)
#define core_signal(bfd) (core_hdr(bfd)->sig)
#define core_command(bfd) (core_hdr(bfd)->cmd)
#define core_datasec(bfd) (core_hdr(bfd)->data_section)
#define core_stacksec(bfd) (core_hdr(bfd)->stack_section)
#define core_regsec(bfd) (core_hdr(bfd)->reg_section)

/* HP PA-RISC relocation types */

enum hppa_reloc_field_selector_type
{
	R_HPPA_FSEL	= 0x0,
	R_HPPA_LSSEL	= 0x1,
	R_HPPA_RSSEL	= 0x2,
	R_HPPA_LSEL	= 0x3,
	R_HPPA_RSEL	= 0x4,
	R_HPPA_LDSEL	= 0x5,
	R_HPPA_RDSEL	= 0x6,
	R_HPPA_LRSEL	= 0x7,
	R_HPPA_RRSEL	= 0x8,
	R_HPPA_PSEL	= 0x9,	/* P'	: procedure address for shlib's	*/
	R_HPPA_LPSEL	= 0xa,	/* LP'	: L' for procedure addresses	*/
	R_HPPA_RPSEL	= 0xb,	/* RP'	: R' for procedure addresses	*/

	R_HPPA_TSEL	= 0xc,	/* T'	: DLT-relative offset for shlib's	*/
	R_HPPA_LTSEL	= 0xd,	/* LT'	: L' for DLT-relative offsets	*/
	R_HPPA_RTSEL	= 0xe	/* RT'	: R' for DLT-relative offsets	*/

};

/* Need to undefine things defined in <machine/som.h> */

#undef e_fsel
#undef e_lssel
#undef e_rssel
#undef e_lsel
#undef e_rsel
#undef e_ldsel
#undef e_rdsel
#undef e_lrsel
#undef e_rrsel
#undef e_psel
#undef e_lpsel
#undef e_rpsel
#undef e_tsel
#undef e_ltsel
#undef e_rtsel
#undef e_one
#undef e_two
#undef e_pcrel
#undef e_con
#undef e_plabel
#undef e_abs

/* for compatibility */
enum hppa_reloc_field_selector_type_alt
{
	e_fsel	= R_HPPA_FSEL,
	e_lssel	= R_HPPA_LSSEL,
	e_rssel	= R_HPPA_RSSEL,
	e_lsel	= R_HPPA_LSEL,
	e_rsel	= R_HPPA_RSEL,
	e_ldsel	= R_HPPA_LDSEL,
	e_rdsel	= R_HPPA_RDSEL,
	e_lrsel	= R_HPPA_LRSEL,
	e_rrsel	= R_HPPA_RRSEL,
	e_psel	= R_HPPA_PSEL,	/* P'	: procedure address for shlib's	*/
	e_lpsel	= R_HPPA_LPSEL,	/* LP'	: L' for procedure addresses	*/
	e_rpsel	= R_HPPA_RPSEL,	/* RP'	: R' for procedure addresses	*/

	e_tsel	= R_HPPA_TSEL,	/* T'	: DLT-relative offset for shlib's	*/
	e_ltsel	= R_HPPA_LTSEL,	/* LT'	: L' for DLT-relative offsets	*/
	e_rtsel	= R_HPPA_RTSEL	/* RT'	: R' for DLT-relative offsets	*/
};

enum hppa_reloc_expr_type
{
	R_HPPA_E_ONE	= 0,
	R_HPPA_E_TWO	= 1,
	R_HPPA_E_PCREL	= 2,
	R_HPPA_E_CON	= 3,
	R_HPPA_E_PLABEL	= 7,
	R_HPPA_E_ABS	= 18
};

/* for compatibility */
enum hppa_reloc_expr_type_alt
{
	e_one	= R_HPPA_E_ONE,
	e_two	= R_HPPA_E_TWO,
	e_pcrel	= R_HPPA_E_PCREL,
	e_con	= R_HPPA_E_CON,
	e_plabel = R_HPPA_E_PLABEL,
	e_abs	= R_HPPA_E_ABS
};

#endif /* _LIBHPPA_H */
