/*
 * Copyright (c) 1983, 1998 Regents of the University of California.
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

static Sym indirect_child;

void mips_find_call PARAMS ((Sym *, bfd_vma, bfd_vma));

void
mips_find_call (parent, p_lowpc, p_highpc)
     Sym *parent;
     bfd_vma p_lowpc;
     bfd_vma p_highpc;
{
  bfd_vma pc, dest_pc;
  unsigned int op;
  int offset;
  Sym *child;
  static bool inited = FALSE;

  if (!inited)
    {
      inited = TRUE;
      sym_init (&indirect_child);
      indirect_child.name = _("<indirect child>");
      indirect_child.cg.prop.fract = 1.0;
      indirect_child.cg.cyc.head = &indirect_child;
    }

  if (!core_text_space)
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
  DBG (CALLDEBUG, printf (_("[find_call] %s: 0x%lx to 0x%lx\n"),
			  parent->name, (unsigned long) p_lowpc,
			  (unsigned long) p_highpc));
  for (pc = p_lowpc; pc < p_highpc; pc += 4)
    {
      op = bfd_get_32 (core_bfd, &((char *)core_text_space)[pc - s_lowpc]);
      if ((op & 0xfc000000) == 0x0c000000)
	{
	  /* This is a "jal" instruction.  Check that the destination
	     is the address of a function.  */
	  DBG (CALLDEBUG,
	       printf (_("[find_call] 0x%lx: jal"), (unsigned long) pc));
          offset = (op & 0x03ffffff) << 2;
	  dest_pc = (pc & ~0xfffffff) | offset;
	  if (dest_pc >= s_lowpc && dest_pc <= s_highpc)
	    {
	      child = sym_lookup (&symtab, dest_pc);
	      DBG (CALLDEBUG,
		   printf (" 0x%lx\t; name=%s, addr=0x%lx",
			   (unsigned long) dest_pc, child->name,
			   (unsigned long) child->addr));
	      if (child->addr == dest_pc)
		{
		  DBG (CALLDEBUG, printf ("\n"));
		  /* a hit:  */
		  arc_add (parent, child, (unsigned long) 0);
		  continue;
		}
	    }
	  /* Something funny going on.  */
	  DBG (CALLDEBUG, printf ("\tbut it's a botch\n"));
	}
      else if ((op & 0xfc00f83f) == 0x0000f809)
	{
	  /* This is a "jalr" instruction (indirect call).  */
	  DBG (CALLDEBUG,
	       printf (_("[find_call] 0x%lx: jalr\n"), (unsigned long) pc));
	  arc_add (parent, &indirect_child, (unsigned long) 0);
	}
    }
}
