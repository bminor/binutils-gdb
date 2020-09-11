SECTIONS {
   . = SIZEOF_HEADERS;
   .dynamic : { *(.dynamic) }
   .text_sec :
	{
		text_sym = .;
		*(.text)
	}
   .plt : { *(.plt) }
   .got : { *(.got) }
   .interp : { *(.interp) }
}
