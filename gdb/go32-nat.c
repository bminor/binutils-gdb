/* Native debugging support for Intel x86 running DJGPP.
   Copyright 1997, 1999 Free Software Foundation, Inc.
   Written by Robert Hoehne.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <fcntl.h>

#include "defs.h"
#include "frame.h"		/* required by inferior.h */
#include "inferior.h"
#include "target.h"
#include "wait.h"
#include "gdbcore.h"
#include "command.h"
#include "floatformat.h"

#include <stdio.h>     /* required for __DJGPP_MINOR__ */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <debug/v2load.h>
#include <debug/dbgcom.h>

#if __DJGPP_MINOR__ < 3
/* This code will be provided from DJGPP 2.03 on. Until then I code it
   here */
typedef struct {
  unsigned short sig0;
  unsigned short sig1;
  unsigned short sig2;
  unsigned short sig3;
  unsigned short exponent:15;
  unsigned short sign:1;
} NPXREG;

typedef struct {
  unsigned int control;
  unsigned int status;
  unsigned int tag;
  unsigned int eip;
  unsigned int cs;
  unsigned int dataptr;
  unsigned int datasel;
  NPXREG reg[8];
} NPX;

static NPX npx;

static void save_npx (void); /* Save the FPU of the debugged program */
static void load_npx (void); /* Restore the FPU of the debugged program */

/* ------------------------------------------------------------------------- */
/* Store the contents of the NPX in the global variable `npx'.  */

static void
save_npx (void)
{
  asm ("inb    $0xa0, %%al
       testb   $0x20, %%al
       jz      1f
       xorb    %%al, %%al
       outb    %%al, $0xf0
       movb    $0x20, %%al
       outb    %%al, $0xa0
       outb    %%al, $0x20
1:
       fnsave  %0
       fwait"
       : "=m" (npx)
       : /* No input */
       : "%eax");
}
/* ------------------------------------------------------------------------- */
/* Reload the contents of the NPX from the global variable `npx'.  */

static void
load_npx (void)
{
  asm ("frstor %0" : "=m" (npx));
}
#endif /* __DJGPP_MINOR < 3 */

extern void _initialize_go32_nat (void);

struct env387
{
  unsigned short control;
  unsigned short r0;
  unsigned short status;
  unsigned short r1;
  unsigned short tag;
  unsigned short r2;
  unsigned long eip;
  unsigned short code_seg;
  unsigned short opcode;
  unsigned long operand;
  unsigned short operand_seg;
  unsigned short r3;
  unsigned char regs[8][10];
};

extern char **environ;

#define SOME_PID 42

static int prog_has_started = 0;
static void
print_387_status (unsigned short status, struct env387 *ep);
static void
go32_open (char *name, int from_tty);
static void
go32_close (int quitting);
static void
go32_attach (char *args, int from_tty);
static void
go32_detach (char *args, int from_tty);
static void
go32_resume (int pid, int step, enum target_signal siggnal);
static int
go32_wait (int pid, struct target_waitstatus *status);
static void
go32_fetch_registers (int regno);
static void
store_register (int regno);
static void
go32_store_registers (int regno);
static void
go32_prepare_to_store (void);
static int
go32_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
                 struct target_ops *target);
static void
go32_files_info (struct target_ops *target);
static void
go32_stop (void);
static void
go32_kill_inferior (void);
static void
go32_create_inferior (char *exec_file, char *args, char **env);
static void
go32_mourn_inferior (void);
static int
go32_can_run (void);
static void
ignore (void);
static void
ignore2 (char *a, int b);
static int go32_insert_aligned_watchpoint (int pid, CORE_ADDR waddr,
                                          CORE_ADDR addr, int len, int rw);
static int go32_insert_nonaligned_watchpoint (int pid, CORE_ADDR waddr,
                                             CORE_ADDR addr, int len, int rw);

static struct target_ops go32_ops;
static void
go32_terminal_init (void);
static void
go32_terminal_inferior (void);
static void
go32_terminal_ours (void);

static void
print_387_status (unsigned short status, struct env387 *ep)
{
  int i;
  int bothstatus;
  int top;
  int fpreg;

  bothstatus = ((status != 0) && (ep->status != 0));
  if (status != 0)
    {
      if (bothstatus)
	printf_unfiltered ("u: ");
      print_387_status_word (status);
    }

  if (ep->status != 0)
    {
      if (bothstatus)
	printf_unfiltered ("e: ");
      print_387_status_word (ep->status);
    }

  print_387_control_word (ep->control & 0xffff);
  printf_unfiltered ("last exception: ");
  printf_unfiltered ("opcode %s; ", local_hex_string (ep->opcode));
  printf_unfiltered ("pc %s:", local_hex_string (ep->code_seg));
  printf_unfiltered ("%s; ", local_hex_string (ep->eip));
  printf_unfiltered ("operand %s", local_hex_string (ep->operand_seg));
  printf_unfiltered (":%s\n", local_hex_string (ep->operand));

  top = (ep->status >> 11) & 7;

  printf_unfiltered ("regno tag   msb          lsb  value\n");
  for (fpreg = 0; fpreg < 8; fpreg++)
    {
      long double val;

      printf_unfiltered ("%s %d: ", fpreg == top ? "=>" : "  ", fpreg);

      switch ((ep->tag >> (fpreg * 2)) & 3)
	{
	case 0:
	  printf_unfiltered ("valid ");
	  break;
	case 1:
	  printf_unfiltered ("zero  ");
	  break;
	case 2:
	  printf_unfiltered ("trap  ");
	  break;
	case 3:
	  printf_unfiltered ("empty ");
	  break;
	}
      for (i = 0; i < 8; i++)
	printf_unfiltered ("%02x", ep->regs[fpreg][i]);

      REGISTER_CONVERT_TO_VIRTUAL (FP0_REGNUM + fpreg, builtin_type_long_double,
				   &ep->regs[fpreg], &val);

      printf_unfiltered ("  %LG\n", val);
    }
}

void
i386_go32_float_info (void)
{
  print_387_status (0, (struct env387 *) &npx);
}

#define r_ofs(x) ((int)(&(((TSS *)0)->x)))

static struct
{
  int tss_ofs;
  int size;
}
regno_mapping[] =
{
  r_ofs (tss_eax), 4,
  r_ofs (tss_ecx), 4,
  r_ofs (tss_edx), 4,
  r_ofs (tss_ebx), 4,
  r_ofs (tss_esp), 4,
  r_ofs (tss_ebp), 4,
  r_ofs (tss_esi), 4,
  r_ofs (tss_edi), 4,
  r_ofs (tss_eip), 4,
  r_ofs (tss_eflags), 4,
  r_ofs (tss_cs), 2,
  r_ofs (tss_ss), 2,
  r_ofs (tss_ds), 2,
  r_ofs (tss_es), 2,
  r_ofs (tss_fs), 2,
  r_ofs (tss_gs), 2,
  0, 10,
  1, 10,
  2, 10,
  3, 10,
  4, 10,
  5, 10,
  6, 10,
  7, 10,
  0, 2,
  4, 2,
  8, 2,
  12, 4,
  16, 2,
  20, 4,
  24, 2
};

static struct
  {
    int go32_sig;
    int gdb_sig;
  }
sig_map[] =
{
  0, TARGET_SIGNAL_FPE,
  1, TARGET_SIGNAL_TRAP,
  2, TARGET_SIGNAL_UNKNOWN,
  3, TARGET_SIGNAL_TRAP,
  4, TARGET_SIGNAL_FPE,
  5, TARGET_SIGNAL_SEGV,
  6, TARGET_SIGNAL_ILL,
  7, TARGET_SIGNAL_FPE,
  8, TARGET_SIGNAL_SEGV,
  9, TARGET_SIGNAL_SEGV,
  10, TARGET_SIGNAL_BUS,
  11, TARGET_SIGNAL_SEGV,
  12, TARGET_SIGNAL_SEGV,
  13, TARGET_SIGNAL_ABRT,
  14, TARGET_SIGNAL_SEGV,
  16, TARGET_SIGNAL_FPE,
  31, TARGET_SIGNAL_ILL,
  0x75, TARGET_SIGNAL_FPE,
  0x79, TARGET_SIGNAL_INT,
  0x1b, TARGET_SIGNAL_INT,
  -1, -1
};

static void
go32_open (char *name, int from_tty)
{
  printf_unfiltered ("Use the `run' command to run go32 programs\n");
}

static void
go32_close (int quitting)
{
}

static void
go32_attach (char *args, int from_tty)
{
  printf_unfiltered ("Use the `run' command to run go32 programs\n");
}

static void
go32_detach (char *args, int from_tty)
{
}

static int resume_is_step;

static void
go32_resume (int pid, int step, enum target_signal siggnal)
  {
    resume_is_step = step;
  }

static int
go32_wait (int pid, struct target_waitstatus *status)
{
  int i;

  if (resume_is_step)
    a_tss.tss_eflags |= 0x0100;
  else
    a_tss.tss_eflags &= 0xfeff;

#if __DJGPP_MINOR__ < 3
  save_npx ();
#endif
  run_child ();
#if __DJGPP_MINOR__ < 3
  load_npx ();
#endif

  if (a_tss.tss_irqn == 0x21)
    {
      status->kind = TARGET_WAITKIND_EXITED;
      status->value.integer = a_tss.tss_eax & 0xff;
    }
  else
    {
      status->value.sig = TARGET_SIGNAL_UNKNOWN;
      status->kind = TARGET_WAITKIND_STOPPED;
      for (i = 0; sig_map[i].go32_sig != -1; i++)
	{
	  if (a_tss.tss_irqn == sig_map[i].go32_sig)
	    {
	      if ((status->value.sig = sig_map[i].gdb_sig) !=
		  TARGET_SIGNAL_TRAP)
		status->kind = TARGET_WAITKIND_SIGNALLED;
	      break;
	    }
	}
    }
  return SOME_PID;
}

static void
go32_fetch_registers (int regno)
{
  /*JHW*/
  int end_reg = regno + 1;	/* just one reg initially */

  if (regno < 0)		/* do the all registers */
    {
      regno = 0;		/* start at first register */
      /* # regs in table */
      end_reg = sizeof (regno_mapping) / sizeof (regno_mapping[0]);
    }

  for (; regno < end_reg; regno++)
    {
      if (regno < 16)
	supply_register (regno,
			 (char *) &a_tss + regno_mapping[regno].tss_ofs);
      else if (regno < 24)
	supply_register (regno,
			 (char *) &npx.reg[regno_mapping[regno].tss_ofs]);
      else if (regno < 31)
	supply_register (regno,
			 (char *) &npx.reg + regno_mapping[regno].tss_ofs);
      else
	{
	  printf_unfiltered ("Invalid register in go32_fetch_register(%d)",
			     regno);
	  exit (1);
	}
    }
}

static void
store_register (int regno)
{
  void *rp;
  void *v = (void *) &registers[REGISTER_BYTE (regno)];

  if (regno < 16)
    rp = (char *) &a_tss + regno_mapping[regno].tss_ofs;
  else if (regno < 24)
    rp = (char *) &npx.reg[regno_mapping[regno].tss_ofs];
  else if (regno > 31)
    rp = (char *) &npx + regno_mapping[regno].tss_ofs;
  else
    {
      printf_unfiltered ("Invalid register in store_register(%d)", regno);
      exit (1);
    }
  memcpy (rp, v, regno_mapping[regno].size);
}

static void
go32_store_registers (int regno)
{
  int r;

  if (regno >= 0)
    store_register (regno);
  else
    {
      for (r = 0; r < sizeof (regno_mapping) / sizeof (regno_mapping[0]); r++)
	store_register (r);
    }
}

static void
go32_prepare_to_store (void)
{
}

static int
go32_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
		  struct target_ops *target)
{
  if (write)
    {
      if (write_child (memaddr, myaddr, len))
	{
	  return 0;
	}
      else
	{
	  return len;
	}
    }
  else
    {
      if (read_child (memaddr, myaddr, len))
	{
	  return 0;
	}
      else
	{
	  return len;
	}
    }
}

static void
go32_files_info (struct target_ops *target)
{
  printf_unfiltered ("You are running a DJGPP V2 program\n");
}

static void
go32_stop (void)
{
  normal_stop ();
  cleanup_client ();
  inferior_pid = 0;
  prog_has_started = 0;
}

static void
go32_kill_inferior (void)
{
  unpush_target (&go32_ops);
}

static void
go32_create_inferior (char *exec_file, char *args, char **env)
{
  jmp_buf start_state;
  char *cmdline;
  char **env_save = environ;

  if (prog_has_started)
    {
      go32_stop ();
      go32_kill_inferior ();
    }

  cmdline = (char *) alloca (strlen (args) + 4);
  cmdline[0] = strlen (args);
  strcpy (cmdline + 1, args);
  cmdline[strlen (args) + 1] = 13;

  environ = env;

  if (v2loadimage (exec_file, cmdline, start_state))
    {
      environ = env_save;
      printf_unfiltered ("Load failed for image %s\n", exec_file);
      exit (1);
    }
  environ = env_save;

  edi_init (start_state);

  inferior_pid = SOME_PID;
  push_target (&go32_ops);
  clear_proceed_status ();
  insert_breakpoints ();
  proceed ((CORE_ADDR) - 1, TARGET_SIGNAL_0, 0);
  prog_has_started = 1;
}

static void
go32_mourn_inferior (void)
{
  go32_kill_inferior ();
  generic_mourn_inferior ();
}

static int
go32_can_run (void)
{
  return 1;
}

static void
ignore (void)
{
}

static void
ignore2 (char *a, int b)
{
}

/* Hardware watchpoint support.  */

#define DR_STATUS 6
#define DR_CONTROL 7
#define DR_ENABLE_SIZE 2
#define DR_LOCAL_ENABLE_SHIFT 0
#define DR_GLOBAL_ENABLE_SHIFT 1
#define DR_LOCAL_SLOWDOWN 0x100
#define DR_GLOBAL_SLOWDOWN 0x200
#define DR_CONTROL_SHIFT 16
#define DR_CONTROL_SIZE 4
#define DR_RW_READ 0x3
#define DR_RW_WRITE 0x1
#define DR_CONTROL_MASK 0xf
#define DR_ENABLE_MASK 0x3
#define DR_LEN_1 0x0
#define DR_LEN_2 0x4
#define DR_LEN_4 0xc

#define D_REGS edi.dr
#define CONTROL D_REGS[DR_CONTROL]
#define STATUS D_REGS[DR_STATUS]

#define IS_REG_FREE(index) \
  (!(CONTROL & (3 << (DR_ENABLE_SIZE * index))))

#define LOCAL_ENABLE_REG(index) \
  (CONTROL |= (1 << (DR_LOCAL_ENABLE_SHIFT + DR_ENABLE_SIZE * index)))

#define GLOBAL_ENABLE_REG(index) \
  (CONTROL |= (1 << (DR_GLOBAL_ENABLE_SHIFT + DR_ENABLE_SIZE * index)))

#define DISABLE_REG(index) \
  (CONTROL &= ~(3 << (DR_ENABLE_SIZE * index)))

#define SET_LOCAL_EXACT() \
  (CONTROL |= DR_LOCAL_SLOWDOWN)

#define SET_GLOBAL_EXACT() \
  (CONTROL |= DR_GLOBAL_SLOWDOWN)

#define SET_BREAK(index,address) \
  do {\
    CONTROL &= ~(DR_CONTROL_MASK << (DR_CONTROL_SHIFT + DR_CONTROL_SIZE * index));\
    D_REGS[index] = address;\
  } while(0)

#define SET_WATCH(index,address,rw,len) \
  do {\
    SET_BREAK(index,address);\
    CONTROL |= (len | rw) << (DR_CONTROL_SHIFT + DR_CONTROL_SIZE * index);\
  } while (0)

#define WATCH_HIT(index) \
  (\
   (STATUS & (1 << index)) && \
   (CONTROL & (DR_CONTROL_MASK << (DR_CONTROL_SHIFT + DR_CONTROL_SIZE * index)))\
  )

#if 0 /* use debugging macro */
#define SHOW_DR(text) \
do { \
  fprintf(stderr,"%08x %08x ",edi.dr[7],edi.dr[6]); \
  fprintf(stderr,"%08x %08x ",edi.dr[0],edi.dr[1]); \
  fprintf(stderr,"%08x %08x ",edi.dr[2],edi.dr[3]); \
  fprintf(stderr,"(%s)\n",#text); \
} while (0)
#else
#define SHOW_DR(text) do {} while (0)
#endif

/* Insert a watchpoint.  */

int
go32_insert_watchpoint (int pid, CORE_ADDR addr, int len, int rw)
{
  int ret = go32_insert_aligned_watchpoint (pid, addr, addr, len, rw);

  SHOW_DR (insert_watch);
  return ret;
}

static int
go32_insert_aligned_watchpoint (int pid, CORE_ADDR waddr, CORE_ADDR addr,
				int len, int rw)
{
  int i;
  int read_write_bits, len_bits;

  /* Look for a free debug register.  */
  for (i = 0; i <= 3; i++)
    {
      if (IS_REG_FREE (i))
	break;
    }

  /* No more debug registers!  */
  if (i > 3)
    return -1;

  read_write_bits = ((rw & 1) ? DR_RW_READ : 0) | ((rw & 2) ? DR_RW_WRITE : 0);

  if (len == 1)
    len_bits = DR_LEN_1;
  else if (len == 2)
    {
      if (addr % 2)
	return go32_insert_nonaligned_watchpoint (pid, waddr, addr, len, rw);
      len_bits = DR_LEN_2;
    }
  else if (len == 4)
    {
      if (addr % 4)
	return go32_insert_nonaligned_watchpoint (pid, waddr, addr, len, rw);
      len_bits = DR_LEN_4;
    }
  else
    return go32_insert_nonaligned_watchpoint (pid, waddr, addr, len, rw);

  SET_WATCH (i, addr, read_write_bits, len_bits);
  LOCAL_ENABLE_REG (i);
  SET_LOCAL_EXACT ();
}

static int
go32_insert_nonaligned_watchpoint (int pid, CORE_ADDR waddr, CORE_ADDR addr,
				   int len, int rw)
{
  int align;
  int size;
  int rv = 0;

  static int size_try_array[16] =
  {
    1, 1, 1, 1,			/* trying size one */
    2, 1, 2, 1,			/* trying size two */
    2, 1, 2, 1,			/* trying size three */
    4, 1, 2, 1			/* trying size four */
  };

  while (len > 0)
    {
      align = addr % 4;
      /* Four is the maximum length for 386.  */
      size = (len > 4) ? 3 : len - 1;
      size = size_try_array[size * 4 + align];
      rv = go32_insert_aligned_watchpoint (pid, waddr, addr, size, rw);
      if (rv)
	{
	  go32_remove_watchpoint (pid, waddr, size);
	  return rv;
	}
      addr += size;
      len -= size;
    }
  return rv;
}

/* Remove a watchpoint.  */

int
go32_remove_watchpoint (int pid, CORE_ADDR addr, int len)
{
  int i;

  for (i = 0; i <= 3; i++)
    {
      if (D_REGS[i] == addr)
	{
	  DISABLE_REG (i);
	}
    }
  SHOW_DR (remove_watch);

  return 0;
}

/* Check if stopped by a watchpoint.  */

CORE_ADDR
go32_stopped_by_watchpoint (int pid)
{
  int i, ret = 0;
  int status;

  status = edi.dr[DR_STATUS];
  SHOW_DR (stopped_by);
  for (i = 0; i <= 3; i++)
    {
      if (WATCH_HIT (i))
	{
	  SHOW_DR (HIT);
	  ret = D_REGS[i];
	}
    }
  /* this is a hack to GDB. If we stopped at a hardware breakpoint,
     the stop_pc must incremented by DECR_PC_AFTER_BREAK. I tried everything
     with the DECR_PC_AFTER_HW_BREAK, but nothing works. */
  /* This is probably fixed by jtc's recent patch -sts 2/19/99 */
  if (STATUS && !ret)
    stop_pc += DECR_PC_AFTER_BREAK;
  STATUS = 0;

  return ret;
}

/* Remove a breakpoint.  */

int
go32_remove_hw_breakpoint (CORE_ADDR addr, CORE_ADDR shadow)
{
  int i;
  for (i = 0; i <= 3; i++)
    {
      if (D_REGS[i] == addr)
	{
	  DISABLE_REG (i);
	}
    }
  SHOW_DR (remove_hw);
  return 0;
}

int
go32_insert_hw_breakpoint (CORE_ADDR addr, CORE_ADDR shadow)
{
  int i;
  int read_write_bits, len_bits;
  int free_debug_register;
  int register_number;

  /* Look for a free debug register.  */
  for (i = 0; i <= 3; i++)
    {
      if (IS_REG_FREE (i))
	break;
    }

  /* No more debug registers!  */
  if (i > 3)
    return -1;

  SET_BREAK (i, addr);
  LOCAL_ENABLE_REG (i);
  SHOW_DR (insert_hw);

  return 0;
}

static int inf_flags_valid = 0;
static int inf_in_flag;
static int inf_out_flag;

static void
go32_terminal_init (void)
{
  /* Save the filemodes for stdin/stout */
  inf_in_flag = setmode(0, 0);
  setmode(0, inf_in_flag);
  inf_out_flag = setmode(1, 0);
  setmode(1, inf_out_flag);
  inf_flags_valid = 1;
}

static void
go32_terminal_inferior (void)
{
  /* set the filemodes for stdin/stdout of the inferior */
  if (inf_flags_valid)
  {
    setmode(0, inf_in_flag);
    setmode(1, inf_out_flag);
  }
}

static void
go32_terminal_ours (void)
{
  /* Switch to text mode on stdin/stdout always on the gdb terminal and
     save the inferior modes to be restored later */
  inf_in_flag = setmode(0, O_TEXT);
  inf_out_flag = setmode(1, O_TEXT);
}

static void
init_go32_ops (void)
{
  go32_ops.to_shortname = "djgpp";
  go32_ops.to_longname = "djgpp target process";
  go32_ops.to_doc =
    "Program loaded by djgpp, when gdb is used as an external debugger";
  go32_ops.to_open = go32_open;
  go32_ops.to_close = go32_close;
  go32_ops.to_detach = go32_detach;
  go32_ops.to_resume = go32_resume;
  go32_ops.to_wait = go32_wait;
  go32_ops.to_fetch_registers = go32_fetch_registers;
  go32_ops.to_store_registers = go32_store_registers;
  go32_ops.to_prepare_to_store = go32_prepare_to_store;
  go32_ops.to_xfer_memory = go32_xfer_memory;
  go32_ops.to_files_info = go32_files_info;
  go32_ops.to_insert_breakpoint = memory_insert_breakpoint;
  go32_ops.to_remove_breakpoint = memory_remove_breakpoint;
  go32_ops.to_terminal_init = go32_terminal_init;
  go32_ops.to_terminal_inferior = go32_terminal_inferior;
  go32_ops.to_terminal_ours_for_output = ignore;
  go32_ops.to_terminal_ours = go32_terminal_ours;
  go32_ops.to_terminal_info = ignore2;
  go32_ops.to_kill = go32_kill_inferior;
  go32_ops.to_create_inferior = go32_create_inferior;
  go32_ops.to_mourn_inferior = go32_mourn_inferior;
  go32_ops.to_can_run = go32_can_run;
  go32_ops.to_stop = go32_stop;
  go32_ops.to_stratum = process_stratum;
  go32_ops.to_has_all_memory = 1;
  go32_ops.to_has_memory = 1;
  go32_ops.to_has_stack = 1;
  go32_ops.to_has_registers = 1;
  go32_ops.to_has_execution = 1;
  go32_ops.to_magic = OPS_MAGIC;
}

void
_initialize_go32_nat (void)
{
  init_go32_ops ();
  add_target (&go32_ops);
}
