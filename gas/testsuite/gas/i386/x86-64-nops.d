#objdump: -drw
#name: x86-64 nops

.*: +file format .*

Disassembly of section .text:

0+000 <.text>:
[	 ]*0:[	 ]+0f 1f 00[	 ]+nopl[ 	]+\(%rax\)
[	 ]*3:[	 ]+0f 1f 40 00[	 ]+nopl[ 	]+0x0\(%rax\)
[	 ]*7:[	 ]+0f 1f 44 00 00[	 ]+nopl[ 	]+0x0\(%rax,%rax,1\)
[	 ]*c:[	 ]+66 0f 1f 44 00 00[	 ]+nopw[ 	]+0x0\(%rax,%rax,1\)
[	 ]*12:[	 ]+0f 1f 80 00 00 00 00[	 ]+nopl[ 	]+0x0\(%rax\)
[	 ]*19:[	 ]+0f 1f 84 00 00 00 00 00[	 ]+nopl[ 	]+0x0\(%rax,%rax,1\)
[	 ]*21:[	 ]+66 0f 1f 84 00 00 00 00 00[	 ]+nopw[ 	]+0x0\(%rax,%rax,1\)
[	 ]*2a:[	 ]+66 2e 0f 1f 84 00 00 00 00 00[	 ]+nopw[ 	]+%cs:0x0\(%rax,%rax,1\)
#pass
