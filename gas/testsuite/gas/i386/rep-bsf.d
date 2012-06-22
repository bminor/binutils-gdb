#objdump: -d
#name: rep prefix on bsf/bsr

.*: +file format .*

Disassembly of section .text:

0+000 <bsf>:
\s*[0-9]+:\s+f3 0f bc c1\s+tzcnt  %ecx,%eax
0+004 <bsr>:
\s*[0-9]+:\s+f3 0f bd c1\s+lzcnt  %ecx,%eax
