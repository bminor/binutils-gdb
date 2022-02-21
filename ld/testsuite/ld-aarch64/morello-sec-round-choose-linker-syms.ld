SECTIONS {
	. = SEGMENT_START("text-segment", SIZEOF_HEADERS);
	.text : { *(.text) }
	.dynamic        : { *(.dynamic) }
	.got            : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) }
	.data.rel.ro : {
	  __data_relro_start = .;
	  *(.data.rel.ro*)
	}
	.data : { *(.data) }
}

