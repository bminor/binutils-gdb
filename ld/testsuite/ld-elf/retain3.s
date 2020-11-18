/* The retention of bar should also prevent foo from being gc'ed, since bar
   references foo.  */
	.section	.text.foo,"ax"
	.global	foo
	.type	foo, %function
foo:
	.word 0

	.section	.text.bar,"axR"
	.global	bar
	.type	bar, %function
bar:
	.long foo

	.section	.text._start,"ax"
	.global	_start
	.type	_start, %function
_start:
	.word 0
