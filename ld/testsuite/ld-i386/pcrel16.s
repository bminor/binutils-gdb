 .text
 .code16
 .global _start, fwd
_start:
	jpo	fwd
 .rept 2500
	testl $0x12345678, %ss:0x76543210(,%eax,4)
 .endr
fwd:
	leave
	ret
