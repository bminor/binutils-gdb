# Linker Script for National Semiconductor's CRX-ELF32.

# The next line should be uncommented if it is desired to link
# without libstart.o and directly enter main.

# ENTRY=_main

test -z "$ENTRY" && ENTRY=_start
cat <<EOF

/* Example Linker Script for linking NS CRX elf32 files. */

/* The next line forces the entry point (${ENTRY} in this script)
   to be entered in the output file as an undefined symbol.
   It is needed in case the entry point is not called explicitly
   (which is the usual case) AND is in an archive.  */

OUTPUT_FORMAT("${OUTPUT_FORMAT}")
OUTPUT_ARCH(${ARCH})
EXTERN(${ENTRY})
ENTRY(${ENTRY})

/* Define memory regions.  */
MEMORY
{
        rom         : ORIGIN = 0x2,         LENGTH = 3M
        ram         : ORIGIN = 4M,          LENGTH = 10M
}

SECTIONS
{
  .text : { __TEXT_START = .; *(.text) *(.text.*) *(.gnu.linkonce.t.*) __TEXT_END = .; } > rom
	
  .rdata : { __RDATA_START = .; *(.rdata_4) *(.rdata_2) *(.rdata_1) *(.rdata.*) *(.gnu.linkonce.r.*) __RDATA_END = .; } > rom

  .ctor ALIGN(4) : { __CTOR_LIST = .; *(.ctors) __CTOR_END = .; } > rom

  .dtor ALIGN(4) : { __DTOR_LIST = .; *(.dtors) __DTOR_END = .; } > rom

  .data : { __DATA_START = .; *(.data_4) *(.data_2) *(.data_1) *(.data) *(.data.*) *(.gnu.linkonce.d.*) __DATA_END = .; } > ram AT > rom

  .bss (NOLOAD) : { __BSS_START = .; *(.bss_4) *(.bss_2) *(.bss_1) *(.bss) *(COMMON) *(.bss.*) *(.gnu.linkonce.b.*) __BSS_END = .; } > ram

/* You may change the sizes of the following sections to fit the actual
   size your program requires.

   The heap and stack are aligned to the bus width, as a speed optimization
   for accessing data located there.  */

  .heap : { . = ALIGN(4); __HEAP_START = .; . += 0x2000; __HEAP_MAX = .; } > ram
  .stack : { . = ALIGN(4); . += 0x6000; __STACK_START = .; } > ram
  .istack : { . = ALIGN(4); . += 0x100; __ISTACK_START = .; } > ram
}

__DATA_IMAGE_START = LOADADDR(.data);
EOF
