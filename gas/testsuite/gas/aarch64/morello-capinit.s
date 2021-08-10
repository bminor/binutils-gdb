.section .rodata
str2:
	.string "Hello World"
	.size str2, .-str2

.data
.globl str
str:
	.string "Hello World"
	.size str, .-str

.align 4
a:
	.capinit str
	.8byte 0
	.8byte 0
	.size a, .-a

b:
	.capinit str+8
	.8byte 0
	.8byte 0x10
	.size b, .-b

c:
	.capinit foo+16
	.8byte 0
	.8byte 0
	.size c, .-c

d:
	.capinit a
	.8byte 0
	.8byte 0
	.size d, .-d

.section .data.rel.ro
.align 4
e:
	.capinit str2
	.8byte 0
	.8byte 0
	.size e, .-e

.align 4
.text
.globl _start
.type _start STT_FUNC
_start:
	ldr	c2, [c2, :lo12:a]
	ret

	.type f, %function
f:
	ldr	c2, [c2, :lo12:a]
        .p2align 4
.Llab:
	.capinit f+((.Llab+1)-f)
	.xword	0
	.xword	0
	.type f2, %function
f2:
	ldr	c2, [c2, :lo12:a]
        .8byte	0
.Llab2:
	.capinit f2+((.Llab2+1)-f2)
	.xword	0
	.xword	0
