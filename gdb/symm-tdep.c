/* Sequent Symmetry target interface, for GDB when running under Unix.
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

/* many 387-specific items of use taken from i386-dep.c */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "symtab.h"

#include <signal.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/dir.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include "gdbcore.h"
#include <fcntl.h>

static long i386_get_frame_setup ();
static i386_follow_jump ();

#include <sgtty.h>
#define TERMINAL struct sgttyb

/* rounds 'one' up to divide evenly by 'two' */

int
round(one,two)
register int one, two;

{
    register int temp;
    temp = (one/two)*two;
    if (one != temp) {
	temp += two;
    }
    return temp;
}


static CORE_ADDR codestream_next_addr;
static CORE_ADDR codestream_addr;
static unsigned char codestream_buf[sizeof (int)];
static int codestream_off;
static int codestream_cnt;

#define codestream_tell() (codestream_addr + codestream_off)
#define codestream_peek() (codestream_cnt == 0 ? \
			   codestream_fill(1): codestream_buf[codestream_off])
#define codestream_get() (codestream_cnt-- == 0 ? \
			 codestream_fill(0) : codestream_buf[codestream_off++])


static unsigned char 
codestream_fill (peek_flag)
{
  codestream_addr = codestream_next_addr;
  codestream_next_addr += sizeof (int);
  codestream_off = 0;
  codestream_cnt = sizeof (int);
  read_memory (codestream_addr,
	       (unsigned char *)codestream_buf,
	       sizeof (int));
  
  if (peek_flag)
    return (codestream_peek());
  else
    return (codestream_get());
}

static void
codestream_seek (place)
{
  codestream_next_addr = place & -sizeof (int);
  codestream_cnt = 0;
  codestream_fill (1);
  while (codestream_tell() != place)
    codestream_get ();
}

static void
codestream_read (buf, count)
     unsigned char *buf;
{
  unsigned char *p;
  int i;
  p = buf;
  for (i = 0; i < count; i++)
    *p++ = codestream_get ();
}

/*
 * Following macro translates i386 opcode register numbers to Symmetry
 * register numbers.  This is used by FRAME_FIND_SAVED_REGS.
 *
 *           %eax  %ecx  %edx  %ebx  %esp  %ebp  %esi  %edi
 * i386        0     1     2     3     4     5     6     7
 * Symmetry    0     2     1     5    14    15     6     7
 *
 */
#define I386_REGNO_TO_SYMMETRY(n) \
((n)==0?0 :(n)==1?2 :(n)==2?1 :(n)==3?5 :(n)==4?14 :(n)==5?15 :(n))

/* from i386-dep.c */
i386_frame_find_saved_regs (fip, fsrp)
     struct frame_info *fip;
     struct frame_saved_regs *fsrp;
{
  unsigned long locals;
  unsigned char *p;
  unsigned char op;
  CORE_ADDR dummy_bottom;
  CORE_ADDR adr;
  int i;
  
  memset (fsrp, 0, sizeof *fsrp);
  
  /* if frame is the end of a dummy, compute where the
   * beginning would be
   */
  dummy_bottom = fip->frame - 4 - NUM_REGS*4 - CALL_DUMMY_LENGTH;
  
  /* check if the PC is in the stack, in a dummy frame */
  if (dummy_bottom <= fip->pc && fip->pc <= fip->frame) 
    {
      /* all regs were saved by push_call_dummy () */
      adr = fip->frame - 4;
      for (i = 0; i < NUM_REGS; i++) 
	{
	  fsrp->regs[i] = adr;
	  adr -= 4;
	}
      return;
    }
  
  locals = i386_get_frame_setup (get_pc_function_start (fip->pc));
  
  if (locals >= 0) 
    {
      adr = fip->frame - 4 - locals;
      for (i = 0; i < 8; i++) 
	{
	  op = codestream_get ();
	  if (op < 0x50 || op > 0x57)
	    break;
	  fsrp->regs[I386_REGNO_TO_SYMMETRY(op - 0x50)] = adr;
	  adr -= 4;
	}
    }
  
  fsrp->regs[PC_REGNUM] = fip->frame + 4;
  fsrp->regs[FP_REGNUM] = fip->frame;
}

static long
i386_get_frame_setup (pc)
{
  unsigned char op;
  
  codestream_seek (pc);
  
  i386_follow_jump ();
  
  op = codestream_get ();
  
  if (op == 0x58) /* popl %eax */
    {
      /*
       * this function must start with
       * 
       *    popl %eax		  0x58
       *    xchgl %eax, (%esp)  0x87 0x04 0x24
       * or xchgl %eax, 0(%esp) 0x87 0x44 0x24 0x00
       *
       * (the system 5 compiler puts out the second xchg
       * inst, and the assembler doesn't try to optimize it,
       * so the 'sib' form gets generated)
       * 
       * this sequence is used to get the address of the return
       * buffer for a function that returns a structure
       */
      int pos;
      unsigned char buf[4];
      static unsigned char proto1[3] = { 0x87,0x04,0x24 };
      static unsigned char proto2[4] = { 0x87,0x44,0x24,0x00 };
      pos = codestream_tell ();
      codestream_read (buf, 4);
      if (memcmp (buf, proto1, 3) == 0)
	pos += 3;
      else if (memcmp (buf, proto2, 4) == 0)
	pos += 4;
      
      codestream_seek (pos);
      op = codestream_get (); /* update next opcode */
    }
  
  if (op == 0x55) 			/* pushl %esp */
    {
      if (codestream_get () != 0x8b)	/* movl %esp, %ebp (2bytes) */
	return (-1);
      if (codestream_get () != 0xec)
	return (-1);
      /*
       * check for stack adjustment 
       *
       *  subl $XXX, %esp
       *
       * note: you can't subtract a 16 bit immediate
       * from a 32 bit reg, so we don't have to worry
       * about a data16 prefix 
       */
      op = codestream_peek ();
      if (op == 0x83)  /* subl with 8 bit immed */
	{
	  codestream_get ();
	  if (codestream_get () != 0xec)
	    return (-1);
	  /* subl with signed byte immediate 
	   * (though it wouldn't make sense to be negative)
	   */
	  return (codestream_get());
	}
      else if (op == 0x81)  /* subl with 32 bit immed */
	{
	  int locals;
	  if (codestream_get () != 0xec)
	    return (-1);
	  /* subl with 32 bit immediate */
	  codestream_read ((unsigned char *)&locals, 4);
	  return (locals);
	} 
      else 
	{
	  return (0);
	}
    } 
  else if (op == 0xc8) 
    {
      /* enter instruction: arg is 16 unsigned immed */
      unsigned short slocals;
      codestream_read ((unsigned char *)&slocals, 2);
      codestream_get (); /* flush final byte of enter instruction */
      return (slocals);
    }
  return (-1);
}

/* next instruction is a jump, move to target */
static
i386_follow_jump ()
{
  int long_delta;
  short short_delta;
  char byte_delta;
  int data16;
  int pos;
  
  pos = codestream_tell ();
  
  data16 = 0;
  if (codestream_peek () == 0x66)
    {
      codestream_get ();
      data16 = 1;
    }
  
  switch (codestream_get ())
    {
    case 0xe9:
      /* relative jump: if data16 == 0, disp32, else disp16 */
      if (data16)
	{
	  codestream_read ((unsigned char *)&short_delta, 2);
	  pos += short_delta + 3; /* include size of jmp inst */
	}
      else
	{
	  codestream_read ((unsigned char *)&long_delta, 4);
	  pos += long_delta + 5;
	}
      break;
    case 0xeb:
      /* relative jump, disp8 (ignore data16) */
      codestream_read ((unsigned char *)&byte_delta, 1);
      pos += byte_delta + 2;
      break;
    }
  codestream_seek (pos + data16);
}

/* return pc of first real instruction */
/* from i386-dep.c */

i386_skip_prologue (pc)
{
  unsigned char op;
  int i;
  
  if (i386_get_frame_setup (pc) < 0)
    return (pc);
  
  /* found valid frame setup - codestream now points to 
   * start of push instructions for saving registers
   */
  
  /* skip over register saves */
  for (i = 0; i < 8; i++)
    {
      op = codestream_peek ();
      /* break if not pushl inst */
      if (op < 0x50 || op > 0x57) 
	break;
      codestream_get ();
    }
  
  i386_follow_jump ();
  
  return (codestream_tell ());
}

void
symmetry_extract_return_value(type, regbuf, valbuf)
     struct type *type;
     char *regbuf;
     char *valbuf;
{
  union { 
    double	d; 
    int	l[2]; 
  } xd; 
  struct minimal_symbol *msymbol;
  float f;

  if (TYPE_CODE_FLT == TYPE_CODE(type)) { 
    msymbol = lookup_minimal_symbol ("1167_flt", (struct objfile *) NULL);
    if (msymbol != NULL) {
      /* found "1167_flt" means 1167, %fp2-%fp3 */ 
      /* float & double; 19= %fp2, 20= %fp3 */
      /* no single precision on 1167 */
      xd.l[1] = *((int *)&regbuf[REGISTER_BYTE(19)]);
      xd.l[0] = *((int *)&regbuf[REGISTER_BYTE(20)]);
      switch (TYPE_LENGTH(type)) {
      case 4:
	/* FIXME: broken for cross-debugging.  */
	f = (float) xd.d;
	memcpy (valbuf, &f, TYPE_LENGTH(type));
	break;
      case 8:
	/* FIXME: broken for cross-debugging.  */
	memcpy (valbuf, &xd.d, TYPE_LENGTH(type)); 
	break;
      default:
	error("Unknown floating point size");
	break;
      }
    } else { 
      /* 387 %st(0), gcc uses this */ 
      i387_to_double(((int *)&regbuf[REGISTER_BYTE(3)]),
		     &xd.d); 
      switch (TYPE_LENGTH(type)) {
      case 4:			/* float */
	f = (float) xd.d;
	/* FIXME: broken for cross-debugging.  */
	memcpy (valbuf, &f, 4); 
	break;
      case 8:			/* double */
	/* FIXME: broken for cross-debugging.  */
	memcpy (valbuf, &xd.d, 8);
	break;
      default:
	error("Unknown floating point size");
	break;
      }
    }
  } else {
    memcpy (valbuf, regbuf, TYPE_LENGTH (type)); 
  }
}

#ifdef _SEQUENT_ /* ptx, not dynix */
/*
 * Convert compiler register number to gdb internal
 * register number.  The PTX C compiler only really
 * puts things in %edi, %esi and %ebx, but it can't hurt
 * to be complete here.
 */
int
ptx_coff_regno_to_gdb(regno)
     int regno;
{
  return I386_REGNO_TO_SYMMETRY(regno);
}

/* For ptx, the value in blockend will be meaningless.  This function
   merely returns the proper offset given the register number.  This
   is much easier, because under ptx, the upage is set up with the
   user struct on "top", and the registers "beneath" it (and thus defines
   TRAD_CORE_USER_OFFSET in bfd).  */

/* The following table is for ptx 1.3.  In theory it should not change with
   the OS version, but if it does we should (if possible) figure out a way
   to accept both the old and the new formats.  */

static unsigned int reg_offsets[NUM_REGS] = {
/*
 * u.u_ar0 = 0xfffff8d0
 * VA_UBLOCK = 0xffffe000
 * VA_UAREA = 0xfffff8e8
 * struct user at ublock offset 0x18e8
 * registers at ublock offset 0x18d0
 */
0x18d0, /* eax */
0x18c8, /* eax */
0x18cc, /* eax */
0x1be0, /* st0 */
0x1bea, /* st1 */
0x18c4, /* ebx */
0x18b8, /* esi */
0x18b4, /* edi */
0x1bf4, /* st2 */
0x1bfe, /* st3 */
0x1c08, /* st4 */
0x1c12, /* st5 */
0x1c1c, /* st6 */
0x1c26, /* st7 */
0x18e0, /* esp */
0x18bc, /* ebp */
0x18d4, /* eip */
0x18dc, /* flags */
0x1c38, /* fp1 */
0x1c3c, /* fp2 */
0x1c40, /* fp3 */
0x1c44, /* fp4 */
0x1c48, /* fp5 */
0x1c4c, /* fp6 */
0x1c50, /* fp7 */
0x1c54, /* fp8 */
0x1c58, /* fp9 */
0x1c5c, /* fp10 */
0x1c60, /* fp11 */
0x1c64, /* fp12 */
0x1c68, /* fp13 */
0x1c6c, /* fp14 */
0x1c70, /* fp15 */
0x1c74, /* fp16 */
0x1c78, /* fp17 */
0x1c7c, /* fp18 */
0x1c80, /* fp19 */
0x1c84, /* fp20 */
0x1c88, /* fp21 */
0x1c8c, /* fp22 */
0x1c90, /* fp23 */
0x1c94, /* fp24 */
0x1c98, /* fp25 */
0x1c9c, /* fp26 */
0x1ca0, /* fp27 */
0x1ca4, /* fp28 */
0x1ca8, /* fp29 */
0x1cac, /* fp30 */
0x1cb0, /* fp31 */
};

unsigned int
register_addr (regno, blockend)
     int regno, blockend;
{
  if ((regno < 0) || (regno >= NUM_REGS)) {
    error("Invalid register number %d.", regno);
  }
  return reg_offsets[regno];
}
#endif /* _SEQUENT_ */
