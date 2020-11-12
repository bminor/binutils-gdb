;# { dg-do assemble }
;# { dg-options "--gdwarf2" }

	.offset 40
	.nop

;# { dg-warning "Warning: dwarf line number information for .* ignored" "" { xfail v850*-*-* } 0 }
