SECTIONS 
{ 
  .text : 
  { 
   CREATE_OBJECT_SYMBOLS 
    *(.text) 
      _etext = .;
    }  
 
  .data  SIZEOF(.text) + ADDR(.text):
  { 
 
    *(.data) 
      _edata = .; 
    }  
  .bss   SIZEOF(.data) + ADDR(.data) :    
  { 
	_bss_start = .;
	*(.bss)	 
	*(COMMON) 
      	_end = . ; 
   } 
} 
