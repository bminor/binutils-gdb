SECTIONS 
{ 
  .text 0:
  { 
    *(.text) 
    }  
  .data SIZEOF(.text) + ADDR(.text):
  { 
    *(.data) 
    }  
  .bss   SIZEOF(.data) + ADDR(.data) :    
  { 
   *(.bss)  
   [COMMON] 
    } 
} 

