cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

${RELOCATING+${LIB_SEARCH_DIRS}}
${STACKZERO+${RELOCATING+${STACKZERO}}}
${SHLIB_PATH+${RELOCATING+${SHLIB_PATH}}}
SECTIONS
{
  .text ${RELOCATING+${TEXT_START_ADDR}}:
  {
    CREATE_OBJECT_SYMBOLS
    *(.text)
    /* The next six sections are for SunOS dynamic linking.  The order
       is important.  */
    *(.dynrel)
    *(.hash)
    *(.dynsym)
    *(.dynstr)
    *(.rules)
    *(.need)
    ${PAD_TEXT+${RELOCATING+. = ${DATA_ALIGNMENT};}}
    ${RELOCATING+_etext = ${DATA_ALIGNMENT};}
  }
  .data  ${RELOCATING+${DATA_ALIGNMENT}} :
  {
    /* The first three sections are for SunOS dynamic linking.  */
    *(.dynamic)
    *(.got)
    *(.plt)
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+_edata  =  .;}
  }
  .bss ${RELOCATING+SIZEOF(.data) + ADDR(.data)} :
  {
   ${RELOCATING+ __bss_start = .};
   *(.bss)
   *(COMMON)
   ${RELOCATING+_end = ALIGN(4) };
   ${RELOCATING+__end = ALIGN(4) };
  }
}
EOF
