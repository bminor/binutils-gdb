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

#include <stdbool.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
enum {
    // For the reset capability we use an internal exponent and need
    // 2^ADDR_WIDTH, which uses the max exponent.
    _CC_N(RESET_EXP) = _CC_N(MAX_EXPONENT),
    _CC_N(RESET_T) = 1u << (_CC_N(ADDR_WIDTH) - _CC_N(RESET_EXP) - _CC_N(FIELD_EXPONENT_HIGH_PART_SIZE)),
#ifdef CC_IS_MORELLO
    // Due to magic constant XOR aversion (i.e. fields are either entirely
    // inverted or not at all, rather than select bits within them like in
    // normal CHERI Concentrate), NULL is special in Morello.
    _CC_N(NULL_EXP) = _CC_N(MAX_ENCODABLE_EXPONENT),
    _CC_N(NULL_T) = 0,
#else
    // NULL uses identical bounds encoding to the reset capability.
    _CC_N(NULL_EXP) = _CC_N(RESET_EXP),
    _CC_N(NULL_T) = _CC_N(RESET_T),
#endif
    _CC_N(RESET_EBT) =
        _CC_ENCODE_EBT_FIELD(1, INTERNAL_EXPONENT) | _CC_ENCODE_EBT_FIELD(_CC_N(RESET_T), EXP_NONZERO_TOP) |
        _CC_ENCODE_EBT_FIELD(0, EXP_NONZERO_BOTTOM) |
        _CC_ENCODE_EBT_FIELD(_CC_N(RESET_EXP) >> _CC_N(FIELD_EXPONENT_LOW_PART_SIZE), EXPONENT_HIGH_PART) |
        _CC_ENCODE_EBT_FIELD(_CC_N(RESET_EXP) & _CC_N(FIELD_EXPONENT_LOW_PART_MAX_VALUE), EXPONENT_LOW_PART),
    _CC_N(NULL_PESBT) = _CC_ENCODE_FIELD(0, UPERMS) | _CC_ENCODE_FIELD(0, HWPERMS) | _CC_ENCODE_FIELD(0, RESERVED) |
                        _CC_ENCODE_FIELD(0, FLAGS) | _CC_ENCODE_FIELD(1, INTERNAL_EXPONENT) |
                        _CC_ENCODE_FIELD(_CC_N(OTYPE_UNSEALED), OTYPE) |
                        _CC_ENCODE_FIELD(_CC_N(NULL_T), EXP_NONZERO_TOP) | _CC_ENCODE_FIELD(0, EXP_NONZERO_BOTTOM) |
                        _CC_ENCODE_FIELD(_CC_N(NULL_EXP) >> _CC_N(FIELD_EXPONENT_LOW_PART_SIZE), EXPONENT_HIGH_PART) |
                        _CC_ENCODE_FIELD(_CC_N(NULL_EXP) & _CC_N(FIELD_EXPONENT_LOW_PART_MAX_VALUE), EXPONENT_LOW_PART),
    // We mask on store/load so this invisibly keeps null 0 whatever we choose
    // it to be.
    _CC_N(NULL_XOR_MASK) = _CC_N(NULL_PESBT),
};
#pragma GCC diagnostic pop

#define _cc_length_t _cc_N(length_t)
#define _cc_offset_t _cc_N(offset_t)
#define _cc_addr_t _cc_N(addr_t)

#define _CC_MANTISSA_WIDTH _CC_N(MANTISSA_WIDTH)
#define _CC_MAX_EXPONENT _CC_N(MAX_EXPONENT)
#define _CC_BOT_INTERNAL_EXP_WIDTH _CC_N(FIELD_EXP_NONZERO_BOTTOM_SIZE)
#define _CC_EXP_LOW_WIDTH _CC_N(FIELD_EXPONENT_LOW_PART_SIZE)
#define _CC_EXP_HIGH_WIDTH _CC_N(FIELD_EXPONENT_HIGH_PART_SIZE)
#define _CC_ADDR_WIDTH _CC_N(ADDR_WIDTH)
#define _CC_LEN_WIDTH _CC_N(LEN_WIDTH)
#define _CC_MAX_ADDR _CC_N(MAX_ADDR)
#define _CC_MAX_TOP _CC_N(MAX_TOP)
#define _CC_CURSOR_MASK _CC_N(CURSOR_MASK)
// Check that the sizes of the individual fields match up
_CC_STATIC_ASSERT_SAME(_CC_N(FIELD_EBT_SIZE) + _CC_N(FIELD_OTYPE_SIZE) + _CC_N(FIELD_FLAGS_SIZE) +
                           _CC_N(FIELD_RESERVED_SIZE) + _CC_N(FIELD_HWPERMS_SIZE) + _CC_N(FIELD_UPERMS_SIZE),
                       _CC_ADDR_WIDTH);
_CC_STATIC_ASSERT_SAME(_CC_N(FIELD_INTERNAL_EXPONENT_SIZE) + _CC_N(FIELD_EXP_ZERO_TOP_SIZE) +
                           _CC_N(FIELD_EXP_ZERO_BOTTOM_SIZE),
                       _CC_N(FIELD_EBT_SIZE));
_CC_STATIC_ASSERT_SAME(_CC_N(FIELD_INTERNAL_EXPONENT_SIZE) + _CC_N(FIELD_TOP_ENCODED_SIZE) +
                           _CC_N(FIELD_BOTTOM_ENCODED_SIZE),
                       _CC_N(FIELD_EBT_SIZE));
_CC_STATIC_ASSERT_SAME(_CC_N(FIELD_INTERNAL_EXPONENT_SIZE) + _CC_N(FIELD_EXP_NONZERO_TOP_SIZE) +
                           _CC_N(FIELD_EXP_NONZERO_BOTTOM_SIZE) + _CC_N(FIELD_EXPONENT_HIGH_PART_SIZE) +
                           _CC_N(FIELD_EXPONENT_LOW_PART_SIZE),
                       _CC_N(FIELD_EBT_SIZE));

// Sanity-check the min/max otype macros:
_CC_STATIC_ASSERT(_CC_N(MIN_RESERVED_OTYPE) >= 0, "MIN_RESERVED_OTYPE is signed?");
_CC_STATIC_ASSERT(_CC_N(MIN_RESERVED_OTYPE) < _CC_N(MAX_RESERVED_OTYPE),
                  "MIN_RESERVED_OTYPE greater than MAX_RESERVED_OTYPE?");
_CC_STATIC_ASSERT(_CC_N(MIN_RESERVED_OTYPE) <= _CC_N(MAX_REPRESENTABLE_OTYPE), "MIN_RESERVED_OTYPE out of range?");
_CC_STATIC_ASSERT(_CC_N(MAX_RESERVED_OTYPE) <= _CC_N(MAX_REPRESENTABLE_OTYPE), "MAX_RESERVED_OTYPE out of range?");

// Forward-declare the accessors since we use them inside the struct body:
struct _cc_N(cap);
static inline uint8_t _cc_N(get_flags)(const struct _cc_N(cap)* cap);
static inline uint32_t _cc_N(get_otype)(const struct _cc_N(cap)* cap);
static inline uint32_t _cc_N(get_perms)(const struct _cc_N(cap)* cap);
static inline uint8_t _cc_N(get_reserved)(const struct _cc_N(cap)* cap);
static inline uint32_t _cc_N(get_uperms)(const struct _cc_N(cap)* cap);

// In order to allow vector loads and store from memory we can optionally reverse the first two fields.
struct _cc_N(cap) {
    /* offset = cursor - base */
#ifdef _CC_REVERSE_PESBT_CURSOR_ORDER
    /* Original PESBT from the decompressed capability. If you modify
     * other fields, you must be sure to either recalculate this field to match */
    _cc_addr_t cr_pesbt;
    _cc_addr_t _cr_cursor; /* Capability cursor */
#else
    _cc_addr_t _cr_cursor;
    _cc_addr_t cr_pesbt;
#endif
    _cc_length_t _cr_top;    /* Capability top */
    _cc_addr_t cr_base;      /* Capability base addr */
    uint8_t cr_tag;          /* Tag */
    uint8_t cr_bounds_valid; /* Set if bounds decode was given an invalid cap */
    uint8_t cr_exp;          /* Exponent */
    uint8_t cr_extra;        /* Additional data stored by the caller */
#ifdef __cplusplus
    inline _cc_addr_t base() const { return cr_base; }
    inline _cc_addr_t address() const { return _cr_cursor; }
    inline _cc_offset_t offset() const { return (_cc_offset_t)_cr_cursor - (_cc_offset_t)cr_base; }
    inline _cc_length_t top() const { return _cr_top; }
    inline _cc_addr_t top64() const {
        const _cc_length_t t = top();
        return t > _CC_MAX_ADDR ? _CC_MAX_ADDR : (_cc_addr_t)t;
    }
    inline _cc_length_t length() const { return _cr_top - cr_base; }
    inline _cc_addr_t length64() const {
        const _cc_length_t l = length();
        return l > _CC_MAX_ADDR ? _CC_MAX_ADDR : (_cc_addr_t)l;
    }
    inline uint32_t software_permissions() const { return _cc_N(get_uperms)(this); }
    inline uint32_t permissions() const { return _cc_N(get_perms)(this); }
    inline uint32_t type() const { return _cc_N(get_otype)(this); }
    inline bool is_sealed() const { return type() != _CC_N(OTYPE_UNSEALED); }
    inline uint8_t reserved_bits() const { return _cc_N(get_reserved)(this); }
    inline uint8_t flags() const { return _cc_N(get_flags)(this); }
    inline bool operator==(const _cc_N(cap) & other) const;
#endif
};
typedef struct _cc_N(cap) _cc_N(cap_t);
#define _cc_cap_t _cc_N(cap_t)

static inline bool _cc_N(exactly_equal)(const _cc_cap_t* a, const _cc_cap_t* b) {
    return a->cr_tag == b->cr_tag && a->_cr_cursor == b->_cr_cursor && a->cr_pesbt == b->cr_pesbt;
}

static inline bool _cc_N(raw_equal)(const _cc_cap_t* a, const _cc_cap_t* b) {
    return a->_cr_cursor == b->_cr_cursor &&
           a->cr_pesbt == b->cr_pesbt &&
           a->_cr_top == b->_cr_top &&
           a->cr_base == b->cr_base &&
           a->cr_tag == b->cr_tag &&
           a->cr_bounds_valid == b->cr_bounds_valid &&
           a->cr_exp == b->cr_exp &&
           a->cr_extra == b->cr_extra;
}

/* Returns the index of the most significant bit set in x */
static inline uint32_t _cc_N(idx_MSNZ)(uint64_t x) {
#if defined(__GNUC__) && !defined(__clang__)
#define CAP_HAVE_BUILTIN_CLZ
#elif defined(__has_builtin)
#if __has_builtin(__builtin_clzll)
#define CAP_HAVE_BUILTIN_CLZ
#endif
#endif

#ifndef CAP_HAVE_BUILTIN_CLZ
/* floor(log2(x)) != floor(log2(y)) */
#warning "__builtin_clzll not supported, using slower path"
#define ld_neq(x, y) (((x) ^ (y)) > ((x) & (y)))
    uint32_t r = ld_neq(x, x & 0x5555555555555555ull) + (ld_neq(x, x & 0x3333333333333333ull) << 1) +
                 (ld_neq(x, x & 0x0f0f0f0f0f0f0f0full) << 2) + (ld_neq(x, x & 0x00ff00ff00ff00ffull) << 3) +
                 (ld_neq(x, x & 0x0000ffff0000ffffull) << 4) + (ld_neq(x, x & 0x00000000ffffffffull) << 5);
#undef ld_neq
#else
    _cc_debug_assert(x != 0);
    uint32_t r = 63u - (uint32_t)__builtin_clzll(x);
#endif
    return r;
}

/*
 * e = idxMSNZ( (rlength + (rlength >> 6)) >> 19 )
 * where (rlength + (rlength >> 6)) needs to be a 65 bit integer
 */
static inline uint32_t _cc_N(compute_e)(_cc_addr_t rlength, uint32_t bwidth) {
    if (rlength < (1u << (bwidth - 1)))
        return 0;

    return (_cc_N(idx_MSNZ)(rlength) - (bwidth - 2));
}

static inline uint32_t _cc_N(get_exponent)(_cc_length_t length) {
    const uint32_t bwidth = _CC_MANTISSA_WIDTH;
    if (length > _CC_MAX_ADDR) {
        return _CC_LEN_WIDTH - (bwidth - 1);
    } else {
        return _cc_N(compute_e)((_cc_addr_t)length, bwidth);
    }
}

static inline uint64_t _cc_N(getbits)(uint64_t src, uint32_t start, uint32_t size) {
    return ((src >> start) & ((UINT64_C(1) << size) - UINT64_C(1)));
}

// truncates `value`, keeping only the _least_ significant `n` bits.
static inline uint64_t _cc_N(truncate64)(uint64_t value, size_t n) { return value & ((UINT64_C(1) << n) - 1); }

// truncates `value`, keeping only the _most_ significant `n` bits.
#define TRUNCATE_LSB_FUNC(type_width)                                                                                  \
    static inline uint64_t _CC_CONCAT(_cc_N(truncateLSB_), type_width)(uint64_t value, size_t n) {                     \
        _CC_STATIC_ASSERT(type_width <= 64, "");                                                                       \
        return value >> (type_width - n);                                                                              \
    }
TRUNCATE_LSB_FUNC(_CC_MANTISSA_WIDTH)
TRUNCATE_LSB_FUNC(32)
TRUNCATE_LSB_FUNC(64)

#define _cc_truncateLSB_generic(type_width) _CC_CONCAT(_cc_N(truncateLSB_), _CC_EXPAND(type_width))
#define _cc_truncateLSB(type_width) _cc_N(_CC_CONCAT(truncateLSB_, type_width))

struct _cc_N(bounds_bits) {
    uint16_t B; // bottom bits (currently 14 bits)
    uint16_t T; // top bits (12 bits plus two implied bits)
    uint8_t E;  // exponent
    bool IE;    // internal exponent flag
};
#define _cc_bounds_bits struct _cc_N(bounds_bits)

#define ALL_WRAPPERS(X, FN, type)                                                                                      \
    static inline _cc_addr_t _cc_N(cap_pesbt_extract_##FN)(_cc_addr_t pesbt) { return _CC_EXTRACT_FIELD(pesbt, X); }   \
    static inline _cc_addr_t _cc_N(cap_pesbt_encode_##FN)(type value) { return _CC_ENCODE_FIELD(value, X); }           \
    static inline _cc_addr_t _cc_N(cap_pesbt_deposit_##FN)(_cc_addr_t pesbt, type value) {                             \
        _cc_debug_assert(value <= _CC_N(FIELD_##X##_MAX_VALUE));                                                       \
        return (pesbt & ~_CC_N(FIELD_##X##_MASK64)) | _CC_ENCODE_FIELD(value, X);                                      \
    }                                                                                                                  \
    static inline type _cc_N(get_##FN)(const _cc_cap_t* cap) { return _cc_N(cap_pesbt_extract_##FN)(cap->cr_pesbt); }  \
    static inline void _cc_N(update_##FN)(_cc_cap_t * cap, _cc_addr_t value) {                                         \
        cap->cr_pesbt = _cc_N(cap_pesbt_deposit_##FN)(cap->cr_pesbt, value);                                           \
    }
ALL_WRAPPERS(HWPERMS, perms, uint32_t)
ALL_WRAPPERS(UPERMS, uperms, uint32_t)
ALL_WRAPPERS(OTYPE, otype, uint32_t)
ALL_WRAPPERS(FLAGS, flags, uint8_t)
ALL_WRAPPERS(RESERVED, reserved, uint8_t)
#undef ALL_WRAPPERS

/// Extract the bits used for bounds and infer the top two bits of T
static inline _cc_bounds_bits _cc_N(extract_bounds_bits)(_cc_addr_t pesbt) {
    _CC_STATIC_ASSERT(_CC_MANTISSA_WIDTH == _CC_N(BOT_WIDTH), "Wrong bot width?");
    uint32_t BWidth = _CC_MANTISSA_WIDTH;
    uint32_t BMask = (1u << BWidth) - 1;
    uint32_t TMask = BMask >> 2;
    _cc_bounds_bits result;
    _CC_STATIC_ASSERT(sizeof(result.B) * __CHAR_BIT__ >= _CC_MANTISSA_WIDTH, "B field too small");
    _CC_STATIC_ASSERT(sizeof(result.T) * __CHAR_BIT__ >= _CC_MANTISSA_WIDTH, "T field too small");
    _CC_STATIC_ASSERT(sizeof(result.E) * __CHAR_BIT__ >=
                          _CC_N(FIELD_EXPONENT_LOW_PART_SIZE) + _CC_N(FIELD_EXPONENT_HIGH_PART_SIZE),
                      "E field too small");
    result.IE = (bool)(uint32_t)_CC_EXTRACT_FIELD(pesbt, INTERNAL_EXPONENT);
    uint8_t L_msb;
    if (result.IE) {
        result.E = (uint8_t)(_CC_EXTRACT_FIELD(pesbt, EXPONENT_LOW_PART) |
                             (_CC_EXTRACT_FIELD(pesbt, EXPONENT_HIGH_PART) << _CC_N(FIELD_EXPONENT_LOW_PART_SIZE)));
        // Do not offset by 1! We also need to encode E=0 even with IE
        // Also allow nonsense values over 64 - BWidth + 2: this is expected by sail-generated tests
        // E = MIN(64 - BWidth + 2, E);
#ifdef CC_IS_MORELLO
        if (result.E == _CC_N(MAX_ENCODABLE_EXPONENT)) {
            result.B = 0;
            // This isn't top, its T. We just special case again when top is calculated.
            result.T = 0;
            return result;
        }
#endif
        result.B = (uint16_t)_CC_EXTRACT_FIELD(pesbt, EXP_NONZERO_BOTTOM) << _CC_N(FIELD_EXPONENT_LOW_PART_SIZE);
        result.T = (uint16_t)_CC_EXTRACT_FIELD(pesbt, EXP_NONZERO_TOP) << _CC_N(FIELD_EXPONENT_HIGH_PART_SIZE);
        L_msb = 1;
    } else {
        // So, I cheated by inverting E on memory load (to match the rest of CHERI), which Morello does not do.
        // This means parts of B and T are incorrectly inverted. So invert back again.
#ifdef CC_IS_MORELLO
        pesbt ^= _CC_N(NULL_XOR_MASK);
#endif
        result.E = 0;
        L_msb = 0;
        result.B = (uint16_t)_CC_EXTRACT_FIELD(pesbt, EXP_ZERO_BOTTOM);
        result.T = (uint16_t)_CC_EXTRACT_FIELD(pesbt, EXP_ZERO_TOP);
    }
    /*
        Reconstruct top two bits of T given T = B + len and:
        1) the top two bits of B
        2) most significant two bits of length derived from format above
        3) carry out of B[20..0] + len[20..0] that is implied if T[20..0] < B[20..0]
    */
    uint8_t L_carry = result.T < (result.B & TMask) ? 1 : 0;
    uint64_t BTop2 = _cc_N(getbits)(result.B, _CC_MANTISSA_WIDTH - 2, 2);
    uint8_t T_infer = (BTop2 + L_carry + L_msb) & 0x3;
    result.T |= ((uint16_t)T_infer) << (BWidth - 2);
    return result;
}

// Certain bit patterns can result in invalid bounds bits. These values must never be tagged!
static inline bool _cc_N(bounds_bits_valid)(_cc_bounds_bits bounds) {
    // https://github.com/CTSRD-CHERI/sail-cheri-riscv/blob/7a308ef3661e43461c8431c391aaece7fba6e992/src/cheri_properties.sail#L104
    _cc_addr_t Bmsb = _cc_N(getbits)(bounds.B, _CC_MANTISSA_WIDTH - 1, 1);
    _cc_addr_t Bmsb2 = _cc_N(getbits)(bounds.B, _CC_MANTISSA_WIDTH - 2, 2);
    _cc_addr_t Tmsb = _cc_N(getbits)(bounds.T, _CC_MANTISSA_WIDTH - 1, 1);
    if (bounds.E >= _CC_MAX_EXPONENT) {
        return Tmsb == 0 && Bmsb2 == 0;
    } else if (bounds.E == _CC_MAX_EXPONENT - 1) {
        return Bmsb == 0;
    } else {
        return true;
    }
}

static inline bool _cc_N(compute_base_top)(_cc_bounds_bits bounds, _cc_addr_t cursor, _cc_addr_t* base_out,
                                           _cc_length_t* top_out) {
#ifdef CC_IS_MORELLO
    if (bounds.E > _CC_MAX_EXPONENT) {
        bool valid = bounds.E == _CC_N(MAX_ENCODABLE_EXPONENT);
        *base_out = 0;
        *top_out = _CC_N(MAX_TOP);
        return valid;
    }

    // Remove flags bits
    cursor = cursor & _CC_CURSOR_MASK;
    // Sign extend
    if (cursor & ((_CC_CURSOR_MASK >> 1) + 1))
        cursor |= ~_CC_CURSOR_MASK;
#endif

    // For the remaining computations we have to clamp E to max_E
    //  let E = min(maxE, unsigned(c.E)) in
    uint8_t E = _CC_MIN(_CC_MAX_EXPONENT, bounds.E);
    /* Extract bits we need to make the top correction and calculate representable limit */
    // let a3 = truncate(a >> (E + mantissa_width - 3), 3) in
    // let B3 = truncateLSB(c.B, 3) in
    // let T3 = truncateLSB(c.T, 3) in
    unsigned a3 = (unsigned)_cc_N(truncate64)(cursor >> (E + _CC_MANTISSA_WIDTH - 3), 3);
    unsigned B3 = (unsigned)_cc_truncateLSB(_CC_MANTISSA_WIDTH)(bounds.B, 3);
    unsigned T3 = (unsigned)_cc_truncateLSB(_CC_MANTISSA_WIDTH)(bounds.T, 3);
    // let R3 = B3 - 0b001 in /* wraps */
    unsigned R3 = (unsigned)_cc_N(truncate64)(B3 - 1, 3); // B3 == 0 ? 7 : B3 - 1;
    /* Do address, base and top lie in the R aligned region above the one containing R? */
    // let aHi = if a3 <_u R3 then 1 else 0 in
    // let bHi = if B3 <_u R3 then 1 else 0 in
    // let tHi = if T3 <_u R3 then 1 else 0 in
    int aHi = a3 < R3 ? 1 : 0;
    int bHi = B3 < R3 ? 1 : 0;
    int tHi = T3 < R3 ? 1 : 0;

    /* Compute region corrections for top and base relative to a */
    // let correction_base = bHi - aHi in
    // let correction_top  = tHi - aHi in
    int correction_base = bHi - aHi;
    int correction_top = tHi - aHi;
    // Note: shifting by 64 is UB and causes wrong results -> clamp the shift value!
    const unsigned a_top_shift = E + _CC_MANTISSA_WIDTH;
    // let a_top = (a >> (E + mantissa_width)) in
    _cc_addr_t a_top = a_top_shift >= _CC_ADDR_WIDTH ? 0 : cursor >> a_top_shift;

    // base : CapLenBits = truncate((a_top + correction_base) @ c.B @ zeros(E), cap_len_width);
    _cc_length_t base = (_cc_addr_t)((int64_t)a_top + correction_base);
    base <<= _CC_MANTISSA_WIDTH;
    base |= bounds.B;
    base <<= E;
    base &= ((_cc_length_t)1 << _CC_LEN_WIDTH) - 1;
    _cc_debug_assert((_cc_addr_t)(base >> _CC_ADDR_WIDTH) <= 1); // max 65/33 bits
    // top  : truncate((a_top + correction_top)  @ c.T @ zeros(E), cap_len_width);
    _cc_length_t top = (_cc_addr_t)((int64_t)a_top + correction_top);
    top <<= _CC_MANTISSA_WIDTH;
    top |= bounds.T;
    top <<= E;
    top &= ((_cc_length_t)1 << _CC_LEN_WIDTH) - 1;
    _cc_debug_assert((_cc_addr_t)(top >> _CC_ADDR_WIDTH) <= 1); // max 65 bits

    /* If the base and top are more than an address space away from each other,
       invert the MSB of top.  This corrects for errors that happen when the
       representable space wraps the address space. */
    //  let base2 : bits(2) = 0b0 @ [base[cap_addr_width - 1]];
    // Note: we ignore the top bit of base here. If we don't we can get cases
    // where setbounds/incoffset/etc. break monotonicity.
    unsigned base2 = _cc_N(truncate64)(base >> (_CC_ADDR_WIDTH - 1), 1);
    //  let top2  : bits(2) = top[cap_addr_width .. cap_addr_width - 1];
    unsigned top2 = _cc_N(truncate64)(top >> (_CC_ADDR_WIDTH - 1), 2);
    //  if (E < (maxE - 1)) & (unsigned(top2 - base2) > 1) then {
    //      top[cap_addr_width] = ~(top[cap_addr_width]);
    //  };
    if (E < (_CC_MAX_EXPONENT - 1) && (top2 - base2) > 1) {
        top = top ^ ((_cc_length_t)1 << _CC_ADDR_WIDTH);
    }

    _cc_debug_assert((_cc_addr_t)(top >> _CC_ADDR_WIDTH) <= 1); // should be at most 1 bit over
    // Check that base <= top for valid inputs
    if (_cc_N(bounds_bits_valid)(bounds)) {
        // Note: base can be > 2^64 for some (untagged) inputs with E near maxE
        // It can also be > top for some (untagged) inputs.
        _cc_debug_assert((_cc_addr_t)base <= top);
    } else {
        // _cc_debug_assert(!tagged && "Should not create invalid tagged capabilities");
    }
    *base_out = (_cc_addr_t)base; // strip the (invalid) top bit
    *top_out = top;

    return true;
}

static inline void _cc_N(decompress_raw)(_cc_addr_t pesbt, _cc_addr_t cursor, bool tag, _cc_cap_t* cdp) {
    cdp->cr_tag = tag;
    cdp->_cr_cursor = cursor;
    cdp->cr_pesbt = pesbt;

    _cc_bounds_bits bounds = _cc_N(extract_bounds_bits)(pesbt);
    bool valid = _cc_N(compute_base_top)(bounds, cursor, &cdp->cr_base, &cdp->_cr_top);
    cdp->cr_bounds_valid = valid;
    cdp->cr_exp = bounds.E;
    if (tag) {
        _cc_debug_assert(cdp->cr_base <= _CC_N(MAX_ADDR));
#ifndef CC_IS_MORELLO
        // Morello is perfectly happy using settag to create capabilities with length greater than 2^64.
        _cc_debug_assert(cdp->_cr_top <= _CC_N(MAX_TOP));
        _cc_debug_assert(cdp->cr_base <= cdp->_cr_top);
#endif
        _cc_debug_assert(_CC_EXTRACT_FIELD(pesbt, RESERVED) == 0);
    }
}

/*
 * Decompress a 128-bit capability.
 */
static inline void _cc_N(decompress_mem)(uint64_t pesbt, uint64_t cursor, bool tag, _cc_cap_t* cdp) {
    _cc_N(decompress_raw)(pesbt ^ _CC_N(NULL_XOR_MASK), cursor, tag, cdp);
}

static inline bool _cc_N(is_cap_sealed)(const _cc_cap_t* cp) { return _cc_N(get_otype)(cp) != _CC_N(OTYPE_UNSEALED); }

// Update ebt bits in pesbt
static inline void _cc_N(update_ebt)(_cc_cap_t* csp, _cc_addr_t new_ebt) {
    csp->cr_pesbt = (csp->cr_pesbt & ~_CC_N(FIELD_EBT_MASK64)) | new_ebt;
}

/*
 * Compress a capability to 128 bits.
 * Note: if you have not been manually modifying fields, just access csp->cr_pesbt.
 * cap_set_decompressed_X will set fields and keep pesbt in sync.
 */
static inline _cc_addr_t _cc_N(compress_raw)(const _cc_cap_t* csp) {
    _cc_debug_assert((!csp->cr_tag || _cc_N(get_reserved)(csp) == 0) &&
                     "Unknown reserved bits set it tagged capability");
    return csp->cr_pesbt;
}

static inline _cc_addr_t _cc_N(compress_mem)(const _cc_cap_t* csp) {
    return _cc_N(compress_raw)(csp) ^ _CC_N(NULL_XOR_MASK);
}

/*
 * Define the following to do the is_representable() check by simply
 * compressing and decompressing the capability and checking to
 * see if it is the same.
 */
// #define SIMPLE_REPRESENT_CHECK

#ifndef SIMPLE_REPRESENT_CHECK
static inline bool _cc_N(all_ones)(uint64_t offset, uint32_t e, uint32_t bwidth) {
    uint64_t Itop;
    uint32_t shift = e + bwidth;

    if (shift >= 63)
        return false;
    Itop = offset >> shift;
    return Itop == (0xfffffffffffffffful >> shift);
}

static inline bool _cc_N(all_zeroes)(uint64_t offset, uint32_t e, uint32_t bwidth) {
    uint32_t shift = e + bwidth;
    uint64_t Itop;

    if (shift >= 63)
        Itop = 0ul;
    else
        Itop = offset >> shift;
    return Itop == 0ul;
}
#endif /* ! SIMPLE_REPRESENT_CHECK */

static bool _cc_N(fast_is_representable_new_addr)(bool sealed, _cc_addr_t base, _cc_length_t length, _cc_addr_t cursor,
                                                  _cc_addr_t new_cursor);

/// Check that a capability is representable by compressing and recompressing
static inline bool _cc_N(is_representable_cap_exact)(const _cc_cap_t* cap) {
    _cc_addr_t pesbt = _cc_N(compress_raw)(cap);
    _cc_cap_t decompressed_cap;
    _cc_N(decompress_raw)(pesbt, cap->_cr_cursor, cap->cr_tag, &decompressed_cap);
    // These fields must not change:
    _cc_debug_assert(decompressed_cap._cr_cursor == cap->_cr_cursor);
    _cc_debug_assert(decompressed_cap.cr_pesbt == cap->cr_pesbt);
    // If any of these fields changed then the capability is not representable:
    if (decompressed_cap.cr_base != cap->cr_base || decompressed_cap._cr_top != cap->_cr_top) {
        return false;
    }
    return true;
}

static inline uint32_t _cc_N(compute_ebt)(_cc_addr_t req_base, _cc_length_t req_top, _cc_addr_t* alignment_mask,
                                          bool* exact) {
#ifdef CC_IS_MORELLO
    if (req_base == 0 && req_top == _CC_N(MAX_TOP)) {
        *exact = true;
        if (alignment_mask)
            *alignment_mask = _CC_MAX_ADDR;
        return _CC_N(RESET_EBT);
    }
#else
    _cc_debug_assert(req_base <= req_top && "Cannot invert base and top");
#endif
    /*
     * With compressed capabilities we may need to increase the range of
     * memory addresses to be wider than requested so it is
     * representable.
     */
    _cc_length_t req_length65 = req_top - req_base;
    // function setCapBounds(cap, base, top) : (Capability, bits(64), bits(65)) -> (bool, Capability) = {
    //  /* {cap with base=base; length=(bits(64)) length; offset=0} */
    //  let base65 = 0b0 @ base;
    //  let length = top - base65;
    //  /* Find an exponent that will put the most significant bit of length
    //     second from the top as assumed during decoding. We ignore the bottom
    //     MW - 1 bits because those are handled by the ie = 0 format. */
    //  let e = 52 - CountLeadingZeros(length[64..13]);
    uint8_t E = (uint8_t)_cc_N(get_exponent)(req_length65);
    const uint64_t req_length64 = (uint64_t)req_length65;
    // Use internal exponent if e is non-zero or if e is zero but
    // but the implied bit of length is not zero (denormal vs. normal case)
    //  let ie = (e != 0) | length[12];
    //
    const bool InternalExponent = E != 0 || _cc_N(getbits)(req_length64, _CC_BOT_INTERNAL_EXP_WIDTH + 1, 1);
    if (!InternalExponent) {
        //  /* The non-ie e == 0 case is easy. It is exact so just extract relevant bits. */
        //  Bbits = truncate(base, 14);
        //  Tbits = truncate(top, 14);
        //  lostSignificantTop  : bool = false;
        //  lostSignificantBase : bool = false;
        //  incE : bool = false;
        uint32_t ebt_bits = _CC_ENCODE_EBT_FIELD(0, INTERNAL_EXPONENT) | _CC_ENCODE_EBT_FIELD(req_top, EXP_ZERO_TOP) |
                            _CC_ENCODE_EBT_FIELD(req_base, EXP_ZERO_BOTTOM);
#ifdef CC_IS_MORELLO
        // Due to morello conditionally inverting bits, we need to invert the bits that would be an internal exponent
        // here
        ebt_bits ^= _CC_ENCODE_EBT_FIELD(~0, EXPONENT_HIGH_PART) | _CC_ENCODE_EBT_FIELD(~0, EXPONENT_LOW_PART);
#endif
        if (alignment_mask)
            *alignment_mask = _CC_MAX_ADDR; // no adjustment to base required
        *exact = true;
        return ebt_bits; /* Exactly representable */
    }
    // Handle IE case:
    //  if ie then {
    //    /* the internal exponent case is trickier */
    //
    //    /* Extract B and T bits (we lose 3 bits of each to store the exponent) */
    //    B_ie = truncate(base >> (e + 3), 11);
    //    T_ie = truncate(top >> (e + 3), 11);
    //
    _cc_addr_t bot_ie = _cc_N(truncate64)(req_base >> (E + _CC_EXP_LOW_WIDTH), _CC_BOT_INTERNAL_EXP_WIDTH);
    if (alignment_mask) {
        *alignment_mask = UINT64_MAX << (E + _CC_EXP_LOW_WIDTH);
    }
    _cc_addr_t top_ie = _cc_N(truncate64)((_cc_addr_t)(req_top >> (E + _CC_EXP_LOW_WIDTH)), _CC_BOT_INTERNAL_EXP_WIDTH);
    //    /* Find out whether we have lost significant bits of base and top using a
    //       mask of bits that we will lose (including 3 extra for exp). */
    //    maskLo : bits(65) = zero_extend(replicate_bits(0b1, e + 3));
    //    z65    : bits(65) = zeros();
    //    lostSignificantBase = (base65 & maskLo) != z65;
    //    lostSignificantTop = (top & maskLo) != z65;
    // TODO: stop using _cc_length_t and just handle bit65 set specially?
    const _cc_length_t maskLo = (((_cc_length_t)1u) << (E + _CC_EXP_LOW_WIDTH)) - 1;
    const _cc_length_t zero65 = 0;
    bool lostSignificantBase = (req_base & maskLo) != zero65;
    bool lostSignificantTop = (req_top & maskLo) != zero65;
    //    if lostSignificantTop then {
    //      /* we must increment T to make sure it is still above top even with lost bits.
    //         It might wrap around but if that makes B<T then decoding will compensate. */
    //      T_ie = T_ie + 1;
    //    };
    if (lostSignificantTop) {
        top_ie = _cc_N(truncate64)(top_ie + 1, _CC_BOT_INTERNAL_EXP_WIDTH);
    }
    //    /* Has the length overflowed? We chose e so that the top two bits of len would be 0b01,
    //       but either because of incrementing T or losing bits of base it might have grown. */
    //    len_ie = T_ie - B_ie;
    //    if len_ie[10] then {
    //      /* length overflow -- increment E by one and then recalculate
    //         T, B etc accordingly */
    //      incE = true;
    //
    //      lostSignificantBase = lostSignificantBase | B_ie[0];
    //      lostSignificantTop  = lostSignificantTop | T_ie[0];
    //
    //      B_ie = truncate(base >> (e + 4), 11);
    //      let incT : range(0,1) = if lostSignificantTop then 1 else 0;
    //      T_ie = truncate(top >> (e + 4), 11) + incT;
    //    };
    const _cc_addr_t len_ie = _cc_N(truncate64)(top_ie - bot_ie, _CC_BOT_INTERNAL_EXP_WIDTH);
    bool incE = false;
    if (_cc_N(getbits)(len_ie, _CC_BOT_INTERNAL_EXP_WIDTH - 1, 1)) {
        incE = true;
        lostSignificantBase = lostSignificantBase || _cc_N(getbits)(bot_ie, 0, 1);
        lostSignificantTop = lostSignificantTop || _cc_N(getbits)(top_ie, 0, 1);
        bot_ie = _cc_N(truncate64)(req_base >> (E + _CC_EXP_LOW_WIDTH + 1), _CC_BOT_INTERNAL_EXP_WIDTH);
        // If we had to adjust bot_ie (shift by one more) also update alignment_mask
        if (alignment_mask) {
            *alignment_mask = UINT64_MAX << (E + _CC_EXP_LOW_WIDTH + 1);
        }
        const bool incT = lostSignificantTop;
        top_ie = _cc_N(truncate64)((_cc_addr_t)(req_top >> (E + _CC_EXP_LOW_WIDTH + 1)), _CC_BOT_INTERNAL_EXP_WIDTH);
        if (incT) {
            top_ie = _cc_N(truncate64)(top_ie + 1, _CC_BOT_INTERNAL_EXP_WIDTH);
        }
    }
    //
    //    Bbits = B_ie @ 0b000;
    //    Tbits = T_ie @ 0b000;
    const _cc_addr_t Bbits = bot_ie << _CC_N(FIELD_EXPONENT_LOW_PART_SIZE);
    const _cc_addr_t Tbits = top_ie << _CC_N(FIELD_EXPONENT_LOW_PART_SIZE);
    const uint8_t newE = E + (incE ? 1 : 0);

    //  };
    //  let exact = not(lostSignificantBase | lostSignificantTop);
    *exact = !lostSignificantBase && !lostSignificantTop;
    // Split E between T and B
    const _cc_addr_t expHighBits =
        _cc_N(getbits)(newE >> _CC_N(FIELD_EXPONENT_LOW_PART_SIZE), 0, _CC_N(FIELD_EXPONENT_HIGH_PART_SIZE));
    const _cc_addr_t expLowBits = _cc_N(getbits)(newE, 0, _CC_N(FIELD_EXPONENT_LOW_PART_SIZE));
    const _cc_addr_t Te = Tbits | expHighBits;
    const _cc_addr_t Be = Bbits | expLowBits;
    return _CC_ENCODE_EBT_FIELD(1, INTERNAL_EXPONENT) | _CC_ENCODE_EBT_FIELD(Te, TOP_ENCODED) |
           _CC_ENCODE_EBT_FIELD(Be, BOTTOM_ENCODED);
}

/*
 * Check to see if a memory region is representable by a compressed
 * capability. It is representable if:
 *
 *   representable = (inRange && inLimits) || (E >= 44)
 *
 * where:
 *
 *   E = compression exponent (see _cc_N(compute_e)() above)
 *
 *   inRange = -s < i < s  where i is the increment (or offset)
 *   (In other words, all the bits of i<63, E+20> are the same.)
 *
 *   inLimits = (i < 0) ? (Imid >= (R - Amid)) && (R != Amid) : (R - Amid - 1)
 *   where Imid = i<E+19, E>, Amid = a<E+19, E>, R = B - 2^12 and a =
 *   base + offset.
 */
static inline bool _cc_N(is_representable_new_addr)(bool sealed, _cc_addr_t base, _cc_length_t length,
                                                    _cc_addr_t cursor, _cc_addr_t new_cursor) {
    _cc_length_t top = (_cc_length_t)base + length;

    cursor &= _CC_CURSOR_MASK;
    new_cursor &= _CC_CURSOR_MASK;

    // in-bounds capabilities are always representable
    if (__builtin_expect(new_cursor >= base && new_cursor < top, true)) {
        return true;
    }

#if defined(_CC_USE_FAST_REP_CHECK)
    const bool slow_representable_check = false;
#else
    const bool slow_representable_check = true;
#endif

    if (slow_representable_check) {
        /* Simply compress and uncompress to check. */
        _cc_cap_t c;
        memset(&c, 0, sizeof(c));
        c.cr_base = base;
        c._cr_top = top;
        c._cr_cursor = cursor;
        // important to set as compress assumes this is in bounds
        c.cr_pesbt = _CC_ENCODE_FIELD(_CC_N(UPERMS_ALL), UPERMS) | _CC_ENCODE_FIELD(_CC_N(PERMS_ALL), HWPERMS) |
                     _CC_ENCODE_FIELD(sealed ? 42 : _CC_N(OTYPE_UNSEALED), OTYPE);
        /* Get an EBT */
        bool exact_input = false;
        _cc_N(update_ebt)(&c, _cc_N(compute_ebt)(base, top, NULL, &exact_input));
        // Looks like this assert gets hit by negative length capabilities. Probably the "exact input" return is wrong.
        if (top > (_cc_length_t)base)
            _cc_debug_assert(exact_input &&
                             "Input capability bounds not representable? They should have been rounded before!");
        /* Check with new cursor */
        c._cr_cursor = new_cursor;
        return _cc_N(is_representable_cap_exact)(&c);
    } else {
        return _cc_N(fast_is_representable_new_addr)(sealed, base, length, cursor, new_cursor);
    }
}

static inline _cc_addr_t _cc_N(cap_bounds_address)(const _cc_cap_t* cap) {
    // Remove flags bits
    _cc_addr_t cursor = cap->_cr_cursor & _CC_CURSOR_MASK;
    // Sign extend
    if (cursor & ((_CC_CURSOR_MASK >> 1) + 1))
        cursor |= ~_CC_CURSOR_MASK;
    return cursor;
}

// This should only be used on decompressed caps, as it relies on the exp field
static inline bool _cc_N(cap_bounds_uses_value)(const _cc_cap_t* cap) {
    return cap->cr_exp < (sizeof(_cc_addr_t) * 8) - _CC_N(FIELD_BOTTOM_ENCODED_SIZE);
}

static inline bool _cc_N(cap_sign_change)(_cc_addr_t addr1, _cc_addr_t addr2) {
#ifdef CC_IS_MORELLO
    return ((addr1 ^ addr2) & (1ULL << (63 - MORELLO_FLAG_BITS)));
#else
    (void)addr1;
    (void)addr2;
    return false;
#endif
}

static inline bool _cc_N(cap_sign_change_causes_unrepresentability)(const _cc_cap_t* cap, _cc_addr_t addr1,
                                                                    _cc_addr_t addr2) {
    return _cc_N(cap_sign_change)(addr1, addr2) && _cc_N(cap_bounds_uses_value)(cap);
}

static inline bool _cc_N(is_representable_with_addr)(const _cc_cap_t* cap, _cc_addr_t new_addr) {
    new_addr &= _CC_CURSOR_MASK;
#ifdef CC_IS_MORELLO
    // If the top bit is changed on morello this can change bounds
    if (_cc_N(cap_sign_change_causes_unrepresentability)(cap, new_addr, cap->_cr_cursor)) {
        return false;
    }
    if (!cap->cr_bounds_valid)
        return false;
#endif
    // in-bounds capabilities are otherwise always representable
    if (__builtin_expect(new_addr >= cap->cr_base && new_addr < cap->_cr_top, true)) {
        return true;
    }
    const _cc_length_t length = cap->_cr_top - cap->cr_base;
    return _cc_N(is_representable_new_addr)(_cc_N(is_cap_sealed)(cap), cap->cr_base, length, cap->_cr_cursor, new_addr);
}

static bool _cc_N(fast_is_representable_new_addr)(bool sealed, _cc_addr_t base, _cc_length_t length, _cc_addr_t cursor,
                                                  _cc_addr_t new_cursor) {
    (void)sealed;
    uint32_t bwidth = _CC_MANTISSA_WIDTH;

    cursor &= _CC_CURSOR_MASK;
    new_cursor &= _CC_CURSOR_MASK;

    uint32_t highest_exp = (_CC_ADDR_WIDTH - bwidth + 2);

    _cc_length_t top = base + length;

    if (top == _CC_MAX_TOP && base == 0) {
        return true; // 1 << 65 is always representable
    }
    if (length == 0) {
        return true; // length 0 is always representable
    }

    uint32_t e = _cc_N(get_exponent)(length);

    int64_t b, r, Imid, Amid;
    bool inRange, inLimits;
    int64_t inc = (int64_t)(new_cursor - cursor);

#define MOD_MASK ((UINT64_C(1) << bwidth) - UINT64_C(1))

    /* Check for the boundary cases. */

    b = (int64_t)((base >> e) & MOD_MASK);
    Imid = (int64_t)((uint64_t)(inc >> e) & MOD_MASK);
    Amid = (int64_t)(((cursor) >> e) & MOD_MASK);

    r = (int64_t)(((uint64_t)((b >> (bwidth - 3)) - 1) << (bwidth - 3)) & MOD_MASK);

    /* inRange, test if bits are all the same */
    inRange = _cc_N(all_ones)((uint64_t)inc, e, bwidth) || _cc_N(all_zeroes)((uint64_t)inc, e, bwidth);

    /* inLimits */
    if (inc >= 0) {
        inLimits = ((uint64_t)Imid < (((uint64_t)(r - Amid - 1l)) & MOD_MASK));
    } else {
        inLimits = ((uint64_t)Imid >= (((uint64_t)(r - Amid)) & MOD_MASK)) && (r != Amid);
    }
#undef MOD_MASK

    return ((inRange && inLimits) || (e >= highest_exp - 2));
}

/* @return whether the operation was able to set precise bounds precise or not */
static inline bool _cc_N(setbounds_impl)(_cc_cap_t* cap, _cc_addr_t req_base, _cc_length_t req_top,
                                         _cc_addr_t* alignment_mask) {
#ifdef CC_IS_MORELLO
    if (!cap->cr_bounds_valid) {
        cap->cr_tag = 0;
    }
    bool from_large = !_cc_N(cap_bounds_uses_value)(cap);
#else
    // Morello allows setbounds to do weird things and will just result in untagged results
    _cc_debug_assert((cap->cr_tag) && "Cannot be used on untagged capabilities");
    _cc_debug_assert((!_cc_N(is_cap_sealed)(cap)) && "Cannot be used on sealed capabilities");
#endif
    _cc_debug_assert(req_base <= req_top && "Cannot invert base and top");
    /*
     * With compressed capabilities we may need to increase the range of
     * memory addresses to be wider than requested so it is
     * representable.
     */
    const _cc_addr_t cursor = cap->_cr_cursor;
#ifndef CC_IS_MORELLO
    _cc_debug_assert(((cap->_cr_top - cap->cr_base) >> _CC_ADDR_WIDTH) <= 1 && "Length must be smaller than 1 << 65");
    _cc_debug_assert((req_top >> _CC_ADDR_WIDTH) <= 1 && "New top must be smaller than 1 << 65");
    _cc_debug_assert(req_base >= cap->cr_base && "Cannot decrease base");
    _cc_debug_assert(req_top <= cap->_cr_top && "Cannot increase top");
    assert((cursor < cap->_cr_top || (cursor == cap->_cr_top && req_base == cap->_cr_top && req_base == req_top)) &&
           "Must be used on inbounds caps or request zero-length cap at top");
    assert((cursor >= cap->cr_base) && "Must be used on inbounds caps");
#endif
    _CC_STATIC_ASSERT(_CC_EXP_LOW_WIDTH == 3, "expected 3 bits to be used by");  // expected 3 bits to
    _CC_STATIC_ASSERT(_CC_EXP_HIGH_WIDTH == 3, "expected 3 bits to be used by"); // expected 3 bits to
    bool exact = false;
    uint32_t new_ebt = _cc_N(compute_ebt)(req_base, req_top, alignment_mask, &exact);

    // TODO: find a faster way to compute top and bot:
    const _cc_addr_t pesbt = _CC_ENCODE_FIELD(0, UPERMS) | _CC_ENCODE_FIELD(0, HWPERMS) |
                             _CC_ENCODE_FIELD(_CC_N(OTYPE_UNSEALED), OTYPE) | _CC_ENCODE_FIELD(new_ebt, EBT);
    _cc_cap_t new_cap;
    _cc_N(decompress_raw)(pesbt, cursor, cap->cr_tag, &new_cap);
    _cc_addr_t new_base = new_cap.cr_base;
    _cc_length_t new_top = new_cap._cr_top;

    if (exact) {
#ifndef CC_IS_MORELLO
        // Morello considers a setbounds that takes a capability from "large" (non-sign extended bounds) to "small"
        // to still be exact, even if this results in a change in requested bounds. The exact assert would be tedious
        // to express so I have turned it off for morello.
        _cc_debug_assert(new_base == req_base && "Should be exact");
        _cc_debug_assert(new_top == req_top && "Should be exact");
#endif
    } else {
        _cc_debug_assert((new_base != req_base || new_top != req_top) &&
                         "Was inexact, but neither base nor top different?");
    }

    _cc_debug_assert(new_top >= new_base);
    _cc_debug_assert((!cap->cr_tag || _cc_N(get_reserved)(cap) == 0) &&
                     "Unknown reserved bits set in tagged capability");
    cap->cr_base = new_base;
    cap->_cr_top = new_top;
    cap->cr_exp = new_cap.cr_exp;
    _cc_N(update_ebt)(cap, new_ebt);
#ifdef CC_IS_MORELLO
    cap->cr_bounds_valid = new_cap.cr_bounds_valid;
    bool to_small = _cc_N(cap_bounds_uses_value)(cap);
    // On morello we may end up with a length that could have been exact, but has changed the flag bits.
    if ((from_large && to_small) && ((new_base ^ req_base) >> (64 - MORELLO_FLAG_BITS))) {
        cap->cr_tag = 0;
    }
#endif

    //  let newCap = {cap with address=base, E=to_bits(6, if incE then e + 1 else e), B=Bbits, T=Tbits, internal_e=ie};
    //  (exact, newCap)
    return exact;
}

/* @return whether the operation was able to set precise bounds precise or not */
static inline bool _cc_N(setbounds)(_cc_cap_t* cap, _cc_addr_t req_base, _cc_length_t req_top) {
    return _cc_N(setbounds_impl)(cap, req_base, req_top, NULL);
}

/* @return the mask that needs to be applied to base in order to get a precisely representable capability */
static inline _cc_addr_t _cc_N(get_alignment_mask)(_cc_addr_t req_length) {
    if (req_length == 0) {
        // With a lenght of zero we know it is precise so we can just return an
        // all ones mask.
        // This avoids undefined behaviour when counting most significant bit later.
        return _CC_MAX_ADDR;
    }
    // To compute the mask we set bounds on a maximum permissions capability and
    // return the mask that was used to adjust the length
    _cc_cap_t tmpcap;
    memset(&tmpcap, 0, sizeof(tmpcap));
    tmpcap.cr_tag = 1;
    tmpcap._cr_top = _CC_MAX_TOP;
    _cc_N(update_otype)(&tmpcap, _CC_N(OTYPE_UNSEALED));
    _cc_N(update_ebt)(&tmpcap, _CC_N(RESET_EBT));
    _cc_addr_t mask = 0;
    _cc_N(setbounds_impl)(&tmpcap, 0, req_length, &mask);
    return mask;
}

static inline _cc_cap_t _cc_N(make_max_perms_cap)(_cc_addr_t base, _cc_addr_t cursor, _cc_length_t top) {
    _cc_cap_t creg;
    memset(&creg, 0, sizeof(creg));
    assert(base <= top && "Invalid arguments");
    creg.cr_base = base;
    creg._cr_cursor = cursor;
    creg.cr_bounds_valid = true;
    creg._cr_top = top;
    creg.cr_pesbt = _CC_ENCODE_FIELD(_CC_N(UPERMS_ALL), UPERMS) | _CC_ENCODE_FIELD(_CC_N(PERMS_ALL), HWPERMS) |
                    _CC_ENCODE_FIELD(_CC_N(OTYPE_UNSEALED), OTYPE);
    creg.cr_tag = true;
    creg.cr_exp = _CC_N(NULL_EXP);
    bool exact_input = false;
    _cc_N(update_ebt)(&creg, _cc_N(compute_ebt)(creg.cr_base, creg._cr_top, NULL, &exact_input));
    assert(exact_input && "Invalid arguments");
    assert(_cc_N(is_representable_cap_exact)(&creg));
    return creg;
}

static inline _cc_addr_t _cc_N(get_required_alignment)(_cc_addr_t req_length) {
    // To get the required alignment from the CRAM mask we can just invert
    // the bits and add one to get a power-of-two
    return ~_cc_N(get_alignment_mask)(req_length) + 1;
}

static inline _cc_addr_t _cc_N(get_representable_length)(_cc_addr_t req_length) {
    _cc_addr_t mask = _cc_N(get_alignment_mask)(req_length);
    return (req_length + ~mask) & mask;
}

/// Provide a C++ class with the same function names
/// to simplify writing code that handles both 128 and 64-bit capabilities
#ifdef __cplusplus
inline bool _cc_N(cap)::operator==(const _cc_N(cap) & other) const { return _cc_N(exactly_equal)(this, &other); }

class _CC_CONCAT(CompressedCap, CC_FORMAT_LOWER) {
public:
    using length_t = _cc_length_t;
    using offset_t = _cc_offset_t;
    using addr_t = _cc_addr_t;
    using cap_t = _cc_cap_t;
    using bounds_bits = _cc_bounds_bits;

    static inline addr_t compress_raw(const cap_t* csp) { return _cc_N(compress_raw)(csp); }
    static inline void decompress_raw(addr_t pesbt, addr_t cursor, bool tag, cap_t* cdp) {
        _cc_N(decompress_raw)(pesbt, cursor, tag, cdp);
    }
    static inline addr_t compress_mem(const cap_t* csp) { return _cc_N(compress_mem)(csp); }
    static inline void decompress_mem(addr_t pesbt, addr_t cursor, bool tag, cap_t* cdp) {
        _cc_N(decompress_mem)(pesbt, cursor, tag, cdp);
    }
    static inline bounds_bits extract_bounds_bits(addr_t pesbt) { return _cc_N(extract_bounds_bits)(pesbt); }
    static inline bool setbounds(cap_t* cap, addr_t req_base, length_t req_top) {
        return _cc_N(setbounds)(cap, req_base, req_top);
    }
    static inline bool is_representable_cap_exact(const cap_t* cap) { return _cc_N(is_representable_cap_exact)(cap); }
    static inline cap_t make_max_perms_cap(addr_t base, addr_t cursor, length_t top) {
        return _cc_N(make_max_perms_cap)(base, cursor, top);
    }
    static inline addr_t representable_length(addr_t len) { return _cc_N(get_representable_length)(len); }
    static inline addr_t representable_mask(addr_t len) { return _cc_N(get_alignment_mask)(len); }
};
#define CompressedCapCC _CC_CONCAT(CompressedCap, CC_FORMAT_LOWER)
#endif
