#source: pr27491-1.s
#as: --64
#ld: --gc-sections -melf_x86_64 -z start-stop-gc -shared
#objdump: -dw

.*: +file format elf64-x86-64


Disassembly of section .text:

[a-f0-9]+ <foo>:
 +[a-f0-9]+:	48 8b 05 ([0-9a-f]{2} ){4}[ \t]+mov +0x[a-f0-9]+\(%rip\),%rax[ \t]+# [a-f0-9]+ <_DYNAMIC\+0x[a-f0-9]+>
 +[a-f0-9]+:	48 8b 05 ([0-9a-f]{2} ){4}[ \t]+mov +0x[a-f0-9]+\(%rip\),%rax[ \t]+# [a-f0-9]+ <_DYNAMIC\+0x[a-f0-9]+>
#pass
