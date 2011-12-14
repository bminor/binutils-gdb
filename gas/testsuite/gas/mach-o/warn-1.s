# { dg-do assemble }

	.lcomm align_too_big,9,100

# { dg-warning "Warning: Alignment (100) too large: 15 assumed." "" { xfail *-*-darwin* } 3 }
