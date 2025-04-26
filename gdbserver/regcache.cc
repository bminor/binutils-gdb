/* Register support routines for the remote server for GDB.
   Copyright (C) 2001-2025 Free Software Foundation, Inc.

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

#include "regdef.h"
#include "gdbthread.h"
#include "tdesc.h"
#include "gdbsupport/rsp-low.h"
#include "gdbsupport/gdb-checked-static-cast.h"

#ifndef IN_PROCESS_AGENT

struct regcache *
get_thread_regcache (thread_info *thread, bool fetch)
{
  regcache *regcache = thread->regcache ();

  /* Threads' regcaches are created lazily, because biarch targets add
     the main thread/lwp before seeing it stop for the first time, and
     it is only after the target sees the thread stop for the first
     time that the target has a chance of determining the process's
     architecture.  IOW, when we first add the process's main thread
     we don't know which architecture/tdesc its regcache should
     have.  */
  if (regcache == NULL)
    {
      process_info *proc = thread->process ();

      gdb_assert (proc->tdesc != NULL);

      thread->set_regcache (std::make_unique<struct regcache> (proc->tdesc));
      regcache = thread->regcache ();
    }

  if (fetch && !regcache->registers_fetched)
    {
      scoped_restore_current_thread restore_thread;

      switch_to_thread (thread);
      /* Invalidate all registers, to prevent stale left-overs.  */
      regcache->reset (REG_UNKNOWN);
      fetch_inferior_registers (regcache, -1);
      regcache->registers_fetched = true;
    }

  return regcache;
}

/* See gdbsupport/common-regcache.h.  */

reg_buffer_common *
get_thread_regcache_for_ptid (ptid_t ptid)
{
  return get_thread_regcache (find_thread_ptid (ptid));
}

void
regcache_invalidate_thread (thread_info *thread)
{
  regcache *regcache = thread->regcache ();

  if (regcache == NULL)
    return;

  if (regcache->registers_fetched)
    {
      scoped_restore_current_thread restore_thread;

      switch_to_thread (thread);
      store_inferior_registers (regcache, -1);
    }

  regcache->registers_fetched = false;
}

/* See regcache.h.  */

void
regcache_invalidate_pid (int pid)
{
  process_info *process = find_process_pid (pid);

  if (process != nullptr)
    process->for_each_thread (regcache_invalidate_thread);
}

/* See regcache.h.  */

void
regcache_invalidate (void)
{
  /* Only update the threads of the current process.  */
  int pid = current_thread->id.pid ();

  regcache_invalidate_pid (pid);
}

#endif

regcache::regcache (const target_desc *tdesc, unsigned char *regbuf)
  : tdesc (tdesc), fixed_size_registers (regbuf)
{
  gdb_assert (regbuf != nullptr);
}

#ifndef IN_PROCESS_AGENT

regcache::regcache (const target_desc *tdesc)
  : tdesc (tdesc), registers_owned (true)
{
  gdb_assert (tdesc->fixed_registers_size != 0);

  /* Make sure to zero-initialize the register cache when it is
     created, in case there are registers the target never
     fetches.  This way they'll read as zero instead of
     garbage.  */
  this->fixed_size_registers
    = (unsigned char *) xmalloc (tdesc->fixed_registers_size);
  size_t num_regs = tdesc->reg_defs.size ();
  m_register_status.reset (new enum register_status[num_regs]);
  reset (REG_UNKNOWN);
}

regcache::~regcache ()
{
  if (registers_owned)
    free (fixed_size_registers);
}

#endif

void
regcache::reset (enum register_status status)
{
  /* Zero-initialize the register cache, in case there are registers
     the target never fetches.  This way they'll read as zero instead
     of garbage.  */
  memset (this->fixed_size_registers, 0, this->tdesc->fixed_registers_size);
#ifndef IN_PROCESS_AGENT
  this->variable_size_registers.reset ();

  for (int i = 0; i < this->tdesc->reg_defs.size (); i++)
    set_register_status (i, status);

  invalidate_variable_size_registers ();
#endif
}

void
regcache::copy_from (regcache *src)
{
  gdb_assert (src != nullptr);
  gdb_assert (src->tdesc == this->tdesc);
  gdb_assert (src != this);

  memcpy (this->fixed_size_registers, src->fixed_size_registers, src->tdesc->fixed_registers_size);
#ifndef IN_PROCESS_AGENT
  if (m_register_status != nullptr && src->m_register_status != nullptr)
    memcpy (m_register_status.get (), src->m_register_status.get (),
	    src->tdesc->reg_defs.size ());
#endif
  this->registers_fetched = src->registers_fetched;
}

/* Return a reference to the description of register N.  */

static const struct gdb::reg &
find_register_by_number (const struct target_desc *tdesc, int n)
{
  gdb_assert (n >= 0);
  gdb_assert (n < tdesc->reg_defs.size ());

  return tdesc->reg_defs[n];
}

#ifndef IN_PROCESS_AGENT

static gdb::array_view<gdb_byte> register_data (const struct regcache *regcache,
						int n);

/* See regcache.h.  */

bool
register_from_string (struct regcache *regcache, int regnum,
		      gdb::array_view<const char> buf)
{
  gdb::array_view<gdb_byte> value = register_data (regcache, regnum);
  int expected_len = value.size () * 2;

  if (buf.size () != expected_len)
    {
      warning (_("Wrong sized packet for register %d (expected %d bytes, got %zu)"),
	       regnum, expected_len, buf.size ());
      return false;
    }

  hex2bin (buf.data (), value.data (), buf.size () / 2);

  return true;
}

void
registers_to_string (struct regcache *regcache, char *buf)
{
  const struct target_desc *tdesc = regcache->tdesc;

  for (int i = 0; i < tdesc->reg_defs.size (); ++i)
    {
      collect_register_as_string (regcache, i, buf);
      buf += regcache->register_size (i) * 2;
    }
}

void
registers_from_string (struct regcache *regcache, char *buf)
{
  int len = strlen (buf);
  unsigned char *registers = regcache->fixed_size_registers;
  unsigned char *var_size_registers = regcache->variable_size_registers.get ();
  const struct target_desc *tdesc = regcache->tdesc;
  int expected_len = (tdesc->fixed_registers_size + regcache->variable_registers_size) * 2;

  if (len != expected_len)
    {
      warning ("Wrong sized register packet (expected %d bytes, got %d)",
	       expected_len, len);
      if (len > expected_len)
	len = expected_len;
    }

  for (int i = 0; i < tdesc->reg_defs.size (); ++i)
    {
      int reg_size = regcache->register_size (i);
      unsigned char *regs;

      if (tdesc->reg_defs[i].size == TDESC_REG_VARIABLE_SIZE)
	regs = var_size_registers;
      else
	regs = registers;

      hex2bin (buf, regs, reg_size);
      buf += reg_size * 2;
    }
}

/* See regcache.h */

std::optional<int>
find_regno_no_throw (const struct target_desc *tdesc, const char *name)
{
  for (int i = 0; i < tdesc->reg_defs.size (); ++i)
    {
      if (strcmp (name, find_register_by_number (tdesc, i).name) == 0)
	return i;
    }
  return {};
}

int
find_regno (const struct target_desc *tdesc, const char *name)
{
  std::optional<int> regnum = find_regno_no_throw (tdesc, name);

  if (regnum.has_value ())
    return *regnum;

  internal_error ("Unknown register %s requested", name);
}

static void
free_register_cache_thread (thread_info *thread)
{
  regcache *regcache = thread->regcache ();

  if (regcache != NULL)
    {
      regcache_invalidate_thread (thread);
      thread->set_regcache (nullptr);
    }
}

void
regcache_release (void)
{
  /* Flush and release all pre-existing register caches.  */
  for_each_thread (free_register_cache_thread);
}
#endif

#ifndef IN_PROCESS_AGENT
/* See regcache.h */

void
regcache::initialize_variable_size_registers ()
{
  const unsigned int num_regs = tdesc->reg_defs.size ();
  int total_size = 0;

  variable_size_sizes.resize (num_regs);
  variable_size_offsets.resize (num_regs);

  for (int i = 0; i < num_regs; i++)
    {
      const gdb::reg &reg = tdesc->reg_defs[i];
      if (reg.size != TDESC_REG_VARIABLE_SIZE)
	{
	  variable_size_sizes[i] = -1;
	  variable_size_offsets[i] = -1;
	  continue;
	}

      long size = target_desc_parameter_value (this, reg.bitsize_parameter);
      variable_size_sizes[i] = size;
      variable_size_offsets[i] = total_size;
      total_size += size;
    }

  variable_size_registers.reset (new unsigned char[total_size]);
  variable_registers_size = total_size;
}

/* See regcache.h */

void
regcache::invalidate_variable_size_registers ()
{
  /* There's nothing to do if there are already no variable-size register
     contents.  */
  if (variable_size_registers == nullptr)
    return;

  for (unsigned int i = 0; i < tdesc->reg_defs.size (); i++)
    {
      const gdb::reg &reg = tdesc->reg_defs[i];
      if (reg.size == TDESC_REG_VARIABLE_SIZE)
	set_register_status (i, REG_UNKNOWN);
    }

  variable_size_registers = nullptr;
  variable_size_sizes.clear ();
  variable_size_offsets.clear ();
}
#endif

int
register_cache_size (const struct target_desc *tdesc)
{
  /* Variable-size registers aren't considered here because this function is
     only used for tracepoints.  */
  return tdesc->fixed_registers_size;
}

int
register_size (const struct target_desc *tdesc, int n)
{
  const gdb::reg &reg = find_register_by_number (tdesc, n);

  gdb_assert (reg.size != TDESC_REG_VARIABLE_SIZE);

  return reg.size / 8;
}

/* See gdbsupport/common-regcache.h.  */

int
regcache::register_size (int regnum) const
{
  const gdb::reg &reg = find_register_by_number (tdesc, regnum);

  if (reg.size == TDESC_REG_VARIABLE_SIZE)
    {
#ifndef IN_PROCESS_AGENT
      if (!variable_size_registers)
	// FIXME: Remove cast.
	const_cast<struct regcache *> (this)->initialize_variable_size_registers ();

      return variable_size_sizes[regnum];
#else
      gdb_assert_not_reached ("Variable-size registers not supported.");
#endif
    }
  else
    return reg.size / 8;
}

static gdb::array_view<gdb_byte>
register_data (const struct regcache *regcache, int n)
{
  const gdb::reg &reg = find_register_by_number (regcache->tdesc, n);

  if (reg.size == TDESC_REG_VARIABLE_SIZE)
    {
#ifndef IN_PROCESS_AGENT
      if (!regcache->variable_size_registers)
	// FIXME: Remove cast.
	const_cast<struct regcache *> (regcache)->initialize_variable_size_registers ();

      return gdb::make_array_view (regcache->variable_size_registers.get ()
				   + regcache->variable_size_offsets[n],
				   regcache->variable_size_sizes[n]);
#else
      gdb_assert_not_reached ("Variable-size registers not supported.");
#endif
    }
  else
    return gdb::make_array_view (regcache->fixed_size_registers + reg.offset / 8,
				 reg.size / 8);
}

void
supply_register (struct regcache *regcache, int n, const void *vbuf)
{
  const gdb::reg &reg = find_register_by_number (regcache->tdesc, n);
  const gdb_byte *buf = static_cast<const gdb_byte *> (vbuf);
  return regcache->raw_supply (n, gdb::make_array_view (buf, reg.size / 8));
}

/* See gdbsupport/common-regcache.h.  */

void
regcache::raw_supply (int n, gdb::array_view<const gdb_byte> src)
{
  auto dst = register_data (this, n);

  if (src.data () != nullptr)
    {
      copy (src, dst);
#ifndef IN_PROCESS_AGENT
      set_register_status (n, REG_VALID);
#endif
    }
  else
    {
      memset (dst.data (), 0, dst.size ());
#ifndef IN_PROCESS_AGENT
      set_register_status (n, REG_UNAVAILABLE);
#endif
    }

#ifndef IN_PROCESS_AGENT
  if (target_is_register_relevant_for_tdesc_parameter (this->tdesc, n))
    /* Invalidate variable-size registers.  */
    this->invalidate_variable_size_registers ();
#endif
}

/* Supply register N with value zero to REGCACHE.  */

void
supply_register_zeroed (struct regcache *regcache, int n)
{
  auto dst = register_data (regcache, n);
  memset (dst.data (), 0, dst.size ());
#ifndef IN_PROCESS_AGENT
  regcache->set_register_status (n, REG_VALID);
#endif
}

void
regcache::raw_supply_part_zeroed (int regnum, int offset, size_t size)
{
  auto dst = register_data (this, regnum).slice (offset, size);
  memset (dst.data (), 0, dst.size ());
#ifndef IN_PROCESS_AGENT
  set_register_status (regnum, REG_VALID);
#endif
}

#ifndef IN_PROCESS_AGENT

/* Supply register called NAME with value zero to REGCACHE.  */

void
supply_register_by_name_zeroed (struct regcache *regcache,
				const char *name)
{
  supply_register_zeroed (regcache, find_regno (regcache->tdesc, name));
}

#endif

/* Supply the whole register set whose contents are stored in BUF, to
   REGCACHE.  */

void
supply_regblock (struct regcache *regcache, const void *buf)
{
  gdb_assert (buf != nullptr);
  const struct target_desc *tdesc = regcache->tdesc;

  memcpy (regcache->fixed_size_registers, buf, tdesc->fixed_registers_size);
#ifndef IN_PROCESS_AGENT
  for (int i = 0; i < tdesc->reg_defs.size (); i++)
    regcache->set_register_status (i, REG_VALID);
#endif
}

#ifndef IN_PROCESS_AGENT

void
supply_register_by_name (struct regcache *regcache,
			 const char *name, const void *buf)
{
  supply_register (regcache, find_regno (regcache->tdesc, name), buf);
}

#endif

void
collect_register (struct regcache *regcache, int n, void *vbuf)
{
  const gdb::reg &reg = find_register_by_number (regcache->tdesc, n);
  gdb_byte *buf = static_cast<gdb_byte *> (vbuf);
  regcache->raw_collect (n, gdb::make_array_view (buf, reg.size / 8));
}

/* See gdbsupport/common-regcache.h.  */

void
regcache::raw_collect (int n, gdb::array_view<gdb_byte> dst) const
{
  auto src = register_data (this, n);
  copy (src, dst);
}

enum register_status
regcache_raw_read_unsigned (reg_buffer_common *reg_buf, int regnum,
			    ULONGEST *val)
{
  int size;
  regcache *regcache = gdb::checked_static_cast<struct regcache *> (reg_buf);

  gdb_assert (regcache != NULL);

  size = regcache->register_size (regnum);

  if (size > (int) sizeof (ULONGEST))
    error (_("That operation is not available on integers of more than"
	    "%d bytes."),
	  (int) sizeof (ULONGEST));

  *val = 0;
  collect_register (regcache, regnum, val);

  return regcache->get_register_status (regnum);
}

#ifndef IN_PROCESS_AGENT

/* See regcache.h.  */

ULONGEST
regcache_raw_get_unsigned_by_name (struct regcache *regcache,
				   const char *name)
{
  return regcache_raw_get_unsigned (regcache,
				    find_regno (regcache->tdesc, name));
}

void
collect_register_as_string (struct regcache *regcache, int n, char *buf)
{
  int reg_size = regcache->register_size (n);

  if (regcache->get_register_status (n) == REG_VALID)
    bin2hex (register_data (regcache, n), buf);
  else
    memset (buf, 'x', reg_size * 2);

  buf += reg_size * 2;
  *buf = '\0';
}

void
collect_register_by_name (struct regcache *regcache,
			  const char *name, void *buf)
{
  collect_register (regcache, find_regno (regcache->tdesc, name), buf);
}

/* Special handling for register PC.  */

CORE_ADDR
regcache_read_pc (reg_buffer_common *regcache)
{
  return the_target->read_pc
    (gdb::checked_static_cast<struct regcache *> (regcache));
}

void
regcache_write_pc (struct regcache *regcache, CORE_ADDR pc)
{
  the_target->write_pc (regcache, pc);
}

#endif

/* See gdbsupport/common-regcache.h.  */

enum register_status
regcache::get_register_status (int regnum) const
{
#ifndef IN_PROCESS_AGENT
  gdb_assert (regnum >= 0 && regnum < tdesc->reg_defs.size ());
  if (m_register_status != nullptr)
    return m_register_status[regnum];
  else
    return REG_VALID;
#else
  return REG_VALID;
#endif
}

void
regcache::set_register_status (int regnum, enum register_status status)
{
#ifndef IN_PROCESS_AGENT
  gdb_assert (regnum >= 0 && regnum < tdesc->reg_defs.size ());
  if (m_register_status != nullptr)
    m_register_status[regnum] = status;
#endif
}

/* See gdbsupport/common-regcache.h.  */

bool
regcache::raw_compare (int regnum, const void *buf, int offset) const
{
  gdb_assert (buf != NULL);

  gdb::array_view<const gdb_byte> regbuf = register_data (this, regnum);
  gdb_assert (offset <= regbuf.size ());
  regbuf = regbuf.slice (offset);

  return memcmp (buf, regbuf.data (), regbuf.size ()) == 0;
}
