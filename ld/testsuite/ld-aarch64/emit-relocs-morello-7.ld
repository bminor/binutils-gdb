SECTIONS {
	. = SEGMENT_START("text-segment", SIZEOF_HEADERS);
	.dynamic : { *(.dynamic) }
	.othersection : { *(.othersection) }
	__text_start = .;
	__text_other = .;
	.text : { *(.text) }
	__data_start = .;
	__data_other = .;
	.data : { *(.data) }
	. = ALIGN(0x1000);
	.got : { *(.got) }
}
