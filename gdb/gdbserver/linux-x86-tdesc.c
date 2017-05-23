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
#include "linux-x86-tdesc.h"

#if defined __i386__ || !defined IN_PROCESS_AGENT
/* Defined in auto-generated file i386-linux.c.  */
void init_registers_i386_linux (void);
extern const struct target_desc *tdesc_i386_linux;

/* Defined in auto-generated file i386-mmx-linux.c.  */
void init_registers_i386_mmx_linux (void);
extern const struct target_desc *tdesc_i386_mmx_linux;

/* Defined in auto-generated file i386-avx-linux.c.  */
void init_registers_i386_avx_linux (void);
extern const struct target_desc *tdesc_i386_avx_linux;

/* Defined in auto-generated file i386-avx-mpx-linux.c.  */
void init_registers_i386_avx_mpx_linux (void);
extern const struct target_desc *tdesc_i386_avx_mpx_linux;

/* Defined in auto-generated file i386-avx-avx512-linux.c.  */
void init_registers_i386_avx_avx512_linux (void);
extern const struct target_desc *tdesc_i386_avx_avx512_linux;

/* Defined in auto-generated file i386-avx-mpx-avx512-linux.c.  */
void init_registers_i386_avx_mpx_avx512_pku_linux (void);
extern const struct target_desc *tdesc_i386_avx_mpx_avx512_pku_linux;

/* Defined in auto-generated file i386-mpx-linux.c.  */
void init_registers_i386_mpx_linux (void);
extern const struct target_desc *tdesc_i386_mpx_linux;
#endif

static const struct target_desc *i386_tdescs[X86_TDESC_LAST] = { };

void
initialize_low_tdesc ()
{
#if defined __i386__ || !defined IN_PROCESS_AGENT
  init_registers_i386_linux ();
  init_registers_i386_mmx_linux ();
  init_registers_i386_avx_linux ();
  init_registers_i386_mpx_linux ();
  init_registers_i386_avx_mpx_linux ();
  init_registers_i386_avx_avx512_linux ();
  init_registers_i386_avx_mpx_avx512_pku_linux ();

  i386_tdescs[X86_TDESC_MMX] = tdesc_i386_mmx_linux;
  i386_tdescs[X86_TDESC_SSE] = tdesc_i386_linux;
  i386_tdescs[X86_TDESC_AVX] = tdesc_i386_avx_linux;
  i386_tdescs[X86_TDESC_MPX] = tdesc_i386_mpx_linux;
  i386_tdescs[X86_TDESC_AVX_MPX] = tdesc_i386_avx_mpx_linux;
  i386_tdescs[X86_TDESC_AVX_AVX512] = tdesc_i386_avx_avx512_linux;
  i386_tdescs[X86_TDESC_AVX_MPX_AVX512_PKU]
    = tdesc_i386_avx_mpx_avx512_pku_linux;
#endif
}

const struct target_desc *
i386_get_ipa_tdesc (int idx)
{
  if (idx >= X86_TDESC_LAST)
    internal_error (__FILE__, __LINE__,
		    "unknown ipa tdesc index: %d", idx);

  return i386_tdescs[idx];
}

#ifdef IN_PROCESS_AGENT

#if defined __i386__
const struct target_desc *
get_ipa_tdesc (int idx)
{
return i386_get_ipa_tdesc (idx);
}
#endif

#else
int
i386_get_ipa_tdesc_idx (const struct target_desc *tdesc)
{
  for (int i = 0; i < X86_TDESC_LAST; i++)
    {
      if (tdesc == i386_tdescs[i])
	return i;
    }

  return 0;
}

#endif
