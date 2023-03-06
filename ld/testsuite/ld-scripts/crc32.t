MEMORY {
  rom : ORIGIN = 0x000000, LENGTH = 0x4000
  ram : ORIGIN = 0x008000, LENGTH = 0x1000
}

_start = 0x000000;
SECTIONS
{
  . = 0x1000 + SIZEOF_HEADERS;
  .text ALIGN (0x200) :

    {
      FILL(0xFF)
      header = .;
      INCLUDE "begin_tag.inc"

      expected = .;
      BYTE(0xCB);
      BYTE(0xF4);
      BYTE(0x39);
      BYTE(0x26);
      LONG(0x0);
      QUAD(0x0);

      crc32 = .;
      DIGEST "_CRC32#BE" "CRC32" (ecc_start , ecc_end);
      LONG(0);
      QUAD(0);

      INCLUDE "end_tag.inc"

      INCLUDE "crc_data.inc"

      INCLUDE "digest_table.inc"
    } > rom

  .data : AT (0x008000) { *(.data) } >ram /* NO default AT>rom */
  . = ALIGN(0x20);
  .bss : { *(.bss) } >ram /* NO default AT>rom */
  /DISCARD/ : { *(*) }
}
