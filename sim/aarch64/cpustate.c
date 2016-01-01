/* cpustate.h -- Prototypes for AArch64 simulator functions.

   Copyright (C) 2015-2016 Free Software Foundation, Inc.

   Contributed by Red Hat.

   This file is part of GDB.

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

#include <stdio.h>

#include "sim-main.h"
#include "cpustate.h"
#include "simulator.h"

/* Some operands are allowed to access the stack pointer (reg 31).
   For others a read from r31 always returns 0, and a write to r31 is ignored.  */
#define reg_num(reg) (((reg) == R31 && !r31_is_sp) ? 32 : (reg))

void
aarch64_set_reg_u64 (sim_cpu *cpu, GReg reg, int r31_is_sp, uint64_t val)
{
  if (reg == R31 && ! r31_is_sp)
    {
      TRACE_REGISTER (cpu, "  GR[31] NOT CHANGED!");
      return;
    }

  if (val != cpu->gr[reg].u64)
    TRACE_REGISTER (cpu,
		    "  GR[%2d] changes from %16" PRIx64 " to %16" PRIx64,
		    reg, cpu->gr[reg].u64, val);

  cpu->gr[reg].u64 = val;
}

void
aarch64_set_reg_s64 (sim_cpu *cpu, GReg reg, int r31_is_sp, int64_t val)
{
  if (reg == R31 && ! r31_is_sp)
    {
      TRACE_REGISTER (cpu, "  GR[31] NOT CHANGED!");
      return;
    }

  if (val != cpu->gr[reg].s64)
    TRACE_REGISTER (cpu,
		    "  GR[%2d] changes from %16" PRIx64 " to %16" PRIx64,
		    reg, cpu->gr[reg].s64, val);

  cpu->gr[reg].s64 = val;
}

uint64_t
aarch64_get_reg_u64 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].u64;
}

int64_t
aarch64_get_reg_s64 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].s64;
}

uint32_t
aarch64_get_reg_u32 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].u32;
}

int32_t
aarch64_get_reg_s32 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].s32;
}

uint32_t
aarch64_get_reg_u16 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].u16;
}

int32_t
aarch64_get_reg_s16 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].s16;
}

uint32_t
aarch64_get_reg_u8 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].u8;
}

int32_t
aarch64_get_reg_s8 (sim_cpu *cpu, GReg reg, int r31_is_sp)
{
  return cpu->gr[reg_num(reg)].s8;
}

uint64_t
aarch64_get_PC (sim_cpu *cpu)
{
  return cpu->pc;
}

uint64_t
aarch64_get_next_PC (sim_cpu *cpu)
{
  return cpu->nextpc;
}

void
aarch64_set_next_PC (sim_cpu *cpu, uint64_t next)
{
  if (next != cpu->nextpc + 4)
    TRACE_REGISTER (cpu,
		    "  NextPC changes from %16" PRIx64 " to %16" PRIx64,
		    cpu->nextpc, next);

  cpu->nextpc = next;
}

void
aarch64_set_next_PC_by_offset (sim_cpu *cpu, int64_t offset)
{
  if (cpu->pc + offset != cpu->nextpc + 4)
    TRACE_REGISTER (cpu,
		    "  NextPC changes from %16" PRIx64 " to %16" PRIx64,
		    cpu->nextpc, cpu->pc + offset);

  cpu->nextpc = cpu->pc + offset;
}

/* Install nextpc as current pc.  */
void
aarch64_update_PC (sim_cpu *cpu)
{
  cpu->pc = cpu->nextpc;
  /* Rezero the register we hand out when asked for ZR just in case it
     was used as the destination for a write by the previous
     instruction.  */
  cpu->gr[32].u64 = 0UL;
}

/* This instruction can be used to save the next PC to LR
   just before installing a branch PC.  */
void
aarch64_save_LR (sim_cpu *cpu)
{
  if (cpu->gr[LR].u64 != cpu->nextpc)
    TRACE_REGISTER (cpu,
		    "  LR    changes from %16" PRIx64 " to %16" PRIx64,
		    cpu->gr[LR].u64, cpu->nextpc);

  cpu->gr[LR].u64 = cpu->nextpc;
}

static const char *
decode_cpsr (FlagMask flags)
{
  switch (flags & CPSR_ALL_FLAGS)
    {
    default:
    case 0:  return "----";
    case 1:  return "---V";
    case 2:  return "--C-";
    case 3:  return "--CV";
    case 4:  return "-Z--";
    case 5:  return "-Z-V";
    case 6:  return "-ZC-";
    case 7:  return "-ZCV";
    case 8:  return "N---";
    case 9:  return "N--V";
    case 10: return "N-C-";
    case 11: return "N-CV";
    case 12: return "NZ--";
    case 13: return "NZ-V";
    case 14: return "NZC-";
    case 15: return "NZCV";
    }
}

/* Retrieve the CPSR register as an int.  */
uint32_t
aarch64_get_CPSR (sim_cpu *cpu)
{
  return cpu->CPSR;
}

/* Set the CPSR register as an int.  */
void
aarch64_set_CPSR (sim_cpu *cpu, uint32_t new_flags)
{
  if (TRACE_REGISTER_P (cpu))
    {
      if (cpu->CPSR != new_flags)
	TRACE_REGISTER (cpu,
			"  CPSR changes from %s to %s",
			decode_cpsr (cpu->CPSR), decode_cpsr (new_flags));
      else
	TRACE_REGISTER (cpu,
			"  CPSR stays at %s", decode_cpsr (cpu->CPSR));
    }

  cpu->CPSR = new_flags & CPSR_ALL_FLAGS;
}

/* Read a specific subset of the CPSR as a bit pattern.  */
uint32_t
aarch64_get_CPSR_bits (sim_cpu *cpu, FlagMask mask)
{
  return cpu->CPSR & mask;
}

/* Assign a specific subset of the CPSR as a bit pattern.  */
void
aarch64_set_CPSR_bits (sim_cpu *cpu, uint32_t mask, uint32_t value)
{
  uint32_t old_flags = cpu->CPSR;

  mask &= CPSR_ALL_FLAGS;
  cpu->CPSR &= ~ mask;
  cpu->CPSR |= (value & mask);

  if (old_flags != cpu->CPSR)
    TRACE_REGISTER (cpu,
		    "  CPSR changes from %s to %s",
		    decode_cpsr (old_flags), decode_cpsr (cpu->CPSR));
}

/* Test the value of a single CPSR returned as non-zero or zero.  */
uint32_t
aarch64_test_CPSR_bit (sim_cpu *cpu, FlagMask bit)
{
  return cpu->CPSR & bit;
}

/* Set a single flag in the CPSR.  */
void
aarch64_set_CPSR_bit (sim_cpu *cpu, FlagMask bit)
{
  uint32_t old_flags = cpu->CPSR;

  cpu->CPSR |= (bit & CPSR_ALL_FLAGS);

  if (old_flags != cpu->CPSR)
    TRACE_REGISTER (cpu,
		    "  CPSR changes from %s to %s",
		    decode_cpsr (old_flags), decode_cpsr (cpu->CPSR));
}

/* Clear a single flag in the CPSR.  */
void
aarch64_clear_CPSR_bit (sim_cpu *cpu, FlagMask bit)
{
  uint32_t old_flags = cpu->CPSR;

  cpu->CPSR &= ~(bit & CPSR_ALL_FLAGS);

  if (old_flags != cpu->CPSR)
    TRACE_REGISTER (cpu,
		    "  CPSR changes from %s to %s",
		    decode_cpsr (old_flags), decode_cpsr (cpu->CPSR));
}

float
aarch64_get_FP_float (sim_cpu *cpu, VReg reg)
{
  return cpu->fr[reg].s;
}

double
aarch64_get_FP_double (sim_cpu *cpu, VReg reg)
{
  return cpu->fr[reg].d;
}

void
aarch64_get_FP_long_double (sim_cpu *cpu, VReg reg, FRegister *a)
{
  a->v[0] = cpu->fr[reg].v[0];
  a->v[1] = cpu->fr[reg].v[1];
}

void
aarch64_set_FP_float (sim_cpu *cpu, VReg reg, float val)
{
  if (val != cpu->fr[reg].s)
    TRACE_REGISTER (cpu,
		    "  FR[%d] changes from %f to %f",
		    reg, cpu->fr[reg].s, val);

  cpu->fr[reg].s = val;
}

void
aarch64_set_FP_double (sim_cpu *cpu, VReg reg, double val)
{
  if (val != cpu->fr[reg].d)
    TRACE_REGISTER (cpu,
		    "  FR[%d] changes from %f to %f",
		    reg, cpu->fr[reg].d, val);

  cpu->fr[reg].d = val;
}

void
aarch64_set_FP_long_double (sim_cpu *cpu, VReg reg, FRegister a)
{
  if (cpu->fr[reg].v[0] != a.v[0]
      || cpu->fr[reg].v[1] != a.v[1])
    TRACE_REGISTER (cpu,
		    "  FR[%d] changes from [%0lx %0lx] to [%lx %lx] ",
		    reg,
		    cpu->fr[reg].v[0], cpu->fr[reg].v[1],
		    a.v[0], a.v[1]);

  cpu->fr[reg].v[0] = a.v[0];
  cpu->fr[reg].v[1] = a.v[1];
}

uint64_t
aarch64_get_vec_u64 (sim_cpu *cpu, VReg reg, unsigned element)
{
  return cpu->fr[reg].v[element];
}

uint32_t
aarch64_get_vec_u32 (sim_cpu *cpu, VReg regno, unsigned element)
{
  return cpu->fr[regno].w[element];
}

uint16_t
aarch64_get_vec_u16 (sim_cpu *cpu, VReg regno, unsigned element)
{
  return cpu->fr[regno].h[element];
}

uint8_t
aarch64_get_vec_u8 (sim_cpu *cpu, VReg regno, unsigned element)
{
  return cpu->fr[regno].b[element];
}

void
aarch64_set_vec_u64 (sim_cpu *  cpu,
		     VReg       regno,
		     unsigned   element,
		     uint64_t   value)
{
  if (value != cpu->fr[regno].v[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<long>[%d] changes from %16" PRIx64
		    " to %16" PRIx64,
		    regno, element, cpu->fr[regno].v[element], value);

  cpu->fr[regno].v[element] = value;
}

void
aarch64_set_vec_u32 (sim_cpu *  cpu,
		     VReg       regno,
		     unsigned   element,
		     uint32_t   value)
{
  if (value != cpu->fr[regno].w[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<word>[%d] changes from %8x to %8x",
		    regno, element, cpu->fr[regno].w[element], value);

  cpu->fr[regno].w[element] = value;
}

void
aarch64_set_vec_u16 (sim_cpu *  cpu,
		     VReg       regno,
		     unsigned   element,
		     uint16_t   value)
{
  if (value != cpu->fr[regno].h[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<half>[%d] changes from %4x to %4x",
		    regno, element, cpu->fr[regno].h[element], value);

  cpu->fr[regno].h[element] = value;
}

void
aarch64_set_vec_u8 (sim_cpu *cpu, VReg regno, unsigned element, uint8_t value)
{
  if (value != cpu->fr[regno].b[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<byte>[%d] changes from %x to %x",
		    regno, element, cpu->fr[regno].b[element], value);

  cpu->fr[regno].b[element] = value;
}

void
aarch64_set_FPSR (sim_cpu *cpu, uint32_t value)
{
  if (cpu->FPSR != value)
    TRACE_REGISTER (cpu,
		    "  FPSR changes from %x to %x", cpu->FPSR, value);

  cpu->FPSR = value & FPSR_ALL_FPSRS;
}

uint32_t
aarch64_get_FPSR (sim_cpu *cpu)
{
  return cpu->FPSR;
}

void
aarch64_set_FPSR_bits (sim_cpu *cpu, uint32_t mask, uint32_t value)
{
  uint32_t old_FPSR = cpu->FPSR;

  mask &= FPSR_ALL_FPSRS;
  cpu->FPSR &= ~mask;
  cpu->FPSR |= (value & mask);

  if (cpu->FPSR != old_FPSR)
    TRACE_REGISTER (cpu,
		    "  FPSR changes from %x to %x", old_FPSR, cpu->FPSR);
}

uint32_t
aarch64_get_FPSR_bits (sim_cpu *cpu, uint32_t mask)
{
  mask &= FPSR_ALL_FPSRS;
  return cpu->FPSR & mask;
}

int
aarch64_test_FPSR_bit (sim_cpu *cpu, FPSRMask flag)
{
  return cpu->FPSR & flag;
}

float
aarch64_get_vec_float (sim_cpu *cpu, VReg v, unsigned e)
{
  return cpu->fr[v].S[e];
}

double
aarch64_get_vec_double (sim_cpu *cpu, VReg v, unsigned e)
{
  return cpu->fr[v].D[e];
}

void
aarch64_set_vec_float (sim_cpu *cpu, VReg v, unsigned e, float f)
{
  if (f != cpu->fr[v].S[e])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<float>[%d] changes from %f to %f",
		    v, e, cpu->fr[v].S[e], f);

  cpu->fr[v].S[e] = f;
}

void
aarch64_set_vec_double (sim_cpu *cpu, VReg v, unsigned e, double d)
{
  if (d != cpu->fr[v].D[e])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<double>[%d] changes from %f to %f",
		    v, e, cpu->fr[v].D[e], d);

  cpu->fr[v].D[e] = d;
}

int64_t
aarch64_get_vec_s64 (sim_cpu *cpu, VReg regno, unsigned element)
{
  return cpu->fr[regno].V[element];
}

int32_t
aarch64_get_vec_s32 (sim_cpu *cpu, VReg regno, unsigned element)
{
  return cpu->fr[regno].W[element];
}

int16_t
aarch64_get_vec_s16 (sim_cpu *cpu, VReg regno, unsigned element)
{
  return cpu->fr[regno].H[element];
}

int8_t
aarch64_get_vec_s8 (sim_cpu *cpu, VReg regno, unsigned element)
{
  return cpu->fr[regno].B[element];
}

void
aarch64_set_vec_s64 (sim_cpu *cpu, VReg regno, unsigned element, int64_t value)
{
  if (value != cpu->fr[regno].V[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<long>[%d] changes from %16" PRIx64 " to %16" PRIx64,
		    regno, element, cpu->fr[regno].V[element], value);

  cpu->fr[regno].V[element] = value;
}

void
aarch64_set_vec_s32 (sim_cpu *cpu, VReg regno, unsigned element, int32_t value)
{
  if (value != cpu->fr[regno].W[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<word>[%d] changes from %8x to %8x",
		    regno, element, cpu->fr[regno].W[element], value);

  cpu->fr[regno].W[element] = value;
}

void
aarch64_set_vec_s16 (sim_cpu *cpu, VReg regno, unsigned element, int16_t value)
{
  if (value != cpu->fr[regno].H[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<half>[%d] changes from %4x to %4x",
		    regno, element, cpu->fr[regno].H[element], value);

  cpu->fr[regno].H[element] = value;
}

void
aarch64_set_vec_s8 (sim_cpu *cpu, VReg regno, unsigned element, int8_t value)
{
  if (value != cpu->fr[regno].B[element])
    TRACE_REGISTER (cpu,
		    "  VR[%2d].<byte>[%d] changes from %x to %x",
		    regno, element, cpu->fr[regno].B[element], value);

  cpu->fr[regno].B[element] = value;
}
