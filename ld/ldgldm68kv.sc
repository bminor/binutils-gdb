/* $Id$
*/

OUTPUT_FORMAT("m68kvcoff")

ENTRY(__startup)
SECTIONS
{
  .text 0x001000a8 : {
    *(.init)
    *(.text)
    *(.fini)
    _etext  =  .;
  }
  .data ((ADDR(.text) + SIZEOF(.text)) % 0x2000) + ALIGN(0x00200000) : {
   	*(.data)
	_edata  =  .;
  }
  .bss   (SIZEOF(.data) + ADDR(.data)) : {
	*(.bss)
	[COMMON]
	_end = ALIGN(0x8);
  }
}
