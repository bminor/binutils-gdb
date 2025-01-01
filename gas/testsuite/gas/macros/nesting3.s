	.text
.macro func
	foo
.endm

.macro do_foo
.macro foo
foo:
	.dc.a 0
.endm
.endm
do_foo
func
