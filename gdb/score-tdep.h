/* Target-dependent code for the S+core architecture, for GDB,
   the GNU Debugger.

   Copyright (C) 2006, 2007, 2008, 2009 Free Software Foundation, Inc.

   Contributed by Qinwei (qinwei@sunnorth.com.cn)
   Contributed by Ching-Peng Lin (cplin@sunplus.com)

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

#ifndef SCORE_TDEP_H
#define SCORE_TDEP_H
#include "math.h"

enum gdb_regnum
{
  SCORE_SP_REGNUM = 0,
  SCORE_FP_REGNUM = 2,
  SCORE_RA_REGNUM = 3,
  SCORE_A0_REGNUM = 4,
  SCORE_AL_REGNUM = 7,
  SCORE_PC_REGNUM = 49,
};

#define SCORE_A0_REGNUM        4
#define SCORE_A1_REGNUM        5
#define SCORE_REGSIZE          4
#define SCORE7_NUM_REGS         56
#define SCORE3_NUM_REGS         50
#define SCORE_BEGIN_ARG_REGNUM 4
#define SCORE_LAST_ARG_REGNUM  7

#define SCORE_INSTLEN          4
#define SCORE16_INSTLEN        2

/* Forward declarations. */
struct regset;

/* Target-dependent structure in gdbarch */
struct gdbarch_tdep
{
    /* Cached core file helpers. */
    struct regset *gregset;
};

/* Linux Core file support (dirty hack)
  
   S+core Linux register set definition, copy from S+core Linux */
struct pt_regs {
    /* Pad bytes for argument save space on the stack. */
    unsigned long pad0[6]; /* may be 4,MIPS accept 6var,SCore accepts 4 Var--yuchen */

    /* Saved main processor registers. */
    unsigned long orig_r4;
    unsigned long regs[32];

    /* Other saved registers. */
    unsigned long cel;
    unsigned long ceh;

    unsigned long sr0;  /*cnt*/
    unsigned long sr1;  /*lcr*/
    unsigned long sr2;  /*scr*/

    /* saved cp0 registers */
    unsigned long cp0_epc;
    unsigned long cp0_ema;
    unsigned long cp0_psr;
    unsigned long cp0_ecr;
    unsigned long cp0_condition;
};

typedef struct pt_regs elf_gregset_t;

#ifdef WITH_SIM

#include <breakpoint.h>

int soc_gh_can_use_watch(int type, int cnt);
int soc_gh_add_watch(unsigned int addr, int len, int type);
int soc_gh_del_watch(unsigned int addr, int len, int type);
int soc_gh_stopped_by_watch(void);
int soc_gh_add_hardbp(unsigned int addr); 
int soc_gh_del_hardbp(unsigned int addr); 

int score_target_can_use_watch(int type, int cnt, int ot);
int score_stopped_by_watch(void);
int score_target_insert_watchpoint (CORE_ADDR addr, int len, int type);
int score_target_remove_watchpoint (CORE_ADDR addr, int len, int type);
int score_target_insert_hw_breakpoint (struct gdbarch *gdbarch, struct bp_target_info * bp_tgt);
int score_target_remove_hw_breakpoint (struct gdbarch *gdbarch, struct bp_target_info * bp_tgt);

#define TARGET_HAS_HARDWARE_WATCHPOINTS

#ifdef TARGET_CAN_USE_HARDWARE_WATCHPOINT
#undef TARGET_CAN_USE_HARDWARE_WATCHPOINT

#define TARGET_CAN_USE_HARDWARE_WATCHPOINT(type, cnt, ot) \
    score_target_can_use_watch(type, cnt, ot)
#endif

#ifdef STOPPED_BY_WATCHPOINT
#undef STOPPED_BY_WATCHPOINT

#define STOPPED_BY_WATCHPOINT(w) \
    score_stopped_by_watch()
#endif

#ifdef target_insert_watchpoint
#undef target_insert_watchpoint

#define target_insert_watchpoint(addr, len, type) \
    score_target_insert_watchpoint (addr, len, type)
#endif

#ifdef target_remove_watchpoint
#undef target_remove_watchpoint

#define target_remove_watchpoint(addr, len, type) \
    score_target_remove_watchpoint (addr, len, type)
#endif

#ifdef target_insert_hw_breakpoint
#undef target_insert_hw_breakpoint

#define target_insert_hw_breakpoint(gdbarch, bp_tgt) \
    score_target_insert_hw_breakpoint (gdbarch, bp_tgt)
#endif

#ifdef target_remove_hw_breakpoint
#undef target_remove_hw_breakpoint

#define target_remove_hw_breakpoint(gdbarch, bp_tgt) \
    score_target_remove_hw_breakpoint (gdbarch, bp_tgt)
#endif

#endif /* WITH_SIM */

#endif /* SCORE_TDEP_H */
