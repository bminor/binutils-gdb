# Check 64bit AVX10.2/512 instructions

	.arch generic64
	.arch .avx10.2/512
	.text
_start:
	.irp m, addne, divne, max, min, mulne, scalefne, subne
	v\m\()pbf16	%zmm28, %zmm29, %zmm30
	v\m\()pbf16	0x10000000(%rbp, %r14, 8), %zmm29, %zmm30{%k7}
	v\m\()pbf16	(%r9){1to32}, %zmm29, %zmm30
	v\m\()pbf16	8128(%rcx), %zmm29, %zmm30
	v\m\()pbf16	-256(%rdx){1to32}, %zmm29, %zmm30{%k7}{z}
	.endr

	.irp m, madd, msub, nmadd, nmsub
	.irp n, 132, 213, 231
	vf\m\n\(\())nepbf16	%zmm28, %zmm29, %zmm30
	vf\m\n\(\())nepbf16	0x10000000(%rbp, %r14, 8), %zmm29, %zmm30{%k7}
	vf\m\n\(\())nepbf16	(%r9){1to32}, %zmm29, %zmm30
	vf\m\n\(\())nepbf16	8128(%rcx), %zmm29, %zmm30
	vf\m\n\(\())nepbf16	-256(%rdx){1to32}, %zmm29, %zmm30{%k7}{z}
	.endr
	.endr

	.irp m, getexp, rcp, rsqrt, sqrtne
	v\m\()pbf16	%zmm29, %zmm30
	v\m\()pbf16	0x10000000(%rbp, %r14, 8), %zmm30{%k7}
	v\m\()pbf16	(%r9){1to32}, %zmm30
	v\m\()pbf16	8128(%rcx), %zmm30
	v\m\()pbf16	-256(%rdx){1to32}, %zmm30{%k7}{z}
	.endr

	.irp m, getmant, reducene, rndscalene
	v\m\()pbf16	$123, %zmm29, %zmm30
	v\m\()pbf16	$123, 0x10000000(%rbp, %r14, 8), %zmm30{%k7}
	v\m\()pbf16	$123, (%r9){1to32}, %zmm30
	v\m\()pbf16	$123, 8128(%rcx), %zmm30
	v\m\()pbf16	$123, -256(%rdx){1to32}, %zmm30{%k7}{z}
	.endr

	vcmppbf16	$123, %zmm28, %zmm29, %k5
	vcmppbf16	$123, 0x10000000(%rbp, %r14, 8), %zmm29, %k5{%k7}
	vcmppbf16	$123, (%r9){1to32}, %zmm29, %k5
	vcmppbf16	$123, 8128(%rcx), %zmm29, %k5
	vcmppbf16	$123, -256(%rdx){1to32}, %zmm29, %k5{%k7}

	vfpclasspbf16	$123, %zmm29, %k5
	vfpclasspbf16z	$123, 0x10000000(%rbp, %r14, 8), %k5{%k7}
	vfpclasspbf16	$123, (%r9){1to32}, %k5
	vfpclasspbf16z	$123, 8128(%rcx), %k5
	vfpclasspbf16	$123, -256(%rdx){1to32}, %k5{%k7}

_intel:
	.intel_syntax noprefix
	.irp m, addne, divne, max, min, mulne, scalefne, subne
	v\m\()pbf16	zmm30, zmm29, zmm28
	v\m\()pbf16	zmm30{k7}, zmm29, [rbp+r14*8+0x10000000]
	v\m\()pbf16	zmm30, zmm29, WORD PTR [r9]{1to32}
	v\m\()pbf16	zmm30, zmm29, ZMMWORD PTR [rcx+8128]
	v\m\()pbf16	zmm30{k7}{z}, zmm29, [rdx-256]{1to32}
	.endr

	.irp m, madd, msub, nmadd, nmsub
	.irp n, 132, 213, 231
	vf\m\n\(\())nepbf16	zmm30, zmm29, zmm28
	vf\m\n\(\())nepbf16	zmm30{k7}, zmm29, ZMMWORD PTR [rbp+r14*8+0x10000000]
	vf\m\n\(\())nepbf16	zmm30, zmm29, [r9]{1to32}
	vf\m\n\(\())nepbf16	zmm30, zmm29, [rcx+8128]
	vf\m\n\(\())nepbf16	zmm30{k7}{z}, zmm29, WORD PTR [rdx-256]{1to32}
	.endr
	.endr

	.irp m, getexp, rcp, rsqrt, sqrtne
	v\m\()pbf16	zmm30, zmm29
	v\m\()pbf16	zmm30{k7}, [rbp+r14*8+0x10000000]
	v\m\()pbf16	zmm30, WORD PTR [r9]{1to32}
	v\m\()pbf16	zmm30, ZMMWORD PTR [rcx+8128]
	v\m\()pbf16	zmm30{k7}{z}, [rdx-256]{1to32}
	.endr

	.irp m, getmant, reducene, rndscalene
	v\m\()pbf16	zmm30, zmm29, 123
	v\m\()pbf16	zmm30{k7}, ZMMWORD PTR [rbp+r14*8+0x10000000], 123
	v\m\()pbf16	zmm30, [r9]{1to32}, 123
	v\m\()pbf16	zmm30, [rcx+8128], 123
	v\m\()pbf16	zmm30{k7}{z}, WORD PTR [rdx-256]{1to32}, 123
	.endr

	vcmppbf16	k5, zmm29, zmm28, 123
	vcmppbf16	k5{k7}, zmm29, [rbp+r14*8+0x10000000], 123
	vcmppbf16	k5, zmm29, WORD PTR [r9]{1to32}, 123
	vcmppbf16	k5, zmm29, ZMMWORD PTR [rcx+8128], 123
	vcmppbf16	k5{k7}, zmm29, [rdx-256]{1to32}, 123

	vfpclasspbf16	k5, zmm29, 123
	vfpclasspbf16	k5{k7}, ZMMWORD PTR [rbp+r14*8+0x10000000], 123
	vfpclasspbf16	k5, [r9]{1to32}, 123
	vfpclasspbf16	k5, ZMMWORD PTR [rcx+8128], 123
	vfpclasspbf16	k5{k7}, WORD PTR [rdx-256]{1to32}, 123
