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

#include "../features/i386/32bit-core.c"
#include "../features/i386/32bit-linux.c"
#include "../features/i386/32bit-sse.c"
#include "../features/i386/32bit-avx.c"
#include "../features/i386/32bit-avx512.c"
#include "../features/i386/32bit-mpx.c"
#include "../features/i386/32bit-pkeys.c"

static const struct target_desc *i386_tdescs[X86_TDESC_LAST] = { };

const struct target_desc *
i386_get_ipa_tdesc (int idx)
{
  if (idx >= X86_TDESC_LAST)
    internal_error (__FILE__, __LINE__,
		    "unknown ipa tdesc index: %d", idx);

  struct target_desc **tdesc = (struct target_desc **) &i386_tdescs[idx];

  if (*tdesc == NULL)
    {
      *tdesc = new target_desc ();

#ifndef IN_PROCESS_AGENT
      set_tdesc_architecture (*tdesc, "i386");
      set_tdesc_osabi (*tdesc, "GNU/Linux");
#endif

      long regnum = 0;

      regnum = create_feature_i386_32bit_core (*tdesc, regnum);

      if (idx != X86_TDESC_MMX)
	regnum = create_feature_i386_32bit_sse (*tdesc, regnum);

      regnum = create_feature_i386_32bit_linux (*tdesc, regnum);

      if (idx == X86_TDESC_AVX || idx == X86_TDESC_AVX_MPX
	  || idx == X86_TDESC_AVX_AVX512
	  || idx == X86_TDESC_AVX_MPX_AVX512_PKU)
	regnum = create_feature_i386_32bit_avx (*tdesc, regnum);

      if (idx == X86_TDESC_MPX || idx == X86_TDESC_AVX_MPX
	  || idx == X86_TDESC_AVX_MPX_AVX512_PKU)
	regnum = create_feature_i386_32bit_mpx (*tdesc, regnum);

      if (idx == X86_TDESC_AVX_AVX512
	  || idx == X86_TDESC_AVX_MPX_AVX512_PKU)
	regnum = create_feature_i386_32bit_avx512 (*tdesc, regnum);

      if (idx == X86_TDESC_AVX_MPX_AVX512_PKU)
	regnum = create_feature_i386_32bit_pkeys (*tdesc, regnum);

      init_target_desc (*tdesc);

#ifndef IN_PROCESS_AGENT
      static const char *expedite_regs_i386[] = { "ebp", "esp", "eip", NULL };
      (*tdesc)->expedite_regs = expedite_regs_i386;
#endif
    }
  return *tdesc;;
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
