cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(z8002)
ENTRY(_start)

SECTIONS 				
{ 					
.text : { 					
	  *(.text) 				
	  *(.strings)
	  *(.rdata)
	}

.ctors   : 
	{
	  ${RELOCATING+ ___ctors = . ;  }
	  *(.ctors);
	  ${RELOCATING+ ___ctors_end = . ; }
	  ___dtors = . ;
	  *(.dtors);
	  ${RELOCATING+ ___dtors_end = . ; }
	} 

.data : {
	*(.data)
	}

.bss : 
  {
    ${RELOCATING+ __start_bss = . ; }
    *(.bss);
    *(COMMON);
    ${RELOCATING+ __end_bss = . ; }
  }

.heap : {
	${RELOCATING+ __start_heap = . ; }
	${RELOCATING+ . = . + 20k  ; }
	${RELOCATING+ __end_heap = . ; }
	} 

.stack ${RELOCATING+ 0xf000 }  : 
	{
	${RELOCATING+ _stack = . ; }
	*(.stack)
	${RELOCATING+ __stack_top = . ; }
	} 

}
EOF




