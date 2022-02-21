SECTIONS {
	. = SEGMENT_START("text-segment", 0x1a0);
	.text : { *(.text) }
	. = DATA_SEGMENT_ALIGN (CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE));
	.dynamic        : { *(.dynamic) }
	.got            : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) }
	. = DATA_SEGMENT_RELRO_END (0, .);
	.othersection ALIGN(0x2000) : { *(.othersection*) }
	.data ALIGN(0x2000) : { *(.data) }
	. = DATA_SEGMENT_END (.);
}
