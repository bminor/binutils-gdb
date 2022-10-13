/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018-2020 Alex Richardson
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract FA8750-10-C-0237
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory (Department of Computer Science and
 * Technology) under DARPA contract HR0011-18-C-0016 ("ECATS"), as part of the
 * DARPA SSITH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

// The following macros are expected to be defined
#define CC_FORMAT_LOWER 64
#define CC_FORMAT_UPPER 64
/* These should match the definitions in sail! */
#define CC64_CAP_SIZE 8
#define CC64_CAP_BITS 64
#define CC64_ADDR_WIDTH 32
#define CC64_LEN_WIDTH 33
#define CC64_MANTISSA_WIDTH 8
#define CC64_MAX_EXPONENT 26
#define CC64_CURSOR_MASK 0xFFFFFFFF

#define CC64_MAX_ADDRESS_PLUS_ONE ((cc64_length_t)1u << CC64_ADDR_WIDTH)
#define CC64_NULL_TOP CC64_MAX_ADDRESS_PLUS_ONE
#define CC64_NULL_LENGTH CC64_MAX_ADDRESS_PLUS_ONE
#define CC64_MAX_LENGTH CC64_MAX_ADDRESS_PLUS_ONE
#define CC64_MAX_TOP CC64_MAX_ADDRESS_PLUS_ONE
#define CC64_MAX_ADDR UINT32_MAX
/* Special otypes are allocated downwards from -1 */
#define CC64_SPECIAL_OTYPE_VAL(subtract) (CC64_MAX_REPRESENTABLE_OTYPE - subtract##u)
#define CC64_SPECIAL_OTYPE_VAL_SIGNED(subtract) (((int64_t)-1) - subtract##u)

/* Use uint64_t to represent 33 bit length */
typedef uint64_t cc64_length_t;
typedef int64_t cc64_offset_t;
typedef uint32_t cc64_addr_t;
#include "cheri_compressed_cap_macros.h"

/* ignore ISO C restricts enumerator values to range of 'int' */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
enum {
    _CC_FIELD(HWPERMS, 63, 52),
    _CC_FIELD(FLAGS, 51, 51),
    _CC_FIELD(OTYPE, 50, 47),
    _CC_FIELD(EBT, 46, 32),

    _CC_FIELD(INTERNAL_EXPONENT, 46, 46),
    _CC_FIELD(TOP_ENCODED, 45, 40),
    _CC_FIELD(BOTTOM_ENCODED, 39, 32),

    // Top/bottom offsets depending in INTERNAL_EXPONENT flag:
    // Without internal exponent:
    _CC_FIELD(EXP_ZERO_TOP, 45, 40),
    _CC_FIELD(EXP_ZERO_BOTTOM, 39, 32),
    // With internal exponent:
    _CC_FIELD(EXP_NONZERO_TOP, 45, 43),
    _CC_FIELD(EXPONENT_HIGH_PART, 42, 40),
    _CC_FIELD(EXP_NONZERO_BOTTOM, 39, 35),
    _CC_FIELD(EXPONENT_LOW_PART, 34, 32),
    _CC_FIELD(RESERVED, 31, 32), /* No reserved bits */
    _CC_FIELD(UPERMS, 31, 32), /* No uperms */
};
#pragma GCC diagnostic pop
_CC_STATIC_ASSERT_SAME(CC64_FIELD_UPERMS_SIZE, 0);
_CC_STATIC_ASSERT_SAME(CC64_FIELD_RESERVED_SIZE, 0);

#define CC64_OTYPE_BITS CC64_FIELD_OTYPE_SIZE
#define CC64_BOT_WIDTH CC64_FIELD_EXP_ZERO_BOTTOM_SIZE
#define CC64_BOT_INTERNAL_EXP_WIDTH CC64_FIELD_EXP_NONZERO_BOTTOM_SIZE
#define CC64_EXP_LOW_WIDTH CC64_FIELD_EXPONENT_LOW_PART_SIZE

#define CC64_PERM_GLOBAL (1 << 0)
#define CC64_PERM_EXECUTE (1 << 1)
#define CC64_PERM_LOAD (1 << 2)
#define CC64_PERM_STORE (1 << 3)
#define CC64_PERM_LOAD_CAP (1 << 4)
#define CC64_PERM_STORE_CAP (1 << 5)
#define CC64_PERM_STORE_LOCAL (1 << 6)
#define CC64_PERM_SEAL (1 << 7)
#define CC64_PERM_CINVOKE (1 << 8)
#define CC64_PERM_UNSEAL (1 << 9)
#define CC64_PERM_ACCESS_SYS_REGS (1 << 10)
#define CC64_PERM_SETCID (1 << 11)
_CC_STATIC_ASSERT(CC64_PERM_SETCID < CC64_FIELD_HWPERMS_MAX_VALUE, "permissions not representable?");

#define CC64_PERMS_ALL (0xfff) /* [0...11] */
#define CC64_UPERMS_ALL (0)  /* [15...18] */
#define CC64_UPERMS_SHFT (15)
#define CC64_MAX_UPERM (0)

// We reserve 16 otypes
enum _CC_N(OTypes) {
    CC64_MAX_REPRESENTABLE_OTYPE = ((1u << CC64_OTYPE_BITS) - 1u),
    _CC_SPECIAL_OTYPE(OTYPE_UNSEALED, 0),
    _CC_SPECIAL_OTYPE(OTYPE_SENTRY, 1),
    _CC_SPECIAL_OTYPE(OTYPE_RESERVED2, 2),
    _CC_SPECIAL_OTYPE(OTYPE_RESERVED3, 3),
    _CC_N(MIN_RESERVED_OTYPE) = _CC_N(OTYPE_RESERVED3),
    _CC_N(MAX_RESERVED_OTYPE) = _CC_N(OTYPE_UNSEALED),
};

#define CC64_LS_SPECIAL_OTYPES(ITEM, ...)                                                                              \
    ITEM(OTYPE_UNSEALED, __VA_ARGS__)                                                                                  \
    ITEM(OTYPE_SENTRY, __VA_ARGS__)

_CC_STATIC_ASSERT_SAME(CC64_MANTISSA_WIDTH, CC64_FIELD_EXP_ZERO_BOTTOM_SIZE);

#include "cheri_compressed_cap_common.h"

// Sanity-check mask is the expected NULL encoding
_CC_STATIC_ASSERT_SAME(CC64_NULL_XOR_MASK, UINT32_C(0x7c302));

#define CC64_FIELD(name, last, start) _CC_FIELD(name, last, start)
#define CC64_ENCODE_FIELD(value, name) _CC_ENCODE_FIELD(value, name)
#define CC64_EXTRACT_FIELD(value, name) _CC_EXTRACT_FIELD(value, name)
#define CC64_ENCODE_EBT_FIELD(value, name) _CC_ENCODE_EBT_FIELD(value, name)

#undef CC_FORMAT_LOWER
#undef CC_FORMAT_UPPER
