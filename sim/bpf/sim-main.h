/* eBPF simulator main header
   Copyright (C) 2020-2023 Free Software Foundation, Inc.

   This file is part of GDB, the GNU debugger.

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
#include "opcodes/bpf-desc.h"
#include "opcodes/bpf-opc.h"
#include "arch.h"
#include "sim-base.h"
#include "cgen-sim.h"
#include "bpf-sim.h"
#include "bpf-helpers.h"

struct bpf_sim_cpu
{
  /* CPU-model specific parts go here.
     Note that in files that don't need to access these pieces WANT_CPU_FOO
     won't be defined and thus these parts won't appear.  This is ok in the
     sense that things work.  It is a source of bugs though.
     One has to of course be careful to not take the size of this
     struct and no structure members accessed in non-cpu specific files can
     go after here.  */
#if defined (WANT_CPU_BPFBF)
  BPFBF_CPU_DATA cpu_data;
#endif
};
#define BPF_SIM_CPU(cpu) ((struct bpf_sim_cpu *) CPU_ARCH_DATA (cpu))

#endif /* ! SIM_MAIN_H */
