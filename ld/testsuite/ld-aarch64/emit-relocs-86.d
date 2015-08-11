#source: emit-relocs-86.s
#as: -mabi=ilp32
#ld: -m aarch64linux32 -e0 --emit-relocs
#objdump: -dr
#...
00400094 <.text>:
  400094:	11001134 	add	w20, w9, #0x4
			400094: R_AARCH64_P32_TLSLD_ADD_DTPREL_LO12	v2
