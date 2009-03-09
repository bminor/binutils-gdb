/* ARC target-dependent stuff. Extension structure access functions
   Copyright 1995, 1997, 2000, 2001, 2004, 2005, 2007
   Free Software Foundation, Inc.

   This file is part of libopcodes.

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   It is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include <stdlib.h>
#include <stdio.h>
#include "bfd.h"
#include "arc-ext.h"
#include "elf/arc.h"

#include "libiberty.h"
#include "sysdep.h"

/* extension structure */
static struct arcExtMap arc_extension_map;

/* Get the name of an extension instruction.  */

const char *
arcExtMap_instName (int opcode, int insn, int *flags)
{
  /* Here the following tasks need to be done.  First of all, the opcode
     stored in the Extension Map is the real opcode.  However, the subopcode
     stored in the instruction to be disassembled is mangled.  We pass (in
     minor opcode), the instruction word.  Here we will un-mangle it and get
     the real subopcode which we can look for in the Extension Map.  This
     function is used both for the ARCTangent and the ARCompact, so we would
     also need some sort of a way to distinguish between the two
     architectures.  This is because the ARCTangent does not do any of this
     mangling so we have no issues there.  */

  /* If P[22:23] is 0 or 2 then un-mangle using iiiiiI.  If it is 1 then use
     iiiiIi.  Now, if P is 3 then check M[5:5] and if it is 0 then un-mangle
     using iiiiiI else iiiiii.  */

  unsigned char minor;
  struct ExtInstruction *temp;

  if (*flags != E_ARC_MACH_A4) /* ARCompact extension instructions.  */
    {
      /* 16-bit instructions.  */
      if (0x08 <= opcode && opcode <= 0x0b)
	{
	  unsigned char I, b, c, i;

	  I = (insn & 0xf800) >> 11;
	  b = (insn & 0x0700) >> 8;
	  c = (insn & 0x00e0) >> 5;
	  i = (insn & 0x001f);

	  if (i)
	    minor = i;
	  else
	    minor = (c == 0x07) ? b : c;
	}
      /* 32-bit instructions.  */
      else
	{
	  unsigned char P, M, I, A, B;

	  P = (insn & 0x00c00000) >> 22;
	  M = (insn & 0x00000020);
	  I = (insn & 0x003f0000) >> 16;
	  A = (insn & 0x0000003f);
	  B = ((insn & 0x07000000) >> 24) | ((insn & 0x00007000) >> 9);

	  if (I != 0x2f)
	    {
#ifndef UNMANGLED
	      switch (P)
		{
		case 3:
		  if (M)
		    {
		      minor = I;
		      break;
		    }
		case 0:
		case 2:
		  minor = (I >> 1) | ((I & 0x1) << 5);
		  break;
		case 1:
		  minor = (I >> 1) | (I & 0x1) | ((I & 0x2) << 4);
		}
#else
	      minor = I;
#endif
	    }
	  else
	    {
	      if (A != 0x3f)
		minor = A;
	      else
		minor = B;
	    }
	}
    }
  else /* ARCTangent extension instructions.  */
    minor = insn;

  temp = arc_extension_map.instructions[INST_HASH (opcode, minor)];
  while (temp)
    {
      if ((temp->major == opcode) && (temp->minor == minor))
	{
	  *flags = temp->flags;
	  return temp->name;
	}
      temp = temp->next;
    }

  return NULL;
}

/* get the name of an extension core register */
const char *
arcExtMap_coreRegName (int value)
{
  if (value < 32)
    return 0;
  return arc_extension_map.coreRegisters[value-32].name;
}

enum ExtReadWrite
arcExtMap_coreReadWrite (int value)
{
  if (value < 32)
    return REG_INVALID;
  return arc_extension_map.coreRegisters[value-32].rw;
}

#if 0
struct ExtAuxRegister *
arc_ExtMap_auxRegs ()
{
  return arc_extension_map.auxRegisters;
}
#endif

/* Get the name of an extension condition code.  */
const char *
arcExtMap_condCodeName (int value)
{
  if (value < 16)
    return 0;
  return arc_extension_map.condCodes[value-16];
}

/* Get the name of an extension aux register.  */

const char *
arcExtMap_auxRegName (long address)
{
  /* Walk the list of aux reg names and find the name.  */
  struct ExtAuxRegister *r;

  for (r = arc_extension_map.auxRegisters; r; r = r->next)
    {
      if (r->address == address)
	return (const char *)r->name;
    }
  return 0;
}

#if 0
/* Recursively free auxilliary register strcture pointers until
   the list is empty.  */
static void
clean_aux_registers (struct ExtAuxRegister *r)
{
  if (r -> next)
    {
      clean_aux_registers (r->next);
      free (r->name);
      free (r->next);
      r->next = NULL;
    }
  else
    free (r->name);
}

/* Free memory that has been allocated for the extensions. */

static void
cleanup_ext_map (void)
{
  struct ExtAuxRegister *r;
  struct ExtInstruction *insn;
  int i;

  /* Clean aux reg structure.  */
  r = arc_extension_map.auxRegisters;
  if (r)
    {
      (clean_aux_registers (r));
      free (r);
    }

  /* Clean instructions.  */
  for (i = INST_HASH_SIZE - 1; i >= 0; i--)
    {
      for (insn = arc_extension_map.instructions[i]; insn ; insn = insn->next)
	{
	  free (insn->name);
	  free (insn);
	}
    }

  /* Clean core reg struct.  */
  for (i = 0; i < NUM_EXT_CORE; i++)
    {
      if (arc_extension_map.coreRegisters[i].name)
	free (arc_extension_map.coreRegisters[i].name);
    }

  for (i = 0; i < NUM_EXT_COND; i++) {
    if (arc_extension_map.condCodes[i])
      free (arc_extension_map.condCodes[i]);
  }

  memset (&arc_extension_map, 0, sizeof (struct arcExtMap));
}
#endif

int
arcExtMap_add (void *base, unsigned long length)
{
  unsigned char *block = base;
  unsigned char *p = block;

  while (p && p < (block + length))
    {
      /* p[0] == length of record
	 p[1] == type of record
	 For instructions:
	   p[2]  = opcode
	   p[3]  = minor opcode (if opcode == 3)
	   p[4]  = flags
	   p[5]+ = name
	 For core regs and condition codes:
	   p[2]  = value
	   p[3]+ = name
	 For aux regs:
	   p[2..5] = value
	   p[6]+   = name
	     (value is p[2]<<24|p[3]<<16|p[4]<<8|p[5]) */
      if (p[0] == 0)
	return -1;

      switch (p[1])
	{ /* type */
	case EXT_INSTRUCTION:
	  {
	    char *insn_name = xstrdup ((char *) (p+5));
	    struct ExtInstruction *insn = XNEW (struct ExtInstruction);
	    int major = p[2];
	    int minor = p[3];
	    struct ExtInstruction **bucket
	      = &arc_extension_map.instructions[INST_HASH (major, minor)];

	    insn->name  = insn_name;
	    insn->major = major;
	    insn->minor = minor;
	    insn->flags = p[4];
	    insn->next  = *bucket;
	    *bucket = insn;
	    break;
	  }
	case EXT_CORE_REGISTER:
	  {
	    unsigned char number = p[2];
	    char *name = (char *) p+3;

	    arc_extension_map.coreRegisters[number-32].number = number;
	    arc_extension_map.coreRegisters[number-32].rw = REG_READWRITE;
	    arc_extension_map.coreRegisters[number-32].name = xstrdup (name);
	    break;
	  }
	case EXT_LONG_CORE_REGISTER:
	  {
	    unsigned char number = p[2];
	    char *name = (char *) p+7;
	    enum ExtReadWrite rw = p[6];

	    arc_extension_map.coreRegisters[number-32].number = number;
	    arc_extension_map.coreRegisters[number-32].rw = rw;
	    arc_extension_map.coreRegisters[number-32].name = xstrdup (name);
	  }
	case EXT_COND_CODE:
	  {
	    char *cc_name = xstrdup ((char *) (p+3));

	    arc_extension_map.condCodes[p[2]-16] = cc_name;
	    break;
	  }
	case EXT_AUX_REGISTER:
	  {
	    /* trickier -- need to store linked list to these */
	    struct ExtAuxRegister *newAuxRegister
	      = XNEW (struct ExtAuxRegister);
	    char *aux_name = xstrdup ((char *) (p+6));

	    newAuxRegister->name = aux_name;
	    newAuxRegister->address = p[2]<<24 | p[3]<<16 | p[4]<<8  | p[5];
	    newAuxRegister->next = arc_extension_map.auxRegisters;
	    arc_extension_map.auxRegisters = newAuxRegister;
	    break;
	  }
	default:
	  return -1;
	}
      p += p[0]; /* move to next record */
    }
  return 0;
}

/* Load extensions described in .arcextmap and .gnu.linkonce.arcextmap.* ELF
   section.  */
void
build_ARC_extmap (bfd *text_bfd)
{
  char *arcExtMap;
  bfd_size_type count;
  asection *p;

  for (p = text_bfd->sections; p != NULL; p = p->next)
    if (!strncmp (p->name,
		  ".gnu.linkonce.arcextmap.",
		  sizeof (".gnu.linkonce.arcextmap.")-1)
	|| !strcmp (p->name,".arcextmap"))
      {
        count = bfd_get_section_size (p);
        arcExtMap = (char *) xmalloc (count);
        if (bfd_get_section_contents (text_bfd, p, (PTR) arcExtMap, 0, count))
	  arcExtMap_add ((PTR) arcExtMap, count);
        free ((PTR) arcExtMap);
      }
}
