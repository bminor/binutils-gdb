.text
.align 0
	
	mrs	r8, CPSR
	mrseq	r9, CPSR_all
	mrs	r1, CPSR_flg
	mrs	r2, SPSR

	msr	CPSR, r1
	msrne	CPSR_flg, #0xf0000000
	msr	SPSR_flg, r8
	msr	SPSR_all, r9

