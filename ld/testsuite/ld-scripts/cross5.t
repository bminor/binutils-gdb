NOCROSSREFS_TO(.nocrossrefs .data)

SECTIONS
{
  .text : { *(.text) }
  .nocrossrefs : { *(.nocrossrefs) }
  .data : { *(.data) *(.opd) }
  .bss : { *(.bss) *(COMMON) }
  /DISCARD/ : { *(*) }
}
