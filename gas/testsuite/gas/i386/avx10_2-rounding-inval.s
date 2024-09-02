# Check invalid AVX10.2 instructions

	.text
	.arch .noavx10.2
_start:
	vcmppd	$123, {sae}, %ymm4, %ymm5, %k5
	vgetexppd	{sae}, %ymm5, %ymm6
	vsqrtpd	{rn-sae}, %ymm5, %ymm6
	vaddpd	{rn-sae}, %ymm4, %ymm5, %ymm6
	vmaxpd	{sae}, %ymm4, %ymm5, %ymm6
	vreducepd	$123, {sae}, %ymm5, %ymm6
	vfmadd132pd	{rn-sae}, %ymm4, %ymm5, %ymm6
	vrangepd	$123, {sae}, %ymm4, %ymm5, %ymm6
	vfcmaddcph	{rn-sae}, %ymm4, %ymm5, %ymm6
	vcvtdq2ph	{rn-sae}, %ymm5, %xmm6
	vcvtdq2ps	{rn-sae}, %ymm5, %ymm6
	vcvtpd2dq	{rn-sae}, %ymm5, %xmm6
	vcvtpd2ph	{rn-sae}, %ymm5, %xmm6
	vcvtpd2qq	{rn-sae}, %ymm5, %ymm6
	vcvtph2dq	{rn-sae}, %xmm5, %ymm6
	vcvtph2qq	{rn-sae}, %xmm5, %ymm6
	vcvtph2pd	{sae}, %xmm5, %ymm6
	vcvtph2ps	{sae}, %xmm5, %ymm6
	vcvtph2uw	{rn-sae}, %ymm5, %ymm6
	vcvtps2dq	{rn-sae}, %ymm5, %ymm6
	vcvtps2pd	{sae}, %xmm5, %ymm6
	vcvtps2phx	{rn-sae}, %ymm5, %xmm6
	vcvtps2qq	{rn-sae}, %xmm5, %ymm6
	vcvtqq2pd	{rn-sae}, %ymm5, %ymm6
	vcvtqq2ph	{rn-sae}, %ymm5, %xmm6
	vcvtqq2ps	{rn-sae}, %ymm5, %xmm6
	vcvttpd2dq	{sae}, %ymm5, %xmm6
	vcvttpd2qq	{sae}, %ymm5, %ymm6
	vcvttph2dq	{sae}, %xmm5, %ymm6
	vcvttph2qq	{sae}, %xmm5, %ymm6
	vcvttph2uw	{sae}, %ymm5, %ymm6
	vcvttps2dq	{sae}, %ymm5, %ymm6
	vcvttps2qq	{sae}, %xmm5, %ymm6
	vcvtuw2ph	{rn-sae}, %ymm5, %ymm6
