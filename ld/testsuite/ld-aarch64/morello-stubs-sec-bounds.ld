SECTIONS {
  . = SIZEOF_HEADERS;
  .text_low : {
    *(.text.1)
    text_sym = .;
  }
  .relocs_mid : { *(.rela.dyn) }
  .text_high 0x200000 : {
    *(.text.3)
  }
  .got : { *(.got) }
  .got.plt : { *(.got.plt) }
  .tail_relro : { *(.data.rel.ro.local) }
}
