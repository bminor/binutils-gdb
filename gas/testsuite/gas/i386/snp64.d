#as: --defsym __amd64__=1
#objdump: -dw
#name: 64-bit SNP insn
#source: snp.s

.*: +file format .*


Disassembly of section \.text:

0+000 <att64>:
[ 	]*[a-f0-9]+:[ 	]+f3 0f 01 ff[ 	]+psmash[ 	]*
[ 	]*[a-f0-9]+:[ 	]+f3 0f 01 ff[ 	]+psmash[ 	]*
[ 	]*[a-f0-9]+:[ 	]+67 f3 0f 01 ff[ 	]+addr32 psmash[ 	]*
[ 	]*[a-f0-9]+:[ 	]+f2 0f 01 ff[ 	]+pvalidate[ 	]*
[ 	]*[a-f0-9]+:[ 	]+67 f2 0f 01 ff[ 	]+addr32 pvalidate[ 	]*
[ 	]*[a-f0-9]+:[ 	]+f2 0f 01 fe[ 	]+rmpupdate[ 	]*
[ 	]*[a-f0-9]+:[ 	]+f2 0f 01 fe[ 	]+rmpupdate[ 	]*
[ 	]*[a-f0-9]+:[ 	]+67 f2 0f 01 fe[ 	]+addr32 rmpupdate[ 	]*
[ 	]*[a-f0-9]+:[ 	]+f3 0f 01 fe[ 	]+rmpadjust[ 	]*
[ 	]*[a-f0-9]+:[ 	]+f3 0f 01 fe[ 	]+rmpadjust[ 	]*
[ 	]*[a-f0-9]+:[ 	]+67 f3 0f 01 fe[ 	]+addr32 rmpadjust[ 	]*
#pass
