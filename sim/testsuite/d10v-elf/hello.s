	.text
	.align 2
	.globl _start
_start:
	# write (1, "Hello World!\n", strlen ("Hello World!..")
	ldi r6, 4
	ldi r2, 1
	ldi r3, hello
	ldi r4, 13
	trap 15

	# exit (0)
	ldi r6, 1
	ldi r2, 0
	trap 15

.section	.rodata
hello:
	.string "Hello World!\n"

