PHDRS {
       code PT_LOAD FLAGS(5);
       data PT_LOAD FLAGS(6);
       dynamic PT_DYNAMIC FLAGS(6);
}

MEMORY {
       RAM : ORIGIN = 0x10000, LENGTH = 0x7fffff
}

SECTIONS {
       .text : { *(.text) *(.stub) *(.gnu.warning) *(.gnu.linkonce.t*) } > RAM :code
       .plt : { *(.plt) } > RAM :code
       .rodata : { *(.rodata) } > RAM :code
       .rdata : { *(.rdata) } > RAM :code
       .data ALIGN(0x1000) : { *(.data) *(.gnu.linkonce.d*) } > RAM :data
       .got : { *(.got) *(.got.plt)  } > RAM :data
       .bss  : { *(.bss) *(.dynbss) *(COMMON) } > RAM :data
       .dynamic  : { *(.dynamic ) } > RAM :dynamic  :data
       .dynsym : { *(.dynsym) } > RAM :dynamic :data
       .dynstr : { *(.dynstr) } > RAM :dynamic :data
       .rela.text : { *(.rela.text) } > RAM :dynamic :data
       .rela.plt : { *(.rela.plt) } > RAM :dynamic :data
       .rela.data : { *(.rela.data) } > RAM :dynamic :data
       .rel.text : { *(.rel.text) } > RAM :dynamic :data
       .rel.plt : { *(.rel.plt) } > RAM :dynamic :data
       .rel.data : { *(.rel.data) } > RAM :dynamic :data
       .hash : { *(.hash) } > RAM :dynamic :data
       .interp : { *(.interp) } > RAM :dynamic :data
       .gnu.version_r : { *(.gnu.version_r) } > RAM :dynamic :data
       .gnu.version_d : { *(.gnu.version_d) }> RAM :dynamic :data
       .gnu.version : { *(.gnu.version) } > RAM :dynamic :data
}
