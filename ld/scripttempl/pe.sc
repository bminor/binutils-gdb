# Linker script for PE.

if test -z "${RELOCATEABLE_OUTPUT_FORMAT}"; then
  RELOCATEABLE_OUTPUT_FORMAT=${OUTPUT_FORMAT}
fi

cat <<EOF
${RELOCATING+OUTPUT_FORMAT(${OUTPUT_FORMAT})}
${RELOCATING-OUTPUT_FORMAT(${RELOCATEABLE_OUTPUT_FORMAT})}

${LIB_SEARCH_DIRS}

ENTRY(_mainCRTStartup)

SECTIONS
{
  .text ${RELOCATING+ __image_base__ + __section_alignment__ } : 
  {
    ${RELOCATING+ *(.init)}
    *(.text)
    ${RELOCATING+*(.text\$*)}
    ${CONSTRUCTING+ ___CTOR_LIST__ = .; __CTOR_LIST__ = . ; 
			LONG (-1); *(.ctors); *(.ctor); LONG (0); }
    ${CONSTRUCTING+ ___DTOR_LIST__ = .; __DTOR_LIST__ = . ; 
			LONG (-1); *(.dtors); *(.dtor);  LONG (0); }
    ${RELOCATING+ *(.fini)}
    /* ??? Why is .gcc_exc here?  */
    ${RELOCATING+ *(.gcc_exc)}
    ${RELOCATING+ etext = .;}
    *(.gcc_except_table)
  }

  .data ${RELOCATING+BLOCK(__section_alignment__)} : 
  {
    ${RELOCATING+__data_start__ = . ;}
    *(.data)
    *(.data2)
    ${RELOCATING+*(.data\$*)}
    ${RELOCATING+__data_end__ = . ;}
    ${RELOCATING+*(.data_cygwin_nocopy)}
  }

  .bss ${RELOCATING+BLOCK(__section_alignment__)} :
  {
    ${RELOCATING+__bss_start__ = . ;}
    *(.bss)
    *(COMMON)
    ${RELOCATING+__bss_end__ = . ;}
  }

  .rdata ${RELOCATING+BLOCK(__section_alignment__)} :
  {
    *(.rdata)
    ${RELOCATING+*(.rdata\$*)}
    *(.eh_frame)
  }

  .edata ${RELOCATING+BLOCK(__section_alignment__)} :
  {
    *(.edata)
  }

  /DISCARD/ :
  {
    *(.debug\$S)
    *(.debug\$T)
    *(.debug\$F)
    *(.drectve)
  }

  .idata ${RELOCATING+BLOCK(__section_alignment__)} :
  {
    /* This cannot currently be handled with grouped sections.
	See pe.em:sort_sections.  */
    *(.idata\$2)
    *(.idata\$3)
    ${RELOCATING+ /* These zeroes mark the end of the import list.  */}
    ${RELOCATING+ LONG (0); LONG (0); LONG (0); LONG (0); LONG (0);}
    *(.idata\$4)
    *(.idata\$5)
    *(.idata\$6)
    *(.idata\$7)
  }
  .CRT ${RELOCATING+BLOCK(__section_alignment__)} :
  { 					
    ${RELOCATING+*(.CRT\$*)}
  }

  .endjunk ${RELOCATING+BLOCK(__section_alignment__)} :
  {
    /* end is deprecated, don't use it */
    ${RELOCATING+ end = .;}
    ${RELOCATING+ __end__ = .;}
  }

  .reloc ${RELOCATING+BLOCK(__section_alignment__)} :
  { 					
    *(.reloc)
  }

  .rsrc ${RELOCATING+BLOCK(__section_alignment__)} :
  { 					
    *(.rsrc)
    ${RELOCATING+*(.rsrc\$*)}
  }

  .stab ${RELOCATING+BLOCK(__section_alignment__)} ${RELOCATING+(NOLOAD)} :
  {
    [ .stab ]
  }

  .stabstr ${RELOCATING+BLOCK(__section_alignment__)} ${RELOCATING+(NOLOAD)} :
  {
    [ .stabstr ]
  }

}
EOF
