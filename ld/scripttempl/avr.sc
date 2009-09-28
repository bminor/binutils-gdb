cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}","${OUTPUT_FORMAT}","${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY
{
  text   (rx)   : ORIGIN = 0, LENGTH = $TEXT_LENGTH
  data   (rw!x) : ORIGIN = $DATA_ORIGIN, LENGTH = $DATA_LENGTH
  eeprom (rw!x) : ORIGIN = 0x810000, LENGTH = 64K
}

SECTIONS
{
  /* Read-only sections, merged into text segment: */
  ${TEXT_DYNAMIC+${DYNAMIC}}
  .hash         : { *(.hash)		}
  .dynsym       : { *(.dynsym)		}
  .dynstr       : { *(.dynstr)		}
  .gnu.version  : { *(.gnu.version)	}
  .gnu.version_d  : { *(.gnu.version_d)	}
  .gnu.version_r  : { *(.gnu.version_r)	}

  .rel.init     : { *(.rel.init)		}
  .rela.init    : { *(.rela.init)	}
  .rel.text     :
    {
      *(.rel.text)
      ${RELOCATING+*(.rel.text.*)}
      ${RELOCATING+*(.rel.gnu.linkonce.t*)}
    }
  .rela.text    :
    {
      *(.rela.text)
      ${RELOCATING+*(.rela.text.*)}
      ${RELOCATING+*(.rela.gnu.linkonce.t*)}
    }
  .rel.fini     : { *(.rel.fini)		}
  .rela.fini    : { *(.rela.fini)	}
  .rel.rodata   :
    {
      *(.rel.rodata)
      ${RELOCATING+*(.rel.rodata.*)}
      ${RELOCATING+*(.rel.gnu.linkonce.r*)}
    }
  .rela.rodata  :
    {
      *(.rela.rodata)
      ${RELOCATING+*(.rela.rodata.*)}
      ${RELOCATING+*(.rela.gnu.linkonce.r*)}
    }
  .rel.data     :
    {
      *(.rel.data)
      ${RELOCATING+*(.rel.data.*)}
      ${RELOCATING+*(.rel.gnu.linkonce.d*)}
    }
  .rela.data    :
    {
      *(.rela.data)
      ${RELOCATING+*(.rela.data.*)}
      ${RELOCATING+*(.rela.gnu.linkonce.d*)}
    }
  .rel.ctors    : { *(.rel.ctors)	}
  .rela.ctors   : { *(.rela.ctors)	}
  .rel.dtors    : { *(.rel.dtors)	}
  .rela.dtors   : { *(.rela.dtors)	}
  .rel.got      : { *(.rel.got)		}
  .rela.got     : { *(.rela.got)		}
  .rel.bss      : { *(.rel.bss)		}
  .rela.bss     : { *(.rela.bss)		}
  .rel.plt      : { *(.rel.plt)		}
  .rela.plt     : { *(.rela.plt)		}

  /* Internal text space or external memory.  */
  .text  :
  {
    *(.vectors)
    KEEP(*(.vectors))

    /* For data that needs to reside in the lower 64k of progmem.  */
    *(.progmem.gcc*)
    *(.progmem*)
    ${RELOCATING+. = ALIGN(2);}

    ${CONSTRUCTING+ __trampolines_start = . ; }
    /* The jump trampolines for the 16-bit limited relocs will reside here.  */
    *(.trampolines)
    *(.trampolines*)
    ${CONSTRUCTING+ __trampolines_end = . ; }

    /* For future tablejump instruction arrays for 3 byte pc devices.
       We don't relax jump/call instructions within these sections.  */
    *(.jumptables) 
    *(.jumptables*) 

    /* For code that needs to reside in the lower 128k progmem.  */
    *(.lowtext)
    *(.lowtext*)

    ${CONSTRUCTING+ __ctors_start = . ; }
    ${CONSTRUCTING+ *(.ctors) }
    ${CONSTRUCTING+ __ctors_end = . ; }
    ${CONSTRUCTING+ __dtors_start = . ; }
    ${CONSTRUCTING+ *(.dtors) }
    ${CONSTRUCTING+ __dtors_end = . ; }
    KEEP(SORT(*)(.ctors))
    KEEP(SORT(*)(.dtors))

    /* From this point on, we don't bother about wether the insns are
       below or above the 16 bits boundary.  */
    *(.init0)  /* Start here after reset.  */
    KEEP (*(.init0))
    *(.init1)
    KEEP (*(.init1))
    *(.init2)  /* Clear __zero_reg__, set up stack pointer.  */
    KEEP (*(.init2))
    *(.init3)
    KEEP (*(.init3))
    *(.init4)  /* Initialize data and BSS.  */
    KEEP (*(.init4))
    *(.init5)
    KEEP (*(.init5))
    *(.init6)  /* C++ constructors.  */
    KEEP (*(.init6))
    *(.init7)
    KEEP (*(.init7))
    *(.init8)
    KEEP (*(.init8))
    *(.init9)  /* Call main().  */
    KEEP (*(.init9))
    *(.text)
    ${RELOCATING+. = ALIGN(2);}
    *(.text.*)
    ${RELOCATING+. = ALIGN(2);}
    *(.fini9)  /* _exit() starts here.  */
    KEEP (*(.fini9))
    *(.fini8)
    KEEP (*(.fini8))
    *(.fini7)
    KEEP (*(.fini7))
    *(.fini6)  /* C++ destructors.  */
    KEEP (*(.fini6))
    *(.fini5)
    KEEP (*(.fini5))
    *(.fini4)
    KEEP (*(.fini4))
    *(.fini3)
    KEEP (*(.fini3))
    *(.fini2)
    KEEP (*(.fini2))
    *(.fini1)
    KEEP (*(.fini1))
    *(.fini0)  /* Infinite loop after program termination.  */
    KEEP (*(.fini0))
    ${RELOCATING+ _etext = . ; }
  } ${RELOCATING+ > text}

  .data	 : ${RELOCATING+AT (ADDR (.text) + SIZEOF (.text))}
  {
    ${RELOCATING+ PROVIDE (__data_start = .) ; }
    *(.data)
    *(.data*)
    *(.rodata)  /* We need to include .rodata here if gcc is used */
    *(.rodata*) /* with -fdata-sections.  */
    *(.gnu.linkonce.d*)
    ${RELOCATING+. = ALIGN(2);}
    ${RELOCATING+ _edata = . ; }
    ${RELOCATING+ PROVIDE (__data_end = .) ; }
  } ${RELOCATING+ > data}

  .bss  :${RELOCATING+ AT (ADDR (.bss))}
  {
    ${RELOCATING+ PROVIDE (__bss_start = .) ; }
    *(.bss)
    *(.bss*)
    *(COMMON)
    ${RELOCATING+ PROVIDE (__bss_end = .) ; }
  } ${RELOCATING+ > data}

  ${RELOCATING+ __data_load_start = LOADADDR(.data); }
  ${RELOCATING+ __data_load_end = __data_load_start + SIZEOF(.data); }

  /* Global data not cleared after reset.  */
  .noinit :
  {
    ${RELOCATING+ PROVIDE (__noinit_start = .) ; }
    *(.noinit*)
    ${RELOCATING+ PROVIDE (__noinit_end = .) ; }
    ${RELOCATING+ _end = . ;  }
    ${RELOCATING+ PROVIDE (__heap_start = .) ; }
  } ${RELOCATING+ > data}

  .eeprom :
  {
    *(.eeprom*)
    ${RELOCATING+ __eeprom_end = . ; }
  } ${RELOCATING+ > eeprom}

  /* Stabs debugging sections.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
 
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }

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
}
EOF

