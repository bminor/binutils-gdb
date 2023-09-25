SECTIONS
{
 .text 0x00 : { *(.text) }
 .text2 0x100000000 : { *(.foo.text) }
}
