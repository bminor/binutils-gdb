/* The SFrame stacktracer - accessing target registers.

   Copyright (C) 2023 Free Software Foundation, Inc.

   This file is part of libsframest.

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

#include <stdint.h>
#include <ucontext.h>

#ifndef SFRAME_STACKTRACE_REGS_H
#define SFRAME_STACKTRACE_REGS_H

#if defined (__x86_64__)

static inline uint64_t
get_context_pc (ucontext_t *cp)
{
  return cp->uc_mcontext.gregs[REG_RIP];
}

static inline uint64_t
get_context_rsp (ucontext_t *cp)
{
  return cp->uc_mcontext.gregs[REG_RSP];
}

static inline uint64_t
get_context_rfp (ucontext_t *cp)
{
  return cp->uc_mcontext.gregs[REG_RBP];
}

static inline uint64_t
get_context_ra (ucontext_t *cp)
{
  return 0;	/* SFRAME_CFA_FIXED_RA_INVALID  */
}

#elif defined (__aarch64__)

static inline uint64_t
get_context_pc (ucontext_t *cp)
{
  return cp->uc_mcontext.pc;
}

static inline uint64_t
get_context_rsp (ucontext_t *cp)
{
  return cp->uc_mcontext.sp;
}

static inline uint64_t
get_context_rfp (ucontext_t *cp)
{
#define UNWIND_AARCH64_X29              29      /* 64-bit frame pointer.  */
  return cp->uc_mcontext.regs[UNWIND_AARCH64_X29];
}

static inline uint64_t
get_context_ra (ucontext_t *cp)
{
#define UNWIND_AARCH64_X30              30      /* 64-bit link pointer.  */
  return cp->uc_mcontext.regs[UNWIND_AARCH64_X30];
}

#elif defined (__s390x__)

static inline uint64_t
get_context_pc (ucontext_t *cp)
{
//  return cp->uc_mcontext.psw.addr;
  return cp->uc_mcontext.gregs[14];
}

static inline uint64_t
get_context_rsp (ucontext_t *cp)
{
  return cp->uc_mcontext.gregs[15];
}

static inline uint64_t
get_context_rfp (ucontext_t *cp)
{
  return cp->uc_mcontext.gregs[11];
}

static inline uint64_t
get_context_ra (ucontext_t *cp)
{
  return cp->uc_mcontext.gregs[14];
}

#endif

#endif /* SFRAME_STACKTRACE_REGS_H.  */
