/* PPC GNU/Linux native support.

   Copyright 1988, 1989, 1991, 1992, 1994, 1996, 2000, 2001, 2002,
   2003 Free Software Foundation, Inc.

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
#include "gdb_string.h"
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "regcache.h"

#include <sys/types.h>
#include <sys/param.h>
#include <signal.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include "gdb_wait.h"
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

/* Default the type of the ptrace transfer to long.  */
#ifndef PTRACE_XFER_TYPE
#define PTRACE_XFER_TYPE long
#endif

/* Write DATA into location ADDR within the "user area" on a 64-bit
   process from a 32-bit process. */
#ifndef PPC_PTRACE_POKEUSR_3264
#define PPC_PTRACE_POKEUSR_3264   0x90 
#endif

/* Read a register (specified by ADDR) out of the "user area" on a
   64-bit process from a 32-bit process. */
#ifndef PPC_PTRACE_PEEKUSR_3264
#define PPC_PTRACE_PEEKUSR_3264   0x91
#endif

/* Write word at location ADDR on a 64-bit process from a 32-bit process. */
#ifndef PPC_PTRACE_POKEDATA_3264
#define PPC_PTRACE_POKEDATA_3264   0x92
#endif

/* Read word at location ADDR on a 64-bit process from a 32-bit
   process. */
#ifndef PPC_PTRACE_PEEKDATA_3264
#define PPC_PTRACE_PEEKDATA_3264   0x94
#endif

#define ARCH64() (REGISTER_RAW_SIZE (0) == 8)

/* REALLY SHAMELESS HACK:

   32 bit programs can exec 64 bit programs and so forth. GDB launches
   the inferior process by lauching ${SHELL} -c <program and args>.
   Fortunately, we know that it only tries to get the PC so we only
   have to hack that.. I THINK.

   At this time we are not ready to unify ppc32 and ppc64 as
   rs/6000-aix is. and there is know easy way to find out if a process
   is running 32 or 64 bits so we have this little hack.

   EVEN MORE SHAMELESS HACK: rs6000-nat.c solves this problem by
   expecting the first few ptracex() calls to fail.

*/
#include <sys/stat.h>
#include "bfd/elf-bfd.h"
static int
ppc_wordsize_pid(pid_t pid)
{
  static ino_t fino = 0;
  static int last = 0;
  struct stat sb;
  const char fmt[] = "/proc/%u/exe";
  FILE *file;
  char *fname = alloca (sizeof(fmt) + 10); /* 10 digit pid.. why not */
  Elf_Internal_Ehdr elfh;

  if ((gdbarch_tdep (current_gdbarch))->wordsize == 4)
    return 4;

  sprintf (fname, fmt, pid);

  if (stat(fname, &sb) == -1)
    {
      internal_error (__FILE__, __LINE__,
		      "could not stat executable from /proc.");
      return 0;
    }

  if (fino == sb.st_ino)
      return last;

  fino = sb.st_ino;

  /* FIXME: could stat the file and check if inode changed. */
  file = fopen (fname, "rb");
  if (file == NULL)
    {
      internal_error (__FILE__, __LINE__,
		      "could not open executable from /proc.");
      return 0;
    }

  if (fread (elfh.e_ident, EI_NIDENT, 1, file) == 1)
    {
      if (elfh.e_ident [EI_CLASS] == ELFCLASS64)
	last = 8;
      else
	last = 4;
    }
  else
    {
      last = 0;
      internal_error (__FILE__, __LINE__,
		      "could not read executable from /proc.");
    }
  fclose (file);
  
  return last;
}

/* Glibc's headers don't define PTRACE_GETVRREGS so we cannot use a
   configure time check.  Some older glibc's (for instance 2.2.1)
   don't have a specific powerpc version of ptrace.h, and fall back on
   a generic one.  In such cases, sys/ptrace.h defines
   PTRACE_GETFPXREGS and PTRACE_SETFPXREGS to the same numbers that
   ppc kernel's asm/ptrace.h defines PTRACE_GETVRREGS and
   PTRACE_SETVRREGS to be.  This also makes a configury check pretty
   much useless.  */

/* These definitions should really come from the glibc header files,
   but Glibc doesn't know about the vrregs yet.  */
#ifndef PTRACE_GETVRREGS
#define PTRACE_GETVRREGS 18
#define PTRACE_SETVRREGS 19
#endif

/* This oddity is because the Linux kernel defines elf_vrregset_t as
   an array of 33 16 bytes long elements.  I.e. it leaves out vrsave.
   However the PTRACE_GETVRREGS and PTRACE_SETVRREGS requests return
   the vrsave as an extra 4 bytes at the end.  I opted for creating a
   flat array of chars, so that it is easier to manipulate for gdb.

   There are 32 vector registers 16 bytes longs, plus a VSCR register
   which is only 4 bytes long, but is fetched as a 16 bytes
   quantity. Up to here we have the elf_vrregset_t structure.
   Appended to this there is space for the VRSAVE register: 4 bytes.
   Even though this vrsave register is not included in the regset
   typedef, it is handled by the ptrace requests.

   Note that GNU/Linux doesn't support little endian PPC hardware,
   therefore the offset at which the real value of the VSCR register
   is located will be always 12 bytes.

   The layout is like this (where x is the actual value of the vscr reg): */

/* *INDENT-OFF* */
/*
   |.|.|.|.|.....|.|.|.|.||.|.|.|x||.|
   <------->     <-------><-------><->
     VR0           VR31     VSCR    VRSAVE
*/
/* *INDENT-ON* */

#define SIZEOF_VRREGS 33*16+4

typedef char gdb_vrregset_t[SIZEOF_VRREGS];

/* For runtime check of ptrace support for VRREGS.  */
int have_ptrace_getvrregs = 1;

int
kernel_u_size (void)
{
  if ((gdbarch_tdep (current_gdbarch))->wordsize == sizeof (PTRACE_XFER_TYPE))
    return (sizeof (struct user));
  else
    {
      /* with a 64-bit kernel, all members of struct user go from 32
         to 64 bit except for the u_comm character array so we can
         double everything and subtract sizeof u_comm. */
      return ((sizeof (struct user) * 2) - sizeof (((struct user*)0)->u_comm));
    }
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
  int wordsize = tdep->wordsize;

  /* General purpose registers occupy 1 slot each in the buffer */
  if (regno >= tdep->ppc_gp0_regnum && regno <= tdep->ppc_gplast_regnum )
    u_addr =  ((PT_R0 + regno) * wordsize);

  /* Floating point regs: 2 slots each */
  if (regno >= FP0_REGNUM && regno <= FPLAST_REGNUM)
    u_addr = ((PT_FPR0 + (regno - FP0_REGNUM) * 2) * wordsize);

  /* UISA special purpose registers: 1 slot each */
  if (regno == PC_REGNUM)
    u_addr = PT_NIP * wordsize;
  if (regno == tdep->ppc_lr_regnum)
    u_addr = PT_LNK * wordsize;
  if (regno == tdep->ppc_cr_regnum)
    u_addr = PT_CCR * wordsize;
  if (regno == tdep->ppc_xer_regnum)
    u_addr = PT_XER * wordsize;
  if (regno == tdep->ppc_ctr_regnum)
    u_addr = PT_CTR * wordsize;
  if (regno == tdep->ppc_mq_regnum)
    u_addr = PT_MQ * wordsize;
  if (regno == tdep->ppc_ps_regnum)
    u_addr = PT_MSR * wordsize;
  if (regno == tdep->ppc_fpscr_regnum)
    u_addr = PT_FPSCR * wordsize;

  return u_addr;
}

static int
ppc_ptrace_cannot_fetch_store_register (int regno)
{
  return (ppc_register_u_addr (regno) == -1);
}

/* The Linux kernel ptrace interface for AltiVec registers uses the
   registers set mechanism, as opposed to the interface for all the
   other registers, that stores/fetches each register individually.  */
static void
fetch_altivec_register (int tid, int regno)
{
  int ret;
  int offset = 0;
  gdb_vrregset_t regs;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int vrregsize = REGISTER_RAW_SIZE (tdep->ppc_vr0_regnum);

  ret = ptrace (PTRACE_GETVRREGS, tid, 0, &regs);
  if (ret < 0)
    {
      if (errno == EIO)
        {
          have_ptrace_getvrregs = 0;
          return;
        }
      perror_with_name ("Unable to fetch AltiVec register");
    }
 
  /* VSCR is fetched as a 16 bytes quantity, but it is really 4 bytes
     long on the hardware.  We deal only with the lower 4 bytes of the
     vector.  VRSAVE is at the end of the array in a 4 bytes slot, so
     there is no need to define an offset for it.  */
  if (regno == (tdep->ppc_vrsave_regnum - 1))
    offset = vrregsize - REGISTER_RAW_SIZE (tdep->ppc_vrsave_regnum);
  
  supply_register (regno,
                   regs + (regno - tdep->ppc_vr0_regnum) * vrregsize + offset);
}

static void
fetch_register (int tid, int regno)
{
  /* This isn't really an address.  But ptrace thinks of it as one.  */
  char mess[128];              /* For messages */
  register int i;
  unsigned int offset;         /* Offset of registers within the u area. */
  char buf[MAX_REGISTER_SIZE];
  CORE_ADDR regaddr = ppc_register_u_addr (regno);
  int wordsize = (gdbarch_tdep (current_gdbarch))->wordsize;

  /* Do the easy thing for now which is to silently succeed if we are
     attached to a 32-bit process when we are expecting 64-bits */
  if (wordsize != ppc_wordsize_pid(tid))
    {
      /* supplying garbage.. but that's ok */
      supply_register (regno, buf);
      return;
    }

  if (altivec_register_p (regno))
    {
      /* If this is the first time through, or if it is not the first
         time through, and we have comfirmed that there is kernel
         support for such a ptrace request, then go and fetch the
         register.  */
      if (have_ptrace_getvrregs)
       {
         fetch_altivec_register (tid, regno);
         return;
       }
     /* If we have discovered that there is no ptrace support for
        AltiVec registers, fall through and return zeroes, because
        regaddr will be -1 in this case.  */
    }

  if (regaddr == -1)
    {
      memset (buf, '\0', REGISTER_RAW_SIZE (regno));   /* Supply zeroes */
      supply_register (regno, buf);
      return;
    }

  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      if (wordsize != sizeof (PTRACE_XFER_TYPE))
	{
	  PTRACE_XFER_TYPE reg;
	  ptrace (PPC_PTRACE_PEEKUSR_3264, tid,
		  (PTRACE_ARG3_TYPE) regaddr, &reg);
	  *(PTRACE_XFER_TYPE *) & buf[i] = reg;
	}
      else
      {
	  *(PTRACE_XFER_TYPE *) & buf[i] = ptrace (PT_READ_U, tid,
						   (PTRACE_ARG3_TYPE) regaddr, 0);
      }

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
supply_vrregset (gdb_vrregset_t *vrregsetp)
{
  int i;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int num_of_vrregs = tdep->ppc_vrsave_regnum - tdep->ppc_vr0_regnum + 1;
  int vrregsize = REGISTER_RAW_SIZE (tdep->ppc_vr0_regnum);
  int offset = vrregsize - REGISTER_RAW_SIZE (tdep->ppc_vrsave_regnum);

  for (i = 0; i < num_of_vrregs; i++)
    {
      /* The last 2 registers of this set are only 32 bit long, not
         128.  However an offset is necessary only for VSCR because it
         occupies a whole vector, while VRSAVE occupies a full 4 bytes
         slot.  */
      if (i == (num_of_vrregs - 2))
        supply_register (tdep->ppc_vr0_regnum + i,
                         *vrregsetp + i * vrregsize + offset);
      else
        supply_register (tdep->ppc_vr0_regnum + i, *vrregsetp + i * vrregsize);
    }
}

static void
fetch_altivec_registers (int tid)
{
  int ret;
  gdb_vrregset_t regs;
  
  ret = ptrace (PTRACE_GETVRREGS, tid, 0, &regs);
  if (ret < 0)
    {
      if (errno == EIO)
	{
          have_ptrace_getvrregs = 0;
	  return;
	}
      perror_with_name ("Unable to fetch AltiVec registers");
    }
  supply_vrregset (&regs);
}

static void 
fetch_ppc_registers (int tid)
{
  int i;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);

  for (i = 0; i <= tdep->ppc_fpscr_regnum; i++)
    fetch_register (tid, i);
  if (tdep->ppc_mq_regnum != -1)
    fetch_register (tid, tdep->ppc_mq_regnum);
  if (have_ptrace_getvrregs)
    if (tdep->ppc_vr0_regnum != -1 && tdep->ppc_vrsave_regnum != -1)
      fetch_altivec_registers (tid);
}

/* Fetch registers from the child process.  Fetch all registers if
   regno == -1, otherwise fetch all general registers or all floating
   point registers depending upon the value of regno.  */
void
fetch_inferior_registers (int regno)
{
  /* Overload thread id onto process id */
  int tid = TIDGET (inferior_ptid);

  /* No thread id, just use process id */
  if (tid == 0)
    tid = PIDGET (inferior_ptid);

  if (regno == -1)
    fetch_ppc_registers (tid);
  else 
    fetch_register (tid, regno);
}

/* Store one register. */
static void
store_altivec_register (int tid, int regno)
{
  int ret;
  int offset = 0;
  gdb_vrregset_t regs;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int vrregsize = REGISTER_RAW_SIZE (tdep->ppc_vr0_regnum);

  ret = ptrace (PTRACE_GETVRREGS, tid, 0, &regs);
  if (ret < 0)
    {
      if (errno == EIO)
        {
          have_ptrace_getvrregs = 0;
          return;
        }
      perror_with_name ("Unable to fetch AltiVec register");
    }

  /* VSCR is fetched as a 16 bytes quantity, but it is really 4 bytes
     long on the hardware.  */
  if (regno == (tdep->ppc_vrsave_regnum - 1))
    offset = vrregsize - REGISTER_RAW_SIZE (tdep->ppc_vrsave_regnum);

  regcache_collect (regno,
                    regs + (regno - tdep->ppc_vr0_regnum) * vrregsize + offset);

  ret = ptrace (PTRACE_SETVRREGS, tid, 0, &regs);
  if (ret < 0)
    perror_with_name ("Unable to store AltiVec register");
}

static void
store_register (int tid, int regno)
{
  /* This isn't really an address.  But ptrace thinks of it as one.  */
  CORE_ADDR regaddr = ppc_register_u_addr (regno);
  char mess[128];              /* For messages */
  register int i;
  unsigned int offset;         /* Offset of registers within the u area.  */
  char buf[MAX_REGISTER_SIZE];
  int wordsize = (gdbarch_tdep (current_gdbarch))->wordsize;

  if (altivec_register_p (regno))
    {
      store_altivec_register (tid, regno);
      return;
    }

  if (regaddr == -1)
    return;

  regcache_collect (regno, buf);
  for (i = 0; i < REGISTER_RAW_SIZE (regno); i += sizeof (PTRACE_XFER_TYPE))
    {
      errno = 0;
      if (wordsize != sizeof (PTRACE_XFER_TYPE))
	{
	  PTRACE_XFER_TYPE reg;
	  ptrace (PPC_PTRACE_POKEUSR_3264, tid, (PTRACE_ARG3_TYPE) regaddr,
		  *(PTRACE_XFER_TYPE *) & buf[i]);
	}
      else
      {
	  ptrace (PT_WRITE_U, tid, (PTRACE_ARG3_TYPE) regaddr,
		  *(PTRACE_XFER_TYPE *) & buf[i]);
      }

      regaddr += sizeof (PTRACE_XFER_TYPE);

      if (errno == EIO 
          && regno == gdbarch_tdep (current_gdbarch)->ppc_fpscr_regnum)
	{
	  /* Some older kernel versions don't allow fpscr to be written.  */
	  continue;
	}

      if (errno != 0)
	{
	  sprintf (mess, "writing register %s (#%d)", 
		   REGISTER_NAME (regno), regno);
	  perror_with_name (mess);
	}
    }
}

static void
fill_vrregset (gdb_vrregset_t *vrregsetp)
{
  int i;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  int num_of_vrregs = tdep->ppc_vrsave_regnum - tdep->ppc_vr0_regnum + 1;
  int vrregsize = REGISTER_RAW_SIZE (tdep->ppc_vr0_regnum);
  int offset = vrregsize - REGISTER_RAW_SIZE (tdep->ppc_vrsave_regnum);

  for (i = 0; i < num_of_vrregs; i++)
    {
      /* The last 2 registers of this set are only 32 bit long, not
         128, but only VSCR is fetched as a 16 bytes quantity.  */
      if (i == (num_of_vrregs - 2))
        regcache_collect (tdep->ppc_vr0_regnum + i,
                          *vrregsetp + i * vrregsize + offset);
      else
        regcache_collect (tdep->ppc_vr0_regnum + i, *vrregsetp + i * vrregsize);
    }
}

static void
store_altivec_registers (int tid)
{
  int ret;
  gdb_vrregset_t regs;

  ret = ptrace (PTRACE_GETVRREGS, tid, 0, (int) &regs);
  if (ret < 0)
    {
      if (errno == EIO)
        {
          have_ptrace_getvrregs = 0;
          return;
        }
      perror_with_name ("Couldn't get AltiVec registers");
    }

  fill_vrregset (&regs);
  
  if (ptrace (PTRACE_SETVRREGS, tid, 0, (int) &regs) < 0)
    perror_with_name ("Couldn't write AltiVec registers");
}

static void
store_ppc_registers (int tid)
{
  int i;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch);
  
  for (i = 0; i <= tdep->ppc_fpscr_regnum; i++)
    store_register (tid, i);
  if (tdep->ppc_mq_regnum != -1)
    store_register (tid, tdep->ppc_mq_regnum);
  if (have_ptrace_getvrregs)
    if (tdep->ppc_vr0_regnum != -1 && tdep->ppc_vrsave_regnum != -1)
      store_altivec_registers (tid);
}

void
store_inferior_registers (int regno)
{
  /* Overload thread id onto process id */
  int tid = TIDGET (inferior_ptid);

  /* No thread id, just use process id */
  if (tid == 0)
    tid = PIDGET (inferior_ptid);

  if (regno >= 0)
    store_register (tid, regno);
  else
    store_ppc_registers (tid);
}

void
supply_gregset (gdb_gregset_t *gregsetp)
{
  ppc_linux_supply_gregset ((char *) gregsetp);
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
  if ((regno == -1) || regno == tdep->ppc_lr_regnum)
    regcache_collect (tdep->ppc_lr_regnum, regp + PT_LNK);
  if ((regno == -1) || regno == tdep->ppc_cr_regnum)
    regcache_collect (tdep->ppc_cr_regnum, regp + PT_CCR);
  if ((regno == -1) || regno == tdep->ppc_xer_regnum)
    regcache_collect (tdep->ppc_xer_regnum, regp + PT_XER);
  if ((regno == -1) || regno == tdep->ppc_ctr_regnum)
    regcache_collect (tdep->ppc_ctr_regnum, regp + PT_CTR);
  if (((regno == -1) || regno == tdep->ppc_mq_regnum)
      && (tdep->ppc_mq_regnum != -1))
    regcache_collect (tdep->ppc_mq_regnum, regp + PT_MQ);
  if ((regno == -1) || regno == tdep->ppc_ps_regnum)
    regcache_collect (tdep->ppc_ps_regnum, regp + PT_MSR);
}

void
supply_fpregset (gdb_fpregset_t * fpregsetp)
{
  ppc_linux_supply_fpregset ((char *) fpregsetp);
}

/* Given a pointer to a floating point register set in /proc format
   (fpregset_t *), update the register specified by REGNO from gdb's
   idea of the current floating point register set.  If REGNO is -1,
   update them all.  */
void
fill_fpregset (gdb_fpregset_t *fpregsetp, int regno)
{
  int regi;
  struct gdbarch_tdep *tdep = gdbarch_tdep (current_gdbarch); 
  
  for (regi = 0; regi < 32; regi++)
    {
      if ((regno == -1) || (regno == FP0_REGNUM + regi))
	regcache_collect (FP0_REGNUM + regi, (char *) (*fpregsetp + regi));
    }
  if ((regno == -1) || regno == tdep->ppc_fpscr_regnum)
    regcache_collect (tdep->ppc_fpscr_regnum, (char *) (*fpregsetp + regi));
}


#ifdef CHILD_XFER_MEMORY

/* this is a complete rip off from infptrace.c */

#ifndef GDB_MAX_ALLOCA
#define GDB_MAX_ALLOCA 0x1000
#endif /* GDB_MAX_ALLOCA */

/* Copy LEN bytes to or from inferior's memory starting at MEMADDR to
   debugger memory starting at MYADDR.  Copy to inferior if WRITE is
   nonzero.  TARGET is ignored.

   Returns the length copied, which is either the LEN argument or
   zero.  This xfer function does not do partial moves, since
   child_ops doesn't allow memory operations to cross below us in the
   target stack anyway.  */

int
child_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len, int write,
		   struct mem_attrib *attrib, struct target_ops *target)
{
    int i;
  /* Round starting address down to longword boundary.  */
    CORE_ADDR addr = memaddr & -(CORE_ADDR) sizeof (PTRACE_XFER_TYPE);
  /* Round ending address up; get number of longwords that makes.  */
    int count = ((((memaddr + len) - addr) + sizeof (PTRACE_XFER_TYPE) - 1)
		 / sizeof (PTRACE_XFER_TYPE));
    int alloc = count * sizeof (PTRACE_XFER_TYPE);
    PTRACE_XFER_TYPE *buffer;
    struct cleanup *old_chain = NULL;
    int wordsize = (gdbarch_tdep (current_gdbarch))->wordsize;
    int arch64 = ARCH64();

  /* Allocate buffer of that many longwords.  */
    if (len < GDB_MAX_ALLOCA)
    {
	buffer = (PTRACE_XFER_TYPE *) alloca (alloc);
    }
    else
    {
	buffer = (PTRACE_XFER_TYPE *) xmalloc (alloc);
	old_chain = make_cleanup (xfree, buffer);
    }

  /* WARNING: from kernel source: "when I and D space are separate,
     these will need to be fixed." */
    if (write)
    {
      /* Fill start and end extra bytes of buffer with existing memory
	 data.  */
	if (addr != memaddr || len < (int) sizeof (PTRACE_XFER_TYPE))
	{
	  /* Need part of initial word -- fetch it.  */
	    if (wordsize == sizeof (PTRACE_XFER_TYPE) && (!arch64))
	    {
		buffer[0] = ptrace (PT_READ_I, PIDGET (inferior_ptid), 
				    (PTRACE_ARG3_TYPE) addr, 0);
	    }
	    else
	    {
		if (arch64) {
		    buffer[0] = ptrace (PTRACE_PEEKDATA, PIDGET (inferior_ptid), 
					(unsigned long) addr, 0);
		}
		else 
		{
		    ptrace (PPC_PTRACE_PEEKDATA_3264, PIDGET (inferior_ptid), 
			    (PTRACE_ARG3_TYPE) &addr, buffer);
		}
	    }
	}
	if (count > 1)		/* FIXME, avoid if even boundary.  */
	{
	    CORE_ADDR a64 = (addr + (count - 1) * sizeof (PTRACE_XFER_TYPE));
	    if (wordsize == sizeof (PTRACE_XFER_TYPE) && (!arch64))
	    {
		buffer[count - 1] = ptrace (PT_READ_I, PIDGET (inferior_ptid),
					    (PTRACE_ARG3_TYPE) a64, 0);
	    }
	    else
		if (arch64) {
		    buffer[count-1] = ptrace (PTRACE_PEEKDATA, PIDGET (inferior_ptid),
					      (PTRACE_ARG3_TYPE) &a64, 0);
		}
		else
		{
		    ptrace (PPC_PTRACE_PEEKDATA_3264, PIDGET (inferior_ptid),
			    (PTRACE_ARG3_TYPE) &a64, &buffer[count - 1]);
		}
	}

      /* Copy data to be written over corresponding part of buffer.  */
	memcpy ((char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)),
		myaddr, len);

      /* Write the entire buffer.  */
	for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
	{
	    errno = 0;
	    if (wordsize == sizeof (PTRACE_XFER_TYPE) && (!arch64))
	    {
		ptrace (PT_WRITE_D, PIDGET (inferior_ptid), 
			(PTRACE_ARG3_TYPE) addr, buffer[i]);
	    }
	    else
		if (arch64) {
		    ptrace (PT_WRITE_D, PIDGET (inferior_ptid), 
			    (unsigned long) addr, buffer[i]);  
		}
		else 
		{
		    ptrace (PPC_PTRACE_POKEDATA_3264, PIDGET (inferior_ptid), 
			    (PTRACE_ARG3_TYPE) &addr, buffer[i]);
		}
	    if (errno)
	    {
		errno = 0;
		if (wordsize == sizeof (PTRACE_XFER_TYPE) && (!arch64))
		{
		    ptrace (PT_WRITE_I, PIDGET (inferior_ptid), 
			    (PTRACE_ARG3_TYPE) addr, buffer[i]);
		}
		else
		    if (arch64) {
			ptrace (PTRACE_POKEDATA, PIDGET (inferior_ptid),  
				(PTRACE_ARG3_TYPE) addr, buffer[i]); 
		    }
		    else 
		    {
			ptrace (PPC_PTRACE_POKEDATA_3264, PIDGET (inferior_ptid), 
				(PTRACE_ARG3_TYPE) &addr, buffer[i]);
		    }
	    }
	    if (errno)
		return 0;
	}
#ifdef CLEAR_INSN_CACHE
	    CLEAR_INSN_CACHE ();
#endif
    }
  else
  {
      /* Read all the longwords.  */
      for (i = 0; i < count; i++, addr += sizeof (PTRACE_XFER_TYPE))
      {
	  errno = 0;
	  if (wordsize == sizeof (PTRACE_XFER_TYPE) && (!arch64))
	  {
	      buffer[i] = ptrace (PT_READ_I, PIDGET (inferior_ptid),
				  (PTRACE_ARG3_TYPE) addr, 0);
	  }
	  else
	  {
	      if (arch64) {
		  buffer[i] = ptrace (PTRACE_PEEKDATA, PIDGET (inferior_ptid),
				      (unsigned long) addr, 0);
	      }
	      else
	      {
		  ptrace (PPC_PTRACE_PEEKDATA_3264, PIDGET (inferior_ptid),
			  (PTRACE_ARG3_TYPE) &addr, &buffer[i]);
	      }
	      if (errno)
		  return 0;
	      QUIT;
	  }

      /* Copy appropriate bytes out of the buffer.  */
	  memcpy (myaddr,
		  (char *) buffer + (memaddr & (sizeof (PTRACE_XFER_TYPE) - 1)),
		  len);
      }
  }
  if (old_chain != NULL)
    do_cleanups (old_chain);
  return len;
    }


/* Did not want to add this originally since the kernel will give us a
   lot of gargabe (and would probably fail if it wasn't for the
   FPU's). But at least you can get the other registers in struct
   pt_regs.  Perhaps we can get the kernels to co-operate. */
static void
udot_info (char *dummy1, int dummy2)
{
  int udot_off;			/* Offset into user struct */
  int udot_val;			/* Value from user struct at udot_off */
  char mess[128];		/* For messages */
  int wordsize = (gdbarch_tdep (current_gdbarch))->wordsize;

  if (!target_has_execution)
    {
      error ("The program is not being run.");
    }

  for (udot_off = 0; udot_off < KERNEL_U_SIZE; udot_off += sizeof (udot_val))
    {
      if ((udot_off % 24) == 0)
	{
	  if (udot_off > 0)
	    {
	      printf_filtered ("\n");
	    }
	  printf_filtered ("%04x:", udot_off);
	}
      if (wordsize != sizeof (PTRACE_XFER_TYPE))
	{
	  PTRACE_XFER_TYPE reg;
	  /* ptrace will place contents in "data" pointer */
	  ptrace (PPC_PTRACE_PEEKUSR_3264, PIDGET (inferior_ptid),
		  (PTRACE_ARG3_TYPE) udot_off, &reg);
	  udot_val = reg;
	}
      else
	udot_val = ptrace (PT_READ_U, PIDGET (inferior_ptid),
			   (PTRACE_ARG3_TYPE) udot_off, 0);
      if (errno != 0)
	{
	  sprintf (mess, "\nreading user struct at offset 0x%x", udot_off);
	  perror_with_name (mess);
	}
      /* Avoid using nonportable (?) "*" in print specs */
      printf_filtered (sizeof (int) == 4 ? " 0x%08x" : " 0x%16x", udot_val);
    }
  printf_filtered ("\n");
}
#endif /* CHILD_XFER_MEMORY */

#include "command.h"
void
_initialize_ppc_linux_nat (void)
{
#ifdef CHILD_XFER_MEMORY
  add_info ("udot", udot_info,
	    "Print contents of kernel ``struct user'' for current child.");
#endif
}
