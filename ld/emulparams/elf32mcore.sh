SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-mcore-big"
BIG_OUTPUT_FORMAT="elf32-mcore-big"
LITTLE_OUTPUT_FORMAT="elf32-mcore-little"
PAGE_SIZE=0x1000
TARGET_PAGE_SIZE=0x400
MAXPAGESIZE=0x1000
TEXT_START_ADDR=0
NONPAGED_TEXT_START_ADDR=0
ARCH=mcore
# 1211 == mov r1,r1
NOP=0x1211
EMBEDDED=yes

OTHER_BSS_SYMBOLS="__bss_start__ = . ;"
OTHER_BSS_END_SYMBOLS="__bss_end__ = . ;"

# Hmmm, there's got to be a better way.  This sets the stack to the
# top of the simulator memory (2^19 bytes).
OTHER_RELOCATING_SECTIONS='.stack 0x80000 : { _stack = .; *(.stack) }'
