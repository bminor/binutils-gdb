/* tc.h - target cpu dependent

   Copyright (C) 1987, 1990, 1991, 1992 Free Software Foundation, Inc.

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

/* In theory (mine, at least!) the machine dependent part of the assembler
   should only have to include one file.  This one.  -- JF */

extern const pseudo_typeS md_pseudo_table[];

/* JF moved this here from as.h under the theory that nobody except MACHINE.c
   and write.c care about it anyway. */

struct relax_type
{
  /* Forward reach. Signed number. > 0. */
  long rlx_forward;
  /* Backward reach. Signed number. < 0. */
  long rlx_backward;

  /* Bytes length of this address. */
  unsigned char rlx_length;

  /* Next longer relax-state.  0 means there is no 'next' relax-state. */
  relax_substateT rlx_more;
};

typedef struct relax_type relax_typeS;

extern const relax_typeS md_relax_table[];	/* Define it in MACHINE.c */

extern int md_reloc_size;	/* Size of a relocation record */

extern void (*md_emit_relocations) ();

char *md_atof PARAMS ((int what_statement_type, char *literalP, int *sizeP));
int md_estimate_size_before_relax PARAMS ((fragS * fragP, segT segment));
int md_parse_option PARAMS ((char **argP, int *cntP, char ***vecP));
long md_pcrel_from PARAMS ((fixS * fixP));
short tc_coff_fix2rtype PARAMS ((fixS * fixP));
void md_assemble PARAMS ((char *str));
void md_begin PARAMS ((void));
void md_create_long_jump PARAMS ((char *ptr, long from_addr, long to_addr,
				  fragS * frag, symbolS * to_symbol));
void md_create_short_jump PARAMS ((char *ptr, long from_addr, long to_addr,
				   fragS * frag, symbolS * to_symbol));
void md_end PARAMS ((void));
void md_number_to_chars PARAMS ((char *buf, long val, int n));

#ifndef md_operand
void md_operand PARAMS ((expressionS * expressionP));
#endif

#ifdef BFD_ASSEMBLER
int md_apply_fix PARAMS ((fixS * fixP, long *val));
#ifndef md_convert_frag
void md_convert_frag PARAMS ((bfd * headers, segT sec, fragS * fragP));
#endif
#ifndef tc_headers_hook
void tc_headers_hook PARAMS ((segT *, fixS *));
#endif
#else
void md_apply_fix PARAMS ((fixS * fixP, long val));
#ifndef md_convert_frag
void md_convert_frag PARAMS ((object_headers * headers, fragS * fragP));
#endif

#ifndef tc_crawl_symbol_chain
void tc_crawl_symbol_chain PARAMS ((object_headers * headers));
#endif /* tc_crawl_symbol_chain */

#ifndef tc_headers_hook
void tc_headers_hook PARAMS ((object_headers * headers));
#endif /* tc_headers_hook */
#endif /* BFD_ASSEMBLER */

#ifndef md_section_align
long md_section_align PARAMS ((segT seg, long align));
#endif

#ifndef md_undefined_symbol
symbolS *md_undefined_symbol PARAMS ((char *name));
#endif

#define TC_COFF_SIZEMACHDEP(frag) tc_coff_sizemachdep(frag)
/* end of tc.h */


