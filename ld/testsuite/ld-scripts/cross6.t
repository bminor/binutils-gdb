NOCROSSREFS_TO(.text .data)

SECTIONS
{
  .text : { *(.text) *(.text.*) }
  .data : { *(.data) *(.data.*) *(.sdata) *(.opd) }
  .bss : { *(.bss) *(COMMON) }
  /DISCARD/ : { *(*) }
}
