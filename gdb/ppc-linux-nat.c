/* PPC linux native support.
   Copyright 1988, 1989, 1991, 1992, 1994, 1996, 2000, 2001
   Free Software Foundation, Inc.

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

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "regcache.h"

#include <sys/types.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/procfs.h>

/* Prototypes for supply_gregset etc. */
#include "gregset.h"
#include "ppc-tdep.h"

int
kernel_u_size (void)
{
  return (sizeof (struct user));
}

/* *INDENT-OFF* */
/* registers layout, as presented by the ptrace interface:
PT_R0, PT_R1, PT_R2, PT_R3, PT_R4, PT_R5, PT_R6, PT_R7,
PT_R8, PT_R9, PT_R10, PT_R11, PT_R12, PT_R13, PT_R14, PT_R15,
PT_R16, PT_R17, PT_R18, PT_R19, PT_R20, PT_R21, PT_R22, PT_R23,
PT_R24, PT_R25, PT_R26, PT_R27, PT_R28, PT_R29, PT_R30, PT_R31,
PT_FPR0, PT_FPR0 + 2, PT_FPR0 + 4, PT_FPR0 + 6, PT_FPR0 + 8, PT_FPR0 + 10, PT_FPR0 + 12, PT_FPR0 + 14,
PT_FPR0 + 16, PT_FPR0 + 18, PT_FPR0 + 20, PT_FPR0 + 22, PT_FPR0 + 24, PT_FPR0 + 26, PT_FPR0 + 28, PT_FPR0 + 30,
PT_FPR0 + 32, PT_FPR0 + 34, PT_FPR0 + 36, PT_FPR0 + 38, PT_FPR0 + 40, PT_FPR0 + 42, PT_FPR0 + 44, PT_FPR0 + 46,
PT_FPR0 + 48, PT_FPR0 + 50, PT_FPR0 + 52, PT_FPR0 + 54, PT_FPR0 + 56, PT_FPR0 + 58, PT_FPR0 + 60, PT_FPR0 + 62,
PT_NIP, PT_MSR, PT_CCR, PT_LNK, PT_CTR, PT_XER, PT_MQ */
/* *INDENT_ON * */

int 
ppc_register_u_addr (int ustart, int regno)
{
  int u_addr = -1;

  /* General purpose registers occupy 1 slot each in the buffer */
  if (regno >= PPC_GP0_REGNUM && regno <= PPC_GPLAST_REGNUM )
    u_addr =  (ustart + (PT_R0 + regno) * 4);

  /* Floating point regs: 2 slots each */
  if (regno >= FP0_REGNUM && regno <= FPLAST_REGNUM)
    u_addr = (ustart + (PT_FPR0 + (regno - FP0_REGNUM) * 2) * 4);

  /* UISA special purpose registers: 1 slot each */
  if (regno == PC_REGNUM)
    u_addr = ustart + PT_NIP * 4;
  if (regno == PPC_LR_REGNUM)
    u_addr = ustart + PT_LNK * 4;
  if (regno == PPC_CR_REGNUM)
    u_addr = ustart + PT_CCR * 4;
  if (regno == PPC_XER_REGNUM)
    u_addr = ustart + PT_XER * 4;
  if (regno == PPC_CTR_REGNUM)
    u_addr = ustart + PT_CTR * 4;
  if (regno == PPC_MQ_REGNUM)
    u_addr = ustart + PT_MQ * 4;
  if (regno == PPC_PS_REGNUM)
    u_addr = ustart + PT_MSR * 4;

  return u_addr;
}

void
supply_gregset (gdb_gregset_t *gregsetp)
{
  int regi;
  register elf_greg_t *regp = (elf_greg_t *) gregsetp;

  for (regi = 0; regi < 32; regi++)
    supply_register (regi, (char *) (regp + regi));

  supply_register (PC_REGNUM, (char *) (regp + PT_NIP));
  supply_register (PPC_LR_REGNUM, (char *) (regp + PT_LNK));
  supply_register (PPC_CR_REGNUM, (char *) (regp + PT_CCR));
  supply_register (PPC_XER_REGNUM, (char *) (regp + PT_XER));
  supply_register (PPC_CTR_REGNUM, (char *) (regp + PT_CTR));
  supply_register (PPC_MQ_REGNUM, (char *) (regp + PT_MQ));
  supply_register (PPC_PS_REGNUM, (char *) (regp + PT_MSR));
}

void
fill_gregset (gdb_gregset_t *gregsetp, int regno)
{
  int regi;
  elf_greg_t *regp = (elf_greg_t *) gregsetp;

  for (regi = 0; regi < 32; regi++)
    {
      if ((regno == -1) || regno == regi)
        regcache_collect (regi, regp + PT_R0 + regi);
    }

  if ((regno == -1) || regno == PC_REGNUM)
    regcache_collect (PC_REGNUM, regp + PT_NIP);
  if ((regno == -1) || regno == PPC_LR_REGNUM)
    regcache_collect (PPC_LR_REGNUM, regp + PT_LNK);
  if ((regno == -1) || regno == PPC_CR_REGNUM)
    regcache_collect (PPC_CR_REGNUM, regp + PT_CCR);
  if ((regno == -1) || regno == PPC_XER_REGNUM)
    regcache_collect (PPC_XER_REGNUM, regp + PT_XER);
  if ((regno == -1) || regno == PPC_CTR_REGNUM)
    regcache_collect (PPC_CTR_REGNUM, regp + PT_CTR);
  if ((regno == -1) || regno == PPC_MQ_REGNUM)
    regcache_collect (PPC_MQ_REGNUM, regp + PT_MQ);
  if ((regno == -1) || regno == PPC_PS_REGNUM)
    regcache_collect (PPC_PS_REGNUM, regp + PT_MSR);
}

void
supply_fpregset (gdb_fpregset_t * fpregsetp)
{
  int regi;
  for (regi = 0; regi < 32; regi++)
    {
      supply_register (FP0_REGNUM + regi, (char *) (*fpregsetp + regi));
    }
}

/*  Given a pointer to a floating point register set in /proc format
   (fpregset_t *), update the register specified by REGNO from gdb's idea
   of the current floating point register set.  If REGNO is -1, update
   them all. */

void
fill_fpregset (gdb_fpregset_t *fpregsetp, int regno)
{
  int regi;
  
  for (regi = 0; regi < 32; regi++)
    {
      if ((regno == -1) || (regno == FP0_REGNUM + regi))
	regcache_collect (FP0_REGNUM + regi, (char *) (*fpregsetp + regi));
    }
}
