OUTPUT_ARCH(sparc)
OUTPUT_FORMAT("a.out-sunos-big")
SEARCH_DIR(/lib) 			
SEARCH_DIR(/usr/lib) 			
SEARCH_DIR(/usr/local/lib) 		
__DYNAMIC = 0; 				
SECTIONS 				
{ 					
  .text 0x2020 BLOCK(0x2000): 		
    { 					
      CREATE_OBJECT_SYMBOLS 		;
      *(.text); 				
      _etext = ALIGN( 0x2000);       	
    }  					
  .data  ALIGN(0x2000)  :		
    { 					
      *(.data); 				
      CONSTRUCTORS;
      _edata  =  .; 			
    }  					
  .bss   SIZEOF(.data) + ADDR(.data) :	
    { 					
      *(.bss)	 			
	[COMMON] 				
	  _end = .;				
      __end = .;				
    } 					
}


