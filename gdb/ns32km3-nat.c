/* OBSOLETE /* Low level interface to ns532 running mach 3.0. */
/* OBSOLETE   Copyright 1992, 1993, 1998, 2000, 2001 Free Software Foundation, Inc. */
/* OBSOLETE */
/* OBSOLETE   This file is part of GDB. */
/* OBSOLETE */
/* OBSOLETE   This program is free software; you can redistribute it and/or modify */
/* OBSOLETE   it under the terms of the GNU General Public License as published by */
/* OBSOLETE   the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE   (at your option) any later version. */
/* OBSOLETE */
/* OBSOLETE   This program is distributed in the hope that it will be useful, */
/* OBSOLETE   but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE   GNU General Public License for more details. */
/* OBSOLETE */
/* OBSOLETE   You should have received a copy of the GNU General Public License */
/* OBSOLETE   along with this program; if not, write to the Free Software */
/* OBSOLETE   Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE   Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE */
/* OBSOLETE#include "defs.h" */
/* OBSOLETE#include "inferior.h" */
/* OBSOLETE#include "regcache.h" */
/* OBSOLETE */
/* OBSOLETE#include <stdio.h> */
/* OBSOLETE */
/* OBSOLETE#include <mach.h> */
/* OBSOLETE#include <mach/message.h> */
/* OBSOLETE#include <mach/exception.h> */
/* OBSOLETE#include <mach_error.h> */
/* OBSOLETE */
/* OBSOLETE#define private static */
/* OBSOLETE */
/* OBSOLETE */
/* OBSOLETE/* Find offsets to thread states at compile time. */
/* OBSOLETE * If your compiler does not grok this, calculate offsets */
/* OBSOLETE * offsets yourself and use them (or get a compatible compiler :-) */
/* OBSOLETE */ */
/* OBSOLETE */
/* OBSOLETE#define  REG_N_OFFSET(reg) (int)(&((struct ns532_combined_state *)0)->ts.reg) */
/* OBSOLETE#define  REG_F_OFFSET(reg) (int)(&((struct ns532_combined_state *)0)->fs.reg) */
/* OBSOLETE */
/* OBSOLETE/* at reg_offset[i] is the offset to the ns532_combined_state */
/* OBSOLETE * location where the gdb registers[i] is stored. */
/* OBSOLETE */ */
/* OBSOLETE */
/* OBSOLETEstatic int reg_offset[] = */
/* OBSOLETE{ */
/* OBSOLETE  REG_N_OFFSET (r0), REG_N_OFFSET (r1), REG_N_OFFSET (r2), REG_N_OFFSET (r3), */
/* OBSOLETE  REG_N_OFFSET (r4), REG_N_OFFSET (r5), REG_N_OFFSET (r6), REG_N_OFFSET (r7), */
/* OBSOLETE  REG_F_OFFSET (l0a), REG_F_OFFSET (l0b), REG_F_OFFSET (l2a), REG_F_OFFSET (l2b), */
/* OBSOLETE  REG_F_OFFSET (l4a), REG_F_OFFSET (l4b), REG_F_OFFSET (l6a), REG_F_OFFSET (l6b), */
/* OBSOLETEREG_N_OFFSET (sp), REG_N_OFFSET (fp), REG_N_OFFSET (pc), REG_N_OFFSET (psr), */
/* OBSOLETE  REG_F_OFFSET (fsr), */
/* OBSOLETE  REG_F_OFFSET (l0a), REG_F_OFFSET (l1a), REG_F_OFFSET (l2a), REG_F_OFFSET (l3a), */
/* OBSOLETE  REG_F_OFFSET (l4a), REG_F_OFFSET (l5a), REG_F_OFFSET (l6a), REG_F_OFFSET (l7a), */
/* OBSOLETE}; */
/* OBSOLETE */
/* OBSOLETE#define REG_ADDRESS(state,regnum) ((char *)(state)+reg_offset[regnum]) */
/* OBSOLETE */
/* OBSOLETE/* Fetch COUNT contiguous registers from thread STATE starting from REGNUM */
/* OBSOLETE * Caller knows that the regs handled in one transaction are of same size. */
/* OBSOLETE */ */
/* OBSOLETE#define FETCH_REGS(state, regnum, count) \ */
/* OBSOLETE  memcpy (&registers[REGISTER_BYTE (regnum)], \ */
/* OBSOLETE	  (char *)state+reg_offset[ regnum ], \ */
/* OBSOLETE	  count*REGISTER_SIZE) */
/* OBSOLETE */
/* OBSOLETE/* Store COUNT contiguous registers to thread STATE starting from REGNUM */ */
/* OBSOLETE#define STORE_REGS(state, regnum, count) \ */
/* OBSOLETE  memcpy ((char *)state+reg_offset[ regnum ], \ */
/* OBSOLETE	  &registers[REGISTER_BYTE (regnum)], \ */
/* OBSOLETE	  count*REGISTER_SIZE) */
/* OBSOLETE */
/* OBSOLETE/* */
/* OBSOLETE * Fetch inferiors registers for gdb. */
/* OBSOLETE * REGNO specifies which (as gdb views it) register, -1 for all. */
/* OBSOLETE */ */
/* OBSOLETE */
/* OBSOLETEvoid */
/* OBSOLETEfetch_inferior_registers (int regno) */
/* OBSOLETE{ */
/* OBSOLETE  kern_return_t ret; */
/* OBSOLETE  thread_state_data_t state; */
/* OBSOLETE  unsigned int stateCnt = NS532_COMBINED_STATE_COUNT; */
/* OBSOLETE  int index; */
/* OBSOLETE */
/* OBSOLETE  if (!MACH_PORT_VALID (current_thread)) */
/* OBSOLETE    error ("fetch inferior registers: Invalid thread"); */
/* OBSOLETE */
/* OBSOLETE  if (must_suspend_thread) */
/* OBSOLETE    setup_thread (current_thread, 1); */
/* OBSOLETE */
/* OBSOLETE  ret = thread_get_state (current_thread, */
/* OBSOLETE			  NS532_COMBINED_STATE, */
/* OBSOLETE			  state, */
/* OBSOLETE			  &stateCnt); */
/* OBSOLETE */
/* OBSOLETE  if (ret != KERN_SUCCESS) */
/* OBSOLETE    warning ("fetch_inferior_registers: %s ", */
/* OBSOLETE	     mach_error_string (ret)); */
/* OBSOLETE#if 0 */
/* OBSOLETE  /* It may be more effective to store validate all of them, */
/* OBSOLETE   * since we fetched them all anyway */
/* OBSOLETE   */ */
/* OBSOLETE  else if (regno != -1) */
/* OBSOLETE    supply_register (regno, (char *) state + reg_offset[regno]); */
/* OBSOLETE#endif */
/* OBSOLETE  else */
/* OBSOLETE    { */
/* OBSOLETE      for (index = 0; index < NUM_REGS; index++) */
/* OBSOLETE	supply_register (index, (char *) state + reg_offset[index]); */
/* OBSOLETE    } */
/* OBSOLETE */
/* OBSOLETE  if (must_suspend_thread) */
/* OBSOLETE    setup_thread (current_thread, 0); */
/* OBSOLETE} */
/* OBSOLETE */
/* OBSOLETE/* Store our register values back into the inferior. */
/* OBSOLETE * If REGNO is -1, do this for all registers. */
/* OBSOLETE * Otherwise, REGNO specifies which register */
/* OBSOLETE * */
/* OBSOLETE * On mach3 all registers are always saved in one call. */
/* OBSOLETE */ */
/* OBSOLETEvoid */
/* OBSOLETEstore_inferior_registers (int regno) */
/* OBSOLETE{ */
/* OBSOLETE  kern_return_t ret; */
/* OBSOLETE  thread_state_data_t state; */
/* OBSOLETE  unsigned int stateCnt = NS532_COMBINED_STATE_COUNT; */
/* OBSOLETE  register int index; */
/* OBSOLETE */
/* OBSOLETE  if (!MACH_PORT_VALID (current_thread)) */
/* OBSOLETE    error ("store inferior registers: Invalid thread"); */
/* OBSOLETE */
/* OBSOLETE  if (must_suspend_thread) */
/* OBSOLETE    setup_thread (current_thread, 1); */
/* OBSOLETE */
/* OBSOLETE  /* Fetch the state of the current thread */ */
/* OBSOLETE  ret = thread_get_state (current_thread, */
/* OBSOLETE			  NS532_COMBINED_STATE, */
/* OBSOLETE			  state, */
/* OBSOLETE			  &stateCnt); */
/* OBSOLETE */
/* OBSOLETE  if (ret != KERN_SUCCESS) */
/* OBSOLETE    { */
/* OBSOLETE      warning ("store_inferior_registers (get): %s", */
/* OBSOLETE	       mach_error_string (ret)); */
/* OBSOLETE      if (must_suspend_thread) */
/* OBSOLETE	setup_thread (current_thread, 0); */
/* OBSOLETE      return; */
/* OBSOLETE    } */
/* OBSOLETE */
/* OBSOLETE  /* move gdb's registers to thread's state */
/* OBSOLETE */
/* OBSOLETE   * Since we save all registers anyway, save the ones */
/* OBSOLETE   * that gdb thinks are valid (e.g. ignore the regno */
/* OBSOLETE   * parameter) */
/* OBSOLETE   */ */
/* OBSOLETE#if 0 */
/* OBSOLETE  if (regno != -1) */
/* OBSOLETE    STORE_REGS (state, regno, 1); */
/* OBSOLETE  else */
/* OBSOLETE#endif */
/* OBSOLETE    { */
/* OBSOLETE      for (index = 0; index < NUM_REGS; index++) */
/* OBSOLETE	STORE_REGS (state, index, 1); */
/* OBSOLETE    } */
/* OBSOLETE */
/* OBSOLETE  /* Write gdb's current view of register to the thread */
/* OBSOLETE   */ */
/* OBSOLETE  ret = thread_set_state (current_thread, */
/* OBSOLETE			  NS532_COMBINED_STATE, */
/* OBSOLETE			  state, */
/* OBSOLETE			  NS532_COMBINED_STATE_COUNT); */
/* OBSOLETE */
/* OBSOLETE  if (ret != KERN_SUCCESS) */
/* OBSOLETE    warning ("store_inferior_registers (set): %s", */
/* OBSOLETE	     mach_error_string (ret)); */
/* OBSOLETE */
/* OBSOLETE  if (must_suspend_thread) */
/* OBSOLETE    setup_thread (current_thread, 0); */
/* OBSOLETE} */
