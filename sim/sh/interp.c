#define MSIZE (256*1024)
#define MMASKL ((MSIZE -1) & ~3)
#define MMASKW ((MSIZE -1) & ~1)
#define MMASKB ((MSIZE -1) & ~0)
/* Simulator for the Hitachi SH architecture.

   Written by Steve Chamberlain of Cygnus Support.
   sac@cygnus.com

   This file is part of SH sim


		THIS SOFTWARE IS NOT COPYRIGHTED

   Cygnus offers the following for use in the public domain.  Cygnus
   makes no warranty with regard to the software or it's performance
   and the user accepts the software "AS IS" with all faults.

   CYGNUS DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD TO
   THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*/

#include <signal.h>
#include <sys/times.h>
#include <sys/param.h>

#define O_RECOMPILE 85
#define DEFINE_TABLE

#define DISASSEMBLER_TABLE

#define SBIT(x) ((x)&sbit)
#define R0 saved_state.asregs.regs[0]
#define Rn saved_state.asregs.regs[n]
#define Rm saved_state.asregs.regs[m]

#define UR0 (unsigned long)(saved_state.asregs.regs[0])
#define UR (unsigned long)R
#define UR (unsigned long)R

#define SR0 saved_state.asregs.regs[0]

#define GBR saved_state.asregs.gbr
#define VBR saved_state.asregs.vbr
#define MACH saved_state.asregs.mach
#define MACL saved_state.asregs.macl
#define GET_SR() (saved_state.asregs.sr.bits.t = T, saved_state.asregs.sr.word)
#define SET_SR(x) {saved_state.asregs.sr.word = (x); T =saved_state.asregs.sr.bits.t;}

#define PC pc
#define C cycles

#define LMEM(x) *((long *)(memory+(x&maskl)))
#define BMEM(x) *((char *)(memory+(x&maskb)))
#define UWMEM(x) *((unsigned short *)(memory+(x&maskw)))
#define SWMEM(x) *((short *)(memory+(x&maskw)))
#define WLAT(x,value)  (LMEM(x) = value)
#define RLAT(x)  (LMEM(x))

#define WWAT(x,value)  (UWMEM(x) = value)
#define RSWAT(x)  (SWMEM(x))
#define RUWAT(x)  (UWMEM(x))

#define WBAT(x,value)  (BMEM(x) = value)
#define RBAT(x)  (BMEM(x))

#define SEXT(x)     ((int)((char)x))
#define SEXTW(y)    ((int)((short)y))
#define M saved_state.asregs.sr.bits.m
#define Q saved_state.asregs.sr.bits.q
#define SL(TEMPPC)  iword= RUWAT(TEMPPC); goto top;
int debug;
typedef union
{

  struct
    {

      int regs[16];
      int pc;
      int pr;

      int gbr;
      int vbr;
      int mach;
      int macl;


      union
	{
	  struct
	    {
	      int d0:22;
	      int m:1;
	      int q:1;
	      int i:4;
	      int d1:2;
	      int s:1;
	      int t:1;
	    }
	  bits;
	  int word;
	}
      sr;
      int ticks;
      int cycles;
      int insts;
      unsigned char *memory;
      int exception;

    }
  asregs;
  int asints[25];

}

saved_state_type;



saved_state_type saved_state;



/*#include "../opcodes/sh-opc.h"*/


static int
get_now ()
{
  struct tms b;
  times (&b);
  return b.tms_utime + b.tms_stime;
}

static int
now_persec ()
{
  return HZ;
}

/* simulate a monitor trap */
trap (i, regs)
     int *regs;
{
  switch (i)
    {
    case 1:
      printf ("%c", regs[0]);
      break;
    case 2:
      saved_state.asregs.exception = SIGQUIT;
      break;
    case 255:
      saved_state.asregs.exception = SIGILL;
      break;
    }

}
void
control_c (sig, code, scp, addr)
     int sig;
     int code;
     char *scp;
     char *addr;
{
  saved_state.asregs.exception = SIGINT;
}


int div1(R,m,n,T)
     int *R;
     int m;
     int n;
     int T;
{
  unsigned long tmp0;
  unsigned char old_q, tmp1;
  
  old_q = Q;
  Q= R[n] <0;
  
  R[n] <<=1;
  R[n] |= T;
  
  switch (old_q) 
    {
    case 0:
      switch (M) 
	{
	case 0:
	  tmp0 = R[n];
	  R[n] -= R[m];
	  tmp1 = (R[n] > tmp0) != Q;
	  break;
	case 1:
	  tmp0 = R[n];
	  R[n] += R[m];
	  tmp1 = (R[n] < tmp0) == Q;
	  break;
	}
      break;
    case 1:
      switch (M)
	{
	case 0:
	  tmp0 = R[n];
	  R[n] += R[m];
	  tmp1 = (R[n] < tmp0) != Q;
	  break;
	case 1:
	  tmp0 = R[n];
	  R[n] -= R[m];
	  tmp1 = (R[n] > tmp0) == Q;
	  break;
	}
      break;

    }

  T=(Q==M);  
return T;  

}

     
int
sim_resume (step)
{
  static int init1;
  int pc;
register  int cycles = 0;
register  int insts = 0;
  int tick_start = get_now ();
  void (*prev) ();
  extern unsigned char sh_jump_table0[];

  register unsigned char *jump_table = sh_jump_table0;

  register int *R = &(saved_state.asregs.regs[0]);
  register int T;
  register int PR;

  register int maskb = MMASKB;
  register int maskw = MMASKW;
  register int maskl = MMASKL;
  register unsigned char *memory = saved_state.asregs.memory;
  register int sbit = (1<<31);
  
  prev = signal (SIGINT, control_c);

  if (step)
    {
      saved_state.asregs.exception = SIGTRAP;
    }
  else
    {
      saved_state.asregs.exception = 0;
    }

  pc = saved_state.asregs.pc;
  PR = saved_state.asregs.pr;
  T = saved_state.asregs.sr.bits.t;

  do
    {
      unsigned int iword = RUWAT (pc);
      unsigned long ult;

      insts++;
    top:

#include "code.c"

      pc += 2;
      cycles++;
    }
  while (!saved_state.asregs.exception);

  if (saved_state.asregs.exception == SIGILL) 
    {
      pc-=2;
    }
  
  saved_state.asregs.ticks += get_now () - tick_start;
  saved_state.asregs.cycles += cycles;
  saved_state.asregs.insts += insts;
  saved_state.asregs.pc = pc;
  saved_state.asregs.sr.bits.t = T;
  saved_state.asregs.pr = PR;

  signal (SIGINT, prev);
}



void
sim_write (addr, buffer, size)
     long int addr;
     unsigned char *buffer;
     int size;
{
  int i;
  init_pointers ();

  for (i = 0; i < size; i++)
    {
      saved_state.asregs.memory[MMASKB & (addr + i)] = buffer[i];
    }
}

void
sim_read (addr, buffer, size)
     long int addr;
     char *buffer;
     int size;
{
  int i;

  init_pointers ();

  for (i = 0; i < size; i++)
    {
      buffer[i] = saved_state.asregs.memory[MMASKB & (addr + i)];
    }
}


sim_store_register (rn, value)
     int rn;
     int value;
{
  saved_state.asregs.regs[rn] = value;
}

sim_fetch_register (rn, buf)
     int rn;
     char *buf;
{

  int value = ((int *) (&saved_state))[rn];

  buf[0] = value >> 24;
  buf[1] = value >> 16;
  buf[2] = value >> 8;
  buf[3] = value >> 0;

}

int
sim_trace ()
{

  int i;
  return 0;

}

sim_stop_signal ()
{
  return saved_state.asregs.exception;
}

sim_set_pc (x)
{
  saved_state.asregs.pc = x;
}



sim_info ()
{
  double timetaken = (double) saved_state.asregs.ticks / (double) now_persec ();
  double virttime = saved_state.asregs.cycles / 10.0e6;

  printf ("\n\ninstructions executed  %10d\n", saved_state.asregs.insts);
  printf ("cycles                  %10d\n", saved_state.asregs.cycles);
  printf ("real time taken        %10.4f\n", timetaken);
  printf ("cycles/second           %10d\n", (int)(saved_state.asregs.cycles/timetaken));
  printf ("virtual time taked     %10.4f\n", virttime);
  printf ("simulation ratio       %10.4f\n", virttime / timetaken);
}

init_pointers ()
{
  if (!saved_state.asregs.memory)
    {
      saved_state.asregs.memory = (unsigned char *) (calloc (64, MSIZE / 64));
    }
}
