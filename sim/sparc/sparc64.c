/* sparc64 simulator support code
   Copyright (C) 1999 Cygnus Solutions.  */

#define WANT_CPU_SPARC64

#include "sim-main.h"
#include <signal.h>
#include "libiberty.h"
#include "bfd.h"
#include "cgen-mem.h"
#include "cgen-ops.h"
#include "targ-vals.h"
#include "trap64.h"

/* Initialize the program counter.  */

void
sparc64_init_pc (SIM_CPU *current_cpu, DI pc, DI npc)
{
  SET_H_PC (pc);
  SET_H_NPC (npc);
}

#if WITH_PROFILE_MODEL_P

void
sparc64_model_mark_get_h_gr (SIM_CPU *cpu, SPARC64_ARGBUF *abuf)
{
  if ((CPU_PROFILE_STATE (cpu)->h_gr & abuf->h_gr_get) != 0)
    {
      PROFILE_MODEL_LOAD_STALL_COUNT (CPU_PROFILE_DATA (cpu)) += 2;
      if (TRACE_INSN_P (cpu))
	cgen_trace_printf (cpu, " ; Load stall of 2 cycles.");
    }
}

void
sparc64_model_mark_set_h_gr (SIM_CPU *cpu, SPARC64_ARGBUF *abuf)
{
}

void
sparc64_model_mark_busy_reg (SIM_CPU *cpu, SPARC64_ARGBUF *abuf)
{
  CPU_PROFILE_STATE (cpu)->h_gr = abuf->h_gr_set;
}

void
sparc64_model_mark_unbusy_reg (SIM_CPU *cpu, SPARC64_ARGBUF *abuf)
{
  CPU_PROFILE_STATE (cpu)->h_gr = 0;
}

#endif /* WITH_PROFILE_MODEL_P */

UDI
sparc64_h_gr_get (SIM_CPU *current_cpu, unsigned int regno)
{
  return GET_INT_REG (cpu, regno);
}

void
sparc64_h_gr_set (SIM_CPU *current_cpu, unsigned int regno, UDI new_val)
{
  SET_INT_REG (cpu, regno, new_val);
}

DI *
sparc64_h_gr_regno_get_addr (SIM_CPU *current_cpu, int regno)
{
}

DI
sparc64_h_gr_regno_get (SIM_CPU *current_cpu, int regno)
{
}

void
sparc64_h_gr_regno_set (SIM_CPU *current_cpu, int regno, DI new_val)
{
}

DI
sparc64_do_save (SIM_CPU *current_cpu, DI rs1, DI rs2_simm13)
{
  DI rd;

  /* If this is a user program, watch for stack overflow ... */

#if 0
  if (STATE_user_prog && GET_AREG (GET_REG (REG_SP)) < mem_end)
    {
      if (user_prog)
	fprintf (stderr, "sim: stack space exhausted!\n");
      sim_signal (SIM_SIGACCESS);
      return 0;
    }
#endif

  if (GET_CANSAVE (current_cpu) == 0)
    {
      return trap (TRAP_SPILL);
    }
  if (GET_CLEANWIN (current_cpu) - GET_CANRESTORE (current_cpu) == 0)
    {
      return trap (TRAP_CLEAN_WINDOW);
    }

  /* The calculation is done using the old window's values
     [those passed in to us].  */

  rd = ADDDI (rs1, rs2_simm13);

  /* Switch to a new window.  */

  save_window ();

  return rd;
}

DI
sparc64_do_restore (SIM_CPU *current_cpu, DI rs1, DI rs2_simm13)
{
  DI rd;

  return rd;

}

/* Initialize the register window mechanism.  */

void
sparc64_alloc_register_windows (SIM_CPU *cpu, int nwindows)
{
  int ag,i,r,w;
  DI *(*v)[32];

  cpu->greg_lookup_table = (DI *(*)[2][32]) xmalloc (nwindows * 2 * 32 * sizeof (void *));
  cpu->globals = (DI (*)[][8]) zalloc (2 * 8 * sizeof (DI));
  cpu->win_regs = (DI (*)[][16]) zalloc (nwindows * 16 * sizeof (DI));

  v = &cpu->greg_lookup_table[0][0];
  for (w = 0; w < nwindows; w++)
    {
      for (ag = 0; ag < 2; ag++, v++)
	{
	  /* Initialize pointers to the global registers ... */
	  for (r = 0; r < 8; r++)
	    (*v)[r] = &cpu->globals[ag][r];

	  /* Initialize pointers to the output registers ... */
	  for (r = 0; r < 8; r++)
	    (*v)[r + 8] = &cpu->win_regs[(w == nwindows - 1 ? 0 : ((w + 1) * 16)) + r];

	  /* Initialize pointers to the local registers ... */
	  for (r = 0; r < 8; r++)
	    (*v)[r + 16] = &cpu->win_regs[(w * 16) + 8 + r];

	  /* Initialize pointers to the input registers ... */
	  for (r = 0; r < 8; r++)
	    (*v)[r + 24] = &cpu->win_regs[(w * 16) + r];
	}
    }
}

void
sparc64_free_register_windows (SIM_CPU *cpu)
{
  free (cpu->win_regs);
  cpu->win_regs = NULL;
  free (cpu->globals);
  cpu->globals = NULL;
  free (cpu->greg_lookup_table);
  cpu->greg_lookup_table = NULL;
}

/* Assign a new value to CWP.

   The register windows are recorded in a seemingly backwards manner.
   %i0-7 live at lower addresses in memory than %o0-7 (even though they
   have higher register numbers).  This is because the stack grows upwards
   in the sense that CWP increases during a "push" operation.

   FIXME: True for v9, but also for v8?  */

void
sparc64_set_cwp (SIM_CPU *current_cpu, int x)
{
  if (x < 0 || x >= GET_NWINDOWS (cpu))
    abort ();

  /* We can't use SET_CWP here because it uses us.  */

  cpu->cgen_cpu.cpu.h_cwp = x;

  cpu->current_greg_lookup_table = &cpu->greg_lookup_table[x][GET_AG (cpu)][0];
}

/* Create a new window.  We assume there is room.

   WARNING: The following must always be true:

   CANSAVE + CANRESTORE + OTHERWIN == NWINDOWS - 2

   We only watch for this during saves.  */

void
sparc64_save_window (SIM_CPU *current_cpu)
{
  if (GET_CANSAVE (cpu) + GET_CANRESTORE (cpu) + GET_OTHERWIN (cpu) != GET_NWINDOWS (cpu) - 2)
    abort ();

  SET_CWP (cpu, ROUND_WIN (GET_CWP (cpu) + 1));
  SET_CANSAVE (cpu, ROUND_WIN (GET_CANSAVE (cpu) - 1));
  SET_CANRESTORE (cpu, ROUND_WIN (GET_CANRESTORE (cpu) + 1));
}

/* Pop a window.  We assume no traps possible.  */

void
sparc64_restore_window (SIM_CPU *current_cpu)
{
  SET_CWP (cpu, ROUND_WIN (GET_CWP (cpu) - 1));
  SET_CANSAVE (cpu, ROUND_WIN (GET_CANSAVE (cpu) + 1));
  SET_CANRESTORE (cpu, ROUND_WIN (GET_CANRESTORE (cpu) - 1));
}

/* Flush the register windows to memory.
   This is necessary, for example, when we want to walk the stack in gdb.

   We use restore_window() and save_window() to traverse the register windows.
   This is the cleanest and simplest thing to do.  */

void
sparc64_flush_windows (SIM_CPU *cpu)
{
  int i,count = GET_CANRESTORE (cpu) + 1;

  /* Save the register windows, changing the current one as we go ... */

  for (i = 0; i < count; i++)
    {
      AI sp = GET_INT_REG (cpu, REG_SP) /*+ stack_bias*/;
      flush_one_window (cpu, sp, CURRENT_LREGS (cpu), CURRENT_IREGS (cpu));
      if (i + 1 != count) /* don't restore window if there isn't one */
	restore_window (cpu);
    }

  /* Restore the window state ...  */

  for (i = 1; i < count; i++)
    save_window (cpu);
}

void
sparc64_flush_one_window (SIM_CPU *current_cpu, AI addr, DI *lregs, DI *iregs)
{
  sim_core_write_buffer (CPU_STATE (cpu), cpu,
			 sim_core_write_map,
			 lregs, addr, 8 * sizeof (DI));
  sim_core_write_buffer (CPU_STATE (cpu), cpu,
			 sim_core_write_map,
			 iregs, addr + 8 * sizeof (DI), 8 * sizeof (DI));
}
