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
#include <sys/ptrace.h>

/* Prototypes for supply_gregset etc. */
#include "gregset.h"
#include "ppc-tdep.h"

#ifndef PT_READ_U
#define PT_READ_U PTRACE_PEEKUSR
#endif
#ifndef PT_WRITE_U
#define PT_WRITE_U PTRACE_POKEUSR
#endif

/* Default the type of the ptrace transfer to int.  */
#ifndef PTRACE_XFER_TYPE
#define PTRACE_XFER_TYPE int
#endif

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

static int
ppc_register_u_addr (int regno)
{
  int u_addr = -1;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  /* General purpose registers occupy 1 slot each in the buffer */
  if (regno >= tdep->ppc_gp0_regnum && regno <= tdep->ppc_gplast_regnum )
    u_addr =  ((PT_R0 + regno) * 4);

  /* Floating point regs: 2 slots each */
  if (regno >= FP0_REGNUM && regno <= FPLAST_REGNUM)
    u_addr = ((PT_FPR0 + (regno - FP0_REGNUM) * 2) * 4);

  /* UISA special purpose registers: 1 slot each */
  if (regno == PC_REGNUM)
    u_addr = PT_NIP * 4;
  if (regno == tdep->ppc_lr_regnum)
    u_addr = PT_LNK * 4;
  if (regno == tdep->ppc_cr_regnum)
    u_addr = PT_CCR * 4;
  if (regno == tdep->ppc_xer_regnum)
    u_addr = PT_XER * 4;
  if (regno == tdep->ppc_ctr_regnum)
    u_addr = PT_CTR * 4;
  if (regno == tdep->ppc_mq_regnum)
    u_addr = PT_MQ * 4;
  if (regno == tdep->ppc_ps_regnum)
    u_addr = PT_MSR * 4;

  return u_addr;
}

static int
ppc_ptrace_cannot_fetch_store_register (int regno)
{
  return (ppc_register_u_addr (regno) == -1);
}

static void
fetch_register (int regno)
{
  /* This isn't really an address.  But ptrace thinks of it as one.  */
  char mess[128];              /* For messages */
  register int i;
  unsigned int offset;         /* Offset of registers within the u area. */
  char *buf = alloca (MAX_REGISTER_RAW_SIZE);
  int tid;
  CORE_ADDR regaddr = ppc_register_u_addr (regno);

  if (regaddr == -1)
    {
      memset (buf, '\0', REGISTER_RAW_SIZE (regno));   /* Supply zeroes */
      supply_register (regno, buf);
      return;
    }

  /* Overload thread id onto process id */
  if ((tid = TIDGET (inferior_ptid)) == 0)
    tid = PIDGET (inferior_ptid);      /* no thread id, just use process id */

  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      *(PTRACE_XFER_TYPE *) & buf[i] = ptrace (PT_READ_U, tid,
					       (PTRACE_ARG3_TYPE) regaddr, 0);
      regaddr += sizeof (PTRACE_XFER_TYPE);
      if (errno != 0)
	{
	  sprintf (mess, "reading register %s (#%d)", 
		   REGISTER_NAME (regno), regno);
	  perror_with_name (mess);
	}
    }
  supply_register (regno, buf);
}

static void 
fetch_ppc_registers (void)
{
  int i;
  int last_register = gdbarch_tdep (current_gdbarch)->ppc_mq_regnum;
  
  for (i = 0; i <= last_register; i++)
    fetch_register (i);
}

/* Fetch registers from the child process.  Fetch all registers if
   regno == -1, otherwise fetch all general registers or all floating
   point registers depending upon the value of regno.  */
void
fetch_inferior_registers (int regno)
{
  if (regno == -1)
    fetch_ppc_registers ();
  else 
    fetch_register (regno);
}

/* Store one register. */
static void
store_register (int regno)
{
  /* This isn't really an address.  But ptrace thinks of it as one.  */
  CORE_ADDR regaddr = ppc_register_u_addr (regno);
  char mess[128];              /* For messages */
  register int i;
  unsigned int offset;         /* Offset of registers within the u area.  */
  int tid;
  char *buf = alloca (MAX_REGISTER_RAW_SIZE);

  if (regaddr == -1)
    {
      return;
    }

  /* Overload thread id onto process id */
  if ((tid = TIDGET (inferior_ptid)) == 0)
    tid = PIDGET (inferior_ptid);      /* no thread id, just use process id */

  regcache_collect (regno, buf);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      ptrace (PT_WRITE_U, tid, (PTRACE_ARG3_TYPE) regaddr,
	      *(PTRACE_XFER_TYPE *) & buf[i]);
      regaddr += sizeof (PTRACE_XFER_TYPE);
      if (errno != 0)
	{
	  sprintf (mess, "writing register %s (#%d)", 
		   REGISTER_NAME (regno), regno);
	  perror_with_name (mess);
	}
    }
}

static void
store_ppc_registers (void)
{
  int i;
  int last_register = gdbarch_tdep (current_gdbarch)->ppc_mq_regnum;
  
  for (i = 0; i <= last_register; i++)
    store_register (i);
}

void
store_inferior_registers (int regno)
{
  if (regno >= 0)
    store_register (regno);
  else
    store_ppc_registers ();
}

void
supply_gregset (gdb_gregset_t *gregsetp)
{
  int regi;
  register elf_greg_t *regp = (elf_greg_t *) gregsetp;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch); 

  for (regi = 0; regi < 32; regi++)
    supply_register (regi, (char *) (regp + regi));

  supply_register (PC_REGNUM, (char *) (regp + PT_NIP));
  supply_register (tdep->ppc_lr_regnum, (char *) (regp + PT_LNK));
  supply_register (tdep->ppc_cr_regnum, (char *) (regp + PT_CCR));
  supply_register (tdep->ppc_xer_regnum, (char *) (regp + PT_XER));
  supply_register (tdep->ppc_ctr_regnum, (char *) (regp + PT_CTR));
  supply_register (tdep->ppc_mq_regnum, (char *) (regp + PT_MQ));
  supply_register (tdep->ppc_ps_regnum, (char *) (regp + PT_MSR));
}

void
fill_gregset (gdb_gregset_t *gregsetp, int regno)
{
  int regi;
  elf_greg_t *regp = (elf_greg_t *) gregsetp;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch); 

  for (regi = 0; regi < 32; regi++)
    {
      if ((regno == -1) || regno == regi)
        regcache_collect (regi, regp + PT_R0 + regi);
    }

  if ((regno == -1) || regno == PC_REGNUM)
    regcache_collect (PC_REGNUM, regp + PT_NIP);
  if ((regno == -1) 
      || regno == tdep->ppc_lr_regnum)
    regcache_collect (tdep->ppc_lr_regnum, regp + PT_LNK);
  if ((regno == -1)
      || regno == tdep->ppc_cr_regnum)
    regcache_collect (tdep->ppc_cr_regnum, regp + PT_CCR);
  if ((regno == -1)
      || regno == tdep->ppc_xer_regnum)
    regcache_collect (tdep->ppc_xer_regnum, regp + PT_XER);
  if ((regno == -1)
      || regno == tdep->ppc_ctr_regnum)
    regcache_collect (tdep->ppc_ctr_regnum, regp + PT_CTR);
  if ((regno == -1)
      || regno == tdep->ppc_mq_regnum)
    regcache_collect (tdep->ppc_mq_regnum, regp + PT_MQ);
  if ((regno == -1)
      || regno == tdep->ppc_ps_regnum)
    regcache_collect (tdep->ppc_ps_regnum, regp + PT_MSR);
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
