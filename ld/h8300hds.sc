OUTPUT_FORMAT("ieee")
OUTPUT_ARCH(h8300)

SECTIONS 				
{ 					
  .code 40:
  { 					

    *(.code) 				
   *(.strings)
    }
  .bss . :
  { 					
   *(.bss)	 			
   [COMMON] 				
     _end = .;				
    } 					
}

