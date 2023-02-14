MEMORY {
  rom : ORIGIN = 0x00000, LENGTH = 0x10000
  ram : ORIGIN = 0x10000, LENGTH = 0x10000
}

_start = 0x000000;
SECTIONS
{
  . = 0x1000 + SIZEOF_HEADERS;
  .text ALIGN (0x20) :
    {
      *(.text)
      ASCIZ "This is a string"
      . = ALIGN(0x20);
      align_label = .;
      ASCIZ "This is another \nstring\123"
      unalign_label = .;
    }
  .data : AT (0x10000) { *(.data) } >ram /* NO default AT>rom */
  . = ALIGN(0x20);
  .bss : { *(.bss) } >ram /* NO default AT>rom */
  /DISCARD/ : { *(*) }
}
