SECTIONS {
   . = SIZEOF_HEADERS;
   .text :
	{
		*(.text)
	}
   .data.rel.ro :
   {
     __data_rel_ro_startsym = .;
     *(.data.rel.ro)
   }
   .got : { *(.got) }
   .iplt : { *(.iplt) }
   .data : { *(.data) }
   .rela.dyn : { *(.rela.dyn) }
   .tail_rw : { *(.tail_rw) }
   .interp : { *(.interp) }
}
