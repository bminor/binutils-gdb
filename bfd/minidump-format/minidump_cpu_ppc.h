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
 * produced on ppc.  These files may be read on any platform provided
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
 * Breakpad minidump extension for PowerPC support.  Based on Darwin/Mac OS X'
 * mach/ppc/_types.h
 */

#ifndef GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_PPC_H__
#define GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_PPC_H__

#define MD_FLOATINGSAVEAREA_PPC_FPR_COUNT 32

typedef struct {
  /* fpregs is a double[32] in mach/ppc/_types.h, but a uint64_t is used
   * here for precise sizing. */
  uint64_t fpregs[MD_FLOATINGSAVEAREA_PPC_FPR_COUNT];
  uint32_t fpscr_pad;
  uint32_t fpscr;      /* Status/control */
} MDFloatingSaveAreaPPC;  /* Based on ppc_float_state */


#define MD_VECTORSAVEAREA_PPC_VR_COUNT 32

typedef struct {
  /* Vector registers (including vscr) are 128 bits, but mach/ppc/_types.h
   * exposes them as four 32-bit quantities. */
  uint128_struct save_vr[MD_VECTORSAVEAREA_PPC_VR_COUNT];
  uint128_struct save_vscr;  /* Status/control */
  uint32_t       save_pad5[4];
  uint32_t       save_vrvalid;  /* Indicates which vector registers are saved */
  uint32_t       save_pad6[7];
} MDVectorSaveAreaPPC;  /* ppc_vector_state */


#define MD_CONTEXT_PPC_GPR_COUNT 32

/* Use the same 32-bit alignment when accessing this structure from 64-bit code
 * as is used natively in 32-bit code.  #pragma pack is a MSVC extension
 * supported by gcc. */
#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

typedef struct {
  /* context_flags is not present in ppc_thread_state, but it aids
   * identification of MDRawContextPPC among other raw context types,
   * and it guarantees alignment when we get to float_save. */
  uint32_t              context_flags;

  uint32_t              srr0;    /* Machine status save/restore: stores pc
                                  * (instruction) */
  uint32_t              srr1;    /* Machine status save/restore: stores msr
                                  * (ps, program/machine state) */
  /* ppc_thread_state contains 32 fields, r0 .. r31.  Here, an array is
   * used for brevity. */
  uint32_t              gpr[MD_CONTEXT_PPC_GPR_COUNT];
  uint32_t              cr;      /* Condition */
  uint32_t              xer;     /* Integer (fiXed-point) exception */
  uint32_t              lr;      /* Link */
  uint32_t              ctr;     /* Count */
  uint32_t              mq;      /* Multiply/Quotient (PPC 601, POWER only) */
  uint32_t              vrsave;  /* Vector save */

  /* float_save and vector_save aren't present in ppc_thread_state, but
   * are represented in separate structures that still define a thread's
   * context. */
  MDFloatingSaveAreaPPC float_save;
  MDVectorSaveAreaPPC   vector_save;
} MDRawContextPPC;  /* Based on ppc_thread_state */

/* Indices into gpr for registers with a dedicated or conventional purpose. */
enum MDPPCRegisterNumbers {
  MD_CONTEXT_PPC_REG_SP = 1
};

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma pack(0)
#else
#pragma pack(pop)
#endif

/* For (MDRawContextPPC).context_flags.  These values indicate the type of
 * context stored in the structure.  MD_CONTEXT_PPC is Breakpad-defined.  Its
 * value was chosen to avoid likely conflicts with MD_CONTEXT_* for other
 * CPUs. */
#define MD_CONTEXT_PPC                0x20000000
#define MD_CONTEXT_PPC_BASE           (MD_CONTEXT_PPC | 0x00000001)
#define MD_CONTEXT_PPC_FLOATING_POINT (MD_CONTEXT_PPC | 0x00000008)
#define MD_CONTEXT_PPC_VECTOR         (MD_CONTEXT_PPC | 0x00000020)

#define MD_CONTEXT_PPC_FULL           MD_CONTEXT_PPC_BASE
#define MD_CONTEXT_PPC_ALL            (MD_CONTEXT_PPC_FULL | \
                                       MD_CONTEXT_PPC_FLOATING_POINT | \
                                       MD_CONTEXT_PPC_VECTOR)

#endif /* GOOGLE_BREAKPAD_COMMON_MINIDUMP_CPU_PPC_H__ */
