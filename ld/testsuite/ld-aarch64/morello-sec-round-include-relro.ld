SECTIONS {
	. = SEGMENT_START("text-segment", SIZEOF_HEADERS);
	.text : { *(.text) }
	. = DATA_SEGMENT_ALIGN (CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE));
	.dynamic        : { *(.dynamic) }
	.got            : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) }
	.data.rel.ro : { *(.data.rel.ro*) }
	. = DATA_SEGMENT_RELRO_END (0, .);
	.data : { *(.data) }
	. = DATA_SEGMENT_END (.);
}

