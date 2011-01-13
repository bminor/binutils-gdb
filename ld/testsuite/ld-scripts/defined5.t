defined = addr1;
SECTIONS {
	.data.cacheline_aligned : { *(.data.cacheline_aligned) }
	. = ALIGN (0x1000);
	.data : { *(.data) }
	addr1  = ADDR (.data);
}
