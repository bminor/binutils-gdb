#objdump: -dr --prefix-addresses
#name: NIOS2 movi

# Test implicit conversion of movi/movhi etc
.*:     file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> movi	r2,32
0+0004 <[^>]*> movhi	r2,8192
0+0008 <[^>]*> movhi	r2,65535
0+000c <[^>]*> orhi	r2,r5,65535
0+0010 <[^>]*> xorhi	r2,r10,65535
0+0014 <[^>]*> andhi	r2,r15,65535
