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
#include "arch-utils.h"
#include "block.h"
#include "dummy-frame.h"
#include "frame.h"
#include "frame-base.h"
#include "frame-unwind.h"
#include "inferior.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "objfiles.h"
#include "regcache.h"
#include "regset.h"
#include "symfile.h"

#include "gdb_assert.h"

#include "x86-64-tdep.h"
#include "i387-tdep.h"

/* Register information.  */

struct x86_64_register_info
{
  char *name;
  struct type **type;
};

static struct x86_64_register_info x86_64_register_info[] =
{
  { "rax", &builtin_type_int64 },
  { "rbx", &builtin_type_int64 },
  { "rcx", &builtin_type_int64 },
  { "rdx", &builtin_type_int64 },
  { "rsi", &builtin_type_int64 },
  { "rdi", &builtin_type_int64 },
  { "rbp", &builtin_type_void_data_ptr },
  { "rsp", &builtin_type_void_data_ptr },

  /* %r8 is indeed register number 8.  */
  { "r8", &builtin_type_int64 },
  { "r9", &builtin_type_int64 },
  { "r10", &builtin_type_int64 },
  { "r11", &builtin_type_int64 },
  { "r12", &builtin_type_int64 },
  { "r13", &builtin_type_int64 },
  { "r14", &builtin_type_int64 },
  { "r15", &builtin_type_int64 },
  { "rip", &builtin_type_void_func_ptr },
  { "eflags", &builtin_type_int32 },
  { "ds", &builtin_type_int32 },
  { "es", &builtin_type_int32 },
  { "fs", &builtin_type_int32 },
  { "gs", &builtin_type_int32 },

  /* %st0 is register number 22.  */
  { "st0", &builtin_type_i387_ext },
  { "st1", &builtin_type_i387_ext },
  { "st2", &builtin_type_i387_ext },
  { "st3", &builtin_type_i387_ext },
  { "st4", &builtin_type_i387_ext },
  { "st5", &builtin_type_i387_ext },
  { "st6", &builtin_type_i387_ext },
  { "st7", &builtin_type_i387_ext },
  { "fctrl", &builtin_type_int32 },
  { "fstat", &builtin_type_int32 },
  { "ftag", &builtin_type_int32 },
  { "fiseg", &builtin_type_int32 },
  { "fioff", &builtin_type_int32 },
  { "foseg", &builtin_type_int32 },
  { "fooff", &builtin_type_int32 },
  { "fop", &builtin_type_int32 },

  /* %xmm0 is register number 38.  */
  { "xmm0", &builtin_type_v4sf },
  { "xmm1", &builtin_type_v4sf },
  { "xmm2", &builtin_type_v4sf },
  { "xmm3", &builtin_type_v4sf },
  { "xmm4", &builtin_type_v4sf },
  { "xmm5", &builtin_type_v4sf },
  { "xmm6", &builtin_type_v4sf },
  { "xmm7", &builtin_type_v4sf },
  { "xmm8", &builtin_type_v4sf },
  { "xmm9", &builtin_type_v4sf },
  { "xmm10", &builtin_type_v4sf },
  { "xmm11", &builtin_type_v4sf },
  { "xmm12", &builtin_type_v4sf },
  { "xmm13", &builtin_type_v4sf },
  { "xmm14", &builtin_type_v4sf },
  { "xmm15", &builtin_type_v4sf },
  { "mxcsr", &builtin_type_int32 }
};

/* Total number of registers.  */
#define X86_64_NUM_REGS \
  (sizeof (x86_64_register_info) / sizeof (x86_64_register_info[0]))

/* Return the name of register REGNUM.  */

static const char *
x86_64_register_name (int regnum)
{
  if (regnum >= 0 && regnum < X86_64_NUM_REGS)
    return x86_64_register_info[regnum].name;

  return NULL;
}

/* Return the GDB type object for the "standard" data type of data in
   register REGNUM. */

static struct type *
x86_64_register_type (struct gdbarch *gdbarch, int regnum)
{
  gdb_assert (regnum >= 0 && regnum < X86_64_NUM_REGS);

  return *x86_64_register_info[regnum].type;
}

/* DWARF Register Number Mapping as defined in the System V psABI,
   section 3.6.  */

static int x86_64_dwarf_regmap[] =
{
  /* General Purpose Registers RAX, RDX, RCX, RBX, RSI, RDI.  */
  X86_64_RAX_REGNUM, X86_64_RDX_REGNUM, 2, 1,
  4, X86_64_RDI_REGNUM,

  /* Frame Pointer Register RBP.  */
  X86_64_RBP_REGNUM,

  /* Stack Pointer Register RSP.  */
  X86_64_RSP_REGNUM,

  /* Extended Integer Registers 8 - 15.  */
  8, 9, 10, 11, 12, 13, 14, 15,

  /* Return Address RA.  Not mapped.  */
  -1,

  /* SSE Registers 0 - 7.  */
  X86_64_XMM0_REGNUM + 0, X86_64_XMM1_REGNUM,
  X86_64_XMM0_REGNUM + 2, X86_64_XMM0_REGNUM + 3,
  X86_64_XMM0_REGNUM + 4, X86_64_XMM0_REGNUM + 5,
  X86_64_XMM0_REGNUM + 6, X86_64_XMM0_REGNUM + 7,

  /* Extended SSE Registers 8 - 15.  */
  X86_64_XMM0_REGNUM + 8, X86_64_XMM0_REGNUM + 9,
  X86_64_XMM0_REGNUM + 10, X86_64_XMM0_REGNUM + 11,
  X86_64_XMM0_REGNUM + 12, X86_64_XMM0_REGNUM + 13,
  X86_64_XMM0_REGNUM + 14, X86_64_XMM0_REGNUM + 15,

  /* Floating Point Registers 0-7.  */
  X86_64_ST0_REGNUM + 0, X86_64_ST0_REGNUM + 1,
  X86_64_ST0_REGNUM + 2, X86_64_ST0_REGNUM + 3,
  X86_64_ST0_REGNUM + 4, X86_64_ST0_REGNUM + 5,
  X86_64_ST0_REGNUM + 6, X86_64_ST0_REGNUM + 7
};

static const int x86_64_dwarf_regmap_len =
  (sizeof (x86_64_dwarf_regmap) / sizeof (x86_64_dwarf_regmap[0]));

/* Convert DWARF register number REG to the appropriate register
   number used by GDB.  */

static int
x86_64_dwarf_reg_to_regnum (int reg)
{
  int regnum = -1;

  if (reg >= 0 || reg < x86_64_dwarf_regmap_len)
    regnum = x86_64_dwarf_regmap[reg];

  if (regnum == -1)
    warning ("Unmapped DWARF Register #%d encountered\n", reg);

  return regnum;
}

/* Return nonzero if a value of type TYPE stored in register REGNUM
   needs any special handling.  */

static int
x86_64_convert_register_p (int regnum, struct type *type)
{
  return i386_fp_regnum_p (regnum);
}


/* The returning of values is done according to the special algorithm.
   Some types are returned in registers an some (big structures) in
   memory.  See the System V psABI for details.  */

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
   See the System V psABI for details.  */

static enum x86_64_reg_class
merge_classes (enum x86_64_reg_class class1, enum x86_64_reg_class class2)
{
  /* Rule (a): If both classes are equal, this is the resulting class.  */
  if (class1 == class2)
    return class1;

  /* Rule (b): If one of the classes is NO_CLASS, the resulting class
     is the other class.  */
  if (class1 == X86_64_NO_CLASS)
    return class2;
  if (class2 == X86_64_NO_CLASS)
    return class1;

  /* Rule (c): If one of the classes is MEMORY, the result is MEMORY.  */
  if (class1 == X86_64_MEMORY_CLASS || class2 == X86_64_MEMORY_CLASS)
    return X86_64_MEMORY_CLASS;

  /* Rule (d): If one of the classes is INTEGER, the result is INTEGER.  */
  if ((class1 == X86_64_INTEGERSI_CLASS && class2 == X86_64_SSESF_CLASS)
      || (class2 == X86_64_INTEGERSI_CLASS && class1 == X86_64_SSESF_CLASS))
    return X86_64_INTEGERSI_CLASS;
  if (class1 == X86_64_INTEGER_CLASS || class1 == X86_64_INTEGERSI_CLASS
      || class2 == X86_64_INTEGER_CLASS || class2 == X86_64_INTEGERSI_CLASS)
    return X86_64_INTEGER_CLASS;

  /* Rule (e): If one of the classes is X87 or X87UP class, MEMORY is
     used as class.  */
  if (class1 == X86_64_X87_CLASS || class1 == X86_64_X87UP_CLASS
      || class2 == X86_64_X87_CLASS || class2 == X86_64_X87UP_CLASS)
    return X86_64_MEMORY_CLASS;

  /* Rule (f): Otherwise class SSE is used.  */
  return X86_64_SSE_CLASS;
}

/* Classify the argument type.  CLASSES will be filled by the register
   class used to pass each word of the operand.  The number of words
   is returned.  In case the parameter should be passed in memory, 0
   is returned.  As a special case for zero sized containers,
   classes[0] will be NO_CLASS and 1 is returned.

   See the System V psABI for details.  */

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
    case TYPE_CODE_ENUM:
    case TYPE_CODE_REF:
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

#define INT_REGS 6
#define SSE_REGS 8

static CORE_ADDR
x86_64_push_arguments (struct regcache *regcache, int nargs,
		       struct value **args, CORE_ADDR sp)
{
  int intreg = 0;
  int ssereg = 0;
  /* For varargs functions we have to pass the total number of SSE
     registers used in %rax.  So, let's count this number.  */
  int total_sse_args = 0;
  /* Once an SSE/int argument is passed on the stack, all subsequent
     arguments are passed there.  */
  int sse_stack = 0;
  int int_stack = 0;
  unsigned total_sp;
  int i;
  char buf[8];
  static int int_parameter_registers[INT_REGS] =
  {
    X86_64_RDI_REGNUM, 4,	/* %rdi, %rsi */
    X86_64_RDX_REGNUM, 2,	/* %rdx, %rcx */
    8, 9			/* %r8, %r9 */
  };
  /* %xmm0 - %xmm7 */
  static int sse_parameter_registers[SSE_REGS] =
  {
    X86_64_XMM0_REGNUM + 0, X86_64_XMM1_REGNUM,
    X86_64_XMM0_REGNUM + 2, X86_64_XMM0_REGNUM + 3,
    X86_64_XMM0_REGNUM + 4, X86_64_XMM0_REGNUM + 5,
    X86_64_XMM0_REGNUM + 6, X86_64_XMM0_REGNUM + 7,
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
	  !examine_argument (class, n, &needed_intregs, &needed_sseregs))
	{			/* memory class */
	  stack_values[stack_values_count++] = i;
	}
      else
	{
	  int j;
	  int offset = 0;

	  if (intreg / 2 + needed_intregs > INT_REGS)
	    int_stack = 1;
	  if (ssereg / 2 + needed_sseregs > SSE_REGS)
	    sse_stack = 1;
	  if (!sse_stack)
	    total_sse_args += needed_sseregs;

	  for (j = 0; j < n; j++)
	    {
	      switch (class[j])
		{
		case X86_64_NO_CLASS:
		  break;
		case X86_64_INTEGER_CLASS:
		  if (int_stack)
		    stack_values[stack_values_count++] = i;
		  else
		    {
		      regcache_cooked_write
			(regcache, int_parameter_registers[(intreg + 1) / 2],
			 VALUE_CONTENTS_ALL (args[i]) + offset);
		      offset += 8;
		      intreg += 2;
		    }
		  break;
		case X86_64_INTEGERSI_CLASS:
		  if (int_stack)
		    stack_values[stack_values_count++] = i;
		  else
		    {
		      LONGEST val = extract_signed_integer
			(VALUE_CONTENTS_ALL (args[i]) + offset, 4);
		      regcache_cooked_write_signed
			(regcache, int_parameter_registers[intreg / 2], val);
		      
		      offset += 8;
		      intreg++;
		    }
		  break;
		case X86_64_SSEDF_CLASS:
		case X86_64_SSESF_CLASS:
		case X86_64_SSE_CLASS:
		  if (sse_stack)
		    stack_values[stack_values_count++] = i;
		  else
		    {
		      regcache_cooked_write
			(regcache, sse_parameter_registers[(ssereg + 1) / 2],
			 VALUE_CONTENTS_ALL (args[i]) + offset);
		      offset += 8;
		      ssereg += 2;
		    }
		  break;
		case X86_64_SSEUP_CLASS:
		  if (sse_stack)
		    stack_values[stack_values_count++] = i;
		  else
		    {
		      regcache_cooked_write
			(regcache, sse_parameter_registers[ssereg / 2],
			 VALUE_CONTENTS_ALL (args[i]) + offset);
		      offset += 8;
		      ssereg++;
		    }
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

  /* We have to make sure that the stack is 16-byte aligned after the
     setup.  Let's calculate size of arguments first, align stack and
     then fill in the arguments.  */
  total_sp = 0;
  for (i = 0; i < stack_values_count; i++)
    {
      struct value *arg = args[stack_values[i]];
      int len = TYPE_LENGTH (VALUE_ENCLOSING_TYPE (arg));
      total_sp += (len + 7) & ~7;
    }
  /* total_sp is now a multiple of 8, if it is not a multiple of 16,
     change the stack pointer so that it will be afterwards correctly
     aligned.  */
  if (total_sp & 15)
    sp -= 8;
    
  /* Push any remaining arguments onto the stack.  */
  while (--stack_values_count >= 0)
    {
      struct value *arg = args[stack_values[stack_values_count]];
      int len = TYPE_LENGTH (VALUE_ENCLOSING_TYPE (arg));

      /* Make sure the stack is 8-byte-aligned.  */
      sp -= (len + 7) & ~7;
      write_memory (sp, VALUE_CONTENTS_ALL (arg), len);
    }

  /* Write number of SSE type arguments to RAX to take care of varargs
     functions.  */
  store_unsigned_integer (buf, 8, total_sse_args);
  regcache_cooked_write (regcache, X86_64_RAX_REGNUM, buf);

  return sp;
}

/* Register classes as defined in the psABI.  */

enum amd64_reg_class
{
  AMD64_INTEGER,
  AMD64_SSE,
  AMD64_SSEUP,
  AMD64_X87,
  AMD64_X87UP,
  AMD64_COMPLEX_X87,
  AMD64_NO_CLASS,
  AMD64_MEMORY
};

/* Return the union class of CLASS1 and CLASS2.  See the psABI for
   details.  */

static enum amd64_reg_class
amd64_merge_classes (enum amd64_reg_class class1, enum amd64_reg_class class2)
{
  /* Rule (a): If both classes are equal, this is the resulting class.  */
  if (class1 == class2)
    return class1;

  /* Rule (b): If one of the classes is NO_CLASS, the resulting class
     is the other class.  */
  if (class1 == AMD64_NO_CLASS)
    return class2;
  if (class2 == AMD64_NO_CLASS)
    return class1;

  /* Rule (c): If one of the classes is MEMORY, the result is MEMORY.  */
  if (class1 == AMD64_MEMORY || class2 == AMD64_MEMORY)
    return AMD64_MEMORY;

  /* Rule (d): If one of the classes is INTEGER, the result is INTEGER.  */
  if (class1 == AMD64_INTEGER || class2 == AMD64_INTEGER)
    return AMD64_INTEGER;

  /* Rule (e): If one of the classes is X87, X87UP, COMPLEX_X87 class,
     MEMORY is used as class.  */
  if (class1 == AMD64_X87 || class1 == AMD64_X87UP
      || class1 == AMD64_COMPLEX_X87 || class2 == AMD64_X87
      || class2 == AMD64_X87UP || class2 == AMD64_COMPLEX_X87)
    return AMD64_MEMORY;

  /* Rule (f): Otherwise class SSE is used.  */
  return AMD64_SSE;
}

static void amd64_classify (struct type *type, enum amd64_reg_class class[2]);

/* Classify TYPE according to the rules for aggregate (structures and
   arrays) and union types, and store the result in CLASS.  */

static void
amd64_classify_aggregate (struct type *type, enum amd64_reg_class class[2])
{
  int len = TYPE_LENGTH (type);

  /* 1. If the size of an object is larger than two eightbytes, or in
        C++, is a non-POD structure or union type, or contains
        unaligned fields, it has class memory.  */
  if (len > 16)
    {
      class[0] = class[1] = AMD64_MEMORY;
      return;
    }

  /* 2. Both eightbytes get initialized to class NO_CLASS.  */
  class[0] = class[1] = AMD64_NO_CLASS;

  /* 3. Each field of an object is classified recursively so that
        always two fields are considered. The resulting class is
        calculated according to the classes of the fields in the
        eightbyte: */

  if (TYPE_CODE (type) == TYPE_CODE_ARRAY)
    {
      struct type *subtype = check_typedef (TYPE_TARGET_TYPE (type));

      /* All fields in an array have the same type.  */
      amd64_classify (subtype, class);
      if (len > 8 && class[1] == AMD64_NO_CLASS)
	class[1] = class[0];
    }
  else
    {
      int i;

      /* Structure or union.  */
      gdb_assert (TYPE_CODE (type) == TYPE_CODE_STRUCT
		  || TYPE_CODE (type) == TYPE_CODE_UNION);

      for (i = 0; i < TYPE_NFIELDS (type); i++)
	{
	  struct type *subtype = check_typedef (TYPE_FIELD_TYPE (type, i));
	  int pos = TYPE_FIELD_BITPOS (type, i) / 64;
	  enum amd64_reg_class subclass[2];

	  gdb_assert (pos == 0 || pos == 1);

	  amd64_classify (subtype, subclass);
	  class[pos] = amd64_merge_classes (class[pos], subclass[0]);
	  if (pos == 0)
	    class[1] = amd64_merge_classes (class[1], subclass[1]);
	}
    }

  /* 4. Then a post merger cleanup is done:  */

  /* Rule (a): If one of the classes is MEMORY, the whole argument is
     passed in memory.  */
  if (class[0] == AMD64_MEMORY || class[1] == AMD64_MEMORY)
    class[0] = class[1] = AMD64_MEMORY;

  /* Rule (b): If SSEUP is not preceeded by SSE, it is converted to
     SSE.  */
  if (class[0] == AMD64_SSEUP)
    class[0] = AMD64_SSE;
  if (class[1] == AMD64_SSEUP && class[0] != AMD64_SSE)
    class[1] = AMD64_SSE;
}

/* Classify TYPE, and store the result in CLASS.  */

static void
amd64_classify (struct type *type, enum amd64_reg_class class[2])
{
  enum type_code code = TYPE_CODE (type);
  int len = TYPE_LENGTH (type);

  class[0] = class[1] = AMD64_NO_CLASS;

  /* Arguments of types (signed and unsigned) _Bool, char, short, int,
     long, long long, and pointers are in the INTEGER class.  */
  if ((code == TYPE_CODE_INT || code == TYPE_CODE_ENUM
       || code == TYPE_CODE_PTR || code == TYPE_CODE_REF)
      && (len == 1 || len == 2 || len == 4 || len == 8))
    class[0] = AMD64_INTEGER;

  /* Arguments of types float, double and __m64 are in class SSE.  */
  else if (code == TYPE_CODE_FLT && (len == 4 || len == 8))
    /* FIXME: __m64 .  */
    class[0] = AMD64_SSE;

  /* Arguments of types __float128 and __m128 are split into two
     halves.  The least significant ones belong to class SSE, the most
     significant one to class SSEUP.  */
  /* FIXME: __float128, __m128.  */

  /* The 64-bit mantissa of arguments of type long double belongs to
     class X87, the 16-bit exponent plus 6 bytes of padding belongs to
     class X87UP.  */
  else if (code == TYPE_CODE_FLT && len == 16)
    /* Class X87 and X87UP.  */
    class[0] = AMD64_X87, class[1] = AMD64_X87UP;

  /* Aggregates.  */
  else if (code == TYPE_CODE_ARRAY || code == TYPE_CODE_STRUCT
	   || code == TYPE_CODE_UNION)
    amd64_classify_aggregate (type, class);
}

static enum return_value_convention
amd64_return_value (struct gdbarch *gdbarch, struct type *type,
		    struct regcache *regcache,
		    void *readbuf, const void *writebuf)
{
  enum amd64_reg_class class[2];
  int len = TYPE_LENGTH (type);
  static int integer_regnum[] = { X86_64_RAX_REGNUM, X86_64_RDX_REGNUM };
  static int sse_regnum[] = { X86_64_XMM0_REGNUM, X86_64_XMM1_REGNUM };
  int integer_reg = 0;
  int sse_reg = 0;
  int i;

  gdb_assert (!(readbuf && writebuf));

  /* 1. Classify the return type with the classification algorithm.  */
  amd64_classify (type, class);

  /* 2. If the type has class MEMORY, then the caller provides space
        for the return value and passes the address of this storage in
        %rdi as if it were the first argument to the function. In
        effect, this address becomes a hidden first argument.  */
  if (class[0] == AMD64_MEMORY)
    return RETURN_VALUE_STRUCT_CONVENTION;

  gdb_assert (class[1] != AMD64_MEMORY);
  gdb_assert (len <= 16);

  for (i = 0; len > 0; i++, len -= 8)
    {
      int regnum = -1;
      int offset = 0;

      switch (class[i])
	{
	case AMD64_INTEGER:
	  /* 3. If the class is INTEGER, the next available register
	     of the sequence %rax, %rdx is used.  */
	  regnum = integer_regnum[integer_reg++];
	  break;

	case AMD64_SSE:
	  /* 4. If the class is SSE, the next available SSE register
             of the sequence %xmm0, %xmm1 is used.  */
	  regnum = sse_regnum[sse_reg++];
	  break;

	case AMD64_SSEUP:
	  /* 5. If the class is SSEUP, the eightbyte is passed in the
	     upper half of the last used SSE register.  */
	  gdb_assert (sse_reg > 0);
	  regnum = sse_regnum[sse_reg - 1];
	  offset = 8;
	  break;

	case AMD64_X87:
	  /* 6. If the class is X87, the value is returned on the X87
             stack in %st0 as 80-bit x87 number.  */
	  regnum = X86_64_ST0_REGNUM;
	  if (writebuf)
	    i387_return_value (gdbarch, regcache);
	  break;

	case AMD64_X87UP:
	  /* 7. If the class is X87UP, the value is returned together
             with the previous X87 value in %st0.  */
	  gdb_assert (i > 0 && class[0] == AMD64_X87);
	  regnum = X86_64_ST0_REGNUM;
	  offset = 8;
	  len = 2;
	  break;

	case AMD64_NO_CLASS:
	  continue;

	default:
	  gdb_assert (!"Unexpected register class.");
	}

      gdb_assert (regnum != -1);

      if (readbuf)
	regcache_raw_read_part (regcache, regnum, offset, min (len, 8),
				(char *) readbuf + i * 8);
      if (writebuf)
	regcache_raw_write_part (regcache, regnum, offset, min (len, 8),
				 (const char *) writebuf + i * 8);
    }

  return RETURN_VALUE_REGISTER_CONVENTION;
}


static CORE_ADDR
x86_64_push_dummy_call (struct gdbarch *gdbarch, CORE_ADDR func_addr,
			struct regcache *regcache, CORE_ADDR bp_addr,
			int nargs, struct value **args,	CORE_ADDR sp,
			int struct_return, CORE_ADDR struct_addr)
{
  char buf[8];

  /* Pass arguments.  */
  sp = x86_64_push_arguments (regcache, nargs, args, sp);

  /* Pass "hidden" argument".  */
  if (struct_return)
    {
      store_unsigned_integer (buf, 8, struct_addr);
      regcache_cooked_write (regcache, X86_64_RDI_REGNUM, buf);
    }

  /* Store return address.  */
  sp -= 8;
  store_unsigned_integer (buf, 8, bp_addr);
  write_memory (sp, buf, 8);

  /* Finally, update the stack pointer...  */
  store_unsigned_integer (buf, 8, sp);
  regcache_cooked_write (regcache, X86_64_RSP_REGNUM, buf);

  /* ...and fake a frame pointer.  */
  regcache_cooked_write (regcache, X86_64_RBP_REGNUM, buf);

  return sp + 16;
}


/* The maximum number of saved registers.  This should include %rip.  */
#define X86_64_NUM_SAVED_REGS	X86_64_NUM_GREGS

struct x86_64_frame_cache
{
  /* Base address.  */
  CORE_ADDR base;
  CORE_ADDR sp_offset;
  CORE_ADDR pc;

  /* Saved registers.  */
  CORE_ADDR saved_regs[X86_64_NUM_SAVED_REGS];
  CORE_ADDR saved_sp;

  /* Do we have a frame?  */
  int frameless_p;
};

/* Allocate and initialize a frame cache.  */

static struct x86_64_frame_cache *
x86_64_alloc_frame_cache (void)
{
  struct x86_64_frame_cache *cache;
  int i;

  cache = FRAME_OBSTACK_ZALLOC (struct x86_64_frame_cache);

  /* Base address.  */
  cache->base = 0;
  cache->sp_offset = -8;
  cache->pc = 0;

  /* Saved registers.  We initialize these to -1 since zero is a valid
     offset (that's where %rbp is supposed to be stored).  */
  for (i = 0; i < X86_64_NUM_SAVED_REGS; i++)
    cache->saved_regs[i] = -1;
  cache->saved_sp = 0;

  /* Frameless until proven otherwise.  */
  cache->frameless_p = 1;

  return cache;
}

/* Do a limited analysis of the prologue at PC and update CACHE
   accordingly.  Bail out early if CURRENT_PC is reached.  Return the
   address where the analysis stopped.

   We will handle only functions beginning with:

      pushq %rbp        0x55
      movq %rsp, %rbp   0x48 0x89 0xe5

   Any function that doesn't start with this sequence will be assumed
   to have no prologue and thus no valid frame pointer in %rbp.  */

static CORE_ADDR
x86_64_analyze_prologue (CORE_ADDR pc, CORE_ADDR current_pc,
			 struct x86_64_frame_cache *cache)
{
  static unsigned char proto[3] = { 0x48, 0x89, 0xe5 };
  unsigned char buf[3];
  unsigned char op;

  if (current_pc <= pc)
    return current_pc;

  op = read_memory_unsigned_integer (pc, 1);

  if (op == 0x55)		/* pushq %rbp */
    {
      /* Take into account that we've executed the `pushq %rbp' that
         starts this instruction sequence.  */
      cache->saved_regs[X86_64_RBP_REGNUM] = 0;
      cache->sp_offset += 8;

      /* If that's all, return now.  */
      if (current_pc <= pc + 1)
        return current_pc;

      /* Check for `movq %rsp, %rbp'.  */
      read_memory (pc + 1, buf, 3);
      if (memcmp (buf, proto, 3) != 0)
	return pc + 1;

      /* OK, we actually have a frame.  */
      cache->frameless_p = 0;
      return pc + 4;
    }

  return pc;
}

/* Return PC of first real instruction.  */

static CORE_ADDR
x86_64_skip_prologue (CORE_ADDR start_pc)
{
  struct x86_64_frame_cache cache;
  CORE_ADDR pc;

  pc = x86_64_analyze_prologue (start_pc, 0xffffffffffffffff, &cache);
  if (cache.frameless_p)
    return start_pc;

  return pc;
}


/* Normal frames.  */

static struct x86_64_frame_cache *
x86_64_frame_cache (struct frame_info *next_frame, void **this_cache)
{
  struct x86_64_frame_cache *cache;
  char buf[8];
  int i;

  if (*this_cache)
    return *this_cache;

  cache = x86_64_alloc_frame_cache ();
  *this_cache = cache;

  cache->pc = frame_func_unwind (next_frame);
  if (cache->pc != 0)
    x86_64_analyze_prologue (cache->pc, frame_pc_unwind (next_frame), cache);

  if (cache->frameless_p)
    {
      /* We didn't find a valid frame, which means that CACHE->base
	 currently holds the frame pointer for our calling frame.  If
	 we're at the start of a function, or somewhere half-way its
	 prologue, the function's frame probably hasn't been fully
	 setup yet.  Try to reconstruct the base address for the stack
	 frame by looking at the stack pointer.  For truly "frameless"
	 functions this might work too.  */

      frame_unwind_register (next_frame, X86_64_RSP_REGNUM, buf);
      cache->base = extract_unsigned_integer (buf, 8) + cache->sp_offset;
    }
  else
    {
      frame_unwind_register (next_frame, X86_64_RBP_REGNUM, buf);
      cache->base = extract_unsigned_integer (buf, 8);
    }

  /* Now that we have the base address for the stack frame we can
     calculate the value of %rsp in the calling frame.  */
  cache->saved_sp = cache->base + 16;

  /* For normal frames, %rip is stored at 8(%rbp).  If we don't have a
     frame we find it at the same offset from the reconstructed base
     address.  */
  cache->saved_regs[X86_64_RIP_REGNUM] = 8;

  /* Adjust all the saved registers such that they contain addresses
     instead of offsets.  */
  for (i = 0; i < X86_64_NUM_SAVED_REGS; i++)
    if (cache->saved_regs[i] != -1)
      cache->saved_regs[i] += cache->base;

  return cache;
}

static void
x86_64_frame_this_id (struct frame_info *next_frame, void **this_cache,
		      struct frame_id *this_id)
{
  struct x86_64_frame_cache *cache =
    x86_64_frame_cache (next_frame, this_cache);

  /* This marks the outermost frame.  */
  if (cache->base == 0)
    return;

  (*this_id) = frame_id_build (cache->base + 16, cache->pc);
}

static void
x86_64_frame_prev_register (struct frame_info *next_frame, void **this_cache,
			    int regnum, int *optimizedp,
			    enum lval_type *lvalp, CORE_ADDR *addrp,
			    int *realnump, void *valuep)
{
  struct x86_64_frame_cache *cache =
    x86_64_frame_cache (next_frame, this_cache);

  gdb_assert (regnum >= 0);

  if (regnum == SP_REGNUM && cache->saved_sp)
    {
      *optimizedp = 0;
      *lvalp = not_lval;
      *addrp = 0;
      *realnump = -1;
      if (valuep)
	{
	  /* Store the value.  */
	  store_unsigned_integer (valuep, 8, cache->saved_sp);
	}
      return;
    }

  if (regnum < X86_64_NUM_SAVED_REGS && cache->saved_regs[regnum] != -1)
    {
      *optimizedp = 0;
      *lvalp = lval_memory;
      *addrp = cache->saved_regs[regnum];
      *realnump = -1;
      if (valuep)
	{
	  /* Read the value in from memory.  */
	  read_memory (*addrp, valuep,
		       register_size (current_gdbarch, regnum));
	}
      return;
    }

  frame_register_unwind (next_frame, regnum,
			 optimizedp, lvalp, addrp, realnump, valuep);
}

static const struct frame_unwind x86_64_frame_unwind =
{
  NORMAL_FRAME,
  x86_64_frame_this_id,
  x86_64_frame_prev_register
};

static const struct frame_unwind *
x86_64_frame_sniffer (struct frame_info *next_frame)
{
  return &x86_64_frame_unwind;
}


/* Signal trampolines.  */

/* FIXME: kettenis/20030419: Perhaps, we can unify the 32-bit and
   64-bit variants.  This would require using identical frame caches
   on both platforms.  */

static struct x86_64_frame_cache *
x86_64_sigtramp_frame_cache (struct frame_info *next_frame, void **this_cache)
{
  struct x86_64_frame_cache *cache;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  CORE_ADDR addr;
  char buf[8];
  int i;

  if (*this_cache)
    return *this_cache;

  cache = x86_64_alloc_frame_cache ();

  frame_unwind_register (next_frame, X86_64_RSP_REGNUM, buf);
  cache->base = extract_unsigned_integer (buf, 8) - 8;

  addr = tdep->sigcontext_addr (next_frame);
  gdb_assert (tdep->sc_reg_offset);
  gdb_assert (tdep->sc_num_regs <= X86_64_NUM_SAVED_REGS);
  for (i = 0; i < tdep->sc_num_regs; i++)
    if (tdep->sc_reg_offset[i] != -1)
      cache->saved_regs[i] = addr + tdep->sc_reg_offset[i];

  *this_cache = cache;
  return cache;
}

static void
x86_64_sigtramp_frame_this_id (struct frame_info *next_frame,
			       void **this_cache, struct frame_id *this_id)
{
  struct x86_64_frame_cache *cache =
    x86_64_sigtramp_frame_cache (next_frame, this_cache);

  (*this_id) = frame_id_build (cache->base + 16, frame_pc_unwind (next_frame));
}

static void
x86_64_sigtramp_frame_prev_register (struct frame_info *next_frame,
				     void **this_cache,
				     int regnum, int *optimizedp,
				     enum lval_type *lvalp, CORE_ADDR *addrp,
				     int *realnump, void *valuep)
{
  /* Make sure we've initialized the cache.  */
  x86_64_sigtramp_frame_cache (next_frame, this_cache);

  x86_64_frame_prev_register (next_frame, this_cache, regnum,
			      optimizedp, lvalp, addrp, realnump, valuep);
}

static const struct frame_unwind x86_64_sigtramp_frame_unwind =
{
  SIGTRAMP_FRAME,
  x86_64_sigtramp_frame_this_id,
  x86_64_sigtramp_frame_prev_register
};

static const struct frame_unwind *
x86_64_sigtramp_frame_sniffer (struct frame_info *next_frame)
{
  CORE_ADDR pc = frame_pc_unwind (next_frame);
  char *name;

  find_pc_partial_function (pc, &name, NULL, NULL);
  if (PC_IN_SIGTRAMP (pc, name))
    {
      gdb_assert (gdbarch_tdep (current_gdbarch)->sigcontext_addr);

      return &x86_64_sigtramp_frame_unwind;
    }

  return NULL;
}


static CORE_ADDR
x86_64_frame_base_address (struct frame_info *next_frame, void **this_cache)
{
  struct x86_64_frame_cache *cache =
    x86_64_frame_cache (next_frame, this_cache);

  return cache->base;
}

static const struct frame_base x86_64_frame_base =
{
  &x86_64_frame_unwind,
  x86_64_frame_base_address,
  x86_64_frame_base_address,
  x86_64_frame_base_address
};

static struct frame_id
x86_64_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  char buf[8];
  CORE_ADDR fp;

  frame_unwind_register (next_frame, X86_64_RBP_REGNUM, buf);
  fp = extract_unsigned_integer (buf, 8);

  return frame_id_build (fp + 16, frame_pc_unwind (next_frame));
}

/* 16 byte align the SP per frame requirements.  */

static CORE_ADDR
x86_64_frame_align (struct gdbarch *gdbarch, CORE_ADDR sp)
{
  return sp & -(CORE_ADDR)16;
}


/* Supply register REGNUM from the floating-point register set REGSET
   to register cache REGCACHE.  If REGNUM is -1, do this for all
   registers in REGSET.  */

static void
x86_64_supply_fpregset (const struct regset *regset, struct regcache *regcache,
			int regnum, const void *fpregs, size_t len)
{
  const struct gdbarch_tdep *tdep = regset->descr;

  gdb_assert (len == tdep->sizeof_fpregset);
  x86_64_supply_fxsave (regcache, regnum, fpregs);
}

/* Return the appropriate register set for the core section identified
   by SECT_NAME and SECT_SIZE.  */

static const struct regset *
x86_64_regset_from_core_section (struct gdbarch *gdbarch,
				 const char *sect_name, size_t sect_size)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  if (strcmp (sect_name, ".reg2") == 0 && sect_size == tdep->sizeof_fpregset)
    {
      if (tdep->fpregset == NULL)
	{
	  tdep->fpregset = XMALLOC (struct regset);
	  tdep->fpregset->descr = tdep;
	  tdep->fpregset->supply_regset = x86_64_supply_fpregset;
	}

      return tdep->fpregset;
    }

  return i386_regset_from_core_section (gdbarch, sect_name, sect_size);
}


void
x86_64_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* AMD64 generally uses `fxsave' instead of `fsave' for saving its
     floating-point registers.  */
  tdep->sizeof_fpregset = I387_SIZEOF_FXSAVE;

  /* AMD64 has an FPU and 16 SSE registers.  */
  tdep->st0_regnum = X86_64_ST0_REGNUM;
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
  set_gdbarch_register_name (gdbarch, x86_64_register_name);
  set_gdbarch_register_type (gdbarch, x86_64_register_type);

  /* Register numbers of various important registers.  */
  set_gdbarch_sp_regnum (gdbarch, X86_64_RSP_REGNUM); /* %rsp */
  set_gdbarch_pc_regnum (gdbarch, X86_64_RIP_REGNUM); /* %rip */
  set_gdbarch_ps_regnum (gdbarch, X86_64_EFLAGS_REGNUM); /* %eflags */
  set_gdbarch_fp0_regnum (gdbarch, X86_64_ST0_REGNUM); /* %st(0) */

  /* The "default" register numbering scheme for the x86-64 is
     referred to as the "DWARF Register Number Mapping" in the System
     V psABI.  The preferred debugging format for all known x86-64
     targets is actually DWARF2, and GCC doesn't seem to support DWARF
     (that is DWARF-1), but we provide the same mapping just in case.
     This mapping is also used for stabs, which GCC does support.  */
  set_gdbarch_stab_reg_to_regnum (gdbarch, x86_64_dwarf_reg_to_regnum);
  set_gdbarch_dwarf_reg_to_regnum (gdbarch, x86_64_dwarf_reg_to_regnum);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, x86_64_dwarf_reg_to_regnum);

  /* We don't override SDB_REG_RO_REGNUM, since COFF doesn't seem to
     be in use on any of the supported x86-64 targets.  */

  /* Call dummy code.  */
  set_gdbarch_push_dummy_call (gdbarch, x86_64_push_dummy_call);
  set_gdbarch_frame_align (gdbarch, x86_64_frame_align);
  set_gdbarch_frame_red_zone_size (gdbarch, 128);

  set_gdbarch_convert_register_p (gdbarch, x86_64_convert_register_p);
  set_gdbarch_register_to_value (gdbarch, i387_register_to_value);
  set_gdbarch_value_to_register (gdbarch, i387_value_to_register);

  set_gdbarch_return_value (gdbarch, amd64_return_value);
  /* Override, since this is handled by x86_64_extract_return_value.  */
  set_gdbarch_extract_struct_value_address (gdbarch, NULL);

  set_gdbarch_skip_prologue (gdbarch, x86_64_skip_prologue);

  /* Avoid wiring in the MMX registers for now.  */
  set_gdbarch_num_pseudo_regs (gdbarch, 0);
  tdep->mm0_regnum = -1;

  set_gdbarch_unwind_dummy_id (gdbarch, x86_64_unwind_dummy_id);

  /* FIXME: kettenis/20021026: This is ELF-specific.  Fine for now,
     since all supported x86-64 targets are ELF, but that might change
     in the future.  */
  set_gdbarch_in_solib_call_trampoline (gdbarch, in_plt_section);

  frame_unwind_append_sniffer (gdbarch, x86_64_sigtramp_frame_sniffer);
  frame_unwind_append_sniffer (gdbarch, x86_64_frame_sniffer);
  frame_base_set_default (gdbarch, &x86_64_frame_base);

  /* If we have a register mapping, enable the generic core file support.  */
  if (tdep->gregset_reg_offset)
    set_gdbarch_regset_from_core_section (gdbarch,
					  x86_64_regset_from_core_section);
}


#define I387_ST0_REGNUM X86_64_ST0_REGNUM

/* The 64-bit FXSAVE format differs from the 32-bit format in the
   sense that the instruction pointer and data pointer are simply
   64-bit offsets into the code segment and the data segment instead
   of a selector offset pair.  The functions below store the upper 32
   bits of these pointers (instead of just the 16-bits of the segment
   selector).  */

/* Fill register REGNUM in REGCACHE with the appropriate
   floating-point or SSE register value from *FXSAVE.  If REGNUM is
   -1, do this for all registers.  This function masks off any of the
   reserved bits in *FXSAVE.  */

void
x86_64_supply_fxsave (struct regcache *regcache, int regnum,
		      const void *fxsave)
{
  i387_supply_fxsave (regcache, regnum, fxsave);

  if (fxsave)
    {
      const char *regs = fxsave;

      if (regnum == -1 || regnum == I387_FISEG_REGNUM)
	regcache_raw_supply (regcache, I387_FISEG_REGNUM, regs + 12);
      if (regnum == -1 || regnum == I387_FOSEG_REGNUM)
	regcache_raw_supply (regcache, I387_FOSEG_REGNUM, regs + 20);
    }
}

/* Fill register REGNUM (if it is a floating-point or SSE register) in
   *FXSAVE with the value in GDB's register cache.  If REGNUM is -1, do
   this for all registers.  This function doesn't touch any of the
   reserved bits in *FXSAVE.  */

void
x86_64_fill_fxsave (char *fxsave, int regnum)
{
  i387_fill_fxsave (fxsave, regnum);

  if (regnum == -1 || regnum == I387_FISEG_REGNUM)
    regcache_collect (I387_FISEG_REGNUM, fxsave + 12);
  if (regnum == -1 || regnum == I387_FOSEG_REGNUM)
    regcache_collect (I387_FOSEG_REGNUM, fxsave + 20);
}
