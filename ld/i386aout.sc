OUTPUT_FORMAT("a.out-i386")
OUTPUT_ARCH(i386)

__DYNAMIC  =  0;
SECTIONS
{
  .text 0 BLOCK(0x1000) :
  {
    CREATE_OBJECT_SYMBOLS
    *(.text)
    _etext  =  ALIGN(0x1000);
    }
  .data ALIGN(0x1000) :
  {
    *(.data)
    CONSTRUCTORS
    _edata  =  .;
  }
  .bss SIZEOF(.data) + ADDR(.data) :
  {
    *(.bss)
    [COMMON]
    _end = .;
  }
}
