/* H8/300 simulator
   Copyright 1992 Free Software Foundation, Inc.

   Contributed by Cygnus Support.
   Written by Steve Chamberlain   (sac@cygnus.com).


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
#include <stdlib.h>
#include <signal.h>
#include "state.h"

#define V (v!=0)
#define C (c!=0)
#define N (n!=0)
#define Z (z!=0)

#define SET_CCR(x) n = x & 0x8; v = x & 0x2; z = x & 0x4;  c = x & 0x1;saved_state.ienable=x&0x80;
#define GET_CCR() ((N << 3) |  (Z<<2) | (V<<1) | C) | ((!saved_state.ienable)<<7)



static union
{
  short int i;
  struct
    {
      char low;
      char high;
    }

  u;
}

littleendian;

static void
meminit ()
{
  if (!saved_state.mem)
    {
      int tmp;

      saved_state.mem = calloc (1024, 64);
      littleendian.i = 1;
      /* initialze the array of pointers to byte registers */
      for (tmp = 0; tmp < 8; tmp++)
	{
	  if (littleendian.u.high)
	    {
	      saved_state.bregp[tmp] = (unsigned char *) (saved_state.reg + tmp);
	      saved_state.bregp[tmp + 8] = saved_state.bregp[tmp] + 1;
	      if (HOST_IS_LITTLE_ENDIAN) 
	       abort();
	    }
	  else
	    {
	      saved_state.bregp[tmp + 8] = (unsigned char *) (saved_state.reg + tmp);
	      saved_state.bregp[tmp] = saved_state.bregp[tmp + 8] + 1;
	      if (!HOST_IS_LITTLE_ENDIAN) 
	       abort();
	    }
	}

      /* we keep two 256 sized pointers to byte regs, one for when we
       want to look at the reg descibed by bits NNNNxxxx and one for
       when we want to look at xxxxNNNN */
      for (tmp = 0; tmp < 256; tmp++)
	{
	  saved_state.bregp_NNNNxxxx[tmp] = saved_state.bregp[(tmp >> 4) & 0xf];
	  saved_state.bregp_xxxxNNNN[tmp] = saved_state.bregp[tmp & 0xf];
	}
      /* We keep two 256 sized pointers to word regs, one for regs in
       xNNNxxxx and one for regs in xxxxxNNNN */
      for (tmp = 0; tmp < 256; tmp++)
	{
	  saved_state.wregp_xNNNxxxx[tmp] = &saved_state.reg[(tmp >> 4) & 0x7];
	  saved_state.wregp_xxxxxNNN[tmp] = &saved_state.reg[tmp & 0x7];
	}

  saved_state.reg[HCHECK] = 10000000; /* don't check the hardware
					 often */
    }

}


void
control_c (sig, code, scp, addr)
     int sig;
     int code;
     char *scp;
     char *addr;
{
  saved_state.exception = SIGINT;
}

void
sim_store_register (reg, val)
int reg;
int val;
{
  saved_state.reg[reg] = val;
}

void
sim_fetch_register (reg, buf)
int reg;
char *buf;
{
  meminit();
  buf[0] = saved_state.reg[reg] >> 8;
  buf[1] = saved_state.reg[reg];
}

void
sim_write (to, from, len)
     int to;
     char *from;
     int len;
{
  int i;
  meminit ();

  for ( i = 0; i < len; i++)
   SET_BYTE_MEM(to + i, from[i]);
}

void
sim_read (from, to, len)
     int from;
     char *to;

     int len;
{
  int i;
  meminit ();
  for (i = 0; i < len; i++) {
    to[i] = BYTE_MEM(from + i);
  }
}

int
sim_stop_signal ()
{
  return saved_state.exception;
}

 void
load_timer_state_from_mem()
{

  saved_state.reg[TIER] = BYTE_MEM(0xff90);
  saved_state.reg[TCSR] = BYTE_MEM(0xff91);
  saved_state.reg[FRC] = WORD_MEM(0xff92);
  saved_state.reg[TCR] = BYTE_MEM(0xff96);
  saved_state.reg[TOCR] = BYTE_MEM(0xff97);


  if ((saved_state.reg[TOCR] & OCRS) == 0) 
  {
    saved_state.reg[OCRA] = WORD_MEM(0xff94);
  }
  else 
  {
    saved_state.reg[OCRB] = WORD_MEM(0xff94);
  }
}

void
store_timer_state_to_mem()
{

  BYTE_MEM(0xff91) = saved_state.reg[TCSR];
  SET_WORD_MEM(0xff92, saved_state.reg[FRC]);
}

void
sim_resume (step, sig)
int step;
int sig;
{
  int lval;
  int tmp;
  int b0;
  int b1;
  int checkfreq;
   int ni; /* Number of insts to execute before checking hw state */
  unsigned char **blow;
  unsigned char **bhigh;
  unsigned short **wlow;
  unsigned short **whigh;
  unsigned short *npc;
  int rn;
  unsigned short int *reg;
  unsigned char **bregp;
  void (*prev) ();
  unsigned short *pc;

  int srca;
  int srcb;
  int dst;
  int cycles ;

  int n;
  int v;
  int z;
  int c;


/* Set up pointers to areas */
  reg = saved_state.reg;
  bregp = saved_state.bregp;
  blow = saved_state.bregp_xxxxNNNN;
  bhigh = saved_state.bregp_NNNNxxxx;

  wlow = saved_state.wregp_xxxxxNNN;
  whigh = saved_state.wregp_xNNNxxxx;


  prev = signal (SIGINT, control_c);
  meminit();
LOAD_INTERPRETER_STATE();
  if (step)
    saved_state.exception = SIGTRAP;
  else
    {
      saved_state.exception = sig;
    }
  do
    {
      b1 = pc[0];
      b0 = b1>> 8;
      b1 &= 0xff;



