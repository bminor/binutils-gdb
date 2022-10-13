#include "test_common.cpp"

// Due to magic constant XOR aversion (i.e. fields are either entirely
// inverted or not at all, rather than select bits within them like in
// normal CHERI Concentrate), NULL is special in Morello, with bounds
// using the maximum encodable exponent, not the minimum necessary one
// like is used for the reset capability.
#ifndef TEST_CC_IS_MORELLO
TEST_CASE("Compressed NULL cap has canonical bounds", "[nullcap]") {
    _cc_cap_t null_cap;
    memset(&null_cap, 0, sizeof(null_cap));
    _cc_N(update_otype)(&null_cap, _CC_N(OTYPE_UNSEALED));
    null_cap._cr_top = _CC_N(NULL_TOP);
    null_cap.cr_pesbt = _CC_N(NULL_PESBT);
    bool ebt_exact = true;
    uint32_t computed_ebt = _cc_N(compute_ebt)(null_cap.cr_base, null_cap._cr_top, NULL, &ebt_exact);
    REQUIRE((computed_ebt & _CC_N(FIELD_EBT_MASK64)) == (null_cap.cr_pesbt & _CC_N(FIELD_EBT_MASK64)));
    REQUIRE(ebt_exact);
}
#endif

// TODO: Implement non-stub sail_compress_common_mem/raw for Morello
#ifndef TEST_CC_IS_MORELLO
TEST_CASE("Compressed NULL cap encodes to zeroes", "[nullcap]") {
    _cc_cap_t null_cap;
    memset(&null_cap, 0, sizeof(null_cap));
    _cc_N(update_otype)(&null_cap, _CC_N(OTYPE_UNSEALED));
    null_cap._cr_top = _CC_N(NULL_TOP);
    null_cap.cr_pesbt = _CC_N(NULL_PESBT);
    auto pesbt = _cc_N(compress_mem)(&null_cap);
    auto pesbt_from_sail = _cc_sail_compress_mem(&null_cap);
    CHECK(pesbt == pesbt_from_sail);
    auto pesbt_without_xor = _cc_N(compress_raw)(&null_cap);
    auto pesbt_from_sail_without_xor = _cc_sail_compress_raw(&null_cap);
    CHECK(pesbt_without_xor == pesbt_from_sail_without_xor);
    fprintf(stderr, "NULL ENCODED: 0x%llx\n", (long long)pesbt_without_xor);
    CHECK(pesbt_without_xor == _CC_N(NULL_XOR_MASK));
    check(pesbt, (_cc_addr_t)0, "compressing NULL should result in zero pesbt");
    _cc_cap_t decompressed;
    memset(&decompressed, 'b', sizeof(decompressed));
    _cc_N(decompress_mem)(pesbt, 0, false, &decompressed);
    CHECK_FIELD(decompressed, base, 0);
    CHECK(decompressed.address() == 0);
    CHECK_FIELD(decompressed, software_permissions, 0);
    CHECK_FIELD(decompressed, permissions, 0);
    CHECK((decompressed.cr_pesbt & _CC_N(FIELD_EBT_MASK64)) == (_CC_N(NULL_PESBT) & _CC_N(FIELD_EBT_MASK64)));
    CHECK(_cc_N(get_reserved)(&decompressed) == 0); // reserved bits
    CHECK(decompressed.length() == _CC_N(NULL_LENGTH));
    CHECK(decompressed.top() == _CC_N(NULL_TOP));
    CHECK_FIELD(decompressed, type, _CC_N(OTYPE_UNSEALED));
}
#endif

TEST_CASE("Zeroes decode to NULL cap", "[nullcap]") {
    _cc_cap_t result;
    memset(&result, 'a', sizeof(result));
    CompressedCapCC::decompress_mem(0, 0, false, &result);
    fprintf(stderr, "Decompressed NULL cap:\n");
    dump_cap_fields(result);
    fprintf(stderr, "\n");
    CHECK_FIELD(result, base, 0);
    CHECK_FIELD(result, offset, 0);
    CHECK_FIELD(result, software_permissions, 0);
    CHECK_FIELD(result, permissions, 0);
    CHECK_FIELD(result, flags, 0);
    CHECK_FIELD(result, reserved_bits, 0);
    CHECK((result.cr_pesbt & _CC_N(FIELD_EBT_MASK64)) == (_CC_N(NULL_PESBT) & _CC_N(FIELD_EBT_MASK64)));
    CHECK_FIELD_RAW(result.length(), _CC_N(NULL_LENGTH));
    CHECK_FIELD(result, type, _CC_N(OTYPE_UNSEALED));
}

static void check_representable(_cc_addr_t base, _cc_length_t length, _cc_addr_t offset, bool should_work,
                                const std::string& ctx) {
    // INFO("Checking representability for " << ctx);
    // INFO("Base = " << base);
    // INFO("Length = " << length);
    // INFO("Expected to work = " << should_work);
    CAPTURE(base, length, should_work, ctx);
    _cc_cap_t cap;
    memset(&cap, 0, sizeof(cap));
    cap.cr_base = base;
    cap._cr_cursor = base + offset;
    cap._cr_top = base + length;
    cap.cr_tag = true;
    bool exact_input = false;
    cap.cr_pesbt = _cc_N(compute_ebt)(cap.cr_base, cap._cr_top, NULL, &exact_input);
    _cc_N(update_otype)(&cap, _CC_N(OTYPE_UNSEALED));
    REQUIRE(exact_input == should_work);
    _cc_addr_t compressed = _cc_N(compress_mem)(&cap);
    _cc_cap_t decompressed;
    memset(&decompressed, 0, sizeof(decompressed));

    _cc_N(decompress_mem)(compressed, cap.cr_base + cap.offset(), cap.cr_tag, &decompressed);
    CAPTURE(cap);
    CAPTURE(decompressed);
    bool unsealed_roundtrip = cap.cr_base == decompressed.cr_base && cap.length() == decompressed.length() &&
                              cap.offset() == decompressed.offset();
    bool unsealed_representable = _cc_N(is_representable_cap_exact)(&cap);
    CHECK(unsealed_representable == should_work);
    CHECK(unsealed_roundtrip == unsealed_representable);
    // TODO: CHECK(fast_representable == unsealed_representable);
    auto cap_sealed = cap;
    _cc_N(update_otype)(&cap_sealed, 5);
    bool sealed_representable = _cc_N(is_representable_cap_exact)(&cap_sealed);
    _cc_N(decompress_mem)(compressed, cap.cr_base + cap.offset(), cap.cr_tag, &decompressed);
    bool sealed_roundtrip = cap.cr_base == decompressed.cr_base && cap.length() == decompressed.length() &&
                            cap.offset() == decompressed.offset();
    CHECK(sealed_representable == should_work);
    CHECK(sealed_roundtrip == unsealed_representable);
    // fprintf(stderr, "Base 0x%" PRIx64 " Len 0x%" PRIx64 "%016" PRIx64 ": roundtrip: sealed=%d, unsealed=%d -- Fast:
    // sealed=%d, unsealed=%d\n",
    //        base, (uint64_t)(length >> 64), (uint64_t)length, sealed_roundtrip, unsealed_roundtrip,
    //        sealed_representable, unsealed_representable);
}

static inline bool check_repr(bool sealed, _cc_addr_t base, _cc_addr_t length, _cc_addr_t offset) {
    (void)sealed;
    auto cap = make_max_perms_cap(base, base + offset, base + length);
    return _cc_N(is_representable_cap_exact)(&cap);
}

TEST_CASE("Check max size cap representable", "[representable]") {
    // 0000000d b:0000000000000000 l:ffffffffffffffff |o:0000000000000000 t:ffffff
    check_representable(0, _CC_N(MAX_ADDRESS_PLUS_ONE), 0, true, "MAX_ADDRESS_PLUS_ONE length");
    check_representable(0, 0, 0, true, "zero length");
    check_representable(0, _CC_N(MAX_ADDR), 0, false, "MAX_ADDR length");

    check_representable((_cc_addr_t)0xffffffffff000000, 0x00000000000ffffff, 0, false, "length with too many bits");
}

// TODO: Implement sail_null_pesbt_128
#ifndef TEST_CC_IS_MORELLO
TEST_CASE("Check NULL mask matches sail", "[sail]") {
    CHECK(_cc_sail_null_pesbt() == _CC_N(NULL_PESBT));
    CHECK(_cc_sail_null_pesbt() == _CC_N(NULL_XOR_MASK));
}
#endif
