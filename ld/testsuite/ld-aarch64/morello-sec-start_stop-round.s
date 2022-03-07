.section __libc_atexit,"aw"
	.xword 42
.data
atexit_location:
	.chericap __start___libc_atexit
.text
.globl _start
.type _start STT_FUNC
_start:
	add c0, c0, :lo12:atexit_location
