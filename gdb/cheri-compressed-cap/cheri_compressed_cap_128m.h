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

/* Notes on Morello Vs Other CHERI platforms:
 * The Morello Psuedo-code thinks of fields as inverted on each use, rather than being
 * XOR-ED as they come from memory. To keep the style the same, I am inverting on load
 * like we do with other CHERI platforms.
 * Morello does NOT invert otype.
 *
 * An all 1's exponent, REGARDLESS of other bits in top and bottom, means max length
 * cap!
 * This gives rise to a whole bunch of non-canon max length caps in buildcap =(
 *
 * Flag bits are the same as top byte ignore bits.
 * When adding to the address, representability checks mask the address AND the increment
 * For bounds calculation, flags are ignored, and 56-bit addresses are SIGN EXTENDED?!?!?
 * To avoid bugs around this, the highest non-flag bit is not allowed to change
 *
 * This means doing a set bounds on a cap with this bit set is out of bounds???
 * Also, caps with flag bits in the address are out of bounds, if length around 2^56 (unless they match the 58th bit)
 *
 * Morello embeds the user permissions in the middle of the hardware permissions.
 */
#define CC_IS_MORELLO

// The following macros are expected to be defined
#define CC_FORMAT_LOWER 128m
#define CC_FORMAT_UPPER 128M
/* These should match the definitions in sail! */
#define CC128M_CAP_SIZE 16
#define CC128M_CAP_BITS 128
#define CC128M_ADDR_WIDTH 64
#define CC128M_LEN_WIDTH 65
/* Max exponent is the largest exponent _required_, not that can be encoded. */
#define CC128M_MANTISSA_WIDTH 16
#define CC128M_MAX_EXPONENT 50
#define CC128M_MAX_ENCODABLE_EXPONENT 63
#define CC128M_CURSOR_MASK 0x00FFFFFFFFFFFFFF
#define CC128M_MAX_ADDRESS_PLUS_ONE ((cc128m_length_t)1u << CC128M_ADDR_WIDTH)
#define CC128M_NULL_TOP CC128M_MAX_ADDRESS_PLUS_ONE
#define CC128M_NULL_LENGTH CC128M_MAX_ADDRESS_PLUS_ONE
#define CC128M_MAX_LENGTH CC128M_MAX_ADDRESS_PLUS_ONE
#define CC128M_MAX_TOP CC128M_MAX_ADDRESS_PLUS_ONE
#define CC128M_MAX_ADDR UINT64_MAX
/* Special otypes are allocated upwards from 0 */
#define CC128M_SPECIAL_OTYPE_VAL(val) (val##u)
#define CC128M_SPECIAL_OTYPE_VAL_SIGNED(val) (val##u)

/* Use __uint128 to represent 65 bit length */
__extension__ typedef unsigned __int128 cc128m_length_t;
__extension__ typedef signed __int128 cc128m_offset_t;
typedef uint64_t cc128m_addr_t;
#include "cheri_compressed_cap_macros.h"

/* ignore ISO C restricts enumerator values to range of 'int' */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
enum {
    // Morello HW perms actually 127..116, and 111...100. But seperating the fields is just a headache, and would make
    // other code more complex. Pretend that they are all HW Perms.
    _CC_FIELD(HWPERMS, 127, 110),
    _CC_FIELD(UPERMS, 111, 112),
    // Should _CC_FIELD(UPERMS, 115, 112), if that wouldn't cause a double count because of above.
    _CC_FIELD(OTYPE, 109, 95),
    _CC_FIELD(EBT, 94, 64),
// This is a bit dodgy. This enum only really works for non-address bits.
// Just provide nonsense values that will make the length of the range 0.
// Should really be `_CC_FIELD(FLAGS, 63, 56)', if this stuff applied to the address
#define MORELLO_FLAG_BITS 8
    _CC_FIELD(FLAGS, 64, 65),
    _CC_FIELD(RESERVED, 64, 65),

    _CC_FIELD(INTERNAL_EXPONENT, 94, 94),
    _CC_FIELD(TOP_ENCODED, 93, 80),
    _CC_FIELD(BOTTOM_ENCODED, 79, 64),

    // Top/bottom offsets depending in INTERNAL_EXPONENT flag:
    // Without internal exponent:
    _CC_FIELD(EXP_ZERO_TOP, 93, 80),
    _CC_FIELD(EXP_ZERO_BOTTOM, 79, 64),
    // With internal exponent:
    _CC_FIELD(EXP_NONZERO_TOP, 93, 83),
    _CC_FIELD(EXPONENT_HIGH_PART, 82, 80),
    _CC_FIELD(EXP_NONZERO_BOTTOM, 79, 67),
    _CC_FIELD(EXPONENT_LOW_PART, 66, 64),
};
#pragma GCC diagnostic pop

#define CC128M_OTYPE_BITS CC128M_FIELD_OTYPE_SIZE
#define CC128M_BOT_WIDTH CC128M_FIELD_EXP_ZERO_BOTTOM_SIZE
#define CC128M_BOT_INTERNAL_EXP_WIDTH CC128M_FIELD_EXP_NONZERO_BOTTOM_SIZE
#define CC128M_EXP_LOW_WIDTH CC128M_FIELD_EXPONENT_LOW_PART_SIZE

#define CC128M_PERM_GLOBAL (1 << 0)
#define CC128M_PERM_EXECUTIVE (1 << 1)
// Then 4 user types
#define CC128M_PERM_MUTABLE_LOAD (1 << 6)
#define CC128M_PERM_SETCID (1 << 7)
#define CC128M_PERM_BRANCH_SEALED_PAIR (1 << 8)
#define CC128M_PERM_CINVOKE CC128M_PERM_BRANCH_SEALED_PAIR
#define CC128M_PERM_SYSTEM (1 << 9)
#define CC128M_PERM_ACCESS_SYS_REGS CC128M_PERM_SYSTEM
#define CC128M_PERM_UNSEAL (1 << 10)
#define CC128M_PERM_SEAL (1 << 11)
#define CC128M_PERM_STORE_LOCAL (1 << 12)
#define CC128M_PERM_STORE_CAP (1 << 13)
#define CC128M_PERM_LOAD_CAP (1 << 14)
#define CC128M_PERM_EXECUTE (1 << 15)
#define CC128M_PERM_STORE (1 << 16)
#define CC128M_PERM_LOAD (1 << 17)

#define CC128M_HIGHEST_PERM CC128M_PERM_LOAD

_CC_STATIC_ASSERT(CC128M_HIGHEST_PERM < CC128M_FIELD_HWPERMS_MAX_VALUE, "permissions not representable?");
_CC_STATIC_ASSERT((CC128M_HIGHEST_PERM << 1) > CC128M_FIELD_HWPERMS_MAX_VALUE, "all permission bits should be used");

#define CC128M_PERMS_ALL (0x3FFFF) /* [0...1,6..17] */
#define CC128M_UPERMS_ALL (0x0)    /* [15...18] */
#define CC128M_UPERMS_SHFT (0)
#define CC128M_MAX_UPERM (3)

/* Morello calls the special otypes LB, LPB and RB.
 * LPB is "load pair branch". It loads the first two caps pointed to and ccalls them.
 * LB is "load branch". It loads, plus an immediate, the cap pointed to and jumps to it.
 * RB is a sentry. God knows what it stands for. Restricted Branch?
 */

// We reserve 3 otypes
enum _CC_N(OTypes) {
    CC128M_FIRST_NONRESERVED_OTYPE = 0,
    CC128M_MAX_REPRESENTABLE_OTYPE = ((1u << CC128M_OTYPE_BITS) - 1u),
    _CC_SPECIAL_OTYPE(OTYPE_UNSEALED, 0),
    _CC_SPECIAL_OTYPE(OTYPE_SENTRY, 1),
    _CC_SPECIAL_OTYPE(OTYPE_LOAD_PAIR_BRANCH, 2),
    _CC_SPECIAL_OTYPE(OTYPE_LOAD_BRANCH, 3),
    _CC_N(MIN_RESERVED_OTYPE) = _CC_N(OTYPE_UNSEALED),
    _CC_N(MAX_RESERVED_OTYPE) = _CC_N(OTYPE_LOAD_BRANCH),
};

#define CC128M_LS_SPECIAL_OTYPES(ITEM, ...)                                                                            \
    ITEM(OTYPE_UNSEALED, __VA_ARGS__)                                                                                  \
    ITEM(OTYPE_SENTRY, __VA_ARGS__)                                                                                    \
    ITEM(OTYPE_LOAD_PAIR_BRANCH, __VA_ARGS__)                                                                          \
    ITEM(OTYPE_LOAD_BRANCH, __VA_ARGS__)

_CC_STATIC_ASSERT_SAME(CC128M_MANTISSA_WIDTH, CC128M_FIELD_EXP_ZERO_BOTTOM_SIZE);

#include "cheri_compressed_cap_common.h"

// Sanity-check mask is the expected NULL encoding
_CC_STATIC_ASSERT_SAME(CC128M_NULL_XOR_MASK, UINT64_C(0x0000000040070007));

#define CC128M_FIELD(name, last, start) _CC_FIELD(name, last, start)
#define CC128M_ENCODE_FIELD(value, name) _CC_ENCODE_FIELD(value, name)
#define CC128M_EXTRACT_FIELD(value, name) _CC_EXTRACT_FIELD(value, name)
#define CC128M_ENCODE_EBT_FIELD(value, name) _CC_ENCODE_EBT_FIELD(value, name)

#undef CC_IS_MORELLO
#undef CC_FORMAT_LOWER
#undef CC_FORMAT_UPPER
