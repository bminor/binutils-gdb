/* $Id$ 
*/

OUTPUT_FORMAT("i386coff")

ENTRY(_start)
SECTIONS 				
{ 	
  .text 0x000000d0 : {
    *(.init)				
    *(.text)	
    *(.fini)      	
    etext  =  .; 
  }
  .data ((ADDR(.text) + SIZEOF(.text)) & 0x0fff) + ALIGN(0x00400000) : {
   	*(.data) 				
	edata  =  .; 			
  }
  .bss   (SIZEOF(.data) + ADDR(.data)) : {
	*(.bss)	 			
	[COMMON] 				
	end = ALIGN(0x8);
  } 
  .comment : { *(.comment) }
}

