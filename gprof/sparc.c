/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "gprof.h"
#include "search_list.h"
#include "source.h"
#include "symtab.h"
#include "cg_arcs.h"
#include "corefile.h"
#include "hist.h"

    /*
     *        opcode of the `callf' instruction
     */
#define	CALL	(0xc0000000)

void sparc_find_call PARAMS ((Sym *, bfd_vma, bfd_vma));

void
sparc_find_call (parent, p_lowpc, p_highpc)
     Sym *parent;
     bfd_vma p_lowpc;
     bfd_vma p_highpc;
{
  bfd_vma pc, dest_pc;
  unsigned int insn;
  Sym *child;

  if (core_text_space == 0)
    {
      return;
    }
  if (p_lowpc < s_lowpc)
    {
      p_lowpc = s_lowpc;
    }
  if (p_highpc > s_highpc)
    {
      p_highpc = s_highpc;
    }
  DBG (CALLDEBUG, printf ("[find_call] %s: 0x%lx to 0x%lx\n",
			  parent->name, (unsigned long) p_lowpc,
			  (unsigned long) p_highpc));
  for (pc = (p_lowpc + 3) & ~(bfd_vma) 3; pc < p_highpc; pc += 4)
    {
      insn = bfd_get_32 (core_bfd, ((unsigned char *) core_text_space
				    + pc - core_text_sect->vma));
      if (insn & CALL)
	{
	  DBG (CALLDEBUG,
	       printf ("[find_call] 0x%lx: callf", (unsigned long) pc));
	  /*
	   * Regular pc relative addressing check that this is the
	   * address of a function.
	   */
	  dest_pc = pc + (((bfd_signed_vma) (insn & 0x3fffffff)
			   ^ 0x20000000) - 0x20000000);
	  if (dest_pc >= s_lowpc && dest_pc <= s_highpc)
	    {
	      child = sym_lookup (&symtab, dest_pc);
	      DBG (CALLDEBUG,
		   printf ("\tdest_pc=0x%lx, (name=%s, addr=0x%lx)\n",
			   (unsigned long) dest_pc, child->name,
			   (unsigned long) child->addr));
	      if (child->addr == dest_pc)
		{
		  /* a hit:  */
		  arc_add (parent, child, (unsigned long) 0);
		  continue;
		}
	    }
	  /*
	   * Something funny going on.
	   */
	  DBG (CALLDEBUG, printf ("\tbut it's a botch\n"));
	}
    }
}
