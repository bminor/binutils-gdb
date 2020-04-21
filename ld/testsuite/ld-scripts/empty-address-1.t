SECTIONS
{
  .text 0x0000000: { *(.text) }
  .data 0x200:
  {
    __data_start = . ;
    *(.data)
  }
  __data_end = .;
  /DISCARD/ : { *(.*) }
}
