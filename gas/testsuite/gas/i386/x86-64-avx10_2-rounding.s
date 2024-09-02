# Check 64bit AVX10.2 instructions

	.text
_start:
	.irp m, pd, ph, ps
	vcmp\m	$123, {sae}, %ymm28, %ymm29, %k5
	vcmp\m	$123, {sae}, %ymm28, %ymm29, %k5{%k7}
	vgetexp\m	{sae}, %ymm29, %ymm30
	vgetexp\m	{sae}, %ymm29, %ymm30{%k7}
	vgetexp\m	{sae}, %ymm29, %ymm30{%k7}{z}
	vsqrt\m	{rn-sae}, %ymm29, %ymm30
	vsqrt\m	{rd-sae}, %ymm29, %ymm30{%k7}
	vsqrt\m	{rz-sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp a, add, div, mul, scalef, sub
	.irp m, pd, ph, ps
	v\a\m	{rn-sae}, %ymm28, %ymm29, %ymm30
	v\a\m	{rd-sae}, %ymm28, %ymm29, %ymm30{%k7}
	v\a\m	{rz-sae}, %ymm28, %ymm29, %ymm30{%k7}{z}
	.endr
	.endr

	.irp a, max, min
	.irp m, pd, ph, ps
	v\a\m	{sae}, %ymm28, %ymm29, %ymm30
	v\a\m	{sae}, %ymm28, %ymm29, %ymm30{%k7}
	v\a\m	{sae}, %ymm28, %ymm29, %ymm30{%k7}{z}
	.endr
	.endr

	.irp a, getmant, reduce, rndscale
	.irp m, pd, ph, ps
	v\a\m	$123, {sae}, %ymm29, %ymm30
	v\a\m	$123, {sae}, %ymm29, %ymm30{%k7}
	v\a\m	$123, {sae}, %ymm29, %ymm30{%k7}{z}
	.endr
	.endr

	.irp a, madd, maddsub, msub, msubadd, nmadd, nmsub
	.irp n, 132, 213, 231
	.irp m, pd, ph, ps
	vf\a\n\m	{rn-sae}, %ymm28, %ymm29, %ymm30
	vf\a\n\m	{rd-sae}, %ymm28, %ymm29, %ymm30{%k7}
	vf\a\n\m	{rz-sae}, %ymm28, %ymm29, %ymm30{%k7}{z}
	.endr
	.endr
	.endr

	.irp a, fixupimm, range
	.irp m, pd, ps
	v\a\m	$123, {sae}, %ymm28, %ymm29, %ymm30
	v\a\m	$123, {sae}, %ymm28, %ymm29, %ymm30{%k7}
	v\a\m	$123, {sae}, %ymm28, %ymm29, %ymm30{%k7}{z}
	.endr
	.endr

	.irp a, cmadd, cmul, madd, mul
	vf\a\()cph	{rn-sae}, %ymm28, %ymm29, %ymm30
	vf\a\()cph	{rd-sae}, %ymm28, %ymm29, %ymm30{%k7}
	vf\a\()cph	{rz-sae}, %ymm28, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp n, dq, udq
	vcvt\n\()2ph	{rn-sae}, %ymm29, %xmm30
	vcvt\n\()2ph	{rd-sae}, %ymm29, %xmm30{%k7}
	vcvt\n\()2ph	{rz-sae}, %ymm29, %xmm30{%k7}{z}

	vcvt\n\()2ps	{rn-sae}, %ymm29, %ymm30
	vcvt\n\()2ps	{rd-sae}, %ymm29, %ymm30{%k7}
	vcvt\n\()2ps	{rz-sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp m, dq, ph, ps, udq
	vcvtpd2\m	{rn-sae}, %ymm29, %xmm30
	vcvtpd2\m	{rd-sae}, %ymm29, %xmm30{%k7}
	vcvtpd2\m	{rz-sae}, %ymm29, %xmm30{%k7}{z}
	.endr

	.irp m, qq, uqq
	vcvtpd2\m	{rn-sae}, %ymm29, %ymm30
	vcvtpd2\m	{rd-sae}, %ymm29, %ymm30{%k7}
	vcvtpd2\m	{rz-sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvtph2\m	{rn-sae}, %xmm29, %ymm30
	vcvtph2\m	{rd-sae}, %xmm29, %ymm30{%k7}
	vcvtph2\m	{rz-sae}, %xmm29, %ymm30{%k7}{z}
	.endr

	.irp m, pd, ps, psx
	vcvtph2\m	{sae}, %xmm29, %ymm30
	vcvtph2\m	{sae}, %xmm29, %ymm30{%k7}
	vcvtph2\m	{sae}, %xmm29, %ymm30{%k7}{z}
	.endr

	.irp m, uw, w
	vcvtph2\m	{rn-sae}, %ymm29, %ymm30
	vcvtph2\m	{rd-sae}, %ymm29, %ymm30{%k7}
	vcvtph2\m	{rz-sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp m, dq, udq
	vcvtps2\m	{rn-sae}, %ymm29, %ymm30
	vcvtps2\m	{rd-sae}, %ymm29, %ymm30{%k7}
	vcvtps2\m	{rz-sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	vcvtps2pd	{sae}, %xmm29, %ymm30
	vcvtps2pd	{sae}, %xmm29, %ymm30{%k7}
	vcvtps2pd	{sae}, %xmm29, %ymm30{%k7}{z}

	vcvtps2phx	{rn-sae}, %ymm29, %xmm30
	vcvtps2phx	{rd-sae}, %ymm29, %xmm30{%k7}
	vcvtps2phx	{rz-sae}, %ymm29, %xmm30{%k7}{z}

	.irp m, qq, uqq
	vcvtps2\m	{rn-sae}, %xmm29, %ymm30
	vcvtps2\m	{rd-sae}, %xmm29, %ymm30{%k7}
	vcvtps2\m	{rz-sae}, %xmm29, %ymm30{%k7}{z}
	.endr

	.irp n, qq, uqq
	vcvt\n\()2pd	{rn-sae}, %ymm29, %ymm30
	vcvt\n\()2pd	{rd-sae}, %ymm29, %ymm30{%k7}
	vcvt\n\()2pd	{rz-sae}, %ymm29, %ymm30{%k7}{z}

	.irp m, ph, ps
	vcvt\n\()2\m	{rn-sae}, %ymm29, %xmm30
	vcvt\n\()2\m	{rd-sae}, %ymm29, %xmm30{%k7}
	vcvt\n\()2\m	{rz-sae}, %ymm29, %xmm30{%k7}{z}
	.endr
	.endr

	.irp m, dq, udq
	vcvttpd2\m	{sae}, %ymm29, %xmm30
	vcvttpd2\m	{sae}, %ymm29, %xmm30{%k7}
	vcvttpd2\m	{sae}, %ymm29, %xmm30{%k7}{z}
	.endr

	.irp m, qq, uqq
	vcvttpd2\m	{sae}, %ymm29, %ymm30
	vcvttpd2\m	{sae}, %ymm29, %ymm30{%k7}
	vcvttpd2\m	{sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvttph2\m	{sae}, %xmm29, %ymm30
	vcvttph2\m	{sae}, %xmm29, %ymm30{%k7}
	vcvttph2\m	{sae}, %xmm29, %ymm30{%k7}{z}
	.endr

	.irp m, uw, w
	vcvttph2\m	{sae}, %ymm29, %ymm30
	vcvttph2\m	{sae}, %ymm29, %ymm30{%k7}
	vcvttph2\m	{sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp m, dq, udq
	vcvttps2\m	{sae}, %ymm29, %ymm30
	vcvttps2\m	{sae}, %ymm29, %ymm30{%k7}
	vcvttps2\m	{sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.irp m, qq, uqq
	vcvttps2\m	{sae}, %xmm29, %ymm30
	vcvttps2\m	{sae}, %xmm29, %ymm30{%k7}
	vcvttps2\m	{sae}, %xmm29, %ymm30{%k7}{z}
	.endr

	.irp n, uw, w
	vcvt\n\()2ph	{rn-sae}, %ymm29, %ymm30
	vcvt\n\()2ph	{rd-sae}, %ymm29, %ymm30{%k7}
	vcvt\n\()2ph	{rz-sae}, %ymm29, %ymm30{%k7}{z}
	.endr

	.intel_syntax noprefix
	.irp m, pd, ph, ps
	vcmp\m	k5, ymm29, ymm28{sae}, 123
	vcmp\m	k5{k7}, ymm29, ymm28{sae}, 123
	vgetexp\m	ymm30, ymm29{sae}
	vgetexp\m	ymm30{k7}, ymm29{sae}
	vgetexp\m	ymm30{k7}{z}, ymm29{sae}
	vsqrt\m	ymm30, ymm29{rn-sae}
	vsqrt\m	ymm30{k7}, ymm29{rd-sae}
	vsqrt\m	ymm30{k7}{z}, ymm29{rz-sae}
	.endr

	.irp a, add, div, mul, scalef, sub
	.irp m, pd, ph, ps
	v\a\m	ymm30, ymm29, ymm28{rn-sae}
	v\a\m	ymm30{k7}, ymm29, ymm28{rd-sae}
	v\a\m	ymm30{k7}{z}, ymm29, ymm28{rz-sae}
	.endr
	.endr

	.irp a, max, min
	.irp m, pd, ph, ps
	v\a\m	ymm30, ymm29, ymm28{sae}
	v\a\m	ymm30{k7}, ymm29, ymm28{sae}
	v\a\m	ymm30{k7}{z}, ymm29, ymm28{sae}
	.endr
	.endr

	.irp a, getmant, reduce, rndscale
	.irp m, pd, ph, ps
	v\a\m	ymm30, ymm29{sae}, 123
	v\a\m	ymm30{k7}, ymm29{sae}, 123
	v\a\m	ymm30{k7}{z}, ymm29{sae}, 123
	.endr
	.endr

	.irp a, madd, maddsub, msub, msubadd, nmadd, nmsub
	.irp n, 132, 213, 231
	.irp m, pd, ph, ps
	vf\a\n\m	ymm30, ymm29, ymm28{rn-sae}
	vf\a\n\m	ymm30{k7}, ymm29, ymm28{rd-sae}
	vf\a\n\m	ymm30{k7}{z}, ymm29, ymm28{rz-sae}
	.endr
	.endr
	.endr

	.irp a, fixupimm, range
	.irp m, pd, ps
	v\a\m	ymm30, ymm29, ymm28{sae}, 123
	v\a\m	ymm30{k7}, ymm29, ymm28{sae}, 123
	v\a\m	ymm30{k7}{z}, ymm29, ymm28{sae}, 123
	.endr
	.endr

	.irp a, cmadd, cmul, madd, mul
	vf\a\()cph	ymm30, ymm29, ymm28{rn-sae}
	vf\a\()cph	ymm30{k7}, ymm29, ymm28{rd-sae}
	vf\a\()cph	ymm30{k7}{z}, ymm29, ymm28{rz-sae}
	.endr

	.irp n, dq, udq
	vcvt\n\()2ph	xmm30, ymm29{rn-sae}
	vcvt\n\()2ph	xmm30{k7}, ymm29{rd-sae}
	vcvt\n\()2ph	xmm30{k7}{z}, ymm29{rz-sae}

	vcvt\n\()2ps	ymm30, ymm29{rn-sae}
	vcvt\n\()2ps	ymm30{k7}, ymm29{rd-sae}
	vcvt\n\()2ps	ymm30{k7}{z}, ymm29{rz-sae}
	.endr

	.irp m, dq, ph, ps, udq
	vcvtpd2\m	xmm30, ymm29{rn-sae}
	vcvtpd2\m	xmm30{k7}, ymm29{rd-sae}
	vcvtpd2\m	xmm30{k7}{z}, ymm29{rz-sae}
	.endr

	.irp m, qq, uqq
	vcvtpd2\m	ymm30, ymm29{rn-sae}
	vcvtpd2\m	ymm30{k7}, ymm29{rd-sae}
	vcvtpd2\m	ymm30{k7}{z}, ymm29{rz-sae}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvtph2\m	ymm30, xmm29{rn-sae}
	vcvtph2\m	ymm30{k7}, xmm29{rd-sae}
	vcvtph2\m	ymm30{k7}{z}, xmm29{rz-sae}
	.endr

	.irp m, pd, ps, psx
	vcvtph2\m	ymm30, xmm29{sae}
	vcvtph2\m	ymm30{k7}, xmm29{sae}
	vcvtph2\m	ymm30{k7}{z}, xmm29{sae}
	.endr

	.irp m, uw, w
	vcvtph2\m	ymm30, ymm29{rn-sae}
	vcvtph2\m	ymm30{k7}, ymm29{rd-sae}
	vcvtph2\m	ymm30{k7}{z}, ymm29{rz-sae}
	.endr

	.irp m, dq, udq
	vcvtps2\m	ymm30, ymm29{rn-sae}
	vcvtps2\m	ymm30{k7}, ymm29{rd-sae}
	vcvtps2\m	ymm30{k7}{z}, ymm29{rz-sae}
	.endr

	vcvtps2pd	ymm30, xmm29{sae}
	vcvtps2pd	ymm30{k7}, xmm29{sae}
	vcvtps2pd	ymm30{k7}{z}, xmm29{sae}

	vcvtps2phx	xmm30, ymm29{rn-sae}
	vcvtps2phx	xmm30{k7}, ymm29{rd-sae}
	vcvtps2phx	xmm30{k7}{z}, ymm29{rz-sae}

	.irp m, qq, uqq
	vcvtps2\m	ymm30, xmm29{rn-sae}
	vcvtps2\m	ymm30{k7}, xmm29{rd-sae}
	vcvtps2\m	ymm30{k7}{z}, xmm29{rz-sae}
	.endr

	.irp n, qq, uqq
	vcvt\n\()2pd	ymm30, ymm29{rn-sae}
	vcvt\n\()2pd	ymm30{k7}, ymm29{rd-sae}
	vcvt\n\()2pd	ymm30{k7}{z}, ymm29{rz-sae}

	.irp m, ph, ps
	vcvt\n\()2\m	xmm30, ymm29{rn-sae}
	vcvt\n\()2\m	xmm30{k7}, ymm29{rd-sae}
	vcvt\n\()2\m	xmm30{k7}{z}, ymm29{rz-sae}
	.endr
	.endr

	.irp m, dq, udq
	vcvttpd2\m	xmm30, ymm29{sae}
	vcvttpd2\m	xmm30{k7}, ymm29{sae}
	vcvttpd2\m	xmm30{k7}{z}, ymm29{sae}
	.endr

	.irp m, qq, uqq
	vcvttpd2\m	ymm30, ymm29{sae}
	vcvttpd2\m	ymm30{k7}, ymm29{sae}
	vcvttpd2\m	ymm30{k7}{z}, ymm29{sae}
	.endr

	.irp m, dq, qq, udq, uqq
	vcvttph2\m	ymm30, xmm29{sae}
	vcvttph2\m	ymm30{k7}, xmm29{sae}
	vcvttph2\m	ymm30{k7}{z}, xmm29{sae}
	.endr

	.irp m, uw, w
	vcvttph2\m	ymm30, ymm29{sae}
	vcvttph2\m	ymm30{k7}, ymm29{sae}
	vcvttph2\m	ymm30{k7}{z}, ymm29{sae}
	.endr

	.irp m, dq, udq
	vcvttps2\m	ymm30, ymm29{sae}
	vcvttps2\m	ymm30{k7}, ymm29{sae}
	vcvttps2\m	ymm30{k7}{z}, ymm29{sae}
	.endr

	.irp m, qq, uqq
	vcvttps2\m	ymm30, xmm29{sae}
	vcvttps2\m	ymm30{k7}, xmm29{sae}
	vcvttps2\m	ymm30{k7}{z}, xmm29{sae}
	.endr

	.irp n, uw, w
	vcvt\n\()2ph	ymm30, ymm29{rn-sae}
	vcvt\n\()2ph	ymm30{k7}, ymm29{rd-sae}
	vcvt\n\()2ph	ymm30{k7}{z}, ymm29{rz-sae}
	.endr
