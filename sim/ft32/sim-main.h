/* Simulator for FTDI FT32 processor.

   Copyright (C) 2009-2015 Free Software Foundation, Inc.
   Contributed by FTDI <support@ftdichip.com>

   This file is part of simulators.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SIM_MAIN_H
#define SIM_MAIN_H

#include "sim-basics.h"

typedef address_word sim_cia;

#include "sim-base.h"
#include "bfd.h"

typedef struct _sim_cpu SIM_CPU;

#include "ft32-sim.h"

struct _sim_cpu {

  /* The following are internal simulator state variables: */
#define CIA_GET(CPU) ((CPU)->state.pc + 0)
#define CIA_SET(CPU,CIA) ((CPU)->state.pc = (CIA))

  struct ft32_cpu_state state;

  sim_cpu_base base;
};

struct sim_state {

  sim_cpu *cpu[MAX_NR_PROCESSORS];
#if (WITH_SMP)
#define STATE_CPU(sd,n) ((sd)->cpu[n])
#else
#define STATE_CPU(sd,n) ((sd)->cpu[0])
#endif

  sim_state_base base;
};

#endif
