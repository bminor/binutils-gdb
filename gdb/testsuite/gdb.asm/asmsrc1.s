	.include "common.inc"
	.include "arch.inc"

comment "WARNING: asm-source.exp checks for line numbers printed by gdb."
comment "Be careful about changing this file without also changing"
comment "asm-source.exp."

	
comment	"This file is not linked with crt0."
comment	"Provide very simplistic equivalent."
	
	.global _start
_start:
	startup
	call main
	exit0


comment "main routine for assembly source debugging test"
comment "This particular testcase uses macros in <arch>.inc to achieve"
comment "machine independence."

	.global main
main:
	enter

comment "Call a macro that consists of several lines of assembler code."

	several_nops

comment "Call a subroutine in another file."

	call foo2

comment "All done."

	exit0

comment "A routine for foo2 to call."

	.global foo3
foo3:
	enter
	leave

	.global exit
exit:
	exit0
