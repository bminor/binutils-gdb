SECTIONS
{
  .text 0x100 : { 
    text_start = .;
    *(.text)
    text_end = .;
  }
  . = 0x1000;
  .data : {
    data_start = .;
    *(.data)
    data_end = .;
  }
}
