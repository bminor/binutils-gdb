# Linker script for Sparc COFF.
# Based on i386coff.sc by Ian Taylor <ian@cygnus.com>.
test -z "$ENTRY" && ENTRY=_start
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

ENTRY(${ENTRY})

SECTIONS
{
  .text ${RELOCATING+ SIZEOF_HEADERS} : {
    *(.init)
    *(.text)
    *(.fini)
    ${RELOCATING+ etext  =  .};
  }
  .data ${RELOCATING+ 0x400000 + (. & 0xffc00fff)} : {
    *(.data)
    ${RELOCATING+ edata  =  .};
  }
  .bss ${RELOCATING+ SIZEOF(.data) + ADDR(.data)} :
  { 					
    *(.bss)
    *(COMMON)
    ${RELOCATING+ end = .};
  }
  .stab  . (NOLOAD) : 
  {
    [ .stab ]
  }
  .stabstr  . (NOLOAD) :
  {
    [ .stabstr ]
  }
}
EOF
