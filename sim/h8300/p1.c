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

#define SET_WORD_MEM(x,y)  {mem[x] = (y)>>8;mem[x+1] = y;}
#define SET_BYTE_MEM(x,y)  mem[x]=y

#define WORD_MEM(x)  ((mem[x]<<8) | (mem[x+1]))
#define BYTE_MEM(x)  mem[x]


#define PC 9
#define CCR 8

struct state
{
  int cycles;
  unsigned short int reg[10];
  unsigned char *(bregp[16]);
  unsigned char *(bregp_NNNNxxxx[256]);
  unsigned char *(bregp_xxxxNNNN[256]);
  unsigned short int *(wregp_xNNNxxxx[256]);
  unsigned short int *(wregp_xxxxxNNN[256]);
}

saved_state;

#define V (v!=0)
#define C (c!=0)
#define N (n!=0)
#define Z (z!=0)

#define SET_CCR(x) n = x & 0x8; v = x & 0x2; z = x & 0x4;  c = x & 0x1;
#define GET_CCR() ((N << 3) |  (Z<<2) | (V<<1) | C)

int exception;

static unsigned char *mem;

void
control_c (sig, code, scp, addr)
     int sig;
     int code;
     char *scp;
     char *addr;
{
  exception = SIGINT;
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
  buf[0] = saved_state.reg[reg] >> 8;
  buf[1] = saved_state.reg[reg];
}

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
  if (!mem)
    {
      int tmp;

      mem = calloc (1024, 64);
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

    }
}

void
sim_write (to, from, len)
     int to;
     char *from;
     int len;
{
  meminit ();
  memcpy (mem + to, from, len);
}

void
sim_read (from, to, len)
     int from;
     char *to;

     int len;
{
  meminit ();
  memcpy (to, mem + from, len);
}

int
sim_stop_signal ()
{
  return exception;
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
  unsigned char **blow;
  unsigned char **bhigh;
  unsigned short **wlow;
  unsigned short **whigh;
  unsigned char *npc;
  int rn;
  unsigned short int *reg;
  unsigned char **bregp;
  void (*prev) ();
  unsigned char *pc;

  int srca;
  int srcb;
  int dst;
  int cycles = saved_state.cycles;

  int n;
  int v;
  int z;
  int c;

  SET_CCR (saved_state.reg[CCR]);
  pc = saved_state.reg[PC] + mem;

  reg = saved_state.reg;
  bregp = saved_state.bregp;
  blow = saved_state.bregp_xxxxNNNN;
  bhigh = saved_state.bregp_NNNNxxxx;

  wlow = saved_state.wregp_xxxxxNNN;
  whigh = saved_state.wregp_xNNNxxxx;

  prev = signal (SIGINT, control_c);
  meminit();
  if (step)
    exception = SIGTRAP;
  else
    {
      exception = sig;
    }
  do
    {
      dst = 0xfeedface;
      b0 = pc[0];
      b1 = pc[1];


