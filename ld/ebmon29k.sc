OUTPUT_FORMAT("coff-a29k-big")
ENTRY(start)

SECTIONS {
	.text 0x8000 : 
		{
		 *(.text);
		_etext = . ;
		}
	.data 0x80002000 :
		{
		 *(.data);
		 *(.mstack); 
		 *(.shbss);
		 *(.rstack);
		*(.mstack);
		CONSTRUCTORS
		}
	.bss  . :
		{ 

		[COMMON] 	
		*(.bss);
		_end = .;
		 } 

}
