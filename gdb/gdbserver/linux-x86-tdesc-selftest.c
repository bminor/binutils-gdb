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
#include "../selftest.h"

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

#ifdef __x86_64__

/* Defined in auto-generated file amd64-linux.c.  */
void init_registers_amd64_linux (void);
extern const struct target_desc *tdesc_amd64_linux;

/* Defined in auto-generated file amd64-avx-linux.c.  */
void init_registers_amd64_avx_linux (void);
extern const struct target_desc *tdesc_amd64_avx_linux;

/* Defined in auto-generated file amd64-avx-avx512-linux.c.  */
void init_registers_amd64_avx_avx512_linux (void);
extern const struct target_desc *tdesc_amd64_avx_avx512_linux;

/* Defined in auto-generated file amd64-avx-mpx-avx512-pku-linux.c.  */
void init_registers_amd64_avx_mpx_avx512_pku_linux (void);
extern const struct target_desc *tdesc_amd64_avx_mpx_avx512_pku_linux;

/* Defined in auto-generated file amd64-avx-mpx-linux.c.  */
void init_registers_amd64_avx_mpx_linux (void);
extern const struct target_desc *tdesc_amd64_avx_mpx_linux;

/* Defined in auto-generated file amd64-mpx-linux.c.  */
void init_registers_amd64_mpx_linux (void);
extern const struct target_desc *tdesc_amd64_mpx_linux;

/* Defined in auto-generated file x32-linux.c.  */
void init_registers_x32_linux (void);
extern const struct target_desc *tdesc_x32_linux;

/* Defined in auto-generated file x32-avx-linux.c.  */
void init_registers_x32_avx_linux (void);
extern const struct target_desc *tdesc_x32_avx_linux;

/* Defined in auto-generated file x32-avx-avx512-linux.c.  */
void init_registers_x32_avx_avx512_linux (void);
extern const struct target_desc *tdesc_x32_avx_avx512_linux;

#endif

namespace selftests {
namespace gdbserver {
static void
i386_tdesc_test ()
{
  const struct target_desc *tdesc = i386_get_ipa_tdesc (X86_TDESC_MMX);

  SELF_CHECK (*tdesc == *tdesc_i386_mmx_linux);
  delete tdesc;

  tdesc = i386_get_ipa_tdesc (X86_TDESC_SSE);
  SELF_CHECK (*tdesc == *tdesc_i386_linux);
  delete tdesc;

  tdesc = i386_get_ipa_tdesc (X86_TDESC_AVX);
  SELF_CHECK (*tdesc == *tdesc_i386_avx_linux);
  delete tdesc;

  tdesc = i386_get_ipa_tdesc (X86_TDESC_MPX);
  SELF_CHECK (*tdesc == *tdesc_i386_mpx_linux);
  delete tdesc;

  tdesc = i386_get_ipa_tdesc (X86_TDESC_AVX_MPX);
  SELF_CHECK (*tdesc == *tdesc_i386_avx_mpx_linux);
  delete tdesc;

  tdesc = i386_get_ipa_tdesc (X86_TDESC_AVX_AVX512);
  SELF_CHECK (*tdesc == *tdesc_i386_avx_avx512_linux);
  delete tdesc;

  tdesc = i386_get_ipa_tdesc (X86_TDESC_AVX_MPX_AVX512_PKU);
  SELF_CHECK (*tdesc == *tdesc_i386_avx_mpx_avx512_pku_linux);
  delete tdesc;
}

#ifdef __x86_64__

static void
amd64_tdesc_test ()
{
  const struct target_desc *tdesc = amd64_get_ipa_tdesc (X86_TDESC_SSE, false);

  SELF_CHECK (*tdesc == *tdesc_amd64_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_AVX, false);
  SELF_CHECK (*tdesc == *tdesc_amd64_avx_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_AVX_AVX512, false);
  SELF_CHECK (*tdesc == *tdesc_amd64_avx_avx512_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_MPX, false);
  SELF_CHECK (*tdesc == *tdesc_amd64_mpx_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_AVX_MPX, false);
  SELF_CHECK (*tdesc == *tdesc_amd64_avx_mpx_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_AVX_MPX_AVX512_PKU, false);
  SELF_CHECK (*tdesc == *tdesc_amd64_avx_mpx_avx512_pku_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_SSE, true);
  SELF_CHECK (*tdesc == *tdesc_x32_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_AVX, true);
  SELF_CHECK (*tdesc == *tdesc_x32_avx_linux);
  delete tdesc;

  tdesc = amd64_get_ipa_tdesc (X86_TDESC_AVX_AVX512, true);
  SELF_CHECK (*tdesc == *tdesc_x32_avx_avx512_linux);
  delete tdesc;
}

#endif
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

  register_self_test (selftests::gdbserver::i386_tdesc_test);

#ifdef __x86_64__
  init_registers_x32_linux ();
  init_registers_x32_avx_linux ();
  init_registers_x32_avx_avx512_linux ();

  init_registers_amd64_linux ();
  init_registers_amd64_avx_linux ();
  init_registers_amd64_mpx_linux ();
  init_registers_amd64_avx_mpx_linux ();
  init_registers_amd64_avx_avx512_linux ();
  init_registers_amd64_avx_mpx_avx512_pku_linux ();

  register_self_test (selftests::gdbserver::amd64_tdesc_test);
#endif
}
