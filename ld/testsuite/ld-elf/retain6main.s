/* Undefined symbol reference in retained section .text.retained_fn requires
   symbol definition to be pulled out of library.  */
	.section	.text.retained_fn,"axR"
	.global	retained_fn
	.type	retained_fn, %function
retained_fn:
	.long bar

	.section	.text._start,"ax"
	.global	_start
	.type	_start, %function
_start:
	.word 0
