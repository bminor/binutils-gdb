;# { dg-do assemble }
;# { dg-skip-if "DWARF2 output not supported" { mcore-*-* mn10*-*-* moxie-*-* v850-*-* } }
;# { dg-skip-if "no NOP opcode" { mmix-*-* openrisc-*-* or32-*-* } }
;# { dg-options "--gdwarf2 --defsym ia64=0" }
;# { dg-options "--gdwarf2 --defsym ia64=1" { target ia64-*-* i370-*-* } }
	.offset 40
.ifeq ia64 - 1
	nop 0
.else
	nop
.endif
;# { dg-warning "Warning: dwarf line number information for .* ignored" "" { target *-*-* } 0 }
