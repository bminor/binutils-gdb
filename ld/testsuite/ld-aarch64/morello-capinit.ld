SECTIONS {
	. = SIZEOF_HEADERS;
	.dynamic : { *(.dynamic) }
	__outer_text_start = .;
	.text_low :
	{
		text_start = .;
		*(.text)
		text1_start = .;
		*(.text.1)
		text1_end = .;
		*(.text.2)
		__text_end = .;
	}
	.plt : { *(.plt) }
	.got : { *(.got) }
	.got : { *(.got) }
	.data : { *(.data) }
}
