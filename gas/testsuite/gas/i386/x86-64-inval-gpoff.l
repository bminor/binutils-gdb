.*: Assembler messages:
.*:3: Error: invalid GPOFF relocation
.*:3: Error: non-pc-relative relocation for pc-relative field
.*:4: Error: invalid GPOFF relocation
.*:5: Error: invalid GPOFF relocation
.*:6: Error: invalid GPOFF relocation
.*:6: Error: non-pc-relative relocation for pc-relative field
.*:7: Error: invalid GPOFF relocation
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+_start:
[ 	]*3[ 	]+\?\?\?\? 648B0500 		movl	%fs:foo@GPOFF\(%rip\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
\*\*\*\*  Error: non-pc-relative relocation for pc-relative field
[ 	]*3[ 	]+000000
[ 	]*4[ 	]+\?\?\?\? 648B8000 		movl	%fs:foo@GPOFF\(%rax\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
[ 	]*4[ 	]+000000
[ 	]*5[ 	]+\?\?\?\? 8B844800 		movl	%ds:foo@GPOFF\(%rax, %rcx, 2\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
[ 	]*5[ 	]+000000
[ 	]*6[ 	]+\?\?\?\? 8B050000 		movl	foo@GPOFF\(%rip\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
\*\*\*\*  Error: non-pc-relative relocation for pc-relative field
[ 	]*6[ 	]+0000
[ 	]*7[ 	]+\?\?\?\? 8B800000 		movl	foo@GPOFF\(%rax\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
[ 	]*7[ 	]+0000
