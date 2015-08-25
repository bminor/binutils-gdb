/* GNU/Linux/AArch64 specific low level interface, for the remote server for
   GDB.

   Copyright (C) 2009-2015 Free Software Foundation, Inc.
   Contributed by ARM Ltd.

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

#include "server.h"
#include "linux-low.h"
#include "nat/aarch64-linux.h"
#include "nat/aarch64-linux-hw-point.h"
#include "linux-aarch32-low.h"
#include "elf/common.h"

#include <signal.h>
#include <sys/user.h>
#include "nat/gdb_ptrace.h"
#include <asm/ptrace.h>
#include <sys/uio.h>

#include "gdb_proc_service.h"

/* Defined in auto-generated files.  */
void init_registers_aarch64 (void);
extern const struct target_desc *tdesc_aarch64;

#ifdef HAVE_SYS_REG_H
#include <sys/reg.h>
#endif

#define AARCH64_X_REGS_NUM 31
#define AARCH64_V_REGS_NUM 32
#define AARCH64_X0_REGNO    0
#define AARCH64_SP_REGNO   31
#define AARCH64_PC_REGNO   32
#define AARCH64_CPSR_REGNO 33
#define AARCH64_V0_REGNO   34
#define AARCH64_FPSR_REGNO (AARCH64_V0_REGNO + AARCH64_V_REGS_NUM)
#define AARCH64_FPCR_REGNO (AARCH64_V0_REGNO + AARCH64_V_REGS_NUM + 1)

#define AARCH64_NUM_REGS (AARCH64_V0_REGNO + AARCH64_V_REGS_NUM + 2)

/* Per-process arch-specific data we want to keep.  */

struct arch_process_info
{
  /* Hardware breakpoint/watchpoint data.
     The reason for them to be per-process rather than per-thread is
     due to the lack of information in the gdbserver environment;
     gdbserver is not told that whether a requested hardware
     breakpoint/watchpoint is thread specific or not, so it has to set
     each hw bp/wp for every thread in the current process.  The
     higher level bp/wp management in gdb will resume a thread if a hw
     bp/wp trap is not expected for it.  Since the hw bp/wp setting is
     same for each thread, it is reasonable for the data to live here.
     */
  struct aarch64_debug_reg_state debug_reg_state;
};

/* Return true if the size of register 0 is 8 byte.  */

static int
is_64bit_tdesc (void)
{
  struct regcache *regcache = get_thread_regcache (current_thread, 0);

  return register_size (regcache->tdesc, 0) == 8;
}

/* Implementation of linux_target_ops method "cannot_store_register".  */

static int
aarch64_cannot_store_register (int regno)
{
  return regno >= AARCH64_NUM_REGS;
}

/* Implementation of linux_target_ops method "cannot_fetch_register".  */

static int
aarch64_cannot_fetch_register (int regno)
{
  return regno >= AARCH64_NUM_REGS;
}

static void
aarch64_fill_gregset (struct regcache *regcache, void *buf)
{
  struct user_pt_regs *regset = buf;
  int i;

  for (i = 0; i < AARCH64_X_REGS_NUM; i++)
    collect_register (regcache, AARCH64_X0_REGNO + i, &regset->regs[i]);
  collect_register (regcache, AARCH64_SP_REGNO, &regset->sp);
  collect_register (regcache, AARCH64_PC_REGNO, &regset->pc);
  collect_register (regcache, AARCH64_CPSR_REGNO, &regset->pstate);
}

static void
aarch64_store_gregset (struct regcache *regcache, const void *buf)
{
  const struct user_pt_regs *regset = buf;
  int i;

  for (i = 0; i < AARCH64_X_REGS_NUM; i++)
    supply_register (regcache, AARCH64_X0_REGNO + i, &regset->regs[i]);
  supply_register (regcache, AARCH64_SP_REGNO, &regset->sp);
  supply_register (regcache, AARCH64_PC_REGNO, &regset->pc);
  supply_register (regcache, AARCH64_CPSR_REGNO, &regset->pstate);
}

static void
aarch64_fill_fpregset (struct regcache *regcache, void *buf)
{
  struct user_fpsimd_state *regset = buf;
  int i;

  for (i = 0; i < AARCH64_V_REGS_NUM; i++)
    collect_register (regcache, AARCH64_V0_REGNO + i, &regset->vregs[i]);
  collect_register (regcache, AARCH64_FPSR_REGNO, &regset->fpsr);
  collect_register (regcache, AARCH64_FPCR_REGNO, &regset->fpcr);
}

static void
aarch64_store_fpregset (struct regcache *regcache, const void *buf)
{
  const struct user_fpsimd_state *regset = buf;
  int i;

  for (i = 0; i < AARCH64_V_REGS_NUM; i++)
    supply_register (regcache, AARCH64_V0_REGNO + i, &regset->vregs[i]);
  supply_register (regcache, AARCH64_FPSR_REGNO, &regset->fpsr);
  supply_register (regcache, AARCH64_FPCR_REGNO, &regset->fpcr);
}

/* Enable miscellaneous debugging output.  The name is historical - it
   was originally used to debug LinuxThreads support.  */
extern int debug_threads;

/* Implementation of linux_target_ops method "get_pc".  */

static CORE_ADDR
aarch64_get_pc (struct regcache *regcache)
{
  if (register_size (regcache->tdesc, 0) == 8)
    {
      unsigned long pc;

      collect_register_by_name (regcache, "pc", &pc);
      if (debug_threads)
	debug_printf ("stop pc is %08lx\n", pc);
      return pc;
    }
  else
    {
      unsigned int pc;

      collect_register_by_name (regcache, "pc", &pc);
      if (debug_threads)
	debug_printf ("stop pc is %04x\n", pc);
      return pc;
    }
}

/* Implementation of linux_target_ops method "set_pc".  */

static void
aarch64_set_pc (struct regcache *regcache, CORE_ADDR pc)
{
  if (register_size (regcache->tdesc, 0) == 8)
    {
      unsigned long newpc = pc;
      supply_register_by_name (regcache, "pc", &newpc);
    }
  else
    {
      unsigned int newpc = pc;
      supply_register_by_name (regcache, "pc", &newpc);
    }
}

#define aarch64_breakpoint_len 4

/* AArch64 BRK software debug mode instruction.
   This instruction needs to match gdb/aarch64-tdep.c
   (aarch64_default_breakpoint).  */
static const gdb_byte aarch64_breakpoint[] = {0x00, 0x00, 0x20, 0xd4};

/* Implementation of linux_target_ops method "breakpoint_at".  */

static int
aarch64_breakpoint_at (CORE_ADDR where)
{
  gdb_byte insn[aarch64_breakpoint_len];

  (*the_target->read_memory) (where, (unsigned char *) &insn,
			      aarch64_breakpoint_len);
  if (memcmp (insn, aarch64_breakpoint, aarch64_breakpoint_len) == 0)
    return 1;

  return 0;
}

static void
aarch64_init_debug_reg_state (struct aarch64_debug_reg_state *state)
{
  int i;

  for (i = 0; i < AARCH64_HBP_MAX_NUM; ++i)
    {
      state->dr_addr_bp[i] = 0;
      state->dr_ctrl_bp[i] = 0;
      state->dr_ref_count_bp[i] = 0;
    }

  for (i = 0; i < AARCH64_HWP_MAX_NUM; ++i)
    {
      state->dr_addr_wp[i] = 0;
      state->dr_ctrl_wp[i] = 0;
      state->dr_ref_count_wp[i] = 0;
    }
}

/* Return the pointer to the debug register state structure in the
   current process' arch-specific data area.  */

struct aarch64_debug_reg_state *
aarch64_get_debug_reg_state (pid_t pid)
{
  struct process_info *proc = find_process_pid (pid);

  return &proc->priv->arch_private->debug_reg_state;
}

/* Implementation of linux_target_ops method "supports_z_point_type".  */

static int
aarch64_supports_z_point_type (char z_type)
{
  switch (z_type)
    {
    case Z_PACKET_SW_BP:
      {
	if (!extended_protocol && is_64bit_tdesc ())
	  {
	    /* Only enable Z0 packet in non-multi-arch debugging.  If
	       extended protocol is used, don't enable Z0 packet because
	       GDBserver may attach to 32-bit process.  */
	    return 1;
	  }
	else
	  {
	    /* Disable Z0 packet so that GDBserver doesn't have to handle
	       different breakpoint instructions (aarch64, arm, thumb etc)
	       in multi-arch debugging.  */
	    return 0;
	  }
      }
    case Z_PACKET_HW_BP:
    case Z_PACKET_WRITE_WP:
    case Z_PACKET_READ_WP:
    case Z_PACKET_ACCESS_WP:
      return 1;
    default:
      return 0;
    }
}

/* Implementation of linux_target_ops method "insert_point".

   It actually only records the info of the to-be-inserted bp/wp;
   the actual insertion will happen when threads are resumed.  */

static int
aarch64_insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
		      int len, struct raw_breakpoint *bp)
{
  int ret;
  enum target_hw_bp_type targ_type;
  struct aarch64_debug_reg_state *state
    = aarch64_get_debug_reg_state (pid_of (current_thread));

  if (show_debug_regs)
    fprintf (stderr, "insert_point on entry (addr=0x%08lx, len=%d)\n",
	     (unsigned long) addr, len);

  /* Determine the type from the raw breakpoint type.  */
  targ_type = raw_bkpt_type_to_target_hw_bp_type (type);

  if (targ_type != hw_execute)
    ret =
      aarch64_handle_watchpoint (targ_type, addr, len, 1 /* is_insert */,
				 state);
  else
    ret =
      aarch64_handle_breakpoint (targ_type, addr, len, 1 /* is_insert */,
				 state);

  if (show_debug_regs)
    aarch64_show_debug_reg_state (state, "insert_point", addr, len,
				  targ_type);

  return ret;
}

/* Implementation of linux_target_ops method "remove_point".

   It actually only records the info of the to-be-removed bp/wp,
   the actual removal will be done when threads are resumed.  */

static int
aarch64_remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
		      int len, struct raw_breakpoint *bp)
{
  int ret;
  enum target_hw_bp_type targ_type;
  struct aarch64_debug_reg_state *state
    = aarch64_get_debug_reg_state (pid_of (current_thread));

  if (show_debug_regs)
    fprintf (stderr, "remove_point on entry (addr=0x%08lx, len=%d)\n",
	     (unsigned long) addr, len);

  /* Determine the type from the raw breakpoint type.  */
  targ_type = raw_bkpt_type_to_target_hw_bp_type (type);

  /* Set up state pointers.  */
  if (targ_type != hw_execute)
    ret =
      aarch64_handle_watchpoint (targ_type, addr, len, 0 /* is_insert */,
				 state);
  else
    ret =
      aarch64_handle_breakpoint (targ_type, addr, len, 0 /* is_insert */,
				 state);

  if (show_debug_regs)
    aarch64_show_debug_reg_state (state, "remove_point", addr, len,
				  targ_type);

  return ret;
}

/* Implementation of linux_target_ops method "stopped_data_address".  */

static CORE_ADDR
aarch64_stopped_data_address (void)
{
  siginfo_t siginfo;
  int pid, i;
  struct aarch64_debug_reg_state *state;

  pid = lwpid_of (current_thread);

  /* Get the siginfo.  */
  if (ptrace (PTRACE_GETSIGINFO, pid, NULL, &siginfo) != 0)
    return (CORE_ADDR) 0;

  /* Need to be a hardware breakpoint/watchpoint trap.  */
  if (siginfo.si_signo != SIGTRAP
      || (siginfo.si_code & 0xffff) != 0x0004 /* TRAP_HWBKPT */)
    return (CORE_ADDR) 0;

  /* Check if the address matches any watched address.  */
  state = aarch64_get_debug_reg_state (pid_of (current_thread));
  for (i = aarch64_num_wp_regs - 1; i >= 0; --i)
    {
      const unsigned int len = aarch64_watchpoint_length (state->dr_ctrl_wp[i]);
      const CORE_ADDR addr_trap = (CORE_ADDR) siginfo.si_addr;
      const CORE_ADDR addr_watch = state->dr_addr_wp[i];
      if (state->dr_ref_count_wp[i]
	  && DR_CONTROL_ENABLED (state->dr_ctrl_wp[i])
	  && addr_trap >= addr_watch
	  && addr_trap < addr_watch + len)
	return addr_trap;
    }

  return (CORE_ADDR) 0;
}

/* Implementation of linux_target_ops method "stopped_by_watchpoint".  */

static int
aarch64_stopped_by_watchpoint (void)
{
  if (aarch64_stopped_data_address () != 0)
    return 1;
  else
    return 0;
}

/* Fetch the thread-local storage pointer for libthread_db.  */

ps_err_e
ps_get_thread_area (const struct ps_prochandle *ph,
		    lwpid_t lwpid, int idx, void **base)
{
  struct iovec iovec;
  uint64_t reg;

  iovec.iov_base = &reg;
  iovec.iov_len = sizeof (reg);

  if (ptrace (PTRACE_GETREGSET, lwpid, NT_ARM_TLS, &iovec) != 0)
    return PS_ERR;

  /* IDX is the bias from the thread pointer to the beginning of the
     thread descriptor.  It has to be subtracted due to implementation
     quirks in libthread_db.  */
  *base = (void *) (reg - idx);

  return PS_OK;
}

/* Implementation of linux_target_ops method "linux_new_process".  */

static struct arch_process_info *
aarch64_linux_new_process (void)
{
  struct arch_process_info *info = xcalloc (1, sizeof (*info));

  aarch64_init_debug_reg_state (&info->debug_reg_state);

  return info;
}

/* Implementation of linux_target_ops method "linux_new_fork".  */

static void
aarch64_linux_new_fork (struct process_info *parent,
			struct process_info *child)
{
  /* These are allocated by linux_add_process.  */
  gdb_assert (parent->priv != NULL
	      && parent->priv->arch_private != NULL);
  gdb_assert (child->priv != NULL
	      && child->priv->arch_private != NULL);

  /* Linux kernel before 2.6.33 commit
     72f674d203cd230426437cdcf7dd6f681dad8b0d
     will inherit hardware debug registers from parent
     on fork/vfork/clone.  Newer Linux kernels create such tasks with
     zeroed debug registers.

     GDB core assumes the child inherits the watchpoints/hw
     breakpoints of the parent, and will remove them all from the
     forked off process.  Copy the debug registers mirrors into the
     new process so that all breakpoints and watchpoints can be
     removed together.  The debug registers mirror will become zeroed
     in the end before detaching the forked off process, thus making
     this compatible with older Linux kernels too.  */

  *child->priv->arch_private = *parent->priv->arch_private;
}

/* Return the right target description according to the ELF file of
   current thread.  */

static const struct target_desc *
aarch64_linux_read_description (void)
{
  unsigned int machine;
  int is_elf64;
  int tid;

  tid = lwpid_of (current_thread);

  is_elf64 = linux_pid_exe_is_elf_64_file (tid, &machine);

  if (is_elf64)
    return tdesc_aarch64;
  else
    return tdesc_arm_with_neon;
}

/* Implementation of linux_target_ops method "arch_setup".  */

static void
aarch64_arch_setup (void)
{
  current_process ()->tdesc = aarch64_linux_read_description ();

  aarch64_linux_get_debug_reg_capacity (lwpid_of (current_thread));
}

static struct regset_info aarch64_regsets[] =
{
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_PRSTATUS,
    sizeof (struct user_pt_regs), GENERAL_REGS,
    aarch64_fill_gregset, aarch64_store_gregset },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_FPREGSET,
    sizeof (struct user_fpsimd_state), FP_REGS,
    aarch64_fill_fpregset, aarch64_store_fpregset
  },
  { 0, 0, 0, -1, -1, NULL, NULL }
};

static struct regsets_info aarch64_regsets_info =
  {
    aarch64_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

static struct regs_info regs_info_aarch64 =
  {
    NULL, /* regset_bitmap */
    NULL, /* usrregs */
    &aarch64_regsets_info,
  };

/* Implementation of linux_target_ops method "regs_info".  */

static const struct regs_info *
aarch64_regs_info (void)
{
  if (is_64bit_tdesc ())
    return &regs_info_aarch64;
  else
    return &regs_info_aarch32;
}

/* Implementation of linux_target_ops method "supports_tracepoints".  */

static int
aarch64_supports_tracepoints (void)
{
  if (current_thread == NULL)
    return 1;
  else
    {
      /* We don't support tracepoints on aarch32 now.  */
      return is_64bit_tdesc ();
    }
}

/* Implementation of linux_target_ops method "supports_range_stepping".  */

static int
aarch64_supports_range_stepping (void)
{
  return 1;
}

struct linux_target_ops the_low_target =
{
  aarch64_arch_setup,
  aarch64_regs_info,
  aarch64_cannot_fetch_register,
  aarch64_cannot_store_register,
  NULL, /* fetch_register */
  aarch64_get_pc,
  aarch64_set_pc,
  (const unsigned char *) &aarch64_breakpoint,
  aarch64_breakpoint_len,
  NULL, /* breakpoint_reinsert_addr */
  0,    /* decr_pc_after_break */
  aarch64_breakpoint_at,
  aarch64_supports_z_point_type,
  aarch64_insert_point,
  aarch64_remove_point,
  aarch64_stopped_by_watchpoint,
  aarch64_stopped_data_address,
  NULL, /* collect_ptrace_register */
  NULL, /* supply_ptrace_register */
  NULL, /* siginfo_fixup */
  aarch64_linux_new_process,
  aarch64_linux_new_thread,
  aarch64_linux_new_fork,
  aarch64_linux_prepare_to_resume,
  NULL, /* process_qsupported */
  aarch64_supports_tracepoints,
  NULL, /* get_thread_area */
  NULL, /* install_fast_tracepoint_jump_pad */
  NULL, /* emit_ops */
  NULL, /* get_min_fast_tracepoint_insn_len */
  aarch64_supports_range_stepping,
};

void
initialize_low_arch (void)
{
  init_registers_aarch64 ();

  initialize_low_arch_aarch32 ();

  initialize_regsets_info (&aarch64_regsets_info);
}
