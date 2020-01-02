#name: Z180 arch test
#source: dummy1.s -z180
#source: dummy2.s -z180
#ld: -e 0
#objdump: -f

.*:[     ]+file format (coff)|(elf32)\-z80
architecture: z180, flags 0x[0-9a-fA-F]+:
.*
.*

