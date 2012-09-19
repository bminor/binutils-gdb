#name: aarch64-emit-relocs-309-low
#source: emit-relocs-309.s
#as:
#ld: -Ttext 0xFFFFC --section-start .got=0x0
#objdump: -dr
#...

Disassembly of section \.text:

00000000000ffffc <_start>:
   ffffc:	d503201f 	nop
  100000:	58800000 	ldr	x0, 0 .*
