/* tc-sparc.h - Macros and type defines for the sparc.
   Copyright (C) 1989, 1990, 1991, 1992 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2,
   or (at your option) any later version.

   GAS is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with GAS; see the file COPYING.  If not, write
   to the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#define TC_SPARC 1

#define LOCAL_LABELS_FB

#define TARGET_ARCH bfd_arch_sparc
#ifdef OBJ_AOUT
#define TARGET_FORMAT "a.out-sunos-big"
#endif
#ifdef OBJ_BOUT
#define TARGET_FORMAT "b.out.big"
#endif
#ifdef OBJ_ELF
#ifndef sparcv9
#define TARGET_FORMAT "elf32-sparc"
#else	/* sparcv9 */
#define TARGET_FORMAT "elf64-sparc" /* v9 */
#define ENV64			/* v9 */
#endif	/* sparcv9 */
#define LOCAL_LABEL(name)	((name)[0] == '.' || !strncmp ((name), "_.L_", 4))
#endif
#define WORKING_DOT_WORD

#define md_convert_frag(b,s,f)		{as_fatal ("sparc convert_frag\n");}
#define md_create_long_jump(p,f,t,fr,s)	as_fatal("sparc_create_long_jump")
#define md_create_short_jump(p,f,t,fr,s) as_fatal("sparc_create_short_jump")
#define md_estimate_size_before_relax(f,s) \
			(as_fatal("estimate_size_before_relax called"),1)
void tc_aout_pre_write_hook ();

#define LISTING_HEADER "SPARC GAS "

/* end of tc-sparc.h */
