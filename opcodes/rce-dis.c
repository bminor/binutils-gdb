/* Disassemble RCE instructions.
   Copyright (C) 1993 Free Software Foundation, Inc.

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

#include <stdio.h>
#define STATIC_TABLE
#define DEFINE_TABLE

#include "rce-opc.h"
#include "dis-asm.h"

int 
print_insn_sop(memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  fprintf_ftype fprintf = info->fprintf_func;
  void *stream = info->stream;
  unsigned  char insn[2];
  unsigned  char nibs[4];
  int status;
  int relmask = ~0;
  rce_opcode_info *op;
  int dslot = 0;
  
  status = info->read_memory_func(memaddr, insn, 2, info);

  if (status != 0) 
    {
      info->memory_error_func(status, memaddr, info);
      return -1;
    }
  /**** DISASSEMBLY NOT YET IMPLEMENTED ****/
  fprintf(stream,".word 0x%02x%02x", insn[0], insn[1]);
  return 2;
}
