ENTRY(main)

SECTIONS
{
  . = 0x10000 + SIZEOF_HEADERS;
  .text : { *(.text) }
  .data : { *(.data) }
}
