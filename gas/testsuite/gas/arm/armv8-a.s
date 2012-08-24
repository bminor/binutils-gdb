	.syntax unified
	.text
	.arch armv8-a

	.arm
foo:
	sevl
	hlt 0x0
	hlt 0xf
	hlt 0xfff0

	.thumb
	.thumb_func
bar:
	sevl
	sevl.n
	sevl.w
	dcps1
	dcps2
	dcps3
	hlt 0
	hlt 63
