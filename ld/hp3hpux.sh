EMULATION_NAME=hp3hpux
SCRIPT_NAME=aout
OUTPUT_FORMAT="a.out-hp300hpux"
TEXT_START_ADDR=0
PAGE_SIZE=4096
ARCH=m68k
STACKZERO="___stack_zero = 0x2000;"
# This is needed for HPUX 9.0; it is unnecessary but harmless for 8.0.
SHLIB_PATH="___dld_shlib_path = 0;"
