/* Intel 386 stuff.
   Copyright (C) 1988, 1989 Free Software Foundation, Inc.

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
#include "gdbcore.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <sys/file.h>
#include <sys/stat.h>

#include <sys/reg.h>

/* this table must line up with REGISTER_NAMES in m-i386.h */
/* symbols like 'EAX' come from <sys/reg.h> */
static int regmap[] = 
{
  EAX, ECX, EDX, EBX,
  UESP, EBP, ESI, EDI,
  EIP, EFL, CS, SS,
  DS, ES, FS, GS,
};

/* blockend is the value of u.u_ar0, and points to the
 * place where GS is stored
 */
i386_register_u_addr (blockend, regnum)
{
#if 0
  /* this will be needed if fp registers are reinstated */
  /* for now, you can look at them with 'info float'
   * sys5 wont let you change them with ptrace anyway
   */
  if (regnum >= FP0_REGNUM && regnum <= FP7_REGNUM) 
    {
      int ubase, fpstate;
      struct user u;
      ubase = blockend + 4 * (SS + 1) - KSTKSZ;
      fpstate = ubase + ((char *)&u.u_fpstate - (char *)&u);
      return (fpstate + 0x1c + 10 * (regnum - FP0_REGNUM));
    } 
  else
#endif
    return (blockend + 4 * regmap[regnum]);
  
}

/* This is broken for cross debugging.  Possible solutions are:

1.  Don't worry about whether the thing compiles for cross-debugging.
Go ahead and call them from i386-tdep.c.
1a.  Same thing but use some macros in xm-i386.h so that gdb will
compile for cross-debugging but just give an error (or some such behavior)
when you attempt to convert floats.

2.  Write a portable (in the sense of running on any machine; it would
always be for i387 floating-point formats) extended<->double converter
(which just deals with the values as arrays of char).

3.  Assume the host machine has *some* IEEE chip.  However, IEEE does
not standardize formats for extended floats (387 is 10 bytes, 68881 is
12 bytes), so this won't work.  */

i387_to_double (from, to)
     char *from;
     char *to;
{
  long *lp;
  /* push extended mode on 387 stack, then pop in double mode
   *
   * first, set exception masks so no error is generated -
   * number will be rounded to inf or 0, if necessary 
   */
  asm ("pushl %eax"); 		/* grab a stack slot */
  asm ("fstcw (%esp)");		/* get 387 control word */
  asm ("movl (%esp),%eax");	/* save old value */
  asm ("orl $0x3f,%eax");		/* mask all exceptions */
  asm ("pushl %eax");
  asm ("fldcw (%esp)");		/* load new value into 387 */
  
  asm ("movl 8(%ebp),%eax");
  asm ("fldt (%eax)");		/* push extended number on 387 stack */
  asm ("fwait");
  asm ("movl 12(%ebp),%eax");
  asm ("fstpl (%eax)");		/* pop double */
  asm ("fwait");
  
  asm ("popl %eax");		/* flush modified control word */
  asm ("fnclex");			/* clear exceptions */
  asm ("fldcw (%esp)");		/* restore original control word */
  asm ("popl %eax");		/* flush saved copy */
}

double_to_i387 (from, to)
     char *from;
     char *to;
{
  /* push double mode on 387 stack, then pop in extended mode
   * no errors are possible because every 64-bit pattern
   * can be converted to an extended
   */
  asm ("movl 8(%ebp),%eax");
  asm ("fldl (%eax)");
  asm ("fwait");
  asm ("movl 12(%ebp),%eax");
  asm ("fstpt (%eax)");
  asm ("fwait");
}

struct env387 
{
  unsigned short control;
  unsigned short r0;
  unsigned short status;
  unsigned short r1;
  unsigned short tag;
  unsigned short r2;
  unsigned long eip;
  unsigned short code_seg;
  unsigned short opcode;
  unsigned long operand;
  unsigned short operand_seg;
  unsigned short r3;
  unsigned char regs[8][10];
};

static
print_387_control_word (control)
unsigned short control;
{
  printf ("control 0x%04x: ", control);
  printf ("compute to ");
  switch ((control >> 8) & 3) 
    {
    case 0: printf ("24 bits; "); break;
    case 1: printf ("(bad); "); break;
    case 2: printf ("53 bits; "); break;
    case 3: printf ("64 bits; "); break;
    }
  printf ("round ");
  switch ((control >> 10) & 3) 
    {
    case 0: printf ("NEAREST; "); break;
    case 1: printf ("DOWN; "); break;
    case 2: printf ("UP; "); break;
    case 3: printf ("CHOP; "); break;
    }
  if (control & 0x3f) 
    {
      printf ("mask:");
      if (control & 0x0001) printf (" INVALID");
      if (control & 0x0002) printf (" DENORM");
      if (control & 0x0004) printf (" DIVZ");
      if (control & 0x0008) printf (" OVERF");
      if (control & 0x0010) printf (" UNDERF");
      if (control & 0x0020) printf (" LOS");
      printf (";");
    }
  printf ("\n");
  if (control & 0xe080) printf ("warning: reserved bits on 0x%x\n",
				control & 0xe080);
}

static
print_387_status_word (status)
     unsigned short status;
{
  printf ("status 0x%04x: ", status);
  if (status & 0xff) 
    {
      printf ("exceptions:");
      if (status & 0x0001) printf (" INVALID");
      if (status & 0x0002) printf (" DENORM");
      if (status & 0x0004) printf (" DIVZ");
      if (status & 0x0008) printf (" OVERF");
      if (status & 0x0010) printf (" UNDERF");
      if (status & 0x0020) printf (" LOS");
      if (status & 0x0040) printf (" FPSTACK");
      printf ("; ");
    }
  printf ("flags: %d%d%d%d; ",
	  (status & 0x4000) != 0,
	  (status & 0x0400) != 0,
	  (status & 0x0200) != 0,
	  (status & 0x0100) != 0);
  
  printf ("top %d\n", (status >> 11) & 7);
}

static
print_387_status (status, ep)
     unsigned short status;
     struct env387 *ep;
{
  int i;
  int bothstatus;
  int top;
  int fpreg;
  unsigned char *p;
  
  bothstatus = ((status != 0) && (ep->status != 0));
  if (status != 0) 
    {
      if (bothstatus)
	printf ("u: ");
      print_387_status_word (status);
    }
  
  if (ep->status != 0) 
    {
      if (bothstatus)
	printf ("e: ");
      print_387_status_word (ep->status);
    }
  
  print_387_control_word (ep->control);
  printf ("last exception: ");
  printf ("opcode 0x%x; ", ep->opcode);
  printf ("pc 0x%x:0x%x; ", ep->code_seg, ep->eip);
  printf ("operand 0x%x:0x%x\n", ep->operand_seg, ep->operand);
  
  top = (ep->status >> 11) & 7;
  
  printf ("regno  tag  msb              lsb  value\n");
  for (fpreg = 7; fpreg >= 0; fpreg--) 
    {
      double val;
      
      printf ("%s %d: ", fpreg == top ? "=>" : "  ", fpreg);
      
      switch ((ep->tag >> (fpreg * 2)) & 3) 
	{
	case 0: printf ("valid "); break;
	case 1: printf ("zero  "); break;
	case 2: printf ("trap  "); break;
	case 3: printf ("empty "); break;
	}
      for (i = 9; i >= 0; i--)
	printf ("%02x", ep->regs[fpreg][i]);
      
      i387_to_double (ep->regs[fpreg], (char *)&val);
      printf ("  %g\n", val);
    }
  if (ep->r0)
    printf ("warning: reserved0 is 0x%x\n", ep->r0);
  if (ep->r1)
    printf ("warning: reserved1 is 0x%x\n", ep->r1);
  if (ep->r2)
    printf ("warning: reserved2 is 0x%x\n", ep->r2);
  if (ep->r3)
    printf ("warning: reserved3 is 0x%x\n", ep->r3);
}

#ifndef U_FPSTATE
#define U_FPSTATE(u) u.u_fpstate
#endif

i386_float_info ()
{
  struct user u; /* just for address computations */
  int i;
  /* fpstate defined in <sys/user.h> */
  struct fpstate *fpstatep;
  char buf[sizeof (struct fpstate) + 2 * sizeof (int)];
  unsigned int uaddr;
  char fpvalid;
  unsigned int rounded_addr;
  unsigned int rounded_size;
  extern int corechan;
  int skip;
  
  uaddr = (char *)&u.u_fpvalid - (char *)&u;
  if (have_inferior_p()) 
    {
      unsigned int data;
      unsigned int mask;
      
      rounded_addr = uaddr & -sizeof (int);
      data = ptrace (3, inferior_pid, rounded_addr, 0);
      mask = 0xff << ((uaddr - rounded_addr) * 8);
      
      fpvalid = ((data & mask) != 0);
    } 
  else 
    {
      if (lseek (corechan, uaddr, 0) < 0)
	perror ("seek on core file");
      if (myread (corechan, &fpvalid, 1) < 0) 
	perror ("read on core file");
      
    }
  
  if (fpvalid == 0) 
    {
      printf ("no floating point status saved\n");
      return;
    }
  
  uaddr = (char *)&U_FPSTATE(u) - (char *)&u;
  if (have_inferior_p ()) 
    {
      int *ip;
      
      rounded_addr = uaddr & -sizeof (int);
      rounded_size = (((uaddr + sizeof (struct fpstate)) - uaddr) +
		      sizeof (int) - 1) / sizeof (int);
      skip = uaddr - rounded_addr;
      
      ip = (int *)buf;
      for (i = 0; i < rounded_size; i++) 
	{
	  *ip++ = ptrace (3, inferior_pid, rounded_addr, 0);
	  rounded_addr += sizeof (int);
	}
    } 
  else 
    {
      if (lseek (corechan, uaddr, 0) < 0)
	perror_with_name ("seek on core file");
      if (myread (corechan, buf, sizeof (struct fpstate)) < 0) 
	perror_with_name ("read from core file");
      skip = 0;
    }
  
  fpstatep = (struct fpstate *)(buf + skip);
  print_387_status (fpstatep->status, (struct env387 *)fpstatep->state);
}

