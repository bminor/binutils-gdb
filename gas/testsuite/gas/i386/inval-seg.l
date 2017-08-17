.*: Assembler messages:
.*:3: Error: .*
.*:4: Error: .*
.*:7: Error: .*
.*:8: Error: .*
.*:9: Error: .*
.*:10: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+\# All the following should be illegal
[ 	]*3[ 	]+movl	%ds,\(%eax\)
[ 	]*4[ 	]+movl	\(%eax\),%ds
[ 	]*5[ 	]+
[ 	]*6[ 	]+\.intel_syntax noprefix
[ 	]*7[ 	]+mov	eax, DWORD PTR fs:foobar:16
[ 	]*8[ 	]+mov	eax, DWORD PTR fs:foobar:barfoo:16
[ 	]*9[ 	]+mov	eax, DWORD PTR fs:ds:16
[ 	]*10[ 	]+mov	eax, DWORD PTR fs:ds:cs:16
