SCRIPT_NAME=i386coff
OUTPUT_FORMAT="coff-i386-lynx"
# This is what their /lib/init1.o seems to want.
ENTRY=_main
# following are dubious
PAGE_SIZE=0x1000
TEXT_START_ADDR=0
NONPAGED_TEXT_START_ADDR=0x1000
ARCH=i386
