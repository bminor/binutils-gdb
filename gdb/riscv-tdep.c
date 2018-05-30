/* Target-dependent code for the RISC-V architecture, for GDB.

   Copyright (C) 2018 Free Software Foundation, Inc.

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
#include "frame.h"
#include "inferior.h"
#include "symtab.h"
#include "value.h"
#include "gdbcmd.h"
#include "language.h"
#include "gdbcore.h"
#include "symfile.h"
#include "objfiles.h"
#include "gdbtypes.h"
#include "target.h"
#include "arch-utils.h"
#include "regcache.h"
#include "osabi.h"
#include "riscv-tdep.h"
#include "block.h"
#include "reggroups.h"
#include "opcode/riscv.h"
#include "elf/riscv.h"
#include "elf-bfd.h"
#include "symcat.h"
#include "dis-asm.h"
#include "frame-unwind.h"
#include "frame-base.h"
#include "trad-frame.h"
#include "infcall.h"
#include "floatformat.h"
#include "remote.h"
#include "target-descriptions.h"
#include "dwarf2-frame.h"
#include "user-regs.h"
#include "valprint.h"
#include "common-defs.h"
#include "opcode/riscv-opc.h"
#include "cli/cli-decode.h"
#include "observable.h"

/* The stack must be 16-byte aligned.  */
#define SP_ALIGNMENT 16

/* Forward declarations.  */
static bool riscv_has_feature (struct gdbarch *gdbarch, char feature);
struct riscv_inferior_data;
struct riscv_inferior_data * riscv_inferior_data (struct inferior *const inf);

/* Define a series of is_XXX_insn functions to check if the value INSN
   is an instance of instruction XXX.  */
#define DECLARE_INSN(INSN_NAME, INSN_MATCH, INSN_MASK) \
static inline bool is_ ## INSN_NAME ## _insn (long insn) \
{ \
  return (insn & INSN_MASK) == INSN_MATCH; \
}
#include "opcode/riscv-opc.h"
#undef DECLARE_INSN

/* Per inferior information for RiscV.  */

struct riscv_inferior_data
{
  /* True when MISA_VALUE is valid, otherwise false.  */
  bool misa_read;

  /* If MISA_READ is true then MISA_VALUE holds the value of the MISA
     register read from the target.  */
  uint32_t misa_value;
};

/* Key created when the RiscV per-inferior data is registered.  */

static const struct inferior_data *riscv_inferior_data_reg;

/* Architectural name for core registers.  */

static const char * const riscv_gdb_reg_names[RISCV_LAST_FP_REGNUM + 1] =
{
  "x0",  "x1",  "x2",  "x3",  "x4",  "x5",  "x6",  "x7",
  "x8",  "x9",  "x10", "x11", "x12", "x13", "x14", "x15",
  "x16", "x17", "x18", "x19", "x20", "x21", "x22", "x23",
  "x24", "x25", "x26", "x27", "x28", "x29", "x30", "x31",
  "pc",
  "f0",  "f1",  "f2",  "f3",  "f4",  "f5",  "f6",  "f7",
  "f8",  "f9",  "f10", "f11", "f12", "f13", "f14", "f15",
  "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
  "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",
};

/* Maps "pretty" register names onto their GDB register number.  */

struct register_alias
{
  /* The register alias.  Usually more descriptive than the
     architectural name of the register.  */
  const char *name;

  /* The GDB register number.  */
  int regnum;
};

/* Table of register aliases.  */

static const struct register_alias riscv_register_aliases[] =
{
  { "zero", 0 },
  { "ra", 1 },
  { "sp", 2 },
  { "gp", 3 },
  { "tp", 4 },
  { "t0", 5 },
  { "t1", 6 },
  { "t2", 7 },
  { "fp", 8 },
  { "s0", 8 },
  { "s1", 9 },
  { "a0", 10 },
  { "a1", 11 },
  { "a2", 12 },
  { "a3", 13 },
  { "a4", 14 },
  { "a5", 15 },
  { "a6", 16 },
  { "a7", 17 },
  { "s2", 18 },
  { "s3", 19 },
  { "s4", 20 },
  { "s5", 21 },
  { "s6", 22 },
  { "s7", 23 },
  { "s8", 24 },
  { "s9", 25 },
  { "s10", 26 },
  { "s11", 27 },
  { "t3", 28 },
  { "t4", 29 },
  { "t5", 30 },
  { "t6", 31 },
  /* pc is 32.  */
  { "ft0", 33 },
  { "ft1", 34 },
  { "ft2", 35 },
  { "ft3", 36 },
  { "ft4", 37 },
  { "ft5", 38 },
  { "ft6", 39 },
  { "ft7", 40 },
  { "fs0", 41 },
  { "fs1", 42 },
  { "fa0", 43 },
  { "fa1", 44 },
  { "fa2", 45 },
  { "fa3", 46 },
  { "fa4", 47 },
  { "fa5", 48 },
  { "fa6", 49 },
  { "fa7", 50 },
  { "fs2", 51 },
  { "fs3", 52 },
  { "fs4", 53 },
  { "fs5", 54 },
  { "fs6", 55 },
  { "fs7", 56 },
  { "fs8", 57 },
  { "fs9", 58 },
  { "fs10", 59 },
  { "fs11", 60 },
  { "ft8", 61 },
  { "ft9", 62 },
  { "ft10", 63 },
  { "ft11", 64 },
#define DECLARE_CSR(name, num) { #name, (num) + 65 },
#include "opcode/riscv-opc.h"
#undef DECLARE_CSR
};

/* Controls whether we place compressed breakpoints or not.  When in auto
   mode GDB tries to determine if the target supports compressed
   breakpoints, and uses them if it does.  */

static enum auto_boolean use_compressed_breakpoints;

/* The show callback for 'show riscv use-compressed-breakpoints'.  */

static void
show_use_compressed_breakpoints (struct ui_file *file, int from_tty,
				 struct cmd_list_element *c,
				 const char *value)
{
  const char *additional_info;
  struct gdbarch *gdbarch = target_gdbarch ();

  if (use_compressed_breakpoints == AUTO_BOOLEAN_AUTO)
    if (riscv_has_feature (gdbarch, 'C'))
      additional_info = _(" (currently on)");
    else
      additional_info = _(" (currently off)");
  else
    additional_info = "";

  fprintf_filtered (file,
		    _("Debugger's use of compressed breakpoints is set "
		      "to %s%s.\n"), value, additional_info);
}

/* The set and show lists for 'set riscv' and 'show riscv' prefixes.  */

static struct cmd_list_element *setriscvcmdlist = NULL;
static struct cmd_list_element *showriscvcmdlist = NULL;

/* The show callback for the 'show riscv' prefix command.  */

static void
show_riscv_command (const char *args, int from_tty)
{
  help_list (showriscvcmdlist, "show riscv ", all_commands, gdb_stdout);
}

/* The set callback for the 'set riscv' prefix command.  */

static void
set_riscv_command (const char *args, int from_tty)
{
  printf_unfiltered
    (_("\"set riscv\" must be followed by an appropriate subcommand.\n"));
  help_list (setriscvcmdlist, "set riscv ", all_commands, gdb_stdout);
}

/* The set and show lists for 'set riscv' and 'show riscv' prefixes.  */

static struct cmd_list_element *setdebugriscvcmdlist = NULL;
static struct cmd_list_element *showdebugriscvcmdlist = NULL;

/* The show callback for the 'show debug riscv' prefix command.  */

static void
show_debug_riscv_command (const char *args, int from_tty)
{
  help_list (showdebugriscvcmdlist, "show debug riscv ", all_commands, gdb_stdout);
}

/* The set callback for the 'set debug riscv' prefix command.  */

static void
set_debug_riscv_command (const char *args, int from_tty)
{
  printf_unfiltered
    (_("\"set debug riscv\" must be followed by an appropriate subcommand.\n"));
  help_list (setdebugriscvcmdlist, "set debug riscv ", all_commands, gdb_stdout);
}

/* The show callback for all 'show debug riscv VARNAME' variables.  */

static void
show_riscv_debug_variable (struct ui_file *file, int from_tty,
			   struct cmd_list_element *c,
			   const char *value)
{
  fprintf_filtered (file,
		    _("RiscV debug variable `%s' is set to: %s\n"),
		    c->name, value);
}

/* When this is set to non-zero debugging information about inferior calls
   will be printed.  */

static unsigned int riscv_debug_infcall = 0;

/* Read the MISA register from the target.  The register will only be read
   once, and the value read will be cached.  If the register can't be read
   from the target then a default value (0) will be returned.  If the
   pointer READ_P is not null, then the bool pointed to is updated  to
   indicate if the value returned was read from the target (true) or is the
   default (false).  */

static uint32_t
riscv_read_misa_reg (bool *read_p)
{
  struct riscv_inferior_data *inf_data
    = riscv_inferior_data (current_inferior ());

  if (!inf_data->misa_read && target_has_registers)
    {
      uint32_t value = 0;
      struct frame_info *frame = get_current_frame ();

      TRY
	{
	  value = get_frame_register_unsigned (frame, RISCV_CSR_MISA_REGNUM);
	}
      CATCH (ex, RETURN_MASK_ERROR)
	{
	  /* Old cores might have MISA located at a different offset.  */
	  value = get_frame_register_unsigned (frame,
					       RISCV_CSR_LEGACY_MISA_REGNUM);
	}
      END_CATCH

      inf_data->misa_read = true;
      inf_data->misa_value = value;
    }

  if (read_p != nullptr)
    *read_p = inf_data->misa_read;

  return inf_data->misa_value;
}

/* Return true if FEATURE is available for the architecture GDBARCH.  The
   FEATURE should be one of the single character feature codes described in
   the RiscV ISA manual, these are between 'A' and 'Z'.  */

static bool
riscv_has_feature (struct gdbarch *gdbarch, char feature)
{
  bool have_read_misa = false;
  uint32_t misa;

  gdb_assert (feature >= 'A' && feature <= 'Z');

  /* It would be nice to always check with the real target where possible,
     however, for compressed instructions this is a bad idea.

     The call to `set_gdbarch_decr_pc_after_break' is made just once per
     GDBARCH and we decide at that point if we should decrement by 2 or 4
     bytes based on whether the BFD has compressed instruction support or
     not.

     If the BFD was not compiled with compressed instruction support, but we
     are running on a target with compressed instructions then we might
     place a 4-byte breakpoint, then decrement the $pc by 2 bytes leading to
     confusion.

     It's safer if we just make decisions about compressed instruction
     support based on the BFD.  */
  if (feature != 'C')
    misa = riscv_read_misa_reg (&have_read_misa);
  if (!have_read_misa || misa == 0)
    misa = gdbarch_tdep (gdbarch)->core_features;

  return (misa & (1 << (feature - 'A'))) != 0;
}

/* Return the width in bytes  of the general purpose registers for GDBARCH.
   Possible return values are 4, 8, or 16 for RiscV variants RV32, RV64, or
   RV128.  */

static int
riscv_isa_xlen (struct gdbarch *gdbarch)
{
  switch (gdbarch_tdep (gdbarch)->abi.fields.base_len)
    {
    default:
      warning (_("unknown xlen size, assuming 4 bytes"));
      /* Fall through.  */
    case 1:
      return 4;
    case 2:
      return 8;
    case 3:
      return 16;
    }
}

/* Return the width in bytes of the floating point registers for GDBARCH.
   If this architecture has no floating point registers, then return 0.
   Possible values are 4, 8, or 16 for depending on which of single, double
   or quad floating point support is available.  */

static int
riscv_isa_flen (struct gdbarch *gdbarch)
{
  if (riscv_has_feature (gdbarch, 'Q'))
    return 16;
  else if (riscv_has_feature (gdbarch, 'D'))
    return 8;
  else if (riscv_has_feature (gdbarch, 'F'))
    return 4;

  return 0;
}

/* Return true if the target for GDBARCH has floating point hardware.  */

static bool
riscv_has_fp_regs (struct gdbarch *gdbarch)
{
  return (riscv_isa_flen (gdbarch) > 0);
}

/* Return true if GDBARCH is using any of the floating point hardware ABIs.  */

static bool
riscv_has_fp_abi (struct gdbarch *gdbarch)
{
  return (gdbarch_tdep (gdbarch)->abi.fields.float_abi != 0);
}

/* Implement the breakpoint_kind_from_pc gdbarch method.  */

static int
riscv_breakpoint_kind_from_pc (struct gdbarch *gdbarch, CORE_ADDR *pcptr)
{
  if (use_compressed_breakpoints == AUTO_BOOLEAN_AUTO)
    {
      if (riscv_has_feature (gdbarch, 'C'))
	return 2;
      else
	return 4;
    }
  else if (use_compressed_breakpoints == AUTO_BOOLEAN_TRUE)
    return 2;
  else
    return 4;
}

/* Implement the sw_breakpoint_from_kind gdbarch method.  */

static const gdb_byte *
riscv_sw_breakpoint_from_kind (struct gdbarch *gdbarch, int kind, int *size)
{
  static const gdb_byte ebreak[] = { 0x73, 0x00, 0x10, 0x00, };
  static const gdb_byte c_ebreak[] = { 0x02, 0x90 };

  *size = kind;
  switch (kind)
    {
    case 2:
      return c_ebreak;
    case 4:
      return ebreak;
    default:
      gdb_assert_not_reached (_("unhandled breakpoint kind"));
    }
}

/* Callback function for user_reg_add.  */

static struct value *
value_of_riscv_user_reg (struct frame_info *frame, const void *baton)
{
  const int *reg_p = (const int *) baton;
  return value_of_register (*reg_p, frame);
}

/* Implement the register_name gdbarch method.  */

static const char *
riscv_register_name (struct gdbarch *gdbarch, int regnum)
{
  /* Prefer to use the alias. */
  if (regnum >= RISCV_ZERO_REGNUM && regnum <= RISCV_LAST_REGNUM)
    {
      int i;

      for (i = 0; i < ARRAY_SIZE (riscv_register_aliases); ++i)
	if (regnum == riscv_register_aliases[i].regnum)
	  return riscv_register_aliases[i].name;
    }

  if (regnum >= RISCV_ZERO_REGNUM && regnum <= RISCV_LAST_FP_REGNUM)
    return riscv_gdb_reg_names[regnum];

  if (regnum >= RISCV_FIRST_CSR_REGNUM && regnum <= RISCV_LAST_CSR_REGNUM)
    {
      static char buf[20];

      xsnprintf (buf, sizeof (buf), "csr%d",
		 regnum - RISCV_FIRST_CSR_REGNUM);
      return buf;
    }

  if (regnum == RISCV_PRIV_REGNUM)
    return "priv";

  return NULL;
}

/* Implement the register_type gdbarch method.  */

static struct type *
riscv_register_type (struct gdbarch *gdbarch, int regnum)
{
  int regsize;

  if (regnum < RISCV_FIRST_FP_REGNUM)
    {
      if (regnum == gdbarch_pc_regnum (gdbarch)
	  || regnum == RISCV_RA_REGNUM)
	return builtin_type (gdbarch)->builtin_func_ptr;

      if (regnum == RISCV_FP_REGNUM
	  || regnum == RISCV_SP_REGNUM
	  || regnum == RISCV_GP_REGNUM
	  || regnum == RISCV_TP_REGNUM)
	return builtin_type (gdbarch)->builtin_data_ptr;

      /* Remaining GPRs vary in size based on the current ISA.  */
      regsize = riscv_isa_xlen (gdbarch);
      switch (regsize)
	{
	case 4:
	  return builtin_type (gdbarch)->builtin_uint32;
	case 8:
	  return builtin_type (gdbarch)->builtin_uint64;
	case 16:
	  return builtin_type (gdbarch)->builtin_uint128;
	default:
	  internal_error (__FILE__, __LINE__,
			  _("unknown isa regsize %i"), regsize);
	}
    }
  else if (regnum <= RISCV_LAST_FP_REGNUM)
    {
      regsize = riscv_isa_xlen (gdbarch);
      switch (regsize)
	{
	case 4:
	  return builtin_type (gdbarch)->builtin_float;
	case 8:
	  return builtin_type (gdbarch)->builtin_double;
	case 16:
	  return builtin_type (gdbarch)->builtin_long_double;
	default:
	  internal_error (__FILE__, __LINE__,
			  _("unknown isa regsize %i"), regsize);
	}
    }
  else if (regnum == RISCV_PRIV_REGNUM)
    return builtin_type (gdbarch)->builtin_int8;
  else
    {
      if (regnum == RISCV_CSR_FFLAGS_REGNUM
	  || regnum == RISCV_CSR_FRM_REGNUM
	  || regnum == RISCV_CSR_FCSR_REGNUM)
	return builtin_type (gdbarch)->builtin_int32;

      regsize = riscv_isa_xlen (gdbarch);
      switch (regsize)
	{
	case 4:
	  return builtin_type (gdbarch)->builtin_int32;
	case 8:
	  return builtin_type (gdbarch)->builtin_int64;
	case 16:
	  return builtin_type (gdbarch)->builtin_int128;
	default:
	  internal_error (__FILE__, __LINE__,
			  _("unknown isa regsize %i"), regsize);
	}
    }
}

/* Helper for riscv_print_registers_info, prints info for a single register
   REGNUM.  */

static void
riscv_print_one_register_info (struct gdbarch *gdbarch,
			       struct ui_file *file,
			       struct frame_info *frame,
			       int regnum)
{
  const char *name = gdbarch_register_name (gdbarch, regnum);
  struct value *val = value_of_register (regnum, frame);
  struct type *regtype = value_type (val);
  int print_raw_format;
  enum tab_stops { value_column_1 = 15 };

  fputs_filtered (name, file);
  print_spaces_filtered (value_column_1 - strlen (name), file);

  print_raw_format = (value_entirely_available (val)
		      && !value_optimized_out (val));

  if (TYPE_CODE (regtype) == TYPE_CODE_FLT)
    {
      struct value_print_options opts;
      const gdb_byte *valaddr = value_contents_for_printing (val);
      enum bfd_endian byte_order = gdbarch_byte_order (get_type_arch (regtype));

      get_user_print_options (&opts);
      opts.deref_ref = 1;

      val_print (regtype,
		 value_embedded_offset (val), 0,
		 file, 0, val, &opts, current_language);

      if (print_raw_format)
	{
	  fprintf_filtered (file, "\t(raw ");
	  print_hex_chars (file, valaddr, TYPE_LENGTH (regtype), byte_order,
			   true);
	  fprintf_filtered (file, ")");
	}
    }
  else
    {
      struct value_print_options opts;

      /* Print the register in hex.  */
      get_formatted_print_options (&opts, 'x');
      opts.deref_ref = 1;
      val_print (regtype,
		 value_embedded_offset (val), 0,
		 file, 0, val, &opts, current_language);

      if (print_raw_format)
	{
	  if (regnum == RISCV_CSR_MSTATUS_REGNUM)
	    {
	      LONGEST d;
	      int size = register_size (gdbarch, regnum);
	      unsigned xlen;

	      d = value_as_long (val);
	      xlen = size * 4;
	      fprintf_filtered (file,
				"\tSD:%X VM:%02X MXR:%X PUM:%X MPRV:%X XS:%X "
				"FS:%X MPP:%x HPP:%X SPP:%X MPIE:%X HPIE:%X "
				"SPIE:%X UPIE:%X MIE:%X HIE:%X SIE:%X UIE:%X",
				(int) ((d >> (xlen - 1)) & 0x1),
				(int) ((d >> 24) & 0x1f),
				(int) ((d >> 19) & 0x1),
				(int) ((d >> 18) & 0x1),
				(int) ((d >> 17) & 0x1),
				(int) ((d >> 15) & 0x3),
				(int) ((d >> 13) & 0x3),
				(int) ((d >> 11) & 0x3),
				(int) ((d >> 9) & 0x3),
				(int) ((d >> 8) & 0x1),
				(int) ((d >> 7) & 0x1),
				(int) ((d >> 6) & 0x1),
				(int) ((d >> 5) & 0x1),
				(int) ((d >> 4) & 0x1),
				(int) ((d >> 3) & 0x1),
				(int) ((d >> 2) & 0x1),
				(int) ((d >> 1) & 0x1),
				(int) ((d >> 0) & 0x1));
	    }
	  else if (regnum == RISCV_CSR_MISA_REGNUM)
	    {
	      int base;
	      unsigned xlen, i;
	      LONGEST d;

	      d = value_as_long (val);
	      base = d >> 30;
	      xlen = 16;

	      for (; base > 0; base--)
		xlen *= 2;
	      fprintf_filtered (file, "\tRV%d", xlen);

	      for (i = 0; i < 26; i++)
		{
		  if (d & (1 << i))
		    fprintf_filtered (file, "%c", 'A' + i);
		}
	    }
	  else if (regnum == RISCV_CSR_FCSR_REGNUM
		   || regnum == RISCV_CSR_FFLAGS_REGNUM
		   || regnum == RISCV_CSR_FRM_REGNUM)
	    {
	      LONGEST d;

	      d = value_as_long (val);

	      fprintf_filtered (file, "\t");
	      if (regnum != RISCV_CSR_FRM_REGNUM)
		fprintf_filtered (file,
				  "RD:%01X NV:%d DZ:%d OF:%d UF:%d NX:%d",
				  (int) ((d >> 5) & 0x7),
				  (int) ((d >> 4) & 0x1),
				  (int) ((d >> 3) & 0x1),
				  (int) ((d >> 2) & 0x1),
				  (int) ((d >> 1) & 0x1),
				  (int) ((d >> 0) & 0x1));

	      if (regnum != RISCV_CSR_FFLAGS_REGNUM)
		{
		  static const char * const sfrm[] =
		    {
		      "RNE (round to nearest; ties to even)",
		      "RTZ (Round towards zero)",
		      "RDN (Round down towards -INF)",
		      "RUP (Round up towards +INF)",
		      "RMM (Round to nearest; ties to max magnitude)",
		      "INVALID[5]",
		      "INVALID[6]",
		      "dynamic rounding mode",
		    };
		  int frm = ((regnum == RISCV_CSR_FCSR_REGNUM)
			     ? (d >> 5) : d) & 0x3;

		  fprintf_filtered (file, "%sFRM:%i [%s]",
				    (regnum == RISCV_CSR_FCSR_REGNUM
				     ? " " : ""),
				    frm, sfrm[frm]);
		}
	    }
	  else if (regnum == RISCV_PRIV_REGNUM)
	    {
	      LONGEST d;
	      uint8_t priv;

	      d = value_as_long (val);
	      priv = d & 0xff;

	      if (priv < 4)
		{
		  static const char * const sprv[] =
		    {
		      "User/Application",
		      "Supervisor",
		      "Hypervisor",
		      "Machine"
		    };
		  fprintf_filtered (file, "\tprv:%d [%s]",
				    priv, sprv[priv]);
		}
	      else
		fprintf_filtered (file, "\tprv:%d [INVALID]", priv);
	    }
	  else
	    {
	      /* If not a vector register, print it also according to its
		 natural format.  */
	      if (TYPE_VECTOR (regtype) == 0)
		{
		  get_user_print_options (&opts);
		  opts.deref_ref = 1;
		  fprintf_filtered (file, "\t");
		  val_print (regtype,
			     value_embedded_offset (val), 0,
			     file, 0, val, &opts, current_language);
		}
	    }
	}
    }
  fprintf_filtered (file, "\n");
}

/* Implement the register_reggroup_p gdbarch method.  Is REGNUM a member
   of REGGROUP?  */

static int
riscv_register_reggroup_p (struct gdbarch  *gdbarch, int regnum,
			   struct reggroup *reggroup)
{
  int float_p;
  int raw_p;
  unsigned int i;

  /* Used by 'info registers' and 'info registers <groupname>'.  */

  if (gdbarch_register_name (gdbarch, regnum) == NULL
      || gdbarch_register_name (gdbarch, regnum)[0] == '\0')
    return 0;

  if (reggroup == all_reggroup)
    {
      if (regnum < RISCV_FIRST_CSR_REGNUM || regnum == RISCV_PRIV_REGNUM)
	return 1;
      /* Only include CSRs that have aliases.  */
      for (i = 0; i < ARRAY_SIZE (riscv_register_aliases); ++i)
	{
	  if (regnum == riscv_register_aliases[i].regnum)
	    return 1;
	}
      return 0;
    }
  else if (reggroup == float_reggroup)
    return ((regnum >= RISCV_FIRST_FP_REGNUM && regnum <= RISCV_LAST_FP_REGNUM)
	    || (regnum == RISCV_CSR_FCSR_REGNUM
		|| regnum == RISCV_CSR_FFLAGS_REGNUM
	        || regnum == RISCV_CSR_FRM_REGNUM));
  else if (reggroup == general_reggroup)
    return regnum < RISCV_FIRST_FP_REGNUM;
  else if (reggroup == restore_reggroup || reggroup == save_reggroup)
    {
      if (riscv_has_fp_regs (gdbarch))
	return regnum <= RISCV_LAST_FP_REGNUM;
      else
	return regnum < RISCV_FIRST_FP_REGNUM;
    }
  else if (reggroup == system_reggroup)
    {
      if (regnum == RISCV_PRIV_REGNUM)
	return 1;
      if (regnum < RISCV_FIRST_CSR_REGNUM || regnum > RISCV_LAST_CSR_REGNUM)
	return 0;
      /* Only include CSRs that have aliases.  */
      for (i = 0; i < ARRAY_SIZE (riscv_register_aliases); ++i)
	{
	  if (regnum == riscv_register_aliases[i].regnum)
	    return 1;
	}
      return 0;
    }
  else if (reggroup == vector_reggroup)
    return 0;
  else
    return 0;
}

/* Implement the print_registers_info gdbarch method.  This is used by
   'info registers' and 'info all-registers'.  */

static void
riscv_print_registers_info (struct gdbarch *gdbarch,
			    struct ui_file *file,
			    struct frame_info *frame,
			    int regnum, int print_all)
{
  if (regnum != -1)
    {
      /* Print one specified register.  */
      gdb_assert (regnum <= RISCV_LAST_REGNUM);
      if (gdbarch_register_name (gdbarch, regnum) == NULL
	  || *(gdbarch_register_name (gdbarch, regnum)) == '\0')
        error (_("Not a valid register for the current processor type"));
      riscv_print_one_register_info (gdbarch, file, frame, regnum);
    }
  else
    {
      struct reggroup *reggroup;

      if (print_all)
	reggroup = all_reggroup;
      else
	reggroup = general_reggroup;

      for (regnum = 0; regnum <= RISCV_LAST_REGNUM; ++regnum)
	{
	  /* Zero never changes, so might as well hide by default.  */
	  if (regnum == RISCV_ZERO_REGNUM && !print_all)
	    continue;

	  /* Registers with no name are not valid on this ISA.  */
	  if (gdbarch_register_name (gdbarch, regnum) == NULL
	      || *(gdbarch_register_name (gdbarch, regnum)) == '\0')
	    continue;

	  /* Is the register in the group we're interested in?  */
	  if (!riscv_register_reggroup_p (gdbarch, regnum, reggroup))
	    continue;

	  riscv_print_one_register_info (gdbarch, file, frame, regnum);
	}
    }
}

/* Class that handles one decoded RiscV instruction.  */

class riscv_insn
{
public:

  /* Enum of all the opcodes that GDB cares about during the prologue scan.  */
  enum opcode
    {
      /* Unknown value is used at initialisation time.  */
      UNKNOWN = 0,

      /* These instructions are all the ones we are interested in during the
	 prologue scan.  */
      ADD,
      ADDI,
      ADDIW,
      ADDW,
      AUIPC,
      LUI,
      SD,
      SW,

      /* Other instructions are not interesting during the prologue scan, and
	 are ignored.  */
      OTHER
    };

  riscv_insn ()
    : m_length (0),
      m_opcode (OTHER),
      m_rd (0),
      m_rs1 (0),
      m_rs2 (0)
  {
    /* Nothing.  */
  }

  void decode (struct gdbarch *gdbarch, CORE_ADDR pc);

  /* Get the length of the instruction in bytes.  */
  int length () const
  { return m_length; }

  /* Get the opcode for this instruction.  */
  enum opcode opcode () const
  { return m_opcode; }

  /* Get destination register field for this instruction.  This is only
     valid if the OPCODE implies there is such a field for this
     instruction.  */
  int rd () const
  { return m_rd; }

  /* Get the RS1 register field for this instruction.  This is only valid
     if the OPCODE implies there is such a field for this instruction.  */
  int rs1 () const
  { return m_rs1; }

  /* Get the RS2 register field for this instruction.  This is only valid
     if the OPCODE implies there is such a field for this instruction.  */
  int rs2 () const
  { return m_rs2; }

  /* Get the immediate for this instruction in signed form.  This is only
     valid if the OPCODE implies there is such a field for this
     instruction.  */
  int imm_signed () const
  { return m_imm.s; }

private:

  /* Extract 5 bit register field at OFFSET from instruction OPCODE.  */
  int decode_register_index (unsigned long opcode, int offset)
  {
    return (opcode >> offset) & 0x1F;
  }

  /* Helper for DECODE, decode 32-bit R-type instruction.  */
  void decode_r_type_insn (enum opcode opcode, ULONGEST ival)
  {
    m_opcode = opcode;
    m_rd = decode_register_index (ival, OP_SH_RD);
    m_rs1 = decode_register_index (ival, OP_SH_RS1);
    m_rs2 = decode_register_index (ival, OP_SH_RS2);
  }

  /* Helper for DECODE, decode 16-bit compressed R-type instruction.  */
  void decode_cr_type_insn (enum opcode opcode, ULONGEST ival)
  {
    m_opcode = opcode;
    m_rd = m_rs1 = decode_register_index (ival, OP_SH_CRS1S);
    m_rs2 = decode_register_index (ival, OP_SH_CRS2);
  }

  /* Helper for DECODE, decode 32-bit I-type instruction.  */
  void decode_i_type_insn (enum opcode opcode, ULONGEST ival)
  {
    m_opcode = opcode;
    m_rd = decode_register_index (ival, OP_SH_RD);
    m_rs1 = decode_register_index (ival, OP_SH_RS1);
    m_imm.s = EXTRACT_ITYPE_IMM (ival);
  }

  /* Helper for DECODE, decode 16-bit compressed I-type instruction.  */
  void decode_ci_type_insn (enum opcode opcode, ULONGEST ival)
  {
    m_opcode = opcode;
    m_rd = m_rs1 = decode_register_index (ival, OP_SH_CRS1S);
    m_imm.s = EXTRACT_RVC_IMM (ival);
  }

  /* Helper for DECODE, decode 32-bit S-type instruction.  */
  void decode_s_type_insn (enum opcode opcode, ULONGEST ival)
  {
    m_opcode = opcode;
    m_rs1 = decode_register_index (ival, OP_SH_RS1);
    m_rs2 = decode_register_index (ival, OP_SH_RS2);
    m_imm.s = EXTRACT_STYPE_IMM (ival);
  }

  /* Helper for DECODE, decode 32-bit U-type instruction.  */
  void decode_u_type_insn (enum opcode opcode, ULONGEST ival)
  {
    m_opcode = opcode;
    m_rd = decode_register_index (ival, OP_SH_RD);
    m_imm.s = EXTRACT_UTYPE_IMM (ival);
  }

  /* Fetch instruction from target memory at ADDR, return the content of
     the instruction, and update LEN with the instruction length.  */
  static ULONGEST fetch_instruction (struct gdbarch *gdbarch,
				     CORE_ADDR addr, int *len);

  /* The length of the instruction in bytes.  Should be 2 or 4.  */
  int m_length;

  /* The instruction opcode.  */
  enum opcode m_opcode;

  /* The three possible registers an instruction might reference.  Not
     every instruction fills in all of these registers.  Which fields are
     valid depends on the opcode.  The naming of these fields matches the
     naming in the riscv isa manual.  */
  int m_rd;
  int m_rs1;
  int m_rs2;

  /* Possible instruction immediate.  This is only valid if the instruction
     format contains an immediate, not all instruction, whether this is
     valid depends on the opcode.  Despite only having one format for now
     the immediate is packed into a union, later instructions might require
     an unsigned formatted immediate, having the union in place now will
     reduce the need for code churn later.  */
  union riscv_insn_immediate
  {
    riscv_insn_immediate ()
      : s (0)
    {
      /* Nothing.  */
    }

    int s;
  } m_imm;
};

/* Fetch instruction from target memory at ADDR, return the content of the
   instruction, and update LEN with the instruction length.  */

ULONGEST
riscv_insn::fetch_instruction (struct gdbarch *gdbarch,
			       CORE_ADDR addr, int *len)
{
  enum bfd_endian byte_order = gdbarch_byte_order_for_code (gdbarch);
  gdb_byte buf[8];
  int instlen, status;

  /* All insns are at least 16 bits.  */
  status = target_read_memory (addr, buf, 2);
  if (status)
    memory_error (TARGET_XFER_E_IO, addr);

  /* If we need more, grab it now.  */
  instlen = riscv_insn_length (buf[0]);
  gdb_assert (instlen <= sizeof (buf));
  *len = instlen;

  if (instlen > 2)
    {
      status = target_read_memory (addr + 2, buf + 2, instlen - 2);
      if (status)
	memory_error (TARGET_XFER_E_IO, addr + 2);
    }

  return extract_unsigned_integer (buf, instlen, byte_order);
}

/* Fetch from target memory an instruction at PC and decode it.  */

void
riscv_insn::decode (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  ULONGEST ival;

  /* Fetch the instruction, and the instructions length.  */
  ival = fetch_instruction (gdbarch, pc, &m_length);

  if (m_length == 4)
    {
      if (is_add_insn (ival))
	decode_r_type_insn (ADD, ival);
      else if (is_addw_insn (ival))
	decode_r_type_insn (ADDW, ival);
      else if (is_addi_insn (ival))
	decode_i_type_insn (ADDI, ival);
      else if (is_addiw_insn (ival))
	decode_i_type_insn (ADDIW, ival);
      else if (is_auipc_insn (ival))
	decode_u_type_insn (AUIPC, ival);
      else if (is_lui_insn (ival))
	decode_u_type_insn (LUI, ival);
      else if (is_sd_insn (ival))
	decode_s_type_insn (SD, ival);
      else if (is_sw_insn (ival))
	decode_s_type_insn (SW, ival);
      else
	/* None of the other fields are valid in this case.  */
	m_opcode = OTHER;
    }
  else if (m_length == 2)
    {
      if (is_c_add_insn (ival))
	decode_cr_type_insn (ADD, ival);
      else if (is_c_addw_insn (ival))
	decode_cr_type_insn (ADDW, ival);
      else if (is_c_addi_insn (ival))
	decode_ci_type_insn (ADDI, ival);
      else if (is_c_addiw_insn (ival))
	decode_ci_type_insn (ADDIW, ival);
      else if (is_c_addi16sp_insn (ival))
	{
	  m_opcode = ADDI;
	  m_rd = m_rs1 = decode_register_index (ival, OP_SH_RD);
	  m_imm.s = EXTRACT_RVC_ADDI16SP_IMM (ival);
	}
      else if (is_lui_insn (ival))
	m_opcode = OTHER;
      else if (is_c_sd_insn (ival))
	m_opcode = OTHER;
      else if (is_sw_insn (ival))
	m_opcode = OTHER;
      else
	/* None of the other fields of INSN are valid in this case.  */
	m_opcode = OTHER;
    }
  else
    internal_error (__FILE__, __LINE__,
		    _("unable to decode %d byte instructions in "
		      "prologue at %s"), m_length,
		    core_addr_to_string (pc));
}

/* The prologue scanner.  This is currently only used for skipping the
   prologue of a function when the DWARF information is not sufficient.
   However, it is written with filling of the frame cache in mind, which
   is why different groups of stack setup instructions are split apart
   during the core of the inner loop.  In the future, the intention is to
   extend this function to fully support building up a frame cache that
   can unwind register values when there is no DWARF information.  */

static CORE_ADDR
riscv_scan_prologue (struct gdbarch *gdbarch,
		     CORE_ADDR start_pc, CORE_ADDR limit_pc)
{
  CORE_ADDR cur_pc, next_pc;
  long frame_offset = 0;
  CORE_ADDR end_prologue_addr = 0;

  if (limit_pc > start_pc + 200)
    limit_pc = start_pc + 200;

  for (next_pc = cur_pc = start_pc; cur_pc < limit_pc; cur_pc = next_pc)
    {
      struct riscv_insn insn;

      /* Decode the current instruction, and decide where the next
	 instruction lives based on the size of this instruction.  */
      insn.decode (gdbarch, cur_pc);
      gdb_assert (insn.length () > 0);
      next_pc = cur_pc + insn.length ();

      /* Look for common stack adjustment insns.  */
      if ((insn.opcode () == riscv_insn::ADDI
	   || insn.opcode () == riscv_insn::ADDIW)
	  && insn.rd () == RISCV_SP_REGNUM
	  && insn.rs1 () == RISCV_SP_REGNUM)
	{
	  /* Handle: addi sp, sp, -i
	     or:     addiw sp, sp, -i  */
	  if (insn.imm_signed () < 0)
	    frame_offset += insn.imm_signed ();
	  else
	    break;
	}
      else if ((insn.opcode () == riscv_insn::SW
		|| insn.opcode () == riscv_insn::SD)
	       && (insn.rs1 () == RISCV_SP_REGNUM
		   || insn.rs1 () == RISCV_FP_REGNUM))
	{
	  /* Handle: sw reg, offset(sp)
	     or:     sd reg, offset(sp)
	     or:     sw reg, offset(s0)
	     or:     sd reg, offset(s0)  */
	  /* Instruction storing a register onto the stack.  */
	}
      else if (insn.opcode () == riscv_insn::ADDI
	       && insn.rd () == RISCV_FP_REGNUM
	       && insn.rs1 () == RISCV_SP_REGNUM)
	{
	  /* Handle: addi s0, sp, size  */
	  /* Instructions setting up the frame pointer.  */
	}
      else if ((insn.opcode () == riscv_insn::ADD
		|| insn.opcode () == riscv_insn::ADDW)
	       && insn.rd () == RISCV_FP_REGNUM
	       && insn.rs1 () == RISCV_SP_REGNUM
	       && insn.rs2 () == RISCV_ZERO_REGNUM)
	{
	  /* Handle: add s0, sp, 0
	     or:     addw s0, sp, 0  */
	  /* Instructions setting up the frame pointer.  */
	}
      else if ((insn.rd () == RISCV_GP_REGNUM
		&& (insn.opcode () == riscv_insn::AUIPC
		    || insn.opcode () == riscv_insn::LUI
		    || (insn.opcode () == riscv_insn::ADDI
			&& insn.rs1 () == RISCV_GP_REGNUM)
		    || (insn.opcode () == riscv_insn::ADD
			&& (insn.rs1 () == RISCV_GP_REGNUM
			    || insn.rs2 () == RISCV_GP_REGNUM))))
	       || (insn.opcode () == riscv_insn::ADDI
		   && insn.rd () == RISCV_ZERO_REGNUM
		   && insn.rs1 () == RISCV_ZERO_REGNUM
		   && insn.imm_signed () == 0))
	{
	  /* Handle: auipc gp, n
	     or:     addi gp, gp, n
	     or:     add gp, gp, reg
	     or:     add gp, reg, gp
	     or:     lui gp, n
	     or:     add x0, x0, 0   (NOP)  */
	  /* These instructions are part of the prologue, but we don't need
	     to do anything special to handle them.  */
	}
      else
	{
	  if (end_prologue_addr == 0)
	    end_prologue_addr = cur_pc;
	}
    }

  if (end_prologue_addr == 0)
    end_prologue_addr = cur_pc;

  return end_prologue_addr;
}

/* Implement the riscv_skip_prologue gdbarch method.  */

static CORE_ADDR
riscv_skip_prologue (struct gdbarch *gdbarch,
		     CORE_ADDR       pc)
{
  CORE_ADDR limit_pc;
  CORE_ADDR func_addr;

  /* See if we can determine the end of the prologue via the symbol
     table.  If so, then return either PC, or the PC after the
     prologue, whichever is greater.  */
  if (find_pc_partial_function (pc, NULL, &func_addr, NULL))
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
  limit_pc = skip_prologue_using_sal (gdbarch, pc);
  if (limit_pc == 0)
    limit_pc = pc + 100;   /* MAGIC! */

  return riscv_scan_prologue (gdbarch, pc, limit_pc);
}

/* Implement the gdbarch push dummy code callback.  */

static CORE_ADDR
riscv_push_dummy_code (struct gdbarch *gdbarch, CORE_ADDR sp,
		       CORE_ADDR funaddr, struct value **args, int nargs,
		       struct type *value_type, CORE_ADDR *real_pc,
		       CORE_ADDR *bp_addr, struct regcache *regcache)
{
  /* Allocate space for a breakpoint, and keep the stack correctly
     aligned.  */
  sp -= 16;
  *bp_addr = sp;
  *real_pc = funaddr;
  return sp;
}

/* Compute the alignment of the type T.  Used while setting up the
   arguments for a dummy call.  */

static int
riscv_type_alignment (struct type *t)
{
  t = check_typedef (t);
  switch (TYPE_CODE (t))
    {
    default:
      error (_("Could not compute alignment of type"));

    case TYPE_CODE_RVALUE_REF:
    case TYPE_CODE_PTR:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
    case TYPE_CODE_REF:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_BOOL:
      return TYPE_LENGTH (t);

    case TYPE_CODE_ARRAY:
    case TYPE_CODE_COMPLEX:
      return riscv_type_alignment (TYPE_TARGET_TYPE (t));

    case TYPE_CODE_STRUCT:
    case TYPE_CODE_UNION:
      {
	int i;
	int align = 1;

	for (i = 0; i < TYPE_NFIELDS (t); ++i)
	  {
	    if (TYPE_FIELD_LOC_KIND (t, i) == FIELD_LOC_KIND_BITPOS)
	      {
		int a = riscv_type_alignment (TYPE_FIELD_TYPE (t, i));
		if (a > align)
		  align = a;
	      }
	  }
	return align;
      }
    }
}

/* Holds information about a single argument either being passed to an
   inferior function, or returned from an inferior function.  This includes
   information about the size, type, etc of the argument, and also
   information about how the argument will be passed (or returned).  */

struct riscv_arg_info
{
  /* Contents of the argument.  */
  const gdb_byte *contents;

  /* Length of argument.  */
  int length;

  /* Alignment required for an argument of this type.  */
  int align;

  /* The type for this argument.  */
  struct type *type;

  /* Each argument can have either 1 or 2 locations assigned to it.  Each
     location describes where part of the argument will be placed.  The
     second location is valid based on the LOC_TYPE and C_LENGTH fields
     of the first location (which is always valid).  */
  struct location
  {
    /* What type of location this is.  */
    enum location_type
      {
       /* Argument passed in a register.  */
       in_reg,

       /* Argument passed as an on stack argument.  */
       on_stack,

       /* Argument passed by reference.  The second location is always
	  valid for a BY_REF argument, and describes where the address
	  of the BY_REF argument should be placed.  */
       by_ref
      } loc_type;

    /* Information that depends on the location type.  */
    union
    {
      /* Which register number to use.  */
      int regno;

      /* The offset into the stack region.  */
      int offset;
    } loc_data;

    /* The length of contents covered by this location.  If this is less
       than the total length of the argument, then the second location
       will be valid, and will describe where the rest of the argument
       will go.  */
    int c_length;

    /* The offset within CONTENTS for this part of the argument.  Will
       always be 0 for the first part.  For the second part of the
       argument, this might be the C_LENGTH value of the first part,
       however, if we are passing a structure in two registers, and there's
       is padding between the first and second field, then this offset
       might be greater than the length of the first argument part.  When
       the second argument location is not holding part of the argument
       value, but is instead holding the address of a reference argument,
       then this offset will be set to 0.  */
    int c_offset;
  } argloc[2];
};

/* Information about a set of registers being used for passing arguments as
   part of a function call.  The register set must be numerically
   sequential from NEXT_REGNUM to LAST_REGNUM.  The register set can be
   disabled from use by setting NEXT_REGNUM greater than LAST_REGNUM.  */

struct riscv_arg_reg
{
  riscv_arg_reg (int first, int last)
    : next_regnum (first),
      last_regnum (last)
  {
    /* Nothing.  */
  }

  /* The GDB register number to use in this set.  */
  int next_regnum;

  /* The last GDB register number to use in this set.  */
  int last_regnum;
};

/* Arguments can be passed as on stack arguments, or by reference.  The
   on stack arguments must be in a continuous region starting from $sp,
   while the by reference arguments can be anywhere, but we'll put them
   on the stack after (at higher address) the on stack arguments.

   This might not be the right approach to take.  The ABI is clear that
   an argument passed by reference can be modified by the callee, which
   us placing the argument (temporarily) onto the stack will not achieve
   (changes will be lost).  There's also the possibility that very large
   arguments could overflow the stack.

   This struct is used to track offset into these two areas for where
   arguments are to be placed.  */
struct riscv_memory_offsets
{
  riscv_memory_offsets ()
    : arg_offset (0),
      ref_offset (0)
  {
    /* Nothing.  */
  }

  /* Offset into on stack argument area.  */
  int arg_offset;

  /* Offset into the pass by reference area.  */
  int ref_offset;
};

/* Holds information about where arguments to a call will be placed.  This
   is updated as arguments are added onto the call, and can be used to
   figure out where the next argument should be placed.  */

struct riscv_call_info
{
  riscv_call_info (struct gdbarch *gdbarch)
    : int_regs (RISCV_A0_REGNUM, RISCV_A0_REGNUM + 7),
      float_regs (RISCV_FA0_REGNUM, RISCV_FA0_REGNUM + 7)
  {
    xlen = riscv_isa_xlen (gdbarch);
    flen = riscv_isa_flen (gdbarch);

    /* Disable use of floating point registers if needed.  */
    if (!riscv_has_fp_abi (gdbarch))
      float_regs.next_regnum = float_regs.last_regnum + 1;
  }

  /* Track the memory areas used for holding in-memory arguments to a
     call.  */
  struct riscv_memory_offsets memory;

  /* Holds information about the next integer register to use for passing
     an argument.  */
  struct riscv_arg_reg int_regs;

  /* Holds information about the next floating point register to use for
     passing an argument.  */
  struct riscv_arg_reg float_regs;

  /* The XLEN and FLEN are copied in to this structure for convenience, and
     are just the results of calling RISCV_ISA_XLEN and RISCV_ISA_FLEN.  */
  int xlen;
  int flen;
};

/* Return the number of registers available for use as parameters in the
   register set REG.  Returned value can be 0 or more.  */

static int
riscv_arg_regs_available (struct riscv_arg_reg *reg)
{
  if (reg->next_regnum > reg->last_regnum)
    return 0;

  return (reg->last_regnum - reg->next_regnum + 1);
}

/* If there is at least one register available in the register set REG then
   the next register from REG is assigned to LOC and the length field of
   LOC is updated to LENGTH.  The register set REG is updated to indicate
   that the assigned register is no longer available and the function
   returns true.

   If there are no registers available in REG then the function returns
   false, and LOC and REG are unchanged.  */

static bool
riscv_assign_reg_location (struct riscv_arg_info::location *loc,
			   struct riscv_arg_reg *reg,
			   int length, int offset)
{
  if (reg->next_regnum <= reg->last_regnum)
    {
      loc->loc_type = riscv_arg_info::location::in_reg;
      loc->loc_data.regno = reg->next_regnum;
      reg->next_regnum++;
      loc->c_length = length;
      loc->c_offset = offset;
      return true;
    }

  return false;
}

/* Assign LOC a location as the next stack parameter, and update MEMORY to
   record that an area of stack has been used to hold the parameter
   described by LOC.

   The length field of LOC is updated to LENGTH, the length of the
   parameter being stored, and ALIGN is the alignment required by the
   parameter, which will affect how memory is allocated out of MEMORY.  */

static void
riscv_assign_stack_location (struct riscv_arg_info::location *loc,
			     struct riscv_memory_offsets *memory,
			     int length, int align)
{
  loc->loc_type = riscv_arg_info::location::on_stack;
  memory->arg_offset
    = align_up (memory->arg_offset, align);
  loc->loc_data.offset = memory->arg_offset;
  memory->arg_offset += length;
  loc->c_length = length;

  /* Offset is always 0, either we're the first location part, in which
     case we're reading content from the start of the argument, or we're
     passing the address of a reference argument, so 0.  */
  loc->c_offset = 0;
}

/* Update AINFO, which describes an argument that should be passed or
   returned using the integer ABI.  The argloc fields within AINFO are
   updated to describe the location in which the argument will be passed to
   a function, or returned from a function.

   The CINFO structure contains the ongoing call information, the holds
   information such as which argument registers are remaining to be
   assigned to parameter, and how much memory has been used by parameters
   so far.

   By examining the state of CINFO a suitable location can be selected,
   and assigned to AINFO.  */

static void
riscv_call_arg_scalar_int (struct riscv_arg_info *ainfo,
			   struct riscv_call_info *cinfo)
{
  if (ainfo->length > (2 * cinfo->xlen))
    {
      /* Argument is going to be passed by reference.  */
      ainfo->argloc[0].loc_type
	= riscv_arg_info::location::by_ref;
      cinfo->memory.ref_offset
	= align_up (cinfo->memory.ref_offset, ainfo->align);
      ainfo->argloc[0].loc_data.offset = cinfo->memory.ref_offset;
      cinfo->memory.ref_offset += ainfo->length;
      ainfo->argloc[0].c_length = ainfo->length;

      /* The second location for this argument is given over to holding the
	 address of the by-reference data.  Pass 0 for the offset as this
	 is not part of the actual argument value.  */
      if (!riscv_assign_reg_location (&ainfo->argloc[1],
				      &cinfo->int_regs,
				      cinfo->xlen, 0))
	riscv_assign_stack_location (&ainfo->argloc[1],
				     &cinfo->memory, cinfo->xlen,
				     cinfo->xlen);
    }
  else
    {
      int len = (ainfo->length > cinfo->xlen) ? cinfo->xlen : ainfo->length;

      if (!riscv_assign_reg_location (&ainfo->argloc[0],
				      &cinfo->int_regs, len, 0))
	riscv_assign_stack_location (&ainfo->argloc[0],
				     &cinfo->memory, len, ainfo->align);

      if (len < ainfo->length)
	{
	  len = ainfo->length - len;
	  if (!riscv_assign_reg_location (&ainfo->argloc[1],
					  &cinfo->int_regs, len,
					  cinfo->xlen))
	    riscv_assign_stack_location (&ainfo->argloc[1],
					 &cinfo->memory, len, cinfo->xlen);
	}
    }
}

/* Like RISCV_CALL_ARG_SCALAR_INT, except the argument described by AINFO
   is being passed with the floating point ABI.  */

static void
riscv_call_arg_scalar_float (struct riscv_arg_info *ainfo,
			     struct riscv_call_info *cinfo)
{
  if (ainfo->length > cinfo->flen)
    return riscv_call_arg_scalar_int (ainfo, cinfo);
  else
    {
      if (!riscv_assign_reg_location (&ainfo->argloc[0],
				      &cinfo->float_regs,
				      ainfo->length, 0))
	return riscv_call_arg_scalar_int (ainfo, cinfo);
    }
}

/* Like RISCV_CALL_ARG_SCALAR_INT, except the argument described by AINFO
   is a complex floating point argument, and is therefore handled
   differently to other argument types.  */

static void
riscv_call_arg_complex_float (struct riscv_arg_info *ainfo,
			      struct riscv_call_info *cinfo)
{
  if (ainfo->length <= (2 * cinfo->flen)
      && riscv_arg_regs_available (&cinfo->float_regs) >= 2)
    {
      bool result;
      int len = ainfo->length / 2;

      result = riscv_assign_reg_location (&ainfo->argloc[0],
					  &cinfo->float_regs, len, len);
      gdb_assert (result);

      result = riscv_assign_reg_location (&ainfo->argloc[1],
					  &cinfo->float_regs, len, len);
      gdb_assert (result);
    }
  else
    return riscv_call_arg_scalar_int (ainfo, cinfo);
}

/* A structure used for holding information about a structure type within
   the inferior program.  The RiscV ABI has special rules for handling some
   structures with a single field or with two fields.  The counting of
   fields here is done after flattening out all nested structures.  */

class riscv_struct_info
{
public:
  riscv_struct_info ()
    : m_number_of_fields (0),
      m_types { nullptr, nullptr }
  {
    /* Nothing.  */
  }

  /* Analyse TYPE descending into nested structures, count the number of
     scalar fields and record the types of the first two fields found.  */
  void analyse (struct type *type);

  /* The number of scalar fields found in the analysed type.  This is
     currently only accurate if the value returned is 0, 1, or 2 as the
     analysis stops counting when the number of fields is 3.  This is
     because the RiscV ABI only has special cases for 1 or 2 fields,
     anything else we just don't care about.  */
  int number_of_fields () const
  { return m_number_of_fields; }

  /* Return the type for scalar field INDEX within the analysed type.  Will
     return nullptr if there is no field at that index.  Only INDEX values
     0 and 1 can be requested as the RiscV ABI only has special cases for
     structures with 1 or 2 fields.  */
  struct type *field_type (int index) const
  {
    gdb_assert (index < (sizeof (m_types) / sizeof (m_types[0])));
    return m_types[index];
  }

private:
  /* The number of scalar fields found within the structure after recursing
     into nested structures.  */
  int m_number_of_fields;

  /* The types of the first two scalar fields found within the structure
     after recursing into nested structures.  */
  struct type *m_types[2];
};

/* Analyse TYPE descending into nested structures, count the number of
   scalar fields and record the types of the first two fields found.  */

void
riscv_struct_info::analyse (struct type *type)
{
  unsigned int count = TYPE_NFIELDS (type);
  unsigned int i;

  for (i = 0; i < count; ++i)
    {
      if (TYPE_FIELD_LOC_KIND (type, i) != FIELD_LOC_KIND_BITPOS)
	continue;

      struct type *field_type = TYPE_FIELD_TYPE (type, i);
      field_type = check_typedef (field_type);

      switch (TYPE_CODE (field_type))
	{
	case TYPE_CODE_STRUCT:
	  analyse (field_type);
	  break;

	default:
	  /* RiscV only flattens out structures.  Anything else does not
	     need to be flattened, we just record the type, and when we
	     look at the analysis results we'll realise this is not a
	     structure we can special case, and pass the structure in
	     memory.  */
	  if (m_number_of_fields < 2)
	    m_types[m_number_of_fields] = field_type;
	  m_number_of_fields++;
	  break;
	}

      /* RiscV only has special handling for structures with 1 or 2 scalar
	 fields, any more than that and the structure is just passed in
	 memory.  We can safely drop out early when we find 3 or more
	 fields then.  */

      if (m_number_of_fields > 2)
	return;
    }
}

/* Like RISCV_CALL_ARG_SCALAR_INT, except the argument described by AINFO
   is a structure.  Small structures on RiscV have some special case
   handling in order that the structure might be passed in register.
   Larger structures are passed in memory.  After assigning location
   information to AINFO, CINFO will have been updated.  */

static void
riscv_call_arg_struct (struct riscv_arg_info *ainfo,
		       struct riscv_call_info *cinfo)
{
  if (riscv_arg_regs_available (&cinfo->float_regs) >= 1)
    {
      struct riscv_struct_info sinfo;

      sinfo.analyse (ainfo->type);
      if (sinfo.number_of_fields () == 1
	  && TYPE_CODE (sinfo.field_type (0)) == TYPE_CODE_COMPLEX)
	{
	  gdb_assert (TYPE_LENGTH (ainfo->type)
		      == TYPE_LENGTH (sinfo.field_type (0)));
	  return riscv_call_arg_complex_float (ainfo, cinfo);
	}

      if (sinfo.number_of_fields () == 1
	  && TYPE_CODE (sinfo.field_type (0)) == TYPE_CODE_FLT)
	{
	  gdb_assert (TYPE_LENGTH (ainfo->type)
		      == TYPE_LENGTH (sinfo.field_type (0)));
	  return riscv_call_arg_scalar_float (ainfo, cinfo);
	}

      if (sinfo.number_of_fields () == 2
	  && TYPE_CODE (sinfo.field_type (0)) == TYPE_CODE_FLT
	  && TYPE_LENGTH (sinfo.field_type (0)) <= cinfo->flen
	  && TYPE_CODE (sinfo.field_type (1)) == TYPE_CODE_FLT
	  && TYPE_LENGTH (sinfo.field_type (1)) <= cinfo->flen
	  && riscv_arg_regs_available (&cinfo->float_regs) >= 2)
	{
	  int len0, len1, offset;

	  gdb_assert (TYPE_LENGTH (ainfo->type) <= (2 * cinfo->flen));

	  len0 = TYPE_LENGTH (sinfo.field_type (0));
	  if (!riscv_assign_reg_location (&ainfo->argloc[0],
					  &cinfo->float_regs, len0, 0))
	    error (_("failed during argument setup"));

	  len1 = TYPE_LENGTH (sinfo.field_type (1));
	  offset = align_up (len0, riscv_type_alignment (sinfo.field_type (1)));
	  gdb_assert (len1 <= (TYPE_LENGTH (ainfo->type)
			       - TYPE_LENGTH (sinfo.field_type (0))));

	  if (!riscv_assign_reg_location (&ainfo->argloc[1],
					  &cinfo->float_regs,
					  len1, offset))
	    error (_("failed during argument setup"));
	  return;
	}

      if (sinfo.number_of_fields () == 2
	  && riscv_arg_regs_available (&cinfo->int_regs) >= 1
	  && (TYPE_CODE (sinfo.field_type (0)) == TYPE_CODE_FLT
	      && TYPE_LENGTH (sinfo.field_type (0)) <= cinfo->flen
	      && is_integral_type (sinfo.field_type (1))
	      && TYPE_LENGTH (sinfo.field_type (1)) <= cinfo->xlen))
	{
	  int len0, len1, offset;

	  gdb_assert (TYPE_LENGTH (ainfo->type)
		      <= (cinfo->flen + cinfo->xlen));

	  len0 = TYPE_LENGTH (sinfo.field_type (0));
	  if (!riscv_assign_reg_location (&ainfo->argloc[0],
					  &cinfo->float_regs, len0, 0))
	    error (_("failed during argument setup"));

	  len1 = TYPE_LENGTH (sinfo.field_type (1));
	  offset = align_up (len0, riscv_type_alignment (sinfo.field_type (1)));
	  gdb_assert (len1 <= cinfo->xlen);
	  if (!riscv_assign_reg_location (&ainfo->argloc[1],
					  &cinfo->int_regs, len1, offset))
	    error (_("failed during argument setup"));
	  return;
	}

      if (sinfo.number_of_fields () == 2
	  && riscv_arg_regs_available (&cinfo->int_regs) >= 1
	  && (is_integral_type (sinfo.field_type (0))
	      && TYPE_LENGTH (sinfo.field_type (0)) <= cinfo->xlen
	      && TYPE_CODE (sinfo.field_type (1)) == TYPE_CODE_FLT
	      && TYPE_LENGTH (sinfo.field_type (1)) <= cinfo->flen))
	{
	  int len0, len1, offset;

	  gdb_assert (TYPE_LENGTH (ainfo->type)
		      <= (cinfo->flen + cinfo->xlen));

	  len0 = TYPE_LENGTH (sinfo.field_type (0));
	  len1 = TYPE_LENGTH (sinfo.field_type (1));
	  offset = align_up (len0, riscv_type_alignment (sinfo.field_type (1)));

	  gdb_assert (len0 <= cinfo->xlen);
	  gdb_assert (len1 <= cinfo->flen);

	  if (!riscv_assign_reg_location (&ainfo->argloc[0],
					  &cinfo->int_regs, len0, 0))
	    error (_("failed during argument setup"));

	  if (!riscv_assign_reg_location (&ainfo->argloc[1],
					  &cinfo->float_regs,
					  len1, offset))
	    error (_("failed during argument setup"));

	  return;
	}
    }

  /* Non of the structure flattening cases apply, so we just pass using
     the integer ABI.  */
  ainfo->length = align_up (ainfo->length, cinfo->xlen);
  riscv_call_arg_scalar_int (ainfo, cinfo);
}

/* Assign a location to call (or return) argument AINFO, the location is
   selected from CINFO which holds information about what call argument
   locations are available for use next.  The TYPE is the type of the
   argument being passed, this information is recorded into AINFO (along
   with some additional information derived from the type).

   After assigning a location to AINFO, CINFO will have been updated.  */

static void
riscv_arg_location (struct gdbarch *gdbarch,
		    struct riscv_arg_info *ainfo,
		    struct riscv_call_info *cinfo,
		    struct type *type)
{
  ainfo->type = type;
  ainfo->length = TYPE_LENGTH (ainfo->type);
  ainfo->align = riscv_type_alignment (ainfo->type);
  ainfo->contents = nullptr;

  switch (TYPE_CODE (ainfo->type))
    {
    case TYPE_CODE_INT:
    case TYPE_CODE_BOOL:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_PTR:
      if (ainfo->length <= cinfo->xlen)
	{
	  ainfo->type = builtin_type (gdbarch)->builtin_long;
	  ainfo->length = cinfo->xlen;
	}
      else if (ainfo->length <= (2 * cinfo->xlen))
	{
	  ainfo->type = builtin_type (gdbarch)->builtin_long_long;
	  ainfo->length = 2 * cinfo->xlen;
	}

      /* Recalculate the alignment requirement.  */
      ainfo->align = riscv_type_alignment (ainfo->type);
      riscv_call_arg_scalar_int (ainfo, cinfo);
      break;

    case TYPE_CODE_FLT:
      riscv_call_arg_scalar_float (ainfo, cinfo);
      break;

    case TYPE_CODE_COMPLEX:
      riscv_call_arg_complex_float (ainfo, cinfo);
      break;

    case TYPE_CODE_STRUCT:
      riscv_call_arg_struct (ainfo, cinfo);
      break;

    default:
      riscv_call_arg_scalar_int (ainfo, cinfo);
      break;
    }
}

/* Used for printing debug information about the call argument location in
   INFO to STREAM.  The addresses in SP_REFS and SP_ARGS are the base
   addresses for the location of pass-by-reference and
   arguments-on-the-stack memory areas.  */

static void
riscv_print_arg_location (ui_file *stream, struct gdbarch *gdbarch,
			  struct riscv_arg_info *info,
			  CORE_ADDR sp_refs, CORE_ADDR sp_args)
{
  const char* type_name = TYPE_NAME (info->type);
  if (type_name == nullptr)
    type_name = "???";

  fprintf_unfiltered (stream, "type: '%s', length: 0x%x, alignment: 0x%x",
		      type_name, info->length, info->align);
  switch (info->argloc[0].loc_type)
    {
    case riscv_arg_info::location::in_reg:
      fprintf_unfiltered
	(stream, ", register %s",
	 gdbarch_register_name (gdbarch, info->argloc[0].loc_data.regno));
      if (info->argloc[0].c_length < info->length)
	{
	  switch (info->argloc[1].loc_type)
	    {
	    case riscv_arg_info::location::in_reg:
	      fprintf_unfiltered
		(stream, ", register %s",
		 gdbarch_register_name (gdbarch,
					info->argloc[1].loc_data.regno));
	      break;

	    case riscv_arg_info::location::on_stack:
	      fprintf_unfiltered (stream, ", on stack at offset 0x%x",
				  info->argloc[1].loc_data.offset);
	      break;

	    case riscv_arg_info::location::by_ref:
	    default:
	      /* The second location should never be a reference, any
		 argument being passed by reference just places its address
		 in the first location and is done.  */
	      error (_("invalid argument location"));
	      break;
	    }

	  if (info->argloc[1].c_offset > info->argloc[0].c_length)
	    fprintf_unfiltered (stream, " (offset 0x%x)",
				info->argloc[1].c_offset);
	}
      break;

    case riscv_arg_info::location::on_stack:
      fprintf_unfiltered (stream, ", on stack at offset 0x%x",
			  info->argloc[0].loc_data.offset);
      break;

    case riscv_arg_info::location::by_ref:
      fprintf_unfiltered
	(stream, ", by reference, data at offset 0x%x (%s)",
	 info->argloc[0].loc_data.offset,
	 core_addr_to_string (sp_refs + info->argloc[0].loc_data.offset));
      if (info->argloc[1].loc_type
	  == riscv_arg_info::location::in_reg)
	fprintf_unfiltered
	  (stream, ", address in register %s",
	   gdbarch_register_name (gdbarch, info->argloc[1].loc_data.regno));
      else
	{
	  gdb_assert (info->argloc[1].loc_type
		      == riscv_arg_info::location::on_stack);
	  fprintf_unfiltered
	    (stream, ", address on stack at offset 0x%x (%s)",
	     info->argloc[1].loc_data.offset,
	     core_addr_to_string (sp_args + info->argloc[1].loc_data.offset));
	}
      break;

    default:
      gdb_assert_not_reached (_("unknown argument location type"));
    }
}

/* Implement the push dummy call gdbarch callback.  */

static CORE_ADDR
riscv_push_dummy_call (struct gdbarch *gdbarch,
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
  CORE_ADDR sp_args, sp_refs;
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  struct riscv_arg_info *arg_info =
    (struct riscv_arg_info *) alloca (nargs * sizeof (struct riscv_arg_info));
  struct riscv_arg_info *info;

  struct riscv_call_info call_info (gdbarch);

  CORE_ADDR osp = sp;

  /* We'll use register $a0 if we're returning a struct.  */
  if (struct_return)
    ++call_info.int_regs.next_regnum;

  for (i = 0, info = &arg_info[0];
       i < nargs;
       ++i, ++info)
    {
      struct value *arg_value;
      struct type *arg_type;

      arg_value = args[i];
      arg_type = check_typedef (value_type (arg_value));

      riscv_arg_location (gdbarch, info, &call_info, arg_type);

      if (info->type != arg_type)
	arg_value = value_cast (info->type, arg_value);
      info->contents = value_contents (arg_value);
    }

  /* Adjust the stack pointer and align it.  */
  sp = sp_refs = align_down (sp - call_info.memory.ref_offset, SP_ALIGNMENT);
  sp = sp_args = align_down (sp - call_info.memory.arg_offset, SP_ALIGNMENT);

  if (riscv_debug_infcall > 0)
    {
      fprintf_unfiltered (gdb_stdlog, "dummy call args:\n");
      fprintf_unfiltered (gdb_stdlog, ": floating point ABI %s in use\n",
	       (riscv_has_fp_abi (gdbarch) ? "is" : "is not"));
      fprintf_unfiltered (gdb_stdlog, ": xlen: %d\n: flen: %d\n",
	       call_info.xlen, call_info.flen);
      if (struct_return)
	fprintf_unfiltered (gdb_stdlog,
			    "[*] struct return pointer in register $A0\n");
      for (i = 0; i < nargs; ++i)
	{
	  struct riscv_arg_info *info = &arg_info [i];

	  fprintf_unfiltered (gdb_stdlog, "[%2d] ", i);
	  riscv_print_arg_location (gdb_stdlog, gdbarch, info, sp_refs, sp_args);
	  fprintf_unfiltered (gdb_stdlog, "\n");
	}
      if (call_info.memory.arg_offset > 0
	  || call_info.memory.ref_offset > 0)
	{
	  fprintf_unfiltered (gdb_stdlog, "              Original sp: %s\n",
			      core_addr_to_string (osp));
	  fprintf_unfiltered (gdb_stdlog, "Stack required (for args): 0x%x\n",
			      call_info.memory.arg_offset);
	  fprintf_unfiltered (gdb_stdlog, "Stack required (for refs): 0x%x\n",
			      call_info.memory.ref_offset);
	  fprintf_unfiltered (gdb_stdlog, "          Stack allocated: %s\n",
			      core_addr_to_string_nz (osp - sp));
	}
    }

  /* Now load the argument into registers, or onto the stack.  */

  if (struct_return)
    {
      gdb_byte buf[sizeof (LONGEST)];

      store_unsigned_integer (buf, call_info.xlen, byte_order, struct_addr);
      regcache->cooked_write (RISCV_A0_REGNUM, buf);
    }

  for (i = 0; i < nargs; ++i)
    {
      CORE_ADDR dst;
      int second_arg_length = 0;
      const gdb_byte *second_arg_data;
      struct riscv_arg_info *info = &arg_info [i];

      gdb_assert (info->length > 0);

      switch (info->argloc[0].loc_type)
	{
	case riscv_arg_info::location::in_reg:
	  {
	    gdb_byte tmp [sizeof (ULONGEST)];

	    gdb_assert (info->argloc[0].c_length <= info->length);
	    memset (tmp, 0, sizeof (tmp));
	    memcpy (tmp, info->contents, info->argloc[0].c_length);
	    regcache->cooked_write (info->argloc[0].loc_data.regno, tmp);
	    second_arg_length =
	      ((info->argloc[0].c_length < info->length)
	       ? info->argloc[1].c_length : 0);
	    second_arg_data = info->contents + info->argloc[1].c_offset;
	  }
	  break;

	case riscv_arg_info::location::on_stack:
	  dst = sp_args + info->argloc[0].loc_data.offset;
	  write_memory (dst, info->contents, info->length);
	  second_arg_length = 0;
	  break;

	case riscv_arg_info::location::by_ref:
	  dst = sp_refs + info->argloc[0].loc_data.offset;
	  write_memory (dst, info->contents, info->length);

	  second_arg_length = call_info.xlen;
	  second_arg_data = (gdb_byte *) &dst;
	  break;

	default:
	  gdb_assert_not_reached (_("unknown argument location type"));
	}

      if (second_arg_length > 0)
	{
	  switch (info->argloc[1].loc_type)
	    {
	    case riscv_arg_info::location::in_reg:
	      {
		gdb_byte tmp [sizeof (ULONGEST)];

		gdb_assert (second_arg_length <= call_info.xlen);
		memset (tmp, 0, sizeof (tmp));
		memcpy (tmp, second_arg_data, second_arg_length);
		regcache->cooked_write (info->argloc[1].loc_data.regno, tmp);
	      }
	      break;

	    case riscv_arg_info::location::on_stack:
	      {
		CORE_ADDR arg_addr;

		arg_addr = sp_args + info->argloc[1].loc_data.offset;
		write_memory (arg_addr, second_arg_data, second_arg_length);
		break;
	      }

	    case riscv_arg_info::location::by_ref:
	    default:
	      /* The second location should never be a reference, any
		 argument being passed by reference just places its address
		 in the first location and is done.  */
	      error (_("invalid argument location"));
	      break;
	    }
	}
    }

  /* Set the dummy return value to bp_addr.
     A dummy breakpoint will be setup to execute the call.  */

  if (riscv_debug_infcall > 0)
    fprintf_unfiltered (gdb_stdlog, ": writing $ra = %s\n",
			core_addr_to_string (bp_addr));
  regcache_cooked_write_unsigned (regcache, RISCV_RA_REGNUM, bp_addr);

  /* Finally, update the stack pointer.  */

  if (riscv_debug_infcall > 0)
    fprintf_unfiltered (gdb_stdlog, ": writing $sp = %s\n",
			core_addr_to_string (sp));
  regcache_cooked_write_unsigned (regcache, RISCV_SP_REGNUM, sp);

  return sp;
}

/* Implement the return_value gdbarch method.  */

static enum return_value_convention
riscv_return_value (struct gdbarch  *gdbarch,
		    struct value *function,
		    struct type *type,
		    struct regcache *regcache,
		    gdb_byte *readbuf,
		    const gdb_byte *writebuf)
{
  enum type_code rv_type = TYPE_CODE (type);
  unsigned int rv_size = TYPE_LENGTH (type);
  int fp, regnum, flen;
  ULONGEST tmp;
  struct riscv_call_info call_info (gdbarch);
  struct riscv_arg_info info;
  struct type *arg_type;

  arg_type = check_typedef (type);
  riscv_arg_location (gdbarch, &info, &call_info, arg_type);

  if (riscv_debug_infcall > 0)
    {
      fprintf_unfiltered (gdb_stdlog, "riscv return value:\n");
      fprintf_unfiltered (gdb_stdlog, "[R] ");
      riscv_print_arg_location (gdb_stdlog, gdbarch, &info, 0, 0);
      fprintf_unfiltered (gdb_stdlog, "\n");
    }

  if (readbuf != nullptr || writebuf != nullptr)
    {
        int regnum;

	switch (info.argloc[0].loc_type)
	  {
	    /* Return value in register(s).  */
	  case riscv_arg_info::location::in_reg:
	    {
	      regnum = info.argloc[0].loc_data.regno;

	      if (readbuf)
		regcache->cooked_read (regnum, readbuf);

	      if (writebuf)
		regcache->cooked_write (regnum, writebuf);

	      /* A return value in register can have a second part in a
		 second register.  */
	      if (info.argloc[0].c_length < info.length)
		{
		  switch (info.argloc[1].loc_type)
		    {
		    case riscv_arg_info::location::in_reg:
		      regnum = info.argloc[1].loc_data.regno;

		      if (readbuf)
			{
			  readbuf += info.argloc[1].c_offset;
			  regcache->cooked_read (regnum, readbuf);
			}

		      if (writebuf)
			{
			  writebuf += info.argloc[1].c_offset;
			  regcache->cooked_write (regnum, writebuf);
			}
		      break;

		    case riscv_arg_info::location::by_ref:
		    case riscv_arg_info::location::on_stack:
		    default:
		      error (_("invalid argument location"));
		      break;
		    }
		}
	    }
	    break;

	    /* Return value by reference will have its address in A0.  */
	  case riscv_arg_info::location::by_ref:
	    {
	      ULONGEST addr;

	      regcache_cooked_read_unsigned (regcache, RISCV_A0_REGNUM,
					     &addr);
	      if (readbuf != nullptr)
		read_memory (addr, readbuf, info.length);
	      if (writebuf != nullptr)
		write_memory (addr, writebuf, info.length);
	    }
	    break;

	  case riscv_arg_info::location::on_stack:
	  default:
	    error (_("invalid argument location"));
	    break;
	  }
    }

  switch (info.argloc[0].loc_type)
    {
    case riscv_arg_info::location::in_reg:
      return RETURN_VALUE_REGISTER_CONVENTION;
    case riscv_arg_info::location::by_ref:
      return RETURN_VALUE_ABI_RETURNS_ADDRESS;
    case riscv_arg_info::location::on_stack:
    default:
      error (_("invalid argument location"));
    }
}

/* Implement the frame_align gdbarch method.  */

static CORE_ADDR
riscv_frame_align (struct gdbarch *gdbarch, CORE_ADDR addr)
{
  return align_down (addr, 16);
}

/* Implement the unwind_pc gdbarch method.  */

static CORE_ADDR
riscv_unwind_pc (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, RISCV_PC_REGNUM);
}

/* Implement the unwind_sp gdbarch method.  */

static CORE_ADDR
riscv_unwind_sp (struct gdbarch *gdbarch, struct frame_info *next_frame)
{
  return frame_unwind_register_unsigned (next_frame, RISCV_SP_REGNUM);
}

/* Implement the dummy_id gdbarch method.  */

static struct frame_id
riscv_dummy_id (struct gdbarch *gdbarch, struct frame_info *this_frame)
{
  return frame_id_build (get_frame_register_signed (this_frame, RISCV_SP_REGNUM),
			 get_frame_pc (this_frame));
}

/* Generate, or return the cached frame cache for the RiscV frame
   unwinder.  */

static struct trad_frame_cache *
riscv_frame_cache (struct frame_info *this_frame, void **this_cache)
{
  CORE_ADDR pc;
  CORE_ADDR start_addr;
  CORE_ADDR stack_addr;
  struct trad_frame_cache *this_trad_cache;
  struct gdbarch *gdbarch = get_frame_arch (this_frame);

  if ((*this_cache) != NULL)
    return (struct trad_frame_cache *) *this_cache;
  this_trad_cache = trad_frame_cache_zalloc (this_frame);
  (*this_cache) = this_trad_cache;

  trad_frame_set_reg_realreg (this_trad_cache, gdbarch_pc_regnum (gdbarch),
			      RISCV_RA_REGNUM);

  pc = get_frame_pc (this_frame);
  find_pc_partial_function (pc, NULL, &start_addr, NULL);
  stack_addr = get_frame_register_signed (this_frame, RISCV_SP_REGNUM);
  trad_frame_set_id (this_trad_cache, frame_id_build (stack_addr, start_addr));

  trad_frame_set_this_base (this_trad_cache, stack_addr);

  return this_trad_cache;
}

/* Implement the this_id callback for RiscV frame unwinder.  */

static void
riscv_frame_this_id (struct frame_info *this_frame,
		     void **prologue_cache,
		     struct frame_id *this_id)
{
  struct trad_frame_cache *info;

  info = riscv_frame_cache (this_frame, prologue_cache);
  trad_frame_get_id (info, this_id);
}

/* Implement the prev_register callback for RiscV frame unwinder.  */

static struct value *
riscv_frame_prev_register (struct frame_info *this_frame,
			   void **prologue_cache,
			   int regnum)
{
  struct trad_frame_cache *info;

  info = riscv_frame_cache (this_frame, prologue_cache);
  return trad_frame_get_register (info, this_frame, regnum);
}

/* Structure defining the RiscV normal frame unwind functions.  Since we
   are the fallback unwinder (DWARF unwinder is used first), we use the
   default frame sniffer, which always accepts the frame.  */

static const struct frame_unwind riscv_frame_unwind =
{
  /*.type          =*/ NORMAL_FRAME,
  /*.stop_reason   =*/ default_frame_unwind_stop_reason,
  /*.this_id       =*/ riscv_frame_this_id,
  /*.prev_register =*/ riscv_frame_prev_register,
  /*.unwind_data   =*/ NULL,
  /*.sniffer       =*/ default_frame_sniffer,
  /*.dealloc_cache =*/ NULL,
  /*.prev_arch     =*/ NULL,
};

/* Initialize the current architecture based on INFO.  If possible,
   re-use an architecture from ARCHES, which is a list of
   architectures already created during this debugging session.

   Called e.g. at program startup, when reading a core file, and when
   reading a binary file.  */

static struct gdbarch *
riscv_gdbarch_init (struct gdbarch_info info,
		    struct gdbarch_list *arches)
{
  struct gdbarch *gdbarch;
  struct gdbarch_tdep *tdep;
  struct gdbarch_tdep tmp_tdep;
  bool has_compressed_isa = false;
  int i;

  /* Ideally, we'd like to get as much information from the target for
     things like register size, and whether the target has floating point
     hardware.  However, there are some things that the target can't tell
     us, like, what ABI is being used.

     So, for now, we take as much information as possible from the ELF,
     including things like register size, and FP hardware support, along
     with information about the ABI.

     Information about this target is built up in TMP_TDEP, and then we
     look for an existing gdbarch in ARCHES that matches TMP_TDEP.  If no
     match is found we'll create a new gdbarch and copy TMP_TDEP over.  */
  memset (&tmp_tdep, 0, sizeof (tmp_tdep));

  if (info.abfd != NULL
      && bfd_get_flavour (info.abfd) == bfd_target_elf_flavour)
    {
      unsigned char eclass = elf_elfheader (info.abfd)->e_ident[EI_CLASS];
      int e_flags = elf_elfheader (info.abfd)->e_flags;

      if (eclass == ELFCLASS32)
	tmp_tdep.abi.fields.base_len = 1;
      else if (eclass == ELFCLASS64)
	tmp_tdep.abi.fields.base_len = 2;
      else
        internal_error (__FILE__, __LINE__,
			_("unknown ELF header class %d"), eclass);

      if (e_flags & EF_RISCV_RVC)
	{
	  has_compressed_isa = true;
	  tmp_tdep.core_features |= (1 << ('C' - 'A'));
	}

      if (e_flags & EF_RISCV_FLOAT_ABI_DOUBLE)
	{
	  tmp_tdep.abi.fields.float_abi = 2;
	  tmp_tdep.core_features |= (1 << ('D' - 'A'));
	  tmp_tdep.core_features |= (1 << ('F' - 'A'));
	}
      else if (e_flags & EF_RISCV_FLOAT_ABI_SINGLE)
	{
	  tmp_tdep.abi.fields.float_abi = 1;
	  tmp_tdep.core_features |= (1 << ('F' - 'A'));
	}
    }
  else
    {
      const struct bfd_arch_info *binfo = info.bfd_arch_info;

      if (binfo->bits_per_word == 32)
	tmp_tdep.abi.fields.base_len = 1;
      else if (binfo->bits_per_word == 64)
	tmp_tdep.abi.fields.base_len = 2;
      else
        internal_error (__FILE__, __LINE__, _("unknown bits_per_word %d"),
			binfo->bits_per_word);
    }

  /* Find a candidate among the list of pre-declared architectures.  */
  for (arches = gdbarch_list_lookup_by_info (arches, &info);
       arches != NULL;
       arches = gdbarch_list_lookup_by_info (arches->next, &info))
    if (gdbarch_tdep (arches->gdbarch)->abi.value == tmp_tdep.abi.value)
      return arches->gdbarch;

  /* None found, so create a new architecture from the information provided.  */
  tdep = (struct gdbarch_tdep *) xmalloc (sizeof *tdep);
  gdbarch = gdbarch_alloc (&info, tdep);
  memcpy (tdep, &tmp_tdep, sizeof (tmp_tdep));

  /* Target data types.  */
  set_gdbarch_short_bit (gdbarch, 16);
  set_gdbarch_int_bit (gdbarch, 32);
  set_gdbarch_long_bit (gdbarch, riscv_isa_xlen (gdbarch) * 8);
  set_gdbarch_long_long_bit (gdbarch, 64);
  set_gdbarch_float_bit (gdbarch, 32);
  set_gdbarch_double_bit (gdbarch, 64);
  set_gdbarch_long_double_bit (gdbarch, 128);
  set_gdbarch_long_double_format (gdbarch, floatformats_ia64_quad);
  set_gdbarch_ptr_bit (gdbarch, riscv_isa_xlen (gdbarch) * 8);
  set_gdbarch_char_signed (gdbarch, 0);

  /* Information about the target architecture.  */
  set_gdbarch_return_value (gdbarch, riscv_return_value);
  set_gdbarch_breakpoint_kind_from_pc (gdbarch, riscv_breakpoint_kind_from_pc);
  set_gdbarch_sw_breakpoint_from_kind (gdbarch, riscv_sw_breakpoint_from_kind);

  /* Register architecture.  */
  set_gdbarch_num_regs (gdbarch, RISCV_LAST_REGNUM + 1);
  set_gdbarch_sp_regnum (gdbarch, RISCV_SP_REGNUM);
  set_gdbarch_pc_regnum (gdbarch, RISCV_PC_REGNUM);
  set_gdbarch_ps_regnum (gdbarch, RISCV_FP_REGNUM);
  set_gdbarch_deprecated_fp_regnum (gdbarch, RISCV_FP_REGNUM);

  /* Functions to supply register information.  */
  set_gdbarch_register_name (gdbarch, riscv_register_name);
  set_gdbarch_register_type (gdbarch, riscv_register_type);
  set_gdbarch_print_registers_info (gdbarch, riscv_print_registers_info);
  set_gdbarch_register_reggroup_p (gdbarch, riscv_register_reggroup_p);

  /* Functions to analyze frames.  */
  set_gdbarch_decr_pc_after_break (gdbarch, (has_compressed_isa ? 2 : 4));
  set_gdbarch_skip_prologue (gdbarch, riscv_skip_prologue);
  set_gdbarch_inner_than (gdbarch, core_addr_lessthan);
  set_gdbarch_frame_align (gdbarch, riscv_frame_align);

  /* Functions to access frame data.  */
  set_gdbarch_unwind_pc (gdbarch, riscv_unwind_pc);
  set_gdbarch_unwind_sp (gdbarch, riscv_unwind_sp);

  /* Functions handling dummy frames.  */
  set_gdbarch_call_dummy_location (gdbarch, ON_STACK);
  set_gdbarch_push_dummy_code (gdbarch, riscv_push_dummy_code);
  set_gdbarch_push_dummy_call (gdbarch, riscv_push_dummy_call);
  set_gdbarch_dummy_id (gdbarch, riscv_dummy_id);

  /* Frame unwinders.  Use DWARF debug info if available, otherwise use our own
     unwinder.  */
  dwarf2_append_unwinders (gdbarch);
  frame_unwind_append_unwinder (gdbarch, &riscv_frame_unwind);

  for (i = 0; i < ARRAY_SIZE (riscv_register_aliases); ++i)
    user_reg_add (gdbarch, riscv_register_aliases[i].name,
		  value_of_riscv_user_reg, &riscv_register_aliases[i].regnum);

  return gdbarch;
}


/* Allocate new riscv_inferior_data object.  */

static struct riscv_inferior_data *
riscv_new_inferior_data (void)
{
  struct riscv_inferior_data *inf_data
    = new (struct riscv_inferior_data);
  inf_data->misa_read = false;
  return inf_data;
}

/* Free inferior data.  */

static void
riscv_inferior_data_cleanup (struct inferior *inf, void *data)
{
  struct riscv_inferior_data *inf_data =
    static_cast <struct riscv_inferior_data *> (data);
  delete (inf_data);
}

/* Return riscv_inferior_data for the given INFERIOR.  If not yet created,
   construct it.  */

struct riscv_inferior_data *
riscv_inferior_data (struct inferior *const inf)
{
  struct riscv_inferior_data *inf_data;

  gdb_assert (inf != NULL);

  inf_data
    = (struct riscv_inferior_data *) inferior_data (inf, riscv_inferior_data_reg);
  if (inf_data == NULL)
    {
      inf_data = riscv_new_inferior_data ();
      set_inferior_data (inf, riscv_inferior_data_reg, inf_data);
    }

  return inf_data;
}

/* Free the inferior data when an inferior exits.  */

static void
riscv_invalidate_inferior_data (struct inferior *inf)
{
  struct riscv_inferior_data *inf_data;

  gdb_assert (inf != NULL);

  /* Don't call RISCV_INFERIOR_DATA as we don't want to create the data if
     we've not already created it by this point.  */
  inf_data
    = (struct riscv_inferior_data *) inferior_data (inf, riscv_inferior_data_reg);
  if (inf_data != NULL)
    {
      delete (inf_data);
      set_inferior_data (inf, riscv_inferior_data_reg, NULL);
    }
}

void
_initialize_riscv_tdep (void)
{
  gdbarch_register (bfd_arch_riscv, riscv_gdbarch_init, NULL);

  /* Register per-inferior data.  */
  riscv_inferior_data_reg
    = register_inferior_data_with_cleanup (NULL, riscv_inferior_data_cleanup);

  /* Observers used to invalidate the inferior data when needed.  */
  gdb::observers::inferior_exit.attach (riscv_invalidate_inferior_data);
  gdb::observers::inferior_appeared.attach (riscv_invalidate_inferior_data);

  /* Add root prefix command for all "set debug riscv" and "show debug
     riscv" commands.  */
  add_prefix_cmd ("riscv", no_class, set_debug_riscv_command,
		  _("RISC-V specific debug commands."),
		  &setdebugriscvcmdlist, "set debug riscv ", 0,
		  &setdebuglist);

  add_prefix_cmd ("riscv", no_class, show_debug_riscv_command,
		  _("RISC-V specific debug commands."),
		  &showdebugriscvcmdlist, "show debug riscv ", 0,
		  &showdebuglist);

  add_setshow_zuinteger_cmd ("infcall", class_maintenance,
			     &riscv_debug_infcall,  _("\
Set riscv inferior call debugging."), _("\
Show riscv inferior call debugging."), _("\
When non-zero, print debugging information for the riscv specific parts\n\
of the inferior call mechanism."),
			     NULL,
			     show_riscv_debug_variable,
			     &setdebugriscvcmdlist, &showdebugriscvcmdlist);

  /* Add root prefix command for all "set riscv" and "show riscv" commands.  */
  add_prefix_cmd ("riscv", no_class, set_riscv_command,
		  _("RISC-V specific commands."),
		  &setriscvcmdlist, "set riscv ", 0, &setlist);

  add_prefix_cmd ("riscv", no_class, show_riscv_command,
		  _("RISC-V specific commands."),
		  &showriscvcmdlist, "show riscv ", 0, &showlist);


  use_compressed_breakpoints = AUTO_BOOLEAN_AUTO;
  add_setshow_auto_boolean_cmd ("use-compressed-breakpoints", no_class,
				&use_compressed_breakpoints,
				_("\
Set debugger's use of compressed breakpoints."), _("	\
Show debugger's use of compressed breakpoints."), _("\
Debugging compressed code requires compressed breakpoints to be used. If\n \
left to 'auto' then gdb will use them if $misa indicates the C extension\n \
is supported. If that doesn't give the correct behavior, then this option\n\
can be used."),
				NULL,
				show_use_compressed_breakpoints,
				&setriscvcmdlist,
				&showriscvcmdlist);
}
