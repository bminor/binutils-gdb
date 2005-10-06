#objdump: -dr --prefix-addresses --show-raw-insn
#as: --allow-reg-prefix
#name: SH --allow-reg-prefix option
# Test SH register names prefixed with $:

.*:     file format elf.*sh.*

Disassembly of section .text:
0x00000000 01 63       	movli.l	@r1,r0
