# Linker script for Alpha systems.
# Ian Lance Taylor <ian@cygnus.com>.
# These variables may be overridden by the emulation file.  The
# defaults are appropriate for an Alpha running OSF/1.
test -z "$ENTRY" && ENTRY=__start
test -z "$TEXT_START_ADDR" && TEXT_START_ADDR="0x120000000 + SIZEOF_HEADERS"
test -z "$DATA_ADDR" && DATA_ADDR=0x140000000
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

ENTRY(${ENTRY})

SECTIONS
{
  .text ${RELOCATING+ ${TEXT_START_ADDR}} : {
    ${RELOCATING+ _FTEXT = . };
    ${RELOCATING+ __istart = . };
    ${RELOCATING+ *(.init) }
    ${RELOCATING+ LONG (0x6bfa8001)}
    ${RELOCATING+ eprol  =  .};
    *(.text)
    ${RELOCATING+ __fstart = . };
    ${RELOCATING+ *(.fini)}
    ${RELOCATING+ LONG (0x6bfa8001)}
    ${RELOCATING+ _ETEXT  =  .};
  }
  .rdata ${RELOCATING+ .} : {
    *(.rdata)
  }
  .pdata . : {
    ${RELOCATING+ _fpdata = .;}
    *(.pdata)
  }
  .data ${RELOCATING+ ${DATA_ADDR}} : {
    ${RELOCATING+ _FDATA = .;}
    *(.data)
    ${CONSTRUCTING+CONSTRUCTORS}
  }
  .xdata . : {
    *(.xdata)
  }
  ${RELOCATING+ _gp = ALIGN (8) + 0x8000;}
  .lit8 ${RELOCATING+ .} : {
    *(.lit8)
  }
  .lit4 ${RELOCATING+ .} : {
    *(.lit4)
  }
  .lita ${RELOCATING+ .} : {
    *(.lita)
  }
  .sdata ${RELOCATING+ .} : {
    *(.sdata)
  }
  ${RELOCATING+ _EDATA  =  .;}
  ${RELOCATING+ _FBSS = .;}
  .sbss ${RELOCATING+ .} : {
    *(.sbss)
    *(.scommon)
  }
  .bss ${RELOCATING+ .} : {
    *(.bss)
    *(COMMON)
  }
  ${RELOCATING+ _END = .;}
  ${RELOCATING+ _end = .;}
}
EOF
