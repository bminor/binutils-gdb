/* ns32k-opcode.h -- Opcode table for National Semi 32k processor
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


#ifdef SEQUENT_COMPATABILITY
#define DEF_MODEC 20
#define DEF_MODEL 21
#endif

#ifndef DEF_MODEC
#define DEF_MODEC 20
#endif

#ifndef DEF_MODEL
#define DEF_MODEL 20
#endif

#define MAX_ARGS 4
#define ARG_LEN 50

#ifdef __STDC__

void fix_new_ns32k(fragS *frag,
		   int where,
		   void *add_symbol, /* really symbolS */
		   void *sub_symbol, /* really symbolS */
		   long offset,
		   int pcrel,
		   int pcrel_adjust,
		   int im_disp,
		   void *bit_fixP, /* really bit_fixS */
		   int bsr);

#else /* __STDC__ */

void fix_new_ns32k();

#endif /* __STDC__ */


/* end of tc-ns32k.h */
