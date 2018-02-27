	.text
foo:
	.dc.l    0x12345678

	.section .rela.text
	.dc.a	 0
	.dc.a    0x000000f1
	.dc.a	 0x000000f1

	.dc.a	 0
	.dc.a    0
	.dc.a	 0
