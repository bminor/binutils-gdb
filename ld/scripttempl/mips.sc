# Linker script for MIPS systems.
# Ian Lance Taylor <ian@cygnus.com>.
# These variables may be overridden by the emulation file.  The
# defaults are appropriate for a DECstation running Ultrix.
test -z "$ENTRY" && ENTRY=__start
test -z "$TEXT_START_ADDR" && TEXT_START_ADDR="0x400000 + SIZEOF_HEADERS"
test -z "$DATA_ADDR" && DATA_ADDR=0x10000000
test -z "$BSS_VAR" && BSS_VAR=
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

ENTRY(${ENTRY})

SECTIONS
{
  .text ${RELOCATING+ ${TEXT_START_ADDR}} : {
    *(.init)
    ${RELOCATING+ eprol  =  .};
    *(.text)
    *(.fini)
    ${RELOCATING+ etext  =  .};
    ${RELOCATING+ _etext  =  .};
  }
  .rdata ${RELOCATING+ ${DATA_ADDR}} : {
    *(.rdata)
  }
  .data ${RELOCATING+ .} : {
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
  }
  ${RELOCATING+ _gp = . + 0x8000;}
  .lit8 ${RELOCATING+ .} : {
    *(.lit8)
  }
  .lit4 ${RELOCATING+ .} : {
    *(.lit4)
  }
  .sdata ${RELOCATING+ .} : {
    *(.sdata)
  }
  ${RELOCATING+ edata  =  .;}
  ${RELOCATING+ _edata  =  .;}
  ${RELOCATING+ ${BSS_VAR}}
  .sbss ${RELOCATING+ .} : {
    *(.sbss)
    *(.scommon)
  }
  .bss ${RELOCATING+ .} : {
    *(.bss)
    *(COMMON)
  }
  ${RELOCATING+ end = .;}
  ${RELOCATING+ _end = .;}
}
EOF
