/* Header file for the format of Windows minidumps.
   Copyright (C) 2020 Free Software Foundation, Inc.
   Written by Google LLC.
   Relicensed with permission, original at:
   https://source.chromium.org/chromium/chromium/src/+/master:third_party/breakpad/breakpad/src/google_breakpad/common

   This file is part of BFD, the Binary File Descriptor library.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* breakpad_types.h: Precise-width types
 *
 * (This is C99 source, please don't corrupt it with C++.)
 *
 * This file ensures that types uintN_t are defined for N = 8, 16, 32, and
 * 64.  Types of precise widths are crucial to the task of writing data
 * structures on one platform and reading them on another.
 *
 * Author: Mark Mentovai */

#ifndef GOOGLE_BREAKPAD_COMMON_BREAKPAD_TYPES_H__
#define GOOGLE_BREAKPAD_COMMON_BREAKPAD_TYPES_H__

#if (defined(_INTTYPES_H) || defined(_INTTYPES_H_)) && \
    !defined(__STDC_FORMAT_MACROS)
#error "inttypes.h has already been included before this header file, but "
#error "without __STDC_FORMAT_MACROS defined."
#endif

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif  /* __STDC_FORMAT_MACROS */
#include <inttypes.h>

typedef struct {
  uint64_t high;
  uint64_t low;
} uint128_struct;

typedef uint64_t breakpad_time_t;

/* Try to get PRIx64 from inttypes.h, but if it's not defined, fall back to
 * llx, which is the format string for "long long" - this is a 64-bit
 * integral type on many systems. */
#ifndef PRIx64
#define PRIx64 "llx"
#endif  /* !PRIx64 */

#endif  /* GOOGLE_BREAKPAD_COMMON_BREAKPAD_TYPES_H__ */
