/* perfipheral simulation

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

/* Fake peripherals for the H8/330 */
#include "state.h"

/* This routine is called every few instructions to see if some sort
  of hardware event is needed */

perifs( )
{
  int interrupt = 0;
  int lval;
  int tmp;
  /* What to do about the 16 bit timer */


  /* Free running counter same as reg a */
  if (saved_state.reg[OCRA] == saved_state.reg[FRC])
  {
    /* Set the counter A overflow bit */
    saved_state.reg[TCSR] |= OCFA;

    if (saved_state.reg[TCSR] & CCLRA) 
    {
      saved_state.reg[FRC] = 0;
    }

    if (saved_state.reg[TIER] & OCIEA) 
    {
      interrupt = 16;
    }
  }

  /* Free running counter same as reg b */
  if (saved_state.reg[OCRB] == saved_state.reg[FRC])
  {
    saved_state.reg[TCSR] |= OCFB;
    if (saved_state.reg[TIER] & OCIEB) 
    {
      interrupt = 17;
    }
  }

  /* inc free runnning counter */
  saved_state.reg[FRC]++;
  
  if (saved_state.reg[FRC] == 0) 
  {
    /* Must have overflowed */
    saved_state.reg[TCSR] |=  OVF;
    if (BYTE_MEM(TIER) & OVIE)
     interrupt = 18;
  }

  /* If we've had an interrupt and the bit is on */
  if (interrupt && saved_state.ienable) 
  {

    int  ccr;

    saved_state.ienable = 0;    
    ccr = saved_state.reg[CCR];
    lval = WORD_MEM((interrupt)<<1);
    lval = WORD_MEM(lval);
   {
     /* Push PC */
     saved_state.reg[7] -= 2;
     tmp = saved_state.reg[7];
     SET_WORD_MEM (tmp, saved_state.reg[PC]);
     /* Push CCR twice */
     saved_state.reg[7] -=2 ;
     tmp =  saved_state.reg[7];
     SET_BYTE_MEM(tmp,ccr);
     SET_BYTE_MEM(tmp+1,ccr);

     /* Set pc to point to first instruction of i vector */
     saved_state.reg[PC] = lval;
   }
  }
}
