# Hitachi H8 testcase 'jmp'
# mach(): all
# as(h8300):	--defsym sim_cpu=0
# as(h8300h):	--defsym sim_cpu=1
# as(h8300s):	--defsym sim_cpu=2
# as(h8sx):	--defsym sim_cpu=3
# ld(h8300h):	-m h8300helf
# ld(h8300s):	-m h8300self
# ld(h8sx):	-m h8300sxelf

	.include "testutils.inc"

	start
	
.if 0				; this one isn't right -- it's an indirect
jmp_8:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  jmp @aa:8		; 8-bit displacement
	jmp @@.Ltgt_8:8
	fail

.Ltgt_8:	
	test_cc_clear
	test_gr_a5a5 0		; Make sure other general regs not disturbed
	test_gr_a5a5 1
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7
.endif

jmp_24:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  jmp @aa:24		; 24-bit address
	jmp @.Ltgt_24:24
	fail

.Ltgt_24:	
	test_cc_clear
	test_gr_a5a5 0		; Make sure other general regs not disturbed
	test_gr_a5a5 1
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7

.if	(sim_cpu)		; Non-zero means h8300h, h8300s, or h8sx
jmp_reg:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  jmp @ern		; register indirect
	mov.l	#.Ltgt_reg, er5
	jmp	@er5
	fail
	
.Ltgt_reg:
	test_cc_clear
	test_gr_a5a5 0		; Make sure other general regs not disturbed
	test_gr_a5a5 1
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_h_gr32 .Ltgt_reg er5
	test_gr_a5a5 6
	test_gr_a5a5 7
.endif

.if (sim_cpu == h8sx)
jmp_32:
	set_grs_a5a5		; Fill all general regs with a fixed pattern
	set_ccr_zero

	;;  jmp @aa:32		; 32-bit address
;	jmp @.Ltgt_32:32	; NOTE:	hard-coded to avoid relaxing
	.word	0x5908
	.long	.Ltgt_32
	fail

.Ltgt_32:	
	test_cc_clear
	test_gr_a5a5 0		; Make sure other general regs not disturbed
	test_gr_a5a5 1
	test_gr_a5a5 2
	test_gr_a5a5 3
	test_gr_a5a5 4
	test_gr_a5a5 5
	test_gr_a5a5 6
	test_gr_a5a5 7
.endif

	pass
	exit 0

	