/* Main header for the m32r-gx.  */

#ifndef SIM_MAIN_H
#define SIM_MAIN_H

#define USING_SIM_BASE_H /* FIXME: quick hack */

struct _sim_cpu; /* FIXME: should be in sim-basics.h */
typedef struct _sim_cpu SIM_CPU;

/* sim-basics.h includes config.h but cgen-types.h must be included before
   sim-basics.h and cgen-types.h needs config.h.  */
#include "config.h"

#include "ansidecl.h"
#include "symcat.h"
#include "cgen-types.h"
#include "arch.h"
#include "sim-basics.h"

/* These must be defined before sim-base.h.  */
typedef USI sim_cia;
#define CIA_GET(cpu)     (cpu)->regs.h_pc
#define CIA_SET(cpu,val) (cpu)->regs.h_pc = (val)

#define TRAP_SYSCALL 0

#include "sim-base.h"
#include "cpu-sim.h"

#ifdef WANT_CPU_M32R
#include "cpu.h"
#include "decode.h"
#endif
#include "cpuall.h"


/* match with definition in gx-translate.c! */
typedef struct tgx_cpu_regs 
{
  unsigned int h_pc;  /* program counter */
  signed int h_gr[16];  /* general registers */
  unsigned int h_cr[16];  /* control registers */
  long long h_accum;  /* accumulator */
  unsigned h_lock;  /* lock */
} tgx_cpu_regs;

/* match with definition in gx-translate.c! */
typedef struct tgx_syscall_data
{
  unsigned pc;
  unsigned func;
  unsigned arg1;
  unsigned arg2;
  unsigned arg3;
  unsigned errcode;
  unsigned result;
  unsigned result2;
} tgx_syscall_data;


struct tgx_info;

/* match with definition in gx-translate.c! */
typedef struct tgx_callbacks 
{
  unsigned (*load)(struct tgx_info* info, unsigned pc, unsigned addr);
  void (*store)(struct tgx_info* info, unsigned pc, unsigned addr, unsigned data);
  signed char (*load1)(struct tgx_info* info, unsigned pc, unsigned addr);
  void (*store1)(struct tgx_info* info, unsigned pc, unsigned addr, signed char data);
  signed short (*load2)(struct tgx_info* info, unsigned pc, unsigned addr);
  void (*store2)(struct tgx_info* info, unsigned pc, unsigned addr, signed short data);
  void (*syscall)(struct tgx_info* info, tgx_syscall_data* data);
} tgx_callbacks;


typedef struct tgx_info
{
  struct tgx_cpu_regs* regs;
  char* pc_flags;
  struct tgx_callbacks* callbacks;
} tgx_info;


struct _sim_cpu 
{
  sim_cpu_base base;
  tgx_cpu_regs regs;
};

struct sim_state {
  sim_cpu *cpu;
#define STATE_CPU(sd, n) (/*&*/ (sd)->cpu)
  sim_state_base base;
};


/* Default memory size.  */
#define M32R_DEFAULT_MEM_SIZE 0x800000 /* 8M */



/* Register access fns.  These look up the current mach and call the
   appropriate handler.  */
SI h_gr_get (SIM_CPU *, UINT);
void h_gr_set (SIM_CPU *, UINT, SI);


#endif /* SIM_MAIN_H */
