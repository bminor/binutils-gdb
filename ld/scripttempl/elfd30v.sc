cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY
{
  text : ORIGIN = ${TEXT_START_ADDR}, LENGTH = ${TEXT_SIZE}
  data : ORIGIN = ${DATA_START_ADDR}, LENGTH = ${DATA_SIZE}
  emem : ORIGIN = ${EMEM_START_ADDR}, LENGTH = ${EMEM_SIZE}
}

SECTIONS
{
  /* Internal text space */
  .stext	${RELOCATING-0} :
  {
    ${RELOCATING+ PROVIDE (__stext_start = .) ; }
    *(.stext)
    ${RELOCATING+ PROVIDE (__stext_end = .) ; }
  } ${RELOCATING+ > text}

  /* Internal text space or external memory */
  .text :
  {
    ${RELOCATING+ PROVIDE (__text_start = .) ; }
    *(.text)
    *(.gnu.linkonce.t*)
    *(.init)
    *(.fini)
    ${RELOCATING+ PROVIDE (__text_end = .) ; }
    ${RELOCATING+ _etext = . ; }
  } ${RELOCATING+ > ${TEXT_MEMORY}}

  /* Internal data space */
  .sdata	${RELOCATING-0} :
  {
    ${RELOCATING+ PROVIDE (__sdata_start = .) ; }
    *(.sdata)
    ${RELOCATING+ PROVIDE (__sdata_end = .) ; }
  } ${RELOCATING+ > data}

  /* Internal data space or external memory */
  .strings	${RELOCATING-0} : { *(.strings) }	${RELOCATING+ > ${DATA_MEMORY}}
  .rodata	${RELOCATING-0} : { *(.rodata) }	${RELOCATING+ > ${DATA_MEMORY}}
  .rodata1	${RELOCATING-0} : { *(.rodata1) }	${RELOCATING+ > ${DATA_MEMORY}}
  .data1	${RELOCATING-0} : { *(.data1) }		${RELOCATING+ > ${DATA_MEMORY}}

  .ctors	${RELOCATING-0} :
  {
    ${CONSTRUCTING+${CTOR_START}}
    *(.ctors)
    ${CONSTRUCTING+${CTOR_END}}
  } ${RELOCATING+ > ${DATA_MEMORY}}

  .dtors	${RELOCATING-0} :
  {
    ${CONSTRUCTING+${DTOR_START}}
    *(.dtors)
    ${CONSTRUCTING+${DTOR_END}}
  } ${RELOCATING+ > ${DATA_MEMORY}}

  .data		${RELOCATING-0} :
  {
    *(.data)
    *(.gnu.linkonce.d*)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+ _edata = . ; }
  } ${RELOCATING+ > ${DATA_MEMORY}}

  /* External memory */
  .etext	${RELOCATING-0} :
  {
    ${RELOCATING+ PROVIDE (__etext_start = .) ; }
    *(.etext)
    ${RELOCATING+ PROVIDE (__etext_end = .) ; }
  } ${RELOCATING+ > emem}

  .edata	${RELOCATING-0} :
  {
    ${RELOCATING+ PROVIDE (__edata_start = .) ; }
    *(.edata)
    ${RELOCATING+ PROVIDE (__edata_end = .) ; }
  } ${RELOCATING+ > emem}

  .sbss		${RELOCATING-0} :
  {
    ${RELOCATING+ PROVIDE (__sbss_start = .) ; }
    *(.sbss)
    ${RELOCATING+ PROVIDE (__sbss_end = .) ;  }
  } ${RELOCATING+ > data}

  .ebss		${RELOCATING-0} :
  {
    ${RELOCATING+ PROVIDE (__ebss_start = .) ; }
    *(.ebss)
    ${RELOCATING+ PROVIDE (__ebss_end = .) ;  }
  } ${RELOCATING+ > data}

  .bss		${RELOCATING-0} :
  {
    ${RELOCATING+ PROVIDE (__bss_start = .) ; }
    *(.bss)
    *(COMMON)
    ${RELOCATING+ PROVIDE (__bss_end = .) ; }
    ${RELOCATING+ _end = . ;  }
  } ${RELOCATING+ > ${DATA_MEMORY}}

  /* Stabs debugging sections.  */
  .stab		 0 : { *(.stab) }
  .stabstr	 0 : { *(.stabstr) }
  .stab.excl	 0 : { *(.stab.excl) }
  .stab.exclstr	 0 : { *(.stab.exclstr) }
  .stab.index	 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }

  .comment	 0 : { *(.comment) }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug	 0 : { *(.debug) }
  .line		 0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo 0 : { *(.debug_srcinfo) }
  .debug_sfnames 0 : { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

  PROVIDE (__stack = ${STACK_START_ADDR});
}
EOF




