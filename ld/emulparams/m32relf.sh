MACHINE=
SCRIPT_NAME=elf
TEMPLATE_NAME=elf32
OUTPUT_FORMAT="elf32-m32r"
TEXT_START_ADDR=0x100
ARCH=m32r
MACHINE=
MAXPAGESIZE=32
EMBEDDED=yes

# For the default linker script, include libgloss.
# If the user specifies a different linker script, we want the
# rest of the files (e.g. crt0.o) included by default, but not libgloss.
# [Thus I was reluctant to handle this in gcc specs.]
INPUT_FILES="INPUT(-lgloss)"

# Hmmm, there's got to be a better way.  This sets the stack to the
# top of simulator memory (8MB).
OTHER_RELOCATING_SECTIONS='PROVIDE (_stack = 0x800000);'
