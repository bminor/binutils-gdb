cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})
ENTRY("\$START\$")
${RELOCATING+${LIB_SEARCH_DIRS}}
${RELOCATING+___stack_zero = ABSOLUTE(0x2000);}
SECTIONS
{
  .text ${RELOCATING+${TEXT_START_ADDR}}:
  {
    ${RELOCATING+__text_start = .};
    CREATE_OBJECT_SYMBOLS
    *(.hppa_linker_stubs)
    *(.text)
    ${RELOCATING+etext = .};
    ${RELOCATING+_etext = .};
  }
  .data  ${RELOCATING+ 0x40000000 } :
  {
    ${RELOCATING+ . = . + 0x1000 };
    ${RELOCATING+__data_start = .};
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+edata = .};
    ${RELOCATING+_edata = .};
  }
  .bss ${RELOCATING+SIZEOF(.data) + ADDR(.data)} :
  {
   *(.bss)
   *(COMMON)
   ${RELOCATING+end = . };
   ${RELOCATING+_end = . };
  }
}
EOF
