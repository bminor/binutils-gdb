cat <<EOF
TARGET(m88kbcs)
OUTPUT_FORMAT(m88kbcs)
OUTPUT_ARCH(${ARCH})
ENTRY(__start)
${RELOCATING+${LIB_SEARCH_DIRS}}

SECTIONS 				
{ 					
  .text ${RELOCATING+ 0x10000 + SIZEOF_HEADERS} :
    {
      CREATE_OBJECT_SYMBOLS
      ${RELOCATING+ __.text.start = .};
      ${RELOCATING+ __.init.start = .};
      ${RELOCATING+ LONG(0xf400c001)}
      ${RELOCATING+ __.init.end = .};
      *(.text) 				
      ${RELOCATING+ __.tdesc_start = .};
      ${RELOCATING+ *(.tdesc)}
      ${RELOCATING+ __.text_end = .}	;
      ${RELOCATING+ __.initp.start = .};
      ${RELOCATING+ __.initp.end =.};

      ${RELOCATING+_etext =.};
    }  					
  .data ${RELOCATING+ SIZEOF(.text) + ADDR(.text) + 0x400000} :
    { 					
      *(.data)
      ${CONSTRUCTING+CONSTRUCTORS}
      *(.comment)
      ${RELOCATING+_edata  =  .};
    }  					
  .bss ${RELOCATING+ SIZEOF(.data) + ADDR(.data)} :
    { 		
      *(.bss)	
      *(COMMON) 	
      ${RELOCATING+ _end = .};
      ${RELOCATING+ __end = .};
    } 					
}
EOF
