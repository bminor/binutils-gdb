NOCROSSREFS ( .text .data )
SECTIONS
{
  .toc  : { *(.toc) }
  .text : { tmpdir/cross1.o }
  .data : { tmpdir/cross2.o }
}
