#objdump: -d
#name: rep prefix on ret

.*: +file format .*

Disassembly of section .text:

0+000 <foo>:
\s*[0-9a-f]+:\s+f3 c3\s+repz ret\s*
