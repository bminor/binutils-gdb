cat <<EOF
SECTIONS
{ 
    .text : 
    { 
	${GLD_STYLE+ CREATE_OBJECT_SYMBOLS}
	*(.text) 
	${RELOCATING+ _etext = .};
    }  
    .data SIZEOF(.text) + ADDR(.text):
    { 
 	*(.data) 
	${RELOCATING+ _edata = .};
    }  
    .bss SIZEOF(.data) + ADDR(.data):
    { 
	${RELOCATING+ _bss_start = .};
	*(.bss)	 
	*(COMMON) 
	${RELOCATING+ _end = .};
    } 
} 
EOF
