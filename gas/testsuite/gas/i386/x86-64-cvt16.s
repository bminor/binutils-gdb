# Check CVT16 instructions (maxcombos=16, maxops=3, archbits=64, seed=1)

	.allow_index_reg
	.text
_start:

# Tests for op VCVTPH2PS imm8, xmm2/mem64, xmm1  (at&t syntax)
    VCVTPH2PS      $0x0,%xmm15,%xmm15
    VCVTPH2PS      $0x0,(%rsi),%xmm15
    VCVTPH2PS      $0x0,%xmm0,%xmm11
    VCVTPH2PS      $0xFF,%xmm15,%xmm0
    VCVTPH2PS      $0x3,%xmm0,%xmm0
    VCVTPH2PS      $0x3,%xmm15,%xmm0
    VCVTPH2PS      $0x0,%xmm11,%xmm11
    VCVTPH2PS      $0x0,%xmm0,%xmm15
    VCVTPH2PS      $0x3,(%rcx),%xmm0
    VCVTPH2PS      $0xFF,(%rsi),%xmm0
    VCVTPH2PS      $0x0,(%rdi),%xmm15
    VCVTPH2PS      $0xFF,%xmm15,%xmm15
    VCVTPH2PS      $0xFF,%xmm11,%xmm11
    VCVTPH2PS      $0xFF,(%rsi),%xmm11
    VCVTPH2PS      $0x3,(%rdi),%xmm15
    VCVTPH2PS      $0x3,%xmm15,%xmm11
# Tests for op VCVTPH2PS imm8, xmm2/mem128, ymm1  (at&t syntax)
    VCVTPH2PS      $0x0,%xmm15,%ymm15
    VCVTPH2PS      $0x0,(%rsi),%ymm15
    VCVTPH2PS      $0x0,%xmm0,%ymm11
    VCVTPH2PS      $0xFF,%xmm15,%ymm0
    VCVTPH2PS      $0x3,%xmm0,%ymm0
    VCVTPH2PS      $0x3,%xmm15,%ymm0
    VCVTPH2PS      $0x0,%xmm11,%ymm11
    VCVTPH2PS      $0x0,%xmm0,%ymm15
    VCVTPH2PS      $0x3,(%rcx),%ymm0
    VCVTPH2PS      $0xFF,(%rsi),%ymm0
    VCVTPH2PS      $0x0,(%rdi),%ymm15
    VCVTPH2PS      $0xFF,%xmm15,%ymm15
    VCVTPH2PS      $0xFF,%xmm11,%ymm11
    VCVTPH2PS      $0xFF,(%rsi),%ymm11
    VCVTPH2PS      $0x3,(%rdi),%ymm15
    VCVTPH2PS      $0x3,%xmm15,%ymm11
# Tests for op VCVTPS2PH imm8, xmm2, xmm1/mem64  (at&t syntax)
    VCVTPS2PH      $0x0,%xmm11,(%rax)
    VCVTPS2PH      $0x0,%xmm15,(%rdi)
    VCVTPS2PH      $0x0,%xmm0,(%r12)
    VCVTPS2PH      $0xFF,%xmm11,%xmm15
    VCVTPS2PH      $0x3,%xmm0,%xmm15
    VCVTPS2PH      $0x3,%xmm11,%xmm15
    VCVTPS2PH      $0x0,%xmm0,%xmm4
    VCVTPS2PH      $0x0,%xmm0,(%rdi)
    VCVTPS2PH      $0xFF,%xmm15,%xmm0
    VCVTPS2PH      $0x0,%xmm15,(%rax)
    VCVTPS2PH      $0xFF,%xmm11,(%rax)
    VCVTPS2PH      $0xFF,%xmm0,(%r12)
    VCVTPS2PH      $0xFF,%xmm15,(%r12)
    VCVTPS2PH      $0x3,%xmm15,(%rax)
    VCVTPS2PH      $0x3,%xmm11,(%r12)
    VCVTPS2PH      $0xFF,%xmm11,%xmm4
# Tests for op VCVTPS2PH imm8, ymm2, xmm1/mem128  (at&t syntax)
    VCVTPS2PH      $0x0,%ymm11,(%rax)
    VCVTPS2PH      $0x0,%ymm15,(%rdi)
    VCVTPS2PH      $0x0,%ymm0,(%r12)
    VCVTPS2PH      $0xFF,%ymm11,%xmm15
    VCVTPS2PH      $0x3,%ymm0,%xmm15
    VCVTPS2PH      $0x3,%ymm11,%xmm15
    VCVTPS2PH      $0x0,%ymm0,%xmm4
    VCVTPS2PH      $0x0,%ymm0,(%rdi)
    VCVTPS2PH      $0xFF,%ymm15,%xmm0
    VCVTPS2PH      $0x0,%ymm15,(%rax)
    VCVTPS2PH      $0xFF,%ymm11,(%rax)
    VCVTPS2PH      $0xFF,%ymm0,(%r12)
    VCVTPS2PH      $0xFF,%ymm15,(%r12)
    VCVTPS2PH      $0x3,%ymm15,(%rax)
    VCVTPS2PH      $0x3,%ymm11,(%r12)
    VCVTPS2PH      $0xFF,%ymm11,%xmm4
