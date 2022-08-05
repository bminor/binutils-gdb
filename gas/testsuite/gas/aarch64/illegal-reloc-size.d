#name: Illegal :size*: relocations.
#as: -march=morello+c64
#source: illegal-reloc-size.s
#error: [^ :]+: Assembler messages:
#error: [^ :]+:[0-9]+: Error: bad expression at operand 2 -- `mov x12,#:size_g0:sym1'
#error: [^ :]+:[0-9]+: Error: bad expression at operand 2 -- `mov x12,#:size_g0_nc:sym1'
#error: [^ :]+:[0-9]+: Error: this relocation modifier is not allowed on this instruction at operand 3 -- `add x12,x12,#:size_g0:sym1'
#error: [^ :]+:[0-9]+: Error: this relocation modifier is not allowed on this instruction at operand 3 -- `add x12,x12,#:size_g0_nc:sym1'
#error: [^ :]+:[0-9]+: Error: bad expression at operand 2 -- `mov x12,#:size_g1:sym1'
#error: [^ :]+:[0-9]+: Error: bad expression at operand 2 -- `mov x12,#:size_g1_nc:sym1'
#error: [^ :]+:[0-9]+: Error: this relocation modifier is not allowed on this instruction at operand 3 -- `add x12,x12,#:size_g1:sym1'
#error: [^ :]+:[0-9]+: Error: this relocation modifier is not allowed on this instruction at operand 3 -- `add x12,x12,#:size_g1_nc:sym1'
#error: [^ :]+:[0-9]+: Error: bad expression at operand 2 -- `mov x12,#:size_g2:sym1'
#error: [^ :]+:[0-9]+: Error: bad expression at operand 2 -- `mov x12,#:size_g2_nc:sym1'
#error: [^ :]+:[0-9]+: Error: this relocation modifier is not allowed on this instruction at operand 3 -- `add x12,x12,#:size_g2:sym1'
#error: [^ :]+:[0-9]+: Error: this relocation modifier is not allowed on this instruction at operand 3 -- `add x12,x12,#:size_g2_nc:sym1'
#error: [^ :]+:[0-9]+: Error: bad expression at operand 2 -- `mov x12,#:size_g3:sym1'
#error: [^ :]+:[0-9]+: Error: this relocation modifier is not allowed on this instruction at operand 3 -- `add x12,x12,#:size_g3:sym1'
