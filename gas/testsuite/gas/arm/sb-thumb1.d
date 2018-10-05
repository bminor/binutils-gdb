#objdump: -dr --prefix-addresses --show-raw-insn
#name: SB instruction (Thumb)
#source: sb.s
#as: -march=armv8.5-a -mthumb

# Test SB Instructio

.*: *file format .*arm.*

Disassembly of section .text:
.*> f3bf 8f70 	sb
