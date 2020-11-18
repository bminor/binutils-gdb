/* DWARF 2 Expression Evaluator.

   Copyright (C) 2001-2021 Free Software Foundation, Inc.

   Contributed by Daniel Berlin <dan@dberlin.org>.

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

#if !defined (DWARF2EXPR_H)
#define DWARF2EXPR_H

#include "leb128.h"
#include "gdbtypes.h"

struct dwarf2_per_objfile;

/* Evaluate the expression at ADDR (LEN bytes long) in a given PER_CU
   FRAME context.  The PER_OBJFILE contains a pointer to the PER_BFD
   information.  ADDR_SIZE defines a size of the DWARF generic type.
   INIT_VALUES vector contains values that are expected to be pushed
   on a DWARF expression stack before the evaluation.  AS_LVAL defines
   if the returned struct value is expected to be a value or a location
   description.  Where TYPE, SUBOBJ_TYPE and SUBOBJ_OFFSET describe
   expected struct value representation of the evaluation result.  The
   ADDR_INFO property can be specified to override the range of memory
   addresses with the passed in buffer.  */
value *dwarf2_evaluate (const gdb_byte *addr, size_t len, bool as_lval,
			dwarf2_per_objfile *per_objfile,
			dwarf2_per_cu_data *per_cu,
			frame_info *frame, int addr_size,
			std::vector<value *> *init_values,
			const struct property_addr_info *addr_info,
			struct type *type = nullptr,
			struct type *subobj_type = nullptr,
			LONGEST subobj_offset = 0);

/* Return the address type used of the ARCH architecture and
   ADDR_SIZE is expected size of the type.  */
type *address_type (gdbarch *arch, int addr_size);

/* Return the value of register number REG (a DWARF register number),
   read as an address in a given FRAME.  */
CORE_ADDR read_addr_from_reg (frame_info *frame, int reg);

/* Check that the current operator is either at the end of an
   expression, or that it is followed by a composition operator or by
   DW_OP_GNU_uninit (which should terminate the expression).  */
void dwarf_expr_require_composition (const gdb_byte *, const gdb_byte *,
				     const char *);

/* If <BUF..BUF_END] contains DW_FORM_block* with single DW_OP_reg* return the
   DWARF register number.  Otherwise return -1.  */
int dwarf_block_to_dwarf_reg (const gdb_byte *buf, const gdb_byte *buf_end);

/* If <BUF..BUF_END] contains DW_FORM_block* with just DW_OP_breg*(0) and
   DW_OP_deref* return the DWARF register number.  Otherwise return -1.
   DEREF_SIZE_RETURN contains -1 for DW_OP_deref; otherwise it contains the
   size from DW_OP_deref_size.  */
int dwarf_block_to_dwarf_reg_deref (const gdb_byte *buf,
				    const gdb_byte *buf_end,
				    CORE_ADDR *deref_size_return);

/* If <BUF..BUF_END] contains DW_FORM_block* with single DW_OP_fbreg(X) fill
   in FB_OFFSET_RETURN with the X offset and return 1.  Otherwise return 0.  */
int dwarf_block_to_fb_offset (const gdb_byte *buf, const gdb_byte *buf_end,
			      CORE_ADDR *fb_offset_return);

/* If <BUF..BUF_END] contains DW_FORM_block* with single DW_OP_bregSP(X) fill
   in SP_OFFSET_RETURN with the X offset and return 1.  Otherwise return 0.
   The matched SP register number depends on GDBARCH.  */
int dwarf_block_to_sp_offset (struct gdbarch *gdbarch, const gdb_byte *buf,
			      const gdb_byte *buf_end,
			      CORE_ADDR *sp_offset_return);

/* Wrappers around the leb128 reader routines to simplify them for our
   purposes.  */

static inline const gdb_byte *
gdb_read_uleb128 (const gdb_byte *buf, const gdb_byte *buf_end,
		  uint64_t *r)
{
  size_t bytes_read = read_uleb128_to_uint64 (buf, buf_end, r);

  if (bytes_read == 0)
    return NULL;
  return buf + bytes_read;
}

static inline const gdb_byte *
gdb_read_sleb128 (const gdb_byte *buf, const gdb_byte *buf_end,
		  int64_t *r)
{
  size_t bytes_read = read_sleb128_to_int64 (buf, buf_end, r);

  if (bytes_read == 0)
    return NULL;
  return buf + bytes_read;
}

static inline const gdb_byte *
gdb_skip_leb128 (const gdb_byte *buf, const gdb_byte *buf_end)
{
  size_t bytes_read = skip_leb128 (buf, buf_end);

  if (bytes_read == 0)
    return NULL;
  return buf + bytes_read;
}

/* Helper to read a uleb128 value or throw an error.  */
extern const gdb_byte *safe_read_uleb128 (const gdb_byte *buf,
					  const gdb_byte *buf_end,
					  uint64_t *r);

/* Helper to read a uleb128 value or throw an error.  */
extern const gdb_byte *safe_read_sleb128 (const gdb_byte *buf,
					  const gdb_byte *buf_end,
					  int64_t *r);

/* Helper to skip a leb128 value or throw an error.  */
extern const gdb_byte *safe_skip_leb128 (const gdb_byte *buf,
					 const gdb_byte *buf_end);

#endif /* dwarf2expr.h */
