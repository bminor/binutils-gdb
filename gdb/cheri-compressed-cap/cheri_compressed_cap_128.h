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
#define CC_FORMAT_LOWER 128
#define CC_FORMAT_UPPER 128
/* These should match the definitions in sail! */
#define CC128_CAP_SIZE 16
#define CC128_CAP_BITS 128
#define CC128_ADDR_WIDTH 64
#define CC128_LEN_WIDTH 65
/* Max exponent is the largest exponent _required_, not that can be encoded. */
#define CC128_MANTISSA_WIDTH 14
#define CC128_MAX_EXPONENT 52
#define CC128_CURSOR_MASK 0xFFFFFFFFFFFFFFFF
#define CC128_MAX_ADDRESS_PLUS_ONE ((cc128_length_t)1u << CC128_ADDR_WIDTH)
#define CC128_NULL_TOP CC128_MAX_ADDRESS_PLUS_ONE
#define CC128_NULL_LENGTH CC128_MAX_ADDRESS_PLUS_ONE
#define CC128_MAX_LENGTH CC128_MAX_ADDRESS_PLUS_ONE
#define CC128_MAX_TOP CC128_MAX_ADDRESS_PLUS_ONE
#define CC128_MAX_ADDR UINT64_MAX
/* Special otypes are allocated downwards from -1 */
#define CC128_SPECIAL_OTYPE_VAL(subtract) (CC128_MAX_REPRESENTABLE_OTYPE - subtract##u)
#define CC128_SPECIAL_OTYPE_VAL_SIGNED(subtract) (((int64_t)-1) - subtract##u)

/* Use __uint128 to represent 65 bit length */
__extension__ typedef unsigned __int128 cc128_length_t;
__extension__ typedef signed __int128 cc128_offset_t;
typedef uint64_t cc128_addr_t;
#include "cheri_compressed_cap_macros.h"

/* ignore ISO C restricts enumerator values to range of 'int' */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
enum {
    _CC_FIELD(UPERMS, 127, 124),
    _CC_FIELD(HWPERMS, 123, 112),
    _CC_FIELD(RESERVED, 111, 110),
    _CC_FIELD(FLAGS, 109, 109),
    _CC_FIELD(OTYPE, 108, 91),
    _CC_FIELD(EBT, 90, 64),

    _CC_FIELD(INTERNAL_EXPONENT, 90, 90),
    _CC_FIELD(TOP_ENCODED, 89, 78),
    _CC_FIELD(BOTTOM_ENCODED, 77, 64),

    // Top/bottom offsets depending in INTERNAL_EXPONENT flag:
    // Without internal exponent:
    _CC_FIELD(EXP_ZERO_TOP, 89, 78),
    _CC_FIELD(EXP_ZERO_BOTTOM, 77, 64),
    // With internal exponent:
    _CC_FIELD(EXP_NONZERO_TOP, 89, 81),
    _CC_FIELD(EXPONENT_HIGH_PART, 80, 78),
    _CC_FIELD(EXP_NONZERO_BOTTOM, 77, 67),
    _CC_FIELD(EXPONENT_LOW_PART, 66, 64),
};
#pragma GCC diagnostic pop

#define CC128_OTYPE_BITS CC128_FIELD_OTYPE_SIZE
#define CC128_BOT_WIDTH CC128_FIELD_EXP_ZERO_BOTTOM_SIZE
#define CC128_BOT_INTERNAL_EXP_WIDTH CC128_FIELD_EXP_NONZERO_BOTTOM_SIZE
#define CC128_EXP_LOW_WIDTH CC128_FIELD_EXPONENT_LOW_PART_SIZE

#define CC128_PERM_GLOBAL (1 << 0)
#define CC128_PERM_EXECUTE (1 << 1)
#define CC128_PERM_LOAD (1 << 2)
#define CC128_PERM_STORE (1 << 3)
#define CC128_PERM_LOAD_CAP (1 << 4)
#define CC128_PERM_STORE_CAP (1 << 5)
#define CC128_PERM_STORE_LOCAL (1 << 6)
#define CC128_PERM_SEAL (1 << 7)
#define CC128_PERM_CINVOKE (1 << 8)
#define CC128_PERM_UNSEAL (1 << 9)
#define CC128_PERM_ACCESS_SYS_REGS (1 << 10)
#define CC128_PERM_SETCID (1 << 11)

#define CC128_HIGHEST_PERM CC128_PERM_SETCID

_CC_STATIC_ASSERT(CC128_HIGHEST_PERM < CC128_FIELD_HWPERMS_MAX_VALUE, "permissions not representable?");
_CC_STATIC_ASSERT((CC128_HIGHEST_PERM << 1) > CC128_FIELD_HWPERMS_MAX_VALUE, "all permission bits should be used");

#define CC128_PERMS_ALL (0xfff) /* [0...11] */
#define CC128_UPERMS_ALL (0xf)  /* [15...18] */
#define CC128_UPERMS_SHFT (15)
#define CC128_UPERMS_MEM_SHFT (12)
#define CC128_MAX_UPERM (3)

// We reserve 16 otypes
enum _CC_N(OTypes) {
    CC128_FIRST_NONRESERVED_OTYPE = 0,
    CC128_MAX_REPRESENTABLE_OTYPE = ((1u << CC128_OTYPE_BITS) - 1u),
    _CC_SPECIAL_OTYPE(OTYPE_UNSEALED, 0),
    _CC_SPECIAL_OTYPE(OTYPE_SENTRY, 1),
    _CC_SPECIAL_OTYPE(OTYPE_INDIRECT_PAIR, 2),
    _CC_SPECIAL_OTYPE(OTYPE_INDIRECT_SENTRY, 3),
    _CC_SPECIAL_OTYPE(OTYPE_RESERVED_LAST, 15),
    /*
     * We allocate otypes subtracting from the maximum value, so the smallest is
     * actually the one with the largest _CC_SPECIAL_OTYPE() argument.
     * With 16 reserved otypes, this is currently -16, i.e. 0x3fff0.
     */
    _CC_N(MIN_RESERVED_OTYPE) = _CC_N(OTYPE_RESERVED_LAST),
    _CC_N(MAX_RESERVED_OTYPE) = _CC_N(OTYPE_UNSEALED),
};

#define CC128_LS_SPECIAL_OTYPES(ITEM, ...)                                                                             \
    ITEM(OTYPE_UNSEALED, __VA_ARGS__)                                                                                  \
    ITEM(OTYPE_SENTRY, __VA_ARGS__)                                                                                    \
    ITEM(OTYPE_INDIRECT_PAIR, __VA_ARGS__)                                                                             \
    ITEM(OTYPE_INDIRECT_SENTRY, __VA_ARGS__)

_CC_STATIC_ASSERT_SAME(CC128_MANTISSA_WIDTH, CC128_FIELD_EXP_ZERO_BOTTOM_SIZE);

#include "cheri_compressed_cap_common.h"

// Sanity-check mask is the expected NULL encoding
_CC_STATIC_ASSERT_SAME(CC128_NULL_XOR_MASK, UINT64_C(0x00001ffffc018004));

__attribute__((deprecated("Use cc128_compress_raw"))) static inline uint64_t
compress_128cap_without_xor(const cc128_cap_t* csp) {
    return cc128_compress_raw(csp);
}

__attribute__((deprecated("Use cc128_compress_mem"))) static inline uint64_t compress_128cap(const cc128_cap_t* csp) {
    return cc128_compress_mem(csp);
}

__attribute__((deprecated("Use cc128_decompress_raw"))) static inline void
decompress_128cap_already_xored(uint64_t pesbt, uint64_t cursor, cc128_cap_t* cdp) {
    cc128_decompress_raw(pesbt, cursor, cdp->cr_tag, cdp);
}

__attribute__((deprecated("Use cc128_decompress_mem"))) static inline void
decompress_128cap(uint64_t pesbt, uint64_t cursor, _cc_cap_t* cdp) {
    cc128_decompress_mem(pesbt, cursor, cdp->cr_tag, cdp);
}

#define CC128_FIELD(name, last, start) _CC_FIELD(name, last, start)
#define CC128_ENCODE_FIELD(value, name) _CC_ENCODE_FIELD(value, name)
#define CC128_EXTRACT_FIELD(value, name) _CC_EXTRACT_FIELD(value, name)
#define CC128_ENCODE_EBT_FIELD(value, name) _CC_ENCODE_EBT_FIELD(value, name)

#undef CC_FORMAT_LOWER
#undef CC_FORMAT_UPPER
