tmpdir/stub:     file format elf32-metag
architecture: metag, flags 0x00000112:
EXEC_P, HAS_SYMS, D_PAGED
start address 0x.*

Disassembly of section .text:
.* <__start-0x8>:
.*:	05 81 18 82 	          MOVT      A0.3,#0x1020
.*:	03 83 1a ac 	          JUMP      A0.3,#0x5060
.* <__start>:
.*:	d4 ff ff ab 	          CALLR     D1RtP,.* <__start-0x8>
	\.\.\.
.* <_far>:
.*:	fe ff ff a0 	          NOP
