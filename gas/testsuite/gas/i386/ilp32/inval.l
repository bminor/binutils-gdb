.*: Assembler messages:
.*:3: Error: .*
.*:4: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+\# All the following should be illegal for x32
[ 	]*3[ 	]+\?\?\?\? 48A10000 		movabs xxx,%rax
\*\*\*\*  Error:'movabs' isn't supported in x[ 	]*32[ 	]+mode
[ 	]*3[ 	]+00000000 
[ 	]*3[ 	]+0000
[ 	]*4[ 	]+\?\?\?\? 48A10000 		movabs foo,%rax
\*\*\*\*  Error:'movabs' isn't supported in x[ 	]*32[ 	]+mode
[ 	]*4[ 	]+00000000 
[ 	]*4[ 	]+0000
[ 	]*5[ 	]+
[ 	]*6[ 	]+\.data
[ 	]*7[ 	]+xxx:
[ 	]*8[ 	]+\?\?\?\? 00       		\.byte 0
