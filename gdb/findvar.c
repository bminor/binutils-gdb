/* Find a variable's value in memory, for GDB, the GNU debugger.
   Copyright (C) 1986, 1987 Free Software Foundation, Inc.

GDB is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GDB General Public License for full details.

Everyone is granted permission to copy, modify and redistribute GDB,
but only under the conditions described in the GDB General Public
License.  A copy of this license is supposed to have been given to you
along with GDB so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

In other words, go ahead and share GDB, but don't try to stop
anyone else from sharing it farther.  Help stamp out software hoarding!
*/

#include "defs.h"
#include "initialize.h"
#include "param.h"
#include "symtab.h"
#include "frame.h"
#include "value.h"

CORE_ADDR read_register ();

START_FILE

/* Return the address in which frame FRAME's value of register REGNUM
   has been saved in memory.  Or return zero if it has not been saved.
   If REGNUM specifies the SP, the value we return is actually
   the SP value, not an address where it was saved.  */

static CORE_ADDR
find_saved_register (frame, regnum)
     FRAME frame;
     int regnum;
{
  struct frame_info fi;
  struct frame_saved_regs saved_regs;

  register FRAME frame1 = 0;
  register CORE_ADDR addr = 0;

  while (1)
    {
      QUIT;
      fi = get_prev_frame_info (frame1);
      if (fi.frame == 0 || fi.frame == frame)
	break;
      get_frame_saved_regs (&fi, &saved_regs);
      if (saved_regs.regs[regnum])
	addr = saved_regs.regs[regnum];
      frame1 = fi.frame;
    }

  return addr;
}

/* Copy the bytes of register REGNUM, relative to the current stack frame,
   into our memory at MYADDR.
   The number of bytes copied is REGISTER_RAW_SIZE (REGNUM).  */

void
read_relative_register_raw_bytes (regnum, myaddr)
     int regnum;
     char *myaddr;
{
  register CORE_ADDR addr;

  if (regnum == FP_REGNUM)
    {
      bcopy (&selected_frame, myaddr, sizeof (CORE_ADDR));
      return;
    }

  addr = find_saved_register (selected_frame, regnum);

  if (addr)
    {
      if (regnum == SP_REGNUM)
	{
	  CORE_ADDR buffer = addr;
	  bcopy (&buffer, myaddr, sizeof (CORE_ADDR));
	}
      else
	read_memory (addr, myaddr, REGISTER_RAW_SIZE (regnum));
      return;
    }
  read_register_bytes (REGISTER_BYTE (regnum),
		       myaddr, REGISTER_RAW_SIZE (regnum));
}

/* Return a `value' with the contents of register REGNUM
   in its virtual format, with the type specified by
   REGISTER_VIRTUAL_TYPE.  */

value
value_of_register (regnum)
     int regnum;
{
  register CORE_ADDR addr = find_saved_register (selected_frame, regnum);
  register value val;
  char raw_buffer[MAX_REGISTER_RAW_SIZE];
  char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];

  if (addr)
    {
      if (regnum == SP_REGNUM)
	return value_from_long (builtin_type_int, addr);
      read_memory (addr, raw_buffer, REGISTER_RAW_SIZE (regnum));
    }
  else
    read_register_bytes (REGISTER_BYTE (regnum), raw_buffer,
			 REGISTER_RAW_SIZE (regnum));

  REGISTER_CONVERT_TO_VIRTUAL (regnum, raw_buffer, virtual_buffer);
  val = allocate_value (REGISTER_VIRTUAL_TYPE (regnum));
  bcopy (virtual_buffer, VALUE_CONTENTS (val), REGISTER_VIRTUAL_SIZE (regnum));
  VALUE_LVAL (val) = addr ? lval_memory : lval_register;
  VALUE_ADDRESS (val) = addr ? addr : REGISTER_BYTE (regnum);
  VALUE_REGNO (val) = regnum;
  return val;
}

/* Low level examining and depositing of registers.

   Note that you must call `fetch_registers' once
   before examining or depositing any registers.  */

char registers[REGISTER_BYTES];

/* Copy LEN bytes of consecutive data from registers
   starting with the REGBYTE'th byte of register data
   into memory at MYADDR.  */

read_register_bytes (regbyte, myaddr, len)
     int regbyte;
     char *myaddr;
     int len;
{
  bcopy (&registers[regbyte], myaddr, len);
}

/* Copy LEN bytes of consecutive data from memory at MYADDR
   into registers starting with the REGBYTE'th byte of register data.  */

write_register_bytes (regbyte, myaddr, len)
     int regbyte;
     char *myaddr;
     int len;
{
  bcopy (myaddr, &registers[regbyte], len);
  if (have_inferior_p ())
    store_inferior_registers (-1);
}

/* Return the contents of register REGNO,
   regarding it as an integer.  */

CORE_ADDR
read_register (regno)
     int regno;
{
  /* This loses when REGISTER_RAW_SIZE (regno) != sizeof (int) */
  return *(int *) &registers[REGISTER_BYTE (regno)];
}

/* Store VALUE in the register number REGNO, regarded as an integer.  */

void
write_register (regno, val)
     int regno, val;
{
  /* This loses when REGISTER_RAW_SIZE (regno) != sizeof (int) */
  *(int *) &registers[REGISTER_BYTE (regno)] = val;

  if (have_inferior_p ())
    store_inferior_registers (regno);
}

/* Record that register REGNO contains VAL.
   This is used when the value is obtained from the inferior or core dump,
   so there is no need to store the value there.  */

void
supply_register (regno, val)
     int regno;
     char *val;
{
  bcopy (val, &registers[REGISTER_BYTE (regno)], REGISTER_RAW_SIZE (regno));
}

/* Given a struct symbol for a variable,
   and a stack frame address, read the value of the variable
   and return a (pointer to a) struct value containing the value.  */

value
read_var_value (var, frame)
     register struct symbol *var;
     FRAME frame;
{
  register value v;

  struct frame_info fi;

  struct type *type = SYMBOL_TYPE (var);
  register CORE_ADDR addr = 0;
  int val = SYMBOL_VALUE (var);
  register int len;

  v = allocate_value (type);
  VALUE_LVAL (v) = lval_memory;	/* The most likely possibility.  */
  len = TYPE_LENGTH (type);

  if (frame == 0) frame = selected_frame;

  switch (SYMBOL_CLASS (var))
    {
    case LOC_CONST:
    case LOC_LABEL:
      bcopy (&val, VALUE_CONTENTS (v), len);
      VALUE_LVAL (v) = not_lval;
      return v;

    case LOC_CONST_BYTES:
      bcopy (val, VALUE_CONTENTS (v), len);
      VALUE_LVAL (v) = not_lval;
      return v;

    case LOC_STATIC:
      addr = val;
      break;

    case LOC_ARG:
      fi = get_frame_info (frame);
      addr = val + FRAME_ARGS_ADDRESS (fi);
      break;

    case LOC_LOCAL:
      fi = get_frame_info (frame);
      addr = val + FRAME_LOCALS_ADDRESS (fi);
      break;

    case LOC_TYPEDEF:
      error ("Cannot look up value of a typedef");

    case LOC_BLOCK:
      VALUE_ADDRESS (v) = BLOCK_START (SYMBOL_BLOCK_VALUE (var));
      return v;

    case LOC_REGISTER:
      {
	char raw_buffer[MAX_REGISTER_RAW_SIZE];
	char virtual_buffer[MAX_REGISTER_VIRTUAL_SIZE];

	VALUE_REGNO (v) = val;

	/* Locate the register's contents in a real register or in core;
	   read the data in raw format.  */

	addr = find_saved_register (frame, val);
	if (addr == 0)
	  {
	    /* Value is really in a register.  */

	    VALUE_LVAL (v) = lval_register;
	    VALUE_ADDRESS (v) = REGISTER_BYTE (val);

	    read_register_bytes (REGISTER_BYTE (val),
				 raw_buffer, REGISTER_RAW_SIZE (val));
	  }
	else
	  {
	    /* Value was in a register that has been saved in memory.  */

	    read_memory (addr, raw_buffer, REGISTER_RAW_SIZE (val));
	    VALUE_ADDRESS (v) = addr;
	  }

	/* Convert the raw contents to virtual contents.
	   (Just copy them if the formats are the same.)  */

	REGISTER_CONVERT_TO_VIRTUAL (val, raw_buffer, virtual_buffer);

	if (REGISTER_CONVERTIBLE (val))
	  {
	    /* When the raw and virtual formats differ, the virtual format
	       corresponds to a specific data type.  If we want that type,
	       copy the data into the value.
	       Otherwise, do a type-conversion.  */

	    if (type != REGISTER_VIRTUAL_TYPE (val))
	      {
		/* eg a variable of type `float' in a 68881 register
		   with raw type `extended' and virtual type `double'.
		   Fetch it as a `double' and then convert to `float'.  */
		v = allocate_value (REGISTER_VIRTUAL_TYPE (val));
		bcopy (virtual_buffer, VALUE_CONTENTS (v), len);
		v = value_cast (type, v);
	      }
	    else
	      bcopy (virtual_buffer, VALUE_CONTENTS (v), len);
	  }
	else
	  {
	    /* Raw and virtual formats are the same for this register.  */

	    union { int i; char c; } test;
	    /* If we want less than the full size, we need to
	       test for a big-endian or little-endian machine.  */
	    test.i = 1;
	    if (test.c != 1 && len < REGISTER_RAW_SIZE (val))
	      {
		/* Big-endian, and we want less than full size.  */
		VALUE_OFFSET (v) = REGISTER_RAW_SIZE (val) - len;
	      }

	    bcopy (virtual_buffer + VALUE_OFFSET (v),
		   VALUE_CONTENTS (v), len);
	  }

	return v;
      }
    }

  read_memory (addr, VALUE_CONTENTS (v), len);
  VALUE_ADDRESS (v) = addr;
  return v;
}

/* Given a struct symbol for a variable,
   and a stack frame address,
   return a (pointer to a) struct value containing the variable's address.  */

value
locate_var_value (var, frame)
     register struct symbol *var;
     FRAME frame;
{
  register CORE_ADDR addr = 0;
  int val = SYMBOL_VALUE (var);
  struct frame_info fi;
  struct type *type = SYMBOL_TYPE (var);

  if (frame == 0) frame = selected_frame;

  switch (SYMBOL_CLASS (var))
    {
    case LOC_CONST:
    case LOC_CONST_BYTES:
      error ("Address requested for identifier \"%s\" which is a constant.",
	     SYMBOL_NAME (var));

    case LOC_REGISTER:
      addr = find_saved_register (frame, val);
      if (addr != 0)
	{
	  union { int i; char c; } test;
	  int len = TYPE_LENGTH (type);
	  /* If var is less than the full size of register, we need to
	     test for a big-endian or little-endian machine.  */
	  test.i = 1;
	  if (test.c != 1 && len < REGISTER_RAW_SIZE (val))
	    /* Big-endian, and we want less than full size.  */
	    addr += REGISTER_RAW_SIZE (val) - len;
	  break;
	}
      error ("Address requested for identifier \"%s\" which is in a register.",
	     SYMBOL_NAME (var));

    case LOC_STATIC:
    case LOC_LABEL:
      addr = val;
      break;

    case LOC_ARG:
      fi = get_frame_info (frame);
      addr = val + FRAME_ARGS_ADDRESS (fi);
      break;

    case LOC_LOCAL:
      fi = get_frame_info (frame);
      addr = val + FRAME_LOCALS_ADDRESS (fi);
      break;

    case LOC_TYPEDEF:
      error ("Address requested for identifier \"%s\" which is a typedef.",
	     SYMBOL_NAME (var));

    case LOC_BLOCK:
      addr = BLOCK_START (SYMBOL_BLOCK_VALUE (var));
      break;
    }

  return value_cast (lookup_pointer_type (type),
		     value_from_long (builtin_type_long, addr));
}

static
initialize ()
{}

END_FILE
