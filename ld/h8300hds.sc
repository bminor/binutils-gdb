OUTPUT_FORMAT("coff-h8300")
OUTPUT_ARCH(h8300)

SECTIONS 				
{ 					
.text 0:
	{ 					
	  *(.text) 				
	  *(.strings)
	}
.data . :
	{
	*(.data)
	}
.bss . :
	{ 					
	*(.bss)	 			
	[COMMON] 				
	 _end = .;				
	} 					
}

