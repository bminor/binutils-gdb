/* Target-dependent code for the x86-64 for GDB, the GNU debugger.

   Copyright 2001, 2002, 2003 Free Software Foundation, Inc.
   Contributed by Jiri Smid, SuSE Labs.

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

#include "defs.h"
#include "inferior.h"
#include "gdbcore.h"
#include "gdbcmd.h"
#include "arch-utils.h"
#include "regcache.h"
#include "symfile.h"
#include "objfiles.h"
#include "x86-64-tdep.h"
#include "dwarf2cfi.h"
#include "gdb_assert.h"

/* Register numbers of various important registers.  */
#define RAX_REGNUM 0
#define RDX_REGNUM 3
#define RDI_REGNUM 5
#define EFLAGS_REGNUM 17
#define ST0_REGNUM 22
#define XMM1_REGNUM  39

struct register_info
{
  int size;
  char *name;
  struct type **type;
};

/* x86_64_register_raw_size_table[i] is the number of bytes of storage in
   GDB's register array occupied by register i.  */
static struct register_info x86_64_register_info_table[] = {
  /*  0 */ {8, "rax", &builtin_type_int64},
  /*  1 */ {8, "rbx", &builtin_type_int64},
  /*  2 */ {8, "rcx", &builtin_type_int64},
  /*  3 */ {8, "rdx", &builtin_type_int64},
  /*  4 */ {8, "rsi", &builtin_type_int64},
  /*  5 */ {8, "rdi", &builtin_type_int64},
  /*  6 */ {8, "rbp", &builtin_type_void_func_ptr},
  /*  7 */ {8, "rsp", &builtin_type_void_func_ptr},
  /*  8 */ {8, "r8", &builtin_type_int64},
  /*  9 */ {8, "r9", &builtin_type_int64},
  /* 10 */ {8, "r10", &builtin_type_int64},
  /* 11 */ {8, "r11", &builtin_type_int64},
  /* 12 */ {8, "r12", &builtin_type_int64},
  /* 13 */ {8, "r13", &builtin_type_int64},
  /* 14 */ {8, "r14", &builtin_type_int64},
  /* 15 */ {8, "r15", &builtin_type_int64},
  /* 16 */ {8, "rip", &builtin_type_void_func_ptr},
  /* 17 */ {4, "eflags", &builtin_type_int32},
  /* 18 */ {4, "ds", &builtin_type_int32},
  /* 19 */ {4, "es", &builtin_type_int32},
  /* 20 */ {4, "fs", &builtin_type_int32},
  /* 21 */ {4, "gs", &builtin_type_int32},
  /* 22 */ {10, "st0", &builtin_type_i387_ext},
  /* 23 */ {10, "st1", &builtin_type_i387_ext},
  /* 24 */ {10, "st2", &builtin_type_i387_ext},
  /* 25 */ {10, "st3", &builtin_type_i387_ext},
  /* 26 */ {10, "st4", &builtin_type_i387_ext},
  /* 27 */ {10, "st5", &builtin_type_i387_ext},
  /* 28 */ {10, "st6", &builtin_type_i387_ext},
  /* 29 */ {10, "st7", &builtin_type_i387_ext},
  /* 30 */ {4, "fctrl", &builtin_type_int32},
  /* 31 */ {4, "fstat", &builtin_type_int32},
  /* 32 */ {4, "ftag", &builtin_type_int32},
  /* 33 */ {4, "fiseg", &builtin_type_int32},
  /* 34 */ {4, "fioff", &builtin_type_int32},
  /* 35 */ {4, "foseg", &builtin_type_int32},
  /* 36 */ {4, "fooff", &builtin_type_int32},
  /* 37 */ {4, "fop", &builtin_type_int32},
  /* 38 */ {16, "xmm0", &builtin_type_v4sf},
  /* 39 */ {16, "xmm1", &builtin_type_v4sf},
  /* 40 */ {16, "xmm2", &builtin_type_v4sf},
  /* 41 */ {16, "xmm3", &builtin_type_v4sf},
  /* 42 */ {16, "xmm4", &builtin_type_v4sf},
  /* 43 */ {16, "xmm5", &builtin_type_v4sf},
  /* 44 */ {16, "xmm6", &builtin_type_v4sf},
  /* 45 */ {16, "xmm7", &builtin_type_v4sf},
  /* 46 */ {16, "xmm8", &builtin_type_v4sf},
  /* 47 */ {16, "xmm9", &builtin_type_v4sf},
  /* 48 */ {16, "xmm10", &builtin_type_v4sf},
  /* 49 */ {16, "xmm11", &builtin_type_v4sf},
  /* 50 */ {16, "xmm12", &builtin_type_v4sf},
  /* 51 */ {16, "xmm13", &builtin_type_v4sf},
  /* 52 */ {16, "xmm14", &builtin_type_v4sf},
  /* 53 */ {16, "xmm15", &builtin_type_v4sf},
  /* 54 */ {4, "mxcsr", &builtin_type_int32}
};

/* This array is a mapping from Dwarf-2 register 
   numbering to GDB's one. Dwarf-2 numbering is 
   defined in x86-64 ABI, section 3.6.  */
static int x86_64_dwarf2gdb_regno_map[] = {
  0, 1, 2, 3,			/* RAX - RDX */
  4, 5, 6, 7,			/* RSI, RDI, RBP, RSP */
  8, 9, 10, 11,			/* R8 - R11 */
  12, 13, 14, 15,		/* R12 - R15 */
  -1,				/* RA - not mapped */
  XMM1_REGNUM - 1, XMM1_REGNUM,	/* XMM0 ... */
  XMM1_REGNUM + 1, XMM1_REGNUM + 2,
  XMM1_REGNUM + 3, XMM1_REGNUM + 4,
  XMM1_REGNUM + 5, XMM1_REGNUM + 6,
  XMM1_REGNUM + 7, XMM1_REGNUM + 8,
  XMM1_REGNUM + 9, XMM1_REGNUM + 10,
  XMM1_REGNUM + 11, XMM1_REGNUM + 12,
  XMM1_REGNUM + 13, XMM1_REGNUM + 14,	/* ... XMM15 */
  ST0_REGNUM + 0, ST0_REGNUM + 1,	/* ST0 ... */
  ST0_REGNUM + 2, ST0_REGNUM + 3,
  ST0_REGNUM + 4, ST0_REGNUM + 5,
  ST0_REGNUM + 6, ST0_REGNUM + 7	/* ... ST7 */
};

static int x86_64_dwarf2gdb_regno_map_length =
  sizeof (x86_64_dwarf2gdb_regno_map) /
  sizeof (x86_64_dwarf2gdb_regno_map[0]);

/* Number of all registers */
#define X86_64_NUM_REGS (sizeof (x86_64_register_info_table) / \
  sizeof (x86_64_register_info_table[0]))

/* Number of general registers.  */
#define X86_64_NUM_GREGS (22)

int x86_64_num_regs = X86_64_NUM_REGS;
int x86_64_num_gregs = X86_64_NUM_GREGS;

/* Did we already print a note about frame pointer?  */
int omit_fp_note_printed = 0;

/* Number of bytes of storage in the actual machine representation for
   register REGNO.  */
int
x86_64_register_raw_size (int regno)
{
  return x86_64_register_info_table[regno].size;
}

/* x86_64_register_byte_table[i] is the offset into the register file of the
   start of register number i.  We initialize this from
   x86_64_register_info_table.  */
int x86_64_register_byte_table[X86_64_NUM_REGS];

/* Index within `registers' of the first byte of the space for register REGNO.  */
int
x86_64_register_byte (int regno)
{
  return x86_64_register_byte_table[regno];
}

/* Return the GDB type object for the "standard" data type of data in
   register N. */
static struct type *
x86_64_register_virtual_type (int regno)
{
  return *x86_64_register_info_table[regno].type;
}

/* x86_64_register_convertible is true if register N's virtual format is
   different from its raw format.  Note that this definition assumes
   that the host supports IEEE 32-bit floats, since it doesn't say
   that SSE registers need conversion.  Even if we can't find a
   counterexample, this is still sloppy.  */
int
x86_64_register_convertible (int regno)
{
  return IS_FP_REGNUM (regno);
}

/* Convert data from raw format for register REGNUM in buffer FROM to
   virtual format with type TYPE in buffer TO.  In principle both
   formats are identical except that the virtual format has two extra
   bytes appended that aren't used.  We set these to zero.  */
void
x86_64_register_convert_to_virtual (int regnum, struct type *type,
				    char *from, char *to)
{
  char buf[12];

  /* We only support floating-point values.  */
  if (TYPE_CODE (type) != TYPE_CODE_FLT)
    {
      warning ("Cannot convert floating-point register value "
	       "to non-floating-point type.");
      memset (to, 0, TYPE_LENGTH (type));
      return;
    }
  /* First add the necessary padding.  */
  memcpy (buf, from, FPU_REG_RAW_SIZE);
  memset (buf + FPU_REG_RAW_SIZE, 0, sizeof buf - FPU_REG_RAW_SIZE);
  /* Convert to TYPE.  This should be a no-op, if TYPE is equivalent
     to the extended floating-point format used by the FPU.  */
  convert_typed_floating (to, type, buf,
			  x86_64_register_virtual_type (regnum));
}

/* Convert data from virtual format with type TYPE in buffer FROM to
   raw format for register REGNUM in buffer TO.  Simply omit the two
   unused bytes.  */

void
x86_64_register_convert_to_raw (struct type *type, int regnum,
				char *from, char *to)
{
  gdb_assert (TYPE_CODE (type) == TYPE_CODE_FLT && TYPE_LENGTH (type) == 12);
  /* Simply omit the two unused bytes.  */
  memcpy (to, from, FPU_REG_RAW_SIZE);
}

/* Dwarf-2 <-> GDB register numbers mapping.  */
int
x86_64_dwarf2_reg_to_regnum (int dw_reg)
{
  if (dw_reg < 0 || dw_reg > x86_64_dwarf2gdb_regno_map_length)
    {
      warning ("Dwarf-2 uses unmapped register #%d\n", dw_reg);
      return dw_reg;
    }

  return x86_64_dwarf2gdb_regno_map[dw_reg];
}

/* This is the variable that is set with "set disassembly-flavour", and
   its legitimate values.  */
static const char att_flavour[] = "att";
static const char intel_flavour[] = "intel";
static const char *valid_flavours[] = {
  att_flavour,
  intel_flavour,
  NULL
};
static const char *disassembly_flavour = att_flavour;

/* Push the return address (pointing to the call dummy) onto the stack
   and return the new value for the stack pointer.  */

static CORE_ADDR
x86_64_push_return_address (CORE_ADDR pc, CORE_ADDR sp)
{
  char buf[8];

  store_unsigned_integer (buf, 8, CALL_DUMMY_ADDRESS ());
  write_memory (sp - 8, buf, 8);
  return sp - 8;
}

static void
x86_64_pop_frame (void)
{
  generic_pop_current_frame (cfi_pop_frame);
}


/* The returning of values is done according to the special algorithm.
   Some types are returned in registers an some (big structures) in memory.
   See ABI for details.
 */

#define MAX_CLASSES 4

enum x86_64_reg_class
{
  X86_64_NO_CLASS,
  X86_64_INTEGER_CLASS,
  X86_64_INTEGERSI_CLASS,
  X86_64_SSE_CLASS,
  X86_64_SSESF_CLASS,
  X86_64_SSEDF_CLASS,
  X86_64_SSEUP_CLASS,
  X86_64_X87_CLASS,
  X86_64_X87UP_CLASS,
  X86_64_MEMORY_CLASS
};

/* Return the union class of CLASS1 and CLASS2.
   See the x86-64 ABI for details.  */

static enum x86_64_reg_class
merge_classes (enum x86_64_reg_class class1, enum x86_64_reg_class class2)
{
  /* Rule #1: If both classes are equal, this is the resulting class.  */
  if (class1 == class2)
    return class1;

  /* Rule #2: If one of the classes is NO_CLASS, the resulting class
     is the other class.  */
  if (class1 == X86_64_NO_CLASS)
    return class2;
  if (class2 == X86_64_NO_CLASS)
    return class1;

  /* Rule #3: If one of the classes is MEMORY, the result is MEMORY.  */
  if (class1 == X86_64_MEMORY_CLASS || class2 == X86_64_MEMORY_CLASS)
    return X86_64_MEMORY_CLASS;

  /* Rule #4: If one of the classes is INTEGER, the result is INTEGER.  */
  if ((class1 == X86_64_INTEGERSI_CLASS && class2 == X86_64_SSESF_CLASS)
      || (class2 == X86_64_INTEGERSI_CLASS && class1 == X86_64_SSESF_CLASS))
    return X86_64_INTEGERSI_CLASS;
  if (class1 == X86_64_INTEGER_CLASS || class1 == X86_64_INTEGERSI_CLASS
      || class2 == X86_64_INTEGER_CLASS || class2 == X86_64_INTEGERSI_CLASS)
    return X86_64_INTEGER_CLASS;

  /* Rule #5: If one of the classes is X87 or X87UP class, MEMORY is used.  */
  if (class1 == X86_64_X87_CLASS || class1 == X86_64_X87UP_CLASS
      || class2 == X86_64_X87_CLASS || class2 == X86_64_X87UP_CLASS)
    return X86_64_MEMORY_CLASS;

  /* Rule #6: Otherwise class SSE is used.  */
  return X86_64_SSE_CLASS;
}

/* Classify the argument type.  CLASSES will be filled by the register
   class used to pass each word of the operand.  The number of words
   is returned.  In case the parameter should be passed in memory, 0
   is returned.  As a special case for zero sized containers,
   classes[0] will be NO_CLASS and 1 is returned.

   See the x86-64 psABI for details.  */

static int
classify_argument (struct type *type,
		   enum x86_64_reg_class classes[MAX_CLASSES], int bit_offset)
{
  int bytes = TYPE_LENGTH (type);
  int words = (bytes + 8 - 1) / 8;

  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_ARRAY:
    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      {
	int i;
	enum x86_64_reg_class subclasses[MAX_CLASSES];

	/* On x86-64 we pass structures larger than 16 bytes on the stack.  */
	if (bytes > 16)
	  return 0;

	for (i = 0; i < words; i++)
	  classes[i] = X86_64_NO_CLASS;

	/* Zero sized arrays or structures are NO_CLASS.  We return 0
	   to signalize memory class, so handle it as special case.  */
	if (!words)
	  {
	    classes[0] = X86_64_NO_CLASS;
	    return 1;
	  }
	switch (TYPE_CODE (type))
	  {
	  case TYPE_CODE_STRUCT:
	    {
	      int j;
	      for (j = 0; j < TYPE_NFIELDS (type); ++j)
		{
		  int num = classify_argument (TYPE_FIELDS (type)[j].type,
					       subclasses,
					       (TYPE_FIELDS (type)[j].loc.
						bitpos + bit_offset) % 256);
		  if (!num)
		    return 0;
		  for (i = 0; i < num; i++)
		    {
		      int pos =
			(TYPE_FIELDS (type)[j].loc.bitpos +
			 bit_offset) / 8 / 8;
		      classes[i + pos] =
			merge_classes (subclasses[i], classes[i + pos]);
		    }
		}
	    }
	    break;
	  case TYPE_CODE_ARRAY:
	    {
	      int num;

	      num = classify_argument (TYPE_TARGET_TYPE (type),
				       subclasses, bit_offset);
	      if (!num)
		return 0;

	      /* The partial classes are now full classes.  */
	      if (subclasses[0] == X86_64_SSESF_CLASS && bytes != 4)
		subclasses[0] = X86_64_SSE_CLASS;
	      if (subclasses[0] == X86_64_INTEGERSI_CLASS && bytes != 4)
		subclasses[0] = X86_64_INTEGER_CLASS;

	      for (i = 0; i < words; i++)
		classes[i] = subclasses[i % num];
	    }
	    break;
	  case TYPE_CODE_UNION:
	    {
	      int j;
	      {
		for (j = 0; j < TYPE_NFIELDS (type); ++j)
		  {
		    int num;
		    num = classify_argument (TYPE_FIELDS (type)[j].type,
					     subclasses, bit_offset);
		    if (!num)
		      return 0;
		    for (i = 0; i < num; i++)
		      classes[i] = merge_classes (subclasses[i], classes[i]);
		  }
	      }
	    }
	    break;
	  default:
	    break;
	  }
	/* Final merger cleanup.  */
	for (i = 0; i < words; i++)
	  {
	    /* If one class is MEMORY, everything should be passed in
	       memory.  */
	    if (classes[i] == X86_64_MEMORY_CLASS)
	      return 0;

	    /* The X86_64_SSEUP_CLASS should be always preceeded by
	       X86_64_SSE_CLASS.  */
	    if (classes[i] == X86_64_SSEUP_CLASS
		&& (i == 0 || classes[i - 1] != X86_64_SSE_CLASS))
	      classes[i] = X86_64_SSE_CLASS;

	    /* X86_64_X87UP_CLASS should be preceeded by X86_64_X87_CLASS.  */
	    if (classes[i] == X86_64_X87UP_CLASS
		&& (i == 0 || classes[i - 1] != X86_64_X87_CLASS))
	      classes[i] = X86_64_SSE_CLASS;
	  }
	return words;
      }
      break;
    case TYPE_CODE_FLT:
      switch (bytes)
	{
	case 4:
	  if (!(bit_offset % 64))
	    classes[0] = X86_64_SSESF_CLASS;
	  else
	    classes[0] = X86_64_SSE_CLASS;
	  return 1;
	case 8:
	  classes[0] = X86_64_SSEDF_CLASS;
	  return 1;
	case 16:
	  classes[0] = X86_64_X87_CLASS;
	  classes[1] = X86_64_X87UP_CLASS;
	  return 2;
	}
      break;
    case TYPE_CODE_INT:
    case TYPE_CODE_PTR:
      switch (bytes)
	{
	case 1:
	case 2:
	case 4:
	case 8:
	  if (bytes * 8 + bit_offset <= 32)
	    classes[0] = X86_64_INTEGERSI_CLASS;
	  else
	    classes[0] = X86_64_INTEGER_CLASS;
	  return 1;
	case 16:
	  classes[0] = classes[1] = X86_64_INTEGER_CLASS;
	  return 2;
	default:
	  break;
	}
    case TYPE_CODE_VOID:
      return 0;
    default:			/* Avoid warning.  */
      break;
    }
  internal_error (__FILE__, __LINE__,
		  "classify_argument: unknown argument type");
}

/* Examine the argument and set *INT_NREGS and *SSE_NREGS to the
   number of registers required based on the information passed in
   CLASSES.  Return 0 if parameter should be passed in memory.  */

static int
examine_argument (enum x86_64_reg_class classes[MAX_CLASSES],
		  int n, int *int_nregs, int *sse_nregs)
{
  *int_nregs = 0;
  *sse_nregs = 0;
  if (!n)
    return 0;
  for (n--; n >= 0; n--)
    switch (classes[n])
      {
      case X86_64_INTEGER_CLASS:
      case X86_64_INTEGERSI_CLASS:
	(*int_nregs)++;
	break;
      case X86_64_SSE_CLASS:
      case X86_64_SSESF_CLASS:
      case X86_64_SSEDF_CLASS:
	(*sse_nregs)++;
	break;
      case X86_64_NO_CLASS:
      case X86_64_SSEUP_CLASS:
      case X86_64_X87_CLASS:
      case X86_64_X87UP_CLASS:
	break;
      case X86_64_MEMORY_CLASS:
	internal_error (__FILE__, __LINE__,
			"examine_argument: unexpected memory class");
      }
  return 1;
}

#define RET_INT_REGS 2
#define RET_SSE_REGS 2

/* Check if the structure in value_type is returned in registers or in
   memory. If this function returns 1, GDB will call
   STORE_STRUCT_RETURN and EXTRACT_STRUCT_VALUE_ADDRESS else
   STORE_RETURN_VALUE and EXTRACT_RETURN_VALUE will be used.  */
int
x86_64_use_struct_convention (int gcc_p, struct type *value_type)
{
  enum x86_64_reg_class class[MAX_CLASSES];
  int n = classify_argument (value_type, class, 0);
  int needed_intregs;
  int needed_sseregs;

  return (!n ||
	  !examine_argument (class, n, &needed_intregs, &needed_sseregs) ||
	  needed_intregs > RET_INT_REGS || needed_sseregs > RET_SSE_REGS);
}

/* Extract from an array REGBUF containing the (raw) register state, a
   function return value of TYPE, and copy that, in virtual format,
   into VALBUF.  */

void
x86_64_extract_return_value (struct type *type, char *regbuf, char *valbuf)
{
  enum x86_64_reg_class class[MAX_CLASSES];
  int n = classify_argument (type, class, 0);
  int needed_intregs;
  int needed_sseregs;
  int intreg = 0;
  int ssereg = 0;
  int offset = 0;
  int ret_int_r[RET_INT_REGS] = { RAX_REGNUM, RDX_REGNUM };
  int ret_sse_r[RET_SSE_REGS] = { XMM0_REGNUM, XMM1_REGNUM };

  if (!n ||
      !examine_argument (class, n, &needed_intregs, &needed_sseregs) ||
      needed_intregs > RET_INT_REGS || needed_sseregs > RET_SSE_REGS)
    {				/* memory class */
      CORE_ADDR addr;
      memcpy (&addr, regbuf, REGISTER_RAW_SIZE (RAX_REGNUM));
      read_memory (addr, valbuf, TYPE_LENGTH (type));
      return;
    }
  else
    {
      int i;
      for (i = 0; i < n; i++)
	{
	  switch (class[i])
	    {
	    case X86_64_NO_CLASS:
	      break;
	    case X86_64_INTEGER_CLASS:
	      memcpy (valbuf + offset,
		      regbuf + REGISTER_BYTE (ret_int_r[(intreg + 1) / 2]),
		      8);
	      offset += 8;
	      intreg += 2;
	      break;
	    case X86_64_INTEGERSI_CLASS:
	      memcpy (valbuf + offset,
		      regbuf + REGISTER_BYTE (ret_int_r[intreg / 2]), 4);
	      offset += 8;
	      intreg++;
	      break;
	    case X86_64_SSEDF_CLASS:
	    case X86_64_SSESF_CLASS:
	    case X86_64_SSE_CLASS:
	      memcpy (valbuf + offset,
		      regbuf + REGISTER_BYTE (ret_sse_r[(ssereg + 1) / 2]),
		      8);
	      offset += 8;
	      ssereg += 2;
	      break;
	    case X86_64_SSEUP_CLASS:
	      memcpy (valbuf + offset + 8,
		      regbuf + REGISTER_BYTE (ret_sse_r[ssereg / 2]), 8);
	      offset += 8;
	      ssereg++;
	      break;
	    case X86_64_X87_CLASS:
	      memcpy (valbuf + offset, regbuf + REGISTER_BYTE (FP0_REGNUM),
		      8);
	      offset += 8;
	      break;
	    case X86_64_X87UP_CLASS:
	      memcpy (valbuf + offset,
		      regbuf + REGISTER_BYTE (FP0_REGNUM) + 8, 8);
	      offset += 8;
	      break;
	    case X86_64_MEMORY_CLASS:
	    default:
	      internal_error (__FILE__, __LINE__,
			      "Unexpected argument class");
	    }
	}
    }
}

static void
x86_64_frame_init_saved_regs (struct frame_info *fi)
{
  /* Do nothing.  Everything is handled by the stack unwinding code.  */
}

#define INT_REGS 6
#define SSE_REGS 16

CORE_ADDR
x86_64_push_arguments (int nargs, struct value **args, CORE_ADDR sp,
		       int struct_return, CORE_ADDR struct_addr)
{
  int intreg = 0;
  int ssereg = 0;
  int i;
  static int int_parameter_registers[INT_REGS] = {
    5 /* RDI */ , 4 /* RSI */ ,
    3 /* RDX */ , 2 /* RCX */ ,
    8 /* R8  */ , 9		/* R9  */
  };
  /* XMM0 - XMM15  */
  static int sse_parameter_registers[SSE_REGS] = {
    XMM1_REGNUM - 1, XMM1_REGNUM, XMM1_REGNUM + 1, XMM1_REGNUM + 2,
    XMM1_REGNUM + 3, XMM1_REGNUM + 4, XMM1_REGNUM + 5, XMM1_REGNUM + 6,
    XMM1_REGNUM + 7, XMM1_REGNUM + 8, XMM1_REGNUM + 9, XMM1_REGNUM + 10,
    XMM1_REGNUM + 11, XMM1_REGNUM + 12, XMM1_REGNUM + 13, XMM1_REGNUM + 14
  };
  int stack_values_count = 0;
  int *stack_values;
  stack_values = alloca (nargs * sizeof (int));
  for (i = 0; i < nargs; i++)
    {
      enum x86_64_reg_class class[MAX_CLASSES];
      int n = classify_argument (args[i]->type, class, 0);
      int needed_intregs;
      int needed_sseregs;

      if (!n ||
	  !examine_argument (class, n, &needed_intregs, &needed_sseregs)
	  || intreg / 2 + needed_intregs > INT_REGS
	  || ssereg / 2 + needed_sseregs > SSE_REGS)
	{			/* memory class */
	  stack_values[stack_values_count++] = i;
	}
      else
	{
	  int j;
	  for (j = 0; j < n; j++)
	    {
	      int offset = 0;
	      switch (class[j])
		{
		case X86_64_NO_CLASS:
		  break;
		case X86_64_INTEGER_CLASS:
		  deprecated_write_register_gen (int_parameter_registers
						 [(intreg + 1) / 2],
						 VALUE_CONTENTS_ALL (args[i]) + offset);
		  offset += 8;
		  intreg += 2;
		  break;
		case X86_64_INTEGERSI_CLASS:
		  deprecated_write_register_gen (int_parameter_registers[intreg / 2],
						 VALUE_CONTENTS_ALL (args[i]) + offset);
		  offset += 8;
		  intreg++;
		  break;
		case X86_64_SSEDF_CLASS:
		case X86_64_SSESF_CLASS:
		case X86_64_SSE_CLASS:
		  deprecated_write_register_gen (sse_parameter_registers
						 [(ssereg + 1) / 2],
						 VALUE_CONTENTS_ALL (args[i]) + offset);
		  offset += 8;
		  ssereg += 2;
		  break;
		case X86_64_SSEUP_CLASS:
		  deprecated_write_register_gen (sse_parameter_registers[ssereg / 2],
						 VALUE_CONTENTS_ALL (args[i]) + offset);
		  offset += 8;
		  ssereg++;
		  break;
		case X86_64_X87_CLASS:
		case X86_64_MEMORY_CLASS:
		  stack_values[stack_values_count++] = i;
		  break;
		case X86_64_X87UP_CLASS:
		  break;
		default:
		  internal_error (__FILE__, __LINE__,
				  "Unexpected argument class");
		}
	      intreg += intreg % 2;
	      ssereg += ssereg % 2;
	    }
	}
    }
  while (--stack_values_count >= 0)
    {
      struct value *arg = args[stack_values[stack_values_count]];
      int len = TYPE_LENGTH (VALUE_ENCLOSING_TYPE (arg));
      len += 7;
      len -= len % 8;
      sp -= len;
      write_memory (sp, VALUE_CONTENTS_ALL (arg), len);
    }
  return sp;
}

/* Write into the appropriate registers a function return value stored
   in VALBUF of type TYPE, given in virtual format.  */
void
x86_64_store_return_value (struct type *type, char *valbuf)
{
  int len = TYPE_LENGTH (type);

  if (TYPE_CODE_FLT == TYPE_CODE (type))
    {
      /* Floating-point return values can be found in %st(0).  */
      if (len == TARGET_LONG_DOUBLE_BIT / TARGET_CHAR_BIT
	  && TARGET_LONG_DOUBLE_FORMAT == &floatformat_i387_ext)
	{
	  /* Copy straight over.  */
	  deprecated_write_register_bytes (REGISTER_BYTE (FP0_REGNUM), valbuf,
					   FPU_REG_RAW_SIZE);
	}
      else
	{
	  char buf[FPU_REG_RAW_SIZE];
	  DOUBLEST val;

	  /* Convert the value found in VALBUF to the extended
	     floating point format used by the FPU.  This is probably
	     not exactly how it would happen on the target itself, but
	     it is the best we can do.  */
	  val = extract_floating (valbuf, TYPE_LENGTH (type));
	  floatformat_from_doublest (&floatformat_i387_ext, &val, buf);
	  deprecated_write_register_bytes (REGISTER_BYTE (FP0_REGNUM), buf,
					   FPU_REG_RAW_SIZE);
	}
    }
  else
    {
      int low_size = REGISTER_RAW_SIZE (0);
      int high_size = REGISTER_RAW_SIZE (1);

      if (len <= low_size)
	deprecated_write_register_bytes (REGISTER_BYTE (0), valbuf, len);
      else if (len <= (low_size + high_size))
	{
	  deprecated_write_register_bytes (REGISTER_BYTE (0), valbuf,
					   low_size);
	  deprecated_write_register_bytes (REGISTER_BYTE (1),
					   valbuf + low_size, len - low_size);
	}
      else
	internal_error (__FILE__, __LINE__,
			"Cannot store return value of %d bytes long.", len);
    }
}


const char *
x86_64_register_name (int reg_nr)
{
  if (reg_nr < 0 || reg_nr >= X86_64_NUM_REGS)
    return NULL;
  return x86_64_register_info_table[reg_nr].name;
}

int
x86_64_register_number (const char *name)
{
  int reg_nr;

  for (reg_nr = 0; reg_nr < X86_64_NUM_REGS; reg_nr++)
    if (strcmp (name, x86_64_register_info_table[reg_nr].name) == 0)
      return reg_nr;
  return -1;
}



/* We have two flavours of disassembly.  The machinery on this page
   deals with switching between those.  */

static int
gdb_print_insn_x86_64 (bfd_vma memaddr, disassemble_info * info)
{
  if (disassembly_flavour == att_flavour)
    return print_insn_i386_att (memaddr, info);
  else if (disassembly_flavour == intel_flavour)
    return print_insn_i386_intel (memaddr, info);
  /* Never reached -- disassembly_flavour is always either att_flavour
     or intel_flavour.  */
  internal_error (__FILE__, __LINE__, "failed internal consistency check");
}


/* Store the address of the place in which to copy the structure the
   subroutine will return.  This is called from call_function. */
void
x86_64_store_struct_return (CORE_ADDR addr, CORE_ADDR sp)
{
  write_register (RDI_REGNUM, addr);
}

int
x86_64_frameless_function_invocation (struct frame_info *frame)
{
  return 0;
}

/* If a function with debugging information and known beginning
   is detected, we will return pc of the next line in the source 
   code. With this approach we effectively skip the prolog.  */

#define PROLOG_BUFSIZE 4
CORE_ADDR
x86_64_skip_prologue (CORE_ADDR pc)
{
  int i;
  struct symtab_and_line v_sal;
  struct symbol *v_function;
  CORE_ADDR endaddr;
  unsigned char prolog_buf[PROLOG_BUFSIZE];

  /* We will handle only functions starting with: */
  static unsigned char prolog_expect[PROLOG_BUFSIZE] =
  {
    0x55,			/* pushq %rbp */
    0x48, 0x89, 0xe5		/* movq %rsp, %rbp */
  };

  read_memory (pc, (char *) prolog_buf, PROLOG_BUFSIZE);

  /* First check, whether pc points to pushq %rbp, movq %rsp, %rbp.  */
  for (i = 0; i < PROLOG_BUFSIZE; i++)
    if (prolog_expect[i] != prolog_buf[i])
      return pc;		/* ... no, it doesn't.  Nothing to skip.  */

  /* OK, we have found the prologue and want PC of the first
     non-prologue instruction.  */
  pc += PROLOG_BUFSIZE;

  v_function = find_pc_function (pc);
  v_sal = find_pc_line (pc, 0);

  /* If pc doesn't point to a function with debuginfo, some of the
     following may be NULL.  */
  if (!v_function || !v_function->ginfo.value.block || !v_sal.symtab)
    return pc;

  endaddr = BLOCK_END (SYMBOL_BLOCK_VALUE (v_function));

  for (i = 0; i < v_sal.symtab->linetable->nitems; i++)
    if (v_sal.symtab->linetable->item[i].pc >= pc
	&& v_sal.symtab->linetable->item[i].pc < endaddr)
      {
	pc = v_sal.symtab->linetable->item[i].pc;
	break;
      }

  return pc;
}

/* Sequence of bytes for breakpoint instruction.  */
static const unsigned char *
x86_64_breakpoint_from_pc (CORE_ADDR *pc, int *lenptr)
{
  static unsigned char breakpoint[] = { 0xcc };
  *lenptr = 1;
  return breakpoint;
}

void
x86_64_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int i, sum;

  /* The x86-64 has 16 SSE registers.  */
  tdep->num_xmm_regs = 16;

  /* This is what all the fuss is about.  */
  set_gdbarch_long_bit (gdbarch, 64);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_ptr_bit (gdbarch, 64);

  /* In contrast to the i386, on the x86-64 a `long double' actually
     takes up 128 bits, even though it's still based on the i387
     extended floating-point format which has only 80 significant bits.  */
  set_gdbarch_long_double_bit (gdbarch, 128);

  set_gdbarch_num_regs (gdbarch, X86_64_NUM_REGS);

  /* Register numbers of various important registers.  */
  set_gdbarch_sp_regnum (gdbarch, 7); /* %rsp */
  set_gdbarch_fp_regnum (gdbarch, 6); /* %rbp */
  set_gdbarch_pc_regnum (gdbarch, 16); /* %rip */
  set_gdbarch_ps_regnum (gdbarch, 17); /* %eflags */
  set_gdbarch_fp0_regnum (gdbarch, X86_64_NUM_GREGS); /* %st(0) */

  /* The "default" register numbering scheme for the x86-64 is
     referred to as the "DWARF register number mapping" in the psABI.
     The preferred debugging format for all known x86-64 targets is
     actually DWARF2, and GCC doesn't seem to support DWARF (that is
     DWARF-1), but we provide the same mapping just in case.  This
     mapping is also used for stabs, which GCC does support.  */
  set_gdbarch_stab_reg_to_regnum (gdbarch, x86_64_dwarf2_reg_to_regnum);
  set_gdbarch_dwarf_reg_to_regnum (gdbarch, x86_64_dwarf2_reg_to_regnum);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, x86_64_dwarf2_reg_to_regnum);

  /* We don't override SDB_REG_RO_REGNUM, sice COFF doesn't seem to be
     in use on any of the supported x86-64 targets.  */

  set_gdbarch_register_name (gdbarch, x86_64_register_name);
  set_gdbarch_register_size (gdbarch, 8);

  /* Total amount of space needed to store our copies of the machine's
     register (SIZEOF_GREGS + SIZEOF_FPU_REGS + SIZEOF_FPU_CTRL_REGS +
     SIZEOF_SSE_REGS) */
  for (i = 0, sum = 0; i < X86_64_NUM_REGS; i++)
    sum += x86_64_register_info_table[i].size;
  set_gdbarch_register_bytes (gdbarch, sum);

  set_gdbarch_register_raw_size (gdbarch, x86_64_register_raw_size);
  set_gdbarch_register_byte (gdbarch, x86_64_register_byte);
  set_gdbarch_register_virtual_type (gdbarch, x86_64_register_virtual_type);

  set_gdbarch_register_convertible (gdbarch, x86_64_register_convertible);
  set_gdbarch_register_convert_to_virtual (gdbarch,
					   x86_64_register_convert_to_virtual);
  set_gdbarch_register_convert_to_raw (gdbarch,
				       x86_64_register_convert_to_raw);

  /* Getting saved registers is handled by unwind information.  */
  set_gdbarch_get_saved_register (gdbarch, cfi_get_saved_register);

  /* FIXME: kettenis/20021026: Should we set parm_boundary to 64 here?  */
  set_gdbarch_read_fp (gdbarch, cfi_read_fp);

  /* FIXME: kettenis/20021026: Should be undeprecated.  */
  set_gdbarch_extract_return_value (gdbarch, NULL);
  set_gdbarch_deprecated_extract_return_value (gdbarch,
					       x86_64_extract_return_value);
  set_gdbarch_push_arguments (gdbarch, x86_64_push_arguments);
  set_gdbarch_push_return_address (gdbarch, x86_64_push_return_address);
  set_gdbarch_pop_frame (gdbarch, x86_64_pop_frame);
  set_gdbarch_store_struct_return (gdbarch, x86_64_store_struct_return);
  /* FIXME: kettenis/20021026: Should be undeprecated.  */
  set_gdbarch_store_return_value (gdbarch, NULL);
  set_gdbarch_deprecated_store_return_value (gdbarch,
					     x86_64_store_return_value);
  /* Override, since this is handled by x86_64_extract_return_value.  */
  set_gdbarch_extract_struct_value_address (gdbarch, NULL);
  set_gdbarch_use_struct_convention (gdbarch, x86_64_use_struct_convention);

  set_gdbarch_frame_init_saved_regs (gdbarch, x86_64_frame_init_saved_regs);
  set_gdbarch_skip_prologue (gdbarch, x86_64_skip_prologue);

  set_gdbarch_frame_chain (gdbarch, x86_64_linux_frame_chain);
  set_gdbarch_frameless_function_invocation (gdbarch,
					 x86_64_frameless_function_invocation);
  /* FIXME: kettenis/20021026: These two are GNU/Linux-specific and
     should be moved elsewhere.  */
  set_gdbarch_frame_saved_pc (gdbarch, x86_64_linux_frame_saved_pc);
  set_gdbarch_saved_pc_after_call (gdbarch, x86_64_linux_saved_pc_after_call);
  set_gdbarch_frame_num_args (gdbarch, frame_num_args_unknown);
  /* FIXME: kettenis/20021026: This one is GNU/Linux-specific too.  */
  set_gdbarch_pc_in_sigtramp (gdbarch, x86_64_linux_in_sigtramp);

  set_gdbarch_num_pseudo_regs (gdbarch, 0);

  /* Build call frame information (CFI) from DWARF2 frame debug info.  */
  set_gdbarch_dwarf2_build_frame_info (gdbarch, dwarf2_build_frame_info);

  /* Initialization of per-frame CFI.  */
  set_gdbarch_init_extra_frame_info (gdbarch, cfi_init_extra_frame_info);

  /* Frame PC initialization is handled by using CFI.  */
  set_gdbarch_deprecated_init_frame_pc (gdbarch, x86_64_init_frame_pc);

  /* Cons up virtual frame pointer for trace.  */
  set_gdbarch_virtual_frame_pointer (gdbarch, cfi_virtual_frame_pointer);

  /* FIXME: kettenis/20021026: This is ELF-specific.  Fine for now,
     since all supported x86-64 targets are ELF, but that might change
     in the future.  */
  set_gdbarch_in_solib_call_trampoline (gdbarch, in_plt_section);
}

void
_initialize_x86_64_tdep (void)
{
  /* Initialize the table saying where each register starts in the
     register file.  */
  {
    int i, offset;

    offset = 0;
    for (i = 0; i < X86_64_NUM_REGS; i++)
      {
	x86_64_register_byte_table[i] = offset;
	offset += x86_64_register_info_table[i].size;
      }
  }
}
