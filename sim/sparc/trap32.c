/* sparc32 trap support
   Copyright (C) 1999 Cygnus Solutions.  */

#define WANT_CPU sparc32
#define WANT_CPU_SPARC32

#include "sim-main.h"
#include "targ-vals.h"

/* Indicate a window overflow has occured.  */

void
sparc32_window_overflow (SIM_CPU *cpu, IADDR pc)
{
  SIM_DESC sd = CPU_STATE (cpu);

  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    sparc32_hw_trap (cpu, pc, TRAP32_WINDOW_OVERFLOW);
  else
    sparc32_hw_trap (cpu, pc, TRAP32_SIM_SPILL);
}

/* Indicate a window underflow has occured.  */

void
sparc32_window_underflow (SIM_CPU *cpu, IADDR pc)
{
  SIM_DESC sd = CPU_STATE (cpu);

  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    sparc32_hw_trap (cpu, pc, TRAP32_WINDOW_UNDERFLOW);
  else
    sparc32_hw_trap (cpu, pc, TRAP32_SIM_FILL);
}

void
sparc32_invalid_insn (SIM_CPU * cpu, IADDR pc)
{
  sparc32_hw_trap (cpu, pc, TRAP32_ILLEGAL_INSN);
}

void
sparc32_core_signal (SIM_DESC sd, SIM_CPU *cpu, sim_cia pc,
		     unsigned int map, int nr_bytes, address_word addr,
		     transfer_type transfer, sim_core_signals sig)
{
  sparc32_hw_trap (cpu, pc,
		   map == exec_map
		   ? TRAP32_INSTRUCTION_ACCESS
		   : TRAP32_DATA_ACCESS);
}

/* Handle hardware generated traps when --environment=operating.  */

static void
sparc32_hw_trap_oper (SIM_CPU *current_cpu, IADDR pc, TRAP32_TYPE trap)
{
  SIM_DESC sd = CPU_STATE (current_cpu);
  IADDR new_pc = (GET_H_TBR () & 0xfffff000) | (trap << 4);
  USI psr = GET_H_PSR ();

  psr &= ~PSR_ET;
  psr = (psr & ~PSR_PS) | (psr & PSR_S ? PSR_PS : 0);
  psr |= PSR_S;
  psr = (psr & ~PSR_CWP) | NEXT_WIN (psr & PSR_CWP);
  SET_H_PSR (psr);
  SET_H_GR (H_GR__L1, GET_H_PC ());
  SET_H_GR (H_GR__L2, GET_H_NPC ());

  SET_H_ANNUL_P (0);

  /* The wrtbr insn doesn't affect the tt part so SET_H_TBR doesn't either
     (??? doesn't *have* to be this way though).
     Therefore we can't use SET_H_TBR here.  */
  CPU (h_tbr) = new_pc;
  SET_H_PC (new_pc);
  SET_H_NPC (new_pc + 4);

  sim_engine_restart (CPU_STATE (current_cpu), current_cpu, NULL, new_pc);
}

/* Handle hardware generated traps when --environment=user.  */

static void
sparc32_hw_trap_user (SIM_CPU *current_cpu, IADDR pc, TRAP32_TYPE trap)
{
  SIM_DESC sd = CPU_STATE (current_cpu);

  switch (trap)
    {
    case TRAP32_SIM_SPILL :
      /* The CWP-1 window is invalid.  */
      {
	int win = NEXT_WIN (GET_H_CWP ());
	int next_win = NEXT_WIN (win);
	int nwindows = GET_NWINDOWS ();
	unsigned int mask = (1 << nwindows) - 1;
	unsigned int wim = GET_H_WIM ();

	/* There's no need to flush `current_regs' here, `next_win' can't
	   refer to it.  */
	sparc32_flush_regwin (current_cpu, pc, next_win, 0 /* error ok (?) */);
	/* Rotate WIM right one.  */
	wim = ((wim & mask) >> 1) | (wim << (nwindows - 1));
	SET_H_WIM (wim & mask);
	return;
      }

    case TRAP32_SIM_FILL :
      /* The CWP+1 window is invalid.  */
      {
	int win = PREV_WIN (GET_H_CWP ());
	int nwindows = GET_NWINDOWS ();
	unsigned int mask = (1 << nwindows) - 1;
	unsigned int wim = GET_H_WIM ();

	/* Load caller's caller's window.
	   There's no need to flush `current_regs' as `win' can't
	   refer to it.  */
	sparc32_load_regwin (current_cpu, pc, win);
	/* Rotate WIM left one.  */
	wim = (wim << 1) | ((wim & mask) >> (nwindows - 1));
	SET_H_WIM (wim & mask);
	return;
      }
    }

  sim_io_eprintf (sd, "Received trap %d\n", trap);
  sim_engine_halt (sd, current_cpu, NULL, pc, sim_stopped, SIM_SIGABRT);
}

/* Handle hardware generated traps.  */

void
sparc32_hw_trap (SIM_CPU *current_cpu, IADDR pc, TRAP32_TYPE trap)
{
  SIM_DESC sd = CPU_STATE (current_cpu);

  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    sparc32_hw_trap_oper (current_cpu, pc, trap);
  else
    sparc32_hw_trap_user (current_cpu, pc, trap);
}

/* Handle the trap insn when --environment=operating.  */

static IADDR
sparc32_sw_trap_oper (SIM_CPU *current_cpu, IADDR pc, SI rs1, SI rs2_simm13)
{
  SIM_DESC sd = CPU_STATE (current_cpu);
  int trap = 128 + ((rs1 + rs2_simm13) & 127);
  IADDR new_pc;

  /* ??? Quick hack to have breakpoints work with gdb+"target sim" until
     other things are working.  */
  if (trap == TRAP32_BREAKPOINT)
    sim_engine_halt (sd, current_cpu, NULL, pc, sim_stopped, SIM_SIGTRAP);

  if (! GET_H_ET ())
    {
      /* Enter error mode.
	 ??? wip, need to remain compatible with erc32 for now.  */
      int i0 = GET_H_GR (H_GR__I0);
      int i1 = GET_H_GR (H_GR__I1);

      if (i1 == LIBGLOSS_EXIT_MAGIC)
	sim_engine_halt (sd, current_cpu, NULL, pc, sim_exited, i0);
      else
	{
	  sim_io_eprintf (sd, "Unexpected program termination, pc=0x%x\n",
			  (int) pc);
	  sim_engine_halt (sd, current_cpu, NULL, pc,
			   sim_signalled, SIM_SIGABRT);
	}
    }

  SET_H_ET (0);
  SET_H_PSR ((GET_H_PSR () & ~(PSR_CWP | PSR_PS))
	     | PSR_S
	     | (GET_H_S () ? PSR_PS : 0)
	     | (NEXT_WIN (GET_H_CWP ())));
  SET_H_GR (H_GR__L1, GET_H_PC ());
  SET_H_GR (H_GR__L2, GET_H_NPC ());
  /* The wrtbr insn doesn't affect the tt part so SET_H_TBR doesn't either
     (??? doesn't *have* to be this way though).
     Therefore we can't use SET_H_TBR here.  */
  CPU (h_tbr) = new_pc = ((GET_H_TBR () & 0xfffff000)
			  | (trap << 4));
  return new_pc;
}

/* Subroutine of sparc32_do_trap to read target memory.  */

static int
syscall_read_mem (host_callback *cb, CB_SYSCALL *sc,
		  unsigned long taddr, char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_read_buffer (sd, cpu, read_map, buf, taddr, bytes);
}

/* Subroutine of sparc32_do_trap to write target memory.  */

static int
syscall_write_mem (host_callback *cb, CB_SYSCALL *sc,
		   unsigned long taddr, const char *buf, int bytes)
{
  SIM_DESC sd = (SIM_DESC) sc->p1;
  SIM_CPU *cpu = (SIM_CPU *) sc->p2;

  return sim_core_write_buffer (sd, cpu, write_map, buf, taddr, bytes);
}

/* Handle the trap insn when --environment=user.  */

static IADDR
sparc32_sw_trap_user (SIM_CPU *current_cpu, IADDR pc, SI rs1, SI rs2_simm13)
{
  SIM_DESC sd = CPU_STATE (current_cpu);
  int trap = 128 + ((rs1 + rs2_simm13) & 127);
  IADDR new_pc = pc + 4;

  switch (trap)
    {
    case TRAP32_SYSCALL :
      /* FIXME: Later make trap number runtime selectable.  */
      {
	CB_SYSCALL s;

	CB_SYSCALL_INIT (&s);
	s.func = a_sparc_h_gr_get (current_cpu, 8);
	s.arg1 = a_sparc_h_gr_get (current_cpu, 9);
	s.arg2 = a_sparc_h_gr_get (current_cpu, 10);
	s.arg3 = a_sparc_h_gr_get (current_cpu, 11);
	if (s.func == TARGET_SYS_exit)
	  {
	    /* Tell sim_resume program called exit().  */
	    sim_engine_halt (sd, current_cpu, NULL, pc, sim_exited, s.arg1);
	  }
	s.p1 = (PTR) sd;
	s.p2 = (PTR) current_cpu;
	s.read_mem = syscall_read_mem;
	s.write_mem = syscall_write_mem;
	cb_syscall (STATE_CALLBACK (CPU_STATE (current_cpu)), &s);
	a_sparc_h_gr_set (current_cpu, 10, s.errcode);
	a_sparc_h_gr_set (current_cpu, 8, s.result);
	a_sparc_h_gr_set (current_cpu, 9, s.result2);
	break;
      }

    case TRAP32_BREAKPOINT :
      sim_engine_halt (sd, current_cpu, NULL, pc, sim_stopped, SIM_SIGTRAP);

    case TRAP32_DIVIDE_0 :
      sim_engine_halt (sd, current_cpu, NULL, pc, sim_stopped, SIM_SIGFPE);

    case TRAP32_FLUSH_REGWIN :
      sparc32_flush_regwins (current_cpu, pc, 0 /* error ok */);
      break;

    default :
      sim_io_eprintf (sd, "Unsupported trap %d\n", trap);
      sim_engine_halt (sd, current_cpu, NULL, pc, sim_stopped, SIM_SIGILL);
    }

  return new_pc;
}

/* Called from the semantic code to handle the trap instruction.  */

IADDR
sparc32_sw_trap (SIM_CPU *current_cpu, IADDR pc, SI rs1, SI rs2_simm13)
{
  SIM_DESC sd = CPU_STATE (current_cpu);
  IADDR new_pc;

  if (STATE_ENVIRONMENT (sd) == OPERATING_ENVIRONMENT)
    new_pc = sparc32_sw_trap_oper (current_cpu, pc, rs1, rs2_simm13);
  else
    new_pc = sparc32_sw_trap_user (current_cpu, pc, rs1, rs2_simm13);
  return new_pc;
}

/* Handle the rett insn.  */

IADDR
sparc32_do_rett (SIM_CPU *current_cpu, IADDR pc, SI rs1, SI rs2_simm13)
{
  int psr = GET_H_PSR ();

  /* FIXME: check for trap conditions.  */

  SET_H_PSR ((psr & ~(PSR_S + PSR_CWP))
	     | ((psr & PSR_PS) ? PSR_S : 0)
	     | PSR_ET
	     | PREV_WIN (psr & PSR_CWP));

  if (TRACE_INSN_P (current_cpu)) /* FIXME */
    {
      trace_result (current_cpu, "sp", 'x', GET_H_GR (H_GR__SP));
      trace_result (current_cpu, "fp", 'x', GET_H_GR (H_GR__FP));
      trace_result (current_cpu, "cwp", 'x', GET_H_CWP ());
    }

  return rs1 + rs2_simm13;
}
