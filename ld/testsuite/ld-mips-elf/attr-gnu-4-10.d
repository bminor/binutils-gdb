#source: attr-gnu-4-1.s -EB -32
#source: attr-gnu-4-0.s -EB -32
#ld: -r -melf32btsmip
#readelf: -hA

ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00.*
  Class:                             ELF32
  Data:                              2's complement, big endian
  Version:                           1 \(current\)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              REL \(Relocatable file\)
  Machine:                           MIPS R3000
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 \(bytes into file\)
  Start of section headers:          ... \(bytes into file\)
  Flags:                             .*
  Size of this header:               52 \(bytes\)
  Size of program headers:           0 \(bytes\)
  Number of program headers:         0
  Size of section headers:           40 \(bytes\)
  Number of section headers:         11
  Section header string table index: 8

Attribute Section: gnu
File Attributes
  Tag_GNU_MIPS_ABI_FP: Hard float \(double precision\)

MIPS ABI Flags Version: 0

ISA: MIPS.*
GPR size: .*
CPR1 size: .*
CPR2 size: 0
FP ABI: Hard float \(double precision\)
ISA Extension: None
ASEs:
	None
FLAGS 1: 0000000.
FLAGS 2: 00000000
