/* GNU/Linux S/390 specific low level interface, for the remote server
   for GDB.
   Copyright (C) 2001-2016 Free Software Foundation, Inc.

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

/* This file is used for both 31-bit and 64-bit S/390 systems.  */

#include "server.h"
#include "linux-low.h"
#include "elf/common.h"

#include <asm/ptrace.h>
#include "nat/gdb_ptrace.h"
#include <sys/uio.h>
#include <elf.h>
#include <inttypes.h>

#include "linux-s390-tdesc.h"

#ifndef HWCAP_S390_HIGH_GPRS
#define HWCAP_S390_HIGH_GPRS 512
#endif

#ifndef HWCAP_S390_TE
#define HWCAP_S390_TE 1024
#endif

#ifndef HWCAP_S390_VX
#define HWCAP_S390_VX 2048
#endif

#define s390_num_regs 52

static int s390_regmap[] = {
  PT_PSWMASK, PT_PSWADDR,

  PT_GPR0, PT_GPR1, PT_GPR2, PT_GPR3,
  PT_GPR4, PT_GPR5, PT_GPR6, PT_GPR7,
  PT_GPR8, PT_GPR9, PT_GPR10, PT_GPR11,
  PT_GPR12, PT_GPR13, PT_GPR14, PT_GPR15,

  PT_ACR0, PT_ACR1, PT_ACR2, PT_ACR3,
  PT_ACR4, PT_ACR5, PT_ACR6, PT_ACR7,
  PT_ACR8, PT_ACR9, PT_ACR10, PT_ACR11,
  PT_ACR12, PT_ACR13, PT_ACR14, PT_ACR15,

  PT_FPC,

#ifndef __s390x__
  PT_FPR0_HI, PT_FPR1_HI, PT_FPR2_HI, PT_FPR3_HI,
  PT_FPR4_HI, PT_FPR5_HI, PT_FPR6_HI, PT_FPR7_HI,
  PT_FPR8_HI, PT_FPR9_HI, PT_FPR10_HI, PT_FPR11_HI,
  PT_FPR12_HI, PT_FPR13_HI, PT_FPR14_HI, PT_FPR15_HI,
#else
  PT_FPR0, PT_FPR1, PT_FPR2, PT_FPR3,
  PT_FPR4, PT_FPR5, PT_FPR6, PT_FPR7,
  PT_FPR8, PT_FPR9, PT_FPR10, PT_FPR11,
  PT_FPR12, PT_FPR13, PT_FPR14, PT_FPR15,
#endif

  PT_ORIGGPR2,
};

#define s390_num_regs_3264 68

#ifdef __s390x__
static int s390_regmap_3264[] = {
  PT_PSWMASK, PT_PSWADDR,

  PT_GPR0, PT_GPR0, PT_GPR1, PT_GPR1,
  PT_GPR2, PT_GPR2, PT_GPR3, PT_GPR3,
  PT_GPR4, PT_GPR4, PT_GPR5, PT_GPR5,
  PT_GPR6, PT_GPR6, PT_GPR7, PT_GPR7,
  PT_GPR8, PT_GPR8, PT_GPR9, PT_GPR9,
  PT_GPR10, PT_GPR10, PT_GPR11, PT_GPR11,
  PT_GPR12, PT_GPR12, PT_GPR13, PT_GPR13,
  PT_GPR14, PT_GPR14, PT_GPR15, PT_GPR15,

  PT_ACR0, PT_ACR1, PT_ACR2, PT_ACR3,
  PT_ACR4, PT_ACR5, PT_ACR6, PT_ACR7,
  PT_ACR8, PT_ACR9, PT_ACR10, PT_ACR11,
  PT_ACR12, PT_ACR13, PT_ACR14, PT_ACR15,

  PT_FPC,

  PT_FPR0, PT_FPR1, PT_FPR2, PT_FPR3,
  PT_FPR4, PT_FPR5, PT_FPR6, PT_FPR7,
  PT_FPR8, PT_FPR9, PT_FPR10, PT_FPR11,
  PT_FPR12, PT_FPR13, PT_FPR14, PT_FPR15,

  PT_ORIGGPR2,
};
#else
static int s390_regmap_3264[] = {
  PT_PSWMASK, PT_PSWADDR,

  -1, PT_GPR0, -1, PT_GPR1,
  -1, PT_GPR2, -1, PT_GPR3,
  -1, PT_GPR4, -1, PT_GPR5,
  -1, PT_GPR6, -1, PT_GPR7,
  -1, PT_GPR8, -1, PT_GPR9,
  -1, PT_GPR10, -1, PT_GPR11,
  -1, PT_GPR12, -1, PT_GPR13,
  -1, PT_GPR14, -1, PT_GPR15,

  PT_ACR0, PT_ACR1, PT_ACR2, PT_ACR3,
  PT_ACR4, PT_ACR5, PT_ACR6, PT_ACR7,
  PT_ACR8, PT_ACR9, PT_ACR10, PT_ACR11,
  PT_ACR12, PT_ACR13, PT_ACR14, PT_ACR15,

  PT_FPC,

  PT_FPR0_HI, PT_FPR1_HI, PT_FPR2_HI, PT_FPR3_HI,
  PT_FPR4_HI, PT_FPR5_HI, PT_FPR6_HI, PT_FPR7_HI,
  PT_FPR8_HI, PT_FPR9_HI, PT_FPR10_HI, PT_FPR11_HI,
  PT_FPR12_HI, PT_FPR13_HI, PT_FPR14_HI, PT_FPR15_HI,

  PT_ORIGGPR2,
};
#endif


static int
s390_cannot_fetch_register (int regno)
{
  return 0;
}

static int
s390_cannot_store_register (int regno)
{
  return 0;
}

static void
s390_collect_ptrace_register (struct regcache *regcache, int regno, char *buf)
{
  int size = register_size (regcache->tdesc, regno);
  const struct regs_info *regs_info = (*the_low_target.regs_info) ();
  struct usrregs_info *usr = regs_info->usrregs;
  int regaddr = usr->regmap[regno];

  if (size < sizeof (long))
    {
      memset (buf, 0, sizeof (long));

      if ((regno ^ 1) < usr->num_regs
	  && usr->regmap[regno ^ 1] == regaddr)
	{
	  collect_register (regcache, regno & ~1, buf);
	  collect_register (regcache, (regno & ~1) + 1,
			    buf + sizeof (long) - size);
	}
      else if (regaddr == PT_PSWMASK)
	{
	  /* Convert 4-byte PSW mask to 8 bytes by clearing bit 12 and copying
	     the basic addressing mode bit from the PSW address.  */
	  char *addr = alloca (register_size (regcache->tdesc, regno ^ 1));
	  collect_register (regcache, regno, buf);
	  collect_register (regcache, regno ^ 1, addr);
	  buf[1] &= ~0x8;
	  buf[size] |= (addr[0] & 0x80);
	}
      else if (regaddr == PT_PSWADDR)
	{
	  /* Convert 4-byte PSW address to 8 bytes by clearing the addressing
	     mode bit (which gets copied to the PSW mask instead).  */
	  collect_register (regcache, regno, buf + sizeof (long) - size);
	  buf[sizeof (long) - size] &= ~0x80;
	}
      else if ((regaddr >= PT_GPR0 && regaddr <= PT_GPR15)
	       || regaddr == PT_ORIGGPR2)
	collect_register (regcache, regno, buf + sizeof (long) - size);
      else
	collect_register (regcache, regno, buf);
    }
  else if (regaddr != -1)
    collect_register (regcache, regno, buf);
}

static void
s390_supply_ptrace_register (struct regcache *regcache,
			     int regno, const char *buf)
{
  int size = register_size (regcache->tdesc, regno);
  const struct regs_info *regs_info = (*the_low_target.regs_info) ();
  struct usrregs_info *usr = regs_info->usrregs;
  int regaddr = usr->regmap[regno];

  if (size < sizeof (long))
    {
      if ((regno ^ 1) < usr->num_regs
	  && usr->regmap[regno ^ 1] == regaddr)
	{
	  supply_register (regcache, regno & ~1, buf);
	  supply_register (regcache, (regno & ~1) + 1,
			   buf + sizeof (long) - size);
	}
      else if (regaddr == PT_PSWMASK)
	{
	  /* Convert 8-byte PSW mask to 4 bytes by setting bit 12 and copying
	     the basic addressing mode into the PSW address.  */
	  char *mask = alloca (size);
	  char *addr = alloca (register_size (regcache->tdesc, regno ^ 1));
	  memcpy (mask, buf, size);
	  mask[1] |= 0x8;
	  supply_register (regcache, regno, mask);

	  collect_register (regcache, regno ^ 1, addr);
	  addr[0] &= ~0x80;
	  addr[0] |= (buf[size] & 0x80);
	  supply_register (regcache, regno ^ 1, addr);
	}
      else if (regaddr == PT_PSWADDR)
	{
	  /* Convert 8-byte PSW address to 4 bytes by truncating, but
	     keeping the addressing mode bit (which was set from the mask).  */
	  char *addr = alloca (size);
	  char amode;
	  collect_register (regcache, regno, addr);
	  amode = addr[0] & 0x80;
	  memcpy (addr, buf + sizeof (long) - size, size);
	  addr[0] &= ~0x80;
	  addr[0] |= amode;
	  supply_register (regcache, regno, addr);
	}
      else if ((regaddr >= PT_GPR0 && regaddr <= PT_GPR15)
	       || regaddr == PT_ORIGGPR2)
	supply_register (regcache, regno, buf + sizeof (long) - size);
      else
	supply_register (regcache, regno, buf);
    }
  else if (regaddr != -1)
    supply_register (regcache, regno, buf);
}

/* Provide only a fill function for the general register set.  ps_lgetregs
   will use this for NPTL support.  */

static void
s390_fill_gregset (struct regcache *regcache, void *buf)
{
  int i;
  const struct regs_info *regs_info = (*the_low_target.regs_info) ();
  struct usrregs_info *usr = regs_info->usrregs;

  for (i = 0; i < usr->num_regs; i++)
    {
      if (usr->regmap[i] < PT_PSWMASK
	  || usr->regmap[i] > PT_ACR15)
	continue;

      s390_collect_ptrace_register (regcache, i,
				    (char *) buf + usr->regmap[i]);
    }
}

/* Fill and store functions for extended register sets.  */

#ifndef __s390x__
static void
s390_fill_gprs_high (struct regcache *regcache, void *buf)
{
  int r0h = find_regno (regcache->tdesc, "r0h");
  int i;

  for (i = 0; i < 16; i++)
    collect_register (regcache, r0h + 2 * i, (char *) buf + 4 * i);
}

static void
s390_store_gprs_high (struct regcache *regcache, const void *buf)
{
  int r0h = find_regno (regcache->tdesc, "r0h");
  int i;

  for (i = 0; i < 16; i++)
    supply_register (regcache, r0h + 2 * i, (const char *) buf + 4 * i);
}
#endif

static void
s390_store_last_break (struct regcache *regcache, const void *buf)
{
  const char *p;

  p = (const char *) buf + 8 - register_size (regcache->tdesc, 0);
  supply_register_by_name (regcache, "last_break", p);
}

static void
s390_fill_system_call (struct regcache *regcache, void *buf)
{
  collect_register_by_name (regcache, "system_call", buf);
}

static void
s390_store_system_call (struct regcache *regcache, const void *buf)
{
  supply_register_by_name (regcache, "system_call", buf);
}

static void
s390_store_tdb (struct regcache *regcache, const void *buf)
{
  int tdb0 = find_regno (regcache->tdesc, "tdb0");
  int tr0 = find_regno (regcache->tdesc, "tr0");
  int i;

  for (i = 0; i < 4; i++)
    supply_register (regcache, tdb0 + i, (const char *) buf + 8 * i);

  for (i = 0; i < 16; i++)
    supply_register (regcache, tr0 + i, (const char *) buf + 8 * (16 + i));
}

static void
s390_fill_vxrs_low (struct regcache *regcache, void *buf)
{
  int v0 = find_regno (regcache->tdesc, "v0l");
  int i;

  for (i = 0; i < 16; i++)
    collect_register (regcache, v0 + i, (char *) buf + 8 * i);
}

static void
s390_store_vxrs_low (struct regcache *regcache, const void *buf)
{
  int v0 = find_regno (regcache->tdesc, "v0l");
  int i;

  for (i = 0; i < 16; i++)
    supply_register (regcache, v0 + i, (const char *) buf + 8 * i);
}

static void
s390_fill_vxrs_high (struct regcache *regcache, void *buf)
{
  int v16 = find_regno (regcache->tdesc, "v16");
  int i;

  for (i = 0; i < 16; i++)
    collect_register (regcache, v16 + i, (char *) buf + 16 * i);
}

static void
s390_store_vxrs_high (struct regcache *regcache, const void *buf)
{
  int v16 = find_regno (regcache->tdesc, "v16");
  int i;

  for (i = 0; i < 16; i++)
    supply_register (regcache, v16 + i, (const char *) buf + 16 * i);
}

static struct regset_info s390_regsets[] = {
  { 0, 0, 0, 0, GENERAL_REGS, s390_fill_gregset, NULL },
#ifndef __s390x__
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_S390_HIGH_GPRS, 0,
    EXTENDED_REGS, s390_fill_gprs_high, s390_store_gprs_high },
#endif
  /* Last break address is read-only; no fill function.  */
  { PTRACE_GETREGSET, -1, NT_S390_LAST_BREAK, 0, EXTENDED_REGS,
    NULL, s390_store_last_break },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_S390_SYSTEM_CALL, 0,
    EXTENDED_REGS, s390_fill_system_call, s390_store_system_call },
  /* TDB is read-only.  */
  { PTRACE_GETREGSET, -1, NT_S390_TDB, 0, EXTENDED_REGS,
    NULL, s390_store_tdb },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_S390_VXRS_LOW, 0,
    EXTENDED_REGS, s390_fill_vxrs_low, s390_store_vxrs_low },
  { PTRACE_GETREGSET, PTRACE_SETREGSET, NT_S390_VXRS_HIGH, 0,
    EXTENDED_REGS, s390_fill_vxrs_high, s390_store_vxrs_high },
  NULL_REGSET
};


static const gdb_byte s390_breakpoint[] = { 0, 1 };
#define s390_breakpoint_len 2

/* Implementation of linux_target_ops method "sw_breakpoint_from_kind".  */

static const gdb_byte *
s390_sw_breakpoint_from_kind (int kind, int *size)
{
  *size = s390_breakpoint_len;
  return s390_breakpoint;
}

static CORE_ADDR
s390_get_pc (struct regcache *regcache)
{
  if (register_size (regcache->tdesc, 0) == 4)
    {
      unsigned int pswa;
      collect_register_by_name (regcache, "pswa", &pswa);
      return pswa & 0x7fffffff;
    }
  else
    {
      unsigned long pc;
      collect_register_by_name (regcache, "pswa", &pc);
      return pc;
    }
}

static void
s390_set_pc (struct regcache *regcache, CORE_ADDR newpc)
{
  if (register_size (regcache->tdesc, 0) == 4)
    {
      unsigned int pswa;
      collect_register_by_name (regcache, "pswa", &pswa);
      pswa = (pswa & 0x80000000) | (newpc & 0x7fffffff);
      supply_register_by_name (regcache, "pswa", &pswa);
    }
  else
    {
      unsigned long pc = newpc;
      supply_register_by_name (regcache, "pswa", &pc);
    }
}

static unsigned long
s390_get_hwcap (const struct target_desc *tdesc)
{
  int wordsize = register_size (tdesc, 0);
  unsigned char *data = alloca (2 * wordsize);
  int offset = 0;

  while ((*the_target->read_auxv) (offset, data, 2 * wordsize) == 2 * wordsize)
    {
      if (wordsize == 4)
        {
          unsigned int *data_p = (unsigned int *)data;
          if (data_p[0] == AT_HWCAP)
	    return data_p[1];
        }
      else
        {
          unsigned long *data_p = (unsigned long *)data;
          if (data_p[0] == AT_HWCAP)
	    return data_p[1];
        }

      offset += 2 * wordsize;
    }

  return 0;
}

static int
s390_check_regset (int pid, int regset, int regsize)
{
  gdb_byte *buf = alloca (regsize);
  struct iovec iov;

  iov.iov_base = buf;
  iov.iov_len = regsize;

  if (ptrace (PTRACE_GETREGSET, pid, (long) regset, (long) &iov) >= 0
      || errno == ENODATA)
    return 1;
  return 0;
}

/* For a 31-bit inferior, whether the kernel supports using the full
   64-bit GPRs.  */
static int have_hwcap_s390_high_gprs = 0;
static int have_hwcap_s390_vx = 0;

static void
s390_arch_setup (void)
{
  const struct target_desc *tdesc;
  struct regset_info *regset;

  /* Check whether the kernel supports extra register sets.  */
  int pid = pid_of (current_thread);
  int have_regset_last_break
    = s390_check_regset (pid, NT_S390_LAST_BREAK, 8);
  int have_regset_system_call
    = s390_check_regset (pid, NT_S390_SYSTEM_CALL, 4);
  int have_regset_tdb = s390_check_regset (pid, NT_S390_TDB, 256);
  int have_regset_vxrs = s390_check_regset (pid, NT_S390_VXRS_LOW, 128)
    && s390_check_regset (pid, NT_S390_VXRS_HIGH, 256);

  /* Assume 31-bit inferior process.  */
  if (have_regset_system_call)
    tdesc = tdesc_s390_linux32v2;
  else if (have_regset_last_break)
    tdesc = tdesc_s390_linux32v1;
  else
    tdesc = tdesc_s390_linux32;

  /* On a 64-bit host, check the low bit of the (31-bit) PSWM
     -- if this is one, we actually have a 64-bit inferior.  */
  {
#ifdef __s390x__
    unsigned int pswm;
    struct regcache *regcache = new_register_cache (tdesc);

    fetch_inferior_registers (regcache, find_regno (tdesc, "pswm"));
    collect_register_by_name (regcache, "pswm", &pswm);
    free_register_cache (regcache);

    if (pswm & 1)
      {
	if (have_regset_tdb)
	  have_regset_tdb =
	    (s390_get_hwcap (tdesc_s390x_linux64v2) & HWCAP_S390_TE) != 0;
	if (have_regset_vxrs)
	  have_regset_vxrs =
	    (s390_get_hwcap (tdesc_s390x_linux64v2) & HWCAP_S390_VX) != 0;

	if (have_regset_vxrs)
	  tdesc = (have_regset_tdb ? tdesc_s390x_tevx_linux64 :
		   tdesc_s390x_vx_linux64);
	else if (have_regset_tdb)
	  tdesc = tdesc_s390x_te_linux64;
	else if (have_regset_system_call)
	  tdesc = tdesc_s390x_linux64v2;
	else if (have_regset_last_break)
	  tdesc = tdesc_s390x_linux64v1;
	else
	  tdesc = tdesc_s390x_linux64;
      }

    /* For a 31-bit inferior, check whether the kernel supports
       using the full 64-bit GPRs.  */
    else
#endif
    if (s390_get_hwcap (tdesc) & HWCAP_S390_HIGH_GPRS)
      {
	have_hwcap_s390_high_gprs = 1;
	if (have_regset_tdb)
	  have_regset_tdb = (s390_get_hwcap (tdesc) & HWCAP_S390_TE) != 0;
	if (have_regset_vxrs)
	  have_regset_vxrs = (s390_get_hwcap (tdesc) & HWCAP_S390_VX) != 0;

	if (have_regset_vxrs)
	  tdesc = (have_regset_tdb ? tdesc_s390_tevx_linux64 :
		   tdesc_s390_vx_linux64);
	else if (have_regset_tdb)
	  tdesc = tdesc_s390_te_linux64;
	else if (have_regset_system_call)
	  tdesc = tdesc_s390_linux64v2;
	else if (have_regset_last_break)
	  tdesc = tdesc_s390_linux64v1;
	else
	  tdesc = tdesc_s390_linux64;
      }

    have_hwcap_s390_vx = have_regset_vxrs;
  }

  /* Update target_regsets according to available register sets.  */
  for (regset = s390_regsets; regset->size >= 0; regset++)
    if (regset->get_request == PTRACE_GETREGSET)
      switch (regset->nt_type)
	{
#ifndef __s390x__
	case NT_S390_HIGH_GPRS:
	  regset->size = have_hwcap_s390_high_gprs ? 64 : 0;
	  break;
#endif
	case NT_S390_LAST_BREAK:
	  regset->size = have_regset_last_break ? 8 : 0;
	  break;
	case NT_S390_SYSTEM_CALL:
	  regset->size = have_regset_system_call ? 4 : 0;
	  break;
	case NT_S390_TDB:
	  regset->size = have_regset_tdb ? 256 : 0;
	  break;
	case NT_S390_VXRS_LOW:
	  regset->size = have_regset_vxrs ? 128 : 0;
	  break;
	case NT_S390_VXRS_HIGH:
	  regset->size = have_regset_vxrs ? 256 : 0;
	  break;
	default:
	  break;
	}

  current_process ()->tdesc = tdesc;
}


static int
s390_breakpoint_at (CORE_ADDR pc)
{
  unsigned char c[s390_breakpoint_len];
  read_inferior_memory (pc, c, s390_breakpoint_len);
  return memcmp (c, s390_breakpoint, s390_breakpoint_len) == 0;
}

/* Breakpoint/Watchpoint support.  */

/* The "supports_z_point_type" linux_target_ops method.  */

static int
s390_supports_z_point_type (char z_type)
{
  switch (z_type)
    {
    case Z_PACKET_SW_BP:
      return 1;
    default:
      return 0;
    }
}

/* Support for hardware single step.  */

static int
s390_supports_hardware_single_step (void)
{
  return 1;
}

static struct usrregs_info s390_usrregs_info =
  {
    s390_num_regs,
    s390_regmap,
  };

static struct regsets_info s390_regsets_info =
  {
    s390_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

static struct regs_info regs_info =
  {
    NULL, /* regset_bitmap */
    &s390_usrregs_info,
    &s390_regsets_info
  };

static struct usrregs_info s390_usrregs_info_3264 =
  {
    s390_num_regs_3264,
    s390_regmap_3264
  };

static struct regsets_info s390_regsets_info_3264 =
  {
    s390_regsets, /* regsets */
    0, /* num_regsets */
    NULL, /* disabled_regsets */
  };

static struct regs_info regs_info_3264 =
  {
    NULL, /* regset_bitmap */
    &s390_usrregs_info_3264,
    &s390_regsets_info_3264
  };

static const struct regs_info *
s390_regs_info (void)
{
  if (have_hwcap_s390_high_gprs)
    {
#ifdef __s390x__
      const struct target_desc *tdesc = current_process ()->tdesc;

      if (register_size (tdesc, 0) == 4)
	return &regs_info_3264;
#else
      return &regs_info_3264;
#endif
    }
  return &regs_info;
}

/* The "supports_tracepoints" linux_target_ops method.  */

static int
s390_supports_tracepoints (void)
{
  return 1;
}

/* Implementation of linux_target_ops method "get_thread_area".  */

static int
s390_get_thread_area (int lwpid, CORE_ADDR *addrp)
{
  CORE_ADDR res = ptrace (PTRACE_PEEKUSER, lwpid, (long) PT_ACR0, (long) 0);
#ifdef __s390x__
  struct regcache *regcache = get_thread_regcache (current_thread, 0);

  if (register_size (regcache->tdesc, 0) == 4)
    res &= 0xffffffffull;
#endif
  *addrp = res;
  return 0;
}


/* Fast tracepoint support.

   The register save area on stack is identical for all targets:

   0x000+i*0x10: VR0-VR31
   0x200+i*8: GR0-GR15
   0x280+i*4: AR0-AR15
   0x2c0: PSWM [64-bit]
   0x2c8: PSWA [64-bit]
   0x2d0: FPC

   If we're on 31-bit linux, we just don't store the high parts of the GPRs.
   Likewise, if there's no VX support, we just store the FRs into the slots
   of low VR halves.  The agent code is responsible for rearranging that
   into regcache.  */

/* Code sequence saving GPRs for 31-bit target with no high GPRs.  There's
   one trick used at the very beginning: since there's no way to allocate
   stack space without destroying CC (lay instruction can do it, but it's
   only supported on later CPUs), we take 4 different execution paths for
   every possible value of CC, allocate stack space, save %r0, stuff the
   CC value in %r0 (shifted to match its position in PSWM high word),
   then branch to common path.  */

static const unsigned char s390_ft_entry_gpr_esa[] = {
  0xa7, 0x14, 0x00, 0x1e,		/* jo .Lcc3 */
  0xa7, 0x24, 0x00, 0x14,		/* jh .Lcc2 */
  0xa7, 0x44, 0x00, 0x0a,		/* jl .Lcc1 */
  /* CC = 0 */
  0xa7, 0xfa, 0xfd, 0x00,		/* ahi %r15, -0x300 */
  0x50, 0x00, 0xf2, 0x04,		/* st %r0, 0x204(%r15) */
  0xa7, 0x08, 0x00, 0x00,		/* lhi %r0, 0 */
  0xa7, 0xf4, 0x00, 0x18,		/* j .Lccdone */
  /* .Lcc1: */
  0xa7, 0xfa, 0xfd, 0x00,		/* ahi %r15, -0x300 */
  0x50, 0x00, 0xf2, 0x04,		/* st %r0, 0x204(%r15) */
  0xa7, 0x08, 0x10, 0x00,		/* lhi %r0, 0x1000 */
  0xa7, 0xf4, 0x00, 0x10,		/* j .Lccdone */
  /* .Lcc2: */
  0xa7, 0xfa, 0xfd, 0x00,		/* ahi %r15, -0x300 */
  0x50, 0x00, 0xf2, 0x04,		/* st %r0, 0x204(%r15) */
  0xa7, 0x08, 0x20, 0x00,		/* lhi %r0, 0x2000 */
  0xa7, 0xf4, 0x00, 0x08,		/* j .Lccdone */
  /* .Lcc3: */
  0xa7, 0xfa, 0xfd, 0x00,		/* ahi %r15, -0x300 */
  0x50, 0x00, 0xf2, 0x04,		/* st %r0, 0x204(%r15) */
  0xa7, 0x08, 0x30, 0x00,		/* lhi %r0, 0x3000 */
  /* .Lccdone: */
  0x50, 0x10, 0xf2, 0x0c,		/* st %r1, 0x20c(%r15) */
  0x50, 0x20, 0xf2, 0x14,		/* st %r2, 0x214(%r15) */
  0x50, 0x30, 0xf2, 0x1c,		/* st %r3, 0x21c(%r15) */
  0x50, 0x40, 0xf2, 0x24,		/* st %r4, 0x224(%r15) */
  0x50, 0x50, 0xf2, 0x2c,		/* st %r5, 0x22c(%r15) */
  0x50, 0x60, 0xf2, 0x34,		/* st %r6, 0x234(%r15) */
  0x50, 0x70, 0xf2, 0x3c,		/* st %r7, 0x23c(%r15) */
  0x50, 0x80, 0xf2, 0x44,		/* st %r8, 0x244(%r15) */
  0x50, 0x90, 0xf2, 0x4c,		/* st %r9, 0x24c(%r15) */
  0x50, 0xa0, 0xf2, 0x54,		/* st %r10, 0x254(%r15) */
  0x50, 0xb0, 0xf2, 0x5c,		/* st %r11, 0x25c(%r15) */
  0x50, 0xc0, 0xf2, 0x64,		/* st %r12, 0x264(%r15) */
  0x50, 0xd0, 0xf2, 0x6c,		/* st %r13, 0x26c(%r15) */
  0x50, 0xe0, 0xf2, 0x74,		/* st %r14, 0x274(%r15) */
  /* Compute original value of %r15 and store it.  We use ahi instead
     of la to preserve the whole value, and not just the low 31 bits.
     This is not particularly important here, but essential in the
     zarch case where someone might be using the high word of %r15
     as an extra register.  */
  0x18, 0x1f,				/* lr %r1, %r15 */
  0xa7, 0x1a, 0x03, 0x00,		/* ahi %r1, 0x300 */
  0x50, 0x10, 0xf2, 0x7c,		/* st %r1, 0x27c(%r15) */
};

/* Code sequence saving GPRs for 31-bit target with high GPRs and for 64-bit
   target.  Same as above, except this time we can use load/store multiple,
   since the 64-bit regs are tightly packed.  */

static const unsigned char s390_ft_entry_gpr_zarch[] = {
  0xa7, 0x14, 0x00, 0x21,		/* jo .Lcc3 */
  0xa7, 0x24, 0x00, 0x16,		/* jh .Lcc2 */
  0xa7, 0x44, 0x00, 0x0b,		/* jl .Lcc1 */
  /* CC = 0 */
  0xa7, 0xfb, 0xfd, 0x00,		/* aghi %r15, -0x300 */
  0xeb, 0x0e, 0xf2, 0x00, 0x00, 0x24,	/* stmg %r0, %r14, 0x200(%r15) */
  0xa7, 0x08, 0x00, 0x00,		/* lhi %r0, 0 */
  0xa7, 0xf4, 0x00, 0x1b,		/* j .Lccdone */
  /* .Lcc1: */
  0xa7, 0xfb, 0xfd, 0x00,		/* aghi %r15, -0x300 */
  0xeb, 0x0e, 0xf2, 0x00, 0x00, 0x24,	/* stmg %r0, %r14, 0x200(%r15) */
  0xa7, 0x08, 0x10, 0x00,		/* lhi %r0, 0x1000 */
  0xa7, 0xf4, 0x00, 0x12,		/* j .Lccdone */
  /* .Lcc2: */
  0xa7, 0xfb, 0xfd, 0x00,		/* aghi %r15, -0x300 */
  0xeb, 0x0e, 0xf2, 0x00, 0x00, 0x24,	/* stmg %r0, %r14, 0x200(%r15) */
  0xa7, 0x08, 0x20, 0x00,		/* lhi %r0, 0x2000 */
  0xa7, 0xf4, 0x00, 0x09,		/* j .Lccdone */
  /* .Lcc3: */
  0xa7, 0xfb, 0xfd, 0x00,		/* aghi %r15, -0x300 */
  0xeb, 0x0e, 0xf2, 0x00, 0x00, 0x24,	/* stmg %r0, %r14, 0x200(%r15) */
  0xa7, 0x08, 0x30, 0x00,		/* lhi %r0, 0x3000 */
  /* .Lccdone: */
  0xb9, 0x04, 0x00, 0x1f,		/* lgr %r1, %r15 */
  0xa7, 0x1b, 0x03, 0x00,		/* aghi %r1, 0x300 */
  0xe3, 0x10, 0xf2, 0x78, 0x00, 0x24,	/* stg %r1, 0x278(%r15) */
};

/* Code sequence saving ARs, PSWM and FPC.  PSWM has to be assembled from
   current PSWM (read by epsw) and CC from entry (in %r0).  */

static const unsigned char s390_ft_entry_misc[] = {
  0x9b, 0x0f, 0xf2, 0x80,		/* stam %a0, %a15, 0x20(%%r15) */
  0xb9, 0x8d, 0x00, 0x23,		/* epsw %r2, %r3 */
  0xa7, 0x18, 0xcf, 0xff,		/* lhi %r1, ~0x3000 */
  0x14, 0x21,				/* nr %r2, %r1 */
  0x16, 0x20,				/* or %r2, %r0 */
  0x50, 0x20, 0xf2, 0xc0,		/* st %r2, 0x2c0(%r15) */
  0x50, 0x30, 0xf2, 0xc4,		/* st %r3, 0x2c4(%r15) */
  0xb2, 0x9c, 0xf2, 0xd0,		/* stfpc 0x2d0(%r15) */
};

/* Code sequence saving FRs, used if VX not supported.  */

static const unsigned char s390_ft_entry_fr[] = {
  0x60, 0x00, 0xf0, 0x00,		/* std %f0, 0x000(%r15) */
  0x60, 0x10, 0xf0, 0x10,		/* std %f1, 0x010(%r15) */
  0x60, 0x20, 0xf0, 0x20,		/* std %f2, 0x020(%r15) */
  0x60, 0x30, 0xf0, 0x30,		/* std %f3, 0x030(%r15) */
  0x60, 0x40, 0xf0, 0x40,		/* std %f4, 0x040(%r15) */
  0x60, 0x50, 0xf0, 0x50,		/* std %f5, 0x050(%r15) */
  0x60, 0x60, 0xf0, 0x60,		/* std %f6, 0x060(%r15) */
  0x60, 0x70, 0xf0, 0x70,		/* std %f7, 0x070(%r15) */
  0x60, 0x80, 0xf0, 0x80,		/* std %f8, 0x080(%r15) */
  0x60, 0x90, 0xf0, 0x90,		/* std %f9, 0x090(%r15) */
  0x60, 0xa0, 0xf0, 0xa0,		/* std %f10, 0x0a0(%r15) */
  0x60, 0xb0, 0xf0, 0xb0,		/* std %f11, 0x0b0(%r15) */
  0x60, 0xc0, 0xf0, 0xc0,		/* std %f12, 0x0c0(%r15) */
  0x60, 0xd0, 0xf0, 0xd0,		/* std %f13, 0x0d0(%r15) */
  0x60, 0xe0, 0xf0, 0xe0,		/* std %f14, 0x0e0(%r15) */
  0x60, 0xf0, 0xf0, 0xf0,		/* std %f15, 0x0f0(%r15) */
};

/* Code sequence saving VRs, used if VX not supported.  */

static const unsigned char s390_ft_entry_vr[] = {
  0xe7, 0x0f, 0xf0, 0x00, 0x00, 0x3e,	/* vstm %v0, %v15, 0x000(%r15) */
  0xe7, 0x0f, 0xf1, 0x00, 0x0c, 0x3e,	/* vstm %v16, %v31, 0x100(%r15) */
};

/* Code sequence doing the collection call for 31-bit target.  %r1 contains
   the address of the literal pool.  */

static const unsigned char s390_ft_main_31[] = {
  /* Load the literals into registers.  */
  0x58, 0x50, 0x10, 0x00,		/* l %r5, 0x0(%r1) */
  0x58, 0x20, 0x10, 0x04,		/* l %r2, 0x4(%r1) */
  0x58, 0x40, 0x10, 0x08,		/* l %r4, 0x8(%r1) */
  0x58, 0x60, 0x10, 0x0c,		/* l %r6, 0xc(%r1) */
  /* Save original PSWA (tracepoint address | 0x80000000).  */
  0x50, 0x50, 0xf2, 0xcc,		/* st %r5, 0x2cc(%r15) */
  /* Construct a collecting_t object at %r15+0x2e0.  */
  0x50, 0x20, 0xf2, 0xe0,		/* st %r2, 0x2e0(%r15) */
  0x9b, 0x00, 0xf2, 0xe4,		/* stam %a0, %a0, 0x2e4(%r15) */
  /* Move its address to %r0.  */
  0x41, 0x00, 0xf2, 0xe0,		/* la %r0, 0x2e0(%r15) */
  /* Take the lock.  */
  /* .Lloop:  */
  0xa7, 0x18, 0x00, 0x00,		/* lhi %r1, 0 */
  0xba, 0x10, 0x60, 0x00,		/* cs %r1, %r0, 0(%r6) */
  0xa7, 0x74, 0xff, 0xfc,		/* jne .Lloop */
  /* Address of the register save block to %r3.  */
  0x18, 0x3f,				/* lr %r3, %r15 */
  /* Make a stack frame, so that we can call the collector.  */
  0xa7, 0xfa, 0xff, 0xa0,		/* ahi %r15, -0x60 */
  /* Call it.  */
  0x0d, 0xe4,				/* basr %r14, %r4 */
  /* And get rid of the stack frame again.  */
  0x41, 0xf0, 0xf0, 0x60,		/* la %r15, 0x60(%r15) */
  /* Leave the lock.  */
  0x07, 0xf0, 				/* br %r0 */
  0xa7, 0x18, 0x00, 0x00,		/* lhi %r1, 0 */
  0x50, 0x10, 0x60, 0x00,		/* st %t1, 0(%r6) */
};

/* Code sequence doing the collection call for 64-bit target.  %r1 contains
   the address of the literal pool.  */

static const unsigned char s390_ft_main_64[] = {
  /* Load the literals into registers.  */
  0xe3, 0x50, 0x10, 0x00, 0x00, 0x04,	/* lg %r5, 0x00(%r1) */
  0xe3, 0x20, 0x10, 0x08, 0x00, 0x04,	/* lg %r2, 0x08(%r1) */
  0xe3, 0x40, 0x10, 0x10, 0x00, 0x04,	/* lg %r4, 0x10(%r1) */
  0xe3, 0x60, 0x10, 0x18, 0x00, 0x04,	/* lg %r6, 0x18(%r1) */
  /* Save original PSWA (tracepoint address).  */
  0xe3, 0x50, 0xf2, 0xc8, 0x00, 0x24,	/* stg %r5, 0x2c8(%r15) */
  /* Construct a collecting_t object at %r15+0x2e0.  */
  0xe3, 0x20, 0xf2, 0xe0, 0x00, 0x24,	/* stg %r2, 0x2e0(%r15) */
  0x9b, 0x01, 0xf2, 0xe8,		/* stam %a0, %a1, 0x2e8(%r15) */
  /* Move its address to %r0.  */
  0x41, 0x00, 0xf2, 0xe0,		/* la %r0, 0x2e0(%r15) */
  /* Take the lock.  */
  /* .Lloop:  */
  0xa7, 0x19, 0x00, 0x00,		/* lghi %r1, 0 */
  0xeb, 0x10, 0x60, 0x00, 0x00, 0x30,	/* csg %r1, %r0, 0(%r6) */
  0xa7, 0x74, 0xff, 0xfb,		/* jne .Lloop */
  /* Address of the register save block to %r3.  */
  0xb9, 0x04, 0x00, 0x3f,		/* lgr %r3, %r15 */
  /* Make a stack frame, so that we can call the collector.  */
  0xa7, 0xfb, 0xff, 0x60,		/* aghi %r15, -0xa0 */
  /* Call it.  */
  0x0d, 0xe4,				/* basr %r14, %r4 */
  /* And get rid of the stack frame again.  */
  0x41, 0xf0, 0xf0, 0xa0,		/* la %r15, 0xa0(%r15) */
  /* Leave the lock.  */
  0x07, 0xf0,				/* br %r0 */
  0xa7, 0x19, 0x00, 0x00,		/* lghi %r1, 0 */
  0xe3, 0x10, 0x60, 0x00, 0x00, 0x24,	/* stg %t1, 0(%r6) */
};

/* Code sequence restoring FRs, for targets with no VX support.  */

static const unsigned char s390_ft_exit_fr[] = {
  0x68, 0x00, 0xf0, 0x00,		/* ld %f0, 0x000(%r15) */
  0x68, 0x10, 0xf0, 0x10,		/* ld %f1, 0x010(%r15) */
  0x68, 0x20, 0xf0, 0x20,		/* ld %f2, 0x020(%r15) */
  0x68, 0x30, 0xf0, 0x30,		/* ld %f3, 0x030(%r15) */
  0x68, 0x40, 0xf0, 0x40,		/* ld %f4, 0x040(%r15) */
  0x68, 0x50, 0xf0, 0x50,		/* ld %f5, 0x050(%r15) */
  0x68, 0x60, 0xf0, 0x60,		/* ld %f6, 0x060(%r15) */
  0x68, 0x70, 0xf0, 0x70,		/* ld %f7, 0x070(%r15) */
  0x68, 0x80, 0xf0, 0x80,		/* ld %f8, 0x080(%r15) */
  0x68, 0x90, 0xf0, 0x90,		/* ld %f9, 0x090(%r15) */
  0x68, 0xa0, 0xf0, 0xa0,		/* ld %f10, 0x0a0(%r15) */
  0x68, 0xb0, 0xf0, 0xb0,		/* ld %f11, 0x0b0(%r15) */
  0x68, 0xc0, 0xf0, 0xc0,		/* ld %f12, 0x0c0(%r15) */
  0x68, 0xd0, 0xf0, 0xd0,		/* ld %f13, 0x0d0(%r15) */
  0x68, 0xe0, 0xf0, 0xe0,		/* ld %f14, 0x0e0(%r15) */
  0x68, 0xf0, 0xf0, 0xf0,		/* ld %f15, 0x0f0(%r15) */
};

/* Code sequence restoring VRs.  */

static const unsigned char s390_ft_exit_vr[] = {
  0xe7, 0x0f, 0xf0, 0x00, 0x00, 0x36,	/* vlm %v0, %v15, 0x000(%r15) */
  0xe7, 0x0f, 0xf1, 0x00, 0x0c, 0x36,	/* vlm %v16, %v31, 0x100(%r15) */
};

/* Code sequence restoring misc registers.  As for PSWM, only CC should be
   modified by C code, so we use the alr instruction to restore it by
   manufacturing an operand that'll result in the original flags.  */

static const unsigned char s390_ft_exit_misc[] = {
  0xb2, 0x9d, 0xf2, 0xd0,		/* lfpc 0x2d0(%r15) */
  0x58, 0x00, 0xf2, 0xc0,		/* l %r0, 0x2c0(%r15) */
  /* Extract CC to high 2 bits of %r0.  */
  0x88, 0x00, 0x00, 0x0c,		/* srl %r0, 12 */
  0x89, 0x00, 0x00, 0x1e,		/* sll %r0, 30 */
  /* Add %r0 to itself.  Result will be nonzero iff CC bit 0 is set, and
     will have carry iff CC bit 1 is set - resulting in the same flags
     as the original.  */
  0x1e, 0x00,				/* alr %r0, %r0 */
  0x9a, 0x0f, 0xf2, 0x80,		/* lam %a0, %a15, 0x280(%r15) */
};

/* Code sequence restoring GPRs, for 31-bit targets with no high GPRs.  */

static const unsigned char s390_ft_exit_gpr_esa[] = {
  0x58, 0x00, 0xf2, 0x04,		/* l %r0, 0x204(%r15) */
  0x58, 0x10, 0xf2, 0x0c,		/* l %r1, 0x20c(%r15) */
  0x58, 0x20, 0xf2, 0x14,		/* l %r2, 0x214(%r15) */
  0x58, 0x30, 0xf2, 0x1c,		/* l %r3, 0x21c(%r15) */
  0x58, 0x40, 0xf2, 0x24,		/* l %r4, 0x224(%r15) */
  0x58, 0x50, 0xf2, 0x2c,		/* l %r5, 0x22c(%r15) */
  0x58, 0x60, 0xf2, 0x34,		/* l %r6, 0x234(%r15) */
  0x58, 0x70, 0xf2, 0x3c,		/* l %r7, 0x23c(%r15) */
  0x58, 0x80, 0xf2, 0x44,		/* l %r8, 0x244(%r15) */
  0x58, 0x90, 0xf2, 0x4c,		/* l %r9, 0x24c(%r15) */
  0x58, 0xa0, 0xf2, 0x54,		/* l %r10, 0x254(%r15) */
  0x58, 0xb0, 0xf2, 0x5c,		/* l %r11, 0x25c(%r15) */
  0x58, 0xc0, 0xf2, 0x64,		/* l %r12, 0x264(%r15) */
  0x58, 0xd0, 0xf2, 0x6c,		/* l %r13, 0x26c(%r15) */
  0x58, 0xe0, 0xf2, 0x74,		/* l %r14, 0x274(%r15) */
  0x58, 0xf0, 0xf2, 0x7c,		/* l %r15, 0x27c(%r15) */
};

/* Code sequence restoring GPRs, for 64-bit targets and 31-bit targets
   with high GPRs.  */

static const unsigned char s390_ft_exit_gpr_zarch[] = {
  0xeb, 0x0f, 0xf2, 0x00, 0x00, 0x04,	/* lmg %r0, %r15, 0x200(%r15) */
};

/* Writes instructions to target, updating the to pointer.  */

static void
append_insns (CORE_ADDR *to, size_t len, const unsigned char *buf)
{
  write_inferior_memory (*to, buf, len);
  *to += len;
}

/* Relocates an instruction from oldloc to *to, updating to.  */

static int
s390_relocate_instruction (CORE_ADDR *to, CORE_ADDR oldloc, int is_64)
{
  gdb_byte buf[6];
  int ilen;
  int op2;
  /* 0: no fixup, 1: PC16DBL fixup, 2: PC32DBL fixup.  */
  int mode = 0;
  int is_bras = 0;
  read_inferior_memory (oldloc, buf, sizeof buf);
  if (buf[0] < 0x40)
    ilen = 2;
  else if (buf[0] < 0xc0)
    ilen = 4;
  else
    ilen = 6;
  switch (buf[0])
    {
    case 0x05: /* BALR */
    case 0x0c: /* BASSM */
    case 0x0d: /* BASR */
    case 0x45: /* BAL */
    case 0x4d: /* BAS */
      /* These save a return address and mess around with registers.
	 We can't relocate them.  */
      return 1;
    case 0x84: /* BRXH */
    case 0x85: /* BRXLE */
      mode = 1;
      break;
    case 0xa7:
      op2 = buf[1] & 0xf;
      /* BRC, BRAS, BRCT, BRCTG */
      if (op2 >= 4 && op2 <= 7)
	mode = 1;
      /* BRAS */
      if (op2 == 5)
	is_bras = 1;
      break;
    case 0xc0:
      op2 = buf[1] & 0xf;
      /* LARL, BRCL, BRASL */
      if (op2 == 0 || op2 == 4 || op2 == 5)
	mode = 2;
      /* BRASL */
      if (op2 == 5)
	is_bras = 1;
      break;
    case 0xc4:
    case 0xc6:
      /* PC-relative addressing instructions.  */
      mode = 2;
      break;
    case 0xc5: /* BPRP */
    case 0xc7: /* BPP */
      /* Branch prediction - just skip it.  */
      return 0;
    case 0xcc:
      op2 = buf[1] & 0xf;
      /* BRCTH */
      if (op2 == 6)
	mode = 2;
      break;
    case 0xec:
      op2 = buf[5];
      switch (op2)
	{
	case 0x44: /* BRXHG */
	case 0x45: /* BRXLG */
	case 0x64: /* CGRJ */
	case 0x65: /* CLGRJ */
	case 0x76: /* CRJ */
	case 0x77: /* CLRJ */
	  mode = 1;
	  break;
	}
      break;
    }

  if (mode != 0)
    {
      /* We'll have to relocate an instruction with a PC-relative field.
	 First, compute the target.  */
      int64_t loffset = 0;
      CORE_ADDR target;
      if (mode == 1)
	{
	  int16_t soffset = 0;
	  memcpy (&soffset, buf + 2, 2);
	  loffset = soffset;
	}
      else if (mode == 2)
	{
	  int32_t soffset = 0;
	  memcpy (&soffset, buf + 2, 4);
	  loffset = soffset;
	}
      target = oldloc + loffset * 2;
      if (!is_64)
	target &= 0x7fffffff;

      if (is_bras)
	{
	  /* BRAS or BRASL was used.  We cannot just relocate those, since
	     they save the return address in a register.  We can, however,
	     replace them with a LARL+JG sequence.  */

	  /* Make the LARL.  */
	  int32_t soffset;
	  buf[0] = 0xc0;
	  buf[1] &= 0xf0;
	  loffset = oldloc + ilen - *to;
	  loffset >>= 1;
	  soffset = loffset;
	  if (soffset != loffset && is_64)
	    return 1;
	  memcpy (buf + 2, &soffset, 4);
	  append_insns (to, 6, buf);

	  /* Note: this is not fully correct.  In 31-bit mode, LARL will write
	     an address with the top bit 0, while BRAS/BRASL will write it
	     with top bit 1.  It should not matter much, since linux compilers
	     use BR and not BSM to return from functions, but it could confuse
	     some poor stack unwinder.  */

	  /* We'll now be writing a JG.  */
	  mode = 2;
	  buf[0] = 0xc0;
	  buf[1] = 0xf4;
	  ilen = 6;
	}

      /* Compute the new offset and write it to the buffer.  */
      loffset = target - *to;
      loffset >>= 1;

      if (mode == 1)
	{
	  int16_t soffset = loffset;
	  if (soffset != loffset)
	    return 1;
	  memcpy (buf + 2, &soffset, 2);
	}
      else if (mode == 2)
	{
	  int32_t soffset = loffset;
	  if (soffset != loffset && is_64)
	    return 1;
	  memcpy (buf + 2, &soffset, 4);
	}
    }
  append_insns (to, ilen, buf);
  return 0;
}

/* Implementation of linux_target_ops method
   "install_fast_tracepoint_jump_pad".  */

static int
s390_install_fast_tracepoint_jump_pad (CORE_ADDR tpoint,
				       CORE_ADDR tpaddr,
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
  int i;
  int64_t loffset;
  int32_t offset;
  unsigned char jbuf[6] = { 0xc0, 0xf4, 0, 0, 0, 0 };	/* jg ... */
  CORE_ADDR buildaddr = *jump_entry;
#ifdef __s390x__
  struct regcache *regcache = get_thread_regcache (current_thread, 0);
  int is_64 = register_size (regcache->tdesc, 0) == 8;
  int is_zarch = is_64 || have_hwcap_s390_high_gprs;
  int has_vx = have_hwcap_s390_vx;
#else
  int is_64 = 0, is_zarch = 0, has_vx = 0;
#endif
  CORE_ADDR literals[4] = {
    tpaddr,
    tpoint,
    collector,
    lockaddr,
  };

  /* First, store the GPRs.  */
  if (is_zarch)
    append_insns (&buildaddr, sizeof s390_ft_entry_gpr_zarch,
			      s390_ft_entry_gpr_zarch);
  else
    append_insns (&buildaddr, sizeof s390_ft_entry_gpr_esa,
			      s390_ft_entry_gpr_esa);

  /* Second, misc registers (ARs, PSWM, FPC).  PSWA will be stored below.  */
  append_insns (&buildaddr, sizeof s390_ft_entry_misc, s390_ft_entry_misc);

  /* Third, FRs or VRs.  */
  if (has_vx)
    append_insns (&buildaddr, sizeof s390_ft_entry_vr, s390_ft_entry_vr);
  else
    append_insns (&buildaddr, sizeof s390_ft_entry_fr, s390_ft_entry_fr);

  /* Now, the main part of code - store PSWA, take lock, call collector,
     leave lock.  First, we'll need to fetch 4 literals.  */
  if (is_64) {
    unsigned char buf[] = {
      0x07, 0x07,		/* nopr %r7 */
      0x07, 0x07,		/* nopr %r7 */
      0x07, 0x07,		/* nopr %r7 */
      0xa7, 0x15, 0x00, 0x12,	/* bras %r1, .Lend */
      0, 0, 0, 0, 0, 0, 0, 0,	/* tpaddr */
      0, 0, 0, 0, 0, 0, 0, 0,	/* tpoint */
      0, 0, 0, 0, 0, 0, 0, 0,	/* collector */
      0, 0, 0, 0, 0, 0, 0, 0,	/* lockaddr */
      /* .Lend: */
    };
    /* Find the proper start place in buf, so that literals will be
       aligned. */
    int bufpos = (buildaddr + 2) & 7;
    /* Stuff the literals into the buffer. */
    for (i = 0; i < 4; i++) {
      uint64_t lit = literals[i];
      memcpy (&buf[sizeof buf - 32 + i * 8], &lit, 8);
    }
    append_insns (&buildaddr, sizeof buf - bufpos, buf + bufpos);
    append_insns (&buildaddr, sizeof s390_ft_main_64, s390_ft_main_64);
  } else {
    unsigned char buf[] = {
      0x07, 0x07,		/* nopr %r7 */
      0xa7, 0x15, 0x00, 0x0a,	/* bras %r1, .Lend */
      0, 0, 0, 0,		/* tpaddr */
      0, 0, 0, 0,		/* tpoint */
      0, 0, 0, 0,		/* collector */
      0, 0, 0, 0,		/* lockaddr */
      /* .Lend: */
    };
    /* Find the proper start place in buf, so that literals will be
       aligned. */
    int bufpos = (buildaddr + 2) & 3;
    /* First literal will be saved as the PSWA, make sure it has the high bit
       set.  */
    literals[0] |= 0x80000000;
    /* Stuff the literals into the buffer. */
    for (i = 0; i < 4; i++) {
      uint32_t lit = literals[i];
      memcpy (&buf[sizeof buf - 16 + i * 4], &lit, 4);
    }
    append_insns (&buildaddr, sizeof buf - bufpos, buf + bufpos);
    append_insns (&buildaddr, sizeof s390_ft_main_31, s390_ft_main_31);
  }

  /* Restore FRs or VRs.  */
  if (has_vx)
    append_insns (&buildaddr, sizeof s390_ft_exit_vr, s390_ft_exit_vr);
  else
    append_insns (&buildaddr, sizeof s390_ft_exit_fr, s390_ft_exit_fr);

  /* Restore misc registers.  */
  append_insns (&buildaddr, sizeof s390_ft_exit_misc, s390_ft_exit_misc);

  /* Restore the GPRs.  */
  if (is_zarch)
    append_insns (&buildaddr, sizeof s390_ft_exit_gpr_zarch,
			      s390_ft_exit_gpr_zarch);
  else
    append_insns (&buildaddr, sizeof s390_ft_exit_gpr_esa,
			      s390_ft_exit_gpr_esa);

  /* Now, adjust the original instruction to execute in the jump
     pad.  */
  *adjusted_insn_addr = buildaddr;
  if (s390_relocate_instruction (&buildaddr, tpaddr, is_64))
    {
      sprintf (err, "E.Could not relocate instruction for tracepoint.");
      return 1;
    }
  *adjusted_insn_addr_end = buildaddr;

  /* Finally, write a jump back to the program.  */

  loffset = (tpaddr + orig_size) - buildaddr;
  loffset >>= 1;
  offset = loffset;
  if (is_64 && offset != loffset)
    {
      sprintf (err,
	       "E.Jump back from jump pad too far from tracepoint "
	       "(offset 0x%" PRIx64 " > int33).", loffset);
      return 1;
    }
  memcpy (jbuf + 2, &offset, 4);
  append_insns (&buildaddr, sizeof jbuf, jbuf);

  /* The jump pad is now built.  Wire in a jump to our jump pad.  This
     is always done last (by our caller actually), so that we can
     install fast tracepoints with threads running.  This relies on
     the agent's atomic write support.  */
  loffset = *jump_entry - tpaddr;
  loffset >>= 1;
  offset = loffset;
  if (is_64 && offset != loffset)
    {
      sprintf (err,
	       "E.Jump back from jump pad too far from tracepoint "
	       "(offset 0x%" PRIx64 " > int33).", loffset);
      return 1;
    }
  memcpy (jbuf + 2, &offset, 4);
  memcpy (jjump_pad_insn, jbuf, sizeof jbuf);
  *jjump_pad_insn_size = sizeof jbuf;

  /* Return the end address of our pad.  */
  *jump_entry = buildaddr;

  return 0;
}

/* Implementation of linux_target_ops method
   "get_min_fast_tracepoint_insn_len".  */

static int
s390_get_min_fast_tracepoint_insn_len (void)
{
  /* We only support using 6-byte jumps to reach the tracepoint code.
     If the tracepoint buffer were allocated sufficiently close (64kiB)
     to the executable code, and the traced instruction itself was close
     enough to the beginning, we could use 4-byte jumps, but this doesn't
     seem to be worth the effort.  */
  return 6;
}

/* Implementation of linux_target_ops method "get_ipa_tdesc_idx".  */

static int
s390_get_ipa_tdesc_idx (void)
{
  struct regcache *regcache = get_thread_regcache (current_thread, 0);
  const struct target_desc *tdesc = regcache->tdesc;

#ifdef __s390x__
  if (tdesc == tdesc_s390x_linux64)
    return S390_TDESC_64;
  if (tdesc == tdesc_s390x_linux64v1)
    return S390_TDESC_64V1;
  if (tdesc == tdesc_s390x_linux64v2)
    return S390_TDESC_64V2;
  if (tdesc == tdesc_s390x_te_linux64)
    return S390_TDESC_TE;
  if (tdesc == tdesc_s390x_vx_linux64)
    return S390_TDESC_VX;
  if (tdesc == tdesc_s390x_tevx_linux64)
    return S390_TDESC_TEVX;
#endif

  if (tdesc == tdesc_s390_linux32)
    return S390_TDESC_32;
  if (tdesc == tdesc_s390_linux32v1)
    return S390_TDESC_32V1;
  if (tdesc == tdesc_s390_linux32v2)
    return S390_TDESC_32V2;
  if (tdesc == tdesc_s390_linux64)
    return S390_TDESC_64;
  if (tdesc == tdesc_s390_linux64v1)
    return S390_TDESC_64V1;
  if (tdesc == tdesc_s390_linux64v2)
    return S390_TDESC_64V2;
  if (tdesc == tdesc_s390_te_linux64)
    return S390_TDESC_TE;
  if (tdesc == tdesc_s390_vx_linux64)
    return S390_TDESC_VX;
  if (tdesc == tdesc_s390_tevx_linux64)
    return S390_TDESC_TEVX;

  return 0;
}

struct linux_target_ops the_low_target = {
  s390_arch_setup,
  s390_regs_info,
  s390_cannot_fetch_register,
  s390_cannot_store_register,
  NULL, /* fetch_register */
  s390_get_pc,
  s390_set_pc,
  NULL, /* breakpoint_kind_from_pc */
  s390_sw_breakpoint_from_kind,
  NULL,
  s390_breakpoint_len,
  s390_breakpoint_at,
  s390_supports_z_point_type,
  NULL,
  NULL,
  NULL,
  NULL,
  s390_collect_ptrace_register,
  s390_supply_ptrace_register,
  NULL, /* siginfo_fixup */
  NULL, /* new_process */
  NULL, /* new_thread */
  NULL, /* new_fork */
  NULL, /* prepare_to_resume */
  NULL, /* process_qsupported */
  s390_supports_tracepoints,
  s390_get_thread_area,
  s390_install_fast_tracepoint_jump_pad,
  NULL, /* emit_ops */
  s390_get_min_fast_tracepoint_insn_len,
  NULL, /* supports_range_stepping */
  NULL, /* breakpoint_kind_from_current_state */
  s390_supports_hardware_single_step,
  NULL, /* get_syscall_trapinfo */
  s390_get_ipa_tdesc_idx,
};

void
initialize_low_arch (void)
{
  /* Initialize the Linux target descriptions.  */

  init_registers_s390_linux32 ();
  init_registers_s390_linux32v1 ();
  init_registers_s390_linux32v2 ();
  init_registers_s390_linux64 ();
  init_registers_s390_linux64v1 ();
  init_registers_s390_linux64v2 ();
  init_registers_s390_te_linux64 ();
  init_registers_s390_vx_linux64 ();
  init_registers_s390_tevx_linux64 ();
#ifdef __s390x__
  init_registers_s390x_linux64 ();
  init_registers_s390x_linux64v1 ();
  init_registers_s390x_linux64v2 ();
  init_registers_s390x_te_linux64 ();
  init_registers_s390x_vx_linux64 ();
  init_registers_s390x_tevx_linux64 ();
#endif

  initialize_regsets_info (&s390_regsets_info);
  initialize_regsets_info (&s390_regsets_info_3264);
}
