/* 
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

#define SET_WORD_MEM(x,y)  {saved_state.mem[(x)>>1] = y;}
#define SET_BYTE_MEM(x,y)  {BYTE_MEM(x)=y;}

#define WORD_MEM(x)  (saved_state.mem[(x)>>1])
#define BYTE_MEM(x)  (*(((char *)(saved_state.mem))+((x)^HOST_IS_LITTLE_ENDIAN)))

#define CCR 	8
#define PC 	9
#define CYCLES 	10
#define HCHECK 	11
#define TIER 	12
#define TCSR 	13
#define FRC 	14
#define OCRA 	15
#define OCRB 	16
#define TCR 	17
#define TOCR 	18
#define ICRA 	19
#define NREG 20
struct state
{
  unsigned short int reg[NREG];
  unsigned char *(bregp[16]);
  unsigned char *(bregp_NNNNxxxx[256]);
  unsigned char *(bregp_xxxxNNNN[256]);
  unsigned short int *(wregp_xNNNxxxx[256]);
  unsigned short int *(wregp_xxxxxNNN[256]);
  int exception;
  int ienable;
  unsigned short *mem;
}

saved_state;



#define OCFA (1<<3)
#define OCFB (1<<2)
#define CCLRA (1<<0)
/* TCR bits */
#define OCIEA (1<<3)
#define OCIEB (1<<2)
#define OVIE (1<<1)
#define OVF  (1<<1)

/* TOCR bits */
#define OCRS (1<<4)


#ifdef __GO32__
#define HOST_IS_LITTLE_ENDIAN 1
#else
#define HOST_IS_LITTLE_ENDIAN 0
#endif

#define SAVE_INTERPRETER_STATE() \
saved_state.reg[CYCLES] = cycles; \
saved_state.reg[PC] = (pc - saved_state.mem) <<1; \
saved_state.reg[CCR] = GET_CCR();  \
store_timer_state_to_mem(); 

#define LOAD_INTERPRETER_STATE() \
  SET_CCR (saved_state.reg[CCR]); \
  checkfreq = saved_state.reg[HCHECK]; \
  pc = (saved_state.reg[PC]>>1) + saved_state.mem; \
  load_timer_state_from_mem(); \
  cycles=saved_state.reg[CYCLES];
