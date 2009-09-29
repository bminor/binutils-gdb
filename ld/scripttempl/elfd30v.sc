
CTOR=".ctors  : 
  {
    ${CONSTRUCTING+ __CTOR_LIST__ = .; }
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */

    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))

    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */

    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    ${CONSTRUCTING+ __CTOR_END__ = .; }
  } ${RELOCATING+ > ${DATA_MEMORY}}"

DTOR="  .dtors	 :
  {
    ${CONSTRUCTING+ __DTOR_LIST__ = .; }
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    ${CONSTRUCTING+ __DTOR_END__ = .; }
  } ${RELOCATING+ > ${DATA_MEMORY}}"

cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY
{
  text ${TEXT_DEF_SECTION} : ORIGIN = ${TEXT_START_ADDR}, LENGTH = ${TEXT_SIZE}
  data ${DATA_DEF_SECTION} : ORIGIN = ${DATA_START_ADDR}, LENGTH = ${DATA_SIZE}
  emem ${EMEM_DEF_SECTION} : ORIGIN = ${EMEM_START_ADDR}, LENGTH = ${EMEM_SIZE}
  eit			   : ORIGIN = ${EIT_START_ADDR},  LENGTH = ${EIT_SIZE}
}

SECTIONS
{
  /* Read-only sections, merged into text segment: */
  ${TEXT_DYNAMIC+${DYNAMIC}}
  .hash			 : { *(.hash) }
  .dynsym		 : { *(.dynsym) }
  .dynstr		 : { *(.dynstr) }
  .gnu.version		 : { *(.gnu.version) }
  .gnu.version_d	 : { *(.gnu.version_d) }
  .gnu.version_r	 : { *(.gnu.version_r) }

  .rel.text		 : { *(.rel.text) *(.rel.gnu.linkonce.t*) }
  .rela.text		 : { *(.rela.text) *(.rela.gnu.linkonce.t*) }
  .rel.data		 : { *(.rel.data) *(.rel.gnu.linkonce.d*) }
  .rela.data		 : { *(.rela.data) *(.rela.gnu.linkonce.d*) }
  .rel.rodata		 : { *(.rel.rodata) *(.rel.gnu.linkonce.r*) }
  .rela.rodata		 : { *(.rela.rodata) *(.rela.gnu.linkonce.r*) }
  .rel.stext		 : { *(.rel.stest) }
  .rela.stext		 : { *(.rela.stest) }
  .rel.etext		 : { *(.rel.etest) }
  .rela.etext		 : { *(.rela.etest) }
  .rel.sdata		 : { *(.rel.sdata) }
  .rela.sdata		 : { *(.rela.sdata) }
  .rel.edata		 : { *(.rel.edata) }
  .rela.edata		 : { *(.rela.edata) }
  .rel.eit_v		 : { *(.rel.eit_v) }
  .rela.eit_v		 : { *(.rela.eit_v) }
  .rel.sbss		 : { *(.rel.sbss) }
  .rela.sbss		 : { *(.rela.sbss) }
  .rel.ebss		 : { *(.rel.ebss) }
  .rela.ebss		 : { *(.rela.ebss) }
  .rel.srodata		 : { *(.rel.srodata) }
  .rela.srodata		 : { *(.rela.srodata) }
  .rel.erodata		 : { *(.rel.erodata) }
  .rela.erodata		 : { *(.rela.erodata) }
  .rel.got		 : { *(.rel.got) }
  .rela.got		 : { *(.rela.got) }
  .rel.ctors		 : { *(.rel.ctors) }
  .rela.ctors		 : { *(.rela.ctors) }
  .rel.dtors		 : { *(.rel.dtors) }
  .rela.dtors		 : { *(.rela.dtors) }
  .rel.init		 : { *(.rel.init) }
  .rela.init		 : { *(.rela.init) }
  .rel.fini		 : { *(.rel.fini) }
  .rela.fini		 : { *(.rela.fini) }
  .rel.bss		 : { *(.rel.bss) }
  .rela.bss		 : { *(.rela.bss) }
  .rel.plt		 : { *(.rel.plt) }
  .rela.plt		 : { *(.rela.plt) }

  .init			 : { *(.init) } =${NOP-0}
  ${DATA_PLT-${PLT}}

  /* Internal text space */
  .stext	 : { *(.stext) }		${RELOCATING+ > text}

  /* Internal text space or external memory */
  .text :
  {
    *(.text)
    *(.gnu.linkonce.t*)
    *(.init)
    *(.fini)
    ${RELOCATING+ _etext = . ; }
  } ${RELOCATING+ > ${TEXT_MEMORY}}

  /* Internal data space */
  .srodata	 : { *(.srodata) }	${RELOCATING+ > data}
  .sdata	 : { *(.sdata) }		${RELOCATING+ > data}

  /* Internal data space or external memory */
  .rodata	 : { *(.rodata) }	${RELOCATING+ > ${DATA_MEMORY}}

  /* C++ exception support.  */
  .eh_frame	 : { KEEP (*(.eh_frame)) }	${RELOCATING+ > ${DATA_MEMORY}}
  .gcc_except_table  : { *(.gcc_except_table) }	${RELOCATING+ > ${DATA_MEMORY}}

  /* Java class registration support.  */
  .jcr		 : { KEEP (*(.jcr)) }	${RELOCATING+ >${DATA_MEMORY}}

  ${RELOCATING+${CTOR}}
  ${RELOCATING+${DTOR}}

  .data		 :
  {
    *(.data)
    *(.gnu.linkonce.d*)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+ _edata = . ; }
  } ${RELOCATING+ > ${DATA_MEMORY}}

  /* External memory */
  .etext	 :
  {
    ${RELOCATING+ PROVIDE (__etext_start = .) ; }
    *(.etext)
    ${RELOCATING+ PROVIDE (__etext_end = .) ; }
  } ${RELOCATING+ > emem}

  .erodata	 : { *(.erodata) }	${RELOCATING+ > emem}
  .edata	 : { *(.edata) }		${RELOCATING+ > emem}

  .sbss		 :
  {
    ${RELOCATING+ PROVIDE (__sbss_start = .) ; }
    *(.sbss)
    ${RELOCATING+ PROVIDE (__sbss_end = .) ;  }
  } ${RELOCATING+ > data}

  .ebss		 :
  {
    ${RELOCATING+ PROVIDE (__ebss_start = .) ; }
    *(.ebss)
    ${RELOCATING+ PROVIDE (__ebss_end = .) ;  }
  } ${RELOCATING+ > data}

  .bss		 :
  {
    ${RELOCATING+ PROVIDE (__bss_start = .) ; }
    *(.bss)
    *(COMMON)
    ${RELOCATING+ PROVIDE (__bss_end = .) ; }
    ${RELOCATING+ _end = . ;  }
  } ${RELOCATING+ > ${DATA_MEMORY}}

  .eit_v	 :
  {
    ${RELOCATING+ PROVIDE (__eit_start = .) ; }
    *(.eit_v)
    ${RELOCATING+ PROVIDE (__eit_end = .) ; }
  } ${RELOCATING+ > eit}

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
  .debug_info     0 : { *(.debug_info) *(.gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }

  PROVIDE (__stack = ${STACK_START_ADDR});
}
EOF




