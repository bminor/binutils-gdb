#source: attr-gnu-4-6.s -32 -EB
#source: attr-gnu-4-0.s -32 -EB
#ld: -r -melf32btsmip
#readelf: -hA

ELF Header:
  Magic:   7f 45 4c 46 01 02 01 00 03 00 00 00 00 00 00 00.*
  Class:                             ELF32
  Data:                              2's complement, big endian
  Version:                           1 \(current\)
  OS/ABI:                            UNIX - System V
  ABI Version:                       3
  Type:                              REL \(Relocatable file\)
  Machine:                           MIPS R3000
  Version:                           0x1
  Entry point address:               0x0
  Start of program headers:          0 \(bytes into file\)
  Start of section headers:          ... \(bytes into file\)
  Flags:                             0x70001000, o32, mips32r2
  Size of this header:               52 \(bytes\)
  Size of program headers:           0 \(bytes\)
  Number of program headers:         0
  Size of section headers:           40 \(bytes\)
  Number of section headers:         11
  Section header string table index: 8
Attribute Section: gnu
File Attributes
  Tag_GNU_MIPS_ABI_FP: Hard float \(32-bit CPU, 64-bit FPU\)

MIPS ABI Flags Version: 0

ISA: MIPS.*
GPR size: .*
CPR1 size: .*
CPR2 size: 0
FP ABI: Hard float \(32-bit CPU, 64-bit FPU\)
ISA Extension: None
ASEs:
	None
FLAGS 1: 0000000.
FLAGS 2: 00000000
