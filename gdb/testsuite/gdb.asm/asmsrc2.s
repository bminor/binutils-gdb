	.include "common.inc"
	.include "arch.inc"

comment "Second file in assembly source debugging testcase."

	.global foo2
foo2:
	gdbasm_enter

comment "Call someplace else."

	gdbasm_call foo3

comment "All done, return."

	gdbasm_leave
