/* GNU/Linux/x86-64 specific target description, for the remote server
   for GDB.
   Copyright (C) 2017 Free Software Foundation, Inc.

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
#include "tdesc.h"
#include "linux-x86-tdesc.h"
#include "arch/i386.h"
#include "common/x86-xstate.h"

static struct target_desc *i386_tdescs[X86_TDESC_LAST] = { };

#if defined __i386__ || !defined IN_PROCESS_AGENT

/* Return the target description according to XCR0.  */

const struct target_desc *
i386_linux_read_description (uint64_t xcr0)
{
  struct target_desc **tdesc = NULL;

  if (xcr0 & X86_XSTATE_PKRU)
     tdesc = &i386_tdescs[X86_TDESC_AVX_MPX_AVX512_PKU];
  else if (xcr0 & X86_XSTATE_AVX512)
    tdesc = &i386_tdescs[X86_TDESC_AVX_AVX512];
  else if ((xcr0 & X86_XSTATE_AVX_MPX_MASK) == X86_XSTATE_AVX_MPX_MASK)
    tdesc = &i386_tdescs[X86_TDESC_AVX_MPX];
  else if (xcr0 & X86_XSTATE_MPX)
    tdesc = &i386_tdescs[X86_TDESC_MPX];
  else if (xcr0 & X86_XSTATE_AVX)
    tdesc = &i386_tdescs[X86_TDESC_AVX];
  else if (xcr0 & X86_XSTATE_SSE)
    tdesc = &i386_tdescs[X86_TDESC_SSE];
  else if (xcr0 & X86_XSTATE_X87)
    tdesc = &i386_tdescs[X86_TDESC_MMX];

  if (tdesc == NULL)
    return NULL;

  if (*tdesc == NULL)
    {
      *tdesc = i386_create_target_description (xcr0);

      init_target_desc (*tdesc);

#ifndef IN_PROCESS_AGENT
      static const char *expedite_regs_i386[] = { "ebp", "esp", "eip", NULL };
      (*tdesc)->expedite_regs = expedite_regs_i386;
#endif
    }

  return *tdesc;;
}
#endif

#ifndef IN_PROCESS_AGENT
int
i386_get_ipa_tdesc_idx (const struct target_desc *tdesc)
{
  for (int i = 0; i < X86_TDESC_LAST; i++)
    {
      if (tdesc == i386_tdescs[i])
	return i;
    }

  /* If none tdesc is found, return the one with minimum features.  */
  return X86_TDESC_MMX;
}

#endif
