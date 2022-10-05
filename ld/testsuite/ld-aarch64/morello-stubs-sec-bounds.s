.arch morello
.section .text.1
.type foo, STT_FUNC
foo:
	bl baz
	mov	c1, c2
	ret

.arch morello+c64
baz2:
	b baz
	ret

lots_of_zeros:
.zero 0xffff0

.section .text.3
.arch morello
baz3:
	b foo
	ret

.arch morello+c64

.type baz, STT_FUNC
.global baz
baz:
	adrp c0, :got:text_sym
	b foo
	ret

.section        .data.rel.ro.local,"aw"
.align  4
.type   ptr, %object
.size   ptr, 16
ptr:
  .chericap baz
