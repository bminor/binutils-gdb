#define TEST_CC_FORMAT_LOWER 256
#define TEST_CC_FORMAT_UPPER 256
#define TEST_CC_IS_CHERI256

#include "test_common.cpp"

TEST_CASE("Compressed NULL cap encodes to zeroes", "[nullcap]") {
    cc256_cap_t null_cap;
    memset(&null_cap, 0, sizeof(null_cap));
    null_cap.cr_otype = CC256_OTYPE_UNSEALED;
    null_cap.cr_length = CC256_NULL_LENGTH;
    inmemory_chericap256 buffer;
    compress_256cap(&buffer, &null_cap);
    check(buffer.u64s[0], (uint64_t)0, "compressing NULL should result in zero[0]");
    check(buffer.u64s[1], (uint64_t)0, "compressing NULL should result in zero[1]");
    check(buffer.u64s[2], (uint64_t)0, "compressing NULL should result in zero[2]");
    check(buffer.u64s[3], (uint64_t)0, "compressing NULL should result in zero[3]");

    // compressing this result should give 0 again
    cc256_cap_t decompressed;
    memset(&decompressed, 'b', sizeof(decompressed));
    decompress_256cap(buffer, &decompressed, false);
    CHECK_FIELD(decompressed, base, 0);
    CHECK_FIELD(decompressed, offset, 0);
    CHECK_FIELD(decompressed, software_permissions, 0);
    CHECK_FIELD(decompressed, permissions, 0);
    CHECK_FIELD_RAW(decompressed.length(), CC256_NULL_LENGTH);
    CHECK_FIELD(decompressed, type, CC256_OTYPE_UNSEALED);
}

TEST_CASE("Zeroes decode to NULL cap", "[nullcap]") {
    inmemory_chericap256 buffer;
    cc256_cap_t result;
    memset(&buffer.bytes, 0, sizeof(buffer));
    memset(&result, 'a', sizeof(result));
    decompress_256cap(buffer, &result, false);
    fprintf(stderr, "Decompressed 256-bit NULL cap:\n");
    dump_cap_fields(result);
    fprintf(stderr, "\n");
    CHECK_FIELD(result, base, 0);
    CHECK_FIELD(result, offset, 0);
    CHECK_FIELD(result, software_permissions, 0);
    CHECK_FIELD(result, permissions, 0);
    CHECK_FIELD(result, flags, 0);
    CHECK_FIELD(result, reserved_bits, 0);
    CHECK_FIELD_RAW(result.length(), CC256_NULL_LENGTH);
    CHECK_FIELD(result, type, CC256_OTYPE_UNSEALED);
}
