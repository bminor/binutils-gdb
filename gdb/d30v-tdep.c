/* Target-dependent code for Mitsubishi D30V, for GDB.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

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

void d30v_frame_find_saved_regs PARAMS ((struct frame_info *fi,
					 struct frame_saved_regs *fsr));
static void d30v_pop_dummy_frame PARAMS ((struct frame_info *fi));
static void d30v_print_flags PARAMS ((void));
static void print_flags_command PARAMS ((char *, int));

/* Discard from the stack the innermost frame, restoring all saved
   registers.  */

void
d30v_pop_frame ()
{
  struct frame_info *frame = get_current_frame ();
  CORE_ADDR fp;
  int regnum;
  struct frame_saved_regs fsr;
  char raw_buffer[8];

  fp = FRAME_FP (frame);
  if (frame->dummy)
    {
      d30v_pop_dummy_frame(frame);
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
d30v_skip_prologue (pc)
     CORE_ADDR pc;
{
  unsigned long op;
  unsigned short op1, op2;
  CORE_ADDR func_addr, func_end;
  struct symtab_and_line sal;

  /* XXX -- these need to be updated, the instruction patterns are actually
     for the d10v, not the d30v.  */
  /* If we have line debugging information, then the end of the */
  /* prologue should the first assembly instruction of  the first source line */
  if (find_pc_partial_function (pc, NULL, &func_addr, &func_end))
    {
      sal = find_pc_line (func_addr, 0);
      if ( sal.end && sal.end < func_end)
	return sal.end;
    }
  
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
	  if ((op & 0xC0000000) == 0x80000000)
	    {
	      op2 = (op & 0x3FFF8000) >> 15;
	      op1 = op & 0x7FFF;
	    } 
	  else 
	    {
	      op1 = (op & 0x3FFF8000) >> 15;
	      op2 = op & 0x7FFF;
	    }
	  if (check_prologue(op1))
	    {
	      if (!check_prologue(op2))
		{
		  /* if the previous opcode was really part of the prologue */
		  /* and not just a NOP, then we want to break after both instructions */
		  if (op1 != 0x5E00)
		    pc += 4;
		  break;
		}
	    }
	  else
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
d30v_frame_chain (frame)
     struct frame_info *frame;
{
  struct frame_saved_regs fsr;

  d30v_frame_find_saved_regs (frame, &fsr);

  if (frame->return_pc == IMEM_START)
    return (CORE_ADDR)0;

  if (!fsr.regs[FP_REGNUM])
    {
      if (!fsr.regs[SP_REGNUM] || fsr.regs[SP_REGNUM] == STACK_START)
	return (CORE_ADDR)0;
      
      return fsr.regs[SP_REGNUM];
    }

  if (!read_memory_unsigned_integer(fsr.regs[FP_REGNUM],2))
    return (CORE_ADDR)0;

  return read_memory_unsigned_integer(fsr.regs[FP_REGNUM],2)| DMEM_START;
}  

static int next_addr, uses_frame;

static int 
prologue_find_regs (op, fsr, addr)
     unsigned short op;
     struct frame_saved_regs *fsr;
     CORE_ADDR addr;
{
  int n;

  /* XXX -- these need to be updated, the instruction patterns are actually
     for the d10v, not the d30v.  */
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
    {
      uses_frame = 1;
      return 1;
    }

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
/* XXX -- these need to be updated, the instruction patterns are actually
   for the d10v, not the d30v.  */
void
d30v_frame_find_saved_regs (fi, fsr)
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

  uses_frame = 0;
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
	  if ((op & 0xC0000000) == 0x80000000)
	    {
	      op2 = (op & 0x3FFF8000) >> 15;
	      op1 = op & 0x7FFF;
	    } 
	  else 
	    {
	      op1 = (op & 0x3FFF8000) >> 15;
	      op2 = op & 0x7FFF;
	    }
	  if (!prologue_find_regs(op1,fsr,pc) || !prologue_find_regs(op2,fsr,pc))
	    break;
	}
      pc += 4;
    }
  
  fi->size = -next_addr;

  if (!(fp & 0xffff))
    fp = read_register(SP_REGNUM) | DMEM_START;

  for (i=0; i<NUM_REGS-1; i++)
    if (fsr->regs[i])
      {
	fsr->regs[i] = fp - (next_addr - fsr->regs[i]); 
      }

  if (fsr->regs[LR_REGNUM])
    fi->return_pc = ((read_memory_unsigned_integer(fsr->regs[LR_REGNUM],2) - 1) << 2) | IMEM_START;
  else
    fi->return_pc = ((read_register(LR_REGNUM) - 1) << 2) | IMEM_START;
  
  /* th SP is not normally (ever?) saved, but check anyway */
  if (!fsr->regs[SP_REGNUM])
    {
      /* if the FP was saved, that means the current FP is valid, */
      /* otherwise, it isn't being used, so we use the SP instead */
      if (uses_frame)
	fsr->regs[SP_REGNUM] = read_register(FP_REGNUM) + fi->size;
      else
	{
	  fsr->regs[SP_REGNUM] = fp + fi->size;
	  fi->frameless = 1;
	  fsr->regs[FP_REGNUM] = 0;
	}
    }
}

void
d30v_init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  struct frame_saved_regs dummy;

  if (fi->next && ((fi->pc & 0xffff) == 0)) 
    fi->pc = fi->next->return_pc; 

  d30v_frame_find_saved_regs (fi, &dummy);
}

static void d30v_print_register PARAMS ((int regnum, int tabular));

static void
d30v_print_register (regnum, tabular)
     int regnum;
     int tabular;
{
  if (regnum < A0_REGNUM)
    {
      if (tabular)
	printf_filtered ("%08x", read_register (regnum));
      else
	printf_filtered ("0x%x	%d", read_register (regnum),
			 read_register (regnum));
    }
  else
    {
      char regbuf[MAX_REGISTER_RAW_SIZE];

      read_relative_register_raw_bytes (regnum, regbuf);

      val_print (REGISTER_VIRTUAL_TYPE (regnum), regbuf, 0,
		 gdb_stdout, 'x', 1, 0, Val_pretty_default);

      if (!tabular)
	{
	  printf_filtered ("	");
	  val_print (REGISTER_VIRTUAL_TYPE (regnum), regbuf, 0,
		 gdb_stdout, 'd', 1, 0, Val_pretty_default);
	}
    }
}

static void
d30v_print_flags ()
{
  long psw = read_register (PSW_REGNUM);
  printf_filtered ("flags #1");
  printf_filtered ("   (sm) %d", (psw & PSW_SM) != 0);
  printf_filtered ("   (ea) %d", (psw & PSW_EA) != 0);
  printf_filtered ("   (db) %d", (psw & PSW_DB) != 0);
  printf_filtered ("   (ds) %d", (psw & PSW_DS) != 0);
  printf_filtered ("   (ie) %d", (psw & PSW_IE) != 0);
  printf_filtered ("   (rp) %d", (psw & PSW_RP) != 0);
  printf_filtered ("   (md) %d\n", (psw & PSW_MD) != 0);

  printf_filtered ("flags #2");
  printf_filtered ("   (f0) %d", (psw & PSW_F0) != 0);
  printf_filtered ("   (f1) %d", (psw & PSW_F1) != 0);
  printf_filtered ("   (f2) %d", (psw & PSW_F2) != 0);
  printf_filtered ("   (f3) %d", (psw & PSW_F3) != 0);
  printf_filtered ("    (s) %d", (psw & PSW_S) != 0);
  printf_filtered ("    (v) %d", (psw & PSW_V) != 0);
  printf_filtered ("   (va) %d", (psw & PSW_VA) != 0);
  printf_filtered ("    (c) %d\n", (psw & PSW_C) != 0);
}

static void
print_flags_command (args, from_tty)
     char *args;
     int from_tty;
{
  d30v_print_flags ();
}

void
d30v_do_registers_info (regnum, fpregs)
     int regnum;
     int fpregs;
{
  long long num1, num2;
  long psw;

  if (regnum != -1)
    {
      if (reg_names[0] == NULL || reg_names[0][0] == '\000')
	return;

      printf_filtered ("%s ", reg_names[regnum]);
      d30v_print_register (regnum, 0);

      printf_filtered ("\n");
      return;
    }

  /* Have to print all the registers.  Format them nicely.  */

  printf_filtered ("PC=");
  print_address (read_pc (), gdb_stdout);

  printf_filtered (" PSW=");
  d30v_print_register (PSW_REGNUM, 1);

  printf_filtered (" BPC=");
  print_address (read_register (BPC_REGNUM), gdb_stdout);

  printf_filtered (" BPSW=");
  d30v_print_register (BPSW_REGNUM, 1);
  printf_filtered ("\n");

  printf_filtered ("DPC=");
  print_address (read_register (DPC_REGNUM), gdb_stdout);

  printf_filtered (" DPSW=");
  d30v_print_register (DPSW_REGNUM, 1);

  printf_filtered (" IBA=");
  print_address (read_register (IBA_REGNUM), gdb_stdout);
  printf_filtered ("\n");

  printf_filtered ("RPT_C=");
  d30v_print_register (RPT_C_REGNUM, 1);

  printf_filtered (" RPT_S=");
  print_address (read_register (RPT_S_REGNUM), gdb_stdout);

  printf_filtered (" RPT_E=");
  print_address (read_register (RPT_E_REGNUM), gdb_stdout);
  printf_filtered ("\n");

  printf_filtered ("MOD_S=");
  print_address (read_register (MOD_S_REGNUM), gdb_stdout);

  printf_filtered (" MOD_E=");
  print_address (read_register (MOD_E_REGNUM), gdb_stdout);
  printf_filtered ("\n");

  printf_filtered ("EIT_VB=");
  print_address (read_register (EIT_VB_REGNUM), gdb_stdout);

  printf_filtered (" INT_S=");
  d30v_print_register (INT_S_REGNUM, 1);

  printf_filtered (" INT_M=");
  d30v_print_register (INT_M_REGNUM, 1);
  printf_filtered ("\n");

  d30v_print_flags ();
  for (regnum = 0; regnum <= 63;)
    {
      int i;

      printf_filtered ("R%d-R%d ", regnum, regnum + 7);
      if (regnum < 10)
	printf_filtered (" ");
      if (regnum + 7 < 10)
	printf_filtered (" ");

      for (i = 0; i < 8; i++)
	{
	  printf_filtered (" ");
	  d30v_print_register (regnum++, 1);
	}

      printf_filtered ("\n");
    }

  printf_filtered ("A0-A1    ");

  d30v_print_register (A0_REGNUM, 1);
  printf_filtered ("    ");
  d30v_print_register (A1_REGNUM, 1);
  printf_filtered ("\n");
}

CORE_ADDR
d30v_fix_call_dummy (dummyname, start_sp, fun, nargs, args, type, gcc_p)
     char *dummyname;
     CORE_ADDR start_sp;
     CORE_ADDR fun;
     int nargs;
     value_ptr *args;
     struct type *type;
     int gcc_p;
{
  int regnum;
  CORE_ADDR sp;
  char buffer[MAX_REGISTER_RAW_SIZE];
  struct frame_info *frame = get_current_frame ();
  frame->dummy = start_sp;
  start_sp |= DMEM_START;

  sp = start_sp;
  for (regnum = 0; regnum < NUM_REGS; regnum++)
    {
      sp -= REGISTER_RAW_SIZE(regnum);
      store_address (buffer, REGISTER_RAW_SIZE(regnum), read_register(regnum));
      write_memory (sp, buffer, REGISTER_RAW_SIZE(regnum));
    }
  write_register (SP_REGNUM, (LONGEST)(sp & 0xffff)); 
  /* now we need to load LR with the return address */
  write_register (LR_REGNUM, (LONGEST)(d30v_call_dummy_address() & 0xffff) >> 2);  
  return sp;
}

static void
d30v_pop_dummy_frame (fi)
     struct frame_info *fi;
{
  CORE_ADDR sp = fi->dummy;
  int regnum;

  for (regnum = 0; regnum < NUM_REGS; regnum++)
    {
      sp -= REGISTER_RAW_SIZE(regnum);
      write_register(regnum, read_memory_unsigned_integer (sp, REGISTER_RAW_SIZE(regnum)));
    }
  flush_cached_frames (); /* needed? */
}


CORE_ADDR
d30v_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     int struct_return;
     CORE_ADDR struct_addr;
{
  int i, len, index=0, regnum=2;
  char buffer[4], *contents;
  LONGEST val;
  CORE_ADDR ptrs[10];

  /* Pass 1. Put all large args on stack */
  for (i = 0; i < nargs; i++)
    {
      value_ptr arg = args[i];
      struct type *arg_type = check_typedef (VALUE_TYPE (arg));
      len = TYPE_LENGTH (arg_type);
      contents = VALUE_CONTENTS(arg);
      val = extract_signed_integer (contents, len);
      if (len > 4)
	{
	  /* put on stack and pass pointers */
	  sp -= len;
	  write_memory (sp, contents, len);
	  ptrs[index++] = sp;
	}
    }

  index = 0;

  for (i = 0; i < nargs; i++)
    {
      value_ptr arg = args[i];
      struct type *arg_type = check_typedef (VALUE_TYPE (arg));
      len = TYPE_LENGTH (arg_type);
      contents = VALUE_CONTENTS(arg);
      val = extract_signed_integer (contents, len);
      if (len > 4)
	{
	  /* use a pointer to previously saved data */
	  if (regnum < 6)
	    write_register (regnum++, ptrs[index++]);
	  else
	    {
	      /* no more registers available.  put it on the stack */
	      sp -= 2;
	      store_address (buffer, 2, ptrs[index++]);
	      write_memory (sp, buffer, 2);
	    }
	}
      else
	{
	  if (regnum < 6 )
	    {
	      if (len == 4)
		write_register (regnum++, val>>16);
	      write_register (regnum++, val & 0xffff);
	    }
	  else
	    {
	      sp -= len;
	      store_address (buffer, len, val);
	      write_memory (sp, buffer, len);
	    }
	}
    }
  return sp;
}


/* pick an out-of-the-way place to set the return value */
/* for an inferior function call.  The link register is set to this  */
/* value and a momentary breakpoint is set there.  When the breakpoint */
/* is hit, the dummy frame is popped and the previous environment is */
/* restored. */

CORE_ADDR
d30v_call_dummy_address ()
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
d30v_extract_return_value (valtype, regbuf, valbuf)
     struct type *valtype;
     char regbuf[REGISTER_BYTES];
     char *valbuf;
{
  memcpy (valbuf, regbuf + REGISTER_BYTE (2), TYPE_LENGTH (valtype));
}

/* The following code implements access to, and display of, the D30V's
   instruction trace buffer.  The buffer consists of 64K or more
   4-byte words of data, of which each words includes an 8-bit count,
   an 8-bit segment number, and a 16-bit instruction address.

   In theory, the trace buffer is continuously capturing instruction
   data that the CPU presents on its "debug bus", but in practice, the
   ROMified GDB stub only enables tracing when it continues or steps
   the program, and stops tracing when the program stops; so it
   actually works for GDB to read the buffer counter out of memory and
   then read each trace word.  The counter records where the tracing
   stops, but there is no record of where it started, so we remember
   the PC when we resumed and then search backwards in the trace
   buffer for a word that includes that address.  This is not perfect,
   because you will miss trace data if the resumption PC is the target
   of a branch.  (The value of the buffer counter is semi-random, any
   trace data from a previous program stop is gone.)  */

/* The address of the last word recorded in the trace buffer.  */

#define DBBC_ADDR (0xd80000)

/* The base of the trace buffer, at least for the "Board_0".  */

#define TRACE_BUFFER_BASE (0xf40000)

static void trace_command PARAMS ((char *, int));

static void untrace_command PARAMS ((char *, int));

static void trace_info PARAMS ((char *, int));

static void tdisassemble_command PARAMS ((char *, int));

static void display_trace PARAMS ((int, int));

/* True when instruction traces are being collected.  */

static int tracing;

/* Remembered PC.  */

static CORE_ADDR last_pc;

/* True when trace output should be displayed whenever program stops.  */

static int trace_display;

/* True when trace listing should include source lines.  */

static int default_trace_show_source = 1;

struct trace_buffer {
  int size;
  short *counts;
  CORE_ADDR *addrs;
} trace_data;

static void
trace_command (args, from_tty)
     char *args;
     int from_tty;
{
  /* Clear the host-side trace buffer, allocating space if needed.  */
  trace_data.size = 0;
  if (trace_data.counts == NULL)
    trace_data.counts = (short *) xmalloc (65536 * sizeof(short));
  if (trace_data.addrs == NULL)
    trace_data.addrs = (CORE_ADDR *) xmalloc (65536 * sizeof(CORE_ADDR));

  tracing = 1;

  printf_filtered ("Tracing is now on.\n");
}

static void
untrace_command (args, from_tty)
     char *args;
     int from_tty;
{
  tracing = 0;

  printf_filtered ("Tracing is now off.\n");
}

static void
trace_info (args, from_tty)
     char *args;
     int from_tty;
{
  int i;

  if (trace_data.size)
    {
      printf_filtered ("%d entries in trace buffer:\n", trace_data.size);

      for (i = 0; i < trace_data.size; ++i)
	{
	  printf_filtered ("%d: %d instruction%s at 0x%x\n",
			   i, trace_data.counts[i],
			   (trace_data.counts[i] == 1 ? "" : "s"),
			   trace_data.addrs[i]);
	}
    }
  else
    printf_filtered ("No entries in trace buffer.\n");

  printf_filtered ("Tracing is currently %s.\n", (tracing ? "on" : "off"));
}

/* Print the instruction at address MEMADDR in debugged memory,
   on STREAM.  Returns length of the instruction, in bytes.  */

static int
print_insn (memaddr, stream)
     CORE_ADDR memaddr;
     GDB_FILE *stream;
{
  /* If there's no disassembler, something is very wrong.  */
  if (tm_print_insn == NULL)
    abort ();

  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    tm_print_insn_info.endian = BFD_ENDIAN_BIG;
  else
    tm_print_insn_info.endian = BFD_ENDIAN_LITTLE;
  return (*tm_print_insn) (memaddr, &tm_print_insn_info);
}

void
d30v_eva_prepare_to_trace ()
{
  if (!tracing)
    return;

  last_pc = read_register (PC_REGNUM);
}

/* Collect trace data from the target board and format it into a form
   more useful for display.  */

void
d30v_eva_get_trace_data ()
{
  int count, i, j, oldsize;
  int trace_addr, trace_seg, trace_cnt, next_cnt;
  unsigned int last_trace, trace_word, next_word;
  unsigned int *tmpspace;

  if (!tracing)
    return;

  tmpspace = xmalloc (65536 * sizeof(unsigned int));

  last_trace = read_memory_unsigned_integer (DBBC_ADDR, 2) << 2;

  /* Collect buffer contents from the target, stopping when we reach
     the word recorded when execution resumed.  */

  count = 0;
  while (last_trace > 0)
    {
      QUIT;
      trace_word =
	read_memory_unsigned_integer (TRACE_BUFFER_BASE + last_trace, 4);
      trace_addr = trace_word & 0xffff;
      last_trace -= 4;
      /* Ignore an apparently nonsensical entry.  */
      if (trace_addr == 0xffd5)
	continue;
      tmpspace[count++] = trace_word;
      if (trace_addr == last_pc)
	break;
      if (count > 65535)
	break;
    }

  /* Move the data to the host-side trace buffer, adjusting counts to
     include the last instruction executed and transforming the address
     into something that GDB likes.  */

  for (i = 0; i < count; ++i)
    {
      trace_word = tmpspace[i];
      next_word = ((i == 0) ? 0 : tmpspace[i - 1]);
      trace_addr = trace_word & 0xffff;
      next_cnt = (next_word >> 24) & 0xff;
      j = trace_data.size + count - i - 1;
      trace_data.addrs[j] = (trace_addr << 2) + 0x1000000;
      trace_data.counts[j] = next_cnt + 1;
    }

  oldsize = trace_data.size;
  trace_data.size += count;

  free (tmpspace);

  if (trace_display)
    display_trace (oldsize, trace_data.size);
}

static void
tdisassemble_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  int i, count;
  CORE_ADDR low, high;
  char *space_index;

  if (!arg)
    {
      low = 0;
      high = trace_data.size;
    }
  else if (!(space_index = (char *) strchr (arg, ' ')))
    {
      low = parse_and_eval_address (arg);
      high = low + 5;
    }
  else
    {
      /* Two arguments.  */
      *space_index = '\0';
      low = parse_and_eval_address (arg);
      high = parse_and_eval_address (space_index + 1);
      if (high < low)
	high = low;
    }

  printf_filtered ("Dump of trace from %d to %d:\n", low, high);

  display_trace (low, high);

  printf_filtered ("End of trace dump.\n");
  gdb_flush (gdb_stdout);
}

static void
display_trace (low, high)
     int low, high;
{
  int i, count, trace_show_source, first, suppress;
  CORE_ADDR next_address;

  trace_show_source = default_trace_show_source;
  if (!have_full_symbols () && !have_partial_symbols())
    {
      trace_show_source = 0;
      printf_filtered ("No symbol table is loaded.  Use the \"file\" command.\n");
      printf_filtered ("Trace will not display any source.\n");
    }

  first = 1;
  suppress = 0;
  for (i = low; i < high; ++i)
    {
      next_address = trace_data.addrs[i];
      count = trace_data.counts[i]; 
      while (count-- > 0)
	{
	  QUIT;
	  if (trace_show_source)
	    {
	      struct symtab_and_line sal, sal_prev;

	      sal_prev = find_pc_line (next_address - 4, 0);
	      sal = find_pc_line (next_address, 0);

	      if (sal.symtab)
		{
		  if (first || sal.line != sal_prev.line)
		    print_source_lines (sal.symtab, sal.line, sal.line + 1, 0);
		  suppress = 0;
		}
	      else
		{
		  if (!suppress)
		    /* FIXME-32x64--assumes sal.pc fits in long.  */
		    printf_filtered ("No source file for address %s.\n",
				     local_hex_string((unsigned long) sal.pc));
		  suppress = 1;
		}
	    }
	  first = 0;
	  print_address (next_address, gdb_stdout);
	  printf_filtered (":");
	  printf_filtered ("\t");
	  wrap_here ("    ");
	  next_address = next_address + print_insn (next_address, gdb_stdout);
	  printf_filtered ("\n");
	  gdb_flush (gdb_stdout);
	}
    }
}

extern void (*target_resume_hook) PARAMS ((void));
extern void (*target_wait_loop_hook) PARAMS ((void));

void
_initialize_d30v_tdep ()
{
  tm_print_insn = print_insn_d30v;

  target_resume_hook = d30v_eva_prepare_to_trace;
  target_wait_loop_hook = d30v_eva_get_trace_data;

  add_info ("flags", print_flags_command, "Print d30v flags.");

  add_com ("trace", class_support, trace_command,
	   "Enable tracing of instruction execution.");

  add_com ("untrace", class_support, untrace_command,
	   "Disable tracing of instruction execution.");

  add_com ("tdisassemble", class_vars, tdisassemble_command,
	   "Disassemble the trace buffer.\n\
Two optional arguments specify a range of trace buffer entries\n\
as reported by info trace (NOT addresses!).");

  add_info ("trace", trace_info,
	    "Display info about the trace data buffer.");

  add_show_from_set (add_set_cmd ("tracedisplay", no_class,
				  var_integer, (char *)&trace_display,
				  "Set automatic display of trace.\n", &setlist),
		     &showlist);
  add_show_from_set (add_set_cmd ("tracesource", no_class,
				  var_integer, (char *)&default_trace_show_source,
				  "Set display of source code with trace.\n", &setlist),
		     &showlist);

} 
