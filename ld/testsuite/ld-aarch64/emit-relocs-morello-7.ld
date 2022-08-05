SECTIONS {
	. = SEGMENT_START("text-segment", SIZEOF_HEADERS);
	.dynamic : { *(.dynamic) }
	.othersection : { *(.othersection) }
	__text_start = .;
	__text_other = .;
	.text : { *(.text) }
	__data_other = .;
	/* Try and collect all read-only stuff so that the data symbols stay at
	 * the start of the .data section.  */
	.rodata : ONLY_IF_RO { *(.*) }
	__data_start = .;
	.data : { *(.data) }
	. = ALIGN(0x1000);
	.got : { *(.got) }
}
