/* Interpreter fragment for the Hitachi H8/300 architecture simulator.

   Written by Steve Chamberlain of Cygnus Support.
   sac@cygnus.com

   This file is part of H8/300 sim


		THIS SOFTWARE IS NOT COPYRIGHTED

   Cygnus offers the following for use in the public domain.  Cygnus
   makes no warranty with regard to the software or it's performance
   and the user accepts the software "AS IS" with all faults.

   CYGNUS DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD TO
   THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*/

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
  if (saved_state.mem == 0)
    {
      int tmp;

      saved_state.mem = (unsigned short *)calloc (1024, 64);
      littleendian.i = 1;
      /* initialze the array of pointers to byte registers */
      for (tmp = 0; tmp < 8; tmp++)
	{
	  if (littleendian.u.high)
	    {
	      saved_state.bregp[tmp] = (unsigned char *) (saved_state.reg + tmp);
	      saved_state.bregp[tmp + 8] = saved_state.bregp[tmp] + 1;
	    }
	  else
	    {
	      saved_state.bregp[tmp + 8] = (unsigned char *) (saved_state.reg + tmp);
	      saved_state.bregp[tmp] = saved_state.bregp[tmp + 8] + 1;
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



