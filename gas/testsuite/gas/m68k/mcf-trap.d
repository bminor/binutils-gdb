#name: mcf-trap
#objdump: -d
#as: -m5208

.*:     file format .*

Disassembly of section .text:

0+ <.text>:
[ 0-9a-f]+:	51fc           	tpf
[ 0-9a-f]+:	51fa 1234      	tpfw #4660
[ 0-9a-f]+:	51fb 1234 5678 	tpfl #305419896
[ 0-9a-f]+:	51fc           	tpf
[ 0-9a-f]+:	51fa 1234      	tpfw #4660
[ 0-9a-f]+:	51fb 1234 5678 	tpfl #305419896
