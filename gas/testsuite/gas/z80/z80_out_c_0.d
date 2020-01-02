#as: --with-inst=out-c-0
#objdump: -d
#name: Z80 undocumented instruction OUT (C),0

.*:.*

Disassembly of section .text:

0+ <.text>:
\s+0:\s+ed 71\s+out \(c\),0
