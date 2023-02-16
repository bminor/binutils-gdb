        .extern ecc_start
	.section .text
main:
	.long 0x45444F43
	.long 0x12345678
	
	.section .data
	.long 0x9abcdef0
	
	.section .bss
	.long 0
