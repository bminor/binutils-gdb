#objdump: -drw -mi8086
#name: i386 with .code16

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
 +[a-f0-9]+:	f3 66 a5             	rep movsl %ds:\(%si\),%es:\(%di\)
 +[a-f0-9]+:	f3 66 a7             	repz cmpsl %es:\(%di\),%ds:\(%si\)
 +[a-f0-9]+:	66 f3 a5             	rep movsl %ds:\(%si\),%es:\(%di\)
 +[a-f0-9]+:	66 f3 a7             	repz cmpsl %es:\(%di\),%ds:\(%si\)
 +[a-f0-9]+:	66 f3 a5             	rep movsl %ds:\(%si\),%es:\(%di\)
 +[a-f0-9]+:	66 f3 a7             	repz cmpsl %es:\(%di\),%ds:\(%si\)
#pass
