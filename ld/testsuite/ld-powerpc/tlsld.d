#source: tlsld.s
#as: -a64
#ld: -melf64ppc
#objdump: -dr
#target: powerpc64*-*-*

.*:     file format .*

Disassembly of section \.text:

.*:
.*	nop
.*	addis   r29,r13,0
.*	mr      r3,r29
.*	addi    r3,r3,4096
.*	nop
.*	addis   r3,r3,0
.*	ld      r3,-32768\(r3\)
.*	nop
.*	addis   r29,r13,0
.*	mr      r3,r29
.*	addi    r3,r3,4096
.*	nop
.*	ld      r3,-32768\(r3\)
.*	nop
.*	nop
.*	nop
.*	nop
.*	addis   r29,r13,0
.*	mr      r3,r29
.*	addi    r3,r3,-28672
.*	nop
.*	ld      r3,0\(r3\)
.*	nop
.*	nop
.*	addis   r29,r13,0
.*	mr      r3,r29
.*	addi    r3,r3,-28672
.*	nop
.*	ld      r3,0\(r3\)
.*	nop
.*	nop
.*	nop
