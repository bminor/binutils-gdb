# Check Illegal prefix for 64bit EVEX-promoted instructions

        .allow_index_reg
        .text
_start:
	#movbe %r23w,%ax set EVEX.pp = f3.
	.insn EVEX.L0.f3.M12.W0 0x60, %di, %ax

	#movbe %r23w,%ax set EVEX.pp = f2.
	.insn EVEX.L0.f2.M12.W0 0x60, %di, %ax

	#VSIB vpgatherqq (%rbp,%zmm17,8),%zmm16{%k1} set EVEX.P[10] == 0
	.byte 0x62, 0xe2, 0xf9, 0x41, 0x91, 0x84, 0xcd
	.byte 0xff

	#EVEX_MAP4 movbe %r23w,%ax set EVEX.mm == 0b01.
	.insn EVEX.L0.66.M13.W0 0x60, %di, %ax

	#EVEX_MAP4 movbe %r23w,%ax set EVEX.aaa[1:0] (P[17:16]) == 0b01
	.insn EVEX.L0.66.M12.W0 0x60, %di, %ax{%k1}

	#EVEX_MAP4 movbe %r18w,%ax set EVEX.L'L == 0b01.
	.insn EVEX.L1.66.M12.W0 0x60, %di, %ax

	#EVEX_MAP4 movbe %r18w,%ax set EVEX.z == 0b1.
	.insn EVEX.L0.66.M12.W0 0x60, %di, %ax {%k7}{z}

	#EVEX from VEX bzhi %rax,(%rax,%rbx),%rcx EVEX.aaa[1:0] (P[17:16])
	#== 0b01
	.insn EVEX.L0.NP.0f38.W1 0xf5, %rax, (%rax,%rbx), %rcx{%k1}

	#EVEX from VEX bzhi %rax,(%rax,%rbx),%ecx EVEX.P[22:21](EVEX.L’L) == 0b01
	.insn EVEX.L1.NP.0f38.W1 0xf5, %rax, (%rax,%rbx), %rcx

	#EVEX from VEX bzhi %rax,(%rax,%rbx),%rcx EVEX.P[23](EVEX.z) == 0b1
	.insn EVEX.L0.NP.0f38.W1 0xf5, %rax, (%rax,%rbx), %rcx {%k7}{z}

	#EVEX from VEX bzhi %rax,(%rax,%rbx),%rcx EVEX.P[20](EVEX.b) == 0b1
	.insn EVEX.L0.NP.0f38.W1 0xf5, %rax, (%rax,%rbx){1to8}, %rcx

	#{evex} inc %rax %rbx EVEX.vvvv != 1111 && EVEX.ND = 0.
	.insn EVEX.L0.NP.M4.W1 0xff/0, (%rax,%rcx), %rbx
