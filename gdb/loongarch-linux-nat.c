/* Target-dependent code for GNU/Linux LoongArch.

   Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

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

#include "defs.h"
#include "command.h"
#include "gdbcmd.h"
#include "inferior.h"
#include "loongarch-tdep.h"
#include "target.h"
#include "regcache.h"
#include "linux-nat-trad.h"
#include "loongarch-linux-tdep.h"
#include "target-descriptions.h"

#include "gdb_proc_service.h"
#include "gregset.h"

#include "nat/gdb_ptrace.h"
#include <asm/ptrace.h>
#include "inf-ptrace.h"
#include "arch/loongarch-linux-nat.h"
#include "elf/common.h"

#include "nat/loongarch-linux-watch.h"

class loongarch_linux_nat_target final : public linux_nat_trad_target
{
public:
  void fetch_registers (struct regcache *, int) override;
  void store_registers (struct regcache *, int) override;

  void close () override;

  int can_use_hw_breakpoint (enum bptype, int, int) override;

  int insert_hw_breakpoint (struct gdbarch *,
			    struct bp_target_info *) override;

  int remove_hw_breakpoint (struct gdbarch *,
			    struct bp_target_info *) override;

  int remove_watchpoint (CORE_ADDR, int, enum target_hw_bp_type,
			 struct expression *) override;

  int insert_watchpoint (CORE_ADDR, int, enum target_hw_bp_type,
			 struct expression *) override;

  bool stopped_by_watchpoint () override;

  bool stopped_data_address (CORE_ADDR *) override;

  int region_ok_for_hw_watchpoint (CORE_ADDR, int) override;

  const struct target_desc *read_description () override;

  enum target_xfer_status xfer_partial (enum target_object object,
					const char *annex, gdb_byte *readbuf,
					const gdb_byte *writebuf,
					ULONGEST offset, ULONGEST len,
					ULONGEST *xfered_len) override;

protected:
  CORE_ADDR register_u_offset (struct gdbarch *gdbarch, int regno,
			       int store_p) override;

  void low_new_thread (struct lwp_info *lp) override;

private:
  void loongarch_fetch_regs (struct regcache *regcache, int regno);
  void loongarch_store_regs (struct regcache *regcache, int regno);
};

/* Assume that we have PTRACE_{GET,SET}REGSET et al. support.  If we do not,
   we'll clear this and use PTRACE_PEEKUSR instead.  */
extern enum tribool have_ptrace_getregset;

static void
ensure_ptrace_getregset ()
{
  have_ptrace_getregset = TRIBOOL_TRUE;
}

/* Fetch REGNO (or all registers if REGNO == -1) from the target
   using any working method.  */

void
loongarch_linux_nat_target::fetch_registers (struct regcache *regcache,
					     int regnum)
{
  if (have_ptrace_getregset == TRIBOOL_UNKNOWN)
    ensure_ptrace_getregset ();

  if (sizeof (void *) == 4)
    gdb_assert (have_ptrace_getregset == TRIBOOL_TRUE
		||register_size (regcache->arch (),
		  gdbarch_tdep (regcache->arch ())->regs.r) != 32);

  if (have_ptrace_getregset == TRIBOOL_TRUE)
    loongarch_fetch_regs (regcache, regnum);
  else
    linux_nat_trad_target::fetch_registers (regcache, regnum);
}

/* Store REGNO (or all registers if REGNO == -1) to the target
   using any working method.  */

void
loongarch_linux_nat_target::store_registers (struct regcache *regcache,
					     int regnum)
{
  if (have_ptrace_getregset == TRIBOOL_UNKNOWN)
    ensure_ptrace_getregset ();

  if (sizeof (void *) == 4)
    gdb_assert (have_ptrace_getregset == TRIBOOL_TRUE
		|| register_size (regcache->arch (),
		   gdbarch_tdep (regcache->arch ())->regs.r) != 32);

  if (have_ptrace_getregset == TRIBOOL_TRUE)
    loongarch_store_regs (regcache, regnum);
  else
    linux_nat_trad_target::store_registers (regcache, regnum);
}

const struct target_desc *
loongarch_linux_nat_target::read_description ()
{
  ensure_ptrace_getregset ();
  return loongarch_linux_read_description_runtime (inferior_ptid.pid ());
}

void
loongarch_linux_nat_target::loongarch_fetch_regs (struct regcache *regcache,
						  int regno)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  pid_t tid = get_ptrace_pid (regcache->ptid ());

  do
    {
      if (regs->r < 0)
	break;
      if (regno == -1)
	;
      else if (regs->r <= regno && regno < regs->r + 32)
	;
      else if (regs->pc == regno)
	;
      else if (regs->badvaddr == regno)
	;
      else
	break;

      loongarch_elf_gregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_PRSTATUS, &iovec));
      loongarch_elf_gregset.supply_regset (NULL, regcache, regno, &regset,
					   sizeof (regset));
    }
  while (0);

  do
    {
      if (regs->f < 0)
	break;
      if (regno == -1)
	;
      else if (regs->f <= regno && regno < regs->f + 32)
	;
      else if (regs->fcc <= regno && regno < regs->fcc + 8)
	;
      else if (regs->fcsr == regno)
	;
      else
	break;

      loongarch_elf_fpregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_FPREGSET, &iovec));
      loongarch_elf_fpregset.supply_regset (NULL, regcache, regno, &regset,
					    sizeof (regset));
    }
  while (0);

  do
    {
      if (regs->scr < 0)
	break;
      else if (regno == -1)
	;
      else if (regs->scr <= regno && regno < regs->scr + 4)
	;
      else if (regs->EFLAG == regno)
	;
      else if (regs->x86_top == regno)
	;
      else
	break;

      loongarch_elf_lbtregset_t regset = { 0 };
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LBT, &iovec));
      loongarch_elf_lbtregset.supply_regset (NULL, regcache, regno, &regset,
					     sizeof (regset));
    }
  while (0);

  do
    {
      if (regs->vr < 0)
	break;
      if (regno == -1)
	;
      else if (regs->vr <= regno && regno < regs->vr + 32)
	;
      else
	break;

      loongarch_elf_lsxregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LSX, &iovec));
      loongarch_elf_lsxregset.supply_regset (NULL, regcache, regno, &regset,
					     sizeof (regset));
    }
  while (0);

  do
    {
      if (regs->xr < 0)
	break;
      else if (regno == -1)
	;
      else if (regs->xr <= regno && regno < regs->xr + 32)
	;
      else
	break;

      loongarch_elf_lasxregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LASX, &iovec));
      loongarch_elf_lasxregset.supply_regset (NULL, regcache, regno, &regset,
					      sizeof (regset));
    }
  while (0);
}

void
loongarch_linux_nat_target::loongarch_store_regs (struct regcache *regcache,
						  int regno)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  pid_t tid = get_ptrace_pid (regcache->ptid ());

  do
    {
      if (regs->r < 0)
	break;
      if (regno == -1)
	;
      else if (regs->r <= regno && regno < regs->r + 32)
	;
      else if (regs->pc == regno || regs->badvaddr == regno)
	;
      else
	break;

      loongarch_elf_gregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_PRSTATUS, &iovec));
      loongarch_elf_gregset.collect_regset (NULL, regcache, regno, &regset,
					    sizeof (regset));
      gdb_assert (0 == ptrace (PTRACE_SETREGSET, tid, NT_PRSTATUS, &iovec));
    }
  while (0);

  do
    {
      if (regs->f < 0)
	break;
      if (regno == -1)
	;
      else if (regs->f <= regno && regno < regs->f + 32)
	;
      else if (regs->fcc <= regno && regno < regs->fcc + 8)
	;
      else if (regs->fcsr == regno)
	;
      else
	break;

      loongarch_elf_fpregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_FPREGSET, &iovec));
      loongarch_elf_fpregset.collect_regset (NULL, regcache, regno, &regset,
					     sizeof (regset));
      gdb_assert (0 == ptrace (PTRACE_SETREGSET, tid, NT_FPREGSET, &iovec));
    }
  while (0);

  do
    {
      if (regs->scr < 0)
	break;
      else if (regno == -1)
	;
      else if (regs->scr <= regno && regno < regs->scr + 4)
	;
      else if (regs->EFLAG == regno)
	;
      else if (regs->x86_top == regno)
	;
      else
	break;

      loongarch_elf_lbtregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LBT, &iovec));
      loongarch_elf_lbtregset.collect_regset (NULL, regcache, regno, &regset,
					      sizeof (regset));
      gdb_assert (0 == ptrace (PTRACE_SETREGSET, tid, NT_LARCH_LBT, &iovec));
    }
  while (0);

  do
    {
      if (regs->vr < 0)
	break;
      if (regno == -1)
	;
      else if (regs->vr <= regno && regno < regs->vr + 32)
	;
      else
	break;

      loongarch_elf_lsxregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LSX, &iovec));
      loongarch_elf_lsxregset.collect_regset (NULL, regcache, regno, &regset,
					      sizeof (regset));
      gdb_assert (0 == ptrace (PTRACE_SETREGSET, tid, NT_LARCH_LSX, &iovec));
    }
  while (0);

  do
    {
      if (regs->xr < 0)
	break;
      else if (regno == -1)
	;
      else if (regs->xr <= regno && regno < regs->xr + 32)
	;
      else
	break;

      loongarch_elf_lasxregset_t regset;
      struct iovec iovec = { .iov_base = &regset, .iov_len = sizeof (regset) };
      gdb_assert (0 == ptrace (PTRACE_GETREGSET, tid, NT_LARCH_LASX, &iovec));
      loongarch_elf_lasxregset.collect_regset (NULL, regcache, regno, &regset,
					       sizeof (regset));
      gdb_assert (0 == ptrace (PTRACE_SETREGSET, tid, NT_LARCH_LASX, &iovec));
    }
  while (0);
}

/* Return the address in the core dump or inferior of register REGNO.  */

CORE_ADDR
loongarch_linux_nat_target::register_u_offset (struct gdbarch *gdbarch,
					       int regno, int store_p)
{
  auto regs = &gdbarch_tdep (gdbarch)->regs;
  /* according to <asm/ptrace.h> */
  if (0 <= regs->r && regs->r <= regno && regno < regs->r + GPR_NUM)
    return GPR_BASE + regno - regs->r;
  else if (regs->pc == regno)
    return PC;
  return -1;
}

/* Implement the to_xfer_partial target_ops method.  */

enum target_xfer_status
loongarch_linux_nat_target::xfer_partial (enum target_object object,
					  const char *annex, gdb_byte *readbuf,
					  const gdb_byte *writebuf,
					  ULONGEST offset, ULONGEST len,
					  ULONGEST *xfered_len)
{
  pid_t pid = inferior_ptid.pid ();

  if (object != TARGET_OBJECT_LARCH)
    return linux_nat_trad_target::xfer_partial (
      object, annex, readbuf, writebuf, offset, len, xfered_len);

  if (strcmp (annex, "cpucfg") == 0)
    {
      if (writebuf)
	return TARGET_XFER_E_IO;
      if (offset % 4 != 0 || offset % 4 != 0)
	return TARGET_XFER_E_IO;
      char t_buf[offset + len];
      struct iovec iovec = { .iov_base = &t_buf, .iov_len = sizeof (t_buf) };
      if (ptrace (PTRACE_GETREGSET, pid, NT_LARCH_CPUCFG, &iovec) < 0)
	{
	  size_t i;
	  for (i = offset / 4; i < (offset + len) / 4 + 1; i++)
	    ((uint32_t *) t_buf)[i] = loongarch_cpucfg (i);
	}
      memcpy (readbuf, t_buf + offset, len);
      *xfered_len = len;
      return TARGET_XFER_OK;
    }

  return TARGET_XFER_E_IO;
}

static loongarch_linux_nat_target the_loongarch_linux_nat_target;

/* Wrapper functions.  These are only used by libthread_db.  */

void
supply_gregset (struct regcache *regcache,
		const gdb_gregset_t /* elf_gregset_t  */ *gregset)
{
  loongarch_elf_gregset.supply_regset (NULL, regcache, -1, gregset,
				       sizeof (gdb_gregset_t));
}

void
fill_gregset (const struct regcache *regcache,
	      gdb_gregset_t /* elf_gregset_t  */ *gregset, int regno)
{
  loongarch_elf_gregset.collect_regset (NULL, regcache, regno, gregset,
					sizeof (gdb_gregset_t));
}

void
supply_fpregset (struct regcache *regcache,
		 const gdb_fpregset_t /* elf_fpregset_t  */ *fpregset)
{
  loongarch_elf_fpregset.supply_regset (NULL, regcache, -1, fpregset,
					sizeof (gdb_fpregset_t));
}

void
fill_fpregset (const struct regcache *regcache,
	       gdb_fpregset_t /* elf_fpregset_t  */ *fpregset, int regno)
{
  loongarch_elf_fpregset.collect_regset (NULL, regcache, regno, fpregset,
					 sizeof (gdb_fpregset_t));
}

/* -1 if the kernel and/or CPU do not support watch registers.
    1 if watch_readback is valid and we can read style, num_valid
      and the masks.
    0 if we need to read the watch_readback.  */

static int watch_readback_valid;

/* Cached watch register read values.  */

static struct pt_watch_regs watch_readback =
{
  .max_valid = MAX_DEBUG_REGISTER,
};

static struct loongarch_watchpoint *current_watches;

/*  The current set of watch register values for writing the
    registers.  */

static struct pt_watch_regs watch_mirror =
{
  .max_valid = MAX_DEBUG_REGISTER,
};

static void
loongarch_show_dr (const char *func, CORE_ADDR addr, int len,
		   enum target_hw_bp_type type)
{
  int i;

  puts_unfiltered (func);
  if (addr || len)
    printf_unfiltered (
      " (addr=%s, len=%d, type=%s)", paddress (target_gdbarch (), addr), len,
      type == hw_write
	? "data-write"
	: (type == hw_read
	     ? "data-read"
	     : (type == hw_access ? "data-read/write"
				  : (type == hw_execute ? "instruction-execute"
							: "??unknown??"))));
  puts_unfiltered (":\n");

  for (i = 0; i < MAX_DEBUG_REGISTER; i++)
    printf_unfiltered (
      "\tDR%d: addr=%s, mask=%s\n", i,
      paddress (target_gdbarch (),
		loongarch_linux_watch_get_addr (&watch_mirror, i)),
      paddress (target_gdbarch (),
		loongarch_linux_watch_get_mask (&watch_mirror, i)));
}

/* Target to_can_use_hw_breakpoint implementation.  Return 1 if we can
   handle the specified watch type.  */

int
loongarch_linux_nat_target::can_use_hw_breakpoint (enum bptype type, int cnt,
						   int ot)
{
  int i;
  uint32_t wanted_mask, irw_mask;
  long lwp = inferior_ptid.lwp ();

  if (!loongarch_linux_read_watch_registers (lwp, &watch_readback,
					     &watch_readback_valid, 0))
    return 0;

  switch (type)
    {
    case bp_hardware_watchpoint:
      wanted_mask = W_MASK;
      break;
    case bp_read_watchpoint:
      wanted_mask = R_MASK;
      break;
    case bp_access_watchpoint:
      wanted_mask = R_MASK | W_MASK;
      break;
    case bp_hardware_breakpoint:
      wanted_mask = I_MASK;
      break;
    default:
      return 0;
    }

  for (i = 0; i < loongarch_linux_watch_get_num_valid (&watch_readback) && cnt;
       i++)
    {
      irw_mask = loongarch_linux_watch_get_irwmask (&watch_readback, i);
      if ((irw_mask & wanted_mask) == wanted_mask)
	cnt--;
    }
  return (cnt == 0) ? 1 : 0;
}

/* Target to_stopped_by_watchpoint implementation.  Return 1 if
   stopped by watchpoint.  The watchhi R and W bits indicate the watch
   register triggered.  */

bool
loongarch_linux_nat_target::stopped_by_watchpoint ()
{
  int n;
  int num_valid;

  if (!loongarch_linux_read_watch_registers (
	inferior_ptid.lwp (), &watch_readback, &watch_readback_valid, 1))
    return false;

  num_valid = loongarch_linux_watch_get_num_valid (&watch_readback);

  for (n = 0; n < MAX_DEBUG_REGISTER && n < num_valid; n++)
    if (loongarch_linux_watch_get_irwstat (&watch_readback, n)
	& (R_MASK | W_MASK))
      return true;

  return false;
}

/* Target to_stopped_data_address implementation.  Set the address
   where the watch triggered (if known).  Return 1 if the address was
   known.  */

bool
loongarch_linux_nat_target::stopped_data_address (CORE_ADDR *paddr)
{
  int num_valid, n;
  if (!loongarch_linux_read_watch_registers (
	inferior_ptid.lwp (), &watch_readback, &watch_readback_valid, 1))
    return false;

  num_valid = loongarch_linux_watch_get_num_valid (&watch_readback);

  for (n = 0; n < MAX_DEBUG_REGISTER && n < num_valid; n++)
    if (loongarch_linux_watch_get_irwstat (&watch_readback, n)
	& (R_MASK | W_MASK))
      {
	CORE_ADDR t_addr, t_mask;
	int t_irw;
	struct loongarch_watchpoint *watch;

	t_addr = loongarch_linux_watch_get_addr (&watch_readback, n);
	t_irw = loongarch_linux_watch_get_irw (&watch_readback, n) & IRW_MASK;
	t_mask = loongarch_linux_watch_get_mask (&watch_readback, n);

	for (watch = current_watches; watch != NULL; watch = watch->next)
	  {
	    CORE_ADDR addr = watch->addr;
	    CORE_ADDR last_byte = addr + watch->len - 1;

	    if ((t_irw & loongarch_linux_watch_type_to_irw (watch->type)) == 0)
	      {
		/* Different type.  */
		continue;
	      }
	    /* Check for overlap of even a single byte.  */
	    if (last_byte >= t_addr && addr <= t_addr + t_mask)
	      {
		*paddr = addr;
		return true;
	      }
	  }
      }
  return false;
}

/* Target to_region_ok_for_hw_watchpoint implementation.  Return 1 if
   the specified region can be covered by the watch registers.  */

int
loongarch_linux_nat_target::region_ok_for_hw_watchpoint (CORE_ADDR addr,
							 int len)
{
  struct pt_watch_regs dummy_regs;
  int i;

  if (!loongarch_linux_read_watch_registers (
	inferior_ptid.lwp (), &watch_readback, &watch_readback_valid, 0))
    return 0;

  dummy_regs = watch_readback;
  /* Clear them out.  */
  for (i = 0; i < loongarch_linux_watch_get_num_valid (&dummy_regs); i++)
    {
      loongarch_linux_watch_set_addr (&dummy_regs, i, 0);
      loongarch_linux_watch_set_mask (&dummy_regs, i, 0);
      loongarch_linux_watch_set_irw (&dummy_regs, i, 0);
    }
  return loongarch_linux_watch_try_one_watch (&dummy_regs, addr, len, 0);
}

/* Write the mirrored watch register values for each thread.  */

static int
write_watchpoint_regs (void)
{
  struct lwp_info *lp;
  int tid;

  ALL_LWPS (lp)
  {
    tid = lp->ptid.lwp ();
    if (ptrace (PTRACE_SET_WATCH_REGS, tid, &watch_mirror, NULL) == -1)
      perror_with_name (_ ("Couldn't write debug register"));
  }
  return 0;
}

/* linux_nat_target::low_new_thread implementation.  */

void
loongarch_linux_nat_target::low_new_thread (struct lwp_info *lp)
{
  long tid = lp->ptid.lwp ();

  if (!loongarch_linux_read_watch_registers (tid, &watch_readback,
					     &watch_readback_valid, 0))
    return;

  if (ptrace (PTRACE_SET_WATCH_REGS, tid, &watch_mirror, NULL) == -1)
    perror_with_name (_ ("Couldn't write debug register"));
}

/* Target to_insert_watchpoint implementation.  Try to insert a new
   watch.  Return zero on success.  */

int
loongarch_linux_nat_target::insert_watchpoint (CORE_ADDR addr, int len,
					       enum target_hw_bp_type type,
					       struct expression *cond)
{
  struct pt_watch_regs regs =
  {
    .max_valid = MAX_DEBUG_REGISTER,
  };
  struct loongarch_watchpoint *new_watch;
  struct loongarch_watchpoint **pw;

  int retval;

  if (!loongarch_linux_read_watch_registers (
	inferior_ptid.lwp (), &watch_readback, &watch_readback_valid, 0))
    return -1;

  if (len <= 0)
    return -1;

  regs = watch_readback;
  /* Add the current watches.  */
  loongarch_linux_watch_populate_regs (current_watches, &regs);

  /* Now try to add the new watch.  */
  if (!loongarch_linux_watch_try_one_watch (
	&regs, addr, len, loongarch_linux_watch_type_to_irw (type)))
    return -1;

  /* It fit.  Stick it on the end of the list.  */
  new_watch = XNEW (struct loongarch_watchpoint);
  new_watch->addr = addr;
  new_watch->len = len;
  new_watch->type = type;
  new_watch->next = NULL;

  pw = &current_watches;
  while (*pw != NULL)
    pw = &(*pw)->next;
  *pw = new_watch;

  watch_mirror = regs;
  retval = write_watchpoint_regs ();

  if (show_debug_regs)
    loongarch_show_dr ("insert_watchpoint", addr, len, type);

  return retval;
}

/* Target to_remove_watchpoint implementation.  Try to remove a watch.
   Return zero on success.  */

int
loongarch_linux_nat_target::remove_watchpoint (CORE_ADDR addr, int len,
					       enum target_hw_bp_type type,
					       struct expression *cond)
{
  int retval;
  int deleted_one;

  struct loongarch_watchpoint **pw;
  struct loongarch_watchpoint *w;

  /* Search for a known watch that matches.  Then unlink and free
     it.  */
  deleted_one = 0;
  pw = &current_watches;
  while ((w = *pw))
    {
      if (w->addr == addr && w->len == len && w->type == type)
	{
	  *pw = w->next;
	  xfree (w);
	  deleted_one = 1;
	  break;
	}
      pw = &(w->next);
    }

  if (!deleted_one)
    return -1; /* We don't know about it, fail doing nothing.  */

  /* At this point watch_readback is known to be valid because we
     could not have added the watch without reading it.  */
  gdb_assert (watch_readback_valid == 1);

  watch_mirror = watch_readback;
  loongarch_linux_watch_populate_regs (current_watches, &watch_mirror);

  retval = write_watchpoint_regs ();

  if (show_debug_regs)
    loongarch_show_dr ("remove_watchpoint", addr, len, type);

  return retval;
}

/* Insert a hardware-assisted breakpoint at BP_TGT->reqstd_address.
   Return 0 on success, -1 on failure.  */

int
loongarch_linux_nat_target::insert_hw_breakpoint (
  struct gdbarch *gdbarch, struct bp_target_info *bp_tgt)
{
  int ret;
  CORE_ADDR addr = bp_tgt->placed_address = bp_tgt->reqstd_address;
  int len = 4;
  const enum target_hw_bp_type type = hw_execute;

  gdbarch_breakpoint_from_pc (gdbarch, &addr, &len);

  if (show_debug_regs)
    fprintf_unfiltered (
      gdb_stdlog, "insert_hw_breakpoint on entry (addr=0x%08lx, len=%d))\n",
      (unsigned long) addr, len);

  ret = insert_watchpoint (addr, len, type, NULL);
  return ret;
}

/* Remove a hardware-assisted breakpoint at BP_TGT->placed_address.
   Return 0 on success, -1 on failure.  */

int
loongarch_linux_nat_target::remove_hw_breakpoint (
  struct gdbarch *gdbarch, struct bp_target_info *bp_tgt)
{
  int ret;
  CORE_ADDR addr = bp_tgt->placed_address;
  int len = 4;
  const enum target_hw_bp_type type = hw_execute;

  gdbarch_breakpoint_from_pc (gdbarch, &addr, &len);

  if (show_debug_regs)
    fprintf_unfiltered (
      gdb_stdlog, "remove_hw_breakpoint on entry (addr=0x%08lx, len=%d))\n",
      (unsigned long) addr, len);

  ret = remove_watchpoint (addr, len, type, NULL);

  return ret;
}

/* Target to_close implementation.  Free any watches and call the
   super implementation.  */

void
loongarch_linux_nat_target::close ()
{
  struct loongarch_watchpoint *w;
  struct loongarch_watchpoint *nw;

  /* Clean out the current_watches list.  */
  w = current_watches;
  while (w)
    {
      nw = w->next;
      xfree (w);
      w = nw;
    }
  current_watches = NULL;

  linux_nat_trad_target::close ();
}

void _initialize_loongarch_linux_nat ();
void
_initialize_loongarch_linux_nat ()
{
  add_setshow_boolean_cmd (
    "show-debug-regs", class_maintenance, &show_debug_regs, _ ("\
Set whether to show variables that mirror the LoongArch debug registers."),
    _ ("\
Show whether to show variables that mirror the LoongArch debug registers."),
    _ ("\
Use \"on\" to enable, \"off\" to disable.\n\
If enabled, the debug registers values are shown when GDB inserts\n\
or removes a hardware breakpoint or watchpoint, and when the inferior\n\
triggers a breakpoint or watchpoint."),
    NULL, NULL, &maintenance_set_cmdlist, &maintenance_show_cmdlist);

  linux_target = &the_loongarch_linux_nat_target;
  add_inf_child_target (&the_loongarch_linux_nat_target);
}
