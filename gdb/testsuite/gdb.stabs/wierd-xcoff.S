#define N_LSYM 0x81
#define N_GSYM 0x80

#define VAR(name) \
	.globl name; \
	.csect .data[RW]; \
	.align 1; \
name:; \
	.long 42; \
	.csect [PR]
#define STAB(string,type,value) .stabx string,value,type
#include "wierd.def"
