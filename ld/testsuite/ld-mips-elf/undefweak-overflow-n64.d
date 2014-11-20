#name: undefined weak symbol overflow (n64)
#source: undefweak-overflow.s
#as: -64 -EB
#ld: -melf64btsmip -Ttext=0x20000000 -e start
#objdump: -dr
#...
    0*20000000:	d85fffff.*
    0*20000004:	00000000.*
    0*20000008:	f85ffffd.*
    0*2000000c:	ec4ffffd.*
    0*20000010:	ec5bfffe.*
    0*20000014:	cbfffffa.*
    0*20000018:	3c04e000.*
    0*2000001c:	1000fff8.*
    0*20000020:	2484ffe0.*
    0*20000024:	0411fff6.*
    0*20000028:	00000000.*
    0*2000002c:	3c047fd0.*
    0*20000030:	8e670c00.*
    0*20000034:	cfe50c00.*
    0*20000038:	9400ffe2.*
    0*2000003c:	0c000c00.*
#pass
