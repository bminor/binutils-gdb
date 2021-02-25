/* DWARF 2 Expression Evaluator.

   Copyright (C) 2001-2021 Free Software Foundation, Inc.

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
#include "objfiles.h"
#include "inferior.h"
#include "observable.h"

/* DWARF evaluator only supports targets with byte size of 8 bits.

   To avoid using hard coded number everywhere, the existing
   HOST_CHAR_BIT constant is used, because it is guaranteed to
   always be equal 8.  */

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
ensure_have_frame (frame_info *frame, const char *op_name)
{
  if (frame == nullptr)
    throw_error (GENERIC_ERROR,
		 _("%s evaluation requires a frame."), op_name);
}

/* Ensure that a PER_CU is defined and throw an exception otherwise.  */

static void
ensure_have_per_cu (dwarf2_per_cu_data *per_cu, const char* op_name)
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

static void ATTRIBUTE_NORETURN
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
read_from_register (frame_info *frame, int regnum,
		    CORE_ADDR offset, gdb::array_view<gdb_byte> buf,
		    int *optimized, int *unavailable)
{
  gdbarch *arch = get_frame_arch (frame);
  int regsize = register_size (arch, regnum);
  int numregs = gdbarch_num_cooked_regs (arch);
  int length = buf.size ();

  /* If a register is wholly inside the OFFSET, skip it.  */
  if (frame == NULL || !regsize
      || offset + length > regsize || numregs < regnum)
    {
      *optimized = 0;
      *unavailable = 1;
      return;
    }

  gdb::byte_vector temp_buf (regsize);
  enum lval_type lval;
  CORE_ADDR address;
  int realnum;

  frame_register (frame, regnum, optimized, unavailable,
		  &lval, &address, &realnum, temp_buf.data ());

  if (!*optimized && !*unavailable)
     memcpy (buf.data (), (char *) temp_buf.data () + offset, length);

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
write_to_register (frame_info *frame, int regnum,
		   CORE_ADDR offset, gdb::array_view<gdb_byte> buf,
		   int *optimized, int *unavailable)
{
  gdbarch *arch = get_frame_arch (frame);
  int regsize = register_size (arch, regnum);
  int numregs = gdbarch_num_cooked_regs (arch);
  int length = buf.size ();

  /* If a register is wholly inside of OFFSET, skip it.  */
  if (frame == NULL || !regsize
     || offset + length > regsize || numregs < regnum)
    {
      *optimized = 0;
      *unavailable = 1;
      return;
    }

  gdb::byte_vector temp_buf (regsize);
  enum lval_type lval;
  CORE_ADDR address;
  int realnum;

  frame_register (frame, regnum, optimized, unavailable,
		  &lval, &address, &realnum, temp_buf.data ());

  if (!*optimized && !*unavailable)
    {
      memcpy ((char *) temp_buf.data () + offset, buf.data (), length);

      put_frame_register (frame, regnum, temp_buf.data ());
    }

  return;
}

/* Helper for read_from_memory and write_to_memory.  */

static void
xfer_memory (CORE_ADDR address, gdb_byte *readbuf,
	     const gdb_byte *writebuf,
	     size_t length, bool stack, int *unavailable)
{
  *unavailable = 0;

  target_object object
    = stack ? TARGET_OBJECT_STACK_MEMORY : TARGET_OBJECT_MEMORY;

  ULONGEST xfered_total = 0;

  while (xfered_total < length)
    {
      ULONGEST xfered_partial;

      enum target_xfer_status status
	= target_xfer_partial (current_inferior ()->top_target (),
			       object, NULL,
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
	  *unavailable = 1;
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
		  size_t length, bool stack, int *unavailable)
{
  xfer_memory (address, buffer, nullptr, length, stack, unavailable);
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
  xfer_memory (address, nullptr, buffer, length, stack, unavailable);

  gdb::observers::memory_changed.notify (current_inferior (), address,
					 length, buffer);
}

type *
address_type (gdbarch *arch, int addr_size)
{
  dwarf_gdbarch_types *types
    = (dwarf_gdbarch_types *) gdbarch_data (arch, dwarf_arch_cookie);
  int ndx;

  if (addr_size == 2)
    ndx = 0;
  else if (addr_size == 4)
    ndx = 1;
  else if (addr_size == 8)
    ndx = 2;
  else
    error (_("Unsupported address size in DWARF expressions: %d bits"),
	   HOST_CHAR_BIT * addr_size);

  if (types->dw_types[ndx] == nullptr)
    types->dw_types[ndx]
      = arch_integer_type (arch, HOST_CHAR_BIT * addr_size,
			   0, "<signed DWARF address type>");

  return types->dw_types[ndx];
}

class dwarf_location;
class dwarf_value;

/* Closure callback functions.  */

static void *
copy_value_closure (const value *v);

static void
free_value_closure (value *v);

static void
rw_closure_value (value *v, value *from);

static int
check_synthetic_pointer (const value *value, LONGEST bit_offset,
			 int bit_length);

static void
write_closure_value (value *to, value *from);

static void
read_closure_value (value *v);

static bool
is_optimized_out_closure_value (value *v);

static value *
indirect_closure_value (value *value);

static value *
coerce_closure_ref (const value *value);

/* Functions for accessing a variable described by DW_OP_piece,
   DW_OP_bit_piece or DW_OP_implicit_pointer.  */

static const lval_funcs closure_value_funcs = {
  read_closure_value,
  write_closure_value,
  is_optimized_out_closure_value,
  indirect_closure_value,
  coerce_closure_ref,
  check_synthetic_pointer,
  copy_value_closure,
  free_value_closure
};

/* Closure class that encapsulates a DWARF location description and a
   frame information used when that location description was created.
   Used for lval_computed value abstraction.  */

class computed_closure : public refcounted_object
{
public:
  computed_closure (std::unique_ptr<dwarf_location> location,
		    struct frame_id frame_id)
    : m_location (std::move (location)), m_frame_id (frame_id)
  {}

  computed_closure (std::unique_ptr<dwarf_location> location,
		    struct frame_info *frame)
    : m_location (std::move (location)), m_frame (frame)
  {}

  const dwarf_location &get_location () const
  {
    return *m_location;
  }

  frame_id get_frame_id () const
  {
    gdb_assert (m_frame == nullptr);
    return m_frame_id;
  }

  frame_info *get_frame () const
  {
    return m_frame;
  }

private:
  /* Entry that this class encloses.  */
  const std::unique_ptr<const dwarf_location> m_location;

  /* Frame ID context of the closure.  */
  frame_id m_frame_id;

  /* In the case of frame expression evaluator the frame_id
     is not safe to use because the frame itself is being built.
     Only in these cases we set and use frame info directly.  */
  frame_info *m_frame = nullptr;
};

/* Base class that describes entries found on a DWARF expression
   evaluation stack.  */

class dwarf_entry
{
protected:
  /* Not expected to be called on it's own.  */
  dwarf_entry () = default;

public:
  virtual ~dwarf_entry () = default;

  virtual std::unique_ptr<dwarf_entry> clone () const = 0;
};

using dwarf_entry_up = std::unique_ptr<dwarf_entry>;

/* Location description entry found on a DWARF expression evaluation
   stack.

   Types of locations descirbed can be: register location, memory
   location, implicit location, implicit pointer location, undefined
   location and composite location (composed out of any of the location
   types including another composite location).  */

class dwarf_location : public dwarf_entry
{
protected:
  /* Not expected to be called on it's own.  */
  dwarf_location (gdbarch *arch, LONGEST offset)
    : m_arch (arch), m_offset (offset)
  {}

public:
  virtual ~dwarf_location () = default;

  dwarf_entry_up clone () const override final
  {
    return this->clone_location ();
  }

  /* Clone the location and return the result as a
     dwarf_location pointer.  */
  virtual std::unique_ptr<dwarf_location> clone_location () const = 0;

  /* Add bit offset to the location description.  */
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

  /* Convert DWARF entry into a DWARF value.  TYPE defines a desired type of
     the returned DWARF value if it doesn't already have one.

     If the conversion from that location description kind to a value is not
     supported, throw an error.  */
  virtual std::unique_ptr<dwarf_value> to_value (struct type *type) const
  {
    ill_formed_expression ();
  }

  /* Read contents from the described location.

     The read operation is performed in the context of a FRAME.
     BIT_SIZE is the number of bits to read.  The data read is copied
     to the caller-managed buffer BUF.  BIG_ENDIAN defines the
     endianness of the target.  BITS_TO_SKIP is a bit offset into the
     location and BUF_BIT_OFFSET is buffer BUF's bit offset.
     LOCATION_BIT_LIMIT is a maximum number of bits that location can
     hold, where value zero signifies that there is no such
     restriction.

     Note that some location types can be read without a FRAME context.

     If the location is optimized out or unavailable, the OPTIMIZED and
     UNAVAILABLE outputs are set accordingly.  */
  virtual void read (frame_info *frame, gdb_byte *buf,
		     int buf_bit_offset, size_t bit_size,
		     LONGEST bits_to_skip, size_t location_bit_limit,
		     bool big_endian, int *optimized,
		     int *unavailable) const = 0;

  /* Write contents to a described location.

     The write operation is performed in the context of a FRAME.
     BIT_SIZE is the number of bits written.  The data written is
     copied from the caller-managed BUF buffer.  BIG_ENDIAN defines an
     endianness of the target.  BITS_TO_SKIP is a bit offset into the
     location and BUF_BIT_OFFSET is buffer BUF's bit offset.
     LOCATION_BIT_LIMIT is a maximum number of bits that location can
     hold, where value zero signifies that there is no such
     restriction.

     Note that some location types can be written without a FRAME
     context.

     If the location is optimized out or unavailable, the OPTIMIZED and
     UNAVAILABLE outputs are set.  */
  virtual void write (frame_info *frame, const gdb_byte *buf,
		      int buf_bit_offset, size_t bit_size,
		      LONGEST bits_to_skip, size_t location_bit_limit,
		      bool big_endian, int *optimized,
		      int *unavailable) const = 0;

  /* Apply dereference operation on the DWARF location description.
     Operation returns a DWARF value of a given TYPE type while FRAME
     contains a frame context information of the location.  ADDR_INFO
     (if present) describes a passed in memory buffer if a regular
     memory read is not desired for certain address range.  If the SIZE
     is specified, it must be equal or smaller than the TYPE type size.
     If SIZE is smaller than the type size, the value will be zero
     extended to the difference.  */
  virtual std::unique_ptr<dwarf_value> deref
    (frame_info *frame, const property_addr_info *addr_info,
     struct type *type, size_t size = 0) const;

/* Read data from the VALUE contents to the location specified by the
   location description.

   The read operation is performed in the context of a FRAME.  BIT_SIZE
   is the number of bits to read.  VALUE_BIT_OFFSET is a bit offset
   into a VALUE content and BITS_TO_SKIP is a bit offset into the
   location.  LOCATION_BIT_LIMIT is a maximum number of bits that
   location can hold, where value zero signifies that there is no such
   restriction.

   Note that some location types can be read without a FRAME context.  */
  virtual void read_from_gdb_value (frame_info *frame, struct value *value,
				    int value_bit_offset,
				    LONGEST bits_to_skip, size_t bit_size,
				    size_t location_bit_limit) const;

/* Write data to the VALUE contents from the location specified by the
   location description.

   The write operation is performed in the context of a FRAME.
   BIT_SIZE is the number of bits to read.  VALUE_BIT_OFFSET is a bit
   offset into a VALUE content and BITS_TO_SKIP is a bit offset into
   the location.  LOCATION_BIT_LIMIT is a maximum number of bits that
   location can hold, where value zero signifies that there is no such
   restriction.

   Note that some location types can be read without a FRAME context.  */
  virtual void write_to_gdb_value (frame_info *frame, struct value *value,
				   int value_bit_offset,
				   LONGEST bits_to_skip, size_t bit_size,
				   size_t location_bit_limit) const;

  /* Check if a given DWARF location description contains an implicit
     pointer location description of a BIT_LENGTH size on a given
     BIT_OFFSET offset.  */
  virtual bool is_implicit_ptr_at (LONGEST bit_offset, int bit_length) const
  {
     return false;
  }

  /* Recursive indirecting of the implicit pointer location description
     if that location is or encapsulates an implicit pointer.  The
     operation is performed in a given FRAME context, using the TYPE as
     the type of the pointer.  Where POINTER_OFFSET is an offset
     applied to that implicit pointer location description before the
     operation.  BIT_OFFSET is a bit offset applied to the location and
     BIT_LENGTH is a bit length of the read.

     Indirecting is only performed on the implicit pointer location
     description parts of the location.  */
  virtual value *indirect_implicit_ptr (frame_info *frame, struct type *type,
					LONGEST pointer_offset = 0,
					LONGEST bit_offset = 0,
					int bit_length = 0) const
  {
    return nullptr;
  }

  /* Check if location description resolves into optimized out.

     The check operation is performed in the context of a FRAME.
     BIG_ENDIAN defines the endianness of the target, BIT_SIZE is the
     number of bits to read and BITS_TO_SKIP is a bit offset into the
     location.  LOCATION_BIT_LIMIT is a maximum number of bits that
     location can hold, where value zero signifies that there is
     no such restriction.  */
  virtual bool is_optimized_out (frame_info *frame, bool big_endian,
				 LONGEST bits_to_skip, size_t bit_size,
				 size_t location_bit_limit) const
  {
    return false;
  }

  /* Convert DWARF location description to the matching struct value
     representation of the given TYPE type in a given FRAME.
     SUBOBJ_TYPE information if specified, will be used for more
     precise description of the source variable type information.
     Where SUBOBJ_OFFSET defines an offset into the DWARF entry
     contents.  */
  virtual value *to_gdb_value (frame_info *frame, struct type *type,
			       struct type *subobj_type,
			       LONGEST subobj_offset) const = 0;

protected:
  /* Architecture of the location.  */
  gdbarch *m_arch;

  /* Byte offset into the location.  */
  LONGEST m_offset;

  /* Bit suboffset of the last byte.  */
  LONGEST m_bit_suboffset = 0;

  /* Whether the location is initialized.  Used for non-standard
     DW_OP_GNU_uninit operation.  */
  bool m_initialised = true;
};

using dwarf_location_up = std::unique_ptr<dwarf_location>;

void
dwarf_location::read_from_gdb_value (frame_info *frame, struct value *value,
				     int value_bit_offset,
				     LONGEST bits_to_skip, size_t bit_size,
				     size_t location_bit_limit) const
{
  int optimized, unavailable;
  bool big_endian = type_byte_order (value_type (value)) == BFD_ENDIAN_BIG;

  this->write (frame, value_contents (value).data (), value_bit_offset,
	       bit_size, bits_to_skip, location_bit_limit,
	       big_endian, &optimized, &unavailable);

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

void
dwarf_location::write_to_gdb_value (frame_info *frame, struct value *value,
				    int value_bit_offset,
				    LONGEST bits_to_skip, size_t bit_size,
				    size_t location_bit_limit) const
{
  int optimized, unavailable;
  bool big_endian = type_byte_order (value_type (value)) == BFD_ENDIAN_BIG;

  this->read (frame, value_contents_raw (value).data (), value_bit_offset,
	      bit_size, bits_to_skip, location_bit_limit,
	      big_endian, &optimized, &unavailable);

  if (optimized)
    mark_value_bits_optimized_out (value, value_bit_offset, bit_size);
  if (unavailable)
    mark_value_bits_unavailable (value, value_bit_offset, bit_size);
}

/* Value entry found on a DWARF expression evaluation stack.  */

class dwarf_value final : public dwarf_entry
{
public:
  dwarf_value (gdb::array_view<const gdb_byte> contents, struct type *type)
    : m_contents (contents.begin (), contents.end ()), m_type (type)
  {}

  dwarf_value (ULONGEST value, struct type *type)
    : m_contents (TYPE_LENGTH (type)), m_type (type)
  {
    pack_unsigned_long (m_contents.data (), type, value);
  }

  dwarf_value (LONGEST value, struct type *type)
    : m_contents (TYPE_LENGTH (type)), m_type (type)
  {
    pack_long (m_contents.data (), type, value);
  }

  dwarf_value (value *gdb_value)
  {
    m_type = value_type (gdb_value);
    gdb::array_view<const gdb_byte> contents = value_contents_raw (gdb_value);
    m_contents
      = std::move (gdb::byte_vector (contents.begin (), contents.end ()));
    m_gdb_value = gdb_value;
  }

  dwarf_entry_up clone () const override
  {
    return make_unique<dwarf_value> (*this);
  }

  gdb::array_view<const gdb_byte> contents () const
  {
    return m_contents;
  }

  struct type *type ()
  {
    return m_type;
  }

  const struct type *type () const
  {
    return m_type;
  }

  LONGEST to_long () const
  {
    return unpack_long (m_type, m_contents.data ());
  }

  /* Convert DWARF value into a DWARF memory location description.
     ARCH defines an architecture of the location described.  */
  dwarf_location_up to_location (struct gdbarch *arch) const;

  /* Convert DWARF value to the matching struct value representation
     of the given TYPE type.  Where OFFSET defines an offset into the
     DWARF value contents.  */
  value *to_gdb_value (struct type *type, LONGEST offset = 0);

private:
  /* Value contents as a stream of bytes in target byte order.  */
  gdb::byte_vector m_contents;

  /* Type of the value held by the entry.  */
  struct type *m_type;

  /* Struct value representation of the DWARF value.   Only used until
     a set of arithmethic/logic operations that works with this class
     are implemented.  */
  value *m_gdb_value = nullptr;
};

using dwarf_value_up = std::unique_ptr<dwarf_value>;

std::unique_ptr<dwarf_value>
dwarf_location::deref (frame_info *frame, const property_addr_info *addr_info,
		       struct type *type, size_t size) const
{
  bool big_endian = type_byte_order (type) == BFD_ENDIAN_BIG;
  size_t actual_size = size != 0 ? size : TYPE_LENGTH (type);

  if (actual_size > TYPE_LENGTH (type))
    ill_formed_expression ();

  /* If the size of the object read from memory is different
     from the type length, we need to zero-extend it.  */
  gdb::byte_vector read_buf (TYPE_LENGTH (type), 0);
  gdb_byte *buf_ptr = read_buf.data ();
  int optimized, unavailable;

  if (big_endian)
    buf_ptr += TYPE_LENGTH (type) - actual_size;

  this->read (frame, buf_ptr, 0, actual_size * HOST_CHAR_BIT,
	      0, 0, big_endian, &optimized, &unavailable);

  if (optimized)
    throw_error (OPTIMIZED_OUT_ERROR,
		 _("Can't dereference "
		   "update bitfield; containing word "
		   "has been optimized out"));
  if (unavailable)
    throw_error (NOT_AVAILABLE_ERROR,
		 _("Can't dereference "
		   "update bitfield; containing word "
		   "is unavailable"));

  return make_unique<dwarf_value>
    (gdb::array_view<const gdb_byte> (read_buf), type);
}

value *
dwarf_value::to_gdb_value (struct type *type, LONGEST offset)
{
  if (m_gdb_value != nullptr)
    return m_gdb_value;

  size_t type_len = TYPE_LENGTH (type);

  if (offset + type_len > TYPE_LENGTH (m_type))
    invalid_synthetic_pointer ();

  m_gdb_value = allocate_value (type);
  memcpy (value_contents_raw (m_gdb_value).data (),
	  m_contents.data () + offset, type_len);
  return m_gdb_value;
}

/* Undefined location description entry.  This is a special location
   description type that describes the location description that is
   not known.  */

class dwarf_undefined final : public dwarf_location
{
public:
  dwarf_undefined (gdbarch *arch)
    : dwarf_location (arch, 0)
  {}

  dwarf_location_up clone_location () const override
  {
    return make_unique<dwarf_undefined> (*this);
  }

  void read (frame_info *frame, gdb_byte *buf, int buf_bit_offset,
	     size_t bit_size, LONGEST bits_to_skip, size_t location_bit_limit,
	     bool big_endian, int *optimized, int *unavailable) const override
  {
    *unavailable = 0;
    *optimized = 1;
  }

  void write (frame_info *frame, const gdb_byte *buf, int buf_bit_offset,
	      size_t bit_size, LONGEST bits_to_skip, size_t location_bit_limit,
	      bool big_endian, int *optimized, int *unavailable) const override
  {
    *unavailable = 0;
    *optimized = 1;
  }

  bool is_optimized_out (frame_info *frame, bool big_endian,
			 LONGEST bits_to_skip, size_t bit_size,
			 size_t location_bit_limit) const override
  {
    return true;
  }

  value *to_gdb_value (frame_info *frame, struct type *type,
		       struct type *subobj_type,
		       LONGEST subobj_offset) const override
  {
    gdb_assert (type != nullptr);
    gdb_assert (subobj_type != nullptr);

    value *retval = allocate_value (subobj_type);
    mark_value_bytes_optimized_out (retval, subobj_offset,
				    TYPE_LENGTH (subobj_type));
    return retval;
  }
};

class dwarf_memory final : public dwarf_location
{
public:
  dwarf_memory (gdbarch *arch, LONGEST offset, bool stack = false)
    : dwarf_location (arch, offset), m_stack (stack)
  {}

  dwarf_location_up clone_location () const override
  {
    return make_unique<dwarf_memory> (*this);
  }

  void set_stack (bool stack)
  {
    m_stack = stack;
  };

  dwarf_value_up to_value (struct type *type) const override;

  void read (frame_info *frame, gdb_byte *buf, int buf_bit_offset,
	     size_t bit_size, LONGEST bits_to_skip,
	     size_t location_bit_limit, bool big_endian,
	     int *optimized, int *unavailable) const override;

  void write (frame_info *frame, const gdb_byte *buf,
	      int buf_bit_offset, size_t bit_size, LONGEST bits_to_skip,
	      size_t location_bit_limit, bool big_endian,
	      int *optimized, int *unavailable) const override;

  std::unique_ptr<dwarf_value> deref (frame_info *frame,
				      const property_addr_info *addr_info,
				      struct type *type,
				      size_t size = 0) const override;

  value *to_gdb_value (frame_info *frame, struct type *type,
		       struct type *subobj_type,
		       LONGEST subobj_offset) const override;

private:
  /* True if the location belongs to a stack memory region.  */
  bool m_stack;
};

dwarf_location_up
dwarf_value::to_location (struct gdbarch *arch) const
{
  LONGEST offset;

  if (gdbarch_integer_to_address_p (arch))
    offset = gdbarch_integer_to_address (arch, m_type, m_contents.data ());
  else
    offset = extract_unsigned_integer (m_contents.data (),
				       TYPE_LENGTH (m_type),
				       type_byte_order (m_type));

  return make_unique<dwarf_memory> (arch, offset);
}

dwarf_value_up
dwarf_memory::to_value (struct type *type) const
{
  return make_unique<dwarf_value> (m_offset, type);
}

void
dwarf_memory::read (frame_info *frame, gdb_byte *buf,
		    int buf_bit_offset, size_t bit_size,
		    LONGEST bits_to_skip, size_t location_bit_limit,
		    bool big_endian, int *optimized, int *unavailable) const
{
  LONGEST total_bits_to_skip = bits_to_skip;
  CORE_ADDR start_address
    = m_offset + (m_bit_suboffset + total_bits_to_skip) / HOST_CHAR_BIT;
  gdb::byte_vector temp_buf;

  *optimized = 0;
  total_bits_to_skip += m_bit_suboffset;

  if (total_bits_to_skip % HOST_CHAR_BIT == 0
      && bit_size % HOST_CHAR_BIT == 0
      && buf_bit_offset % HOST_CHAR_BIT == 0)
    {
      /* Everything is byte-aligned, no buffer needed.  */
      read_from_memory (start_address,
			buf + buf_bit_offset / HOST_CHAR_BIT,
			bit_size / HOST_CHAR_BIT, m_stack, unavailable);
    }
  else
    {
      LONGEST this_size = bits_to_bytes (total_bits_to_skip, bit_size);
      temp_buf.resize (this_size);

      /* Can only read from memory on byte granularity so an
	 additional buffer is required.  */
      read_from_memory (start_address, temp_buf.data (), this_size,
			m_stack, unavailable);

      if (!*unavailable)
	copy_bitwise (buf, buf_bit_offset, temp_buf.data (),
		      total_bits_to_skip % HOST_CHAR_BIT,
		      bit_size, big_endian);
    }
}

void
dwarf_memory::write (frame_info *frame, const gdb_byte *buf,
		     int buf_bit_offset, size_t bit_size,
		     LONGEST bits_to_skip, size_t location_bit_limit,
		     bool big_endian, int *optimized, int *unavailable) const
{
  LONGEST total_bits_to_skip = bits_to_skip;
  CORE_ADDR start_address
    = m_offset + (m_bit_suboffset + total_bits_to_skip) / HOST_CHAR_BIT;
  gdb::byte_vector temp_buf;

  *optimized = 0;
  total_bits_to_skip += m_bit_suboffset;

  if (total_bits_to_skip % HOST_CHAR_BIT == 0
      && bit_size % HOST_CHAR_BIT == 0
      && buf_bit_offset % HOST_CHAR_BIT == 0)
    {
      /* Everything is byte-aligned; no buffer needed.  */
      write_to_memory (start_address, buf + buf_bit_offset / HOST_CHAR_BIT,
		       bit_size / HOST_CHAR_BIT, m_stack, unavailable);
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
			      this_size, m_stack, unavailable);
	  else
	    {
	      /* Only the first and last bytes can possibly have
		 any bits reused.  */
	      read_from_memory (start_address, temp_buf.data (),
				1, m_stack, unavailable);

	      if (!*unavailable)
		read_from_memory (start_address + this_size - 1,
				  &temp_buf[this_size - 1], 1,
				  m_stack, unavailable);
	    }
	}

      copy_bitwise (temp_buf.data (), total_bits_to_skip % HOST_CHAR_BIT,
		    buf, buf_bit_offset, bit_size, big_endian);

      write_to_memory (start_address, temp_buf.data (), this_size,
		       m_stack, unavailable);
    }
}

std::unique_ptr<dwarf_value>
dwarf_memory::deref (frame_info *frame, const property_addr_info *addr_info,
		     struct type *type, size_t size) const
{
  bool big_endian = type_byte_order (type) == BFD_ENDIAN_BIG;
  size_t actual_size = size != 0 ? size : TYPE_LENGTH (type);

  if (actual_size > TYPE_LENGTH (type))
    ill_formed_expression ();

  gdb::byte_vector read_buf (TYPE_LENGTH (type), 0);
  size_t size_in_bits = actual_size * HOST_CHAR_BIT;
  gdb_byte *buf_ptr = read_buf.data ();
  bool passed_in_buf = false;

  if (big_endian)
    buf_ptr += TYPE_LENGTH (type) - actual_size;

  /* Covers the case where we have a passed in memory that is not
     part of the target and requires for the location description
     to address it instead of addressing the actual target
     memory.  */
  LONGEST this_size = bits_to_bytes (m_bit_suboffset, size_in_bits);

  /* We shouldn't have a case where we read from a passed in
     memory and the same memory being marked as stack. */
  if (!m_stack && this_size && addr_info != nullptr
      && addr_info->valaddr.data () != nullptr)
    {
      CORE_ADDR offset = (CORE_ADDR) m_offset - addr_info->addr;

      if (offset < addr_info->valaddr.size ()
	  && offset + this_size <= addr_info->valaddr.size ())
	{
	  /* Using second buffer here because the copy_bitwise
	     doesn't support in place copy.  */
	  gdb::byte_vector temp_buf (this_size);

	  memcpy (temp_buf.data (), addr_info->valaddr.data () + offset,
		  this_size);
	  copy_bitwise (buf_ptr, 0, temp_buf.data (),
			m_bit_suboffset, size_in_bits, big_endian);
	  passed_in_buf = true;
	}
    }

  if (!passed_in_buf)
    {
      int optimized, unavailable;

      this->read (frame, buf_ptr, 0, size_in_bits, 0, 0,
		  big_endian, &optimized, &unavailable);

      if (optimized)
	throw_error (OPTIMIZED_OUT_ERROR,
		     _("Can't dereference "
		     "update bitfield; containing word "
		     "has been optimized out"));
      if (unavailable)
	throw_error (NOT_AVAILABLE_ERROR,
		     _("Can't dereference "
		     "update bitfield; containing word "
		     "is unavailable"));
    }

  return make_unique<dwarf_value>
    (gdb::array_view<const gdb_byte> (read_buf), type);
}

value *
dwarf_memory::to_gdb_value (frame_info *frame, struct type *type,
			    struct type *subobj_type,
			    LONGEST subobj_offset) const
{
  gdb_assert (type != nullptr);
  gdb_assert (subobj_type != nullptr);

  struct type *ptr_type = builtin_type (m_arch)->builtin_data_ptr;

  if (subobj_type->code () == TYPE_CODE_FUNC
      || subobj_type->code () == TYPE_CODE_METHOD)
    ptr_type = builtin_type (m_arch)->builtin_func_ptr;

  CORE_ADDR address
    = value_as_address (value_from_pointer (ptr_type, m_offset));
  value *retval = value_at_lazy (subobj_type, address + subobj_offset);
  set_value_stack (retval, m_stack);
  set_value_bitpos (retval, m_bit_suboffset);
  return retval;
}

/* Register location description entry.  */

class dwarf_register final : public dwarf_location
{
public:
  dwarf_register (gdbarch *arch, unsigned int regnum, LONGEST offset = 0)
    : dwarf_location (arch, offset), m_regnum (regnum)
  {}

  dwarf_location_up clone_location () const override
  {
    return make_unique<dwarf_register> (*this);
  }

  void read (frame_info *frame, gdb_byte *buf, int buf_bit_offset,
	     size_t bit_size, LONGEST bits_to_skip, size_t location_bit_limit,
	     bool big_endian, int *optimized, int *unavailable) const override;

  void write (frame_info *frame, const gdb_byte *buf,
	      int buf_bit_offset, size_t bit_size, LONGEST bits_to_skip,
	      size_t location_bit_limit, bool big_endian,
	      int *optimized, int *unavailable) const override;

  bool is_optimized_out (frame_info *frame, bool big_endian,
			 LONGEST bits_to_skip, size_t bit_size,
			 size_t location_bit_limit) const override;

  value *to_gdb_value (frame_info *frame, struct type *type,
		       struct type *subobj_type,
		       LONGEST subobj_offset) const override;


private:
  /* DWARF register number.  */
  unsigned int m_regnum;
};

void
dwarf_register::read (frame_info *frame, gdb_byte *buf,
		      int buf_bit_offset, size_t bit_size,
		      LONGEST bits_to_skip, size_t location_bit_limit,
		      bool big_endian, int *optimized, int *unavailable) const
{
  LONGEST total_bits_to_skip = bits_to_skip;
  size_t read_bit_limit = location_bit_limit;
  gdbarch *frame_arch = get_frame_arch (frame);
  int reg = dwarf_reg_to_regnum_or_error (frame_arch, m_regnum);
  ULONGEST reg_bits = HOST_CHAR_BIT * register_size (frame_arch, reg);
  gdb::byte_vector temp_buf;

  if (big_endian)
    {
      if (!read_bit_limit || reg_bits <= read_bit_limit)
	read_bit_limit = bit_size;

      total_bits_to_skip += reg_bits - (m_offset * HOST_CHAR_BIT
					+ m_bit_suboffset + read_bit_limit);
    }
  else
    total_bits_to_skip += m_offset * HOST_CHAR_BIT + m_bit_suboffset;

  LONGEST this_size = bits_to_bytes (total_bits_to_skip, bit_size);
  temp_buf.resize (this_size);

  if (frame == nullptr)
    internal_error (__FILE__, __LINE__, _("invalid frame information"));

  /* Can only read from a register on byte granularity so an
     additional buffer is required.  */
  read_from_register (frame, reg, total_bits_to_skip / HOST_CHAR_BIT,
		      temp_buf, optimized, unavailable);

  /* Only copy data if valid.  */
  if (!*optimized && !*unavailable)
    copy_bitwise (buf, buf_bit_offset, temp_buf.data (),
		  total_bits_to_skip % HOST_CHAR_BIT, bit_size, big_endian);
}

void
dwarf_register::write (frame_info *frame, const gdb_byte *buf,
		       int buf_bit_offset, size_t bit_size,
		       LONGEST bits_to_skip, size_t location_bit_limit,
		       bool big_endian, int *optimized, int *unavailable) const
{
  LONGEST total_bits_to_skip = bits_to_skip;
  size_t write_bit_limit = location_bit_limit;
  gdbarch *frame_arch = get_frame_arch (frame);
  int reg = dwarf_reg_to_regnum_or_error (frame_arch, m_regnum);
  ULONGEST reg_bits = HOST_CHAR_BIT * register_size (frame_arch, reg);
  gdb::byte_vector temp_buf;

  if (frame == nullptr)
    internal_error (__FILE__, __LINE__, _("invalid frame information"));

  if (big_endian)
    {
      if (!write_bit_limit || reg_bits <= write_bit_limit)
	write_bit_limit = bit_size;

      total_bits_to_skip += reg_bits - (m_offset * HOST_CHAR_BIT
					+ m_bit_suboffset + write_bit_limit);
    }
  else
    total_bits_to_skip += m_offset * HOST_CHAR_BIT + m_bit_suboffset;

  LONGEST this_size = bits_to_bytes (total_bits_to_skip, bit_size);
  temp_buf.resize (this_size);

  if (total_bits_to_skip % HOST_CHAR_BIT != 0
      || bit_size % HOST_CHAR_BIT != 0)
    {
      /* Contents is copied non-byte-aligned into the register.
         Need some bits from original register value.  */
      read_from_register (frame, reg,
			  total_bits_to_skip / HOST_CHAR_BIT,
			  temp_buf, optimized, unavailable);
    }

  copy_bitwise (temp_buf.data (), total_bits_to_skip % HOST_CHAR_BIT, buf,
		buf_bit_offset, bit_size, big_endian);

  write_to_register (frame, reg, total_bits_to_skip / HOST_CHAR_BIT,
		     temp_buf, optimized, unavailable);
}

bool
dwarf_register::is_optimized_out (frame_info *frame, bool big_endian,
				  LONGEST bits_to_skip, size_t bit_size,
				  size_t location_bit_limit) const
{
  int optimized, unavailable;
  gdb::byte_vector temp_buf (bit_size);

  this->read (frame, temp_buf.data (), 0, bit_size,
	      bits_to_skip, location_bit_limit,
	      big_endian, &optimized, &unavailable);

  if (optimized)
    return true;

  return false;
}

value *
dwarf_register::to_gdb_value (frame_info *frame, struct type *type,
			      struct type *subobj_type,
			      LONGEST subobj_offset) const
{
  gdb_assert (type != nullptr);
  gdb_assert (subobj_type != nullptr);

  gdbarch *frame_arch = get_frame_arch (frame);
  int gdb_regnum = dwarf_reg_to_regnum_or_error (frame_arch, m_regnum);

  if (frame == nullptr)
    internal_error (__FILE__, __LINE__, _("invalid frame information"));

  /* Construct the value.  */
  value *retval
    = gdbarch_value_from_register (frame_arch, type,
				   gdb_regnum, get_frame_id (frame));

  /* DWARF evaluator only supports targets with byte size of 8 bits,
     while struct value offset is expressed in memory unit size.  */
  int unit_size = gdbarch_addressable_memory_unit_size (m_arch);
  LONGEST retval_offset = value_offset (retval) * unit_size;

  if (type_byte_order (type) == BFD_ENDIAN_BIG
      && TYPE_LENGTH (type) + m_offset < retval_offset)
    /* Big-endian, and we want less than full size.  */
    set_value_offset (retval, (retval_offset - m_offset) / unit_size);
  else
    set_value_offset (retval, (retval_offset + m_offset) / unit_size);

  set_value_bitpos (retval,
		    m_bit_suboffset + (m_offset % unit_size) * HOST_CHAR_BIT);

  /* Get the data.  */
  read_frame_register_value (retval, frame);

  if (value_optimized_out (retval))
    {
      /* This means the register has undefined value / was not saved.
	 As we're computing the location of some variable etc. in the
	 program, not a value for inspecting a register ($pc, $sp, etc.),
	 return a generic optimized out value instead, so that we show
	 <optimized out> instead of <not saved>.  */
      value *temp = allocate_value (subobj_type);
      value_contents_copy (temp, 0, retval, 0, 0, TYPE_LENGTH (subobj_type));
      retval = temp;
    }

  return retval;
}

/* Implicit location description entry.  Describes a location
   description not found on the target but instead saved in a
   gdb-allocated buffer.  */

class dwarf_implicit final : public dwarf_location
{
public:

  dwarf_implicit (gdbarch *arch, gdb::array_view<const gdb_byte> contents,
		  enum bfd_endian byte_order)
    : dwarf_location (arch, 0),
      m_contents (contents.begin (), contents.end ()),
      m_byte_order (byte_order)
  {}

  dwarf_location_up clone_location () const override
  {
    return make_unique<dwarf_implicit> (*this);
  }

  void read (frame_info *frame, gdb_byte *buf, int buf_bit_offset,
	     size_t bit_size, LONGEST bits_to_skip, size_t location_bit_limit,
	     bool big_endian, int *optimized, int *unavailable) const override;

  void write (frame_info *frame, const gdb_byte *buf,
	      int buf_bit_offset, size_t bit_size,
	      LONGEST bits_to_skip, size_t location_bit_limit,
	      bool big_endian, int* optimized, int* unavailable) const override
  {
    *optimized = 1;
    *unavailable = 0;
  }

  bool is_optimized_out (frame_info *frame, bool big_endian,
			 LONGEST bits_to_skip, size_t bit_size,
			 size_t location_bit_limit) const override
  {
    return true;
  }

  value *to_gdb_value (frame_info *frame, struct type *type,
		       struct type *subobj_type,
		       LONGEST subobj_offset) const override;

private:
  /* Implicit location contents as a stream of bytes in target byte-order.  */
  gdb::byte_vector m_contents;

  /* Contents original byte order.  */
  bfd_endian m_byte_order;
};

void
dwarf_implicit::read (frame_info *frame, gdb_byte *buf,
		      int buf_bit_offset, size_t bit_size,
		      LONGEST bits_to_skip, size_t location_bit_limit,
		      bool big_endian, int *optimized, int *unavailable) const
{
  ULONGEST implicit_bit_size = HOST_CHAR_BIT * m_contents.size ();
  LONGEST total_bits_to_skip = bits_to_skip;
  size_t read_bit_limit = location_bit_limit;

  *optimized = 0;
  *unavailable = 0;

  /* Cut off at the end of the implicit value.  */
  if (m_byte_order == BFD_ENDIAN_BIG)
    {
      if (!read_bit_limit || read_bit_limit > implicit_bit_size)
	read_bit_limit = bit_size;

      total_bits_to_skip
	+= implicit_bit_size - (m_offset * HOST_CHAR_BIT
			       + m_bit_suboffset + read_bit_limit);
    }
  else
    total_bits_to_skip += m_offset * HOST_CHAR_BIT + m_bit_suboffset;

  if (total_bits_to_skip >= implicit_bit_size)
    {
      *unavailable = 1;
      return;
    }

  if (bit_size > implicit_bit_size - total_bits_to_skip)
    bit_size = implicit_bit_size - total_bits_to_skip;

  copy_bitwise (buf, buf_bit_offset, m_contents.data (),
		total_bits_to_skip, bit_size, big_endian);
}

value *
dwarf_implicit::to_gdb_value (frame_info *frame, struct type *type,
			      struct type *subobj_type,
			      LONGEST subobj_offset) const
{
  gdb_assert (type != nullptr);
  gdb_assert (subobj_type != nullptr);

  size_t subtype_len = TYPE_LENGTH (subobj_type);
  size_t type_len = TYPE_LENGTH (type);

  /* To be compatible with expected error output of the existing
     tests, the invalid synthetic pointer is not reported for
     DW_OP_implicit_value operation.  */
  if (subobj_offset + subtype_len > type_len
      && m_byte_order != BFD_ENDIAN_UNKNOWN)
    invalid_synthetic_pointer ();

  value *retval = allocate_value (subobj_type);

  /* The given offset is relative to the actual object.  */
  if (m_byte_order == BFD_ENDIAN_BIG)
    subobj_offset += m_contents.size () - type_len;

  memcpy ((void *) value_contents_raw (retval).data (),
	  (void *) (m_contents.data () + subobj_offset), subtype_len);

  return retval;
}

/* Implicit pointer location description entry.  */

class dwarf_implicit_pointer final : public dwarf_location
{
public:
  dwarf_implicit_pointer (gdbarch *arch,
			  dwarf2_per_objfile *per_objfile,
			  dwarf2_per_cu_data *per_cu,
			  int addr_size, sect_offset die_offset,
			  LONGEST offset)
    : dwarf_location (arch, offset),
      m_per_objfile (per_objfile), m_per_cu (per_cu),
      m_addr_size (addr_size), m_die_offset (die_offset)
  {}

  dwarf_location_up clone_location () const override
  {
    return make_unique<dwarf_implicit_pointer> (*this);
  }

  void read (frame_info *frame, gdb_byte *buf, int buf_bit_offset,
	     size_t bit_size, LONGEST bits_to_skip, size_t location_bit_limit,
	     bool big_endian, int *optimized, int *unavailable) const override;

  void write (frame_info *frame, const gdb_byte *buf,
	      int buf_bit_offset, size_t bit_size, LONGEST bits_to_skip,
	      size_t location_bit_limit, bool big_endian,
	      int* optimized, int* unavailable) const override
  {
    *optimized = 1;
    *unavailable = 0;
  }

  /* Reading from and writing to an implicit pointer is not meaningful,
     so we just skip them here.  */
  void read_from_gdb_value (frame_info *frame, struct value *value,
			    int value_bit_offset,
			    LONGEST bits_to_skip, size_t bit_size,
			    size_t location_bit_limit) const override
  {
    mark_value_bits_optimized_out (value, bits_to_skip, bit_size);
  }

  void write_to_gdb_value (frame_info *frame, struct value *value,
			   int value_bit_offset,
			   LONGEST bits_to_skip, size_t bit_size,
			   size_t location_bit_limit) const override
  {}

  bool is_implicit_ptr_at (LONGEST bit_offset, int bit_length) const override
  {
     return true;
  }

  value *indirect_implicit_ptr (frame_info *frame, struct type *type,
				LONGEST pointer_offset = 0,
				LONGEST bit_offset = 0,
				int bit_length = 0) const override;

  value *to_gdb_value (frame_info *frame, struct type *type,
		       struct type *subobj_type,
		       LONGEST subobj_offset) const override;

private:
  /* Per object file data of the implicit pointer.  */
  dwarf2_per_objfile *m_per_objfile;

  /* Compilation unit context of the implicit pointer.  */
  dwarf2_per_cu_data *m_per_cu;

  /* Address size for the evaluation.  */
  int m_addr_size;

  /* DWARF die offset pointed by the implicit pointer.  */
  sect_offset m_die_offset;
};

void
dwarf_implicit_pointer::read (frame_info *frame, gdb_byte *buf,
			      int buf_bit_offset, size_t bit_size,
                              LONGEST bits_to_skip, size_t location_bit_limit,
			      bool big_endian, int *optimized,
			      int *unavailable) const
{
  frame_info *actual_frame = frame;
  LONGEST total_bits_to_skip = bits_to_skip + m_bit_suboffset;

  if (actual_frame == nullptr)
    actual_frame = get_selected_frame (_("No frame selected."));

  struct type *type
    = address_type (get_frame_arch (actual_frame), m_addr_size);

  struct value *value
    = indirect_synthetic_pointer (m_die_offset, m_offset, m_per_cu,
				  m_per_objfile, actual_frame, type);

  gdb_byte *value_contents = value_contents_raw (value).data ()
			     + total_bits_to_skip / HOST_CHAR_BIT;

  if (total_bits_to_skip % HOST_CHAR_BIT == 0
      && bit_size % HOST_CHAR_BIT == 0
      && buf_bit_offset % HOST_CHAR_BIT == 0)
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

value *
dwarf_implicit_pointer::indirect_implicit_ptr (frame_info *frame,
					       struct type *type,
					       LONGEST pointer_offset,
					       LONGEST bit_offset,
					       int bit_length) const
{
  return indirect_synthetic_pointer (m_die_offset, m_offset + pointer_offset,
				     m_per_cu, m_per_objfile, frame, type);
}

value *
dwarf_implicit_pointer::to_gdb_value (frame_info *frame, struct type *type,
				      struct type *subobj_type,
				      LONGEST subobj_offset) const
{
  gdb_assert (type != nullptr);
  gdb_assert (subobj_type != nullptr);

  computed_closure *closure
    = new computed_closure (make_unique<dwarf_implicit_pointer> (*this),
			    get_frame_id (frame));
  closure->incref ();

  value *retval
    = allocate_computed_value (subobj_type, &closure_value_funcs, closure);
  set_value_offset (retval, subobj_offset);

  return retval;
}

/* Composite location description entry.  */

class dwarf_composite final : public dwarf_location
{
public:
  dwarf_composite (gdbarch *arch, dwarf2_per_cu_data *per_cu)
    : dwarf_location (arch, 0), m_per_cu (per_cu)
  {}

  dwarf_location_up clone_location () const override
  {
    return make_unique<dwarf_composite> (*this);
  }

  void add_piece (std::unique_ptr<dwarf_location> location, ULONGEST bit_size)
  {
    gdb_assert (location != nullptr);
    gdb_assert (!m_completed);
    m_pieces.emplace_back (std::move (location), bit_size);
  }

  void set_completed (bool completed)
  {
    m_completed = completed;
  };

  bool is_completed () const
  {
    return m_completed;
  };

  void read (frame_info *frame, gdb_byte *buf, int buf_bit_offset,
	     size_t bit_size, LONGEST bits_to_skip, size_t location_bit_limit,
	     bool big_endian, int *optimized, int *unavailable) const override;

  void write (frame_info *frame, const gdb_byte *buf,
	      int buf_bit_offset, size_t bit_size, LONGEST bits_to_skip,
	      size_t location_bit_limit, bool big_endian,
	      int *optimized, int *unavailable) const override;

  void read_from_gdb_value (frame_info *frame, struct value *value,
			    int value_bit_offset,
			    LONGEST bits_to_skip, size_t bit_size,
			    size_t location_bit_limit) const override;

  void write_to_gdb_value (frame_info *frame, struct value *value,
			   int value_bit_offset,
			   LONGEST bits_to_skip, size_t bit_size,
			   size_t location_bit_limit) const override;

  bool is_implicit_ptr_at (LONGEST bit_offset, int bit_length) const override;

  value *indirect_implicit_ptr (frame_info *frame, struct type *type,
				LONGEST pointer_offset = 0,
				LONGEST bit_offset = 0,
				int bit_length = 0) const override;

  bool is_optimized_out (frame_info *frame, bool big_endian,
			 LONGEST bits_to_skip, size_t bit_size,
			 size_t location_bit_limit) const override;

  value *to_gdb_value (frame_info *frame, struct type *type,
		       struct type *subobj_type,
		       LONGEST subobj_offset) const override;

private:
  /* Composite piece that contains a piece location
     description and it's size.  */
  struct piece
  {
  public:
    piece (std::unique_ptr<dwarf_location> location, ULONGEST size)
      : location (std::move (location)), size (size)
    {}

    /* We need to make a piece copyiable, because dwarf_composite can be
       copied / cloned.  */
    piece (const piece &other)
      : location (other.location->clone_location ()), size (other.size)
    {}

    piece (piece &&) = default;

    void operator=(const piece &) = delete;
    void operator=(piece &&) = delete;

    std::unique_ptr<dwarf_location> location;
    ULONGEST size;
  };

  /* Compilation unit context of the pointer.  */
  dwarf2_per_cu_data *m_per_cu;

  /* Vector of composite pieces.  */
  std::vector<piece> m_pieces;

  /* True if location description is completed.  */
  bool m_completed = false;
};

void
dwarf_composite::read (frame_info *frame, gdb_byte *buf,
		       int buf_bit_offset, size_t bit_size,
		       LONGEST bits_to_skip, size_t location_bit_limit,
		       bool big_endian, int *optimized, int *unavailable) const
{
  unsigned int pieces_num = m_pieces.size ();
  LONGEST total_bits_to_skip = bits_to_skip;
  unsigned int i;

  if (!m_completed)
    ill_formed_expression ();

  total_bits_to_skip += m_offset * HOST_CHAR_BIT + m_bit_suboffset;

  /* Skip pieces covered by the read offset.  */
  for (i = 0; i < pieces_num; i++)
    {
      LONGEST piece_bit_size = m_pieces[i].size;

      if (total_bits_to_skip < piece_bit_size)
        break;

      total_bits_to_skip -= piece_bit_size;
    }

  for (; i < pieces_num; i++)
    {
      LONGEST piece_bit_size = m_pieces[i].size;
      LONGEST actual_bit_size = piece_bit_size;

      if (actual_bit_size > bit_size)
        actual_bit_size = bit_size;

      m_pieces[i].location->read (frame, buf, buf_bit_offset,
				  actual_bit_size, total_bits_to_skip,
				  piece_bit_size, big_endian,
				  optimized, unavailable);

      if (bit_size == actual_bit_size || *optimized || *unavailable)
	break;

      buf_bit_offset += actual_bit_size;
      bit_size -= actual_bit_size;
    }
}

void
dwarf_composite::write (frame_info *frame, const gdb_byte *buf,
			int buf_bit_offset, size_t bit_size,
			LONGEST bits_to_skip, size_t location_bit_limit,
			bool big_endian, int *optimized,
			int *unavailable) const
{
  LONGEST total_bits_to_skip = bits_to_skip;
  unsigned int pieces_num = m_pieces.size ();
  unsigned int i;

  if (!m_completed)
    ill_formed_expression ();

  total_bits_to_skip += m_offset * HOST_CHAR_BIT + m_bit_suboffset;

  /* Skip pieces covered by the write offset.  */
  for (i = 0; i < pieces_num; i++)
    {
      LONGEST piece_bit_size = m_pieces[i].size;

      if (total_bits_to_skip < piece_bit_size)
	break;

      total_bits_to_skip -= piece_bit_size;
    }

  for (; i < pieces_num; i++)
    {
      LONGEST piece_bit_size = m_pieces[i].size;
      LONGEST actual_bit_size = piece_bit_size;

      if (actual_bit_size > bit_size)
        actual_bit_size = bit_size;

      m_pieces[i].location->write (frame, buf, buf_bit_offset,
				   actual_bit_size, total_bits_to_skip,
				   piece_bit_size, big_endian,
				   optimized, unavailable);

      if (bit_size == actual_bit_size || *optimized || *unavailable)
	break;

      buf_bit_offset += actual_bit_size;
      bit_size -= actual_bit_size;
    }
}

void
dwarf_composite::read_from_gdb_value (frame_info *frame, struct value *value,
				      int value_bit_offset,
				      LONGEST bits_to_skip, size_t bit_size,
				      size_t location_bit_limit) const
{
  ULONGEST total_bits_to_skip
    = bits_to_skip + HOST_CHAR_BIT * m_offset + m_bit_suboffset;
  ULONGEST remaining_bit_size = bit_size;
  ULONGEST bit_offset = value_bit_offset;
  unsigned int pieces_num = m_pieces.size ();
  unsigned int i;

  /* Advance to the first non-skipped piece.  */
  for (i = 0; i < pieces_num; i++)
    {
      ULONGEST piece_bit_size = m_pieces[i].size;

      if (total_bits_to_skip < piece_bit_size)
	break;

      total_bits_to_skip -= piece_bit_size;
    }

  for (; i < pieces_num; i++)
    {
      const dwarf_location &location = *m_pieces[i].location;
      ULONGEST piece_bit_size = m_pieces[i].size;
      size_t this_bit_size = piece_bit_size - total_bits_to_skip;

      if (this_bit_size > remaining_bit_size)
	this_bit_size = remaining_bit_size;

      location.read_from_gdb_value (frame, value, bit_offset,
				    total_bits_to_skip, this_bit_size,
				    piece_bit_size);

      bit_offset += this_bit_size;
      remaining_bit_size -= this_bit_size;
      total_bits_to_skip = 0;
    }
}

void
dwarf_composite::write_to_gdb_value (frame_info *frame, struct value *value,
				     int value_bit_offset,
				     LONGEST bits_to_skip, size_t bit_size,
				     size_t location_bit_limit) const
{
  ULONGEST total_bits_to_skip
    = bits_to_skip + HOST_CHAR_BIT * m_offset + m_bit_suboffset;
  ULONGEST remaining_bit_size = bit_size;
  ULONGEST bit_offset = value_bit_offset;
  unsigned int pieces_num = m_pieces.size ();
  unsigned int i;

  /* Advance to the first non-skipped piece.  */
  for (i = 0; i < pieces_num; i++)
    {
      ULONGEST piece_bit_size = m_pieces[i].size;

      if (total_bits_to_skip < piece_bit_size)
	break;

      total_bits_to_skip -= piece_bit_size;
    }

  for (; i < pieces_num; i++)
    {
      const dwarf_location &location = *m_pieces[i].location;
      ULONGEST piece_bit_size = m_pieces[i].size;
      size_t this_bit_size = piece_bit_size - total_bits_to_skip;

      if (this_bit_size > remaining_bit_size)
	this_bit_size = remaining_bit_size;

      location.write_to_gdb_value (frame, value, bit_offset,
				   total_bits_to_skip, this_bit_size,
				   piece_bit_size);

      bit_offset += this_bit_size;
      remaining_bit_size -= this_bit_size;
      total_bits_to_skip = 0;
    }
}

bool
dwarf_composite::is_implicit_ptr_at (LONGEST bit_offset, int bit_length) const
{
  /* Advance to the first non-skipped piece.  */
  unsigned int pieces_num = m_pieces.size ();
  LONGEST total_bit_offset = bit_offset;
  LONGEST total_bit_length = bit_length;

  total_bit_offset += HOST_CHAR_BIT * m_offset + m_bit_suboffset;

  for (unsigned int i = 0; i < pieces_num && total_bit_length != 0; i++)
    {
      const piece &piece = m_pieces[i];
      ULONGEST read_bit_length = piece.size;

      if (total_bit_offset >= read_bit_length)
	{
	  total_bit_offset -= read_bit_length;
	  continue;
	}

      read_bit_length -= total_bit_offset;

      if (total_bit_length < read_bit_length)
	read_bit_length = total_bit_length;

      if (piece.location->is_implicit_ptr_at (total_bit_offset,
					      read_bit_length))
	return true;

      total_bit_offset = 0;
      total_bit_length -= read_bit_length;
    }

    return false;
}

value *
dwarf_composite::indirect_implicit_ptr (frame_info *frame, struct type *type,
					LONGEST pointer_offset,
					LONGEST bit_offset,
					int bit_length) const
{
  LONGEST total_bit_offset = HOST_CHAR_BIT * m_offset
			     + m_bit_suboffset + bit_offset;

  /* Advance to the first non-skipped piece.  */
  for (const piece &piece : m_pieces)
    {
      ULONGEST read_bit_length = piece.size;

      if (total_bit_offset >= read_bit_length)
	{
	  total_bit_offset -= read_bit_length;
	  continue;
	}

      read_bit_length -= total_bit_offset;

      if (bit_length < read_bit_length)
	read_bit_length = bit_length;

      return piece.location->indirect_implicit_ptr (frame, type,
						    pointer_offset,
						    total_bit_offset,
						    read_bit_length);
    }

  return nullptr;
}

bool
dwarf_composite::is_optimized_out (frame_info *frame, bool big_endian,
				   LONGEST bits_to_skip, size_t bit_size,
				   size_t location_bit_limit) const
{
  ULONGEST total_bits_to_skip
    = bits_to_skip + HOST_CHAR_BIT * m_offset + m_bit_suboffset;
  ULONGEST remaining_bit_size = bit_size;
  unsigned int pieces_num = m_pieces.size ();
  unsigned int i;

  /* Advance to the first non-skipped piece.  */
  for (i = 0; i < pieces_num; i++)
    {
      ULONGEST piece_bit_size = m_pieces[i].size;

      if (total_bits_to_skip < piece_bit_size)
	break;

      total_bits_to_skip -= piece_bit_size;
    }

  for (; i < pieces_num; i++)
    {
      const dwarf_location &location = *m_pieces[i].location;
      ULONGEST piece_bit_size = m_pieces[i].size;
      size_t this_bit_size = piece_bit_size - total_bits_to_skip;

      if (this_bit_size > remaining_bit_size)
	this_bit_size = remaining_bit_size;

      if (location.is_optimized_out (frame, big_endian, total_bits_to_skip,
				     this_bit_size, piece_bit_size))
	return true;

      remaining_bit_size -= this_bit_size;
      total_bits_to_skip = 0;
    }

  return false;
}

value *
dwarf_composite::to_gdb_value (frame_info *frame, struct type *type,
			       struct type *subobj_type,
			       LONGEST subobj_offset) const
{
  gdb_assert (type != nullptr);
  gdb_assert (subobj_type != nullptr);

  ULONGEST bit_size = 0;

  for (const piece &piece : m_pieces)
    bit_size += piece.size;

  /* Complain if the expression is larger than the size of the
     outer type.  */
  if (bit_size > HOST_CHAR_BIT * TYPE_LENGTH (type))
    invalid_synthetic_pointer ();

  computed_closure *closure;
  std::unique_ptr<dwarf_composite> composite_copy
    = make_unique<dwarf_composite> (*this);
  composite_copy->set_completed (true);

  /* If compilation unit information is not available
     we are in a CFI context.  */
  if (m_per_cu == nullptr)
    closure = new computed_closure (std::move (composite_copy), frame);
  else
    closure = new computed_closure (std::move (composite_copy),
				    get_frame_id (frame));

  closure->incref ();

  value *retval
    = allocate_computed_value (subobj_type, &closure_value_funcs, closure);
  set_value_offset (retval, subobj_offset);

  return retval;
}

/* Return ENTRY as a dwarf_location.
   If already a dwarf_location, return it as is, otherwise convert it.  */

static dwarf_location_up
to_location (dwarf_entry_up entry, gdbarch *arch)
{
  dwarf_location *location = dynamic_cast<dwarf_location *> (entry.get ());

  if (location != nullptr)
    {
      entry.release ();
      return dwarf_location_up (location);
    }

  dwarf_value *value = dynamic_cast<dwarf_value *> (entry.get ());
  gdb_assert (value != nullptr);

  return value->to_location (arch);
}

/* Return ENTRY as a dwarf_value.
   If already a dwarf_value, return it as is, otherwise convert it.  */

static dwarf_value_up
to_value (dwarf_entry_up entry, type *address_type)
{
  dwarf_value *value = dynamic_cast<dwarf_value *> (entry.get ());

  if (value != nullptr)
    {
      entry.release ();
      return dwarf_value_up (value);
    }

  dwarf_location *location = dynamic_cast<dwarf_location *> (entry.get ());
  gdb_assert (location != nullptr);

  return location->to_value (address_type);
}

/* Set of functions that perform different arithmetic operations
   on dwarf_value arguments.

   Currently the existing struct value operations are used under the
   hood to avoid the code duplication.  Vector types are planned to be
   promoted to base types in the future anyway which means that the
   operations subset needed is just going to grow anyway.  */

/* Compare two DWARF value's ARG1 and ARG2 for equality in a context
   of a value entry comparison.  */

static bool
dwarf_value_equal_op (dwarf_value &arg1, dwarf_value &arg2)
{
  struct value *arg1_value = arg1.to_gdb_value (arg1.type ());
  struct value *arg2_value = arg2.to_gdb_value (arg2.type ());
  return value_equal (arg1_value, arg2_value);
}

/* Compare if DWARF value ARG1 is less then DWARF value ARG2 in a
   context of a value entry comparison.   */

static bool
dwarf_value_less_op (dwarf_value &arg1, dwarf_value &arg2)
{
  struct value *arg1_value = arg1.to_gdb_value (arg1.type ());
  struct value *arg2_value = arg2.to_gdb_value (arg2.type ());
  return value_less (arg1_value, arg2_value);
}

/* Apply binary operation OP on given ARG1 and ARG2 arguments
   and return a new value entry containing the result of that
   operation.  */

static dwarf_value_up
dwarf_value_binary_op (dwarf_value &arg1, dwarf_value &arg2,
		       enum exp_opcode op)
{
  struct value *arg1_value = arg1.to_gdb_value (arg1.type ());
  struct value *arg2_value = arg2.to_gdb_value (arg2.type ());
  return make_unique<dwarf_value> (value_binop (arg1_value, arg2_value, op));
}

/* Apply a negation operation on ARG and return a new value entry
   containing the result of that operation.  */

static dwarf_value_up
dwarf_value_negation_op (dwarf_value &arg)
{
  return make_unique<dwarf_value> (value_neg (arg.to_gdb_value (arg.type ())));
}

/* Apply a complement operation on ARG and return a new value entry
   containing the result of that operation.  */

static dwarf_value_up
dwarf_value_complement_op (dwarf_value &arg)
{
  value *result = value_complement (arg.to_gdb_value (arg.type ()));
  return make_unique<dwarf_value> (result);
}

/* Apply a cast operation on ARG and return a new value entry
   containing the result of that operation.  */

static dwarf_value_up
dwarf_value_cast_op (dwarf_value &arg, struct type *type)
{
  struct value *result = value_cast (type, arg.to_gdb_value (arg.type ()));
  return make_unique<dwarf_value> (result);
}

static void *
copy_value_closure (const value *v)
{
  computed_closure *closure = ((computed_closure*) value_computed_closure (v));

  if (closure == nullptr)
    internal_error (__FILE__, __LINE__, _("invalid closure type"));

  closure->incref ();
  return closure;
}

static void
free_value_closure (value *v)
{
  computed_closure *closure = ((computed_closure*) value_computed_closure (v));

  if (closure == nullptr)
    internal_error (__FILE__, __LINE__, _("invalid closure type"));

  closure->decref ();

  if (closure->refcount () == 0)
    delete closure;
}

/* Read or write a closure value V.  If FROM != NULL, operate in "write
   mode": copy FROM into the closure comprising V.  If FROM == NULL,
   operate in "read mode": fetch the contents of the (lazy) value V by
   composing it from its closure.  */

static void
rw_closure_value (value *v, value *from)
{
  LONGEST bit_offset = 0, max_bit_size;
  computed_closure *closure = (computed_closure*) value_computed_closure (v);
  struct type *v_type = value_type (v);
  bool big_endian = type_byte_order (v_type) == BFD_ENDIAN_BIG;
  const dwarf_location &location = closure->get_location ();
  /* DWARF evaluator only supports targets with byte size of 8 bits,
     while struct value offset is expressed in memory unit size.  */
  int unit_size = gdbarch_addressable_memory_unit_size (v_type->arch ());

  if (from == nullptr)
    {
      if (v_type != value_enclosing_type (v))
        internal_error (__FILE__, __LINE__,
			_("Should not be able to create a lazy value with "
			  "an enclosing type"));
    }

  ULONGEST bits_to_skip = HOST_CHAR_BIT * unit_size * value_offset (v);

  /* If there are bits that don't complete a byte, count them in.  */
  if (value_bitsize (v))
    {
      bits_to_skip
	+= HOST_CHAR_BIT * unit_size * value_offset (value_parent (v))
	   + value_bitpos (v);
      if (from != nullptr && big_endian)
	{
	  /* Use the least significant bits of FROM.  */
	  max_bit_size = HOST_CHAR_BIT * TYPE_LENGTH (value_type (from));
	  bit_offset = max_bit_size - value_bitsize (v);
	}
      else
	max_bit_size = value_bitsize (v);
    }
  else
    max_bit_size = HOST_CHAR_BIT * TYPE_LENGTH (v_type);

  frame_info *frame = closure->get_frame ();

  if (frame == nullptr)
    frame = frame_find_by_id (closure->get_frame_id ());

  if (from == nullptr)
    {
      location.write_to_gdb_value (frame, v, bit_offset, bits_to_skip,
				   max_bit_size - bit_offset, 0);
    }
  else
    {
      location.read_from_gdb_value (frame, from, bit_offset, bits_to_skip,
				    max_bit_size - bit_offset, 0);
    }
}

static void
read_closure_value (value *v)
{
  rw_closure_value (v, NULL);
}

static void
write_closure_value (value *to, value *from)
{
  rw_closure_value (to, from);
}

/* Check if a closure value V describes any piece of the
   underlying location description as optimized out.  */

static bool
is_optimized_out_closure_value (value *v)
{
  LONGEST max_bit_size;
  computed_closure *closure = (computed_closure*) value_computed_closure (v);
  struct type *v_type = value_type (v);
  bool big_endian = type_byte_order (v_type) == BFD_ENDIAN_BIG;
  const dwarf_location &location = closure->get_location ();
  /* DWARF evaluator only supports targets with byte size of 8 bits,
     while struct value offset is expressed in memory unit size.  */
  int unit_size = gdbarch_addressable_memory_unit_size (v_type->arch ());

  if (v_type != value_enclosing_type (v))
    internal_error (__FILE__, __LINE__,
		    _("Should not be able to create a lazy value with "
		      "an enclosing type"));

  ULONGEST bits_to_skip = HOST_CHAR_BIT * unit_size * value_offset (v);

  /* If there are bits that don't complete a byte, count them in.  */
  if (value_bitsize (v))
    {
      bits_to_skip
	+= HOST_CHAR_BIT * unit_size * value_offset (value_parent (v))
	   + value_bitpos (v);
      max_bit_size = value_bitsize (v);
    }
  else
    max_bit_size = HOST_CHAR_BIT * TYPE_LENGTH (v_type);

  frame_info *frame = closure->get_frame ();

  if (frame == nullptr)
    frame = frame_find_by_id (closure->get_frame_id ());

  return location.is_optimized_out (frame, big_endian, bits_to_skip,
				    max_bit_size, 0);
}

/* An implementation of an lval_funcs method to see whether a value is
   a synthetic pointer.  */

static int
check_synthetic_pointer (const value *value, LONGEST bit_offset,
			 int bit_length)
{
  /* DWARF evaluator only supports targets with byte size of 8 bits,
     while struct value offset is expressed in memory unit size.  */
  int unit_size
    = gdbarch_addressable_memory_unit_size (value_type (value)->arch ());
  LONGEST total_bit_offset
    = HOST_CHAR_BIT * unit_size * value_offset (value) + bit_offset;

  if (value_bitsize (value))
    total_bit_offset += value_bitpos (value);

  computed_closure *closure
    = (computed_closure *) value_computed_closure (value);

  return closure->get_location ().is_implicit_ptr_at (total_bit_offset,
						      bit_length);
}

/* An implementation of an lval_funcs method to indirect through a
   pointer.  This handles the synthetic pointer case when needed.  */

static value *
indirect_closure_value (value *value)
{
  computed_closure *closure
    = (computed_closure *) value_computed_closure (value);

  struct type *type = check_typedef (value_type (value));
  if (type->code () != TYPE_CODE_PTR)
    return nullptr;

  /* DWARF evaluator only supports targets with byte size of 8 bits,
     while struct value offset is expressed in memory unit size.  */
  int unit_size = gdbarch_addressable_memory_unit_size (type->arch ());
  LONGEST bit_length = HOST_CHAR_BIT * TYPE_LENGTH (type);
  LONGEST bit_offset = HOST_CHAR_BIT * unit_size * value_offset (value);

  if (value_bitsize (value))
    bit_offset += value_bitpos (value);

  frame_info *frame = get_selected_frame (_("No frame selected."));

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
  bfd_endian byte_order = gdbarch_byte_order (get_frame_arch (frame));
  LONGEST pointer_offset
    = extract_signed_integer (value_contents (value).data (),
			      TYPE_LENGTH (type), byte_order);

  return closure->get_location ().indirect_implicit_ptr (frame, type,
							 pointer_offset,
							 bit_offset, bit_length);
}

/* Implementation of the coerce_ref method of lval_funcs for synthetic C++
   references.  */

static value *
coerce_closure_ref (const value *value)
{
  struct type *type = check_typedef (value_type (value));

  if (value_bits_synthetic_pointer (value, value_embedded_offset (value),
				    HOST_CHAR_BIT * TYPE_LENGTH (type)))
    {
      computed_closure *closure
	= (computed_closure *) value_computed_closure (value);
      frame_info *frame = get_selected_frame (_("No frame selected."));

      return closure->get_location ().indirect_implicit_ptr (frame, type);
    }
  else
    {
      /* Else: not a synthetic reference; do nothing.  */
      return nullptr;
    }
}

/* Convert struct value VALUE to the matching DWARF entry
   representation.  ARCH describes an architecture of the new
   entry.  */

static dwarf_location_up
gdb_value_to_dwarf_entry (gdbarch *arch, struct value *value)
{
  struct type *type = value_type (value);

  /* DWARF evaluator only supports targets with byte size of 8 bits,
     while struct value offset is expressed in memory unit size.  */
  int unit_size = gdbarch_addressable_memory_unit_size (arch);
  LONGEST offset = value_offset (value) * unit_size;

  switch (value_lval_const (value))
    {
      /* We can only convert struct value to a location because
	 we can't distinguish between the implicit value and
	 not_lval.  */
    case not_lval:
      {
	gdb_byte *contents_start = value_contents_raw (value).data () + offset;

	return make_unique<dwarf_implicit>
	  (arch, gdb::array_view<const gdb_byte> (contents_start,
						  TYPE_LENGTH (type)),
	   type_byte_order (type));
      }
    case lval_memory:
      return make_unique<dwarf_memory> (arch, value_address (value),
					value_stack (value));
    case lval_register:
      return make_unique<dwarf_register> (arch, VALUE_REGNUM (value), offset);
    case lval_computed:
      {
	/* Dwarf entry is enclosed by the closure anyway so we just
	   need to unwrap it here.  */
	computed_closure *closure
	  = ((computed_closure *) value_computed_closure (value));

	const dwarf_location &location = closure->get_location ();
	dwarf_location_up location_copy = location.clone_location ();
	location_copy->add_bit_offset (offset * HOST_CHAR_BIT);
	return location_copy;
      }
    default:
      internal_error (__FILE__, __LINE__, _("invalid location type"));
  }
}

/* Given context CTX, section offset SECT_OFF, and compilation unit
   data PER_CU, execute the "variable value" operation on the DIE
   found at SECT_OFF.  */

static value *
sect_variable_value (sect_offset sect_off,
		     dwarf2_per_cu_data *per_cu,
		     dwarf2_per_objfile *per_objfile)
{
  const char *var_name = nullptr;
  struct type *die_type
    = dwarf2_fetch_die_type_sect_off (sect_off, per_cu, per_objfile,
				      &var_name);

  if (die_type == NULL)
    error (_("Bad DW_OP_GNU_variable_value DIE."));

  /* Note: Things still work when the following test is removed.  This
     test and error is here to conform to the proposed specification.  */
  if (die_type->code () != TYPE_CODE_INT
      && die_type->code () != TYPE_CODE_ENUM
      && die_type->code () != TYPE_CODE_RANGE
      && die_type->code () != TYPE_CODE_PTR)
    error (_("Type of DW_OP_GNU_variable_value DIE must be an integer or pointer."));

  if (var_name != nullptr)
    {
      value *result = compute_var_value (var_name);
      if (result != nullptr)
	return result;
    }

  struct type *type = lookup_pointer_type (die_type);
  frame_info *frame = get_selected_frame (_("No frame selected."));
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
  dwarf_expr_context (dwarf2_per_objfile *per_objfile,
		      int addr_size);

  /* Evaluate the expression at ADDR (LEN bytes long) in a given PER_CU
     FRAME context.  INIT_VALUES vector contains values that are
     expected to be pushed on a DWARF expression stack before the
     evaluation.  AS_LVAL defines if the returned struct value is
     expected to be a value or a location description.  Where TYPE,
     SUBOBJ_TYPE and SUBOBJ_OFFSET describe expected struct value
     representation of the evaluation result.  The ADDR_INFO property
     can be specified to override the range of memory addresses with
     the passed in buffer.  */
  struct value *evaluate (const gdb_byte *addr, size_t len, bool as_lval,
			  dwarf2_per_cu_data *per_cu, frame_info *frame,
			  std::vector<value *> *init_values,
			  const property_addr_info *addr_info,
			  struct type *type, struct type *subobj_type,
			  LONGEST subobj_offset);

private:
  /* The stack of DWARF entries.  */
  std::vector<dwarf_entry_up> m_stack;

  /* Target address size in bytes.  */
  int m_addr_size;

  /* The current depth of dwarf expression recursion, via DW_OP_call*,
     DW_OP_fbreg, DW_OP_push_object_address, etc., and the maximum
     depth we'll tolerate before raising an error.  */
  int m_recursion_depth = 0, m_max_recursion_depth = 0x100;

  /* We evaluate the expression in the context of this objfile.  */
  dwarf2_per_objfile *m_per_objfile;

  /* Frame information used for the evaluation.  */
  frame_info *m_frame = nullptr;

  /* Compilation unit used for the evaluation.  */
  dwarf2_per_cu_data *m_per_cu = nullptr;

  /* Property address info used for the evaluation.  */
  const property_addr_info *m_addr_info = nullptr;

  /* Evaluate the expression at ADDR (LEN bytes long).  */
  void eval (const gdb_byte *addr, size_t len);

  /* Return the type used for DWARF operations where the type is
     unspecified in the DWARF spec.  Only certain sizes are
     supported.  */
  type *address_type () const;

  /* Push ENTRY onto the stack.  */
  void push (dwarf_entry_up value);

  /* Return true if the expression stack is empty.  */
  bool stack_empty_p () const;

  /* Pop a top element of the stack and add as a composite piece.
     The action is based on the context:

      - If the stack is empty, then an incomplete composite location
	description (comprised of one undefined location description),
	is pushed on the stack.

      - Otherwise, if the top stack entry is an incomplete composite
	location description, then it is updated to append a new piece
	comprised of one undefined location description.  The
	incomplete composite location description is then left on the
	stack.

      - Otherwise, if the top stack entry is a location description or
	can be converted to one, it is popped. Then:

	 - If the top stack entry (after popping) is a location
	   description comprised of one incomplete composite location
	   description, then it is updated to append a new piece
	   specified by the previously popped location description.
	   The incomplete composite location description is then left
	   on the stack.

	 - Otherwise, a new location description comprised of one
	   incomplete composite location description, with a new piece
	   specified by the previously popped location description, is
	   pushed on the stack.

      - Otherwise, the DWARF expression is ill-formed  */
  void add_piece (ULONGEST bit_size, ULONGEST bit_offset);

  /* The engine for the expression evaluator.  Using the context in this
     object, evaluate the expression between OP_PTR and OP_END.  */
  void execute_stack_op (const gdb_byte *op_ptr, const gdb_byte *op_end);

  /* Pop the top item off of the stack.  */
  dwarf_entry_up pop ();

  /* Retrieve the N'th item on the stack.  */
  dwarf_entry &fetch (int n);

  /* Fetch the result of the expression evaluation in a form of
     a struct value, where TYPE, SUBOBJ_TYPE and SUBOBJ_OFFSET
     describe the source level representation of that result.
     AS_LVAL defines if the fetched struct value is expected to
     be a value or a location description.  */
  value *fetch_result (struct type *type, struct type *subobj_type,
		       LONGEST subobj_offset, bool as_lval);

  /* Return the location expression for the frame base attribute, in
     START and LENGTH.  The result must be live until the current
     expression evaluation is complete.  */
  void get_frame_base (const gdb_byte **start, size_t *length);

  /* Return the base type given by the indicated DIE at DIE_CU_OFF.
     This can throw an exception if the DIE is invalid or does not
     represent a base type.  */
  type *get_base_type (cu_offset die_cu_off);

  /* Execute DW_AT_location expression for the DWARF expression
     subroutine in the DIE at DIE_CU_OFF in the CU.  Do not touch
     STACK while it being passed to and returned from the called DWARF
     subroutine.  */
  void dwarf_call (cu_offset die_cu_off);

  /* Push on DWARF stack an entry evaluated for DW_TAG_call_site's
     parameter matching KIND and KIND_U at the caller of specified
     BATON. If DEREF_SIZE is not -1 then use DW_AT_call_data_value
     instead of DW_AT_call_value.  */
  void push_dwarf_reg_entry_value (call_site_parameter_kind kind,
				   call_site_parameter_u kind_u,
				   int deref_size);
};

/* Return the type used for DWARF operations where the type is
   unspecified in the DWARF spec.  Only certain sizes are
   supported.  */

type *
dwarf_expr_context::address_type () const
{
  return ::address_type (this->m_per_objfile->objfile->arch (),
			 this->m_addr_size);
}

/* Create a new context for the expression evaluator.  */

dwarf_expr_context::dwarf_expr_context (dwarf2_per_objfile *per_objfile,
					int addr_size)
: m_addr_size (addr_size),
  m_per_objfile (per_objfile)
{
}

void
dwarf_expr_context::push (dwarf_entry_up entry)
{
  this->m_stack.emplace_back (std::move (entry));
}

dwarf_entry_up
dwarf_expr_context::pop ()
{
  if (this->m_stack.empty ())
    error (_("dwarf expression stack underflow"));

  dwarf_entry_up entry = std::move (this->m_stack.back ());
  this->m_stack.pop_back ();
  return entry;
}

dwarf_entry &
dwarf_expr_context::fetch (int n)
{
  if (this->m_stack.size () <= n)
     error (_("Asked for position %d of stack, "
	      "stack only has %zu elements on it."),
	    n, this->m_stack.size ());
  return *this->m_stack[this->m_stack.size () - (1 + n)];
}

void
dwarf_expr_context::get_frame_base (const gdb_byte **start,
				    size_t * length)
{
  ensure_have_frame (this->m_frame, "DW_OP_fbreg");

  const block *bl = get_frame_block (this->m_frame, NULL);

  if (bl == NULL)
    error (_("frame address is not available."));

  /* Use block_linkage_function, which returns a real (not inlined)
     function, instead of get_frame_function, which may return an
     inlined function.  */
  symbol *framefunc = block_linkage_function (bl);

  /* If we found a frame-relative symbol then it was certainly within
     some function associated with a frame. If we can't find the frame,
     something has gone wrong.  */
  gdb_assert (framefunc != NULL);

  func_get_frame_base_dwarf_block (framefunc,
				   get_frame_address_in_block (this->m_frame),
				   start, length);
}

type *
dwarf_expr_context::get_base_type (cu_offset die_cu_off)
{
  if (this->m_per_cu == nullptr)
    return builtin_type (this->m_per_objfile->objfile->arch ())->builtin_int;

  type *result = dwarf2_get_die_type (die_cu_off, this->m_per_cu,
				      this->m_per_objfile);

  if (result == nullptr)
    error (_("Could not find type for operation"));
  return result;
}

void
dwarf_expr_context::dwarf_call (cu_offset die_cu_off)
{
  ensure_have_per_cu (this->m_per_cu, "DW_OP_call");

  frame_info *frame = this->m_frame;

  auto get_pc_from_frame = [frame] ()
    {
      ensure_have_frame (frame, "DW_OP_call");
      return get_frame_address_in_block (frame);
    };

  dwarf2_locexpr_baton block
    = dwarf2_fetch_die_loc_cu_off (die_cu_off, this->m_per_cu,
				   this->m_per_objfile, get_pc_from_frame);

  /* DW_OP_call_ref is currently not supported.  */
  gdb_assert (block.per_cu == this->m_per_cu);

  this->eval (block.data, block.size);
}

void
dwarf_expr_context::push_dwarf_reg_entry_value (call_site_parameter_kind kind,
						call_site_parameter_u kind_u,
						int deref_size)
{
  ensure_have_per_cu (this->m_per_cu, "DW_OP_entry_value");
  ensure_have_frame (this->m_frame, "DW_OP_entry_value");

  dwarf2_per_cu_data *caller_per_cu;
  dwarf2_per_objfile *caller_per_objfile;
  frame_info *caller_frame = get_prev_frame (this->m_frame);
  call_site_parameter *parameter
    = dwarf_expr_reg_to_entry_parameter (this->m_frame, kind, kind_u,
					 &caller_per_cu,
					 &caller_per_objfile);
  const gdb_byte *data_src
    = deref_size == -1 ? parameter->value : parameter->data_value;
  size_t size
    = deref_size == -1 ? parameter->value_size : parameter->data_value_size;

  /* DEREF_SIZE size is not verified here.  */
  if (data_src == nullptr)
    throw_error (NO_ENTRY_VALUE_ERROR,
		 _("Cannot resolve DW_AT_call_data_value"));

  /* We are about to evaluate an expression in the context of the caller
     of the current frame.  This evaluation context may be different from
     the current (callee's) context), so temporarily set the caller's context.

     It is possible for the caller to be from a different objfile from the
     callee if the call is made through a function pointer.  */
  scoped_restore save_frame = make_scoped_restore (&this->m_frame,
						   caller_frame);
  scoped_restore save_per_cu = make_scoped_restore (&this->m_per_cu,
						    caller_per_cu);
  scoped_restore save_addr_info = make_scoped_restore (&this->m_addr_info,
						       nullptr);
  scoped_restore save_per_objfile = make_scoped_restore (&this->m_per_objfile,
							 caller_per_objfile);

  scoped_restore save_addr_size = make_scoped_restore (&this->m_addr_size);
  this->m_addr_size = this->m_per_cu->addr_size ();

  this->eval (data_src, size);
}

value *
dwarf_expr_context::fetch_result (struct type *type, struct type *subobj_type,
				  LONGEST subobj_offset, bool as_lval)
{
  gdbarch *arch = this->m_per_objfile->objfile->arch ();

  if (type == nullptr)
    type = address_type ();

  if (subobj_type == nullptr)
    subobj_type = type;

  if (as_lval)
    {
      dwarf_location_up location = to_location (pop (), arch);
      return location->to_gdb_value (this->m_frame, type,
				     subobj_type, subobj_offset);
    }
  else
    {
      dwarf_value_up value = to_value (pop (), address_type ());
      return value->to_gdb_value (subobj_type, subobj_offset);
    }
}

value *
dwarf_expr_context::evaluate (const gdb_byte *addr, size_t len, bool as_lval,
			      dwarf2_per_cu_data *per_cu, frame_info *frame,
			      std::vector<value *> *init_values,
			      const property_addr_info *addr_info,
			      struct type *type, struct type *subobj_type,
			      LONGEST subobj_offset)
{
  this->m_per_cu = per_cu;
  this->m_frame = frame;
  this->m_addr_info = addr_info;
  gdbarch *arch = this->m_per_objfile->objfile->arch ();

  if (init_values != nullptr)
    for (value *val : *init_values)
      push (gdb_value_to_dwarf_entry (arch, val));

  eval (addr, len);
  return fetch_result (type, subobj_type, subobj_offset, as_lval);
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
  return this->m_stack.empty ();
}

void
dwarf_expr_context::add_piece (ULONGEST bit_size, ULONGEST bit_offset)
{
  dwarf_location_up piece;
  gdbarch *arch = this->m_per_objfile->objfile->arch ();

  if (stack_empty_p ())
    piece = make_unique<dwarf_undefined> (arch);
  else
    {
      dwarf_entry &top_entry = fetch (0);
      dwarf_composite *top_entry_as_composite
	= dynamic_cast <dwarf_composite *> (&top_entry);

      if (top_entry_as_composite != nullptr
	  && !top_entry_as_composite->is_completed ())
	piece = make_unique<dwarf_undefined> (arch);
      else
	piece = to_location (pop (), arch);
    }

  piece->add_bit_offset (bit_offset);

  /* The composite to push the piece in.  */
  dwarf_composite *composite;

  /* If stack is empty then it is a start of a new composite.  In the
     future this will check if the composite is finished or not.  */
  if (stack_empty_p ())
    {
      std::unique_ptr<dwarf_composite> new_composite
	= make_unique<dwarf_composite> (arch, this->m_per_cu);
      composite = new_composite.get ();
      push (std::move (new_composite));
    }
  else
    {
      dwarf_entry &top_entry = fetch (0);
      dwarf_composite *top_entry_as_composite
	= dynamic_cast <dwarf_composite *> (&top_entry);

      if (top_entry_as_composite != nullptr
	  && !top_entry_as_composite->is_completed ())
	composite = top_entry_as_composite;
      else
	{
	  std::unique_ptr<dwarf_composite> new_composite
	    = make_unique<dwarf_composite> (arch, this->m_per_cu);
	  composite = new_composite.get ();
	  push (std::move (new_composite));
	}
    }

  composite->add_piece (std::move (piece), bit_size);
}

void
dwarf_expr_context::eval (const gdb_byte *addr, size_t len)
{
  int old_recursion_depth = this->m_recursion_depth;

  execute_stack_op (addr, addr + len);

  /* RECURSION_DEPTH becomes invalid if an exception was thrown here.  */

  gdb_assert (this->m_recursion_depth == old_recursion_depth);
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

/* See expr.h.  */

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
  gdbarch *arch = this->m_per_objfile->objfile->arch ();
  bfd_endian byte_order = gdbarch_byte_order (arch);
  /* Old-style "untyped" DWARF values need special treatment in a
     couple of places, specifically DW_OP_mod and DW_OP_shr.  We need
     a special type for these values so we can distinguish them from
     values that have an explicit type, because explicitly-typed
     values do not need special treatment.  This special type must be
     different (in the `==' sense) from any base type coming from the
     CU.  */
  type *address_type = this->address_type ();

  if (this->m_recursion_depth > this->m_max_recursion_depth)
    error (_("DWARF-2 expression error: Loop detected (%d)."),
	   this->m_recursion_depth);
  this->m_recursion_depth++;

  while (op_ptr < op_end)
    {
      dwarf_location_atom op = (dwarf_location_atom) *op_ptr++;

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
	  {
	    ULONGEST result = op - DW_OP_lit0;
	    push (make_unique<dwarf_value> (result, address_type));
	    break;
	  }

	case DW_OP_addr:
	  {
	    ULONGEST result = extract_unsigned_integer (op_ptr,
							this->m_addr_size,
							byte_order);
	    op_ptr += this->m_addr_size;
	    /* Some versions of GCC emit DW_OP_addr before
	       DW_OP_GNU_push_tls_address.  In this case the value is an
	       index, not an address.  We don't support things like
	       branching between the address and the TLS op.  */
	    if (op_ptr >= op_end || *op_ptr != DW_OP_GNU_push_tls_address)
	      {
		result += this->m_per_objfile->objfile->text_section_offset ();
		push (make_unique<dwarf_memory> (arch, result));
	      }
	    else
	      /* This is a special case where the value is expected to be
		 created instead of memory location.  */
	      push (make_unique<dwarf_value> (result, address_type));
	    break;
	  }

	case DW_OP_addrx:
	case DW_OP_GNU_addr_index:
	  {
	    ensure_have_per_cu (this->m_per_cu, "DW_OP_addrx");
	    uint64_t uoffset;

	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    ULONGEST result = dwarf2_read_addr_index (this->m_per_cu,
						      this->m_per_objfile,
						      uoffset);
	    result += this->m_per_objfile->objfile->text_section_offset ();
	    push (make_unique<dwarf_memory> (arch, result));
	    break;
	  }
	case DW_OP_GNU_const_index:
	  {
	    ensure_have_per_cu (this->m_per_cu, "DW_OP_GNU_const_index");
	    uint64_t uoffset;

	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    ULONGEST result = dwarf2_read_addr_index (this->m_per_cu,
						      this->m_per_objfile,
						      uoffset);
	    push (make_unique<dwarf_value> (result, address_type));
	    break;
	  }

	case DW_OP_const1u:
	  {
	    ULONGEST result = extract_unsigned_integer (op_ptr, 1, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 1;
	    break;
	  }
	case DW_OP_const1s:
	  {
	    ULONGEST result = extract_signed_integer (op_ptr, 1, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 1;
	    break;
	  }
	case DW_OP_const2u:
	  {
	    ULONGEST result = extract_unsigned_integer (op_ptr, 2, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 2;
	    break;
	  }
	case DW_OP_const2s:
	  {
	    ULONGEST result = extract_signed_integer (op_ptr, 2, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 2;
	    break;
	  }
	case DW_OP_const4u:
	  {
	    ULONGEST result = extract_unsigned_integer (op_ptr, 4, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 4;
	    break;
	  }
	case DW_OP_const4s:
	  {
	    ULONGEST result = extract_signed_integer (op_ptr, 4, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 4;
	    break;
	  }
	case DW_OP_const8u:
	  {
	    ULONGEST result = extract_unsigned_integer (op_ptr, 8, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 8;
	    break;
	  }
	case DW_OP_const8s:
	  {
	    ULONGEST result = extract_signed_integer (op_ptr, 8, byte_order);
	    push (make_unique<dwarf_value> (result, address_type));
	    op_ptr += 8;
	    break;
	  }
	case DW_OP_constu:
	  {
	    uint64_t uoffset;
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    push (make_unique<dwarf_value> ((ULONGEST) uoffset, address_type));
	    break;
	  }
	case DW_OP_consts:
	  {
	    int64_t offset;
	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &offset);
	    push (make_unique<dwarf_value> ((ULONGEST) offset, address_type));
	    break;
	  }

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
	case DW_OP_regx:
	  {
	    ensure_have_frame (this->m_frame, "DW_OP_reg");
	    ULONGEST result;

	    if (op == DW_OP_regx)
	      {
		uint64_t reg;
		op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
		result = reg;
	      }
	    else
	      result = op - DW_OP_reg0;

	    push (make_unique<dwarf_register> (arch, result));
	    break;
	  }

	case DW_OP_implicit_value:
	  {
	    uint64_t len;

	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &len);
	    if (op_ptr + len > op_end)
	      error (_("DW_OP_implicit_value: too few bytes available."));
	    push (make_unique<dwarf_implicit>
		    (arch, gdb::array_view<const gdb_byte> (op_ptr, len),
		     BFD_ENDIAN_UNKNOWN));
	    op_ptr += len;
	    break;
	  }

	case DW_OP_stack_value:
	  {
	    std::unique_ptr<dwarf_value> value
	      = to_value (pop (), address_type);

	    push (make_unique<dwarf_implicit>
		    (arch, value->contents (),
		     type_byte_order (value->type ())));
	    break;
	  }

	case DW_OP_implicit_pointer:
	case DW_OP_GNU_implicit_pointer:
	  {
	    int64_t len;
	    ensure_have_per_cu (this->m_per_cu, "DW_OP_implicit_pointer");

	    int ref_addr_size = this->m_per_cu->ref_addr_size ();

	    /* The referred-to DIE of sect_offset kind.  */
	    sect_offset die_offset
	      = (sect_offset) extract_unsigned_integer (op_ptr, ref_addr_size,
							byte_order);
	    op_ptr += ref_addr_size;

	    /* The byte offset into the data.  */
	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &len);
	    push (make_unique<dwarf_implicit_pointer> (arch,
						       this->m_per_objfile,
						       this->m_per_cu,
						       this->m_addr_size,
						       die_offset, len));
	    break;
	  }

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
	case DW_OP_bregx:
	  {
	    uint64_t reg;
	    int64_t offset;

	    if (op == DW_OP_bregx)
	      op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
	    else
	      reg = op - DW_OP_breg0;

	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &offset);
	    ensure_have_frame (this->m_frame, "DW_OP_breg");

	    gdbarch *frame_arch = get_frame_arch (this->m_frame);

	    int regnum = dwarf_reg_to_regnum_or_error (frame_arch, reg);
	    ULONGEST reg_size = register_size (frame_arch, regnum);
	    dwarf_register registr (arch, reg);
	    dwarf_value_up value = registr.deref (this->m_frame,
						  this->m_addr_info,
						  address_type, reg_size);
	    dwarf_location_up location = value->to_location (arch);
	    location->add_bit_offset (offset * HOST_CHAR_BIT);
	    push (std::move (location));
	    break;
	  }

	case DW_OP_fbreg:
	  {
	    int64_t offset;
	    op_ptr = safe_read_sleb128 (op_ptr, op_end, &offset);
	    /* Rather than create a whole new context, we simply
	       backup the current stack locally and install a new empty stack,
	       then reset it afterwards, effectively erasing whatever the
	       recursive call put there.  */
	    std::vector<std::unique_ptr<dwarf_entry>> saved_stack
	      = std::move (this->m_stack);
	    this->m_stack.clear ();

	    const gdb_byte *datastart;
	    size_t datalen;

	    this->get_frame_base (&datastart, &datalen);
	    eval (datastart, datalen);
	    dwarf_entry_up entry = pop ();

	    dwarf_register *registr
	      = dynamic_cast<dwarf_register *> (entry.get ());

	    if (registr != nullptr)
	      entry = registr->deref (this->m_frame, this->m_addr_info,
				      address_type);

	    entry = to_location (std::move (entry), arch);
	    dwarf_memory *memory = dynamic_cast<dwarf_memory *> (entry.get ());

	    /* If we get anything else then memory location here,
	       the DWARF standard defines the expression as ill formed.  */
	    if (memory == nullptr)
	      ill_formed_expression ();

	    memory->add_bit_offset (offset * HOST_CHAR_BIT);
	    memory->set_stack (true);

	    /* Restore the content of the original stack.  */
	    this->m_stack = std::move (saved_stack);
	    push (std::move (entry));
	    break;
	  }

	case DW_OP_dup:
	  push (fetch (0).clone ());
	  break;

	case DW_OP_drop:
	  pop ();
	  break;

	case DW_OP_pick:
	  {
	    int64_t offset = *op_ptr++;
	    push (fetch (offset).clone ());
	    break;
	  }
	  
	case DW_OP_swap:
	  {
	    if (this->m_stack.size () < 2)
	       error (_("Not enough elements for "
			"DW_OP_swap.  Need 2, have %zu."),
		      this->m_stack.size ());

	    std::swap (this->m_stack[this->m_stack.size () - 1],
		       this->m_stack[this->m_stack.size () - 2]);
	    break;
	  }

	case DW_OP_over:
	  push (fetch (1).clone ());
	  break;

	case DW_OP_rot:
	  {
	    if (this->m_stack.size () < 3)
	       error (_("Not enough elements for "
			"DW_OP_rot.  Need 3, have %zu."),
		      this->m_stack.size ());

	    std::swap (this->m_stack[this->m_stack.size () - 1],
		       this->m_stack[this->m_stack.size () - 2]);
	    std::swap (this->m_stack[this->m_stack.size () - 2],
		       this->m_stack[this->m_stack.size () - 3]);
	    break;
	  }

	case DW_OP_deref:
	case DW_OP_deref_size:
	case DW_OP_deref_type:
	case DW_OP_GNU_deref_type:
	  {
	    int addr_size = (op == DW_OP_deref ? this->m_addr_size : *op_ptr++);
	    struct type *type = address_type;

	    if (op == DW_OP_deref_type || op == DW_OP_GNU_deref_type)
	      {
		uint64_t uoffset;
		op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
		cu_offset type_die_cu_off = (cu_offset) uoffset;
		type = get_base_type (type_die_cu_off);
		addr_size = TYPE_LENGTH (type);
	      }

	    dwarf_location_up location = to_location (pop (), arch);
	    push (location->deref (this->m_frame, this->m_addr_info,
				   type, addr_size));
	    break;
	  }

	case DW_OP_abs:
	  {
	    dwarf_value_up arg = to_value (pop (), address_type);
	    struct value *arg_value = arg->to_gdb_value (arg->type ());

	    if (value_less (arg_value, value_zero (arg->type (), not_lval)))
	      arg = dwarf_value_negation_op (*arg);

	    push (std::move (arg));
	    break;
	  }

	case DW_OP_neg:
	  {
	    dwarf_value_up arg = to_value (pop (), address_type);
	    arg = dwarf_value_negation_op (*arg);
	    push (std::move (arg));
	    break;
	  }

	case DW_OP_not:
	  {
	    dwarf_value_up arg = to_value (pop (), address_type);
	    dwarf_require_integral (arg->type ());
	    arg = dwarf_value_complement_op (*arg);
	    push (std::move (arg));
	    break;
	  }

	case DW_OP_plus_uconst:
	  {
	    dwarf_value_up arg = to_value (pop (), address_type);
	    dwarf_require_integral (arg->type ());

	    uint64_t reg;
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
	    ULONGEST result = arg->to_long () + reg;
	    push (make_unique<dwarf_value> (result, address_type));
	    break;
	  }

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
	    dwarf_value_up arg2 = to_value (pop (), address_type);
	    dwarf_value_up arg1 = to_value (pop (), address_type);

	    if (! base_types_equal_p (arg1->type (), arg2->type ()))
	      error (_("Incompatible types on DWARF stack"));

	    switch (op)
	      {
	      case DW_OP_and:
		dwarf_require_integral (arg1->type ());
		dwarf_require_integral (arg2->type ());
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_BITWISE_AND));
		break;
	      case DW_OP_div:
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_DIV));
		break;
	      case DW_OP_minus:
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_SUB));
		break;
	      case DW_OP_mod:
		{
		  int cast_back = 0;
		  type *orig_type = arg1->type ();

		  /* We have to special-case "old-style" untyped values
		     -- these must have mod computed using unsigned
		     math.  */
		  if (orig_type == address_type)
		    {
		      type *utype = get_unsigned_type (arch, orig_type);

		      cast_back = 1;
		      arg1 = dwarf_value_cast_op (*arg1, utype);
		      arg2 = dwarf_value_cast_op (*arg2, utype);
		    }
		  /* Note that value_binop doesn't handle float or
		     decimal float here.  This seems unimportant.  */
		  dwarf_value_up result_val
		    = dwarf_value_binary_op (*arg1, *arg2, BINOP_MOD);
		  if (cast_back)
		    result_val = dwarf_value_cast_op (*result_val, orig_type);

		  push (std::move (result_val));
		}
		break;
	      case DW_OP_mul:
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_MUL));
		break;
	      case DW_OP_or:
		dwarf_require_integral (arg1->type ());
		dwarf_require_integral (arg2->type ());
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_BITWISE_IOR));
		break;
	      case DW_OP_plus:
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_ADD));
		break;
	      case DW_OP_shl:
		dwarf_require_integral (arg1->type ());
		dwarf_require_integral (arg2->type ());
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_LSH));
		break;
	      case DW_OP_shr:
		{
		  dwarf_require_integral (arg1->type ());
		  dwarf_require_integral (arg2->type ());
		  if (!arg1->type ()->is_unsigned ())
		    {
		      struct type *utype
			= get_unsigned_type (arch, arg1->type ());

		      arg1 = dwarf_value_cast_op (*arg1, utype);
		    }

		  dwarf_value_up result_val
		    = dwarf_value_binary_op (*arg1, *arg2, BINOP_RSH);

		  /* Make sure we wind up with the same type we started
		     with.  */
		  if (result_val->type () != arg2->type ())
		    result_val = dwarf_value_cast_op (*result_val,
						      arg2->type ());

		  push (std::move (result_val));
		  break;
		}
	      case DW_OP_shra:
		{
		  dwarf_require_integral (arg1->type ());
		  dwarf_require_integral (arg2->type ());
		  if (arg1->type ()->is_unsigned ())
		    {
		      struct type *stype
			= get_signed_type (arch, arg1->type ());

		      arg1 = dwarf_value_cast_op (*arg1, stype);
		    }

		  dwarf_value_up result_val
		    = dwarf_value_binary_op (*arg1, *arg2, BINOP_RSH);

		  /* Make sure we wind up with the same type we started  with.  */
		  if (result_val->type () != arg2->type ())
		    result_val
		      = dwarf_value_cast_op (*result_val, arg2->type ());

		  push (std::move (result_val));
		  break;
		}
	      case DW_OP_xor:
		dwarf_require_integral (arg1->type ());
		dwarf_require_integral (arg2->type ());
		push (dwarf_value_binary_op (*arg1, *arg2, BINOP_BITWISE_XOR));
		break;
	      case DW_OP_le:
		{
		  /* A <= B is !(B < A).  */
		  ULONGEST result = ! dwarf_value_less_op (*arg2, *arg1);
		  push (make_unique<dwarf_value> (result, address_type));
		  break;
		}
	      case DW_OP_ge:
		{
		  /* A >= B is !(A < B).  */
		  ULONGEST result = ! dwarf_value_less_op (*arg1, *arg2);
		  push (make_unique<dwarf_value> (result, address_type));
		  break;
		}
	      case DW_OP_eq:
		{
		  ULONGEST result = dwarf_value_equal_op (*arg1, *arg2);
		  push (make_unique<dwarf_value> (result, address_type));
		  break;
		}
	      case DW_OP_lt:
		{
		  ULONGEST result = dwarf_value_less_op (*arg1, *arg2);
		  push (make_unique<dwarf_value> (result, address_type));
		  break;
		}
	      case DW_OP_gt:
		{
		/* A > B is B < A.  */
		  ULONGEST result = dwarf_value_less_op (*arg2, *arg1);
		  push (make_unique<dwarf_value> (result, address_type));
		  break;
		}
	      case DW_OP_ne:
		{
		  ULONGEST result = ! dwarf_value_equal_op (*arg1, *arg2);
		  push (make_unique<dwarf_value> (result, address_type));
		  break;
		}
	      default:
		internal_error (__FILE__, __LINE__,
				_("Can't be reached."));
	      }
	    break;
	  }

	case DW_OP_call_frame_cfa:
	  {
	    ensure_have_frame (this->m_frame, "DW_OP_call_frame_cfa");

	    ULONGEST result = dwarf2_frame_cfa (this->m_frame);
	    push (make_unique<dwarf_memory> (arch, result, true));
	    break;
	  }

	case DW_OP_GNU_push_tls_address:
	case DW_OP_form_tls_address:
	  {
	    /* Variable is at a constant offset in the thread-local
	       storage block into the objfile for the current thread and
	       the dynamic linker module containing this expression.  Here
	       we return returns the offset from that base.  The top of the
	       stack has the offset from the beginning of the thread
	       control block at which the variable is located.  Nothing
	       should follow this operator, so the top of stack would be
	       returned.  */
	    dwarf_value_up value = to_value (pop (), address_type);;
	    ULONGEST result
	      = target_translate_tls_address (this->m_per_objfile->objfile,
					      result);
	    push (make_unique<dwarf_memory> (arch, result));
	    break;
	  }

	case DW_OP_skip:
	  {
	    int64_t offset = extract_signed_integer (op_ptr, 2, byte_order);
	    op_ptr += 2;
	    op_ptr += offset;
	    break;
	  }

	case DW_OP_bra:
	  {
	    dwarf_value_up value = to_value (pop (), address_type);

	    int64_t offset = extract_signed_integer (op_ptr, 2, byte_order);
	    op_ptr += 2;
	    dwarf_require_integral (value->type ());

	    if (value->to_long () != 0)
	      op_ptr += offset;
	    break;
	  }

	case DW_OP_nop:
	  break;

	case DW_OP_piece:
	  {
	    uint64_t size;

	    /* Record the piece.  */
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &size);
	    add_piece (HOST_CHAR_BIT * size, 0);
	    break;
	  }

	case DW_OP_bit_piece:
	  {
	    uint64_t size, uleb_offset;

	    /* Record the piece.  */
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &size);
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uleb_offset);
	    add_piece (size, uleb_offset);
	    break;
	  }

	case DW_OP_GNU_uninit:
	  {
	    if (op_ptr != op_end)
	      error (_("DWARF-2 expression error: DW_OP_GNU_uninit must always "
		     "be the very last op."));

	    dwarf_entry &entry = fetch (0);
	    dwarf_location *location = dynamic_cast<dwarf_location *> (&entry);

	    if (location == nullptr)
	      ill_formed_expression ();

	    location->set_initialised (false);
	    break;
	  }

	case DW_OP_call2:
	  {
	    cu_offset cu_off
	      = (cu_offset) extract_unsigned_integer (op_ptr, 2, byte_order);
	    op_ptr += 2;
	    this->dwarf_call (cu_off);
	    break;
	  }

	case DW_OP_call4:
	  {
	    cu_offset cu_off
	      = (cu_offset) extract_unsigned_integer (op_ptr, 4, byte_order);
	    op_ptr += 4;
	    this->dwarf_call (cu_off);
	    break;
	  }

	case DW_OP_GNU_variable_value:
	  {
	    ensure_have_per_cu (this->m_per_cu, "DW_OP_GNU_variable_value");
	    int ref_addr_size = this->m_per_cu->ref_addr_size ();

	    sect_offset sect_off
	      = (sect_offset) extract_unsigned_integer (op_ptr, ref_addr_size,
							byte_order);
	    op_ptr += ref_addr_size;
	    struct value *value = sect_variable_value
	      (sect_off, this->m_per_cu, this->m_per_objfile);
	    value = value_cast (address_type, value);

	    dwarf_entry_up entry = gdb_value_to_dwarf_entry (arch, value);

	    if (dynamic_cast<dwarf_undefined *> (entry.get ()) != nullptr)
	      error_value_optimized_out ();

	    dwarf_location_up location = to_location (std::move (entry), arch);
	    push (location->deref (this->m_frame, this->m_addr_info,
				   address_type));
	    break;
	  }
	
	case DW_OP_entry_value:
	case DW_OP_GNU_entry_value:
	  {
	    uint64_t len;
	    CORE_ADDR deref_size;
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
		break;
	      }

	    kind_u.dwarf_reg = dwarf_block_to_dwarf_reg_deref (op_ptr,
							       op_ptr + len,
							       &deref_size);
	    if (kind_u.dwarf_reg != -1)
	      {
		if (deref_size == -1)
		  deref_size = this->m_addr_size;
		op_ptr += len;
		this->push_dwarf_reg_entry_value (CALL_SITE_PARAMETER_DWARF_REG,
						  kind_u, deref_size);
		break;
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
	    break;
	  }

	case DW_OP_const_type:
	case DW_OP_GNU_const_type:
	  {
	    uint64_t uoffset;
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    cu_offset type_die_cu_off = (cu_offset) uoffset;

	    int n = *op_ptr++;
	    const gdb_byte *data = op_ptr;
	    op_ptr += n;

	    struct type *type = get_base_type (type_die_cu_off);

	    if (TYPE_LENGTH (type) != n)
	      error (_("DW_OP_const_type has different sizes for type and data"));

	    push (make_unique<dwarf_value>
	      (gdb::array_view<const gdb_byte> (data, n), type));
	    break;
	  }

	case DW_OP_regval_type:
	case DW_OP_GNU_regval_type:
	  {
	    uint64_t uoffset, reg;
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &reg);
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    cu_offset type_die_cu_off = (cu_offset) uoffset;

	    ensure_have_frame (this->m_frame, "DW_OP_regval_type");
	    struct type *type = get_base_type (type_die_cu_off);

	    dwarf_register registr (arch, reg);
	    push (registr.deref (this->m_frame, this->m_addr_info, type));
	    break;
	  }

	case DW_OP_convert:
	case DW_OP_GNU_convert:
	case DW_OP_reinterpret:
	case DW_OP_GNU_reinterpret:
	  {
	    uint64_t uoffset;
	    dwarf_value_up value = to_value (pop (), address_type);

	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    cu_offset type_die_cu_off = (cu_offset) uoffset;

	    struct type *type;

	    if (to_underlying (type_die_cu_off) == 0)
	      type = address_type;
	    else
	      type = get_base_type (type_die_cu_off);

	    if (op == DW_OP_convert || op == DW_OP_GNU_convert)
	      value = dwarf_value_cast_op (*value, type);
	    else if (type == value->type ())
	      {
		/* Nothing.  */
	      }
	    else if (TYPE_LENGTH (type) != TYPE_LENGTH (value->type ()))
	      error (_("DW_OP_reinterpret has wrong size"));
	    else
	      value = make_unique<dwarf_value> (value->contents (), type);
	    push (std::move (value));
	    break;
	  }

	case DW_OP_push_object_address:
	  if (this->m_addr_info == nullptr
	      || (this->m_addr_info->valaddr.data () == nullptr
		  && this->m_addr_info->addr == 0))
	    error (_("Location address is not set."));

	  /* Return the address of the object we are
	     currently observing.  */
	  push (make_unique<dwarf_memory> (arch, this->m_addr_info->addr));
	  break;

	case DW_OP_LLVM_offset:
	  {
	    dwarf_value_up value = to_value (pop (), address_type);
	    dwarf_require_integral (value->type ());
	    dwarf_location_up location = to_location (pop (), arch);

	    location->add_bit_offset (value->to_long () * HOST_CHAR_BIT);
	    push (std::move (location));
	    break;
	  }

	case DW_OP_LLVM_offset_constu:
	  {
	    uint64_t uoffset;
	    op_ptr = safe_read_uleb128 (op_ptr, op_end, &uoffset);
	    ULONGEST result = uoffset;
	    dwarf_location_up location = to_location (pop (), arch);

	    location->add_bit_offset (result * HOST_CHAR_BIT);
	    push (std::move (location));
	    break;
	  }

	case DW_OP_LLVM_bit_offset:
	  {
	    dwarf_value_up value = to_value (pop (), address_type);
	    dwarf_require_integral (value->type ());
	    dwarf_location_up location = to_location (pop (), arch);

	    location->add_bit_offset (value->to_long ());
	    push (std::move (location));
	    break;
	  }

	case DW_OP_LLVM_undefined:
	  push (make_unique<dwarf_undefined> (arch));
	  break;

	case DW_OP_LLVM_piece_end:
	  {
	    dwarf_entry &entry = fetch (0);
	    dwarf_composite *composite
	      = dynamic_cast<dwarf_composite *> (&entry);

	    if (composite == nullptr || composite->is_completed ())
	      ill_formed_expression ();

	    composite->set_completed (true);
	    break;
	  }

	default:
	  error (_("Unhandled dwarf expression opcode 0x%x"), op);
	}
    }

  this->m_recursion_depth--;
  gdb_assert (this->m_recursion_depth >= 0);
}

/* See expr.h.  */

value *
dwarf2_evaluate (const gdb_byte *addr, size_t len, bool as_lval,
		 dwarf2_per_objfile *per_objfile, dwarf2_per_cu_data *per_cu,
		 frame_info *frame, int addr_size,
		 std::vector<value *> *init_values,
		 const property_addr_info *addr_info,
		 struct type *type, struct type *subobj_type,
		 LONGEST subobj_offset)
{
  dwarf_expr_context ctx (per_objfile, addr_size);

  return ctx.evaluate (addr, len, as_lval, per_cu,
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
