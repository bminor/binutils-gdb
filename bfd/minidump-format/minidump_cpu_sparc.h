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
 * produced on sparc.  These files may be read on any platform provided
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
 * Author: Mark Mentovai
 * Change to split into its own file: Neal Sidhwaney */

/*
 * SPARC support, see (solaris)sys/procfs_isa.h also
 */

#ifndef GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_SPARC_H__
#define GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_SPARC_H__

#define MD_FLOATINGSAVEAREA_SPARC_FPR_COUNT 32

typedef struct {

  /* FPU floating point regs */
  uint64_t      regs[MD_FLOATINGSAVEAREA_SPARC_FPR_COUNT];

  uint64_t      filler;
  uint64_t      fsr;        /* FPU status register */
} MDFloatingSaveAreaSPARC;  /* FLOATING_SAVE_AREA */

#define MD_CONTEXT_SPARC_GPR_COUNT 32

typedef struct {
  /* The next field determines the layout of the structure, and which parts
   * of it are populated
   */
  uint32_t      context_flags;
  uint32_t      flag_pad;
  /*
   * General register access (SPARC).
   * Don't confuse definitions here with definitions in <sys/regset.h>.
   * Registers are 32 bits for ILP32, 64 bits for LP64.
   * SPARC V7/V8 is for 32bit, SPARC V9 is for 64bit
   */

  /* 32 Integer working registers */

  /* g_r[0-7]   global registers(g0-g7)
   * g_r[8-15]  out registers(o0-o7)
   * g_r[16-23] local registers(l0-l7)
   * g_r[24-31] in registers(i0-i7)
   */
  uint64_t     g_r[MD_CONTEXT_SPARC_GPR_COUNT];

  /* several control registers */

  /* Processor State register(PSR) for SPARC V7/V8
   * Condition Code register (CCR) for SPARC V9
   */
  uint64_t     ccr;

  uint64_t     pc;     /* Program Counter register (PC) */
  uint64_t     npc;    /* Next Program Counter register (nPC) */
  uint64_t     y;      /* Y register (Y) */

  /* Address Space Identifier register (ASI) for SPARC V9
   * WIM for SPARC V7/V8
   */
  uint64_t     asi;

  /* Floating-Point Registers State register (FPRS) for SPARC V9
   * TBR for for SPARC V7/V8
   */
  uint64_t     fprs;

  /* The next field is included with MD_CONTEXT_SPARC_FLOATING_POINT */
  MDFloatingSaveAreaSPARC float_save;

} MDRawContextSPARC;  /* CONTEXT_SPARC */

/* Indices into g_r for registers with a dedicated or conventional purpose. */
enum MDSPARCRegisterNumbers {
  MD_CONTEXT_SPARC_REG_SP = 14
};

/* For (MDRawContextSPARC).context_flags.  These values indicate the type of
 * context stored in the structure.  MD_CONTEXT_SPARC is Breakpad-defined.  Its
 * value was chosen to avoid likely conflicts with MD_CONTEXT_* for other
 * CPUs. */
#define MD_CONTEXT_SPARC                 0x10000000
#define MD_CONTEXT_SPARC_CONTROL         (MD_CONTEXT_SPARC | 0x00000001)
#define MD_CONTEXT_SPARC_INTEGER         (MD_CONTEXT_SPARC | 0x00000002)
#define MD_CONTEXT_SAPARC_FLOATING_POINT (MD_CONTEXT_SPARC | 0x00000004)
#define MD_CONTEXT_SAPARC_EXTRA          (MD_CONTEXT_SPARC | 0x00000008)

#define MD_CONTEXT_SPARC_FULL            (MD_CONTEXT_SPARC_CONTROL | \
                                          MD_CONTEXT_SPARC_INTEGER)

#define MD_CONTEXT_SPARC_ALL             (MD_CONTEXT_SPARC_FULL | \
                                          MD_CONTEXT_SAPARC_FLOATING_POINT | \
                                          MD_CONTEXT_SAPARC_EXTRA)

#endif /* GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_SPARC_H__ */
