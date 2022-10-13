#define TEST_CC_FORMAT_LOWER 128
#define TEST_CC_FORMAT_UPPER 128

#include "simple_test_common.cpp"

TEST_CASE("QEMU representability regression tests", "[representable]") {
    // regression test from QEMU broken cincoffset
    CHECK(check_repr(false, 0x00000000000b7fcc, 0x00000000000000e1, 52));
    check_representable(0x00000000000b7fcc, 0x00000000000000e1, 52, true, "regression");
    // $c17: v:1 s:0 p:0007817d b:00000000401cf020 l:0000000000001800 o:0 t:-1 + 0x1800
    check_representable(0x00000000401cf020, 0x0000000000001800, 0x1800, true, "regression");
    CHECK(check_repr(false, 0x00000000401cf020, 0x0000000000001800, 0x1800));
    // $c18: v:1 s:0 p:0007817d b:00000000401ffff8 l:0000000000000008 o:0 t:-1 + 0x8
    check_representable(0x00000000401ffff8, 0x0000000000000008, 0x8, true, "regression");
    CHECK(check_repr(false, 0x00000000401ffff8, 0x0000000000000008, 0x8));
}
