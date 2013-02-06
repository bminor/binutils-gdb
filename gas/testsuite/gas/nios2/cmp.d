#objdump: -dr --prefix-addresses 
#name: NIOS2 cmp

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> cmpeq	r11,r2,r3
0+0004 <[^>]*> cmpge	r11,r2,r3
0+0008 <[^>]*> cmpgeu	r11,r2,r3
0+000c <[^>]*> cmplt	r11,r2,r3
0+0010 <[^>]*> cmpltu	r11,r2,r3
0+0014 <[^>]*> cmpne	r11,r2,r3
0+0018 <[^>]*> cmpgei	r11,r2,0
[	]*18: R_NIOS2_S16	value
0+001c <[^>]*> cmpgeui	r11,r2,0
[	]*1c: R_NIOS2_U16	value\+0x200
0+0020 <[^>]*> cmplti	r11,r2,0
[	]*20: R_NIOS2_S16	value
0+0024 <[^>]*> cmpltui	r11,r2,0
[	]*24: R_NIOS2_U16	value\+0x200
0+0028 <[^>]*> cmpgei	r11,r2,32767
0+002c <[^>]*> cmpgeui	r11,r2,32768
0+0030 <[^>]*> cmplti	r11,r2,-32768
0+0034 <[^>]*> cmpltui	r11,r2,65535
