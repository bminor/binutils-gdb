/* GNU/Linux/PowerPC specific low level interface, for the remote server for
   GDB.
   Copyright (C) 1995-2016 Free Software Foundation, Inc.

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

#include "server.h"
#include "linux-low.h"

#include <elf.h>
#include <asm/ptrace.h>

#include "nat/ppc-linux.h"
#include "linux-ppc-tdesc.h"
#include "ax.h"
#include "tracepoint.h"

#define PPC_FIELD(value, from, len) \
	(((value) >> (32 - (from) - (len))) & ((1 << (len)) - 1))
#define PPC_SEXT(v, bs) \
	((((CORE_ADDR) (v) & (((CORE_ADDR) 1 << (bs)) - 1)) \
	  ^ ((CORE_ADDR) 1 << ((bs) - 1))) \
	 - ((CORE_ADDR) 1 << ((bs) - 1)))
#define PPC_OP6(insn)	PPC_FIELD (insn, 0, 6)
#define PPC_BO(insn)	PPC_FIELD (insn, 6, 5)
#define PPC_LI(insn)	(PPC_SEXT (PPC_FIELD (insn, 6, 24), 24) << 2)
#define PPC_BD(insn)	(PPC_SEXT (PPC_FIELD (insn, 16, 14), 14) << 2)

static unsigned long ppc_hwcap;


#define ppc_num_regs 73

#ifdef __powerpc64__
/* We use a constant for FPSCR instead of PT_FPSCR, because
   many shipped PPC64 kernels had the wrong value in ptrace.h.  */
static int ppc_regmap[] =
 {PT_R0 * 8,     PT_R1 * 8,     PT_R2 * 8,     PT_R3 * 8,
  PT_R4 * 8,     PT_R5 * 8,     PT_R6 * 8,     PT_R7 * 8,
  PT_R8 * 8,     PT_R9 * 8,     PT_R10 * 8,    PT_R11 * 8,
  PT_R12 * 8,    PT_R13 * 8,    PT_R14 * 8,    PT_R15 * 8,
  PT_R16 * 8,    PT_R17 * 8,    PT_R18 * 8,    PT_R19 * 8,
  PT_R20 * 8,    PT_R21 * 8,    PT_R22 * 8,    PT_R23 * 8,
  PT_R24 * 8,    PT_R25 * 8,    PT_R26 * 8,    PT_R27 * 8,
  PT_R28 * 8,    PT_R29 * 8,    PT_R30 * 8,    PT_R31 * 8,
  PT_FPR0*8,     PT_FPR0*8 + 8, PT_FPR0*8+16,  PT_FPR0*8+24,
  PT_FPR0*8+32,  PT_FPR0*8+40,  PT_FPR0*8+48,  PT_FPR0*8+56,
  PT_FPR0*8+64,  PT_FPR0*8+72,  PT_FPR0*8+80,  PT_FPR0*8+88,
  PT_FPR0*8+96,  PT_FPR0*8+104,  PT_FPR0*8+112,  PT_FPR0*8+120,
  PT_FPR0*8+128, PT_FPR0*8+136,  PT_FPR0*8+144,  PT_FPR0*8+152,
  PT_FPR0*8+160,  PT_FPR0*8+168,  PT_FPR0*8+176,  PT_FPR0*8+184,
  PT_FPR0*8+192,  PT_FPR0*8+200,  PT_FPR0*8+208,  PT_FPR0*8+216,
  PT_FPR0*8+224,  PT_FPR0*8+232,  PT_FPR0*8+240,  PT_FPR0*8+248,
  PT_NIP * 8,    PT_MSR * 8,    PT_CCR * 8,    PT_LNK * 8,
  PT_CTR * 8,    PT_XER * 8,    PT_FPR0*8 + 256,
  PT_ORIG_R3 * 8, PT_TRAP * 8 };
#else
/* Currently, don't check/send MQ.  */
static int ppc_regmap[] =
 {PT_R0 * 4,     PT_R1 * 4,     PT_R2 * 4,     PT_R3 * 4,
  PT_R4 * 4,     PT_R5 * 4,     PT_R6 * 4,     PT_R7 * 4,
  PT_R8 * 4,     PT_R9 * 4,     PT_R10 * 4,    PT_R11 * 4,
  PT_R12 * 4,    PT_R13 * 4,    PT_R14 * 4,    PT_R15 * 4,
  PT_R16 * 4,    PT_R17 * 4,    PT_R18 * 4,    PT_R19 * 4,
  PT_R20 * 4,    PT_R21 * 4,    PT_R22 * 4,    PT_R23 * 4,
  PT_R24 * 4,    PT_R25 * 4,    PT_R26 * 4,    PT_R27 * 4,
  PT_R28 * 4,    PT_R29 * 4,    PT_R30 * 4,    PT_R31 * 4,
  PT_FPR0*4,     PT_FPR0*4 + 8, PT_FPR0*4+16,  PT_FPR0*4+24,
  PT_FPR0*4+32,  PT_FPR0*4+40,  PT_FPR0*4+48,  PT_FPR0*4+56,
  PT_FPR0*4+64,  PT_FPR0*4+72,  PT_FPR0*4+80,  PT_FPR0*4+88,
  PT_FPR0*4+96,  PT_FPR0*4+104,  PT_FPR0*4+112,  PT_FPR0*4+120,
  PT_FPR0*4+128, PT_FPR0*4+136,  PT_FPR0*4+144,  PT_FPR0*4+152,
  PT_FPR0*4+160,  PT_FPR0*4+168,  PT_FPR0*4+176,  PT_FPR0*4+184,
  PT_FPR0*4+192,  PT_FPR0*4+200,  PT_FPR0*4+208,  PT_FPR0*4+216,
  PT_FPR0*4+224,  PT_FPR0*4+232,  PT_FPR0*4+240,  PT_FPR0*4+248,
  PT_NIP * 4,    PT_MSR * 4,    PT_CCR * 4,    PT_LNK * 4,
  PT_CTR * 4,    PT_XER * 4,    PT_FPSCR * 4,
  PT_ORIG_R3 * 4, PT_TRAP * 4
 };

static int ppc_regmap_e500[] =
 {PT_R0 * 4,     PT_R1 * 4,     PT_R2 * 4,     PT_R3 * 4,
  PT_R4 * 4,     PT_R5 * 4,     PT_R6 * 4,     PT_R7 * 4,
  PT_R8 * 4,     PT_R9 * 4,     PT_R10 * 4,    PT_R11 * 4,
  PT_R12 * 4,    PT_R13 * 4,    PT_R14 * 4,    PT_R15 * 4,
  PT_R16 * 4,    PT_R17 * 4,    PT_R18 * 4,    PT_R19 * 4,
  PT_R20 * 4,    PT_R21 * 4,    PT_R22 * 4,    PT_R23 * 4,
  PT_R24 * 4,    PT_R25 * 4,    PT_R26 * 4,    PT_R27 * 4,
  PT_R28 * 4,    PT_R29 * 4,    PT_R30 * 4,    PT_R31 * 4,
  -1,            -1,            -1,            -1,
  -1,            -1,            -1,            -1,
  -1,            -1,            -1,            -1,
  -1,            -1,            -1,            -1,
  -1,            -1,            -1,            -1,
  -1,            -1,            -1,            -1,
  -1,            -1,            -1,            -1,
  -1,            -1,            -1,            -1,
  PT_NIP * 4,    PT_MSR * 4,    PT_CCR * 4,    PT_LNK * 4,
  PT_CTR * 4,    PT_XER * 4,    -1,
  PT_ORIG_R3 * 4, PT_TRAP * 4
 };
#endif

static int
ppc_cannot_store_register (int regno)
{
  const struct target_desc *tdesc = current_process ()->tdesc;

#ifndef __powerpc64__
  /* Some kernels do not allow us to store fpscr.  */
  if (!(ppc_hwcap & PPC_FEATURE_HAS_SPE)
      && regno == find_regno (tdesc, "fpscr"))
    return 2;
#endif

  /* Some kernels do not allow us to store orig_r3 or trap.  */
  if (regno == find_regno (tdesc, "orig_r3")
      || regno == find_regno (tdesc, "trap"))
    return 2;

  return 0;
}

static int
ppc_cannot_fetch_register (int regno)
{
  return 0;
}

static void
ppc_collect_ptrace_register (struct regcache *regcache, int regno, char *buf)
{
  memset (buf, 0, sizeof (long));

  if (__BYTE_ORDER == __LITTLE_ENDIAN)
    {
      /* Little-endian values always sit at the left end of the buffer.  */
      collect_register (regcache, regno, buf);
    }
  else if (__BYTE_ORDER == __BIG_ENDIAN)
    {
      /* Big-endian values sit at the right end of the buffer.  In case of
         registers whose sizes are smaller than sizeof (long), we must use a
         padding to access them correctly.  */
      int size = register_size (regcache->tdesc, regno);

      if (size < sizeof (long))
	collect_register (regcache, regno, buf + sizeof (long) - size);
      else
	collect_register (regcache, regno, buf);
    }
  else
    perror_with_name ("Unexpected byte order");
}

static void
ppc_supply_ptrace_register (struct regcache *regcache,
			    int regno, const char *buf)
{
  if (__BYTE_ORDER == __LITTLE_ENDIAN)
    {
      /* Little-endian values always sit at the left end of the buffer.  */
      supply_register (regcache, regno, buf);
    }
  else if (__BYTE_ORDER == __BIG_ENDIAN)
    {
      /* Big-endian values sit at the right end of the buffer.  In case of
         registers whose sizes are smaller than sizeof (long), we must use a
         padding to access them correctly.  */
      int size = register_size (regcache->tdesc, regno);

      if (size < sizeof (long))
	supply_register (regcache, regno, buf + sizeof (long) - size);
      else
	supply_register (regcache, regno, buf);
    }
  else
    perror_with_name ("Unexpected byte order");
}


#define INSTR_SC        0x44000002
#define NR_spu_run      0x0116

/* If the PPU thread is currently stopped on a spu_run system call,
   return to FD and ADDR the file handle and NPC parameter address
   used with the system call.  Return non-zero if successful.  */
static int
parse_spufs_run (struct regcache *regcache, int *fd, CORE_ADDR *addr)
{
  CORE_ADDR curr_pc;
  int curr_insn;
  int curr_r0;

  if (register_size (regcache->tdesc, 0) == 4)
    {
      unsigned int pc, r0, r3, r4;
      collect_register_by_name (regcache, "pc", &pc);
      collect_register_by_name (regcache, "r0", &r0);
      collect_register_by_name (regcache, "orig_r3", &r3);
      collect_register_by_name (regcache, "r4", &r4);
      curr_pc = (CORE_ADDR) pc;
      curr_r0 = (int) r0;
      *fd = (int) r3;
      *addr = (CORE_ADDR) r4;
    }
  else
    {
      unsigned long pc, r0, r3, r4;
      collect_register_by_name (regcache, "pc", &pc);
      collect_register_by_name (regcache, "r0", &r0);
      collect_register_by_name (regcache, "orig_r3", &r3);
      collect_register_by_name (regcache, "r4", &r4);
      curr_pc = (CORE_ADDR) pc;
      curr_r0 = (int) r0;
      *fd = (int) r3;
      *addr = (CORE_ADDR) r4;
    }

  /* Fetch instruction preceding current NIP.  */
  if ((*the_target->read_memory) (curr_pc - 4,
				  (unsigned char *) &curr_insn, 4) != 0)
    return 0;
  /* It should be a "sc" instruction.  */
  if (curr_insn != INSTR_SC)
    return 0;
  /* System call number should be NR_spu_run.  */
  if (curr_r0 != NR_spu_run)
    return 0;

  return 1;
}

static CORE_ADDR
ppc_get_pc (struct regcache *regcache)
{
  CORE_ADDR addr;
  int fd;

  if (parse_spufs_run (regcache, &fd, &addr))
    {
      unsigned int pc;
      (*the_target->read_memory) (addr, (unsigned char *) &pc, 4);
      return ((CORE_ADDR)1 << 63)
	| ((CORE_ADDR)fd << 32) | (CORE_ADDR) (pc - 4);
    }
  else if (register_size (regcache->tdesc, 0) == 4)
    {
      unsigned int pc;
      collect_register_by_name (regcache, "pc", &pc);
      return (CORE_ADDR) pc;
    }
  else
    {
      unsigned long pc;
      collect_register_by_name (regcache, "pc", &pc);
      return (CORE_ADDR) pc;
    }
}

static void
ppc_set_pc (struct regcache *regcache, CORE_ADDR pc)
{
  CORE_ADDR addr;
  int fd;

  if (parse_spufs_run (regcache, &fd, &addr))
    {
      unsigned int newpc = pc;
      (*the_target->write_memory) (addr, (unsigned char *) &newpc, 4);
    }
  else if (register_size (regcache->tdesc, 0) == 4)
    {
      unsigned int newpc = pc;
      supply_register_by_name (regcache, "pc", &newpc);
    }
  else
    {
      unsigned long newpc = pc;
      supply_register_by_name (regcache, "pc", &newpc);
    }
}


static int
ppc_get_auxv (unsigned long type, unsigned long *valp)
{
  const struct target_desc *tdesc = current_process ()->tdesc;
  int wordsize = register_size (tdesc, 0);
  unsigned char *data = (unsigned char *) alloca (2 * wordsize);
  int offset = 0;

  while ((*the_target->read_auxv) (offset, data, 2 * wordsize) == 2 * wordsize)
    {
      if (wordsize == 4)
	{
	  unsigned int *data_p = (unsigned int *)data;
	  if (data_p[0] == type)
	    {
	      *valp = data_p[1];
	      return 1;
	    }
	}
      else
	{
	  unsigned long *data_p = (unsigned long *)data;
	  if (data_p[0] == type)
	    {
	      *valp = data_p[1];
	      return 1;
	    }
	}

      offset += 2 * wordsize;
    }

  *valp = 0;
  return 0;
}

#ifndef __powerpc64__
static int ppc_regmap_adjusted;
#endif


/* Correct in either endianness.
   This instruction is "twge r2, r2", which GDB uses as a software
   breakpoint.  */
static const unsigned int ppc_breakpoint = 0x7d821008;
#define ppc_breakpoint_len 4

/* Implementation of linux_target_ops method "sw_breakpoint_from_kind".  */

static const gdb_byte *
ppc_sw_breakpoint_from_kind (int kind, int *size)
{
  *size = ppc_breakpoint_len;
  return (const gdb_byte *) &ppc_breakpoint;
}

static int
ppc_breakpoint_at (CORE_ADDR where)
{
  unsigned int insn;

  if (where & ((CORE_ADDR)1 << 63))
    {
      char mem_annex[32];
      sprintf (mem_annex, "%d/mem", (int)((where >> 32) & 0x7fffffff));
      (*the_target->qxfer_spu) (mem_annex, (unsigned char *) &insn,
				NULL, where & 0xffffffff, 4);
      if (insn == 0x3fff)
	return 1;
    }
  else
    {
      (*the_target->read_memory) (where, (unsigned char *) &insn, 4);
      if (insn == ppc_breakpoint)
	return 1;
      /* If necessary, recognize more trap instructions here.  GDB only uses
	 the one.  */
    }

  return 0;
}

/* Implement supports_z_point_type target-ops.
   Returns true if type Z_TYPE breakpoint is supported.

   Handling software breakpoint at server side, so tracepoints
   and breakpoints can be inserted at the same location.  */

static int
ppc_supports_z_point_type (char z_type)
{
  switch (z_type)
    {
    case Z_PACKET_SW_BP:
      return 1;
    case Z_PACKET_HW_BP:
    case Z_PACKET_WRITE_WP:
    case Z_PACKET_ACCESS_WP:
    default:
      return 0;
    }
}

/* Implement insert_point target-ops.
   Returns 0 on success, -1 on failure and 1 on unsupported.  */

static int
ppc_insert_point (enum raw_bkpt_type type, CORE_ADDR addr,
		  int size, struct raw_breakpoint *bp)
{
  switch (type)
    {
    case raw_bkpt_type_sw:
      return insert_memory_breakpoint (bp);

    case raw_bkpt_type_hw:
    case raw_bkpt_type_write_wp:
    case raw_bkpt_type_access_wp:
    default:
      /* Unsupported.  */
      return 1;
    }
}

/* Implement remove_point target-ops.
   Returns 0 on success, -1 on failure and 1 on unsupported.  */

static int
ppc_remove_point (enum raw_bkpt_type type, CORE_ADDR addr,
		  int size, struct raw_breakpoint *bp)
{
  switch (type)
    {
    case raw_bkpt_type_sw:
      return remove_memory_breakpoint (bp);

    case raw_bkpt_type_hw:
    case raw_bkpt_type_write_wp:
    case raw_bkpt_type_access_wp:
    default:
      /* Unsupported.  */
      return 1;
    }
}

/* Provide only a fill function for the general register set.  ps_lgetregs
   will use this for NPTL support.  */

static void ppc_fill_gregset (struct regcache *regcache, void *buf)
{
  int i;

  for (i = 0; i < 32; i++)
    ppc_collect_ptrace_register (regcache, i, (char *) buf + ppc_regmap[i]);

  for (i = 64; i < 70; i++)
    ppc_collect_ptrace_register (regcache, i, (char *) buf + ppc_regmap[i]);

  for (i = 71; i < 73; i++)
    ppc_collect_ptrace_register (regcache, i, (char *) buf + ppc_regmap[i]);
}

#define SIZEOF_VSXREGS 32*8

static void
ppc_fill_vsxregset (struct regcache *regcache, void *buf)
{
  int i, base;
  char *regset = (char *) buf;

  if (!(ppc_hwcap & PPC_FEATURE_HAS_VSX))
    return;

  base = find_regno (regcache->tdesc, "vs0h");
  for (i = 0; i < 32; i++)
    collect_register (regcache, base + i, &regset[i * 8]);
}

static void
ppc_store_vsxregset (struct regcache *regcache, const void *buf)
{
  int i, base;
  const char *regset = (const char *) buf;

  if (!(ppc_hwcap & PPC_FEATURE_HAS_VSX))
    return;

  base = find_regno (regcache->tdesc, "vs0h");
  for (i = 0; i < 32; i++)
    supply_register (regcache, base + i, &regset[i * 8]);
}

#define SIZEOF_VRREGS 33*16+4

static void
ppc_fill_vrregset (struct regcache *regcache, void *buf)
{
  int i, base;
  char *regset = (char *) buf;

  if (!(ppc_hwcap & PPC_FEATURE_HAS_ALTIVEC))
    return;

  base = find_regno (regcache->tdesc, "vr0");
  for (i = 0; i < 32; i++)
    collect_register (regcache, base + i, &regset[i * 16]);

  collect_register_by_name (regcache, "vscr", &regset[32 * 16 + 12]);
  collect_register_by_name (regcache, "vrsave", &regset[33 * 16]);
}

static void
ppc_store_vrregset (struct regcache *regcache, const void *buf)
{
  int i, base;
  const char *regset = (const char *) buf;

  if (!(ppc_hwcap & PPC_FEATURE_HAS_ALTIVEC))
    return;

  base = find_regno (regcache->tdesc, "vr0");
  for (i = 0; i < 32; i++)
    supply_register (regcache, base + i, &regset[i * 16]);

  supply_register_by_name (regcache, "vscr", &regset[32 * 16 + 12]);
  supply_register_by_name (regcache, "vrsave", &regset[33 * 16]);
}

struct gdb_evrregset_t
{
  unsigned long evr[32];
  unsigned long long acc;
  unsigned long spefscr;
};

static void
ppc_fill_evrregset (struct regcache *regcache, void *buf)
{
  int i, ev0;
  struct gdb_evrregset_t *regset = (struct gdb_evrregset_t *) buf;

  if (!(ppc_hwcap & PPC_FEATURE_HAS_SPE))
    return;

  ev0 = find_regno (regcache->tdesc, "ev0h");
  for (i = 0; i < 32; i++)
    collect_register (regcache, ev0 + i, &regset->evr[i]);

  collect_register_by_name (regcache, "acc", &regset->acc);
  collect_register_by_name (regcache, "spefscr", &regset->spefscr);
}

static void
ppc_store_evrregset (struct regcache *regcache, const void *buf)
{
  int i, ev0;
  const struct gdb_evrregset_t *regset = (const struct gdb_evrregset_t *) buf;

  if (!(ppc_hwcap & PPC_FEATURE_HAS_SPE))
    return;

  ev0 = find_regno (regcache->tdesc, "ev0h");
  for (i = 0; i < 32; i++)
    supply_register (regcache, ev0 + i, &regset->evr[i]);

  supply_register_by_name (regcache, "acc", &regset->acc);
  supply_register_by_name (regcache, "spefscr", &regset->spefscr);
}

/* Support for hardware single step.  */

static int
ppc_supports_hardware_single_step (void)
{
  return 1;
}

static struct regset_info ppc_regsets[] = {
  /* List the extra register sets before GENERAL_REGS.  That way we will
     fetch them every time, but still fall back to PTRACE_PEEKUSER for the
     general registers.  Some kernels support these, but not the newer
     PPC_PTRACE_GETREGS.  */
  { PTRACE_GETVSXREGS, PTRACE_SETVSXREGS, 0, SIZEOF_VSXREGS, EXTENDED_REGS,
  ppc_fill_vsxregset, ppc_store_vsxregset },
  { PTRACE_GETVRREGS, PTRACE_SETVRREGS, 0, SIZEOF_VRREGS, EXTENDED_REGS,
    ppc_fill_vrregset, ppc_store_vrregset },
  { PTRACE_GETEVRREGS, PTRACE_SETEVRREGS, 0, 32 * 4 + 8 + 4, EXTENDED_REGS,
    ppc_fill_evrregset, ppc_store_evrregset },
  { 0, 0, 0, 0, GENERAL_REGS, ppc_fill_gregset, NULL },
  NULL_REGSET
};

static struct usrregs_info ppc_usrregs_info =
  {
    ppc_num_regs,
    ppc_regmap,
  };

static struct regsets_info ppc_regsets_info =
  {
    ppc_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

static struct regs_info regs_info =
  {
    NULL, /* regset_bitmap */
    &ppc_usrregs_info,
    &ppc_regsets_info
  };

static const struct regs_info *
ppc_regs_info (void)
{
  return &regs_info;
}

static void
ppc_arch_setup (void)
{
  const struct target_desc *tdesc;
#ifdef __powerpc64__
  long msr;
  struct regcache *regcache;

  /* On a 64-bit host, assume 64-bit inferior process with no
     AltiVec registers.  Reset ppc_hwcap to ensure that the
     collect_register call below does not fail.  */
  tdesc = tdesc_powerpc_64l;
  current_process ()->tdesc = tdesc;
  ppc_hwcap = 0;

  regcache = new_register_cache (tdesc);
  fetch_inferior_registers (regcache, find_regno (tdesc, "msr"));
  collect_register_by_name (regcache, "msr", &msr);
  free_register_cache (regcache);
  if (ppc64_64bit_inferior_p (msr))
    {
      ppc_get_auxv (AT_HWCAP, &ppc_hwcap);
      if (ppc_hwcap & PPC_FEATURE_CELL)
	tdesc = tdesc_powerpc_cell64l;
      else if (ppc_hwcap & PPC_FEATURE_HAS_VSX)
	{
	  /* Power ISA 2.05 (implemented by Power 6 and newer processors)
	     increases the FPSCR from 32 bits to 64 bits. Even though Power 7
	     supports this ISA version, it doesn't have PPC_FEATURE_ARCH_2_05
	     set, only PPC_FEATURE_ARCH_2_06.  Since for now the only bits
	     used in the higher half of the register are for Decimal Floating
	     Point, we check if that feature is available to decide the size
	     of the FPSCR.  */
	  if (ppc_hwcap & PPC_FEATURE_HAS_DFP)
	    tdesc = tdesc_powerpc_isa205_vsx64l;
	  else
	    tdesc = tdesc_powerpc_vsx64l;
	}
      else if (ppc_hwcap & PPC_FEATURE_HAS_ALTIVEC)
	{
	  if (ppc_hwcap & PPC_FEATURE_HAS_DFP)
	    tdesc = tdesc_powerpc_isa205_altivec64l;
	  else
	    tdesc = tdesc_powerpc_altivec64l;
	}

      current_process ()->tdesc = tdesc;
      return;
    }
#endif

  /* OK, we have a 32-bit inferior.  */
  tdesc = tdesc_powerpc_32l;
  current_process ()->tdesc = tdesc;

  ppc_get_auxv (AT_HWCAP, &ppc_hwcap);
  if (ppc_hwcap & PPC_FEATURE_CELL)
    tdesc = tdesc_powerpc_cell32l;
  else if (ppc_hwcap & PPC_FEATURE_HAS_VSX)
    {
      if (ppc_hwcap & PPC_FEATURE_HAS_DFP)
	tdesc = tdesc_powerpc_isa205_vsx32l;
      else
	tdesc = tdesc_powerpc_vsx32l;
    }
  else if (ppc_hwcap & PPC_FEATURE_HAS_ALTIVEC)
    {
      if (ppc_hwcap & PPC_FEATURE_HAS_DFP)
	tdesc = tdesc_powerpc_isa205_altivec32l;
      else
	tdesc = tdesc_powerpc_altivec32l;
    }

  /* On 32-bit machines, check for SPE registers.
     Set the low target's regmap field as appropriately.  */
#ifndef __powerpc64__
  if (ppc_hwcap & PPC_FEATURE_HAS_SPE)
    tdesc = tdesc_powerpc_e500l;

  if (!ppc_regmap_adjusted)
    {
      if (ppc_hwcap & PPC_FEATURE_HAS_SPE)
	ppc_usrregs_info.regmap = ppc_regmap_e500;

      /* If the FPSCR is 64-bit wide, we need to fetch the whole
	 64-bit slot and not just its second word.  The PT_FPSCR
	 supplied in a 32-bit GDB compilation doesn't reflect
	 this.  */
      if (register_size (tdesc, 70) == 8)
	ppc_regmap[70] = (48 + 2*32) * sizeof (long);

      ppc_regmap_adjusted = 1;
   }
#endif
  current_process ()->tdesc = tdesc;
}

/* Implementation of linux_target_ops method "supports_tracepoints".  */

static int
ppc_supports_tracepoints (void)
{
  return 1;
}

/* Get the thread area address.  This is used to recognize which
   thread is which when tracing with the in-process agent library.  We
   don't read anything from the address, and treat it as opaque; it's
   the address itself that we assume is unique per-thread.  */

static int
ppc_get_thread_area (int lwpid, CORE_ADDR *addr)
{
  struct lwp_info *lwp = find_lwp_pid (pid_to_ptid (lwpid));
  struct thread_info *thr = get_lwp_thread (lwp);
  struct regcache *regcache = get_thread_regcache (thr, 1);
  ULONGEST tp = 0;

#ifdef __powerpc64__
  if (register_size (regcache->tdesc, 0) == 8)
    collect_register_by_name (regcache, "r13", &tp);
  else
#endif
    collect_register_by_name (regcache, "r2", &tp);

  *addr = tp;

  return 0;
}

#ifdef __powerpc64__

/* Older glibc doesn't provide this.  */

#ifndef EF_PPC64_ABI
#define EF_PPC64_ABI 3
#endif

/* Returns 1 if inferior is using ELFv2 ABI.  Undefined for 32-bit
   inferiors.  */

static int
is_elfv2_inferior (void)
{
  /* To be used as fallback if we're unable to determine the right result -
     assume inferior uses the same ABI as gdbserver.  */
#if _CALL_ELF == 2
  const int def_res = 1;
#else
  const int def_res = 0;
#endif
  unsigned long phdr;
  Elf64_Ehdr ehdr;

  if (!ppc_get_auxv (AT_PHDR, &phdr))
    return def_res;

  /* Assume ELF header is at the beginning of the page where program headers
     are located.  If it doesn't look like one, bail.  */

  read_inferior_memory (phdr & ~0xfff, (unsigned char *) &ehdr, sizeof ehdr);
  if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG))
    return def_res;

  return (ehdr.e_flags & EF_PPC64_ABI) == 2;
}

#endif

/* Generate a ds-form instruction in BUF and return the number of bytes written

   0      6     11   16          30 32
   | OPCD | RST | RA |     DS    |XO|  */

__attribute__((unused)) /* Maybe unused due to conditional compilation.  */
static int
gen_ds_form (uint32_t *buf, int opcd, int rst, int ra, int ds, int xo)
{
  uint32_t insn;

  gdb_assert ((opcd & ~0x3f) == 0);
  gdb_assert ((rst & ~0x1f) == 0);
  gdb_assert ((ra & ~0x1f) == 0);
  gdb_assert ((xo & ~0x3) == 0);

  insn = (rst << 21) | (ra << 16) | (ds & 0xfffc) | (xo & 0x3);
  *buf = (opcd << 26) | insn;
  return 1;
}

/* Followings are frequently used ds-form instructions.  */

#define GEN_STD(buf, rs, ra, offset)	gen_ds_form (buf, 62, rs, ra, offset, 0)
#define GEN_STDU(buf, rs, ra, offset)	gen_ds_form (buf, 62, rs, ra, offset, 1)
#define GEN_LD(buf, rt, ra, offset)	gen_ds_form (buf, 58, rt, ra, offset, 0)
#define GEN_LDU(buf, rt, ra, offset)	gen_ds_form (buf, 58, rt, ra, offset, 1)

/* Generate a d-form instruction in BUF.

   0      6     11   16             32
   | OPCD | RST | RA |       D      |  */

static int
gen_d_form (uint32_t *buf, int opcd, int rst, int ra, int si)
{
  uint32_t insn;

  gdb_assert ((opcd & ~0x3f) == 0);
  gdb_assert ((rst & ~0x1f) == 0);
  gdb_assert ((ra & ~0x1f) == 0);

  insn = (rst << 21) | (ra << 16) | (si & 0xffff);
  *buf = (opcd << 26) | insn;
  return 1;
}

/* Followings are frequently used d-form instructions.  */

#define GEN_ADDI(buf, rt, ra, si)	gen_d_form (buf, 14, rt, ra, si)
#define GEN_ADDIS(buf, rt, ra, si)	gen_d_form (buf, 15, rt, ra, si)
#define GEN_LI(buf, rt, si)		GEN_ADDI (buf, rt, 0, si)
#define GEN_LIS(buf, rt, si)		GEN_ADDIS (buf, rt, 0, si)
#define GEN_ORI(buf, rt, ra, si)	gen_d_form (buf, 24, rt, ra, si)
#define GEN_ORIS(buf, rt, ra, si)	gen_d_form (buf, 25, rt, ra, si)
#define GEN_LWZ(buf, rt, ra, si)	gen_d_form (buf, 32, rt, ra, si)
#define GEN_STW(buf, rt, ra, si)	gen_d_form (buf, 36, rt, ra, si)
#define GEN_STWU(buf, rt, ra, si)	gen_d_form (buf, 37, rt, ra, si)

/* Generate a xfx-form instruction in BUF and return the number of bytes
   written.

   0      6     11         21        31 32
   | OPCD | RST |    RI    |    XO   |/|  */

static int
gen_xfx_form (uint32_t *buf, int opcd, int rst, int ri, int xo)
{
  uint32_t insn;
  unsigned int n = ((ri & 0x1f) << 5) | ((ri >> 5) & 0x1f);

  gdb_assert ((opcd & ~0x3f) == 0);
  gdb_assert ((rst & ~0x1f) == 0);
  gdb_assert ((xo & ~0x3ff) == 0);

  insn = (rst << 21) | (n << 11) | (xo << 1);
  *buf = (opcd << 26) | insn;
  return 1;
}

/* Followings are frequently used xfx-form instructions.  */

#define GEN_MFSPR(buf, rt, spr)		gen_xfx_form (buf, 31, rt, spr, 339)
#define GEN_MTSPR(buf, rt, spr)		gen_xfx_form (buf, 31, rt, spr, 467)
#define GEN_MFCR(buf, rt)		gen_xfx_form (buf, 31, rt, 0, 19)
#define GEN_MTCR(buf, rt)		gen_xfx_form (buf, 31, rt, 0x3cf, 144)
#define GEN_SYNC(buf, L, E)             gen_xfx_form (buf, 31, L & 0x3, \
						      E & 0xf, 598)
#define GEN_LWSYNC(buf)			GEN_SYNC (buf, 1, 0)


/* Generate a x-form instruction in BUF and return the number of bytes written.

   0      6     11   16   21       31 32
   | OPCD | RST | RA | RB |   XO   |RC|  */

static int
gen_x_form (uint32_t *buf, int opcd, int rst, int ra, int rb, int xo, int rc)
{
  uint32_t insn;

  gdb_assert ((opcd & ~0x3f) == 0);
  gdb_assert ((rst & ~0x1f) == 0);
  gdb_assert ((ra & ~0x1f) == 0);
  gdb_assert ((rb & ~0x1f) == 0);
  gdb_assert ((xo & ~0x3ff) == 0);
  gdb_assert ((rc & ~1) == 0);

  insn = (rst << 21) | (ra << 16) | (rb << 11) | (xo << 1) | rc;
  *buf = (opcd << 26) | insn;
  return 1;
}

/* Followings are frequently used x-form instructions.  */

#define GEN_OR(buf, ra, rs, rb)		gen_x_form (buf, 31, rs, ra, rb, 444, 0)
#define GEN_MR(buf, ra, rs)		GEN_OR (buf, ra, rs, rs)
#define GEN_LWARX(buf, rt, ra, rb)	gen_x_form (buf, 31, rt, ra, rb, 20, 0)
#define GEN_STWCX(buf, rs, ra, rb)	gen_x_form (buf, 31, rs, ra, rb, 150, 1)
/* Assume bf = cr7.  */
#define GEN_CMPW(buf, ra, rb)		gen_x_form (buf, 31, 28, ra, rb, 0, 0)


/* Generate a md-form instruction in BUF and return the number of bytes written.

   0      6    11   16   21   27   30 31 32
   | OPCD | RS | RA | sh | mb | XO |sh|Rc|  */

static int
gen_md_form (uint32_t *buf, int opcd, int rs, int ra, int sh, int mb,
	     int xo, int rc)
{
  uint32_t insn;
  unsigned int n = ((mb & 0x1f) << 1) | ((mb >> 5) & 0x1);
  unsigned int sh0_4 = sh & 0x1f;
  unsigned int sh5 = (sh >> 5) & 1;

  gdb_assert ((opcd & ~0x3f) == 0);
  gdb_assert ((rs & ~0x1f) == 0);
  gdb_assert ((ra & ~0x1f) == 0);
  gdb_assert ((sh & ~0x3f) == 0);
  gdb_assert ((mb & ~0x3f) == 0);
  gdb_assert ((xo & ~0x7) == 0);
  gdb_assert ((rc & ~0x1) == 0);

  insn = (rs << 21) | (ra << 16) | (sh0_4 << 11) | (n << 5)
	 | (sh5 << 1) | (xo << 2) | (rc & 1);
  *buf = (opcd << 26) | insn;
  return 1;
}

/* The following are frequently used md-form instructions.  */

#define GEN_RLDICL(buf, ra, rs ,sh, mb) \
				gen_md_form (buf, 30, rs, ra, sh, mb, 0, 0)
#define GEN_RLDICR(buf, ra, rs ,sh, mb) \
				gen_md_form (buf, 30, rs, ra, sh, mb, 1, 0)

/* Generate a i-form instruction in BUF and return the number of bytes written.

   0      6                          30 31 32
   | OPCD |            LI            |AA|LK|  */

static int
gen_i_form (uint32_t *buf, int opcd, int li, int aa, int lk)
{
  uint32_t insn;

  gdb_assert ((opcd & ~0x3f) == 0);

  insn = (li & 0x3fffffc) | (aa & 1) | (lk & 1);
  *buf = (opcd << 26) | insn;
  return 1;
}

/* The following are frequently used i-form instructions.  */

#define GEN_B(buf, li)		gen_i_form (buf, 18, li, 0, 0)
#define GEN_BL(buf, li)		gen_i_form (buf, 18, li, 0, 1)

/* Generate a b-form instruction in BUF and return the number of bytes written.

   0      6    11   16               30 31 32
   | OPCD | BO | BI |      BD        |AA|LK|  */

static int
gen_b_form (uint32_t *buf, int opcd, int bo, int bi, int bd,
	    int aa, int lk)
{
  uint32_t insn;

  gdb_assert ((opcd & ~0x3f) == 0);
  gdb_assert ((bo & ~0x1f) == 0);
  gdb_assert ((bi & ~0x1f) == 0);

  insn = (bo << 21) | (bi << 16) | (bd & 0xfffc) | (aa & 1) | (lk & 1);
  *buf = (opcd << 26) | insn;
  return 1;
}

/* The following are frequently used b-form instructions.  */
/* Assume bi = cr7.  */
#define GEN_BNE(buf, bd)  gen_b_form (buf, 16, 0x4, (7 << 2) | 2, bd, 0 ,0)

/* GEN_LOAD and GEN_STORE generate 64- or 32-bit load/store for ppc64 or ppc32
   respectively.  They are primary used for save/restore GPRs in jump-pad,
   not used for bytecode compiling.  */

#ifdef __powerpc64__
#define GEN_LOAD(buf, rt, ra, si, is_64)	(is_64 ? \
						 GEN_LD (buf, rt, ra, si) : \
						 GEN_LWZ (buf, rt, ra, si))
#define GEN_STORE(buf, rt, ra, si, is_64)	(is_64 ? \
						 GEN_STD (buf, rt, ra, si) : \
						 GEN_STW (buf, rt, ra, si))
#else
#define GEN_LOAD(buf, rt, ra, si, is_64)	GEN_LWZ (buf, rt, ra, si)
#define GEN_STORE(buf, rt, ra, si, is_64)	GEN_STW (buf, rt, ra, si)
#endif

/* Generate a sequence of instructions to load IMM in the register REG.
   Write the instructions in BUF and return the number of bytes written.  */

static int
gen_limm (uint32_t *buf, int reg, uint64_t imm, int is_64)
{
  uint32_t *p = buf;

  if ((imm + 32768) < 65536)
    {
      /* li	reg, imm[15:0] */
      p += GEN_LI (p, reg, imm);
    }
  else if ((imm >> 32) == 0)
    {
      /* lis	reg, imm[31:16]
	 ori	reg, reg, imm[15:0]
	 rldicl reg, reg, 0, 32 */
      p += GEN_LIS (p, reg, (imm >> 16) & 0xffff);
      if ((imm & 0xffff) != 0)
	p += GEN_ORI (p, reg, reg, imm & 0xffff);
      /* Clear upper 32-bit if sign-bit is set.  */
      if (imm & (1u << 31) && is_64)
	p += GEN_RLDICL (p, reg, reg, 0, 32);
    }
  else
    {
      gdb_assert (is_64);
      /* lis    reg, <imm[63:48]>
	 ori    reg, reg, <imm[48:32]>
	 rldicr reg, reg, 32, 31
	 oris   reg, reg, <imm[31:16]>
	 ori    reg, reg, <imm[15:0]> */
      p += GEN_LIS (p, reg, ((imm >> 48) & 0xffff));
      if (((imm >> 32) & 0xffff) != 0)
        p += GEN_ORI (p, reg, reg, ((imm >> 32) & 0xffff));
      p += GEN_RLDICR (p, reg, reg, 32, 31);
      if (((imm >> 16) & 0xffff) != 0)
        p += GEN_ORIS (p, reg, reg, ((imm >> 16) & 0xffff));
      if ((imm & 0xffff) != 0)
        p += GEN_ORI (p, reg, reg, (imm & 0xffff));
    }

  return p - buf;
}

/* Generate a sequence for atomically exchange at location LOCK.
   This code sequence clobbers r6, r7, r8.  LOCK is the location for
   the atomic-xchg, OLD_VALUE is expected old value stored in the
   location, and R_NEW is a register for the new value.  */

static int
gen_atomic_xchg (uint32_t *buf, CORE_ADDR lock, int old_value, int r_new,
		 int is_64)
{
  const int r_lock = 6;
  const int r_old = 7;
  const int r_tmp = 8;
  uint32_t *p = buf;

  /*
  1: lwarx   TMP, 0, LOCK
     cmpwi   TMP, OLD
     bne     1b
     stwcx.  NEW, 0, LOCK
     bne     1b */

  p += gen_limm (p, r_lock, lock, is_64);
  p += gen_limm (p, r_old, old_value, is_64);

  p += GEN_LWARX (p, r_tmp, 0, r_lock);
  p += GEN_CMPW (p, r_tmp, r_old);
  p += GEN_BNE (p, -8);
  p += GEN_STWCX (p, r_new, 0, r_lock);
  p += GEN_BNE (p, -16);

  return p - buf;
}

/* Generate a sequence of instructions for calling a function
   at address of FN.  Return the number of bytes are written in BUF.  */

static int
gen_call (uint32_t *buf, CORE_ADDR fn, int is_64, int is_opd)
{
  uint32_t *p = buf;

  /* Must be called by r12 for caller to calculate TOC address. */
  p += gen_limm (p, 12, fn, is_64);
  if (is_opd)
    {
      p += GEN_LOAD (p, 11, 12, 16, is_64);
      p += GEN_LOAD (p, 2, 12, 8, is_64);
      p += GEN_LOAD (p, 12, 12, 0, is_64);
    }
  p += GEN_MTSPR (p, 12, 9);		/* mtctr  r12 */
  *p++ = 0x4e800421;			/* bctrl */

  return p - buf;
}

/* Copy the instruction from OLDLOC to *TO, and update *TO to *TO + size
   of instruction.  This function is used to adjust pc-relative instructions
   when copying.  */

static void
ppc_relocate_instruction (CORE_ADDR *to, CORE_ADDR oldloc)
{
  uint32_t insn, op6;
  long rel, newrel;

  read_inferior_memory (oldloc, (unsigned char *) &insn, 4);
  op6 = PPC_OP6 (insn);

  if (op6 == 18 && (insn & 2) == 0)
    {
      /* branch && AA = 0 */
      rel = PPC_LI (insn);
      newrel = (oldloc - *to) + rel;

      /* Out of range. Cannot relocate instruction.  */
      if (newrel >= (1 << 25) || newrel < -(1 << 25))
	return;

      insn = (insn & ~0x3fffffc) | (newrel & 0x3fffffc);
    }
  else if (op6 == 16 && (insn & 2) == 0)
    {
      /* conditional branch && AA = 0 */

      /* If the new relocation is too big for even a 26-bit unconditional
	 branch, there is nothing we can do.  Just abort.

	 Otherwise, if it can be fit in 16-bit conditional branch, just
	 copy the instruction and relocate the address.

	 If the it's  big for conditional-branch (16-bit), try to invert the
	 condition and jump with 26-bit branch.  For example,

	 beq  .Lgoto
	 INSN1

	 =>

	 bne  1f (+8)
	 b    .Lgoto
       1:INSN1

	 After this transform, we are actually jump from *TO+4 instead of *TO,
	 so check the relocation again because it will be 1-insn farther then
	 before if *TO is after OLDLOC.


	 For BDNZT (or so) is transformed from

	 bdnzt  eq, .Lgoto
	 INSN1

	 =>

	 bdz    1f (+12)
	 bf     eq, 1f (+8)
	 b      .Lgoto
       1:INSN1

	 See also "BO field encodings".  */

      rel = PPC_BD (insn);
      newrel = (oldloc - *to) + rel;

      if (newrel < (1 << 15) && newrel >= -(1 << 15))
	insn = (insn & ~0xfffc) | (newrel & 0xfffc);
      else if ((PPC_BO (insn) & 0x14) == 0x4 || (PPC_BO (insn) & 0x14) == 0x10)
	{
	  newrel -= 4;

	  /* Out of range. Cannot relocate instruction.  */
	  if (newrel >= (1 << 25) || newrel < -(1 << 25))
	    return;

	  if ((PPC_BO (insn) & 0x14) == 0x4)
	    insn ^= (1 << 24);
	  else if ((PPC_BO (insn) & 0x14) == 0x10)
	    insn ^= (1 << 22);

	  /* Jump over the unconditional branch.  */
	  insn = (insn & ~0xfffc) | 0x8;
	  write_inferior_memory (*to, (unsigned char *) &insn, 4);
	  *to += 4;

	  /* Build a unconditional branch and copy LK bit.  */
	  insn = (18 << 26) | (0x3fffffc & newrel) | (insn & 0x3);
	  write_inferior_memory (*to, (unsigned char *) &insn, 4);
	  *to += 4;

	  return;
	}
      else if ((PPC_BO (insn) & 0x14) == 0)
	{
	  uint32_t bdnz_insn = (16 << 26) | (0x10 << 21) | 12;
	  uint32_t bf_insn = (16 << 26) | (0x4 << 21) | 8;

	  newrel -= 8;

	  /* Out of range. Cannot relocate instruction.  */
	  if (newrel >= (1 << 25) || newrel < -(1 << 25))
	    return;

	  /* Copy BI field.  */
	  bf_insn |= (insn & 0x1f0000);

	  /* Invert condition.  */
	  bdnz_insn |= (insn ^ (1 << 22)) & (1 << 22);
	  bf_insn |= (insn ^ (1 << 24)) & (1 << 24);

	  write_inferior_memory (*to, (unsigned char *) &bdnz_insn, 4);
	  *to += 4;
	  write_inferior_memory (*to, (unsigned char *) &bf_insn, 4);
	  *to += 4;

	  /* Build a unconditional branch and copy LK bit.  */
	  insn = (18 << 26) | (0x3fffffc & newrel) | (insn & 0x3);
	  write_inferior_memory (*to, (unsigned char *) &insn, 4);
	  *to += 4;

	  return;
	}
      else /* (BO & 0x14) == 0x14, branch always.  */
	{
	  /* Out of range. Cannot relocate instruction.  */
	  if (newrel >= (1 << 25) || newrel < -(1 << 25))
	    return;

	  /* Build a unconditional branch and copy LK bit.  */
	  insn = (18 << 26) | (0x3fffffc & newrel) | (insn & 0x3);
	  write_inferior_memory (*to, (unsigned char *) &insn, 4);
	  *to += 4;

	  return;
	}
    }

  write_inferior_memory (*to, (unsigned char *) &insn, 4);
  *to += 4;
}

/* Implement install_fast_tracepoint_jump_pad of target_ops.
   See target.h for details.  */

static int
ppc_install_fast_tracepoint_jump_pad (CORE_ADDR tpoint, CORE_ADDR tpaddr,
				      CORE_ADDR collector,
				      CORE_ADDR lockaddr,
				      ULONGEST orig_size,
				      CORE_ADDR *jump_entry,
				      CORE_ADDR *trampoline,
				      ULONGEST *trampoline_size,
				      unsigned char *jjump_pad_insn,
				      ULONGEST *jjump_pad_insn_size,
				      CORE_ADDR *adjusted_insn_addr,
				      CORE_ADDR *adjusted_insn_addr_end,
				      char *err)
{
  uint32_t buf[256];
  uint32_t *p = buf;
  int j, offset;
  CORE_ADDR buildaddr = *jump_entry;
  const CORE_ADDR entryaddr = *jump_entry;
  int rsz, min_frame, frame_size, tp_reg;
#ifdef __powerpc64__
  struct regcache *regcache = get_thread_regcache (current_thread, 0);
  int is_64 = register_size (regcache->tdesc, 0) == 8;
  int is_opd = is_64 && !is_elfv2_inferior ();
#else
  int is_64 = 0, is_opd = 0;
#endif

#ifdef __powerpc64__
  if (is_64)
    {
      /* Minimum frame size is 32 bytes for ELFv2, and 112 bytes for ELFv1.  */
      rsz = 8;
      min_frame = 112;
      frame_size = (40 * rsz) + min_frame;
      tp_reg = 13;
    }
  else
    {
#endif
      rsz = 4;
      min_frame = 16;
      frame_size = (40 * rsz) + min_frame;
      tp_reg = 2;
#ifdef __powerpc64__
    }
#endif

  /* Stack frame layout for this jump pad,

     High	thread_area (r13/r2)    |
		tpoint			- collecting_t obj
		PC/<tpaddr>		| +36
		CTR			| +35
		LR			| +34
		XER			| +33
		CR			| +32
		R31			|
		R29			|
		...			|
		R1			| +1
		R0			- collected registers
		...			|
		...			|
     Low	Back-chain		-


     The code flow of this jump pad,

     1. Adjust SP
     2. Save GPR and SPR
     3. Prepare argument
     4. Call gdb_collector
     5. Restore GPR and SPR
     6. Restore SP
     7. Build a jump for back to the program
     8. Copy/relocate original instruction
     9. Build a jump for replacing orignal instruction.  */

  /* Adjust stack pointer.  */
  if (is_64)
    p += GEN_STDU (p, 1, 1, -frame_size);		/* stdu   r1,-frame_size(r1) */
  else
    p += GEN_STWU (p, 1, 1, -frame_size);		/* stwu   r1,-frame_size(r1) */

  /* Store GPRs.  Save R1 later, because it had just been modified, but
     we want the original value.  */
  for (j = 2; j < 32; j++)
    p += GEN_STORE (p, j, 1, min_frame + j * rsz, is_64);
  p += GEN_STORE (p, 0, 1, min_frame + 0 * rsz, is_64);
  /* Set r0 to the original value of r1 before adjusting stack frame,
     and then save it.  */
  p += GEN_ADDI (p, 0, 1, frame_size);
  p += GEN_STORE (p, 0, 1, min_frame + 1 * rsz, is_64);

  /* Save CR, XER, LR, and CTR.  */
  p += GEN_MFCR (p, 3);					/* mfcr   r3 */
  p += GEN_MFSPR (p, 4, 1);				/* mfxer  r4 */
  p += GEN_MFSPR (p, 5, 8);				/* mflr   r5 */
  p += GEN_MFSPR (p, 6, 9);				/* mfctr  r6 */
  p += GEN_STORE (p, 3, 1, min_frame + 32 * rsz, is_64);/* std    r3, 32(r1) */
  p += GEN_STORE (p, 4, 1, min_frame + 33 * rsz, is_64);/* std    r4, 33(r1) */
  p += GEN_STORE (p, 5, 1, min_frame + 34 * rsz, is_64);/* std    r5, 34(r1) */
  p += GEN_STORE (p, 6, 1, min_frame + 35 * rsz, is_64);/* std    r6, 35(r1) */

  /* Save PC<tpaddr>  */
  p += gen_limm (p, 3, tpaddr, is_64);
  p += GEN_STORE (p, 3, 1, min_frame + 36 * rsz, is_64);


  /* Setup arguments to collector.  */
  /* Set r4 to collected registers.  */
  p += GEN_ADDI (p, 4, 1, min_frame);
  /* Set r3 to TPOINT.  */
  p += gen_limm (p, 3, tpoint, is_64);

  /* Prepare collecting_t object for lock.  */
  p += GEN_STORE (p, 3, 1, min_frame + 37 * rsz, is_64);
  p += GEN_STORE (p, tp_reg, 1, min_frame + 38 * rsz, is_64);
  /* Set R5 to collecting object.  */
  p += GEN_ADDI (p, 5, 1, 37 * rsz);

  p += GEN_LWSYNC (p);
  p += gen_atomic_xchg (p, lockaddr, 0, 5, is_64);
  p += GEN_LWSYNC (p);

  /* Call to collector.  */
  p += gen_call (p, collector, is_64, is_opd);

  /* Simply write 0 to release the lock.  */
  p += gen_limm (p, 3, lockaddr, is_64);
  p += gen_limm (p, 4, 0, is_64);
  p += GEN_LWSYNC (p);
  p += GEN_STORE (p, 4, 3, 0, is_64);

  /* Restore stack and registers.  */
  p += GEN_LOAD (p, 3, 1, min_frame + 32 * rsz, is_64);	/* ld	r3, 32(r1) */
  p += GEN_LOAD (p, 4, 1, min_frame + 33 * rsz, is_64);	/* ld	r4, 33(r1) */
  p += GEN_LOAD (p, 5, 1, min_frame + 34 * rsz, is_64);	/* ld	r5, 34(r1) */
  p += GEN_LOAD (p, 6, 1, min_frame + 35 * rsz, is_64);	/* ld	r6, 35(r1) */
  p += GEN_MTCR (p, 3);					/* mtcr	  r3 */
  p += GEN_MTSPR (p, 4, 1);				/* mtxer  r4 */
  p += GEN_MTSPR (p, 5, 8);				/* mtlr   r5 */
  p += GEN_MTSPR (p, 6, 9);				/* mtctr  r6 */

  /* Restore GPRs.  */
  for (j = 2; j < 32; j++)
    p += GEN_LOAD (p, j, 1, min_frame + j * rsz, is_64);
  p += GEN_LOAD (p, 0, 1, min_frame + 0 * rsz, is_64);
  /* Restore SP.  */
  p += GEN_ADDI (p, 1, 1, frame_size);

  /* Flush instructions to inferior memory.  */
  write_inferior_memory (buildaddr, (unsigned char *) buf, (p - buf) * 4);

  /* Now, insert the original instruction to execute in the jump pad.  */
  *adjusted_insn_addr = buildaddr + (p - buf) * 4;
  *adjusted_insn_addr_end = *adjusted_insn_addr;
  ppc_relocate_instruction (adjusted_insn_addr_end, tpaddr);

  /* Verify the relocation size.  If should be 4 for normal copy,
     8 or 12 for some conditional branch.  */
  if ((*adjusted_insn_addr_end - *adjusted_insn_addr == 0)
      || (*adjusted_insn_addr_end - *adjusted_insn_addr > 12))
    {
      sprintf (err, "E.Unexpected instruction length = %d"
		    "when relocate instruction.",
		    (int) (*adjusted_insn_addr_end - *adjusted_insn_addr));
      return 1;
    }

  buildaddr = *adjusted_insn_addr_end;
  p = buf;
  /* Finally, write a jump back to the program.  */
  offset = (tpaddr + 4) - buildaddr;
  if (offset >= (1 << 25) || offset < -(1 << 25))
    {
      sprintf (err, "E.Jump back from jump pad too far from tracepoint "
		    "(offset 0x%x > 26-bit).", offset);
      return 1;
    }
  /* b <tpaddr+4> */
  p += GEN_B (p, offset);
  write_inferior_memory (buildaddr, (unsigned char *) buf, (p - buf) * 4);
  *jump_entry = buildaddr + (p - buf) * 4;

  /* The jump pad is now built.  Wire in a jump to our jump pad.  This
     is always done last (by our caller actually), so that we can
     install fast tracepoints with threads running.  This relies on
     the agent's atomic write support.  */
  offset = entryaddr - tpaddr;
  if (offset >= (1 << 25) || offset < -(1 << 25))
    {
      sprintf (err, "E.Jump back from jump pad too far from tracepoint "
		    "(offset 0x%x > 26-bit).", offset);
      return 1;
    }
  /* b <jentry> */
  GEN_B ((uint32_t *) jjump_pad_insn, offset);
  *jjump_pad_insn_size = 4;

  return 0;
}

/* Returns the minimum instruction length for installing a tracepoint.  */

static int
ppc_get_min_fast_tracepoint_insn_len (void)
{
  return 4;
}

/* Implementation of linux_target_ops method "get_ipa_tdesc_idx".  */

static int
ppc_get_ipa_tdesc_idx (void)
{
  struct regcache *regcache = get_thread_regcache (current_thread, 0);
  const struct target_desc *tdesc = regcache->tdesc;

#ifdef __powerpc64__
  if (tdesc == tdesc_powerpc_64l)
    return PPC_TDESC_BASE;
  if (tdesc == tdesc_powerpc_altivec64l)
    return PPC_TDESC_ALTIVEC;
  if (tdesc == tdesc_powerpc_cell64l)
    return PPC_TDESC_CELL;
  if (tdesc == tdesc_powerpc_vsx64l)
    return PPC_TDESC_VSX;
  if (tdesc == tdesc_powerpc_isa205_64l)
    return PPC_TDESC_ISA205;
  if (tdesc == tdesc_powerpc_isa205_altivec64l)
    return PPC_TDESC_ISA205_ALTIVEC;
  if (tdesc == tdesc_powerpc_isa205_vsx64l)
    return PPC_TDESC_ISA205_VSX;
#endif

  if (tdesc == tdesc_powerpc_32l)
    return PPC_TDESC_BASE;
  if (tdesc == tdesc_powerpc_altivec32l)
    return PPC_TDESC_ALTIVEC;
  if (tdesc == tdesc_powerpc_cell32l)
    return PPC_TDESC_CELL;
  if (tdesc == tdesc_powerpc_vsx32l)
    return PPC_TDESC_VSX;
  if (tdesc == tdesc_powerpc_isa205_32l)
    return PPC_TDESC_ISA205;
  if (tdesc == tdesc_powerpc_isa205_altivec32l)
    return PPC_TDESC_ISA205_ALTIVEC;
  if (tdesc == tdesc_powerpc_isa205_vsx32l)
    return PPC_TDESC_ISA205_VSX;
  if (tdesc == tdesc_powerpc_e500l)
    return PPC_TDESC_E500;

  return 0;
}

struct linux_target_ops the_low_target = {
  ppc_arch_setup,
  ppc_regs_info,
  ppc_cannot_fetch_register,
  ppc_cannot_store_register,
  NULL, /* fetch_register */
  ppc_get_pc,
  ppc_set_pc,
  NULL, /* breakpoint_kind_from_pc */
  ppc_sw_breakpoint_from_kind,
  NULL,
  0,
  ppc_breakpoint_at,
  ppc_supports_z_point_type,
  ppc_insert_point,
  ppc_remove_point,
  NULL,
  NULL,
  ppc_collect_ptrace_register,
  ppc_supply_ptrace_register,
  NULL, /* siginfo_fixup */
  NULL, /* new_process */
  NULL, /* new_thread */
  NULL, /* new_fork */
  NULL, /* prepare_to_resume */
  NULL, /* process_qsupported */
  ppc_supports_tracepoints,
  ppc_get_thread_area,
  ppc_install_fast_tracepoint_jump_pad,
  NULL, /* emit_ops */
  ppc_get_min_fast_tracepoint_insn_len,
  NULL, /* supports_range_stepping */
  NULL, /* breakpoint_kind_from_current_state */
  ppc_supports_hardware_single_step,
  NULL, /* get_syscall_trapinfo */
  ppc_get_ipa_tdesc_idx,
};

void
initialize_low_arch (void)
{
  /* Initialize the Linux target descriptions.  */

  init_registers_powerpc_32l ();
  init_registers_powerpc_altivec32l ();
  init_registers_powerpc_cell32l ();
  init_registers_powerpc_vsx32l ();
  init_registers_powerpc_isa205_32l ();
  init_registers_powerpc_isa205_altivec32l ();
  init_registers_powerpc_isa205_vsx32l ();
  init_registers_powerpc_e500l ();
#if __powerpc64__
  init_registers_powerpc_64l ();
  init_registers_powerpc_altivec64l ();
  init_registers_powerpc_cell64l ();
  init_registers_powerpc_vsx64l ();
  init_registers_powerpc_isa205_64l ();
  init_registers_powerpc_isa205_altivec64l ();
  init_registers_powerpc_isa205_vsx64l ();
#endif

  initialize_regsets_info (&ppc_regsets_info);
}
