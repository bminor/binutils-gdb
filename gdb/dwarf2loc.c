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

#include "elf/dwarf2.h"
#include "dwarf2expr.h"
#include "dwarf2loc.h"

#include "gdb_string.h"

#ifndef DWARF2_REG_TO_REGNUM
#define DWARF2_REG_TO_REGNUM(REG) (REG)
#endif

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
dwarf_expr_read_reg (void *baton, int regnum, enum lval_type *lvalp,
		     CORE_ADDR *addrp)
{
  CORE_ADDR result;
  struct dwarf_expr_baton *debaton = (struct dwarf_expr_baton *) baton;
  char *buf = (char *) alloca (MAX_REGISTER_RAW_SIZE);
  int optimized, realnum;

  frame_register (debaton->frame, DWARF2_REG_TO_REGNUM (regnum),
		  &optimized, lvalp, addrp, &realnum, buf);
  result = extract_address (buf, REGISTER_RAW_SIZE (regnum));

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
  struct symbol *framefunc;
  struct dwarf2_locexpr_baton *symbaton;
  struct dwarf_expr_baton *debaton = (struct dwarf_expr_baton *) baton;
  framefunc = get_frame_function (debaton->frame);
  symbaton = SYMBOL_LOCATION_BATON (framefunc);
  *start = symbaton->data;
  *length = symbaton->size;
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

void
locexpr_tracepoint_var_ref (struct symbol * symbol, struct agent_expr * ax,
			    struct axs_value * value)
{
  struct dwarf2_locexpr_baton *dlbaton = SYMBOL_LOCATION_BATON (symbol);

  if (dlbaton->size == 0)
    error ("Symbol \"%s\" has been optimized out.",
	   SYMBOL_PRINT_NAME (symbol));

  if (dlbaton->size == 1
      && dlbaton->data[0] >= DW_OP_reg0
      && dlbaton->data[0] <= DW_OP_reg31)
    {
      value->kind = axs_lvalue_register;
      value->u.reg = dlbaton->data[0] - DW_OP_reg0;
    }
  else if (dlbaton->data[0] == DW_OP_regx)
    {
      ULONGEST reg;
      read_uleb128 (dlbaton->data + 1, dlbaton->data + dlbaton->size,
		    &reg);
      value->kind = axs_lvalue_register;
      value->u.reg = reg;
    }
  else if (dlbaton->data[0] == DW_OP_fbreg)
    {
      /* And this is worse than just minimal; we should honor the frame base
	 as above.  */
      int frame_reg;
      LONGEST frame_offset;
      unsigned char *buf_end;

      buf_end = read_sleb128 (dlbaton->data + 1, dlbaton->data + dlbaton->size,
			      &frame_offset);
      if (buf_end != dlbaton->data + dlbaton->size)
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

/* The set of location functions used with the DWARF-2 expression
   evaluator.  */
struct location_funcs dwarf2_locexpr_funcs = {
  locexpr_read_variable,
  locexpr_read_needs_frame,
  locexpr_describe_location,
  locexpr_tracepoint_var_ref
};
