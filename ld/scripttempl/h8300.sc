cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY
{
	/* 0xc4 is a magic entry.  We should have the linker just
	   skip over it one day... */
	vectors : o = 0x0000, l = 196
	magicvectors : o = 0xc4, l = 60
	ram    : o = 0x0100, l = 65536 - 256 - 4
	topram : o = 0xfffc, l = 4
}

SECTIONS 				
{ 					
.vectors : {
/* Use something like this to place a specific function's address
   into the vector table. 

	SHORT(ABSOLUTE(_foobar))
*/

	*(.vectors)
        } ${RELOCATING+ > vectors}

.text :	{ 					
	*(.text) 				
	*(.strings)
   	${RELOCATING+ _etext = . ; }
	} ${RELOCATING+ > ram}
.tors : {
	___ctors = . ;
	*(.ctors)
	___ctors_end = . ;
	___dtors = . ;
	*(.dtors)
	___dtors_end = . ;
	} ${RELOCATING+ > ram}
.data : {
	*(.data)
	${RELOCATING+ _edata = . ; }
	} ${RELOCATING+ > ram}
.bss : {
	${RELOCATING+ _bss_start = . ;}
	*(.bss)
	*(COMMON)
	${RELOCATING+ _end = . ;  }
	} ${RELOCATING+ >ram}
.stack : {
	${RELOCATING+ _stack = . ; }
	*(.stack)
	} ${RELOCATING+ > topram}
.stab 0 ${RELOCATING+(NOLOAD)} : {
	[ .stab ]
	}
.stabstr 0 ${RELOCATING+(NOLOAD)} : {
	[ .stabstr ]
	}
}
EOF
