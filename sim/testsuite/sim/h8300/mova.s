# Hitachi H8 testcase 'mova'
# mach(): h8sx
# as(h8300):	--defsym sim_cpu=0
# as(h8300h):	--defsym sim_cpu=1
# as(h8300s):	--defsym sim_cpu=2
# as(h8sx):	--defsym sim_cpu=3
# ld(h8300h):	-m h8300helf
# ld(h8300s):	-m h8300self
# ld(h8sx):	-m h8300sxelf

	.include "testutils.inc"

	start

movabl16_reg8:
	set_grs_a5a5
	set_ccr_zero

	mova/b.l	@(1:16, r2l.b), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0xa6 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movabl16_reg16:
	set_grs_a5a5
	set_ccr_zero

	mova/b.l	@(1:16, r2.w), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0xa5a6 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movabl32_reg8:
	set_grs_a5a5
	set_ccr_zero

	mova/b.l	@(1:32, r2l.b), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0xa6 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movabl32_reg16:
	set_grs_a5a5
	set_ccr_zero

	mova/b.l	@(1:32, r2.w), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0xa5a6 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movawl16_reg8:
	set_grs_a5a5
	set_ccr_zero

	mova/w.l	@(1:16, r2l.b), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x14b er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movawl16_reg16:
	set_grs_a5a5
	set_ccr_zero

	mova/w.l	@(1:16, r2.w), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x14b4b er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movawl32_reg8:
	set_grs_a5a5
	set_ccr_zero

	mova/w.l	@(1:32, r2l.b), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x14b er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movawl32_reg16:
	set_grs_a5a5
	set_ccr_zero

	mova/w.l	@(1:32, r2.w), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x14b4b er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movall16_reg8:
	set_grs_a5a5
	set_ccr_zero

	mova/l.l	@(1:16, r2l.b), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x295 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movall16_reg16:
	set_grs_a5a5
	set_ccr_zero

	mova/l.l	@(1:16, r2.w), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x29695 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movall32_reg8:
	set_grs_a5a5
	set_ccr_zero

	mova/l.l	@(1:32, r2l.b), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x295 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

movall32_reg16:
	set_grs_a5a5
	set_ccr_zero

	mova/l.l	@(1:32, r2.w), er3

	test_cc_clear
	test_gr_a5a5	0	; Make sure other regs not affected
	test_gr_a5a5	1
	test_gr_a5a5	2
	test_h_gr32	0x29695 er3
	test_gr_a5a5	4
	test_gr_a5a5	5
	test_gr_a5a5	6
	test_gr_a5a5	7

	pass

	exit 0
