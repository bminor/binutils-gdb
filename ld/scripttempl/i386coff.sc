# Linker script for 386 COFF.  This works on SVR3.2 and SCO Unix 3.2.2.
# .data2 handles SCO, which uses two data sections.
# Ian Taylor <ian@cygnus.com>.
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

ENTRY(_start)

SECTIONS
{
  .text ${RELOCATING+ SIZEOF_HEADERS} : {
    *(.init)
    *(.text)
    *(.fini)
    ${RELOCATING+ etext  =  .};
  }
  .data ${RELOCATING+ 0x400000 + (. & 0xffc00fff)} : {
    *(.data .data2)
    ${RELOCATING+ edata  =  .};
  }
  .bss ${RELOCATING+ SIZEOF(.data) + ADDR(.data)} :
  { 					
    *(.bss)
    *(COMMON)
    ${RELOCATING+ end = .};
  }
}
EOF
