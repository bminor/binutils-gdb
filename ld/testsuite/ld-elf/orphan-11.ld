SECTIONS
{
  . = SIZEOF_HEADERS;
  .text : { *(.text .text.*) }
  .data : { *(.data .data.*) }
  .bss : { *(.bss .bss.*) *(COMMON) }
  .sbss : { *(.sbss .sbss.*) }
  .note : { *(.note .note.*) }
  .rela : { *(.rela .rela.*) }
  .rel : { *(.rel .rel.*) }

  /DISCARD/ : {
            *(.reginfo) *(.MIPS.abiflags) *(.trampolines) *(.iplt*)
            *(.note*) *(.got*) *(.igot*) *(.*.attributes) *(.*.info)
            *(.pdr) "linker stubs*"(*)  }
}
