/* Target-dependent code for MItsubishi D10V, for GDB.
   Copyright (C) 1996 Free Software Foundation, Inc.
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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*  Contributed by Martin Hunt, hunt@cygnus.com */

#include "defs.h"
#include "frame.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "value.h"
#include "inferior.h"
#include "dis-asm.h"  

void
d10v_pop_frame ()
{
}

CORE_ADDR
d10v_skip_prologue (start_pc)
     CORE_ADDR start_pc;
{
}
 
CORE_ADDR
d10v_frame_chain (frame)
     struct frame_info *frame;
{
  if (!inside_entry_file (frame->pc))
    return read_memory_integer (FRAME_FP (frame) + frame->f_offset, 4);
  else
    return 0;
}  

/* Put here the code to store, into a struct frame_saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special: the address we
   return for it IS the sp for the next frame. */
void
d10v_frame_find_saved_regs (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{                             
}

void
d10v_init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  struct frame_saved_regs dummy;
  if (fi->next)
    fi->pc = fi->next->return_pc;
  d10v_frame_find_saved_regs (fi, &dummy);
}

static void
show_regs (args, from_tty)
     char *args;
     int from_tty;
{
  long long num1, num2;
  printf_filtered ("PC=%04x (0x%x) PSW=%04x RPT_S=%04x RPT_E=%04x RPT_C=%04x\n",
                   read_register (PC_REGNUM), read_register (PC_REGNUM) << 2,
                   read_register (PSW_REGNUM),
                   read_register (24),
                   read_register (25),
                   read_register (23));
  printf_filtered ("R0-R7  %04x %04x %04x %04x %04x %04x %04x %04x\n",
                   read_register (0),
                   read_register (1),
                   read_register (2),
                   read_register (3),
                   read_register (4),
                   read_register (5),
                   read_register (6),
                   read_register (7));
  printf_filtered ("R8-R15 %04x %04x %04x %04x %04x %04x %04x %04x\n",
                   read_register (8), 
                   read_register (9),
                   read_register (10),
                   read_register (11),
                   read_register (12),
                   read_register (13),
                   read_register (14),
                   read_register (15));
  read_register_gen (A0_REGNUM, (char *)&num1);
  read_register_gen (A0_REGNUM+1, (char *)&num2);
  printf_filtered ("A0-A1  %010llx %010llx\n",num1, num2);
}              

void
_initialize_d10v_tdep ()
{
  struct cmd_list_element *c;
  tm_print_insn = print_insn_d10v;
  add_com ("regs", class_vars, show_regs, "Print all registers");
} 

CORE_ADDR
d10v_read_register_pid (regno, pid)
     int regno, pid;
{
  int save_pid;
  CORE_ADDR retval;

  if (pid == inferior_pid)
    return (read_register(regno)) << 2;

  save_pid = inferior_pid;
  inferior_pid = pid;
  retval = read_register (regno);
  inferior_pid = save_pid;
  return (retval << 2);
}

void
d10v_write_register_pid (regno, val, pid)
     int regno;
     LONGEST val;
     int pid;
{
  int save_pid;

  val >>= 2;

  if (pid == inferior_pid)
    {
      write_register (regno, val);
      return;
    }

  save_pid = inferior_pid;
  inferior_pid = pid;
  write_register (regno, val);
  inferior_pid = save_pid;
}
