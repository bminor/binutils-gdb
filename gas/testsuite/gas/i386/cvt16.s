# Check CVT16 instructions (maxcombos=16, maxops=3, archbits=32, seed=1)

	.allow_index_reg
	.text
_start:

# Tests for op VCVTPH2PS imm8, xmm2/mem64, xmm1  (at&t syntax)
    VCVTPH2PS      $0x0,%xmm7,%xmm7
    VCVTPH2PS      $0x0,(%ebx),%xmm7
    VCVTPH2PS      $0x0,%xmm0,%xmm5
    VCVTPH2PS      $0xFF,%xmm5,%xmm0
    VCVTPH2PS      $0x3,%xmm0,%xmm0
    VCVTPH2PS      $0x3,%xmm7,%xmm0
    VCVTPH2PS      $0x0,%xmm5,%xmm5
    VCVTPH2PS      $0x0,%xmm0,%xmm7
    VCVTPH2PS      $0x3,(%eax),%xmm0
    VCVTPH2PS      $0xFF,(%ebx),%xmm0
    VCVTPH2PS      $0x0,(%eax),%xmm7
    VCVTPH2PS      $0xFF,%xmm7,%xmm7
    VCVTPH2PS      $0xFF,%xmm5,%xmm5
    VCVTPH2PS      $0xFF,(%ebx),%xmm5
    VCVTPH2PS      $0xFF,%xmm7,%xmm0
    VCVTPH2PS      $0x3,(%eax),%xmm7
# Tests for op VCVTPH2PS imm8, xmm2/mem128, ymm1  (at&t syntax)
    VCVTPH2PS      $0x0,%xmm7,%ymm7
    VCVTPH2PS      $0x0,(%ebx),%ymm7
    VCVTPH2PS      $0x0,%xmm0,%ymm5
    VCVTPH2PS      $0xFF,%xmm5,%ymm0
    VCVTPH2PS      $0x3,%xmm0,%ymm0
    VCVTPH2PS      $0x3,%xmm7,%ymm0
    VCVTPH2PS      $0x0,%xmm5,%ymm5
    VCVTPH2PS      $0x0,%xmm0,%ymm7
    VCVTPH2PS      $0x3,(%eax),%ymm0
    VCVTPH2PS      $0xFF,(%ebx),%ymm0
    VCVTPH2PS      $0x0,(%eax),%ymm7
    VCVTPH2PS      $0xFF,%xmm7,%ymm7
    VCVTPH2PS      $0xFF,%xmm5,%ymm5
    VCVTPH2PS      $0xFF,(%ebx),%ymm5
    VCVTPH2PS      $0xFF,%xmm7,%ymm0
    VCVTPH2PS      $0x3,(%eax),%ymm7
# Tests for op VCVTPS2PH imm8, xmm2, xmm1/mem64  (at&t syntax)
    VCVTPS2PH      $0x0,%xmm5,(%ebx)
    VCVTPS2PH      $0x0,%xmm7,(%esi)
    VCVTPS2PH      $0x0,%xmm0,(%eax)
    VCVTPS2PH      $0xFF,%xmm5,%xmm2
    VCVTPS2PH      $0x3,%xmm0,%xmm2
    VCVTPS2PH      $0x3,%xmm5,%xmm2
    VCVTPS2PH      $0x0,%xmm0,%xmm7
    VCVTPS2PH      $0x0,%xmm0,(%esi)
    VCVTPS2PH      $0xFF,%xmm7,%xmm0
    VCVTPS2PH      $0x0,%xmm7,(%ebx)
    VCVTPS2PH      $0xFF,%xmm5,(%ebx)
    VCVTPS2PH      $0xFF,%xmm0,(%eax)
    VCVTPS2PH      $0xFF,%xmm7,(%eax)
    VCVTPS2PH      $0x3,%xmm7,(%ebx)
    VCVTPS2PH      $0x3,%xmm5,(%eax)
    VCVTPS2PH      $0xFF,%xmm5,%xmm7
# Tests for op VCVTPS2PH imm8, ymm2, xmm1/mem128  (at&t syntax)
    VCVTPS2PH      $0x0,%ymm5,(%ebx)
    VCVTPS2PH      $0x0,%ymm7,(%esi)
    VCVTPS2PH      $0x0,%ymm0,(%eax)
    VCVTPS2PH      $0xFF,%ymm5,%xmm2
    VCVTPS2PH      $0x3,%ymm0,%xmm2
    VCVTPS2PH      $0x3,%ymm5,%xmm2
    VCVTPS2PH      $0x0,%ymm0,%xmm7
    VCVTPS2PH      $0x0,%ymm0,(%esi)
    VCVTPS2PH      $0xFF,%ymm7,%xmm0
    VCVTPS2PH      $0x0,%ymm7,(%ebx)
    VCVTPS2PH      $0xFF,%ymm5,(%ebx)
    VCVTPS2PH      $0xFF,%ymm0,(%eax)
    VCVTPS2PH      $0xFF,%ymm7,(%eax)
    VCVTPS2PH      $0x3,%ymm7,(%ebx)
    VCVTPS2PH      $0x3,%ymm5,(%eax)
    VCVTPS2PH      $0xFF,%ymm5,%xmm7
