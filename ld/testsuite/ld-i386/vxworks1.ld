SECTIONS
{
  . = 0x80000;
  .interp : { *(.interp) }
  .hash : { *(.hash) }
  .dynsym : { *(.dynsym) }
  .dynstr : { *(.dynstr) }

  . = ALIGN (0x100);
  .rel.dyn : { *(.rel.dyn) }
  .rel.plt : { *(.rel.plt) }

  . = ALIGN (0x100);
  .plt : { *(.plt) }

  . = ALIGN (0x100);
  .text : { *(.text) }

  . = ALIGN (0x1000);
  .dynamic : { *(.dynamic) }

  . = ALIGN (0x100);
  .got : { *(.got.plt) *(.got) }
}
