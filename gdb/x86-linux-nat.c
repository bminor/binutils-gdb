/* Native-dependent code for GNU/Linux x86 (i386 and x86-64).

   Copyright (C) 1999-2025 Free Software Foundation, Inc.

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

#include "inferior.h"
#include "elf/common.h"
#include "gdb_proc_service.h"
#include "nat/gdb_ptrace.h"
#include <sys/user.h>
#include <sys/procfs.h>
#include <sys/uio.h>

#include "x86-nat.h"
#ifndef __x86_64__
#include "nat/i386-linux.h"
#endif
#include "x86-linux-nat.h"
#include "i386-linux-tdep.h"
#ifdef __x86_64__
#include "amd64-linux-tdep.h"
#endif
#include "gdbsupport/x86-xstate.h"
#include "nat/x86-xstate.h"
#include "nat/linux-btrace.h"
#include "nat/linux-nat.h"
#include "nat/x86-linux.h"
#include "nat/x86-linux-dregs.h"
#include "nat/linux-ptrace.h"
#include "nat/x86-linux-tdesc.h"

#include <asm/ldt.h>
#include <linux/unistd.h>
#include <linux/elf.h>
#include <sys/user.h>
#include <sys/procfs.h>
#include <sys/uio.h>

/* linux_nat_target::low_new_fork implementation.  */

void
x86_linux_nat_target::low_new_fork (struct lwp_info *parent, pid_t child_pid)
{
  pid_t parent_pid;
  struct x86_debug_reg_state *parent_state;
  struct x86_debug_reg_state *child_state;

  /* NULL means no watchpoint has ever been set in the parent.  In
     that case, there's nothing to do.  */
  if (parent->arch_private == NULL)
    return;

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

  parent_pid = parent->ptid.pid ();
  parent_state = x86_debug_reg_state (parent_pid);
  child_state = x86_debug_reg_state (child_pid);
  *child_state = *parent_state;
}


x86_linux_nat_target::~x86_linux_nat_target ()
{
}

/* Implement the virtual inf_ptrace_target::post_startup_inferior method.  */

void
x86_linux_nat_target::post_startup_inferior (ptid_t ptid)
{
  x86_cleanup_dregs ();
  linux_nat_target::post_startup_inferior (ptid);
}

/* Get Linux/x86 target description from running target.  */

const struct target_desc *
x86_linux_nat_target::read_description ()
{
  /* The x86_linux_tdesc_for_tid call only reads xcr0 the first time it is
     called, the xcr0 value is stored here and reused on subsequent calls.  */
  static uint64_t xcr0_storage;

  if (inferior_ptid == null_ptid)
    return this->beneath ()->read_description ();

  int tid = inferior_ptid.pid ();

  return x86_linux_tdesc_for_tid (tid, &xcr0_storage, &this->m_xsave_layout);
}


/* Enable branch tracing.  */

struct btrace_target_info *
x86_linux_nat_target::enable_btrace (thread_info *tp,
				     const struct btrace_config *conf)
{
  struct btrace_target_info *tinfo = nullptr;
  ptid_t ptid = tp->ptid;
  try
    {
      tinfo = linux_enable_btrace (ptid, conf);
    }
  catch (const gdb_exception_error &exception)
    {
      error (_("Could not enable branch tracing for %s: %s"),
	     target_pid_to_str (ptid).c_str (), exception.what ());
    }

  return tinfo;
}

/* Disable branch tracing.  */

void
x86_linux_nat_target::disable_btrace (struct btrace_target_info *tinfo)
{
  enum btrace_error errcode = linux_disable_btrace (tinfo);

  if (errcode != BTRACE_ERR_NONE)
    error (_("Could not disable branch tracing."));
}

/* Teardown branch tracing.  */

void
x86_linux_nat_target::teardown_btrace (struct btrace_target_info *tinfo)
{
  /* Ignore errors.  */
  linux_disable_btrace (tinfo);
}

enum btrace_error
x86_linux_nat_target::read_btrace (struct btrace_data *data,
				   struct btrace_target_info *btinfo,
				   enum btrace_read_type type)
{
  return linux_read_btrace (data, btinfo, type);
}

/* See to_btrace_conf in target.h.  */

const struct btrace_config *
x86_linux_nat_target::btrace_conf (const struct btrace_target_info *btinfo)
{
  return linux_btrace_conf (btinfo);
}

/* ... */

static int
get_thread_area (pid_t pid, unsigned int idx, struct user_desc *ud)
{
  void *addr = (void *) (uintptr_t) idx;

#ifndef PTRACE_GET_THREAD_AREA
#define PTRACE_GET_THREAD_AREA 25
#endif

  if (ptrace (PTRACE_GET_THREAD_AREA, pid, addr, ud) < 0)
    return -1;

  return 0;
}

/* Get all TLS area data.  */

static gdb::byte_vector
x86_linux_get_tls_desc (unsigned int base_gdt_idx)
{
  static_assert (sizeof (unsigned int) == 4);

  struct user_desc tls_desc[3];
  memset (tls_desc, 0, sizeof (tls_desc));

  pid_t pid = inferior_ptid.pid ();

  /* Set true if we see a non-empty GDT entry.  */
  bool seen_non_empty = false;

  /* Linux reserves 3 GDT entries for TLS.  */
  for (unsigned int pos = 0; pos < 3; ++pos)
    {
      if (get_thread_area (pid, base_gdt_idx + pos, &tls_desc[pos]) != 0)
	return {};
      seen_non_empty |= (tls_desc[pos].base_addr != 0 || tls_desc[pos].limit != 0);
    }

  /* The kernel doesn't emit NT_I386_TLS if all the descriptors are empty.  */
  if (!seen_non_empty)
    return {};

  /* Copy the descriptors into a byte vector for return.  */
  gdb::byte_vector buf;
  buf.resize (sizeof (tls_desc));
  memcpy (buf.data (), tls_desc, sizeof (tls_desc));
  return buf;
}

/* See x86-linux-nat.h.  */

enum target_xfer_status
x86_linux_nat_target::xfer_tls_desc (gdb_byte *readbuf, const gdb_byte *writebuf,
				     ULONGEST offset, ULONGEST len, ULONGEST *xfered_len,
				     uint32_t gdt_base_idx)
{
  /* Don't allow writting to the TLS GDT descriptors.  */
  if (writebuf != nullptr)
    return TARGET_XFER_E_IO;

  gdb::byte_vector buf = x86_linux_get_tls_desc (gdt_base_idx);
  if (buf.empty ())
    return TARGET_XFER_E_IO;

  ssize_t buf_size = buf.size ();
  if (offset >= buf_size)
    return TARGET_XFER_EOF;
  buf_size -= offset;

  if (buf_size > len)
    buf_size = len;

  memcpy (readbuf, buf.data () + offset, buf_size);
  *xfered_len = buf_size;
  return TARGET_XFER_OK;
}



/* Helper for ps_get_thread_area.  Sets BASE_ADDR to a pointer to
   the thread local storage (or its descriptor) and returns PS_OK
   on success.  Returns PS_ERR on failure.  */

ps_err_e
x86_linux_get_thread_area (pid_t pid, void *addr, unsigned int *base_addr)
{
  /* NOTE: cagney/2003-08-26: The definition of this buffer is found
     in the kernel header <asm-i386/ldt.h>.  It, after padding, is 4 x
     4 byte integers in size: `entry_number', `base_addr', `limit',
     and a bunch of status bits.

     The values returned by this ptrace call should be part of the
     regcache buffer, and ps_get_thread_area should channel its
     request through the regcache.  That way remote targets could
     provide the value using the remote protocol and not this direct
     call.

     Is this function needed?  I'm guessing that the `base' is the
     address of a descriptor that libthread_db uses to find the
     thread local address base that GDB needs.  Perhaps that
     descriptor is defined by the ABI.  Anyway, given that
     libthread_db calls this function without prompting (gdb
     requesting tls base) I guess it needs info in there anyway.  */
  unsigned int desc[4];

  /* This code assumes that "int" is 32 bits and that
     GET_THREAD_AREA returns no more than 4 int values.  */
  gdb_assert (sizeof (int) == 4);

#ifndef PTRACE_GET_THREAD_AREA
#define PTRACE_GET_THREAD_AREA 25
#endif

  if (ptrace (PTRACE_GET_THREAD_AREA, pid, addr, &desc) < 0)
    return PS_ERR;

  *base_addr = desc[1];
  return PS_OK;
}


void _initialize_x86_linux_nat ();
void
_initialize_x86_linux_nat ()
{
  /* Initialize the debug register function vectors.  */
  x86_dr_low.set_control = x86_linux_dr_set_control;
  x86_dr_low.set_addr = x86_linux_dr_set_addr;
  x86_dr_low.get_addr = x86_linux_dr_get_addr;
  x86_dr_low.get_status = x86_linux_dr_get_status;
  x86_dr_low.get_control = x86_linux_dr_get_control;
  x86_set_debug_register_length (sizeof (void *));
}
