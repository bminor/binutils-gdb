	comment "subroutine prologue"
	.macro gdbasm_enter
	st r13,@-sp
	.endm

	comment "subroutine epilogue"
	.macro gdbasm_leave
	ld r13,@sp+
	jmp r13
	.endm

	.macro gdbasm_call subr
	bl \subr
	.endm

	.macro gdbasm_several_nops
	nop
	nop
	nop
	nop
	.endm

	comment "exit (0)"
	.macro gdbasm_exit0
	ldi r4, 1
	ldi r0, 0
	trap 15
	.endm

	comment "crt0 startup"
	.macro gdbasm_startup
	.endm
