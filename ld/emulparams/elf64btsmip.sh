# If you change this file, please also look at files which source this one:
# elf64ltsmip.sh

. ${srcdir}/emulparams/elf64bmip-defs.sh
OUTPUT_FORMAT="elf64-tradbigmips"
BIG_OUTPUT_FORMAT="elf64-tradbigmips"
LITTLE_OUTPUT_FORMAT="elf64-tradlittlemips"

DATA_ADDR=0x0400000000
NONPAGED_TEXT_START_ADDR=0x10000000
SHLIB_TEXT_START_ADDR=0
TEXT_DYNAMIC=

# Magic sections.
INITIAL_READONLY_SECTIONS='.MIPS.options : { *(.MIPS.options) }'
OTHER_TEXT_SECTIONS='*(.mips16.fn.*) *(.mips16.call.*)'
OTHER_SECTIONS='
  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }
'
