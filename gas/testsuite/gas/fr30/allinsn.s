 .data
foodata: .word 42
 .text
footext:
	.text
	.global add
add:
	add r0, r1
	add #0, r2
	.text
	.global add2
add2:
	add2 #-1, r3
	.text
	.global addc
addc:
	addc r4, r5
	.text
	.global addn
addn:
	addn r6, r7
	addn #15, r8
	.text
	.global addn2
addn2:
	addn2 #-16, r9
	.text
	.global sub
sub:
	sub r10, r11
	.text
	.global subc
subc:
	subc r12, r13
	.text
	.global subn
subn:
	subn r14, r15
	.text
	.global cmp
cmp:
	cmp ac, fp
	cmp #1, sp
	.text
	.global cmp2
cmp2:
	cmp2 #-15, r0
	.text
	.global and
and:
	and r1, r2
	and r3, @r4
	.text
	.global andh
andh:
	andh r5, @r6
	.text
	.global andb
andb:
	andb r7, @r8
	.text
	.global or
or:
	or r9, r10
	or r11, @r12
	.text
	.global orh
orh:
	orh r13, @r14
	.text
	.global orb
orb:
	orb r15, @ac
	.text
	.global eor
eor:
	eor fp, sp
	eor r0, @r1
	.text
	.global eorh
eorh:
	eorh r2, @r3
	.text
	.global eorb
eorb:
	eorb r4, @r5
	.text
	.global bandl
bandl:
	bandl #16, @ r6
	.text
	.global bandh
nadh:
	bandh #7, @r7
	.text
	.global borl
borl:
	borl #3, @r8
	.text
	.global borh
borh:
	borh #13, @r9
	.text
	.global beorl
beorl:
	beorl #15, @r10
	.text
	.global beorh
beorh:
	beorh #1, @r11
	.text
	.global btstl
btstl:
	btstl #0, r12
	.text
	.global btsth
btsth:
	btsth #8, r13
	.text
	.global mul
mul:
	mul r14, r15
	.text
	.global mulu
mulu:
	mulu ac, fp
	.text
	.global muluh
muluh:	
	muluh sp, r0
	.text
	.global mulh
mulh:	
	mulh r1, r2
	.text
	.global div0s
div0s:
	div0s r3
	.text
	.global div0u
div0u:
	div0u r4
	.text
	.global div1
div1:
	div1 r5
	.text
	.global div2
div2:
	div2 r6
	.text
	.global div3
div3:
	div3
	.text
	.global div4s
div4s:
	div4s
	.text
	.global lsl
lsl:
	lsl r7, r8
	lsl #3, r9
	.text
	.global lsl2
lsl2:
	lsl2 #0, r10
	.text
	.global lsr
lsr:
	lsr r11, r12
	lsr #15, r13
	.text
	.global lsr2
lsr2:
	lsr2 #15, r14
	.text
	.global asr
asr:
	asr r15, ac
	asr #6, fp
	.text
	.global asr2
asr2:
	asr2 #7, sp
	.text
	.global ldi_32
ldi_32:
	ldi:32 #0x12345678, r0
	.text
	.global ldi_20
ldi_20:
	ldi:20 #0x000fffff, r1
	.text
	.global ldi_8
ldi_8:
	ldi:8 #0xff, r2
	.text
	.global ld
ld:
	ld @r3, r4
	ld @(r13, r5), r6
	ld @(r14, #0x1fc), r7
	ld @(r15, #0x3c), r8
	ld @r15+, r9
	ld @r15+, pc
	ld @r15+, ps
	ld @r15+, tbr
	ld @r15+, rp
	ld @r15+, ssp
	.text
	.global lduh
lduh:
	lduh $r10, r11
	lduh @(r13, r12), r13
	lduh @(r14, -256), r15
	.text
	.global ldub
ldub:
	ldub @ac, fp
	ldub @(r13, sp), r0
	ldub @(r14, -128), r1
	.text
	.global st
st:
	st r2, @r3
	st r4, @(r13, r5)
	st r6, @(r14, -512)
	st r7, @(r15, 0x3c)
	st r8, @ - r15
	st mdh, @-r15
	st ps, @ - r15
	.text
	.global lsth
sth:
	sth r9, @r10
	sth r11, @(r13, r12)
	sth r13, @(r14, 128)
	.text
	.global stb
stb:
	stb r14, @r15
	stb r0, @(r13, r1)
	stb r2, @(r14, -128)
	.text
	.global mov
mov:
	mov r3, r4
	mov mdl, r5
	mov ps, r6
	mov r7, usp
	mov r8, ps
	.text
	.global jmp
jmp:
	jmp @r9
	.text
	.global call
call:
	call footext
	call @r10
	.text
	.global ret
ret:
	ret
	.text
	.global int
int:
	int #255
	.text
	.global inte
inte:
	inte
	.text
	.global reti
reti:
	reti
	.text
	.global bra
bra:
	bra footext
	.text
	.global bno
bno:
	bno footext
	.text
	.global beq
beq:
	beq footext
	.text
	.global bne
bne:
	bne footext
	.text
	.global bc
bc:
	bc footext
	.text
	.global bnc
bnc:
	bnc footext
	.text
	.global bn
bn:
	bn footext
	.text
	.global bp
bp:
	bp footext
	.text
	.global bv
bv:
	bv footext
	.text
	.global bnv
bnv:
	bnv footext
	.text
	.global blt
blt:
	blt footext
	.text
	.global bge
bge:
	bge footext
	.text
	.global ble
ble:
	ble footext
	.text
	.global bgt
bgt:
	bgt footext
	.text
	.global bls
bls:
	bls footext
	.text
	.global bhi
bhi:
	bhi footext
	.text
	.global jmp_d
jmp_d:
	jmp:d @r11
	nop
	.text
	.global call_d
call_d:
	call:d footext
	nop
	call:d @r12
	nop
	.text
	.global ret_d
ret_d:
	ret:d
	nop
	.text
	.global bra_d
bra_d:
	bra:d footext
	.text
	.global bno_d
bno_d:
	bno:d footext
	.text
	.global beq_d
beq_d:
	beq:d footext
	.text
	.global bne_d
bne_d:
	bne:d footext
	.text
	.global bc_d
bc_d:
	bc:d footext
	.text
	.global bnc_d
bnc_d:
	bnc:d footext
	.text
	.global bn_d
bn_d:
	bn:d footext
	.text
	.global bp_d
bp_d:
	bp:d footext
	.text
	.global bv_d
bv_d:
	bv:d footext
	.text
	.global bnv_d
bnv_d:
	bnv:d footext
	.text
	.global blt_d
blt_d:
	blt:d footext
	.text
	.global bge_d
bge_d:
	bge:d footext
	.text
	.global ble_d
ble_d:
	ble:d footext
	.text
	.global bgt_d
bgt_d:
	bgt:d footext
	.text
	.global bls_d
bls_d:
	bls:d footext
	.text
	.global bhi_d
bhi_d:
	bhi:d footext
	.text
	.global dmov
dmov:
	dmov @88H, r13
	dmov r13, @54H
	dmov @0x44, r13+
	dmov @r13+, @2
	dmov @2cH, @-r15
	dmov @r15+, @38
	.text
	.global dmovh
dmovh:
	dmovh @88H, r13
	dmovh r13, @52h
	dmovh @0x34, @r13 +
	dmovh @r13+, @0x52
	.text
	.global dmovb
dmovb:
	dmovb @91H, r13
	dmovb r13, @0x53
	dmovb @71, r13+
	dmovb @r13+, @0
	.text
	.global ldres
ldres:
	ldres @r2+, #8
	.text
	.global stres
stres:
	stres #15, @r3+
	.text
	.global copop
copop:
	copop #15, #1, cr3, cr4
	copop #15, #4, cr5, cr6
	copop #15, #255, cr7, cr0
	.text
	.global copld
copld:
	copld #0, #0, r4, cr0
	.text
	.global copst
copst:
	copst #7, #2, cr1, r5
	.text
	.global copsv
copsv:
	copsv #8, #3, cr2, r6
	.text
	.global nop
nop:
	nop
	.text
	.global andccr
andccr:
	andccr #255
	.text
	.global orccr
orccr:
	orccr #125
	.text
	.global stilm
stilm:
	stilm #97
	.text
	.global addsp
addsp:
	addsp #-512
	.text
	.global extsb
extsb:
	extsb r9
	.text
	.global extub
extub:
	extub r10
	.text
	.global extsh
extsh:
	extsh r11
	.text
	.global extuh
extuh:
	extuh r12
	.text
	.global ldm0
ldm0:
	ldm0 (r0, r2, r3, r7)
	.text
	.global ldm1
ldm1:
	ldm1 (r8, r11, sp)
	.text
	.global stm0
stm0:
	stm0 (r2, r3)
	.text
	.global stm1
stm1:
	stm1 (ac, fp)
	.text
	.global enter
enter:
	enter #1020
	.text
	.global leave
leave:
	leave 
	.text
	.global xchb
xchb:
	xchb @r14, r15
