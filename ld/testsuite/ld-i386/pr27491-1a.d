#source: pr27491-1.s
#as: --32
#ld: --gc-sections -melf_i386 -z start-stop-gc -shared
#objdump: -dw

.*: +file format elf32-i386


Disassembly of section .text:

[a-f0-9]+ <foo>:
 +[a-f0-9]+:	8b 83 ([0-9a-f]{2} ){4}[ \t]+mov +-0x[a-f0-9]+\(%ebx\),%eax
 +[a-f0-9]+:	8b 83 ([0-9a-f]{2} ){4}[ \t]+mov +-0x[a-f0-9]+\(%ebx\),%eax
#pass
