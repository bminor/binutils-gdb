# Linker script for 386 go32
# DJ Delorie (dj@ctron.com)

test -z "$ENTRY" && ENTRY=_start
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

ENTRY(${ENTRY})

SECTIONS
{
  .text ${RELOCATING+ SIZEOF_HEADERS} : {
    *(.text)
    ${RELOCATING+ etext  =  .};
  }
  .data ${RELOCATING+ ALIGN(${DATA_ALIGNMENT})} : {
    ${RELOCATING+ *(.ctor)}
    ${RELOCATING+ *(.dtor)}
    *(.data)
    ${RELOCATING+ edata  =  .};
    ${RELOCATING+. = ALIGN(${PAGE_SIZE});}
  }
  .bss ${RELOCATING+ SIZEOF(.data) + ADDR(.data)} :
  { 					
    *(.bss)
    *(COMMON)
    ${RELOCATING+ end = .};
  }
}
EOF
