/* Table of opcodes for the AMD 29000
   Copyright (C) 1990, 1991 Free Software Foundation, Inc.
   Contributed by Cygnus Support.  Written by Jim Kingdon.

This file is part of GDB.

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

struct am29k_opcode {
  /* Name of the instruction.  */
  char *name;

  /* Opcode (i.e. most significant byte of the word).  */
  unsigned char opcode;

  /* A string of characters which describe the operands.
     Valid characters are:
     ,        Itself.  The character appears in the assembly code.
     a        RA.  The register number is in bits 8-15 of the instruction.
     b        RB.  The register number is in bits 0-7 of the instruction.
     c        RC.  The register number is in bits 16-23 of the instruction.
     i        An immediate operand is in bits 0-7 of the instruction.
     x        Bits 0-7 and 16-23 of the instruction are bits 0-7 and 8-15
              (respectively) of the immediate operand.
     h        Same as x but the instruction contains bits 16-31 of the
              immediate operand.
     X        Same as x but bits 16-31 of the signed immediate operand
              are set to 1 (thus the operand is always negative).
     P,A      Bits 0-7 and 16-23 of the instruction are bits 2-9 and 10-17
              (respectively) of the immediate operand.
	      P=PC-relative, sign-extended to 32 bits.
	      A=Absolute, zero-extended to 32 bits.
     e        CE bit (bit 23) for a load/store instruction.
     n        Control field (bits 16-22) for a load/store instruction.
     v        Immediate operand in bits 16-23 of the instruction.
              (used for trap numbers).
     s        SA.  Special-purpose register number in bits 8-15
              of the instruction.
     u        UI--bit 7 of the instruction.
     r        RND--bits 4-6 of the instruction.
     d        FD--bits 2-3 of the instruction.
     f        FS--bits 0-1 of the instruction.  */
  char *args;
};

static struct am29k_opcode am29k_opcodes[] =
{

{ "add", 0x14, "c,a,b" },
{ "add", 0x15, "c,a,i" },
{ "addc", 0x1c, "c,a,b" },
{ "addc", 0x1d, "c,a,i" },
{ "addcs", 0x18, "c,a,b" },
{ "addcs", 0x19, "c,a,i" },
{ "addcu", 0x1a, "c,a,b" },
{ "addcu", 0x1b, "c,a,i" },
{ "adds", 0x10, "c,a,b" },
{ "adds", 0x11, "c,a,i" },
{ "addu", 0x12, "c,a,b" },
{ "addu", 0x13, "c,a,i" },
{ "and", 0x90, "c,a,b" },
{ "and", 0x91, "c,a,i" },
{ "andn", 0x9c, "c,a,b" },
{ "andn", 0x9d, "c,a,i" },
{ "aseq", 0x70, "v,a,b" },
{ "aseq", 0x71, "v,a,i" },
{ "asge", 0x5c, "v,a,b" },
{ "asge", 0x5d, "v,a,i" },
{ "asgeu", 0x5e, "v,a,b" },
{ "asgeu", 0x5f, "v,a,i" },
{ "asgt", 0x58, "v,a,b" },
{ "asgt", 0x59, "v,a,i" },
{ "asgtu", 0x5a, "v,a,b" },
{ "asgtu", 0x5b, "v,a,i" },
{ "asle", 0x54, "v,a,b" },
{ "asle", 0x55, "v,a,i" },
{ "asleu", 0x56, "v,a,b" },
{ "asleu", 0x57, "v,a,i" },
{ "aslt", 0x50, "v,a,b" },
{ "aslt", 0x51, "v,a,i" },
{ "asltu", 0x52, "v,a,b" },
{ "asltu", 0x53, "v,a,i" },
{ "asneq", 0x72, "v,a,b" },
{ "asneq", 0x73, "v,a,i" },
{ "call", 0xa8, "a,P" },
{ "call", 0xa9, "a,A" },
{ "calli", 0xc8, "a,b" },
{ "class", 0xe6, "c,a,f" },
{ "clz", 0x08, "c,b" },
{ "clz", 0x09, "c,i" },
{ "const", 0x03, "a,x" },
{ "consth", 0x02, "a,h" },
{ "consthz", 0x05, "a,h" },
{ "constn", 0x01, "a,X" },
{ "convert", 0xe4, "c,a,u,r,d,f" },
{ "cpbyte", 0x2e, "c,a,b" },
{ "cpbyte", 0x2f, "c,a,i" },
{ "cpeq", 0x60, "c,a,b" },
{ "cpeq", 0x61, "c,a,i" },
{ "cpge", 0x4c, "c,a,b" },
{ "cpge", 0x4d, "c,a,i" },
{ "cpgeu", 0x4e, "c,a,b" },
{ "cpgeu", 0x4f, "c,a,i" },
{ "cpgt", 0x48, "c,a,b" },
{ "cpgt", 0x49, "c,a,i" },
{ "cpgtu", 0x4a, "c,a,b" },
{ "cpgtu", 0x4b, "c,a,i" },
{ "cple", 0x44, "c,a,b" },
{ "cple", 0x45, "c,a,i" },
{ "cpleu", 0x46, "c,a,b" },
{ "cpleu", 0x47, "c,a,i" },
{ "cplt", 0x40, "c,a,b" },
{ "cplt", 0x41, "c,a,i" },
{ "cpltu", 0x42, "c,a,b" },
{ "cpltu", 0x43, "c,a,i" },
{ "cpneq", 0x62, "c,a,b" },
{ "cpneq", 0x63, "c,a,i" },
{ "dadd", 0xf1, "c,a,b" },
{ "ddiv", 0xf7, "c,a,b" },
{ "deq", 0xeb, "c,a,b" },
{ "dge", 0xef, "c,a,b" },
{ "dgt", 0xed, "c,a,b" },
{ "div", 0x6a, "c,a,b" },
{ "div", 0x6b, "c,a,i" },
{ "div0", 0x68, "c,b" },
{ "div0", 0x69, "c,i" },
{ "divide", 0xe1, "c,a,b" },
{ "dividu", 0xe3, "c,a,b" },
{ "divl", 0x6c, "c,a,b" },
{ "divl", 0x6d, "c,a,i" },
{ "divrem", 0x6e, "c,a,b" },
{ "divrem", 0x6f, "c,a,i" },
{ "dmac", 0xd9, "F,C,a,b" },
{ "dmsm", 0xdb, "c,a,b" },
{ "dmul", 0xf5, "c,a,b" },
{ "dsub", 0xf3, "c,a,b" },
{ "emulate", 0xd7, "v,a,b" },
{ "exbyte", 0x0a, "c,a,b" },
{ "exbyte", 0x0b, "c,a,i" },
{ "exhw", 0x7c, "c,a,b" },
{ "exhw", 0x7d, "c,a,i" },
{ "exhws", 0x7e, "c,a" },
{ "extract", 0x7a, "c,a,b" },
{ "extract", 0x7b, "c,a,i" },
{ "fadd", 0xf0, "c,a,b" },
{ "fdiv", 0xf6, "c,a,b" },
{ "fdmul", 0xf9, "c,a,b" },
{ "feq", 0xea, "c,a,b" },
{ "fge", 0xee, "c,a,b" },
{ "fgt", 0xec, "c,a,b" },
{ "fmac", 0xd8, "F,C,a,b" },
{ "fmsm", 0xda, "c,a,b" },
{ "fmul", 0xf4, "c,a,b" },
{ "fsub", 0xf2, "c,a,b" },
{ "halt", 0x89, "" },
{ "inbyte", 0x0c, "c,a,b" },
{ "inbyte", 0x0d, "c,a,i" },
{ "inhw", 0x78, "c,a,b" },
{ "inhw", 0x79, "c,a,i" },
{ "inv", 0x9f, "" },
{ "iret", 0x88, "" },
{ "iretinv", 0x8c, "" },
{ "jmp", 0xa0, "P" },
{ "jmp", 0xa1, "A" },
{ "jmpf", 0xa4, "a,P" },
{ "jmpf", 0xa5, "a,A" },
{ "jmpfdec", 0xb4, "a,P" },
{ "jmpfdec", 0xb5, "a,A" },
{ "jmpfi", 0xc4, "a,b" },
{ "jmpi", 0xc0, "b" },
{ "jmpt", 0xac, "a,P" },
{ "jmpt", 0xad, "a,A" },
{ "jmpti", 0xcc, "a,b" },
{ "load", 0x16, "e,n,a,b" },
{ "load", 0x17, "e,n,a,i" },
{ "loadl", 0x06, "e,n,a,b" },
{ "loadl", 0x07, "e,n,a,i" },
{ "loadm", 0x36, "e,n,a,b" },
{ "loadm", 0x37, "e,n,a,i" },
{ "loadset", 0x26, "e,n,a,b" },
{ "loadset", 0x27, "e,n,a,i" },
{ "mfacc", 0xe9, "c,d,f" },
{ "mfsr", 0xc6, "c,s" },
{ "mftlb", 0xb6, "c,a" },
{ "mtacc", 0xe8, "c,d,f" },
{ "mtsr", 0xce, "s,b" },
{ "mtsrim", 0x04, "s,x" },
{ "mttlb", 0xbe, "a,b" },
{ "mul", 0x64, "c,a,b" },
{ "mul", 0x65, "c,a,i" },
{ "mull", 0x66, "c,a,b" },
{ "mull", 0x67, "c,a,i" },
{ "multiplu", 0xe2, "c,a,b" },
{ "multiply", 0xe0, "c,a,b" },
{ "multm", 0xde, "c,a,b" },
{ "multmu", 0xdf, "c,a,b" },
{ "mulu", 0x74, "c,a,b" },
{ "mulu", 0x75, "c,a,i" },
{ "nand", 0x9a, "c,a,b" },
{ "nand", 0x9b, "c,a,i" },
{ "nor", 0x98, "c,a,b" },
{ "nor", 0x99, "c,a,i" },
{ "or", 0x92, "c,a,b" },
{ "or", 0x93, "c,a,i" },
{ "orn", 0xaa, "c,a,b" },
{ "orn", 0xab, "c,a,i" },

/* The description of "setip" in Chapter 8 ("instruction set") of the user's
   manual claims that these are absolute register numbers.  But section
   7.2.1 explains that they are not.  The latter is correct, so print
   these normally ("lr0", "lr5", etc.).  */
{ "setip", 0x9e, "c,a,b" },

{ "sll", 0x80, "c,a,b" },
{ "sll", 0x81, "c,a,i" },
{ "sqrt", 0xe5, "c,a,f" },
{ "sra", 0x86, "c,a,b" },
{ "sra", 0x87, "c,a,i" },
{ "srl", 0x82, "c,a,b" },
{ "srl", 0x83, "c,a,i" },
{ "store", 0x1e, "e,n,a,b" },
{ "store", 0x1f, "e,n,a,i" },
{ "storel", 0x0e, "e,n,a,b" },
{ "storel", 0x0f, "e,n,a,i" },
{ "storem", 0x3e, "e,n,a,b" },
{ "storem", 0x3f, "e,n,a,i" },
{ "sub", 0x24, "c,a,b" },
{ "sub", 0x25, "c,a,i" },
{ "subc", 0x2c, "c,a,b" },
{ "subc", 0x2d, "c,a,i" },
{ "subcs", 0x28, "c,a,b" },
{ "subcs", 0x29, "c,a,i" },
{ "subcu", 0x2a, "c,a,b" },
{ "subcu", 0x2b, "c,a,i" },
{ "subr", 0x34, "c,a,b" },
{ "subr", 0x35, "c,a,i" },
{ "subrc", 0x3c, "c,a,b" },
{ "subrc", 0x3d, "c,a,i" },
{ "subrcs", 0x38, "c,a,b" },
{ "subrcs", 0x39, "c,a,i" },
{ "subrcu", 0x3a, "c,a,b" },
{ "subrcu", 0x3b, "c,a,i" },
{ "subrs", 0x30, "c,a,b" },
{ "subrs", 0x31, "c,a,i" },
{ "subru", 0x32, "c,a,b" },
{ "subru", 0x33, "c,a,i" },
{ "subs", 0x20, "c,a,b" },
{ "subs", 0x21, "c,a,i" },
{ "subu", 0x22, "c,a,b" },
{ "subu", 0x23, "c,a,i" },
{ "xnor", 0x96, "c,a,b" },
{ "xnor", 0x97, "c,a,i" },
{ "xor", 0x94, "c,a,b" },
{ "xor", 0x95, "c,a,i" }

};

#define NUM_OPCODES ((sizeof am29k_opcodes) / (sizeof am29k_opcodes[0]))

