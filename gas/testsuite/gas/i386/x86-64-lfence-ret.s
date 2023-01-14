	.text
_start:
	retw
	retw	$20
	ret
	ret	$30
	data16 rex.w ret
	data16 rex.w ret	$40
	lretw
	lretw	$40
	lret
	lret	$40
	lretq
	lretq	$40
