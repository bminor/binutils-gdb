#source: ifunc-22.s
#objdump: -s -j .got
#ld: -static
#target: aarch64*-*-*

# Ensure GOT is populated correctly in static link

.*:     file format elf64-(little|big)aarch64

Contents of section \.got:
 4100f0 00000000 00000000 d0004000 00000000  ..........@.....
