OUTPUT_FORMAT("coff-h8300")
OUTPUT_ARCH(h8300)

MEMORY {
	rom   : o = 0x0000, l = 0x7fe0 
	duart : o = 0x7fe0, l = 16
	ram   : o = 0x8000, l = 29k
	hmsram: o = 0xfb80, l = 512
	}

SECTIONS 				
{ 					
.text :
	{ 					
	  *(.text) 				
	  *(.strings)
	} > ram
.data  :
	{
	*(.data)
	} > ram
.bss  :
	{
	_bss_start = . ; 					 
	*(.bss)	 			
	*(COMMON) 				
	 _end = .;			
	} >ram			
}

