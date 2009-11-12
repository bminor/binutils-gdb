#objdump: -dw
#name: x86-64 lockable insns

.*: +file format .*


Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	f0 03 03             	lock add \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 03 64          	lock addl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 13 03             	lock adc \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 13 64          	lock adcl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 23 03             	lock and \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 23 64          	lock andl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f bb 03          	lock btc %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ba 3b 64       	lock btcl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f b3 03          	lock btr %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ba 33 64       	lock btrl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ab 03          	lock bts %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ba 2b 64       	lock btsl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f b1 03          	lock cmpxchg %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f c7 0b          	lock cmpxchg8b \(%rbx\)
[ 	]*[a-f0-9]+:	f0 ff 0b             	lock decl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 ff 03             	lock incl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 f7 1b             	lock negl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 f7 13             	lock notl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 0b 03             	lock or \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 0b 64          	lock orl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 1b 03             	lock sbb \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 1b 64          	lock sbbl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 2b 03             	lock sub \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 2b 64          	lock subl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f c1 03          	lock xadd %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 87 03             	lock xchg %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 87 03             	lock xchg %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 33 03             	lock xor \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 33 64          	lock xorl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 03 03             	lock add \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 03 64          	lock addl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 13 03             	lock adc \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 13 64          	lock adcl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 23 03             	lock and \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 23 64          	lock andl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f bb 03          	lock btc %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ba 3b 64       	lock btcl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f b3 03          	lock btr %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ba 33 64       	lock btrl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ab 03          	lock bts %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f ba 2b 64       	lock btsl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f b1 03          	lock cmpxchg %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f c7 0b          	lock cmpxchg8b \(%rbx\)
[ 	]*[a-f0-9]+:	f0 ff 0b             	lock decl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 ff 03             	lock incl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 f7 1b             	lock negl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 f7 13             	lock notl \(%rbx\)
[ 	]*[a-f0-9]+:	f0 0b 03             	lock or \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 0b 64          	lock orl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 1b 03             	lock sbb \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 1b 64          	lock sbbl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 2b 03             	lock sub \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 2b 64          	lock subl \$0x64,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 0f c1 03          	lock xadd %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 87 03             	lock xchg %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 87 03             	lock xchg %eax,\(%rbx\)
[ 	]*[a-f0-9]+:	f0 33 03             	lock xor \(%rbx\),%eax
[ 	]*[a-f0-9]+:	f0 83 33 64          	lock xorl \$0x64,\(%rbx\)
#pass
