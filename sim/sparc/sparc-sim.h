/* collection of junk waiting time to sort out
   Copyright (C) 1999 Cygnus Solutions.  */

#ifndef SPARC_SIM_H
#define SPARC_SIM_H

/* GDB register numbers, will eventually live in more public place.  */
#define SPARC32_PC_REGNUM 68
#define SPARC32_NPC_REGNUM 69
#define SPARC64_PC_REGNUM ?
#define SPARC64_NPC_REGNUM ?

/* Commonly used registers.  */
#define REG_O0 8
#define REG_O1 9
#define REG_O2 10
#define REG_O3 11
#define REG_O4 12
#define REG_O5 13
#define REG_SP 14
#define REG_FP 30

#define CURRENT_GREGS(cpu) (& (cpu)->current_regs[0])
#define CURRENT_OREGS(cpu) (& (cpu)->current_regs[8])
#define CURRENT_LREGS(cpu) (& (cpu)->current_regs[16])
#define CURRENT_IREGS(cpu) (& (cpu)->current_regs[24])

/* The register windows are recorded in a seemingly backwards manner.
   %i0-7 live at lower addresses in memory than %o0-7 (even though they
   have higher register numbers).  This is because on v9 the stack grows
   upwards in the sense that CWP increases during a "push" (save) operation.
   On v8 CWP decreases during a "push" (save) operation.  */

#define REAL_GREGS(cpu) \
  (& (cpu)->global_regs[0])
#define REAL_IREGS(cpu, win) \
  (& (cpu)->win_regs[win][0])
#define REAL_LREGS(cpu, win) \
  (& (cpu)->win_regs[win][8])
#ifdef WANT_CPU_SPARC32
#define REAL_OREGS(cpu, win) \
  (& (cpu)->win_regs[ROUND_WIN ((win) - 1)][0])
#endif
#ifdef WANT_CPU_SPARC64
#define REAL_OREGS(cpu, win) \
  (& (cpu)->win_regs[ROUND_WIN ((win) + 1)][0])
#endif

/* Window state.  */

/* Various ways to handle register windows:
   - don't, do all index calculations at runtime
   - table of lookup tables, one entry for each window
   - working copy of registers that is swapped in/out on each
     register window change
   - table of 32 pointers, one for each regs
   - table of 4 pointers, one for each of g,o,l,i regs
   - ???
*/

/* FIXME: Hardcodes number of windows.  */
#define GET_NWINDOWS() NWINDOWS

/* We take advantage of the fact that NWINDOWS is always a power of two.  */
#define ROUND_WIN(x) ((x) & (NWINDOWS - 1))

/* Return the next and previous windows of WIN.
   Note that the next window for sparc32 is win-1 whereas the next window
   for sparc64 is win+1.  */
#ifdef WANT_CPU_SPARC32
#define NEXT_WIN(win) ROUND_WIN ((win) - 1)
#define PREV_WIN(win) ROUND_WIN ((win) + 1)
#endif
#ifdef WANT_CPU_SPARC64
#define NEXT_WIN(win) ROUND_WIN ((win) + 1)
#define PREV_WIN(win) ROUND_WIN ((win) - 1)
#endif

#if 0 /* FIXME: Use GET_H_FOO */
/* These are v9 specific.  */
#define GET_CANSAVE(cpu_)    (cpu_->cgen_cpu.cpu.h_cansave)
#define GET_CANRESTORE(cpu_) (cpu_->cgen_cpu.cpu.h_canrestore)
#define GET_CLEANWIN(cpu_)   (cpu_->cgen_cpu.cpu.h_cleanwin)
#define GET_OTHERWIN(cpu_)   (cpu_->cgen_cpu.cpu.h_otherwin)
#define GET_WSTATE(cpu_)     (cpu_->cgen_cpu.cpu.h_wstate)

/* These are v9 specific.  */
#define SET_CANSAVE(cpu_, x)    (cpu_->cgen_cpu.cpu.h_cansave = (x))
#define SET_CANRESTORE(cpu_, x) (cpu_->cgen_cpu.cpu.h_canrestore = (x))
#define SET_CLEANWIN(cpu_, x)   (cpu_->cgen_cpu.cpu.h_cleanwin = (x))
#define SET_OTHERWIN(cpu_, x)   (cpu_->cgen_cpu.cpu.h_otherwin = (x))
#define SET_WSTATE(cpu_, x)     (cpu_->cgen_cpu.cpu.h_wstate = (x))
#endif

#define CPU32_CGEN(cpu) (& (cpu)->sparc_cpu.sparc32.cpu_cgen)

#ifdef WANT_CPU_SPARC32
/*#define ARGBUF SPARC32_ARGBUF*/
#define CPU_PROFILE_STATE(cpu) (& CPU32 (cpu)->cpu_profile)
/* Macro to access heart of cpu: the registers, etc.  */
#define CPU_CGEN(cpu) CPU32_CGEN (cpu)
#endif

#define CPU64_CGEN(cpu) (& (cpu)->sparc_cpu.sparc64.cpu_cgen)

#ifdef WANT_CPU_SPARC64
/*#define ARGBUF SPARC64_ARGBUF*/
#define CPU_PROFILE_STATE(cpu) (& CPU64 (cpu)->cpu_profile)
/* Macro to access heart of cpu: the registers, etc.  */
#define CPU_CGEN(cpu) CPU64_CGEN (cpu)
#endif

/* ASI accesses.  */

#define DECLARE_GETMEM(mode, size) \
extern mode XCONCAT3 (GETMEM,mode,ASI) (SIM_CPU *, IADDR, ADDR, INT);

DECLARE_GETMEM (QI, 1)
DECLARE_GETMEM (UQI, 1)
DECLARE_GETMEM (HI, 2)
DECLARE_GETMEM (UHI, 2)
DECLARE_GETMEM (SI, 4)
DECLARE_GETMEM (USI, 4)
DECLARE_GETMEM (DI, 8)
DECLARE_GETMEM (UDI, 8)

#undef DECLARE_GETMEM

#define DECLARE_SETMEM(mode, size) \
extern void XCONCAT3 (SETMEM,mode,ASI) (SIM_CPU *, IADDR, ADDR, INT, mode);

DECLARE_SETMEM (QI, 1)
DECLARE_SETMEM (UQI, 1)
DECLARE_SETMEM (HI, 2)
DECLARE_SETMEM (UHI, 2)
DECLARE_SETMEM (SI, 4)
DECLARE_SETMEM (USI, 4)
DECLARE_SETMEM (DI, 8)
DECLARE_SETMEM (UDI, 8)

#undef DECLARE_SETMEM

/* Misc. support routines.  */

void sparc32_cold_reset (SIM_CPU *, int userland_p_);

SI sparc32_do_restore (SIM_CPU *, IADDR pc_, SI rs1_, SI rs2_simm13_);
SI sparc32_do_save (SIM_CPU *, IADDR pc_, SI rs1_, SI rs2_simm13_);

void sparc32_do_ldstub (SIM_CPU *, IADDR pc_, INT rd_regno_,
			SI rs1_, SI rs2_simm13_, INT asi_);
void sparc32_do_swap (SIM_CPU *, IADDR pc_, INT rd_regno_,
		      SI rs1_, SI rs2_simm13_, INT asi_);

void sparc32_invalid_insn (SIM_CPU *, IADDR pc_);
void sparc32_core_signal (SIM_DESC, SIM_CPU *, sim_cia pc_,
			  unsigned int map_, int nr_bytes_, address_word addr_,
			  transfer_type transfer_, sim_core_signals sig_);

/* Profiling support.  */

void model_mark_get_h_gr (SIM_CPU *, void *);
void model_mark_set_h_gr (SIM_CPU *, void *);
void model_mark_busy_reg (SIM_CPU *, void *);
void model_mark_unbusy_reg (SIM_CPU *, void *);

/* Called by semantic code to annul the next insn.  */

#define SEM_ANNUL_INSN(cpu, pc, yes) \
do { CPU_CGEN_HW (cpu)->h_annul_p = (yes); } while (0)

#if 0 /* old experiment */

/* Macros used by the semantic code generator and cgen-run.c to control
   branches.  */

/* Value returned for the next PC when a branch has occured.  */
#define PC_BRANCH_INSN 3

/* Value of npc when there is no delay slot.  */
#define NPC_NO_DELAY_INSN 3

/* CURRENT_CPU isn't used, a variable of same name must exist for SET_H_NPC */
#define BRANCH_NEW_PC(current_cpu, var, addr) \
do { \
  SET_H_PC (var); \
  SET_H_NPC (addr); \
  var = PC_BRANCH_INSN; \
} while (0)

/* Mark the next instruction as being annulled if YES is non-zero.
   VAR is the local variable that contains the next PC value.
   CURRENT_CPU isn't used, a variable of same name must exist for SET_H_NPC */
#define ANNUL_NEXT_INSN(current_cpu, var, yes) \
do { \
  if (yes) \
    { \
      SET_H_ANNUL_P (yes); \
      var = PC_BRANCH_INSN; \
    } \
} while (0)

/* Update the PC.
   We also have to watch for delay slots and annulled insns.
   current_cpu isn't used, a variable of same name must exist for SET_H_PC */
#define UPDATE_PC(current_cpu, new_pc) \
do { \
  if ((new_pc) == PC_BRANCH_INSN) \
    { \
      int annul_p = GET_H_ANNUL_P (); \
      PCADDR npc = GET_H_NPC (); \
      SET_H_ANNUL_P (0); \
      if (npc != NPC_NO_DELAY_INSN) \
	{ \
	  if (annul_p) \
	    { \
	      /* FIXME: Could update cycle count.  */ \
	      SET_H_PC (npc); \
	      SET_H_NPC (NPC_NO_DELAY_INSN) ; \
	    } \
	  else \
	    goto GotoFirstInsn; \
	} \
      else \
	{ \
	  /* Untaken annulled branch.  */ \
	  /* FIXME: Could update cycle count.  */ \
	  ASSERT (annul_p); \
	  SET_H_PC (GET_H_PC () + 8); \
	} \
    } \
  else \
    SET_H_PC (new_pc); \
} while (0)

/* Return nonzero if in non-annulled delay slot.  */
#define EXECUTE_FIRST_INSN_P(current_cpu) \
(GET_H_NPC () != NPC_NO_DELAY_INSN)

/* Called when the first instruction is in non-annulled delay slot.  */

#define EXECUTE_FIRST_INSN(current_cpu, sc, fast_p) \
do { \
  PCADDR new_pc = execute (current_cpu, sc, fast_p); \
  if (new_pc == PC_BRANCH_INSN) \
    { \
      abort (); /* FIXME: dcti in delay slot not handled yet */ \
    } \
  else \
    { \
      SET_H_PC (GET_H_NPC ()); \
      SET_H_NPC (NPC_NO_DELAY_INSN); \
    } \
} while (0)

#endif /* old experiment */

#endif /* SPARC_SIM_H */
