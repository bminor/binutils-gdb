SECTIONS {
   . = SIZEOF_HEADERS;
   .text_low : { *(.text.1) *(.text.2) }
   .text_high 0xf000000 : { *(.text.3) }
}
