# Source file used to test -mips5 instructions.

text_label:	
	abs.ps		$f1, $f2
	add.ps		$f3, $f4, $f5
	alnv.ps		$f6, $f7, $f8, $3
	c.eq.ps		$f9, $f10
	c.eq.ps		$fcc2, $f11, $f12
	c.f.ps	    	$f9, $f10
	c.f.ps	    	$fcc2, $f11, $f12
	c.le.ps		$f9, $f10
	c.le.ps		$fcc2, $f11, $f12
	c.lt.ps		$f9, $f10
	c.lt.ps		$fcc2, $f11, $f12
	c.nge.ps	$f9, $f10
	c.nge.ps	$fcc2, $f11, $f12
	c.ngl.ps	$f9, $f10
	c.ngl.ps	$fcc2, $f11, $f12
	c.ngle.ps	$f9, $f10
	c.ngle.ps	$fcc2, $f11, $f12
	c.ngt.ps	$f9, $f10
	c.ngt.ps	$fcc2, $f11, $f12
	c.ole.ps	$f9, $f10
	c.ole.ps	$fcc2, $f11, $f12
	c.olt.ps	$f9, $f10
	c.olt.ps	$fcc2, $f11, $f12
	c.seq.ps	$f9, $f10
	c.seq.ps	$fcc2, $f11, $f12
	c.sf.ps		$f9, $f10
	c.sf.ps		$fcc2, $f11, $f12
	c.ueq.ps	$f9, $f10
	c.ueq.ps	$fcc2, $f11, $f12
	c.ule.ps	$f9, $f10
	c.ule.ps	$fcc2, $f11, $f12
	c.ult.ps	$f9, $f10
	c.ult.ps	$fcc2, $f11, $f12
	c.un.ps		$f9, $f10
	c.un.ps		$fcc2, $f11, $f12
	cvt.ps.s	$f13, $f14, $f15
	cvt.s.pl	$f16, $f17
	cvt.s.pu	$f18, $f19
	luxc1		$f20, $4($5)
	madd.ps		$f21, $f22, $f23, $f24
	mov.ps		$f25, $f26
	movf.ps		$f27, $f28, $fcc2
	movn.ps		$f27, $f28, $3
	movt.ps		$f29, $f30, $fcc4
	movz.ps		$f29, $f30, $5
	msub.ps		$f31, $f0, $f1, $f2
	mul.ps		$f3, $f4, $f5
	neg.ps		$f6, $f7
	nmadd.ps	$f8, $f9, $f10, $f11
	nmsub.ps	$f8, $f9, $f10, $f11
	pll.ps		$f12, $f13, $f14
	plu.ps		$f15, $f16, $f17
	pul.ps		$f18, $f19, $f20
	puu.ps		$f21, $f22, $f23
	sub.ps		$f24, $f25, $f26
	suxc1		$f27, $6($7)

	c.eq.ps		$fcc3, $f11, $f12	# warns
	movf.ps		$f27, $f28, $fcc3	# warns

# Force at least 8 (non-delay-slot) zero bytes, to make 'objdump' print ...
      .space  8
