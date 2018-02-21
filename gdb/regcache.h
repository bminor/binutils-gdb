/* Cache and manage the values of registers for GDB, the GNU debugger.

   Copyright (C) 1986-2018 Free Software Foundation, Inc.

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

#ifndef REGCACHE_H
#define REGCACHE_H

#include "common-regcache.h"
#include <forward_list>

struct regcache;
struct regset;
struct gdbarch;
struct address_space;

extern struct regcache *get_current_regcache (void);
extern struct regcache *get_thread_regcache (ptid_t ptid);
extern struct regcache *get_thread_arch_regcache (ptid_t, struct gdbarch *);
extern struct regcache *get_thread_arch_aspace_regcache (ptid_t,
							 struct gdbarch *,
							 struct address_space *);

/* Return REGCACHE's ptid.  */

extern ptid_t regcache_get_ptid (const struct regcache *regcache);

enum register_status regcache_register_status (const struct regcache *regcache,
					       int regnum);

/* Make certain that the register REGNUM in REGCACHE is up-to-date.  */

void regcache_raw_update (struct regcache *regcache, int regnum);

/* Transfer a raw register [0..NUM_REGS) between core-gdb and the
   regcache.  The read variants return the status of the register.  */

enum register_status regcache_raw_read (struct regcache *regcache,
					int rawnum, gdb_byte *buf);
void regcache_raw_write (struct regcache *regcache, int rawnum,
			 const gdb_byte *buf);
extern enum register_status
  regcache_raw_read_signed (struct regcache *regcache,
			    int regnum, LONGEST *val);

extern void regcache_raw_write_signed (struct regcache *regcache,
				       int regnum, LONGEST val);
extern void regcache_raw_write_unsigned (struct regcache *regcache,
					 int regnum, ULONGEST val);

/* Return the register's value in signed or throw if it's not
   available.  */

extern LONGEST regcache_raw_get_signed (struct regcache *regcache,
					int regnum);

/* Partial transfer of raw registers.  These perform read, modify,
   write style operations.  The read variant returns the status of the
   register.  */

extern enum register_status
  regcache_raw_read_part (struct regcache *regcache, int regnum,
			  int offset, int len, gdb_byte *buf);
void regcache_raw_write_part (struct regcache *regcache, int regnum,
			      int offset, int len, const gdb_byte *buf);

void regcache_invalidate (struct regcache *regcache, int regnum);

/* Transfer of pseudo-registers.  The read variants return a register
   status, as an indication of when a ``cooked'' register was
   constructed from valid, invalid or unavailable ``raw''
   registers.  */

/* Transfer a cooked register [0..NUM_REGS+NUM_PSEUDO_REGS).  */
enum register_status regcache_cooked_read (struct regcache *regcache,
					   int rawnum, gdb_byte *buf);
void regcache_cooked_write (struct regcache *regcache, int rawnum,
			    const gdb_byte *buf);

/* Read register REGNUM from REGCACHE and return a new value.  This
   will call mark_value_bytes_unavailable as appropriate.  */

struct value *regcache_cooked_read_value (struct regcache *regcache,
					  int regnum);

/* Read a register as a signed/unsigned quantity.  */
extern enum register_status
  regcache_cooked_read_signed (struct regcache *regcache,
			       int regnum, LONGEST *val);
extern enum register_status
  regcache_cooked_read_unsigned (struct regcache *regcache,
				 int regnum, ULONGEST *val);
extern void regcache_cooked_write_signed (struct regcache *regcache,
					  int regnum, LONGEST val);
extern void regcache_cooked_write_unsigned (struct regcache *regcache,
					    int regnum, ULONGEST val);

/* Partial transfer of a cooked register.  These perform read, modify,
   write style operations.  */

enum register_status regcache_cooked_read_part (struct regcache *regcache,
						int regnum, int offset,
						int len, gdb_byte *buf);
void regcache_cooked_write_part (struct regcache *regcache, int regnum,
				 int offset, int len, const gdb_byte *buf);

/* Special routines to read/write the PC.  */

/* For regcache_read_pc see common/common-regcache.h.  */
extern void regcache_write_pc (struct regcache *regcache, CORE_ADDR pc);

/* Transfer a raw register [0..NUM_REGS) between the regcache and the
   target.  These functions are called by the target in response to a
   target_fetch_registers() or target_store_registers().  */

extern void regcache_raw_supply (struct regcache *regcache,
				 int regnum, const void *buf);
extern void regcache_raw_collect (const struct regcache *regcache,
				  int regnum, void *buf);

/* Mapping between register numbers and offsets in a buffer, for use
   in the '*regset' functions below.  In an array of
   'regcache_map_entry' each element is interpreted like follows:

   - If 'regno' is a register number: Map register 'regno' to the
     current offset (starting with 0) and increase the current offset
     by 'size' (or the register's size, if 'size' is zero).  Repeat
     this with consecutive register numbers up to 'regno+count-1'.

   - If 'regno' is REGCACHE_MAP_SKIP: Add 'count*size' to the current
     offset.

   - If count=0: End of the map.  */

struct regcache_map_entry
{
  int count;
  int regno;
  int size;
};

/* Special value for the 'regno' field in the struct above.  */

enum
  {
    REGCACHE_MAP_SKIP = -1,
  };

/* Transfer a set of registers (as described by REGSET) between
   REGCACHE and BUF.  If REGNUM == -1, transfer all registers
   belonging to the regset, otherwise just the register numbered
   REGNUM.  The REGSET's 'regmap' field must point to an array of
   'struct regcache_map_entry'.

   These functions are suitable for the 'regset_supply' and
   'regset_collect' fields in a regset structure.  */

extern void regcache_supply_regset (const struct regset *regset,
				    struct regcache *regcache,
				    int regnum, const void *buf,
				    size_t size);
extern void regcache_collect_regset (const struct regset *regset,
				     const struct regcache *regcache,
				     int regnum, void *buf, size_t size);


/* The type of a register.  This function is slightly more efficient
   then its gdbarch vector counterpart since it returns a precomputed
   value stored in a table.  */

extern struct type *register_type (struct gdbarch *gdbarch, int regnum);


/* Return the size of register REGNUM.  All registers should have only
   one size.  */
   
extern int register_size (struct gdbarch *gdbarch, int regnum);

typedef enum register_status (regcache_cooked_read_ftype) (void *src,
							   int regnum,
							   gdb_byte *buf);

/* A (register_number, register_value) pair.  */

typedef struct cached_reg
{
  int num;
  gdb_byte *data;
} cached_reg_t;

/* Buffer of registers.  */

class reg_buffer
{
public:
  reg_buffer (gdbarch *gdbarch, bool has_pseudo);

  DISABLE_COPY_AND_ASSIGN (reg_buffer);

  /* Return regcache's architecture.  */
  gdbarch *arch () const;

  enum register_status get_register_status (int regnum) const;

  virtual ~reg_buffer ()
  {
    xfree (m_registers);
    xfree (m_register_status);
  }
protected:
  /* Assert on the range of REGNUM.  */
  void assert_regnum (int regnum) const;

  int num_raw_registers () const;

  gdb_byte *register_buffer (int regnum) const;

  /* Save a register cache.  The set of registers saved into the
     regcache determined by the save_reggroup.  COOKED_READ returns
     zero iff the register's value can't be returned.  */
  void save (regcache_cooked_read_ftype *cooked_read, void *src);

  struct regcache_descr *m_descr;

  bool m_has_pseudo;
  /* The register buffers.  */
  gdb_byte *m_registers;
  /* Register cache status.  */
  signed char *m_register_status;

  friend class regcache;
  friend class detached_regcache;
};

/* An abstract class which only has methods doing read.  */

class readable_regcache : public reg_buffer
{
public:
  readable_regcache (gdbarch *gdbarch, bool has_pseudo)
    : reg_buffer (gdbarch, has_pseudo)
  {}

  enum register_status raw_read (int regnum, gdb_byte *buf);
  template<typename T, typename = RequireLongest<T>>
  enum register_status raw_read (int regnum, T *val);

  enum register_status raw_read_part (int regnum, int offset, int len,
				      gdb_byte *buf);

  virtual void raw_update (int regnum) = 0;

  enum register_status cooked_read (int regnum, gdb_byte *buf);
  template<typename T, typename = RequireLongest<T>>
  enum register_status cooked_read (int regnum, T *val);

  enum register_status cooked_read_part (int regnum, int offset, int len,
					 gdb_byte *buf);

  struct value *cooked_read_value (int regnum);

protected:
  enum register_status read_part (int regnum, int offset, int len, void *in,
				  bool is_raw);
};

/* Buffer of registers, can be read and written.  */

class detached_regcache : public readable_regcache
{
public:
  detached_regcache (gdbarch *gdbarch, bool has_pseudo)
    : readable_regcache (gdbarch, has_pseudo)
  {}

  void raw_supply (int regnum, const void *buf);

  void raw_supply (int regnum, const reg_buffer &src)
  {
    raw_supply (regnum, src.register_buffer (regnum));
  }

  void raw_update (int regnum) override
  {}

  void raw_supply_integer (int regnum, const gdb_byte *addr, int addr_len,
			   bool is_signed);

  void raw_supply_zeroed (int regnum);

  void invalidate (int regnum);

  DISABLE_COPY_AND_ASSIGN (detached_regcache);
};

class readonly_detached_regcache;

/* The register cache for storing raw register values.  */

class regcache : public detached_regcache
{
public:
  DISABLE_COPY_AND_ASSIGN (regcache);

  /* Return REGCACHE's address space.  */
  const address_space *aspace () const
  {
    return m_aspace;
  }

  /* Restore 'this' regcache.  The set of registers restored into
     the regcache determined by the restore_reggroup.
     Writes to regcache will go through to the target.  SRC is a
     read-only register cache.  */
  void restore (readonly_detached_regcache *src);

  void cooked_write (int regnum, const gdb_byte *buf);

  void raw_write (int regnum, const gdb_byte *buf);

  template<typename T, typename = RequireLongest<T>>
  void raw_write (int regnum, T val);

  template<typename T, typename = RequireLongest<T>>
  void cooked_write (int regnum, T val);

  void raw_update (int regnum) override;

  void raw_collect (int regnum, void *buf) const;

  void raw_collect_integer (int regnum, gdb_byte *addr, int addr_len,
			    bool is_signed) const;

  void raw_write_part (int regnum, int offset, int len, const gdb_byte *buf);

  void cooked_write_part (int regnum, int offset, int len,
			  const gdb_byte *buf);

  void supply_regset (const struct regset *regset,
		      int regnum, const void *buf, size_t size);


  void collect_regset (const struct regset *regset, int regnum,
		       void *buf, size_t size) const;

  ptid_t ptid () const
  {
    return m_ptid;
  }

  void set_ptid (const ptid_t ptid)
  {
    this->m_ptid = ptid;
  }

/* Dump the contents of a register from the register cache to the target
   debug.  */
  void debug_print_register (const char *func, int regno);

  static void regcache_thread_ptid_changed (ptid_t old_ptid, ptid_t new_ptid);
protected:
  regcache (gdbarch *gdbarch, const address_space *aspace_);
  static std::forward_list<regcache *> current_regcache;

private:

  void transfer_regset (const struct regset *regset,
			struct regcache *out_regcache,
			int regnum, const void *in_buf,
			void *out_buf, size_t size) const;

  enum register_status write_part (int regnum, int offset, int len,
				   const void *out, bool is_raw);


  /* The address space of this register cache (for registers where it
     makes sense, like PC or SP).  */
  const address_space * const m_aspace;

  /* If this is a read-write cache, which thread's registers is
     it connected to?  */
  ptid_t m_ptid;

  friend struct regcache *
  get_thread_arch_aspace_regcache (ptid_t ptid, struct gdbarch *gdbarch,
				   struct address_space *aspace);

  friend void
  registers_changed_ptid (ptid_t ptid);
};

class readonly_detached_regcache : public readable_regcache
{
public:
  readonly_detached_regcache (const regcache &src);

  /* Create a readonly regcache by getting contents from COOKED_READ.  */

  readonly_detached_regcache (gdbarch *gdbarch,
			      regcache_cooked_read_ftype *cooked_read,
			      void *src)
    : readable_regcache (gdbarch, true)
  {
    save (cooked_read, src);
  }

  DISABLE_COPY_AND_ASSIGN (readonly_detached_regcache);

  void raw_update (int regnum) override
  {}
};

extern void registers_changed (void);
extern void registers_changed_ptid (ptid_t);

/* An abstract base class for register dump.  */

class register_dump
{
public:
  void dump (ui_file *file);
  virtual ~register_dump () = default;

protected:
  register_dump (gdbarch *arch)
    : m_gdbarch (arch)
  {}

  /* Dump the register REGNUM contents.  If REGNUM is -1, print the
     header.  */
  virtual void dump_reg (ui_file *file, int regnum) = 0;

  gdbarch *m_gdbarch;
};

#endif /* REGCACHE_H */
