/* Target-dependent code for GNU/Linux LoongArch.

   Copyright (C) 2021 Free Software Foundation, Inc.
   Contributed by Loongson Ltd.

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
#include "inferior.h"
#include "gdbcore.h"
#include "target.h"
#include "solib-svr4.h"
#include "osabi.h"
#include "loongarch-tdep.h"
#include "frame.h"
#include "regcache.h"
#include "trad-frame.h"
#include "tramp-frame.h"
#include "gdbtypes.h"
#include "objfiles.h"
#include "solib.h"
#include "solist.h"
#include "symtab.h"
#include "target-descriptions.h"
#include "loongarch-linux-tdep.h"
#include "glibc-tdep.h"
#include "linux-tdep.h"
#include "xml-syscall.h"
#include "gdbsupport/gdb_signals.h"

static void
loongarch_supply_elf_gregset (const struct regset *r,
			      struct regcache *regcache, int regno,
			      const void *gprs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->r && sizeof (loongarch_elf_gregset_t) <= len);

  int regsize = register_size (regcache->arch (), regs->r);
  const gdb_byte *buf = NULL;

  if (regno == -1)
    {
      /* Set $r0 = 0.  */
      regcache->raw_supply_zeroed (regs->r);

      for (int i = 1; i < 32; i++)
	{
	buf = (const gdb_byte*)gprs + regsize * i;
	regcache->raw_supply (regs->r + i, (const void *)buf);
	}

      /* Size base (pc) = regsize * regs->pc.  */
      buf = (const gdb_byte*)gprs + regsize * regs->pc;
      regcache->raw_supply (regs->pc, (const void *)buf);

      /* Size base (badvaddr) = regsize * regs->badvaddr.  */
      buf = (const gdb_byte*)gprs + regsize * regs->badvaddr;
      regcache->raw_supply (regs->badvaddr, (const void *)buf);
    }
  else if (regs->r == regno)
    regcache->raw_supply_zeroed (regno);
  else if ((regs->r < regno && regno < regs->r + 32)
	  || (regs->pc == regno)
	  || (regs->badvaddr == regno))
    {
    /* Offset offset (regno) = regsize * (regno - regs->r).  */
    buf = (const gdb_byte*)gprs + regsize * (regno - regs->r);
    regcache->raw_supply (regno, (const void *)buf);
    }
}

static void
loongarch_fill_elf_gregset (const struct regset *r,
			    const struct regcache *regcache, int regno,
			    void *gprs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->r && sizeof (loongarch_elf_gregset_t) <= len);
  int regsize = register_size (regcache->arch (), regs->r);
  gdb_byte *buf = NULL;

  if (regno == -1)
    {
      for (int i = 0; i < 32; i++)
	{
	buf = (gdb_byte *)gprs + regsize * i;
	regcache->raw_collect (regs->r + i, (void *)buf);
	}

      /* Size base (pc) = regsize * regs->pc.  */
      buf = (gdb_byte *)gprs + regsize * regs->pc;
      regcache->raw_collect (regs->pc, (void *)buf);

      /* Size base (badvaddr) = regsize * regs->badvaddr.  */
      buf = (gdb_byte *)gprs + regsize * regs->badvaddr;
      regcache->raw_collect (regs->badvaddr, (void *)buf);
    }
  else if ((regs->r <= regno && regno < regs->r + 32)
	  ||(regs->pc == regno)
	  ||(regs->badvaddr == regno))
    {
    /* Offset offset (regno) = regsize * (regno - regs->r).  */
    buf = (gdb_byte *)gprs + regsize * (regno - regs->r);
    regcache->raw_collect (regno, (void *)buf);
    }
}

const struct regset loongarch_elf_gregset =
{
  NULL,
  loongarch_supply_elf_gregset,
  loongarch_fill_elf_gregset,
};

static void
loongarch_supply_elf_fpregset (const struct regset *r,
			       struct regcache *regcache, int regno,
			       const void *fprs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->f && sizeof (loongarch_elf_fpregset_t) <= len);

  const gdb_byte *buf = NULL;
  int fprsize = register_size (regcache->arch (), regs->f);
  int fccsize = register_size (regcache->arch (), regs->fcc);

  if (regno == -1)
    {
    /* 32 fprs.  */
    for (int i = 0; i < 32; i++)
      {
      buf = (const gdb_byte *)fprs + fprsize * i;
      regcache->raw_supply (regs->f + i, (const void *)buf);
      }

    /* 8 fccs , base (fcc) = 32 * sizeof (fpr).  */
    buf = (const gdb_byte *)fprs + fprsize * 32;
    for (int i = 0; i < 8; i++)
      {
      regcache->raw_supply (regs->fcc + i, (const void *)buf);
      buf += fccsize;
      }

    /* Size base (fcsr) = 32 * sizeof (fpr) + 8 * sizeof (fcc).  */
    buf = (const gdb_byte *)fprs + 32 * fprsize + 8 * fccsize;
    regno = regs->fcsr;
    }
  else if (regs->f <= regno && regno < regs->f + 32)
    {
    /* Offset offset (regno - f) = (regno - regs->f) * sizeof (fpr).  */
    buf = (const gdb_byte *)fprs + fprsize * (regno - regs->f);
    }
  else if (regs->fcc <= regno && regno < regs->fcc + 8)
    {
    /* Size base (fcc) + offset (regno - fcc)
       = 32 * sizeof (fpr) + (regno - regs->fcc) * sizeof (fcc).  */
    buf = (const gdb_byte *)fprs + 32 * fprsize
	  + (regno - regs->fcc) * fccsize;
    }
  else if (regs->fcsr == regno)
    {
    /* Size base (fcsr) = 32 * sizeof (fpr) + 8 * sizeof (fcc).  */
    buf = (const gdb_byte *)fprs + 32 * fprsize + 8 * fccsize;
    }
  else
    {
    return;
    }

    /* Supply register.  */
    regcache->raw_supply (regno, (const void *)buf);
}

static void
loongarch_fill_elf_fpregset (const struct regset *r,
			     const struct regcache *regcache, int regno,
			     void *fprs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->f && sizeof (loongarch_elf_fpregset_t) <= len);
  gdb_byte *buf = NULL;
  int fprsize = register_size (regcache->arch (), regs->f);
  int fccsize = register_size (regcache->arch (), regs->fcc);

  if (regno == -1)
    {
    /* 32 fprs.  */
    for (int i = 0; i < 32; i++)
      {
      buf = (gdb_byte *)fprs + fprsize * i;
      regcache->raw_collect (regs->f + i, (void *)buf);
      }

      /* 8 fccs , base (fcc) = 32 * sizeof (fpr).  */
      buf = (gdb_byte *)fprs + fprsize * 32;
      for (int i = 0; i < 8; i++)
	{
	regcache->raw_collect (regs->fcc + i, (void *)buf);
	buf += fccsize;
	}

      /* Size base (fcsr) = 32 * sizeof (fpr) + 8 * sizeof (fcc).  */
      buf = (gdb_byte *)fprs + fprsize * 32 + fccsize * 8;
      regno = regs->fcsr;
    }
  else if (regs->f <= regno && regno < regs->f + 32)
    {
    /* Offset offset (regno - f) = (regno - regs->f) * sizeof (fpr).  */
    buf = (gdb_byte *)fprs + fprsize * (regno - regs->f);
    }
  else if (regs->fcc <= regno && regno < regs->fcc + 8)
    {
    /* Size base (fcc) + offset (regno - fcc)
       = 32 * sizeof (fpr) + (regno - regs->fcc) * sizeof (fcc).  */
    buf = (gdb_byte *)fprs + 32 * fprsize + (regno - regs->fcc) * fccsize;
    }
  else if (regs->fcsr == regno)
    {
    /* Size base (fcsr) = 32 * sizeof (fpr) + 8 * sizeof (fcc).  */
    buf = (gdb_byte *)fprs + 32 * fprsize + 8 * fccsize;
    }
  else
    {
    return;
    }

    /* Supply register.  */
    regcache->raw_collect (regno, (void *)buf);
}

const struct regset loongarch_elf_fpregset =
{
  NULL,
  loongarch_supply_elf_fpregset,
  loongarch_fill_elf_fpregset,
};

static void
loongarch_supply_elf_cpucfgregset (const struct regset *r,
				   struct regcache *regcache, int regno,
				   const void *cpucfgs, size_t len)
{
}

static void
loongarch_fill_elf_cpucfgregset (const struct regset *r,
				 const struct regcache *regcache, int regno,
				 void *cpucfgs, size_t len)
{
  ULONGEST xfered_len;
  target_xfer_partial (current_inferior ()->top_target (),
		       /* current_top_target (),*/ TARGET_OBJECT_LARCH,
		       "cpucfg", (gdb_byte *) cpucfgs, NULL, 0, len,
		       &xfered_len);
  memset ((gdb_byte *) cpucfgs + xfered_len, 0, len - xfered_len);
}

const struct regset loongarch_elf_cpucfgregset =
{
  NULL,
  loongarch_supply_elf_cpucfgregset,
  loongarch_fill_elf_cpucfgregset,
};

static void
loongarch_supply_elf_lbtregset (const struct regset *r,
				struct regcache *regcache, int regno,
				const void *lbtrs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->scr && sizeof (loongarch_elf_lbtregset_t) <= len);

  const gdb_byte *buf = NULL;
  int scrsize = register_size (regcache->arch (), regs->scr);
  int efsize = register_size (regcache->arch (), regs->EFLAG);

  if (regno == -1)
    {
    /* 4 scrs.  */
    for (int i = 0; i < 4; i++)
      {
      buf = (const gdb_byte *)lbtrs + scrsize * i;
      regcache->raw_supply (regs->scr + i, (const void *)buf);
      }

      /* Size base (EFLAG) = 4 * sizeof (scr).  */
      buf = (const gdb_byte *)lbtrs + scrsize * 4;
      regcache->raw_supply (regs->EFLAG, (const void *)buf);

      /* Size base (x86_top) = 4 * sizeof (scr) + sizeof (EFLAG).  */
      buf = (const gdb_byte *)lbtrs + scrsize * 4 + efsize;
      regno = regs->x86_top;
    }
  else if (regs->scr <= regno && regno < regs->scr + 4)
    /* Offset offset (EFLAG) = sizeof (scr) * (regno - regs->scr).  */
    buf = (const gdb_byte *)lbtrs + scrsize * (regno - regs->scr);
  else if (regs->EFLAG == regno)
    /* Size base (EFLAG) = 4 * sizeof (scr).  */
    buf = (const gdb_byte *)lbtrs + scrsize * 4;
  else if (regs->x86_top == regno)
    {
    /* Size base (x86_top) = 4 * sizeof (scr) + sizeof (EFLAG).  */
    buf = (const gdb_byte *)lbtrs + scrsize * 4 + efsize;
    }
  else
    {
    return;
    }

  regcache->raw_supply (regno, (const void *)buf);
}

static void
loongarch_fill_elf_lbtregset (const struct regset *r,
			      const struct regcache *regcache, int regno,
			      void *lbtrs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->scr && sizeof (loongarch_elf_lbtregset_t) <= len);

  gdb_byte *buf = NULL;
  int scrsize = register_size (regcache->arch (), regs->scr);
  int efsize = register_size (regcache->arch (), regs->EFLAG);

  if (regno == -1)
    {

    /* 4 scrs.  */
    for (int i = 0; i < 4; i++)
      {
      buf = (gdb_byte *)lbtrs + scrsize * i;
      regcache->raw_collect (regs->scr + i, (void *)buf);
      }

      /* Size base (EFLAG) = 4 * sizeof (scr).  */
      buf = (gdb_byte *)lbtrs + scrsize * 4;
      regcache->raw_collect (regs->EFLAG, (void *)buf);

      /* Size base (x86_top) = 4 * sizeof (scr) + sizeof (EFLAG).  */
      buf = (gdb_byte *)lbtrs + scrsize * 4 + efsize;
      regno = regs->x86_top;
    }
  else if (regs->scr <= regno && regno < regs->scr + 4)
    /* Offset offset (EFLAG) = sizeof (scr) * (regno - regs->scr).  */
    buf = (gdb_byte *)lbtrs + scrsize * (regno - regs->scr);
  else if (regs->EFLAG == regno)
    /* Size base (EFLAG) = 4 * sizeof (scr).  */
    buf = (gdb_byte *)lbtrs + scrsize * 4;
  else if (regs->x86_top == regno)
    /* Size base (x86_top) = 4 * sizeof (scr) + sizeof (EFLAG).  */
    buf = (gdb_byte *)lbtrs + scrsize * 4 + efsize;
  else
    {
    return;
    }

  regcache->raw_collect (regno, (void *)buf);
}

const struct regset loongarch_elf_lbtregset =
{
  NULL,
  loongarch_supply_elf_lbtregset,
  loongarch_fill_elf_lbtregset,
};

static void
loongarch_supply_elf_lsxregset (const struct regset *r,
				struct regcache *regcache, int regno,
				const void *lsxrs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->vr && sizeof (loongarch_elf_lsxregset_t) <= len);

  const gdb_byte *buf = NULL;
  int regsize = register_size (regcache->arch (), regs->vr);

  if (regno == -1)
    {
    for (int i = 0; i < 32; i++)
      {
      buf = (const gdb_byte *)lsxrs + regsize * i;
      regcache->raw_supply (regs->vr + i, (const void *)buf);
      }
    }
  else if (regs->vr <= regno && regno < regs->vr + 32)
    {
      buf = (const gdb_byte *)lsxrs + regsize * (regno - regs->vr);
      regcache->raw_supply (regno, (const void *)buf);
    }
}

static void
loongarch_fill_elf_lsxregset (const struct regset *r,
			      const struct regcache *regcache, int regno,
			      void *lsxrs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->vr && sizeof (loongarch_elf_lsxregset_t) <= len);

  gdb_byte *buf = NULL;
  int regsize = register_size (regcache->arch (), regs->vr);

  if (regno == -1)
    {
    for (int i = 0; i < 32; i++)
      {
      buf = (gdb_byte *)lsxrs + regsize * i;
      regcache->raw_collect (regs->vr + i, (void *)buf);
      }
    }
  else if (regs->vr <= regno && regno < regs->vr + 32)
    {
    buf = (gdb_byte *)lsxrs + regsize * (regno - regs->vr);
    regcache->raw_collect (regno, (void *)buf);
    }
}

const struct regset loongarch_elf_lsxregset =
{
  NULL,
  loongarch_supply_elf_lsxregset,
  loongarch_fill_elf_lsxregset,
};

static void
loongarch_supply_elf_lasxregset (const struct regset *r,
				 struct regcache *regcache, int regno,
				 const void *lasxrs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->xr && sizeof (loongarch_elf_lasxregset_t) <= len);

  const gdb_byte *buf = NULL;
  int regsize = register_size (regcache->arch (), regs->xr);

  if (regno == -1)
    {
    for (int i = 0; i < 32; i++)
      {
      buf = (const gdb_byte *)lasxrs + regsize * i;
      regcache->raw_supply (regs->xr + i, (const void *)buf);
      }
    }
  else if (regs->xr <= regno && regno < regs->xr + 32)
    {
    buf = (const gdb_byte *)lasxrs + regsize * (regno - regs->xr);
    regcache->raw_supply (regno, (const void *)buf);
    }
}

static void
loongarch_fill_elf_lasxregset (const struct regset *r,
			       const struct regcache *regcache, int regno,
			       void *lasxrs, size_t len)
{
  auto regs = &gdbarch_tdep (regcache->arch ())->regs;
  gdb_assert (0 <= regs->xr && sizeof (loongarch_elf_lasxregset_t) <= len);

  gdb_byte *buf = NULL;
  int regsize = register_size (regcache->arch (), regs->xr);

  if (regno == -1)
    {
    for (int i = 0; i < 32; i++)
      {
      buf = (gdb_byte *)lasxrs + regsize * i;
      regcache->raw_collect (regs->xr + i, (void *)buf);
      }
    }
  else if (regs->xr <= regno && regno < regs->xr + 32)
    {
    buf = (gdb_byte *)lasxrs + regsize * (regno - regs->xr);
    regcache->raw_collect (regno, (void *)buf);
    }
}

const struct regset loongarch_elf_lasxregset =
{
  NULL,
  loongarch_supply_elf_lasxregset,
  loongarch_fill_elf_lasxregset,
};

static void
loongarch_linux_iterate_over_regset_sections (
  struct gdbarch *gdbarch, iterate_over_regset_sections_cb *cb, void *cb_data,
  const struct regcache *regcache)
{
  auto regs = &gdbarch_tdep (gdbarch)->regs;
  if (0 <= regs->r)
    cb (".reg", sizeof (loongarch_elf_gregset_t),
	sizeof (loongarch_elf_gregset_t), &loongarch_elf_gregset, NULL,
	cb_data);
  if (0 <= regs->f)
    cb (".reg2", sizeof (loongarch_elf_fpregset_t),
	sizeof (loongarch_elf_fpregset_t), &loongarch_elf_fpregset, NULL,
	cb_data);
  do
    {
      uint32_t t;
      ULONGEST xfered_len;
      if (target_xfer_partial (current_inferior ()->top_target (),
			       /* current_top_target (),*/ TARGET_OBJECT_LARCH,
			       "cpucfg", (gdb_byte *) &t, NULL, 0, sizeof (t),
			       &xfered_len) != TARGET_XFER_OK)
	break;
      cb (".reg-loongarch-cpucfg", 64 * 4, 64 * 4, &loongarch_elf_cpucfgregset,
	  "LoongArch CPU config", cb_data);
    }
  while (0);
  if (0 <= regs->scr)
    cb (".reg-loongarch-lbt", sizeof (loongarch_elf_lbtregset_t),
	sizeof (loongarch_elf_lbtregset_t), &loongarch_elf_lbtregset,
	"LoongArch Binary Translation", cb_data);
  if (0 <= regs->vr)
    cb (".reg-loongarch-lsx", sizeof (loongarch_elf_lsxregset_t),
	sizeof (loongarch_elf_lsxregset_t), &loongarch_elf_lsxregset,
	"LoongArch SIMD Extension", cb_data);
  if (0 <= regs->xr)
    cb (".reg-loongarch-lasx", sizeof (loongarch_elf_lasxregset_t),
	sizeof (loongarch_elf_lasxregset_t), &loongarch_elf_lasxregset,
	"LoongArch Advanced SIMD Extension", cb_data);
}

static const struct target_desc *
loongarch_linux_core_read_description (struct gdbarch *gdbarch,
				       struct target_ops *target, bfd *abfd)
{
  int rlen, fpu32, fpu64, lbt, lsx, lasx;

  rlen = 64;
  fpu32 = 0;

  fpu64 = !!bfd_get_section_by_name (abfd, ".reg2");
  lbt = !!bfd_get_section_by_name (abfd, ".reg-loongarch-lbt");
  lsx = !!bfd_get_section_by_name (abfd, ".reg-loongarch-lsx");
  lasx = !!bfd_get_section_by_name (abfd, ".reg-loongarch-lasx");

  return loongarch_create_target_description (rlen, fpu32, fpu64, lbt, lsx,
					      lasx);
}

static void
loongarch_linux_lp64_sigframe_init (const struct tramp_frame *self,
				    struct frame_info *this_frame,
				    struct trad_frame_cache *this_cache,
				    CORE_ADDR func)
{
  struct gdbarch *gdbarch = get_frame_arch (this_frame);
  auto regs = &gdbarch_tdep (gdbarch)->regs;
  CORE_ADDR frame_sp = get_frame_sp (this_frame);

  CORE_ADDR sigcontext_base = frame_sp + 224;
  int i;

  trad_frame_set_reg_addr (this_cache, regs->pc, sigcontext_base);
  for (i = 0; i < 32; i++)
    trad_frame_set_reg_addr (this_cache, regs->r + i,
			     sigcontext_base + 8 + i * 8);

  trad_frame_set_id (this_cache, frame_id_build (frame_sp, func));
}

static const struct tramp_frame loongarch_linux_lp64_rt_sigframe =
{
  SIGTRAMP_FRAME,
  4,
  { /* From $kernel/arch/loongarch/vdso/sigreturn.S.  */
    /* ori	$r11, $r0, 0x8b(__NR_rt_sigreturn)  */
    { 0x03822c0b, ULONGEST_MAX },
    { 0x002b0000, ULONGEST_MAX }, /* syscall	0  */
    { TRAMP_SENTINEL_INSN, ULONGEST_MAX } },
  loongarch_linux_lp64_sigframe_init,
  NULL
};

/* Return the current system call's number present in the
   a7 register.  When the function fails, it returns -1.  */

static LONGEST
loongarch_linux_get_syscall_number (struct gdbarch *gdbarch,
				    thread_info *thread)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  auto regs = &tdep->regs;
  struct regcache *regcache = get_thread_regcache (thread);
  LONGEST ret;

  switch (tdep->ef_abi)
    {
    case EF_LARCH_ABI_LP64:
      if (REG_VALID
	  == regcache_cooked_read_signed (regcache, regs->r + 11, &ret))
	return ret;
    }

  return -1;
}

static CORE_ADDR
loongarch_linux_syscall_next_pc (struct frame_info *frame)
{
  struct gdbarch *gdbarch = get_frame_arch (frame);
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);
  auto regs = &tdep->regs;
  ULONGEST a7 = get_frame_register_unsigned (frame, regs->r + 11);

  switch (tdep->ef_abi)
    {
    case EF_LARCH_ABI_LP64:
      /* If we are about to make a sigreturn syscall, use the unwinder to
	 decode the signal frame.  */
      if (a7 == 0x8b /* LP64: __NR_rt_sigreturn.  */)
	return frame_unwind_caller_pc (get_current_frame ());
    }

  return -1;
}

static void
loongarch_linux_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  linux_init_abi (info, gdbarch, 0); /* FIXME displaced step support.  */

  switch (tdep->ef_abi)
    {
    case EF_LARCH_ABI_LP32:
      set_solib_svr4_fetch_link_map_offsets (
	gdbarch, svr4_ilp32_fetch_link_map_offsets);
      break;
    case EF_LARCH_ABI_LP64:
      set_solib_svr4_fetch_link_map_offsets (gdbarch,
					     svr4_lp64_fetch_link_map_offsets);
      tramp_frame_prepend_unwinder (gdbarch,
				    &loongarch_linux_lp64_rt_sigframe);
      tdep->syscall_next_pc = loongarch_linux_syscall_next_pc;

      set_gdbarch_get_syscall_number (gdbarch,
				      loongarch_linux_get_syscall_number);
      break;
    }

  /* GNU/Linux uses the dynamic linker included in the GNU C Library.  */
  set_gdbarch_skip_solib_resolver (gdbarch, glibc_skip_solib_resolver);

  /* Enable TLS support.  */
  set_gdbarch_fetch_tls_load_module_address (gdbarch,
					     svr4_fetch_objfile_link_map);

  set_gdbarch_call_dummy_location (gdbarch, AT_ENTRY_POINT);

  /* Core file support.  */
  set_gdbarch_iterate_over_regset_sections (
    gdbarch, loongarch_linux_iterate_over_regset_sections);
  set_gdbarch_core_read_description (gdbarch,
				     loongarch_linux_core_read_description);
}

void _initialize_loongarch_linux_tdep ();
void
_initialize_loongarch_linux_tdep ()
{
  gdbarch_register_osabi (
    bfd_arch_loongarch,
    bfd_mach_loongarch32 /* GDB may not care what arch variant is this.
     So we specify DEFAULT_BFD_ARCH.  */
    ,
    GDB_OSABI_LINUX, loongarch_linux_init_abi);
}
