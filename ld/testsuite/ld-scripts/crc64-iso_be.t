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
      BYTE(0x01);
      BYTE(0x10);
      BYTE(0xa4);
      BYTE(0x75);
      BYTE(0xc7);
      BYTE(0x56);
      BYTE(0x09);
      BYTE(0xb9);
      QUAD(0x0);

      crc64 = .;
      DIGEST "_CRC64" "CRC64-GO-ISO" (ecc_start , ecc_end)
      QUAD(0x0);

      INCLUDE "end_tag.inc"

      INCLUDE "crc_data.inc"

      INCLUDE "digest_table_be.inc"
    } > rom

  .data : AT (0x008000) { *(.data) } >ram /* NO default AT>rom */
  . = ALIGN(0x20);
  .bss : { *(.bss) } >ram /* NO default AT>rom */
  /DISCARD/ : { *(*) }
}
