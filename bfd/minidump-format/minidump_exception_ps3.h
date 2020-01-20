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

/* minidump_exception_ps3.h: A definition of exception codes for
 * PS3 */


#ifndef GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_PS3_H__
#define GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_PS3_H__

#include <stddef.h>

#include "breakpad_types.h"

typedef enum {
  MD_EXCEPTION_CODE_PS3_UNKNOWN = 0,
  MD_EXCEPTION_CODE_PS3_TRAP_EXCEP = 1,
  MD_EXCEPTION_CODE_PS3_PRIV_INSTR = 2,
  MD_EXCEPTION_CODE_PS3_ILLEGAL_INSTR = 3,
  MD_EXCEPTION_CODE_PS3_INSTR_STORAGE = 4,
  MD_EXCEPTION_CODE_PS3_INSTR_SEGMENT = 5,
  MD_EXCEPTION_CODE_PS3_DATA_STORAGE = 6,
  MD_EXCEPTION_CODE_PS3_DATA_SEGMENT = 7,
  MD_EXCEPTION_CODE_PS3_FLOAT_POINT = 8,
  MD_EXCEPTION_CODE_PS3_DABR_MATCH = 9,
  MD_EXCEPTION_CODE_PS3_ALIGN_EXCEP = 10,
  MD_EXCEPTION_CODE_PS3_MEMORY_ACCESS = 11,
  MD_EXCEPTION_CODE_PS3_COPRO_ALIGN = 12,
  MD_EXCEPTION_CODE_PS3_COPRO_INVALID_COM = 13,
  MD_EXCEPTION_CODE_PS3_COPRO_ERR = 14,
  MD_EXCEPTION_CODE_PS3_COPRO_FIR = 15,
  MD_EXCEPTION_CODE_PS3_COPRO_DATA_SEGMENT = 16,
  MD_EXCEPTION_CODE_PS3_COPRO_DATA_STORAGE = 17,
  MD_EXCEPTION_CODE_PS3_COPRO_STOP_INSTR = 18,
  MD_EXCEPTION_CODE_PS3_COPRO_HALT_INSTR = 19,
  MD_EXCEPTION_CODE_PS3_COPRO_HALTINST_UNKNOWN = 20,
  MD_EXCEPTION_CODE_PS3_COPRO_MEMORY_ACCESS = 21,
  MD_EXCEPTION_CODE_PS3_GRAPHIC = 22
} MDExceptionCodePS3;

#endif /* GOOGLE_BREAKPAD_COMMON_MINIDUMP_EXCEPTION_PS3_H__ */
