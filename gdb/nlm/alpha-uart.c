/*
 * Copyright (C) 1993, 1994 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 */



/*$Id$*/

#include "nwtypes.h"
#include "alpha-uart.h"
/*#include "alphansi.h"*/
#include "alpha-patch.h"
#include <stdio.h>

ULONG com_interrupt()
{
  if (inVti(com1Lsr) & 1)
    return COM1 | inVti (com1Rbr);

  return 0;
}

void 
  putcLpt(int c)
{
  int i, s;
  while(!((s = inVti(lptSr)) & 0x80))
    ;

  for(i=0;i<lptDly;++i)
    ;

  outVti(lptDr,    c);
  for(i=0;i<lptDly;++i)
    ;

  outVti(lptCr,   lptnInit|lptSTB|lptAFD);
  for(i=0;i<lptDly;++i)
    ;
 
  outVti(lptCr,   lptnInit|lptAFD);
  for(i=0;i<lptDly;++i)
    ;

  s = inVti(lptSr);
}



void  
  uart_putchar(int port, char c)
{ 
  int lsr, thr;
  switch(port)
  {
  case COM1:
    lsr = com1Lsr;
    thr = com1Thr;
    break;
  case COM2:
    lsr = com2Lsr;
    thr = com2Thr;
    break;    
#if 0
  case LPT:
    putcLpt(c);
    return;
  case VGA:
    vga_putc(c);
    return;
#endif
  default:
    return;   
  }
  
  while ((inVti(lsr) & 0x20) == 0);
  outVti(thr,c);
  if (c == '\n') uart_putchar(port, '\r');
}



int uart_init_line(int line, int baud)
{
  int i;
  int baudconst;

  switch (baud)
    {
    case 57600: baudconst = 2; break;
    case 38400: baudconst = 3; break;
    case 19200: baudconst = 6; break;
    case 9600: baudconst = 12; break;
    case 4800: baudconst = 24; break;
    case 2400: baudconst = 48; break;
    case 1200: baudconst = 96; break;
    case 300: baudconst = 384; break;
    case 150: baudconst = 768; break;
    default: baudconst = 12; break;
    }

  if (line == 1)
    { 
      outVti(com1Lcr, 0x87);
      outVti(com1Dlm,    0);
      outVti(com1Dll,   baudconst);
      outVti(com1Lcr, 0x07);
      outVti(com1Mcr, 0x0F);
      com1_disable_int();
    }

  else if (line == 2)
    {
      outVti(com2Lcr, 0x87);
      outVti(com2Dlm,    0);
      outVti(com2Dll,   baudconst);
      outVti(com2Lcr, 0x07);
      outVti(com2Mcr, 0x0F);
    }
  fprintf (stderr, "com1Ier = 0x%x\n", inVti(com1Ier));
  fprintf (stderr, "com1Lcr = 0x%x\n", inVti(com1Lcr));
  fprintf (stderr, "com1Dlm = 0x%x\n", inVti(com1Dlm));
  fprintf (stderr, "com1Dll = 0x%x\n", inVti(com1Dll));
  fprintf (stderr, "com1Lcr = 0x%x\n", inVti(com1Lcr));
  fprintf (stderr, "com1Mcr = 0x%x\n", inVti(com1Mcr));
}



int  
  uart_init()
{ 
  uart_init_line(1, 19200);
}



com1_enable_int()
{
  outVti(com1Ier, 0x01);
}



com1_disable_int()
{
  outVti(com1Ier, 0x0);
}



com2_enable_int()
{
  outVti(com2Ier, 0x01);
}



com2_disable_int()
{
  outVti(com2Ier, 0x0);
}
