
# CYGNUS LOCAL entire file

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
EMBEDDED=yes

# There is a problem with the NOP value - it must work for both
# big endian and little endian systems.  Unfortunately there is
# no symmetrical mcore opcode that functions as a noop.  The
# chosen solution is to use "tst r0, r14".  This is a symetrical
# value, and apart from the corruption of the C bit, it has no other
# side effects.  Since the carry bit is never tested without being
# explicitly set first, and since the NOP code is only used as a
# fill value between independantly viable peices of code, it should
# not matter.
NOP=0x0e0e

OTHER_BSS_SYMBOLS="__bss_start__ = . ;"
OTHER_BSS_END_SYMBOLS="__bss_end__ = . ;"

# Hmmm, there's got to be a better way.  This sets the stack to the
# top of the simulator memory (2^19 bytes).
OTHER_RELOCATING_SECTIONS='.stack 0x80000 : { _stack = .; *(.stack) }'
