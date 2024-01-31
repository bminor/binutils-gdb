/* GNU/Linux/x86-64 specific target description, for the remote server
   for GDB.
   Copyright (C) 2017-2024 Free Software Foundation, Inc.

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
#include "gdbsupport/x86-xstate.h"
#ifdef __x86_64__
#include "arch/amd64.h"
#endif
#include "x86-tdesc.h"
#include "nat/x86-linux-tdesc.h"

/* A structure used to describe a single cpu feature that might, or might
   not, be checked for when creating a target description for one of i386,
   amd64, or x32.  */

struct x86_tdesc_feature {
  /* The cpu feature mask.  This is a mask against an xcr0 value.  */
  uint64_t feature;

  /* Is this feature checked when creating an i386 target description.  */
  bool is_i386;

  /* Is this feature checked when creating an amd64 target description.  */
  bool is_amd64;

  /* Is this feature checked when creating an x32 target description.  */
  bool is_x32;
};

/* A constant table that describes all of the cpu features that are
   checked when building a target description for i386, amd64, or x32.  */

static constexpr x86_tdesc_feature x86_linux_all_tdesc_features[] = {
  /* Feature,           i386,	amd64,	x32.  */
  { X86_XSTATE_PKRU,	true,	true, 	true },
  { X86_XSTATE_AVX512,	true,	true, 	true },
  { X86_XSTATE_AVX,	true,	true, 	true },
  { X86_XSTATE_MPX,	true,	true, 	false },
  { X86_XSTATE_SSE,	true,	false, 	false },
  { X86_XSTATE_X87,	true,	false, 	false }
};

/* Return a compile time constant which is a mask of all the cpu features
   that are checked for when building an i386 target description.  */

static constexpr uint64_t
x86_linux_i386_tdesc_feature_mask ()
{
  uint64_t mask = 0;

  for (const auto &entry : x86_linux_all_tdesc_features)
    if (entry.is_i386)
      mask |= entry.feature;

  return mask;
}

/* Return a compile time constant which is a mask of all the cpu features
   that are checked for when building an amd64 target description.  */

static constexpr uint64_t
x86_linux_amd64_tdesc_feature_mask ()
{
  uint64_t mask = 0;

  for (const auto &entry : x86_linux_all_tdesc_features)
    if (entry.is_amd64)
      mask |= entry.feature;

  return mask;
}

/* Return a compile time constant which is a mask of all the cpu features
   that are checked for when building an x32 target description.  */

static constexpr uint64_t
x86_linux_x32_tdesc_feature_mask ()
{
  uint64_t mask = 0;

  for (const auto &entry : x86_linux_all_tdesc_features)
    if (entry.is_x32)
      mask |= entry.feature;

  return mask;
}

/* Return a compile time constant which is a count of the number of cpu
   features that are checked for when building an i386 target description.  */

static constexpr int
x86_linux_i386_tdesc_count ()
{
  uint64_t count = 0;

  for (const auto &entry : x86_linux_all_tdesc_features)
    if (entry.is_i386)
      ++count;

  gdb_assert (count > 0);

  return (1 << count);
}

/* Return a compile time constant which is a count of the number of cpu
   features that are checked for when building an amd64 target description.  */

static constexpr int
x86_linux_amd64_tdesc_count ()
{
  uint64_t count = 0;

  for (const auto &entry : x86_linux_all_tdesc_features)
    if (entry.is_amd64)
      ++count;

  gdb_assert (count > 0);

  return (1 << count);
}

/* Return a compile time constant which is a count of the number of cpu
   features that are checked for when building an x32 target description.  */

static constexpr int
x86_linux_x32_tdesc_count ()
{
  uint64_t count = 0;

  for (const auto &entry : x86_linux_all_tdesc_features)
    if (entry.is_x32)
      ++count;

  gdb_assert (count > 0);

  return (1 << count);
}

#ifdef IN_PROCESS_AGENT

/* See linux-x86-tdesc.h.  */

int
x86_linux_amd64_ipa_tdesc_count ()
{
  return x86_linux_amd64_tdesc_count ();
}

/* See linux-x86-tdesc.h.  */

int
x86_linux_x32_ipa_tdesc_count ()
{
  return x86_linux_x32_tdesc_count ();
}

/* See linux-x86-tdesc.h.  */

int
x86_linux_i386_ipa_tdesc_count ()
{
  return x86_linux_i386_tdesc_count ();
}

#endif /* IN_PROCESS_AGENT */

/* Convert an xcr0 value into an integer.  The integer will be passed to
   the in-process-agent where it will then be passed to
   x86_linux_tdesc_idx_to_xcr0 to get back the xcr0 value.  */

int
x86_linux_xcr0_to_tdesc_idx (uint64_t xcr0)
{
  /* The following table shows which features are checked for when creating
     the target descriptions (see nat/x86-linux-tdesc.c), the feature order
     represents the bit order within the generated index number.

     i386  | x87 sse mpx avx avx512 pkru
     amd64 |         mpx avx avx512 pkru
     i32   |             avx avx512 pkru

     The features are ordered so that for each mode (i386, amd64, i32) the
     generated index will form a continuous range.  */

  int idx = 0;

  for (int i = 0; i < ARRAY_SIZE (x86_linux_all_tdesc_features); ++i)
    {
      if ((xcr0 & x86_linux_all_tdesc_features[i].feature) != 0)
	idx |= (1 << i);
    }

  return idx;
}


#ifdef IN_PROCESS_AGENT

/* Convert an index number (as returned from x86_linux_xcr0_to_tdesc_idx)
   into an xcr0 value which can then be used to create a target
   description.  */

uint64_t
x86_linux_tdesc_idx_to_xcr0 (int idx)
{
  uint64_t xcr0 = 0;

  for (int i = 0; i < ARRAY_SIZE (x86_linux_all_tdesc_features); ++i)
    {
      if ((idx & (1 << i)) != 0)
	xcr0 |= x86_linux_all_tdesc_features[i].feature;
    }

  return xcr0;
}

#endif /* IN_PROCESS_AGENT */

#if defined __i386__ || !defined IN_PROCESS_AGENT

/* A cache of all possible i386 target descriptions.  */

static struct target_desc *i386_tdescs[x86_linux_i386_tdesc_count ()] = { };

/* See nat/x86-linux-tdesc.h.  */

const struct target_desc *
i386_linux_read_description (uint64_t xcr0)
{
  xcr0 &= x86_linux_i386_tdesc_feature_mask ();
  int idx = x86_linux_xcr0_to_tdesc_idx (xcr0);

  gdb_assert (idx >= 0 && idx < x86_linux_i386_tdesc_count ());

  target_desc **tdesc = &i386_tdescs[idx];

  if (*tdesc == nullptr)
    {
      *tdesc = i386_create_target_description (xcr0, true, false);

      init_target_desc (*tdesc, i386_expedite_regs);
    }

  return *tdesc;
}
#endif

#ifdef __x86_64__

/* A cache of all possible amd64 target descriptions.  */

static target_desc *amd64_tdescs[x86_linux_amd64_tdesc_count ()] = { };

/* A cache of all possible x32 target descriptions.  */

static target_desc *x32_tdescs[x86_linux_x32_tdesc_count ()] = { };

/* See nat/x86-linux-tdesc.h.  */

const struct target_desc *
amd64_linux_read_description (uint64_t xcr0, bool is_x32)
{
  if (is_x32)
    xcr0 &= x86_linux_x32_tdesc_feature_mask ();
  else
    xcr0 &= x86_linux_amd64_tdesc_feature_mask ();

  int idx = x86_linux_xcr0_to_tdesc_idx (xcr0);

  if (is_x32)
    gdb_assert (idx >= 0 && idx < x86_linux_x32_tdesc_count ());
  else
    gdb_assert (idx >= 0 && idx < x86_linux_amd64_tdesc_count ());

  target_desc **tdesc = nullptr;

  if (is_x32)
    tdesc = &x32_tdescs[idx];
  else
    tdesc = &amd64_tdescs[idx];

  if (*tdesc == nullptr)
    {
      *tdesc = amd64_create_target_description (xcr0, is_x32, true, true);

      init_target_desc (*tdesc, amd64_expedite_regs);
    }
  return *tdesc;
}

#endif
