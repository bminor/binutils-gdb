#objdump: -drw
#name: i386 quoted symbols

.*: +file format .*

Disassembly of section .text:

0+ <quoted>:
[ 	]*[a-f0-9]+:[ 	]*a1 00 00 00 00[	 ]+mov    0x0,%eax	[a-f0-9]+: (R_386_|dir)?32	x\(y\)
[ 	]*[a-f0-9]+:[ 	]*8b 80 00 00 00 00[	 ]+mov    0x0\(%eax\),%eax	[a-f0-9]+: (R_386_|dir)?32	x\(y\)
[ 	]*[a-f0-9]+:[ 	]*26 a1 00 00 00 00[	 ]+mov    %es:0x0,%eax	[a-f0-9]+: (R_386_|dir)?32	x\(y\)
[ 	]*[a-f0-9]+:[ 	]*26 8b 80 00 00 00 00[	 ]+mov    %es:0x0\(%eax\),%eax	[a-f0-9]+: (R_386_|dir)?32	x\(y\)
[ 	]*[a-f0-9]+:[ 	]*a1 00 00 00 00[	 ]+mov    0x0,%eax	[a-f0-9]+: (R_386_|dir)?32	x\(y
[ 	]*[a-f0-9]+:[ 	]*a1 00 00 00 00[	 ]+mov    0x0,%eax	[a-f0-9]+: (R_386_|dir)?32	x\)y
[ 	]*[a-f0-9]+:[ 	]*a1 00 00 00 00[	 ]+mov    0x0,%eax	[a-f0-9]+: (R_386_|dir)?32	x\?y
[ 	]*[a-f0-9]+:[ 	]*a1 00 00 00 00[	 ]+mov    0x0,%eax	[a-f0-9]+: (R_386_|dir)?32	x\{y
[ 	]*[a-f0-9]+:[ 	]*a1 00 00 00 00[	 ]+mov    0x0,%eax	[a-f0-9]+: (R_386_|dir)?32	x\{z\}
[ 	]*[a-f0-9]+:[ 	]*ff 15 00 00 00 00[	 ]+call   \*0x0	[a-f0-9]+: (R_386_|dir)?32	x\(y\)
[ 	]*[a-f0-9]+:[ 	]*26 ff 15 00 00 00 00[	 ]+call   \*%es:0x0	[a-f0-9]+: (R_386_|dir)?32	x\(y\)
[ 	]*[a-f0-9]+:[ 	]*26 ff 15 00 00 00 00[	 ]+call   \*%es:0x0	[a-f0-9]+: (R_386_|dir)?32	x\(y\)
#pass
