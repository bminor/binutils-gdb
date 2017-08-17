.*: Assembler messages:
.*:3: Error: .*
.*:4: Error: .*
.*:5: Error: .*
.*:6: Error: .*
.*:9: Error: .*
.*:10: Error: .*
.*:11: Error: .*
.*:12: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+\# All the following should be illegal
[ 	]*3[ 	]+movq	%ds,\(%rax\)
[ 	]*4[ 	]+movl	%ds,\(%rax\)
[ 	]*5[ 	]+movq	\(%rax\),%ds
[ 	]*6[ 	]+movl	\(%rax\),%ds
[ 	]*7[ 	]+
[ 	]*8[ 	]+\.intel_syntax noprefix
[ 	]*9[ 	]+mov	eax, DWORD PTR fs:foobar:16
[ 	]*10[ 	]+mov	eax, DWORD PTR fs:foobar:barfoo:16
[ 	]*11[ 	]+mov	eax, DWORD PTR fs:ds:16
[ 	]*12[ 	]+mov	eax, DWORD PTR fs:ds:cs:16
