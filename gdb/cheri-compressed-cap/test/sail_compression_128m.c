#include "elf.h"
#include "rts.h"
#include "sail.h"

static void sailgen_zzz3zzz3__id(sail_int* rop, sail_int);

static void sailgen_zzz3zzz3__id(sail_int* zcbz30, sail_int zx) {
    __label__ end_function_1, end_block_exception_2, end_function_131;

    COPY(sail_int)((*(&zcbz30)), zx);
end_function_1:;
    goto end_function_131;
end_block_exception_2:;
    goto end_function_131;
end_function_131:;
}

static void sailgen_zzz3zzz3Zeros(lbits* rop, sail_int);

static void sailgen_zzz3zzz3Zeros(lbits* zcbz31, sail_int zn) {
    __label__ end_function_4, end_block_exception_5, end_function_130;

    zeros((*(&zcbz31)), zn);
end_function_4:;
    goto end_function_130;
end_block_exception_5:;
    goto end_function_130;
end_function_130:;
}

static fbits sailgen_Bit(uint64_t);

static fbits sailgen_Bit(uint64_t zb) {
    __label__ end_function_7, end_block_exception_8;

    fbits zcbz32;
    {
        sail_int zgsz31;
        CREATE(sail_int)(&zgsz31);
        CONVERT_OF(sail_int, mach_int)(&zgsz31, INT64_C(0));
        lbits zgsz30;
        CREATE(lbits)(&zgsz30);
        CONVERT_OF(lbits, fbits)(&zgsz30, zb, UINT64_C(1), true);
        zcbz32 = bitvector_access(zgsz30, zgsz31);
        KILL(sail_int)(&zgsz31);
        KILL(lbits)(&zgsz30);
    }
end_function_7:;
    return zcbz32;
end_block_exception_8:;

    return UINT64_C(0);
}

static void sailgen_Slice_int(lbits* rop, sail_int, sail_int, sail_int);

static void sailgen_Slice_int(lbits* zcbz33, sail_int zi, sail_int zl, sail_int zn) {
    __label__ end_function_10, end_block_exception_11, end_function_129;

    get_slice_int((*(&zcbz33)), zn, zi, zl);
end_function_10:;
    goto end_function_129;
end_block_exception_11:;
    goto end_function_129;
end_function_129:;
}

static int64_t zCAP_FLAGS_LO_BIT;
static void create_letbind_0(void) {

    int64_t zgsz32;
    zgsz32 = INT64_C(56);
    zCAP_FLAGS_LO_BIT = zgsz32;

let_end_12:;
}
static void kill_letbind_0(void) {}

static int64_t zCAP_VALUE_HI_BIT;
static void create_letbind_1(void) {

    int64_t zgsz33;
    zgsz33 = INT64_C(63);
    zCAP_VALUE_HI_BIT = zgsz33;

let_end_13:;
}
static void kill_letbind_1(void) {}

static int64_t zCAP_VALUE_LO_BIT;
static void create_letbind_2(void) {

    int64_t zgsz34;
    zgsz34 = INT64_C(0);
    zCAP_VALUE_LO_BIT = zgsz34;

let_end_14:;
}
static void kill_letbind_2(void) {}

static int64_t zCAP_VALUE_NUM_BITS;
static void create_letbind_3(void) {

    int64_t zgsz38;
    int64_t zgaz30;
    {
        sail_int zgsz35;
        CREATE(sail_int)(&zgsz35);
        CONVERT_OF(sail_int, mach_int)(&zgsz35, zCAP_VALUE_HI_BIT);
        sail_int zgsz36;
        CREATE(sail_int)(&zgsz36);
        CONVERT_OF(sail_int, mach_int)(&zgsz36, zCAP_VALUE_LO_BIT);
        sail_int zgsz37;
        CREATE(sail_int)(&zgsz37);
        sub_int(&zgsz37, zgsz35, zgsz36);
        zgaz30 = CONVERT_OF(mach_int, sail_int)(zgsz37);
        KILL(sail_int)(&zgsz37);
        KILL(sail_int)(&zgsz36);
        KILL(sail_int)(&zgsz35);
    }
    {
        sail_int zgsz39;
        CREATE(sail_int)(&zgsz39);
        CONVERT_OF(sail_int, mach_int)(&zgsz39, zgaz30);
        sail_int zgsz310;
        CREATE(sail_int)(&zgsz310);
        CONVERT_OF(sail_int, mach_int)(&zgsz310, INT64_C(1));
        sail_int zgsz311;
        CREATE(sail_int)(&zgsz311);
        add_int(&zgsz311, zgsz39, zgsz310);
        zgsz38 = CONVERT_OF(mach_int, sail_int)(zgsz311);
        KILL(sail_int)(&zgsz311);
        KILL(sail_int)(&zgsz310);
        KILL(sail_int)(&zgsz39);
    }

    zCAP_VALUE_NUM_BITS = zgsz38;

let_end_15:;
}
static void kill_letbind_3(void) {}

static int64_t zCAP_BASE_HI_BIT;
static void create_letbind_4(void) {

    int64_t zgsz312;
    zgsz312 = INT64_C(79);
    zCAP_BASE_HI_BIT = zgsz312;

let_end_16:;
}
static void kill_letbind_4(void) {}

static int64_t zCAP_BASE_LO_BIT;
static void create_letbind_5(void) {

    int64_t zgsz313;
    zgsz313 = INT64_C(64);
    zCAP_BASE_LO_BIT = zgsz313;

let_end_17:;
}
static void kill_letbind_5(void) {}

static int64_t zCAP_MW;
static void create_letbind_6(void) {

    int64_t zgsz317;
    int64_t zgaz31;
    {
        sail_int zgsz314;
        CREATE(sail_int)(&zgsz314);
        CONVERT_OF(sail_int, mach_int)(&zgsz314, zCAP_BASE_HI_BIT);
        sail_int zgsz315;
        CREATE(sail_int)(&zgsz315);
        CONVERT_OF(sail_int, mach_int)(&zgsz315, zCAP_BASE_LO_BIT);
        sail_int zgsz316;
        CREATE(sail_int)(&zgsz316);
        sub_int(&zgsz316, zgsz314, zgsz315);
        zgaz31 = CONVERT_OF(mach_int, sail_int)(zgsz316);
        KILL(sail_int)(&zgsz316);
        KILL(sail_int)(&zgsz315);
        KILL(sail_int)(&zgsz314);
    }
    {
        sail_int zgsz318;
        CREATE(sail_int)(&zgsz318);
        CONVERT_OF(sail_int, mach_int)(&zgsz318, zgaz31);
        sail_int zgsz319;
        CREATE(sail_int)(&zgsz319);
        CONVERT_OF(sail_int, mach_int)(&zgsz319, INT64_C(1));
        sail_int zgsz320;
        CREATE(sail_int)(&zgsz320);
        add_int(&zgsz320, zgsz318, zgsz319);
        zgsz317 = CONVERT_OF(mach_int, sail_int)(zgsz320);
        KILL(sail_int)(&zgsz320);
        KILL(sail_int)(&zgsz319);
        KILL(sail_int)(&zgsz318);
    }

    zCAP_MW = zgsz317;

let_end_18:;
}
static void kill_letbind_6(void) {}

static int64_t zCAP_BASE_EXP_HI_BIT;
static void create_letbind_7(void) {

    int64_t zgsz321;
    zgsz321 = INT64_C(66);
    zCAP_BASE_EXP_HI_BIT = zgsz321;

let_end_19:;
}
static void kill_letbind_7(void) {}

static int64_t zCAP_LIMIT_EXP_HI_BIT;
static void create_letbind_8(void) {

    int64_t zgsz322;
    zgsz322 = INT64_C(82);
    zCAP_LIMIT_EXP_HI_BIT = zgsz322;

let_end_20:;
}
static void kill_letbind_8(void) {}

static int64_t zCAP_LIMIT_LO_BIT;
static void create_letbind_9(void) {

    int64_t zgsz323;
    zgsz323 = INT64_C(80);
    zCAP_LIMIT_LO_BIT = zgsz323;

let_end_21:;
}
static void kill_letbind_9(void) {}

static int64_t zCAP_IE_BIT;
static void create_letbind_10(void) {

    int64_t zgsz324;
    zgsz324 = INT64_C(94);
    zCAP_IE_BIT = zgsz324;

let_end_22:;
}
static void kill_letbind_10(void) {}

static bool sailgen_CapIsInternalExponent(lbits);

static bool sailgen_CapIsInternalExponent(lbits zc) {
    __label__ cleanup_25, end_cleanup_26, end_function_24, end_block_exception_27;

    bool zcbz34;
    bool zgaz34;
    {
        uint64_t zgaz33;
        {
            fbits zgaz32;
            {
                sail_int zgsz325;
                CREATE(sail_int)(&zgsz325);
                CONVERT_OF(sail_int, mach_int)(&zgsz325, zCAP_IE_BIT);
                zgaz32 = bitvector_access(zc, zgsz325);
                KILL(sail_int)(&zgsz325);
            }
            uint64_t zgsz326;
            zgsz326 = UINT64_C(0b0);
            zgsz326 = update_fbits(zgsz326, INT64_C(0), zgaz32);
            zgaz33 = zgsz326;
        }
        {
            lbits zgsz328;
            CREATE(lbits)(&zgsz328);
            CONVERT_OF(lbits, fbits)(&zgsz328, UINT64_C(0b0), UINT64_C(1), true);
            lbits zgsz327;
            CREATE(lbits)(&zgsz327);
            CONVERT_OF(lbits, fbits)(&zgsz327, zgaz33, UINT64_C(1), true);
            zgaz34 = eq_bits(zgsz327, zgsz328);
            KILL(lbits)(&zgsz328);
            KILL(lbits)(&zgsz327);
        }
    }
    zcbz34 = zgaz34;
    goto cleanup_25;
    /* unreachable after return */

    goto end_cleanup_26;
cleanup_25:;

    goto end_function_24;
end_cleanup_26:;
end_function_24:;
    return zcbz34;
end_block_exception_27:;

    return false;
}

static int64_t sailgen_CapGetExponent(lbits);

static int64_t sailgen_CapGetExponent(lbits zc) {
    __label__ end_function_29, end_block_exception_34;

    int64_t zcbz35;
    bool zgaz35;
    zgaz35 = sailgen_CapIsInternalExponent(zc);
    if (zgaz35) {
        __label__ cleanup_32, end_cleanup_33;

        uint64_t znexp;
        {
            uint64_t zgaz38;
            {
                sail_int zgsz329;
                CREATE(sail_int)(&zgsz329);
                CONVERT_OF(sail_int, mach_int)(&zgsz329, zCAP_LIMIT_EXP_HI_BIT);
                sail_int zgsz330;
                CREATE(sail_int)(&zgsz330);
                CONVERT_OF(sail_int, mach_int)(&zgsz330, zCAP_LIMIT_LO_BIT);
                lbits zgsz331;
                CREATE(lbits)(&zgsz331);
                vector_subrange_lbits(&zgsz331, zc, zgsz329, zgsz330);
                zgaz38 = CONVERT_OF(fbits, lbits)(zgsz331, true);
                KILL(lbits)(&zgsz331);
                KILL(sail_int)(&zgsz330);
                KILL(sail_int)(&zgsz329);
            }
            uint64_t zgaz39;
            {
                sail_int zgsz332;
                CREATE(sail_int)(&zgsz332);
                CONVERT_OF(sail_int, mach_int)(&zgsz332, zCAP_BASE_EXP_HI_BIT);
                sail_int zgsz333;
                CREATE(sail_int)(&zgsz333);
                CONVERT_OF(sail_int, mach_int)(&zgsz333, zCAP_BASE_LO_BIT);
                lbits zgsz334;
                CREATE(lbits)(&zgsz334);
                vector_subrange_lbits(&zgsz334, zc, zgsz332, zgsz333);
                zgaz39 = CONVERT_OF(fbits, lbits)(zgsz334, true);
                KILL(lbits)(&zgsz334);
                KILL(sail_int)(&zgsz333);
                KILL(sail_int)(&zgsz332);
            }
            {
                lbits zgsz335;
                CREATE(lbits)(&zgsz335);
                CONVERT_OF(lbits, fbits)(&zgsz335, zgaz38, UINT64_C(3), true);
                lbits zgsz336;
                CREATE(lbits)(&zgsz336);
                CONVERT_OF(lbits, fbits)(&zgsz336, zgaz39, UINT64_C(3), true);
                lbits zgsz337;
                CREATE(lbits)(&zgsz337);
                append(&zgsz337, zgsz335, zgsz336);
                znexp = CONVERT_OF(fbits, lbits)(zgsz337, true);
                KILL(lbits)(&zgsz337);
                KILL(lbits)(&zgsz336);
                KILL(lbits)(&zgsz335);
            }
        }
        int64_t zgaz37;
        {
            uint64_t zgaz36;
            {
                lbits zgsz338;
                CREATE(lbits)(&zgsz338);
                CONVERT_OF(lbits, fbits)(&zgsz338, znexp, UINT64_C(6), true);
                lbits zgsz339;
                CREATE(lbits)(&zgsz339);
                not_bits(&zgsz339, zgsz338);
                zgaz36 = CONVERT_OF(fbits, lbits)(zgsz339, true);
                KILL(lbits)(&zgsz339);
                KILL(lbits)(&zgsz338);
            }
            zgaz37 = ((mach_int)zgaz36);
        }
        zcbz35 = zgaz37;
        goto cleanup_32;
        /* unreachable after return */

        goto end_cleanup_33;
    cleanup_32:;

        goto end_function_29;
    end_cleanup_33:;
    } else {
        __label__ cleanup_30, end_cleanup_31;

        zcbz35 = INT64_C(0);
        goto cleanup_30;
        /* unreachable after return */
        goto end_cleanup_31;
    cleanup_30:;

        goto end_function_29;
    end_cleanup_31:;
    }

end_function_29:;
    return zcbz35;
end_block_exception_34:;

    return INT64_C(0xdeadc0de);
}

static uint64_t sailgen_CapGetValue(lbits);

static uint64_t sailgen_CapGetValue(lbits zc) {
    __label__ cleanup_37, end_cleanup_38, end_function_36, end_block_exception_39;

    uint64_t zcbz36;
    uint64_t zgaz310;
    {
        sail_int zgsz340;
        CREATE(sail_int)(&zgsz340);
        CONVERT_OF(sail_int, mach_int)(&zgsz340, zCAP_VALUE_HI_BIT);
        sail_int zgsz341;
        CREATE(sail_int)(&zgsz341);
        CONVERT_OF(sail_int, mach_int)(&zgsz341, zCAP_VALUE_LO_BIT);
        lbits zgsz342;
        CREATE(lbits)(&zgsz342);
        vector_subrange_lbits(&zgsz342, zc, zgsz340, zgsz341);
        zgaz310 = CONVERT_OF(fbits, lbits)(zgsz342, true);
        KILL(lbits)(&zgsz342);
        KILL(sail_int)(&zgsz341);
        KILL(sail_int)(&zgsz340);
    }
    zcbz36 = zgaz310;
    goto cleanup_37;
    /* unreachable after return */

    goto end_cleanup_38;
cleanup_37:;

    goto end_function_36;
end_cleanup_38:;
end_function_36:;
    return zcbz36;
end_block_exception_39:;

    return UINT64_C(0xdeadc0de);
}

static int64_t zCAP_BOUND_NUM_BITS;
static void create_letbind_11(void) {

    int64_t zgsz343;
    {
        sail_int zgsz344;
        CREATE(sail_int)(&zgsz344);
        CONVERT_OF(sail_int, mach_int)(&zgsz344, zCAP_VALUE_NUM_BITS);
        sail_int zgsz345;
        CREATE(sail_int)(&zgsz345);
        CONVERT_OF(sail_int, mach_int)(&zgsz345, INT64_C(1));
        sail_int zgsz346;
        CREATE(sail_int)(&zgsz346);
        add_int(&zgsz346, zgsz344, zgsz345);
        zgsz343 = CONVERT_OF(mach_int, sail_int)(zgsz346);
        KILL(sail_int)(&zgsz346);
        KILL(sail_int)(&zgsz345);
        KILL(sail_int)(&zgsz344);
    }
    zCAP_BOUND_NUM_BITS = zgsz343;

let_end_40:;
}
static void kill_letbind_11(void) {}

static lbits zCAP_BOUND_MAX;
static void create_letbind_12(void) {
    CREATE(lbits)(&zCAP_BOUND_MAX);

    lbits zgsz349;
    CREATE(lbits)(&zgsz349);
    sail_int zgaz311;
    CREATE(sail_int)(&zgaz311);
    {
        sail_int zgsz348;
        CREATE(sail_int)(&zgsz348);
        CONVERT_OF(sail_int, mach_int)(&zgsz348, zCAP_VALUE_NUM_BITS);
        sail_int zgsz347;
        CREATE(sail_int)(&zgsz347);
        CONVERT_OF(sail_int, mach_int)(&zgsz347, INT64_C(1));
        shl_int(&zgaz311, zgsz347, zgsz348);
        KILL(sail_int)(&zgsz348);
        KILL(sail_int)(&zgsz347);
    }
    {
        sail_int zgsz351;
        CREATE(sail_int)(&zgsz351);
        CONVERT_OF(sail_int, mach_int)(&zgsz351, zCAP_BOUND_NUM_BITS);
        sail_int zgsz350;
        CREATE(sail_int)(&zgsz350);
        CONVERT_OF(sail_int, mach_int)(&zgsz350, INT64_C(0));
        sailgen_Slice_int(&zgsz349, zgaz311, zgsz350, zgsz351);
        KILL(sail_int)(&zgsz351);
        KILL(sail_int)(&zgsz350);
    }
    KILL(sail_int)(&zgaz311);
    COPY(lbits)(&zCAP_BOUND_MAX, zgsz349);
    KILL(lbits)(&zgsz349);
let_end_41:;
}
static void kill_letbind_12(void) { KILL(lbits)(&zCAP_BOUND_MAX); }

static lbits zCAP_BOUND_MIN;
static void create_letbind_13(void) {
    CREATE(lbits)(&zCAP_BOUND_MIN);

    lbits zgsz352;
    CREATE(lbits)(&zgsz352);
    int64_t zgaz312;
    zgaz312 = ((mach_int)UINT64_C(0x0));
    {
        sail_int zgsz355;
        CREATE(sail_int)(&zgsz355);
        CONVERT_OF(sail_int, mach_int)(&zgsz355, zCAP_BOUND_NUM_BITS);
        sail_int zgsz354;
        CREATE(sail_int)(&zgsz354);
        CONVERT_OF(sail_int, mach_int)(&zgsz354, INT64_C(0));
        sail_int zgsz353;
        CREATE(sail_int)(&zgsz353);
        CONVERT_OF(sail_int, mach_int)(&zgsz353, zgaz312);
        sailgen_Slice_int(&zgsz352, zgsz353, zgsz354, zgsz355);
        KILL(sail_int)(&zgsz355);
        KILL(sail_int)(&zgsz354);
        KILL(sail_int)(&zgsz353);
    }

    COPY(lbits)(&zCAP_BOUND_MIN, zgsz352);
    KILL(lbits)(&zgsz352);
let_end_42:;
}
static void kill_letbind_13(void) { KILL(lbits)(&zCAP_BOUND_MIN); }

static int64_t zCAP_MAX_ENCODEABLE_EXPONENT;
static void create_letbind_14(void) {

    int64_t zgsz356;
    zgsz356 = INT64_C(63);
    zCAP_MAX_ENCODEABLE_EXPONENT = zgsz356;

let_end_43:;
}
static void kill_letbind_14(void) {}

static int64_t zCAP_MAX_EXPONENT;
static void create_letbind_15(void) {

    int64_t zgsz360;
    int64_t zgaz313;
    {
        sail_int zgsz357;
        CREATE(sail_int)(&zgsz357);
        CONVERT_OF(sail_int, mach_int)(&zgsz357, zCAP_VALUE_NUM_BITS);
        sail_int zgsz358;
        CREATE(sail_int)(&zgsz358);
        CONVERT_OF(sail_int, mach_int)(&zgsz358, zCAP_MW);
        sail_int zgsz359;
        CREATE(sail_int)(&zgsz359);
        sub_int(&zgsz359, zgsz357, zgsz358);
        zgaz313 = CONVERT_OF(mach_int, sail_int)(zgsz359);
        KILL(sail_int)(&zgsz359);
        KILL(sail_int)(&zgsz358);
        KILL(sail_int)(&zgsz357);
    }
    {
        sail_int zgsz361;
        CREATE(sail_int)(&zgsz361);
        CONVERT_OF(sail_int, mach_int)(&zgsz361, zgaz313);
        sail_int zgsz362;
        CREATE(sail_int)(&zgsz362);
        CONVERT_OF(sail_int, mach_int)(&zgsz362, INT64_C(2));
        sail_int zgsz363;
        CREATE(sail_int)(&zgsz363);
        add_int(&zgsz363, zgsz361, zgsz362);
        zgsz360 = CONVERT_OF(mach_int, sail_int)(zgsz363);
        KILL(sail_int)(&zgsz363);
        KILL(sail_int)(&zgsz362);
        KILL(sail_int)(&zgsz361);
    }

    zCAP_MAX_EXPONENT = zgsz360;

let_end_44:;
}
static void kill_letbind_15(void) {}

static uint64_t sailgen_CapBoundsAddress(uint64_t);

static uint64_t sailgen_CapBoundsAddress(uint64_t zaddress) {
    __label__ cleanup_47, end_cleanup_48, end_function_46, end_block_exception_49;

    uint64_t zcbz37;
    uint64_t zgaz316;
    {
        uint64_t zgaz315;
        {
            int64_t zgaz314;
            {
                sail_int zgsz364;
                CREATE(sail_int)(&zgsz364);
                CONVERT_OF(sail_int, mach_int)(&zgsz364, zCAP_FLAGS_LO_BIT);
                sail_int zgsz365;
                CREATE(sail_int)(&zgsz365);
                CONVERT_OF(sail_int, mach_int)(&zgsz365, INT64_C(1));
                sail_int zgsz366;
                CREATE(sail_int)(&zgsz366);
                sub_int(&zgsz366, zgsz364, zgsz365);
                zgaz314 = CONVERT_OF(mach_int, sail_int)(zgsz366);
                KILL(sail_int)(&zgsz366);
                KILL(sail_int)(&zgsz365);
                KILL(sail_int)(&zgsz364);
            }
            {
                lbits zgsz367;
                CREATE(lbits)(&zgsz367);
                CONVERT_OF(lbits, fbits)(&zgsz367, zaddress, UINT64_C(64), true);
                sail_int zgsz368;
                CREATE(sail_int)(&zgsz368);
                CONVERT_OF(sail_int, mach_int)(&zgsz368, zgaz314);
                sail_int zgsz369;
                CREATE(sail_int)(&zgsz369);
                CONVERT_OF(sail_int, mach_int)(&zgsz369, INT64_C(0));
                lbits zgsz370;
                CREATE(lbits)(&zgsz370);
                vector_subrange_lbits(&zgsz370, zgsz367, zgsz368, zgsz369);
                zgaz315 = CONVERT_OF(fbits, lbits)(zgsz370, true);
                KILL(lbits)(&zgsz370);
                KILL(sail_int)(&zgsz369);
                KILL(sail_int)(&zgsz368);
                KILL(lbits)(&zgsz367);
            }
        }
        {
            lbits zgsz371;
            CREATE(lbits)(&zgsz371);
            CONVERT_OF(lbits, fbits)(&zgsz371, zgaz315, UINT64_C(56), true);
            sail_int zgsz372;
            CREATE(sail_int)(&zgsz372);
            CONVERT_OF(sail_int, mach_int)(&zgsz372, zCAP_VALUE_NUM_BITS);
            lbits zgsz373;
            CREATE(lbits)(&zgsz373);
            sign_extend(&zgsz373, zgsz371, zgsz372);
            zgaz316 = CONVERT_OF(fbits, lbits)(zgsz373, true);
            KILL(lbits)(&zgsz373);
            KILL(sail_int)(&zgsz372);
            KILL(lbits)(&zgsz371);
        }
    }
    zcbz37 = zgaz316;
    goto cleanup_47;
    /* unreachable after return */

    goto end_cleanup_48;
cleanup_47:;

    goto end_function_46;
end_cleanup_48:;
end_function_46:;
    return zcbz37;
end_block_exception_49:;

    return UINT64_C(0xdeadc0de);
}

static int64_t zCAP_BASE_MANTISSA_LO_BIT;
static void create_letbind_16(void) {

    int64_t zgsz374;
    zgsz374 = INT64_C(67);
    zCAP_BASE_MANTISSA_LO_BIT = zgsz374;

let_end_50:;
}
static void kill_letbind_16(void) {}

static uint64_t sailgen_CapGetBottom(lbits);

static uint64_t sailgen_CapGetBottom(lbits zc) {
    __label__ end_function_52, end_block_exception_57;

    uint64_t zcbz38;
    bool zgaz317;
    zgaz317 = sailgen_CapIsInternalExponent(zc);
    if (zgaz317) {
        __label__ cleanup_55, end_cleanup_56;

        uint64_t zgaz319;
        {
            uint64_t zgaz318;
            {
                sail_int zgsz378;
                CREATE(sail_int)(&zgsz378);
                CONVERT_OF(sail_int, mach_int)(&zgsz378, zCAP_BASE_HI_BIT);
                sail_int zgsz379;
                CREATE(sail_int)(&zgsz379);
                CONVERT_OF(sail_int, mach_int)(&zgsz379, zCAP_BASE_MANTISSA_LO_BIT);
                lbits zgsz380;
                CREATE(lbits)(&zgsz380);
                vector_subrange_lbits(&zgsz380, zc, zgsz378, zgsz379);
                zgaz318 = CONVERT_OF(fbits, lbits)(zgsz380, true);
                KILL(lbits)(&zgsz380);
                KILL(sail_int)(&zgsz379);
                KILL(sail_int)(&zgsz378);
            }
            {
                lbits zgsz381;
                CREATE(lbits)(&zgsz381);
                CONVERT_OF(lbits, fbits)(&zgsz381, zgaz318, UINT64_C(13), true);
                lbits zgsz382;
                CREATE(lbits)(&zgsz382);
                CONVERT_OF(lbits, fbits)(&zgsz382, UINT64_C(0b000), UINT64_C(3), true);
                lbits zgsz383;
                CREATE(lbits)(&zgsz383);
                append(&zgsz383, zgsz381, zgsz382);
                zgaz319 = CONVERT_OF(fbits, lbits)(zgsz383, true);
                KILL(lbits)(&zgsz383);
                KILL(lbits)(&zgsz382);
                KILL(lbits)(&zgsz381);
            }
        }
        zcbz38 = zgaz319;
        goto cleanup_55;
        /* unreachable after return */

        goto end_cleanup_56;
    cleanup_55:;

        goto end_function_52;
    end_cleanup_56:;
    } else {
        __label__ cleanup_53, end_cleanup_54;

        uint64_t zgaz320;
        {
            sail_int zgsz375;
            CREATE(sail_int)(&zgsz375);
            CONVERT_OF(sail_int, mach_int)(&zgsz375, zCAP_BASE_HI_BIT);
            sail_int zgsz376;
            CREATE(sail_int)(&zgsz376);
            CONVERT_OF(sail_int, mach_int)(&zgsz376, zCAP_BASE_LO_BIT);
            lbits zgsz377;
            CREATE(lbits)(&zgsz377);
            vector_subrange_lbits(&zgsz377, zc, zgsz375, zgsz376);
            zgaz320 = CONVERT_OF(fbits, lbits)(zgsz377, true);
            KILL(lbits)(&zgsz377);
            KILL(sail_int)(&zgsz376);
            KILL(sail_int)(&zgsz375);
        }
        zcbz38 = zgaz320;
        goto cleanup_53;
        /* unreachable after return */

        goto end_cleanup_54;
    cleanup_53:;

        goto end_function_52;
    end_cleanup_54:;
    }

end_function_52:;
    return zcbz38;
end_block_exception_57:;

    return UINT64_C(0xdeadc0de);
}

static int64_t zCAP_LIMIT_HI_BIT;
static void create_letbind_17(void) {

    int64_t zgsz384;
    zgsz384 = INT64_C(93);
    zCAP_LIMIT_HI_BIT = zgsz384;

let_end_58:;
}
static void kill_letbind_17(void) {}

static int64_t zCAP_LIMIT_MANTISSA_LO_BIT;
static void create_letbind_18(void) {

    int64_t zgsz385;
    zgsz385 = INT64_C(83);
    zCAP_LIMIT_MANTISSA_LO_BIT = zgsz385;

let_end_59:;
}
static void kill_letbind_18(void) {}

static bool sailgen_zzz3zzz3CapUnsignedLessThan(lbits, lbits);

static bool sailgen_zzzz7NzzJzzK3z3CapUnsignedLessThan(uint64_t, uint64_t);

static bool sailgen_zzz3zzz3CapUnsignedLessThan(lbits za, lbits zb) {
    __label__ cleanup_62, end_cleanup_63, end_function_61, end_block_exception_64;

    bool zcbz39;
    bool zgaz323;
    {
        sail_int zgaz321;
        CREATE(sail_int)(&zgaz321);
        sail_unsigned(&zgaz321, za);
        sail_int zgaz322;
        CREATE(sail_int)(&zgaz322);
        sail_unsigned(&zgaz322, zb);
        zgaz323 = lt(zgaz321, zgaz322);
        KILL(sail_int)(&zgaz322);
        KILL(sail_int)(&zgaz321);
    }
    zcbz39 = zgaz323;
    goto cleanup_62;
    /* unreachable after return */

    goto end_cleanup_63;
cleanup_62:;

    goto end_function_61;
end_cleanup_63:;
end_function_61:;
    return zcbz39;
end_block_exception_64:;

    return false;
}

static bool sailgen_zzzz7NzzJzzK3z3CapUnsignedLessThan(uint64_t za, uint64_t zb) {
    __label__ cleanup_67, end_cleanup_68, end_function_66, end_block_exception_69;

    bool zcbz310;
    bool zgaz326;
    {
        int64_t zgaz324;
        zgaz324 = ((mach_int)za);
        int64_t zgaz325;
        zgaz325 = ((mach_int)zb);
        {
            sail_int zgsz387;
            CREATE(sail_int)(&zgsz387);
            CONVERT_OF(sail_int, mach_int)(&zgsz387, zgaz325);
            sail_int zgsz386;
            CREATE(sail_int)(&zgsz386);
            CONVERT_OF(sail_int, mach_int)(&zgsz386, zgaz324);
            zgaz326 = lt(zgsz386, zgsz387);
            KILL(sail_int)(&zgsz387);
            KILL(sail_int)(&zgsz386);
        }
    }
    zcbz310 = zgaz326;
    goto cleanup_67;
    /* unreachable after return */

    goto end_cleanup_68;
cleanup_67:;

    goto end_function_66;
end_cleanup_68:;
end_function_66:;
    return zcbz310;
end_block_exception_69:;

    return false;
}

static uint64_t sailgen_CapGetTop(lbits);

static uint64_t sailgen_CapGetTop(lbits zc) {
    __label__ cleanup_72, end_cleanup_73, end_function_71, end_block_exception_74;

    uint64_t zcbz311;
    uint64_t zlmsb;
    zlmsb = UINT64_C(0b00);
    uint64_t zlcarry;
    zlcarry = UINT64_C(0b00);
    uint64_t zb;
    zb = sailgen_CapGetBottom(zc);
    uint64_t zt;
    {
        int64_t zgaz344;
        {
            int64_t zgaz343;
            {
                sail_int zgsz388;
                CREATE(sail_int)(&zgsz388);
                CONVERT_OF(sail_int, mach_int)(&zgsz388, INT64_C(79));
                sail_int zgsz389;
                CREATE(sail_int)(&zgsz389);
                CONVERT_OF(sail_int, mach_int)(&zgsz389, INT64_C(64));
                sail_int zgsz390;
                CREATE(sail_int)(&zgsz390);
                sub_int(&zgsz390, zgsz388, zgsz389);
                zgaz343 = CONVERT_OF(mach_int, sail_int)(zgsz390);
                KILL(sail_int)(&zgsz390);
                KILL(sail_int)(&zgsz389);
                KILL(sail_int)(&zgsz388);
            }
            {
                sail_int zgsz391;
                CREATE(sail_int)(&zgsz391);
                CONVERT_OF(sail_int, mach_int)(&zgsz391, zgaz343);
                sail_int zgsz392;
                CREATE(sail_int)(&zgsz392);
                CONVERT_OF(sail_int, mach_int)(&zgsz392, INT64_C(1));
                sail_int zgsz393;
                CREATE(sail_int)(&zgsz393);
                add_int(&zgsz393, zgsz391, zgsz392);
                zgaz344 = CONVERT_OF(mach_int, sail_int)(zgsz393);
                KILL(sail_int)(&zgsz393);
                KILL(sail_int)(&zgsz392);
                KILL(sail_int)(&zgsz391);
            }
        }
        {
            sail_int zgsz394;
            CREATE(sail_int)(&zgsz394);
            CONVERT_OF(sail_int, mach_int)(&zgsz394, zgaz344);
            lbits zgsz395;
            CREATE(lbits)(&zgsz395);
            UNDEFINED(lbits)(&zgsz395, zgsz394);
            zt = CONVERT_OF(fbits, lbits)(zgsz395, true);
            KILL(lbits)(&zgsz395);
            KILL(sail_int)(&zgsz394);
        }
    }
    {
        bool zgaz327;
        zgaz327 = sailgen_CapIsInternalExponent(zc);
        unit zgsz3141;
        if (zgaz327) {
            {
                zlmsb = UINT64_C(0b01);
                unit zgsz3148;
                zgsz3148 = UNIT;
            }
            uint64_t zgaz329;
            {
                uint64_t zgaz328;
                {
                    sail_int zgsz3149;
                    CREATE(sail_int)(&zgsz3149);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3149, zCAP_LIMIT_HI_BIT);
                    sail_int zgsz3150;
                    CREATE(sail_int)(&zgsz3150);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3150, zCAP_LIMIT_MANTISSA_LO_BIT);
                    lbits zgsz3151;
                    CREATE(lbits)(&zgsz3151);
                    vector_subrange_lbits(&zgsz3151, zc, zgsz3149, zgsz3150);
                    zgaz328 = CONVERT_OF(fbits, lbits)(zgsz3151, true);
                    KILL(lbits)(&zgsz3151);
                    KILL(sail_int)(&zgsz3150);
                    KILL(sail_int)(&zgsz3149);
                }
                {
                    lbits zgsz3152;
                    CREATE(lbits)(&zgsz3152);
                    CONVERT_OF(lbits, fbits)(&zgsz3152, UINT64_C(0b00), UINT64_C(2), true);
                    lbits zgsz3153;
                    CREATE(lbits)(&zgsz3153);
                    CONVERT_OF(lbits, fbits)(&zgsz3153, zgaz328, UINT64_C(11), true);
                    lbits zgsz3154;
                    CREATE(lbits)(&zgsz3154);
                    append(&zgsz3154, zgsz3152, zgsz3153);
                    zgaz329 = CONVERT_OF(fbits, lbits)(zgsz3154, true);
                    KILL(lbits)(&zgsz3154);
                    KILL(lbits)(&zgsz3153);
                    KILL(lbits)(&zgsz3152);
                }
            }
            {
                lbits zgsz3155;
                CREATE(lbits)(&zgsz3155);
                CONVERT_OF(lbits, fbits)(&zgsz3155, zgaz329, UINT64_C(13), true);
                lbits zgsz3156;
                CREATE(lbits)(&zgsz3156);
                CONVERT_OF(lbits, fbits)(&zgsz3156, UINT64_C(0b000), UINT64_C(3), true);
                lbits zgsz3157;
                CREATE(lbits)(&zgsz3157);
                append(&zgsz3157, zgsz3155, zgsz3156);
                zt = CONVERT_OF(fbits, lbits)(zgsz3157, true);
                KILL(lbits)(&zgsz3157);
                KILL(lbits)(&zgsz3156);
                KILL(lbits)(&zgsz3155);
            }
            zgsz3141 = UNIT;

        } else {
            uint64_t zgaz330;
            {
                sail_int zgsz3142;
                CREATE(sail_int)(&zgsz3142);
                CONVERT_OF(sail_int, mach_int)(&zgsz3142, zCAP_LIMIT_HI_BIT);
                sail_int zgsz3143;
                CREATE(sail_int)(&zgsz3143);
                CONVERT_OF(sail_int, mach_int)(&zgsz3143, zCAP_LIMIT_LO_BIT);
                lbits zgsz3144;
                CREATE(lbits)(&zgsz3144);
                vector_subrange_lbits(&zgsz3144, zc, zgsz3142, zgsz3143);
                zgaz330 = CONVERT_OF(fbits, lbits)(zgsz3144, true);
                KILL(lbits)(&zgsz3144);
                KILL(sail_int)(&zgsz3143);
                KILL(sail_int)(&zgsz3142);
            }
            {
                lbits zgsz3145;
                CREATE(lbits)(&zgsz3145);
                CONVERT_OF(lbits, fbits)(&zgsz3145, UINT64_C(0b00), UINT64_C(2), true);
                lbits zgsz3146;
                CREATE(lbits)(&zgsz3146);
                CONVERT_OF(lbits, fbits)(&zgsz3146, zgaz330, UINT64_C(14), true);
                lbits zgsz3147;
                CREATE(lbits)(&zgsz3147);
                append(&zgsz3147, zgsz3145, zgsz3146);
                zt = CONVERT_OF(fbits, lbits)(zgsz3147, true);
                KILL(lbits)(&zgsz3147);
                KILL(lbits)(&zgsz3146);
                KILL(lbits)(&zgsz3145);
            }
            zgsz3141 = UNIT;
        }
    }
    {
        bool zgaz335;
        {
            uint64_t zgaz333;
            {
                int64_t zgaz331;
                {
                    sail_int zgsz396;
                    CREATE(sail_int)(&zgsz396);
                    CONVERT_OF(sail_int, mach_int)(&zgsz396, zCAP_MW);
                    sail_int zgsz397;
                    CREATE(sail_int)(&zgsz397);
                    CONVERT_OF(sail_int, mach_int)(&zgsz397, INT64_C(3));
                    sail_int zgsz398;
                    CREATE(sail_int)(&zgsz398);
                    sub_int(&zgsz398, zgsz396, zgsz397);
                    zgaz331 = CONVERT_OF(mach_int, sail_int)(zgsz398);
                    KILL(sail_int)(&zgsz398);
                    KILL(sail_int)(&zgsz397);
                    KILL(sail_int)(&zgsz396);
                }
                {
                    lbits zgsz399;
                    CREATE(lbits)(&zgsz399);
                    CONVERT_OF(lbits, fbits)(&zgsz399, zt, UINT64_C(16), true);
                    sail_int zgsz3100;
                    CREATE(sail_int)(&zgsz3100);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3100, zgaz331);
                    sail_int zgsz3101;
                    CREATE(sail_int)(&zgsz3101);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3101, INT64_C(0));
                    lbits zgsz3102;
                    CREATE(lbits)(&zgsz3102);
                    vector_subrange_lbits(&zgsz3102, zgsz399, zgsz3100, zgsz3101);
                    zgaz333 = CONVERT_OF(fbits, lbits)(zgsz3102, true);
                    KILL(lbits)(&zgsz3102);
                    KILL(sail_int)(&zgsz3101);
                    KILL(sail_int)(&zgsz3100);
                    KILL(lbits)(&zgsz399);
                }
            }
            uint64_t zgaz334;
            {
                int64_t zgaz332;
                {
                    sail_int zgsz3103;
                    CREATE(sail_int)(&zgsz3103);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3103, zCAP_MW);
                    sail_int zgsz3104;
                    CREATE(sail_int)(&zgsz3104);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3104, INT64_C(3));
                    sail_int zgsz3105;
                    CREATE(sail_int)(&zgsz3105);
                    sub_int(&zgsz3105, zgsz3103, zgsz3104);
                    zgaz332 = CONVERT_OF(mach_int, sail_int)(zgsz3105);
                    KILL(sail_int)(&zgsz3105);
                    KILL(sail_int)(&zgsz3104);
                    KILL(sail_int)(&zgsz3103);
                }
                {
                    lbits zgsz3106;
                    CREATE(lbits)(&zgsz3106);
                    CONVERT_OF(lbits, fbits)(&zgsz3106, zb, UINT64_C(16), true);
                    sail_int zgsz3107;
                    CREATE(sail_int)(&zgsz3107);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3107, zgaz332);
                    sail_int zgsz3108;
                    CREATE(sail_int)(&zgsz3108);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3108, INT64_C(0));
                    lbits zgsz3109;
                    CREATE(lbits)(&zgsz3109);
                    vector_subrange_lbits(&zgsz3109, zgsz3106, zgsz3107, zgsz3108);
                    zgaz334 = CONVERT_OF(fbits, lbits)(zgsz3109, true);
                    KILL(lbits)(&zgsz3109);
                    KILL(sail_int)(&zgsz3108);
                    KILL(sail_int)(&zgsz3107);
                    KILL(lbits)(&zgsz3106);
                }
            }
            {
                lbits zgsz3111;
                CREATE(lbits)(&zgsz3111);
                CONVERT_OF(lbits, fbits)(&zgsz3111, zgaz334, UINT64_C(14), true);
                lbits zgsz3110;
                CREATE(lbits)(&zgsz3110);
                CONVERT_OF(lbits, fbits)(&zgsz3110, zgaz333, UINT64_C(14), true);
                zgaz335 = sailgen_zzz3zzz3CapUnsignedLessThan(zgsz3110, zgsz3111);
                KILL(lbits)(&zgsz3111);
                KILL(lbits)(&zgsz3110);
            }
        }
        unit zgsz3140;
        if (zgaz335) {
            zlcarry = UINT64_C(0b01);
            zgsz3140 = UNIT;
        } else {
            zgsz3140 = UNIT;
        }
    }
    {
        int64_t zgaz340;
        {
            sail_int zgsz3112;
            CREATE(sail_int)(&zgsz3112);
            CONVERT_OF(sail_int, mach_int)(&zgsz3112, zCAP_MW);
            sail_int zgsz3113;
            CREATE(sail_int)(&zgsz3113);
            CONVERT_OF(sail_int, mach_int)(&zgsz3113, INT64_C(1));
            sail_int zgsz3114;
            CREATE(sail_int)(&zgsz3114);
            sub_int(&zgsz3114, zgsz3112, zgsz3113);
            zgaz340 = CONVERT_OF(mach_int, sail_int)(zgsz3114);
            KILL(sail_int)(&zgsz3114);
            KILL(sail_int)(&zgsz3113);
            KILL(sail_int)(&zgsz3112);
        }
        int64_t zgaz341;
        {
            sail_int zgsz3115;
            CREATE(sail_int)(&zgsz3115);
            CONVERT_OF(sail_int, mach_int)(&zgsz3115, zCAP_MW);
            sail_int zgsz3116;
            CREATE(sail_int)(&zgsz3116);
            CONVERT_OF(sail_int, mach_int)(&zgsz3116, INT64_C(2));
            sail_int zgsz3117;
            CREATE(sail_int)(&zgsz3117);
            sub_int(&zgsz3117, zgsz3115, zgsz3116);
            zgaz341 = CONVERT_OF(mach_int, sail_int)(zgsz3117);
            KILL(sail_int)(&zgsz3117);
            KILL(sail_int)(&zgsz3116);
            KILL(sail_int)(&zgsz3115);
        }
        uint64_t zgaz342;
        {
            uint64_t zgaz339;
            {
                uint64_t zgaz338;
                {
                    int64_t zgaz336;
                    {
                        sail_int zgsz3118;
                        CREATE(sail_int)(&zgsz3118);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3118, zCAP_MW);
                        sail_int zgsz3119;
                        CREATE(sail_int)(&zgsz3119);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3119, INT64_C(1));
                        sail_int zgsz3120;
                        CREATE(sail_int)(&zgsz3120);
                        sub_int(&zgsz3120, zgsz3118, zgsz3119);
                        zgaz336 = CONVERT_OF(mach_int, sail_int)(zgsz3120);
                        KILL(sail_int)(&zgsz3120);
                        KILL(sail_int)(&zgsz3119);
                        KILL(sail_int)(&zgsz3118);
                    }
                    int64_t zgaz337;
                    {
                        sail_int zgsz3121;
                        CREATE(sail_int)(&zgsz3121);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3121, zCAP_MW);
                        sail_int zgsz3122;
                        CREATE(sail_int)(&zgsz3122);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3122, INT64_C(2));
                        sail_int zgsz3123;
                        CREATE(sail_int)(&zgsz3123);
                        sub_int(&zgsz3123, zgsz3121, zgsz3122);
                        zgaz337 = CONVERT_OF(mach_int, sail_int)(zgsz3123);
                        KILL(sail_int)(&zgsz3123);
                        KILL(sail_int)(&zgsz3122);
                        KILL(sail_int)(&zgsz3121);
                    }
                    {
                        lbits zgsz3124;
                        CREATE(lbits)(&zgsz3124);
                        CONVERT_OF(lbits, fbits)(&zgsz3124, zb, UINT64_C(16), true);
                        sail_int zgsz3125;
                        CREATE(sail_int)(&zgsz3125);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3125, zgaz336);
                        sail_int zgsz3126;
                        CREATE(sail_int)(&zgsz3126);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3126, zgaz337);
                        lbits zgsz3127;
                        CREATE(lbits)(&zgsz3127);
                        vector_subrange_lbits(&zgsz3127, zgsz3124, zgsz3125, zgsz3126);
                        zgaz338 = CONVERT_OF(fbits, lbits)(zgsz3127, true);
                        KILL(lbits)(&zgsz3127);
                        KILL(sail_int)(&zgsz3126);
                        KILL(sail_int)(&zgsz3125);
                        KILL(lbits)(&zgsz3124);
                    }
                }
                {
                    lbits zgsz3128;
                    CREATE(lbits)(&zgsz3128);
                    CONVERT_OF(lbits, fbits)(&zgsz3128, zgaz338, UINT64_C(2), true);
                    lbits zgsz3129;
                    CREATE(lbits)(&zgsz3129);
                    CONVERT_OF(lbits, fbits)(&zgsz3129, zlmsb, UINT64_C(2), true);
                    lbits zgsz3130;
                    CREATE(lbits)(&zgsz3130);
                    add_bits(&zgsz3130, zgsz3128, zgsz3129);
                    zgaz339 = CONVERT_OF(fbits, lbits)(zgsz3130, true);
                    KILL(lbits)(&zgsz3130);
                    KILL(lbits)(&zgsz3129);
                    KILL(lbits)(&zgsz3128);
                }
            }
            {
                lbits zgsz3131;
                CREATE(lbits)(&zgsz3131);
                CONVERT_OF(lbits, fbits)(&zgsz3131, zgaz339, UINT64_C(2), true);
                lbits zgsz3132;
                CREATE(lbits)(&zgsz3132);
                CONVERT_OF(lbits, fbits)(&zgsz3132, zlcarry, UINT64_C(2), true);
                lbits zgsz3133;
                CREATE(lbits)(&zgsz3133);
                add_bits(&zgsz3133, zgsz3131, zgsz3132);
                zgaz342 = CONVERT_OF(fbits, lbits)(zgsz3133, true);
                KILL(lbits)(&zgsz3133);
                KILL(lbits)(&zgsz3132);
                KILL(lbits)(&zgsz3131);
            }
        }
        {
            lbits zgsz3134;
            CREATE(lbits)(&zgsz3134);
            CONVERT_OF(lbits, fbits)(&zgsz3134, zt, UINT64_C(16), true);
            sail_int zgsz3135;
            CREATE(sail_int)(&zgsz3135);
            CONVERT_OF(sail_int, mach_int)(&zgsz3135, zgaz340);
            sail_int zgsz3136;
            CREATE(sail_int)(&zgsz3136);
            CONVERT_OF(sail_int, mach_int)(&zgsz3136, zgaz341);
            lbits zgsz3137;
            CREATE(lbits)(&zgsz3137);
            CONVERT_OF(lbits, fbits)(&zgsz3137, zgaz342, UINT64_C(2), true);
            lbits zgsz3138;
            CREATE(lbits)(&zgsz3138);
            vector_update_subrange_lbits(&zgsz3138, zgsz3134, zgsz3135, zgsz3136, zgsz3137);
            zt = CONVERT_OF(fbits, lbits)(zgsz3138, true);
            KILL(lbits)(&zgsz3138);
            KILL(lbits)(&zgsz3137);
            KILL(sail_int)(&zgsz3136);
            KILL(sail_int)(&zgsz3135);
            KILL(lbits)(&zgsz3134);
        }
        unit zgsz3139;
        zgsz3139 = UNIT;
    }
    zcbz311 = zt;
    goto cleanup_72;
    /* unreachable after return */

    goto end_cleanup_73;
cleanup_72:;

    goto end_function_71;
end_cleanup_73:;
end_function_71:;
    return zcbz311;
end_block_exception_74:;

    return UINT64_C(0xdeadc0de);
}

static bool sailgen_CapIsExponentOutOfRange(lbits);

static bool sailgen_CapIsExponentOutOfRange(lbits zc) {
    __label__ case_77, finish_match_76, end_function_78, end_block_exception_81;

    bool zcbz312;
    int64_t zgaz345;
    zgaz345 = sailgen_CapGetExponent(zc);
    bool zgsz3158;
    {
        __label__ cleanup_79, end_cleanup_80;

        int64_t zexp;
        zexp = zgaz345;
        bool zgaz347;
        {
            bool zgaz346;
            {
                sail_int zgsz3160;
                CREATE(sail_int)(&zgsz3160);
                CONVERT_OF(sail_int, mach_int)(&zgsz3160, zCAP_MAX_EXPONENT);
                sail_int zgsz3159;
                CREATE(sail_int)(&zgsz3159);
                CONVERT_OF(sail_int, mach_int)(&zgsz3159, zexp);
                zgaz346 = gt(zgsz3159, zgsz3160);
                KILL(sail_int)(&zgsz3160);
                KILL(sail_int)(&zgsz3159);
            }
            bool zgsz3161;
            if (zgaz346) {
                {
                    sail_int zgsz3163;
                    CREATE(sail_int)(&zgsz3163);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3163, zCAP_MAX_ENCODEABLE_EXPONENT);
                    sail_int zgsz3162;
                    CREATE(sail_int)(&zgsz3162);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3162, zexp);
                    zgsz3161 = lt(zgsz3162, zgsz3163);
                    KILL(sail_int)(&zgsz3163);
                    KILL(sail_int)(&zgsz3162);
                }
            } else {
                zgsz3161 = false;
            }
            zgaz347 = zgsz3161;
        }
        zcbz312 = zgaz347;
        goto cleanup_79;
        /* unreachable after return */

        goto finish_match_76;
        goto end_cleanup_80;
    cleanup_79:;

        goto end_function_78;
    end_cleanup_80:;
    }
case_77:;
    sail_match_failure("CapIsExponentOutOfRange");
finish_match_76:;
    zcbz312 = zgsz3158;

end_function_78:;
    return zcbz312;
end_block_exception_81:;

    return false;
}

static bool sailgen_zzzz7NzzJzzK2z3CapUnsignedGreaterThan(uint64_t, uint64_t);

static bool sailgen_zzzz7NzzJzzK2z3CapUnsignedGreaterThan(uint64_t za, uint64_t zb) {
    __label__ cleanup_84, end_cleanup_85, end_function_83, end_block_exception_86;

    bool zcbz313;
    bool zgaz350;
    {
        int64_t zgaz348;
        zgaz348 = ((mach_int)za);
        int64_t zgaz349;
        zgaz349 = ((mach_int)zb);
        {
            sail_int zgsz3166;
            CREATE(sail_int)(&zgsz3166);
            CONVERT_OF(sail_int, mach_int)(&zgsz3166, zgaz349);
            sail_int zgsz3165;
            CREATE(sail_int)(&zgsz3165);
            CONVERT_OF(sail_int, mach_int)(&zgsz3165, zgaz348);
            zgaz350 = gt(zgsz3165, zgsz3166);
            KILL(sail_int)(&zgsz3166);
            KILL(sail_int)(&zgsz3165);
        }
    }
    zcbz313 = zgaz350;
    goto cleanup_84;
    /* unreachable after return */

    goto end_cleanup_85;
cleanup_84:;

    goto end_function_83;
end_cleanup_85:;
end_function_83:;
    return zcbz313;
end_block_exception_86:;

    return false;
}

// struct tuple_(%bv, %bv, %bool)
struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 {
    lbits ztup0;
    lbits ztup1;
    bool ztup2;
};

static void COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9* rop,
                                                    const struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 op) {
    COPY(lbits)(&rop->ztup0, op.ztup0);
    COPY(lbits)(&rop->ztup1, op.ztup1);
    rop->ztup2 = op.ztup2;
}

static void CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9* op) {
    CREATE(lbits)(&op->ztup0);
    CREATE(lbits)(&op->ztup1);
}

static void RECREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9* op) {
    RECREATE(lbits)(&op->ztup0);
    RECREATE(lbits)(&op->ztup1);
}

static void KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9* op) {
    KILL(lbits)(&op->ztup0);
    KILL(lbits)(&op->ztup1);
}

static bool EQUAL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 op1,
                                                     struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 op2) {
    return EQUAL(lbits)(op1.ztup0, op2.ztup0) && EQUAL(lbits)(op1.ztup1, op2.ztup1) &&
           EQUAL(bool)(op1.ztup2, op2.ztup2);
}

static void sailgen_CapGetBounds(struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9* rop, lbits);

static void sailgen_CapGetBounds(struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9* zcbz314, lbits zc) {
    __label__ case_89, finish_match_88, end_function_100, end_block_exception_107, end_function_128;

    int64_t zgaz351;
    zgaz351 = sailgen_CapGetExponent(zc);
    struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3167;
    CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3167);
    {
        __label__ case_91, finish_match_90;

        int64_t zexp;
        zexp = zgaz351;
        {
            bool zgaz352;
            {
                sail_int zgsz3169;
                CREATE(sail_int)(&zgsz3169);
                CONVERT_OF(sail_int, mach_int)(&zgsz3169, zCAP_MAX_ENCODEABLE_EXPONENT);
                sail_int zgsz3168;
                CREATE(sail_int)(&zgsz3168);
                CONVERT_OF(sail_int, mach_int)(&zgsz3168, zexp);
                zgaz352 = eq_int(zgsz3168, zgsz3169);
                KILL(sail_int)(&zgsz3169);
                KILL(sail_int)(&zgsz3168);
            }
            unit zgsz3176;
            if (zgaz352) {
                __label__ cleanup_105, end_cleanup_106;

                struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3177;
                CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3177);
                COPY(lbits)(&((&zgsz3177)->ztup0), zCAP_BOUND_MIN);
                COPY(lbits)(&((&zgsz3177)->ztup1), zCAP_BOUND_MAX);
                zgsz3177.ztup2 = true;
                COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)((*(&zcbz314)), zgsz3177);
                goto cleanup_105;
                /* unreachable after return */
                goto end_cleanup_106;
            cleanup_105:;

                KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3167);

                KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3177);
                goto end_function_100;
            end_cleanup_106:;
            } else {
                zgsz3176 = UNIT;
            }
        }
        {
            bool zgaz354;
            {
                bool zgaz353;
                {
                    sail_int zgsz3171;
                    CREATE(sail_int)(&zgsz3171);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3171, zCAP_MAX_ENCODEABLE_EXPONENT);
                    sail_int zgsz3170;
                    CREATE(sail_int)(&zgsz3170);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3170, zexp);
                    zgaz353 = eq_int(zgsz3170, zgsz3171);
                    KILL(sail_int)(&zgsz3171);
                    KILL(sail_int)(&zgsz3170);
                }
                zgaz354 = not(zgaz353);
            }
            unit zgsz3175;
            zgsz3175 = sail_assert(zgaz354, "v8_base.sail 1714:4 - 1716:5");
        }
        {
            bool zgaz355;
            zgaz355 = sailgen_CapIsExponentOutOfRange(zc);
            unit zgsz3173;
            if (zgaz355) {
                __label__ cleanup_103, end_cleanup_104;

                struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3174;
                CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3174);
                COPY(lbits)(&((&zgsz3174)->ztup0), zCAP_BOUND_MIN);
                COPY(lbits)(&((&zgsz3174)->ztup1), zCAP_BOUND_MAX);
                zgsz3174.ztup2 = false;
                COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)((*(&zcbz314)), zgsz3174);
                goto cleanup_103;
                /* unreachable after return */
                goto end_cleanup_104;
            cleanup_103:;

                KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3167);

                KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3174);
                goto end_function_100;
            end_cleanup_104:;
            } else {
                zgsz3173 = UNIT;
            }
        }
        {
            bool zgaz357;
            {
                bool zgaz356;
                zgaz356 = sailgen_CapIsExponentOutOfRange(zc);
                zgaz357 = not(zgaz356);
            }
            unit zgsz3172;
            zgsz3172 = sail_assert(zgaz357, "v8_base.sail 1717:4 - 1719:5");
        }
        lbits zbase;
        CREATE(lbits)(&zbase);
        {
            sail_int zgsz3178;
            CREATE(sail_int)(&zgsz3178);
            CONVERT_OF(sail_int, mach_int)(&zgsz3178, INT64_C(66));
            UNDEFINED(lbits)(&zbase, zgsz3178);
            KILL(sail_int)(&zgsz3178);
        }
        lbits zlimit;
        CREATE(lbits)(&zlimit);
        {
            sail_int zgsz3179;
            CREATE(sail_int)(&zgsz3179);
            CONVERT_OF(sail_int, mach_int)(&zgsz3179, INT64_C(66));
            UNDEFINED(lbits)(&zlimit, zgsz3179);
            KILL(sail_int)(&zgsz3179);
        }
        uint64_t zbottom;
        zbottom = sailgen_CapGetBottom(zc);
        uint64_t ztop;
        ztop = sailgen_CapGetTop(zc);
        {
            sbits zgaz358;
            {
                sail_int zgsz3180;
                CREATE(sail_int)(&zgsz3180);
                CONVERT_OF(sail_int, mach_int)(&zgsz3180, zexp);
                lbits zgsz3181;
                CREATE(lbits)(&zgsz3181);
                sailgen_zzz3zzz3Zeros(&zgsz3181, zgsz3180);
                zgaz358 = CONVERT_OF(sbits, lbits)(zgsz3181, true);
                KILL(lbits)(&zgsz3181);
                KILL(sail_int)(&zgsz3180);
            }
            {
                lbits zgsz3185;
                CREATE(lbits)(&zgsz3185);
                CONVERT_OF(lbits, sbits)(&zgsz3185, zgaz358, true);
                sail_int zgsz3184;
                CREATE(sail_int)(&zgsz3184);
                CONVERT_OF(sail_int, mach_int)(&zgsz3184, INT64_C(0));
                sail_int zgsz3183;
                CREATE(sail_int)(&zgsz3183);
                CONVERT_OF(sail_int, mach_int)(&zgsz3183, zexp);
                sail_int zgsz3182;
                CREATE(sail_int)(&zgsz3182);
                CONVERT_OF(sail_int, mach_int)(&zgsz3182, INT64_C(66));
                set_slice(&zbase, zgsz3182, zgsz3183, zbase, zgsz3184, zgsz3185);
                KILL(lbits)(&zgsz3185);
                KILL(sail_int)(&zgsz3184);
                KILL(sail_int)(&zgsz3183);
                KILL(sail_int)(&zgsz3182);
            }
            unit zgsz3229;
            zgsz3229 = UNIT;
        }
        {
            sbits zgaz359;
            {
                sail_int zgsz3186;
                CREATE(sail_int)(&zgsz3186);
                CONVERT_OF(sail_int, mach_int)(&zgsz3186, zexp);
                lbits zgsz3187;
                CREATE(lbits)(&zgsz3187);
                sailgen_zzz3zzz3Zeros(&zgsz3187, zgsz3186);
                zgaz359 = CONVERT_OF(sbits, lbits)(zgsz3187, true);
                KILL(lbits)(&zgsz3187);
                KILL(sail_int)(&zgsz3186);
            }
            {
                lbits zgsz3191;
                CREATE(lbits)(&zgsz3191);
                CONVERT_OF(lbits, sbits)(&zgsz3191, zgaz359, true);
                sail_int zgsz3190;
                CREATE(sail_int)(&zgsz3190);
                CONVERT_OF(sail_int, mach_int)(&zgsz3190, INT64_C(0));
                sail_int zgsz3189;
                CREATE(sail_int)(&zgsz3189);
                CONVERT_OF(sail_int, mach_int)(&zgsz3189, zexp);
                sail_int zgsz3188;
                CREATE(sail_int)(&zgsz3188);
                CONVERT_OF(sail_int, mach_int)(&zgsz3188, INT64_C(66));
                set_slice(&zlimit, zgsz3188, zgsz3189, zlimit, zgsz3190, zgsz3191);
                KILL(lbits)(&zgsz3191);
                KILL(sail_int)(&zgsz3190);
                KILL(sail_int)(&zgsz3189);
                KILL(sail_int)(&zgsz3188);
            }
            unit zgsz3228;
            zgsz3228 = UNIT;
        }
        {
            bool zgaz365;
            {
                bool zgaz364;
                {
                    int64_t zgaz360;
                    {
                        sail_int zgsz3192;
                        CREATE(sail_int)(&zgsz3192);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3192, zexp);
                        sail_int zgsz3193;
                        CREATE(sail_int)(&zgsz3193);
                        sailgen_zzz3zzz3__id(&zgsz3193, zgsz3192);
                        zgaz360 = CONVERT_OF(mach_int, sail_int)(zgsz3193);
                        KILL(sail_int)(&zgsz3193);
                        KILL(sail_int)(&zgsz3192);
                    }
                    {
                        sail_int zgsz3195;
                        CREATE(sail_int)(&zgsz3195);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3195, zgaz360);
                        sail_int zgsz3194;
                        CREATE(sail_int)(&zgsz3194);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3194, INT64_C(0));
                        zgaz364 = lteq(zgsz3194, zgsz3195);
                        KILL(sail_int)(&zgsz3195);
                        KILL(sail_int)(&zgsz3194);
                    }
                }
                bool zgsz3204;
                if (zgaz364) {
                    int64_t zgaz363;
                    {
                        int64_t zgaz362;
                        {
                            int64_t zgaz361;
                            {
                                sail_int zgsz3196;
                                CREATE(sail_int)(&zgsz3196);
                                CONVERT_OF(sail_int, mach_int)(&zgsz3196, zexp);
                                sail_int zgsz3197;
                                CREATE(sail_int)(&zgsz3197);
                                sailgen_zzz3zzz3__id(&zgsz3197, zgsz3196);
                                zgaz361 = CONVERT_OF(mach_int, sail_int)(zgsz3197);
                                KILL(sail_int)(&zgsz3197);
                                KILL(sail_int)(&zgsz3196);
                            }
                            {
                                sail_int zgsz3198;
                                CREATE(sail_int)(&zgsz3198);
                                CONVERT_OF(sail_int, mach_int)(&zgsz3198, zgaz361);
                                sail_int zgsz3199;
                                CREATE(sail_int)(&zgsz3199);
                                CONVERT_OF(sail_int, mach_int)(&zgsz3199, INT64_C(16));
                                sail_int zgsz3200;
                                CREATE(sail_int)(&zgsz3200);
                                add_int(&zgsz3200, zgsz3198, zgsz3199);
                                zgaz362 = CONVERT_OF(mach_int, sail_int)(zgsz3200);
                                KILL(sail_int)(&zgsz3200);
                                KILL(sail_int)(&zgsz3199);
                                KILL(sail_int)(&zgsz3198);
                            }
                        }
                        {
                            sail_int zgsz3201;
                            CREATE(sail_int)(&zgsz3201);
                            CONVERT_OF(sail_int, mach_int)(&zgsz3201, zgaz362);
                            sail_int zgsz3202;
                            CREATE(sail_int)(&zgsz3202);
                            CONVERT_OF(sail_int, mach_int)(&zgsz3202, INT64_C(1));
                            sail_int zgsz3203;
                            CREATE(sail_int)(&zgsz3203);
                            sub_int(&zgsz3203, zgsz3201, zgsz3202);
                            zgaz363 = CONVERT_OF(mach_int, sail_int)(zgsz3203);
                            KILL(sail_int)(&zgsz3203);
                            KILL(sail_int)(&zgsz3202);
                            KILL(sail_int)(&zgsz3201);
                        }
                    }
                    {
                        sail_int zgsz3206;
                        CREATE(sail_int)(&zgsz3206);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3206, INT64_C(66));
                        sail_int zgsz3205;
                        CREATE(sail_int)(&zgsz3205);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3205, zgaz363);
                        zgsz3204 = lt(zgsz3205, zgsz3206);
                        KILL(sail_int)(&zgsz3206);
                        KILL(sail_int)(&zgsz3205);
                    }

                } else {
                    zgsz3204 = false;
                }
                zgaz365 = zgsz3204;
            }
            unit zgsz3227;
            zgsz3227 = sail_assert(zgaz365, "v8_base.sail 1726:57 - 1726:58");
        }
        {
            int64_t zgaz367;
            {
                int64_t zgaz366;
                {
                    sail_int zgsz3207;
                    CREATE(sail_int)(&zgsz3207);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3207, zexp);
                    sail_int zgsz3208;
                    CREATE(sail_int)(&zgsz3208);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3208, zCAP_MW);
                    sail_int zgsz3209;
                    CREATE(sail_int)(&zgsz3209);
                    add_int(&zgsz3209, zgsz3207, zgsz3208);
                    zgaz366 = CONVERT_OF(mach_int, sail_int)(zgsz3209);
                    KILL(sail_int)(&zgsz3209);
                    KILL(sail_int)(&zgsz3208);
                    KILL(sail_int)(&zgsz3207);
                }
                {
                    sail_int zgsz3210;
                    CREATE(sail_int)(&zgsz3210);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3210, zgaz366);
                    sail_int zgsz3211;
                    CREATE(sail_int)(&zgsz3211);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3211, INT64_C(1));
                    sail_int zgsz3212;
                    CREATE(sail_int)(&zgsz3212);
                    sub_int(&zgsz3212, zgsz3210, zgsz3211);
                    zgaz367 = CONVERT_OF(mach_int, sail_int)(zgsz3212);
                    KILL(sail_int)(&zgsz3212);
                    KILL(sail_int)(&zgsz3211);
                    KILL(sail_int)(&zgsz3210);
                }
            }
            {
                lbits zgsz3215;
                CREATE(lbits)(&zgsz3215);
                CONVERT_OF(lbits, fbits)(&zgsz3215, zbottom, UINT64_C(16), true);
                sail_int zgsz3214;
                CREATE(sail_int)(&zgsz3214);
                CONVERT_OF(sail_int, mach_int)(&zgsz3214, zexp);
                sail_int zgsz3213;
                CREATE(sail_int)(&zgsz3213);
                CONVERT_OF(sail_int, mach_int)(&zgsz3213, zgaz367);
                vector_update_subrange_lbits(&zbase, zbase, zgsz3213, zgsz3214, zgsz3215);
                KILL(lbits)(&zgsz3215);
                KILL(sail_int)(&zgsz3214);
                KILL(sail_int)(&zgsz3213);
            }
            unit zgsz3226;
            zgsz3226 = UNIT;
        }
        {
            int64_t zgaz369;
            {
                int64_t zgaz368;
                {
                    sail_int zgsz3216;
                    CREATE(sail_int)(&zgsz3216);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3216, zexp);
                    sail_int zgsz3217;
                    CREATE(sail_int)(&zgsz3217);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3217, zCAP_MW);
                    sail_int zgsz3218;
                    CREATE(sail_int)(&zgsz3218);
                    add_int(&zgsz3218, zgsz3216, zgsz3217);
                    zgaz368 = CONVERT_OF(mach_int, sail_int)(zgsz3218);
                    KILL(sail_int)(&zgsz3218);
                    KILL(sail_int)(&zgsz3217);
                    KILL(sail_int)(&zgsz3216);
                }
                {
                    sail_int zgsz3219;
                    CREATE(sail_int)(&zgsz3219);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3219, zgaz368);
                    sail_int zgsz3220;
                    CREATE(sail_int)(&zgsz3220);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3220, INT64_C(1));
                    sail_int zgsz3221;
                    CREATE(sail_int)(&zgsz3221);
                    sub_int(&zgsz3221, zgsz3219, zgsz3220);
                    zgaz369 = CONVERT_OF(mach_int, sail_int)(zgsz3221);
                    KILL(sail_int)(&zgsz3221);
                    KILL(sail_int)(&zgsz3220);
                    KILL(sail_int)(&zgsz3219);
                }
            }
            {
                lbits zgsz3224;
                CREATE(lbits)(&zgsz3224);
                CONVERT_OF(lbits, fbits)(&zgsz3224, ztop, UINT64_C(16), true);
                sail_int zgsz3223;
                CREATE(sail_int)(&zgsz3223);
                CONVERT_OF(sail_int, mach_int)(&zgsz3223, zexp);
                sail_int zgsz3222;
                CREATE(sail_int)(&zgsz3222);
                CONVERT_OF(sail_int, mach_int)(&zgsz3222, zgaz369);
                vector_update_subrange_lbits(&zlimit, zlimit, zgsz3222, zgsz3223, zgsz3224);
                KILL(lbits)(&zgsz3224);
                KILL(sail_int)(&zgsz3223);
                KILL(sail_int)(&zgsz3222);
            }
            unit zgsz3225;
            zgsz3225 = UNIT;
        }
        lbits za;
        CREATE(lbits)(&za);
        {
            uint64_t zgaz3106;
            {
                uint64_t zgaz3105;
                zgaz3105 = sailgen_CapGetValue(zc);
                zgaz3106 = sailgen_CapBoundsAddress(zgaz3105);
            }
            {
                lbits zgsz3231;
                CREATE(lbits)(&zgsz3231);
                CONVERT_OF(lbits, fbits)(&zgsz3231, zgaz3106, UINT64_C(64), true);
                lbits zgsz3230;
                CREATE(lbits)(&zgsz3230);
                CONVERT_OF(lbits, fbits)(&zgsz3230, UINT64_C(0b00), UINT64_C(2), true);
                append(&za, zgsz3230, zgsz3231);
                KILL(lbits)(&zgsz3231);
                KILL(lbits)(&zgsz3230);
            }
        }
        uint64_t zA3;
        {
            int64_t zgaz3103;
            {
                int64_t zgaz3101;
                {
                    sail_int zgsz3232;
                    CREATE(sail_int)(&zgsz3232);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3232, zexp);
                    sail_int zgsz3233;
                    CREATE(sail_int)(&zgsz3233);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3233, zCAP_MW);
                    sail_int zgsz3234;
                    CREATE(sail_int)(&zgsz3234);
                    add_int(&zgsz3234, zgsz3232, zgsz3233);
                    zgaz3101 = CONVERT_OF(mach_int, sail_int)(zgsz3234);
                    KILL(sail_int)(&zgsz3234);
                    KILL(sail_int)(&zgsz3233);
                    KILL(sail_int)(&zgsz3232);
                }
                {
                    sail_int zgsz3235;
                    CREATE(sail_int)(&zgsz3235);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3235, zgaz3101);
                    sail_int zgsz3236;
                    CREATE(sail_int)(&zgsz3236);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3236, INT64_C(1));
                    sail_int zgsz3237;
                    CREATE(sail_int)(&zgsz3237);
                    sub_int(&zgsz3237, zgsz3235, zgsz3236);
                    zgaz3103 = CONVERT_OF(mach_int, sail_int)(zgsz3237);
                    KILL(sail_int)(&zgsz3237);
                    KILL(sail_int)(&zgsz3236);
                    KILL(sail_int)(&zgsz3235);
                }
            }
            int64_t zgaz3104;
            {
                int64_t zgaz3102;
                {
                    sail_int zgsz3238;
                    CREATE(sail_int)(&zgsz3238);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3238, zexp);
                    sail_int zgsz3239;
                    CREATE(sail_int)(&zgsz3239);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3239, zCAP_MW);
                    sail_int zgsz3240;
                    CREATE(sail_int)(&zgsz3240);
                    add_int(&zgsz3240, zgsz3238, zgsz3239);
                    zgaz3102 = CONVERT_OF(mach_int, sail_int)(zgsz3240);
                    KILL(sail_int)(&zgsz3240);
                    KILL(sail_int)(&zgsz3239);
                    KILL(sail_int)(&zgsz3238);
                }
                {
                    sail_int zgsz3241;
                    CREATE(sail_int)(&zgsz3241);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3241, zgaz3102);
                    sail_int zgsz3242;
                    CREATE(sail_int)(&zgsz3242);
                    CONVERT_OF(sail_int, mach_int)(&zgsz3242, INT64_C(3));
                    sail_int zgsz3243;
                    CREATE(sail_int)(&zgsz3243);
                    sub_int(&zgsz3243, zgsz3241, zgsz3242);
                    zgaz3104 = CONVERT_OF(mach_int, sail_int)(zgsz3243);
                    KILL(sail_int)(&zgsz3243);
                    KILL(sail_int)(&zgsz3242);
                    KILL(sail_int)(&zgsz3241);
                }
            }
            {
                sail_int zgsz3244;
                CREATE(sail_int)(&zgsz3244);
                CONVERT_OF(sail_int, mach_int)(&zgsz3244, zgaz3103);
                sail_int zgsz3245;
                CREATE(sail_int)(&zgsz3245);
                CONVERT_OF(sail_int, mach_int)(&zgsz3245, zgaz3104);
                lbits zgsz3246;
                CREATE(lbits)(&zgsz3246);
                vector_subrange_lbits(&zgsz3246, za, zgsz3244, zgsz3245);
                zA3 = CONVERT_OF(fbits, lbits)(zgsz3246, true);
                KILL(lbits)(&zgsz3246);
                KILL(sail_int)(&zgsz3245);
                KILL(sail_int)(&zgsz3244);
            }
        }
        uint64_t zB3;
        {
            int64_t zgaz399;
            {
                sail_int zgsz3247;
                CREATE(sail_int)(&zgsz3247);
                CONVERT_OF(sail_int, mach_int)(&zgsz3247, zCAP_MW);
                sail_int zgsz3248;
                CREATE(sail_int)(&zgsz3248);
                CONVERT_OF(sail_int, mach_int)(&zgsz3248, INT64_C(1));
                sail_int zgsz3249;
                CREATE(sail_int)(&zgsz3249);
                sub_int(&zgsz3249, zgsz3247, zgsz3248);
                zgaz399 = CONVERT_OF(mach_int, sail_int)(zgsz3249);
                KILL(sail_int)(&zgsz3249);
                KILL(sail_int)(&zgsz3248);
                KILL(sail_int)(&zgsz3247);
            }
            int64_t zgaz3100;
            {
                sail_int zgsz3250;
                CREATE(sail_int)(&zgsz3250);
                CONVERT_OF(sail_int, mach_int)(&zgsz3250, zCAP_MW);
                sail_int zgsz3251;
                CREATE(sail_int)(&zgsz3251);
                CONVERT_OF(sail_int, mach_int)(&zgsz3251, INT64_C(3));
                sail_int zgsz3252;
                CREATE(sail_int)(&zgsz3252);
                sub_int(&zgsz3252, zgsz3250, zgsz3251);
                zgaz3100 = CONVERT_OF(mach_int, sail_int)(zgsz3252);
                KILL(sail_int)(&zgsz3252);
                KILL(sail_int)(&zgsz3251);
                KILL(sail_int)(&zgsz3250);
            }
            {
                lbits zgsz3253;
                CREATE(lbits)(&zgsz3253);
                CONVERT_OF(lbits, fbits)(&zgsz3253, zbottom, UINT64_C(16), true);
                sail_int zgsz3254;
                CREATE(sail_int)(&zgsz3254);
                CONVERT_OF(sail_int, mach_int)(&zgsz3254, zgaz399);
                sail_int zgsz3255;
                CREATE(sail_int)(&zgsz3255);
                CONVERT_OF(sail_int, mach_int)(&zgsz3255, zgaz3100);
                lbits zgsz3256;
                CREATE(lbits)(&zgsz3256);
                vector_subrange_lbits(&zgsz3256, zgsz3253, zgsz3254, zgsz3255);
                zB3 = CONVERT_OF(fbits, lbits)(zgsz3256, true);
                KILL(lbits)(&zgsz3256);
                KILL(sail_int)(&zgsz3255);
                KILL(sail_int)(&zgsz3254);
                KILL(lbits)(&zgsz3253);
            }
        }
        uint64_t zT3;
        {
            int64_t zgaz397;
            {
                sail_int zgsz3257;
                CREATE(sail_int)(&zgsz3257);
                CONVERT_OF(sail_int, mach_int)(&zgsz3257, zCAP_MW);
                sail_int zgsz3258;
                CREATE(sail_int)(&zgsz3258);
                CONVERT_OF(sail_int, mach_int)(&zgsz3258, INT64_C(1));
                sail_int zgsz3259;
                CREATE(sail_int)(&zgsz3259);
                sub_int(&zgsz3259, zgsz3257, zgsz3258);
                zgaz397 = CONVERT_OF(mach_int, sail_int)(zgsz3259);
                KILL(sail_int)(&zgsz3259);
                KILL(sail_int)(&zgsz3258);
                KILL(sail_int)(&zgsz3257);
            }
            int64_t zgaz398;
            {
                sail_int zgsz3260;
                CREATE(sail_int)(&zgsz3260);
                CONVERT_OF(sail_int, mach_int)(&zgsz3260, zCAP_MW);
                sail_int zgsz3261;
                CREATE(sail_int)(&zgsz3261);
                CONVERT_OF(sail_int, mach_int)(&zgsz3261, INT64_C(3));
                sail_int zgsz3262;
                CREATE(sail_int)(&zgsz3262);
                sub_int(&zgsz3262, zgsz3260, zgsz3261);
                zgaz398 = CONVERT_OF(mach_int, sail_int)(zgsz3262);
                KILL(sail_int)(&zgsz3262);
                KILL(sail_int)(&zgsz3261);
                KILL(sail_int)(&zgsz3260);
            }
            {
                lbits zgsz3263;
                CREATE(lbits)(&zgsz3263);
                CONVERT_OF(lbits, fbits)(&zgsz3263, ztop, UINT64_C(16), true);
                sail_int zgsz3264;
                CREATE(sail_int)(&zgsz3264);
                CONVERT_OF(sail_int, mach_int)(&zgsz3264, zgaz397);
                sail_int zgsz3265;
                CREATE(sail_int)(&zgsz3265);
                CONVERT_OF(sail_int, mach_int)(&zgsz3265, zgaz398);
                lbits zgsz3266;
                CREATE(lbits)(&zgsz3266);
                vector_subrange_lbits(&zgsz3266, zgsz3263, zgsz3264, zgsz3265);
                zT3 = CONVERT_OF(fbits, lbits)(zgsz3266, true);
                KILL(lbits)(&zgsz3266);
                KILL(sail_int)(&zgsz3265);
                KILL(sail_int)(&zgsz3264);
                KILL(lbits)(&zgsz3263);
            }
        }
        uint64_t zR3;
        {
            lbits zgsz3267;
            CREATE(lbits)(&zgsz3267);
            CONVERT_OF(lbits, fbits)(&zgsz3267, zB3, UINT64_C(3), true);
            lbits zgsz3268;
            CREATE(lbits)(&zgsz3268);
            CONVERT_OF(lbits, fbits)(&zgsz3268, UINT64_C(0b001), UINT64_C(3), true);
            lbits zgsz3269;
            CREATE(lbits)(&zgsz3269);
            sub_bits(&zgsz3269, zgsz3267, zgsz3268);
            zR3 = CONVERT_OF(fbits, lbits)(zgsz3269, true);
            KILL(lbits)(&zgsz3269);
            KILL(lbits)(&zgsz3268);
            KILL(lbits)(&zgsz3267);
        }
        int64_t zaHi;
        zaHi = INT64_C(0);
        {
            bool zgaz370;
            zgaz370 = sailgen_zzzz7NzzJzzK3z3CapUnsignedLessThan(zA3, zR3);
            unit zgsz3270;
            if (zgaz370) {
                zaHi = INT64_C(1);
                zgsz3270 = UNIT;
            } else {
                zaHi = INT64_C(0);
                zgsz3270 = UNIT;
            }
        }
        struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3271;
        CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3271);
        {
            __label__ case_93, finish_match_92;

            int64_t zaHishadowz30;
            zaHishadowz30 = zaHi;
            int64_t zbHi;
            zbHi = INT64_C(0);
            {
                bool zgaz371;
                zgaz371 = sailgen_zzzz7NzzJzzK3z3CapUnsignedLessThan(zB3, zR3);
                unit zgsz3272;
                if (zgaz371) {
                    zbHi = INT64_C(1);
                    zgsz3272 = UNIT;
                } else {
                    zbHi = INT64_C(0);
                    zgsz3272 = UNIT;
                }
            }
            struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3273;
            CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3273);
            {
                __label__ case_95, finish_match_94;

                int64_t zbHishadowz31;
                zbHishadowz31 = zbHi;
                int64_t ztHi;
                ztHi = INT64_C(0);
                {
                    bool zgaz372;
                    zgaz372 = sailgen_zzzz7NzzJzzK3z3CapUnsignedLessThan(zT3, zR3);
                    unit zgsz3274;
                    if (zgaz372) {
                        ztHi = INT64_C(1);
                        zgsz3274 = UNIT;
                    } else {
                        ztHi = INT64_C(0);
                        zgsz3274 = UNIT;
                    }
                }
                struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3275;
                CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3275);
                {
                    __label__ case_97, finish_match_96;

                    int64_t ztHishadowz32;
                    ztHishadowz32 = ztHi;
                    int64_t zgaz373;
                    {
                        sail_int zgsz3276;
                        CREATE(sail_int)(&zgsz3276);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3276, zbHishadowz31);
                        sail_int zgsz3277;
                        CREATE(sail_int)(&zgsz3277);
                        CONVERT_OF(sail_int, mach_int)(&zgsz3277, zaHishadowz30);
                        sail_int zgsz3278;
                        CREATE(sail_int)(&zgsz3278);
                        sub_int(&zgsz3278, zgsz3276, zgsz3277);
                        zgaz373 = CONVERT_OF(mach_int, sail_int)(zgsz3278);
                        KILL(sail_int)(&zgsz3278);
                        KILL(sail_int)(&zgsz3277);
                        KILL(sail_int)(&zgsz3276);
                    }
                    struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3279;
                    CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3279);
                    {
                        __label__ case_99, finish_match_98;

                        int64_t zcorrection_base;
                        zcorrection_base = zgaz373;
                        int64_t zgaz374;
                        {
                            sail_int zgsz3280;
                            CREATE(sail_int)(&zgsz3280);
                            CONVERT_OF(sail_int, mach_int)(&zgsz3280, ztHishadowz32);
                            sail_int zgsz3281;
                            CREATE(sail_int)(&zgsz3281);
                            CONVERT_OF(sail_int, mach_int)(&zgsz3281, zaHishadowz30);
                            sail_int zgsz3282;
                            CREATE(sail_int)(&zgsz3282);
                            sub_int(&zgsz3282, zgsz3280, zgsz3281);
                            zgaz374 = CONVERT_OF(mach_int, sail_int)(zgsz3282);
                            KILL(sail_int)(&zgsz3282);
                            KILL(sail_int)(&zgsz3281);
                            KILL(sail_int)(&zgsz3280);
                        }
                        struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3283;
                        CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3283);
                        {
                            __label__ cleanup_101, end_cleanup_102;

                            int64_t zcorrection_limit;
                            zcorrection_limit = zgaz374;
                            {
                                bool zgaz377;
                                {
                                    int64_t zgaz375;
                                    {
                                        sail_int zgsz3284;
                                        CREATE(sail_int)(&zgsz3284);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3284, zexp);
                                        sail_int zgsz3285;
                                        CREATE(sail_int)(&zgsz3285);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3285, zCAP_MW);
                                        sail_int zgsz3286;
                                        CREATE(sail_int)(&zgsz3286);
                                        add_int(&zgsz3286, zgsz3284, zgsz3285);
                                        zgaz375 = CONVERT_OF(mach_int, sail_int)(zgsz3286);
                                        KILL(sail_int)(&zgsz3286);
                                        KILL(sail_int)(&zgsz3285);
                                        KILL(sail_int)(&zgsz3284);
                                    }
                                    int64_t zgaz376;
                                    {
                                        sail_int zgsz3287;
                                        CREATE(sail_int)(&zgsz3287);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3287, zCAP_MAX_EXPONENT);
                                        sail_int zgsz3288;
                                        CREATE(sail_int)(&zgsz3288);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3288, zCAP_MW);
                                        sail_int zgsz3289;
                                        CREATE(sail_int)(&zgsz3289);
                                        add_int(&zgsz3289, zgsz3287, zgsz3288);
                                        zgaz376 = CONVERT_OF(mach_int, sail_int)(zgsz3289);
                                        KILL(sail_int)(&zgsz3289);
                                        KILL(sail_int)(&zgsz3288);
                                        KILL(sail_int)(&zgsz3287);
                                    }
                                    {
                                        sail_int zgsz3291;
                                        CREATE(sail_int)(&zgsz3291);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3291, zgaz376);
                                        sail_int zgsz3290;
                                        CREATE(sail_int)(&zgsz3290);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3290, zgaz375);
                                        zgaz377 = lt(zgsz3290, zgsz3291);
                                        KILL(sail_int)(&zgsz3291);
                                        KILL(sail_int)(&zgsz3290);
                                    }
                                }
                                unit zgsz3292;
                                if (zgaz377) {
                                    sbits zatop;
                                    {
                                        int64_t zgaz382;
                                        {
                                            sail_int zgsz3293;
                                            CREATE(sail_int)(&zgsz3293);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3293, zexp);
                                            sail_int zgsz3294;
                                            CREATE(sail_int)(&zgsz3294);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3294, zCAP_MW);
                                            sail_int zgsz3295;
                                            CREATE(sail_int)(&zgsz3295);
                                            add_int(&zgsz3295, zgsz3293, zgsz3294);
                                            zgaz382 = CONVERT_OF(mach_int, sail_int)(zgsz3295);
                                            KILL(sail_int)(&zgsz3295);
                                            KILL(sail_int)(&zgsz3294);
                                            KILL(sail_int)(&zgsz3293);
                                        }
                                        {
                                            sail_int zgsz3296;
                                            CREATE(sail_int)(&zgsz3296);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3296, INT64_C(65));
                                            sail_int zgsz3297;
                                            CREATE(sail_int)(&zgsz3297);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3297, zgaz382);
                                            lbits zgsz3298;
                                            CREATE(lbits)(&zgsz3298);
                                            vector_subrange_lbits(&zgsz3298, za, zgsz3296, zgsz3297);
                                            zatop = CONVERT_OF(sbits, lbits)(zgsz3298, true);
                                            KILL(lbits)(&zgsz3298);
                                            KILL(sail_int)(&zgsz3297);
                                            KILL(sail_int)(&zgsz3296);
                                        }
                                    }
                                    {
                                        int64_t zgaz378;
                                        {
                                            sail_int zgsz3299;
                                            CREATE(sail_int)(&zgsz3299);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3299, zexp);
                                            sail_int zgsz3300;
                                            CREATE(sail_int)(&zgsz3300);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3300, zCAP_MW);
                                            sail_int zgsz3301;
                                            CREATE(sail_int)(&zgsz3301);
                                            add_int(&zgsz3301, zgsz3299, zgsz3300);
                                            zgaz378 = CONVERT_OF(mach_int, sail_int)(zgsz3301);
                                            KILL(sail_int)(&zgsz3301);
                                            KILL(sail_int)(&zgsz3300);
                                            KILL(sail_int)(&zgsz3299);
                                        }
                                        sbits zgaz379;
                                        {
                                            lbits zgsz3302;
                                            CREATE(lbits)(&zgsz3302);
                                            CONVERT_OF(lbits, sbits)(&zgsz3302, zatop, true);
                                            sail_int zgsz3303;
                                            CREATE(sail_int)(&zgsz3303);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3303, zcorrection_base);
                                            lbits zgsz3304;
                                            CREATE(lbits)(&zgsz3304);
                                            add_bits_int(&zgsz3304, zgsz3302, zgsz3303);
                                            zgaz379 = CONVERT_OF(sbits, lbits)(zgsz3304, true);
                                            KILL(lbits)(&zgsz3304);
                                            KILL(sail_int)(&zgsz3303);
                                            KILL(lbits)(&zgsz3302);
                                        }
                                        {
                                            lbits zgsz3307;
                                            CREATE(lbits)(&zgsz3307);
                                            CONVERT_OF(lbits, sbits)(&zgsz3307, zgaz379, true);
                                            sail_int zgsz3306;
                                            CREATE(sail_int)(&zgsz3306);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3306, zgaz378);
                                            sail_int zgsz3305;
                                            CREATE(sail_int)(&zgsz3305);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3305, INT64_C(65));
                                            vector_update_subrange_lbits(&zbase, zbase, zgsz3305, zgsz3306, zgsz3307);
                                            KILL(lbits)(&zgsz3307);
                                            KILL(sail_int)(&zgsz3306);
                                            KILL(sail_int)(&zgsz3305);
                                        }
                                        unit zgsz3308;
                                        zgsz3308 = UNIT;
                                    }
                                    int64_t zgaz380;
                                    {
                                        sail_int zgsz3309;
                                        CREATE(sail_int)(&zgsz3309);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3309, zexp);
                                        sail_int zgsz3310;
                                        CREATE(sail_int)(&zgsz3310);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3310, zCAP_MW);
                                        sail_int zgsz3311;
                                        CREATE(sail_int)(&zgsz3311);
                                        add_int(&zgsz3311, zgsz3309, zgsz3310);
                                        zgaz380 = CONVERT_OF(mach_int, sail_int)(zgsz3311);
                                        KILL(sail_int)(&zgsz3311);
                                        KILL(sail_int)(&zgsz3310);
                                        KILL(sail_int)(&zgsz3309);
                                    }
                                    sbits zgaz381;
                                    {
                                        lbits zgsz3312;
                                        CREATE(lbits)(&zgsz3312);
                                        CONVERT_OF(lbits, sbits)(&zgsz3312, zatop, true);
                                        sail_int zgsz3313;
                                        CREATE(sail_int)(&zgsz3313);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3313, zcorrection_limit);
                                        lbits zgsz3314;
                                        CREATE(lbits)(&zgsz3314);
                                        add_bits_int(&zgsz3314, zgsz3312, zgsz3313);
                                        zgaz381 = CONVERT_OF(sbits, lbits)(zgsz3314, true);
                                        KILL(lbits)(&zgsz3314);
                                        KILL(sail_int)(&zgsz3313);
                                        KILL(lbits)(&zgsz3312);
                                    }
                                    {
                                        lbits zgsz3317;
                                        CREATE(lbits)(&zgsz3317);
                                        CONVERT_OF(lbits, sbits)(&zgsz3317, zgaz381, true);
                                        sail_int zgsz3316;
                                        CREATE(sail_int)(&zgsz3316);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3316, zgaz380);
                                        sail_int zgsz3315;
                                        CREATE(sail_int)(&zgsz3315);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3315, INT64_C(65));
                                        vector_update_subrange_lbits(&zlimit, zlimit, zgsz3315, zgsz3316, zgsz3317);
                                        KILL(lbits)(&zgsz3317);
                                        KILL(sail_int)(&zgsz3316);
                                        KILL(sail_int)(&zgsz3315);
                                    }
                                    zgsz3292 = UNIT;

                                } else {
                                    zgsz3292 = UNIT;
                                }
                            }
                            uint64_t zl2;
                            {
                                sail_int zgsz3318;
                                CREATE(sail_int)(&zgsz3318);
                                CONVERT_OF(sail_int, mach_int)(&zgsz3318, INT64_C(64));
                                sail_int zgsz3319;
                                CREATE(sail_int)(&zgsz3319);
                                CONVERT_OF(sail_int, mach_int)(&zgsz3319, INT64_C(63));
                                lbits zgsz3320;
                                CREATE(lbits)(&zgsz3320);
                                vector_subrange_lbits(&zgsz3320, zlimit, zgsz3318, zgsz3319);
                                zl2 = CONVERT_OF(fbits, lbits)(zgsz3320, true);
                                KILL(lbits)(&zgsz3320);
                                KILL(sail_int)(&zgsz3319);
                                KILL(sail_int)(&zgsz3318);
                            }
                            uint64_t zb2;
                            {
                                uint64_t zgaz396;
                                {
                                    fbits zgaz395;
                                    {
                                        sail_int zgsz3321;
                                        CREATE(sail_int)(&zgsz3321);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3321, INT64_C(63));
                                        zgaz395 = bitvector_access(zbase, zgsz3321);
                                        KILL(sail_int)(&zgsz3321);
                                    }
                                    uint64_t zgsz3322;
                                    zgsz3322 = UINT64_C(0b0);
                                    zgsz3322 = update_fbits(zgsz3322, INT64_C(0), zgaz395);
                                    zgaz396 = zgsz3322;
                                }
                                {
                                    lbits zgsz3323;
                                    CREATE(lbits)(&zgsz3323);
                                    CONVERT_OF(lbits, fbits)(&zgsz3323, UINT64_C(0b0), UINT64_C(1), true);
                                    lbits zgsz3324;
                                    CREATE(lbits)(&zgsz3324);
                                    CONVERT_OF(lbits, fbits)(&zgsz3324, zgaz396, UINT64_C(1), true);
                                    lbits zgsz3325;
                                    CREATE(lbits)(&zgsz3325);
                                    append(&zgsz3325, zgsz3323, zgsz3324);
                                    zb2 = CONVERT_OF(fbits, lbits)(zgsz3325, true);
                                    KILL(lbits)(&zgsz3325);
                                    KILL(lbits)(&zgsz3324);
                                    KILL(lbits)(&zgsz3323);
                                }
                            }
                            {
                                bool zgaz386;
                                {
                                    bool zgaz385;
                                    {
                                        int64_t zgaz383;
                                        {
                                            sail_int zgsz3326;
                                            CREATE(sail_int)(&zgsz3326);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3326, zCAP_MAX_EXPONENT);
                                            sail_int zgsz3327;
                                            CREATE(sail_int)(&zgsz3327);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3327, INT64_C(1));
                                            sail_int zgsz3328;
                                            CREATE(sail_int)(&zgsz3328);
                                            sub_int(&zgsz3328, zgsz3326, zgsz3327);
                                            zgaz383 = CONVERT_OF(mach_int, sail_int)(zgsz3328);
                                            KILL(sail_int)(&zgsz3328);
                                            KILL(sail_int)(&zgsz3327);
                                            KILL(sail_int)(&zgsz3326);
                                        }
                                        {
                                            sail_int zgsz3330;
                                            CREATE(sail_int)(&zgsz3330);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3330, zgaz383);
                                            sail_int zgsz3329;
                                            CREATE(sail_int)(&zgsz3329);
                                            CONVERT_OF(sail_int, mach_int)(&zgsz3329, zexp);
                                            zgaz385 = lt(zgsz3329, zgsz3330);
                                            KILL(sail_int)(&zgsz3330);
                                            KILL(sail_int)(&zgsz3329);
                                        }
                                    }
                                    bool zgsz3334;
                                    if (zgaz385) {
                                        uint64_t zgaz384;
                                        {
                                            lbits zgsz3331;
                                            CREATE(lbits)(&zgsz3331);
                                            CONVERT_OF(lbits, fbits)(&zgsz3331, zl2, UINT64_C(2), true);
                                            lbits zgsz3332;
                                            CREATE(lbits)(&zgsz3332);
                                            CONVERT_OF(lbits, fbits)(&zgsz3332, zb2, UINT64_C(2), true);
                                            lbits zgsz3333;
                                            CREATE(lbits)(&zgsz3333);
                                            sub_bits(&zgsz3333, zgsz3331, zgsz3332);
                                            zgaz384 = CONVERT_OF(fbits, lbits)(zgsz3333, true);
                                            KILL(lbits)(&zgsz3333);
                                            KILL(lbits)(&zgsz3332);
                                            KILL(lbits)(&zgsz3331);
                                        }
                                        zgsz3334 =
                                            sailgen_zzzz7NzzJzzK2z3CapUnsignedGreaterThan(zgaz384, UINT64_C(0b01));

                                    } else {
                                        zgsz3334 = false;
                                    }
                                    zgaz386 = zgsz3334;
                                }
                                unit zgsz3335;
                                if (zgaz386) {
                                    fbits zgaz390;
                                    {
                                        uint64_t zgaz389;
                                        {
                                            uint64_t zgaz388;
                                            {
                                                fbits zgaz387;
                                                {
                                                    sail_int zgsz3336;
                                                    CREATE(sail_int)(&zgsz3336);
                                                    CONVERT_OF(sail_int, mach_int)(&zgsz3336, INT64_C(64));
                                                    zgaz387 = bitvector_access(zlimit, zgsz3336);
                                                    KILL(sail_int)(&zgsz3336);
                                                }
                                                uint64_t zgsz3337;
                                                zgsz3337 = UINT64_C(0b0);
                                                zgsz3337 = update_fbits(zgsz3337, INT64_C(0), zgaz387);
                                                zgaz388 = zgsz3337;
                                            }
                                            {
                                                lbits zgsz3338;
                                                CREATE(lbits)(&zgsz3338);
                                                CONVERT_OF(lbits, fbits)(&zgsz3338, zgaz388, UINT64_C(1), true);
                                                lbits zgsz3339;
                                                CREATE(lbits)(&zgsz3339);
                                                not_bits(&zgsz3339, zgsz3338);
                                                zgaz389 = CONVERT_OF(fbits, lbits)(zgsz3339, true);
                                                KILL(lbits)(&zgsz3339);
                                                KILL(lbits)(&zgsz3338);
                                            }
                                        }
                                        zgaz390 = sailgen_Bit(zgaz389);
                                    }
                                    {
                                        sail_int zgsz3340;
                                        CREATE(sail_int)(&zgsz3340);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3340, INT64_C(64));
                                        update_lbits(&zlimit, zlimit, zgsz3340, zgaz390);
                                        KILL(sail_int)(&zgsz3340);
                                    }
                                    zgsz3335 = UNIT;

                                } else {
                                    zgsz3335 = UNIT;
                                }
                            }
                            struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgaz394;
                            CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgaz394);
                            {
                                lbits zgaz392;
                                CREATE(lbits)(&zgaz392);
                                {
                                    uint64_t zgaz391;
                                    {
                                        sail_int zgsz3341;
                                        CREATE(sail_int)(&zgsz3341);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3341, INT64_C(63));
                                        sail_int zgsz3342;
                                        CREATE(sail_int)(&zgsz3342);
                                        CONVERT_OF(sail_int, mach_int)(&zgsz3342, INT64_C(0));
                                        lbits zgsz3343;
                                        CREATE(lbits)(&zgsz3343);
                                        vector_subrange_lbits(&zgsz3343, zbase, zgsz3341, zgsz3342);
                                        zgaz391 = CONVERT_OF(fbits, lbits)(zgsz3343, true);
                                        KILL(lbits)(&zgsz3343);
                                        KILL(sail_int)(&zgsz3342);
                                        KILL(sail_int)(&zgsz3341);
                                    }
                                    {
                                        lbits zgsz3345;
                                        CREATE(lbits)(&zgsz3345);
                                        CONVERT_OF(lbits, fbits)(&zgsz3345, zgaz391, UINT64_C(64), true);
                                        lbits zgsz3344;
                                        CREATE(lbits)(&zgsz3344);
                                        CONVERT_OF(lbits, fbits)(&zgsz3344, UINT64_C(0b0), UINT64_C(1), true);
                                        append(&zgaz392, zgsz3344, zgsz3345);
                                        KILL(lbits)(&zgsz3345);
                                        KILL(lbits)(&zgsz3344);
                                    }
                                }
                                lbits zgaz393;
                                CREATE(lbits)(&zgaz393);
                                {
                                    sail_int zgsz3347;
                                    CREATE(sail_int)(&zgsz3347);
                                    CONVERT_OF(sail_int, mach_int)(&zgsz3347, INT64_C(0));
                                    sail_int zgsz3346;
                                    CREATE(sail_int)(&zgsz3346);
                                    CONVERT_OF(sail_int, mach_int)(&zgsz3346, INT64_C(64));
                                    vector_subrange_lbits(&zgaz393, zlimit, zgsz3346, zgsz3347);
                                    KILL(sail_int)(&zgsz3347);
                                    KILL(sail_int)(&zgsz3346);
                                }
                                struct ztuple_z8z5bvzCz0z5bvzCz0z5boolz9 zgsz3348;
                                CREATE(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3348);
                                COPY(lbits)(&((&zgsz3348)->ztup0), zgaz392);
                                COPY(lbits)(&((&zgsz3348)->ztup1), zgaz393);
                                zgsz3348.ztup2 = true;
                                COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgaz394, zgsz3348);
                                KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3348);
                                KILL(lbits)(&zgaz393);
                                KILL(lbits)(&zgaz392);
                            }
                            COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)((*(&zcbz314)), zgaz394);
                            goto cleanup_101;
                            /* unreachable after return */
                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgaz394);

                            goto finish_match_98;
                            goto end_cleanup_102;
                        cleanup_101:;

                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3167);

                            KILL(lbits)(&zbase);
                            KILL(lbits)(&zlimit);

                            KILL(lbits)(&za);

                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3271);

                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3273);

                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3275);

                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3279);

                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3283);

                            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgaz394);
                            goto end_function_100;
                        end_cleanup_102:;
                        }
                    case_99:;
                        sail_match_failure("CapGetBounds");
                    finish_match_98:;
                        COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3279, zgsz3283);
                        KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3283);

                        goto finish_match_96;
                    }
                case_97:;
                    sail_match_failure("CapGetBounds");
                finish_match_96:;
                    COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3275, zgsz3279);
                    KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3279);

                    goto finish_match_94;
                }
            case_95:;
                sail_match_failure("CapGetBounds");
            finish_match_94:;
                COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3273, zgsz3275);
                KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3275);

                goto finish_match_92;
            }
        case_93:;
            sail_match_failure("CapGetBounds");
        finish_match_92:;
            COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3271, zgsz3273);
            KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3273);

            goto finish_match_90;
        }
    case_91:;
        sail_match_failure("CapGetBounds");
    finish_match_90:;
        COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3167, zgsz3271);
        KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3271);

        KILL(lbits)(&za);

        KILL(lbits)(&zlimit);
        KILL(lbits)(&zbase);

        goto finish_match_88;
    }
case_89:;
    sail_match_failure("CapGetBounds");
finish_match_88:;
    COPY(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)((*(&zcbz314)), zgsz3167);
    KILL(ztuple_z8z5bvzCz0z5bvzCz0z5boolz9)(&zgsz3167);

end_function_100:;
    goto end_function_128;
end_block_exception_107:;
    goto end_function_128;
end_function_128:;
}

static int64_t zCAP_TAG_BIT;
static void create_letbind_19(void) {

    int64_t zgsz3355;
    zgsz3355 = INT64_C(128);
    zCAP_TAG_BIT = zgsz3355;

let_end_108:;
}
static void kill_letbind_19(void) {}

static int64_t zCAP_OTYPE_HI_BIT;
static void create_letbind_20(void) {

    int64_t zgsz3356;
    zgsz3356 = INT64_C(109);
    zCAP_OTYPE_HI_BIT = zgsz3356;

let_end_109:;
}
static void kill_letbind_20(void) {}

static int64_t zCAP_OTYPE_LO_BIT;
static void create_letbind_21(void) {

    int64_t zgsz3357;
    zgsz3357 = INT64_C(95);
    zCAP_OTYPE_LO_BIT = zgsz3357;

let_end_110:;
}
static void kill_letbind_21(void) {}

static uint64_t sailgen_CapGetObjectType(lbits);

static uint64_t sailgen_CapGetObjectType(lbits zc) {
    __label__ cleanup_113, end_cleanup_114, end_function_112, end_block_exception_115;

    uint64_t zcbz315;
    uint64_t zgaz3108;
    {
        uint64_t zgaz3107;
        {
            sail_int zgsz3358;
            CREATE(sail_int)(&zgsz3358);
            CONVERT_OF(sail_int, mach_int)(&zgsz3358, zCAP_OTYPE_HI_BIT);
            sail_int zgsz3359;
            CREATE(sail_int)(&zgsz3359);
            CONVERT_OF(sail_int, mach_int)(&zgsz3359, zCAP_OTYPE_LO_BIT);
            lbits zgsz3360;
            CREATE(lbits)(&zgsz3360);
            vector_subrange_lbits(&zgsz3360, zc, zgsz3358, zgsz3359);
            zgaz3107 = CONVERT_OF(fbits, lbits)(zgsz3360, true);
            KILL(lbits)(&zgsz3360);
            KILL(sail_int)(&zgsz3359);
            KILL(sail_int)(&zgsz3358);
        }
        {
            lbits zgsz3361;
            CREATE(lbits)(&zgsz3361);
            CONVERT_OF(lbits, fbits)(&zgsz3361, zgaz3107, UINT64_C(15), true);
            sail_int zgsz3362;
            CREATE(sail_int)(&zgsz3362);
            CONVERT_OF(sail_int, mach_int)(&zgsz3362, zCAP_VALUE_NUM_BITS);
            lbits zgsz3363;
            CREATE(lbits)(&zgsz3363);
            zero_extend(&zgsz3363, zgsz3361, zgsz3362);
            zgaz3108 = CONVERT_OF(fbits, lbits)(zgsz3363, true);
            KILL(lbits)(&zgsz3363);
            KILL(sail_int)(&zgsz3362);
            KILL(lbits)(&zgsz3361);
        }
    }
    zcbz315 = zgaz3108;
    goto cleanup_113;
    /* unreachable after return */

    goto end_cleanup_114;
cleanup_113:;

    goto end_function_112;
end_cleanup_114:;
end_function_112:;
    return zcbz315;
end_block_exception_115:;

    return UINT64_C(0xdeadc0de);
}

static int64_t zCAP_PERMS_HI_BIT;
static void create_letbind_22(void) {

    int64_t zgsz3364;
    zgsz3364 = INT64_C(127);
    zCAP_PERMS_HI_BIT = zgsz3364;

let_end_116:;
}
static void kill_letbind_22(void) {}

static int64_t zCAP_PERMS_LO_BIT;
static void create_letbind_23(void) {

    int64_t zgsz3365;
    zgsz3365 = INT64_C(110);
    zCAP_PERMS_LO_BIT = zgsz3365;

let_end_117:;
}
static void kill_letbind_23(void) {}

static uint64_t sailgen_CapGetPermissions(lbits);

static uint64_t sailgen_CapGetPermissions(lbits zc) {
    __label__ cleanup_120, end_cleanup_121, end_function_119, end_block_exception_122;

    uint64_t zcbz316;
    uint64_t zgaz3109;
    {
        sail_int zgsz3366;
        CREATE(sail_int)(&zgsz3366);
        CONVERT_OF(sail_int, mach_int)(&zgsz3366, zCAP_PERMS_HI_BIT);
        sail_int zgsz3367;
        CREATE(sail_int)(&zgsz3367);
        CONVERT_OF(sail_int, mach_int)(&zgsz3367, zCAP_PERMS_LO_BIT);
        lbits zgsz3368;
        CREATE(lbits)(&zgsz3368);
        vector_subrange_lbits(&zgsz3368, zc, zgsz3366, zgsz3367);
        zgaz3109 = CONVERT_OF(fbits, lbits)(zgsz3368, true);
        KILL(lbits)(&zgsz3368);
        KILL(sail_int)(&zgsz3367);
        KILL(sail_int)(&zgsz3366);
    }
    zcbz316 = zgaz3109;
    goto cleanup_120;
    /* unreachable after return */

    goto end_cleanup_121;
cleanup_120:;

    goto end_function_119;
end_cleanup_121:;
end_function_119:;
    return zcbz316;
end_block_exception_122:;

    return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_CapGetTag(lbits);

static uint64_t sailgen_CapGetTag(lbits zc) {
    __label__ cleanup_125, end_cleanup_126, end_function_124, end_block_exception_127;

    uint64_t zcbz317;
    uint64_t zgaz3112;
    {
        uint64_t zgaz3111;
        {
            fbits zgaz3110;
            {
                sail_int zgsz3369;
                CREATE(sail_int)(&zgsz3369);
                CONVERT_OF(sail_int, mach_int)(&zgsz3369, zCAP_TAG_BIT);
                zgaz3110 = bitvector_access(zc, zgsz3369);
                KILL(sail_int)(&zgsz3369);
            }
            uint64_t zgsz3370;
            zgsz3370 = UINT64_C(0b0);
            zgsz3370 = update_fbits(zgsz3370, INT64_C(0), zgaz3110);
            zgaz3111 = zgsz3370;
        }
        {
            lbits zgsz3371;
            CREATE(lbits)(&zgsz3371);
            CONVERT_OF(lbits, fbits)(&zgsz3371, zgaz3111, UINT64_C(1), true);
            sail_int zgsz3372;
            CREATE(sail_int)(&zgsz3372);
            CONVERT_OF(sail_int, mach_int)(&zgsz3372, INT64_C(64));
            lbits zgsz3373;
            CREATE(lbits)(&zgsz3373);
            zero_extend(&zgsz3373, zgsz3371, zgsz3372);
            zgaz3112 = CONVERT_OF(fbits, lbits)(zgsz3373, true);
            KILL(lbits)(&zgsz3373);
            KILL(sail_int)(&zgsz3372);
            KILL(lbits)(&zgsz3371);
        }
    }
    zcbz317 = zgaz3112;
    goto cleanup_125;
    /* unreachable after return */

    goto end_cleanup_126;
cleanup_125:;

    goto end_function_124;
end_cleanup_126:;
end_function_124:;
    return zcbz317;
end_block_exception_127:;

    return UINT64_C(0xdeadc0de);
}

static void model_init(void) {
    setup_rts();
    create_letbind_0();
    create_letbind_1();
    create_letbind_2();
    create_letbind_3();
    create_letbind_4();
    create_letbind_5();
    create_letbind_6();
    create_letbind_7();
    create_letbind_8();
    create_letbind_9();
    create_letbind_10();
    create_letbind_11();
    create_letbind_12();
    create_letbind_13();
    create_letbind_14();
    create_letbind_15();
    create_letbind_16();
    create_letbind_17();
    create_letbind_18();
    create_letbind_19();
    create_letbind_20();
    create_letbind_21();
    create_letbind_22();
    create_letbind_23();
}

static void model_fini(void) {
    kill_letbind_23();
    kill_letbind_22();
    kill_letbind_21();
    kill_letbind_20();
    kill_letbind_19();
    kill_letbind_18();
    kill_letbind_17();
    kill_letbind_16();
    kill_letbind_15();
    kill_letbind_14();
    kill_letbind_13();
    kill_letbind_12();
    kill_letbind_11();
    kill_letbind_10();
    kill_letbind_9();
    kill_letbind_8();
    kill_letbind_7();
    kill_letbind_6();
    kill_letbind_5();
    kill_letbind_4();
    kill_letbind_3();
    kill_letbind_2();
    kill_letbind_1();
    kill_letbind_0();
    cleanup_rts();
}
