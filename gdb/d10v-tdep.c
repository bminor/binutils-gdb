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
#include "gdb_string.h"
#include "value.h"
#include "inferior.h"
#include "dis-asm.h"  
#include "symfile.h"
#include "objfiles.h"

void d10v_frame_find_saved_regs PARAMS ((struct frame_info *fi, struct frame_saved_regs *fsr));
static void d10v_pop_dummy_frame PARAMS ((struct frame_info *fi));

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */

void
d10v_pop_frame ()
{
  struct frame_info *frame = get_current_frame ();
  CORE_ADDR fp;
  int regnum;
  struct frame_saved_regs fsr;
  char raw_buffer[8];

  fp = FRAME_FP (frame);
  printf("pop_frame %x\n",(int)fp);
  if (frame->dummy)
    {
      d10v_pop_dummy_frame(frame);
      return;
    }

  /* fill out fsr with the address of where each */
  /* register was stored in the frame */
  get_frame_saved_regs (frame, &fsr);
  
  /* now update the current registers with the old values */
  for (regnum = A0_REGNUM; regnum < A0_REGNUM+2 ; regnum++)
    {
      if (fsr.regs[regnum])
	{
	  read_memory (fsr.regs[regnum], raw_buffer, 8);
	  write_register_bytes (REGISTER_BYTE (regnum), raw_buffer, 8);
	}
    }
  for (regnum = 0; regnum < SP_REGNUM; regnum++)
    {
      if (fsr.regs[regnum])
	{
	  write_register (regnum, read_memory_unsigned_integer (fsr.regs[regnum], 2));
	}
    }
  if (fsr.regs[PSW_REGNUM])
    {
      write_register (PSW_REGNUM, read_memory_unsigned_integer (fsr.regs[PSW_REGNUM], 2));
    }

  write_register (PC_REGNUM, read_register(13));
  write_register (SP_REGNUM, fp + frame->size);
  target_store_registers (-1);
  flush_cached_frames ();
}

static int 
check_prologue (op)
     unsigned short op;
{
  /* st  rn, @-sp */
  if ((op & 0x7E1F) == 0x6C1F)
    return 1;

  /* st2w  rn, @-sp */
  if ((op & 0x7E3F) == 0x6E1F)
    return 1;

  /* subi  sp, n */
  if ((op & 0x7FE1) == 0x01E1)
    return 1;

  /* mv  r11, sp */
  if (op == 0x417E)
    return 1;

  /* nop */
  if (op == 0x5E00)
    return 1;

  /* st  rn, @sp */
  if ((op & 0x7E1F) == 0x681E)
    return 1;

  /* st2w  rn, @sp */
 if ((op & 0x7E3F) == 0x3A1E)
   return 1;

  return 0;
}

CORE_ADDR
d10v_skip_prologue (pc)
     CORE_ADDR pc;
{
  unsigned long op;
  unsigned short op1, op2;

  if (target_read_memory (pc, (char *)&op, 4))
    return pc;			/* Can't access it -- assume no prologue. */

  while (1)
    {
      op = (unsigned long)read_memory_integer (pc, 4);
      if ((op & 0xC0000000) == 0xC0000000)
	{
	  /* long instruction */
	  if ( ((op & 0x3FFF0000) != 0x01FF0000) &&   /* add3 sp,sp,n */
	       ((op & 0x3F0F0000) != 0x340F0000) &&   /* st  rn, @(offset,sp) */
 	       ((op & 0x3F1F0000) != 0x350F0000))     /* st2w  rn, @(offset,sp) */
	    break;
	}
      else
	{
	  /* short instructions */
	  op1 = (op & 0x3FFF8000) >> 15;
	  op2 = op & 0x7FFF;
	  if (!check_prologue(op1) || !check_prologue(op2))
	    break;
	}
      pc += 4;
    }
  return pc;
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.
*/

CORE_ADDR
d10v_frame_chain (frame)
     struct frame_info *frame;
{
  struct frame_saved_regs fsr;

  if (inside_entry_file (frame->pc))
    return 0;

  d10v_frame_find_saved_regs (frame, &fsr);

  if (!fsr.regs[FP_REGNUM])
    {
      return (CORE_ADDR)fsr.regs[SP_REGNUM]+0x2000000;
    }
  return read_memory_unsigned_integer(fsr.regs[FP_REGNUM],2)+0x2000000;
}  

static int next_addr;

static int 
prologue_find_regs (op, fsr, addr)
     unsigned short op;
     struct frame_saved_regs *fsr;
     CORE_ADDR addr;
{
  int n;

  /* st  rn, @-sp */
  if ((op & 0x7E1F) == 0x6C1F)
    {
      n = (op & 0x1E0) >> 5;
      next_addr -= 2;
      fsr->regs[n] = next_addr;
      return 1;
    }

  /* st2w  rn, @-sp */
  else if ((op & 0x7E3F) == 0x6E1F)
    {
      n = (op & 0x1E0) >> 5;
      next_addr -= 4;
      fsr->regs[n] = next_addr;
      fsr->regs[n+1] = next_addr+2;
      return 1;
    }

  /* subi  sp, n */
  if ((op & 0x7FE1) == 0x01E1)
    {
      n = (op & 0x1E) >> 1;
      if (n == 0)
	n = 16;
      next_addr -= n;
      return 1;
    }

  /* mv  r11, sp */
  if (op == 0x417E)
      return 1;

  /* nop */
  if (op == 0x5E00)
    return 1;

  /* st  rn, @sp */
  if ((op & 0x7E1F) == 0x681E)
    {
      n = (op & 0x1E0) >> 5;
      fsr->regs[n] = next_addr;
      return 1;
    }

  /* st2w  rn, @sp */
  if ((op & 0x7E3F) == 0x3A1E)
    {
      n = (op & 0x1E0) >> 5;
      fsr->regs[n] = next_addr;
      fsr->regs[n+1] = next_addr+2;
      return 1;
    }

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
  CORE_ADDR fp, pc;
  unsigned long op;
  unsigned short op1, op2;
  int i;

  fp = fi->frame;
  memset (fsr, 0, sizeof (*fsr));
  next_addr = 0;

  pc = get_pc_function_start (fi->pc);

  while (1)
    {
      op = (unsigned long)read_memory_integer (pc, 4);
      if ((op & 0xC0000000) == 0xC0000000)
	{
	  /* long instruction */
	  if ((op & 0x3FFF0000) == 0x01FF0000)
	    {
	      /* add3 sp,sp,n */
	      short n = op & 0xFFFF;
	      next_addr += n;
	    }
	  else if ((op & 0x3F0F0000) == 0x340F0000)
	    {
	      /* st  rn, @(offset,sp) */
	      short offset = op & 0xFFFF;
	      short n = (op >> 20) & 0xF;
	      fsr->regs[n] = next_addr + offset;
	    }
	  else if ((op & 0x3F1F0000) == 0x350F0000)
	    {
	      /* st2w  rn, @(offset,sp) */
	      short offset = op & 0xFFFF;
	      short n = (op >> 20) & 0xF;
	      fsr->regs[n] = next_addr + offset;
	      fsr->regs[n+1] = next_addr + offset + 2;
	    }
	  else
	    break;
	}
      else
	{
	  /* short instructions */
	  op1 = (op & 0x3FFF8000) >> 15;
	  op2 = op & 0x7FFF;
	  if (!prologue_find_regs(op1,fsr,pc) || !prologue_find_regs(op2,fsr,pc))
	    break;
	}
      pc += 4;
    }
  
  fi->size = -next_addr;

  for (i=0; i<NUM_REGS-1; i++)
    if (fsr->regs[i])
      {
	fsr->regs[i] = fp - (next_addr - fsr->regs[i]); 
      }

  if (fsr->regs[13])
    fi->return_pc = ((read_memory_unsigned_integer(fsr->regs[13],2)-1) << 2) + 0x1000000;
  else
    fi->return_pc = ((read_register(13) - 1)  << 2) + 0x1000000;

  /* th SP is not normally (ever?) saved, but check anyway */
  if (!fsr->regs[SP_REGNUM])
    {
      /* if the FP was saved, that means the current FP is valid, */
      /* otherwise, it isn't being used, so we use the SP instead */
      if (fsr->regs[FP_REGNUM])
	fsr->regs[SP_REGNUM] = read_register(FP_REGNUM) + fi->size;
      else
	fsr->regs[SP_REGNUM] = read_register(SP_REGNUM) + fi->size;
    }
}

void
d10v_init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  struct frame_saved_regs dummy;

  if (fi->next && (fi->pc == 0)) 
    fi->pc = fi->next->return_pc; 

  /*
  printf("init_extra_frame_info: fi->pc=%x  frame=%x  PC=%x SP=%x\n",
	 (int)fi->pc,(int)fi->frame,(int)read_register(PC_REGNUM)<<2,(int)read_register(SP_REGNUM));
	 */

  d10v_frame_find_saved_regs (fi, &dummy); 
  if (!dummy.regs[FP_REGNUM])
    {
      /*      printf("init_extra_frame_info: sp=%x  size=%x\n",dummy.regs[SP_REGNUM],fi->size);; */
      fi->frame = dummy.regs[SP_REGNUM] - fi->size + 0x2000000;
      d10v_frame_find_saved_regs (fi, &dummy); 
    }
  /*  printf("init_extra_frame_info end: pc=%x  frame=%x\n",(int)fi->pc,(int)fi->frame); */
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
  printf_filtered ("IMAP0 %04x    IMAP1 %04x    DMAP %04x\n",
                   read_register (IMAP0_REGNUM),
                   read_register (IMAP1_REGNUM),
                   read_register (DMAP_REGNUM));
  read_register_gen (A0_REGNUM, (char *)&num1);
  read_register_gen (A0_REGNUM+1, (char *)&num2);
  printf_filtered ("A0-A1  %010llx %010llx\n",num1, num2);
}

void
_initialize_d10v_tdep ()
{
  tm_print_insn = print_insn_d10v;
  add_com ("regs", class_vars, show_regs, "Print all registers");
} 

static CORE_ADDR
d10v_xlate_addr (addr)
     int addr;
{
  int imap;

  if (addr < 0x20000)
    imap = (int)read_register(IMAP0_REGNUM);
  else
    imap = (int)read_register(IMAP1_REGNUM);

  if (imap & 0x1000)
    return (CORE_ADDR)(addr + 0x1000000);
  return (CORE_ADDR)(addr + (imap & 0xff)*0x20000);
}


CORE_ADDR
d10v_read_pc (pid)
     int pid;
{
  int save_pid, retval;

  save_pid = inferior_pid;
  inferior_pid = pid;
  retval = (int)read_register (PC_REGNUM);
  inferior_pid = save_pid;
  return d10v_xlate_addr(retval << 2);
}

void
d10v_write_pc (val, pid)
     LONGEST val;
     int pid;
{
  int save_pid;

  save_pid = inferior_pid;
  inferior_pid = pid;
  write_register (PC_REGNUM, (val & 0x3ffff) >> 2);
  inferior_pid = save_pid;
}

CORE_ADDR
d10v_read_fp ()
{
  return (read_register(FP_REGNUM) + 0x2000000);
}

void
d10v_write_fp (val)
     LONGEST val;
{
  write_register (FP_REGNUM, val & 0xffff);
}

void
d10v_fix_call_dummy (dummyname, start_sp, fun, nargs, args, type, gcc_p)
     char *dummyname;
     CORE_ADDR start_sp;
     CORE_ADDR fun;
     int nargs;
     value_ptr *args;
     struct type *type;
     int gcc_p;
{
  int regnum, i;
  CORE_ADDR sp;
  char buffer[MAX_REGISTER_RAW_SIZE];
  struct frame_info *frame = get_current_frame ();
  frame->dummy = 1;
  printf("D10v_fix_call_dummy: %x %x %d  frame=%x\n",(int)start_sp,(int)fun,nargs,(int)frame->frame);
  sp = start_sp;
  for (regnum = 0; regnum < NUM_REGS-1; regnum++)
    {
      store_address (buffer, REGISTER_RAW_SIZE(regnum), read_register(regnum));
      write_memory (sp, buffer, REGISTER_RAW_SIZE(regnum));
      sp -= REGISTER_RAW_SIZE(regnum);
    }
  write_register (SP_REGNUM, (LONGEST)sp);  
  printf("writing %x to sp\n",(int)sp);
  /* now we need to load LR with the return address */
  write_register (LR_REGNUM, (LONGEST)d10v_call_dummy_address()>>2);  
}

static void
d10v_pop_dummy_frame (fi)
     struct frame_info *fi;
{
  printf("pop_dummy_frame:  start_sp=%x\n",(int)fi->frame);
  /*
  sp = start_sp;
  for (regnum = 0; regnum < NUM_REGS-1; regnum++)
    {
      store_address (buffer, REGISTER_RAW_SIZE(regnum), read_register(regnum));
      write_memory (sp, buffer, REGISTER_RAW_SIZE(regnum));
      sp -= REGISTER_RAW_SIZE(regnum);
    }
    */
}


CORE_ADDR
d10v_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     int struct_return;
     CORE_ADDR struct_addr;
{
  int i, len, regnum=2;
  char *contents;
  LONGEST val;
  
  for (i = 0; i < nargs; i++)
    {
      value_ptr arg = args[i];
      struct type *arg_type = check_typedef (VALUE_TYPE (arg));
      switch (TYPE_CODE (arg_type))
        {
	case TYPE_CODE_INT:
	case TYPE_CODE_BOOL:
	case TYPE_CODE_CHAR:
	case TYPE_CODE_RANGE:
	case TYPE_CODE_ENUM:
	  break;
	default:
	  break;
	}
      len = TYPE_LENGTH (arg_type);
      contents = VALUE_CONTENTS(arg);
      val = extract_signed_integer (contents, len);
      printf("arg %d:  len=%d  contents=%x\n",i,len,(int)val);
      if (len == 4)
	write_register (regnum++, val>>16);
      write_register (regnum++, val & 0xffff);
    }
}


CORE_ADDR
d10v_call_dummy_address ()
{
  CORE_ADDR entry;
  struct minimal_symbol *sym;

  entry = entry_point_address ();

  if (entry != 0)
    return entry;

  sym = lookup_minimal_symbol ("_start", NULL, symfile_objfile);

  if (!sym || MSYMBOL_TYPE (sym) != mst_text)
    return 0;
  else
    return SYMBOL_VALUE_ADDRESS (sym);
}

/* Given a return value in `regbuf' with a type `valtype', 
   extract and copy its value into `valbuf'.  */

void
d10v_extract_return_value (valtype, regbuf, valbuf)
     struct type *valtype;
     char regbuf[REGISTER_BYTES];
     char *valbuf;
{
  printf("EXTRACT: regbuf=%x, *regbuf=%x %x %x %x  len=%d %d\n",(int)regbuf,*(int *)regbuf,
	 *(int *)(regbuf+4),*(int *)(regbuf+8),*(int *)(regbuf+12),TYPE_LENGTH (valtype),REGISTER_BYTE (2) );
  memcpy (valbuf, regbuf + REGISTER_BYTE (2), TYPE_LENGTH (valtype));
}
