MACHINE=
SCRIPT_NAME=elf
OUTPUT_FORMAT="elf32-v850"
TEXT_START_ADDR=0x0
ARCH=v850
MAXPAGESIZE=256
ENTRY=_start

# These are for compatibility with the COFF toolchain.
CTOR_START='___ctors = .;'
CTOR_END='___ctors_end = .;'
DTOR_START='___dtors = .;'
DTOR_END='___dtors_end = .;'

