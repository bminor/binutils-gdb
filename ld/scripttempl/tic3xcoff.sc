# 32 interrupt vectors + 32 trap vectors each of 4 bytes
# The .bss and .data sections need to be contiguous for direct addressing
# The data page pointer gets loaded with the start of .bss
# TI C compiler uses .cinit to initialise variables in .bss

test -z "$ENTRY" && ENTRY=_start
# These are substituted in as variables in order to get '}' in a shell
# conditional expansion.
INIT='.init : { *(.init) }'
FINI='.fini : { *(.fini) }'
cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH("${OUTPUT_ARCH}")
${LIB_SEARCH_DIRS}

ENTRY(${ENTRY})
${RELOCATING+ __SYSMEM_SIZE = DEFINED(__SYSMEM_SIZE) ? __SYSMEM_SIZE : 0x4000;}
${RELOCATING+ __STACK_SIZE  = DEFINED(__STACK_SIZE)  ? __STACK_SIZE  : 0x1000;}

SECTIONS
{
  .comms ${RELOCATING+ 64} : {
    *(.comms)
  } 
  .bss ${RELOCATING+ SIZEOF(.comms) + ADDR(.comms)} : {
    ${RELOCATING+ .bss  =  .;}	
    *(.bss)
    *(COMMON)
    ${RELOCATING+  end  =  .;}
    ${RELOCATING+  _end  =  end;}
  }
  .data ${RELOCATING+ SIZEOF(.bss) + ADDR(.bss)} :
  { 				
    ${RELOCATING+  .data  = .;}
    *(.data)
    ${RELOCATING+  edata  =  .;}
  }
  .const ${RELOCATING+ SIZEOF(.data) + ADDR(.data)} :
  { 					
    *(.const)
  }
  .cinit ${RELOCATING+ SIZEOF(.const) + ADDR(.const)} :
  { 					
    ${RELOCATING+  cinit = .;}
    *(.cinit)
    LONG(0);
  }
  .text ${RELOCATING+ SIZEOF(.cinit) + ADDR(.cinit)} : {
    ${RELOCATING+  .text =  .;}
    ${RELOCATING+ *(.init)}
    *(.text)
    ${CONSTRUCTING+ ___CTOR_LIST__ = .;}
    ${CONSTRUCTING+ LONG(___CTOR_END__ - ___CTOR_LIST__ - 2)}
    ${CONSTRUCTING+ *(.ctors)}
    ${CONSTRUCTING+ LONG(0);}
    ${CONSTRUCTING+ ___CTOR_END__  = .;}
    ${CONSTRUCTING+ ___DTOR_LIST__ = .;}
    ${CONSTRUCTING+ LONG(___DTOR_END__ - ___DTOR_LIST__ - 2)}
    ${CONSTRUCTING+ *(.dtors)}
    ${CONSTRUCTING+ LONG(0)}
    ${CONSTRUCTING+ ___DTOR_END__  = .;}
    ${RELOCATING+ *(.fini)}
    ${RELOCATING+  etext =  .;}
    ${RELOCATING+  _etext =  etext;}
  }
  .stack ${RELOCATING+ SIZEOF(.text) + ADDR(.text)} :
  { 					
    *(.stack)
    ${RELOCATING+ .  =  . + __STACK_SIZE};		
  }
  .sysmem ${RELOCATING+ SIZEOF(.stack) + ADDR(.stack)} :
  { 					
    *(.sysmem)
  }
  .heap ${RELOCATING+ SIZEOF(.sysmem) + ADDR(.sysmem)} :
  { 					
    ${RELOCATING+ . += __SYSMEM_SIZE - SIZEOF(.sysmem)};
  }
  ${RELOCATING- ${INIT}}
  ${RELOCATING- ${FINI}}
  .stab  0 ${RELOCATING+(NOLOAD)} : 
  {
    [ .stab ]
  }
  .stabstr  0 ${RELOCATING+(NOLOAD)} :
  {
    [ .stabstr ]
  }
/* The TI tools sets cinit to -1 if the ram model is used.  */
    ${RELOCATING+  cinit = SIZEOF(.cinit) == 1 ? cinit : -1;}
}
EOF
