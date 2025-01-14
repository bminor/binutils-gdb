# Check Invalid 64bit AMX-MOVRS instructions

	.text
_start:
	t2rpntlvwz0rs	(%rip), %tmm2
	t2rpntlvwz0rst1	(%rip), %tmm2
	t2rpntlvwz1rs	(%rip), %tmm2
	t2rpntlvwz1rst1	(%rip), %tmm2
	t2rpntlvwz0rs	(%r9), %tmm1
	t2rpntlvwz0rst1	(%r9), %tmm3
	t2rpntlvwz1rs	(%r9), %tmm5
	t2rpntlvwz1rst1	(%r9), %tmm7

	.arch .noamx_transpose
_transpose:
	t2rpntlvwz0rs	(%r9), %tmm2
	t2rpntlvwz0rst1	(%r9), %tmm2
	t2rpntlvwz1rs	(%r9), %tmm2
	t2rpntlvwz1rst1	(%r9), %tmm2
