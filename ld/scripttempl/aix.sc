# AIX linker script.
# AIX always uses shared libraries.  The section VMA appears to be
# unimportant.  The native linker aligns the sections on boundaries
# specified by the -H option.
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})
${RELOCATING+${LIB_SEARCH_DIRS}}
ENTRY(__start)
SECTIONS
{
  .pad 0 : { *(.pad) }
  ${RELOCATING+PROVIDE (_text = .);}
  .text ${RELOCATING-0} : {
    *(.text)
    *(.pr)
    *(.ro)
    *(.db)
    *(.gl)
    *(.xo)
    *(.ti)
    *(.tb)
  }
  ${RELOCATING+PROVIDE (_etext = .);}
  ${RELOCATING+PROVIDE (_data = .);}
  .data ${RELOCATING-0} : {
    *(.data)
    *(.rw)
    *(.sv)
    *(.ua)
    *(.ds)
    *(.tc0)
    *(.tc)
    *(.td)
  }
  ${RELOCATING+PROVIDE (_edata = .);}
  .bss : {
    *(.bss)
    *(.bs)
    *(.uc)
    *(COMMON)
  }
  ${RELOCATING+PROVIDE (_end = .);}
  ${RELOCATING+PROVIDE (end = .);}
  .loader 0 : {
    *(.loader)
  }
  .debug 0 : {
    *(.debug)
  }
}
EOF
