; Test program to tic80 alu operations
; compile with tic80-coff-gcc -nostartfiles -nostdlib

	.macro	write string
	.section .const
1:	.ascii	"\string\n"
2:
	.section .text
	addu	1,r0,r2			; filedes
	addu	1b,r0,r4		; string
	addu	2b-1b,r0,r6		; length
	addu	4,r0,r15		; write system call #
	trap	72
	.endm

	.macro	expect reg, value, string
	xor	\value,\reg,r19		; produces r19 == 0 if matches expected value
	bcnd.a	3f,r19,eq0.w
	.section .const
1:	.ascii	"Expected \value for \string\n"
2:
	.section .text
	addu	1,r0,r2			; filedes
	addu	1b,r0,r4		; string
	addu	2b-1b,r0,r6		; length
	addu	4,r0,r15		; write system call #
	trap	72

	ld	_errors(r0),r19
	addu	1,r19,r19
	st	_errors(r0),r19
3:
	.endm

	.globl	__start
	.globl	_errors
	.data
_errors:
	.long	0

	.text

__start:
	write	"Test add"
	addu	0x12345678,r0,r7
	addu	0x12345678,r0,r8
	add	r7,r8,r9
	expect	r9,0x2468acf0,"add #1"

	addu	0x54545478,r0,r8
	add	0x0001,r8,r9
	expect	r9,0x54545479,"add #2"

	addu	0x12345678,r0,r8
	add	0x12345678,r8,r9
	expect	r9,0x2468acf0,"add #3"

; This will cause overflow
;	add	0x789abcde,r8,r9
;	expect	r9,0x8acf1356,"add #4"

	write	"Test addu"
	addu	0x12345678,r0,r7
	addu	0x12345678,r0,r8
	addu	r7,r8,r9
	expect	r9,0x2468acf0,"addu #1"

	addu	0x54545478,r0,r8
	addu	0x0001,r8,r9
	expect	r9,0x54545479,"addu #2"

	addu	0x12345678,r0,r8
	addu	0x12345678,r8,r9
	expect	r9,0x2468acf0,"addu #3"

	addu	0x789abcde,r8,r9
	expect	r9,0x8acf1356,"addu #4"

	write	"Test and"
	addu	0xf,r0,r7
	addu	0x12345678,r0,r8
	and	r7,r8,r9
	expect	r9,0x8,"and #1"

	and	0x7fff,r8,r9
	expect	r9,0x5678,"add #2"

	and	0xff0000ff,r8,r9
	expect	r9,0x12000078,"add #3"

	write	"Test and.tt"
	addu	0xf,r0,r7
	addu	0x12345678,r0,r8
	and.tt	r7,r8,r9
	expect	r9,0x8,"and.tt #1"

	and.tt	0x7fff,r8,r9
	expect	r9,0x5678,"add.tt #2"

	and.tt	0xff0000ff,r8,r9
	expect	r9,0x12000078,"add.tt #3"

	write	"Test and.ff"
	addu	0xf,r0,r7
	addu	0x12345678,r0,r8
	and.ff	r7,r8,r9
	expect	r9,0xedcba980,"and.ff #1"

	and.ff	0x7fff,r8,r9
	expect	r9,0xedcb8000,"add.ff #2"

	and.ff	0xff0000ff,r8,r9
	expect	r9,0x00cba900,"add.ff #3"

	write	"Test and.ft"
	addu	0xf,r0,r7
	addu	0x12345678,r0,r8
	and.ft	r7,r8,r9
	expect	r9,0x12345670,"and.ft #1"

	and.ft	0x7fff,r8,r9
	expect	r9,0x12340000,"add.ft #2"

	and.ft	0xff0000ff,r8,r9
	expect	r9,0x00345600,"add.ft #3"

	write	"Test and.tf"
	addu	0xf,r0,r7
	addu	0x12345678,r0,r8
	and.tf	r7,r8,r9
	expect	r9,0x7,"and.tf #1"

	and.tf	0x7fff,r8,r9
	expect	r9,0x2987,"add.tf #2"

	and.tf	0xff0000ff,r8,r9
	expect	r9,0xed000087,"add.tf #3"

	write	"Test cmp"
	addu	0x12345678,r0,r7
	addu	0x54545478,r0,r8
	cmp	r7,r8,r9
	expect	r9,0x19a99aa9,"cmp #1"

	cmp	0xd3c4,r8,r9
	expect	r9,0x19a96a5a,"cmp #2"

	cmp	0x54545478,r7,r9
	expect	r9,0x26666aa9,"cmp #3"

	cmp	0x13c4,r8,r9
	expect	r9,0x19a66a5a,"cmp #4"

	write	"Test exts"
	addu	5,r0,r4
	addu	8,r0,r5
	addu	0x10203040,r0,r7
	exts	5,8,r7,r9
	expect	r9,0xffffff82,"exts #1"

	exts	r4,8,r7,r9
	expect	r9,0xffffff82,"exts #2"

	addu	0x0f0e0d0c,r0,r9
	exts	5,8,r9,r7
	expect	r7,0x00000068,"exts #3"

	exts	r4,8,r9,r7
	expect	r7,0x00000068,"exts #4"

	write	"Test extu"
	addu	5,r0,r4
	addu	8,r0,r5
	addu	0x10203040,r0,r7
	extu	5,8,r7,r9
	expect	r9,0x00000082,"extu #1"

	extu	r4,8,r7,r9
	expect	r9,0x00000082,"extu #2"

	addu	0x0f0e0d0c,r0,r9
	extu	5,8,r9,r7
	expect	r7,0x00000068,"extu #3"

	extu	r4,8,r9,r7
	expect	r7,0x00000068,"extu #4"

	write	"Test ins"
	addu	4,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	ins	4,11,r7,r9
	expect	r9,0xfedcbe78,"ins #1"

	addu	0xfedcba98,r0,r9
	ins	r4,11,r7,r9
	expect	r9,0xfedcbe78,"ins #2"

	addu	0xfedcba98,r0,r9
	ins	4,11,r9,r7
	expect	r7,0x01234187,"ins #3"

	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	ins	r4,11,r9,r7
	expect	r7,0x01234187,"ins #4"

	write	"Test lmo"
	addu	0x0089abc,r0,r7
	lmo	r7,r9
	expect	r9,0xc,"lmo #1"

	lmo	r0,r9
	expect	r9,32,"lmo #2"

	write	"Test nop"
	nop

	write	"Test or"
	addu	0x0000000f,r0,r7
	addu	0x12345678,r0,r8
	or	r7,r8,r9
	expect	r9,0x1234567f,"or #1"

	or	0x7fff,r8,r9
	expect	r9,0x12347fff,"or #2"

	or	0xff0000ff,r8,r9
	expect	r9,0xff3456ff,"or #3"

	write	"Test or.tt"
	addu	0x0000000f,r0,r7
	addu	0x12345678,r0,r8
	or.tt	r7,r8,r9
	expect	r9,0x1234567f,"or.tt #1"

	or.tt	0x7fff,r8,r9
	expect	r9,0x12347fff,"or.tt #2"

	or.tt	0xff0000ff,r8,r9
	expect	r9,0xff3456ff,"or.tt #3"

	write	"Test or.ff"
	addu	0x0000000f,r0,r7
	addu	0x12345678,r0,r8
	or.ff	r7,r8,r9
	expect	r9,0xfffffff7,"or.ff #1"

	or.ff	0x7fff,r8,r9
	expect	r9,0xffffa987,"or.ff #2"

	or.ff	0xff0000ff,r8,r9
	expect	r9,0xedffff87,"or.ff #3"

	write	"Test or.ft"
	addu	0x0000000f,r0,r7
	addu	0x12345678,r0,r8
	or.ft	r7,r8,r9
	expect	r9,0xfffffff8,"or.ft #1"

	or.ft	0x7fff,r8,r9
	expect	r9,0xffffd678,"or.ft #2"

	or.ft	0xff0000ff,r8,r9
	expect	r9,0x12ffff78,"or.ft #3"

	write	"Test or.tf"
	addu	0x0000000f,r0,r7
	addu	0x12345678,r0,r8
	or.tf	r7,r8,r9
	expect	r9,0xedcba98f,"or.tf #1"

	or.tf	0x7fff,r8,r9
	expect	r9,0xedcbffff,"or.tf #2"

	or.tf	0xff0000ff,r8,r9
	expect	r9,0xffcba9ff,"or.tf #3"

	write	"Test rmo"
	addu	0xdcba9800,r0,r7
	rmo	r7,r9
	expect	r9,0x14,"rmo #1"

	rmo	r0,r9
	expect	r9,32,"rmo #2"

	write	"Test rotl"
	addu	27,r0,r4
	addu	0x1234567,r0,r7
	rotl	27,32,r7,r9
	expect	r9,0x38091a2b,"rotl #1"

	rotl	r4,32,r7,r9
	expect	r9,0x38091a2b,"rotl #2"

	write	"Test rotr"
	addu	5,r0,r4
	addu	0x1234567,r0,r7
	rotr	5,32,r7,r9
	expect	r9,0x38091a2b,"rotr #1"

	rotr	r4,32,r7,r9
	expect	r9,0x38091a2b,"rotr #2"

	write	"Test shl"
	addu	5,r0,r4
	addu	0x1234567,r0,r7
	shl	5,32,r7,r9
	expect	r9,0x2468ace0,"shl #1"

	shl	r4,32,r7,r9
	expect	r9,0x2468ace0,"shl #2"

	write	"Test sl"
	addu	4,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	sl.im	4,11,r7,r9
	expect	r9,0xfedcbe78,"sl #1"

	addu	0xfedcba98,r0,r9
	sl.im	r4,11,r7,r9
	expect	r9,0xfedcbe78,"sl #2"

	addu	0xfedcba98,r0,r9
	sl.im	4,11,r9,r7
	expect	r7,0x01234187,"sl #3"

	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	sl.im	r4,11,r9,r7
	expect	r7,0x01234187,"sl #4"

	addu	27,r0,r4
	addu	0x1234567,r0,r7
	sl.dz	27,32,r7,r9
	expect	r9,0x38091a2b,"sl #5"

	sl.dz	r4,32,r7,r9
	expect	r9,0x38091a2b,"sl #6"

	addu	5,r0,r4
	addu	0x1234567,r0,r7
	sl.iz	5,32,r7,r9
	expect	r9,0x2468ace0,"sl #7"

	sl.iz	r4,32,r7,r9
	expect	r9,0x2468ace0,"sl #8"

	write	"Test sli"
	addu	4,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	sli.im	4,11,r7,r9
	expect	r9,0x12345298,"sli #1"

	addu	0xfedcba98,r0,r9
	sli.im	r4,11,r7,r9
	expect	r9,0x12345298,"sli #2"

	addu	0xfedcba98,r0,r9
	sli.im	4,11,r9,r7
	expect	r7,0xedcbad67,"sli #3"

	addu	0x1234567,r0,r7
	sli.im	r4,11,r9,r7
	expect	r7,0xedcbad67,"sli #4"

	write	"Test sr"
	addu	5,r0,r4
	addu	0x1234567,r0,r7
	sr.dz	5,32,r7,r9
	expect	r9,0x38091a2b,"sr #1"

	sr.dz	r4,32,r7,r9
	expect	r9,0x38091a2b,"sr #2"

	addu	5,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	sr.es	5,32,r7,r9
	expect	r9,0x00091a2b,"sr #3"

	sr.es	r4,32,r7,r9
	expect	r9,0x00091a2b,"sr #4"

	addu	0xfedcba98,r0,r9
	sr.es	5,32,r9,r7
	expect	r7,0xfff6e5d4,"sr #5"

	addu	0x1234567,r0,r7
	sr.es	r4,32,r9,r7
	expect	r7,0xfff6e5d4,"sr #6"

	addu	5,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	sr.ez	5,32,r7,r9
	expect	r9,0x00091a2b,"sr #7"

	sr.ez	r4,32,r7,r9
	expect	r9,0x00091a2b,"sr #8"

	addu	0xfedcba98,r0,r9
	sr.ez	5,32,r9,r7
	expect	r7,0x07f6e5d4,"sr #9"

	addu	0x1234567,r0,r7
	sr.ez	r4,32,r9,r7
	expect	r7,0x07f6e5d4,"sr #10"

	addu	5,r0,r4
	addu	8,r0,r5
	addu	0x10203040,r0,r7
	sr.ds	5,8,r7,r9
	expect	r9,0xffffff82,"sr #11"

	sr.ds	r4,8,r7,r9
	expect	r9,0xffffff82,"sr #12"

	addu	0x0f0e0d0c,r0,r9
	sr.ds	5,8,r9,r7
	expect	r7,0x00000068,"sr #13"

	sr.ds	r4,8,r9,r7
	expect	r7,0x00000068,"sr #14"

	addu	5,r0,r4
	addu	8,r0,r5
	addu	0x10203040,r0,r7
	sr.dz	5,8,r7,r9
	expect	r9,0x00000082,"sr #15"

	sr.dz	r4,8,r7,r9
	expect	r9,0x00000082,"sr #16"

	addu	0x0f0e0d0c,r0,r9
	sr.dz	5,8,r9,r7
	expect	r7,0x00000068,"sr #17"

	sr.dz	r4,8,r9,r7
	expect	r7,0x00000068,"sr #18"

	write	"Test sra"
	addu	5,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	sra	5,32,r7,r9
	expect	r9,0x00091a2b,"sra #1"

	sra	r4,32,r7,r9
	expect	r9,0x00091a2b,"sra #2"

	addu	0xfedcba98,r0,r9
	sra	5,32,r9,r7
	expect	r7,0xfff6e5d4,"sra #3"

	addu	0x1234567,r0,r7
	sra	r4,32,r9,r7
	expect	r7,0xfff6e5d4,"sra #4"

	addu	0xaabbccdd, r0, r4
	sra	0,0,r4,r2
	expect	r2, 0xaabbccdd, "sra #5"

	write	"Test sri"
	addu	5,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	sri.em	5,11,r7,r9
	expect	r9,0xf8091a98,"sri #1"

	sri.em	r4,11,r7,r9
	expect	r9,0xf8091a98,"sri #2"

	write	"Test srl"
	addu	5,r0,r4
	addu	0x1234567,r0,r7
	addu	0xfedcba98,r0,r9
	srl	5,32,r7,r9
	expect	r9,0x00091a2b,"srl #1"

	srl	r4,32,r7,r9
	expect	r9,0x00091a2b,"srl #2"

	addu	0xfedcba98,r0,r9
	srl	5,32,r9,r7
	expect	r7,0x07f6e5d4,"srl #3"

	addu	0x1234567,r0,r7
	srl	r4,32,r9,r7
	expect	r7,0x07f6e5d4,"srl #4"

	write	"Test sub"
	addu	0x12345678,r0,r7
	addu	0x12345677,r0,r8
	sub	r7,r8,r9
	expect	r9,1,"sub #1"

	addu	1,r0,r8
	sub	2,r8,r9
	expect	r9,1,"sub #2"

	addu	0x12345677,r0,r8
	sub	0x12345678,r8,r9
	expect	r9,1,"sub #3"

; This overflows
;	addu	0,r0,r7
;	addu	0x80000000,r0,r8
;	sub	r7,r8,r9
;	expect	r9,0x80000000,"sub #4"

	write	"Test subu"
	addu	0x12345678,r0,r7
	addu	0x12345677,r0,r8
	subu	r7,r8,r9
	expect	r9,1,"subu #1"

	addu	1,r0,r8
	subu	2,r8,r9
	expect	r9,1,"subu #2"

	addu	0x12345677,r0,r8
	subu	0x12345678,r8,r9
	expect	r9,1,"subu #3"

	addu	0,r0,r7
	addu	0x80000000,r0,r8
	subu	r7,r8,r9
	expect	r9,0x80000000,"subu #4"

	write	"Test xnor"
	addu	0xf,r0,r7
	addu	0x12345678,r0,r8
	xnor	r7,r8,r9
	expect	r9,0xedcba988,"xnor #1"

	xnor	0x7fff,r8,r9
	expect	r9,0xedcbd678,"xnor #2"

	xnor	0xff0000ff,r8,r9
	expect	r9,0x12cba978,"xnor #3"

	write	"Test xor"
	addu	0xf,r0,r7
	addu	0x12345678,r0,r8
	xor	r7,r8,r9
	expect	r9,0x12345677,"xor #1"

	xor	0x7fff,r8,r9
	expect	r9,0x12342987,"xor #2"

	xor	0xff0000ff,r8,r9
	expect	r9,0xed345687,"xor #3"

	write	"Done"
	ld	_errors(r0),r2		; # of errors
	addu	1,r0,r15		; exit system call number
	trap	72


