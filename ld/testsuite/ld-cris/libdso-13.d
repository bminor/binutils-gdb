#source: dso-1.s
#source: dsov32-3.s
#as: --pic --no-underscore --march=v32
#ld: --shared -m crislinux
#readelf: -d -r
#error: relocation R_CRIS_32_PCREL should not be used in a shared object; recompile with -fPIC

# FIXME: Have a textrel-enabling linker option.  Split out the
# expected readelf output into a separate test using that option.
# Check that a TEXTREL reloc is correctly generated for PCREL
# relocations.

Dynamic segment at offset 0x240 contains 10 entries:
  Tag[ 	]+Type[ 	]+Name/Value
 0x0+4 \(HASH\)[ 	]+0x94
 0x0+5 \(STRTAB\)[ 	]+0x1e8
 0x0+6 \(SYMTAB\)[ 	]+0xe8
 0x0+a \(STRSZ\)[ 	]+47 \(bytes\)
 0x0+b \(SYMENT\)[ 	]+16 \(bytes\)
 0x0+7 \(RELA\)[ 	]+0x218
 0x0+8 \(RELASZ\)[ 	]+12 \(bytes\)
 0x0+9 \(RELAENT\)[ 	]+12 \(bytes\)
 0x0+16 \(TEXTREL\)[ 	]+0x0
 0x0+ \(NULL\)[ 	]+0x0

Relocation section '\.rela\.text' at offset 0x218 contains 1 entries:
 Offset[ 	]+Info[ 	]+Type[ 	]+Sym\.Value  Sym\. Name \+ Addend
0+22a  0+f06 R_CRIS_32_PCREL[ 	]+0+224[ 	]+dsofn \+ 6
