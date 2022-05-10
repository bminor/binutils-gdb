/* Target-dependent code for the LoongArch architecture, for GDB.

   Copyright (C) 2022 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "arch-utils.h"
#include "dwarf2/frame.h"
#include "elf-bfd.h"
#include "frame-unwind.h"
#include "gdbcore.h"
#include "loongarch-tdep.h"
#include "target.h"
#include "target-descriptions.h"
#include "trad-frame.h"
#include "user-regs.h"

/* Fetch the instruction at PC.  */

static insn_t
loongarch_fetch_instruction (CORE_ADDR pc)
{
  size_t insn_len = loongarch_insn_length (0);
  gdb_byte buf[insn_len];
  int err;

  err = target_read_memory (pc, buf, insn_len);
  if (err)
    memory_error (TARGET_XFER_E_IO, pc);

  return extract_unsigned_integer (buf, insn_len, BFD_ENDIAN_LITTLE);
}

/* Return TRUE if INSN is a branch instruction, otherwise return FALSE.  */

static bool
loongarch_insn_is_branch (insn_t insn)
{
  if ((insn & 0xfc000000) == 0x4c000000       /* jirl	rd, rj, offs16  */
      || (insn & 0xfc000000) == 0x50000000    /* b	offs26  */
      || (insn & 0xfc000000) == 0x54000000    /* bl	offs26  */
      || (insn & 0xfc000000) == 0x58000000    /* beq	rj, rd, offs16  */
      || (insn & 0xfc000000) == 0x5c000000    /* bne	rj, rd, offs16  */
      || (insn & 0xfc000000) == 0x60000000    /* blt	rj, rd, offs16  */
      || (insn & 0xfc000000) == 0x64000000    /* bge	rj, rd, offs16  */
      || (insn & 0xfc000000) == 0x68000000    /* bltu	rj, rd, offs16  */
      || (insn & 0xfc000000) == 0x6c000000    /* bgeu	rj, rd, offs16  */
      || (insn & 0xfc000000) == 0x40000000    /* beqz	rj, offs21  */
      || (insn & 0xfc000000) == 0x44000000)   /* bnez	rj, offs21  */
    return true;
  return false;
}

/* Analyze the function prologue from START_PC to LIMIT_PC.
   Return the address of the first instruction past the prologue.  */

static CORE_ADDR
loongarch_scan_prologue (struct gdbarch *gdbarch, CORE_ADDR start_pc,
			 CORE_ADDR limit_pc, struct frame_info *this_frame,
			 struct trad_frame_cache *this_cache)
{
  CORE_ADDR cur_pc = start_pc, prologue_end = 0;
  loongarch_gdbarch_tdep *tdep = (loongarch_gdbarch_tdep *) gdbarch_tdep (gdbarch);
  auto regs = tdep->regs;
  int32_t sp = regs.r + 3;
  int32_t fp = regs.r + 22;
  int32_t reg_value[32] = {0};
  int32_t reg_used[32] = {1, 0};

  while (cur_pc < limit_pc)
    {
      insn_t insn = loongarch_fetch_instruction (cur_pc);
      size_t insn_len = loongarch_insn_length (insn);
      int32_t rd = loongarch_decode_imm ("0:5", insn, 0);
      int32_t rj = loongarch_decode_imm ("5:5", insn, 0);
      int32_t rk = loongarch_decode_imm ("10:5", insn, 0);
      int32_t si12 = loongarch_decode_imm ("10:12", insn, 1);
      int32_t si20 = loongarch_decode_imm ("5:20", insn, 1);

      if ((insn & 0xffc00000) == 0x02c00000	/* addi.d sp,sp,si12  */
	  && rd == sp && rj == sp && si12 < 0)
	{
	  prologue_end = cur_pc + insn_len;
	}
      else if ((insn & 0xffc00000) == 0x02c00000 /* addi.d fp,sp,si12  */
	       && rd == fp && rj == sp && si12 > 0)
	{
	  prologue_end = cur_pc + insn_len;
	}
      else if ((insn & 0xffc00000) == 0x29c00000 /* st.d rd,sp,si12  */
	       && rj == sp)
	{
	  prologue_end = cur_pc + insn_len;
	}
      else if ((insn & 0xff000000) == 0x27000000 /* stptr.d rd,sp,si14  */
	       && rj == sp)
	{
	  prologue_end = cur_pc + insn_len;
	}
      else if ((insn & 0xfe000000) == 0x14000000) /* lu12i.w rd,si20  */
	{
	  reg_value[rd] = si20 << 12;
	  reg_used[rd] = 1;
	}
      else if ((insn & 0xffc00000) == 0x03800000) /* ori rd,rj,si12  */
	{
	  if (reg_used[rj])
	  {
	    reg_value[rd] = reg_value[rj] | (si12 & 0xfff);
	    reg_used[rd] = 1;
	  }
	}
      else if ((insn & 0xffff8000) == 0x00108000 /* add.d sp,sp,rk  */
	       && rd == sp && rj == sp)
	{
	  if (reg_used[rk] == 1 && reg_value[rk] < 0)
	    {
	      prologue_end = cur_pc + insn_len;
	      break;
	    }
	}
      else if (loongarch_insn_is_branch (insn))
	{
	  break;
	}

      cur_pc += insn_len;
    }

  if (prologue_end == 0)
    prologue_end = cur_pc;

  return prologue_end;
}

/* Implement the loongarch_skip_prologue gdbarch method.  */

static CORE_ADDR
loongarch_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  CORE_ADDR func_addr;

  /* See if we can determine the end of the prologue via the symbol table.
     If so, then return either PC, or the PC after the prologue, whichever
     is greater.  */
  if (find_pc_partial_function (pc, nullptr, &func_addr, nullptr))
    {
      CORE_ADDR post_prologue_pc
	= skip_prologue_using_sal (gdbarch, func_addr);
      if (post_prologue_pc != 0)
	return std::max (pc, post_prologue_pc);
    }

  /* Can't determine prologue from the symbol table, need to examine
     instructions.  */

  /* Find an upper limit on the function prologue using the debug
     information.  If the debug information could not be used to provide
     that bound, then use an arbitrary large number as the upper bound.  */
  CORE_ADDR limit_pc = skip_prologue_using_sal (gdbarch, pc);
  if (limit_pc == 0)
    limit_pc = pc + 100;	/* Arbitrary large number.  */

  return loongarch_scan_prologue (gdbarch, pc, limit_pc, nullptr, nullptr);
}

/* Adjust the address downward (direction of stack growth) so that it
   is correctly aligned for a new stack frame.  */

static CORE_ADDR
loongarch_frame_align (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  return align_down (addr, 16);
}

/* Generate, or return the cached frame cache for LoongArch frame unwinder.  */

static struct trad_frame_cache *
loongarch_frame_cache (struct frame_info *this_frame, void **this_cache)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  struct trad_frame_cache *cache;
  CORE_ADDR pc;

  if (*this_cache != nullptr)
    return (struct trad_frame_cache *) *this_cache;

  cache = trad_frame_cache_zalloc (this_frame);
  *this_cache = cache;

  loongarch_gdbarch_tdep *tdep = (loongarch_gdbarch_tdep *) gdbarch_tdep (gdbarch);
  trad_frame_set_reg_realreg (cache, gdbarch_pc_regnum (gdbarch), tdep->regs.ra);

  pc = get_frame_address_in_block (this_frame);
  trad_frame_set_id (cache, frame_id_build_unavailable_stack (pc));

  return cache;
}

/* Implement the this_id callback for LoongArch frame unwinder.  */

static void
loongarch_frame_this_id (struct frame_info *this_frame, void **prologue_cache,
			 struct frame_id *this_id)
{
  struct trad_frame_cache *info;

  info = loongarch_frame_cache (this_frame, prologue_cache);
  trad_frame_get_id (info, this_id);
}

/* Implement the prev_register callback for LoongArch frame unwinder.  */

static struct value *
loongarch_frame_prev_register (struct frame_info *this_frame,
			       void **prologue_cache, int regnum)
{
  struct trad_frame_cache *info;

  info = loongarch_frame_cache (this_frame, prologue_cache);
  return trad_frame_get_register (info, this_frame, regnum);
}

static const struct frame_unwind loongarch_frame_unwind = {
  "loongarch prologue",
  /*.type	   =*/NORMAL_FRAME,
  /*.stop_reason   =*/default_frame_unwind_stop_reason,
  /*.this_id	   =*/loongarch_frame_this_id,
  /*.prev_register =*/loongarch_frame_prev_register,
  /*.unwind_data   =*/nullptr,
  /*.sniffer	   =*/default_frame_sniffer,
  /*.dealloc_cache =*/nullptr,
  /*.prev_arch	   =*/nullptr,
};

/* Implement the return_value gdbarch method for LoongArch.  */

static enum return_value_convention
loongarch_return_value (struct gdbarch *gdbarch, struct value *function,
			struct type *type, struct regcache *regcache,
			gdb_byte *readbuf, const gdb_byte *writebuf)
{
  loongarch_gdbarch_tdep *tdep = (loongarch_gdbarch_tdep *) gdbarch_tdep (gdbarch);
  auto regs = tdep->regs;
  int len = TYPE_LENGTH (type);
  int regnum = -1;

  /* See if our value is returned through a register.  If it is, then
     store the associated register number in REGNUM.  */
  switch (type->code ())
    {
      case TYPE_CODE_INT:
	regnum = regs.r + 4;
	break;
    }

  /* Extract the return value from the register where it was stored.  */
  if (readbuf != nullptr)
    regcache->raw_read_part (regnum, 0, len, readbuf);
  if (writebuf != nullptr)
    regcache->raw_write_part (regnum, 0, len, writebuf);

  return RETURN_VALUE_REGISTER_CONVENTION;
}

/* Implement the "dwarf2_reg_to_regnum" gdbarch method.  */

static int
loongarch_dwarf2_reg_to_regnum (struct gdbarch *gdbarch, int num)
{
  loongarch_gdbarch_tdep *tdep = (loongarch_gdbarch_tdep *) gdbarch_tdep (gdbarch);
  auto regs = tdep->regs;

  if (0 <= num && num < 32)
    return regs.r + num;
  else
    return -1;
}

static constexpr gdb_byte loongarch_default_breakpoint[] = {0x05, 0x00, 0x2a, 0x00};
typedef BP_MANIPULATION (loongarch_default_breakpoint) loongarch_breakpoint;

/* Extract a set of required target features out of ABFD.  If ABFD is nullptr
   then a LOONGARCH_GDBARCH_FEATURES is returned in its default state.  */

static struct loongarch_gdbarch_features
loongarch_features_from_bfd (const bfd *abfd)
{
  struct loongarch_gdbarch_features features;

  /* Now try to improve on the defaults by looking at the binary we are
     going to execute.  We assume the user knows what they are doing and
     that the target will match the binary.  Remember, this code path is
     only used at all if the target hasn't given us a description, so this
     is really a last ditched effort to do something sane before giving
     up.  */
  if (abfd != nullptr && bfd_get_flavour (abfd) == bfd_target_elf_flavour)
    {
      unsigned char eclass = elf_elfheader (abfd)->e_ident[EI_CLASS];

      if (eclass == ELFCLASS32)
	features.xlen = 4;
      else if (eclass == ELFCLASS64)
	features.xlen = 8;
      else
	internal_error (__FILE__, __LINE__,
			_("unknown ELF header class %d"), eclass);
    }

  return features;
}

/* Find a suitable default target description.  Use the contents of INFO,
   specifically the bfd object being executed, to guide the selection of a
   suitable default target description.  */

static const struct target_desc *
loongarch_find_default_target_description (const struct gdbarch_info info)
{
  /* Extract desired feature set from INFO.  */
  struct loongarch_gdbarch_features features
    = loongarch_features_from_bfd (info.abfd);

  /* If the XLEN field is still 0 then we got nothing useful from INFO.BFD,
     maybe there was no bfd object.  In this case we fall back to a minimal
     useful target with no floating point, the x-register size is selected
     based on the architecture from INFO.  */
  if (features.xlen == 0)
    features.xlen = info.bfd_arch_info->bits_per_address == 32 ? 4 : 8;

  /* Now build a target description based on the feature set.  */
  return loongarch_lookup_target_description (features);
}

/* Initialize the current architecture based on INFO  */

static struct gdbarch *
loongarch_gdbarch_init (struct gdbarch_info info, struct gdbarch_list *arches)
{
  const struct target_desc *tdesc = info.target_desc;

  /* Ensure we always have a target description.  */
  if (!tdesc_has_registers (tdesc))
    tdesc = loongarch_find_default_target_description (info);

  const struct tdesc_feature *feature_cpu
    = tdesc_find_feature (tdesc, "org.gnu.gdb.loongarch.base");
  if (feature_cpu == nullptr)
    return nullptr;

  int xlen_bitsize = tdesc_register_bitsize (feature_cpu, "pc");
  struct loongarch_gdbarch_features features;
  features.xlen = (xlen_bitsize / 8);

  size_t regnum = 0;
  tdesc_arch_data_up tdesc_data = tdesc_data_alloc ();
  loongarch_gdbarch_tdep *tdep = new loongarch_gdbarch_tdep;
  tdep->regs.r = regnum;

  /* Validate the description provides the mandatory base registers
     and allocate their numbers.  */
  bool valid_p = true;
  for (int i = 0; i < 32; i++)
    valid_p &= tdesc_numbered_register (feature_cpu, tdesc_data.get (), regnum++,
					loongarch_r_normal_name[i] + 1);
  valid_p &= tdesc_numbered_register (feature_cpu, tdesc_data.get (),
				      tdep->regs.pc = regnum++, "pc");
  valid_p &= tdesc_numbered_register (feature_cpu, tdesc_data.get (),
				      tdep->regs.badv = regnum++, "badv");
  if (!valid_p)
    return nullptr;

  /* LoongArch code is always little-endian.  */
  info.byte_order_for_code = BFD_ENDIAN_LITTLE;

  /* Have a look at what the supplied (if any) bfd object requires of the
     target, then check that this matches with what the target is
     providing.  */
  struct loongarch_gdbarch_features abi_features
    = loongarch_features_from_bfd (info.abfd);

  /* If the ABI_FEATURES xlen is 0 then this indicates we got no useful abi
     features from the INFO object.  In this case we just treat the
     hardware features as defining the abi.  */
  if (abi_features.xlen == 0)
    abi_features = features;

  /* Find a candidate among the list of pre-declared architectures.  */
  for (arches = gdbarch_list_lookup_by_info (arches, &info);
       arches != nullptr;
       arches = gdbarch_list_lookup_by_info (arches->next, &info))
    {
      /* Check that the feature set of the ARCHES matches the feature set
	 we are looking for.  If it doesn't then we can't reuse this
	 gdbarch.  */
      loongarch_gdbarch_tdep *candidate_tdep
	= (loongarch_gdbarch_tdep *) gdbarch_tdep (arches->gdbarch);

      if (candidate_tdep->abi_features != abi_features)
	continue;

      break;
    }

  if (arches != nullptr)
    return arches->gdbarch;

  /* None found, so create a new architecture from the information provided.  */
  struct gdbarch *gdbarch = gdbarch_alloc (&info, tdep);
  tdep->abi_features = abi_features;

  /* Target data types.  */
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_long_bit (gdbarch, info.bfd_arch_info->bits_per_address);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 128);
  set_gdbarch_long_double_format (gdbarch, floatformats_ieee_quad);
  set_gdbarch_ptr_bit (gdbarch, info.bfd_arch_info->bits_per_address);
  set_gdbarch_char_signed (gdbarch, 0);

  info.target_desc = tdesc;
  info.tdesc_data = tdesc_data.get ();

  /* Information about registers.  */
  tdep->regs.ra = tdep->regs.r + 1;
  tdep->regs.sp = tdep->regs.r + 3;
  set_gdbarch_num_regs (gdbarch, regnum);
  set_gdbarch_sp_regnum (gdbarch, tdep->regs.sp);
  set_gdbarch_pc_regnum (gdbarch, tdep->regs.pc);

  /* Finalise the target description registers.  */
  tdesc_use_registers (gdbarch, tdesc, std::move (tdesc_data));

  /* Return value info  */
  set_gdbarch_return_value (gdbarch, loongarch_return_value);

  /* Advance PC across function entry code.  */
  set_gdbarch_skip_prologue (gdbarch, loongarch_skip_prologue);

  /* Stack grows downward.  */
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);

  /* Frame info.  */
  set_gdbarch_frame_align (gdbarch, loongarch_frame_align);

  /* Breakpoint manipulation.  */
  set_gdbarch_breakpoint_kind_from_pc (gdbarch, loongarch_breakpoint::kind_from_pc);
  set_gdbarch_sw_breakpoint_from_kind (gdbarch, loongarch_breakpoint::bp_from_kind);

  /* Frame unwinders. Use DWARF debug info if available, otherwise use our own unwinder.  */
  set_gdbarch_dwarf2_reg_to_regnum (gdbarch, loongarch_dwarf2_reg_to_regnum);
  dwarf2_append_unwinders (gdbarch);
  frame_unwind_append_unwinder (gdbarch, &loongarch_frame_unwind);

  /* Hook in OS ABI-specific overrides, if they have been registered.  */
  gdbarch_init_osabi (info, gdbarch);

  return gdbarch;
}

void _initialize_loongarch_tdep ();
void
_initialize_loongarch_tdep ()
{
  gdbarch_register (bfd_arch_loongarch, loongarch_gdbarch_init, nullptr);
}
