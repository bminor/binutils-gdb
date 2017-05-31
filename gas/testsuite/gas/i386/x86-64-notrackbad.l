.*: Assembler messages:
.*:6: Error: expecting register indirect branch instruction after `notrack'
.*:6: Warning: skipping prefixes on this instruction
.*:7: Error: expecting register indirect branch instruction after `notrack'
.*:7: Warning: skipping prefixes on this instruction
.*:9: Error: expecting register indirect branch instruction after `notrack'
.*:10: Error: expecting register indirect branch instruction after `notrack'
.*:11: Error: same type of prefix used twice
.*:12: Error: expecting no other prefixes after `notrack'
.*:13: Error: expecting no other prefixes after `notrack'
.*:14: Error: expecting no other prefixes after `notrack'
.*:17: Error: expecting register indirect branch instruction after `notrack'
.*:18: Error: expecting register indirect branch instruction after `notrack'
.*:19: Error: same type of prefix used twice
.*:20: Error: expecting no other prefixes after `notrack'
.*:21: Error: expecting no other prefixes after `notrack'
.*:22: Error: expecting no other prefixes after `notrack'
GAS LISTING .*


[ 	]*1[ 	]+\# Check 64bit unsupported NOTRACK prefix
[ 	]*2[ 	]+
[ 	]*3[ 	]+\.allow_index_reg
[ 	]*4[ 	]+\.text
[ 	]*5[ 	]+_start:
[ 	]*6[ 	]+\?\?\?\? [0-9A-F]* 		notrack call foo
\*\*\*\*  Error: expecting register indirect branch instruction after `notrack'
\*\*\*\*  Warning: skipping prefixes on this instruction
[ 	]*6[ 	]+[0-9A-F]*
[ 	]*7[ 	]+\?\?\?\? [0-9A-F]* 		notrack jmp foo
\*\*\*\*  Error: expecting register indirect branch instruction after `notrack'
\*\*\*\*  Warning: skipping prefixes on this instruction
[ 	]*7[ 	]+[0-9A-F]*
[ 	]*8[ 	]+
[ 	]*9[ 	]+\?\?\?\? 3EFF10   		notrack call \*\(%rax\)
\*\*\*\*  Error: expecting register indirect branch instruction after `notrack'
[ 	]*10[ 	]+\?\?\?\? 3EFF20   		notrack jmp \*\(%rax\)
\*\*\*\*  Error: expecting register indirect branch instruction after `notrack'
[ 	]*11[ 	]+fs notrack call \*%rax
[ 	]*12[ 	]+notrack fs call \*%rax
[ 	]*13[ 	]+notrack bnd call \*%rax
[ 	]*14[ 	]+notrack data16 call \*%rax
[ 	]*15[ 	]+
[ 	]*16[ 	]+\.intel_syntax noprefix
[ 	]*17[ 	]+\?\?\?\? 3EFF10   		notrack call \[rax\]
\*\*\*\*  Error: expecting register indirect branch instruction after `notrack'
[ 	]*18[ 	]+\?\?\?\? 3EFF20   		notrack jmp \[rax\]
\*\*\*\*  Error: expecting register indirect branch instruction after `notrack'
[ 	]*19[ 	]+fs notrack call rax
[ 	]*20[ 	]+notrack fs call rax
[ 	]*21[ 	]+notrack bnd call rax
[ 	]*22.*[ 	]+notrack data16 call rax
#pass
