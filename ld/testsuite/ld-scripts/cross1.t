NOCROSSREFS ( .text .data )
SECTIONS
{
  .toc  : { *(.toc) }
  .opd  : { *(.opd) }
  .text : { tmpdir/cross1.o }
  .data : { tmpdir/cross2.o }
}
