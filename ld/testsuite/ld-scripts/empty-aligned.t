SECTIONS
{
  .text : { *(.text) }
  .text2 :
  {
    . = ALIGN(4096);
    *(.text2)
  }
  .text3 :
  {
    *(.text3)
    . = ALIGN(4096);
  }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
