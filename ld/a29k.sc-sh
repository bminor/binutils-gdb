cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
INPUT(/lab3/u3/sym1/tools/usr/lib/segments.o) /* Has .rstack/.mstack */ 
${LIB_SEARCH_DIRS}

MEMORY {
	text   	: ORIGIN = 0x1000000, LENGTH = 0x1000000
	talias 	: ORIGIN = 0x2000000, LENGTH = 0x1000000
	data	: ORIGIN = 0x3000000, LENGTH = 0x1000000
	mstack 	: ORIGIN = 0x4000000, LENGTH = 0x1000000
	rstack 	: ORIGIN = 0x5000000, LENGTH = 0x1000000
}
SECTIONS
{
  .text : {
    *(.text)
    ${RELOCATING+ __etext  =  .};
    *(.lit)
    *(.shdata)
  } ${RELOCATING+ > text}
  .shbss SIZEOF(.text) + ADDR(.text) :	{
    *(.shbss)
  } 
  .talias :	 { } ${RELOCATING+ > talias}
  .data  : {
    *(.data)
    ${RELOCATING+ __edata  =  .};
  } ${RELOCATING+ > data}
  .bss   SIZEOF(.data) + ADDR(.data) :
  { 					
   *(.bss)
   *(COMMON)
     ${RELOCATING+ __end = ALIGN(0x8)};
  }
  .mstack  : { } ${RELOCATING+ > mstack}
  .rstack  : { } ${RELOCATING+ > rstack}
}
EOF
