#
# Unusual variables checked by this code:
#	NOP - two byte opcode for no-op (defaults to 0)
#	DATA_ADDR - if end-of-text-plus-one-page isn't right for data start
#	OTHER_READONLY_SECTIONS - other than .text .init .ctors .rodata ...
#		(e.g., .PARISC.milli)
#	OTHER_READWRITE_SECTIONS - other than .data .bss .sdata ...
#		(e.g., .PARISC.global)
#	EXECUTABLE_SYMBOLS - symbols that must be defined for an
#		executable (e.g., _DYNAMIC_LINK)
#
# When adding sections, do note that the names of some sections are used
# when specifying the start address of the next.
#
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})

${RELOCATING+${LIB_SEARCH_DIRS}}
${RELOCATING+/* Do we need any of these for elf?
   __DYNAMIC = 0; ${STACKZERO+${STACKZERO}} ${SHLIB_PATH+${SHLIB_PATH}}  */}
${RELOCATING+${EXECUTABLE_SYMBOLS}}
${RELOCATING- /* For some reason, the Solaris linker makes bad executables
  if gld -r is used and the intermediate file has sections starting
  at non-zero addresses.  Could be a Solaris ld bug, could be a GNU ld
  bug.  But for now assigning the zero vmas works.  */}
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  .text ${RELOCATING+${TEXT_START_ADDR}} ${RELOCATING-0} :
  {
    *(.text)
    CREATE_OBJECT_SYMBOLS
    ${RELOCATING+_etext = .;}
  }
  .init    ${RELOCATING+.} ${RELOCATING-0} : { *(.init)    } =${NOP-0}
  .fini    ${RELOCATING+.} ${RELOCATING-0} : { *(.fini)    } =${NOP-0}
  .ctors   ${RELOCATING+.} ${RELOCATING-0} : { *(.ctors)   }
  .dtors   ${RELOCATING+.} ${RELOCATING-0} : { *(.dtors)   }
  .rodata  ${RELOCATING+.} ${RELOCATING-0} : { *(.rodata)  }
  .rodata1 ${RELOCATING+.} ${RELOCATING-0} : { *(.rodata1) }
  ${RELOCATING+${OTHER_READONLY_SECTIONS}}
  /* also: .hash .dynsym .dynstr .plt(if r/o) .rel.got */

  /* Read-write section, merged into data segment: */
  .data  ${RELOCATING+
	    ${DATA_ADDR- ADDR(.rodata1)+SIZEOF(.rodata1)+${MAXPAGESIZE}}
	  }
	 ${RELOCATING-0} :
  {
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
    ${RELOCATING+_edata  =  .;}
  }
  .data1 ${RELOCATING+.} ${RELOCATING-0} : { *(.data1) }
  ${RELOCATING+${OTHER_READWRITE_SECTIONS}}
  /* also (before uninitialized portion): .dynamic .got .plt(if r/w)
     (or does .dynamic go into its own segment?) */
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata   ${RELOCATING+.} ${RELOCATING-0} : { *(.sdata) }
  ${RELOCATING+__bss_start = .};
  .sbss    ${RELOCATING+.} ${RELOCATING-0} : { *(.sbss) *(.scommon) }
  .bss     ${RELOCATING+.} ${RELOCATING-0} :
  {
   *(.bss)
   *(COMMON)
   ${RELOCATING+_end = . };
   ${RELOCATING+end = . };
  }

  /* Debug sections.  These should never be loadable, but they must have
     zero addresses for the debuggers to work correctly.  */
  .line			0 : { *(.line)			}
  .debug		0 : { *(.debug)			}
  .debug_sfnames	0 : { *(.debug_sfnames)		}
  .debug_srcinfo	0 : { *(.debug_srcinfo)		}
  .debug_macinfo	0 : { *(.debug_macinfo)		}
  .debug_pubnames	0 : { *(.debug_pubnames)	}
  .debug_aranges	0 : { *(.debug_aranges)		}
}
EOF
