/* Copyright (C) 1988, 1990 Free Software Foundation, Inc.

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
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include "defs.h"
#include "param.h"
#include "frame.h"
#include "inferior.h"
#include "value.h"

#ifdef USG
#include <sys/types.h>
#endif

#include <sys/param.h>
#include <sys/dir.h>
#include <signal.h>
#include "gdbcore.h"
#include <sys/user.h>
#ifndef USER			/* added to support BCS ptrace_user */

#define USER ptrace_user
#endif
#include <sys/ioctl.h>
#include <fcntl.h>

#ifdef COFF_ENCAPSULATE
#include "a.out.encap.h"
#else
#include <a.out.h>
#endif

#include <sys/file.h>
#include <sys/stat.h>

#include "symtab.h"
#include "setjmp.h"
#include "value.h"

int stack_error;
jmp_buf stack_jmp;

void
tdesc_error_function  (environment, continuable, message)
dc_word_t environment;
dc_boolean_t continuable;
char *message;
{
  if (stack_error) longjmp (stack_jmp, 1);
  if (!continuable)
    {
      printf("%s\n",message);
      abort();
    }
}


void
tdesc_read_function (environment, memory, length, buffer)
dc_word_t environment;
dc_word_t memory;
int length;
char *buffer;
{
  int ptrace_code;
  errno = 0;
  if (memory < 2048) 
#if 0
    /* This is a no-op!  It sets buffer, but doesn't do anything to
       what buffer points to.  What does this function do anyway?
       And this is wrong for cross-debugging.  */
    buffer = ptrace (3, inferior_pid, memory, 0);
#else
    return;
#endif
  else
    read_memory (memory, buffer, length);
}

/* Map function for tdesc */
void
tdesc_map_function (map_env, loc, map_info_in, map_info_out)
dc_word_t map_env;
dc_word_t loc;
dc_map_info_in_t map_info_in;
dc_map_info_out_t *map_info_out;
{
int map_flags = DC_MIO_ENTRY_POINT | DC_MIO_IMPLICIT_PROLOGUE_END;
int entry_point = get_pc_function_start(loc);
map_info_out->flags = map_flags;
map_info_out->entry_point = entry_point;
}

dc_handle_t tdesc_handle;

extern int debug_info;

void
init_tdesc ()
{
  tdesc_handle = dc_initiate (debug_info, tdesc_error_function,
			      0,tdesc_read_function,0,0,0,0,0,tdesc_map_function,0);
} 
dc_dcontext_t current_context;
  
/* setup current context, called from wait_for_inferior */

dc_dcontext_t
init_dcontext()
{
  dc_word_t reg_info[DC_NUM_REG];
  dc_word_t reg_flags[2] = {0,-1};
  dc_word_t aux_info[DC_NUM_AUX];
  dc_word_t aux_flags[2] = {0,-1};
  dc_exactness_t loc_exact = DC_NO;
  dc_word_t psr_info;
  dc_boolean_t psr_ind = 0;
  dc_word_t psr_flags[2] = {0,-1};

  bcopy (&registers, reg_info, DC_NUM_REG * 4);
  aux_info[DC_AUX_LOC] =  read_register(SXIP_REGNUM);
  aux_info[DC_AUX_SXIP] = read_register(SXIP_REGNUM);
  aux_info[DC_AUX_SNIP] = read_register(SNIP_REGNUM);
  aux_info[DC_AUX_SFIP] = read_register(SFIP_REGNUM);
  aux_info[DC_AUX_FPSR] = read_register(FPSR_REGNUM);
  aux_info[DC_AUX_FPCR] = read_register(FPCR_REGNUM);

  psr_info = read_register(PSR_REGNUM);

  return dc_make_dcontext (tdesc_handle, reg_info, reg_flags, aux_info,
                         aux_flags, loc_exact, psr_info, psr_ind, psr_flags);
}


dc_dcontext_t
get_prev_context (context)
  dc_dcontext_t context;
{
  return current_context = dc_previous_dcontext (context); 
}
  



/* Determine frame base for this file's frames.  This will be either
   the CFA or the old style FP_REGNUM; the symtab for the current pc's
   file has the information					      */

CORE_ADDR
get_frame_base(pc)
CORE_ADDR pc;
{
  struct symtab *this_file = find_pc_symtab(pc);
  int coffsem_frame_position;
  
  /* If this_file is null, there's a good chance the file was compiled
     without -g.  If that's the case, use CFA (canonical frame addr) 
     as the default frame pointer.                                 */

  if (this_file)
    {
    coffsem_frame_position = this_file->coffsem & 3;
    if (coffsem_frame_position == 1) 
      return (CORE_ADDR) dc_general_register (current_context, FP_REGNUM);
    else
      /* default is CFA, as well as if coffsem==2 */
      return (CORE_ADDR) dc_frame_address  (current_context);
    }

  return (CORE_ADDR) dc_frame_address (current_context);
}

#if TARGET_BYTE_ORDER != HOST_BYTE_ORDER
you lose
#else /* Host and target byte order the same.  */
#define SINGLE_EXP_BITS  8
#define DOUBLE_EXP_BITS 11
int
IEEE_isNAN(fp, len)
     int *fp, len;
     /* fp points to a single precision OR double precision
      * floating point value; len is the number of bytes, either 4 or 8.
      * Returns 1 iff fp points to a valid IEEE floating point number.
      * Returns 0 if fp points to a denormalized number or a NaN
      */
{
  int exponent;
  if (len == 4)
    {
      exponent = *fp;
      exponent = exponent << 1 >> (32 - SINGLE_EXP_BITS - 1);
      return ((exponent == -1) || (! exponent && *fp));
    }
  else if (len == 8)
    {
      exponent = *(fp+1);
      exponent = exponent << 1 >> (32 - DOUBLE_EXP_BITS - 1);
      return ((exponent == -1) || (! exponent && *fp * *(fp+1)));
    }
  else return 1;
}
#endif /* Host and target byte order the same.  */

#define FIRST_PRESERVED_REGNUM	14
#define LAST_PRESERVED_REGNUM	25
#define FIRST_PARM_REGNUM	2
#define LAST_PARM_REGNUM	9

#define MAX_REG_PARMS	(LAST_PARM_REGNUM - FIRST_PARM_REGNUM + 1)

void
frame_find_saved_regs (fi, fsr)
      struct frame_info *fi;
      struct frame_saved_regs *fsr;
{
   register int regnum;
 
   error ("Feature not implemented for the 88k yet.");
   return;
 
#if 0
   for (regnum = FIRST_PARM_REGNUM; regnum <= LAST_PARM_REGNUM; regnum++)
     fsr->regs[regnum]
        = (unsigned) fi->frame - ((regnum - FIRST_PARM_REGNUM) * 4);
 
   fsr->regs[SP_REGNUM] = 0;		/* SP not saved in frames */
   fsr->regs[FP_REGNUM] = fi->frame;
   fsr->regs[PC_REGNUM] = fi->frame + 4;
#endif
}

static int
pushed_size (prev_words, v)
     int prev_words;
     struct value *v;
{
  switch (TYPE_CODE (VALUE_TYPE (v)))
    {
      case TYPE_CODE_VOID:		/* Void type (values zero length) */

	return 0;	/* That was easy! */

      case TYPE_CODE_PTR:		/* Pointer type */
      case TYPE_CODE_ENUM:		/* Enumeration type */
      case TYPE_CODE_INT:		/* Integer type */
      case TYPE_CODE_REF:		/* C++ Reference types */
      case TYPE_CODE_ARRAY:		/* Array type, lower bound zero */

	return 1;

      case TYPE_CODE_FLT:		/* Floating type */

	if (TYPE_LENGTH (VALUE_TYPE (v)) == 4)
	  return 1;
	else
	  /* Assume that it must be a double.  */
	  if (prev_words & 1)		/* at an odd-word boundary */
	    return 3;			/* round to 8-byte boundary */
	  else
	    return 2;

      case TYPE_CODE_STRUCT:		/* C struct or Pascal record */
      case TYPE_CODE_UNION:		/* C union or Pascal variant part */

	return (((TYPE_LENGTH (VALUE_TYPE (v)) + 3) / 4) * 4);

      case TYPE_CODE_FUNC:		/* Function type */
      case TYPE_CODE_SET:		/* Pascal sets */
      case TYPE_CODE_RANGE:		/* Range (integers within bounds) */
      case TYPE_CODE_PASCAL_ARRAY:	/* Array with explicit type of index */
      case TYPE_CODE_MEMBER:		/* Member type */
      case TYPE_CODE_METHOD:		/* Method type */
	/* Don't know how to pass these yet.  */

      case TYPE_CODE_UNDEF:		/* Not used; catches errors */
      default:
	abort ();
    }
}

static void
store_parm_word (address, val)
     CORE_ADDR address;
     int val;
{
  write_memory (address, &val, 4);
}

static int
store_parm (prev_words, left_parm_addr, v)
     unsigned int prev_words;
     CORE_ADDR left_parm_addr;
     struct value *v;
{
  CORE_ADDR start = left_parm_addr + (prev_words * 4);
  int *val_addr = (int *)VALUE_CONTENTS(v);

  switch (TYPE_CODE (VALUE_TYPE (v)))
    {
      case TYPE_CODE_VOID:		/* Void type (values zero length) */

	return 0;

      case TYPE_CODE_PTR:		/* Pointer type */
      case TYPE_CODE_ENUM:		/* Enumeration type */
      case TYPE_CODE_INT:		/* Integer type */
      case TYPE_CODE_ARRAY:		/* Array type, lower bound zero */
      case TYPE_CODE_REF:		/* C++ Reference types */

	store_parm_word (start, *val_addr);
	return 1;

      case TYPE_CODE_FLT:		/* Floating type */

	if (TYPE_LENGTH (VALUE_TYPE (v)) == 4)
	  {
	    store_parm_word (start, *val_addr);
	    return 1;
	  }
	else
	  {
	    store_parm_word (start + ((prev_words & 1) * 4), val_addr[0]);
	    store_parm_word (start + ((prev_words & 1) * 4) + 4, val_addr[1]);
	    return 2 + (prev_words & 1);
	  }

      case TYPE_CODE_STRUCT:		/* C struct or Pascal record */
      case TYPE_CODE_UNION:		/* C union or Pascal variant part */

	{
	  unsigned int words = (((TYPE_LENGTH (VALUE_TYPE (v)) + 3) / 4) * 4);
	  unsigned int word;

	  for (word = 0; word < words; word++)
	    store_parm_word (start + (word * 4), val_addr[word]);
	  return words;
	}

      default:
	abort ();
    }
}

 /* This routine sets up all of the parameter values needed to make a pseudo
    call.  The name "push_parameters" is a misnomer on some archs,
    because (on the m88k) most parameters generally end up being passed in
    registers rather than on the stack.  In this routine however, we do
    end up storing *all* parameter values onto the stack (even if we will
    realize later that some of these stores were unnecessary).  */

void
push_parameters (return_type, struct_conv, nargs, args)
      struct type *return_type; 
      int struct_conv;
      int nargs;
      value *args;
 {
   int parm_num;
   unsigned int p_words = 0;
   CORE_ADDR left_parm_addr;
 
   /* Start out by creating a space for the return value (if need be).  We
      only need to do this if the return value is a struct or union.  If we
      do make a space for a struct or union return value, then we must also
      arrange for the base address of that space to go into r12, which is the
      standard place to pass the address of the return value area to the
      callee.  Note that only structs and unions are returned in this fashion.
      Ints, enums, pointers, and floats are returned into r2.  Doubles are
      returned into the register pair {r2,r3}.  Note also that the space
      reserved for a struct or union return value only has to be word aligned
      (not double-word) but it is double-word aligned here anyway (just in
      case that becomes important someday).  */
 
   switch (TYPE_CODE (return_type))
     {
       case TYPE_CODE_STRUCT:
       case TYPE_CODE_UNION:
         {
           int return_bytes = ((TYPE_LENGTH (return_type) + 7) / 8) * 8;
           CORE_ADDR rv_addr;
 
           rv_addr = read_register (SP_REGNUM) - return_bytes;
 
           write_register (SP_REGNUM, rv_addr); /* push space onto the stack */
           write_register (SRA_REGNUM, rv_addr);/* set return value register */
         }
     }
 
   /* Here we make a pre-pass on the whole parameter list to figure out exactly
      how many words worth of stuff we are going to pass.  */
 
   for (p_words = 0, parm_num = 0; parm_num < nargs; parm_num++)
     p_words += pushed_size (p_words, value_arg_coerce (args[parm_num]));
 
   /* Now, check to see if we have to round up the number of parameter words
      to get up to the next 8-bytes boundary.  This may be necessary because
      of the software convention to always keep the stack aligned on an 8-byte
      boundary.  */
 
   if (p_words & 1)
     p_words++;		/* round to 8-byte boundary */
 
   /* Now figure out the absolute address of the leftmost parameter, and update
      the stack pointer to point at that address.  */
 
   left_parm_addr = read_register (SP_REGNUM) - (p_words * 4);
   write_register (SP_REGNUM, left_parm_addr);
 
   /* Now we can go through all of the parameters (in left-to-right order)
      and write them to their parameter stack slots.  Note that we are not
      really "pushing" the parameter values.  The stack space for these values
      was already allocated above.  Now we are just filling it up.  */
 
   for (p_words = 0, parm_num = 0; parm_num < nargs; parm_num++)
     p_words +=
       store_parm (p_words, left_parm_addr, value_arg_coerce (args[parm_num]));
 
   /* Now that we are all done storing the parameter values into the stack, we
      must go back and load up the parameter registers with the values from the
      corresponding stack slots.  Note that in the two cases of (a) gaps in the
      parameter word sequence causes by (otherwise) misaligned doubles, and (b)
      slots correcponding to structs or unions, the work we do here in loading
      some parameter registers may be unnecessary, but who cares?  */
 
   for (p_words = 0; p_words < 8; p_words++)
     {
       write_register (FIRST_PARM_REGNUM + p_words,
         read_memory_integer (left_parm_addr + (p_words * 4), 4));
     }
}

void
pop_frame ()
{
  error ("Feature not implemented for the m88k yet.");
  return;
}

 void
 collect_returned_value (rval, value_type, struct_return, nargs, args)
      value *rval;
      struct type *value_type;
      int struct_return;
      int nargs;
      value *args;
 {
   char retbuf[REGISTER_BYTES];
 
   bcopy (registers, retbuf, REGISTER_BYTES);
   *rval = value_being_returned (value_type, retbuf, struct_return);
   return;
 }

#if 0
/* Now handled in a machine independent way with CALL_DUMMY_LOCATION.  */
 /* Stuff a breakpoint instruction onto the stack (or elsewhere if the stack
    is not a good place for it).  Return the address at which the instruction
    got stuffed, or zero if we were unable to stuff it anywhere.  */
  
 CORE_ADDR
 push_breakpoint ()
  {
   static char breakpoint_insn[] = BREAKPOINT;
   extern CORE_ADDR text_end;	/* of inferior */
   static char readback_buffer[] = BREAKPOINT;
   int i;
  
   /* With a little bit of luck, we can just stash the breakpoint instruction
      in the word just beyond the end of normal text space.  For systems on
      which the hardware will not allow us to execute out of the stack segment,
      we have to hope that we *are* at least allowed to effectively extend the
      text segment by one word.  If the actual end of user's the text segment
      happens to fall right at a page boundary this trick may fail.  Note that
      we check for this by reading after writing, and comparing in order to
      be sure that the write worked.  */
 
   write_memory (text_end, &breakpoint_insn, 4);
 
   /* Fill the readback buffer with some garbage which is certain to be
      unequal to the breakpoint insn.  That way we can tell if the
      following read doesn't actually succeed.  */
 
   for (i = 0; i < sizeof (readback_buffer); i++)
     readback_buffer[i] = ~ readback_buffer[i];	/* Invert the bits */
 
   /* Now check that the breakpoint insn was successfully installed.  */
 
   read_memory (text_end, readback_buffer, sizeof (readback_buffer));
   for (i = 0; i < sizeof (readback_buffer); i++)
     if (readback_buffer[i] != breakpoint_insn[i])
       return 0;		/* Failed to install! */
 
   return text_end;
 }
#endif

/* Like dc_psr_register but takes an extra int arg.  */
static dc_word_t
psr_register (context, dummy)
     dc_dcontext_t context;
     int dummy;
{
  return dc_psr_register (context);
}

/* Same functionality as get_saved_register in findvar.c, but implemented
   to use tdesc.  */
void
get_saved_register (raw_buffer, optim, addrp, frame, regnum, lvalp)
     char *raw_buffer;
     int *optim;
     CORE_ADDR *addrp;
     FRAME frame;
     int regnum;
     enum lval_type *lvalp;
{
  struct frame_info *fi = get_frame_info (frame);
  
  /* Functions to say whether a register is optimized out, and
     if not, to get the value.  Take as args a context and the
     value of get_reg_arg.  */
  int (*get_reg_state) ();
  dc_word_t (*get_reg) ();
  int get_reg_arg;

  /* Because tdesc doesn't tell us whether it got it from a register
     or memory, always say we don't have an address for it.  */
  if (addrp != NULL)
    *addrp = 0;
  
  if (regnum < DC_NUM_REG)
    {
      get_reg_state = dc_general_register_state;
      get_reg = dc_general_register;
      get_reg_arg = regnum;
    }
  else
    {
      get_reg_state = dc_auxiliary_register_state;
      get_reg = dc_auxiliary_register;
      switch (regnum)
	{
	case SXIP_REGNUM:
	  get_reg_arg = DC_AUX_SXIP;
	  break;
	case SNIP_REGNUM:
	  get_reg_arg = DC_AUX_SNIP;
	  break;
	case FPSR_REGNUM:
	  get_reg_arg = DC_AUX_FPSR;
	  break;
	case FPCR_REGNUM:
	  get_reg_arg = DC_AUX_FPCR;
	  break;
	case PSR_REGNUM:
	  get_reg_state = dc_psr_register_bit_state;
	  get_reg = psr_register;
	  get_reg_arg = 0;
	  break;
	default:
	  if (optim != NULL)
	    *optim = 1;
	  return;
	}
    }

  if ((*get_reg_state) (fi->frame_context, get_reg_arg))
    {
      if (raw_buffer != NULL)
	*(int *)raw_buffer = (*get_reg) (fi->frame_context, get_reg_arg);
      if (optim != NULL)
	*optim = 0;
      return;
    }
  else
    {
      if (optim != NULL)
	*optim = 1;
      return;
    }

  /* Well, the caller can't treat it as a register or memory...  */
  if (lvalp != NULL)
    *lvalp = not_lval;
}
