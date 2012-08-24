	.syntax unified
	.text
	.arch armv8-a

	.arm
foo:
	sevl

	.thumb
	.thumb_func
bar:
	sevl
	sevl.n
	sevl.w
	dcps1
	dcps2
	dcps3
