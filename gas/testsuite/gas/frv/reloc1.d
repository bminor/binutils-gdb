#objdump: -dr

.* elf32-frv

Disassembly.*:

.* <begin>:
.*:	80 88 00 00 *	nop
.*:	80 88 00 00 *	nop
.* <f1>:
.*:	80 88 00 00 *	nop
.*:	80 88 00 00 *	nop
.*:	80 88 00 00 *	nop
.* <f2>:
.*:	80 3c 00 00 *	call.*
			.*: R_FRV_LABEL24	f1
.*:	c0 1a 00 00 *	bra.*
			.*: R_FRV_LABEL16	f1
.*:	fe 3f ff fe *	call .* <f2>
.*:	c0 1a ff fd *	bra .* <f2>
	\.\.\.
