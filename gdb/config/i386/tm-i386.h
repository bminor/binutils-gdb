/* Macro definitions for GDB on an Intel i[345]86.
   Copyright 1995, 1996, 1998, 1999, 2000, 2001
   Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef TM_I386_H
#define TM_I386_H 1

#define GDB_MULTI_ARCH GDB_MULTI_ARCH_PARTIAL

#include "regcache.h"

/* Forward declarations for prototypes.  */
struct frame_info;
struct frame_saved_regs;
struct value;
struct type;

/* Return the GDB type object for the "standard" data type of data in
   register REGNUM.  */

#define REGISTER_VIRTUAL_TYPE(regnum) i386_register_virtual_type (regnum)
extern struct type *i386_register_virtual_type (int regnum);

/* Return true iff register REGNUM's virtual format is different from
   its raw format.  */

#define REGISTER_CONVERTIBLE(regnum) i386_register_convertible (regnum)
extern int i386_register_convertible (int regnum);

/* Convert data from raw format for register REGNUM in buffer FROM to
   virtual format with type TYPE in buffer TO.  */

#define REGISTER_CONVERT_TO_VIRTUAL(regnum, type, from, to) \
  i386_register_convert_to_virtual ((regnum), (type), (from), (to))
extern void i386_register_convert_to_virtual (int regnum, struct type *type,
					      char *from, char *to);

/* Convert data from virtual format with type TYPE in buffer FROM to
   raw format for register REGNUM in buffer TO.  */

#define REGISTER_CONVERT_TO_RAW(type, regnum, from, to) \
  i386_register_convert_to_raw ((type), (regnum), (from), (to))
extern void i386_register_convert_to_raw (struct type *type, int regnum,
					  char *from, char *to);

/* Print out the i387 floating point state.  */
extern void i387_float_info (void);
#define FLOAT_INFO { i387_float_info (); }


#define PUSH_ARGUMENTS(nargs, args, sp, struct_return, struct_addr) \
  i386_push_arguments ((nargs), (args), (sp), (struct_return), (struct_addr))
extern CORE_ADDR i386_push_arguments (int nargs, struct value **args,
				      CORE_ADDR sp, int struct_return,
				      CORE_ADDR struct_addr);

/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function.  */

#define STORE_STRUCT_RETURN(addr, sp) \
  i386_store_struct_return ((addr), (sp))
extern void i386_store_struct_return (CORE_ADDR addr, CORE_ADDR sp);

/* Extract from an array REGBUF containing the (raw) register state
   a function return value of type TYPE, and copy that, in virtual format,
   into VALBUF.  */

#define DEPRECATED_EXTRACT_RETURN_VALUE(type, regbuf, valbuf) \
  i386_extract_return_value ((type), (regbuf), (valbuf))
extern void i386_extract_return_value (struct type *type, char *regbuf,
				       char *valbuf);

/* Write into the appropriate registers a function return value stored
   in VALBUF of type TYPE, given in virtual format.  */

#define STORE_RETURN_VALUE(type, valbuf) \
  i386_store_return_value ((type), (valbuf))
extern void i386_store_return_value (struct type *type, char *valbuf);

/* Extract from an array REGBUF containing the (raw) register state
   the address in which a function should return its structure value,
   as a CORE_ADDR.  */

#define DEPRECATED_EXTRACT_STRUCT_VALUE_ADDRESS(regbuf) \
  i386_extract_struct_value_address ((regbuf))
extern CORE_ADDR i386_extract_struct_value_address (char *regbuf);


/* Things needed for making the inferior call functions.  */

/* "An argument's size is increased, if necessary, to make it a
   multiple of [32 bit] words.  This may require tail padding,
   depending on the size of the argument" - from the x86 ABI.  */
#define PARM_BOUNDARY 32

/* Push an empty stack frame, to record the current PC, etc.  */

#define PUSH_DUMMY_FRAME { i386_push_dummy_frame (); }

extern void i386_push_dummy_frame (void);

/* Discard from the stack the innermost frame, restoring all registers.  */

#define POP_FRAME  { i386_pop_frame (); }

extern void i386_pop_frame (void);


/* this is 
 *   call 11223344 (32 bit relative)
 *   int3
 */

#define CALL_DUMMY { 0x223344e8, 0xcc11 }

#define CALL_DUMMY_LENGTH 8

#define CALL_DUMMY_START_OFFSET 0	/* Start execution at beginning of dummy */

#define CALL_DUMMY_BREAKPOINT_OFFSET 5

/* Insert the specified number of args and function address
   into a call sequence of the above form stored at DUMMYNAME.  */

#define FIX_CALL_DUMMY(dummyname, pc, fun, nargs, args, type, gcc_p) \
  i386_fix_call_dummy (dummyname, pc, fun, nargs, args, type, gcc_p)
extern void i386_fix_call_dummy (char *dummy, CORE_ADDR pc, CORE_ADDR fun,
				 int nargs, struct value **args,
				 struct type *type, int gcc_p);

/* FIXME: kettenis/2000-06-12: These do not belong here.  */
extern void print_387_control_word (unsigned int);
extern void print_387_status_word (unsigned int);

/* Offset from SP to first arg on stack at first instruction of a function */

#define SP_ARG0 (1 * 4)

#endif /* ifndef TM_I386_H */
