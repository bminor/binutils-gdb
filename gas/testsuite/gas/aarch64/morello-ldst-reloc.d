#as: -march=morello+c64
#objdump: -dr

.*:     file format .*


Disassembly of section \.text:

.* <_start>:
.*:	c2400042 	ldr	c2, \[c2\]
			.*: R_AARCH64_LDST128_ABS_LO12_NC	\.data\+0x10
.*:	82600042 	ldr	c2, \[x2\]
			.*: R_AARCH64_LDST128_ABS_LO12_NC	\.data\+0x10
.*:	82600c42 	ldr	x2, \[x2\]
			.*: R_AARCH64_LDST64_ABS_LO12_NC	\.data\+0x10
.*:	82600842 	ldr	w2, \[x2\]
			.*: R_AARCH64_LDST32_ABS_LO12_NC	\.data\+0x10

.* <add>:
.*:	02000000 	add	c0, c0, #0x0
			.*: R_AARCH64_ADD_ABS_LO12_NC	ptr

.* <f1>:
  .*:	90800002 	adrp	c2, 10 <add>
			.*: R_MORELLO_ADR_GOT_PAGE	cap
  .*:	c2400042 	ldr	c2, \[c2\]
			.*: R_MORELLO_LD128_GOT_LO12_NC	cap
  .*:	82600042 	ldr	c2, \[x2\]
			.*: R_MORELLO_LD128_GOT_LO12_NC	ptr
  .*:	f9400042 	ldr	x2, \[c2\]
			.*: R_AARCH64_LD64_GOT_LO12_NC	ptr
  .*:	82600c42 	ldr	x2, \[x2\]
			.*: R_AARCH64_LD64_GOT_LO12_NC	ptr
