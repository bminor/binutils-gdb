NOCROSSREFS ( .text .data )
SECTIONS
{
  .text : { *(.text) }
  .data : { *(.data) *(.sdata) }
}
