OUTPUT_FORMAT("coff-a29k-big")
INPUT(/lab3/u3/sym1/tools/usr/lib/segments.o) /* Has .rstack/.mstack */ 
SEARCH_DIR(/lib)
SEARCH_DIR(/usr/lib)
SEARCH_DIR(/usr/local/lib)
MEMORY {
	text   	: ORIGIN = 0x1000000, LENGTH = 0x1000000
	talias 	: ORIGIN = 0x2000000, LENGTH = 0x1000000
	data	: ORIGIN = 0x3000000, LENGTH = 0x1000000
	mstack 	: ORIGIN = 0x4000000, LENGTH = 0x1000000
	rstack 	: ORIGIN = 0x5000000, LENGTH = 0x1000000
}
SECTIONS 				
{ 					
  .text : { 					
    *(.text)
    __etext  =  .;       	
    *(.lit) 				
    *(.shdata)  
  } > text
  .shbss SIZEOF(.text) + ADDR(.text) :	{
    *(.shbss) 				
  } 
  .talias :	 { } > talias
  .data  : { 					
    *(.data) 				
     __edata  =  .; 			
  } > data
  .bss   SIZEOF(.data) + ADDR(.data) :	
  { 					
   *(.bss)	 			
   [COMMON] 				
     __end = ALIGN(0x8);				
  } 
  .mstack  : { } > mstack
  .rstack  : { } > rstack 
}

