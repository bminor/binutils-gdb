# Check crc32 in SSE4.2

	.text
foo:

crc32b (%esi), %eax
crc32w (%esi), %eax
crc32l (%esi), %eax
crc32 (%esi), %eax
crc32  %al, %eax
crc32b  %al, %eax
crc32  %ax, %eax
crc32w  %ax, %eax
crc32  %eax, %eax
crc32l  %eax, %eax

.intel_syntax noprefix
crc32b  eax,byte ptr [esi]
crc32  eax,byte ptr [esi]
crc32w  eax, word ptr [esi]
crc32  eax, word ptr [esi]
crc32d  eax,dword ptr [esi]
crc32  eax,dword ptr [esi]
crc32  eax,al
crc32b  eax,al
crc32  eax, ax
crc32w  eax, ax
crc32  eax,eax
crc32d  eax,eax

.p2align 4,0
