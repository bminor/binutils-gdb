/* DWARF 2 location expression support for GDB.
   Copyright 2003 Free Software Foundation, Inc.
   Contributed by Daniel Jacobowitz, MontaVista Software, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "ui-out.h"
#include "value.h"
#include "frame.h"
#include "gdbcore.h"
#include "target.h"
#include "inferior.h"
#include "ax.h"
#include "ax-gdb.h"
#include "regcache.h"

#include "elf/dwarf2.h"
#include "dwarf2expr.h"
#include "dwarf2loc.h"

#include "gdb_string.h"

#ifndef DWARF2_REG_TO_REGNUM
#define DWARF2_REG_TO_REGNUM(REG) (REG)
#endif

/* A helper function for dealing with location lists.  Given a
   symbol baton (BATON) and a pc value (PC), find the appropriate
   location expression, set *LOCEXPR_LENGTH, and return a pointer
   to the beginning of the expression.  Returns NULL on failure.

   For now, only return the first matching location expression; there
   can be more than one in the list.  */

static char *
find_location_expression (struct dwarf2_loclist_baton *baton,
			  int *locexpr_length, CORE_ADDR pc)
{
  CORE_ADDR base_address = baton->base_address;
  CORE_ADDR low, high;
  char *loc_ptr, *buf_end;
  unsigned int addr_size = TARGET_ADDR_BIT / TARGET_CHAR_BIT, length;
  CORE_ADDR base_mask = ~(~(CORE_ADDR)1 << (addr_size * 8 - 1));

  loc_ptr = baton->data;
  buf_end = baton->data + baton->size;

  while (1)
    {
      low = dwarf2_read_address (loc_ptr, buf_end, &length);
      loc_ptr += length;
      high = dwarf2_read_address (loc_ptr, buf_end, &length);
      loc_ptr += length;

      /* An end-of-list entry.  */
      if (low == 0 && high == 0)
	return NULL;

      /* A base-address-selection entry.  */
      if ((low & base_mask) == base_mask)
	{
	  base_address = high;
	  continue;
	}

      /* Otherwise, a location expression entry.  */
      low += base_address;
      high += base_address;

      length = extract_unsigned_integer (loc_ptr, 2);
      loc_ptr += 2;

      if (pc >= low && pc < high)
	{
	  *locexpr_length = length;
	  return loc_ptr;
	}

      loc_ptr += length;
    }
}

/* This is the baton used when performing dwarf2 expression
   evaluation.  */
struct dwarf_expr_baton
{
  struct frame_info *frame;
  struct objfile *objfile;
};

/* Helper functions for dwarf2_evaluate_loc_desc.  */

/* Using the frame specified in BATON, read register REGNUM.  The lval
   type will be returned in LVALP, and for lval_memory the register
   save address will be returned in ADDRP.  */
static CORE_ADDR
dwarf_expr_read_reg (void *baton, int dwarf_regnum, enum lval_type *lvalp,
		     CORE_ADDR *addrp)
{
  struct dwarf_expr_baton *debaton = (struct dwarf_expr_baton *) baton;
  CORE_ADDR result;
  char *buf;
  int optimized, regnum, realnum, regsize;

  regnum = DWARF2_REG_TO_REGNUM (dwarf_regnum);
  regsize = register_size (current_gdbarch, regnum);
  buf = (char *) alloca (regsize);

  frame_register (debaton->frame, regnum, &optimized, lvalp, addrp, &realnum,
		  buf);
  result = extract_address (buf, regsize);

  return result;
}

/* Read memory at ADDR (length LEN) into BUF.  */

static void
dwarf_expr_read_mem (void *baton, char *buf, CORE_ADDR addr, size_t len)
{
  read_memory (addr, buf, len);
}

/* Using the frame specified in BATON, find the location expression
   describing the frame base.  Return a pointer to it in START and
   its length in LENGTH.  */
static void
dwarf_expr_frame_base (void *baton, unsigned char **start, size_t * length)
{
  /* FIXME: cagney/2003-03-26: This code should be using
     get_frame_base_address(), and then implement a dwarf2 specific
     this_base method.  */
  struct symbol *framefunc;
  struct dwarf_expr_baton *debaton = (struct dwarf_expr_baton *) baton;

  framefunc = get_frame_function (debaton->frame);

  if (SYMBOL_LOCATION_FUNCS (framefunc) == &dwarf2_loclist_funcs)
    {
      struct dwarf2_loclist_baton *symbaton;
      symbaton = SYMBOL_LOCATION_BATON (framefunc);
      *start = find_location_expression (symbaton, length,
					 get_frame_pc (debaton->frame));
    }
  else
    {
      struct dwarf2_locexpr_baton *symbaton;
      symbaton = SYMBOL_LOCATION_BATON (framefunc);
      *length = symbaton->size;
      *start = symbaton->data;
    }

  if (*start == NULL)
    error ("Could not find the frame base for \"%s\".",
	   SYMBOL_NATURAL_NAME (framefunc));
}

/* Using the objfile specified in BATON, find the address for the
   current thread's thread-local storage with offset OFFSET.  */
static CORE_ADDR
dwarf_expr_tls_address (void *baton, CORE_ADDR offset)
{
  struct dwarf_expr_baton *debaton = (struct dwarf_expr_baton *) baton;
  CORE_ADDR addr;

  if (target_get_thread_local_address_p ())
    addr = target_get_thread_local_address (inferior_ptid,
					    debaton->objfile,
					    offset);
  else
    error ("Cannot find thread-local variables on this target");

  return addr;
}

/* Evaluate a location description, starting at DATA and with length
   SIZE, to find the current location of variable VAR in the context
   of FRAME.  */
static struct value *
dwarf2_evaluate_loc_desc (struct symbol *var, struct frame_info *frame,
			  unsigned char *data, unsigned short size,
			  struct objfile *objfile)
{
  CORE_ADDR result;
  struct value *retval;
  struct dwarf_expr_baton baton;
  struct dwarf_expr_context *ctx;

  if (size == 0)
    {
      retval = allocate_value (SYMBOL_TYPE (var));
      VALUE_LVAL (retval) = not_lval;
      VALUE_OPTIMIZED_OUT (retval) = 1;
    }

  baton.frame = frame;
  baton.objfile = objfile;

  ctx = new_dwarf_expr_context ();
  ctx->baton = &baton;
  ctx->read_reg = dwarf_expr_read_reg;
  ctx->read_mem = dwarf_expr_read_mem;
  ctx->get_frame_base = dwarf_expr_frame_base;
  ctx->get_tls_address = dwarf_expr_tls_address;

  dwarf_expr_eval (ctx, data, size);

  retval = allocate_value (SYMBOL_TYPE (var));
  VALUE_BFD_SECTION (retval) = SYMBOL_BFD_SECTION (var);

  if (ctx->in_reg)
    {
      store_unsigned_integer (VALUE_CONTENTS_RAW (retval),
			      TYPE_LENGTH (SYMBOL_TYPE (var)),
			      dwarf_expr_fetch (ctx, 0));
      VALUE_LVAL (retval) = lval_register;
      VALUE_REGNO (retval) = ctx->regnum;
    }
  else
    {
      result = dwarf_expr_fetch (ctx, 0);
      VALUE_LVAL (retval) = lval_memory;
      VALUE_LAZY (retval) = 1;
      VALUE_ADDRESS (retval) = result;
    }

  free_dwarf_expr_context (ctx);

  return retval;
}





/* Helper functions and baton for dwarf2_loc_desc_needs_frame.  */

struct needs_frame_baton
{
  int needs_frame;
};

/* Reads from registers do require a frame.  */
static CORE_ADDR
needs_frame_read_reg (void *baton, int regnum, enum lval_type *lvalp,
			    CORE_ADDR *addrp)
{
  struct needs_frame_baton *nf_baton = baton;
  nf_baton->needs_frame = 1;
  return 1;
}

/* Reads from memory do not require a frame.  */
static void
needs_frame_read_mem (void *baton, char *buf, CORE_ADDR addr, size_t len)
{
  memset (buf, 0, len);
}

/* Frame-relative accesses do require a frame.  */
static void
needs_frame_frame_base (void *baton, unsigned char **start, size_t * length)
{
  static char lit0 = DW_OP_lit0;
  struct needs_frame_baton *nf_baton = baton;

  *start = &lit0;
  *length = 1;

  nf_baton->needs_frame = 1;
}

/* Thread-local accesses do require a frame.  */
static CORE_ADDR
needs_frame_tls_address (void *baton, CORE_ADDR offset)
{
  struct needs_frame_baton *nf_baton = baton;
  nf_baton->needs_frame = 1;
  return 1;
}

/* Return non-zero iff the location expression at DATA (length SIZE)
   requires a frame to evaluate.  */

static int
dwarf2_loc_desc_needs_frame (unsigned char *data, unsigned short size)
{
  struct needs_frame_baton baton;
  struct dwarf_expr_context *ctx;

  baton.needs_frame = 0;

  ctx = new_dwarf_expr_context ();
  ctx->baton = &baton;
  ctx->read_reg = needs_frame_read_reg;
  ctx->read_mem = needs_frame_read_mem;
  ctx->get_frame_base = needs_frame_frame_base;
  ctx->get_tls_address = needs_frame_tls_address;

  dwarf_expr_eval (ctx, data, size);

  free_dwarf_expr_context (ctx);

  return baton.needs_frame;
}

static void
dwarf2_tracepoint_var_ref (struct symbol * symbol, struct agent_expr * ax,
			   struct axs_value * value, unsigned char *data,
			   int size)
{
  if (size == 0)
    error ("Symbol \"%s\" has been optimized out.",
	   SYMBOL_PRINT_NAME (symbol));

  if (size == 1
      && data[0] >= DW_OP_reg0
      && data[0] <= DW_OP_reg31)
    {
      value->kind = axs_lvalue_register;
      value->u.reg = data[0] - DW_OP_reg0;
    }
  else if (data[0] == DW_OP_regx)
    {
      ULONGEST reg;
      read_uleb128 (data + 1, data + size, &reg);
      value->kind = axs_lvalue_register;
      value->u.reg = reg;
    }
  else if (data[0] == DW_OP_fbreg)
    {
      /* And this is worse than just minimal; we should honor the frame base
	 as above.  */
      int frame_reg;
      LONGEST frame_offset;
      unsigned char *buf_end;

      buf_end = read_sleb128 (data + 1, data + size, &frame_offset);
      if (buf_end != data + size)
	error ("Unexpected opcode after DW_OP_fbreg for symbol \"%s\".",
	       SYMBOL_PRINT_NAME (symbol));

      TARGET_VIRTUAL_FRAME_POINTER (ax->scope, &frame_reg, &frame_offset);
      ax_reg (ax, frame_reg);
      ax_const_l (ax, frame_offset);
      ax_simple (ax, aop_add);

      ax_const_l (ax, frame_offset);
      ax_simple (ax, aop_add);
      value->kind = axs_lvalue_memory;
    }
  else
    error ("Unsupported DWARF opcode in the location of \"%s\".",
	   SYMBOL_PRINT_NAME (symbol));
}

/* Return the value of SYMBOL in FRAME using the DWARF-2 expression
   evaluator to calculate the location.  */
static struct value *
locexpr_read_variable (struct symbol *symbol, struct frame_info *frame)
{
  struct dwarf2_locexpr_baton *dlbaton = SYMBOL_LOCATION_BATON (symbol);
  struct value *val;
  val = dwarf2_evaluate_loc_desc (symbol, frame, dlbaton->data, dlbaton->size,
				  dlbaton->objfile);

  return val;
}

/* Return non-zero iff we need a frame to evaluate SYMBOL.  */
static int
locexpr_read_needs_frame (struct symbol *symbol)
{
  struct dwarf2_locexpr_baton *dlbaton = SYMBOL_LOCATION_BATON (symbol);
  return dwarf2_loc_desc_needs_frame (dlbaton->data, dlbaton->size);
}

/* Print a natural-language description of SYMBOL to STREAM.  */
static int
locexpr_describe_location (struct symbol *symbol, struct ui_file *stream)
{
  /* FIXME: be more extensive.  */
  struct dwarf2_locexpr_baton *dlbaton = SYMBOL_LOCATION_BATON (symbol);

  if (dlbaton->size == 1
      && dlbaton->data[0] >= DW_OP_reg0
      && dlbaton->data[0] <= DW_OP_reg31)
    {
      int regno = DWARF2_REG_TO_REGNUM (dlbaton->data[0] - DW_OP_reg0);
      fprintf_filtered (stream,
			"a variable in register %s", REGISTER_NAME (regno));
      return 1;
    }

  fprintf_filtered (stream,
		    "a variable with complex or multiple locations (DWARF2)");
  return 1;
}


/* Describe the location of SYMBOL as an agent value in VALUE, generating
   any necessary bytecode in AX.

   NOTE drow/2003-02-26: This function is extremely minimal, because
   doing it correctly is extremely complicated and there is no
   publicly available stub with tracepoint support for me to test
   against.  When there is one this function should be revisited.  */

static void
locexpr_tracepoint_var_ref (struct symbol * symbol, struct agent_expr * ax,
			    struct axs_value * value)
{
  struct dwarf2_locexpr_baton *dlbaton = SYMBOL_LOCATION_BATON (symbol);

  dwarf2_tracepoint_var_ref (symbol, ax, value, dlbaton->data, dlbaton->size);
}

/* The set of location functions used with the DWARF-2 expression
   evaluator.  */
struct location_funcs dwarf2_locexpr_funcs = {
  locexpr_read_variable,
  locexpr_read_needs_frame,
  locexpr_describe_location,
  locexpr_tracepoint_var_ref
};


/* Wrapper functions for location lists.  These generally find
   the appropriate location expression and call something above.  */

/* Return the value of SYMBOL in FRAME using the DWARF-2 expression
   evaluator to calculate the location.  */
static struct value *
loclist_read_variable (struct symbol *symbol, struct frame_info *frame)
{
  struct dwarf2_loclist_baton *dlbaton = SYMBOL_LOCATION_BATON (symbol);
  struct value *val;
  unsigned char *data;
  int size;

  data = find_location_expression (dlbaton, &size,
				   frame ? get_frame_pc (frame) : 0);
  if (data == NULL)
    error ("Variable \"%s\" is not available.", SYMBOL_NATURAL_NAME (symbol));

  val = dwarf2_evaluate_loc_desc (symbol, frame, data, size, dlbaton->objfile);

  return val;
}

/* Return non-zero iff we need a frame to evaluate SYMBOL.  */
static int
loclist_read_needs_frame (struct symbol *symbol)
{
  /* If there's a location list, then assume we need to have a frame
     to choose the appropriate location expression.  With tracking of
     global variables this is not necessarily true, but such tracking
     is disabled in GCC at the moment until we figure out how to
     represent it.  */

  return 1;
}

/* Print a natural-language description of SYMBOL to STREAM.  */
static int
loclist_describe_location (struct symbol *symbol, struct ui_file *stream)
{
  /* FIXME: Could print the entire list of locations.  */
  fprintf_filtered (stream, "a variable with multiple locations");
  return 1;
}

/* Describe the location of SYMBOL as an agent value in VALUE, generating
   any necessary bytecode in AX.  */
static void
loclist_tracepoint_var_ref (struct symbol * symbol, struct agent_expr * ax,
			    struct axs_value * value)
{
  struct dwarf2_loclist_baton *dlbaton = SYMBOL_LOCATION_BATON (symbol);
  unsigned char *data;
  int size;

  data = find_location_expression (dlbaton, &size, ax->scope);
  if (data == NULL)
    error ("Variable \"%s\" is not available.", SYMBOL_NATURAL_NAME (symbol));

  dwarf2_tracepoint_var_ref (symbol, ax, value, data, size);
}

/* The set of location functions used with the DWARF-2 expression
   evaluator and location lists.  */
struct location_funcs dwarf2_loclist_funcs = {
  loclist_read_variable,
  loclist_read_needs_frame,
  loclist_describe_location,
  loclist_tracepoint_var_ref
};
