
	comment "subroutine prologue"
	.macro gdbasm_enter
	st.a    blink,[sp,-4]
	st.a	fp, [sp,-4]
	mov	fp,sp
	.endm

	comment "subroutine epilogue"
	.macro gdbasm_leave
	ld.ab  fp, [sp,4]
	ld     blink,[sp,0]
	j.d    [blink]
	add    sp,sp,4
	.endm

	.macro gdbasm_call subr
	bl	\subr
	.endm

	.macro gdbasm_several_nops
	nop
	nop
	nop
	nop
	.endm

	comment "exit (0)"
	.macro gdbasm_exit0
	mov_s    r0,0
	trap_s	 0
	.endm

	comment "crt0 startup"
	.macro gdbasm_startup
	mov    fp, 0
	.endm

	comment "Declare a data variable"
	.macro gdbasm_datavar name value
	.data
\name:
	.long	\value
	.endm

	comment "Declare the start of a subroutine"
	.macro gdbasm_declare name
	.type	\name, @function
\name:
	.endm

	comment "End a subroutine"
	.macro gdbasm_end name
	.size	\name, .-name
	.endm
