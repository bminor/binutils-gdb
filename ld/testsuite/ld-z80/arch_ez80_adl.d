#name: eZ80 ADL mode arch test
#source: dummy1.s -ez80-adl
#source: dummy2.s -ez80-adl
#ld: -e 0
#objdump: -f

.*:[     ]+file format (coff)|(elf32)\-z80
architecture: ez80-adl, flags 0x[0-9a-fA-F]+:
.*
.*

