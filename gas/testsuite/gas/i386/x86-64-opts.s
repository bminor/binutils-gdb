# Check 64bit instructions with encoding options

	.allow_index_reg
	.text
_start:

# Tests for op reg, reg
	mov %dl,%cl
	mov.s %dl,%cl
	mov %dx,%cx
	mov.s %dx,%cx
	mov %edx,%ecx
	mov.s %edx,%ecx
	movb %dl,%cl
	movb.s %dl,%cl
	movw %dx,%cx
	movw.s %dx,%cx
	movl %edx,%ecx
	movl.s %edx,%ecx
	mov %rdx,%rcx
	mov.s %rdx,%rcx
	movq %rdx,%rcx
	movq.s %rdx,%rcx

# Tests for op ymm, ymm
	vmovapd %ymm4,%ymm6
	vmovapd.s %ymm4,%ymm6
	vmovaps %ymm4,%ymm6
	vmovaps.s %ymm4,%ymm6
	vmovdqa %ymm4,%ymm6
	vmovdqa.s %ymm4,%ymm6
	vmovdqu %ymm4,%ymm6
	vmovdqu.s %ymm4,%ymm6
	vmovupd %ymm4,%ymm6
	vmovupd.s %ymm4,%ymm6
	vmovups %ymm4,%ymm6
	vmovups.s %ymm4,%ymm6

# Tests for op xmm, xmm
	movapd %xmm4,%xmm6
	movapd.s %xmm4,%xmm6
	movaps %xmm4,%xmm6
	movaps.s %xmm4,%xmm6
	movdqa %xmm4,%xmm6
	movdqa.s %xmm4,%xmm6
	movdqu %xmm4,%xmm6
	movdqu.s %xmm4,%xmm6
	movq %xmm4,%xmm6
	movq.s %xmm4,%xmm6
	movsd %xmm4,%xmm6
	movsd.s %xmm4,%xmm6
	movss %xmm4,%xmm6
	movss.s %xmm4,%xmm6
	movupd %xmm4,%xmm6
	movupd.s %xmm4,%xmm6
	movups %xmm4,%xmm6
	movups.s %xmm4,%xmm6
	vmovapd %xmm4,%xmm6
	vmovapd.s %xmm4,%xmm6
	vmovaps %xmm4,%xmm6
	vmovaps.s %xmm4,%xmm6
	vmovdqa %xmm4,%xmm6
	vmovdqa.s %xmm4,%xmm6
	vmovdqu %xmm4,%xmm6
	vmovdqu.s %xmm4,%xmm6
	vmovq %xmm4,%xmm6
	vmovq.s %xmm4,%xmm6
	vmovupd %xmm4,%xmm6
	vmovupd.s %xmm4,%xmm6
	vmovups %xmm4,%xmm6
	vmovups.s %xmm4,%xmm6

# Tests for op xmm, xmm, xmm
	vmovsd %xmm4,%xmm6,%xmm2
	vmovsd.s %xmm4,%xmm6,%xmm2
	vmovss %xmm4,%xmm6,%xmm2
	vmovss.s %xmm4,%xmm6,%xmm2

# Tests for op mm, mm
	movq %mm0,%mm4
	movq.s %mm0,%mm4

	.intel_syntax noprefix

# Tests for op reg, reg
	mov cl,dl
	mov.s cl,dl
	mov cx,dx
	mov.s cx,dx
	mov ecx,edx
	mov.s ecx,edx
	mov rcx,rdx
	mov.s rcx,rdx

# Tests for op ymm, ymm
	vmovapd ymm6,ymm4
	vmovapd.s ymm6,ymm4
	vmovaps ymm6,ymm4
	vmovaps.s ymm6,ymm4
	vmovdqa ymm6,ymm4
	vmovdqa.s ymm6,ymm4
	vmovdqu ymm6,ymm4
	vmovdqu.s ymm6,ymm4
	vmovupd ymm6,ymm4
	vmovupd.s ymm6,ymm4
	vmovups ymm6,ymm4
	vmovups.s ymm6,ymm4

# Tests for op xmm, xmm
	movapd xmm6,xmm4
	movapd.s xmm6,xmm4
	movaps xmm6,xmm4
	movaps.s xmm6,xmm4
	movdqa xmm6,xmm4
	movdqa.s xmm6,xmm4
	movdqu xmm6,xmm4
	movdqu.s xmm6,xmm4
	movq xmm6,xmm4
	movq.s xmm6,xmm4
	movsd xmm6,xmm4
	movsd.s xmm6,xmm4
	movss xmm6,xmm4
	movss.s xmm6,xmm4
	movupd xmm6,xmm4
	movupd.s xmm6,xmm4
	movups xmm6,xmm4
	movups.s xmm6,xmm4
	vmovapd xmm6,xmm4
	vmovapd.s xmm6,xmm4
	vmovaps xmm6,xmm4
	vmovaps.s xmm6,xmm4
	vmovdqa xmm6,xmm4
	vmovdqa.s xmm6,xmm4
	vmovdqu xmm6,xmm4
	vmovdqu.s xmm6,xmm4
	vmovq xmm6,xmm4
	vmovq.s xmm6,xmm4
	vmovupd xmm6,xmm4
	vmovupd.s xmm6,xmm4
	vmovups xmm6,xmm4
	vmovups.s xmm6,xmm4

# Tests for op xmm, xmm, xmm
	vmovsd xmm2,xmm6,xmm4
	vmovsd.s xmm2,xmm6,xmm4
	vmovss xmm2,xmm6,xmm4
	vmovss.s xmm2,xmm6,xmm4

# Tests for op mm, mm
	movq mm4,mm0
	movq.s mm4,mm0
