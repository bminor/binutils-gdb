/* GDB legitimately expects a file name.  */
/* The sun3 assembler bogusly requires that the value of this stab be a
   label.  Placate it.  */
	.stabs "wierd.c",0x64,0,0,Label0
Label0:

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

/* Stuff with backslashes needs to go here, since the aix assembler treats
   them differently.  */

STAB("sym92:\\ !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~",N_LSYM,0)
STAB("type92:t92=\\ !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~",N_LSYM,0)
STAB("attr92:G392=@\\ !#$%&'()*+,-./0123456789:<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~;1",N_GSYM, 0)
