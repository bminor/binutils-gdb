/* Copyright (C) 2017 Free Software Foundation, Inc.

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
#include "tdesc.h"
#include "common/selftest.h"
#include "x86-xstate.h"

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

namespace selftests {
namespace tdesc {
static void
i386_tdesc_test ()
{
  struct
  {
    unsigned int mask;
    const target_desc *tdesc;
  } tdesc_tests[] = {
    { X86_XSTATE_X87, tdesc_i386_mmx_linux },
    { X86_XSTATE_SSE_MASK, tdesc_i386_linux },
    { X86_XSTATE_AVX_MASK, tdesc_i386_avx_linux },
    { X86_XSTATE_MPX_MASK, tdesc_i386_mpx_linux },
    { X86_XSTATE_AVX_MPX_MASK, tdesc_i386_avx_mpx_linux },
    { X86_XSTATE_AVX_AVX512_MASK, tdesc_i386_avx_avx512_linux },
    { X86_XSTATE_AVX_MPX_AVX512_PKU_MASK, tdesc_i386_avx_mpx_avx512_pku_linux }
  };

  for (auto &elem : tdesc_tests)
    {
      const target_desc *tdesc = i386_linux_read_description (elem.mask);

      SELF_CHECK (*tdesc == *elem.tdesc);
    }
}
}
} // namespace selftests

void
initialize_low_tdesc ()
{
  init_registers_i386_linux ();
  init_registers_i386_mmx_linux ();
  init_registers_i386_avx_linux ();
  init_registers_i386_mpx_linux ();
  init_registers_i386_avx_mpx_linux ();
  init_registers_i386_avx_avx512_linux ();
  init_registers_i386_avx_mpx_avx512_pku_linux ();

  register_self_test (selftests::tdesc::i386_tdesc_test);
}
