/* TIc80 Simulator.
   Copyright (C) 1997 Free Software Foundation, Inc.
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



struct _sim_cpu {
  unsigned32 reg[32];
  unsigned64 acc[4];
  sim_cia cia;
  sim_cpu_base base;
};

#define GPR(N) ((CPU)->reg[N])
#define ACC(N) ((CPU)->acc[N])

#define SP_FPR(N) (GPR(N))
#define DP_FPR(N) (*(unsigned64*)&GPR(N))
#define vSP_FPR(N) (N == 0 ? (unsigned32)0 : GPR(N))
#define vDP_FPR(N) (*(unsigned64*)&GPR(N))
