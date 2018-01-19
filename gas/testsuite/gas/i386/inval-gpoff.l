.*: Assembler messages:
.*:3: Error: invalid GPOFF relocation
.*:4: Error: invalid GPOFF relocation
.*:5: Error: invalid GPOFF relocation
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+_start:
[ 	]*3[ 	]+\?\?\?\? 648B8000 		movl	%fs:foo@GPOFF\(%eax\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
[ 	]*3[ 	]+000000
[ 	]*4[ 	]+\?\?\?\? 8B844800 		movl	%ds:foo@GPOFF\(%eax, %ecx, 2\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
[ 	]*4[ 	]+000000
[ 	]*5[ 	]+\?\?\?\? 8B800000 		movl	foo@GPOFF\(%eax\), %eax
\*\*\*\*  Error: invalid GPOFF relocation
[ 	]*5[ 	]+0000
