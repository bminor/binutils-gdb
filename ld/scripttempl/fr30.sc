cat << EOF
OUTPUT_FORMAT("elf32-fr30", "elf32-fr30",
	      "elf32-fr30")
OUTPUT_ARCH(fr30)
ENTRY(_start)
SEARCH_DIR(.);
SECTIONS
{
  /* Read-only sections, merged into text segment: */
  . = ${TEXT_START_ADDR};
  .interp	: { *(.interp) }
  .hash		: { *(.hash) }
  .dynsym	: { *(.dynsym) }
  .dynstr	: { *(.dynstr) }
  .rel.text	: { *(.rel.text) }
  .rela.text	: { *(.rela.text) }
  .rel.data	: { *(.rel.data) }
  .rela.data	: { *(.rela.data) }
  .rel.rodata	: { *(.rel.rodata) }
  .rela.rodata	: { *(.rela.rodata) }
  .rel.got	: { *(.rel.got) }
  .rela.got	: { *(.rela.got) }
  .rel.ctors	: { *(.rel.ctors) }
  .rela.ctors	: { *(.rela.ctors) }
  .rel.dtors	: { *(.rel.dtors) }
  .rela.dtors	: { *(.rela.dtors) }
  .rel.init	: { *(.rel.init) }
  .rela.init	: { *(.rela.init) }
  .rel.fini	: { *(.rel.fini) }
  .rela.fini	: { *(.rela.fini) }
  .rel.bss	: { *(.rel.bss) }
  .rela.bss	: { *(.rela.bss) }
  .rel.plt	: { *(.rel.plt) }
  .rela.plt	: { *(.rela.plt) }
  .init		: { KEEP (*(.init)) } =0
  .plt		: { *(.plt) }

  .text		: {
    *(.text)
    ${RELOCATING+*(.text.*)}
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
    *(.gnu.linkonce.t*)
  } =0

  ${RELOCATING+_etext = .;}
  ${RELOCATING+PROVIDE (etext = .);}

  .fini		: { KEEP (*(.fini))    } =0
  .rodata	: { *(.rodata) ${RELOCATING+*(.rodata.*)} *(.gnu.linkonce.r*) }
  .rodata1	: { *(.rodata1) }

  .data		: {
    *(.data)
    ${RELOCATING+*(.data.*)}
    *(.gnu.linkonce.d*)
    CONSTRUCTORS
  }
  .data1	: { *(.data1) }
  .ctors	: {
    ${RELOCATING+___ctors = .;}
    KEEP (*(.ctors))
    ${RELOCATING+___ctors_end = .;}
  }

  .dtors	: {
    ${RELOCATING+___dtors = .;}
    KEEP (*(.dtors))
    ${RELOCATING+___dtors_end = .;}
  }

  .got		: { *(.got.plt) *(.got) }
  .dynamic	: { *(.dynamic) }

  ${RELOCATING+_edata  = . ;}
  ${RELOCATING+PROVIDE (edata = _edata);}

  .bss       :
  {
	${RELOCATING+__bss_start = . ;}
	*(.dynbss)
	*(.bss)
	*(COMMON)
  }

  ${RELOCATING+_end = . ;}
  ${RELOCATING+PROVIDE (end = .);}

  /* Stabs debugging sections.  */
  .stab 0		: { *(.stab) }
  .stabstr 0		: { *(.stabstr) }
  .stab.excl 0		: { *(.stab.excl) }
  .stab.exclstr 0	: { *(.stab.exclstr) }
  .stab.index 0		: { *(.stab.index) }
  .stab.indexstr 0	: { *(.stab.indexstr) }
  .comment 0		: { *(.comment) }

  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */

  /* DWARF 1 */
  .debug          0	: { *(.debug) }
  .line           0	: { *(.line) }

  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0	: { *(.debug_srcinfo) }
  .debug_sfnames  0	: { *(.debug_sfnames) }

  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0	: { *(.debug_aranges) }
  .debug_pubnames 0	: { *(.debug_pubnames) }

  /* DWARF 2 */
  .debug_info     0	: { *(.debug_info) }
  .debug_abbrev   0	: { *(.debug_abbrev) }
  .debug_line     0	: { *(.debug_line) }
  .debug_frame    0	: { *(.debug_frame) }
  .debug_str      0	: { *(.debug_str) }
  .debug_loc      0	: { *(.debug_loc) }
  .debug_macinfo  0	: { *(.debug_macinfo) }

  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0	: { *(.debug_weaknames) }
  .debug_funcnames 0	: { *(.debug_funcnames) }
  .debug_typenames 0	: { *(.debug_typenames) }
  .debug_varnames  0	: { *(.debug_varnames) }

  /* User stack */
  .stack 0x200000	: {
	${RELOCATING+__stack = .;}
	*(.stack)
  }
  /* These must appear regardless of  .  */
}
EOF
