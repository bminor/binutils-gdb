	.section	".text"
	.global tls_gd
	.section	.tbss,"awT",@nobits
	.align 4
	.type	tls_gd, #object
	.size	tls_gd, 4
tls_gd:
	.skip	4
	.section	".text"
	.align 4
	.global get_gdp
	.type	get_gdp, #function
	.proc	0104
get_gdp:
	sethi	%tgd_hi22(tls_gd), %g1
	add	%g1, %tgd_lo10(tls_gd), %g1
	add	%l7, %g1, %o0, %tgd_add(tls_gd)
	call	__tls_get_addr, %tgd_call(tls_gd)
	 nop
	.size	get_gdp, .-get_gdp
