NOCROSSREFS_TO(.data .nocrossrefs)

SECTIONS
{
  .text : { *(.text) }
  .nocrossrefs : { *(.nocrossrefs) }
  .data : { *(.data) *(.opd) }
  .bss : { *(.bss) *(COMMON) }
  /DISCARD/ : { *(*) }
}
