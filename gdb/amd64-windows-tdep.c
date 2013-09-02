/* Copyright (C) 2009-2013 Free Software Foundation, Inc.

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
#include "osabi.h"
#include "amd64-tdep.h"
#include "solib.h"
#include "solib-target.h"
#include "gdbtypes.h"
#include "gdbcore.h"
#include "regcache.h"
#include "windows-tdep.h"
#include "frame.h"
#include "objfiles.h"
#include "frame-unwind.h"
#include "coff/internal.h"
#include "coff/i386.h"
#include "coff/pe.h"
#include "libcoff.h"

/* The registers used to pass integer arguments during a function call.  */
static int amd64_windows_dummy_call_integer_regs[] =
{
  AMD64_RCX_REGNUM,          /* %rcx */
  AMD64_RDX_REGNUM,          /* %rdx */
  8,                         /* %r8 */
  9                          /* %r9 */
};

/* Implement the "classify" method in the gdbarch_tdep structure
   for amd64-windows.  */

static void
amd64_windows_classify (struct type *type, enum amd64_reg_class class[2])
{
  switch (TYPE_CODE (type))
    {
      case TYPE_CODE_ARRAY:
	/* Arrays are always passed by memory.	*/
	class[0] = class[1] = AMD64_MEMORY;
	break;

      case TYPE_CODE_STRUCT:
      case TYPE_CODE_UNION:
        /* Struct/Union types whose size is 1, 2, 4, or 8 bytes
	   are passed as if they were integers of the same size.
	   Types of different sizes are passed by memory.  */
	if (TYPE_LENGTH (type) == 1
	    || TYPE_LENGTH (type) == 2
	    || TYPE_LENGTH (type) == 4
	    || TYPE_LENGTH (type) == 8)
	  {
	    class[0] = AMD64_INTEGER;
	    class[1] = AMD64_NO_CLASS;
	  }
	else
	  class[0] = class[1] = AMD64_MEMORY;
	break;

      default:
	/* For all the other types, the conventions are the same as
	   with the System V ABI.  */
	amd64_classify (type, class);
    }
}

/* Implement the "return_value" gdbarch method for amd64-windows.  */

static enum return_value_convention
amd64_windows_return_value (struct gdbarch *gdbarch, struct value *function,
			    struct type *type, struct regcache *regcache,
			    gdb_byte *readbuf, const gdb_byte *writebuf)
{
  int len = TYPE_LENGTH (type);
  int regnum = -1;

  /* See if our value is returned through a register.  If it is, then
     store the associated register number in REGNUM.  */
  switch (TYPE_CODE (type))
    {
      case TYPE_CODE_FLT:
      case TYPE_CODE_DECFLOAT:
        /* __m128, __m128i, __m128d, floats, and doubles are returned
           via XMM0.  */
        if (len == 4 || len == 8 || len == 16)
          regnum = AMD64_XMM0_REGNUM;
        break;
      default:
        /* All other values that are 1, 2, 4 or 8 bytes long are returned
           via RAX.  */
        if (len == 1 || len == 2 || len == 4 || len == 8)
          regnum = AMD64_RAX_REGNUM;
        break;
    }

  if (regnum < 0)
    {
      /* RAX contains the address where the return value has been stored.  */
      if (readbuf)
        {
	  ULONGEST addr;

	  regcache_raw_read_unsigned (regcache, AMD64_RAX_REGNUM, &addr);
	  read_memory (addr, readbuf, TYPE_LENGTH (type));
	}
      return RETURN_VALUE_ABI_RETURNS_ADDRESS;
    }
  else
    {
      /* Extract the return value from the register where it was stored.  */
      if (readbuf)
	regcache_raw_read_part (regcache, regnum, 0, len, readbuf);
      if (writebuf)
	regcache_raw_write_part (regcache, regnum, 0, len, writebuf);
      return RETURN_VALUE_REGISTER_CONVENTION;
    }
}

/* Check that the code pointed to by PC corresponds to a call to
   __main, skip it if so.  Return PC otherwise.  */

static CORE_ADDR
amd64_skip_main_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  gdb_byte op;

  target_read_memory (pc, &op, 1);
  if (op == 0xe8)
    {
      gdb_byte buf[4];

      if (target_read_memory (pc + 1, buf, sizeof buf) == 0)
 	{
 	  struct bound_minimal_symbol s;
 	  CORE_ADDR call_dest;

	  call_dest = pc + 5 + extract_signed_integer (buf, 4, byte_order);
 	  s = lookup_minimal_symbol_by_pc (call_dest);
 	  if (s.minsym != NULL
 	      && SYMBOL_LINKAGE_NAME (s.minsym) != NULL
 	      && strcmp (SYMBOL_LINKAGE_NAME (s.minsym), "__main") == 0)
 	    pc += 5;
 	}
    }

  return pc;
}

struct amd64_windows_frame_cache
{
  /* ImageBase for the module.  */
  CORE_ADDR image_base;

  /* Function start and end rva.  */
  CORE_ADDR start_rva;
  CORE_ADDR end_rva;

  /* Next instruction to be executed.  */
  CORE_ADDR pc;

  /* Current sp.  */
  CORE_ADDR sp;

  /* Address of saved integer and xmm registers.  */
  CORE_ADDR prev_reg_addr[16];
  CORE_ADDR prev_xmm_addr[16];

  /* These two next fields are set only for machine info frames.  */

  /* Likewise for RIP.  */
  CORE_ADDR prev_rip_addr;

  /* Likewise for RSP.  */
  CORE_ADDR prev_rsp_addr;

  /* Address of the previous frame.  */
  CORE_ADDR prev_sp;
};

/* Convert a Windows register number to gdb.  */
static const enum amd64_regnum amd64_windows_w2gdb_regnum[] =
{
  AMD64_RAX_REGNUM,
  AMD64_RCX_REGNUM,
  AMD64_RDX_REGNUM,
  AMD64_RBX_REGNUM,
  AMD64_RSP_REGNUM,
  AMD64_RBP_REGNUM,
  AMD64_RSI_REGNUM,
  AMD64_RDI_REGNUM,
  AMD64_R8_REGNUM,
  AMD64_R9_REGNUM,
  AMD64_R10_REGNUM,
  AMD64_R11_REGNUM,
  AMD64_R12_REGNUM,
  AMD64_R13_REGNUM,
  AMD64_R14_REGNUM,
  AMD64_R15_REGNUM
};

/* Return TRUE iff PC is the the range of the function corresponding to
   CACHE.  */

static int
pc_in_range (CORE_ADDR pc, const struct amd64_windows_frame_cache *cache)
{
  return (pc >= cache->image_base + cache->start_rva
	  && pc < cache->image_base + cache->end_rva);
}

/* Try to recognize and decode an epilogue sequence.

   Return -1 if we fail to read the instructions for any reason.
   Return 1 if an epilogue sequence was recognized, 0 otherwise.  */

static int
amd64_windows_frame_decode_epilogue (struct frame_info *this_frame,
				     struct amd64_windows_frame_cache *cache)
{
  /* According to MSDN an epilogue "must consist of either an add RSP,constant
     or lea RSP,constant[FPReg], followed by a series of zero or more 8-byte
     register pops and a return or a jmp".

     Furthermore, according to RtlVirtualUnwind, the complete list of
     epilog marker is:
     - ret                      [c3]
     - ret n                    [c2 imm16]
     - rep ret                  [f3 c3]
     - jmp imm8 | imm32         [eb rel8] or [e9 rel32]
     - jmp qword ptr imm32                 - not handled
     - rex.w jmp reg            [4X ff eY]
  */

  CORE_ADDR pc = cache->pc;
  CORE_ADDR cur_sp = cache->sp;
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  gdb_byte op;
  gdb_byte rex;

  /* We don't care about the instruction deallocating the frame:
     if it hasn't been executed, the pc is still in the body,
     if it has been executed, the following epilog decoding will work.  */

  /* First decode:
     -  pop reg                 [41 58-5f] or [58-5f].  */

  while (1)
    {
      /* Read opcode. */
      if (target_read_memory (pc, &op, 1) != 0)
	return -1;

      if (op >= 0x40 && op <= 0x4f)
	{
	  /* REX prefix.  */
	  rex = op;

	  /* Read opcode. */
	  if (target_read_memory (pc + 1, &op, 1) != 0)
	    return -1;
	}
      else
	rex = 0;

      if (op >= 0x58 && op <= 0x5f)
	{
	  /* pop reg  */
	  gdb_byte reg = (op & 0x0f) | ((rex & 1) << 3);

	  cache->prev_reg_addr[amd64_windows_w2gdb_regnum[reg]] = cur_sp;
	  cur_sp += 8;
	}
      else
	break;

      /* Allow the user to break this loop.  This shouldn't happen as the
	 number of consecutive pop should be small.  */
      QUIT;
    }

  /* Then decode the marker.  */

  /* Read opcode.  */
  if (target_read_memory (pc, &op, 1) != 0)
    return -1;

  switch (op)
    {
    case 0xc3:
      /* Ret.  */
      cache->prev_rip_addr = cur_sp;
      cache->prev_sp = cur_sp + 8;
      return 1;

    case 0xeb:
      {
	/* jmp rel8  */
	gdb_byte rel8;
	CORE_ADDR npc;

	if (target_read_memory (pc + 1, &rel8, 1) != 0)
	  return -1;
	npc = pc + 2 + (signed char) rel8;

	/* If the jump is within the function, then this is not a marker,
	   otherwise this is a tail-call.  */
	return !pc_in_range (npc, cache);
      }

    case 0xec:
      {
	/* jmp rel32  */
	gdb_byte rel32[4];
	CORE_ADDR npc;

	if (target_read_memory (pc + 1, rel32, 4) != 0)
	  return -1;
	npc = pc + 5 + extract_signed_integer (rel32, 4, byte_order);

	/* If the jump is within the function, then this is not a marker,
	   otherwise this is a tail-call.  */
	return !pc_in_range (npc, cache);
      }

    case 0xc2:
      {
	/* ret n  */
	gdb_byte imm16[2];

	if (target_read_memory (pc + 1, imm16, 2) != 0)
	  return -1;
	cache->prev_rip_addr = cur_sp;
	cache->prev_sp = cur_sp
	  + extract_unsigned_integer (imm16, 4, byte_order);
	return 1;
      }

    case 0xf3:
      {
	/* rep; ret  */
	gdb_byte op1;

	if (target_read_memory (pc + 2, &op1, 1) != 0)
	  return -1;
	if (op1 != 0xc3)
	  return 0;

	cache->prev_rip_addr = cur_sp;
	cache->prev_sp = cur_sp + 8;
	return 1;
      }

    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4a:
    case 0x4b:
    case 0x4c:
    case 0x4d:
    case 0x4e:
    case 0x4f:
      /* Got a REX prefix, read next byte.  */
      rex = op;
      if (target_read_memory (pc + 1, &op, 1) != 0)
	return -1;

      if (op == 0xff)
	{
	  /* rex jmp reg  */
	  gdb_byte op1;
	  unsigned int reg;
	  gdb_byte buf[8];

	  if (target_read_memory (pc + 2, &op1, 1) != 0)
	    return -1;
	  return (op1 & 0xf8) == 0xe0;
	}
      else
	return 0;

    default:
      /* Not REX, so unknown.  */
      return 0;
    }
}

/* Decode and execute unwind insns at UNWIND_INFO.  */

static void
amd64_windows_frame_decode_insns (struct frame_info *this_frame,
				  struct amd64_windows_frame_cache *cache,
				  CORE_ADDR unwind_info)
{
  CORE_ADDR save_addr = 0;
  CORE_ADDR cur_sp = cache->sp;
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  int j;

  for (j = 0; ; j++)
    {
      struct external_pex64_unwind_info ex_ui;
      /* There are at most 256 16-bit unwind insns.  */
      gdb_byte insns[2 * 256];
      gdb_byte *p;
      gdb_byte *end_insns;
      unsigned char codes_count;
      unsigned char frame_reg;
      unsigned char frame_off;

      /* Read and decode header.  */
      if (target_read_memory (cache->image_base + unwind_info,
			      (gdb_byte *) &ex_ui, sizeof (ex_ui)) != 0)
	return;

      if (frame_debug)
	fprintf_unfiltered
	  (gdb_stdlog,
	   "amd64_windows_frame_decodes_insn: "
	   "%s: ver: %02x, plgsz: %02x, cnt: %02x, frame: %02x\n",
	   paddress (gdbarch, unwind_info),
	   ex_ui.Version_Flags, ex_ui.SizeOfPrologue,
	   ex_ui.CountOfCodes, ex_ui.FrameRegisterOffset);

      /* Check version.  */
      if (PEX64_UWI_VERSION (ex_ui.Version_Flags) != 1)
	return;

      if (j == 0
	  && (cache->pc >=
	      cache->image_base + cache->start_rva + ex_ui.SizeOfPrologue))
	{
	  /* Not in the prologue.  We want to detect if the PC points to an
	     epilogue. If so, the epilogue detection+decoding function is
	     sufficient.  Otherwise, the unwinder will consider that the PC
	     is in the body of the function and will need to decode unwind
	     info.  */
	  if (amd64_windows_frame_decode_epilogue (this_frame, cache) == 1)
	    return;

	  /* Not in an epilog.  Clear possible side effects.  */
	  memset (cache->prev_reg_addr, 0, sizeof (cache->prev_reg_addr));
	}

      codes_count = ex_ui.CountOfCodes;
      frame_reg = PEX64_UWI_FRAMEREG (ex_ui.FrameRegisterOffset);

      if (frame_reg != 0)
	{
	  /* According to msdn:
	     If an FP reg is used, then any unwind code taking an offset must
	     only be used after the FP reg is established in the prolog.  */
	  gdb_byte buf[8];
	  int frreg = amd64_windows_w2gdb_regnum[frame_reg];

	  get_frame_register (this_frame, frreg, buf);
	  save_addr = extract_unsigned_integer (buf, 8, byte_order);

	  if (frame_debug)
	    fprintf_unfiltered (gdb_stdlog, "   frame_reg=%s, val=%s\n",
				gdbarch_register_name (gdbarch, frreg),
				paddress (gdbarch, save_addr));
	}

      /* Read opcodes.  */
      if (codes_count != 0
	  && target_read_memory (cache->image_base + unwind_info
				 + sizeof (ex_ui),
				 insns, codes_count * 2) != 0)
	return;

      end_insns = &insns[codes_count * 2];
      for (p = insns; p < end_insns; p += 2)
	{
	  int reg;

	  if (frame_debug)
	    fprintf_unfiltered
	      (gdb_stdlog, "   op #%u: off=0x%02x, insn=0x%02x\n",
	       (unsigned) (p - insns), p[0], p[1]);

	  /* Virtually execute the operation.  */
	  if (cache->pc >= cache->image_base + cache->start_rva + p[0])
	    {
	      /* If there is no frame registers defined, the current value of
		 rsp is used instead.  */
	      if (frame_reg == 0)
		save_addr = cur_sp;

	      switch (PEX64_UNWCODE_CODE (p[1]))
		{
		case UWOP_PUSH_NONVOL:
		  /* Push pre-decrements RSP.  */
		  reg = amd64_windows_w2gdb_regnum[PEX64_UNWCODE_INFO (p[1])];
		  cache->prev_reg_addr[reg] = cur_sp;
		  cur_sp += 8;
		  break;
		case UWOP_ALLOC_LARGE:
		  if (PEX64_UNWCODE_INFO (p[1]) == 0)
		    cur_sp +=
		      8 * extract_unsigned_integer (p + 2, 2, byte_order);
		  else if (PEX64_UNWCODE_INFO (p[1]) == 1)
		    cur_sp += extract_unsigned_integer (p + 2, 4, byte_order);
		  else
		    return;
		  break;
		case UWOP_ALLOC_SMALL:
		  cur_sp += 8 + 8 * PEX64_UNWCODE_INFO (p[1]);
		  break;
		case UWOP_SET_FPREG:
		  cur_sp = save_addr
		    - PEX64_UWI_FRAMEOFF (ex_ui.FrameRegisterOffset) * 16;
		  break;
		case UWOP_SAVE_NONVOL:
		  reg = amd64_windows_w2gdb_regnum[PEX64_UNWCODE_INFO (p[1])];
		  cache->prev_reg_addr[reg] = save_addr
		    - 8 * extract_unsigned_integer (p + 2, 2, byte_order);
		  break;
		case UWOP_SAVE_NONVOL_FAR:
		  reg = amd64_windows_w2gdb_regnum[PEX64_UNWCODE_INFO (p[1])];
		  cache->prev_reg_addr[reg] = save_addr
		    - 8 * extract_unsigned_integer (p + 2, 4, byte_order);
		  break;
		case UWOP_SAVE_XMM128:
		  cache->prev_xmm_addr[PEX64_UNWCODE_INFO (p[1])] =
		    save_addr
		    - 16 * extract_unsigned_integer (p + 2, 2, byte_order);
		  break;
		case UWOP_SAVE_XMM128_FAR:
		  cache->prev_xmm_addr[PEX64_UNWCODE_INFO (p[1])] =
		    save_addr
		    - 16 * extract_unsigned_integer (p + 2, 4, byte_order);
		  break;
		case UWOP_PUSH_MACHFRAME:
		  if (PEX64_UNWCODE_INFO (p[1]) == 0)
		    {
		      cache->prev_rip_addr = cur_sp + 0;
		      cache->prev_rsp_addr = cur_sp + 24;
		      cur_sp += 40;
		    }
		  else if (PEX64_UNWCODE_INFO (p[1]) == 1)
		    {
		      cache->prev_rip_addr = cur_sp + 8;
		      cache->prev_rsp_addr = cur_sp + 32;
		      cur_sp += 48;
		    }
		  else
		    return;
		  break;
		default:
		  return;
		}
	    }

	  /* Adjust with the length of the opcode.  */
	  switch (PEX64_UNWCODE_CODE (p[1]))
	    {
	    case UWOP_PUSH_NONVOL:
	    case UWOP_ALLOC_SMALL:
	    case UWOP_SET_FPREG:
	    case UWOP_PUSH_MACHFRAME:
	      break;
	    case UWOP_ALLOC_LARGE:
	      if (PEX64_UNWCODE_INFO (p[1]) == 0)
		p += 2;
	      else if (PEX64_UNWCODE_INFO (p[1]) == 1)
		p += 4;
	      else
		return;
	      break;
	    case UWOP_SAVE_NONVOL:
	    case UWOP_SAVE_XMM128:
	      p += 2;
	      break;
	    case UWOP_SAVE_NONVOL_FAR:
	    case UWOP_SAVE_XMM128_FAR:
	      p += 4;
	      break;
	    default:
	      return;
	    }
	}
      if (PEX64_UWI_FLAGS (ex_ui.Version_Flags) != UNW_FLAG_CHAININFO)
	break;
      else
	{
	  /* Read the chained unwind info.  */
	  struct external_pex64_runtime_function d;
	  CORE_ADDR chain_vma;

	  chain_vma = cache->image_base + unwind_info
	    + sizeof (ex_ui) + ((codes_count + 1) & ~1) * 2 + 8;

	  if (target_read_memory (chain_vma, (gdb_byte *) &d, sizeof (d)) != 0)
	    return;

	  cache->start_rva =
	    extract_unsigned_integer (d.rva_BeginAddress, 4, byte_order);
	  cache->end_rva =
	    extract_unsigned_integer (d.rva_EndAddress, 4, byte_order);
	  unwind_info =
	    extract_unsigned_integer (d.rva_UnwindData, 4, byte_order);
	}

      /* Allow the user to break this loop.  */
      QUIT;
    }
  /* PC is saved by the call.  */
  if (cache->prev_rip_addr == 0)
    cache->prev_rip_addr = cur_sp;
  cache->prev_sp = cur_sp + 8;

  if (frame_debug)
    fprintf_unfiltered (gdb_stdlog, "   prev_sp: %s, prev_pc @%s\n",
			paddress (gdbarch, cache->prev_sp),
			paddress (gdbarch, cache->prev_rip_addr));
}

/* Find SEH unwind info for PC, returning 0 on success.

   UNWIND_INFO is set to the rva of unwind info address, IMAGE_BASE
   to the base address of the corresponding image, and START_RVA
   to the rva of the function containing PC.  */

static int
amd64_windows_find_unwind_info (struct gdbarch *gdbarch, CORE_ADDR pc,
				CORE_ADDR *unwind_info,
				CORE_ADDR *image_base,
				CORE_ADDR *start_rva,
				CORE_ADDR *end_rva)
{
  struct obj_section *sec;
  pe_data_type *pe;
  IMAGE_DATA_DIRECTORY *dir;
  struct objfile *objfile;
  unsigned long lo, hi;
  CORE_ADDR base;
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);

  /* Get the corresponding exception directory.  */
  sec = find_pc_section (pc);
  if (sec == NULL)
    return -1;
  objfile = sec->objfile;
  pe = pe_data (sec->objfile->obfd);
  dir = &pe->pe_opthdr.DataDirectory[PE_EXCEPTION_TABLE];

  base = pe->pe_opthdr.ImageBase
    + ANOFFSET (objfile->section_offsets, SECT_OFF_TEXT (objfile));
  *image_base = base;

  /* Find the entry.

     Note: This does not handle dynamically added entries (for JIT
     engines).  For this, we would need to ask the kernel directly,
     which means getting some info from the native layer.  For the
     rest of the code, however, it's probably faster to search
     the entry ourselves.  */
  lo = 0;
  hi = dir->Size / sizeof (struct external_pex64_runtime_function);
  *unwind_info = 0;
  while (lo <= hi)
    {
      unsigned long mid = lo + (hi - lo) / 2;
      struct external_pex64_runtime_function d;
      CORE_ADDR sa, ea;

      if (target_read_memory (base + dir->VirtualAddress + mid * sizeof (d),
			      (gdb_byte *) &d, sizeof (d)) != 0)
	return -1;

      sa = extract_unsigned_integer (d.rva_BeginAddress, 4, byte_order);
      ea = extract_unsigned_integer (d.rva_EndAddress, 4, byte_order);
      if (pc < base + sa)
	hi = mid - 1;
      else if (pc >= base + ea)
	lo = mid + 1;
      else if (pc >= base + sa && pc < base + ea)
	{
	  /* Got it.  */
	  *start_rva = sa;
	  *end_rva = ea;
	  *unwind_info =
	    extract_unsigned_integer (d.rva_UnwindData, 4, byte_order);
	  break;
	}
      else
	break;
    }

  if (frame_debug)
    fprintf_unfiltered
      (gdb_stdlog,
       "amd64_windows_find_unwind_data:  image_base=%s, unwind_data=%s\n",
       paddress (gdbarch, base), paddress (gdbarch, *unwind_info));

  if (*unwind_info & 1)
    {
      /* Unofficially documented unwind info redirection, when UNWIND_INFO
	 address is odd (http://www.codemachine.com/article_x64deepdive.html).
      */
      struct external_pex64_runtime_function d;
      CORE_ADDR sa, ea;

      if (target_read_memory (base + (*unwind_info & ~1),
			      (gdb_byte *) &d, sizeof (d)) != 0)
	return -1;

      *start_rva =
	extract_unsigned_integer (d.rva_BeginAddress, 4, byte_order);
      *end_rva = extract_unsigned_integer (d.rva_EndAddress, 4, byte_order);
      *unwind_info =
	extract_unsigned_integer (d.rva_UnwindData, 4, byte_order);

    }
  return 0;
}

/* Fill THIS_CACHE using the native amd64-windows unwinding data
   for THIS_FRAME.  */

static struct amd64_windows_frame_cache *
amd64_windows_frame_cache (struct frame_info *this_frame, void **this_cache)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  struct amd64_windows_frame_cache *cache;
  gdb_byte buf[8];
  struct obj_section *sec;
  pe_data_type *pe;
  IMAGE_DATA_DIRECTORY *dir;
  CORE_ADDR image_base;
  CORE_ADDR pc;
  struct objfile *objfile;
  unsigned long lo, hi;
  CORE_ADDR unwind_info = 0;

  if (*this_cache)
    return *this_cache;

  cache = FRAME_OBSTACK_ZALLOC (struct amd64_windows_frame_cache);
  *this_cache = cache;

  /* Get current PC and SP.  */
  pc = get_frame_pc (this_frame);
  get_frame_register (this_frame, AMD64_RSP_REGNUM, buf);
  cache->sp = extract_unsigned_integer (buf, 8, byte_order);
  cache->pc = pc;

  if (amd64_windows_find_unwind_info (gdbarch, pc, &unwind_info,
				      &cache->image_base,
				      &cache->start_rva,
				      &cache->end_rva))
    return cache;

  if (unwind_info == 0)
    {
      /* Assume a leaf function.  */
      cache->prev_sp = cache->sp + 8;
      cache->prev_rip_addr = cache->sp;
    }
  else
    {
      /* Decode unwind insns to compute saved addresses.  */
      amd64_windows_frame_decode_insns (this_frame, cache, unwind_info);
    }
  return cache;
}

/* Implement the "prev_register" method of struct frame_unwind
   using the standard Windows x64 SEH info.  */

static struct value *
amd64_windows_frame_prev_register (struct frame_info *this_frame,
				   void **this_cache, int regnum)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  struct amd64_windows_frame_cache *cache =
    amd64_windows_frame_cache (this_frame, this_cache);
  struct value *val;
  CORE_ADDR prev;

  if (frame_debug)
    fprintf_unfiltered (gdb_stdlog,
			"amd64_windows_frame_prev_register %s for sp=%s\n",
			gdbarch_register_name (gdbarch, regnum),
			paddress (gdbarch, cache->prev_sp));

  if (regnum >= AMD64_XMM0_REGNUM && regnum <= AMD64_XMM0_REGNUM + 15)
      prev = cache->prev_xmm_addr[regnum - AMD64_XMM0_REGNUM];
  else if (regnum == AMD64_RSP_REGNUM)
    {
      prev = cache->prev_rsp_addr;
      if (prev == 0)
	return frame_unwind_got_constant (this_frame, regnum, cache->prev_sp);
    }
  else if (regnum >= AMD64_RAX_REGNUM && regnum <= AMD64_R15_REGNUM)
    prev = cache->prev_reg_addr[regnum - AMD64_RAX_REGNUM];
  else if (regnum == AMD64_RIP_REGNUM)
    prev = cache->prev_rip_addr;
  else
    prev = 0;

  if (prev && frame_debug)
    fprintf_unfiltered (gdb_stdlog, "  -> at %s\n", paddress (gdbarch, prev));

  if (prev)
    {
      /* Register was saved.  */
      return frame_unwind_got_memory (this_frame, regnum, prev);
    }
  else
    {
      /* Register is either volatile or not modified.  */
      return frame_unwind_got_register (this_frame, regnum, regnum);
    }
}

/* Implement the "this_id" method of struct frame_unwind using
   the standard Windows x64 SEH info.  */

static void
amd64_windows_frame_this_id (struct frame_info *this_frame, void **this_cache,
		   struct frame_id *this_id)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  struct amd64_windows_frame_cache *cache =
    amd64_windows_frame_cache (this_frame, this_cache);

  *this_id = frame_id_build (cache->prev_sp,
			     cache->image_base + cache->start_rva);
}

/* Windows x64 SEH unwinder.  */

static const struct frame_unwind amd64_windows_frame_unwind =
{
  NORMAL_FRAME,
  default_frame_unwind_stop_reason,
  &amd64_windows_frame_this_id,
  &amd64_windows_frame_prev_register,
  NULL,
  default_frame_sniffer
};

/* Implement the "skip_prologue" gdbarch method.  */

static CORE_ADDR
amd64_windows_skip_prologue (struct gdbarch *gdbarch, CORE_ADDR pc)
{
  CORE_ADDR func_addr;
  CORE_ADDR unwind_info = 0;
  CORE_ADDR image_base, start_rva, end_rva;
  struct external_pex64_unwind_info ex_ui;

  /* Use prologue size from unwind info.  */
  if (amd64_windows_find_unwind_info (gdbarch, pc, &unwind_info,
				      &image_base, &start_rva, &end_rva) == 0)
    {
      if (unwind_info == 0)
	{
	  /* Leaf function.  */
	  return pc;
	}
      else if (target_read_memory (image_base + unwind_info,
				   (gdb_byte *) &ex_ui, sizeof (ex_ui)) == 0
	       && PEX64_UWI_VERSION (ex_ui.Version_Flags) == 1)
	return max (pc, image_base + start_rva + ex_ui.SizeOfPrologue);
    }

  /* See if we can determine the end of the prologue via the symbol
     table.  If so, then return either the PC, or the PC after
     the prologue, whichever is greater.  */
  if (find_pc_partial_function (pc, NULL, &func_addr, NULL))
    {
      CORE_ADDR post_prologue_pc
	= skip_prologue_using_sal (gdbarch, func_addr);

      if (post_prologue_pc != 0)
	return max (pc, post_prologue_pc);
    }

  return pc;
}

/* Check Win64 DLL jmp trampolines and find jump destination.  */

static CORE_ADDR
amd64_windows_skip_trampoline_code (struct frame_info *frame, CORE_ADDR pc)
{
  CORE_ADDR destination = 0;
  struct gdbarch *gdbarch = get_frame_arch (frame);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);

  /* Check for jmp *<offset>(%rip) (jump near, absolute indirect (/4)).  */
  if (pc && read_memory_unsigned_integer (pc, 2, byte_order) == 0x25ff)
    {
      /* Get opcode offset and see if we can find a reference in our data.  */
      ULONGEST offset
	= read_memory_unsigned_integer (pc + 2, 4, byte_order);

      /* Get address of function pointer at end of pc.  */
      CORE_ADDR indirect_addr = pc + offset + 6;

      struct minimal_symbol *indsym
	= (indirect_addr
	   ? lookup_minimal_symbol_by_pc (indirect_addr).minsym
	   : NULL);
      const char *symname = indsym ? SYMBOL_LINKAGE_NAME (indsym) : NULL;

      if (symname)
	{
	  if (strncmp (symname, "__imp_", 6) == 0
	      || strncmp (symname, "_imp_", 5) == 0)
	    destination
	      = read_memory_unsigned_integer (indirect_addr, 8, byte_order);
	}
    }

  return destination;
}

/* Implement the "auto_wide_charset" gdbarch method.  */

static const char *
amd64_windows_auto_wide_charset (void)
{
  return "UTF-16";
}

static void
amd64_windows_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* The dwarf2 unwinder (appended very early by i386_gdbarch_init) is
     preferred over the SEH one.  The reasons are:
     - binaries without SEH but with dwarf2 debug info are correcly handled
       (although they aren't ABI compliant, gcc before 4.7 didn't emit SEH
       info).
     - dwarf3 DW_OP_call_frame_cfa is correctly handled (it can only be
       handled if the dwarf2 unwinder is used).

    The call to amd64_init_abi appends default unwinders, that aren't
    compatible with the SEH one.
  */
  frame_unwind_append_unwinder (gdbarch, &amd64_windows_frame_unwind);

  amd64_init_abi (info, gdbarch);

  /* On Windows, "long"s are only 32bit.  */
  set_gdbarch_long_bit (gdbarch, 32);

  /* Function calls.  */
  tdep->call_dummy_num_integer_regs =
    ARRAY_SIZE (amd64_windows_dummy_call_integer_regs);
  tdep->call_dummy_integer_regs = amd64_windows_dummy_call_integer_regs;
  tdep->classify = amd64_windows_classify;
  tdep->memory_args_by_pointer = 1;
  tdep->integer_param_regs_saved_in_caller_frame = 1;
  set_gdbarch_return_value (gdbarch, amd64_windows_return_value);
  set_gdbarch_skip_main_prologue (gdbarch, amd64_skip_main_prologue);
  set_gdbarch_skip_trampoline_code (gdbarch,
				    amd64_windows_skip_trampoline_code);

  set_gdbarch_iterate_over_objfiles_in_search_order
    (gdbarch, windows_iterate_over_objfiles_in_search_order);

  set_gdbarch_skip_prologue (gdbarch, amd64_windows_skip_prologue);

  set_gdbarch_auto_wide_charset (gdbarch, amd64_windows_auto_wide_charset);

  set_solib_ops (gdbarch, &solib_target_so_ops);
}

/* -Wmissing-prototypes */
extern initialize_file_ftype _initialize_amd64_windows_tdep;

void
_initialize_amd64_windows_tdep (void)
{
  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_x86_64, GDB_OSABI_CYGWIN,
                          amd64_windows_init_abi);
}
