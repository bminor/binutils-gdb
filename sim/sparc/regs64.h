/* sparc64 register definitions
   Copyright (C) 1999 Cygnus Solutions.  */

#ifndef REG64_H
#define REG64_H

/* The PSR is a hodge-podge of various things.
   ??? The final organization of this is wip.  */

extern USI sparc32_get_h_psr_handler (SIM_CPU *);
extern void sparc32_set_h_psr_handler (SIM_CPU *, USI);
#define GET_H_PSR()    sparc32_get_h_psr_handler (current_cpu)
#define SET_H_PSR(val) sparc32_set_h_psr_handler (current_cpu, (val))

/* The y reg is a virtual reg as it's actually one of the asr regs.
   ??? To be replaced in time with get/set specs.  */
#if 0
#define sparc32_h_y_get(cpu)     (CPU_CGEN_HW (cpu)->h_asr[0])
#define sparc32_h_y_set(cpu,val) (CPU_CGEN_HW (cpu)->h_asr[0] = (val))
#endif
#define GET_H_Y()       (CPU (h_asr) [0])
#define SET_H_Y(newval) do { CPU (h_asr) [0] = (newval); } while (0)

/* The Trap Base Register.  */
#define GET_H_TBR() CPU (h_tbr)
#define SET_H_TBR(newval) \
  do { \
    CPU (h_tbr) = (CPU (h_tbr) & 0xff0) | ((newval) & 0xfffff000); \
  } while (0)

/* sparc32 register window stuff.  */

/* Handle gets/sets of h-cwp.
   This handles swapping out the current set of window registers
   and swapping in the new.  How the "swapping" is done depends on the
   register window implementation of the day.  */
void sparc32_set_h_cwp_handler (SIM_CPU *, int);
#define GET_H_CWP() CPU (h_cwp)
#define SET_H_CWP(newval) sparc32_set_h_cwp_handler (current_cpu, (newval))

/* WIM accessors.  */
/* ??? Yes, mask computation assumes nwindows < 32.  */
#define GET_H_WIM()       (CPU (h_wim) & ((1 << GET_NWINDOWS ()) - 1))
#define SET_H_WIM(newval) (CPU (h_wim) = (newval))

/* Return non-zero if window WIN is valid in WIM.  */
#define WINDOW_VALID_P(win, wim) (((wim) & (1 << (win))) == 0)

void sparc32_alloc_regwins (SIM_CPU *, int);
void sparc32_free_regwins (SIM_CPU *);
void sparc32_swapout_regwin (SIM_CPU *, int);
void sparc32_swapin_regwin (SIM_CPU *, int);

void sparc32_load_regwin (SIM_CPU *, IADDR pc_, int win_);
void sparc32_flush_regwin (SIM_CPU *, IADDR pc_, int win_, int no_errors_p_);
void sparc32_flush_regwins (SIM_CPU *, IADDR pc_, int no_errors_p_);

void sparc32_save_regwin (SIM_CPU *);
void sparc32_restore_regwin (SIM_CPU *);

/* Integer register access macros.
   Provides an interface between the cpu description and the register window
   implementation of the day.  To be solidified in time.  */
#define GET_H_GR(r) (current_cpu->current_regs[r])

/* ??? The r != 0 test may not be necessary as sufficient numbers of dni
   entries can prevent this from occuring (I think).  Even then though doing
   this makes things more robust, and a lot of dni's would be needed.
   ??? The other way to handle %g0 is to always reset it for each insn
   [perhaps optimized to only do so when necessary].  */
#define SET_H_GR(r, val) \
  ((r) != 0 ? (current_cpu->current_regs[r] = (val)) : 0)

#endif /* REG64_H */
