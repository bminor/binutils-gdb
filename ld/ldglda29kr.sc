OUTPUT_FORMAT("coff-a29k-big")
SEARCH_DIR(/lab3/u3/sym1/tools/lib)
SEARCH_DIR(/lab3/u3/sym1/tools/usr/lib)
SEARCH_DIR(/lab3/u3/sym1/tools/usr/local/lib)
MEMORY {
	text   	: ORIGIN = 0x1000000, LENGTH = 0x1000000
	talias 	: ORIGIN = 0x2000000, LENGTH = 0x1000000
	data	: ORIGIN = 0x3000000, LENGTH = 0x1000000
	mstack 	: ORIGIN = 0x4000000, LENGTH = 0x1000000
	rstack 	: ORIGIN = 0x5000000, LENGTH = 0x1000000
}
SECTIONS 				
{ 					
  .text 0 : { 					
    *(.text)
    __etext  =  .;       	
    *(.lit) 				
    *(.shdata)  
  } > text
  .shbss SIZEOF(.text) + ADDR(.text) :	{
    *(.shbss) 				
  } 
  .data  SIZEOF(.shbss) + ADDR(.shbss): { 					
    *(.data) 				
  } 
  .bss   SIZEOF(.data) + ADDR(.data) :	
  { 					
   *(.bss)	 			
   [COMMON] 				
  } 
}

