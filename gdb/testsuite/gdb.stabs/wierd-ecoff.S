/* GDB legitimately expects a file name.  */
 .file	1 "wierd.c"
 @stabs
 .stabs "wierd.c",0x64,0,0,0

#define N_LSYM 0x80
#define N_GSYM 0x20

#define N_BCOMM 0xe2
#define N_ECOMM 0xe4
#define BEGIN_COMMON(name) .stabs name,N_BCOMM,0,0,0
#define END_COMMON(name) .stabs name,N_ECOMM,0,0,0

#define VAR(name) \
	.globl name; \
.data; \
	.align 2; \
name:; \
	.word 42

#define STAB(string,type,value) .stabs string,type,0,0,value
#include "wierd.def"

/* Stuff with backslashes needs to go here, since gcc with stabs treats
   them differently.  */

STAB("sym92:\ !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~",N_LSYM,0)
STAB("type92:t92=\ !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~",N_LSYM,0)
STAB("attr92:G392=@\ !#$%&'()*+,-./0123456789:<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~;1",N_GSYM, 0)
