/* Target-dependent code for GNU/Linux on MIPS processors.

   Copyright 2001, 2002 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "gdbcore.h"
#include "target.h"
#include "solib-svr4.h"
#include "osabi.h"
#include "mips-tdep.h"
#include "gdb_string.h"
#include "gdb_assert.h"

/* Copied from <asm/elf.h>.  */
#define ELF_NGREG       45
#define ELF_NFPREG      33

typedef unsigned char elf_greg_t[4];
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef unsigned char elf_fpreg_t[8];
typedef elf_fpreg_t elf_fpregset_t[ELF_NFPREG];

/* 0 - 31 are integer registers, 32 - 63 are fp registers.  */
#define FPR_BASE        32
#define PC              64
#define CAUSE           65
#define BADVADDR        66
#define MMHI            67
#define MMLO            68
#define FPC_CSR         69
#define FPC_EIR         70

#define EF_REG0			6
#define EF_REG31		37
#define EF_LO			38
#define EF_HI			39
#define EF_CP0_EPC		40
#define EF_CP0_BADVADDR		41
#define EF_CP0_STATUS		42
#define EF_CP0_CAUSE		43

#define EF_SIZE			180

/* Figure out where the longjmp will land.
   We expect the first arg to be a pointer to the jmp_buf structure from
   which we extract the pc (MIPS_LINUX_JB_PC) that we will land at.  The pc
   is copied into PC.  This routine returns 1 on success.  */

#define MIPS_LINUX_JB_ELEMENT_SIZE 4
#define MIPS_LINUX_JB_PC 0

static int
mips_linux_get_longjmp_target (CORE_ADDR *pc)
{
  CORE_ADDR jb_addr;
  char buf[TARGET_PTR_BIT / TARGET_CHAR_BIT];

  jb_addr = read_register (A0_REGNUM);

  if (target_read_memory (jb_addr
			  + MIPS_LINUX_JB_PC * MIPS_LINUX_JB_ELEMENT_SIZE,
			  buf, TARGET_PTR_BIT / TARGET_CHAR_BIT))
    return 0;

  *pc = extract_address (buf, TARGET_PTR_BIT / TARGET_CHAR_BIT);

  return 1;
}

/* Transform the bits comprising a 32-bit register to the right
   size for supply_register().  This is needed when MIPS_REGSIZE is 8.  */

static void
supply_32bit_reg (int regnum, const void *addr)
{
  char *buf = alloca (MAX_REGISTER_RAW_SIZE);
  store_signed_integer (buf, REGISTER_RAW_SIZE (regnum),
                        extract_signed_integer (addr, 4));
  supply_register (regnum, buf);
}

/* Unpack an elf_gregset_t into GDB's register cache.  */

void 
supply_gregset (elf_gregset_t *gregsetp)
{
  int regi;
  elf_greg_t *regp = *gregsetp;
  char *zerobuf = alloca (MAX_REGISTER_RAW_SIZE);

  memset (zerobuf, 0, MAX_REGISTER_RAW_SIZE);

  for (regi = EF_REG0; regi <= EF_REG31; regi++)
    supply_32bit_reg ((regi - EF_REG0), (char *)(regp + regi));

  supply_32bit_reg (LO_REGNUM, (char *)(regp + EF_LO));
  supply_32bit_reg (HI_REGNUM, (char *)(regp + EF_HI));

  supply_32bit_reg (PC_REGNUM, (char *)(regp + EF_CP0_EPC));
  supply_32bit_reg (BADVADDR_REGNUM, (char *)(regp + EF_CP0_BADVADDR));
  supply_32bit_reg (PS_REGNUM, (char *)(regp + EF_CP0_STATUS));
  supply_32bit_reg (CAUSE_REGNUM, (char *)(regp + EF_CP0_CAUSE));

  /* Fill inaccessible registers with zero.  */
  supply_register (UNUSED_REGNUM, zerobuf);
  for (regi = FIRST_EMBED_REGNUM; regi < LAST_EMBED_REGNUM; regi++)
    supply_register (regi, zerobuf);
}

/* Pack our registers (or one register) into an elf_gregset_t.  */

void
fill_gregset (elf_gregset_t *gregsetp, int regno)
{
  int regaddr, regi;
  elf_greg_t *regp = *gregsetp;
  void *dst;

  if (regno == -1)
    {
      memset (regp, 0, sizeof (elf_gregset_t));
      for (regi = 0; regi < 32; regi++)
        fill_gregset (gregsetp, regi);
      fill_gregset (gregsetp, LO_REGNUM);
      fill_gregset (gregsetp, HI_REGNUM);
      fill_gregset (gregsetp, PC_REGNUM);
      fill_gregset (gregsetp, BADVADDR_REGNUM);
      fill_gregset (gregsetp, PS_REGNUM);
      fill_gregset (gregsetp, CAUSE_REGNUM);

      return;
   }

  if (regno < 32)
    {
      dst = regp + regno + EF_REG0;
      regcache_collect (regno, dst);
      return;
    }

  regaddr = -1;
  switch (regno)
    {
      case LO_REGNUM:
	regaddr = EF_LO;
	break;
      case HI_REGNUM:
	regaddr = EF_HI;
	break;
      case PC_REGNUM:
	regaddr = EF_CP0_EPC;
	break;
      case BADVADDR_REGNUM:
	regaddr = EF_CP0_BADVADDR;
	break;
      case PS_REGNUM:
	regaddr = EF_CP0_STATUS;
	break;
      case CAUSE_REGNUM:
	regaddr = EF_CP0_CAUSE;
	break;
    }

  if (regaddr != -1)
    {
      dst = regp + regaddr;
      regcache_collect (regno, dst);
    }
}

/* Likewise, unpack an elf_fpregset_t.  */

void
supply_fpregset (elf_fpregset_t *fpregsetp)
{
  register int regi;
  char *zerobuf = alloca (MAX_REGISTER_RAW_SIZE);

  memset (zerobuf, 0, MAX_REGISTER_RAW_SIZE);

  for (regi = 0; regi < 32; regi++)
    supply_register (FP0_REGNUM + regi,
		     (char *)(*fpregsetp + regi));

  supply_register (FCRCS_REGNUM, (char *)(*fpregsetp + 32));

  /* FIXME: how can we supply FCRIR_REGNUM?  The ABI doesn't tell us. */
  supply_register (FCRIR_REGNUM, zerobuf);
}

/* Likewise, pack one or all floating point registers into an
   elf_fpregset_t.  */

void
fill_fpregset (elf_fpregset_t *fpregsetp, int regno)
{
  char *from, *to;

  if ((regno >= FP0_REGNUM) && (regno < FP0_REGNUM + 32))
    {
      from = (char *) &deprecated_registers[REGISTER_BYTE (regno)];
      to = (char *) (*fpregsetp + regno - FP0_REGNUM);
      memcpy (to, from, REGISTER_RAW_SIZE (regno - FP0_REGNUM));
    }
  else if (regno == FCRCS_REGNUM)
    {
      from = (char *) &deprecated_registers[REGISTER_BYTE (regno)];
      to = (char *) (*fpregsetp + 32);
      memcpy (to, from, REGISTER_RAW_SIZE (regno));
    }
  else if (regno == -1)
    {
      int regi;

      for (regi = 0; regi < 32; regi++)
	fill_fpregset (fpregsetp, FP0_REGNUM + regi);
      fill_fpregset(fpregsetp, FCRCS_REGNUM);
    }
}

/* Map gdb internal register number to ptrace ``address''.
   These ``addresses'' are normally defined in <asm/ptrace.h>.  */

static CORE_ADDR
mips_linux_register_addr (int regno, CORE_ADDR blockend)
{
  int regaddr;

  if (regno < 0 || regno >= NUM_REGS)
    error ("Bogon register number %d.", regno);

  if (regno < 32)
    regaddr = regno;
  else if ((regno >= FP0_REGNUM) && (regno < FP0_REGNUM + 32))
    regaddr = FPR_BASE + (regno - FP0_REGNUM);
  else if (regno == PC_REGNUM)
    regaddr = PC;
  else if (regno == CAUSE_REGNUM)
    regaddr = CAUSE;
  else if (regno == BADVADDR_REGNUM)
    regaddr = BADVADDR;
  else if (regno == LO_REGNUM)
    regaddr = MMLO;
  else if (regno == HI_REGNUM)
    regaddr = MMHI;
  else if (regno == FCRCS_REGNUM)
    regaddr = FPC_CSR;
  else if (regno == FCRIR_REGNUM)
    regaddr = FPC_EIR;
  else
    error ("Unknowable register number %d.", regno);

  return regaddr;
}


/* Fetch (and possibly build) an appropriate link_map_offsets
   structure for native GNU/Linux MIPS targets using the struct offsets
   defined in link.h (but without actual reference to that file).

   This makes it possible to access GNU/Linux MIPS shared libraries from a
   GDB that was built on a different host platform (for cross debugging).  */

static struct link_map_offsets *
mips_linux_svr4_fetch_link_map_offsets (void)
{ 
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = NULL;

  if (lmp == NULL)
    { 
      lmp = &lmo;

      lmo.r_debug_size = 8;	/* The actual size is 20 bytes, but
				   this is all we need.  */
      lmo.r_map_offset = 4;
      lmo.r_map_size   = 4;

      lmo.link_map_size = 20;

      lmo.l_addr_offset = 0;
      lmo.l_addr_size   = 4;

      lmo.l_name_offset = 4;
      lmo.l_name_size   = 4;

      lmo.l_next_offset = 12;
      lmo.l_next_size   = 4;

      lmo.l_prev_offset = 16;
      lmo.l_prev_size   = 4;
    }

  return lmp;
}

/* Support for 64-bit ABIs.  */

/* Copied from <asm/elf.h>.  */
#define MIPS64_ELF_NGREG       45
#define MIPS64_ELF_NFPREG      33

typedef unsigned char mips64_elf_greg_t[8];
typedef mips64_elf_greg_t mips64_elf_gregset_t[MIPS64_ELF_NGREG];

typedef unsigned char mips64_elf_fpreg_t[8];
typedef mips64_elf_fpreg_t mips64_elf_fpregset_t[MIPS64_ELF_NFPREG];

/* 0 - 31 are integer registers, 32 - 63 are fp registers.  */
#define MIPS64_FPR_BASE                 32
#define MIPS64_PC                       64
#define MIPS64_CAUSE                    65
#define MIPS64_BADVADDR                 66
#define MIPS64_MMHI                     67
#define MIPS64_MMLO                     68
#define MIPS64_FPC_CSR                  69
#define MIPS64_FPC_EIR                  70

#define MIPS64_EF_REG0			 0
#define MIPS64_EF_REG31			31
#define MIPS64_EF_LO			32
#define MIPS64_EF_HI			33
#define MIPS64_EF_CP0_EPC		34
#define MIPS64_EF_CP0_BADVADDR		35
#define MIPS64_EF_CP0_STATUS		36
#define MIPS64_EF_CP0_CAUSE		37

#define MIPS64_EF_SIZE			304

/* Figure out where the longjmp will land.
   We expect the first arg to be a pointer to the jmp_buf structure from
   which we extract the pc (MIPS_LINUX_JB_PC) that we will land at.  The pc
   is copied into PC.  This routine returns 1 on success.  */

/* Details about jmp_buf.  */

#define MIPS64_LINUX_JB_PC 0

static int
mips64_linux_get_longjmp_target (CORE_ADDR *pc)
{
  CORE_ADDR jb_addr;
  void *buf = alloca (TARGET_PTR_BIT / TARGET_CHAR_BIT);
  int element_size = TARGET_PTR_BIT == 32 ? 4 : 8;

  jb_addr = read_register (A0_REGNUM);

  if (target_read_memory (jb_addr + MIPS64_LINUX_JB_PC * element_size,
			  buf, TARGET_PTR_BIT / TARGET_CHAR_BIT))
    return 0;

  *pc = extract_address (buf, TARGET_PTR_BIT / TARGET_CHAR_BIT);

  return 1;
}

/* Unpack an elf_gregset_t into GDB's register cache.  */

static void 
mips64_supply_gregset (mips64_elf_gregset_t *gregsetp)
{
  int regi;
  mips64_elf_greg_t *regp = *gregsetp;
  char *zerobuf = alloca (MAX_REGISTER_RAW_SIZE);

  memset (zerobuf, 0, MAX_REGISTER_RAW_SIZE);

  for (regi = MIPS64_EF_REG0; regi <= MIPS64_EF_REG31; regi++)
    supply_register ((regi - MIPS64_EF_REG0), (char *)(regp + regi));

  supply_register (LO_REGNUM, (char *)(regp + MIPS64_EF_LO));
  supply_register (HI_REGNUM, (char *)(regp + MIPS64_EF_HI));

  supply_register (PC_REGNUM, (char *)(regp + MIPS64_EF_CP0_EPC));
  supply_register (BADVADDR_REGNUM, (char *)(regp + MIPS64_EF_CP0_BADVADDR));
  supply_register (PS_REGNUM, (char *)(regp + MIPS64_EF_CP0_STATUS));
  supply_register (CAUSE_REGNUM, (char *)(regp + MIPS64_EF_CP0_CAUSE));

  /* Fill inaccessible registers with zero.  */
  supply_register (UNUSED_REGNUM, zerobuf);
  for (regi = FIRST_EMBED_REGNUM; regi < LAST_EMBED_REGNUM; regi++)
    supply_register (regi, zerobuf);
}

/* Pack our registers (or one register) into an elf_gregset_t.  */

static void
mips64_fill_gregset (mips64_elf_gregset_t *gregsetp, int regno)
{
  int regaddr, regi;
  mips64_elf_greg_t *regp = *gregsetp;
  void *src, *dst;

  if (regno == -1)
    {
      memset (regp, 0, sizeof (mips64_elf_gregset_t));
      for (regi = 0; regi < 32; regi++)
        mips64_fill_gregset (gregsetp, regi);
      mips64_fill_gregset (gregsetp, LO_REGNUM);
      mips64_fill_gregset (gregsetp, HI_REGNUM);
      mips64_fill_gregset (gregsetp, PC_REGNUM);
      mips64_fill_gregset (gregsetp, BADVADDR_REGNUM);
      mips64_fill_gregset (gregsetp, PS_REGNUM);
      mips64_fill_gregset (gregsetp, CAUSE_REGNUM);

      return;
   }

  if (regno < 32)
    {
      dst = regp + regno + MIPS64_EF_REG0;
      regcache_collect (regno, dst);
      return;
    }

  regaddr = -1;
  switch (regno)
    {
      case LO_REGNUM:
	regaddr = MIPS64_EF_LO;
	break;
      case HI_REGNUM:
	regaddr = MIPS64_EF_HI;
	break;
      case PC_REGNUM:
	regaddr = MIPS64_EF_CP0_EPC;
	break;
      case BADVADDR_REGNUM:
	regaddr = MIPS64_EF_CP0_BADVADDR;
	break;
      case PS_REGNUM:
	regaddr = MIPS64_EF_CP0_STATUS;
	break;
      case CAUSE_REGNUM:
	regaddr = MIPS64_EF_CP0_CAUSE;
	break;
    }

  if (regaddr != -1)
    {
      dst = regp + regaddr;
      regcache_collect (regno, dst);
    }
}

/* Likewise, unpack an elf_fpregset_t.  */

static void
mips64_supply_fpregset (mips64_elf_fpregset_t *fpregsetp)
{
  register int regi;
  char *zerobuf = alloca (MAX_REGISTER_RAW_SIZE);

  memset (zerobuf, 0, MAX_REGISTER_RAW_SIZE);

  for (regi = 0; regi < 32; regi++)
    supply_register (FP0_REGNUM + regi,
		     (char *)(*fpregsetp + regi));

  supply_register (FCRCS_REGNUM, (char *)(*fpregsetp + 32));

  /* FIXME: how can we supply FCRIR_REGNUM?  The ABI doesn't tell us. */
  supply_register (FCRIR_REGNUM, zerobuf);
}

/* Likewise, pack one or all floating point registers into an
   elf_fpregset_t.  */

static void
mips64_fill_fpregset (mips64_elf_fpregset_t *fpregsetp, int regno)
{
  char *from, *to;

  if ((regno >= FP0_REGNUM) && (regno < FP0_REGNUM + 32))
    {
      from = (char *) &deprecated_registers[REGISTER_BYTE (regno)];
      to = (char *) (*fpregsetp + regno - FP0_REGNUM);
      memcpy (to, from, REGISTER_RAW_SIZE (regno - FP0_REGNUM));
    }
  else if (regno == FCRCS_REGNUM)
    {
      from = (char *) &deprecated_registers[REGISTER_BYTE (regno)];
      to = (char *) (*fpregsetp + 32);
      memcpy (to, from, REGISTER_RAW_SIZE (regno));
    }
  else if (regno == -1)
    {
      int regi;

      for (regi = 0; regi < 32; regi++)
	mips64_fill_fpregset (fpregsetp, FP0_REGNUM + regi);
      mips64_fill_fpregset(fpregsetp, FCRCS_REGNUM);
    }
}


/* Map gdb internal register number to ptrace ``address''.
   These ``addresses'' are normally defined in <asm/ptrace.h>.  */

static CORE_ADDR
mips64_linux_register_addr (int regno, CORE_ADDR blockend)
{
  int regaddr;

  if (regno < 0 || regno >= NUM_REGS)
    error ("Bogon register number %d.", regno);

  if (regno < 32)
    regaddr = regno;
  else if ((regno >= FP0_REGNUM) && (regno < FP0_REGNUM + 32))
    regaddr = MIPS64_FPR_BASE + (regno - FP0_REGNUM);
  else if (regno == PC_REGNUM)
    regaddr = MIPS64_PC;
  else if (regno == CAUSE_REGNUM)
    regaddr = MIPS64_CAUSE;
  else if (regno == BADVADDR_REGNUM)
    regaddr = MIPS64_BADVADDR;
  else if (regno == LO_REGNUM)
    regaddr = MIPS64_MMLO;
  else if (regno == HI_REGNUM)
    regaddr = MIPS64_MMHI;
  else if (regno == FCRCS_REGNUM)
    regaddr = MIPS64_FPC_CSR;
  else if (regno == FCRIR_REGNUM)
    regaddr = MIPS64_FPC_EIR;
  else
    error ("Unknowable register number %d.", regno);

  return regaddr;
}

/*  Use a local version of this function to get the correct types for
    regsets, until multi-arch core support is ready.  */

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
		      int which, CORE_ADDR reg_addr)
{
  elf_gregset_t gregset;
  elf_fpregset_t fpregset;
  mips64_elf_gregset_t gregset64;
  mips64_elf_fpregset_t fpregset64;

  if (which == 0)
    {
      if (core_reg_size == sizeof (gregset))
	{
	  memcpy ((char *) &gregset, core_reg_sect, sizeof (gregset));
	  supply_gregset (&gregset);
	}
      else if (core_reg_size == sizeof (gregset64))
	{
	  memcpy ((char *) &gregset64, core_reg_sect, sizeof (gregset64));
	  mips64_supply_gregset (&gregset64);
	}
      else
	{
	  warning ("wrong size gregset struct in core file");
	}
    }
  else if (which == 2)
    {
      if (core_reg_size == sizeof (fpregset))
	{
	  memcpy ((char *) &fpregset, core_reg_sect, sizeof (fpregset));
	  supply_fpregset (&fpregset);
	}
      else if (core_reg_size == sizeof (fpregset64))
	{
	  memcpy ((char *) &fpregset64, core_reg_sect, sizeof (fpregset64));
	  mips64_supply_fpregset (&fpregset64);
	}
      else
	{
	  warning ("wrong size fpregset struct in core file");
	}
    }
}

/* Register that we are able to handle ELF file formats using standard
   procfs "regset" structures.  */

static struct core_fns regset_core_fns =
{
  bfd_target_elf_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

/* Fetch (and possibly build) an appropriate link_map_offsets
   structure for native GNU/Linux MIPS targets using the struct offsets
   defined in link.h (but without actual reference to that file).

   This makes it possible to access GNU/Linux MIPS shared libraries from a
   GDB that was built on a different host platform (for cross debugging).  */

static struct link_map_offsets *
mips64_linux_svr4_fetch_link_map_offsets (void)
{ 
  static struct link_map_offsets lmo;
  static struct link_map_offsets *lmp = NULL;

  if (lmp == NULL)
    { 
      lmp = &lmo;

      lmo.r_debug_size = 16;	/* The actual size is 40 bytes, but
				   this is all we need.  */
      lmo.r_map_offset = 8;
      lmo.r_map_size   = 8;

      lmo.link_map_size = 40;

      lmo.l_addr_offset = 0;
      lmo.l_addr_size   = 8;

      lmo.l_name_offset = 8;
      lmo.l_name_size   = 8;

      lmo.l_next_offset = 24;
      lmo.l_next_size   = 8;

      lmo.l_prev_offset = 32;
      lmo.l_prev_size   = 8;
    }

  return lmp;
}

/* Handle for obtaining pointer to the current register_addr() function
   for a given architecture.  */
static struct gdbarch_data *register_addr_data;

CORE_ADDR
register_addr (int regno, CORE_ADDR blockend)
{
  CORE_ADDR (*register_addr_ptr) (int, CORE_ADDR) =
    gdbarch_data (current_gdbarch, register_addr_data);

  gdb_assert (register_addr_ptr != 0);

  return register_addr_ptr (regno, blockend);
}

static void
set_mips_linux_register_addr (struct gdbarch *gdbarch,
                              CORE_ADDR (*register_addr_ptr) (int, CORE_ADDR))
{
  set_gdbarch_data (gdbarch, register_addr_data, register_addr_ptr);
}

static void *
init_register_addr_data (struct gdbarch *gdbarch)
{
  return 0;
}

static void
mips_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  enum mips_abi abi = mips_abi (gdbarch);

  switch (abi)
    {
      case MIPS_ABI_O32:
	set_gdbarch_get_longjmp_target (gdbarch,
	                                mips_linux_get_longjmp_target);
	set_solib_svr4_fetch_link_map_offsets
	  (gdbarch, mips_linux_svr4_fetch_link_map_offsets);
	set_mips_linux_register_addr (gdbarch, mips_linux_register_addr);
	break;
      case MIPS_ABI_N32:
	set_gdbarch_get_longjmp_target (gdbarch,
	                                mips_linux_get_longjmp_target);
	set_solib_svr4_fetch_link_map_offsets
	  (gdbarch, mips_linux_svr4_fetch_link_map_offsets);
	set_mips_linux_register_addr (gdbarch, mips64_linux_register_addr);
	break;
      case MIPS_ABI_N64:
	set_gdbarch_get_longjmp_target (gdbarch,
	                                mips64_linux_get_longjmp_target);
	set_solib_svr4_fetch_link_map_offsets
	  (gdbarch, mips64_linux_svr4_fetch_link_map_offsets);
	set_mips_linux_register_addr (gdbarch, mips64_linux_register_addr);
	break;
      default:
	internal_error (__FILE__, __LINE__, "can't handle ABI");
	break;
    }
}

void
_initialize_mips_linux_tdep (void)
{
  const struct bfd_arch_info *arch_info;

  register_addr_data =
    register_gdbarch_data (init_register_addr_data, 0);

  for (arch_info = bfd_lookup_arch (bfd_arch_mips, 0);
       arch_info != NULL;
       arch_info = arch_info->next)
    {
      gdbarch_register_osabi (bfd_arch_mips, arch_info->mach, GDB_OSABI_LINUX,
			      mips_linux_init_abi);
    }

  add_core_fns (&regset_core_fns);
}
