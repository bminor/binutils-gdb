cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY {
	ram   : o = 0x1000, l = 512k
	}

SECTIONS 				
{ 					
.text :
	{ 					
	  *(.text) 				
	  *(.strings)
   	 ${RELOCATING+ _etext = . ; }
	} ${RELOCATING+ > ram}
.data  :
	{
	*(.data)
	${RELOCATING+ _edata = . ; }
	} ${RELOCATING+ > ram}
.bss  :
	{
	${RELOCATING+ _bss_start = . ;}
	*(.bss)
	*(COMMON)
	${RELOCATING+ _end = . ;  }
	} ${RELOCATING+ >ram}
.stack : 
	{
	${RELOCATING+ _stack = . ; }
	*(.stack)
	} ${RELOCATING+ > ram}
}
EOF




