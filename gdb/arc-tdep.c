/* ARC target-dependent stuff.
   Copyright (C) 1995 Free Software Foundation, Inc.

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

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "target.h"
#include "floatformat.h"
#include "symtab.h"

static void codestream_read PARAMS ((unsigned int *, int));
static void codestream_seek PARAMS ((int));
static unsigned int codestream_fill PARAMS ((int));

#define CODESTREAM_BUFSIZ 16 
static CORE_ADDR codestream_next_addr;
static CORE_ADDR codestream_addr;
static unsigned int codestream_buf[CODESTREAM_BUFSIZ];
static int codestream_off;
static int codestream_cnt;

#define codestream_tell() (codestream_addr + codestream_off)
#define codestream_peek() (codestream_cnt == 0 ? \
			   codestream_fill(1): codestream_buf[codestream_off])
#define codestream_get() (codestream_cnt-- == 0 ? \
			 codestream_fill(0) : codestream_buf[codestream_off++])
#define OPMASK	0xf8000000

static unsigned int 
codestream_fill (peek_flag)
    int peek_flag;
{
  codestream_addr = codestream_next_addr;
  codestream_next_addr += CODESTREAM_BUFSIZ;
  codestream_off = 0;
  codestream_cnt = CODESTREAM_BUFSIZ;
  read_memory (codestream_addr, (char *) codestream_buf, CODESTREAM_BUFSIZ);
  
  if (peek_flag)
    return (codestream_peek());
  else
    return (codestream_get());
}

static void
codestream_seek (place)
    int place;
{
  codestream_next_addr = place / CODESTREAM_BUFSIZ;
  codestream_next_addr *= CODESTREAM_BUFSIZ;
  codestream_cnt = 0;
  codestream_fill (1);
  while (codestream_tell() != place)
    codestream_get ();
}

static void
codestream_read (buf, count)
     unsigned int *buf;
     int count;
{
  unsigned int *p;
  int i;
  p = buf;
  for (i = 0; i < count; i++)
    *p++ = codestream_get ();
}

/*
 * find & return amound a local space allocated, and advance codestream to
 * first register push (if any)
 * if entry sequence doesn't make sense, return -1, and leave 
 * codestream pointer random
 */

static long
arc_get_frame_setup (pc)
     int pc;
{
  unsigned int insn, n;

  codestream_seek (pc);
  insn = codestream_get ();

  if (insn & OPMASK == 0x10000000)		/* st fp,[sp] */
    {	
      insn = codestream_get ();
      if (insn & OPMASK != 0x10000000)	        /* st blink,[sp,4] */
	{
	  if (insn & OPMASK != 0x60000000)      /* for leaf, no st blink */
	    return -1;
	}
      else if (codestream_get () & OPMASK != 0x60000000)    /* mov fp,sp */
	return (-1);

      /* check for stack adjustment sub sp,nnn,sp */
      insn = codestream_peek ();
      if (insn & OPMASK == 0x50000000)
	{
	  n = (insn & 0x000001ff ); 
          codestream_get ();

	  /* this sequence is used to get the address of the return
	   * buffer for a function that returns a structure
	   */
	  insn = codestream_peek ();
	  if (insn & OPMASK == 0x60000000)
	    codestream_get ();

	  return n;
	}
      else
	{
	  return (0);
	}
    }
  return (-1);
}

/* return pc of first real instruction */
CORE_ADDR
skip_prologue (pc)
     int pc;
{
  unsigned int insn;
  int i;
  CORE_ADDR pos;
  
  if (arc_get_frame_setup (pc) < 0)
    return (pc);
  
  /* skip over register saves */
  for (i = 0; i < 10; i++)
    {
      insn = codestream_peek ();
      if (insn & OPMASK != 0x10000000)       /* break if not st inst */
	break;
      codestream_get ();
    }
     
  codestream_seek (pos);
  return (codestream_tell ());
}

/* Return number of args passed to a frame.
   Can return -1, meaning no way to tell.  */
int
frame_num_args (fi)
     struct frame_info *fi;
{
#if 1
  return -1;
#else
  /* This loses because not only might the compiler not be popping the
     args right after the function call, it might be popping args from both
     this call and a previous one, and we would say there are more args
     than there really are.  Is it true for ARC */

  int retpc;						
  unsigned char op;					
  struct frame_info *pfi;

  int frameless;

  FRAMELESS_FUNCTION_INVOCATION (fi, frameless);
  if (frameless)
    /* In the absence of a frame pointer, GDB doesn't get correct values
       for nameless arguments.  Return -1, so it doesn't print any
       nameless arguments.  */
    return -1;

  pfi = get_prev_frame_info (fi);			
  if (pfi == 0)
    {
      /* Note:  this can happen if we are looking at the frame for
	 main, because FRAME_CHAIN_VALID won't let us go into
	 start.  If we have debugging symbols, that's not really
	 a big deal; it just means it will only show as many arguments
	 to main as are declared.  */
      return -1;
    }
  else
    {
      retpc = pfi->pc;					
      op = read_memory_integer (retpc, 1);			
      if (op == 0x59)					
	/* pop %ecx */			       
	return 1;				
	}
      else
	{
	  return 0;
	}
    }
#endif
}

/*
 * parse the first few instructions of the function to see
 * what registers were stored.
 *
 * The startup sequence can be at the start of the function.
 * 'st fp,[sp], st blink,[sp+4], mov fp,sp' 
 *
 * Local space is allocated just below by sub sp,nnn,sp
 * Next, the registers used by this function are stored.
 */

void
frame_find_saved_regs (fip, fsrp)
     struct frame_info *fip;
     struct frame_saved_regs *fsrp;
{
  long locals;
  unsigned int insn;
  CORE_ADDR dummy_bottom;
  CORE_ADDR adr;
  int i, regnum, offset;
  
  memset (fsrp, 0, sizeof *fsrp);
  
  /* if frame is the end of a dummy, compute where the
   * beginning would be
   */
  dummy_bottom = fip->frame - 4 - REGISTER_BYTES - CALL_DUMMY_LENGTH;
  
  /* check if the PC is in the stack, in a dummy frame */
  if (dummy_bottom <= fip->pc && fip->pc <= fip->frame) 
    {
      /* all regs were saved by push_call_dummy () */
      adr = fip->frame;
      for (i = 0; i < NUM_REGS; i++) 
	{
	  adr -= REGISTER_RAW_SIZE (i);
	  fsrp->regs[i] = adr;
	}
      return;
    }
  
  locals = arc_get_frame_setup (get_pc_function_start (fip->pc));
  
  if (locals >= 0) 
    {
      adr = fip->frame - locals;
      for (i = 0; i < 10; i++) 
	{
	  insn = codestream_get ();
	  if (insn & 0xffff8000 != 0x100d8000)
	    break;
          regnum = (insn & 0x00007c00) >> 9;
	  offset = (insn << 23) >> 23;
	  fsrp->regs[regnum] = adr + offset;
	}
    }
  
  fsrp->regs[PC_REGNUM] = fip->frame + 4;
  fsrp->regs[FP_REGNUM] = fip->frame;
}

void
push_dummy_frame ()
{
  CORE_ADDR sp = read_register (SP_REGNUM);
  int regnum;
  char regbuf[MAX_REGISTER_RAW_SIZE];

  read_register_gen (PC_REGNUM, regbuf);
  write_memory (sp+4, regbuf, REGISTER_SIZE);
  read_register_gen (FP_REGNUM, regbuf);
  write_memory (sp, regbuf, REGISTER_SIZE);
  write_register (FP_REGNUM, sp);
  for (regnum = 0; regnum < NUM_REGS; regnum++)
    {
      read_register_gen (regnum, regbuf);
      sp = push_bytes (sp, regbuf, REGISTER_RAW_SIZE (regnum));
    }
  sp += (2*REGISTER_SIZE);
  write_register (SP_REGNUM, sp);
}

void
pop_frame ()
{
  struct frame_info *frame = get_current_frame ();
  CORE_ADDR fp;
  int regnum;
  struct frame_saved_regs fsr;
  char regbuf[MAX_REGISTER_RAW_SIZE];
  
  fp = FRAME_FP (frame);
  get_frame_saved_regs (frame, &fsr);
  for (regnum = 0; regnum < NUM_REGS; regnum++) 
    {
      CORE_ADDR adr;
      adr = fsr.regs[regnum];
      if (adr)
	{
	  read_memory (adr, regbuf, REGISTER_RAW_SIZE (regnum));
	  write_register_bytes (REGISTER_BYTE (regnum), regbuf,
				REGISTER_RAW_SIZE (regnum));
	}
    }
  write_register (FP_REGNUM, read_memory_integer (fp, 4));
  write_register (PC_REGNUM, read_memory_integer (fp + 4, 4));
  write_register (SP_REGNUM, fp + 8);
  flush_cached_frames ();
}

#ifdef GET_LONGJMP_TARGET
/* Figure out where the longjmp will land.  Slurp the args out of the stack.
   We expect the first arg to be a pointer to the jmp_buf structure from which
   we extract the pc (JB_PC) that we will land at.  The pc is copied into PC.
   This routine returns true on success. */

int
get_longjmp_target(pc)
     CORE_ADDR *pc;
{
  char buf[TARGET_PTR_BIT / TARGET_CHAR_BIT];
  CORE_ADDR sp, jb_addr;

  sp = read_register (SP_REGNUM);

  if (target_read_memory (sp + SP_ARG0, /* Offset of first arg on stack */
			  buf,
			  TARGET_PTR_BIT / TARGET_CHAR_BIT))
    return 0;

  jb_addr = extract_address (buf, TARGET_PTR_BIT / TARGET_CHAR_BIT);

  if (target_read_memory (jb_addr + JB_PC * JB_ELEMENT_SIZE, buf,
			  TARGET_PTR_BIT / TARGET_CHAR_BIT))
    return 0;

  *pc = extract_address (buf, TARGET_PTR_BIT / TARGET_CHAR_BIT);

  return 1;
}
#endif /* GET_LONGJMP_TARGET */

void _initialize_arc_tdep ()
{
  tm_print_insn = print_insn_arc;
}
