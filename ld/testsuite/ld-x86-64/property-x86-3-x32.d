#source: property-x86-3.s
#as: --x32
#ld: -shared -m elf32_x86_64
#readelf: -n

Displaying notes found in: .note.gnu.property
  Owner                 Data size	Description
  GNU                  0x00000018	NT_GNU_PROPERTY_TYPE_0
      Properties: x86 ISA needed: CMOV, SSE, SSSE3, SSE4_1
	x86 ISA used: SSE, SSE3, SSE4_1, AVX
