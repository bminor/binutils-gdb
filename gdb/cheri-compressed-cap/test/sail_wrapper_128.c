/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Alex Richardson
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

#include "sail.h"

/* Provide the 128-specific APIs for sail_wrapper_common.c */

#define SAIL_COMPRESSION_GENERATED_C_FILE "sail_compression_128.c"
#define sail_bounds_tuple ztuple_z8z5bv64zCz0z5bvz9

#define SAIL_WRAPPER_CC_FORMAT_LOWER 128
#define SAIL_WRAPPER_CC_FORMAT_UPPER 128

/* CHERI-64 uses uint64_t, CHERI-128 uses lbits */
typedef lbits sail_cap_bits;
#define SAIL_COMPRESSION_INDIRECT_BITS
static void create_sail_cap_bits(sail_cap_bits* bits) { create_lbits(bits); }
static void kill_sail_cap_bits(sail_cap_bits* bits) { kill_lbits(bits); }
static void pesbt_and_addr_to_sail_cap_bits(sail_cap_bits* out, uint64_t pesbt, uint64_t cursor) {
    lbits sail_pesbt;
    lbits sail_cursor;
    CREATE_OF(lbits, fbits)(&sail_pesbt, pesbt, 64, true);
    CREATE_OF(lbits, fbits)(&sail_cursor, cursor, 64, true);
    CREATE(lbits)(out);
    append(out, sail_pesbt, sail_cursor);
    KILL(lbits)(&sail_pesbt);
    KILL(lbits)(&sail_cursor);
}
static void cc_length_t_to_sail_cap_bits(sail_cap_bits* out, unsigned __int128 len) {
    lbits len_high;
    lbits len_low;
    CREATE_OF(lbits, fbits)(&len_high, len >> 64, 1, true);
    CREATE_OF(lbits, fbits)(&len_low, (uint64_t)len, 64, true);
    append(out, len_high, len_low);
    KILL(lbits)(&len_high);
    KILL(lbits)(&len_low);
}

static inline uint64_t extract_bits(lbits op, uint64_t start, uint64_t len);
static inline uint64_t extract_sail_cap_bits(sail_cap_bits* bits, uint64_t start, uint64_t len) {
    return extract_bits(*bits, start, len);
}

#include "sail_wrapper_common.c"

static inline void set_top_base_from_sail(const struct zCapability* sail, _cc_cap_t* c) {
    // Would be nice to have a stable name for this tuple:
    struct sail_bounds_tuple base_top;
    _CC_CONCAT(create_, sail_bounds_tuple)(&base_top);
    sailgen_getCapBoundsBits(&base_top, *sail);
    c->cr_base = base_top.ztup0;
    sail_int top_len;
    CREATE(sail_int)(&top_len);
    length_lbits(&top_len, base_top.ztup1);
    assert(CONVERT_OF(mach_int, sail_int)(top_len) == 65);
    KILL(sail_int)(&top_len);
    fbits top_high = extract_bits(base_top.ztup1, 64, 1);
    fbits top_low = CONVERT_OF(fbits, lbits)(base_top.ztup1, true);
    c->_cr_top = (((cc128_length_t)top_high) << 64) | (cc128_length_t)top_low;
    _CC_CONCAT(kill_, sail_bounds_tuple)(&base_top);
}

/* Exported API */
void sail_decode_128_mem(uint64_t mem_pesbt, uint64_t mem_cursor, bool tag, cc128_cap_t* cdp) {
    sail_decode_common_mem(mem_pesbt, mem_cursor, tag, cdp);
}
void sail_decode_128_raw(uint64_t mem_pesbt, uint64_t mem_cursor, bool tag, cc128_cap_t* cdp) {
    sail_decode_common_raw(mem_pesbt, mem_cursor, tag, cdp);
}
struct cc128_bounds_bits sail_extract_bounds_bits_128(uint64_t pesbt) {
    return sail_extract_bounds_bits_common(pesbt);
}
uint64_t sail_compress_128_raw(const cc128_cap_t* csp) { return sail_compress_common_raw(csp); }
uint64_t sail_compress_128_mem(const cc128_cap_t* csp) { return sail_compress_common_mem(csp); }

cc128_addr_t sail_representable_mask_128(cc128_addr_t len) { return sailgen_getRepresentableAlignmentMask(len); }
cc128_addr_t sail_representable_length_128(cc128_addr_t len) { return sailgen_getRepresentableLength(len); }
