/* Target-dependent code for GDB, the GNU debugger.
   Copyright (C) 1986, 1987, 1989, 1991 Free Software Foundation, Inc.

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

#include <stdio.h>

#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "target.h"

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <sys/ptrace.h>
#include <sys/reg.h>

#include <a.out.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/core.h>

extern int errno;
extern int attach_flag;

/* Nonzero if we just simulated a single step break. */
int one_stepped;

#if 0

/* This is Damon's implementation of single step simulation. It suffers the
   following program:

   1 main () {
   2   char buf[10];
   3   puts ("test");
   4   strcmp (buf, "test");  puts ("test");
   5   exit (0);
   6 }

   You cannot `next' on line 4 in the above program. gdb puts a breakpoint
   to the return address of `strcmp', and when execution arrives that point,
   it is still in the line range and gdb attemps to resume it with single 
   steps. At that point the breakpoint at step_resume_break_address (return 
   address of strcmp) and single step's breakpoint mixes up and we end up
   with a breakpoint which its shadow and itself are identical.

   Fix that problem and use this version. FIXMEmgo.
*/


static struct sstep_breaks {
	int address;
	int data;
} tbreak[2];


/*
 * branch_dest -	calculate all places the current instruction may go
 */
static
branch_dest(tb)
     register struct sstep_breaks *tb; 
{
    register ulong opcode, iar;
    long instr;
    int immediate, absolute;;

    iar = read_pc();					/* current IAR	*/
    target_read_memory(iar, &instr, sizeof (instr));	/* current inst	*/

    opcode   = instr >> 26;
    absolute = instr & 2;

    tb[1].address = -1;

    switch (opcode) {
      case 0x10:			/* branch conditional	*/
	immediate = ((instr & ~3) << 16) >> 16;

	/*
	 * two possible locations for next instruction
	 */
	tb[0].address = iar + 4;
	tb[1].address = immediate + (absolute ? 0 : iar);

	break;

      case 0x12:			/* branch unconditional	*/
	immediate = ((instr & ~3) << 6) >> 6;

	/*
	 * only one possible location for next instr
	 */
	tb[0].address = immediate + (absolute ? 0 : iar);

	break;

      case 0x13:			/* branch conditional register	*/
	/*
	 * WE NEED TO CHECK THE CR HERE, TO SEE IF THIS IS
	 * REALLY UNCONDITIONAL.
	 */
	tb++->address = iar + 4;

	switch ((instr >> 1) & 0x3ff) {
	  case 0x10:			/* branch conditional register	*/
	    tb->address = read_register(LR_REGNUM) & ~3;
	    sigtramp_chk(tb);		/* return from sig handler?	*/
	    break;

	  case 0x210:			/* branch cond to CTR		*/
	    tb->address = read_register(CTR_REGNUM) & ~3;
	    sigtramp_chk(tb);		/* return from sig handler?	*/
	    break;

	  default:
	    /*
	     * not a branch.
	     */
	    tb->address = iar + 4;
	    break;
	}
	break;
	
      default:
	/*
	 * not a branch, flow proceeds normally
	 */
	tb->address = iar + 4;
	break;
    }
}

/*
 * sigtramp_chk -	heuristic check to see if we think we are returning
 *			from a signal handler.
 *
 * Input:
 *	tb	-	^ to a single step branch location
 *
 * Note:
 *	When we are at the "br" instruction returning to a signal handler,
 *	we return in user mode to an address in the kernel.  If the
 *	segment of the branch target is 0, we may very well be in a
 *	signal handler.  From scrounging through this code, we note that
 *	register 29 has the signal context pointer, from which we can
 *	determine where we will end up next.
 */
sigtramp_chk(tb)
register struct sstep_breaks *tb; {
    struct sigcontext sc;

    if (tb->address & 0xf0000000)
	return;		/* can't have been sigtramp	*/

    if (target_read_memory(read_register(GPR29), &sc, sizeof (sc)))
	return;		/* read fails, heuristic fails	*/

    if ((sc.sc_jmpbuf.jmp_context.iar & 0xf0000000) == 0x10000000) {
	/*
	 * looks like it might be ok.....
	 */
	tb->address = sc.sc_jmpbuf.jmp_context.iar;
    }
}


/*
 * single_step -	no trace mode harware support, or software support.
 *			sigh.
 */
single_step(signal) {
    register i;

    if (!one_stepped) {
	/*
	 * need to set breakpoints for single step.
	 * figure out all places the current instruction could go.
	 */
	branch_dest(&tbreak[0]);

	/*
	 * always at least one place to go to
	 */
	target_insert_breakpoint(tbreak[0].address, &tbreak[0].data);

	/*
	 * if there is another possible location, set a breakpoint there
	 * as well.
	 */
	if (tbreak[1].address != -1)
	    target_insert_breakpoint(tbreak[1].address, &tbreak[1].data);

	one_stepped = 1;
	ptrace(PT_CONTINUE, inferior_pid, 1, signal, 0);
    } else {
	/*
	 * need to clear the breakpoints.
	 */
	for (i = 0; i < 2; ++i)
	    if (tbreak[i].address != -1)
		target_remove_breakpoint(tbreak[i].address, &tbreak[i].data);

	one_stepped = 0;
    }

    return 1;
}

#else	/* !DAMON'S VERSION */

/* Breakpoint shadows for the single step instructions will be kept here. */

static struct sstep_breaks {
	int address;
	int data;
} stepBreaks[2];


/*
 * Calculate the destination of a branch/jump.  Return -1 if not a branch.
 */
static int
branch_dest (opcode, instr, pc, safety)
 int opcode, instr, pc, safety;
{
  register long offset;
  unsigned dest;
  int immediate;
  int absolute;
  int ext_op;

  absolute = (int) ((instr >> 1) & 1);

  switch (opcode) {
     case 18	:
	immediate = ((instr & ~3) << 6) >> 6;	/* br unconditionl */

     case 16	:  
	if (opcode != 18)		        /* br conditional */
	  immediate = ((instr & ~3) << 16) >> 16;
	if (absolute)
	  dest = immediate;	
	else
	  dest = pc + immediate;
	break;

      case 19	:
	ext_op = (instr>>1) & 0x3ff;

	if (ext_op == 16)			/* br conditional register */
	  dest = read_register (LR_REGNUM) & ~3;

	else if (ext_op == 528)			/* br cond to count reg */
	  dest = read_register (CTR_REGNUM) & ~3;

	else return -1; 
	break;
	
       default: return -1;
  }
  return (dest < 0x10000000) ? safety : dest;
}



/* AIX does not support PT_STEP. Simulate it. */

int
single_step (signal)
int signal;
{
#define	INSNLEN(OPCODE)	 4

  static char breakp[] = BREAKPOINT;
  int ii, insn, ret, loc;
  int breaks[2], opcode;

  if (!one_stepped) {
    extern CORE_ADDR text_start;
    loc = read_pc ();

    ret = read_memory (loc, &insn, sizeof (int));
    if (ret)
      printf ("Error in single_step()!!\n");

    breaks[0] = loc + INSNLEN(insn);
    opcode = insn >> 26;
    breaks[1] = branch_dest (opcode, insn, loc, breaks[0]);

    stepBreaks[1].address = -1;

    for (ii=0; ii < 2; ++ii) {

      /* ignore invalid breakpoint. */
      if ( breaks[ii] == -1)
        continue;

      read_memory (breaks[ii], &(stepBreaks[ii].data), sizeof(int));

      ret = write_memory (breaks[ii], breakp, sizeof(int));
      stepBreaks[ii].address = breaks[ii];
    }  

    one_stepped = 1;
    ptrace (PT_CONTINUE, inferior_pid, 1, signal);
  }
  else {

    /* remove step breakpoints. */
    for (ii=0; ii < 2; ++ii)
      if (stepBreaks[ii].address != -1)
        write_memory 
           (stepBreaks[ii].address, &(stepBreaks[ii].data), sizeof(int));

    one_stepped = 0;
  }
  return 1;
}
#endif /* !DAMON's version of single step. */



/* return pc value after skipping a function prologue. */

skip_prologue (pc)
int pc;
{
  unsigned int tmp;
  unsigned int op;

  if (target_read_memory (pc, (char *)&op, sizeof (op)))
    return pc;			/* Can't access it -- assume no prologue. */
  SWAP_TARGET_AND_HOST (&op, sizeof (op));

  /* Assume that subsequent fetches can fail with low probability.  */

  if (op == 0x7c0802a6) {		/* mflr r0 */
    pc += 4;
    op = read_memory_integer (pc, 4);
  }
  else				/* else, this is a frameless invocation */
    return pc;

  if ((op & 0xfc00003e) == 0x7c000026) { /* mfcr Rx */
    pc += 4;
    op = read_memory_integer (pc, 4);
  }

  if ((op & 0xfc000000) == 0x48000000) { /* bl foo, to save fprs??? */
    pc += 4;
    op = read_memory_integer (pc, 4);
  }

  if ((op & 0xfc1f0000) == 0xd8010000) { /* stfd Rx,NUM(r1) */
    pc += 4;				 /* store floating register double */
    op = read_memory_integer (pc, 4);
  }

  if ((op & 0xfc1f0000) == 0xbc010000) { /* stm Rx, NUM(r1) */
    pc += 4;
    op = read_memory_integer (pc, 4);
  }

  while (((tmp = op >> 16) == 0x9001) || /* st   r0, NUM(r1) */
	 (tmp == 0x9421) ||		/* stu  r1, NUM(r1) */
	 (op == 0x93e1fffc)) 		/* st   r31,-4(r1) */
  {
    pc += 4;
    op = read_memory_integer (pc, 4);
  }

  while ((tmp = (op >> 22)) == 0x20f) {	/* l	r31, ... or */
    pc += 4;				/* l	r30, ...    */
    op = read_memory_integer (pc, 4);
  }

  while ((op & 0xfc1f0000) == 0x90010000) {	/* st r?, NUM(r1) */  
    pc += 4;
    op = read_memory_integer (pc, 4);
  }

  if (op == 0x603f0000) {			/* oril r31, r1, 0x0 */
    pc += 4;					/* this happens if r31 is used as */
    op = read_memory_integer (pc, 4);		/* frame ptr. (gcc does that)	  */

    if ((op >> 16) == 0x907f) {			/* st r3, NUM(r31) */
      pc += 4;
      op = read_memory_integer (pc, 4);
    }
  }
  return pc;
}

/* text start and end addresses in virtual memory. */

CORE_ADDR text_start;
CORE_ADDR text_end;


/*************************************************************************
  Support for creating pushind a dummy frame into the stack, and popping
  frames, etc. 
*************************************************************************/

#define	DUMMY_FRAME_ADDR_SIZE 10

/* Make sure you initialize these in somewhere, in case gdb gives up what it
   was debugging and starts debugging something else. FIXMEmgo */

static int dummy_frame_count = 0;
static int dummy_frame_size = 0;
static CORE_ADDR *dummy_frame_addr = 0;

extern int stop_stack_dummy;

/* push a dummy frame into stack, save all register. Currently we are saving
   only gpr's and fpr's, which is not good enough! FIXMEmgo */
   
push_dummy_frame ()
{
  int sp, pc;				/* stack pointer and link register */
  int ii;

  if (dummy_frame_count >= dummy_frame_size) {
    dummy_frame_size += DUMMY_FRAME_ADDR_SIZE;
    if (dummy_frame_addr)
      dummy_frame_addr = (CORE_ADDR*) xrealloc 
        (dummy_frame_addr, sizeof(CORE_ADDR) * (dummy_frame_size));
    else
      dummy_frame_addr = (CORE_ADDR*) 
	xmalloc (sizeof(CORE_ADDR) * (dummy_frame_size));
  }
  
  sp = read_register(SP_REGNUM);
  pc = read_register(PC_REGNUM);  

  dummy_frame_addr [dummy_frame_count++] = sp;

  /* Be careful! If the stack pointer is not decremented first, then kernel 
     thinks he is free to use the sapce underneath it. And kernel actually 
     uses that area for IPC purposes when executing ptrace(2) calls. So 
     before writing register values into the new frame, decrement and update
     %sp first in order to secure your frame. */

  write_register (SP_REGNUM, sp-408);

#if 1
  /* gdb relies on the state of current_frame. We'd better update it,
     otherwise things like do_registers_info() wouldn't work properly! */

  flush_cached_frames ();
  set_current_frame (create_new_frame (sp-408, pc));
#endif /* 0 */

  /* save program counter in link register's space. */
  write_memory (sp+8, &pc, 4);

  /* save full floating point registers here. They will be from F14..F31
     for know. I am not sure if we need to save everything here! */

  /* fpr's, f0..f31 */
  for (ii = 0; ii < 32; ++ii)
    write_memory (sp-8-(ii*8), &registers[REGISTER_BYTE (31-ii+FP0_REGNUM)], 8);

  /* gpr's r0..r31 */
  for (ii=1; ii <=32; ++ii)
    write_memory (sp-256-(ii*4), &registers[REGISTER_BYTE (32-ii)], 4);

  /* so far, 32*2 + 32 words = 384 bytes have been written. We need 6 words
     (24 bytes) for the rest of the registers. It brings the total to 408 
     bytes.
     save sp or so call back chain right here. */
  write_memory (sp-408, &sp, 4);
  sp -= 408;

  /* And finally, this is the back chain. */
  write_memory (sp+8, &pc, 4);
}


/* Pop a dummy frame.

   In rs6000 when we push a dummy frame, we save all of the registers. This
   is usually done before user calls a function explicitly.

   After a dummy frame is pushed, some instructions are copied into stack, and
   stack pointer is decremented even more. Since we don't have a frame pointer to
   get back to the parent frame of the dummy, we start having trouble poping it.
   Therefore, we keep a dummy frame stack, keeping addresses of dummy frames as
   such. When poping happens and when we detect that was a dummy frame, we pop
   it back to its parent by using dummy frame stack (`dummy_frame_addr' array).
 */
   
pop_dummy_frame ()
{
  CORE_ADDR sp, pc;
  int ii;
  sp = dummy_frame_addr [--dummy_frame_count];

  /* restore all fpr's. */
  for (ii = 1; ii <= 32; ++ii)
    read_memory (sp-(ii*8), &registers[REGISTER_BYTE (32-ii+FP0_REGNUM)], 8);

  /* restore all gpr's */
  for (ii=1; ii <= 32; ++ii) {
    read_memory (sp-256-(ii*4), &registers[REGISTER_BYTE (32-ii)], 4);
  }

  read_memory (sp-400, &registers [REGISTER_BYTE(PC_REGNUM)], 4);

  /* when a dummy frame was being pushed, we had to decrement %sp first, in 
     order to secure astack space. Thus, saved %sp (or %r1) value, is not the
     one we should restore. Change it with the one we need. */

  *(int*)&registers [REGISTER_BYTE(FP_REGNUM)] = sp;

  /* Now we can restore all registers. */

  store_inferior_registers (-1);
  pc = read_pc ();
  flush_cached_frames ();
  set_current_frame (create_new_frame (sp, pc));
}


/* pop the innermost frame, go back to the caller. */

pop_frame ()
{
  int pc, lr, sp, prev_sp;		/* %pc, %lr, %sp */
  FRAME fr = get_current_frame ();
  int offset = 0;
  int frameless = 0;			/* TRUE if function is frameless */
  int addr, ii;
  int saved_gpr, saved_fpr;		/* # of saved gpr's and fpr's */

  pc = read_pc ();
  sp = FRAME_FP (fr);

  if (stop_stack_dummy && dummy_frame_count) {
    pop_dummy_frame ();
    return;
  }

  /* figure out previous %pc value. If the function is frameless, it is 
     still in the link register, otherwise walk the frames and retrieve the
     saved %pc value in the previous frame. */

  addr = get_pc_function_start (fr->pc) + FUNCTION_START_OFFSET;
  function_frame_info (addr, &frameless, &offset, &saved_gpr, &saved_fpr);

  read_memory (sp, &prev_sp, 4);
  if (frameless)
    lr = read_register (LR_REGNUM);
  else
    read_memory (prev_sp+8, &lr, 4);

  /* reset %pc value. */
  write_register (PC_REGNUM, lr);

  /* reset register values if any was saved earlier. */
  addr = prev_sp - offset;

  if (saved_gpr != -1)
    for (ii=saved_gpr; ii <= 31; ++ii) {
      read_memory (addr, &registers [REGISTER_BYTE (ii)], 4);
      addr += sizeof (int);
    }

  if (saved_fpr != -1)
    for (ii=saved_fpr; ii <= 31; ++ii) {
      read_memory (addr, &registers [REGISTER_BYTE (ii+FP0_REGNUM)], 8);
      addr += 8;
  }

  write_register (SP_REGNUM, prev_sp);
  store_inferior_registers (-1);
  flush_cached_frames ();
  set_current_frame (create_new_frame (prev_sp, lr));
}


/* fixup the call sequence of a dummy function, with the real function address.
   its argumets will be passed by gdb. */

fix_call_dummy(dummyname, pc, fun, nargs, type)
  char *dummyname;
  int pc;
  int fun;
  int nargs;					/* not used */
  int type;					/* not used */

{
#define	TOC_ADDR_OFFSET		20
#define	TARGET_ADDR_OFFSET	28

  int ii;
  unsigned long target_addr;
  unsigned long tocvalue;

  target_addr = fun;
  tocvalue = find_toc_address (target_addr);

  ii  = *(int*)((char*)dummyname + TOC_ADDR_OFFSET);
  ii = (ii & 0xffff0000) | (tocvalue >> 16);
  *(int*)((char*)dummyname + TOC_ADDR_OFFSET) = ii;

  ii  = *(int*)((char*)dummyname + TOC_ADDR_OFFSET+4);
  ii = (ii & 0xffff0000) | (tocvalue & 0x0000ffff);
  *(int*)((char*)dummyname + TOC_ADDR_OFFSET+4) = ii;

  ii  = *(int*)((char*)dummyname + TARGET_ADDR_OFFSET);
  ii = (ii & 0xffff0000) | (target_addr >> 16);
  *(int*)((char*)dummyname + TARGET_ADDR_OFFSET) = ii;

  ii  = *(int*)((char*)dummyname + TARGET_ADDR_OFFSET+4);
  ii = (ii & 0xffff0000) | (target_addr & 0x0000ffff);
  *(int*)((char*)dummyname + TARGET_ADDR_OFFSET+4) = ii;
}



/* return information about a function frame.
    - frameless is TRUE, if function does not save %pc value in its frame.
    - offset is the number of bytes used in the frame to save registers.
    - saved_gpr is the number of the first saved gpr.
    - saved_fpr is the number of the first saved fpr.
 */
function_frame_info (pc, frameless, offset, saved_gpr, saved_fpr)
  int pc;
  int *frameless, *offset, *saved_gpr, *saved_fpr;
{
  unsigned int tmp;
  register unsigned int op;

  *offset = 0;
  *saved_gpr = *saved_fpr = -1;

  if (!inferior_pid)
    return;

  op  = read_memory_integer (pc, 4);
  if (op == 0x7c0802a6) {		/* mflr r0 */
    pc += 4;
    op = read_memory_integer (pc, 4);
    *frameless = 0;
  }
  else				/* else, this is a frameless invocation */
    *frameless = 1;


  if ((op & 0xfc00003e) == 0x7c000026) { /* mfcr Rx */
    pc += 4;
    op = read_memory_integer (pc, 4);
  }

  if ((op & 0xfc000000) == 0x48000000) { /* bl foo, to save fprs??? */
    pc += 4;
    op = read_memory_integer (pc, 4);
  }

  if ((op & 0xfc1f0000) == 0xd8010000) { /* stfd Rx,NUM(r1) */
    pc += 4;				 /* store floating register double */
    op = read_memory_integer (pc, 4);
  }

  if ((op & 0xfc1f0000) == 0xbc010000) { /* stm Rx, NUM(r1) */
    int tmp2;
    *saved_gpr = (op >> 21) & 0x1f;
    tmp2 = op & 0xffff;
    if (tmp2 > 0x7fff)
      tmp2 = 0xffff0000 | tmp2;

    if (tmp2 < 0) {
      tmp2 = tmp2 * -1;
      *saved_fpr = (tmp2 - ((32 - *saved_gpr) * 4)) / 8;
      if ( *saved_fpr > 0)
        *saved_fpr = 32 - *saved_fpr;
      else
        *saved_fpr = -1;
    }
    *offset = tmp2;
  }
}


/* Pass the arguments in either registers, or in the stack. In RS6000, the first
   eight words of the argument list (that might be less than eight parameters if
   some parameters occupy more than one word) are passed in r3..r11 registers.
   float and double parameters are passed in fpr's, in addition to that. Rest of
   the parameters if any are passed in user stack. There might be cases in which
   half of the parameter is copied into registers, the other half is pushed into
   stack.

   If the function is returning a structure, then the return address is passed
   in r3, then the first 7 words of the parametes can be passed in registers,
   starting from r4. */

CORE_ADDR
push_arguments (nargs, args, sp, struct_return, struct_addr)
  int nargs;
  value *args;
  CORE_ADDR sp;
  int struct_return;
  CORE_ADDR struct_addr;
{
  int ii, len;
  int argno;					/* current argument number */
  int argbytes;					/* current argument byte */
  char tmp_buffer [50];
  value arg;
  int f_argno = 0;				/* current floating point argno */

  CORE_ADDR saved_sp, pc;

  if ( dummy_frame_count <= 0)
    printf ("FATAL ERROR -push_arguments()! frame not found!!\n");

  /* The first eight words of ther arguments are passed in registers. Copy
     them appropriately.

     If the function is returning a `struct', then the first word (which 
     will be passed in r3) is used for struct return address. In that
     case we should advance one word and start from r4 register to copy 
     parameters. */

  ii =  struct_return ? 1 : 0;

  for (argno=0, argbytes=0; argno < nargs && ii<8; ++ii) {

    arg = value_arg_coerce (args[argno]);
    len = TYPE_LENGTH (VALUE_TYPE (arg));

    if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_FLT) {

      /* floating point arguments are passed in fpr's, as well as gpr's.
         There are 13 fpr's reserved for passing parameters. At this point
         there is no way we would run out of them. */

      if (len > 8)
        printf (
"Fatal Error: a floating point parameter #%d with a size > 8 is found!\n", argno);

      bcopy (VALUE_CONTENTS (arg), 
         &registers[REGISTER_BYTE(FP0_REGNUM + 1 + f_argno)], len);
      ++f_argno;
    }

    if (len > 4) {

      /* Argument takes more than one register. */
      while (argbytes < len) {

	*(int*)&registers[REGISTER_BYTE(ii+3)] = 0;
	bcopy ( ((char*)VALUE_CONTENTS (arg))+argbytes, 
			&registers[REGISTER_BYTE(ii+3)], 
			(len - argbytes) > 4 ? 4 : len - argbytes);
	++ii, argbytes += 4;

	if (ii >= 8)
	  goto ran_out_of_registers_for_arguments;
      }
      argbytes = 0;
      --ii;
    }
    else {        /* Argument can fit in one register. No problem. */
      *(int*)&registers[REGISTER_BYTE(ii+3)] = 0;
      bcopy (VALUE_CONTENTS (arg), &registers[REGISTER_BYTE(ii+3)], len);
    }
    ++argno;
  }

ran_out_of_registers_for_arguments:

  /* location for 8 parameters are always reserved. */
  sp -= 4 * 8;

  /* another six words for back chain, TOC register, link register, etc. */
  sp -= 24;

  /* if there are more arguments, allocate space for them in 
     the stack, then push them starting from the ninth one. */

  if ((argno < nargs) || argbytes) {
    int space = 0, jj;
    value val;

    if (argbytes) {
      space += ((len - argbytes + 3) & -4);
      jj = argno + 1;
    }
    else
      jj = argno;

    for (; jj < nargs; ++jj) {
      val = value_arg_coerce (args[jj]);
      space += ((TYPE_LENGTH (VALUE_TYPE (val))) + 3) & -4;
    }

    /* add location required for the rest of the parameters */
    space = (space + 7) & -8;
    sp -= space;

    /* This is another instance we need to be concerned about securing our
	stack space. If we write anything underneath %sp (r1), we might conflict
	with the kernel who thinks he is free to use this area. So, update %sp
	first before doing anything else. */

    write_register (SP_REGNUM, sp);

#if 0
    pc = read_pc ();
    flush_cached_frames ();
    set_current_frame (create_new_frame (sp, pc));
#endif

    /* if the last argument copied into the registers didn't fit there 
       completely, push the rest of it into stack. */

    if (argbytes) {
      write_memory (
        sp+24+(ii*4), ((char*)VALUE_CONTENTS (arg))+argbytes, len - argbytes);
      ++argno;
      ii += ((len - argbytes + 3) & -4) / 4;
    }

    /* push the rest of the arguments into stack. */
    for (; argno < nargs; ++argno) {

      arg = value_arg_coerce (args[argno]);
      len = TYPE_LENGTH (VALUE_TYPE (arg));


      /* float types should be passed in fpr's, as well as in the stack. */
      if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_FLT && f_argno < 13) {

        if (len > 8)
          printf (
"Fatal Error: a floating point parameter #%d with a size > 8 is found!\n", argno);

        bcopy (VALUE_CONTENTS (arg), 
           &registers[REGISTER_BYTE(FP0_REGNUM + 1 + f_argno)], len);
        ++f_argno;
      }

      write_memory (sp+24+(ii*4), VALUE_CONTENTS (arg), len);
      ii += ((len + 3) & -4) / 4;
    }
  }
  else {

    /* Secure stack areas first, before doing anything else. */
    write_register (SP_REGNUM, sp);

#if 0
    pc = read_pc ();
    flush_cached_frames ();
    set_current_frame (create_new_frame (sp, pc));
#endif
  }

  saved_sp = dummy_frame_addr [dummy_frame_count - 1];
  read_memory (saved_sp, tmp_buffer, 24);
  write_memory (sp, tmp_buffer, 24);

    write_memory (sp, &saved_sp, 4);	/* set back chain properly */

  store_inferior_registers (-1);
  return sp;
}

/* a given return value in `regbuf' with a type `valtype', extract and copy its
   value into `valbuf' */

extract_return_value (valtype, regbuf, valbuf)
  struct type *valtype;
  char regbuf[REGISTER_BYTES];
  char *valbuf;
{

  if (TYPE_CODE (valtype) == TYPE_CODE_FLT) {

    double dd; float ff;
    /* floats and doubles are returned in fpr1. fpr's have a size of 8 bytes.
       We need to truncate the return value into float size (4 byte) if
       necessary. */

    if (TYPE_LENGTH (valtype) > 4) 		/* this is a double */
      bcopy (&regbuf[REGISTER_BYTE (FP0_REGNUM + 1)], valbuf, 
						TYPE_LENGTH (valtype));
    else {		/* float */
      bcopy (&regbuf[REGISTER_BYTE (FP0_REGNUM + 1)], &dd, 8);
      ff = (float)dd;
      bcopy (&ff, valbuf, sizeof(float));
    }
  }
  else
    /* return value is copied starting from r3. */
    bcopy (&regbuf[REGISTER_BYTE (3)], valbuf, TYPE_LENGTH (valtype));
}


/* keep keep structure return address in this variable. */

CORE_ADDR rs6000_struct_return_address;


/* Throw away this debugging code. FIXMEmgo. */
print_frame(fram)
int fram;
{
  int ii, val;
  for (ii=0; ii<40; ++ii) {
    if ((ii % 4) == 0)
      printf ("\n");
    val = read_memory_integer (fram + ii * 4, 4);
    printf ("0x%08x\t", val);
  }
  printf ("\n");
}



/* Indirect function calls use a piece of trampoline code do co context switching,
   i.e. to set the new TOC table. Skip such code if exists. */

skip_trampoline_code (pc)
int pc;
{
  register unsigned int ii, op;

  static unsigned trampoline_code[] = {
	0x800b0000,			/*     l   r0,0x0(r11)	*/
	0x90410014,			/*    st   r2,0x14(r1)	*/
	0x7c0903a6,			/* mtctr   r0		*/
	0x804b0004,			/*     l   r2,0x4(r11)	*/
	0x816b0008,			/*     l  r11,0x8(r11)	*/
	0x4e800420,			/*  bctr		*/
	0x4e800020,			/*    br		*/
	0
  };

  for (ii=0; trampoline_code[ii]; ++ii) {
    op  = read_memory_integer (pc + (ii*4), 4);
    if (op != trampoline_code [ii])
      return NULL;
  }
  ii = read_register (11);		/* r11 holds destination addr	*/
  pc = read_memory_integer (ii, 4);	/* (r11) value			*/
  return pc;
}

