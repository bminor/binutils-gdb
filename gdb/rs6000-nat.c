/* IBM RS/6000 native-dependent code for GDB, the GNU debugger.
   Copyright 1986, 1987, 1989, 1991, 1992, 1994, 1995, 1996, 1997, 1998
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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "inferior.h"
#include "target.h"
#include "gdbcore.h"
#include "xcoffsolib.h"
#include "symfile.h"
#include "objfiles.h"
#include "libbfd.h"		/* For bfd_cache_lookup (FIXME) */
#include "bfd.h"
#include "gdb-stabs.h"

#include <sys/ptrace.h>
#include <sys/reg.h>

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include <a.out.h>
#include <sys/file.h>
#include "gdb_stat.h"
#include <sys/core.h>
#define __LDINFO_PTRACE32__	/* for __ld_info32 */
#define __LDINFO_PTRACE64__	/* for __ld_info64 */
#include <sys/ldr.h>
#include <sys/systemcfg.h>

/* On AIX4.3+, sys/ldr.h provides different versions of struct ld_info for
   debugging 32-bit and 64-bit processes.  Define a typedef and macros for
   accessing fields in the appropriate structures. */

/* In 32-bit compilation mode (which is the only mode from which ptrace()
   works on 4.3), __ld_info32 is #defined as equivalent to ld_info. */

#ifdef __ld_info32
# define ARCH3264
#endif

/* Return whether the current architecture is 64-bit. */

#ifndef ARCH3264
# define ARCH64() 0
#else
# define ARCH64() (REGISTER_RAW_SIZE (0) == 8)
#endif

/* Union of 32-bit and 64-bit ".reg" core file sections. */

typedef union {
#ifdef ARCH3264
  struct __context64 r64;
#else
  struct mstsave r64;
#endif
  struct mstsave r32;
} CoreRegs;

/* Union of 32-bit and 64-bit versions of ld_info. */

typedef union {
#ifndef ARCH3264
  struct ld_info l32;
  struct ld_info l64;
#else
  struct __ld_info32 l32;
  struct __ld_info64 l64;
#endif
} LdInfo;

/* If compiling with 32-bit and 64-bit debugging capability (e.g. AIX 4.x),
   declare and initialize a variable named VAR suitable for use as the arch64
   parameter to the various LDI_*() macros. */

#ifndef ARCH3264
# define ARCH64_DECL(var)
#else
# define ARCH64_DECL(var) int var = ARCH64 ()
#endif

/* Return LDI's FIELD for a 64-bit process if ARCH64 and for a 32-bit process
   otherwise.  This technique only works for FIELDs with the same data type in
   32-bit and 64-bit versions of ld_info. */

#ifndef ARCH3264
# define LDI_FIELD(ldi, arch64, field) (ldi)->l32.ldinfo_##field
#else
# define LDI_FIELD(ldi, arch64, field) \
  (arch64 ? (ldi)->l64.ldinfo_##field : (ldi)->l32.ldinfo_##field)
#endif

/* Return various LDI fields for a 64-bit process if ARCH64 and for a 32-bit
   process otherwise. */

#define LDI_NEXT(ldi, arch64)		LDI_FIELD(ldi, arch64, next)
#define LDI_FD(ldi, arch64)		LDI_FIELD(ldi, arch64, fd)
#define LDI_FILENAME(ldi, arch64)	LDI_FIELD(ldi, arch64, filename)

extern struct vmap *map_vmap (bfd * bf, bfd * arch);

extern struct target_ops exec_ops;

static void vmap_exec (void);

static void vmap_ldinfo (LdInfo *);

static struct vmap *add_vmap (LdInfo *);

static int objfile_symbol_add (void *);

static void vmap_symtab (struct vmap *);

static void fetch_core_registers (char *, unsigned int, int, CORE_ADDR);

static void exec_one_dummy_insn (void);

extern void
fixup_breakpoints (CORE_ADDR low, CORE_ADDR high, CORE_ADDR delta);

/* Conversion from gdb-to-system special purpose register numbers. */

static int special_regs[] =
{
  IAR,				/* PC_REGNUM    */
  MSR,				/* PS_REGNUM    */
  CR,				/* CR_REGNUM    */
  LR,				/* LR_REGNUM    */
  CTR,				/* CTR_REGNUM   */
  XER,				/* XER_REGNUM   */
  MQ				/* MQ_REGNUM    */
};

/* Call ptrace(REQ, ID, ADDR, DATA, BUF). */

static int
ptrace32 (int req, int id, int *addr, int data, int *buf)
{
  int ret = ptrace (req, id, (int *)addr, data, buf);
#if 0
  printf ("ptrace32 (%d, %d, 0x%x, %08x, 0x%x) = 0x%x\n",
	  req, id, (unsigned int)addr, data, (unsigned int)buf, ret);
#endif
  return ret;
}

/* Call ptracex(REQ, ID, ADDR, DATA, BUF). */

static int
ptrace64 (int req, int id, long long addr, int data, int *buf)
{
#ifdef ARCH3264
  int ret = ptracex (req, id, addr, data, buf);
#else
  int ret = 0;
#endif
#if 0
  printf ("ptrace64 (%d, %d, 0x%llx, %08x, 0x%x) = 0x%x\n",
	  req, id, addr, data, (unsigned int)buf, ret);
#endif
  return ret;
}

/* Fetch register REGNO from the inferior. */

static void
fetch_register (int regno)
{
  int *addr = (int *) &registers[REGISTER_BYTE (regno)];
  int nr;

  /* Retrieved values may be -1, so infer errors from errno. */
  errno = 0;

  /* Floating-point registers. */
  if (regno >= FP0_REGNUM && regno <= FPLAST_REGNUM)
    {
      nr = regno - FP0_REGNUM + FPR0;
      ptrace32 (PT_READ_FPR, inferior_pid, addr, nr, 0);
    }

  /* Bogus register number. */
  else if (regno > LAST_UISA_SP_REGNUM)
    fprintf_unfiltered (gdb_stderr,
			"gdb error: register no %d not implemented.\n",
			regno);

  /* Fixed-point registers. */
  else
    {
      if (regno >= FIRST_UISA_SP_REGNUM)
	nr = special_regs[regno - FIRST_UISA_SP_REGNUM];
      else
	nr = regno;

      if (!ARCH64 ())
	*addr = ptrace32 (PT_READ_GPR, inferior_pid, (int *)nr, 0, 0);
      else
	{
	  /* PT_READ_GPR requires the buffer parameter to point to long long,
	     even if the register is really only 32 bits. */
	  long long buf;
	  ptrace64 (PT_READ_GPR, inferior_pid, nr, 0, (int *)&buf);
	  if (REGISTER_RAW_SIZE (regno) == 8)
	    memcpy (addr, &buf, 8);
	  else
	    *addr = buf;
	}
    }

  if (!errno)
    register_valid[regno] = 1;
  else
    {
#if 0
      /* FIXME: this happens 3 times at the start of each 64-bit program. */
      perror ("ptrace read");
#endif
      errno = 0;
    }
}

/* Store register REGNO back into the inferior. */

static void
store_register (int regno)
{
  int *addr = (int *) &registers[REGISTER_BYTE (regno)];
  int nr;

  /* -1 can be a successful return value, so infer errors from errno. */
  errno = 0;

  /* Floating-point registers. */
  if (regno >= FP0_REGNUM && regno <= FPLAST_REGNUM)
    {
      nr = regno - FP0_REGNUM + FPR0;
      ptrace32 (PT_WRITE_FPR, inferior_pid, addr, nr, 0);
    }

  /* Bogus register number. */
  else if (regno > LAST_UISA_SP_REGNUM)
    {
      if (regno >= NUM_REGS)
	fprintf_unfiltered (gdb_stderr,
			    "gdb error: register no %d not implemented.\n",
			    regno);
    }

  /* Fixed-point registers. */
  else
    {
      if (regno == SP_REGNUM)
	/* Execute one dummy instruction (which is a breakpoint) in inferior
	   process to give kernel a chance to do internal housekeeping.
	   Otherwise the following ptrace(2) calls will mess up user stack
	   since kernel will get confused about the bottom of the stack
	   (%sp). */
	exec_one_dummy_insn ();

      if (regno >= FIRST_UISA_SP_REGNUM)
	nr = special_regs[regno - FIRST_UISA_SP_REGNUM];
      else
	nr = regno;

      if (!ARCH64 ())
	ptrace32 (PT_WRITE_GPR, inferior_pid, (int *)nr, *addr, 0);
      else
	{
	  /* PT_WRITE_GPR requires the buffer parameter to point to an 8-byte
	     area, even if the register is really only 32 bits. */
	  long long buf;
	  if (REGISTER_RAW_SIZE (regno) == 8)
	    memcpy (&buf, addr, 8);
	  else
	    buf = *addr;
	  ptrace64 (PT_WRITE_GPR, inferior_pid, nr, 0, (int *)&buf);
	}
    }

  if (errno)
    {
      perror ("ptrace write");
      errno = 0;
    }
}

/* Read from the inferior all registers if REGNO == -1 and just register
   REGNO otherwise. */

void
fetch_inferior_registers (int regno)
{
  if (regno != -1)
    fetch_register (regno);

  else
    {
      /* read 32 general purpose registers. */
      for (regno = 0; regno < 32; regno++)
	fetch_register (regno);

      /* read general purpose floating point registers. */
      for (regno = FP0_REGNUM; regno <= FPLAST_REGNUM; regno++)
	fetch_register (regno);

      /* read special registers. */
      for (regno = FIRST_UISA_SP_REGNUM; regno <= LAST_UISA_SP_REGNUM; regno++)
	fetch_register (regno);
    }
}

/* Store our register values back into the inferior.
   If REGNO is -1, do this for all registers.
   Otherwise, REGNO specifies which register (so we can save time).  */

void
store_inferior_registers (int regno)
{
  if (regno != -1)
    store_register (regno);

  else
    {
      /* write general purpose registers first! */
      for (regno = GPR0; regno <= GPR31; regno++)
	store_register (regno);

      /* write floating point registers now. */
      for (regno = FP0_REGNUM; regno <= FPLAST_REGNUM; regno++)
	store_register (regno);

      /* write special registers. */

      for (regno = FIRST_UISA_SP_REGNUM; regno <= LAST_UISA_SP_REGNUM; regno++)
	store_register (regno);
    }
}

/* Store in *TO the 32-bit word at 32-bit-aligned ADDR in the child
   process, which is 64-bit if ARCH64 and 32-bit otherwise.  Return
   success. */

static int
read_word (CORE_ADDR from, int *to, int arch64)
{
  /* Retrieved values may be -1, so infer errors from errno. */
  errno = 0;

  if (arch64)
    *to = ptrace64 (PT_READ_I, inferior_pid, from, 0, NULL);
  else
    *to = ptrace32 (PT_READ_I, inferior_pid, (int *)(long) from, 0, NULL);

  return !errno;
}

/* Copy LEN bytes to or from inferior's memory starting at MEMADDR
   to debugger memory starting at MYADDR.  Copy to inferior if
   WRITE is nonzero.

   Returns the length copied, which is either the LEN argument or zero.
   This xfer function does not do partial moves, since child_ops
   doesn't allow memory operations to cross below us in the target stack
   anyway.  */

int
child_xfer_memory (CORE_ADDR memaddr, char *myaddr, int len,
		   int write, struct target_ops *target)
{
  /* Round starting address down to 32-bit word boundary. */
  int mask = sizeof (int) - 1;
  CORE_ADDR addr = memaddr & ~(CORE_ADDR)mask;

  /* Round ending address up to 32-bit word boundary. */
  int count = ((memaddr + len - addr + mask) & ~(CORE_ADDR)mask)
    / sizeof (int);

  /* Allocate word transfer buffer. */
  int *buf = (int *) alloca (count * sizeof (int));

  int arch64 = ARCH64 ();
  int i;

  if (!write)
    {
      /* Retrieve memory a word at a time. */
      for (i = 0; i < count; i++, addr += sizeof (int))
	{
	  if (!read_word (addr, buf + i, arch64))
	    return 0;
	  QUIT;
	}

      /* Copy memory to supplied buffer. */
      addr -= count * sizeof (int);
      memcpy (myaddr, (char *)buf + (memaddr - addr), len);
    }
  else
    {
      /* Fetch leading memory needed for alignment. */
      if (addr < memaddr)
	if (!read_word (addr, buf, arch64))
	  return 0;

      /* Fetch trailing memory needed for alignment. */
      if (addr + count * sizeof (int) > memaddr + len)
	if (!read_word (addr, buf + count - 1, arch64))
	  return 0;

      /* Copy supplied data into memory buffer. */
      memcpy ((char *)buf + (memaddr - addr), myaddr, len);

      /* Store memory one word at a time. */
      for (i = 0, errno = 0; i < count; i++, addr += sizeof (int))
	{
	  if (arch64)
	    ptrace64 (PT_WRITE_D, inferior_pid, addr, buf[i], NULL);
	  else
	    ptrace32 (PT_WRITE_D, inferior_pid, (int *)(long) addr,
		      buf[i], NULL);

	  if (errno)
	    return 0;
	  QUIT;
	}
    }

  return len;
}

/* Execute one dummy breakpoint instruction.  This way we give the kernel
   a chance to do some housekeeping and update inferior's internal data,
   including u_area. */

static void
exec_one_dummy_insn (void)
{
#define	DUMMY_INSN_ADDR	(TEXT_SEGMENT_BASE)+0x200

  char shadow_contents[BREAKPOINT_MAX];		/* Stash old bkpt addr contents */
  int ret, status, pid;
  CORE_ADDR prev_pc;

  /* We plant one dummy breakpoint into DUMMY_INSN_ADDR address. We
     assume that this address will never be executed again by the real
     code. */

  target_insert_breakpoint (DUMMY_INSN_ADDR, shadow_contents);

  /* You might think this could be done with a single ptrace call, and
     you'd be correct for just about every platform I've ever worked
     on.  However, rs6000-ibm-aix4.1.3 seems to have screwed this up --
     the inferior never hits the breakpoint (it's also worth noting
     powerpc-ibm-aix4.1.3 works correctly).  */
  prev_pc = read_pc ();
  write_pc (DUMMY_INSN_ADDR);
  if (ARCH64 ())
    ret = ptrace64 (PT_CONTINUE, inferior_pid, 1, 0, NULL);
  else
    ret = ptrace32 (PT_CONTINUE, inferior_pid, (int *)1, 0, NULL);

  if (ret != 0)
    perror ("pt_continue");

  do
    {
      pid = wait (&status);
    }
  while (pid != inferior_pid);

  write_pc (prev_pc);
  target_remove_breakpoint (DUMMY_INSN_ADDR, shadow_contents);
}

/* Fetch registers from the register section in core bfd. */

static void
fetch_core_registers (char *core_reg_sect, unsigned core_reg_size,
		      int which, CORE_ADDR reg_addr)
{
  CoreRegs *regs;
  double *fprs;
  int arch64, i, size;
  void *gprs, *sprs[7];

  if (which != 0)
    {
      fprintf_unfiltered
	(gdb_stderr,
	 "Gdb error: unknown parameter to fetch_core_registers().\n");
      return;
    }

  arch64 = ARCH64 ();
  regs = (CoreRegs *) core_reg_sect;

  /* Retrieve register pointers. */

  if (arch64)
    {
      gprs = regs->r64.gpr;
      fprs = regs->r64.fpr;
      sprs[0] = &regs->r64.iar;
      sprs[1] = &regs->r64.msr;
      sprs[2] = &regs->r64.cr;
      sprs[3] = &regs->r64.lr;
      sprs[4] = &regs->r64.ctr;
      sprs[5] = &regs->r64.xer;
    }
  else
    {
      gprs = regs->r32.gpr;
      fprs = regs->r32.fpr;
      sprs[0] = &regs->r32.iar;
      sprs[1] = &regs->r32.msr;
      sprs[2] = &regs->r32.cr;
      sprs[3] = &regs->r32.lr;
      sprs[4] = &regs->r32.ctr;
      sprs[5] = &regs->r32.xer;
      sprs[6] = &regs->r32.mq;
    }

  /* Copy from pointers to registers[]. */

  memcpy (registers, gprs, 32 * (arch64 ? 8 : 4));
  memcpy (registers + REGISTER_BYTE (FP0_REGNUM), fprs, 32 * 8);
  for (i = FIRST_UISA_SP_REGNUM; i <= LAST_UISA_SP_REGNUM; i++)
    {
      size = REGISTER_RAW_SIZE (i);
      if (size)
	memcpy (registers + REGISTER_BYTE (i),
		sprs[i - FIRST_UISA_SP_REGNUM], size);
    }
}


/* Copy information about text and data sections from LDI to VP for a 64-bit
   process if ARCH64 and for a 32-bit process otherwise. */

static void
vmap_secs (struct vmap *vp, LdInfo *ldi, int arch64)
{
  if (arch64)
    {
      vp->tstart = (CORE_ADDR) ldi->l64.ldinfo_textorg;
      vp->tend = vp->tstart + ldi->l64.ldinfo_textsize;
      vp->dstart = (CORE_ADDR) ldi->l64.ldinfo_dataorg;
      vp->dend = vp->dstart + ldi->l64.ldinfo_datasize;
    }
  else
    {
      vp->tstart = (unsigned long) ldi->l32.ldinfo_textorg;
      vp->tend = vp->tstart + ldi->l32.ldinfo_textsize;
      vp->dstart = (unsigned long) ldi->l32.ldinfo_dataorg;
      vp->dend = vp->dstart + ldi->l32.ldinfo_datasize;
    }

  /* The run time loader maps the file header in addition to the text
     section and returns a pointer to the header in ldinfo_textorg.
     Adjust the text start address to point to the real start address
     of the text section.  */
  vp->tstart += vp->toffs;
}

/* handle symbol translation on vmapping */

static void
vmap_symtab (struct vmap *vp)
{
  register struct objfile *objfile;
  struct section_offsets *new_offsets;
  int i;

  objfile = vp->objfile;
  if (objfile == NULL)
    {
      /* OK, it's not an objfile we opened ourselves.
         Currently, that can only happen with the exec file, so
         relocate the symbols for the symfile.  */
      if (symfile_objfile == NULL)
	return;
      objfile = symfile_objfile;
    }
  else if (!vp->loaded)
    /* If symbols are not yet loaded, offsets are not yet valid. */
    return;

  new_offsets = (struct section_offsets *) alloca (SIZEOF_SECTION_OFFSETS);

  for (i = 0; i < objfile->num_sections; ++i)
    new_offsets->offsets[i] = ANOFFSET (objfile->section_offsets, i);

  /* The symbols in the object file are linked to the VMA of the section,
     relocate them VMA relative.  */
  new_offsets->offsets[SECT_OFF_TEXT (objfile)] = vp->tstart - vp->tvma;
  new_offsets->offsets[SECT_OFF_DATA (objfile)] = vp->dstart - vp->dvma;
  new_offsets->offsets[SECT_OFF_BSS (objfile)] = vp->dstart - vp->dvma;

  objfile_relocate (objfile, new_offsets);
}

/* Add symbols for an objfile.  */

static int
objfile_symbol_add (void *arg)
{
  struct objfile *obj = (struct objfile *) arg;

  syms_from_objfile (obj, NULL, 0, 0);
  new_symfile_objfile (obj, 0, 0);
  return 1;
}

/* Add symbols for a vmap. Return zero upon error.  */

int
vmap_add_symbols (struct vmap *vp)
{
  if (catch_errors (objfile_symbol_add, vp->objfile,
		    "Error while reading shared library symbols:\n",
		    RETURN_MASK_ALL))
    {
      /* Note this is only done if symbol reading was successful.  */
      vp->loaded = 1;
      vmap_symtab (vp);
      return 1;
    }
  return 0;
}

/* Add a new vmap entry based on ldinfo() information.

   If ldi->ldinfo_fd is not valid (e.g. this struct ld_info is from a
   core file), the caller should set it to -1, and we will open the file.

   Return the vmap new entry.  */

static struct vmap *
add_vmap (LdInfo *ldi)
{
  bfd *abfd, *last;
  register char *mem, *objname, *filename;
  struct objfile *obj;
  struct vmap *vp;
  int fd;
  ARCH64_DECL (arch64);

  /* This ldi structure was allocated using alloca() in 
     xcoff_relocate_symtab(). Now we need to have persistent object 
     and member names, so we should save them. */

  filename = LDI_FILENAME (ldi, arch64);
  mem = filename + strlen (filename) + 1;
  mem = savestring (mem, strlen (mem));
  objname = savestring (filename, strlen (filename));

  fd = LDI_FD (ldi, arch64);
  if (fd < 0)
    /* Note that this opens it once for every member; a possible
       enhancement would be to only open it once for every object.  */
    abfd = bfd_openr (objname, gnutarget);
  else
    abfd = bfd_fdopenr (objname, gnutarget, fd);
  if (!abfd)
    {
      warning ("Could not open `%s' as an executable file: %s",
	       objname, bfd_errmsg (bfd_get_error ()));
      return NULL;
    }

  /* make sure we have an object file */

  if (bfd_check_format (abfd, bfd_object))
    vp = map_vmap (abfd, 0);

  else if (bfd_check_format (abfd, bfd_archive))
    {
      last = 0;
      /* FIXME??? am I tossing BFDs?  bfd? */
      while ((last = bfd_openr_next_archived_file (abfd, last)))
	if (STREQ (mem, last->filename))
	  break;

      if (!last)
	{
	  warning ("\"%s\": member \"%s\" missing.", objname, mem);
	  bfd_close (abfd);
	  return NULL;
	}

      if (!bfd_check_format (last, bfd_object))
	{
	  warning ("\"%s\": member \"%s\" not in executable format: %s.",
		   objname, mem, bfd_errmsg (bfd_get_error ()));
	  bfd_close (last);
	  bfd_close (abfd);
	  return NULL;
	}

      vp = map_vmap (last, abfd);
    }
  else
    {
      warning ("\"%s\": not in executable format: %s.",
	       objname, bfd_errmsg (bfd_get_error ()));
      bfd_close (abfd);
      return NULL;
    }
  obj = allocate_objfile (vp->bfd, 0);
  vp->objfile = obj;

  /* Always add symbols for the main objfile.  */
  if (vp == vmap || auto_solib_add)
    vmap_add_symbols (vp);
  return vp;
}

/* update VMAP info with ldinfo() information
   Input is ptr to ldinfo() results.  */

static void
vmap_ldinfo (LdInfo *ldi)
{
  struct stat ii, vi;
  register struct vmap *vp;
  int got_one, retried;
  int got_exec_file = 0;
  uint next;
  int arch64 = ARCH64 ();

  /* For each *ldi, see if we have a corresponding *vp.
     If so, update the mapping, and symbol table.
     If not, add an entry and symbol table.  */

  do
    {
      char *name = LDI_FILENAME (ldi, arch64);
      char *memb = name + strlen (name) + 1;
      int fd = LDI_FD (ldi, arch64);

      retried = 0;

      if (fstat (fd, &ii) < 0)
	{
	  /* The kernel sets ld_info to -1, if the process is still using the
	     object, and the object is removed. Keep the symbol info for the
	     removed object and issue a warning.  */
	  warning ("%s (fd=%d) has disappeared, keeping its symbols",
		   name, fd);
	  continue;
	}
    retry:
      for (got_one = 0, vp = vmap; vp; vp = vp->nxt)
	{
	  struct objfile *objfile;

	  /* First try to find a `vp', which is the same as in ldinfo.
	     If not the same, just continue and grep the next `vp'. If same,
	     relocate its tstart, tend, dstart, dend values. If no such `vp'
	     found, get out of this for loop, add this ldi entry as a new vmap
	     (add_vmap) and come back, find its `vp' and so on... */

	  /* The filenames are not always sufficient to match on. */

	  if ((name[0] == '/' && !STREQ (name, vp->name))
	      || (memb[0] && !STREQ (memb, vp->member)))
	    continue;

	  /* See if we are referring to the same file.
	     We have to check objfile->obfd, symfile.c:reread_symbols might
	     have updated the obfd after a change.  */
	  objfile = vp->objfile == NULL ? symfile_objfile : vp->objfile;
	  if (objfile == NULL
	      || objfile->obfd == NULL
	      || bfd_stat (objfile->obfd, &vi) < 0)
	    {
	      warning ("Unable to stat %s, keeping its symbols", name);
	      continue;
	    }

	  if (ii.st_dev != vi.st_dev || ii.st_ino != vi.st_ino)
	    continue;

	  if (!retried)
	    close (fd);

	  ++got_one;

	  /* Found a corresponding VMAP.  Remap!  */

	  vmap_secs (vp, ldi, arch64);

	  /* The objfile is only NULL for the exec file.  */
	  if (vp->objfile == NULL)
	    got_exec_file = 1;

	  /* relocate symbol table(s). */
	  vmap_symtab (vp);

	  /* There may be more, so we don't break out of the loop.  */
	}

      /* if there was no matching *vp, we must perforce create the sucker(s) */
      if (!got_one && !retried)
	{
	  add_vmap (ldi);
	  ++retried;
	  goto retry;
	}
    }
  while ((next = LDI_NEXT (ldi, arch64))
	 && (ldi = (void *) (next + (char *) ldi)));

  /* If we don't find the symfile_objfile anywhere in the ldinfo, it
     is unlikely that the symbol file is relocated to the proper
     address.  And we might have attached to a process which is
     running a different copy of the same executable.  */
  if (symfile_objfile != NULL && !got_exec_file)
    {
      warning_begin ();
      fputs_unfiltered ("Symbol file ", gdb_stderr);
      fputs_unfiltered (symfile_objfile->name, gdb_stderr);
      fputs_unfiltered ("\nis not mapped; discarding it.\n\
If in fact that file has symbols which the mapped files listed by\n\
\"info files\" lack, you can load symbols with the \"symbol-file\" or\n\
\"add-symbol-file\" commands (note that you must take care of relocating\n\
symbols to the proper address).\n", gdb_stderr);
      free_objfile (symfile_objfile);
      symfile_objfile = NULL;
    }
  breakpoint_re_set ();
}

/* As well as symbol tables, exec_sections need relocation. After
   the inferior process' termination, there will be a relocated symbol
   table exist with no corresponding inferior process. At that time, we
   need to use `exec' bfd, rather than the inferior process's memory space
   to look up symbols.

   `exec_sections' need to be relocated only once, as long as the exec
   file remains unchanged.
 */

static void
vmap_exec (void)
{
  static bfd *execbfd;
  int i;

  if (execbfd == exec_bfd)
    return;

  execbfd = exec_bfd;

  if (!vmap || !exec_ops.to_sections)
    error ("vmap_exec: vmap or exec_ops.to_sections == 0\n");

  for (i = 0; &exec_ops.to_sections[i] < exec_ops.to_sections_end; i++)
    {
      if (STREQ (".text", exec_ops.to_sections[i].the_bfd_section->name))
	{
	  exec_ops.to_sections[i].addr += vmap->tstart - vmap->tvma;
	  exec_ops.to_sections[i].endaddr += vmap->tstart - vmap->tvma;
	}
      else if (STREQ (".data", exec_ops.to_sections[i].the_bfd_section->name))
	{
	  exec_ops.to_sections[i].addr += vmap->dstart - vmap->dvma;
	  exec_ops.to_sections[i].endaddr += vmap->dstart - vmap->dvma;
	}
      else if (STREQ (".bss", exec_ops.to_sections[i].the_bfd_section->name))
	{
	  exec_ops.to_sections[i].addr += vmap->dstart - vmap->dvma;
	  exec_ops.to_sections[i].endaddr += vmap->dstart - vmap->dvma;
	}
    }
}

/* Set the current architecture from the host running GDB.  Called when
   starting a child process. */

static void
set_host_arch (int pid)
{
  enum bfd_architecture arch;
  unsigned long mach;
  bfd abfd;
  struct gdbarch_info info;

  if (__power_rs ())
    {
      arch = bfd_arch_rs6000;
      mach = bfd_mach_rs6k;
    }
  else
    {
      arch = bfd_arch_powerpc;
      mach = bfd_mach_ppc;
    }
  bfd_default_set_arch_mach (&abfd, arch, mach);

  memset (&info, 0, sizeof info);
  info.bfd_arch_info = bfd_get_arch_info (&abfd);

  if (!gdbarch_update_p (info))
    {
      internal_error ("set_host_arch: failed to select architecture");
    }
}


/* xcoff_relocate_symtab -      hook for symbol table relocation.
   also reads shared libraries.. */

void
xcoff_relocate_symtab (unsigned int pid)
{
  int load_segs = 64; /* number of load segments */
  int rc;
  LdInfo *ldi = NULL;
  int arch64 = ARCH64 ();
  int ldisize = arch64 ? sizeof (ldi->l64) : sizeof (ldi->l32);
  int size;

  do
    {
      size = load_segs * ldisize;
      ldi = (void *) xrealloc (ldi, size);

#if 0
      /* According to my humble theory, AIX has some timing problems and
         when the user stack grows, kernel doesn't update stack info in time
         and ptrace calls step on user stack. That is why we sleep here a
         little, and give kernel to update its internals. */
      usleep (36000);
#endif

      if (arch64)
	rc = ptrace64 (PT_LDINFO, pid, (unsigned long) ldi, size, NULL);
      else
	rc = ptrace32 (PT_LDINFO, pid, (int *) ldi, size, NULL);

      if (rc == -1)
        {
          if (errno == ENOMEM)
            load_segs *= 2;
          else
            perror_with_name ("ptrace ldinfo");
        }
      else
	{
          vmap_ldinfo (ldi);
          vmap_exec (); /* relocate the exec and core sections as well. */
	}
    } while (rc == -1);
  if (ldi)
    free (ldi);
}

/* Core file stuff.  */

/* Relocate symtabs and read in shared library info, based on symbols
   from the core file.  */

void
xcoff_relocate_core (struct target_ops *target)
{
  sec_ptr ldinfo_sec;
  int offset = 0;
  LdInfo *ldi;
  struct vmap *vp;
  int arch64 = ARCH64 ();

  /* Size of a struct ld_info except for the variable-length filename. */
  int nonfilesz = (int)LDI_FILENAME ((LdInfo *)0, arch64);

  /* Allocated size of buffer.  */
  int buffer_size = nonfilesz;
  char *buffer = xmalloc (buffer_size);
  struct cleanup *old = make_cleanup (free_current_contents, &buffer);

  ldinfo_sec = bfd_get_section_by_name (core_bfd, ".ldinfo");
  if (ldinfo_sec == NULL)
    {
    bfd_err:
      fprintf_filtered (gdb_stderr, "Couldn't get ldinfo from core file: %s\n",
			bfd_errmsg (bfd_get_error ()));
      do_cleanups (old);
      return;
    }
  do
    {
      int i;
      int names_found = 0;

      /* Read in everything but the name.  */
      if (bfd_get_section_contents (core_bfd, ldinfo_sec, buffer,
				    offset, nonfilesz) == 0)
	goto bfd_err;

      /* Now the name.  */
      i = nonfilesz;
      do
	{
	  if (i == buffer_size)
	    {
	      buffer_size *= 2;
	      buffer = xrealloc (buffer, buffer_size);
	    }
	  if (bfd_get_section_contents (core_bfd, ldinfo_sec, &buffer[i],
					offset + i, 1) == 0)
	    goto bfd_err;
	  if (buffer[i++] == '\0')
	    ++names_found;
	}
      while (names_found < 2);

      ldi = (LdInfo *) buffer;

      /* Can't use a file descriptor from the core file; need to open it.  */
      if (arch64)
	ldi->l64.ldinfo_fd = -1;
      else
	ldi->l32.ldinfo_fd = -1;

      /* The first ldinfo is for the exec file, allocated elsewhere.  */
      if (offset == 0 && vmap != NULL)
	vp = vmap;
      else
	vp = add_vmap (ldi);

      /* Process next shared library upon error. */
      offset += LDI_NEXT (ldi, arch64);
      if (vp == NULL)
	continue;

      vmap_secs (vp, ldi, arch64);

      /* Unless this is the exec file,
         add our sections to the section table for the core target.  */
      if (vp != vmap)
	{
	  struct section_table *stp;

	  target_resize_to_sections (target, 2);
	  stp = target->to_sections_end - 2;

	  stp->bfd = vp->bfd;
	  stp->the_bfd_section = bfd_get_section_by_name (stp->bfd, ".text");
	  stp->addr = vp->tstart;
	  stp->endaddr = vp->tend;
	  stp++;

	  stp->bfd = vp->bfd;
	  stp->the_bfd_section = bfd_get_section_by_name (stp->bfd, ".data");
	  stp->addr = vp->dstart;
	  stp->endaddr = vp->dend;
	}

      vmap_symtab (vp);
    }
  while (LDI_NEXT (ldi, arch64) != 0);
  vmap_exec ();
  breakpoint_re_set ();
  do_cleanups (old);
}

int
kernel_u_size (void)
{
  return (sizeof (struct user));
}

/* Under AIX, we have to pass the correct TOC pointer to a function
   when calling functions in the inferior.
   We try to find the relative toc offset of the objfile containing PC
   and add the current load address of the data segment from the vmap.  */

static CORE_ADDR
find_toc_address (CORE_ADDR pc)
{
  struct vmap *vp;
  extern CORE_ADDR get_toc_offset (struct objfile *);	/* xcoffread.c */

  for (vp = vmap; vp; vp = vp->nxt)
    {
      if (pc >= vp->tstart && pc < vp->tend)
	{
	  /* vp->objfile is only NULL for the exec file.  */
	  return vp->dstart + get_toc_offset (vp->objfile == NULL
					      ? symfile_objfile
					      : vp->objfile);
	}
    }
  error ("Unable to find TOC entry for pc 0x%x\n", pc);
}

/* Register that we are able to handle rs6000 core file formats. */

static struct core_fns rs6000_core_fns =
{
  bfd_target_xcoff_flavour,		/* core_flavour */
  default_check_format,			/* check_format */
  default_core_sniffer,			/* core_sniffer */
  fetch_core_registers,			/* core_read_registers */
  NULL					/* next */
};

void
_initialize_core_rs6000 (void)
{
  /* Initialize hook in rs6000-tdep.c for determining the TOC address when
     calling functions in the inferior.  */
  rs6000_find_toc_address_hook = find_toc_address;

  /* Initialize hook in rs6000-tdep.c to set the current architecture when
     starting a child process. */
  rs6000_set_host_arch_hook = set_host_arch;

  add_core_fns (&rs6000_core_fns);
}
