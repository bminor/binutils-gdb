/* sparc32 simulator support code
   Copyright (C) 1999 Cygnus Solutions.  */

#define WANT_CPU sparc32
#define WANT_CPU_SPARC32

#include "sim-main.h"
#include <signal.h>
#include "libiberty.h"
#include "bfd.h"
#include "cgen-mem.h"
#include "cgen-ops.h"
#include "targ-vals.h"

static void sparc32_init_regwins (SIM_CPU *current_cpu);
static void sparc32_set_psr_no_cwp (SIM_CPU *current_cpu, USI newval);

/* gdb register access support.
   The contents of BUF are in target byte order.  */

int
sparc32_fetch_register (SIM_CPU *current_cpu, int rn, unsigned char *buf, int len)
{
  if (rn < 32)
    {
      SETTSI (buf, a_sparc_h_gr_get (current_cpu, rn));
    }
  else
    switch (rn)
      {
      case SPARC32_PC_REGNUM :
	SETTSI (buf, a_sparc_h_pc_get (current_cpu));
	break;
      case SPARC32_NPC_REGNUM :
	{
	  USI npc = a_sparc_h_npc_get (current_cpu);
#if 0 /* experiment */
	  if (npc == NPC_NO_DELAY_INSN)
	    npc = a_sparc_h_pc_get (current_cpu) + 4;
#endif
	  SETTSI (buf, npc);
	  break;
	}
      default :
	return 0;
      }

  return -1; /*FIXME*/
}
 
/* gdb register access support.
   The contents of BUF are in target byte order.  */

int
sparc32_store_register (SIM_CPU *current_cpu, int rn, unsigned char *buf, int len)
{
  if (rn < 32)
    {
      a_sparc_h_gr_set (current_cpu, rn, GETTSI (buf));
    }
  else
    switch (rn)
      {
      case SPARC32_PC_REGNUM :
	a_sparc_h_pc_set (current_cpu, GETTSI (buf));
	break;
      case SPARC32_NPC_REGNUM :
	a_sparc_h_npc_set (current_cpu, GETTSI (buf));
	break;
      default :
	return 0;
      }

  return -1; /*FIXME*/
}

/* Initialization.  */

/* Initialize the program counter.  */

void
sparc32_init_pc (SIM_CPU *current_cpu, SI pc, SI npc)
{
  SET_H_PC (pc);
  SET_H_NPC (npc);
}

/* Do a pseudo power-on-reset.
   USERLAND_P is non-zero to prepare to run a user-land program.  */

void
sparc32_cold_reset (SIM_CPU *current_cpu, int userland_p)
{
  int i;

  /* Initialize the PSR.
     This has to be careful as we just want to initialize the CWP, we don't
     want to "set" it (which causes the old window to be "swapped out").
     ??? impl,ver need better values.  */
  sparc32_set_psr_no_cwp (current_cpu, 0 | PSR_S);

  /* Initialize cwp directly (bypassing SET_H_CWP and SET_H_PSR) as we just
     want to initialize things, not "swap" the current window out.  */
  CPU (h_cwp) = 0;
  sparc32_init_regwins (current_cpu);

  /* Mark the last window as invalid.  This creates a distinguishable end
     of register window stack.  The last window is window 1 (mask 2) as
     saves decrement CWP.
     Note that the last and first window overlap.  */
  SET_H_WIM (2);

  sparc32_init_pc (current_cpu, 0, 4);

  for (i = 0; i < 32; ++i)
    SET_H_GR (i, 0);
  SET_H_FSR (0);
}

/* Do a warm reset (the reset trap).  */

void
sparc32_warm_reset (SIM_CPU *current_cpu)
{
  /* FIXME: unimplemented yet */
}

/* Special purpose registers.  */

/* The PSR.
   ??? add ability to specify a register as a set of bitfields.  */

USI
sparc32_get_h_psr_handler (SIM_CPU *current_cpu)
{
  USI val;
  val = CPU (h_psr) & (PSR_IMPL | PSR_VER);
  val |= GET_H_ICC_C () ? PSR_C : 0;
  val |= GET_H_ICC_N () ? PSR_N : 0;
  val |= GET_H_ICC_V () ? PSR_V : 0;
  val |= GET_H_ICC_Z () ? PSR_Z : 0;
  val |= GET_H_EC () ? PSR_EC : 0;
  val |= GET_H_EF () ? PSR_EF : 0;
  val |= (GET_H_PIL () & 0xf) << 8;
  val |= GET_H_S () ? PSR_S : 0;
  val |= GET_H_PS () ? PSR_PS : 0;
  val |= GET_H_ET () ? PSR_ET : 0;
  val |= GET_H_CWP ();
  return val;
}

/* Utility to set everything in the PSR except CWP
   (needed by sparc32_cold_reset).  */

static void
sparc32_set_psr_no_cwp (SIM_CPU *current_cpu, USI newval)
{
  SET_H_ICC_C ((newval & PSR_C) != 0);
  SET_H_ICC_N ((newval & PSR_N) != 0);
  SET_H_ICC_V ((newval & PSR_V) != 0);
  SET_H_ICC_Z ((newval & PSR_Z) != 0);
  SET_H_EC ((newval & PSR_EC) != 0);
  SET_H_EF ((newval & PSR_EF) != 0);
  SET_H_PIL ((newval & PSR_PIL) >> 8);
  SET_H_S ((newval & PSR_S) != 0);
  SET_H_PS ((newval & PSR_PS) != 0);
  SET_H_ET ((newval & PSR_ET) != 0);
}

void
sparc32_set_h_psr_handler (SIM_CPU *current_cpu, USI newval)
{
  sparc32_set_psr_no_cwp (current_cpu, newval);
  SET_H_CWP (newval & PSR_CWP);
}

/* Register window support.  */

/* Allocate space for the register window mechanism.
   This version doesn't have much to do.
   Other register window implementations have more to do.  */

void
sparc32_alloc_regwins (SIM_CPU *current_cpu, int nwindows)
{
  /* nothing to do in current window implementation */
}

void
sparc32_free_regwins (SIM_CPU *current_cpu)
{
  /* nothing to do in current window implementation */
}

/* Initialize the register window control registers for running
   user programs.  */

static void
sparc32_init_regwins (SIM_CPU *current_cpu)
{
  /* nothing to do in current window implementation */
}

/* Assign a new value to CWP.
   SET_H_CWP calls this.

   This swaps out the current values of the o/l/i regs from `current_regs'
   and swaps in the new values.  */

void
sparc32_set_h_cwp_handler (SIM_CPU *current_cpu, int new)
{
  int old = GET_H_CWP ();

  if (new < 0 || new >= GET_NWINDOWS ())
    abort ();

  CPU (h_cwp) = new;

  /* Swap current values out of `current_regs'.
     Do this even if old == new.  */
  sparc32_swapout_regwin (current_cpu, old);

  /* Swap new values into `current_regs'.  */
  if (old != new)
    sparc32_swapin_regwin (current_cpu, new);
}

/* Swap out the values in `current_regs'.  */

void
sparc32_swapout_regwin (SIM_CPU *current_cpu, int win)
{
  int n = 8 * sizeof (SI);

  memcpy (REAL_OREGS (current_cpu, win), CURRENT_OREGS (current_cpu), n);
  memcpy (REAL_LREGS (current_cpu, win), CURRENT_LREGS (current_cpu), n);
  memcpy (REAL_IREGS (current_cpu, win), CURRENT_IREGS (current_cpu), n);
}

/* Swap int values for `current_regs'.  */

void
sparc32_swapin_regwin (SIM_CPU *current_cpu, int win)
{
  int n = 8 * sizeof (SI);

  memcpy (CURRENT_OREGS (current_cpu), REAL_OREGS (current_cpu, win), n);
  memcpy (CURRENT_LREGS (current_cpu), REAL_LREGS (current_cpu, win), n);
  memcpy (CURRENT_IREGS (current_cpu), REAL_IREGS (current_cpu, win), n);
}

/* Create a new window.  We assume there is room.  */

void
sparc32_save_regwin (SIM_CPU *current_cpu)
{
  SET_H_CWP (NEXT_WIN (GET_H_CWP ()));
}

/* Pop a window.  We assume no traps possible.  */

void
sparc32_restore_regwin (SIM_CPU *current_cpu)
{
  SET_H_CWP (PREV_WIN (GET_H_CWP ()));
}

/* Flush the register windows to memory.
   This is necessary, for example, when we want to walk the stack in gdb.
   NO_ERRORS_P is non-zero if memory faults must be avoided.  This is important
   when returning to gdb, the processor has "stopped".

   ??? At present we only handle user programs.  */

void
sparc32_flush_regwins (SIM_CPU *current_cpu, IADDR pc, int no_errors_p)
{
  int i, win;
  int count = GET_NWINDOWS ();

  /* Flush the current window cache.  */
  sparc32_swapout_regwin (current_cpu, GET_H_CWP ());

  /* For each register window that is marked valid, flush it to memory.
     We start at the current window and move upwards on the stack.  */

  for (i = 0, win = GET_H_CWP (); i < count; i++, win = PREV_WIN (win))
    {
      /* Don't go passed an invalid window.  */
      if (! WINDOW_VALID_P (win, GET_H_WIM ()))
	break;
      sparc32_flush_regwin (current_cpu, pc, win, no_errors_p);
    }
}

void
sparc32_flush_regwin (SIM_CPU *cpu, IADDR pc, int win, int no_errors_p)
{
  int i;
  USI sp,fp,addr;
  /* Fetch pointers to lregs and iregs for this frame.  */
  SI *lregs = REAL_LREGS (cpu, win);
  SI *iregs = REAL_IREGS (cpu, win);
  SIM_DESC sd = CPU_STATE (cpu);

  /* Fetch values of sp,fp for this frame.  */
  sp = REAL_OREGS (cpu, win) [6];
  fp = REAL_IREGS (cpu, win) [6];

  /* Exit early if there'd be a memory fault but we can't have any errors.  */
  if (no_errors_p)
    {
      /* Check if sp and fp indicate a proper save may not have been done.  */
      if (fp <= sp
	  || fp - sp < 8 * sizeof (SI))
	return;
      /* sp misaligned? */
      if (sp & 3)
	return;
    }

  /* Use sim_core_write_aligned_N here to handle endian conversions.  */

  addr = sp;
  for (i = 0; i < 8; i++)
    {
      if (no_errors_p)
	{
	  char reg[4];
	  SETTSI (reg, lregs[i]);
	  if (sim_core_write_buffer (sd, cpu, write_map, reg, addr, 4) == 0)
	    return;
	}
      else
	sim_core_write_aligned_4 (cpu, pc, write_map, addr, lregs[i]);
      addr += 4;
    }
  for (i = 0; i < 8; i++)
    {
      if (no_errors_p)
	{
	  char reg[4];
	  SETTSI (reg, lregs[i]);
	  if (sim_core_write_buffer (sd, cpu, write_map, reg, addr, 4) == 0)
	    return;
	}
      else
	sim_core_write_aligned_4 (cpu, pc, write_map, addr, iregs[i]);
      addr += 4;
    }
}

void
sparc32_load_regwin (SIM_CPU *cpu, IADDR pc, int win)
{
  int i;
  /* Fetch value of sp for this frame.  */
  SI addr = REAL_OREGS (cpu, win) [6];
  /* Fetch pointers to lregs and iregs for this frame.  */
  SI *lregs = REAL_LREGS (cpu, win);
  SI *iregs = REAL_IREGS (cpu, win);

  /* Use sim_core_read_aligned_N here to handle endian conversions.  */

  for (i = 0; i < 8; i++)
    {
      lregs[i] = sim_core_read_aligned_4 (cpu, pc, read_map, addr);
      addr += 4;
    }
  for (i = 0; i < 8; i++)
    {
      iregs[i] = sim_core_read_aligned_4 (cpu, pc, read_map, addr);
      addr += 4;
    }
}

/* Save/restore insns.  */

/* Handle the save instruction.  */

SI
sparc32_do_save (SIM_CPU *current_cpu, IADDR pc, SI rs1, SI rs2_simm13)
{
  SI rd;
  int oldwin,newwin,wim;

  /* FIXME: Watch for stack overflow if user prog.  */

  /* Determine new window number and see if its bit is set in the
     Window Invalid Mask.  */
  oldwin = GET_H_CWP ();
  newwin = NEXT_WIN (oldwin);
  wim = GET_H_WIM ();
  if (! WINDOW_VALID_P (newwin, wim))
    sparc32_window_overflow (current_cpu, pc);

  /* `rs1' and `rs2_simm13' are based on the old window (which we want) */
  rd = rs1 + rs2_simm13;

  /* Switch to the new window.  */
  sparc32_save_regwin (current_cpu);

  if (TRACE_INSN_P (current_cpu)) /* FIXME */
    {
      trace_result (current_cpu, "sp", 'x', GET_H_GR (H_GR__SP));
      trace_result (current_cpu, "fp", 'x', GET_H_GR (H_GR__FP));
      trace_result (current_cpu, "cwp", 'x', GET_H_CWP ());
    }

  /* `rd' will be saved in the new window by the semantic code.  */
  return rd;
}

/* Handle the restore instruction.  */

SI
sparc32_do_restore (SIM_CPU *current_cpu, IADDR pc, SI rs1, SI rs2_simm13)
{
  SI rd;
  int oldwin,newwin,wim;

  /* Determine new window number and see if its bit is set in the
     Window Invalid Mask.  */
  oldwin = GET_H_CWP ();
  newwin = PREV_WIN (oldwin);
  wim = GET_H_WIM ();
  if (! WINDOW_VALID_P (newwin, wim))
    sparc32_window_underflow (current_cpu, pc);

  /* `rs1' and `rs2_simm13' are based on the old window (which we want) */
  rd = rs1 + rs2_simm13;

  /* Switch to the previous window.  */
  sparc32_restore_regwin (current_cpu);

  if (TRACE_INSN_P (current_cpu)) /* FIXME */
    {
      trace_result (current_cpu, "sp", 'x', GET_H_GR (H_GR__SP));
      trace_result (current_cpu, "fp", 'x', GET_H_GR (H_GR__FP));
      trace_result (current_cpu, "cwp", 'x', GET_H_CWP ());
    }

  /* `rd' will be saved in the new window by the semantic code.  */
  return rd;
}

/* ASI accesses.  */

#define DEFINE_GETMEM(mode, size) \
mode \
XCONCAT3 (GETMEM,mode,ASI) (SIM_CPU *cpu, IADDR pc, ADDR a, INT asi) \
{ \
  return 0; /* FIXME:wip */ \
}

DEFINE_GETMEM (QI, 1)
DEFINE_GETMEM (UQI, 1)
DEFINE_GETMEM (HI, 2)
DEFINE_GETMEM (UHI, 2)
DEFINE_GETMEM (SI, 4)
DEFINE_GETMEM (USI, 4)
DEFINE_GETMEM (DI, 8)
DEFINE_GETMEM (UDI, 8)

#undef DEFINE_GETMEM

#define DEFINE_SETMEM(mode, size) \
void \
XCONCAT3 (SETMEM,mode,ASI) (SIM_CPU *cpu, IADDR pc, ADDR a, INT asi, mode newval) \
{ \
  return; /* FIXME:wip */ \
}

DEFINE_SETMEM (QI, 1)
DEFINE_SETMEM (UQI, 1)
DEFINE_SETMEM (HI, 2)
DEFINE_SETMEM (UHI, 2)
DEFINE_SETMEM (SI, 4)
DEFINE_SETMEM (USI, 4)
DEFINE_SETMEM (DI, 8)
DEFINE_SETMEM (UDI, 8)

#undef SETMEM

/* ldstub, swap insns */

void
sparc32_do_ldstub (SIM_CPU *current_cpu, IADDR pc, INT rd_regno,
		   SI rs1, SI rs2_simm13, INT asi)
{
}

void
sparc32_do_swap (SIM_CPU *current_cpu, IADDR pc, INT rd_regno,
		 SI rs1, SI rs2_simm13, INT asi)
{
}

/* Profiling support.  */

#if WITH_PROFILE_MODEL_P

/* FIXME: Some of these should be inline or macros.  Later.  */

void
sparc32_model_mark_get_h_gr (SIM_CPU *cpu, ARGBUF *abuf)
{
}

void
sparc32_model_mark_set_h_gr (SIM_CPU *cpu, ARGBUF *abuf)
{
}

void
sparc32_model_mark_busy_reg (SIM_CPU *cpu, ARGBUF *abuf)
{
}

void
sparc32_model_mark_unbusy_reg (SIM_CPU *cpu, ARGBUF *abuf)
{
}

/* Initialize cycle counting for an insn.
   FIRST_P is non-zero if this is the first insn in a set of parallel
   insns.  */

void
sparc32_model_insn_before (SIM_CPU *cpu, int first_p)
{
}

/* Record the cycles computed for an insn.
   LAST_P is non-zero if this is the last insn in a set of parallel insns,
   and we update the total cycle count.
   CYCLES is the cycle count of the insn.  */

void
sparc32_model_insn_after (SIM_CPU *cpu, int last_p, int cycles)
{
}

int
sparc32_model_sparc32_def_u_exec (SIM_CPU *cpu, const IDESC *idesc,
				  int unit_num, int referenced)
{
  return idesc->timing->units[unit_num].done;
}

#endif /* WITH_PROFILE_MODEL_P */

/* Debugging stuff.  */

/* Pretty print the control and integer registers.
   This can be invoked with the user-defined "dump" command in gdb.  */

void
sim_debug_dump ()
{
  extern SIM_DESC current_state;
  host_callback *cb = STATE_CALLBACK (current_state);
  SIM_CPU *current_cpu = STATE_CPU (current_state, 0);

  sim_cb_printf (cb, "CPU Registers\n");
  sim_cb_printf (cb, "CWP:%4d WIM:%4d\n", GET_H_CWP (), GET_H_WIM ());
}
