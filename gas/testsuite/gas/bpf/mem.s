# eBPF tests for MEM instructions, modulus lddw.

        .text

 	ldabsw %r2, %r3, 0xbeef
	ldabsh %r4, %r5, 0xbeef
 	ldabsb %r6, %r7, 0xbeef
 	ldabsdw %r8, %r9, 0xbeef
	ldindw %r2, %r3, 0xbeef
 	ldindh %r4, %r5, 0xbeef
 	ldindb %r6, %r7, 0xbeef
 	ldinddw %r8, %r9, 0xbeef
 	ldxw %r2, [%r1+0x7eef]
 	ldxh %r2, [%r1+0x7eef]
 	ldxb %r2, [%r1+0x7eef]
 	ldxdw %r2, [%r1+-2]
 	stxw [%r1+0x7eef], %r2
 	stxh [%r1+0x7eef], %r2
 	stxb [%r1+0x7eef], %r2
 	stxdw [%r1+-2], %r2
 	stb [%r1+0x7eef], 0x11223344
 	sth [%r1+0x7eef], 0x11223344
 	stw [%r1+0x7eef], 0x11223344
 	stdw [%r1+-2], 0x11223344
