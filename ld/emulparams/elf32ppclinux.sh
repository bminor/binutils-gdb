. ${srcdir}/emulparams/elf32ppc.sh
TEXT_START_ADDR=0x10000000
unset EXECUTABLE_SYMBOLS
unset OTHER_BSS_END_SYMBOLS
test -z "${RELOCATING}" || OTHER_SECTIONS="/DISCARD/	: { *(.fixup) }"
OTHER_READWRITE_SECTIONS="
  .got1         ${RELOCATING-0} : { *(.got1) }
  .got2         ${RELOCATING-0} : { *(.got2) }"
OTHER_GOT_RELOC_SECTIONS="
  .rela.got1         ${RELOCATING-0} : { *(.rela.got1) }
  .rela.got2         ${RELOCATING-0} : { *(.rela.got2) }"
