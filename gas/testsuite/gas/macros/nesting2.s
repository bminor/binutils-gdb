.macro function name
 .macro endfunc
 .endm
	.text
\name:
.endm

function foo
	.dc.a 0
endfunc
