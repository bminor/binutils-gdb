. ${srcdir}/emulparams/armelf.sh
SCRIPT_NAME="armbpabi"
GENERATE_COMBRELOC_SCRIPT=1
OUTPUT_FORMAT="elf32-littlearm-symbian"
BIG_OUTPUT_FORMAT="elf32-bigarm-symbian"
LITTLE_OUTPUT_FORMAT="$OUTPUT_FORMAT"
TARGET1_IS_REL=1
TARGET2_TYPE=abs
# On BPABI systems, program headers should not be mapped.
EMBEDDED=yes

# This value should match ELF_MAXPAGESIZE in BFD.  Otherwise, elf.c
# will not place read-write sections in a separate ELF segment from
# the read-only sections.
MAXPAGESIZE=0x8000
