# name: Macro scrubbing
# as:
# objdump: -dr --prefix-addresses --show-raw-insn

[^:]+: +file format .*arm.*

Disassembly of section .text:

0+0 <[^>]*> e8bd8030 ?	ldmia	sp!, {r4, r5, pc}
