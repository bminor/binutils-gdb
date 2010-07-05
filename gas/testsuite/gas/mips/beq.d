#objdump: -dr --prefix-addresses -mmips:4000
#name: MIPS beq
#as: -32

# Test the beq macro.

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> beq	a0,a1,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> beqz	a0,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> li	at,1
[0-9a-f]+ <[^>]*> beq	a0,at,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> li	at,0x8000
[0-9a-f]+ <[^>]*> beq	a0,at,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> li	at,-32768
[0-9a-f]+ <[^>]*> beq	a0,at,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> beq	a0,at,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> lui	at,0x1
[0-9a-f]+ <[^>]*> ori	at,at,0xa5a5
[0-9a-f]+ <[^>]*> beq	a0,at,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> bnez	a0,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> beqzl	a0,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> bnezl	a0,0+0000 <.*>
[0-9a-f]+ <[^>]*> nop
	\.\.\.
[0-9a-f]+ <[^>]*> j	0+0000 <.*>
[ 	]*20068: (MIPS_JMP|JMPADDR|R_MIPS_26)	\.text
[0-9a-f]+ <[^>]*> nop
[0-9a-f]+ <[^>]*> jal	0+0000 <.*>
[ 	]*20070: (MIPS_JMP|JMPADDR|R_MIPS_26)	\.text
[0-9a-f]+ <[^>]*> nop
#[0-9a-f]+ <[^>]*> b	0+20078 <.*\+0x20078>
#[ 	]*20078: .*16	external_label
#[0-9a-f]+ <[^>]*> nop
#[0-9a-f]+ <[^>]*> bal	0+20080 <.*\+0x20080>
#[ 	]*20080: .*16	external_label
#[0-9a-f]+ <[^>]*> nop
	\.\.\.
