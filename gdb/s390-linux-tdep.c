/* Target-dependent code for GNU/Linux on S390.

   Copyright (C) 2001-2017 Free Software Foundation, Inc.

   Contributed by D.J. Barrow (djbarrow@de.ibm.com,barrow_dj@yahoo.com)
   for IBM Deutschland Entwicklung GmbH, IBM Corporation.

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

#include "gdbcore.h"
#include "linux-record.h"
#include "linux-tdep.h"
#include "record-full.h"
#include "regcache.h"
#include "regset.h"
#include "s390-linux-tdep.h"
#include "s390-tdep.h"
#include "solib-svr4.h"
#include "target.h"
#include "xml-syscall.h"

#include "features/s390-linux32.c"
#include "features/s390-linux32v1.c"
#include "features/s390-linux32v2.c"
#include "features/s390-linux64.c"
#include "features/s390-linux64v1.c"
#include "features/s390-linux64v2.c"
#include "features/s390-te-linux64.c"
#include "features/s390-vx-linux64.c"
#include "features/s390-tevx-linux64.c"
#include "features/s390x-linux64.c"
#include "features/s390x-linux64v1.c"
#include "features/s390x-linux64v2.c"
#include "features/s390x-te-linux64.c"
#include "features/s390x-vx-linux64.c"
#include "features/s390x-tevx-linux64.c"

#define XML_SYSCALL_FILENAME_S390 "syscalls/s390-linux.xml"
#define XML_SYSCALL_FILENAME_S390X "syscalls/s390x-linux.xml"


/* The "guess_tracepoint_registers" gdbarch method.  */

static void
s390_guess_tracepoint_registers (struct gdbarch *gdbarch,
				 struct regcache *regcache,
				 CORE_ADDR addr)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int sz = register_size (gdbarch, S390_PSWA_REGNUM);
  gdb_byte *reg = (gdb_byte *) alloca (sz);
  ULONGEST pswm, pswa;

  /* Set PSWA from the location and a default PSWM (the only part we're
     unlikely to get right is the CC).  */
  if (tdep->abi == ABI_LINUX_S390)
    {
      /* 31-bit PSWA needs high bit set (it's very unlikely the target
	 was in 24-bit mode).  */
      pswa = addr | 0x80000000UL;
      pswm = 0x070d0000UL;
    }
  else
    {
      pswa = addr;
      pswm = 0x0705000180000000ULL;
    }

  store_unsigned_integer (reg, sz, gdbarch_byte_order (gdbarch), pswa);
  regcache_raw_supply (regcache, S390_PSWA_REGNUM, reg);

  store_unsigned_integer (reg, sz, gdbarch_byte_order (gdbarch), pswm);
  regcache_raw_supply (regcache, S390_PSWM_REGNUM, reg);
}


/* Maps for register sets.  */

static const struct regcache_map_entry s390_gregmap[] =
  {
    { 1, S390_PSWM_REGNUM },
    { 1, S390_PSWA_REGNUM },
    { 16, S390_R0_REGNUM },
    { 16, S390_A0_REGNUM },
    { 1, S390_ORIG_R2_REGNUM },
    { 0 }
  };

static const struct regcache_map_entry s390_fpregmap[] =
  {
    { 1, S390_FPC_REGNUM, 8 },
    { 16, S390_F0_REGNUM, 8 },
    { 0 }
  };

static const struct regcache_map_entry s390_regmap_upper[] =
  {
    { 16, S390_R0_UPPER_REGNUM, 4 },
    { 0 }
  };

static const struct regcache_map_entry s390_regmap_last_break[] =
  {
    { 1, REGCACHE_MAP_SKIP, 4 },
    { 1, S390_LAST_BREAK_REGNUM, 4 },
    { 0 }
  };

static const struct regcache_map_entry s390x_regmap_last_break[] =
  {
    { 1, S390_LAST_BREAK_REGNUM, 8 },
    { 0 }
  };

static const struct regcache_map_entry s390_regmap_system_call[] =
  {
    { 1, S390_SYSTEM_CALL_REGNUM, 4 },
    { 0 }
  };

static const struct regcache_map_entry s390_regmap_tdb[] =
  {
    { 1, S390_TDB_DWORD0_REGNUM, 8 },
    { 1, S390_TDB_ABORT_CODE_REGNUM, 8 },
    { 1, S390_TDB_CONFLICT_TOKEN_REGNUM, 8 },
    { 1, S390_TDB_ATIA_REGNUM, 8 },
    { 12, REGCACHE_MAP_SKIP, 8 },
    { 16, S390_TDB_R0_REGNUM, 8 },
    { 0 }
  };

static const struct regcache_map_entry s390_regmap_vxrs_low[] =
  {
    { 16, S390_V0_LOWER_REGNUM, 8 },
    { 0 }
  };

static const struct regcache_map_entry s390_regmap_vxrs_high[] =
  {
    { 16, S390_V16_REGNUM, 16 },
    { 0 }
  };


/* Supply the TDB regset.  Like regcache_supply_regset, but invalidate
   the TDB registers unless the TDB format field is valid.  */

static void
s390_supply_tdb_regset (const struct regset *regset, struct regcache *regcache,
		    int regnum, const void *regs, size_t len)
{
  ULONGEST tdw;
  enum register_status ret;

  regcache_supply_regset (regset, regcache, regnum, regs, len);
  ret = regcache_cooked_read_unsigned (regcache, S390_TDB_DWORD0_REGNUM, &tdw);
  if (ret != REG_VALID || (tdw >> 56) != 1)
    regcache_supply_regset (regset, regcache, regnum, NULL, len);
}

const struct regset s390_gregset = {
  s390_gregmap,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390_fpregset = {
  s390_fpregmap,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390_upper_regset = {
  s390_regmap_upper,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390_last_break_regset = {
  s390_regmap_last_break,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390x_last_break_regset = {
  s390x_regmap_last_break,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390_system_call_regset = {
  s390_regmap_system_call,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390_tdb_regset = {
  s390_regmap_tdb,
  s390_supply_tdb_regset,
  regcache_collect_regset
};

const struct regset s390_vxrs_low_regset = {
  s390_regmap_vxrs_low,
  regcache_supply_regset,
  regcache_collect_regset
};

const struct regset s390_vxrs_high_regset = {
  s390_regmap_vxrs_high,
  regcache_supply_regset,
  regcache_collect_regset
};


/* Syscall handling.  */

/* Retrieve the syscall number at a ptrace syscall-stop.  Return -1
   upon error. */

static LONGEST
s390_linux_get_syscall_number (struct gdbarch *gdbarch,
			       ptid_t ptid)
{
  struct regcache *regs = get_thread_regcache (ptid);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  ULONGEST pc;
  ULONGEST svc_number = -1;
  unsigned opcode;

  /* Assume that the PC points after the 2-byte SVC instruction.  We
     don't currently support SVC via EXECUTE. */
  regcache_cooked_read_unsigned (regs, tdep->pc_regnum, &pc);
  pc -= 2;
  opcode = read_memory_unsigned_integer ((CORE_ADDR) pc, 1, byte_order);
  if (opcode != op_svc)
    return -1;

  svc_number = read_memory_unsigned_integer ((CORE_ADDR) pc + 1, 1,
					     byte_order);
  if (svc_number == 0)
    regcache_cooked_read_unsigned (regs, S390_R1_REGNUM, &svc_number);

  return svc_number;
}

/* Process record-replay */

static struct linux_record_tdep s390_linux_record_tdep;
static struct linux_record_tdep s390x_linux_record_tdep;

/* Record all registers but PC register for process-record.  */

static int
s390_all_but_pc_registers_record (struct regcache *regcache)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int i;

  for (i = 0; i < 16; i++)
    {
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + i))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + i))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + i))
        return -1;
      if (tdep->gpr_full_regnum != -1)
        if (record_full_arch_list_add_reg (regcache, S390_R0_UPPER_REGNUM + i))
          return -1;
      if (tdep->v0_full_regnum != -1)
        {
          if (record_full_arch_list_add_reg (regcache, S390_V0_LOWER_REGNUM + i))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_V16_REGNUM + i))
            return -1;
        }
    }
  if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
    return -1;
  if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
    return -1;

  return 0;
}

static enum gdb_syscall
s390_canonicalize_syscall (int syscall, enum s390_abi_kind abi)
{
  switch (syscall)
    {
    /* s390 syscall numbers < 222 are mostly the same as x86, so just list
       the exceptions.  */
    case 0:
      return gdb_sys_no_syscall;
    case 7:
      return gdb_sys_restart_syscall;
    /* These syscalls work only on 31-bit.  */
    case 13: /* time */
    case 16: /* lchown[16] */
    case 23: /* setuid[16] */
    case 24: /* getuid[16] */
    case 25: /* stime */
    case 46: /* setgid[16] */
    case 47: /* getgid[16] */
    case 49: /* seteuid[16] */
    case 50: /* getegid[16] */
    case 70: /* setreuid[16] */
    case 71: /* setregid[16] */
    case 76: /* [old_]getrlimit */
    case 80: /* getgroups[16] */
    case 81: /* setgroups[16] */
    case 95: /* fchown[16] */
    case 101: /* ioperm */
    case 138: /* setfsuid[16] */
    case 139: /* setfsgid[16] */
    case 140: /* _llseek */
    case 164: /* setresuid[16] */
    case 165: /* getresuid[16] */
    case 170: /* setresgid[16] */
    case 171: /* getresgid[16] */
    case 182: /* chown[16] */
    case 192: /* mmap2 */
    case 193: /* truncate64 */
    case 194: /* ftruncate64 */
    case 195: /* stat64 */
    case 196: /* lstat64 */
    case 197: /* fstat64 */
    case 221: /* fcntl64 */
      if (abi == ABI_LINUX_S390)
        return (enum gdb_syscall) syscall;
      return gdb_sys_no_syscall;
    /* These syscalls don't exist on s390.  */
    case 17: /* break */
    case 18: /* oldstat */
    case 28: /* oldfstat */
    case 31: /* stty */
    case 32: /* gtty */
    case 35: /* ftime */
    case 44: /* prof */
    case 53: /* lock */
    case 56: /* mpx */
    case 58: /* ulimit */
    case 59: /* oldolduname */
    case 68: /* sgetmask */
    case 69: /* ssetmask */
    case 82: /* [old_]select */
    case 84: /* oldlstat */
    case 98: /* profil */
    case 109: /* olduname */
    case 113: /* vm86old */
    case 123: /* modify_ldt */
    case 166: /* vm86 */
      return gdb_sys_no_syscall;
    case 110:
      return gdb_sys_lookup_dcookie;
    /* Here come the differences.  */
    case 222:
      return gdb_sys_readahead;
    case 223:
      if (abi == ABI_LINUX_S390)
        return gdb_sys_sendfile64;
      return gdb_sys_no_syscall;
    /* 224-235 handled below */
    case 236:
      return gdb_sys_gettid;
    case 237:
      return gdb_sys_tkill;
    case 238:
      return gdb_sys_futex;
    case 239:
      return gdb_sys_sched_setaffinity;
    case 240:
      return gdb_sys_sched_getaffinity;
    case 241:
      return gdb_sys_tgkill;
    /* 242 reserved */
    case 243:
      return gdb_sys_io_setup;
    case 244:
      return gdb_sys_io_destroy;
    case 245:
      return gdb_sys_io_getevents;
    case 246:
      return gdb_sys_io_submit;
    case 247:
      return gdb_sys_io_cancel;
    case 248:
      return gdb_sys_exit_group;
    case 249:
      return gdb_sys_epoll_create;
    case 250:
      return gdb_sys_epoll_ctl;
    case 251:
      return gdb_sys_epoll_wait;
    case 252:
      return gdb_sys_set_tid_address;
    case 253:
      return gdb_sys_fadvise64;
    /* 254-262 handled below */
    /* 263 reserved */
    case 264:
      if (abi == ABI_LINUX_S390)
        return gdb_sys_fadvise64_64;
      return gdb_sys_no_syscall;
    case 265:
      return gdb_sys_statfs64;
    case 266:
      return gdb_sys_fstatfs64;
    case 267:
      return gdb_sys_remap_file_pages;
    /* 268-270 reserved */
    /* 271-277 handled below */
    case 278:
      return gdb_sys_add_key;
    case 279:
      return gdb_sys_request_key;
    case 280:
      return gdb_sys_keyctl;
    case 281:
      return gdb_sys_waitid;
    /* 282-312 handled below */
    case 293:
      if (abi == ABI_LINUX_S390)
        return gdb_sys_fstatat64;
      return gdb_sys_newfstatat;
    /* 313+ not yet supported */
    default:
      {
	int ret;

	/* Most "old" syscalls copied from i386.  */
	if (syscall <= 221)
	  ret = syscall;
	/* xattr syscalls.  */
	else if (syscall >= 224 && syscall <= 235)
	  ret = syscall + 2;
	/* timer syscalls.  */
	else if (syscall >= 254 && syscall <= 262)
	  ret = syscall + 5;
	/* mq_* and kexec_load */
	else if (syscall >= 271 && syscall <= 277)
	  ret = syscall + 6;
	/* ioprio_set .. epoll_pwait */
	else if (syscall >= 282 && syscall <= 312)
	  ret = syscall + 7;
	else
	  ret = gdb_sys_no_syscall;

	return (enum gdb_syscall) ret;
      }
    }
}

static int
s390_linux_syscall_record (struct regcache *regcache, LONGEST syscall_native)
{
  struct gdbarch *gdbarch = get_regcache_arch (regcache);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  int ret;
  enum gdb_syscall syscall_gdb;

  /* On s390, syscall number can be passed either as immediate field of svc
     instruction, or in %r1 (with svc 0).  */
  if (syscall_native == 0)
    regcache_raw_read_signed (regcache, S390_R1_REGNUM, &syscall_native);

  syscall_gdb = s390_canonicalize_syscall (syscall_native, tdep->abi);

  if (syscall_gdb < 0)
    {
      printf_unfiltered (_("Process record and replay target doesn't "
                           "support syscall number %s\n"),
                         plongest (syscall_native));
      return -1;
    }

  if (syscall_gdb == gdb_sys_sigreturn
      || syscall_gdb == gdb_sys_rt_sigreturn)
    {
      if (s390_all_but_pc_registers_record (regcache))
        return -1;
      return 0;
    }

  if (tdep->abi == ABI_LINUX_ZSERIES)
    ret = record_linux_system_call (syscall_gdb, regcache,
                                    &s390x_linux_record_tdep);
  else
    ret = record_linux_system_call (syscall_gdb, regcache,
                                    &s390_linux_record_tdep);

  if (ret)
    return ret;

  /* Record the return value of the system call.  */
  if (record_full_arch_list_add_reg (regcache, S390_R2_REGNUM))
    return -1;

  return 0;
}

static int
s390_linux_record_signal (struct gdbarch *gdbarch, struct regcache *regcache,
                          enum gdb_signal signal)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  /* There are two kinds of signal frames on s390. rt_sigframe is always
     the larger one, so don't even bother with sigframe.  */
  const int sizeof_rt_sigframe = (tdep->abi == ABI_LINUX_ZSERIES ?
                                  160 + 8 + 128 + 1024 : 96 + 8 + 128 + 1000);
  ULONGEST sp;
  int i;

  for (i = 0; i < 16; i++)
    {
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + i))
        return -1;
      if (tdep->gpr_full_regnum != -1)
        if (record_full_arch_list_add_reg (regcache, S390_R0_UPPER_REGNUM + i))
          return -1;
    }
  if (record_full_arch_list_add_reg (regcache, S390_PSWA_REGNUM))
    return -1;
  if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
    return -1;

  /* Record the change in the stack.
     frame-size = sizeof (struct rt_sigframe) + SIGNAL_FRAMESIZE  */
  regcache_raw_read_unsigned (regcache, S390_SP_REGNUM, &sp);
  sp -= sizeof_rt_sigframe;

  if (record_full_arch_list_add_mem (sp, sizeof_rt_sigframe))
    return -1;

  if (record_full_arch_list_add_end ())
    return -1;

  return 0;
}


/* Implement gdbarch_gcc_target_options.  GCC does not know "-m32" or
   "-mcmodel=large".  */

static char *
s390_gcc_target_options (struct gdbarch *gdbarch)
{
  return xstrdup (gdbarch_ptr_bit (gdbarch) == 64 ? "-m64" : "-m31");
}

/* Implement gdbarch_gnu_triplet_regexp.  Target triplets are "s390-*"
   for 31-bit and "s390x-*" for 64-bit, while the BFD arch name is
   always "s390".  Note that an s390x compiler supports "-m31" as
   well.  */

static const char *
s390_gnu_triplet_regexp (struct gdbarch *gdbarch)
{
  return "s390x?";
}

/* Implementation of `gdbarch_stap_is_single_operand', as defined in
   gdbarch.h.  */

static int
s390_stap_is_single_operand (struct gdbarch *gdbarch, const char *s)
{
  return ((isdigit (*s) && s[1] == '(' && s[2] == '%') /* Displacement
							  or indirection.  */
	  || *s == '%' /* Register access.  */
	  || isdigit (*s)); /* Literal number.  */
}


/* Process record and replay helpers.  */

/* Takes the intermediate sum of address calculations and masks off upper
   bits according to current addressing mode.  */

static CORE_ADDR
s390_record_address_mask (struct gdbarch *gdbarch, struct regcache *regcache,
                         CORE_ADDR val) {
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  ULONGEST pswm, pswa;
  int am;
  if (tdep->abi == ABI_LINUX_S390)
    {
      regcache_raw_read_unsigned (regcache, S390_PSWA_REGNUM, &pswa);
      am = pswa >> 31 & 1;
    }
  else
    {
      regcache_raw_read_unsigned (regcache, S390_PSWM_REGNUM, &pswm);
      am = pswm >> 31 & 3;
    }
  switch (am)
    {
    case 0:
      return val & 0xffffff;
    case 1:
      return val & 0x7fffffff;
    case 3:
      return val;
    default:
      fprintf_unfiltered (gdb_stdlog, "Warning: Addressing mode %d used.", am);
      return 0;
    }
}

/* Calculates memory address using pre-calculated index, raw instruction word
   with b and d/dl fields, and raw instruction byte with dh field.  Index and
   dh should be set to 0 if unused.  */

static CORE_ADDR
s390_record_calc_disp_common (struct gdbarch *gdbarch, struct regcache *regcache,
                             ULONGEST x, uint16_t bd, int8_t dh)
{
  uint8_t rb = bd >> 12 & 0xf;
  int32_t d = (bd & 0xfff) | ((int32_t)dh << 12);
  ULONGEST b;
  CORE_ADDR res = d + x;
  if (rb)
    {
      regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + rb, &b);
      res += b;
    }
  return s390_record_address_mask (gdbarch, regcache, res);
}

/* Calculates memory address using raw x, b + d/dl, dh fields from
   instruction.  rx and dh should be set to 0 if unused.  */

static CORE_ADDR
s390_record_calc_disp (struct gdbarch *gdbarch, struct regcache *regcache,
                      uint8_t rx, uint16_t bd, int8_t dh)
{
  ULONGEST x = 0;
  if (rx)
    regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + rx, &x);
  return s390_record_calc_disp_common (gdbarch, regcache, x, bd, dh);
}

/* Calculates memory address for VSCE[GF] instructions.  */

static int
s390_record_calc_disp_vsce (struct gdbarch *gdbarch, struct regcache *regcache,
                            uint8_t vx, uint8_t el, uint8_t es, uint16_t bd,
                            int8_t dh, CORE_ADDR *res)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);
  ULONGEST x;
  gdb_byte buf[16];
  if (tdep->v0_full_regnum == -1 || el * es >= 16)
    return -1;
  if (vx < 16)
    regcache_cooked_read (regcache, tdep->v0_full_regnum + vx, buf);
  else
    regcache_raw_read (regcache, S390_V16_REGNUM + vx - 16, buf);
  x = extract_unsigned_integer (buf + el * es, es, byte_order);
  *res = s390_record_calc_disp_common (gdbarch, regcache, x, bd, dh);
  return 0;
}

/* Calculates memory address for instructions with relative long addressing.  */

static CORE_ADDR
s390_record_calc_rl (struct gdbarch *gdbarch, struct regcache *regcache,
                    CORE_ADDR addr, uint16_t i1, uint16_t i2)
{
  int32_t ri = i1 << 16 | i2;
  return s390_record_address_mask (gdbarch, regcache, addr + (LONGEST)ri * 2);
}

/* Population count helper.  */

static int s390_popcnt (unsigned int x) {
  int res = 0;
  while (x)
    {
      if (x & 1)
        res++;
      x >>= 1;
    }
  return res;
}

/* Record 64-bit register.  */

static int
s390_record_gpr_g (struct gdbarch *gdbarch, struct regcache *regcache, int i)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + i))
    return -1;
  if (tdep->abi == ABI_LINUX_S390)
    if (record_full_arch_list_add_reg (regcache, S390_R0_UPPER_REGNUM + i))
      return -1;
  return 0;
}

/* Record high 32 bits of a register.  */

static int
s390_record_gpr_h (struct gdbarch *gdbarch, struct regcache *regcache, int i)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  if (tdep->abi == ABI_LINUX_S390)
    {
      if (record_full_arch_list_add_reg (regcache, S390_R0_UPPER_REGNUM + i))
        return -1;
    }
  else
    {
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + i))
        return -1;
    }
  return 0;
}

/* Record vector register.  */

static int
s390_record_vr (struct gdbarch *gdbarch, struct regcache *regcache, int i)
{
  if (i < 16)
    {
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + i))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_V0_LOWER_REGNUM + i))
        return -1;
    }
  else
    {
      if (record_full_arch_list_add_reg (regcache, S390_V16_REGNUM + i - 16))
        return -1;
    }
  return 0;
}

static int
s390_process_record (struct gdbarch *gdbarch, struct regcache *regcache,
                    CORE_ADDR addr)
{
  uint16_t insn[3] = {0};
  /* Instruction as bytes.  */
  uint8_t ibyte[6];
  /* Instruction as nibbles.  */
  uint8_t inib[12];
  /* Instruction vector registers.  */
  uint8_t ivec[4];
  CORE_ADDR oaddr, oaddr2, oaddr3;
  ULONGEST tmp;
  int i, n;
  /* if EX/EXRL instruction used, here's the reg parameter */
  int ex = -1;
  enum bfd_endian byte_order = gdbarch_byte_order (gdbarch);

  /* Attempting to use EX or EXRL jumps back here */
ex:

  /* Read instruction.  */
  insn[0] = read_memory_unsigned_integer (addr, 2, byte_order);
  /* If execute was involved, do the adjustment.  */
  if (ex != -1)
    insn[0] |= ex & 0xff;
  /* Two highest bits determine instruction size.  */
  if (insn[0] >= 0x4000)
    insn[1] = read_memory_unsigned_integer (addr+2, 2, byte_order);
  else
    /* Not necessary, but avoids uninitialized variable warnings.  */
    insn[1] = 0;
  if (insn[0] >= 0xc000)
    insn[2] = read_memory_unsigned_integer (addr+4, 2, byte_order);
  else
    insn[2] = 0;
  /* Split instruction into bytes and nibbles.  */
  for (i = 0; i < 3; i++)
    {
      ibyte[i*2] = insn[i] >> 8 & 0xff;
      ibyte[i*2+1] = insn[i] & 0xff;
    }
  for (i = 0; i < 6; i++)
    {
      inib[i*2] = ibyte[i] >> 4 & 0xf;
      inib[i*2+1] = ibyte[i] & 0xf;
    }
  /* Compute vector registers, if applicable.  */
  ivec[0] = (inib[9] >> 3 & 1) << 4 | inib[2];
  ivec[1] = (inib[9] >> 2 & 1) << 4 | inib[3];
  ivec[2] = (inib[9] >> 1 & 1) << 4 | inib[4];
  ivec[3] = (inib[9] >> 0 & 1) << 4 | inib[8];

  switch (ibyte[0])
    {
    /* 0x00 undefined */

    case 0x01:
      /* E-format instruction */
      switch (ibyte[1])
        {
        /* 0x00 undefined */
        /* 0x01 unsupported: PR - program return */
        /* 0x02 unsupported: UPT */
        /* 0x03 undefined */
        /* 0x04 privileged: PTFF - perform timing facility function */
        /* 0x05-0x06 undefined */
        /* 0x07 privileged: SCKPF - set clock programmable field */
        /* 0x08-0x09 undefined */

        case 0x0a: /* PFPO - perform floating point operation */
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          if (!(tmp & 0x80000000u))
            {
              uint8_t ofc = tmp >> 16 & 0xff;
              switch (ofc)
                {
                case 0x00: /* HFP32 */
                case 0x01: /* HFP64 */
                case 0x05: /* BFP32 */
                case 0x06: /* BFP64 */
                case 0x08: /* DFP32 */
                case 0x09: /* DFP64 */
                  if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM))
                    return -1;
                  break;
                case 0x02: /* HFP128 */
                case 0x07: /* BFP128 */
                case 0x0a: /* DFP128 */
                  if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM))
                    return -1;
                  if (record_full_arch_list_add_reg (regcache, S390_F2_REGNUM))
                    return -1;
                  break;
                default:
                  fprintf_unfiltered (gdb_stdlog, "Warning: Unknown PFPO OFC %02x at %s.\n",
                                      ofc, paddress (gdbarch, addr));
                  return -1;
                }

              if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
                return -1;
            }
          if (record_full_arch_list_add_reg (regcache, S390_R1_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0x0b: /* TAM - test address mode */
        case 0x0c: /* SAM24 - set address mode 24 */
        case 0x0d: /* SAM31 - set address mode 31 */
        case 0x0e: /* SAM64 - set address mode 64 */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0x0f-0xfe undefined */

        /* 0xff unsupported: TRAP */

        default:
          goto UNKNOWN_OP;
        }
      break;

    /* 0x02 undefined */
    /* 0x03 undefined */

    case 0x04: /* SPM - set program mask */
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x05: /* BALR - branch and link */
    case 0x45: /* BAL - branch and link */
    case 0x06: /* BCTR - branch on count */
    case 0x46: /* BCT - branch on count */
    case 0x0d: /* BASR - branch and save */
    case 0x4d: /* BAS - branch and save */
    case 0x84: /* BRXH - branch relative on index high */
    case 0x85: /* BRXLE - branch relative on index low or equal */
    case 0x86: /* BXH - branch on index high */
    case 0x87: /* BXLE - branch on index low or equal */
      /* BA[SL]* use native-size destination for linkage info, BCT*, BRX*, BX*
         use 32-bit destination as counter.  */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      break;

    case 0x07: /* BCR - branch on condition */
    case 0x47: /* BC - branch on condition */
      /* No effect other than PC transfer.  */
      break;

    /* 0x08 undefined */
    /* 0x09 undefined */

    case 0x0a:
      /* SVC - supervisor call */
      if (s390_linux_syscall_record (regcache, ibyte[1]))
        return -1;
      break;

    case 0x0b: /* BSM - branch and set mode */
      if (inib[2])
        if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
          return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x0c: /* BASSM - branch and save and set mode */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x0e: /* MVCL - move long [interruptible] */
      regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[2], &tmp);
      oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
      regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[2] | 1), &tmp);
      tmp &= 0xffffff;
      if (record_full_arch_list_add_mem (oaddr, tmp))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[3] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x0f: /* CLCL - compare logical long [interruptible] */
    case 0xa9: /* CLCLE - compare logical long extended [partial] */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[3] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x10: /* LPR - load positive */
    case 0x11: /* LNR - load negative */
    case 0x12: /* LTR - load and test */
    case 0x13: /* LCR - load complement */
    case 0x14: /* NR - and */
    case 0x16: /* OR - or */
    case 0x17: /* XR - xor */
    case 0x1a: /* AR - add */
    case 0x1b: /* SR - subtract */
    case 0x1e: /* ALR - add logical */
    case 0x1f: /* SLR - subtract logical */
    case 0x54: /* N - and */
    case 0x56: /* O - or */
    case 0x57: /* X - xor */
    case 0x5a: /* A - add */
    case 0x5b: /* S - subtract */
    case 0x5e: /* AL - add logical */
    case 0x5f: /* SL - subtract logical */
    case 0x4a: /* AH - add halfword */
    case 0x4b: /* SH - subtract halfword */
    case 0x8a: /* SRA - shift right single */
    case 0x8b: /* SLA - shift left single */
    case 0xbf: /* ICM - insert characters under mask */
      /* 32-bit destination + flags */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x15: /* CLR - compare logical */
    case 0x55: /* CL - compare logical */
    case 0x19: /* CR - compare */
    case 0x29: /* CDR - compare */
    case 0x39: /* CER - compare */
    case 0x49: /* CH - compare halfword */
    case 0x59: /* C - compare */
    case 0x69: /* CD - compare */
    case 0x79: /* CE - compare */
    case 0x91: /* TM - test under mask */
    case 0x95: /* CLI - compare logical */
    case 0xbd: /* CLM - compare logical under mask */
    case 0xd5: /* CLC - compare logical */
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x18: /* LR - load */
    case 0x48: /* LH - load halfword */
    case 0x58: /* L - load */
    case 0x41: /* LA - load address */
    case 0x43: /* IC - insert character */
    case 0x4c: /* MH - multiply halfword */
    case 0x71: /* MS - multiply single */
    case 0x88: /* SRL - shift right single logical */
    case 0x89: /* SLL - shift left single logical */
      /* 32-bit, 8-bit (IC), or native width (LA) destination, no flags */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      break;

    case 0x1c: /* MR - multiply */
    case 0x5c: /* M - multiply */
    case 0x1d: /* DR - divide */
    case 0x5d: /* D - divide */
    case 0x8c: /* SRDL - shift right double logical */
    case 0x8d: /* SLDL - shift left double logical */
      /* 32-bit pair destination, no flags */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
        return -1;
      break;

    case 0x20: /* LPDR - load positive */
    case 0x30: /* LPER - load positive */
    case 0x21: /* LNDR - load negative */
    case 0x31: /* LNER - load negative */
    case 0x22: /* LTDR - load and test */
    case 0x32: /* LTER - load and test */
    case 0x23: /* LCDR - load complement */
    case 0x33: /* LCER - load complement */
    case 0x2a: /* ADR - add */
    case 0x3a: /* AER - add */
    case 0x6a: /* AD - add */
    case 0x7a: /* AE - add */
    case 0x2b: /* SDR - subtract */
    case 0x3b: /* SER - subtract */
    case 0x6b: /* SD - subtract */
    case 0x7b: /* SE - subtract */
    case 0x2e: /* AWR - add unnormalized */
    case 0x3e: /* AUR - add unnormalized */
    case 0x6e: /* AW - add unnormalized */
    case 0x7e: /* AU - add unnormalized */
    case 0x2f: /* SWR - subtract unnormalized */
    case 0x3f: /* SUR - subtract unnormalized */
    case 0x6f: /* SW - subtract unnormalized */
    case 0x7f: /* SU - subtract unnormalized */
      /* float destination + flags */
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x24: /* HDR - halve */
    case 0x34: /* HER - halve */
    case 0x25: /* LDXR - load rounded */
    case 0x35: /* LEDR - load rounded */
    case 0x28: /* LDR - load */
    case 0x38: /* LER - load */
    case 0x68: /* LD - load */
    case 0x78: /* LE - load */
    case 0x2c: /* MDR - multiply */
    case 0x3c: /* MDER - multiply */
    case 0x6c: /* MD - multiply */
    case 0x7c: /* MDE - multiply */
    case 0x2d: /* DDR - divide */
    case 0x3d: /* DER - divide */
    case 0x6d: /* DD - divide */
    case 0x7d: /* DE - divide */
      /* float destination, no flags */
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
        return -1;
      break;

    case 0x26: /* MXR - multiply */
    case 0x27: /* MXDR - multiply */
    case 0x67: /* MXD - multiply */
      /* float pair destination, no flags */
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[2] | 2)))
        return -1;
      break;

    case 0x36: /* AXR - add */
    case 0x37: /* SXR - subtract */
      /* float pair destination + flags */
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[2] | 2)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x40: /* STH - store halfword */
      oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 2))
        return -1;
      break;

    case 0x42: /* STC - store character */
      oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 1))
        return -1;
      break;

    case 0x44: /* EX - execute */
      if (ex != -1)
        {
          fprintf_unfiltered (gdb_stdlog, "Warning: Double execute at %s.\n",
                              paddress (gdbarch, addr));
          return -1;
        }
      addr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
      if (inib[2])
        {
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[2], &tmp);
          ex = tmp & 0xff;
        }
      else
        {
          ex = 0;
        }
      goto ex;

    case 0x4e: /* CVD - convert to decimal */
    case 0x60: /* STD - store */
      oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 8))
        return -1;
      break;

    case 0x4f: /* CVB - convert to binary */
      /* 32-bit gpr destination + FPC (DXC write) */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
        return -1;
      break;

    case 0x50: /* ST - store */
    case 0x70: /* STE - store */
      oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 4))
        return -1;
      break;

    case 0x51: /* LAE - load address extended */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + inib[2]))
        return -1;
      break;

    /* 0x52 undefined */
    /* 0x53 undefined */

    /* 0x61-0x66 undefined */

    /* 0x72-0x77 undefined */

    /* 0x80 privileged: SSM - set system mask */
    /* 0x81 undefined */
    /* 0x82 privileged: LPSW - load PSW */
    /* 0x83 privileged: diagnose */

    case 0x8e: /* SRDA - shift right double */
    case 0x8f: /* SLDA - shift left double */
      /* 32-bit pair destination + flags */
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x90: /* STM - store multiple */
    case 0x9b: /* STAM - store access multiple */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (inib[2] <= inib[3])
        n = inib[3] - inib[2] + 1;
      else
        n = inib[3] + 0x10 - inib[2] + 1;
      if (record_full_arch_list_add_mem (oaddr, n * 4))
        return -1;
      break;

    case 0x92: /* MVI - move */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 1))
        return -1;
      break;

    case 0x93: /* TS - test and set */
    case 0x94: /* NI - and */
    case 0x96: /* OI - or */
    case 0x97: /* XI - xor */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 1))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0x98: /* LM - load multiple */
      for (i = inib[2]; i != inib[3]; i++, i &= 0xf)
        if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + i))
          return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
        return -1;
      break;

    /* 0x99 privileged: TRACE */

    case 0x9a: /* LAM - load access multiple */
      for (i = inib[2]; i != inib[3]; i++, i &= 0xf)
        if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + i))
          return -1;
      if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + inib[3]))
        return -1;
      break;

    /* 0x9c-0x9f privileged and obsolete (old I/O) */
    /* 0xa0-0xa4 undefined */

    case 0xa5:
    case 0xa7:
      /* RI-format instruction */
      switch (ibyte[0] << 4 | inib[3])
        {
        case 0xa50: /* IIHH - insert immediate */
        case 0xa51: /* IIHL - insert immediate */
          /* high 32-bit destination */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          break;

        case 0xa52: /* IILH - insert immediate */
        case 0xa53: /* IILL - insert immediate */
        case 0xa75: /* BRAS - branch relative and save */
        case 0xa76: /* BRCT - branch relative on count */
        case 0xa78: /* LHI - load halfword immediate */
        case 0xa7c: /* MHI - multiply halfword immediate */
          /* 32-bit or native destination */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          break;

        case 0xa54: /* NIHH - and immediate */
        case 0xa55: /* NIHL - and immediate */
        case 0xa58: /* OIHH - or immediate */
        case 0xa59: /* OIHL - or immediate */
          /* high 32-bit destination + flags */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xa56: /* NILH - and immediate */
        case 0xa57: /* NILL - and immediate */
        case 0xa5a: /* OILH - or immediate */
        case 0xa5b: /* OILL - or immediate */
        case 0xa7a: /* AHI - add halfword immediate */
          /* 32-bit destination + flags */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xa5c: /* LLIHH - load logical immediate */
        case 0xa5d: /* LLIHL - load logical immediate */
        case 0xa5e: /* LLILH - load logical immediate */
        case 0xa5f: /* LLILL - load logical immediate */
        case 0xa77: /* BRCTG - branch relative on count */
        case 0xa79: /* LGHI - load halfword immediate */
        case 0xa7d: /* MGHI - multiply halfword immediate */
          /* 64-bit destination */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          break;

        case 0xa70: /* TMLH - test under mask */
        case 0xa71: /* TMLL - test under mask */
        case 0xa72: /* TMHH - test under mask */
        case 0xa73: /* TMHL - test under mask */
        case 0xa7e: /* CHI - compare halfword immediate */
        case 0xa7f: /* CGHI - compare halfword immediate */
          /* flags only */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xa74: /* BRC - branch relative on condition */
          /* no register change */
          break;

        case 0xa7b: /* AGHI - add halfword immediate */
          /* 64-bit destination + flags */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        default:
          goto UNKNOWN_OP;
        }
      break;

    /* 0xa6 undefined */

    case 0xa8: /* MVCLE - move long extended [partial] */
      regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[2], &tmp);
      oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
      regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[2] | 1), &tmp);
      if (record_full_arch_list_add_mem (oaddr, tmp))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[3] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    /* 0xaa-0xab undefined */
    /* 0xac privileged: STNSM - store then and system mask */
    /* 0xad privileged: STOSM - store then or system mask */
    /* 0xae privileged: SIGP - signal processor */
    /* 0xaf unsupported: MC - monitor call */
    /* 0xb0 undefined */
    /* 0xb1 privileged: LRA - load real address */

    case 0xb2:
    case 0xb3:
    case 0xb9:
      /* S/RRD/RRE/RRF/IE-format instruction */
      switch (insn[0])
        {
        /* 0xb200-0xb204 undefined or privileged */

        case 0xb205: /* STCK - store clock */
        case 0xb27c: /* STCKF - store clock fast */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb206-0xb219 undefined, privileged, or unsupported */
        /* 0xb21a unsupported: CFC */
        /* 0xb21b-0xb221 undefined or privileged */

        case 0xb222: /* IPM - insert program mask */
        case 0xb24f: /* EAR - extract access */
        case 0xb252: /* MSR - multiply single */
        case 0xb2ec: /* ETND - extract transaction nesting depth */
        case 0xb38c: /* EFPC - extract fpc */
        case 0xb91f: /* LRVR - load reversed */
        case 0xb926: /* LBR - load byte */
        case 0xb927: /* LHR - load halfword */
        case 0xb994: /* LLCR - load logical character */
        case 0xb995: /* LLHR - load logical halfword */
        case 0xb9f2: /* LOCR - load on condition */
          /* 32-bit gpr destination */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          break;

        /* 0xb223-0xb22c privileged or unsupported */

        case 0xb22d: /* DXR - divide */
        case 0xb325: /* LXDR - load lengthened */
        case 0xb326: /* LXER - load lengthened */
        case 0xb336: /* SQXR - square root */
        case 0xb365: /* LXR - load */
        case 0xb367: /* FIXR - load fp integer */
        case 0xb376: /* LZXR - load zero */
        case 0xb3b6: /* CXFR - convert from fixed */
        case 0xb3c6: /* CXGR - convert from fixed */
        case 0xb3fe: /* IEXTR - insert biased exponent */
          /* float pair destination */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[6] | 2)))
            return -1;
          break;

        /* 0xb22e-0xb240 undefined, privileged, or unsupported */

        case 0xb241: /* CKSM - checksum [partial] */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb242-0xb243 undefined */

        case 0xb244: /* SQDR - square root */
        case 0xb245: /* SQER - square root */
        case 0xb324: /* LDER - load lengthened */
        case 0xb337: /* MEER - multiply */
        case 0xb366: /* LEXR - load rounded */
        case 0xb370: /* LPDFR - load positive */
        case 0xb371: /* LNDFR - load negative */
        case 0xb372: /* CSDFR - copy sign */
        case 0xb373: /* LCDFR - load complement */
        case 0xb374: /* LZER - load zero */
        case 0xb375: /* LZDR - load zero */
        case 0xb377: /* FIER - load fp integer */
        case 0xb37f: /* FIDR - load fp integer */
        case 0xb3b4: /* CEFR - convert from fixed */
        case 0xb3b5: /* CDFR - convert from fixed */
        case 0xb3c1: /* LDGR - load fpr from gr */
        case 0xb3c4: /* CEGR - convert from fixed */
        case 0xb3c5: /* CDGR - convert from fixed */
        case 0xb3f6: /* IEDTR - insert biased exponent */
          /* float destination */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          break;

        /* 0xb246-0xb24c: privileged or unsupported */

        case 0xb24d: /* CPYA - copy access */
        case 0xb24e: /* SAR - set access */
          if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + inib[6]))
            return -1;
          break;

        /* 0xb250-0xb251 undefined or privileged */
        /* 0xb253-0xb254 undefined or privileged */

        case 0xb255: /* MVST - move string [partial] */
          {
            uint8_t end;
            gdb_byte cur;
            ULONGEST num = 0;
            /* Read ending byte.  */
            regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
            end = tmp & 0xff;
            /* Get address of second operand.  */
            regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[7], &tmp);
            oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
            /* Search for ending byte and compute length.  */
            do {
              num++;
              if (target_read_memory (oaddr, &cur, 1))
                return -1;
              oaddr++;
            } while (cur != end);
            /* Get address of first operand and record it.  */
            regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
            oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
            if (record_full_arch_list_add_mem (oaddr, num))
              return -1;
            /* Record the registers.  */
            if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
              return -1;
            if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
              return -1;
            if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
              return -1;
          }
          break;

        /* 0xb256 undefined */

        case 0xb257: /* CUSE - compare until substring equal [interruptible] */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb258-0xb25c undefined, privileged, or unsupported */

        case 0xb25d: /* CLST - compare logical string [partial] */
        case 0xb25e: /* SRST - search string [partial] */
        case 0xb9be: /* SRSTU - search string unicode [partial] */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb25f-0xb262 undefined */

        case 0xb263: /* CMPSC - compression call [interruptible] */
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[6] | 1), &tmp);
          if (record_full_arch_list_add_mem (oaddr, tmp))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R1_REGNUM))
            return -1;
          /* DXC may be written */
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb264-0xb277 undefined, privileged, or unsupported */

        case 0xb278: /* STCKE - store clock extended */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 16))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb279-0xb27b undefined or unsupported */
        /* 0xb27d-0xb298 undefined or privileged */

        case 0xb299: /* SRNM - set rounding mode */
        case 0xb2b8: /* SRNMB - set bfp rounding mode */
        case 0xb2b9: /* SRNMT - set dfp rounding mode */
        case 0xb29d: /* LFPC - load fpc */
        case 0xb2bd: /* LFAS - load fpc and signal */
        case 0xb384: /* SFPC - set fpc */
        case 0xb385: /* SFASR - set fpc and signal */
        case 0xb960: /* CGRT - compare and trap */
        case 0xb961: /* CLGRT - compare logical and trap */
        case 0xb972: /* CRT - compare and trap */
        case 0xb973: /* CLRT - compare logical and trap */
          /* fpc only - including possible DXC write for trapping insns */
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xb29a-0xb29b undefined */

        case 0xb29c: /* STFPC - store fpc */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          break;

        /* 0xb29e-0xb2a4 undefined */

        case 0xb2a5: /* TRE - translate extended [partial] */
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[6] | 1), &tmp);
          if (record_full_arch_list_add_mem (oaddr, tmp))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb2a6: /* CU21 - convert UTF-16 to UTF-8 [partial] */
        case 0xb2a7: /* CU12 - convert UTF-8 to UTF-16 [partial] */
        case 0xb9b0: /* CU14 - convert UTF-8 to UTF-32 [partial] */
        case 0xb9b1: /* CU24 - convert UTF-16 to UTF-32 [partial] */
        case 0xb9b2: /* CU41 - convert UTF-32 to UTF-8 [partial] */
        case 0xb9b3: /* CU42 - convert UTF-32 to UTF-16 [partial] */
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[6] | 1), &tmp);
          if (record_full_arch_list_add_mem (oaddr, tmp))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb2a8-0xb2af undefined */

        case 0xb2b0: /* STFLE - store facility list extended */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0xff;
          if (record_full_arch_list_add_mem (oaddr, 8 * (tmp + 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb2b1-0xb2b7 undefined or privileged */
        /* 0xb2ba-0xb2bc undefined */
        /* 0xb2be-0xb2e7 undefined */
        /* 0xb2e9-0xb2eb undefined */
        /* 0xb2ed-0xb2f7 undefined */
        /* 0xb2f8 unsupported: TEND */
        /* 0xb2f9 undefined */

        case 0xb2e8: /* PPA - perform processor assist */
        case 0xb2fa: /* NIAI - next instruction access intent */
          /* no visible effects */
          break;

        /* 0xb2fb undefined */
        /* 0xb2fc unsupported: TABORT */
        /* 0xb2fd-0xb2fe undefined */
        /* 0xb2ff unsupported: TRAP */

        case 0xb300: /* LPEBR - load positive */
        case 0xb301: /* LNEBR - load negative */
        case 0xb303: /* LCEBR - load complement */
        case 0xb310: /* LPDBR - load positive */
        case 0xb311: /* LNDBR - load negative */
        case 0xb313: /* LCDBR - load complement */
        case 0xb350: /* TBEDR - convert hfp to bfp */
        case 0xb351: /* TBDR - convert hfp to bfp */
        case 0xb358: /* THDER - convert bfp to hfp */
        case 0xb359: /* THDR - convert bfp to hfp */
          /* float destination + flags */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb304: /* LDEBR - load lengthened */
        case 0xb30c: /* MDEBR - multiply */
        case 0xb30d: /* DEBR - divide */
        case 0xb314: /* SQEBR - square root */
        case 0xb315: /* SQDBR - square root */
        case 0xb317: /* MEEBR - multiply */
        case 0xb31c: /* MDBR - multiply */
        case 0xb31d: /* DDBR - divide */
        case 0xb344: /* LEDBRA - load rounded */
        case 0xb345: /* LDXBRA - load rounded */
        case 0xb346: /* LEXBRA - load rounded */
        case 0xb357: /* FIEBRA - load fp integer */
        case 0xb35f: /* FIDBRA - load fp integer */
        case 0xb390: /* CELFBR - convert from logical */
        case 0xb391: /* CDLFBR - convert from logical */
        case 0xb394: /* CEFBR - convert from fixed */
        case 0xb395: /* CDFBR - convert from fixed */
        case 0xb3a0: /* CELGBR - convert from logical */
        case 0xb3a1: /* CDLGBR - convert from logical */
        case 0xb3a4: /* CEGBR - convert from fixed */
        case 0xb3a5: /* CDGBR - convert from fixed */
        case 0xb3d0: /* MDTR - multiply */
        case 0xb3d1: /* DDTR - divide */
        case 0xb3d4: /* LDETR - load lengthened */
        case 0xb3d5: /* LEDTR - load lengthened */
        case 0xb3d7: /* FIDTR - load fp integer */
        case 0xb3dd: /* LDXTR - load lengthened */
        case 0xb3f1: /* CDGTR - convert from fixed */
        case 0xb3f2: /* CDUTR - convert from unsigned packed */
        case 0xb3f3: /* CDSTR - convert from signed packed */
        case 0xb3f5: /* QADTR - quantize */
        case 0xb3f7: /* RRDTR - reround */
        case 0xb951: /* CDFTR - convert from fixed */
        case 0xb952: /* CDLGTR - convert from logical */
        case 0xb953: /* CDLFTR - convert from logical */
          /* float destination + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xb305: /* LXDBR - load lengthened */
        case 0xb306: /* LXEBR - load lengthened */
        case 0xb307: /* MXDBR - multiply */
        case 0xb316: /* SQXBR - square root */
        case 0xb34c: /* MXBR - multiply */
        case 0xb34d: /* DXBR - divide */
        case 0xb347: /* FIXBRA - load fp integer */
        case 0xb392: /* CXLFBR - convert from logical */
        case 0xb396: /* CXFBR - convert from fixed */
        case 0xb3a2: /* CXLGBR - convert from logical */
        case 0xb3a6: /* CXGBR - convert from fixed */
        case 0xb3d8: /* MXTR - multiply */
        case 0xb3d9: /* DXTR - divide */
        case 0xb3dc: /* LXDTR - load lengthened */
        case 0xb3df: /* FIXTR - load fp integer */
        case 0xb3f9: /* CXGTR - convert from fixed */
        case 0xb3fa: /* CXUTR - convert from unsigned packed */
        case 0xb3fb: /* CXSTR - convert from signed packed */
        case 0xb3fd: /* QAXTR - quantize */
        case 0xb3ff: /* RRXTR - reround */
        case 0xb959: /* CXFTR - convert from fixed */
        case 0xb95a: /* CXLGTR - convert from logical */
        case 0xb95b: /* CXLFTR - convert from logical */
          /* float pair destination + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[6] | 2)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xb308: /* KEBR - compare and signal */
        case 0xb309: /* CEBR - compare */
        case 0xb318: /* KDBR - compare and signal */
        case 0xb319: /* CDBR - compare */
        case 0xb348: /* KXBR - compare and signal */
        case 0xb349: /* CXBR - compare */
        case 0xb3e0: /* KDTR - compare and signal */
        case 0xb3e4: /* CDTR - compare */
        case 0xb3e8: /* KXTR - compare and signal */
        case 0xb3ec: /* CXTR - compare */
          /* flags + fpc only */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xb302: /* LTEBR - load and test */
        case 0xb312: /* LTDBR - load and test */
        case 0xb30a: /* AEBR - add */
        case 0xb30b: /* SEBR - subtract */
        case 0xb31a: /* ADBR - add */
        case 0xb31b: /* SDBR - subtract */
        case 0xb3d2: /* ADTR - add */
        case 0xb3d3: /* SDTR - subtract */
        case 0xb3d6: /* LTDTR - load and test */
          /* float destination + flags + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xb30e: /* MAEBR - multiply and add */
        case 0xb30f: /* MSEBR - multiply and subtract */
        case 0xb31e: /* MADBR - multiply and add */
        case 0xb31f: /* MSDBR - multiply and subtract */
          /* float destination [RRD] + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[4]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xb320-0xb323 undefined */
        /* 0xb327-0xb32d undefined */

        case 0xb32e: /* MAER - multiply and add */
        case 0xb32f: /* MSER - multiply and subtract */
        case 0xb338: /* MAYLR - multiply and add unnormalized */
        case 0xb339: /* MYLR - multiply unnormalized */
        case 0xb33c: /* MAYHR - multiply and add unnormalized */
        case 0xb33d: /* MYHR - multiply unnormalized */
        case 0xb33e: /* MADR - multiply and add */
        case 0xb33f: /* MSDR - multiply and subtract */
          /* float destination [RRD] */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[4]))
            return -1;
          break;

        /* 0xb330-0xb335 undefined */

        case 0xb33a: /* MAYR - multiply and add unnormalized */
        case 0xb33b: /* MYR - multiply unnormalized */
          /* float pair destination [RRD] */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[4]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[4] | 2)))
            return -1;
          break;

        case 0xb340: /* LPXBR - load positive */
        case 0xb341: /* LNXBR - load negative */
        case 0xb343: /* LCXBR - load complement */
        case 0xb360: /* LPXR - load positive */
        case 0xb361: /* LNXR - load negative */
        case 0xb362: /* LTXR - load and test */
        case 0xb363: /* LCXR - load complement */
          /* float pair destination + flags */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[6] | 2)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb342: /* LTXBR - load and test */
        case 0xb34a: /* AXBR - add */
        case 0xb34b: /* SXBR - subtract */
        case 0xb3da: /* AXTR - add */
        case 0xb3db: /* SXTR - subtract */
        case 0xb3de: /* LTXTR - load and test */
          /* float pair destination + flags + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[6] | 2)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xb34e-0xb34f undefined */
        /* 0xb352 undefined */

        case 0xb353: /* DIEBR - divide to integer */
        case 0xb35b: /* DIDBR - divide to integer */
          /* two float destinations + flags + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[4]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xb354-0xb356 undefined */
        /* 0xb35a undefined */

        /* 0xb35c-0xb35e undefined */
        /* 0xb364 undefined */
        /* 0xb368 undefined */

        case 0xb369: /* CXR - compare */
        case 0xb3f4: /* CEDTR - compare biased exponent */
        case 0xb3fc: /* CEXTR - compare biased exponent */
        case 0xb920: /* CGR - compare */
        case 0xb921: /* CLGR - compare logical */
        case 0xb930: /* CGFR - compare */
        case 0xb931: /* CLGFR - compare logical */
        case 0xb9cd: /* CHHR - compare high */
        case 0xb9cf: /* CLHHR - compare logical high */
        case 0xb9dd: /* CHLR - compare high */
        case 0xb9df: /* CLHLR - compare logical high */
          /* flags only */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb36a-0xb36f undefined */
        /* 0xb377-0xb37e undefined */
        /* 0xb380-0xb383 undefined */
        /* 0xb386-0xb38b undefined */
        /* 0xb38d-0xb38f undefined */
        /* 0xb393 undefined */
        /* 0xb397 undefined */

        case 0xb398: /* CFEBR - convert to fixed */
        case 0xb399: /* CFDBR - convert to fixed */
        case 0xb39a: /* CFXBR - convert to fixed */
        case 0xb39c: /* CLFEBR - convert to logical */
        case 0xb39d: /* CLFDBR - convert to logical */
        case 0xb39e: /* CLFXBR - convert to logical */
        case 0xb941: /* CFDTR - convert to fixed */
        case 0xb949: /* CFXTR - convert to fixed */
        case 0xb943: /* CLFDTR - convert to logical */
        case 0xb94b: /* CLFXTR - convert to logical */
          /* 32-bit gpr destination + flags + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xb39b undefined */
        /* 0xb39f undefined */

        /* 0xb3a3 undefined */
        /* 0xb3a7 undefined */

        case 0xb3a8: /* CGEBR - convert to fixed */
        case 0xb3a9: /* CGDBR - convert to fixed */
        case 0xb3aa: /* CGXBR - convert to fixed */
        case 0xb3ac: /* CLGEBR - convert to logical */
        case 0xb3ad: /* CLGDBR - convert to logical */
        case 0xb3ae: /* CLGXBR - convert to logical */
        case 0xb3e1: /* CGDTR - convert to fixed */
        case 0xb3e9: /* CGXTR - convert to fixed */
        case 0xb942: /* CLGDTR - convert to logical */
        case 0xb94a: /* CLGXTR - convert to logical */
          /* 64-bit gpr destination + flags + fpc */
          if (s390_record_gpr_g (gdbarch, regcache, inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xb3ab undefined */
        /* 0xb3af-0xb3b3 undefined */
        /* 0xb3b7 undefined */

        case 0xb3b8: /* CFER - convert to fixed */
        case 0xb3b9: /* CFDR - convert to fixed */
        case 0xb3ba: /* CFXR - convert to fixed */
        case 0xb998: /* ALCR - add logical with carry */
        case 0xb999: /* SLBR - subtract logical with borrow */
        case 0xb9f4: /* NRK - and */
        case 0xb9f6: /* ORK - or */
        case 0xb9f7: /* XRK - xor */
        case 0xb9f8: /* ARK - add */
        case 0xb9f9: /* SRK - subtract */
        case 0xb9fa: /* ALRK - add logical */
        case 0xb9fb: /* SLRK - subtract logical */
          /* 32-bit gpr destination + flags */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb3c8: /* CGER - convert to fixed */
        case 0xb3c9: /* CGDR - convert to fixed */
        case 0xb3ca: /* CGXR - convert to fixed */
        case 0xb900: /* LPGR - load positive */
        case 0xb901: /* LNGR - load negative */
        case 0xb902: /* LTGR - load and test */
        case 0xb903: /* LCGR - load complement */
        case 0xb908: /* AGR - add */
        case 0xb909: /* SGR - subtract */
        case 0xb90a: /* ALGR - add logical */
        case 0xb90b: /* SLGR - subtract logical */
        case 0xb910: /* LPGFR - load positive */
        case 0xb911: /* LNGFR - load negative */
        case 0xb912: /* LTGFR - load and test */
        case 0xb913: /* LCGFR - load complement */
        case 0xb918: /* AGFR - add */
        case 0xb919: /* SGFR - subtract */
        case 0xb91a: /* ALGFR - add logical */
        case 0xb91b: /* SLGFR - subtract logical */
        case 0xb980: /* NGR - and */
        case 0xb981: /* OGR - or */
        case 0xb982: /* XGR - xor */
        case 0xb988: /* ALCGR - add logical with carry */
        case 0xb989: /* SLBGR - subtract logical with borrow */
        case 0xb9e1: /* POPCNT - population count */
        case 0xb9e4: /* NGRK - and */
        case 0xb9e6: /* OGRK - or */
        case 0xb9e7: /* XGRK - xor */
        case 0xb9e8: /* AGRK - add */
        case 0xb9e9: /* SGRK - subtract */
        case 0xb9ea: /* ALGRK - add logical */
        case 0xb9eb: /* SLGRK - subtract logical */
          /* 64-bit gpr destination + flags */
          if (s390_record_gpr_g (gdbarch, regcache, inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb3bb-0xb3c0 undefined */
        /* 0xb3c2-0xb3c3 undefined */
        /* 0xb3c7 undefined */
        /* 0xb3cb-0xb3cc undefined */

        case 0xb3cd: /* LGDR - load gr from fpr */
        case 0xb3e2: /* CUDTR - convert to unsigned packed */
        case 0xb3e3: /* CSDTR - convert to signed packed */
        case 0xb3e5: /* EEDTR - extract biased exponent */
        case 0xb3e7: /* ESDTR - extract significance */
        case 0xb3ed: /* EEXTR - extract biased exponent */
        case 0xb3ef: /* ESXTR - extract significance */
        case 0xb904: /* LGR - load */
        case 0xb906: /* LGBR - load byte */
        case 0xb907: /* LGHR - load halfword */
        case 0xb90c: /* MSGR - multiply single */
        case 0xb90f: /* LRVGR - load reversed */
        case 0xb914: /* LGFR - load */
        case 0xb916: /* LLGFR - load logical */
        case 0xb917: /* LLGTR - load logical thirty one bits */
        case 0xb91c: /* MSGFR - load */
        case 0xb946: /* BCTGR - branch on count */
        case 0xb984: /* LLGCR - load logical character */
        case 0xb985: /* LLGHR - load logical halfword */
        case 0xb9e2: /* LOCGR - load on condition */
          /* 64-bit gpr destination  */
          if (s390_record_gpr_g (gdbarch, regcache, inib[6]))
            return -1;
          break;

        /* 0xb3ce-0xb3cf undefined */
        /* 0xb3e6 undefined */

        case 0xb3ea: /* CUXTR - convert to unsigned packed */
        case 0xb3eb: /* CSXTR - convert to signed packed */
        case 0xb90d: /* DSGR - divide single */
        case 0xb91d: /* DSGFR - divide single */
        case 0xb986: /* MLGR - multiply logical */
        case 0xb987: /* DLGR - divide logical */
          /* 64-bit gpr pair destination  */
          if (s390_record_gpr_g (gdbarch, regcache, inib[6]))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[6] | 1))
            return -1;
          break;

        /* 0xb3ee undefined */
        /* 0xb3f0 undefined */
        /* 0xb3f8 undefined */

        /* 0xb905 privileged */

        /* 0xb90e unsupported: EREGG */

        /* 0xb915 undefined */

        case 0xb91e: /* KMAC - compute message authentication code [partial] */
          regcache_raw_read_unsigned (regcache, S390_R1_REGNUM, &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0xff;
          switch (tmp)
            {
              case 0x00: /* KMAC-Query */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x01: /* KMAC-DEA */
              case 0x02: /* KMAC-TDEA-128 */
              case 0x03: /* KMAC-TDEA-192 */
              case 0x09: /* KMAC-Encrypted-DEA */
              case 0x0a: /* KMAC-Encrypted-TDEA-128 */
              case 0x0b: /* KMAC-Encrypted-TDEA-192 */
                if (record_full_arch_list_add_mem (oaddr, 8))
                  return -1;
                break;

              case 0x12: /* KMAC-AES-128 */
              case 0x13: /* KMAC-AES-192 */
              case 0x14: /* KMAC-AES-256 */
              case 0x1a: /* KMAC-Encrypted-AES-128 */
              case 0x1b: /* KMAC-Encrypted-AES-192 */
              case 0x1c: /* KMAC-Encrypted-AES-256 */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              default:
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown KMAC function %02x at %s.\n",
                                    (int)tmp, paddress (gdbarch, addr));
                return -1;
            }
          if (tmp != 0)
            {
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
                return -1;
            }
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb922-0xb924 undefined */
        /* 0xb925 privileged */
        /* 0xb928 privileged */
        /* 0xb929 undefined */

        case 0xb92a: /* KMF - cipher message with cipher feedback [partial] */
        case 0xb92b: /* KMO - cipher message with output feedback [partial] */
        case 0xb92f: /* KMC - cipher message with chaining [partial] */
          regcache_raw_read_unsigned (regcache, S390_R1_REGNUM, &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0x7f;
          switch (tmp)
            {
              case 0x00: /* KM*-Query */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x01: /* KM*-DEA */
              case 0x02: /* KM*-TDEA-128 */
              case 0x03: /* KM*-TDEA-192 */
              case 0x09: /* KM*-Encrypted-DEA */
              case 0x0a: /* KM*-Encrypted-TDEA-128 */
              case 0x0b: /* KM*-Encrypted-TDEA-192 */
                if (record_full_arch_list_add_mem (oaddr, 8))
                  return -1;
                break;

              case 0x12: /* KM*-AES-128 */
              case 0x13: /* KM*-AES-192 */
              case 0x14: /* KM*-AES-256 */
              case 0x1a: /* KM*-Encrypted-AES-128 */
              case 0x1b: /* KM*-Encrypted-AES-192 */
              case 0x1c: /* KM*-Encrypted-AES-256 */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x43: /* KMC-PRNG */
                /* Only valid for KMC.  */
                if (insn[0] == 0xb92f)
                  {
                    if (record_full_arch_list_add_mem (oaddr, 8))
                      return -1;
                    break;
                  }
                /* For other instructions, fallthru.  */
              default:
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown KM* function %02x at %s.\n",
                                    (int)tmp, paddress (gdbarch, addr));
                return -1;
            }
          if (tmp != 0)
            {
              regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
              oaddr2 = s390_record_address_mask (gdbarch, regcache, tmp);
              regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[7] | 1), &tmp);
              if (record_full_arch_list_add_mem (oaddr2, tmp))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
                return -1;
            }
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb92c: /* PCC - perform cryptographic computation [partial] */
          regcache_raw_read_unsigned (regcache, S390_R1_REGNUM, &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0x7f;
          switch (tmp)
            {
              case 0x00: /* PCC-Query */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x01: /* PCC-Compute-Last-Block-CMAC-Using-DEA */
              case 0x02: /* PCC-Compute-Last-Block-CMAC-Using-TDEA-128 */
              case 0x03: /* PCC-Compute-Last-Block-CMAC-Using-TDEA-192 */
              case 0x09: /* PCC-Compute-Last-Block-CMAC-Using-Encrypted-DEA */
              case 0x0a: /* PCC-Compute-Last-Block-CMAC-Using-Encrypted-TDEA-128 */
              case 0x0b: /* PCC-Compute-Last-Block-CMAC-Using-Encrypted-TDEA-192 */
                if (record_full_arch_list_add_mem (oaddr + 0x10, 8))
                  return -1;
                break;

              case 0x12: /* PCC-Compute-Last-Block-CMAC-Using-AES-128 */
              case 0x13: /* PCC-Compute-Last-Block-CMAC-Using-AES-192 */
              case 0x14: /* PCC-Compute-Last-Block-CMAC-Using-AES-256 */
              case 0x1a: /* PCC-Compute-Last-Block-CMAC-Using-Encrypted-AES-128 */
              case 0x1b: /* PCC-Compute-Last-Block-CMAC-Using-Encrypted-AES-192 */
              case 0x1c: /* PCC-Compute-Last-Block-CMAC-Using-Encrypted-AES-256 */
                if (record_full_arch_list_add_mem (oaddr + 0x18, 16))
                  return -1;
                break;

              case 0x32: /* PCC-Compute-XTS-Parameter-Using-AES-128 */
                if (record_full_arch_list_add_mem (oaddr + 0x30, 32))
                  return -1;
                break;

              case 0x34: /* PCC-Compute-XTS-Parameter-Using-AES-256 */
                if (record_full_arch_list_add_mem (oaddr + 0x40, 32))
                  return -1;
                break;

              case 0x3a: /* PCC-Compute-XTS-Parameter-Using-Encrypted-AES-128 */
                if (record_full_arch_list_add_mem (oaddr + 0x50, 32))
                  return -1;
                break;

              case 0x3c: /* PCC-Compute-XTS-Parameter-Using-Encrypted-AES-256 */
                if (record_full_arch_list_add_mem (oaddr + 0x60, 32))
                  return -1;
                break;

              default:
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown PCC function %02x at %s.\n",
                                    (int)tmp, paddress (gdbarch, addr));
                return -1;
            }
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb92d: /* KMCTR - cipher message with counter [partial] */
          regcache_raw_read_unsigned (regcache, S390_R1_REGNUM, &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0x7f;
          switch (tmp)
            {
              case 0x00: /* KMCTR-Query */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x01: /* KMCTR-DEA */
              case 0x02: /* KMCTR-TDEA-128 */
              case 0x03: /* KMCTR-TDEA-192 */
              case 0x09: /* KMCTR-Encrypted-DEA */
              case 0x0a: /* KMCTR-Encrypted-TDEA-128 */
              case 0x0b: /* KMCTR-Encrypted-TDEA-192 */
              case 0x12: /* KMCTR-AES-128 */
              case 0x13: /* KMCTR-AES-192 */
              case 0x14: /* KMCTR-AES-256 */
              case 0x1a: /* KMCTR-Encrypted-AES-128 */
              case 0x1b: /* KMCTR-Encrypted-AES-192 */
              case 0x1c: /* KMCTR-Encrypted-AES-256 */
                break;

              default:
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown KMCTR function %02x at %s.\n",
                                    (int)tmp, paddress (gdbarch, addr));
                return -1;
            }
          if (tmp != 0)
            {
              regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
              oaddr2 = s390_record_address_mask (gdbarch, regcache, tmp);
              regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[7] | 1), &tmp);
              if (record_full_arch_list_add_mem (oaddr2, tmp))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[4]))
                return -1;
            }
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb92e: /* KM - cipher message [partial] */
          regcache_raw_read_unsigned (regcache, S390_R1_REGNUM, &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0x7f;
          switch (tmp)
            {
              case 0x00: /* KM-Query */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x01: /* KM-DEA */
              case 0x02: /* KM-TDEA-128 */
              case 0x03: /* KM-TDEA-192 */
              case 0x09: /* KM-Encrypted-DEA */
              case 0x0a: /* KM-Encrypted-TDEA-128 */
              case 0x0b: /* KM-Encrypted-TDEA-192 */
              case 0x12: /* KM-AES-128 */
              case 0x13: /* KM-AES-192 */
              case 0x14: /* KM-AES-256 */
              case 0x1a: /* KM-Encrypted-AES-128 */
              case 0x1b: /* KM-Encrypted-AES-192 */
              case 0x1c: /* KM-Encrypted-AES-256 */
                break;

              case 0x32: /* KM-XTS-AES-128 */
                if (record_full_arch_list_add_mem (oaddr + 0x10, 16))
                  return -1;
                break;

              case 0x34: /* KM-XTS-AES-256 */
                if (record_full_arch_list_add_mem (oaddr + 0x20, 16))
                  return -1;
                break;

              case 0x3a: /* KM-XTS-Encrypted-AES-128 */
                if (record_full_arch_list_add_mem (oaddr + 0x30, 16))
                  return -1;
                break;

              case 0x3c: /* KM-XTS-Encrypted-AES-256 */
                if (record_full_arch_list_add_mem (oaddr + 0x40, 16))
                  return -1;
                break;

              default:
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown KM function %02x at %s.\n",
                                    (int)tmp, paddress (gdbarch, addr));
                return -1;
            }
          if (tmp != 0)
            {
              regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
              oaddr2 = s390_record_address_mask (gdbarch, regcache, tmp);
              regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[7] | 1), &tmp);
              if (record_full_arch_list_add_mem (oaddr2, tmp))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
                return -1;
            }
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb932-0xb93b undefined */

        case 0xb93c: /* PPNO - perform pseudorandom number operation [partial] */
          regcache_raw_read_unsigned (regcache, S390_R1_REGNUM, &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0xff;
          switch (tmp)
            {
              case 0x00: /* PPNO-Query */
              case 0x80: /* PPNO-Query */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x03: /* PPNO-SHA-512-DRNG - generate */
                if (record_full_arch_list_add_mem (oaddr, 240))
                  return -1;
                regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
                oaddr2 = s390_record_address_mask (gdbarch, regcache, tmp);
                regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[6] | 1), &tmp);
                if (record_full_arch_list_add_mem (oaddr2, tmp))
                  return -1;
                if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
                  return -1;
                if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
                  return -1;
                break;

              case 0x83: /* PPNO-SHA-512-DRNG - seed */
                if (record_full_arch_list_add_mem (oaddr, 240))
                  return -1;
                if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
                  return -1;
                if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
                  return -1;
                break;

              default:
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown PPNO function %02x at %s.\n",
                                    (int)tmp, paddress (gdbarch, addr));
                return -1;
            }
          /* DXC may be written */
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb93d undefined */

        case 0xb93e: /* KIMD - compute intermediate message digest [partial] */
        case 0xb93f: /* KLMD - compute last message digest [partial] */
          regcache_raw_read_unsigned (regcache, S390_R1_REGNUM, &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
          tmp &= 0xff;
          switch (tmp)
            {
              case 0x00: /* K*MD-Query */
                if (record_full_arch_list_add_mem (oaddr, 16))
                  return -1;
                break;

              case 0x01: /* K*MD-SHA-1 */
                if (record_full_arch_list_add_mem (oaddr, 20))
                  return -1;
                break;

              case 0x02: /* K*MD-SHA-256 */
                if (record_full_arch_list_add_mem (oaddr, 32))
                  return -1;
                break;

              case 0x03: /* K*MD-SHA-512 */
                if (record_full_arch_list_add_mem (oaddr, 64))
                  return -1;
                break;

              case 0x41: /* KIMD-GHASH */
                /* Only valid for KIMD.  */
                if (insn[0] == 0xb93e)
                  {
                    if (record_full_arch_list_add_mem (oaddr, 16))
                      return -1;
                    break;
                  }
                /* For KLMD, fallthru.  */
              default:
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown KMAC function %02x at %s.\n",
                                    (int)tmp, paddress (gdbarch, addr));
                return -1;
            }
          if (tmp != 0)
            {
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
                return -1;
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[7] | 1)))
                return -1;
            }
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb940 undefined */
        /* 0xb944-0xb945 undefined */
        /* 0xb947-0xb948 undefined */
        /* 0xb94c-0xb950 undefined */
        /* 0xb954-0xb958 undefined */
        /* 0xb95c-0xb95f undefined */
        /* 0xb962-0xb971 undefined */
        /* 0xb974-0xb97f undefined */

        case 0xb983: /* FLOGR - find leftmost one */
          /* 64-bit gpr pair destination + flags */
          if (s390_record_gpr_g (gdbarch, regcache, inib[6]))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[6] | 1))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb98a privileged */
        /* 0xb98b-0xb98c undefined */

        case 0xb98d: /* EPSW - extract psw */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (inib[7])
            if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
              return -1;
          break;

        /* 0xb98e-0xb98f privileged */

        case 0xb990: /* TRTT - translate two to two [partial] */
        case 0xb991: /* TRTO - translate two to one [partial] */
        case 0xb992: /* TROT - translate one to two [partial] */
        case 0xb993: /* TROO - translate one to one [partial] */
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[6], &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[6] | 1), &tmp);
          /* tmp is source length, we want destination length.  Adjust.  */
          if (insn[0] == 0xb991)
            tmp >>= 1;
          if (insn[0] == 0xb992)
            tmp <<= 1;
          if (record_full_arch_list_add_mem (oaddr, tmp))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xb996: /* MLR - multiply logical */
        case 0xb997: /* DLR - divide logical */
          /* 32-bit gpr pair destination  */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
            return -1;
          break;

        /* 0xb99a-0xb9af unsupported, privileged, or undefined */
        /* 0xb9b4-0xb9bc undefined */

        case 0xb9bd: /* TRTRE - translate and test reverse extended [partial] */
        case 0xb9bf: /* TRTE - translate and test extended [partial] */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[6] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[7]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb9c0-0xb9c7 undefined */

        case 0xb9c8: /* AHHHR - add high */
        case 0xb9c9: /* SHHHR - subtract high */
        case 0xb9ca: /* ALHHHR - add logical high */
        case 0xb9cb: /* SLHHHR - subtract logical high */
        case 0xb9d8: /* AHHLR - add high */
        case 0xb9d9: /* SHHLR - subtract high */
        case 0xb9da: /* ALHHLR - add logical high */
        case 0xb9db: /* SLHHLR - subtract logical high */
          /* 32-bit high gpr destination + flags */
          if (s390_record_gpr_h (gdbarch, regcache, inib[6]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xb9cc undefined */
        /* 0xb9ce undefined */
        /* 0xb9d0-0xb9d7 undefined */
        /* 0xb9dc undefined */
        /* 0xb9de undefined */

        case 0xb9e0: /* LOCFHR - load high on condition */
          /* 32-bit high gpr destination */
          if (s390_record_gpr_h (gdbarch, regcache, inib[6]))
            return -1;
          break;

        /* 0xb9e3 undefined */
        /* 0xb9e5 undefined */
        /* 0xb9ec-0xb9f1 undefined */
        /* 0xb9f3 undefined */
        /* 0xb9f5 undefined */
        /* 0xb9fc-0xb9ff undefined */

        default:
          goto UNKNOWN_OP;
        }
      break;

    /* 0xb4-0xb5 undefined */
    /* 0xb6 privileged: STCTL - store control */
    /* 0xb7 privileged: LCTL - load control */
    /* 0xb8 undefined */

    case 0xba: /* CS - compare and swap */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 4))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0xbb: /* CDS - compare double and swap */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 8))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    /* 0xbc undefined */

    case 0xbe: /* STCM - store characters under mask */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, s390_popcnt (inib[3])))
        return -1;
      break;

    case 0xc0:
    case 0xc2:
    case 0xc4:
    case 0xc6:
    case 0xcc:
      /* RIL-format instruction */
      switch (ibyte[0] << 4 | inib[3])
        {
        case 0xc00: /* LARL - load address relative long */
        case 0xc05: /* BRASL - branch relative and save long */
        case 0xc09: /* IILF - insert immediate */
        case 0xc21: /* MSFI - multiply single immediate */
        case 0xc42: /* LLHRL - load logical halfword relative long */
        case 0xc45: /* LHRL - load halfword relative long */
        case 0xc4d: /* LRL - load relative long */
          /* 32-bit or native gpr destination */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          break;

        case 0xc01: /* LGFI - load immediate */
        case 0xc0e: /* LLIHF - load logical immediate */
        case 0xc0f: /* LLILF - load logical immediate */
        case 0xc20: /* MSGFI - multiply single immediate */
        case 0xc44: /* LGHRL - load halfword relative long */
        case 0xc46: /* LLGHRL - load logical halfword relative long */
        case 0xc48: /* LGRL - load relative long */
        case 0xc4c: /* LGFRL - load relative long */
        case 0xc4e: /* LLGFRL - load logical relative long */
          /* 64-bit gpr destination */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          break;

        /* 0xc02-0xc03 undefined */

        case 0xc04: /* BRCL - branch relative on condition long */
        case 0xc62: /* PFDRL - prefetch data relative long */
          break;

        case 0xc06: /* XIHF - xor immediate */
        case 0xc0a: /* NIHF - and immediate */
        case 0xc0c: /* OIHF - or immediate */
        case 0xcc8: /* AIH - add immediate high */
        case 0xcca: /* ALSIH - add logical with signed immediate high */
          /* 32-bit high gpr destination + flags */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xc07: /* XILF - xor immediate */
        case 0xc0b: /* NILF - and immediate */
        case 0xc0d: /* OILF - or immediate */
        case 0xc25: /* SLFI - subtract logical immediate */
        case 0xc29: /* AFI - add immediate */
        case 0xc2b: /* ALFI - add logical immediate */
          /* 32-bit gpr destination + flags */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xc08: /* IIHF - insert immediate */
        case 0xcc6: /* BRCTH - branch relative on count high */
        case 0xccb: /* ALSIHN - add logical with signed immediate high */
          /* 32-bit high gpr destination */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          break;

        /* 0xc22-0xc23 undefined */

        case 0xc24: /* SLGFI - subtract logical immediate */
        case 0xc28: /* AGFI - add immediate */
        case 0xc2a: /* ALGFI - add logical immediate */
          /* 64-bit gpr destination + flags */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xc26-0xc27 undefined */

        case 0xc2c: /* CGFI - compare immediate */
        case 0xc2d: /* CFI - compare immediate */
        case 0xc2e: /* CLGFI - compare logical immediate */
        case 0xc2f: /* CLFI - compare logical immediate */
        case 0xc64: /* CGHRL - compare halfword relative long */
        case 0xc65: /* CHRL - compare halfword relative long */
        case 0xc66: /* CLGHRL - compare logical halfword relative long */
        case 0xc67: /* CLHRL - compare logical halfword relative long */
        case 0xc68: /* CGRL - compare relative long */
        case 0xc6a: /* CLGRL - compare logical relative long */
        case 0xc6c: /* CGFRL - compare relative long */
        case 0xc6d: /* CRL - compare relative long */
        case 0xc6e: /* CLGFRL - compare logical relative long */
        case 0xc6f: /* CLRL - compare logical relative long */
        case 0xccd: /* CIH - compare immediate high */
        case 0xccf: /* CLIH - compare logical immediate high */
          /* flags only */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xc40-0xc41 undefined */
        /* 0xc43 undefined */

        case 0xc47: /* STHRL - store halfword relative long */
          oaddr = s390_record_calc_rl (gdbarch, regcache, addr, insn[1], insn[2]);
          if (record_full_arch_list_add_mem (oaddr, 2))
            return -1;
          break;

        /* 0xc49-0xc4a undefined */

        case 0xc4b: /* STGRL - store relative long */
          oaddr = s390_record_calc_rl (gdbarch, regcache, addr, insn[1], insn[2]);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          break;

        case 0xc4f: /* STRL - store relative long */
          oaddr = s390_record_calc_rl (gdbarch, regcache, addr, insn[1], insn[2]);
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          break;

        case 0xc60: /* EXRL - execute relative long */
          if (ex != -1)
            {
              fprintf_unfiltered (gdb_stdlog, "Warning: Double execute at %s.\n",
                                  paddress (gdbarch, addr));
              return -1;
            }
          addr = s390_record_calc_rl (gdbarch, regcache, addr, insn[1], insn[2]);
          if (inib[2])
            {
              regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[2], &tmp);
              ex = tmp & 0xff;
            }
          else
            {
              ex = 0;
            }
          goto ex;

        /* 0xc61 undefined */
        /* 0xc63 undefined */
        /* 0xc69 undefined */
        /* 0xc6b undefined */
        /* 0xcc0-0xcc5 undefined */
        /* 0xcc7 undefined */
        /* 0xcc9 undefined */
        /* 0xccc undefined */
        /* 0xcce undefined */

        default:
          goto UNKNOWN_OP;
        }
      break;

    /* 0xc1 undefined */
    /* 0xc3 undefined */

    case 0xc5: /* BPRP - branch prediction relative preload */
    case 0xc7: /* BPP - branch prediction preload */
      /* no visible effect */
      break;

    case 0xc8:
      /* SSF-format instruction */
      switch (ibyte[0] << 4 | inib[3])
        {
        /* 0xc80 unsupported */

        case 0xc81: /* ECTG - extract cpu time */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, 0))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, 1))
            return -1;
          break;

        case 0xc82: /* CSST - compare and swap and store */
          {
            uint8_t fc, sc;
            regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
            fc = tmp & 0xff;
            sc = tmp >> 8 & 0xff;

            /* First and third operands.  */
            oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
            switch (fc)
              {
                case 0x00: /* 32-bit */
                  if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
                    return -1;
                  if (record_full_arch_list_add_mem (oaddr, 4))
                    return -1;
                  break;

                case 0x01: /* 64-bit */
                  if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
                    return -1;
                  if (record_full_arch_list_add_mem (oaddr, 8))
                    return -1;
                  break;

                case 0x02: /* 128-bit */
                  if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
                    return -1;
                  if (s390_record_gpr_g (gdbarch, regcache, inib[2] | 1))
                    return -1;
                  if (record_full_arch_list_add_mem (oaddr, 16))
                    return -1;
                  break;

                default:
                  fprintf_unfiltered (gdb_stdlog, "Warning: Unknown CSST FC %02x at %s.\n",
                                      fc, paddress (gdbarch, addr));
                  return -1;
              }

            /* Second operand.  */
            oaddr2 = s390_record_calc_disp (gdbarch, regcache, 0, insn[2], 0);
            if (sc > 4)
              {
                fprintf_unfiltered (gdb_stdlog, "Warning: Unknown CSST FC %02x at %s.\n",
                                    sc, paddress (gdbarch, addr));
                return -1;
              }

            if (record_full_arch_list_add_mem (oaddr2, 1 << sc))
              return -1;

            /* Flags.  */
            if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
              return -1;
          }
          break;

        /* 0xc83 undefined */

        case 0xc84: /* LPD - load pair disjoint */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xc85: /* LPDG - load pair disjoint */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[2] | 1))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xc86-0xc8f undefined */

        default:
          goto UNKNOWN_OP;
        }
      break;

    /* 0xc9-0xcb undefined */
    /* 0xcd-0xcf undefined */

    case 0xd0: /* TRTR - translate and test reversed */
    case 0xdd: /* TRT - translate and test */
      if (record_full_arch_list_add_reg (regcache, S390_R1_REGNUM))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R2_REGNUM))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0xd1: /* MVN - move numbers */
    case 0xd2: /* MVC - move */
    case 0xd3: /* MVZ - move zones */
    case 0xdc: /* TR - translate */
    case 0xe8: /* MVCIN - move inverse */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, ibyte[1] + 1))
        return -1;
      break;

    case 0xd4: /* NC - and */
    case 0xd6: /* OC - or*/
    case 0xd7: /* XC - xor */
    case 0xe2: /* UNPKU - unpack unicode */
    case 0xea: /* UNPKA - unpack ASCII */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, ibyte[1] + 1))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0xde: /* ED - edit */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, ibyte[1] + 1))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      /* DXC may be written */
      if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
        return -1;
      break;

    case 0xdf: /* EDMK - edit and mark */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, ibyte[1] + 1))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_R1_REGNUM))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      /* DXC may be written */
      if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
        return -1;
      break;

    /* 0xd8 undefined */
    /* 0xd9 unsupported: MVCK - move with key */
    /* 0xda unsupported: MVCP - move to primary */
    /* 0xdb unsupported: MVCS - move to secondary */
    /* 0xe0 undefined */

    case 0xe1: /* PKU - pack unicode */
    case 0xe9: /* PKA - pack ASCII */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, 16))
        return -1;
      break;

    case 0xe3:
    case 0xe7:
    case 0xeb:
    case 0xed:
      /* RXY/RXE/RXF/RSL/RSY/SIY/V*-format instruction */
      switch (ibyte[0] << 8 | ibyte[5])
        {
        /* 0xe300-0xe301 undefined */

        case 0xe302: /* LTG - load and test */
        case 0xe308: /* AG - add */
        case 0xe309: /* SG - subtract */
        case 0xe30a: /* ALG - add logical */
        case 0xe30b: /* SLG - subtract logical */
        case 0xe318: /* AGF - add */
        case 0xe319: /* SGF - subtract */
        case 0xe31a: /* ALGF - add logical */
        case 0xe31b: /* SLGF - subtract logical */
        case 0xe332: /* LTGF - load and test */
        case 0xe380: /* NG - and */
        case 0xe381: /* OG - or */
        case 0xe382: /* XG - xor */
        case 0xe388: /* ALCG - add logical with carry */
        case 0xe389: /* SLBG - subtract logical with borrow */
        case 0xeb0a: /* SRAG - shift right single */
        case 0xeb0b: /* SLAG - shift left single */
          /* 64-bit gpr destination + flags */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xe303 privileged */

        case 0xe304: /* LG - load */
        case 0xe30c: /* MSG - multiply single */
        case 0xe30f: /* LRVG - load reversed */
        case 0xe314: /* LGF - load */
        case 0xe315: /* LGH - load halfword */
        case 0xe316: /* LLGF - load logical */
        case 0xe317: /* LLGT - load logical thirty one bits */
        case 0xe31c: /* MSGF - multiply single */
        case 0xe32a: /* LZRG - load and zero rightmost byte */
        case 0xe33a: /* LLZRGF - load logical and zero rightmost byte */
        case 0xe346: /* BCTG - branch on count */
        case 0xe377: /* LGB - load byte */
        case 0xe390: /* LLGC - load logical character */
        case 0xe391: /* LLGH - load logical halfword */
        case 0xeb0c: /* SRLG - shift right single logical */
        case 0xeb0d: /* SLLG - shift left single logical */
        case 0xeb1c: /* RLLG - rotate left single logical */
        case 0xeb44: /* BXHG - branch on index high */
        case 0xeb45: /* BXLEG - branch on index low or equal */
        case 0xeb4c: /* ECAG - extract cpu attribute */
        case 0xebe2: /* LOCG - load on condition */
          /* 64-bit gpr destination */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          break;

        /* 0xe305 undefined */

        case 0xe306: /* CVBY - convert to binary */
          /* 32-bit or native gpr destination + FPC (DXC write) */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe307 undefined */

        case 0xe30d: /* DSG - divide single */
        case 0xe31d: /* DSGF - divide single */
        case 0xe386: /* MLG - multiply logical */
        case 0xe387: /* DLG - divide logical */
        case 0xe38f: /* LPQ - load pair from quadword */
          /* 64-bit gpr pair destination  */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[2] | 1))
            return -1;
          break;

        case 0xe30e: /* CVBG - convert to binary */
          /* 64-bit gpr destination + FPC (DXC write) */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe310-0xe311 undefined */

        case 0xe312: /* LT - load and test */
        case 0xe354: /* NY - and */
        case 0xe356: /* OY - or */
        case 0xe357: /* XY - xor */
        case 0xe35a: /* AY - add */
        case 0xe35b: /* SY - subtract */
        case 0xe35e: /* ALY - add logical */
        case 0xe35f: /* SLY - subtract logical */
        case 0xe37a: /* AHY - add halfword */
        case 0xe37b: /* SHY - subtract halfword */
        case 0xe398: /* ALC - add logical with carry */
        case 0xe399: /* SLB - subtract logical with borrow */
        case 0xe727: /* LCBB - load count to block bounduary */
        case 0xeb81: /* ICMY - insert characters under mask */
        case 0xebdc: /* SRAK - shift left single */
        case 0xebdd: /* SLAK - shift left single */
          /* 32-bit gpr destination + flags */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xe313 privileged */

        case 0xe31e: /* LRV - load reversed */
        case 0xe31f: /* LRVH - load reversed */
        case 0xe33b: /* LZRF - load and zero rightmost byte */
        case 0xe351: /* MSY - multiply single */
        case 0xe358: /* LY - load */
        case 0xe371: /* LAY - load address */
        case 0xe373: /* ICY - insert character */
        case 0xe376: /* LB - load byte */
        case 0xe378: /* LHY - load */
        case 0xe37c: /* MHY - multiply halfword */
        case 0xe394: /* LLC - load logical character */
        case 0xe395: /* LLH - load logical halfword */
        case 0xeb1d: /* RLL - rotate left single logical */
        case 0xebde: /* SRLK - shift left single logical */
        case 0xebdf: /* SLLK - shift left single logical */
        case 0xebf2: /* LOC - load on condition */
          /* 32-bit or native gpr destination */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          break;

        case 0xe320: /* CG - compare */
        case 0xe321: /* CLG - compare logical */
        case 0xe330: /* CGF - compare */
        case 0xe331: /* CLGF - compare logical */
        case 0xe334: /* CGH - compare halfword */
        case 0xe355: /* CLY - compare logical */
        case 0xe359: /* CY - compare */
        case 0xe379: /* CHY - compare halfword */
        case 0xe3cd: /* CHF - compare high */
        case 0xe3cf: /* CLHF - compare logical high */
        case 0xeb20: /* CLMH - compare logical under mask high */
        case 0xeb21: /* CLMY - compare logical under mask */
        case 0xeb51: /* TMY - test under mask */
        case 0xeb55: /* CLIY - compare logical */
        case 0xebc0: /* TP - test decimal */
        case 0xed10: /* TCEB - test data class */
        case 0xed11: /* TCDB - test data class */
        case 0xed12: /* TCXB - test data class */
        case 0xed50: /* TDCET - test data class */
        case 0xed51: /* TDGET - test data group */
        case 0xed54: /* TDCDT - test data class */
        case 0xed55: /* TDGDT - test data group */
        case 0xed58: /* TDCXT - test data class */
        case 0xed59: /* TDGXT - test data group */
          /* flags only */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xe322-0xe323 undefined */

        case 0xe324: /* STG - store */
        case 0xe325: /* NTSTG - nontransactional store */
        case 0xe326: /* CVDY - convert to decimal */
        case 0xe32f: /* STRVG - store reversed */
        case 0xebe3: /* STOCG - store on condition */
        case 0xed67: /* STDY - store */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          break;

        /* 0xe327-0xe329 undefined */
        /* 0xe32b-0xe32d undefined */

        case 0xe32e: /* CVDG - convert to decimal */
        case 0xe38e: /* STPQ - store pair to quadword */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 16))
            return -1;
          break;

        /* 0xe333 undefined */
        /* 0xe335 undefined */

        case 0xe336: /* PFD - prefetch data */
          break;

        /* 0xe337-0xe339 undefined */
        /* 0xe33c-0xe33d undefined */

        case 0xe33e: /* STRV - store reversed */
        case 0xe350: /* STY - store */
        case 0xe3cb: /* STFH - store high */
        case 0xebe1: /* STOCFH - store high on condition */
        case 0xebf3: /* STOC - store on condition */
        case 0xed66: /* STEY - store */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          break;

        case 0xe33f: /* STRVH - store reversed */
        case 0xe370: /* STHY - store halfword */
        case 0xe3c7: /* STHH - store halfword high */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 2))
            return -1;
          break;

        /* 0xe340-0xe345 undefined */
        /* 0xe347-0xe34f undefined */
        /* 0xe352-0xe353 undefined */

        case 0xe35c: /* MFY - multiply */
        case 0xe396: /* ML - multiply logical */
        case 0xe397: /* DL - divide logical */
          /* 32-bit gpr pair destination */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
            return -1;
          break;

        /* 0xe35d undefined */
        /* 0xe360-0xe36f undefined */

        case 0xe372: /* STCY - store character */
        case 0xe3c3: /* STCH - store character high */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 1))
            return -1;
          break;

        /* 0xe374 undefined */

        case 0xe375: /* LAEY - load address extended */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + inib[2]))
            return -1;
          break;

        /* 0xe37d-0xe37f undefined */
        /* 0xe383-0xe384 undefined */

        case 0xe385: /* LGAT - load and trap */
        case 0xe39c: /* LLGTAT - load logical thirty one bits and trap */
        case 0xe39d: /* LLGFAT - load logical and trap */
        case 0xe721: /* VLGV - vector load gr from vr element */
          /* 64-bit gpr destination + fpc for possible DXC write */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe38a-0xe38d undefined */
        /* 0xe392-0xe393 undefined */
        /* 0xe39a-0xe39b undefined */
        /* 0xe39e undefined */

        case 0xe39f: /* LAT - load and trap */
          /* 32-bit gpr destination + fpc for possible DXC write */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe3a0-0xe3bf undefined */

        case 0xe3c0: /* LBH - load byte high */
        case 0xe3c2: /* LLCH - load logical character high */
        case 0xe3c4: /* LHH - load halfword high */
        case 0xe3c6: /* LLHH - load logical halfword high */
        case 0xe3ca: /* LFH - load high */
        case 0xebe0: /* LOCFH - load high on condition */
          /* 32-bit high gpr destination */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          break;

        /* 0xe3c1 undefined */
        /* 0xe3c5 undefined */

        case 0xe3c8: /* LFHAT - load high and trap */
          /* 32-bit high gpr destination + fpc for possible DXC write */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe3c9 undefined */
        /* 0xe3cc undefined */
        /* 0xe3ce undefined */
        /* 0xe3d0-0xe3ff undefined */

        case 0xe700: /* VLEB - vector load element */
        case 0xe701: /* VLEH - vector load element */
        case 0xe702: /* VLEG - vector load element */
        case 0xe703: /* VLEF - vector load element */
        case 0xe704: /* VLLEZ - vector load logical element and zero */
        case 0xe705: /* VLREP - vector load and replicate */
        case 0xe706: /* VL - vector load */
        case 0xe707: /* VLBB - vector load to block bounduary */
        case 0xe712: /* VGEG - vector gather element */
        case 0xe713: /* VGEF - vector gather element */
        case 0xe722: /* VLVG - vector load vr element from gr */
        case 0xe730: /* VESL - vector element shift left */
        case 0xe733: /* VERLL - vector element rotate left logical */
        case 0xe737: /* VLL - vector load with length */
        case 0xe738: /* VESRL - vector element shift right logical */
        case 0xe73a: /* VESRA - vector element shift right arithmetic */
        case 0xe740: /* VLEIB - vector load element immediate */
        case 0xe741: /* VLEIH - vector load element immediate */
        case 0xe742: /* VLEIG - vector load element immediate */
        case 0xe743: /* VLEIF - vector load element immediate */
        case 0xe744: /* VGBM - vector generate byte mask */
        case 0xe745: /* VREPI - vector replicate immediate */
        case 0xe746: /* VGM - vector generate mask */
        case 0xe74d: /* VREP - vector replicate */
        case 0xe750: /* VPOPCT - vector population count */
        case 0xe752: /* VCTZ - vector count trailing zeros */
        case 0xe753: /* VCLZ - vector count leading zeros */
        case 0xe756: /* VLR - vector load */
        case 0xe75f: /* VSEG -vector sign extend to doubleword */
        case 0xe760: /* VMRL - vector merge low */
        case 0xe761: /* VMRH - vector merge high */
        case 0xe762: /* VLVGP - vector load vr from grs disjoint */
        case 0xe764: /* VSUM - vector sum across word */
        case 0xe765: /* VSUMG - vector sum across doubleword */
        case 0xe766: /* VCKSM - vector checksum */
        case 0xe767: /* VSUMQ - vector sum across quadword */
        case 0xe768: /* VN - vector and */
        case 0xe769: /* VNC - vector and with complement */
        case 0xe76a: /* VO - vector or */
        case 0xe76b: /* VNO - vector nor */
        case 0xe76d: /* VX - vector xor */
        case 0xe770: /* VESLV - vector element shift left */
        case 0xe772: /* VERIM - vector element rotate and insert under mask */
        case 0xe773: /* VERLLV - vector element rotate left logical */
        case 0xe774: /* VSL - vector shift left */
        case 0xe775: /* VSLB - vector shift left by byte */
        case 0xe777: /* VSLDB - vector shift left double by byte */
        case 0xe778: /* VESRLV - vector element shift right logical */
        case 0xe77a: /* VESRAV - vector element shift right arithmetic */
        case 0xe77c: /* VSRL - vector shift right logical */
        case 0xe77d: /* VSRLB - vector shift right logical by byte */
        case 0xe77e: /* VSRA - vector shift right arithmetic */
        case 0xe77f: /* VSRAB - vector shift right arithmetic by byte */
        case 0xe784: /* VPDI - vector permute doubleword immediate */
        case 0xe78c: /* VPERM - vector permute */
        case 0xe78d: /* VSEL - vector select */
        case 0xe78e: /* VFMS - vector fp multiply and subtract */
        case 0xe78f: /* VFMA - vector fp multiply and add */
        case 0xe794: /* VPK - vector pack */
        case 0xe7a1: /* VMLH - vector multiply logical high */
        case 0xe7a2: /* VML - vector multiply low */
        case 0xe7a3: /* VMH - vector multiply high */
        case 0xe7a4: /* VMLE - vector multiply logical even */
        case 0xe7a5: /* VMLO - vector multiply logical odd */
        case 0xe7a6: /* VME - vector multiply even */
        case 0xe7a7: /* VMO - vector multiply odd */
        case 0xe7a9: /* VMALH - vector multiply and add logical high */
        case 0xe7aa: /* VMAL - vector multiply and add low */
        case 0xe7ab: /* VMAH - vector multiply and add high */
        case 0xe7ac: /* VMALE - vector multiply and add logical even */
        case 0xe7ad: /* VMALO - vector multiply and add logical odd */
        case 0xe7ae: /* VMAE - vector multiply and add even */
        case 0xe7af: /* VMAO - vector multiply and add odd */
        case 0xe7b4: /* VGFM - vector Galois field multiply sum */
        case 0xe7b9: /* VACCC - vector add with carry compute carry */
        case 0xe7bb: /* VAC - vector add with carry */
        case 0xe7bc: /* VGFMA - vector Galois field multiply sum and accumulate */
        case 0xe7bd: /* VSBCBI - vector subtract with borrow compute borrow indication */
        case 0xe7bf: /* VSBI - vector subtract with borrow indication */
        case 0xe7c0: /* VCLGD - vector convert to logical 64-bit */
        case 0xe7c1: /* VCDLG - vector convert from logical 64-bit */
        case 0xe7c2: /* VCGD - vector convert to fixed 64-bit */
        case 0xe7c3: /* VCDG - vector convert from fixed 64-bit */
        case 0xe7c4: /* VLDE - vector fp load lengthened */
        case 0xe7c5: /* VLED - vector fp load rounded */
        case 0xe7c7: /* VFI - vector load fp integer */
        case 0xe7cc: /* VFPSO - vector fp perform sign operation */
        case 0xe7ce: /* VFSQ - vector fp square root */
        case 0xe7d4: /* VUPLL - vector unpack logical low */
        case 0xe7d6: /* VUPL - vector unpack low */
        case 0xe7d5: /* VUPLH - vector unpack logical high */
        case 0xe7d7: /* VUPH - vector unpack high */
        case 0xe7de: /* VLC - vector load complement */
        case 0xe7df: /* VLP - vector load positive */
        case 0xe7e2: /* VFA - vector fp subtract */
        case 0xe7e3: /* VFA - vector fp add */
        case 0xe7e5: /* VFD - vector fp divide */
        case 0xe7e7: /* VFM - vector fp multiply */
        case 0xe7f0: /* VAVGL - vector average logical */
        case 0xe7f1: /* VACC - vector add and compute carry */
        case 0xe7f2: /* VAVG - vector average */
        case 0xe7f3: /* VA - vector add */
        case 0xe7f5: /* VSCBI - vector subtract compute borrow indication */
        case 0xe7f7: /* VS - vector subtract */
        case 0xe7fc: /* VMNL - vector minimum logical */
        case 0xe7fd: /* VMXL - vector maximum logical */
        case 0xe7fe: /* VMN - vector minimum */
        case 0xe7ff: /* VMX - vector maximum */
          /* vector destination + FPC */
          if (s390_record_vr (gdbarch, regcache, ivec[0]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xe708: /* VSTEB - vector store element */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 1))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xe709: /* VSTEH - vector store element */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 2))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xe70a: /* VSTEG - vector store element */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xe70b: /* VSTEF - vector store element */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe70c-0xe70d undefined */

        case 0xe70e: /* VST - vector store */
          oaddr = s390_record_calc_disp (gdbarch, regcache, inib[3], insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 16))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe70f-0xe711 undefined */
        /* 0xe714-0xe719 undefined */

        case 0xe71a: /* VSCEG - vector scatter element */
          if (s390_record_calc_disp_vsce (gdbarch, regcache, ivec[1], inib[8], 8, insn[1], 0, &oaddr))
            return -1;
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xe71b: /* VSCEF - vector scatter element */
          if (s390_record_calc_disp_vsce (gdbarch, regcache, ivec[1], inib[8], 4, insn[1], 0, &oaddr))
            return -1;
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe71c-0xe720 undefined */
        /* 0xe723-0xe726 undefined */
        /* 0xe728-0xe72f undefined */
        /* 0xe731-0xe732 undefined */
        /* 0xe734-0xe735 undefined */

        case 0xe736: /* VLM - vector load multiple */
          for (i = ivec[0]; i != ivec[1]; i++, i &= 0x1f)
            if (s390_record_vr (gdbarch, regcache, i))
              return -1;
          if (s390_record_vr (gdbarch, regcache, ivec[1]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe739 undefined */
        /* 0xe73b-0xe73d undefined */

        case 0xe73e: /* VSTM - vector store multiple */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (ivec[0] <= ivec[1])
            n = ivec[1] - ivec[0] + 1;
          else
            n = ivec[1] + 0x20 - ivec[0] + 1;
          if (record_full_arch_list_add_mem (oaddr, n * 16))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xe73f: /* VSTL - vector store with length */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[3], &tmp);
          tmp &= 0xffffffffu;
          if (tmp > 16)
            tmp = 16;
          if (record_full_arch_list_add_mem (oaddr, tmp))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe747-0xe749 undefined */

        case 0xe74a: /* VFTCI - vector fp test data class immediate */
        case 0xe75c: /* VISTR - vector isolate string */
        case 0xe780: /* VFEE - vector find element equal */
        case 0xe781: /* VFENE - vector find element not equal */
        case 0xe782: /* VFA - vector find any element equal */
        case 0xe78a: /* VSTRC - vector string range compare */
        case 0xe795: /* VPKLS - vector pack logical saturate */
        case 0xe797: /* VPKS - vector pack saturate */
        case 0xe7e8: /* VFCE - vector fp compare equal */
        case 0xe7ea: /* VFCHE - vector fp compare high or equal */
        case 0xe7eb: /* VFCE - vector fp compare high */
        case 0xe7f8: /* VCEQ - vector compare equal */
        case 0xe7f9: /* VCHL - vector compare high logical */
        case 0xe7fb: /* VCH - vector compare high */
          /* vector destination + flags + FPC */
          if (s390_record_vr (gdbarch, regcache, ivec[0]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe74b-0xe74c undefined */
        /* 0xe74e-0xe74f undefined */
        /* 0xe751 undefined */
        /* 0xe754-0xe755 undefined */
        /* 0xe757-0xe75b undefined */
        /* 0xe75d-0xe75e undefined */
        /* 0xe763 undefined */
        /* 0xe76c undefined */
        /* 0xe76e-0xe76f undefined */
        /* 0xe771 undefined */
        /* 0xe776 undefined */
        /* 0xe779 undefined */
        /* 0xe77b undefined */
        /* 0xe783 undefined */
        /* 0xe785-0xe789 undefined */
        /* 0xe78b undefined */
        /* 0xe790-0xe793 undefined */
        /* 0xe796 undefined */
        /* 0xe798-0xe7a0 undefined */
        /* 0xe7a8 undefined */
        /* 0xe7b0-0xe7b3 undefined */
        /* 0xe7b5-0xe7b8 undefined */
        /* 0xe7ba undefined */
        /* 0xe7be undefined */
        /* 0xe7c6 undefined */
        /* 0xe7c8-0xe7c9 undefined */

        case 0xe7ca: /* WFK - vector fp compare and signal scalar */
        case 0xe7cb: /* WFC - vector fp compare scalar */
        case 0xe7d8: /* VTM - vector test under mask */
        case 0xe7d9: /* VECL - vector element compare logical */
        case 0xe7db: /* VEC - vector element compare */
        case 0xed08: /* KEB - compare and signal */
        case 0xed09: /* CEB - compare */
        case 0xed18: /* KDB - compare and signal */
        case 0xed19: /* CDB - compare */
          /* flags + fpc only */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xe7cd undefined */
        /* 0xe7cf-0xe7d3 undefined */
        /* 0xe7da undefined */
        /* 0xe7dc-0xe7dd undefined */
        /* 0xe7e0-0xe7e1 undefined */
        /* 0xe7e4 undefined */
        /* 0xe7e6 undefined */
        /* 0xe7e9 undefined */
        /* 0xe7ec-0xe7ef undefined */
        /* 0xe7f4 undefined */
        /* 0xe7f6 undefined */
        /* 0xe7fa undefined */

        /* 0xeb00-0xeb03 undefined */

        case 0xeb04: /* LMG - load multiple */
          for (i = inib[2]; i != inib[3]; i++, i &= 0xf)
            if (s390_record_gpr_g (gdbarch, regcache, i))
              return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[3]))
            return -1;
          break;

        /* 0xeb05-0xeb09 undefined */
        /* 0xeb0e undefined */
        /* 0xeb0f privileged: TRACG */
        /* 0xeb10-0xeb13 undefined */

        case 0xeb14: /* CSY - compare and swap */
        case 0xebf4: /* LAN - load and and */
        case 0xebf6: /* LAO - load and or */
        case 0xebf7: /* LAX - load and xor */
        case 0xebf8: /* LAA - load and add */
        case 0xebfa: /* LAAL - load and add logical */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb15-0xeb1b undefined */
        /* 0xeb1e-0xeb1f undefined */
        /* 0xeb22 undefined */

        case 0xeb23: /* CLT - compare logical and trap */
        case 0xeb2b: /* CLGT - compare logical and trap */
          /* fpc only - including possible DXC write for trapping insns */
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xeb24: /* STMG - store multiple */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (inib[2] <= inib[3])
            n = inib[3] - inib[2] + 1;
          else
            n = inib[3] + 0x10 - inib[2] + 1;
          if (record_full_arch_list_add_mem (oaddr, n * 8))
            return -1;
          break;

        /* 0xeb25 privileged */

        case 0xeb26: /* STMH - store multiple high */
        case 0xeb90: /* STMY - store multiple */
        case 0xeb9b: /* STAMY - store access multiple */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (inib[2] <= inib[3])
            n = inib[3] - inib[2] + 1;
          else
            n = inib[3] + 0x10 - inib[2] + 1;
          if (record_full_arch_list_add_mem (oaddr, n * 4))
            return -1;
          break;

        /* 0xeb27-0xeb2a undefined */

        case 0xeb2c: /* STCMH - store characters under mask */
        case 0xeb2d: /* STCMY - store characters under mask */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, s390_popcnt (inib[3])))
            return -1;
          break;

        /* 0xeb2e undefined */
        /* 0xeb2f privileged */

        case 0xeb30: /* CSG - compare and swap */
        case 0xebe4: /* LANG - load and and */
        case 0xebe6: /* LAOG - load and or */
        case 0xebe7: /* LAXG - load and xor */
        case 0xebe8: /* LAAG - load and add */
        case 0xebea: /* LAALG - load and add logical */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xeb31: /* CDSY - compare double and swap */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb32-0xeb3d undefined */

        case 0xeb3e: /* CDSG - compare double and swap */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 16))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (s390_record_gpr_g (gdbarch, regcache, inib[2] | 1))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb3f-0xeb43 undefined */
        /* 0xeb46-0xeb4b undefined */
        /* 0xeb4d-0xeb50 undefined */

        case 0xeb52: /* MVIY - move */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 1))
            return -1;
          break;

        case 0xeb54: /* NIY - and */
        case 0xeb56: /* OIY - or */
        case 0xeb57: /* XIY - xor */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 1))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb53 undefined */
        /* 0xeb58-0xeb69 undefined */

        case 0xeb6a: /* ASI - add immediate */
        case 0xeb6e: /* ALSI - add immediate */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb6b-0xeb6d undefined */
        /* 0xeb6f-0xeb79 undefined */

        case 0xeb7a: /* AGSI - add immediate */
        case 0xeb7e: /* ALGSI - add immediate */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], ibyte[4]);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb7b-0xeb7d undefined */
        /* 0xeb7f undefined */

        case 0xeb80: /* ICMH - insert characters under mask */
          /* 32-bit high gpr destination + flags */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb82-0xeb8d undefined */

        case 0xeb8e: /* MVCLU - move long unicode [partial] */
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + inib[2], &tmp);
          oaddr = s390_record_address_mask (gdbarch, regcache, tmp);
          regcache_raw_read_unsigned (regcache, S390_R0_REGNUM + (inib[2] | 1), &tmp);
          if (record_full_arch_list_add_mem (oaddr, tmp))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[3] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        case 0xeb8f: /* CLCLU - compare logical long unicode [partial] */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[2] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + (inib[3] | 1)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xeb91-0xeb95 undefined */

        case 0xeb96: /* LMH - load multiple high */
          for (i = inib[2]; i != inib[3]; i++, i &= 0xf)
            if (s390_record_gpr_h (gdbarch, regcache, i))
              return -1;
          if (s390_record_gpr_h (gdbarch, regcache, inib[3]))
            return -1;
          break;

        /* 0xeb97 undefined */

        case 0xeb98: /* LMY - load multiple */
          for (i = inib[2]; i != inib[3]; i++, i &= 0xf)
            if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + i))
              return -1;
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
            return -1;
          break;

        /* 0xeb99 undefined */

        case 0xeb9a: /* LAMY - load access multiple */
          for (i = inib[2]; i != inib[3]; i++, i &= 0xf)
            if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + i))
              return -1;
          if (record_full_arch_list_add_reg (regcache, S390_A0_REGNUM + inib[3]))
            return -1;
          break;

        /* 0xeb9c-0xebbf undefined */
        /* 0xebc1-0xebdb undefined */
        /* 0xebe5 undefined */
        /* 0xebe9 undefined */
        /* 0xebeb-0xebf1 undefined */
        /* 0xebf5 undefined */
        /* 0xebf9 undefined */
        /* 0xebfb-0xebff undefined */

        /* 0xed00-0xed03 undefined */

        case 0xed04: /* LDEB - load lengthened */
        case 0xed0c: /* MDEB - multiply */
        case 0xed0d: /* DEB - divide */
        case 0xed14: /* SQEB - square root */
        case 0xed15: /* SQDB - square root */
        case 0xed17: /* MEEB - multiply */
        case 0xed1c: /* MDB - multiply */
        case 0xed1d: /* DDB - divide */
          /* float destination + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xed05: /* LXDB - load lengthened */
        case 0xed06: /* LXEB - load lengthened */
        case 0xed07: /* MXDB - multiply */
          /* float pair destination + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[2] | 2)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xed0a: /* AEB - add */
        case 0xed0b: /* SEB - subtract */
        case 0xed1a: /* ADB - add */
        case 0xed1b: /* SDB - subtract */
          /* float destination + flags + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        case 0xed0e: /* MAEB - multiply and add */
        case 0xed0f: /* MSEB - multiply and subtract */
        case 0xed1e: /* MADB - multiply and add */
        case 0xed1f: /* MSDB - multiply and subtract */
        case 0xed40: /* SLDT - shift significand left */
        case 0xed41: /* SRDT - shift significand right */
        case 0xedaa: /* CDZT - convert from zoned */
        case 0xedae: /* CDPT - convert from packed */
          /* float destination [RXF] + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[8]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xed13 undefined */
        /* 0xed16 undefined */
        /* 0xed20-0xed23 undefined */

        case 0xed24: /* LDE - load lengthened */
        case 0xed34: /* SQE - square root */
        case 0xed35: /* SQD - square root */
        case 0xed37: /* MEE - multiply */
        case 0xed64: /* LEY - load */
        case 0xed65: /* LDY - load */
          /* float destination */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
            return -1;
          break;

        case 0xed25: /* LXD - load lengthened */
        case 0xed26: /* LXE - load lengthened */
          /* float pair destination */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[2] | 2)))
            return -1;
          break;

        /* 0xed27-0xed2d undefined */

        case 0xed2e: /* MAE - multiply and add */
        case 0xed2f: /* MSE - multiply and subtract */
        case 0xed38: /* MAYL - multiply and add unnormalized */
        case 0xed39: /* MYL - multiply unnormalized */
        case 0xed3c: /* MAYH - multiply and add unnormalized */
        case 0xed3d: /* MYH - multiply unnormalized */
        case 0xed3e: /* MAD - multiply and add */
        case 0xed3f: /* MSD - multiply and subtract */
          /* float destination [RXF] */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[8]))
            return -1;
          break;

        /* 0xed30-0xed33 undefined */
        /* 0xed36 undefined */

        case 0xed3a: /* MAY - multiply and add unnormalized */
        case 0xed3b: /* MY - multiply unnormalized */
          /* float pair destination [RXF] */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[8]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[8] | 2)))
            return -1;
          break;

        /* 0xed42-0xed47 undefind */

        case 0xed48: /* SLXT - shift significand left */
        case 0xed49: /* SRXT - shift significand right */
        case 0xedab: /* CXZT - convert from zoned */
        case 0xedaf: /* CXPT - convert from packed */
          /* float pair destination [RXF] + fpc */
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + inib[8]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_F0_REGNUM + (inib[8] | 2)))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xed4a-0xed4f undefind */
        /* 0xed52-0xed53 undefind */
        /* 0xed56-0xed57 undefind */
        /* 0xed5a-0xed63 undefind */
        /* 0xed68-0xeda7 undefined */

        case 0xeda8: /* CZDT - convert to zoned */
        case 0xeda9: /* CZXT - convert to zoned */
        case 0xedac: /* CPDT - convert to packed */
        case 0xedad: /* CPXT - convert to packed */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, ibyte[1] + 1))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xedb0-0xedff undefined */

        default:
          goto UNKNOWN_OP;
        }
      break;

    /* 0xe4 undefined */

    case 0xe5:
      /* SSE/SIL-format instruction */
      switch (insn[0])
        {
        /* 0xe500-0xe543 undefined, privileged, or unsupported */

        case 0xe544: /* MVHHI - move */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 2))
            return -1;
          break;

        /* 0xe545-0xe547 undefined */

        case 0xe548: /* MVGHI - move */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 8))
            return -1;
          break;

        /* 0xe549-0xe54b undefined */

        case 0xe54c: /* MVHI - move */
          oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
          if (record_full_arch_list_add_mem (oaddr, 4))
            return -1;
          break;

        /* 0xe54d-0xe553 undefined */

        case 0xe554: /* CHHSI - compare halfword immediate */
        case 0xe555: /* CLHHSI - compare logical immediate */
        case 0xe558: /* CGHSI - compare halfword immediate */
        case 0xe559: /* CLGHSI - compare logical immediate */
        case 0xe55c: /* CHSI - compare halfword immediate */
        case 0xe55d: /* CLFHSI - compare logical immediate */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xe556-0xe557 undefined */
        /* 0xe55a-0xe55b undefined */
        /* 0xe55e-0xe55f undefined */

        case 0xe560: /* TBEGIN - transaction begin */
          /* The transaction will be immediately aborted after this
             instruction, due to single-stepping.  This instruction is
             only supported so that the program can fail a few times
             and go to the non-transactional fallback.  */
          if (inib[4])
            {
              /* Transaction diagnostic block - user.  */
              oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
              if (record_full_arch_list_add_mem (oaddr, 256))
                return -1;
            }
          /* Transaction diagnostic block - supervisor.  */
          if (record_full_arch_list_add_reg (regcache, S390_TDB_DWORD0_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_TDB_ABORT_CODE_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_TDB_CONFLICT_TOKEN_REGNUM))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_TDB_ATIA_REGNUM))
            return -1;
          for (i = 0; i < 16; i++)
            if (record_full_arch_list_add_reg (regcache, S390_TDB_R0_REGNUM + i))
              return -1;
          /* And flags.  */
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xe561 unsupported: TBEGINC */
        /* 0xe562-0xe5ff undefined */

        default:
          goto UNKNOWN_OP;
        }
      break;

    /* 0xe6 undefined */

    case 0xec:
      /* RIE/RIS/RRS-format instruction */
      switch (ibyte[0] << 8 | ibyte[5])
        {
        /* 0xec00-0xec41 undefined */

        case 0xec42: /* LOCHI - load halfword immediate on condition */
        case 0xec51: /* RISBLG - rotate then insert selected bits low */
          /* 32-bit or native gpr destination */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          break;

        /* 0xec43 undefined */

        case 0xec44: /* BRXHG - branch relative on index high */
        case 0xec45: /* BRXLG - branch relative on index low or equal */
        case 0xec46: /* LOCGHI - load halfword immediate on condition */
        case 0xec59: /* RISBGN - rotate then insert selected bits */
          /* 64-bit gpr destination */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          break;

        /* 0xec47-0xec4d undefined */

        case 0xec4e: /* LOCHHI - load halfword immediate on condition */
        case 0xec5d: /* RISBHG - rotate then insert selected bits high */
          /* 32-bit high gpr destination */
          if (s390_record_gpr_h (gdbarch, regcache, inib[2]))
            return -1;
          break;

        /* 0xec4f-0xec50 undefined */
        /* 0xec52-0xec53 undefined */

        case 0xec54: /* RNSBG - rotate then and selected bits */
        case 0xec55: /* RISBG - rotate then insert selected bits */
        case 0xec56: /* ROSBG - rotate then or selected bits */
        case 0xec57: /* RXSBG - rotate then xor selected bits */
        case 0xecd9: /* AGHIK - add immediate */
        case 0xecdb: /* ALGHSIK - add logical immediate */
          /* 64-bit gpr destination + flags */
          if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xec58 undefined */
        /* 0xec5a-0xec5c undefined */
        /* 0xec5e-0xec63 undefined */

        case 0xec64: /* CGRJ - compare and branch relative */
        case 0xec65: /* CLGRJ - compare logical and branch relative */
        case 0xec76: /* CRJ - compare and branch relative */
        case 0xec77: /* CLRJ - compare logical and branch relative */
        case 0xec7c: /* CGIJ - compare immediate and branch relative */
        case 0xec7d: /* CLGIJ - compare logical immediate and branch relative */
        case 0xec7e: /* CIJ - compare immediate and branch relative */
        case 0xec7f: /* CLIJ - compare logical immediate and branch relative */
        case 0xece4: /* CGRB - compare and branch */
        case 0xece5: /* CLGRB - compare logical and branch */
        case 0xecf6: /* CRB - compare and branch */
        case 0xecf7: /* CLRB - compare logical and branch */
        case 0xecfc: /* CGIB - compare immediate and branch */
        case 0xecfd: /* CLGIB - compare logical immediate and branch */
        case 0xecfe: /* CIB - compare immediate and branch */
        case 0xecff: /* CLIB - compare logical immediate and branch */
          break;

        /* 0xec66-0xec6f undefined */

        case 0xec70: /* CGIT - compare immediate and trap */
        case 0xec71: /* CLGIT - compare logical immediate and trap */
        case 0xec72: /* CIT - compare immediate and trap */
        case 0xec73: /* CLFIT - compare logical immediate and trap */
          /* fpc only - including possible DXC write for trapping insns */
          if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
            return -1;
          break;

        /* 0xec74-0xec75 undefined */
        /* 0xec78-0xec7b undefined */

        /* 0xec80-0xecd7 undefined */

        case 0xecd8: /* AHIK - add immediate */
        case 0xecda: /* ALHSIK - add logical immediate */
          /* 32-bit gpr destination + flags */
          if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
            return -1;
          if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
            return -1;
          break;

        /* 0xecdc-0xece3 undefined */
        /* 0xece6-0xecf5 undefined */
        /* 0xecf8-0xecfb undefined */

        default:
          goto UNKNOWN_OP;
        }
      break;

    case 0xee: /* PLO - perform locked operation */
      regcache_raw_read_unsigned (regcache, S390_R0_REGNUM, &tmp);
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      oaddr2 = s390_record_calc_disp (gdbarch, regcache, 0, insn[2], 0);
      if (!(tmp & 0x100))
        {
          uint8_t fc = tmp & 0xff;
          gdb_byte buf[8];
          switch (fc)
            {
            case 0x00: /* CL */
              /* op1c */
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
                return -1;
              /* op3 */
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
                return -1;
              break;

            case 0x01: /* CLG */
              /* op1c */
              if (record_full_arch_list_add_mem (oaddr2 + 0x08, 8))
                return -1;
              /* op3 */
              if (record_full_arch_list_add_mem (oaddr2 + 0x28, 8))
                return -1;
              break;

            case 0x02: /* CLGR */
              /* op1c */
              if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
                return -1;
              /* op3 */
              if (s390_record_gpr_g (gdbarch, regcache, inib[3]))
                return -1;
              break;

            case 0x03: /* CLX */
              /* op1c */
              if (record_full_arch_list_add_mem (oaddr2 + 0x00, 16))
                return -1;
              /* op3 */
              if (record_full_arch_list_add_mem (oaddr2 + 0x20, 16))
                return -1;
              break;

            case 0x08: /* DCS */
              /* op3c */
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[3]))
                return -1;
              /* fallthru */
            case 0x0c: /* CSST */
              /* op4 */
              if (record_full_arch_list_add_mem (oaddr2, 4))
                return -1;
              goto CS;

            case 0x14: /* CSTST */
              /* op8 */
              if (target_read_memory (oaddr2 + 0x88, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 4))
                return -1;
              /* fallthru */
            case 0x10: /* CSDST */
              /* op6 */
              if (target_read_memory (oaddr2 + 0x68, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 4))
                return -1;
              /* op4 */
              if (target_read_memory (oaddr2 + 0x48, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 4))
                return -1;
              /* fallthru */
            case 0x04: /* CS */
CS:
              /* op1c */
              if (record_full_arch_list_add_reg (regcache, S390_R0_REGNUM + inib[2]))
                return -1;
              /* op2 */
              if (record_full_arch_list_add_mem (oaddr, 4))
                return -1;
              break;

            case 0x09: /* DCSG */
              /* op3c */
              if (record_full_arch_list_add_mem (oaddr2 + 0x28, 8))
                return -1;
              goto CSSTG;

            case 0x15: /* CSTSTG */
              /* op8 */
              if (target_read_memory (oaddr2 + 0x88, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 8))
                return -1;
              /* fallthru */
            case 0x11: /* CSDSTG */
              /* op6 */
              if (target_read_memory (oaddr2 + 0x68, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 8))
                return -1;
              /* fallthru */
            case 0x0d: /* CSSTG */
CSSTG:
              /* op4 */
              if (target_read_memory (oaddr2 + 0x48, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 8))
                return -1;
              /* fallthru */
            case 0x05: /* CSG */
              /* op1c */
              if (record_full_arch_list_add_mem (oaddr2 + 0x08, 8))
                return -1;
              /* op2 */
              if (record_full_arch_list_add_mem (oaddr, 8))
                return -1;
              break;

            case 0x0a: /* DCSGR */
              /* op3c */
              if (s390_record_gpr_g (gdbarch, regcache, inib[3]))
                return -1;
              /* fallthru */
            case 0x0e: /* CSSTGR */
              /* op4 */
              if (record_full_arch_list_add_mem (oaddr2, 8))
                return -1;
              goto CSGR;

            case 0x16: /* CSTSTGR */
              /* op8 */
              if (target_read_memory (oaddr2 + 0x88, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 8))
                return -1;
              /* fallthru */
            case 0x12: /* CSDSTGR */
              /* op6 */
              if (target_read_memory (oaddr2 + 0x68, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 8))
                return -1;
              /* op4 */
              if (target_read_memory (oaddr2 + 0x48, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 8))
                return -1;
              /* fallthru */
            case 0x06: /* CSGR */
CSGR:
              /* op1c */
              if (s390_record_gpr_g (gdbarch, regcache, inib[2]))
                return -1;
              /* op2 */
              if (record_full_arch_list_add_mem (oaddr, 8))
                return -1;
              break;

            case 0x0b: /* DCSX */
              /* op3c */
              if (record_full_arch_list_add_mem (oaddr2 + 0x20, 16))
                return -1;
              goto CSSTX;

            case 0x17: /* CSTSTX */
              /* op8 */
              if (target_read_memory (oaddr2 + 0x88, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 16))
                return -1;
              /* fallthru */
            case 0x13: /* CSDSTX */
              /* op6 */
              if (target_read_memory (oaddr2 + 0x68, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 16))
                return -1;
              /* fallthru */
            case 0x0f: /* CSSTX */
CSSTX:
              /* op4 */
              if (target_read_memory (oaddr2 + 0x48, buf, 8))
                return -1;
              oaddr3 = extract_unsigned_integer (buf, 8, byte_order);
              oaddr3 = s390_record_address_mask (gdbarch, regcache, oaddr3);
              if (record_full_arch_list_add_mem (oaddr3, 16))
                return -1;
              /* fallthru */
            case 0x07: /* CSX */
              /* op1c */
              if (record_full_arch_list_add_mem (oaddr2 + 0x00, 16))
                return -1;
              /* op2 */
              if (record_full_arch_list_add_mem (oaddr, 16))
                return -1;
              break;

            default:
              fprintf_unfiltered (gdb_stdlog, "Warning: Unknown PLO FC %02x at %s.\n",
                                  fc, paddress (gdbarch, addr));
              return -1;
            }
        }
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      break;

    case 0xef: /* LMD - load multiple disjoint */
      for (i = inib[2]; i != inib[3]; i++, i &= 0xf)
        if (s390_record_gpr_g (gdbarch, regcache, i))
          return -1;
      if (s390_record_gpr_g (gdbarch, regcache, inib[3]))
        return -1;
      break;

    case 0xf0: /* SRP - shift and round decimal */
    case 0xf8: /* ZAP - zero and add */
    case 0xfa: /* AP - add decimal */
    case 0xfb: /* SP - subtract decimal */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, inib[2] + 1))
        return -1;
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      /* DXC may be written */
      if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
        return -1;
      break;

    case 0xf1: /* MVO - move with offset */
    case 0xf2: /* PACK - pack */
    case 0xf3: /* UNPK - unpack */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, inib[2] + 1))
        return -1;
      break;

    /* 0xf4-0xf7 undefined */

    case 0xf9: /* CP - compare decimal */
      if (record_full_arch_list_add_reg (regcache, S390_PSWM_REGNUM))
        return -1;
      /* DXC may be written */
      if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
        return -1;
      break;

    case 0xfc: /* MP - multiply decimal */
    case 0xfd: /* DP - divide decimal */
      oaddr = s390_record_calc_disp (gdbarch, regcache, 0, insn[1], 0);
      if (record_full_arch_list_add_mem (oaddr, inib[2] + 1))
        return -1;
      /* DXC may be written */
      if (record_full_arch_list_add_reg (regcache, S390_FPC_REGNUM))
        return -1;
      break;

    /* 0xfe-0xff undefined */

    default:
UNKNOWN_OP:
      fprintf_unfiltered (gdb_stdlog, "Warning: Don't know how to record %04x "
                          "at %s.\n", insn[0], paddress (gdbarch, addr));
      return -1;
  }

  if (record_full_arch_list_add_reg (regcache, S390_PSWA_REGNUM))
    return -1;
  if (record_full_arch_list_add_end ())
    return -1;
  return 0;
}

/* Initialize linux_record_tdep if not initialized yet.  */

static void
s390_init_linux_record_tdep (struct linux_record_tdep *record_tdep,
                             enum s390_abi_kind abi)
{
  /* These values are the size of the type that will be used in a system
     call.  They are obtained from Linux Kernel source.  */

  if (abi == ABI_LINUX_ZSERIES)
    {
      record_tdep->size_pointer = 8;
      /* no _old_kernel_stat */
      record_tdep->size_tms = 32;
      record_tdep->size_loff_t = 8;
      record_tdep->size_flock = 32;
      record_tdep->size_ustat = 32;
      record_tdep->size_old_sigaction = 32;
      record_tdep->size_old_sigset_t = 8;
      record_tdep->size_rlimit = 16;
      record_tdep->size_rusage = 144;
      record_tdep->size_timeval = 16;
      record_tdep->size_timezone = 8;
      /* old_[ug]id_t never used */
      record_tdep->size_fd_set = 128;
      record_tdep->size_old_dirent = 280;
      record_tdep->size_statfs = 88;
      record_tdep->size_statfs64 = 88;
      record_tdep->size_sockaddr = 16;
      record_tdep->size_int = 4;
      record_tdep->size_long = 8;
      record_tdep->size_ulong = 8;
      record_tdep->size_msghdr = 56;
      record_tdep->size_itimerval = 32;
      record_tdep->size_stat = 144;
      /* old_utsname unused */
      record_tdep->size_sysinfo = 112;
      record_tdep->size_msqid_ds = 120;
      record_tdep->size_shmid_ds = 112;
      record_tdep->size_new_utsname = 390;
      record_tdep->size_timex = 208;
      record_tdep->size_mem_dqinfo = 24;
      record_tdep->size_if_dqblk = 72;
      record_tdep->size_fs_quota_stat = 80;
      record_tdep->size_timespec = 16;
      record_tdep->size_pollfd = 8;
      record_tdep->size_NFS_FHSIZE = 32;
      record_tdep->size_knfsd_fh = 132;
      record_tdep->size_TASK_COMM_LEN = 16;
      record_tdep->size_sigaction = 32;
      record_tdep->size_sigset_t = 8;
      record_tdep->size_siginfo_t = 128;
      record_tdep->size_cap_user_data_t = 12;
      record_tdep->size_stack_t = 24;
      record_tdep->size_off_t = 8;
      /* stat64 unused */
      record_tdep->size_gid_t = 4;
      record_tdep->size_uid_t = 4;
      record_tdep->size_PAGE_SIZE = 0x1000;        /* 4KB */
      record_tdep->size_flock64 = 32;
      record_tdep->size_io_event = 32;
      record_tdep->size_iocb = 64;
      record_tdep->size_epoll_event = 16;
      record_tdep->size_itimerspec = 32;
      record_tdep->size_mq_attr = 64;
      record_tdep->size_termios = 36;
      record_tdep->size_termios2 = 44;
      record_tdep->size_pid_t = 4;
      record_tdep->size_winsize = 8;
      record_tdep->size_serial_struct = 72;
      record_tdep->size_serial_icounter_struct = 80;
      record_tdep->size_size_t = 8;
      record_tdep->size_iovec = 16;
      record_tdep->size_time_t = 8;
    }
  else if (abi == ABI_LINUX_S390)
    {
      record_tdep->size_pointer = 4;
      record_tdep->size__old_kernel_stat = 32;
      record_tdep->size_tms = 16;
      record_tdep->size_loff_t = 8;
      record_tdep->size_flock = 16;
      record_tdep->size_ustat = 20;
      record_tdep->size_old_sigaction = 16;
      record_tdep->size_old_sigset_t = 4;
      record_tdep->size_rlimit = 8;
      record_tdep->size_rusage = 72;
      record_tdep->size_timeval = 8;
      record_tdep->size_timezone = 8;
      record_tdep->size_old_gid_t = 2;
      record_tdep->size_old_uid_t = 2;
      record_tdep->size_fd_set = 128;
      record_tdep->size_old_dirent = 268;
      record_tdep->size_statfs = 64;
      record_tdep->size_statfs64 = 88;
      record_tdep->size_sockaddr = 16;
      record_tdep->size_int = 4;
      record_tdep->size_long = 4;
      record_tdep->size_ulong = 4;
      record_tdep->size_msghdr = 28;
      record_tdep->size_itimerval = 16;
      record_tdep->size_stat = 64;
      /* old_utsname unused */
      record_tdep->size_sysinfo = 64;
      record_tdep->size_msqid_ds = 88;
      record_tdep->size_shmid_ds = 84;
      record_tdep->size_new_utsname = 390;
      record_tdep->size_timex = 128;
      record_tdep->size_mem_dqinfo = 24;
      record_tdep->size_if_dqblk = 72;
      record_tdep->size_fs_quota_stat = 80;
      record_tdep->size_timespec = 8;
      record_tdep->size_pollfd = 8;
      record_tdep->size_NFS_FHSIZE = 32;
      record_tdep->size_knfsd_fh = 132;
      record_tdep->size_TASK_COMM_LEN = 16;
      record_tdep->size_sigaction = 20;
      record_tdep->size_sigset_t = 8;
      record_tdep->size_siginfo_t = 128;
      record_tdep->size_cap_user_data_t = 12;
      record_tdep->size_stack_t = 12;
      record_tdep->size_off_t = 4;
      record_tdep->size_stat64 = 104;
      record_tdep->size_gid_t = 4;
      record_tdep->size_uid_t = 4;
      record_tdep->size_PAGE_SIZE = 0x1000;        /* 4KB */
      record_tdep->size_flock64 = 32;
      record_tdep->size_io_event = 32;
      record_tdep->size_iocb = 64;
      record_tdep->size_epoll_event = 16;
      record_tdep->size_itimerspec = 16;
      record_tdep->size_mq_attr = 32;
      record_tdep->size_termios = 36;
      record_tdep->size_termios2 = 44;
      record_tdep->size_pid_t = 4;
      record_tdep->size_winsize = 8;
      record_tdep->size_serial_struct = 60;
      record_tdep->size_serial_icounter_struct = 80;
      record_tdep->size_size_t = 4;
      record_tdep->size_iovec = 8;
      record_tdep->size_time_t = 4;
    }

  /* These values are the second argument of system call "sys_fcntl"
     and "sys_fcntl64".  They are obtained from Linux Kernel source.  */
  record_tdep->fcntl_F_GETLK = 5;
  record_tdep->fcntl_F_GETLK64 = 12;
  record_tdep->fcntl_F_SETLK64 = 13;
  record_tdep->fcntl_F_SETLKW64 = 14;

  record_tdep->arg1 = S390_R2_REGNUM;
  record_tdep->arg2 = S390_R3_REGNUM;
  record_tdep->arg3 = S390_R4_REGNUM;
  record_tdep->arg4 = S390_R5_REGNUM;
  record_tdep->arg5 = S390_R6_REGNUM;

  /* These values are the second argument of system call "sys_ioctl".
     They are obtained from Linux Kernel source.
     See arch/s390/include/uapi/asm/ioctls.h.  */

  record_tdep->ioctl_TCGETS = 0x5401;
  record_tdep->ioctl_TCSETS = 0x5402;
  record_tdep->ioctl_TCSETSW = 0x5403;
  record_tdep->ioctl_TCSETSF = 0x5404;
  record_tdep->ioctl_TCGETA = 0x5405;
  record_tdep->ioctl_TCSETA = 0x5406;
  record_tdep->ioctl_TCSETAW = 0x5407;
  record_tdep->ioctl_TCSETAF = 0x5408;
  record_tdep->ioctl_TCSBRK = 0x5409;
  record_tdep->ioctl_TCXONC = 0x540a;
  record_tdep->ioctl_TCFLSH = 0x540b;
  record_tdep->ioctl_TIOCEXCL = 0x540c;
  record_tdep->ioctl_TIOCNXCL = 0x540d;
  record_tdep->ioctl_TIOCSCTTY = 0x540e;
  record_tdep->ioctl_TIOCGPGRP = 0x540f;
  record_tdep->ioctl_TIOCSPGRP = 0x5410;
  record_tdep->ioctl_TIOCOUTQ = 0x5411;
  record_tdep->ioctl_TIOCSTI = 0x5412;
  record_tdep->ioctl_TIOCGWINSZ = 0x5413;
  record_tdep->ioctl_TIOCSWINSZ = 0x5414;
  record_tdep->ioctl_TIOCMGET = 0x5415;
  record_tdep->ioctl_TIOCMBIS = 0x5416;
  record_tdep->ioctl_TIOCMBIC = 0x5417;
  record_tdep->ioctl_TIOCMSET = 0x5418;
  record_tdep->ioctl_TIOCGSOFTCAR = 0x5419;
  record_tdep->ioctl_TIOCSSOFTCAR = 0x541a;
  record_tdep->ioctl_FIONREAD = 0x541b;
  record_tdep->ioctl_TIOCINQ = 0x541b; /* alias */
  record_tdep->ioctl_TIOCLINUX = 0x541c;
  record_tdep->ioctl_TIOCCONS = 0x541d;
  record_tdep->ioctl_TIOCGSERIAL = 0x541e;
  record_tdep->ioctl_TIOCSSERIAL = 0x541f;
  record_tdep->ioctl_TIOCPKT = 0x5420;
  record_tdep->ioctl_FIONBIO = 0x5421;
  record_tdep->ioctl_TIOCNOTTY = 0x5422;
  record_tdep->ioctl_TIOCSETD = 0x5423;
  record_tdep->ioctl_TIOCGETD = 0x5424;
  record_tdep->ioctl_TCSBRKP = 0x5425;
  record_tdep->ioctl_TIOCSBRK = 0x5427;
  record_tdep->ioctl_TIOCCBRK = 0x5428;
  record_tdep->ioctl_TIOCGSID = 0x5429;
  record_tdep->ioctl_TCGETS2 = 0x802c542a;
  record_tdep->ioctl_TCSETS2 = 0x402c542b;
  record_tdep->ioctl_TCSETSW2 = 0x402c542c;
  record_tdep->ioctl_TCSETSF2 = 0x402c542d;
  record_tdep->ioctl_TIOCGPTN = 0x80045430;
  record_tdep->ioctl_TIOCSPTLCK = 0x40045431;
  record_tdep->ioctl_FIONCLEX = 0x5450;
  record_tdep->ioctl_FIOCLEX = 0x5451;
  record_tdep->ioctl_FIOASYNC = 0x5452;
  record_tdep->ioctl_TIOCSERCONFIG = 0x5453;
  record_tdep->ioctl_TIOCSERGWILD = 0x5454;
  record_tdep->ioctl_TIOCSERSWILD = 0x5455;
  record_tdep->ioctl_TIOCGLCKTRMIOS = 0x5456;
  record_tdep->ioctl_TIOCSLCKTRMIOS = 0x5457;
  record_tdep->ioctl_TIOCSERGSTRUCT = 0x5458;
  record_tdep->ioctl_TIOCSERGETLSR = 0x5459;
  record_tdep->ioctl_TIOCSERGETMULTI = 0x545a;
  record_tdep->ioctl_TIOCSERSETMULTI = 0x545b;
  record_tdep->ioctl_TIOCMIWAIT = 0x545c;
  record_tdep->ioctl_TIOCGICOUNT = 0x545d;
  record_tdep->ioctl_FIOQSIZE = 0x545e;
}


/* Initialize GNU/Linux specific gdbarch hooks.  */

void
s390_gdbarch_linux_init (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  linux_init_abi (info, gdbarch);

  set_gdbarch_guess_tracepoint_registers (gdbarch, s390_guess_tracepoint_registers);

  /* Syscall handling.  */
  set_gdbarch_get_syscall_number (gdbarch, s390_linux_get_syscall_number);

  switch (tdep->abi)
    {
    case ABI_LINUX_S390:
      set_solib_svr4_fetch_link_map_offsets
	(gdbarch, svr4_ilp32_fetch_link_map_offsets);
      set_xml_syscall_file_name (gdbarch, XML_SYSCALL_FILENAME_S390);
      break;

    case ABI_LINUX_ZSERIES:
      set_solib_svr4_fetch_link_map_offsets
	(gdbarch, svr4_lp64_fetch_link_map_offsets);
      set_xml_syscall_file_name (gdbarch, XML_SYSCALL_FILENAME_S390X);
      break;
    }

  set_gdbarch_skip_trampoline_code (gdbarch, find_solib_trampoline_target);

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
					     svr4_fetch_objfile_link_map);

  set_gdbarch_stap_is_single_operand (gdbarch, s390_stap_is_single_operand);
  set_gdbarch_gcc_target_options (gdbarch, s390_gcc_target_options);
  set_gdbarch_gnu_triplet_regexp (gdbarch, s390_gnu_triplet_regexp);

  /* Support reverse debugging.  */
  set_gdbarch_process_record (gdbarch, s390_process_record);
  set_gdbarch_process_record_signal (gdbarch, s390_linux_record_signal);

  s390_init_linux_record_tdep (&s390_linux_record_tdep, ABI_LINUX_S390);
  s390_init_linux_record_tdep (&s390x_linux_record_tdep, ABI_LINUX_ZSERIES);
}

extern initialize_file_ftype _initialize_s390_linux_tdep; /* -Wmissing-prototypes */

void
_initialize_s390_linux_tdep (void)
{
  /* Initialize the GNU/Linux target descriptions.  */
  initialize_tdesc_s390_linux32 ();
  initialize_tdesc_s390_linux32v1 ();
  initialize_tdesc_s390_linux32v2 ();
  initialize_tdesc_s390_linux64 ();
  initialize_tdesc_s390_linux64v1 ();
  initialize_tdesc_s390_linux64v2 ();
  initialize_tdesc_s390_te_linux64 ();
  initialize_tdesc_s390_vx_linux64 ();
  initialize_tdesc_s390_tevx_linux64 ();
  initialize_tdesc_s390x_linux64 ();
  initialize_tdesc_s390x_linux64v1 ();
  initialize_tdesc_s390x_linux64v2 ();
  initialize_tdesc_s390x_te_linux64 ();
  initialize_tdesc_s390x_vx_linux64 ();
  initialize_tdesc_s390x_tevx_linux64 ();
}
