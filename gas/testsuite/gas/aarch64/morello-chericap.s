// Just using the same testcase as the capinit one, but with capinit switched
// to chericap.  chericap does not allocate space so we remove the data
// directives.
//
// On top of that we add a testcase using a more complicated form that these
// directives see in the real world.  Just to ensure that works too.
        .text
        .globl  f
        .p2align        2
        .type   f,@function
f:
	stp	c29, c30, [csp, #-32]!
	mov	c29, csp
	b	.LBB0_1
.Ltmp0:
.LBB0_1:
	ldp	c29, c30, [csp], #32
	ret	c30
.Lfunc_end0:
        .size   f, .Lfunc_end0-f

        .type   bar,@object
        .data
        .p2align        4
bar:
        .chericap f+((.Ltmp0+1)-f)
        .size   bar, 16

        .type   f.p,@object
        .data
        .p2align        4
f.p:
        .capinit f+((.Ltmp0+1)-f)
        .xword  0
        .xword  0
        .size   f.p, 16


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
	.chericap str
	.size a, .-a

b:
	.chericap str+8
	.size b, .-b

c:
	.chericap foo+16
	.size c, .-c

d:
	.chericap a
	.size d, .-d

.section .data.rel.ro
.align 4
e:
	.chericap str2
	.size e, .-e

.align 4
.text
.globl _start
.type _start STT_FUNC
_start:
	ldr	c2, [c2, :lo12:a]
	ret

	.type g, %function
g:
	ldr	c2, [c2, :lo12:a]
        .p2align 4
.Llab:
	.chericap g+((.Llab+1)-g)
	.type g2, %function
g2:
	ldr	c2, [c2, :lo12:a]
        .8byte	0
.Llab2:
	.chericap g2+((.Llab2+1)-g2)
