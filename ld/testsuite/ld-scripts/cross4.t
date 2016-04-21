NOCROSSREFS_TO(.data .nocrossrefs)

SECTIONS
{
  .text : { *(.text) *(.text.*) }
  .nocrossrefs : { *(.nocrossrefs) }
  .data : { *(.data) *(.data.*) *(.sdata) *(.opd) }
  .bss : { *(.bss) *(COMMON) }
  /DISCARD/ : { *(*) }
}
