SECTIONS {
   . = SIZEOF_HEADERS;
   .text_low : { *(.text.1) *(.text.2) }
   .relocs_mid : { *(.rela.dyn) }
   .text_high 0x20000 : { *(.text.3) }
   .tail_relro : { *(.data.rel.ro.local) }
}
