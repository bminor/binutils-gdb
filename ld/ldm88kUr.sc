SCRIPT 
SEARCH_DIR(/lib) 			
SEARCH_DIR(/usr/lib) 			
SEARCH_DIR(/usr/local/lib) 		
SECTIONS 				
{ 					
  .text 0: 				
  { 					
   CREATE_OBJECT_SYMBOLS                
    *(.text) 				
    } 					
  .data SIZEOF(.text) + ADDR(.text) :	
  { 					
    *(.data) 				
    ___DTOR_LIST__ = . ;               	
    LONG((___CTOR_LIST__ - .) / 4 -2)  	
    *(___DTOR_LIST__)                   
    LONG(0)                             
    ___CTOR_LIST__ = . ;               	
    LONG((___end_list__  - .) / 4 -2)     
    *(___CTOR_LIST__)                   
    LONG(0)                             
    ___end_list__  =  . ;			
    } 					
  .bss SIZEOF(.data) + ADDR(.data) :	
  {					
    *(.bss)				
   [COMMON]				
    } 					
} 					
ENDSCRIPT 
