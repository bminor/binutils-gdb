# Check 64bit AMX-MOVRS instructions

	.text
_start:
	t2rpntlvwz0rs	0x10000000(%rbp, %r14, 8), %tmm6
	t2rpntlvwz0rs	(%r9), %tmm2
	t2rpntlvwz0rst1	0x10000000(%rbp, %r14, 8), %tmm6
	t2rpntlvwz0rst1	(%r9), %tmm2
	t2rpntlvwz1rs	0x10000000(%rbp, %r14, 8), %tmm6
	t2rpntlvwz1rs	(%r9), %tmm2
	t2rpntlvwz1rst1	0x10000000(%rbp, %r14, 8), %tmm6
	t2rpntlvwz1rst1	(%r9), %tmm2
	tileloaddrs	0x10000000(%rbp, %r14, 8), %tmm6
	tileloaddrs	(%r9), %tmm3
	tileloaddrst1	0x10000000(%rbp, %r14, 8), %tmm6
	tileloaddrst1	(%r9), %tmm3

_intel:
	.intel_syntax noprefix
	t2rpntlvwz0rs	tmm6, [rbp+r14*8+0x10000000]
	t2rpntlvwz0rs	tmm2, [r9]
	t2rpntlvwz0rst1	tmm6, [rbp+r14*8+0x10000000]
	t2rpntlvwz0rst1	tmm2, [r9]
	t2rpntlvwz1rs	tmm6, [rbp+r14*8+0x10000000]
	t2rpntlvwz1rs	tmm2, [r9]
	t2rpntlvwz1rst1	tmm6, [rbp+r14*8+0x10000000]
	t2rpntlvwz1rst1	tmm2, [r9]
	tileloaddrs	tmm6, [rbp+r14*8+0x10000000]
	tileloaddrs	tmm3, [r9]
	tileloaddrst1	tmm6, [rbp+r14*8+0x10000000]
	tileloaddrst1	tmm3, [r9]
