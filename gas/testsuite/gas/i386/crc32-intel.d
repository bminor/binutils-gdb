#objdump: -dwMintel
#name: i386 crc32 (Intel disassembly)
#source: crc32.s

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	f2 0f 38 f0 06       	crc32b eax,BYTE PTR \[esi\]
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 06    	crc32w eax,WORD PTR \[esi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[esi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[esi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
[ 	]*[a-f0-9]+:	f2 0f 38 f0 06       	crc32b eax,BYTE PTR \[esi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f0 06       	crc32b eax,BYTE PTR \[esi\]
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 06    	crc32w eax,WORD PTR \[esi\]
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 06    	crc32w eax,WORD PTR \[esi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[esi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[esi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
#pass
