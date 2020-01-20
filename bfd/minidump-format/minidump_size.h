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

// minidump_size.h: Provides a C++ template for programmatic access to
// the sizes of various types defined in minidump_format.h.
//
// Author: Mark Mentovai

#ifndef GOOGLE_BREAKPAD_COMMON_MINIDUMP_SIZE_H__
#define GOOGLE_BREAKPAD_COMMON_MINIDUMP_SIZE_H__

#include <sys/types.h>

#include "minidump_format.h"

namespace google_breakpad {

template<typename T>
class minidump_size {
 public:
  static size_t size() { return sizeof(T); }
};

// Explicit specializations for variable-length types.  The size returned
// for these should be the size for an object without its variable-length
// section.

template<>
class minidump_size<MDString> {
 public:
  static size_t size() { return MDString_minsize; }
};

template<>
class minidump_size<MDRawThreadList> {
 public:
  static size_t size() { return MDRawThreadList_minsize; }
};

template<>
class minidump_size<MDCVInfoPDB20> {
 public:
  static size_t size() { return MDCVInfoPDB20_minsize; }
};

template<>
class minidump_size<MDCVInfoPDB70> {
 public:
  static size_t size() { return MDCVInfoPDB70_minsize; }
};

template<>
class minidump_size<MDCVInfoELF> {
 public:
  static size_t size() { return MDCVInfoELF_minsize; }
};

template<>
class minidump_size<MDImageDebugMisc> {
 public:
  static size_t size() { return MDImageDebugMisc_minsize; }
};

template<>
class minidump_size<MDRawModuleList> {
 public:
  static size_t size() { return MDRawModuleList_minsize; }
};

template<>
class minidump_size<MDRawMemoryList> {
 public:
  static size_t size() { return MDRawMemoryList_minsize; }
};

// Explicit specialization for MDRawModule, for which sizeof may include
// tail-padding on some architectures but not others.

template<>
class minidump_size<MDRawModule> {
 public:
  static size_t size() { return MD_MODULE_SIZE; }
};

}  // namespace google_breakpad

#endif  // GOOGLE_BREAKPAD_COMMON_MINIDUMP_SIZE_H__
