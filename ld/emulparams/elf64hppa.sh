SCRIPT_NAME=elf
ELFSIZE=64
OUTPUT_FORMAT="elf64-hppa"
LIB_PATH=/usr/lib/pa20_64:/opt/langtools/lib/pa20_64
TEXT_START_ADDR=0x4000000000001000
DATA_ADDR=0x8000000000001000

# The HP dynamic linker actually requires you set the start of text and
# data to some reasonable value.  Of course nobody knows what reasoanble
# really is, so we just use the same values that HP's linker uses.
SHLIB_TEXT_START_ADDR=0x4000000000001000
SHLIB_DATA_ADDR=0x8000000000001000

TARGET_PAGE_SIZE=4096
MAXPAGESIZE=4096
ARCH=hppa
MACHINE=hppa2.0w
ENTRY="main"
TEMPLATE_NAME=elf32
GENERATE_SHLIB_SCRIPT=yes
# We really want multiple .stub sections, one for each input .text section,
# but for now this is good enough.
OTHER_READONLY_SECTIONS='.PARISC.unwind : { *(.PARISC.unwind) } .stubs : { *(.stubs) }'

# The PA64 ELF port treats .plt sections differently than most.  We also have
# to create a .opd section.  What most systems call the .got, we call the .dlt
OTHER_READWRITE_SECTIONS='.opd : { *(.opd) } __gp = .; .plt : { *(.plt) } .dlt : { *(.dlt) }'

# The PA64 ELF port has two additional bss sections. huge bss and thread bss.
# Make sure they end up in the appropriate location.  We also have to set
# __TLS_SIZE to the size of the thread bss section.
OTHER_BSS_SECTIONS='.hbss : { *(.hbss) } .tbss : { *(.tbss) } __TLS_SIZE = SIZEOF(.tbss);'

# HPs use .dlt where systems use .got.  Sigh.
OTHER_GOT_RELOC_SECTIONS='.rela.dlt : { *(.rela.dlt) }'

# This does not have to be strictly the bottom of the text segment since
# it is merely for performing relocations relative to a symbol that is not
# going to be referenced in any other manner.
#
# If it was the true base of the text segment, then we could use it for
# building unwinders instead of computing the base of the text segment
# in the BFD backend.
TEXT_START_SYMBOLS='__text_dummy = . ;'
DATA_START_SYMBOLS='__hp_load_map = .; . += 16;'

# The linker is required to define these two symbols.
EXECUTABLE_SYMBOLS='__SYSTEM_ID = 0x214; _FPU_STATUS = 0x0;'
DATA_PLT=

# .dynamic should be at the start of the .text segment.
TEXT_DYNAMIC=

# The PA64 ELF port needs two additional initializer sections and also wants
# a start/end symbol pair for the .init and .fini sections.
INIT_START='KEEP (*(.HP.init)); __preinit_start = .; KEEP (*(.preinit)); __preinit_end = .;__init_start = .;'
INIT_END='__init_end = .;'
FINI_START='__fini_start = .;'
FINI_END='__fini_end = .;'
