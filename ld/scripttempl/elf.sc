cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

${RELOCATING+${LIB_SEARCH_DIRS}}
/* Do we need any of these for elf?
   ${RELOCATING+__DYNAMIC  =  0;}
   ${STACKZERO+${RELOCATING+${STACKZERO}}}
   ${SHLIB_PATH+${RELOCATING+${SHLIB_PATH}}}  */
SECTIONS
{
  .text ${RELOCATING+${TEXT_START_ADDR}}:
  {
    CREATE_OBJECT_SYMBOLS
    *(.init)
    *(.text)
    *(.fini)
    *(.rodata)
    ${RELOCATING+_etext = ${DATA_ALIGNMENT};}
  }
  .data  ${RELOCATING+${DATA_ALIGNMENT}} :
  {
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+_edata  =  .;}
  }
  .bss ${RELOCATING+SIZEOF(.data) + ADDR(.data)} :
  {
    ${RELOCATING+ __bss_start = .};
   *(.bss)
   *(COMMON)
   ${RELOCATING+_end = . };
   ${RELOCATING+end = . };
  }
}
EOF
