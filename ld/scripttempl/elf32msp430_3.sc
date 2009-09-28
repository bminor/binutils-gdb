cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}","${OUTPUT_FORMAT}","${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

MEMORY
{
  text   (rx)   : ORIGIN = $ROM_START,  LENGTH = $ROM_SIZE
  data   (rwx)  : ORIGIN = $RAM_START, 	LENGTH = $RAM_SIZE
  vectors (rw)  : ORIGIN = 0xffe0,      LENGTH = 0x20
}

SECTIONS
{
  /* Read-only sections, merged into text segment.  */
  ${TEXT_DYNAMIC+${DYNAMIC}}
  .hash         : { *(.hash)             }
  .dynsym       : { *(.dynsym)           }
  .dynstr       : { *(.dynstr)           }
  .gnu.version  : { *(.gnu.version)      }
  .gnu.version_d  : { *(.gnu.version_d)  }
  .gnu.version_r  : { *(.gnu.version_r)  }

  .rel.init     : { *(.rel.init) }
  .rela.init    : { *(.rela.init)        }
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
  .rel.fini     : { *(.rel.fini) }
  .rela.fini    : { *(.rela.fini)        }
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
  .rel.ctors    : { *(.rel.ctors)        }
  .rela.ctors   : { *(.rela.ctors)       }
  .rel.dtors    : { *(.rel.dtors)        }
  .rela.dtors   : { *(.rela.dtors)       }
  .rel.got      : { *(.rel.got)          }
  .rela.got     : { *(.rela.got)         }
  .rel.bss      : { *(.rel.bss)          }
  .rela.bss     : { *(.rela.bss)         }
  .rel.plt      : { *(.rel.plt)          }
  .rela.plt     : { *(.rela.plt)         }

  /* Internal text space.  */
  .text :
  {
    ${RELOCATING+. = ALIGN(2);}
    *(.init)
    *(.init0)  /* Start here after reset.  */
    *(.init1)
    *(.init2)
    *(.init3)
    *(.init4)
    *(.init5)
    *(.init6)  /* C++ constructors.  */
    *(.init7)
    *(.init8)
    *(.init9)  /* Call main().  */

    ${CONSTRUCTING+ __ctors_start = . ; }
    ${CONSTRUCTING+ *(.ctors) }
    ${CONSTRUCTING+ __ctors_end = . ; }
    ${CONSTRUCTING+ __dtors_start = . ; }
    ${CONSTRUCTING+ *(.dtors) }
    ${CONSTRUCTING+ __dtors_end = . ; }

    ${RELOCATING+. = ALIGN(2);}
    *(.text)
    ${RELOCATING+. = ALIGN(2);}
    *(.text.*)

    ${RELOCATING+. = ALIGN(2);}
    *(.fini9)
    *(.fini8)
    *(.fini7)
    *(.fini6)  /* C++ destructors.  */
    *(.fini5)
    *(.fini4)
    *(.fini3)
    *(.fini2)
    *(.fini1)
    *(.fini0)  /* Infinite loop after program termination.  */
    *(.fini)

    ${RELOCATING+ _etext = . ; }
  } ${RELOCATING+ > text}

  .data  : ${RELOCATING+AT (ADDR (.text) + SIZEOF (.text))}
  {  
    ${RELOCATING+ PROVIDE (__data_start = .) ; }
    *(.data)
    *(.gnu.linkonce.d*)
    ${RELOCATING+. = ALIGN(2);}
    ${RELOCATING+ _edata = . ; }
  } ${RELOCATING+ > data}
  
  .bss ${RELOCATING+ SIZEOF(.data) + ADDR(.data)} :
  {
    ${RELOCATING+ PROVIDE (__bss_start = .) ; }
    *(.bss)
    *(COMMON)
    ${RELOCATING+ PROVIDE (__bss_end = .) ; }
    ${RELOCATING+ _end = . ;  }
  } ${RELOCATING+ > data}

  .noinit ${RELOCATING+ SIZEOF(.bss) + ADDR(.bss)} :
  {
    ${RELOCATING+ PROVIDE (__noinit_start = .) ; }
    *(.noinit)
    *(COMMON)
    ${RELOCATING+ PROVIDE (__noinit_end = .) ; }
    ${RELOCATING+ _end = . ;  }
  } ${RELOCATING+ > data}

  .vectors :
  {
    ${RELOCATING+ PROVIDE (__vectors_start = .) ; }
    *(.vectors*)
    ${RELOCATING+ _vectors_end = . ; }
  } ${RELOCATING+ > vectors}

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

  PROVIDE (__stack = ${STACK}) ;
  PROVIDE (__data_start_rom = _etext) ;
  PROVIDE (__data_end_rom   = _etext + SIZEOF (.data)) ;
  PROVIDE (__noinit_start_rom = _etext + SIZEOF (.data)) ;
  PROVIDE (__noinit_end_rom = _etext + SIZEOF (.data) + SIZEOF (.noinit)) ;
}
EOF
