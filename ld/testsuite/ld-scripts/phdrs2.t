PHDRS
{
  text PT_LOAD ;
  data PT_LOAD ;
}

SECTIONS
{
  . = 0x800000 - 1;
  /* The PHDRS generated should start at the aligned .text section
     address, not the unaligned .empty section address */
  .empty : { 
	EMPTY_START = ABSOLUTE(.) ;
	*(.empty) 
	EMPTY_END = ABSOLUTE(.) ;
	} : text
  .text : { *(.text) } :text
  .data : { *(.data)
	LONG(EMPTY_START) ;
	 } :data
	
  /DISCARD/ : { *(.*) }
}
