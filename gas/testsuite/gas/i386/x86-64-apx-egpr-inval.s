# Check illegal 64bit APX_F instructions
	.text
	.arch .noapx_f
	test    $0x7, %r17d
	.arch .apx_f
	test    $0x7, %r17d
	xsave (%r16, %rbx)
	xsave64 (%r16, %r31)
	xrstor (%r16, %rbx)
	xrstor64 (%r16, %rbx)
	xsaves (%rbx, %r16)
	xsaves64 (%r16, %rbx)
	xrstors (%rbx, %r31)
	xrstors64 (%r16, %rbx)
	xsaveopt (%r16, %rbx)
	xsaveopt64 (%r16, %r31)
	xsavec (%r16, %rbx)
	xsavec64 (%r16, %r31)
