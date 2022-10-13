/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 Lawrence Esswood
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
#ifndef CHERI_COMPRESSED_CAP_H
#define CHERI_COMPRESSED_CAP_H

#ifdef CC_IS_MORELLO
#error "Use new cc128m/CC128M definitions instead of defining CC_IS_MORELLO"
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "cheri_compressed_cap_64.h"
#include "cheri_compressed_cap_128.h"
#include "cheri_compressed_cap_128m.h"

/* Legacy CHERI256 things: */

/* For CHERI256 all permissions are shifted by one since the sealed bit comes first */
#define CC256_HWPERMS_COUNT (12)
#define CC256_HWPERMS_RESERVED_COUNT (3)
#define CC256_UPERMS_COUNT (16)
#define CC256_MAX_UPERM CC256_UPERMS_COUNT
#define CC256_FLAGS_COUNT (1)
#define CC256_FLAGS_ALL_BITS _CC_BITMASK64(CC256_FLAGS_COUNT) /* 1 bit */
#define CC256_PERMS_MEM_SHFT CC256_FLAGS_COUNT                /* flags bit comes first */
#define CC256_UPERMS_MEM_SHFT (CC256_PERMS_MEM_SHFT + CC256_HWPERMS_COUNT + CC256_HWPERMS_RESERVED_COUNT)
#define CC256_UPERMS_SHFT             (15)
_CC_STATIC_ASSERT(CC128_UPERMS_SHFT >=  CC256_HWPERMS_COUNT + CC256_HWPERMS_RESERVED_COUNT, "");
_CC_STATIC_ASSERT_SAME(CC256_PERMS_MEM_SHFT + CC256_HWPERMS_COUNT + CC256_HWPERMS_RESERVED_COUNT, CC256_UPERMS_MEM_SHFT);
#define CC256_PERMS_ALL_BITS _CC_BITMASK64(CC256_HWPERMS_COUNT)                                         /* 12 bits */
#define CC256_PERMS_ALL_BITS_UNTAGGED _CC_BITMASK64(CC256_HWPERMS_COUNT + CC256_HWPERMS_RESERVED_COUNT) /* 15 bits */
#define CC256_UPERMS_ALL_BITS _CC_BITMASK64(CC256_UPERMS_COUNT)                                         /* 19 bits */
#define CC256_OTYPE_ALL_BITS _CC_BITMASK64(CC256_OTYPE_BITS)
#define CC256_RESERVED_ALL_BITS _CC_BITMASK64(CC256_RESERVED_BITS)
#define CC256_OTYPE_MEM_SHFT (32)
_CC_STATIC_ASSERT_SAME(CC256_UPERMS_MEM_SHFT + CC256_UPERMS_COUNT, CC256_OTYPE_MEM_SHFT);
#define CC256_OTYPE_BITS (24)
#define CC256_RESERVED_MEM_SHFT (CC256_OTYPE_MEM_SHFT + CC256_OTYPE_BITS)
#define CC256_RESERVED_BITS (8)
#define CC256_NULL_LENGTH ((cc128_length_t)UINT64_MAX)
#define CC256_NULL_TOP ((cc128_length_t)UINT64_MAX)
_CC_STATIC_ASSERT_SAME(CC256_FLAGS_COUNT + CC256_HWPERMS_COUNT + CC256_HWPERMS_RESERVED_COUNT + CC256_UPERMS_COUNT +
                        CC256_OTYPE_BITS + CC256_RESERVED_BITS,
                       64);

#define CC256_SPECIAL_OTYPE(name, subtract) CC256_##name = (CC256_MAX_REPRESENTABLE_OTYPE - subtract##u)
// Reserve 16 otypes
enum CC256_OTypes {
    CC256_MAX_REPRESENTABLE_OTYPE = ((1u << CC256_OTYPE_BITS) - 1u),
    CC256_SPECIAL_OTYPE(OTYPE_UNSEALED, 0),
    CC256_SPECIAL_OTYPE(OTYPE_SENTRY, 1),
    CC256_SPECIAL_OTYPE(MAX_RESERVED_OTYPE, 0),
    CC256_SPECIAL_OTYPE(MIN_RESERVED_OTYPE, 15),
};

#define CC256_LS_SPECIAL_OTYPES(ITEM, ...)                                                                             \
    ITEM(OTYPE_UNSEALED, __VA_ARGS__)                                                                                  \
    ITEM(OTYPE_SENTRY, __VA_ARGS__)

typedef struct cc256_cap {
    uint64_t cr_cursor;
    uint64_t cr_base;
    uint64_t cr_length;
    uint32_t cr_otype;
    uint16_t cr_perms;
    uint16_t cr_uperms;
    uint8_t cr_tag;
    uint8_t cr_flags;
    uint8_t cr_reserved;
#ifdef __cplusplus
    inline uint64_t base() const { return cr_base; }
    inline uint64_t address() const { return cr_cursor; }
    inline int64_t offset() const { return cr_cursor - cr_base; }
    inline cc128_length_t top() const { return (cc128_length_t)cr_base + cr_length; }
    inline cc128_addr_t top64() const {
        const cc128_length_t t = top();
        return t > CC128_MAX_ADDR ? CC128_MAX_ADDR : (cc128_addr_t)t;
    }
    inline uint64_t length() const { return cr_length; }
    inline uint64_t length64() const { return cr_length; }
    inline uint32_t software_permissions() const { return cr_uperms; }
    inline uint32_t permissions() const { return cr_perms; }
    inline uint32_t type() const { return cr_otype; }
    inline bool is_sealed() const { return type() != CC256_OTYPE_UNSEALED; }
    inline uint8_t flags() const { return cr_flags; }
    inline uint8_t reserved_bits() const { return cr_reserved; }
#endif
} cc256_cap_t;

/* Also support decoding of the raw 256-bit capabilities */
typedef union _inmemory_chericap256 {
    uint8_t bytes[32];
    uint32_t u32s[8];
    uint64_t u64s[4];
} inmemory_chericap256;

static inline bool cc256_is_cap_sealed(const cc256_cap_t* cp) { return cp->cr_otype != CC256_OTYPE_UNSEALED; }

static inline void decompress_256cap(inmemory_chericap256 mem, cc256_cap_t* cdp, bool tagged) {
    /* See CHERI ISA: Figure 3.1: 256-bit memory representation of a capability */
    uint32_t hwperms_mask = tagged ? CC256_PERMS_ALL_BITS : CC256_PERMS_ALL_BITS_UNTAGGED;
    cdp->cr_flags = mem.u64s[0] & CC256_FLAGS_ALL_BITS;
    cdp->cr_perms = (mem.u64s[0] >> CC256_PERMS_MEM_SHFT) & hwperms_mask;
    cdp->cr_uperms = (mem.u64s[0] >> CC256_UPERMS_MEM_SHFT) & CC256_UPERMS_ALL_BITS;
    cdp->cr_otype = ((mem.u64s[0] >> CC256_OTYPE_MEM_SHFT) & CC256_OTYPE_ALL_BITS) ^ CC256_OTYPE_UNSEALED;
    cdp->cr_reserved = (mem.u64s[0] >> CC256_RESERVED_MEM_SHFT) & CC256_RESERVED_ALL_BITS;
    cdp->cr_base = mem.u64s[2];
    /* Length is xor'ed with -1 to ensure that NULL is all zeroes in memory */
    uint64_t length = mem.u64s[3] ^ CC256_NULL_LENGTH;
    cdp->cr_length = length;
    cdp->cr_cursor = mem.u64s[1];
    cdp->cr_tag = tagged;
    _cc_debug_assert(!(cdp->cr_tag && cdp->cr_reserved) && "Unknown reserved bits set it tagged capability");
}

static inline void compress_256cap(inmemory_chericap256* buffer, const cc256_cap_t* csp) {
    _cc_debug_assert(!(csp->cr_tag && csp->cr_reserved) && "Unknown reserved bits set it tagged capability");
    bool flags_bits = csp->cr_flags & CC256_FLAGS_ALL_BITS;
    // When writing an untagged value, just write back the bits that were loaded (including the reserved HWPERMS)
    uint64_t hwperms_mask = csp->cr_tag ? CC256_PERMS_ALL_BITS : CC256_PERMS_ALL_BITS_UNTAGGED;
    buffer->u64s[0] =
        flags_bits | ((csp->cr_perms & hwperms_mask) << CC256_PERMS_MEM_SHFT) |
        ((csp->cr_uperms & CC256_UPERMS_ALL_BITS) << CC256_UPERMS_MEM_SHFT) |
        ((uint64_t)((csp->cr_otype ^ CC256_OTYPE_UNSEALED) & CC256_OTYPE_ALL_BITS) << CC256_OTYPE_MEM_SHFT) |
        ((uint64_t)(csp->cr_reserved & CC256_RESERVED_ALL_BITS) << CC256_RESERVED_MEM_SHFT);
    buffer->u64s[1] = csp->cr_cursor;
    buffer->u64s[2] = csp->cr_base;
    buffer->u64s[3] = csp->cr_length ^ CC256_NULL_LENGTH;
}

#endif /* CHERI_COMPRESSED_CAP_H */
