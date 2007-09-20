#Test the special case of the index bits, 0x4, in SIB.

	.text
	.allow_index_reg
foo:
	mov	(%rbx),%eax
	mov	(%rbx,%riz,1),%eax
	mov	(%rbx,%riz,2),%eax
	mov	(%rbx,%riz,4),%eax
	mov	(%rbx,%riz,8),%eax
	mov	(%rsp),%eax
	mov	(%rsp,%riz,1),%eax
	mov	(%rsp,%riz,2),%eax
	mov	(%rsp,%riz,4),%eax
	mov	(%rsp,%riz,8),%eax
	.p2align 4
