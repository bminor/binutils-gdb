#include <cinttypes>
#include <iostream>
#include <ostream>
#include <string>

#define CC_FORMAT_LOWER TEST_CC_FORMAT_LOWER
#define CC_FORMAT_UPPER TEST_CC_FORMAT_UPPER

#define DO_STRINGIFY2(x) #x
#define DO_STRINGIFY1(x) DO_STRINGIFY2(x)
#define STRINGIFY(x) DO_STRINGIFY1(x)

static const char* otype_suffix(uint32_t otype) {
    switch (otype) {
#define OTYPE_CASE(Name, ...)                                                                                          \
    case _CC_N(Name): return " (" STRINGIFY(_CC_N(Name)) ")";
        _CC_N(LS_SPECIAL_OTYPES)(OTYPE_CASE, )
    default: return "";
    }
}

#ifndef TEST_CC_IS_CHERI256
std::ostream& operator<<(std::ostream& os, const _cc_N(bounds_bits)& value);
std::ostream& operator<<(std::ostream& os, const _cc_N(bounds_bits)& value) {
    os << "{ B: " << (unsigned)value.B << " T: " << (unsigned)value.T << " E: " << (unsigned)value.E
       << " IE: " << (unsigned)value.IE << " }";
    return os;
}

static inline bool operator==(const _cc_N(bounds_bits)& a, const _cc_N(bounds_bits)& b) {
    return a.B == b.B && a.E == b.E && a.T == b.T && a.IE == b.IE;
}
#endif

#include "catch.hpp"

static bool failed = false;

template <typename T> static inline bool check(T expected, T actual, const std::string& msg) {
    if (expected == actual)
        return true;
    std::cerr << "ERROR: " << msg << ": expected 0x" << std::hex << expected << " != 0x" << actual << "\n";
    failed = true;
    return false;
}

template <class T, std::size_t N> constexpr inline size_t array_lengthof(T (&)[N]) { return N; }

template <class Cap> static void dump_cap_fields(const Cap& result) {
    fprintf(stderr, "Permissions: 0x%" PRIx32 "\n", result.permissions()); // TODO: decode perms
    fprintf(stderr, "User Perms:  0x%" PRIx32 "\n", result.software_permissions());
    fprintf(stderr, "Base:        0x%016" PRIx64 "\n", (uint64_t)result.base());
    fprintf(stderr, "Offset:      0x%016" PRIx64 "\n", (uint64_t)result.offset());
    fprintf(stderr, "Cursor:      0x%016" PRIx64 "\n", (uint64_t)result.address());
    unsigned __int128 len_full = result.length();
    fprintf(stderr, "Length:      0x%" PRIx64 "%016" PRIx64 " %s\n", (uint64_t)(len_full >> 64), (uint64_t)len_full,
            len_full > UINT64_MAX ? " (greater than UINT64_MAX)" : "");
    unsigned __int128 top_full = result.top();
    fprintf(stderr, "Top:         0x%" PRIx64 "%016" PRIx64 " %s\n", (uint64_t)(top_full >> 64), (uint64_t)top_full,
            top_full > UINT64_MAX ? " (greater than UINT64_MAX)" : "");
    fprintf(stderr, "Flags:       %d\n", (int)result.flags());
    fprintf(stderr, "Reserved:    %d\n", (int)result.reserved_bits());
    fprintf(stderr, "Sealed:      %d\n", (int)result.is_sealed());
    fprintf(stderr, "OType:       0x%" PRIx32 "%s\n", result.type(), otype_suffix(result.type()));
    fprintf(stderr, "\n");
}

#ifndef TEST_CC_IS_CHERI256
__attribute__((used)) static _cc_cap_t decompress_representable(_cc_addr_t pesbt_already_xored, _cc_addr_t cursor) {
    _cc_cap_t result;
    printf("Decompressing pesbt = %016" PRIx64 ", cursor = %016" PRIx64 "\n", (uint64_t)pesbt_already_xored, (uint64_t)cursor);
    _cc_N(decompress_raw)(pesbt_already_xored, cursor, false, &result);
    dump_cap_fields(result);
    // Check that the result is the same again when compressed
    _cc_addr_t new_pesbt_already_xored = _cc_N(compress_raw)(&result);
    CHECK(pesbt_already_xored == new_pesbt_already_xored);
    CHECK(cursor == result.address());
    return result;
}

inline _cc_cap_t make_max_perms_cap(_cc_addr_t base, _cc_addr_t offset, _cc_length_t length) {
    return _cc_N(make_max_perms_cap)(base, offset, length);
}
#endif

#define CHECK_FIELD_RAW(value, expected) CHECK(value == expected)
#define CHECK_FIELD(cap, field, expected) CHECK((uint64_t)expected == cap.field())

enum {
#ifdef CC128_OLD_FORMAT
    TESTING_OLD_FORMAT = 1
#else
    TESTING_OLD_FORMAT = 0
#endif
};
