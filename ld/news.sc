OUTPUT_FORMAT("a.out-newsos3")
OUTPUT_ARCH(m68k)

SEARCH_DIR(/lib) 			
SEARCH_DIR(/usr/lib) 			
SEARCH_DIR(/usr/local/lib) 		
__DYNAMIC  =  0; 				
SECTIONS 				
{ 					
  .text 0:
  { 					
   CREATE_OBJECT_SYMBOLS 		
    *(.text) 				
    _etext  =  ALIGN(0x1000);
    }
  .data  ALIGN(0x1000) :		
  { 					
    *(.data) 				
      CONSTRUCTORS;
      _edata  =  .; 			
  }  					
  .bss   SIZEOF(.data) + ADDR(.data) :	
  { 					
   *(.bss)	 			
   [COMMON] 				
     _end = .;				
    } 					
}

