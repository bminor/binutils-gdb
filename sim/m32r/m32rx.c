/* m32rx simulator support code
   Copyright (C) 1997, 1998 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

This file is part of GDB, the GNU debugger.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#define WANT_CPU
#define WANT_CPU_M32RX

#include "sim-main.h"
#include <signal.h>
#include "libiberty.h"
#include "bfd.h"
/* FIXME: need to provide general mechanism for accessing target files 
   these.  For now this is a hack to avoid getting the host version.  */
#include "../../libgloss/m32r/sys/syscall.h"
#include "targ-vals.h"

/* The contents of BUF are in target byte order.  */

void
m32rx_fetch_register (sd, rn, buf)
     SIM_DESC sd;
     int rn;
     unsigned char *buf;
{
  SIM_CPU *current_cpu = STATE_CPU (sd, 0);

  if (rn < 16)
    SETTWI (buf, GET_H_GR (rn));
  else if (rn < 21)
    SETTWI (buf, GET_H_CR (rn - 16));
  else switch (rn) {
    case PC_REGNUM:
      SETTWI (buf, GET_H_PC ());
      break;
    case ACCL_REGNUM:
      SETTWI (buf, GETLODI (GET_H_ACCUM ()));
      break;
    case ACCH_REGNUM:
      SETTWI (buf, GETHIDI (GET_H_ACCUM ()));
      break;
#if 0
    case 23: *reg = STATE_CPU_CPU (sd, 0)->h_cond;		break;
    case 24: *reg = STATE_CPU_CPU (sd, 0)->h_sm;		break;
    case 25: *reg = STATE_CPU_CPU (sd, 0)->h_bsm;		break;
    case 26: *reg = STATE_CPU_CPU (sd, 0)->h_ie;		break;
    case 27: *reg = STATE_CPU_CPU (sd, 0)->h_bie;		break;
    case 28: *reg = STATE_CPU_CPU (sd, 0)->h_bcarry;		break; /* rename: bc */
    case 29: memcpy (buf, &STATE_CPU_CPU (sd, 0)->h_bpc, sizeof(WI));	break; /* duplicate */
#endif
    default: abort ();
  }
}
 
/* The contents of BUF are in target byte order.  */

void
m32rx_store_register (sd, rn, buf)
     SIM_DESC sd;
     int rn;
     unsigned char *buf;
{
  SIM_CPU *current_cpu = STATE_CPU (sd, 0);

  if (rn < 16)
    SET_H_GR (rn, GETTWI (buf));
  else if (rn < 21)
    SET_H_CR (rn - 16, GETTWI (buf));
  else switch (rn) {
    case PC_REGNUM:
      SET_H_PC (GETTWI (buf));
      break;
    case ACCL_REGNUM:
      SETLODI (CPU (h_accum), GETTWI (buf));
      break;
    case ACCH_REGNUM:
      SETHIDI (CPU (h_accum), GETTWI (buf));
      break;
#if 0
    case 23: STATE_CPU_CPU (sd, 0)->h_cond   = *reg;			break;
    case 24: STATE_CPU_CPU (sd, 0)->h_sm     = *reg;			break;
    case 25: STATE_CPU_CPU (sd, 0)->h_bsm    = *reg;			break;
    case 26: STATE_CPU_CPU (sd, 0)->h_ie     = *reg;			break;
    case 27: STATE_CPU_CPU (sd, 0)->h_bie    = *reg;			break;
    case 28: STATE_CPU_CPU (sd, 0)->h_bcarry = *reg;			break; /* rename: bc */
    case 29: memcpy (&STATE_CPU_CPU (sd, 0)->h_bpc, buf, sizeof(DI));	break; /* duplicate */
#endif
  }
}

/* Cover fn to access h-accums.  */

UDI
m32rx_h_accums_get (SIM_CPU *current_cpu, UINT accum)
{
  return 0;
}
