/* DWARF 2 Expression Evaluator.

   Copyright (C) 2001-2020 Free Software Foundation, Inc.

   Contributed by Daniel Berlin (dan@dberlin.org)

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
#include "block.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "value.h"
#include "gdbcore.h"
#include "dwarf2.h"
#include "dwarf2/expr.h"
#include "dwarf2/loc.h"
#include "dwarf2/read.h"
#include "frame.h"
#include "gdbsupport/underlying.h"
#include "gdbarch.h"
#include "inferior.h"
#include "observable.h"

/* Cookie for gdbarch data.  */

static struct gdbarch_data *dwarf_arch_cookie;

/* This holds gdbarch-specific types used by the DWARF expression
   evaluator.  See comments in execute_stack_op.  */

struct dwarf_gdbarch_types
{
  struct type *dw_types[3];
};

/* Allocate and fill in dwarf_gdbarch_types for an arch.  */

static void *
dwarf_gdbarch_types_init (struct gdbarch *gdbarch)
{
  struct dwarf_gdbarch_types *types
    = GDBARCH_OBSTACK_ZALLOC (gdbarch, struct dwarf_gdbarch_types);

  /* The types themselves are lazily initialized.  */

  return types;
}

/* Ensure that a FRAME is defined, throw an exception otherwise.  */

static void
ensure_have_frame (struct frame_info *frame, const char *op_name)
{
  if (frame == nullptr)
    throw_error (GENERIC_ERROR,
		 _("%s evaluation requires a frame."), op_name);
}

/* Ensure that a PER_CU is defined and throw an exception otherwise.  */

static void
ensure_have_per_cu (struct dwarf2_per_cu_data *per_cu, const char* op_name)
{
  if (per_cu == nullptr)
    throw_error (GENERIC_ERROR,
		 _("%s evaluation requires a compilation unit."), op_name);
}

/* Return the number of bytes overlapping a contiguous chunk of N_BITS
   bits whose first bit is located at bit offset START.  */

static size_t
bits_to_bytes (ULONGEST start, ULONGEST n_bits)
{
  return (start % HOST_CHAR_BIT + n_bits + HOST_CHAR_BIT - 1) / HOST_CHAR_BIT;
}

/* Throw an exception about the invalid DWARF expression.  */

static void
ill_formed_expression ()
{
  error (_("Ill-formed DWARF expression"));
}

/* Read register REGNUM's contents in a given FRAME context.

   The data read is offsetted by OFFSET, and the number of bytes read
   is defined by LENGTH.  The data is then copied into the
   caller-managed buffer BUF.

   If the register is optimized out or unavailable for the given
   FRAME, the OPTIMIZED and UNAVAILABLE outputs are set
   accordingly  */

static void
read_from_register (struct frame_info *frame, int regnum,
		    CORE_ADDR offset, int length, gdb_byte *buf,
		    int *optimized, int *unavailable)
{
  struct gdbarch *gdbarch = get_frame_arch (frame);
  int regsize = register_size (gdbarch, regnum);
  int numregs = gdbarch_num_cooked_regs (gdbarch);

  /* If a register is wholly inside the OFFSET, skip it.  */
  if (frame == NULL || !regsize
      || (offset + length) > regsize || numregs < regnum)
    {
      (*optimized) = 0;
      (*unavailable) = 1;
      return;
    }

  gdb::byte_vector temp_buf (regsize);
  enum lval_type lval;
  CORE_ADDR address;
  int realnum;

  frame_register (frame, regnum, optimized, unavailable,
		  &lval, &address, &realnum, temp_buf.data ());

  if (!(*optimized) && !(*unavailable))
     memcpy (buf, (char *) temp_buf.data () + offset, length);

  return;
}

/* Write register REGNUM's contents in a given FRAME context.

   The data written is offsetted by OFFSET, and the number of bytes
   written is defined by LENGTH.  The data is copied from
   caller-managed buffer BUF.

   If the register is optimized out or unavailable for the given
   FRAME, the OPTIMIZED and UNAVAILABLE outputs are set
   accordingly. */

static void
write_to_register (struct frame_info *frame, int regnum,
		   CORE_ADDR offset, int length, gdb_byte *buf,
		   int *optimized, int *unavailable)
{
  struct gdbarch *gdbarch = get_frame_arch (frame);
  int regsize = register_size (gdbarch, regnum);
  int numregs = gdbarch_num_cooked_regs (gdbarch);

  /* If a register is wholly inside of OFFSET, skip it.  */
  if (frame == NULL || !regsize
     || (offset + length) > regsize || numregs < regnum)
    {
      (*optimized) = 0;
      (*unavailable) = 1;
      return;
    }

  gdb::byte_vector temp_buf (regsize);
  enum lval_type lval;
  CORE_ADDR address;
  int realnum;

  frame_register (frame, regnum, optimized, unavailable,
		  &lval, &address, &realnum, temp_buf.data ());

  if (!(*optimized) && !(*unavailable))
    {
      memcpy ((char *) temp_buf.data () + offset, buf, length);

      put_frame_register (frame, regnum, temp_buf.data ());
    }

  return;
}

/* Helper for read_from_memory and write_from_memory.  */

static void
xfer_from_memory (CORE_ADDR address, gdb_byte *readbuf,
		  const gdb_byte *writebuf,
		  size_t length, bool stack, int *unavailable)
{
  (*unavailable) = 0;

  enum target_object object
    = stack ? TARGET_OBJECT_STACK_MEMORY : TARGET_OBJECT_MEMORY;

  ULONGEST xfered_total = 0;

  while (xfered_total < length)
    {
      ULONGEST xfered_partial;

      enum target_xfer_status status
	= target_xfer_partial (current_top_target (), object, NULL,
			       (readbuf != nullptr
				? readbuf + xfered_total
				: nullptr),
			       (writebuf != nullptr
				? writebuf + xfered_total
				: nullptr),
			       address + xfered_total, length - xfered_total,
			       &xfered_partial);

      if (status == TARGET_XFER_OK)
	{
	  xfered_total += xfered_partial;
	  QUIT;
	}
      else if (status == TARGET_XFER_UNAVAILABLE)
	{
	  (*unavailable) = 1;
	  return;
	}
      else if (status == TARGET_XFER_EOF)
	memory_error (TARGET_XFER_E_IO, address + xfered_total);
      else
	memory_error (status, address + xfered_total);
    }
}

/* Read LENGTH bytes of memory contents starting at ADDRESS.

   The data read is copied to a caller-managed buffer BUF.  STACK
   indicates whether the memory range specified belongs to a stack
   memory region.

   If the memory is unavailable, the UNAVAILABLE output is set.  */

static void
read_from_memory (CORE_ADDR address, gdb_byte *buffer,
		  size_t length, bool stack, int* unavailable)
{
  xfer_from_memory (address, buffer, nullptr, length, stack, unavailable);
}

/* Write LENGTH bytes of memory contents starting at ADDRESS.

   The data written is copied from a caller-managed buffer buf.  STACK
   indicates whether the memory range specified belongs to a stack
   memory region.

   If the memory is unavailable, the UNAVAILABLE output is set.  */

static void
write_to_memory (CORE_ADDR address, const gdb_byte *buffer,
		 size_t length, bool stack, int *unavailable)
{
  xfer_from_memory (address, nullptr, buffer, length, stack, unavailable);

  gdb::observers::memory_changed.notify (current_inferior (), address,
					 length, buffer);
}

/* Base class that describes entries found on a DWARF expression
   evaluation stack.  */

class dwarf_entry : public refcounted_object
{
public:
  /* Not expected to be called on it's own.  */
  dwarf_entry () = default;

  virtual ~dwarf_entry () = 0;
};

dwarf_entry::~dwarf_entry () = default;

/* Value entry found on a DWARF expression evaluation stack.  */

class dwarf_value : public dwarf_entry
{
public:
  dwarf_value (const gdb_byte* contents, struct type *type)
  {
    size_t type_len = TYPE_LENGTH (type);
    m_contents.reset ((gdb_byte *) xzalloc (type_len));

    memcpy (m_contents.get (), contents, type_len);
    m_type = type;
  }

  dwarf_value (ULONGEST value, struct type *type)
  {
    m_contents.reset ((gdb_byte *) xzalloc (TYPE_LENGTH (type)));

    pack_unsigned_long (m_contents.get (), type, value);
    m_type = type;
  }

  dwarf_value (LONGEST value, struct type *type)
  {
    m_contents.reset ((gdb_byte *) xzalloc (TYPE_LENGTH (type)));

    pack_long (m_contents.get (), type, value);
    m_type = type;
  }

  virtual ~dwarf_value () = default;

  const gdb_byte* get_contents () const
  {
    return m_contents.get ();
  }

  struct type* get_type () const
  {
    return m_type;
  }

  LONGEST to_long () const
  {
    return unpack_long (m_type, m_contents.get ());
  }

private:
  /* Value contents as a stream of bytes in target byte order.  */
  gdb::unique_xmalloc_ptr<gdb_byte> m_contents;

  /* Type of the value held by the entry.  */
  struct type *m_type;
};

/* Location description entry found on a DWARF expression evaluation stack.

   Types of locations descirbed can be: register location, memory location,
   implicit location, implicit pointer location, undefined location and
   composite location (made out of any of the location types including
   another composite location).  */

class dwarf_location : public dwarf_entry
{
public:
  /* Not expected to be called on it's own.  */
  dwarf_location (LONGEST offset = 0, LONGEST bit_suboffset = 0)
    : m_initialised (true)
  {
    m_offset = offset;
    m_offset += bit_suboffset / HOST_CHAR_BIT;
    m_bit_suboffset = bit_suboffset % HOST_CHAR_BIT;
  }

  virtual ~dwarf_location () = default;

  LONGEST get_offset () const
  {
    return m_offset;
  };

  LONGEST get_bit_suboffset () const
  {
    return m_bit_suboffset;
  };

  void add_bit_offset (LONGEST bit_offset)
  {
    LONGEST bit_total_offset = m_bit_suboffset + bit_offset;

    m_offset += bit_total_offset / HOST_CHAR_BIT;
    m_bit_suboffset = bit_total_offset % HOST_CHAR_BIT;
  };

  void set_initialised (bool initialised)
  {
    m_initialised = initialised;
  };

  bool is_initialised () const
  {
    return m_initialised;
  };

private:
  /* Byte offset into the location.  */
  LONGEST m_offset;

  /* Bit suboffset of the last byte.  */
  LONGEST m_bit_suboffset;

  /* Whether the location is initialized.  Used for non-standard
     DW_OP_GNU_uninit operation.  */
  bool m_initialised;
};

/* Undefined location description entry.  This is a special location
   description type that describes the location description that is
   not known.  */

class dwarf_undefined : public dwarf_location
{
public:
  dwarf_undefined (LONGEST offset = 0, LONGEST bit_suboffset = 0)
    : dwarf_location (offset, bit_suboffset)
  {}
};

class dwarf_memory : public dwarf_location
{
public:
  dwarf_memory (LONGEST offset, LONGEST bit_suboffset = 0,
		bool stack = false)
    : dwarf_location (offset, bit_suboffset),
      m_stack (stack)
  {}

  bool in_stack () const
  {
    return m_stack;
  };

  void set_stack (bool stack)
  {
    m_stack = stack;
  };
private:
  /* True if the location belongs to a stack memory region.  */
  bool m_stack;
};

/* Register location description entry.  */

class dwarf_register : public dwarf_location
{
public:
  dwarf_register (unsigned int regnum,
		  LONGEST offset = 0, LONGEST bit_suboffset = 0)
    : dwarf_location (offset, bit_suboffset),
      m_regnum (regnum)
  {}

  unsigned int get_regnum () const
  {
    return m_regnum;
  };

private:
  /* DWARF register number.  */
  unsigned int m_regnum;
};

/* Implicit location description entry.  Describes a location
   description not found on the target but instead saved in a
   gdb-allocated buffer.  */

class dwarf_implicit : public dwarf_location
{
public:

  dwarf_implicit (const gdb_byte* contents, size_t size,
		  enum bfd_endian byte_order)
  {
    m_contents.reset ((gdb_byte *) xzalloc (size));

    memcpy (m_contents.get (), contents, size);
    m_size = size;
    m_byte_order = byte_order;
  }

  const gdb_byte* get_contents () const
  {
    return m_contents.get ();
  }

  size_t get_size () const
  {
    return m_size;
  }

  size_t get_byte_order () const
  {
    return m_byte_order;
  }

private:
  /* Implicit location contents as a stream of bytes in target byte-order.  */
  gdb::unique_xmalloc_ptr<gdb_byte> m_contents;

  /* Contents byte stream size.  */
  size_t m_size;

  /* Contents original byte order.  */
  enum bfd_endian m_byte_order;
};

/* Implicit pointer location description entry.  */

class dwarf_implicit_pointer : public dwarf_location
{
public:
  dwarf_implicit_pointer (dwarf2_per_objfile *per_objfile,
			  struct dwarf2_per_cu_data *per_cu,
			  int addr_size, sect_offset die_offset,
			  LONGEST offset, LONGEST bit_suboffset = 0)
    : dwarf_location (offset, bit_suboffset),
      m_per_objfile (per_objfile), m_per_cu (per_cu),
      m_addr_size (addr_size), m_die_offset (die_offset)
  {}

  dwarf2_per_objfile *get_per_objfile () const
  {
    return m_per_objfile;
  }

  dwarf2_per_cu_data *get_per_cu () const
  {
    return m_per_cu;
  }

  int get_addr_size () const
  {
    return m_addr_size;
  }

  sect_offset get_die_offset () const
  {
    return m_die_offset;
  }

private:
  /* Per object file data of the implicit pointer.  */
  dwarf2_per_objfile *m_per_objfile;

  /* Compilation unit context of the implicit pointer.  */
  struct dwarf2_per_cu_data *m_per_cu;

  /* Address size for the evaluation.  */
  int m_addr_size;

  /* DWARF die offset pointed by the implicit pointer.  */
  sect_offset m_die_offset;
};

/* Composite location description entry.  */

class dwarf_composite : public dwarf_location
{
public:
  dwarf_composite (LONGEST offset = 0, LONGEST bit_suboffset = 0)
    : dwarf_location (offset, bit_suboffset)
  {}

  /* A composite location gets detached from its factory object for
     the purpose of lval_computed resolution, which means that it
     needs to take care of garbage collecting its pieces.  */
  ~dwarf_composite () override
  {
    for (unsigned int i = 0; i < m_pieces.size (); i++)
      {
	dwarf_location* location = m_pieces[i].m_location;

	location->decref ();

	if (location->refcount () == 0)
	  delete location;
      }
  }

  void add_piece (dwarf_location* location, ULONGEST bit_size)
  {
    gdb_assert (location != nullptr);
    location->incref ();
    m_pieces.emplace_back (location, bit_size);
  }

  const dwarf_location* get_piece_at (unsigned int index) const
  {
    gdb_assert (index < m_pieces.size ());
    return m_pieces[index].m_location;
  }

  ULONGEST get_bit_size_at (unsigned int index) const
  {
    gdb_assert (index < m_pieces.size ());
    return m_pieces[index].m_size;
  }

  size_t get_pieces_num () const
  {
    return m_pieces.size ();
  }

private:
  /* Composite piece that contains a piece location
     description and it's size.  */
  class piece
  {
  public:
    piece (dwarf_location *location, ULONGEST size)
    : m_location (location),
      m_size (size)
    {}

    dwarf_location *m_location;
    ULONGEST m_size;
  };

  /* Vector of composite pieces.  */
  std::vector<struct piece> m_pieces;
};

/* Read contents from the location specified by the DWARF location
   description entry LOCATION.

   The read operation is performed in the context of FRAME.  BIT_SIZE
   is the number of bits to read.  The data read is copied to the
   caller-managed buffer BUF.  BIG_ENDIAN defines the endianness of
   the target.  BITS_TO_SKIP is a bit offset into the location and
   BUF_BIT_OFFSET is buffer BUF's bit offset.  LOCATION_BIT_LIMIT is a
   a maximum number of bits that location can hold, where value zero
   signifies that there is no such restriction.

   Note that some location types can be read without a FRAME context.

   If the location is optimized out or unavailable, the OPTIMIZED and
   UNAVAILABLE outputs are set accordingly.  */

static void
read_from_location (const dwarf_location *location, struct frame_info *frame,
		    LONGEST bits_to_skip, gdb_byte *buf, int buf_bit_offset,
		    size_t bit_size, size_t location_bit_limit,
		    bool big_endian, int* optimized, int* unavailable)
{
  LONGEST offset = location->get_offset ();
  LONGEST bit_suboffset = location->get_bit_suboffset ();
  LONGEST total_bits_to_skip = bits_to_skip;
  size_t read_bit_limit = location_bit_limit;
  gdb::byte_vector temp_buf;

  /* Reads from undefined locations are always marked as optimized
     out.  */
  if (dynamic_cast<const dwarf_undefined *> (location) != nullptr)
    {
      (*unavailable) = 0;
      (*optimized) = 1;
    }
  else if (auto register_entry
	     = dynamic_cast<const dwarf_register *> (location))
    {
      struct gdbarch *arch = get_frame_arch (frame);
      int reg = dwarf_reg_to_regnum_or_error (arch,
					      register_entry->get_regnum ());
      ULONGEST reg_bits = HOST_CHAR_BIT * register_size (arch, reg);

      if (big_endian)
	{
	  if (!read_bit_limit || reg_bits <= read_bit_limit)
	    read_bit_limit = bit_size;

	  total_bits_to_skip
	    += reg_bits - (offset * HOST_CHAR_BIT
			   + bit_suboffset + read_bit_limit);
	}
      else
	total_bits_to_skip += offset * HOST_CHAR_BIT + bit_suboffset;

      LONGEST this_size = bits_to_bytes (total_bits_to_skip, bit_size);
      temp_buf.resize (this_size);

      /* Can only read from a register on byte granularity so an
	 additional buffer is required.  */
      read_from_register (frame, reg, total_bits_to_skip / HOST_CHAR_BIT,
			  this_size, temp_buf.data (), optimized, unavailable);

      /* Only copy data if valid.  */
      if (!(*optimized) && !(*unavailable))
	copy_bitwise (buf, buf_bit_offset, temp_buf.data (),
		      total_bits_to_skip % HOST_CHAR_BIT,
		      bit_size, big_endian);
    }
  else if (auto memory_entry = dynamic_cast<const dwarf_memory *> (location))
    {
      CORE_ADDR start_address
	= offset + (bit_suboffset + total_bits_to_skip) / HOST_CHAR_BIT;

      (*optimized) = 0;
      total_bits_to_skip += bit_suboffset;

      if ((total_bits_to_skip % HOST_CHAR_BIT) == 0
	  && (bit_size % HOST_CHAR_BIT) == 0
	  && (buf_bit_offset % HOST_CHAR_BIT) == 0)
	{
	  /* Everything is byte-aligned, no buffer needed.  */
	  read_from_memory (start_address,
			    buf + buf_bit_offset / HOST_CHAR_BIT,
			    bit_size / HOST_CHAR_BIT,
			    memory_entry->in_stack (), unavailable);
	}
      else
	{
	  LONGEST this_size = bits_to_bytes (total_bits_to_skip, bit_size);
	  temp_buf.resize (this_size);

	  /* Can only read from memory on byte granularity so an
	     additional buffer is required.  */
	  read_from_memory (start_address, temp_buf.data (), this_size,
			    memory_entry->in_stack (), unavailable);

	  if (!(*unavailable))
	    copy_bitwise (buf, buf_bit_offset, temp_buf.data (),
			  total_bits_to_skip % HOST_CHAR_BIT,
			  bit_size, big_endian);
	}
    }
  else if (auto implicit_entry
	     = dynamic_cast<const dwarf_implicit *> (location))
    {
      ULONGEST literal_bit_size = HOST_CHAR_BIT * implicit_entry->get_size ();

      (*optimized) = 0;
      (*unavailable) = 0;

      /* Cut off at the end of the implicit value.  */
      if (implicit_entry->get_byte_order() == BFD_ENDIAN_BIG)
	{
	  if (!read_bit_limit || read_bit_limit > literal_bit_size)
	    read_bit_limit = bit_size;

	  total_bits_to_skip
	    += literal_bit_size - (offset * HOST_CHAR_BIT
				   + bit_suboffset + read_bit_limit);
	}
      else
	total_bits_to_skip += offset * HOST_CHAR_BIT + bit_suboffset;

      if (total_bits_to_skip >= literal_bit_size)
	{
	  (*unavailable) = 1;
	  return;
	}

      if (bit_size > literal_bit_size - total_bits_to_skip)
	bit_size = literal_bit_size - total_bits_to_skip;

      copy_bitwise (buf, buf_bit_offset, implicit_entry->get_contents (),
		    total_bits_to_skip, bit_size, big_endian);
    }
  else if (auto pointer_entry
	     = dynamic_cast<const dwarf_implicit_pointer *> (location))
    {
      struct frame_info *read_frame = frame;

      if (read_frame == nullptr)
	read_frame = get_selected_frame (_("No frame selected."));

      struct type *type = address_type (get_frame_arch (read_frame),
					pointer_entry->get_addr_size ());

      struct value *value
	= indirect_synthetic_pointer (pointer_entry->get_die_offset (),
				      pointer_entry->get_offset (),
				      pointer_entry->get_per_cu (),
				      pointer_entry->get_per_objfile (),
				      read_frame, type);

      total_bits_to_skip += bit_suboffset;

      gdb_byte *value_contents
	= value_contents_raw (value) + total_bits_to_skip / HOST_CHAR_BIT;

      if ((total_bits_to_skip % HOST_CHAR_BIT) == 0
	  && (bit_size % HOST_CHAR_BIT) == 0
	  && (buf_bit_offset % HOST_CHAR_BIT) == 0)
	{
	  memcpy (buf + buf_bit_offset / HOST_CHAR_BIT,
		  value_contents, bit_size / HOST_CHAR_BIT);
	}
      else
	{
	  copy_bitwise (buf, buf_bit_offset, value_contents,
			total_bits_to_skip % HOST_CHAR_BIT,
			bit_size, big_endian);
	}
    }
  else if (auto composite_entry
	     = dynamic_cast<const dwarf_composite *> (location))
    {
      unsigned int pieces_num = composite_entry->get_pieces_num ();
      unsigned int i;

      total_bits_to_skip += offset * HOST_CHAR_BIT + bit_suboffset;

      /* Skip pieces covered by the read offset.  */
      for (i = 0; i < pieces_num; i++)
	{
	  LONGEST piece_bit_size = composite_entry->get_bit_size_at (i);

	  if (total_bits_to_skip < piece_bit_size)
	    break;

	  total_bits_to_skip -= piece_bit_size;
	}

      for (; i < pieces_num; i++)
	{
	  LONGEST piece_bit_size = composite_entry->get_bit_size_at (i);
	  LONGEST actual_bit_size = piece_bit_size;

	  if (actual_bit_size > bit_size)
	    actual_bit_size = bit_size;

	  read_from_location (composite_entry->get_piece_at (i), frame,
			      total_bits_to_skip, buf, buf_bit_offset,
			      actual_bit_size, piece_bit_size, big_endian,
			      optimized, unavailable);

	  if (bit_size == actual_bit_size || (*optimized) || (*unavailable))
	    break;

	  buf_bit_offset += actual_bit_size;
	  bit_size -= actual_bit_size;
	}
    }
  else
    internal_error (__FILE__, __LINE__, _("invalid location type"));
}

/* Write contents to a location specified by the DWARF location
   description entry LOCATION.

   The write operation is performed in the context of FRAME.  BIT_SIZE
   is the number of bits written.  The data written is copied from the
   caller-managed BUF buffer.  BIG_ENDIAN defines an endianness of the
   target.  BITS_TO_SKIP is a bit offset into the location and
   BUF_BIT_OFFSET is buffer BUF's bit offset.  LOCATION_BIT_LIMIT is a
   a maximum number of bits that location can hold, where value zero
   signifies that there is no such restriction.

   Note that some location types can be written without a FRAME
   context.

   If the location is optimized out or unavailable, the OPTIMIZED and
   UNAVAILABLE outputs are set.  */

static void
write_to_location (const dwarf_location *location, struct frame_info *frame,
		   LONGEST bits_to_skip, const gdb_byte *buf,
		   int buf_bit_offset, size_t bit_size,
		   size_t location_bit_limit, bool big_endian,
		   int* optimized, int* unavailable)
{
  LONGEST offset = location->get_offset();
  LONGEST bit_suboffset = location->get_bit_suboffset();
  LONGEST total_bits_to_skip = bits_to_skip;
  size_t write_bit_limit = location_bit_limit;
  gdb::byte_vector temp_buf;

  /* Writes to undefined locations are always marked as optimized
     out.  */
  if (dynamic_cast<const dwarf_undefined *> (location) != nullptr)
    {
      (*unavailable) = 0;
      (*optimized) = 1;
    }
  else if (auto register_entry
	     = dynamic_cast<const dwarf_register *> (location))
    {
      struct gdbarch *arch = get_frame_arch (frame);
      int gdb_regnum
	= dwarf_reg_to_regnum_or_error (arch, register_entry->get_regnum ());
      ULONGEST reg_bits = HOST_CHAR_BIT * register_size (arch, gdb_regnum);

      if (big_endian)
	{
	  if (!write_bit_limit || reg_bits <= write_bit_limit)
	    write_bit_limit = bit_size;

	  total_bits_to_skip
	    += reg_bits - (offset * HOST_CHAR_BIT
			   + bit_suboffset + write_bit_limit);
	}
      else
	total_bits_to_skip += offset * HOST_CHAR_BIT + bit_suboffset;

      LONGEST this_size = bits_to_bytes (total_bits_to_skip, bit_size);
      temp_buf.resize (this_size);

      if (total_bits_to_skip % HOST_CHAR_BIT != 0
	  || bit_size % HOST_CHAR_BIT != 0)
	{
	  /* Contents is copied non-byte-aligned into the register.
	     Need some bits from original register value.  */
	  read_from_register (frame, gdb_regnum,
			      total_bits_to_skip / HOST_CHAR_BIT,
			      this_size, temp_buf.data (), optimized,
			      unavailable);
	}

      copy_bitwise (temp_buf.data (), total_bits_to_skip % HOST_CHAR_BIT, buf,
		    buf_bit_offset, bit_size, big_endian);

      write_to_register (frame, gdb_regnum, total_bits_to_skip / HOST_CHAR_BIT,
			 this_size, temp_buf.data (), optimized, unavailable);
    }
  else if (auto memory_entry = dynamic_cast<const dwarf_memory *> (location))
    {
      CORE_ADDR start_address
	= offset + (bit_suboffset + total_bits_to_skip) / HOST_CHAR_BIT;

      total_bits_to_skip += bit_suboffset;
      (*optimized) = 0;

      if ((total_bits_to_skip % HOST_CHAR_BIT == 0)
	  && (bit_size % HOST_CHAR_BIT == 0)
	  && (buf_bit_offset % HOST_CHAR_BIT == 0))
	{
	  /* Everything is byte-aligned; no buffer needed.  */
	  write_to_memory (start_address,
			   buf + buf_bit_offset / HOST_CHAR_BIT,
			   bit_size / HOST_CHAR_BIT, memory_entry->in_stack (),
			   unavailable);
	}
      else
	{
	  LONGEST this_size = bits_to_bytes (total_bits_to_skip, bit_size);
	  temp_buf.resize (this_size);

	  if (total_bits_to_skip % HOST_CHAR_BIT != 0
	      || bit_size % HOST_CHAR_BIT != 0)
	    {
	      if (this_size <= HOST_CHAR_BIT)
		/* Perform a single read for small sizes.  */
		read_from_memory (start_address, temp_buf.data (),
				  this_size, memory_entry->in_stack (),
				  unavailable);
	      else
		{
		  /* Only the first and last bytes can possibly have
		     any bits reused.  */
		  read_from_memory (start_address, temp_buf.data (),
				    1, memory_entry->in_stack (), unavailable);

		  if (!(*unavailable))
		    read_from_memory (start_address + this_size - 1,
				      &temp_buf[this_size - 1], 1,
				      memory_entry->in_stack (), unavailable);
		}
	    }

	  copy_bitwise (temp_buf.data (), total_bits_to_skip % HOST_CHAR_BIT,
			buf, buf_bit_offset, bit_size, big_endian);

	  write_to_memory (start_address, temp_buf.data (), this_size,
			   memory_entry->in_stack (), unavailable);
	}
    }
  else if (dynamic_cast<const dwarf_implicit *> (location) != nullptr)
    {
      (*optimized) = 1;
      (*unavailable) = 0;
    }
  else if (dynamic_cast<const dwarf_implicit_pointer *> (location) != nullptr)
    {
      (*optimized) = 1;
      (*unavailable) = 0;
    }
  else if (auto composite_entry
	     = dynamic_cast<const dwarf_composite *> (location))
    {
      unsigned int pieces_num = composite_entry->get_pieces_num ();
      unsigned int i;

      total_bits_to_skip += offset * HOST_CHAR_BIT + bit_suboffset;

      /* Skip pieces covered by the write offset.  */
      for (i = 0; i < pieces_num; i++)
	{
	  LONGEST piece_bit_size = composite_entry->get_bit_size_at (i);

	  if (total_bits_to_skip < piece_bit_size)
	    break;

	  total_bits_to_skip -= piece_bit_size;
	}

      for (; i < pieces_num; i++)
	{
	  LONGEST piece_bit_size = composite_entry->get_bit_size_at (i);
	  LONGEST actual_bit_size = piece_bit_size;

	  if (actual_bit_size > bit_size)
	    actual_bit_size = bit_size;

	  write_to_location (composite_entry->get_piece_at (i), frame,
			     total_bits_to_skip, buf, buf_bit_offset,
			     actual_bit_size, piece_bit_size,
			     big_endian, optimized, unavailable);

	  if (bit_size == actual_bit_size || (*optimized) || (*unavailable))
	    break;

	  buf_bit_offset += actual_bit_size;
	  bit_size -= actual_bit_size;
	}
    }
  else
    internal_error (__FILE__, __LINE__, _("invalid location type"));
}

/* Convert a value entry to the matching struct value representation
   of a given TYPE.  OFFSET defines the offset into the value
   contents.

   We only need to support dwarf_value to gdb struct value conversion
   here so that we can utilize the existing unary and binary operations
   on struct value's.

   We could implement them for the dwarf_value's but that would lead
   to code duplication with no real gain at the moment.  */

static struct value *
value_to_gdb_value (const dwarf_value *value, struct type *type,
		    LONGEST offset = 0)
{
  size_t type_len = TYPE_LENGTH (type);

  if (offset + type_len > TYPE_LENGTH (value->get_type ()))
    invalid_synthetic_pointer ();

  struct value *retval = allocate_value (type);
  memcpy (value_contents_raw (retval),
	  value->get_contents () + offset, type_len);

  return retval;
}

/* Compare two DWARF value's ARG1 and ARG2 for equality in a context
   of a value entry comparison.  */

static bool
dwarf_value_equal_op (dwarf_value *arg1, dwarf_value *arg2)
{
  struct value *arg1_value = value_to_gdb_value (arg1, arg1->get_type ());
  struct value *arg2_value = value_to_gdb_value (arg2, arg2->get_type ());

  return value_equal (arg1_value, arg2_value);
}

/* Compare if DWARF value ARG1 is lesser then DWARF value ARG2 in a
   context of a value entry comparison.   */

static bool
dwarf_value_less_op (dwarf_value *arg1, dwarf_value *arg2)
{
  struct value *arg1_value = value_to_gdb_value (arg1, arg1->get_type ());
  struct value *arg2_value = value_to_gdb_value (arg2, arg2->get_type ());

  return value_less (arg1_value, arg2_value);
}

/* Factory class for creation and lifetime management of all DWARF
   entries found on a DWARF evaluation stack.  */

class dwarf_entry_factory
{
public:
  dwarf_entry_factory () = default;
  ~dwarf_entry_factory ();

  /* Create a value entry of a given TYPE and copy a type size number of
     bytes from the CONTENTS byte stream to the entry.  */
  dwarf_value *create_value (const gdb_byte* contents, struct type *type);

  /* Creates a value entry of a TYPE type and copies the NUM
     value to it's contents byte stream.  */
  dwarf_value *create_value (ULONGEST num, struct type *type);

  /* Create a value entry of TYPE type and copy the NUM value to its
     contents byte stream.  */
  dwarf_value *create_value (LONGEST num, struct type *type);

  /* Create an undefined location description entry.  */
  dwarf_undefined *create_undefined ();

  /* Create a memory location description entry.  */
  dwarf_memory *create_memory (LONGEST offset, LONGEST bit_suboffset = 0,
			       bool stack = false);

  /* Create a register location description entry.  */
  dwarf_register *create_register (unsigned int regnum, LONGEST offset = 0,
				   LONGEST bit_suboffset = 0);

  /* Create an implicit location description entry and copy SIZE
     number of bytes from the CONTENTS byte stream to the location.
     BYTE_ORDER holds the byte order of the location described.  */
  dwarf_implicit *create_implicit (const gdb_byte *content, size_t size,
				   enum bfd_endian byte_order);

  /* Create an implicit pointer location description entry.  */
  dwarf_implicit_pointer *create_implicit_pointer
    (dwarf2_per_objfile *per_objfile, struct dwarf2_per_cu_data *per_cu,
     int addr_size, sect_offset die_offset, LONGEST offset,
     LONGEST bit_suboffset = 0);

  /* Create a composite location description entry.  */
  dwarf_composite *create_composite (LONGEST offset = 0,
				     LONGEST bit_suboffset = 0);

  /* Convert an entry to a location description entry. If the entry
     is a location description entry a dynamic cast is applied.

     In a case of a value entry, the value is implicitly
     converted to a memory location description entry.  */
  dwarf_location *entry_to_location (dwarf_entry *entry);

  /* Convert an entry to a value entry.  If the entry is a value entry
     a dynamic cast is applied.

     A location description entry is implicitly converted to a value
     entry of DEFAULT_TYPE type.
     Note that only 'memory location description entry' to 'value
     entry' conversion is currently supported. */
  dwarf_value *entry_to_value (dwarf_entry *entry, struct type *default_type);

  /* Execute OP operation between ARG1 and ARG2 and return a new value
     entry containing the result of that operation.  */
  dwarf_value *value_binary_op (const dwarf_value *arg1,
				const dwarf_value *arg2, enum exp_opcode op);

  /* Execute a negation operation on ARG and return a new value entry
     containing the result of that operation.  */
  dwarf_value *value_negation_op (const dwarf_value *arg);

  /* Execute a complement operation on ARG and return a new value
     entry containing the result of that operation.  */
  dwarf_value *value_complement_op (const dwarf_value *arg);

  /* Execute a cast operation on ARG and return a new value entry
     containing the result of that operation.  */
  dwarf_value *value_cast_op (const dwarf_value *arg, struct type *type);

private:
  /* Record entry for garbage collection.  */
  void record_entry (dwarf_entry *entry);

  /* List of all entries created by the factory.  */
  std::vector<dwarf_entry *> m_dwarf_entries;
};

dwarf_entry_factory::~dwarf_entry_factory ()
{
  for (unsigned int i = 0; i < m_dwarf_entries.size (); i++)
    {
      dwarf_entry* entry = m_dwarf_entries[i];

      entry->decref ();

      if (entry->refcount () == 0)
	delete entry;
    }
}

void
dwarf_entry_factory::record_entry (dwarf_entry *entry)
{
  entry->incref ();
  m_dwarf_entries.push_back (entry);
}

dwarf_value *
dwarf_entry_factory::create_value (const gdb_byte* content, struct type *type)
{
  dwarf_value *value = new dwarf_value (content, type);
  record_entry (value);
  return value;
}

dwarf_value *
dwarf_entry_factory::create_value (ULONGEST num, struct type *type)
{
  dwarf_value *value = new dwarf_value (num, type);
  record_entry (value);
  return value;
}

dwarf_value *
dwarf_entry_factory::create_value (LONGEST num, struct type *type)
{
  dwarf_value *value = new dwarf_value (num, type);
  record_entry (value);
  return value;
}

dwarf_undefined *
dwarf_entry_factory::create_undefined ()
{
  dwarf_undefined *undefined_entry = new dwarf_undefined ();
  record_entry (undefined_entry);
  return undefined_entry;
}

dwarf_memory *
dwarf_entry_factory::create_memory (LONGEST offset, LONGEST bit_suboffset,
				    bool stack)
{
  dwarf_memory *memory_entry
    = new dwarf_memory (offset, bit_suboffset, stack);
  record_entry (memory_entry);
  return memory_entry;
}

dwarf_register *
dwarf_entry_factory::create_register (unsigned int regnum, LONGEST offset,
				      LONGEST bit_suboffset)
{
  dwarf_register *register_entry
    = new dwarf_register (regnum, offset, bit_suboffset);
  record_entry (register_entry);
  return register_entry;
}

dwarf_implicit *
dwarf_entry_factory::create_implicit (const gdb_byte* content, size_t size,
				      enum bfd_endian byte_order)
{
  dwarf_implicit *implicit_entry
    = new dwarf_implicit (content, size, byte_order);
  record_entry (implicit_entry);
  return implicit_entry;
}

dwarf_implicit_pointer *
dwarf_entry_factory::create_implicit_pointer
  (dwarf2_per_objfile *per_objfile, struct dwarf2_per_cu_data *per_cu,
   int addr_size, sect_offset die_offset, LONGEST offset,
   LONGEST bit_suboffset)
{
  dwarf_implicit_pointer *implicit_pointer_entry
    = new dwarf_implicit_pointer (per_objfile, per_cu, addr_size,
				  die_offset, offset, bit_suboffset);
  record_entry (implicit_pointer_entry);
  return implicit_pointer_entry;
}

dwarf_composite *
dwarf_entry_factory::create_composite (LONGEST offset, LONGEST bit_suboffset)
{
  dwarf_composite *composite_entry
    = new dwarf_composite (offset, bit_suboffset);
  record_entry (composite_entry);
  return composite_entry;
}

dwarf_location *
dwarf_entry_factory::entry_to_location (dwarf_entry *entry)
{
  /* If the given entry is already a location,
     just send it back to the caller.  */
  if (auto location = dynamic_cast<dwarf_location *> (entry))
    return location;

  auto value = dynamic_cast<dwarf_value *> (entry);
  gdb_assert (value != nullptr);

  struct type *type = value->get_type ();
  struct gdbarch *gdbarch = get_type_arch (type);
  LONGEST offset;

  if (gdbarch_integer_to_address_p (gdbarch))
    offset = gdbarch_integer_to_address (gdbarch, type,
					 value->get_contents ());

  offset = extract_unsigned_integer (value->get_contents (),
				     TYPE_LENGTH (type),
				     type_byte_order (type));

  return create_memory (offset);
}

dwarf_value *
dwarf_entry_factory::entry_to_value (dwarf_entry *entry,
				     struct type *default_type)
{
  /* If the given entry is already a value,
     just send it back to the caller.  */
  if (auto value = dynamic_cast<dwarf_value *> (entry))
    return value;

  auto location = dynamic_cast<dwarf_location *> (entry);
  gdb_assert (location != nullptr);

  /* We only support memory location to value conversion at this point.
     It is hard to define how would that conversion work for other
     location description types.  */
  if (dynamic_cast<dwarf_memory *> (location) == nullptr)
    ill_formed_expression ();

  return create_value (location->get_offset (), default_type);
}

/* We use the existing struct value operations to avoid code
   duplication.  Vector types are planned to be promoted to base types
   in the future anyway which means that the subset we actually need
   from these operations is just going to grow anyway.  */

dwarf_value *
dwarf_entry_factory::value_binary_op (const dwarf_value *arg1,
				      const dwarf_value *arg2,
				      enum exp_opcode op)
{
  struct value *arg1_value = value_to_gdb_value (arg1, arg1->get_type ());
  struct value *arg2_value = value_to_gdb_value (arg2, arg2->get_type ());
  struct value *result = value_binop (arg1_value, arg2_value, op);

  return create_value (value_contents_raw (result), value_type (result));
}

dwarf_value *
dwarf_entry_factory::value_negation_op (const dwarf_value *arg)
{
  struct value *result
    = value_neg (value_to_gdb_value (arg, arg->get_type ()));
  return create_value (value_contents_raw (result), value_type (result));
}

dwarf_value *
dwarf_entry_factory::value_complement_op (const dwarf_value *arg)
{
  struct value *result
    = value_complement (value_to_gdb_value (arg, arg->get_type ()));
  return create_value (value_contents_raw (result), value_type (result));
}

dwarf_value *
dwarf_entry_factory::value_cast_op (const dwarf_value *arg, struct type *type)
{
  struct value *result
    = value_cast (type, value_to_gdb_value (arg, arg->get_type ()));
  return create_value (value_contents_raw (result), type);
}

/* Closure class that encapsulates a location description and
   a context in which that description is created.  Used for
   lval_computed value abstraction.  */
class computed_closure : public refcounted_object
{
public:
  computed_closure (dwarf_entry *entry, struct frame_id frame_id) :
		    m_entry (entry), m_frame_id (frame_id)
		    {entry->incref ();}

  virtual ~computed_closure ()
  {
    m_entry->decref ();

    if (m_entry->refcount () == 0)
      delete m_entry;
  }

  dwarf_entry *get_entry () const
  {
    return m_entry;
  }

  struct frame_id get_frame_id () const
  {
    return m_frame_id;
  }

private:
  /* Entry that this class encloses.  */
  dwarf_entry *m_entry;

  /* Frame ID context of the closure.  */
  struct frame_id m_frame_id;
};

/* Read or write a closure value V.  If FROM != NULL, operate in "write
   mode": copy FROM into the closure comprising V.  If FROM == NULL,
   operate in "read mode": fetch the contents of the (lazy) value V by
   composing it from its closure.  */

static void
rw_closure_value (struct value *v, struct value *from)
{
  LONGEST bit_offset = 0, max_bit_offset;
  computed_closure *closure = ((computed_closure*) value_computed_closure (v));
  bool big_endian = type_byte_order (value_type (v)) == BFD_ENDIAN_BIG;
  dwarf_entry *entry = closure->get_entry ();

  /* Only expect implicit pointer and composite location
     description here.  */
  if (entry == nullptr
      || (dynamic_cast<dwarf_implicit_pointer *> (entry) == nullptr
	  && dynamic_cast<dwarf_composite *> (entry) == nullptr))
    internal_error (__FILE__, __LINE__, _("invalid location type"));

  if (from == NULL)
    {
      if (value_type (v) != value_enclosing_type (v))
	  internal_error (__FILE__, __LINE__,
			  _("Should not be able to create a lazy value with "
			    "an enclosing type"));
    }

  ULONGEST bits_to_skip = HOST_CHAR_BIT * value_offset (v);

  /* If there are bits that don't complete a byte, count them in.  */
  if (value_bitsize (v))
    {
      bits_to_skip += (HOST_CHAR_BIT * value_offset (value_parent (v))
		       + value_bitpos (v));
      if (from != NULL && big_endian)
	{
	  /* Use the least significant bits of FROM.  */
	  max_bit_offset = HOST_CHAR_BIT * TYPE_LENGTH (value_type (from));
	  bit_offset = max_bit_offset - value_bitsize (v);
	}
      else
	max_bit_offset = value_bitsize (v);
    }
  else
    max_bit_offset = HOST_CHAR_BIT * TYPE_LENGTH (value_type (v));

  /* Reading from and writing to an implicit pointer is not meaningful,
     so we just skip them here.  */
  if (dynamic_cast<dwarf_implicit_pointer *> (entry) != nullptr)
    {
      if (from != NULL)
	mark_value_bits_optimized_out (v, bits_to_skip,
				       HOST_CHAR_BIT
				       * TYPE_LENGTH (value_type (v)));
      return;
    }

  struct frame_info *frame = frame_find_by_id (closure->get_frame_id ());

  dwarf_composite *composite_entry = dynamic_cast<dwarf_composite *> (entry);

  /* Advance to the first non-skipped piece.  */
  unsigned int i;
  unsigned int pieces_num = composite_entry->get_pieces_num ();

  for (i = 0; i < pieces_num; i++)
    {
      ULONGEST bit_size = composite_entry->get_bit_size_at (i);

      if (bits_to_skip < bit_size)
	break;

      bits_to_skip -= bit_size;
    }

  for (; i < pieces_num && bit_offset < max_bit_offset; i++)
    {
      const dwarf_location *location = composite_entry->get_piece_at (i);
      ULONGEST bit_size = composite_entry->get_bit_size_at (i);
      size_t this_bit_size = bit_size - bits_to_skip;
      int optimized, unavailable;

      if (this_bit_size > max_bit_offset - bit_offset)
	this_bit_size = max_bit_offset - bit_offset;

      if (from == NULL)
	{
	  /* Implicit pointers are handled later.  */
	  if (dynamic_cast<const dwarf_implicit_pointer *>
		(location) == nullptr)
	    {
	      read_from_location (location, frame, bits_to_skip,
				  value_contents_raw (v), bit_offset,
				  this_bit_size, bit_size, big_endian,
				  &optimized, &unavailable);

	      if (optimized)
		mark_value_bits_optimized_out (v, bit_offset, this_bit_size);
	      if (unavailable)
		mark_value_bits_unavailable (v, bit_offset, this_bit_size);
	    }
	}
      else
	{
	  write_to_location (location, frame, bits_to_skip,
			     value_contents (from), bit_offset,
			     this_bit_size, bit_size, big_endian,
			     &optimized, &unavailable);

	  if (optimized)
	    throw_error (OPTIMIZED_OUT_ERROR,
			 _("Can't do read-modify-write to "
			   "update bitfield; containing word "
			   "has been optimized out"));
	  if (unavailable)
	    throw_error (NOT_AVAILABLE_ERROR,
			 _("Can't do read-modify-write to "
			   "update bitfield; containing word "
			   "is unavailable"));
    }

      bit_offset += this_bit_size;
      bits_to_skip = 0;
    }
}

static void
read_closure_value (struct value *v)
{
  rw_closure_value (v, NULL);
}

static void
write_closure_value (struct value *to, struct value *from)
{
  rw_closure_value (to, from);
}

/* Check if a given location contains an implicit pointer
   location description of a BIT_LENGTH size on a given
   BIT_OFFSET offset.  */

static int
check_synthetic_pointer_location (const dwarf_location *location,
				  LONGEST bit_offset, int bit_length)
{
  if (dynamic_cast<const dwarf_implicit_pointer *> (location) != nullptr)
    return 1;

  if (auto composite_entry
	= dynamic_cast<const dwarf_composite *> (location))
  {
    /* Advance to the first non-skipped piece.  */
    unsigned int pieces_num = composite_entry->get_pieces_num ();
    LONGEST total_bit_offset = bit_offset;
    LONGEST total_bit_length = bit_length;

    total_bit_offset += HOST_CHAR_BIT * composite_entry->get_offset ()
			+ composite_entry->get_bit_suboffset ();

    for (unsigned int i = 0; i < pieces_num && total_bit_length != 0; i++)
      {
	ULONGEST read_bit_length = composite_entry->get_bit_size_at (i);

	if (total_bit_offset >= read_bit_length)
	  {
	     total_bit_offset -= read_bit_length;
	     continue;
	  }

	read_bit_length -= total_bit_offset;

	if (total_bit_length < read_bit_length)
	  read_bit_length = total_bit_length;

	const dwarf_location *piece = composite_entry->get_piece_at (i);

	if (!check_synthetic_pointer_location (piece, total_bit_offset,
					       read_bit_length))
	  return 0;

	total_bit_offset = 0;
	total_bit_length -= read_bit_length;
      }

    return 1;
  }

  return 0;
}

/* An implementation of an lval_funcs method to see whether a value is
   a synthetic pointer.  */

static int
check_synthetic_pointer (const struct value *value, LONGEST bit_offset,
			 int bit_length)
{
  LONGEST total_bit_offset = bit_offset + HOST_CHAR_BIT * value_offset (value);

  if (value_bitsize (value))
    total_bit_offset += value_bitpos (value);

  computed_closure *closure
    = (computed_closure *) value_computed_closure (value);
  auto location
    = dynamic_cast<dwarf_location *> (closure->get_entry ());

  if (location == nullptr)
    return 0;

  return check_synthetic_pointer_location (location, total_bit_offset,
					   bit_length);
}

/* Recursive indirect read from a given LOCATION location description
   in a given FRAME context, using the TYPE as the type of the pointer.
   Where INDIRECT_OFFSET is an offset applied to the implicit pointer
   location description before the indirect read. BIT_OFFSET is an bit
   offset applied to the LOCATION before the indirection and BIT_LENGTH
   is a bit length of the LOCATION to be used for the read.

   Read is only performed on the implicit pointer location
   description parts of the LOCATION.  */

static struct value *
indirect_from_location (const dwarf_location *location,
			struct frame_info *frame,
			struct type *type, LONGEST indirect_offset,
			LONGEST bit_offset, int bit_length)
{
  if (auto pointer_entry
	= dynamic_cast<const dwarf_implicit_pointer *> (location))
    {
      gdb_assert (pointer_entry->get_per_cu () != nullptr);

      LONGEST total_indirect_offset
	= indirect_offset + pointer_entry->get_offset ();

      return indirect_synthetic_pointer (pointer_entry->get_die_offset (),
					 total_indirect_offset,
					 pointer_entry->get_per_cu (),
					 pointer_entry->get_per_objfile (),
					 frame, type);
    }
  else if (auto composite_entry
	     = dynamic_cast<const dwarf_composite *> (location))
    {
      /* Advance to the first non-skipped piece.  */
      unsigned int pieces_num = composite_entry->get_pieces_num ();
      LONGEST total_bit_offset = bit_offset;

      total_bit_offset += HOST_CHAR_BIT * composite_entry->get_offset ()
			  + composite_entry->get_bit_suboffset ();

      for (unsigned int i = 0; i < pieces_num; i++)
	{
	  ULONGEST read_bit_length = composite_entry->get_bit_size_at (i);

	  if (total_bit_offset >= read_bit_length)
	    {
	      total_bit_offset -= read_bit_length;
	      continue;
	    }

	  read_bit_length -= total_bit_offset;

	  if (bit_length < read_bit_length)
	    read_bit_length = bit_length;

	  const dwarf_location *piece = composite_entry->get_piece_at (i);

	  return indirect_from_location (piece, frame, type, indirect_offset,
					 total_bit_offset, read_bit_length);
	}
    }

  return NULL;
}

/* An implementation of an lval_funcs method to indirect through a
   pointer.  This handles the synthetic pointer case when needed.  */

static struct value *
indirect_closure_value (struct value *value)
{
  computed_closure *closure
    = (computed_closure *) value_computed_closure (value);

  struct type *type = check_typedef (value_type (value));
  if (type->code () != TYPE_CODE_PTR)
    return NULL;

  LONGEST bit_length = HOST_CHAR_BIT * TYPE_LENGTH (type);
  LONGEST bit_offset = HOST_CHAR_BIT * value_offset (value);

  if (value_bitsize (value))
    bit_offset += value_bitpos (value);

  auto location
    = dynamic_cast<dwarf_location *> (closure->get_entry ());

  /* Only location descriptions are meaningful here.  */
  if (location == nullptr)
    return NULL;

  struct frame_info *frame = get_selected_frame (_("No frame selected."));

  /* This is an offset requested by GDB, such as value subscripts.
     However, due to how synthetic pointers are implemented, this is
     always presented to us as a pointer type.  This means we have to
     sign-extend it manually as appropriate.  Use raw
     extract_signed_integer directly rather than value_as_address and
     sign extend afterwards on architectures that would need it
     (mostly everywhere except MIPS, which has signed addresses) as
     the later would go through gdbarch_pointer_to_address and thus
     return a CORE_ADDR with high bits set on architectures that
     encode address spaces and other things in CORE_ADDR.  */
  enum bfd_endian byte_order = gdbarch_byte_order (get_frame_arch (frame));
  LONGEST indirect_offset
    = extract_signed_integer (value_contents (value),
			      TYPE_LENGTH (type), byte_order);

  return indirect_from_location (location, frame, type, indirect_offset,
				 bit_offset, bit_length);
}

/* Implementation of the coerce_ref method of lval_funcs for synthetic C++
   references.  */

static struct value *
coerce_closure_ref (const struct value *value)
{
  struct type *type = check_typedef (value_type (value));

  if (value_bits_synthetic_pointer (value, value_embedded_offset (value),
				    TARGET_CHAR_BIT * TYPE_LENGTH (type)))
    {
      computed_closure *closure
	= (computed_closure *) value_computed_closure (value);
      struct frame_info *frame
	= get_selected_frame (_("No frame selected."));

      auto pointer_entry
	= dynamic_cast<dwarf_implicit_pointer *> (closure->get_entry ());

      /* Only implicit pointer location description is meaningful here.  */
      if (pointer_entry == nullptr)
	return NULL;

      return indirect_synthetic_pointer (pointer_entry->get_die_offset (),
					 pointer_entry->get_offset (),
					 pointer_entry->get_per_cu (),
					 pointer_entry->get_per_objfile (),
					 frame, type);
    }
  else
    {
      /* Else: not a synthetic reference; do nothing.  */
      return NULL;
    }
}

static void *
copy_value_closure (const struct value *v)
{
  computed_closure *closure = ((computed_closure*) value_computed_closure (v));

  if (closure == nullptr)
    internal_error (__FILE__, __LINE__, _("invalid closure type"));

  closure->incref ();
  return closure;
}

static void
free_value_closure (struct value *v)
{
  computed_closure *closure = ((computed_closure*) value_computed_closure (v));

  if (closure == nullptr)
    internal_error (__FILE__, __LINE__, _("invalid closure type"));

  closure->decref ();

  if (closure->refcount () == 0)
    delete closure;
}

/* Functions for accessing a variable described by DW_OP_piece,
   DW_OP_bit_piece or DW_OP_implicit_pointer.  */
static const struct lval_funcs closure_value_funcs = {
  read_closure_value,
  write_closure_value,
  indirect_closure_value,
  coerce_closure_ref,
  check_synthetic_pointer,
  copy_value_closure,
  free_value_closure
};

/* Given context CTX, section offset SECT_OFF, and compilation unit
   data PER_CU, execute the "variable value" operation on the DIE
   found at SECT_OFF.  */

static struct value *
sect_variable_value (sect_offset sect_off,
		     dwarf2_per_cu_data *per_cu,
		     dwarf2_per_objfile *per_objfile)
{
  struct type *die_type
    = dwarf2_fetch_die_type_sect_off (sect_off, per_cu, per_objfile);

  if (die_type == NULL)
    error (_("Bad DW_OP_GNU_variable_value DIE."));

  /* Note: Things still work when the following test is removed.  This
     test and error is here to conform to the proposed specification.  */
  if (die_type->code () != TYPE_CODE_INT
      && die_type->code () != TYPE_CODE_PTR)
    error (_("Type of DW_OP_GNU_variable_value DIE must be an integer or pointer."));

  struct type *type = lookup_pointer_type (die_type);
  struct frame_info *frame = get_selected_frame (_("No frame selected."));
  return indirect_synthetic_pointer (sect_off, 0, per_cu, per_objfile, frame,
				     type, true);
}

/* The expression evaluator works with a dwarf_expr_context, describing
   its current state and its callbacks.  */
struct dwarf_expr_context
{
  /* Create a new context for the expression evaluator.

     We should ever only pass in the PER_OBJFILE and the ADDR_SIZE
     information should be retrievable from there.  The PER_OBJFILE
     contains a pointer to the PER_BFD information anyway and the
     address size information must be the same for the whole BFD.   */
  dwarf_expr_context (struct dwarf2_per_objfile *per_objfile,
		      int addr_size);

  /* Destroy dwarf entry factory object.  */
  virtual ~dwarf_expr_context ();

  /* Evaluate the expression at ADDR (LEN bytes long) in a given PER_CU
     FRAME context.  INIT_VALUES vector contains values that are
     expected to be pushed on a DWARF expression stack before the
     evaluation.  AS_LVAL defines if the returned struct value is
     expected to be a value or a location description.  Where TYPE,
     SUBOBJ_TYPE and SUBOBJ_OFFSET describe expected struct value
     representation of the evaluation result.  The ADDR_INFO property
     can be specified to override the range of memory addresses with
     the passed in buffer.  */
  struct value *eval_exp (const gdb_byte *addr, size_t len, bool as_lval,
			  struct dwarf2_per_cu_data *per_cu,
			  struct frame_info *frame,
			  std::vector<struct value *> *init_values,
			  const struct property_addr_info *addr_info,
			  struct type *type, struct type *subobj_type,
			  LONGEST subobj_offset);

private:
  /* The stack of values.  */
  std::vector<dwarf_entry *> stack;

  /* Target architecture to use for address operations.  */
  struct gdbarch *gdbarch;

  /* Target address size in bytes.  */
  int addr_size;

  /* DW_FORM_ref_addr size in bytes.  If -1 DWARF is executed
     from a frame context and operations depending on DW_FORM_ref_addr
     are not allowed.  */
  int ref_addr_size;

  /* The current depth of dwarf expression recursion, via DW_OP_call*,
     DW_OP_fbreg, DW_OP_push_object_address, etc., and the maximum
     depth we'll tolerate before raising an error.  */
  int recursion_depth, max_recursion_depth;

  /* We evaluate the expression in the context of this objfile.  */
  dwarf2_per_objfile *per_objfile;

  /* Frame information used for the evaluation.  */
  struct frame_info *frame;

  /* Compilation unit used for the evaluation.  */
  struct dwarf2_per_cu_data *per_cu;

  /* Property address info used for the evaluation.  */
  const struct property_addr_info *addr_info;

  /* Factory in charge of the dwarf entry's life cycle.  */
  dwarf_entry_factory *entry_factory;

  /* Evaluate the expression at ADDR (LEN bytes long).  */
  void eval (const gdb_byte *addr, size_t len);

  /* Return the type used for DWARF operations where the type is
     unspecified in the DWARF spec.  Only certain sizes are
     supported.  */
  struct type *address_type () const;

  /* Push ENTRY onto the stack.  */
  void push (dwarf_entry *value);

  /* Return true if the expression stack is empty.  */
  bool stack_empty_p () const;

  /* Pop a top element of the stack and add as a composite piece.

     If the fallowing top element of the stack is a composite
     location description, the piece will be added to it.  Otherwise
     a new composite location description will be created and
     the piece will be added to that composite.  */
  dwarf_entry *add_piece (ULONGEST bit_size, ULONGEST bit_offset);

  /* The engine for the expression evaluator.  Using the context in this
     object, evaluate the expression between OP_PTR and OP_END.  */
  void execute_stack_op (const gdb_byte *op_ptr, const gdb_byte *op_end);

  /* Pop the top item off of the stack.  */
  void pop ();

  /* Retrieve the N'th item on the stack.  */
  dwarf_entry *fetch (int n);

  /* Fetch the result of the expression evaluation in a form of
     a struct value, where TYPE, SUBOBJ_TYPE and SUBOBJ_OFFSET
     describe the source level representation of that result.
     AS_LVAL defines if the fetched struct value is expected to
     be a value or a location description.  */
  struct value *fetch_result (struct type *type,
			      struct type *subobj_type,
			      LONGEST subobj_offset,
			      bool as_lval);

  /* Return the location expression for the frame base attribute, in
     START and LENGTH.  The result must be live until the current
     expression evaluation is complete.  */
  void get_frame_base (const gdb_byte **start, size_t *length);

  /* Return the base type given by the indicated DIE at DIE_CU_OFF.
     This can throw an exception if the DIE is invalid or does not
     represent a base type.  SIZE is non-zero if this function should
     verify that the resulting type has the correct size.  */
  struct type *get_base_type (cu_offset die_cu_off, int size);

  /* Execute DW_AT_location expression for the DWARF expression
     subroutine in the DIE at DIE_CU_OFF in the CU.  Do not touch
     STACK while it being passed to and returned from the called DWARF
     subroutine.  */
  void dwarf_call (cu_offset die_cu_off);

  /* Push on DWARF stack an entry evaluated for DW_TAG_call_site's
     parameter matching KIND and KIND_U at the caller of specified
     BATON. If DEREF_SIZE is not -1 then use DW_AT_call_data_value
     instead of DW_AT_call_value.  */
  void push_dwarf_reg_entry_value (enum call_site_parameter_kind kind,
				   union call_site_parameter_u kind_u,
				   int deref_size);

  /* Apply dereference operation on the DWARF ENTRY.  In the case of a
     value entry, the entry will be implicitly converted to the
     appropriate location description before the operation is applied.
     If the SIZE is specified, it must be equal or smaller then the
     TYPE type size.  If SIZE is smaller then the type size, the value
     will be zero extended to the difference.  */
  dwarf_entry* dwarf_entry_deref (dwarf_entry *entry, struct type *type,
				  size_t size = 0);

  /* Convert struct value to the matching DWARF entry representation.
     Used for non-standard DW_OP_GNU_variable_value operation
     support.  */
  dwarf_entry *gdb_value_to_dwarf_entry (struct value *value);

  /* Convert DWARF entry to the matching struct value representation
     of the given TYPE type. SUBOBJ_TYPE information if specified, will
     be used for more precise description of the source variable type
     information.  Where SUBOBJ_OFFSET defines an offset into the DWARF
     entry contents.  */
  struct value *dwarf_entry_to_gdb_value (dwarf_entry *entry,
					  struct type *type,
					  struct type *subobj_type = nullptr,
					  LONGEST subobj_offset = 0);
};

struct type *
address_type (struct gdbarch *gdbarch, int addr_size)
{
  struct dwarf_gdbarch_types *types
    = (struct dwarf_gdbarch_types *) gdbarch_data (gdbarch,
						   dwarf_arch_cookie);
  int ndx;

  if (addr_size == 2)
    ndx = 0;
  else if (addr_size == 4)
    ndx = 1;
  else if (addr_size == 8)
    ndx = 2;
  else
    error (_("Unsupported address size in DWARF expressions: %d bits"),
	   8 * addr_size);

  if (types->dw_types[ndx] == NULL)
    types->dw_types[ndx]
      = arch_integer_type (gdbarch, HOST_CHAR_BIT * addr_size,
			   0, "<signed DWARF address type>");

  return types->dw_types[ndx];
}

/* Return the type used for DWARF operations where the type is
   unspecified in the DWARF spec.  Only certain sizes are
   supported.  */

struct type *
dwarf_expr_context::address_type () const
{
  return ::address_type (this->gdbarch, this->addr_size);
}

/* Create a new context for the expression evaluator.  */

dwarf_expr_context::dwarf_expr_context (dwarf2_per_objfile *per_objfile,
					int addr_size)
: gdbarch (per_objfile->objfile->arch ()),
  addr_size (addr_size),
  ref_addr_size (0),
  recursion_depth (0),
  max_recursion_depth (0x100),
  per_objfile (per_objfile),
  frame (nullptr),
  per_cu (nullptr),
  addr_info (nullptr)
{
  entry_factory = new dwarf_entry_factory ();
}

dwarf_expr_context::~dwarf_expr_context ()
{
  delete entry_factory;
}

void
dwarf_expr_context::push (dwarf_entry *entry)
{
  stack.emplace_back (entry);
}

void
dwarf_expr_context::pop ()
{
  if (stack.empty ())
    error (_("dwarf expression stack underflow"));

  stack.pop_back ();
}

dwarf_entry *
dwarf_expr_context::fetch (int n)
{
  if (stack.size () <= n)
     error (_("Asked for position %d of stack, "
	      "stack only has %zu elements on it."),
	    n, stack.size ());
  return stack[stack.size () - (1 + n)];
}

void
dwarf_expr_context::get_frame_base (const gdb_byte **start,
				    size_t * length)
{
  ensure_have_frame (frame, "DW_OP_fbreg");

  const struct block *bl = get_frame_block (frame, NULL);

  if (bl == NULL)
    error (_("frame address is not available."));

  /* Use block_linkage_function, which returns a real (not inlined)
     function, instead of get_frame_function, which may return an
     inlined function.  */
  struct symbol *framefunc = block_linkage_function (bl);

  /* If we found a frame-relative symbol then it was certainly within
     some function associated with a frame. If we can't find the frame,
     something has gone wrong.  */
  gdb_assert (framefunc != NULL);

  func_get_frame_base_dwarf_block (framefunc,
				   get_frame_address_in_block (frame),
				   start, length);
}

struct type *
dwarf_expr_context::get_base_type (cu_offset die_cu_off, int size)
{
  if (per_cu == nullptr)
    return builtin_type (this->gdbarch)->builtin_int;

  struct type *result = dwarf2_get_die_type (die_cu_off, per_cu, per_objfile);

  if (result == NULL)
    error (_("Could not find type for DW_OP_const_type"));

  if (size != 0 && TYPE_LENGTH (result) != size)
    error (_("DW_OP_const_type has different sizes for type and data"));

  return result;
}

void
dwarf_expr_context::dwarf_call (cu_offset die_cu_off)
{
  ensure_have_per_cu (per_cu, "DW_OP_call");

  struct frame_info *frame = this->frame;

  auto get_pc_from_frame = [frame] ()
    {
      ensure_have_frame (frame, "DW_OP_call");
      return get_frame_address_in_block (frame);
    };

  struct dwarf2_locexpr_baton block
    = dwarf2_fetch_die_loc_cu_off (die_cu_off, per_cu, per_objfile,
				   get_pc_from_frame);

  /* DW_OP_call_ref is currently not supported.  */
  gdb_assert (block.per_cu == per_cu);

  this->eval (block.data, block.size);
}

void
dwarf_expr_context::push_dwarf_reg_entry_value
		      (enum call_site_parameter_kind kind,
		       union call_site_parameter_u kind_u,
		       int deref_size)
{
  ensure_have_per_cu (per_cu, "DW_OP_entry_value");
  ensure_have_frame (frame, "DW_OP_entry_value");

  dwarf2_per_cu_data *caller_per_cu;
  dwarf2_per_objfile *caller_per_objfile;
  struct frame_info *caller_frame = get_prev_frame (frame);
  struct call_site_parameter *parameter
    = dwarf_expr_reg_to_entry_parameter (frame, kind, kind_u,
					 &caller_per_cu,
					 &caller_per_objfile);
  const gdb_byte *data_src
    = deref_size == -1 ? parameter->value : parameter->data_value;
  size_t size
    = deref_size == -1 ? parameter->value_size : parameter->data_value_size;

  /* DEREF_SIZE size is not verified here.  */
  if (data_src == NULL)
    throw_error (NO_ENTRY_VALUE_ERROR,
		 _("Cannot resolve DW_AT_call_data_value"));

  /* We are about to evaluate an expression in the context of the caller
     of the current frame.  This evaluation context may be different from
     the current (callee's) context), so temporarily set the caller's context.

     It is possible for the caller to be from a different objfile from the
     callee if the call is made through a function pointer.  */
  scoped_restore save_frame = make_scoped_restore (&this->frame,
						   caller_frame);
  scoped_restore save_per_cu = make_scoped_restore (&this->per_cu,
						    caller_per_cu);
  scoped_restore save_addr_info = make_scoped_restore (&this->addr_info,
						       nullptr);
  scoped_restore save_per_objfile = make_scoped_restore (&this->per_objfile,
							 caller_per_objfile);

  scoped_restore save_arch = make_scoped_restore (&this->gdbarch);
  this->gdbarch = this->per_objfile->objfile->arch ();
  scoped_restore save_addr_size = make_scoped_restore (&this->addr_size);
  this->addr_size = this->per_cu->addr_size ();

  this->eval (data_src, size);
}

struct value *
dwarf_expr_context::fetch_result (struct type *type,
				  struct type *subobj_type,
				  LONGEST subobj_offset,
				  bool as_lval)
{
  if (type == nullptr)
    type = address_type ();

  if (subobj_type == nullptr)
    subobj_type = type;

  dwarf_entry *entry = fetch (0);

  if (!as_lval)
    entry = entry_factory->entry_to_value (entry, address_type ());
   else
    entry = entry_factory->entry_to_location (entry);

  return dwarf_entry_to_gdb_value (entry, type, subobj_type, subobj_offset);
}

struct value *
dwarf_expr_context::eval_exp (const gdb_byte *addr, size_t len, bool as_lval,
			      struct dwarf2_per_cu_data *per_cu,
			      struct frame_info *frame,
			      std::vector<struct value *> *init_values,
			      const struct property_addr_info *addr_info,
			      struct type *type,
			      struct type *subobj_type,
			      LONGEST subobj_offset)
{
  this->per_cu = per_cu;
  this->frame = frame;
  this->addr_info = addr_info;

  if (per_cu != nullptr)
    this->ref_addr_size = per_cu->ref_addr_size ();

  if (init_values != nullptr)
    for (unsigned int i = 0; i < init_values->size (); i++)
      push (gdb_value_to_dwarf_entry ((*init_values)[i]));

  eval (addr, len);
  return fetch_result (type, subobj_type, subobj_offset, as_lval);
}

dwarf_entry *
dwarf_expr_context::dwarf_entry_deref (dwarf_entry *entry,
				       struct type *type, size_t size)
{
  bool big_endian = type_byte_order (type) == BFD_ENDIAN_BIG;
  dwarf_location *location = entry_factory->entry_to_location (entry);
  size_t actual_size = size != 0 ? size : TYPE_LENGTH (type);

  if (actual_size > TYPE_LENGTH (type))
    ill_formed_expression ();

  /* If the size of the object read from memory is different
     from the type length, we need to zero-extend it.  */
  gdb::byte_vector read_buf (TYPE_LENGTH (type), 0);
  size_t size_in_bits = actual_size * HOST_CHAR_BIT;
  gdb_byte *buf_ptr = read_buf.data();
  bool passed_in_buf = false;

  if (big_endian)
    buf_ptr += TYPE_LENGTH (type) - actual_size;

  if (auto memory_entry = dynamic_cast<dwarf_memory *> (location))
    {
      /* Covers the case where we have a passed in memory that is not
	 part of the target and requires for the location description
	 to address it instead of addressing the actual target
	 memory.  */
      CORE_ADDR start_address = memory_entry->get_offset ();
      LONGEST bit_suboffset = location->get_bit_suboffset ();
      LONGEST this_size = bits_to_bytes (bit_suboffset, size_in_bits);

      /* We shouldn't have a case where we read from a passed in
	 memory and the same memory being marked as stack. */
      if (!memory_entry->in_stack () && this_size && addr_info != nullptr)
	{
	  CORE_ADDR offset = start_address - addr_info->addr;

	  /* Using second buffer here because the copy_bitwise
	     doesn't support in place copy.  */
	  gdb::byte_vector temp_buf (this_size);

	  if (offset < addr_info->valaddr.size ()
	      && offset + this_size <= addr_info->valaddr.size ())
	    {
	      memcpy (temp_buf.data (), addr_info->valaddr.data (), this_size);
	      copy_bitwise (buf_ptr, 0, temp_buf.data (),
			    bit_suboffset, size_in_bits,
			    big_endian);
	      passed_in_buf = true;
	    }
	}
    }

  if (!passed_in_buf)
    {
      int optimized, unavailable;

      read_from_location (location, this->frame, 0, buf_ptr, 0, size_in_bits,
			  0, big_endian, &optimized, &unavailable);

      if (optimized)
	throw_error (OPTIMIZED_OUT_ERROR,
		     _("Can't do read-modify-write to "
		       "update bitfield; containing word "
		       "has been optimized out"));
      if (unavailable)
	throw_error (NOT_AVAILABLE_ERROR,
		     _("Can't dereference "
		       "update bitfield; containing word "
		       "is unavailable"));
    }

  return entry_factory->create_value (read_buf.data (), type);
}

dwarf_entry *
dwarf_expr_context::gdb_value_to_dwarf_entry (struct value *value)
{
  if (value_optimized_out (value))
    return entry_factory->create_undefined ();

  LONGEST offset = value_offset (value);

  switch (value_lval_const (value))
    {
      /* We can only convert struct value to a location because we
	 can't distinguish between the implicit value and
	 not_lval.  */
    case not_lval:
      {
	gdb_byte *contents_start = value_contents_raw (value) + offset;
	struct type *type = value_type (value);

	return entry_factory->create_implicit (contents_start,
					       TYPE_LENGTH (type),
					       type_byte_order (type));
      }
    case lval_memory:
      return entry_factory->create_memory (value_address (value) + offset,
					   0, value_stack (value));
    case lval_register:
      return entry_factory->create_register (VALUE_REGNUM (value), offset);
    case lval_computed:
      {
	/* Dwarf entry is enclosed by the closure anyway so we just
	   need to unwrap it here.  */
	computed_closure *closure
	  = ((computed_closure *) value_computed_closure (value));
	auto location = dynamic_cast<dwarf_location *> (closure->get_entry ());

	if (location == nullptr)
	  internal_error (__FILE__, __LINE__, _("invalid closure type"));

	location->add_bit_offset (offset * HOST_CHAR_BIT);
	return location;
      }
    default:
      internal_error (__FILE__, __LINE__, _("invalid location type"));
  }
}

struct value *
dwarf_expr_context::dwarf_entry_to_gdb_value (dwarf_entry *entry,
					      struct type *type,
					      struct type *subobj_type,
					      LONGEST subobj_offset)
{
  struct gdbarch *gdbarch = get_type_arch (type);
  struct value *retval = nullptr;

  if (subobj_type == nullptr)
    subobj_type = type;

  if (auto value = dynamic_cast<dwarf_value *> (entry))
    retval = value_to_gdb_value (value, subobj_type, subobj_offset);
  else if (dynamic_cast<dwarf_undefined *> (entry) != nullptr)
    {
      retval = allocate_value (subobj_type);
      mark_value_bytes_optimized_out (retval, subobj_offset,
				      TYPE_LENGTH (subobj_type));
    }
  else if (auto memory_entry = dynamic_cast<dwarf_memory *> (entry))
    {
      struct type *ptr_type = builtin_type (gdbarch)->builtin_data_ptr;
      CORE_ADDR address = memory_entry->get_offset ();

      if (subobj_type->code () == TYPE_CODE_FUNC
	  || subobj_type->code () == TYPE_CODE_METHOD)
	ptr_type = builtin_type (gdbarch)->builtin_func_ptr;

      address = value_as_address (value_from_pointer (ptr_type, address));
      retval = value_at_lazy (subobj_type, address + subobj_offset);
      set_value_stack (retval, memory_entry->in_stack ());
    }
  else if (auto register_entry = dynamic_cast<dwarf_register *> (entry))
    {
      unsigned int regnum = register_entry->get_regnum ();
      int gdb_regnum = dwarf_reg_to_regnum_or_error (gdbarch, regnum);

      /* Construct the value.  */
      retval = gdbarch_value_from_register (gdbarch, type,
					    gdb_regnum, get_frame_id (frame));

      /* Get the data.  */
      read_frame_register_value (retval, frame);

      if (value_optimized_out (retval))
	{
	  /* This means the register has undefined value / was not saved.
	     As we're computing the location of some variable etc. in the
	     program, not a value for inspecting a register ($pc, $sp, etc.),
	     return a generic optimized out value instead, so that we show
	     <optimized out> instead of <not saved>.  */
	  struct value *temp = allocate_value (subobj_type);
	  value_contents_copy (temp, 0, retval, 0, TYPE_LENGTH (subobj_type));
	  retval = temp;
	}
    }
  else if (auto implicit_entry = dynamic_cast<dwarf_implicit *> (entry))
    {
      size_t subtype_len = TYPE_LENGTH (subobj_type);
      size_t type_len = TYPE_LENGTH (type);

      /* To be compatible with expected error output of the existing
	 tests, the invalid synthetic pointer is not reported for
	 DW_OP_implicit_value operation.  */
      if ((subobj_offset + subtype_len > type_len)
	  && implicit_entry->get_byte_order() != BFD_ENDIAN_UNKNOWN)
	  invalid_synthetic_pointer ();

      retval = allocate_value (subobj_type);

      /* The given offset is relative to the actual object.  */
      if (implicit_entry->get_byte_order() == BFD_ENDIAN_BIG)
	subobj_offset += implicit_entry->get_size () - type_len;

      memcpy ((void *)value_contents_raw (retval),
	      (void *)(implicit_entry->get_contents () + subobj_offset),
	      subtype_len);
    }
  else if (dynamic_cast<dwarf_implicit_pointer *> (entry) != nullptr)
    {
      /* Complain if the expression is larger than the size of the
	 outer type.  */
      if (this->addr_size > HOST_CHAR_BIT * TYPE_LENGTH (type))
	invalid_synthetic_pointer ();

      computed_closure *closure
	= new computed_closure (entry, get_frame_id (frame));
      closure->incref ();

      retval
	= allocate_computed_value (subobj_type, &closure_value_funcs, closure);
      set_value_offset (retval, subobj_offset);
    }
  else if (auto composite_entry = dynamic_cast<dwarf_composite *> (entry))
    {
      size_t pieces_num = composite_entry->get_pieces_num ();
      ULONGEST bit_size = 0;

      for (unsigned int i = 0; i < pieces_num; i++)
	bit_size += composite_entry->get_bit_size_at (i);

      /* Complain if the expression is larger than the size of the
	 outer type.  */
      if (bit_size > HOST_CHAR_BIT * TYPE_LENGTH (type))
	invalid_synthetic_pointer ();

      computed_closure *closure
	= new computed_closure (entry, get_frame_id (frame));
      closure->incref ();

      retval
	= allocate_computed_value (subobj_type, &closure_value_funcs, closure);
      set_value_offset (retval, subobj_offset);
  }

  return retval;
}

/* Require that TYPE be an integral type; throw an exception if not.  */

static void
dwarf_require_integral (struct type *type)
{
  if (type->code () != TYPE_CODE_INT
      && type->code () != TYPE_CODE_CHAR
      && type->code () != TYPE_CODE_BOOL)
    error (_("integral type expected in DWARF expression"));
}

/* Return the unsigned form of TYPE.  TYPE is necessarily an integral
   type.  */

static struct type *
get_unsigned_type (struct gdbarch *gdbarch, struct type *type)
{
  switch (TYPE_LENGTH (type))
    {
    case 1:
      return builtin_type (gdbarch)->builtin_uint8;
    case 2:
      return builtin_type (gdbarch)->builtin_uint16;
    case 4:
      return builtin_type (gdbarch)->builtin_uint32;
    case 8:
      return builtin_type (gdbarch)->builtin_uint64;
    default:
      error (_("no unsigned variant found for type, while evaluating "
	       "DWARF expression"));
    }
}

/* Return the signed form of TYPE.  TYPE is necessarily an integral
   type.  */

static struct type *
get_signed_type (struct gdbarch *gdbarch, struct type *type)
{
  switch (TYPE_LENGTH (type))
    {
    case 1:
      return builtin_type (gdbarch)->builtin_int8;
    case 2:
      return builtin_type (gdbarch)->builtin_int16;
    case 4:
      return builtin_type (gdbarch)->builtin_int32;
    case 8:
      return builtin_type (gdbarch)->builtin_int64;
    default:
      error (_("no signed variant found for type, while evaluating "
	       "DWARF expression"));
    }
}

bool
dwarf_expr_context::stack_empty_p () const
{
  return stack.empty ();
}

dwarf_entry *
dwarf_expr_context::add_piece (ULONGEST bit_size, ULONGEST bit_offset)
{
  dwarf_location *piece_entry;
  dwarf_composite *composite_entry;

  if (!stack_empty_p ()
      && dynamic_cast<dwarf_composite *> (fetch (0)) == nullptr)
    {
      piece_entry = entry_factory->entry_to_location (fetch (0));
      pop ();
    }
  else
    piece_entry = entry_factory->create_undefined ();

  piece_entry->add_bit_offset (bit_offset);

  /* If stack is empty then it is a start of a new composite.  In the
     future this will check if the composite is finished or not.  */
  if (stack_empty_p ()
      || dynamic_cast<dwarf_composite *> (fetch (0)) == nullptr)
    composite_entry = entry_factory->create_composite ();
  else
    {
      composite_entry = dynamic_cast<dwarf_composite *> (fetch (0));
      pop ();
    }

  composite_entry->add_piece (piece_entry, bit_size);
  return composite_entry;
}

void
dwarf_expr_context::eval (const gdb_byte *addr, size_t len)
{
  int old_recursion_depth = this->recursion_depth;

  execute_stack_op (addr, addr + len);

  /* RECURSION_DEPTH becomes invalid if an exception was thrown here.  */

  gdb_assert (this->recursion_depth == old_recursion_depth);
}

/* See expr.h.  */

const gdb_byte *
safe_read_uleb128 (const gdb_byte *buf, const gdb_byte *buf_end,
		   uint64_t *r)
{
  buf = gdb_read_uleb128 (buf, buf_end, r);
  if (buf == NULL)
    error (_("DWARF expression error: ran off end of buffer reading uleb128 value"));
  return buf;
}

/* See expr.h.  */

const gdb_byte *
safe_read_sleb128 (const gdb_byte *buf, const gdb_byte *buf_end,
		   int64_t *r)
{
  buf = gdb_read_sleb128 (buf, buf_end, r);
  if (buf == NULL)
    error (_("DWARF expression error: ran off end of buffer reading sleb128 value"));
  return buf;
}

const gdb_byte *
safe_skip_leb128 (const gdb_byte *buf, const gdb_byte *buf_end)
{
  buf = gdb_skip_leb128 (buf, buf_end);
  if (buf == NULL)
    error (_("DWARF expression error: ran off end of buffer reading leb128 value"));
  return buf;
}

/* See expr.h.  */

void
dwarf_expr_require_composition (const gdb_byte *op_ptr, const gdb_byte *op_end,
				const char *op_name)
{
  if (op_ptr != op_end && *op_ptr != DW_OP_piece && *op_ptr != DW_OP_bit_piece
      && *op_ptr != DW_OP_GNU_uninit)
    error (_("DWARF-2 expression error: `%s' operations must be "
	     "used either alone or in conjunction with DW_OP_piece "
	     "or DW_OP_bit_piece."),
	   op_name);
}

/* Return true iff the types T1 and T2 are "the same".  This only does
   checks that might reasonably be needed to compare DWARF base
   types.  */

static int
base_types_equal_p (struct type *t1, struct type *t2)
{
  if (t1->code () != t2->code ())
    return 0;
  if (t1->is_unsigned () != t2->is_unsigned ())
    return 0;
  return TYPE_LENGTH (t1) == TYPE_LENGTH (t2);
}

/* See expr.h.  */

int
dwarf_block_to_dwarf_reg (const gdb_byte *buf, const gdb_byte *buf_end)
{
  uint64_t dwarf_reg;

  if (buf_end <= buf)
    return -1;
  if (*buf >= DW_OP_reg0 && *buf <= DW_OP_reg31)
    {
      if (buf_end - buf != 1)
	return -1;
      return *buf - DW_OP_reg0;
    }

  if (*buf == DW_OP_regval_type || *buf == DW_OP_GNU_regval_type)
    {
      buf++;
      buf = gdb_read_uleb128 (buf, buf_end, &dwarf_reg);
      if (buf == NULL)
	return -1;
      buf = gdb_skip_leb128 (buf, buf_end);
      if (buf == NULL)
	return -1;
    }
  else if (*buf == DW_OP_regx)
    {
      buf++;
      buf = gdb_read_uleb128 (buf, buf_end, &dwarf_reg);
      if (buf == NULL)
	return -1;
    }
  else
    return -1;
  if (buf != buf_end || (int) dwarf_reg != dwarf_reg)
    return -1;
  return dwarf_reg;
}

/* See expr.h.  */

int
dwarf_block_to_dwarf_reg_deref (const gdb_byte *buf, const gdb_byte *buf_end,
				CORE_ADDR *deref_size_return)
{
  uint64_t dwarf_reg;
  int64_t offset;

  if (buf_end <= buf)
    return -1;

  if (*buf >= DW_OP_breg0 && *buf <= DW_OP_breg31)
    {
      dwarf_reg = *buf - DW_OP_breg0;
      buf++;
      if (buf >= buf_end)
	return -1;
    }
  else if (*buf == DW_OP_bregx)
    {
      buf++;
      buf = gdb_read_uleb128 (buf, buf_end, &dwarf_reg);
      if (buf == NULL)
	return -1;
      if ((int) dwarf_reg != dwarf_reg)
       return -1;
    }
  else
    return -1;

  buf = gdb_read_sleb128 (buf, buf_end, &offset);
  if (buf == NULL)
    return -1;
  if (offset != 0)
    return -1;

  if (*buf == DW_OP_deref)
    {
      buf++;
      *deref_size_return = -1;
    }
  else if (*buf == DW_OP_deref_size)
    {
      buf++;
      if (buf >= buf_end)
       return -1;
      *deref_size_return = *buf++;
    }
  else
    return -1;

  if (buf != buf_end)
    return -1;

  return dwarf_reg;
}

/* See expr.h.  */

int
dwarf_block_to_fb_offset (const gdb_byte *buf, const gdb_byte *buf_end,
			  CORE_ADDR *fb_offset_return)
{
  int64_t fb_offset;

  if (buf_end <= buf)
    return 0;

  if (*buf != DW_OP_fbreg)
    return 0;
  buf++;

  buf = gdb_read_sleb128 (buf, buf_end, &fb_offset);
  if (buf == NULL)
    return 0;
  *fb_offset_return = fb_offset;
  if (buf != buf_end || fb_offset != (LONGEST) *fb_offset_return)
    return 0;

  return 1;
}

/* See expr.h.  */

int
dwarf_block_to_sp_offset (struct gdbarch *gdbarch, const gdb_byte *buf,
			  const gdb_byte *buf_end, CORE_ADDR *sp_offset_return)
{
  uint64_t dwarf_reg;
  int64_t sp_offset;

  if (buf_end <= buf)
    return 0;
  if (*buf >= DW_OP_breg0 && *buf <= DW_OP_breg31)
    {
      dwarf_reg = *buf - DW_OP_breg0;
      buf++;
    }
  else
    {
      if (*buf != DW_OP_bregx)
       return 0;
      buf++;
      buf = gdb_read_uleb128 (buf, buf_end, &dwarf_reg);
      if (buf == NULL)
	return 0;
    }

  if (dwarf_reg_to_regnum (gdbarch, dwarf_reg)
      != gdbarch_sp_regnum (gdbarch))
    return 0;

  buf = gdb_read_sleb128 (buf, buf_end, &sp_offset);
  if (buf == NULL)
    return 0;
  *sp_offset_return = sp_offset;
  if (buf != buf_end || sp_offset != (LONGEST) *sp_offset_return)
    return 0;

  return 1;
}

void
dwarf_expr_context::execute_stack_op (const gdb_byte *op_ptr,
				      const gdb_byte *op_end)
{
  enum bfd_endian byte_order = gdbarch_byte_order (this->gdbarch);
  /* Old-style "untyped" DWARF values need special treatment in a
     couple of places, specifically DW_OP_mod and DW_OP_shr.  We need
     a special type for these values so we can distinguish them from
     values that have an explicit type, because explicitly-typed
     values do not need special treatment.  This special type must be
     different (in the `==' sense) from any base type coming from the
     CU.  */
  struct type *address_type = this->address_type ();

  if (this->recursion_depth > this->max_recursion_depth)
    error (_("DWARF-2 expression error: Loop detected (%d)."),
	   this->recursion_depth);
  this->recursion_depth++;

  while (op_ptr < op_end)
    {
      enum dwarf_location_atom op = (enum dwarf_location_atom) *op_ptr++;
      ULONGEST result;
      uint64_t uoffset, reg;
      int64_t offset;
      struct dwarf_entry *result_entry = NULL;

      /* The DWARF expression might have a bug causing an infinite
	 loop.  In that case, quitting is the only way out.  */
      QUIT;

      switch (op)
	{
	case DW_OP_lit0:
	case DW_OP_lit1:
	case DW_OP_lit2:
	case DW_OP_lit3:
	case DW_OP_lit4:
	case DW_OP_lit5:
	case DW_OP_lit6:
	case DW_OP_lit7:
	case DW_OP_lit8:
	case DW_OP_lit9:
	case DW_OP_lit10:
	case DW_OP_lit11:
	case DW_OP_lit12:
	case DW_OP_lit13:
	case DW_OP_lit14:
	case DW_OP_lit15:
	case DW_OP_lit16:
	case DW_OP_lit17:
	case DW_OP_lit18:
	case DW_OP_lit19:
	case DW_OP_lit20:
	case DW_OP_lit21:
	case DW_OP_lit22:
	case DW_OP_lit23:
	case DW_OP_lit24:
	case DW_OP_lit25:
	case DW_OP_lit26:
	case DW_OP_lit27:
	case DW_OP_lit28:
	case DW_OP_lit29:
	case DW_OP_lit30:
	case DW_OP_lit31:
	  result = op - DW_OP_lit0;
	  result_entry = entry_factory->create_value (result, address_type);
	  break;

	case DW_OP_addr:
	  result = extract_unsigned_integer (op_ptr,
					     this->addr_size, byte_order);
	  op_ptr += this->addr_size;
	  /* Some versions of GCC emit DW_OP_addr before
	     DW_OP_GNU_push_tls_address.  In this case the value is an
	     index, not an address.  We don't support things like
	     branching between the address and the TLS op.  */
	  if (op_ptr >= op_end || *op_ptr != DW_OP_GNU_push_tls_address)
	    {
	      result += this->per_objfile->objfile->text_section_offset ();
	      result_entry = entry_factory->create_memory (result);
	    }
	  else
	    /* This is a special case where the value is expected to be
	       created instead of memory location.  */
	    result_entry = entry_factory->create_value (result, address_type);
	  break;

	case DW_OP_addrx:
	case DW_OP_GNU_addr_index:
	  ensure_have_per_cu (this->per_cu, "DW_OP_addrx");

	  op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	  result = dwarf2_read_addr_index (this->per_cu, this->per_objfile,
					   uoffset);
	  result += this->per_objfile->objfile->text_section_offset ();
	  result_entry = entry_factory->create_memory (result);
	  break;
	case DW_OP_GNU_const_index:
	  ensure_have_per_cu (per_cu, "DW_OP_GNU_const_index");

	  op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	  result = dwarf2_read_addr_index (this->per_cu, this->per_objfile,
					   uoffset);
	  result_entry = entry_factory->create_value (result, address_type);
	  break;

	case DW_OP_const1u:
	  result = extract_unsigned_integer (op_ptr, 1, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 1;
	  break;
	case DW_OP_const1s:
	  result = extract_signed_integer (op_ptr, 1, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 1;
	  break;
	case DW_OP_const2u:
	  result = extract_unsigned_integer (op_ptr, 2, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 2;
	  break;
	case DW_OP_const2s:
	  result = extract_signed_integer (op_ptr, 2, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 2;
	  break;
	case DW_OP_const4u:
	  result = extract_unsigned_integer (op_ptr, 4, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 4;
	  break;
	case DW_OP_const4s:
	  result = extract_signed_integer (op_ptr, 4, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 4;
	  break;
	case DW_OP_const8u:
	  result = extract_unsigned_integer (op_ptr, 8, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 8;
	  break;
	case DW_OP_const8s:
	  result = extract_signed_integer (op_ptr, 8, byte_order);
	  result_entry = entry_factory->create_value (result, address_type);
	  op_ptr += 8;
	  break;
	case DW_OP_constu:
	  op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	  result = uoffset;
	  result_entry = entry_factory->create_value (result, address_type);
	  break;
	case DW_OP_consts:
	  op_ptr = safe_read_sleb128 (op_ptr, op_end, &offset);
	  result = offset;
	  result_entry = entry_factory->create_value (result, address_type);
	  break;

	/* The DW_OP_reg operations are required to occur alone in
	   location expressions.  */
	case DW_OP_reg0:
	case DW_OP_reg1:
	case DW_OP_reg2:
	case DW_OP_reg3:
	case DW_OP_reg4:
	case DW_OP_reg5:
	case DW_OP_reg6:
	case DW_OP_reg7:
	case DW_OP_reg8:
	case DW_OP_reg9:
	case DW_OP_reg10:
	case DW_OP_reg11:
	case DW_OP_reg12:
	case DW_OP_reg13:
	case DW_OP_reg14:
	case DW_OP_reg15:
	case DW_OP_reg16:
	case DW_OP_reg17:
	case DW_OP_reg18:
	case DW_OP_reg19:
	case DW_OP_reg20:
	case DW_OP_reg21:
	case DW_OP_reg22:
	case DW_OP_reg23:
	case DW_OP_reg24:
	case DW_OP_reg25:
	case DW_OP_reg26:
	case DW_OP_reg27:
	case DW_OP_reg28:
	case DW_OP_reg29:
	case DW_OP_reg30:
	case DW_OP_reg31:
	  dwarf_expr_require_composition (op_ptr, op_end, "DW_OP_reg");

	  result = op - DW_OP_reg0;
	  result_entry = entry_factory->create_register (result);
	  break;

	case DW_OP_regx:
	  op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
	  dwarf_expr_require_composition (op_ptr, op_end, "DW_OP_regx");

	  result = reg;
	  result_entry = entry_factory->create_register (reg);
	  break;

	case DW_OP_implicit_value:
	  {
	    uint64_t len;

	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &len);
	    if (op_ptr + len > op_end)
	      error (_("DW_OP_implicit_value: too few bytes available."));
	    result_entry = entry_factory->create_implicit (op_ptr, len,
							   BFD_ENDIAN_UNKNOWN);
	    op_ptr += len;
	    dwarf_expr_require_composition (op_ptr, op_end,
					    "DW_OP_implicit_value");
	  }
	  break;

	case DW_OP_stack_value:
	  {
	    dwarf_value *value
	      = entry_factory->entry_to_value (fetch (0), address_type);
	    pop ();

	    struct type* type = value->get_type ();

	    result_entry
	      = entry_factory->create_implicit (value->get_contents (),
						TYPE_LENGTH (type),
						type_byte_order (type));

	    dwarf_expr_require_composition (op_ptr, op_end, "DW_OP_stack_value");
	  }
	  break;

	case DW_OP_implicit_pointer:
	case DW_OP_GNU_implicit_pointer:
	  {
	    int64_t len;
	    ensure_have_per_cu (per_cu, "DW_OP_implicit_pointer");

	    /* The referred-to DIE of sect_offset kind.  */
	    sect_offset die_offset
	      = (sect_offset) extract_unsigned_integer (op_ptr,
							this->ref_addr_size,
							byte_order);
	    op_ptr += this->ref_addr_size;

	    /* The byte offset into the data.  */
	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &len);
	    result_entry
	      = entry_factory->create_implicit_pointer (this->per_objfile,
							this->per_cu,
							this->addr_size,
							die_offset, len);

	    dwarf_expr_require_composition (op_ptr, op_end,
					    "DW_OP_implicit_pointer");
	  }
	  break;

	case DW_OP_breg0:
	case DW_OP_breg1:
	case DW_OP_breg2:
	case DW_OP_breg3:
	case DW_OP_breg4:
	case DW_OP_breg5:
	case DW_OP_breg6:
	case DW_OP_breg7:
	case DW_OP_breg8:
	case DW_OP_breg9:
	case DW_OP_breg10:
	case DW_OP_breg11:
	case DW_OP_breg12:
	case DW_OP_breg13:
	case DW_OP_breg14:
	case DW_OP_breg15:
	case DW_OP_breg16:
	case DW_OP_breg17:
	case DW_OP_breg18:
	case DW_OP_breg19:
	case DW_OP_breg20:
	case DW_OP_breg21:
	case DW_OP_breg22:
	case DW_OP_breg23:
	case DW_OP_breg24:
	case DW_OP_breg25:
	case DW_OP_breg26:
	case DW_OP_breg27:
	case DW_OP_breg28:
	case DW_OP_breg29:
	case DW_OP_breg30:
	case DW_OP_breg31:
	  {
	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &offset);
	    ensure_have_frame (this->frame, "DW_OP_breg");

	    reg = op - DW_OP_breg0;

	    int regnum = dwarf_reg_to_regnum_or_error (this->gdbarch, reg);
	    ULONGEST reg_size = register_size (this->gdbarch, regnum);
	    dwarf_location *location = entry_factory->create_register (reg);
	    result_entry
	      = dwarf_entry_deref (location, address_type, reg_size);
	    location = entry_factory->entry_to_location (result_entry);
	    location->add_bit_offset (offset * HOST_CHAR_BIT);
	    result_entry = location;
	  }
	  break;
	case DW_OP_bregx:
	  {
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &offset);
	    ensure_have_frame (this->frame, "DW_OP_bregx");

	    int regnum = dwarf_reg_to_regnum_or_error (this->gdbarch, reg);
	    ULONGEST reg_size = register_size (this->gdbarch, regnum);
	    dwarf_location *location = entry_factory->create_register (reg);
	    result_entry
	      = dwarf_entry_deref (location, address_type, reg_size);
	    location = entry_factory->entry_to_location (result_entry);
	    location->add_bit_offset (offset * HOST_CHAR_BIT);
	    result_entry = location;
	  }
	  break;
	case DW_OP_fbreg:
	  {
	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &offset);

	    /* Rather than create a whole new context, we simply
	       backup the current stack locally and install a new empty stack,
	       then reset it afterwards, effectively erasing whatever the
	       recursive call put there.  */
	    std::vector<dwarf_entry *> saved_stack = std::move (stack);
	    stack.clear ();

	    const gdb_byte *datastart;
	    size_t datalen;

	    this->get_frame_base (&datastart, &datalen);
	    eval (datastart, datalen);
	    result_entry = fetch (0);

	    if (dynamic_cast<dwarf_register *> (result_entry) != nullptr)
	      result_entry = dwarf_entry_deref (result_entry, address_type);

	    result_entry = entry_factory->entry_to_location (result_entry);
	    auto memory_entry = dynamic_cast<dwarf_memory *> (result_entry);

	    /* If we get anything else then memory location here,
	       the DWARF standard defines the expression as ill formed.  */
	    if (memory_entry == nullptr)
	      ill_formed_expression ();

	    memory_entry->add_bit_offset (offset * HOST_CHAR_BIT);
	    memory_entry->set_stack (true);
	    result_entry = memory_entry;

	    /* Restore the content of the original stack.  */
	    stack = std::move (saved_stack);
	  }
	  break;

	case DW_OP_dup:
	  result_entry = fetch (0);
	  break;

	case DW_OP_drop:
	  pop ();
	  goto no_push;

	case DW_OP_pick:
	  offset = *op_ptr++;
	  result_entry = fetch (offset);
	  break;
	  
	case DW_OP_swap:
	  {
	    if (stack.size () < 2)
	       error (_("Not enough elements for "
			"DW_OP_swap.  Need 2, have %zu."),
		      stack.size ());

	    dwarf_entry *temp = stack[stack.size () - 1];
	    stack[stack.size () - 1] = stack[stack.size () - 2];
	    stack[stack.size () - 2] = temp;
	    goto no_push;
	  }

	case DW_OP_over:
	  result_entry = fetch (1);
	  break;

	case DW_OP_rot:
	  {
	    if (stack.size () < 3)
	       error (_("Not enough elements for "
			"DW_OP_rot.  Need 3, have %zu."),
		      stack.size ());

	    dwarf_entry *temp = stack[stack.size () - 1];
	    stack[stack.size () - 1] = stack[stack.size () - 2];
	    stack[stack.size () - 2] = stack[stack.size () - 3];
	    stack[stack.size () - 3] = temp;
	    goto no_push;
	  }

	case DW_OP_deref:
	case DW_OP_deref_size:
	case DW_OP_deref_type:
	case DW_OP_GNU_deref_type:
	  {
	    int addr_size = (op == DW_OP_deref ? this->addr_size : *op_ptr++);
	    struct type *type = address_type;

	    if (op == DW_OP_deref_type || op == DW_OP_GNU_deref_type)
	      {
		op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
		cu_offset type_die_cu_off = (cu_offset) uoffset;
		type = get_base_type (type_die_cu_off, 0);
		addr_size = TYPE_LENGTH (type);
	      }

	    result_entry = dwarf_entry_deref (fetch (0), type, addr_size);
	    pop ();
	  }
	  break;

	case DW_OP_abs:
	case DW_OP_neg:
	case DW_OP_not:
	case DW_OP_plus_uconst:
	  {
	    /* Unary operations.  */
	    dwarf_value *arg
	      = entry_factory->entry_to_value (fetch (0), address_type);
	    pop ();

	    switch (op)
	      {
	      case DW_OP_abs:
		{
		  struct value *arg_value
		    = value_to_gdb_value (arg, arg->get_type ());

		  if (value_less (arg_value,
				  value_zero (arg->get_type (), not_lval)))
		    arg = entry_factory->value_negation_op (arg);
		}
		break;
	      case DW_OP_neg:
		arg = entry_factory->value_negation_op (arg);
		break;
	      case DW_OP_not:
		dwarf_require_integral (arg->get_type ());
		arg = entry_factory->value_complement_op (arg);
		break;
	      case DW_OP_plus_uconst:
		dwarf_require_integral (arg->get_type ());
		op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
		result = arg->to_long () + reg;
		arg = entry_factory->create_value (result, address_type);
		break;
	      }
	    result_entry = arg;
	  }
	  break;

	case DW_OP_and:
	case DW_OP_div:
	case DW_OP_minus:
	case DW_OP_mod:
	case DW_OP_mul:
	case DW_OP_or:
	case DW_OP_plus:
	case DW_OP_shl:
	case DW_OP_shr:
	case DW_OP_shra:
	case DW_OP_xor:
	case DW_OP_le:
	case DW_OP_ge:
	case DW_OP_eq:
	case DW_OP_lt:
	case DW_OP_gt:
	case DW_OP_ne:
	  {
	    /* Binary operations.  */
	    dwarf_value *arg2
	      = entry_factory->entry_to_value (fetch (0), address_type);
	    pop ();

	    dwarf_value *arg1
	      = entry_factory->entry_to_value (fetch (0), address_type);
	    pop ();

	    if (! base_types_equal_p (arg1->get_type (), arg2->get_type ()))
	      error (_("Incompatible types on DWARF stack"));

	    dwarf_value *op_result;

	    switch (op)
	      {
	      case DW_OP_and:
		dwarf_require_integral (arg1->get_type ());
		dwarf_require_integral (arg2->get_type ());
		op_result = entry_factory->value_binary_op (arg1, arg2,
							    BINOP_BITWISE_AND);
		break;
	      case DW_OP_div:
		op_result
		  = entry_factory->value_binary_op (arg1, arg2, BINOP_DIV);
		break;
	      case DW_OP_minus:
		op_result
		  = entry_factory->value_binary_op (arg1, arg2, BINOP_SUB);
		break;
	      case DW_OP_mod:
		{
		  int cast_back = 0;
		  struct type *orig_type = arg1->get_type ();

		  /* We have to special-case "old-style" untyped values
		     -- these must have mod computed using unsigned
		     math.  */
		  if (orig_type == address_type)
		    {
		      struct type *utype
			= get_unsigned_type (this->gdbarch, orig_type);

		      cast_back = 1;
		      arg1 = entry_factory->value_cast_op (arg1, utype);
		      arg2 = entry_factory->value_cast_op (arg2, utype);
		    }
		  /* Note that value_binop doesn't handle float or
		     decimal float here.  This seems unimportant.  */
		  op_result
		    = entry_factory->value_binary_op (arg1, arg2, BINOP_MOD);
		  if (cast_back)
		    op_result
		      = entry_factory->value_cast_op (op_result, orig_type);
		}
		break;
	      case DW_OP_mul:
		op_result
		  = entry_factory->value_binary_op (arg1, arg2, BINOP_MUL);
		break;
	      case DW_OP_or:
		dwarf_require_integral (arg1->get_type ());
		dwarf_require_integral (arg2->get_type ());
		op_result = entry_factory->value_binary_op (arg1, arg2,
							    BINOP_BITWISE_IOR);
		break;
	      case DW_OP_plus:
		op_result
		  = entry_factory->value_binary_op (arg1, arg2, BINOP_ADD);
		break;
	      case DW_OP_shl:
		dwarf_require_integral (arg1->get_type ());
		dwarf_require_integral (arg2->get_type ());
		op_result
		  = entry_factory->value_binary_op (arg1, arg2, BINOP_LSH);
		break;
	      case DW_OP_shr:
		dwarf_require_integral (arg1->get_type ());
		dwarf_require_integral (arg2->get_type ());
		if (!arg1->get_type ()->is_unsigned ())
		  {
		    struct type *utype
		      = get_unsigned_type (this->gdbarch, arg1->get_type ());

		    arg1 = entry_factory->value_cast_op (arg1, utype);
		  }

		op_result
		  = entry_factory->value_binary_op (arg1, arg2, BINOP_RSH);
		/* Make sure we wind up with the same type we started
		   with.  */
		if (op_result->get_type () != arg2->get_type ())
		  op_result = entry_factory->value_cast_op (op_result,
							    arg2->get_type ());
		break;
	      case DW_OP_shra:
		dwarf_require_integral (arg1->get_type ());
		dwarf_require_integral (arg2->get_type ());
		if (arg1->get_type ()->is_unsigned ())
		  {
		    struct type *stype
		      = get_signed_type (this->gdbarch, arg1->get_type ());

		    arg1 = entry_factory->value_cast_op (arg1, stype);
		  }

		op_result
		  = entry_factory->value_binary_op (arg1, arg2, BINOP_RSH);
		/* Make sure we wind up with the same type we started  with.  */
		if (op_result->get_type () != arg2->get_type ())
		  op_result = entry_factory->value_cast_op (op_result,
							    arg2->get_type ());
		break;
	      case DW_OP_xor:
		dwarf_require_integral (arg1->get_type ());
		dwarf_require_integral (arg2->get_type ());
		op_result = entry_factory->value_binary_op (arg1, arg2,
							    BINOP_BITWISE_XOR);
		break;
	      case DW_OP_le:
		/* A <= B is !(B < A).  */
		result = ! dwarf_value_less_op (arg2, arg1);
		op_result = entry_factory->create_value (result, address_type);
		break;
	      case DW_OP_ge:
		/* A >= B is !(A < B).  */
		result = ! dwarf_value_less_op (arg1, arg2);
		op_result = entry_factory->create_value (result, address_type);
		break;
	      case DW_OP_eq:
		result = dwarf_value_equal_op (arg1, arg2);
		op_result = entry_factory->create_value (result, address_type);
		break;
	      case DW_OP_lt:
		result = dwarf_value_less_op (arg1, arg2);
		op_result = entry_factory->create_value (result, address_type);
		break;
	      case DW_OP_gt:
		/* A > B is B < A.  */
		result = dwarf_value_less_op (arg2, arg1);
		op_result = entry_factory->create_value (result, address_type);
		break;
	      case DW_OP_ne:
		result = ! dwarf_value_equal_op (arg1, arg2);
		op_result = entry_factory->create_value (result, address_type);
		break;
	      default:
		internal_error (__FILE__, __LINE__,
				_("Can't be reached."));
	      }
	    result_entry = op_result;
	  }
	  break;

	case DW_OP_call_frame_cfa:
	  ensure_have_frame (this->frame, "DW_OP_call_frame_cfa");

	  result = dwarf2_frame_cfa (this->frame);
	  result_entry = entry_factory->create_memory (result, 0, true);
	  break;

	case DW_OP_GNU_push_tls_address:
	case DW_OP_form_tls_address:
	  /* Variable is at a constant offset in the thread-local
	  storage block into the objfile for the current thread and
	  the dynamic linker module containing this expression.  Here
	  we return returns the offset from that base.  The top of the
	  stack has the offset from the beginning of the thread
	  control block at which the variable is located.  Nothing
	  should follow this operator, so the top of stack would be
	  returned.  */
	  result
	    = entry_factory->entry_to_value (fetch (0),
					     address_type)->to_long ();
	  pop ();
	  result = target_translate_tls_address (this->per_objfile->objfile,
						 result);
	  result_entry = entry_factory->create_memory (result);
	  break;

	case DW_OP_skip:
	  offset = extract_signed_integer (op_ptr, 2, byte_order);
	  op_ptr += 2;
	  op_ptr += offset;
	  goto no_push;

	case DW_OP_bra:
	  {
	    dwarf_value *dwarf_value
	      = entry_factory->entry_to_value (fetch (0), address_type);

	    offset = extract_signed_integer (op_ptr, 2, byte_order);
	    op_ptr += 2;
	    dwarf_require_integral (dwarf_value->get_type ());
	    if (dwarf_value->to_long () != 0)
	      op_ptr += offset;
	    pop ();
	  }
	  goto no_push;

	case DW_OP_nop:
	  goto no_push;

	case DW_OP_piece:
	  {
	    uint64_t size;

	    /* Record the piece.  */
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &size);
	    result_entry = add_piece (HOST_CHAR_BIT * size, 0);
	  }
	  break;

	case DW_OP_bit_piece:
	  {
	    uint64_t size, uleb_offset;

	    /* Record the piece.  */
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &size);
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uleb_offset);
	    result_entry = add_piece (size, uleb_offset);
	  }
	  break;

	case DW_OP_GNU_uninit:
	  {
	    if (op_ptr != op_end)
	      error (_("DWARF-2 expression error: DW_OP_GNU_uninit must always "
		     "be the very last op."));

	    auto location = dynamic_cast<dwarf_location *> (fetch (0));

	    if (location == nullptr)
	      ill_formed_expression ();

	    location->set_initialised (false);
	    result_entry = location;
	  }
	  goto no_push;

	case DW_OP_call2:
	  {
	    cu_offset cu_off
	      = (cu_offset) extract_unsigned_integer (op_ptr, 2, byte_order);
	    op_ptr += 2;
	    this->dwarf_call (cu_off);
	  }
	  goto no_push;

	case DW_OP_call4:
	  {
	    cu_offset cu_off
	      = (cu_offset) extract_unsigned_integer (op_ptr, 4, byte_order);
	    op_ptr += 4;
	    this->dwarf_call (cu_off);
	  }
	  goto no_push;

	case DW_OP_GNU_variable_value:
	  {
	    ensure_have_per_cu (per_cu, "DW_OP_GNU_variable_value");

	    sect_offset sect_off
	      = (sect_offset) extract_unsigned_integer (op_ptr,
							this->ref_addr_size,
							byte_order);
	    op_ptr += this->ref_addr_size;
	    struct value *value
	      = sect_variable_value (sect_off, per_cu, per_objfile);
	    value = value_cast (address_type, value);

	    result_entry = gdb_value_to_dwarf_entry (value);

	    if (dynamic_cast<dwarf_undefined *> (result_entry) != nullptr)
	      error_value_optimized_out ();
	    else
	      result_entry = dwarf_entry_deref (result_entry, address_type);
	  }
	  break;
	
	case DW_OP_entry_value:
	case DW_OP_GNU_entry_value:
	  {
	    uint64_t len;
	    union call_site_parameter_u kind_u;

	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &len);
	    if (op_ptr + len > op_end)
	      error (_("DW_OP_entry_value: too few bytes available."));

	    kind_u.dwarf_reg = dwarf_block_to_dwarf_reg (op_ptr, op_ptr + len);
	    if (kind_u.dwarf_reg != -1)
	      {
		op_ptr += len;
		this->push_dwarf_reg_entry_value (CALL_SITE_PARAMETER_DWARF_REG,
						  kind_u,
						  -1 /* deref_size */);
		goto no_push;
	      }

	    CORE_ADDR deref_size;

	    kind_u.dwarf_reg = dwarf_block_to_dwarf_reg_deref (op_ptr,
							       op_ptr + len,
							       &deref_size);
	    if (kind_u.dwarf_reg != -1)
	      {
		if (deref_size == -1)
		  deref_size = this->addr_size;
		op_ptr += len;
		this->push_dwarf_reg_entry_value (CALL_SITE_PARAMETER_DWARF_REG,
						  kind_u, deref_size);
		goto no_push;
	      }

	    error (_("DWARF-2 expression error: DW_OP_entry_value is "
		     "supported only for single DW_OP_reg* "
		     "or for DW_OP_breg*(0)+DW_OP_deref*"));
	  }

	case DW_OP_GNU_parameter_ref:
	  {
	    union call_site_parameter_u kind_u;

	    kind_u.param_cu_off
	      = (cu_offset) extract_unsigned_integer (op_ptr, 4, byte_order);
	    op_ptr += 4;
	    this->push_dwarf_reg_entry_value (CALL_SITE_PARAMETER_PARAM_OFFSET,
					      kind_u,
					      -1 /* deref_size */);
	  }
	  goto no_push;

	case DW_OP_const_type:
	case DW_OP_GNU_const_type:
	  {
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    cu_offset type_die_cu_off = (cu_offset) uoffset;

	    int n = *op_ptr++;
	    const gdb_byte *data = op_ptr;
	    op_ptr += n;

	    struct type *type = get_base_type (type_die_cu_off, n);
	    result_entry
	      = entry_factory->create_value (data, type);
	  }
	  break;

	case DW_OP_regval_type:
	case DW_OP_GNU_regval_type:
	  {
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    cu_offset type_die_cu_off = (cu_offset) uoffset;

	    ensure_have_frame (this->frame, "DW_OP_regval_type");
	    struct type *type = get_base_type (type_die_cu_off, 0);

	    dwarf_register *register_descr
	      = entry_factory->create_register (reg);
	    result_entry = dwarf_entry_deref (register_descr, type);
	  }
	  break;

	case DW_OP_convert:
	case DW_OP_GNU_convert:
	case DW_OP_reinterpret:
	case DW_OP_GNU_reinterpret:
	  {
	    dwarf_value *dwarf_value
	      = entry_factory->entry_to_value (fetch (0), address_type);

	    pop ();

	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    cu_offset type_die_cu_off = (cu_offset) uoffset;

	    struct type *type;

	    if (to_underlying (type_die_cu_off) == 0)
	      type = address_type;
	    else
	      type = get_base_type (type_die_cu_off, 0);

	    if (op == DW_OP_convert || op == DW_OP_GNU_convert)
	      dwarf_value = entry_factory->value_cast_op (dwarf_value, type);
	    else if (type == dwarf_value->get_type ())
	      {
		/* Nothing.  */
	      }
	    else if (TYPE_LENGTH (type)
		     != TYPE_LENGTH (dwarf_value->get_type ()))
	      error (_("DW_OP_reinterpret has wrong size"));
	    else
	      dwarf_value
		= entry_factory->create_value (dwarf_value->get_contents (),
					       type);
	    result_entry = dwarf_value;
	  }
	  break;

	case DW_OP_push_object_address:
	  if (addr_info == nullptr)
	    error (_("Location address is not set."));

	  /* Return the address of the object we are currently observing.  */
	  result_entry
	    = entry_factory->create_memory (this->addr_info->addr);
	  break;

	default:
	  error (_("Unhandled dwarf expression opcode 0x%x"), op);
	}

      /* Most things push a result value.  */
      gdb_assert (result_entry != NULL);
      push (result_entry);
    no_push:
      ;
    }

  this->recursion_depth--;
  gdb_assert (this->recursion_depth >= 0);
}

/* See expr.h.  */

struct value *
dwarf2_eval_exp (const gdb_byte *addr, size_t len, bool as_lval,
		 struct dwarf2_per_objfile *per_objfile,
		 struct dwarf2_per_cu_data *per_cu,
		 struct frame_info *frame, int addr_size,
		 std::vector<struct value *> *init_values,
		 const struct property_addr_info *addr_info,
		 struct type *type, struct type *subobj_type,
		 LONGEST subobj_offset)
{
  dwarf_expr_context ctx (per_objfile, addr_size);

  return ctx.eval_exp (addr, len, as_lval, per_cu,
		       frame, init_values, addr_info,
		       type, subobj_type, subobj_offset);
}

void _initialize_dwarf2expr ();
void
_initialize_dwarf2expr ()
{
  dwarf_arch_cookie
    = gdbarch_data_register_post_init (dwarf_gdbarch_types_init);
}
