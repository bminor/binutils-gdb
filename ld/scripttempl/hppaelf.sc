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
    *(.PARISC.stubs)
    *(.text)
    *(.PARISC.unwind)
    ${RELOCATING+etext = .};
    ${RELOCATING+_etext = .};
  }
  .data  0x40000000 :
  {
    ${RELOCATING+ . = . + 0x1000 };
    ${RELOCATING+__data_start = .};
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+edata = .};
    ${RELOCATING+_edata = .};
  }
  .bss 0x40000000 ${RELOCATING++SIZEOF(.data)} :
  {
   *(.bss)
   *(COMMON)
   ${RELOCATING+end = . };
   ${RELOCATING+_end = . };
  }
}
EOF
