TARGET(m88kbcs)
OUTPUT_FORMAT(m88kbcs)
OUTPUT_ARCH(m88k)
ENTRY(__start)
SEARCH_DIR(/lib) 			
SEARCH_DIR(/usr/lib) 			
SEARCH_DIR(/usr/local/lib) 		
SECTIONS 				
{ 					
  .text 0x10000 + SIZEOF_HEADERS :
    { 					
      __.text.start = .;
      __.init.start = .;
      LONG(0xf400c001) 
      __.init.end = .;
      *(.text) 				
      __.tdesc_start = .;
      *(.tdesc) 			
      __.text_end = .;	
      __.initp.start = .;
      __.initp.end =.;

      _etext =.;
    }  					
  .data SIZEOF(.text) + ADDR(.text) + 0x400000:
    { 					
      *(.data) 				
	_edata  =  .; 			
    }  					
  .bss   SIZEOF(.data) + ADDR(.data) :	
    { 		
      *(.bss)	
      [COMMON] 	
      _end = .;				
      ___end = .;
    } 					
}



