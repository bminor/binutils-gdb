/* Target-dependent code for the Xtensa port of GDB, the GNU debugger.

   Copyright (C) 2003, 2005, 2006 Free Software Foundation, Inc.

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
#include "frame.h"
#include "symtab.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "value.h"
#include "dis-asm.h"
#include "inferior.h"
#include "floatformat.h"
#include "regcache.h"
#include "reggroups.h"
#include "regset.h"

#include "dummy-frame.h"
#include "elf/dwarf2.h"
#include "dwarf2-frame.h"
#include "dwarf2loc.h"
#include "frame.h"
#include "frame-base.h"
#include "frame-unwind.h"

#include "arch-utils.h"
#include "gdbarch.h"
#include "remote.h"
#include "serial.h"

#include "command.h"
#include "gdbcmd.h"
#include "gdb_assert.h"

#include "xtensa-tdep.h"


static int xtensa_debug_level = 0;

#define DEBUGWARN(args...) \
  if (xtensa_debug_level > 0) \
    fprintf_unfiltered (gdb_stdlog, "(warn ) " args)

#define DEBUGINFO(args...) \
  if (xtensa_debug_level > 1) \
    fprintf_unfiltered (gdb_stdlog, "(info ) " args)

#define DEBUGTRACE(args...) \
  if (xtensa_debug_level > 2) \
    fprintf_unfiltered (gdb_stdlog, "(trace) " args)

#define DEBUGVERB(args...) \
  if (xtensa_debug_level > 3) \
    fprintf_unfiltered (gdb_stdlog, "(verb ) " args)


/* According to the ABI, the SP must be aligned to 16-byte boundaries.  */

#define SP_ALIGNMENT 16


/* We use a6 through a11 for passing arguments to a function called by GDB.  */

#define ARGS_FIRST_REG    A6_REGNUM
#define ARGS_NUM_REGS     6
#define REGISTER_SIZE	  4


/* Extract the call size from the return address or ps register.  */

#define PS_CALLINC_SHIFT 16
#define PS_CALLINC_MASK 0x00030000
#define CALLINC(ps) (((ps) & PS_CALLINC_MASK) >> PS_CALLINC_SHIFT)
#define WINSIZE(ra) (4 * (( (ra) >> 30) & 0x3))


/* Convert a live Ax register number to the corresponding Areg number.  */

#define AREG_NUMBER(r, wb) \
  ((((r) - A0_REGNUM + (((wb) & WB_MASK)<<WB_SHIFT)) & AREGS_MASK) + AR_BASE)


/* Define prototypes.  */

extern struct gdbarch_tdep *xtensa_config_tdep (struct gdbarch_info *);
extern int xtensa_config_byte_order (struct gdbarch_info *);


/* XTENSA_IS_ENTRY tests whether the first byte of an instruction
   indicates that the instruction is an ENTRY instruction.  */

#define XTENSA_IS_ENTRY(op1) \
  ((TARGET_BYTE_ORDER == BFD_ENDIAN_BIG) ? ((op1) == 0x6c) : ((op1) == 0x36))

#define XTENSA_ENTRY_LENGTH  3


/* windowing_enabled() returns true, if windowing is enabled.
   WOE must be set to 1; EXCM to 0.
   Note: We assume that EXCM is always 0 for XEA1.  */

static inline int
windowing_enabled (CORE_ADDR ps)
{
  return ((ps & (1 << 4)) == 0 && (ps & (1 << 18)) != 0);
}

/* Return the window size of the previous call to the function from which we
   have just returned.

   This function is used to extract the return value after a called function
   has returned to the callee.  On Xtensa, the register that holds the return
   value (from the perspective of the caller) depends on what call
   instruction was used.  For now, we are assuming that the call instruction
   precedes the current address, so we simply analyze the call instruction.
   If we are in a dummy frame, we simply return 4 as we used a 'pseudo-call4'
   method to call the inferior function.  */

static int
extract_call_winsize (CORE_ADDR pc)
{
  int winsize = 4;	/* Default: No call, e.g. dummy frame.  */
  int insn;
  char buf[4];

  DEBUGTRACE ("extract_call_winsize (pc = 0x%08x)\n", (int) pc);

  /* Read the previous instruction (should be a call[x]{4|8|12}.  */
  read_memory (pc-3, buf, 3);
  insn = extract_unsigned_integer (buf, 3);

  /* Decode call instruction:
     Little Endian
       call{0,4,8,12}   OFFSET || {00,01,10,11} || 0101
       callx{0,4,8,12}  OFFSET || 11 || {00,01,10,11} || 0000
     Big Endian
       call{0,4,8,12}   0101 || {00,01,10,11} || OFFSET
       callx{0,4,8,12}  0000 || {00,01,10,11} || 11 || OFFSET.  */

  /* Lookup call insn.
     (Return the default value (4) if we can't find a valid call insn.  */

  if (TARGET_BYTE_ORDER == BFD_ENDIAN_LITTLE)
    {
      if (((insn & 0xf) == 0x5) || ((insn & 0xcf) == 0xc0))
	winsize = (insn & 0x30) >> 2;   /* 0, 4, 8, 12  */
    }
  else
    {
      if (((insn >> 20) == 0x5) || (((insn >> 16) & 0xf3) == 0x03))
	winsize = (insn >> 16) & 0xc;   /* 0, 4, 8, 12  */
    }
  return winsize;
}


/* REGISTER INFORMATION */

/* Returns the name of a register.  */

static const char *
xtensa_register_name (int regnum)
{
  /* Return the name stored in the register map.  */
  if (regnum >= 0 && regnum < NUM_REGS + NUM_PSEUDO_REGS)
    return REGMAP[regnum].name;

  /* Invalid register number.  */
  internal_error (__FILE__, __LINE__, _("invalid register %d"), regnum);
  return 0;
}


/* Return the type of a register.  Create a new type, if necessary.  */

static struct ctype_cache
{
  struct ctype_cache *next;
  int size;
  struct type *virtual_type;
} *type_entries = NULL;

static struct type *
xtensa_register_type (struct gdbarch *gdbarch, int regnum)
{
  /* Return signed integer for ARx and Ax registers.  */
  if ((regnum >= AR_BASE && regnum < AR_BASE + NUM_AREGS)
      || (regnum >= A0_BASE && regnum < A0_BASE + 16))
    return builtin_type_int;

  if (regnum == PC_REGNUM || regnum == A1_REGNUM)
    return lookup_pointer_type (builtin_type_void);

  /* Return the stored type for all other registers.  */
  else if (regnum >= 0 && regnum < NUM_REGS + NUM_PSEUDO_REGS)
    {
      xtensa_register_t* reg = &REGMAP[regnum];

      /* Set ctype for this register (only the first time we ask for it).  */

      if (reg->ctype == 0)
	{
	  struct ctype_cache *tp;
	  int size = reg->byte_size;

	  /* We always use the memory representation, even if the register
	     width is smaller.  */
	  switch (size)
	    {
	    case 1:
	      reg->ctype = builtin_type_uint8;
	      break;

	    case 2:
	      reg->ctype = builtin_type_uint16;
	      break;

	    case 4:
	      reg->ctype = builtin_type_uint32;
	      break;

	    case 8:
	      reg->ctype = builtin_type_uint64;
	      break;

	    case 16:
	      reg->ctype = builtin_type_uint128;
	      break;

	    default:
	      for (tp = type_entries; tp != NULL; tp = tp->next)
		if (tp->size == size)
		  break;

	      if (tp == NULL)
		{
		  char *name = xmalloc (16);
		  tp = xmalloc (sizeof (struct ctype_cache));
		  tp->next = type_entries;
		  type_entries = tp;
		  tp->size = size;

		  sprintf (name, "int%d", size * 8);
		  tp->virtual_type = init_type (TYPE_CODE_INT, size,
						TYPE_FLAG_UNSIGNED, name,
						NULL);
		}

	      reg->ctype = tp->virtual_type;
	    }
	}
      return reg->ctype;
    }

  /* Invalid register number.  */
  internal_error (__FILE__, __LINE__, _("invalid register number %d"), regnum);
  return 0;
}


/* Returns the 'local' register number for stubs, dwarf2, etc.
   The debugging information enumerates registers starting from 0 for A0
   to n for An.  So, we only have to add the base number for A0.  */

static int
xtensa_reg_to_regnum (int regnum)
{
  int i;

  if (regnum >= 0 && regnum < 16)
    return A0_BASE + regnum;

  for (i = 0; i < NUM_REGS + NUM_PSEUDO_REGS; i++)
    if (regnum == REGMAP[i].target_number)
      return i;

  /* Invalid register number.  */
  internal_error (__FILE__, __LINE__,
		  _("invalid dwarf/stabs register number %d"), regnum);
  return 0;
}


/* Handle the special case of masked registers.  */

/* Write the bits of a masked register to the various registers that
   are combined into this register.  Only the masked areas of these
   registers are modified; the other fields are untouched.
   (Note: The size of masked registers is always less or equal 32 bits.)  */

static void
xtensa_register_write_masked (xtensa_register_t *reg, unsigned char *buffer)
{
  unsigned int value[(MAX_REGISTER_SIZE + 3) / 4];

  const xtensa_mask_t *mask = reg->mask;

  int shift = 0;		/* Shift for next mask (mod 32).  */
  int start, size;		/* Start bit and size of current mask.  */

  unsigned int *ptr = value;
  unsigned int regval, m, mem = 0;

  int bytesize = reg->byte_size;
  int bitsize = bytesize * 8;
  int i, r;

  DEBUGTRACE ("xtensa_register_write_masked ()\n");

  /* Copy the masked register to host byte-order.  */
  if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
    for (i = 0; i < bytesize; i++)
      {
	mem >>= 8;
	mem |= (buffer[bytesize - i - 1] << 24);
	if ((i & 3) == 3)
	  *ptr++ = mem;
      }
  else
    for (i = 0; i < bytesize; i++)
      {
	mem >>= 8;
	mem |= (buffer[i] << 24);
	if ((i & 3) == 3)
	  *ptr++ = mem;
      }

  /* We might have to shift the final value:
     bytesize & 3 == 0 -> nothing to do, we use the full 32 bits,
     bytesize & 3 == x -> shift (4-x) * 8.  */

  *ptr = mem >> (((0 - bytesize) & 3) * 8);
  ptr = value;
  mem = *ptr;

  /* Write the bits to the masked areas of the other registers.  */
  for (i = 0; i < mask->count; i++)
    {
      start = mask->mask[i].bit_start;
      size = mask->mask[i].bit_size;
      regval = mem >> shift;

      if ((shift += size) > bitsize)
	error (_("size of all masks is larger than the register"));

      if (shift >= 32)
	{
	  mem = *(++ptr);
	  shift -= 32;
	  bitsize -= 32;

	  if (shift > 0)
	    regval |= mem << (size - shift);
	}

      /* Make sure we have a valid register.  */
      r = mask->mask[i].reg_num;
      if (r >= 0 && size > 0)
	{
	  /* Don't overwrite the unmasked areas.  */
	  m = 0xffffffff >> (32 - size) << start;
	  regval <<= start;
	  regval = (regval & m) | (read_register (r) & ~m);
	  write_register (r, regval);
	}
    }
}


/* Read the masked areas of the registers and assemble it into a single
   register.  */

static void
xtensa_register_read_masked (xtensa_register_t *reg, unsigned char *buffer)
{
  unsigned int value[(MAX_REGISTER_SIZE + 3) / 4];

  const xtensa_mask_t *mask = reg->mask;

  int shift = 0;
  int start, size;

  unsigned int *ptr = value;
  unsigned int regval, mem = 0;

  int bytesize = reg->byte_size;
  int bitsize = bytesize * 8;
  int i;

  DEBUGTRACE ("xtensa_register_read_masked (reg \"%s\", ...)\n",
	      reg->name == 0 ? "" : reg->name);

  /* Assemble the register from the masked areas of other registers.  */
  for (i = 0; i < mask->count; i++)
    {
      int r = mask->mask[i].reg_num;
      regval = (r >= 0) ? read_register (r) : 0;
      start = mask->mask[i].bit_start;
      size = mask->mask[i].bit_size;

      regval >>= start;

      if (size < 32)
	regval &= (0xffffffff >> (32 - size));

      mem |= regval << shift;

      if ((shift += size) > bitsize)
	error (_("size of all masks is larger than the register"));

      if (shift >= 32)
	{
	  *ptr++ = mem;
	  bitsize -= 32;
	  shift -= 32;

	  if (shift == 0)
	    mem = 0;
	  else
	    mem = regval >> (size - shift);
	}
    }

  if (shift > 0)
    *ptr = mem;

  /* Copy value to target byte order.  */
  ptr = value;
  mem = *ptr;

  if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
    for (i = 0; i < bytesize; i++)
      {
	if ((i & 3) == 0)
	  mem = *ptr++;
	buffer[bytesize - i - 1] = mem & 0xff;
	mem >>= 8;
      }
  else
    for (i = 0; i < bytesize; i++)
      {
	if ((i & 3) == 0)
	  mem = *ptr++;
	buffer[i] = mem & 0xff;
	mem >>= 8;
      }
}


/* Read pseudo registers.  */

static void
xtensa_pseudo_register_read (struct gdbarch *gdbarch,
			     struct regcache *regcache,
			     int regnum,
			     gdb_byte *buffer)
{
  DEBUGTRACE ("xtensa_pseudo_register_read (... regnum = %d (%s) ...)\n",
	      regnum, xtensa_register_name (regnum));

  /* Check if it is FP (renumber it in this case -> A0...A15).  */
  if (regnum == FP_ALIAS)
    error (_("trying to read FP"));

  /* Read aliases a0..a15.  */
  if (regnum >= A0_REGNUM && regnum <= A15_REGNUM)
    {
      char *buf = (char *) alloca (MAX_REGISTER_SIZE);

      regcache_raw_read (regcache, WB_REGNUM, buf);
      regnum = AREG_NUMBER (regnum, extract_unsigned_integer (buf, 4));
    }

  /* We can always read 'regular' registers.  */
  if (regnum >= 0 && regnum < NUM_REGS)
    regcache_raw_read (regcache, regnum, buffer);

  /* Pseudo registers.  */
  else if (regnum >= 0 && regnum < NUM_REGS + NUM_PSEUDO_REGS)
    {
      xtensa_register_t *reg = &REGMAP[regnum];
      xtensa_register_type_t type = reg->type;
      int flags = XTENSA_TARGET_FLAGS;

      /* Can we read Unknown or Unmapped registers?  */
      if (type == xtRegisterTypeUnmapped || type == xtRegisterTypeUnknown)
	{
	  if ((flags & xtTargetFlagsNonVisibleRegs) == 0)
	    {
	      warning (_("cannot read register %s"),
		       xtensa_register_name (regnum));
	      return;
	    }
	}

      /* Some targets cannot read TIE register files.  */
      else if (type == xtRegisterTypeTieRegfile)
        {
	  /* Use 'fetch' to get register?  */
	  if (flags & xtTargetFlagsUseFetchStore)
	    {
	      warning (_("cannot read register"));
	      return;
	    }

	  /* On some targets (esp. simulators), we can always read the reg.  */
	  else if ((flags & xtTargetFlagsNonVisibleRegs) == 0)
	    {
	      warning (_("cannot read register"));
	      return;
	    }
	}

      /* We can always read mapped registers.  */
      else if (type == xtRegisterTypeMapped || type == xtRegisterTypeTieState)
        {
	  xtensa_register_read_masked (reg, (unsigned char *) buffer);
	  return;
	}

      /* Assume that we can read the register.  */
      regcache_raw_read (regcache, regnum, buffer);
    }

  else
    internal_error (__FILE__, __LINE__,
		    _("invalid register number %d"), regnum);
}


/* Write pseudo registers.  */

static void
xtensa_pseudo_register_write (struct gdbarch *gdbarch,
			      struct regcache *regcache,
			      int regnum,
			      const gdb_byte *buffer)
{
  DEBUGTRACE ("xtensa_pseudo_register_write (... regnum = %d (%s) ...)\n",
	      regnum, xtensa_register_name (regnum));

  /* Check if this is FP.  */
  if (regnum == FP_ALIAS)
    error (_("trying to write FP"));

  /* Renumber register, if aliase a0..a15.  */
  if (regnum >= A0_REGNUM && regnum <= A15_REGNUM)
    {
      char *buf = (char *) alloca (MAX_REGISTER_SIZE);
      unsigned int wb;

      regcache_raw_read (regcache, WB_REGNUM, buf);
      regnum = AREG_NUMBER (regnum, extract_unsigned_integer (buf, 4));
    }

  /* We can always write 'core' registers.
     Note: We might have converted Ax->ARy.  */
  if (regnum >= 0 && regnum < NUM_REGS)
    regcache_raw_write (regcache, regnum, buffer);

  /* Pseudo registers.  */
  else if (regnum >= 0 && regnum < NUM_REGS + NUM_PSEUDO_REGS)
    {
      xtensa_register_t *reg = &REGMAP[regnum];
      xtensa_register_type_t type = reg->type;
      int flags = XTENSA_TARGET_FLAGS;

      /* On most targets, we can't write registers of type "Unknown"
	 or "Unmapped".  */
      if (type == xtRegisterTypeUnmapped || type == xtRegisterTypeUnknown)
        {
	  if ((flags & xtTargetFlagsNonVisibleRegs) == 0)
	    {
	      warning (_("cannot write register %s"),
		       xtensa_register_name (regnum));
	      return;
	    }
	}

      /* Some targets cannot read TIE register files.  */
      else if (type == xtRegisterTypeTieRegfile)
        {
	  /* Use 'store' to get register?  */
	  if (flags & xtTargetFlagsUseFetchStore)
	    {
	      warning (_("cannot write register"));
	      return;
	    }

	  /* On some targets (esp. simulators), we can always write
	     the register.  */

	  else if ((flags & xtTargetFlagsNonVisibleRegs) == 0)
	    {
	      warning (_("cannot write register"));
	      return;
	    }
	}

      /* We can always write mapped registers.  */
      else if (type == xtRegisterTypeMapped || type == xtRegisterTypeTieState)
        {
	  xtensa_register_write_masked (reg, (unsigned char *) buffer);
	  return;
	}

      /* Assume that we can write the register.  */
      regcache_raw_write (regcache, regnum, buffer);
    }

  else
    internal_error (__FILE__, __LINE__,
		    _("invalid register number %d"), regnum);
}


static struct reggroup *xtensa_ar_reggroup;
static struct reggroup *xtensa_user_reggroup;
static struct reggroup *xtensa_vectra_reggroup;

static void
xtensa_init_reggroups (void)
{
  xtensa_ar_reggroup = reggroup_new ("ar", USER_REGGROUP);
  xtensa_user_reggroup = reggroup_new ("user", USER_REGGROUP);
  xtensa_vectra_reggroup = reggroup_new ("vectra", USER_REGGROUP);
}


static void
xtensa_add_reggroups (struct gdbarch *gdbarch)
{
  reggroup_add (gdbarch, all_reggroup);
  reggroup_add (gdbarch, save_reggroup);
  reggroup_add (gdbarch, restore_reggroup);
  reggroup_add (gdbarch, system_reggroup);
  reggroup_add (gdbarch, vector_reggroup);		/* vectra */
  reggroup_add (gdbarch, general_reggroup);		/* core */
  reggroup_add (gdbarch, float_reggroup);		/* float */

  reggroup_add (gdbarch, xtensa_ar_reggroup);		/* ar */
  reggroup_add (gdbarch, xtensa_user_reggroup);		/* user */
  reggroup_add (gdbarch, xtensa_vectra_reggroup);	/* vectra */
}


#define SAVE_REST_FLAGS	(XTENSA_REGISTER_FLAGS_READABLE \
			| XTENSA_REGISTER_FLAGS_WRITABLE \
			| XTENSA_REGISTER_FLAGS_VOLATILE)

#define SAVE_REST_VALID	(XTENSA_REGISTER_FLAGS_READABLE \
			| XTENSA_REGISTER_FLAGS_WRITABLE)

static int
xtensa_register_reggroup_p (struct gdbarch *gdbarch,
			    int regnum,
    			    struct reggroup *group)
{
  xtensa_register_t* reg = &REGMAP[regnum];
  xtensa_register_type_t type = reg->type;
  xtensa_register_group_t rg = reg->group;

  /* First, skip registers that are not visible to this target
     (unknown and unmapped registers when not using ISS).  */

  if (type == xtRegisterTypeUnmapped || type == xtRegisterTypeUnknown)
    return 0;
  if (group == all_reggroup)
    return 1;
  if (group == xtensa_ar_reggroup)
    return rg & xtRegisterGroupAddrReg;
  if (group == xtensa_user_reggroup)
    return rg & xtRegisterGroupUser;
  if (group == float_reggroup)
    return rg & xtRegisterGroupFloat;
  if (group == general_reggroup)
    return rg & xtRegisterGroupGeneral;
  if (group == float_reggroup)
    return rg & xtRegisterGroupFloat;
  if (group == system_reggroup)
    return rg & xtRegisterGroupState;
  if (group == vector_reggroup || group == xtensa_vectra_reggroup)
    return rg & xtRegisterGroupVectra;
  if (group == save_reggroup || group == restore_reggroup)
    return (regnum < NUM_REGS
	    && (reg->flags & SAVE_REST_FLAGS) == SAVE_REST_VALID);
  else
    return 1;
}


/* CORE FILE SUPPORT */

/* Supply register REGNUM from the buffer specified by GREGS and LEN
   in the general-purpose register set REGSET to register cache
   REGCACHE.  If REGNUM is -1, do this for all registers in REGSET.  */

static void
xtensa_supply_gregset (const struct regset *regset,
		       struct regcache *rc,
		       int regnum,
		       const void *gregs,
		       size_t len)
{
  const xtensa_elf_gregset_t *regs = gregs;
  int i;

  DEBUGTRACE ("xtensa_supply_gregset (..., regnum==%d, ...) \n", regnum);

  if (regnum == PC_REGNUM || regnum == -1)
    regcache_raw_supply (rc, PC_REGNUM, (char *) &regs->pc);
  if (regnum == PS_REGNUM || regnum == -1)
    regcache_raw_supply (rc, PS_REGNUM, (char *) &regs->ps);
  if (regnum == WB_REGNUM || regnum == -1)
    regcache_raw_supply (rc, WB_REGNUM, (char *) &regs->windowbase);
  if (regnum == WS_REGNUM || regnum == -1)
    regcache_raw_supply (rc, WS_REGNUM, (char *) &regs->windowstart);
  if (regnum == LBEG_REGNUM || regnum == -1)
    regcache_raw_supply (rc, LBEG_REGNUM, (char *) &regs->lbeg);
  if (regnum == LEND_REGNUM || regnum == -1)
    regcache_raw_supply (rc, LEND_REGNUM, (char *) &regs->lend);
  if (regnum == LCOUNT_REGNUM || regnum == -1)
    regcache_raw_supply (rc, LCOUNT_REGNUM, (char *) &regs->lcount);
  if (regnum == SAR_REGNUM || regnum == -1)
    regcache_raw_supply (rc, SAR_REGNUM, (char *) &regs->sar);
  if (regnum == EXCCAUSE_REGNUM || regnum == -1)
    regcache_raw_supply (rc, EXCCAUSE_REGNUM, (char *) &regs->exccause);
  if (regnum == EXCVADDR_REGNUM || regnum == -1)
    regcache_raw_supply (rc, EXCVADDR_REGNUM, (char *) &regs->excvaddr);
  if (regnum >= AR_BASE && regnum < AR_BASE + NUM_AREGS)
    regcache_raw_supply (rc, regnum, (char *) &regs->ar[regnum - AR_BASE]);
  else if (regnum == -1)
    {
      for (i = 0; i < NUM_AREGS; ++i)
	regcache_raw_supply (rc, AR_BASE + i, (char *) &regs->ar[i]);
    }
}


/* Xtensa register set.  */

static struct regset
xtensa_gregset =
{
  NULL,
  xtensa_supply_gregset
};


/* Return the appropriate register set for the core section identified
   by SECT_NAME and SECT_SIZE.  */

static const struct regset *
xtensa_regset_from_core_section (struct gdbarch *core_arch,
				 const char *sect_name,
				 size_t sect_size)
{
  DEBUGTRACE ("xtensa_regset_from_core_section "
	      "(..., sect_name==\"%s\", sect_size==%x) \n",
	      sect_name, sect_size);

  if (strcmp (sect_name, ".reg") == 0
      && sect_size >= sizeof(xtensa_elf_gregset_t))
    return &xtensa_gregset;

  return NULL;
}


/* F R A M E */

/* We currently don't support the call0-abi, so we have at max. 12 registers
   saved on the stack.  */

#define XTENSA_NUM_SAVED_AREGS 12

typedef struct xtensa_frame_cache
{
  CORE_ADDR base;
  CORE_ADDR pc;
  CORE_ADDR ra;		/* The raw return address; use to compute call_inc.  */
  CORE_ADDR ps;
  int wb;		/* Base for this frame; -1 if not in regfile.  */
  int callsize;		/* Call size to next frame.  */
  int ws;
  CORE_ADDR aregs[XTENSA_NUM_SAVED_AREGS];
  CORE_ADDR prev_sp;
} xtensa_frame_cache_t;


static struct xtensa_frame_cache *
xtensa_alloc_frame_cache (void)
{
  xtensa_frame_cache_t *cache;
  int i;

  DEBUGTRACE ("xtensa_alloc_frame_cache ()\n");

  cache = FRAME_OBSTACK_ZALLOC (xtensa_frame_cache_t);

  cache->base = 0;
  cache->pc = 0;
  cache->ra = 0;
  cache->wb = 0;
  cache->ps = 0;
  cache->callsize = -1;
  cache->prev_sp = 0;

  for (i = 0; i < XTENSA_NUM_SAVED_AREGS; i++)
    cache->aregs[i] = -1;

  return cache;
}


static CORE_ADDR
xtensa_frame_align (struct gdbarch *gdbarch, CORE_ADDR address)
{
  return address & ~15;
}


static CORE_ADDR
xtensa_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  char buf[8];

  DEBUGTRACE ("xtensa_unwind_pc (next_frame = %p)\n", next_frame);

  frame_unwind_register (next_frame, PC_REGNUM, buf);

  DEBUGINFO ("[xtensa_unwind_pc] pc = 0x%08x\n", (unsigned int)
	     extract_typed_address (buf, builtin_type_void_func_ptr));

  return extract_typed_address (buf, builtin_type_void_func_ptr);
}


static struct frame_id
xtensa_unwind_dummy_id (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  CORE_ADDR pc, fp;
  char buf[4];

  /* next_frame->prev is a dummy frame. Return a frame ID of that frame.  */

  DEBUGTRACE ("xtensa_unwind_dummy_id ()\n");

  pc = frame_pc_unwind (next_frame);
  frame_unwind_register (next_frame, A1_REGNUM, buf);
  fp = extract_unsigned_integer (buf, 4);

  /* Make dummy frame ID unique by adding a constant.  */
  return frame_id_build (fp+SP_ALIGNMENT, pc);
}


static struct xtensa_frame_cache *
xtensa_frame_cache (struct frame_info *next_frame, void **this_cache)
{
  xtensa_frame_cache_t *cache;
  char buf[4];
  CORE_ADDR ra, wb, ws, pc, sp, ps;
  char op1;

  DEBUGTRACE ("xtensa_frame_cache (next_frame %p, *this_cache %p)\n",
	      next_frame, this_cache ? *this_cache : (void*)0xdeadbeef);

  /* Already cached?  */
  if (*this_cache)
    return *this_cache;

  /* Get pristine xtensa-frame.  */
  cache = xtensa_alloc_frame_cache ();
  *this_cache = cache;

  /* Get windowbase, windowstart, ps, and pc.  */
  wb = frame_unwind_register_unsigned (next_frame, WB_REGNUM);
  ws = frame_unwind_register_unsigned (next_frame, WS_REGNUM);
  ps = frame_unwind_register_unsigned (next_frame, PS_REGNUM);
  pc = frame_unwind_register_unsigned (next_frame, PC_REGNUM);

  op1 = read_memory_integer (pc, 1);
  if (XTENSA_IS_ENTRY (op1) || !windowing_enabled (read_register (PS_REGNUM)))
    {
      int callinc = CALLINC (frame_unwind_register_unsigned (next_frame,
							     PS_REGNUM));
      ra = frame_unwind_register_unsigned (next_frame,
					   A0_REGNUM + callinc * 4);

      DEBUGINFO("[xtensa_frame_cache] 'entry' at 0x%08x\n (callinc = %d)",
	  	(int)pc, callinc);

      /* ENTRY hasn't been executed yet, therefore callsize is still 0.  */
      cache->callsize = 0;
      cache->wb = wb;
      cache->ws = ws;
      cache->prev_sp = read_register (A1_REGNUM);
    }
  else
    {
      ra = frame_unwind_register_unsigned (next_frame, A0_REGNUM);
      cache->callsize = WINSIZE (ra);
      cache->wb = (wb - (cache->callsize / 4)) & ((NUM_AREGS / 4) - 1);
      cache->ws = ws & ~(1 << wb);
    }

  cache->pc = ((frame_func_unwind (next_frame) & 0xc0000000)
	       | (ra & 0x3fffffff));
  cache->ps = (ps & ~PS_CALLINC_MASK) | ((WINSIZE(ra)/4) << PS_CALLINC_SHIFT);


  /* Note: We could also calculate the location on stack when we actually
     access the register.  However, this approach, saving the location
     in the cache frame, is probably easier to support the call0 ABI.  */

  if (cache->ws == 0)
    {
      int i;

      /* Set A0...A3.  */
      sp = frame_unwind_register_unsigned (next_frame, A1_REGNUM) - 16;

      for (i = 0; i < 4; i++, sp += 4)
	{
	  cache->aregs[i] = sp;
	}

      if (cache->callsize > 4)
	{
	  /* Set A4...A7/A11.  */

	  sp = (CORE_ADDR) read_memory_integer (sp - 12, 4);
	  sp = (CORE_ADDR) read_memory_integer (sp - 12, 4);
	  sp -= cache->callsize * 4;

	  for ( /* i=4  */ ; i < cache->callsize; i++, sp += 4)
	    {
	      cache->aregs[i] = sp;
	    }
	}
    }

  if (cache->prev_sp == 0) 
    {
      if (cache->ws == 0)
	{
	  /* Register window overflow already happened.
	     We can read caller's frame SP from the proper spill loction.  */
	  cache->prev_sp =
		  read_memory_integer (cache->aregs[1],
				       register_size (current_gdbarch,
						      A1_REGNUM));
	}
      else
	{
	  /* Read caller's frame SP directly from the previous window.  */

	  int regnum = AREG_NUMBER (A1_REGNUM, cache->wb);

	  cache->prev_sp = read_register (regnum);
	}
    }

  cache->base = frame_unwind_register_unsigned (next_frame,A1_REGNUM);

  DEBUGINFO ("[xtensa_frame_cache] base 0x%08x, wb %d, "
	     "ws 0x%08x, callsize %d, pc 0x%08x, ps 0x%08x, prev_sp 0x%08x\n",
	     (unsigned int) cache->base, (unsigned int) cache->wb,
	     cache->ws, cache->callsize, (unsigned int) cache->pc,
	     (unsigned int) cache->ps, (unsigned int) cache->prev_sp);

  return cache;
}


static void
xtensa_frame_this_id (struct frame_info *next_frame,
		      void **this_cache,
		      struct frame_id *this_id)
{
  struct xtensa_frame_cache *cache =
    xtensa_frame_cache (next_frame, this_cache);

  DEBUGTRACE ("xtensa_frame_this_id (next 0x%08x, *this 0x%08x)\n",
	      (unsigned int) next_frame, (unsigned int) *this_cache);

  if (cache->prev_sp == 0)
    return;

  (*this_id) = frame_id_build (cache->prev_sp, cache->pc);
}


static void
xtensa_frame_prev_register (struct frame_info *next_frame,
			    void **this_cache,
			    int regnum,
			    int *optimizedp,
			    enum lval_type *lvalp,
			    CORE_ADDR *addrp,
			    int *realnump,
			    gdb_byte *valuep)
{
  struct xtensa_frame_cache *cache =
    xtensa_frame_cache (next_frame, this_cache);
  CORE_ADDR saved_reg = 0;
  int done = 1;

  DEBUGTRACE ("xtensa_frame_prev_register (next 0x%08x, "
	      "*this 0x%08x, regnum %d (%s), ...)\n",
	      (unsigned int) next_frame,
	      *this_cache? (unsigned int) *this_cache : 0, regnum,
	      xtensa_register_name (regnum));

  if (regnum == WS_REGNUM)
    {
      if (cache->ws != 0)
	saved_reg = cache->ws;
      else
	saved_reg = 1 << cache->wb;
    }
  else if (regnum == WB_REGNUM)
    saved_reg = cache->wb;
  else if (regnum == PC_REGNUM)
    saved_reg = cache->pc;
  else if (regnum == PS_REGNUM)
    saved_reg = cache->ps;
  else
    done = 0;

  if (done)
    {
      *optimizedp = 0;
      *lvalp = not_lval;
      *addrp = 0;
      *realnump = -1;
      if (valuep)
	store_unsigned_integer (valuep, 4, saved_reg);

      return;
    }

  /* Convert Ax register numbers to ARx register numbers.  */
  if (regnum >= A0_REGNUM && regnum <= A15_REGNUM)
    regnum = AREG_NUMBER (regnum, cache->wb);

  /* Check if ARx register has been saved to stack.  */
  if (regnum >= AR_BASE && regnum <= (AR_BASE + NUM_AREGS))
    {
      int areg = regnum - AR_BASE - (cache->wb * 4);

      if (areg >= 0
	  && areg < XTENSA_NUM_SAVED_AREGS
	  && cache->aregs[areg] != -1)
        {
    	  *optimizedp = 0;
	  *lvalp = lval_memory;
	  *addrp = cache->aregs[areg];
	  *realnump = -1;

	  if (valuep)
	    read_memory (*addrp, valuep,
			 register_size (current_gdbarch, regnum));

	  DEBUGINFO ("[xtensa_frame_prev_register] register on stack\n");
	  return;
	}
    }

  /* Note: All other registers have been either saved to the dummy stack
     or are still alive in the processor.  */

  *optimizedp = 0;
  *lvalp = lval_register;
  *addrp = 0;
  *realnump = regnum;
  if (valuep)
    frame_unwind_register (next_frame, (*realnump), valuep);
}


static const struct frame_unwind
xtensa_frame_unwind =
{
  NORMAL_FRAME,
  xtensa_frame_this_id,
  xtensa_frame_prev_register
};

static const struct frame_unwind *
xtensa_frame_sniffer (struct frame_info *next_frame)
{
  return &xtensa_frame_unwind;
}

static CORE_ADDR
xtensa_frame_base_address (struct frame_info *next_frame, void **this_cache)
{
  struct xtensa_frame_cache *cache =
    xtensa_frame_cache (next_frame, this_cache);

  return cache->base;
}

static const struct frame_base
xtensa_frame_base =
{
  &xtensa_frame_unwind,
  xtensa_frame_base_address,
  xtensa_frame_base_address,
  xtensa_frame_base_address
};


static void
xtensa_extract_return_value (struct type *type,
			     struct regcache *regcache,
			     void *dst)
{
  bfd_byte *valbuf = dst;
  int len = TYPE_LENGTH (type);
  ULONGEST pc, wb;
  int callsize, areg;
  int offset = 0;

  DEBUGTRACE ("xtensa_extract_return_value (...)\n");

  gdb_assert(len > 0);

  /* First, we have to find the caller window in the register file.  */
  regcache_raw_read_unsigned (regcache, PC_REGNUM, &pc);
  callsize = extract_call_winsize (pc);

  /* On Xtensa, we can return up to 4 words (or 2 when called by call12).  */
  if (len > (callsize > 8 ? 8 : 16))
    internal_error (__FILE__, __LINE__,
	            _("cannot extract return value of %d bytes long"), len);

  /* Get the register offset of the return register (A2) in the caller
     window.  */
  regcache_raw_read_unsigned (regcache, WB_REGNUM, &wb);
  areg = AREG_NUMBER(A2_REGNUM + callsize, wb);

  DEBUGINFO ("[xtensa_extract_return_value] areg %d len %d\n", areg, len);

  if (len < 4 && TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
    offset = 4 - len;

  for (; len > 0; len -= 4, areg++, valbuf += 4)
    {
      if (len < 4)
	regcache_raw_read_part (regcache, areg, offset, len, valbuf);
      else
	regcache_raw_read (regcache, areg, valbuf);
    }
}


static void
xtensa_store_return_value (struct type *type,
			   struct regcache *regcache,
			   const void *dst)
{
  const bfd_byte *valbuf = dst;
  unsigned int areg;
  ULONGEST pc, wb;
  int callsize;
  int len = TYPE_LENGTH (type);
  int offset = 0;

  DEBUGTRACE ("xtensa_store_return_value (...)\n");

  regcache_raw_read_unsigned (regcache, WB_REGNUM, &wb);
  regcache_raw_read_unsigned (regcache, PC_REGNUM, &pc);
  callsize = extract_call_winsize (pc);

  if (len > (callsize > 8 ? 8 : 16))
    internal_error (__FILE__, __LINE__,
		    _("unimplemented for this length: %d"),
		    TYPE_LENGTH (type));

  DEBUGTRACE ("[xtensa_store_return_value] callsize %d wb %d\n",
              callsize, (int) wb);

  if (len < 4 && TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
    offset = 4 - len;

  areg = AREG_NUMBER (A2_REGNUM + callsize, wb);

  for (; len > 0; len -= 4, areg++, valbuf += 4)
    {
      if (len < 4)
	regcache_raw_write_part (regcache, areg, offset, len, valbuf);
      else
	regcache_raw_write (regcache, areg, valbuf);
    }
}


enum return_value_convention
xtensa_return_value (struct gdbarch *gdbarch,
		     struct type *valtype,
		     struct regcache *regcache,
		     gdb_byte *readbuf,
		     const gdb_byte *writebuf)
{
  /* Note: Structures up to 16 bytes are returned in registers.  */

  int struct_return = ((TYPE_CODE (valtype) == TYPE_CODE_STRUCT
			|| TYPE_CODE (valtype) == TYPE_CODE_UNION
			|| TYPE_CODE (valtype) == TYPE_CODE_ARRAY)
		       && TYPE_LENGTH (valtype) > 16);

  if (struct_return)
    return RETURN_VALUE_STRUCT_CONVENTION;

  DEBUGTRACE ("xtensa_return_value(...)\n");

  if (writebuf != NULL)
    {
      xtensa_store_return_value (valtype, regcache, writebuf);
    }

  if (readbuf != NULL)
    {
      gdb_assert (!struct_return);
      xtensa_extract_return_value (valtype, regcache, readbuf);
    }
  return RETURN_VALUE_REGISTER_CONVENTION;
}


/* DUMMY FRAME */

static CORE_ADDR
xtensa_push_dummy_call (struct gdbarch *gdbarch,
			struct value *function,
			struct regcache *regcache,
			CORE_ADDR bp_addr,
			int nargs,
			struct value **args,
			CORE_ADDR sp,
			int struct_return,
			CORE_ADDR struct_addr)
{
  int i;
  int size, onstack_size;
  char *buf = (char *) alloca (16);
  CORE_ADDR ra, ps;
  struct argument_info
  {
    const bfd_byte *contents;
    int length;
    int onstack;		/* onstack == 0 => in reg */
    int align;			/* alignment */
    union
    {
      int offset;		/* stack offset if on stack */
      int regno;		/* regno if in register */
    } u;
  };

  struct argument_info *arg_info =
    (struct argument_info *) alloca (nargs * sizeof (struct argument_info));

  CORE_ADDR osp = sp;

  DEBUGTRACE ("xtensa_push_dummy_call (...)\n");

  if (xtensa_debug_level > 3)
    {
      int i;
      DEBUGINFO ("[xtensa_push_dummy_call] nargs = %d\n", nargs);
      DEBUGINFO ("[xtensa_push_dummy_call] sp=0x%x, struct_return=%d, "
		 "struct_addr=0x%x\n",
		 (int) sp, (int) struct_return, (int) struct_addr);

      for (i = 0; i < nargs; i++)
        {
	  struct value *arg = args[i];
	  struct type *arg_type = check_typedef (value_type (arg));
	  fprintf_unfiltered (gdb_stdlog, "%2d: 0x%08x %3d ",
			      i, (int) arg, TYPE_LENGTH (arg_type));
	  switch (TYPE_CODE (arg_type))
	    {
	    case TYPE_CODE_INT:
	      fprintf_unfiltered (gdb_stdlog, "int");
	      break;
	    case TYPE_CODE_STRUCT:
	      fprintf_unfiltered (gdb_stdlog, "struct");
	      break;
	    default:
	      fprintf_unfiltered (gdb_stdlog, "%3d", TYPE_CODE (arg_type));
	      break;
	    }
	  fprintf_unfiltered (gdb_stdlog, " 0x%08x\n",
			      (unsigned int) value_contents (arg));
	}
    }

  /* First loop: collect information.
     Cast into type_long.  (This shouldn't happen often for C because
     GDB already does this earlier.)  It's possible that GDB could
     do it all the time but it's harmless to leave this code here.  */

  size = 0;
  onstack_size = 0;
  i = 0;

  if (struct_return)
    size = REGISTER_SIZE;

  for (i = 0; i < nargs; i++)
    {
      struct argument_info *info = &arg_info[i];
      struct value *arg = args[i];
      struct type *arg_type = check_typedef (value_type (arg));

      switch (TYPE_CODE (arg_type))
	{
	case TYPE_CODE_INT:
	case TYPE_CODE_BOOL:
	case TYPE_CODE_CHAR:
	case TYPE_CODE_RANGE:
	case TYPE_CODE_ENUM:

	  /* Cast argument to long if necessary as the mask does it too.  */
	  if (TYPE_LENGTH (arg_type) < TYPE_LENGTH (builtin_type_long))
	    {
	      arg_type = builtin_type_long;
	      arg = value_cast (arg_type, arg);
	    }
	  info->align = TYPE_LENGTH (builtin_type_long);
	  break;

	case TYPE_CODE_FLT:

	  /* Align doubles correctly.  */
	  if (TYPE_LENGTH (arg_type) == TYPE_LENGTH (builtin_type_double))
	    info->align = TYPE_LENGTH (builtin_type_double);
	  else
	    info->align = TYPE_LENGTH (builtin_type_long);
	  break;

	case TYPE_CODE_STRUCT:
	default:
	  info->align = TYPE_LENGTH (builtin_type_long);
	  break;
	}
      info->length = TYPE_LENGTH (arg_type);
      info->contents = value_contents (arg);

      /* Align size and onstack_size.  */
      size = (size + info->align - 1) & ~(info->align - 1);
      onstack_size = (onstack_size + info->align - 1) & ~(info->align - 1);

      if (size + info->length > REGISTER_SIZE * ARGS_NUM_REGS)
	{
	  info->onstack = 1;
	  info->u.offset = onstack_size;
	  onstack_size += info->length;
	}
      else
	{
	  info->onstack = 0;
	  info->u.regno = ARGS_FIRST_REG + size / REGISTER_SIZE;
	}
      size += info->length;
    }

  /* Adjust the stack pointer and align it.  */
  sp = align_down (sp - onstack_size, SP_ALIGNMENT);

  /* Simulate MOVSP.  */
  if (sp != osp)
    {
      read_memory (osp - 16, buf, 16);
      write_memory (sp - 16, buf, 16);
    }

  /* Second Loop: Load arguments.  */

  if (struct_return)
    {
      store_unsigned_integer (buf, REGISTER_SIZE, struct_addr);
      regcache_cooked_write (regcache, ARGS_FIRST_REG, buf);
    }

  for (i = 0; i < nargs; i++)
    {
      struct argument_info *info = &arg_info[i];

      if (info->onstack)
	{
	  int n = info->length;
	  CORE_ADDR offset = sp + info->u.offset;

	  /* Odd-sized structs are aligned to the lower side of a memory
	     word in big-endian mode and require a shift.  This only
	     applies for structures smaller than one word.  */

	  if (n < REGISTER_SIZE && TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
	    offset += (REGISTER_SIZE - n);

	  write_memory (offset, info->contents, info->length);

	}
      else
	{
	  int n = info->length;
	  const bfd_byte *cp = info->contents;
	  int r = info->u.regno;

	  /* Odd-sized structs are aligned to the lower side of registers in
	     big-endian mode and require a shift.  The odd-sized leftover will
	     be at the end.  Note that this is only true for structures smaller
	     than REGISTER_SIZE; for larger odd-sized structures the excess
	     will be left-aligned in the register on both endiannesses.  */

	  if (n < REGISTER_SIZE && TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
	    {
	      ULONGEST v = extract_unsigned_integer (cp, REGISTER_SIZE);
	      v = v >> ((REGISTER_SIZE - n) * TARGET_CHAR_BIT);

	      store_unsigned_integer (buf, REGISTER_SIZE, v);
	      regcache_cooked_write (regcache, r, buf);

	      cp += REGISTER_SIZE;
	      n -= REGISTER_SIZE;
	      r++;
	    }
	  else
	    while (n > 0)
	      {
	        /* ULONGEST v = extract_unsigned_integer (cp, REGISTER_SIZE);*/
		regcache_cooked_write (regcache, r, cp);

		/* write_register (r, v); */
		cp += REGISTER_SIZE;
		n -= REGISTER_SIZE;
		r++;
	      }
	}
    }


  /* Set the return address of dummy frame to the dummy address.
     Note: The return address for the current function (in A0) is
     saved in the dummy frame, so we can savely overwrite A0 here.  */

  ra = (bp_addr & 0x3fffffff) | 0x40000000;
  regcache_raw_read (regcache, PS_REGNUM, buf);
  ps = extract_unsigned_integer (buf, 4) & ~0x00030000;
  regcache_cooked_write_unsigned (regcache, A4_REGNUM, ra);
  regcache_cooked_write_unsigned (regcache, PS_REGNUM, ps | 0x00010000);

  /* Set new stack pointer and return it.  */
  regcache_cooked_write_unsigned (regcache, A1_REGNUM, sp);
  /* Make dummy frame ID unique by adding a constant.  */
  return sp + SP_ALIGNMENT;
}


/* Return a breakpoint for the current location of PC.  We always use
   the density version if we have density instructions (regardless of the
   current instruction at PC), and use regular instructions otherwise.  */

#define BIG_BREAKPOINT { 0x00, 0x04, 0x00 }
#define LITTLE_BREAKPOINT { 0x00, 0x40, 0x00 }
#define DENSITY_BIG_BREAKPOINT { 0xd2, 0x0f }
#define DENSITY_LITTLE_BREAKPOINT { 0x2d, 0xf0 }

const unsigned char *
xtensa_breakpoint_from_pc (CORE_ADDR *pcptr, int *lenptr)
{
  static char big_breakpoint[] = BIG_BREAKPOINT;
  static char little_breakpoint[] = LITTLE_BREAKPOINT;
  static char density_big_breakpoint[] = DENSITY_BIG_BREAKPOINT;
  static char density_little_breakpoint[] = DENSITY_LITTLE_BREAKPOINT;

  DEBUGTRACE ("xtensa_breakpoint_from_pc (pc = 0x%08x)\n", (int) *pcptr);

  if (ISA_USE_DENSITY_INSTRUCTIONS)
    {
      if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
	{
	  *lenptr = sizeof (density_big_breakpoint);
	  return density_big_breakpoint;
	}
      else
	{
	  *lenptr = sizeof (density_little_breakpoint);
	  return density_little_breakpoint;
	}
    }
  else
    {
      if (TARGET_BYTE_ORDER == BFD_ENDIAN_BIG)
	{
	  *lenptr = sizeof (big_breakpoint);
	  return big_breakpoint;
	}
      else
	{
	  *lenptr = sizeof (little_breakpoint);
	  return little_breakpoint;
	}
    }
}


/* Return the pc of the first real instruction.  We assume that this
   machine uses register windows.

   If we have debug info ( line-number info, in particular ) we simply skip
   the code associated with the first function line effectively skipping
   the prologue code. It works even in cases like

   int main()
   {	int local_var = 1;
   	....
   }

   because, for this source code, both Xtensa compilers will generate two
   separate entries ( with the same line number ) in dwarf line-number
   section to make sure there is a boundary between the prologue code and
   the rest of the function.

   If there is no debug info, we need to analyze the code. */

CORE_ADDR
xtensa_skip_prologue (CORE_ADDR start_pc)
{
  DEBUGTRACE ("xtensa_skip_prologue (start_pc = 0x%08x)\n", (int) start_pc);

  if (ISA_USE_WINDOWED_REGISTERS)
    {
      unsigned char op1;
      struct symtab_and_line prologue_sal;

      op1 = read_memory_integer (start_pc, 1);
      if (!XTENSA_IS_ENTRY (op1))
	return start_pc;

      prologue_sal = find_pc_line (start_pc, 0);
      if (prologue_sal.line != 0)
	return prologue_sal.end;
      else
	return start_pc + XTENSA_ENTRY_LENGTH;
    }
  else
    {
      internal_error (__FILE__, __LINE__,
		      _("non-windowed configurations are not supported"));
      return start_pc;
    }
}


/* CONFIGURATION CHECK */

/* Verify the current configuration.  */

static void
xtensa_verify_config (struct gdbarch *gdbarch)
{
  struct ui_file *log;
  struct cleanup *cleanups;
  struct gdbarch_tdep *tdep;
  long dummy;
  char *buf;

  tdep = gdbarch_tdep (gdbarch);
  log = mem_fileopen ();
  cleanups = make_cleanup_ui_file_delete (log);

  /* Verify that we got a reasonable number of AREGS.  */
  if ((tdep->num_aregs & -tdep->num_aregs) != tdep->num_aregs)
    fprintf_unfiltered (log, "\n\tnum_aregs: Number of AR registers (%d) "
			"is not a power of two!", tdep->num_aregs);

  /* Verify that certain registers exist.  */
  if (tdep->pc_regnum == -1)
    fprintf_unfiltered (log, "\n\tpc_regnum: No PC register");
  if (tdep->ps_regnum == -1)
    fprintf_unfiltered (log, "\n\tps_regnum: No PS register");
  if (tdep->wb_regnum == -1)
    fprintf_unfiltered (log, "\n\twb_regnum: No WB register");
  if (tdep->ws_regnum == -1)
    fprintf_unfiltered (log, "\n\tws_regnum: No WS register");
  if (tdep->ar_base == -1)
    fprintf_unfiltered (log, "\n\tar_base: No AR registers");
  if (tdep->a0_base == -1)
    fprintf_unfiltered (log, "\n\ta0_base: No Ax registers");

  buf = ui_file_xstrdup (log, &dummy);
  make_cleanup (xfree, buf);
  if (strlen (buf) > 0)
    internal_error (__FILE__, __LINE__,
		    _("the following are invalid: %s"), buf);
  do_cleanups (cleanups);
}


/* Module "constructor" function.  */

static struct gdbarch *
xtensa_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  struct gdbarch_tdep *tdep;
  struct gdbarch *gdbarch;
  struct xtensa_abi_handler *abi_handler;

  DEBUGTRACE ("gdbarch_init()\n");

  /* We have to set the byte order before we call gdbarch_alloc.  */
  info.byte_order = xtensa_config_byte_order (&info);

  tdep = xtensa_config_tdep (&info);
  gdbarch = gdbarch_alloc (&info, tdep);

  /* Verify our configuration.  */
  xtensa_verify_config (gdbarch);

  /* Pseudo-Register read/write  */
  set_gdbarch_pseudo_register_read (gdbarch, xtensa_pseudo_register_read);
  set_gdbarch_pseudo_register_write (gdbarch, xtensa_pseudo_register_write);

  /* Set target information.  */
  set_gdbarch_num_regs (gdbarch, tdep->num_regs);
  set_gdbarch_num_pseudo_regs (gdbarch, tdep->num_pseudo_regs);
  set_gdbarch_sp_regnum (gdbarch, tdep->a0_base + 1);
  set_gdbarch_pc_regnum (gdbarch, tdep->pc_regnum);
  set_gdbarch_ps_regnum (gdbarch, tdep->ps_regnum);

  /* Renumber registers for known formats (stab, dwarf, and dwarf2).  */
  set_gdbarch_stab_reg_to_regnum (gdbarch, xtensa_reg_to_regnum);
  set_gdbarch_dwarf_reg_to_regnum (gdbarch, xtensa_reg_to_regnum);
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, xtensa_reg_to_regnum);

  /* We provide our own function to get register information.  */
  set_gdbarch_register_name (gdbarch, xtensa_register_name);
  set_gdbarch_register_type (gdbarch, xtensa_register_type);

  /* To call functions from GDB using dummy frame */
  set_gdbarch_push_dummy_call (gdbarch, xtensa_push_dummy_call);

  set_gdbarch_believe_pcc_promotion (gdbarch, 1);

  set_gdbarch_return_value (gdbarch, xtensa_return_value);

  /* Advance PC across any prologue instructions to reach "real" code.  */
  set_gdbarch_skip_prologue (gdbarch, xtensa_skip_prologue);

  /* Stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  /* Set breakpoints.  */
  set_gdbarch_breakpoint_from_pc (gdbarch, xtensa_breakpoint_from_pc);

  /* After breakpoint instruction or illegal instruction, pc still
     points at break instruction, so don't decrement.  */
  set_gdbarch_decr_pc_after_break (gdbarch, 0);

  /* We don't skip args.  */
  set_gdbarch_frame_args_skip (gdbarch, 0);

  set_gdbarch_unwind_pc (gdbarch, xtensa_unwind_pc);

  set_gdbarch_frame_align (gdbarch, xtensa_frame_align);

  set_gdbarch_unwind_dummy_id (gdbarch, xtensa_unwind_dummy_id);

  /* Frame handling.  */
  frame_base_set_default (gdbarch, &xtensa_frame_base);
  frame_unwind_append_sniffer (gdbarch, xtensa_frame_sniffer);

  set_gdbarch_print_insn (gdbarch, print_insn_xtensa);

  set_gdbarch_have_nonsteppable_watchpoint (gdbarch, 1);

  xtensa_add_reggroups (gdbarch);
  set_gdbarch_register_reggroup_p (gdbarch, xtensa_register_reggroup_p);

  set_gdbarch_regset_from_core_section (gdbarch,
					xtensa_regset_from_core_section);

  return gdbarch;
}


/* Dump xtensa tdep structure.  */

static void
xtensa_dump_tdep (struct gdbarch *current_gdbarch, struct ui_file *file)
{
  error (_("xtensa_dump_tdep(): not implemented"));
}


void
_initialize_xtensa_tdep (void)
{
  struct cmd_list_element *c;

  gdbarch_register (bfd_arch_xtensa, xtensa_gdbarch_init, xtensa_dump_tdep);
  xtensa_init_reggroups ();

  add_setshow_zinteger_cmd ("xtensa",
			    class_maintenance,
			    &xtensa_debug_level, _("\
Set Xtensa debugging."), _("\
Show Xtensa debugging."), _("\
When non-zero, Xtensa-specific debugging is enabled. \
Can be 1, 2, 3, or 4 indicating the level of debugging."),
			    NULL,
			    NULL,
			    &setdebuglist, &showdebuglist);
}
