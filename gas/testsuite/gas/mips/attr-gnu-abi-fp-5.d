#as: -32 -mips2 -mfpxx
#source: attr-gnu-abi-fp-5.s
#readelf: -A
#name: MIPS gnu_attribute Tag_GNU_MIPS_ABI_FP,5

Attribute Section: gnu
File Attributes
  Tag_GNU_MIPS_ABI_FP: Hard float \(32-bit CPU, Any FPU\)

MIPS ABI Flags Version: 0

ISA: MIPS.*
GPR size: 32
CPR1 size: 32
CPR2 size: 0
FP ABI: Hard float \(32-bit CPU, Any FPU\)
ISA Extension: .*
ASEs:
	None
FLAGS 1: 0000000.
FLAGS 2: 00000000
