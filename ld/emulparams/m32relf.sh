MACHINE=
SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-m32r"
TEXT_START_ADDR=0x0
ARCH=m32r
MAXPAGESIZE=32

# Hmmm, there's got to be a better way.  This sets the stack to the
# top of the simulator memory (currently 1M).
OTHER_RELOCATING_SECTIONS='.stack 0x100000 : { _stack = .; *(.stack) }'
