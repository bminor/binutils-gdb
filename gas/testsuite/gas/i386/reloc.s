.text
foo:	mov	$foo, %bl
	push	$foo
	add	$foo, %eax
	add	$foo, %ebx
	imul	$foo, %edx
	lcall	$0, $foo
	pushw	$foo
