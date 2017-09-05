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
#include "x86-xstate.h"
#include <inttypes.h>

#if defined __i386__ || !defined IN_PROCESS_AGENT
#include "../features/i386/32bit-core.c"
#include "../features/i386/32bit-linux.c"
#include "../features/i386/32bit-sse.c"
#include "../features/i386/32bit-avx.c"
#include "../features/i386/32bit-avx512.c"
#include "../features/i386/32bit-mpx.c"
#include "../features/i386/32bit-pkeys.c"

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

static struct target_desc *i386_tdescs[X86_TDESC_LAST] = { };

#if defined GDB_SELF_TEST && !defined IN_PROCESS_AGENT
#include "selftest.h"

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
#endif /* GDB_SELF_TEST */

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

#if GDB_SELF_TEST && !defined IN_PROCESS_AGENT
  selftests::register_test (selftests::tdesc::i386_tdesc_test);
#endif
#endif
}

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
      *tdesc = new target_desc ();

      long regnum = 0;

      if (xcr0 & X86_XSTATE_X87)
	regnum = create_feature_i386_32bit_core (*tdesc, regnum);

      if (xcr0 & X86_XSTATE_SSE)
	regnum = create_feature_i386_32bit_sse (*tdesc, regnum);

      regnum = create_feature_i386_32bit_linux (*tdesc, regnum);

      if (xcr0 & X86_XSTATE_AVX)
	regnum = create_feature_i386_32bit_avx (*tdesc, regnum);

      if (xcr0 & X86_XSTATE_MPX)
	regnum = create_feature_i386_32bit_mpx (*tdesc, regnum);

      if (xcr0 & X86_XSTATE_AVX512)
	regnum = create_feature_i386_32bit_avx512 (*tdesc, regnum);

      if (xcr0 & X86_XSTATE_PKRU)
	regnum = create_feature_i386_32bit_pkeys (*tdesc, regnum);

      init_target_desc (*tdesc);

#ifndef IN_PROCESS_AGENT
      static const char *expedite_regs_i386[] = { "ebp", "esp", "eip", NULL };
      (*tdesc)->expedite_regs = expedite_regs_i386;

      if (xcr0 & X86_XSTATE_PKRU)
	(*tdesc)->xmltarget = "i386-avx-mpx-avx512-pku-linux.xml";
      else if (xcr0 & X86_XSTATE_AVX512)
	(*tdesc)->xmltarget = "i386-avx-avx512-linux.xml";
      else if ((xcr0 & X86_XSTATE_AVX_MPX_MASK) == X86_XSTATE_AVX_MPX_MASK)
	(*tdesc)->xmltarget = "i386-avx-mpx-linux.xml";
      else if (xcr0 & X86_XSTATE_MPX)
	(*tdesc)->xmltarget = "i386-mpx-linux.xml";
      else if (xcr0 & X86_XSTATE_AVX)
	(*tdesc)->xmltarget = "i386-avx-linux.xml";
      else if (xcr0 & X86_XSTATE_SSE)
	(*tdesc)->xmltarget = "i386-linux.xml";
      else if (xcr0 & X86_XSTATE_X87)
	(*tdesc)->xmltarget = "i386-mmx-linux.xml";
      else
	internal_error (__FILE__, __LINE__,
			"unknown xcr0: %" PRIu64, xcr0);
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
