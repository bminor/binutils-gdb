cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

${RELOCATING+${LIB_SEARCH_DIRS}}
${RELOCATING+__DYNAMIC  =  0;}
SECTIONS
{
  .text ${RELOCATING+${TEXT_START_ADDR}}:
  {
    CREATE_OBJECT_SYMBOLS
    *(.text)
    ${RELOCATING+etext = ${DATA_ALIGNMENT};}
  }
  .data  ${RELOCATING+${DATA_ALIGNMENT}} :
  {
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+edata  =  .;}
  }
  .bss ${RELOCATING+SIZEOF(.data) + ADDR(.data)} :
  {
   *(.bss)
   *(COMMON)
   ${RELOCATING+end = . };
  }
}
EOF
