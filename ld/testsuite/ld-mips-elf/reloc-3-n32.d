#name: R_MIPS16_GPREL reloc n32
#source: ../../../gas/testsuite/gas/mips/elf-rel6.s
#as: -march=mips64 -mabi=n32 -EB
#objdump: --prefix-addresses -tdr --show-raw-insn
#ld: -Ttext 0x500000 -e 0x500000 -N -melf32btsmipn32


.*:     file format elf.*mips.*

#...

Disassembly of section \.text:
0+500000 <[^>]*> f010 8352 	lb	v0,-32750\(v1\)
0+500004 <[^>]*> f010 8353 	lb	v0,-32749\(v1\)
0+500008 <[^>]*> f252 8346 	lb	v0,-28090\(v1\)
0+50000c <[^>]*> 6500      	nop
0+50000e <[^>]*> 6500      	nop
#pass
