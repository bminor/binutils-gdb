/* Target-dependent code for the Fujitsu SPARCLITE for GDB, the GNU debugger.
   Copyright 1994  Free Software Foundation, Inc.

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

#include "defs.h"
#include "gdbcore.h"
#include "breakpoint.h"

#define DDA2_SUP_ASI		0xb000000
#define DDA1_SUP_ASI		0xb0000

#define DDA2_ASI_MASK 		0xff000000
#define DDA1_ASI_MASK 		0xff0000 
#define DIA2_SUP_MODE 		0x8000
#define DIA1_SUP_MODE 		0x4000
#define DDA2_ENABLE 		0x100
#define DDA1_ENABLE 		0x80
#define DIA2_ENABLE 		0x40
#define DIA1_ENABLE 		0x20
#define DSINGLE_STEP 		0x10
#define DDV_TYPE_MASK 		0xc
#define DDV_TYPE_LOAD 		0x0
#define DDV_TYPE_STORE 		0x4
#define DDV_TYPE_ACCESS 	0x8
#define DDV_TYPE_ALWAYS		0xc
#define DDV_COND		0x2
#define DDV_MASK		0x1

int
sparclite_insert_watchpoint(addr, len, type)
    CORE_ADDR addr;
    int len;
    int type;
{
CORE_ADDR dcr;

  dcr = read_register (DCR_REGNUM);

  if (!(dcr & DDA1_ENABLE))
  {
    write_register (DDA1_REGNUM, addr);
    dcr &= ~(DDA1_ASI_MASK | DDV_TYPE_MASK);
    dcr |= (DDA1_SUP_ASI | DDA1_ENABLE);
    if (type == 1)
    {
      write_register (DDV1_REGNUM, 0);
      write_register (DDV2_REGNUM, 0xffffffff);
      dcr |= (DDV_TYPE_LOAD & (~DDV_COND & ~DDV_MASK));
    }   
    else if (type == 0)
    {
      write_register (DDV1_REGNUM, 0);
      write_register (DDV2_REGNUM, 0xffffffff);
      dcr |= (DDV_TYPE_STORE & (~DDV_COND & ~DDV_MASK));
    }
    else
    {
      write_register (DDV1_REGNUM, 0);
      write_register (DDV2_REGNUM, 0xffffffff);
      dcr |= (DDV_TYPE_ACCESS);
    }
    write_register (DCR_REGNUM, dcr);
  }
  else if (!(dcr & DDA2_ENABLE))
  {
    write_register (DDA2_REGNUM, addr);
    dcr &= ~(DDA2_ASI_MASK & DDV_TYPE_MASK);
    dcr |= (DDA2_SUP_ASI | DDA2_ENABLE);
    if (type == 1)
    {
      write_register (DDV1_REGNUM, 0);
      write_register (DDV2_REGNUM, 0xffffffff);
      dcr |= (DDV_TYPE_LOAD & ~DDV_COND & ~DDV_MASK);
    }
    else if (type == 0)
    {
      write_register (DDV1_REGNUM, 0);
      write_register (DDV2_REGNUM, 0xffffffff);
      dcr |= (DDV_TYPE_STORE & ~DDV_COND & ~DDV_MASK);
    }
    else
    {
      write_register (DDV1_REGNUM, 0);
      write_register (DDV2_REGNUM, 0xffffffff);
      dcr |= (DDV_TYPE_ACCESS);
    }
    write_register (DCR_REGNUM, dcr);
  }
  else
    return -1;

  return 0;
} 

int
sparclite_remove_watchpoint(addr, len, type)
    CORE_ADDR addr;
    int len;
    int type;
{
CORE_ADDR dcr, dda1, dda2;

  dcr = read_register (DCR_REGNUM);
  dda1 = read_register (DDA1_REGNUM);
  dda2 = read_register (DDA2_REGNUM);

  if ((dcr & DDA1_ENABLE) && addr == dda1) {
    write_register (DCR_REGNUM, (dcr & ~DDA1_ENABLE));
  }
  else if ((dcr & DDA2_ENABLE) && addr == dda2) {
    write_register (DCR_REGNUM, (dcr & ~DDA2_ENABLE));
  }
  else
    return -1;

  return 0;
}

int
sparclite_insert_hw_breakpoint(addr, len)
    CORE_ADDR addr;
    int len;
{
CORE_ADDR dcr;

  dcr = read_register (DCR_REGNUM);
  
  if (!(dcr & DIA1_ENABLE)) {
    write_register (DIA1_REGNUM, addr);
    write_register (DCR_REGNUM, (dcr | DIA1_ENABLE | DIA1_SUP_MODE));
  }
  else if (!(dcr & DIA2_ENABLE)) {
    write_register (DIA2_REGNUM, addr);
    write_register (DCR_REGNUM, (dcr | DIA2_ENABLE | DIA2_SUP_MODE));
  }
  else
    return -1;

  return 0;
}

int
sparclite_remove_hw_breakpoint(addr, shadow)
    CORE_ADDR addr;
    int shadow;
{
CORE_ADDR dcr, dia1, dia2;

  dcr = read_register (DCR_REGNUM);
  dia1 = read_register (DIA1_REGNUM);
  dia2 = read_register (DIA2_REGNUM);
  
  if ((dcr & DIA1_ENABLE) && addr == dia1) {
    write_register (DCR_REGNUM, (dcr & ~DIA1_ENABLE));
  }
  else if ((dcr & DIA2_ENABLE) && addr == dia2) {
    write_register (DCR_REGNUM, (dcr & ~DIA2_ENABLE));
  }
  else
    return -1;
   
  return 0;
}

int
sparclite_check_watch_resources(type, cnt, ot)
    int type;
    int cnt;
    int ot;
{
  if (type == bp_hardware_breakpoint) {
    if (TARGET_HW_BREAK_LIMIT == 0) return 0;
    else if (cnt <= TARGET_HW_BREAK_LIMIT) return 1;
  }
  else {
    if (TARGET_HW_WATCH_LIMIT == 0) return 0;
    else if (ot) return -1;
    else if (cnt <= TARGET_HW_WATCH_LIMIT) return 1;
  }
  return -1;
}

CORE_ADDR
sparclite_stopped_data_address()
{
  CORE_ADDR dsr, dda1, dda2;

  dsr = read_register (DSR_REGNUM);
  dda1 = read_register (DDA1_REGNUM);
  dda2 = read_register (DDA2_REGNUM);

  if (dsr & 0x10) return dda1;
  else if (dsr & 0x20) return dda2;
  else return 0;
}
