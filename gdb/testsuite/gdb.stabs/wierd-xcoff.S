#define N_GSYM 128
#define N_LSYM 129
#define N_BCOMM 135
#define N_ECOMM 137

#define VAR(name) \
	.globl name; \
	.align 1; \
name:; \
	.long 42;
#define STAB(string,type,value) .stabx string,value,type,0

/* Put everything in this csect, which seems to make things work.
   The compiler actually puts the stabs in .csect [PR], but that didn't
   work here (I guess because there is no text section).  */
	.csect .data[RW]
#include "wierd.def"

/* Stuff with backslashes needs to go here, since the aix assembler treats
   them differently.  */

STAB("sym92:\ !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~",N_LSYM,0)
STAB("type92:t92=\ !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~",N_LSYM,0)
STAB("attr92:G392=@\ !#$%&'()*+,-./0123456789:<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~;1",N_GSYM, 0)
