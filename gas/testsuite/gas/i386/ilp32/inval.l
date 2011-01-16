.*: Assembler messages:
.*:3: Error: .*
.*:4: Error: .*
.*:5: Error: .*
.*:6: Error: .*
.*:10: Error: .*
GAS LISTING .*


[ 	]*1[ 	]+\.text
[ 	]*2[ 	]+\# All the following should be illegal for x32
[ 	]*3[ 	]+\?\?\?\? 48A10000 		movabs xxx,%rax
[ 	]*3[ 	]+00000000 
[ 	]*3[ 	]+0000
[ 	]*4[ 	]+\?\?\?\? 48A10000 		movabs foo,%rax
[ 	]*4[ 	]+00000000 
[ 	]*4[ 	]+0000
[ 	]*5[ 	]+\?\?\?\? 48A10000 		movabsq xxx,%rax
[ 	]*5[ 	]+00000000 
[ 	]*5[ 	]+0000
[ 	]*6[ 	]+\?\?\?\? 48A10000 		movabsq foo,%rax
[ 	]*6[ 	]+00000000 
[ 	]*6[ 	]+0000
[ 	]*7[ 	]+
[ 	]*8[ 	]+\.data
[ 	]*9[ 	]+xxx:
[ 	]*10[ 	]+\?\?\?\? 00000000 		\.quad foo
\*\*\*\*  Error:cannot represent relocation type BFD_RELOC_[ 	]*64[ 	]+in x32 mode
[ 	]*10[ 	]+00000000 
