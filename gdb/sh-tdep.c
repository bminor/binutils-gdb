/* Target-dependent code for Hitachi Super-H, for GDB.
   Copyright (C) 1993, 1994, 1995, 1996 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
 Contributed by Steve Chamberlain
                sac@cygnus.com
 */

#include "defs.h"
#include "frame.h"
#include "obstack.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "value.h"
#include "dis-asm.h"
#include "inferior.h"		/* for BEFORE_TEXT_END etc. */

extern int remote_write_size;	/* in remote.c */

/* Default to the original SH.  */

#define DEFAULT_SH_TYPE "sh"

/* This value is the model of SH in use.  */

char *sh_processor_type;

char *tmp_sh_processor_type;

/* A set of original names, to be used when restoring back to generic
   registers from a specific set.  */

char *sh_generic_reg_names[] = REGISTER_NAMES;

char *sh_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
};

char *sh3_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "",     "",     "",     "",     "",     "",     "",     "",
  "ssr",  "spc",
  "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
  "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1"
};

char *sh3e_reg_names[] = {
  "r0",   "r1",   "r2",   "r3",   "r4",   "r5",   "r6",   "r7",
  "r8",   "r9",   "r10",  "r11",  "r12",  "r13",  "r14",  "r15",
  "pc",   "pr",   "gbr",  "vbr",  "mach", "macl", "sr",
  "fpul", "fpscr",
  "fr0",  "fr1",  "fr2",  "fr3",  "fr4",  "fr5",  "fr6",  "fr7",
  "fr8",  "fr9",  "fr10", "fr11", "fr12", "fr13", "fr14", "fr15",
  "ssr",  "spc",
  "r0b0", "r1b0", "r2b0", "r3b0", "r4b0", "r5b0", "r6b0", "r7b0",
  "r0b1", "r1b1", "r2b1", "r3b1", "r4b1", "r5b1", "r6b1", "r7b1",
};

struct {
  char *name;
  char **regnames;
} sh_processor_type_table[] = {
  { "sh", sh_reg_names },
  { "sh3", sh3_reg_names },
  { "sh3e", sh3e_reg_names },
  { NULL, NULL }
};

/* Prologue looks like
   [mov.l	<regs>,@-r15]...
   [sts.l	pr,@-r15]
   [mov.l	r14,@-r15]
   [mov		r15,r14]
*/

#define IS_STS(x)  		((x) == 0x4f22)
#define IS_PUSH(x) 		(((x) & 0xff0f) == 0x2f06)
#define GET_PUSHED_REG(x)  	(((x) >> 4) & 0xf)
#define IS_MOV_SP_FP(x)  	((x) == 0x6ef3)
#define IS_ADD_SP(x) 		(((x) & 0xff00) == 0x7f00)
#define IS_MOV_R3(x) 		(((x) & 0xff00) == 0x1a00)
#define IS_SHLL_R3(x)		((x) == 0x4300)
#define IS_ADD_R3SP(x)		((x) == 0x3f3c)

/* Skip any prologue before the guts of a function */

CORE_ADDR
sh_skip_prologue (start_pc)
     CORE_ADDR start_pc;
{
  int w;

  w = read_memory_integer (start_pc, 2);
  while (IS_STS (w)
	 || IS_PUSH (w)
	 || IS_MOV_SP_FP (w)
	 || IS_MOV_R3 (w)
	 || IS_ADD_R3SP (w)
	 || IS_ADD_SP (w)
	 || IS_SHLL_R3 (w))
    {
      start_pc += 2;
      w = read_memory_integer (start_pc, 2);
    }

  return start_pc;
}

/* Disassemble an instruction.  */

int
gdb_print_insn_sh (memaddr, info)
     bfd_vma memaddr;
     disassemble_info *info;
{
  if (TARGET_BYTE_ORDER == BIG_ENDIAN)
    return print_insn_sh (memaddr, info);
  else
    return print_insn_shl (memaddr, info);
}

/* Given a GDB frame, determine the address of the calling function's frame.
   This will be used to create a new GDB frame struct, and then
   INIT_EXTRA_FRAME_INFO and INIT_FRAME_PC will be called for the new frame.

   For us, the frame address is its stack pointer value, so we look up
   the function prologue to determine the caller's sp value, and return it.  */

CORE_ADDR
sh_frame_chain (frame)
     struct frame_info *frame;
{
  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    return frame->frame;	/* dummy frame same as caller's frame */
  if (!inside_entry_file (frame->pc))
    return read_memory_integer (FRAME_FP (frame) + frame->f_offset, 4);
  else
    return 0;
}

/* Find REGNUM on the stack.  Otherwise, it's in an active register.  One thing
   we might want to do here is to check REGNUM against the clobber mask, and
   somehow flag it as invalid if it isn't saved on the stack somewhere.  This
   would provide a graceful failure mode when trying to get the value of
   caller-saves registers for an inner frame.  */

CORE_ADDR
sh_find_callers_reg (fi, regnum)
     struct frame_info *fi;
     int regnum;
{
  struct frame_saved_regs fsr;

  for (; fi; fi = fi->next)
    if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
      /* When the caller requests PR from the dummy frame, we return PC because
	 that's where the previous routine appears to have done a call from. */
      return generic_read_register_dummy (fi, regnum);
    else 
      {
	FRAME_FIND_SAVED_REGS(fi, fsr);
	if (fsr.regs[regnum] != 0)
	  return read_memory_integer (fsr.regs[regnum], 
				      REGISTER_RAW_SIZE(regnum));
      }
  return read_register (regnum);
}

/* Put here the code to store, into a struct frame_saved_regs, the
   addresses of the saved registers of frame described by FRAME_INFO.
   This includes special registers such as pc and fp saved in special
   ways in the stack frame.  sp is even more special: the address we
   return for it IS the sp for the next frame. */

/* FIXME!  A lot of this should be abstracted out into a sh_scan_prologue 
   function, and the struct frame_info should have a frame_saved_regs
   embedded in it, so we would only have to do this once. */

void
sh_frame_find_saved_regs (fi, fsr)
     struct frame_info *fi;
     struct frame_saved_regs *fsr;
{
  int where[NUM_REGS];
  int rn;
  int have_fp = 0;
  int depth;
  int pc;
  int opc;
  int insn;
  int r3_val = 0;
  char * dummy_regs = generic_find_dummy_frame (fi->pc, fi->frame, fi->frame);

  if (dummy_regs)
    {
      /* DANGER!  This is ONLY going to work if the char buffer format of
	 the saved registers is byte-for-byte identical to the 
	 CORE_ADDR regs[NUM_REGS] format used by struct frame_saved_regs! */
      memcpy (&fsr->regs, dummy_regs, sizeof(fsr));
      return;
    }

  opc = pc = get_pc_function_start (fi->pc);

  insn = read_memory_integer (pc, 2);

  fi->leaf_function = 1;
  fi->f_offset = 0;

  for (rn = 0; rn < NUM_REGS; rn++)
    where[rn] = -1;

  depth = 0;

  /* Loop around examining the prologue insns, but give up
     after 15 of them, since we're getting silly then */
  while (pc < opc + 15 * 2)
    {
      /* See where the registers will be saved to */
      if (IS_PUSH (insn))
	{
	  pc += 2;
	  rn = GET_PUSHED_REG (insn);
	  where[rn] = depth;
	  insn = read_memory_integer (pc, 2);
	  depth += 4;
	}
      else if (IS_STS (insn))
	{
	  pc += 2;
	  where[PR_REGNUM] = depth;
	  insn = read_memory_integer (pc, 2);
	  /* If we're storing the pr then this isn't a leaf */
	  fi->leaf_function = 0;
	  depth += 4;
	}
      else if (IS_MOV_R3 (insn))
	{
	  r3_val = (char) (insn & 0xff);
	  pc += 2;
	  insn = read_memory_integer (pc, 2);
	}
      else if (IS_SHLL_R3 (insn))
	{
	  r3_val <<= 1;
	  pc += 2;
	  insn = read_memory_integer (pc, 2);
	}
      else if (IS_ADD_R3SP (insn))
	{
	  depth += -r3_val;
	  pc += 2;
	  insn = read_memory_integer (pc, 2);
	}
      else if (IS_ADD_SP (insn))
	{
	  pc += 2;
	  depth += -((char) (insn & 0xff));
	  insn = read_memory_integer (pc, 2);
	}
      else
	break;
    }

  /* Now we know how deep things are, we can work out their addresses */

  for (rn = 0; rn < NUM_REGS; rn++)
    {
      if (where[rn] >= 0)
	{
	  if (rn == FP_REGNUM)
	    have_fp = 1;

	  fsr->regs[rn] = fi->frame - where[rn] + depth - 4;
	}
      else
	{
	  fsr->regs[rn] = 0;
	}
    }

  if (have_fp)
    {
      fsr->regs[SP_REGNUM] = read_memory_integer (fsr->regs[FP_REGNUM], 4);
    }
  else
    {
      fsr->regs[SP_REGNUM] = fi->frame - 4;
    }

  fi->f_offset = depth - where[FP_REGNUM] - 4;
  /* Work out the return pc - either from the saved pr or the pr
     value */
}

/* initialize the extra info saved in a FRAME */

void
sh_init_extra_frame_info (fromleaf, fi)
     int fromleaf;
     struct frame_info *fi;
{
  struct frame_saved_regs fsr;

  if (fi->next)
    fi->pc = FRAME_SAVED_PC (fi->next);

  if (PC_IN_CALL_DUMMY (fi->pc, fi->frame, fi->frame))
    {
      /* We need to setup fi->frame here because run_stack_dummy gets it wrong
	 by assuming it's always FP.  */
      fi->frame     = generic_read_register_dummy (fi, SP_REGNUM);
      fi->return_pc = generic_read_register_dummy (fi, PC_REGNUM);
      fi->f_offset = -(CALL_DUMMY_LENGTH + 4);
      fi->leaf_function = 0;
      return;
    }
  else
    {
      FRAME_FIND_SAVED_REGS (fi, fsr);
      fi->return_pc = sh_find_callers_reg (fi, PR_REGNUM);
    }
}

/* Discard from the stack the innermost frame,
   restoring all saved registers.  */

void
sh_pop_frame ()
{
  register struct frame_info *frame = get_current_frame ();
  register CORE_ADDR fp;
  register int regnum;
  struct frame_saved_regs fsr;

  if (PC_IN_CALL_DUMMY (frame->pc, frame->frame, frame->frame))
    generic_pop_dummy_frame ();
  else
  {
    fp = FRAME_FP (frame);
    get_frame_saved_regs (frame, &fsr);

    /* Copy regs from where they were saved in the frame */
    for (regnum = 0; regnum < NUM_REGS; regnum++)
      if (fsr.regs[regnum])
	write_register (regnum, read_memory_integer (fsr.regs[regnum], 4));

    write_register (PC_REGNUM, frame->return_pc);
    write_register (SP_REGNUM, fp + 4);
  }
  flush_cached_frames ();
}

/* Function: push_arguments
   Setup the function arguments for calling a function in the inferior.

   On the Hitachi SH architecture, there are four registers (R4 to R7)
   which are dedicated for passing function arguments.  Up to the first
   four arguments (depending on size) may go into these registers.
   The rest go on the stack.

   Arguments that are smaller than 4 bytes will still take up a whole
   register or a whole 32-bit word on the stack, and will be 
   right-justified in the register or the stack word.  This includes
   chars, shorts, and small aggregate types.

   Arguments that are larger than 4 bytes may be split between two or 
   more registers.  If there are not enough registers free, an argument
   may be passed partly in a register (or registers), and partly on the
   stack.  This includes doubles, long longs, and larger aggregates. 
   As far as I know, there is no upper limit to the size of aggregates 
   that will be passed in this way; in other words, the convention of 
   passing a pointer to a large aggregate instead of a copy is not used.

   An exceptional case exists for struct arguments (and possibly other
   aggregates such as arrays) if the size is larger than 4 bytes but 
   not a multiple of 4 bytes.  In this case the argument is never split 
   between the registers and the stack, but instead is copied in its
   entirety onto the stack, AND also copied into as many registers as 
   there is room for.  In other words, space in registers permitting, 
   two copies of the same argument are passed in.  As far as I can tell,
   only the one on the stack is used, although that may be a function 
   of the level of compiler optimization.  I suspect this is a compiler
   bug.  Arguments of these odd sizes are left-justified within the 
   word (as opposed to arguments smaller than 4 bytes, which are 
   right-justified).
 

   If the function is to return an aggregate type such as a struct, it 
   is either returned in the normal return value register R0 (if its 
   size is no greater than one byte), or else the caller must allocate
   space into which the callee will copy the return value (if the size
   is greater than one byte).  In this case, a pointer to the return 
   value location is passed into the callee in register R2, which does 
   not displace any of the other arguments passed in via registers R4
   to R7.   */

CORE_ADDR
sh_push_arguments (nargs, args, sp, struct_return, struct_addr)
     int nargs;
     value_ptr *args;
     CORE_ADDR sp;
     unsigned char struct_return;
     CORE_ADDR struct_addr;
{
  int argreg;
  int argnum;
  CORE_ADDR regval;
  char *val;
  char valbuf[4];
  int len;
  int push[4];		/* some of the first 4 args may not need to be pushed
			   onto the stack, because they can go in registers */

  /* first force sp to a 4-byte alignment */
  sp = sp & ~3;

  /* The "struct return pointer" pseudo-argument has its own dedicated 
     register */
  if (struct_return)
      write_register (STRUCT_RETURN_REGNUM, struct_addr);

  /* Now load as many as possible of the first arguments into registers.
     There are 16 bytes in four registers available. 
     Loop thru args from first to last.  */
  push[0] = push[1] = push[2] = push[3] = 0;
  for (argnum = 0, argreg = ARG0_REGNUM; 
       argnum < nargs && argreg <= ARGLAST_REGNUM; 
       argnum++)
    {
      struct type *type = VALUE_TYPE (args[argnum]);
      
      len = TYPE_LENGTH (type);

      switch (TYPE_CODE(type)) {
      case TYPE_CODE_STRUCT:
      case TYPE_CODE_UNION:
      /* case TYPE_CODE_ARRAY:  case TYPE_CODE_STRING: */
	if (len <= 4   ||   (len & ~3) == 0)
	  push[argnum] = 0;		/* doesn't get pushed onto stack */
	else
	  push[argnum] = len;		/* does    get pushed onto stack */
	break;
      default:
	push[argnum] = 0;		/* doesn't get pushed onto stack */
      }
      if (len < 4)
	{ /* value gets right-justified in the register */
	  memcpy(valbuf + (4 - len), 
		 (char *) VALUE_CONTENTS (args[argnum]), len);
	  val = valbuf;
	}
      else
	val = (char *) VALUE_CONTENTS (args[argnum]);

      while (len > 0)
	{
	  regval = extract_address (val, REGISTER_RAW_SIZE (argreg));
	  write_register (argreg, regval);

	  len -= REGISTER_RAW_SIZE (argreg);
	  val += REGISTER_RAW_SIZE (argreg);
	  argreg++;
	  if (argreg > ARGLAST_REGNUM)
	    {
	      push[argnum] = len;	/* ran out of arg passing registers! */
	      break;			/* len bytes remain to go onto stack */
	    }
	}
    }

  /* Now push as many as necessary of the remaining arguments onto the stack.
     For args 0 to 3, the arg may have been passed in a register. 
     Loop thru args from last to first.  */
  for (argnum = nargs-1; argnum >= 0; --argnum)
    {
      if (argnum < 4 && push[argnum] == 0)
	continue;	/* no need to push this arg */

      len = TYPE_LENGTH (VALUE_TYPE (args[argnum]));
      if (len < 4)
	{
	  memcpy(valbuf + (4 - len), 
		 (char *) VALUE_CONTENTS (args[argnum]), len);
	  val = valbuf;
	}
      else
	val = (char *) VALUE_CONTENTS (args[argnum]);

      if (argnum < 4)
	if (len > push[argnum])		/* some part may already be in a reg */
	  {
	    val += (len - push[argnum]);
	    len = push[argnum];
	  }

      sp -= (len + 3) & ~3;
      write_memory (sp, val, len);
    }
  return sp;
}

/* Function: push_return_address (pc)
   Set up the return address for the inferior function call.
   Necessary for targets where we don't actually execute a JSR/BSR instruction */

void
sh_push_return_address (pc)
     CORE_ADDR pc;
{
  write_register (PR_REGNUM, entry_point_address ());
}

/* Command to set the processor type.  */

void
sh_set_processor_type_command (args, from_tty)
     char *args;
     int from_tty;
{
  int i;
  char *temp;

  /* The `set' commands work by setting the value, then calling the hook,
     so we let the general command modify a scratch location, then decide
     here if we really want to modify the processor type.  */
  if (tmp_sh_processor_type == NULL || *tmp_sh_processor_type == '\0')
    {
      printf_unfiltered ("The known SH processor types are as follows:\n\n");
      for (i = 0; sh_processor_type_table[i].name != NULL; ++i)
	printf_unfiltered ("%s\n", sh_processor_type_table[i].name);

      /* Restore the value.  */
      tmp_sh_processor_type = strsave (sh_processor_type);

      return;
    }
  
  if (!sh_set_processor_type (tmp_sh_processor_type))
    {
      /* Restore to a valid value before erroring out.  */
      temp = tmp_sh_processor_type;
      tmp_sh_processor_type = strsave (sh_processor_type);
      error ("Unknown processor type `%s'.", temp);
    }
}

/* This is a dummy not actually run.  */

static void
sh_show_processor_type_command (args, from_tty)
     char *args;
     int from_tty;
{
}

/* Modify the actual processor type. */

int
sh_set_processor_type (str)
     char *str;
{
  int i, j;

  if (str == NULL)
    return 0;

  for (i = 0; sh_processor_type_table[i].name != NULL; ++i)
    {
      if (strcasecmp (str, sh_processor_type_table[i].name) == 0)
	{
	  sh_processor_type = str;

	  for (j = 0; j < NUM_REGS; ++j)
	    reg_names[j] = sh_processor_type_table[i].regnames[j];

	  return 1;
	}
    }

  return 0;
}

/* Print the registers in a form similar to the E7000 */

static void
sh_show_regs (args, from_tty)
     char *args;
     int from_tty;
{
  printf_filtered ("PC=%08x SR=%08x PR=%08x MACH=%08x MACHL=%08x\n",
		   read_register (PC_REGNUM),
		   read_register (SR_REGNUM),
		   read_register (PR_REGNUM),
		   read_register (MACH_REGNUM),
		   read_register (MACL_REGNUM));

  printf_filtered ("R0-R7  %08x %08x %08x %08x %08x %08x %08x %08x\n",
		   read_register (0),
		   read_register (1),
		   read_register (2),
		   read_register (3),
		   read_register (4),
		   read_register (5),
		   read_register (6),
		   read_register (7));
  printf_filtered ("R8-R15 %08x %08x %08x %08x %08x %08x %08x %08x\n",
		   read_register (8),
		   read_register (9),
		   read_register (10),
		   read_register (11),
		   read_register (12),
		   read_register (13),
		   read_register (14),
		   read_register (15));
}

void
sh_extract_return_value (type, regbuf, valbuf)
     struct type *type;
     void *regbuf;
     void *valbuf;
{
  int len = TYPE_LENGTH(type);

  if (len <= 4)
    memcpy (valbuf, ((char *) regbuf) + 4 - len, len);
  else if (len <= 8)
    memcpy (valbuf, ((char *) regbuf) + 8 - len, len);
  else
    error ("bad size for return value");
}

void
_initialize_sh_tdep ()
{
  struct cmd_list_element *c;

  tm_print_insn = gdb_print_insn_sh;

  c = add_set_cmd ("processor", class_support, var_string_noescape,
		   (char *) &tmp_sh_processor_type,
		   "Set the type of SH processor in use.\n\
Set this to be able to access processor-type-specific registers.\n\
",
		   &setlist);
  c->function.cfunc = sh_set_processor_type_command;
  c = add_show_from_set (c, &showlist);
  c->function.cfunc = sh_show_processor_type_command;

  tmp_sh_processor_type = strsave (DEFAULT_SH_TYPE);
  sh_set_processor_type_command (strsave (DEFAULT_SH_TYPE), 0);

  add_com ("regs", class_vars, sh_show_regs, "Print all registers");

  /* Reduce the remote write size because some CMONs can't take
    more than 400 bytes in a packet.  300 seems like a safe bet.  */
  remote_write_size = 300;
}

/*
 * DUMMY FRAMES
 * 
 * The following code serves to maintain the dummy stack frames for
 * inferior function calls (ie. when gdb calls into the inferior via
 * call_function_by_hand).  This code saves the machine state before 
 * the call in host memory, so it must maintain an independant stack 
 * and keep it consistant etc.  I am attempting to make this code 
 * generic enough to be used by many targets.
 *
 * The cheapest and most generic way to do CALL_DUMMY on a new target
 * is probably to define CALL_DUMMY to be empty, CALL_DUMMY_LENGTH to zero,
 * and CALL_DUMMY_LOCATION to AT_ENTRY.  Then you must remember to define
 * PUSH_RETURN_ADDRESS, because there won't be a call instruction to do it.
 */

/* Dummy frame.  This saves the processor state just prior to setting up the
   inferior function call.  On most targets, the registers are saved on the
   target stack, but that really slows down function calls.  */

struct dummy_frame
{
  struct dummy_frame *next;

  CORE_ADDR pc;
  CORE_ADDR fp;
  CORE_ADDR sp;
  char regs[REGISTER_BYTES];
};

static struct dummy_frame *dummy_frame_stack = NULL;

/* Function: find_dummy_frame(pc, fp, sp)
   Search the stack of dummy frames for one matching the given PC, FP and SP.
   This is the work-horse for pc_in_call_dummy and read_register_dummy     */

char * 
generic_find_dummy_frame (pc, fp, sp)
     CORE_ADDR pc;
     CORE_ADDR fp;
     CORE_ADDR sp;
{
  struct dummy_frame * dummyframe;
  CORE_ADDR bkpt_address;
  extern CORE_ADDR text_end;

#if CALL_DUMMY_LOCATION == AT_ENTRY_POINT
  bkpt_address = entry_point_address () + CALL_DUMMY_BREAKPOINT_OFFSET;
  if (pc != bkpt_address &&
      pc != bkpt_address + DECR_PC_AFTER_BREAK)
    return 0;
#endif	/* AT_ENTRY_POINT */

#if CALL_DUMMY_LOCATION == BEFORE_TEXT_END
  bkpt_address = text_end - CALL_DUMMY_LENGTH + CALL_DUMMY_BREAKPOINT_OFFSET;
  if (pc != bkpt_address &&
      pc != bkpt_address + DECR_PC_AFTER_BREAK)
    return 0;
#endif	/* BEFORE_TEXT_END */

#if CALL_DUMMY_LOCATION == AFTER_TEXT_END
  bkpt_address = text_end + CALL_DUMMY_BREAKPOINT_OFFSET;
  if (pc != bkpt_address &&
      pc != bkpt_address + DECR_PC_AFTER_BREAK)
    return 0;
#endif	/* AFTER_TEXT_END */

  for (dummyframe = dummy_frame_stack;
       dummyframe;
       dummyframe = dummyframe->next)
    if (fp == dummyframe->fp || 
	sp == dummyframe->sp)
      {
#if CALL_DUMMY_LOCATION == ON_STACK
	CORE_ADDR bkpt_offset;	/* distance from original frame ptr to bkpt */

	if (1 INNER_THAN 2)
	  bkpt_offset = CALL_DUMMY_BREAK_OFFSET;
	else
	  bkpt_offset = CALL_DUMMY_LENGTH - CALL_DUMMY_BREAK_OFFSET;

	if (pc + bkpt_offset == dummyframe->fp ||
	    pc + bkpt_offset == dummyframe->sp ||
	    pc + bkpt_offset + DECR_PC_AFTER_BREAK == dummyframe->fp ||
	    pc + bkpt_offset + DECR_PC_AFTER_BREAK == dummyframe->sp)
#endif /* ON_STACK */
	  return dummyframe->regs;
      }
  return 0;
}

/* Function: pc_in_call_dummy (pc, fp, sp)
   Return true if this is a dummy frame created by gdb for an inferior call */

int
generic_pc_in_call_dummy (pc, fp, sp)
     CORE_ADDR pc;
     CORE_ADDR fp;
     CORE_ADDR sp;
{
  /* if find_dummy_frame succeeds, then PC is in a call dummy */
  return (generic_find_dummy_frame (pc, fp, sp) != 0);
}

/* Function: read_register_dummy (pc, fp, sp, regno)
   Find a saved register from before GDB calls a function in the inferior */

CORE_ADDR
generic_read_register_dummy (fi, regno)
     struct frame_info *fi;
     int regno;
{
  char *dummy_regs = generic_find_dummy_frame (fi->pc, fi->frame, NULL);

  if (dummy_regs)
    return extract_address (&dummy_regs[REGISTER_BYTE (regno)],
			    REGISTER_RAW_SIZE(regno));
  else
    return 0;
}

/* Save all the registers on the dummy frame stack.  Most ports save the
   registers on the target stack.  This results in lots of unnecessary memory
   references, which are slow when debugging via a serial line.  Instead, we
   save all the registers internally, and never write them to the stack.  The
   registers get restored when the called function returns to the entry point,
   where a breakpoint is laying in wait.  */

void
generic_push_dummy_frame ()
{
  struct dummy_frame *dummy_frame;
  CORE_ADDR fp = read_register(FP_REGNUM);

  /* check to see if there are stale dummy frames, 
     perhaps left over from when a longjump took us out of a 
     function that was called by the debugger */

  dummy_frame = dummy_frame_stack;
  while (dummy_frame)
    if (dummy_frame->fp INNER_THAN fp)	/* stale -- destroy! */
      {
	dummy_frame_stack = dummy_frame->next;
	free (dummy_frame);
	dummy_frame = dummy_frame_stack;
      }
    else
      dummy_frame = dummy_frame->next;

  dummy_frame = xmalloc (sizeof (struct dummy_frame));

  read_register_bytes (0, dummy_frame->regs, REGISTER_BYTES);
  dummy_frame->pc   = read_register (PC_REGNUM);
  dummy_frame->fp   = read_register (FP_REGNUM);
  dummy_frame->sp   = read_register (SP_REGNUM);
  dummy_frame->next = dummy_frame_stack;
  dummy_frame_stack = dummy_frame;
}

/* Function: pop_dummy_frame
   Restore the machine state from a saved dummy stack frame. */

void
generic_pop_dummy_frame ()
{
  struct dummy_frame *dummy_frame = dummy_frame_stack;

  if (!dummy_frame)
    error ("Can't pop dummy frame!");
  dummy_frame_stack = dummy_frame->next;
  write_register_bytes (0, dummy_frame->regs, REGISTER_BYTES);
  free (dummy_frame);
}

/* Function: frame_chain_valid 
   Returns true for a user frame or a call_function_by_hand dummy frame,
   and false for the CRT0 start-up frame.  Purpose is to terminate backtrace */

int
generic_frame_chain_valid (fp, fi)
     CORE_ADDR fp;
     struct frame_info *fi;
{
  if (PC_IN_CALL_DUMMY(FRAME_SAVED_PC(fi), fp, fp))
    return 1;	/* don't prune CALL_DUMMY frames */
  else		/* fall back to default algorithm (see frame.h) */
    return (fp != 0 && !inside_entry_file (FRAME_SAVED_PC(fi)));
}

