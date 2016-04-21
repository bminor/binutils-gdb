NOCROSSREFS_TO(.data .text)

SECTIONS
{
  .text : { *(.text) *(.text.*) }
  .data : { *(.data) *(.data.*) *(.sdata) *(.opd) }
  .bss : { *(.bss) *(COMMON) }
  /DISCARD/ : { *(*) }
}
