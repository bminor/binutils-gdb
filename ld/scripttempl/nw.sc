#
# Unusual variables checked by this code:
#	NOP - four byte opcode for no-op (defaults to 0)
#	DATA_ADDR - if end-of-text-plus-one-page isn't right for data start
#	OTHER_READONLY_SECTIONS - other than .text .init .ctors .rodata ...
#		(e.g., .PARISC.milli)
#	OTHER_READWRITE_SECTIONS - other than .data .bss .sdata ...
#		(e.g., .PARISC.global)
#	OTHER_SECTIONS - at the end
#	EXECUTABLE_SYMBOLS - symbols that must be defined for an
#		executable (e.g., _DYNAMIC_LINK)
#	TEXT_START_SYMBOLS - symbols that appear at the start of the
#		.text section.
#	DATA_START_SYMBOLS - symbols that appear at the start of the
#		.data section.
#	OTHER_BSS_SYMBOLS - symbols that appear at the start of the
#		.bss section besides __bss_start.
#	DATA_PLT - .plt should be in data segment, not text segment.
#
# When adding sections, do note that the names of some sections are used
# when specifying the start address of the next.
#
test -z "${BIG_OUTPUT_FORMAT}" && BIG_OUTPUT_FORMAT=${OUTPUT_FORMAT}
test -z "${LITTLE_OUTPUT_FORMAT}" && LITTLE_OUTPUT_FORMAT=${OUTPUT_FORMAT}
test "$LD_FLAG" = "N" && DATA_ADDR=.
INTERP=".interp    : { *(.interp) 	}"
PLT=".plt     : { *(.plt)	}"
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}", "${BIG_OUTPUT_FORMAT}",
	      "${LITTLE_OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

${RELOCATING+${LIB_SEARCH_DIRS}}
${RELOCATING+/* Do we need any of these for elf?
   __DYNAMIC = 0; ${STACKZERO+${STACKZERO}} ${SHLIB_PATH+${SHLIB_PATH}}  */}
${RELOCATING+${EXECUTABLE_SYMBOLS}}

SECTIONS
{
  /* Read-only sections, merged into text segment: */
  ${CREATE_SHLIB-${RELOCATING+. = ${TEXT_START_ADDR} + SIZEOF_HEADERS;}}
  ${CREATE_SHLIB+${RELOCATING+. = SIZEOF_HEADERS;}}
  ${CREATE_SHLIB-${INTERP}}
  .hash         : { *(.hash)		}
  .dynsym       : { *(.dynsym)		}
  .dynstr       : { *(.dynstr)		}
  .rel.text     : { *(.rel.text)		}
  .rela.text    : { *(.rela.text) 	}
  .rel.data     : { *(.rel.data)		}
  .rela.data    : { *(.rela.data) 	}
  .rel.rodata   : { *(.rel.rodata) 	}
  .rela.rodata  : { *(.rela.rodata) 	}
  .rel.got      : { *(.rel.got)		}
  .rela.got     : { *(.rela.got)		}
  .rel.ctors    : { *(.rel.ctors)	}
  .rela.ctors   : { *(.rela.ctors)	}
  .rel.dtors    : { *(.rel.dtors)	}
  .rela.dtors   : { *(.rela.dtors)	}
  .rel.bss      : { *(.rel.bss)		}
  .rela.bss     : { *(.rela.bss)		}
  .rel.plt      : { *(.rel.plt)		}
  .rela.plt     : { *(.rela.plt)		}
  .init         : { *(.init)	} =${NOP-0}
  ${DATA_PLT-${PLT}}
  .text     :
  {
    ${RELOCATING+${TEXT_START_SYMBOLS}}
    *(.text)
    ${CONSTRUCTING+	__CTOR_LIST__ = .;}
    ${CONSTRUCTING+	LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)}
    ${CONSTRUCTING+	*(.ctors)}
    ${CONSTRUCTING+	LONG(0)}
    ${CONSTRUCTING+	__CTOR_END__ = .;}
    ${CONSTRUCTING+	__DTOR_LIST__ = .;}
    ${CONSTRUCTING+	LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)}
    ${CONSTRUCTING+	*(.dtors)}
    ${CONSTRUCTING+	LONG(0)}
    ${CONSTRUCTING+	__DTOR_END__ = .;}
  }
  ${RELOCATING+_etext = .;}
  ${RELOCATING+PROVIDE (etext = .);}
  .fini     : { *(.fini)    } =${NOP-0}
  .ctors    : { *(.ctors)   }
  .dtors    : { *(.dtors)   }
  .rodata   : { *(.rodata)  }
  .rodata1  : { *(.rodata1) }
  ${OTHER_READONLY_SECTIONS}

  /* Read-write section, merged into data segment: */
  ${RELOCATING+. = ${DATA_ADDR- ALIGN(8) + ${MAXPAGESIZE}};}
  .data   :
  {
    ${RELOCATING+${DATA_START_SYMBOLS}}
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
  }
  .data1  : { *(.data1) }
  ${OTHER_READWRITE_SECTIONS}
  .got          : { *(.got.plt) *(.got) }
  .dynamic      : { *(.dynamic) }
  ${DATA_PLT+${PLT}}
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata    : { *(.sdata) }
  ${RELOCATING+_edata  =  .;}
  ${RELOCATING+PROVIDE (edata = .);}
  ${RELOCATING+__bss_start = .;}
  ${RELOCATING+${OTHER_BSS_SYMBOLS}}
  .sbss     : { *(.sbss) *(.scommon) }
  .bss      :
  {
   *(.dynbss)
   *(.bss)
   *(COMMON)
  }
  ${RELOCATING+_end = . ;}
  ${RELOCATING+PROVIDE (end = .);}

  /* These are needed for ELF backends which have not yet been
     converted to the new style linker.  */
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }

  ${OTHER_SECTIONS}
}
EOF
