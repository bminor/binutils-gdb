#name: Z80/eZ80 arch combination test
#source: dummy1.s -z80
#source: dummy2.s -ez80
#ld: -e 0
#objdump: -f

.*:[     ]+file format (coff)|(elf32)\-z80
architecture: ez80-z80, flags 0x[0-9a-fA-F]+:
.*
.*

