# Linker script for Motorola Delta sysV68 R3V7.1
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})
${LIB_SEARCH_DIRS}

ENTRY(_start)

SECTIONS
{
  .text ${TEXT_START_ADDR} : {
    *(.init)
    *(.text)
    *(.fini)
  }
  ${RELOCATING+ etext = .;}
  .data ${RELOCATING+ 0x400000 + (. & 0xfff)} : {
    *(.data .data2)
  }
  ${RELOCATING+ edata = .;}
  .bss . :
  { 					
    *(.bss)
    *(COMMON)
  }
  ${RELOCATING+ end = .;}
}
