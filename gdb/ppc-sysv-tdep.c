/* Target-dependent code for PowerPC systems using the SVR4 ABI
   for GDB, the GNU debugger.

   Copyright 2000, 2001, 2002 Free Software Foundation, Inc.

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
#include "gdbcore.h"
#include "inferior.h"
#include "regcache.h"
#include "value.h"
#include "gdb_string.h"

#include "ppc-tdep.h"

/* Ensure that X is aligned to an S byte boundary (assuming that S is
   a power of 2) rounding up/down where necessary.  */

static ULONGEST
align_up (ULONGEST x, int s)
{
  return (x + s - 1) & -s;
}

static ULONGEST
align_down (ULONGEST x, int s)
{
  return (x & -s);
}

/* Pass the arguments in either registers, or in the stack. Using the
   ppc sysv ABI, the first eight words of the argument list (that might
   be less than eight parameters if some parameters occupy more than one
   word) are passed in r3..r10 registers.  float and double parameters are
   passed in fpr's, in addition to that. Rest of the parameters if any
   are passed in user stack. 

   If the function is returning a structure, then the return address is passed
   in r3, then the first 7 words of the parametes can be passed in registers,
   starting from r4. */

CORE_ADDR
ppc_sysv_abi_push_dummy_call (struct gdbarch *gdbarch, CORE_ADDR func_addr,
			      struct regcache *regcache, CORE_ADDR bp_addr,
			      int nargs, struct value **args, CORE_ADDR sp,
			      int struct_return, CORE_ADDR struct_addr)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  const CORE_ADDR saved_sp = read_sp ();
  int argspace = 0;		/* 0 is an initial wrong guess.  */
  int write_pass;

  /* Go through the argument list twice.

     Pass 1: Figure out how much new stack space is required for
     arguments and pushed values.  Unlike the PowerOpen ABI, the SysV
     ABI doesn't reserve any extra space for parameters which are put
     in registers, but does always push structures and then pass their
     address.

     Pass 2: Replay the same computation but this time also write the
     values out to the target.  */

  for (write_pass = 0; write_pass < 2; write_pass++)
    {
      int argno;
      /* Next available floating point register for float and double
         arguments.  */
      int freg = 1;
      /* Next available general register for non-float, non-vector
         arguments.  */
      int greg = 3;
      /* Next available vector register for vector arguments.  */
      int vreg = 2;
      /* Arguments start above the "LR save word" and "Back chain".  */
      int argoffset = 2 * tdep->wordsize;
      /* Structures start after the arguments.  */
      int structoffset = argoffset + argspace;

      /* If the function is returning a `struct', then the first word
	 (which will be passed in r3) is used for struct return
	 address.  In that case we should advance one word and start
	 from r4 register to copy parameters.  */
      if (struct_return)
	{
	  if (write_pass)
	    regcache_cooked_write_signed (regcache,
					  tdep->ppc_gp0_regnum + greg,
					  struct_addr);
	  greg++;
	}

      for (argno = 0; argno < nargs; argno++)
	{
	  struct value *arg = args[argno];
	  struct type *type = check_typedef (VALUE_TYPE (arg));
	  int len = TYPE_LENGTH (type);
	  char *val = VALUE_CONTENTS (arg);

	  if (TYPE_CODE (type) == TYPE_CODE_FLT
	      && ppc_floating_point_unit_p (current_gdbarch)
	      && len <= 8)
	    {
	      /* Floating point value converted to "double" then
                 passed in an FP register, when the registers run out,
                 8 byte aligned stack is used.  */
	      if (freg <= 8)
		{
		  if (write_pass)
		    {
		      /* Always store the floating point value using
                         the register's floating-point format.  */
		      char regval[MAX_REGISTER_SIZE];
		      struct type *regtype
			= register_type (gdbarch, FP0_REGNUM + freg);
		      convert_typed_floating (val, type, regval, regtype);
		      regcache_cooked_write (regcache, FP0_REGNUM + freg,
					     regval);
		    }
		  freg++;
		}
	      else
		{
		  /* SysV ABI converts floats to doubles before
                     writing them to an 8 byte aligned stack location.  */
		  argoffset = align_up (argoffset, 8);
		  if (write_pass)
		    {
		      char memval[8];
		      struct type *memtype;
		      switch (TARGET_BYTE_ORDER)
			{
			case BFD_ENDIAN_BIG:
			  memtype = builtin_type_ieee_double_big;
			  break;
			case BFD_ENDIAN_LITTLE:
			  memtype = builtin_type_ieee_double_little;
			  break;
			default:
			  internal_error (__FILE__, __LINE__, "bad switch");
			}
		      convert_typed_floating (val, type, memval, memtype);
		      write_memory (sp + argoffset, val, len);
		    }
		  argoffset += 8;
		}
	    }
	  else if (len == 8 
		   && (TYPE_CODE (type) == TYPE_CODE_INT /* long long */
		       || (!ppc_floating_point_unit_p (current_gdbarch)
			   && TYPE_CODE (type) == TYPE_CODE_FLT))) /* double */
	    {
	      /* "long long" or "double" passed in an odd/even
                 register pair with the low addressed word in the odd
                 register and the high addressed word in the even
                 register, or when the registers run out an 8 byte
                 aligned stack location.  */
	      if (greg > 9)
		{
		  /* Just in case GREG was 10.  */
		  greg = 11;
		  argoffset = align_up (argoffset, 8);
		  if (write_pass)
		    write_memory (sp + argoffset, val, len);
		  argoffset += 8;
		}
	      else if (tdep->wordsize == 8)
		{
		  if (write_pass)
		    regcache_cooked_write (regcache,
					   tdep->ppc_gp0_regnum + greg,
					   val);
		  greg += 1;
		}
	      else
		{
		  /* Must start on an odd register - r3/r4 etc.  */
		  if ((greg & 1) == 0)
		    greg++;
		  if (write_pass)
		    {
		      regcache_cooked_write (regcache,
					     tdep->ppc_gp0_regnum + greg + 0,
					     val + 0);
		      regcache_cooked_write (regcache,
					     tdep->ppc_gp0_regnum + greg + 1,
					     val + 4);
		    }
		  greg += 2;
		}
	    }
	  else if (len == 16
		   && TYPE_CODE (type) == TYPE_CODE_ARRAY
		   && TYPE_VECTOR (type)
		   && tdep->ppc_vr0_regnum >= 0)
	    {
	      /* Vector parameter passed in an Altivec register, or
                 when that runs out, 16 byte aligned stack location.  */
	      if (vreg <= 13)
		{
		  if (write_pass)
		    regcache_cooked_write (current_regcache,
					   tdep->ppc_vr0_regnum + vreg,
					   val);
		  vreg++;
		}
	      else
		{
		  argoffset = align_up (argoffset, 16);
		  if (write_pass)
		    write_memory (sp + argoffset, val, 16);
		  argoffset += 16;
		}
	    }
          else if (len == 8 
		   && TYPE_CODE (type) == TYPE_CODE_ARRAY
		   && TYPE_VECTOR (type)
		   && tdep->ppc_ev0_regnum >= 0)
            {
	      /* Vector parameter passed in an e500 register, or when
                 that runs out, 8 byte aligned stack location.  Note
                 that since e500 vector and general purpose registers
                 both map onto the same underlying register set, a
                 "greg" and not a "vreg" is consumed here.  A cooked
                 write stores the value in the correct locations
                 within the raw register cache.  */
              if (greg <= 10)
                {
		  if (write_pass)
		    regcache_cooked_write (current_regcache,
					   tdep->ppc_ev0_regnum + greg,
					   val);
                  greg++;
                }
              else
                {
		  argoffset = align_up (argoffset, 8);
		  if (write_pass)
		    write_memory (sp + argoffset, val, 8);
                  argoffset += 8;
                }
            }
	  else
	    {
	      /* Reduce the parameter down to something that fits in a
                 "word".  */
	      char word[MAX_REGISTER_SIZE];
	      memset (word, 0, MAX_REGISTER_SIZE);
	      if (len > tdep->wordsize
		  || TYPE_CODE (type) == TYPE_CODE_STRUCT
		  || TYPE_CODE (type) == TYPE_CODE_UNION)
		{
		  /* Structs and large values are put on an 8 byte
                     aligned stack ... */
		  structoffset = align_up (structoffset, 8);
		  if (write_pass)
		    write_memory (sp + structoffset, val, len);
		  /* ... and then a "word" pointing to that address is
                     passed as the parameter.  */
		  store_unsigned_integer (word, tdep->wordsize,
					  sp + structoffset);
		  structoffset += len;
		}
	      else if (TYPE_CODE (type) == TYPE_CODE_INT)
		/* Sign or zero extend the "int" into a "word".  */
		store_unsigned_integer (word, tdep->wordsize,
					unpack_long (type, val));
	      else
		/* Always goes in the low address.  */
		memcpy (word, val, len);
	      /* Store that "word" in a register, or on the stack.
                 The words have "4" byte alignment.  */
	      if (greg <= 10)
		{
		  if (write_pass)
		    regcache_cooked_write (regcache,
					   tdep->ppc_gp0_regnum + greg,
					   word);
		  greg++;
		}
	      else
		{
		  argoffset = align_up (argoffset, tdep->wordsize);
		  if (write_pass)
		    write_memory (sp + argoffset, word, tdep->wordsize);
		  argoffset += tdep->wordsize;
		}
	    }
	}

      /* Compute the actual stack space requirements.  */
      if (!write_pass)
	{
	  /* Remember the amount of space needed by the arguments.  */
	  argspace = argoffset;
	  /* Allocate space for both the arguments and the structures.  */
	  sp -= (argoffset + structoffset);
	  /* Ensure that the stack is still 16 byte aligned.  */
	  sp = align_down (sp, 16);
	}
    }

  /* Update %sp.   */
  regcache_cooked_write_signed (regcache, SP_REGNUM, sp);

  /* Write the backchain (it occupies WORDSIZED bytes).  */
  write_memory_signed_integer (sp, tdep->wordsize, saved_sp);

  /* Point the inferior function call's return address at the dummy's
     breakpoint.  */
  regcache_cooked_write_signed (regcache, tdep->ppc_lr_regnum, bp_addr);

  return sp;
}

/* Until November 2001, gcc was not complying to the SYSV ABI for 
   returning structures less than or equal to 8 bytes in size.  It was
   returning everything in memory.  When this was corrected, it wasn't
   fixed for native platforms.  */
int     
ppc_sysv_abi_broken_use_struct_convention (int gcc_p, struct type *value_type)
{  
  if ((TYPE_LENGTH (value_type) == 16 || TYPE_LENGTH (value_type) == 8)
      && TYPE_VECTOR (value_type))
    return 0;                            

  return generic_use_struct_convention (gcc_p, value_type);
}

/* Structures 8 bytes or less long are returned in the r3 & r4
   registers, according to the SYSV ABI. */
int
ppc_sysv_abi_use_struct_convention (int gcc_p, struct type *value_type)
{
  if ((TYPE_LENGTH (value_type) == 16 || TYPE_LENGTH (value_type) == 8)
      && TYPE_VECTOR (value_type))
    return 0;

  return (TYPE_LENGTH (value_type) > 8);
}   
