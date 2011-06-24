	.syntax unified	
	.arch	armv5te
	.section        .text.answer,"ax",%progbits
	.align	2
	.global	answer
	.type	answer, %function
answer:
	.fnstart
        .cantunwind
	mov	r0, #42
	bx	lr
	.fnend
	.size	answer, .-answer

# Check that we can handle an empty .text section
	.section        .text.empty,"ax",%progbits
	.align	2
        .global empty
        .type   empty, %function
empty:
        .fnstart
        .cantunwind
        .fnend
        .size   empty, .-empty

