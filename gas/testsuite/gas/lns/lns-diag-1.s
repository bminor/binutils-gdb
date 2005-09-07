	.file 1 "foo.c"
	.file 0 "bar.c"
	.file 2 baz.c
	.file 1 "bar.c"

	.loc 1 1
	.loc 1 2 3
	.loc 3 1
	.loc 1 1 1 1

	.loc basic_block
	.loc basic_block 0
	.loc prologue_end
	.loc epilogue_begin

	.loc is_stmt 0
	.loc is_stmt 1
	.loc is_stmt 2
	.loc is_stmt foo

	.loc isa 1
	.loc isa 2
	.loc isa -1
	.loc isa 0

	.loc frobnitz
