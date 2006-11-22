/* SPU target-dependent code for GDB, the GNU debugger.
   Copyright (C) 2006 Free Software Foundation, Inc.

   Contributed by Ulrich Weigand <uweigand@de.ibm.com>.
   Based on a port by Sid Manning <sid@us.ibm.com>.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include "defs.h"
#include "arch-utils.h"
#include "gdbtypes.h"
#include "gdbcmd.h"
#include "gdbcore.h"
#include "gdb_string.h"
#include "gdb_assert.h"
#include "frame.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "symtab.h"
#include "symfile.h"
#include "value.h"
#include "inferior.h"
#include "dis-asm.h"
#include "objfiles.h"
#include "language.h"
#include "regcache.h"
#include "reggroups.h"
#include "floatformat.h"

#include "spu-tdep.h"


/* Registers.  */

static const char *
spu_register_name (int reg_nr)
{
  static char *register_names[] = 
    {
      "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
      "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
      "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
      "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
      "r32", "r33", "r34", "r35", "r36", "r37", "r38", "r39",
      "r40", "r41", "r42", "r43", "r44", "r45", "r46", "r47",
      "r48", "r49", "r50", "r51", "r52", "r53", "r54", "r55",
      "r56", "r57", "r58", "r59", "r60", "r61", "r62", "r63",
      "r64", "r65", "r66", "r67", "r68", "r69", "r70", "r71",
      "r72", "r73", "r74", "r75", "r76", "r77", "r78", "r79",
      "r80", "r81", "r82", "r83", "r84", "r85", "r86", "r87",
      "r88", "r89", "r90", "r91", "r92", "r93", "r94", "r95",
      "r96", "r97", "r98", "r99", "r100", "r101", "r102", "r103",
      "r104", "r105", "r106", "r107", "r108", "r109", "r110", "r111",
      "r112", "r113", "r114", "r115", "r116", "r117", "r118", "r119",
      "r120", "r121", "r122", "r123", "r124", "r125", "r126", "r127",
      "id", "pc", "sp"
    };

  if (reg_nr < 0)
    return NULL;
  if (reg_nr >= sizeof register_names / sizeof *register_names)
    return NULL;

  return register_names[reg_nr];
}

static struct type *
spu_register_type (struct gdbarch *gdbarch, int reg_nr)
{
  if (reg_nr < SPU_NUM_GPRS)
    return builtin_type_vec128;

  switch (reg_nr)
    {
    case SPU_ID_REGNUM:
      return builtin_type_uint32;

    case SPU_PC_REGNUM:
      return builtin_type_void_func_ptr;

    case SPU_SP_REGNUM:
      return builtin_type_void_data_ptr;

    default:
      internal_error (__FILE__, __LINE__, "invalid regnum");
    }
}

/* Pseudo registers for preferred slots - stack pointer.  */

static void
spu_pseudo_register_read (struct gdbarch *gdbarch, struct regcache *regcache,
                          int regnum, gdb_byte *buf)
{
  gdb_byte reg[16];

  switch (regnum)
    {
    case SPU_SP_REGNUM:
      regcache_raw_read (regcache, SPU_RAW_SP_REGNUM, reg);
      memcpy (buf, reg, 4);
      break;

    default:
      internal_error (__FILE__, __LINE__, _("invalid regnum"));
    }
}

static void
spu_pseudo_register_write (struct gdbarch *gdbarch, struct regcache *regcache,
                           int regnum, const gdb_byte *buf)
{
  gdb_byte reg[16];

  switch (regnum)
    {
    case SPU_SP_REGNUM:
      regcache_raw_read (regcache, SPU_RAW_SP_REGNUM, reg);
      memcpy (reg, buf, 4);
      regcache_raw_write (regcache, SPU_RAW_SP_REGNUM, reg);
      break;

    default:
      internal_error (__FILE__, __LINE__, _("invalid regnum"));
    }
}

/* Value conversion -- access scalar values at the preferred slot.  */

static int
spu_convert_register_p (int regno, struct type *type)
{
  return regno < SPU_NUM_GPRS && TYPE_LENGTH (type) < 16;
}

static void
spu_register_to_value (struct frame_info *frame, int regnum,
                       struct type *valtype, gdb_byte *out)
{
  gdb_byte in[16];
  int len = TYPE_LENGTH (valtype);
  int preferred_slot = len < 4 ? 4 - len : 0;
  gdb_assert (len < 16);

  get_frame_register (frame, regnum, in);
  memcpy (out, in + preferred_slot, len);
}

static void
spu_value_to_register (struct frame_info *frame, int regnum,
                       struct type *valtype, const gdb_byte *in)
{
  gdb_byte out[16];
  int len = TYPE_LENGTH (valtype);
  int preferred_slot = len < 4 ? 4 - len : 0;
  gdb_assert (len < 16);

  memset (out, 0, 16);
  memcpy (out + preferred_slot, in, len);
  put_frame_register (frame, regnum, out);
}

/* Register groups.  */

static int
spu_register_reggroup_p (struct gdbarch *gdbarch, int regnum,
			 struct reggroup *group)
{
  /* Registers displayed via 'info regs'.  */
  if (group == general_reggroup)
    return 1;

  /* Registers displayed via 'info float'.  */
  if (group == float_reggroup)
    return 0;

  /* Registers that need to be saved/restored in order to
     push or pop frames.  */
  if (group == save_reggroup || group == restore_reggroup)
    return 1;

  return default_register_reggroup_p (gdbarch, regnum, group);
}


/* Decoding SPU instructions.  */

enum
  {
    op_lqd   = 0x34,
    op_lqx   = 0x3c4,
    op_lqa   = 0x61,
    op_lqr   = 0x67,
    op_stqd  = 0x24,
    op_stqx  = 0x144,
    op_stqa  = 0x41,
    op_stqr  = 0x47,

    op_il    = 0x081,
    op_ila   = 0x21,
    op_a     = 0x0c0,
    op_ai    = 0x1c,

    op_selb  = 0x4,

    op_br    = 0x64,
    op_bra   = 0x60,
    op_brsl  = 0x66,
    op_brasl = 0x62,
    op_brnz  = 0x42,
    op_brz   = 0x40,
    op_brhnz = 0x46,
    op_brhz  = 0x44,
    op_bi    = 0x1a8,
    op_bisl  = 0x1a9,
    op_biz   = 0x128,
    op_binz  = 0x129,
    op_bihz  = 0x12a,
    op_bihnz = 0x12b,
  };

static int
is_rr (unsigned int insn, int op, int *rt, int *ra, int *rb)
{
  if ((insn >> 21) == op)
    {
      *rt = insn & 127;
      *ra = (insn >> 7) & 127;
      *rb = (insn >> 14) & 127;
      return 1;
    }

  return 0;
}

static int
is_rrr (unsigned int insn, int op, int *rt, int *ra, int *rb, int *rc)
{
  if ((insn >> 28) == op)
    {
      *rt = (insn >> 21) & 127;
      *ra = (insn >> 7) & 127;
      *rb = (insn >> 14) & 127;
      *rc = insn & 127;
      return 1;
    }

  return 0;
}

static int
is_ri7 (unsigned int insn, int op, int *rt, int *ra, int *i7)
{
  if ((insn >> 21) == op)
    {
      *rt = insn & 127;
      *ra = (insn >> 7) & 127;
      *i7 = (((insn >> 14) & 127) ^ 0x40) - 0x40;
      return 1;
    }

  return 0;
}

static int
is_ri10 (unsigned int insn, int op, int *rt, int *ra, int *i10)
{
  if ((insn >> 24) == op)
    {
      *rt = insn & 127;
      *ra = (insn >> 7) & 127;
      *i10 = (((insn >> 14) & 0x3ff) ^ 0x200) - 0x200;
      return 1;
    }

  return 0;
}

static int
is_ri16 (unsigned int insn, int op, int *rt, int *i16)
{
  if ((insn >> 23) == op)
    {
      *rt = insn & 127;
      *i16 = (((insn >> 7) & 0xffff) ^ 0x8000) - 0x8000;
      return 1;
    }

  return 0;
}

static int
is_ri18 (unsigned int insn, int op, int *rt, int *i18)
{
  if ((insn >> 25) == op)
    {
      *rt = insn & 127;
      *i18 = (((insn >> 7) & 0x3ffff) ^ 0x20000) - 0x20000;
      return 1;
    }

  return 0;
}

static int
is_branch (unsigned int insn, int *offset, int *reg)
{
  int rt, i7, i16;

  if (is_ri16 (insn, op_br, &rt, &i16)
      || is_ri16 (insn, op_brsl, &rt, &i16)
      || is_ri16 (insn, op_brnz, &rt, &i16)
      || is_ri16 (insn, op_brz, &rt, &i16)
      || is_ri16 (insn, op_brhnz, &rt, &i16)
      || is_ri16 (insn, op_brhz, &rt, &i16))
    {
      *reg = SPU_PC_REGNUM;
      *offset = i16 << 2;
      return 1;
    }

  if (is_ri16 (insn, op_bra, &rt, &i16)
      || is_ri16 (insn, op_brasl, &rt, &i16))
    {
      *reg = -1;
      *offset = i16 << 2;
      return 1;
    }

  if (is_ri7 (insn, op_bi, &rt, reg, &i7)
      || is_ri7 (insn, op_bisl, &rt, reg, &i7)
      || is_ri7 (insn, op_biz, &rt, reg, &i7)
      || is_ri7 (insn, op_binz, &rt, reg, &i7)
      || is_ri7 (insn, op_bihz, &rt, reg, &i7)
      || is_ri7 (insn, op_bihnz, &rt, reg, &i7))
    {
      *offset = 0;
      return 1;
    }

  return 0;
}


/* Prolog parsing.  */

struct spu_prologue_data
  {
    /* Stack frame size.  -1 if analysis was unsuccessful.  */
    int size;

    /* How to find the CFA.  The CFA is equal to SP at function entry.  */
    int cfa_reg;
    int cfa_offset;

    /* Offset relative to CFA where a register is saved.  -1 if invalid.  */
    int reg_offset[SPU_NUM_GPRS];
  };

static CORE_ADDR
spu_analyze_prologue (CORE_ADDR start_pc, CORE_ADDR end_pc,
                      struct spu_prologue_data *data)
{
  int found_sp = 0;
  int found_fp = 0;
  int found_lr = 0;
  int reg_immed[SPU_NUM_GPRS];
  gdb_byte buf[16];
  CORE_ADDR prolog_pc = start_pc;
  CORE_ADDR pc;
  int i;


  /* Initialize DATA to default values.  */
  data->size = -1;

  data->cfa_reg = SPU_RAW_SP_REGNUM;
  data->cfa_offset = 0;

  for (i = 0; i < SPU_NUM_GPRS; i++)
    data->reg_offset[i] = -1;

  /* Set up REG_IMMED array.  This is non-zero for a register if we know its
     preferred slot currently holds this immediate value.  */
  for (i = 0; i < SPU_NUM_GPRS; i++)
      reg_immed[i] = 0;

  /* Scan instructions until the first branch.

     The following instructions are important prolog components:

	- The first instruction to set up the stack pointer.
	- The first instruction to set up the frame pointer.
	- The first instruction to save the link register.

     We return the instruction after the latest of these three,
     or the incoming PC if none is found.  The first instruction
     to set up the stack pointer also defines the frame size.

     Note that instructions saving incoming arguments to their stack
     slots are not counted as important, because they are hard to
     identify with certainty.  This should not matter much, because
     arguments are relevant only in code compiled with debug data,
     and in such code the GDB core will advance until the first source
     line anyway, using SAL data.

     For purposes of stack unwinding, we analyze the following types
     of instructions in addition:

      - Any instruction adding to the current frame pointer.
      - Any instruction loading an immediate constant into a register.
      - Any instruction storing a register onto the stack.

     These are used to compute the CFA and REG_OFFSET output.  */

  for (pc = start_pc; pc < end_pc; pc += 4)
    {
      unsigned int insn;
      int rt, ra, rb, rc, immed;

      if (target_read_memory (pc, buf, 4))
	break;
      insn = extract_unsigned_integer (buf, 4);

      /* AI is the typical instruction to set up a stack frame.
         It is also used to initialize the frame pointer.  */
      if (is_ri10 (insn, op_ai, &rt, &ra, &immed))
	{
	  if (rt == data->cfa_reg && ra == data->cfa_reg)
	    data->cfa_offset -= immed;

	  if (rt == SPU_RAW_SP_REGNUM && ra == SPU_RAW_SP_REGNUM
	      && !found_sp)
	    {
	      found_sp = 1;
	      prolog_pc = pc + 4;

	      data->size = -immed;
	    }
	  else if (rt == SPU_FP_REGNUM && ra == SPU_RAW_SP_REGNUM
		   && !found_fp)
	    {
	      found_fp = 1;
	      prolog_pc = pc + 4;

	      data->cfa_reg = SPU_FP_REGNUM;
	      data->cfa_offset -= immed;
	    }
	}

      /* A is used to set up stack frames of size >= 512 bytes.
         If we have tracked the contents of the addend register,
         we can handle this as well.  */
      else if (is_rr (insn, op_a, &rt, &ra, &rb))
	{
	  if (rt == data->cfa_reg && ra == data->cfa_reg)
	    {
	      if (reg_immed[rb] != 0)
		data->cfa_offset -= reg_immed[rb];
	      else
		data->cfa_reg = -1;  /* We don't know the CFA any more.  */
	    }

	  if (rt == SPU_RAW_SP_REGNUM && ra == SPU_RAW_SP_REGNUM
	      && !found_sp)
	    {
	      found_sp = 1;
	      prolog_pc = pc + 4;

	      if (reg_immed[rb] != 0)
		data->size = -reg_immed[rb];
	    }
	}

      /* We need to track IL and ILA used to load immediate constants
         in case they are later used as input to an A instruction.  */
      else if (is_ri16 (insn, op_il, &rt, &immed))
	{
	  reg_immed[rt] = immed;
	}

      else if (is_ri18 (insn, op_ila, &rt, &immed))
	{
	  reg_immed[rt] = immed & 0x3ffff;
	}

      /* STQD is used to save registers to the stack.  */
      else if (is_ri10 (insn, op_stqd, &rt, &ra, &immed))
	{
	  if (ra == data->cfa_reg)
	    data->reg_offset[rt] = data->cfa_offset - (immed << 4);

	  if (ra == data->cfa_reg && rt == SPU_LR_REGNUM
              && !found_lr)
	    {
	      found_lr = 1;
	      prolog_pc = pc + 4;
	    }
	}

      /* _start uses SELB to set up the stack pointer.  */
      else if (is_rrr (insn, op_selb, &rt, &ra, &rb, &rc))
	{
	  if (rt == SPU_RAW_SP_REGNUM && !found_sp)
	    found_sp = 1;
	}

      /* We terminate if we find a branch.  */
      else if (is_branch (insn, &immed, &ra))
	break;
    }


  /* If we successfully parsed until here, and didn't find any instruction
     modifying SP, we assume we have a frameless function.  */
  if (!found_sp)
    data->size = 0;

  /* Return cooked instead of raw SP.  */
  if (data->cfa_reg == SPU_RAW_SP_REGNUM)
    data->cfa_reg = SPU_SP_REGNUM;

  return prolog_pc;
}

/* Return the first instruction after the prologue starting at PC.  */
static CORE_ADDR
spu_skip_prologue (CORE_ADDR pc)
{
  struct spu_prologue_data data;
  return spu_analyze_prologue (pc, (CORE_ADDR)-1, &data);
}

/* Return the frame pointer in use at address PC.  */
static void
spu_virtual_frame_pointer (CORE_ADDR pc, int *reg, LONGEST *offset)
{
  struct spu_prologue_data data;
  spu_analyze_prologue (pc, (CORE_ADDR)-1, &data);

  if (data.size != -1 && data.cfa_reg != -1)
    {
      /* The 'frame pointer' address is CFA minus frame size.  */
      *reg = data.cfa_reg;
      *offset = data.cfa_offset - data.size;
    }
  else
    {
      /* ??? We don't really know ... */
      *reg = SPU_SP_REGNUM;
      *offset = 0;
    }
}

/* Normal stack frames.  */

struct spu_unwind_cache
{
  CORE_ADDR func;
  CORE_ADDR frame_base;
  CORE_ADDR local_base;

  struct trad_frame_saved_reg *saved_regs;
};

static struct spu_unwind_cache *
spu_frame_unwind_cache (struct frame_info *next_frame,
			void **this_prologue_cache)
{
  struct spu_unwind_cache *info;
  struct spu_prologue_data data;

  if (*this_prologue_cache)
    return *this_prologue_cache;

  info = FRAME_OBSTACK_ZALLOC (struct spu_unwind_cache);
  *this_prologue_cache = info;
  info->saved_regs = trad_frame_alloc_saved_regs (next_frame);
  info->frame_base = 0;
  info->local_base = 0;

  /* Find the start of the current function, and analyze its prologue.  */
  info->func = frame_func_unwind (next_frame);
  if (info->func == 0)
    {
      /* Fall back to using the current PC as frame ID.  */
      info->func = frame_pc_unwind (next_frame);
      data.size = -1;
    }
  else
    spu_analyze_prologue (info->func, frame_pc_unwind (next_frame), &data);


  /* If successful, use prologue analysis data.  */
  if (data.size != -1 && data.cfa_reg != -1)
    {
      CORE_ADDR cfa;
      int i;
      gdb_byte buf[16];

      /* Determine CFA via unwound CFA_REG plus CFA_OFFSET.  */
      frame_unwind_register (next_frame, data.cfa_reg, buf);
      cfa = extract_unsigned_integer (buf, 4) + data.cfa_offset;

      /* Call-saved register slots.  */
      for (i = 0; i < SPU_NUM_GPRS; i++)
	if (i == SPU_LR_REGNUM
	    || (i >= SPU_SAVED1_REGNUM && i <= SPU_SAVEDN_REGNUM))
	  if (data.reg_offset[i] != -1)
	    info->saved_regs[i].addr = cfa - data.reg_offset[i];

      /* The previous PC comes from the link register.  */
      if (trad_frame_addr_p (info->saved_regs, SPU_LR_REGNUM))
	info->saved_regs[SPU_PC_REGNUM] = info->saved_regs[SPU_LR_REGNUM];
      else
	info->saved_regs[SPU_PC_REGNUM].realreg = SPU_LR_REGNUM;

      /* The previous SP is equal to the CFA.  */
      trad_frame_set_value (info->saved_regs, SPU_SP_REGNUM, cfa);

      /* Frame bases.  */
      info->frame_base = cfa;
      info->local_base = cfa - data.size;
    }

  /* Otherwise, fall back to reading the backchain link.  */
  else
    {
      CORE_ADDR reg, backchain;

      /* Get the backchain.  */
      reg = frame_unwind_register_unsigned (next_frame, SPU_SP_REGNUM);
      backchain = read_memory_unsigned_integer (reg, 4);

      /* A zero backchain terminates the frame chain.  Also, sanity
         check against the local store size limit.  */
      if (backchain != 0 && backchain < SPU_LS_SIZE)
	{
	  /* Assume the link register is saved into its slot.  */
	  if (backchain + 16 < SPU_LS_SIZE)
	    info->saved_regs[SPU_LR_REGNUM].addr = backchain + 16;

	  /* This will also be the previous PC.  */
	  if (trad_frame_addr_p (info->saved_regs, SPU_LR_REGNUM))
	    info->saved_regs[SPU_PC_REGNUM] = info->saved_regs[SPU_LR_REGNUM];
	  else
	    info->saved_regs[SPU_PC_REGNUM].realreg = SPU_LR_REGNUM;

	  /* The previous SP will equal the backchain value.  */
	  trad_frame_set_value (info->saved_regs, SPU_SP_REGNUM, backchain);

          /* Frame bases.  */
	  info->frame_base = backchain;
	  info->local_base = reg;
	}
    }
 
  return info;
}

static void
spu_frame_this_id (struct frame_info *next_frame,
		   void **this_prologue_cache, struct frame_id *this_id)
{
  struct spu_unwind_cache *info =
    spu_frame_unwind_cache (next_frame, this_prologue_cache);

  if (info->frame_base == 0)
    return;

  *this_id = frame_id_build (info->frame_base, info->func);
}

static void
spu_frame_prev_register (struct frame_info *next_frame,
			 void **this_prologue_cache,
			 int regnum, int *optimizedp,
			 enum lval_type *lvalp, CORE_ADDR * addrp,
			 int *realnump, gdb_byte *bufferp)
{
  struct spu_unwind_cache *info
    = spu_frame_unwind_cache (next_frame, this_prologue_cache);

  /* Special-case the stack pointer.  */
  if (regnum == SPU_RAW_SP_REGNUM)
    regnum = SPU_SP_REGNUM;

  trad_frame_get_prev_register (next_frame, info->saved_regs, regnum,
				optimizedp, lvalp, addrp, realnump, bufferp);
}

static const struct frame_unwind spu_frame_unwind = {
  NORMAL_FRAME,
  spu_frame_this_id,
  spu_frame_prev_register
};

const struct frame_unwind *
spu_frame_sniffer (struct frame_info *next_frame)
{
  return &spu_frame_unwind;
}

static CORE_ADDR
spu_frame_base_address (struct frame_info *next_frame, void **this_cache)
{
  struct spu_unwind_cache *info
    = spu_frame_unwind_cache (next_frame, this_cache);
  return info->local_base;
}

static const struct frame_base spu_frame_base = {
  &spu_frame_unwind,
  spu_frame_base_address,
  spu_frame_base_address,
  spu_frame_base_address
};

static CORE_ADDR
spu_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, SPU_PC_REGNUM);
}

static CORE_ADDR
spu_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, SPU_SP_REGNUM);
}


/* Function calling convention.  */

static int
spu_scalar_value_p (struct type *type)
{
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_INT:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_BOOL:
    case TYPE_CODE_PTR:
    case TYPE_CODE_REF:
      return TYPE_LENGTH (type) <= 16;

    default:
      return 0;
    }
}

static void
spu_value_to_regcache (struct regcache *regcache, int regnum,
		       struct type *type, const gdb_byte *in)
{
  int len = TYPE_LENGTH (type);

  if (spu_scalar_value_p (type))
    {
      int preferred_slot = len < 4 ? 4 - len : 0;
      regcache_cooked_write_part (regcache, regnum, preferred_slot, len, in);
    }
  else
    {
      while (len >= 16)
	{
	  regcache_cooked_write (regcache, regnum++, in);
	  in += 16;
	  len -= 16;
	}

      if (len > 0)
	regcache_cooked_write_part (regcache, regnum, 0, len, in);
    }
}

static void
spu_regcache_to_value (struct regcache *regcache, int regnum,
		       struct type *type, gdb_byte *out)
{
  int len = TYPE_LENGTH (type);

  if (spu_scalar_value_p (type))
    {
      int preferred_slot = len < 4 ? 4 - len : 0;
      regcache_cooked_read_part (regcache, regnum, preferred_slot, len, out);
    }
  else
    {
      while (len >= 16)
	{
	  regcache_cooked_read (regcache, regnum++, out);
	  out += 16;
	  len -= 16;
	}

      if (len > 0)
	regcache_cooked_read_part (regcache, regnum, 0, len, out);
    }
}

static CORE_ADDR
spu_push_dummy_call (struct gdbarch *gdbarch, struct value *function,
		     struct regcache *regcache, CORE_ADDR bp_addr,
		     int nargs, struct value **args, CORE_ADDR sp,
		     int struct_return, CORE_ADDR struct_addr)
{
  int i;
  int regnum = SPU_ARG1_REGNUM;
  int stack_arg = -1;
  gdb_byte buf[16];

  /* Set the return address.  */
  memset (buf, 0, sizeof buf);
  store_unsigned_integer (buf, 4, bp_addr);
  regcache_cooked_write (regcache, SPU_LR_REGNUM, buf);

  /* If STRUCT_RETURN is true, then the struct return address (in
     STRUCT_ADDR) will consume the first argument-passing register.
     Both adjust the register count and store that value.  */
  if (struct_return)
    {
      memset (buf, 0, sizeof buf);
      store_unsigned_integer (buf, 4, struct_addr);
      regcache_cooked_write (regcache, regnum++, buf);
    }

  /* Fill in argument registers.  */
  for (i = 0; i < nargs; i++)
    {
      struct value *arg = args[i];
      struct type *type = check_typedef (value_type (arg));
      const gdb_byte *contents = value_contents (arg);
      int len = TYPE_LENGTH (type);
      int n_regs = align_up (len, 16) / 16;

      /* If the argument doesn't wholly fit into registers, it and
	 all subsequent arguments go to the stack.  */
      if (regnum + n_regs - 1 > SPU_ARGN_REGNUM)
	{
	  stack_arg = i;
	  break;
	}

      spu_value_to_regcache (regcache, regnum, type, contents);
      regnum += n_regs;
    }

  /* Overflow arguments go to the stack.  */
  if (stack_arg != -1)
    {
      CORE_ADDR ap;

      /* Allocate all required stack size.  */
      for (i = stack_arg; i < nargs; i++)
	{
	  struct type *type = check_typedef (value_type (args[i]));
	  sp -= align_up (TYPE_LENGTH (type), 16);
	}

      /* Fill in stack arguments.  */
      ap = sp;
      for (i = stack_arg; i < nargs; i++)
	{
	  struct value *arg = args[i];
	  struct type *type = check_typedef (value_type (arg));
	  int len = TYPE_LENGTH (type);
	  int preferred_slot;
	  
	  if (spu_scalar_value_p (type))
	    preferred_slot = len < 4 ? 4 - len : 0;
	  else
	    preferred_slot = 0;

	  target_write_memory (ap + preferred_slot, value_contents (arg), len);
	  ap += align_up (TYPE_LENGTH (type), 16);
	}
    }

  /* Allocate stack frame header.  */
  sp -= 32;

  /* Finally, update the SP register.  */
  regcache_cooked_write_unsigned (regcache, SPU_SP_REGNUM, sp);

  return sp;
}

static struct frame_id
spu_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_id_build (spu_unwind_sp (gdbarch, next_frame),
			 spu_unwind_pc (gdbarch, next_frame));
}

/* Function return value access.  */

static enum return_value_convention
spu_return_value (struct gdbarch *gdbarch, struct type *type,
                  struct regcache *regcache, gdb_byte *out, const gdb_byte *in)
{
  enum return_value_convention rvc;

  if (TYPE_LENGTH (type) <= (SPU_ARGN_REGNUM - SPU_ARG1_REGNUM + 1) * 16)
    rvc = RETURN_VALUE_REGISTER_CONVENTION;
  else
    rvc = RETURN_VALUE_STRUCT_CONVENTION;

  if (in)
    {
      switch (rvc)
	{
	case RETURN_VALUE_REGISTER_CONVENTION:
	  spu_value_to_regcache (regcache, SPU_ARG1_REGNUM, type, in);
	  break;

	case RETURN_VALUE_STRUCT_CONVENTION:
	  error ("Cannot set function return value.");
	  break;
	}
    }
  else if (out)
    {
      switch (rvc)
	{
	case RETURN_VALUE_REGISTER_CONVENTION:
	  spu_regcache_to_value (regcache, SPU_ARG1_REGNUM, type, out);
	  break;

	case RETURN_VALUE_STRUCT_CONVENTION:
	  error ("Function return value unknown.");
	  break;
	}
    }

  return rvc;
}


/* Breakpoints.  */

static const gdb_byte *
spu_breakpoint_from_pc (CORE_ADDR * pcptr, int *lenptr)
{
  static const gdb_byte breakpoint[] = { 0x00, 0x00, 0x3f, 0xff };

  *lenptr = sizeof breakpoint;
  return breakpoint;
}


/* Software single-stepping support.  */

void
spu_software_single_step (enum target_signal signal, int insert_breakpoints_p)
{
  if (insert_breakpoints_p)
    {
      CORE_ADDR pc, next_pc;
      unsigned int insn;
      int offset, reg;
      gdb_byte buf[4];

      regcache_cooked_read (current_regcache, SPU_PC_REGNUM, buf);
      pc = extract_unsigned_integer (buf, 4);

      if (target_read_memory (pc, buf, 4))
	return;
      insn = extract_unsigned_integer (buf, 4);

       /* Next sequential instruction is at PC + 4, except if the current
	  instruction is a PPE-assisted call, in which case it is at PC + 8.
	  Wrap around LS limit to be on the safe side.  */
      if ((insn & 0xffffff00) == 0x00002100)
	next_pc = (pc + 8) & (SPU_LS_SIZE - 1) & -4;
      else
	next_pc = (pc + 4) & (SPU_LS_SIZE - 1) & -4;

      insert_single_step_breakpoint (next_pc);

      if (is_branch (insn, &offset, &reg))
	{
	  CORE_ADDR target = offset;

	  if (reg == SPU_PC_REGNUM)
	    target += pc;
	  else if (reg != -1)
	    {
	      regcache_cooked_read_part (current_regcache, reg, 0, 4, buf);
	      target += extract_unsigned_integer (buf, 4);
	    }

	  target = target & (SPU_LS_SIZE - 1) & -4;
	  if (target != next_pc)
	    insert_single_step_breakpoint (target);
	}
    }
  else
    remove_single_step_breakpoints ();
}


/* Set up gdbarch struct.  */

static struct gdbarch *
spu_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;

  /* Find a candidate among the list of pre-declared architectures.  */
  arches = gdbarch_list_lookup_by_info (arches, &info);
  if (arches != NULL)
    return arches->gdbarch;

  /* Is is for us?  */
  if (info.bfd_arch_info->mach != bfd_mach_spu)
    return NULL;

  /* Yes, create a new architecture.  */
  gdbarch = gdbarch_alloc (&info, NULL);

  /* Disassembler.  */
  set_gdbarch_print_insn (gdbarch, print_insn_spu);

  /* Registers.  */
  set_gdbarch_num_regs (gdbarch, SPU_NUM_REGS);
  set_gdbarch_num_pseudo_regs (gdbarch, SPU_NUM_PSEUDO_REGS);
  set_gdbarch_sp_regnum (gdbarch, SPU_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, SPU_PC_REGNUM);
  set_gdbarch_register_name (gdbarch, spu_register_name);
  set_gdbarch_register_type (gdbarch, spu_register_type);
  set_gdbarch_pseudo_register_read (gdbarch, spu_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, spu_pseudo_register_write);
  set_gdbarch_convert_register_p (gdbarch, spu_convert_register_p);
  set_gdbarch_register_to_value (gdbarch, spu_register_to_value);
  set_gdbarch_value_to_register (gdbarch, spu_value_to_register);
  set_gdbarch_register_reggroup_p (gdbarch, spu_register_reggroup_p);

  /* Data types.  */
  set_gdbarch_char_signed (gdbarch, 0);
  set_gdbarch_ptr_bit (gdbarch, 32);
  set_gdbarch_addr_bit (gdbarch, 32);
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_long_bit (gdbarch, 32);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 64);
  set_gdbarch_float_format (gdbarch, &floatformat_ieee_single_big);
  set_gdbarch_double_format (gdbarch, &floatformat_ieee_double_big);
  set_gdbarch_long_double_format (gdbarch, &floatformat_ieee_double_big);

  /* Inferior function calls.  */
  set_gdbarch_push_dummy_call (gdbarch, spu_push_dummy_call);
  set_gdbarch_unwind_dummy_id (gdbarch, spu_unwind_dummy_id);
  set_gdbarch_return_value (gdbarch, spu_return_value);

  /* Frame handling.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  frame_unwind_append_sniffer (gdbarch, spu_frame_sniffer);
  frame_base_set_default (gdbarch, &spu_frame_base);
  set_gdbarch_unwind_pc (gdbarch, spu_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, spu_unwind_sp);
  set_gdbarch_virtual_frame_pointer (gdbarch, spu_virtual_frame_pointer);
  set_gdbarch_frame_args_skip (gdbarch, 0);
  set_gdbarch_skip_prologue (gdbarch, spu_skip_prologue);

  /* Breakpoints.  */
  set_gdbarch_decr_pc_after_break (gdbarch, 4);
  set_gdbarch_breakpoint_from_pc (gdbarch, spu_breakpoint_from_pc);
  set_gdbarch_cannot_step_breakpoint (gdbarch, 1);
  set_gdbarch_software_single_step (gdbarch, spu_software_single_step);

  return gdbarch;
}

void
_initialize_spu_tdep (void)
{
  register_gdbarch_init (bfd_arch_spu, spu_gdbarch_init);
}
