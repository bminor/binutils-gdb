NOCROSSREFS_TO(.data .text)

SECTIONS
{
  .text : { *(.text) }
  .data : { *(.data) *(.opd) }
  .bss : { *(.bss) *(COMMON) }
  /DISCARD/ : { *(*) }
}
