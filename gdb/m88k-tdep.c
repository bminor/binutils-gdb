/* Target-machine dependent code for Motorola 88000 series, for GDB.
   Copyright (C) 1988, 1990, 1991 Free Software Foundation, Inc.

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

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "value.h"
#include "gdbcore.h"

#include "symtab.h"
#include "setjmp.h"
#include "value.h"
#include "ieee-float.h"	/* for ext_format & friends */

/* Size of an instruction */
#define	BYTES_PER_88K_INSN	4

void frame_find_saved_regs ();

/* is this target an m88110?  Otherwise assume m88100.  This has
   relevance for the ways in which we screw with instruction pointers.  */ 
int target_is_m88110 = 0;

/* FIXME: this is really just a guess based on m88110 being big
   endian. */
const struct ext_format ext_format_m88110 = {
/* tot sbyte smask expbyte	manbyte */
   10, 0,    0x80, 0,1,		4,8		/* m88110 */
};

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */

FRAME_ADDR
frame_chain (thisframe)
     FRAME thisframe;
{

  frame_find_saved_regs (thisframe, (struct frame_saved_regs *) 0);
  /* NOTE:  this depends on frame_find_saved_regs returning the VALUE, not
 	    the ADDRESS, of SP_REGNUM.  It also depends on the cache of
	    frame_find_saved_regs results.  */
  if (thisframe->fsr->regs[SP_REGNUM])
    return thisframe->fsr->regs[SP_REGNUM];
  else
    return thisframe->frame;	/* Leaf fn -- next frame up has same SP. */
}

int
frameless_function_invocation (frame)
     FRAME frame;
{

  frame_find_saved_regs (frame, (struct frame_saved_regs *) 0);
  /* NOTE:  this depends on frame_find_saved_regs returning the VALUE, not
 	    the ADDRESS, of SP_REGNUM.  It also depends on the cache of
	    frame_find_saved_regs results.  */
  if (frame->fsr->regs[SP_REGNUM])
    return 0;			/* Frameful -- return addr saved somewhere */
  else
    return 1;			/* Frameless -- no saved return address */
}

void
init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  fi->fsr = 0;			/* Not yet allocated */
  fi->args_pointer = 0;		/* Unknown */
  fi->locals_pointer = 0;	/* Unknown */
}

/* Examine an m88k function prologue, recording the addresses at which
   registers are saved explicitly by the prologue code, and returning
   the address of the first instruction after the prologue (but not
   after the instruction at address LIMIT, as explained below).

   LIMIT places an upper bound on addresses of the instructions to be
   examined.  If the prologue code scan reaches LIMIT, the scan is
   aborted and LIMIT is returned.  This is used, when examining the
   prologue for the current frame, to keep examine_prologue () from
   claiming that a given register has been saved when in fact the
   instruction that saves it has not yet been executed.  LIMIT is used
   at other times to stop the scan when we hit code after the true
   function prologue (e.g. for the first source line) which might
   otherwise be mistaken for function prologue.

   The format of the function prologue matched by this routine is
   derived from examination of the source to gcc 1.95, particularly
   the routine output_prologue () in config/out-m88k.c.

   subu r31,r31,n			# stack pointer update

   (st rn,r31,offset)?			# save incoming regs
   (st.d rn,r31,offset)?

   (addu r30,r31,n)?			# frame pointer update

   (pic sequence)?			# PIC code prologue

   (or   rn,rm,0)?			# Move parameters to other regs
*/

/* Macros for extracting fields from instructions.  */

#define BITMASK(pos, width) (((0x1 << (width)) - 1) << (pos))
#define EXTRACT_FIELD(val, pos, width) ((val) >> (pos) & BITMASK (0, width))

/* Prologue code that handles position-independent-code setup.  */

struct pic_prologue_code {
  unsigned long insn, mask;
};

static struct pic_prologue_code pic_prologue_code [] = {
/* FIXME -- until this is translated to hex, we won't match it... */
  { 0xffffffff, 0 },
					/* or r10,r1,0  (if not saved) */
					/* bsr.n LabN */
					/* or.u r25,r0,const */
					/*LabN: or r25,r25,const2 */
					/* addu r25,r25,1 */
					/* or r1,r10,0  (if not saved) */
};

/* Fetch the instruction at ADDR, returning 0 if ADDR is beyond LIM or
   is not the address of a valid instruction, the address of the next
   instruction beyond ADDR otherwise.  *PWORD1 receives the first word
   of the instruction.  PWORD2 is ignored -- a remnant of the original
   i960 version.  */

#define NEXT_PROLOGUE_INSN(addr, lim, pword1) \
  (((addr) < (lim)) ? next_insn (addr, pword1) : 0)

/* Read the m88k instruction at 'memaddr' and return the address of 
   the next instruction after that, or 0 if 'memaddr' is not the
   address of a valid instruction.  The instruction
   is stored at 'pword1'.  */

CORE_ADDR
next_insn (memaddr, pword1)
     unsigned long *pword1;
     CORE_ADDR memaddr;
{
  *pword1 = read_memory_integer (memaddr, BYTES_PER_88K_INSN);
  return memaddr + BYTES_PER_88K_INSN;
}

/* Read a register from frames called by us (or from the hardware regs).  */

static int
read_next_frame_reg(fi, regno)
     FRAME fi;
     int regno;
{
  for (; fi; fi = fi->next) {
      if (regno == SP_REGNUM) return fi->frame;
      else if (fi->fsr->regs[regno])
	return read_memory_integer(fi->fsr->regs[regno], 4);
  }
  return read_register(regno);
}

/* Examine the prologue of a function.  `ip' points to the first instruction.
   `limit' is the limit of the prologue (e.g. the addr of the first 
   linenumber, or perhaps the program counter if we're stepping through).
   `frame_sp' is the stack pointer value in use in this frame.  
   `fsr' is a pointer to a frame_saved_regs structure into which we put
   info about the registers saved by this frame.  
   `fi' is a struct frame_info pointer; we fill in various fields in it
   to reflect the offsets of the arg pointer and the locals pointer.  */

static CORE_ADDR
examine_prologue (ip, limit, frame_sp, fsr, fi)
     register CORE_ADDR ip;
     register CORE_ADDR limit;
     FRAME_ADDR frame_sp;
     struct frame_saved_regs *fsr;
     struct frame_info *fi;
{
  register CORE_ADDR next_ip;
  register int src;
  register struct pic_prologue_code *pcode;
  unsigned int insn;
  int size, offset;
  char must_adjust[32];		/* If set, must adjust offsets in fsr */
  int sp_offset = -1;		/* -1 means not set (valid must be mult of 8) */
  int fp_offset = -1;		/* -1 means not set */
  CORE_ADDR frame_fp;

  memset (must_adjust, '\0', sizeof (must_adjust));
  next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn);

  /* Accept move of incoming registers to other registers, using
     "or rd,rs,0" or "or.u rd,rs,0" or "or rd,r0,rs" or "or rd,rs,r0".
     We don't have to worry about walking into the first lines of code,
     since the first line number will stop us (assuming we have symbols).
     What we have actually seen is "or r10,r0,r12".  */

#define	OR_MOVE_INSN	0x58000000		/* or/or.u with immed of 0 */
#define	OR_MOVE_MASK	0xF800FFFF
#define	OR_REG_MOVE1_INSN	0xF4005800	/* or rd,r0,rs */
#define	OR_REG_MOVE1_MASK	0xFC1FFFE0
#define	OR_REG_MOVE2_INSN	0xF4005800	/* or rd,rs,r0 */
#define	OR_REG_MOVE2_MASK	0xFC00FFFF
  while (next_ip && 
	 ((insn & OR_MOVE_MASK) == OR_MOVE_INSN ||
	  (insn & OR_REG_MOVE1_MASK) == OR_REG_MOVE1_INSN ||
	  (insn & OR_REG_MOVE2_MASK) == OR_REG_MOVE2_INSN
	 )
	)
    {
      /* We don't care what moves to where.  The result of the moves 
 	 has already been reflected in what the compiler tells us is the
	 location of these parameters.  */
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn);
    }

  /* Accept an optional "subu sp,sp,n" to set up the stack pointer.  */

#define	SUBU_SP_INSN	0x67ff0000
#define	SUBU_SP_MASK	0xffff0007	/* Note offset must be mult. of 8 */
#define	SUBU_OFFSET(x)	((unsigned)(x & 0xFFFF))
  if (next_ip &&
      ((insn & SUBU_SP_MASK) == SUBU_SP_INSN))	/* subu r31, r31, N */
    {
      sp_offset = -SUBU_OFFSET (insn);
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn);
    }

  /* The function must start with a stack-pointer adjustment, or
     we don't know WHAT'S going on... */
  if (sp_offset == -1)
    return ip;

  /* Accept zero or more instances of "st rx,sp,n" or "st.d rx,sp,n".  
     This may cause us to mistake the copying of a register
     parameter to the frame for the saving of a callee-saved
     register, but that can't be helped, since with the
     "-fcall-saved" flag, any register can be made callee-saved.
     This probably doesn't matter, since the ``saved'' caller's values of
     non-callee-saved registers are not relevant anyway.  */

#define	STD_STACK_INSN	0x201f0000
#define	STD_STACK_MASK	0xfc1f0000
#define	ST_STACK_INSN	0x241f0000
#define	ST_STACK_MASK	0xfc1f0000
#define	ST_OFFSET(x)	((unsigned)((x) & 0xFFFF))
#define	ST_SRC(x)	EXTRACT_FIELD ((x), 21, 5)

  while (next_ip)
    {
           if ((insn & ST_STACK_MASK)  == ST_STACK_INSN)
 	size = 1;
      else if ((insn & STD_STACK_MASK) == STD_STACK_INSN)
	size = 2;
      else
	break;

      src = ST_SRC (insn);
      offset = ST_OFFSET (insn);
      while (size--)
	{
	  must_adjust[src] = 1;
	  fsr->regs[src++] = offset;		/* Will be adjusted later */
	  offset += 4;
	}
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn);
    }

  /* Accept an optional "addu r30,r31,n" to set up the frame pointer.  */

#define	ADDU_FP_INSN	0x63df0000
#define	ADDU_FP_MASK	0xffff0000
#define	ADDU_OFFSET(x)	((unsigned)(x & 0xFFFF))
  if (next_ip &&
      ((insn & ADDU_FP_MASK) == ADDU_FP_INSN))	/* addu r30, r31, N */
    {
      fp_offset = ADDU_OFFSET (insn);
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn);
    }

  /* Accept the PIC prologue code if present.  */

  pcode = pic_prologue_code;
  size = sizeof (pic_prologue_code) / sizeof (*pic_prologue_code);
  /* If return addr is saved, we don't use first or last insn of PICstuff.  */
  if (fsr->regs[SRP_REGNUM]) {
    pcode++;
    size-=2;
  }

  while (size-- && next_ip && (pcode->insn == (pcode->mask & insn)))
    {
      pcode++;
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn);
    }

  /* Accept moves of parameter registers to other registers, using
     "or rd,rs,0" or "or.u rd,rs,0" or "or rd,r0,rs" or "or rd,rs,r0".
     We don't have to worry about walking into the first lines of code,
     since the first line number will stop us (assuming we have symbols).
     What gcc actually seems to produce is "or rd,r0,rs".  */

#define	OR_MOVE_INSN	0x58000000		/* or/or.u with immed of 0 */
#define	OR_MOVE_MASK	0xF800FFFF
#define	OR_REG_MOVE1_INSN	0xF4005800	/* or rd,r0,rs */
#define	OR_REG_MOVE1_MASK	0xFC1FFFE0
#define	OR_REG_MOVE2_INSN	0xF4005800	/* or rd,rs,r0 */
#define	OR_REG_MOVE2_MASK	0xFC00FFFF
  while (next_ip && 
	 ((insn & OR_MOVE_MASK) == OR_MOVE_INSN ||
	  (insn & OR_REG_MOVE1_MASK) == OR_REG_MOVE1_INSN ||
	  (insn & OR_REG_MOVE2_MASK) == OR_REG_MOVE2_INSN
	 )
	)
    {
      /* We don't care what moves to where.  The result of the moves 
 	 has already been reflected in what the compiler tells us is the
	 location of these parameters.  */
      ip = next_ip;
      next_ip = NEXT_PROLOGUE_INSN (ip, limit, &insn);
    }

  /* We're done with the prologue.  If we don't care about the stack
     frame itself, just return.  (Note that fsr->regs has been trashed,
     but the one caller who calls with fi==0 passes a dummy there.)  */

  if (fi == 0)
    return ip;

  /*
     OK, now we have:

     	sp_offset	original (before any alloca calls) displacement of SP
			(will be negative).

	fp_offset	displacement from original SP to the FP for this frame
			or -1.

	fsr->regs[0..31]	displacement from original SP to the stack
				location where reg[0..31] is stored.

	must_adjust[0..31]	set if corresponding offset was set.

     If alloca has been called between the function prologue and the current
     IP, then the current SP (frame_sp) will not be the original SP as set by
     the function prologue.  If the current SP is not the original SP, then the
     compiler will have allocated an FP for this frame, fp_offset will be set,
     and we can use it to calculate the original SP.

     Then, we figure out where the arguments and locals are, and relocate the
     offsets in fsr->regs to absolute addresses.  */

  if (fp_offset != -1) {
    /* We have a frame pointer, so get it, and base our calc's on it.  */
    frame_fp = (CORE_ADDR) read_next_frame_reg (fi->next, ACTUAL_FP_REGNUM);
    frame_sp = frame_fp - fp_offset;
  } else {
    /* We have no frame pointer, therefore frame_sp is still the same value
       as set by prologue.  But where is the frame itself?  */
    if (must_adjust[SRP_REGNUM]) {
      /* Function header saved SRP (r1), the return address.  Frame starts
	 4 bytes down from where it was saved.  */
      frame_fp = frame_sp + fsr->regs[SRP_REGNUM] - 4;
      fi->locals_pointer = frame_fp;
    } else {
      /* Function header didn't save SRP (r1), so we are in a leaf fn or
	 are otherwise confused.  */
      frame_fp = -1;
    }
  }

  /* The locals are relative to the FP (whether it exists as an allocated
     register, or just as an assumed offset from the SP) */
  fi->locals_pointer = frame_fp;

  /* The arguments are just above the SP as it was before we adjusted it
     on entry.  */
  fi->args_pointer = frame_sp - sp_offset;

  /* Now that we know the SP value used by the prologue, we know where
     it saved all the registers.  */
  for (src = 0; src < 32; src++)
    if (must_adjust[src])
      fsr->regs[src] += frame_sp;
 
  /* The saved value of the SP is always known.  */
  /* (we hope...) */
  if (fsr->regs[SP_REGNUM] != 0 
   && fsr->regs[SP_REGNUM] != frame_sp - sp_offset)
    fprintf(stderr, "Bad saved SP value %x != %x, offset %x!\n",
        fsr->regs[SP_REGNUM],
	frame_sp - sp_offset, sp_offset);

  fsr->regs[SP_REGNUM] = frame_sp - sp_offset;

  return (ip);
}

/* Given an ip value corresponding to the start of a function,
   return the ip of the first instruction after the function 
   prologue.  */

CORE_ADDR
skip_prologue (ip)
     CORE_ADDR (ip);
{
  struct frame_saved_regs saved_regs_dummy;
  struct symtab_and_line sal;
  CORE_ADDR limit;

  sal = find_pc_line (ip, 0);
  limit = (sal.end) ? sal.end : 0xffffffff;

  return (examine_prologue (ip, limit, (FRAME_ADDR) 0, &saved_regs_dummy,
			    (struct frame_info *)0 ));
}

/* Put here the code to store, into a struct frame_saved_regs,
   the addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special:
   the address we return for it IS the sp for the next frame.

   We cache the result of doing this in the frame_cache_obstack, since
   it is fairly expensive.  */

void
frame_find_saved_regs (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{
  register struct frame_saved_regs *cache_fsr;
  extern struct obstack frame_cache_obstack;
  CORE_ADDR ip;
  struct symtab_and_line sal;
  CORE_ADDR limit;

  if (!fi->fsr)
    {
      cache_fsr = (struct frame_saved_regs *)
		  obstack_alloc (&frame_cache_obstack,
				 sizeof (struct frame_saved_regs));
      memset (cache_fsr, '\0', sizeof (struct frame_saved_regs));
      fi->fsr = cache_fsr;

      /* Find the start and end of the function prologue.  If the PC
	 is in the function prologue, we only consider the part that
	 has executed already.  */
         
      ip = get_pc_function_start (fi->pc);
      sal = find_pc_line (ip, 0);
      limit = (sal.end && sal.end < fi->pc) ? sal.end: fi->pc;

      /* This will fill in fields in *fi as well as in cache_fsr.  */
      examine_prologue (ip, limit, fi->frame, cache_fsr, fi);
    }

  if (fsr)
    *fsr = *fi->fsr;
}

/* Return the address of the locals block for the frame
   described by FI.  Returns 0 if the address is unknown.
   NOTE!  Frame locals are referred to by negative offsets from the
   argument pointer, so this is the same as frame_args_address().  */

CORE_ADDR
frame_locals_address (fi)
     struct frame_info *fi;
{
  struct frame_saved_regs fsr;

  if (fi->args_pointer)	/* Cached value is likely there.  */
    return fi->args_pointer;

  /* Nope, generate it.  */

  get_frame_saved_regs (fi, &fsr);

  return fi->args_pointer;
}

/* Return the address of the argument block for the frame
   described by FI.  Returns 0 if the address is unknown.  */

CORE_ADDR
frame_args_address (fi)
     struct frame_info *fi;
{
  struct frame_saved_regs fsr;

  if (fi->args_pointer)		/* Cached value is likely there.  */
    return fi->args_pointer;

  /* Nope, generate it.  */

  get_frame_saved_regs (fi, &fsr);

  return fi->args_pointer;
}

/* Return the saved PC from this frame.

   If the frame has a memory copy of SRP_REGNUM, use that.  If not,
   just use the register SRP_REGNUM itself.  */

CORE_ADDR
frame_saved_pc (frame)
     FRAME frame;
{
  return read_next_frame_reg(frame, SRP_REGNUM);
}

#if 0
/* I believe this is all obsolete call dummy stuff.  */
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
      case TYPE_CODE_ARRAY:		/* Array type, lower & upper bounds */

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
      case TYPE_CODE_STRING:		/* String type */
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
  write_memory (address, (char *)&val, 4);
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
      case TYPE_CODE_ARRAY:		/* Array type, lower & upper bounds */
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

#define	FIRST_PARM_REGNUM	2

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
	   break;
         }
       default: break;
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
collect_returned_value (rval, value_type, struct_return, nargs, args)
     value *rval;
     struct type *value_type;
     int struct_return;
     int nargs;
     value *args;
{
  char retbuf[REGISTER_BYTES];

  memcpy (retbuf, registers, REGISTER_BYTES);
  *rval = value_being_returned (value_type, retbuf, struct_return);
  return;
}
#endif /* 0 */

/*start of lines added by kev*/

#define DUMMY_FRAME_SIZE 192

static void
write_word (sp, word)
     CORE_ADDR sp;
     REGISTER_TYPE word;
{
  register int len = sizeof (REGISTER_TYPE);
  char buffer[MAX_REGISTER_RAW_SIZE];

  store_unsigned_integer (buffer, len, word);
  write_memory (sp, buffer, len);
}

void
m88k_push_dummy_frame()
{
  register CORE_ADDR sp = read_register (SP_REGNUM);
  register int rn;
  int offset;

  sp -= DUMMY_FRAME_SIZE;	/* allocate a bunch of space */

  for (rn = 0, offset = 0; rn <= SP_REGNUM; rn++, offset+=4)
    write_word (sp+offset, read_register(rn));
  
  write_word (sp+offset, read_register (SXIP_REGNUM));
  offset += 4;

  write_word (sp+offset, read_register (SNIP_REGNUM));
  offset += 4;

  write_word (sp+offset, read_register (SFIP_REGNUM));
  offset += 4;

  write_word (sp+offset, read_register (PSR_REGNUM));
  offset += 4;

  write_word (sp+offset, read_register (FPSR_REGNUM));
  offset += 4;

  write_word (sp+offset, read_register (FPCR_REGNUM));
  offset += 4;

  write_register (SP_REGNUM, sp);
  write_register (ACTUAL_FP_REGNUM, sp);
}

void
pop_frame ()
{
  register FRAME frame = get_current_frame ();
  register CORE_ADDR fp;
  register int regnum;
  struct frame_saved_regs fsr;
  struct frame_info *fi;

  fi = get_frame_info (frame);
  fp = fi -> frame;
  get_frame_saved_regs (fi, &fsr);

  if (PC_IN_CALL_DUMMY (read_pc(), read_register(SP_REGNUM), FRAME_FP(fi)))
    {
      /* FIXME: I think get_frame_saved_regs should be handling this so
	 that we can deal with the saved registers properly (e.g. frame
	 1 is a call dummy, the user types "frame 2" and then "print $ps").  */
      register CORE_ADDR sp = read_register (ACTUAL_FP_REGNUM);
      int offset;

      for (regnum = 0, offset = 0; regnum <= SP_REGNUM; regnum++, offset+=4)
	(void) write_register (regnum, read_memory_integer (sp+offset, 4));
  
      write_register (SXIP_REGNUM, read_memory_integer (sp+offset, 4));
      offset += 4;

      write_register (SNIP_REGNUM, read_memory_integer (sp+offset, 4));
      offset += 4;

      write_register (SFIP_REGNUM, read_memory_integer (sp+offset, 4));
      offset += 4;

      write_register (PSR_REGNUM, read_memory_integer (sp+offset, 4));
      offset += 4;

      write_register (FPSR_REGNUM, read_memory_integer (sp+offset, 4));
      offset += 4;

      write_register (FPCR_REGNUM, read_memory_integer (sp+offset, 4));
      offset += 4;

    }
  else 
    {
      for (regnum = FP_REGNUM ; regnum > 0 ; regnum--)
	  if (fsr.regs[regnum])
	      write_register (regnum,
			      read_memory_integer (fsr.regs[regnum], 4));
      write_pc(frame_saved_pc(frame));
    }
  reinit_frame_cache ();
}
