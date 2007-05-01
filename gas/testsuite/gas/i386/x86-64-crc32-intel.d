#objdump: -drwMintel
#name: x86-64 crc32 (Intel mode)
#source: x86-64-crc32.s

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	f2 0f 38 f0 06       	crc32b eax,BYTE PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 48 0f 38 f0 06    	crc32b rax,BYTE PTR \[rsi\]
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 06    	crc32w eax,WORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 48 0f 38 f1 06    	crc32q rax,QWORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	f2 48 0f 38 f0 c0    	crc32b rax,al
[ 	]*[a-f0-9]+:	f2 48 0f 38 f0 c0    	crc32b rax,al
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
[ 	]*[a-f0-9]+:	f2 48 0f 38 f1 c0    	crc32q rax,rax
[ 	]*[a-f0-9]+:	f2 48 0f 38 f1 c0    	crc32q rax,rax
[ 	]*[a-f0-9]+:	f2 48 0f 38 f0 06    	crc32b rax,BYTE PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 48 0f 38 f0 06    	crc32b rax,BYTE PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f0 06       	crc32b eax,BYTE PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f0 06       	crc32b eax,BYTE PTR \[rsi\]
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 06    	crc32w eax,WORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 06    	crc32w eax,WORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f1 06       	crc32d eax,DWORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 48 0f 38 f1 06    	crc32q rax,QWORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 48 0f 38 f1 06    	crc32q rax,QWORD PTR \[rsi\]
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	f2 0f 38 f0 c0       	crc32b eax,al
[ 	]*[a-f0-9]+:	f2 48 0f 38 f0 c0    	crc32b rax,al
[ 	]*[a-f0-9]+:	f2 48 0f 38 f0 c0    	crc32b rax,al
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	66 f2 0f 38 f1 c0    	crc32w eax,ax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
[ 	]*[a-f0-9]+:	f2 0f 38 f1 c0       	crc32d eax,eax
[ 	]*[a-f0-9]+:	f2 48 0f 38 f1 c0    	crc32q rax,rax
[ 	]*[a-f0-9]+:	f2 48 0f 38 f1 c0    	crc32q rax,rax
#pass
