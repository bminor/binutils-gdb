#Test the special case of the index bits, 0x4, in SIB.

	.text
	.allow_index_reg
foo:
	mov	(%ebx),%eax
	mov	(%ebx,%eiz,1),%eax
	mov	(%ebx,%eiz,2),%eax
	mov	(%ebx,%eiz,4),%eax
	mov	(%ebx,%eiz,8),%eax
	mov	(%esp),%eax
	mov	(%esp,%eiz,1),%eax
	mov	(%esp,%eiz,2),%eax
	mov	(%esp,%eiz,4),%eax
	mov	(%esp,%eiz,8),%eax
	.p2align 4
