cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})
ENTRY("\$START\$")
${RELOCATING+${LIB_SEARCH_DIRS}}
SECTIONS
{
  .text ${RELOCATING+${TEXT_START_ADDR}}:
  {
    ${RELOCATING+__text_start = .};
    CREATE_OBJECT_SYMBOLS
    *(.hppa_linker_stubs)
    *(.text)
  }
  .data  ${RELOCATING+ 0x40000000 } :
  {
    ${RELOCATING+__data_start = .};
    ${RELOCATING+ . = . + 0x1000 };
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
  }
  .bss ${RELOCATING+SIZEOF(.data) + ADDR(.data)} :
  {
   *(.bss)
   *(COMMON)
   ${RELOCATING+_end = . };
   ${RELOCATING+__end = . };
  }
}
EOF
