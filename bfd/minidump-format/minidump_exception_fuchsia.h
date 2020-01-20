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

/* minidump_exception_fuchsia.h: A definition of exception codes for Fuchsia.
 *
 * Author: Ivan Penkov */

#ifndef GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_FUCHSIA_H_
#define GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_FUCHSIA_H_

#include <stddef.h>

#include "breakpad_types.h"

// Based on zircon/system/public/zircon/syscalls/exception.h
typedef enum {
  // Architectural exceptions
  MD_EXCEPTION_CODE_FUCHSIA_GENERAL = 0x8,
  MD_EXCEPTION_CODE_FUCHSIA_FATAL_PAGE_FAULT = 0x108,
  MD_EXCEPTION_CODE_FUCHSIA_UNDEFINED_INSTRUCTION = 0x208,
  MD_EXCEPTION_CODE_FUCHSIA_SW_BREAKPOINT = 0x308,
  MD_EXCEPTION_CODE_FUCHSIA_HW_BREAKPOINT = 0x408,
  MD_EXCEPTION_CODE_FUCHSIA_UNALIGNED_ACCESS = 0x508,
  //
  // Synthetic exceptions
  MD_EXCEPTION_CODE_FUCHSIA_THREAD_STARTING = 0x8008,
  MD_EXCEPTION_CODE_FUCHSIA_THREAD_EXITING = 0x8108,
  MD_EXCEPTION_CODE_FUCHSIA_POLICY_ERROR = 0x8208,
  MD_EXCEPTION_CODE_FUCHSIA_PROCESS_STARTING = 0x8308,
} MDExceptionCodeFuchsia;

#endif  // GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_FUCHSIA_H_
