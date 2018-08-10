#source: pr23494b.s
#PROG: objcopy
#as: --x32
#objcopy: -O elf64-x86-64
#readelf: -n
#not-target: x86_64-*-nacl*

Displaying notes found in: .note.gnu.property
  Owner                 Data size	Description
  GNU                  0x00000010	NT_GNU_PROPERTY_TYPE_0
      Properties: x86 ISA used: 586, SSE
