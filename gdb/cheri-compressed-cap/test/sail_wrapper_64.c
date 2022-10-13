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

/* Provide the 64-specific APIs for sail_wrapper_common.c */
#define SAIL_COMPRESSION_GENERATED_C_FILE "sail_compression_64.c"
// Would be nice to have a stable name for this tuple:
#define sail_bounds_tuple ztuple_z8z5bv32zCz0z5bv33z9

#define SAIL_WRAPPER_CC_FORMAT_LOWER 64
#define SAIL_WRAPPER_CC_FORMAT_UPPER 64

/* CHERI-64 uses uint64_t, CHERI-128 uses lbits */
typedef uint64_t sail_cap_bits;
static void CREATE(sail_cap_bits)(sail_cap_bits* bits) { *bits = 0; }
static void KILL(sail_cap_bits)(sail_cap_bits* bits) { *bits = UINT64_MAX; }
static void pesbt_and_addr_to_sail_cap_bits(sail_cap_bits* out, uint32_t pesbt, uint32_t cursor) {
    *out = (sail_cap_bits)pesbt << 32 | (sail_cap_bits)cursor;
}
static void cc_length_t_to_sail_cap_bits(sail_cap_bits* out, uint64_t len) { *out = len; }
static uint64_t cc64_getbits(uint64_t bits, uint32_t start, uint32_t size);
static uint64_t extract_sail_cap_bits(sail_cap_bits* bits, uint64_t start, uint64_t len) {
    return cc64_getbits(*bits, start, len);
}
#include "sail_wrapper_common.c"

static inline void set_top_base_from_sail(const struct zCapability* sail, _cc_cap_t* c) {
    struct sail_bounds_tuple base_top = sailgen_getCapBoundsBits(*sail);
    c->cr_base = base_top.ztup0;
    c->_cr_top = base_top.ztup1;
}

/* Exported API */
void sail_decode_64_mem(cc64_addr_t mem_pesbt, cc64_addr_t mem_cursor, bool tag, cc64_cap_t* cdp) {
    sail_decode_common_mem(mem_pesbt, mem_cursor, tag, cdp);
}
void sail_decode_64_raw(cc64_addr_t mem_pesbt, cc64_addr_t mem_cursor, bool tag, cc64_cap_t* cdp) {
    sail_decode_common_raw(mem_pesbt, mem_cursor, tag, cdp);
}
struct cc64_bounds_bits sail_extract_bounds_bits_64(cc64_addr_t pesbt) {
    return sail_extract_bounds_bits_common(pesbt);
}
cc64_addr_t sail_compress_64_raw(const cc64_cap_t* csp) { return sail_compress_common_raw(csp); }
cc64_addr_t sail_compress_64_mem(const cc64_cap_t* csp) { return sail_compress_common_mem(csp); }

cc64_addr_t sail_representable_mask_64(cc64_addr_t len) { return sailgen_getRepresentableAlignmentMask(len); }
cc64_addr_t sail_representable_length_64(cc64_addr_t len) { return sailgen_getRepresentableLength(len); }
