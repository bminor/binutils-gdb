	.eabi_attribute	Tag_CPU_arch, 11	@ V6-M
	.thumb
	.type myfunc, function
	.global myfunc
	.text
	.space 102400000
myfunc:
	bx	lr
