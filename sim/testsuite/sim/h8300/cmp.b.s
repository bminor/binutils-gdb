# Hitachi H8 testcase 'cmp.b'
# mach(): all
# as(h8300):	--defsym sim_cpu=0
# as(h8300h):	--defsym sim_cpu=1
# as(h8300s):	--defsym sim_cpu=2
# as(h8sx):	--defsym sim_cpu=3
# ld(h8300h):	-m h8300helf	
# ld(h8300s):	-m h8300self	
# ld(h8sx):	-m h8300sxelf

	.include "testutils.inc"

	# Instructions tested:
	# cmp.b #xx:8, rd	;                     a rd   xxxxxxxx
	# cmp.b #xx:8, @erd	;         7 d rd ???? a ???? xxxxxxxx
	# cmp.b #xx:8, @erd+	; 0 1 7 4 6 c rd 1??? a ???? xxxxxxxx
	# cmp.b #xx:8, @erd-	; 0 1 7 6 6 c rd 1??? a ???? xxxxxxxx
	# cmp.b #xx:8, @+erd	; 0 1 7 5 6 c rd 1??? a ???? xxxxxxxx
	# cmp.b #xx:8, @-erd	; 0 1 7 7 6 c rd 1??? a ???? xxxxxxxx
	# cmp.b rs, rd		;                     1 c rs rd
	# cmp.b reg8, @erd	;         7 d rd ???? 1 c rs ????
	# cmp.b reg8, @erd+	;         0 1 7     9 8 rd 2 rs
	# cmp.b reg8, @erd-	;         0 1 7     9 a rd 2 rs
	# cmp.b reg8, @+erd	;         0 1 7     9 9 rd 2 rs
	# cmp.b reg8, @-erd	;         0 1 7     9 b rd 2 rs
	#

	# Coming soon:
	# ...

.data
pre_byte:	.byte 0
byte_dest:	.byte 0xa5
post_byte:	.byte 0

	start
	
cmp_b_imm8_reg8:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	;;  fixme set ccr

	;;  cmp.b #xx:8,Rd
	cmp.b	#0xa5, r0l	; Immediate 8-bit src, reg8 dest
	beq	.Leq1
	fail
.Leq1:	cmp.b	#0xa6, r0l
	blt	.Llt1
	fail
.Llt1:	cmp.b	#0xa4, r0l
	bgt	.Lgt1
	fail
.Lgt1:	
	;; fixme test ccr	; H=0 N=1 Z=0 V=0 C=0
	test_h_gr16 0xa5a5 r0	; r0 unchanged
.if (sim_cpu)			; non-zero means h8300h, s, or sx
	test_h_gr32 0xa5a5a5a5 er0	; er0 unchanged
.endif
	test_gr_a5a5 1		; Make sure other general regs not disturbed
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7
	
.if (sim_cpu == h8sx)
cmp_b_imm8_rdind:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b #xx:8,@eRd
	mov	#byte_dest, er0
	cmp.b	#0xa5:8, @er0	; Immediate 8-bit src, reg indirect dst
;;; 	.word	0x7d00
;;; 	.word	0xa0a5
	beq	.Leq2
	fail
.Leq2:	set_ccr_zero
	cmp.b	#0xa6, @er0
;;; 	.word	0x7d00
;;; 	.word	0xa0a6
	blt	.Llt2
	fail
.Llt2:	set_ccr_zero
	cmp.b	#0xa4, @er0
;;; 	.word	0x7d00
;;; 	.word	0xa0a4
	bgt	.Lgt2
	fail
.Lgt2:		
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear
	
	test_h_gr32 byte_dest er0	; er0 still contains address
	test_gr_a5a5 1		; Make sure other general regs not disturbed
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L2
	fail
.L2:

cmp_b_imm8_rdpostinc:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b #xx:8,@eRd+
	mov	#byte_dest, er0
	cmp.b	#0xa5:8, @er0+	; Immediate 8-bit src, reg postinc dst
;;; 	.word	0x0174
;;; 	.word	0x6c08
;;; 	.word	0xa0a5
	beq	.Leq3
	fail
.Leq3:	test_h_gr32 post_byte er0	; er0 contains address plus one
	mov	#byte_dest, er0
	set_ccr_zero
	cmp.b	#0xa6, @er0+
;;; 	.word	0x0174
;;; 	.word	0x6c08
;;; 	.word	0xa0a6
	blt	.Llt3
	fail
.Llt3:	test_h_gr32 post_byte er0	; er0 contains address plus one
	mov	#byte_dest, er0
	set_ccr_zero
	cmp.b	#0xa4, @er0+
;;; 	.word	0x0174
;;; 	.word	0x6c08
;;; 	.word	0xa0a4
	bgt	.Lgt3
	fail
.Lgt3:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 post_byte er0	; er0 contains address plus one
	test_gr_a5a5 1		; Make sure other general regs not disturbed
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L3
	fail
.L3:

cmp_b_imm8_rdpostdec:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b #xx:8,@eRd-
	mov	#byte_dest, er0
	cmp.b	#0xa5:8, @er0-	; Immediate 8-bit src, reg postdec dst
;;; 	.word	0x0176
;;; 	.word	0x6c08
;;; 	.word	0xa0a5
	beq	.Leq4
	fail
.Leq4:	test_h_gr32 pre_byte er0	; er0 contains address minus one
	mov	#byte_dest, er0
	set_ccr_zero
	cmp.b	#0xa6, @er0-
;;; 	.word	0x0176
;;; 	.word	0x6c08
;;; 	.word	0xa0a6
	blt	.Llt4
	fail
.Llt4:	test_h_gr32 pre_byte er0	; er0 contains address minus one
	mov	#byte_dest, er0
	set_ccr_zero
	cmp.b	#0xa4, @er0-
;;; 	.word	0x0176
;;; 	.word	0x6c08
;;; 	.word	0xa0a4
	bgt	.Lgt4
	fail
.Lgt4:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 pre_byte er0	; er0 contains address minus one
	test_gr_a5a5 1		; Make sure other general regs not disturbed
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L4
	fail
.L4:

cmp_b_imm8_rdpreinc:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b #xx:8,@+eRd
	mov	#pre_byte, er0
	cmp.b	#0xa5:8, @+er0	; Immediate 8-bit src, reg pre-inc dst
;;; 	.word	0x0175
;;; 	.word	0x6c08
;;; 	.word	0xa0a5
	beq	.Leq5
	fail
.Leq5:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#pre_byte, er0
	set_ccr_zero
	cmp.b	#0xa6, @+er0
;;; 	.word	0x0175
;;; 	.word	0x6c08
;;; 	.word	0xa0a6
	blt	.Llt5
	fail
.Llt5:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#pre_byte, er0
	set_ccr_zero
	cmp.b	#0xa4, @+er0
;;; 	.word	0x0175
;;; 	.word	0x6c08
;;; 	.word	0xa0a4
	bgt	.Lgt5
	fail
.Lgt5:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 byte_dest er0	; er0 contains destination address 
	test_gr_a5a5 1		; Make sure other general regs not disturbed
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L5
	fail
.L5:

cmp_b_imm8_rdpredec:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b #xx:8,@-eRd
	mov	#post_byte, er0
	cmp.b	#0xa5:8, @-er0	; Immediate 8-bit src, reg pre-dec dst
;;; 	.word	0x0177
;;; 	.word	0x6c08
;;; 	.word	0xa0a5
	beq	.Leq6
	fail
.Leq6:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#post_byte, er0
	set_ccr_zero
	cmp.b	#0xa6, @-er0
;;; 	.word	0x0177
;;; 	.word	0x6c08
;;; 	.word	0xa0a6
	blt	.Llt6
	fail
.Llt6:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#post_byte, er0
	set_ccr_zero
	cmp.b	#0xa4, @-er0
;;; 	.word	0x0177
;;; 	.word	0x6c08
;;; 	.word	0xa0a4
	bgt	.Lgt6
	fail
.Lgt6:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 byte_dest er0	; er0 contains destination address 
	test_gr_a5a5 1		; Make sure other general regs not disturbed
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L6
	fail
.L6:


.endif

cmp_b_reg8_reg8:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	;;  fixme set ccr

	;;  cmp.b Rs,Rd
	mov.b	#0xa5, r0h
	cmp.b	r0h, r0l	; Reg8 src, reg8 dst
	beq	.Leq7
	fail
.Leq7:	mov.b	#0xa6, r0h
	cmp.b	r0h, r0l
	blt	.Llt7
	fail
.Llt7:	mov.b	#0xa4, r0h
	cmp.b	r0h, r0l
	bgt	.Lgt7
	fail
.Lgt7:
	;; fixme test ccr	; H=0 N=1 Z=0 V=0 C=0
	test_h_gr16 0xa4a5 r0	; r0l unchanged.
.if (sim_cpu)			; non-zero means h8300h, s, or sx
	test_h_gr32 0xa5a5a4a5 er0	; r0l unchanged
.endif
	test_gr_a5a5 1		; Make sure other general regs not disturbed
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7
	
.if (sim_cpu == h8sx)
cmp_b_reg8_rdind:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b rs8,@eRd	; cmp reg8 to register indirect
	mov	#byte_dest, er0
	mov	#0xa5, r1l
	cmp.b	r1l, @er0	; reg8 src, reg indirect dest
;;; 	.word	0x7d00
;;; 	.word	0x1c90
	beq	.Leq8
	fail
.Leq8:	set_ccr_zero
	mov	#0xa6, r1l
	cmp.b	r1l, @er0
;;; 	.word	0x7d00
;;; 	.word	0x1c90
	blt	.Llt8
	fail
.Llt8:	set_ccr_zero
	mov	#0xa4, r1l
	cmp.b	r1l, @er0
;;; 	.word	0x7d00
;;; 	.word	0x1c90
	bgt	.Lgt8
	fail
.Lgt8:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 byte_dest er0	; er0 still contains address
	test_h_gr32 0xa5a5a5a4 er1	; er1 has the test load

	test_gr_a5a5 2		; Make sure other general regs not disturbed
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (no change).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L8
	fail
.L8:

cmp_b_reg8_rdpostinc:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b reg8,@eRd+
	mov	#byte_dest, er0
	mov	#0xa5, r1l
	cmp.b	r1l, @er0+	; Immediate 8-bit src, reg post-incr dst
;;; 	.word	0x0179
;;; 	.word	0x8029
	beq	.Leq9
	fail
.Leq9:	test_h_gr32 post_byte er0	; er0 contains address plus one
	mov	#byte_dest er0
	mov	#0xa6, r1l
	set_ccr_zero
	cmp.b	r1l, @er0+
;;; 	.word	0x0179
;;; 	.word	0x8029
	blt	.Llt9
	fail
.Llt9:	test_h_gr32 post_byte er0	; er0 contains address plus one
	mov	#byte_dest er0
	mov	#0xa4, r1l
	set_ccr_zero
	cmp.b	r1l, @er0+
;;; 	.word	0x0179
;;; 	.word	0x8029
	bgt	.Lgt9
	fail
.Lgt9:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 post_byte er0	; er0 contains address plus one
	test_h_gr32 0xa5a5a5a4 er1	; er1 contains test load
	test_gr_a5a5 2		; Make sure other general regs not disturbed
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L9
	fail
.L9:

cmp_b_reg8_rdpostdec:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b reg8,@eRd-
	mov	#byte_dest, er0
	mov	#0xa5, r1l
	cmp.b	r1l, @er0-	; Immediate 8-bit src, reg postdec dst
;;; 	.word	0x0179
;;; 	.word	0xa029
	beq	.Leq10
	fail
.Leq10:	test_h_gr32 pre_byte er0	; er0 contains address minus one
	mov	#byte_dest er0
	mov	#0xa6, r1l
	set_ccr_zero
	cmp.b	r1l, @er0-
;;; 	.word	0x0179
;;; 	.word	0xa029
	blt	.Llt10
	fail
.Llt10:	test_h_gr32 pre_byte er0	; er0 contains address minus one
	mov	#byte_dest er0
	mov	#0xa4, r1l
	set_ccr_zero
	cmp.b	r1l, @er0-
;;; 	.word	0x0179
;;; 	.word	0xa029
	bgt	.Lgt10
	fail
.Lgt10:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 pre_byte er0	; er0 contains address minus one
	test_h_gr32 0xa5a5a5a4 er1	; er1 contains test load
	test_gr_a5a5 2		; Make sure other general regs not disturbed
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L10
	fail
.L10:

cmp_b_reg8_rdpreinc:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b reg8,@+eRd
	mov	#pre_byte, er0
	mov	#0xa5, r1l
	cmp.b	r1l, @+er0	; Immediate 8-bit src, reg post-incr dst
;;; 	.word	0x0179
;;; 	.word	0x9029
	beq	.Leq11
	fail
.Leq11:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#pre_byte er0
	mov	#0xa6, r1l
	set_ccr_zero
	cmp.b	r1l, @+er0
;;; 	.word	0x0179
;;; 	.word	0x9029
	blt	.Llt11
	fail
.Llt11:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#pre_byte er0
	mov	#0xa4, r1l
	set_ccr_zero
	cmp.b	r1l, @+er0
;;; 	.word	0x0179
;;; 	.word	0x9029
	bgt	.Lgt11
	fail
.Lgt11:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 byte_dest er0	; er0 contains destination address 
	test_h_gr32 0xa5a5a5a4 er1	; er1 contains test load
	test_gr_a5a5 2		; Make sure other general regs not disturbed
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L11
	fail
.L11:

cmp_b_reg8_rdpredec:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  cmp.b reg8,@-eRd
	mov	#post_byte, er0
	mov	#0xa5, r1l
	cmp.b	r1l, @-er0	; Immediate 8-bit src, reg postdec dst
;;; 	.word	0x0179
;;; 	.word	0xb029
	beq	.Leq12
	fail
.Leq12:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#post_byte er0
	mov	#0xa6, r1l
	set_ccr_zero
	cmp.b	r1l, @-er0
;;; 	.word	0x0179
;;; 	.word	0xb029
	blt	.Llt12
	fail
.Llt12:	test_h_gr32 byte_dest er0	; er0 contains destination address 
	mov	#post_byte er0
	mov	#0xa4, r1l
	set_ccr_zero
	cmp.b	r1l, @-er0
;;; 	.word	0x0179
;;; 	.word	0xb029
	bgt	.Lgt12
	fail
.Lgt12:
	test_carry_clear	; H=0 N=0 Z=0 V=0 C=0
	test_ovf_clear
	test_zero_clear
	test_neg_clear

	test_h_gr32 byte_dest er0	; er0 contains destination address 
	test_h_gr32 0xa5a5a5a4 er1	; er1 contains test load
	test_gr_a5a5 2		; Make sure other general regs not disturbed
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

	;; Now check the result of the cmp to memory (memory unchanged).
	sub.b	r0l, r0l
	mov.b	@byte_dest, r0l
	cmp.b	#0xa5, r0l
	beq	.L12
	fail
.L12:

.endif
	pass

	exit 0
