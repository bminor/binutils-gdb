/* GDB legitimately expects a file name.  */
	.stabs "wierd.c",0x64,0,0,0

#define N_LSYM 0x80
#define N_GSYM 0x20
#define N_BCOMM 0xe2
#define N_ECOMM 0xe4

#define VAR(name) \
	.globl name; \
.data; \
	.align 2; \
name:; \
	.long 42

#define STAB(string,type,value) .stabs string,type,0,0,value
#include "wierd.def"
