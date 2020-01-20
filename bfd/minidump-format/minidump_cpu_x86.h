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

/* minidump_format.h: A cross-platform reimplementation of minidump-related
 * portions of DbgHelp.h from the Windows Platform SDK.
 *
 * (This is C99 source, please don't corrupt it with C++.)
 *
 * This file contains the necessary definitions to read minidump files
 * produced on x86.  These files may be read on any platform provided
 * that the alignments of these structures on the processing system are
 * identical to the alignments of these structures on the producing system.
 * For this reason, precise-sized types are used.  The structures defined
 * by this file have been laid out to minimize alignment problems by ensuring
 * ensuring that all members are aligned on their natural boundaries.  In
 * In some cases, tail-padding may be significant when different ABIs specify
 * different tail-padding behaviors.  To avoid problems when reading or
 * writing affected structures, MD_*_SIZE macros are provided where needed,
 * containing the useful size of the structures without padding.
 *
 * Structures that are defined by Microsoft to contain a zero-length array
 * are instead defined here to contain an array with one element, as
 * zero-length arrays are forbidden by standard C and C++.  In these cases,
 * *_minsize constants are provided to be used in place of sizeof.  For a
 * cleaner interface to these sizes when using C++, see minidump_size.h.
 *
 * These structures are also sufficient to populate minidump files.
 *
 * These definitions may be extended to support handling minidump files
 * for other CPUs and other operating systems.
 *
 * Because precise data type sizes are crucial for this implementation to
 * function properly and portably in terms of interoperability with minidumps
 * produced by DbgHelp on Windows, a set of primitive types with known sizes
 * are used as the basis of each structure defined by this file.  DbgHelp
 * on Windows is assumed to be the reference implementation; this file
 * seeks to provide a cross-platform compatible implementation.  To avoid
 * collisions with the types and values defined and used by DbgHelp in the
 * event that this implementation is used on Windows, each type and value
 * defined here is given a new name, beginning with "MD".  Names of the
 * equivalent types and values in the Windows Platform SDK are given in
 * comments.
 *
 * Author: Mark Mentovai */

#ifndef GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_X86_H__
#define GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_X86_H__

#define MD_FLOATINGSAVEAREA_X86_REGISTERAREA_SIZE 80
     /* SIZE_OF_80387_REGISTERS */

typedef struct {
  uint32_t control_word;
  uint32_t status_word;
  uint32_t tag_word;
  uint32_t error_offset;
  uint32_t error_selector;
  uint32_t data_offset;
  uint32_t data_selector;

  /* register_area contains eight 80-bit (x87 "long double") quantities for
   * floating-point registers %st0 (%mm0) through %st7 (%mm7). */
  uint8_t  register_area[MD_FLOATINGSAVEAREA_X86_REGISTERAREA_SIZE];
  uint32_t cr0_npx_state;
} MDFloatingSaveAreaX86;  /* FLOATING_SAVE_AREA */


#define MD_CONTEXT_X86_EXTENDED_REGISTERS_SIZE 512
     /* MAXIMUM_SUPPORTED_EXTENSION */

typedef struct {
  /* The next field determines the layout of the structure, and which parts
   * of it are populated */
  uint32_t             context_flags;

  /* The next 6 registers are included with MD_CONTEXT_X86_DEBUG_REGISTERS */
  uint32_t             dr0;
  uint32_t             dr1;
  uint32_t             dr2;
  uint32_t             dr3;
  uint32_t             dr6;
  uint32_t             dr7;

  /* The next field is included with MD_CONTEXT_X86_FLOATING_POINT */
  MDFloatingSaveAreaX86 float_save;

  /* The next 4 registers are included with MD_CONTEXT_X86_SEGMENTS */
  uint32_t             gs; 
  uint32_t             fs;
  uint32_t             es;
  uint32_t             ds;
  /* The next 6 registers are included with MD_CONTEXT_X86_INTEGER */
  uint32_t             edi;
  uint32_t             esi;
  uint32_t             ebx;
  uint32_t             edx;
  uint32_t             ecx;
  uint32_t             eax;

  /* The next 6 registers are included with MD_CONTEXT_X86_CONTROL */
  uint32_t             ebp;
  uint32_t             eip;
  uint32_t             cs;      /* WinNT.h says "must be sanitized" */
  uint32_t             eflags;  /* WinNT.h says "must be sanitized" */
  uint32_t             esp;
  uint32_t             ss;

  /* The next field is included with MD_CONTEXT_X86_EXTENDED_REGISTERS.
   * It contains vector (MMX/SSE) registers.  It it laid out in the
   * format used by the fxsave and fsrstor instructions, so it includes
   * a copy of the x87 floating-point registers as well.  See FXSAVE in
   * "Intel Architecture Software Developer's Manual, Volume 2." */
  uint8_t              extended_registers[
                         MD_CONTEXT_X86_EXTENDED_REGISTERS_SIZE];
} MDRawContextX86;  /* CONTEXT */

/* For (MDRawContextX86).context_flags.  These values indicate the type of
 * context stored in the structure.  The high 24 bits identify the CPU, the
 * low 8 bits identify the type of context saved. */
#define MD_CONTEXT_X86                    0x00010000
     /* CONTEXT_i386, CONTEXT_i486: identifies CPU */
#define MD_CONTEXT_X86_CONTROL            (MD_CONTEXT_X86 | 0x00000001)
     /* CONTEXT_CONTROL */
#define MD_CONTEXT_X86_INTEGER            (MD_CONTEXT_X86 | 0x00000002)
     /* CONTEXT_INTEGER */
#define MD_CONTEXT_X86_SEGMENTS           (MD_CONTEXT_X86 | 0x00000004)
     /* CONTEXT_SEGMENTS */
#define MD_CONTEXT_X86_FLOATING_POINT     (MD_CONTEXT_X86 | 0x00000008)
     /* CONTEXT_FLOATING_POINT */
#define MD_CONTEXT_X86_DEBUG_REGISTERS    (MD_CONTEXT_X86 | 0x00000010)
     /* CONTEXT_DEBUG_REGISTERS */
#define MD_CONTEXT_X86_EXTENDED_REGISTERS (MD_CONTEXT_X86 | 0x00000020)
     /* CONTEXT_EXTENDED_REGISTERS */
#define MD_CONTEXT_X86_XSTATE             (MD_CONTEXT_X86 | 0x00000040)
     /* CONTEXT_XSTATE */

#define MD_CONTEXT_X86_FULL              (MD_CONTEXT_X86_CONTROL | \
                                          MD_CONTEXT_X86_INTEGER | \
                                          MD_CONTEXT_X86_SEGMENTS)
     /* CONTEXT_FULL */

#define MD_CONTEXT_X86_ALL               (MD_CONTEXT_X86_FULL | \
                                          MD_CONTEXT_X86_FLOATING_POINT | \
                                          MD_CONTEXT_X86_DEBUG_REGISTERS | \
                                          MD_CONTEXT_X86_EXTENDED_REGISTERS)
     /* CONTEXT_ALL */

#endif /* GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_X86_H__ */
